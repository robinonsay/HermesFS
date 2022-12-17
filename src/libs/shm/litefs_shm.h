#ifndef LITEFS_SHM_H
#define LITEFS_SHM_H

#define LITEFS_SHM_SUCCESS  (0)
#define LITEFS_SHM_ERROR    (-1)

typedef struct LITEFS_SHM_LOCKS_TAG LITEFS_SHM_LOCKS_T;

typedef struct LITEFS_SHM_CONTEXT_TAG
{
    const char*         strShmName;
    void*               vShmPtr;
    LITEFS_SHM_LOCKS_T* lockPtr;
    unsigned long       ulSizeShm;
} LITEFS_SHM_CONTEXT_T;

int LiteFS_ShmInit(LITEFS_SHM_CONTEXT_T* shmCntxtPtr, const char* strShmName, unsigned long ulSizeShm);
void LiteFS_ShmClose(LITEFS_SHM_CONTEXT_T* shmCntxtPtr);
void LiteFS_ShmWLck(LITEFS_SHM_CONTEXT_T* shmCntxtPtr);
void LiteFS_ShmWUnlck(LITEFS_SHM_CONTEXT_T* shmCntxtPtr);
void LiteFS_ShmRLck(LITEFS_SHM_CONTEXT_T* shmCntxtPtr);
void LiteFS_ShmRUnLck(LITEFS_SHM_CONTEXT_T* shmCntxtPtr);

#endif /* LITEFS_SHM_H */
