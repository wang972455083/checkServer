#ifndef _CONFIG_H_
#define _CONFIG_H_


#include "LIni.h"
#include "LSingleton.h"

class Config :public LSingleton<Config>
{
public:
	virtual	bool	Init();

	virtual	bool	Final();


	Lint	GetLogLevel();

	Lstring	GetDbHost();
	Lstring	GetDbUser();
	Lstring	GetDbPass();
	Lstring	GetDbName();
	Lshort	GetDbPort();

	Lstring	GetInsideIp();
	Lshort	GetInsidePort();

	Lstring GetRedisIp();
	Lshort  GetRedisPort();

	Lstring	GetOutsideIp();
	Lshort	GetOutsidePort();
	Lstring	GetLocalLogin();//±¾µØµÇÂ¼

	Lstring	GetCenterKey();
	bool	GetWxAuthSwitch();
public://remote log
	Lstring GetRemoteLogUrl();
	Lstring GetRemoteLogCacheFileName();
	Lint GetMaxCachedLogSize();
	Lshort GetMaxCachedLogNum();
	Lint GetMaxFileLogSize();
	Lint GetRemoteLogThreadStart();

	Lstring GetRedPacketBindUrl();

	bool	IsRightAppID(Lint appId);

	void	UpdateAppIds();
	void    ReloadAppIds();

	Lstring GetAppServerName(Lint appId);
	Lshort  GetDeviceVerify();
	Lstring GetDeviceVerifyUrl();

private:
	LIniConfig	m_ini;


private:
	Lshort		m_inPort;
	Lshort		m_outPort;

	Lstring		m_inIp;
	Lstring		m_outIp;

	std::map<Lint,Lstring>	m_AppIds;
};

#define gConfig Config::Instance()

#endif