#include "util.h"

std::vector<std::wstring> split_string(std::wstring str, wchar_t delimiter)
{
	std::wstringstream ss(str);

	std::wstring token;
	std::vector<std::wstring> tokens;

	while (std::getline(ss, token, delimiter))
		tokens.push_back(token);

	return tokens;
}

std::wstring s2ws(const std::string& str)
{
	using convert_typeX = std::codecvt_utf8<wchar_t>;
	std::wstring_convert<convert_typeX, wchar_t> converterX;

	return converterX.from_bytes(str);
}

std::string ws2s(const std::wstring& wstr)
{
	using convert_typeX = std::codecvt_utf8<wchar_t>;
	std::wstring_convert<convert_typeX, wchar_t> converterX;
	return converterX.to_bytes(wstr);
}

std::wstring get_time()
{
	time_t lt;
	struct tm* t_m;

	lt = time(nullptr);
	t_m = localtime(&lt);

	int time_h = t_m->tm_hour;
	int time_m = t_m->tm_min;
	int time_s = t_m->tm_sec;

	std::wstring time;

	if (time_h < 10)
		time += L"0";

	time += std::to_wstring(time_h) + L":";

	if (time_m < 10)
		time += L"0";

	time += std::to_wstring(time_m) + L":";

	if (time_s < 10)
		time += L"0";

	time += std::to_wstring(time_s);

	return time;
}

// credits: DVI Cable Simulator, Doki Doki Literature Club C++
void stream_music(void* src, DWORD size, HSTREAM& stream)
{
	BASS_ChannelStop(stream);
	BASS_StreamFree(stream);

	stream = BASS_StreamCreateFile(TRUE, src, 0, size, 0);

	//BASS_ChannelFlags(stream, BASS_SAMPLE_LOOP, BASS_SAMPLE_LOOP);
	BASS_ChannelPlay(stream, TRUE);
}