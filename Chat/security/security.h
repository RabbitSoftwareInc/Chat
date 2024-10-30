#pragma once

#include "base64.h"
#include "sha-256.h"

class c_security
{
public:
	std::string encryption_key = "default";
	std::string get_encryption_key(std::string signature_begin = "", std::string signature_end = "");

	std::string encode_message(std::wstring& msg, std::string key);
	std::wstring decode_message(std::string msg, std::string key);
};

extern c_security* g_security;