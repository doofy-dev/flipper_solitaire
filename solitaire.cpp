#include <furi.h>
#include "GameLoop.h"

#ifdef __cplusplus
extern "C"
{
#endif
int32_t solitaire_app(void *p) {
    UNUSED(p);
    int32_t return_code = 0;
    auto *loop = new GameLoop();
    loop->Start();
    delete loop;
    return return_code;
}

#ifdef __cplusplus
}
#endif