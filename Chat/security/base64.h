#pragma once

#ifndef _BASE64_H_
#define _BASE64_H_

#include <vector>
#include <string>
typedef unsigned char BYTE;

std::string base64_encoded(BYTE const* buf, unsigned int bufLen);
std::vector<BYTE> base64_decoded(std::string const&);

#endif