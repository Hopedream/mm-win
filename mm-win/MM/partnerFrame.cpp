#include "StdAfx.h"
#include "partnerFrame.h"

CPartnerFrame::CPartnerFrame(CPaintManagerUI* lpPaintMgr):m_pPaintMrg(lpPaintMgr)
{
		CDialogBuilder builder;
		CContainerUI* pPartnerFrm = static_cast<CContainerUI*>(builder.Create(_T("partner\\partnerSkinUI.xml"), (UINT)0));
		if( pPartnerFrm ) {
			this->Add(pPartnerFrm);
			m_lpThis = static_cast<CPartnerFrame*>(pPartnerFrm);
		}
		else {
			this->RemoveAll();
			return;
		}
}

