#include<iostream>
#include<vector>
#include<tuple>
#include<queue>
#include<string>

using Point = std::pair<int, int>;
const int NONE = -1;
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
enum Object
{
    WALL = 0,
    ROCK = 1
};
const int NUM_OF_CELL_KIND = 5;
int remainingTime;
std::vector<int> skillCost;

class Points
{
public:
    std::vector<std::vector<int>> id;
    std::vector<Point> point;
    Points() { id.clear(); point.clear(); }
    Points(int height, int width,int num) :id(height, std::vector<int>(width, NONE)), point(num) {}
};

class State
{
public:
    int skillPoint;
    int Height, Width;
    Points dogs;
    Points objects;
    Points ninjas;
    Points souls;
    std::vector<int> skillCount;
    
    State() {}

    void input(int numOfSkills)
    {
        using std::cin;
        cin >> skillPoint;

        int Height, Width;
        cin >> Height >> Width;
        this->Height = Height;
        this->Width = Width;

        objects = Points(Height, Width,0);
        for (int row = 0; row < Height; row++)
        {
            for (int col = 0; col < Width; col++)
            {
                char c; cin >> c;
                if (c == 'W') objects.id[row][col] = WALL;
                if (c == 'O') objects.id[row][col] = ROCK;
            }
        }
        
        int numNinjas;
        cin >> numNinjas;
        ninjas = Points(Height, Width,numNinjas);
        for (int i = 0; i < numNinjas; i++)
        {
            int id, row, col;
            cin >> id >> row >> col;
            ninjas.id[row][col] = id;
            ninjas.point[id] = std::make_pair(row, col);
        }

        int numDogs;
        cin >> numDogs;
        dogs = Points(Height, Width, numDogs);
        for (int i = 0; i < numDogs; i++)
        {
            int id, row, col;
            cin >> id >> row >> col;
            dogs.id[row][col] = id;
            dogs.point[id] = std::make_pair(row, col);
        }

        int numSouls;
        cin >> numSouls;
        souls = Points(Height, Width, numSouls);
        for (int i = 0; i < numSouls; i++)
        {
            int row, col;
            cin >> row >> col;
            souls.id[row][col] = 0;
            souls.point[i] = std::make_pair(row, col);
        }

        for (int i = 0; i < numOfSkills; i++)
        {
            int count; cin >> count;
            skillCount.push_back(count);
        }
    }
};

std::vector<int> dRow = { -1, 0, 0, 1, 0 };
std::vector<int> dCol = {  0,-1, 1, 0, 0 };
std::vector<std::string> d = { "U","L","R","D","N" };
int getMinDist(State st,Point p1,Point p2)
{
    std::queue<Point> q;
    Points dist(st.Height, st.Width, 0);
    q.push(p1);
    dist.id[p1.first][p1.second] = 0;
    while (q.front() != p2)
    {
        int curRow, curCol;
        curRow = q.front().first;
        curCol = q.front().second; q.pop();
        for (int i = 0; i < 4; i++)
        {
            int nextRow, nextCol;
            nextRow = curRow + dRow[i];
            nextCol = curCol + dCol[i];
            if (nextRow == -1 || nextCol == -1 || nextRow == st.Height || nextCol == st.Width) continue;
            if (dist.id[nextRow][nextCol] != NONE) continue;
            if (st.dogs.id[nextRow][nextCol] != NONE)
            {
                dist.id[nextRow][nextCol] = INT_MAX/2;
                continue;
            }
            q.push(std::make_pair(nextRow, nextCol));
            dist.id[nextRow][nextCol] = dist.id[curRow][curCol] + 1;
        }
        if (q.empty()) return INT_MAX;
    }
    int minDist = dist.id[p2.first][p2.second];
    return dist.id[p2.first][p2.second];
}

State myState;
State rivalState;
void thinkAndRun()
{
    using std::cout;
    using std::endl;
    using std::string;
    std::vector<string> nextDir(2);
    int maxStep = 2;
    for (int ninjaId = 0; ninjaId < myState.ninjas.point.size(); ninjaId++)
    {
        for (int step = 0; step < maxStep; step++)
        {
            int min = INT_MAX;
            int numDir = 4;
            int numSoul = 0;
            for (int soulId = 0; soulId < myState.souls.point.size(); soulId++)
            {
                for (int dir = 0; dir < 4; dir++)
                {
                    int cur = INT_MAX;
                    int nextRow = myState.ninjas.point[ninjaId].first + dRow[dir];
                    int nextCol = myState.ninjas.point[ninjaId].second + dCol[dir];
                    if (!(myState.dogs.id[nextRow][nextCol] != NONE || myState.objects.id[nextRow][nextCol] == WALL ||
                                                                       (myState.objects.id[nextRow][nextCol] == ROCK &&
                                                                        (myState.objects.id[nextRow + dRow[dir]][nextCol + dCol[dir]] != NONE ||
                                                                         myState.dogs.id[nextRow + dRow[dir]][nextCol + dCol[dir]] != NONE ||
                                                                         myState.ninjas.id[nextRow + dRow[dir]][nextCol + dCol[dir]] != NONE))))
                        cur = getMinDist(myState, std::make_pair(nextRow, nextCol), myState.souls.point[soulId]);
                    if (min > cur)
                    {
                        min = cur;
                        numSoul = soulId;
                        numDir = dir;
                    }
                }
            }
            int curRow = myState.ninjas.point[ninjaId].first + dRow[numDir];
            int curCol = myState.ninjas.point[ninjaId].second + dCol[numDir];
            myState.ninjas.point[ninjaId] = std::make_pair(curRow, curCol);
            if (myState.souls.point[numSoul] == std::make_pair(curRow, curCol))
                myState.souls.point.erase(myState.souls.point.begin() + numSoul);
            nextDir[ninjaId] += d[numDir];
        }
    }
    cout << 2 << endl;
    cout << nextDir[0] << endl;
    cout << nextDir[1] << endl;
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

    myState.input(numOfSkills);
    rivalState.input(numOfSkills);
    return true;
}

int main()
{
    using namespace std;
    cout << "NINJAROCK" << endl;
    cout.flush();

    while (input())
    {
        thinkAndRun();
        cout.flush();
    }
}