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

	Lstring GetCenterIp();
	Lshort GetCenterPort();
	Lstring GetCenterKey();


	Lstring GetDBIp();
	Lshort GetDBPort();
	Lstring GetDBKey();


	Lstring GetInsideIp();
	Lshort GetInsidePort();
	bool	GetFree();
	void	SetFree(Lint free);
	bool	GetDebugModel();
	Lint	GetServerID();
	Lstring	GetServerName();
	Lint	GetRewardCardCount();

	bool	IsIOSOpenPay();
	bool	IsAndroidOpenPay();

public://划分玩法
	bool IsSlipTrio();

public:
	void ReloadConfig();

public://remote log
	Lstring GetRemoteLogUrl();
	Lstring GetRemoteLogCacheFileName();
	Lint GetMaxCachedLogSize();
	Lshort GetMaxCachedLogNum();
	Lint GetMaxFileLogSize();
	Lint GetRemoteLogThreadStart();

private:
	LIniConfig	m_ini;


private:
	Lshort		m_inPort;
	Lshort		m_outPort;

	Lstring		m_inIp;
	Lstring		m_outIp;
	Lint		m_free;
	Lint		m_DebugMod;
	Lint		m_ServerID;						//服务器ID

private:
	Lint		m_slip_trio;		//是否将三人麻将滑到专属服务器
	Lint		m_ios_open_pay;		//是否开启苹果支付
	Lint		m_android_open_pay;	//是否开启安卓支付

};

#define gConfig Config::Instance()

#endif