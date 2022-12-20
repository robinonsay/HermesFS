#ifndef HERMES_SHM_H
#define HERMES_SHM_H

#define HERMES_SHM_SUCCESS  (0)
#define HERMES_SHM_ERROR    (-1)

typedef struct HERMES_SHM_LOCKS_TAG HERMES_SHM_LOCKS_T;

typedef struct HERMES_SHM_CONTEXT_TAG
{
    const char*         strShmName;
    void*               vShmPtr;
    HERMES_SHM_LOCKS_T* lockPtr;
    unsigned long       ulSizeShm;
} HERMES_SHM_CONTEXT_T;

int Hermes_ShmInit(HERMES_SHM_CONTEXT_T* shmCntxtPtr, const char* strShmName, unsigned long ulSizeShm);
void Hermes_ShmClose(HERMES_SHM_CONTEXT_T* shmCntxtPtr);
void Hermes_ShmWLck(HERMES_SHM_CONTEXT_T* shmCntxtPtr);
void Hermes_ShmWUnlck(HERMES_SHM_CONTEXT_T* shmCntxtPtr);
void Hermes_ShmRLck(HERMES_SHM_CONTEXT_T* shmCntxtPtr);
void Hermes_ShmRUnLck(HERMES_SHM_CONTEXT_T* shmCntxtPtr);

#endif /* HERMES_SHM_H */
