#include "Config.h"
#include "LLog.h"
#include "LTool.h"


bool Config::Init()
{
	m_ini.LoadFile("game_server_config.ini");



	return true;
}

bool Config::Final()
{
	return true;
}

Lint Config::GetLogLevel()
{
	return m_ini.GetInt("LogLevel", LLOG_LEVEL_DEBUG);
}

Lstring	Config::GetDbHost()
{
	return m_ini.GetString("DbHost", "192.168.1.139");
}

Lstring	Config::GetDbUser()
{
	return m_ini.GetString("DbUser", "root");
}

Lstring	Config::GetDbPass()
{
	return m_ini.GetString("DbPass", "");
}

Lstring	Config::GetDbName()
{
	return m_ini.GetString("DbName", "test");
}

Lshort	Config::GetDbPort()
{
	return m_ini.GetShort("DbPort", 3306);
}


Lint Config::GetId()
{
	return m_ini.GetInt("Id", 1);
}

Lstring	Config::GetOutsideIp()
{
	return m_ini.GetString("OutsideIp", "127.0.0.1");
}
Lshort	Config::GetOutsidePort()
{
	return m_ini.GetShort("OutsidePort", 2001);
}



Lstring Config::GetRedisIp()
{
	return m_ini.GetString("RedisIp", "127.0.0.1");
}

Lshort Config::GetRedisPort()
{
	return m_ini.GetShort("RedisPort", 6379);
}

Lstring Config::GetLogicManagerIp()
{
	return m_ini.GetString("LogicManagerIp", "127.0.0.1");
}

Lshort Config::GetLogicManagerPort()
{
	return m_ini.GetShort("LogicManagerPort", 9001);
}



