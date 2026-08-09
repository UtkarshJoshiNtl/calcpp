#include "operator/expression_evaluator.h"

#include <cmath>
#include <cstddef>
#include <cwchar>
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <string>

namespace calculator {

namespace {

double Pi() {
    return std::acos(-1.0);
}

std::wstring Widen(const std::string& text) {
    return std::wstring(text.begin(), text.end());
}

class Parser {
public:
    explicit Parser(const std::wstring& expression)
        : expression_(expression) {}

    double Parse() {
        const double value = ParseExpression();
        SkipWhitespace();
        if (position_ != expression_.size()) {
            throw std::runtime_error("Unexpected trailing characters");
        }
        return value;
    }

private:
    double ParseExpression() {
        double value = ParseTerm();
        while (true) {
            SkipWhitespace();
            if (Match(L'+')) {
                value += ParseTerm();
            } else if (Match(L'-')) {
                value -= ParseTerm();
            } else {
                break;
            }
        }
        return value;
    }

    double ParseTerm() {
        double value = ParsePower();
        while (true) {
            SkipWhitespace();
            if (Match(L'*')) {
                value *= ParsePower();
            } else if (Match(L'/')) {
                const double denominator = ParsePower();
                if (denominator == 0.0) {
                    throw std::runtime_error("Division by zero");
                }
                value /= denominator;
            } else {
                break;
            }
        }
        return value;
    }

    double ParsePower() {
        double value = ParseUnary();
        SkipWhitespace();
        if (Match(L'^')) {
            const double exponent = ParsePower();
            value = std::pow(value, exponent);
        }
        return value;
    }

    double ParseUnary() {
        SkipWhitespace();
        if (Match(L'+')) {
            return ParseUnary();
        }
        if (Match(L'-')) {
            return -ParseUnary();
        }
        return ParsePrimary();
    }

    double ParsePrimary() {
        SkipWhitespace();
        if (Match(L'(')) {
            const double value = ParseExpression();
            SkipWhitespace();
            if (!Match(L')')) {
                throw std::runtime_error("Expected closing ')' ");
            }
            return value;
        }

        if (position_ < expression_.size() && (std::iswdigit(expression_[position_]) || expression_[position_] == L'.')) {
            return ParseNumber();
        }

        if (position_ < expression_.size() && std::iswalpha(expression_[position_])) {
            return ParseIdentifier();
        }

        throw std::runtime_error("Expected a number, constant, function, or parenthesized expression");
    }

    double ParseNumber() {
        const wchar_t* start = expression_.c_str() + position_;
        wchar_t* end = nullptr;
        const double value = std::wcstod(start, &end);
        if (end == start) {
            throw std::runtime_error("Invalid number");
        }
        position_ = static_cast<std::size_t>(end - expression_.c_str());
        return value;
    }

    double ParseIdentifier() {
        const std::size_t start = position_;
        while (position_ < expression_.size() && std::iswalpha(expression_[position_])) {
            ++position_;
        }

        const std::wstring identifier = expression_.substr(start, position_ - start);

        if (identifier == L"pi") {
            return Pi();
        }

        if (identifier == L"e") {
            return std::exp(1.0);
        }

        SkipWhitespace();
        if (!Match(L'(')) {
            throw std::runtime_error("Expected '(' after function name");
        }

        const double argument = ParseExpression();
        SkipWhitespace();
        if (!Match(L')')) {
            throw std::runtime_error("Expected closing ')' after function argument");
        }

        if (identifier == L"sin") {
            return std::sin(argument);
        }
        if (identifier == L"cos") {
            return std::cos(argument);
        }
        if (identifier == L"tan") {
            return std::tan(argument);
        }
        if (identifier == L"sqrt") {
            if (argument < 0.0) {
                throw std::runtime_error("Square root of a negative number");
            }
            return std::sqrt(argument);
        }
        if (identifier == L"ln") {
            if (argument <= 0.0) {
                throw std::runtime_error("Natural log requires a positive number");
            }
            return std::log(argument);
        }
        if (identifier == L"log") {
            if (argument <= 0.0) {
                throw std::runtime_error("Base-10 log requires a positive number");
            }
            return std::log10(argument);
        }
        if (identifier == L"abs") {
            return std::fabs(argument);
        }

        throw std::runtime_error("Unknown function");
    }

    void SkipWhitespace() {
        while (position_ < expression_.size() && std::iswspace(expression_[position_])) {
            ++position_;
        }
    }

    bool Match(wchar_t character) {
        if (position_ >= expression_.size() || expression_[position_] != character) {
            return false;
        }
        ++position_;
        return true;
    }

    const std::wstring& expression_;
    std::size_t position_ = 0;
};

}  // namespace

EvaluationResult ExpressionEvaluator::Evaluate(const std::wstring& expression) {
    EvaluationResult result;

    try {
        Parser parser(expression);
        result.value = parser.Parse();
        result.success = true;
    } catch (const std::exception& exception) {
        result.success = false;
        result.error = Widen(exception.what());
    }

    return result;
}

std::wstring ExpressionEvaluator::FormatValue(double value) {
    if (std::isnan(value)) {
        return L"nan";
    }

    if (std::isinf(value)) {
        return value > 0 ? L"inf" : L"-inf";
    }

    std::wostringstream stream;
    stream << std::setprecision(12) << std::defaultfloat << value;
    std::wstring text = stream.str();

    const std::size_t scientific_pos = text.find_first_of(L"eE");
    const std::size_t decimal_pos = text.find(L'.');
    if (decimal_pos != std::wstring::npos && scientific_pos == std::wstring::npos) {
        while (!text.empty() && text.back() == L'0') {
            text.pop_back();
        }
        if (!text.empty() && text.back() == L'.') {
            text.pop_back();
        }
    }

    if (text.empty()) {
        text = L"0";
    }

    return text;
}

}  // namespace calculator
