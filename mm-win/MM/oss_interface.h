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
	std::tstring m_strAcessID;
	std::tstring m_strAcessIDSecret;
	std::tstring m_strEndpoint;
	std::tstring m_strBucketName;
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
	std::tstring getAcessID();
	std::tstring getAcessIDSecret();
	std::tstring getEndpoint();
	std::tstring getBucketName();

	int getossdomain();
	int getossport();
	void setossdomain(int);
	void setossport(int);
	void setAcessID(std::tstring&);
	void setAcessIDSecret(std::tstring&);
	void setAcessEndpoint(std::tstring&);
	void setBucketName(std::tstring&);
	int createBucket(std::tstring& strbucketName, std::tstring& strEndPoint,oss_acl_e acl);
	int uploadObjectFromFile(std::tstring& strObjectName, std::tstring& strFilePath);
	int downloadObjectByUrl(std::tstring & strUrl, std::tstring& strFilePath, int64_t filesize, std::tstring &fileFullName, std::tstring& strKey, std::tstring& strOpposite);   //todo

	int listFilesFromBucket(std::vector<std::tstring> & vctFiles);
	std::tstring getFileUrl(std::tstring &strObjectName);
	int uploadPartFromFile(std::tstring& strObject, std::tstring& strFilePath, int64_t filesize, std::tstring& strKey, std::tstring& strOpposite);
	int abord_upload(std::tstring &strObject, std::tstring &strUpload_id);
	int uploadPartFromFileEx(std::tstring& strObject, std::tstring& strFilePath, int64_t filesize, std::tstring& strKey, std::tstring& strOpposite);
};