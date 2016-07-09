#include "FrameGuideOrg.h"
#include "mainDlg.h"
#include "MsgBox.h"
#include "StringHelper.h"
#include "Controller.h"
#include "FileHelper.h"
#include "../Common/Logger.h"
#include "webClient.h"
#include <sstream>

const CDuiString municipalities[] = {"北京","天津","上海","重庆" };
CFrameGuideOrg* CFrameGuideOrg::m_pInstance = NULL;
CFrameGuideOrg::CFrameGuideOrg()
{
}


CFrameGuideOrg::~CFrameGuideOrg(void)
{
}

void CFrameGuideOrg::HandleSetFocusEvent( TNotifyUI& msg )
{

}

void CFrameGuideOrg::HandleKillFocusEvent( TNotifyUI& msg )
{

}

void CFrameGuideOrg::HandleTextChangedEvent( TNotifyUI& msg )
{

}

void CFrameGuideOrg::HandleClickEvent( TNotifyUI& msg )
{
	if (_tcsicmp(msg.pSender->GetName(), _T("btn_close")) == 0)
	{
		clearFrame();
		ShowWindow(false);
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
	else if (m_pCombProperty == msg.pSender)
	{
		//tstring::number()
		int iSel = m_pCombProperty->GetCurSel() +1;
		stringstream ss;
		ss<<iSel;
		m_stProperty = ss.str();
	}
}

void CFrameGuideOrg::HandleItemSelectEvent( TNotifyUI& msg )
{
	if (m_pCombProvince == msg.pSender)
	{
		CDuiString strProvinceName = m_pCombProvince->GetText();
		//if (IsMunicipalities(strProvinceName))
		//{
			//todo
			//m_pCombDistrict->SetText("");
			//m_pCombDistrict->SetEnabled(false);
		//}
		//else 
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
		//tstring::number()
		int iSel = m_pCombProperty->GetCurSel() +1;
		stringstream ss;
		ss<<iSel;
		m_stProperty = ss.str();
	}
}

DuiLib::CDuiString CFrameGuideOrg::GetSkinFile()
{
	return _T("guide_org.xml");
}

CControlUI* CFrameGuideOrg::CreateControl( LPCTSTR pstrClass )
{
	return NULL;
}

void CFrameGuideOrg::InitWindow()
{
	m_pEdtName = static_cast<CEditUI*>(m_PaintManager.FindControl(_T("edit_org_name")));
	m_pCombProvince = static_cast<CComboUI*>(m_PaintManager.FindControl(_T("CombProvince")));
	m_pCombCity = static_cast<CComboUI*>(m_PaintManager.FindControl(_T("CombCity")));
	m_pCombDistrict = static_cast<CComboUI*>(m_PaintManager.FindControl(_T("CombDistrict")));
	m_pEditCellphone = static_cast<CEditUI*>(m_PaintManager.FindControl(_T("edit_cellphone")));
	m_pEditAreacode = static_cast<CEditUI*>(m_PaintManager.FindControl(_T("edit_area_code")));
	m_pEditTelephone = static_cast<CEditUI*>(m_PaintManager.FindControl(_T("edit_phone")));
	m_pEditAddress = static_cast<CEditUI*>(m_PaintManager.FindControl(_T("edit_address")));
	initPropertyComb();
	ParseLocInfo();
	initProvinceComb();
	//getorg();
	//deleteorg();
}

CFrameGuideOrg* CFrameGuideOrg::Instance()
{

	if(NULL == m_pInstance)
	{
		m_pInstance = new CFrameGuideOrg;
		m_pInstance->CreateWnd();
	}
	return m_pInstance;
}

void CFrameGuideOrg::clearFrame()
{
	m_pEditAddress->SetText("");
	//m_pEditAreacode->SetText("");
	m_pEditCellphone->SetText("");
	m_pEditTelephone->SetText("");
	m_pEdtName->SetText("");
}

bool CFrameGuideOrg::createOrg()
{
	orgInfo orginfo;
	

	if (m_pEdtName->GetText().GetLength() < 1)
	{
		CMsgBox* pMsgBox = new CMsgBox("提示", "企业名称不能为空！",  MSGBOX_ICON_WARNING);
		pMsgBox->ShowModal(this->GetHWND());
		return false;
	}
	else
		orginfo.strName = m_pEdtName->GetText();
	if (m_pCombProvince->GetText().GetLength()<1)
	{
		CMsgBox* pMsgBox = new CMsgBox("提示", "请选择省！",  MSGBOX_ICON_WARNING);
		pMsgBox->ShowModal(this->GetHWND());
		return false;
	}
	else
	{
		if (m_pCombCity->GetText().GetLength()<1)
		{
			CMsgBox* pMsgBox = new CMsgBox("提示", "请选择市（区）！",  MSGBOX_ICON_WARNING);
			pMsgBox->ShowModal(this->GetHWND());
			return false;
		}
		else
		{
			CDuiString strProvinceName = m_pCombProvince->GetText();
			if (m_pCombDistrict->GetText().GetLength()<1 && !IsMunicipalities(strProvinceName))
			{
				CMsgBox* pMsgBox = new CMsgBox("提示", "请选择区（县）！",  MSGBOX_ICON_WARNING);
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
	//准备工作全部ok
	//1. 先创建一个org
	if (!apiCreateOrg(orginfo))
	{
		return false;
	}
	//2. 再根据上述信息设置org   设置头像
	CMsgBox* pMsgBox = new CMsgBox("提示", "创建机构成功！",  MSGBOX_ICON_WARNING);
	pMsgBox->ShowModal(this->GetHWND());
	clearFrame();
	ShowWindow(false);
	mainDlg::getInstance()->setUserIdentity(USER_ORG_BOSS);
	//mainDlg::getInstance()->PostMessage(WM_CREATE_ORG);
	return true;
}

bool CFrameGuideOrg::apiCreateOrg( orgInfo & info)
{
	CHttpClient httpClient;
	tstring strUrl(WEB_SERVER_BASE_URL);
	strUrl += _T("org/create-org?");
	tstring strPost = _T("");
	strPost += _T("name=");
	tstring strNameDefault = info.strName;
	strPost += strNameDefault;
	strPost += _T("&address=");
	strPost += info.strAddress;
	strPost += _T("&phone=");
	strPost += info.strCellPhone;
	strPost += _T("&detail_address=");
	strPost += info.strDetailAddr;
	strPost += _T("&telephone=");
	strPost += info.strTelephone;
	strPost += _T("&cate=");//新加的物流参数，暂时定为1
	strPost += info.strProperty;
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
		if (strHttpResponse.find("the user has org already") != -1)
		{
			CMsgBox* pMsgBox = new CMsgBox("提示", "用户已有机构，无法再次创建！",  MSGBOX_ICON_WARNING);
			pMsgBox->ShowModal(this->GetHWND());

		}
		else if (strHttpResponse.find("the name is existed already") != -1)
		{
			CMsgBox* pMsgBox = new CMsgBox("提示", "重复机构名，创建失败！",  MSGBOX_ICON_WARNING);
			pMsgBox->ShowModal(this->GetHWND());
		}
		else
		{
			CMsgBox* pMsgBox = new CMsgBox("提示", "创建机构失败！",  MSGBOX_ICON_WARNING);
			pMsgBox->ShowModal(this->GetHWND());
		}
		m_strOid = "-1";
		return false;
	}
	else
	{
		m_strOid = strHttpResponse.substr(22, strHttpResponse.find("\"}") - 22);
		return true;
	}
}

bool CFrameGuideOrg::IsMunicipalities( CDuiString strProvinceName )
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

bool CFrameGuideOrg::ParseLocInfo()
{
	tstring strRegions = CWebClient::getRegions();
	strRegions = CChineseCode::DecodeUTF8(strRegions);
	Json::Reader reader;
	return reader.parse(strRegions,m_valLoc);
}

void CFrameGuideOrg::initProvinceComb()
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

bool CFrameGuideOrg::deleteorg()
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

bool CFrameGuideOrg::getorg()
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

void CFrameGuideOrg::initCityComb( CDuiString strProvinceName )
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

void CFrameGuideOrg::initDistrictComb( CDuiString strCityName )
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

void CFrameGuideOrg::initPropertyComb()
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
			m_pCombProperty->SelectItem(2);  //默认选中经销商
		}
	}
}
