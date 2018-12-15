#ifndef _WORK_H_
#define _WORK_H_

#include "LRunnable.h"
#include "LSingleton.h"
#include "LTime.h"
#include "LMsg.h"
#include "LNet.h"
#include "LMsgS2S.h"
#include "BlockingQueue.h"
#include "DBSession.h"
#include "ActiveManager.h"
#include "LCardsOperType.h"

class ShipInfo
{
public:
	ShipInfo(Lint inviter,Lint fcount, Lint scount)
	{
		m_inviter = inviter;
		m_count = fcount;
		m_secondCount = scount;
		m_inviteeFirst = false;
		m_inviterFirst = false;
		m_inviteeSecond = false;
		m_inviterSecond = false;

	}
	Lint m_inviter;
	Lint m_count;
	Lint m_secondCount;
	bool m_inviteeFirst; //第一次领取
	bool m_inviterFirst; 
	bool m_inviteeSecond; //第二次领取
	bool m_inviterSecond;
private:
	ShipInfo();
};

enum TYPE_DRAW
{
	DRAW_NULL = 0,
	DRAW_BIG_WINNER = 1,
	DRAW_CREATE_ROOM = 2,
	DRAW_FEN_XIANG = 3,
};

struct BaseInfo
{
	Lint	m_id;
	Lint	m_userId;
	Lstring	m_city;
	Lstring m_common_play;

	BaseInfo()
	{
		m_id = 0;
		m_userId = 0;
	}
};

class RewardCfg;

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

public:
	//处理客户端连接上来消息
	void			HanderClientIn(LMsgIn* msg);

	//处理客户端掉线的消息 
	void			HanderUserKick(LMsgKick* msg);

	//清除24小时以上日志， 每天早上6点执行 
	void			ClearOldLogVideo();

public:

	void			SendMsgToMatchServer(LMsg& msg);

	void			SendToClient(LSocketPtr client, LMsg& msg);

	//logicmanager登录进来
	void			HanderLogicManagerLogin(LMsgLMG2LdbLogin* msg);

	//matchserver登录进来
	void	HanderMatchServerLogin(LMsgM2LdbLogin* msg);
	//matchserver请求存储比赛积分
	void    HanderMatchReqSaveScore(LMsgM2LDBScoreboard* msg);
	//matchserver请求存储现金比赛结果信息
	void    HanderMatchSaveResultInfo(LMsgM2DbMatchResultInfo* msg);
	//logic请求积分榜
	void    HanderLogicReqScoreBoard(LMsgL2LDBReqScoreBord* msg);
	//玩家请求比赛记录
	void    HanderRequestRenManMatchRecord(LMsgL2DBRequestRenManMatchRecord* msg);
	//处理新转盘活动的数据刷新
	void	HanderZhuanPanNotice(LMsgS2LDZhuanPanNotice* msg);

	//logic登录进来
	void			HanderLogicLogin(LMsgL2LdbLogin* msg);

	//logic请求玩家战绩
	void			HanderLogicReqVipLog(LMsgL2LBDReqVipLog* msg);

	//logic请求玩家录像
	void			HanderLogicReqVideo(LMsgL2LDBReqVideo* msg);

	//logic请求保存战绩
	void			HanderLogicReqSaveLog(LMsgL2LDBSaveLogItem* msg);

	//logic请求保存录像
	void			HanderLogicReqSaveVideo(LMsgL2LDBSaveVideo* msg);

	//logic请求某分享录像
	void			HanderLogicReqShareVideo(LMsgL2LDBGetShareVideo* msg);

	//logic请求玩家分享的录像
	void			HanderLogicGetShareVideo(LMsgL2LDBGetSsharedVideo* msg);

	//logic请求玩家获取某分享录像Id
	void			HanderLogicGetShareVideoId(LMsgL2LDBGGetShareVideoId* msg);

	//保存share video id
	void			HanderLogicSaveShareVideoId(LMsgL2LDBSaveShareVideoId* msg); 


	//处理抽奖信息请求
	void		HandleLogicReqActive(LMsgL2LDBReqActive* msg);

	//保存抽奖信息
	void		HandleLogicSaveSyncDraw(LMsgL2LDBSyncDraw* msg);

	//修改抽奖电话
	void		HandleLogicUpdatePhone(LMsgL2LDBSyncPhone* msg);	

	void		HanderKeepAlive(LMsgS2SKeepAlive* msg);

	void		HanderUserReward(LMsgLMG2LDBUserReward *msg);

	/******************************************************************/
	// 活动相关
	void        HanderUserLogin(LMsgLMG2LdbUserLogin* msg);
	void        HanderLogicSetActivity(LMsgCE2LSetActivity* msg);
	void        SetDrawActive(const Lstring& strSet, int id);
	
	void        HanderActivityRequestShareForDraw(LMsgLGM2LDBActivityRequestShareForDraw *msg);
	void        HanderAddUserPlayCount(LMsgL2LMGModifyPlayCount* msg);
	void        HanderActivityRequestDrawOpen(LMsgLGM2LDBActivityRequestDrawOpen *msg);
	void        HanderActivityRequestDraw(LMsgLGM2LDBActivityRequestDraw *msg);
	void        HanderActivityRequestLog(LMsgLGM2LDBActivityRequestLog *msg);
	void        HanderActivityPhone(LMsgLGM2LDBActivityWritePhone *msg);
	// 发送给玩家
	void	SendToUser(LSocketPtr LSp,const Lint& userid, const Lstring& uuid ,LMsg& msg);
	/*****************************************************************/
	void        HanderUserCreateMatch(LMsgS2SCreateMatch* msg);
	void        HanderUserDissolveMatch( LMsgS2SDissolveMatch* msg);
	void	    HanderUserCheckCanCreateMatch(LMsgS2SCheckCanCreateMatch* msg);
	/*****************************************************************/
	void		HanderActivityRedPacketBind(LMsgCe2LDBBindingRelation* msg);
	void		HanderDeskUser(LMsgL2LDBDeskUser* msg);
	void		_inviteProcess(Lint invitee, Lint regtime);
	void		HanderRequestRedPacket(LMsgL2DBRequestRedpacket* msg);
	void		SetRedPacketsActive(const Lstring& strSet, int id);
	void		SetRedActivityFalse(Lint invitee, Lint type);
	void		SetRedActivityTrue(Lint invitee, Lint type);

	void		SendToLMUser(const Lint& userid, LMsg& msg);
	void		SaveMysql(Lstring sql);
	void		HanderGetInvitingInfo( LMsgL2DBGetInvitingInfo* msg );
	//昨天的活动设置
	void		_reloadActivity();
	void		_loadFriendship();

	/******************************************************************/
private:
	void		HandleMsgLoop(void);

public:
	ActivityZhuanPanRedpacket*		LoadZhuanPanRedpacketData(Lint userId);
	void							HanderZhuanPanRedpacketOpt(LMsgL2DBZhuanpanRedpacket* msg);
public:
	NewZhuanPanActivity*			LoadNewZhuanPanActivityData(Lint userId);
	bool							ResetNewZhuanPanActivityData(NewZhuanPanActivity* pActivity);
	bool							SaveNewZhuanPanActivityToSql(NewZhuanPanActivity* pActivity);
	int								GetDrawTimes(NewZhuanPanActivity* pActivity);
	TYPE_DRAW						GetDrawType(NewZhuanPanActivity* pActivity);
	void							DrawEndSetData(NewZhuanPanActivity* pActivity,TYPE_DRAW type);
	void							SaveRewardToSql(Lint userId,RewardCfg* pReward);
	void							HanderNewZhuanPanActivityOpt(LMsgL2DBNewZhuanPanActivity* msg);
	void							HanderJiaXiangOpt(LMsgL2DBJiaXiangOpt* msg);
public:
	void							LoadBaseInfoFromSql();
	BaseInfo*						CreateBaseInfo(Lint userId);
	BaseInfo*						GetBaseInfo(Lint userId);
	void							SaveBaseInfo(BaseInfo* pInfo);

private:
	LTime		m_tickTimer;

	Llong		m_30MSceTick;//30毫秒循环一次

	Llong		m_200MSceTick;//200毫秒循环一次

	Llong		m_1SceTick;//1秒循环一次

	Llong		m_5SceTick;

	Llong		m_30SceTick;//30秒循环一次

	Llong		m_600SceTick;//5分钟秒循环一次

private:

	BlockingQueue<LMsg*>		m_queue;
	boost::thread_specific_ptr<DBSession>	m_dbsession;
	DBSession								m_Maindbsession;
	LSocketPtr	m_logicManager;//链接manager
	LSocketPtr	m_MatchServer;//链接MatchServer
	boost::mutex m_mutexUser2recommIdMap;
	boost::recursive_mutex m_Mutex;


private:
	std::map<Lint,ShipInfo> m_user2recommIdMap;
	std::map<Lint, BaseInfo*>	m_mapBaseInfo;
};


#define gWork Work::Instance()

#endif