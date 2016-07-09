#pragma once
#include <string>  
namespace std{
#ifdef _UNICODE
	typedef wstring tstring;
#else
	typedef string tstring;
#endif
}  
class CHttpClient  
{  
public:  
    CHttpClient(void);  
    ~CHttpClient(void);  
  
public:  
	int Post(const std::tstring & strUrl, const std::tstring & strPost, 
		const std::tstring & strToken, std::tstring & strResponse);
    
	int Get(const std::tstring & strUrl,const std::tstring & strToken, 
		std::tstring & strResponse);

	/** 
    * @brief HTTP POST请求 
    * @param strUrl 输入参数,请求的Url地址,如:http://www.baidu.com 
    * @param strPost 输入参数,使用如下格式para1=val1¶2=val2&… 
    * @param strResponse 输出参数,返回的内容 
    * @return 返回是否Post成功 
    */  
    int Post(const std::tstring & strUrl, const std::tstring & strPost, std::tstring & strResponse);  
  
    int CSSPost(const std::tstring & strUrl, const std::tstring & strPost, std::tstring & strResponse);  
    /** 
    * @brief HTTP GET请求 
    * @param strUrl 输入参数,请求的Url地址,如:http://www.baidu.com 
    * @param strResponse 输出参数,返回的内容 
    * @return 返回是否Post成功 
    */  
    int Get(const std::tstring & strUrl, std::tstring & strResponse);  
  
	int CSSGet(const std::tstring & strUrl, std::tstring & strResponse,const std::tstring strToken="");  
    /** 
    * @brief HTTPS POST请求,无证书版本 
    * @param strUrl 输入参数,请求的Url地址,如:https://www.alipay.com 
    * @param strPost 输入参数,使用如下格式para1=val1¶2=val2&… 
    * @param strResponse 输出参数,返回的内容 
    * @param pCaPath 输入参数,为CA证书的路径.如果输入为NULL,则不验证服务器端证书的有效性. 
    * @return 返回是否Post成功 
    */  
    int Posts(const std::tstring & strUrl, const std::tstring & strPost, std::tstring & strResponse, const char * pCaPath = NULL);  
  
    /** 
    * @brief HTTPS GET请求,无证书版本 
    * @param strUrl 输入参数,请求的Url地址,如:https://www.alipay.com 
    * @param strResponse 输出参数,返回的内容 
    * @param pCaPath 输入参数,为CA证书的路径.如果输入为NULL,则不验证服务器端证书的有效性. 
    * @return 返回是否Post成功 
    */  
    int Gets(const std::tstring & strUrl, std::tstring & strResponse, const char * pCaPath = NULL);  
  
public:  
    void SetDebug(bool bDebug);  


  
private:  
    bool m_bDebug;  
};  





//class CHttpClient
//{
//public:
//	CHttpClient();
//	~CHttpClient();
//
//	//sHeaderSend		定义http头   
//	//sHeaderReceive	返回头   
//	//sMessage			返回页面内容   
//	//bIsPost			是否Post提交   
//	int SendRequest(tstring strSrvIp, tstring sHeaderSend, 
//					tstring sHeaderReceive, tstring sMessage, bool bIsPost);
//};