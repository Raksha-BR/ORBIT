#pragma once

namespace orbit {

enum class AnswerClassification {
    AutoAnswer,
    RequiresConfirmation,
    UserOnly,
    Unknown
};

} // namespace orbit