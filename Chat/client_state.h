#pragma once
#include "util/util.h"
#include "networking/message.h"

class c_client_state
{
public:
	std::wstring current_user_name = L"default";

	HSTREAM player_stream;

	bool censorship = true;
	bool sound_on_new_message = true;

	bool insecure = false;

	bool data_transfer = false;
	bool data_transfer_receive = false;

	bool should_send_message = false;
	std::wstring message_text = L"";

	std::wstring data_transfer_file = L"";

	std::vector<std::wstring> message_history;
};

extern c_client_state* g_client_state;