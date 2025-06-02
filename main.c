#define _XOPEN_SOURCE 700

#include <stdio.h>
#include <string.h>
#include <locale.h>
#include <wchar.h>
#include <wctype.h>
#include <stdlib.h>
#include <limits.h>
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

int current_page = 1;


// Functions


// Structure
int visual_width(const char *s);
void print_border_top();
void print_border_bottom();
void print_line(const char *text);
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
void capture_input(char* direction);


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
        fgets(response, sizeof(response), stdin);

        if (response[0] == 'q' || response[0] == 'Q') {
            reset_terminal_mode();
            return 0;
        } else if (response[0] == 'a' || response[0] == 'A') {
            if (current_page > 1) current_page--;
        } else if (response[0] == 'd' || response[0] == 'D') {
            if (current_page < 3) current_page++;
        } else if (response[0] == '\n' && current_page == 3) {
            break;
        }
    }

    MapConfig map1 = {
        .map_name = "Map 1",
        .intro_text = "Catacombs",
        .color_code = "yellow",
        .file_name = "map.txt"
    };

    load_map(&map1);

    char direction[10];
    do {
        show_map(&map1);
        capture_input(direction);

        if (should_exit(direction[0])) {
            clear_memory();
            reset_terminal_mode();
            return 0;
        }

        move_player(direction, &map1);

    } while (1);

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
        "                  https://github.com/LucasWerppFranco/C-rpg-game/tree/main                        "
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
      "      )))     ~----'   /      \\                                   )       )                      ",
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

    print_line(map_config->intro_text);

    for (int i = 0; i < lines; i++) {
        printf("║");  // borda esquerda

        for (int j = 0; j < columns; j++) {
            if (strcmp(map_config->color_code, "yellow") == 0 && strcmp(map[i][j].content, "@") == 0) {
                printf("\033[33m%s\033[0m", map[i][j].content);  // imprime com cor
            } else {
                printf("%s", map[i][j].content);
            }
        }

        int visual_length = columns;  
        for (int k = visual_length; k < WIDTH - 2; k++) {
            printf(" ");
        }

        printf("║\n");  
    }

    print_border_bottom();

    print_box("Comands:\n"
              "w/a/s/d or arrows to move\n"
              "Q to exit");
}

int should_exit(char command) {
    return command == 'q';
}

void move_player(char* direction, const MapConfig* map_config) {
    int x = -1, y = -1;

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
        dx = +1;
    } else if (strcmp(direction, "a") == 0 || strcmp(direction, "^[[D") == 0) {
        dy = -1;
    } else if (strcmp(direction, "d") == 0 || strcmp(direction, "^[[C") == 0) {
        dy = +1;
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
    } else if (strcmp(map[new_x][new_y].content, "#") == 0) {
        if (beyond_x >= 0 && beyond_x < lines && beyond_y >= 0 && beyond_y < columns) {
            if (strcmp(map[beyond_x][beyond_y].content, ".") == 0) {
                strcpy(map[beyond_x][beyond_y].content, "#");
                strcpy(map[new_x][new_y].content, "");
                strcpy(map[x][y].content, ".");
            }
        }
    }

    if (map[new_x][new_y].number > 0) {
        printf("Você pisou no número %d!\n", map[new_x][new_y].number);
        map[new_x][new_y].number = 0;  
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


