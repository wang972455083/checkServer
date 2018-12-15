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

	Lstring	GetTeamIp();
	Lshort	GetTeamPort();

	Lstring	GetLogicManagerIp();
	Lshort	GetLogicManagerPort();
	Lstring GetLogicKey();

	Lstring	GetOutsideIp();
	Lshort	GetOutsidePort();

	Lint	GetMsgFrequency();

private:
	LIniConfig	m_ini;


private:
	Lshort		m_inPort;
	Lshort		m_outPort;

	Lstring		m_inIp;
	Lstring		m_outIp;

	Lint		m_msgFreq;
};


#define gConfig Config::Instance()

#endif