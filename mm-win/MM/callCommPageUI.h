#pragma once

class CCallCommPageUI : public CContainerUI
{
public:
	CCallCommPageUI(CPaintManagerUI& paint_manager);	
	void SetPhone(CDuiString strPhone);
private:
	CPaintManagerUI& m_paintMrg;
	CCallCommPageUI* m_lpThis;
};