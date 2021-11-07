#include "app.h"

int main()
{
	app_state_t state;

	if (!app_init(&state)) return 1;
	app_run(&state);
	app_shutdown(&state);

	return 0;
}
