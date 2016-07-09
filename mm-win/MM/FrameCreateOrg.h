#pragma once
#include "mmwinimplbase.h"
#include "UIDefine.h"
#include "json/json.h"
class CFrameCreateOrg :
	public MMWindowImplBase
{
public:
	CFrameCreateOrg();
	~CFrameCreateOrg(void);
	static CFrameCreateOrg* Instance();
public:
	virtual void HandleSetFocusEvent(TNotifyUI& msg);
	virtual void HandleKillFocusEvent(TNotifyUI& msg);
	virtual void HandleTextChangedEvent(TNotifyUI& msg);
	virtual void HandleClickEvent(TNotifyUI& msg);
	virtual void HandleItemSelectEvent(TNotifyUI& msg);
	virtual CDuiString GetSkinFile();
	virtual CControlUI* CreateControl(LPCTSTR pstrClass);
	virtual void InitWindow();
private:
	void clearFrame();
	void onSelectAvatar();
	bool createOrg();
	bool apiCreateOrg(orgInfo &);
	bool apisaveAvatar(tstring stroid);
	bool IsMunicipalities(CDuiString strProvinceName);
	void initProvinceComb();
	void initPropertyComb();
	void initCityComb(CDuiString strProvinceName);
	void initDistrictComb(CDuiString strCityName);
	bool ParseLocInfo();
	bool deleteorg();
	bool getorg();
private:
	static CFrameCreateOrg* m_pInstance;
	tstring m_strAvatar;
	tstring m_strAvatarType;
	tstring m_strOid;                               //当前用户所在组织 oid;
	CButtonUI *m_pBtnSelect;                       //头像按钮
	CEditUI   *m_pEdtName;                         //名称
	CComboUI *m_pCombProvince;                     //地址-省
	CComboUI *m_pCombCity;                         //地址-市
	CComboUI *m_pCombDistrict;                     //地址-区
	CComboUI *m_pCombProperty;                     //机构属性
	CEditUI *m_pEditCellphone;                     //手机
	CEditUI *m_pEditAreacode;                      //电话-区号
	CEditUI *m_pEditTelephone;                     //电话-电话号码
	CEditUI *m_pEditAddress;                       //地址-详细
	Json::Value m_valLoc;
	Json::Value m_valProvince;
	bool m_bSelectedAvatar;                        // 是否选择了头像     
	tstring m_stProperty;                           // 选择的机构属性
};

