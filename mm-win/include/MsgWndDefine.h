#pragma once

#include <string>
#include "MsgWnd.h"

using namespace std;

enum
{
	WM_MSG_USER_DEF_BEGIN = WM_USER+99,

	//define your message.
	//WM_CONNECT_SUCCESS,
	WM_NEWMSG_RECEIVED, 
	//WM_RECV_ADD_REQUEST,
	WM_GROUPMSG_RECEIVED,
	WM_GROUPMSG_RECVPACKFILE,	//收到群成员发送的资料包信息
	WM_GROUPPACK_UPLOADED,		//群资料包所有的文件上传完毕了

	WM_MSG_USER_DEF_END,

};

extern CMsgWnd* g_pMsgWnd;