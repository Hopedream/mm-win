#pragma once
#include "mmwinimplbase.h"
#include "../include/UsrManagerEvent_i.h"
#include "UserManager.h"
#include "json/json.h"
#include "UIDefine.h"

class CFrameChangeOrganization :
	public MMWindowImplBase, public CUserManagerEventBase
{
public:
	static CFrameChangeOrganization* Instance();
	CFrameChangeOrganization();
	~CFrameChangeOrganization(void);
public:
	virtual void HandleSetFocusEvent(TNotifyUI& msg);
	virtual void HandleKillFocusEvent(TNotifyUI& msg);
	virtual void HandleTextChangedEvent(TNotifyUI& msg);
	virtual void HandleClickEvent(TNotifyUI& msg);
	virtual void HandleItemSelectEvent(TNotifyUI& msg);
	virtual CDuiString GetSkinFile();
	virtual CControlUI* CreateControl(LPCTSTR pstrClass);
	virtual void InitWindow();

	virtual void OnMySelfCardInfo(sUserVCard& oneInfo);
	virtual void OnStoreVCardResult(tstring& strAccount, bool bSucc);
	void showFocus();
private:
	void updateWindow();
	bool ParseLocInfo();
	void initProvinceComb();
	void initVocationComb();
		void initPropertyComb();
	void initCityComb(CDuiString strProvinceName);
	void initDistrictComb(CDuiString strCityName);
	bool IsMunicipalities(CDuiString strProvinceName);
	void clearwindow();
	bool getOrgInfo();
	bool getOrgAvatar();
	bool setOrgInfo(orgInfo &oneinfo);
	void saveOrgInfo();
	bool setOrgAvatar(tstring stroid);
	void onSelectAvatar();
public:
private:
	//控件
	//CLabelUI *m_plblTips;
	tstring m_strAvatar;
	tstring m_strOid;
	CButtonUI *m_pBtnSelect;                       //头像按钮
	CEditUI   *m_pEdtName;                         //名称
	CComboUI *m_pCombProvince;                     //地址-省
	CComboUI *m_pCombCity;                         //地址-市
	CComboUI *m_pCombDistrict;                     //地址-区
	CEditUI *m_pEditCellphone;                     //手机
	CEditUI *m_pEditAreacode;                      //电话-区号
	CEditUI *m_pEditTelephone;                     //电话-电话号码
	CEditUI *m_pEditAddress;                       //地址-详细
	CComboUI *m_pCombProperty;					   //属性
	static CFrameChangeOrganization* m_pInstance;
	//sUserInfo m_myUserInfo;
	orgInfo m_orgInfo;
	sUserVCard m_myUserInfo;

	Json::Value m_valLoc;
	Json::Value m_valProvince;

	bool m_bSelectedAvatar;                        // 是否选择了头像    
	tstring m_stProperty;
};

