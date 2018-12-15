#include <boost/thread/scoped_thread.hpp>
#include "Work.h"
#include "LLog.h"
#include "Config.h"
#include "InsideNet.h"
#include "LMsgS2S.h"
#include "DBSession.h"
#include "ActiveManager.h"
#include "HttpRequest.h"
#include "HttpRedPacketThread.h"
#include "MatchResultManager.h"
#include "ActivityConfigTbl.h"
#include "NewZhuanPanActivityTbl.h"

//初始化
bool Work::Init()
{
	//设置log文件
	LLog::Instance().SetFileName("LogicDB");

	//初始化配置文件
	if(!gConfig.Init())
	{
		LLOG_ERROR("gConfig.Init error");
		return false;
	}

	LLog::Instance().SetLogLevel(gConfig.GetLogLevel());

	LMsgFactory::Instance().Init();

	if (!gActiveManager.Init())
	{
		LLOG_ERROR("gActiveManager.Init error");
		return false;
	}

	if (!gInsideNet.Init())
	{
		LLOG_ERROR("gInsideNet.Init error");
		return false;
	}

	if (!gHRP.Init(true))
	{
		LLOG_ERROR("gHRP.init error");
		return false;
	}

	if(!m_Maindbsession.Init(gConfig.GetDbHost(), gConfig.GetDbUser(), gConfig.GetDbPass(), gConfig.GetDbName(), "utf8mb4", gConfig.GetDbPort()))
	{
		return false;
	}

	if (!gMatchResultManager.Init())
	{
		LLOG_ERROR("gMatchResultManager.Init error");
		return false;
	}

	LTime cur;
	m_30MSceTick = cur.MSecs();
	m_200MSceTick = cur.MSecs();
	m_1SceTick = cur.MSecs();
	m_30SceTick = cur.MSecs();
	m_600SceTick = cur.MSecs();

	return true;
}

bool Work::Final()
{
	return true;
}

//启动
void Work::Start()
{
	_loadFriendship();
	gInsideNet.Start();

	gHRP.start();

	LRunnable::Start();

	LoadBaseInfoFromSql();
}

//等待
void Work::Join()
{
	gInsideNet.Join();

	//等待逻辑线程终止
	LRunnable::Join();

}

//停止
void Work::Stop()
{
	gInsideNet.Stop();

	LRunnable::Stop();
}

void Work::Clear()
{

}

void Work::Run()
{
	boost::thread_group threads;
	for (int i = 0; i < gConfig.GetWorkerNum(); i++)
		threads.create_thread(boost::bind(&Work::HandleMsgLoop, this));

	while (!GetStop())
	{
		//这里处理游戏主循环
		LTime cur;
		if(cur.MSecs() - m_tickTimer.MSecs() > 1)
		{
			m_tickTimer = cur;
			Tick(cur);	
		}

		LMsg* msg = (LMsg*)Pop();
		if(msg == NULL)
		{
			Sleep(1);
			continue;
		}
		else
		{
			//HanderMsg(msg);
			//delete msg;
			if(msg->m_msgId == MSG_S_2_S_KEEP_ALIVE)
			{
				HanderKeepAlive((LMsgS2SKeepAlive*)msg);
				delete msg;
			}
			else
				m_queue.Push(msg);
		}
	}

	for (int i = 0; i < threads.size(); i++)
		m_queue.Push(NULL);

	threads.join_all();
}

void Work::Tick(LTime& cur)
{
	//30毫秒循环一次
	if (cur.MSecs() - m_30MSceTick > 30)
	{
		m_30MSceTick = cur.MSecs();
	}

	//200毫秒循环一次
	if (cur.MSecs() - m_200MSceTick > 200)
	{
		m_200MSceTick = cur.MSecs();
	}

	//1000毫秒循环一次
	if (cur.MSecs() - m_1SceTick > 1000)
	{
		m_1SceTick = cur.MSecs();
	}

	//5秒循环一次
	if (cur.MSecs() - m_5SceTick > 5000)
	{
		m_5SceTick = cur.MSecs();
		if (gWork.GetCurTime().GetHour() == 0 && gWork.GetCurTime().GetMin() ==0 &&(gWork.GetCurTime().GetSec() >= 0 && gWork.GetCurTime().GetSec() < 5))
		{
			gActiveManager.ReloadRedPacketActivitySetTomorrow();
			//清空钱数
			gActiveManager.redisWriteDataByString(NewZhuanPanActivityDailyDrawMoney,0);
		}
	}

	//30秒循环一次
	if (cur.MSecs() - m_30SceTick > 30*1000)
	{
		m_30SceTick = cur.MSecs();

		ActivityConfigTbl::Instance().LoadActivityConfg();
		NewZhuanPanActivityTbl::Instance().LoadNewZhuanPanTbl();
	}

	//5分钟循环一次
	if (cur.MSecs() - m_600SceTick > 5*60 * 1000)
	{
		m_600SceTick = cur.MSecs();
		//删除LOG
		if (gWork.GetCurTime().GetHour() == 3 && (gWork.GetCurTime().GetMin() >= 0 && gWork.GetCurTime().GetMin() < 5) )
		{
			boost::scoped_thread<boost::detach> t((boost::thread(boost::bind(&Work::ClearOldLogVideo, this))));
		}
	}
}

LTime& Work::GetCurTime()
{
	return m_tickTimer;
}

void Work::HanderMsg(LMsg* msg)
{
	//玩家请求登录
	switch(msg->m_msgId)
	{
	case MSG_CLIENT_KICK:
		HanderUserKick((LMsgKick*)msg);
		break;
	case MSG_CLIENT_IN:
		HanderClientIn((LMsgIn*)msg);
		break;
	//////////////////////////////////////////////////////////////////////////
	//logicmanager 与logicdb
	case MSG_LMG_2_LDB_LOGIN:
		HanderLogicManagerLogin((LMsgLMG2LdbLogin*)msg);
		break;
	//matchserver与logicdb
	case MSG_M_2_LDB_LOGIN:
		HanderMatchServerLogin((LMsgM2LdbLogin*)msg);
		break;
	case MSG_M_2_LDB_SCOREBORD:
		HanderMatchReqSaveScore((LMsgM2LDBScoreboard*)msg);
		break;
	case MSG_M_2_LDB_MATCH_RESULT_INFO:
		HanderMatchSaveResultInfo((LMsgM2DbMatchResultInfo*)msg);
		break;
	//logic 跟 logicdb 之间的消息交互
	case MSG_L_2_LDB_LOGIN:
		HanderLogicLogin((LMsgL2LdbLogin*)msg);
		break;
	case MSG_L_2_LDB_VIDEO:
		HanderLogicReqVideo((LMsgL2LDBReqVideo*) msg);
		break;
	case MSG_L_2_LDB_ROOM_SAVE:
		HanderLogicReqSaveLog((LMsgL2LDBSaveLogItem*) msg);
		break;
	case MSG_L_2_LDB_VIDEO_SAVE:
		HanderLogicReqSaveVideo((LMsgL2LDBSaveVideo*) msg);
		break;
	case MSG_L_2_LDB_VIP_LOG:
		HanderLogicReqVipLog((LMsgL2LBDReqVipLog*) msg);
		break;
	case MSG_L_2_LDB_REQ_ACTIVE:
		HandleLogicReqActive((LMsgL2LDBReqActive*) msg);
		break;
	case MSG_L_2_LDB_SYC_DRAW:
		HandleLogicSaveSyncDraw((LMsgL2LDBSyncDraw*) msg);
		break;
	case MSG_L_2_LDB_SYC_PHONE:
		HandleLogicUpdatePhone((LMsgL2LDBSyncPhone*) msg);
		break;
	case MSG_L_2_LDB_SCOREBORD:
		HanderLogicReqScoreBoard((LMsgL2LDBReqScoreBord*)msg);
		break;
	case MSG_LMG_2_LDB_USER_REWARD:
		HanderUserReward((LMsgLMG2LDBUserReward*) msg);
		break;
	/****************************************************/
		//玩家登陆时发送活动信息
	case MSG_LMG_2_LDB_USER_LOGIN:
		HanderUserLogin((LMsgLMG2LdbUserLogin*)msg);
		break;
		//玩家分享获得一次抽奖机会
	case MSG_LMG_2_LDB_ACTIVITY_REQUEST_SHARE_FOR_DRAW:
		HanderActivityRequestShareForDraw((LMsgLGM2LDBActivityRequestShareForDraw *)msg);
		break;
		//玩家打牌获得抽奖机会
	case MSG_L_2_LMG_MODIFY_USER_PLAYCOUNT:
		HanderAddUserPlayCount((LMsgL2LMGModifyPlayCount*)msg);
		break;
		//玩家打开转盘
	case MSG_LMG_2_LDB_ACTIVITY_REQUEST_DRAW_OPEN:
		HanderActivityRequestDrawOpen((LMsgLGM2LDBActivityRequestDrawOpen *)msg);
		break;
		//玩家请求抽奖
	case MSG_LMG_2_LDB_ACTIVITY_REQUEST_DRAW:
		HanderActivityRequestDraw((LMsgLGM2LDBActivityRequestDraw *)msg);
		break;
		//玩家请求抽奖记录
	case MSG_LMG_2_LDB_ACTIVITY_REQUEST_LOG:
		HanderActivityRequestLog((LMsgLGM2LDBActivityRequestLog *)msg);
		break;
		//玩家填写领奖号码
	case MSG_LMG_2_LDB_ACTIVITY_WRITE_PHONE:
		HanderActivityPhone((LMsgLGM2LDBActivityWritePhone *)msg);
		break;
	case MSG_CE_2_L_SET_ACTIVITY:
		HanderLogicSetActivity((LMsgCE2LSetActivity*)msg);
		break;
	/****************************************************/
	case MSG_S_2_S_CREATE_MATCH:
		HanderUserCreateMatch((LMsgS2SCreateMatch*)msg);
		break;
	case MSG_S_2_S_DISSOLVE_MATCH:
		HanderUserDissolveMatch((LMsgS2SDissolveMatch*)msg);
		break;
	case MSG_S_2_S_CHECK_CAN_CREATE_MATCH:
		HanderUserCheckCanCreateMatch((LMsgS2SCheckCanCreateMatch*)msg);
		break;
	/****************************************************/
		//logic 玩家请求分享某录像，先去sharevideo中查看是否已被分享
	case MSG_L_2_LDB_GETSHAREVIDEOID:
		HanderLogicGetShareVideoId((LMsgL2LDBGGetShareVideoId*) msg);
		break;
		//logic 请求保存某已转移到sharevideo中的某录像的shareid
	case MSG_L_2_LDB_SAVESHAREVIDEOID:
		HanderLogicSaveShareVideoId((LMsgL2LDBSaveShareVideoId*) msg);
		break;
		//logic 玩家通过shareid来获取该分享码下的录像 在sharevideo中查询
	case MSG_L_2_LDB_GETSHAREVIDEO:
		HanderLogicReqShareVideo((LMsgL2LDBGetShareVideo*) msg);
		break;
		//loigc 通过精确的videoid来查找sharevideo中的对应录像
	case MSG_L_2_LDB_GETSHAREVIDEOBYVIDEOID:
		HanderLogicGetShareVideo((LMsgL2LDBGetSsharedVideo*) msg);
		break;
	case MSG_S_2_S_KEEP_ALIVE:
		HanderKeepAlive((LMsgS2SKeepAlive*)msg);
		break;
	case MSG_CE_2_LDB_BINDING_RELATION:
		HanderActivityRedPacketBind((LMsgCe2LDBBindingRelation*)msg);
		break;
	case MSG_L_2_LDB_DESK_USER_INFO:
		HanderDeskUser((LMsgL2LDBDeskUser*)msg);
		break;
	case MSG_L_2_LDB_REQUEST_REDPACKET:
		HanderRequestRedPacket((LMsgL2DBRequestRedpacket*)msg);
		break;
	case MSG_DB_2_L_GET_INVITING_INFO:
		HanderGetInvitingInfo((LMsgL2DBGetInvitingInfo*)msg);
		break;
	case MSG_L_2_LDB_ZHUAN_PAN_REDPACKET:
		HanderZhuanPanRedpacketOpt((LMsgL2DBZhuanpanRedpacket*)msg);
		break;
	case MSG_L_2_LDB_REQUEST_REN_MAN_MATCH_RECORD:
		HanderRequestRenManMatchRecord((LMsgL2DBRequestRenManMatchRecord*) msg);
		break;
	case MSG_S_2_LDB_ZhuanPan_Notice:
		HanderZhuanPanNotice((LMsgS2LDZhuanPanNotice*) msg);
		break;
	case MSG_L_2_LDB_NEW_ZHUAN_PAN_ACTIVITY:
		HanderNewZhuanPanActivityOpt((LMsgL2DBNewZhuanPanActivity*)msg);
		break;
	case MSG_L_2_LDB_JIA_XIANG_OPT:
		HanderJiaXiangOpt((LMsgL2DBJiaXiangOpt*)msg);
		break;
	default:
		LLOG_ERROR("Unknown messgae, id = %d", msg->m_msgId);
		break;
	}
}

void Work::HanderUserKick(LMsgKick* msg)
{
	
}

void Work::HanderClientIn(LMsgIn* msg)
{
	
}

void Work::ClearOldLogVideo()
{
	DBSession session;
	if (!session.Init(gConfig.GetDbHost(), gConfig.GetDbUser(), gConfig.GetDbPass(), gConfig.GetDbName(), "utf8mb4", gConfig.GetDbPort()))
	{
		LLOG_ERROR("database session init error");
		return;
	}
	MYSQL* m = session.GetMysql();

	if (m == NULL)
	{
		LLOG_ERROR("Work::ClearOldLogVideo mysql null");
		return;
	}
	std::stringstream ss;	
	ss << "delete from video where time < '"; 
	ss << gWork.GetCurTime().Secs() - 24*60*60 << "'"; 

	session.Excute(ss.str());

	ss.str("");
	ss << "delete from log where time < '"; 
	ss << gWork.GetCurTime().Secs() - 24*60*60*7 << "'"; 

	session.Excute(ss.str());

	LLOG_ERROR("Work::ClearOldLogVideo");
}

void Work::SendMsgToMatchServer(LMsg & msg)
{
	if (m_MatchServer)
		m_MatchServer->Send(msg.GetSendBuff());
}

void Work::SendToClient(LSocketPtr client, LMsg & msg)
{
	if (client)
		client->Send(msg.GetSendBuff());
}
//logicmanager
void Work::HanderLogicManagerLogin(LMsgLMG2LdbLogin* msg)
{
	if(msg->m_key != gConfig.GetLogicDBKey())
	{
		LLOG_ERROR("Work::HanderLogicLogin key error %s",msg->m_key.c_str());
		return;
	}

	LLOG_ERROR("Logic Manager login");

	m_logicManager = msg->m_sp;
}

//matchserver登录
void Work::HanderMatchServerLogin(LMsgM2LdbLogin* msg)
{
	if (msg == NULL)
	{
		return;
	}

	m_MatchServer = msg->m_sp;
	gMatchResultManager.LoadMatchResultInfo(msg->m_serverID);
	LLOG_ERROR("Match Server login");
	//m_MatchServer = msg->m_sp;
}

void Work::HanderMatchReqSaveScore(LMsgM2LDBScoreboard* msg)
{
	if (msg)
	{
		LLOG_DEBUG("Work::HanderMatchReqSaveScore save %d:%s", msg->m_roomId, msg->m_insertSql.c_str());
		if (!m_dbsession->Excute(msg->m_insertSql.c_str()))
		{
			LLOG_ERROR("Work::HanderMatchReqSaveScore Error");
		}
	}
}

void Work::HanderMatchSaveResultInfo(LMsgM2DbMatchResultInfo* msg)
{
	if (msg)
	{
		LLOG_DEBUG("Work::HanderMatchSaveResultInfo");
		gMatchResultManager.SaveResultInfo(msg->m_info);
	}
}

//logic登录进来
void Work::HanderLogicLogin(LMsgL2LdbLogin* msg)
{

	LLOG_ERROR("Logic Server login");

	//if(msg->m_key != gConfig.GetLogicDBKey())
	//{
	//	LLOG_ERROR("Work::HanderLogicLogin key error %s",msg->m_key.c_str());
	//	return;
	//}
	//auto logic = m_logicServer.begin();
	//for (; logic != m_logicServer.end(); ++ logic)
	//{
	//	if (logic->second == msg->m_sp)
	//	{
	//		return;
	//	}
	//}
	//
	//m_logicServer[msg->m_serverID] = msg->m_sp;

}

//logic请求玩家积分榜
void Work::HanderLogicReqScoreBoard(LMsgL2LDBReqScoreBord* msg)
{
	LMsgLDB2LReqScoreBord log;
	log.m_roomId = msg->m_roomId;
	log.m_strUUID = msg->m_strUUID;
	log.m_userid = msg->m_userId;
	log.m_count = 0;

	MYSQL* pMySQL = m_dbsession->GetMysql();

	if (pMySQL == NULL)
	{
		LLOG_ERROR("Don't get SQL session");
		msg->m_sp->Send(log.GetSendBuff());
		return;
	}

	LLOG_ERROR("Request scoreBoard. %d:%d", msg->m_userId, msg->m_roomId);
	//log id
	std::stringstream ss;
	ss << "SELECT userInfo,matchTime FROM matchscore WHERE matchId='";
	ss << msg->m_roomId << "'";

	if (mysql_real_query(pMySQL, ss.str().c_str(), ss.str().size()))
	{
		LLOG_ERROR("UserManager::HanderLogicReqVideo sql error %s", mysql_error(pMySQL));
		msg->m_sp->Send(log.GetSendBuff());
		return;
	}

	MYSQL_RES* res = mysql_store_result(pMySQL);
	MYSQL_ROW row = mysql_fetch_row(res);
	if (row)
	{
		Lstring str = *row++;
		Lint matchtime = atoi(*row);
		log.m_time = matchtime;
		std::vector<Lstring> des, subDes;
		L_ParseString(str, des, ";");

		for (Lint i = 0; i < des.size(); i++)
		{
			MatchScoreNotice score_notice;
			subDes.clear();
			L_ParseString(des[i], subDes, ",");
			if (subDes.size() == 2)
			{
				score_notice.m_id = atoi(subDes[0].c_str());
				score_notice.m_score = atoi(subDes[1].c_str());

				log.UserScoreVec.push_back(score_notice);
				log.m_count++;
			}
		}
	}

	mysql_free_result(res);

	msg->m_sp->Send(log.GetSendBuff());
}

//请求玩家个人比赛记录
void Work::HanderRequestRenManMatchRecord(LMsgL2DBRequestRenManMatchRecord* msg)
{
	if (msg == NULL)
	{
		LLOG_ERROR("Work::HanderRequestRenManMatchRecord(LMsgL2DBRequestRenManMatchRecord* msg) msg is null");
		return;
	}

	MYSQL* pMySQL = m_dbsession->GetMysql();
	

	if (pMySQL == NULL)
	{
		LLOG_ERROR("HanderRequestRenManMatchRecord Don't get SQL session");
		return;
	}

	LLOG_ERROR("HanderRequestRenManMatchRecord. %d ", msg->m_userId);
	//log id

	if (msg->m_type == 0)
	{
		LMsgS2CRequestRenManMatchRecordRet renManMatchRecord;

		std::stringstream ss;
		ss << "SELECT matchRoomIndex,matchName,matchTime,mc,matchCardLevel,rewardType,rewardCount,lastTurn,playState,athletesNum,promotionNum,taoTai FROM RenManMatchRecord WHERE userId='";
		ss << msg->m_userId << "' ORDER BY matchTime DESC LIMIT 20";

		if (mysql_real_query(pMySQL, ss.str().c_str(), ss.str().size()))
		{
			LLOG_ERROR("Work::HanderRequestRenManMatchRecord sql error %s", mysql_error(pMySQL));
			return;
		}

		renManMatchRecord.m_userId = msg->m_userId;

		MYSQL_RES* res = mysql_store_result(pMySQL);
		MYSQL_ROW row = mysql_fetch_row(res);
		while (row)
		{
			RenManMatchRecordInfo recordinfo;
			recordinfo.m_MatchRoomIndex = atoi(*row++);
			recordinfo.m_MatchName = *row++;
			recordinfo.m_StartTime = atoi(*row++);
			recordinfo.m_Mc = atoi(*row++);
			recordinfo.m_MatchLevelCard = atoi(*row++);
			recordinfo.m_RewardType = atoi(*row++);
			recordinfo.m_RewardCounts = atoi(*row++);
			recordinfo.m_LastTurn = atoi(*row++);
			recordinfo.m_PlayState = atoi(*row++);
			recordinfo.m_AthletesNum = atoi(*row++);
			recordinfo.m_PromotionNum = atoi(*row++);
			recordinfo.m_IsTaotai = atoi(*row++);

			row = mysql_fetch_row(res);

			renManMatchRecord.m_VecRenManMatchRecordInfo.push_back(recordinfo);
		}

		mysql_free_result(res);

		SendToLMUser(msg->m_userId, renManMatchRecord);
	}
}


//logic请求玩家战绩
void Work::HanderLogicReqVipLog(LMsgL2LBDReqVipLog* msg)
{
	if(!msg)
		return;

	MYSQL* m = m_dbsession->GetMysql();

	LMsgLBD2LReqVipLog log;
	log.m_userid = msg->m_userId;
	log.m_count = 0;
	if (m == NULL)
	{
		LLOG_ERROR("Work::HanderLogicReqVipLog mysql null");
		SendToClient(msg->m_sp, log);
		return;
	}

	LLOG_DEBUG("Work::HanderLogicReqVipLog %d:%d",msg->m_userId,msg->m_reqUserId);
	//log id
	std::stringstream ss;	
	ss << "SELECT Id,Time,Pos1,Pos2,Pos3,Pos4,Flag,DeskId,MaxCircle,CurCircle,Pass,Score1,Score2,Score3,Score4,Reset,Data,PlayType FROM Log WHERE (Pos1=";
	ss << msg->m_reqUserId << " OR Pos2=";
	ss << msg->m_reqUserId << " OR Pos3=";
	ss << msg->m_reqUserId << " OR Pos4=";
	ss << msg->m_reqUserId << ") AND ";
	ss << "Time>" << gWork.GetCurTime().Secs()-24*60*60;

	ss << " ORDER BY Time DESC";
	LLOG_DEBUG("Work::HanderLogicReqVipLog %s", ss.str().c_str());
	if (mysql_real_query(m, ss.str().c_str(), ss.str().size()))
	{
		LLOG_ERROR("Work::HanderLogicReqVipLog sql error %s", mysql_error(m));
		SendToClient(msg->m_sp, log);
		return;
	}

	MYSQL_RES* res = mysql_store_result(m);
	MYSQL_ROW row = mysql_fetch_row(res);
	while (row)
	{
		LogInfo& info = log.m_info[log.m_count++];
		info.m_id = *row++;
		info.m_time = atoi(*row++);
		info.m_posUserId[0] = atoi(*row++);
		info.m_posUserId[1] = atoi(*row++);
		info.m_posUserId[2] = atoi(*row++);
		info.m_posUserId[3] = atoi(*row++);
		info.m_flag = atoi(*row++);
		info.m_deskId = atoi(*row++);
		info.m_maxCircle = atoi(*row++);
		info.m_curCircle = atoi(*row++);
		info.m_secret = *row++;
		info.m_score[0] = atoi(*row++);
		info.m_score[1] = atoi(*row++);
		info.m_score[2] = atoi(*row++);
		info.m_score[3] = atoi(*row++);
		info.m_reset = atoi(*row++);
		info.m_data = *row++;
		info.m_playtype = *row++;

		row = mysql_fetch_row(res);
		if(log.m_count >= 10)
			break;
	}

	mysql_free_result(res);
	SendToClient(msg->m_sp, log);
}

//logic请求玩家录像
void Work::HanderLogicReqVideo(LMsgL2LDBReqVideo* msg)
{
	LMsgLDB2LReqVideo log;
	log.m_userid = msg->m_userId;
	log.m_count = 0;

	MYSQL* m = m_dbsession->GetMysql();

	if (m == NULL)
	{
		LLOG_ERROR("Work::HanderLogicReqVipLog mysql null");
		msg->m_sp->Send(log.GetSendBuff());
		return;
	}

	//log id
	std::stringstream ss;	
	ss << "SELECT Id,Time,UserId1,UserId2,UserId3,UserId4,Zhuang,DeskId,MaxCircle,CurCircle,Score1,Score2,Score3,Score4,Flag,Data, PlayType,serverId,resultData FROM Video WHERE Id='";
	ss << msg->m_videoId<<"'";

	if (mysql_real_query(m, ss.str().c_str(), ss.str().size()))
	{
		LLOG_ERROR("UserManager::HanderLogicReqVideo sql error %s", mysql_error(m));
		SendToClient(msg->m_sp, log);
		return;
	}

	MYSQL_RES* res = mysql_store_result(m);
	MYSQL_ROW row = mysql_fetch_row(res);
	if (row)
	{
		VideoLog& info = log.m_video;
		info.m_Id = *row++;
		info.m_time = atoi(*row++);

		info.m_userId[0] = atoi(*row++);
		info.m_userId[1] = atoi(*row++);
		info.m_userId[2] = atoi(*row++);
		info.m_userId[3] = atoi(*row++);

		info.m_zhuang = atoi(*row++);
		info.m_deskId = atoi(*row++);
		info.m_maxCircle = atoi(*row++);
		info.m_curCircle = atoi(*row++);

		info.m_score[0] = atoi(*row++);
		info.m_score[1] = atoi(*row++);
		info.m_score[2] = atoi(*row++);
		info.m_score[3] = atoi(*row++);

		info.m_flag = atoi(*row++);

		info.m_str = *row++;

		info.m_str_playtype = *row++;
		info.m_nAppId = atoi(*row++);
		info.m_str_result_data = *row++;
		log.m_count = 1;
	}

	mysql_free_result(res);
	SendToClient(msg->m_sp, log);
}

void Work::HandleLogicReqActive(LMsgL2LDBReqActive* msg)
{
	if (msg == NULL || msg->m_activeId <= 0 || msg->m_begin_date <= 0)
	{
		LLOG_ERROR("Work::HandleLogicReqActive:: msg is error.");
		return;
	}
	
	{
		//检测玩家是否有抽卡记录
		MYSQL* m = m_dbsession->GetMysql();

		if (m == NULL)
		{
			LLOG_ERROR("Work::HandleLogicReqActive:: mysql is error.");
			return;
		}

		std::stringstream ss;	
		ss << "SELECT COUNT(*) FROM activelog WHERE ";
		ss << "activeID = " << msg->m_activeId;
		ss << " AND ";
		ss << " reward = " << msg->m_limit_gift_id;
		ss << " AND ";
		ss << " date >= " << msg->m_begin_date;
		ss << ";";

		if (mysql_real_query(m, ss.str().c_str(), ss.str().size()))
		{
			LLOG_ERROR("UserManager::HandleLogicReqActive sql error %s", mysql_error(m));
			return ;
		}
		

		LLOG_DEBUG("Work::HandleLogicReqActive");

		Lint nCount = 0;
		LMsgLDB2LActiveInfo activeinfo;
		activeinfo.m_active_id = msg->m_activeId;
		activeinfo.m_reward_id = msg->m_limit_gift_id;

		MYSQL_RES* res = mysql_store_result(m);
		MYSQL_ROW row = mysql_fetch_row(res);
		while (row)
		{
			activeinfo.m_draw_limit_count = atoi(*row++);
			row = mysql_fetch_row(res);
		}
		mysql_free_result(res);

		SendToClient(msg->m_sp, activeinfo);
	}
}

void Work::HandleLogicSaveSyncDraw(LMsgL2LDBSyncDraw* msg)
{
	if (msg == NULL)
	{
		return;
	}
	if (msg->m_userId > 0 )
	{
		std::stringstream ss;	
		ss << "insert into activelog(activeID, userID, reward, phone, date) values(";
		ss << msg->m_activeID << ",";
		ss << msg->m_userId << ",";
		ss << msg->m_giftID << ",";
		ss <<"''" << ",";
		ss << msg->m_date << ")";

		m_dbsession->Excute(ss.str());
	}

}

void Work::HandleLogicUpdatePhone(LMsgL2LDBSyncPhone* msg)
{
	if (msg == NULL)
	{
		return;
	}
	if (msg->m_userId > 0 )
	{
		std::stringstream ss;	
// 		ss << "update activelog set phone = '"; //13846181478' where date = '20160507' and activeID = 1001;
// 		ss << msg->m_phone << "' where date = ";
// 		ss << msg->m_date << " and activeID = ";
// 		ss << msg->m_activeID << " and userID = ";
// 		ss << msg->m_userId << " ";

		ss << " update activelog set ";
		ss << "  phone = " << msg->m_phone << " ";
		ss << " where id in ( select lastid from ( select max(id) as lastid from activelog where ";
		ss << " date = " << msg->m_date << " ";
		ss << " and ";
		ss << " activeID = " << msg->m_activeID << " ";
		ss << " and ";
		ss << " userID = " << msg->m_userId << " ";
		ss << ") as temp);";

		m_dbsession->Excute(ss.str());

		LLOG_ERROR("Work::HandleLogicUpdatePhone: %s", ss.str().c_str());
	}
}

//logic请求保存战绩
void Work::HanderLogicReqSaveLog(LMsgL2LDBSaveLogItem* msg)
{
	LLOG_DEBUG("Work::HanderLogincSaveVipLog save %d:%s",msg->m_type,msg->m_sql.c_str());

	m_dbsession->Excute(msg->m_sql);
}

//logic请求保存录像
void Work::HanderLogicReqSaveVideo(LMsgL2LDBSaveVideo* msg)
{
	LLOG_DEBUG("Work::HanderLogincSaveVideo save %d:%s", msg->m_type, msg->m_sql.c_str());

	m_dbsession->Excute(msg->m_sql);
}


void Work::HanderLogicReqShareVideo(LMsgL2LDBGetShareVideo* msg)
{
	if(msg==NULL)
	{
		return;
	}
	LMsgLDB2LRetShareVideo log;
	log.m_userid = msg->m_userId;

	MYSQL* m = m_dbsession->GetMysql();

	if (m == NULL)
	{
		LLOG_ERROR("Work::HanderLogicReqVipLog mysql null");
		SendToClient(msg->m_sp, log);
		return;
	}

	//log id
	std::stringstream ss;	
	ss << "SELECT VideoId,Time,UserId1,UserId2,UserId3,UserId4,Score1,Score2,Score3,Score4,serverID FROM sharevideo WHERE ShareID='";
	ss << msg->m_shareId<<"'";

	if (mysql_real_query(m, ss.str().c_str(), ss.str().size()))
	{
		LLOG_ERROR("UserManager::HanderLogicReqShareVideo sql error %s", mysql_error(m));
		SendToClient(msg->m_sp, log);
		return;
	}

	MYSQL_RES* res = mysql_store_result(m);
	MYSQL_ROW row = mysql_fetch_row(res);
	while (row)
	{
		ShareVideoInfo info;
		info.m_videoId = *row++;
		info.m_time = atoi(*row++);

		info.m_userId[0] = atoi(*row++);
		info.m_userId[1] = atoi(*row++);
		info.m_userId[2] = atoi(*row++);
		info.m_userId[3] = atoi(*row++);

		info.m_score[0] = atoi(*row++);
		info.m_score[1] = atoi(*row++);
		info.m_score[2] = atoi(*row++);
		info.m_score[3] = atoi(*row++);
		log.m_count++;
		log.m_info.push_back(info);
		row = mysql_fetch_row(res);
	}
	mysql_free_result(res);
	SendToClient(msg->m_sp, log);
}

void Work::HanderLogicGetShareVideoId(LMsgL2LDBGGetShareVideoId* msg)
{
	if(msg==NULL)
	{
		return;
	}
	LMsgLDB2LRetShareVideoId sharevideo_id;
	sharevideo_id.m_userId = msg->m_userId;
	sharevideo_id.m_videoId = msg->m_videoId;
	//加载所有的玩家

	MYSQL* m = m_dbsession->GetMysql();

	if (m == NULL)
	{
		LLOG_ERROR("Work::HanderLogicReqVipLog mysql null");
		SendToClient(msg->m_sp, sharevideo_id);
		return;
	}

	//log id
	std::stringstream ss;	
	ss << "SELECT ShareId FROM sharevideo WHERE VideoId='";
	ss << msg->m_videoId<<"'";

	if (mysql_real_query(m, ss.str().c_str(), ss.str().size()))
	{
		LLOG_ERROR("UserManager::HanderLogicGetShareVideoId _ 1 _ sql error %s", mysql_error(m));
		SendToClient(msg->m_sp, sharevideo_id);
		return;
	}

	MYSQL_RES* res = mysql_store_result(m);
	MYSQL_ROW row = mysql_fetch_row(res);
	if (row)
	{
		sharevideo_id.m_shareId = atoi(*row++);
	}
	mysql_free_result(res);

	if(sharevideo_id.m_shareId==0)
	{
		std::stringstream ss;	
		ss << "SELECT UserId1,UserId2,UserId3,UserId4,Score1,Score2,Score3,Score4,Time,Zhuang,Data,PlayType,DeskId,Flag,CurCircle,MaxCircle,serverID,resultData FROM video WHERE Id='";
		ss << msg->m_videoId<<"'";

		if (mysql_real_query(m, ss.str().c_str(), ss.str().size()))
		{
			LLOG_ERROR("UserManager::HanderLogicGetShareVideoId _ 2 _ sql error %s", mysql_error(m));
			SendToClient(msg->m_sp, sharevideo_id);
			return;
		}

		MYSQL_RES* res = mysql_store_result(m);
		MYSQL_ROW row = mysql_fetch_row(res);
		Lint userId[4];
		Lint Score[4];
		Lint time;
		Lint zhuang;
		Lstring data;
		Lstring playtype;
		Lint deskid;
		Lint flag;
		Lint curcircle;
		Lint maxcircle;
		Lint serverId;
		Lstring result_data;

		if (row)
		{
			userId[0]= atoi(*row++);
			userId[1]= atoi(*row++);
			userId[2]= atoi(*row++);
			userId[3]= atoi(*row++);
			Score[0] = atoi(*row++);
			Score[1] = atoi(*row++);
			Score[2] = atoi(*row++);
			Score[3] = atoi(*row++);
			time = atoi(*row++);
			zhuang = atoi(*row++);
			data = *row++;
			playtype = *row++;
			deskid = atoi(*row++);
			flag = atoi(*row++);
			curcircle = atoi(*row++);
			maxcircle = atoi(*row++);
			serverId = atoi(*row++);
			result_data = *row++;

			std::stringstream ss1;	
			ss1 << "insert into sharevideo(VideoId,time,UserId1,UserId2,UserId3,UserId4,Score1,Score2,Score3,Score4,Zhuang,Data,PlayType,DeskId,Flag,MaxCircle,CurCircle,serverID,resultData) values('";
			ss1 << msg->m_videoId.c_str() <<"',"<<time<<","<<userId[0]<<","<<userId[1]<<","<<userId[2]<<","<<userId[3]<<","<<Score[0]<<","<<Score[1];
			ss1 << ","<<Score[2]<<","<<Score[3] << ","<<zhuang <<",'"<< data.c_str() <<"','" << playtype.c_str() <<"'," << deskid << "," << flag << "," << maxcircle <<","<< curcircle <<"," << serverId <<",'" << result_data.c_str() <<"')";

			LLOG_DEBUG("%s", ss1.str().c_str());
			m_dbsession->Excute(ss1.str());
		}
		else{
			sharevideo_id.m_shareId = -1;
		}

		mysql_free_result(res);

	}
	SendToClient(msg->m_sp, sharevideo_id);
}

void Work::HanderLogicSaveShareVideoId(LMsgL2LDBSaveShareVideoId* msg)
{
	if (msg == NULL)
	{
		return;
	}
	if (msg->m_videoId != "" )
	{
		//log id
		std::stringstream ss;
		ss << "update sharevideo set ShareId = "<< msg->m_shareId <<" where Videoid = '"<< msg->m_videoId << "' ";

		LLOG_DEBUG("%s", ss.str().c_str());
		m_dbsession->Excute(ss.str());
	}
}

void Work::HanderLogicGetShareVideo(LMsgL2LDBGetSsharedVideo* msg)
{
	LMsgLDB2LReqVideo log;
	log.m_userid = msg->m_userId;
	log.m_count = 0;

	MYSQL* m = m_dbsession->GetMysql();

	if (m == NULL)
	{
		LLOG_ERROR("Work::HanderLogicReqVipLog mysql null");
		SendToClient(msg->m_sp, log);
		return;
	}

	//log id
	std::stringstream ss;	
	ss << "SELECT VideoId,Time,UserId1,UserId2,UserId3,UserId4,Zhuang,DeskId,MaxCircle,CurCircle,Score1,Score2,Score3,Score4,Flag,Data,PlayType,resultData FROM sharevideo WHERE VideoId='";
	ss << msg->m_videoId<<"'";

	if (mysql_real_query(m, ss.str().c_str(), ss.str().size()))
	{
		LLOG_ERROR("UserManager::HanderLogicGetShareVideo sql error %s", mysql_error(m));
		SendToClient(msg->m_sp, log);
		return;
	}

	MYSQL_RES* res = mysql_store_result(m);
	MYSQL_ROW row = mysql_fetch_row(res);
	if (row)
	{
		VideoLog& info = log.m_video;
		info.m_Id = *row++;
		info.m_time = atoi(*row++);

		info.m_userId[0] = atoi(*row++);
		info.m_userId[1] = atoi(*row++);
		info.m_userId[2] = atoi(*row++);
		info.m_userId[3] = atoi(*row++);

		info.m_zhuang = atoi(*row++);
		info.m_deskId = atoi(*row++);
		info.m_maxCircle = atoi(*row++);
		info.m_curCircle = atoi(*row++);
		
		info.m_score[0] = atoi(*row++);
		info.m_score[1] = atoi(*row++);
		info.m_score[2] = atoi(*row++);
		info.m_score[3] = atoi(*row++);

		info.m_flag = atoi(*row++);

		info.m_str = *row++;

		info.m_str_playtype = *row++;
		info.m_str_result_data = *row++;
		log.m_count = 1;
	}

	mysql_free_result(res);
	SendToClient(msg->m_sp, log);
}

//void Work::HandleLMGHeartBeat(LMsgLMG2LDBHEARTBEAT* msg)
//{
//	LLOG_DEBUG("HeartBeat: LMG_2_LDB");
//	if (msg == NULL)
//	{
//		return;
//	}
//	LMsgLDB2LMGHEARTBEAT_BACK back;
//	SendToClient(msg->m_sp, back);
//}
//
//void Work::HandleLHeartBeat(LMsgL2LDBHEARTBEAT* msg)
//{
//	LLOG_DEBUG("HeartBeat: L_2_LDB");
//	if (msg == NULL)
//	{
//		return;
//	}
//	LMsgLDB2LHEARTBEAT_BACK back;
//	SendToClient(msg->m_sp, back);
//}


//void Work::HandleExecSQL(LMsgL2LDBExecSql * msg)
//{
//	if (msg == NULL)
//	{
//		return;
//	}
//	LLOG_DEBUG("Work::HandleExecSQL sql=%s", msg->m_sql.c_str());
//	m_dbsession->Excute(msg->m_sql);
//}
void Work::HandleMsgLoop(void)
{
	m_dbsession.reset(new DBSession);
	if (!m_dbsession->Init(gConfig.GetDbHost(), gConfig.GetDbUser(), gConfig.GetDbPass(), gConfig.GetDbName(), "utf8mb4", gConfig.GetDbPort()))
	{
		LLOG_ERROR("m_dbsession.Init error");
		return;
	}
	stringstream ss;
	ss << boost::this_thread::get_id();
	LLOG_ERROR("Thread %s:connecting to database succeed.", ss.str().c_str());

	while (true)
	{
		LMsg* msg = m_queue.Pop();
		if (msg != NULL)
		{
			HanderMsg(msg);
			delete msg;
		}
		else
			break;
	}
}

void Work::HanderKeepAlive(LMsgS2SKeepAlive* msg)
{
	if(msg == NULL || !msg->m_sp)
		return;
	LLOG_DEBUG("KeepAlive from %s:%d received.", msg->m_sp->GetRemoteIp().c_str(), msg->m_sp->GetRemotePort());

	LMsgS2SKeepAliveAck ack;
	msg->m_sp->Send(ack.GetSendBuff());
}

void Work::HanderUserReward(LMsgLMG2LDBUserReward *msg)
{
	if (msg == NULL)
	{
		return;
	}
	if (msg->m_user_id > 0 && msg->m_active_id > 0)
	{
		//检测玩家是否有抽卡记录
		MYSQL* m = m_dbsession->GetMysql();
		if (m == NULL)
		{
			return;
		}

		std::stringstream ss;	
		ss << "select reward, date, phone from activelog where userID = ";
		ss << msg->m_user_id;
		ss << " and activeID = ";
		ss << msg->m_active_id;
		ss <<" order by date desc ";

		if (mysql_real_query(m, ss.str().c_str(), ss.str().size()))
		{
			LLOG_ERROR("UserManager::HanderUserReward sql error %s", mysql_error(m));
			return ;
		}

		LMsgLDB2LMGUserReward user_reward;
		user_reward.m_userid = msg->m_user_id;

		MYSQL_RES* res = mysql_store_result(m);
		MYSQL_ROW row = mysql_fetch_row(res);
		while (row)
		{
			LActiveInfo info;
			info.m_rewardID = atoi(*row++);
			info.m_date = atoi(*row++);
			info.m_phoneNum = *row++;

			if (info.m_rewardID <= 0 || info.m_date <= 0)
			{
				break;
			}

			user_reward.m_DrawInfo.push_back(info);

			row = mysql_fetch_row(res);
		}
		mysql_free_result(res);

		SendToClient(msg->m_sp, user_reward);
	}
}

void Work::HanderUserLogin(LMsgLMG2LdbUserLogin* msg)
{
	// 活动信息
	LMsgS2CActivityInfo active;

	// 填充转盘信息
	if (gActiveManager.CheckDrawActiveValid())
	{
		LMsgS2CActivityInfo::Info draw;
		draw.m_id = gActiveManager.GetDrawActiveId();
		char time[16];
		sprintf(time, "%d", gActiveManager.GetDrawBeginTime());
		draw.m_attri.push_back(LMsgS2CActivityInfo::Value());
		draw.m_attri.back().m_key = "BeginTime";
		draw.m_attri.back().m_value = time;
		sprintf(time, "%d", gActiveManager.GetDrawEndTime());
		draw.m_attri.push_back(LMsgS2CActivityInfo::Value());
		draw.m_attri.back().m_key = "EndTime";
		draw.m_attri.back().m_value = time;
		active.m_activities.push_back(draw);
		active.m_RestTimes = gActiveManager.getRestTimes(msg->m_userId);//获取玩家剩余抽奖次数
	}

	// 填充分享信息
	//if (gActiveManager.CheckShareActiveStart())  //分享活动开启
	//{
	//	LMsgS2CActivityInfo::Info share;
	//	share.m_id = gActiveManager.GetShareActiveId();
	//	share.m_info = gActiveManager.GetShareActiveSet();

	//	char name[64] = { 0 };
	//	sprintf(name, "ActivityShare_userid_%d", msg->m_userId);

	//	bool suc = false;
	//	int time = 0;
	//	bool ret = gActiveManager.m_RedisClient.readDataByString(std::string(name), time);
	//	if (ret)
	//	{
	//		if (time != 0)
	//		{
	//			LTime lastTime;
	//			int curDay = lastTime.GetDate();
	//			lastTime.SetSecs(time);
	//			int lastDay = lastTime.GetDate();
	//			if (curDay != lastDay)
	//				suc = true;
	//		}
	//		else
	//		{
	//			suc = true;
	//		}
	//	}

	//	LMsgS2CActivityInfo::ATTRI attri;
	//	attri.m_key = "canshare";
	//	attri.m_value = "0";
	//	if (suc)
	//	{
	//		attri.m_value = "1";
	//	}

	//	share.m_attri.push_back(attri);
	//	active.m_activities.push_back(share);
	//}

	// 填充红包活动信息
	if (gActiveManager.CheckRedPacketActiveValid())
	{
		LMsgS2CActivityInfo::Info redPackets;
		redPackets.m_id = gActiveManager.GetRedPacketsActiveId();
		char time[16];
		sprintf(time, "%d", gActiveManager.GetRedPacketsBeginTime());
		redPackets.m_attri.push_back(LMsgS2CActivityInfo::Value());
		redPackets.m_attri.back().m_key = "BeginTime";
		redPackets.m_attri.back().m_value = time;
		sprintf(time, "%d", gActiveManager.GetRedPacketsEndTime());
		redPackets.m_attri.push_back(LMsgS2CActivityInfo::Value());
		redPackets.m_attri.back().m_key = "EndTime";
		redPackets.m_attri.back().m_value = time;
		active.m_activities.push_back(redPackets);
	}

	if(gActiveManager.IsZhuanPanRedpacketOpen())
	{
		LMsgS2CActivityInfo::Info zhuanRedPackets;
		zhuanRedPackets.m_id = int(ACTIVITY_ZHUAN_PAN_REDPACKET);
		active.m_activities.push_back(zhuanRedPackets);
	}

	SendToUser(msg->m_sp,msg->m_userId, msg->m_strUUID,active);
}

void Work::HanderLogicSetActivity(LMsgCE2LSetActivity* msg)
{
	if (!msg)
	{
		return;
	}
	if (msg->m_activityId == ActivityID_Draw)
	{
		SetDrawActive(msg->m_strActivity, msg->m_activityId);
	}
	//else if (msg->m_activityId == ActivityID_Share)
	//{
	//	SetShareActive(msg->m_strActivity, msg->m_activityId);
	//}
	else if (msg->m_activityId == ActivityID_Invite)
	{
		SetRedPacketsActive(msg->m_strActivity, msg->m_activityId);
	}
}

void Work::SetDrawActive(const Lstring& strSet, int id)
{
	gActiveManager.SetDrawActiveConfig(strSet, id);
}

void Work::SendToUser(LSocketPtr LSp, const Lint& userid,const Lstring& uuid,LMsg& msg)
{
	if (LSp)
	{//发送到logicmanager
		LMsgLDB2LMGUserMsg send;
		send.m_userId = userid;
		send.m_dataBuff = msg.GetSendBuff();
		LSp->Send(send.GetSendBuff());
	}
}

void Work::HanderActivityRequestShareForDraw(LMsgLGM2LDBActivityRequestShareForDraw *msg)
{
	LMsgS2CActivityFinishShareForDraw share;
	share.m_errorCode = 0;
	//share.m_GiftCount = 0;

	char sharetime[128] = { 0 };
	char drawCount[128] = { 0 };
	char drawCountTime[128] = { 0 };
	char alreadyDrawedCount[128] = { 0 };
	char alreadyDrawedCountTime[128] = { 0 };


	sprintf(drawCountTime, "ActivityLastUpdateCanDrawCountTime_userid_%d", msg->m_userId);
	sprintf(sharetime, "ActivityShareForDrawTime_userid_%d", msg->m_userId);
	sprintf(drawCount, "ActivityCanDrawCount_userid_%d", msg->m_userId);
	sprintf(alreadyDrawedCount, "ActivityAlreadyDrawedCount_userid_%d", msg->m_userId);
	sprintf(alreadyDrawedCountTime, "ActivityLastUpdateAlreadyDrawedCountTime_userid_%d", msg->m_userId);


	if (gActiveManager.isRedisTimeNotToday(std::string(sharetime)))
	{
		gActiveManager.updateRedisKeyTime(std::string(sharetime));

		if (gActiveManager.isRedisTimeNotToday(std::string(drawCountTime)))
		{
			gActiveManager.updateRedisKeyTime(std::string(drawCountTime));

			bool ret = gActiveManager.redisWriteDataByString(std::string(drawCount), 1);
			if (ret)
			{
				gActiveManager.redisSetKeyTTLTime(std::string(drawCount), (24 * 60 * 60));
			}
			//	share.m_GiftCount = getRestTimes(msg->m_userId); //数量
		}
		else
		{
			gActiveManager.increaseRedisKeyValue(std::string(drawCount));
			//if (isRedisTimeNotToday(std::string(alreadyDrawedCountTime)))
			//{
			//	share.m_ErrorCode = 0;  //成功
			//	share.m_GiftCount = getRestTimes(msg->m_userId);  //数量
			//	Lint count = 0;
			//	bool res = m_RedisClient.readDataByString(std::string(drawCount),count);
			//	if (res)
			//	{
			//		share.m_GiftCount = count;
			//	}
			//}
			//else
			//{
			//	share.m_ErrorCode = 0;  //成功		
			//}
		}
	}

	share.m_GiftCount = gActiveManager.getRestTimes(msg->m_userId);

	SendToUser(msg->m_sp,msg->m_userId,  msg->m_strUUID, share);
}

void Work::HanderActivityRequestDrawOpen(LMsgLGM2LDBActivityRequestDrawOpen *msg)
{
	LMsgS2CActivityReplyDrawOpen active;
	// 读取中奖的玩家
	if (gActiveManager.CheckDrawActiveValid())
	{
		active.m_errorCode = 0;
		//active.m_SpendType = gActiveManager.GetDrawSpendType();
		//active.m_SpendCount = gActiveManager.GetDrawSpendCount();
		//isRedisTimeNotToday();
		std::list<std::string> winusers;
		gActiveManager.redisReadDataByList("ActivityDraw_WinUsers", 0, 100, winusers);

		for (auto it = winusers.begin(); it != winusers.end(); ++it)
		{
			active.m_winUsers.push_back(ActivityDrawWinUser());

			ActivityDrawWinUser& user = active.m_winUsers.back();
			size_t pos = it->find("|");
			if (pos != std::string::npos)
			{
				user.m_userId = it->substr(0, pos);
				user.m_giftName = it->substr(pos + 1);
			}
		}
		active.m_RestTimes = gActiveManager.getRestTimes(msg->m_userId);
	}
	else
	{
		active.m_errorCode = 1;
	}
	SendToUser(msg->m_sp,msg->m_userId, msg->m_strUUID, active);
}

void Work::HanderActivityRequestDraw(LMsgLGM2LDBActivityRequestDraw *msg)
{
	LMsgS2CActivityReplyDraw active;

	if (gActiveManager.CheckDrawActiveValid())
	{
		ActivityDrawGift fGift;
		ActivityDrawGift gift = gActiveManager.GetDrawGift(fGift);

		Lint spendtype = 0;
		Lint spendcount = 0;

		//if ( spendtype == 1 && spendcount > msg->m_cardNum )
		//{
		//	active.m_errorCode = 1;
		//}
		char canDrawCountTime[128] = { 0 };
		char alreadyDrawedCountTime[128] = { 0 };
		char canDrawCount[128] = { 0 };
		char alreadyDrawedCount[128] = { 0 };
		sprintf(canDrawCountTime, "ActivityLastUpdateCanDrawCountTime_userid_%d", msg->m_userId);
		sprintf(alreadyDrawedCountTime, "ActivityLastUpdateAlreadyDrawedCountTime_userid_%d", msg->m_userId);
		sprintf(canDrawCount, "ActivityCanDrawCount_userid_%d", msg->m_userId);
		sprintf(alreadyDrawedCount, "ActivityAlreadyDrawedCount_userid_%d", msg->m_userId);

		if (gActiveManager.isRedisTimeNotToday(std::string(canDrawCountTime))) // 今日还未更新过 可以抽奖的次数
		{
			active.m_RestTimes = 0;
			active.m_errorCode = 1;//抽奖次数不足
		}
		else
		{

			if (gActiveManager.isRedisTimeNotToday(std::string(alreadyDrawedCountTime)))//今天还没抽过
			{

				gActiveManager.updateRedisKeyTime(std::string(alreadyDrawedCountTime));
				bool ret = gActiveManager.redisWriteDataByString(std::string(alreadyDrawedCount), 0);
				if (ret)
				{
					gActiveManager.redisSetKeyTTLTime(std::string(alreadyDrawedCount), (24 * 60 * 60));
				}
				int count = 0;
				ret = gActiveManager.redisReadDataByString(std::string(canDrawCount), count);
				if (ret)
				{
					if (count > 0)
					{
						gActiveManager.redisWriteDataByString(std::string(alreadyDrawedCount), 1);

						if (gift.m_GiftIndex != -1)
						{
							active.m_RestTimes = count - 1;
							active.m_errorCode = 0;
							active.m_GiftIndex = gift.m_GiftIndex;
							active.m_GiftName = gift.m_GiftName;
							active.m_GiftType = gift.m_GiftType;
							active.m_GiftCount = gift.m_GiftCount;

							if (gift.m_GiftType == 1)
							{
								LMsgL2LMGModifyCard modify;
								modify.admin = "system";
								modify.cardType = CARD_TYPE_8;
								modify.cardNum = gift.m_GiftCount;
								modify.isAddCard = 1;
								modify.operType = CARDS_OPER_TYPE_LUCKY_DRAW;
								modify.m_userid = msg->m_userId;
								//modify.m_strUUID = msg->m_strUUID;
								gWork.SendToClient(msg->m_sp,modify);
							}

							// mysql记录中间信息
							MYSQL* m = m_dbsession->GetMysql();
							if (m)
							{
								LTime cur;
								std::stringstream ss;
								ss << "insert into activeGiftlog(ActiveId, UserId, SpendType, SpendCount, GiftName, GiftType, GiftCount, Date) values(";
								ss << gActiveManager.GetDrawActiveId() << ",";
								ss << msg->m_userId << ",";
								ss << spendtype << ",";
								ss << spendcount << ",";
								ss << "'" << gift.m_GiftName << "',";
								ss << gift.m_GiftType << ",";
								ss << gift.m_GiftCount << ",";
								ss << cur.Secs() << ")";

								if (mysql_real_query(m, ss.str().c_str(), ss.str().size()) == 0)
								{
									// 如果奖励的类型不是房卡 让玩家填写手机号
									if (gift.m_GiftType != 1)
									{
										active.m_NeedPhoneNum = 1;
										active.m_AutoId = (Lint)mysql_insert_id(m);
									}
								}
							}

							// redis记录中奖信息
							std::ostringstream str;
							str << "***";
							str << msg->m_userId % 1000;
							str << "|";
							str << gift.m_GiftName;
							gActiveManager.redisWriteDataByList("ActivityDraw_WinUsers", str.str(), 100);

							// 假数据
							if (fGift.m_GiftIndex != -1)
							{
								std::ostringstream str;
								str << "***";
								str << L_Rand(100, 999);
								str << "|";
								str << fGift.m_GiftName;
								gActiveManager.redisWriteDataByList("ActivityDraw_WinUsers", str.str(), 100);
							}
						}
						else
						{
							active.m_errorCode = -1;
						}

					}
				}
			}
			else
			{
				int count = 0;
				int alreadCount = 0;
				bool ret = gActiveManager.redisReadDataByString(std::string(canDrawCount), count);
				bool ret2 = gActiveManager.redisReadDataByString(std::string(alreadyDrawedCount), alreadCount);
				if (ret && ret2)
				{
					if (count - alreadCount > 0)
					{
						gActiveManager.redisWriteDataByString(std::string(alreadyDrawedCount), alreadCount + 1);

						if (gift.m_GiftIndex != -1)
						{
							active.m_RestTimes = count - alreadCount - 1;
							active.m_errorCode = 0;
							active.m_GiftIndex = gift.m_GiftIndex;
							active.m_GiftName = gift.m_GiftName;
							active.m_GiftType = gift.m_GiftType;
							active.m_GiftCount = gift.m_GiftCount;

							if (gift.m_GiftType == 1)
							{
								LMsgL2LMGModifyCard modify;
								modify.admin = "system";
								modify.cardType = CARD_TYPE_8;
								modify.cardNum = gift.m_GiftCount;
								modify.isAddCard = 1;
								modify.operType = CARDS_OPER_TYPE_LUCKY_DRAW;
								modify.m_userid = msg->m_userId;
								//modify.m_strUUID = msg->m_strUUID;
								gWork.SendToClient(msg->m_sp, modify);
							}

							// mysql记录中间信息
							MYSQL* m = m_dbsession->GetMysql();
							if (m)
							{
								LTime cur;
								std::stringstream ss;
								ss << "insert into activeGiftlog(ActiveId, UserId, SpendType, SpendCount, GiftName, GiftType, GiftCount, Date) values(";
								ss << gActiveManager.GetDrawActiveId() << ",";
								ss << msg->m_userId << ",";
								ss << spendtype << ",";
								ss << spendcount << ",";
								ss << "'" << gift.m_GiftName << "',";
								ss << gift.m_GiftType << ",";
								ss << gift.m_GiftCount << ",";
								ss << cur.Secs() << ")";

								if (mysql_real_query(m, ss.str().c_str(), ss.str().size()) == 0)
								{
									// 如果奖励的类型不是房卡 让玩家填写手机号
									if (gift.m_GiftType != 1)
									{
										active.m_NeedPhoneNum = 1;
										active.m_AutoId = (Lint)mysql_insert_id(m);
									}
								}
							}

							// redis记录中奖信息
							std::ostringstream str;
							str << "***";
							str << msg->m_userId % 1000;
							str << "|";
							str << gift.m_GiftName;
							gActiveManager.redisWriteDataByList("ActivityDraw_WinUsers", str.str(), 100);

							// 假数据
							if (fGift.m_GiftIndex != -1)
							{
								std::ostringstream str;
								str << "***";
								str << L_Rand(100, 999);
								str << "|";
								str << fGift.m_GiftName;
								gActiveManager.redisWriteDataByList("ActivityDraw_WinUsers", str.str(), 100);
							}
						}
						else
						{
							active.m_errorCode = -1;
						}
					}
					else
					{
						active.m_errorCode = 1;
					}
				}
			}
		}
	}
	else
	{
		active.m_errorCode = 2; //活动已关闭
	}

	SendToUser(msg->m_sp, msg->m_userId, msg->m_strUUID, active);
}

void Work::HanderActivityRequestLog(LMsgLGM2LDBActivityRequestLog *msg)
{
	LMsgS2CActivityReplyLog send;
	send.m_errorCode = -1;
	std::stringstream ss;
	ss << "SELECT Id, ActiveId, SpendType, SpendCount, GiftName, GiftType, GiftCount, Date, Phone FROM activeGiftlog WHERE";
	if (msg->m_activeId != 0)
	{
		ss << " ActiveId = " << msg->m_activeId << " AND";
	}
	ss << " UserId = " << msg->m_userId;
	ss<<" order by Date desc limit 10";

	MYSQL* m = m_dbsession->GetMysql();
	if (m)
	{
		LLOG_DEBUG("HanderActivityRequestLog sql:%s",ss.str().c_str());

		if (!mysql_real_query(m, ss.str().c_str(), ss.str().size()))
		{
			send.m_errorCode = 0;
			MYSQL_RES* res = mysql_store_result(m);
			if (res)
			{
				MYSQL_ROW row = mysql_fetch_row(res);
				while (row)
				{
					send.m_logs.push_back(ActivityLog());
					ActivityLog& log = send.m_logs.back();

					log.AutoId = atoi(*row++);
					log.Id = atoi(*row++);
					log.SpendType = atoi(*row++);
					log.SpendCount = atoi(*row++);
					log.GiftName = *row++;
					log.GiftType = atoi(*row++);
					log.GiftCount = atoi(*row++);
					log.Date = atoi(*row++);
					log.Phone = *row++;

					row = mysql_fetch_row(res);
				}
				mysql_free_result(res);
			}
		}
	}
	SendToUser(msg->m_sp, msg->m_userId, msg->m_strUUID, send);
}

void Work::HanderActivityPhone(LMsgLGM2LDBActivityWritePhone *msg)
{
	// 保存手机号
	std::stringstream ss;
	ss << "UPDATE activeGiftlog SET Phone = '" << msg->m_PhoneNum << "' WHERE";
	ss << " Id = " << msg->m_AutoId;

	LMsgS2CActivityPhone send;
	if (m_dbsession->Excute(ss.str()))
	{
		send.m_errorCode = 0;
	}
	else
	{
		send.m_errorCode = 1;
	}
	SendToUser(msg->m_sp, msg->m_userId, msg->m_strUUID, send);
}

void Work::HanderAddUserPlayCount(LMsgL2LMGModifyPlayCount* msg)
{
	char lastPlayTime[128] = { 0 };
	char canDrawCountTime[128] = { 0 };
	char canDrawCount[128] = { 0 };
	char todayTotalPlayCount[128] = { 0 };
	bool isAddedDrawCount = false;
	sprintf(lastPlayTime, "ActivityLastPlayTime_userid_%d", msg->m_userid);
	sprintf(canDrawCountTime, "ActivityLastUpdateCanDrawCountTime_userid_%d", msg->m_userid);
	sprintf(canDrawCount, "ActivityCanDrawCount_userid_%d", msg->m_userid);
	sprintf(todayTotalPlayCount, "ActivityTodayTotalPlayCount_userid_%d", msg->m_userid);

	if (gActiveManager.isRedisTimeNotToday(std::string(lastPlayTime))) //今天第一局
	{

		gActiveManager.updateRedisKeyTime(std::string(lastPlayTime));
		bool ret = gActiveManager.redisWriteDataByString(std::string(todayTotalPlayCount), 1);
		if (ret)
		{
			gActiveManager.redisSetKeyTTLTime(std::string(todayTotalPlayCount), (24 * 60 * 60));
		}


		if (gActiveManager.isRedisTimeNotToday(std::string(canDrawCountTime)))//今日还未更新过canDrawCountTime
		{
			gActiveManager.updateRedisKeyTime(std::string(canDrawCountTime));
			ret = gActiveManager.redisWriteDataByString(std::string(canDrawCount), 1);
			if (ret)
			{
				gActiveManager.redisSetKeyTTLTime(std::string(canDrawCount), (24 * 60 * 60));
			}
		}
		else
		{
			gActiveManager.increaseRedisKeyValue(std::string(canDrawCount));
		}
		isAddedDrawCount = true;

	}
	else//不是第一局  肯定更新过canDrawCountTime了
	{
		Lint count = 0;
		bool ret = gActiveManager.redisReadDataByString(std::string(todayTotalPlayCount), count);
		if (ret)
		{
			if (count == 2)//玩够3局再增一次抽奖机会
			{
				gActiveManager.increaseRedisKeyValue((std::string(canDrawCount)));
				isAddedDrawCount = true;
			}
			count += 1;
			gActiveManager.redisWriteDataByString(std::string(todayTotalPlayCount), count);
		}
	}
	if (isAddedDrawCount && gActiveManager.CheckDrawActiveValid())
	{
		LMsgS2CActivityFinishShareForDraw share;
		//char sharetime[128] = { 0 };
		//char drawCount[128] = { 0 };
		//char drawCountTime[128] = { 0 };
		//char alreadyDrawedCount[128] = { 0 };
		//char alreadyDrawedCountTime[128] = { 0 };

		//sprintf(drawCountTime, "ActivityLastUpdateCanDrawCountTime_userid_%d", msg->m_userid);
		//sprintf(sharetime, "ActivityShareForDrawTime_userid_%d", msg->m_userid);
		//sprintf(drawCount, "ActivityCanDrawCount_userid_%d", msg->m_userid);
		//sprintf(alreadyDrawedCount, "ActivityAlreadyDrawedCount_userid_%d", msg->m_userid);
		//sprintf(alreadyDrawedCountTime, "ActivityLastUpdateAlreadyDrawedCountTime_userid_%d", msg->m_userid);

		//if (isRedisTimeNotToday(std::string(alreadyDrawedCountTime)))
		//{
		//	share.m_ErrorCode = 0;  //成功
		//	share.m_GiftCount = 0;  //数量
		//	Lint count = 0;
		//	bool res = m_RedisClient.readDataByString(std::string(drawCount), count);
		//	if (res)
		//	{
		//		share.m_GiftCount = count;
		//	}
		//}
		//else
		//{
		//	share.m_ErrorCode = 0;  //成功
		//	share.m_GiftCount = getRestTimes(msg->m_userid);
		//}
		share.m_errorCode = 0;
		share.m_GiftCount = gActiveManager.getRestTimes(msg->m_userid);

		SendToUser(msg->m_sp, msg->m_userid, msg->m_user_unioid, share);
	}
}

void Work::HanderUserCreateMatch(LMsgS2SCreateMatch* msg)
{
	if (!msg) return;
	MYSQL* m = m_dbsession->GetMysql();
	if (m == NULL)
	{
		LLOG_ERROR("Work::HanderLogicReqVipLog mysql null");
		return;
	}
	std::stringstream ss;
	ss << "SELECT count(*)  from matchmanagers WHERE userid =";
	ss << msg->m_userId << " and status = 0";

	if (mysql_real_query(m, ss.str().c_str(), ss.str().size()))
	{
		LLOG_ERROR("UserManager::HanderLogicGetShareVideoId _ 1 _ sql error %s", mysql_error(m));
		return;
	}
	Lint count = 0;
	MYSQL_RES* res = mysql_store_result(m);
	MYSQL_ROW row = mysql_fetch_row(res);
	if (row)
	{
		count = atoi(*row++);
	}
	mysql_free_result(res);

	if (count <= 0)
	{
		return;
	}

	LMsgS2SCreateMatch send;
	send.m_matchName = msg->m_matchName;
	send.m_userId = msg->m_userId;
	send.m_AthletesNum = msg->m_AthletesNum;
	send.m_PlayState = msg->m_PlayState;
	send.m_playtype = msg->m_playtype;
	send.m_playtypeCount = msg->m_playtype.size();
	send.m_RegistrationNum = msg->m_RegistrationNum;
	send.m_RegistrationRules = msg->m_RegistrationRules;
	send.m_RotateCount = msg->m_RotateCount;
	send.m_Score = msg->m_Score;
	send.m_StartTime = msg->m_StartTime;
	send.m_MatchType = msg->m_MatchType;
	send.m_nMaxFanshu = msg->m_nMaxFanshu;
	send.m_CirclePerRound = msg->m_CirclePerRound;
	send.m_appId = msg->m_appId;
	msg->m_sp->Send(send.GetSendBuff());
}

void Work::HanderUserDissolveMatch(LMsgS2SDissolveMatch* msg)
{
	if (!msg) return;
	MYSQL* m = m_dbsession->GetMysql();
	if (m == NULL)
	{
		LLOG_ERROR("Work::HanderLogicReqVipLog mysql null");
		return;
	}
	std::stringstream ss;
	ss << "SELECT count(*)  from matchmanagers WHERE userid =";
	ss << msg->m_userId << " and status = 0";

	if (mysql_real_query(m, ss.str().c_str(), ss.str().size()))
	{
		LLOG_ERROR("UserManager::HanderLogicGetShareVideoId _ 1 _ sql error %s", mysql_error(m));
		return;
	}
	Lint count = 0;
	MYSQL_RES* res = mysql_store_result(m);
	MYSQL_ROW row = mysql_fetch_row(res);
	if (row)
	{
		count = atoi(*row++);
	}
	mysql_free_result(res);

	if (count <= 0)
	{
		return;
	}

	LMsgS2SDissolveMatch send;
	send.m_userId = msg->m_userId;
	send.m_matchId = msg->m_matchId;
	msg->m_sp->Send(send.GetSendBuff());
}

void Work::HanderUserCheckCanCreateMatch(LMsgS2SCheckCanCreateMatch* msg)
{
	if (!msg) return;
	MYSQL* m = m_dbsession->GetMysql();
	std::stringstream ss;
	ss << "SELECT count(*)  from matchmanagers WHERE userid =";
	ss << msg->m_userId << " and status = 0";

	if (mysql_real_query(m, ss.str().c_str(), ss.str().size()))
	{
		LLOG_ERROR("UserManager::HanderLogicGetShareVideoId _ 1 _ sql error %s", mysql_error(m));
		return;
	}
	Lint count = 0;
	MYSQL_RES* res = mysql_store_result(m);
	MYSQL_ROW row = mysql_fetch_row(res);
	if (row)
	{
		count = atoi(*row++);
	}
	mysql_free_result(res);

	LMsgS2CUserCheckCanCreateMatchRet send;

	if (count <= 0)
	{
		send.m_error = 1;
	}
	LLOG_DEBUG("Work::HanderUserCheckCanCreateMatch ErrorCode: %d",send.m_error);
	gWork.SendToUser(msg->m_sp,msg->m_userId,msg->m_uuid, send);
}

void Work::HanderActivityRedPacketBind(LMsgCe2LDBBindingRelation* msg)
{
	boost::mutex::scoped_lock l(m_mutexUser2recommIdMap);
	if (msg == NULL)
	{
		return;
	}
	Lint inviteeId = msg->m_invitee;
	Lint inviterId = msg->m_inviter;
	if (gActiveManager.CheckRedPacketActiveValid())
	{
		auto it = m_user2recommIdMap.find(inviteeId);
		if (it != m_user2recommIdMap.end())
		{
			return; //已被绑定过
		}

		MYSQL* m = m_dbsession->GetMysql();
		if (m == NULL)
		{
			LLOG_ERROR("Work::HanderActivityRedPacketBind mysql null");
			return;
		}

		std::stringstream ss1;
		ss1 << "select count(*) from activitybindingrelation where inviter = ";
		ss1 << msg->m_inviter;
		Lint count = 0;
		if (mysql_real_query(m, ss1.str().c_str(), ss1.str().size()))
		{
			LLOG_ERROR("UserManager::HanderActivityRedPacketBind _ 1 _ sql error %s", mysql_error(m));
			return;
		}

		MYSQL_RES* res = mysql_store_result(m);
		MYSQL_ROW row = mysql_fetch_row(res);
		if (row)
		{
			count = atoi(*row++);
		}
		mysql_free_result(res);


		if (count >= 20)
		{
			return;
		}

		ShipInfo info(inviterId, 0, 0);
		m_user2recommIdMap.insert(std::make_pair(inviteeId, info));

		std::stringstream ss;
		ss << "INSERT INTO activitybindingrelation (Invitee, Inviter, bindTime, firstTaskFinish, secondTaskFinish) VALUES (";
		ss <<inviteeId << ",";
		ss <<inviterId << ",";
		ss << gWork.GetCurTime().Secs() << ",";
		ss << 0 << ",";
		ss << 0 << ")";

		//Lstring* sql = new Lstring(ss.str());
		m_dbsession->Excute(ss.str());

	}
}

void Work::HanderDeskUser(LMsgL2LDBDeskUser* msg)
{
	if (msg)
	{
		for (Lint i = 0; i < DESK_USER_COUNT; ++i)
		{
			_inviteProcess(msg->m_userID[i], msg->m_regTime[i]);
		}
	}
}

void Work::_inviteProcess(Lint invitee, Lint regtime)
{
	auto it = m_user2recommIdMap.find(invitee);
	if (it != m_user2recommIdMap.end())
	{
		LTime cur, regTime;
		Lint i = cur.Secs();
		regTime.SetSecs(regtime);

		it->second.m_count += 1;
		std::stringstream ss;
		ss << "UPDATE activitybindingrelation  SET firstTaskFinish = ";
		ss << it->second.m_count;
		ss << " WHERE Invitee = '";
		ss << invitee << "'";
		//Lstring* sql = new Lstring(ss.str());
		m_dbsession->Excute(ss.str());

		if (cur.GetYday()==regTime.GetYday()+1&& cur.GetYear()==regTime.GetYear()) //判断是否是注册完第二天打的
		{
			it->second.m_secondCount += 1;
			std::stringstream ss;
			ss << "UPDATE activitybindingrelation  SET secondTaskFinish = ";
			ss << it->second.m_secondCount;
			ss << " WHERE Invitee = ";
			ss << invitee;
			//Lstring* sql = new Lstring(ss.str());
			m_dbsession->Excute(ss.str());
		}

	}
}

void Work::HanderRequestRedPacket(LMsgL2DBRequestRedpacket* msg)
{
	if (msg == NULL )
	{
		return;
	}
	LMsgS2CRequestRedPacket ret;
	if (!gActiveManager.CheckRedPacketActiveValid())
	{
		ret.m_error = Expired;
		SendToLMUser(msg->m_userid, ret);
		return;
	}
	auto it = m_user2recommIdMap.find(msg->m_Invitee);
	if (it != m_user2recommIdMap.end() && it->second.m_inviter == msg->m_Inviter)
	{
		if (msg->m_type == inviteeFirst)
		{
			if (it->second.m_count <= 0 || it->second.m_inviteeFirst == true)
				return;
		}
		else if (msg->m_type == inviterFirst)
		{
			if (it->second.m_count <= 0 || it->second.m_inviterFirst == true)
				return;
		}
		else if (msg->m_type == inviteeSecond)
		{
			if (it->second.m_secondCount <= 0 || it->second.m_inviteeSecond == true)
				return;
		}
		else if (msg->m_type == inviterSecond)
		{
			if (it->second.m_secondCount <= 0 || it->second.m_inviterSecond == true)
				return;
		}

		Money money = gActiveManager.GetRedPacketGift();
		if (money.m_money > 0)
		{

			gWork.SetRedActivityTrue(msg->m_Invitee,msg->m_type);

			RedPacketRequest request;
			request.m_type = msg->m_type;
			request.m_money = money.m_money;
			//request.m_type = msg->m_type;
			std::stringstream forSign;
			forSign << "we34RtF" << msg->m_userid << msg->m_Invitee << "_" << msg->m_type;
			forSign << "shandong_db33001" << money.m_money;

			Lstring sign = HttpRequest::Instance().Md5Hex(forSign.str().c_str(), forSign.str().length());

			std::stringstream ss;
			ss << gConfig.GetRedPacketUrl() << "red/submit?userId="<<msg->m_userid<<"&redpId="<< msg->m_Invitee<<"_"<< msg->m_type<<"&&serverCode=shandong_db&gameId=33001&activityName=shandongInvite";
			ss << "&awardAmount=" << money.m_money << "&sign="<< sign;

			request.m_url = ss.str();
			request.m_userId = msg->m_userid;
			request.m_index = money.m_index;
			request.m_invitee = msg->m_Invitee;
			gHRP.push(request);
		}
		else
		{
			LMsgS2CRequestRedPacket send;
			send.m_inviteeId = msg->m_Invitee;
			send.m_type = msg->m_type;
			send.m_error = OutOfMoney;
			
			SendToLMUser(msg->m_userid, send);
		}
	}
}

void Work::SetRedActivityFalse(Lint invitee, Lint type)
{
	boost::mutex::scoped_lock l(m_mutexUser2recommIdMap);
	auto it = m_user2recommIdMap.find(invitee);
	if (it != m_user2recommIdMap.end())
	{
		if (type == inviteeFirst)
		{
			it->second.m_inviteeFirst = false;
		}
		else if (type == inviterFirst)
		{
			it->second.m_inviterFirst = false;
		}
		else if (type == inviteeSecond)
		{
			it->second.m_inviteeSecond = false;
		}
		else if (type == inviterSecond)
		{
			it->second.m_inviterSecond = false;
		}
	}
}

void Work::SetRedActivityTrue(Lint invitee, Lint type)
{
	boost::mutex::scoped_lock l(m_mutexUser2recommIdMap);
	auto it = m_user2recommIdMap.find(invitee);
	if (it != m_user2recommIdMap.end())
	{
		if (type == inviteeFirst)
		{
			it->second.m_inviteeFirst = true;
		}
		else if (type == inviterFirst)
		{
			it->second.m_inviterFirst = true;
		}
		else if (type == inviteeSecond)
		{
			it->second.m_inviteeSecond = true;
		}
		else if (type == inviterSecond)
		{
			it->second.m_inviterSecond = true;
		}
	}
}

void Work::SendToLMUser( const Lint& userid, LMsg& msg )
{
	LLOG_DEBUG("Work::SendToUser enter");
	if(m_logicManager)
	{
		LLOG_DEBUG("Work::SendToUser m_logicManager not null");
		LMsgLDB2LMGUserMsg send;
		send.m_userId = userid;
		send.m_dataBuff = msg.GetSendBuff();
		m_logicManager->Send(send.GetSendBuff());
	}
}

void Work::SaveMysql(Lstring sql)
{
	m_Maindbsession.Excute(sql.c_str());
}

void Work::SetRedPacketsActive(const Lstring& strSet, int id)
{
	gActiveManager.SetRedPacketsActiveConfig(strSet, id);
}

//获取邀请信息
void Work::HanderGetInvitingInfo( LMsgL2DBGetInvitingInfo* msg )
{
	LLOG_DEBUG("Work::HandleReqBindingRelationships");

	LMsgLDB2LMGBindingRelationships ret;
	ret.m_strUUID= msg->m_strUUID;
	ret.m_userId = msg->m_userId;
	ret.m_inviterId = -1;
	//ret.m_inviteeCount = 0;
	ret.m_remainReward = gActiveManager.GetRaminGiftCount() > 0 ? 1 : 0;

	MYSQL* m = m_dbsession->GetMysql();
	if (m != NULL)
	{
		// read inviter
		std::stringstream ss;
		ss << "select  t1.inviter,t1.firstTaskFinish,t1.secondTaskFinish,t2.GiftCount,t2.Date,t2.GiftType from activitybindingrelation t1 LEFT JOIN redpacketactivitylog t2 ON t1.invitee = t2.UserId and (t2.GiftType = 1 or t2.GiftType = 3) and t2.SpecialId = 1004 where t1.invitee =" << msg->m_userId;
		if (!mysql_real_query(m, ss.str().c_str(), ss.str().size()))
		{
			LLOG_DEBUG(ss.str().c_str());
			MYSQL_RES* res = mysql_store_result(m);
			if (res)
			{
				MYSQL_ROW row = mysql_fetch_row(res);
				while (row)
				{
					ret.m_inviterId = atoi(*row++);
					Lint firstTask = atoi(*row++);
					Lint secondTask = atoi(*row++);
					Lint count = 0, date = 0, type = 0;
					if (*row)
					{
						count = atoi(*row++);
						date = atoi(*row++);
						type = atoi(*row++);
					}
					ret.m_firstTaskFinishedAsInvitee = firstTask;
					ret.m_secondTaskFinishedAsInvitee = secondTask;
					switch (type)
					{
					case inviteeFirst:
						ret.m_firstMoneyAsInvitee = count;
						ret.m_firstMoneyAsInviteeDate = date;		
						break;
					case inviteeSecond:
						ret.m_secondMoneyAsInvitee = count;
						ret.m_secondMoneyAsInviteeDate = date;
						break;
					}
					row = mysql_fetch_row(res);
				}
			}
			mysql_free_result(res);		
		}
		// read invitee
		ss.str("");
		ss << "SELECT  t.Invitee,t.firstTaskFinish,t.secondTaskFinish, t2.GiftCount,t2.GiftType,t2.Date FROM activitybindingrelation t LEFT JOIN redpacketactivitylog t2 ON  t.invitee = t2.UserId and (t2.GiftType = 2 or t2.GiftType = 4) and  t2.SpecialId = 1004 where t.inviter = ";
		ss << msg->m_userId;
		LLOG_DEBUG(ss.str().c_str());
		if (!mysql_real_query(m, ss.str().c_str(), ss.str().size()))
		{
			MYSQL_RES* res = mysql_store_result(m);
			if (res)
			{
				MYSQL_ROW row = mysql_fetch_row(res);

				while (row)
				{
					ret.m_invitees.push_back(atoi(*row++));
					ret.m_firstTaskFinished.push_back(atoi(*row++));
					ret.m_secondTaskFinished.push_back(atoi(*row++));
					ret.m_money.push_back(0);
					ret.m_type.push_back(0);
					ret.m_date.push_back(0);
					if (*row)
					{
						ret.m_money.back() =atoi(*row++);
						ret.m_type.back() = atoi(*row++);
						ret.m_date.back() = atoi(*row++);
					}
					ret.m_count++;
					row = mysql_fetch_row(res);
				}
				mysql_free_result(res);
			}
		}
	}
	else
		LLOG_ERROR("Work::HandleReqTaskProgress mysql null");

	msg->m_sp->Send(ret.GetSendBuff());
}

void Work::_reloadActivity()
{
	LLOG_ERROR("Work::_reloadActivity");
}

void Work::_loadFriendship()
{
	MYSQL* m = m_Maindbsession.GetMysql();
	if (m != NULL)
	{
		// read inviter
		std::stringstream ss;
		ss << "SELECT  t.Invitee, t.Inviter,t.firstTaskFinish,t.secondTaskFinish, t2.GiftCount,t2.GiftType FROM activitybindingrelation t LEFT JOIN redpacketactivitylog t2 ON  t.invitee = t2.UserId and  t2.SpecialId = 1004";    //全加载
		if (!mysql_real_query(m, ss.str().c_str(), ss.str().size()))
		{
			MYSQL_RES* res = mysql_store_result(m);
			if (res)
			{
				MYSQL_ROW row = mysql_fetch_row(res);
				while (row)
				{
					Lint inviteeId = atoi(*row++);
					Lint inviterId = atoi(*row++);
					Lint fcount = atoi(*row++);
					Lint scount = atoi(*row++);
					if (!*row)
					{
						ShipInfo info(inviterId, fcount, scount);
						m_user2recommIdMap.insert(std::make_pair(inviteeId, info));
					}
					else
					{
						Lint reward = atoi(*row++);
						Lint type = atoi(*row++);
						auto it = m_user2recommIdMap.find(inviteeId);
						if (it != m_user2recommIdMap.end())
						{
							if (inviteeFirst == type)
							{
								it->second.m_inviteeFirst = true;
							}
							else if (inviteeSecond == type)
							{
								it->second.m_inviteeSecond = true;
							}
							else if (inviterFirst == type)
							{
								it->second.m_inviterFirst = true;
							}
							else if (inviterSecond == type)
							{
								it->second.m_inviterSecond = true;
							}
						}
						else
						{
							ShipInfo info(inviterId, fcount, scount);
							if (inviteeFirst == type)
							{
								info.m_inviteeFirst = true;
							}
							else if (inviteeSecond == type)
							{
								info.m_inviteeSecond = true;
							}
							else if (inviterFirst == type)
							{
								info.m_inviterFirst = true;
							}
							else if (inviterSecond == type)
							{
								info.m_inviterSecond = true;
							}
							m_user2recommIdMap.insert(std::make_pair(inviteeId, info));
						}
					}
					row = mysql_fetch_row(res);
				}
				mysql_free_result(res);
			}
		}
	}
	else
	{
		LLOG_ERROR("Work::LoadFriendship mysql null");
	}
}

ActivityZhuanPanRedpacket*  Work::LoadZhuanPanRedpacketData(Lint userId)
{
	ActivityZhuanPanRedpacket* pData = gActiveManager.GetZhuanPanRedpacketData(userId);
	if(NULL == pData)
	{
		pData = new ActivityZhuanPanRedpacket();

		MYSQL* m = m_dbsession->GetMysql();
		std::stringstream ss;	
		ss << "select drawTime,drawCount FROM zhuanpanredpacket where userId= ";
		ss<<userId;

		LLOG_DEBUG("LoadZhuanPanRedpacketData sql,%s",ss.str().c_str());
		if (mysql_real_query(m, ss.str().c_str(), ss.str().size()))
		{
			LLOG_ERROR("UserManager::LoadZhuanPanRedpacketData _ 1 _ sql error %s", mysql_error(m));
			delete pData;
			pData = NULL;
			return NULL;
		}

		MYSQL_RES* res = mysql_store_result(m);
		MYSQL_ROW row = mysql_fetch_row(res);
		if (row)
		{
			pData->m_drawTime = atoi(*row++);
			pData->m_drawCnt = atoi(*row++);
		}

		mysql_free_result(res);

		gActiveManager.UpdateZhuanPanRedpacketMap(userId,pData);
	}

	return pData;
}

void Work::HanderZhuanPanRedpacketOpt(LMsgL2DBZhuanpanRedpacket* msg)
{
	if(msg == NULL)
		return ;

	LMsgS2CZhuanPanRedpacket send;
	send.m_type = msg->m_type;
	if(!gActiveManager.IsZhuanPanRedpacketOpen())
	{
		send.m_errorCode = LMsgS2CZhuanPanRedpacket::Error_Time;

		SendToLMUser(msg->m_userId,send);
		return ;
	}

	ActivityZhuanPanRedpacket* pData = LoadZhuanPanRedpacketData(msg->m_userId);
	if(NULL == pData)
	{
		send.m_errorCode = LMsgS2CZhuanPanRedpacket::Error_UnKown;
		SendToLMUser(msg->m_userId,send);
		return ;
	}

	switch(msg->m_type)
	{
	case 0:
		{
			LTime cur;
			send.m_lastDrawTime = pData->m_drawTime;
			send.m_drawCnt = pData->m_drawCnt;
			send.m_curTime = cur.Secs();
			SendToLMUser(msg->m_userId,send);
			return ;
		}
		break;
	case 1:
		{
			LTime cur,last;
			last.SetSecs(pData->m_drawTime);

			if(LTimeTool::IsSameDay(cur,last))
			{
				send.m_errorCode = LMsgS2CZhuanPanRedpacket::Error_Hav_Fetch;
				send.m_lastDrawTime = pData->m_drawTime;
				SendToLMUser(msg->m_userId,send);

				return ;
			}

			int nLastTime = pData->m_drawTime;
			int nLastCnt = pData->m_drawCnt;

			pData->m_drawTime = cur.Secs();
			pData->m_drawCnt++;

			send.m_lastDrawTime = pData->m_drawTime;
			send.m_drawCnt = pData->m_drawCnt;

			//只要不是第二次就送房卡
			if(pData->m_drawCnt !=2)
			{
				LMsgL2LMGModifyCard modify;
				modify.admin = "system";
				modify.cardType = CARD_TYPE_8;
				modify.cardNum = 1;
				modify.isAddCard = 1;
				modify.operType = CARDS_OPER_TYPE_LUCKY_DRAW;
				modify.m_userid = msg->m_userId;
				//modify.m_strUUID = msg->m_strUUID;
				gWork.SendToClient(msg->m_sp, modify);
			}

			SendToLMUser(msg->m_userId,send);

			MYSQL* m = m_dbsession->GetMysql();
			std::stringstream ss;	
			ss << "INSERT INTO zhuanpanredpacket(userId,drawTime,drawCount)VALUES(";
			ss<<msg->m_userId;
			ss<<",";
			ss<<pData->m_drawTime;
			ss<<",";
			ss<<pData->m_drawCnt;
			ss<<")";
			ss<<"ON DUPLICATE KEY UPDATE ";
			ss<<"drawTime=";
			ss<<pData->m_drawTime;
			ss<<",";
			ss<<"drawCount=";
			ss<<pData->m_drawCnt;
			ss<<";";

			LLOG_DEBUG("HanderZhuanPanRedpacketOpt sql:%s",ss.str().c_str());
			
			if (!m_dbsession->Excute(ss.str().c_str()))
			{
				LLOG_ERROR("Work::HanderZhuanPanRedpacketOpt Insert Error");
			}
			
		}
		break;
	}

	return ;
}

void Work::HanderZhuanPanNotice(LMsgS2LDZhuanPanNotice* msg)
{
	if(NULL == msg)
		return ;

	boost::recursive_mutex::scoped_lock l(m_Mutex);

	if(!NewZhuanPanActivityTbl::Instance().IsOpen())
		return ;

	bool bMarkDirty = false;
	NewZhuanPanActivity* pActivity = LoadNewZhuanPanActivityData(msg->m_userid);
	if(pActivity)
	{
		bMarkDirty = ResetNewZhuanPanActivityData(pActivity);
		switch(msg->m_opt)
		{
		case LMsgS2LDZhuanPanNotice::OPT_FENX_XIANG:
			{
				LTime cur;
				if(pActivity->m_LastFenXiangTime == 0)
				{
					pActivity->m_ContinueFenXiangDays++;
					pActivity->m_LastFenXiangTime = cur.Secs();

					bMarkDirty = true;
				}
				else
				{
					LTime _time;
					_time.SetSecs(pActivity->m_LastFenXiangTime);
					if(LTimeTool::IntervalDay(_time,cur)==1)
					{
						pActivity->m_ContinueFenXiangDays++;
						pActivity->m_LastFenXiangTime = cur.Secs();

						bMarkDirty = true;
					}
				}
			}
			break;
		case LMsgS2LDZhuanPanNotice::OPT_MAX_WINNER:
			{
				if(pActivity->m_DailyMaxWinnerDarwCnt<NewZhuanPanActivityTbl::Instance().GetMaxWinnerDailyDrawMaxCnt())
				{
					pActivity->m_DailyMaxWinnerCnt++;

					bMarkDirty = true;
				}
			}
			break;
		case LMsgS2LDZhuanPanNotice::OPT_CREATE_ROOM:
			{
				if(pActivity->m_DailyCreateRoomDarwCnt<NewZhuanPanActivityTbl::Instance().GetCreateRoomDailyDrawMaxCnt())
				{
					pActivity->m_DailyCreateRoomCnt++;

					bMarkDirty = true;
				}
			}
			break;
		}
	}

	if(bMarkDirty)
	{
		SaveNewZhuanPanActivityToSql(pActivity);
	}
}

NewZhuanPanActivity* Work::LoadNewZhuanPanActivityData(Lint userId)
{
	NewZhuanPanActivity* pActivity = gActiveManager.GetNewZhuanPanActivityData(userId);
	if(NULL == pActivity)
	{
		MYSQL* m = m_dbsession->GetMysql();
		std::stringstream ss;	
		ss << "select UserId,DailyMaxWinnerCnt,DailyMaxWinnerDarwCnt,DailyCreateRoomCnt,DailyCreateRoomDarwCnt,LastFenXiangTime,ContinueFenXiangDays,ResetTime FROM NewZhuanPanActivityInfo where userId= ";
		ss<<userId;

		LLOG_DEBUG("LoadNewZhuanPanActivityData sql,%s",ss.str().c_str());
		if (mysql_real_query(m, ss.str().c_str(), ss.str().size()))
		{
			LLOG_ERROR("UserManager::LoadNewZhuanPanActivityData _ 1 _ sql error %s", mysql_error(m));
			return NULL;
		}

		pActivity = new NewZhuanPanActivity();
		pActivity->m_userId = userId;
		
		MYSQL_RES* res = mysql_store_result(m);
		MYSQL_ROW row = mysql_fetch_row(res);
		if (row)
		{
			pActivity->m_userId = atoi(*row++);
			pActivity->m_DailyMaxWinnerCnt = atoi(*row++);
			pActivity->m_DailyMaxWinnerDarwCnt = atoi(*row++);
			pActivity->m_DailyCreateRoomCnt = atoi(*row++);
			pActivity->m_DailyCreateRoomDarwCnt = atoi(*row++);
			pActivity->m_LastFenXiangTime = atoi(*row++);
			pActivity->m_ContinueFenXiangDays = atoi(*row++);
			pActivity->m_ResetTime = atoi(*row++);
		}

		mysql_free_result(res);

		gActiveManager.InsertNewZhuanPanActivityMap(userId,pActivity);
	}
	
	return pActivity;
}

bool Work::ResetNewZhuanPanActivityData(NewZhuanPanActivity* pActivity)
{
	if(NULL == pActivity)
		return false;

	bool bReset = false;
	LTime cur,last;
	last.SetSecs(pActivity->m_ResetTime);

	if(!LTimeTool::IsSameDay(cur,last))
	{
		pActivity->m_ResetTime = cur.Secs();
		pActivity->DailyClear();

		if(pActivity->m_ContinueFenXiangDays<NewZhuanPanActivityTbl::Instance().GetContinueLoginNeedCnt())
		{
			last.SetSecs(pActivity->m_LastFenXiangTime);
			//分享不连续了
			if(LTimeTool::IntervalDay(last,cur)>=2)
			{
				pActivity->m_LastFenXiangTime = 0;
				pActivity->m_ContinueFenXiangDays = 0;
			}
		}
		else   //达到分享要求后第二天数据自动清除
		{
			pActivity->m_LastFenXiangTime = 0;
			pActivity->m_ContinueFenXiangDays = 0;
		}
		
		bReset = true;
	}

	return bReset;
}

int Work::GetDrawTimes(NewZhuanPanActivity* pActivity)
{
	if(NULL == pActivity)
		return 0;

	int nTotalTimes = 0;
	//计算最大赢家拥有的次数
	int nTimes = pActivity->m_DailyMaxWinnerCnt/gNewZhuanPanTbl.GetMaxWinnerNeedCnt();
	if(nTimes>gNewZhuanPanTbl.GetMaxWinnerDailyDrawMaxCnt())
	{
		nTimes = gNewZhuanPanTbl.GetMaxWinnerDailyDrawMaxCnt();
	}
	nTotalTimes += (nTimes - pActivity->m_DailyMaxWinnerDarwCnt);

	//计算创建房间的抽奖次数
	nTimes = pActivity->m_DailyCreateRoomCnt/gNewZhuanPanTbl.GetCreateRoomNeedCnt();
	if(nTimes>gNewZhuanPanTbl.GetCreateRoomDailyDrawMaxCnt())
	{
		nTimes = gNewZhuanPanTbl.GetCreateRoomDailyDrawMaxCnt();
	}
	nTotalTimes += (nTimes - pActivity->m_DailyCreateRoomDarwCnt);

	//计算连续登陆的抽奖次数
	if(pActivity->m_ContinueFenXiangDays>=gNewZhuanPanTbl.GetContinueLoginNeedCnt())
	{
		nTotalTimes++;
	}

	return nTotalTimes;
}

TYPE_DRAW Work::GetDrawType(NewZhuanPanActivity* pActivity)
{
	if (NULL == pActivity)
		return DRAW_NULL;

	int nTimes = pActivity->m_DailyMaxWinnerCnt / gNewZhuanPanTbl.GetMaxWinnerNeedCnt();
	if (nTimes > gNewZhuanPanTbl.GetMaxWinnerDailyDrawMaxCnt())
	{
		nTimes = gNewZhuanPanTbl.GetMaxWinnerDailyDrawMaxCnt();
	}
	if (nTimes > pActivity->m_DailyMaxWinnerDarwCnt)
	{
		return DRAW_BIG_WINNER;
	}

	//计算创建房间的抽奖次数
	nTimes = pActivity->m_DailyCreateRoomCnt / gNewZhuanPanTbl.GetCreateRoomNeedCnt();
	if (nTimes > gNewZhuanPanTbl.GetCreateRoomDailyDrawMaxCnt())
	{
		nTimes = gNewZhuanPanTbl.GetCreateRoomDailyDrawMaxCnt();
	}
	if (nTimes > pActivity->m_DailyCreateRoomDarwCnt)
	{
		return DRAW_CREATE_ROOM;
	}

	//计算连续登陆的抽奖次数
	if (pActivity->m_ContinueFenXiangDays >= gNewZhuanPanTbl.GetContinueLoginNeedCnt())
	{
		return DRAW_FEN_XIANG;
	}

	return DRAW_NULL;
}

void Work::DrawEndSetData(NewZhuanPanActivity* pActivity,TYPE_DRAW type)
{
	if(NULL == pActivity)
		return ;

	switch (type)
	{
	case DRAW_NULL:
		break;
	case DRAW_BIG_WINNER:
		pActivity->m_DailyMaxWinnerDarwCnt++;
		break;
	case DRAW_CREATE_ROOM:
		pActivity->m_DailyCreateRoomDarwCnt++;
		break;
	case DRAW_FEN_XIANG:
	{
		pActivity->m_ContinueFenXiangDays = 0;
		pActivity->m_LastFenXiangTime = 0;
	}
	break;
	default:
		break;
	}
}

void Work::HanderNewZhuanPanActivityOpt(LMsgL2DBNewZhuanPanActivity* msg)
{
	if(NULL == msg)
		return ;

	boost::recursive_mutex::scoped_lock l(m_Mutex);

	LMsgS2CNewZhuanPanActivity send;
	send.m_type = msg->m_type;

	if(!gNewZhuanPanTbl.IsOpen())
	{
		send.m_errorCode = 2;
		SendToLMUser(msg->m_userId,send);
		return ;
	}	
	

	LTime cur;
	send.m_time = cur.Secs();

	NewZhuanPanActivity* pActivity = LoadNewZhuanPanActivityData(msg->m_userId);
	if(NULL == pActivity)
		return ;

	bool bMarkDirty = ResetNewZhuanPanActivityData(pActivity);
	
	//抽奖操作
	if(msg->m_type == 1)
	{
		TYPE_DRAW type = GetDrawType(pActivity);
		if(type == DRAW_NULL)
		{
			send.m_errorCode = 1;
			SendToLMUser(msg->m_userId,send);
			return ;
		}

		int nDailyDrawMoney = 0;
		if(gActiveManager.redisReadDataByString(NewZhuanPanActivityDailyDrawMoney,nDailyDrawMoney))
		{
			bool bOnlyCard = false;
			if(nDailyDrawMoney>=gNewZhuanPanTbl.GetMaxMoney())
			{
				bOnlyCard = true;
			}

			RewardCfg* pReward = gNewZhuanPanTbl.GetReward(bOnlyCard);
			if(NULL == pReward)
			{
				LLOG_ERROR("new zhuanpan activity redward config error!!");
				return ;
			}

			switch(pReward->m_redwardType)
			{
			case RT_CARD:
				{
					LMsgL2LMGModifyCard modify;
					modify.admin = "system";
					modify.cardType = CARD_TYPE_8;
					modify.cardNum = pReward->m_redwardCount;
					modify.isAddCard = 1;
					modify.operType = CARDS_OPER_TYPE_LUCKY_DRAW;
					modify.m_userid = msg->m_userId;
					gWork.SendToClient(msg->m_sp, modify);
				}
				break;
			case RT_MONEY:
				{
					if(bOnlyCard)
					{
						LLOG_ERROR("new zhuanpan activity redward money error!!");
						send.m_errorCode = 1;
						SendToLMUser(msg->m_userId,send);
						return ;
					}

					nDailyDrawMoney += pReward->m_redwardCount;
					gActiveManager.redisWriteDataByString(NewZhuanPanActivityDailyDrawMoney,nDailyDrawMoney);
				}
				break;
			}

			SaveRewardToSql(msg->m_userId,pReward);

			DrawEndSetData(pActivity,type);
			bMarkDirty = true;

			send.m_rewardType = pReward->m_redwardType;
			send.m_rewardCnt = pReward->m_redwardCount;

			LLOG_ERROR("NewZhuanPanReward userId:%d,Reward:%d-%d",msg->m_userId,pReward->m_redwardType,pReward->m_redwardCount);
		}

	}

	send.m_drawLeftTimes = GetDrawTimes(pActivity);
	send.m_dailyMaxWinnerCnt = pActivity->m_DailyMaxWinnerCnt;
	send.m_dailyCreateRoomCnt = pActivity->m_DailyCreateRoomCnt;
	send.m_continueFenXiangDays = pActivity->m_ContinueFenXiangDays;

	if(bMarkDirty)
	{
		SaveNewZhuanPanActivityToSql(pActivity);
	}

	SendToLMUser(msg->m_userId,send);
}

bool Work::SaveNewZhuanPanActivityToSql(NewZhuanPanActivity* pActivity)
{
	if(NULL == pActivity)
		return false;

	MYSQL* m = m_dbsession->GetMysql();
	std::stringstream ss;	
	ss << "INSERT INTO NewZhuanPanActivityInfo(UserId,DailyMaxWinnerCnt,DailyMaxWinnerDarwCnt,DailyCreateRoomCnt,DailyCreateRoomDarwCnt,LastFenXiangTime,ContinueFenXiangDays,ResetTime)VALUES(";
	ss<<pActivity->m_userId;
	ss<<",";
	ss<<pActivity->m_DailyMaxWinnerCnt;
	ss<<",";
	ss<<pActivity->m_DailyMaxWinnerDarwCnt;
	ss<<",";
	ss<<pActivity->m_DailyCreateRoomCnt;
	ss<<",";
	ss<<pActivity->m_DailyCreateRoomDarwCnt;
	ss<<",";
	ss<<pActivity->m_LastFenXiangTime;
	ss<<",";
	ss<<pActivity->m_ContinueFenXiangDays;
	ss<<",";
	ss<<pActivity->m_ResetTime;
	ss<<")";
	ss<<"ON DUPLICATE KEY UPDATE ";
	ss<<"DailyMaxWinnerCnt=";
	ss<<pActivity->m_DailyMaxWinnerCnt;
	ss<<",";
	ss<<"DailyMaxWinnerDarwCnt=";
	ss<<pActivity->m_DailyMaxWinnerDarwCnt;
	ss<<",";
	ss<<"DailyCreateRoomCnt=";
	ss<<pActivity->m_DailyCreateRoomCnt;
	ss<<",";
	ss<<"DailyCreateRoomDarwCnt=";
	ss<<pActivity->m_DailyCreateRoomDarwCnt;
	ss<<",";
	ss<<"LastFenXiangTime=";
	ss<<pActivity->m_LastFenXiangTime;
	ss<<",";
	ss<<"ContinueFenXiangDays=";
	ss<<pActivity->m_ContinueFenXiangDays;
	ss<<",";
	ss<<"ResetTime=";
	ss<<pActivity->m_ResetTime;
	ss<<";";

	LLOG_DEBUG("SaveNewZhuanPanActivityToSql sql:%s",ss.str().c_str());

	if (!m_dbsession->Excute(ss.str().c_str()))
	{
		LLOG_ERROR("Work::SaveNewZhuanPanActivityToSql Insert Error");
		return false;
	}

	return true;
}

void Work::SaveRewardToSql(Lint userId,RewardCfg* pReward)
{
	if(NULL == pReward)
		return ;

	MYSQL* m = m_dbsession->GetMysql();
	if (m)
	{
		LTime cur;
		std::stringstream ss;
		ss << "insert into activeGiftlog(ActiveId, UserId,SpendType,GiftType, GiftCount, Date) values(";
		ss << int(NEW_ZHUAN_PAN_ACTIVITY) << ",";
		ss << userId<< ",";
		ss << pReward->m_rewardId<<",";
		ss << pReward->m_redwardType << ",";
		ss << pReward->m_redwardCount << ",";
		ss << cur.Secs() << ")";

		//LLOG_DEBUG("SaveRewardToSql sql:%s",ss.str().c_str());

		if (!m_dbsession->Excute(ss.str().c_str()))
		{
			LLOG_ERROR("Work::SaveNewZhuanPanActivityToSql Insert Error");
		}
	}
}

void Work::HanderJiaXiangOpt(LMsgL2DBJiaXiangOpt* msg)
{
	if (NULL == msg)
		return;

	BaseInfo* pInfo = GetBaseInfo(msg->m_userId);

	if (NULL == pInfo)
	{
		pInfo = CreateBaseInfo(msg->m_userId);
	}

	if (NULL == pInfo)
		return;

	bool bMarkDirty = false;

	LMsgS2CJiaXiangOpt send;
	send.m_type = msg->m_type;
	send.m_userId = msg->m_userId;

	switch (msg->m_type)
	{
	case LMsgL2DBJiaXiangOpt::TP_OPEN:
	{
		
		send.m_city = pInfo->m_city;
		send.m_common_play = pInfo->m_common_play;

	}
	break;
	case LMsgL2DBJiaXiangOpt::TP_SET_CITY:
	{
		pInfo->m_city = msg->m_content;
		bMarkDirty = true;
	}
	break;
	case LMsgL2DBJiaXiangOpt::TP_SET_COMMON_PLAY:
	{
		pInfo->m_common_play = msg->m_content;
		bMarkDirty = true;
	}
	break;
	default:
		break;
	}

	if (bMarkDirty)
	{
		SaveBaseInfo(pInfo);
	}

	SendToLMUser(msg->m_userId,send);
}

void Work::LoadBaseInfoFromSql()
{
	MYSQL* m = m_Maindbsession.GetMysql();
	if (NULL == m)
		return;

	Lint limitfrom = 0;
	Lint count = 500000;

	//while (true)
	//{
		std::stringstream ss;
		ss << "select Id, UserId,City,CommonPlayType FROM BaseInfo ORDER BY Id";
		//ss << limitfrom << "," << count;


		LLOG_DEBUG("LoadBaseInfoFromSql sql,%s", ss.str().c_str());
		if (mysql_real_query(m, ss.str().c_str(), ss.str().size()))
		{
			LLOG_ERROR("UserManager::LoadBaseInfoFromSql _ 1 _ sql error %s", mysql_error(m));
			return ;
		}

		MYSQL_RES* res = mysql_store_result(m);
		MYSQL_ROW row = mysql_fetch_row(res);
		

		while (row)
		{
			BaseInfo * pInfo = new BaseInfo();

			pInfo->m_id = atoi(*row++);
			pInfo->m_userId = atoi(*row++);
			pInfo->m_city = *row++;
			pInfo->m_common_play = *row++;
			m_mapBaseInfo[pInfo->m_userId] = pInfo;

			row = mysql_fetch_row(res);

			LLOG_DEBUG("BaseInfo UserId:%d", pInfo->m_userId);
		}

		mysql_free_result(res);
	//}
	
}

BaseInfo* Work::CreateBaseInfo(Lint userId)
{
	MYSQL* m = m_dbsession->GetMysql();
	if (m)
	{
		BaseInfo* pInfo = new BaseInfo();
		pInfo->m_userId = userId;

		std::stringstream ss;
		ss << "INSERT INTO BaseInfo (UserId) VALUES (";
		ss << pInfo->m_userId;
		ss << ")";

		if (mysql_real_query(m, ss.str().c_str(), ss.str().size()))
		{
			LLOG_ERROR("CreateBaseInfosql error %s", ss.str().c_str());
			delete pInfo;

			return NULL;
		}

		pInfo->m_id = (Lint)mysql_insert_id(m);
		m_mapBaseInfo[pInfo->m_userId] = pInfo;
		return pInfo;
	}

	return NULL;
}

BaseInfo* Work::GetBaseInfo(Lint userId)
{
	std::map<Lint, BaseInfo*>::iterator iter = m_mapBaseInfo.find(userId);
	if (iter != m_mapBaseInfo.end())
	{
		return  iter->second;
	}

	return NULL;
}

void Work::SaveBaseInfo(BaseInfo* pInfo)
{
	if (NULL == pInfo)
		return;

	MYSQL* m = m_dbsession->GetMysql();

	if (m == NULL)
	{
		LLOG_ERROR("UserManager::SaveUser mysql null");
		return;
	}

	std::stringstream ss;
	ss << "UPDATE BaseInfo SET ";
	ss << "Id=" <<pInfo->m_id << ",";
	ss << "UserId=" << pInfo->m_userId << ",";
	ss << "City='" << pInfo->m_city << "',";
	ss << "CommonPlayType='" << pInfo->m_common_play << "'";
	ss << " WHERE UserId=" << pInfo->m_userId << "";

	LLOG_DEBUG("SaveBaseInfo sql,%s", ss.str().c_str());

	if (mysql_real_query(m, ss.str().c_str(), ss.str().size()))
	{
		LLOG_ERROR("CreateBaseInfosql error %s", ss.str().c_str());
		
		return ;
	}
}