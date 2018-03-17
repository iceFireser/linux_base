#ifndef _HMDB_H_
#define _HMDB_H_

typedef VOID * HMDB_HANDLE;

/*
 * Default file access permissions for new files.
 */
#define FILE_MODE   (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)

/*
 * Default permissions for new directories.
 */
#define DIR_MODE    (FILE_MODE | S_IXUSR | S_IXGRP | S_IXOTH)


#define INDEXLEN_MIN 6 /* 键 分隔符 真实数据的开始偏移 分隔符 真实数据的长度 '\n' */
#define INDEXLEN_MAX 1024
#define DATALEN_MIN 2 /* 数据 '\n' */
#define DATALEN_MAX 1024

HMDB_HANDLE HMDB_Open(IN const CHAR *pcPathName, IN INT iFlag, ...);
VOID HMDB_Close(IN HMDB_HANDLE pHandle);
CHAR *HMDB_Fetch(IN HMDB_HANDLE pHandle, IN const CHAR *pcKey);
INT HMDB_Delete(IN HMDB_HANDLE handle, IN const CHAR *pcKey);
INT HMDB_Store(IN HMDB_HANDLE handle, IN const CHAR *pcKey, IN const CHAR *pcData, IN INT iFlag);
VOID HMDB_Rewind(IN HMDB_HANDLE handle);
CHAR *HMDB_NextRec(IN HMDB_HANDLE handle, IN CHAR *pcKey, IN ULONG ulLen);

#define HMDB_INSERT     1
#define HMDB_REPLACE    2
#define HMDB_STORE      3

/* 示例用法 */
/*

    HMDB_HANDLE    handle = 0;

    if ((handle = HMDB_Open("db4", O_RDWR | O_CREAT | O_TRUNC,
      FILE_MODE)) == NULL)
        ERR_Sys("db_open error");

    if (HMDB_Store(handle, "Alpha", "data1", HMDB_INSERT) != 0)
        ERR_Sys("HMDB_Store error for alpha");
    if (HMDB_Store(handle, "beta", "Data for beta", HMDB_INSERT) != 0)
        ERR_Sys("HMDB_Store error for beta");
    if (HMDB_Store(handle, "gamma", "record3", HMDB_INSERT) != 0)
        ERR_Sys("HMDB_Store error for gamma");

    HMDB_Close(handle);


*/


#endif

