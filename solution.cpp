#include <iostream>
#include <vector>
#include <stack>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <numeric> // needed for accumulate in compute_score
#include <random>

using namespace std;

void write_board_csv(const vector<vector<int>>& board, bool first) {
    ios_base::openmode mode = ios::app;
    if (first) mode = ios::trunc;
    ofstream fout("game_output.csv", mode);
    if (!fout) return;
    for (int r=0;r<4;r++){
        for (int c=0;c<4;c++){
            fout<<board[r][c];
            if (!(r==3 && c==3)) fout<<",";
        }
    }
    fout<<"\n";
}

void print_board(const vector<vector<int>>& board, bool first) {
    for (const auto &row : board) {
        for (auto& val : row) {
            if (val == 0) cout << ".\t";
            else cout << val << "\t";
        }
        cout << "\n";
    }
    cout << endl;
    write_board_csv(board, first);
}

// TODO: use algorithms to spawn properly
void spawn_tile(vector<vector<int>>& board) {
    vector<vector<int>> empty_spaces;
    for (int r=0;r<4;r++) {
        for (int c=0;c<4;c++) {
            if (board[r][c] == 0) {
                vector<int> point = {r, c};
                empty_spaces.push_back(point);
            }
        }
    }

    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dist1(0, empty_spaces.size()-1);

    vector<int> point = empty_spaces[dist1(gen)];

    uniform_int_distribution<> dist2(1, 10);

    // 10% of the time generate a 4
    if (dist2(gen) == 1) {
        board[point[0]][point[1]] = 4;
    } else {
        board[point[0]][point[1]] = 2;
    }
}

void transpose(vector<vector<int>>& vec) {
    for (int i = 0; i < vec.size(); i++) {
        for (int j = 0; j < i+1; j++) {
            swap(vec[i][j], vec[j][i]);
        }
    }
}

void pad_left(vector<int>& row, const int size, const int num=0) {
    row.insert(row.begin(), size - row.size(), num);
}

void pad_right(vector<int>&row, const int size, const int num=0) {
    row.resize(size, num);
}

// TODO: implement using STL algorithms
bool move_left(vector<vector<int>>& board){
    for (int r=0; r < 4; r++) {
        vector<int> compressed;
        copy_if(board[r].begin(), board[r].end(), back_inserter(compressed), [](int num) {return num != 0;});
        // merging
        pad_right(compressed, 4);
        for (int i=0; i < 3; i++) {
            if (compressed[i] == compressed[i+1]) {
                compressed[i] *= 2;
                compressed[i+1] = 0;
            }
        }
        board[r].clear();
        copy_if(compressed.begin(), compressed.end(), back_inserter(board[r]), [](int num) {return num != 0;});
        pad_right(board[r], 4);
    }
    return true;
}
bool move_right(vector<vector<int>>& board){
    for (int r=0; r < 4; r++) {
        vector<int> compressed;
        copy_if(board[r].begin(), board[r].end(), back_inserter(compressed), [](int num) {return num != 0;});
        pad_left(compressed, 4);
        for (int i=3; i > 0; i--) {
            if (compressed[i] == compressed[i-1]) {
                compressed[i] *= 2;
                compressed[i-1] = 0;
            }
        }
        board[r].clear();
        copy_if(compressed.begin(), compressed.end(), back_inserter(board[r]), [](int num) {return num != 0;});
        pad_left(board[r], 4);
    }

    return true;
}
bool move_up(vector<vector<int>>& board){
    transpose(board);
    move_left(board);
    transpose(board);

    return true;
}
bool move_down(vector<vector<int>>& board){
    transpose(board);
    move_right(board);
    transpose(board);

    return true;
}

// TODO: compute score by summing board values
template <class Board>
int compute_score(const Board& board) {
    int sum{};
    for (auto it = board.begin(); it != board.end(); it++) {
        for (auto jt = (*it).begin(); jt != (*it).end(); jt++) {
            sum += *jt;
        }
    }
    return sum;
}

int main(){
    srand(time(nullptr));
    vector<vector<int>> board(4, vector<int>(4,0));
    spawn_tile(board);
    spawn_tile(board);

    stack<vector<vector<int>>> history;
    bool first=true;

    while(true){
        print_board(board, first);
        first=false;
        cout << "Score: " << compute_score(board) << "\n";
        cout<<"Move (w=up, a=left, s=down, d=right), u=undo, q=quit: ";
        char cmd;
        if (!(cin>>cmd)) break;
        if (cmd=='q') break;

        if (cmd=='u') {
            // TODO: Implement undo handling here using the history stack
            if (!history.empty()) {
                vector<vector<int>> past_board = history.top();
                board = past_board;
                history.pop();
            }
            print_board(board,false);
            continue;
        }

        vector<vector<int>> prev = board;
        bool moved=false;
        if (cmd=='a') moved=move_left(board);
        else if (cmd=='d') moved=move_right(board);
        else if (cmd=='w') moved=move_up(board);
        else if (cmd=='s') moved=move_down(board);

        if (moved) {
            // TODO: push prev to history stack for undo
            history.push(prev);
            spawn_tile(board);
        }
    }
    return 0;
}
