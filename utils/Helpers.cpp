#include "Helpers.h"

char *basename(const char *path) {
    const char *base = path;
    while (*path) {
        if (*path++ == '/') {
            base = path;
        }
    }
    return (char *) base;
}

void check_ptr(void *p, const char *file, int line, const char *func) {
    UNUSED(file);
    UNUSED(line);
    UNUSED(func);
    if (p == NULL) {
        FURI_LOG_W("App", "[NULLPTR] %s:%s():%i", basename((char *) file), func, line);
    }
}

float lerp(float a, float b, float t) {
    if (t > 1) return b;
    return (1 - t) * a + t * b;
}

LogTimer::LogTimer(const char *n):name(n) {
    start = furi_get_tick();
}

LogTimer::~LogTimer() {
    FURI_LOG_D("App", "%s took %fms", name, furi_get_tick()-start);
}
