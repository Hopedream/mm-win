#pragma once

//����ӿں�Login event�ӿ��ظ��ˡ�Ŀ����Ϊ��LoginDlg������Login����
//����Controllerȥ�����������С�LoginDlgΪ�˵õ��¼�����������ע����¼��ˡ���û�������ع���

class IControllerEvent
{
public:
	virtual void OnConnected()		= 0;
	virtual void OnConnectFail(const int errCode)	= 0;
};