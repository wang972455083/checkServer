#ifndef _CONFIG_H_
#define _CONFIG_H_


#include "LIni.h"
#include "LSingleton.h"

class Config :public LSingleton<Config>
{
public:
	virtual	bool	Init();

	virtual	bool	Final();

	Lint	GetId();

	Lint	GetLogLevel();

	Lstring	GetDbHost();
	Lstring	GetDbUser();
	Lstring	GetDbPass();
	Lstring	GetDbName();
	Lshort	GetDbPort();


	Lstring GetRedisIp();
	Lshort  GetRedisPort();

	Lstring	GetOutsideIp();
	Lshort	GetOutsidePort();


public:
	Lstring GetLogicManagerIp();
	Lshort GetLogicManagerPort();

private:
	LIniConfig	m_ini;


private:
	Lshort		m_inPort;
	Lshort		m_outPort;

	Lstring		m_inIp;
	Lstring		m_outIp;

	
};

#define gConfig Config::Instance()

#endif