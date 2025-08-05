#define _XOPEN_SOURCE 700

#include <stdio.h>
#include <string.h>
#include <locale.h>
#include <wchar.h>
#include <wctype.h>
#include <stdlib.h>
#include <limits.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <tusk.h>
#include "data.h"

#ifdef _WIN32
    #define CLEAR "cls"
#else
    #define CLEAR "clear"
#endif

#define WIDTH 100
#define MAIN_HEIGHT 40
#define SECONDARY_HEIGHT 4

typedef struct {
    char content[5];
    int number;
} Cell;

Cell** map;
int lines, columns;

typedef struct {
    char* map_name;
    char* intro_text;
    char* color_code;
    const char* file_name;
} MapConfig;

pthread_t enemy_thread;

volatile int redraw_needed = 0;

int game_running = 1;
int current_page = 1;


// Functions


// Structure
int visual_width(const char *s);
void print_border_top();
void print_border_bottom();
void print_line(const char *text);
void print_line_center(const char *text);
void print_line_right(const char *text);
void print_instructions(int current_page);
void print_box(const char *text);
void print_title();
void print_page_header(const char *title);
void print_story(int n);
// Map
void lock_map();
void clear_memory();
void load_map(const MapConfig* map_config);
void show_map(const MapConfig* map_config);
int should_exit(char command);
void move_player(char* direction, const MapConfig* map_config);
void* enemy_ai_thread(void* arg);
void move_all_enemies();
void capture_input(char* direction);

// Pathfinding - new
int find_next_step(int start_x, int start_y, int target_x, int target_y, int* next_x, int* next_y);

// Main Functions


int main() {
    setlocale(LC_ALL, "");
    setbuf(stdout, NULL);

    char response[10];
    set_conio_terminal_mode();
    print_title();

    print_box("Press ENTER to start");
    fgets(response, sizeof(response), stdin);

    current_page = 1;

    while (1) {
        print_story(current_page);
        print_instructions(current_page);
        int ch = getchar();

        if (ch == 'q' || ch == 'Q') {
            reset_terminal_mode();
            return 0;
        } else if (ch == 'a' || ch == 'A') {
            if (current_page > 1) current_page--;
        } else if (ch == 'd' || ch == 'D') {
            if (current_page < 3) current_page++;
        } else if ((ch == '\n' || ch == '\r') && current_page == 3) {
            break;
        }
    }

    MapConfig map1 = {
        .map_name = "Map 1",
        .intro_text = "Abaddon's Catacombs",
        .color_code = "yellow",
        .file_name = "map.txt"
    };

    load_map(&map1);

    pthread_create(&enemy_thread, NULL, enemy_ai_thread, NULL);

    char direction[10];

    redraw_needed = 1;

    do {
        if (redraw_needed) {
            show_map(&map1);
            redraw_needed = 0;
        }

        capture_input(direction);

        if (should_exit(direction[0])) {
            game_running = 0;
            pthread_join(enemy_thread, NULL);
            clear_memory();
            reset_terminal_mode();
            return 0;
        }

        move_player(direction, &map1);

    } while (1);

    game_running = 0;
    pthread_join(enemy_thread, NULL);

    clear_memory();
    reset_terminal_mode();
    return 0;
}


// Structure


int visual_width(const char *s) {
    int width = 0;
    wchar_t wc;
    mbstate_t state;
    memset(&state, 0, sizeof state);
    const char *p = s;

    while (*p) {
        size_t len = mbrtowc(&wc, p, MB_CUR_MAX, &state);
        if (len == (size_t)-1 || len == (size_t)-2) break;
        int w = wcwidth(wc);
        if (w > 0) width += w;
        p += len;
    }

    return width;
}

void print_border_top() {
    printf("╔");

    for (int i = 0; i < WIDTH - 2; i++) {
        printf("═");
    }

    printf("╗\n");
}

void print_border_bottom() {
    printf("╚");

    for (int i = 0; i < WIDTH - 2; i++) {
        printf("═");
    }

    printf("╝\n");
}

void print_line(const char *text) {
    printf("║");

    int content_width = visual_width(text);  
    printf("%s", text);

    int padding = WIDTH - 2 - content_width;
    for (int j = 0; j < padding; j++) {
        printf(" ");
    }

    printf("║\n");
}

void print_line_center(const char *text) {
    printf("║");

    int content_width = visual_width(text);
    int total_padding = WIDTH - 2 - content_width;
    int left_padding = total_padding / 2;
    int right_padding = total_padding - left_padding;

    for (int j = 0; j < left_padding; j++) {
        printf(" ");
    }

    printf("%s", text);

    for (int j = 0; j < right_padding; j++) {
        printf(" ");
    }

    printf("║\n");
}

void print_line_right(const char *text) {
    printf("║");

    int content_width = visual_width(text);
    int left_padding = WIDTH - 2 - content_width;

    for (int j = 0; j < left_padding; j++) {
        printf(" ");
    }

    printf("%s", text);

    printf("║\n");
}

void print_instructions(int current_page) {
    char instrucao[100];

    if (current_page == 3) {
        snprintf(instrucao, sizeof(instrucao),
                 "Press ENTER to continue the game\nOr 'A' to return, 'Q' to exit");
    } else {
        snprintf(instrucao, sizeof(instrucao),
                 "Press 'D' to advance\n'A' to return\n'Q' to exit");
    }

    print_box(instrucao);
}

void print_box(const char *text) {
    print_border_top();

    char *copy = strdup(text);  
    char *line = strtok(copy, "\n");

    int lines_printed = 0;
    while (line != NULL && lines_printed < SECONDARY_HEIGHT) {
        print_line(line);
        line = strtok(NULL, "\n");
        lines_printed++;
    }

    for (; lines_printed < SECONDARY_HEIGHT; lines_printed++) {
        print_line("");
    }

    print_border_bottom();
    free(copy); 
}

void print_title() {
    system(CLEAR);
    print_border_top();
    const char *text[] = {
        "                                                                                                  ",
        "                                                                                                  ",
        "                                                                                                  ",
        "                                                                                                  ",
        "                ▓█████▄  █    ██  ███▄    █   ▄████ ▓█████  ▒█████   ███▄    █                    ",
        "                ▒██▀ ██▌ ██  ▓██▒ ██ ▀█   █  ██▒ ▀█▒▓█   ▀ ▒██▒  ██▒ ██ ▀█   █                    ",
        "                ░██   █▌▓██  ▒██░▓██  ▀█ ██▒▒██░▄▄▄░▒███   ▒██░  ██▒▓██  ▀█ ██▒                   ",
        "                ░▓█▄   ▌▓▓█  ░██░▓██▒  ▐▌██▒░▓█  ██▓▒▓█  ▄ ▒██   ██░▓██▒  ▐▌██▒                   ",
        "                ░▒████▓ ▒▒█████▓ ▒██░   ▓██░░▒▓███▀▒░▒████▒░ ████▓▒░▒██░   ▓██░                   ",
        "                 ▒▒▓  ▒ ░▒▓▒ ▒ ▒ ░ ▒░   ▒ ▒  ░▒   ▒ ░░ ▒░ ░░ ▒░▒░▒░ ░ ▒░   ▒ ▒                    ",
        "                 ░ ▒  ▒ ░░▒░ ░ ░ ░ ░░   ░ ▒░  ░   ░  ░ ░  ░  ░ ▒ ▒░ ░ ░░   ░ ▒░                   ",
        "                 ░ ░  ░  ░░░ ░ ░    ░   ░ ░ ░ ░   ░    ░   ░ ░ ░ ▒     ░   ░ ░                    ",
        "                   ░       ░              ░       ░    ░  ░    ░ ░           ░                    ",
        "                ░                                                                                 ",
        "            ▒█████    █████▒   ▄▄██████▓ ██░ ██ ▓█████     ███▄ ▄███▓ ▄▄▄      ▓█████▄            ",
        "           ▒██▒  ██▒▓██   ▒    ▓  ██▒ ▓▒▓██░ ██▒▓█   ▀    ▓██▒▀█▀ ██▒▒████▄    ▒██▀ ██▌           ",
        "           ▒██░  ██▒▒████ ░    ▒ ▓██░ ▒░▒██▀▀██░▒███      ▓██    ▓██░▒██  ▀█▄  ░██   █▌           ",
        "           ▒██   ██░░▓█▒  ░    ░ ▓██▓ ░ ░▓█ ░██ ▒▓█  ▄    ▒██    ▒██ ░██▄▄▄▄██ ░▓█▄   ▌           ",
        "           ░ ████▓▒░░▒█░         ▒██▒ ░ ░▓█▒░██▓░▒████▒   ▒██▒   ░██▒ ▓█   ▓██▒░▒████▓            ",
        "           ░ ▒░▒░▒░  ▒ ░         ▒ ░░    ▒ ░░▒░▒░░ ▒░ ░   ░ ▒░   ░  ░ ▒▒   ▓▒█░ ▒▒▓  ▒            ",
        "             ░ ▒ ▒░  ░             ░     ▒ ░▒░ ░ ░ ░  ░   ░  ░      ░  ▒   ▒▒ ░ ░ ▒  ▒            ",
        "           ░ ░ ░ ▒   ░ ░         ░       ░  ░░ ░   ░      ░      ░     ░   ▒    ░ ░  ░            ",
        "               ░ ░                       ░  ░  ░   ░  ░          ░         ░  ░   ░               ",
        "                                                                                ░                 ",
        "                           ███▄ ▄███▓ ▄▄▄        ▄████ ▓█████                                     ",
        "                          ▓██▒▀█▀ ██▒▒████▄     ██▒ ▀█▒▓█   ▀                                     ",
        "                          ▓██    ▓██░▒██  ▀█▄  ▒██░▄▄▄░▒███                                       ",
        "                          ▒██    ▒██ ░██▄▄▄▄██ ░▓█  ██▓▒▓█  ▄                                     ",
        "                          ▒██▒   ░██▒ ▓█   ▓██▒░▒▓███▀▒░▒████▒                                    ",
        "                          ░ ▒░   ░  ░ ▒▒   ▓▒█░ ░▒   ▒ ░░ ▒░ ░                                    ",
        "                          ░  ░      ░  ▒   ▒▒ ░  ░   ░  ░ ░  ░                                    ",
        "                          ░      ░     ░   ▒   ░ ░   ░    ░                                       ",
        "                                 ░         ░  ░      ░    ░  ░                                    ",
        "                                                                                                  ",
        "                                                                                                  ",
        "                                                                                                  ",
        "                                                                                                  ",
        "   From -> Lucas Werpp Franco                                      Thanks for playing my game :)  ",
        "                                                                                                  ",
        "                       https://github.com/LucasWerppFranco/C-rpg-game                             "
    };

    size_t lines = sizeof(text) / sizeof(text[0]);
    for (size_t i = 0; i < MAIN_HEIGHT; i++) {
        if (i < lines) {
            print_line(text[i]);
        } else {
            print_line("");
        }
    }
    print_border_bottom();
}

void print_page_header(const char *title) {
    char buffer[WIDTH];

    snprintf(buffer, sizeof(buffer), "     |    |                             \\  %s  /                                              ", title);
    print_line(buffer);

    snprintf(buffer, sizeof(buffer), "     | %d  |                              \\_______/                                               ", current_page);
    print_line(buffer);

    print_line("     |    |                                                                                       ");
    print_line("     | /\\ |                                                                                      ");
    print_line("     |/  \\|                                                                                      ");
}

void print_story(int n) {
    system(CLEAR);
    print_border_top();

    print_page_header("STORY");

    const char **text = NULL;
    size_t lines = 0;

    static const char *story1[] = {
        "                                                                                                  ",
        "                                                                                                  ",
        "                                                                                                  ",
        "                                                                                                  ",
        "                                                                                                  ",
        "                                                         _______________________                  ",
        "              ________________________-------------------                       `\\               ",
        "             /:--__                                                              |                ",
        "            ||< > |                                   ___________________________/                ",
        "            | \\__/_________________-------------------                         |                 ",
        "            |                                                                  |                  ",
        "             |                       THE LORD OF THE RINGS                      |                 ",
        "             |                                                                  |                 ",
        "             |       Three Rings for the Elven-kings under the sky,             |                 ",
        "              |        Seven for the Dwarf-lords in their halls of stone,        |                ",
        "              |      Nine for Mortal Men doomed to die,                          |                ",
        "              |        One for the Dark Lord on his dark throne                  |                ",
        "              |      In the Land of Mordor where the Shadows lie.                 |               ",
        "               |       One Ring to rule them all, One Ring to find them,          |               ",
        "               |       One Ring to bring them all and in the darkness bind them   |               ",
        "               |     In the Land of Mordor where the Shadows lie.                |                ",
        "              |                                              ____________________|_               ",
        "              |  ___________________-------------------------                      `\\            ",
        "              |/`--_                                                                 |            ",
        "              ||[ ]||                                            ___________________/             ",
        "               \\===/___________________--------------------------                                ",
        "                                                                                                  ",
        "                                                                                                  ",
        "                                                                                                  ",
        "                                                                                                  ",
        "                                                                                                  ",
        "                                                                                                  ",
        "                                                                                                  ",
        "                                                                                                  ",
        "                                                                                                  "
    };

    static const char *story2[] = {
      "                                                                                                  ",
      "                                           /|                                                     ",
      "                                          |||                                                     ",
      "                                          |||                                                     ",
      "                                          |||                                                     ",
      "                                          |||                                                     ",
      "                                          |||                                                     ",
      "                                          |||                                                     ",
      "                                          |||                                                     ",
      "                                       ~-[{o}]-~                                                  ",
      "                                          |/|                                                     ",
      "                                          |/|                                                     ",
      "                  ///~`     |\\\\\\\\_        `0'         =\\\\\\\\         . .                   ",
      "                 ,  |='  ,))\\_| ~-_                    _)  \\      _/_/|                         ",
      "                / ,' ,;((((((    ~ \\                  `~~~\\-~-_ /~ (_/\\                        ",
      "              /' -~/~)))))))'\\_   _/'                      \\_  /'  D   |                        ",
      "             (       (((((( ~-/ ~-/                          ~-;  /    \\--_                      ",
      "              ~~--|   ))''    ')  `                            `~~\\_    \\   )                   ",
      "                  :        (_  ~\\           ,                    /~~-     ./                     ",
      "                   \\        \\_   )--__  /(_/)                   |    )    )|                    ",
      "         ___       |_     \\__/~-__    ~~   ,'      /,_;,   __--(   _/      |                     ",
      "       //~~\\`\\    /' ~~~----|     ~~~~~~~~'        \\-  ((~~    __-~        |                   ",
      "     ((()   `\\`\\_(_     _-~~-\\                      ``~~ ~~~~~~   \\_      /                   ",
      "     )))     ~----'   /      \\                                   )       )                      ",
      "       (         ;`~--'        :                                _-    ,;;(                        ",
      "                 |    `\\       |                             _-~    ,;;;;)                       ",
      "                 |    /'`\\     ;                          _-~          _/                        ",
      "                /~   /    |    )                         /;;;''  ,;;:-~                           ",
      "               |    /     / | /                         |;;'   ,''                                ",
      "               /   /     |  \\\\|                         |   ,;(                                 ",
      "             _/  /'       \\  \\_)                   .---__\\_    \\,--._______                   ",
      "            ( )|'         (~-_|                   (;;'  ;;;~~~/' `;;|  `;;;\\                     ",
      "             ) `\\_         |-_;;--__               ~~~----__/'    /'_______/                     ",
      "             `----'       (   `~--_ ~~~;;------------~~~~~ ;;;'_/'                                ",
      "                     `~~~~~~~~'~~~-----....___;;;____---~~                                        "
    };

    static const char *story3[] = {
    "                                                                                                  ",
    "                    / \\                                                                          ",
    "                   / | \\  Three Rings for the Elvin-Kings under the sky.                         ",
    "                  /  |  \\ Seven for the DwarfLords in their halls of stone.                      ",
    "                 |   |   |    Nine for the Mortal Men doomed to die.                              ",
    "                 |   |   |    One for the Dark Lord on his dark throne.                           ",
    "                 |   |   |In the Land of Mordor where the Shadow Lies.                            ",
    "                 |   |   |                                                                        ",
    "                 |   |   |  One Ring to rule them all,One Ring to find them,                      ",
    "                 |   |   |One Ring to bring them all and in the Darkness                          ",
    "                 |   |   |   Bind Them                                                            ",
    "                 |   |   |  In the Land of Mordor where the Shadows Lie.                          ",
    "                 |   |   |                                                                        ",
    "                 |   |   |                                                                        ",
    "                 |   |   |           This quote is from my Favorite Book                          ",
    "                 |   |   |                       - Lord Of THe Rings -                            ",
    "                 |   |   |                              by J.R.R. Tolkien                         ",
    "                 |   |   |                                                                        ",
    "                 |   |   |                                                                        ",
    "                 |   |   |                                                                        ",
    "                 |   |   |                                                                        ",
    "                 |   |   |                      .____---^^     ^^---____.                         ",
    "                 |   |   |                      TI      *       *      IT                         ",
    "                 |   |   |                      !I          *          I!                         ",
    "                 |  / \\  |                       X                     X                         ",
    "/\\               |/     \\|               /\\      XL         ?         JX                       ",
    "\\ \\_____________/         \\_____________/ /      II    ?   / \\   ?    II                      ",
    " \\______________\\         /______________/       II   / \\ /   \\ / \\   II                     ",
    "                 \\       /                        X  /   v     v   \\  X                         ",
    "                 |\\\\   //|                        ``/    _     _    \\''                        ",
    "                 |//\\ ///|                         \\\\- _-_ -_- _-_ -//                         ",
    "                 |///////|                           \\\\_-  -_-  -_//                            ",
    "                 |///////|                             ``       ''                                ",
    "                 |///////|                               ``-_-''                                  ",
    "                 |///////|                                                                        ",
    "                 |///////|                                                                        ",
    "                 |///////|                                                                        ",
    "                / \\/\\_/\\/ \\                                                                   ",
    "               |\\_/\\/ \\/\\_/|                                                                  ",
    "               |/ \\/\\ /\\/ \\|                                                                  ",
    "                \\_/\\/_\\/\\_/                                                                   ",
    "                  \\_/_\\_/                                                                       "
    };

    switch (n) {
        case 1:
            text = story1;
            lines = sizeof(story1) / sizeof(story1[0]);
            break;
        case 2:
            text = story2;
            lines = sizeof(story2) / sizeof(story2[0]);
            break;
        case 3:
            text = story3;
            lines = sizeof(story3) / sizeof(story3[0]);
            break;
        default:
            print_line("Non-existent page");
            print_border_bottom();
            return;
    }

    for (size_t i = 0; i < MAIN_HEIGHT - 5; i++) {
        if (i < lines) {
            print_line(text[i]);
        } else {
            print_line("");
        }
    }

    print_border_bottom();
}



// MAP


void lock_map() {
    map = malloc(sizeof(Cell*) * lines);
    if (map == NULL) {
        printf("Error allocating map.\n");
        exit(1);
    }

    for (int i = 0; i < lines; i++) {
        map[i] = malloc(sizeof(Cell) * columns);
        if (map[i] == NULL) {
            printf("Error allocating row %d.\n", i);
            exit(1);
        }
    }
}

void clear_memory() {
    for (int i = 0; i < lines; i++) {
        free(map[i]);
    }
    free(map);
}

void load_map(const MapConfig* map_config) {
    FILE* f = fopen(map_config->file_name, "r");
    if (!f) {
        printf("Error: 1 - '%s' not found\n", map_config->file_name);
        exit(1);
    }

    fscanf(f, "%d %d\n", &lines, &columns);
    lock_map();

    for (int i = 0; i < lines; i++) {
        for (int j = 0; j < columns; j++) {
            unsigned char buffer[5] = {0};
            int c = fgetc(f);

            if (c == EOF) break;

            int size = 1;
            buffer[0] = c;
            if ((c & 0xE0) == 0xC0) size = 2;
            else if ((c & 0xF0) == 0xE0) size = 3;
            else if ((c & 0xF8) == 0xF0) size = 4;

            for (int k = 1; k < size; k++) {
                buffer[k] = fgetc(f);
            }

            if (buffer[0] >= '1' && buffer[0] <= '9') {
                map[i][j].content[0] = '.';  
                map[i][j].number = buffer[0] - '0'; 
            } else {
                strncpy(map[i][j].content, (char*)buffer, 5);
                map[i][j].number = 0; 
            }
        }
        fgetc(f); 
    }

    fclose(f);
}

void show_map(const MapConfig* map_config) {
    system(CLEAR);
    print_border_top();

    print_line_center(map_config->intro_text);  

    for (int i = 0; i < lines; i++) {
        char buffer[WIDTH * 2]; 
        int k = 0;

        for (int j = 0; j < columns; j++) {
            if (strcmp(map_config->color_code, "yellow") == 0 && strcmp(map[i][j].content, "@") == 0) {
                k += snprintf(buffer + k, sizeof(buffer) - k, "\033[33m%s\033[0m", map[i][j].content);
            } else {
                k += snprintf(buffer + k, sizeof(buffer) - k, "%s", map[i][j].content);
            }
        }

        buffer[k] = '\0';

        print_line_center(buffer);  
    }

    print_border_bottom();

    print_box("Comandos:\n"
              "w/a/s/d ou setas para mover\n"
              "q para sair");
}

int should_exit(char command) {
    return command == 'q';
}

void move_player(char* direction, const MapConfig* map_config) {
    int x = -1, y = -1;

    // encontra jogador
    for (int i = 0; i < lines && x == -1; i++) {
        for (int j = 0; j < columns; j++) {
            if (strcmp(map[i][j].content, "") == 0) {
                x = i;
                y = j;
                break;
            }
        }
    }

    int dx = 0, dy = 0;

    if (strcmp(direction, "w") == 0 || strcmp(direction, "^[[A") == 0) {
        dx = -1;
    } else if (strcmp(direction, "s") == 0 || strcmp(direction, "^[[B") == 0) {
        dx = 1;
    } else if (strcmp(direction, "a") == 0 || strcmp(direction, "^[[D") == 0) {
        dy = -1;
    } else if (strcmp(direction, "d") == 0 || strcmp(direction, "^[[C") == 0) {
        dy = 1;
    } else {
        return;
    }

    int new_x = x + dx;
    int new_y = y + dy;
    int beyond_x = x + 2 * dx;
    int beyond_y = y + 2 * dy;

    if (new_x < 0 || new_x >= lines || new_y < 0 || new_y >= columns) return;

    if (strcmp(map[new_x][new_y].content, ".") == 0) {
        strcpy(map[x][y].content, ".");
        strcpy(map[new_x][new_y].content, "");
        redraw_needed = 1;  
    } else if (strcmp(map[new_x][new_y].content, "#") == 0) {
        if (beyond_x >= 0 && beyond_x < lines && beyond_y >= 0 && beyond_y < columns) {
            if (strcmp(map[beyond_x][beyond_y].content, ".") == 0) {
                strcpy(map[beyond_x][beyond_y].content, "#");
                strcpy(map[new_x][new_y].content, "");
                strcpy(map[x][y].content, ".");
                redraw_needed = 1;  
            }
        }
    }
}


void* enemy_ai_thread(void* arg) {
    while (game_running) {
        move_all_enemies();
        usleep(500000);
    }
    return NULL;
}

// Breadth-First Search pathfinding para encontrar próximo passo do inimigo até o jogador
int find_next_step(int start_x, int start_y, int target_x, int target_y, int* next_x, int* next_y) {
    if (start_x == target_x && start_y == target_y) {
        *next_x = start_x;
        *next_y = start_y;
        return 1;
    }

    // Estruturas para BFS
    typedef struct {
        int x, y;
    } Point;

    Point queue[lines * columns];
    int front = 0, back = 0;

    // Matriz para controlar visitados e anterior para reconstruir caminho
    int (*visited)[columns] = malloc(sizeof(int[lines][columns]));
    Point (*prev)[columns] = malloc(sizeof(Point[lines][columns]));

    if (!visited || !prev) {
        if (visited) free(visited);
        if (prev) free(prev);
        return 0; // falha na alocação
    }

    for (int i = 0; i < lines; i++) {
        for (int j = 0; j < columns; j++) {
            visited[i][j] = 0;
            prev[i][j].x = -1;
            prev[i][j].y = -1;
        }
    }

    queue[back++] = (Point){start_x, start_y};
    visited[start_x][start_y] = 1;

    int dirs[4][2] = {{1,0},{-1,0},{0,1},{0,-1}};

    int found = 0;

    while (front < back && !found) {
        Point p = queue[front++];

        for (int d = 0; d < 4; d++) {
            int nx = p.x + dirs[d][0];
            int ny = p.y + dirs[d][1];

            if (nx >= 0 && nx < lines && ny >= 0 && ny < columns) {
                if (!visited[nx][ny]) {
                    // Condição para passar pelas células: só avança se são "livres"
                    // Considerar "." e símbolos especiais que podem ser atravessáveis
                    if (strcmp(map[nx][ny].content, ".") == 0 ||
                        strcmp(map[nx][ny].content, "") == 0) { // inclui jogador
                        visited[nx][ny] = 1;
                        prev[nx][ny] = p;
                        if (nx == target_x && ny == target_y) {
                            found = 1;
                            break;
                        }
                        queue[back++] = (Point){nx, ny};
                    }
                }
            }
        }
    }

    if (!found) {
        free(visited);
        free(prev);
        return 0; // sem caminho encontrado
    }

    // Reconstruir caminho do destino até início para encontrar o próximo passo
    Point step = (Point){target_x, target_y};
    Point before_step;

    while (1) {
        Point par = prev[step.x][step.y];
        if (par.x == start_x && par.y == start_y) {
            before_step = step;
            break;
        }
        step = par;
    }

    *next_x = before_step.x;
    *next_y = before_step.y;

    free(visited);
    free(prev);
    return 1;
}

void move_all_enemies() {
    int player_x = -1, player_y = -1;

    // Localiza jogador no mapa
    for (int i = 0; i < lines; i++) {
        for (int j = 0; j < columns; j++) {
            if (strcmp(map[i][j].content, "") == 0) {
                player_x = i;
                player_y = j;
                break;
            }
        }
        if (player_x != -1) break;
    }

    if (player_x == -1) return;

    typedef struct {
        int x, y;
    } EnemyPos;

    EnemyPos enemies[lines * columns];
    int enemy_count = 0;

    // Localiza inimigos no mapa
    for (int i = 0; i < lines; i++) {
        for (int j = 0; j < columns; j++) {
            if (strcmp(map[i][j].content, "󰚌") == 0) {
                enemies[enemy_count].x = i;
                enemies[enemy_count].y = j;
                enemy_count++;
            }
        }
    }

    for (int e = 0; e < enemy_count; e++) {
        int enemy_x = enemies[e].x;
        int enemy_y = enemies[e].y;

        int next_x, next_y;

        // Usa pathfinding para achar próximo passo em direção ao jogador
        int path_found = find_next_step(enemy_x, enemy_y, player_x, player_y, &next_x, &next_y);

        if (path_found) {
            if (next_x != enemy_x || next_y != enemy_y) {
                // Verifica se a posição de próximo passo está livre (deve estar, mas por segurança)
                if (strcmp(map[next_x][next_y].content, ".") == 0 ||
                    strcmp(map[next_x][next_y].content, "") == 0) {
                    strcpy(map[enemy_x][enemy_y].content, ".");
                    strcpy(map[next_x][next_y].content, "󰚌");
                    redraw_needed = 1;
                }
            }
        }
        // Se não encontrou caminho, inimigo não se move
    }
}

void capture_input(char* direction) {
    int c = fgetc(stdin);

    if (c == '\033') {
        fgetc(stdin);
        c = fgetc(stdin);

        switch(c) {
            case 'A': strcpy(direction, "^[[A"); break;
            case 'B': strcpy(direction, "^[[B"); break;
            case 'C': strcpy(direction, "^[[C"); break;
            case 'D': strcpy(direction, "^[[D"); break;
        }
    } else {
        direction[0] = c;
        direction[1] = '\0';
    }
}


// COMBAT SYSTEM




