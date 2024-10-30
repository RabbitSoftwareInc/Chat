// Chat client on TCP (WinSock) with full packet encryption.
// Copyright (C) Rabbit Software 2024.

#define VERSION L"v1.5"
#define CHANGELOG L"29.10.2024: Fixed issues caused by packet fragmentation. Added support for custom localizations."

#include "client_state.h"
#include "security/security.h"

#include "networking/networking.h"
#include "gui/gui.h"

c_client_state* g_client_state = new c_client_state;

DWORD WINAPI gui_thread(PVOID n)
{
	g_gui->initialize();
	return 0;
}

int main(int argc, char* argv[])
{
	ShowWindow(GetConsoleWindow(), SW_HIDE);

	if (argc <= 1)
		return 1;

	srand(time(0));
	g_client_state->current_user_name = L"Client" + std::to_wstring(rand());

	for (int i = 0; i < argc; i++)
	{
		if (!strcmp(argv[i], "--disable-censorship"))
			g_client_state->censorship = false;

		if (!strcmp(argv[i], "--disable-msg-sound"))
			g_client_state->sound_on_new_message = false;

		if (!strcmp(argv[i], "--server"))
			g_networking->set_is_server(true);

		if (!strcmp(argv[i], "--ip-addr") && i < (argc - 1))
		{
			std::string ip_address = std::string(argv[i + 1]);
			g_networking->set_ip_address(s2ws(ip_address));
		}

		if (!strcmp(argv[i], "--port") && i < (argc - 1))
			g_networking->set_port(std::stoi(argv[i + 1]));

		if (!strcmp(argv[i], "--encryption-key") && i < (argc - 1))
			g_security->encryption_key = std::string(argv[i + 1]);
		
		if (!strcmp(argv[i], "--name") && i < (argc - 1))
			g_client_state->current_user_name = s2ws(std::string(argv[i + 1]));
	}

	BASS_Init(-1, 44100, 0, 0, NULL);

	CreateThread(NULL, NULL, gui_thread, NULL, NULL, NULL);
	g_networking->initialize();

	BASS_Free();
}