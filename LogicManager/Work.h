#ifndef _WORK_H_
#define _WORK_H_

#include "LRunnable.h"
#include "LSingleton.h"
#include "LTime.h"
#include "LMsg.h"
#include "LNet.h"
#include "LMsgS2S.h"
#include "TCPClient.h"
#include "GarbageCollectionService.h"
#include "LCardsOperType.h"
#include "RoomManager.h"

class User;

struct UserLoginInfo
{
	Lint	m_seed;
	Lint	m_time;
	LUser	m_user;
};

struct StrBoardInfo
{
	Lstring m_strInfo;
	Lstring m_strId;
};



class Work:public LRunnable,public LSingleton<Work>
{
public:
	virtual bool	Init();
	virtual bool	Final();

	virtual void	Start();
	virtual void	Run();
	virtual void	Clear();
	virtual	void	Join();
	virtual	void	Stop();

	void			Tick(LTime& cur);
	void			HanderMsg(LMsg* msg);

	LTime&			GetCurTime();
	
	//md5加密算法
	Lstring			Md5Hex(const void* data,Lsize size);
public:
	//处理http消息 
	void			HanderHttp(LMsgHttp* msg);	
	//处理客户端连接上来消息
	void			HanderClientIn(LMsgIn* msg);
	//处理客户端掉线的消息 
	void			HanderUserKick(LMsgKick* msg);

	void            HanderKeepAliveAck(LMsgS2SKeepAliveAck*msg);
	
	void            HanderKeepAlive(LMsgS2SKeepAlive*msg);

	//处理玩家登陆的消息 
	void			HanderUserLogin(Lint id,Lint seed,Lint gateId,Llong geteUserId,Lstring& md5,Lstring& ip, const Lstring &location);

	//处理玩家断线重连的消息 
	void			HanderUserReLogin(Lint id,Lint seed,Lint gateId,Llong gateUserId,Lstring& md5,Lstring& ip);
	
	//处理玩家登陆的消息 
	void			HanderUserLogout(Lint gateId, Llong geteUserId);

public:
	void			ConnectToCenter();

	void			SendToCenter(LMsg& msg);
	//发送网关信息到center
	void			SendGateInfoToCenter();
	
	//发送logic消息到其他gate
	void			SendLogicInfoToGates(Lint nGateID = 0);



	//center服务器关闭
	void			HanderCenterLogout();
	void			HanderDBServerLogout();
	UserLoginInfo*  GetUserLoginInfo(Lint userId);
	void			DelUserLoginInfo(Lint userId);
	
	

	


	void			HanderGateLogin(LMsgG2ServerLogin* msg);
	void			HanderGateLogout(LMsgKick* msg);
	GateInfo*		GetGateInfoBySp(LSocketPtr sp);
	GateInfo*		GetGateInfoById(Lint id);
	void			DelGateInfo(Lint id);

	void			HanderGateUserMsg(LMsgG2LMUserMsg* msg,GateInfo* gate);
public:
	void			HanderLogicLogin(LMsgL2LMGLogin* msg);
	void			HanderLogicLogout(LMsgKick* msg);
	
	LogicInfo*		GetLogicInfoBySp(LSocketPtr sp);
	
	LogicInfo*		GetLogicInfoById(Lint id);
	
	LogicInfo*		GetLogicInfoByPresure();
	
	LogicInfo*		GetLogicInfoByGameType(Lint game_type);
	void			DelLogicInfo(Lint id);
	


public:
	void SendMessageToLogic(Lint logicID, LMsg& msg);
	void SendMessageToGate(Lint gateID, LMsg& msg);
	void SendMessageToAllLogic(LMsg& msg);
	void SendMessageToAllGate(LMsg& msg);

	void SendMessageToUser(LSocketPtr sp, Lint user_id,LMsg& msg);
	

private:
	void			checkAndTouchLogic();
	void			TryToReconnect();
	void			SendLoginToCenter();
	
	void			EraseExpiredLogics(LTime& cur);
	void			OnUserLogin(User *user);


	//分割線
	void			HanderGame2LMLogin(LMsgGame2LMGLogin* msg);
	void			HanderGameLogout(LMsgKick* msg);
	void			SendGameServerInfoToGates(Lint GateID = 0);
	GameServerInfo*	GetGameInfoBySp(LSocketPtr sp);
	void			DelGameInfo(Lint id);

public:

	//void			HanderDeskOpt(LMsgC2SDeskOpt* msg);
	
	//void			HanderDeskOptRespon(LMsgL2LMDeskOpt* msg);
	
	//void			FillDeskMsg(DeskMsg& send, LMsgL2LMDeskOpt* msg);

	void			HanderC2SQuickRoomOpt(LSocketPtr sp,LMsgC2SQuickRoomOpt* msg);

	void			HanderUserLogin(LSocketPtr sp,LMsgC2SLMLogin* msg);

	LSocketPtr		GetGateSpByUserId(int user_id);

	void			ModifyUserStatus(int user_id, USER_STATUS status, int logic_server_id);
	void			HanderLM2LMQuickCreateRoom(LMsgLM2LMQuckCreateRoom* msg);
	void			HanderL2LMQuickCreateRoomOpt(LMsgL2LMQuickCreateRoomOpt* msg);

private:
	LTime		m_tickTimer;

	Llong		m_30MSceTick;//30毫秒循环一次

	Llong		m_200MSceTick;//200毫秒循环一次

	Llong		m_1SceTick;//1秒循环一次

	Llong		m_5SceTick;//5秒循环一次

	Llong		m_30SceTick;//30秒循环一次

	Llong		m_600SceTick;//5分钟秒循环一次

	Llong		m_3600SecTick;	// 1小时循环一次
	bool		m_IsLogicDB;	//Logic状态， false 需要重连
private:


	std::map<Lint, GateInfo> m_gateInfo;

	std::map<Lint, LogicInfo> m_logicInfo;
	
	std::map<Lint, std::map<Lint, Lint> > m_mapLogicServerOnlySupportType; //这些服务器只支持这些游戏类型。格式： 主键是logicserver。后面是游戏类型

	std::map<Lint, Lint>	  m_toEraseLogics;

	std::map<Lint, UserLoginInfo> m_reqLoginUser;//等待登录的玩家队列



	GarbageCollectionService m_gcService;
private:
	bool m_open_ios_pay;
	bool m_open_android_pay;

	


	//分割線
	std::map<Lint, GameServerInfo> m_game_server_info;

private:
	//用于保存user所在的gateserver 
	std::map<Lint, LSocketPtr>		m_users_gate_sp;

};

#define gWork Work::Instance()

#endif