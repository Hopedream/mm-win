#include "oss_interface.h"
#include "UIDefine.h"
#include <stdlib.h>
#include <atlstr.h>
#include "../Common/Logger.h"

HANDLE ossInterface::m_hEventExit = NULL;
HANDLE ossInterface::m_hThreadExit=NULL;
ossInterface::ossInterface():
	m_strAcessID(_T("")),
	m_strAcessIDSecret(_T("")),
	m_strEndpoint(_T("")),
	m_iossdomain(1),
	m_iossport(80),
	m_strBucketName(_T("")),
	m_pOptions(NULL),
	m_pPool(NULL)
{
	m_hEventExit = CreateEvent(NULL,TRUE,FALSE,NULL);
	ResetEvent(m_hEventExit);
	m_hThreadExit = CreateEvent(NULL,TRUE,FALSE,NULL);
	ResetEvent(m_hThreadExit);

	m_hEventStop = CreateEvent(NULL, TRUE, FALSE, NULL);
	ResetEvent(m_hEventStop);

	::InitializeCriticalSection(&m_cs);
}

ossInterface::~ossInterface()
{
	::DeleteCriticalSection(&m_cs);
	
}

void ossInterface::InitGlobal()
{
	if (aos_http_io_initialize("MM", 0) != AOSE_OK) {
		exit(1);
	}
}

void ossInterface::initOSS()
{
	/*if (aos_http_io_initialize("MM", 0) != AOSE_OK) {
	exit(1);
	}*/

	::EnterCriticalSection(&m_cs);
	aos_pool_create(&m_pPool, NULL);

	m_pOptions = oss_request_options_create(m_pPool);
	m_pOptions->config = oss_config_create(m_pOptions->pool);
	aos_str_set(&m_pOptions->config->host, m_strEndpoint.data());   //"endpoint"
	m_pOptions->config->port = m_iossport;
	aos_str_set(&m_pOptions->config->id, m_strAcessID.data());       //  acesskeyid
	aos_str_set(&m_pOptions->config->key, m_strAcessIDSecret.data());        // acesskeysecret
	m_pOptions->config->is_oss_domain = m_iossdomain;
	m_pOptions->ctl = aos_http_controller_create(m_pOptions->pool, 0);

	::LeaveCriticalSection(&m_cs);
}

void ossInterface::deinitializeOSS()
{
	aos_http_io_deinitialize();
}

std::tstring ossInterface::getEndpoint()
{
	return m_strEndpoint;
}

std::tstring ossInterface::getBucketName()
{
	return m_strBucketName;
}

int ossInterface::getossdomain()
{
	return m_iossdomain;
}

int ossInterface::getossport()
{
	return m_iossport;
}

void ossInterface::setossdomain( int idomain)
{
	m_iossdomain = idomain;
}

void ossInterface::setossport( int iPort )
{
	m_iossport = iPort;
}

void ossInterface::setAcessID( std::tstring& strAcessID)
{
	m_strAcessID = strAcessID;
}

void ossInterface::setAcessIDSecret( std::tstring& strAcessIDSecret)
{
	m_strAcessIDSecret = strAcessIDSecret;
}

void ossInterface::setAcessEndpoint( std::tstring& strEndpoint)
{
	m_strEndpoint = strEndpoint;
}

void ossInterface::setBucketName( std::tstring& strBucketName)
{
	m_strBucketName = strBucketName;
}

std::tstring ossInterface::getAcessID()
{
	return m_strAcessID;
}

std::tstring ossInterface::getAcessIDSecret()
{
	return m_strAcessIDSecret;
}

int ossInterface::uploadObjectFromFile( std::tstring& strObjectName, std::tstring& strFilePath )
{
	aos_status_t *s;
	aos_table_t *headers;
	aos_table_t *resp_headers;
	aos_string_t bucket;
	aos_string_t object;
	aos_string_t file;

	aos_str_set(&bucket, m_strBucketName.data());
	aos_str_set(&object, strObjectName.data());
	aos_str_set(&file, strFilePath.data());
	headers = aos_table_make(m_pPool, 0);
	s = oss_put_object_from_file(m_pOptions, &bucket, &object, &file, headers, &resp_headers);
	return s->code;
}

int ossInterface::downloadObjectByUrl(std::tstring & strUrl, std::tstring& strFilePath, int64_t filesize, std::tstring &fileFullName, std::tstring& strKey, std::tstring& strOpposite)
{
	double iProcess = 0;
	aos_status_t *s;
	aos_table_t *headers;
	aos_table_t *resp_headers;
	aos_string_t bucket;
	aos_string_t object;
	aos_string_t file;
	aos_string_t strurl;
	int64_t ibegin = -1;
	int64_t iend = -1;
	int64_t iPart = 200 * 1024;
	aos_str_set(&bucket, m_strBucketName.data());
	aos_str_set(&strurl, strUrl.data());
	aos_str_set(&file, strFilePath.data());
	headers = aos_table_make(m_pPool, 1);
	do 
	{
		ibegin = iend + 1;
		iend += iPart;
		char strbegin[16];
		char strend[16];
		sprintf(strbegin,"%d",ibegin);
		sprintf(strend, "%d", iend);
		tstring strRange = "bytes=" + tstring(strbegin) + '-'+ tstring(strend);
		apr_table_set(headers, "Range", strRange.c_str());
		s = oss_get_object_to_file_by_url(m_pOptions, &strurl, headers, &file, &resp_headers);

		double ivalue = double(iPart * 100) / double(filesize);
		iProcess += ivalue;
		for (list<IOssEventBase*>::iterator it = m_listHandlers.begin(); 
			it != m_listHandlers.end(); ++it)
		{
			IOssEventBase* pHandler = (IOssEventBase*)(*it);
			if (NULL != pHandler)
			{
				if (iProcess >= 100)
					iProcess = 98;
				pHandler->OnProcess(strKey, iProcess, strOpposite);
			}
		}

	} while (s->code == 206);    //没有下载完	
	if (s->code == OSS_SUCCESS)
	{
		for (list<IOssEventBase*>::iterator it = m_listHandlers.begin(); 
			it != m_listHandlers.end(); ++it)
		{
			IOssEventBase* pHandler = (IOssEventBase*)(*it);
			if (NULL != pHandler)
			{
				pHandler->OnProcess(strKey, 99, strOpposite);
			}
		}
	}
	return s->code;
}

std::tstring ossInterface::getFileUrl( std::tstring &strObjectName )
{
	aos_http_request_t *req;
	std::tstring url_str;

	aos_string_t bucket;
	aos_string_t object;
	apr_time_t now;
	int64_t expire_time;
	//int64_t one_hour = 3600;

	aos_str_set(&bucket, m_strBucketName.data());
	aos_str_set(&object, strObjectName.data());
	req = aos_http_request_create(m_pPool);
	req->method = HTTP_GET;
	now = apr_time_now(); //millisecond
	expire_time = now / 1000000 + 315360000;  //10 years
	url_str = oss_gen_signed_url(m_pOptions, &bucket, &object, expire_time, req);
	return url_str;
}

void ossInterface::AddEventHandler( IOssEventBase* pHandler )
{
	if (NULL != pHandler)
	{
		m_listHandlers.push_back(pHandler);
	}
}

void ossInterface::RemoveHandler(IOssEventBase* pHandler)
{
	for(list<IOssEventBase*>::iterator it = m_listHandlers.begin(); 
		it != m_listHandlers.end(); ++it)
	{
		if (pHandler == *it)
		{
			m_listHandlers.erase(it);
			break;
		}
	}
}

void ossInterface::StopCurrentTask()
{
	SetEvent(m_hEventStop);
	Sleep(100);
}

int ossInterface::uploadPartFromFile( std::tstring& strObject, std::tstring& strFilePath, int64_t filesize, std::tstring& strKey, std::tstring& strOpposite)
{
	aos_status_t *s;
	aos_table_t *headers;
	aos_table_t *resp_headers;
	aos_string_t bucket;
	aos_string_t object;
	//aos_string_t file;
	oss_upload_file_t *upload_file;
	int64_t part_size = 200*1024;
	aos_string_t upload_id;
	aos_list_t complete_part_list;
	oss_complete_part_content_t *complete_content;
	int part_num = 1;
	char *part_num_str; 
	char *etag; 
	double iProcess = 0;

	aos_str_set(&bucket, m_strBucketName.data());
	aos_str_set(&object, strObject.data());
	aos_str_null(&upload_id);
	aos_list_init(&complete_part_list); 
	headers = aos_table_make(m_pPool, 0);

	//初始化分段上传
	s = oss_init_multipart_upload(m_pOptions, &bucket, &object, headers, &upload_id, &resp_headers);
	if (s->code != OSS_SUCCESS)
	{
		for (list<IOssEventBase*>::iterator it = m_listHandlers.begin(); 
			it != m_listHandlers.end(); ++it)
		{
			IOssEventBase* pHandler = (IOssEventBase*)(*it);
			if (NULL != pHandler)
			{
				pHandler->OnProcess(strKey, -1, strOpposite);
			}
		}
		return s->code;
	}
	upload_file = oss_create_upload_file(m_pPool);
	aos_str_set(&upload_file->filename, _T(strFilePath.c_str()));
	upload_file->file_pos = 0;
	upload_file->file_last = 0;
	while(1)
	{
		if(WaitForSingleObject(m_hEventExit,0) == WAIT_OBJECT_0)
		{
			return -1;
		}
		upload_file->file_pos = upload_file->file_last;
		upload_file->file_last +=  part_size;
		s = oss_upload_part_from_file(m_pOptions, &bucket, &object, &upload_id, part_num, upload_file, &resp_headers);
		if (s->code != OSS_SUCCESS)
		{
			for (list<IOssEventBase*>::iterator it = m_listHandlers.begin(); 
				it != m_listHandlers.end(); ++it)
			{
				IOssEventBase* pHandler = (IOssEventBase*)(*it);
				if (NULL != pHandler)
				{
					pHandler->OnProcess(strKey, -1, strOpposite);
				}
			}
			return s->code;
		}
		double ivalue = double(part_size * 100) / double(filesize);
		iProcess += ivalue;
		for (list<IOssEventBase*>::iterator it = m_listHandlers.begin(); 
			it != m_listHandlers.end(); ++it)
		{
			IOssEventBase* pHandler = (IOssEventBase*)(*it);
			if (NULL != pHandler)
			{
				if (iProcess >= 100)
					iProcess = 98;
				pHandler->OnProcess(strKey, iProcess, strOpposite);
				CString cstrLog;
				cstrLog.Format("%s----------OnProcess res = %d", strObject.c_str(), iProcess);
				CLogger::GetInstance()->PrintLog(cstrLog, ENMLOGTYPE::LOG_INFO);
			}
		}
		//获取每段上传以后的列表
		complete_content = oss_create_complete_part_content(m_pPool); 
		part_num_str = apr_psprintf(m_pPool, "%d", part_num); 
		aos_str_set(&complete_content->part_number, part_num_str); 
		etag = apr_pstrdup(m_pPool, (char*)apr_table_get(resp_headers, "ETag")); 
		aos_str_set(&complete_content->etag, etag); 
		aos_list_add_tail(&complete_content->node, &complete_part_list); 
		if (upload_file->file_last >= filesize)
			break;
		part_num ++;
	}
	//完成分段上传
	s = oss_complete_multipart_upload(m_pOptions, &bucket, &object, &upload_id, &complete_part_list, &resp_headers);
	if (s->code != OSS_SUCCESS)
	{
		for (list<IOssEventBase*>::iterator it = m_listHandlers.begin(); 
			it != m_listHandlers.end(); ++it)
		{
			IOssEventBase* pHandler = (IOssEventBase*)(*it);
			if (NULL != pHandler)
			{
				pHandler->OnProcess(strKey, -1, strOpposite);
			}
		}
	}
	return s->code;
}

void ossInterface::StopTask()
{
	SetEvent(m_hEventExit);
}

int ossInterface::abord_upload( std::tstring &strObject, std::tstring &strUpload_id )
{
	aos_status_t *s;
	aos_table_t *resp_headers;
	aos_string_t bucket;
	aos_string_t object;
	aos_string_t upload_id;
	aos_str_set(&bucket, m_strBucketName.data());
	aos_str_set(&object, strObject.data());
	aos_str_set(&upload_id, strUpload_id.data());
	s = oss_abort_multipart_upload(m_pOptions, &bucket, &object, &upload_id, &resp_headers);
	return s->code;
}
int ossInterface::uploadPartFromFileEx(std::tstring& strObject, std::tstring& strFilePath, int64_t filesize, std::tstring& strKey, std::tstring& strOpposite)
{
	aos_status_t *s;
	aos_table_t *headers;
	aos_table_t *resp_headers;
	aos_string_t bucket;
	aos_string_t object;
	//aos_string_t file;
	oss_upload_file_t *upload_file;
	int64_t part_size = 200*1024;
	aos_string_t upload_id;
	aos_list_t complete_part_list;
	oss_complete_part_content_t *complete_content;
	int part_num = 1;
	char *part_num_str; 
	char *etag; 
	double iProcess = 0;

	aos_str_set(&bucket, m_strBucketName.data());
	aos_str_set(&object, strObject.data());
	aos_str_null(&upload_id);
	aos_list_init(&complete_part_list); 
	headers = aos_table_make(m_pPool, 0);

	//初始化分段上传
	s = oss_init_multipart_upload(m_pOptions, &bucket, &object, headers, &upload_id, &resp_headers);
	if (s->code != OSS_SUCCESS)
	{
		for (list<IOssEventBase*>::iterator it = m_listHandlers.begin(); 
			it != m_listHandlers.end(); ++it)
		{
			IOssEventBase* pHandler = (IOssEventBase*)(*it);
			if (NULL != pHandler)
			{
				pHandler->OnProcess(strKey, -1, strOpposite);
			}
		}
		return s->code;
	}
	upload_file = oss_create_upload_file(m_pPool);
	aos_str_set(&upload_file->filename, _T(strFilePath.c_str()));
	upload_file->file_pos = 0;
	upload_file->file_last = 0;
	while(1)
	{
		if (WaitForSingleObject(m_hEventStop, 0) == WAIT_OBJECT_0)
		{
			//break;
			return -1;
		}
		upload_file->file_pos = upload_file->file_last;
		upload_file->file_last +=  part_size;
		s = oss_upload_part_from_file(m_pOptions, &bucket, &object, &upload_id, part_num, upload_file, &resp_headers);
		if (s->code != OSS_SUCCESS)
		{
			for (list<IOssEventBase*>::iterator it = m_listHandlers.begin(); 
				it != m_listHandlers.end(); ++it)
			{
				IOssEventBase* pHandler = (IOssEventBase*)(*it);
				if (NULL != pHandler)
				{
					pHandler->OnProcess(strKey, -1, strOpposite);
				}
			}
			return s->code;
		}
		double ivalue = double(part_size * 100) / double(filesize);
		iProcess += ivalue;
		for (list<IOssEventBase*>::iterator it = m_listHandlers.begin(); 
			it != m_listHandlers.end(); ++it)
		{
			IOssEventBase* pHandler = (IOssEventBase*)(*it);
			if (NULL != pHandler)
			{
				if (iProcess >= 100)
					iProcess = 98;
				pHandler->OnProcess(strKey, iProcess, strOpposite);
				CString cstrLog;
				cstrLog.Format("%s----------OnProcess res = %d", strObject.c_str(), iProcess);
				CLogger::GetInstance()->PrintLog(cstrLog, ENMLOGTYPE::LOG_INFO);
			}
		}
		//获取每段上传以后的列表
		complete_content = oss_create_complete_part_content(m_pPool); 
		part_num_str = apr_psprintf(m_pPool, "%d", part_num); 
		aos_str_set(&complete_content->part_number, part_num_str); 
		etag = apr_pstrdup(m_pPool, (char*)apr_table_get(resp_headers, "ETag")); 
		aos_str_set(&complete_content->etag, etag); 
		aos_list_add_tail(&complete_content->node, &complete_part_list); 
		if (upload_file->file_last >= filesize)
			break;
		part_num ++;
	}
	//完成分段上传
	s = oss_complete_multipart_upload(m_pOptions, &bucket, &object, &upload_id, &complete_part_list, &resp_headers);
	if (s->code != OSS_SUCCESS)
	{
		for (list<IOssEventBase*>::iterator it = m_listHandlers.begin(); 
			it != m_listHandlers.end(); ++it)
		{
			IOssEventBase* pHandler = (IOssEventBase*)(*it);
			if (NULL != pHandler)
			{
				pHandler->OnProcess(strKey, -1, strOpposite);
			}
		}
	}
	return s->code;
}
