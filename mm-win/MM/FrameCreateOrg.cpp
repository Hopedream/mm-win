#include "FrameCreateOrg.h"
#include "mainDlg.h"
#include <CommDlg.h>
#include "MsgBox.h"
#include "StringHelper.h"
#include "Controller.h"
#include "FileHelper.h"
#include "../Common/Logger.h"
#include "MagicKHelper.h"
#include "webClient.h"
#include "../gloox-1.0.14/src/base64.h"
#include <sstream>


const CDuiString municipalities[] = {"����","���","�Ϻ�","����" };

CFrameCreateOrg* CFrameCreateOrg::m_pInstance = NULL;
CFrameCreateOrg::CFrameCreateOrg()
{
	m_bSelectedAvatar = false;
}


CFrameCreateOrg::~CFrameCreateOrg(void)
{
}


CFrameCreateOrg* CFrameCreateOrg::Instance()
{
	if(NULL == m_pInstance)
	{
		m_pInstance = new CFrameCreateOrg;
		m_pInstance->CreateWnd(mainDlg::getInstance()->GetHWND());
	}
	return m_pInstance;
}

void CFrameCreateOrg::HandleSetFocusEvent( TNotifyUI& msg )
{

}

void CFrameCreateOrg::HandleKillFocusEvent( TNotifyUI& msg )
{

}

void CFrameCreateOrg::HandleTextChangedEvent( TNotifyUI& msg )
{

}

void CFrameCreateOrg::HandleClickEvent( TNotifyUI& msg )
{
	if (_tcsicmp(msg.pSender->GetName(), _T("btn_close")) == 0)
	{
		clearFrame();
		ShowWindow(false);
	}
	else if (_tcsicmp(msg.pSender->GetName(), _T("btn_select_avatar")) == 0)
	{
		onSelectAvatar();
	}
	else if (_tcsicmp(msg.pSender->GetName(), _T("btn_ok")) == 0)
	{
		createOrg();
	}
	else if (_tcsicmp(msg.pSender->GetName(), _T("btn_cancel")) == 0)
	{
		clearFrame();
		ShowWindow(false);
	}
}

void CFrameCreateOrg::HandleItemSelectEvent( TNotifyUI& msg )
{
	if (m_pCombProvince == msg.pSender)
	{
		CDuiString strProvinceName = m_pCombProvince->GetText();
		m_pCombDistrict->SetEnabled(true);
		initCityComb(strProvinceName);
	}
	else if (m_pCombCity == msg.pSender)
	{
		initDistrictComb(m_pCombCity->GetText());
	}
	else if (m_pCombProperty == msg.pSender)
	{
		//tstring::number()
		int iSel = m_pCombProperty->GetCurSel() +1;
		stringstream ss;
		ss<<iSel;
		m_stProperty = ss.str();
	}
}

DuiLib::CDuiString CFrameCreateOrg::GetSkinFile()
{
	return _T("sys_create_org.xml");
}

CControlUI* CFrameCreateOrg::CreateControl( LPCTSTR pstrClass )
{
	return NULL;
}

void CFrameCreateOrg::InitWindow()
{
	m_pBtnSelect = static_cast<CButtonUI*>(m_PaintManager.FindControl(_T("btn_select_avatar")));
	m_pEdtName = static_cast<CEditUI*>(m_PaintManager.FindControl(_T("edit_org_name")));
	m_pCombProvince = static_cast<CComboUI*>(m_PaintManager.FindControl(_T("CombProvince")));
	m_pCombCity = static_cast<CComboUI*>(m_PaintManager.FindControl(_T("CombCity")));
	m_pCombDistrict = static_cast<CComboUI*>(m_PaintManager.FindControl(_T("CombDistrict")));
	m_pEditCellphone = static_cast<CEditUI*>(m_PaintManager.FindControl(_T("edit_cellphone")));
	//m_pEditAreacode = static_cast<CEditUI*>(m_PaintManager.FindControl(_T("edit_area_code")));
	m_pEditTelephone = static_cast<CEditUI*>(m_PaintManager.FindControl(_T("edit_phone")));
	m_pEditAddress = static_cast<CEditUI*>(m_PaintManager.FindControl(_T("edit_address")));

	initPropertyComb();
	ParseLocInfo();
	initProvinceComb();
	//getorg();
	//deleteorg();
}

void CFrameCreateOrg::initPropertyComb()
{
	m_pCombProperty = static_cast<CComboUI*>(m_PaintManager.FindControl(_T("CombProperty")));
	if (m_pCombProperty  == NULL)
		return;
	
	for (int i=0; i<ORG_PROPERTY_LENGTH; i++)
	{
		CListLabelElementUI* pListLabelElement = new CListLabelElementUI;
		tstring strValue = g_OrgProperty[i];
		pListLabelElement->SetText(strValue.c_str());
		pListLabelElement->SetTag(i);
		pListLabelElement->SetFixedWidth(210);
		m_pCombProperty->Add(pListLabelElement);
		if (ORG_PROPERTY_LENGTH >= 2)
		{
			m_pCombProperty->SelectItem(2);  //Ĭ��ѡ�о�����
		}
	}
	
}

void CFrameCreateOrg::clearFrame()
{
	m_pBtnSelect->SetBkImage("");
	m_pEditAddress->SetText("");
	//m_pEditAreacode->SetText("");
	m_pEditCellphone->SetText("");
	m_pEditTelephone->SetText("");
	m_pEdtName->SetText("");
}

void CFrameCreateOrg::onSelectAvatar()
{
	vector<tstring> lstFiles;
	if (getSelectedFiles(lstFiles,"JPG��ʽͼƬ\0*.jpg\0JPEG��ʽͼƬ\0\*.jpeg\0BMP��ʽͼƬ\0*.bmp\0All\0*.*\0"), false);
	{		
		if (0 == lstFiles.size())
		{
			return;
		}
		tstring strPhonePath = lstFiles.front();
		strPhonePath = CChineseCode::EncodeUTF8(strPhonePath);
		tstring strId = getTmpFileId();
		tstring strtmp;
		strtmp = CFileHelper::GetHeadCacheDir();
		strtmp += strId +  "tmp.jpeg" ;			
		if (CMagicKHelper::Instance()->ResizeImage(strPhonePath, strtmp, 100, 100))
		{
			m_pBtnSelect->SetBkImage(strtmp.c_str());	
		}
		else
		{
			CMsgBox* pMsgBox = new CMsgBox("��ʾ", "��ȡ�ļ�ʧ�ܣ�",  MSGBOX_ICON_WARNING);
			pMsgBox->ShowModal(this->GetHWND());
			return;

		}
		

		//	//2. 
		CMagicKHelper::Instance()->GetPhotoBinval(strtmp, m_strAvatar);
		//3.
		m_bSelectedAvatar = true;
		m_strAvatarType = "image/jpeg";
	}
}

bool CFrameCreateOrg::createOrg()
{
	orgInfo orginfo;
	orginfo.strPhotoBinary = m_strAvatar;
	orginfo.strPhotoType = m_strAvatarType;

	if (m_pEdtName->GetText().GetLength() < 1)
	{
		CMsgBox* pMsgBox = new CMsgBox("��ʾ", "��ҵ���Ʋ���Ϊ�գ�",  MSGBOX_ICON_WARNING);
		pMsgBox->ShowModal(this->GetHWND());
		return false;
	}
	else
		orginfo.strName = m_pEdtName->GetText();
	if (m_pCombProvince->GetText().GetLength()<1)
	{
		CMsgBox* pMsgBox = new CMsgBox("��ʾ", "��ѡ��ʡ��",  MSGBOX_ICON_WARNING);
		pMsgBox->ShowModal(this->GetHWND());
		return false;
	}
	else
	{
		if (m_pCombCity->GetText().GetLength()<1)
		{
			CMsgBox* pMsgBox = new CMsgBox("��ʾ", "��ѡ���У�������",  MSGBOX_ICON_WARNING);
			pMsgBox->ShowModal(this->GetHWND());
			return false;
		}
		else
		{
			CDuiString strProvinceName = m_pCombProvince->GetText();
			if (m_pCombDistrict->GetText().GetLength()<1 && !IsMunicipalities(strProvinceName))
			{
				CMsgBox* pMsgBox = new CMsgBox("��ʾ", "��ѡ�������أ���",  MSGBOX_ICON_WARNING);
				pMsgBox->ShowModal(this->GetHWND());
				return false;
			}
			else
			{
				tstring strPro = m_pCombProvince->GetItemAt(m_pCombProvince->GetCurSel())->GetName();
				tstring strCity = m_pCombCity->GetItemAt(m_pCombCity->GetCurSel())->GetName();
				tstring strDis = m_pCombDistrict->GetItemAt(m_pCombDistrict->GetCurSel())->GetName();

				orginfo.strAddress = strPro + "," + strCity + "," + strDis;

				orginfo.strDetailAddr =m_pEditAddress->GetText();
			}
		}
	}
	orginfo.strTelephone = m_pEditTelephone->GetText();
	orginfo.strTelephone = m_pEditTelephone->GetText();
	orginfo.strCellPhone = m_pEditCellphone->GetText();
	orginfo.strProperty = m_stProperty;
	//׼������ȫ��ok
	//1. �ȴ���һ��org
	if (!apiCreateOrg(orginfo))
	{
		return false;
	}
	//2. �ٸ���������Ϣ����org   ����ͷ��
	if (m_bSelectedAvatar)
	{
		m_bSelectedAvatar = false;
		if (!apisaveAvatar(m_strOid))
		{
			CMsgBox* pMsgBox = new CMsgBox("��ʾ", "����ͷ��ʧ�ܣ�",  MSGBOX_ICON_WARNING);
			pMsgBox->ShowModal(this->GetHWND());
		}
	}
	
	CMsgBox* pMsgBox = new CMsgBox("��ʾ", "���������ɹ���",  MSGBOX_ICON_WARNING);
	pMsgBox->ShowModal(this->GetHWND());
	clearFrame();
	ShowWindow(false);
	mainDlg::getInstance()->setUserIdentity(USER_ORG_BOSS);
	//mainDlg::getInstance()->PostMessage(WM_CREATE_ORG);
	return true;
}

bool CFrameCreateOrg::IsMunicipalities(CDuiString strProvinceName)
{
	bool bFound = false;
	for (int idx = 0; idx < sizeof(municipalities)/sizeof(CDuiString); ++idx)
	{
		if (0 == strProvinceName.CompareNoCase(municipalities[idx]))
		{
			bFound = true;
			break;
		}
	}
	return bFound;
}

bool CFrameCreateOrg::apiCreateOrg( orgInfo & info)
{
	CHttpClient httpClient;
	tstring strUrl(WEB_SERVER_BASE_URL);
	strUrl += _T("org/create-org?");
	tstring strPost = _T("");
	strPost += _T("name=");
	tstring strNameDefault = info.strName;
	strPost += strNameDefault;
	strPost += _T("&address=");// 110000,110100,110101");
	strPost += info.strAddress;
	strPost += _T("&phone=");
	strPost += info.strCellPhone;
	strPost += _T("&detail_address=");
	strPost += info.strDetailAddr;
	strPost += _T("&telephone=");
	strPost += info.strTelephone;
	strPost += _T("&cate=");//�¼ӵ�������������ʱ��Ϊ1
	strPost += info.strProperty;
 	strPost += _T("&txpass=");//֧������
 	strPost += "123456";
 	strPost += _T("&txpasscf=");//֧������
 	strPost += "123456";


	strUrl += strPost;
	tstring strToken = CController::Instance()->GetToken();
	tstring strHttpResponse;
	int retCode = httpClient.Get(CChineseCode::EncodeUTF8(strUrl), strToken, strHttpResponse);
	if (strHttpResponse.find("\"result\":1") == -1)
	{
		if (strHttpResponse.find("the user has org already") != -1)
		{
			CMsgBox* pMsgBox = new CMsgBox("��ʾ", "�û����л������޷��ٴδ�����",  MSGBOX_ICON_WARNING);
			pMsgBox->ShowModal(this->GetHWND());

		}
		else if (strHttpResponse.find("the name is existed already") != -1)
		{
			CMsgBox* pMsgBox = new CMsgBox("��ʾ", "�ظ�������������ʧ�ܣ�",  MSGBOX_ICON_WARNING);
			pMsgBox->ShowModal(this->GetHWND());
		}
		else
		{
			CMsgBox* pMsgBox = new CMsgBox("��ʾ", "��������ʧ�ܣ�",  MSGBOX_ICON_WARNING);
			pMsgBox->ShowModal(this->GetHWND());
		}
		m_strOid = "-1";
		return false;
	}
	else
	{
//		m_strOid = strHttpResponse.substr(22, strHttpResponse.find("\"}") - 22);
		Json::Reader reader;
		Json::Value root;
		try{
			if (reader.parse(CChineseCode::DecodeUTF8(strHttpResponse), root))
			{
				m_strOid =  root["org_id"].asString();
				return true;
			}
		}catch(...){
			CLogger::GetInstance()->PrintErrLog("parse http response exception: %s", strHttpResponse.c_str());
		}
		return true;
	}
}


void CFrameCreateOrg::initProvinceComb()
{
	m_pCombProvince->RemoveAll();
	for (int  idx = 0; idx < m_valLoc["provinces"].size(); ++idx)
	{
		tstring strCode = m_valLoc["provinces"][idx]["region_code"].asString();
		tstring strProvinceName = m_valLoc["provinces"][idx]["region_name"].asString();
		CListLabelElementUI* pListLabelElement = new CListLabelElementUI;
		pListLabelElement->SetText(strProvinceName.c_str());
		pListLabelElement->SetName(strCode.c_str());
		pListLabelElement->SetTag(idx);
		pListLabelElement->SetFixedWidth(120);
		m_pCombProvince->Add(pListLabelElement);
	}	
}


void CFrameCreateOrg::initCityComb(CDuiString strProvinceName)
{
	m_pCombCity->RemoveAll();
	for (int  idx = 0; idx < m_valLoc["provinces"].size(); ++idx)
	{
		if (0 == strProvinceName.CompareNoCase(m_valLoc["provinces"][idx]["region_name"].asString().c_str()))
		{
			m_valProvince = m_valLoc["provinces"][idx]; //
			for (int nCityIdx = 0; nCityIdx < m_valProvince["cities"].size(); ++nCityIdx)
			{
				tstring strCityName = m_valProvince["cities"][nCityIdx]["region_name"].asString();
				tstring  strCode = m_valProvince["cities"][nCityIdx]["region_code"].asString();
				CListLabelElementUI* pListLabelElement = new CListLabelElementUI;
				pListLabelElement->SetText(strCityName.c_str());
				pListLabelElement->SetName(strCode.c_str());		
				pListLabelElement->SetTag(nCityIdx);
				m_pCombCity->Add(pListLabelElement);
			}
			m_pCombCity->SelectItem(0);
			return;
		}		
	}	
}

void CFrameCreateOrg::initDistrictComb(CDuiString strCityName)
{
	m_pCombDistrict->RemoveAll();

	for (int nCityIdx = 0; nCityIdx < m_valProvince["cities"].size(); ++nCityIdx)
	{
		Json::Value valCity = m_valProvince["cities"][nCityIdx];
		if (0 == strCityName.CompareNoCase( valCity["region_name"].asString().c_str()))
		{
			for (int nRegionIdx = 0; nRegionIdx < valCity["districts"].size(); ++nRegionIdx)
			{
				CListLabelElementUI* pListLabelElement = new CListLabelElementUI;
				tstring  strCode = valCity["districts"][nRegionIdx]["region_code"].asString();
				pListLabelElement->SetText(valCity["districts"][nRegionIdx]["region_name"].asString().c_str());
				pListLabelElement->SetName(strCode.c_str());
				pListLabelElement->SetTag(nRegionIdx);
				m_pCombDistrict->Add(pListLabelElement);
			}	
			m_pCombDistrict->SelectItem(0);
			return;
		}			
	}
}

bool CFrameCreateOrg::ParseLocInfo()
{
	tstring strRegions = CWebClient::getRegions();
	strRegions = CChineseCode::DecodeUTF8(strRegions);
	Json::Reader reader;
	return reader.parse(strRegions,m_valLoc);
}

bool CFrameCreateOrg::apisaveAvatar(tstring stroid)
{
	if (stroid == "-1")
		return false;
	CHttpClient httpClient;

	tstring strTmp = m_strAvatar;
	tstring strDecodedBuff = Base64::encode64(strTmp);
	strDecodedBuff = UrlEncode(strDecodedBuff);

	tstring strUrl(WEB_SERVER_BASE_URL);
	strUrl += _T("org/save-org-avatar");
	tstring strPost = _T("oid=") + stroid;
	strPost += _T("&avatar=");
	strPost += strDecodedBuff;
	strPost += _T("&type=");
	strPost += m_strAvatarType;
	tstring strToken = CController::Instance()->GetToken();
	tstring strHttpResponse;
	int retCode = httpClient.Post(strUrl, strPost, strToken, strHttpResponse);
	if (strHttpResponse.find("\"result\":1") == -1)
	{
		return false;
	}
	else
	{
		return true;
	}
}

bool CFrameCreateOrg::deleteorg()
{
	CHttpClient httpClient;
	tstring strUrl(WEB_SERVER_BASE_URL);
	strUrl += _T("org/del-org?");
	tstring strPost = _T("");
	strPost += _T("oid=");
	strPost += m_strOid;
	strUrl += strPost;
	tstring strToken = CController::Instance()->GetToken();
	tstring strHttpResponse;
	int retCode = httpClient.Get(CChineseCode::EncodeUTF8(strUrl), strToken, strHttpResponse);
	if (strHttpResponse.find("\"result\":1") == -1)
	{
		return false;
	}
	else
	{		
		return true;
	}
}

bool CFrameCreateOrg::getorg()
{
	CHttpClient httpClient;
	tstring strUrl(WEB_SERVER_BASE_URL);
	strUrl += _T("org/get-org");
	tstring strPost; // = _T("oid=");
	
	tstring strToken = CController::Instance()->GetToken();
	tstring strHttpResponse;
	int retCode = httpClient.Post(strUrl, strPost, strToken, strHttpResponse);
	if (strHttpResponse.find("\"result\":0") == -1)
	{
		m_strOid = strHttpResponse.substr(7, strHttpResponse.find("\",\"") - 7);
		return true;	
	}
	else
	{
		return false;
	}
}