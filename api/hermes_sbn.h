#ifndef HERMES_SBN_H
#define HERMES_SBN_H

#define HERMES_SBN_SUCCESS  (0)
#define HERMES_SBN_ERROR    (-1)

typedef struct HERMES_SBN_APP_CNTXT_TAG HERMES_SBN_APP_CNTXT_T;

int Hermes_SbnInit(HERMES_SBN_APP_CNTXT_T* sbnPtr, char strAppName[]);
int Hermes_SbnSubscribe(HERMES_SBN_APP_CNTXT_T* sbnPtr, const char strAppName[], unsigned long ulMid);
void Hermes_SbnPublish(HERMES_SBN_APP_CNTXT_T* sbnPtr, void* msgPtr, unsigned long ulMsgSize);
void* Hermes_SbnGetMsg(HERMES_SBN_APP_CNTXT_T* sbnPtr, const char strAppName[], unsigned long ulMid);

#endif /* HERMES_SBN_H */