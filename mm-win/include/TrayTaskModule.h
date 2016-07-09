#pragma once

#include "..\Common\MMDefine.h"

/*
* ģ�����ӿڶ���
*/

class ITrayTaskModuleEvent;
class ITrayTaskModule
{
public:
	virtual void AddEventHandler(ITrayTaskModuleEvent* pEventHandler) = 0;
	virtual void Release() = 0;
	virtual void Initialize(HINSTANCE hInstance) = 0;
	virtual void Uninitialize() = 0;
	virtual BOOL IsFlashing() = 0;
	virtual void Flash(tstring strAccount, EMsgType eType) = 0;
	virtual void FlashGroup(tstring& strRoomId) = 0;
	virtual void StopFlash(tstring strAccount, EMsgType eType) = 0;
	virtual void StopGroupFlash(tstring& strRoomID) = 0;
};

/**
* �ṩ���ⲿʹ�õ�ͳһģ��ӿ�
*/
ITrayTaskModule* GetTrayTaskModule();

