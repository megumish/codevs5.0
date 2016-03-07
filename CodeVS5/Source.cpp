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
    Point() { row = -1; col = -1; }
    Point(int row, int col) { this->row = row; this->col = col; }
    bool operator== (const Point& p){ return row == p.row && col == p.col; }
};
class Points
{
public:
    std::vector<std::vector<int>> field;
    std::vector<int> ids;
    Points() { field.clear(); ids.clear(); }
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
std::vector<std::vector<Direction>> enableSteps;
JITSUName enableJitsu = MU;

std::tuple<State,State,JITSUName,Point,std::string,std::string> predict(State mySt,State riSt)
{
    using std::make_tuple;
    using std::get;
    using std::vector;

    JITSUName useJitsu = MU;
    Point usePoint = Point(NONE, NONE);
    if (enableJitsu == SYUNSIN && mySt.skillPoint >= skillCost[SYUNSIN])
    {
        mySt.remSteps = 3;
        useJitsu = SYUNSIN;
    }

    /*if (enableJitsu == BUNSIN && mySt.skillPoint >= skillCost[BUNSIN])
    {
        if ([&]()
        {
            for (auto& ninjaId : mySt.ninjas.ids)
            {
                Point ninjaPoint = mySt.ninjas.getPoint(ninjaId);
                for (int dir = 0; dir < 4; dir++)
                {
                    if (mySt.dogs.field[ninjaPoint.row + dRow[dir]][ninjaPoint.col + dCol[dir]] != NONE) return true;
                }
            }
            return false;
        }())
        {
            for (auto& ninjaId : mySt.ninjas.ids)
            {
                Point ninjaPoint = mySt.ninjas.getPoint(ninjaId);
                for (int dir = 0; dir < 4; dir++)
                {
                    if (mySt.dogs.field[ninjaPoint.row + dRow[dir]][ninjaPoint.col + dCol[dir]] != NONE)
                    {
                        mySt.dogs.field[ninjaPoint.row + dRow[dir]][ninjaPoint.col + dCol[dir]] = NONE;
                        int newDogId = mySt.dogs.ids.size();
                        mySt.dogs.ids.push_back(newDogId);
                        mySt.dogs.field[ninjaPoint.row + dRow[dir] * 2][ninjaPoint.col + dCol[dir] * 2] = newDogId;
                    }
                }
            }
            useJitsu = BUNSIN;
            int max = -INFTY;
            Points collision(mySt.Height,mySt.Width,0);
            Points ninjaDist0 = getMinDist(mySt, mySt.ninjas.getPoint(0), collision);
            Points ninjaDist1 = getMinDist(mySt, mySt.ninjas.getPoint(1), collision);
            for (int row = 0; row < mySt.Height; row++)
            {
                for (int col = 0; col < mySt.Width; col++)
                {
                    int cur = ninjaDist0.field[row][col] + ninjaDist1.field[row][col];
                    if (max < cur && mySt.objects.field[row][col] == NONE)
                    {
                        max = cur;
                        usePoint = Point(row, col);
                    }
                }
            }
        }
    }*/

    if (enableJitsu == TENSOUMETSU && mySt.skillPoint >= skillCost[TENSOUMETSU])
    {
        int useNinja = 0;
        if ([&]()
        {
            int max = -INFTY;
            for (auto& ninjaId : mySt.ninjas.ids)
            {
                int numDogs = 0;
                Point ninjaPoint = mySt.ninjas.getPoint(ninjaId);
                for (int dir = 0; dir < 4; dir++)
                {
                    if (mySt.dogs.field[ninjaPoint.row + dRow[dir]][ninjaPoint.col + dCol[dir]] != NONE)
                    {
                        numDogs++;
                    }
                }
                if (max < numDogs)
                {
                    max = numDogs;
                    useNinja = ninjaId;
                }
            }
            if (max >= 2) return true;
            return false;
        }())
        {
            Point ninjaPoint = mySt.ninjas.getPoint(useNinja);
            for (int dir = 0; dir < 4; dir++)
            {
                if (mySt.dogs.field[ninjaPoint.row + dRow[dir]][ninjaPoint.col + dCol[dir]] != NONE)
                {
                    mySt.dogs.field[ninjaPoint.row + dRow[dir]][ninjaPoint.col + dCol[dir]] = NONE;
                }
            }
            useJitsu = TENSOUMETSU;
            usePoint = Point(useNinja, NONE);
        }
    }

    enableSteps.clear();
    std::queue<vector<Direction>> q;
    q.push(vector<Direction>(1, UP));
    q.push(vector<Direction>(1, LEFT));
    q.push(vector<Direction>(1, RIGHT));
    q.push(vector<Direction>(1, DOWN));
    while (!q.empty())
    {
        auto steps = q.front(); q.pop();
        enableSteps.push_back(steps);
        if (steps.size() < mySt.remSteps)
        {
            steps.push_back(UP);    q.push(steps); steps.erase(steps.end() - 1);
            steps.push_back(LEFT);  q.push(steps); steps.erase(steps.end() - 1);
            steps.push_back(RIGHT); q.push(steps); steps.erase(steps.end() - 1);
            steps.push_back(DOWN);  q.push(steps); steps.erase(steps.end() - 1);
        }
    }
    std::reverse(enableSteps.begin(), enableSteps.end());
    auto ret = make_tuple(mySt, riSt, MU, Point(NONE, NONE), std::string(""), std::string(""));
    int maxScore = -INFTY;

    vector<vector<Direction>> steps(2);
    for (int step1 = 0; step1 < enableSteps.size(); step1++)
    {
        steps[0] = enableSteps[step1];
        for (int step2 = 0; step2 < enableSteps.size(); step2++)
        {
            steps[1] = enableSteps[step2];
            State newMySt = mySt;
            if ([&newMySt, &steps]()
            {
                //忍者が歩く
                for (int ninjaId = 0; ninjaId < newMySt.ninjas.ids.size(); ninjaId++)
                {
                    //忍者が重なったときの対処ができるまでの一時しのぎ
                    if (newMySt.ninjas.getPoint(ninjaId) == Point(NONE, NONE)) return false;
                    for (auto& dir : steps[ninjaId])
                    {
                        int nextRow = newMySt.ninjas.getPoint(ninjaId).row + dRow[dir];
                        int nextCol = newMySt.ninjas.getPoint(ninjaId).col + dCol[dir];
                        if (newMySt.objects.field[nextRow][nextCol] == WALL) return false;
                        if (newMySt.objects.field[nextRow][nextCol] == ROCK)
                        {
                            if (newMySt.objects.field[nextRow + dRow[dir]][nextCol + dCol[dir]] != NONE ||
                                newMySt.dogs.field[nextRow + dRow[dir]][nextCol + dCol[dir]] != NONE ||
                                newMySt.ninjas.field[nextRow + dRow[dir]][nextCol + dCol[dir]] != NONE) return false;
                            else
                            {
                                newMySt.objects.field[nextRow][nextCol] = NONE;
                                newMySt.objects.field[nextRow + dRow[dir]][nextCol + dCol[dir]] = ROCK;
                            }
                        }
                        //忍者が重なったとき上書きされてしまうので困った。
                        newMySt.ninjas.field[newMySt.ninjas.getPoint(ninjaId).row][newMySt.ninjas.getPoint(ninjaId).col] = NONE;
                        newMySt.ninjas.field[nextRow][nextCol] = ninjaId;
                        int num = newMySt.ninjas.field[nextRow][nextCol];
                        if (newMySt.souls.field[nextRow][nextCol] != NONE)
                        {
                            newMySt.souls.field[nextRow][nextCol] = NONE;
                            newMySt.skillPoint++;
                        }
                    }
                }
                ////忍者が重なったときの一次処理
                //if (newMySt.ninjas.getPoint(0).row == NONE || newMySt.ninjas.getPoint(1).row == NONE) return false;
                ////犬が歩く
                //Points collision(newMySt.Height, newMySt.Width, 0);
                //for (int row = 0; row < newMySt.Height; row++)
                //{
                //    for (int col = 0; col < newMySt.Width; col++)
                //    {
                //        if (newMySt.objects.field[row][col] != NONE) collision.field[row][col] = INFTY;
                //    }
                //}
                //Points ninjaDist0 = getMinDist(newMySt, newMySt.ninjas.getPoint(0), collision);
                //Points ninjaDist1 = getMinDist(newMySt, newMySt.ninjas.getPoint(1), collision);
                //for (auto& dogId : newMySt.dogs.ids)
                //{
                //    Point dogPoint = newMySt.dogs.getPoint(dogId);
                //    if (dogPoint.row == NONE) continue;
                //    if (ninjaDist0.field[dogPoint.row][dogPoint.col] <= ninjaDist1.field[dogPoint.row][dogPoint.col])
                //    {
                //        int curDist = ninjaDist0.field[dogPoint.row][dogPoint.col];
                //        for (int dir = 0; dir < 5; dir++)
                //        {
                //            if (curDist > ninjaDist0.field[dogPoint.row + dRow[dir]][dogPoint.col + dCol[dir]])
                //            {
                //                curDist = ninjaDist0.field[dogPoint.row + dRow[dir]][dogPoint.col + dCol[dir]];
                //                newMySt.dogs.field[dogPoint.row][dogPoint.col] = NONE;
                //                newMySt.dogs.field[dogPoint.row + dRow[dir]][dogPoint.col + dCol[dir]] = dogId;
                //            }
                //        }
                //    }
                //    else
                //    {
                //        int curDist = ninjaDist1.field[dogPoint.row][dogPoint.col];
                //        for (int dir = 0; dir < 5; dir++)
                //        {
                //            if (curDist > ninjaDist1.field[dogPoint.row + dRow[dir]][dogPoint.col + dCol[dir]])
                //            {
                //                curDist = ninjaDist1.field[dogPoint.row + dRow[dir]][dogPoint.col + dCol[dir]];
                //                newMySt.dogs.field[dogPoint.row][dogPoint.col] = NONE;
                //                newMySt.dogs.field[dogPoint.row + dRow[dir]][dogPoint.col + dCol[dir]] = dogId;
                //            }
                //        }
                //    }
                //}
                for (auto& ninjaId : newMySt.ninjas.ids)
                {
                    Point ninjaPoint = newMySt.ninjas.getPoint(ninjaId);
                    //忍者が重なったときの対処ができるまでの一時しのぎ
                    if (ninjaPoint == Point(NONE, NONE)) return false;
                    int numBlock = 0;
                    for (int dir = 0; dir < 5; dir++)
                    {
                        if (newMySt.dogs.field[ninjaPoint.row + dRow[dir]][ninjaPoint.col + dCol[dir]] != NONE) return false;
                        //追い込まれたときは回転切りを使うようにするのでこれは封じる。
                        //これはこれとしてとっておいたほうがいい。スキルが使えない時もあるので。
                        if (newMySt.objects.field[ninjaPoint.row + dRow[dir]][ninjaPoint.col + dCol[dir]] == WALL) numBlock++;
                        if (newMySt.objects.field[ninjaPoint.row + dRow[dir]][ninjaPoint.col + dCol[dir]] == ROCK &&
                            (newMySt.objects.field[ninjaPoint.row + dRow[dir]*2][ninjaPoint.col + dCol[dir]*2] != NONE ||
                             newMySt.dogs.field[ninjaPoint.row + dRow[dir]*2][ninjaPoint.col + dCol[dir]*2]    != NONE ||
                             newMySt.ninjas.field[ninjaPoint.row + dRow[dir]*2][ninjaPoint.col + dCol[dir]*2]  != NONE )) numBlock++;
                    }
                    if (numBlock >= 3) return false;
                }
                return true;
            }())
            {
                vector<int> minSoulDist(mySt.ninjas.ids.size());
                auto evaluateDistForSoulAndDog = [&newMySt](int ninjaId)
                {
                    int soulMinDist = INFTY;
                    int dogMinDist = INFTY;
                    int numCloseDog = 0;
                    //すべての岩を通れないと仮定。ただし四方が他のもので囲われていない岩は通れると仮定
                    Points soulCollision(newMySt.Height, newMySt.Width, 0);
                    for (int row = 0; row < newMySt.Height; row++)
                    {
                        for (int col = 0; col < newMySt.Width; col++)
                        {
                            if (newMySt.objects.field[row][col] != NONE ||
                                newMySt.dogs.field[row][col]    != NONE) soulCollision.field[row][col] = INFTY;
                            if (newMySt.objects.field[row][col] == ROCK)
                            {
                                int numBlock = 0;
                                for (int dir = 0; dir < 4; dir++)
                                {
                                    if (newMySt.objects.field[row + dRow[dir]][col + dCol[dir]] == -1 ||
                                        newMySt.dogs.field[row + dRow[dir]][col + dCol[dir]]    == -1 ) numBlock++;
                                }
                                if (numBlock != 4) soulCollision.field[row][col] = NONE;
                            }
                        }
                    }
                    Points dogCollision(newMySt.Height, newMySt.Width, 0);
                    for (int row = 0; row < newMySt.Height; row++)
                    {
                        for (int col = 0; col < newMySt.Width; col++)
                        {
                            if (newMySt.objects.field[row][col] != -1) dogCollision.field[row][col] = INFTY;
                        }
                    }
                    Points soulpoints = getMinDist(newMySt, newMySt.ninjas.getPoint(ninjaId), soulCollision);
                    for (auto& soulId : newMySt.souls.ids)
                    {
                        Point soulPoint = newMySt.souls.getPoint(soulId);
                        if (soulPoint == Point(NONE, NONE)) continue;
                        if (soulMinDist > soulpoints.field[soulPoint.row][soulPoint.col])
                        {
                            soulMinDist = soulpoints.field[soulPoint.row][soulPoint.col];
                        }
                    }
                    Points dogpoints = getMinDist(newMySt, newMySt.ninjas.getPoint(ninjaId), dogCollision);
                    for (auto& dogId : newMySt.dogs.ids)
                    {
                        Point dogPoint = newMySt.dogs.getPoint(dogId);
                        if (dogPoint == Point(NONE, NONE)) continue;
                        if (dogpoints.field[dogPoint.row][dogPoint.col] <= 4) numCloseDog++;
                        if (dogMinDist > dogpoints.field[dogPoint.row][dogPoint.col])
                        {
                            dogMinDist = dogpoints.field[dogPoint.row][dogPoint.col];
                        }
                    }
                    if (numCloseDog == 0) return 200 + (100 - 4 * soulMinDist);
                    return (200-6*numCloseDog) + (100-4*soulMinDist);
                };
                auto distNinjas = [&newMySt]()
                {
                    Points collision(newMySt.Height, newMySt.Width, 0);
                    Points points = getMinDist(newMySt, newMySt.ninjas.getPoint(0), collision);
                    if(points.field[newMySt.ninjas.getPoint(1).row][newMySt.ninjas.getPoint(1).col] <= 3)
                        return points.field[newMySt.ninjas.getPoint(1).row][newMySt.ninjas.getPoint(1).col];
                    return 3;
                };
                int curScore = evaluateDistForSoulAndDog(0) + evaluateDistForSoulAndDog(1) + distNinjas() + newMySt.skillPoint * 1000;
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
                    ret = make_tuple(newMySt,riSt,useJitsu,usePoint,stepsStr[0],stepsStr[1]);
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
    if (get<4>(nextAction) == "") cout << 3 << endl << "7 0" << endl;
    else if (get<5>(nextAction) == "") cout << 3 << endl << "7 1" << endl;
    else if (get<2>(nextAction) == MU) cout << 2 << endl;
    else if (get<2>(nextAction) == SYUNSIN)
    {
        cout << 3 << endl;
        cout << get<2>(nextAction) << endl;
    }
    else if (get<2>(nextAction) == TENSOUMETSU)
    {
        cout << 3 << endl;
        cout << get<2>(nextAction) << " " << get<3>(nextAction).row << endl;
    }
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

    if (skillCost[SYUNSIN] <= 3)
    {
        enableJitsu = SYUNSIN;
    }
    else if (skillCost[TENSOUMETSU] <= 10)
    {
        enableJitsu = TENSOUMETSU;
    }
    myState.input(numOfSkills);
    rivalState.input(numOfSkills);
    return true;
}

int main()
{
    using std::cout;
    using std::endl;
    using std::vector;

    cout << "NINJAROCK" << endl;
    cout.flush();

    while (input())
    {
        thinkAndRun();
        cout.flush();
    }
}