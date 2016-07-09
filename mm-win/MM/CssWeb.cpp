#include "CssWeb.h"
#include "mainDlg.h"
#include "webClient.h"
#include "WkeWebkit.h"
#include "CDialogBuilderCallbackEx.h"
#include "MsgBox.h"

jsValue JS_CALL JsMotorType(jsExecState es)
{
	const wchar_t* text = jsToStringW(es, jsArg(es, 0));

 	int length = WideCharToMultiByte(CP_ACP, 0, text, -1, 0, 0, 0, 0 );   
 	char* nowchar = new char[length+1];  
 	memset(nowchar,0,length+1);  
 	WideCharToMultiByte( CP_ACP, 0, text, -1, nowchar,length, 0, 0 ); 

	CMsgBox* pMsgBox = new CMsgBox("测试", nowchar);
	pMsgBox->ShowModal();

	delete nowchar;
	nowchar = NULL;

	return jsUndefined();
}

jsValue JS_CALL JsVin(jsExecState es)
{
	const wchar_t* text = jsToStringW(es, jsArg(es, 0));

	int length = WideCharToMultiByte(CP_ACP, 0, text, -1, 0, 0, 0, 0 );   
	char* nowchar = new char[length+1];  
	memset(nowchar,0,length+1);  
	WideCharToMultiByte( CP_ACP, 0, text, -1, nowchar,length, 0, 0 ); 

	CMsgBox* pMsgBox = new CMsgBox("测试", nowchar);
	pMsgBox->ShowModal();

	delete nowchar;
	nowchar = NULL;

	return jsUndefined();
}

CssWeb* CssWeb::m_pInstance = NULL;
CssWeb::CssWeb()
{

}


CssWeb::~CssWeb(void)
{
// 	delete m_pInstance;
// 	m_pInstance = NULL;
}

CssWeb* CssWeb::Instance()
{
	if(NULL == m_pInstance)
	{
		m_pInstance = new CssWeb();
		m_pInstance->CreateWnd();

		m_pInstance->web_kit_ui_ = static_cast<CWkeWebkitUI*>(m_pInstance->m_PaintManager.FindControl(_T("ie")));

		RECT rect;
		rect.left = 0;
		rect.top = 30;	
		rect.right = rect.left + 680;
		rect.bottom = rect.top + 555;
		m_pInstance->web_kit_ui_->SetPos(rect);

		jsBindFunction("bindMotorType", JsMotorType, 1);//这里绑定js函数，让js主动调用c++函数
		jsBindFunction("bindVin", JsVin, 1);//这里绑定js函数，让js主动调用c++函数
//		m_pInstance->CreateWnd(mainDlg::getInstance()->GetHWND());
	}
	return m_pInstance;
}

void CssWeb::HandleSetFocusEvent( TNotifyUI& msg )
{

}

void CssWeb::HandleKillFocusEvent( TNotifyUI& msg )
{

}

void CssWeb::HandleTextChangedEvent( TNotifyUI& msg )
{

}

void CssWeb::HandleClickEvent( TNotifyUI& msg )
{
	if (_tcsicmp(msg.pSender->GetName(), _T("buttonClose")) == 0)
	{
		ShowWindow(false);
	}
	else if (_tcsicmp(msg.pSender->GetName(), _T("buttonOK")) == 0)
	{
		ShowWindow(false);
	}
}

DuiLib::CDuiString CssWeb::GetSkinFile()
{
	return _T("css_web.xml");
}

void CssWeb::InitWindow()
{
	//CWebClient::getRegions();
	tstring strtmp = CWebClient::getRegions();
	int i=0;
}

LRESULT CssWeb::HandleCustomMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
	return 0;
}

void CssWeb::SetParams(int webtype, const tstring& url)
{	
	web_kit_ui_->SetURL(url);
	web_kit_ui_->SetInternVisible();
}

CControlUI* CssWeb::CreateControl( LPCTSTR pstrClass )
{
	if (_tcsicmp(pstrClass, _T("wkeWebkit")) == 0)
	{
		return new CWkeWebkitUI();
	}

	return NULL;
}
