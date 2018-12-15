#pragma once

#include "base.h"
#include "LBase.h"
#include "vector"
#include "map"
#include "LMsg.h"


class Desk;
class ChessBlock;

struct ChessAttackData
{
	Lint	m_attack_block_id;
	Point	m_get_damage_pos;

	void Clear()
	{
		m_attack_block_id = 0;
		m_get_damage_pos.m_posX = 0;
		m_get_damage_pos.m_posY = 0;
	}
};

class ChessFighter
{
public:
	Lint		m_user_id;
	Lint		m_money;
	Lint		m_shou;
};

class ChessGame
{
public:
	ChessGame();
	~ChessGame();

public:
	void Start(Desk* desk);
	void Init();
	
	void InitChessPos();
	void CheckMove(std::shared_ptr<ChessBlock>& pBlock, const Point& to_point);
	void Attack(const Point& from_point, const Point& to_point);
	bool IsHaveBlock(const Point& pos);

public:
	void HanderChessMove(LMsgC2SChessMove* msg);
	void HanderChessAttack(LMsgC2SChessAttack* msg);
	void HanderChessUpgrade(LMsgC2SChessUpgrade* msg);
	void FillBlockMsg(BlockMsg& msg, const std::shared_ptr<ChessBlock> block);
	void SendAllChessInfoToUser(Lint user_id);

private:
	bool IsChessPos(const Point& pt);
	void StepEnd();
	bool DestoryBolck(std::shared_ptr<ChessBlock> pBlock);
	
	void Move(std::shared_ptr<ChessBlock>& pBlock, const Point& pos);
	//这个就是限定在垂直和水平上
	bool IsHaveOneBlock(const Point& from_point, const Point& to_point);
	void GameOver();

	CHESS_COLOR GetUserColor(Lint user_id);
	std::shared_ptr<ChessBlock> GetBlockById(Lint block_id);
	bool CostCoin(CHESS_COLOR color, int cost);
	void AddAttackData(Lint attack_block_id, Point get_damage_pos);
	void ClearAttackData();
private:
	int m_Chess[POS_LIMIT_X][POS_LIMIT_Y];
	int														m_Color;   
	std::map<int, std::shared_ptr<ChessBlock>>				m_map_blocks;
	std::map<CHESS_COLOR, std::shared_ptr<ChessFighter> >	m_map_fighters;
	Desk*													m_desk;
	ChessAttackData											m_attack_data;
};

