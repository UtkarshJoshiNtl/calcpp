#pragma once

#include <string>

namespace calculator {

struct EvaluationResult {
    bool success = false;
    double value = 0.0;
    std::wstring error;
};

class ExpressionEvaluator {
public:
    static EvaluationResult Evaluate(const std::wstring& expression);
    static std::wstring FormatValue(double value);
};

}  // namespace calculator
