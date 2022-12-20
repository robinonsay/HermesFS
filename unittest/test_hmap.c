#include "utils/hermes_hmap.h"

#include <stdio.h>
#include <stdbool.h>

static bool TestHMapSetAndGet()
{
    bool bPass = true;
    HERMES_HMAP_T hmap = {0};
    HERMES_HMAP_ITEM_T itemTbl[4] = {0};
    Hermes_HmapInit(&hmap, itemTbl, sizeof(itemTbl));
    int values[4] = {0};
    values[0] = 69420;
    values[1] = 666;
    values[2] = 6969;
    values[3] = 21;
    char keyStr[4][1024] = {
        "Hello World! I'm key0.",
        "Hello World! I'm key1.",
        "Hello World! I'm key2.",
        "Hello World! I'm key3."
    };
    for(int i = 0; i < 4; i++)
    {
        Hermes_HmapSet(&hmap, keyStr[i], sizeof(keyStr[i]), &values[i]);
        int* iValPtr = (int*) Hermes_HmapGet(&hmap, keyStr, sizeof(keyStr[i]));
        bPass = bPass && (values[0] == *iValPtr);
    }
    if(!bPass)
    {
        goto exit;
    }
exit:
    return bPass;
}

int main(int argc, char const *argv[])
{
    int iStatus = 0;
    iStatus = !TestHMapSetAndGet();
    printf("Test HMAP Set and Get: %i\n", iStatus);
    return iStatus;
}

