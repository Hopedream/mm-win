#pragma once
// 此处需要包含duilib的头文件#include <UIlib.h>那一段，详细代码请见前面教程
#include "StdAfx.h"
#include "UIDefine.h"



class UIChoseFile : public WindowImplBase
{
protected:
	virtual ~UIChoseFile(){};        // 私有化析构函数，这样此对象只能通过new来生成，而不能直接定义变量。就保证了delete this不会出错
	CDuiString  m_strXMLPath;
	HWND* m_phWndParent;
	
private:
	//bool getSelectedFiles(vector<tstring> &);
	
private:

public:

	explicit UIChoseFile(LPCTSTR pszXMLPath): m_strXMLPath(pszXMLPath){}
	
	virtual LPCTSTR    GetWindowClassName()const{ return _T("UIChoseFile "); }
	virtual CDuiString GetSkinFolder()          { return _T("");			}
	virtual CDuiString GetSkinFile()            { return m_strXMLPath;      }
	//DUI_DECLARE_MESSAGE_MAP()
	virtual void       OnFinalMessage(HWND hWnd){ delete this;              }
	//virtual LRESULT OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	//{
	//	//Close();
	//	//bHandled = FALSE;
	//	return 0;
	//}
	virtual void Notify(TNotifyUI& msg);

	void Init(HWND& hWndParent, POINT ptPos)
	{
		m_phWndParent = &hWndParent;
		Create(hWndParent, _T("ChoseFile"), UI_WNDSTYLE_CHILD, WS_EX_WINDOWEDGE);
		//::ClientToScreen(hWndParent, &ptPos);
		::SetWindowPos(*this, NULL, ptPos.x, ptPos.y, 0, 0, SWP_NOZORDER | SWP_NOSIZE | SWP_NOACTIVATE);
	}

	virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		LRESULT lRes = 0;

		if(m_PaintManager.MessageHandler(uMsg, wParam, lParam, lRes)) 
		{
			return lRes;
		}

		return __super::HandleMessage(uMsg, wParam, lParam);
	}


};

