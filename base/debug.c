#include "head.h"

#define DEBUG_FRAME_MAX 100



VOID DEBUG_BackTrace(DEBUG_BT_ENUM enBt, ...)
{
    ULONG ulRet = ERROR_SUCCESS;

    VOID *apBuffer[DEBUG_FRAME_MAX] = {0};
    INT iPtrNum = 0;
    CHAR **ppcStrings = NULL;

    INT iFd = -1;

    if (DEBUG_BT_FILE == enBt)
    {
        va_list ap;
        va_start(ap, enBt);
        iFd = va_arg(ap, INT);
        va_end(ap);
    }


    printf("%s\n", __FUNCTION__);

    iPtrNum = backtrace(apBuffer, DEBUG_FRAME_MAX);

    switch (enBt)
    {
        case DEBUG_BT_DEFAULT:
        case DEBUG_BT_PRINT:
        {
            if (NULL == (ppcStrings = backtrace_symbols(apBuffer, iPtrNum)))
            {
                ERR_Sys("backtrace_symbols");
            }

            for (INT i = 0; i < iPtrNum; i++)
            {
                printf("%s\n", ppcStrings[i]);
            }

            free(ppcStrings);
            break;
        }
        case DEBUG_BT_DEFAULT_FILE:
        {
            mode_t mode = umask(0);
            if (-1 == (iFd = open("backtrace.txt", O_CREAT | O_APPEND | O_RDWR, 0744)))
            {
                ERR_Sys("open");
            }
            umask(mode);
        }
        case DEBUG_BT_FILE:
        {

            backtrace_symbols_fd(apBuffer, iPtrNum, iFd);
            break;

        }
        default:
        {
            ERR_Hint("DEBUG_BackTrace parameter error");
        }
    }

    sync();

    return;
}

