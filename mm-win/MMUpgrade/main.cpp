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
	pWnd->DoUpdate();//ִ�и��������
	//pWnd->DoUpload("103"); //�ϴ��ļ����
	CPaintManagerUI::MessageLoop();
	CPaintManagerUI::Term();
	::CoUninitialize();
	return 0;
}
