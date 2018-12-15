#include "TableManager.h"
#include "Table/ChessBoardCfg.h"


bool TableManager::Init()
{
	return true;
}


bool TableManager::Final()
{
	return true;
}

void TableManager::Load()
{
	ChessBoardCfg::Instance().LoadTbl();
}
