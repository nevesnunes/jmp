#include "all.h"

int match_prefix(const char *str, const char *pre) {
    if (!pre || !str)
        return 0;

    size_t length_pre = strlen(pre), length_str = strlen(str);

    return (strlen(pre) == 0) && (length_str < length_pre) ?
            0 :
            strncmp(pre, str, length_pre) == 0;
}

int ipow(int base, int exp) {
    int result = 1;
    while (exp) {
        if (exp & 1)
            result *= base;
        exp >>= 1;
        base *= base;
    }

    return result;
}

int max(int a, int b) { return a > b ? a : b; }

int min(int a, int b) { return a < b ? a : b; }

void error_exit(bool condition, const char *command, const char *cause) {
    if (condition) {
        printf("Failed to run command \"%s\": %s\n", command, cause);
        exit(1);
    }
}
