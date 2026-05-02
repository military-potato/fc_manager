#include <iostream>
#include <string>
#include "player.h"
using namespace std;


class Team
{
    public:
        string TeamName;
        Player* lineup[10];
    
    Team(string name) : TeamName(name)
    {
        for (int i = 0; i < 10; i++)
        {
            lineup[i] = NULL;
        }
    }
    void setLineup(Player* player)
    {
        
    }
} 