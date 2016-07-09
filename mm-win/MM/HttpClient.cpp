
#include "StdAfx.h"
#include "httpclient.h"
#include "curl/curl.h"
#include <string>
#include <sstream>
#include "../Common/Logger.h"

#define RE_PORT "9001"

CHttpClient::CHttpClient(void) : 
m_bDebug(false)
{

}

CHttpClient::~CHttpClient(void)
{

}

static int OnDebug(CURL *, curl_infotype itype, char * pData, size_t size, void *)
{
	if(itype == CURLINFO_TEXT)
	{
		//printf("[TEXT]%s\n", pData);
	}
	else if(itype == CURLINFO_HEADER_IN)
	{
		printf("[HEADER_IN]%s\n", pData);
	}
	else if(itype == CURLINFO_HEADER_OUT)
	{
		printf("[HEADER_OUT]%s\n", pData);
	}
	else if(itype == CURLINFO_DATA_IN)
	{
		printf("[DATA_IN]%s\n", pData);
	}
	else if(itype == CURLINFO_DATA_OUT)
	{
		printf("[DATA_OUT]%s\n", pData);
	}
	return 0;
}

static size_t OnWriteData(void* buffer, size_t size, size_t nmemb, void* lpVoid)
{
	std::tstring* str = dynamic_cast<std::tstring*>((std::tstring *)lpVoid);
	if( NULL == str || NULL == buffer )
	{
		return -1;
	}

    char* pData = (char*)buffer;
    str->append(pData, size * nmemb);
	return nmemb;
}

int CHttpClient::Post(const std::tstring & strUrl, const std::tstring & strPost, 
	const std::tstring & strToken, std::tstring & strResponse)
{
	CLogger::GetInstance()->PrintLog(LOG_DEBUG, "Http Post Req, URL: %s, Content: %s", strUrl.c_str(), strPost.c_str());
	ostringstream os;
	os<<"Authorization: Bearer "
	  <<strToken.c_str();

	CURLcode res;
	CURL* curl = curl_easy_init();
	if(NULL == curl)
	{
		return CURLE_FAILED_INIT;
	}
	if(m_bDebug)
	{
		curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);
		curl_easy_setopt(curl, CURLOPT_DEBUGFUNCTION, OnDebug);
	}
	struct curl_slist *headers=NULL; /* init to NULL is important */
	headers = curl_slist_append(headers, os.str().c_str());
	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);//修改http头

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
	curl_slist_free_all(headers);
	curl_easy_cleanup(curl);
	CLogger::GetInstance()->PrintLog(LOG_DEBUG, "Http Post Req Response: %s" , strResponse.c_str());
	return res;
}

int CHttpClient::Post(const std::tstring & strUrl, const std::tstring & strPost, std::tstring & strResponse)
{
	CURLcode res;
	CURL* curl = curl_easy_init();
	if(NULL == curl)
	{
		return CURLE_FAILED_INIT;
	}
	if(m_bDebug)
	{
		curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);
		curl_easy_setopt(curl, CURLOPT_DEBUGFUNCTION, OnDebug);
	}
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

int CHttpClient::CSSPost(const std::tstring & strUrl, const std::tstring & strPost, std::tstring & strResponse)
{
	CURLcode res;
	CURL* curl = curl_easy_init();
	if(NULL == curl)
	{
		return CURLE_FAILED_INIT;
	}
	if(m_bDebug)
	{
		curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);
		curl_easy_setopt(curl, CURLOPT_DEBUGFUNCTION, OnDebug);
	}
	curl_easy_setopt(curl, CURLOPT_URL, strUrl.c_str());
	//curl_easy_setopt(curl, CURLOPT_PORT, RE_PORT);
	curl_easy_setopt(curl, CURLOPT_POST, 1);

	curl_slist* lstHeader = NULL;
	lstHeader = curl_slist_append(lstHeader,"Connection: keep-alive");
	lstHeader = curl_slist_append(lstHeader,"Accept: application/json, text/javascript, */*; q=0.01");
	lstHeader = curl_slist_append(lstHeader,"X-Requested-With: XMLHttpRequest");
	lstHeader = curl_slist_append(lstHeader,"User-Agent: Mozilla/5.0 (Windows NT 6.1; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/51.0.2704.103 Safari/537.36");
	lstHeader = curl_slist_append(lstHeader,"Content-Type: application/json;charset=UTF-8");
	lstHeader = curl_slist_append(lstHeader,"Accept-Encoding: gzip, deflate");
	lstHeader = curl_slist_append(lstHeader,"Accept-Language: zh-CN,zh;q=0.8");

	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, lstHeader);
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

int CHttpClient::Get(const std::tstring & strUrl, std::tstring & strResponse)
{

	CURLcode res;
	CURL* curl = curl_easy_init();
	if(NULL == curl)
	{
		return CURLE_FAILED_INIT;
	}
	if(m_bDebug)
	{
		curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);
		curl_easy_setopt(curl, CURLOPT_DEBUGFUNCTION, OnDebug);
	}
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

int CHttpClient::CSSGet(const std::tstring & strUrl, std::tstring & strResponse,const std::tstring strToken)
{
	CURLcode res;
	CURL* curl = curl_easy_init();
	if(NULL == curl)
	{
		return CURLE_FAILED_INIT;
	}
	if(m_bDebug)
	{
		curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);
		curl_easy_setopt(curl, CURLOPT_DEBUGFUNCTION, OnDebug);
	}
/*<pre name="code" class="cpp">	*/
	curl_easy_setopt(curl, CURLOPT_URL, strUrl.c_str());

	curl_slist* lstHeader = NULL;
	if(!strToken.empty())
	{
		ostringstream os;
		os<<"Authorization: Bearer "<<strToken.c_str();
		lstHeader = curl_slist_append(lstHeader, os.str().c_str());
	}
	lstHeader = curl_slist_append(lstHeader,"Connection: keep-alive");
	lstHeader = curl_slist_append(lstHeader,"Accept: application/json, text/javascript, */*; q=0.01");
	lstHeader = curl_slist_append(lstHeader,"X-Requested-With: XMLHttpRequest");
	lstHeader = curl_slist_append(lstHeader,"User-Agent: Mozilla/5.0 (Windows NT 6.1; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/51.0.2704.103 Safari/537.36");
	lstHeader = curl_slist_append(lstHeader,"Content-Type: application/json;charset=UTF-8");
	lstHeader = curl_slist_append(lstHeader,"Accept-Encoding: gzip, deflate");
	lstHeader = curl_slist_append(lstHeader,"Accept-Language: zh-CN,zh;q=0.8");

	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, lstHeader);
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

int CHttpClient::Get(const std::tstring & strUrl,const std::tstring & strToken, std::tstring & strResponse)
{
	CLogger::GetInstance()->PrintLog(LOG_DEBUG, "Http Get Req: %s", strUrl.c_str());
	ostringstream os;
	os<<"Authorization: Bearer "
		<<strToken.c_str();

	CURLcode res;
	CURL* curl = curl_easy_init();
	if(NULL == curl)
	{
		return CURLE_FAILED_INIT;
	}
	if(m_bDebug)
	{
		curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);
		curl_easy_setopt(curl, CURLOPT_DEBUGFUNCTION, OnDebug);
	}
	struct curl_slist *headers=NULL; /* init to NULL is important */
	headers = curl_slist_append(headers, os.str().c_str());
	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);//修改http头

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
	curl_slist_free_all(headers);
	curl_easy_cleanup(curl);
	CLogger::GetInstance()->PrintLog(LOG_DEBUG, "Http Get Req Response: %s", strResponse.c_str());
	return res;
}



int CHttpClient::Posts(const std::tstring & strUrl, const std::tstring & strPost, std::tstring & strResponse, const char * pCaPath)
{
	CURLcode res;
	CURL* curl = curl_easy_init();
	if(NULL == curl)
	{
		return CURLE_FAILED_INIT;
	}
	if(m_bDebug)
	{
		curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);
		curl_easy_setopt(curl, CURLOPT_DEBUGFUNCTION, OnDebug);
	}
	curl_easy_setopt(curl, CURLOPT_URL, strUrl.c_str());
	//curl_easy_setopt(curl, CURLOPT_PORT, RE_PORT);
	curl_easy_setopt(curl, CURLOPT_POST, 1);
	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, strPost.c_str());
	curl_easy_setopt(curl, CURLOPT_READFUNCTION, NULL);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, OnWriteData);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&strResponse);
	curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
	if(NULL == pCaPath)
	{
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, false);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, false);
	}
	else
	{
		//缺省情况就是PEM，所以无需设置，另外支持DER
		//curl_easy_setopt(curl,CURLOPT_SSLCERTTYPE,"PEM");
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, true);
		curl_easy_setopt(curl, CURLOPT_CAINFO, pCaPath);
	}
	curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 3);
	curl_easy_setopt(curl, CURLOPT_TIMEOUT, 3);
	res = curl_easy_perform(curl);
	curl_easy_cleanup(curl);
	return res;
}

int CHttpClient::Gets(const std::tstring & strUrl, std::tstring & strResponse, const char * pCaPath)
{
	CURLcode res;
	CURL* curl = curl_easy_init();
	if(NULL == curl)
	{
		return CURLE_FAILED_INIT;
	}
	if(m_bDebug)
	{
		curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);
		curl_easy_setopt(curl, CURLOPT_DEBUGFUNCTION, OnDebug);
	}
	curl_easy_setopt(curl, CURLOPT_URL, strUrl.c_str());
	//curl_easy_setopt(curl, CURLOPT_PORT, RE_PORT);
	curl_easy_setopt(curl, CURLOPT_READFUNCTION, NULL);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, OnWriteData);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&strResponse);
	curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
	if(NULL == pCaPath)
	{
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, false);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, false);
	}
	else
	{
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, true);
		curl_easy_setopt(curl, CURLOPT_CAINFO, pCaPath);
	}
	curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 3);
	curl_easy_setopt(curl, CURLOPT_TIMEOUT, 3);
	res = curl_easy_perform(curl);
	curl_easy_cleanup(curl);
	return res;
}

///////////////////////////////////////////////////////////////////////////////////////////////

void CHttpClient::SetDebug(bool bDebug)
{
	m_bDebug = bDebug;
}
// </pre><p></p>
// <pre></pre>
// <br>
// <br>
// <p></p>
// <br>
   




//#include "StdAfx.h"
//#include "HttpClient.h"
//#include "Request.h"
//
//CHttpClient::CHttpClient()
//{
//
//}
//
//CHttpClient::~CHttpClient()
//{
//
//}
//
//int CHttpClient::SendRequest(tstring strSrvIp, tstring sHeaderSend, 
//							 tstring sHeaderReceive, tstring sMessage, bool bIsPost)
//{
//	//初始化类   
//	Request myRequest;    
//	int i =myRequest.SendRequest(bIsPost, strSrvIp, sHeaderSend,   
//								sHeaderReceive, sMessage);   
//	if (i)   
//	{      
//		/*cout<<"Http头:"<<endl;   
//		cout<< sHeaderSend <<endl;   
//		cout<<"响应头"<<endl;   
//		cout<< sHeaderReceive <<endl;   
//		cout<<"网页内容"<<endl;   
//		cout<< sMessage <<endl;*/   
//	}else   
//	{   
//		//cout<<"网络不可到达"<<endl;   
//	}   
//
//	return i;
//}