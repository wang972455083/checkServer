#ifndef _NEW_ZHUAN_PAN_ACTIVITY_TBL_H_
#define _NEW_ZHUAN_PAN_ACTIVITY_TBL_H_

#include<utility>
#include "LSingleton.h"

struct RewardCfg
{
	Lint	m_rewardId;
	Lint	m_redwardType;
	Lint	m_redwardCount;
	Lint	m_redwardWeight;

	RewardCfg()
	{
		m_rewardId = 0;
		m_redwardType = 0;
		m_redwardCount = 0;
		m_redwardWeight = 0;
	}
};

enum
{
	RT_CARD = 1,	//·¿¿¨
	RT_MONEY = 2,	// Ç®
};

class NewZhuanPanActivityTbl:public LSingleton<NewZhuanPanActivityTbl>
{
public:
	NewZhuanPanActivityTbl();
	~NewZhuanPanActivityTbl();

	virtual bool Init();
	virtual bool Final();

public:
	bool LoadRewardTbl();
	bool LoadNewZhuanPanTbl();
	void Clear();
public:
	bool	IsOpen(){return m_bOpen;}
	int		GetMaxWinnerNeedCnt(){return m_nMaxWinnerNeedCnt;}
	int		GetMaxWinnerDailyDrawMaxCnt(){return m_nMaxWinnerDailyDrawMaxCnt;}
	int		GetCreateRoomNeedCnt(){return m_nCreateRoomNeedCnt;}
	int		GetCreateRoomDailyDrawMaxCnt(){return m_nCreateRoomDailyDrawMaxCnt;}
	int		GetContinueLoginNeedCnt(){return m_nContinueLoginNeedCnt;}
	int		GetMaxMoney(){return m_nMaxMoney;}
public:
	RewardCfg* GetReward(bool bOnlyCard);

private:
	bool	m_bOpen;
	int		m_nMaxWinnerNeedCnt;
	int		m_nMaxWinnerDailyDrawMaxCnt;

	int		m_nCreateRoomNeedCnt;
	int		m_nCreateRoomDailyDrawMaxCnt;

	int		m_nContinueLoginNeedCnt;
	int		m_nMaxMoney;

	int		m_nMaxCardWeight;
	int		m_nMaxWeight;
	std::vector<RewardCfg*>	m_vecReward;
	
};

#define gNewZhuanPanTbl NewZhuanPanActivityTbl::Instance()

#endif