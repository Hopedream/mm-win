#pragma once

class CPartnerFrame : public CContainerUI
{
public:
	CPartnerFrame(CPaintManagerUI* lpPaintMgr);	
protected:
private:
	CPaintManagerUI* m_pPaintMrg;
	CPartnerFrame* m_lpThis;
};