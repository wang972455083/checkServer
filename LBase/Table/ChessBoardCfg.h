#pragma once
#include <map>
#include <vector>
#include "../LSingleton.h"

using namespace std;


class ChessBoardTbl
{
public:
	ChessBoardTbl();
	~ChessBoardTbl();

public:
	int		m_id;
	int		m_type;
	int		m_min_hp;
	int		m_max_hp;
	int		m_min_attack;
	int		m_max_attack;
	int		m_min_coin_level;
	int		m_max_coin_level;
	
	int		m_upgrade_hp_cost;
	int		m_upgrad_attack_cost;
	vector<int> m_upgrade_coin_level_cost;
	int		m_grade;
};

class ChessBoardCfg : public LSingleton<ChessBoardCfg>
{
	
public:
	bool Init();
	bool Final();

	

	bool			LoadTbl();
	void			ClearTbl();

	ChessBoardTbl*	GetChessBoard(int chess_id);

	
protected:
	static const char*		szChessBoardFile;
	map<int, ChessBoardTbl*> m_mapBoards;
};
