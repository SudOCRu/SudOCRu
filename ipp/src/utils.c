#include "utils.h"

const char IND_LOAD[] = "..";
const char IND_OK[] = "\033[32;1mOK\033[0m";

void PrintStage(unsigned char id, unsigned char total, char* stage, int ok)
{
    const char* indicator = ok ? IND_OK : IND_LOAD;
    if (ok)
    {
        printf("\r%hhu/%hhu [%s] %s\n", id, total, indicator, stage);
    }
    else
    {
        printf("%hhu/%hhu [%s] %s", id, total, indicator, stage);
        fflush(stdout);
    }
}
