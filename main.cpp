#include <iostream>
#include <deque>
#include <string>
#include <random>
#include <vector>
#include <Windows.h>
#include <conio.h>
#include <chrono>
#include <thread>


using namespace std;
using namespace std::chrono;

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
const int TPS = 128;
const auto TICK = milliseconds(1000 / TPS);
int flushTick = 7;
pair<int, int> food;
long long nowTick;
int score;

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

    bool move(pair<int, int> food) {
        int x = body.front().first + dx[dir]; x = (x + H) % H;
        int y = body.front().second + dy[dir]; y = (y + W) % W;
        if(food == pair<int, int> {x, y}) {
            body.push_front({x, y});
            return true;
        }
        else {
            body.push_front({x, y});
            body.pop_back();
            return false;
        }
    }
};

void genFood(deque<pair<int, int>> body, pair<int, int>& food) {
    bool generated = false;
    if(body.size() == H * W) {
        generated = true;
        food = {-1, -1};
    }
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
    }
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

void mkDisplay(vector<vector<int>>& display, deque<pair<int, int>> body, pair<int, int> food) {
    while(!body.empty()) {
        int x = body.front().first;
        int y = body.front().second;
        display[x][y] = 1;
        body.pop_front();
    }
    if(food != pair<int, int> {-1, -1}) display[food.first][food.second] = 2;
}

void displayDisplay(vector<vector<int>>& display, int score, bool& gameOver) {
    cout << "\033[2J\033[H";
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
            else if(food == pair<int, int> {-1, -1} && i == H / 2 && W / 2 - 5 <= j && j <= W / 2 + 4) {
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

enum Difficulty {
    EASY = 0,
    NORMAL,
    HARD
};

class Settings {
    public :

    Difficulty difficulty = NORMAL;
    int width = 40; //20~60
    int height = 20; //10~30
    int speed = 3; //1~5

    void reset(void) {
        difficulty = NORMAL;
        width = 40; 
        height = 20; 
        speed = 3;
    }
};

void game(Settings settings) {
    H = settings.height;
    W = settings.width;
    flushTick = 10 - settings.speed;
    vector<vector<int>> display(H, vector<int>(W, 0)); //1 == body, 2 == food;
    Snake snake;
    genFood(snake.body, food);
    while(!gameOver) {
        if(nowTick % flushTick == 0) {
            if(snake.move(food)) {
                score++;
                genFood(snake.body, food);
            }
            for(auto& r : display) fill(r.begin(), r.end(), 0);
            mkDisplay(display, snake.body, food);
            gameOver = gameOverJudgement(snake.body);
            displayDisplay(display, score, gameOver);
        }
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
    EXIT
};

TitleResult titleScreen(void) {

    cout << "\033[2J\033[H";

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
    cout << (result == EXIT ? "        > " : "          ") << "EXIT\n";
    cout << flush;
    while(true) {
        if(_kbhit()) {
            char c = _getch();
            if(c == '\t') {
                result = TitleResult((result + 1) % 3);
                cout << "\033[10;1H";
                cout << (result == START ? "        > " : "          ") << "START\n";
                cout << (result == SETTINGS ? "        > " : "          ") << "SETTINGS\n";
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
    RESET,
    DONE
};

void SettingsScreen(Settings& settings) {
    SettingsItem item = WIDTH;
    cout << "\033[2J\033[H";

    cout << "SETTINGS\n\n";
    cout << (item == 0 ? "> " : "  ") << "Width      : " << settings.width << '\n';
    cout << (item == 1 ? "> " : "  ") << "Height     : " << settings.height << '\n';
    cout << (item == 2 ? "> " : "  ") << "Speed      : " << settings.speed << "\n\n";
    cout << (item == 3 ? "> " : "  ") << "Reset\n\n";
    cout << (item == 4 ? "> " : "  ") << "Done\n";
    cout << flush;

    while(true) {
        if(_kbhit()) {
            cout << "\033[2J\033[H";
            int c = _getch();
            if(c == 9) {
                item = SettingsItem((item + 1) % 5);
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
                        if(settings.speed < 5) {
                            settings.speed++;
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
                }
            }
            cout << "SETTINGS\n\n";
            cout << (item == 0 ? "> " : "  ") << "Width      : " << settings.width << '\n';
            cout << (item == 1 ? "> " : "  ") << "Height     : " << settings.height << '\n';
            cout << (item == 2 ? "> " : "  ") << "Speed      : " << settings.speed << "\n\n";
            cout << (item == 3 ? "> " : "  ") << "Reset\n\n";
            cout << (item == 4 ? "> " : "  ") << "Done\n";
            cout << flush;
        }
        this_thread::sleep_for(TICK);
    }
}

enum Restart {
    RESTART = 0,
    TITLE
};

Restart gameEndedScreen(Settings settings) {
    Restart restart = RESTART;
    int H = settings.height;
    cout << '\n';
    cout << "\033[" << H + 4 << ";1H";
    cout << (restart == RESTART ? "> " : "  ") << "RESTART\n";
    cout << (restart == TITLE ? "> " : "  ") << "TITLE\n";
    cout << flush;
    while(true) {
        if(_kbhit()) {
            cout << "\033[" << H + 4 << ";1H";
            char c = _getch();
            if(c == '\t') {
                if(restart == TITLE) restart = RESTART;
                else restart = TITLE;
                cout << (restart == RESTART ? "> " : "  ") << "RESTART\n";
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
            game(settings);
            if(gameEndedScreen(settings) == RESTART) restart = true;
            else restart = false;
        }
        else if(r == EXIT) {
            cout << "\nAre You Sure to Exit ?\n\n";
            cout << flush;
            while(true) {
                cout << "[Y/n]: ";
                cout << flush;
                char c;
                cin >> c;
                if(c == 'Y' || c == 'y') return 0;
                else if(c == 'n') break;
                this_thread::sleep_for(TICK);
            }

        }


        
        this_thread::sleep_for(TICK);
    }
    
}

