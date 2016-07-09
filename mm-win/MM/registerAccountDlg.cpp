#include "registerAccountDlg.h"
#include "HttpClient.h"
#include "json/json.h"
#include "MsgBox.h"
#include "webClient.h"
#include "../Common/regexHelper.h"
#include "../Common/Logger.h"
#include "../include/Global_Def.h"
#include "mainDlg.h"
#include <sstream>
#include "StringHelper.h"
#include "Utility.h"

const TCHAR* const kRegisterNewUserBtnName = _T("btnRegisterNewUser");



#define MOBILE_NUM_LEN 11
#define MIN_PASSWD_LEN 6
#define MAX_PASSWD_LEN 12

#define PIN_CODE_VALID_TIME 120 // ��֤����Чʱ��120sec
#define TIMER_UNIT_SECOND 1000

#define IMG_GET_PIN_BKIMG  _T("btn_default4.png")
#define IMG_GET_PIN_ACTIVE_BKIMG _T("btn_default3.png")
#define IMG_GET_PIN_ACTIVE_HOT_BKIMG _T("btn_press3.png")
#define WARNING_TEXT_COLOR 0xF8661B
#define  FOCUS_BORDER_COLOR 0xFF76c1db
#define  BORDER_COLOR 0x00CCCCCC

#define INPUT_USERNAME_TIPTEXT _T("����������ע���ֻ�����")
#define MOBILE_PHONE_ERROR_TIPTEXT _T("�ֻ����벻��ȷ������������")
#define INPUT_PASSWD_TIPTEXT _T("����Ϊ6-12λ���֡���Сд��ĸ")
#define PASSWD_SHORT_ERROR _T("���볤������Ϊ6λ")
#define PASSWD_LONG_ERROR _T("���볤�����Ϊ12λ")
#define PASSWD_MISMATCH_ERROR  _T("������������벻һ��")
#define GET_PINCODE_TIPTEXT _T("������֤��")
#define INPUT_PINCODE_TIPTEXT _T("��������֤��")
#define PINCODE_ERROR  _T("��֤��������������룡")
#define SEND_PIN_TIP "ϵͳ���������ֻ�������֤�룬��ע����գ�"


#define REG_RESULT_SUCCESS "ע��ɹ���"
#define REG_SUCCESS_DESCRIPTION "���ھͿ�ʼ��������֮�ðɣ�"
#define REG_RESULT_FAILED "ע��ʧ�ܣ�"
#define REG_SUCCESS_ICON "systemMsgBox\\MsgIcon0.png"
#define REG_FAILED_ICON "systemMsgBox\\MsgIcon1.png"
#define REG_BTN_LOGIN "������¼"
#define REG_BTN_RE_SINGUP "����ע��"

#define TIMERID_VERIFY_CODE  1

bool CRegisterAccountDlg::m_bActive = false;
CRegisterAccountDlg::CRegisterAccountDlg(int dlg_type)
	: dlg_type_(dlg_type)
{
}

CRegisterAccountDlg::~CRegisterAccountDlg()
{

}

DuiLib::CDuiString CRegisterAccountDlg::GetSkinFile()
{
	return _T("register\\registerNewUserUISkin.xml");
}

LRESULT CRegisterAccountDlg::HandleCustomMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
	if (WM_TIMER == uMsg && TIMERID_VERIFY_CODE == wParam)
	{
		int nLeftTime = --m_nPinLifecycle;
		if (nLeftTime > 0)
		{
			TCHAR szMsg[100];			
			_stprintf(szMsg,_T("ʣ��%d��"), nLeftTime);
			m_pBtnGetPinCode->SetText(szMsg);
		}
		else
		{
			KillTimer(m_PaintManager.GetPaintWindow(), TIMERID_VERIFY_CODE);
			EnableFetchPinCodeBtn(true);
		}
	}
	//bHandled = TRUE;
	return 0;
}


void CRegisterAccountDlg::EnableFetchPinCodeBtn(bool bEnabled)
{
	m_pBtnGetPinCode->SetEnabled(bEnabled);
	if (bEnabled)
	{			
		m_pBtnGetPinCode->SetText(GET_PINCODE_TIPTEXT);
		m_pBtnGetPinCode->SetNormalImage(IMG_GET_PIN_ACTIVE_BKIMG);
		m_pBtnGetPinCode->SetHotImage(IMG_GET_PIN_ACTIVE_HOT_BKIMG);
		m_pBtnGetPinCode->SetPushedImage(IMG_GET_PIN_ACTIVE_HOT_BKIMG);
	}
	else
	{
		m_pBtnGetPinCode->SetNormalImage(IMG_GET_PIN_BKIMG);
	}
}

void CRegisterAccountDlg::HandleClickEvent( TNotifyUI& msg )
{
	if (0 == msg.pSender->GetName().Compare(kRegisterNewUserBtnName) )
	{
		switch (dlg_type_)
		{
		case REG_PAGE_VIEW:
		case REG_COMPLETE_VIEW:
			RegisterUser();
			break;
		case DLG_TYPE_RESET_PASSWORD:
			ResetPassword();
			break;
		default:
			break;
		}
				
	}
	else if (m_pBtnGetPinCode == msg.pSender)
	{
		if (dlg_type_ == DLG_TYPE_RESET_PASSWORD)
		{
			GetVerifyCodeFromCss();
		}
		else
		{
			GetVerifyCode();
		}
		
	}
	else if (msg.pSender->GetName() == "btnclose")
	{
		m_bActive = false;
		Close();
	}
	else if (msg.pSender == m_pBtnCompleteReg)
	{
		if (0 == m_pLabelRegResult->GetText().Find(REG_RESULT_SUCCESS))
		{
			m_bActive = false;
			Close();
		}
		else
		{
			SetRegTabView(REG_PAGE_VIEW);
		}
	}
	else if (msg.pSender == m_pBtnPasswdMode)
	{
		SwitchPasswdMode();
	}
}


void CRegisterAccountDlg::SetRegTabView(int idx)
{
	CTabLayoutUI* pTabLayout = static_cast<CTabLayoutUI*>(m_PaintManager.FindControl(_T("regSwitchLayout")));
	CLabelUI* pLabelTitle = static_cast<CLabelUI*>(m_PaintManager.FindControl(_T("lableTitle")));
	if (pTabLayout &&  pLabelTitle)
	{
		pTabLayout->SelectItem(idx);
		switch (idx)
		{
		case REG_PAGE_VIEW:
			pLabelTitle->SetText("ע��");
			break;
		case REG_COMPLETE_VIEW:
			pLabelTitle->SetText("ע�����");
			break;
		case DLG_TYPE_RESET_PASSWORD:
			pLabelTitle->SetText("����֧������");
			break;
		default:
			break;
		}
//		pLabelTitle->SetText(REG_PAGE_VIEW == idx ? "ע��" : "ע�����" );
	}	
}

void CRegisterAccountDlg::RegisterUser()
{
	if (!CheckRegisterInfo())
	{
		return;
	}

	tstring strUrl(WEB_SERVER_SITE_URL);
	strUrl += _T("sign-up");

	tstring strPost;
	strPost += _T("cell_phone=");
	strPost += m_pUserNameEditUI->GetText().GetData();
	strPost += _T("&passwd=");
	strPost += m_pPasswdEditUI->GetText().GetData();
	strPost += _T("&verify_code=");
	strPost += m_pPinCodeEditUI->GetText().GetData();
	 

	tstring strHttpResponse;
	CHttpClient httpClient;
	int retCode = httpClient.Post(strUrl,strPost, strHttpResponse);
	//if (0 != retCode)
	//{
	//	//tstring strErr = httpClient.GetStrError(retCode);
	//	MessageBox(NULL, _T("ע���û�ʧ��"), "ע��", MB_OK);
	//	return;
	//}

	if (strHttpResponse.find("result") != -1)
	{
		int nRetCode = -1;
		Json::Value root;
		Json::Reader reader;
		try
		{
			if (reader.parse(strHttpResponse,root))
			{
				nRetCode =  root["result"].asInt();
			}
		}
		catch (...)
		{
			CLogger::GetInstance()->PrintErrLog("ע��ʧ��");		
		}

		if (eRegisterSuccess == nRetCode)
		{
			SetRegTabView(REG_COMPLETE_VIEW);
		}
		else
		{
			CLogger::GetInstance()->PrintLog(LOG_INFO, "register failed: %s", strHttpResponse.c_str());

//			tstring::npos != root["code"].asString().find("1001")
			if (strHttpResponse.find("1001") != -1 || strHttpResponse.find("1011") != -1)
			{
				m_pLabelPinCode->SetText("��֤�����");
				m_pLabelPinCode->SetTextColor(WARNING_TEXT_COLOR);
			}		
			else
			{
				CDuiString strErrMsg;
				if (tstring::npos != root["code"].asString().find("1003"))
				{
					strErrMsg = "���û��Ѿ���ƽ̨�û�����ֱ�ӵ�¼";
				}
				else
				{
					strErrMsg = "ע��ƽ̨ʧ��";			
				}
				SetRegTabView(REG_COMPLETE_VIEW);
				m_pCtrlCompleteRegIcon->SetBkImage(REG_FAILED_ICON);
				m_pLabelRegResult->SetText(REG_RESULT_FAILED);
				m_pBtnCompleteReg->SetText(REG_BTN_RE_SINGUP);
				m_pLabelRegResultDescription->SetText(strErrMsg);
			}

			return;
		}
	}
	else
	{
		CMsgBox* pMsgBox = new CMsgBox("����","��������ע������ʧ�ܣ����Ժ����ԣ�", MSGBOX_ICON_ERROR);
		pMsgBox->ShowModal();
	}

}


void CRegisterAccountDlg::InitWindow()
{	
	m_pRegisterTipEditUI = static_cast<CEditUI*>(m_PaintManager.FindControl(_T("editRegisterTip")));
	m_pPasswdEditUI = static_cast<CEditUI*>(m_PaintManager.FindControl(_T("editPasswd")));
	m_pUserNameEditUI = static_cast<CEditUI*>(m_PaintManager.FindControl(_T("editUserName")));
	m_pConfirmPasswdEditUI = static_cast<CEditUI*>(m_PaintManager.FindControl(_T("editConfirmPasswd")));	
	m_pPinCodeEditUI = static_cast<CEditUI*>(m_PaintManager.FindControl(_T("editPinCode")));	
	m_pBtnGetPinCode = static_cast<CButtonUI*>(m_PaintManager.FindControl(_T("btnGetPinCode")));
	m_pAcceptAgree = static_cast<CCheckBoxUI*>(m_PaintManager.FindControl(_T("optAcceptAgreement")));

	m_pLabelPhoneTip = static_cast<CLabelUI*>(m_PaintManager.FindControl(_T("labelPhoneNoTip")));
	m_pLabelPinCode = static_cast<CLabelUI*>(m_PaintManager.FindControl(_T("labelPinCodeTip")));
	m_pLabelPasswdTip = static_cast<CLabelUI*>(m_PaintManager.FindControl(_T("labelPasswdTip")));
	m_pLabelRePasswdTip = static_cast<CLabelUI*>(m_PaintManager.FindControl(_T("labelRePasswdTip")));
	

	m_pCtrlCompleteRegIcon = static_cast<CControlUI*>(m_PaintManager.FindControl(_T("ctrlCompleteRegImg")));
	m_pLabelRegResult = static_cast<CLabelUI*>(m_PaintManager.FindControl(_T("labelRegResult")));
	m_pLabelRegResultDescription = static_cast<CLabelUI*>(m_PaintManager.FindControl(_T("labelRegDescription")));
	m_pBtnCompleteReg = static_cast<CButtonUI*>(m_PaintManager.FindControl(_T("btnCompleteReg")));
	m_pBtnPasswdMode = static_cast<CButtonUI*>(m_PaintManager.FindControl(_T("btnPasswdMode")));

	InitRegDlg();	
}

void CRegisterAccountDlg::InitRegDlg()
{
	SetRegTabView(dlg_type_);
	m_pLabelPhoneTip->SetText(INPUT_USERNAME_TIPTEXT);
	m_pLabelPhoneTip->SetTextColor(GRAY_TEXT_COLOR);

	m_pLabelPinCode->SetText(INPUT_PINCODE_TIPTEXT);
	m_pLabelPinCode->SetTextColor(GRAY_TEXT_COLOR);

	m_pLabelPasswdTip->SetText(INPUT_PASSWD_TIPTEXT);
	m_pLabelPasswdTip->SetTextColor(GRAY_TEXT_COLOR);

	m_pLabelRePasswdTip->SetText(INPUT_PASSWD_TIPTEXT);
	m_pLabelRePasswdTip->SetTextColor(GRAY_TEXT_COLOR);

	EnableFetchPinCodeBtn(false);	

	m_pCtrlCompleteRegIcon->SetBkImage(REG_SUCCESS_ICON);
	m_pLabelRegResult->SetText(REG_RESULT_SUCCESS);
	m_pBtnCompleteReg->SetText(REG_BTN_LOGIN);
	m_pLabelRegResultDescription->SetText(REG_SUCCESS_DESCRIPTION);

	m_pPasswdEditUI->SetPasswordMode(true);
	//m_pPasswdEditUI->SetPasswordChar(249);
	m_pConfirmPasswdEditUI->SetPasswordMode(true);

	m_pBtnPasswdMode->SetNormalImage("register\\password-off.PNG");

	if (DLG_TYPE_RESET_PASSWORD == dlg_type_)
	{
		tstring my_phone = mainDlg::getInstance()->getMyUserInfo().strPhone;
		m_pUserNameEditUI->SetText(my_phone.c_str());
		m_pUserNameEditUI->SetEnabled(false);
		m_pBtnGetPinCode->SetEnabled(true);
	}
}

bool CRegisterAccountDlg::CheckRegisterInfo()
{	
	tstring strUserName = m_pUserNameEditUI->GetText().GetData();
	tstring strPasswd = m_pPasswdEditUI->GetText().GetData();
	tstring strConfirmPasswd = m_pConfirmPasswdEditUI->GetText().GetData();
	tstring strPinCode = m_pPinCodeEditUI->GetText().GetData();

	if (tstring::npos != strUserName.find_first_of(INPUT_USERNAME_TIPTEXT)  ||  0 == strUserName.length())
	{
		m_pLabelPhoneTip->SetText(INPUT_USERNAME_TIPTEXT);
		m_pLabelPhoneTip->SetTextColor(WARNING_TEXT_COLOR);
		return false;
	}

	if (!CRegexHelper::IsValidMobileNo(strUserName.c_str()))
	{
		m_pLabelPhoneTip->SetText(MOBILE_PHONE_ERROR_TIPTEXT);
		m_pLabelPhoneTip->SetTextColor(WARNING_TEXT_COLOR);
		return false;
	}

	if (0== strPinCode.length())
	{
		m_pLabelPinCode->SetText(INPUT_PINCODE_TIPTEXT);
		m_pLabelPinCode->SetTextColor(WARNING_TEXT_COLOR);
		return false;
	}

	if (strPasswd.length() < MIN_PASSWD_LEN)
	{
		m_pLabelPasswdTip->SetText(PASSWD_SHORT_ERROR);
		m_pLabelPasswdTip->SetTextColor(WARNING_TEXT_COLOR);
		return false;
	}

	if (strPasswd.length() > MAX_PASSWD_LEN)
	{
		m_pLabelPasswdTip->SetText(PASSWD_LONG_ERROR);
		m_pLabelPasswdTip->SetTextColor(WARNING_TEXT_COLOR);
		return false;
	}

	if (0 != strPasswd.compare(strConfirmPasswd))
	{		
		m_pLabelRePasswdTip->SetText(PASSWD_MISMATCH_ERROR);
		m_pLabelRePasswdTip->SetTextColor(WARNING_TEXT_COLOR);
		return false;
	}
	
	if (!m_pAcceptAgree->GetCheck())
	{
		CMsgBox::MsgBox(this->GetHWND(), "ע��", "ע��֮ǰ�������Ķ�MMʹ��Э��", MSGBOX_ICON_WARNING);
		return false;
	}

	return true;
}


void CRegisterAccountDlg::SwitchPasswdMode()
{
	if (m_pPasswdEditUI->IsPasswordMode())
	{
		m_pPasswdEditUI->SetPasswordMode(false);
		m_pConfirmPasswdEditUI->SetPasswordMode(false);
		m_pBtnPasswdMode->SetNormalImage("register\\password-on.PNG");
	}
	else 
	{
		m_pPasswdEditUI->SetPasswordMode(true);
		m_pConfirmPasswdEditUI->SetPasswordMode(true);
		m_pBtnPasswdMode->SetNormalImage("register\\password-off.PNG");
	}
}

void CRegisterAccountDlg::HandleSetFocusEvent(TNotifyUI& msg)
{
	if (msg.pSender == m_pPasswdEditUI)
	{
		m_pPasswdEditUI->GetParent()->SetBorderColor(FOCUS_BORDER_COLOR);
	}

	if (msg.pSender == m_pUserNameEditUI && WARNING_TEXT_COLOR == m_pLabelPhoneTip->GetTextColor())
	{
		m_pLabelPhoneTip->SetText(INPUT_USERNAME_TIPTEXT);
		m_pLabelPhoneTip->SetTextColor(GRAY_TEXT_COLOR);
	}
	else if (msg.pSender == m_pPinCodeEditUI && WARNING_TEXT_COLOR == m_pLabelPinCode->GetTextColor())
	{
		m_pLabelPinCode->SetText(INPUT_PINCODE_TIPTEXT);
		m_pLabelPinCode->SetTextColor(GRAY_TEXT_COLOR);
	}
	else if (msg.pSender == m_pPasswdEditUI && WARNING_TEXT_COLOR == m_pLabelPasswdTip->GetTextColor())
	{
		m_pLabelPasswdTip->SetText(INPUT_PASSWD_TIPTEXT);
		m_pLabelPasswdTip->SetTextColor(GRAY_TEXT_COLOR);
	}
	else if (msg.pSender == m_pConfirmPasswdEditUI && WARNING_TEXT_COLOR == m_pLabelRePasswdTip->GetTextColor())
	{
		m_pLabelRePasswdTip->SetText(INPUT_PASSWD_TIPTEXT);
		m_pLabelRePasswdTip->SetTextColor(GRAY_TEXT_COLOR);
	}
}

void CRegisterAccountDlg::HandleKillFocusEvent(TNotifyUI& msg)
{
	if (msg.pSender == m_pPasswdEditUI)
	{
		m_pPasswdEditUI->GetParent()->SetBorderColor(BORDER_COLOR);
	}
}

void CRegisterAccountDlg::HandleTextChangedEvent( TNotifyUI& msg )
{
	if (msg.pSender == m_pUserNameEditUI)
	{	
		EnableFetchPinCodeBtn(MOBILE_NUM_LEN == m_pUserNameEditUI->GetText().GetLength());	
	}
}


void CRegisterAccountDlg::GetVerifyCode()
{
	if (!CRegexHelper::IsValidMobileNo(m_pUserNameEditUI->GetText().GetData()))
	{
		m_pLabelPhoneTip->SetText(MOBILE_PHONE_ERROR_TIPTEXT);
		m_pLabelPhoneTip->SetTextColor(WARNING_TEXT_COLOR);
		return ;
	}

	tstring strPhone = m_pUserNameEditUI->GetText().GetData();

	//�ж�������Ƿ��Ѿ�ע����ˡ�	
	if (CWebClient::IsValidUser(strPhone))
	{
		
		m_pLabelPhoneTip->SetText("���ֻ������Ѿ���ע�ᣬ��ֱ�ӵ�¼");
		m_pLabelPhoneTip->SetTextColor(WARNING_TEXT_COLOR);
		return;
				
	}
	
	//////////////////////////////////////////////////////////////////////////
	tstring strUrl(WEB_SERVER_SITE_URL);
	strUrl += _T("create-verify-code");

	tstring strPost = _T("");
	strPost += _T("cell_phone=");
	strPost += m_pUserNameEditUI->GetText().GetData();	

	CHttpClient httpClient;
	tstring strHttpResponse;
	int retCode = httpClient.Post(strUrl, strPost,strHttpResponse);
	CLogger::GetInstance()->PrintLog(LOG_INFO, "get register pin code:%s", strHttpResponse.c_str());

	if (strHttpResponse.empty())
	{
		m_pLabelPhoneTip->SetText("���糬ʱ����ȡ��֤��ʧ�ܣ������ԣ�");
		m_pLabelPhoneTip->SetTextColor(WARNING_TEXT_COLOR);
		return;
	}

	if (strHttpResponse.find("result") != -1)
	{
		bool bSucc = false;
		Json::Reader reader;
		Json::Value root;
		if (reader.parse(strHttpResponse, root))
		{
//			int nResult = root["result"].asInt();
//			if (nResult == 1)
			if (strHttpResponse.find("\"result\":1") != -1)
			{
				bSucc = true;
			}
			else if(strHttpResponse.find("\"result\":0") != -1)
			{
				if (!root["code"].isNull())
				{
					if (tstring::npos != root["code"].asString().find("1003"))
					{
						m_pLabelPhoneTip->SetText("���ֻ������Ѿ���ע�ᣬ��ֱ�ӵ�¼");
						m_pLabelPhoneTip->SetTextColor(WARNING_TEXT_COLOR);
						return;
					}
					else
					{
						m_pLabelPhoneTip->SetText(MOBILE_PHONE_ERROR_TIPTEXT);
						m_pLabelPhoneTip->SetTextColor(WARNING_TEXT_COLOR);
						return;
					}
				}
			}
		}

		if (bSucc)
		{
			m_pLabelPhoneTip->SetText(SEND_PIN_TIP);

			SetTimer(m_PaintManager.GetPaintWindow(), TIMERID_VERIFY_CODE, TIMER_UNIT_SECOND, NULL);
			m_nPinLifecycle = PIN_CODE_VALID_TIME;
			CDuiString strMsg;
			strMsg.Format("ʣ��%d��", m_nPinLifecycle);
			m_pBtnGetPinCode->SetText(strMsg);
			m_pBtnGetPinCode->SetEnabled(false);
			m_pBtnGetPinCode->SetNormalImage(IMG_GET_PIN_BKIMG);
		}
	}
	else
	{
		m_pLabelPhoneTip->SetText("������������ʧ�ܣ������ԣ�");
		m_pLabelPhoneTip->SetTextColor(WARNING_TEXT_COLOR);
		return;
	}
	
}

void CRegisterAccountDlg::GetVerifyCodeFromCss()
{
	//////////////////////////////////////////////////////////////////////////
	//��CSS��ȡ��֤��
	tstring ret_verify_code("123456");
	tstring strUrl("124.206.140.155:8090/commonApi/cssapi.do?txid=jam0011");
	//	strUrl += _T("sign-up");

	orgInfo info;
	CWebClient::GetOrgByPhone(mainDlg::getInstance()->getMyUserInfo().strAccount, info);

	tstring strPost;
	std::ostringstream os;
	os<<"{";
	os<<"\"appcode\":\""<<"MMT"<<"\","
		<<"\"userid\":\""<<mainDlg::getInstance()->getMyUserInfo().strAccount<<"\","
		<<"\"username\":\""<<mainDlg::getInstance()->getMyUserInfo().strUserNickname<<"\","
		<<"\"orgid\":\""<<info.strId<<"\","
		<<"\"orgname\":\""<<info.strName<<"\","
		<<"\"mmbno\":\""<<info.strMemberId<<"\""//������Ա��
		<<"}";

	strPost = os.str();

	tstring strHttpResponse;
	CHttpClient httpClient;

	tstring strUTF8Url;
	CChineseCode::GB2312ToUTF_8(strUTF8Url, strUrl.c_str(), strUrl.length());

	tstring strUTF8Post;
	CChineseCode::GB2312ToUTF_8(strUTF8Post, strPost.c_str(), strPost.length());

	int retCode = httpClient.CSSPost(strUrl, strPost, strHttpResponse);

	strHttpResponse = CChineseCode::DecodeUTF8(strHttpResponse);

	ReplaceAll(strHttpResponse, "\\", "");


	strHttpResponse = strHttpResponse.substr(1, strHttpResponse.length());
	strHttpResponse = strHttpResponse.substr(0, strHttpResponse.length()-1);

	if (strHttpResponse.find("retcode") != -1 && strHttpResponse.find("retmsg") != -1
		&& strHttpResponse.find("vcode") != -1)
	{
		tstring ret_code;
		tstring ret_string;
		Json::Value root;
		Json::Reader reader;
		try
		{
			if (reader.parse(strHttpResponse,root))
			{
				ret_code =  root["retcode"].asString();
				ret_string =  root["retmsg"].asString();
				ret_verify_code = root["vcode"].asString();
			}
		}
		catch (...)
		{
			tstring str;
			str = "��CSS��ȡ��֤��ʧ��,HttpӦ��:  " + strHttpResponse;
			CLogger::GetInstance()->PrintErrLog(str.c_str());		
		}

		if ("0" == ret_code)
		{
//			CMsgBox* pMsgBox = new CMsgBox("����֧������","����֧������ɹ���", MSGBOX_ICON_ERROR);
//			pMsgBox->ShowModal();
		}
		else
		{
			tstring str;
			str = "��ѯ������֤����󣬴�����룺" + ret_code;
			CMsgBox* pMsgBox = new CMsgBox("����", str, MSGBOX_ICON_ERROR);
			pMsgBox->ShowModal();
			return;
		}
	}
	else
	{
		tstring str;
		str = "������������ʧ�ܣ�" + strHttpResponse;
		CMsgBox* pMsgBox = new CMsgBox("����", str, MSGBOX_ICON_ERROR);
		pMsgBox->ShowModal();
//		return;
	}
	//////////////////////////////////////////////////////////////////////////



	strUrl = WEB_SERVER_SITE_URL;
	strUrl += _T("create-verify-code");
	strUrl += _T("?tid=4");
	strUrl += _T("&code=");
	strUrl += ret_verify_code;

	strPost = _T("");
	strPost += _T("cell_phone=");
	strPost += m_pUserNameEditUI->GetText().GetData();	
	
	strHttpResponse = "";
	retCode = httpClient.Post(strUrl, strPost,strHttpResponse);
	CLogger::GetInstance()->PrintLog(LOG_INFO, "get register pin code:%s", strHttpResponse.c_str());

	if (strHttpResponse.empty())
	{
		m_pLabelPhoneTip->SetText("���糬ʱ����ȡ��֤��ʧ�ܣ������ԣ�");
		m_pLabelPhoneTip->SetTextColor(WARNING_TEXT_COLOR);
		return;
	}

	//if (strHttpResponse.find("code") != -1)
	if (strHttpResponse.find("result") != -1)
	{
		bool bSucc = false;
		Json::Reader reader;
		Json::Value root;
		if (reader.parse(strHttpResponse, root))
		{
			int nResult = root["result"].asInt();
			if (nResult == 1)
			{
				//
				bSucc = true;
			}
			else if(nResult == 0)
			{
				if (!root["code"].isNull())
				{
					if (tstring::npos != root["code"].asString().find("1003"))
					{
						m_pLabelPhoneTip->SetText("���ֻ������Ѿ���ע�ᣬ��ֱ�ӵ�¼");
						m_pLabelPhoneTip->SetTextColor(WARNING_TEXT_COLOR);
						return;
					}
					else
					{
						m_pLabelPhoneTip->SetText(MOBILE_PHONE_ERROR_TIPTEXT);
						m_pLabelPhoneTip->SetTextColor(WARNING_TEXT_COLOR);
						return;
					}
				}
			}
		}

		if (bSucc)
		{
			m_pLabelPhoneTip->SetText(SEND_PIN_TIP);

			SetTimer(m_PaintManager.GetPaintWindow(), TIMERID_VERIFY_CODE, TIMER_UNIT_SECOND, NULL);
			m_nPinLifecycle = PIN_CODE_VALID_TIME;
			CDuiString strMsg;
			strMsg.Format("ʣ��%d��", m_nPinLifecycle);
			m_pBtnGetPinCode->SetText(strMsg);
			m_pBtnGetPinCode->SetEnabled(false);
			m_pBtnGetPinCode->SetNormalImage(IMG_GET_PIN_BKIMG);
		}
	}
	else
	{
		m_pLabelPhoneTip->SetText("������������ʧ�ܣ������ԣ�");
		m_pLabelPhoneTip->SetTextColor(WARNING_TEXT_COLOR);
		return;
	}

}

void  CRegisterAccountDlg::CreateWnd(HWND hWnd)
{
	if (!m_bActive)
	{
		__super::CreateWnd(hWnd);
		m_bActive = true;
	}
	switch (dlg_type_)
	{
	case REG_PAGE_VIEW:
		break;
	case DLG_TYPE_RESET_PASSWORD:
		break;
	default:
		break;
	}

}

void CRegisterAccountDlg::ResetPassword()
{

// 	return;
// 	if (!CheckRegisterInfo())
// 	{
// 		return;
// 	}


//http://localhost:8088/commonApi/cssapi.do?txid=JLM001
	
//	body {"userid":"123","orgname":"orgname","appcode":"UNIPEI","orgid":"2"}

	tstring strUrl("124.206.140.155:8090/commonApi/cssapi.do?txid=jam012");
//	strUrl += _T("sign-up");

	orgInfo info;
	CWebClient::GetOrgByPhone(mainDlg::getInstance()->getMyUserInfo().strAccount, info);

	tstring strPost;
	std::ostringstream os;
	os<<"{";
	os<<"\"appcode\":\""<<"MMT"<<"\","
		<<"\"userid\":\""<<mainDlg::getInstance()->getMyUserInfo().strAccount<<"\","
		<<"\"username\":\""<<mainDlg::getInstance()->getMyUserInfo().strUserNickname<<"\","
		<<"\"orgid\":\""<<info.strId<<"\","
		<<"\"orgname\":\""<<info.strName<<"\","
		<<"\"mmbno\":\""<<info.strMemberId<<"\","//������Ա��
		<<"\"newtxpass\":\""<<"321000"<<"\","
		<<"\"newtxpasscf\":\""<<"1"<<"\","
		<<"\"vcode\":\""<<"000000"<<"\""
		<<"}";

	strPost = os.str();

	tstring strHttpResponse;
	CHttpClient httpClient;

	tstring strUTF8Url;
	CChineseCode::GB2312ToUTF_8(strUTF8Url, strUrl.c_str(), strUrl.length());

	tstring strUTF8Post;
	CChineseCode::GB2312ToUTF_8(strUTF8Post, strPost.c_str(), strPost.length());

	int retCode = httpClient.CSSPost(strUrl, strPost, strHttpResponse);

	strHttpResponse = CChineseCode::DecodeUTF8(strHttpResponse);

	ReplaceAll(strHttpResponse, "\\", "");


	strHttpResponse = strHttpResponse.substr(1, strHttpResponse.length());
	strHttpResponse = strHttpResponse.substr(0, strHttpResponse.length()-1);

	if (strHttpResponse.find("retcode") != -1 && strHttpResponse.find("retmsg") != -1)
	{
		tstring ret_code;
		tstring ret_string;
		Json::Value root;
		Json::Reader reader;
		try
		{
			if (reader.parse(strHttpResponse,root))
			{
				ret_code =  root["retcode"].asString();
				ret_string =  root["retmsg"].asString();
			}
		}
		catch (...)
		{
			tstring str;
			str = "ע��ʧ��,HttpӦ��:  " + strHttpResponse;
			CLogger::GetInstance()->PrintErrLog(str.c_str());		
		}

		if ("0" == ret_code)
		{
			CMsgBox* pMsgBox = new CMsgBox("����֧������","����֧������ɹ���", MSGBOX_ICON_ERROR);
			pMsgBox->ShowModal();
		}
		else
		{
			//������ʱ����
 			CLogger::GetInstance()->PrintLog(LOG_INFO, "reset password failed: %s", strHttpResponse.c_str());

			tstring str;
			str = "����֧��������󣬴�����룺" + ret_code;
			CMsgBox* pMsgBox = new CMsgBox("����", str, MSGBOX_ICON_ERROR);
			pMsgBox->ShowModal();

			return;
		}
	}
	else
	{
		CMsgBox* pMsgBox = new CMsgBox("����","��������ע������ʧ�ܣ����Ժ����ԣ�", MSGBOX_ICON_ERROR);
		pMsgBox->ShowModal();
	}
}
