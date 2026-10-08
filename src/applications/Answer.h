#pragma once

#include "AnswerClassification.h"

#include <string>

namespace orbit {

struct Answer {
    std::string value;

    AnswerClassification classification;

    std::string reason;
};

} // namespace orbit