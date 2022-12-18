#include "litefs.h"
#include "litefsapp.h"
#include <dlfcn.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>

#define MAINFEST_PATH       ("../out/manifest")
#define PATH_SIZE           (1024)
#define MAX_CHILD_PROCESSES (256)

pid_t g_pidLastChildProc = 0;
pid_t g_pidChildPids[MAX_CHILD_PROCESSES] = {0};
uint16_t g_uiPIDIndex = 0;

void (*AppExit)();

void OnExit(int iSigNum);
static void RunApp(const char* strLibPath);
static void MainProcess();

int main(int argc, char const *argv[])
{
    int iStatus = LITEFS_SUCCESS;
    struct sigaction sigHandler;
    sigHandler.sa_handler = OnExit;
    if(sigaction(SIGINT, &sigHandler, NULL))
    {
        perror("LITEFS ERROR: Cannot set sig handler");
        iStatus = LITEFS_ERROR;
        goto exit;
    }
    FILE* fileManifestPtr = fopen(MAINFEST_PATH, "r");
    if(fileManifestPtr == NULL)
    {
        perror("LITEFS ERROR: Cannot open file");
        iStatus = LITEFS_ERROR;
        goto exit;
    }
    char strLibPath[PATH_SIZE] = {0};
    uint16_t uiCounter = 0;
    while(fgets(strLibPath, sizeof(strLibPath), fileManifestPtr) != NULL)
    {
        if(g_uiPIDIndex < sizeof(g_pidChildPids)/sizeof(g_pidChildPids[0]))
        {
            size_t sLastChar = strlen(strLibPath) - 1;
            if(strLibPath[sLastChar] == '\n')
            {
                strLibPath[sLastChar] = '\0';
            }
            printf("Running: %s\n", strLibPath);
            RunApp(strLibPath);
            // Code below is MAIN PROCESS
        }
        else
        {
            printf("LITEFS ERROR: Cannot run, too many apps listed\n");
            OnExit(0);
            break;
        }
        MainProcess();
    }

exit:
    return iStatus;
}

void OnExit(int iSigNum)
{
    if(g_pidLastChildProc == 0)
    {
        AppExit();
    }
    size_t sPidSize = sizeof(g_pidChildPids)/sizeof(g_pidChildPids[0]);
    for(uint16_t i = 0; i < sPidSize && g_pidChildPids[i] > 0; i++)
    {
        kill(g_pidChildPids[i], SIGINT);
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
        g_pidLastChildProc = fork();
        if(g_pidLastChildProc > 0)
        {
            printf("LITEFS LOG: Closing handle\n");
            dlclose(handle);
            g_pidChildPids[g_uiPIDIndex] = g_pidLastChildProc;
            g_uiPIDIndex++;
        }
        else if(g_pidLastChildProc == 0)
        {
            LITEFS_API_T* appAPI = (LITEFS_API_T*) dlsym(handle, "gLiteFS_AppAPI");
            void (*apiInit)() = (void (*)()) dlsym(handle, "LiteFS_ApiInit");
            apiInit();
            appAPI->LiteFS_AppInit();
            AppExit = appAPI->LiteFS_AppExit;
            appAPI->LiteFS_AppRun();
        }
        else
        {
            perror("FORK ERROR");
        }
    }
}

static void MainProcess()
{
    while(true)
    {
        printf("Main Process\n");
        sleep(2);
    }
}
