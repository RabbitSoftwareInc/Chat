#pragma once
#include "../util/util.h"

struct Message_t
{
	Message_t();
	Message_t(std::wstring text);
	Message_t(std::wstring name, std::wstring text, std::wstring time);

	std::wstring user_name = L"";
	std::wstring user_message = L"";

	std::wstring send_time = L"";

	bool shown = false;
};