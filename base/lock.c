#include "head.h"


INT LOCK_SomeThing(IN INT iFd, IN INT iCmd, IN INT iType,
                          IN off_t lOffset, IN INT iWhence, IN off_t lLen)
{

    struct flock stLock;

    stLock.l_type = iType; /* 锁类型 读锁 写锁 解锁 F_RDLCK, F_WRLCK, F_UNLCK */

    /*  l_whence为下列其中一种:（SEEK_SET,SEEK_CUR和SEEK_END和依次为0，1和2）.
        SEEK_SET 将读写位置指向文件头后再增加offset个位移量。
        SEEK_CUR 以目前的读写位置往后增加offset个位移量。
        SEEK_END 将读写位置指向文件尾后再增加offset个位移量。
        当whence 值为SEEK_CUR 或SEEK_END时，参数offet允许负值的出现。 */
    stLock.l_whence = iWhence;
    stLock.l_start = lOffset;  /* 要锁的首字节在文件中的偏移 */
    stLock.l_len = lLen; /* 要锁住的字节长度 */

    INT iRet = ERROR_SUCCESS;

    iRet = fcntl(iFd, iCmd, &stLock);

    if (iRet < 0)
    {
        ERR_Sys("LOCK_SomeThing: error");
    }

    return iRet;
}


