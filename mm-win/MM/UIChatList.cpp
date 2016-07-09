#include "UIChatList.h"
#include <ctime>
#include <sstream>
#include "mainDlg.h"
#include "HttpClient.h"
#include "Controller.h"
#include "..\include\DBTaskModule.h"
#include <atlstr.h>
#include "..\json\json.h"
#include "..\Common\regexHelper.h"
#include "..\Common\Logger.h"
#include "StringHelper.h"
#include "UIDefine.h"
#include "Utility.h"


const int sn_Max_ListItem_Width = 280 + 0;	//一行消息内容的最大长度
const int sn_YPos_MyChatText	= 360;	//我的消息气泡的最右边Y的坐标值
const int sn_FIVE_MINITUS		= 300;	//5分钟的秒数
const int sn_Init_Row_Height	= 33;	//默认初始化聊天记录的高度
const int sn_Offset_Row_Height	= 23;	//默认一行聊天记录的高度,12Pixel 雅黑字体的高度值
const int sn_Offset_Top			= 8;	//聊天内容的顶部偏移值
const int sn_Offset_Left		= 15;	//聊天内容的左边偏移值

namespace DuiLib
{
	const TCHAR* const kcontrolLogControName = _T("controlLogo");	//聊天记录的头像控件
	const TCHAR* const klabelTextControlName = _T("labelText");		//聊天内容文本控件


	DuiLib::CChatListUI::CChatListUI( CPaintManagerUI *paint_manager )
	{
		m_pPaintManager = paint_manager;
		m_curTime = 0;
		m_iTag = 0;
	}


	DuiLib::CChatListUI::~CChatListUI()
	{
		
	}

	bool DuiLib::CChatListUI::AddOppositeMsgItem(ChatInfo* pchatInfo)
	{
		if (pchatInfo->uTime - m_curTime >= sn_FIVE_MINITUS)               //时间超过5分钟
		{
			//增加一个时间节点。
			struct tm * now = localtime(&pchatInfo->uTime);
			char chBuf[50];
			memset(chBuf, 0, 50);
			sprintf(chBuf, "%02u:%02u", now->tm_hour, now->tm_min);
			CDuiString strTm = chBuf;
			AddTimeItem(strTm);
		}
		//接收文件提示
		if (pchatInfo->m_strRawMsg.Find("#COMMAND#type=filedownloaded") != -1)
		{
			tstring sSignal = "#COMMAND#type=filedownloaded,";
			tstring sRawMsg =  pchatInfo->m_strRawMsg.GetData();
			tstring sJson = sRawMsg.substr(sSignal.length());
			tstring sFileName;

			Json::Reader reader;
			Json::Value root;
			try
			{
				if (reader.parse(sJson, root))
				{
					sFileName = root["filename"].asString();
				}
			}
			catch (...)
			{
				CLogger::GetInstance()->PrintErrLog( "parse filedownload receipt exception: %s", sJson.c_str());	
			}	
	
			CDuiString sReply;
			sReply.Format("%s%s","对方接收了您发送的文件",sFileName.c_str());
			AddTimeItem(sReply, 10);//借用下
			return true;
		}

		//拒收文件提示
		if (pchatInfo->m_strRawMsg.Find("#COMMAND#type=filerejected") != -1)
		{
			tstring sSignal = "#COMMAND#type=filerejected,";
			tstring sRawMsg =  pchatInfo->m_strRawMsg.GetData();
			tstring sJson = sRawMsg.substr(sSignal.length());
			tstring sFileName;

			Json::Reader reader;
			Json::Value root;
			try
			{
				if (reader.parse(sJson, root))
				{
					sFileName = root["filename"].asString();
				}
			}
			catch (...)
			{
				CLogger::GetInstance()->PrintErrLog( "parse filedownload receipt exception: %s", sJson.c_str());	
			}	

			CDuiString sReply;
			sReply.Format("%s%s","对方拒收了您发送的文件",sFileName.c_str());
			AddTimeItem(sReply, 10);//借用下
			return true;
		}
		m_curTime = pchatInfo->uTime;
		
		CDialogBuilder DlgBuilder;
		CListContainerElementUI *plstItem = NULL;
		if (!DlgBuilder.GetMarkup()->IsValid())
		{
			plstItem = static_cast<CListContainerElementUI*>(DlgBuilder.Create(_T("chat_opposite_item.xml"),
				(UINT)0, NULL, m_pPaintManager));
		}
		else
		{
			plstItem = static_cast<CListContainerElementUI*>(DlgBuilder.Create((UINT)0, m_pPaintManager));
		}
		if (plstItem == NULL)
			return false;
		CControlUI *pControlLogo = m_pPaintManager->FindSubControlByName(plstItem, kcontrolLogControName);
		if (pControlLogo != NULL)
		{
			pControlLogo->SetBkImage(pchatInfo->m_strLogo);
		}

		//计算文本的的高度
		int nTextTotalHeight = 0;
		CRichEditUI *plabelText = static_cast<CRichEditUI*>(m_pPaintManager->FindSubControlByName(plstItem, klabelTextControlName));
		//plabelText->SetTextStyle(16);
		if (plabelText != NULL)
		{
			//设置richedit 风格  todo
			/*PARAFORMAT2 pf;
			DWORD res = plabelText->GetParaFormat(pf);
			pf.dyLineSpacing = 100;
			bool bsuss = plabelText->SetParaFormat(pf);*/

			CDuiString strOutput;
			int nMaxTextWidth = 0;
			if (pchatInfo->m_lstMsg.size() == 1)
			{
				std::vector<CDuiString> lstMsg;
				tstring strValue = pchatInfo->m_lstMsg[0];
				tstring strLeft, strRight;
				strRight = strValue;
				int iValue  = strRight.find(13);
				while(1)
				{
					if (iValue < 0)
					{
						lstMsg.push_back(strRight.c_str());
						break;
					}
					strLeft = strRight.substr(0,strRight.find(13) +1);
					strRight = strRight.substr(strRight.find(13) +1, strRight.length());
					lstMsg.push_back(strLeft.c_str());
					iValue  = strRight.find(13);	
				}
				pchatInfo->m_lstMsg = lstMsg;
			}

			//int 
			for (int i=0; i<pchatInfo->m_lstMsg.size(); i++)
			{
 				CDuiString strValue = pchatInfo->m_lstMsg[i];
 				HDC hdc = ::GetDC(mainDlg::getInstance()->GetHWND());
 				SIZE sz = CRenderEngine::GetTextSize(hdc, m_pPaintManager, strValue, -1,0);
 				::ReleaseDC(mainDlg::getInstance()->GetHWND(), hdc);

				tstring strTmp = strValue.GetData();
				int inindex = 0; // = strTmp.find(9);
				for (int i=0; i<strTmp.length(); i++)
				{
					if (strTmp[i] == 9)
						inindex++;
				}
				//if (inindex > 0)
				//{
				sz.cx += 41 * inindex;
				//计算当前一行需要分几行
				if (sz.cx > sn_Max_ListItem_Width)    //超过一行
				{
					nMaxTextWidth = sn_Max_ListItem_Width;
					int nLeft = sz.cx % sn_Max_ListItem_Width;
					int nRow = sz.cx/sn_Max_ListItem_Width;
					if (nLeft>0)
					{
						nRow += 1;
					}

					nTextTotalHeight += nRow* sn_Offset_Row_Height;
				}
				//当前字符串没有超过最大值
				else
				{
					//更新最大的字符串的长度值
					nMaxTextWidth = sz.cx > nMaxTextWidth ? sz.cx : nMaxTextWidth;

					nTextTotalHeight += sn_Offset_Row_Height;
				}

				//还原字符串内容
				strOutput += pchatInfo->m_lstMsg[i];
			}

			plabelText->SetFixedWidth(nMaxTextWidth + sn_Offset_Left + sn_Offset_Left);// + 18);
			plabelText->SetFixedHeight(nTextTotalHeight + sn_Offset_Top * 2);
			plabelText->SetText(strOutput);
		}
		plstItem->SetFixedHeight(nTextTotalHeight + sn_Offset_Top * 2 + 18);
		
		if (!CListUI::Add(plstItem))
		{
			delete plstItem;
		}

		EndListDown(nTextTotalHeight + sn_Offset_Top * 2 + 18);

		return 1;
	}
	//增加系统提醒消息：xxx已经是你的好友，你们可以聊天了。
	bool DuiLib::CChatListUI::AddSysNotification(tstring strAccount)
	{
		//ostringstream os;
		//os<<"你已添加了"<<strAccount.c_str()<<"，现在可以开始聊天了。";

		int iItemHeight = 30;
		CDialogBuilder m_DlgBuilder;
		CListContainerElementUI *plstItem = NULL;
		if (!m_DlgBuilder.GetMarkup()->IsValid())
		{
			plstItem = static_cast<CListContainerElementUI*>(m_DlgBuilder.Create(_T("sys_notify_item.xml"),
				(UINT)0, NULL, m_pPaintManager));
		}
		else
		{
			plstItem = static_cast<CListContainerElementUI*>(m_DlgBuilder.Create((UINT)0, m_pPaintManager));
		}
		if (plstItem == NULL)
			return false;
		CLabelUI *plblTm= static_cast<CLabelUI*>(m_pPaintManager->FindSubControlByName(plstItem, _T("sysNotification")));
		if (plblTm != NULL)
			plblTm->SetText("我们已经是好友了，开始聊天吧!");
		plstItem->SetFixedHeight(iItemHeight);
		if (!CListUI::Add(plstItem))
		{
			delete plstItem;
		}

		EndListDown(iItemHeight);
		//EndDown();
		return 1;
		
	}
	
	bool DuiLib::CChatListUI::AddSysItem(CDuiString& strSys)
	{
		int iItemHeight = 30;
		CDialogBuilder m_DlgBuilder;
		CListContainerElementUI *plstItem = NULL;
		if (!m_DlgBuilder.GetMarkup()->IsValid())
		{
			plstItem = static_cast<CListContainerElementUI*>(m_DlgBuilder.Create(_T("chat_sys_item.xml"),
				(UINT)0, NULL, m_pPaintManager));
		}
		else
		{
			plstItem = static_cast<CListContainerElementUI*>(m_DlgBuilder.Create((UINT)0, m_pPaintManager));
		}
		if (plstItem == NULL)
			return false;
		CLabelUI *plblTm= static_cast<CLabelUI*>(m_pPaintManager->FindSubControlByName(plstItem, _T("labelSys")));
		if (plblTm != NULL)
			plblTm->SetText(strSys);
		plstItem->SetFixedHeight(iItemHeight);
		if (!CListUI::Add(plstItem))
		{
			delete plstItem;
		}

		EndListDown(iItemHeight);
		//EndDown();
		return 1;
	}

	bool DuiLib::CChatListUI::AddTimeItem(CDuiString& strTm, int extra_height_need/* = 0*/)
	{
//		int iItemHeight = 30;
		CDialogBuilder m_DlgBuilder;
		CListContainerElementUI *plstItem = NULL;
		if (!m_DlgBuilder.GetMarkup()->IsValid())
		{
			plstItem = static_cast<CListContainerElementUI*>(m_DlgBuilder.Create(_T("chat_time_item.xml"),
				(UINT)0, NULL, m_pPaintManager));
		}
		else
		{
			plstItem = static_cast<CListContainerElementUI*>(m_DlgBuilder.Create((UINT)0, m_pPaintManager));
		}
		if (plstItem == NULL)
			return false;
		CRichEditUI *plblTm= static_cast<CRichEditUI*>(m_pPaintManager->FindSubControlByName(plstItem, _T("labelText")));
		if (plblTm != NULL)
			plblTm->SetText(strTm);

		////////////////////////////////////////////////////////////////////////////////////////////////
		CDuiString strValue = strTm;
		int nMaxTextWidth = 0;
		int nTextTotalHeight = 0;
		HDC hdc = ::GetDC(mainDlg::getInstance()->GetHWND());
		SIZE sz = CRenderEngine::GetTextSize(hdc, m_pPaintManager, strValue, -1,0);
		::ReleaseDC(mainDlg::getInstance()->GetHWND(), hdc);

		//计算当前一行需要分几行
		if (sz.cx > sn_Max_ListItem_Width)    //超过一行
		{
			nMaxTextWidth = sn_Max_ListItem_Width;
			int nLeft = sz.cx % sn_Max_ListItem_Width;
			int nRow = sz.cx/sn_Max_ListItem_Width;
			if (nLeft>0)
			{
				nRow += 1;
			}

			nTextTotalHeight += nRow* sn_Offset_Row_Height;
		}
		//当前字符串没有超过最大值
		else
		{
			//更新最大的字符串的长度值
			nMaxTextWidth = sz.cx > nMaxTextWidth ? sz.cx : nMaxTextWidth;

			nTextTotalHeight += sn_Offset_Row_Height;
		}
		///////////////////////////////////////////////////////////////////////////////////////
		plblTm->SetFixedHeight(nTextTotalHeight+18);
		plstItem->SetFixedHeight(nTextTotalHeight+18+extra_height_need);


		if (!CListUI::Add(plstItem))
		{
			delete plstItem;
		}

		EndListDown(nTextTotalHeight+18+extra_height_need);
		//EndDown();
		return 1;
	}

	bool DuiLib::CChatListUI::AddMyMsgItem( ChatInfo* pchatInfo )
	{
		//EndDown();
		if (pchatInfo->uTime - m_curTime >= sn_FIVE_MINITUS)               //时间超过5分钟
		{
			struct tm * now = localtime(&pchatInfo->uTime);
			char chBuf[50];
			memset(chBuf, 0, 50);
			sprintf(chBuf, "%02u:%02u", now->tm_hour, now->tm_min);
			CDuiString strTm = chBuf;

			AddTimeItem(strTm);
		}
		m_curTime = pchatInfo->uTime;

		CDialogBuilder DlgBuilder;
		CListContainerElementUI *plstItem = NULL;
		if (!DlgBuilder.GetMarkup()->IsValid())
		{
			plstItem = static_cast<CListContainerElementUI*>(DlgBuilder.Create(_T("chat_my_item.xml"),
				(UINT)0, NULL, m_pPaintManager));
		}
		else
		{
			plstItem = static_cast<CListContainerElementUI*>(DlgBuilder.Create((UINT)0, m_pPaintManager));
		}
		if (plstItem == NULL)
			return false;
		CControlUI *pControlLogo = m_pPaintManager->FindSubControlByName(plstItem, kcontrolLogControName);
		if (pControlLogo != NULL)
		{
			pControlLogo->SetBkImage(pchatInfo->m_strLogo);
		}

		int nTextTotalHeight = 0;
		

		CRichEditUI *plabelText = static_cast<CRichEditUI*>(m_pPaintManager->FindSubControlByName(plstItem, klabelTextControlName));
		if (plabelText != NULL)
		{
			CDuiString strOutput;
			int iMaxStringLength = 0;
			//如果传进来的数据没有分行，需要自己分行处理
			if (pchatInfo->m_lstMsg.size() == 1)
			{
				std::vector<CDuiString> lstMsg;
				tstring strValue = pchatInfo->m_lstMsg[0];
				tstring strLeft, strRight;
				strRight = strValue;
				int iValue  = strRight.find(13);
				while(1)
				{
					if (iValue < 0)
					{
						lstMsg.push_back(strRight.c_str());
						break;
					}
					strLeft = strRight.substr(0,strRight.find(13) + 1);
					strRight = strRight.substr(strRight.find(13) +1, strRight.length());
					lstMsg.push_back(strLeft.c_str());
					iValue  = strRight.find(13);
				
				}
				pchatInfo->m_lstMsg = lstMsg;
			}

			for (int i=0; i<pchatInfo->m_lstMsg.size(); i++)
			{
				CDuiString strValue = pchatInfo->m_lstMsg[i];
				HDC hdc = ::GetDC(mainDlg::getInstance()->GetHWND());
				SIZE sz = CRenderEngine::GetTextSize(hdc, m_pPaintManager, strValue, -1,0);
				tstring strTmp = strValue.GetData();
				int inindex = 0; // = strTmp.find(9);
				for (int i=0; i<strTmp.length(); i++)
				{
					if (strTmp[i] == 9)
						inindex++;
				}
				//if (inindex > 0)
				//{
					sz.cx += 41 * inindex;
				//}
				::ReleaseDC(mainDlg::getInstance()->GetHWND(), hdc);
				
				//计算当前一行需要分几行
				if (sz.cx > sn_Max_ListItem_Width)    //超过一行
				{
					iMaxStringLength = sn_Max_ListItem_Width;
					int nLeft = sz.cx % sn_Max_ListItem_Width;
					int nRow = sz.cx/sn_Max_ListItem_Width;
					if (nLeft>0)
					{
						nRow += 1;
					}
					
					nTextTotalHeight += (nRow)* sn_Offset_Row_Height;// + sn_Offset_Top * 2;
				}
				//当前字符串没有超过最大值
				else
				{
					//更新最大的字符串的长度值
					iMaxStringLength = sz.cx > iMaxStringLength ? sz.cx : iMaxStringLength;
					//nRow += 1;
					nTextTotalHeight += sn_Offset_Row_Height;
				}

				//还原字符串内容
				strOutput += pchatInfo->m_lstMsg[i];
			}
			

			//构建包围文字的矩形框
			RECT rcText;
			rcText.top	  = 2;
			rcText.bottom = rcText.top + nTextTotalHeight + sn_Offset_Top * 2;
			rcText.right  = sn_YPos_MyChatText;
			int nWidth;
			//如果有分行的字符串，说明当前文本框的宽度达到了最大值。
			if (iMaxStringLength == sn_Max_ListItem_Width)
			{
				rcText.left = sn_YPos_MyChatText-sn_Max_ListItem_Width-sn_Offset_Left * 2;
				//nWidth = sn_Offset_Left*2 + sn_Max_ListItem_Width;
			}
			//当前的每一行字符串都没有达到最大值
			else
			{
				rcText.left = sn_YPos_MyChatText-iMaxStringLength-sn_Offset_Left * 2;
				//nWidth = sn_Offset_Left*2 + sn_Max_ListItem_Width;
			}		
			//rcText.left = 200;
			plabelText->SetPos(rcText);
			//plabelText->SetFixedHeight(nTextTotalHeight + sn_Offset_Top * 2);
		//	plabelText->SetFixedWidth(nTextTotalHeight + sn_Offset_Top * 2);
			plabelText->SetText(strOutput);
			//CDuiString str = plabelText->GetTextRange(0, strOutput.GetLength());
		}

		plstItem->SetFixedHeight(nTextTotalHeight + sn_Offset_Top * 2 + 28);
		
		if (!CListUI::Add(plstItem))
		{
			delete plstItem;
		}

		EndListDown(nTextTotalHeight + sn_Offset_Top * 2 + 28);

		return 1;
	}

	bool CChatListUI::AddOppositeFileMsgItem( ChatInfo *pFilesInfo )
	{
		//2. ui

		if (pFilesInfo->uTime - m_curTime >= sn_FIVE_MINITUS)               //时间超过5分钟
		{
			struct tm * now = localtime(&pFilesInfo->uTime);
			char chBuf[50];
			memset(chBuf, 0, 50);
			sprintf(chBuf, "%02u:%02u", now->tm_hour, now->tm_min);
			CDuiString strTm = chBuf;

			AddTimeItem(strTm);
		}
		m_curTime = pFilesInfo->uTime;


		CDialogBuilder m_DlgBuilder;
		CListContainerElementUI *plstItem = NULL;

		if (!m_DlgBuilder.GetMarkup()->IsValid())
		{
			plstItem = static_cast<CListContainerElementUI*>(m_DlgBuilder.Create(_T("chat_opposite_file_item.xml"),
				(UINT)0, NULL, m_pPaintManager));
		}
		else
		{
			plstItem = static_cast<CListContainerElementUI*>(m_DlgBuilder.Create((UINT)0, m_pPaintManager));
		}
		if (plstItem == NULL)
			return false;

		if(pFilesInfo->m_strRawMsg.Find("#COMMAND#type=fileRequest") != -1)
		{
			pFilesInfo->m_lstFiles.clear();
			tstring strRight = pFilesInfo->m_strRawMsg;
			fileInfo downLoadInfo;
			downLoadInfo.strFileFullName = strRight.substr(strRight.find("\"file_name\":\"") + 13, strRight.find("\",\"file_type\"") - (strRight.find("\"file_name\":\"") + 13));
			//m_bAddFiles = false;
			strRight = strRight.substr(strRight.find("\"file_type\":\""), strRight.length());
			downLoadInfo.strFileType = strRight.substr(strRight.find("\"file_type\":\"") + 13, strRight.find("\",\"file_size\"") - (strRight.find("\"file_type\":\"") + 13));
			strRight = strRight.substr(strRight.find("\"file_size\":\""), strRight.length());
			downLoadInfo.dwFileSize = atol(strRight.substr(strRight.find("\"file_size\":\"") + 13, strRight.find("\",\"ali_file_url\"") - (strRight.find("\"file_size\":\"") + 13)).c_str());
			strRight = strRight.substr(strRight.find("\"ali_file_url\":\""), strRight.length());
			downLoadInfo.strFileUrl = strRight.substr(strRight.find("\"ali_file_url\":\"") + 16, strRight.find("\",\"file_path\"") - (strRight.find("\"ali_file_url\":\"") + 16));
			tstring strName = downLoadInfo.strFileFullName;
			tstring strfileName = strName.substr(0, strName.find_last_of("."));
			downLoadInfo.strFileName = strfileName;
			pFilesInfo->m_lstFiles.push_back(downLoadInfo);
		}

		CListUI *plstFile = static_cast<CListUI*>(m_pPaintManager->FindSubControlByName(plstItem, _T("lstFiles")));
		int iHeight = 40;
		iHeight += 30 * pFilesInfo->m_lstFiles.size();
		plstItem->SetFixedHeight(iHeight+40);
		CHorizontalLayoutUI *pHorUI = static_cast<CHorizontalLayoutUI*>(m_pPaintManager->FindSubControlByName(plstItem, _T("HorizontalLayoutUI2")));
		pHorUI->SetFixedHeight(iHeight-30);
		CContainerUI *pContUI = static_cast<CContainerUI*>(m_pPaintManager->FindSubControlByName(plstItem, _T("ContainerUI2")));
		pContUI->SetFixedHeight(iHeight-30);
		plstFile->SetFixedHeight(iHeight-40);

		
		


		createFileList(plstFile, pFilesInfo->m_lstFiles);
		plstItem->SetTag(m_iTag);
		CControlUI *pControl = static_cast<CControlUI*>(m_pPaintManager->FindSubControlByName(plstItem, _T("controlLogo")));
		if (pControl != NULL)
		{
			pControl->SetBkImage(pFilesInfo->m_strLogo);
		}
		//m_mapGetFiles.insert(make_pair(m_iTag, lstGetFiles));
		m_iTag ++;

		if (!CListUI::Add(plstItem))
		{
			delete plstItem;
		}

		EndListDown(iHeight+40);
		//EndDown();
		return 1;
	}

	bool DuiLib::CChatListUI::AddMyFileMsgItem( ChatInfo* pFilesInfo )
	{
		if (pFilesInfo->uTime - m_curTime >= sn_FIVE_MINITUS)               //时间超过5分钟
		{
			struct tm * now = localtime(&pFilesInfo->uTime);
			char chBuf[50];
			memset(chBuf, 0, 50);
			sprintf(chBuf, "%02u:%02u", now->tm_hour, now->tm_min);
			CDuiString strTm = chBuf;

			AddTimeItem(strTm);
		}
		m_curTime = pFilesInfo->uTime;


		CDialogBuilder m_DlgBuilder;
		CListContainerElementUI *plstItem = NULL;
		
		if (!m_DlgBuilder.GetMarkup()->IsValid())
		{
			plstItem = static_cast<CListContainerElementUI*>(m_DlgBuilder.Create(_T("chat_file_item.xml"),
				(UINT)0, NULL, m_pPaintManager));
		}
		else
		{
			plstItem = static_cast<CListContainerElementUI*>(m_DlgBuilder.Create((UINT)0, m_pPaintManager));
		}
		if (plstItem == NULL)
			return false;
		
		if(pFilesInfo->m_strRawMsg.Find("#COMMAND#type=fileRequest") != -1)
		{
			pFilesInfo->m_lstFiles.clear();
			tstring strRight = pFilesInfo->m_strRawMsg;
			fileInfo downLoadInfo;
			downLoadInfo.strFileFullName = strRight.substr(strRight.find("\"file_name\":\"") + 13, strRight.find("\",\"file_type\"") - (strRight.find("\"file_name\":\"") + 13));
			//m_bAddFiles = false;
			strRight = strRight.substr(strRight.find("\"file_type\":\""), strRight.length());
			downLoadInfo.strFileType = strRight.substr(strRight.find("\"file_type\":\"") + 13, strRight.find("\",\"file_size\"") - (strRight.find("\"file_type\":\"") + 13));
			strRight = strRight.substr(strRight.find("\"file_size\":\""), strRight.length());
			downLoadInfo.dwFileSize = atol(strRight.substr(strRight.find("\"file_size\":\"") + 13, strRight.find("\",\"ali_file_url\"") - (strRight.find("\"file_size\":\"") + 13)).c_str());
			strRight = strRight.substr(strRight.find("\"ali_file_url\":\""), strRight.length());
			downLoadInfo.strFileUrl = strRight.substr(strRight.find("\"ali_file_url\":\"") + 16, strRight.find("\",\"file_path\"") - (strRight.find("\"ali_file_url\":\"") + 16));
			tstring strName = downLoadInfo.strFileFullName;
			tstring strfileName = strName.substr(0, strName.find_last_of("."));
			downLoadInfo.strFileName = strfileName;
			pFilesInfo->m_lstFiles.push_back(downLoadInfo);
		}
		CListUI *plstFile = static_cast<CListUI*>(m_pPaintManager->FindSubControlByName(plstItem, _T("lstFiles")));
		int iHeight = 40;
		iHeight += 30 * pFilesInfo->m_lstFiles.size();
		plstItem->SetFixedHeight(iHeight+10);
		CHorizontalLayoutUI *pHorUI = static_cast<CHorizontalLayoutUI*>(m_pPaintManager->FindSubControlByName(plstItem, _T("HorizontalLayoutUI2")));
		pHorUI->SetFixedHeight(iHeight-30);
		CContainerUI *pContUI = static_cast<CContainerUI*>(m_pPaintManager->FindSubControlByName(plstItem, _T("ContainerUI2")));
		pContUI->SetFixedHeight(iHeight-30);
		plstFile->SetFixedHeight(iHeight-40);
		
		createFileList(plstFile, pFilesInfo->m_lstFiles);
		CControlUI *pControl = static_cast<CControlUI*>(m_pPaintManager->FindSubControlByName(plstItem, _T("controlLogo")));
		if (pControl != NULL)
		{
			pControl->SetBkImage(pFilesInfo->m_strLogo);
		}
		if (!CListUI::Add(plstItem))
		{
			delete plstItem;
		}

		EndListDown(iHeight+10);
		//EndDown();
		return 1;
	}

	bool CChatListUI::AddMyGroupFileItem(tstring& strPackID, tstring& strAvatar, time_t timeRecv, sPackItem& onepack)
	{
		if (timeRecv - m_curTime >= sn_FIVE_MINITUS)               //时间超过5分钟
		{
			struct tm * now = localtime(&timeRecv/*&pFilesInfo->uTime*/);
			char chBuf[50];
			memset(chBuf, 0, 50);
			sprintf(chBuf, "%02u:%02u", now->tm_hour, now->tm_min);
			CDuiString strTm = chBuf;

			AddTimeItem(strTm);
		}
		m_curTime = timeRecv;//pFilesInfo->uTime;


		CDialogBuilder m_DlgBuilder;
		CListContainerElementUI *plstItem = NULL;

		if (!m_DlgBuilder.GetMarkup()->IsValid())
		{
			plstItem = static_cast<CListContainerElementUI*>(m_DlgBuilder.Create(_T("groupchat_packfile_my_item.xml"),
				(UINT)0, NULL, m_pPaintManager));
		}
		else
		{
			plstItem = static_cast<CListContainerElementUI*>(m_DlgBuilder.Create((UINT)0, m_pPaintManager));
		}
		if (plstItem == NULL)
			return false;

		CControlUI *pControl = static_cast<CControlUI*>(m_pPaintManager->FindSubControlByName(plstItem, _T("controlLogo")));
		if (pControl != NULL)
		{
			pControl->SetBkImage(strAvatar.c_str()/*pFilesInfo->m_strLogo*/);
		}


		sPackItem* pOnePack = NULL;//new sPackItem;
		pOnePack = new sPackItem;
		pOnePack->strPackID = onepack.strPackID;
		pOnePack->strPackName = onepack.strPackName;
		pOnePack->strPackCreaterPhone = onepack.strPackCreaterPhone;
		pOnePack->nPackStatus = onepack.nPackStatus;
		pOnePack->strPackProp = onepack.strPackProp;
		pOnePack->strPackSender = onepack.strPackSender;
		pOnePack->strPackSize = onepack.strPackSize;
		pOnePack->strPackTime = onepack.strPackTime;
		pOnePack->listFile = onepack.listFile;
	
		//给”查看“按钮设置数据
		CControlUI* pOpenBtn = static_cast<CControlUI*>(m_pPaintManager->FindSubControlByName(plstItem, _T("btn_open_packfiles")));
		if (NULL != pOpenBtn)
		{
			pOpenBtn->SetTag((UINT_PTR)pOnePack);
		}
		CControlUI* pShouCangBtn = static_cast<CControlUI*>(m_pPaintManager->FindSubControlByName(plstItem, _T("btn_shoucang_packfiles")));
		if (NULL != pShouCangBtn)
		{
			pShouCangBtn->SetTag((UINT_PTR)pOnePack);
		}

		//给整个ListItem设置data，情况chatList的时候，需要释放这块内存。
		plstItem->SetTag((UINT_PTR)pOnePack);

		CControlUI* pPackPropty = static_cast<CControlUI*>(m_pPaintManager->FindSubControlByName(plstItem, _T("label_pack_proproty")));
		if (NULL != pPackPropty)
		{
			pPackPropty->SetText(pOnePack->strPackProp.c_str());
		}

		CControlUI* pPackName = static_cast<CControlUI*>(m_pPaintManager->FindSubControlByName(plstItem, _T("label_pack_name")));
		if (NULL != pPackName)
		{
			pPackName->SetText(pOnePack->strPackName.c_str());
		}

		if (!CListUI::Add(plstItem))
		{
			delete plstItem;
		}

		EndListDown(/*iHeight*/200);//groupchat_packfile_my_item.xml里面定义的高度
		return 1;
	}

	bool CChatListUI::AddOppositeGroupFileItem(tstring& strPackID,tstring& strAvatar, time_t timeRecv, sPackItem& onepack)
	{
		if (timeRecv - m_curTime >= sn_FIVE_MINITUS)               //时间超过5分钟
		{
			struct tm * now = localtime(&timeRecv);
			char chBuf[50];
			memset(chBuf, 0, 50);
			sprintf(chBuf, "%02u:%02u", now->tm_hour, now->tm_min);
			CDuiString strTm = chBuf;

			AddTimeItem(strTm);
		}
		m_curTime = timeRecv;/*pFilesInfo->uTime;*/


		CDialogBuilder m_DlgBuilder;
		CListContainerElementUI *plstItem = NULL;

		if (!m_DlgBuilder.GetMarkup()->IsValid())
		{
			plstItem = static_cast<CListContainerElementUI*>(m_DlgBuilder.Create(_T("groupchat_packfile_oppsite_item.xml"),
				(UINT)0, NULL, m_pPaintManager));
		}
		else
		{
			plstItem = static_cast<CListContainerElementUI*>(m_DlgBuilder.Create((UINT)0, m_pPaintManager));
		}
		if (plstItem == NULL)
			return false;

		CControlUI *pControl = static_cast<CControlUI*>(m_pPaintManager->FindSubControlByName(plstItem, _T("controlLogo")));
		if (pControl != NULL)
		{
			pControl->SetBkImage(strAvatar.c_str());
		}
		
		//m_iTag ++;

		sPackItem* pOnePack = NULL;//new sPackItem;
		pOnePack = new sPackItem;
		pOnePack->strPackID = strPackID;
		pOnePack->strPackID = onepack.strPackID;
		pOnePack->strPackName = onepack.strPackName;
		pOnePack->strPackCreaterPhone = onepack.strPackCreaterPhone;
		pOnePack->nPackStatus = onepack.nPackStatus;
		pOnePack->strPackProp = onepack.strPackProp;
		pOnePack->strPackSender = onepack.strPackSender;
		pOnePack->strPackSize = onepack.strPackSize;
		pOnePack->strPackTime = onepack.strPackTime;
		pOnePack->listFile = onepack.listFile;

		CControlUI* pOpenBtn = static_cast<CControlUI*>(m_pPaintManager->FindSubControlByName(plstItem, _T("btn_open_packfiles")));
		if (NULL != pOpenBtn)
		{
			pOpenBtn->SetTag((UINT_PTR)pOnePack);
		}
		CControlUI* pShouCangBtn = static_cast<CControlUI*>(m_pPaintManager->FindSubControlByName(plstItem, _T("btn_shoucang_packfiles")));
		if (NULL != pShouCangBtn)
		{
			pShouCangBtn->SetTag((UINT_PTR)pOnePack);
		}

		//给整个ListItem设置data，情况chatList的时候，需要释放这块内存。
		plstItem->SetTag((UINT_PTR)pOnePack);

		CControlUI* pPackPropty = static_cast<CControlUI*>(m_pPaintManager->FindSubControlByName(plstItem, _T("label_pack_proproty")));
		if (NULL != pPackPropty)
		{
			pPackPropty->SetText(pOnePack->strPackProp.c_str());
		}

		CControlUI* pPackName = static_cast<CControlUI*>(m_pPaintManager->FindSubControlByName(plstItem, _T("label_pack_name")));
		if (NULL != pPackName)
		{
			pPackName->SetText(pOnePack->strPackName.c_str());
		}



		if (!CListUI::Add(plstItem))
		{
			delete plstItem;
		}

		EndListDown(190/*iHeight*/);//groupchat_packfile_my_item.xml里面定义的高度
		
		return 1;
	}

	void CChatListUI::createFileList( CListUI * pParent, vector<fileInfo>& m_lstFiles)
	{
		pParent->RemoveAll();
		//int iHeight = 40;
		for (int i=0; i<m_lstFiles.size(); i++)
		{
			CDialogBuilder m_DlgBuilder;
			CListContainerElementUI *plstItem = NULL;
			if (!m_DlgBuilder.GetMarkup()->IsValid())
			{
				plstItem = static_cast<CListContainerElementUI*>(m_DlgBuilder.Create(_T("file_item_child.xml"),
					(UINT)0, NULL, m_pPaintManager));
			}
			else
			{
				plstItem = static_cast<CListContainerElementUI*>(m_DlgBuilder.Create((UINT)0, m_pPaintManager));
			}
			if (plstItem == NULL)
				return;
			plstItem->SetFixedHeight(30);
			pParent->Add(plstItem);
			//設置名字和icon
			CDuiString strName = m_lstFiles[i].strFileName.c_str();
			// CString strType = m_lstFiles[i].strFileType.c_str();
			CDuiString strType = m_lstFiles[i].strFileType.c_str();
			CLabelUI *plablName = static_cast<CLabelUI*>(m_pPaintManager->FindSubControlByName(plstItem, _T("labelFileName")));
			CControlUI *pcontrlType = static_cast<CControlUI*>(m_pPaintManager->FindSubControlByName(plstItem, _T("controlFileLogo")));

			CDuiString strFullName;
			if(!strType.IsEmpty())
			{
				strFullName = strName + "." + strType;
			}
			else
			{
				strFullName = strName;
			}
			plablName->SetText(strFullName);
			pcontrlType->SetBkImage(GetPackFileIcon(m_lstFiles[i].strFileType).c_str());
			//strType.MakeLower();
			//if (   strType == _T("png") 
			//	|| strType == _T("bmp")
			//	|| strType == _T("jpg"))
			//{
			//	pcontrlType->SetBkImage("image(1).png");
			//}
			//else if (strType == _T("txt")
			//	|| strType == _T("doc")
			//	|| strType == _T("xls")
			//	|| strType == _T("xml"))
			//{
			//	pcontrlType->SetBkImage("Doc.png");
			//}
			//else
			//{
			//	//todo
			//}
		}	
	}

	bool CChatListUI::AddOppositeVoiceItem( ChatInfo *pFilesInfo )
	{
		if (pFilesInfo->uTime - m_curTime >= sn_FIVE_MINITUS)               //时间超过5分钟
		{
			struct tm * now = localtime(&pFilesInfo->uTime);
			char chBuf[50];
			memset(chBuf, 0, 50);
			sprintf(chBuf, "%02u:%02u", now->tm_hour, now->tm_min);
			CDuiString strTm = chBuf;

			AddTimeItem(strTm);
		}
		m_curTime = pFilesInfo->uTime;


		CDialogBuilder m_DlgBuilder;
		CListContainerElementUI *plstItem = NULL;

		if (!m_DlgBuilder.GetMarkup()->IsValid())
		{
			plstItem = static_cast<CListContainerElementUI*>(m_DlgBuilder.Create(_T("chat_opposite_file_item.xml"),
				(UINT)0, NULL, m_pPaintManager));
		}
		else
		{
			plstItem = static_cast<CListContainerElementUI*>(m_DlgBuilder.Create((UINT)0, m_pPaintManager));
		}
		if (plstItem == NULL)
			return false;

		if(pFilesInfo->m_strRawMsg.Find("#COMMAND#type=fileRequest") != -1)
		{
			pFilesInfo->m_lstFiles.clear();
			tstring strRight = pFilesInfo->m_strRawMsg;
			fileInfo downLoadInfo;
			downLoadInfo.strFileFullName = strRight.substr(strRight.find("\"file_name\":\"") + 13, strRight.find("\",\"file_type\"") - (strRight.find("\"file_name\":\"") + 13));
			//m_bAddFiles = false;
			strRight = strRight.substr(strRight.find("\"file_type\":\""), strRight.length());
			downLoadInfo.strFileType = strRight.substr(strRight.find("\"file_type\":\"") + 13, strRight.find("\",\"file_size\"") - (strRight.find("\"file_type\":\"") + 13));
			strRight = strRight.substr(strRight.find("\"file_size\":\""), strRight.length());
			downLoadInfo.dwFileSize = atol(strRight.substr(strRight.find("\"file_size\":\"") + 13, strRight.find("\",\"ali_file_url\"") - (strRight.find("\"file_size\":\"") + 13)).c_str());
			strRight = strRight.substr(strRight.find("\"ali_file_url\":\""), strRight.length());
			downLoadInfo.strFileUrl = strRight.substr(strRight.find("\"ali_file_url\":\"") + 16, strRight.find("\",\"file_path\"") - (strRight.find("\"ali_file_url\":\"") + 16));
			tstring strName = downLoadInfo.strFileFullName;
			tstring strfileName = strName.substr(0, strName.find_last_of("."));
			downLoadInfo.strFileName = strfileName;
			pFilesInfo->m_lstFiles.push_back(downLoadInfo);
		}

		CListUI *plstFile = static_cast<CListUI*>(m_pPaintManager->FindSubControlByName(plstItem, _T("lstFiles")));
		int iHeight = 40;
		iHeight += 30 * pFilesInfo->m_lstFiles.size();
		plstItem->SetFixedHeight(iHeight+40);
		CHorizontalLayoutUI *pHorUI = static_cast<CHorizontalLayoutUI*>(m_pPaintManager->FindSubControlByName(plstItem, _T("HorizontalLayoutUI2")));
		pHorUI->SetFixedHeight(iHeight-30);
		CContainerUI *pContUI = static_cast<CContainerUI*>(m_pPaintManager->FindSubControlByName(plstItem, _T("ContainerUI2")));
		pContUI->SetFixedHeight(iHeight-30);
		plstFile->SetFixedHeight(iHeight-40);





		createFileList(plstFile, pFilesInfo->m_lstFiles);
		plstItem->SetTag(m_iTag);
		CControlUI *pControl = static_cast<CControlUI*>(m_pPaintManager->FindSubControlByName(plstItem, _T("controlLogo")));
		if (pControl != NULL)
		{
			pControl->SetBkImage(pFilesInfo->m_strLogo);
		}
		//m_mapGetFiles.insert(make_pair(m_iTag, lstGetFiles));
		m_iTag ++;

		if (!CListUI::Add(plstItem))
		{
			delete plstItem;
		}

		EndListDown(iHeight+40);
		//EndDown();
		return 1;
	}
}

void CChatListUI::ClearMessage( const CDuiString& account )
{
//	MapAccout2Messages::iterator itFind = m_mapDest2Msgs.find(account);
//	m_mapDest2Msgs.erase(itFind);
	for (MapAccout2Messages::iterator it = m_mapDest2Msgs.begin(); it != m_mapDest2Msgs.end(); ++it)
	{
		if (it->first == account)
		{
			m_mapDest2Msgs.erase(it);
			break;
		}
	}
}

void CChatListUI::ReloadChatList(CDuiString& strAccount)
{
	MapAccout2Messages::iterator itFind = m_mapDest2Msgs.find(strAccount);
	if (itFind != m_mapDest2Msgs.end())
	{
		ListMessages& listMsgs = itFind->second;
		for (ListMessages::iterator it = listMsgs.begin(); it != listMsgs.end(); ++it)
		{
			ChatInfo& oneMsg = *it;
			if (_tcsicmp(oneMsg.m_strAccount.GetData(), m_strMyAcount.c_str())==0)
			{
				if (oneMsg.m_strRawMsg.Find("#COMMAND#type=fileRequest") != -1 || oneMsg.m_strRawMsg.Find("#COMMAND#type=imgRequest") != -1)
				{
					
					AddOppositeFileMsgItem(&oneMsg);
				}
				else
					AddMyMsgItem(&oneMsg);
			}

			else
			{
				if (oneMsg.m_strRawMsg.Find("#COMMAND#type=fileRequest") != -1 || oneMsg.m_strRawMsg.Find("#COMMAND#type=imgRequest") != -1)
				{
					AddMyFileMsgItem(&oneMsg);
				}
				else
					AddOppositeMsgItem(&oneMsg);
			}
		}
	}
}

void CChatListUI::SetMySelfAccount(tstring& strAccount)
{
	m_strMyAcount = strAccount;
}

void CChatListUI::CacheChatMessage(CDuiString& strDestAccount, ChatInfo* pchatInfo)
{
	//cache message.
	ChatInfo oneMsgItem;
	oneMsgItem.m_lstMsg = pchatInfo->m_lstMsg;
	oneMsgItem.m_strAccount = pchatInfo->m_strAccount;
	oneMsgItem.m_strRawMsg = pchatInfo->m_strRawMsg;
	oneMsgItem.uTime = pchatInfo->uTime;
	oneMsgItem.m_strLogo = pchatInfo->m_strLogo;

	MapAccout2Messages::iterator itFind = m_mapDest2Msgs.find(strDestAccount);
	if (itFind != m_mapDest2Msgs.end())
	{
		ListMessages& listMsgs = itFind->second;
		listMsgs.push_back(oneMsgItem);
	}
	//新增一个账号
	else
	{
		ListMessages listMsgs;
		listMsgs.push_back(oneMsgItem);
		m_mapDest2Msgs.insert(make_pair(strDestAccount, listMsgs));
	}
}

vector<fileInfo> CChatListUI::getDownLoadFiles(int iTGag)
{
	m_lstGetFiles lstfiles;
	//for()
	MapGetFiles::iterator itFind = m_mapGetFiles.find(iTGag);
	if (itFind != m_mapGetFiles.end())
	{
		lstfiles = itFind->second;
	}
	return lstfiles;
}

void CChatListUI::EndListDown(int nOffset)
{
	CScrollBarUI* pScrollBar = (CScrollBarUI*)(m_pPaintManager->FindSubControlByClass(this, _T("ScrollBarUI")));
	if (NULL != pScrollBar)
	{
		int nCurrHeight = pScrollBar->GetScrollRange();
		pScrollBar->SetScrollRange(nCurrHeight+nOffset);
	}

	EndDown();
}