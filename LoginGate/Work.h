#ifndef _WORK_H_
#define _WORK_H_

#include <map>
#include "LRunnable.h"
#include "LSingleton.h"
#include "LTime.h"
#include "LMsg.h"
#include "LMsgS2S.h"
#include "LSocket.h"
#include "TCPClient.h"
#include "GarbageCollectionService.h"
#include "WeiXinAuthService.h"
#include <boost/uuid/random_generator.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/uuid/uuid_io.hpp>

using namespace std;

struct CLIENT_INFO
{
	Llong idOnGate;
	Lint  time;
	std::string m_token;
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

private:
	void			Tick(LTime& cur);
	LTime&			GetCurTime();

private:
	void			HanderMsg(LMsg* msg);
	void			HanderClientIn(LMsgIn* msg);
	void			HanderUserKick(LMsgKick* msg);
	void			HanderUserLogin(LMsgC2SMsg* msg);
	void			HanderCEWrappedMsg(LMsgLGCEUserMsgWrapper* msg);
	void			HanderLoginToken(LMsgC2SLoginToken *msg);

private:
	void			AddSp(LSocketPtr sp);
	void			DelSp(LSocketPtr sp);
	void			ConnectToCenter(void);
	void			CheckTimeout(LTime& cur);
	bool			CheckMd5Hex(const LMsgC2SMsg* msg);
	void			SendLoginToken(LSocketPtr client_sp);


public:
	void			SendToCenter(LMsg& msg);

private:
	LTime			m_tickTimer;

	Llong			m_30MSecTick;//30毫秒循环一次

	Llong			m_200MSecTick;//200毫秒循环一次

	Llong			m_1SecTick;//1秒循环一次

	Llong			m_5SecTick;	//5秒循环一次

	Llong			m_30SecTick;//30秒循环一次

	Llong			m_600SecTick;//5分钟秒循环一次

private:
	TCPClientPtr						m_centerClient;
	std::map<Llong, LSocketPtr>			m_mapId2Client;
	std::map<LSocketPtr, CLIENT_INFO>	m_mapClientInfo;
	Llong								m_allocClientId;

	GarbageCollectionService m_gcService;
	WeiXinAuthService m_wxAuthService;

	boost::uuids::random_generator		m_secret_key;
};

#define gWork Work::Instance()

#endif