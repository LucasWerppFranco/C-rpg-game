#define _XOPEN_SOURCE 700

#include <stdio.h>
#include <string.h>
#include <locale.h>
#include <wchar.h>
#include <wctype.h>
#include <stdlib.h>
#include <limits.h>

#define WIDTH 80

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

int main() {
    setlocale(LC_ALL, "");

    printf("╔");
    for (int i = 0; i < WIDTH - 2; i++) {
        printf("═");
    }
    printf("╗\n");

    const char *text[] = {
        "     ▓█████▄  █    ██  ███▄    █   ▄████ ▓█████  ▒█████   ███▄    █         ",
        "     ▒██▀ ██▌ ██  ▓██▒ ██ ▀█   █  ██▒ ▀█▒▓█   ▀ ▒██▒  ██▒ ██ ▀█   █         ",
        "     ░██   █▌▓██  ▒██░▓██  ▀█ ██▒▒██░▄▄▄░▒███   ▒██░  ██▒▓██  ▀█ ██▒        ",
        "     ░▓█▄   ▌▓▓█  ░██░▓██▒  ▐▌██▒░▓█  ██▓▒▓█  ▄ ▒██   ██░▓██▒  ▐▌██▒        ",
        "     ░▒████▓ ▒▒█████▓ ▒██░   ▓██░░▒▓███▀▒░▒████▒░ ████▓▒░▒██░   ▓██░        ",
        "      ▒▒▓  ▒ ░▒▓▒ ▒ ▒ ░ ▒░   ▒ ▒  ░▒   ▒ ░░ ▒░ ░░ ▒░▒░▒░ ░ ▒░   ▒ ▒         ",
        "      ░ ▒  ▒ ░░▒░ ░ ░ ░ ░░   ░ ▒░  ░   ░  ░ ░  ░  ░ ▒ ▒░ ░ ░░   ░ ▒░        ",
        "      ░ ░  ░  ░░░ ░ ░    ░   ░ ░ ░ ░   ░    ░   ░ ░ ░ ▒     ░   ░ ░         ",
        "        ░       ░              ░       ░    ░  ░    ░ ░           ░         ",
        "     ░                                                                      ",
        " ▒█████    █████▒   ▄▄██████▓ ██░ ██ ▓█████     ███▄ ▄███▓ ▄▄▄      ▓█████▄ ",
        "▒██▒  ██▒▓██   ▒    ▓  ██▒ ▓▒▓██░ ██▒▓█   ▀    ▓██▒▀█▀ ██▒▒████▄    ▒██▀ ██▌",
        "▒██░  ██▒▒████ ░    ▒ ▓██░ ▒░▒██▀▀██░▒███      ▓██    ▓██░▒██  ▀█▄  ░██   █▌",
        "▒██   ██░░▓█▒  ░    ░ ▓██▓ ░ ░▓█ ░██ ▒▓█  ▄    ▒██    ▒██ ░██▄▄▄▄██ ░▓█▄   ▌",
        "░ ████▓▒░░▒█░         ▒██▒ ░ ░▓█▒░██▓░▒████▒   ▒██▒   ░██▒ ▓█   ▓██▒░▒████▓ ",
        "░ ▒░▒░▒░  ▒ ░         ▒ ░░    ▒ ░░▒░▒░░ ▒░ ░   ░ ▒░   ░  ░ ▒▒   ▓▒█░ ▒▒▓  ▒ ",
        "  ░ ▒ ▒░  ░             ░     ▒ ░▒░ ░ ░ ░  ░   ░  ░      ░  ▒   ▒▒ ░ ░ ▒  ▒ ",
        "░ ░ ░ ▒   ░ ░         ░       ░  ░░ ░   ░      ░      ░     ░   ▒    ░ ░  ░ ",
        "    ░ ░                       ░  ░  ░   ░  ░          ░         ░  ░   ░    ",
        "                                                                     ░      ",
        "                ███▄ ▄███▓ ▄▄▄        ▄████ ▓█████                          ",
        "               ▓██▒▀█▀ ██▒▒████▄     ██▒ ▀█▒▓█   ▀                          ",
        "               ▓██    ▓██░▒██  ▀█▄  ▒██░▄▄▄░▒███                            ",
        "               ▒██    ▒██ ░██▄▄▄▄██ ░▓█  ██▓▒▓█  ▄                          ",
        "               ▒██▒   ░██▒ ▓█   ▓██▒░▒▓███▀▒░▒████▒                         ",
        "               ░ ▒░   ░  ░ ▒▒   ▓▒█░ ░▒   ▒ ░░ ▒░ ░                         ",
        "               ░  ░      ░  ▒   ▒▒ ░  ░   ░  ░ ░  ░                         ",
        "               ░      ░     ░   ▒   ░ ░   ░    ░                            ",
        "                      ░         ░  ░      ░    ░  ░                         "
    };

    size_t lines = sizeof(text) / sizeof(text[0]);

    for (size_t i = 0; i < lines; i++) {
        printf("║");
        int content_width = visual_width(text[i]);
        int padding = WIDTH - 2 - content_width;
        printf("%s", text[i]);
        for (int j = 0; j < padding; j++) {
            printf(" ");
        }
        printf("║\n");
    }

    printf("╚");
    for (int i = 0; i < WIDTH - 2; i++) {
        printf("═");
    }
    printf("╝\n");

    return 0;
}

