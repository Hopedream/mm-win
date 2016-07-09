#include "FrameChangeOrganization.h"
#include "Controller.h"
#include "mainDlg.h"
#include "FileHelper.h"
#include "../Common/Logger.h"
#include "StringHelper.h"
#include "MagicKHelper.h"
#include <CommDlg.h>
#include "MsgBox.h"
#include "base64.h"
#include <cstring>
#include "webClient.h"
#include "../gloox-1.0.14/src/base64.h"
#include "base64_.h"
#include <ShellAPI.h>
#include "../Common/regexHelper.h" 
#include <sstream>

const CDuiString municipalities[] = {"北京","天津","上海","重庆" };

CFrameChangeOrganization* CFrameChangeOrganization::m_pInstance = NULL;

CFrameChangeOrganization::CFrameChangeOrganization()
{
	CController::Instance()->GetUserInfoMgr()->AddEventHandler((IUserManagerEvent*)this);
	m_bSelectedAvatar = false;
}


CFrameChangeOrganization::~CFrameChangeOrganization(void)
{
	CController::Instance()->GetUserInfoMgr()->RemoveEventHandler((IUserManagerEvent*)this);
}

void CFrameChangeOrganization::HandleSetFocusEvent( TNotifyUI& msg )
{

}

void CFrameChangeOrganization::HandleKillFocusEvent( TNotifyUI& msg )
{

}

void CFrameChangeOrganization::HandleTextChangedEvent( TNotifyUI& msg )
{

}

void CFrameChangeOrganization::HandleClickEvent( TNotifyUI& msg )
{
	if (_tcsicmp(msg.pSender->GetName(), "btn_close") == 0)
	{
		clearwindow();
		
		ShowWindow(false);
		updateWindow();
	}
	else if (_tcsicmp(msg.pSender->GetName(), "btn_ok") == 0)
	{
		saveOrgInfo();
	}
	else if (_tcsicmp(msg.pSender->GetName(), "btn_cancel") == 0)
	{
		//getOrgAvatar();
		clearwindow();
		ShowWindow(false);
		updateWindow();
	}
	else if (_tcsicmp(msg.pSender->GetName(), "btn_select_avatar") == 0)
	{
		onSelectAvatar();
	}
}

bool CFrameChangeOrganization::IsMunicipalities(CDuiString strProvinceName)
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

void CFrameChangeOrganization::HandleItemSelectEvent( TNotifyUI& msg )
{
	if (m_pCombProvince == msg.pSender)
	{
		CDuiString strProvinceName = m_pCombProvince->GetText();
		/*if (IsMunicipalities(strProvinceName))
		{
			m_pCombDistrict->SetText("");
			m_pCombDistrict->SetEnabled(false);
		}
		else */
		//{
			m_pCombDistrict->SetEnabled(true);
		//}
		initCityComb(strProvinceName);
	}
	else if (m_pCombCity == msg.pSender)
	{
		initDistrictComb(m_pCombCity->GetText());
	}
	else if (m_pCombProperty == msg.pSender)
	{
		int iSel = m_pCombProperty->GetCurSel() + 1;
		stringstream ss;
		ss<<iSel;
		m_stProperty = ss.str();
	}
}

DuiLib::CDuiString CFrameChangeOrganization::GetSkinFile()
{
	return _T("sys_change_organization1.xml");
}

CControlUI* CFrameChangeOrganization::CreateControl( LPCTSTR pstrClass )
{
	return NULL;
}

void CFrameChangeOrganization::InitWindow()
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
	m_pCombProperty = static_cast<CComboUI*>(m_PaintManager.FindControl(_T("CombProperty")));

	initPropertyComb();
	ParseLocInfo();
	initProvinceComb();
	//updateWindow();
	getOrgInfo();
	getOrgAvatar();
}

CFrameChangeOrganization* CFrameChangeOrganization::Instance()
{
	if(NULL == m_pInstance)
	{
		m_pInstance = new CFrameChangeOrganization;
		m_pInstance->CreateWnd(mainDlg::getInstance()->GetHWND());
	}
	return m_pInstance;
}

void CFrameChangeOrganization::OnMySelfCardInfo( sUserVCard& oneInfo )
{
	
	//int i = 0;
	//sUserVCard* pMyVcard = new sUserVCard(oneInfo);
	//m_myVcard = oneInfo;
	//PostMessage(WM_FETCH_VCARD);
	updateWindow();
}

void CFrameChangeOrganization::OnStoreVCardResult( tstring& strAccount, bool bSucc )
{
	if (bSucc)
	{
		//m_plblTips->SetText(_T("修改机构信息成功！"));
		CController::Instance()->GetUserInfoMgr()->FetchVCardByAccount(m_myUserInfo.strAccount);
	}
	else 
	{
		//m_plblTips->SetText(_T("修改机构信息失败！"));
	}
}

void CFrameChangeOrganization::updateWindow()
{
	getOrgInfo();
	getOrgAvatar();
}

bool CFrameChangeOrganization::ParseLocInfo()
{
	tstring strRegions = CWebClient::getRegions();
	strRegions = CChineseCode::DecodeUTF8(strRegions);
	Json::Reader reader;
	return reader.parse(strRegions,m_valLoc);
}

void CFrameChangeOrganization::initVocationComb()
{

}



void CFrameChangeOrganization::initProvinceComb()
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


void CFrameChangeOrganization::initCityComb(CDuiString strProvinceName)
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

void CFrameChangeOrganization::initDistrictComb(CDuiString strCityName)
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

void CFrameChangeOrganization::clearwindow()
{
	//m_plblTips->SetText("");
}

void CFrameChangeOrganization::showFocus()
{
	//static_cast<CButtonUI*>(m_PaintManager.FindControl("buttonClose"))->SetFocus();
}

bool CFrameChangeOrganization::getOrgInfo()
{
	CHttpClient httpClient;
	tstring strUrl(WEB_SERVER_BASE_URL);
	strUrl += _T("org/get-org");
	tstring strPost; 
	tstring strToken = CController::Instance()->GetToken();
	tstring strHttpResponse;
	int retCode = httpClient.Post(strUrl, strPost, strToken, strHttpResponse);
	if (strHttpResponse.find("\"result\":0") == -1)
	{
		tstring strTmp = CChineseCode::DecodeUTF8(strHttpResponse);
		if (strTmp.find("name") != -1 && strTmp.find("address") != -1 && strTmp.find("detail_address") != -1)
		{
			Json::Reader reader;
			Json::Value root;
			try{
				if (reader.parse(strTmp, root))
				{
					m_strOid = root["id"].asString();
					tstring strName = root["name"].asString();
					tstring strAddress = root["address"].asString();
					tstring strPhone = root["phone"].asString();
					tstring strDeAdd = root["detail_address"].asString();
					tstring strTelePhone = root["telephone"].asString();
					int iProperty = root["cate"].asInt();

					m_pEdtName->SetText(strName.c_str());
					m_pEditCellphone->SetText(strPhone.c_str());

					tstring strProvinceCode = strAddress.substr(0,strAddress.find(":"));
					tstring strAddRight  = strAddress.substr(strAddress.find(",") + 1, strAddress.length());
					CListLabelElementUI *pElement = static_cast<CListLabelElementUI*>(m_PaintManager.FindSubControlByName(m_pCombProvince, strProvinceCode.c_str()));
					if (pElement != NULL )
						m_pCombProvince->SelectItem(pElement->GetTag());

					tstring strCity = strAddRight.substr(0, strAddRight.find(":"));
					strAddRight = strAddRight.substr(strAddRight.find(",") + 1, strAddRight.length());
					CListLabelElementUI *pElementCity = static_cast<CListLabelElementUI*>(m_PaintManager.FindSubControlByName(m_pCombCity, strCity.c_str()));
					if (pElementCity != NULL )
						m_pCombCity->SelectItem(pElementCity->GetTag());

					tstring strDir = strAddRight.substr(0, strAddRight.find(":"));
					CListLabelElementUI *pElementDir = static_cast<CListLabelElementUI*>(m_PaintManager.FindSubControlByName(m_pCombDistrict, strDir.c_str()));
					if (pElementDir != NULL )
						m_pCombDistrict->SelectItem(pElementDir->GetTag());

					m_pEditAddress->SetText(strDeAdd.c_str());

					//m_pEditAddress->SetText(strProperty.c_str());

					//if (strTelePhone.find("-") != -1)
					//{
						//tstring strAre = strTelePhone.substr(0, strTelePhone.find("-"));
						//m_pEditAreacode->SetText(strAre.c_str());
						//strTelePhone = strTelePhone.substr(strTelePhone.find("-") + 1, strTelePhone.length());
						m_pEditTelephone->SetText(strTelePhone.c_str());
					//}

						if(m_pCombProperty != NULL)
						{
							iProperty = iProperty - 1;
							m_pCombProperty->SelectItem(iProperty);
						}

				}
			}catch(...){
				CLogger::GetInstance()->PrintErrLog("parse http response exception: %s", strHttpResponse.c_str());
			}
			//tstring strAddRight = strDeAdd; // 截取地址用

			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}
}

bool CFrameChangeOrganization::getOrgAvatar()
{
	CHttpClient httpClient;
	tstring strUrl(WEB_SERVER_BASE_URL);
	strUrl += _T("org/get-org-avatar?");
	tstring strPost = _T("");
	strPost += _T("oid=");
	tstring strNameDefault = m_strOid;
	strPost += strNameDefault;
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
		tstring strAva;
		Json::Reader reader;
		Json::Value root;
		if (reader.parse(strHttpResponse, root) && strHttpResponse.find("avatar") != -1)
		{
			strAva = root["avatar"].asString();
		}
		else
		{
			return false;
		}
		//tstring strResponse;
		//int res = httpClient.Get(strAva, strResponse);

		DuiLib::CDuiString strAvatar  = CFileHelper::GetHeadCacheDir();
		strAvatar += m_strOid.c_str();
		strAvatar += getTmpFileId().c_str();
		strAva = Base64::decode64(strAva);
		if(CMagicKHelper::Instance()->GeneratePng(strAva, tstring(strAvatar.GetData())))
		{
			tstring strPhotoPath = tstring(strAvatar+_T(".png"));
			m_pBtnSelect->SetBkImage(strPhotoPath.c_str());
		}
		return true;
	}
	
}

bool CFrameChangeOrganization::setOrgInfo(orgInfo &oneinfo)
{
	//oneinfo.strAddress = "110000,110100,110101";
	CHttpClient httpClient;
	tstring strUrl(WEB_SERVER_BASE_URL);
	strUrl += _T("org/modify-org?");
	tstring strPost = _T("");
	strPost += _T("oid=");
	tstring strNameDefault = m_strOid;
	strPost += strNameDefault;
	strPost += _T("&name=");
	strPost += oneinfo.strName;
	strPost += _T("&address=");
	strPost += oneinfo.strAddress;
	strPost += _T("&phone=");
	strPost += oneinfo.strCellPhone;
	strPost += _T("&detail_address=");
	strPost += oneinfo.strDetailAddr;
	strPost += _T("&telephone=");
	strPost += oneinfo.strTelephone;
	strPost += _T("&cate=");
	strPost += oneinfo.strProperty;
 	strPost += _T("&txpass=");//支付密码
 	strPost += "123456";
 	strPost += _T("&txpasscf=");//支付密码
 	strPost += "123456";

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

bool CFrameChangeOrganization::setOrgAvatar(tstring stroid)
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

void CFrameChangeOrganization::onSelectAvatar()
{
	vector<tstring> lstFiles;
	if (getSelectedFiles(lstFiles,"JPG格式图片\0*.jpg\0JPEG格式图片\0\*.jpeg\0BMP格式图片\0*.bmp\0All\0*.*\0"), false);
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
			CMsgBox* pMsgBox = new CMsgBox("提示", "读取文件失败！",  MSGBOX_ICON_WARNING);
			pMsgBox->ShowModal(this->GetHWND());
			return;

		}
		

		//	//2. 
		CMagicKHelper::Instance()->GetPhotoBinval(strtmp, m_strAvatar);
		//3.
		m_bSelectedAvatar = true;
	}
}

void CFrameChangeOrganization::saveOrgInfo()
{
	orgInfo orginfo;
	orginfo.strPhotoBinary = m_strAvatar;
	orginfo.strPhotoType = "image/jpeg";

	if (m_pEdtName->GetText().GetLength() < 1)
	{
		CMsgBox* pMsgBox = new CMsgBox("提示", "企业名称不能为空！",  MSGBOX_ICON_WARNING);
		pMsgBox->ShowModal(this->GetHWND());
		return;
	}
	else
	{
		
		orginfo.strName = m_pEdtName->GetText();
		if (!CRegexHelper::IsValidOrgName(orginfo.strName))
		{
			CMsgBox* pMsgBox = new CMsgBox("提示", "企业名称不能包含非法字符！",  MSGBOX_ICON_WARNING);
			pMsgBox->ShowModal(this->GetHWND());
			return;
		}
			
	}
	if (m_pCombProvince->GetText().GetLength()<1)
	{
		CMsgBox* pMsgBox = new CMsgBox("提示", "请选择省！",  MSGBOX_ICON_WARNING);
		pMsgBox->ShowModal(this->GetHWND());
		return;
	}
	else
	{
		if (m_pCombCity->GetText().GetLength()<1)
		{
			CMsgBox* pMsgBox = new CMsgBox("提示", "请选择市（区）！",  MSGBOX_ICON_WARNING);
			pMsgBox->ShowModal(this->GetHWND());
			return;
		}
		else
		{
			CDuiString strProvinceName = m_pCombProvince->GetText();
			if (m_pCombDistrict->GetText().GetLength()<1 && !IsMunicipalities(strProvinceName))
			{
				CMsgBox* pMsgBox = new CMsgBox("提示", "请选择区（县）！",  MSGBOX_ICON_WARNING);
				pMsgBox->ShowModal(this->GetHWND());
				return;
			}
			else
			{
				//tstring strProvinceCode = strAddress.substr(0,strAddress.find(","));
				//tstring strAddRight  = strAddress.substr(strAddress.find(",") + 1, strAddress.length());
				//CListLabelElementUI *pElement = static_cast<CListLabelElementUI*>(m_PaintManager.FindSubControlByName(m_pCombProvince, strProvinceCode.c_str()));
				tstring strPro = m_pCombProvince->GetItemAt(m_pCombProvince->GetCurSel())->GetName();
				tstring strCity = m_pCombCity->GetItemAt(m_pCombCity->GetCurSel())->GetName();
				tstring strDis = m_pCombDistrict->GetItemAt(m_pCombDistrict->GetCurSel())->GetName();

				orginfo.strAddress = strPro + "," + strCity + "," + strDis;

				orginfo.strDetailAddr = m_pEditAddress->GetText();

				//orginfo.strProperty = m_pEditProperty->GetText();
			}
		}
	}
	orginfo.strTelephone = m_pEditTelephone->GetText();
	orginfo.strTelephone = m_pEditTelephone->GetText();
	orginfo.strCellPhone = m_pEditCellphone->GetText();
	orginfo.strProperty = m_stProperty;
	//准备工作全部ok
	//1. 设置org
	if (!setOrgInfo(orginfo))
	{
		CMsgBox* pMsgBox = new CMsgBox("提示", "修改机构信息失败！",  MSGBOX_ICON_WARNING);
		pMsgBox->ShowModal(this->GetHWND());
		return;
	}
	//2. 再根据上述信息设置org   设置头像
	if (m_bSelectedAvatar)
	{
		m_bSelectedAvatar = false;
		if (!setOrgAvatar(m_strOid))
		{
			CMsgBox* pMsgBox = new CMsgBox("提示", "修改机构头像失败！",  MSGBOX_ICON_WARNING);
			pMsgBox->ShowModal(this->GetHWND());
		}
	}
	
	CMsgBox* pMsgBox = new CMsgBox("提示", "修改机构信息成功！",  MSGBOX_ICON_WARNING);
	pMsgBox->ShowModal(this->GetHWND());
	//clearFrame();
	mainDlg::getInstance()->PostMessage(WM_MODIFY_ORG_INFO);
	ShowWindow(false);
}

void CFrameChangeOrganization::initPropertyComb()
{
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
			m_pCombProperty->SelectItem(2);  //默认选中经销商
		}
	}
}





