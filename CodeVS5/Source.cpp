#include<iostream>
#include<vector>
#include<tuple>
#include<queue>
#include<string>
#include<map>
#include<random>
#include<functional>

const int NONE = 100000000;
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
std::vector<int> jitsuesCost;

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
class Cell
{
public:
    std::vector<int> ids;
    Cell() { ids.clear(); }
};
class Cells
{
public:
    std::vector<std::vector<Cell>> field;
    std::vector<int> ids;
    Cells() { field.clear(); ids.clear(); }
    Cells(int height, int width) : field(height, std::vector<Cell>(width)) { ids.clear(); }
    Point getPoint(int id)
    {
        for (int row = 0; row < field.size(); row++)
        {
            for (int col = 0; col < field[row].size(); col++)
            {
                for (auto& ownId : field[row][col].ids)
                {
                    if (id == ownId) return Point(row, col);
                }
            }
        }
        return Point(NONE, NONE);
    }
};
class State
{
public:
    int jitsuPoint;
    int Height, Width;
    int remSteps;
    Points dogs;
    Points objects;
    Cells ninjas;
    Points souls;
    Cells ninjaSmells;
    std::vector<int> skillCount;
    
    State() {}

    void input(int numOfSkills)
    {
        using std::cin;

        remSteps = 2;

        cin >> jitsuPoint;

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
        ninjas = Cells(Height, Width);
        ninjaSmells = Cells(Height, Width);
        for (int i = 0; i < numNinjas; i++)
        {
            int id, row, col;
            cin >> id >> row >> col;
            ninjas.field[row][col].ids.push_back(id);
            ninjas.ids.push_back(id);
        }

        int numDogs;
        cin >> numDogs;
        dogs = Points(Height, Width,0);
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

class States
{
public:
    State mySt;
    State riSt;
    std::vector<int> firstSteps;
    bool isFirst;
    States(State mySt, State riSt) :firstSteps(2)
    {
        this->mySt = mySt;
        this->riSt = riSt;
        isFirst = true;
    }
    States(State mySt, State riSt, std::vector<int> firstSteps)
    {
        this->mySt = mySt;
        this->riSt = riSt;
        this->firstSteps = firstSteps;
        isFirst = false;
    }
};

State myState;
State rivalState;
std::vector<std::vector<Direction>> enableSteps;
JITSUName enableJitsu = MU;
std::multimap<int, JITSUName> guardJitsu;
std::multimap<int, JITSUName> attackJitsu;

int evaluate(State& mySt, State& riSt)
{
    for (auto& ninjaId : mySt.ninjas.ids)
    {
        Point ninjaPoint = mySt.ninjas.getPoint(ninjaId);
        if (mySt.dogs.field[ninjaPoint.row][ninjaPoint.col] != NONE) 
            return -NONE-1;
    }

    Point ninjaPoint0 = mySt.ninjas.getPoint(0);
    std::queue<Point> q;
    q.push(ninjaPoint0);

    Points points0(mySt.Height, mySt.Width, 0);
    points0.field[ninjaPoint0.row][ninjaPoint0.col] = 0;
    while (!q.empty())
    {
        Point point = q.front(); q.pop();
        for (int dir = 0; dir < 4; dir++)
        {
            int nextRow = point.row + dRow[dir];
            int nextCol = point.col + dCol[dir];
            if (nextRow == 0 || nextCol == 0 || nextRow == mySt.Height || nextCol == mySt.Width) continue;
            if (mySt.objects.field[nextRow][nextCol] == NONE && mySt.dogs.field[nextRow][nextCol] == NONE && points0.field[nextRow][nextCol] == NONE)
            {
                q.push(Point(nextRow, nextCol));
                points0.field[nextRow][nextCol] = points0.field[point.row][point.col] + 1;
            }
        }
    }

    Point ninjaPoint1 = mySt.ninjas.getPoint(1);
    q.push(ninjaPoint1);

    Points points1(mySt.Height, mySt.Width, 0);
    points1.field[ninjaPoint1.row][ninjaPoint1.col] = 0;
    while (!q.empty())
    {
        Point point = q.front(); q.pop();
        for (int dir = 0; dir < 4; dir++)
        {
            int nextRow = point.row + dRow[dir];
            int nextCol = point.col + dCol[dir];
            if (nextRow == 0 || nextCol == 0 || nextRow == mySt.Height || nextCol == mySt.Width) continue;
            if (mySt.objects.field[nextRow][nextCol] == NONE && mySt.dogs.field[nextRow][nextCol] == NONE && points1.field[nextRow][nextCol] == NONE)
            {
                q.push(Point(nextRow, nextCol));
                points1.field[nextRow][nextCol] = points1.field[point.row][point.col] + 1;
            }
        }
    }

    int soulMinDist0;
    soulMinDist0 = NONE+1;
    for (auto& soulId : mySt.souls.ids)
    {
        Point soulPoint = mySt.souls.getPoint(soulId);
        if (soulPoint == Point(NONE, NONE)) continue;
        if (soulMinDist0 > points0.field[soulPoint.row][soulPoint.col])
        {
            soulMinDist0 = points0.field[soulPoint.row][soulPoint.col];
        }
    }

    int soulMinDist1;
    soulMinDist1 = NONE + 1;
    for (auto& soulId : mySt.souls.ids)
    {
        Point soulPoint = mySt.souls.getPoint(soulId);
        if (soulPoint == Point(NONE, NONE)) continue;
        if (soulMinDist1 > points1.field[soulPoint.row][soulPoint.col])
        {
            soulMinDist1 = points1.field[soulPoint.row][soulPoint.col];
        }
    }

    int numDogs0;
    numDogs0 = 0;
    for (auto& dogId : mySt.dogs.ids)
    {
        Point dogPoint = mySt.dogs.getPoint(dogId);
        if (dogPoint == Point(NONE, NONE)) continue;
        if (points0.field[dogPoint.row][dogPoint.col] <= 3) numDogs0++;
    }

    int numDogs1;
    numDogs1 = 0;
    for (auto& dogId : mySt.dogs.ids)
    {
        Point dogPoint = mySt.dogs.getPoint(dogId);
        if (dogPoint == Point(NONE, NONE)) continue;
        if (points1.field[dogPoint.row][dogPoint.col] <= 3) numDogs1++;
    }

    if (soulMinDist0 >= NONE || soulMinDist1 >= NONE) return (100 + 100 + 100 - 8 * numDogs0 + 100 - 8 * numDogs1 + mySt.jitsuPoint * 100);
    return (100 - 4*soulMinDist0 +100 - 4*soulMinDist1 + mySt.jitsuPoint * 100);
}

std::pair<std::vector<std::vector<Direction>>,int> predict(State mySt,State riSt)
{
    using std::vector;

    volatile int numStep = mySt.remSteps;
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
    auto ret = make_pair(vector<vector<Direction>>(2,vector<Direction>(0)), 0);
    int maxScore = -NONE;
    std::multimap<int, States,std::greater<int>> scores;

    vector<States> goodChoice;
    std::queue<States> statesQ;
    statesQ.push(States(mySt, riSt));
    int count = 0;
    while (count < 5)
    {
        if (statesQ.empty())
        {
            count++;
            int i = 0;
            if (scores.size() != 0)
            {
                goodChoice.push_back(scores.begin()->second);
                for (auto& score : scores)
                {
                    if (i >= 10) break;
                    statesQ.push(score.second);
                    i++;
                }
                scores.clear();
                continue;
            }
            else
                break;
        }
        auto states = statesQ.front(); statesQ.pop();
        vector<vector<Direction>> steps(2);
        for (int step1 = 0; step1 < enableSteps.size(); step1++)
        {
            steps[0] = enableSteps[step1];
            for (int step2 = 0; step2 < enableSteps.size(); step2++)
            {
                steps[1] = enableSteps[step2];
                State newMySt = states.mySt;
                if ([&newMySt, &steps]()
                {
                    //忍者が歩く
                    for (int ninjaId = 0; ninjaId < newMySt.ninjas.ids.size(); ninjaId++)
                    {
                        for (auto& dir : steps[ninjaId])
                        {
                            Point nowPoint = newMySt.ninjas.getPoint(ninjaId);
                            int nextRow = nowPoint.row + dRow[dir];
                            int nextCol = nowPoint.col + dCol[dir];
                            if (newMySt.objects.field[nextRow][nextCol] == WALL) return false;
                            if (newMySt.objects.field[nextRow][nextCol] == ROCK)
                            {
                                if (newMySt.objects.field[nextRow + dRow[dir]][nextCol + dCol[dir]] != NONE ||
                                    newMySt.dogs.field[nextRow + dRow[dir]][nextCol + dCol[dir]] != NONE /*||
                                    newMySt.ninjas.field[nextRow + dRow[dir]][nextCol + dCol[dir]].ids.size() != 0*/) return false;
                                else
                                {
                                    newMySt.objects.field[nextRow][nextCol] = NONE;
                                    newMySt.objects.field[nextRow + dRow[dir]][nextCol + dCol[dir]] = ROCK;
                                }
                            }
                            auto& ninjaCell = newMySt.ninjas.field[nowPoint.row][nowPoint.col];
                            ninjaCell.ids.erase(find(ninjaCell.ids.begin(), ninjaCell.ids.end(), ninjaId));
                            newMySt.ninjas.field[nextRow][nextCol].ids.push_back(ninjaId);
                            if (newMySt.souls.field[nextRow][nextCol] != NONE)
                            {
                                newMySt.souls.field[nextRow][nextCol] = NONE;
                                newMySt.jitsuPoint++;
                            }
                        }
                    }
                    //std::cerr << "犬移動前:" << std::endl;
                    //for (int row = 0; row < newMySt.Height; row++)
                    //{
                    //    for (int col = 0; col < newMySt.Width; col++)
                    //    {
                    //        if (newMySt.ninjas.field[row][col].ids.size() != 0) std::cerr << "N";
                    //        else if (newMySt.dogs.field[row][col] != NONE) std::cerr << "D";
                    //        else std::cerr << "X";
                    //    }
                    //    std::cerr << std::endl;
                    //}
                    //犬が歩く
                    Point ninjaPoint0 = newMySt.ninjas.getPoint(0);
                    if (newMySt.ninjaSmells.ids.size() != 0)
                    {
                        ninjaPoint0 = newMySt.ninjaSmells.getPoint(0);
                    }
                    std::queue<Point> q;
                    q.push(ninjaPoint0);

                    Points points0(newMySt.Height, newMySt.Width, 0);
                    points0.field[ninjaPoint0.row][ninjaPoint0.col] = 0;
                    while (!q.empty())
                    {
                        Point point = q.front(); q.pop();
                        for (int dir = 0; dir < 4; dir++)
                        {
                            int nextRow = point.row + dRow[dir];
                            int nextCol = point.col + dCol[dir];
                            if (nextRow == 0 || nextCol == 0 || nextRow == newMySt.Height || nextCol == newMySt.Width) continue;
                            if (newMySt.objects.field[nextRow][nextCol] == NONE && points0.field[nextRow][nextCol] == NONE)
                            {
                                q.push(Point(nextRow, nextCol));
                                points0.field[nextRow][nextCol] = points0.field[point.row][point.col] + 1;
                            }
                        }
                    }
                    Point ninjaPoint1 = newMySt.ninjas.getPoint(1);
                    if (newMySt.ninjaSmells.ids.size() != 0)
                    {
                        ninjaPoint1 = newMySt.ninjaSmells.getPoint(0);
                    }
                    q.push(ninjaPoint1);

                    Points points1(newMySt.Height, newMySt.Width, 0);
                    points1.field[ninjaPoint1.row][ninjaPoint1.col] = 0;
                    while (!q.empty())
                    {
                        Point point = q.front(); q.pop();
                        for (int dir = 0; dir < 4; dir++)
                        {
                            int nextRow = point.row + dRow[dir];
                            int nextCol = point.col + dCol[dir];
                            if (nextRow == 0 || nextCol == 0 || nextRow == newMySt.Height || nextCol == newMySt.Width) continue;
                            if (newMySt.objects.field[nextRow][nextCol] == NONE && points1.field[nextRow][nextCol] == NONE)
                            {
                                q.push(Point(nextRow, nextCol));
                                points1.field[nextRow][nextCol] = points1.field[point.row][point.col] + 1;
                            }
                        }
                    }

                    Points points(newMySt.Height, newMySt.Width, 0);
                    for (int row = 0; row < newMySt.Height; row++)
                    {
                        for (int col = 0; col < newMySt.Width; col++)
                        {
                            //if (col != 0) std::cerr << ",";
                            points.field[row][col] = std::min(points0.field[row][col], points1.field[row][col]);
                            ////std::cerr << points.field[row][col];
                        }
                        //std::cerr << std::endl;
                    }

                    for (int dist = 0; dist < newMySt.Height + newMySt.Width; dist++)
                    {
                        vector<int> dogList;
                        while (true)
                        {
                            Point curPoint = points.getPoint(dist);
                            if (curPoint == Point(NONE, NONE)) break;
                            if (newMySt.dogs.field[curPoint.row][curPoint.col] != NONE)
                            {
                                dogList.push_back(newMySt.dogs.field[curPoint.row][curPoint.col]);
                            }
                            points.field[curPoint.row][curPoint.col] = points.field[curPoint.row][curPoint.col] - 1;
                        }
                        std::sort(dogList.begin(), dogList.end());
                        for (auto& dogId : dogList)
                        {
                            for (int dir = 0; dir < 4; dir++)
                            {
                                Point dogPoint = newMySt.dogs.getPoint(dogId);
                                int nextRow = dogPoint.row + dRow[dir];
                                int nextCol = dogPoint.col + dCol[dir];
                                if (points.field[dogPoint.row][dogPoint.col] > points.field[nextRow][nextCol]
                                    && newMySt.dogs.field[nextRow][nextCol] == NONE)
                                {
                                    std::swap(newMySt.dogs.field[dogPoint.row][dogPoint.col], newMySt.dogs.field[nextRow][nextCol]);
                                    break;
                                }
                            }
                        }
                    }
                    //std::cerr << "犬移動後:" << std::endl;
                    //for (int row = 0; row < newMySt.Height; row++)
                    //{
                    //    for (int col = 0; col < newMySt.Width; col++)
                    //    {
                    //        if (newMySt.ninjas.field[row][col].ids.size() != 0) std::cerr << "N";
                    //        else if (newMySt.dogs.field[row][col] != NONE) std::cerr << "D";
                    //        else std::cerr << "X";
                    //        
                    //    }
                    //    std::cerr << std::endl;
                    //}
                    for (auto& ninjaId : newMySt.ninjas.ids)
                    {
                        Point ninjaPoint = newMySt.ninjas.getPoint(ninjaId);
                        int numBlock = 0;
                        for (int dir = 0; dir < 5; dir++)
                        {
                            if (newMySt.objects.field[ninjaPoint.row + dRow[dir]][ninjaPoint.col + dCol[dir]] == WALL ||
                                newMySt.dogs.field[ninjaPoint.row +dRow[dir]][ninjaPoint.col + dCol[dir]] != NONE) numBlock++;
                            if (newMySt.objects.field[ninjaPoint.row + dRow[dir]][ninjaPoint.col + dCol[dir]] == ROCK &&
                                (newMySt.objects.field[ninjaPoint.row + dRow[dir] * 2][ninjaPoint.col + dCol[dir] * 2] != NONE ||
                                    newMySt.dogs.field[ninjaPoint.row + dRow[dir] * 2][ninjaPoint.col + dCol[dir] * 2] != NONE ||
                                    newMySt.ninjas.field[ninjaPoint.row + dRow[dir] * 2][ninjaPoint.col + dCol[dir] * 2].ids.size() != 0)) numBlock++;
                        }
                        if (numBlock >= 3) return false;
                    }
                    return true;
                }())
                {
                    int score = evaluate(newMySt, riSt);
                    if (score > -NONE)
                    {
                        if (states.isFirst)
                        {
                            vector<int> firstSteps(2);
                            firstSteps[0] = step1;
                            firstSteps[1] = step2;
                            if (newMySt.remSteps == 3)
                            {
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
                                    if (steps.size() < 2)
                                    {
                                        steps.push_back(UP);    q.push(steps); steps.erase(steps.end() - 1);
                                        steps.push_back(LEFT);  q.push(steps); steps.erase(steps.end() - 1);
                                        steps.push_back(RIGHT); q.push(steps); steps.erase(steps.end() - 1);
                                        steps.push_back(DOWN);  q.push(steps); steps.erase(steps.end() - 1);
                                    }
                                }
                                std::reverse(enableSteps.begin(), enableSteps.end());
                            }
                            newMySt.ninjas.ids.clear();
                            scores.insert(std::make_pair(score, States(newMySt, riSt, firstSteps)));
                        }
                        else
                        {
                            scores.insert(std::make_pair(score, States(newMySt, riSt, states.firstSteps)));
                        }
                    }
                }
            }
        }
    }
    
    enableSteps.clear();
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

    if (goodChoice.size() != 0)
    {
        States bestStates = *(goodChoice.end() - 1);
        ret.first[0] = enableSteps[bestStates.firstSteps[0]];
        ret.first[1] = enableSteps[bestStates.firstSteps[1]];
        ret.second = goodChoice.size();
    }
    return ret;
}


void thinkAndRun()
{
    using std::cout;
    using std::endl;
    using std::get;
    using std::string;
    //落石予想
    //石が置かれるのを予想すると、犬の動きまで変わってしまい。犬に衝突してしまい不便。自雷撃で解決した方がいい。
    //for (auto& ninjaId : myState.ninjas.ids)
    //{
    //    auto ninjaPoint = myState.ninjas.getPoint(ninjaId);
    //    int numObjects = 0;
    //    for (int dir = 0; dir < 4; dir++)
    //    {
    //        int nextRow = ninjaPoint.row + dRow[dir];
    //        int nextCol = ninjaPoint.col + dCol[dir];
    //        if (myState.objects.field[nextRow][nextCol] == WALL || myState.dogs.field[nextRow][nextCol] != NONE) numObjects++;
    //        else if (myState.objects.field[nextRow][nextCol] == ROCK)
    //        {
    //            if (myState.objects.field[nextRow + dRow[dir]][nextCol + dCol[dir]] != NONE ||
    //                myState.dogs.field[nextRow + dRow[dir]][nextCol + dCol[dir]] != NONE ) numObjects++;
    //        }
    //    }
    //    if (numObjects >= 3)
    //    {
    //        for (int dir = 0; dir < 4; dir++)
    //        {
    //            int row = ninjaPoint.row + dRow[dir];
    //            int col = ninjaPoint.col + dCol[dir];
    //            if (row <= 0 || col <= 0 || row >= myState.Height || col >= myState.Width) continue;
    //            if (myState.objects.field[row][col] != NONE ||
    //                myState.dogs.field[row][col] != NONE ||
    //                myState.ninjas.field[row][col].ids.size() != 0) continue;
    //            else
    //            {
    //                myState.objects.field[row][col] = ROCK;
    //            }
    //        }
    //        for (int dir = 0; dir < 4; dir++)
    //        {
    //            int row = ninjaPoint.row + dRow[dir] * 2;
    //            int col = ninjaPoint.col + dCol[dir] * 2;
    //            if (row <= 0 || col <= 0 || row >= myState.Height || col >= myState.Width) continue;
    //            if (myState.objects.field[row][col] != NONE || 
    //                myState.dogs.field[row][col] != NONE || 
    //                myState.ninjas.field[row][col].ids.size() != 0) continue;
    //            else
    //            {
    //                myState.objects.field[row][col] = ROCK;
    //            }
    //        }
    //    }
    //    if (numObjects >= 2)
    //    {
    //        for (int dir = 0; dir < 4; dir++)
    //        {
    //            int row = ninjaPoint.row + dRow[dir];
    //            int col = ninjaPoint.col + dCol[dir];
    //            if (row <= 0 || col <= 0 || row >= myState.Height || col >= myState.Width) continue;
    //            if (myState.objects.field[row][col] != NONE ||
    //                myState.dogs.field[row][col] != NONE ||
    //                myState.ninjas.field[row][col].ids.size() != 0) continue;
    //            else
    //            {
    //                myState.objects.field[row][col] = ROCK;
    //            }
    //        }
    //    }
    //}
    auto nextAction = predict(myState, rivalState);
    auto nextSteps = get<0>(nextAction);
    auto depth = get<1>(nextAction);
    auto nextJitsu = MU;
    auto jitsuPoint = Point(NONE, NONE);
    if (depth <= 2)
    {
        for (auto& jitsuPair : guardJitsu)
        {
            int jitsuCost = jitsuPair.first;
            JITSUName jitsu = jitsuPair.second;
            if (myState.jitsuPoint >= jitsuCost && nextJitsu == MU)
            {
                auto mySt = myState;
                Points memory = Points(mySt.Height, mySt.Width, 0);
                int cnt = 0;
                switch (jitsu)
                {
                case SYUNSIN:
                    mySt.remSteps = 3;
                    nextAction = predict(mySt, rivalState);
                    if (get<1>(nextAction) >= 2)
                    {
                        nextSteps = get<0>(nextAction);
                        nextJitsu = jitsu;
                    }
                    break;
                case BUNSIN:
                    while (cnt < 10 - jitsuesCost[BUNSIN])
                    {
                        cnt++;
                        mySt.ninjaSmells = Cells(mySt.Height, mySt.Width);
                        std::random_device seed_gen;
                        std::mt19937 engine(seed_gen());
                        std::uniform_int_distribution<int> distHeight(1, 15);
                        std::uniform_int_distribution<int> distWidth(1, 12);
                        int row = distHeight(engine);
                        int col = distWidth(engine);
                        if (mySt.objects.field[row][col] != NONE && memory.field[row][col] == NONE) continue;
                        else
                        {
                            memory.field[row][col] = 0;
                            mySt.ninjaSmells.ids.push_back(0);
                            mySt.ninjaSmells.field[row][col].ids.push_back(0);
                            nextAction = predict(mySt, rivalState);
                            if (get<1>(nextAction) >= 1)
                            {
                                nextSteps = get<0>(nextAction);
                                nextJitsu = jitsu;
                                jitsuPoint = Point(row, col);
                                break;
                            }
                            else continue;
                        }
                    }
                    break;
                case TENSOUMETSU:
                    [&]()
                    {
                        for (auto& ninjaId : mySt.ninjas.ids)
                        {
                            auto newMySt = mySt;
                            Point ninjaPoint = newMySt.ninjas.getPoint(ninjaId);
                            for (int dir = 0; dir < 4; dir++)
                            {
                                int row = ninjaPoint.row + dRow[dir];
                                int col = ninjaPoint.col + dCol[dir];
                                newMySt.dogs.field[row][col] = NONE;
                            }
                            nextAction = predict(newMySt, rivalState);
                            if (get<1>(nextAction) >= 1)
                            {
                                nextSteps = get<0>(nextAction);
                                nextJitsu = jitsu;
                                jitsuPoint = Point(ninjaId, NONE);
                                return;
                            }
                        }
                    }();
                    break;
                }
            }
        }
    }

    if (nextJitsu == MU && myState.jitsuPoint >= jitsuesCost[KAMINARI_GAKURE])
    {
        [&]()
        {
            for (auto& ninjaId : myState.ninjas.ids)
            {
                auto ninjaPoint = myState.ninjas.getPoint(ninjaId);
                int numObjects = 0;
                int numRock = 0;
                for (int dir = 0; dir < 4; dir++)
                {
                    int nextRow = ninjaPoint.row + dRow[dir];
                    int nextCol = ninjaPoint.col + dCol[dir];
                    if (myState.objects.field[nextRow][nextCol] == WALL || myState.dogs.field[nextRow][nextCol] != NONE) numObjects++;
                    else if (myState.objects.field[nextRow][nextCol] == ROCK)
                    {
                        if (myState.objects.field[nextRow + dRow[dir]][nextCol + dCol[dir]] != NONE ||
                            myState.dogs.field[nextRow + dRow[dir]][nextCol + dCol[dir]] != NONE) numObjects++;
                        else numRock++;
                    }
                }
                if (numObjects >= 2 && numRock >= 1)
                {
                    int nextRow = ninjaPoint.row + dRow[nextSteps[ninjaId][0]];
                    int nextCol = ninjaPoint.col + dCol[nextSteps[ninjaId][0]];
                    if (myState.objects.field[nextRow][nextCol] == ROCK)
                    {
                        cout << 3 << endl;
                        cout << KAMINARI_GAKURE << " " << nextRow << " " << nextCol << endl;
                        nextJitsu = KAMINARI_GAKURE;
                        return;
                    }
                    int nextNextRow = ninjaPoint.row + dRow[nextSteps[ninjaId][0]] + dRow[nextSteps[ninjaId][1]];
                    int nextNextCol = ninjaPoint.col + dCol[nextSteps[ninjaId][0]] + dCol[nextSteps[ninjaId][1]];
                    if (myState.objects.field[nextNextRow][nextNextCol] == ROCK)
                    {
                        cout << 3 << endl;
                        cout << KAMINARI_GAKURE << " " << nextNextRow << " " << nextNextCol << endl;
                        nextJitsu = KAMINARI_GAKURE;
                        return;
                    }
                }
            }
        }();
    }

    if (nextJitsu == MU && myState.jitsuPoint >= jitsuesCost[IWA_OTOSHI])
    {
        [&nextJitsu]()
        {
            for (auto& ninjaId : rivalState.ninjas.ids)
            {
                auto ninjaPoint = rivalState.ninjas.getPoint(ninjaId);
                int numObjects = 0;
                for (int dir = 0; dir < 4; dir++)
                {
                    int nextRow = ninjaPoint.row + dRow[dir];
                    int nextCol = ninjaPoint.col + dCol[dir];
                    if (rivalState.objects.field[nextRow][nextCol] == WALL || rivalState.dogs.field[nextRow][nextCol] != NONE) numObjects++;
                    else if (rivalState.objects.field[nextRow][nextCol] == ROCK)
                    {
                        if (rivalState.objects.field[nextRow + dRow[dir]][nextCol + dCol[dir]] != NONE ||
                            rivalState.dogs.field[nextRow + dRow[dir]][nextCol + dCol[dir]] != NONE) numObjects++;
                    }
                }
                if (numObjects >= 3)
                {
                    for (int dir = 0; dir < 4; dir++)
                    {
                        int row = ninjaPoint.row + dRow[dir] * 2;
                        int col = ninjaPoint.col + dCol[dir] * 2;
                        if (row <= 0 || col <= 0 || row >= rivalState.Height || col >= rivalState.Width) continue;
                        if (rivalState.objects.field[row][col] != NONE ||
                            rivalState.dogs.field[row][col] != NONE ||
                            rivalState.ninjas.field[row][col].ids.size() != 0) continue;
                        else
                        {
                            cout << 3 << endl;
                            cout << IWA_OTOSHI << " " << row << " " << col << endl;
                            nextJitsu = IWA_OTOSHI;
                            return;
                        }
                    }
                    for (int dir = 0; dir < 4; dir++)
                    {
                        int row = ninjaPoint.row + dRow[dir];
                        int col = ninjaPoint.col + dCol[dir];
                        if (row <= 0 || col <= 0 || row >= rivalState.Height || col >= rivalState.Width) continue;
                        if (rivalState.objects.field[row][col] != NONE ||
                            rivalState.dogs.field[row][col] != NONE ||
                            rivalState.ninjas.field[row][col].ids.size() != 0) continue;
                        else
                        {
                            cout << 3 << endl;
                            cout << IWA_OTOSHI << " " << row << " " << col << endl;
                            nextJitsu = IWA_OTOSHI;
                            return;
                        }
                    }
                }
            }
        }();
    }

    if (nextJitsu == MU && myState.jitsuPoint >= jitsuesCost[SYUNSIN] * 5)
    {
        myState.remSteps = 3;
        nextAction = predict(myState, rivalState);
        nextSteps = get<0>(nextAction);
        nextJitsu = SYUNSIN;
    }
    if (nextJitsu == SYUNSIN)
    {
        cout << 3 << endl;
        cout << SYUNSIN << endl;
    }
    
    if (nextJitsu == BUNSIN)
    {
        cout << 3 << endl;
        cout << BUNSIN << " " << jitsuPoint.row << " " << jitsuPoint.col << endl;
    }

    if (nextJitsu == TENSOUMETSU)
    {
        cout << 3 << endl;
        cout << TENSOUMETSU << " " << jitsuPoint.row << endl;
    }
    if (nextJitsu == MU)
    {
        cout << 2 << endl;
    }
    std::cerr << nextJitsu << endl;
    std::vector<std::string> stepsStr(2, std::string(""));
    for (auto& dir : nextSteps[0])
    {
        stepsStr[0] += d[dir];
    }
    for (auto& dir : nextSteps[1])
    {
        stepsStr[1] += d[dir];
    }
    cout << stepsStr[0] << endl;
    cout << stepsStr[1] << endl;
}


bool input()
{
    using namespace std;
    if (!(cin >> remainingTime)) return false;

    int numOfSkills;
    cin >> numOfSkills;

    int cost;
    
    attackJitsu.clear();
    guardJitsu.clear();
    cin >> cost;
    guardJitsu.insert(make_pair(cost, SYUNSIN));
    jitsuesCost.push_back(cost);

    //自落石はとりあえず使わない。
    cin >> cost;
    jitsuesCost.push_back(cost);

    cin >> cost;
    attackJitsu.insert(make_pair(cost, IWA_OTOSHI));
    jitsuesCost.push_back(cost);

    cin >> cost;
    guardJitsu.insert(make_pair(cost, KAMINARI_GAKURE));
    jitsuesCost.push_back(cost);

    //敵雷撃はとりあえず使わない。
    cin >> cost;
    jitsuesCost.push_back(cost);
    
    cin >> cost;
    guardJitsu.insert(make_pair(cost, BUNSIN));
    jitsuesCost.push_back(cost);

    cin >> cost;
    attackJitsu.insert(make_pair(cost, YUUDOU));
    jitsuesCost.push_back(cost);

    cin >> cost;
    guardJitsu.insert(make_pair(cost, TENSOUMETSU));
    jitsuesCost.push_back(cost);

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