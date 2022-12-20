#define HERMES_HMAP_SET_POLY8
#include "hermes_hmap.h"
#undef HERMES_HMAP_SET_POLY8

#include <stdio.h>
#include <string.h>

#define BIT_SIZE_32 (32)

void Hermes_HmapInit(HERMES_HMAP_T* hmapPtr, HERMES_HMAP_ITEM_T itemTbl[], unsigned long ulItemTblSize)
{
    memset(itemTbl, 0, ulItemTblSize);
    hmapPtr->itemTbl = itemTbl;
    hmapPtr->ulItemTblSize = ulItemTblSize;
    hmapPtr->ulItemTblLen = ulItemTblSize / sizeof(HERMES_HMAP_ITEM_T);
    hmapPtr->state.ulLen = 0;
    hmapPtr->state.ulSize = 0;
}

void* Hermes_HmapGet(HERMES_HMAP_T* hmapPtr, void* keyPtr, unsigned long ulKeySize)
{
    void* valuePtr = NULL;
    unsigned long ulHash = 0;
    if(ulKeySize > BIT_SIZE_32)
    {
        ulHash = Hermes_HashArr(keyPtr, ulKeySize);
    }
    else
    {
        memcpy(&ulHash, keyPtr, ulKeySize);
    }
    unsigned long counter = 0;
    HERMES_HMAP_ITEM_T* currItemPtr = &hmapPtr->itemTbl[ulHash % hmapPtr->ulItemTblLen];
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

int Hermes_HmapSet(HERMES_HMAP_T* hmapPtr, void* keyPtr, unsigned long ulKeySize, void* valPtr)
{
    int iStatus = HERMES_HMAP_SUCCESS;
    if(hmapPtr->state.ulLen >= hmapPtr->ulItemTblLen)
    {
        iStatus = HERMES_HMAP_ERROR;
        goto exit;
    }
    unsigned long ulHash = 0;
    if(ulKeySize > BIT_SIZE_32)
    {
        ulHash = Hermes_HashArr(keyPtr, ulKeySize);
    }
    else
    {
        memcpy(&ulHash, keyPtr, ulKeySize);
    }
    unsigned long counter = 0;
    unsigned long ulTableIndex = ulHash % hmapPtr->ulItemTblLen;
    HERMES_HMAP_ITEM_T* currItemPtr = &hmapPtr->itemTbl[ulTableIndex];
    while(currItemPtr->nextItemPtr != NULL)
    {
        if(counter >= hmapPtr->ulItemTblLen)
        {
            iStatus = HERMES_HMAP_ERROR;
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
    ulTableIndex = (currItemPtr - hmapPtr->itemTbl) / sizeof(HERMES_HMAP_ITEM_T);
    while(hmapPtr->itemTbl[ulTableIndex].vValuePtr != NULL)
    {
        if(counter >= hmapPtr->ulItemTblLen)
        {
            iStatus = HERMES_HMAP_ERROR;
            break;
        }
        ulTableIndex = (ulTableIndex + 1) % hmapPtr->ulItemTblLen;
        counter++;
    }
    if(iStatus)
    {
        goto exit;
    }
    if (currItemPtr == &hmapPtr->itemTbl[ulTableIndex])
    {
        currItemPtr->ulKeyHash = ulHash;
        currItemPtr->vValuePtr = valPtr;
        currItemPtr->nextItemPtr = NULL;
        currItemPtr->prevItemPtr = NULL;
    }
    else
    {
        hmapPtr->itemTbl[ulTableIndex].ulKeyHash = ulHash;
        hmapPtr->itemTbl[ulTableIndex].vValuePtr = valPtr;
        hmapPtr->itemTbl[ulTableIndex].nextItemPtr = NULL;
        hmapPtr->itemTbl[ulTableIndex].prevItemPtr = currItemPtr;
        currItemPtr->nextItemPtr = &hmapPtr->itemTbl[ulTableIndex];
    }
    hmapPtr->state.ulLen++;
    hmapPtr->state.ulSize += sizeof(HERMES_HMAP_ITEM_T);
exit:
    return iStatus;
}

void Hermes_HmapRemove(HERMES_HMAP_T* hmapPtr, void* keyPtr, unsigned long ulKeySize)
{
    HERMES_HMAP_ITEM_T* itemPtr = Hermes_HmapGet(hmapPtr, keyPtr, ulKeySize);
    HERMES_HMAP_ITEM_T* prevItemPtr = itemPtr->prevItemPtr;
    HERMES_HMAP_ITEM_T* nextItemPtr = itemPtr->nextItemPtr;
    itemPtr->ulKeyHash = 0;
    itemPtr->vValuePtr = NULL;
    itemPtr->nextItemPtr = NULL;
    itemPtr->prevItemPtr = NULL;
    if(prevItemPtr != NULL)
    {
        prevItemPtr->nextItemPtr = nextItemPtr;
    }
    if(nextItemPtr != NULL)
    {
        nextItemPtr->prevItemPtr = prevItemPtr;
    }
}

/* https://wiki.osdev.org/CRC32 */
unsigned long Hermes_HashStr(char str[])
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

unsigned long Hermes_HashArr(unsigned char byteArr[], unsigned long ulByteArrSize)
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
