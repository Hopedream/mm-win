#include "FrameBussinessData.h"
#include "mainDlg.h"
#include "webClient.h"
#include "..\json\json.h"
#include "..\Common\Logger.h"
#include "StringHelper.h"


CFrameBussinessData* CFrameBussinessData::m_pInstance = NULL;
CFrameBussinessData::CFrameBussinessData(void)
{
}


CFrameBussinessData::~CFrameBussinessData(void)
{
}

CFrameBussinessData* CFrameBussinessData::Instance()
{
	if(NULL == m_pInstance)
	{
		m_pInstance = new CFrameBussinessData;
		m_pInstance->CreateWnd(mainDlg::getInstance()->GetHWND());
	}
	return m_pInstance;
}

void CFrameBussinessData::HandleClickEvent( TNotifyUI& msg )
{
	 if (_tcsicmp(msg.pSender->GetName(), _T("buttonClose")) == 0)
	{
		if (m_date_Recv_begin != NULL && m_date_Recv_end != NULL)
		{
			m_date_Recv_begin->SetText(_T(""));
			m_date_Recv_end->SetText(_T(""));
		}
		
		ShowWindow(false);
	}
	 else if (_tcsicmp(msg.pSender->GetName(), _T("buttonFind")) == 0)
	 {
		 onButtonFind();
	 }
}

void CFrameBussinessData::HandleSelectedChangedEvent( TNotifyUI& msg )
{

}

void CFrameBussinessData::HandleItemSelectEvent( TNotifyUI& msg )
{

}

DuiLib::CDuiString CFrameBussinessData::GetSkinFile()
{
	return _T("view_bussiness_data.xml");
}

CControlUI* CFrameBussinessData::CreateControl( LPCTSTR pstrClass )
{
	return NULL;
}

void CFrameBussinessData::InitWindow()
{
	m_plstBussData = static_cast<CListUI*>(m_PaintManager.FindControl(_T("lstEmployee")));
	m_pedit_fileName = static_cast<CEditUI*>(m_PaintManager.FindControl(_T("edit_fileName")));
	m_pedit_Bid = static_cast<CEditUI*>(m_PaintManager.FindControl(_T("edit_Bid")));
	m_pedit_SendOrg = static_cast<CEditUI*>(m_PaintManager.FindControl(_T("edit_SendOrg")));
	m_pedit_RecvOrg = static_cast<CEditUI*>(m_PaintManager.FindControl(_T("edit_RecvOrg")));
	m_pedit_Sender = static_cast<CEditUI*>(m_PaintManager.FindControl(_T("edit_Sender")));
	m_date_Send_begin = static_cast<CDateTimeUI*>(m_PaintManager.FindControl(_T("date_Send_begin")));
	m_date_Send_end = static_cast<CDateTimeUI*>(m_PaintManager.FindControl(_T("date_Send_end")));
	m_date_Recv_begin = static_cast<CDateTimeUI*>(m_PaintManager.FindControl(_T("date_Recv_begin")));
	m_date_Recv_end = static_cast<CDateTimeUI*>(m_PaintManager.FindControl(_T("date_Recv_end")));
	//
}

void CFrameBussinessData::onButtonFind()
{
	if (m_plstBussData != NULL)
		m_plstBussData->RemoveAll();
	tstring strFileName=(_T(""));
	if (m_pedit_fileName != NULL)
	{
		strFileName = m_pedit_fileName->GetText();
	}

	tstring strBid=(_T(""));
	if (m_pedit_Bid != NULL)
	{
		strBid = m_pedit_Bid->GetText();
	}

	tstring strSendOrg=(_T(""));
	if (m_pedit_SendOrg != NULL)
	{
		strSendOrg = m_pedit_SendOrg->GetText();
	}

	tstring strRecvOrg=(_T(""));
	if (m_pedit_RecvOrg != NULL)
	{
		strRecvOrg = m_pedit_RecvOrg->GetText();
	}

	tstring strSender=(_T(""));
	if (m_pedit_Sender != NULL)
	{
		strSender = m_pedit_Sender->GetText();
	}

	tstring strSendTime=(_T(""));
	if (m_date_Send_begin != NULL)
	{
		strSendTime = m_date_Send_begin->GetText();
	}

	tstring strSendTimeEnd=(_T(""));
	if (m_date_Send_end != NULL)
	{
		strSendTimeEnd = m_date_Send_end->GetText();
	}

	tstring strRecvTime=(_T(""));
	if (m_date_Recv_begin != NULL)
	{
		strRecvTime = m_date_Recv_begin->GetText();
	}

	tstring strRecvTimeEnd=(_T(""));
	if (m_date_Recv_end != NULL)
	{
		strRecvTimeEnd = m_date_Recv_end->GetText();
	}

	tstring strRes = CWebClient::getFindFiles(strFileName, strBid, strSender, 
		strSendTime, strSendTimeEnd, strRecvOrg, strRecvTime, strRecvTimeEnd);

	strRes = CChineseCode::DecodeUTF8(strRes);

	Json::Reader reader;
	Json::Value root;
	try
	{
		if (reader.parse(strRes, root))
		{
			int nPackCount = root.size();

			for (int  i=0;  i<nPackCount;  i++)
			{
				busFileInfo bsInfo;
				bsInfo.strFileName = root[i]["file_name"].asString();
				bsInfo.strBid = root[i]["bid"].asString();
				bsInfo.strRecvOrg = root[i]["receiver_oname"].asString();
				bsInfo.strRecvTime = root[i]["receive_time"].asString();
				bsInfo.strSender = root[i]["sender_nick"].asString();
				bsInfo.strSendOrg = root[i]["sender_oname"].asString();
				bsInfo.strSendTime = root[i]["send_time"].asString();
				AddItem(bsInfo);
			}
		}
	}
	catch (...)
	{
		CLogger::GetInstance()->PrintErrLog( "parse http Response exception: %s", strRes.c_str());	
	}	

}

bool CFrameBussinessData::AddItem( busFileInfo memInfo )
{
	//return 0;
	CDialogBuilder DlgBuilder;
	CListContainerElementUI *plstItem = NULL;
	if (!DlgBuilder.GetMarkup()->IsValid())
	{
		plstItem = static_cast<CListContainerElementUI*>(DlgBuilder.Create(_T("view_bussiness_item.xml"),
			(UINT)0, NULL, &m_PaintManager));
	}
	else
	{
		plstItem = static_cast<CListContainerElementUI*>(DlgBuilder.Create((UINT)0, &m_PaintManager));
	}
	if (plstItem == NULL)
		return false;
	else
	{
		CLabelUI *labelFileName = static_cast<CLabelUI *>(m_PaintManager.FindSubControlByName(plstItem, _T("labelFileName")));
		if (labelFileName!= NULL)
			labelFileName->SetText(memInfo.strFileName.c_str());
		CLabelUI *labelBid = static_cast<CLabelUI *>(m_PaintManager.FindSubControlByName(plstItem, _T("labelBid")));
		if (labelBid != NULL)
		{
			labelBid->SetText(memInfo.strBid.c_str());
		}

		CLabelUI *labelSendOrg = static_cast<CLabelUI *>(m_PaintManager.FindSubControlByName(plstItem, _T("labelSendOrg")));
		if (labelSendOrg != NULL)
		{
			labelSendOrg->SetText(memInfo.strSendOrg.c_str());
		}
		CLabelUI *labelSender = static_cast<CLabelUI *>(m_PaintManager.FindSubControlByName(plstItem, _T("labelSender")));
		if (labelBid != NULL)
		{
			labelSender->SetText(memInfo.strSender.c_str());
		}
		CLabelUI *labelSendTime = static_cast<CLabelUI *>(m_PaintManager.FindSubControlByName(plstItem, _T("labelSendTime")));
		if (labelBid != NULL)
		{
			labelSendTime->SetText(memInfo.strSendTime.c_str());
		}
		CLabelUI *labelRecvOrg = static_cast<CLabelUI *>(m_PaintManager.FindSubControlByName(plstItem, _T("labelRecvOrg")));
		if (labelBid != NULL)
		{
			labelRecvOrg->SetText(memInfo.strRecvOrg.c_str());
		}
		CLabelUI *labelRecvTime = static_cast<CLabelUI *>(m_PaintManager.FindSubControlByName(plstItem, _T("labelRecvTime")));
		if (labelBid != NULL)
		{
			labelRecvTime->SetText(memInfo.strRecvTime.c_str());
		}
		

		plstItem->SetFixedHeight(32);
		if (!m_plstBussData->Add(plstItem))
		{
			delete plstItem;
			return false;
		}
		return true;
	}
}
