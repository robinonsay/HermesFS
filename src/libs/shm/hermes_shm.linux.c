#include "hermes_shm.h"

#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <sys/mman.h>
#include <sys/stat.h>        /* For mode constants */
#include <fcntl.h>           /* For O_* constants */

struct HERMES_SHM_LOCKS_TAG
{
    volatile uint16_t   uiNumRdrs;
    volatile uint8_t    uiRLck;
    volatile uint8_t    uiWLck;
};

static int IOClose(int fd);
static void SpinLck(volatile uint8_t* uiLck);
static void ClearLck(volatile uint8_t* uiLck);

int Hermes_ShmInit(HERMES_SHM_CONTEXT_T* shmCntxtPtr, const char* strShmName, unsigned long ulSizeShm)
{
    int iStatus = HERMES_SHM_SUCCESS;
    int iShmFd = shm_open(strShmName, O_RDWR | O_CREAT, S_IRWXU);
    if(iShmFd == -1)
    {
        iStatus = HERMES_SHM_ERROR;
        goto exit;
    }
    shmCntxtPtr->ulSizeShm = ulSizeShm + sizeof(HERMES_SHM_LOCKS_T);
    if(!ftruncate(iShmFd, shmCntxtPtr->ulSizeShm))
    {
        iStatus = HERMES_SHM_ERROR;
        goto exit;
    }
    shmCntxtPtr->vShmPtr = mmap(NULL, shmCntxtPtr->ulSizeShm, PROT_EXEC | PROT_READ | PROT_WRITE, MAP_SHARED, iShmFd, 0);
    if(shmCntxtPtr->vShmPtr == MAP_FAILED)
    {
        iStatus = HERMES_SHM_ERROR;
        goto exit;
    }
    shmCntxtPtr->strShmName = strShmName;
    shmCntxtPtr->lockPtr = (HERMES_SHM_LOCKS_T*) ((uint8_t*) shmCntxtPtr->vShmPtr) + ulSizeShm;
exit:
    if(iShmFd != -1)
    {
        IOClose(iShmFd);
    }
    return iStatus;
}

void Hermes_ShmClose(HERMES_SHM_CONTEXT_T* shmCntxtPtr)
{
    if(!munmap(shmCntxtPtr->vShmPtr, shmCntxtPtr->ulSizeShm))
    {
        shm_unlink(shmCntxtPtr->strShmName);
    }
}

void Hermes_ShmWLck(HERMES_SHM_CONTEXT_T* shmCntxtPtr)
{
    SpinLck(&shmCntxtPtr->lockPtr->uiWLck);
}

void Hermes_ShmWUnlck(HERMES_SHM_CONTEXT_T* shmCntxtPtr)
{
    ClearLck(&shmCntxtPtr->lockPtr->uiWLck);
}

void Hermes_ShmRLck(HERMES_SHM_CONTEXT_T* shmCntxtPtr)
{
    SpinLck(&shmCntxtPtr->lockPtr->uiRLck);
    shmCntxtPtr->lockPtr->uiNumRdrs++;
    if(shmCntxtPtr->lockPtr->uiNumRdrs == 1)
    {
        SpinLck(&shmCntxtPtr->lockPtr->uiWLck);
    }
    ClearLck(&shmCntxtPtr->lockPtr->uiRLck);
}

void Hermes_ShmRUnLck(HERMES_SHM_CONTEXT_T* shmCntxtPtr)
{
    SpinLck(&shmCntxtPtr->lockPtr->uiRLck);
    shmCntxtPtr->lockPtr->uiNumRdrs--;
    if(shmCntxtPtr->lockPtr->uiNumRdrs == 0)
    {
        ClearLck(&shmCntxtPtr->lockPtr->uiWLck);
    }
    ClearLck(&shmCntxtPtr->lockPtr->uiRLck);
}

static int IOClose(int fd)
{
    int iStatus = HERMES_SHM_SUCCESS;
    while(!close(fd))
    {
        if(errno != EINTR)
        {
            iStatus = HERMES_SHM_ERROR;
            break;
        }
    }
    return iStatus;
}

static void SpinLck(volatile uint8_t* uiLck)
{
    while(__atomic_test_and_set(uiLck, __ATOMIC_RELAXED));
}

static void ClearLck(volatile uint8_t* uiLck)
{
    __atomic_clear(uiLck, __ATOMIC_RELAXED);
}
