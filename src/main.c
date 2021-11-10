#include <windows.h>

#include "app.h"

int main()
{
	app_state_t state;

	if (!app_init(&state)) return 1;
	app_run(&state);
	app_shutdown(&state);

	return 0;
}

int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hInstPrev, PSTR cmdline, int cmdshow)
{
	return main();
}
