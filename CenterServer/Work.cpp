#include "Work.h"
#include "LLog.h"
#include "Config.h"
#include "DbServer.h"
#include "InsideNet.h"
#include "UserManager.h"
#include "HttpRequest.h"
#include "jsoncpp/json.h"
#include "RemoteLogThread.h"
#include "HttpRedPacketThread.h"
#include "Base64.h"


//#define HorseSetKeyShanDong         "HorseSetShanDong"

//#define ActiveFreeSetKeyShanDong	"ActiveFreeSetShanDong"
#define ActiveAppIdList         "ActiveAppIdList"
#define ActiveFreeSetKey	"ActiveFreeSet"

//初始化
bool Work::Init()
{
	//首先设置log文件，不然log没有开启，log文件没办法查看
	LLog::Instance().SetFileName("CenterServer");

	//初始化配置文件
	if(!gConfig.Init())
	{
		LLOG_ERROR("gConfig.Init error");
		return false;
	}

	//设置log级别
	LLog::Instance().SetLogLevel(gConfig.GetLogLevel());

	//网络消息初始化
	LMsgFactory::Instance().Init();

	//数据库初始化
	if(!gDbServer.Init())
	{
		LLOG_ERROR("gDbServer.Init error");
		return false;
	}
	if(!m_dbsession.Init(gConfig.GetDbHost(),gConfig.GetDbUser(),gConfig.GetDbPass(),gConfig.GetDbName(),"utf8mb4",gConfig.GetDbPort()))
	{
		return false;
	}
	if(!gUserManager.Init())
	{
		LLOG_ERROR("gUserManager.Init error");
		return false;
	}

	LLOG_ERROR("Load users successfully");

	//外部端口初始化
	if (!gOutsideNet.Init())
	{
		LLOG_ERROR("gInsideNet.Init error");
		return false;
	}

	//gm访问端口初始化
	if (!gGmNet.Init())
	{
		LLOG_ERROR("gGmNet.Init error");
		return false;
	}

	if (!gHRP.Init(true))
	{
		LLOG_ERROR("gHRP.init error");
		return false;
	}

	//redis服务初始化
	if (!_initRedisAndActive())
	{
		LLOG_ERROR("Fail to init Redis ip and port");
		return false;
	}

	LTime cur;
	m_30MSceTick = cur.MSecs();
	m_200MSceTick = cur.MSecs();
	m_1SceTick = cur.MSecs();
	m_30SceTick = cur.MSecs();
	m_600SceTick = cur.MSecs();

	if (gRLT.init(gConfig.GetRemoteLogThreadStart(),
		(const char*)gConfig.GetRemoteLogUrl().c_str(),
		(const char*)gConfig.GetRemoteLogCacheFileName().c_str(),
		(size_t)gConfig.GetMaxCachedLogSize(),
		(size_t)gConfig.GetMaxCachedLogNum(),
		(size_t)gConfig.GetMaxFileLogSize()) < 0)
	{
		LLOG_ERROR("gRLT.init error");
		return false;
	}

	return true;
}

bool Work::Final()
{
	return true;
}

//启动
void Work::Start()
{
	//主线程开启
	LRunnable::Start();

	gRLT.start();

	//数据库线程开启
	gDbServer.Start();

	//外部网络开启
	gOutsideNet.Start();

	//gm网络开启
	gGmNet.Start();

	gHRP.start();

	m_gcService.Start();
}

//等待
void Work::Join()
{
	gGmNet.Join();

	//等待网络线程终止
	gOutsideNet.Join();

	//等待数据库线程终止
	gDbServer.Join();


	//等待逻辑线程终止
	LRunnable::Join();

}

//停止
void Work::Stop()
{
	m_gateInfo.clear();

	m_logicClient.clear();

	m_spMap.clear();

	gGmNet.Stop();

	gOutsideNet.Stop();

	gDbServer.Stop();

	gRLT.stop();

	LRunnable::Stop();

	m_gcService.Stop();
}

void Work::Clear()
{

}

void Work::Run()
{
	while(!GetStop())
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
			HanderMsg(msg);
			//delete msg;
			m_gcService.Collect(msg);
		}
	}
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

	//30秒循环一次
	if (cur.MSecs() - m_30SceTick > 30*1000)
	{
		m_30SceTick = cur.MSecs();
		SaveCurrentOnline();

		gConfig.ReloadAppIds();
	}

	//5分钟循环一次
	if (cur.MSecs() - m_600SceTick > 5*60 * 1000)
	{
		m_600SceTick = cur.MSecs();
	}
}

LTime& Work::GetCurTime()
{
	return m_tickTimer;
}

void Work::HanderMsg(LMsg* msg)
{
	if (msg == NULL)
	{
		return;
	}
	//玩家请求登录
	switch(msg->m_msgId)
	{
	case MSG_CLIENT_KICK:
		HanderUserKick((LMsgKick*)msg);
		break;
	case MSG_CLIENT_IN:
		HanderClientIn((LMsgIn*)msg);
		break;
	case MSG_HTTP:
		HanderHttp((LMsgHttp*)msg);
		break;
	case MSG_LG_CE_USER_MSG_WRAPPER:
		HanderLGWrappedMsg((LMsgLGCEUserMsgWrapper*)msg);
		break;
	case MSG_S_2_S_KEEP_ALIVE:
		HanderKeepAlive((LMsgS2SKeepAlive*)msg);
		break;
	case MSG_L_2_CE_LOGIN:
		HanderLogicLogin((LMsgL2CeLogin*)msg);
		break;
	case MSG_L_2_CE_GATE_INFO:
		HanderLogicGateInfo((LMsgL2CeGateInfo*)msg);
		break;
	case MSG_L_2_CE_USER_SAVE:
		HanderLogicSaveUser((LMsgL2CeSaveUser*) msg);
		break;
	case MSG_L_2_CE_CARD_SAVE:
		//HanderLogicSaveCardInfo((LMsgL2CeSaveCardInfo*)msg);	//注释掉，使用增量扣卡
		break;
	case MSG_L_2_CE_USER_LOGIN:
		HanderLogicUserLogin((LMsgL2CeUserServerLogin*) msg);
		break;
	case MSG_L_2_CE_USER_LOGOUT:
		HanderLogicUserLogout((LMsgL2CeUserServerLogout*) msg);
		break;
	case MSG_L_2_CE_USER_RELOGIN:
		HanderLogicUserReLogin((LMsgL2CeUerReLogin*) msg);
		break;
	case MSG_L_2_LMG_MODIFY_USER_PLAYCOUNT:
		HanderModifyUserPlayCount((LMsgL2LMGModifyPlayCount*) msg);
		break;
	case MSG_LMG_2_CE_CardExchange:
		HanderLogicManagerCardExchange((LMsgLMG2CeCardExchange*) msg);
		break;
	case MSG_LMG_2_CE_LastRelief:
		HanderLastRelief((LMsgLMG2CeLastRelief*) msg);
		break;
	case MSG_LMG_2_CE_PhoneNumber:
		HanderPhoneNumber((LMsgLMG2CePhoneNumber*) msg);
		break;
	case MSG_LMG_2_CE_FenXiang:
		HanderFenXiang((LMsgLMG2CeFenXiang*)msg);
		break;
	case MSG_L_BRODCAST_RESET_BLOCK_USER:
		HanderLogicRestBlockUser((LMsgLMGResetBlockUser*)msg);
		break;
	default:
		LLOG_ERROR("Message Not Handle,Id=%d",msg->m_msgId);
		break;
	}
}

void Work::AddSp(LSocketPtr sp)
{
	m_spMap[sp] = GetCurTime().Secs();
}

void Work::DelSp(LSocketPtr sp)
{
	if (m_spMap.count(sp))
	{
		m_spMap.erase(m_spMap.find(sp));
	}
}

void Work::HanderUserKick(LMsgKick* msg)
{
	auto itor = m_logicClient.begin();
	for (; itor != m_logicClient.end(); itor ++)
	{
		if (msg->m_sp == itor->second)
		{
			HanderLogicLogout(msg);
			break;
		}
	}

	DelSp(msg->m_sp);
}

void Work::HanderClientIn(LMsgIn* msg)
{
	if (msg == NULL)
	{
		return;
	}
	AddSp(msg->m_sp);
}

static char* ADMIN_PATH = "/cgi-bin/admin";

static void SendRet(Lstring errorInfo, LSocketPtr send)
{
	char strOK[512] = { 0 };
	
	std::string strSendBuf;
	strSendBuf.reserve(1024);
	strSendBuf += "HTTP/1.1 200 OK\r\n";

	sprintf_s(strOK, sizeof(strOK)-1, "Content-Length:%d\r\n", (Lint)errorInfo.size());
	strSendBuf += strOK;
	strSendBuf += "Content-Type: text/html; charset=utf-8\r\n\r\n";
	
	memset(strOK,0,sizeof(strOK));
	sprintf_s(strOK, sizeof(strOK)-1, "%s", errorInfo.c_str());
	strSendBuf += strOK;

	LBuffPtr buff(new LBuff());
	buff->Write(strSendBuf.c_str(), strSendBuf.size());
	send->Send(buff);
	//send->Kick();
}


//处理玩家登陆的消息 
int Work::HanderUserLogin(LMsgC2SMsg* msg, LMsgS2CMsg& ret)
{
	if (msg == NULL)
	{
		return 0;
	}

	LLOG_DEBUG("HanderUserLogin!");

	if(!IsRightAppID(msg->m_nAppId))
	{
		LLOG_ERROR("Work::HanderUserLogin Error AppId %d",msg->m_nAppId);
		ret.m_errorCode = 7;
		return 7;
	}

	if(msg->m_plate == "wechat")
	{
		LLOG_DEBUG("HanderUserLogin wechat!");

		return HanderUserWechatLogin(msg, ret);
	}
	else if(gConfig.GetLocalLogin()=="true" && msg->m_plate == "local"){
	
		HanderUserLocalLogin(msg, ret);
	}
	else
	{
		LLOG_ERROR("Work::HanderUserLogin plate error %s",msg->m_plate.c_str());
	}

	return 0;
}

int Work::HanderUserWechatLogin(LMsgC2SMsg* msg, LMsgS2CMsg& ret)
{
	if (msg == NULL)
	{
		return 0;
	}

	ret.m_errorCode = 1;//服务器还没启动成功
	auto itor = m_logicClient.find(msg->m_severID);
	if (itor == m_logicClient.end())
	{
		LLOG_ERROR("Work::HanderUserWechatLogin logicid not exiext %d",msg->m_severID);
		return 3;
	}

	if (msg->m_uuid.empty() || msg->m_uuid.size()>32)
	{
		LLOG_ERROR("Error! HanderUserWechatLogin, uuid is invalid!");
		return 4;
	}
	
	DUser* user = gUserManager.GetUserByOpenId(msg->m_uuid);

	if (user == NULL)
	{
		Lstring errMsg;
		Lint checkDevice = _checkDeviceWhenReg(msg->m_deviceId, errMsg);
		if (checkDevice != 2000)
		{
			// device 8001: 注册过于频繁  8002：设备已被封禁
			//reply->m_errorCode = checkDevice;
			//reply->m_errorMsg = errMsg;
			LLOG_ERROR("Work::HanderUserWechatLogin Device cannot regist %d", checkDevice);
			return checkDevice;
		}

		if (gConfig.GetWxAuthSwitch())
		{
			LLOG_DEBUG("[DEBUG] config start wx Auth");
			if (msg->m_wxConfirmed == 0) 
			{
				LLOG_DEBUG("[DEBUG] Work::HanderUserWechatLogin m_wxConfirmed: %d, Send to Login", msg->m_wxConfirmed);
				return 1;
			} 
			else 
			{
				LLOG_DEBUG("[DEBUG] wx has Confirmed");
			}
		}

		//处理性别值异常
		if (msg->m_sex < 1 || msg->m_sex > 2)
		{
			msg->m_sex = 2;
		}

		//玩家昵称转义
		for (Lsize i = 0; i < msg->m_nikename.size(); ++i)
		{
			if (msg->m_nikename[i] == '\'' || msg->m_nikename[i] == '\n' || msg->m_nikename[i] == '\t' || msg->m_nikename[i] == '|')
				msg->m_nikename[i] = '-';
		}

		user = new DUser();
		user->m_usert.m_openid = msg->m_openId;
		user->m_usert.m_unioid = msg->m_uuid;
		user->m_usert.m_nike = msg->m_nikename;
		user->m_usert.m_sex = msg->m_sex;
		user->m_usert.m_headImageUrl = msg->m_imageUrl;
		user->m_usert.m_regTime = gWork.GetCurTime().Secs();
		user->m_usert.m_lastLoginTime = gWork.GetCurTime().Secs();
		user->m_usert.m_nAppId = msg->m_nAppId;

		if (!gUserManager.CreateUser(user))
		{
			ret.m_errorCode = 4;//创建角色失败
			delete user;
			return 6;
		}
		gUserManager.AddUser(user);

		////玩家注册日志
		RLOG("reg", LTimeTool::GetLocalTimeInString()
			<< "|" << user->m_usert.m_nAppId
			<< "|" << user->m_usert.m_id
			<< "|" << user->m_usert.m_sex
			<< "|" << msg->m_clientIp
			<< "|" << "V4.00.00"
 		);

		BindRegRequest request;
		request.m_invitee = user->m_usert.m_id;
		std::stringstream ss1;
		ss1 << gConfig.GetRedPacketBindUrl() << "red/getInviter?serverCode=shandong_db33001&unionId=" << user->m_usert.m_unioid;
		request.m_url = ss1.str();
		gHRP.push(request);

		if (gConfig.GetDeviceVerify() && !msg->m_deviceId.empty())
		{
			LMsgDeviceCheckRequest* deviceCheck = new LMsgDeviceCheckRequest;
			deviceCheck->m_verifyType = 1;
			deviceCheck->m_userId = user->m_usert.m_id;
			deviceCheck->m_serverId = user->m_usert.m_nAppId;
			deviceCheck->m_deviceType = msg->m_deviceType;
			deviceCheck->m_deviceId = msg->m_deviceId;
			deviceCheck->m_loginIp = msg->m_clientIp;
			HanderDeviceCheckRequest(deviceCheck);
		}
	}
	else
	{
		if (msg->m_reLogin == 0)
		{
			LMsgDeviceCheckRequest* deviceCheck = new LMsgDeviceCheckRequest;
			deviceCheck->m_verifyType = 1;
			deviceCheck->m_userId = user->m_usert.m_id;
			deviceCheck->m_serverId = user->m_usert.m_nAppId;
			deviceCheck->m_deviceType = msg->m_deviceType;
			deviceCheck->m_deviceId = msg->m_deviceId;
			deviceCheck->m_loginIp = msg->m_clientIp;

			HanderDeviceCheckRequest(deviceCheck);
		}

		//昵称中的单引号处理
		for(Lsize i = 0 ; i < msg->m_nikename.size(); ++i)
		{
			if(msg->m_nikename[i] == '\'' || msg->m_nikename[i] == '\n' || msg->m_nikename[i] == '\t' || msg->m_nikename[i] == '|')
				msg->m_nikename[i] = ' ';
		}

		user->m_usert.m_nike = msg->m_nikename;
		user->m_usert.m_sex = msg->m_sex;
		user->m_usert.m_headImageUrl = msg->m_imageUrl;
		user->m_usert.m_unioid = msg->m_uuid;
		//user->m_usert.m_regTime = gWork.GetCurTime().Secs();
		user->m_usert.m_lastLoginTime = gWork.GetCurTime().Secs();
	}

	user->m_lastReqTime = gWork.GetCurTime().Secs();
	if (user->m_logicID != msg->m_severID && user->m_logicID != 0 )
	{
		//不是在本服务器登陆的玩家, 发送消息，提示退出原APP
		LLOG_DEBUG("Work::HanderUserWechatLogin logicid not empty %d : %d",msg->m_severID,user->m_logicID);
		ret.m_errorCode = 5;
		ret.m_errorMsg = string_To_UTF8(getServerNameByID(user->m_logicID)) ;
		//msg->m_sp->Send(ret.GetSendBuff());
		return 7;
	}

	if(user->m_usert.m_nAppId != msg->m_nAppId)
	{
		//不是该客户端用户
		ret.m_errorCode = 5;
		ret.m_errorMsg = "AppId Error!" ;
		return 7;
	}

	user->SetCopy(true);
	LMsgCe2LUserLogin send;
	send.m_seed = L_Rand(10000000, 99999999);
	send.user = user->m_usert;
	SendMsgToLogic(send, msg->m_severID);

	ret.m_errorCode = 0;
	GateInfo* info = GetGateUsefulInfo(msg->m_severID);
	if (info)
	{
		ret.m_gateIp = info->m_ip;
		ret.m_gatePort = info->m_port;
	}

	ret.m_id = user->m_usert.m_id;
	ret.m_seed = send.m_seed;
	ret.m_totalPlayNum = user->m_usert.m_totalplaynum;

	LLOG_DEBUG("Handle WeChat login success!");

	return 0;
}

void Work::HanderUserLocalLogin(LMsgC2SMsg* msg, LMsgS2CMsg& ret)
{
	if (msg == NULL)
	{
		return;
	}
	ret.m_errorCode = 0;
	auto itor = m_logicClient.find(msg->m_severID);
	if (itor == m_logicClient.end())
	{
		ret.m_errorCode = 1;
		//msg->m_sp->Send(ret.GetSendBuff());
		return;
	}

	msg->m_uuid = msg->m_openId;
	if (msg->m_uuid.empty() || msg->m_uuid.size() > 32)
	{
		ret.m_errorCode = 2;
		//msg->m_sp->Send(ret.GetSendBuff());
		return;
	}
	DUser* user = gUserManager.GetUserByOpenId(msg->m_uuid);
	if (user == NULL)
	{
		Lstring errMsg;
		Lint checkDevice = _checkDeviceWhenReg(msg->m_deviceId, errMsg);
		if (checkDevice != 2000)
		{
			// device 8001: 注册过于频繁  8002：设备已被封禁
			//reply->m_errorCode = checkDevice;
			//reply->m_errorMsg = errMsg;
			ret.m_errorCode = checkDevice;
			LLOG_ERROR("Work::HanderUserLocalLogin Device cannot regist %d", checkDevice);
			return ;
		}

		//玩家昵称转义
		for (Lsize i = 0; i < msg->m_nikename.size(); ++i)
		{
			if (msg->m_nikename[i] == '\'' || msg->m_nikename[i] == '\n' || msg->m_nikename[i] == '\t' || msg->m_nikename[i] == '|')
				msg->m_nikename[i] = '-';
		}

		user = new DUser();
		user->m_usert.m_openid = msg->m_openId;
		user->m_usert.m_unioid = msg->m_uuid;
		//user->m_usert.m_nike = msg->m_nike;
		user->m_usert.m_nike = msg->m_nikename;
		user->m_usert.m_sex = msg->m_sex;
		user->m_usert.m_headImageUrl = msg->m_imageUrl;
		user->m_usert.m_regTime = gWork.GetCurTime().Secs();
		user->m_usert.m_lastLoginTime = gWork.GetCurTime().Secs();
		user->m_usert.m_nAppId = msg->m_nAppId;

		if (!gUserManager.CreateUser(user))
		{
			ret.m_errorCode = 4;//创建角色失败
			//msg->m_sp->Send(ret.GetSendBuff());
			delete user;
			return;
		}
		gUserManager.AddUser(user);
		
		////玩家注册日志
		RLOG("reg", LTimeTool::GetLocalTimeInString()
			<< "|" << user->m_usert.m_nAppId
			<< "|" << user->m_usert.m_id
			<< "|" << user->m_usert.m_sex
			<< "|" << msg->m_clientIp
			<< "|" << "V4.00.00"
		);

		BindRegRequest request;
		request.m_invitee = user->m_usert.m_id;
		std::stringstream ss1;
		ss1 << gConfig.GetRedPacketBindUrl() << "red/getInviter?serverCode=shandong_db33001&unionId=" << user->m_usert.m_unioid;
		request.m_url = ss1.str();
		gHRP.push(request);

		if (gConfig.GetDeviceVerify() && !msg->m_deviceId.empty())
		{
			LMsgDeviceCheckRequest* deviceCheck = new LMsgDeviceCheckRequest;
			deviceCheck->m_verifyType = 1;
			deviceCheck->m_userId = user->m_usert.m_id;
			deviceCheck->m_serverId = user->m_usert.m_nAppId;
			deviceCheck->m_deviceType = msg->m_deviceType;
			deviceCheck->m_deviceId = msg->m_deviceId;
			deviceCheck->m_loginIp = msg->m_clientIp;
			HanderDeviceCheckRequest(deviceCheck);
		}
	}
	else
	{
		if (msg->m_reLogin == 0)
		{
			LMsgDeviceCheckRequest* deviceCheck = new LMsgDeviceCheckRequest;
			deviceCheck->m_verifyType = 1;
			deviceCheck->m_userId = user->m_usert.m_id;
			deviceCheck->m_serverId = user->m_usert.m_nAppId;
			deviceCheck->m_deviceType = msg->m_deviceType;
			deviceCheck->m_deviceId = msg->m_deviceId;
			deviceCheck->m_loginIp = msg->m_clientIp;

			HanderDeviceCheckRequest(deviceCheck);
		}
	}

	if (user->m_logicID != msg->m_severID && user->m_logicID > 0)
	{
		//不是在本服务器登陆的玩家, 发送消息，提示退出原APP
		ret.m_errorCode = 5;
		ret.m_errorMsg = string_To_UTF8(getServerNameByID(user->m_logicID)) ;
		//msg->m_sp->Send(ret.GetSendBuff());
		return;
	}

	if(user->m_usert.m_nAppId != msg->m_nAppId)
	{
		//不是该客户端用户
		ret.m_errorCode = 5;
		ret.m_errorMsg = "AppId Error!" ;
		return ;
	}

	user->SetCopy(true);

	LMsgCe2LUserLogin send;
	send.m_seed = L_Rand(10000000, 99999999);
	send.user = user->m_usert;
	SendMsgToLogic(send, msg->m_severID);

	GateInfo* info = GetGateUsefulInfo(msg->m_severID);
	if (info)
	{
		ret.m_gateIp = info->m_ip;
		ret.m_gatePort = info->m_port;
	}

	ret.m_id = user->m_usert.m_id;
	ret.m_seed = send.m_seed;
	ret.m_totalPlayNum = user->m_usert.m_totalplaynum;
}

/*
	errorCode 类型
	0	-	请求成功
	1	-	请求url错误
	2	-	消息类型不存
	3	-	sign不正确
	4	-	请求超时
	5	-	请求方法错误
	6	-	请求参数错误
*/

void Work::HanderHttp(LMsgHttp* msg)
{	
	if (msg == NULL)
	{
		return;
	}
 	if (msg->m_head.size() > 4 && msg->m_head.substr(0, 4) == "GET ")
 	{
 		//std::string::size_type pos = msg->m_head.find_last_of("HTTP");
		std::string::size_type pos = msg->m_head.find("\r\n");
 		if (pos == std::string::npos || pos < 8)
 		{
 			LLOG_ERROR("Work::HanderHttp GET error");
			SendRet("{\"errorCode\":5,\"errorMsg\":\"req function error\"}",msg->m_sp);
 			return;
 		}
 
 		//std::string totalUrl = msg->m_head.substr(4, pos-8);
		std::string totalUrl = msg->m_head.substr(4, pos - 13);
 		std::string::size_type pos2 = totalUrl.find('?', 0);
 		if (pos2 == std::string::npos)
 		{
 			LLOG_ERROR("Work::HanderHttp GET param error");
			SendRet("{\"errorCode\":6,\"errorMsg\":\"req param error\"}", msg->m_sp);
 			return;
 		}
 
 		std::string reqType = totalUrl.substr(0, pos2);
 		std::string reqParam = totalUrl.substr(pos2 + 1, totalUrl.size() - pos2);
 		if (reqType == ADMIN_PATH )
 		{
  			std::map<Lstring, Lstring> paraMaps;
  			HttpRequest::Instance().DeodeUrlParams(reqParam, paraMaps, true);
			if (!HanderCheckMd5(paraMaps))
			{
				SendRet("{\"errorCode\":3,\"errorMsg\":\"sign error\"}", msg->m_sp);//签名错误
				return;
			}

			//Lint time = atoi(paraMaps["time"].c_str());
			//if (gWork.GetCurTime().Secs() - time > 5)
			//{
			//	SendRet("{\"errorCode\":4,\"errorMsg\":\"time out\"}", msg->m_sp);//超时
			//	return;
			//}
  			if (paraMaps["msg"] == "charge")
				HanderGMCharge(paraMaps,msg->m_sp);
  			else if (paraMaps["msg"] == "horse")
				HanderGMHorse(paraMaps, msg->m_sp);
			else if (paraMaps["msg"] == "Free")
				HanderSetFreeSet(paraMaps, msg->m_sp);
			else if (paraMaps["msg"] == "ActivityInvite") //表示转盘抽奖相关活动
				HanderActivityDrawSet(paraMaps, msg->m_sp);
			else if (paraMaps["msg"] == "buy")
				HanderGMBuyInfo(paraMaps, msg->m_sp);
			else if (paraMaps["msg"] == "online")
				HanderGMOnlineCount(paraMaps, msg->m_sp);
			else if (paraMaps["msg"] == "close")
				HanderGMCloseServer(paraMaps, msg->m_sp);
			else if (paraMaps["msg"] == "board")
				HanderGMboard(paraMaps, msg->m_sp);
			else if (paraMaps["msg"] == "payswitch")
				HanderGMPaySwitch(paraMaps, msg->m_sp);
			else if (paraMaps["msg"] == "ActivityRedPackets") //要求玩家送红包
				HanderActivityRedPackets(paraMaps, msg->m_sp);
			else if (paraMaps["msg"] == "policy")
				HanderGMPolicyBlock(paraMaps, msg->m_sp);
			else
				SendRet("{\"errorCode\":2,\"errorMsg\":\"msg type error\"}", msg->m_sp);//消息类型错误
 		}
 		else
 		{
			SendRet("{\"errorCode\":1,\"errorMsg\":\"get url error\"}", msg->m_sp);//请求链接错误
 		}
 	}
}


void Work::HanderLogicLogin(LMsgL2CeLogin* msg)
{
	if (msg == NULL)
	{
		return;
	}
	//判断key是否正确
	if (msg->m_key != gConfig.GetCenterKey())
	{
		msg->m_sp->Stop();
		return;
	}

	auto itor = m_logicClient.find(msg->m_ID);
	if (itor != m_logicClient.end())
	{
		if (itor->second)
		{
			if(itor->second != msg->m_sp)
			{
				itor->second->Stop();
				m_logicClient.erase(itor);
				LLOG_ERROR("Work::HanderLogicLogin: delete already existed logicmanger %d.", msg->m_ID);
			}
			else
				return;
		}
	}

	m_logicClient[msg->m_ID] = msg->m_sp;
	serverInfo[msg->m_ID] = msg->m_ServerName;
	if (msg->m_NeedLoadUsers)
	{
		gUserManager.LoadUserIdInfo(msg->m_ID);
	}
	
	DelSp(m_logicClient[msg->m_ID]);

	// 发送活动配置信息及跑马灯
	SendActiveSetToLM(msg->m_ID);
}

void Work::HanderLogicLogout(LMsgKick*msg)
{
	if (msg == NULL)
	{
		return;
	}
	LLOG_ERROR("Work::HanderLogicLogout");
	auto itor = m_logicClient.begin();
	for (; itor != m_logicClient.end(); itor ++)
	{
		if (msg->m_sp == itor->second)
		{
			Lint	serverID = itor->first;
			m_logicClient.erase(itor);
			auto iGate = m_gateInfo.find(serverID);
			if (iGate != m_gateInfo.end())
			{
				m_gateInfo.erase(iGate);
			}
			break;
		}
	}
}

void Work::HanderLogicGateInfo(LMsgL2CeGateInfo* msg)
{
	if (msg == NULL)
	{
		return;
	}
	if (msg->m_ID <= 0)
	{
		LLOG_ERROR("Work::HanderLogicGateInfo %d", msg->m_ID);
		return;
	}
	auto itor = m_logicClient.find(msg->m_ID);
	if (itor == m_logicClient.end())
	{
		msg->m_sp->Stop();
		return;
	}
	m_gateInfo[msg->m_ID].clear();
	for (Lint i = 0; i < msg->m_count; ++i)
	{
		m_gateInfo[msg->m_ID][msg->m_gate[i].m_id] = msg->m_gate[i];
	}

	for(std::map<Lint,Lint>::iterator it=msg->m_userCountMap.begin();it!=msg->m_userCountMap.end();++it)
	{
		m_onlineUserCount[it->first]=it->second;
	}
}

void Work::HanderLogicSaveCardInfo(LMsgL2CeSaveCardInfo* msg)
{	
	if (NULL != msg && !msg->m_charge.m_operator.empty())
	{
		LTime tm;
		tm.SetSecs(msg->m_charge.m_time);
		std::stringstream ssTableName;
		ssTableName << "charge_";
		ssTableName.fill('0');
		ssTableName.width(4);
		ssTableName << tm.GetYear();
		ssTableName.width(2);
		ssTableName << (tm.GetMonth()+1);
		ssTableName << " ";
 
		std::stringstream ss;
		ss << "INSERT INTO ";
		ss << ssTableName.str();  //分表路由
		ss << "(ServerId,UserId,Time ,CardNum,Type,Balance,Operator,Remark) VALUES (";
		ss << "'" << msg->m_charge.m_nAppId << "',";
		ss << "'" << msg->m_charge.m_userId << "',";
		ss << "'" << msg->m_charge.m_time << "',";
		ss << "'" << msg->m_charge.m_cardNum << "',";
		ss << "'" << msg->m_charge.m_operType << "',";
		ss << "'" << msg->m_charge.m_balance << "',";
		ss << "'" << msg->m_charge.m_operator << "',";
		ss << "'" << msg->m_charge.m_remark << "')";

		LLOG_DEBUG("Work::HanderLogincSaveCardInfo save %s", ss.str().c_str());

		Lstring* sql = new Lstring(ss.str());
		gDbServer.Push(sql);
	}
}


void Work::HanderLogicSaveUser(LMsgL2CeSaveUser* msg)
{
	if (msg == NULL)
	{
		return;
	}

	DUser* user = gUserManager.GetUserByOpenId(msg->m_user.m_unioid);
	if(user == NULL)
	{
		LLOG_ERROR("Work::HanderLogincSaveUser save not exiest %d:%s",msg->m_user.m_id,msg->m_user.m_unioid.c_str());
		return;
	}

	if (user->m_logicID != msg->m_ID && user->m_logicID != 0)
	{
		LLOG_ERROR("Work::HanderLogincSaveUser logic error %d:%d:%d:%s",msg->m_user.m_id,user->m_logicID,msg->m_ID,msg->m_user.m_unioid.c_str());
		return;
	}
	
	user->SetCopy(true);
	user->m_usert = msg->m_user;
	gUserManager.SaveUser(user);	
}

GateInfo* Work::GetGateUsefulInfo(Lint logicID)
{
	static Lint sindex = 0;
	GateInfo* info = NULL;
	auto itor = m_gateInfo.find(logicID);
	if (itor == m_gateInfo.end())
	{
		return info;
	}
	
	Lint index = 0;
	auto it = itor->second.begin();
	for (; it != itor->second.end(); ++it)
	{
		if (index++ % itor->second.size() == sindex % itor->second.size())
		{
			info = &it->second;
			break;
		}
	}

	sindex++;
	return info;
}

void Work::SendMsgToLogic(LMsg& msg, Lint logicID)
{
	auto itor = m_logicClient.find(logicID);
	if (itor != m_logicClient.end())
	{
		itor->second->Send(msg.GetSendBuff());
	}
	else
	{
		LLOG_ERROR("void Work::SendMsgToLogic ERROR: %d is invilid", logicID);
	}
}

void Work::HanderLGWrappedMsg(LMsgLGCEUserMsgWrapper* msg)
{
	LLOG_DEBUG("Enter Work::HanderLGWrappedMsg");
	if(msg == NULL || msg->m_userMsg == NULL)
		return;

	if(msg->m_userMsg->m_msgId == MSG_C_2_S_MSG)
	{
		LMsgS2CMsg ret;
		int retCode = HanderUserLogin((LMsgC2SMsg*)msg->m_userMsg, ret);

		//返回1,新用户,需要验证微信
		if (1 == retCode)
		{
			LMsgLGCEUserMsgWrapper wrapper;
			wrapper.m_idOnGate = msg->m_idOnGate;
			wrapper.m_userMsg = msg->m_userMsg;
			wrapper.m_deleteUserMsg = false;
			msg->m_sp->Send(wrapper.GetSendBuff());
		}
		else
		{
			LMsgLGCEUserMsgWrapper wrapper;
			wrapper.m_idOnGate = msg->m_idOnGate;
			wrapper.m_userMsg = &ret;
			wrapper.m_deleteUserMsg = false;
			msg->m_sp->Send(wrapper.GetSendBuff());
		}
	}
	LLOG_DEBUG("Leave Work::HanderLGWrappedMsg");
}

//设置玩家登陆标志位
void Work::HanderLogicUserLogin(LMsgL2CeUserServerLogin* msg)
{
	if (msg == NULL)
	{
		return;
	}
	LLOG_DEBUG("Work::HanderLogicUserLogin, userID: %s, ServerID %d", msg->m_openID.c_str(), msg->m_serverID);
	DUser* user = gUserManager.GetUserByOpenId(msg->m_openID);
	if (user == NULL)
	{
		//没找到玩家
		return;
	}

	auto itor = m_logicClient.find(msg->m_serverID);
	if (itor == m_logicClient.end())
	{
		//没找到这台服务器
		user->m_logicID = 0;
		return;
	}
	user->m_logicID = msg->m_serverID;
	user->m_usert.m_lastLoginTime =  gWork.GetCurTime().Secs();
	user->m_usert.m_location = msg->m_location;
	gUserManager.SaveUserLastLogin(user);		//玩家最后登陆时间
}

void Work::HanderLogicUserLogout(LMsgL2CeUserServerLogout* msg)
{
	if (msg == NULL)
	{
		return;
	}
	LLOG_DEBUG("Work::HanderLogicUserLogot, userID: %s, ServerID %d", msg->m_openID.c_str(), msg->m_serverID);
	DUser* user = gUserManager.GetUserByOpenId(msg->m_openID);
	if (user == NULL)
	{
		//没找到玩家
		return;
	}

	auto itor = m_logicClient.find(msg->m_serverID);
	if (itor == m_logicClient.end())
	{
		//没找到这台服务器
		return;
	}
	user->m_logicID = 0;
	gUserManager.SaveUser(user);		//玩家离线，保存玩家数据
}


bool Work::HanderCheckMd5(std::map<Lstring, Lstring>& param)
{
	std::vector<std::string> keys;
	for (std::map<std::string, std::string>::const_iterator it = param.begin(); it != param.end(); ++it)
	{
		const std::string& strKey = it->first;
		if (strKey.compare("sign") != 0)
			keys.push_back(strKey);
	}
	std::sort(keys.begin(), keys.end());
	std::string reqsort;
	for (size_t i = 0; i < keys.size(); ++i)
	{
		reqsort += keys[i] + param[keys[i]];
	}
	reqsort += "1234567890987654321";


 	std::string sign = HttpRequest::Instance().Md5Hex(reqsort.c_str(), reqsort.length());
 	LLOG_DEBUG("Work::HanderCheckMd5,source:%s,sign:%s", reqsort.c_str(), sign.c_str());
 
 	//加密参数不对
	if (sign != param["sign"])
 	{
		LLOG_ERROR("Work::HanderCheckMd5 sign error %s:%s", param["sign"].c_str(), sign.c_str());
 		return false;
 	}

	return true;
}

void Work::HanderGMCharge(std::map<Lstring, Lstring>& param, LSocketPtr sp)
{
	//admin=admin openId=openId cardType=1 cardNum=1 operType=1 sign=sign time=time

	LLOG_ERROR("Work::HanderGMCharge %s:%s", param["openId"].c_str(), param["admin"].c_str());

	Lint	errorCode = 0;
	DUser*	user = gUserManager.GetUserByOpenId(param["openId"]);
	Lint	cardType = atoi(param["cardType"].c_str());
	Lint	cardNum = atoi(param["cardNum"].c_str());
	Lint	operType = atoi(param["operType"].c_str());
	if (user)
	{
		if (user->m_usert.m_blockEndTime == -1 || user->m_usert.m_blockEndTime > gWork.GetCurTime().Secs())
		{
			SendRet("{\"errorCode\":7,\"errorMsg\":\"user not exiest\"}", sp);//玩家不存在
			return;
		}

		if (user->m_logicID)//这里在logic里面有备份
		{
			LMsgCe2LGMCharge gmcharge;
			gmcharge.m_cardType = cardType;
			gmcharge.m_cardCount = cardNum;
			gmcharge.m_oper = operType;
			gmcharge.m_userid = user->m_usert.m_id;
			gmcharge.m_admin = param["admin"];
			gmcharge.m_appId = user->m_usert.m_nAppId;
			SendMsgToLogic(gmcharge, user->m_logicID);
		}

		switch (cardType)
		{
		case CARD_TYPE_4:
			user->m_usert.m_numOfCard1s += cardNum;
			user->m_usert.m_totalbuynum += cardNum / 2;
			break;
		case CARD_TYPE_8:
			user->m_usert.m_numOfCard2s += cardNum;
			user->m_usert.m_totalbuynum += cardNum;
			break;
		case CARD_TYPE_16:
			user->m_usert.m_numOfCard3s += cardNum;
			user->m_usert.m_totalbuynum += cardNum * 2;
			break;
		}

		gUserManager.SaveUser(user);

		LMsgL2CeSaveCardInfo msg;
		LCharge& charge = msg.m_charge;
		
		charge.m_userId = user->m_usert.m_id;
		charge.m_time = gWork.GetCurTime().Secs();
		charge.m_cardNum = cardNum;
		charge.m_operType = operType;
		charge.m_balance = user->m_usert.m_numOfCard2s;
		charge.m_operator = param["admin"];
		HanderLogicSaveCardInfo(&msg);

		SendRet("{\"errorCode\":0,\"errorMsg\":\"success\"}", sp);//成功
	}
	else
	{
		SendRet("{\"errorCode\":7,\"errorMsg\":\"user not exiest\"}", sp);//玩家不存在
	}
}

void Work::HanderGMHorse(std::map<Lstring, Lstring>& param, LSocketPtr sp)
{
	//admin=admin horse=horse sign=sign time=time serverid=10
	LMsgCe2LGMHorse horse;
	horse.m_str = param["horse"];
	LLOG_ERROR("Work::HanderGMHorse %s", param["horse"].c_str());
	//m_strHorse = param["horse"];
	//if (!m_RedisClient.writeDataByString(HorseSetKeyShanDong, m_strHorse))
	//{
	//	LLOG_ERROR("Work::HanderGMHorse sets storage Redis failed, please check Redis-server");
	//}

	if (param["serverid"].empty())
	{
		auto iServer = m_logicClient.begin();
		for (; iServer != m_logicClient.end(); ++iServer)
		{
			SendMsgToLogic(horse, iServer->first);
		}
	}
	else
	{
		Lint nServerID = atoi(param["serverid"].c_str());
		SendMsgToLogic(horse, nServerID);
	}

	SendRet("{\"errorCode\":0,\"errorMsg\":\"success\"}", sp);//成功
}

void Work::HanderGMBuyInfo(std::map<Lstring, Lstring>& param, LSocketPtr sp)
{
	//admin=admin buy=buy sign=sign time=time serverid=10
	LMsgCe2LGMBuyInfo buy;
	buy.m_str = param["buy"];
	if (param["serverid"].empty())
	{
		auto iServer = m_logicClient.begin();
		for (; iServer != m_logicClient.end(); ++iServer)
		{
			SendMsgToLogic(buy, iServer->first);
		}
	}
	else
	{
		Lint nServerID = atoi(param["serverid"].c_str());
		SendMsgToLogic(buy, nServerID);
	}
	SendRet("{\"errorCode\":0,\"errorMsg\":\"success\"}", sp);//成功
}

void Work::HanderGMboard(std::map<Lstring, Lstring>& param, LSocketPtr sp)
{
	LMsgCe2LGMBoardInfo board;
	board.m_str = param["msginfo"];
	board.m_strMsgid = param["msgid"];
	if (param["serverid"].empty())
	{
		auto iServer = m_logicClient.begin();
		for (; iServer != m_logicClient.end(); ++iServer)
		{
			SendMsgToLogic(board, iServer->first);
		}
	}
	else
	{
		Lint nServerID = atoi(param["serverid"].c_str());
		SendMsgToLogic(board, nServerID);
	}
	SendRet("{\"errorCode\":0,\"errorMsg\":\"success\"}", sp);//成功
}

void Work::HanderGMPaySwitch(std::map<Lstring, Lstring>& param, LSocketPtr sp)
{
	LMsgCe2LGMPaySwitch pay_switch;
	pay_switch.m_open_ios_pay = atoi(param["iospay"].c_str());
	pay_switch.m_open_android_pay = atoi(param["androidpay"].c_str());

	if (param["serverid"].empty())
	{
		auto iServer = m_logicClient.begin();
		for (; iServer != m_logicClient.end(); ++iServer)
		{
			SendMsgToLogic(pay_switch, iServer->first);
		}
	}
	else
	{
		Lint nServerID = atoi(param["serverid"].c_str());
		SendMsgToLogic(pay_switch, nServerID);
	}


	SendRet("{\"errorCode\":0,\"errorMsg\":\"success\"}", sp);//成功
}


void Work::HanderGMOnlineCount(std::map<Lstring, Lstring>& param, LSocketPtr sp)
{
	//admin=admin sign=sign time=time

	std::stringstream ss;
	ss << "{\"errorCode\":0,\"errorMsg\":\"success\",\"data\":[";

	Lint nCount = 0;
	auto it = m_gateInfo.begin();
	for (; it != m_gateInfo.end(); it ++)
	{
		auto itor = it->second.begin();
		for (; itor != it->second.end(); itor ++)
		{
			nCount += itor->second.m_userCount;
		}
	}

	ss << "{\"count\":" << nCount << "},";

	Lstring tmp = ss.str().substr(0, ss.str().size() - 1);
	tmp += "]}";

	SendRet(tmp, sp);
}

void Work::HanderGMCloseServer(std::map<Lstring, Lstring>& param, LSocketPtr sp)
{
	LLOG_DEBUG("Work::HanderGMCloseServer %s", param["cmd"].c_str());
	if (param["cmd"] == "close_server_ty_ll")
	{
		SendRet("{\"errorCode\":0,\"errorMsg\":\"success\"}", sp);//成功
		Stop();
		
	}
	else
	{
		SendRet("{\"errorCode\":10,\"errorMsg\":\"close cmd error\"}", sp);//成功
		//Stop();
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

DBSession& Work::GetDbSession()
{
	return m_dbsession;
}

Lstring& Work::getServerNameByID(Lint serverID)
{
	static Lstring unknowServer = "未知麻将";
	std::map<Lint, Lstring>::iterator itor = serverInfo.find(serverID);
	if (itor != serverInfo.end())
	{
		return serverInfo[serverID];
	}
	return unknowServer;
}

void Work::SaveCurrentOnline()
{
	
	auto iServer = m_onlineUserCount.begin();
	for (; iServer != m_onlineUserCount.end(); iServer ++)
	{
		
		
		std::stringstream ss;
		ss << "insert into onlinelog(DateTime, serverID, serverName, onlineCount) values('";
		ss << gWork.GetCurTime().Secs() << "','";
		ss << iServer->first << "','";
		ss << string_To_UTF8(gConfig.GetAppServerName(iServer->first))  << "','";
		ss << iServer->second << "')";

		Lstring* sql = new Lstring(ss.str());
		gDbServer.Push(sql);
	}
}

void Work::HanderLogicUserReLogin(LMsgL2CeUerReLogin* msg)
{
	if (msg == NULL)
	{
		return;
	}
	LLOG_DEBUG("Work::HanderLogicUserReLogin, userID: %s, ServerID %d", msg->m_uuid.c_str(), msg->m_ID);
	DUser* user = gUserManager.GetUserByOpenId(msg->m_uuid);
	if (user == NULL)
	{
		//没找到玩家
		return;
	}

	auto itor = m_logicClient.find(msg->m_ID);
	if (itor == m_logicClient.end())
	{
		//没找到这台服务器
		return;
	}
	if (user->m_logicID != 0)
	{
		auto itor = m_logicClient.find(user->m_logicID);
		if (itor == m_logicClient.end())
		{
			//没找到这台服务器, 或这台服务器宕机了， 允许登陆其他服务器
		}		
		else if (user->m_logicID != msg->m_ID )
		{
			//玩家所在所务器正常连接，并且玩家在这台服务器
			return;
		}
	}

	//允许玩家登陆, 直接记录， 防止有玩家两个APP同时请求
	user->m_logicID = msg->m_ID;

	LMsgCe2LUserLogin send;
	send.m_seed = msg->m_seed;
	send.user = user->m_usert;
	SendMsgToLogic(send, msg->m_ID);


	LMsgCe2LUserReLogin relogin;
	relogin.m_ip = msg->m_ip;
	relogin.m_gateId = msg->m_gateId;
	relogin.m_gateUserId = msg->m_gateUserId;
	relogin.m_md5 = msg->m_md5;
	relogin.m_seed = msg->m_seed;
	relogin.m_userid = msg->m_userid;
	SendMsgToLogic(relogin, msg->m_ID);
}

void Work::HanderModifyUserPlayCount(LMsgL2LMGModifyPlayCount* msg) 
{
	if (NULL == msg)
	{
		LLOG_ERROR("Work::HanderModifyUserPlayCount: message is null.");
		return;
	}

	DUser* user = gUserManager.GetUserByOpenId(msg->m_user_unioid);
	
	if(user == NULL)
	{
		LLOG_ERROR("Work::HanderModifyUserPlayCount save not exiest %s", msg->m_user_unioid.c_str());
		return;
	}

	user->m_usert.m_totalplaynum = msg->m_curPlayCount;
	gUserManager.SaveUser(user);
}

void Work::HanderLogicManagerCardExchange(LMsgLMG2CeCardExchange* msg)
{
	if (NULL == msg)
	{
		LLOG_ERROR("Work::HanderLogicManagerCardExchange: message is null.");
		return;
	}

	DUser *user = gUserManager.GetUserByOpenId(msg->m_unionId);

	if (NULL == user)
	{
		LLOG_ERROR("Work::HanderLogicManagerCardExchange: user is error. user uid:%s", msg->m_unionId.c_str());
		return;
	}

	int exchange_card_count(0);

	if(msg->m_cardType==CARD_TYPE_8)
	{
		exchange_card_count = msg->m_cardNum;
	}
	else if(msg->m_cardType==CARD_TYPE_16)
	{
		exchange_card_count = msg->m_cardNum;
	}
	else if (msg->m_cardType == CARD_TYPE_1)
	{
		exchange_card_count = msg->m_cardNum;
	}
	else
	{
		LLOG_ERROR("Work::HanderLogicManagerCardExchange: m_cardType is error,.");
		return;
	}

	switch(msg->m_exchangeType)
	{
	case LMsgLMG2CeCardExchange::ETC_DEL:
		{
			user->m_usert.m_numOfCard2s -= exchange_card_count;
		}
		break;
	case LMsgLMG2CeCardExchange::ETC_ADD:
		{
			user->m_usert.m_numOfCard2s += exchange_card_count;
		}
		break;
	default:
		LLOG_ERROR("Work::HanderLogicManagerCardExchange error msg exchangetype");
		return;
	}

	//容错处理
	user->m_usert.m_numOfCard2s = (user->m_usert.m_numOfCard2s > 0)?user->m_usert.m_numOfCard2s:0;

	if(msg->m_oper == CARDS_OPER_TYPE_INIT)
	{
		user->m_usert.m_new = 1;
	}

	gUserManager.SaveUser(user);

	LMsgL2CeSaveCardInfo charge_msg;
	LCharge& charge   = charge_msg.m_charge;
	charge.m_userId   = user->m_usert.m_id;
	charge.m_time     = gWork.GetCurTime().Secs();
	charge.m_cardNum  = (msg->m_exchangeType == LMsgLMG2CeCardExchange::ETC_ADD) ? exchange_card_count : (0- exchange_card_count);
	charge.m_operType = msg->m_oper;
	charge.m_balance  = user->m_usert.m_numOfCard2s;
	charge.m_operator = "system";
	charge.m_nAppId = user->m_usert.m_nAppId;

	HanderLogicSaveCardInfo(&charge_msg);
}

void Work::HanderLastRelief(LMsgLMG2CeLastRelief* msg)
{
	if (NULL == msg)
	{
		LLOG_ERROR("Work::HanderLastRelief: message is null.");
		return;
	}

	DUser *user = gUserManager.GetUserByOpenId(msg->m_unionId);

	if (NULL == user)
	{
		LLOG_ERROR("Work::HanderLastRelief: user is error. user uid:%s", msg->m_unionId.c_str());
		return;
	}

	user->m_usert.m_last_relief_time = msg->m_last_relief_time;

	gUserManager.SaveUser(user);
}

void Work::HanderPhoneNumber(LMsgLMG2CePhoneNumber* msg)
{
	if (NULL == msg)
	{
		LLOG_ERROR("Work::HanderPhoneNumber: message is null.");
		return;
	}

	DUser *user = gUserManager.GetUserByOpenId(msg->m_unionId);

	if (NULL == user)
	{
		LLOG_ERROR("Work::HanderPhoneNumber: user is error. user uid:%s", msg->m_unionId.c_str());
		return;
	}

	if(user->m_usert.m_phoneNumber.empty())
	{
        user->m_usert.m_phoneNumber = msg->m_phonenumber;
		gUserManager.SaveUser(user);
		LMsgCe2LMGPhoneNumber relogin;
		relogin.m_userid = user->m_usert.m_id;
		relogin.m_errorCode = LMsgCe2LMGPhoneNumber::Error_None;
		SendMsgToLogic(relogin, msg->m_ID);
	}
	else
	{
		LMsgCe2LMGPhoneNumber relogin;
		relogin.m_userid = user->m_usert.m_id;
		relogin.m_errorCode = LMsgCe2LMGPhoneNumber::Error_Exits;
		SendMsgToLogic(relogin, msg->m_ID);
	}
}

void Work::HanderSetFreeSet(std::map<Lstring, Lstring>& param, LSocketPtr sp)
{
	if (param.find("content") == param.end())
	{
		LLOG_ERROR("Work::HanderSetGameFree is null");
		SendRet("{\"errorCode\":6,\"errorMsg\":\"GameFree centent fail\"}", sp);//失败
		return;
	}
	LLOG_ERROR("Work::HanderSetFreeSet %s", param["content"].c_str());

	if (param.find("serverid") == param.end())
	{
		LLOG_ERROR("Work::HanderSetGameFree is null");
		SendRet("{\"errorCode\":6,\"errorMsg\":\"ServerId Null\"}", sp);//失败
		return;
	}

	//m_strActiveFreeSet = param["content"];
	//if (!m_RedisClient.writeDataByString(ActiveFreeSetKeyShanDong, m_strActiveFreeSet))
	//{
	//	LLOG_ERROR("Work::HanderSetFreeSet sets storage Redis failed, please check Redis-server");
	//}

	Lint appId = atoi(param["serverid"].c_str());

	m_mapAppId2StrSet[appId] = param["content"];

	std::stringstream ss;
	for (auto it = m_mapAppId2StrSet.begin(); it != m_mapAppId2StrSet.end(); ++it)
	{
		ss << it->first << "=";
	}
	Lstring newKeyList = ss.str();

	newKeyList = newKeyList.substr(0, newKeyList.length() - 1);
	if (!m_RedisClient.writeDataByString(ActiveAppIdList, newKeyList))
	{
		LLOG_ERROR("Work::HanderSetFreeSet sets storage Redis failed, please check Redis-server");
	}

	std::string strKey = ActiveFreeSetKey + param["serverid"];
	if (!m_RedisClient.writeDataByString(strKey, m_mapAppId2StrSet[appId]))
	{
		LLOG_ERROR("Work::HanderSetFreeSet sets storage Redis failed, please check Redis-server");
	}

	for (auto itLogicM = m_logicClient.begin(); itLogicM != m_logicClient.end(); itLogicM++)
	{
		int nAppId = 0;

		Json::Reader reader(Json::Features::strictMode());
		Json::Value  value;
		if (!reader.parse(param["content"], value))
		{
			LLOG_ERROR("MatchRewardsManager::LoadRewardsConfig json error %s", reader.getFormatedErrorMessages().c_str());
			SendRet("{\"errorCode\":6,\"errorMsg\":\"content json Error\"}", sp);//失败
			return;
		}

		if (!value["ServerID"].isNull() && value["ServerID"].isString())
		{
			nAppId = atoi(value["ServerID"].asCString());
		}
		else
		{
			LLOG_ERROR("Work::HanderSetGameFree json error");
			SendRet("{\"errorCode\":6,\"errorMsg\":\"GameFree AppID Error\"}", sp);//失败
			return;
		}

		if (!IsRightAppID(nAppId))
		{
			LLOG_ERROR("Work::HanderSetGameFree is null");
			SendRet("{\"errorCode\":6,\"errorMsg\":\"GameFree AppID Error\"}", sp);//失败
			return;
		}

		LMsgCE2LSetGameFree free;
		free.m_ServerID = itLogicM->first;
		free.m_nAppId = nAppId;
		free.m_strFreeSet = m_mapAppId2StrSet[appId];
		itLogicM->second->Send(free.GetSendBuff());
	}

	SendRet("{\"errorCode\":0,\"errorMsg\":\"success\"}", sp);//成功
}

void Work::HanderActivityDrawSet(std::map<Lstring, Lstring>& param, LSocketPtr sp)
{
	if (param.find("content") == param.end())
	{
		LLOG_ERROR("Work::HanderActivityShareSet is null");
		SendRet("{\"errorCode\":6,\"errorMsg\":\"DrawActive content fail\"}", sp);//失败
		return;
	}
	LLOG_ERROR("Work::HanderActivityDrawSet %s", param["content"].c_str());

	Lstring strSet = param["content"];

	if (param["serverid"].empty())
	{
		for (auto itLogicM = m_logicClient.begin(); itLogicM != m_logicClient.end(); itLogicM++)
		{
			LMsgCE2LSetActivity active;
			active.m_ServerID = itLogicM->first;
			active.m_strActivity = strSet;
			active.m_activityId = ActivityID_Draw;
			itLogicM->second->Send(active.GetSendBuff());
		}
	}
	else
	{
		Lint nServerID = atoi(param["serverid"].c_str());
		LMsgCE2LSetActivity active;
		active.m_ServerID = nServerID;
		active.m_strActivity = strSet;
		active.m_activityId = ActivityID_Draw;
		SendMsgToLogic(active, nServerID);
	}

	SendRet("{\"errorCode\":0,\"errorMsg\":\"success\"}", sp);//成功
}

bool Work::_initRedisAndActive()
{
	if (m_RedisClient.initRedis(gConfig.GetRedisIp(), gConfig.GetRedisPort()))
	{
		Lstring strAppIdList;
		if (!m_RedisClient.readDataByString(ActiveAppIdList, strAppIdList))   //先读取appid： 1001=1002=1003
		{
			LLOG_ERROR("Work::_initRedisAndActive ActiveFreeSet get from redis is failed, now set is empty");
		}
		else
		{
			std::vector<Lstring> strFreeSetAppIdList;
			L_ParseString(strAppIdList, strFreeSetAppIdList, "=");
			for (auto it = strFreeSetAppIdList.begin(); it != strFreeSetAppIdList.end(); ++it)
			{
				Lstring strKey = ActiveFreeSetKey + *it;      //eg: ActiveFreeSet1001
				Lstring strSet = "";
				if (m_RedisClient.readDataByString(strKey, strSet))
				{
					Lint appId = atoi((*it).c_str());
					m_mapAppId2StrSet[appId] = strSet;
				}
			}
			LLOG_ERROR("ActiveFreeSet AppIdList is %s", strAppIdList.c_str());
		}

		return true;
	}
	else
	{
		//m_strActiveFreeSet = "";
		//m_strHorse = "";
		return false;
	}
}

void Work::SendActiveSetToLM(Lint LMid)
{
	for (auto it = m_mapAppId2StrSet.begin(); it != m_mapAppId2StrSet.end(); ++it)
	{
		// 限时免费活动
		LMsgCE2LSetGameFree FreeActive;
		FreeActive.m_ServerID = it->first;
		FreeActive.m_strFreeSet = it->second;
		SendMsgToLogic(FreeActive, LMid);
	}

	//跑马灯信息
	//LMsgCe2LGMHorse horse;
	//horse.m_str = m_strHorse;
	//SendMsgToLogic(horse, LMid);

	//// 牌型活动
	//LMsgCE2LSetPXActive PXActive;
	//PXActive.m_ServerID = LMid;
	//PXActive.m_strActiveSet = m_strActivePXSet;
	//SendMsgToLogic(PXActive, LMid);

	//// 老用户登录送房卡活动
	//LMsgCE2LSetOUGCActive OUGCActive;
	//OUGCActive.m_ServerID = LMid;
	//OUGCActive.m_strActiveSet = m_strActiveOUGCSet;
	//SendMsgToLogic(OUGCActive, LMid);
}

void Work::HanderFenXiang(LMsgLMG2CeFenXiang* msg)
{
	if (NULL == msg)
	{
		LLOG_ERROR("Work::HanderPhoneNumber: message is null.");
		return;
	}

	DUser *user = gUserManager.GetUserByOpenId(msg->m_unionId);
	if(NULL == user)
	{
		return ;
	}

	int nServerID = msg->m_ID;
	LMsgCe2LMGFenXiang send;
	ActivityFenXiangInfo* pInfo =gUserManager.GetActivityFenXiangInfo(user->m_usert.m_id);
	if(NULL == pInfo)
	{
		pInfo = new ActivityFenXiangInfo();
		pInfo->m_userId = user->m_usert.m_id;
		if(!gUserManager.InstertFenXiangInfo(pInfo))
		{
			send.m_errorCode = 3;
			SendMsgToLogic(send, nServerID);
			return ;
		}

	}
	send.m_userid = user->m_usert.m_id;

	LTime lastFetchTimer;
	lastFetchTimer.SetSecs(pInfo->m_lastFetchTime);
	//清除数据
	if(!IsSameDay(lastFetchTimer,GetCurTime()))
	{
		if(pInfo->m_fetchCnt>0)
		{
			pInfo->m_fetchCnt = 0;
			gUserManager.SaveFenXiangInfo(pInfo);
		}
	}

	if(pInfo->m_fetchCnt>0)
	{
		send.m_errorCode = 1;
		SendMsgToLogic(send, nServerID);
		return;
	}

	pInfo->m_fetchCnt = 1;
	pInfo->m_lastFetchTime = GetCurTime().Secs();

	gUserManager.SaveFenXiangInfo(pInfo);
	//代表分享领取成功，增加房卡在logicManager操作，先这样吧，时间紧

	send.m_errorCode = 0;
	SendMsgToLogic(send,nServerID);

	return;
}

bool Work::IsSameDay(LTime t1,LTime t2)
{
	if (t1.GetYear() == t2.GetYear()&& t1.GetYday() == t2.GetYday())
		return true;

	return false;
}

void Work::SendMsgToAllLogicManager(LMsg& msg)
{
	for(std::map<Lint,LSocketPtr>::iterator iter = m_logicClient.begin();iter!=m_logicClient.end();iter++)
	{
		iter->second->Send(msg.GetSendBuff());
	}
}

void Work::HanderActivityRedPackets(std::map<Lstring, Lstring>& param, LSocketPtr sp)
{
	if (param.find("content") == param.end())
	{
		LLOG_ERROR("Work::HanderActivityRedPacketsSet is null");
		SendRet("{\"errorCode\":6,\"errorMsg\":\"DrawActive content fail\"}", sp);//失败
		return;
	}
	LLOG_ERROR("Work::HanderActivityShareSet %s", param["content"].c_str());

	Lstring strShareSet = param["content"];

	if (param["serverid"].empty())
	{
		
		for (auto itLogicM = m_logicClient.begin(); itLogicM != m_logicClient.end(); itLogicM++)
		{
			LMsgCE2LSetActivity active;
			active.m_ServerID = itLogicM->first;
			active.m_strActivity = strShareSet;
			active.m_activityId = ActivityID_Invite;
			itLogicM->second->Send(active.GetSendBuff());
		}
	}
	else
	{
		Lint nServerID = atoi(param["serverid"].c_str());
		LMsgCE2LSetActivity active;
		active.m_ServerID = nServerID;
		active.m_strActivity = strShareSet;
		active.m_activityId = ActivityID_Invite;
		SendMsgToLogic(active, nServerID);
	}

	SendRet("{\"errorCode\":0,\"errorMsg\":\"success\"}", sp);//成功
}

bool Work::IsRightAppID(Lint appId)
{
	return gConfig.IsRightAppID(appId);
}

void Work::HanderGMPolicyBlock(std::map<Lstring, Lstring>& param, LSocketPtr sp)
{
	if (param.find("userId") == param.end() || param.find("optType") == param.end())
	{
		SendRet("{\"errorCode\":1,\"errorMsg\":\"param error\"}", sp);//参数错误
		return;
	}

	LLOG_ERROR("Work::HanderGMPolicyBlock userId=[%s] optType=[%s]", param["userId"].c_str(), param["optType"].c_str());

	Lint userId = atoi(param["userId"].c_str());
	DUser* user = gUserManager.GetUserById(userId);
	if (!user)
	{
		SendRet("{\"errorCode\":1,\"errorMsg\":\"user not exiest\"}", sp);//玩家不存在
		return;
	}

	Lint optType = atoi(param["optType"].c_str());
	Lint startTime = param.count("startTime") > 0 ? atoi(param["startTime"].c_str()) : 0;
	Lint endTime = param.count("endTime") > 0 ? atoi(param["endTime"].c_str()) : -1;

	PolicyBlock(user, optType, startTime, endTime, true);

	SendRet("{\"errorCode\":0,\"errorMsg\":\"success\"}", sp);//成功
}

void Work::HanderLogicRestBlockUser(LMsgLMGResetBlockUser* msg)
{
	if (nullptr == msg)
	{
		return;
	}

	for (auto it = msg->m_unblockedUsers.begin(); it != msg->m_unblockedUsers.end(); it++)
	{
		DUser* user = gUserManager.GetUserById(*it);
		if (user)
		{
			PolicyBlock(user, 2, 0, 0, false);
		}
	}
}

void Work::PolicyBlock(DUser* user, Lint optType, Lint startTime, Lint endTime, bool sendLMG)
{
	LLOG_INFO("userid=%d opt=%d start=%d end=%d", user->m_usert.m_id, optType, startTime, endTime);
	if (optType == 0)
	{
		user->m_usert.m_blockStartTime = startTime;
		user->m_usert.m_blockEndTime = endTime;

		// 封禁用户
		std::stringstream ss;
		ss << "INSERT INTO block_user(UserID, StartTime, EndTime) VALUES(";
		ss << "'" << user->m_usert.m_id << "',";
		ss << "'" << startTime << "',";
		ss << "'" << endTime << "') ON DUPLICATE KEY UPDATE ";
		ss << "StartTime='" << startTime << "', ";
		ss << "EndTime='" << endTime << "';";

		LLOG_INFO("block user sql =%s", ss.str().c_str());

		Lstring* sql = new Lstring(ss.str());
		gDbServer.Push(sql);
	}
	if (optType == 2)
	{
		// 解封用户
		user->m_usert.m_blockStartTime = 0;
		user->m_usert.m_blockEndTime = 0;

		std::stringstream ss;
		ss << "DELETE FROM block_user WHERE UserID=";
		ss << "'" << user->m_usert.m_id << "'";

		LLOG_DEBUG("reset block user sql =%s", ss.str().c_str());

		Lstring* sql = new Lstring(ss.str());
		gDbServer.Push(sql);
	}

	if (sendLMG)
	{
		// 通知LogicManager
		LMsgCE2LBlockUser msg;
		msg.m_strUUID = user->m_usert.m_unioid;
		msg.m_userId = user->m_usert.m_id;
		msg.m_optType = optType;
		msg.m_blockStartTime = startTime;
		msg.m_blockEndTime = endTime;
		SendMsgToAllLogicManager(msg);
	}
}

void Work::HanderDeviceCheckRequest(LMsgDeviceCheckRequest* msg)
{
	if (msg && msg->m_verifyType == 1 && gConfig.GetDeviceVerify() && !gConfig.GetDeviceVerifyUrl().empty())
	{
		if (msg->m_deviceId.empty())
		{
			LLOG_ERROR("No device id, skip device login check %d", msg->m_userId);
			return;
		}
		Lstring afterBase64;
		if (!Base64::Base64Encode(msg->m_deviceId, afterBase64))
		{
			afterBase64 = msg->m_deviceId;
		}
		// 验证设备是否允许登录，这里只发给后台即可，如果设备被封会调用游戏接口
		std::map<Lstring, Lstring> param;
		param.insert(std::make_pair("gameId", toString(msg->m_serverId)));
		param.insert(std::make_pair("userId", toString(msg->m_userId)));
		param.insert(std::make_pair("deviceType", toString(msg->m_deviceType)));
		param.insert(std::make_pair("deviceId", afterBase64));
		param.insert(std::make_pair("loginIp", msg->m_loginIp));

		Lstring strParam = _makeParam(param);

		std::stringstream ss;
		ss << gConfig.GetDeviceVerifyUrl();
		ss << "/oauth/check/login?";
		ss << strParam;

		Lstring strUrl = ss.str();
		bool bIsHttps = false;
		if (strUrl.find("https") != std::string::npos)
		{
			bIsHttps = true;
		}

		Lstring httpRet;
		if (!HttpRequest::Instance().HttpGet(strUrl, httpRet, bIsHttps))
		{
			LLOG_ERROR("http request error %s", strUrl.c_str());
			return;
		}

		// 读取结果值
		Json::Reader reader;
		Json::Value value;
		if (!reader.parse(httpRet, value))
		{
			LLOG_ERROR("parsr json  error %s", httpRet.c_str());
			return;
		}

		/*{
		"meta": {
		"message": "操作成功",
		"code" : "2000"
		}
		}*/
		if (!value["meta"].isNull())
		{
			Json::Value meta = value["meta"];
			// 正常
			if (!meta["code"].isNull() && meta["code"].asString() == "2000")
			{
				;
			}
			else
			{
				LLOG_ERROR("http server process error %s", httpRet.c_str());
			}
		}
	}
}

Lstring Work::_makeParam(std::map<Lstring, Lstring>& param)
{
	// key1=value1&key2=value2
	std::vector<std::string> keys;
	for (std::map<std::string, std::string>::const_iterator it = param.begin(); it != param.end(); ++it)
	{
		const std::string& strKey = it->first;
		if (strKey.compare("sign") != 0 && !it->second.empty())
			keys.push_back(strKey);
	}
	std::sort(keys.begin(), keys.end());
	std::string beforeSign;
	for (size_t i = 0; i < keys.size(); ++i)
	{
		if (!beforeSign.empty())
		{
			beforeSign += "&";
		}
		beforeSign += keys[i] + "=" + param[keys[i]];
	}
	Lstring strParam = beforeSign;
	if (!beforeSign.empty())
	{
		beforeSign += "&";
	}
	beforeSign += "key=B1081EE774CE54FB6CAC6075009729183c6e0b8a9c15224a8228b9a98ca1531d";
	std::string sign = HttpRequest::Instance().Md5Hex(beforeSign.c_str(), beforeSign.length());
	std::transform(sign.begin(), sign.end(), sign.begin(), (int(*)(int))std::toupper);

	if (!strParam.empty())
	{
		strParam += "&";
	}
	strParam += "sign=" + sign;
	return strParam;
}

Lint Work::_checkDeviceWhenReg(Lstring deviceId, Lstring& errMsg)
{
	if (!gConfig.GetDeviceVerify() || gConfig.GetDeviceVerifyUrl().empty())
	{
		return 2000;
	}
	if (deviceId.empty())
	{
		LLOG_ERROR("No device id, skip device check");
		return 2000;
	}
	// 验证设备是否允许注册，同步等待验证结果
	std::map<Lstring, Lstring> param;
	Lstring afterBase64;
	if (!Base64::Base64Encode(deviceId, afterBase64))
	{
		afterBase64 = deviceId;
	}
	param.insert(std::make_pair("deviceId", afterBase64));

	Lstring strParam = _makeParam(param);

	std::stringstream ss;
	ss << gConfig.GetDeviceVerifyUrl();
	ss << "/oauth/check/regist?";
	ss << strParam;

	Lint iRet = 2000;
	do
	{
		Lstring strUrl = ss.str();
		bool bIsHttps = false;
		if (strUrl.find("https") != std::string::npos)
		{
			bIsHttps = true;
		}

		Lstring httpRet;
		if (!HttpRequest::Instance().HttpGet(strUrl, httpRet, bIsHttps))
		{
			LLOG_ERROR("http request error %s", strUrl.c_str());
			break;
		}

		// 读取结果值
		Json::Reader reader;
		Json::Value value;
		if (!reader.parse(httpRet, value))
		{
			LLOG_ERROR("parser json  error %s", httpRet.c_str());
			break;
		}

		/* {
		"meta": {
		"message": "由于您注册账号过于频繁，请24小时后再来",
		"code" : "8001"
		}
		}*/
		if (!value["meta"].isNull())
		{
			Json::Value meta = value["meta"];
			// 正常
			if (!meta["code"].isNull())
			{
				Lstring code = meta["code"].asString();
				iRet = atoi(code.c_str());
				errMsg = !meta["message"].isNull() ? meta["message"].asString() : "";
			}
			else
			{
				LLOG_ERROR("http server process error %s", httpRet.c_str());
				break;
			}
		}
	} while (false);

	return iRet;
}

template<typename T> string Work::toString(const T& t)
{
	ostringstream oss;  //创建一个格式化输出流
	oss << t;             //把值传递如流中
	return oss.str();   
}
