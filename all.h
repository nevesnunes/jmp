#pragma once

#include <ctype.h>
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ncurses keycode translation
typedef enum {
    CtrlA = 1,
    CtrlB,
    CtrlC,
    CtrlD,
    CtrlE,
    CtrlF,
    CtrlG,
    CtrlH,
    Tab,
    CtrlJ,
    CtrlK,
    CtrlL,
    CtrlM,
    CtrlN,
    CtrlO,
    CtrlP,
    CtrlQ,
    CtrlR,
    CtrlS,
    CtrlT,
    CtrlU,
    CtrlV,
    CtrlW,
    CtrlX,
    CtrlY,
    CtrlZ,
    Esc,

    Del = 51,
    End,
    PageUp,
    PageDown,

    ArrowUp = 65,
    ArrowDown,
    ArrowRight,
    ArrowLeft,

    BackSpace = 127
} keys_enum;

// basic helper functions
int match_prefix(const char *str, const char *pre);
int ipow(int base, int exp);
int max(int a, int b);
int min(int a, int b);

// exit with failure?
void error_exit(bool condition, const char *command, const char *cause);
