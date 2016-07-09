#include "StdAfx.h"
#include "callCommPageUI.h"

CCallCommPageUI::CCallCommPageUI(CPaintManagerUI& paint_manager):m_paintMrg(paint_manager)
{
		CDialogBuilder builder;
		CContainerUI* pComputerExamine = static_cast<CContainerUI*>(builder.Create(_T("Call\\callCommUI.xml"), (UINT)0));
		if( pComputerExamine ) {
			this->Add(pComputerExamine);
			m_lpThis = static_cast<CCallCommPageUI*>(pComputerExamine);
		}
		else {
			this->RemoveAll();
			return;
		}
}

void CCallCommPageUI::SetPhone( CDuiString strPhone )
{
	CEditUI* phoneEdit = static_cast<CEditUI*>(m_paintMrg.FindSubControlByName(m_lpThis, "phoneNum"));
	phoneEdit->SetText(strPhone);
}

