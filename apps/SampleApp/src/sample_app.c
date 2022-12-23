#include "hermes_app.h"
#include "hermes_sbn.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

static char gstrAppName[] = "SampleApp";

static int Hermes_SampleAppInit();
static int Hermes_SampleAppRun();
static void Hermes_SampleAppExit();

typedef struct APP_DATA_TAG
{
    HERMES_SBN_T sbn;
} APP_DATA_T;

APP_DATA_T gAppData;

void Hermes_RegisterApp(HERMES_API_T* apiPtr)
{
    apiPtr->strAppName = gstrAppName;
    apiPtr->Hermes_AppInit = Hermes_SampleAppInit;
    apiPtr->Hermes_AppRun = Hermes_SampleAppRun;
    apiPtr->Hermes_AppExit = Hermes_SampleAppExit;
}

static int Hermes_SampleAppInit()
{
    int iStatus = Hermes_SbnInit(&gAppData.sbn, gstrAppName);
    return iStatus;
}

static int Hermes_SampleAppRun()
{
    unsigned long ulCounter = 0;
    char strData[256] = {0};
    while(true)
    {
        sprintf(strData, "Sample App Says Hello %lu", ulCounter);
        Hermes_SbnPublish(&gAppData.sbn, strData, strlen(strData));
        ulCounter++;
        sleep(1);
    }
    return 0;
}

static void Hermes_SampleAppExit()
{
    printf("\nSampleApp Exit\n");
}
