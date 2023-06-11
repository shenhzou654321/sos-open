
#include <stdio.h>
#include <pthread.h>

#define Fs_CycleList_init(cycleList) \
    /* 主节点 */\
    (cycleList).node.privious=(cycleList).node.next=&(cycleList).node;\
    /* 节点数量 */\
    (cycleList).nodeCount = 0
/* 输出cycleList的所有节点到fd中 */
#define Fs_CycleList_out(cycleList,fd) {\
    unsigned int ii=0;\
    struct FsCycleListNode *pNode;\
    fprintf(fd,"cycleList.nodeCount=%lu,%#lx/%#lx/%#lx\n",(cycleList).nodeCount,&(cycleList).node,(cycleList).node.privious,(cycleList).node.next);\
    for (pNode =Fs_CycleList_get_head((cycleList)); ii+1 < Fs_CycleList_get_count((cycleList));ii++,pNode =Fs_CycleList_node_get_next(pNode)) {\
        fprintf(fd,"[%u]:%#lx/%#lx/%#lx,",ii,pNode,pNode->privious,pNode->next);\
    }\
    if(Fs_CycleList_get_count((cycleList))>0){\
        fprintf(fd,"[%u]:%#lx/%#lx/%#lx.\n",ii,pNode,pNode->privious,pNode->next);\
    }\
}
/* 在pNodeAfter之后插入pNode */
#define Fs_CycleList_insert_after(cycleList,pNode,pNodeAfter) do{\
    /* FsPrintf(1,"_Fs_CycleList_insert_after,pNode=%#lx,pNodeAfter=%#lx\n",pNode,pNodeAfter);Fs_CycleList_out((cycleList),stdout); */\
    (cycleList).nodeCount++;\
    ((struct FsCycleListNode*)(pNode))->privious=(struct FsCycleListNode*)(pNodeAfter);\
    ((struct FsCycleListNode*)(pNode))->next=((struct FsCycleListNode*)(pNodeAfter))->next;\
    ((struct FsCycleListNode*)(pNodeAfter))->next->privious=(struct FsCycleListNode*)(pNode);\
    ((struct FsCycleListNode*)(pNodeAfter))->next=(struct FsCycleListNode*)(pNode);\
}while(0)
/* 在pNodeBefore之前插入pNode */
#define Fs_CycleList_insert_before(cycleList,pNode,pNodeBefore) do{\
    /* FsPrintf(1,"_Fs_CycleList_insert_before,pNode=%#lx,pNodeBefore=%#lx\n",pNode,pNodeBefore);Fs_CycleList_out((cycleList),stdout); */\
    (cycleList).nodeCount++;\
    ((struct FsCycleListNode*)(pNode))->privious=((struct FsCycleListNode*)(pNodeBefore))->privious;\
    ((struct FsCycleListNode*)(pNode))->next=(struct FsCycleListNode*)(pNodeBefore);\
    ((struct FsCycleListNode*)(pNodeBefore))->privious->next=(struct FsCycleListNode*)(pNode);\
    ((struct FsCycleListNode*)(pNodeBefore))->privious=(struct FsCycleListNode*)(pNode);\
}while(0)

/* 头部插入一节点 */
#define Fs_CycleList_insert_head(cycleList,pNode) do{\
    /* FsPrintf(1,"_Fs_CycleList_insert_head,pNode=%#lx\n",pNode);Fs_CycleList_out((cycleList),stdout);*/\
    (cycleList).nodeCount++;\
    ((struct FsCycleListNode*)(pNode))->privious=&(cycleList).node;\
    ((struct FsCycleListNode*)(pNode))->next=(cycleList).node.next;\
    (cycleList).node.next->privious=(struct FsCycleListNode*)(pNode);\
    (cycleList).node.next=(struct FsCycleListNode*)(pNode);\
}while(0)
/* 尾部插入一节点 */
#define Fs_CycleList_insert_tail(cycleList,pNode) do{\
    /* FsPrintf(1,"_Fs_CycleList_insert_tail,pNode=%#lx\n",pNode);Fs_CycleList_out((cycleList),stdout); */\
    (cycleList).nodeCount++;\
    ((struct FsCycleListNode*)(pNode))->privious=(cycleList).node.privious;\
    ((struct FsCycleListNode*)(pNode))->next=&(cycleList).node;\
    (cycleList).node.privious->next=(struct FsCycleListNode*)(pNode);\
    (cycleList).node.privious=(struct FsCycleListNode*)(pNode);\
}while(0)
/* 移除一节点 */
#define Fs_CycleList_remove(cycleList,pNode) do{\
    /* FsPrintf(1,"_Fs_CycleList_remove,pNode=%#lx\n",pNode);Fs_CycleList_out((cycleList),stdout); */\
    (cycleList).nodeCount--;\
    ((struct FsCycleListNode*)(pNode))->next->privious = ((struct FsCycleListNode*)(pNode))->privious;\
    ((struct FsCycleListNode*)(pNode))->privious->next = ((struct FsCycleListNode*)(pNode))->next;\
}while(0)
/* 线程安全地移除一节点 */
#define Fs_CycleList_remove_pthreadSafety(cycleList,pNode,mutex) do{\
    /* FsPrintf(1,"_Fs_CycleList_remove,pNode=%#lx\n",pNode);Fs_CycleList_out((cycleList),stdout); */\
    pthread_mutex_lock(&(mutex));\
    Fs_CycleList_remove(cycleList,pNode);\
    pthread_mutex_unlock(&(mutex));\
}while(0)
/* 移除头节点 */
#define Fs_CycleList_remove_head(cycleList) (cycleList).node.next;/* FsPrintf(1,"_Fs_CycleList_remove_head\n");Fs_CycleList_out((cycleList),stdout);*/(cycleList).nodeCount--;(cycleList).node.next->next->privious= &(cycleList).node;(cycleList).node.next= (cycleList).node.next->next
/* 移除尾节点 */
#define Fs_CycleList_remove_tail(cycleList) (cycleList).node.privious;/* FsPrintf(1,"_Fs_CycleList_remove_tail\n");Fs_CycleList_out((cycleList),stdout);*/(cycleList).nodeCount--;(cycleList).privious->privious->next= &(cycleList).node;(cycleList).node.privious= (cycleList).node.privious->privious 
