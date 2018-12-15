#include "LLog.h"
#include "DbServer.h"
#include "Config.h"

DBSession::DBSession()
	:m_Port(0),m_Mysql(NULL),m_use(false)
{

}

DBSession::~DBSession()
{
	if(m_Mysql)
	{
		mysql_close(m_Mysql);
	}
}

bool DBSession::Init(std::string host,std::string user,std::string pass,std::string dbname,std::string character,Lshort port)
{
	m_Host = host;
	m_User = user;
	m_Pass = pass;
	m_DBName = dbname;
	m_Port = port;
	m_Character = character;

	//初始化mysql
	if((m_Mysql = mysql_init(NULL)) == 0)
	{
		LLOG_ERROR("mysql_init() error");
		return false;
	}

	//设置mysql自动重连
	char value = 1;
	if(mysql_options(m_Mysql, MYSQL_OPT_RECONNECT, (char*)&value) != 0)
	{
		LLOG_ERROR("mysql_options error %s",mysql_error(m_Mysql));
		return false;
	}

	//连接数据库
	if(mysql_real_connect(m_Mysql,m_Host.c_str(),m_User.c_str(),m_Pass.c_str(),dbname.c_str(),m_Port,NULL,0) == 0)
	{
		LLOG_ERROR("mysql_real_connect error %s",mysql_error(m_Mysql));
		return false;
	}

	//设置mysql字符集
	if(mysql_set_character_set(m_Mysql, m_Character.c_str()) != 0)
	{
		LLOG_ERROR("mysql_set_character_set error %s",mysql_error(m_Mysql));
		return false;
	}

	return true;
}

MYSQL* DBSession::GetMysql()
{
	try
	{
		mysql_ping(m_Mysql);
	}
	catch (...)
	{
		return NULL;
	}

	return m_Mysql;
}

void DBSession::SetUse(bool use)
{
	m_use = use;
}

bool DBSession::GetUse()
{
	return m_use;
}

//////////////////////////////////////////////////////////////////////////
bool DbServer::Init()
{
	boost::mutex::scoped_lock(m_mut);
	for(Lint i = 0 ; i < 1; ++i)
	{
		DBSession* s = new DBSession();
		if(!s->Init(gConfig.GetDbHost(),gConfig.GetDbUser(),gConfig.GetDbPass(),gConfig.GetDbName(),"utf8mb4",gConfig.GetDbPort()))
		{
			delete s;
			return false;
		}
		m_sessionList.push_back(s);
	}
	return true;
}

bool DbServer::Final()
{
	boost::mutex::scoped_lock(m_mut);
	for(Lsize i = 0 ; i < m_sessionList.size(); ++i)
	{
		delete m_sessionList[i];
	}
	return true;
}

void DbServer::Clear()
{

}

void DbServer::Run()
{
	while(!GetStop())
	{
		Lstring* str = (Lstring*)Pop();
		if(str == NULL)
		{
			Sleep(1);
			continue;
		}
		else
		{
			Excute(*str);
			delete str;
		}
	}
}

void DbServer::Excute(std::string& str)
{
	DbTool t;
	MYSQL* mysql = t.GetSession();
	if(mysql == NULL)
	{
		LLOG_ERROR("DbServer::Excute MYSQL NULL");
		return ;
	}
	if(mysql_real_query(mysql,str.c_str(),str.size()))
	{
		LLOG_ERROR("DbServer::Excute error = %s,sql = %s ", mysql_error(mysql),str.c_str());
	}
}

DBSession* DbServer::GetSession()
{
	boost::mutex::scoped_lock(m_mut);
	for(Lsize i = 0 ; i < m_sessionList.size(); ++i)
	{
		if(!m_sessionList[i]->GetUse())
		{
			m_sessionList[i]->SetUse(true);
			return m_sessionList[i];
		}
	}

	return NULL;
}

void DbServer::PutSession(DBSession* s)
{
	boost::mutex::scoped_lock(m_mut);
	s->SetUse(false);
}


DbTool::DbTool()
{
	m_s = gDbServer.GetSession();
}

DbTool::~DbTool()
{
	if(m_s)
	{
		gDbServer.PutSession(m_s);
	}
}

MYSQL* DbTool::GetSession()
{
	if(m_s)
	{
		return m_s->GetMysql();
	}

	return NULL;
}