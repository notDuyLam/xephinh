#include "Header.h"

void gotoxy(int x, int y) {
    // Get the console handle
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

    // Create a COORD structure and set the coordinates
    COORD pos;
    pos.X = x;
    pos.Y = y;

    // Set the console cursor position
    SetConsoleCursorPosition(hConsole, pos);
}

// 0: hide
// 1: show
void ShowConsoleCursor(bool showFlag) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO cursorInfo;

    GetConsoleCursorInfo(hConsole, &cursorInfo);
    cursorInfo.bVisible = showFlag; // set the cursor visibility
    SetConsoleCursorInfo(hConsole, &cursorInfo);
}

static BOOL SetConsoleSize(int cols, int rows) {
    HWND hWnd;
    HANDLE hConOut;
    CONSOLE_FONT_INFO fi;
    CONSOLE_SCREEN_BUFFER_INFO bi;
    int w, h, bw, bh;
    RECT rect = { 0, 0, 0, 0 };
    COORD coord = { 0, 0 };
    hWnd = GetConsoleWindow();
    if (hWnd) {
        hConOut = GetStdHandle(STD_OUTPUT_HANDLE);
        if (hConOut && hConOut != (HANDLE)-1) {
            if (GetCurrentConsoleFont(hConOut, FALSE, &fi)) {
                if (GetClientRect(hWnd, &rect)) {
                    w = rect.right - rect.left;
                    h = rect.bottom - rect.top;
                    if (GetWindowRect(hWnd, &rect)) {
                        bw = rect.right - rect.left - w;
                        bh = rect.bottom - rect.top - h;
                        if (GetConsoleScreenBufferInfo(hConOut, &bi)) {
                            coord.X = bi.dwSize.X;
                            coord.Y = bi.dwSize.Y;
                            if (coord.X < cols || coord.Y < rows) {
                                if (coord.X < cols) {
                                    coord.X = cols;
                                }
                                if (coord.Y < rows) {
                                    coord.Y = rows;
                                }
                                if (!SetConsoleScreenBufferSize(hConOut, coord)) {
                                    return FALSE;
                                }
                            }
                            return SetWindowPos(hWnd, NULL, rect.left, rect.top, cols * fi.dwFontSize.X + bw, rows * fi.dwFontSize.Y + bh, SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOOWNERZORDER | SWP_NOZORDER);
                        }
                    }
                }
            }
        }
    }
    return FALSE;
}


class block {
private:
    int x, y;
    static int tetrominoes[7][4][4];
    int shape[4][4];
    int lastRow;
    int lastCol;
public:
    block() {
        x = 67;
        y = 7;
    }
    void randomShape() {
        int r = rand() % 7;
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                shape[i][j] = tetrominoes[r][i][j];
            }
        }
        if (r == 1)
        {
            lastCol = 4;
            lastRow = 1;
        }
        else if (r == 2)
        {
            lastCol = 2;
            lastRow = 2;
        }
        else {
            lastCol = 3;
            lastRow = 2;
        }
    }
    void randomPos() {
        x = 36 + rand() % (64 - 36 + 1);
        y = 6;
    }
    void draw() {
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                gotoxy(x + j, y + i);
                if (shape[i][j] == 1)
                    cout << char(219);
            }
        }
    }
    pair<int, int> getXY() {
        return { x, y };
    }
    int getShape(int i, int j) {
        return shape[i][j];
    }
    void clear() {
        for (int i = 0; i < lastRow; i++) {
            for (int j = 0; j < lastCol; j++) {
                if (shape[i][j] == 1)
                {
                    gotoxy(x + j, y + i);
                    cout << " ";
                }
            }
        }
    }
    void move() {
        clear();
        if (_kbhit()) {
            // Lấy phím được nhấn
            char ch = _getch();
            // Kiểm tra nếu phím 'A' được nhấn
            if (ch == 'A' || ch == 'a') {
                x--;
            }
            else if (ch == 'D' || ch == 'd') {
                x++;
            }
        }
        y++;
        clear();
        draw();
    }
    int getLastRow() {
        return lastRow;
    }
    int getLastCol() {
        return lastCol;
    }
    void copyBlock(block other) {
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                this->shape[i][j] = other.shape[i][j];
            }
        }
        
    }
};
int block::tetrominoes[7][4][4] = {
    {{1, 1, 1, 1},
     {0, 0, 0, 0},
     {0, 0, 0, 0},
     {0, 0, 0, 0}},

    {{1, 1},
     {1, 1}},

    {{1, 1, 1},
     {0, 1, 0},
     {0, 0, 0}},

    {{1, 1, 1},
     {1, 0, 0},
     {0, 0, 0}},

    {{1, 1, 1},
     {0, 0, 1},
     {0, 0, 0}},

    {{1, 1, 0},
     {0, 1, 1},
     {0, 0, 0}},

    {{0, 1, 1},
     {1, 1, 0},
     {0, 0, 0}}
};

class game {
private:
    vector<vector<int>> game_map;
    float speed;
    block dropping_bl;
    block incoming_bl; 
public:
    // 29, 19
    game() : game_map(31, vector<int>(40, 0)) {};
    void draw_map() {
        //draw the top
        gotoxy(35, 5);
        cout << "+";
        for (int i = 1; i <= 29; i++)
            cout << "-";
        cout << "+";
        // draw the middle
        for(int i=6;i<=24;i++) 
        {
            gotoxy(35, i);
            cout << "|";
            gotoxy(65, i);
            cout << "|";
        }
        //draw the bottom
        gotoxy(35, 25);
        cout << "+";
        for (int i = 1; i <= 29; i++)
            cout << "-";
        cout << "+";
            //draw on the game_map 
        //draw the top
        for (int i = 0; i < 30; i++) {
            game_map[i][0] = 1;
        }
        //draw the middle
        for (int i = 0; i < 20; i++) {
            game_map[0][i] = 1;
            game_map[29][i] = 1;
        }
        //draw the bottom
        for (int i = 0; i < 30; i++) {
            game_map[i][19] = 1;
        }
    }
    void draw_on_map() {
        pair<int, int> cords = dropping_bl.getXY();
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                if (dropping_bl.getShape(i, j) == 1)
                    game_map[cords.first + j - 36][cords.second + i - 6+1] = 1;
            }
        }
    }
    void clear_on_map() {
        pair<int, int> cords = dropping_bl.getXY();
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                if (dropping_bl.getShape(i, j) == 1)
                    game_map[cords.first + j - 36][cords.second + i - 6] = 0;
            }
        }
    }
    bool checkCollision() {
        bool flag1 = false;
        bool flag2 = false;
        pair<int, int> cords = dropping_bl.getXY();
        for (int i = 0; i < dropping_bl.getLastCol(); i++) {
            flag1 = game_map[cords.first + i - 36][cords.second - 6 + dropping_bl.getLastRow()] == 1;
            flag2 = cords.second - 6 + dropping_bl.getLastRow() >= 20;
            if (flag1 || flag2)
                return true;
        }
        return false;
    }
    void start_game() {
        dropping_bl.randomShape();
        incoming_bl.randomShape();
        dropping_bl.randomPos();
        dropping_bl.draw();
        incoming_bl.draw();
        draw_on_map();
        int delay = 50;
        game_map;
        while (true) {
            
            //delay -= s.getScore();
            Sleep(delay);
            
            dropping_bl.move();
            if (checkCollision()) {
                dropping_bl.copyBlock(incoming_bl);
                dropping_bl.randomPos();
                incoming_bl.randomShape();
                continue;
            }

            clear_on_map();
            draw_on_map();

        }
    }
};

int main() {
    ShowConsoleCursor(0);
    SetConsoleSize(WIDTH, HEIGHT);
    system("cls");
    game g;
    g.draw_map();
    g.start_game();
    return 0;
}
