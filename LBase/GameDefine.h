#ifndef _GAME_DEFINE_H_
#define _GAME_DEFINE_H_

#define CARD_COUNT 144
#define REN_MAN_MATCH_TYPE 9999; //人满赛的标记

#define FEN_BAO_COST_CARD_16_CIRCLE_TYPE  5  //分包16圈扣除房卡数量

enum CARD_COLOR
{
	CARD_COLOR_WAN = 1,			//万
	CARD_COLOR_TUO = 2,			//筒
	CARD_COLOR_SUO = 3,			//条
	CARD_COLOR_FENG = 4,		//东南西北风
	CARD_COLOR_JIAN = 5,		//中发白
	CARD_COLOR_FLOWER = 6,		//春夏秋冬 梅兰竹菊
};

//玩家思考类型
enum THINK_OPERATOR
{
	THINK_OPERATOR_NULL = 0, 
	THINK_OPERATOR_OUT = 1,             //打牌
	THINK_OPERATOR_BOMB      = 2,		//点炮
	THINK_OPERATOR_AGANG     = 3,		//暗杠
	THINK_OPERATOR_MGANG     = 4,		//明杠
	THINK_OPERATOR_PENG      = 5,		//碰
	THINK_OPERATOR_CHI       = 6,		//吃
	THINK_OPERATOR_ABU       = 7,		//暗补
	THINK_OPERATOR_MBU       = 8,		//明补
	THINK_OPERATOR_BU_HUA    = 9,		//补花
	THINK_OPERATOR_ZHI_MGANG = 10,		//直杠
	THINK_OPERATOR_WAN_MGANG = 11,		//弯杠
	THINK_OPERATOR_TING      = 12,		//听牌
	THINK_OPERATOR_AN_LOU    = 13,      //暗楼
	THINK_OPERATOR_MING_LOU  = 14,      //明楼
	THINK_OPERATOR_SHUAI     = 15,		//甩牌
	THINK_OPERATOR_CANCLE_TING=16,      //打牌导致不能听
	THINK_OPERATOR_BAO_ZUI	  =17,      //菏泽麻将报嘴
	THINK_OPERATOR_LAST_CARD_GUO	  =18,      //菏泽麻将最后两张牌不能打出,这个就是过的想法，过就是自己不大出牌，然后直接下家摸牌
	THINK_OPERATOR_NIU_CARD   =19,      //扭牌
	THINK_OPERATOR_QISHOU_TING = 20,	//潍坊起手听牌
	THINK_OPERATOR_MAI_AN_GANG = 21,    //烟台买暗杠
	THINK_OPERATOR_QI_SHOU_MING_LOU=22,	//邹城起手明楼
	THINK_OPERATOR_XU_NI_5         = 23,
	THINK_OPERATOR_ZHANG_MAO		= 24, //威海长毛
	THINK_OPERATOR_LUAN_MAO			= 25, //威海乱毛
	THINK_OPERATOR_CHU_MAO			= 26, //出毛牌
	THINK_OPERATOR_FANG_FENG		= 27,  //放风
	THINK_OPERATOR_LAIZI_GANG       = 28,  //癞子杠
	THINK_OPERATOR_PIZI_GANG        = 29,  //痞子杠
	THINK_OPERATOR_HONG_ZHONG_GANG        = 30,  //红中杠
	THINK_OPERATOR_QIANG_GANG_HU	= 31,  //抢杠胡
	};
#define DESK_USER_COUNT 4	//一桌玩家数量
#define DESK_USER_COUNT_TRIO 3	//3人玩法的人数

#define INVAILD_POS		-1  //无效的位置
#define EFFECTIVE_POS(pos, max_pos)	((pos) >= 0 && (pos) < (max_pos))

#define	PLAY_CARD_TIME		10  //出牌时间
#define WAIT_READY_TIME		15  //准备时间

#define RESERVE_CARD_COUNT	15	//预留出来的7蹲半牌,15张
#define RESERVE_CARD_COUNT_JINAN	12	//预留出来的6墩牌荒庄 济南麻将
#define BASE_SCORE	1			        //底分

//比赛房间当前状态
enum ROOM_STATE
{
	ROOM_FREE = 0,	//桌子空闲状态
	ROOM_WAIT,			//桌子等待状态
	ROOM_PLAY,			//正在游戏中
	ROOM_CONTINUE,      //一轮结束，继续游戏
	ROOM_END,           //比赛结束
						//	DESK_COUNT_RESULT,	//正在结算中
						//	DESK_RESET_ROOM		//正在申请解散房间中
};

enum MATCH_RULES
{
	MATCH_CARD     = 1,           //房卡
};

enum MATCH_LEVEL
{
	MATCH_RULES_V0 = 0,           //V0资格卡 玩家被淘汰 参赛资格被扣除
	MATCH_RULES_V1 = 1,           //V1资格卡
	MATCH_RULES_V2 = 2,           //V2资格卡
	MATCH_RULES_V3 = 3,           //V3资格卡
	MATCH_RULES_V4 = 4,           //V4资格卡 
	MATCH_RULES_V5 = 5,           //V5资格卡
	MATCH_RULES_V6 = 6,           //V6资格卡
};

enum MATCH_USER_STATE
{
	MATCH_USER_FREE = 0,	//桌子空闲状态
	MATCH_USER_WAIT,			//桌子等待状态
	MATCH_USER_M_PLAY,			//正在游戏中
	MATCH_USER_CONTINUE,      //一轮结束，继续游戏
	MATCH_USER_END,           //比赛结束
	MATCH_USER_ERROR,         //错误状态
};

//桌子当前状态
enum DESK_STATE
{
	DESK_FREE =		0,	//桌子空闲状态
	DESK_WAIT,			//桌子等待状态
	DESK_PLAY,			//正在游戏中
};

//牌局当前状态
enum DESK_PLAY_STATE
{
	DESK_PLAY_START_HU,		//起手胡状态
	DESK_PLAY_GET_CARD,		//摸牌
	DESK_PLAY_THINK_CARD,	//思考
	DESK_PLAY_END_CARD,		//海底牌
	DESK_PLAY_SET_GA,		//定噶
	DESK_PLAY_BUY_ITEM,		//买东西
	DESK_PLAY_XUNI_5,       //烟台虚拟5
	DESK_PLAY_BUY_AN_GANG,	//买暗杠
};

//家胡牌情况，0-没胡，1-自摸，2-收炮，3-点炮
enum WIN_TYPE
{
	WIN_ZIMO,		//自摸 0
	WIN_BOMB,		//点炮 1
	WIN_NONE,		//荒庄 2
	WIN_MAIANGANG_WIN, //买暗杠赢 3
	WIN_MAIANGANG_LOSE,//买暗杠输 4
};

//胡牌方式
enum WIN_TYPE_SUB
{
	WIN_SUB_NONE,
	WIN_SUB_ZIMO,		//自摸
	WIN_SUB_BOMB,		//收炮
	WIN_SUB_ABOMB,		//点炮

	WIN_SUB_DZIMO,		//大胡自摸
	WIN_SUB_DBOMB,		//大胡收炮
	WIN_SUB_DABOMB,		//大胡点炮
};

//起手胡牌类型
enum START_HU_INFO
{
	START_HU_NULL = 0,
	START_HU_QUEYISE=1,   	//缺一色
	START_HU_BANBANHU,		//板板胡
	START_HU_DASIXI,		//大四喜
	START_HU_LIULIUSHUN,	//六六顺
	START_HU_TIANHU,		//天胡
};
//麻将类型
enum GameType
{
	HaiNanTrio = -11,
	ZhuanZhuan = -10,
	GuangDongJiPingHuMaJing = -9,
	HuNanMaJing = -8,
	ChangShaMaJing = -7,
	ShaoYangMaJiang = -6,
	HUNAMPAODEKUAI = -5,
	GuangDongCustomTuiDaoHu = -4,
	HeYuanJokerChickenHu = -3,
	GuangDongOneHundred = -2,
	GuangDongTuiDaoHuMaJing = -1,

	JiNanMaJiang       = 0,      //济南
	LouBaoMaJiang      = 1,	     //山东搂宝
	QingDaoMaJiang	   = 2,	     //山东青岛
	LinYiMajiang       = 3,      //临沂麻将
	HeZeMaJiang        = 4,		 //菏泽麻将
	YanTaiMajiang      = 5,      //烟台麻将
	JiNingMaJiang      = 6,		 //济宁麻将
	TaiAnMaJiang       = 7,      //泰安麻将
	WeiFangMaJiang     = 8,      //潍坊麻将
	ZouChengMaJiang    = 9,	     //邹城麻将
	JiaXiangMaJiang    = 10,	 //嘉祥麻将
	JinXiangMaJiang    = 11,	 //金乡麻将
	LiaoChengMaJiang   = 12,	 //聊城麻将
	TengZhouMaJiang    = 13,	 //滕州麻将
	ZaoZhuangMaJiang   = 14,	 //枣庄麻将
	WeiHaiMaJiang      = 15,	 //威海麻将
	MouPingMajiang     = 16,	 //牟平玩法
	YanZhouMaJiang     = 17,     //兖州麻将
	LinQuMaJiang       = 18,     //临朐麻将
	HuaShuiMaJiang	   = 19,	 //滑水麻将

	CaoXianMaJiang		=20,	 //曹县玩法
	LongKouMajiang		= 21,	 //龙口麻将
	HaiNanMaJiang = 100,  //海南

	
	
	/*********200--250湖北占用*****************/
	HuBeiYiPiLaiMaJiang = 201,   //湖北一痞二癞
	HuBeiEnShiYingMaJiang = 202, //湖北恩施硬麻将
	XianNingMaJiang			= 203, //咸宁红中癞子杠
	TongShanMaJiang			= 204,  //通山麻将
	/****************************************/

	//1000以上为斗地主玩法，1000以下为麻将玩法
	LBase_Dou_Di_Zhu = 1000,			// 斗地主
	LBase_Jing_Dian_Dou_Di_Zhu = 1001,	//经典斗地主,这个必须和LBaseDouDiZhu里的宏的值相同
	LBase_Wei_Fang_Pu_Ke = 2001,
};
//胡牌类型
enum HU_PAI_LEIXING
{
	HU_INVALID = 0,				//没胡
	HU_XIAOHU = 1,				//小胡
	HU_ZIMO = 7,					//自摸
	HU_GANGSHANGKAIHUA = 8,		//杠上开花
	HU_GANGSHANGPAO = 9,			//杠上炮
	HU_HAIDILAO = 10,				//海底捞
	HU_HAIDIPAO = 11,				//海底炮
	HU_QIANGGANGHU = 12,			//抢扛胡
	HU_QISHOUFOURLAN = 13,		//起手四个赖子
	HU_MENQING = 14,				//门清
	HU_SANYUANPAIONE = 15,		//一副中发白刻
	HU_SANYUANPAITWO = 16,		//二副中发白刻
	HU_FENGPAIKEQUAN = 17,		//风牌刻与圈相同
	HU_FENGPAIKEWEI = 18,			//风牌刻与位相同

	HU_JIHU = 101,				//鸡胡
	HU_PING = 102,				//平胡
	HU_PENGPENGHU = 103,		//碰碰胡
	HU_HUNYISE = 104,			//混一色
	HU_BAOHU = 104,
	HU_HUNPENG = 105,			//混碰
	HU_QINGYISE = 106,			//清一色
	HU_QINGPENG = 107,			//清碰
	HU_HUNYAOJIU = 108,			//混幺九
	HU_XIAOSANYUAN = 109,		//小三元
	HU_XIAOSIXI = 110,			//小四喜
	HU_ZIYISE = 111,			//字一色
	HU_QINGYAOJIU = 112,		//清幺九
	HU_DASANYUAN = 113,			//大三元
	HU_DASIXI = 114,			//大四喜
	HU_SHISANYAO = 115,			//十三幺
	HU_HAOHUAQIDUI = 116,		//豪华七对
	HU_QIDUI = 117,				//小七对
	HU_RENHU = 118,				//人胡
	HU_DIHU = 119,				//地胡
	HU_TIANHU = 120,			//天胡
	HU_JIULIANBAODENG = 121,	//九莲宝灯

	HU_CHAOHAOHUAQIDUI = 122,   //超豪华七对
	//HU_FOURGHOST = 122,			//四鬼胡
	HU_YAOJIU = 123,			//幺九
	HU_NOGHOST2TIMES = 124,		//无鬼加番

	HU_FLOWERMATCHSINGLE = 125,	//花吊单
	HU_FLOWERMATCHFLOWER = 126, //花吊花
	HU_GENGZHUANGSUCC = 127,	//跟庄成功

	HU_QING_QIDUI = 128,		//清七对
	HU_QING_HAOHUAQIDUI = 129,	//清豪华七对
	HU_QING_PENGPENGHU = 130,	//清大对
	HU_JIANGYISE = 131,			//将一色
	HU_258JIANGYISE_JINAN = 150, //将一色
	HU_13BUKAO = 151,            //十三不靠
	HU_YITIAOLONG = 152,         //一条龙
	HU_DADIAOCHE = 153,          //大吊车

	HU_DANDIAOJIANG = 154,          //单调将
	HU_HUIJIANG = 155,           //会将
	HU_SHENJIANG = 156,          //神将
	HU_HUHUA = 157,              //胡花
	HU_8HUAHU = 158,             //8花胡
	HU_DANDIAO = 159,             //单调胡
	HU_BIAN    = 160,
	HU_BIANKADIAO = 161,		  //胡边卡掉
	HU_CHAOCHAOHAOHUAQIDUI = 162, //超超豪华七对
	HU_SHUAIHUN = 163,				//甩混
	HU_MAN_HUI_HU = 164,	//满会胡
	HU_YING_HU	  = 165,	//硬胡
	HU_QUAN_QIU_REN = 166,	//全球人
	HU_DUO_HUI_HU = 167,	//多会胡

	///////////////////胡牌类型--湖北占用///////////////////////
	HU_RUANPIHU = 500,                   //软屁胡
	HU_YINGPIHU = 501,                   //硬屁胡
	HU_RUANXIAOXUE = 502,                //软小血
	HU_YINHXIAOXUE = 503,                //硬小血
	HU_RUANQUANQIUREN  = 504,            //软全求人
	HU_YINGQUANQIUREN  = 505,            //硬全求人
	HU_RUANQINGYISE = 506,               //软清一色
	HU_YINGQINGYISE = 507,               //硬清一色
	HU_RUANJIANGYISE = 508,              //软将一色
	HU_YINGJIANGYISE = 509,              //硬将一色
	HU_RUANPENGPENGHU = 510,             //软碰碰胡
	HU_YINGPENGPENGHU = 511,             //硬碰碰胡
	HU_RUANQIDUI      = 512,             //软七对
	HU_YINGQIDUI      = 513,             //硬七对
	HU_RUANHAOHUAQIDUI = 514,            //软豪华七对
	HU_YINGHAOHUAQIDUI = 515,            //硬豪华七对
	HU_RUANCHAOHAOHUAQIDUI = 516,        //软超豪华七对
	HU_YINGCHAOHAOHUAQIDUI = 517,        //硬超豪华七对
	HU_SILAIZIHU           = 518,        //四癞子胡
};

//请求解散房间
enum RESET_ROOM_TYPE
{
	RESET_ROOM_TYPE_WAIT,
	RESET_ROOM_TYPE_AGREE,
	RESET_ROOM_TYPE_REFLUSE,
};

enum  PLAYTYPE
{
	PT_UNDEFINE = -1,
	PT_ZHANIAO0 = 0,
	PT_QIXIAODUI = 5,
	PT_ZHANIAO2 = 2,
	PT_ZHANIAO4 = 4,
	PT_ZHANIAO6 = 6,

	PT_GHOSTCARDNO = 7,
	PT_GHOSTCARDBAIBAN = 8,
	PT_GHOSTCARDFlop = 9,
	PT_REMOVEWINDCARDS = 10,

	PT_ZIMOHU = 11,
	PT_DIANPAOHU = 12,
	PT_KEQIANGGANG = 13,
	PT_HONGZHONG = 14,
	PT_ZHUANGXIAN = 15,			//庄闲

	PT_QIXIAOFUIDOUBLESCORE = 16,
	PT_QIANGGANGHORSERSCORE = 17,
	PT_QIANGGANGHU = 18,

	PT_NOGHOST2TIMESSCORE = 19,
	PT_GANGSHANGKAIHUAQUANBAO = 20,
	PT_FOURGHOSTHU = 21,
	PT_FOURGHOSTHU2TIMESSCORE = 22,
	PT_DUIDUIHU2TIMESSCORE = 23,
	PT_QINGYISE4TIMESSCORE = 24,
	PT_YAOJIU6TIMESSCORE = 25,
	PT_ALLWIND8TIMES = 26,
	PT_SHISANYAO8TIMES = 27,
	PT_GHOSTCARDFLOPTWO = 28,

	PT_GHOSTCARDFLOWER = 29,		//花牌做鬼
	PT_MINGGANGKEQIANG = 30,
	PT_YAOJIU1OR9 = 31,
	PT_QIXIAODUI4TIMESSCORE = 32,
	PT_GENGZHUANG = 33,
	PT_JIANGMA = 34,
	PT_MAGENGDIFEN = 35,
	PT_HONGZHONGZUOMA = 36,
	PT_HONGZHONGJIAFEN = 37,

	//海南麻将
	PT_HAVE_FAN = 101,			//有番
	PT_WU_FAN = 102,			//无番
	PT_LIAN_ZHUANG = 103,		//连庄
	PT_HAVE_GA = 104,			//有噶
	PT_LIU_JU_SUAN_GANG = 105,	//流局算杠(算杠、花胡、首杠、首跟)
	PT_DIAN_PAO_ZI_FU = 106,	//点炮点杠自付
	PT_FENG_YAN = 107,			//风牌可以当眼
	PT_SHUN_HUA_HU = 108,		//顺花胡
	PT_3_NOT_FENG = 109,		//3人玩法无风牌
	PT_3_NOT_EAT = 110,			//3人玩法不可吃
	PT_FREEDOM_GA = 111,		//自由上噶

	//山东搂宝
	PT_258_JIANG = 112,			//258做将
	PT_CAN_EAT = 113,			//可吃
	PT_CAN_JIE_PAO = 114,		//可接炮
	PT_CAN_QIANG_GANG = 115,	//可抢杠
	PT_13_YAO = 116,			//13幺可胡
	PT_DIAN_GANG_KAI_HUA = 117,	//点杠开花（包饺子）
	PT_MAI_3_PIAO = 118,		//买3票
	PT_MAI_5_PIAO = 119,		//买5票
	PT_FAN_XIANG_Multi = 120,	//番相乘
	PT_FAN_XIANG_ADD = 121,		//番相加
	PT_DA_DIAO_CHE = 122,		//大吊车
	PT_AN_LOU = 123,	    	//暗喽


	//济南麻将
	//PT_FengDing32_JiNan = 200,		        //封顶32
	//PT_FengDing64_JiNan = 201,               //封顶64
	//PT_WuFengDing_JiNan = 202,               //不封顶
	PT_258_Jiang_JiNan = 203,		        //258做将
	//PT_Luan_Jiang_JiNan = 204,               //乱将
	//PT_CanChiPai_JiNan = 205,               //可以吃牌
	//PT_WuFengPai_JiNan = 206,               //带风牌
	//PT_MenQing_JiNan = 207,		            //门清
	//PT_DaDiaoChe_JiNan = 208,			    //大吊车
	//PT_ShiSanYao_JiNan = 209,               //十三幺
	//PT_HunYiSe_JiNan = 210,                 //混一色
	//PT_HaiDiLao_JiNan = 211,			    //海底捞月
	//PT_GangBaoBaoPai_JiNan = 212,           //杠爆全包
	PT_TINGCanHu_JiNan = 213,				//报听才可胡
	PT_3_REN_JiNan       = 214,                 //3人玩法
	PT_4_REN_JiNan       = 215,                 //4人玩法
	PT_MO_DAO_DI_JiNan = 216,                 //摸到底
	PT_MING_LOU_JI_NAN		= 217,				//明楼
	PT_BU_DAI_FENG_JI_NAN   = 218,				//不带风
	PT_BU_DAI_HUA_JI_NAN	= 219,				//不带花
	
	   //临沂麻将
	PT_258_Jiang_LinYi     = 300,		        //258做将
	PT_19_Jiang_LinYi      = 301,               //19做将
	PT_Luan_Jiang_LinYi    = 302,               //乱将
	PT_Only_ZiMoHu_LinYi   = 303,               //只能自摸胡
	PT_CanChiPai_LinYi     = 304,               //可以吃牌

	//烟台麻将
	PT_258_Jiang_YanTai = 350,                 //258做将
	PT_Luan_Jiang_YanTai = 351,                //乱将
	PT_Mai_An_Gang = 352,                      //买暗杠
	PT_Can_Not_Chi_YanTai = 353,               //不可以吃
	//PT_Yi_Tiao_Long_YanTai = 354,              //一条龙
	//PT_Da_Diao_Che_YanTai = 355,               //大吊车
	//PT_Qing_YiSe_YanTai = 356,                 //清一色
	PT_3_REN_YanTai     = 357,                 //3人玩法
	PT_4_REN_YanTai     = 358,                 //4人玩法
	PT_MINGLOU_YanTai   = 359,                 //明楼
	PT_XUNI5_YanTai     = 360,                 //虚拟5
	PT_DAIHUI_YanTai    = 361,                 //带会儿牌
	PT_ZIMOHU_YanTai    = 362,                 //自摸胡
	PT_Fan_Xiang_Multi_YanTai = 363,	        //番相乘
	PT_Fan_Xiang_Add_YanTai = 364,		        //番相加
	PT_MAI_3_DAI_PIAO_YanTai = 365,	            //烟台 带漂
	PT_BUDAIFENG_YanTai      = 366,             //不带风
	//青岛麻将
	PT_Have_Hua_QingDao            = 400,		        //有花
	PT_Can_Eat_QingDao             = 401,		        //可吃
	PT_Have_Hui_QingDao            = 402,		        //有会牌
	PT_Dan_Diao_Jiang_QingDao      = 403,		        //单调将
	PT_Yi_Tiao_Long_QingDao        = 404,		        //一条龙
	PT_Hun_Yi_Se_QingDao           = 405,		        //混一色
	PT_Fan_Xiang_Multi_QingDao     = 406,	            //番相乘
	PT_Fan_Xiang_Add1_QingDao      = 407,		        //番相加1分
	//PT_Fan_Xiang_Add10_QingDao     = 408,		        //番相加10分
	PT_Pao_3_Zhang_QingDao         = 409,		        //买3票
	PT_Pao_5_Zhang_QingDao         = 410,		        //买5票
	PT_Yi_Pao_Duo_Xiang            = 411,		        //一炮多响
	PT_NIU_PAI                     = 412,		        //扭牌

	//菏泽麻将
	
	PT_He_Ze_Bao_Zui_1				= 500,  //报嘴
	PT_He_Ze_Bao_Zui_2				= 501,
	PT_He_Ze_Bao_Zui_3				= 502,
	PT_He_Ze_Bao_Zui_4				= 503,  //这几个只能出现一个

	PT_He_Ze_Mai_Zha_Dan			= 504,   //买炸弹
	PT_He_Ze_Mai_Pao_Zi				= 505,   //买跑子
	PT_He_Ze_Dai_Cai				= 506,   //带财
	PT_He_Ze_Mai_Que_Men			= 507,	 //买缺门
	PT_He_Ze_Dai_Que_Men			= 508,	 //带缺门
	PT_He_Ze_Bu_Dai_Feng				= 509,	 //不带风
	PT_Bian_Ka_Diao					= 510,	 //边卡吊
	PT_He_Ze_3_Peo					= 511,
	PT_He_Ze_4_Peo					= 512,
	PT_He_Ze_ZI_MO_HU				= 513,	//自摸胡
	
	//济宁麻将
	PT_3_Pu_Bao_Card_Ji_Ning		= 600, //三扑包胡
	PT_3_Ren_Ji_Ning				= 601, //三人玩法
	PT_4_Ren_Ji_Ning				= 602, //四人玩法

	//泰安麻将
	PT_TAIAN_258_JIANG  = 650,            //258将
	PT_TAIAN_QING_YI_SE = 651,            //清一色
	PT_TAIAN_YI_TIAO_LONG = 652,          //一条龙
	PT_TAIAN_QIDUI        = 653,          //七对
	PT_TAIAN_ZIMOHU       = 654,          //自摸胡
	PT_TIAN_CAN_NOT_EAT	  = 655,		  //吃选项
	PT_TIAN_XIA_PAO		  = 656,			//泰安下跑功能

	//潍坊麻将
	PT_WEIFANG_FENGDING_8FAN = 700,		           //封顶8番
	PT_WEIFANG_FENGDING_16FAN = 701,               //封顶16番
	PT_WEIFANG_FENGDING_NULL = 702,                //不封顶
	PT_WEIFANG_258_JIANG = 703,                   //258将
	PT_WEIFANG_HAVE_HUA  = 704,                   //有花牌
	PT_WEIFANG_ZHUANG_DOUBLE_DI = 705,            //庄家低分翻倍
	PT_WEIFANG_CAN_NOT_EAT = 706,		          //不可吃
	//PT_WEI_FANG_ANLOUHUANPAI  = 707,             //暗楼可换牌
	PT_WEIFANG_GANGBAO         = 708,             //杠爆全包（包饺子）
	PT_WEIFANG_HAVE_HUI        = 709,             //有会牌
	PT_WEIFANG_DAIFENG         = 710,             //带风
	PT_WEIFANG_3_REN           = 711,             //3人玩法
	PT_WEIFANG_BAOTING         = 722,             //报听可胡
	//邹城玩法
	PT_3_Ren_Zou_Cheng				= 801, //三人玩法
	PT_4_Ren_Zou_Cheng				= 802, //四人玩法
	PT_Zou_Cheng_Ming_Lou_Hu		= 803, //必须明楼胡

	//嘉祥玩法
	PT_3_Ren_Jia_Xiang				= 901, //三人玩法
	PT_4_Ren_Jia_Xiang				= 902, //四人玩法

	//金乡玩法
	PT_3_Ren_Jin_Xiang				= 1001, //三人玩法
	PT_4_Ren_Jin_Xiang				= 1002, //四人玩法

	//聊城玩法
	PT_LIAO_CHENG_MING_DA			= 1101,	//明打
	//滕州玩法
	PT_3_Ren_Teng_Zhou				= 1201,	//三人局
	PT_4_Ren_Teng_Zhou				= 1202,	//四人局

	//枣庄玩法
	PT_3_Ren_Zao_Zhuang				= 1301, //三人局
	PT_4_Ren_Zao_Zhuang				= 1302, //四人局

	//威海玩法
	PT_3_Ren_Wei_Hai				= 1401, //三人局
	PT_4_Ren_Wei_Hai				= 1402, //四人局

	PT_Pao_Fen_Wei_Hai				= 1403, //跑分
	PT_Ming_Lou_Wei_Hai				= 1404, //明楼
	PT_Chang_Mao_Wei_Hai			= 1405, //长毛
	PT_Luan_Mao_Wei_Hai				= 1406, //乱毛
	PT_Jiang_258_Wei_Hai			= 1407, //258做将
	PT_Can_Not_Chi_Wei_Hai			= 1408, //不可吃
	PT_Zi_Mo_Hu_Wei_Hai				= 1409, //自摸胡

	//牟平麻将
	
	PT_Can_Not_Chi_MuPing = 1501,                 //不可吃          
	PT_3_REN_MouPing     = 1502,                 //3人玩法
	PT_4_REN_MouPing     = 1503,                 //4人玩法
	     
	PT_Pao_Fen_MuPing   = 1505,				    //买跑分
	PT_DAIHUI_MuPing    = 1506,                 //带会儿牌
	PT_ZIMOHU_MuPing    = 1507,                 //自摸胡
	PT_XiangCheng_MuPing    = 1508,                 //翻数相乘
	PT_XiangJia_MuPing    = 1509,                 //翻数加
	PT_BUDaiFeng_MuPing     = 1510,					//带风选项

	//兖州麻将
	PT_3_Ren_Yan_Zhou = 1601, //三人玩法
	PT_4_Ren_Yan_Zhou = 1602, //四人玩法
	PT_DAI_HUA_Yan_Zhou = 1603, //带花
	PT_BU_KE_CHI_Yan_Zhou = 1604, //不可吃
	PT_MING_LOU_Yan_Zhou = 1605, //明楼
	PT_ShiSanBuKao_Yan_Zhou = 1606, //十三不靠

	//临朐麻将
	PT_LINQU_FENGDING_8FAN = 1700,		           //封顶8番
	PT_LINQU_FENGDING_16FAN = 1701,                  //封顶16番
	PT_LINQU_FENGDING_NULL = 1702,                   //不封顶
	PT_LINQU_258_JIANG = 1703,                       //258将
	//PT_WEIFANG_HAVE_HUA = 704,                         //有花牌
	PT_LINQU_ZHUANG_DOUBLE_DI = 1705,                 //庄家低分翻倍
	PT_LINQU_CAN_NOT_EAT = 1706,		               //不可吃
	//PT_WEI_FANG_ANLOUHUANPAI  = 707,                 //暗楼可换牌
	PT_LINQU_GANGBAO = 1708,                          //杠爆全包（包饺子）
	PT_LINQU_HAVE_HUI = 1709,                         //有会牌
	PT_LINQU_DAIFENG = 1710,                          //带风
	PT_LINQU_3_REN = 1711,                            //3人玩法
	PT_LINQU_BAOTING = 1722,                          //报听可胡

	//滑水麻将
	//鱼分数0-10
	PT_HUA_SHUI_Yu_Score_0	= 1800,
	PT_HUA_SHUI_Yu_Score_1	= 1801,
	PT_HUA_SHUI_Yu_Score_2	= 1802,
	PT_HUA_SHUI_Yu_Score_3	= 1803,
	PT_HUA_SHUI_Yu_Score_4	= 1804,
	PT_HUA_SHUI_Yu_Score_5	= 1805,
	PT_HUA_SHUI_Yu_Score_6	= 1806,
	PT_HUA_SHUI_Yu_Score_7	= 1807,
	PT_HUA_SHUI_Yu_Score_8	= 1808,
	PT_HUA_SHUI_Yu_Score_9	= 1809,
	PT_HUA_SHUI_Yu_Score_10 = 1810,

	PT_HUA_SHUI_258_Jiang	= 1811,		//258将
	PT_HUA_SHUI_Dai_Feng	= 1812,		//带风
	PT_HUA_SHUI_Hong_Zhong_Lai_Zi	= 1813,		//红中赖子
	PT_HUA_SHUI_Zi_Mo_Hu	= 1814,				//自模糊
	PT_HUA_SHUI_Dan_Yu		= 1815,			//单鱼
	PT_HUA_SHUI_Yu_Chi_Yu	= 1816,			//鱼吃鱼

	//曹县玩法

	PT_CAO_XIAN_BIAN_KA_DIAO	= 1901,					//边卡吊
	PT_CAO_XIAN_BU_DAI_FENG		= 1902,					//不带风
	PT_CAO_XIAN_CAI_SHEN_4		= 1903,					//4神
	PT_CAO_XIAN_CAI_SHEN_8		= 1904,					//8神
	PT_CAO_XIAN_ZI_MO_HU		= 1905,					//自摸胡

	PT_CAO_XIAN_Bao_Zui_1		= 1906,  //报嘴
	PT_CAO_XIAN_Bao_Zui_2		= 1907,
	PT_CAO_XIAN_Bao_Zui_3		= 1908,

	PT_CAO_XIAN_3_Peo			= 1909,					//三人局
	PT_CAO_XIAN_4_Peo			= 1910,	//四人局

	PT_CAO_XIAN_Buy_Que_Men		= 1911,  //买缺门

	//龙口麻将
	PT_LONG_KOU_3_PEO			=2001,
	PT_LONG_KOU_4_PEO			=2002,
	PT_LONG_KOU_DAI_HUI			= 2003,  //带会牌
	PT_LONG_KOU_ZI_MU_HU		= 2004,  //自摸胡
	PT_LONG_KOU_BU_KE_CHI		= 2005,	 //不可吃
	PT_LONG_KOU_PAO_FEN			= 2006,	 //买跑分
	PT_LONG_KOU_ZHUANG_FAN_FAN			= 2007,	 //庄翻番

	/*******************湖北占用****************/

	//恩施一痞二赖
	PT_ENSHI_YIPILAI_GANGSHANGPAO = 3001, //恩施一痞二赖--杠上炮
	PT_ENSHI_YIPILAI_JINZHIYANGPI = 3002, //恩施一痞二赖--禁止养痞

	//恩施硬麻将
	PT_ENSHI_YINGMJ_TAI_ZHUANG    = 3101, //恩施硬麻将--抬庄
	PT_ENSHI_YINGMJ_PENG_PENG_HU  = 3102, //恩施硬麻将--碰碰胡
	PT_ENSHI_YINGMJ_QUANQIUREN    = 3103, //恩施硬麻将--全求人
	PT_ENSHI_YINGMJ_BAOSANJIA     = 3104, //恩施硬麻将--包三家

	//咸宁麻将
	PT_XIAN_NING_DAI_BAIBAN_FACAI  = 4001,
	PT_XIAN_NING_3_FAN_HU			= 4002,
	PT_XIAN_NING_4_FAN_HU			= 4003,
	PT_XIAN_NING_SHUANG_DA_HU_4_FAN = 4004,	//双大胡四番
	PT_XIAN_NING_ZHI_GANG_QIANG_GANG = 4005,	//直杠抢杠
	PT_XIAN_NING_TONGSHAN			 = 4006,	//咸宁通山玩法

	PT_XIAN_NING_1_BEI_SCORE		= 4007,	//		1倍分
	PT_XIAN_NING_2_BEI_SCORE		= 4008,	//		2倍分
	PT_XIAN_NING_3_BEI_SCORE		= 4009,	//		3倍分
	PT_XIAN_NING_5_BEI_SCORE		= 4010,	//		5倍分

};


enum TYPE_HeZe_BuyItem
{
	HeZe_Buy_Min	= 0,
	HeZe_Buy_Zha_Dan = 1,	//菏泽买炸弹
	HeZe_Buy_Pao_Zi = 2,	//菏泽买跑子
	HeZe_Buy_Max,
};

enum ErrorCode
{
	ErrorNone = 0,
	UserNotExist = 1,
	TaskNotExist = 2,
	ExceedMaximum = 3,
	AlreadyBinded = 4,
	NotQualified = 5,
	TaskNotFinished = 6,
	Expired = 7,
	BindingSelf = 8,
	IviterRegistRecently = 9,
	IviteeRegistLongTimeAgo = 10,
	CannotInviteEachother = 11,
	OutOfMoney = 12,
	OutOfChance = 13,
	NotBind = 14,
	Unknown = 100
};

enum EXCLUSIVE_PLAY
{
	EP_DOU_DI_ZHU = 1,		//斗地主专属玩法，这个对应着玩法服务器的配置文件的ExclusivePlay
};
//拆包的ID
enum APP_ID
{
	APP_MIAN_PACKET		= 33001,
	APP_YANTAI_PACKET	= 33002,
	APP_WEI_FANF_PACKET = 33003,
};

enum MaoType
{
	MT_NULL = 0,
	MT_FENG = 1,   //风牌组成
	MT_JIAN = 2,	//中发白组成的
	MT_ONE  = 3,	//一万一桶一条组成的
	MT_NINE	= 4,	//九万九条九通
	MT_MAX,			
};

// 支付类型
enum PAY_TYPE
{
	PT_ONE_PAY = 0,		// 房主支付
	PT_ALL_PAY = 1,		// 所有人AA
};

enum SDK_Reward_Type
{
	Reward_Type_Card = 1,		// 房卡
	Reward_Type_RedPacket = 2,	// 红包
	Reward_Type_Coupon = 3,		// 参赛券
	Reward_Type_Object = 4,		// 实物
	Reward_Type_Jewel = 5,		// 钻石
	Reward_Type_Gold = 6,		// 金币
};

#endif