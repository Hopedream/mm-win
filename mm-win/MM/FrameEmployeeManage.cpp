#include "FrameEmployeeManage.h"
#include "mainDlg.h"
#include "Controller.h"
#include "StringHelper.h"
#include "UserManager.h"
#include "mainDlg.h"
#include "../Common/regexHelper.h"
#include "MsgBox.h"


CFrameEmployeeManage* CFrameEmployeeManage::m_pInstance = NULL;
CFrameEmployeeManage::CFrameEmployeeManage()
{
	m_bHaveOrg = false;
	CController::Instance()->GetUserInfoMgr()->AddEventHandler(this);
}


CFrameEmployeeManage::~CFrameEmployeeManage(void)
{
}

void CFrameEmployeeManage::HandleSetFocusEvent( TNotifyUI& msg )
{

}

void CFrameEmployeeManage::HandleKillFocusEvent( TNotifyUI& msg )
{

}

void CFrameEmployeeManage::HandleTextChangedEvent( TNotifyUI& msg )
{

}

void CFrameEmployeeManage::HandleClickEvent( TNotifyUI& msg )
{
	if (_tcsicmp(msg.pSender->GetName(), _T("buttonClose")) == 0)
	{
		ShowWindow(false);
		InitWindow();
	}
	else if (_tcsicmp(msg.pSender->GetName(), _T("buttonOK")) == 0)
	{
		onbtnOk();
	}
	else if (_tcsicmp(msg.pSender->GetName(), _T("buttonCancel")) == 0)
	{
		ShowWindow(false);
		InitWindow();
	}
	else if (_tcsicmp(msg.pSender->GetName(), _T("buttonremove")) == 0)
	{
		tstring strPhone = msg.pSender->GetParent()->GetParent()->GetParent()->GetName();
		//删除
		//int i=0;
		//调用web端接口
		CHttpClient httpClient;
		tstring strUrl(WEB_SERVER_BASE_URL);
		strUrl += _T("org/del-member?");
		tstring strPost = _T("");
		strPost += _T("oid=");
		tstring strNameDefault = m_strOrgId;
		strPost += strNameDefault;
		strPost += _T("&cell_phone=") + strPhone; // + "]";           //110000,110100,110101");
		strUrl += strPost;
		tstring strToken = CController::Instance()->GetToken();
		tstring strHttpResponse;
		int retCode = httpClient.Get(CChineseCode::EncodeUTF8(strUrl), strToken, strHttpResponse);
		if (strHttpResponse.find("\"result\":1") == -1)
		{
			CMsgBox* pMsgBox = new CMsgBox("删除", "解除关系失败",  MSGBOX_ICON_WARNING);
			pMsgBox->ShowModal(this->GetHWND());
			return;
		}
		else
		{
			//删除成功
			InitWindow();
			//CController::Instance()->GetUserInfoMgr()->FetchVCardByAccount(strPhone);
			//m_vctMembers.push_back(info);
		}	

	}
	else if (_tcsicmp(msg.pSender->GetName(), _T("buttonTransfer")) == 0)
	{
		tstring strPhone = msg.pSender->GetParent()->GetParent()->GetParent()->GetName();
		if (!TransferOwnerPermissions(strPhone))
		{
			CMsgBox* pMsgBox = new CMsgBox("转让", "转让老板权限失败",  MSGBOX_ICON_WARNING);
			pMsgBox->ShowModal(this->GetHWND());
			return;
		}
		else 
		{
			CMsgBox* pMsgBox = new CMsgBox("转让", "转让老板权限成功",  MSGBOX_ICON_WARNING);
			pMsgBox->ShowModal(this->GetHWND());
			ShowWindow(false);
		}
	}

	else if (_tcsicmp(msg.pSender->GetName(), _T("buttonAdd")) == 0)
	{
		tstring strPhone = m_pEditPhone->GetText();
		if (!CRegexHelper::IsValidMobileNo(strPhone.c_str()))
		{
			CMsgBox* pMsgBox = new CMsgBox("查找", "输入的手机号码格式错误",  MSGBOX_ICON_WARNING);
			pMsgBox->ShowModal(this->GetHWND());
			return;
		}
		for (int i=0; i<m_vctMembers.size(); i++)
		{
			if (m_vctMembers[i].strPhone == strPhone)
			{
				CMsgBox* pMsgBox = new CMsgBox("查找", "账号已添加，请不要重复添加",  MSGBOX_ICON_WARNING);
				pMsgBox->ShowModal(this->GetHWND());
				return;
			}
		}
		OrgMemberInfo info;
		info.strAccount = strPhone + "@test-d";
		info.strPhone = strPhone;
		info.m_bVerify = true;
				
		//调用web端接口
		CHttpClient httpClient;
		tstring strUrl(WEB_SERVER_BASE_URL);
		strUrl += _T("org/add-members?");
		tstring strPost = _T("");
		strPost += _T("oid=");
		tstring strNameDefault = m_strOrgId;
		strPost += strNameDefault;
		strPost += _T("&cell_phones=[") + strPhone + "]";           //110000,110100,110101");
		strUrl += strPost;
		tstring strToken = CController::Instance()->GetToken();
		tstring strHttpResponse;
		int retCode = httpClient.Get(CChineseCode::EncodeUTF8(strUrl), strToken, strHttpResponse);
		if (strHttpResponse.find("\"result\":1") == -1)
		{
			CMsgBox* pMsgBox = new CMsgBox("添加", "添加失败，请确认对方没有加入其他机构！",  MSGBOX_ICON_WARNING);
			pMsgBox->ShowModal(this->GetHWND());
			return;
		}
		else
		{
			//成功
			InitWindow();
			//CController::Instance()->GetUserInfoMgr()->FetchVCardByAccount(strPhone);
			//m_vctMembers.push_back(info);
		}	
	}
}

void CFrameEmployeeManage::HandleItemSelectEvent( TNotifyUI& msg )
{

}

DuiLib::CDuiString CFrameEmployeeManage::GetSkinFile()
{
	return _T("sys_employee_manage.xml");
}

CControlUI* CFrameEmployeeManage::CreateControl( LPCTSTR pstrClass )
{
	return NULL;
}

void CFrameEmployeeManage::InitWindow()
{
	m_pEditPhone = static_cast<CEditUI *>(m_PaintManager.FindControl(_T("editPhone")));
	CDuiString str;
	m_plstEmployee = static_cast<CListUI*>(m_PaintManager.FindControl(_T("lstEmployee")));
	//tstring strOrgId;
	getOrgInfo(m_strOrgId);

	m_vctMembers.clear();
	getOrgMembers(m_strOrgId, m_vctMembers);

	//CListContainerElementUI *pListElement = new CListContainerElementUI;
	
}	

CFrameEmployeeManage* CFrameEmployeeManage::Instance()
{
	if(NULL == m_pInstance)
	{
		m_pInstance = new CFrameEmployeeManage;
		m_pInstance->CreateWnd(mainDlg::getInstance()->GetHWND());
	}
	return m_pInstance;
}

bool CFrameEmployeeManage::createDefaultOrg()
{
	CHttpClient httpClient;
	tstring strUrl(WEB_SERVER_BASE_URL);
	strUrl += _T("org/create-org?");
	tstring strPost = _T("");
	strPost += _T("name=");
	tstring strNameDefault = _T("默认组织");
	strPost += strNameDefault;
	strPost += _T("&address=110000,110100,110101");
	strPost += _T("&cate=1");
	strUrl += strPost;
	tstring strToken = CController::Instance()->GetToken();
	tstring strHttpResponse;
	int retCode = httpClient.Get(CChineseCode::EncodeUTF8(strUrl), strToken, strHttpResponse);
	if (strHttpResponse.find("\"result\":1") == -1)
	{
		m_bHaveOrg = true;
		return true;
	}
	else
	{
		m_bHaveOrg = false;	
		return false;
	}
}

bool CFrameEmployeeManage::getOrgInfo(tstring& strOrgId)
{
	CHttpClient httpClient;
	tstring strUrl(WEB_SERVER_BASE_URL);
	strUrl += _T("org/get-org");
	tstring strPost = _T("");
	tstring strToken = CController::Instance()->GetToken();
	tstring strHttpResponse;
	int retCode = httpClient.Post(strUrl, strPost, strToken, strHttpResponse);
	tstring strId = strHttpResponse.substr(strHttpResponse.find("\"id\":\"") + 6, strHttpResponse.length());
	strId = strId.substr(0, strId.find("\",\""));
	strOrgId = strId;
	return true;
}

bool CFrameEmployeeManage::getOrgMembers(tstring strOrgId, vector<OrgMemberInfo>& vctMembers)
{
	getRecipient();
	CHttpClient httpClient;
	tstring strUrl(WEB_SERVER_BASE_URL);
	strUrl += _T("org/get-org-members?");
	tstring strPost = _T("oid=") + strOrgId;
	strUrl += strPost;
	tstring strToken = CController::Instance()->GetToken();
	tstring strHttpResponse;
	int retCode = httpClient.Get(strUrl, strToken, strHttpResponse);
	tstring strRight = strHttpResponse;
	int nIndex = strRight.find("}");
	while (nIndex > 0)
	{
		tstring strOneItem = strRight.substr(strRight.find("{") + 1, (strRight.find("}") - strRight.find("{")) -1);
		//
		OrgMemberInfo oneInfo;

		tstring strCellPhone = strOneItem.substr(strOneItem.find("\"cell_phone\":\"") + 14, strOneItem.length());
		strCellPhone = strCellPhone.substr(0, strCellPhone.find("\",\""));
		oneInfo.strPhone = strCellPhone;

		tstring strNickName = strOneItem.substr(strOneItem.find("\"nick\":\"") + 8, strOneItem.length());
		strNickName = strNickName.substr(0, strNickName.find("\",\""));
		oneInfo.strNickName = strNickName;

		tstring strAccount = strOneItem.substr(strOneItem.find("\"name\":\"") + 8, strOneItem.length());
		strAccount = strAccount.substr(0, strAccount.find("\",\""));
		oneInfo.strAccount = strAccount;
		CController::Instance()->GetUserInfoMgr()->FetchVCardByAccount(strCellPhone);		
		oneInfo.m_bVerify = true;
		getDuty(oneInfo);
		m_vctMembers.push_back(oneInfo);

		strRight = strRight.substr(strRight.find("}") + 1, strRight.length());
		nIndex = strRight.find("}");
	}

	return true;
}

bool CFrameEmployeeManage::AddItem(OrgMemberInfo memInfo)
{
	CDialogBuilder DlgBuilder;
	CListContainerElementUI *plstItem = NULL;
	if (!DlgBuilder.GetMarkup()->IsValid())
	{
		plstItem = static_cast<CListContainerElementUI*>(DlgBuilder.Create(_T("sys_employee_manage_item.xml"),
			(UINT)0, NULL, &m_PaintManager));
	}
	else
	{
		plstItem = static_cast<CListContainerElementUI*>(DlgBuilder.Create((UINT)0, &m_PaintManager));
	}
	if (plstItem == NULL)
		return false;
	else
	{
		CLabelUI *plabelPhone = static_cast<CLabelUI *>(m_PaintManager.FindSubControlByName(plstItem, _T("labelcellphone")));
		if (plabelPhone!= NULL)
			plabelPhone->SetText(memInfo.strPhone.c_str());
		plstItem->SetName(memInfo.strPhone.c_str());
		CLabelUI *plablNick = static_cast<CLabelUI *>(m_PaintManager.FindSubControlByName(plstItem, _T("labelNick")));
		if (plablNick != NULL)
		{
			// 如果员工昵称不为空显示昵称
			if (memInfo.strNickName == "")
			{
				plablNick->SetText(memInfo.strPhone.c_str());
			}
			else
				plablNick->SetText(memInfo.strNickName.c_str());
		}

		CControlUI *pControlLogo = static_cast<CControlUI *>(m_PaintManager.FindSubControlByName(plstItem, _T("controlLogo")));
		if (pControlLogo != NULL)
		{
			bool bExist = PathFileExists(memInfo.strLogo.c_str());
			if (!bExist || memInfo.strLogo.empty())
			{
				pControlLogo->SetBkImage(DEF_AVATAR_NOR);
			}
			else
			{
				pControlLogo->SetBkImage(memInfo.strLogo.c_str());
			}
		}

		COptionUI *pOptionRecipient = static_cast<COptionUI*> (m_PaintManager.FindSubControlByName(plstItem, _T("option_recipient")));
		if (pOptionRecipient != NULL)
		{
			if (m_strRecipient == memInfo.strPhone)
				pOptionRecipient->Selected(true);
			else
				pOptionRecipient->Selected(false);
		}

		COptionUI *pOptionrequirement = static_cast<COptionUI*> (m_PaintManager.FindSubControlByName(plstItem, _T("Option_requirement")));
		if (pOptionrequirement != NULL)
		{
			if (memInfo.brequirement)
				pOptionRecipient->Selected(true);
			else
				pOptionRecipient->Selected(false);
		}
		COptionUI *pOptionEngage = static_cast<COptionUI*> (m_PaintManager.FindSubControlByName(plstItem, _T("Option_engage")));
		if (pOptionEngage != NULL)
		{
			if (memInfo.bengage)
				pOptionEngage->Selected(true);
			else
				pOptionEngage->Selected(false);
		}
		COptionUI *pOptionexecute = static_cast<COptionUI*> (m_PaintManager.FindSubControlByName(plstItem, _T("Option_execute")));
		if (pOptionexecute != NULL)
		{
			if (memInfo.bexecute)
				pOptionexecute->Selected(true);
			else
				pOptionexecute->Selected(false);
		}
		COptionUI *pOptionaftersale = static_cast<COptionUI*> (m_PaintManager.FindSubControlByName(plstItem, _T("Option_aftersale")));
		if (pOptionaftersale != NULL)
		{
			if (memInfo.baftersale)
				pOptionaftersale->Selected(true);
			else
				pOptionaftersale->Selected(false);
		}

		plstItem->SetFixedHeight(40);
		if (!m_plstEmployee->Add(plstItem))
		{
			delete plstItem;
			return false;
		}
		return true;
	}
}

void CFrameEmployeeManage::OnStrangerVCard( tstring& strAccount, sUserVCard& oneInfo )
{
	for (int i=0; i<m_vctMembers.size(); i++)
	{
		if ((m_vctMembers[i].strPhone) == oneInfo.strPhone)
		{
			m_vctMembers[i].strLogo = oneInfo.strPhotoPath;
			m_vctMembers[i].strNickName = oneInfo.strUserNickname;
			m_vctMembers[i].strAccount = oneInfo.strAccount;
			addItems();
		}
	}
}

void CFrameEmployeeManage::OnMySelfCardInfo( sUserVCard& oneInfo )
{
	//tstring strAccount = mainDlg::getInstance()->getMyselfItem().m_strAccount;
	for (int i=0; i<m_vctMembers.size(); i++)
	{
		if ((m_vctMembers[i].strPhone) == oneInfo.strPhone)
		{
			m_vctMembers[i].strLogo = oneInfo.strPhotoPath;
			m_vctMembers[i].strNickName = oneInfo.strUserNickname;
			m_vctMembers[i].strAccount = oneInfo.strAccount;
			addItems();

		}
	}
}

void CFrameEmployeeManage::OnVCardReturned( tstring& strAccount, sUserVCard& oneInfo )
{
	for (int i=0; i<m_vctMembers.size(); i++)
	{
		if ((m_vctMembers[i].strPhone) == oneInfo.strPhone)
		{
			m_vctMembers[i].strLogo = oneInfo.strPhotoPath;
			m_vctMembers[i].strNickName = oneInfo.strUserNickname;
			m_vctMembers[i].strAccount = oneInfo.strAccount;
			addItems();
		}
	}
}

bool CFrameEmployeeManage::AddWaitItem( OrgMemberInfo memInfo )
{
	CDialogBuilder DlgBuilder;
	CListContainerElementUI *plstItem = NULL;
	if (!DlgBuilder.GetMarkup()->IsValid())
	{
		plstItem = static_cast<CListContainerElementUI*>(DlgBuilder.Create(_T("sys_employee_manage_wait_item.xml"),
			(UINT)0, NULL, &m_PaintManager));
	}
	else
	{
		plstItem = static_cast<CListContainerElementUI*>(DlgBuilder.Create((UINT)0, &m_PaintManager));
	}
	if (plstItem == NULL)
		return false;
	else
	{
		CLabelUI *plabelPhone = static_cast<CLabelUI *>(m_PaintManager.FindSubControlByName(plstItem, _T("labelcellphone")));
		if (plabelPhone!= NULL)
			plabelPhone->SetText(memInfo.strPhone.c_str());
		plstItem->SetName(memInfo.strPhone.c_str());
		CLabelUI *plablNick = static_cast<CLabelUI *>(m_PaintManager.FindSubControlByName(plstItem, _T("labelNick")));
		if (plablNick != NULL)
		{
			plablNick->SetText(memInfo.strNickName.c_str());
		}

		CControlUI *pControlLogo = static_cast<CControlUI *>(m_PaintManager.FindSubControlByName(plstItem, _T("controlLogo")));
		if (pControlLogo != NULL)
		{
			bool bExist = PathFileExists(memInfo.strLogo.c_str());
			if (!bExist || memInfo.strLogo.empty())
			{
				pControlLogo->SetBkImage(DEF_AVATAR_NOR);
			}
			else
			{
				pControlLogo->SetBkImage(memInfo.strLogo.c_str());
			}
		}


		plstItem->SetFixedHeight(40);
		if (!m_plstEmployee->Add(plstItem))
		{
			delete plstItem;
			return false;
		}
		return true;
	}
}

void CFrameEmployeeManage::addItems()
{
	m_plstEmployee->RemoveAll();
	for (int i=0; i<m_vctMembers.size(); i++)
	{
		AddItem(m_vctMembers[i]);
	}
}

bool CFrameEmployeeManage::TransferOwnerPermissions( tstring strPhone )  //todo
{

	CHttpClient httpClient;
	tstring strUrl(WEB_SERVER_BASE_URL);
	strUrl += _T("org/transfer-admin?");
	tstring strPost = _T("");
	strPost += _T("oid=");
	strPost += m_strOrgId;
	strPost += _T("&cell_phone=");
	strPost += strPhone;
	strUrl += strPost;
	tstring strToken = CController::Instance()->GetToken();
	tstring strHttpResponse;
	int retCode = httpClient.Get(strUrl, strToken, strHttpResponse);
	if (strHttpResponse.find("\"result\":0") != -1)
	{
		return false;
	}
	else
	{
		return true;
	}
}

void CFrameEmployeeManage::HandleSelectedChangedEvent( TNotifyUI& msg )
{
	if (_tcsicmp(msg.pSender->GetName(), _T("option_recipient")) ==0 )
	{
		m_strRecipient = msg.pSender->GetParent()->GetParent()->GetParent()->GetName();
	}
	else if (_tcsicmp(msg.pSender->GetName(), _T("Option_requirement")) == 0)
	{
		COptionUI *pOptn = static_cast<COptionUI*>(msg.pSender);
		bool bSuc = pOptn->IsSelected();
		tstring strPhone = msg.pSender->GetParent()->GetParent()->GetParent()->GetName();
		for (int i=0; i< m_vctMembers.size(); i++)
		{
			if (m_vctMembers[i].strPhone == strPhone)
			{
				m_vctMembers[i].brequirement = bSuc;
				break;
			}
		}
		
	}
	else if (_tcsicmp(msg.pSender->GetName(), _T("Option_engage")) == 0)
	{
		COptionUI *pOptn = static_cast<COptionUI*>(msg.pSender);
		bool bSuc = pOptn->IsSelected();
		tstring strPhone = msg.pSender->GetParent()->GetParent()->GetParent()->GetName();
		for (int i=0; i< m_vctMembers.size(); i++)
		{
			if (m_vctMembers[i].strPhone == strPhone)
			{
				m_vctMembers[i].bengage = bSuc;
				break;
			}
		}
	}
	else if (_tcsicmp(msg.pSender->GetName(), _T("Option_execute")) == 0)
	{
		COptionUI *pOptn = static_cast<COptionUI*>(msg.pSender);
		bool bSuc = pOptn->IsSelected();
		tstring strPhone = msg.pSender->GetParent()->GetParent()->GetParent()->GetName();
		for (int i=0; i< m_vctMembers.size(); i++)
		{
			if (m_vctMembers[i].strPhone == strPhone)
			{
				m_vctMembers[i].bexecute = bSuc;
				break;
			}
		}
	}
	else if (_tcsicmp(msg.pSender->GetName(), _T("Option_aftersale")) == 0)
	{
		COptionUI *pOptn = static_cast<COptionUI*>(msg.pSender);
		bool bSuc = pOptn->IsSelected();
		tstring strPhone = msg.pSender->GetParent()->GetParent()->GetParent()->GetName();
		for (int i=0; i< m_vctMembers.size(); i++)
		{
			if (m_vctMembers[i].strPhone == strPhone)
			{
				m_vctMembers[i].baftersale = bSuc;
				break;
			}
		}
	}
}

bool CFrameEmployeeManage::getDuty( OrgMemberInfo &oneInfo )   
{
	CHttpClient httpClient;
	tstring strUrl(WEB_SERVER_BASE_URL);
	strUrl += _T("org/get-duty?");
	tstring strPost = _T("");
	strPost += _T("oid=");
	strPost += m_strOrgId;
	strPost += _T("&cell_phone=");
	strPost += oneInfo.strPhone;
	strUrl += strPost;
	tstring strToken = CController::Instance()->GetToken();
	tstring strHttpResponse;
	int retCode = httpClient.Get(strUrl, strToken, strHttpResponse);
	if (strHttpResponse.find("\"result\":0") != -1)
	{
		return false;
	}
	else
	{
		tstring strDuty = strHttpResponse.substr(strHttpResponse.find("\"duty\":\"") + 8, strHttpResponse.length());
		strDuty = strDuty.substr(0, strDuty.find("\",\""));
		if (strDuty.find("1") >= 0)
			oneInfo.brequirement = true;
		else
			oneInfo.brequirement = false;
		if (strDuty.find("2") >= 0)
			oneInfo.bengage = true;
		else
			oneInfo.bengage = false;
		if (strDuty.find("3") >= 0)
			oneInfo.bexecute = true;
		else
			oneInfo.bexecute = false;
		if (strDuty.find("4") >= 0)
			oneInfo.baftersale = true;
		else
			oneInfo.baftersale = false;
		return true;
	}
}

bool CFrameEmployeeManage::getRecipient()                 //todo
{
	CHttpClient httpClient;
	tstring strUrl(WEB_SERVER_BASE_URL);
	strUrl += _T("org/get-default?");
	tstring strPost = _T("");
	strPost += _T("oid=");
	strPost += m_strOrgId;
	strUrl += strPost;
	tstring strToken = CController::Instance()->GetToken();
	tstring strHttpResponse;
	int retCode = httpClient.Get(strUrl, strToken, strHttpResponse);
	if (strHttpResponse.find("\"result\":0") != -1)
	{
		return false;
	}
	else
	{

		return true;
	}
}

bool CFrameEmployeeManage::setRecipient( tstring strPhone )
{
	CHttpClient httpClient;
	tstring strUrl(WEB_SERVER_BASE_URL);
	strUrl += _T("org/set-default?");
	tstring strPost = _T("");
	strPost += _T("oid=");
	strPost += m_strOrgId;
	strPost += _T("&cell_phone=");
	strPost += strPhone;
	strUrl += strPost;
	tstring strToken = CController::Instance()->GetToken();
	tstring strHttpResponse;
	int retCode = httpClient.Get(strUrl, strToken, strHttpResponse);
	if (strHttpResponse.find("\"result\":1") == -1)
	{
		return false;
	}
	else
	{
		return true;
	}
}

bool CFrameEmployeeManage::setDuty( OrgMemberInfo &oneInfo )
{
	CHttpClient httpClient;
	tstring strUrl(WEB_SERVER_BASE_URL);
	strUrl += _T("org/set-default?");
	tstring strPost = _T("");
	strPost += _T("oid=");
	strPost += m_strOrgId;
	strPost += _T("&cell_phone=");
	strPost += oneInfo.strPhone;
	strPost += _T("&duty=");
	tstring strDuty("");
	if (oneInfo.brequirement)
		strDuty += _T("1");
	if (oneInfo.bengage)
		strDuty += _T(",2");
	if (oneInfo.bexecute)
		strDuty += _T(",3");
	if (oneInfo.baftersale)
		strDuty += _T(",4");
	strPost += strDuty;
	strUrl += strPost;
	tstring strToken = CController::Instance()->GetToken();
	tstring strHttpResponse;
	int retCode = httpClient.Get(strUrl, strToken, strHttpResponse);
	if (strHttpResponse.find("\"result\":1") == -1)
	{
		return false;
	}
	else
	{
		return true;
	}
	return true;
}

void CFrameEmployeeManage::onbtnOk()
{
	//1. 保存默认接收人
	setRecipient(m_strRecipient);
	//2. 保存员工任务
	for (int i=0; i<m_vctMembers.size(); i++)
	{
		setDuty(m_vctMembers[i]);
	}
}

