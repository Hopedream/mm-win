#pragma once
#include "..\DuiLib\UIlib.h"
#include <atlbase.h>
#include <atlwin.h>
#include <string>
#include <iostream>
#include "uWindow.h"

using namespace DuiLib;
using namespace std;

int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
	CPaintManagerUI::SetInstance(hInstance);

	HRESULT Hr = ::CoInitialize(NULL);
	if( FAILED(Hr) ) return 0;


	uWindow* pWnd = new uWindow();
	pWnd->CreateWnd();
	pWnd->DoUpdate();//执行更新用这个
	//pWnd->DoUpload("103"); //上传文件这个
	CPaintManagerUI::MessageLoop();
	CPaintManagerUI::Term();
	::CoUninitialize();
	return 0;
}
