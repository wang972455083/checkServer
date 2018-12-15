#ifndef __MATCHRESULTMANAGER_H__
#define __MATCHRESULTMANAGER_H__

#include "LSingleton.h"
#include "DBSession.h"
#include "LMsgS2S.h"
#include <vector>


class MatchResultManager: public LSingleton<MatchResultManager>
{
public:
	virtual	bool		Init();
	virtual	bool		Final();

	void				LoadMatchResultInfo();
	void				LoadMatchResultInfo(Lint serverID);

	void                AddRecord(MatchResultInfo* info);

	void                SaveResultInfo(MatchResultInfo &info);

private:
	std::map<Lint, MatchResultInfo*>	m_userMatchInfoMap;
	DBSession								m_dbsession;
	boost::mutex                       m_mutexInsert;
};

#define gMatchResultManager MatchResultManager::Instance()

#endif 