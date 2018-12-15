#pragma once
#include <map>
//#include "../common/const_def.h"
using namespace std;

class ActiveShopTbl
{
public:
	ActiveShopTbl()
	{

	}
	~ActiveShopTbl()
	{
	}

	int m_nID;

	time_t m_tBeginTime;
	time_t m_tEndTime;

	int m_nRewardID;
	int m_nActiveFlag;

	int m_nType;
	int m_nBuyCnt;
	int m_nCost;

	bool IsCurrentTime( ) const;
};
class ActiveShopCfg
{
	enum E_ACTIVE_SHOP_FILE
	{
		E_ACTIVE_SHOP_ID=0,
		E_ACTIVE_SHOP_USEING,
		E_ACTIVE_SHOP_COMMON,
		E_ACTIVE_SHOP_TIME_BG_D,
		E_ACTIVE_SHOP_TIME_BG_T,
		E_ACTIVE_SHOP_TIME_ED_D,
		E_ACTIVE_SHOP_TIME_ED_T,
		E_ACTIVE_SHOP_REWARD_ID,
		E_ACTIVE_SHOP_ACTIVE_FLAG,
		E_ACTIVE_SHOP_TYEP,
		E_ACTIVE_SHOP_BUY_CNT,
		E_ACTIVE_SHOP_COST,
	};

public:
	ActiveShopCfg(void);
	~ActiveShopCfg(void);

	static ActiveShopCfg& Instance()
	{
		static ActiveShopCfg inst;
		return inst;
	}

	bool			LoadTbl();
	void			ClearTbl();

	const ActiveShopTbl* GetActiveShopTbl(int nID);
	const ActiveShopTbl* GetActiveShopTblByNum( int nNum );
	const ActiveShopTbl* GetActiveShopTblByType( int nType );

	int				GetTblNum(){ return m_mapGift.size(); }
protected:
	static const char*		szGiftFile;
	map<int,ActiveShopTbl*> m_mapGift;
};

