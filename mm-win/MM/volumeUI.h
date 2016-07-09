#pragma once
// �˴���Ҫ����duilib��ͷ�ļ�#include <UIlib.h>��һ�Σ���ϸ�������ǰ��̳�
#include "StdAfx.h"
#include "MMWinImplBase.h"

class CVolumeUI: public MMWindowImplBase
{
public:
	explicit CVolumeUI();

protected:
	virtual ~CVolumeUI();   // ˽�л����������������˶���ֻ��ͨ��new�����ɣ�������ֱ�Ӷ���������ͱ�֤��delete this�������

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