#include <bits/stdc++.h>
#ifdef _WIN32
#include <windows.h>
#include <conio.h>
#ifndef ENABLE_VIRTUAL_TERMINAL_PROCESSING 
#define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004
#endif
#else
#endif

using namespace std;

struct Cell { 
    int value; 
    bool is_fixed; 
};

struct PlayerScore { 
    string name; 
    int score; 
    time_t time; 
    PlayerScore* next; 
};

struct BoardState {
    vector<vector<int>> data;
    BoardState* next;
};

class SudokuGame {
private:
    vector<vector<Cell>> board;
    BoardState* undo_stack;
    int level;
    time_t start_time;
    bool game_over;
    int mistakes;
    vector<vector<int>> initial_board;
    int cursorX, cursorY;
    PlayerScore* leaderboard;
    string current_player;
    bool paused;

    #ifndef _WIN32
    int getch() {
        struct termios oldt, newt;
        int ch;
        tcgetattr(STDIN_FILENO, &oldt);
        newt = oldt;
        newt.c_lflag &= ~(ICANON | ECHO);
        tcsetattr(STDIN_FILENO, TCSANOW, &newt);
        ch = getchar();
        tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
        return ch;
    }

    int kbhit() {
        struct termios oldt, newt;
        int ch;
        int oldf;

        tcgetattr(STDIN_FILENO, &oldt);
        newt = oldt;
        newt.c_lflag &= ~(ICANON | ECHO);
        tcsetattr(STDIN_FILENO, TCSANOW, &newt);
        oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
        fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

        ch = getchar();

        tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
        fcntl(STDIN_FILENO, F_SETFL, oldf);

        if (ch != EOF) {
            ungetc(ch, stdin);
            return 1;
        }
        return 0;
    }
    #else
    int getch() { return ::_getch(); }
    int kbhit() { return ::_kbhit(); }
    #endif

    string drawHeader() {
        stringstream ss;
        time_t current_time = time(nullptr);
        int remaining = 0;
        switch(level) {
            case 1: remaining = 300 - difftime(current_time, start_time); break;
            case 2: remaining = 180 - difftime(current_time, start_time); break;
            case 3: remaining = 120 - difftime(current_time, start_time); break;
        }

        if(remaining <= 0) {
            game_over = true;
            ss << "Time's up! Game over!\n";
            return ss.str();
        }

        ss << "Player: " << current_player << " | "
           << "Time: " << remaining/60 << ":" << setw(2) << setfill('0') << remaining%60 << " | "
           << "Mistakes: " << mistakes << "/3\n"
           << "Use arrows to move, numbers to input\n"
           << "U: Undo | S: Save | Q: Quit | ESC: Pause\n";
        return ss.str();
    }

    string drawBoard() {
        stringstream ss;
        ss << "\n";
        for(int i=0; i<9; i++) {
            if(i % 3 == 0 && i != 0) {
                ss << " ------+-------+------\n";
            }
            for(int j=0; j<9; j++) {
                if(j % 3 == 0 && j != 0) ss << "| ";
                if(i == cursorY && j == cursorX) ss << "[";
                else if(board[i][j].is_fixed) ss << "(";
                else ss << " ";

                ss << (board[i][j].value == 0 ? ' ' : static_cast<char>(board[i][j].value + '0'));

                if(i == cursorY && j == cursorX) ss << "]";
                else if(board[i][j].is_fixed) ss << ")";
                else ss << " ";
            }
            ss << "\n";
        }
        return ss.str();
    }

    void saveState() {
        BoardState* newState = new BoardState;
        newState->data.resize(9, vector<int>(9));
        for(int i=0; i<9; i++) {
            for(int j=0; j<9; j++) {
                newState->data[i][j] = board[i][j].value;
            }
        }
        newState->next = undo_stack;
        undo_stack = newState;
    }

    bool generateBoard(int row, int col) {
        if(row == 9) return true;
        if(col == 9) return generateBoard(row+1, 0);
        
        vector<int> nums = {1,2,3,4,5,6,7,8,9};
        shuffle(nums.begin(), nums.end(), mt19937(random_device()()));
        
        for(int num : nums) {
            if(isValid(row, col, num)) {
                board[row][col].value = num;
                if(generateBoard(row, col+1)) return true;
                board[row][col].value = 0;
            }
        }
        return false;
    }

    void removeCells(int difficulty) {
        int cellsToRemove;
        switch(difficulty) {
            case 1: cellsToRemove = 30; break;
            case 2: cellsToRemove = 40; break;
            case 3: cellsToRemove = 50; break;
            default: cellsToRemove = 30;
        }
        
        while(cellsToRemove > 0) {
            int row = rand()%9;
            int col = rand()%9;
            if(board[row][col].value != 0) {
                initial_board[row][col] = board[row][col].value;
                board[row][col].value = 0;
                board[row][col].is_fixed = false;
                cellsToRemove--;
            }
        }
    }

    bool isValid(int row, int col, int num) {
        for(int i=0; i<9; i++) {
            if(board[row][i].value == num) return false;
            if(board[i][col].value == num) return false;
        }
        
        int startRow = row - row%3;
        int startCol = col - col%3;
        for(int i=0; i<3; i++) {
            for(int j=0; j<3; j++) {
                if(board[startRow+i][startCol+j].value == num) return false;
            }
        }
        return true;
    }

    void updateLeaderboard() {
        int score = 0;
        for(int i=0; i<9; i++) {
            for(int j=0; j<9; j++) {
                if(board[i][j].value != 0 && !board[i][j].is_fixed) score += 10;
            }
        }
        score -= (mistakes * 50);

        PlayerScore* newScore = new PlayerScore{
            current_player, 
            score, 
            time(nullptr) - start_time, 
            leaderboard
        };
        leaderboard = newScore;
        sortLeaderboard();
        saveLeaderboard();
    }

    void sortLeaderboard() {
        if(!leaderboard) return;
        bool sorted;
        do {
            sorted = true;
            PlayerScore **pp = &leaderboard;
            while(*pp && (*pp)->next) {
                if((*pp)->score < (*pp)->next->score || 
                  ((*pp)->score == (*pp)->next->score && (*pp)->time > (*pp)->next->time)) {
                    PlayerScore* temp = *pp;
                    *pp = (*pp)->next;
                    temp->next = (*pp)->next;
                    (*pp)->next = temp;
                    sorted = false;
                }
                pp = &(*pp)->next;
            }
        } while(!sorted);
    }

    void saveLeaderboard() {
        ofstream file("leaderboard.txt");
        PlayerScore* current = leaderboard;
        while(current) {
            file << current->name << " "
                 << current->score << " "
                 << current->time << "\n";
            current = current->next;
        }
    }

    void loadLeaderboard() {
        ifstream file("leaderboard.txt");
        string name;
        int score;
        time_t timeVal;
        while(file >> name >> score >> timeVal) {
            PlayerScore* newScore = new PlayerScore{name, score, timeVal, leaderboard};
            leaderboard = newScore;
        }
        sortLeaderboard();
    }

    void showPauseMenu() {
        stringstream ss;
        //Clear the initial board 
        ss << "\033[2J\033[1;1H";
        ss << "=== PAUSE MENU ===\n"
           << "1. Resume Game\n"
           << "2. Save Game\n"
           << "3. Main Menu\n"
           << "Choice: ";
        cout << ss.str() << flush;
        
        int choice;
        cin >> choice;
        if(choice == 2) saveGame();
        if(choice == 3) game_over = true;
    }

    void showSaveMessage() {
        stringstream ss;
        ss << "\033[1;1H";
        ss << "Game saved successfully!\n";
        ss << "Press any key to continue...";
        cout << ss.str() << flush;
        this->getch();
    }

public:
    SudokuGame() : undo_stack(nullptr), leaderboard(nullptr), paused(false) {
        board.resize(9, vector<Cell>(9, {0, false}));
        initial_board.resize(9, vector<int>(9, 0));
        #ifdef _WIN32
        HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        CONSOLE_CURSOR_INFO cursorInfo;
        GetConsoleCursorInfo(hConsole, &cursorInfo);
        cursorInfo.bVisible = false;
        SetConsoleCursorInfo(hConsole, &cursorInfo);
        #endif
        loadLeaderboard();
    }

    ~SudokuGame() {
        while(undo_stack) {
            BoardState* temp = undo_stack;
            undo_stack = undo_stack->next;
            delete temp;
        }
        
        PlayerScore* current = leaderboard;
        while(current) {
            PlayerScore* next = current->next;
            delete current;
            current = next;
        }
        
        #ifdef _WIN32
        HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        CONSOLE_CURSOR_INFO cursorInfo;
        GetConsoleCursorInfo(hConsole, &cursorInfo);
        cursorInfo.bVisible = true;
        SetConsoleCursorInfo(hConsole, &cursorInfo);
        #endif
    }

    void clearScreen() {
        #ifdef _WIN32
        system("cls");
        #else
        cout << "\033[2J\033[1;1H";
        #endif
    }

    void saveGame() {
        ofstream file("savegame.sud");
        if(file) {
            file << current_player << "\n"
                 << level << "\n"
                 << mistakes << "\n"
                 << difftime(time(nullptr), start_time) << "\n"
                 << cursorX << "\n"
                 << cursorY << "\n";
            
            for(int i=0; i<9; i++) {
                for(int j=0; j<9; j++) {
                    file << board[i][j].value << " ";
                }
                file << "\n";
            }
            
            for(int i=0; i<9; i++) {
                for(int j=0; j<9; j++) {
                    file << initial_board[i][j] << " ";
                }
                file << "\n";
            }
            file.close();
            this->showSaveMessage();
        } else {
            stringstream ss;
            ss << "\033[1;1H";
            ss << "Error saving game!\n";
            ss << "Press any key to continue...";
            cout << ss.str() << flush;
            this->getch();
        }
    }

    bool loadGame() {
        ifstream file("savegame.sud");
        if(file) {
            board.clear();
            initial_board.clear();
            
            board.resize(9, vector<Cell>(9, {0, false}));
            initial_board.resize(9, vector<int>(9, 0));

            file >> current_player >> level >> mistakes;
            time_t elapsed;
            file >> elapsed >> cursorX >> cursorY;
            start_time = time(nullptr) - elapsed;
            
            for(int i=0; i<9; i++) {
                for(int j=0; j<9; j++) {
                    file >> board[i][j].value;
                }
            }
            
            for(int i=0; i<9; i++) {
                for(int j=0; j<9; j++) {
                    file >> initial_board[i][j];
                    board[i][j].is_fixed = (board[i][j].value == initial_board[i][j]);
                }
            }
            file.close();
            return true;
        }
        return false;
    }

    void showLeaderboard() {
        stringstream ss;
        ss << "\033[2J\033[1;1H";
        ss << "=== LEADERBOARD ===\n";
        PlayerScore* current = leaderboard;
        int rank = 1;
        while(current && rank <= 10) {
            ss << rank++ << ". " << current->name 
               << " - Score: " << current->score
               << " (Time: " << current->time/60 << "m " << current->time%60 << "s)\n";
            current = current->next;
        }
        ss << "\nPress any key to continue...";
        cout << ss.str() << flush;
        this->getch();
    }

    void startGame() {
        clearScreen();
        cout << "Enter your name: ";
        cin >> current_player;

        cout << "Select difficulty (1-3): ";
        while(!(cin >> level) || level < 1 || level > 3) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid input! Enter 1-3: ";
        }

        board = vector<vector<Cell>>(9, vector<Cell>(9, {0, false}));
        if(!generateBoard(0, 0)) {
            cerr << "Failed to generate board!\n";
            return;
        }

        for(int i=0; i<9; i++) {
            for(int j=0; j<9; j++) {
                initial_board[i][j] = board[i][j].value;
                board[i][j].is_fixed = (board[i][j].value != 0);
            }
        }

        removeCells(level);
        start_time = time(nullptr);
        saveState();
        game_over = false;
        cursorX = cursorY = 0;
        mistakes = 0;

        #ifdef _WIN32
        HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
        DWORD dwMode = 0;
        GetConsoleMode(hOut, &dwMode);
        if (!(dwMode & ENABLE_VIRTUAL_TERMINAL_PROCESSING)) {
            dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
            SetConsoleMode(hOut, dwMode);
        }
        #endif

        while(!game_over) {
            stringstream buffer;
            buffer << "\033[1;1H";
            buffer << drawHeader();
            buffer << drawBoard();
            buffer << "\033[J";
            cout << buffer.str() << flush;

            if(this->kbhit()) {
                int ch = this->getch();
                switch(ch) {
                    case 72: if(cursorY > 0) cursorY--; break; // Up
                    case 80: if(cursorY < 8) cursorY++; break; // Down
                    case 75: if(cursorX > 0) cursorX--; break; // Left
                    case 77: if(cursorX < 8) cursorX++; break; // Right
                    case 'u': case 'U': // Undo
                        if(undo_stack && undo_stack->next) {
                            BoardState* temp = undo_stack;
                            undo_stack = undo_stack->next;
                            for(int i=0; i<9; i++) {
                                for(int j=0; j<9; j++) {
                                    board[i][j].value = undo_stack->data[i][j];
                                }
                            }
                            delete temp;
                        }
                        break;
                    case 's': case 'S': // Save
                        saveGame();
                        break;
                    case 27: // ESC
                        showPauseMenu();
                        break;
                    case 'q': case 'Q': // Quit
                        game_over = true;
                        break;
                    default:
                        if(ch >= '1' && ch <= '9' && !board[cursorY][cursorX].is_fixed) {
                            int num = ch - '0';
                            if(isValid(cursorY, cursorX, num)) {
                                saveState();
                                board[cursorY][cursorX].value = num;
                            } else {
                                if(++mistakes >= 3) game_over = true;
                            }
                        }
                }
            }

            bool win = true;
            for(int i=0; i<9; i++) {
                for(int j=0; j<9; j++) {
                    if(board[i][j].value == 0) {
                        win = false;
                        break;
                    }
                }
                if(!win) break;
            }
            
            if(win) {
                clearScreen();
                cout << "Congratulations! You won!\n";
                updateLeaderboard();
                game_over = true;
            }
            #ifdef _WIN32
            Sleep(50);
            #else
            usleep(50000);
            #endif
        }
    }
};

int main() {
    #ifdef _WIN32
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut != INVALID_HANDLE_VALUE) {
        DWORD dwMode = 0;
        GetConsoleMode(hOut, &dwMode);
        if (!(dwMode & ENABLE_VIRTUAL_TERMINAL_PROCESSING)) {
            dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
            SetConsoleMode(hOut, dwMode);
        }
    }
    #endif

    SudokuGame game;
    while(true) {
        game.clearScreen();
        cout << "=== SUDOKU ===\n"
             << "1. New Game\n"
             << "2. Load Game\n"
             << "3. Leaderboard\n"
             << "4. Exit\n"
             << "Choice: ";

        int choice;
        cin >> choice;
        switch(choice) {
            case 1: 
                game.startGame();
                break;
            case 2: 
                if(game.loadGame()) {
                    game.startGame();
                } else {
                    cout << "No saved game found!\n";
                    #ifdef _WIN32
                    Sleep(1000);
                    #else
                    usleep(1000000);
                    #endif
                }
                break;
            case 3: 
                game.showLeaderboard();
                break;
            case 4: 
                return 0;
            default:
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << "Invalid choice!\n";
                #ifdef _WIN32
                Sleep(1000);
                #else
                usleep(1000000);
                #endif
        }
    }
}
