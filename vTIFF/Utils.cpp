#include"Utils.h"



int GetIntII(byte* pd, int startPos, int Length)
{
    int value = 0;
    for (int i = 0; i < Length; i++)
        value |= pd[startPos + i] << i * 8;
    return value;
}

int GetIntMM(byte* pd, int startPos, int Length)
{
    int value = 0;
    int st = startPos + Length - 1;
    for (int i = 0; i < Length; i++)
        value |= pd[st - i] << i * 8;
    return value;
}
