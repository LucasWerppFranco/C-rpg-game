#define _XOPEN_SOURCE 700

#include <stdio.h>
#include <string.h>
#include <locale.h>
#include <wchar.h>
#include <wctype.h>
#include <stdlib.h>
#include <limits.h>

#ifdef _WIN32
    #define CLEAR "cls"
#else
    #define CLEAR "clear"
#endif

#define WIDTH 100
#define MAIN_HEIGHT 35
#define SECONDARY_HEIGHT 5

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
    int padding = WIDTH - 2 - content_width;
    printf("%s", text);
    for (int j = 0; j < padding; j++) {
        printf(" ");
    }
    printf("║\n");
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

void print_main_border() {
    for (int i = 0; i < MAIN_HEIGHT; i++) {
        print_line("");
    }
}

void print_secondary_border() {
    for (int i = 0; i < SECONDARY_HEIGHT; i++) {
        print_line("");
    }
}

void print_title() {
    system(CLEAR);
    print_border_top();
    const char *text[] = {
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

void print_story_1() {
    system(CLEAR);
    print_border_top();
    const char *text[] = {
      "     |    |                             \\  STORY  /                                              ",
      "     | 1  |                              \\_______/                                               ",
      "     |    |                                                                                       ",
      "     | /\\ |                                                                                      ",
      "     |/  \\|                                                                                      ",
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
      "                                                                                                  "
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

int main() {
  setlocale(LC_ALL, "");

  char response[10];

  print_title();

  print_box("Do you want to start the game? (y/n):");
  printf(": ");

  fgets(response, sizeof(response), stdin);

  if (response[0] == 'y' || response[0] == 'Y') {
      int pages = 0;
      char confirmation[0];

      print_story_1();

      print_box("Press (N) to go to the next page");
      printf(": "); 
  }
    
    return 0;
} 
