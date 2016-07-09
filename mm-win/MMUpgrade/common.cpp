#include "common.h"
#include "curl.h"
#include "StringHelper.h"


#define WEB_SERVER_BASE_URL "http://124.206.140.155:9001/api/"

common::common(void)
{
}


common::~common(void)
{
}

static size_t OnWriteData(void* buffer, size_t size, size_t nmemb, void* lpVoid)
{
	std::string* str = dynamic_cast<std::string*>((std::string *)lpVoid);
	if( NULL == str || NULL == buffer )
	{
		return -1;
	}

    char* pData = (char*)buffer;
    str->append(pData, size * nmemb);
	return nmemb;
}

int common::Post(const std::string & strUrl, const std::string & strPost, std::string & strResponse)
{
	CURLcode res;
	CURL* curl = curl_easy_init();
	if(NULL == curl)
	{
		return CURLE_FAILED_INIT;
	}
	//if(m_bDebug)
	//{
	//	curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);
	//	curl_easy_setopt(curl, CURLOPT_DEBUGFUNCTION, OnDebug);
	//}
	curl_easy_setopt(curl, CURLOPT_URL, strUrl.c_str());
	//curl_easy_setopt(curl, CURLOPT_PORT, RE_PORT);
	curl_easy_setopt(curl, CURLOPT_POST, 1);
	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, strPost.c_str());
	curl_easy_setopt(curl, CURLOPT_READFUNCTION, NULL);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, OnWriteData);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&strResponse);
	curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
	curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 3);
	curl_easy_setopt(curl, CURLOPT_TIMEOUT, 3);
	res = curl_easy_perform(curl);
	curl_easy_cleanup(curl);
	return res;
}

int common::Get(const std::string & strUrl, std::string & strResponse)
{

	CURLcode res;
	CURL* curl = curl_easy_init();
	if(NULL == curl)
	{
		return CURLE_FAILED_INIT;
	}
	//if(m_bDebug)
	//{
	//	curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);
	//	curl_easy_setopt(curl, CURLOPT_DEBUGFUNCTION, OnDebug);
	//}
/*<pre name="code" class="cpp">	*/
	curl_easy_setopt(curl, CURLOPT_URL, strUrl.c_str());
	//curl_easy_setopt(curl, CURLOPT_PORT, RE_PORT);
	curl_easy_setopt(curl, CURLOPT_READFUNCTION, NULL);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, OnWriteData);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&strResponse);
	/**
	* 当多个线程都使用超时处理的时候，同时主线程中有sleep或是wait等操作。
	* 如果不设置这个选项，libcurl将会发信号打断这个wait从而导致程序退出。
	*/
	curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
	curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 3);
	curl_easy_setopt(curl, CURLOPT_TIMEOUT, 3);
	res = curl_easy_perform(curl);
	curl_easy_cleanup(curl);
	return res;
}


std::string common::GetVersion()
{
	std::string strhttpUrl(WEB_SERVER_BASE_URL);
	strhttpUrl += "site/get-version";
	std::string strResponse;
	int nResult = Get(strhttpUrl, strResponse);
	return CChineseCode::DecodeUTF8(strResponse);
}

bool common::SetVerison(string strVersion)
{
	string strUrl(WEB_SERVER_BASE_URL);
	strUrl += "site/save-version";
	//string strToken = CController::Instance()->GetToken();
	string strHttpResponse;
	int retCode = Post(strUrl, strVersion,strHttpResponse);
	if (retCode == 1)
	{
		return true;
	}
	
	return false;
}

string& common::replace_all_distinct(string&   str,const   string&   old_value,const   string&   new_value)     
{     
    for(string::size_type   pos(0);   pos!=string::npos;   pos+=new_value.length())   {     
        if(   (pos=str.find(old_value,pos))!=string::npos   )     
            str.replace(pos,old_value.length(),new_value);     
        else   break;     
    }     
    return   str;     
} 