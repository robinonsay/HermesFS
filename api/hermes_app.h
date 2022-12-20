#ifndef HERMES_APP_H
#define HERMES_APP_H

#define HERMES_APP_SUCCESS  (0)
#define HERMES_APP_ERROR    (-1)

#define HERMES_APP_NAME_LEN (256)

typedef struct HERMES_API_TAG{
    char*   strAppName;
    int     (*Hermes_AppInit)();
    int     (*Hermes_AppRun)();
    void    (*Hermes_AppExit)();
} HERMES_API_T;

void Hermes_RegisterApp(HERMES_API_T* apiPtr);

#endif /* HERMES_APP_H */
