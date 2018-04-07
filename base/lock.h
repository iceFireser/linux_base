#ifndef _LOCK_H_
#define _LOCK_H_


INT LOCK_SomeThing(IN INT iFd, IN INT iCmd, IN INT iType,
                          IN off_t lOffset, IN INT iWhence, IN off_t lLen);

#define File_ReadWaitLock(iFd, lOffset, iWhence, lLen) \
                                      LOCK_SomeThing((iFd), F_SETLKW, F_RDLCK, (lOffset), (iWhence), (lLen))
#define File_WriteWaitLock(iFd, lOffset, iWhence, lLen) \
                                      LOCK_SomeThing((iFd), F_SETLKW, F_WRLCK, (lOffset), (iWhence), (lLen))
#define File_UnLock(iFd, lOffset, iWhence, lLen) \
                                      LOCK_SomeThing((iFd), F_SETLK, F_UNLCK, (lOffset), (iWhence), (lLen))


#endif
