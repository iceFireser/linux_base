#ifndef _DEBUG_H_
#define _DEBUG_H_

typedef enum DEBUG_BT_tag
{
    DEBUG_BT_DEFAULT       = 0x00000000, /* just print backtrace */
    DEBUG_BT_PRINT         = 0x00000001, /* same to DEBUG_BT_DEFAULT */
    DEBUG_BT_DEFAULT_FILE  = 0x00000002, /* record to default file */
    DEBUG_BT_FILE          = 0x00000003, /* record to appoint file, not support */
    DEBUG_BT_MAX
}DEBUG_BT_ENUM;

VOID DEBUG_BackTrace(DEBUG_BT_ENUM enBt, ...);

/* 示例用法 */
/* DEBUG_BackTrace(0); */

#endif