#include "hermes_app.h"
#include "hermes_sbn.h"
#include <dlfcn.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/types.h>
#include <dirent.h>

#define PATH_SIZE           (1024)
#define MAX_CHILD_PROCESSES (256)

struct GLOBALS_TAG
{
    char            strAppName[PATH_SIZE];
    HERMES_SBN_T    sbn;
    HERMES_API_T    appApi;
    pid_t           pidChildren[MAX_CHILD_PROCESSES];
    uint16_t        uiPidIndex;
    uint8_t         uiIsRoot;
} gGlobals = {0};


void OnExit(int iSigNum);
static void RunApp(const char* strLibPath);
static int RootInit();
static void RootProcess();

int main(int argc, char const *argv[])
{
    int iStatus = HERMES_APP_SUCCESS;
    struct sigaction sigHandler;
    sigHandler.sa_handler = OnExit;
    if(sigaction(SIGINT, &sigHandler, NULL))
    {
        perror("HERMES ERROR: Cannot set sig handler");
        iStatus = HERMES_APP_ERROR;
        goto exit;
    }
    char path[256] = "./app/";
    DIR* appDirPtr;
    if(argc == 2)
    {
        strcpy(path, argv[1]);
    }
    appDirPtr = opendir(path);
    if(!appDirPtr)
    {
        perror("HERMES ERROR: Could not open app directory");
        goto exit;
    }
    printf("STARTING HERMES\n");
    RootInit();
    for(struct dirent* entryPtr = readdir(appDirPtr); entryPtr; entryPtr = readdir(appDirPtr))
    {
        if(entryPtr->d_type == DT_REG)
        {
            strcat(path, entryPtr->d_name);
            RunApp(path);
        }
    }
    if(gGlobals.uiIsRoot)
    {
        RootProcess();
    }
exit:
    return iStatus;
}

void OnExit(int iSigNum)
{
    if(gGlobals.uiIsRoot)
    {
        printf("\nClosing Root Process\n");
        Hermes_SbnClose(&gGlobals.sbn);
    }
    else
    {
        printf("\nClosing %s\n", gGlobals.appApi.strAppName);
        gGlobals.appApi.Hermes_AppExit();
    }
    size_t sPidSize = sizeof(gGlobals.pidChildren)/sizeof(gGlobals.pidChildren[0]);
    for(uint16_t i = 0; i < sPidSize; i++)
    {
        if(gGlobals.pidChildren[gGlobals.uiPidIndex] > 0)
        {
            kill(gGlobals.pidChildren[i], SIGINT);
        }
    }
    exit(0);
}

static void RunApp(const char* strLibPath)
{
    void* handle = dlopen(strLibPath, RTLD_LAZY);
    if(handle == NULL)
    {
        printf("Could not load dll\n");
    }else
    {
        gGlobals.uiPidIndex++;
        gGlobals.pidChildren[gGlobals.uiPidIndex] = fork();
        if(gGlobals.pidChildren[gGlobals.uiPidIndex] > 0)
        {
            printf("HERMES LOG: Closing handle\n");
            dlclose(handle);
            printf("App PID: [%i]\n", gGlobals.pidChildren[gGlobals.uiPidIndex]);
            gGlobals.uiIsRoot = 1;
        }
        else if(gGlobals.pidChildren[gGlobals.uiPidIndex] == 0)
        {
            void (*AppApiInit)(HERMES_API_T* apiPtr) = (void (*)(HERMES_API_T*)) dlsym(handle, "Hermes_RegisterApp");
            AppApiInit(&gGlobals.appApi);
            printf("Starting %s\n", gGlobals.appApi.strAppName);
            gGlobals.appApi.Hermes_AppInit();
            gGlobals.appApi.Hermes_AppRun();
        }
        else
        {
            perror("FORK ERROR");
        }
    }
}

static int RootInit()
{
    strcpy(gGlobals.strAppName, "SampleApp");
    int iStatus = Hermes_SbnInit(&gGlobals.sbn, gGlobals.strAppName);
    return iStatus;
}

static void RootProcess()
{
    char str[PATH_SIZE] = {0};
    while(1)
    {
        while(Hermes_SbnDequeue(&gGlobals.sbn, &gGlobals.sbn.pipeOut, str, sizeof(str)) == HERMES_SBN_SUCCESS)
        {
            printf("Recvd Data: %s\n", str);
        }
        sleep(1);
    }
}
