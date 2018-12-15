#include "Config.h"
#include "LLog.h"

bool Config::Init()
{
	m_ini.LoadFile("logic_db_config.ini");
	return true;
}

bool Config::Final()
{
	return true;
}

Lint Config::GetLogLevel()
{
	//Ä¬ÈÏdebug log
	return m_ini.GetInt("LogLevel", LLOG_LEVEL_DEBUG);
}

Lstring Config::GetLogicDBIp()
{
	return m_ini.GetString("LogicDBIp", "127.0.0.1");
}

Lshort Config::GetLogicDBPort()
{
	return m_ini.GetShort("LogicDBPort", 7001);
}

Lstring Config::GetLogicDBKey()
{
	return m_ini.GetString("LogicDBKey", "45fcbdcbfbaa1a2f7a70df0a2a766c1d");
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

Lint Config::GetWorkerNum()
{
	return m_ini.GetInt("WorkerNum", 4);
}

Lstring Config::GetRedisIp()
{
	return m_ini.GetString("RedisIp", "192.168.10.122");
}

Lshort Config::GetRedisPort()
{
	return m_ini.GetShort("RedisPort", 6379);
}

Lstring Config::GetGMIp()
{
	return m_ini.GetString("GMIp", "127.0.0.1");
}

Lshort  Config::GetGMPot()
{
	return m_ini.GetShort("GMPort", 9001);
}

Lstring Config::GetRedPacketUrl()
{
	return m_ini.GetString("ActivityRedRequestUrl", "http://172.16.70.25:7780/xlhy-activity/");
}