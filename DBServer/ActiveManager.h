#ifndef _ACTIVE_MANAGER_H
#define _ACTIVE_MANAGER_H

#include "LSingleton.h"
#include "LMsgS2S.h"
#include "LRedisClient.h"

// 转盘
#define ActivityDraw_Config "ActivityDraw_Config"						// 配置
#define ActivityDraw_RemainTotalCount_ "ActivityDraw_RemainTotalCount_"	// 剩余库存数量
#define ActivityDraw_RemainTotalDate "ActivityDraw_RemainTotalDate"		// 记录剩余库存日期

//红包
#define ActivityRedPackets_Config "ActivityRedPackets_Config"						// 配置
#define ActivityRedPackets_Config_Tomorrow "ActivityRedPackets_Config_Tomorrow"		// 配置明天生效
#define ActivityRedPackets_Config_Time "ActivityRedPackets_Config_Time"			// 配置起始时间	立即生效		
#define ActivityRedPackets_RemainTotalCount_ "ActivityRedPackets_RemainTotalCount_"	// 剩余库存数量
#define ActivityRedPackets_Remain          "ActivityRedPackets_Remain"				//	剩余红包总金额
#define ActivityRedPackets_RemainTotalDate "ActivityRedPackets_RemainTotalDate"		// 记录剩余库存日期

// 分享
#define ActivityShare_Config "ActivityShare_Config"
//新转盘活动每天抽中的钱数
#define NewZhuanPanActivityDailyDrawMoney "NewZhuanPanActivityDailyDrawMoney"

//活动id
enum
{
	ACTIVITY_ZHUAN_PAN_REDPACKET = 1005,
	NEW_ZHUAN_PAN_ACTIVITY		 = 1006,  //新转盘活动
};

struct Activity
{
	Activity()
	{
		m_ActiveID = 0;
		m_BeginTime = -1;
		m_EndTime = -1;
	}

	Lint	m_ActiveID;		// ActivityID类型
	Lstring	m_ActiveName;
	Lstring	m_Desc;
	Lint  m_BeginTime;
	Lint  m_EndTime;

	Lstring m_strConfig;		// 后台配置文件
};

struct Money
{
	Lint m_money;
	Lint m_index;

	Money(Lint money,Lint index)
	{
		m_money = money;
		m_index = index;
	}
};

struct ActivityDrawGift
{
	ActivityDrawGift()
	{
		m_GiftIndex = -1;
		m_GiftType = 0;
		m_GiftCount = 0;
		m_GiftPercent = 0;
		m_GiftTotalCount = 0;
		m_GiftRemainTotalCount = 0;
		m_GiftEnsure = false;
		m_GiftResumeNextDay = false;
	}
	Lint m_GiftIndex;			// 奖励索引 自动生成
	Lstring m_GiftName;			// 奖励名称
	Lint m_GiftType;			// 奖励类型 1房卡（单位个） 2话费（单位元） 3 京东购物卡（单位元） 4 红米NOTE 4X 32G（单位个） 5 小米5C（单位个） 6 Macbook pro 13（单位台）
	Lint m_GiftCount;			// 奖励数量
	Lint m_GiftPercent;			// 奖励权值
	Lint m_GiftTotalCount;		// 配置的库存 -1表示无限
	Lint m_GiftRemainTotalCount;// 剩余的库存  -1表示无限
	bool m_GiftEnsure;			// 其他库存为0时 确保中的奖品
	bool m_GiftResumeNextDay;	// 隔日是否恢复库存
};
struct ActivityRedPacketsGift
{
	ActivityRedPacketsGift()
	{
		m_GiftIndex = -1;
		m_GiftIntervalBottom = 0;
		m_GiftIntervalTop = 0;
		m_GiftPercent = 0;
		m_GiftTotalCount = 0;
		m_GiftRemainTotalCount = 0;
		m_GiftEnsure = false;
		m_GiftResumeNextDay = false;
	}
	Lint m_GiftIndex;			// 奖励索引 自动生成
	Lstring m_GiftName;			// 奖励名称
	Lint m_GiftIntervalBottom;	// 奖励档次下限
	Lint m_GiftIntervalTop;		// 奖励档次上限
	Lint m_GiftPercent;			// 奖励权值
	Lint m_GiftTotalCount;		// 配置的库存 -1表示无限
	Lint m_GiftRemainTotalCount;// 剩余的库存  -1表示无限
	bool m_GiftEnsure;			// 其他库存为0时 确保中的奖品
	bool m_GiftResumeNextDay;	// 隔日是否恢复库存
};

struct ActivityDraw : public Activity
{
	ActivityDraw()
	{
		m_SpendType = 0;
		m_SpendCount = 0;
		m_GiftTotalDate = 0;
		m_GiftTime = 0;
		m_TotalGiftPercent = 0;
	}

	Lint m_SpendType;			// 消耗类型 0免费 1房卡（单位个）
	Lint m_SpendCount;			// 消耗数量
	Lint m_GiftTotalDate;		// 库存日志 用于隔日恢复
	Lint m_GiftTime;			// 表示抽奖的次数
	std::vector<ActivityDrawGift> m_Gift;
	Lint m_TotalGiftPercent;	// 中奖总的权值
};

struct ActivityShare : public Activity
{
	ActivityShare()
	{
		m_GiftType = 0;
		m_GiftCount = 0;
	}

	Lstring m_GiftName;			// 奖励名称
	Lint m_GiftType;			// 励类型 1房卡（单位个）
	Lint m_GiftCount;			// 奖励数量
};

struct ActivityRedPackets : public Activity
{
	ActivityRedPackets()
	{
		m_GiftTotalDate = 0;
		m_TotalGiftPercent = 0;
		m_limit = 0;
		m_left = 0;
		m_limiteResumeNextDay = false;
	}

	Lint m_GiftTotalDate;		// 库存日志 用于隔日恢复
	std::vector<ActivityRedPacketsGift> m_Gift;
	Lint m_TotalGiftPercent;	// 中奖总的权值
	Lint m_limit;				// 当日红包总量（不超过）（分）
	Lint m_left;			// 剩余的红包总金额（分）
	bool m_limiteResumeNextDay;  //当日红包总量重置
};

//新转盘送红包活动
struct ActivityZhuanPanRedpacket
{
	Lint		m_drawCnt;
	Lint		m_drawTime;

	ActivityZhuanPanRedpacket()
	{
		m_drawCnt = 0;
		m_drawTime = 0;
	}
};

struct NewZhuanPanActivity
{
	Lint	m_userId;
	Lint	m_DailyMaxWinnerCnt;
	Lint	m_DailyMaxWinnerDarwCnt;
	Lint	m_DailyCreateRoomCnt;
	Lint	m_DailyCreateRoomDarwCnt;
	Lint	m_LastFenXiangTime;
	Lint	m_ContinueFenXiangDays;
	Lint	m_ResetTime;

	NewZhuanPanActivity()
	{
		m_userId = 0;
		m_DailyMaxWinnerCnt = 0;
		m_DailyMaxWinnerDarwCnt = 0;
		m_DailyCreateRoomCnt = 0;
		m_DailyCreateRoomDarwCnt = 0;
		m_LastFenXiangTime = 0;
		m_ContinueFenXiangDays = 0;
		m_ResetTime = 0;
	}

	void DailyClear()
	{
		m_DailyMaxWinnerCnt = 0;
		m_DailyMaxWinnerDarwCnt = 0;
		m_DailyCreateRoomCnt = 0;
		m_DailyCreateRoomDarwCnt = 0;
	}
};

class ActiveManager:public LSingleton<ActiveManager>
{
public:
	virtual	bool		Init();
	virtual	bool		Final();

	// 转盘接口====================================================
	// 设置转盘数据 内部清除、解析和保存
	void SetDrawActiveConfig(const Lstring& strSet,int id);
	// 清除转盘数据
	void DrawSetClear();
	// 解析转盘数据 内部调用
	void _UpdateDrawActiveSet(const Lstring& strSet,int id);
	// 保存读取转盘配置
	void SaveDrawActiveSet();
	void ReadDrawActiveSet();
	// 是否开启转盘
	bool CheckDrawActiveValid();
	//用户是否拥有开启转盘的条件
	bool CheckUserActiveValid();
	// 转盘ID
	int GetDrawActiveId();
	int GetDrawBeginTime();
	int GetDrawEndTime();
	// 转盘消耗
	int GetDrawSpendType();
	int GetDrawSpendCount();
	// 计算转盘中奖 fGift表示要生成的假数据
	ActivityDrawGift GetDrawGift( ActivityDrawGift& fGift );
	Lint    getRestTimes(Lint userid);
	bool	isRedisTimeNotToday(const Lstring& key);
	void	updateRedisKeyTime(const Lstring& key);
	void	increaseRedisKeyValue(const Lstring& key);
	//此处封装的目的是为了多线程的互斥访问
	bool    redisWriteDataByString(const std::string& strKey, int iWrite);
	bool    redisReadDataByString(const std::string& strKey, int& iRead);
	bool    redisSetKeyTTLTime(const std::string& strKey, unsigned int tim_secs);
	bool    redisReadDataByList(const std::string& strKey, unsigned int iFromIndex, unsigned int iReadCount, std::list<std::string>& listRead);
	bool    redisWriteDataByList(const std::string& strKey, const std::string& strWrite, unsigned int limit);
	// 分享接口
	void UpdateShareActiveSet(const Lstring& strSet,int id);
	Lstring GetShareActiveSet(); 
	int GetShareActiveId();

	ActivityShare GetShareSet();

	bool CheckShareActiveValid();
	bool CheckShareActiveStart();
	void ShareSetClear();

	//红包接口
	void SetRedPacketsActiveConfig(const Lstring& strSet, int id);
	void RedPacketsSetClear();
	void _UpdateRedPacketsActiveSet(const Lstring& strSet, int id);
	void SaveRedPacketsActiveSet();
	void ReadRedPacketsActiveSet();
	bool CheckRedPacketActiveValid();

	//服务重启加载redis用
	void _ReloadRedPacketsActiveSet(const Lstring& strSet, int id);
	void _ReloadRedPacketsActiveTime(const Lstring& strSet);

	// 红包
	int GetRedPacketsActiveId();
	int GetRedPacketsBeginTime();
	int GetRedPacketsEndTime();
	Money GetRedPacketGift();
	int GetRaminGiftCount();
	void ReturnRedPacketBack(Lint index, Lint money, Lint invitee, Lint type);
	void ReloadRedPacketActivitySetTomorrow();


	//新转盘活动
	void UpdateZhuanPanRedpacketMap(Lint userId,ActivityZhuanPanRedpacket* pData);
	ActivityZhuanPanRedpacket* GetZhuanPanRedpacketData(Lint userId);
	bool	IsZhuanPanRedpacketOpen();	

	//新新转盘活动
public:
	void InsertNewZhuanPanActivityMap(Lint userId,NewZhuanPanActivity* pData);
	NewZhuanPanActivity* GetNewZhuanPanActivityData(Lint userId);

public:
	boost::recursive_mutex m_Mutex;
	CRedisClient m_RedisClient;

	ActivityDraw m_Draw;
	ActivityShare m_Share;
	ActivityRedPackets m_RedPackets;

	std::map<Lint,ActivityZhuanPanRedpacket*>	m_mapZhuanPanRedpacket;
	std::map<Lint,NewZhuanPanActivity*>			m_mapNewZhuanPanActivity;
};

#define gActiveManager ActiveManager::Instance()

#endif

