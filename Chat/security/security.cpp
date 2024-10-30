#include "security.h"

c_security* g_security = new c_security;

std::string c_security::get_encryption_key(std::string signature_begin, std::string signature_end)
{
	return sha256(signature_begin + encryption_key + signature_end);
}

std::string c_security::encode_message(std::wstring& msg, std::string key)
{
	std::string cipher;
	int i, k;

	size_t size = msg.size() * sizeof(wchar_t);
	unsigned char const* c = (unsigned char const*)msg.c_str();

	std::vector<unsigned char> mmm(c, c + size);

	for (i = 0; i < mmm.size(); i++)
	{
		k = (i) % key.length();
		cipher.push_back(char(mmm[i] ^ (unsigned char)key.at(k)));
	}

	cipher = base64_encoded((BYTE*)cipher.data(), cipher.size());
	return cipher;
}

std::wstring c_security::decode_message(std::string msg, std::string key)
{
	std::wstring wstr;

	std::vector<BYTE> bt;
	std::vector<unsigned char> bt1;

	int i, k;
	bt = base64_decoded(msg);

	unsigned int b = 0;

	for (i = 0; i < bt.size(); i++)
	{
		k = (i) % key.length();
		bt1.push_back(((unsigned char)(bt.at(i)) ^ (unsigned char)(key.at(k))));
	}

	if (!bt1.empty())
	{
		wchar_t* begin = reinterpret_cast<wchar_t*>(&bt1[0]);
		wstr.assign(begin, begin + bt1.size() / sizeof(wchar_t));
	}

	return wstr;
}