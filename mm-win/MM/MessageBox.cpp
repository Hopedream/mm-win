
#include "Utility.h"
#include "MessageBox.h"
#include <sstream>

using namespace std;


const TCHAR* mb_minize		= "btn_msgbox_minize";
const TCHAR* mb_close		= "btn_msgbox_close";
const TCHAR* mb_close_2		= "btn_msgbox_close_2";
const TCHAR* mb_text_tips	= "text_msgbox_tips";


CMessageBox::CMessageBox()
{
	m_hMainWnd = NULL;
}

CMessageBox::~CMessageBox()
{

}

void CMessageBox::Notify(TNotifyUI& msg)
{
	if (_tcsicmp(msg.sType, _T("click")) == 0)
	{
		CDuiString strControlName = msg.pSender->GetName();

		//�رս���
		if (_tcsicmp(msg.pSender->GetName(), mb_close) == 0)
		{
			ShowWindow(SW_HIDE);
			//Close();
		}
		if (_tcsicmp(msg.pSender->GetName(), mb_close_2) == 0)
		{
			ShowWindow(SW_HIDE);
		}
		else if (_tcsicmp(msg.pSender->GetName(), mb_minize) == 0)
		{
			ShowWindow(SW_HIDE);
			//Close();
		}
	}
}
//��ȡ�û���Ϣ������ʼ��������
bool CMessageBox::Initialize(tstring& strTips)
{
	//����Icon���

	//����tips����
	CTextUI* pTips = static_cast<CTextUI*>(m_PaintManager.FindControl(mb_text_tips));
	if (NULL != pTips)
	{
		pTips->SetText(strTips.c_str());
	}

	return true;
}

void CMessageBox::CreateWnd()
{
	Create(NULL, _T("MM1.0"), UI_WNDSTYLE_FRAME, WS_EX_STATICEDGE | WS_EX_APPWINDOW, 0, 0, 600, 800);		
	CenterWindow();	
}

//��ʼ������
void CMessageBox::InitWindow()
{
}