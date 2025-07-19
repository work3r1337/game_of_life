#include <ncurses.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#define LINES 25
#define COLUMNS 80
#define MIN_DELAY 100
#define MAX_DELAY 1000
#define STATIC_FIELD LINES *COLUMNS

typedef enum { CELL_DEAD = 0, CELL_ALIVE = 1 } CellState;

typedef enum { PLAY, STOP, ERROR } Status;

bool input_generation(int generation[LINES][COLUMNS]);

void update_generation(int curr_generation[LINES][COLUMNS], int next_generation[LINES][COLUMNS]);

int count_neighbors(int generation[LINES][COLUMNS], int i, int j);

bool should_live(int neighbors, bool is_alive);

bool is_static(const int curr_generation[LINES][COLUMNS], const int next_generation[LINES][COLUMNS]);

int change_speed(char control_button, Status *status, int time_mili_sec);

void draw_generation(const int generation[LINES][COLUMNS]);

int main() {
    int curr_generation[LINES][COLUMNS];
    int next_generation[LINES][COLUMNS];
    int time_mili_sec = (MAX_DELAY - MIN_DELAY) / 2;
    Status status = PLAY;
    if (!input_generation(curr_generation) || !freopen("/dev/tty", "r", stdin))
        status = ERROR;
    else {
        initscr();
        nodelay(stdscr, true);
        noecho();
    }

    // Основной цикл игры
    while (status == PLAY) {
        char control_button = getch();
        time_mili_sec = change_speed(control_button, &status, time_mili_sec);
        napms(time_mili_sec);
        clear();
        draw_generation(curr_generation);
        update_generation(curr_generation, next_generation);
        if (is_static(curr_generation, next_generation)) status = STOP;
        memcpy(curr_generation, next_generation, sizeof(int) * LINES * COLUMNS);
    }
    if (status != ERROR) endwin();
    return 0;
}

// Функция инициализации поколения
bool input_generation(int generation[LINES][COLUMNS]) {
    for (int i = 0; i < LINES; i++) {
        for (int j = 0; j < COLUMNS; j++) {
            int value;
            if (scanf("%d", &value) != 1 || (value != CELL_DEAD && value != CELL_ALIVE)) {
                return false;
            }
            generation[i][j] = value;
        }
    }
    return true;
}

// Функция создающая новое поколение на основе текущего
void update_generation(int curr_generation[LINES][COLUMNS], int next_generation[LINES][COLUMNS]) {
    for (int i = 0; i < LINES; i++)
        for (int j = 0; j < COLUMNS; j++)
            next_generation[i][j] =
                should_live(count_neighbors(curr_generation, i, j), curr_generation[i][j]);
}

// Функция подсчета соседей
int count_neighbors(int generation[LINES][COLUMNS], int i, int j) {
    // Количество соседей
    int sum = 0;
    // i и j для координат соседей
    int i_minus = i - 1, j_minus = j - 1;
    for (int k = 0; k < 3; k++)
        for (int m = 0; m < 3; m++)
            sum += generation[(k + i_minus + LINES) % LINES][(m + j_minus + COLUMNS) % COLUMNS];
    sum -= generation[i][j];

    return sum;
}

// Функция принятия решения о следующем поколении
bool should_live(int neighbors, bool is_alive) { return (neighbors == 3) || (is_alive && neighbors == 2); }

// Функция проверяющая статичность поколений
bool is_static(const int curr_generation[LINES][COLUMNS], const int next_generation[LINES][COLUMNS]) {
    for (int i = 0; i < LINES; i++)
        for (int j = 0; j < COLUMNS; j++)
            if (curr_generation[i][j] != next_generation[i][j]) return false;
    return true;
}

// Функция смены скорости
int change_speed(char control_button, Status *status, int time_mili_sec) {
    if (control_button == 'z' && time_mili_sec < MAX_DELAY)
        time_mili_sec += 100;
    else if (control_button == 'a' && time_mili_sec > MIN_DELAY)
        time_mili_sec -= 100;
    else if (control_button == ' ')
        *status = STOP;

    return time_mili_sec;
}

// Функция отрисовки
void draw_generation(const int generation[LINES][COLUMNS]) {
    for (int i = 0; i < LINES; i++) {
        for (int j = 0; j < COLUMNS; j++) {
            printw(generation[i][j] == CELL_ALIVE ? "0" : "-");
        }
        printw("\n");
    }
}
