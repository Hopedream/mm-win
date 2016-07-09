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
	tstring m_strOid;                               //��ǰ�û�������֯ oid;
	CButtonUI *m_pBtnSelect;                       //ͷ��ť
	CEditUI   *m_pEdtName;                         //����
	CComboUI *m_pCombProvince;                     //��ַ-ʡ
	CComboUI *m_pCombCity;                         //��ַ-��
	CComboUI *m_pCombDistrict;                     //��ַ-��
	CComboUI *m_pCombProperty;                     //��������
	CEditUI *m_pEditCellphone;                     //�ֻ�
	CEditUI *m_pEditAreacode;                      //�绰-����
	CEditUI *m_pEditTelephone;                     //�绰-�绰����
	CEditUI *m_pEditAddress;                       //��ַ-��ϸ
	Json::Value m_valLoc;
	Json::Value m_valProvince;
	bool m_bSelectedAvatar;                        // �Ƿ�ѡ����ͷ��     
	tstring m_stProperty;                           // ѡ��Ļ�������
};

