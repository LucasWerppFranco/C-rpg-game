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

#ifdef _WIN32
    #define CLEAR "cls"
#else
    #define CLEAR "clear"
#endif

#define WIDTH 100
#define MAIN_HEIGHT 40
#define SECONDARY_HEIGHT 4

int current_page = 1;

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


void print_story_1() {
    system(CLEAR);
    print_border_top();

    print_page_header("STORY");

    const char *text[] = {
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
        "                                              (teste)                                             "
    };

    size_t lines = sizeof(text) / sizeof(text[0]);
    for (size_t i = 0; i < MAIN_HEIGHT - 5; i++) { 
        if (i < lines) {
            print_line(text[i]);
        } else {
            print_line("");
        }
    }

    print_border_bottom();
}

void print_story_2() {
    system(CLEAR);
    print_border_top();

    print_page_header("STORY");

    const char *text[] = {
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

    size_t lines = sizeof(text) / sizeof(text[0]);
    for (size_t i = 0; i < MAIN_HEIGHT -5; i++) {
        if (i < lines) {
            print_line(text[i]);
        } else {
            print_line("");
        }
    }
    print_border_bottom();
}

void print_story_3() {
  system(CLEAR);
  print_border_top();
  print_page_header("STORY");

  const char *text[] = {
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

size_t lines = sizeof(text) / sizeof(text[0]);
    for (size_t i = 0; i < MAIN_HEIGHT -5; i++) {
        if (i < lines) {
            print_line(text[i]);
        } else {
            print_line("");
        }
    }
    print_border_bottom();
}

void print_story_1();
void print_story_2();
void print_story_3();
void print_title();

int main() {
  setlocale(LC_ALL, "");

  char response[10];
  set_conio_terminal_mode();
  print_title();

  print_box("press ENTER to start the game");
  fgets(response, sizeof(response), stdin);

  if (response[0] == '\n') {
    current_page = 1;
    char input[10];

    struct timespec ts;
    ts.tv_sec = 0;
    ts.tv_nsec = 100000 * 1000;

    while (1) {
        if (kbhit()) {
            char ch = getchar();
            switch (current_page) {
                case 1:
                    print_story_1();
                    break;
                case 2:
                    print_story_2();
                    break;
                case 3:
                    print_story_3();
                    break;
                default:
                    print_box("End of story. Press (A) to go back or (Q) to quit.");
                    break;
            }

            print_box("Press (D) for next, (A) to return, or (S) to skip:");

            if (ch == 'q' || ch == 'Q') {
                break; 
            } else if ((ch == 'd' || ch == 'D') && current_page < 3) {
                current_page++; 
            } else if ((ch == 'a' || ch == 'A') && current_page > 1) {
                current_page--; 
            } else if (ch == 's' || ch == 'S') {
                current_page = 1;
                break; 
            }
        }
        nanosleep(&ts, NULL); 
    }
}

reset_terminal_mode(); 
return 0;
}
