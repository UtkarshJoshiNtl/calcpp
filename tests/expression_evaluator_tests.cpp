#include "operator/expression_evaluator.h"

#include <iostream>
#include <vector>
#include <cmath>

using namespace calculator;

struct TestCase {
    std::wstring expr;
    double expected;
    bool expect_success;
};

int main() {
    std::vector<TestCase> cases = {
        {L"1+1", 2.0, true},
        {L"2*(3+4)", 14.0, true},
        {L"pi", std::acos(-1.0), true},
        {L"sqrt(9)", 3.0, true},
        {L"ln(e)", 1.0, true},
        {L"1/0", 0.0, false},
        {L"(1+2", 0.0, false},
        {L"sin(pi/2)", 1.0, true},
        {L"2^3^2", 512.0, true}, // right-associative
        {L"abs(-5)", 5.0, true},
    };

    int failures = 0;
    for (const auto& tc : cases) {
        const EvaluationResult r = ExpressionEvaluator::Evaluate(tc.expr);
        if (r.success != tc.expect_success) {
            std::wcout << L"Test failed for expr '" << tc.expr << L"': success mismatch\n";
            ++failures;
            continue;
        }
        if (!r.success) {
            std::wcout << L"Expected failure for '" << tc.expr << L"': " << r.error << L"\n";
            continue;
        }
        const double got = r.value;
        const double diff = std::fabs(got - tc.expected);
        if (diff > 1e-9 * std::max(1.0, std::fabs(tc.expected))) {
            std::wcout << L"Test failed for '" << tc.expr << L"': expected " << tc.expected << L", got " << got << L"\n";
            ++failures;
        }
    }

    if (failures == 0) {
        std::wcout << L"All tests passed\n";
        return 0;
    }

    std::wcout << failures << L" tests failed\n";
    return 1;
}
