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
	tstring m_strOid;                               //��ǰ�û�������֯ oid;
	CEditUI   *m_pEdtName;                         //����
	CComboUI *m_pCombProvince;                     //��ַ-ʡ
	CComboUI *m_pCombCity;                         //��ַ-��
	CComboUI *m_pCombDistrict;                     //��ַ-��
	CEditUI *m_pEditCellphone;                     //�ֻ�
	CEditUI *m_pEditAreacode;                      //�绰-����
	CEditUI *m_pEditTelephone;                     //�绰-�绰����
	CComboUI *m_pCombProperty;                     //��������
	CEditUI *m_pEditAddress;                       //��ַ-��ϸ

	Json::Value m_valLoc;
	Json::Value m_valProvince;

	tstring m_stProperty;                           // ѡ��Ļ�������
};

