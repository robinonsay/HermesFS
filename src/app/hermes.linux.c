#include "hermes.h"
#include "hermes_app.h"
#include <dlfcn.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>

#define MAINFEST_PATH       ("../out/manifest")
#define PATH_SIZE           (1024)
#define MAX_CHILD_PROCESSES (256)

HERMES_API_T gAppApi = {0};
pid_t gPidLastChildProc = 0;
pid_t gPidChildPids[MAX_CHILD_PROCESSES] = {0};
uint16_t guiPIDIndex = 0;

void OnExit(int iSigNum);
static void RunApp(const char* strLibPath);
static void MainProcess();

int main(int argc, char const *argv[])
{
    int iStatus = HERMES_SUCCESS;
    struct sigaction sigHandler;
    sigHandler.sa_handler = OnExit;
    if(sigaction(SIGINT, &sigHandler, NULL))
    {
        perror("HERMES ERROR: Cannot set sig handler");
        iStatus = HERMES_ERROR;
        goto exit;
    }
    FILE* fileManifestPtr = fopen(MAINFEST_PATH, "r");
    if(fileManifestPtr == NULL)
    {
        perror("HERMES ERROR: Cannot open file");
        iStatus = HERMES_ERROR;
        goto exit;
    }
    char strLibPath[PATH_SIZE] = {0};
    uint16_t uiCounter = 0;
    while(fgets(strLibPath, sizeof(strLibPath), fileManifestPtr) != NULL)
    {
        if(guiPIDIndex < sizeof(gPidChildPids)/sizeof(gPidChildPids[0]))
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
            printf("HERMES ERROR: Cannot run, too many apps listed\n");
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
    if(gPidLastChildProc == 0)
    {
        printf("\nClosing %s\n", gAppApi.strAppName);
        gAppApi.Hermes_AppExit();
    }
    size_t sPidSize = sizeof(gPidChildPids)/sizeof(gPidChildPids[0]);
    for(uint16_t i = 0; i < sPidSize && gPidChildPids[i] > 0; i++)
    {
        kill(gPidChildPids[i], SIGINT);
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
        gPidLastChildProc = fork();
        if(gPidLastChildProc > 0)
        {
            printf("HERMES LOG: Closing handle\n");
            dlclose(handle);
            gPidChildPids[guiPIDIndex] = gPidLastChildProc;
            guiPIDIndex++;
            printf("App PID: [%i]\n", gPidLastChildProc);
        }
        else if(gPidLastChildProc == 0)
        {
            void (*AppApiInit)(HERMES_API_T* apiPtr) = (void (*)(HERMES_API_T*)) dlsym(handle, "Hermes_RegisterApp");
            AppApiInit(&gAppApi);
            printf("Starting %s\n", gAppApi.strAppName);
            gAppApi.Hermes_AppInit();
            gAppApi.Hermes_AppRun();
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
