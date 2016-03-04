#include<iostream>
#include<vector>
#include<tuple>
#include<queue>
#include<string>
#include<map>

const int NONE = -1;
const int INFTY = 100000000;
enum JITSUName
{
    MU = -1,
    SYUNSIN = 0,
    IWA_GAKURE = 1,
    IWA_OTOSHI = 2,
    KAMINARI_GAKURE = 3,
    KAMINARI_OTOSHI = 4,
    BUNSIN = 5,
    YUUDOU = 6,
    TENSOUMETSU = 7,
};
const int NUM_OF_SKILLS = 8;
enum Object
{
    WALL = 0,
    ROCK = 1
};
const int NUM_OF_CELL_KIND = 5;
enum Direction
{
    UP = 0,
    LEFT = 1,
    RIGHT = 2,
    DOWN = 3,
    NOMOVE = 4
};
std::vector<int> dRow = { -1, 0, 0, 1, 0 };
std::vector<int> dCol = { 0,-1, 1, 0, 0 };
std::vector<std::string> d = { "U","L","R","D","N" };
int remainingTime;
std::vector<int> skillCost;

class Point
{
public:
    int row;
    int col;
    Point(int row, int col) { this->row = row; this->col = col; }
    bool operator== (const Point& p){ return row == p.row && col == p.col; }
};
class Points
{
public:
    std::vector<std::vector<int>> field;
    std::vector<int> ids;
    Points() { field.clear(); }
    Points(int height, int width,int num) :field(height, std::vector<int>(width, NONE)){}
    Point getPoint(int id)
    {
        for (int row = 0; row < field.size(); row++)
        {
            for (int col = 0; col < field[row].size(); col++)
            {
                if (id == field[row][col]) return Point(row, col);
            }
        }
        return Point(NONE, NONE);
    }
};

class State
{
public:
    int skillPoint;
    int Height, Width;
    int remSteps;
    Points dogs;
    Points objects;
    Points ninjas;
    Points souls;
    Points ninjaSmells;
    std::vector<int> skillCount;
    
    State() {}

    void input(int numOfSkills)
    {
        using std::cin;

        remSteps = 2;

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
                if (c == 'W') objects.field[row][col] = WALL;
                if (c == 'O') objects.field[row][col] = ROCK;
            }
        }
        
        int numNinjas;
        cin >> numNinjas;
        ninjas = Points(Height, Width,numNinjas);
        ninjaSmells = Points(Height, Width, numNinjas);
        for (int i = 0; i < numNinjas; i++)
        {
            int id, row, col;
            cin >> id >> row >> col;
            ninjas.field[row][col] = id;
            ninjas.ids.push_back(id);
        }

        int numDogs;
        cin >> numDogs;
        dogs = Points(Height, Width, numDogs);
        for (int i = 0; i < numDogs; i++)
        {
            int id, row, col;
            cin >> id >> row >> col;
            dogs.field[row][col] = id;
            dogs.ids.push_back(id);
        }

        int numSouls;
        cin >> numSouls;
        souls = Points(Height, Width, numSouls);
        for (int i = 0; i < numSouls; i++)
        {
            int row, col;
            cin >> row >> col;
            souls.field[row][col] = i;
            souls.ids.push_back(i);
        }

        for (int i = 0; i < numOfSkills; i++)
        {
            int count; cin >> count;
            skillCount.push_back(count);
        }
    }
};

Points getMinDist(State st,Point p,Points dist)
{
    std::queue<Point> q;
    q.push(p);
    dist.field[p.row][p.col] = 0;
    while (!(q.empty()))
    {
        int curRow, curCol;
        curRow = q.front().row;
        curCol = q.front().col; q.pop();
        for (int i = 0; i < 4; i++)
        {
            int nextRow, nextCol;
            nextRow = curRow + dRow[i];
            nextCol = curCol + dCol[i];
            if (nextRow == -1 || nextCol == -1 || nextRow == st.Height || nextCol == st.Width) continue;
            if (dist.field[nextRow][nextCol] != NONE) continue;
            q.push(Point(nextRow, nextCol));
            dist.field[nextRow][nextCol] = dist.field[curRow][curCol] + 1;
        }
    }
    for (int row = 0; row < st.Height; row++)
    {
        for (int col = 0; col < st.Width; col++)
        {
            if (dist.field[row][col] == NONE) dist.field[row][col] = INFTY;
        }
    }
    return dist;
}

State myState;
State rivalState;

std::tuple<State,State,JITSUName,Point,std::string,std::string> predict(State mySt,State riSt)
{
    using std::make_tuple;
    using std::get;
    using std::vector;
    vector<std::tuple<State,State,JITSUName,Point>> statesAfterJITSU;

    statesAfterJITSU.push_back(make_tuple(mySt, riSt, MU,Point(NONE,NONE)));

    if (mySt.skillPoint >= skillCost[SYUNSIN])
    {
        auto predictBySYUNSIN = [](State st) {st.remSteps++; return st; };
        statesAfterJITSU.push_back(make_tuple(predictBySYUNSIN(mySt), riSt,SYUNSIN,Point(NONE,NONE)));
    }

    if (mySt.skillPoint >= skillCost[IWA_GAKURE])
    {
        auto predictByIWA_GAKURE = [](State st, Point p) {st.objects.field[p.row][p.col] = ROCK; return st; };
        for (int row = 0; row < mySt.Height; row++)
        {
            for (int col = 0; col < mySt.Width; col++)
            {
                if (mySt.objects.field[row][col] == -1 && mySt.dogs.field[row][col] == -1 && mySt.ninjas.field[row][col] == -1)
                    statesAfterJITSU.push_back(make_tuple(predictByIWA_GAKURE(mySt, Point(row, col)), riSt,IWA_GAKURE,Point(row,col)));
            }
        }
    }

    if (mySt.skillPoint >= skillCost[KAMINARI_GAKURE])
    {
        auto predictByKAMINARI_GAKURE = [](State st, Point p) {st.objects.field[p.row][p.col] = NONE; return st; };
        for (int row = 0; row < mySt.Height; row++)
        {
            for (int col = 0; col < mySt.Width; col++)
            {
                if (mySt.objects.field[row][col] == ROCK)
                    statesAfterJITSU.push_back(make_tuple(predictByKAMINARI_GAKURE(mySt, Point(row, col)), riSt,KAMINARI_GAKURE,Point(row,col)));
            }
        }
    }

    if (mySt.skillPoint >= skillCost[BUNSIN])
    {
        auto predictByBUNSIN = [](State st, Point p) {st.ninjaSmells.field[p.row][p.col] = 1; st.ninjaSmells.ids.push_back(1); return st; };
        for (int row = 0; row < mySt.Height; row++)
        {
            for (int col = 0; col < mySt.Width; col++)
            {
                if (mySt.objects.field[row][col] == -1)
                    statesAfterJITSU.push_back(make_tuple(predictByBUNSIN(mySt, Point(row, col)), riSt, BUNSIN,Point(row,col)));
            }
        }
    }

    int maxScore = -INFTY;
    int cnt = 0;
    auto ret = make_tuple(mySt, riSt, MU,Point(NONE,NONE),std::string(""), std::string(""));
    for (int i = 0; i < statesAfterJITSU.size(); i++)
    {
        auto states = statesAfterJITSU[i];
        vector<vector<Direction>> enableSteps;
        std::queue<vector<Direction>> q;
        q.push(vector<Direction>(1, UP));
        q.push(vector<Direction>(1, LEFT));
        q.push(vector<Direction>(1, RIGHT));
        q.push(vector<Direction>(1, DOWN));
        while (!q.empty())
        {
            auto steps = q.front(); q.pop();
            enableSteps.push_back(steps);
            if (steps.size() < get<0>(states).remSteps)
            {
                steps.push_back(UP);    q.push(steps); steps.erase(steps.end() - 1);
                steps.push_back(LEFT);  q.push(steps); steps.erase(steps.end() - 1);
                steps.push_back(RIGHT); q.push(steps); steps.erase(steps.end() - 1);
                steps.push_back(DOWN);  q.push(steps); steps.erase(steps.end() - 1);
            }
        }
        vector<vector<Direction>> steps(2);
        for (int step1 = 0; step1 < enableSteps.size(); step1++)
        {
            steps[0] = enableSteps[step1];
            for (int step2 = 0; step2 < enableSteps.size(); step2++)
            {
                steps[1] = enableSteps[step2];
                cnt++;
                //参照値だったらどうしよう。右辺値参照だから大丈夫?
                auto newMySt = get<0>(states);
                if ([&newMySt, &steps]()
                {
                    //忍者が歩く
                    for (int ninjaId = 0; ninjaId < newMySt.ninjas.ids.size(); ninjaId++)
                    {
                        for (auto& dir : steps[ninjaId])
                        {
                            int nextRow = newMySt.ninjas.getPoint(ninjaId).row + dRow[dir];
                            int nextCol = newMySt.ninjas.getPoint(ninjaId).col + dCol[dir];
                            if (newMySt.objects.field[nextRow][nextCol] == WALL) return false;
                            if (newMySt.objects.field[nextRow][nextCol] == ROCK)
                            {
                                if (newMySt.objects.field[nextRow + dRow[dir]][nextCol + dCol[dir]] != -1 ||
                                    newMySt.dogs.field[nextRow + dRow[dir]][nextCol + dCol[dir]] != -1 ||
                                    newMySt.ninjas.field[nextRow + dRow[dir]][nextCol + dCol[dir]] != -1) return false;
                                else
                                {
                                    newMySt.objects.field[nextRow][nextCol] = NONE;
                                    newMySt.objects.field[nextRow + dRow[dir]][nextCol + dCol[dir]] = ROCK;
                                }
                            }
                            newMySt.ninjas.field[newMySt.ninjas.getPoint(ninjaId).row][newMySt.ninjas.getPoint(ninjaId).col] = NONE;
                            newMySt.ninjas.field[nextRow][nextCol] = ninjaId;
                            if (newMySt.souls.field[nextRow][nextCol] != NONE)
                            {
                                newMySt.souls.field[nextRow][nextCol] = NONE;
                                newMySt.skillPoint++;
                            }
                        }
                    }
                    //犬が歩く。犬は犬の動きたいほうに動くと仮定。つまり犬と犬が重なることはない。
                    for (auto& dogId : newMySt.dogs.ids)
                    {
                        Point curDogPoint = newMySt.dogs.getPoint(dogId);
                        Points collision(newMySt.Height, newMySt.Width, 0);
                        for (int row = 0; row < newMySt.Height; row++)
                        {
                            for (int col = 0; col < newMySt.Width; col++)
                            {
                                if (newMySt.objects.field[row][col] != -1) collision.field[row][col] = INFTY;
                            }
                        }
                        int numDir = NOMOVE;
                        int min = INFTY;
                        for (int dir = 0; dir < 4; dir++)
                        {
                            int nextRow = newMySt.dogs.getPoint(dogId).row + dRow[dir];
                            int nextCol = newMySt.dogs.getPoint(dogId).col + dCol[dir];
                            if (newMySt.objects.field[nextRow][nextCol] != -1) continue;
                            Points points = getMinDist(newMySt, Point(nextRow, nextCol), collision);
                            if (newMySt.ninjaSmells.ids.size() == 0)
                            {
                                for (auto& ninjaId : newMySt.ninjas.ids)
                                {
                                    Point ninjaPoint = newMySt.ninjas.getPoint(ninjaId);
                                    if (min > points.field[ninjaPoint.row][ninjaPoint.col])
                                    {
                                        min = points.field[ninjaPoint.row][ninjaPoint.col];
                                        numDir = dir;
                                    }
                                }
                            }
                            else
                            {
                                for (int ninjaSmellId = 0; ninjaSmellId < newMySt.ninjaSmells.ids.size(); ninjaSmellId++)
                                {
                                    Point ninjaSmellPoint = newMySt.ninjas.getPoint(ninjaSmellId);
                                    if (ninjaSmellPoint == Point(NONE, NONE)) continue;
                                    if (min > points.field[ninjaSmellPoint.row][ninjaSmellPoint.col])
                                    {
                                        min = points.field[ninjaSmellPoint.row][ninjaSmellPoint.col];
                                        numDir = dir;
                                    }
                                }
                            }
                        }
                        Point nextDogPoint = Point(curDogPoint.row + dRow[numDir], curDogPoint.col + dCol[numDir]);
                        //マジックナンバーを使っているのであとから書きかえたい。
                        if (nextDogPoint == newMySt.ninjas.getPoint(0) || nextDogPoint == newMySt.ninjas.getPoint(1)) return false;
                        newMySt.dogs.field[curDogPoint.row][curDogPoint.col] = NONE;
                        newMySt.dogs.field[nextDogPoint.row][nextDogPoint.col];
                    }
                    return true;
                }())
                {
                    vector<int> minSoulDist(mySt.ninjas.ids.size());
                    auto getMinSoulDist = [&newMySt](int ninjaId)
                    {
                        int minDist = INFTY;
                        //すべての岩を通れないと仮定
                        Points collision(newMySt.Height, newMySt.Width, 0);
                        for (int row = 0; row < newMySt.Height; row++)
                        {
                            for (int col = 0; col < newMySt.Width; col++)
                            {
                                if (newMySt.objects.field[row][col] != -1) collision.field[row][col] = INFTY;
                            }
                        }
                        //なぜか忍者が一人消えているときがある。
                        Points points = getMinDist(newMySt, newMySt.ninjas.getPoint(ninjaId), collision);
                        for (auto& soulId : newMySt.souls.ids)
                        {
                            Point soulPoint = newMySt.souls.getPoint(soulId);
                            if (soulPoint == Point(NONE, NONE)) continue;
                            if (minDist > points.field[soulPoint.row][soulPoint.col])
                            {
                                minDist = points.field[soulPoint.row][soulPoint.col];
                            }
                        }
                        return minDist;
                    };
                    int curScore = (100 - getMinSoulDist(0)) + (100 - getMinSoulDist(1)) + newMySt.skillPoint*100;
                    if (maxScore < curScore)
                    {
                        maxScore = curScore;
                        vector<std::string> stepsStr(2, std::string(""));
                        for (auto& dir : steps[0])
                        {
                            stepsStr[0] += d[dir];
                        }
                        for (auto& dir : steps[1])
                        {
                            stepsStr[1] += d[dir];
                        }
                        ret = make_tuple(get<0>(states), get<1>(states), get<2>(states),get<3>(states), stepsStr[0],stepsStr[1]);
                    }
                }
            }
        }
    }
    return ret;
}

void thinkAndRun()
{
    using std::cout;
    using std::endl;
    using std::get;
    using std::string;
    std::vector<string> nextDir(2);
    auto nextAction = predict(myState, rivalState);
    if (get<2>(nextAction) == MU) cout << 2 << endl;
    else
    {
        cout << 3 << endl;
        cout << get<2>(nextAction) << " " << get<3>(nextAction).row << " " << get<3>(nextAction).col << endl;
    }
    cout << get<4>(nextAction) << endl;
    cout << get<5>(nextAction) << endl;
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
    using std::cout;
    using std::endl;
    cout << "NINJAROCK" << endl;
    cout.flush();

    while (input())
    {
        thinkAndRun();
        cout.flush();
    }
}