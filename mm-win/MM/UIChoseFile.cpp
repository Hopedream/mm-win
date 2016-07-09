#include "UIChoseFile.h"
#include <CommDlg.h>
#include "UIUpdateFiles.h"
#include "mainDlg.h"
void UIChoseFile::Notify( TNotifyUI& msg )
{
	if (_tcsicmp(msg.sType, _T("click")) == 0)
	{
		if (_tcsicmp(msg.pSender->GetName(), _T("buttonClose")) == 0)
		{
			mainDlg::getInstance()->setLimitChoseFile(false);
			Close();
		}
	}
}



