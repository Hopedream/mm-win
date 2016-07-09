#include "FrameEmployeeManage1.h"
#include "mainDlg.h"
#include "UserManager.h"
#include "Controller.h"
#include "MsgBox.h"
#include "StringHelper.h"
#include "../Common/regexHelper.h"
#include "..\json\json.h"

CFrameEmployeeManage1* CFrameEmployeeManage1::m_pInstance = NULL;

CFrameEmployeeManage1::CFrameEmployeeManage1()
{
	CController::Instance()->GetUserInfoMgr()->AddEventHandler(this);
}


CFrameEmployeeManage1::~CFrameEmployeeManage1(void)
{
}

void CFrameEmployeeManage1::HandleSetFocusEvent( TNotifyUI& msg )
{

}

void CFrameEmployeeManage1::HandleKillFocusEvent( TNotifyUI& msg )
{

}

void CFrameEmployeeManage1::HandleTextChangedEvent( TNotifyUI& msg )
{

}

void CFrameEmployeeManage1::HandleClickEvent( TNotifyUI& msg )
{
	if (_tcsicmp(msg.pSender->GetName(), _T("buttonOK"))== 0)
	{
		if (onbtnOk())
			ShowWindow(false);
	}
	else if (_tcsicmp(msg.pSender->GetName(), _T("buttonAdd")) == 0)
	{
		tstring strPhone = m_pEditPhone->GetText();
		AddMember(strPhone);
	}
	else if (_tcsicmp(msg.pSender->GetName(), _T("buttonClose")) == 0)
	{
		/*CMsgBox* pMsgBox = new CMsgBox("老板权限转让", "是否确定转让权限？",MSGBOX_ICON_ASK, MSGBOX_IDYESNO);
		if (MSGBOX_IDNO == pMsgBox->ShowModal(NULL))
		{			
			return;
		}		*/
		ShowWindow(false);
		updateWindow();
	}
	else if (_tcsicmp(msg.pSender->GetName(), _T("buttonCancel")) == 0)
	{
		ShowWindow(false);
		updateWindow();
	}
	else if (_tcsicmp(msg.pSender->GetName(), _T("buttonremove")) == 0)
	{
		CMsgBox* pMsgBox = new CMsgBox("员工管理", "是否确定解除关系？",MSGBOX_ICON_ASK, MSGBOX_IDYESNO);
		if (MSGBOX_IDNO == pMsgBox->ShowModal(NULL))
		{			
			return;
		}		
		tstring strPhone = msg.pSender->GetParent()->GetParent()->GetParent()->GetName();
		MemRemove(strPhone);
	}
	else if (_tcsicmp(msg.pSender->GetName(), _T("buttonTransfer")) == 0)
	{
		tstring strPhone = msg.pSender->GetParent()->GetParent()->GetParent()->GetName();
		CMsgBox* pMsgBox = new CMsgBox("老板权限转让", "是否确定转让权限？",MSGBOX_ICON_ASK, MSGBOX_IDYESNO);
		if (MSGBOX_IDNO == pMsgBox->ShowModal(NULL))
		{			
			return;
		}		
		if (!TransferOwnerPermissions(strPhone))
		{
			CMsgBox* pMsgBox = new CMsgBox("老板权限转让", "转让老板权限失败",  MSGBOX_ICON_WARNING);
			pMsgBox->ShowModal(this->GetHWND());
			return;
		}
		else 
		{
			CMsgBox* pMsgBox = new CMsgBox("老板权限转让", "转让老板权限成功",  MSGBOX_ICON_WARNING);
			pMsgBox->ShowModal(this->GetHWND());
			ShowWindow(false);
		}
	}
}

void CFrameEmployeeManage1::HandleSelectedChangedEvent( TNotifyUI& msg )
{
	if (_tcsicmp(msg.pSender->GetName(), _T("option_recipient")) == 0)
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
				return;
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

void CFrameEmployeeManage1::HandleItemSelectEvent( TNotifyUI& msg )
{

}

DuiLib::CDuiString CFrameEmployeeManage1::GetSkinFile()
{
	return _T("sys_employee_manage.xml");
}

CControlUI* CFrameEmployeeManage1::CreateControl( LPCTSTR pstrClass )
{
	return NULL;
}

void CFrameEmployeeManage1::InitWindow()
{
	m_plstEmployee = static_cast<CListUI*>(m_PaintManager.FindControl(_T("lstEmployee")));
	m_pEditPhone = static_cast<CEditUI *>(m_PaintManager.FindControl(_T("editPhone")));
	if (m_plstEmployee == NULL || m_pEditPhone == NULL)
		return;
	if (!getOrgInfo())
		return;
	m_vctMembers.clear();
	getOrgMembers(m_strOid, m_vctMembers);
	if (m_vctMembers.size() > 0)
	{
		//1. 获取默认接收人
		
		for (int i=0; i<m_vctMembers.size(); i++)
		{
			//3. 获取个人vcard
			CController::Instance()->GetUserInfoMgr()->FetchVCardByAccount(m_vctMembers[i].strPhone);
			//4. 加到ui
			AddItem(m_vctMembers[i]);
		}
	}
}

CFrameEmployeeManage1* CFrameEmployeeManage1::Instance()
{
	if(NULL == m_pInstance)
	{
		m_pInstance = new CFrameEmployeeManage1;
		m_pInstance->CreateWnd(mainDlg::getInstance()->GetHWND());
	}
	return m_pInstance;
}

bool CFrameEmployeeManage1::getOrgInfo()
{
	CHttpClient httpClient;
	tstring strUrl(WEB_SERVER_BASE_URL);
	strUrl += _T("org/get-org");
	tstring strPost = (_T(""));
	tstring strToken = CController::Instance()->GetToken();
	tstring strHttpResponse;
	int retCode = httpClient.Post(strUrl, strPost, strToken, strHttpResponse);
	if (strHttpResponse.find("\"result\":0") != -1)
	{
		return false;
	}
	else
	{
		tstring strId = strHttpResponse.substr(strHttpResponse.find("\"id\":\"") + 6, strHttpResponse.length());
		m_strOid = strId.substr(0, strId.find("\",\""));
		return true;
	}
	
}

bool CFrameEmployeeManage1::getOrgMembers( tstring strOrgId, vector<OrgMemberInfo>& vctMembers )
{	
	m_strMySelfPhone = mainDlg::getInstance()->getMyUserInfo().strPhone;
	CHttpClient httpClient;
	tstring strUrl(WEB_SERVER_BASE_URL);
	strUrl += _T("org/get-org-members?");
	tstring strPost = _T("oid=") + m_strOid;
	strUrl += strPost;
	tstring strToken = CController::Instance()->GetToken();
	tstring strHttpResponse;
	int retCode = httpClient.Get(strUrl, strToken, strHttpResponse);

	int find = strHttpResponse.find("members");
	bool res_empty = strHttpResponse.empty();

	if (!strHttpResponse.empty() && strHttpResponse.find("members") != -1)
	{
		Json::Reader reader;
		Json::Value root;
		if (reader.parse(strHttpResponse, root))
		{
			int nSize = root["members"].size();
			for (int i=0; i<nSize; i++)
			{


				OrgMemberInfo oneInfo;
				oneInfo.strPhone = root["members"][i]["cell_phone"].asString();
				if (root["members"][i]["is_default"].asInt() == 1)
					m_strRecipient = oneInfo.strPhone;

				tstring strDuty = root["members"][i]["duty"].asString();
				if (strDuty.find("null") != -1)
				{
					oneInfo.brequirement = false;
					oneInfo.bengage = false;
					oneInfo.bexecute = false;
					oneInfo.baftersale = false;
				}
				else
				{
					int nIndex = strDuty.find("1");
					if (nIndex >= 0)
						oneInfo.brequirement = true;
					else
						oneInfo.brequirement = false;

					nIndex = strDuty.find("2");
					if (nIndex >= 0)
						oneInfo.bengage = true;
					else
						oneInfo.bengage = false;

					nIndex = strDuty.find("3");
					if (nIndex >= 0)
						oneInfo.bexecute = true;
					else
						oneInfo.bexecute = false;

					nIndex = strDuty.find("4");
					if (nIndex >= 0)
						oneInfo.baftersale = true;
					else
						oneInfo.baftersale = false;
				}
				//将业务属性全部屏蔽，这里都设置为false
				oneInfo.brequirement = false;
				oneInfo.bengage = false;
				oneInfo.bexecute = false;
				oneInfo.baftersale = false;

				if (oneInfo.strPhone != m_strMySelfPhone)
					m_vctMembers.push_back(oneInfo);
				
			}
		}
	}
	else
	{
		return false;
	}
	return true;
}

bool CFrameEmployeeManage1::getRecipient()
{
	CHttpClient httpClient;
	tstring strUrl(WEB_SERVER_BASE_URL);
	strUrl += _T("org/get-default?");
	tstring strPost = (_T(""));
	strPost += _T("oid=");
	strPost += m_strOid;
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
		//todo
		m_strRecipient = strHttpResponse.substr(strHttpResponse.find("\"cell_phone\":\"") + 14, strHttpResponse.find("\",\"") - (strHttpResponse.find("\"cell_phone\":\"") + 14));
		return true;
	}
}

bool CFrameEmployeeManage1::getDuty( OrgMemberInfo &oneInfo )
{
	CHttpClient httpClient;
	tstring strUrl(WEB_SERVER_BASE_URL);
	strUrl += _T("org/get-duty?");
	tstring strPost = (_T(""));
	strPost += _T("oid=");
	strPost += m_strOid;
	strPost += _T("&cell_phone=");
	strPost += oneInfo.strPhone;
	strUrl += strPost;
	tstring strToken = CController::Instance()->GetToken();
	tstring strHttpResponse;
	int retCode = httpClient.Get(strUrl, strToken, strHttpResponse);  //todo jiekou wenti
	if (strHttpResponse.find("\"result\":0") != -1)
	{
		oneInfo.brequirement = false;
		oneInfo.bengage = false;
		oneInfo.bexecute = false;
		oneInfo.baftersale = false;
		return false;
	}
	else
	{
		//todo
		tstring strDuty = strHttpResponse.substr(strHttpResponse.find("\"duty\":") + 7, strHttpResponse.length());
		strDuty = strDuty.substr(0, strDuty.find("\",\""));
		int nIndex = strDuty.find("1");
		if (nIndex >= 0)
			oneInfo.brequirement = true;
		else
			oneInfo.brequirement = false;

		nIndex = strDuty.find("2");
		if (nIndex >= 0)
			oneInfo.bengage = true;
		else
			oneInfo.bengage = false;

		nIndex = strDuty.find("3");
		if (nIndex >= 0)
			oneInfo.bexecute = true;
		else
			oneInfo.bexecute = false;

		nIndex = strDuty.find("4");
		if (nIndex >= 0)
			oneInfo.baftersale = true;
		else
			oneInfo.baftersale = false;
		return true;
	}
}


void CFrameEmployeeManage1::OnVCardReturned( tstring& strAccount, sUserVCard& oneInfo )
{
	for (int i=0; i<m_vctMembers.size(); i++)
	{
		if ((m_vctMembers[i].strPhone) == oneInfo.strPhone)
		{
			m_vctMembers[i].strLogo = oneInfo.strPhotoPath;
			m_vctMembers[i].strNickName = oneInfo.strUserNickname;
			m_vctMembers[i].strAccount = oneInfo.strAccount;
			updateItem(m_vctMembers[i]);
			return;
		}
	}
}

void CFrameEmployeeManage1::OnMySelfCardInfo( sUserVCard& oneInfo )
{
	/*for (int i=0; i<m_vctMembers.size(); i++)
	{
		if ((m_vctMembers[i].strPhone) == oneInfo.strPhone)
		{
			m_vctMembers[i].strLogo = oneInfo.strPhotoPath;
			m_vctMembers[i].strNickName = oneInfo.strUserNickname;
			m_vctMembers[i].strAccount = oneInfo.strAccount;
			updateItem(m_vctMembers[i]);
			return;
		}
	}*/
}

LRESULT CFrameEmployeeManage1::OnMouseWheel( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
    bHandled = FALSE;  
    return 0;  
}

LRESULT CFrameEmployeeManage1::OnMouseMove( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
    bHandled = TRUE;  
    return 0;  
}

bool CFrameEmployeeManage1::AddItem( OrgMemberInfo memInfo )
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
				pOptionrequirement->Selected(true);
			else
				pOptionrequirement->Selected(false);
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

bool CFrameEmployeeManage1::onbtnOk()
{
	if (!setRecipient(m_strRecipient))
	{
		//CMsgBox* pMsgBox = new CMsgBox("设置默认联系人", "设置默认联系人失败",  MSGBOX_ICON_WARNING);
		//pMsgBox->ShowModal(this->GetHWND());
		return false;
	}
	for (int i=0; i<m_vctMembers.size(); i++)
	{
		if (!setDuty(m_vctMembers[i]))
		{

			return false;
		}
	}
	return true;
}

bool CFrameEmployeeManage1::setRecipient( tstring strPhone )
{
	if (strPhone == "")
	{
		CMsgBox* pMsgBox = new CMsgBox("设置", "请选择默认接收人",  MSGBOX_ICON_WARNING);
		pMsgBox->ShowModal(this->GetHWND());
		return false;
	}
	CHttpClient httpClient;
	tstring strUrl(WEB_SERVER_BASE_URL);
	strUrl += _T("org/set-default?");
	tstring strPost = (_T(""));
	strPost += _T("oid=");
	strPost += m_strOid;
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

bool CFrameEmployeeManage1::setDuty( OrgMemberInfo &oneInfo )
{
	vector<tstring> vec_duty;

	CHttpClient httpClient;
	tstring strUrl(WEB_SERVER_BASE_URL);
	strUrl += _T("org/set-duty?");
	tstring strPost = (_T(""));
	strPost += _T("oid=");
	strPost += m_strOid;
	strPost += _T("&cell_phone=");
	strPost += oneInfo.strPhone;
	strPost += _T("&duties=");
	tstring strDuty(_T(""));

//	(a>=0)?vecProperty.push_back("需求"):NULL;
//	(b>=0)?vecProperty.push_back("订约"):NULL;
///	(c>=0)?vecProperty.push_back("执行"):NULL;
//	(d>=0)?vecProperty.push_back("售后"):NULL;

	if (oneInfo.brequirement)             //todo
	{
		strDuty += _T("1,");
		vec_duty.push_back("需求");
	}
	if (oneInfo.bengage)
	{
		strDuty += _T("2,");
		vec_duty.push_back("订约");
	}
	if (oneInfo.bexecute)
	{
		strDuty += _T("3,");
		vec_duty.push_back("执行");
	}
	if (oneInfo.baftersale)
	{
		strDuty += _T("4,");
		vec_duty.push_back("售后");
	}

	strDuty=strDuty.substr(0, strDuty.length() -1);
	if ((!oneInfo.brequirement) && (!oneInfo.bexecute) && (!oneInfo.bengage) && (!oneInfo.baftersale))
		strDuty = "null";
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
		//好友列表的权限需要同步更新
		mainDlg::getInstance()->UpdateDutyInFriendList(oneInfo.strAccount, vec_duty);
		return true;
	}	
}

bool CFrameEmployeeManage1::AddMember( tstring strPhone )
{
	if (!CRegexHelper::IsValidMobileNo(strPhone.c_str()))
	{
		CMsgBox* pMsgBox = new CMsgBox("查找", "输入的手机号码格式错误",  MSGBOX_ICON_WARNING);
		pMsgBox->ShowModal(this->GetHWND());
		return false;
	}
	for (int i=0; i<m_vctMembers.size(); i++)
	{
		if (m_vctMembers[i].strPhone == strPhone)
		{
			CMsgBox* pMsgBox = new CMsgBox("查找", "账号已添加，请不要重复添加",  MSGBOX_ICON_WARNING);
			pMsgBox->ShowModal(this->GetHWND());
			return false;
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
	tstring strPost = (_T(""));
	strPost += _T("oid=");
	tstring strNameDefault = m_strOid;
	strPost += strNameDefault;
	strPost += _T("&cell_phones=[") + strPhone + "]";           //110000,110100,110101");
	strUrl += strPost;
	tstring strToken = CController::Instance()->GetToken();
	tstring strHttpResponse;
	int retCode = httpClient.Get(CChineseCode::EncodeUTF8(strUrl), strToken, strHttpResponse);
	if (strHttpResponse.find("success_phones") == -1 || strHttpResponse.length() < 31)
	{
		if (strHttpResponse.find("exist_phones") != -1)
		{
			CMsgBox* pMsgBox = new CMsgBox("添加", "此账号已有机构, 添加员工失败！",  MSGBOX_ICON_WARNING);
			pMsgBox->ShowModal(this->GetHWND());
		}
		else if (strHttpResponse == "{\"success_phones\":[\"\"]}")
		{
			CMsgBox* pMsgBox = new CMsgBox("添加", "此账号非平台账号, 添加员工失败！",  MSGBOX_ICON_WARNING);
			pMsgBox->ShowModal(this->GetHWND());
		}
		else
		{
			CMsgBox* pMsgBox = new CMsgBox("添加", "添加员工失败！",  MSGBOX_ICON_WARNING);
			pMsgBox->ShowModal(this->GetHWND());
		}
		
		return false;
	}
	else
	{
		//成功
		OrgMemberInfo oneinfo;
		oneinfo.baftersale = false;
		oneinfo.brequirement = false;
		oneinfo.bengage = false;
		oneinfo.bexecute = false;
		oneinfo.strPhone = strPhone;
		m_vctMembers.push_back(oneinfo);
		AddItem(oneinfo);
		CController::Instance()->GetUserInfoMgr()->FetchVCardByAccount(strPhone);
		CMsgBox* pMsgBox = new CMsgBox("添加", "添加员工成功！",  MSGBOX_ICON_WARNING);
		pMsgBox->ShowModal(this->GetHWND());
		return true;
	}	
}

bool CFrameEmployeeManage1::MemRemove( tstring strPhone )
{
	//删除
	//int i=0;
	//调用web端接口
	CHttpClient httpClient;
	tstring strUrl(WEB_SERVER_BASE_URL);
	strUrl += _T("org/del-member?");
	tstring strPost = (_T(""));
	strPost += _T("oid=");
	tstring strNameDefault = m_strOid;
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
		return false;
	}
	else
	{
		//删除成功
		for (vector<OrgMemberInfo>::iterator itr = m_vctMembers.begin(); itr!=m_vctMembers.end(); itr++)
		{
			if (itr->strPhone == strPhone)
			{
				m_vctMembers.erase(itr);
				break;
			}
		}
		CListContainerElementUI *pElm = static_cast<CListContainerElementUI*>(m_PaintManager.FindControl(strPhone.c_str()));
		if (pElm != NULL)
			m_plstEmployee->Remove(pElm);
		return true;
	}	
}

bool CFrameEmployeeManage1::TransferOwnerPermissions( tstring strPhone )
{
	CHttpClient httpClient;
	tstring strUrl(WEB_SERVER_BASE_URL);
	strUrl += _T("org/transfer-admin?");
	tstring strPost = (_T(""));
	strPost += _T("oid=");
	strPost += m_strOid;
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

bool CFrameEmployeeManage1::updateItem( OrgMemberInfo memInfo )
{
	CListContainerElementUI *plstItem = static_cast<CListContainerElementUI*>(m_PaintManager.FindControl(memInfo.strPhone.c_str()));
	if (plstItem == NULL)
		return false;
	else
	{
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
		return true;
	}
}

void CFrameEmployeeManage1::updateWindow()
{
	m_plstEmployee->RemoveAll();

	if (!getRecipient())
		m_strRecipient=(_T(""));
	//2. 获取个人职责
	for (int i=0; i<m_vctMembers.size(); i++)
	{
		getDuty(m_vctMembers[i]);
		//4. 加到ui
		AddItem(m_vctMembers[i]);
	}
}
