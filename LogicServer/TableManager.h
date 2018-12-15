#pragma once
#include "LSingleton.h"

class TableManager : public LSingleton<TableManager>
{
public:
	bool Init();
	bool Final();

public:
	void	Load();
};

