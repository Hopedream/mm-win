#pragma once
#include <string>  
#include <vector>  
namespace std{
#ifdef _UNICODE
	typedef wstring tstring;
#else
	typedef string tstring;
#endif
}
std::tstring base64_encode0(unsigned char const* , unsigned int len);  
std::vector<unsigned char> base64_decode0(std::tstring const& s); 


