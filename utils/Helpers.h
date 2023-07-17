#pragma once
#define M_PIX2		6.28318530717958647692	/* 2 pi */
#include "furi.h"
#define l_abs(x) ((x) < 0 ? -(x) : (x))
#define DEG_2_RAD  0.01745329251994329576f
#define RAD_2_DEG  565.48667764616278292327f

#define DEBUG_BUILD
#ifdef DEBUG_BUILD
#define check_pointer(X) check_ptr( X, __FILE__, __LINE__, __FUNCTION__)
#else
#define check_pointer(X) while(0)
#endif

#define CHECK_HEAP() FURI_LOG_I("FlipperGameEngine", "Free/total heap: %zu / %zu", memmgr_get_free_heap(), memmgr_get_total_heap())
char *basename(const char *path);

void check_ptr(void *p, const char *file, int line, const char *func);

float lerp(float a, float b, float t);

class LogTimer{
    double start;
    const char* name;
public:
    explicit LogTimer(const char* name);
    ~LogTimer();
};

class TypeRegistry {
public:
    template <typename T>
    static int getTypeID() {
        static int typeID = getNextTypeID();
        return typeID;
    }

private:
    static int getNextTypeID() {
        static int currentTypeID = 0;
        return ++currentTypeID;
    }
};