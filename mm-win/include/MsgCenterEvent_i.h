#pragma once

#include "..\Common\MMDefine.h"
#include <string>

/**
* �ýӿڽ��մ�sessionģ�����������ԭʼ��Ϣ,
* �ýӿ���Ҫ֧�ֵ���ϢӦ�ð���������˭�����͵����ݣ��ı���Ϣ��ϵͳ������Ϣ���Զ�����飬Image��
* Ŀǰ�ýӿ�ֻ֧���ı���Ϣ����Ҫ��չ��
*/
class IMsgCenterModuleEvent
{
public:
	virtual void OnReceiveMessage(CMsgBase* pRecvMsg, EMsgType eMsgType) = 0;
	//virtual void OnReceiveCmdMsg(tstring& strFrom, ECmdType eType, tstring& strParas) = 0;
};
