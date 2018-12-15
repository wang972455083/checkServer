#include "LRedisClient.h"
#include "LLog.h"
#include "BaseCommonFunc.h"

CRedisClient::CRedisClient()
{
	m_bInit = false;
	m_uRedisPort = 0;
	m_strRedisIp = "";
}

CRedisClient::~CRedisClient()
{

}

bool CRedisClient::initRedis(std::string strRedisIp, unsigned short uRedisPort)
{
	if(m_bInit)
	{
		return true;
	}

	if(strRedisIp.empty())
	{
		LLOG_ERROR("Redis ip is empty");
		return false;
	}

	m_strRedisIp = strRedisIp;
	m_uRedisPort = uRedisPort;

	m_bInit = _connectRedis();

	return m_bInit;
}

bool CRedisClient::writeDataByString(const std::string& strKey, const std::string& strWrite, unsigned int iKeyTimeOut)
{
	if(!m_bInit)
	{
		LLOG_ERROR("Please init redis client");
		return false;
	}

	std::list<std::string> listArg;
	listArg.push_back(strKey);
	listArg.push_back(strWrite);

	if(iKeyTimeOut != -1)
	{
		listArg.push_back("EX");
		listArg.push_back(convertUInt2String(iKeyTimeOut));
	}

	RedisValue result;
	return _excuteCommand("SET", listArg, result); 
}

bool CRedisClient::writeDataByString(const std::string& strKey, int iWrite, unsigned int iKeyTimeOut)
{
	return writeDataByString(strKey, convertInt2String(iWrite), iKeyTimeOut);
}

bool CRedisClient::writeDataByString(const std::string& strKey, unsigned int uWrite, unsigned int iKeyTimeOut)
{
	return writeDataByString(strKey, convertUInt2String(uWrite), iKeyTimeOut);
}

bool CRedisClient::writeDataByString(const std::string& strKey, short sWrite, unsigned int iKeyTimeOut)
{
	return writeDataByString(strKey, convertInt2String(sWrite), iKeyTimeOut);
}

bool CRedisClient::writeDataByString(const std::string& strKey, unsigned short suWrite, unsigned int iKeyTimeOut)
{
	return writeDataByString(strKey, convertUInt2String(suWrite), iKeyTimeOut);
}

bool CRedisClient::writeDataByString(const std::string& strKey, bool bWrite, unsigned int iKeyTimeOut)
{
	return writeDataByString(strKey, convertInt2String(bWrite), iKeyTimeOut);
}

bool CRedisClient::readDataByString(const std::string& strKey, std::string& strRead)
{
	if(!m_bInit)
	{
		LLOG_ERROR("Please init redis client");
		return false;
	}

	std::list<std::string> listArg;
	listArg.push_back(strKey);

	RedisValue result;
	bool bRet = _excuteCommand("GET", listArg, result);
	if(bRet)
	{
		strRead = result.toString();
	}

	return bRet;
}

bool CRedisClient::readDataByString(const std::string& strKey, int& iRead)
{
	std::string strRead;
	if(readDataByString(strKey, strRead))
	{
		iRead = atoi(strRead.c_str());
		return true;
	}

	return false;
}

bool CRedisClient::readDataByString(const std::string& strKey, unsigned int& uRead)
{
	std::string strRead;
	if(readDataByString(strKey, strRead))
	{
		uRead = (unsigned int)atoi(strRead.c_str());
		return true;
	}

	return false;
}

bool CRedisClient::readDataByString(const std::string& strKey, short& sRead)
{
	std::string strRead;
	if(readDataByString(strKey, strRead))
	{
		sRead = (short)atoi(strRead.c_str());
		return true;
	}

	return false;
}

bool CRedisClient::readDataByString(const std::string& strKey, unsigned short& suRead)
{
	std::string strRead;
	if(readDataByString(strKey, strRead))
	{
		suRead = (unsigned short)atoi(strRead.c_str());
		return true;
	}

	return false;
}

bool CRedisClient::readDataByString(const std::string& strKey, bool& bRead)
{
	std::string strRead;
	if(readDataByString(strKey, strRead))
	{
		bRead = (bool)atoi(strRead.c_str());
		return true;
	}

	return false;
}

bool CRedisClient::writeDataByHash(const std::string& strKey, const std::string& strField, const std::string& strWrite)
{
	if(!m_bInit)
	{
		LLOG_ERROR("Please init redis client");
		return false;
	}

	std::list<std::string> listArg;
	listArg.push_back(strKey);
	listArg.push_back(strField);
	listArg.push_back(strWrite);

	RedisValue result;
	return _excuteCommand("HSET", listArg, result);
}

bool CRedisClient::writeDataByHash(const std::string& strKey, const std::string& strField, int iWrite)
{
	return writeDataByHash(strKey, strField, convertInt2String(iWrite));
}

bool CRedisClient::writeDataByHash(const std::string& strKey, const std::string& strField, unsigned int uWrite)
{
	return writeDataByHash(strKey, strField, convertUInt2String(uWrite));
}

bool CRedisClient::writeDataByHash(const std::string& strKey, const std::string& strField, short sWrite)
{
	return writeDataByHash(strKey, strField, convertInt2String(sWrite));
}

bool CRedisClient::writeDataByHash(const std::string& strKey, const std::string& strField, unsigned short suWrite)
{
	return writeDataByHash(strKey, strField, convertUInt2String(suWrite));
}

bool CRedisClient::writeDataByHash(const std::string& strKey, const std::string& strField, bool bWrite)
{
	return writeDataByHash(strKey, strField, convertInt2String(bWrite));
}

bool CRedisClient::readDataByHash(const std::string& strKey, const std::string& strField, std::string& strRead)
{
	if(!m_bInit)
	{
		LLOG_ERROR("Please init redis client");
		return false;
	}

	std::list<std::string> listArg;
	listArg.push_back(strKey);
	listArg.push_back(strField);

	RedisValue result;
	bool bRet = _excuteCommand("HGET", listArg, result);
	if(bRet)
	{
		strRead = result.toString();
	}

	return bRet;
}

bool CRedisClient::readDataByHash(const std::string& strKey, const std::string& strField, int& iRead)
{
	std::string strRead;
	if(readDataByHash(strKey, strField, strRead))
	{
		iRead = atoi(strRead.c_str());
		return true;
	}

	return false;
}

bool CRedisClient::readDataByHash(const std::string& strKey, const std::string& strField, unsigned int& uRead)
{
	std::string strRead;
	if(readDataByHash(strKey, strField, strRead))
	{
		uRead = (unsigned int)atoi(strRead.c_str());
		return true;
	}

	return false;
}

bool CRedisClient::readDataByHash(const std::string& strKey, const std::string& strField, short& sRead)
{
	std::string strRead;
	if(readDataByHash(strKey, strField, strRead))
	{
		sRead = (short)atoi(strRead.c_str());
		return true;
	}

	return false;
}

bool CRedisClient::readDataByHash(const std::string& strKey, const std::string& strField, unsigned short& uRead)
{
	std::string strRead;
	if(readDataByHash(strKey, strField, strRead))
	{
		uRead = (unsigned short)atoi(strRead.c_str());
		return true;
	}

	return false;
}

bool CRedisClient::readDataByHash(const std::string& strKey, const std::string& strField, bool& bRead)
{
	std::string strRead;
	if(readDataByHash(strKey, strField, strRead))
	{
		bRead = (bool)atoi(strRead.c_str());
		return true;
	}

	return false;
}

bool CRedisClient::writeDataByList(const std::string& strKey, const std::string& strWrite)
{
	if(!m_bInit)
	{
		LLOG_ERROR("Please init redis client");
		return false;
	}

	std::list<std::string> listArg;
	listArg.push_back(strKey);
	listArg.push_back(strWrite);

	RedisValue result;
	return _excuteCommand("LPUSH", listArg, result);
}

bool CRedisClient::writeDataByList(const std::string& strKey, const int& iWrite)
{
	return writeDataByList(strKey, convertInt2String(iWrite));
}

bool CRedisClient::writeDataByList(const std::string& strKey, const unsigned int& uWrite)
{
	return writeDataByList(strKey, convertUInt2String(uWrite));
}

bool CRedisClient::writeDataByList(const std::string& strKey, const short& sWrite)
{
	return writeDataByList(strKey, convertInt2String(sWrite));
}

bool CRedisClient::writeDataByList(const std::string& strKey, const unsigned short& suWrite)
{
	return writeDataByList(strKey, convertUInt2String(suWrite));
}

bool CRedisClient::writeDataByList(const std::string& strKey, const bool& bWrite)
{
	return writeDataByList(strKey, convertInt2String(bWrite));
}

bool CRedisClient::writeDataByList(const std::string& strKey, const std::string& strWrite, unsigned int limit)
{
	if(!m_bInit)
	{
		LLOG_ERROR("Please init redis client");
		return false;
	}

	std::list<std::string> listArg;
	listArg.push_back(strKey);
	listArg.push_back(strWrite);

	RedisValue result;
	bool ret = _excuteCommand("LPUSH", listArg, result);
	if(ret)
	{
		listArg.clear();
		listArg.push_back(strKey);

		Lint count = result.toInt();
		while(count-- > limit)
		{
			_excuteCommand("RPOP", listArg, result);
		}
	}
	return ret;
}

bool CRedisClient::rmListDataByValue( const std::string& strKey, const std::string& strValue )
{
	if(!m_bInit)
	{
		LLOG_ERROR("Please init redis client");
		return false;
	}

	std::list<std::string> listArg;
	listArg.push_back(strKey);
	listArg.push_back("0");
	listArg.push_back(strValue);

	RedisValue result;
	return _excuteCommand("LREM", listArg, result);
}

bool CRedisClient::popListDataByKey( const std::string& strKey, std::string& outValue )
{
	if(!m_bInit)
	{
		LLOG_ERROR("Please init redis client");
		return false;
	}

	std::list<std::string> listArg;
	listArg.push_back(strKey);

	RedisValue result;
	bool ret = _excuteCommand("LPOP", listArg, result);
	if(ret)
	{
		outValue = result.toString();
	}
	return ret;
}

bool CRedisClient::readDataByList(const std::string& strKey, unsigned int iFromIndex, unsigned int iReadCount, std::list<std::string>& listRead)
{
	if(!m_bInit)
	{
		LLOG_ERROR("Please init redis client");
		return false;
	}

	std::list<std::string> listArg;
	listArg.push_back(strKey);
	listArg.push_back(convertUInt2String(iFromIndex));
	listArg.push_back(convertUInt2String(iReadCount + iFromIndex - 1));

	RedisValue result;
	bool bRet = _excuteCommand("LRANGE", listArg, result);
	if(bRet)
	{
		if(result.isArray())
		{
			std::vector<RedisValue> vecValue = result.toArray();
			for(auto it = vecValue.begin(); it != vecValue.end(); it++)
			{
				std::string strRead = it->toString();
				listRead.push_back(strRead);
			}
		}
	}

	return bRet;
}

bool CRedisClient::setKeyTTLTime( const std::string& strKey, unsigned int tim_secs )
{
	if(!m_bInit)
	{
		LLOG_ERROR("Please init redis client");
		return false;
	}

	std::list<std::string> listArg;
	listArg.push_back(strKey);
	listArg.push_back(convertUInt2String(tim_secs));

	RedisValue result;
	
	return _excuteCommand("EXPIRE", listArg, result);
}

bool CRedisClient::isExistKey(const std::string& strKey)
{
	if(!m_bInit)
	{
		LLOG_ERROR("Please init redis client");
		return false;
	}

	std::list<std::string> listArg;
	listArg.push_back(strKey);

	RedisValue result;
	if(_excuteCommand("EXISTS", listArg, result))
	{
		return result.toInt() == 1;	
	}
	else
	{
		return false;
	}
}

bool CRedisClient::delKey(const std::string& strKey)
{
	if(!m_bInit)
	{
		LLOG_ERROR("Please init redis client");
		return false;
	}

	std::list<std::string> listArg;
	listArg.push_back(strKey);

	RedisValue result;
	return _excuteCommand("DEL", listArg, result);
}

bool CRedisClient::delHKey(const std::string& strKey, const std::string& strField)
{
	if(!m_bInit)
	{
		LLOG_ERROR("Please init redis client");
		return false;
	}

	std::list<std::string> listArg;
	listArg.push_back(strKey);
	listArg.push_back(strField);

	RedisValue result;
	return _excuteCommand("HDEL", listArg, result);
}

bool CRedisClient::setKeyTimeOut(const std::string& strKey, unsigned int iKeyTimeOut)
{
	if(!m_bInit)
	{
		LLOG_ERROR("Please init redis client");
		return false;
	}

	std::list<std::string> listArg;

	listArg.push_back(strKey);

	char chTime[20] = {0};
	sprintf(chTime, "%d", (Lint)iKeyTimeOut);
	listArg.push_back(chTime);

	RedisValue result;
	return _excuteCommand("EXPIRE", listArg, result);
}

bool CRedisClient::_connectRedis()
{
	m_redisClient.reset(new RedisSyncClient(m_ioService));

	bool bRet = false;
	try
	{
		boost::asio::ip::address redisAddress = boost::asio::ip::address::from_string(m_strRedisIp);
		std::string strError;
		if(m_redisClient->connect(redisAddress, m_uRedisPort, strError))
		{
			bRet = true;
		}
		else
		{
			LLOG_ERROR("Fail to connect redis. Error = %s", strError.c_str());
		}
	}
	catch(boost::system::error_code e)
	{
		LLOG_ERROR("Excepion on boost. Error = %s", e.message().c_str());
	}
	catch(...)
	{
		LLOG_ERROR("Exception on redis");
	}

	return bRet;
}

bool CRedisClient::_excuteCommand(const std::string& strCmd, const std::list<std::string>& listArg, RedisValue& result)
{
	int iArgCount = listArg.size();
	if(iArgCount > 7)
	{
		LLOG_ERROR("arg count is more than 7");
		return false;
	}
	
	bool bRet = false;
	int iTryTimes = 0;
	do 
	{
		try
		{
			if(iArgCount == 0)
			{
				result = m_redisClient->command(strCmd);
			}
			else
			{
				result = m_redisClient->command(strCmd, listArg);
			}

			if(result.isOk())
			{
				bRet = true;
			}
			else
			{
				LLOG_ERROR("Failed to excute command. Error = %s", result.toString().c_str());
			}
			break;
		}
		catch(...)
		{
			LLOG_ERROR("Exception on excute redis command");
		}

		//说明没有执行成功，可能断开连接,试着重新连接
		if(!_connectRedis())
		{
			LLOG_ERROR("Don't connect redis");
			break;
		}

		iTryTimes++;
	}while(iTryTimes < 3);

	return bRet;
}

bool CRedisClient::WriteDataToZSet(const std::string& key, std::string score, std::string member)
{
	if (!m_bInit)
	{
		LLOG_ERROR("Please init redis client");
		return false;
	}

	std::list<std::string> listArg;
	listArg.push_back(key);
	listArg.push_back(score);
	listArg.push_back(member);

	RedisValue result;
	return _excuteCommand("ZINCRBY", listArg, result);
}

bool CRedisClient::WriteDataToZSet(const std::string& key, Llong score, int member)
{
	return WriteDataToZSet(key, convertLL2String(score), convertInt2String(member));
}

bool CRedisClient::GetDataFromZSetReverScores(const std::string& key, Lint start, Lint end, std::vector<std::string>& result)
{
	if (!m_bInit)
	{
		LLOG_ERROR("Please init redis client");
		return false;
	}

	if (start > end)
		return false;

	std::list<std::string> listArg;
	listArg.push_back(key);
	listArg.push_back(convertInt2String(start));
	listArg.push_back(convertInt2String(end));
	listArg.push_back("withscores");
	

	RedisValue redis_result;
	bool bRet = _excuteCommand("ZREVRANGE", listArg, redis_result);
	if (bRet)
	{
		if (redis_result.isArray())
		{
			std::vector<RedisValue> vecValue = redis_result.toArray();
			for (auto it = vecValue.begin(); it != vecValue.end(); it++)
			{
				std::string strRead = it->toString();
				result.push_back(strRead);
			}
		}
	}

	return bRet;
}

bool CRedisClient::GetScoreFromZSet(const std::string& key, int member, Llong& redis_scores)
{
	if (!m_bInit)
	{
		LLOG_ERROR("Please init redis client");
		return false;
	}

	std::list<std::string> listArg;
	listArg.push_back(key);
	listArg.push_back(convertInt2String(member));
	
	RedisValue redis_result;
	bool bRet = _excuteCommand("ZSCORE", listArg, redis_result);
	if (bRet)
	{
		if (redis_result.isString())
		{
			redis_scores = atoll(redis_result.toString().c_str());
		}
	}

	return bRet;
}