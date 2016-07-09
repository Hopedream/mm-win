#include "PartnerInfoPage.h"
#include "mainDlg.h"
#include "Utility.h"
#include "MsgBox.h"
#include "videoDlg.h"
#include <sstream>
#include "CDialogBuilderCallbackEx.h"
#include "..\Include\Global_Def.h"
#include "Controller.h"
#include "UserManager.h"
#include "webClient.h"
#include "ProjectListView.h"
#include "FileHelper.h"
#include "mainDlg.h"

using namespace std;

const TCHAR* bpp_btn_avatar			= "LabelBussinessPartnerAvatar";
const TCHAR* bpp_label_nick			= "parnter_nick";
const TCHAR* bpp_label_beizhu		= "label_parnter_beizhu";
const TCHAR* bpp_label_company		= "label_partner_company";
const TCHAR* bpp_label_hangye		= "label_hangye";
const TCHAR* bpp_label_phone		= "label_phonenumber";
const TCHAR* bpp_company_phone		= "label_comp_phone";
const TCHAR* bpp_address			= "label_address";

const TCHAR* bpp_modify_beizhu		= "btn_modify_beizhu";
const TCHAR* bpp_btn_send_msg		= "btn_send_message";
const TCHAR* bpp_netcall_free		= "btn_netcall_free";
const TCHAR* bpp_cellphone_call		= "btn_callbyphone";
const TCHAR* bpp_check_projectLists = "btn_check_projectlist";
const TCHAR* bpp_add_orgFriend = "btn_add_orgfriend";
const TCHAR* bpp_del_orgFriend = "btn_del_orgfriend";


CPartnerInfoPage::CPartnerInfoPage(DuiLib::CPaintManagerUI* ppm)
{
	m_PaintManager = ppm;
	m_strCurAccount="";
	m_pLabelBeizhu = NULL;
	//m_strMyOrgID = 
	CDialogBuilder DlgBuilder1;
	CDialogBuilderCallbackEx cb1(m_PaintManager);
	CTabLayoutUI*pLayout = static_cast<CTabLayoutUI*>(m_PaintManager->FindControl("BussinessPartnerTabLayout"));
	if (NULL == pLayout)
	{
		return;
	}

	//1.�����̼���Ϣ��pageҳ��
	CHorizontalLayoutUI* pBussinessPartnerInfoPage = NULL;
	if (!DlgBuilder1.GetMarkup()->IsValid())
	{
		pBussinessPartnerInfoPage = static_cast<CHorizontalLayoutUI*>(DlgBuilder1.Create(_T("bussiness_partner_page.xml"),
			(UINT)0, &cb1, m_PaintManager));
	}
	else
	{
		pBussinessPartnerInfoPage = static_cast<CHorizontalLayoutUI*>(DlgBuilder1.Create((UINT)0, m_PaintManager));
	}

	if (NULL != pBussinessPartnerInfoPage)
	{
		pLayout->AddAt(pBussinessPartnerInfoPage, 0);
	}

	

	//2. ������ͨ������Ϣ��pageҳ��	
	CHorizontalLayoutUI* pFriendInfoPage = NULL;
	CDialogBuilder DlgBuilder2;
	if (!DlgBuilder2.GetMarkup()->IsValid())
	{
		pFriendInfoPage = static_cast<CHorizontalLayoutUI*>(DlgBuilder2.Create(_T("friend_info.xml"),
							(UINT)0, &cb1, m_PaintManager));
	}
	else
	{
		pFriendInfoPage = static_cast<CHorizontalLayoutUI*>(DlgBuilder2.Create((UINT)0, m_PaintManager));
	}
	
	if (NULL != pFriendInfoPage)
	{
		pLayout->AddAt(pFriendInfoPage, 1);
	}
	
	//3.   ����һ���հ�ҳ

	CHorizontalLayoutUI* porginfo = NULL;
	CDialogBuilder DlgBuilder3;
	if (!DlgBuilder3.GetMarkup()->IsValid())
	{
		porginfo = static_cast<CHorizontalLayoutUI*>(DlgBuilder3.Create(_T("tab_org_info_page.xml"),
			(UINT)0, &cb1, m_PaintManager));
	}
	else
	{
		porginfo = static_cast<CHorizontalLayoutUI*>(DlgBuilder3.Create((UINT)0, m_PaintManager));
	}

	if (NULL != porginfo)
	{
		pLayout->AddAt(porginfo, 2);
	}

	m_PaintManager->AddNotifier(this);
}

CPartnerInfoPage::~CPartnerInfoPage()
{
	m_PaintManager->RemoveNotifier(this);
}

void CPartnerInfoPage::UpdataMemo(tstring strNewMemo)
{
	if (NULL != m_pLabelBeizhu)
	{
		if (strNewMemo.empty())
		{
			m_pLabelBeizhu->SetText("��");
		}
		else
		{			
			m_pLabelBeizhu->SetText(strNewMemo.c_str());
		}
	}
}

//��ȡ�û���Ϣ������ʼ��������
bool CPartnerInfoPage::Initialize(tstring strAccount, sUserVCard& pUserInfo, tstring strOrg , tstring strOid)
{
	CTabLayoutUI*pLayout = static_cast<CTabLayoutUI*>(m_PaintManager->FindControl("BussinessPartnerTabLayout"));
	CHorizontalLayoutUI* pParentPane;
	if (NULL == pLayout)
	{
		return false;
	}

	//��ʾ��OrgItem
	if (strOrg.empty() && strOid.empty() && !strAccount.empty())
	{
		m_strCurOrgID = strAccount;
		pLayout->SelectItem(2);
		initOrgInfo(m_strCurOrgID);
		return true;
	}
	else if (strOid == "-1")
	{
		pLayout->SelectItem(1);
		pParentPane = static_cast<CHorizontalLayoutUI*>(pLayout->GetItemAt(1));
	}
	else 
	{
		pLayout->SelectItem(0);
		pParentPane = static_cast<CHorizontalLayoutUI*>(pLayout->GetItemAt(0));

		CLabelUI* send_msg = static_cast<CLabelUI*>(m_PaintManager->FindSubControlByName(pParentPane, bpp_btn_send_msg));
		CLabelUI* free_net_call = static_cast<CLabelUI*>(m_PaintManager->FindSubControlByName(pParentPane, bpp_netcall_free));
		CLabelUI* call_by_phone = static_cast<CLabelUI*>(m_PaintManager->FindSubControlByName(pParentPane, bpp_cellphone_call));
		//���غ�����Ϣҳ��ʱҪ�ж��ǲ��Ǳ��ˣ��Ǳ������������а�ť
		if (NULL != send_msg)
		{			
			send_msg->SetVisible((mainDlg::getInstance()->getMyUserInfo().strAccount == strAccount)?false:true);
		}
		if (NULL != free_net_call)
		{
			free_net_call->SetVisible((mainDlg::getInstance()->getMyUserInfo().strAccount == strAccount)?false:true);
		}
		if (NULL != call_by_phone)
		{
			call_by_phone->SetVisible((mainDlg::getInstance()->getMyUserInfo().strAccount == strAccount)?false:true);
		}
	}

	CHorizontalLayoutUI* pHLUI = static_cast<CHorizontalLayoutUI*>(m_PaintManager->FindSubControlByName(pParentPane,"HLayoutPartnerPage"));
	if (NULL != pHLUI) pHLUI->SetVisible(true);

	//������Լ����������޸ı�ע
	CButtonUI* pBtnModifyBeizu = static_cast<CButtonUI*>(m_PaintManager->FindSubControlByName(pParentPane, bpp_modify_beizhu));
	if (NULL != pBtnModifyBeizu) pBtnModifyBeizu->SetVisible(!CController::Instance()->GetUserInfoMgr()->IsMySelf(strAccount));
	
	orgInfo org;
	CWebClient::GetOrgByPhone(strAccount, org, strOid);

	m_strCurAccount = strAccount;
	CButtonUI* pBtnAvatar = static_cast<CButtonUI*>(m_PaintManager->FindSubControlByName(pParentPane, bpp_btn_avatar));
	if (NULL != pBtnAvatar)
	{	
		bool bExist = PathFileExists(pUserInfo.strPhotoPath.c_str());
		if (!bExist || pUserInfo.strPhotoPath.empty())
		{
			pBtnAvatar->SetBkImage(DEF_AVATAR_NOR);
		}
		else
		{
			pBtnAvatar->SetBkImage(pUserInfo.strPhotoPath.c_str());
		}
	}

	int index = pUserInfo.strAccount.find('@');
	tstring strPhone = pUserInfo.strAccount.substr(0,index);


	CLabelUI* pLabelNickname = static_cast<CLabelUI*>(m_PaintManager->FindSubControlByName(pParentPane, bpp_label_nick));
	if (NULL != pLabelNickname)
	{
		if (pUserInfo.strUserNickname.empty())
		{
			pLabelNickname->SetText(strPhone.c_str());
		}
		else
		{
			pLabelNickname->SetText(pUserInfo.strUserNickname.c_str());
		}

	}

	m_pLabelBeizhu = static_cast<CLabelUI*>(m_PaintManager->FindSubControlByName(pParentPane, bpp_label_beizhu));
	if (NULL != m_pLabelBeizhu)
	{
		if (pUserInfo.strRemark.empty())
		{
			m_pLabelBeizhu->SetText("��");
		}
		else
		{
			m_pLabelBeizhu->SetText(pUserInfo.strRemark.c_str());
		}
	}

	CLabelUI* pLabelCompany = static_cast<CLabelUI*>(m_PaintManager->FindSubControlByName(pParentPane, bpp_label_company));
	if (NULL != pLabelCompany)
	{
		pLabelCompany->SetText(org.strName.c_str());
	}

	CLabelUI* pHangYe = static_cast<CLabelUI*>(m_PaintManager->FindSubControlByName(pParentPane, bpp_label_hangye));
	if (NULL != pHangYe)
	{
		pHangYe->SetText("�������");
	}

	CLabelUI* pPhone= static_cast<CLabelUI*>(m_PaintManager->FindSubControlByName(pParentPane, bpp_label_phone));
	if (NULL != pPhone)
	{
		pPhone->SetText(strPhone.c_str());
	}

	CLabelUI* pCompanyPhone = static_cast<CLabelUI*>(m_PaintManager->FindSubControlByName(pParentPane, bpp_company_phone));
	if (NULL != pCompanyPhone)
	{
		pCompanyPhone->SetText(org.strTelephone.c_str());
	}

	CLabelUI* pAddress = static_cast<CLabelUI*>(m_PaintManager->FindSubControlByName(pParentPane, bpp_address));
	if (NULL != pAddress)
	{
		pAddress->SetText(org.strDetailAddr.c_str());
	}

	return true;
}

void CPartnerInfoPage::Notify( TNotifyUI& msg )
{
	if (msg.sType != DUI_MSGTYPE_CLICK ||  m_strCurAccount.empty())
	{
		return;
	}

	if (msg.pSender->GetName() == bpp_netcall_free)
	{
		NetCall();			
	}
	else if (msg.pSender->GetName() == bpp_cellphone_call)
	{
		PhoneCall();
	}
	else if (msg.pSender->GetName() == bpp_btn_send_msg)
	{
		SendMsg();
	}	
	else if (msg.pSender->GetName() == bpp_modify_beizhu)
	{
		mainDlg::getInstance()->OnModifyPartnerMemo(m_strCurAccount);
	}	
	//�鿴����������Ŀ�б�
	else if (msg.pSender->GetName() == bpp_check_projectLists)
	{
		BusinessDetail();
	}
	else if (msg.pSender->GetName() == bpp_add_orgFriend)
	{
		addOrgFriend();
	}

}

void CPartnerInfoPage::ShowGroupProjectList()
{
	tstring strMyOrgID = mainDlg::getInstance()->getMyselfItem().m_strOrgID.GetData();

	//������� �ҵĺ��� ���Ҳ鿴�����Լ��Ļ����ǲ�֧�ֵģ��Լ���Ȼ������Լ��Ļ�����ҵ�񰡣���
	if (_tcsicmp(strMyOrgID.c_str(), DEF_ORGID_MYFRIEND) != 0 &&
		_tcsicmp(strMyOrgID.c_str(), m_strCurOrgID.c_str())== 0)
	{
		CMsgBox* pMsgBox = new CMsgBox("����", "�����Լ��Ļ���û��ҵ���������ݣ�", MSGBOX_ICON_WARNING, MSGBOX_IDOK);
		pMsgBox->ShowModal();
		return;
	}

	//�鿴������������Ŀ�б�
	//������ֻ��ŵ�view����ʾ���ˣ������ʾ���ˣ���ֱ����ʾ������
	if (!m_strCurOrgID.empty())
	{
		map<tstring, CProjectListView*>::iterator itFind = m_mapId2Projview.find(m_strCurOrgID);
		if (itFind != m_mapId2Projview.end())
		{
			itFind->second->Initialize(m_strCurOrgID, true);
			itFind->second->ShowWindow(true);
			return;
		}

		//����view����ʾ������cache
		CProjectListView* pListView = new CProjectListView();
		if (NULL != pListView)
		{
			pListView->CreateWnd();
			pListView->Initialize(m_strCurOrgID, true);
			pListView->ShowWindow(true);
		}

		m_mapId2Projview.insert(make_pair(m_strCurOrgID, pListView));
	}
}

void CPartnerInfoPage::InitOrgInfoPage()
{
	return;
	CHorizontalLayoutUI* pHLUI = static_cast<CHorizontalLayoutUI*>(m_PaintManager->FindControl("HLayoutPartnerPage"));
	if (NULL != pHLUI) pHLUI->SetVisible(false);
}

bool CPartnerInfoPage::updateView( sUserVCard& pUserInfo )
{
	if (pUserInfo.strAccount != m_strCurAccount)
		return false;
	CTabLayoutUI*pLayout = static_cast<CTabLayoutUI*>(m_PaintManager->FindControl("BussinessPartnerTabLayout"));
	CHorizontalLayoutUI* pParentPane;
	if (NULL == pLayout)
	{
		return false;
	}
	int iIndex = pLayout->GetCurSel();
	pParentPane = static_cast<CHorizontalLayoutUI*>(pLayout->GetItemAt(iIndex));

	//orgInfo org;
	//CWebClient::GetOrgByPhone(strAccount, org, strOid);

	//m_strCurAccount = strAccount;
	CButtonUI* pBtnAvatar = static_cast<CButtonUI*>(m_PaintManager->FindSubControlByName(pParentPane, bpp_btn_avatar));
	if (NULL != pBtnAvatar)
	{	
		bool bExist = PathFileExists(pUserInfo.strPhotoPath.c_str());
		if (!bExist || pUserInfo.strPhotoPath.empty())
		{
			pBtnAvatar->SetBkImage(DEF_AVATAR_NOR);
		}
		else
		{
			pBtnAvatar->SetBkImage(pUserInfo.strPhotoPath.c_str());
		}
	}

	int index = pUserInfo.strAccount.find('@');
	tstring strPhone = pUserInfo.strAccount.substr(0,index);


	CLabelUI* pLabelNickname = static_cast<CLabelUI*>(m_PaintManager->FindSubControlByName(pParentPane, bpp_label_nick));
	if (NULL != pLabelNickname)
	{
		if (pUserInfo.strUserNickname.empty())
		{
			pLabelNickname->SetText(strPhone.c_str());
		}
		else
		{
			pLabelNickname->SetText(pUserInfo.strUserNickname.c_str());
		}

	}

	m_pLabelBeizhu = static_cast<CLabelUI*>(m_PaintManager->FindSubControlByName(pParentPane, bpp_label_beizhu));
	if (NULL != m_pLabelBeizhu)
	{
		if (pUserInfo.strRemark.empty())
		{
			m_pLabelBeizhu->SetText("��");
		}
		else
		{
			m_pLabelBeizhu->SetText(pUserInfo.strRemark.c_str());
		}
	}

	//CLabelUI* pLabelCompany = static_cast<CLabelUI*>(m_PaintManager->FindSubControlByName(pParentPane, bpp_label_company));
	//if (NULL != pLabelCompany)
	//{
	//	pLabelCompany->SetText(org.strName.c_str());
	//}

	//CLabelUI* pHangYe = static_cast<CLabelUI*>(m_PaintManager->FindSubControlByName(pParentPane, bpp_label_hangye));
	//if (NULL != pHangYe)
	//{
	//	pHangYe->SetText("�������");
	//}

	//CLabelUI* pPhone= static_cast<CLabelUI*>(m_PaintManager->FindSubControlByName(pParentPane, bpp_label_phone));
	//if (NULL != pPhone)
	//{
	//	pPhone->SetText(strPhone.c_str());
	//}

	//CLabelUI* pCompanyPhone = static_cast<CLabelUI*>(m_PaintManager->FindSubControlByName(pParentPane, bpp_company_phone));
	//if (NULL != pCompanyPhone)
	//{
	//	pCompanyPhone->SetText(org.strTelephone.c_str());
	//}

	//CLabelUI* pAddress = static_cast<CLabelUI*>(m_PaintManager->FindSubControlByName(pParentPane, bpp_address));
	//if (NULL != pAddress)
	//{
	//	pAddress->SetText(org.strDetailAddr.c_str());
	//}

	return true;
}

void CPartnerInfoPage::initOrgInfo(tstring& strOrgId)
{
	m_strMyOrgID = mainDlg::getInstance()->getMyselfItem().m_strOrgID.GetData();
	CTabLayoutUI*pLayout = static_cast<CTabLayoutUI*>(m_PaintManager->FindControl("BussinessPartnerTabLayout"));
	CHorizontalLayoutUI* pParentPane;
	if (NULL == pLayout)
	{
		return;
	}
	pParentPane = static_cast<CHorizontalLayoutUI*>(pLayout->GetItemAt(2));
	if (pParentPane == NULL)
		return;

	orgInfo org;
	CWebClient::GetOrgById(strOrgId, org);
	tstring strOrgAva;
	if (CWebClient::getOrgAvatar(strOrgId, strOrgAva))
	{
		CLabelUI *plabelAva = static_cast<CLabelUI*>(m_PaintManager->FindSubControlByName(pParentPane, _T("LabelBussinessPartnerAvatar")));
		if (plabelAva != NULL)
			plabelAva->SetBkImage(strOrgAva.c_str());
	}
	else
	{
		CLabelUI *plabelAva = static_cast<CLabelUI*>(m_PaintManager->FindSubControlByName(pParentPane, _T("LabelBussinessPartnerAvatar")));
		if (plabelAva != NULL)
			plabelAva->SetBkImage("bussiness_partner_default.PNG");
	}
	//todo ����ͷ��

	CLabelUI *pLabelPhone = static_cast<CLabelUI*>(m_PaintManager->FindSubControlByName(pParentPane, _T("label_phonenumber")));
	if (pLabelPhone != NULL)
	{
		pLabelPhone->SetText(org.strCellPhone.c_str());
	}

	CLabelUI *pLabelTel = static_cast<CLabelUI*>(m_PaintManager->FindSubControlByName(pParentPane, _T("label_comp_phone")));
	if (pLabelTel != NULL)
		pLabelTel->SetText(org.strTelephone.c_str());
	CLabelUI *pLabelAdd = static_cast<CLabelUI*>(m_PaintManager->FindSubControlByName(pParentPane, _T("label_address")));
	if (pLabelAdd != NULL)
		pLabelAdd->SetText(org.strDetailAddr.c_str());

	CLabelUI *pLabelName = static_cast<CLabelUI*>(m_PaintManager->FindSubControlByName(pParentPane, _T("parnter_nick")));
	if (pLabelName != NULL)
		pLabelName->SetText(org.strName.c_str());

	CButtonUI* pBtnCheckPorjs = static_cast<CButtonUI*>(m_PaintManager->FindSubControlByName(pParentPane, _T("btn_check_org_projectlist")));
	CButtonUI* pBtnAddOrg = static_cast<CButtonUI*>(m_PaintManager->FindSubControlByName(pParentPane, _T("btn_add_orgfriend")));
	if (pBtnAddOrg == NULL)
		pBtnAddOrg = static_cast<CButtonUI*>(m_PaintManager->FindSubControlByName(pParentPane, _T("btn_del_orgfriend")));
	if (NULL == pBtnCheckPorjs || NULL == pBtnAddOrg)
	{
		return;
	}

	//�������ҵĺ��ѣ������ص� ���鿴ҵ����������ť
	if (_tcsicmp(strOrgId.c_str(), DEF_ORGID_MYFRIEND) == 0 || m_strMyOrgID == strOrgId)  //�����Ǳ�����
	{
		pBtnCheckPorjs->SetVisible(false);
		pBtnAddOrg->SetVisible(false);
	}
	else
	{
		pBtnCheckPorjs->SetVisible(true);
		bool bFriend = CWebClient::isOrgBuddy(m_strMyOrgID, strOrgId);
		if (!bFriend)
		{
			pBtnAddOrg->SetVisible(true);
			pBtnAddOrg->SetName("btn_add_orgfriend");
			pBtnAddOrg->SetText("��ӻ�������");
			pBtnAddOrg->SetTag(atoi(strOrgId.c_str()));
		}
		else
		{
			pBtnAddOrg->SetVisible(true);
			pBtnAddOrg->SetName("btn_del_orgfriend");
			pBtnAddOrg->SetText("�����������");
			pBtnAddOrg->SetTag(atoi(strOrgId.c_str()));
		}
	}
}

void CPartnerInfoPage::NetCall()
{
	CVideoDlg::Instance()->OnLauchNetCall(m_strCurAccount.c_str(), WM_CALL_IP_PHONE);
}

void CPartnerInfoPage::PhoneCall() 
{
	CVideoDlg::Instance()->OnLauchNetCall(m_strCurAccount.c_str(), WM_CALL_SMART_PHONE);
}

void CPartnerInfoPage::SendMsg()
{
	mainDlg::getInstance()->OnShowChatWnd(m_strCurAccount);	
}

void CPartnerInfoPage::BusinessDetail()
{
	//������ֻ��ŵ�view����ʾ���ˣ������ʾ���ˣ���ֱ����ʾ������
	tstring strFriendPhone = CFileHelper::GetUserName(m_strCurAccount);

	map<tstring, CProjectListView*>::iterator itFind = m_mapId2Projview.find(strFriendPhone);
	if (itFind != m_mapId2Projview.end())
	{
		itFind->second->Initialize(strFriendPhone, false);
		itFind->second->ShowWindow(true);
		return;
	}

	//����view����ʾ������cache
	CProjectListView* pListView = new CProjectListView();
	if (NULL != pListView)
	{
		pListView->CreateWnd();
		pListView->Initialize(strFriendPhone, false);
		pListView->ShowWindow(true);
	}

	m_mapId2Projview.insert(make_pair(strFriendPhone, pListView));
}

void CPartnerInfoPage::addOrgFriend()
{
	if (m_strCurOrgID.empty())
	{
		CMsgBox* pMsgBox = new CMsgBox("����","����idΪ�գ����飡", MSGBOX_ICON_ERROR);
		pMsgBox->ShowModal();
	}
	else
	{
		if(CWebClient::addOrgFriend(m_strCurOrgID))
		{
			CMsgBox* pMsgBox = new CMsgBox("��ӻ�������","���ѳɹ���������", MSGBOX_ICON_NORMAL);
			pMsgBox->ShowModal();
		}
		else
		{
			CMsgBox* pMsgBox = new CMsgBox("����","��ӻ�������ʧ�ܣ�", MSGBOX_ICON_ERROR);
			pMsgBox->ShowModal();
		}
	}
	
}

void CPartnerInfoPage::delOrgFriend( int iOrgid )
{
	stringstream ss;
	tstring strOrg;
	ss << iOrgid;
	ss >> strOrg;
	CWebClient::delOrgFriend(strOrg);
}

bool CPartnerInfoPage::isOrgBuddy( tstring strOrgid, tstring strOrgidDest )
{
	return CWebClient::isOrgBuddy(strOrgid, strOrgidDest);
}
