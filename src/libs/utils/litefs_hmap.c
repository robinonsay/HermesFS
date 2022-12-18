#define LITEFS_HMAP_SET_POLY8
#include "litefs_hmap.h"
#undef LITEFS_HMAP_SET_POLY8

#include <stdio.h>
#include <string.h>

#define BIT_SIZE_32 (32)

void LiteFS_HmapInit(LITEFS_HMAP_T* hmapPtr, LITEFS_HMAP_ITEM_T itemTbl[], unsigned long ulItemTblSize)
{
    memset(itemTbl, 0, ulItemTblSize);
    hmapPtr->itemTbl = itemTbl;
    hmapPtr->ulItemTblSize = ulItemTblSize;
    hmapPtr->ulItemTblLen = ulItemTblSize / sizeof(LITEFS_HMAP_ITEM_T);
    hmapPtr->state.ulLen = 0;
    hmapPtr->state.ulSize = 0;
}

void* LiteFS_HmapGet(LITEFS_HMAP_T* hmapPtr, void* keyPtr, unsigned long ulKeySize)
{
    void* valuePtr = NULL;
    unsigned long ulHash = 0;
    if(ulKeySize > BIT_SIZE_32)
    {
        ulHash = LiteFS_HashArr(keyPtr, ulKeySize);
    }
    else
    {
        memcpy(&ulHash, keyPtr, ulKeySize);
    }
    unsigned long counter = 0;
    LITEFS_HMAP_ITEM_T* currItemPtr = &hmapPtr->itemTbl[ulHash % hmapPtr->ulItemTblLen];
    while(currItemPtr->nextItemPtr != NULL && currItemPtr->ulKeyHash != ulHash)
    {
        if(counter >= hmapPtr->ulItemTblLen)
        {
            break;
        }
        currItemPtr = currItemPtr->nextItemPtr;
        counter++;
    }
    if(currItemPtr->ulKeyHash == ulHash)
    {
        valuePtr = currItemPtr->vValuePtr;
    }
    return valuePtr;
}

int LiteFS_HmapSet(LITEFS_HMAP_T* hmapPtr, void* keyPtr, unsigned long ulKeySize, void* valPtr)
{
    int iStatus = LITEFS_HMAP_SUCCESS;
    if(hmapPtr->state.ulLen >= hmapPtr->ulItemTblLen)
    {
        iStatus = LITEFS_HMAP_ERROR;
        goto exit;
    }
    unsigned long ulHash = 0;
    if(ulKeySize > BIT_SIZE_32)
    {
        ulHash = LiteFS_HashArr(keyPtr, ulKeySize);
    }
    else
    {
        memcpy(&ulHash, keyPtr, ulKeySize);
    }
    unsigned long counter = 0;
    unsigned long ulTableIndex = ulHash % hmapPtr->ulItemTblLen;
    LITEFS_HMAP_ITEM_T* currItemPtr = &hmapPtr->itemTbl[ulTableIndex];
    while(currItemPtr->nextItemPtr != NULL)
    {
        if(counter >= hmapPtr->ulItemTblLen)
        {
            iStatus = LITEFS_HMAP_ERROR;
            break;
        }
        currItemPtr = currItemPtr->nextItemPtr;
        counter++;
    }
    if(iStatus)
    {
        goto exit;
    }
    counter = 0;
    ulTableIndex = (currItemPtr - hmapPtr->itemTbl) / sizeof(LITEFS_HMAP_ITEM_T);
    while(hmapPtr->itemTbl[ulTableIndex].vValuePtr != NULL)
    {
        if(counter >= hmapPtr->ulItemTblLen)
        {
            iStatus = LITEFS_HMAP_ERROR;
            break;
        }
        ulTableIndex = (ulTableIndex + 1) % hmapPtr->ulItemTblLen;
        counter++;
    }
    if(iStatus)
    {
        goto exit;
    }
    currItemPtr->nextItemPtr = &hmapPtr->itemTbl[ulTableIndex];
    currItemPtr->nextItemPtr->ulKeyHash = ulHash;
    currItemPtr->nextItemPtr->vValuePtr = valPtr;
    currItemPtr->nextItemPtr->nextItemPtr = NULL;
    hmapPtr->state.ulLen++;
    hmapPtr->state.ulSize += sizeof(LITEFS_HMAP_ITEM_T);
exit:
    return iStatus;
}

/* https://wiki.osdev.org/CRC32 */
unsigned long LiteFS_HashStr(char str[])
{
    unsigned long ulHash = 0xFFFFFFFF;
    unsigned short index = 0;
    for(unsigned short i = 0; str[i] != '\0'; i++)
    {
        index = (ulHash ^ str[i]) & 0xFF;
        ulHash = g_uiPoly8Lookup[index] ^ (ulHash >> 8);

    }
    ulHash ^= 0xFFFFFFFF;
    return ulHash;
}

unsigned long LiteFS_HashArr(unsigned char byteArr[], unsigned long ulByteArrSize)
{
    unsigned long ulHash = 0xFFFFFFFF;
    unsigned short index = 0;
    for(unsigned short i = 0; i < ulByteArrSize; i++)
    {
        index = (ulHash ^ byteArr[i]) & 0xFF;
        ulHash = g_uiPoly8Lookup[index] ^ (ulHash >> 8);

    }
    ulHash ^= 0xFFFFFFFF;
    return ulHash;
}
