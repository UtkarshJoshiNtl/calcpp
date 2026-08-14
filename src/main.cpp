#include <windows.h>

#include "gui/main_window.h"

namespace {

void EnablePerMonitorDpiAwareness() {
    HMODULE user32 = GetModuleHandleW(L"user32.dll");
    if (user32 != nullptr) {
        using SetProcessDpiAwarenessContextFn =
            BOOL(WINAPI*)(DPI_AWARENESS_CONTEXT);
        auto set_context = reinterpret_cast<SetProcessDpiAwarenessContextFn>(
            GetProcAddress(user32, "SetProcessDpiAwarenessContext"));
        if (set_context != nullptr) {
            set_context(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
            return;
        }
    }

    HMODULE shcore = GetModuleHandleW(L"shcore.dll");
    if (shcore != nullptr) {
        using SetProcessDpiAwarenessFn = HRESULT(WINAPI*)(int);
        auto set_awareness = reinterpret_cast<SetProcessDpiAwarenessFn>(
            GetProcAddress(shcore, "SetProcessDpiAwareness"));
        if (set_awareness != nullptr &&
            set_awareness(2 /* PROCESS_PER_MONITOR_DPI_AWARE */) == S_OK) {
            return;
        }
    }

    SetProcessDPIAware();
}

}  // namespace

int WINAPI wWinMain(HINSTANCE instance, HINSTANCE, PWSTR, int show_command) {
	EnablePerMonitorDpiAwareness();
	calculator::MainWindow window;
	return window.Run(instance, show_command);
}

