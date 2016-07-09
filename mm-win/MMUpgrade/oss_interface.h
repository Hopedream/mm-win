#pragma once
#include "aos_log.h"
#include "aos_util.h"
#include "aos_string.h"
#include "aos_status.h"
#include "oss_auth.h"
#include "oss_util.h"
#include "oss_api.h"
#include "aos_list.h"
#include "oss_define.h"
#include <xstring>
#include <vector>
#include <list>
#include "oss_event.h"
using namespace std;
//extern const int OSS_MAX_PART_NUM;

#define OSS_PER_RET_NUM 1000 
#define OSS_SUCCESS 200

#define aos_status_copy(src, dst, p)\
(dst) = aos_status_create(p);\
	(dst)->code = (src)->code;\
	(dst)->error_code = apr_pstrdup(p, (src)->error_code);\
	(dst)->error_msg = apr_pstrdup(p, (src)->error_msg); 


typedef struct { 
	int part_num; 
	char *etag; 
} oss_upload_part_t;



//int part_sort_cmp(const void *a, const void *b) 
//{ 
//	return (((oss_upload_part_t*)a)->part_num - ((oss_upload_part_t*)b)->part_num > 0 ? 1 : -1); 
//} 

/*int part_sort_cmp(const void *a, const void *b) 
{ 
	return (((oss_upload_part_t*)a)->part_num - ((oss_upload_part_t*)b)->part_num > 0 ? 1 : -1); 
}*/ 


class ossInterface
{
public:
	static HANDLE m_hThreadExit;
private:
	static HANDLE m_hEventExit;

	HANDLE m_hEventStop;
	CRITICAL_SECTION m_cs;
	std::string m_strAcessID;
	std::string m_strAcessIDSecret;
	std::string m_strEndpoint;
	std::string m_strBucketName;
	int m_iossdomain;
	int m_iossport;
	oss_request_options_t *m_pOptions;
	aos_pool_t            *m_pPool;
	list<IOssEventBase*> m_listHandlers;

public:
	ossInterface();
	~ossInterface();
public:
	void AddEventHandler(IOssEventBase* pHandler);
	void RemoveHandler(IOssEventBase* pHandler);

	static void InitGlobal();
	static void deinitializeOSS();   //


	void initOSS();         //≥ı ºªØoss
	static void StopTask();
	void StopCurrentTask();
	std::string getAcessID();
	std::string getAcessIDSecret();
	std::string getEndpoint();
	std::string getBucketName();

	int getossdomain();
	int getossport();
	void setossdomain(int);
	void setossport(int);
	void setAcessID(std::string&);
	void setAcessIDSecret(std::string&);
	void setAcessEndpoint(std::string&);
	void setBucketName(std::string&);
	int createBucket(std::string& strbucketName, std::string& strEndPoint,oss_acl_e acl);
	int uploadObjectFromFile(std::string& strObjectName, std::string& strFilePath);
	int downloadObjectByUrl(std::string & strUrl, std::string& strFilePath, int64_t filesize, std::string &fileFullName, std::string& strKey, std::string& strOpposite);   //todo

	int listFilesFromBucket(std::vector<std::string> & vctFiles);
	std::string getFileUrl(std::string &strObjectName);
	int uploadPartFromFile(std::string& strObject, std::string& strFilePath, int64_t filesize, std::string& strKey, std::string& strOpposite);
	int abord_upload(std::string &strObject, std::string &strUpload_id);
	int uploadPartFromFileEx(std::string& strObject, std::string& strFilePath, int64_t filesize, std::string& strKey, std::string& strOpposite);
};