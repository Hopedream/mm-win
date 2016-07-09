#include "StdAfx.h"
#include "imdb.h"
#include <ctime>
#include "FileHelper.h"
#include "Utility.h"

#define CLIENT_VERSION 1.0f  
#define DB_TABLE_VERSION  _T("Version_DB")
#define DBVERSION_CREATE_TABLE  _T("create table Version_DB (version_data int);")
#define DB_VERSION_INSERT_ITEM  _T("insert into Version_DB values (%f);")

#define TEXT_SLASH   _T("\\")
#define BEGIN_TRANSACTION  _T("begin transaction;")
#define COMMIT_TRANSACTION  _T("commit transaction;")

#define MSG_FLLENAME    _T("msg.db")   
#define MSG_TABLE_HEAD   _T("UID_")
#define MSG_GROUP_TABLE_HEAD _T("ROOM_")

/*#define MSG_CREATE_TABLE   _T("create table %s (type, flags, uid,db_when ,stime ,msgtext,msgSeq,sessionid,oldSessionid,param0 ,param1,fontName,fontSize,fontColor,fontflags,delay,keynum INTEGER PRIMARY KEY)")*/
#define MSG_CREATE_TABLE   _T("CREATE TABLE %s ( uid TEXT, msgText TEXT, stime TEXT);")
#define MSG_GROUP_CREATE_TABLE _T("CREATE TABLE %s ( roomId TEXT, senderPhone TEXT, senderAccount TEXT, msgText TEXT, stime TEXT);")
//#define MSG_INSERT_ITEM  _T("insert into %s  values (%d ,%d,%d,%d,'%s',%d,%lf,%lf,%d,%d,'%s',%d,%d,%d,%d);")
#define MSG_INSERT_ITEM  _T("insert into %s  values (%d ,%d,%ld,%d,%d,'%s',%d,%s,%d,%d,'%s',%d,%d,%d,%d);")

//系统消息
#define MSG_SYS_TABLE _T("SYS_MSG")
#define MSG_CREATE_SYSMSG_TABLE _T("create table %s (eSubType INTEGER, sTime INTEGER, msgContent TEXT, bProcessed INTEGER, exInfo TEXT);")


//存联系人信息
#define CONTACTER_FLLENAME    _T("contacts.db")
#define CONTACTER_TABLE       _T("CONTACTER_INFO")
#define CONTACTER_EXTEND_TABLE       _T("CONTACTER_EXTEND_INFO")

/*
+---------------+---------------------+------+-----+---------+-------+
| Field         | Type                | Null | Key | Default | Extra |
+---------------+---------------------+------+-----+---------+-------+
| Fleft_uid     | bigint(64) unsigned | NO   | PRI | 0       |       | 
| Fbind_account | varchar(64)         | YES  |     | NULL    |       | 
| Freal_name    | varchar(64)         | YES  |     | NULL    |       | 
| Fsex          | tinyint(3) unsigned | YES  |     | 255     |       | 
| Fstate        | tinyint(3)          | YES  |     | 0       |       | 
| Fvip          | tinyint(3)          | YES  |     | 0       |       | 
| Fhead_pic     | int(11) unsigned    | YES  |     | 0       |       | 
| Fblock_set    | tinyint(3) unsigned | YES  |     | 0       |       | 
| Fsigned       | varchar(255)        | YES  |     | NULL    |       | 
| good_uin      | int(11) unsigned    | NO   |     | NULL    |       | 
| Fversion      | int(11) unsigned    | YES  |     | 1       |       | 
| Fsavemsg      | tinyint(3)          | YES  |     | 0       |       | 
| Fservice_mask | int(11)             | YES  |     | 0       |       | 
+---------------+---------------------+------+-----+---------+-------+
*/
// 
// typedef struct
// {
// 	tstring strUserName;
// 	tstring strUserNickname;
// 	tstring strGender;		//性别
// 	tstring strSignature;	//签名
// 	tstring strPhone;
// 	tstring strEmail;
// 	tstring strAddr;
// 	tstring strPhotoType;	//头像类型
// 	tstring strPhotoBinary;	//头像base64
// 	tstring strUrl;			//头像的Url地址
// 	tstring strPhotoPath;	//在线头像本地路径
// 	tstring strGrayAvatar;	//离线头像本地路径
// }sUserVCard;
#define CONTACTER_CREATE_TABLE  _T("CREATE TABLE CONTACTER_INFO ( uid TEXT, nick TEXT, remark TEXT, gender TEXT, signature TEXT, phone TEXT, email TEXT, address TEXT, photoType TEXT, photoBinary TEXT, photoUrl TEXT,photoPath TEXT,photoGrayPath TEXT);")
//#define CONTACTER_CREATE_TABLE  _T("create table CONTACTER_INFO (Fleft_uid BIGINT PRIMARY KEY,Fbind_account TEXT,Freal_name TEXT,Fsex INTEGER,Fstate INTEGER,Fvip INTEGER,Fhead_pic INTEGER,Fblock_set INTEGER,Fsigned TEXT,good_uin BIGINT,Fversion INTEGER,Fsavemsg INTEGER,Fservice_mask INTEGER);") 
#define CONTACT_INFO_INSERT_ITEM  _T("insert into CONTACTER_INFO  values ( ")



/*
+-----------------+---------------------+------+-----+---------+-------+
| Field           | Type                | Null | Key | Default | Extra |
+-----------------+---------------------+------+-----+---------+-------+
| Fleft_uid       | bigint(64)          | NO   | PRI | 0       |       | 
| Fpublic_flag    | tinyint(3)          | YES  |     | 0       |       | 
| Femail          | varchar(128)        | YES  |     | NULL    |       | 
| Fmoblie         | varchar(16)         | YES  |     | NULL    |       | 
| Fphone          | varchar(32)         | YES  |     | NULL    |       | 
| Flast_school    | varchar(64)         | YES  |     | NULL    |       | 
| Fcareer         | varchar(64)         | YES  |     | NULL    |       | 
| Flast_company   | varchar(64)         | YES  |     | NULL    |       | 
| Flast_province  | varchar(32)         | YES  |     | NULL    |       | 
| Flast_city      | varchar(32)         | YES  |     | NULL    |       | 
| Flast_town      | varchar(32)         | YES  |     | NULL    |       | 
| Fhome_province  | varchar(32)         | YES  |     | NULL    |       | 
| Fhome_city      | varchar(32)         | YES  |     | NULL    |       | 
| Fhome_town      | varchar(32)         | YES  |     | NULL    |       | 
| Fage            | tinyint(3) unsigned | YES  |     | 255     |       | 
| Fbirthday_month | tinyint(3) unsigned | YES  |     | 255     |       | 
| Fbirthday_date  | tinyint(3) unsigned | YES  |     | 255     |       | 
| Frefer_birthday | tinyint(3) unsigned | YES  |     | 255     |       | 
| Fhome_page      | varchar(128)        | YES  |     | NULL    |       | 
| Fauth_type      | tinyint(3)          | YES  |     | 0       |       | 
| Fquestion       | varchar(128)        | YES  |     | NULL    |       | 
| Fanswer         | varchar(128)        | YES  |     | NULL    |       | 
+-----------------+---------------------+------+-----+---------+-------+
22 rows in set (0.01 sec)
*/
#define CONTACTER_CREATE_EXTEND_TABLE  _T("create table CONTACTER_EXTEND_INFO (Fleft_uid BIGINT PRIMARY KEY,Fpublic_flag,Femail,Fmoblie,Flast_school,Fcareer,Flast_company,Flast_province,Flast_city,Fhome_town,Fage,Fbirthday_month,Fbirthday_date,Frefer_birthday,Fhome_page,Fauth_type,Fquestion,Fanswer);") 
#define CONTACTER_EXTEND_INFO_INSERT_ITEM   _T("insert into CONTACTER_EXTEND_INFO  values ( ")

#define CONTACTER_FACE_TABLE _T("CONTACTER_FACE")
#define CONTACTER_FACE_CREATE_TABLE _T("create table CONTACTER_FACE (Fleft_uid unique ,face blob);")

#define RECENTLIST_TABLE    _T("RECENTLIST_TABLE")
#define RECENTLIST_GROUP	_T("Recent_Group_Table")

#define RECENTLIST_CREATE_TABLE  _T("create table RECENTLIST_TABLE (uid TEXT, sTime TEXT);")
#define RECENTLIST_CREATE_GROUP_TABLE _T("create table Recent_Group_Table (roomId TEXT, orgId TEXT, orgName TEXT, account TEXT, groupNick TEXT, photoPath TEXT);")
//#define RECENTLIST_CREATE_TABLE  _T("create table RECENTLIST_TABLE (FFriend_uid BIGINT PRIMARY KEY,FUpdate_time INTEGER);")

//登陆框的选项：记住密码，自动登陆
#define LOGIN_PARA_TABLE	_T("LOGIN_PARAM_TABLE")
#define LOGIN_PARAM_TABLE_CREATE _T("create table LOGIN_PARAM_TABLE (uid TEXT, pwd TEXT, sTime INTEGER, bRemePwd INTEGER, bAutoLogin INTEGER);")

//发送消息快捷键方式
#define SEND_MSG_KEYMODE_TABLE	_T("SEND_MSG_KEYMODE_TABLE")
#define SEND_MSG_KEYMODE_TABLE_CREATE _T("create table SEND_MSG_KEYMODE_TABLE (uid TEXT, keymode INTEGER);")

#define CALL_HISTORY_TABLE    _T("CALLHISTORY_TABLE")
#define CALL_HISTORY_CREATE_TABLE  _T("create table CALLHISTORY_TABLE (uid TEXT, sTime INTEGER, dialIndicate INTEGER);")

//存groupindex和uin的关联
#define GROUP_UNION_UIN_TABLE   _T("GROUP_UNION_UIN")
#define GROUP_UNION_UIN_CREATE_TABLE  _T("create table GROUP_UNION_UIN (groupIndex ,uin ,PRIMARY KEY(groupIndex,uin));")
#define GROUP_UNION_UIN_INSERT_ITEM  _T("insert into GROUP_UNION_UIN  values (%d ,%l);")
#define RECENT_GROUP_UNION_UIN_TABLE   _T("RECENT_GROUP_UNION_UIN")
#define RECENT_GROUP_UNION_UIN_CREATE_TABLE  _T("create table RECENT_GROUP_UNION_UIN (groupIndex ,uin ,activetime ,PRIMARY KEY(groupIndex,uin));")
//存group信息
#define GROUP_FLLENAME    _T("groups.db")
#define GROUP_TABLE    _T("GROUP_LIST")
#define RECENT_GROUP_TABLE    _T("RECENT_GROUP_LIST")
#define GROUP_CREATE_TABLE  _T("create table GROUP_LIST (groupIndex  ,groupName  ,version );")
#define RECENT_GROUP_CREATE_TABLE  _T("create table RECENT_GROUP_LIST (groupIndex  ,groupName  ,activetime,version );")


#define QRMSG_FLLENAME    _T("reply.db")
#define QRMSG_TABLE    _T("QRMSG_TABLE")
#define QRMSG_CREATE_TABLE  _T("create table QRMSG_TABLE (qrItem);")
#define QRMSG_INSERT_ITEM  _T("insert into QRMSG_TABLE  values ('%s' );")

#define ARMSG_FLLENAME    _T("reply.db")
#define ARMSG_TABLE    _T("ARMSG_TABLE")
#define ARMSG_CREATE_TABLE  _T("create table ARMSG_TABLE (arItem);")
#define ARMSG_INSERT_ITEM  _T("insert into ARMSG_TABLE values ('%s' );")

#define UIN_INFO_FLLENAME    _T("user.db")
#define UIN_INFO_TABLE    _T("UIN_INFO_TABLE")
#define UIN_EXTEND_INFO_TABLE    _T("UIN_EXTEND_INFO_TABLE")

/*
+---------------+---------------------+------+-----+---------+-------+
| Field         | Type                | Null | Key | Default | Extra |
+---------------+---------------------+------+-----+---------+-------+
| Fleft_uid     | bigint(64) unsigned | NO   | PRI | 0       |       | 
| Fbind_account | varchar(64)         | YES  |     | NULL    |       | 
| Freal_name    | varchar(64)         | YES  |     | NULL    |       | 
| Fsex          | tinyint(3) unsigned | YES  |     | 255     |       | 
| Fstate        | tinyint(3)          | YES  |     | 0       |       | 
| Fvip          | tinyint(3)          | YES  |     | 0       |       | 
| Fhead_pic     | int(11) unsigned    | YES  |     | 0       |       | 
| Fblock_set    | tinyint(3) unsigned | YES  |     | 0       |       | 
| Fsigned       | varchar(255)        | YES  |     | NULL    |       | 
| good_uin      | int(11) unsigned    | NO   |     | NULL    |       | 
| Fversion      | int(11) unsigned    | YES  |     | 1       |       | 
| Fsavemsg      | tinyint(3)          | YES  |     | 0       |       | 
| Fservice_mask | int(11)             | YES  |     | 0       |       | 
+---------------+---------------------+------+-----+---------+-------+
#define CONTACTER_CREATE_TABLE  _T("create table CONTACTER_INFO (Fleft_uid unique,Fbind_account,Freal_name,Fsex,Fstate,Fvip,Fhead_pic,Fblock_set,Fsigned,Fsavemsg,Fversion,Fsavemsg,Fservice_mask);") 
*/

//#define UIN_INFO_CREATE_TABLE  _T("create table UIN_INFO_TABLE (uin INTEGER PRIMARY KEY ,account ,nick,password,face,infover,moto, gender,status, motto,faceurl,vip, passauther,region,level,groupver, frilistver, friinfover, lastUpdatetime, lastUpdateImagetime,Bigfaceurl);")
#define UIN_INFO_CREATE_TABLE  _T("create table UIN_INFO_TABLE (Fleft_uid BIGINT PRIMARY KEY,Fbind_account TEXT,Freal_name TEXT,Fsex INTEGER,Fstate INTEGER,Fvip INTEGER,Fhead_pic INTEGER,Fblock_set INTEGER,Fsigned TEXT,good_uin BIGINT,Fversion INTEGER,Fsavemsg INTEGER,Fservice_mask INTEGER);") //_T("create table UIN_INFO_TABLE (Fleft_uid INTEGER PRIMARY KEY,Fbind_account,Freal_name,Fsex,Fstate,Fvip,Fhead_pic,Fblock_set,Fsigned,Fsavemsg,Fversion,Fsavemsg,Fservice_mask);")
#define UIN_INFO_INSERT_ITEM  _T("insert into UIN_INFO_TABLE  values ( ")

/*
+-----------------+---------------------+------+-----+---------+-------+
| Field           | Type                | Null | Key | Default | Extra |
+-----------------+---------------------+------+-----+---------+-------+
| Fleft_uid       | bigint(64)          | NO   | PRI | 0       |       | 
| Fpublic_flag    | tinyint(3)          | YES  |     | 0       |       | 
| Femail          | varchar(128)        | YES  |     | NULL    |       | 
| Fmoblie         | varchar(16)         | YES  |     | NULL    |       | 
| Fphone          | varchar(32)         | YES  |     | NULL    |       | 
| Flast_school    | varchar(64)         | YES  |     | NULL    |       | 
| Fcareer         | varchar(64)         | YES  |     | NULL    |       | 
| Flast_company   | varchar(64)         | YES  |     | NULL    |       | 
| Flast_province  | varchar(32)         | YES  |     | NULL    |       | 
| Flast_city      | varchar(32)         | YES  |     | NULL    |       | 
| Flast_town      | varchar(32)         | YES  |     | NULL    |       | 
| Fhome_province  | varchar(32)         | YES  |     | NULL    |       | 
| Fhome_city      | varchar(32)         | YES  |     | NULL    |       | 
| Fhome_town      | varchar(32)         | YES  |     | NULL    |       | 
| Fage            | tinyint(3) unsigned | YES  |     | 255     |       | 
| Fbirthday_month | tinyint(3) unsigned | YES  |     | 255     |       | 
| Fbirthday_date  | tinyint(3) unsigned | YES  |     | 255     |       | 
| Frefer_birthday | tinyint(3) unsigned | YES  |     | 255     |       | 
| Fhome_page      | varchar(128)        | YES  |     | NULL    |       | 
| Fauth_type      | tinyint(3)          | YES  |     | 0       |       | 
| Fquestion       | varchar(128)        | YES  |     | NULL    |       | 
| Fanswer         | varchar(128)        | YES  |     | NULL    |       | 
+-----------------+---------------------+------+-----+---------+-------+
22 rows in set (0.01 sec)
#define CONTACTER_CREATE_EXTEND_TABLE  _T("create table CONTACTER_EXTEND_INFO (Fleft_uid INTEGER PRIMARY KEY,Fpublic_flag,Femail,Fmoblie,Flast_school,Fcareer,Flast_company,Flast_province,Flast_city,Fhome_town,Fage,Fbirthday_month,Fbirthday_date,Frefer_birthday,Fhome_page,Fauth_type,Fquestion,Fanswer);") 
*/
#define UIN_EXTEND_INFO_CREATE_TABLE  _T("create table UIN_EXTEND_INFO_TABLE (Fleft_uid INTEGER PRIMARY KEY,Fpublic_flag,Femail,Fmoblie,Flast_school,Fcareer,Flast_company,Flast_province,Flast_city,Fhome_town,Fage,Fbirthday_month,Fbirthday_date,Frefer_birthday,Fhome_page,Fauth_type,Fquestion,Fanswer);") 
#define UIN_EXTEND_INFO_INSERT_ITEM   _T("insert into UIN_EXTEND_INFO_TABLE  values ( ")

#define UIN_INFO_FACE_CREATE_TABLE _T("create table USER_FACE (uin unique ,face blob);")
#define UIN_INFO_FACE_TABLE    _T("USER_FACE")




#define LAST_USER_INSERT_ITEM  _T("insert into LAST_USER_TABLE  values ( 0,'thelastone' ) ")
#define LAST_USER_CREATE_TABLE _T("create table LAST_USER_TABLE (uin  ,thelastone );")
#define LAST_USER_TABLE    _T("LAST_USER_TABLE")

#define OPTIONAL_FLLENAME    _T("optional.db")
#define OPTIONAL_TABLE    _T("OPTIONAL_TABLE")
#define OPTIONAL_CREATE_TABLE  _T("create table OPTIONAL_TABLE (key unique ,value  );")
#define KEYVALUE_FLLENAME    _T("optional.db")
#define KEYVALUE_TABLE    _T("KEYVALUE_TABLE")
#define KEYVALUE_CREATE_TABLE  _T("create table KEYVALUE_TABLE (key  ,value ,type,PRIMARY KEY(key,type) );")
#define RECENT_GROUP_INDEX 500000


IMDB* IMDB::m_pInstance;

IMDB::IMDB(void)
{
	/*char temp[MAX_PATH+1];
	tstring tempstr;
	tstring::size_type index;
	IM_uint32 nSize =  ::GetModuleFileName(NULL,temp,MAX_PATH);
	m_ModulePath=temp;

	index = m_ModulePath.rfind (TEXT_SLASH);
	if ( index != tstring::npos )
	{
		m_strUserPath=m_ModulePath.substr(0,index);
		m_strUserPath += _T(TEXT_SLASH);
		m_strUserPath += "User\\";

		if (!PathFileExists(m_strUserPath.c_str()))
		{
			CreateDirectory(m_strUserPath.c_str(), NULL);
		}
	}*/
	m_ModulePath = CFileHelper::GetUserDir().GetData();
	m_strUserPath = m_ModulePath + "User\\";
	if (!PathFileExists(m_strUserPath.c_str()))
	{
		CreateDirectory(m_strUserPath.c_str(), NULL);
	}
}

IMDB::~IMDB(void)
{

}


IMDB* IMDB::GetInstance()
{
	if(m_pInstance)
	{
		return m_pInstance;
	}
	else
	{
		m_pInstance = new IMDB;
		if (m_pInstance)
		{
			return m_pInstance;
		}
		else
		{
			return NULL;
		}
	}
}

const TCHAR * IMDB::GetModulePath()
{
	return this->m_ModulePath.c_str();
}

void IMDB::ReleaseInstance()
{
	if(m_pInstance)
	{
		delete m_pInstance;
		m_pInstance = NULL;
	}
}


bool IMDB::SetDBPath(IM_UID uid)
{
	m_bFirstLogin = false;
	//char temp[MAX_PATH+1];
	//tstring tempstr;
	//tstring::size_type index;
	//IM_uint32 nSize =  ::GetModuleFileName(NULL,temp,MAX_PATH);
	//m_ModulePath=temp;

	//index = m_ModulePath.rfind (TEXT_SLASH);
	//m_ModulePath = CFileHelper::GetUserDir().GetData();
	//if ( index != tstring::npos )
	{
		//_stprintf(temp,_T("%s"),uid);
		//tstring uinstr(temp);
		tstring uinstr = uid;
		//m_ModuleUinPath=m_ModulePath.substr(0,index);
		//m_ModuleUinPath += _T(TEXT_SLASH);
		m_ModuleUinPath = m_ModulePath + "User\\";

		if (!PathFileExists(m_ModuleUinPath.c_str()))
		{
			CreateDirectory(m_ModuleUinPath.c_str(), NULL);
		}

		m_ModuleUinPath += uinstr + _T(TEXT_SLASH);
		if (!PathFileExists(m_ModuleUinPath.c_str()))
		{
			m_bFirstLogin = true;
			CreateDirectory(m_ModuleUinPath.c_str(), NULL);
		}

		m_ModuleUinPath += "DB\\";
		if (!PathFileExists(m_ModuleUinPath.c_str()))
		{
			CreateDirectory(m_ModuleUinPath.c_str(), NULL);
		}

		//m_ModulePath = m_ModulePath.substr(0,index) + _T(TEXT_SLASH);
		//CreateDirectory(m_ModuleUinPath.c_str(),NULL);
		return true;
	}
	return false;
}

bool IMDB::PrepareTableTalkMsg(CppSQLite3DB &db,tstring tablename)
{
	bool re;

	re = db.tableExists(DB_TABLE_VERSION);
	if(re == false)  //说明数据库是新建，生成库的基本表
	{
		db.execDML(DBVERSION_CREATE_TABLE);
		_stprintf(m_buf, DB_VERSION_INSERT_ITEM, CLIENT_VERSION);
		db.execDML(m_buf);
	}

	re = db.tableExists(tablename.c_str());
	if(re == false)  //说明数据库中没有这个用户的聊天记录
	{
		//char buf[128];
		_stprintf(m_buf,MSG_CREATE_TABLE, tablename.c_str());
		db.execDML(m_buf);
	}

	return true;
}

bool IMDB::PrepareTableGroupMsg(CppSQLite3DB &db, tstring strRoomTableName)
{
	bool re;

	//这个room表是否已经存在了？！
	re = db.tableExists(strRoomTableName.c_str());
	if(re == false)  //说明数据库中没有这个用户的聊天记录
	{
		_stprintf(m_buf,MSG_GROUP_CREATE_TABLE, strRoomTableName.c_str());
		db.execDML(m_buf);
	}

	return true;
}

bool IMDB::PrepareTableGroupRecent(CppSQLite3DB& db)
{
	bool re;
	re = db.tableExists(RECENTLIST_GROUP);
	if(re == false)  
	{
		int nResult = db.execDML(RECENTLIST_CREATE_GROUP_TABLE);
		re = nResult == 1? true : false;
	}

	return re;
}

bool IMDB::PrepareTableRecentList(CppSQLite3DB& db)
{
	bool re;
	re = db.tableExists(RECENTLIST_TABLE);
	if(re == false)  
	{
		int nResult =db.execDML(RECENTLIST_CREATE_TABLE);
		re = nResult == 1 ? true : false;
	}

	return re;
}

bool IMDB::PrepareTableCallHistoryList(CppSQLite3DB& db)
{
	bool re;
	re = db.tableExists(CALL_HISTORY_TABLE);
	if(re == false)  
	{
		int nResult = db.execDML(CALL_HISTORY_CREATE_TABLE);
		re = nResult == 1 ? true : false;
	}

	return re;
}

bool IMDB::PrepareTableSysMsg(CppSQLite3DB& db)
{
	bool re;
	re = db.tableExists(MSG_SYS_TABLE);
	if(re == false)  //说明数据库中没有组列表
	{
		_stprintf(m_buf,MSG_CREATE_SYSMSG_TABLE, MSG_SYS_TABLE);
		int nResult = db.execDML(m_buf);
		re = nResult == 1 ? true : false;
	}

	return re;
}

bool IMDB::PrepareTableContacts(CppSQLite3DB& db) 
{
	bool re;

// 	re = db.tableExists(DB_TABLE_VERSION);
// 	if(re == false)  //说明数据库是新建，生成库的基本表
// 	{
// 		db.execDML(DBVERSION_CREATE_TABLE);
// 
// 		_stprintf(m_buf, DB_VERSION_INSERT_ITEM, CLIENT_VERSION);
// 		db.execDML(m_buf);
// 
// 	}

// 	CppSQLite3Query q = db.execQuery(_T("select * from Version_DB ;"));
// 	float version = (float)q.getFloatField(0);
// 	if (version < CLIENT_VERSION)//数据库版本低于当前程序版本，更新数据库
// 	{
// 		//update;
// 	}
// 	q.finalize();
	
	re = db.tableExists(CONTACTER_TABLE);
	if(re == false)  //说明数据库中没有组列表
	{
		int nReuslt = db.execDML(CONTACTER_CREATE_TABLE);
		re = nReuslt == 1 ? true : false;
	}

	//lee:暂时还不需要
// 	re = db.tableExists(CONTACTER_EXTEND_TABLE);
// 	if(re == false)  //说明数据库中没有组列表
// 	{
// 		db.execDML(CONTACTER_CREATE_EXTEND_TABLE);
// 	}
	
	//lee:个人信息表
// 	re = db.tableExists(UIN_INFO_TABLE);
// 	if(re == false)  
// 	{
// 		db.execDML(UIN_INFO_CREATE_TABLE);
// 	}
// 
// 	re = db.tableExists(UIN_EXTEND_INFO_TABLE);
// 	if(re == false)  
// 	{
// 		db.execDML(UIN_EXTEND_INFO_CREATE_TABLE);
// 	}

	//最近联系人

	return true;
}

CppSQLite3DB & IMDB::OpenTableTalkMsg()
{
	if (m_TalkMsgDb.IsOpen())
	{
		return m_TalkMsgDb;
	}
	else
	{
		tstring filename = m_ModuleUinPath;
		filename += MSG_FLLENAME;
		m_TalkMsgDb.open(filename.c_str());

		return m_TalkMsgDb;
	}
}

CppSQLite3DB & IMDB::OpenTableContacts()
{
	if (m_ContactDb.IsOpen())
	{
		return m_ContactDb;
	}
	else
	{
		tstring filename = m_ModuleUinPath;
		filename += CONTACTER_FLLENAME;
		m_ContactDb.open(filename.c_str());
		return m_ContactDb;
	}
}

CppSQLite3DB & IMDB::OpenTableOptional()
{
	if (m_OptionalDb.IsOpen())
	{
		return m_OptionalDb;
	}
	else
	{
		tstring filename = m_ModuleUinPath;
		filename += OPTIONAL_FLLENAME;
		m_OptionalDb.open(filename.c_str());
		return m_OptionalDb;
	}
}

CppSQLite3DB & IMDB::OpenTableGlobalOptional()
{
	if (m_GlobalOptionalDb.IsOpen())
	{
		return m_GlobalOptionalDb;
	}
	else
	{
		tstring filename = m_strUserPath/*m_ModulePath*/;
		filename += OPTIONAL_FLLENAME;
		m_GlobalOptionalDb.open(filename.c_str());
		return m_GlobalOptionalDb;
	}
}


void IMDB::CloseDb()
{
	m_TalkMsgDb.close();
	m_ContactDb.close();
	m_OptionalDb.close();
	m_GlobalOptionalDb.close();
}


void SpecialReplace(tstring & s)
{
	tstring::size_type postion  = 0;
	while( (postion = s.find(_T("'"),postion)) != TSTRING::npos )
	{
		s.replace(postion,1,_T("''"));
		postion+=2;
		if(postion >= s.length())
		{
			return;
		}
	}
}

bool IMDB::SaveSysMessage(sSysMsg& sysMsg)
{
	bool bResult = false;
	try
	{
		SpecialReplace(sysMsg.strMsg);

		tstring tablename;
		tablename  = MSG_SYS_TABLE;
		tstring filename = m_ModuleUinPath;
		filename += MSG_FLLENAME;
		CppSQLite3DB &db = OpenTableTalkMsg();
		if(!db.IsOpen())
		{
			return false;
		}

		bResult = PrepareTableSysMsg(db) ;
		if (bResult != true)
		{
			return bResult;
		}

		TSTRING_OSTREAM os;
		//插入 eSubType INTEGER, sTime INTEGER, msgContent TEXT, bProcessed INTEGER, exInfo TEXT
		os<<_T("INSERT INTO ")<<tablename<<_T("(eSubType, sTime, msgContent, bProcessed, exInfo)  VALUES( '")
			<<sysMsg.eSubType<<_T("'")<<_T(",'")
			<<sysMsg.uTime<<_T("',")
			<<_T("'")<<sysMsg.strMsg.c_str()<<_T("',")
			<<_T("'")<<sysMsg.bProcessed<<_T("',")
			<<_T("'")<<sysMsg.strSender.c_str()<<_T("'")
			<<_T(");");

		tstring strSQL = os.str().c_str();
		db.execDML(strSQL.c_str());

		return true;
	}
	catch (CppSQLite3Exception& e)
	{
		const int errorcode = e.errorCode();
		const tstring(e.errorMessage());
	}
	catch(...)
	{
		bResult =  false;
	}

	return bResult;
}
bool IMDB::LoadSysMessage(vector<sSysMsg>& vecSysMsgs)
{
	try
	{
		bool re;
		tstring filename2 = m_ModuleUinPath;
		filename2 += MSG_FLLENAME;
		CppSQLite3DB &db2 = OpenTableTalkMsg();
		if(!db2.IsOpen())
		{
			return false;
		}

		re = PrepareTableSysMsg(db2) ;
		if (re != true)
		{
			return re;
		}


		sprintf_s(m_buf, sizeof(m_buf), _T("select * from %s;"),MSG_SYS_TABLE);


		
		m_query = db2.execQuery(m_buf);
		int nCap = vecSysMsgs.capacity();
		vecSysMsgs.reserve(100);//如果不开始分配，会导致在下面的语句执行时候，重新reserve导致crash。。。?!
		nCap = vecSysMsgs.capacity();

		//eSubType INTEGER, sTime INTEGER, msgContent TEXT, bProcessed INTEGER, exInfo TEXT
		while(!m_query.eof())
		{
			sSysMsg sysMsg;
			sysMsg.eSubType = (ESysMsgSubType)(m_query.getIntField(_T("eSubType"), 0));
			sysMsg.uTime = m_query.getIntField(_T("sTime"), 0);
			sysMsg.strMsg = m_query.getStringField("msgContent", 0);
			sysMsg.bProcessed = m_query.getIntField(_T("bProcessed"), 0);
			sysMsg.strExInfo = m_query.getStringField(_T("exInfo"), 0);
			sysMsg.strSender = sysMsg.strExInfo;
//			sysMsg.strReqId = sysMsg.strExInfo;
			vecSysMsgs.push_back(sysMsg);

			m_query.nextRow();
		}

		return true;
	}
	catch (CppSQLite3Exception& e)
	{
		const int errorcode = e.errorCode();
		const tstring(e.errorMessage());
		return false;
	}

	return true;
}
bool IMDB::GetLastSysMsg(sSysMsg& sysMsg)
{
	vector<sSysMsg>* pVec = new vector<sSysMsg>;
	if (LoadSysMessage(*pVec))
	{
		int nSize=pVec->size();
		if (nSize>0)
		{
			sysMsg = (*pVec)[nSize-1];
			
			return true;
		}
	}

	return false;
}
bool IMDB::UpdateSysMsgStatus(time_t time, bool bProcessed)
{
	bool bResult;

	tstring tablename;
	tablename  = MSG_SYS_TABLE;
	tstring filename = m_ModuleUinPath;
	filename += MSG_FLLENAME;
	CppSQLite3DB &db = OpenTableTalkMsg();
	if(!db.IsOpen())
	{
		return false;
	}

	bResult = PrepareTableSysMsg(db) ;
	if (bResult != true)
	{
		return bResult;
	}
	//基本上这个接口就是为了改为‘已处理’的。。。
	sprintf_s(m_buf, sizeof(m_buf), _T("update SYS_MSG set  bProcessed = '1' where sTime = '%d';"),time);
	
// 	TSTRING_OSTREAM os;
// 	TSTRING_OSTREAM os2;
// 	os<<_T("update SYS_MSG set  bProcessed = ")
// 		<<_T("'")<<(int)bProcessed<<_T("'") << _T("  where sTime = ")
// 		<<_T("'")<<time<<_T("'")<<_T(" ;");
// 	
// 	tstring strSql = os.str().c_str();
// 	
	
	db.execDML(m_buf);

	return true;
}

bool IMDB::RemoveGroupChatMessage(const tstring& room_id)
{
	bool bResult = false;
	try
	{
		//这个room的表名
		tstring tablename;
		tablename = MSG_GROUP_TABLE_HEAD + room_id;

		//打开msg.db数据库
		tstring filename = m_ModuleUinPath;
		filename += MSG_FLLENAME;
		CppSQLite3DB &db = OpenTableTalkMsg();
		if(!db.IsOpen())
		{
			return false;
		}

		TSTRING_OSTREAM os;
		//插入消息记录！！uid, stime, msgtext
		os<<_T("DROP TABLE ")<<tablename<<_T(";");

		tstring strSQL = os.str().c_str();
		db.execDML(strSQL.c_str());

		return true;
	}
	catch (CppSQLite3Exception& e)
	{
		const int errorcode = e.errorCode();
		const tstring(e.errorMessage());
	}
	catch(...)
	{
		bResult =  false;
	}
	if (!bResult)
	{
		//MessageBox(NULL, "Save group message failed.", "error!",MB_YESNO);
	}
	return bResult;
}

bool IMDB::SaveGroupChatMessage(const tstring& strRoomId, _tGroupChatMessage& tGroupMsg)
{
	bool bResult = false;
	try
	{
		//消息处理
		tstring msgstr = tGroupMsg.strMsg;
		SpecialReplace(msgstr);

		//这个room的表名
		tstring tablename;
		tablename = MSG_GROUP_TABLE_HEAD + strRoomId;

		//打开msg.db数据库
		tstring filename = m_ModuleUinPath;
		filename += MSG_FLLENAME;
		CppSQLite3DB &db = OpenTableTalkMsg();
		if(!db.IsOpen())
		{
			return false;
		}

		//创建room表
		bResult = PrepareTableGroupMsg(db,tablename) ;
		if (!bResult)
		{
			
		MessageBox(NULL, "open table failed.", "error!",MB_YESNO);
			
			return bResult;
		}

		//插入消息记录！CREATE TABLE %s ( roomId TEXT, senderPhone TEXT, senderAccount TEXT, msgText TEXT, stime TEXT);

		/*TSTRING_OSTREAM os;
		os<<_T("INSERT INTO ")<<tablename<<_T("(roomId, senderPhone, senderAccount, msgText, stime)  VALUES( '")
		<<strRoomId.c_str()<<_T("'")<<_T(",'")
		<<tGroupMsg.strSenderPhone.c_str()<<_T("'")<<_T(",'")
		<<tGroupMsg.strSenderAccount.c_str()<<_T("'")<<_T(",'")
		<<msgstr.c_str()<<_T("','")
		<<tGroupMsg.m_timeChat<<_T("'")
		<<_T(");");
		tstring strSQL = os.str().c_str();
		db.execDML(strSQL.c_str());*/

		int nBuffLen = tGroupMsg.strMsg.length()+tGroupMsg.strRoomID.length()
			+tGroupMsg.strSenderAccount.length()+tGroupMsg.strSenderPhone.length()
			+tGroupMsg.strTimeString.length()
			+tstring("INSERT INTO %s (roomId, senderPhone, senderAccount, msgText, stime) VALUES('%s','%s','%s','%s','%d');").length();
		char* pBuf = new char[nBuffLen+2048];
		memset(pBuf, 0, nBuffLen+2048);
		sprintf_s(pBuf, nBuffLen+2048, _T("INSERT INTO %s (roomId, senderPhone, senderAccount, msgText, stime) VALUES('%s','%s','%s','%s','%d');"),
			tablename.c_str(), strRoomId.c_str(), tGroupMsg.strSenderPhone.c_str(), tGroupMsg.strSenderAccount.c_str(),msgstr.c_str(), tGroupMsg.m_timeChat);
		db.execDML(pBuf);
		delete [] pBuf;
		

		return true;
	}
	catch (CppSQLite3Exception& e)
	{
		const int errorcode = e.errorCode();
		const tstring(e.errorMessage());
	}
	catch(...)
	{
		bResult =  false;
	}
	if (!bResult)
	{
		//MessageBox(NULL, "Save group message failed.", "error!",MB_YESNO);
	}
	return bResult;
}

bool IMDB::RemoveChatMessage(IM_UID uid)
{
	bool bResult = false;
	try
	{

		tstring strDestID = CFileHelper::GetUserName(uid);

		tstring tablename;
		tablename  =MSG_TABLE_HEAD + strDestID;
		tstring filename = m_ModuleUinPath;
		filename += MSG_FLLENAME;
		CppSQLite3DB &db = OpenTableTalkMsg();
		if(!db.IsOpen())
		{
			return false;
		}

// 		bResult = PrepareTableTalkMsg(db,tablename) ;
// 		if (bResult != true)
// 		{
// 			return bResult;
// 		}

		TSTRING_OSTREAM os;
		//插入消息记录！！uid, stime, msgtext
		os<<_T("DROP TABLE ")<<tablename<<_T(";");

		tstring strSQL = os.str().c_str();
		db.execDML(strSQL.c_str());

		return true;
	}
	catch (CppSQLite3Exception& e)
	{
		const int errorcode = e.errorCode();
		const tstring(e.errorMessage());
		//LOG(LOG_LEVEL_ERROR, "SaveTalkMsg() errorcode:" << e.errorCode() << "errorMessage:" << e.errorMessage());
		///assert(0);
	}
	catch(...)
	{
		bResult =  false;
		//LOG(LOG_LEVEL_ERROR, "SaveTalkMsg() errorcode:catch (...)");
		///assert(0);
	}

	return bResult;
}

bool IMDB::SaveChatMessage(IM_UID uid, _tChatMessage& tMsg)
{
	bool bResult = false;
	try
	{

		tstring strDestID = CFileHelper::GetUserName(uid);
		tstring msgstr = tMsg.strChatText;
		SpecialReplace(msgstr);

		tstring tablename;
		tablename  =MSG_TABLE_HEAD + strDestID;
		tstring filename = m_ModuleUinPath;
		filename += MSG_FLLENAME;
		CppSQLite3DB &db = OpenTableTalkMsg();
		if(!db.IsOpen())
		{
			return false;
		}

		bResult = PrepareTableTalkMsg(db,tablename) ;
		if (bResult != true)
		{
			return bResult;
		}

		TSTRING_OSTREAM os;
		//插入消息记录！！uid, stime, msgtext
		os<<_T("INSERT INTO ")<<tablename<<_T("(uid, msgText, stime)  VALUES( '")
			<<(CFileHelper::GetUserName(tMsg.strDestUser)).c_str()<<_T("'")<<_T(",'")
			<<msgstr.c_str()<<_T("',")
			<<_T("'")<<tMsg.m_timeChat<<_T("'")
			<<_T(");");

		tstring strSQL = os.str().c_str();
		db.execDML(strSQL.c_str());

		return true;
	}
	catch (CppSQLite3Exception& e)
	{
		const int errorcode = e.errorCode();
		const tstring(e.errorMessage());
		//LOG(LOG_LEVEL_ERROR, "SaveTalkMsg() errorcode:" << e.errorCode() << "errorMessage:" << e.errorMessage());
		///assert(0);
	}
	catch(...)
	{
		bResult =  false;
		//LOG(LOG_LEVEL_ERROR, "SaveTalkMsg() errorcode:catch (...)");
		///assert(0);
	}

	return bResult;
}
//lee:缓存本地好友信息
bool IMDB::SaveUserVCard(IM_UID uid,sUserVCard& useinfo)
{
	if (uid.empty())
	{
		return false;
	}
	try
	{
		bool re;

		tstring  tablename2;
		tablename2  = CONTACTER_TABLE;
		tstring filename2 = m_ModuleUinPath;
		filename2 += CONTACTER_FLLENAME;
		CppSQLite3DB &db2 = OpenTableContacts();
		if(!db2.IsOpen())
		{
			return false;
		}

		//创建数据库
		re = PrepareTableContacts(db2);
		if (re != true)
		{
			return re;
		}

 		if(BeExistContacter(db2, uid))
 		{
			//直接删除这行数据，再添加
			//sprintf_s(m_buf, sizeof(m_buf), _T("DELETE FROM CONTACTER_INFO WHERE uid = '%s';"),uid.c_str());
			//db2.execDML(m_buf);
             UpdateUserInfo(db2,uid,useinfo);
 		}
 		else
		{
			AddUserInfo(db2,uid,useinfo);
		}

		return true;
	}
	catch (CppSQLite3Exception& e)
	{
		const int errorcode = e.errorCode();
		const tstring(e.errorMessage());
		return false;
	}
}


bool IMDB::UpdateUserInfo(CppSQLite3DB& db,IM_UID uid,/*_tUserInfo*/sUserVCard & usrinfo)
{
	
	bool re;
	TSTRING_OSTREAM os;
	TSTRING_OSTREAM os2;

	TSTRING nick = usrinfo.strUserNickname.c_str();
	TSTRING motto = usrinfo.strSignature.c_str();

	os<<_T("update CONTACTER_INFO set  ")
		<<_T("nick = '")<<usrinfo.strUserNickname.c_str()<<_T("',")//
		<<_T("remark = '")<<usrinfo.strRemark.c_str()<<_T("',")//
		<<_T("gender = '")<<usrinfo.strGender.c_str()<<_T("',")
		<<_T("signature = '")<<usrinfo.strSignature.c_str()<<_T("',")
		<<_T("phone = '")<<usrinfo.strPhone.c_str()<<_T("',")
		<<_T("email = '")<<usrinfo.strEmail.c_str()<<_T("',")
		<<_T("address = '")<<usrinfo.strAddr.c_str()<<_T("',")
		<<_T("photoType = '")<<usrinfo.strPhotoType.c_str()<<_T("',")
		<<_T("photoUrl = '")<<usrinfo.strUrl.c_str()<<_T("',")
		<<_T("photoPath = '")<<usrinfo.strPhotoPath.c_str()<<_T("',")
		<<_T("photoGrayPath = '")<<usrinfo.strGrayAvatar.c_str()<<_T("' ")
		<<_T(" where uid = ")<<uid.c_str()<<_T(";");

	//db.execDML(BEGIN_TRANSACTION);
	tstring strSql = os.str();
	db.execDML(strSql.c_str());

	return true;
}

bool IMDB::UpdateUserExtendInfo(CppSQLite3DB& db,IM_UID uid,_tDetailedUserInfo & extendinfo)
{
	bool re;
	TSTRING_OSTREAM os;
	TSTRING_OSTREAM os2;

	//SpecialReplace(nick);
	//SpecialReplace(motto);
	/*
	+-----------------+---------------------+------+-----+---------+-------+
	| Field           | Type                | Null | Key | Default | Extra |
	+-----------------+---------------------+------+-----+---------+-------+
	| Fleft_uid       | bigint(64)          | NO   | PRI | 0       |       | 
	| Fpublic_flag    | tinyint(3)          | YES  |     | 0       |       | 
	| Femail          | varchar(128)        | YES  |     | NULL    |       | 
	| Fmoblie         | varchar(16)         | YES  |     | NULL    |       | 
	| Fphone          | varchar(32)         | YES  |     | NULL    |       | 
	| Flast_school    | varchar(64)         | YES  |     | NULL    |       | 
	| Fcareer         | varchar(64)         | YES  |     | NULL    |       | 
	| Flast_company   | varchar(64)         | YES  |     | NULL    |       | 
	| Flast_province  | varchar(32)         | YES  |     | NULL    |       | 
	| Flast_city      | varchar(32)         | YES  |     | NULL    |       | 
	| Flast_town      | varchar(32)         | YES  |     | NULL    |       | 
	| Fhome_province  | varchar(32)         | YES  |     | NULL    |       | 
	| Fhome_city      | varchar(32)         | YES  |     | NULL    |       | 
	| Fhome_town      | varchar(32)         | YES  |     | NULL    |       | 
	| Fage            | tinyint(3) unsigned | YES  |     | 255     |       | 
	| Fbirthday_month | tinyint(3) unsigned | YES  |     | 255     |       | 
	| Fbirthday_date  | tinyint(3) unsigned | YES  |     | 255     |       | 
	| Frefer_birthday | tinyint(3) unsigned | YES  |     | 255     |       | 
	| Fhome_page      | varchar(128)        | YES  |     | NULL    |       | 
	| Fauth_type      | tinyint(3)          | YES  |     | 0       |       | 
	| Fquestion       | varchar(128)        | YES  |     | NULL    |       | 
	| Fanswer         | varchar(128)        | YES  |     | NULL    |       | 
	+-----------------+---------------------+------+-----+---------+-------+
	22 rows in set (0.01 sec)
	#define CONTACTER_CREATE_EXTEND_TABLE  _T("create table CONTACTER_EXTEND_INFO (Fleft_uid INTEGER PRIMARY KEY,Fpublic_flag,Femail,Fmoblie,Flast_school,Fcareer,Flast_company,Flast_province,Flast_city,Fhome_town,Fage,Fbirthday_month,Fbirthday_date,Frefer_birthday,Fhome_page,Fauth_type,Fquestion,Fanswer);") 
	*/
	/*
	struct _tDetailedUserInfo
	{
	IM_uint8 age;
	IM_uint8 birthday_type;
	IM_uint8 birthday_month;
	IM_uint8 birthday_day;
	char     home_region[50];
	char     home_city[50];
	char     home_province[50];
	char     local_region[50];
	char     local_city[50];
	char     local_province[50];
	char     college[64];
	char     company[64];
	char     carrier[64];
	char     home_page[128];
	char     e_mail[32];
	char     mobile_num[11];
	char     telphone[13];
	};
	*/
	os<<_T("update UIN_EXTEND_INFO_TABLE set  Fage = ")
		<<extendinfo.age<<_T(",  Frefer_birthday = ")
		<<extendinfo.birthday_type<<_T(",  Fbirthday_month = ")
		<<extendinfo.birthday_month<<_T(",  Fbirthday_date = ")
		<<extendinfo.birthday_day<<_T(",  Fhome_town = '")
		<<extendinfo.home_region<<_T("',  Fhome_city = '")
		<<extendinfo.local_city<<_T("',   Fhome_province = '")
		<<extendinfo.home_province<<_T("',   Flast_school = '")
		<<extendinfo.college<<_T("',   Flast_company = '")
		<<extendinfo.company<<_T("',   Fcareer = '")
		<<extendinfo.carrier<<_T("',   Fhome_page = '")
		<<extendinfo.home_page<<_T("',   Femail = '")
		<<extendinfo.e_mail<<_T("',   Fmoblie = '")
		<<extendinfo.mobile_num<<_T("',   Fphone = '")
		<<extendinfo.telphone<<_T("'  where Fleft_uid = ")
		<<uid<<_T(" ;");

	//db.execDML(BEGIN_TRANSACTION);
	db.execDML(os.str().c_str());

	return true;
}

bool IMDB::UpdateContactInfo(CppSQLite3DB& db,IM_UID uid,_tUserInfo & usrinfo)
{
	bool re;
	TSTRING_OSTREAM os;
	TSTRING_OSTREAM os2;

	TSTRING nick = usrinfo.name;
	TSTRING motto = usrinfo.signature;

	//SpecialReplace(nick);
	//SpecialReplace(motto);

	os<<_T("update CONTACTER_INFO set  Fbind_account = '")
		<<nick<<_T(",  Fsigned = '")
		<<motto<<_T("',  Fhead_pic = ")
		<<(IM_uint32)usrinfo.pic_id<<_T(",  Fsex = ")
		<<(IM_uint32)usrinfo.gender<<_T("  where Fleft_uid = ")
		<<uid<<_T(" ;");

	//db.execDML(BEGIN_TRANSACTION);
	db.execDML(os.str().c_str());

	return true;
}

bool IMDB::UpdateContactExtendInfo(CppSQLite3DB& db,IM_UID uid,_tDetailedUserInfo & extendinfo)
{
	bool re;
	TSTRING_OSTREAM os;
	TSTRING_OSTREAM os2;

	//SpecialReplace(nick);
	//SpecialReplace(motto);
	/*
	+-----------------+---------------------+------+-----+---------+-------+
	| Field           | Type                | Null | Key | Default | Extra |
	+-----------------+---------------------+------+-----+---------+-------+
	| Fleft_uid       | bigint(64)          | NO   | PRI | 0       |       | 
	| Fpublic_flag    | tinyint(3)          | YES  |     | 0       |       | 
	| Femail          | varchar(128)        | YES  |     | NULL    |       | 
	| Fmoblie         | varchar(16)         | YES  |     | NULL    |       | 
	| Fphone          | varchar(32)         | YES  |     | NULL    |       | 
	| Flast_school    | varchar(64)         | YES  |     | NULL    |       | 
	| Fcareer         | varchar(64)         | YES  |     | NULL    |       | 
	| Flast_company   | varchar(64)         | YES  |     | NULL    |       | 
	| Flast_province  | varchar(32)         | YES  |     | NULL    |       | 
	| Flast_city      | varchar(32)         | YES  |     | NULL    |       | 
	| Flast_town      | varchar(32)         | YES  |     | NULL    |       | 
	| Fhome_province  | varchar(32)         | YES  |     | NULL    |       | 
	| Fhome_city      | varchar(32)         | YES  |     | NULL    |       | 
	| Fhome_town      | varchar(32)         | YES  |     | NULL    |       | 
	| Fage            | tinyint(3) unsigned | YES  |     | 255     |       | 
	| Fbirthday_month | tinyint(3) unsigned | YES  |     | 255     |       | 
	| Fbirthday_date  | tinyint(3) unsigned | YES  |     | 255     |       | 
	| Frefer_birthday | tinyint(3) unsigned | YES  |     | 255     |       | 
	| Fhome_page      | varchar(128)        | YES  |     | NULL    |       | 
	| Fauth_type      | tinyint(3)          | YES  |     | 0       |       | 
	| Fquestion       | varchar(128)        | YES  |     | NULL    |       | 
	| Fanswer         | varchar(128)        | YES  |     | NULL    |       | 
	+-----------------+---------------------+------+-----+---------+-------+
	22 rows in set (0.01 sec)
	#define CONTACTER_CREATE_EXTEND_TABLE  _T("create table CONTACTER_EXTEND_INFO (Fleft_uid INTEGER PRIMARY KEY,Fpublic_flag,Femail,Fmoblie,Flast_school,Fcareer,Flast_company,Flast_province,Flast_city,Fhome_town,Fage,Fbirthday_month,Fbirthday_date,Frefer_birthday,Fhome_page,Fauth_type,Fquestion,Fanswer);") 
	*/
	/*
	struct _tDetailedUserInfo
	{
	IM_uint8 age;
	IM_uint8 birthday_type;
	IM_uint8 birthday_month;
	IM_uint8 birthday_day;
	char     home_region[50];
	char     home_city[50];
	char     home_province[50];
	char     local_region[50];
	char     local_city[50];
	char     local_province[50];
	char     college[64];
	char     company[64];
	char     carrier[64];
	char     home_page[128];
	char     e_mail[32];
	char     mobile_num[11];
	char     telphone[13];
	};
	*/
	os<<_T("update CONTACTER_EXTEND_INFO set  Fage = ")
		<<extendinfo.age<<_T(",  Frefer_birthday = ")
		<<extendinfo.birthday_type<<_T(",  Fbirthday_month = ")
		<<extendinfo.birthday_month<<_T(",  Fbirthday_date = ")
		<<extendinfo.birthday_day<<_T(",  Fhome_town = '")
		<<extendinfo.home_region<<_T("',  Fhome_city = '")
		<<extendinfo.local_city<<_T("',   Fhome_province = '")
		<<extendinfo.home_province<<_T("',   Flast_school = '")
		<<extendinfo.college<<_T("',   Flast_company = '")
		<<extendinfo.company<<_T("',   Fcareer = '")
		<<extendinfo.carrier<<_T("',   Fhome_page = '")
		<<extendinfo.home_page<<_T("',   Femail = '")
		<<extendinfo.e_mail<<_T("',   Fmoblie = '")
		<<extendinfo.mobile_num<<_T("',   Fphone = '")
		<<extendinfo.telphone<<_T("'  where Fleft_uid = ")
		<<uid<<_T(" ;");

	//db.execDML(BEGIN_TRANSACTION);
	db.execDML(os.str().c_str());

	return true;
}

bool IMDB::AddUserInfo(CppSQLite3DB& db,IM_UID uid,/*_tUserInfo*/sUserVCard & usrinfo)
{
	TSTRING_OSTREAM os;
	TSTRING_OSTREAM os2;

	TSTRING nick = usrinfo.strUserNickname;
	TSTRING motto = usrinfo.strSignature;

	//SpecialReplace(nick);
	//SpecialReplace(motto);
	/*
	+---------------+---------------------+------+-----+---------+-------+
	| Field         | Type                | Null | Key | Default | Extra |
	+---------------+---------------------+------+-----+---------+-------+
	| Fleft_uid     | bigint(64) unsigned | NO   | PRI | 0       |       | 
	| Fbind_account | varchar(64)         | YES  |     | NULL    |       | 
	| Freal_name    | varchar(64)         | YES  |     | NULL    |       | 
	| Fsex          | tinyint(3) unsigned | YES  |     | 255     |       | 
	| Fstate        | tinyint(3)          | YES  |     | 0       |       | 
	| Fvip          | tinyint(3)          | YES  |     | 0       |       | 
	| Fhead_pic     | int(11) unsigned    | YES  |     | 0       |       | 
	| Fblock_set    | tinyint(3) unsigned | YES  |     | 0       |       | 
	| Fsigned       | varchar(255)        | YES  |     | NULL    |       | 
	| good_uin      | int(11) unsigned    | NO   |     | NULL    |       | 
	| Fversion      | int(11) unsigned    | YES  |     | 1       |       | 
	| Fsavemsg      | tinyint(3)          | YES  |     | 0       |       | 
	| Fservice_mask | int(11)             | YES  |     | 0       |       | 
	+---------------+---------------------+------+-----+---------+-------+
	*/

	//typedef struct
	//{
	//	tstring strUserName;
	//	tstring strUserNickname;
	//	tstring strGender;		//性别
	//	tstring strSignature;	//签名
	//	tstring strPhone;
	//	tstring strEmail;
	//	tstring strAddr;
	//	tstring strPhotoType;	//头像类型
	//	tstring strPhotoBinary;	//头像base64
	//	tstring strUrl;			//头像的Url地址
	//	tstring strPhotoPath;	//在线头像本地路径
	//	tstring strGrayAvatar;	//离线头像本地路径
	//}sUserVCard;

	os<</*UIN_INFO_INSERT_ITEM*/CONTACT_INFO_INSERT_ITEM
		<<_T("'")<<uid.c_str()<<_T("',")
		<<_T("'")<<usrinfo.strUserNickname.c_str()<<_T("',")
		<<_T("'")<<usrinfo.strRemark.c_str()<<_T("',")
		<<_T("'")<<usrinfo.strGender.c_str()<<_T("',")
		<<_T("'")<<usrinfo.strSignature.c_str()<<_T("',")
		<<_T("'")<<usrinfo.strPhone.c_str()<<_T("',")
		<<_T("'")<<usrinfo.strEmail.c_str()<<_T("',")
		<<_T("'")<<usrinfo.strAddr.c_str()<<_T("',")
		<<_T("'")<<usrinfo.strPhotoType.c_str()<<_T("',")
		<<_T("'")<<usrinfo.strPhotoBinary.c_str()<<_T("',")
		<<_T("'")<<usrinfo.strUrl.c_str()<<_T("',")
		<<_T("'")<<usrinfo.strPhotoPath.c_str()<<_T("',")
		<<_T("'")<<usrinfo.strGrayAvatar.c_str()<<_T("');");
		tstring strSQL = os.str().c_str();
	db.execDML(strSQL.c_str()/*os.str().c_str()*/);


	return true;
}

bool IMDB::AddUserExtendInfo(CppSQLite3DB& db,IM_UID uid,_tDetailedUserInfo & extendinfo)
{
	TSTRING_OSTREAM os;
	TSTRING_OSTREAM os2;

	/*
	+-----------------+---------------------+------+-----+---------+-------+
	| Field           | Type                | Null | Key | Default | Extra |
	+-----------------+---------------------+------+-----+---------+-------+
	| Fleft_uid       | bigint(64)          | NO   | PRI | 0       |       | 
	| Fpublic_flag    | tinyint(3)          | YES  |     | 0       |       | 
	| Femail          | varchar(128)        | YES  |     | NULL    |       | 
	| Fmoblie         | varchar(16)         | YES  |     | NULL    |       | 
	| Fphone          | varchar(32)         | YES  |     | NULL    |       | 
	| Flast_school    | varchar(64)         | YES  |     | NULL    |       | 
	| Fcareer         | varchar(64)         | YES  |     | NULL    |       | 
	| Flast_company   | varchar(64)         | YES  |     | NULL    |       | 
	| Flast_province  | varchar(32)         | YES  |     | NULL    |       | 
	| Flast_city      | varchar(32)         | YES  |     | NULL    |       | 
	| Flast_town      | varchar(32)         | YES  |     | NULL    |       | 
	| Fhome_province  | varchar(32)         | YES  |     | NULL    |       | 
	| Fhome_city      | varchar(32)         | YES  |     | NULL    |       | 
	| Fhome_town      | varchar(32)         | YES  |     | NULL    |       | 
	| Fage            | tinyint(3) unsigned | YES  |     | 255     |       | 
	| Fbirthday_month | tinyint(3) unsigned | YES  |     | 255     |       | 
	| Fbirthday_date  | tinyint(3) unsigned | YES  |     | 255     |       | 
	| Frefer_birthday | tinyint(3) unsigned | YES  |     | 255     |       | 
	| Fhome_page      | varchar(128)        | YES  |     | NULL    |       | 
	| Fauth_type      | tinyint(3)          | YES  |     | 0       |       | 
	| Fquestion       | varchar(128)        | YES  |     | NULL    |       | 
	| Fanswer         | varchar(128)        | YES  |     | NULL    |       | 
	+-----------------+---------------------+------+-----+---------+-------+
	22 rows in set (0.01 sec)
	#define CONTACTER_CREATE_EXTEND_TABLE  _T("create table CONTACTER_EXTEND_INFO (Fleft_uid INTEGER PRIMARY KEY,Fpublic_flag,Femail,Fmoblie,Flast_school,Fcareer,Flast_company,Flast_province,Flast_city,Fhome_town,Fage,Fbirthday_month,Fbirthday_date,Frefer_birthday,Fhome_page,Fauth_type,Fquestion,Fanswer);") 
	*/
	/*
	struct _tDetailedUserInfo
	{
	IM_uint8 age;
	IM_uint8 birthday_type;
	IM_uint8 birthday_month;
	IM_uint8 birthday_day;
	char     home_region[50];
	char     home_city[50];
	char     home_province[50];
	char     local_region[50];
	char     local_city[50];
	char     local_province[50];
	char     college[64];
	char     company[64];
	char     carrier[64];
	char     home_page[128];
	char     e_mail[32];
	char     mobile_num[11];
	char     telphone[13];
	};
	*/

	os<<UIN_EXTEND_INFO_INSERT_ITEM<<uid<<_T(",")
		<<(UINT32)0<<_T(",'")
		<<extendinfo.e_mail<<_T("','")
		<<extendinfo.mobile_num<<_T("','")
		<<extendinfo.telphone<<_T("','")
		<<extendinfo.college<<_T("','")
		<<extendinfo.carrier<<_T("','")
		<<extendinfo.company<<_T("','")
		<<extendinfo.local_province<<_T("','")
		<<extendinfo.local_city<<_T("','")
		<<extendinfo.local_region<<_T("',")
		<<extendinfo.age<<_T(",")
		<<extendinfo.birthday_month<<_T(",")
		<<extendinfo.birthday_day<<_T(",")
		<<extendinfo.birthday_type<<_T(",'")
		<<extendinfo.home_page<<_T("',")
		<<(UINT16)0<<_T(",'")
		<<(_T(""))<<_T("','")
		<<(_T(""))<<_T("');");
	    db.execDML(os.str().c_str());


	return true;
}

bool IMDB::AddContactInfo(CppSQLite3DB& db,IM_UID uid,_tUserInfo & usrinfo)
{
	TSTRING_OSTREAM os;
	TSTRING_OSTREAM os2;

	TSTRING nick = usrinfo.name;
	TSTRING motto = usrinfo.signature;

	//SpecialReplace(nick);
	//SpecialReplace(motto);
	/*
	+---------------+---------------------+------+-----+---------+-------+
	| Field         | Type                | Null | Key | Default | Extra |
	+---------------+---------------------+------+-----+---------+-------+
	| Fleft_uid     | bigint(64) unsigned | NO   | PRI | 0       |       | 
	| Fbind_account | varchar(64)         | YES  |     | NULL    |       | 
	| Freal_name    | varchar(64)         | YES  |     | NULL    |       | 
	| Fsex          | tinyint(3) unsigned | YES  |     | 255     |       | 
	| Fstate        | tinyint(3)          | YES  |     | 0       |       | 
	| Fvip          | tinyint(3)          | YES  |     | 0       |       | 
	| Fhead_pic     | int(11) unsigned    | YES  |     | 0       |       | 
	| Fblock_set    | tinyint(3) unsigned | YES  |     | 0       |       | 
	| Fsigned       | varchar(255)        | YES  |     | NULL    |       | 
	| good_uin      | int(11) unsigned    | NO   |     | NULL    |       | 
	| Fversion      | int(11) unsigned    | YES  |     | 1       |       | 
	| Fsavemsg      | tinyint(3)          | YES  |     | 0       |       | 
	| Fservice_mask | int(11)             | YES  |     | 0       |       | 
	+---------------+---------------------+------+-----+---------+-------+
	*/

	os<<CONTACT_INFO_INSERT_ITEM<<uid<<_T(",'")
		<<nick<<_T("','")
		<<nick<<_T("',")
		<<usrinfo.gender<<_T(",")
		<<(UINT32)0<<_T(",")
		<<(UINT32)0<<_T(",")
		<<usrinfo.pic_id<<_T("',")
		<<(UINT16)0<<_T(",'")
		<<motto<<_T("',")
		<<uid<<_T("',")
		<<(UINT32)0<<_T(",")
		<<(UINT32)0<<_T(",")
		<<(UINT32)0<<_T(");");
	db.execDML(os.str().c_str());


	return true;
}

bool IMDB::AddContactExtendInfo(CppSQLite3DB& db,IM_UID uid,_tDetailedUserInfo & extendinfo)
{
	TSTRING_OSTREAM os;
	TSTRING_OSTREAM os2;

	/*
	+-----------------+---------------------+------+-----+---------+-------+
	| Field           | Type                | Null | Key | Default | Extra |
	+-----------------+---------------------+------+-----+---------+-------+
	| Fleft_uid       | bigint(64)          | NO   | PRI | 0       |       | 
	| Fpublic_flag    | tinyint(3)          | YES  |     | 0       |       | 
	| Femail          | varchar(128)        | YES  |     | NULL    |       | 
	| Fmoblie         | varchar(16)         | YES  |     | NULL    |       | 
	| Fphone          | varchar(32)         | YES  |     | NULL    |       | 
	| Flast_school    | varchar(64)         | YES  |     | NULL    |       | 
	| Fcareer         | varchar(64)         | YES  |     | NULL    |       | 
	| Flast_company   | varchar(64)         | YES  |     | NULL    |       | 
	| Flast_province  | varchar(32)         | YES  |     | NULL    |       | 
	| Flast_city      | varchar(32)         | YES  |     | NULL    |       | 
	| Flast_town      | varchar(32)         | YES  |     | NULL    |       | 
	| Fhome_province  | varchar(32)         | YES  |     | NULL    |       | 
	| Fhome_city      | varchar(32)         | YES  |     | NULL    |       | 
	| Fhome_town      | varchar(32)         | YES  |     | NULL    |       | 
	| Fage            | tinyint(3) unsigned | YES  |     | 255     |       | 
	| Fbirthday_month | tinyint(3) unsigned | YES  |     | 255     |       | 
	| Fbirthday_date  | tinyint(3) unsigned | YES  |     | 255     |       | 
	| Frefer_birthday | tinyint(3) unsigned | YES  |     | 255     |       | 
	| Fhome_page      | varchar(128)        | YES  |     | NULL    |       | 
	| Fauth_type      | tinyint(3)          | YES  |     | 0       |       | 
	| Fquestion       | varchar(128)        | YES  |     | NULL    |       | 
	| Fanswer         | varchar(128)        | YES  |     | NULL    |       | 
	+-----------------+---------------------+------+-----+---------+-------+
	22 rows in set (0.01 sec)
	#define CONTACTER_CREATE_EXTEND_TABLE  _T("create table CONTACTER_EXTEND_INFO (Fleft_uid INTEGER PRIMARY KEY,Fpublic_flag,Femail,Fmoblie,Flast_school,Fcareer,Flast_company,Flast_province,Flast_city,Fhome_town,Fage,Fbirthday_month,Fbirthday_date,Frefer_birthday,Fhome_page,Fauth_type,Fquestion,Fanswer);") 
	*/
	/*
	struct _tDetailedUserInfo
	{
	IM_uint8 age;
	IM_uint8 birthday_type;
	IM_uint8 birthday_month;
	IM_uint8 birthday_day;
	char     home_region[50];
	char     home_city[50];
	char     home_province[50];
	char     local_region[50];
	char     local_city[50];
	char     local_province[50];
	char     college[64];
	char     company[64];
	char     carrier[64];
	char     home_page[128];
	char     e_mail[32];
	char     mobile_num[11];
	char     telphone[13];
	};
	*/

	os<<CONTACTER_EXTEND_INFO_INSERT_ITEM<<uid<<_T(",")
		<<(UINT32)0<<_T(",'")
		<<extendinfo.e_mail<<_T("','")
		<<extendinfo.mobile_num<<_T("','")
		<<extendinfo.telphone<<_T("','")
		<<extendinfo.college<<_T("','")
		<<extendinfo.carrier<<_T("','")
		<<extendinfo.company<<_T("','")
		<<extendinfo.local_province<<_T("','")
		<<extendinfo.local_city<<_T("','")
		<<extendinfo.local_region<<_T("',")
		<<extendinfo.age<<_T(",")
		<<extendinfo.birthday_month<<_T(",")
		<<extendinfo.birthday_day<<_T(",")
		<<extendinfo.birthday_type<<_T(",'")
		<<extendinfo.home_page<<_T("',")
		<<(UINT16)0<<_T(",'")
		<<(_T(""))<<_T("','")
		<<(_T(""))<<_T("');");
	db.execDML(os.str().c_str());


	return true;
}

bool IMDB::BeExistUserInfo(CppSQLite3DB& db,IM_UID uid)
{
	TSTRING_OSTREAM os;
	os<<_T("select * from UIN_INFO_TABLE where Fleft_uid = ")<<uid<<_T(" ;");
	//db.execDML(BEGIN_TRANSACTION);
	CppSQLite3Query q = db.execQuery(os.str().c_str());
	bool re = !q.eof();
	q.finalize();
	//db.execDML(COMMIT_TRANSACTION);
	return re;
}

bool IMDB::BeExistContacter(CppSQLite3DB& db,IM_UID uid)
{
	//lee:在函数结束的时候，os的释放会出问题；
	//解决方案1:使用m_buf.2:os成员变量化。原因可能是stl库的版本连接的有问题。

	sprintf_s(m_buf, sizeof(m_buf), _T("select * from CONTACTER_INFO where uid = '%s';"),uid.c_str());
	//m_os.str(_T(""));
	//m_os<<_T("select * from CONTACTER_INFO where uid = '")<<uid.c_str()<<_T("' ;");
	//db.execDML(BEGIN_TRANSACTION);
	m_query = db.execQuery(m_buf/*m_os.str().c_str()*/);
	bool re = !m_query.eof();
	m_query.finalize();
	//db.execDML(COMMIT_TRANSACTION);
	
	return re;
}



bool IMDB::BeExistUserExtendInfo(CppSQLite3DB& db,IM_UID uid)
{
	TSTRING_OSTREAM os;
	os<<_T("select * from UIN_EXTEND_INFO_TABLE where Fleft_uid = ")<<uid<<_T(" ;");
	//db.execDML(BEGIN_TRANSACTION);
	CppSQLite3Query q = db.execQuery(os.str().c_str());
	bool re = !q.eof();
	q.finalize();
	//db.execDML(COMMIT_TRANSACTION);
	return re;
}

bool IMDB::BeExistContacterExtendInfo(CppSQLite3DB& db,IM_UID uid)
{
	TSTRING_OSTREAM os;
	os<<_T("select * from CONTACTER_EXTEND_INFO where Fleft_uid = ")<<uid<<_T(" ;");
	//db.execDML(BEGIN_TRANSACTION);
	CppSQLite3Query q = db.execQuery(os.str().c_str());
	bool re = !q.eof();
	q.finalize();
	//db.execDML(COMMIT_TRANSACTION);
	return re;
}

bool IMDB::BeExistRecentGroupItem(CppSQLite3DB& db,const tstring& strRoomID)
{
	sprintf_s(m_buf, sizeof(m_buf), _T("select * from Recent_Group_Table where roomId = '%s';"),strRoomID.c_str());
	m_query = db.execQuery(m_buf);
	bool re = !m_query.eof();
	m_query.finalize();

	return re;
}


bool IMDB::BeExistRecentListItem(CppSQLite3DB& db,IM_UID uid)
{
	//m_os.str(_T(""));
	//m_os<<_T("select * from RECENTLIST_TABLE where uid = ")<<uid<<_T(" ;");

	sprintf_s(m_buf, sizeof(m_buf), _T("select * from RECENTLIST_TABLE where uid = '%s';"),uid.c_str());
	//db.execDML(BEGIN_TRANSACTION);
	m_query = db.execQuery(m_buf);
	bool re = !m_query.eof();
	m_query.finalize();
	//db.execDML(COMMIT_TRANSACTION);
	return re;
}

bool IMDB::LoadGroupChatMsg(const tstring& strRoomID, vector<_tGroupChatMessage>& vecGroupMsgs)
{
	vecGroupMsgs.clear();	
	bool bResult = false;
	try
	{
		//得到room表的名称
		tstring tablename;
		tablename  =MSG_GROUP_TABLE_HEAD + strRoomID;

		//打开msg.db数据库
		tstring filename = m_ModuleUinPath;
		filename += MSG_FLLENAME;
		CppSQLite3DB &db = OpenTableTalkMsg();
		if(!db.IsOpen())
		{
			return false;
		}

		//看数据库是否存在，如果不存在就说明没有历史消息
		bResult = db.tableExists(tablename.c_str());
		if(bResult == false)  //说明没有聊天记录
		{
			return true;
		}

		//read it .
		sprintf_s(m_buf, sizeof(m_buf), _T("select * from %s ;"),tablename.c_str());
		m_query = db.execQuery(m_buf);

		//roomId TEXT, senderPhone TEXT, senderAccount TEXT, msgText TEXT, stime TEXT
		while (!m_query.eof())
		{
			_tGroupChatMessage oneMsg;
			oneMsg.strRoomID = m_query.getStringField(_T("roomId"),0);
			oneMsg.strSenderPhone = m_query.getStringField(_T("senderPhone"),0);
			oneMsg.strSenderAccount = m_query.getStringField(_T("senderAccount"),0);
			oneMsg.strMsg    = m_query.getStringField(_T("msgText"),0);

			//get time and converte to tstring.
			time_t t = m_query.getIntField(_T("stime"),0);
			oneMsg.m_timeChat = t;
			oneMsg.strTimeString = GetHMSTimeString(t);
			vecGroupMsgs.push_back(oneMsg);
			m_query.nextRow();
		}

		//lee:此处有异常说q的访问越界了：stack around the variable “q” was corrupte
		//修改策略为：在code generation--basic runtime check:从Both (/RTC1, equiv. to /RTCsu) (/RTC1)修改为Default.
		//q.finalize();
		//lee:db内的指针对象已经为野指针，不可以再执行。

		//db.execDML(COMMIT_TRANSACTION);

		bResult = true;
	}
	catch (CppSQLite3Exception& e)
	{
		const int errorcode = e.errorCode();
		const tstring(e.errorMessage());

	}
	catch(...)
	{
		bResult =  false;

	}

	return bResult;
}

bool IMDB::LoadChatMessage(IM_UID uin, vector<_tChatMessage>& vecMsg)
{
	vecMsg.clear();	
	bool bResult = false;
	try
	{
		tstring tablename;
		tablename  =MSG_TABLE_HEAD + uin;

		tstring filename = m_ModuleUinPath;
		filename += MSG_FLLENAME;
		CppSQLite3DB &db = OpenTableTalkMsg();
		if(!db.IsOpen())
		{
			return false;
		}

		bResult = db.tableExists(tablename.c_str());
		if(bResult == false)  //说明没有聊天记录
		{
			return false;
		}

		//read it .
		sprintf_s(m_buf, sizeof(m_buf), _T("select * from %s ;"),tablename.c_str());
		//db.execDML(BEGIN_TRANSACTION);
		m_query = db.execQuery(m_buf);

		while (!m_query.eof())
		{
			_tChatMessage oneMsg;
			oneMsg.strDestUser = m_query.getStringField(_T("uid"),0);
			oneMsg.strChatText = m_query.getStringField(_T("msgText"),0);
			
			//get time and converte to tstring.
			time_t t = m_query.getIntField(_T("stime"),0);
			oneMsg.strTimeStrFormat = GetHMSTimeString(t);
			vecMsg.push_back(oneMsg);
			m_query.nextRow();
		}

		//lee:此处有异常说q的访问越界了：stack around the variable “q” was corrupte
		//修改策略为：在code generation--basic runtime check:从Both (/RTC1, equiv. to /RTCsu) (/RTC1)修改为Default.
		//q.finalize();
		//lee:db内的指针对象已经为野指针，不可以再执行。
		
		//db.execDML(COMMIT_TRANSACTION);
		
		bResult = true;
	}
	catch (CppSQLite3Exception& e)
	{
		const int errorcode = e.errorCode();
		const tstring(e.errorMessage());
	
	}
	catch(...)
	{
		bResult =  false;
	
	}

	return bResult;
}

bool IMDB::LoadLastGroupChatMsg(const tstring& strRoomID, _tGroupChatMessage& groupMsg)
{
	vector<_tGroupChatMessage> vecMsg;
	if (LoadGroupChatMsg(strRoomID,vecMsg))
	{
		int nSize=vecMsg.size();
		if (nSize>0)
		{
			groupMsg = vecMsg[nSize-1];
			return true;
		}
	}

	return false;
}

bool IMDB::LoadLastMessage(IM_UID uid, _tChatMessage& lastMsg)
{
	vector<_tChatMessage> vecMsg;
	if (LoadChatMessage(uid,vecMsg))
	{
		int nSize=vecMsg.size();
		if (nSize>0)
		{
			lastMsg = vecMsg[nSize-1];
			return true;
		}
	}

	return false;
}

bool IMDB::LoadUserVCard(IM_UID uid,sUserVCard& usrinfo)
{
	try
	{
		bool re;
		tstring  tablename2;
		tablename2  =CONTACTER_TABLE;
		tstring filename2 = m_ModuleUinPath;
		filename2 += CONTACTER_FLLENAME;
		CppSQLite3DB &db = OpenTableContacts();
		if(!db.IsOpen())
		{
			return false;
		}

		re = PrepareTableContacts(db) ;
		if (re != true)
		{
			return re;
		}

// 		sprintf_s(m_buf, sizeof(m_buf), _T("select * from CONTACTER_INFO where uid = '%s';"),uid.c_str());
// 		//m_os.str(_T(""));
// 		//m_os<<_T("select * from CONTACTER_INFO where uid = '")<<uid.c_str()<<_T("' ;");
// 		db.execDML(BEGIN_TRANSACTION);
// 		CppSQLite3Query q = db.execQuery(m_buf/*m_os.str().c_str()*/);
// 		bool re = !q.eof();
// 		q.finalize();
// 		db.execDML(COMMIT_TRANSACTION);



		sprintf_s(m_buf, sizeof(m_buf), _T("select * from CONTACTER_INFO where uid = '%s';"),uid.c_str());

		//m_os.str(_T(""));
		//m_os<<_T("select * from CONTACTER_INFO where uid = ")<<uid.c_str()<<_T(" ;");

		//_stprintf(m_buf,_T("select * from %s ;"),tablename.c_str());
		//db.execDML(BEGIN_TRANSACTION);
		m_query = db.execQuery(m_buf);

		if (!m_query.eof())
		{
// 			typedef struct
// 			{
// 				tstring strUserName;
// 				tstring strUserNickname;
// 				tstring strGender;		//性别
// 				tstring strSignature;	//签名
// 				tstring strPhone;
// 				tstring strEmail;
// 				tstring strAddr;
// 				tstring strPhotoType;	//头像类型
// 				tstring strPhotoBinary;	//头像base64
// 				tstring strUrl;			//头像的Url地址
// 				tstring strPhotoPath;	//在线头像本地路径
// 				tstring strGrayAvatar;	//离线头像本地路径
// 			}sUserVCard;

			usrinfo.strAccount = m_query.getStringField("uid", 0);
			usrinfo.strUserNickname = m_query.getStringField("nick", 0);
			usrinfo.strRemark = m_query.getStringField("remark", 0);
			usrinfo.strGender = m_query.getStringField("gender", 0);
			usrinfo.strSignature = m_query.getStringField("signature", 0);
			usrinfo.strPhone = m_query.getStringField("phone", 0);
			usrinfo.strEmail = m_query.getStringField("email", 0);
			usrinfo.strAddr = m_query.getStringField("address", 0);
			usrinfo.strPhotoType = m_query.getStringField("photoType", 0);
			usrinfo.strPhotoBinary = m_query.getStringField("photoBinary", 0);
			usrinfo.strUrl = m_query.getStringField("photoUrl",0);
			usrinfo.strPhotoPath = m_query.getStringField("photoPath",0);
			usrinfo.strGrayAvatar = m_query.getStringField("photoGrayPath",0);

			re = true;
		}
		else
		{
			re = false;
		}

		//m_query.finalize();

		//db.execDML(COMMIT_TRANSACTION);

		return re;
	}
	catch (CppSQLite3Exception& e)
	{
		const int errorcode = e.errorCode();
		const TSTRING(e.errorMessage());
		//cerr << e.errorCode() << ":" << e.errorMessage() << endl;
		return false;
	}
}

bool IMDB::LoadContactBaseInfo(IM_UID uid,_tUserInfo& usrinfo)
{
	try
	{
		bool re;
		tstring  tablename2;
		tablename2  =CONTACTER_TABLE;
		tstring filename2 = m_ModuleUinPath;
		filename2 += CONTACTER_FLLENAME;
		CppSQLite3DB &db = OpenTableContacts();
		if(!db.IsOpen())
		{
			return false;
		}

		re = PrepareTableContacts(db) ;
		if (re != true)
		{
			return re;
		}


		TSTRING_OSTREAM os;
		os<<_T("select * from CONTACTER_INFO where Fleft_uid = ")<<uid<<_T(" ;");
		//_stprintf(m_buf,_T("select * from %s ;"),tablename.c_str());
		db.execDML(BEGIN_TRANSACTION);
		CppSQLite3Query q = db.execQuery(os.str().c_str());

		if (!q.eof())
		{
			/*
			+---------------+---------------------+------+-----+---------+-------+
			| Field         | Type                | Null | Key | Default | Extra |
			+---------------+---------------------+------+-----+---------+-------+
			| Fleft_uid     | bigint(64) unsigned | NO   | PRI | 0       |       | 
			| Fbind_account | varchar(64)         | YES  |     | NULL    |       | 
			| Freal_name    | varchar(64)         | YES  |     | NULL    |       | 
			| Fsex          | tinyint(3) unsigned | YES  |     | 255     |       | 
			| Fstate        | tinyint(3)          | YES  |     | 0       |       | 
			| Fvip          | tinyint(3)          | YES  |     | 0       |       | 
			| Fhead_pic     | int(11) unsigned    | YES  |     | 0       |       | 
			| Fblock_set    | tinyint(3) unsigned | YES  |     | 0       |       | 
			| Fsigned       | varchar(255)        | YES  |     | NULL    |       | 
			| good_uin      | int(11) unsigned    | NO   |     | NULL    |       | 
			| Fversion      | int(11) unsigned    | YES  |     | 1       |       | 
			| Fsavemsg      | tinyint(3)          | YES  |     | 0       |       | 
			| Fservice_mask | int(11)             | YES  |     | 0       |       | 
			+---------------+---------------------+------+-----+---------+-------+
			*/


			usrinfo.gender  = q.getIntField(_T("Fsex"),0);
			strncpy(usrinfo.name,q.getStringField(_T("Fbind_account"),0),50);
			usrinfo.pic_id =  q.getIntField(_T("Fhead_pic"),0);
			strncpy(usrinfo.signature,q.getStringField(_T("Fsigned"),0),128);
		}

		q.finalize();

		return true;
	}
	catch (CppSQLite3Exception& e)
	{
		const int errorcode = e.errorCode();
		const TSTRING(e.errorMessage());
		//cerr << e.errorCode() << ":" << e.errorMessage() << endl;
		return false;
	}
}

//删除动作只认room_id
bool IMDB::RemoveFromRecentGroup(const tstring& room_id)
{
	try
	{
		bool re;

		//打开数据库：contact.db
		tstring filename2 = m_ModuleUinPath;
		filename2 += CONTACTER_FLLENAME;
		CppSQLite3DB &db2 = OpenTableContacts();
		if(!db2.IsOpen())
		{
			return false;
		}
		//打开group_rencent_table
		re = PrepareTableGroupRecent(db2);
		if (re != true)
		{
			return re;
		}

		//如果不存在就加入新的GroupItem
		if (BeExistRecentGroupItem(db2,room_id))
		{
			RemoveRecentGroupItem(db2,room_id);
		}

		return true;
	}
	catch (CppSQLite3Exception& e)
	{
		const int errorcode = e.errorCode();
		const tstring(e.errorMessage());
		return false;
	}

	return true;
}

bool IMDB::UpdateAvatarInRecentGroup(const tstring& room_id, const tstring& avatar_path)
{
	try
	{
		bool re;

		//打开数据库：contact.db
		tstring filename2 = m_ModuleUinPath;
		filename2 += CONTACTER_FLLENAME;
		CppSQLite3DB &db2 = OpenTableContacts();
		if(!db2.IsOpen())
		{
			return false;
		}
		//打开group_rencent_table
		re = PrepareTableGroupRecent(db2);
		if (re != true)
		{
			return re;
		}

		//如果不存在就加入新的GroupItem
		if (BeExistRecentGroupItem(db2,room_id))
		{
			UpdateAvatarInRecentGroupItem(db2, room_id, avatar_path);
		}

		return true;
	}
	catch (CppSQLite3Exception& e)
	{
		const int errorcode = e.errorCode();
		const tstring(e.errorMessage());
		return false;
	}

	return true;
}

bool IMDB::SaveRecentGroupItem(RecentGroupItem& oneGroup)
{
	try
	{
		bool re;

		//打开数据库：contact.db
		tstring filename2 = m_ModuleUinPath;
		filename2 += CONTACTER_FLLENAME;
		CppSQLite3DB &db2 = OpenTableContacts();
		if(!db2.IsOpen())
		{
			return false;
		}
		//打开group_rencent_table
		re = PrepareTableGroupRecent(db2);
		if (re != true)
		{
			return re;
		}

		//如果不存在就加入新的GroupItem
		if (!BeExistRecentGroupItem(db2,oneGroup.strRoomID))
		{
			AddRecentGroupItem(db2,oneGroup);
		}

		return true;
	}
	catch (CppSQLite3Exception& e)
	{
		const int errorcode = e.errorCode();
		const tstring(e.errorMessage());
		return false;
	}

	return true;
}

//最近联系人
bool IMDB::SaveRecentFriendList(/*vector<RECENTITEM>& recentlist*/RECENTITEM& recent )
{
	try
	{
		bool re;

		tstring  tablename2;
		tablename2  = CONTACTER_TABLE;
		tstring filename2 = m_ModuleUinPath;
		filename2 += CONTACTER_FLLENAME;
		CppSQLite3DB &db2 = OpenTableContacts();
		if(!db2.IsOpen())
		{
			return false;
		}

		re = PrepareTableRecentList(db2);//PrepareTableContacts(db2) ;
		if (re != true)
		{
			return re;
		}

		//vector<RECENTITEM>::iterator iter;
		//for (iter  = recentlist.begin();iter!=recentlist.end();iter++)
		{
			if (BeExistRecentListItem(db2,(IM_UID)recent.strUid))
			{
				//UpdateContactInfo(db2,uid,useinfo);
				UpdateRecentItemTime(db2,recent);
			}
			else
			{
				//AddContactInfo(db2,uid,useinfo);
				AddRecentItem(db2,recent);
			}
		}

		
		return true;
	}
	catch (CppSQLite3Exception& e)
	{
		const int errorcode = e.errorCode();
		const tstring(e.errorMessage());
		return false;
	}
	
	return true;
}

bool IMDB::LoadRecentGroupItem(vector<RecentGroupItem>& recentGroup)
{
	try
	{
		bool re;
		//打开contact.db数据库
		CppSQLite3DB &db2 = OpenTableContacts();
		if(!db2.IsOpen())
		{
			return false;
		}

		//打开Recent_Group_Table表
		re = PrepareTableGroupRecent(db2) ;
		if (re != true)
		{
			return re;
		}
		sprintf_s(m_buf, sizeof(m_buf), _T("select * from %s;"),"Recent_Group_Table");

		RecentGroupItem item;
		m_query = db2.execQuery(m_buf);
		while(!m_query.eof())
		{
			//roomId TEXT, orgId TEXT, orgName TEXT, account TEXT, groupNick TEXT
			item.strRoomID = m_query.getStringField(_T("roomId"),0);
			item.strOrgID = m_query.getStringField(_T("orgId"),0);
			item.strOrgName = m_query.getStringField(_T("orgName"),0);
			item.strParticipant = m_query.getStringField(_T("account"),0);
			item.strGroupNick = m_query.getStringField(_T("groupNick"),0);
			item.strAvatarPath = m_query.getStringField(_T("photoPath"),0);
			recentGroup.push_back(item);
			m_query.nextRow();
		}

		return true;
	}
	catch (CppSQLite3Exception& e)
	{
		const int errorcode = e.errorCode();
		const tstring(e.errorMessage());
		return false;
	}

	return true;
}

bool IMDB::LoadRecentFriendList(vector<RECENTITEM>& recentlist)
{
	try
	{
		bool re;

		tstring  tablename2;
		tablename2  = CONTACTER_TABLE;
		tstring filename2 = m_ModuleUinPath;
		filename2 += CONTACTER_FLLENAME;
		CppSQLite3DB &db2 = OpenTableContacts();
		if(!db2.IsOpen())
		{
			return false;
		}

		re = PrepareTableRecentList(db2) ;
		if (re != true)
		{
			return re;
		}


		sprintf_s(m_buf, sizeof(m_buf), _T("select * from %s;"),"RECENTLIST_TABLE");


        RECENTITEM item;
		//m_os.str(_T(""));
		//m_os<<_T("select * from RECENTLIST_TABLE;");
		m_query = db2.execQuery(m_buf/*m_os.str().c_str()*/);
		//db2.execDML(BEGIN_TRANSACTION);
		while(!m_query.eof())
		{
			item.strUid  = m_query.getStringField(_T("uid"),0);
			item.strTime = m_query.getStringField(_T("sTime"),0);
			recentlist.push_back(item);
			m_query.nextRow();
		}
		
		//q.finalize();
		//db2.execDML(COMMIT_TRANSACTION);
		return true;

		
	}
	catch (CppSQLite3Exception& e)
	{
		const int errorcode = e.errorCode();
		const tstring(e.errorMessage());
		return false;
	}

	return true;
}


bool IMDB::RemoveRecentItem(IM_UID  uid)
{
	try
	{
		bool re;
		tstring  tablename2;
		tablename2  = CONTACTER_TABLE;
		tstring filename2 = m_ModuleUinPath;
		filename2 += CONTACTER_FLLENAME;
		CppSQLite3DB &db2 = OpenTableContacts();
		if(!db2.IsOpen())
		{
			return false;
		}

		re = PrepareTableContacts(db2) ;
		if (re != true)
		{
			return re;
		}

		//RECENTITEM item;
		//TSTRING_OSTREAM os;
		sprintf_s(m_buf, "delete from RECENTLIST_TABLE where uid = '%s';",uid.c_str());
		//os<<_T("delete from RECENTLIST_TABLE where uid = ")<<uid<<_T(" ;");
		//db2.execDML(BEGIN_TRANSACTION);
		db2./*execQuery*/execDML(m_buf/*os.str().c_str()*/);
		//db2.execDML(COMMIT_TRANSACTION);
	}
	catch (CppSQLite3Exception& e)
	{
		const int errorcode = e.errorCode();
		const tstring(e.errorMessage());
		return false;
	}
	return true;
}

bool IMDB::AddRecentItem(RECENTITEM& item)
{
	try
	{
		bool re;

		tstring  tablename2;
		tablename2  = CONTACTER_TABLE;
		tstring filename2 = m_ModuleUinPath;
		filename2 += CONTACTER_FLLENAME;
		CppSQLite3DB &db2 = OpenTableContacts();
		if(!db2.IsOpen())
		{
			return false;
		}

		re = PrepareTableContacts(db2) ;
		if (re != true)
		{
			return re;
		}

		if (BeExistRecentListItem(db2,item.strUid))
		{
			UpdateRecentItemTime(db2,item);
		}
		else
		{
			AddRecentItem(db2,item);
		}
	


		return true;
	}
	catch (CppSQLite3Exception& e)
	{
		const int errorcode = e.errorCode();
		const tstring(e.errorMessage());
		return false;
	}

	return true;
}


bool IMDB::UpdateRecentItemTime(RECENTITEM& item)
{
	return AddRecentItem(item);
}

bool IMDB::AddRecentGroupItem(CppSQLite3DB& db, RecentGroupItem& item)
{
	//roomId TEXT, orgId TEXT, orgName TEXT, account TEXT, groupNick TEXT
	sprintf_s(m_buf, sizeof(m_buf), _T("insert into Recent_Group_Table  values ( '%s','%s','%s' ,'%s' ,'%s' ,'%s');"),
		item.strRoomID.c_str(),item.strOrgID.c_str(), item.strOrgName.c_str(), item.strParticipant.c_str(), item.strGroupNick.c_str(),
		item.strAvatarPath.c_str());
	db.execDML(m_buf);
	return true;
}

bool IMDB::RemoveRecentGroupItem(CppSQLite3DB& db, const tstring& room_id)
{
	sprintf_s(m_buf, "delete from Recent_Group_Table where roomId = '%s';", room_id.c_str());
	db.execDML(m_buf);
	return true;
}

bool IMDB::UpdateAvatarInRecentGroupItem(CppSQLite3DB& db, const tstring& room_id, const tstring& avatar_path)
{
	TSTRING_OSTREAM os;
	TSTRING_OSTREAM os2;
	os<<_T("update Recent_Group_Table set  photoPath = ")
		<<_T("'")<<avatar_path.c_str()<<_T("'") << _T("  where roomId = ")
		<<_T("'")<<room_id.c_str()<<_T("'")<<_T(" ;");
	db.execDML(os.str().c_str());
	return true;
}

bool IMDB::AddRecentItem(CppSQLite3DB& db,RECENTITEM& item)
{
	sprintf_s(m_buf, sizeof(m_buf), _T("insert into RECENTLIST_TABLE  values ( '%s','%s');"),item.strUid.c_str(),item.strTime.c_str());
	db.execDML(m_buf);
// 	m_os.str(_T(""));
// 	m_os<<_T("insert into RECENTLIST_TABLE  values ( ")
// 	<<_T("'")<<item.strUid.c_str()<<_T("'")<<_T(",")
// 	<<_T("'")<<item.strTime.c_str()<<_T("');");
// 	db.execDML(m_os.str().c_str());

	return true;
}

bool IMDB::UpdateRecentItemTime(CppSQLite3DB& db,RECENTITEM& item)
{
	TSTRING_OSTREAM os;
	TSTRING_OSTREAM os2;
	os<<_T("update RECENTLIST_TABLE set  sTime = ")
		<<_T("'")<<item.strTime.c_str()<<_T("'") << _T("  where uid = ")
		<<_T("'")<<item.strUid.c_str()<<_T("'")<<_T(" ;");
	db.execDML(os.str().c_str());
	return true;
}

bool IMDB::BeExistCallHistoryItem(CppSQLite3DB& db,IM_UID uid)
{

	sprintf_s(m_buf, sizeof(m_buf), _T("select * from CALLHISTORY_TABLE where uid = '%s';"),uid.c_str());
	//db.execDML(BEGIN_TRANSACTION);
	m_query = db.execQuery(m_buf);
	bool re = !m_query.eof();
	m_query.finalize();
	//db.execDML(COMMIT_TRANSACTION);
	return re;
}

bool IMDB::BeExistLoginParamItem(CppSQLite3DB& db,IM_UID strPhoneNum)
{
	bool re = false;
	//查询是否有这个人的记录了。
	sprintf_s(m_buf, sizeof(m_buf), _T("select * from LOGIN_PARAM_TABLE where uid = '%s';"),strPhoneNum.c_str());
	m_query = db.execQuery(m_buf);
	re = !m_query.eof();
	m_query.finalize();

	return re;
}

bool IMDB::UpdateLoginParams(CppSQLite3DB& db, tstring strPhoneNum, tstring strPwd, bool bRemPwd, bool bAutoLogin, __int64 nTime)
{
	//uid TEXT, pwd TEXT, bRemePwd INTEGER, bAutoLogin INTEGER
	sprintf_s(m_buf, sizeof(m_buf), 
		_T("update LOGIN_PARAM_TABLE set  pwd = '%s', sTime=%I64d, bRemePwd=%d,bAutoLogin=%d where uid='%s';"), 
		strPwd.c_str(), nTime, bRemPwd?1:0, bAutoLogin?1:0, strPhoneNum.c_str());

	int ret = db.execDML(m_buf);
	return true;
}

bool IMDB::AddLoginParams(CppSQLite3DB& db, tstring& strPhoneNum, tstring& strPwd,  bool bRemPwd, bool bAutoLogin, __int64 sTime)
{
	sprintf_s(m_buf, sizeof(m_buf), _T("insert into  LOGIN_PARAM_TABLE values ( '%s','%s',%I64d,%d,%d);"),
		strPhoneNum.c_str(),
		strPwd.c_str(),
		sTime,
		bRemPwd?1:0, 
		bAutoLogin?1:0);

	int i = db.execDML(m_buf);

	return true;
}


bool IMDB::SaveSendMsgHotKey( int keyMode )
{
	try
	{
		tstring  tablename2;
		tablename2  = SEND_MSG_KEYMODE_TABLE;
		tstring filename2 = m_strUserPath;/*m_ModuleUinPath;*/
		filename2 += OPTIONAL_FLLENAME;/*CONTACTER_FLLENAME;*/
		CppSQLite3DB &db2 = OpenTableGlobalOptional();//OpenTableContacts();
		if(!db2.IsOpen())
		{
			return false;
		}

		if( !db2.tableExists(SEND_MSG_KEYMODE_TABLE))  
		{
			db2.execDML(SEND_MSG_KEYMODE_TABLE_CREATE);
			sprintf_s(m_buf, sizeof(m_buf), _T("insert into  SEND_MSG_KEYMODE_TABLE values ('keymode',%d);"), keyMode);
			db2.execDML(m_buf);
		}
		else
		{
			sprintf_s(m_buf, sizeof(m_buf), _T("update SEND_MSG_KEYMODE_TABLE set  keymode =%d;"), keyMode);
			db2.execDML(m_buf);
		}
	}
	catch (CppSQLite3Exception& e)
	{
		const int errorcode = e.errorCode();
		const tstring(e.errorMessage());
		tstring strMsg = e.errorCodeAsString(errorcode);
		return false;
	}

	return true;
}


bool IMDB::SaveLoginParams(tstring& strPhoneNum, tstring strPwd,  bool bRemPwd, bool bAutoLogin,__int64 sTime)
{
	try
	{
		bool re;

		tstring  tablename2;
		tablename2  = LOGIN_PARA_TABLE;
		tstring filename2 = m_strUserPath;/*m_ModuleUinPath;*/
		filename2 += OPTIONAL_FLLENAME;/*CONTACTER_FLLENAME;*/
		CppSQLite3DB &db2 = OpenTableGlobalOptional();//OpenTableContacts();
		if(!db2.IsOpen())
		{
			return false;
		}

		//查询table是否存在，不存在就先创建一个
		re = db2.tableExists(LOGIN_PARA_TABLE);
		if(re == false)  
		{
			db2.execDML(LOGIN_PARAM_TABLE_CREATE);
		}
	
		//查询这个人是否有记录
		if (BeExistLoginParamItem(db2,(IM_UID)strPhoneNum))
		{
			UpdateLoginParams(db2, strPhoneNum, strPwd, bRemPwd, bAutoLogin, sTime);
		}
		else
		{
			AddLoginParams(db2, strPhoneNum, strPwd, bRemPwd, bAutoLogin, sTime);
		}

		return true;
	}
	catch (CppSQLite3Exception& e)
	{
		const int errorcode = e.errorCode();
		const tstring(e.errorMessage());
		tstring strMsg = e.errorCodeAsString(errorcode);
		return false;
	}
	return true;
}

bool IMDB::GetLoginParams(vector<sLoginUser>& vecLogined)
{
	try
	{
		bool re;

		tstring  tablename2;
		tablename2  = LOGIN_PARA_TABLE;
		tstring filename2 = m_strUserPath/*m_ModuleUinPath*/;
		filename2 += OPTIONAL_FLLENAME/*CONTACTER_FLLENAME*/;
		CppSQLite3DB &db2 = OpenTableGlobalOptional();/*OpenTableContacts();*/
		if(!db2.IsOpen())
		{
			return false;
		}

		//数据库是否存在，不存就创建
		re = db2.tableExists(LOGIN_PARA_TABLE);
		if(re == false)  
		{
			//数据库都没有，就说明没有记住密码或者自动登陆的设置：返回false
			//db2.execDML(LOGIN_PARAM_TABLE_CREATE);
			return false;
		}

		sprintf_s(m_buf, sizeof(m_buf), _T("select * from  LOGIN_PARAM_TABLE;"));
		m_query = db2.execQuery(m_buf);
		//uid TEXT, pwd TEXT, bRemePwd INTEGER, bAutoLogin INTEGER
		while(!m_query.eof())
		{
			sLoginUser oneUser;
			oneUser.strPhoneNum = m_query.getStringField(_T("uid"), 0);
			oneUser.strPwd = m_query.getStringField(_T("pwd"),0);
			oneUser.timeLogin = m_query.getBigIntField(_T("sTime"), 0);
			int nRemPwd = m_query.getIntField(_T("bRemePwd"), 0);
			int nAutoLogin = m_query.getIntField(_T("bAutoLogin"), 0);
			oneUser.bRemPwd = nRemPwd == 0 ? false : true;
			oneUser.bAutoLogin = nAutoLogin == 0 ? false : true;

			vecLogined.push_back(oneUser);

			m_query.nextRow();
		}
		
		m_query.finalize();

		return vecLogined.size()>0;
	}
	catch (CppSQLite3Exception& e)
	{
		const int errorcode = e.errorCode();
		const tstring(e.errorMessage());
		tstring strMsg = e.errorCodeAsString(errorcode);
		return false;
	}
	return true;
}

bool IMDB::SaveCallHistoryFriendList(CallHistoryItem& recent )
{
	try
	{
		bool re;

		tstring  tablename2;
		tablename2  = CALL_HISTORY_TABLE;
		tstring filename2 = m_ModuleUinPath;
		filename2 += CONTACTER_FLLENAME;
		CppSQLite3DB &db2 = OpenTableContacts();
		if(!db2.IsOpen())
		{
			return false;
		}

		re = PrepareTableCallHistoryList(db2);
		if (re != true)
		{
			return re;
		}

		if (BeExistCallHistoryItem(db2,(IM_UID)recent.strUid))
		{
			UpdateCallHistoryTime(db2,recent);
		}
		else
		{
			AddCallHistoryItem(db2,recent);
		}

		return true;
	}
	catch (CppSQLite3Exception& e)
	{
		const int errorcode = e.errorCode();
		const tstring(e.errorMessage());
		tstring strMsg = e.errorCodeAsString(errorcode);
		return false;
	}
	return true;
}

bool IMDB::LoadCallHistoryList(vector<CallHistoryItem>& recentlist)
{
	try
	{
		bool re;

		tstring  tablename2;
		tablename2  = CALL_HISTORY_TABLE;
		tstring filename2 = m_ModuleUinPath;
		filename2 += CONTACTER_FLLENAME;
		CppSQLite3DB &db2 = OpenTableContacts();
		if(!db2.IsOpen())
		{
			return false;
		}

		re = PrepareTableCallHistoryList(db2) ;
		if (re != true)
		{
			return re;
		}


		sprintf_s(m_buf, sizeof(m_buf), _T("select * from %s;"), CALL_HISTORY_TABLE);


		CallHistoryItem item;
		m_query = db2.execQuery(m_buf/*m_os.str().c_str()*/);

		while(!m_query.eof())
		{
			item.strUid  = m_query.getStringField(_T("uid"),0);
			item.u32Time = m_query.getBigIntField(_T("sTime"),0);
			item.eDialIndicate = (EDialIndicate)m_query.getIntField(_T("dialIndicate"),eValideCall); 
			recentlist.push_back(item);
			m_query.nextRow();
		}
		return true;
	}
	catch (CppSQLite3Exception& e)
	{
		const int errorcode = e.errorCode();
		const tstring(e.errorMessage());
		return false;
	}

	return true;
}



bool IMDB::RemoveCallHistoryItem(IM_UID  uid)
{
	try
	{
		bool re;
		tstring  tablename2;
		tablename2  = CALL_HISTORY_TABLE;
		tstring filename2 = m_ModuleUinPath;
		filename2 += CONTACTER_FLLENAME;
		CppSQLite3DB &db2 = OpenTableContacts();
		if(!db2.IsOpen())
		{
			return false;
		}

		re = PrepareTableCallHistoryList(db2) ;
		if (re != true)
		{
			return re;
		}
		
		sprintf_s(m_buf, "delete from CALLHISTORY_TABLE where uid = '%s';",uid.c_str());
		db2./*execQuery*/execDML(m_buf/*os.str().c_str()*/);
	}
	catch (CppSQLite3Exception& e)
	{
		const int errorcode = e.errorCode();
		const tstring(e.errorMessage());
		return false;
	}
	return true;
}

bool IMDB::AddCallHistoryItem(CallHistoryItem& item)
{
	try
	{
		bool re;

		tstring  tablename2;
		tablename2  = CALL_HISTORY_TABLE;
		tstring filename2 = m_ModuleUinPath;
		filename2 += CONTACTER_FLLENAME;
		CppSQLite3DB &db2 = OpenTableContacts();
		if(!db2.IsOpen())
		{
			return false;
		}

		re = PrepareTableCallHistoryList(db2) ;
		if (re != true)
		{
			return re;
		}

		if (BeExistCallHistoryItem(db2,item.strUid))
		{
			//RemoveCallHistoryItem(item.strUid);
			UpdateCallHistoryTime(db2,item);
		}
		else
		{
			AddCallHistoryItem(db2,item);
		}
		return true;
	}
	catch (CppSQLite3Exception& e)
	{
		const int errorcode = e.errorCode();
		const tstring(e.errorMessage());
		return false;
	}

	return true;
}


bool IMDB::UpdateCallHistoryTime(CallHistoryItem& item)
{
	return AddCallHistoryItem(item);
}

bool IMDB::AddCallHistoryItem(CppSQLite3DB& db,CallHistoryItem& item)
{
	sprintf_s(m_buf, sizeof(m_buf), _T("insert into CALLHISTORY_TABLE  values ( '%s','%u',%d);"),item.strUid.c_str(), item.u32Time, item.eDialIndicate);
	int i = db.execDML(m_buf);
	return true;
}

bool IMDB::UpdateCallHistoryTime(CppSQLite3DB& db,CallHistoryItem& item)
{
	/*TSTRING_OSTREAM os;
	TSTRING_OSTREAM os2;
	os<<_T("update CALL_HISTORY_TABLE set  sTime = ")
		<<_T("'")<<item.u32Time<<_T("'") << _T("  where uid = ")
		<<_T("'")<<item.strUid.c_str()<<_T("'")<<_T(" ;");
*/
	sprintf_s(m_buf, sizeof(m_buf), _T("update CALLHISTORY_TABLE set  sTime=%u,dialIndicate=%d where uid='%s';"), item.u32Time, item.eDialIndicate, item.strUid.c_str());

	int ret = db.execDML(m_buf);
	return true;
}

int IMDB::GetSendMsgHotKey()
{
	try
	{
		tstring  tablename2;
		tablename2  = SEND_MSG_KEYMODE_TABLE;
		tstring filename2 = m_strUserPath;/*m_ModuleUinPath;*/
		filename2 += OPTIONAL_FLLENAME;/*CONTACTER_FLLENAME;*/
		CppSQLite3DB &db2 = OpenTableGlobalOptional();//OpenTableContacts();
		if(!db2.IsOpen())
		{
			return false;
		}

		if(!db2.tableExists(SEND_MSG_KEYMODE_TABLE))  
		{
			return 0;
		}
		int keyMode = 0;
		tstring strSql = _T("select * from SEND_MSG_KEYMODE_TABLE where uid = 'keymode'; ");
		m_query = db2.execQuery(strSql.c_str());		
		while(!m_query.eof())
		{
			 keyMode =  m_query.getIntField(_T("keymode"),0);
			m_query.nextRow();
		}	
		return keyMode;
	}
	catch (CppSQLite3Exception& e)
	{
		const int errorcode = e.errorCode();
		const tstring(e.errorMessage());
		tstring strMsg = e.errorCodeAsString(errorcode);
		return false;
	}	
}

bool IMDB::IsFirstLogin()
{
	return m_bFirstLogin;
}

