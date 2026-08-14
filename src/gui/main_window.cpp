#include "gui/main_window.h"

#include <cmath>
#include <string>
#include <cwctype>

#include "operator/expression_evaluator.h"

namespace calculator {

namespace {

constexpr wchar_t kWindowClassName[] = L"CalcCppMainWindow";
constexpr wchar_t kWindowTitle[] = L"CalcCpp Scientific Calculator";

constexpr int kMargin = 12;
constexpr int kGap = 8;
constexpr int kDisplayHeight = 48;
constexpr int kMinClientWidth = 520;
constexpr int kMinClientHeight = 560;

}  // namespace

MainWindow::~MainWindow() {
    if (font_ != nullptr) {
        DeleteObject(font_);
        font_ = nullptr;
    }
}

int MainWindow::Run(HINSTANCE instance, int show_command) {
    instance_ = instance;
    if (!RegisterClass()) {
        return 1;
    }

    if (!CreateWindowInstance(show_command)) {
        return 1;
    }

    MSG message{};
    while (GetMessageW(&message, nullptr, 0, 0) > 0) {
        TranslateMessage(&message);
        DispatchMessageW(&message);
    }

    return static_cast<int>(message.wParam);
}

bool MainWindow::RegisterClass() {
    WNDCLASSEXW window_class{};
    window_class.cbSize = sizeof(window_class);
    window_class.style = CS_HREDRAW | CS_VREDRAW;
    window_class.lpfnWndProc = MainWindow::WindowProc;
    window_class.hInstance = instance_;
    window_class.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    window_class.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);
    window_class.lpszClassName = kWindowClassName;

    return RegisterClassExW(&window_class) != 0 || GetLastError() == ERROR_CLASS_ALREADY_EXISTS;
}

bool MainWindow::CreateWindowInstance(int show_command) {
    window_ = CreateWindowExW(
        0,
        kWindowClassName,
        kWindowTitle,
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        720,
        640,
        nullptr,
        nullptr,
        instance_,
        this);

    if (!window_) {
        return false;
    }

    ShowWindow(window_, show_command);
    UpdateWindow(window_);
    return true;
}

void MainWindow::CreateChildControls() {
    RecreateFont(GetDpi());

    display_ = CreateWindowExW(
        WS_EX_CLIENTEDGE,
        L"EDIT",
        L"",
        WS_CHILD | WS_VISIBLE | ES_READONLY | ES_RIGHT | ES_AUTOHSCROLL,
        0,
        0,
        0,
        0,
        window_,
        reinterpret_cast<HMENU>(kDisplayId),
        instance_,
        nullptr);

    if (display_ != nullptr) {
        SetWindowLongPtrW(display_, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));
        SubclassDisplay();
        SendMessageW(display_, WM_SETFONT, reinterpret_cast<WPARAM>(font_), TRUE);
    }

    button_specs_ = {
        {kButtonIdBase + 0, L"C", L"", Action::Clear},
        {kButtonIdBase + 1, L"BS", L"", Action::Backspace},
        {kButtonIdBase + 2, L"(", L"(", Action::Append},
        {kButtonIdBase + 3, L")", L")", Action::Append},
        {kButtonIdBase + 4, L"/", L"/", Action::Append},
        {kButtonIdBase + 5, L"^", L"^", Action::Append},
        {kButtonIdBase + 6, L"7", L"7", Action::Append},
        {kButtonIdBase + 7, L"8", L"8", Action::Append},
        {kButtonIdBase + 8, L"9", L"9", Action::Append},
        {kButtonIdBase + 9, L"*", L"*", Action::Append},
        {kButtonIdBase + 10, L"sqrt", L"sqrt(", Action::Append},
        {kButtonIdBase + 11, L"pi", L"pi", Action::Append},
        {kButtonIdBase + 12, L"4", L"4", Action::Append},
        {kButtonIdBase + 13, L"5", L"5", Action::Append},
        {kButtonIdBase + 14, L"6", L"6", Action::Append},
        {kButtonIdBase + 15, L"-", L"-", Action::Append},
        {kButtonIdBase + 16, L"sin", L"sin(", Action::Append},
        {kButtonIdBase + 17, L"cos", L"cos(", Action::Append},
        {kButtonIdBase + 18, L"1", L"1", Action::Append},
        {kButtonIdBase + 19, L"2", L"2", Action::Append},
        {kButtonIdBase + 20, L"3", L"3", Action::Append},
        {kButtonIdBase + 21, L"+", L"+", Action::Append},
        {kButtonIdBase + 22, L"tan", L"tan(", Action::Append},
        {kButtonIdBase + 23, L"ln", L"ln(", Action::Append},
        {kButtonIdBase + 24, L"0", L"0", Action::Append},
        {kButtonIdBase + 25, L".", L".", Action::Append},
        {kButtonIdBase + 26, L"=", L"", Action::Evaluate},
        {kButtonIdBase + 27, L"e", L"e", Action::Append},
        {kButtonIdBase + 28, L"log", L"log(", Action::Append},
        {kButtonIdBase + 29, L"abs", L"abs(", Action::Append},
    };

    buttons_.reserve(button_specs_.size());
    for (const auto& spec : button_specs_) {
        HWND button = CreateWindowExW(
            0,
            L"BUTTON",
            spec.text,
            WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
            0,
            0,
            0,
            0,
            window_,
            reinterpret_cast<HMENU>(static_cast<intptr_t>(spec.id)),
            instance_,
            nullptr);

        if (button != nullptr) {
            SendMessageW(button, WM_SETFONT, reinterpret_cast<WPARAM>(font_), TRUE);
        }
        buttons_.push_back(button);
    }

    UpdateDisplay(L"0");
}

UINT MainWindow::GetDpi() const {
    if (window_ != nullptr) {
        using GetDpiForWindowFn = UINT(WINAPI*)(HWND);
        HMODULE user32 = GetModuleHandleW(L"user32.dll");
        if (user32 != nullptr) {
            auto get_dpi = reinterpret_cast<GetDpiForWindowFn>(
                GetProcAddress(user32, "GetDpiForWindow"));
            if (get_dpi != nullptr) {
                const UINT dpi = get_dpi(window_);
                if (dpi != 0) {
                    return dpi;
                }
            }
        }
    }

    HDC screen = GetDC(nullptr);
    const UINT dpi = static_cast<UINT>(GetDeviceCaps(screen, LOGPIXELSX));
    ReleaseDC(nullptr, screen);
    return dpi != 0 ? dpi : 96;
}

void MainWindow::RecreateFont(UINT dpi) {
    if (font_ != nullptr) {
        DeleteObject(font_);
        font_ = nullptr;
    }

    const int height = -MulDiv(9, static_cast<int>(dpi), 72);
    font_ = CreateFontW(
        height,
        0,
        0,
        0,
        FW_NORMAL,
        FALSE,
        FALSE,
        FALSE,
        DEFAULT_CHARSET,
        OUT_DEFAULT_PRECIS,
        CLIP_DEFAULT_PRECIS,
        CLEARTYPE_QUALITY,
        DEFAULT_PITCH | FF_DONTCARE,
        L"Segoe UI");

    if (font_ == nullptr) {
        font_ = static_cast<HFONT>(GetStockObject(DEFAULT_GUI_FONT));
    }
}

void MainWindow::ApplyFontToControls() {
    if (font_ == nullptr) {
        return;
    }

    if (display_ != nullptr) {
        SendMessageW(display_, WM_SETFONT, reinterpret_cast<WPARAM>(font_), TRUE);
    }
    for (HWND button : buttons_) {
        SendMessageW(button, WM_SETFONT, reinterpret_cast<WPARAM>(font_), TRUE);
    }
}

void MainWindow::SubclassDisplay() {
    if (display_ == nullptr) {
        return;
    }
    previous_display_proc_ = reinterpret_cast<WNDPROC>(
        SetWindowLongPtrW(display_, GWLP_WNDPROC,
                          reinterpret_cast<LONG_PTR>(
                              reinterpret_cast<WNDPROC>(&MainWindow::DisplayProc))));
}

LRESULT CALLBACK MainWindow::DisplayProc(HWND window, UINT message, WPARAM wparam, LPARAM lparam) {
    MainWindow* self = reinterpret_cast<MainWindow*>(
        GetWindowLongPtrW(window, GWLP_USERDATA));

    switch (message) {
    case WM_CHAR:
    case WM_KEYDOWN:
    case WM_KEYUP:
    case WM_SYSKEYDOWN:
        if (self != nullptr && self->window_ != nullptr) {
            SendMessageW(self->window_, message, wparam, lparam);
            return 0;
        }
        break;
    default:
        break;
    }

    if (self != nullptr && self->previous_display_proc_ != nullptr) {
        return CallWindowProcW(self->previous_display_proc_, window, message, wparam, lparam);
    }
    return DefWindowProcW(window, message, wparam, lparam);
}

void MainWindow::LayoutControls() {
    if (window_ == nullptr || display_ == nullptr) {
        return;
    }

    RECT client_rect{};
    GetClientRect(window_, &client_rect);
    const int width = client_rect.right - client_rect.left;
    const int height = client_rect.bottom - client_rect.top;

    const int display_width = width - (kMargin * 2);
    SetWindowPos(display_, nullptr, kMargin, kMargin, display_width, kDisplayHeight, SWP_NOZORDER);

    constexpr int kColumns = 6;
    constexpr int kRows = 5;

    const int button_top = kMargin + kDisplayHeight + kGap;
    const int available_width = width - (kMargin * 2) - (kGap * (kColumns - 1));
    const int available_height = height - button_top - kMargin - (kGap * (kRows - 1));

    const int button_width = available_width / kColumns;
    const int button_height = available_height / kRows;

    for (std::size_t index = 0; index < buttons_.size(); ++index) {
        const int row = static_cast<int>(index / kColumns);
        const int column = static_cast<int>(index % kColumns);
        const int x = kMargin + column * (button_width + kGap);
        const int y = button_top + row * (button_height + kGap);

        if (buttons_[index] != nullptr) {
            SetWindowPos(buttons_[index], nullptr, x, y, button_width, button_height, SWP_NOZORDER);
        }
    }
}

void MainWindow::UpdateDisplay(const std::wstring& text) const {
    if (display_ != nullptr) {
        SetWindowTextW(display_, text.c_str());
    }
}

std::wstring MainWindow::GetDisplayText() const {
    if (display_ == nullptr) {
        return {};
    }

    const int length = GetWindowTextLengthW(display_);
    std::wstring text(static_cast<std::size_t>(length) + 1, L'\0');
    GetWindowTextW(display_, text.data(), length + 1);
    text.resize(static_cast<std::size_t>(length));
    return text;
}

void MainWindow::AppendText(const std::wstring& token) {
    std::wstring current = GetDisplayText();
    if (current == L"0") {
        current.clear();
    }
    current += token;
    UpdateDisplay(current.empty() ? L"0" : current);
}

void MainWindow::ClearDisplay() {
    UpdateDisplay(L"0");
}

void MainWindow::RemoveLastCharacter() {
    std::wstring current = GetDisplayText();
    if (current.empty() || current == L"0") {
        UpdateDisplay(L"0");
        return;
    }

    current.pop_back();
    if (current.empty()) {
        current = L"0";
    }
    UpdateDisplay(current);
}

void MainWindow::EvaluateExpression() {
    const std::wstring expression = GetDisplayText();
    const EvaluationResult result = ExpressionEvaluator::Evaluate(expression);

    if (!result.success) {
        MessageBoxW(window_, result.error.c_str(), L"Calculation error", MB_OK | MB_ICONERROR);
        return;
    }

    UpdateDisplay(ExpressionEvaluator::FormatValue(result.value));
}

void MainWindow::HandleCommand(WORD command_id) {
    if (command_id == kDisplayId) {
        return;
    }

    for (const auto& spec : button_specs_) {
        if (spec.id == command_id) {
            HandleButton(spec.id);
            break;
        }
    }

    SetFocus(window_);
}

void MainWindow::HandleButton(int button_id) {
    const ButtonSpec* spec = nullptr;
    for (const auto& candidate : button_specs_) {
        if (candidate.id == button_id) {
            spec = &candidate;
            break;
        }
    }

    if (spec == nullptr) {
        return;
    }

    switch (spec->action) {
    case Action::Append:
        AppendText(spec->token);
        break;
    case Action::Clear:
        ClearDisplay();
        break;
    case Action::Backspace:
        RemoveLastCharacter();
        break;
    case Action::Evaluate:
        EvaluateExpression();
        break;
    }
}

void MainWindow::SetWindowTitle() {
    SetWindowTextW(window_, kWindowTitle);
}

LRESULT CALLBACK MainWindow::WindowProc(HWND window, UINT message, WPARAM wparam, LPARAM lparam) {
    MainWindow* self = nullptr;

    if (message == WM_NCCREATE) {
        const auto* create_struct = reinterpret_cast<CREATESTRUCTW*>(lparam);
        self = static_cast<MainWindow*>(create_struct->lpCreateParams);
        SetWindowLongPtrW(window, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(self));
        self->window_ = window;
    } else {
        self = reinterpret_cast<MainWindow*>(GetWindowLongPtrW(window, GWLP_USERDATA));
    }

    if (self != nullptr) {
        return self->HandleMessage(message, wparam, lparam);
    }

    return DefWindowProcW(window, message, wparam, lparam);
}

LRESULT MainWindow::HandleMessage(UINT message, WPARAM wparam, LPARAM lparam) {
    switch (message) {
    case WM_CREATE:
        SetWindowTitle();
        CreateChildControls();
        LayoutControls();
        SetFocus(window_);
        return 0;
    case WM_CHAR: {
        const wchar_t ch = static_cast<wchar_t>(wparam);
        if (iswprint(ch)) {
            std::wstring s(1, ch);
            AppendText(s);
        }
        return 0;
    }
    case WM_KEYDOWN:
        switch (wparam) {
        case VK_BACK:
            RemoveLastCharacter();
            return 0;
        case VK_RETURN:
            EvaluateExpression();
            return 0;
        case VK_ESCAPE:
            ClearDisplay();
            return 0;
        default:
            break;
        }
        return 0;
    case WM_SIZE:
        LayoutControls();
        return 0;
    case WM_COMMAND:
        HandleCommand(LOWORD(wparam));
        return 0;
    case WM_DPICHANGED: {
        const auto* suggested = reinterpret_cast<const RECT*>(lparam);
        SetWindowPos(window_,
                     nullptr,
                     suggested->left,
                     suggested->top,
                     suggested->right - suggested->left,
                     suggested->bottom - suggested->top,
                     SWP_NOZORDER | SWP_NOACTIVATE);
        RecreateFont(static_cast<UINT>(wparam));
        ApplyFontToControls();
        LayoutControls();
        return 0;
    }
    case WM_GETMINMAXINFO: {
        auto* info = reinterpret_cast<MINMAXINFO*>(lparam);
        info->ptMinTrackSize.x = kMinClientWidth;
        info->ptMinTrackSize.y = kMinClientHeight;
        return 0;
    }
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    default:
        break;
    }

    return DefWindowProcW(window_, message, wparam, lparam);
}

}  // namespace calculator
