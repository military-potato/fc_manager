#include <string>
using namespace std;

class player
{
    public:
        string name;
        int attack;
        int middle;
        int defense;
    
        Player(string n, int atk, int mid, int def) : name(n), attack(atk), middle(mid), defense(def) {}
};