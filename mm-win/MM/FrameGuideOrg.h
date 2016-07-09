#pragma once
#include "mmwinimplbase.h"
#include "UIDefine.h"
#include "json/json.h"

class CFrameGuideOrg :
	public MMWindowImplBase
{
public:
	CFrameGuideOrg();
	~CFrameGuideOrg(void);
	static CFrameGuideOrg* Instance();

public:
	virtual void HandleSetFocusEvent(TNotifyUI& msg);
	virtual void HandleKillFocusEvent(TNotifyUI& msg);
	virtual void HandleTextChangedEvent(TNotifyUI& msg);
	virtual void HandleClickEvent(TNotifyUI& msg);
	virtual void HandleItemSelectEvent(TNotifyUI& msg);
	virtual CDuiString GetSkinFile();
	virtual CControlUI* CreateControl(LPCTSTR pstrClass);
	virtual void InitWindow();

public:
	static CFrameGuideOrg* m_pInstance;

private:
	void clearFrame();
	bool createOrg();
	bool apiCreateOrg(orgInfo &);
	bool IsMunicipalities(CDuiString strProvinceName);
	bool ParseLocInfo();
	void initProvinceComb();
	void initPropertyComb();
	void initCityComb(CDuiString strProvinceName);
	void initDistrictComb(CDuiString strCityName);
	bool deleteorg();
	bool getorg();
private:
	tstring m_strOid;                               //当前用户所在组织 oid;
	CEditUI   *m_pEdtName;                         //名称
	CComboUI *m_pCombProvince;                     //地址-省
	CComboUI *m_pCombCity;                         //地址-市
	CComboUI *m_pCombDistrict;                     //地址-区
	CEditUI *m_pEditCellphone;                     //手机
	CEditUI *m_pEditAreacode;                      //电话-区号
	CEditUI *m_pEditTelephone;                     //电话-电话号码
	CComboUI *m_pCombProperty;                     //机构属性
	CEditUI *m_pEditAddress;                       //地址-详细

	Json::Value m_valLoc;
	Json::Value m_valProvince;

	tstring m_stProperty;                           // 选择的机构属性
};

