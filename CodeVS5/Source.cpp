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
std::vector<int> jitsuCost;

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
    Cells(int height, int width) : field(height, std::vector<Cell>(width)) {}
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
    JITSUName firstJitsu;
    Point firstPoint;
    bool isFirst;
    States(State mySt, State riSt) :firstSteps(2), firstPoint(mySt.Height, mySt.Width)
    {
        this->mySt = mySt;
        this->riSt = riSt;
        firstJitsu = MU;
        isFirst = true;
    }
    States(State mySt, State riSt, std::vector<int> firstSteps, JITSUName firstJitsu, Point firstPoint)
    {
        this->mySt = mySt;
        this->riSt = riSt;
        this->firstSteps = firstSteps;
        this->firstJitsu = firstJitsu;
        this->firstPoint = firstPoint;
        isFirst = false;
    }
};

State myState;
State rivalState;
std::vector<std::vector<Direction>> enableSteps;
JITSUName enableJitsu = MU;

int evaluate(State& mySt, State& riSt)
{
    for (auto& ninjaId : mySt.ninjas.ids)
    {
        Point ninjaPoint = mySt.ninjas.getPoint(ninjaId);
        if (mySt.dogs.field[ninjaPoint.row][ninjaPoint.col] != NONE) 
            return -NONE;
    }

    if (jitsuCost[TENSOUMETSU] <= 10 && jitsuCost[TENSOUMETSU] <= mySt.jitsuPoint)
    {
        //return Œ¢‚ªŽü‚è‚É‘½‚¢‚Ù‚ÇA‘å‚«‚¢•]‰¿
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

    return (100-soulMinDist0 + -soulMinDist1 + mySt.jitsuPoint * 100);
}

std::tuple<JITSUName,Point,std::string,std::string> predict(State mySt,State riSt)
{
    using std::vector;
    volatile int dogSize = mySt.dogs.ids.size();
    JITSUName useJitsu = MU;
    Point usePoint = Point(NONE, NONE);
    if (useJitsu == MU && jitsuCost[TENSOUMETSU] <= 10 && jitsuCost[TENSOUMETSU] <= mySt.jitsuPoint)
    {
        
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
    auto ret = make_tuple(MU, Point(NONE, NONE), std::string(""), std::string(""));
    int maxScore = -NONE;
    std::multimap<int, States,std::greater<int>> scores;

    vector<States> goodChoice;
    std::queue<States> statesQ;
    statesQ.push(States(mySt, riSt));
    int count = 0;
    while (count < 3)
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
                    //”EŽÒ‚ª•à‚­
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
                                    newMySt.dogs.field[nextRow + dRow[dir]][nextCol + dCol[dir]] != NONE ||
                                    newMySt.ninjas.field[nextRow + dRow[dir]][nextCol + dCol[dir]].ids.size() != 0) return false;
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
                    //std::cerr << "Œ¢ˆÚ“®‘O:" << std::endl;
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
                    //Œ¢‚ª•à‚­
                    Point ninjaPoint0 = newMySt.ninjas.getPoint(0);
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
                    //std::cerr << "Œ¢ˆÚ“®Œã:" << std::endl;
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
                            scores.insert(std::make_pair(score, States(newMySt, riSt, firstSteps, MU, Point(NONE, NONE))));
                        }
                        else
                        {
                            scores.insert(std::make_pair(score, States(newMySt, riSt, states.firstSteps, states.firstJitsu, states.firstPoint)));
                        }
                    }
                }
            }
        }
    }
    
    volatile int size = goodChoice.size();
    if (goodChoice.size() != 0)
    {
        States bestStates = *(goodChoice.end() - 1);
        vector<std::string> stepsStr(2, std::string(""));
        for (auto& dir : enableSteps[bestStates.firstSteps[0]])
        {
            stepsStr[0] += d[dir];
        }
        for (auto& dir : enableSteps[bestStates.firstSteps[1]])
        {
            stepsStr[1] += d[dir];
        }
        ret = make_tuple(bestStates.firstJitsu, bestStates.firstPoint, stepsStr[0], stepsStr[1]);
    }
    else
    {

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
    if (get<0>(nextAction) == MU) cout << 2 << endl;
    else if (get<0>(nextAction) == SYUNSIN)
    {
        cout << 3 << endl;
        cout << get<0>(nextAction) << endl;
    }
    else if (get<0>(nextAction) == TENSOUMETSU)
    {
        cout << 3 << endl;
        cout << get<0>(nextAction) << " " << get<1>(nextAction).row << endl;
    }
    else
    {
        cout << 3 << endl;
        cout << get<0>(nextAction) << " " << get<1>(nextAction).row << " " << get<1>(nextAction).col << endl;
    }
    cout << get<2>(nextAction) << endl;
    cout << get<3>(nextAction) << endl;
}


bool input()
{
    using namespace std;
    if (!(cin >> remainingTime)) return false;

    int numOfSkills;
    cin >> numOfSkills;
    
    jitsuCost.clear();
    for (int i = 0; i < numOfSkills; i++)
    {
        int cost;
        cin >> cost;
        jitsuCost.push_back(cost);
    }

    if (jitsuCost[SYUNSIN] <= 3)
    {
        enableJitsu = SYUNSIN;
    }
    else if (jitsuCost[TENSOUMETSU] <= 10)
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