#include<iostream>
#include<vector>

enum JITSUName
{
    SYUNSIN = 0,
    IWA_GAKURE = 1,
    IWA_OTOSHI = 2,
    KAMINARI_GAKURE = 3,
    KAMINARI_OTOSHI = 4,
    BUNSIN = 5,
    YUUDOU = 6,
    TENSOUMETSU = 7
};
enum CellKind
{
    WALL = 0,
    OBJECT = 1,
    NINJA = 2,
    DOG = 3
};
const int NUM_OF_CELL_KIND = 4;
int remainingTime;
std::vector<int> skillCost;

class Point 
{
public:
    int x, y;
    Point() { x = y = -1; }
    Point(int x, int y) : x(x), y(y) {}
    bool operator== (const Point &p) const { return x == p.x && y == p.y; }
};

class Cell
{
public:
    Point point;
    std::vector<bool> inclusions;
    Cell() :inclusions(NUM_OF_CELL_KIND) {}
};

class State
{
public:
    int skillPoint;
    int H, W;
};
void think()
{

}

bool input()
{
    using namespace std;
    if (!(cin >> remainingTime)) return false;

    int numOfSkills;
    cin >> numOfSkills;
    
    skillCost.clear();
    for (int i = 0; i < numOfSkills; i++)
    {
        int cost;
        cin >> cost;
        skillCost.push_back(cost);
    }


}

int main()
{
    using namespace std;
    cout << "NINJAROCK" << endl;
    cout.flush();

    while (input())
    {
        think();
        cout.flush();
    }
}