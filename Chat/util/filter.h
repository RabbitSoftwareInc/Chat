#pragma once
#include <string>
#include <algorithm>

class c_text_filter
{
public:
	int get_word_idx_in_ban_list(std::wstring word);
	bool is_allowed_low_character_word(std::wstring word);

	std::wstring replace_banned_word(std::wstring word);
	std::wstring filter_message_text(std::wstring text);
};

extern c_text_filter* g_text_filter;