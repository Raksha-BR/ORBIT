#pragma once

#include <string>

namespace orbit {

enum class EvidenceSource {
    Resume,
    UserProvided,
    Application,
    Email,
    ImportedProfile
};

struct Evidence {
    EvidenceSource source;

    std::string reference;
    std::string excerpt;
};

} // namespace orbit