#include "TileLayoutGroupPackFileList.h"
#include "CDialogBuilderCallbackEx.h"
#include "Utility.h"
#include "FileHelper.h"

CTileLayoutGroupPackList::CTileLayoutGroupPackList(CPaintManagerUI* ppm)
{
	m_pPaintManager = ppm;
}

CTileLayoutGroupPackList::~CTileLayoutGroupPackList()
{

}

void CTileLayoutGroupPackList::DoEvent(TEventUI& event)
{
	CTileLayoutUI::DoEvent(event);
}

//ע��:���ڸ���Ļ�����TileLayout������create_project_opposite_employee.xml�ĸ��ڵ㲻������ListContainerElement����Ӧ����Container�ڵ㡣��������ˢ��Listʧ�ܵ����⡣
void CTileLayoutGroupPackList::AddOneItem(sPackFileItem& onePackFile)
{
	SetItemSize(CDuiSize(500, 66));

	CDialogBuilder builder;
	CContainerUI* pPackFileListItem = static_cast<CContainerUI*>(builder.Create(_T("download_packfile_item.xml"), (UINT)0));
	if( pPackFileListItem != NULL ) 
	{	
		if( pPackFileListItem == NULL ) 
			pPackFileListItem = static_cast<CContainerUI*>(builder.Create());

		if( pPackFileListItem != NULL ) 
		{
			CControlUI* pFileIcon = static_cast<CControlUI*>(m_pPaintManager->FindSubControlByName(pPackFileListItem, "gc_downloadfile_icon"));
			if (NULL != pFileIcon)
			{
				pFileIcon->SetBkImage(GetPackFileIcon(onePackFile.strFileType).c_str());
			}

			CControlUI* pFileName = static_cast<CControlUI*>(m_pPaintManager->FindSubControlByName(pPackFileListItem, "gc_downloadfile_name"));
			if (NULL != pFileName)
			{
				pFileName->SetText(onePackFile.strFileName.c_str());
			}

			CControlUI* pFileSize = static_cast<CControlUI*>(m_pPaintManager->FindSubControlByName(pPackFileListItem, "gc_downloadfile_size"));
			if (NULL != pFileSize)
			{
				pFileSize->SetText(onePackFile.strFileSizeShow.c_str());
			}

			//��ʼ��Ϊ���õģ�ֻ��������ɾ�����Ϊenabled״̬
			CControlUI* pOpenBtn = static_cast<CControlUI*>(m_pPaintManager->FindSubControlByName(pPackFileListItem,"btn_open_downloaded_file"));
			if (NULL != pOpenBtn)
			{
				pOpenBtn->SetEnabled(false);
			}
	
			//Ϊ�˸��½���,�򿪣������ļ�ȫ·������Ϊtag��
			tstring strLocalFullPath = CFileHelper::GetPackDir(onePackFile.strPackName, onePackFile.strPackID);
			strLocalFullPath += onePackFile.strFileName;
			int nLen = strLocalFullPath.length();
			char* pBuff = new char[nLen+1];
			memset(pBuff, 0, nLen+1);
			strcpy(pBuff, strLocalFullPath.c_str());

			pPackFileListItem->SetTag((UINT_PTR)pBuff);

			this->Add(pPackFileListItem);
		}
		else 
		{
			this->RemoveAll();
			return;
		}
	}
}

void CTileLayoutGroupPackList::SetProgress(tstring& strFullpathFile, int nValue)
{
	int nCount = GetCount();
	if (nCount>0)
	{
		for (int i=0; i<nCount; i++)
		{
			CContainerUI* pListItem = (CContainerUI*)GetItemAt(i);
			if (NULL != pListItem)
			{
				char* pFileBuff = (char*)pListItem->GetTag();
				tstring strFileName = pFileBuff;
				if (!strFileName.empty())
				{
					if (_tcsicmp(strFileName.c_str(), strFullpathFile.c_str()) == 0)
					{
						//3. �����ļ���С
						CProgressUI*pProgressBar  = static_cast<CProgressUI*>(m_pPaintManager->FindSubControlByName(pListItem, _T("download_packfile_progress")));
						if (NULL != pProgressBar)
						{
							if (nValue == 99 || nValue == 98)
							{
								nValue = 100;
							}

							pProgressBar->SetValue(nValue);
							
							//�������ˣ����ؽ�����������
							if (nValue == 100)
							{
								pProgressBar->SetVisible(false);

								CControlUI* pOpenBtn = static_cast<CControlUI*>(m_pPaintManager->FindSubControlByName(pListItem,"btn_open_downloaded_file"));
								if (NULL != pOpenBtn)
								{
									pOpenBtn->SetEnabled(true);
								}
							}
						}
					}
				}
			}

		}
	}

}

void CTileLayoutGroupPackList::DeSelectItem(tstring& strAccount)
{
	int nCount = GetCount();
	if (nCount>0)
	{
		for (int i=0; i<nCount; i++)
		{
			CContainerUI* pListItem = static_cast<CContainerUI*>(GetItemAt(i));
			if (NULL != pListItem)
			{
				sFriendListItem* pFriend = (sFriendListItem*)pListItem->GetTag();
				if(_tcsicmp(pFriend->strAccount.c_str(), strAccount.c_str()) == 0)
				{
					COptionUI* pOption = static_cast<COptionUI*>(m_pPaintManager->FindSubControlByName(pListItem, _T("option_oppo_employe_selected")));
					if (NULL != pOption)
					{
						pOption->Selected(false);
						break;
					}
				}
			}
		}
	}
}

void CTileLayoutGroupPackList::SetItemSeleted(tstring& strAccount)
{
	int nCount = GetCount();
	if (nCount>0)
	{
		for (int i=0; i<nCount; i++)
		{
			CContainerUI* pListItem = static_cast<CContainerUI*>(GetItemAt(i));
			if (NULL != pListItem)
			{
				sFriendListItem* pFriend = (sFriendListItem*)pListItem->GetTag();
				if(_tcsicmp(pFriend->strAccount.c_str(), strAccount.c_str()) == 0)
				{
					COptionUI* pOption = static_cast<COptionUI*>(m_pPaintManager->FindSubControlByName(pListItem, _T("option_oppo_employe_selected")));
					if (NULL != pOption)
					{
						pOption->Selected(true);
						break;
					}
				}
			}
		}
	}
}