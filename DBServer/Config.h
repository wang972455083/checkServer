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

	Lstring GetLogicDBIp();
	Lshort	GetLogicDBPort();
	Lstring GetLogicDBKey();


	Lstring	GetDbHost();
	Lstring	GetDbUser();
	Lstring	GetDbPass();
	Lstring	GetDbName();
	Lshort	GetDbPort();
	Lint	GetWorkerNum();

	Lstring GetRedisIp();
	Lshort	GetRedisPort();

	//提供给后台验证红包使用
	Lstring GetGMIp();
	Lshort  GetGMPot();

	Lstring GetRedPacketUrl();
private:
	LIniConfig	m_ini;
};

#define gConfig Config::Instance()

#endif