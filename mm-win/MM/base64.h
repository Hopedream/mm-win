#ifndef _BASE64_H_
#define _BASE64_H_

#include <vector>
#include <string>
typedef unsigned char BYTE;
namespace std{
#ifdef _UNICODE
	typedef wstring tstring;
#else
	typedef string tstring;
#endif
}
std::tstring base64_encode(BYTE const* buf, unsigned int bufLen);
std::vector<BYTE> base64_decode(std::tstring const&);

#endif