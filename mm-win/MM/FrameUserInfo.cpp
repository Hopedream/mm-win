#include "FrameUserInfo.h"
#include "Controller.h"
#include "mainDlg.h"
#include "MagicKHelper.h"
#include <CommDlg.h>
#include "FileHelper.h"
#include "MsgBox.h"
#include "StringHelper.h"
#include "../Common/Logger.h"
//#include "../include\third_party\include\CxImage\ximage.h"

CFrameUserInfo* CFrameUserInfo::m_pInstance = NULL;
CFrameUserInfo::CFrameUserInfo()
{
	CController::Instance()->GetUserInfoMgr()->AddEventHandler((IUserManagerEvent*)this);
}


CFrameUserInfo::~CFrameUserInfo(void)
{
	CController::Instance()->GetUserInfoMgr()->RemoveEventHandler((IUserManagerEvent*)this);
}

void CFrameUserInfo::HandleSetFocusEvent( TNotifyUI& msg )
{

}

void CFrameUserInfo::HandleKillFocusEvent( TNotifyUI& msg )
{

}

void CFrameUserInfo::HandleTextChangedEvent( TNotifyUI& msg )
{

}

void CFrameUserInfo::HandleClickEvent( TNotifyUI& msg )
{
	if (_tcsicmp(msg.pSender->GetName(), _T("btn_close")) == 0)
	{
		this->ShowWindow(false);
		updateinfo();
	}
	else if (_tcsicmp(msg.pSender->GetName(), _T("btn_cancel")) == 0)
	{
		ShowWindow(false);
		updateinfo();
	}
	else if (_tcsicmp(msg.pSender->GetName(), _T("btn_ok")) == 0)
	{
		SavePersonInfo();
	}
	else if (_tcsicmp(msg.pSender->GetName(), _T("btn_select_avatar")) == 0)
	{
		onSelectAvatar();
	}
}

void CFrameUserInfo::HandleItemSelectEvent( TNotifyUI& msg )
{

}

DuiLib::CDuiString CFrameUserInfo::GetSkinFile()
{
	return _T("sys_user_info.xml");
}

CControlUI* CFrameUserInfo::CreateControl( LPCTSTR pstrClass )
{
	return NULL;
}

void CFrameUserInfo::InitWindow()
{
	m_pBtnSelect = static_cast<CButtonUI*>(m_PaintManager.FindControl(_T("btn_select_avatar")));
	m_pEdtName = static_cast<CEditUI*>(m_PaintManager.FindControl(_T("edt_user_name")));
	m_pCombVocation = static_cast<CComboUI*>(m_PaintManager.FindControl(_T("cmb_vocation")));
	updateinfo();
}

CFrameUserInfo* CFrameUserInfo::Instance()
{
	if(NULL == m_pInstance)
	{
		m_pInstance = new CFrameUserInfo;
		m_pInstance->CreateWnd(mainDlg::getInstance()->GetHWND());
	}
	return m_pInstance;
}

void CFrameUserInfo::OnMySelfCardInfo( sUserVCard& oneInfo )
{
	//updateinfo();
	
}

void CFrameUserInfo::OnStoreVCardResult( tstring& strAccount, bool bSucc )
{
	if (bSucc)
	{
		CMsgBox* pMsgBox = new CMsgBox("提示", "修改用户信息成功！",  MSGBOX_ICON_WARNING);
		pMsgBox->ShowModal(this->GetHWND());
		//CController::Instance()->GetUserInfoMgr()->FetchVCardByAccount(m_myUserInfo.strAccount);
		mainDlg::getInstance()->OnMySelfCardInfo(m_myUserInfo);
		ShowWindow(false);
	}
	else 
	{
		CMsgBox* pMsgBox = new CMsgBox("提示", "修改用户信息失败！",  MSGBOX_ICON_WARNING);
		pMsgBox->ShowModal(this->GetHWND());
	}
}

void CFrameUserInfo::onSelectAvatar()
{
	CLogger::GetInstance()->PrintErrLog("onSelectAvatar()");
	m_strAvatar.clear();
	LPCSTR lpFilter = TEXT("Image files (*.jpg;*.bmp;*.png)\0*.jpg;*.bmp;*.png\0\0");
	OPENFILENAME ofn;
	TCHAR szOpenFileNames[80*MAX_PATH];
	TCHAR szPath[MAX_PATH];
	TCHAR szFileName[80*MAX_PATH];

	TCHAR* p;
	int nLen = 0;
	ZeroMemory( &ofn, sizeof(ofn) );
	ofn.lStructSize = sizeof ( ofn );
	ofn.hwndOwner = NULL  ;
	ofn.lpstrFile = szOpenFileNames ;
	ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = sizeof( szOpenFileNames );
	ofn.lpstrFilter = lpFilter;
	ofn.Flags = OFN_EXPLORER |OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
	tstring strPath = "";
	if( GetOpenFileName( &ofn ) )
	{
		strPath = ofn.lpstrFile;
		strPath = CChineseCode::EncodeUTF8(strPath);
	}
	if (strPath != "")
	{
		//1. ui展示
		//tstring strtmp;
		if (m_pBtnSelect != NULL)
		{
			//m_strUpdateLogo = CFileHelper::GetHeadCacheDir() + "tmp.jpeg" ;	


			m_strUpdateLogo = "";
			tstring strID = getTmpFileId();
			m_strUpdateLogo += CFileHelper::GetHeadCacheDir();
			m_strUpdateLogo +=  strID.c_str();//
			m_strUpdateLogo += "tmp.jpeg" ;	
			//CLogger::GetInstance()->PrintErrLog("selected file path = %s", strPath.c_str());
			//CLogger::GetInstance()->PrintErrLog("m_strUpdateLogo path = %s", m_strUpdateLogo);
			if (CMagicKHelper::Instance()->ResizeImage(strPath, m_strUpdateLogo.GetData(), 100, 100))
			{
				m_pBtnSelect->SetBkImage(m_strUpdateLogo);
			}
			else
			{
				CMsgBox* pMsgBox = new CMsgBox("提示", "读取文件失败！",  MSGBOX_ICON_WARNING);
				pMsgBox->ShowModal(this->GetHWND());
				return;
			}
		}
		//2. 
		CMagicKHelper::Instance()->GetPhotoBinval(m_strUpdateLogo.GetData(), m_strAvatar);
		//3.
		m_myUserInfo.strPhotoType = "image/jpeg";
		m_myUserInfo.strPhotoPath =  m_strUpdateLogo.IsEmpty()? m_pBtnSelect->GetBkImage(): m_strUpdateLogo;
		m_myUserInfo.strPhotoBinary = m_strAvatar;
	}
}

void CFrameUserInfo::SavePersonInfo()
{
	if (m_pEdtName->GetText().GetLength() < 1)
	{
		CMsgBox* pMsgBox = new CMsgBox("提示", "用户名称不能为空！",  MSGBOX_ICON_WARNING);
		pMsgBox->ShowModal(this->GetHWND());
		return;
	}
	else
	{
		m_myUserInfo.strUserNickname = m_pEdtName->GetText();
	}
	if (m_pCombVocation->GetText().GetLength() < 1)
	{
		CMsgBox* pMsgBox = new CMsgBox("提示", "请选择所属行业！",  MSGBOX_ICON_WARNING);
		pMsgBox->ShowModal(this->GetHWND());
		return;
	}
	else
	{
		m_myUserInfo.strVocation = m_pCombVocation->GetText();
	}
	CController::Instance()->GetUserInfoMgr()->StoreMyVCard(m_myUserInfo);
	
}

void CFrameUserInfo::updateinfo()
{
	m_myUserInfo = mainDlg::getInstance()->getMyUserInfo();
	//初始化ui
	tstring strlogo = m_myUserInfo.strPhotoPath.empty() ? DEF_AVATAR_NOR :m_myUserInfo.strPhotoPath.c_str();
	m_pEdtName->SetText(m_myUserInfo.strUserNickname.c_str());
	m_pBtnSelect->SetBkImage(strlogo.c_str());
}


