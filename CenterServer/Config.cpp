#include "Config.h"
#include "LLog.h"
#include "LTool.h"


bool Config::Init()
{
	m_ini.LoadFile("center_config.ini");

	UpdateAppIds();

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


Lstring	Config::GetInsideIp()
{
	return m_ini.GetString("InsideIp","127.0.0.1");
}

Lshort Config::GetInsidePort()
{
	return m_ini.GetShort("InsidePort",9001);
}

Lstring	Config::GetOutsideIp()
{
	return m_ini.GetString("OutsideIp", "127.0.0.1");
}
Lshort	Config::GetOutsidePort()
{
	return m_ini.GetShort("OutsidePort", 2001);
}

Lstring Config::GetLocalLogin()
{
	return m_ini.GetString("LocalLogin", "false");
}

Lstring	Config::GetCenterKey()
{
	return m_ini.GetString("CenterKey", "45fcbdcbfbaa1a2f7a70df0a2a766c1d");
}

bool Config::GetWxAuthSwitch()
{
	return 1 == m_ini.GetInt("WxAuth", 0);
}

Lstring Config::GetRemoteLogUrl()
{
	return m_ini.GetString("RemoteLogUrl", "");
}

Lstring Config::GetRemoteLogCacheFileName()
{
	return m_ini.GetString("RemoteLogCacheFileName", "/tmp/rlog_cache.log");
}

Lint Config::GetMaxCachedLogSize()
{
	// default value: 20 * 1024
	return m_ini.GetInt("MaxCachedLogSize", 20480);
}

Lshort Config::GetMaxCachedLogNum()
{
	// default value: 100
	return m_ini.GetShort("MaxCachedLogNum", 100);
}

Lint Config::GetMaxFileLogSize()
{
	// default value: 10 * 1024 * 1024
	return m_ini.GetInt("MaxFileLogSize", 10485760);
}

Lint Config::GetRemoteLogThreadStart()
{
	return m_ini.GetInt("RemoteLogThreadStart", 0);
}

Lstring Config::GetRedisIp()
{
	return m_ini.GetString("RedisIp", "127.0.0.1");
}

Lshort Config::GetRedisPort()
{
	return m_ini.GetShort("RedisPort", 6379);
}

Lstring Config::GetRedPacketBindUrl()
{
	return m_ini.GetString("ActivityRedBindUrl", "http://172.16.70.25:7780/xlhy-activity/");
}

bool Config::IsRightAppID(Lint appId)
{
	std::map<Lint,Lstring>::iterator iter = m_AppIds.find(appId);
	if(iter != m_AppIds.end())
		return true;

	return false;
}

void Config::UpdateAppIds()
{
	Lstring strAppIds = m_ini.GetString("AppIDs");
	LLOG_DEBUG("Config AppIDs:%s", strAppIds.c_str());
	std::map<Lstring,Lstring> AppIds;
	L_ParseString(strAppIds,AppIds,"|",":");

	m_AppIds.clear();
	for(auto it=AppIds.begin();it!=AppIds.end();++it)
	{
		m_AppIds.insert(std::pair<Lint,Lstring>(atoi(it->first.c_str()),it->second));
		LLOG_DEBUG("Config Data:%s_%s", it->first.c_str(),it->second.c_str());
	}
}

void Config::ReloadAppIds()
{
	Init();
	UpdateAppIds();
}

Lstring Config::GetAppServerName(Lint appId)
{
	auto it = m_AppIds.find(appId);
	if(it != m_AppIds.end())
		return it->second;

	return "";
}

Lshort Config::GetDeviceVerify()
{
	return m_ini.GetShort("NeedDeviceCheck", 0);
}

Lstring Config::GetDeviceVerifyUrl()
{
	return m_ini.GetString("DeviceCheckURL", "");
}
