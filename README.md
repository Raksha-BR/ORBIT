# ORBIT

**ORBIT** is an intelligent job-search and application engine that discovers, analyzes, matches, automates, and learns from job applications.

## Vision

ORBIT aims to reduce the repetitive work involved in modern job hunting by automating job discovery, job analysis, candidate matching, application preparation, application workflows, and application tracking.

## Core Lifecycle

Discover → Understand → Match → Prepare → Apply → Track → Learn

## Technology Stack

- **Languages:** C++, Python
- **Build System:** CMake
- **Database:** PostgreSQL
- **Testing:** GoogleTest
- **Libraries:** libcurl, nlohmann/json
- **Development Environment:** Linux
- **Future Directions:** AI / ML, Browser Automation, Distributed Systems, Docker, Observability

## Getting Started

### Prerequisites

- C++ compiler with C++20 support
- CMake
- PostgreSQL
- Git

### Clone the Repository

```bash
git clone git@github.com:Raksha-BR/ORBIT.git
cd ORBIT
```

### Build the Project

Configure the project:

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
```

Compile:

```bash
cmake --build build
```

Alternatively, use the project build script:

```bash
./scripts/build.sh
```

### Run Tests

```bash
ctest --test-dir build --output-on-failure
```

### Command-Line Interface

ORBIT provides a CLI for interacting with job listings.

Examples:

```bash
# List available jobs
./build/orbit_cli jobs list

# Search jobs by keyword
./build/orbit_cli jobs search --keyword C++

# Display a job by ID
./build/orbit_cli jobs show --id 1

# Rank jobs
./build/orbit_cli jobs rank --top 10

# Collect jobs from the mock source
./build/orbit_cli jobs collect --source mock
```

## Current Capabilities

- Job repository backed by PostgreSQL
- Job listing and keyword search
- Job relevance scoring and ranking
- Job source collection pipeline
- Job deduplication
- CLI-based job management
- Automated unit and integration testing

## Project Roadmap

- [x] Job repository and database integration
- [x] Job search service
- [x] Job relevance scoring
- [x] Job ranking filters
- [x] Job source collection pipeline
- [ ] Improved real-time job discovery
- [ ] AI-powered job description analysis
- [ ] Candidate profile and skill matching
- [ ] Resume-based job recommendations
- [ ] Application preparation and tracking
- [ ] Browser-based application automation

## Development Status

🚧 **Early Development**

ORBIT is actively evolving toward an intelligent, end-to-end job-search and application platform.