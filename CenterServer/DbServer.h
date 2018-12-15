#ifndef _DB_SERVER_H_
#define _DB_SERVER_H_

#include "LBase.h"
#include "LSingleton.h"
#include "LRunnable.h"

class DBSession
{
public:
	DBSession();
	~DBSession();

	bool Init(Lstring host,Lstring user,Lstring pass,Lstring dbname,Lstring _Character,Lshort port);

	MYSQL* GetMysql();

	bool GetUse();
	void SetUse(bool use);

private:
	Lstring m_Host;
	Lstring m_User;
	Lstring m_Pass;
	Lstring m_DBName;
	Lstring m_Character;
	Lshort  m_Port;
	MYSQL*  m_Mysql;
	bool	m_use;
};


//db服务器
class DbServer:public LRunnable,public LSingleton<DbServer>
{
public:
	virtual bool		Init();
	virtual bool		Final();

	virtual void		Run();
	virtual void		Clear();

	void				Excute(std::string& sql);

	//获取session
	DBSession*			GetSession();

	//回收session
	void				PutSession(DBSession*);

private:
	boost::mutex m_mut;
	std::vector<DBSession*> m_sessionList;
};

//获取dbsession的工具类
class DbTool
{
public:
	DbTool();
	~DbTool();

	MYSQL* GetSession();
private:
	DBSession* m_s;
};

#define gDbServer DbServer::Instance()

#endif