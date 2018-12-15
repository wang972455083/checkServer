#ifndef _LVIP_LOG_H
#define _LVIP_LOG_H

#include "LTool.h"
#include "LBuff.h"
#include "GameDefine.h"

struct VipDeskLog
{
	Lstring	 m_videoId;//录像id
	Lint	 m_gold[DESK_USER_COUNT];
	Lint	 m_win[DESK_USER_COUNT];
	Lint	 m_zhuangPos;				//坐庄的位置
	Lint	 m_time;					//本局时间
	Lint	 m_angang[DESK_USER_COUNT]; //暗杠
	Lint	 m_mgang[DESK_USER_COUNT];  //明杠
	Lint	 m_bomb[DESK_USER_COUNT];	//点炮者的放炮信息
	Lint	 m_zimo[DESK_USER_COUNT];	//自摸

	VipDeskLog()
	{
		memset(m_gold, 0, sizeof(m_gold));
		memset(m_win, 0, sizeof(m_win));
		memset(m_angang, 0, sizeof(m_angang));
		memset(m_mgang, 0, sizeof(m_mgang));
		memset(m_bomb, 0, sizeof(m_bomb));
		memset(m_zimo, 0, sizeof(m_zimo));
		m_zhuangPos = INVAILD_POS;
		m_time = 0;
	}
};

struct LVipLogItem
{
	Lstring			m_id;		//id
	Lint			m_time;		//时间
	Lint			m_flag;		//房间玩法
	Lint            m_state;	//房间圈数
	Lint			m_deskId;	//桌子id
	Lstring			m_secret;	//密码
	Lint			m_maxCircle;//总圈数
	Lint			m_curCircle;//当前圈数
	Lint			m_posUserId[DESK_USER_COUNT];//0-3各个位置上的玩家id
	Lint			m_curZhuangPos;//当前庄家
	Lint			m_score[4];    // 各个位置上面的积分
	Lint			m_reset;       //是否解算
	std::vector<Lint> m_playtype;		//玩法列表
	std::vector<VipDeskLog*> m_log;//每一把的记录
	Lint			m_appId;

	LVipLogItem();
	virtual  ~LVipLogItem();
	virtual  Lstring		ToString();

	virtual  void			FromString(Lstring str);

	virtual  Lstring		PlayTypeToStrint();
	virtual  Lstring        PlayTypeToString4RLog();

	virtual  void			PlayTypeFromString(Lstring str);
};
#endif