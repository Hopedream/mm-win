
/*
 *	Version:V1.2.0
 *
 *  Copyright (c) 2014 The UCS project authors. All Rights Reserved.
 *
 *	http://www.ucpaas.com
 */

#ifndef UCS_CLIENT_H
#define UCS_CLIENT_H

#ifdef UCSCLIENT_EXPORTS
#define UCSCLIENT_API  __declspec(dllexport)
//#define UCSCLIENT_API
#else
#define UCSCLIENT_API __declspec(dllimport)
#endif

#define QUERY_USERSTATE_CLIENT 0
#define QUERY_USERSTATE_PHONE 1

#define VIDEO_PREVIEW_MODE 0 // Ԥ��
#define VIDEO_SEND_MODE 1 // ����Ƶ��
#define VIDEO_RECEIVE_MODE 2 // ������Ƶ��

#define UCS_ONLINE_PC 1
#define UCS_ONLINE_IOS 2
#define UCS_ONLINE_ANDROID 4

#define UCS_ONLINE 1
#define UCS_OFFLINE 0

#define UCS_ONLINE_MODE_WIFI 1
#define UCS_ONLINE_MODE_2G 2
#define UCS_ONLINE_MODE_3G 4


//��Ϣ�ṹ
typedef struct _UCS_INSTANTMESSAGE
{
	int msgId;				//��Ϣid
	__int64 time;			//��Ϣ����ʱ��
	int type;				//��Ϣ����
	char fromClient[33];	//���͵�client
	char toClient[33];		//������Ϣclient
	char content[2048];		//��Ϣ����
	char fileName[256];		//�ļ�����
	int  fileSize;			//�ļ���С
	char extension[256];	//��չ
}UCS_INSTANTMESSAGE;
//���ؽṹ
typedef struct _UCS_DOWNLOADINFO
{
	int msgId;
	char fileUrl[1024];
	char filePath[1024];
	int fileType;
}UCS_DOWNLOADINFO;

//��������
typedef enum _UCS_DIALTYPE
{
	VOICE_CALL = 0,
	VOICE_CALL_LANDING,
	VIDEO_CALL,
	VOICE_CALL_AUTO = 3
}UCS_DIALTYPE;

typedef struct _UCS_CAMERAINFO
{
	char name[256];		//�豸����
	int capabilityCount;
}UCS_CAMERAINFO;

typedef struct _UCS_CAMERADEVICE
{
	int rotate;	//�Ƕȣ�Ĭ�� (0��
	int index;	//��Ӧ����ͷ����
	int height;
	int width;
}UCS_CAMERADEVICE;

struct UCS_ONLINESTATUS
{
	char client[64];
	char phone[64];
	int pv;		//����ƽ̨0x01(PC),0x02(ios),0x04(android)
	int state;	//����״̬1������ 2�������� 
	__int64 time; //ʱ���,����ʱΪ�������ʱ��
	int netMode;	//����ģʽ 0x01(wifi),0x02(2G),0x04(3G)
	UCS_ONLINESTATUS(char* _client, char* _phone, int _pv, int _state, __int64 _time, int _netMode)
	{
		strcpy(client, _client);
		strcpy(phone, _phone);
		pv = _pv;
		state = _state;
		time = _time;
		netMode = _netMode;
	}
};
//�ص�����
typedef struct _UCS_CALLBACKINTERFACE
{
	//�ɹ���¼ƽ̨
	void (*onConnectionSuccessful)();
	//��ƽ̨���ӶϿ������
	void (*onConnectionFailed)(int reason);

	//�к��н���
	void (*onIncomingCall)(const char *callid, UCS_DIALTYPE callType, const char *callerNumber,const char *callerName,const char *userData);
	//����
	void (*onAlerting)(const char *callid);
	//����ͨ��״̬
	void (*onAnswer)(const char *callid);
	//����ʧ��
	void (*onDialFailed)(const char *callid, int reason);
	//�Ҷ�
	void (*onHangUp)(const char *callid, int reason);

	//˫��غ�
	void (*onCallBack)(int reason);
	//֪ͨ�յ���Ϣ
	void (*onReceiveIMessage)(UCS_INSTANTMESSAGE *data);
	//���ͼ�ʱ��Ϣ
	void (*onSendIMessage)(int reason, UCS_INSTANTMESSAGE *data);
	//�����ļ�
	void (*onDownloadFile)(int reason, UCS_DOWNLOADINFO *data);

	//��ѯ����״̬
	void (*onQueryUserState)(int reason, UCS_ONLINESTATUS *data, int size);

}UCS_CALLBACKINTERFACE;

UCSCLIENT_API int Ucs_init(UCS_CALLBACKINTERFACE *CallbackInterface);
/*
	������   : Ucs_unInit
	����     : ע��client
	����ֵ   : �Ƿ�ɹ� 0���ɹ��� ��0ʧ��
*/
UCSCLIENT_API int Ucs_unInit();

UCSCLIENT_API int Ucs_connectToken(const char *token);
/*
	������   : Ucs_connect
	����     : ����Ucpaasƽ̨��������
	����     : 
			   [IN]  accountSid		: �û����ʺ�
			   [IN]  accountToken		: ���˺�����
			   [IN]  clientNumber		: client�˺�
			   [IN]  clientPwd		: client����
	����ֵ   : �Ƿ����ӳɹ� 0���ɹ��� ��0ʧ��

*/
UCSCLIENT_API int Ucs_connect(const char *accountSid, const char *accountToken,
			const char *clientNumber, const char *clientPwd);
/*
	������   : Ucs_isConnected()
	����     : ��ȡ��ǰ�ʺ��������������״̬��
	����ֵ   : �Ƿ�ɹ� true���ɹ��� falseʧ��
*/
UCSCLIENT_API bool Ucs_isConnected();
/*
	������   : Ucs_dial
	����     : �������		
	����     : 
				[IN]  callType			: ��������enum {VOICE_CALL,//����VoIP�绰 VOICE_CALL_LANDING//������ص绰 VIDEO_CALL,//��ƵVoIP�绰 };
				[IN]  calledNumber	: ���з����롣
	����ֵ   : ����ֵΪcallid,���κ��е�Ψһ��ʶ; NULL��ʾʧ��.
*/
UCSCLIENT_API const char* Ucs_dial(UCS_DIALTYPE callType, const char *calledNumber);
/*
	������   : Ucs_dial
	����     : �������		
	����     : 
				[IN]  callType			: ��������enum {
											VOICE_CALL,//����VoIP�绰 
											VOICE_CALL_LANDING//������ص绰 
											VOICE_AUTO,//���ܵ绰 
											VIDEO_CALL,//��ƵVoIP�绰 };
				[IN]  calledNumber	: ���з����롣{VOICE_CALL_LANDING,VOICE_AUTOʱ:Ϊ�绰���룬VOICE_CALL,VIDEO_CALLʱ:Ϊclient����}
				[IN]  *userData		: ͸�����ݣ�����С��128�ֽ�
	����ֵ   : ����ֵΪcallid,���κ��е�Ψһ��ʶ; NULL��ʾʧ��.
*/
UCSCLIENT_API const char* Ucs_dialData(UCS_DIALTYPE callType, const char *calledNumber, const char *userData);

/*
	������   : Ucs_hangUp
	����     : �һ�
	����     : [IN]  callid	  : ��ǰ���е�Ψһ��ʶ�� ���callid ΪNULL,��������к���.
	����ֵ   : �Ƿ�ɹ� 0���ɹ��� ��0ʧ��
*/
UCSCLIENT_API int Ucs_hangUp(const char *callid);
/*

	������   : Ucs_answer
	����     : Ӧ�����
	����     : [IN]  callid	  : ��ǰ���е�Ψһ��ʶ
	����ֵ   : �Ƿ�ɹ� 0���ɹ��� ��0ʧ��
*/

UCSCLIENT_API int Ucs_answer(const char *callid);

/*

	������   : Ucs_reject
	����     : �ܾ�����
	����     : [IN]  callid	  : ��ǰ���е�Ψһ��ʶ
	����ֵ   : �Ƿ�ɹ� 0���ɹ��� ��0ʧ��
*/
UCSCLIENT_API int Ucs_reject(const char *callid);

/*
	������   : Ucs_sendDTMF
	����     : ���Ͱ�����Ϣ
	����     : 
				[IN]  callid	  : ��ǰ���е�Ψһ��ʶ.
				[IN]	 dtmf	  : һ������ֵ
	����ֵ   : �Ƿ�ɹ� 0���ɹ��� ��0ʧ��
*/
UCSCLIENT_API int Ucs_sendDTMF(const char *callid, const char dtmf);

/*

	������   : Ucs_sendInstanceMessage
	����     : ���ͼ�ʱ��Ϣ��֧���ı���Ϣ���߸�����Ϣ��text��filePath����ͬʱΪ��
	����     :
				[IN]	receiver	: �����ߡ����շ���VoIP�˺Ż�����Ⱥ��Id������ΪNULL
				[IN]	text		: �ı���Ϣ���ݡ�text��attached����ͬʱΪNULL
				[IN]	filePath	: �����ļ����ƣ�ȫ·����
				[IN]	msgType		: ��Ϣ����
	����ֵ   : ��Ϣid��
*/
UCSCLIENT_API int Ucs_sendInstanceMessage(const char *receiver, const char *text, char *filePath, const int msgType);

/*

	������   : Ucs_onCallback
	����     : ˫��غ�
	����     : 
				[IN] phoneNumber: ���к��롣
	����ֵ   : �Ƿ�ɹ� 0���ɹ��� ��0ʧ��
*/
UCSCLIENT_API int Ucs_callBack(const char *phoneNumber);
UCSCLIENT_API int Ucs_callBackShowNum(const char *phoneNumber, const char *toNum, const char *fromNum);
/*

	������   : Ucs_getSpeakerStatus
	����     : ��ȡ��ǰ����������״̬
	����     : 
	����ֵ   : true������ false�ر�
*/
UCSCLIENT_API bool Ucs_getSpeakerStatus();

/*
	������   : Ucs_setSpeakerSpeaker
	����     : ����������״̬,
	����     : [IN]  enable : �Ƿ���
	����ֵ   : �Ƿ�ɹ� 0���ɹ��� ��0ʧ��
*/
UCSCLIENT_API int Ucs_setSpeakerStatus(bool enable);
/*

	������   : Ucs_getMicMuteStatus
	����     : ��ȡ����״̬
	����     : 
	����ֵ   : true������ false�ر�
*/
UCSCLIENT_API bool Ucs_getMicMuteStatus();

/*

	������   : Ucs_setMicMuteMuteStatus
	����     : ͨ�����������þ������Լ��������Է���������ͨ���Է��������Լ���������
	����     : [IN] enable : �Ƿ���
	����ֵ   : �Ƿ�ɹ� 0���ɹ��� ��0ʧ��
*/
UCSCLIENT_API int Ucs_setMicMuteStatus(bool enable);

/*

	������   : Ucs_downloadFile
	����     : ���ݴ����url���ض�ý�����ļ�
	����     : [IN]downloadInfo  : ���ؽṹ����ָ�룬����url��msgID, type, filePath�ļ�����·���ŵ�һ���ṹ����
	����ֵ   : �Ƿ�ɹ� 0���ɹ��� ��0ʧ��
*/
UCSCLIENT_API int Ucs_downloadFile(UCS_DOWNLOADINFO downloadInfo);
/*

	������   : Ucs_playVoice
	����     : ��������
	����     : [IN]  fileName	   : ¼���ļ������ȫ·��
	����ֵ   : �Ƿ�ɹ� 0���ɹ��� ��0ʧ��
*/
UCSCLIENT_API int Ucs_playVoice(const char *filePath);
/*

	������   : Ucs_stopVoice
	����     : ֹͣ��������
	����     : ��
	����ֵ   : �Ƿ�ɹ� 0���ɹ��� ��0ʧ��
*/
UCSCLIENT_API int Ucs_stopVoice();
/*

	������   : Ucs_startVoiceRecord
	����     : ��ʼ¼������
	����     : [IN]  filePath	   : ¼���ļ������ȫ·��
	����ֵ   : �Ƿ�ɹ� 0���ɹ��� ��0ʧ��
*/
UCSCLIENT_API int Ucs_startVoiceRecord(const char *filePath);
/*

	������   : Ucs_stopVoiceRecord
	����     : ֹͣ¼������
	����     : ��
	����ֵ   : ��
*/
UCSCLIENT_API int Ucs_stopVoiceRecord();

/*
	������   : Ucs_setRing
	����     : ������������
	����     : [IN]  filePath	   : �ļ������ȫ·��
	����ֵ   : �Ƿ�ɹ� 0���ɹ��� ��0ʧ��
*/
UCSCLIENT_API int Ucs_setRing(const char *filePath);

/*
	������   : Ucs_setRingBack
	����     : ���û�����
	����     : [IN]  filePath	   : �ļ������ȫ·��
	����ֵ   : �Ƿ�ɹ� 0���ɹ��� ��0ʧ��
*/
UCSCLIENT_API int Ucs_setRingBack(const char *filePath);

/*

	������   : Ucs_stopVoiceRecord
	����     : ��������������
	����     : [IN] volume  :������ֵ0-100
	����ֵ   : ��
*/
UCSCLIENT_API bool Ucs_setVolumeValue(unsigned int volume);

/*

	������   : Ucs_stopVoiceRecord
	����     : ��ȡ����������
	����     : ��
	����ֵ   : ������ֵ
*/
UCSCLIENT_API unsigned int Ucs_getVolumeValue();

/*
     ������   : Ucs_getVersion
     ����     : ��ȡ���õĵײ����ͨ�ź���ģ��汾��Ϣ
     ����ֵ   : �汾��Ϣ�ַ�����
*/
UCSCLIENT_API const char *Ucs_getSDKVersion();


UCSCLIENT_API int Ucs_setListServer(const char *listServerIp, int port);

UCSCLIENT_API int Ucs_connectCustom(const char *loginServerIp, int port, const char *accountSid, const char *accountToken,
			  const char *clientNumber, const char *clientPwd);

UCSCLIENT_API int Ucs_connectTokenCustom(const char *loginServerIp, int port, const char *token);

/*

	������   : Ucs_setVideoHandle
	����     : ������ʾ���ھ��
	����     : [IN]  pLocalVideo		: ��ʾ����ͼ�񴰿�/�ؼ����
				[IN]  pRemoteVideo	: ��ʾԶ��ͼ�񴰿�/�ؼ����
	����ֵ   : �Ƿ�ɹ� 0���ɹ��� ��0ʧ��
*/
UCSCLIENT_API int Ucs_setVideoHandle(void *pLocalVideo, void *pRemoteVideo);


/*
	������   : Ucs_getCameraNum
	����     : ��ȡ����ͷ����
	����     : ��
	����ֵ   : ��ǰ����ͷ����
*/
UCSCLIENT_API int Ucs_getCameraNum();

/*
	������   : Ucs_getCameraInfo
	����     : ��ȡ����ͷ��Ϣ
	����     : [IN]	index	:����ͷ��Ӧ��������0��ʼ
				[OUT]	info	:����ͷ��Ϣ
	����ֵ   : �Ƿ�ɹ� 0���ɹ��� ��0ʧ��
*/
UCSCLIENT_API int Ucs_getCameraInfo(int index, UCS_CAMERAINFO *info);

/*
	������   : Ucs_getCurCamera
	����     : ��ȡ��ǰ����ͷ
	����     : ��
	����ֵ   : ��ǰ����ͷ����
*/
UCSCLIENT_API int Ucs_getCurCamera();
/*
	������   : Ucs_setVideoDevice
	����     : �л���ǰ����ͷ
	����     : [IN]	video
	����ֵ   : �Ƿ�ɹ� 0���ɹ��� ��0ʧ��
*/
UCSCLIENT_API int Ucs_setCameraDevice(const UCS_CAMERADEVICE *video);
/*
	������   : Ucs_openVideo
	����     : ����Ƶ
	����     : [IN]	mode :0��Ԥ�� 1��ֻ������Ƶ�� 2��������Ƶ�� 
	����ֵ   : �Ƿ�ɹ� 0���ɹ��� ��0ʧ��
*/
UCSCLIENT_API int Ucs_openVideo(const int mode);
/*
	������   : Ucs_closeVideo
	����     : �ر���Ƶ
	����     : [IN]	mode :0��Ԥ�� 1������Ƶ�� 2��������Ƶ�� 
	����ֵ   : �Ƿ�ɹ� 0���ɹ��� ��0ʧ��
*/
UCSCLIENT_API int Ucs_closeVideo(const int mode);

/*
	������   : Ucs_queryUserState
	����     : ��ȡ�û�����״̬
	����     : [IN]	friendNum	:��ѯclient�˺�/phone��������
				[IN]	size		:client/phone��������
				{IN]	numType		:��ѯ���� 0����client��ѯ��1����phone��ѯ
	����ֵ   : �Ƿ�ɹ� 0���ɹ��� ��0ʧ��
*/
UCSCLIENT_API int Ucs_queryUserState(const char **friendNum, int size, int numType);

/*
	������   : Ucs_queryOneUserState
	����     : ��ȡ�û�����״̬
	����     : [IN]	friendNum	:��ѯclient�˺�/phone����
				{IN]	numType		:��ѯ���� 0����client��ѯ��1����phone��ѯ
	����ֵ   : �Ƿ�ɹ� 0���ɹ��� ��0ʧ��
*/
UCSCLIENT_API int Ucs_queryOneUserState(const char *friendNum, int numType);
#endif