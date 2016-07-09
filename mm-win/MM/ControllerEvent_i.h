#pragma once

//这个接口和Login event接口重复了。目的是为了LoginDlg不持有Login服务，
//而让Controller去创建爱您持有。LoginDlg为了得到事件，就让它关注这个事件了。还没想好如何重构。

class IControllerEvent
{
public:
	virtual void OnConnected()		= 0;
	virtual void OnConnectFail(const int errCode)	= 0;
};