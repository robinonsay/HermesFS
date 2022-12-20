#ifndef HERMES_SBN_H
#define HERMES_SBN_H

#define HERMES_SBN_SUCCESS  (0)
#define HERMES_SBN_ERROR    (-1)

#ifndef HERMES_SBN_PIPE_DEPTH
#define HERMES_SBN_PIPE_DEPTH   (32)
#endif /* HERMES_SBN_PIPE_DEPTH */

#ifndef HERMES_SBN_PIPE_SIZE
#define HERMES_SBN_PIPE_SIZE    (0x8000)
#endif /* HERMES_SBN_PIPE_SIZE */

#ifndef HERMES_SBN_OUTBOX_SIZE
#define HERMES_SBN_OUTBOX_SIZE  (0x8000)
#endif /* HERMES_SBN_OUTBOX_SIZE */

#define HERMES_SBN_SIZE         (HERMES_SBN_OUTBOX_SIZE + HERMES_SBN_PIPE_SIZE)

#include "shm/hermes_shm.h"

typedef struct HERMES_SBN_ITEM_TAG HERMES_SBN_ITEM_T;
typedef struct HERMES_SBN_TAG
{
    struct
    {
        unsigned long           ulLength;
        unsigned long           ulSize;
        HERMES_SBN_ITEM_T*      startPtr;
        HERMES_SBN_ITEM_T*      endPtr;
    } pipe;
    struct
    {
        unsigned long           ulLength;
        unsigned long           ulSize;
        HERMES_SBN_ITEM_T*      startPtr;
        HERMES_SBN_ITEM_T*      endPtr;
    } outbox;
    HERMES_SHM_CONTEXT_T    shmCntxt;
} HERMES_SBN_T;


int Hermes_SbnInit(HERMES_SBN_T* sbnPtr, char strAppName[]);
int Hermes_SbnPublish(HERMES_SBN_T* sbnPtr, void* srcPtr, unsigned long ulSrcSize);
int Hermes_SbnDequeueOutbox(HERMES_SBN_T* sbnPtr, void* destPtr);
void Hermes_SbnClose(HERMES_SBN_T* sbnPtr);

#endif /* HERMES_SBN_H */
