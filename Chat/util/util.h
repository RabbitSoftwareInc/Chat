#define _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS

#pragma once

#include "../win_includes.h"
#include "../libraries/bass/bass.h"

extern std::vector<std::wstring> split_string(std::wstring str, wchar_t delimiter);
extern std::wstring s2ws(const std::string& str);
extern std::string ws2s(const std::wstring& wstr);
extern std::wstring get_time();
extern void stream_music(void* src, DWORD size, HSTREAM& stream);