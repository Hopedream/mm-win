#include "TileLayoutResPacket.h"
#include "CDialogBuilderCallbackEx.h"
#include "Utility.h"


CTileLayoutResPacket::CTileLayoutResPacket(CPaintManagerUI* ppm)
{
	m_pPaintManager = ppm;
// 	SetItemSize(CDuiSize(130, 50));
// 
// 	CDialogBuilder builder;
// 	CContainerUI* pOneFile = static_cast<CContainerUI*>(builder.Create(_T("create_proj_askprice_packitem.xml"), (UINT)0));
// 	if( pOneFile != NULL ) 
// 	{
// 		for(int i = 0; i < 5; ++i) 
// 		{
// 			if( pOneFile == NULL ) 
// 				pOneFile = static_cast<CContainerUI*>(builder.Create());
// 
// 			if( pOneFile != NULL ) 
// 			{
// 				pOneFile->SetTag(GetNextItemID());
// 				this->Add(pOneFile);
// 				
// 				pOneFile = NULL;
// 			}
// 			else 
// 			{
// 				this->RemoveAll();
// 				return;
// 			}
// 		}
// 	}
}

CTileLayoutResPacket::~CTileLayoutResPacket()
{

}

void CTileLayoutResPacket::DoEvent(TEventUI& event)
{
	CTileLayoutUI::DoEvent(event);
}

void CTileLayoutResPacket::AddOneItem(fileInfo* pFile,bool isVisible)
{
	SetItemSize(CDuiSize(130, 65));

	CDialogBuilder builder;
	CContainerUI* pOneFile = static_cast<CContainerUI*>(builder.Create(_T("create_proj_askprice_packitem.xml"), (UINT)0));
	if( pOneFile != NULL ) 
	{	
		if( pOneFile == NULL ) 
			pOneFile = static_cast<CContainerUI*>(builder.Create());

		if( pOneFile != NULL ) 
		{
			fileInfo* pNew = new fileInfo;
			pNew->dwFileSize = pFile->dwFileSize;
			pNew->strFileFullName = pFile->strFileFullName;
			pNew->strFileName = pFile->strFileName;
			pNew->strFilePath = pFile->strFilePath;
			pNew->strFileType = pFile->strFileType;
			pNew->strFileUrl = pFile->strFileUrl;
			
			pOneFile->SetTag((UINT_PTR)pNew);

			this->Add(pOneFile);
		}
		else 
		{
			this->RemoveAll();
			return;
		}

		//1. 设置icon
		CControlUI* pFileIcon = static_cast<CControlUI*>(m_pPaintManager->FindSubControlByName(pOneFile, _T("control_file_image")));
		if (NULL != pFileIcon)
		{
			pFileIcon->SetBkImage(GetDefaultFileIcon(pFile->strFileType).c_str());
		}

		//2. 设置名字
		CLabelUI *pLableFileName = static_cast<CLabelUI*>(m_pPaintManager->FindSubControlByName(pOneFile, _T("label_askfile_name")));
		if (NULL != pLableFileName)
		{
			pLableFileName->SetText(pFile->strFileName.c_str());
		}
		
		//3. 设置文件大小
		CLabelUI* pLabelFileSize = static_cast<CLabelUI*>(m_pPaintManager->FindSubControlByName(pOneFile, _T("label_askfile_size")));
		if (NULL != pLabelFileSize)
		{
			pLabelFileSize->SetText(GetFileSizeFormat(pFile->dwFileSize).c_str());
		}

		//4. 设置删除按钮状态
		CButtonUI* pBtn = static_cast<CButtonUI*>(m_pPaintManager->FindSubControlByName(pOneFile, _T("btn_delete_askfileitem")));
		if (NULL != pBtn)
		{
			pBtn->SetVisible(isVisible);
		}

		//5. 设置进度条状态
		CProgressUI* pPro = static_cast<CProgressUI*>(m_pPaintManager->FindSubControlByName(pOneFile, _T("progress_res_file")));
		if (NULL != pPro)
		{
			pPro->SetVisible(isVisible);
		}
	}
}

void CTileLayoutResPacket::SetProgress(tstring& strFile, int nValue)
{
	int nCount = GetCount();
	if (nCount>0)
	{
		for (int i=0; i<nCount; i++)
		{
			CContainerUI* pListItem = (CContainerUI*)GetItemAt(i);
			if (NULL != pListItem)
			{
				fileInfo* pFile = (fileInfo*)pListItem->GetTag();
				if (NULL != pFile)
				{
					tstring strFileName = pFile->strFileFullName;          //
					if (_tcsicmp(strFileName.c_str(), strFile.c_str()) == 0)
					{
						//3. 设置文件大小
						CProgressUI*pProgressBar  = static_cast<CProgressUI*>(m_pPaintManager->FindSubControlByName(pListItem, _T("progress_res_file")));
						if (NULL != pProgressBar)
						{
							if (nValue == 98)
							{
								nValue = 100;
							}

							pProgressBar->SetValue(nValue);
						}
					}
				}
			}

		}
	}
	
}