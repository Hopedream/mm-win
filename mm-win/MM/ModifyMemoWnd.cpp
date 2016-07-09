#include "ModifyMemoWnd.h"
#include <sstream>
#include "..\Include\Global_Def.h"

using namespace std;

const TCHAR* memo_btn_close			= "btn_memo_close";
const TCHAR* memo_btn_ok			= "btn_memo_ok";
const TCHAR* memo_edit				= "edit_memo_content";



CModifyMemoWnd::CModifyMemoWnd()
{

}

CModifyMemoWnd::~CModifyMemoWnd()
{

}

void CModifyMemoWnd::Notify(TNotifyUI& msg)
{
	if (_tcsicmp(msg.sType, _T("click")) == 0)
	{
		CDuiString strControlName = msg.pSender->GetName();

		//关闭界面
		if (_tcsicmp(msg.pSender->GetName(), memo_btn_close) == 0)
		{
			PostMessage(WM_CLOSE);
		}
		else if (_tcsicmp(msg.pSender->GetName(), memo_btn_ok) == 0)
		{
			//修改备注名
			CEditUI* pEditMemo = static_cast<CEditUI*>(m_PaintManager.FindControl(memo_edit));
			if (NULL != pEditMemo)
			{
				m_strNewMemo = pEditMemo->GetText();
			}

			if (_tcsicmp(m_strNewMemo.c_str(), m_strCurMemo.c_str())==0)
			{

			}
			else
			{
				char* pAccount = new char[256];
				memset(pAccount, 0, 256);
				strcpy(pAccount, m_strAccount.c_str());

				char* pMemo =new char[256];
				memset(pMemo, 0, 256);
				strcpy(pMemo, m_strNewMemo.c_str());

				::SendMessage(m_hMainWnd, WM_MEMO_UPDATE, (WPARAM)pAccount, (LPARAM)pMemo);
			}
			
			PostMessage(WM_CLOSE);
			//PostMessage(WM_CLOSE, (WPARAM)MB_RETURN_OK);
		}
	}
}
//获取用户信息，并初始化到界面

bool CModifyMemoWnd::InitMemo(tstring& strAccount, tstring& strMemo)
{
	m_strAccount = strAccount;

	if (strMemo.empty())
	{
		m_strCurMemo = "无";
		return false;
	}

	m_strCurMemo = strMemo;
	

	return true;
}

UINT CModifyMemoWnd::ShowModal(HWND hParnt)
{
	Create(m_hWnd, (_T("")), UI_WNDSTYLE_DIALOG, UI_WNDSTYLE_EX_FRAME, 0, 0, 0, 0, NULL);		
	CenterWindow();

	//初始化备注
	CEditUI* pEditMemo = static_cast<CEditUI*>(m_PaintManager.FindControl(memo_edit));
	if (NULL != pEditMemo)
	{
		pEditMemo->SetText(m_strCurMemo.c_str());
	}

	return CWindowWnd::ShowModal();
}

void CModifyMemoWnd::ShowWindow(HWND hParent)
{
	m_hMainWnd = hParent;

	Create(hParent, _T("mmemo"), 
		WS_EX_TOOLWINDOW | WS_EX_TOPMOST, 
		WS_EX_TOOLWINDOW, 
		0, 0, 398,200);
	CenterWindow();

	//初始化备注
	CEditUI* pEditMemo = static_cast<CEditUI*>(m_PaintManager.FindControl(memo_edit));
	if (NULL != pEditMemo)
	{
		pEditMemo->SetText(m_strCurMemo.c_str());
	}

	return CWindowWnd::ShowWindow();
}

//初始化窗口
void CModifyMemoWnd::InitWindow()
{
}