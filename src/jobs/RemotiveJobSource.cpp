#include "jobs/RemotiveJobSource.h"

#include <curl/curl.h>
#include <nlohmann/json.hpp>

#include <stdexcept>
#include <string>

namespace orbit {
namespace {

using json = nlohmann::json;

size_t write_response(
    char* data,
    size_t size,
    size_t count,
    void* destination
) {
    const size_t bytes = size * count;
    auto* response = static_cast<std::string*>(destination);
    response->append(data, bytes);
    return bytes;
}

std::string read_string(
    const json& item,
    const char* key
) {
    const auto found = item.find(key);

    if (found == item.end() || found->is_null()) {
        return "";
    }

    if (found->is_string()) {
        return found->get<std::string>();
    }

    return "";
}

} // namespace

std::string RemotiveJobSource::name() const {
    return "Remotive";
}

std::vector<Job> RemotiveJobSource::fetch_jobs() {
    static const bool curl_initialized = [] {
        if (curl_global_init(CURL_GLOBAL_DEFAULT) != CURLE_OK) {
            throw std::runtime_error(
                "Failed to initialize libcurl"
            );
        }
        return true;
    }();

    (void)curl_initialized;

    CURL* curl = curl_easy_init();

    if (curl == nullptr) {
        throw std::runtime_error(
            "Failed to create HTTP request"
        );
    }

    std::string response;

    curl_easy_setopt(
        curl,
        CURLOPT_URL,
        "https://remotive.com/api/remote-jobs?category=software-dev&limit=100"
    );
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_response);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 15L);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 45L);
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "ORBIT-JobCollector/1.0");

    const CURLcode result = curl_easy_perform(curl);

    long status_code = 0;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &status_code);
    curl_easy_cleanup(curl);

    if (result != CURLE_OK) {
        throw std::runtime_error(
            std::string("HTTP request failed: ") +
            curl_easy_strerror(result)
        );
    }

    if (status_code < 200 || status_code >= 300) {
        throw std::runtime_error(
            "Remotive API returned HTTP status " +
            std::to_string(status_code)
        );
    }

    const json document = json::parse(response);

    if (!document.contains("jobs") ||
        !document["jobs"].is_array()) {
        throw std::runtime_error(
            "Remotive API response has no jobs array"
        );
    }

    std::vector<Job> jobs;

    for (const auto& item : document["jobs"]) {
        if (!item.is_object() ||
            !item.contains("id") ||
            !item["id"].is_number_integer()) {
            continue;
        }

        Job job;
        job.title = read_string(item, "title");
        job.company = read_string(item, "company_name");
        job.location = read_string(
            item,
            "candidate_required_location"
        );
        job.description = read_string(item, "description");
        job.application_url = read_string(item, "url");
        job.original_url = job.application_url;
        job.source_name = name();
        job.external_job_id = std::to_string(
            item["id"].get<long long>()
        );

        const std::string salary = read_string(item, "salary");

        if (!salary.empty()) {
            job.salary = salary;
        }

        if (item.contains("tags") && item["tags"].is_array()) {
            for (const auto& tag : item["tags"]) {
                if (tag.is_string()) {
                    job.skills.push_back(tag.get<std::string>());
                }
            }
        }

        if (job.title.empty() ||
            job.company.empty() ||
            job.application_url.empty()) {
            continue;
        }

        jobs.push_back(std::move(job));
    }

    return jobs;
}

} // namespace orbit