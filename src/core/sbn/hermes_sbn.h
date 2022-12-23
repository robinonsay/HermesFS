#ifndef HERMES_SBN_H
#define HERMES_SBN_H

#define HERMES_SBN_SUCCESS  (0)
#define HERMES_SBN_ERROR    (-1)

#ifndef HERMES_SBN_PIPE_DEPTH
#define HERMES_SBN_PIPE_DEPTH   (32)
#endif /* HERMES_SBN_PIPE_DEPTH */

#ifndef HERMES_SBN_IN_PIPE_SIZE
#define HERMES_SBN_IN_PIPE_SIZE    (0x20000)
#endif /* HERMES_SBN_IN_PIPE_SIZE */

#ifndef HERMES_SBN_OUT_PIPE_SIZE
#define HERMES_SBN_OUT_PIPE_SIZE  (0x20000)
#endif /* HERMES_SBN_OUT_PIPE_SIZE */

#define HERMES_SBN_SIZE         (HERMES_SBN_OUT_PIPE_SIZE + HERMES_SBN_IN_PIPE_SIZE)

#include "hermes_shm.h"

typedef struct HERMES_SBN_HEADER_TAG HERMES_SBN_HEADER_T;
typedef struct HERMES_SBN_ITEM_TAG HERMES_SBN_ITEM_T;
typedef struct HERMES_SBN_PIPE_TAG
{
    unsigned char*          startPtr;
    unsigned char*          endPtr;
    HERMES_SBN_HEADER_T*    hdrPtr;
} HERMES_SBN_PIPE_T;

typedef struct HERMES_SBN_TAG
{
    HERMES_SBN_PIPE_T       pipeIn;
    HERMES_SBN_PIPE_T       pipeOut;
    HERMES_SHM_CONTEXT_T    shmCntxt;
} HERMES_SBN_T;


int Hermes_SbnInit(HERMES_SBN_T* sbnPtr, char strAppName[]);
int Hermes_SbnPublish(HERMES_SBN_T* sbnPtr, void* srcPtr, unsigned long ulSrcSize);
int Hermes_SbnDequeue(HERMES_SBN_T* sbnPtr, HERMES_SBN_PIPE_T* pipePtr, void* destPtr, unsigned long destSize);
void Hermes_SbnClose(HERMES_SBN_T* sbnPtr);

#endif /* HERMES_SBN_H */
