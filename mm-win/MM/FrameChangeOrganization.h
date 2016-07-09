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
	//�ؼ�
	//CLabelUI *m_plblTips;
	tstring m_strAvatar;
	tstring m_strOid;
	CButtonUI *m_pBtnSelect;                       //ͷ��ť
	CEditUI   *m_pEdtName;                         //����
	CComboUI *m_pCombProvince;                     //��ַ-ʡ
	CComboUI *m_pCombCity;                         //��ַ-��
	CComboUI *m_pCombDistrict;                     //��ַ-��
	CEditUI *m_pEditCellphone;                     //�ֻ�
	CEditUI *m_pEditAreacode;                      //�绰-����
	CEditUI *m_pEditTelephone;                     //�绰-�绰����
	CEditUI *m_pEditAddress;                       //��ַ-��ϸ
	CComboUI *m_pCombProperty;					   //����
	static CFrameChangeOrganization* m_pInstance;
	//sUserInfo m_myUserInfo;
	orgInfo m_orgInfo;
	sUserVCard m_myUserInfo;

	Json::Value m_valLoc;
	Json::Value m_valProvince;

	bool m_bSelectedAvatar;                        // �Ƿ�ѡ����ͷ��    
	tstring m_stProperty;
};

