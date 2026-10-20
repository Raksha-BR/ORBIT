
#include "core/Job.h"
#include "jobs/JobDeduplicator.h"
#include "jobs/JobSearchService.h"
#include "storage/postgres/PostgresConnection.h"
#include "storage/postgres/PostgresJobRepository.h"
#include "storage/postgres/PostgresSchema.h"
#include "jobs/JobSourceService.h"
#include "jobs/MockJobSource.h"
#include "jobs/RemotiveJobSource.h"
#include "jobs/JobRelevanceScorer.h"

#include <algorithm>
#include <cctype>
#include <fstream>
#include <iostream>
#include <map>
#include <optional>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace {

std::string trim(const std::string& value) {
    const auto first = value.find_first_not_of(" \t\r\n");

    if (first == std::string::npos) {
        return "";
    }

    const auto last = value.find_last_not_of(" \t\r\n");
    return value.substr(first, last - first + 1);
}

std::string lowercase(std::string value) {
    std::transform(
        value.begin(),
        value.end(),
        value.begin(),
        [](unsigned char ch) {
            return static_cast<char>(std::tolower(ch));
        }
    );

    return value;
}

std::vector<std::string> parse_csv_line(const std::string& line) {
    std::vector<std::string> fields;
    std::string field;
    bool inside_quotes = false;

    for (std::size_t i = 0; i < line.size(); ++i) {
        const char ch = line[i];

        if (ch == '"') {
            if (inside_quotes &&
                i + 1 < line.size() &&
                line[i + 1] == '"') {
                field += '"';
                ++i;
            } else {
                inside_quotes = !inside_quotes;
            }
        } else if (ch == ',' && !inside_quotes) {
            fields.push_back(field);
            field.clear();
        } else {
            field += ch;
        }
    }

    if (inside_quotes) {
        throw std::runtime_error(
            "CSV row contains an unclosed quoted field"
        );
    }

    fields.push_back(field);
    return fields;
}

std::vector<std::string> split_skills(const std::string& value) {
    std::vector<std::string> skills;
    std::istringstream stream(value);
    std::string skill;

    while (std::getline(stream, skill, '|')) {
        skill = trim(skill);

        if (!skill.empty()) {
            skills.push_back(skill);
        }
    }

    return skills;
}

std::optional<int> parse_experience(const std::string& value) {
    const std::string cleaned = trim(value);

    if (cleaned.empty()) {
        return std::nullopt;
    }

    std::size_t parsed = 0;
    const int years = std::stoi(cleaned, &parsed);

    if (parsed != cleaned.size() || years < 0) {
        throw std::runtime_error(
            "Minimum experience must be a non-negative integer"
        );
    }

    return years;
}

void print_usage() {
    std::cout
        << "ORBIT - Job Search CLI\n\n"
        << "Usage:\n"
        << "  orbit --help\n"
        << "  orbit jobs list\n"
        << "  orbit jobs search --keyword <keyword>\n"
        << "  orbit jobs show --id <job_id>\n"
        << "  orbit jobs import --file <csv_file>\n\n"
        << "CSV columns:\n"
        << "  title,company,location,salary,"
        << "minimum_years_experience,skills,description,"
        << "application_url,source_name,external_job_id,original_url\n"
        << "Use | between skills, for example: C++|Python|SQL\n"
        << "  orbit jobs collect --source mock\n"
        << "  orbit jobs collect --source remotive\n"
        << "  orbit jobs rank\n";
}

void print_job(const orbit::Job& job) {
    std::cout
        << "ID: " << job.id << '\n'
        << "Title: " << job.title << '\n'
        << "Company: " << job.company << '\n'
        << "Location: " << job.location << '\n'
        << "Salary: "
        << (job.salary.has_value() ? *job.salary : "Not specified")
        << '\n'
        << "Description: " << job.description << '\n'
        << "Application URL: " << job.application_url << '\n'
        << "Source: " << job.source_name << '\n'
        << "External Job ID: " << job.external_job_id << '\n'
        << "Original URL: " << job.original_url << '\n'
        << "Skills: ";

    for (std::size_t i = 0; i < job.skills.size(); ++i) {
        if (i > 0) {
            std::cout << ", ";
        }

        std::cout << job.skills[i];
    }

    std::cout << "\n\n";
}

std::size_t import_jobs(
    const std::string& file_path,
    orbit::PostgresJobRepository& repository,
    orbit::JobSearchService& search_service
) {
    std::ifstream file(file_path);

    if (!file.is_open()) {
        throw std::runtime_error(
            "Cannot open CSV file: " + file_path
        );
    }

    std::string line;

    if (!std::getline(file, line)) {
        throw std::runtime_error("CSV file is empty");
    }

    if (line.size() >= 3 &&
        static_cast<unsigned char>(line[0]) == 0xEF &&
        static_cast<unsigned char>(line[1]) == 0xBB &&
        static_cast<unsigned char>(line[2]) == 0xBF) {
        line.erase(0, 3);
    }

    const auto header_fields = parse_csv_line(line);
    std::map<std::string, std::size_t> columns;

    for (std::size_t i = 0; i < header_fields.size(); ++i) {
        const std::string name = lowercase(trim(header_fields[i]));

        if (!columns.emplace(name, i).second) {
            throw std::runtime_error(
                "Duplicate CSV column: " + name
            );
        }
    }

    const std::vector<std::string> required_columns = {
        "title",
        "company",
        "location",
        "salary",
        "minimum_years_experience",
        "skills",
        "description",
        "application_url",
        "source_name",
        "external_job_id",
        "original_url"
    };

    for (const auto& column : required_columns) {
        if (columns.find(column) == columns.end()) {
            throw std::runtime_error(
                "Missing CSV column: " + column
            );
        }
    }

    auto existing_jobs = repository.find_all();
    orbit::JobId next_id = 1;

    for (const auto& job : existing_jobs) {
        if (job.id >= next_id) {
            next_id = job.id + 1;
        }
    }

    std::vector<orbit::Job> incoming_jobs;
    std::size_t line_number = 1;

    while (std::getline(file, line)) {
        ++line_number;

        if (trim(line).empty()) {
            continue;
        }

        try {
            const auto fields = parse_csv_line(line);

            if (fields.size() != header_fields.size()) {
                throw std::runtime_error(
                    "Expected " +
                    std::to_string(header_fields.size()) +
                    " fields, received " +
                    std::to_string(fields.size())
                );
            }

            const auto get = [&](const std::string& name)
                -> std::string {
                return trim(fields.at(columns.at(name)));
            };

            orbit::Job job;
            job.id = next_id++;
            job.title = get("title");
            job.company = get("company");
            job.location = get("location");

            const std::string salary = get("salary");
            if (!salary.empty()) {
                job.salary = salary;
            }

            job.minimum_years_experience =
                parse_experience(get("minimum_years_experience"));

            job.skills = split_skills(get("skills"));
            job.description = get("description");
            job.application_url = get("application_url");
            job.source_name = get("source_name");
            job.external_job_id = get("external_job_id");
            job.original_url = get("original_url");

            if (job.title.empty() || job.company.empty()) {
                throw std::runtime_error(
                    "Title and company must not be empty"
                );
            }

            incoming_jobs.push_back(std::move(job));
        } catch (const std::exception& error) {
            throw std::runtime_error(
                "CSV line " + std::to_string(line_number) +
                ": " + error.what()
            );
        }
    }

    const auto saved_jobs =
        search_service.save_jobs(std::move(incoming_jobs));

    return saved_jobs.size();
}

} // namespace

int main(int argc, char* argv[]) {
    if (argc == 1 ||
        std::string(argv[1]) == "--help" ||
        std::string(argv[1]) == "-h") {
        print_usage();
        return 0;
    }

    try {
        orbit::PostgresConnection connection(
            "host=localhost port=5432 dbname=orbit "
            "user=orbit_user password=orbit_dev_password"
        );

        orbit::PostgresSchema::initialize(connection);
        orbit::PostgresJobRepository repository(connection);
        orbit::JobDeduplicator deduplicator;
        orbit::JobSearchService search_service(
            repository,
            deduplicator
        );
        orbit::JobSourceService source_service(search_service);

        const std::string command = argv[1];

        if (command != "jobs" || argc < 3) {
            print_usage();
            return 1;
        }

        const std::string action = argv[2];

        if (action == "list") {
            const auto jobs = search_service.get_all_jobs();

            if (jobs.empty()) {
                std::cout << "No jobs found.\n";
                return 0;
            }

            for (const auto& job : jobs) {
                print_job(job);
            }

            return 0;
        }

        
        
        

        if (action == "rank") {
            std::size_t top = 0;
            int min_score = 0;
            bool sde_only = false;

            for (int i = 3; i < argc; ++i) {
                const std::string arg = argv[i];

                if (arg == "--sde-only") {
                    sde_only = true;
                } else if (arg == "--top" && i + 1 < argc) {
                    const std::string value = argv[++i];
                    std::size_t parsed = 0;
                    const auto number = std::stoull(value, &parsed);

                    if (parsed != value.size() || number == 0) {
                        std::cerr << "Error: --top must be a positive integer\n";
                        return 1;
                    }

                    top = static_cast<std::size_t>(number);
                } else if (arg == "--min-score" && i + 1 < argc) {
                    const std::string value = argv[++i];
                    std::size_t parsed = 0;
                    const int number = std::stoi(value, &parsed);

                    if (parsed != value.size() ||
                        number < 0 || number > 100) {
                        std::cerr << "Error: --min-score must be from 0 to 100\n";
                        return 1;
                    }

                    min_score = number;
                } else {
                    std::cerr << "Error: unknown or incomplete rank option: "
                              << arg << '\n';
                    std::cerr << "Usage: orbit jobs rank "
                              << "[--top N] [--min-score N] [--sde-only]\n";
                    return 1;
                }
            }

            const auto jobs = search_service.get_all_jobs();

            if (jobs.empty()) {
                std::cout << "No jobs found. Collect or import jobs first.\n";
                return 0;
            }

            
            const auto title_contains_any =
                [](const std::string& title,
                   const std::vector<std::string>& keywords) {
                    for (const auto& keyword : keywords) {
                        if (title.find(keyword) != std::string::npos) {
                            return true;
                        }
                    }
                    return false;
                };

            const auto is_sde_role = [&title_contains_any](
    const std::string& raw_title) {
                const std::string title = lowercase(raw_title);

                // Exclude roles outside the target experience level.
                if (title_contains_any(title, {
                        "senior", "sr.", "sr ",
                        "staff engineer", "principal engineer",
                        "lead engineer", "tech lead",
                        "engineering manager", "director",
                        "vice president", "vp of engineering",
                        "architect",
                        "service desk", "help desk",
                        "technical support", "desktop support",
                        "sales", "recruiter", "writer",
                        "marketing", "office assistant",
                        "content reviewer"
                    })) {
                    return false;
                }

                return title_contains_any(title, {
                    "software engineer",
                    "software developer",
                    "backend engineer",
                    "back-end engineer",
                    "backend developer",
                    "back-end developer",
                    "full-stack developer",
                    "full stack developer",
                    "software development engineer",
                    "platform engineer",
                    "systems engineer",
                    "c++ engineer",
                    "python engineer",
                    "sde",
                    "programmer",
                    "developer"
                });
            };

            orbit::JobRelevanceScorer scorer;

            std::vector<std::pair<orbit::Job, orbit::JobRelevanceScore>>
                ranked_jobs;

            ranked_jobs.reserve(jobs.size());

            for (const auto& job : jobs) {
                if (sde_only && !is_sde_role(job.title)) {
                    continue;
                }

                auto relevance = scorer.score(job);

                if (relevance.score < min_score) {
                    continue;
                }

                ranked_jobs.emplace_back(job, std::move(relevance));
            }

            std::stable_sort(
                ranked_jobs.begin(),
                ranked_jobs.end(),
                [](const auto& left, const auto& right) {
                    return left.second.score > right.second.score;
                }
            );

            if (ranked_jobs.empty()) {
                std::cout << "No jobs matched the requested filters.\n";
                return 0;
            }

            if (top > 0 && ranked_jobs.size() > top) {
                ranked_jobs.resize(top);
            }

            std::cout << "ORBIT Job Relevance Ranking\n";
            std::cout << "===========================\n\n";

            for (std::size_t i = 0; i < ranked_jobs.size(); ++i) {
                const auto& job = ranked_jobs[i].first;
                const auto& relevance = ranked_jobs[i].second;

                std::cout
                    << (i + 1) << ". [" << relevance.score << "/100] "
                    << job.title << '\n'
                    << "   Company: "
                    << (job.company.empty() ? "Unknown" : job.company)
                    << '\n'
                    << "   Location: "
                    << (job.location.empty() ? "Unknown" : job.location)
                    << '\n'
                    << "   Salary: "
                    << (job.salary.has_value()
                            ? *job.salary : "Not specified")
                    << '\n'
                    << "   Experience: ";

                if (job.minimum_years_experience.has_value()) {
                    std::cout << *job.minimum_years_experience << " year(s)";
                } else {
                    std::cout << "Not specified";
                }

                std::cout << "\n   Reasons:\n";

                for (const auto& reason : relevance.reasons) {
                    std::cout << "     - " << reason << '\n';
                }

                if (!job.application_url.empty()) {
                    std::cout << "   Apply: " << job.application_url << '\n';
                } else if (!job.original_url.empty()) {
                    std::cout << "   Apply: " << job.original_url << '\n';
                }

                std::cout << '\n';
            }

            std::cout << "Showing " << ranked_jobs.size() << " job(s).\n";
            return 0;
        }



        if (action == "search") {
            std::string keyword;

            for (int i = 3; i + 1 < argc; ++i) {
                if (std::string(argv[i]) == "--keyword") {
                    keyword = argv[i + 1];
                    break;
                }
            }

            if (keyword.empty()) {
                std::cerr
                    << "Error: provide --keyword <keyword>\n";
                return 1;
            }

            const auto jobs =
                search_service.search_by_keyword(keyword);

            if (jobs.empty()) {
                std::cout << "No matching jobs found.\n";
                return 0;
            }

            for (const auto& job : jobs) {
                print_job(job);
            }

            return 0;
        }

        if (action == "show") {
            std::string id_text;

            for (int i = 3; i + 1 < argc; ++i) {
                if (std::string(argv[i]) == "--id") {
                    id_text = argv[i + 1];
                    break;
                }
            }

            if (id_text.empty()) {
                std::cerr << "Error: provide --id <job_id>\n";
                return 1;
            }

            std::size_t parsed = 0;
            const long long id = std::stoll(id_text, &parsed);

            if (parsed != id_text.size() || id < 0) {
                std::cerr << "Error: invalid job ID\n";
                return 1;
            }

            const auto job =
                repository.find(static_cast<orbit::JobId>(id));

            if (!job.has_value()) {
                std::cerr << "Job not found: " << id << '\n';
                return 1;
            }

            print_job(*job);
            return 0;
        }


        if (action == "import") {
            std::string file_path;

            for (int i = 3; i + 1 < argc; ++i) {
                if (std::string(argv[i]) == "--file") {
                    file_path = argv[i + 1];
                    break;
                }
            }

            if (file_path.empty()) {
                std::cerr
                    << "Error: provide --file <csv_file>\n";
                return 1;
            }

            const std::size_t imported = import_jobs(
                file_path,
                repository,
                search_service
            );

            std::cout
                << "Import complete. New jobs saved: "
                << imported << '\n';

            return 0;
        }

        if (action == "collect") {
            std::string source_name;

            for (int i = 3; i + 1 < argc; ++i) {
                if (std::string(argv[i]) == "--source") {
                    source_name = argv[i + 1];
                    break;
                }
            }

            if (source_name != "mock" && source_name != "remotive") {
                std::cerr
                    << "Error: supported sources are mock and remotive\n";
                return 1;
            }

            orbit::JobSourceResult result;

            if (source_name == "mock") {
                orbit::MockJobSource source;
                result = source_service.collect(source);
            } else {
                orbit::RemotiveJobSource source;
                result = source_service.collect(source);
            }

            if (!result.error.empty()) {
                std::cerr
                    << "Collection failed: "
                    << result.error << '\n';
                return 1;
            }

            std::cout
                << "Source: " << result.source_name << '\n'
                << "Fetched: " << result.fetched_count << '\n'
                << "New jobs saved: " << result.jobs.size() << '\n'
                << "Skipped as duplicates: "
                << result.rejected_count << '\n';

            return 0;
        }

        print_usage();
        return 1;
    } catch (const std::exception& error) {
        std::cerr << "ORBIT error: " << error.what() << '\n';
        return 1;
    }
}
