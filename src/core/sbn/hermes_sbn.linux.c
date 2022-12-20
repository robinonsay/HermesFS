#include "hermes_sbn.h"
#include "shm/hermes_shm.h"

#include <string.h>

struct HERMES_SBN_ITEM_TAG
{
    unsigned long   ulIndex;
    void*           dataPtr;
    unsigned long   ulDataSize;
    struct HERMES_SBN_ITEM_TAG* prevItemPtr;
    struct HERMES_SBN_ITEM_TAG* nextItemPtr;
} ;

int Hermes_SbnInit(HERMES_SBN_T* sbnPtr, char strAppName[])
{
    memset(sbnPtr, 0, sizeof(HERMES_SBN_T));
    int iStatus = Hermes_ShmInit(&sbnPtr->shmCntxt, strAppName, HERMES_SBN_SIZE);
    return iStatus;
}

int Hermes_SbnPublish(HERMES_SBN_T* sbnPtr, void* srcPtr, unsigned long ulSrcSize)
{
    int iStatus = HERMES_SBN_SUCCESS;
    unsigned long ulItemSize = ulSrcSize + sizeof(HERMES_SBN_ITEM_T);
    if(sbnPtr->outbox.startPtr == NULL)
    {
        sbnPtr->outbox.startPtr = (HERMES_SBN_ITEM_T*) sbnPtr->shmCntxt.vShmPtr;
    }
    if(sbnPtr->outbox.endPtr == NULL)
    {
        sbnPtr->outbox.endPtr = sbnPtr->outbox.startPtr;
    }
    if((ulItemSize + sbnPtr->outbox.ulSize) > HERMES_SBN_OUTBOX_SIZE)
    {
        iStatus = HERMES_SBN_ERROR;
        goto exit;
    }
    Hermes_ShmWLck(&sbnPtr->shmCntxt);
    sbnPtr->outbox.endPtr->nextItemPtr = (HERMES_SBN_ITEM_T*) ((unsigned char*) sbnPtr->outbox.endPtr->dataPtr + sbnPtr->outbox.endPtr->ulDataSize);
    sbnPtr->outbox.endPtr->nextItemPtr->prevItemPtr = sbnPtr->outbox.endPtr;
    sbnPtr->outbox.endPtr = sbnPtr->outbox.endPtr->nextItemPtr;
    sbnPtr->outbox.endPtr->ulIndex = sbnPtr->outbox.endPtr->prevItemPtr->ulIndex + 1;
    sbnPtr->outbox.endPtr->ulDataSize = ulSrcSize;
    sbnPtr->outbox.endPtr->dataPtr = sbnPtr->outbox.endPtr + sizeof(HERMES_SBN_ITEM_T);
    memcpy(sbnPtr->outbox.endPtr->dataPtr, srcPtr, ulSrcSize);
    sbnPtr->outbox.ulLength++;
    sbnPtr->outbox.ulSize += ulItemSize;
    Hermes_ShmWUnlck(&sbnPtr->shmCntxt);
exit:
    return iStatus;
}

int Hermes_SbnDequeueOutbox(HERMES_SBN_T* sbnPtr, void* destPtr)
{
    int iStatus = HERMES_SBN_ERROR;
    if(sbnPtr->outbox.startPtr != NULL){
        Hermes_ShmRLck(&sbnPtr->shmCntxt);
        memcpy(destPtr, sbnPtr->outbox.startPtr->dataPtr, sbnPtr->outbox.startPtr->ulDataSize);
        sbnPtr->outbox.ulSize -= sizeof(HERMES_SBN_ITEM_T) + sbnPtr->outbox.startPtr->ulDataSize;
        sbnPtr->outbox.startPtr = sbnPtr->outbox.startPtr->nextItemPtr;
        Hermes_ShmRUnLck(&sbnPtr->shmCntxt);
        iStatus = HERMES_SBN_SUCCESS;
        sbnPtr->outbox.ulLength--;
    }
    return iStatus;
}

void Hermes_SbnClose(HERMES_SBN_T* sbnPtr)
{
    Hermes_ShmClose(&sbnPtr->shmCntxt);
}
