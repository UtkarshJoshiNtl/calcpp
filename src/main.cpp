#include <windows.h>

#include "gui/main_window.h"

int WINAPI wWinMain(HINSTANCE instance, HINSTANCE, PWSTR, int show_command) {
	calculator::MainWindow window;
	return window.Run(instance, show_command);
}

