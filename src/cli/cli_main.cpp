#include "operator/expression_evaluator.h"

#include <iostream>
#include <string>
#include <locale>
#include <codecvt>

using namespace calculator;

static std::wstring ToWString(const std::string& s) {
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> conv;
    return conv.from_bytes(s);
}

static std::string ToString(const std::wstring& ws) {
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> conv;
    return conv.to_bytes(ws);
}

int main() {
    std::string line;
    std::cout << "CalcCpp CLI. Type 'quit' or 'exit' to leave.\n";
    while (true) {
        std::cout << "> ";
        if (!std::getline(std::cin, line)) break;
        if (line.empty()) continue;
        if (line == "quit" || line == "exit") break;

        const std::wstring expr = ToWString(line);
        const EvaluationResult r = ExpressionEvaluator::Evaluate(expr);
        if (!r.success) {
            std::cout << "Error: " << ToString(r.error) << "\n";
        } else {
            std::wcout << L"= " << ExpressionEvaluator::FormatValue(r.value) << L"\n";
        }
    }
    return 0;
}
