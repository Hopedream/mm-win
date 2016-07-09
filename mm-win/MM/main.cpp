
#pragma once
#include "..\DuiLib\UIlib.h"
/*#include "..\DuiLib\Utils\WinImplBase.h"*/
#include "StdAfx.h"
#include <atlbase.h>
CComModule _Module;
#include <atlwin.h>

#include "..\include\TrayTaskModule.h"
//Msg window.
#include "..\include\MsgWndDefine.h"
#include "..\include\GDIPlusLib.h"

#include "MsgWnd.h"

#include "RegisterHelper.h"
#include "MsgSessionMgr.h"
#include "Login.h"
#include "mainDlg.h"
#include "loginDlg.h"
#include "oss_interface.h"
#include "../Common/Logger.h"


//imagemagic++
#include <Magick++.h>
#include <string>
#include <iostream>

#include "WkeWebkit.h"

CGDIPlusInitializer* g_pGDIPlus = NULL;
CMsgWnd* g_pMsgWnd = NULL;
class ITrayTaskModule;

int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
	CPaintManagerUI::SetInstance(hInstance);
	//CPaintManagerUI::SetResourcePath(CPaintManagerUI::GetInstancePath());

	HINSTANCE hInstRich = ::LoadLibrary(_T("Riched20.dll"));

	::CoInitialize(NULL);
	::OleInitialize(NULL);
	CWkeWebkitUI::WkeWebkit_Init();
	_Module.Init( 0, hInstance );

#if defined(WIN32) && !defined(UNDER_CE)
	HRESULT Hr = ::CoInitialize(NULL);
#else
	HRESULT Hr = ::CoInitializeEx(NULL, COINIT_MULTITHREADED);
#endif
	if( FAILED(Hr) ) return 0;

	CLogger::GetInstance()->Init();
	//Init global oss
	ossInterface::InitGlobal();
	//Create message window.
	g_pMsgWnd = new CMsgWnd;
	g_pMsgWnd->Create(NULL, _T("MM1.0"), UI_WNDSTYLE_FRAME, WS_EX_STATICEDGE | WS_EX_APPWINDOW, 0, 0, 10, 10);
	::ShowWindow(*g_pMsgWnd, SW_HIDE);

	//Global Initialize
	if (NULL == g_pGDIPlus)
	{
		g_pGDIPlus = new CGDIPlusInitializer;
	}

	//Init Traytask manager
	GetTrayTaskModule()->Initialize(hInstance);
	loginDlg* pFrame = new loginDlg();
	//if(pFrame->NeedUpdate())
	//{
	//	::ShellExecute(NULL, NULL, "MMUpgrade.exe", NULL, NULL, SW_SHOWNORMAL);
	//}
	//else
	{
		pFrame->CreateLoginDlg();
		CPaintManagerUI::MessageLoop();
	}
	CPaintManagerUI::Term();

	_Module.Term();
	CWkeWebkitUI::WkeWebkit_Shutdown();
	::OleUninitialize();
	::CoUninitialize();
	
	//free gdi+
	if (g_pGDIPlus != NULL)
	{
		delete g_pGDIPlus;
		g_pGDIPlus = NULL;
	}

	//
	::FreeLibrary(hInstRich);

	ossInterface::deinitializeOSS();
	CLogger::GetInstance()->DestroyLogger();
	return 0;

}
