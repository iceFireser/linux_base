#ifndef _LIST_H_
#define _LIST_H_

/* single list */

typedef struct tagSINGLE_NODE
{
    struct tagSINGLE_NODE *pstNext;

}SINGLE_NODE_S;

typedef struct tagSINGLE_HEAD
{
    SINGLE_NODE_S *pstFirst;
    SINGLE_NODE_S *pstLast;
}SINGLE_HEAD_S;

typedef BOOL_T (*PF_SINGLE_LIST_SEARCH)(IN SINGLE_NODE_S *pstNode, IN VOID *pData);

VOID SingleListInit(IN SINGLE_HEAD_S *pstHead);
VOID SingleListAdd(IN SINGLE_HEAD_S *pstHead, IN SINGLE_NODE_S *pstNode);
VOID SingleListDel(IN SINGLE_HEAD_S *pstHead, IN SINGLE_NODE_S *pstNode);
SINGLE_NODE_S *SingleListSearch(IN SINGLE_HEAD_S *pstHead, IN PF_SINGLE_LIST_SEARCH pfSearch);
VOID SingleListFreeAll(IN SINGLE_HEAD_S *pstHead);



#define SINGLE_LIST_FOR_EACH(pstHead, pstNode) \
for (pstNode = pstHead->pstFirst; \
     pstNode != NULL; \
     pstNode = pstNode->pstNext)

#define SINGLE_LIST_FOR_EACH_PRE(pstHead, pstNode, pstPreNode) \
for (pstNode = pstHead->pstFirst, pstPreNode = NULL; \
     pstNode != NULL; \
     pstPreNode = pstNode, pstNode = pstNode->pstNext)

#define SINGLE_LIST_SAFE_FOR_EACH(pstHead, pstNode) \
for (SINGLE_NODE_S *pstNextNode = NULL, pstNode = pstHead->pstFirst; \
     ((pstNode != NULL) && (pstNextNode = pstNode->pstNext, 1)); \
     pstNode = pstNextNode)

/* double list */

#endif


