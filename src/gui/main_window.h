#pragma once

#include <windows.h>

#include <string>
#include <vector>

namespace calculator {

class MainWindow {
public:
    int Run(HINSTANCE instance, int show_command);

private:
    enum class Action {
        Append,
        Clear,
        Backspace,
        Evaluate,
    };

    struct ButtonSpec {
        int id;
        const wchar_t* text;
        const wchar_t* token;
        Action action;
    };

    static constexpr int kDisplayId = 1001;
    static constexpr int kButtonIdBase = 2000;

    bool RegisterClass();
    bool CreateWindowInstance(int show_command);
    void CreateChildControls();
    void LayoutControls();
    void UpdateDisplay(const std::wstring& text) const;
    std::wstring GetDisplayText() const;
    void AppendText(const std::wstring& token);
    void ClearDisplay();
    void RemoveLastCharacter();
    void EvaluateExpression();
    void HandleCommand(WORD command_id);
    void HandleButton(int button_id);
    void SetWindowTitle();

    static LRESULT CALLBACK WindowProc(HWND window, UINT message, WPARAM wparam, LPARAM lparam);
    LRESULT HandleMessage(UINT message, WPARAM wparam, LPARAM lparam);

    HWND window_ = nullptr;
    HWND display_ = nullptr;
    HINSTANCE instance_ = nullptr;
    HFONT font_ = nullptr;
    std::vector<HWND> buttons_;
    std::vector<ButtonSpec> button_specs_;
};

}  // namespace calculator
