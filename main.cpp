#include <iostream>
#include <deque>
#include <string>
#include <random>
#include <vector>
#include <chrono>
#include <thread>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <ctime>
#include <algorithm>
#include <filesystem>
#include <Windows.h>
#include <conio.h>


using namespace std;
using namespace std::chrono;
using namespace std::filesystem;

static const auto FAST_IO = []()
{
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    return 0;
}();

static const auto OUTPUT = []()
{
    SetConsoleOutputCP(CP_UTF8);
    return 0;
}();

const vector<string> texts = {"·", "█", "@"};
const string gameOverText = "GAME OVER!";
const string gameClearText = "GAME CLEAR";
int H = 20;
int W = 40;
const int dx[4] = {-1, 1, 0, 0};
const int dy[4] = {0, 0, -1, 1};
bool gameOver = false;
const int TPS = 120;
const auto TICK = milliseconds(1000 / TPS);
int flushTick;
vector<pair<int, int>> foods;
long long nowTick;
int score;
int F = 1;

enum Dir {
    UP = 0,
    DOWN,
    LEFT,
    RIGHT
};

int randInt(int l, int r) {
    static random_device rd;
    static mt19937 gen(rd());
    uniform_int_distribution<> dist(l, r);
    return dist(gen);
}

class Snake {
    public :

    Dir dir = UP;
    deque<pair<int, int>> body = {{H / 2, W / 2}};

    bool move(vector<pair<int, int>>& foods) {
        int x = body.front().first + dx[dir]; x = (x + H) % H;
        int y = body.front().second + dy[dir]; y = (y + W) % W;
        for(size_t i = 0; i < foods.size(); i++) {
            if(foods[i] == pair<int, int> {x, y}) {
                body.push_front({x, y});
                foods.erase(foods.begin() + i);
                return true;
            }
        }
        body.push_front({x, y});
        body.pop_back();
        return false;
    }
};

void addFood(deque<pair<int, int>> body, vector<pair<int, int>>& foods, int F) {
    pair<int, int> food;
    bool generated = false;
    while(!generated) {
        food = {randInt(0, H - 1), randInt(0, W - 1)};
        generated = true;
        while(!body.empty()) {
            int x = body.front().first;
            int y = body.front().second;
            if(food == pair<int, int> {x, y}) {
                generated = false;
                break;
            }
            body.pop_front();
        }
        for(auto f : foods) {
            if(food == f) {
                generated = false;
                break;
            }
        }
    }
    foods.push_back(food);
}

bool gameOverJudgement(deque<pair<int, int>> body) {
    pair<int, int> head = body.front();
    body.pop_front();
    while(!body.empty()) {
        if(head == body.front()) return true;
        body.pop_front();
    }
    return false;
}

void mkDisplay(vector<vector<int>>& display, deque<pair<int, int>> body, vector<pair<int, int>>& foods) {
    while(!body.empty()) {
        int x = body.front().first;
        int y = body.front().second;
        display[x][y] = 1;
        body.pop_front();
    }
    for(auto food : foods) {
        display[food.first][food.second] = 2;
    }
}

void displayDisplay(vector<vector<int>>& display, int score, bool& gameOver, Snake& snake) {
    cout << "\033[2J\033[1;1H";
    cout << "┌";
    for(int i = 0; i < W; i++) {
        cout << "─";
    }
    cout << "┐\n";
    for(int i = 0; i < H; i++) {
        cout << "│";
        for(int j = 0; j < W; j++) {
            if(gameOver && i == H / 2 && W / 2 - 5 <= j && j <= W / 2 + 4) {
                cout << gameOverText[j - (W / 2 - 5)];
            }
            else if(snake.body.size() + foods.size() == H * W && i == H / 2 && W / 2 - 5 <= j && j <= W / 2 + 4) {
                cout << gameClearText[j - (W / 2 - 5)];
                gameOver = true;
            }
            else {
                cout << texts[display[i][j]];
            }
        }
        cout << "│\n";
    }
    cout << "└";
    for(int i = 0; i < W; i++) {
        cout << "─";
    }
    cout << "┘ " << "SCORE: " << score << '\n';
    cout << flush;
}



class Settings {
    public :

    int width = 40; //20~60
    int height = 20; //10~30
    int speed = 2; //1~3
    int foodSpawn = 1; //1~5

    void reset(void) {
        width = 40; 
        height = 20; 
        speed = 2;
        foodSpawn = 1;
    }
};

void game(const Settings& settings) {
    H = settings.height;
    W = settings.width;
    F = settings.foodSpawn;
    flushTick = 6 - settings.speed;
    vector<vector<int>> display(H, vector<int>(W, 0)); //1 == body, 2 == food;
    Snake snake;
    for(int i = 0; i < F; i++) {
        addFood(snake.body, foods, F);
    }
    while(!gameOver) {
        if(nowTick % flushTick == 0) {
            if(snake.move(foods)) {
                score++;
                addFood(snake.body, foods, F);
            }
        }
        for(auto& r : display) fill(r.begin(), r.end(), 0);
        mkDisplay(display, snake.body, foods);
        gameOver = gameOverJudgement(snake.body);
        displayDisplay(display, score, gameOver, snake);
        if(_kbhit()) {
            char c = _getch();
            if(c == 'w') {
                if(snake.dir != DOWN) {
                    snake.dir = UP;
                }
            }
            if(c == 'a') {
                if(snake.dir != RIGHT) {
                    snake.dir = LEFT;
                }
            }
            if(c == 's') {
                if(snake.dir != UP) {
                    snake.dir = DOWN;
                }
            }
            if(c == 'd') {
                if(snake.dir != LEFT) {
                    snake.dir = RIGHT;
                }
            }
        }
        this_thread::sleep_for(TICK);
        nowTick++;
    }
} 

enum TitleResult {
    START = 0,
    SETTINGS,
    HIGHSCORES,
    EXIT
};

TitleResult titleScreen(void) {

    cout << "\033[2J\033[1;1H";

    cout << R"(

███████╗███╗   ██╗ █████╗ ██╗  ██╗███████╗
██╔════╝████╗  ██║██╔══██╗██║ ██╔╝██╔════╝
███████╗██╔██╗ ██║███████║█████╔╝ █████╗
╚════██║██║╚██╗██║██╔══██║██╔═██╗ ██╔══╝
███████║██║ ╚████║██║  ██║██║  ██╗███████╗
╚══════╝╚═╝  ╚═══╝╚═╝  ╚═╝╚═╝  ╚═╝╚══════╝

        )";
    cout << flush;

    TitleResult result = START;

    cout << '\n';
    cout << "\033[10;1H";
    cout << (result == START ? "        > " : "          ") << "START\n";
    cout << (result == SETTINGS ? "        > " : "          ") << "SETTINGS\n";
    cout << (result == HIGHSCORES ? "        > " : "          ") << "HIGH SCORES\n";
    cout << '\n';
    cout << (result == EXIT ? "        > " : "          ") << "EXIT\n";
    cout << flush;
    while(true) {
        if(_kbhit()) {
            char c = _getch();
            if(c == '\t') {
                result = TitleResult((result + 1) % 4);
                cout << "\033[10;1H";
                cout << (result == START ? "        > " : "          ") << "START\n";
                cout << (result == SETTINGS ? "        > " : "          ") << "SETTINGS\n";
                cout << (result == HIGHSCORES ? "        > " : "          ") << "HIGH SCORES\n";
                cout << '\n';
                cout << (result == EXIT ? "        > " : "          ") << "EXIT\n";
                cout << flush;
            }
            else if(c == '\r') {
                return result;
            }
        }
        this_thread::sleep_for(TICK);
    }
}

enum SettingsItem {
    WIDTH = 0,
    HEIGHT,
    SPEED,
    FOODSPAWN,
    RESET,
    DONE
};

void SettingsScreen(Settings& settings) {
    SettingsItem item = WIDTH;
    cout << "\033[2J\033[1;1H";

    cout << "SETTINGS\n\n";
    cout << (item == WIDTH ? "> " : "  ") << "Width       " << (settings.width > 20 ? "< " : "  ") << settings.width << (settings.width < 60 ? " >" : "  ") << '\n';
    cout << (item == HEIGHT ? "> " : "  ") << "Height      " << (settings.height > 10 ? "< " : "  ") << settings.height << (settings.height < 30 ? " >" : "  ") << '\n';
    cout << (item == SPEED ? "> " : "  ") << "Speed       " << (settings.speed > 1 ? "< " : "  ") << settings.speed << (settings.speed < 3 ? " >" : "  ") << '\n';
    cout << (item == FOODSPAWN ? "> " : "  ") << "Food Spawn  " << (settings.foodSpawn > 1 ? "< " : "  ") << settings.foodSpawn << (settings.foodSpawn < 5 ? " >" : "  ") << '\n';
    cout << '\n';
    cout << (item == RESET ? "> " : "  ") << "Reset\n\n";
    cout << (item == DONE ? "> " : "  ") << "Done\n";
    cout << flush;

    while(true) {
        if(_kbhit()) {
            cout << "\033[2J\033[1;1H";
            int c = _getch();
            if(c == 9) {
                item = SettingsItem((item + 1) % 6);
            }
            if(c == 13 && item == DONE) {
                return;
            }
            if(c == 13 && item == RESET) {
                settings.reset();
            }
            if(c == 0 || c == 224) {
                c = _getch();
                if(c == 77) {
                    if(item == WIDTH) {
                        if(settings.width < 60) {
                            settings.width++;
                        }
                    }
                    else if(item == HEIGHT) {
                        if(settings.height < 30) {
                            settings.height++;
                        }
                    }
                    else if(item == SPEED) {
                        if(settings.speed < 3) {
                            settings.speed++;
                        }
                    }
                    else if(item == FOODSPAWN) {
                        if(settings.foodSpawn < 5) {
                            settings.foodSpawn++;
                        }
                    }
                }
                if(c == 75) {
                    if(item == WIDTH) {
                        if(settings.width > 20) {
                            settings.width--;
                        }
                    }
                    else if(item == HEIGHT) {
                        if(settings.height > 10) {
                            settings.height--;
                        }
                    }
                    else if(item == SPEED) {
                        if(settings.speed > 1) {
                            settings.speed--;
                        }
                    }
                    else if(item == FOODSPAWN) {
                        if(settings.foodSpawn > 1) {
                            settings.foodSpawn--;
                        }
                    }
                }
            }
            cout << "SETTINGS\n\n";
            cout << (item == WIDTH ? "> " : "  ") << "Width       " << (settings.width > 20 ? "< " : "  ") << settings.width << (settings.width < 60 ? " >" : "  ") << '\n';
            cout << (item == HEIGHT ? "> " : "  ") << "Height      " << (settings.height > 10 ? "< " : "  ") << settings.height << (settings.height < 30 ? " >" : "  ") << '\n';
            cout << (item == SPEED ? "> " : "  ") << "Speed       " << (settings.speed > 1 ? "< " : "  ") << settings.speed << (settings.speed < 3 ? " >" : "  ") << '\n';
            cout << (item == FOODSPAWN ? "> " : "  ") << "Food Spawn  " << (settings.foodSpawn > 1 ? "< " : "  ") << settings.foodSpawn << (settings.foodSpawn < 5 ? " >" : "  ") << '\n';
            cout << '\n';
            cout << (item == RESET ? "> " : "  ") << "Reset\n\n";
            cout << (item == DONE ? "> " : "  ") << "Done\n";
            cout << flush;
        }
        this_thread::sleep_for(TICK);
    }
}

enum Restart {
    RESTART = 0,
    SAVESCORE,
    TITLE
};

Restart gameEndedScreen(Settings settings) {
    Restart restart = RESTART;
    int H = settings.height;
    cout << '\n';
    cout << "\033[" << H + 4 << ";1H";
    cout << (restart == RESTART ? "> " : "  ") << "RESTART\n";
    cout << (restart == SAVESCORE ? "> " : "  ") << "SAVE SCORE\n";
    cout << (restart == TITLE ? "> " : "  ") << "TITLE\n";
    cout << flush;
    while(true) {
        if(_kbhit()) {
            cout << "\033[" << H + 4 << ";1H";
            char c = _getch();
            if(c == '\t') {
                restart = Restart((restart + 1) % 3);
                cout << (restart == RESTART ? "> " : "  ") << "RESTART\n";
                cout << (restart == SAVESCORE ? "> " : "  ") << "SAVE SCORE\n";
                cout << (restart == TITLE ? "> " : "  ") << "TITLE\n";
                cout << flush;
            }
            if(c == '\r') {
                return restart;
            }
        }
        this_thread::sleep_for(TICK);
    }
}

struct Record {
    string name;
    int score;
    string datetime;
};

string nowTime() {
    time_t t = time(nullptr);
    tm local;
    localtime_s(&local, &t);
    stringstream ss;
    ss << put_time(&local, "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

void saveRecord(const string& name, int score) {
    create_directories("history");
    ofstream("history/history.txt", ios::app);
    ofstream fout("history/history.txt", ios::app);
    fout << name << "," << score << "," << nowTime() << '\n';
}

vector<Record> loadRecord() {
    vector<Record> records;
    ifstream fin("history/history.txt");
    string line;
    while(getline(fin, line)) {
        stringstream ss(line);
        Record r;
        getline(ss, r.name, ',');
        string score;
        getline(ss, score, ',');
        getline(ss, r.datetime);
        r.score = stoi(score);
        records.push_back(r);
    }
    sort(records.begin(), records.end(), [](const Record& a, const Record& b) {
            return a.score > b.score;
        });
    return records;
}



int main() {

    cout << "\033[?25l";

    bool restart = false;

    Settings settings;

    while(true) {
        
        TitleResult r;
        if(!restart) r = titleScreen();
        else r = START;

        if(r == SETTINGS) {
            SettingsScreen(settings);
        }
        else if(r == START) {
            gameOver = false;
            score = 0;
            nowTick = 0;
            foods.clear();
            game(settings);
            Restart endedResult = gameEndedScreen(settings);
            if(endedResult == RESTART) restart = true;
            else restart = false;
            if(endedResult == SAVESCORE) {
                cout << "\033[" << H + 4 << ";1H";
                cout << "NAME (10 chars max)\n                    \n> __________";
                cout << flush;
                string name;
                while(true){
                    if(_kbhit()) {
                        int c = _getch();
                        if(c == '\r') break;
                        if(c == '\b'){
                            if(!name.empty())
                                name.pop_back();
                        }
                        else if(isprint(c) && name.size() < 10){
                            name.push_back(c);
                        }
                        cout << "\033[" << H + 4 << ";1H";
                        cout << "NAME (10 chars max)\n\n> ";
                        cout << name;
                        for(int i = 0; i < 10 - name.size(); i++) cout << '_';
                        cout << "\n\n";
                        cout << flush;
                    }
                    this_thread::sleep_for(TICK);
                }
                saveRecord(name, score);
                cout << "Saved!\n\nPress Enter to Continue...";
                cout << flush;
                while(true) {
                    if(_kbhit()) {
                        if(_getch() == '\r') {
                            break;
                        }
                    }
                    this_thread::sleep_for(TICK);
                }
            }
        }
        else if(r == HIGHSCORES) {
            vector<Record> records = loadRecord();
            cout << "\033[2J\033[1;1H";
            cout << "HISTORY\n\n RANK  NAME        SCORE  DATE\n\n";
            for(size_t i = 0; i < records.size() && i < 30; i++) {
                cout << left << ' ' << setw(4) << i + 1 << "  ";
                cout << setw(10) << records[i].name;
                cout << "  ";
                cout << setw(5) << records[i].score;
                cout << "  ";
                cout << records[i].datetime;
                cout << '\n';
            }
            cout << '\n';
            cout << "Press Enter to Close\n";
            cout << flush;
            while(true) {
                if(_kbhit()) {
                    if(_getch() == '\r') {
                        break;
                    }
                }
                this_thread::sleep_for(TICK);
            }
        }
        else if(r == EXIT) {
            cout << "\nAre You Sure to Exit ?\n\n";
            cout << flush;
            while(true) {
                cout << "[Y/n]: ";
                cout << flush;
                char c;
                cin >> c;
                if(c == 'Y' || c == 'y') {
                    cout << '\n';
                    return 0;
                }
                else if(c == 'n') break;
                this_thread::sleep_for(TICK);
            }
        }
        this_thread::sleep_for(TICK);
    }
}


