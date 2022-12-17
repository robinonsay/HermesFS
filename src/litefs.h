#ifndef LITEFS_H
#define LITEFS_H

#define LITEFS_SUCCESS  (0)
#define LITEFS_ERROR    (-1)

#include <stdint.h>

typedef struct LITEFS_API_TAG{
    uint32_t (*LiteFS_AppInit)();
    uint32_t (*LiteFS_AppRun)();
} LITEFS_API_T;

LITEFS_API_T gLiteFS_AppAPI;

void LiteFS_ApiInit();

#endif /* LITEFS_H */
