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
const int H = 20;
const int W = 40;
const int dx[4] = {-1, 1, 0, 0};
const int dy[4] = {0, 0, -1, 1};
vector<vector<int>> display(H, vector<int>(W, 0)); //1 == body, 2 == food;
bool gameOver = false;
const int TPS = 32;
const auto TICK = milliseconds(1000 / TPS);
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
    display[food.first][food.second] = 2;
}

void displayDisplay(vector<vector<int>>& display, int score, bool gameOver) {
    cout << "\033[2J\033[H";
    cout << "┌";
    for(int i = 0; i < W; i++) {
        cout << "─";
    }
    cout << "┐\n";
    for(int i = 0; i < H; i++) {
        cout << "│";
        for(int j = 0; j < W; j++) {
            if(gameOver && i == H / 2 && W / 2 - 4 <= j && j <= W / 2 + 5) {
                cout << gameOverText[j - (W / 2 - 4)];
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

void game(void) {
    Snake snake;
    genFood(snake.body, food);
    while(!gameOver) {
        if(nowTick % 3 == 0) {
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

int main() {

    bool restart = false;

    while(true) {

        cout << "\033[2J\033[H";

        if(!restart) {
            cout << R"(

███████╗███╗   ██╗ █████╗ ██╗  ██╗███████╗
██╔════╝████╗  ██║██╔══██╗██║ ██╔╝██╔════╝
███████╗██╔██╗ ██║███████║█████╔╝ █████╗
╚════██║██║╚██╗██║██╔══██║██╔═██╗ ██╔══╝
███████║██║ ╚████║██║  ██║██║  ██╗███████╗
╚══════╝╚═╝  ╚═══╝╚═╝  ╚═╝╚═╝  ╚═╝╚══════╝

        ══ PRESS SPACE TO START ══

            )";

            cout << flush;
        

            while(true) {
                if(_kbhit()) {
                    if(_getch() == ' ') {
                        restart = true;
                        break;
                    }
                }
                this_thread::sleep_for(TICK);
            }

            cout << "\033[2J\033[H";
        }

        nowTick = 0;
        score = 0;
        gameOver = false;
        for(auto& r : display) fill(r.begin(), r.end(), 0);
        game();

        cout << "\n══ PRESS R TO RESTART ══\n";

        cout << flush;

        while(true) {
            if(_kbhit()) {
                if(_getch() == 'r') {
                    break;
                }
            }
            this_thread::sleep_for(TICK);
        }
    }
}

