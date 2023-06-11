/* 
 * File:   GroupSqrt.h
 * Author: fslib
 *
 * Created on 2021年05月19日, 下午5:03
 */
#if !defined GROUPSQRT_H && !defined JAVA_IN_C

#define GROUPSQRT_H
#include "../../PublicTools/publicDefine.h"
#include <math.h>


#ifdef __cplusplus
extern "C" {
#endif

#pragma pack(push,1)

#pragma pack(pop)

    /* 开方分组 */
    typedef struct {
        /* 开方分组的每组的成员数 */
        unsigned int groupSqrt_member;
        /* 开方分组的组数 */
        unsigned int groupSqrt_group;
        /* 储存开方分组组的值的数组 */
        unsigned char *groupValue;
    } FsGroupSqrt;

    /* 开方分组 */
    typedef struct {
        /* 开方分组的每组的成员数 */
        unsigned int groupSqrt_member;
        /* 开方分组的组数 */
        unsigned int groupSqrt_group;
        /* 储存开方分组组的值的数组 */
        unsigned char *groupValue;
        /* 开方分组用的互斥锁 */
        pthread_mutex_t groupSqrt_mutex;
    } FsGroupSqrtPthreadSafety;


#define Fs_GroupSqrt_init(___pGroupSqrt) do{\
    /* 开方分组的每组的成员数 */\
    (___pGroupSqrt)->groupSqrt_member=1;\
    /* 开方分组的组数 */\
    (___pGroupSqrt)->groupSqrt_group = 1;\
        /* 储存开方公组组的值的数组 */\
    (___pGroupSqrt)->groupValue = (unsigned char *) fsMalloc(1);\
}while (0)
#define Fs_GroupSqrt_release(___pGroupSqrt) do{\
    /* 储存开方公组组的值的数组 */\
    fsFree((___pGroupSqrt)->groupValue);\
}while (0)
#define Fs_GroupSqrtPthreadSafety_init(___pGroupSqrt) do{\
    Fs_GroupSqrt_init(___pGroupSqrt);\
    /* 开方分组用的互斥锁 */\
    pthread_mutex_init(&(___pGroupSqrt)->groupSqrt_mutex, NULL);\
}while (0)
#define Fs_GroupSqrtPthreadSafety_release(___pGroupSqrt) do{\
    Fs_GroupSqrt_release(___pGroupSqrt);\
        /* 开方分组用的互斥锁 */\
    pthread_mutex_destroy(&(___pGroupSqrt)->groupSqrt_mutex);\
}while (0)
    /* 计算分组数量,及每组的成员数 */
#define Fs_GroupSqrt_cal(/* 开方分组的每组的成员数 *//* unsigned int */ ___rst_groupSqrt_member,/* 开方分组的组数 *//* unsigned int */ ___rst_groupSqrt_group,/* 通道数 */___channelCount) do{\
    (___rst_groupSqrt_group)=sqrt((___channelCount)+1);\
    (___rst_groupSqrt_member)=((___channelCount)+(___rst_groupSqrt_group)-1)/(___rst_groupSqrt_group);\
    if(0==(___rst_groupSqrt_member))(___rst_groupSqrt_member)=1;\
}while(0)
    /* 使用___channelCount重置___pGroupSqrt的参数 */
#define Fs_GroupSqrt_resize(___pGroupSqrt,/* 通道数 */___channelCount) do{\
   Fs_GroupSqrt_cal((___pGroupSqrt)->groupSqrt_member,(___pGroupSqrt)->groupSqrt_group,___channelCount);\
   (___pGroupSqrt)->groupValue = (unsigned char *) fsRealloc((___pGroupSqrt)->groupValue, (___pGroupSqrt)->groupSqrt_group);\
}while(0)
    /* 设置___pGroupSqrtOut的指针 */
#define Fs_GroupSqrtOut_set(___pGroupSqrtOut,/* 开方分组的每组的成员数 *//* unsigned int */ ___rst_groupSqrt_member,/* 开方分组的组数 *//* unsigned int */ ___rst_groupSqrt_group\
,/* 储存开方公组组的值的数组 */ ___groupValue,___index) do {\
    const unsigned int theIndex=((___index)/(___rst_groupSqrt_member));\
    (___pGroupSqrtOut)=(___groupValue)+(theIndex<(___rst_groupSqrt_group)?theIndex:0);\
}while(0)
    /* 设置___pGroupSqrtOut的指针 */
#define Fs_GroupSqrtOut_set_pthreadSafety(___pGroupSqrtOut,/* 开方分组的每组的成员数 *//* unsigned int */ ___rst_groupSqrt_member,/* 开方分组的组数 *//* unsigned int */ ___rst_groupSqrt_group\
,/* 储存开方公组组的值的数组 */ ___groupValue,___index, /* 开方分组用的互斥锁 */ ___pGroupSqrt_mutex) do {\
    pthread_mutex_lock(___pGroupSqrt_mutex);\
    Fs_GroupSqrtOut_set(___pGroupSqrtOut,___rst_groupSqrt_member,___rst_groupSqrt_group,___groupValue,___index);\
    pthread_mutex_unlock(___pGroupSqrt_mutex);\
}while(0)
    /* 设置*(___pGroupSqrtOut)的值 */
#define Fs_GroupSqrtOut_value_set(/* 有数据输出时要写的unsigned char* */ ___pGroupSqrtOut, /* 开方分组用的互斥锁 */ ___pGroupSqrt_mutex) do {\
    pthread_mutex_lock(___pGroupSqrt_mutex);\
    *(___pGroupSqrtOut)=1;\
    pthread_mutex_unlock(___pGroupSqrt_mutex);\
}while(0)



#ifdef __cplusplus
}
#endif

#endif /* GROUPSQRT_H */

