#include "usernumtotype.h"
#include <stdio.h>

userNumToType::userNumToType()
{
    unsigned int i;
    for (i=0;i<sizeof(numBuff)/sizeof(int);i++)
    {
        numBuff[i]=0x00;
    }
}
