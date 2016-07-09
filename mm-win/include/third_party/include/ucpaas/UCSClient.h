
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

#define VIDEO_PREVIEW_MODE 0 // 预览
#define VIDEO_SEND_MODE 1 // 送视频流
#define VIDEO_RECEIVE_MODE 2 // 接收视频流

#define UCS_ONLINE_PC 1
#define UCS_ONLINE_IOS 2
#define UCS_ONLINE_ANDROID 4

#define UCS_ONLINE 1
#define UCS_OFFLINE 0

#define UCS_ONLINE_MODE_WIFI 1
#define UCS_ONLINE_MODE_2G 2
#define UCS_ONLINE_MODE_3G 4


//消息结构
typedef struct _UCS_INSTANTMESSAGE
{
	int msgId;				//消息id
	__int64 time;			//消息发送时间
	int type;				//消息类型
	char fromClient[33];	//发送的client
	char toClient[33];		//接收消息client
	char content[2048];		//消息内容
	char fileName[256];		//文件名称
	int  fileSize;			//文件大小
	char extension[256];	//扩展
}UCS_INSTANTMESSAGE;
//下载结构
typedef struct _UCS_DOWNLOADINFO
{
	int msgId;
	char fileUrl[1024];
	char filePath[1024];
	int fileType;
}UCS_DOWNLOADINFO;

//呼叫类型
typedef enum _UCS_DIALTYPE
{
	VOICE_CALL = 0,
	VOICE_CALL_LANDING,
	VIDEO_CALL,
	VOICE_CALL_AUTO = 3
}UCS_DIALTYPE;

typedef struct _UCS_CAMERAINFO
{
	char name[256];		//设备名称
	int capabilityCount;
}UCS_CAMERAINFO;

typedef struct _UCS_CAMERADEVICE
{
	int rotate;	//角度，默认 (0）
	int index;	//对应摄像头索引
	int height;
	int width;
}UCS_CAMERADEVICE;

struct UCS_ONLINESTATUS
{
	char client[64];
	char phone[64];
	int pv;		//在线平台0x01(PC),0x02(ios),0x04(android)
	int state;	//在线状态1：在线 2：不在线 
	__int64 time; //时间戳,离线时为最后下线时间
	int netMode;	//网络模式 0x01(wifi),0x02(2G),0x04(3G)
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
//回调函数
typedef struct _UCS_CALLBACKINTERFACE
{
	//成功登录平台
	void (*onConnectionSuccessful)();
	//与平台连接断开或出错
	void (*onConnectionFailed)(int reason);

	//有呼叫接入
	void (*onIncomingCall)(const char *callid, UCS_DIALTYPE callType, const char *callerNumber,const char *callerName,const char *userData);
	//振铃
	void (*onAlerting)(const char *callid);
	//进入通话状态
	void (*onAnswer)(const char *callid);
	//呼叫失败
	void (*onDialFailed)(const char *callid, int reason);
	//挂断
	void (*onHangUp)(const char *callid, int reason);

	//双向回呼
	void (*onCallBack)(int reason);
	//通知收到消息
	void (*onReceiveIMessage)(UCS_INSTANTMESSAGE *data);
	//发送即时消息
	void (*onSendIMessage)(int reason, UCS_INSTANTMESSAGE *data);
	//下载文件
	void (*onDownloadFile)(int reason, UCS_DOWNLOADINFO *data);

	//查询在线状态
	void (*onQueryUserState)(int reason, UCS_ONLINESTATUS *data, int size);

}UCS_CALLBACKINTERFACE;

UCSCLIENT_API int Ucs_init(UCS_CALLBACKINTERFACE *CallbackInterface);
/*
	函数名   : Ucs_unInit
	功能     : 注销client
	返回值   : 是否成功 0：成功； 非0失败
*/
UCSCLIENT_API int Ucs_unInit();

UCSCLIENT_API int Ucs_connectToken(const char *token);
/*
	函数名   : Ucs_connect
	功能     : 连接Ucpaas平台服务器。
	参数     : 
			   [IN]  accountSid		: 用户主帐号
			   [IN]  accountToken		: 主账号密码
			   [IN]  clientNumber		: client账号
			   [IN]  clientPwd		: client密码
	返回值   : 是否连接成功 0：成功； 非0失败

*/
UCSCLIENT_API int Ucs_connect(const char *accountSid, const char *accountToken,
			const char *clientNumber, const char *clientPwd);
/*
	函数名   : Ucs_isConnected()
	功能     : 获取当前帐号与服务器的连接状态。
	返回值   : 是否成功 true：成功； false失败
*/
UCSCLIENT_API bool Ucs_isConnected();
/*
	函数名   : Ucs_dial
	功能     : 发起呼叫		
	参数     : 
				[IN]  callType			: 呼叫类型enum {VOICE_CALL,//语音VoIP电话 VOICE_CALL_LANDING//语音落地电话 VIDEO_CALL,//视频VoIP电话 };
				[IN]  calledNumber	: 被叫方号码。
	返回值   : 返回值为callid,本次呼叫的唯一标识; NULL表示失败.
*/
UCSCLIENT_API const char* Ucs_dial(UCS_DIALTYPE callType, const char *calledNumber);
/*
	函数名   : Ucs_dial
	功能     : 发起呼叫		
	参数     : 
				[IN]  callType			: 呼叫类型enum {
											VOICE_CALL,//语音VoIP电话 
											VOICE_CALL_LANDING//语音落地电话 
											VOICE_AUTO,//智能电话 
											VIDEO_CALL,//视频VoIP电话 };
				[IN]  calledNumber	: 被叫方号码。{VOICE_CALL_LANDING,VOICE_AUTO时:为电话号码，VOICE_CALL,VIDEO_CALL时:为client号码}
				[IN]  *userData		: 透传数据，必须小于128字节
	返回值   : 返回值为callid,本次呼叫的唯一标识; NULL表示失败.
*/
UCSCLIENT_API const char* Ucs_dialData(UCS_DIALTYPE callType, const char *calledNumber, const char *userData);

/*
	函数名   : Ucs_hangUp
	功能     : 挂机
	参数     : [IN]  callid	  : 当前呼叫的唯一标识， 如果callid 为NULL,这代表所有呼叫.
	返回值   : 是否成功 0：成功； 非0失败
*/
UCSCLIENT_API int Ucs_hangUp(const char *callid);
/*

	函数名   : Ucs_answer
	功能     : 应答呼入
	参数     : [IN]  callid	  : 当前呼叫的唯一标识
	返回值   : 是否成功 0：成功； 非0失败
*/

UCSCLIENT_API int Ucs_answer(const char *callid);

/*

	函数名   : Ucs_reject
	功能     : 拒绝呼叫
	参数     : [IN]  callid	  : 当前呼叫的唯一标识
	返回值   : 是否成功 0：成功； 非0失败
*/
UCSCLIENT_API int Ucs_reject(const char *callid);

/*
	函数名   : Ucs_sendDTMF
	功能     : 发送按键信息
	参数     : 
				[IN]  callid	  : 当前呼叫的唯一标识.
				[IN]	 dtmf	  : 一个按键值
	返回值   : 是否成功 0：成功； 非0失败
*/
UCSCLIENT_API int Ucs_sendDTMF(const char *callid, const char dtmf);

/*

	函数名   : Ucs_sendInstanceMessage
	功能     : 发送即时消息。支持文本消息或者附件消息，text与filePath不能同时为空
	参数     :
				[IN]	receiver	: 接收者。接收方的VoIP账号或者是群组Id，不能为NULL
				[IN]	text		: 文本消息内容。text与attached不能同时为NULL
				[IN]	filePath	: 附件文件名称，全路径。
				[IN]	msgType		: 消息类型
	返回值   : 消息id；
*/
UCSCLIENT_API int Ucs_sendInstanceMessage(const char *receiver, const char *text, char *filePath, const int msgType);

/*

	函数名   : Ucs_onCallback
	功能     : 双向回呼
	参数     : 
				[IN] phoneNumber: 被叫号码。
	返回值   : 是否成功 0：成功； 非0失败
*/
UCSCLIENT_API int Ucs_callBack(const char *phoneNumber);
UCSCLIENT_API int Ucs_callBackShowNum(const char *phoneNumber, const char *toNum, const char *fromNum);
/*

	函数名   : Ucs_getSpeakerStatus
	功能     : 获取当前扬声器否开启状态
	参数     : 
	返回值   : true开启； false关闭
*/
UCSCLIENT_API bool Ucs_getSpeakerStatus();

/*
	函数名   : Ucs_setSpeakerSpeaker
	功能     : 设置扬声器状态,
	参数     : [IN]  enable : 是否开启
	返回值   : 是否成功 0：成功； 非0失败
*/
UCSCLIENT_API int Ucs_setSpeakerStatus(bool enable);
/*

	函数名   : Ucs_getMicMuteStatus
	功能     : 获取静音状态
	参数     : 
	返回值   : true开启； false关闭
*/
UCSCLIENT_API bool Ucs_getMicMuteStatus();

/*

	函数名   : Ucs_setMicMuteMuteStatus
	功能     : 通话过程中设置静音，自己能听到对方的声音，通话对方听不到自己的声音。
	参数     : [IN] enable : 是否开启
	返回值   : 是否成功 0：成功； 非0失败
*/
UCSCLIENT_API int Ucs_setMicMuteStatus(bool enable);

/*

	函数名   : Ucs_downloadFile
	功能     : 根据传入的url下载多媒体言文件
	参数     : [IN]downloadInfo  : 下载结构体首指针，下载url，msgID, type, filePath文件保存路径放到一个结构体中
	返回值   : 是否成功 0：成功； 非0失败
*/
UCSCLIENT_API int Ucs_downloadFile(UCS_DOWNLOADINFO downloadInfo);
/*

	函数名   : Ucs_playVoice
	功能     : 播放语音
	参数     : [IN]  fileName	   : 录音文件保存的全路径
	返回值   : 是否成功 0：成功； 非0失败
*/
UCSCLIENT_API int Ucs_playVoice(const char *filePath);
/*

	函数名   : Ucs_stopVoice
	功能     : 停止播放语音
	参数     : 无
	返回值   : 是否成功 0：成功； 非0失败
*/
UCSCLIENT_API int Ucs_stopVoice();
/*

	函数名   : Ucs_startVoiceRecord
	功能     : 开始录制语音
	参数     : [IN]  filePath	   : 录音文件保存的全路径
	返回值   : 是否成功 0：成功； 非0失败
*/
UCSCLIENT_API int Ucs_startVoiceRecord(const char *filePath);
/*

	函数名   : Ucs_stopVoiceRecord
	功能     : 停止录制语音
	参数     : 无
	返回值   : 无
*/
UCSCLIENT_API int Ucs_stopVoiceRecord();

/*
	函数名   : Ucs_setRing
	功能     : 设置来电铃声
	参数     : [IN]  filePath	   : 文件保存的全路径
	返回值   : 是否成功 0：成功； 非0失败
*/
UCSCLIENT_API int Ucs_setRing(const char *filePath);

/*
	函数名   : Ucs_setRingBack
	功能     : 设置回铃声
	参数     : [IN]  filePath	   : 文件保存的全路径
	返回值   : 是否成功 0：成功； 非0失败
*/
UCSCLIENT_API int Ucs_setRingBack(const char *filePath);

/*

	函数名   : Ucs_stopVoiceRecord
	功能     : 设置扬声器音量
	参数     : [IN] volume  :音量数值0-100
	返回值   : 无
*/
UCSCLIENT_API bool Ucs_setVolumeValue(unsigned int volume);

/*

	函数名   : Ucs_stopVoiceRecord
	功能     : 获取扬声器音量
	参数     : 无
	返回值   : 音量数值
*/
UCSCLIENT_API unsigned int Ucs_getVolumeValue();

/*
     函数名   : Ucs_getVersion
     功能     : 获取引用的底层基本通信函数模块版本信息
     返回值   : 版本信息字符串。
*/
UCSCLIENT_API const char *Ucs_getSDKVersion();


UCSCLIENT_API int Ucs_setListServer(const char *listServerIp, int port);

UCSCLIENT_API int Ucs_connectCustom(const char *loginServerIp, int port, const char *accountSid, const char *accountToken,
			  const char *clientNumber, const char *clientPwd);

UCSCLIENT_API int Ucs_connectTokenCustom(const char *loginServerIp, int port, const char *token);

/*

	函数名   : Ucs_setVideoHandle
	功能     : 设置显示窗口句柄
	参数     : [IN]  pLocalVideo		: 显示本地图像窗口/控件句柄
				[IN]  pRemoteVideo	: 显示远程图像窗口/控件句柄
	返回值   : 是否成功 0：成功； 非0失败
*/
UCSCLIENT_API int Ucs_setVideoHandle(void *pLocalVideo, void *pRemoteVideo);


/*
	函数名   : Ucs_getCameraNum
	功能     : 获取摄像头数量
	参数     : 无
	返回值   : 当前摄像头数量
*/
UCSCLIENT_API int Ucs_getCameraNum();

/*
	函数名   : Ucs_getCameraInfo
	功能     : 获取摄像头信息
	参数     : [IN]	index	:摄像头对应索引，从0开始
				[OUT]	info	:摄像头信息
	返回值   : 是否成功 0：成功； 非0失败
*/
UCSCLIENT_API int Ucs_getCameraInfo(int index, UCS_CAMERAINFO *info);

/*
	函数名   : Ucs_getCurCamera
	功能     : 获取当前摄像头
	参数     : 无
	返回值   : 当前摄像头索引
*/
UCSCLIENT_API int Ucs_getCurCamera();
/*
	函数名   : Ucs_setVideoDevice
	功能     : 切换当前摄像头
	参数     : [IN]	video
	返回值   : 是否成功 0：成功； 非0失败
*/
UCSCLIENT_API int Ucs_setCameraDevice(const UCS_CAMERADEVICE *video);
/*
	函数名   : Ucs_openVideo
	功能     : 打开视频
	参数     : [IN]	mode :0：预览 1：只发送视频流 2：接收视频流 
	返回值   : 是否成功 0：成功； 非0失败
*/
UCSCLIENT_API int Ucs_openVideo(const int mode);
/*
	函数名   : Ucs_closeVideo
	功能     : 关闭视频
	参数     : [IN]	mode :0：预览 1：送视频流 2：接收视频流 
	返回值   : 是否成功 0：成功； 非0失败
*/
UCSCLIENT_API int Ucs_closeVideo(const int mode);

/*
	函数名   : Ucs_queryUserState
	功能     : 获取用户在线状态
	参数     : [IN]	friendNum	:查询client账号/phone号码数组
				[IN]	size		:client/phone数组数量
				{IN]	numType		:查询类型 0：按client查询；1：按phone查询
	返回值   : 是否成功 0：成功； 非0失败
*/
UCSCLIENT_API int Ucs_queryUserState(const char **friendNum, int size, int numType);

/*
	函数名   : Ucs_queryOneUserState
	功能     : 获取用户在线状态
	参数     : [IN]	friendNum	:查询client账号/phone号码
				{IN]	numType		:查询类型 0：按client查询；1：按phone查询
	返回值   : 是否成功 0：成功； 非0失败
*/
UCSCLIENT_API int Ucs_queryOneUserState(const char *friendNum, int numType);
#endif