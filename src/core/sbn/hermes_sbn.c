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
    Hermes_ShmWLck(&sbnPtr->shmCntxt);
    memcpy(sbnPtr->pipeOut.hdrPtr, &hdrZero, sizeof(hdrZero));
    memset(sbnPtr->pipeOut.startPtr, 0, HERMES_SBN_OUT_PIPE_SIZE);
    Hermes_ShmWUnlck(&sbnPtr->shmCntxt);
    return iStatus;
}

int Hermes_SbnPublish(HERMES_SBN_T* sbnPtr, void* srcPtr, unsigned long ulSrcSize)
{
    int iStatus = HERMES_SBN_ERROR;
    HERMES_SBN_HEADER_T* hdrPtr = sbnPtr->pipeOut.hdrPtr;
    HERMES_SBN_ITEM_T* lastItemPtr = NULL;
    HERMES_SBN_ITEM_T* nextItemPtr = NULL;
    unsigned long ulOffst = 0;
    unsigned long ulItemSize = ulSrcSize + sizeof(HERMES_SBN_ITEM_T);
    Hermes_ShmWLck(&sbnPtr->shmCntxt);
    printf("PUB\n");
    if(hdrPtr->ulPipeSize + ulItemSize <= HERMES_SBN_OUT_PIPE_SIZE)
    {
        lastItemPtr = (HERMES_SBN_ITEM_T*)(sbnPtr->pipeOut.startPtr + hdrPtr->ulBackItemOffst);
        if(lastItemPtr->ulNextItemOffst + ulItemSize < HERMES_SBN_OUT_PIPE_SIZE)
        {
            ulOffst = lastItemPtr->ulNextItemOffst + ulItemSize;
            nextItemPtr = (HERMES_SBN_ITEM_T*)(sbnPtr->pipeOut.startPtr + lastItemPtr->ulNextItemOffst);
        }
        else if(ulItemSize < hdrPtr->ulFrontItemOffst)
        {
            ulOffst = ulItemSize;
            nextItemPtr = (HERMES_SBN_ITEM_T*) (sbnPtr->pipeOut.startPtr);
        }
        else
        {
            nextItemPtr = NULL;
        }
    }
    if(lastItemPtr && nextItemPtr)
    {
        void* destPtr = nextItemPtr + 1;
        nextItemPtr->ulDataSize = ulSrcSize;
        nextItemPtr->ulNextItemOffst = ulOffst;
        hdrPtr->ulPipeSize += ulItemSize;
        memcpy(destPtr, srcPtr, ulSrcSize);
        iStatus = HERMES_SBN_SUCCESS;
    }
    printf("FRONT_ITEM: %lu, BACK_ITEM: %lu\n", hdrPtr->ulFrontItemOffst, hdrPtr->ulBackItemOffst);
    Hermes_ShmWUnlck(&sbnPtr->shmCntxt);
    return iStatus;
}

int Hermes_SbnDequeue(HERMES_SBN_T* sbnPtr, HERMES_SBN_PIPE_T* pipePtr, void* destPtr, unsigned long destSize)
{
    int iStatus = HERMES_SBN_ERROR;
    Hermes_ShmWLck(&sbnPtr->shmCntxt);
    HERMES_SBN_ITEM_T* firstItem = (HERMES_SBN_ITEM_T*)(pipePtr->startPtr + pipePtr->hdrPtr->ulFrontItemOffst);
    if(firstItem->ulDataSize > 0 && firstItem->ulDataSize < destSize)
    {
        void* srcPtr = firstItem + 1;
        memcpy(destPtr, srcPtr, firstItem->ulDataSize);
        iStatus = HERMES_SBN_SUCCESS;
    }
    if(iStatus == HERMES_SBN_SUCCESS)
    {
        pipePtr->hdrPtr->ulPipeSize -= (firstItem->ulDataSize + sizeof(HERMES_SBN_ITEM_T));
        pipePtr->hdrPtr->ulFrontItemOffst = firstItem->ulNextItemOffst;
        firstItem->ulDataSize = 0;
        firstItem->ulNextItemOffst = 0;
    }
    Hermes_ShmWUnlck(&sbnPtr->shmCntxt);
    return iStatus;
}

void Hermes_SbnClose(HERMES_SBN_T* sbnPtr)
{
    Hermes_ShmClose(&sbnPtr->shmCntxt);
}
