#ifndef _L_CARDS_OPER_TYPE_H_
#define _L_CARDS_OPER_TYPE_H_

enum CARDS_OPER_TYPE
{
	CARDS_OPER_TYPE_INIT								= 0,        // 新手赠送
	CARDS_OPER_TYPE_RECHARGE							= 1,        // 充值
	CARDS_OPER_TYPE_FREE_GIVEN							= 2,        // 免费赠送        
	CARDS_OPER_TYPE_ROOM_CHARGE_BY_OWNER				= 3,        // 房主支付房费
	CARDS_OPER_TYPE_ROOM_CHARGE_BY_AA					= 4,        // AA制支付房费
	CARDS_OPER_TYPE_ROOM_CHARGE_BY_WINNER				= 5,        // 大赢家支付房费
	CARDS_OPER_TYPE_ACTIVITY							= 6,        // 活动送房卡
	CARDS_OPER_TYPE_PURCHASE_GIVEN						= 7,        // 购买道具赠送
	CARDS_OPER_TYPE_VIP_GIVE							= 8,        // 购买道具VIP额外赠送
	CARDS_OPER_TYPE_INVITING							= 9,        // 邀请好友送房卡
	CARDS_OPER_TYPE_ACCEPT_INVITING						= 10,       // 接受邀请送房卡
	CARDS_OPER_TYPE_PLAYING								= 11,       // 完成牌局送房卡、完成牌局开启福袋获得房卡
	CARDS_OPER_TYPE_SHARE								= 12,       // 分享送房卡
	CARDS_OPER_TYPE_SIGNIN								= 13,       // 签到送房卡
	CARDS_OPER_TYPE_REPLENISH_SIGNIN					= 14,       // 补签到扣房卡
	CARDS_OPER_TYPE_TASK								= 15,       // 完成任务送房卡
	CARDS_OPER_TYPE_RANKING								= 16,       // 排行榜送房卡
	CARDS_OPER_TYPE_RECRUIT_NEW_PLAYER					= 17,       // 拉新送房卡
	CARDS_OPER_TYPE_OLD_PLAYER_BACK						= 18,       // 老玩家回流送房卡
	CARDS_OPER_TYPE_VOTE								= 19,       // 参与投票送房卡
	CARDS_OPER_TYPE_BIND_MOBILE							= 20,       // 绑定手机号送房卡
	CARDS_OPER_TYPE_BIND_AGENT							= 21,       // 绑定推广员送房卡
	CARDS_OPER_TYPE_SPECIAL_CARD_STYLE					= 22,       // 特殊牌型送房卡
	CARDS_OPER_TYPE_EXCHANGE_LUCKY_DRAW_QUALIFICATION   = 23,       // 抽奖或者转盘消耗
	CARDS_OPER_TYPE_LUCKY_DRAW                          = 24,       // 抽奖或者转盘获得房卡
	CARDS_OPER_TYPE_EXCHANGE_GOLDEN_COIN                = 25,       // 兑换金币
	CARDS_OPER_TYPE_EXCHANGE_BY_GOLDEN_COIN             = 26,       // 金币兑换房卡
	CARDS_OPER_TYPE_EXCHANGE_BY_GIFT_CERTIFICATE        = 27,       // 礼券购房卡
	CARDS_OPER_TYPE_MATCH_ENTRY                         = 28,       // 比赛场报名扣卡
	CARDS_OPER_TYPE_WIN_MATCH                           = 29,       // 比赛场赢房卡
	CARDS_OPER_TYPE_ENTER_MATCH_GIVEN                   = 30,       // 参与比赛场送房卡
	CARDS_OPER_TYPE_QUIT_MATCH_RETURN                   = 31,       // 比赛场退赛报名费退还
	CARDS_OPER_TYPE_RELIEF_IN_MATCH                     = 32,       // 比赛场房卡输光后救济赠送
	CARDS_OPER_TYPE_PRICE_ADJUSTMENT_COMPENSATION       = 100,      // 价格调整补偿
	CARDS_OPER_TYPE_ACCIDENT_COMPENSATION               = 101,      // 事故补偿
	CARDS_OPER_TYPE_REWARD_BY_GM                        = 102,      // 平台奖励
	CARDS_OPER_TYPE_DEDUCTION_BY_GM                     = 103,      // 平台扣卡
};

enum TEAM_CARDS_OPER_TYPE
{
	TCARDS_OPER_TYPE_RECHARGE_BY_CREATOR = 0,      // 工会创建者充卡
	TCARDS_OPER_TYPE_ROOM_CHARGE_BY_OWNER = 1,      // 工会成员创建房间扣卡
};

#endif  // _L_CARDS_OPER_TYPE_H_