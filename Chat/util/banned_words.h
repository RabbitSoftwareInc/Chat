#pragma once
#include <vector>
#include <string>

// format: words, replacement
std::vector<std::tuple<std::vector<std::wstring>, std::wstring>> banned_words =
{
	std::make_tuple(std::vector<std::wstring> { L"fuck" },  L""),
	std::make_tuple(std::vector<std::wstring> { L"shit", L"crap" },  L""),
	std::make_tuple(std::vector<std::wstring> { L"nigg" },  L""),
	std::make_tuple(std::vector<std::wstring> { L"retard", L"idiot", L"bastard", L"asshole", L"fag" },  L""),
	std::make_tuple(std::vector<std::wstring> { L"kys", L"suicide" },  L""),
	std::make_tuple(std::vector<std::wstring> { L"bitch", L"cum", L"dick", L"dildo", L"ass", L"anal" },  L""),
};

// letters that can be replaced by numbers
// format: number, letter
std::vector<std::pair<wchar_t, wchar_t>> number_letters =
{
	std::make_pair(L'1', L'i'),
	std::make_pair(L'2', L'r'),
	std::make_pair(L'3', L'e'),
	std::make_pair(L'4', L'a'),
	std::make_pair(L'6', L'u'),
	std::make_pair(L'7', L't'),
	std::make_pair(L'8', L'b'),
};

// special symbols to remove from word
std::vector<wchar_t> special_symbols =
{
	L' ',
	L'!',
	L'@',
	L'#',
	L'$',
	L'%',
	L'?',
	L'^',
	L'&',
	L'*',
	L'(',
	L')',
	L'[',
	L']',
	L'{',
	L'}',
	L':',
	L';',
	L'\'',
	L'\"',
	L'<',
	L'>',
	L',',
	L'.',
	L'\\',
	L'/',
	L'-',
	L'=',
	L'+',
	L'_',
};

std::vector<std::wstring> allowed_low_character_words =
{
	L"i",
	L"am",
	L"you",
	L"your",
	L"u",
	L"ur",
	L"in",
	L"is",
	L"a",
	L"an",
	L"the",
};

// word endings to add to compare with ban list
std::vector<std::wstring> word_endings =
{
	L"",
	L"s",
	L"ed",
	L"er",
	L"ing",
	L"y",
	L"py",
	L"ty",
};