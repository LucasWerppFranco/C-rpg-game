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
}

int main() {
  setlocale(LC_ALL, "");

  char response[10];

  system(CLEAR);
  print_border_top();
  print_title();
  print_border_bottom();

  print_border_top();
  print_line("Do you want to start the game? (y/n): ");
  print_border_bottom();
  printf(": ");

  fgets(response, sizeof(response), stdin);

  if (response[0] == 'y' || response[0] == 'Y') {
    system(CLEAR);
    print_border_top();
    print_main_border();
    print_border_bottom();

    print_border_top();
    print_line("Press F to go to the next page");
    print_border_bottom();
    printf(": ");
  }
    
    return 0;
} 
