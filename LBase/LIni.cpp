#include "LIni.h"


#define MAX_LINE_CHAR 256


LIniConfig::LIniConfig()
{

}

LIniConfig::LIniConfig(Lstring file)
{
	LoadFile(file);
}

void LIniConfig::LoadFile(Lstring file)
{
	m_ini_map.clear();
	char line_content[MAX_LINE_CHAR];
	std::ifstream ifs(file.c_str());
	while(ifs)
	{
		ifs.getline(line_content,MAX_LINE_CHAR);
		std::string s = line_content;
		std::string::iterator it = s.begin();
		for(;it != s.end();)
		{
			if((*it) == ' ' || (*it) == '\t')
			{
				it = s.erase(it);
			}
			else if((*it) == ';' || (*it) == '#')
			{
				it = s.erase(it,s.end());
			}
			else
			{
				++it;
			}
		}
		int pos = s.find('='); 
		if(pos != std::string::npos)
		{
			m_ini_map[s.substr(0,pos)]=s.substr(pos+1,s.size());
		}
	}
	ifs.close();
}


LIniConfig::~LIniConfig()
{

}

Lstring	LIniConfig::GetString(std::string key)
{
    if (m_ini_map.count(key))
    {
        return m_ini_map[key];
    } 
    else
    {
        return "";
    }
}

Lstring	LIniConfig::GetString(std::string key,std::string def)
{
    if (m_ini_map.count(key))
    {
        return m_ini_map[key];
    } 
    else
    {
        return def;
    }
}

Lint LIniConfig::GetInt(Lstring key)
{
    std::string map_value=GetString(key);
    if (map_value.size())
    {
        return atoi(map_value.c_str());
    }
    else
    {
        return 0;
    }
}

Lint LIniConfig::GetInt(std::string key,Lint def)
{
    std::string map_value=GetString(key);
    if (map_value.size())
    {
        return atoi(map_value.c_str());
    }
    else
    {
        return def;
    }
}

Lshort LIniConfig::GetShort(std::string key)
{
    std::string map_value=GetString(key);
    if (map_value.size())
    {
        return (Lshort)atoi(map_value.c_str());
    }
    else
    {
        return 0;
    }
}

Lshort LIniConfig::GetShort(std::string key,Lshort def)
{
    std::string map_value=GetString(key);
    if (map_value.size())
    {
        return (Lshort)atoi(map_value.c_str());
    }
    else
    {
        return def;
    }
}

Lchar LIniConfig::GetChar(std::string key)
{
    std::string map_value=GetString(key);
    if (map_value.size())
    {
        return (Lchar)atoi(map_value.c_str());
    }
    else
    {
        return 0;
    }
}

Lchar LIniConfig::GetChar(std::string key,Lchar def)
{
    std::string map_value=GetString(key);
    if (map_value.size())
    {
        return (Lchar)atoi(map_value.c_str());
    }
    else
    {
        return def;
    }
}