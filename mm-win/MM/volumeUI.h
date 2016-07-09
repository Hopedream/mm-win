#pragma once
// 此处需要包含duilib的头文件#include <UIlib.h>那一段，详细代码请见前面教程
#include "StdAfx.h"
#include "MMWinImplBase.h"

class CVolumeUI: public MMWindowImplBase
{
public:
	explicit CVolumeUI();

protected:
	virtual ~CVolumeUI();   // 私有化析构函数，这样此对象只能通过new来生成，而不能直接定义变量。就保证了delete this不会出错

public:
	void Init(HWND hWndParent, POINT ptPos);
	//virtual void    OnFinalMessage(HWND hWnd);
	virtual LRESULT HandleMessage (UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual LRESULT OnKillFocus   (UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual CDuiString GetSkinFile();
	virtual void HandleValueChangedEvent(TNotifyUI& msg);

private:
	HWND m_hParentWnd;
	CSliderUI* m_pSliderUI;
};