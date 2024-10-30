#include "message.h"

#include "../client_state.h"

Message_t::Message_t()
{
	user_name = L"";
	user_message = L"";

	send_time = L"";
}

Message_t::Message_t(std::wstring text)
{
	user_name = g_client_state->current_user_name;
	user_message = text;

	send_time = get_time();
}

Message_t::Message_t(std::wstring name, std::wstring text, std::wstring time)
{
	user_name = name;
	user_message = text;

	send_time = time;
}