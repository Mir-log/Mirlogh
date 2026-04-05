#ifndef MIRROR_H
#define MIRROR_H

// does not edit the .h files name due to it's my femboy friend's name.qwq

#include <stdio.h>
#include <time.h>
#include <string.h>

#ifdef _WIN32
#include <windows.h>
#endif

#define MIRROR_FAST()                      \
    do {                                   \
        setbuf(stdout, NULL);              \
    } while (0)

#ifdef _WIN32
static inline void mirror_vt_mode(void) {
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut == INVALID_HANDLE_VALUE)
        return;
    DWORD dwMode = 0;
    if (!GetConsoleMode(hOut, &dwMode))
        return;
    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode(hOut, dwMode);
}
#else
static inline void mirror_vt_mode(void) {}
#endif

static inline void mirror_vt_mode_auto(void) {
#ifdef _WIN32
    static int done = 0;
    if (!done) {
        mirror_vt_mode();
        done = 1;
    }
#else
    (void)0;
#endif
}

enum mirror_mode {
    MIRROR_CLEARSTRWITHOUTANOTHER,
    MIRROR_TIMESTRCLEAR,
    MIRROR_NOCOLOR,
    MIRROR_COLOREDSTR
};

enum mirror_lv {
    MIRROR_INFO,
    MIRROR_ACCESS,
    MIRROR_WARN,
    MIRROR_ERROR,
    MIRROR_PANIC
};

enum mirror_colors {
    MIRROR_GREEN,
    MIRROR_BLUE,
    MIRROR_RED,
    MIRROR_WHITE,
    MIRROR_YELLOW,
    MIRROR_AUTO
};

static inline const char* mirror_match_lv(enum mirror_lv lv) {
    switch (lv) {
        case MIRROR_INFO:   return "[INFO]";
        case MIRROR_ACCESS: return "[ACCESS]";
        case MIRROR_WARN:   return "[WARN]";
        case MIRROR_ERROR:  return "[ERROR]";
        case MIRROR_PANIC:  return "[PANIC]";
        default:            return "[UNKNOWN]";
    }
}

static inline const char* mirror_color_code(enum mirror_colors col) {
    switch (col) {
        case MIRROR_GREEN:  return "\033[32m";
        case MIRROR_BLUE:   return "\033[34m";
        case MIRROR_RED:    return "\033[31m";
        case MIRROR_WHITE:  return "\033[37m";
        case MIRROR_YELLOW: return "\033[33m";
        default:            return "\033[0m";
    }
}

static inline void mirror_utcnow(char *buf, size_t size) {
    time_t now = time(NULL);
    struct tm tm_now;
#ifdef _WIN32
    gmtime_s(&tm_now, &now);
#else
    gmtime_r(&now, &tm_now);
#endif
    strftime(buf, size, "%Y-%m-%d %H:%M:%S UTC", &tm_now);
}

static inline void mirror_mirprint(enum mirror_lv lv, const char *msg,
                                   enum mirror_mode mode, enum mirror_colors col) {
    mirror_vt_mode_auto();

    char timebuf[64];
    mirror_utcnow(timebuf, sizeof(timebuf));

    switch (mode) {
        case MIRROR_CLEARSTRWITHOUTANOTHER:
            printf("%s\n", msg);
            break;
        case MIRROR_TIMESTRCLEAR:
            printf("%s %s\n", timebuf, msg);
            break;
        case MIRROR_NOCOLOR:
            printf("%s %s %s\n", timebuf, mirror_match_lv(lv), msg);
            break;
        case MIRROR_COLOREDSTR: {
            enum mirror_colors use = col;
            if (use == MIRROR_AUTO) {
                switch (lv) {
                    case MIRROR_INFO:   use = MIRROR_WHITE; break;
                    case MIRROR_ACCESS: use = MIRROR_GREEN; break;
                    case MIRROR_WARN:   use = MIRROR_YELLOW; break;
                    case MIRROR_ERROR:
                    case MIRROR_PANIC:  use = MIRROR_RED; break;
                    default:            use = MIRROR_WHITE; break;
                }
            }
            printf("%s%s %s %s\033[0m\n",
                   mirror_color_code(use), timebuf,
                   mirror_match_lv(lv), msg);
            break;
        }
        default:
            printf("%s [UNKNOWN] %s\n", timebuf, msg);
            break;
    }
}

#endif // MIRROR_H
