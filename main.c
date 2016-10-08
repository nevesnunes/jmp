#define _XOPEN_SOURCE 500

#include <locale.h>
#include <ncurses.h>

#include "all.h"
#include "hints.h"
#include "options.h"

#define MAX_BUFFER_SIZE 1024
#define MAX_SMALL_BUFFER_SIZE 128

typedef struct window_t {
    // hint text matches user input?
    bool matched;

    // assigned hint text
    char *hint;

    // xproperties
    char *class;
    char *title;
    char *id;
} window_t;

window_t* new_window() {
    window_t *window = (window_t*) malloc(sizeof(window_t));

    window->matched = false;
    window->hint = NULL; // Allocated by make_hint()
    window->class = (char*) malloc(sizeof(char) * MAX_BUFFER_SIZE);
    window->title = (char*) malloc(sizeof(char) * MAX_BUFFER_SIZE);
    window->id = (char*) malloc(sizeof(char) * MAX_SMALL_BUFFER_SIZE);

    return window;
}

int compare_windows(const void *a, const void *b) { 
    window_t *ia = *(window_t **)a;
    window_t *ib = *(window_t **)b;

    return strcmp(ia->class, ib->class);
} 

window_t** make_windows(const char *hints) {
    window_t **windows = (window_t**) calloc(MAX_BUFFER_SIZE, sizeof(*windows));

    FILE *fp;
    char command[MAX_BUFFER_SIZE];
    sprintf(command, "wmctrl -lx");
    fp = popen(command, "r");
        error_exit(!fp, command, strerror(errno));

        // Parse window properties
        size_t count_windows = 0;
        char output[MAX_BUFFER_SIZE];
        char *err;
        char *id, *class, *class1, *class2, *client_name, *title;
        while (fgets(output, sizeof(output)-1, fp)) {
            id = strtok(output, " ");
            strtok(NULL, " "); // Skip desktop number
            class = strtok(NULL, " ");
            client_name = strtok(NULL, " ");
            title = strtok(NULL, "\n");

            // Check if id is an hexadecimal number
            if (!id)
                continue;
            errno = 0;
            strtoul(id, &err, 16);
            if (*err || errno == EINVAL || errno == ERANGE)
                continue;

            // Should skip panels and other such windows
            if (!client_name || (strcmp(client_name, "N/A") == 0))
                continue;

            // Simplify class name
            if (!class)
                continue;
            class1 = strtok(class, ".");
            class2 = strtok(NULL, ".");
            class = (!class2) ? class1 : class2;

            windows[count_windows] = new_window();
            strncpy(windows[count_windows]->id, id,
                    sizeof(char) * MAX_SMALL_BUFFER_SIZE);
            strncpy(windows[count_windows]->class, class,
                    sizeof(char) * MAX_BUFFER_SIZE);
            strncpy(windows[count_windows]->title, title,
                    sizeof(char) * MAX_BUFFER_SIZE);
            windows[count_windows]->hint = make_hint(hints, count_windows);

            count_windows++;
        }
    error_exit(WEXITSTATUS(pclose(fp)) == 127, command, "Exit code 127");

    qsort(windows, count_windows, sizeof(windows[0]), compare_windows);

    return windows;
}

void clean_exit(window_t **windows, char *input) {
    for(size_t i = 0; windows[i]; i++) {
        free(windows[i]->hint);
        free(windows[i]->class);
        free(windows[i]->title);
        free(windows[i]->id);
        free(windows[i]);
    }
    free(windows);
    free(input);

    endwin();

    exit(EXIT_SUCCESS);
}

void switch_to_window(window_t **windows, window_t *window, char *input) {
    // Select the first window if none was selected
    if (!window) {
        if (windows[0])
            window = windows[0];
        else
            return;
    }

    FILE *fp;
    char command[MAX_BUFFER_SIZE];
    sprintf(command, "wmctrl -i -a %s", window->id);
    fp = popen(command, "r");
        error_exit(!fp, command, strerror(errno));
    error_exit(WEXITSTATUS(pclose(fp)) == 127, command, "Exit code 127");

    clean_exit(windows, input); 
}

void setup_screen() {
    setlocale(LC_ALL, "");

    initscr();

    scrollok(stdscr,TRUE);    
    noecho();
    raw();

    // Colors
    use_default_colors();
    start_color();
    init_pair(1, COLOR_WHITE, COLOR_RED);
}

void draw_screen(
        window_t **windows,
        window_t **window_selected,
        char *input,
        int page) {
    clear();

    // Mark windows to be displayed
    size_t count_windows = 0;
    size_t count_matches = 0;
    *window_selected = NULL;
    for(size_t i = 0; windows[i]; i++, count_windows++) {
        char *hint = windows[i]->hint;
        if (match_prefix(hint, input)) {
            windows[i]->matched = true;
            count_matches++;
        } else {
            windows[i]->matched = false;
        }
    }

    int row, col;
    getmaxyx(stdscr, row, col);

    // Limit displayed windows by number of rows in the current screen
    size_t row_limit = min(count_windows, row - 1);
    size_t row_lower_bound = page * row_limit;
    size_t row_upper_bound = min(count_windows, (page + 1) * row_limit);

    // Draw entries
    for(size_t i = row_lower_bound; i < row_upper_bound; i++) {
        if (windows[i]->matched) {
            if (!(*window_selected)) {
                *window_selected = windows[i];
            }

            attron(COLOR_PAIR(1));
            attron(A_BOLD);
                // Subtract by one to leave an empty row for the prompt
                mvprintw(row - (row_upper_bound - i) - 1, 0, windows[i]->hint);
            attroff(A_BOLD);
            attroff(COLOR_PAIR(1));

            char description[MAX_BUFFER_SIZE];
            if (option_display_class) {
                snprintf(description, col - 1, " %s -> %s\n",
                        windows[i]->class,
                        windows[i]->title);
            } else {
                snprintf(description, col - 1, " %s\n",
                        windows[i]->title);
            }
            if (strcmp((*window_selected)->id, windows[i]->id) == 0) {
                attron(A_STANDOUT);
                    addstr(description);
                attroff(A_STANDOUT);
            } else {
                addstr(description);
            }
        }
    }

    // Activate selected window if there's no other matches
    if (count_matches == 1)
        switch_to_window(windows, *window_selected, input);

    // Prompt
    attron(A_BOLD);
        mvprintw(row - 1, 0, "> %s", input);
    attroff(A_BOLD);

    refresh();
}

int main() {
    window_t **windows = make_windows(option_hints);
    window_t *window_selected = NULL;
    char *input = (char*) calloc(MAX_BUFFER_SIZE, sizeof(char));
    int page = 0;

    setup_screen();
    draw_screen(windows, &window_selected, input, page);

    keys_enum key;
    while((key = getch())) {
        // parse escape sequence
        if (key == Esc) {
            nodelay(stdscr, TRUE);
            keys_enum key2 = getch();
            if ((int) key2 == -1)
                break;
            else {
                keys_enum key3 = getch();

                // scroll down
                if (key3 == ArrowDown || key3 == PageDown) {
                    page = max(page - 1, 0);

                // scroll up
                } else if (key3 == ArrowUp || key3 == PageUp) {
                    page++;
                }

                while(getch() > 0)
                    ; // NOTHING
                nodelay(stdscr, FALSE);
            }

        // terminate
        } else if (key == CtrlC || key == CtrlD || key == CtrlG) {
            break;

        // accept selection
        } else if (key == CtrlJ || key == CtrlM) {
            switch_to_window(windows, window_selected, input);

        // scroll up
        } else if (key == CtrlP) {
            page++;

        // scroll down
        } else if (key == CtrlN) {
            page = max(page - 1, 0);

        // delete last char
        } else if (key == CtrlH || key == BackSpace) {
            int index_last_char = strlen(input) - 1;
            if (index_last_char < 0)
                index_last_char = 0;
            input[index_last_char] = '\0';

        // delete input
        } else if (key == CtrlL) {
            memset(input, 0, strlen(input));

        // add key to input
        } else if (isprint(key)) {
            sprintf(input + strlen(input), "%c", (char) key);
        } 

        draw_screen(windows, &window_selected, input, page);
    }

    clean_exit(windows, input);

    return EXIT_SUCCESS;
}
