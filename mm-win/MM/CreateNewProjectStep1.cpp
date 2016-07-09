#include "CreateNewProjectStep1.h"
#include "CreateNewProjectStep2.h"
#include "TileLayoutResPacket.h"
#include "UIDefine.h"
#include "Utility.h"
#include "Controller.h"
#include <sstream>
#include "MsgBox.h"
#include "HttpClient.h"
#include "oss_interface.h"
#include "StringHelper.h"
#include "..\json\json.h"
#include "..\Common\Logger.h"
#include "..\Include\Global_Def.h"
#include "CssWeb.h"
#include "webClient.h"

#define MW_TIMER_TOPWND 100

using namespace std;

struct sTmpInfo
{
	fileInfo* pFile;
	void* pThis;
	IOssEventBase* pEventHandle;
	tstring strProjectID;
	tstring strPackID;
	sTmpInfo()
	{
		pFile=NULL;
		pThis=NULL;
		pEventHandle=NULL;
	}
};



CCreateNewProjStep1::CCreateNewProjStep1(CPaintManagerUI* ppm /*= NULL*/)
	: save_btn_(NULL)
{
	//m_pProjProcessor = NULL;
	m_pPaintManager = ppm;
	m_hMainWnd = NULL;
	m_pSecondStepPage = NULL;
	m_pListWithProperty = NULL;
	m_bShowSecondPage = false;

	::InitializeCriticalSection(&m_csUploadFile);
}

CCreateNewProjStep1::~CCreateNewProjStep1()
{
// 	if (NULL != m_pProjProcessor)
// 	{
// 		delete m_pProjProcessor;
// 		m_pProjProcessor = NULL;
// 	}
	
	//ΪʲôҪdelete control������Ҫ����ԭ��
	//close_btn_ ��ɾ���ͻ�����������Ҫ�����о� by zyy
//	DeleteCtrl();

	if (NULL != m_pSecondStepPage)
	{
		m_pSecondStepPage->PostMessage(WM_CLOSE);
	}
	::DeleteCriticalSection(&m_csUploadFile);
}

bool CCreateNewProjStep1::CheckIfDataChanged()
{
	bool bChanged = false;	
	tstring strProjName = new_project_name_edit_->GetText();
	if (!strProjName.empty() 
		&&_tcsicmp(strProjName.c_str(), m_sLastSavedProjData.strProjName.c_str()) != 0)
	{
		bChanged = true;
	}

	if (!bChanged)
	{		
		tstring strProperty = respack_property_combo_->GetText();
		if (_tcsicmp(strProperty.c_str(), m_sLastSavedProjData.strResProperty.c_str()) != 0)
		{
			bChanged = true;
		}
	}
	
	if (!bChanged)
	{
		tstring strResPackName = respack_name_edit_->GetText();
		if (!strResPackName.empty() 
			&& _tcsicmp(strResPackName.c_str(), m_sLastSavedProjData.strResName.c_str()) != 0)
		{
			bChanged = true;
		}
	}

	if (!bChanged)
	{		
		tstring strMemo = new_project_memo_richedit->GetText();
		if (_tcsicmp(strMemo.c_str(), m_sLastSavedProjData.strMemo.c_str()) != 0)
		{
			bChanged = true;
		}
	}

	if (!bChanged)
	{
		//�ȿ��ļ�������û�б仯��
		int nFileCount = m_pListWithProperty->GetCount();
		if (nFileCount != m_sLastSavedProjData.mapFile2Size.size())
		{
			bChanged = true;
		}

		//�Ƚ��ļ���+��С�ɱ仯�ˡ�
		if (!bChanged)
		{
			if (nFileCount>0)
			{
				for (int i=0; i<nFileCount; i++)
				{
					CContainerUI* pListItem = (CContainerUI*)m_pListWithProperty->GetItemAt(i);
					if (NULL != pListItem)
					{
						fileInfo* pFile = (fileInfo*)pListItem->GetTag();
						if (NULL != pFile)
						{
							tstring uploadName = pFile->strFileFullName;          //
							DWORD dwFileSize = pFile->dwFileSize;
							map<tstring, DWORD>::iterator itFind = m_sLastSavedProjData.mapFile2Size.find(uploadName);
							if (itFind == m_sLastSavedProjData.mapFile2Size.end())
							{
								bChanged = true;
								break;
							}
							else
							{
								if (itFind->second != dwFileSize)
								{
									bChanged = true;
									break;
								}
							}
						}
					}
				}
			}
		}
	}

	return bChanged;
}

void CCreateNewProjStep1::Notify(TNotifyUI& msg)
{
	if (_tcsicmp(msg.sType, _T("click")) == 0)
	{
		//�رս���
//		if (msg.pSender == close_btn_)
		if (msg.pSender->GetName() == _T("btn_close_create_proj"))
		{
			OnBnClickClose();
		}
		else if (msg.pSender == select_car_btn_)
		{
			OnBnClickSelectCar();
		}
		else if (msg.pSender == vin_btn_)
		{
			OnBnClickVin();		
		}
		else if (msg.pSender == select_friend_btn_)
		{
			OnBnClickSelectFriend();			
		}
		else if (msg.pSender->GetName() == _T("btn_delete_askfileitem"))
		{
			OnBnClickRemoveOneFile(msg.pSender);
		}
		//ɾ��������Դ��1.
		else if(msg.pSender == delete_respack1_btn_)
		{
			OnDeleteRespack1();
		}
		//ѡ���ļ���
		else if (msg.pSender == select_file_btn_)
		{
			OnBnClickSelectFiles();
		}
		//"����"��ť
		else if (msg.pSender == save_btn_)
		{
			OnBnClickSaveProject();				
		}
	}
	else if (_tcsicmp(msg.sType, _T("dbclick")) == 0)
	{
		return;
	}

}
//��ȡ��ĿID�˺�
bool CCreateNewProjStep1::Initialize()
{
	create_room_completed_ = false;
	return true;
}

void CCreateNewProjStep1::CreateWnd()
{
	Create(NULL, _T("MM1.0"), UI_WNDSTYLE_DIALOG, WS_EX_STATICEDGE | WS_EX_APPWINDOW, 0, 0, 600, 800);		
	CenterWindow();	
}

//��ʼ������
void CCreateNewProjStep1::InitWindow()
{
	InitCtrl();	
	InitMember();
}

void CCreateNewProjStep1::OnShowSecondStep()
{
	if (NULL  == m_pSecondStepPage)
	{
		m_pSecondStepPage = new CCreateNewProjStep2(m_pPaintManager);
		if (NULL != m_pSecondStepPage)
		{
			m_pSecondStepPage->SetParentWnd(this);
			m_pSecondStepPage->CreateWnd();
			m_pSecondStepPage->Initialize();//��ʼ��һ�Ρ�
		}
	}

	bool bWithProperty = respack_property_combo_->GetCurSel() == 1;
	m_pSecondStepPage->SetResProperty(bWithProperty, m_sLastSavedProjData.strProjectID
		,m_sLastSavedProjData.strProjName, m_sLastSavedProjData.strPackID);

	m_pSecondStepPage->ShowWindow(true);
	//m_pSecondStepPage->SetForegroundWnd();
	ShowWindow(false);
}

CControlUI* CCreateNewProjStep1::CreateControl(LPCTSTR pstrClass)
{
	CTileLayoutResPacket* pResPacketList = new CTileLayoutResPacket(m_pPaintManager);
	if (NULL != pResPacketList)
	{
		m_pListWithProperty = pResPacketList;
		return pResPacketList;
	}

	return NULL;
}

void CCreateNewProjStep1::InitComboProperty()
{	

	//Init comboBox items
	
		CListLabelElementUI* pElement0 = new CListLabelElementUI;
		if (NULL != pElement0)
		{
			pElement0->SetText("������");
			pElement0->SetName("comboitem_noproperty");
			pElement0->SetTag(0);
			respack_property_combo_->Add(pElement0);
		}

		CListLabelElementUI* pElement1 = new CListLabelElementUI;
		if (NULL != pElement1)
		{
			pElement1->SetText("ѯ�۵�");
			pElement1->SetName("comboitem_xjd");
			pElement1->SetTag(1);
			respack_property_combo_->Add(pElement1);
		}

		CListLabelElementUI* pElement2 = new CListLabelElementUI;
		if (NULL != pElement2)
		{
			pElement2->SetText("���۵�");
			pElement2->SetName("comboitem_bjd");
			pElement2->SetTag(2);
			respack_property_combo_->Add(pElement2);
		}

		CListLabelElementUI* pElement3 = new CListLabelElementUI;
		if (NULL != pElement3)
		{
			pElement3->SetText("��ͬ��������");
			pElement3->SetName("comboitem_htbctk");
			pElement3->SetTag(3);
			respack_property_combo_->Add(pElement3);
		}
		CListLabelElementUI* pElement4 = new CListLabelElementUI;
		if (NULL != pElement4)
		{
			pElement4->SetText("��Լ�����޸����滻");
			pElement4->SetName("comboitem_hytkxgyth");
			pElement4->SetTag(4);
			respack_property_combo_->Add(pElement4);
		}
		CListLabelElementUI* pElement5 = new CListLabelElementUI;
		if (NULL != pElement5)
		{
			pElement5->SetText("��Լ�й涨�Ľ����ĵ�");
			pElement5->SetName("comboitem_hyzgddjfwd");
			pElement5->SetTag(5);
			respack_property_combo_->Add(pElement5);
		}
		CListLabelElementUI* pElement6 = new CListLabelElementUI;
		if (NULL != pElement6)
		{
			pElement6->SetText("����֪ͨ��");
			pElement6->SetName("comboitem_fhtzd");
			pElement6->SetTag(6);
			respack_property_combo_->Add(pElement6);
		}
		CListLabelElementUI* pElement7 = new CListLabelElementUI;
		if (NULL != pElement7 )
		{
			pElement7->SetText("���˸�֪��");
			pElement7->SetName("comboitem_fygzd");
			pElement7->SetTag(7);
			respack_property_combo_->Add(pElement7);
		}
		CListLabelElementUI* pElement8 = new CListLabelElementUI;
		if (NULL != pElement8)
		{
			pElement8->SetText("װ�䵥");
			pElement8->SetName("comboitem_zxd");
			pElement8->SetTag(8);
			respack_property_combo_->Add(pElement8);
		}
		CListLabelElementUI* pElement9 = new CListLabelElementUI;
		if (NULL != pElement9)
		{
			pElement9->SetText("�ջ���");
			pElement9->SetName("comboitem_shd");
			pElement9->SetTag(9);
			respack_property_combo_->Add(pElement9);
		}
		CListLabelElementUI* pElement10 = new CListLabelElementUI;
		if (NULL != pElement10 )
		{
			pElement10->SetText("����֪ͨ��");
			pElement10->SetName("comboitem_ystzd");
			pElement10->SetTag(10);
			respack_property_combo_->Add(pElement10);
		}
		CListLabelElementUI* pElement11 = new CListLabelElementUI;
		if (NULL != pElement11)
		{
			pElement11->SetText("����ƾ֤");
			pElement11->SetName("comboitem_fkpz");
			pElement11->SetTag(11);
			respack_property_combo_->Add(pElement11);
		}

		respack_property_combo_->SelectItem(0);
	
}

void CCreateNewProjStep1::InitMember()
{
}

void CCreateNewProjStep1::OnBnClickSelectFiles()
{
	vector<tstring> vecSelectedFiles;
	if (getSelectedFiles(vecSelectedFiles))
	{
		int nSize = vecSelectedFiles.size();
		if (nSize>0)
		{
			for (int i=0; i<nSize; i++)
			{
				tstring filePath = vecSelectedFiles[i];
				tstring strValue = filePath.c_str();
				//��ȡ�ļ���
				//1. ȥ���ļ���
				int iPos=strValue.find_last_of("\\");
				strValue = strValue.substr(iPos + 1); 
				CDuiString strFileFullName = strValue.c_str();
				//2. ȥ���ļ����ͺ�׺
				iPos = strValue.find_last_of('.');
				CDuiString strfileType = strValue.substr(iPos+1).c_str();
				CDuiString strFileName = strValue.substr(0, iPos).c_str();

				fileInfo oneFile;
				oneFile.strFileFullName = strFileFullName;
				oneFile.strFileName = strFileName;
				oneFile.strFilePath = filePath;
				oneFile.strFileType = strfileType;
				oneFile.dwFileSize = getFileSize(vecSelectedFiles[i]);
				if(oneFile.dwFileSize <= 0)
				{
					CMsgBox* pMsgBox = new CMsgBox("�ϴ��ļ�Ϊ��","�ļ����ݲ���Ϊ�գ�", MSGBOX_ICON_ERROR, MSGBOX_IDOK);
					pMsgBox->ShowModal();
					return;
				}
				//
				int nComboSelectItem = respack_property_combo_->GetCurSel();
				//���������ϰ�List
				//if (nComboSelectItem == 0)
				//{
					//m_pListWithNoProperty->AddOneItem(&oneFile);
				//}
				//else
				//{
					m_pListWithProperty->AddOneItem(&oneFile);
				//}
			}
		}	
	}
}


bool CCreateNewProjStep1::OnBnClickSaveProject()
{
	if (CheckIfDataChanged())
	{
		save_btn_->SetEnabled(false);

		if (IsSavingFile())
		{
			CMsgBox* pMsgBox = new CMsgBox("��ʾ", "���ڱ����ļ�����ȴ���", MSGBOX_ICON_WARNING, MSGBOX_IDOK);
			pMsgBox->ShowModal();
			return false;
		}


		//1������ҵ�����ƻ�ȡҵ��ID
		tstring strProjName = new_project_name_edit_->GetText();
		if (strProjName.empty())
		{
			CMsgBox* pMsgBox = new CMsgBox("�½�ҵ��","�½�ҵ�����Ʋ���Ϊ�գ�", MSGBOX_ICON_ERROR, MSGBOX_IDOK);
			pMsgBox->ShowModal();
			return false;
		}
		m_sLastSavedProjData.strProjName = strProjName;

		tstring strResPackName;
		if (NULL != respack_name_edit_)
		{
			strResPackName = respack_name_edit_->GetText();
			if (strResPackName.empty())
			{
				CMsgBox* pMsgBox = new CMsgBox("�½�ҵ��","��Դ�����Ʋ���Ϊ�գ�", MSGBOX_ICON_ERROR, MSGBOX_IDOK);
				pMsgBox->ShowModal();
				return false;
			}
		}
		m_sLastSavedProjData.strResName = strResPackName;


		tstring strMemo;
		if (NULL != new_project_memo_richedit)
		{
			strMemo = new_project_memo_richedit->GetText();
		}
		m_sLastSavedProjData.strMemo = strMemo;

		tstring strProperty;
		if (NULL != respack_property_combo_)
		{
			strProperty = respack_property_combo_->GetText();
		}
		m_sLastSavedProjData.strResProperty = strProperty;
		int nSelectIndex = respack_property_combo_->GetCurSel();

		int nFileCount = m_pListWithProperty->GetCount();
		if (nFileCount == 0)
		{
			CMsgBox* pMsgBox = new CMsgBox("�½���Ŀ","�������Դ�ļ���", MSGBOX_ICON_ERROR, MSGBOX_IDOK);
			pMsgBox->ShowModal();
			return false;
		}

		//�����ļ�size���ܴ���20M
		for (int i=0; i<nFileCount; i++)
		{
			CContainerUI* pListItem = (CContainerUI*)m_pListWithProperty->GetItemAt(i);
			if (NULL != pListItem)
			{
				fileInfo* pFile = (fileInfo*)pListItem->GetTag();
				if (NULL != pFile)
				{
					DWORD dwSize = pFile->dwFileSize;
					if (dwSize>20*1024*1024)
					{
						CMsgBox* pMsgBox = new CMsgBox("�ϴ��ļ�̫��","�����ļ���С���ܳ���20M��", MSGBOX_ICON_ERROR, MSGBOX_IDOK);
						pMsgBox->ShowModal();
						m_bShowSecondPage = false;
						return false;
					}
				}
			}
		}

		if (nFileCount>10)
		{
			CMsgBox* pMsgBox = new CMsgBox("�ϴ��ļ�����","��ѡ����ļ�����10������ɾ�������ļ����ϴ���", MSGBOX_ICON_ERROR, MSGBOX_IDOK);
			pMsgBox->ShowModal();
			return false;
		}

		if (!create_room_completed_)
		{
			//1.����ҵ��
			tstring strUrlNewProj(WEB_SERVER_BASE_URL);
			strUrlNewProj += "business/create-business?name=";
			strUrlNewProj += strProjName;

			CHttpClient reqClient;
			tstring strResponse;
			tstring strToken = CController::Instance()->GetToken();

			tstring strOutUTF8Projname;
			CChineseCode::GB2312ToUTF_8(strOutUTF8Projname, strUrlNewProj.c_str() , strUrlNewProj.length());


			int nSucc = reqClient.Get(strOutUTF8Projname, strToken, strResponse);

			tstring strProjectID;
			if (!strResponse.empty())
			{
				tstring strSub = "\"bid\":\"";
				tstring strEnd = "\"}";

				int nIdx = strResponse.find(strSub);
				int nIdxEnd = strResponse.rfind(strEnd);;
				if (nIdx != -1 && nIdxEnd != -1)
				{
					strProjectID = strResponse.substr(nIdx+strSub.length(), nIdxEnd-(nIdx+strSub.length()));
				}
			}
			else
			{
				return false;
			}

			if (strProjectID.empty())
			{
				CMsgBox* pMsgBox = new CMsgBox("�½���Ŀ","��ȡ��ĿIDʧ�ܣ�", MSGBOX_ICON_ERROR, MSGBOX_IDOK);
				pMsgBox->ShowModal();
				return false;
			}

			m_sLastSavedProjData.strProjectID = strProjectID;

			//2.��ȡһ��PackID
			tstring strFilePackID;
			tstring strUrlGetPackID(WEB_SERVER_BASE_URL);
			strUrlGetPackID += "file/create-file-pack?";
			ostringstream oss;
			oss<<"bid="<<strProjectID<<"&"
				<<"attribute="<<nSelectIndex+1<<"&"
				<<"name="<<strResPackName;
			strUrlGetPackID += oss.str().c_str();
			tstring strOutUTF8;
			CChineseCode::GB2312ToUTF_8(strOutUTF8, strUrlGetPackID.c_str() , strUrlGetPackID.length());

			tstring strGetPackIDResult;
			reqClient.Get(strOutUTF8,strToken, strGetPackIDResult);
			if (!strGetPackIDResult.empty() && strGetPackIDResult.find("id") != -1)
			{
				Json::Reader reader;
				Json::Value root;
				try
				{
					if (reader.parse(strGetPackIDResult, root))
					{
						strFilePackID = root["id"].asString();
					}
				}
				catch (...)
				{
					CLogger::GetInstance()->PrintErrLog( "parse http Response exception: %s", strResponse.c_str());	
				}	
			}

			if (strFilePackID.empty())
			{
				CMsgBox* pMsgBox = new CMsgBox("�½���Ŀ","��ȡ�ļ�PackIDʧ�ܣ�", MSGBOX_ICON_ERROR, MSGBOX_IDOK);
				pMsgBox->ShowModal();
				return false;
			}
			m_sLastSavedProjData.strPackID = strFilePackID;
		}	

		//�ڴ���ҵ��֮��Ψһ���Ը��ĵ���ҵ�����������ļ�
		if (nFileCount>0)
		{
			for (int i=0; i<nFileCount; i++)
			{
				CContainerUI* pListItem = (CContainerUI*)m_pListWithProperty->GetItemAt(i);
				if (NULL != pListItem)
				{
					fileInfo* pFile = (fileInfo*)pListItem->GetTag();
					if (NULL != pFile)
					{
						tstring uploadName = pFile->strFileFullName;          //
						DWORD size = pFile->dwFileSize;
						CProgressUI*pProgressBar  = static_cast<CProgressUI*>(m_pPaintManager->FindSubControlByName(pListItem, _T("progress_res_file")));
						if (NULL != pProgressBar)
						{
							// �Ѿ��ϴ��ľͲ����ϴ��ˡ�
							if (pProgressBar->GetValue() == 100)
							{
								continue;
							}
						}
						m_sLastSavedProjData.mapFile2Size.insert(make_pair(uploadName, size));


						uploadFiles(m_sLastSavedProjData.strProjectID, m_sLastSavedProjData.strPackID, *pFile, this);
					}
				}
			}
		}

		create_room_completed_ = true;
		return true;

		save_btn_->SetEnabled(true);
	}	
}

bool CCreateNewProjStep1::uploadFiles( tstring& strProjectID, tstring& strPackID, fileInfo& lstFiles, IOssEventBase* pEventHandle)
{
	HANDLE   hThread;  
	unsigned   threadID;   

	sTmpInfo* pTmp = new sTmpInfo;
	pTmp->pFile = &lstFiles;
	pTmp->pThis = (void*)this;
	pTmp->strProjectID = strProjectID;
	pTmp->strPackID = strPackID;
	pTmp->pEventHandle = pEventHandle;
	hThread = (HANDLE)_beginthreadex(NULL,0,uploadThreadFunc,pTmp,0,&threadID);
	CloseHandle(hThread); 
	return true;
}

unsigned  __stdcall  CCreateNewProjStep1::uploadThreadFunc( void*pArguments)
{
	sTmpInfo* pTmp = (sTmpInfo*)pArguments;
	if (NULL != pTmp && NULL != pTmp->pThis)
	{
		((CCreateNewProjStep1*)(pTmp->pThis))->uploadSignalFile(pTmp->strProjectID, pTmp->strPackID, *(pTmp->pFile), pTmp->pEventHandle);
	} 
	return   0;  
}

bool CCreateNewProjStep1::uploadSignalFile( tstring& strProjectID, tstring& strPackID, fileInfo& infoFile, IOssEventBase* pEventHandle)
{
	
	//2.�ϴ���������
	tstring strEndPoint("oss-cn-hangzhou.aliyuncs.com");
	tstring strAcessID("nHGOOiBnBsJCs04n");
	tstring strAcessIDSecret("Yod3tgIrcqVLpwuIk6yxRJKm56pvXr");
	tstring strBucket("jp-mm");

	ossInterface* pOssControl = new ossInterface;
	pOssControl->AddEventHandler(pEventHandle);
	pOssControl->setAcessEndpoint(strEndPoint);
	pOssControl->setAcessID(strAcessID);
	pOssControl->setAcessIDSecret(strAcessIDSecret);
	pOssControl->setBucketName(strBucket);
	pOssControl->initOSS();

	CHttpClient reqClient;
	tstring strResponse;

	tstring strToken = CController::Instance()->GetToken();

	tstring strOut;
	CChineseCode::GB2312ToUTF_8(strOut, infoFile.strFilePath.c_str(), infoFile.strFilePath.length());

	tstring uploadName = infoFile.strFileFullName;          //
	DWORD sz = infoFile.dwFileSize;

	tstring strObject;
	CChineseCode::GB2312ToUTF_8(strObject, uploadName.c_str(), uploadName.length());

	sTaskItem oneItem;
	oneItem.pOssObject = pOssControl;
	oneItem.bSaving    = true;
	oneItem.strPathFile = infoFile.strFilePath;
	oneItem.strFileName = infoFile.strFileFullName;
	oneItem.hExitEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	ResetEvent(oneItem.hExitEvent);

	::EnterCriticalSection(&m_csUploadFile);
	m_mapSaveFileState.insert(make_pair(oneItem.strPathFile, oneItem));
	::LeaveCriticalSection(&m_csUploadFile);


	int res = pOssControl->uploadPartFromFileEx(strObject, strOut, sz, uploadName/*strObject*/, tstring(""));
	if (res == OSS_SUCCESS)
	{
		//��ȡ������ url
		tstring struploadName = CChineseCode::EncodeUTF8(uploadName);
		tstring strUrl = pOssControl->getFileUrl(struploadName);   //todo encode
		strUrl = UrlEncode(strUrl);

		tstring strFileFullName = infoFile.strFileFullName;
		tstring strParam(WEB_SERVER_BASE_URL);
		strParam += "file/save-pack-files";

		ostringstream oss;
		oss<<sz;
		tstring strSize = oss.str();

		tstring strConverted;
		if (strFileFullName.find("&") != -1)
		{
			strConverted = UrlEncode(strFileFullName);
		}
		else
		{
			strConverted = strFileFullName;
		}
		

		tstring strPost = "bid=" + strProjectID + "&pack_id="+strPackID + "&file_datas="
			+"[{" + '"'+"file_name" +'"'+":" + '"'+strConverted.c_str()+'"' +','+"\"file_size\":\""
			+ strSize.c_str() +"\"" +',' + '"'+ "file_type"+'"' +":" + '"'+infoFile.strFileType+'"'+',' 
			+'"'+ "ali_file_url"+'"'+ ":" + '"'+strUrl+'"' + "}]";

		int nResult=0;
		reqClient.Post(strParam,CChineseCode::EncodeUTF8(strPost), strToken, strResponse );	
		if (!strResponse.empty() && strResponse.find("result") != -1)      //����web���ݿ�ʧ�� 
		{
			Json::Reader reader;
			Json::Value root;
			if (reader.parse(strResponse, root))
			{
				nResult = root["result"].asInt();
			}
		}

		if (nResult == 1)
		{
		}
		else
		{
			tstring strError("�����ļ����ϣ�[");
			strError += uploadName;
			strError += "]ʧ�ܣ�";
			CMsgBox* pMsgBox = new CMsgBox("�½���Ŀ",strError.c_str(), MSGBOX_ICON_ERROR, MSGBOX_IDOK);
			pMsgBox->ShowModal();
		}
	}

	pOssControl->RemoveHandler(pEventHandle);

	if (res == OSS_SUCCESS)
	{
		
	}
	else
	{
		//��֪�߳��˳��ˡ�
		SetEvent(oneItem.hExitEvent);
	}

	//�޸�״̬��
	::EnterCriticalSection(&m_csUploadFile);
	for (map<tstring, sTaskItem>::iterator it = m_mapSaveFileState.begin();
		it != m_mapSaveFileState.end(); ++it)
	{
		if (_tcsicmp(it->second.strFileName.c_str(), uploadName.c_str()) == 0)
		{
			it->second.bSaving = false;
		}
	}
	::LeaveCriticalSection(&m_csUploadFile);

	bool bSaving = IsSavingFile();

	if (!bSaving)
	{
		//֪ͨ���״���ȥˢ���£���Ŀ�б�
		::PostMessage(m_hMainWnd, WM_USER+912, 0, 0);

		//�����ǰ�ǡ����桱����
		if (!m_bShowSecondPage)
		{
			//�����ϰ�״̬����Ϊ��1���ղ�����
			CHttpClient reqClient;
			tstring strResponse;
			tstring strToken = CController::Instance()->GetToken();

			//����pack״̬
			tstring strUrlUpdatePack(WEB_SERVER_BASE_URL);
			strUrlUpdatePack += "file/update-pack-status?pack_id=";
			strUrlUpdatePack += strPackID;
			strUrlUpdatePack += "&status=";
			strUrlUpdatePack += "1";

			tstring strOutUTF8Projname;
			CChineseCode::GB2312ToUTF_8(strOutUTF8Projname, strUrlUpdatePack.c_str() , strUrlUpdatePack.length());
			int nSucc = reqClient.Get(strOutUTF8Projname, strToken, strResponse);
			if ( strResponse.find("1") != -1)
			{
				//return true;
			}
			else
			{
				CMsgBox* pMsgBox = new CMsgBox("����","�޸ĸ����ϰ�״̬ʧ�ܣ�");
				pMsgBox->ShowModal();
				//return false;			
			}


			//�رյ�ǰstep 1���ڡ�
			PostMessage(WM_USER+119);
		}
		//�����ǰ�ǡ�ѡ����ϵ�ˡ����������޸�����״̬Ϊ0.
		else
		{
			//�����ϰ�״̬����Ϊ��1���ղ�����
			CHttpClient reqClient;
			tstring strResponse;
			tstring strToken = CController::Instance()->GetToken();

			//����pack״̬
			tstring strUrlUpdatePack(WEB_SERVER_BASE_URL);
			strUrlUpdatePack += "file/update-pack-status?pack_id=";
			strUrlUpdatePack += strPackID;
			strUrlUpdatePack += "&status=";
			strUrlUpdatePack += "0";

			tstring strOutUTF8Projname;
			CChineseCode::GB2312ToUTF_8(strOutUTF8Projname, strUrlUpdatePack.c_str() , strUrlUpdatePack.length());
			int nSucc = reqClient.Get(strOutUTF8Projname, strToken, strResponse);
			if ( strResponse.find("1") != -1)
			{
				return true;
			}
			else
			{
				CMsgBox* pMsgBox = new CMsgBox("����","�޸ĸ����ϰ�״̬ʧ�ܣ�");
				pMsgBox->ShowModal();
				return false;			
			}
		}
	}
		
	return res == OSS_SUCCESS;
}

void CCreateNewProjStep1::OnProcess(tstring& fileName, int iValue, tstring &strKey)
{
	if (NULL != m_pListWithProperty)
	{
		m_pListWithProperty->SetProgress(fileName, iValue);
	}
}

LRESULT CCreateNewProjStep1::HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	//WM_SHOW_STEP1_WND�������Ǵ�step2�ٷ��ص�step1��
	//���ｫ�����޸�ҵ�����ƺ����ϰ�����,��Ϊû�а취ɾ���Ѿ��½���ҵ������ϰ�
	if (uMsg == WM_SHOW_STEP1_WND)
	{		
		new_project_name_edit_->SetEnabled(false);		
		respack_name_edit_->SetEnabled(false);

		ShowWindow(true);
		SetForegroundWnd();
	}
	else if (WM_TIMER == uMsg && wParam == MW_TIMER_TOPWND)
	{
		::SetWindowPos(this->GetHWND(), HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
		KillTimer(this->GetHWND(), MW_TIMER_TOPWND);
	}
	else if (uMsg == WM_STEP2_CLOSED)
	{
		m_pSecondStepPage = NULL;

		//��֪���������ö���
		if (NULL != m_hMainWnd)
		{
			::SendMessage(m_hMainWnd, WM_PROJECT_WND_CLOSED, 0, 0);
		}

		//�����Լ���
		Close();
	}
	else if (uMsg == WM_USER+119)
	{
		//�����Ӵ���
		if (NULL != m_pSecondStepPage)
		{
			m_pSecondStepPage->SendMessage(WM_CLOSE);
			m_pSecondStepPage = NULL;
		}

		//��֪���������ö���
		if (NULL != m_hMainWnd)
		{
			::PostMessage(m_hMainWnd, WM_PROJECT_WND_CLOSED, 0, 0);
		}

		//������Դ
		Close();
	}

	
	return __super::HandleCustomMessage(uMsg, wParam, lParam, bHandled);
}

void CCreateNewProjStep1::SetForegroundWnd()
{
	::SetWindowPos(this->GetHWND(), HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
	SetTimer(this->GetHWND(), MW_TIMER_TOPWND, 50, NULL);
}

bool CCreateNewProjStep1::IsSavingFile()
{
	::EnterCriticalSection(&m_csUploadFile);
	bool bSavingfile = false;
	for (map<tstring, sTaskItem>::iterator it = m_mapSaveFileState.begin();
		it != m_mapSaveFileState.end(); ++it)
	{
		if (it->second.bSaving)
		{
			bSavingfile = true;
			break;
		}
	}
	::LeaveCriticalSection(&m_csUploadFile);

	return bSavingfile;
}

bool CCreateNewProjStep1::ClearTaskInfo(tstring& strFullPath)
{
	bool bSucc = false;

	::EnterCriticalSection(&m_csUploadFile);
	for (map<tstring, sTaskItem>::iterator it = m_mapSaveFileState.begin();
		it != m_mapSaveFileState.end(); ++it)
	{
		if (_tcsicmp(it->second.strPathFile.c_str(), strFullPath.c_str()) == 0)
		{
			if (it->second.pOssObject != NULL)
			{
				//�������ִ�У�����ֹ������������������Ϣ
				if (it->second.bSaving)
				{
					it->second.pOssObject->StopCurrentTask();
					if(WaitForSingleObject(it->second.hExitEvent, INFINITE) == WAIT_OBJECT_0)
					{
						bSucc = true;
					}
				}

				delete it->second.pOssObject;
				it->second.pOssObject = NULL;
				m_mapSaveFileState.erase(it);

				break;
			}	
		}
	}
	::LeaveCriticalSection(&m_csUploadFile);

	return bSucc;
}

void CCreateNewProjStep1::OnBnClickRemoveOneFile(CControlUI* pControl)
{
	CContainerUI* pContainer = (CContainerUI*)(pControl->GetParent());
	if (NULL != pContainer)
	{
		//for test.
		//int nTag = pContainer->GetTag();
		//int nIndex = m_pResFileItem->GetItemIndex(pContainer);
		if (NULL != pContainer->GetParent())
		{
			//���task��Ϣ
			tstring strName = pContainer->GetParent()->GetName();
			fileInfo* pFile = (fileInfo*)(pContainer->GetTag());
			if(ClearTaskInfo(pFile->strFilePath))
			{
			}
			
			//����������������ļ��б�
			for (map<tstring, DWORD>::iterator it = m_sLastSavedProjData.mapFile2Size.begin();
				it != m_sLastSavedProjData.mapFile2Size.end(); ++it)
			{
				if (_tcsicmp(it->first.c_str(), pFile->strFileFullName.c_str()) == 0)
				{
					m_sLastSavedProjData.mapFile2Size.erase(it);
					break;
				}
			}

			delete pFile;
			pFile = NULL;

			((CContainerUI*)pContainer->GetParent())->Remove(pContainer);
		}
	}
}

bool CCreateNewProjStep1::IsParamEmpty()
{
	tstring strProjName = new_project_name_edit_->GetText();
	if (strProjName.empty())
	{
		CMsgBox* pMsgBox = new CMsgBox("�½�ҵ��","�½�ҵ�����Ʋ���Ϊ�գ�", MSGBOX_ICON_ERROR, MSGBOX_IDOK);
		pMsgBox->ShowModal();
		return false;
	}
	
	tstring strResPackName;
	if (NULL != respack_name_edit_)
	{
		strResPackName = respack_name_edit_->GetText();
		if (strResPackName.empty())
		{
			CMsgBox* pMsgBox = new CMsgBox("�½�ҵ��","��Դ�����Ʋ���Ϊ�գ�", MSGBOX_ICON_ERROR, MSGBOX_IDOK);
			pMsgBox->ShowModal();
			return false;
		}
	}

	int nFileCount = m_pListWithProperty->GetCount();
	if (nFileCount == 0)
	{
		CMsgBox* pMsgBox = new CMsgBox("�½���Ŀ","�������Դ�ļ���", MSGBOX_ICON_ERROR, MSGBOX_IDOK);
		pMsgBox->ShowModal();
		return false;
	}

	return true;
}

//��ʼ���ؼ�
void CCreateNewProjStep1::InitCtrl()
{
	save_btn_ = static_cast<CButtonUI*>(m_PaintManager.FindControl(_T("btn_save_resource")));
	new_project_name_edit_ = static_cast<CEditUI*>(m_PaintManager.FindControl(_T("edit_new_project_name")));
	respack_property_combo_ = static_cast<CComboUI*>(m_PaintManager.FindControl(_T("combo_res_property")));
	InitComboProperty();//��ʼ��combo�б�
	respack_name_edit_ = static_cast<CEditUI*>(m_PaintManager.FindControl(_T("edit_res_name")));
	take_photo_btn = static_cast<CButtonUI*>(m_PaintManager.FindControl(_T("btn_new_proj_takephoto")));
	select_file_btn_ = static_cast<CButtonUI*>(m_PaintManager.FindControl(_T("btn_new_proj_files")));
	capture_btn_ = static_cast<CButtonUI*>(m_PaintManager.FindControl(_T("btn_new_proj_capture")));
	collection_btn_ = static_cast<CButtonUI*>(m_PaintManager.FindControl(_T("btn_new_proj_collection")));
	select_car_btn_ = static_cast<CButtonUI*>(m_PaintManager.FindControl(_T("btn_tool_selectcar")));
	vin_btn_ = static_cast<CButtonUI*>(m_PaintManager.FindControl(_T("btn_tool_VIN")));
	delete_respack1_btn_ = static_cast<CButtonUI*>(m_PaintManager.FindControl(_T("btn_delete_select_res")));
//	close_btn_ = static_cast<CButtonUI*>(m_PaintManager.FindControl(_T("btn_close_create_proj")));
	select_friend_btn_ = static_cast<CButtonUI*>(m_PaintManager.FindControl(_T("btn_select_friend")));
//	delete_res_item_btn_ = static_cast<CButtonUI*>(m_PaintManager.FindControl(_T("btn_delete_askfileitem")));
	new_project_memo_richedit = static_cast<CRichEditUI*>(m_PaintManager.FindControl(_T("richedit_create_newproj_memo")));	
}

//ɾ���ؼ�
void CCreateNewProjStep1::DeleteCtrl()
{
	if (save_btn_)
	{
		delete save_btn_;
		save_btn_ = NULL;
	}
	if (new_project_name_edit_)
	{
		delete new_project_name_edit_;
		new_project_name_edit_ = NULL;
	}
	if (respack_property_combo_)
	{
		delete respack_property_combo_;
		respack_property_combo_ = NULL;
	}
	if (respack_name_edit_)
	{
		delete respack_name_edit_;
		respack_name_edit_ = NULL;
	}
	if (take_photo_btn)
	{
		delete take_photo_btn;
		take_photo_btn = NULL;
	}
	if (select_file_btn_)
	{
		delete select_file_btn_;
		select_file_btn_ = NULL;
	}
	if (capture_btn_)
	{
		delete capture_btn_;
		capture_btn_ = NULL;
	}
	if (collection_btn_)
	{
		delete collection_btn_;
		collection_btn_ = NULL;
	}
	if (select_car_btn_)
	{
		delete select_car_btn_;
		select_car_btn_ = NULL;
	}
	if (vin_btn_)
	{
		delete vin_btn_;
		vin_btn_ = NULL;
	}
	if (delete_respack1_btn_)
	{
		delete delete_respack1_btn_;
		delete_respack1_btn_ = NULL;
	}
	if (close_btn_)
	{
		delete close_btn_;
		close_btn_ = NULL;
	}
	if (select_friend_btn_)
	{
		delete select_friend_btn_;
		select_friend_btn_ = NULL;
	}
// 	if (delete_res_item_btn_)
// 	{
// 		delete delete_res_item_btn_;
// 		delete_res_item_btn_ = NULL;
// 	}
	if (new_project_memo_richedit)
	{
		delete new_project_memo_richedit;
		new_project_memo_richedit = NULL;
	}
}

bool CCreateNewProjStep1::OnBnClickClose()
{
	if (IsSavingFile())
	{
		CMsgBox* pMsgBox = new CMsgBox("����", "���ڱ����ļ�����ȴ���", MSGBOX_ICON_WARNING, MSGBOX_IDYES);
		pMsgBox->ShowModal();
		return true;
	}

	//��������Ƿ��б仯���Ƿ���Ҫ��������
	if (CheckIfDataChanged())
	{
		CMsgBox* pMsgBox = new CMsgBox("����", "����δ�����������Ϣ���Ƿ񱣴棿", MSGBOX_ICON_WARNING, MSGBOX_IDYESNO);
		if (MSGBOX_IDYES == pMsgBox->ShowModal())
		{
//			close_btn_->SetEnabled(false);
			OnBnClickSaveProject();
//			close_btn_->SetEnabled(true);
			return true;
		}
	}

	//�����Ӵ���
	if (NULL != m_pSecondStepPage)
	{
		m_pSecondStepPage->SendMessage(WM_CLOSE);
		m_pSecondStepPage = NULL;
	}

	//��֪���������ö���
	if (NULL != m_hMainWnd)
	{
		::PostMessage(m_hMainWnd, WM_PROJECT_WND_CLOSED, 0, 0);
	}

	//������Դ
	Close();
	return true;
}

void CCreateNewProjStep1::OnBnClickSelectCar()
{
	tstring uid,sid;
	CWebClient::GetUidAndSid(uid, sid);
	tstring strUrlColl(WEB_ROOT);
	strUrlColl += "user/csslogin/index?uid=";
	strUrlColl += uid;
	strUrlColl += "&sid=";
	strUrlColl += sid;
	strUrlColl += "&referUrl=/";//finance/index/index";
	strUrlColl += REFER_MOTOR_TYPE;			

	CssWeb::Instance()->SetParams(WEB_MOTOR_TYPE, strUrlColl);
	CssWeb::Instance()->ShowWindow(true);			
}

void CCreateNewProjStep1::OnBnClickVin()
{
	tstring uid,sid;
	CWebClient::GetUidAndSid(uid, sid);
	tstring strUrlColl(WEB_ROOT);
	strUrlColl += "user/csslogin/index?uid=";
	strUrlColl += uid;
	strUrlColl += "&sid=";
	strUrlColl += sid;
	strUrlColl += "&referUrl=/";//finance/index/index";
	strUrlColl += REFER_VIN;			

	CssWeb::Instance()->SetParams(WEB_VIN, strUrlColl);
	CssWeb::Instance()->ShowWindow(true);	
}

void CCreateNewProjStep1::OnBnClickSelectFriend()
{
	//����ǿյģ�Ҳ���Ѳ�������
	if (!IsParamEmpty())
	{
		return;
	}

	if (IsSavingFile())
	{
		CMsgBox* pMsgBox = new CMsgBox("����", "���ڱ����ļ�����ȴ���", MSGBOX_ICON_WARNING, MSGBOX_IDYES);
		pMsgBox->ShowModal();
		return;
	}


	if (CheckIfDataChanged())
	{
		CMsgBox* pMsgBox = new CMsgBox("����", "����δ�����������Ϣ���Ƿ񱣴棿", MSGBOX_ICON_WARNING, MSGBOX_IDYESNO);
		if (MSGBOX_IDYES == pMsgBox->ShowModal())
		{
			m_bShowSecondPage = false;
			if (OnBnClickSaveProject())
			{
				m_bShowSecondPage = true;
			}
		}
	}

	if (m_bShowSecondPage)
	{
		OnShowSecondStep();
	}
}

void CCreateNewProjStep1::OnDeleteRespack1()
{
	int nCount = m_pListWithProperty->GetCount();
	for (int i=0; i<nCount;i++)
	{
		CContainerUI* pOneFileItem = static_cast<CContainerUI*>(m_pListWithProperty->GetItemAt(i));
		if (NULL != pOneFileItem)
		{
			fileInfo* pFile = NULL;
			pFile = (fileInfo*)(pOneFileItem->GetTag());
			if (NULL == pFile)
			{
				continue;
			}

			tstring strFullFileName = pFile->strFileFullName;
			tstring strFilepath = pFile->strFilePath;
			if(ClearTaskInfo(strFilepath))
			{

			}

			//����������������ļ��б�
			for (map<tstring, DWORD>::iterator it = m_sLastSavedProjData.mapFile2Size.begin();
				it != m_sLastSavedProjData.mapFile2Size.end(); ++it)
			{
				if (_tcsicmp(it->first.c_str(), strFullFileName.c_str()) == 0)
				{
					m_sLastSavedProjData.mapFile2Size.erase(it);
					break;
				}
			}

			//delete pFile;
			//pFile = NULL;
		}

	}


	if (NULL != m_pListWithProperty)
	{
		m_pListWithProperty->RemoveAll();
	}
}
