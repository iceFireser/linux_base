#ifndef _STRING_EX_H_
#define _STRING_EX_H_

/* 安全拼装函数 返回值为真正拼装到缓冲区的字节数，不包括'\0', 当拼装满缓冲区时，
 * 返回为带'\0'的字节数 */
size_t vscnprintf(IN CHAR *pcBuf, IN size_t ulLen, IN const CHAR *pcFmt, IN va_list pAp);
size_t scnprintf(IN CHAR *pcBuf, IN size_t ulLen, IN const CHAR *pcFmt, ...);

#define STRING_APPEND_INIT size_t __ulLen = 0
#define STRING_APPEND_AGAIN __ulLen = 0
#define STRING_APPENDING(pcBuf, ulLen, pcFmt, ...) \
    __ulLen += scnprintf(pcBuf + __ulLen, ulLen - __ulLen, pcFmt, __VA_ARGS__)
#define STRING_V_APPENDING(pcBuf, ulLen, pcFmt, pAp) \
    __ulLen += vscnprintf(pcBuf + __ulLen, ulLen - __ulLen, pcFmt, pAp)

/* 安全拷贝 最多ulLen个字节到pcDest 包括'\0'，一定会在末尾补'\0' */
CHAR *strlcpy(IN CHAR *pcDest, IN const CHAR *pcSrc, IN size_t ulLen);

/* 安全追加 最多ulLen个字节到pcDest 包括'\0'，一定会在末尾补'\0' */
CHAR *strlcat(IN CHAR *pcDest, IN const CHAR *pcSrc, IN size_t ulLen);


#endif
