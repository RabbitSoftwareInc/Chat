// Chat messages censorship and filtering utility.
// Copyright (C) Rabbit Software 2024.

// This filter meant to be used in these projects:
// DVI Cable Simulator: Chat
// Doki Doki Literature Club C++: Chat

#include "filter.h"
#include "banned_words.h"

#include "util.h"

c_text_filter* g_text_filter = new c_text_filter;

int c_text_filter::get_word_idx_in_ban_list(std::wstring word)
{
	std::wstring lowercase_original = word;

	for (int i = 0; i < lowercase_original.length(); i++)
		lowercase_original.at(i) = std::tolower(lowercase_original.at(i));

	for (int i = 0; i < lowercase_original.length(); i++)
	{
		for (int c = 0; c < number_letters.size(); c++)
		{
			if (lowercase_original.at(i) == number_letters.at(c).first)
				lowercase_original.at(i) = number_letters.at(c).second;
		}
	}

	wchar_t last_symbol = '\0';
	std::wstring lowercase_filtered = L"";
	std::wstring lowercase = L"";

	for (int i = 0; i < lowercase_original.length(); i++)
	{
		if (lowercase_original.at(i) != last_symbol)
			lowercase_filtered += lowercase_original.at(i);

		last_symbol = lowercase_original.at(i);
	}

	for (int i = 0; i < lowercase_filtered.length(); i++)
	{
		bool found_special_symbol = false;

		for (int c = 0; c < special_symbols.size(); c++)
		{
			if (special_symbols.at(c) == lowercase_filtered.at(i))
				found_special_symbol = true;
		}

		if (!found_special_symbol)
			lowercase += lowercase_filtered.at(i);
	}

	for (int i = 0; i < banned_words.size(); i++)
	{
		for (int c = 0; c < std::get<0>(banned_words.at(i)).size(); c++)
		{
			for (int p = 0; p < word_endings.size(); p++)
			{
				if (std::wstring(std::get<0>(banned_words.at(i)).at(c) + word_endings.at(p)) == lowercase || wcsstr(lowercase.c_str(), std::wstring(std::get<0>(banned_words.at(i)).at(c) + word_endings.at(p)).c_str()))
					return i;
			}
		}
	}

	return -1;
}

bool c_text_filter::is_allowed_low_character_word(std::wstring word)
{
	for (int i = 0; i < allowed_low_character_words.size(); i++)
	{
		if (word == allowed_low_character_words.at(i))
			return true;
	}

	return false;
}

std::wstring c_text_filter::replace_banned_word(std::wstring word)
{
	std::vector<bool> cases;

	for (int i = 0; i < word.length(); i++)
		cases.push_back(std::isupper(word.at(i)));

	int idx = get_word_idx_in_ban_list(word);
	std::wstring replace_word = std::get<1>(banned_words.at(idx));

	if (word.length() == replace_word.length())
	{
		for (int i = 0; i < replace_word.length(); i++)
		{
			if (cases.at(i))
				replace_word.at(i) = std::toupper(replace_word.at(i));
		}
	}
	else
	{
		int uppercases = 0;

		for (int i = 0; i < cases.size(); i++)
		{
			if (cases.at(i))
				uppercases++;
		}

		if (uppercases == cases.size())
		{
			for (int i = 0; i < replace_word.length(); i++)
				replace_word.at(i) = std::toupper(replace_word.at(i));
		}
		else
		{
			if (uppercases == 1 && cases.at(0))
				replace_word.at(0) = std::toupper(replace_word.at(0));
			else
			{
				for (int i = 0; i < std::clamp((int)replace_word.length(), 0, (int)cases.size()); i++)
				{
					if (cases.at(i))
						replace_word.at(i) = std::toupper(replace_word.at(i));
				}
			}
		}
	}

	return replace_word;
}

std::wstring c_text_filter::filter_message_text(std::wstring text)
{
	std::vector<std::wstring> words = split_string(text, L' ');

	if (words.empty())
		return text;

	std::vector<std::wstring> words_backup = words;

	std::vector<std::pair<int, std::wstring>> low_character_words;

	for (int i = 0; i < words.size(); i++)
	{
		if (words.at(i).length() < 6)
		{
			if (!is_allowed_low_character_word(words.at(i)))
				low_character_words.push_back(std::make_pair(i, words.at(i)));
		}
	}

	int first_id = -1;
	int last_id = -1;

	int first_id_low = -1;
	int last_id_low = -1;

	int ids_in_a_row = 0;

	for (int i = 0; i < low_character_words.size(); i++)
	{
		if ((last_id + 1) == low_character_words.at(i).first)
		{
			ids_in_a_row++;

			if (last_id_low == -1)
				first_id_low = i;

			last_id_low = i;
		}

		if (last_id == -1)
			first_id = low_character_words.at(i).first;

		last_id = low_character_words.at(i).first;
	}

	if (ids_in_a_row > 0)
	{
		while (ids_in_a_row > 1)
		{
			words.erase(words.begin() + first_id);
			ids_in_a_row--;
		}

		std::wstring new_word = L"";

		for (int i = first_id_low; i < last_id_low + 1; i++)
			new_word += low_character_words.at(i).second;

		if (new_word != L"")
			words.at(first_id) = new_word;
	}

	int banned_words_count = 0;

	for (int i = 0; i < words.size(); i++)
	{
		if (get_word_idx_in_ban_list(words.at(i)) != -1)
		{
			words.at(i) = replace_banned_word(words.at(i));
			banned_words_count++;
		}
	}

	if (!banned_words_count)
		words = words_backup;

	std::wstring filtered_text = L"";

	for (int i = 0; i < words.size(); i++)
	{
		filtered_text += words.at(i);

		if (i < (words.size() - 1))
			filtered_text += L" ";
	}

	if (banned_words_count)
		filtered_text = L"*censored*";

	return filtered_text;
}