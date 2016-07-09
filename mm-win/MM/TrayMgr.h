#pragma once

#include <ShellAPI.h>
#include <vector>
#include <stack>
using namespace std;

#define Use_Default 0
#define Int_Default 0
#define Default 0

#define No_Care -1
#define No_Care_HWND (HWND)-1

#define Use_Current_Int -1
#define Use_Current_Pointer -1
#define Use_Current_SZ (TCHAR*)(-1)

#define Use_Current Use_Current_Int
#define Use_Current_HICON (HICON)(-1)


enum ETrayNotifyMode
{
	eTNMode_Null = 0,
	eTNMode_Event,
	eTNMode_Msg,
};

enum ETrayFlashStopMode
{
	eTFSMode_Click = 0,
	eTFSMode_DoubleClick,
};

enum ETrayEventId
{
	WM_LBUTTONDOWN_Late = WM_USER + 8765,
	WM_TrayFlashStopped,
};

class ITrayEvent
{
public:
	virtual void OnTrayEvent(WPARAM w, LPARAM l) = 0;
};

struct STrayFlashStackItem
{
	HICON hicon;
	DWORD dwParam;

	STrayFlashStackItem()
	{
		hicon = NULL;
		dwParam = 0;
	}
};

class CTrayMgr
{
public:
	CTrayMgr();
	~CTrayMgr(void);

	void Init(HWND hwndAttach = No_Care_HWND, ETrayNotifyMode = eTNMode_Event, ETrayFlashStopMode = eTFSMode_DoubleClick);
	BOOL AddTaskbarIcon(HICON hicon, LPCTSTR szTip, UINT uCallbackMsgId = No_Care);

	//BOOL ReAdd(HICON hicon, LPCTSTR szTip, UINT uCallbackMsgId = No_Care);

	BOOL Delete();
	HICON GetIcon(){return m_nid.hIcon;}

	BOOL Change(HICON hiconNew, LPCTSTR szTipNew = Use_Current_SZ, UINT uCallbackMsgIdNew = Use_Current)
	{
		return InnerChange(hiconNew, szTipNew, uCallbackMsgIdNew);
	};

	BOOL ChangeIcon(HICON hiconNew)
	{
		return InnerChange(hiconNew, Use_Current_SZ, Use_Current);
	}

	BOOL ChangeTip(LPCTSTR szTipNew)
	{
		return InnerChange(Use_Current_HICON, szTipNew, Use_Current);
	}

	BOOL Flash(HICON hiconNew = Use_Current_HICON, DWORD dwParam = 0);
	BOOL StopFlash(int nIndex = -1);
	BOOL StopAllFlash();

	// �¼�����
	void AddEventHandler(ITrayEvent* pHandler);
	void DeleteEventHandler(ITrayEvent* pHandler);
	void FireOnTrayEvent(WPARAM w, LPARAM l);

	BOOL IsFlashing()
	{
		//ASSERT(m_bFlashing == TRUE || m_bFlashing == FALSE);
		return m_bFlashing;
	}

	//��ȡ��Ϣ��ջ�Ĵ�С
	int GetFlashStatckSize()
	{
		return m_stackFlash.size();
	}
	//��ȡ��Ϣ��ջ
	vector<STrayFlashStackItem>* GetFlashStack()
	{
		return &m_stackFlash;
	}
	
	//��ȡ���ھ��
	HWND GetWindowHandle();

	//��ȡ��˸��Ϣ��ջ����Ϣ
	DWORD GetParam(int idx);
protected:
	NOTIFYICONDATA m_nid;
	//��˸��Ϣ�Ķ�ջ
	vector<STrayFlashStackItem> m_stackFlash;
	//�Ƿ�����˸
	BOOL m_bFlashing;
	//������Ϣ����
	HWND m_hwndTray;
	ETrayNotifyMode m_eTNMode;

	//�¼������ߣ�TrayTaskModuleImplement module
	vector<ITrayEvent*> m_vecTrayEventHandler;
	BOOL m_bWaitForDblClk;

	ETrayFlashStopMode m_eTFSMode;
	HICON m_hiconBeforeFlash;
	UINT m_uTrayFlashTimerId;

protected:
	//��Ϣ����ע��ʹ���
	static void CALLBACK OnFlashTimer(HWND hwnd, UINT uMsg, UINT nEvent, DWORD dwTime);
	static LRESULT CALLBACK TrayWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	static ATOM RegisterClassForTrayWnd(/*HINSTANCE hInstance*/);

	BOOL InnerChange(HICON hicon = Use_Current_HICON, LPCTSTR szTip = Use_Current_SZ, UINT uCallbackMsgId = Use_Current);
};
