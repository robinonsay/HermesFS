#include "hermes_sbn.h"
#include "hermes_shm.h"

#include <string.h>
#include <stdio.h>

struct HERMES_SBN_HEADER_TAG
{
    unsigned long   ulFrontItemOffst;
    unsigned long   ulBackItemOffst;
    unsigned long   ulPipeSize;
};

struct HERMES_SBN_ITEM_TAG
{
    unsigned long   ulDataSize;
    unsigned long   ulNextItemOffst;
};

int Hermes_SbnInit(HERMES_SBN_T* sbnPtr, char strAppName[])
{
    memset(sbnPtr, 0, sizeof(HERMES_SBN_T));
    int iStatus = Hermes_ShmInit(&sbnPtr->shmCntxt, strAppName, sizeof(HERMES_SBN_HEADER_T) + HERMES_SBN_SIZE);
    sbnPtr->pipeOut.hdrPtr = (HERMES_SBN_HEADER_T*) sbnPtr->shmCntxt.vShmPtr;
    sbnPtr->pipeOut.startPtr = (unsigned char*)(sbnPtr->pipeOut.hdrPtr + 1);
    sbnPtr->pipeOut.endPtr = sbnPtr->pipeOut.startPtr + HERMES_SBN_OUT_PIPE_SIZE;
    HERMES_SBN_HEADER_T hdrZero = {0};
    HERMES_SBN_ITEM_T itemZero = {0};
    Hermes_ShmWLck(&sbnPtr->shmCntxt);
    memcpy(sbnPtr->pipeOut.hdrPtr, &hdrZero, sizeof(hdrZero));
    memcpy(sbnPtr->pipeOut.startPtr, &itemZero, sizeof(itemZero));
    Hermes_ShmWUnlck(&sbnPtr->shmCntxt);
    return iStatus;
}

int Hermes_SbnPublish(HERMES_SBN_T* sbnPtr, void* srcPtr, unsigned long ulSrcSize)
{
    int iStatus = HERMES_SBN_ERROR;
    HERMES_SBN_HEADER_T* hdrPtr = sbnPtr->pipeOut.hdrPtr;
    HERMES_SBN_ITEM_T* lastItemPtr = NULL;
    HERMES_SBN_ITEM_T* newItemPtr = NULL;
    unsigned long ulItemSize = ulSrcSize + sizeof(HERMES_SBN_ITEM_T);
    Hermes_ShmWLck(&sbnPtr->shmCntxt);
    lastItemPtr = (HERMES_SBN_ITEM_T*)(sbnPtr->pipeOut.startPtr + hdrPtr->ulBackItemOffst);
    if(hdrPtr->ulPipeSize == 0 && hdrPtr->ulBackItemOffst + ulItemSize < HERMES_SBN_OUT_PIPE_SIZE)
    {
        newItemPtr = lastItemPtr;
        hdrPtr->ulFrontItemOffst = hdrPtr->ulBackItemOffst;
    }
    else if(hdrPtr->ulPipeSize + ulItemSize < HERMES_SBN_OUT_PIPE_SIZE && lastItemPtr->ulNextItemOffst + ulItemSize < HERMES_SBN_OUT_PIPE_SIZE)
    {
        hdrPtr->ulBackItemOffst = lastItemPtr->ulNextItemOffst;
        newItemPtr = (HERMES_SBN_ITEM_T*)(sbnPtr->pipeOut.startPtr + hdrPtr->ulBackItemOffst);
        
    }
    else if(hdrPtr->ulPipeSize + ulItemSize < HERMES_SBN_OUT_PIPE_SIZE)
    {
        newItemPtr = (HERMES_SBN_ITEM_T*)sbnPtr->pipeOut.startPtr;
        hdrPtr->ulBackItemOffst = 0;
    }
    else
    {
        iStatus = HERMES_SBN_ERROR;
    }
    if(lastItemPtr && newItemPtr)
    {
        void* destPtr = newItemPtr + 1;
        memcpy(destPtr, srcPtr, ulSrcSize);
        newItemPtr->ulDataSize = ulSrcSize;
        newItemPtr->ulNextItemOffst = hdrPtr->ulBackItemOffst + ulItemSize;
        hdrPtr->ulPipeSize += ulItemSize;
    }
    Hermes_ShmWUnlck(&sbnPtr->shmCntxt);
    return iStatus;
}

int Hermes_SbnDequeue(HERMES_SBN_T* sbnPtr, HERMES_SBN_PIPE_T* pipePtr, void* destPtr, unsigned long destSize)
{
    int iStatus = HERMES_SBN_ERROR;
    Hermes_ShmWLck(&sbnPtr->shmCntxt);
    HERMES_SBN_ITEM_T* firstItem = (HERMES_SBN_ITEM_T*)(pipePtr->startPtr + pipePtr->hdrPtr->ulFrontItemOffst);
    if(firstItem->ulDataSize > 0 && firstItem->ulDataSize < destSize && pipePtr->hdrPtr->ulPipeSize > 0)
    {
        void* srcPtr = firstItem + 1;
        memcpy(destPtr, srcPtr, firstItem->ulDataSize);
        pipePtr->hdrPtr->ulPipeSize -= (firstItem->ulDataSize + sizeof(HERMES_SBN_ITEM_T));
        pipePtr->hdrPtr->ulFrontItemOffst = firstItem->ulNextItemOffst;
        iStatus = HERMES_SBN_SUCCESS;
    }
    Hermes_ShmWUnlck(&sbnPtr->shmCntxt);
    return iStatus;
}

void Hermes_SbnClose(HERMES_SBN_T* sbnPtr)
{
    Hermes_ShmClose(&sbnPtr->shmCntxt);
}
