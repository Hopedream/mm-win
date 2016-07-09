#pragma once
#include <string>
class common
{
public:
	common(void);
	~common(void);

	int Post(const std::string & strUrl, const std::string & strPost, std::string & strResponse);
    int Get(const std::string & strUrl, std::string & strResponse); 

	std::string GetVersion();
	bool SetVerison(std::string strVersion);

	std::string& replace_all_distinct(std::string&   str,const   std::string&   old_value,const   std::string&   new_value);
};

