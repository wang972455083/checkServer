#ifndef _SCORE_H_
#define _SCORE_H_

#include <vector>

//算分项
enum SCORE_ITEM
{
	SI_INVALID			= 0,	//无效项
	SI_XIAO_HU			= 1,	//平胡
	SI_QING_YI_SE		= 2,	//清一色
	SI_PENG_PENG_HU		= 3,	//碰碰胡
	SI_QI_DUI			= 4,	//七对
	SI_HAO_HUA_QI_DUI	= 5,	//豪华七对
	SI_QING_PENG_PENG_HU= 6,	//清碰碰胡
	SI_QING_QI_DUI		= 7,	//清七对
	SI_QING_HAO_HUA_QI_DUI = 8,	//清豪华七对
	SI_JIE_PAO			= 9,	//接炮
	SI_DIAN_PAO			= 10,	//点炮
	SI_ZI_MO			= 11,	//自摸
	SI_TIAN_HU			= 12,	//天胡
	SI_DI_HU			= 13,	//地胡
	SI_HUA_HU			= 14,	//花胡
	SI_CHAO_HAO_HUA_QI_DUI = 15,//超豪华七对
	SI_CHAO_CHAO_HAO_HUA_QI_DUI = 16,//超超豪华七对


	/********湖北占用******/
	SI_XIAO_XUE    = 800,    //小血
	SI_DA_XUE      = 801,    //大血
	SI_RUAN_PI_HU  = 802,    //软屁胡
	SI_YING_PI_HU  = 803,    //硬屁胡 
	SI_QUANQIU_REN = 804,    //全求人
	/*********************/

	SI_MING_GANG		= 940,	//明杠(all)
	SI_AN_GANG			= 950,	//暗杠(all)
	SI_DI_HUA           = 960,	//底花济南
	SI_DI_BU_HUA        = 970,	//补花济南
	SI_SHOU_HUA         = 980,	//手花济南
	SI_GA               = 990,	//跑分(all)
	
	/********湖北占用******/
	SI_PIZIGANG      = 1000, //痞子杠
	SI_LAIZIGANG     = 1001, //癞子杠

	SI_FENGDING      = 1010, //封顶
	SI_JINDING       = 1011, //金顶

	//咸宁
	SI_QIANG_GANG_HU_LOSE						= 1012,	//抢杠失败
	SI_QIANG_GANG_HU_WIN						= 1013,	//抢杠成功


	//SI_YING_QIANG_GANG_HU					= 1016,		//硬抢杠胡
	//SI_RUAN_QIANG_GANG_HU					= 1017,		//软抢杠胡
	SI_ZUAN_SHI_DING						= 1018,		//钻石顶
	SI_YANG_GUANG_DING						= 1019,		//阳光顶
	
	SI_HONG_ZHONG_GANG						= 1020,		//红中开杠
	SI_KAI_KOU								= 1021,		//开口
	SI_YING_PAI								= 1022,		//硬牌

	SI_YIN_DING								= 1023,		//银顶

	/**********************/

	SI_QIANG_GANG		= 17,	//抢杠
	
	SI_GANG_SHANG_HUA	= 18,	//杠上花
	SI_HUA_SHANG_HUA	= 19,	//花上花
	SI_LIAN_ZHUANG		= 20,	//连庄
	SI_SHI_SAN_YAO		= 21,	//13幺

	SI_ZHUANG_XIAN		= 23,	//庄闲
	SI_SHOU_ZHANG_BEI_GEN = 24,	//首张被跟
	SI_SHOU_ZHANG_BEI_GANG= 25,	//首张被杠	
	SI_3_DAO_BAO_PAI	= 26,	//3道包牌
	SI_4_DAO_BAO_PAI	= 27,	//4道包牌
	SI_QIANG_GANG_BAO_PAI = 28,	//抢杠包牌
	SI_DIAN_GANG		= 29,	//点杠 3#
	SI_SHUN_HUA_HU		= 30,	//顺花胡

	//搂宝
	SI_ZI_YI_SE			= 31,	//字一色
	SI_JIANG_YI_SE		= 32,	//将一色
	SI_LOU_BAO			= 33,	//搂宝
	SI_BAO_ZHONG_BAO	= 34,	//宝中宝
	SI_BAO_MING_LOU 	= 35,	//明楼
	SI_BAO_HAIDILAO 	= 36,	//海底捞
	SI_BAO_SHUAIPAI 	= 37,	//甩牌
	SI_BAO_DADIAOCHE 	= 38,	//大吊车
	SI_BAO_JIAO_ZI      = 39,   //包饺子
	SI_BU_GANG  		= 40,	//补杠搂宝
	//青岛
	SI_DAN_DIAO_JIANG = 41,          //单调将
	SI_HUI_JIANG = 42,          //会将
	SI_SHEN_JIANG = 43,          //神将
	SI_HU_HUA = 44,          //胡花
	SI_JIU_LIAN_BAO_DENG = 45,	//九莲宝灯
	SI_DA_SAN_YUAN = 46,	//大三元
	SI_DA_SI_XI = 47,	//大四喜
	SI_BAO_SAN_JIA = 48,	//包三家
	//菏泽麻将
	SI_BAO_ZUI	= 49,		//报嘴
	SI_MAI_ZHA_DAN = 50,	//买炸弹
	SI_MAI_PAO_ZI  = 51,	//买跑子
	SI_DAI_CAI	   = 52,	//带财
	SI_CHI_SAN_KOU	   = 53,	//吃三口
	SI_BUY_QUE_MEN = 54,	//买缺门

	//烟台麻将
	SI_MAI_AN_GANG_WIN = 55, //买暗杠赢
	SI_MAI_AN_GANG_LOSE = 56, //买暗杠输
	SI_GANG_PAO = 57,         //杠炮
	//济南
	SI_258JIANGYISE = 151,       //将一色
	SI_HUN_YI_SE = 152,		     //混一色
	SI_HAI_DI_LAO = 153,         //海底捞
	SI_DA_DIAO_CHE = 154,        //大吊车
	SI_YI_TIAO_LONG = 155,       //一条龙
	SI_SHI_SAN_BU_KAO = 156,     //十三不靠
	SI_MING_LOU = 157,		     //明楼
	SI_MENQING = 158,		     //门清

	SI_TOTAL_HUA = 162,          //总花数济南
	SI_TOTAL_FAN = 163,          //总番数济南
	SI_HUANG_ZHUANG_COUNT = 200, //荒庄次数济南

	SI_PU=201,					//金乡 铺
	SI_MING_DA = 202,			//明打
	SI_DAN_DIAO = 203,				//单调
	SI_BIAN = 204,					//边胡
	SI_MAO = 205,					//毛的个数

	SI_DAI_QUE_MEN = 206,			//带缺门
	SI_DIAN_PENG = 209,				//点碰
	SI_SHEN_CARD = 208,				//神牌
	SI_YU_SCORE = 207,				//鱼分
	SI_SHUAI_HUN = 210,				//甩会牌
	SI_MAN_HUI = 211,				//满会牌

						
	SI_END = 10000,
};

struct MatchScoreNotice
{
	/////////////////match使用/////////////////
	Lint		m_id;  //玩家id
	Lstring		m_nike;//玩家昵称
	Lint        m_score;//玩家积分
	Lint        m_mc;   //名次
	Lstring		m_headImageUrl;//玩家头像
	/////////////////////////////////
	MSGPACK_DEFINE(m_id, m_nike, m_score, m_mc, m_headImageUrl);
public:
	MatchScoreNotice()
		: m_id(0)
		, m_score(0)
		, m_mc(0)
	{}
};

struct ScoreNotice
{
	int m_score_item;	//算分项
	int m_count;		//个数
	MSGPACK_DEFINE(m_score_item,m_count);
public:
	ScoreNotice()
		:m_score_item(SI_INVALID)
		,m_count(0)
	{}
};

typedef std::vector<ScoreNotice> ScoreNoticeVec; 

#endif