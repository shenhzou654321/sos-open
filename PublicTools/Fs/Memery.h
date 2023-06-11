/* 
 * File:   Memery.h
 * Author: fslib
 *
 * Created on 2013年8月5日, 上午11:34
 */

#ifndef MEMERY_H
#define MEMERY_H
#include "../../PublicTools/publicDefine.h"
#include "../../PublicTools/Fs/ObjectList.h"
#include "../../PublicTools/Fs/StructList.h"
#ifdef __cplusplus
extern "C" {
#endif
    /* 共享buffer,变量只允许存在于局部变量之中,传递只能使用函数的参数形式传递,不能定义为成员变量
     * 结构为头+每级堆栈数据
     *     头(非调试模式):[unsigned int]整个buffer的长度+[unsigned int]当前堆栈可使用空间的偏移量
     *     头(调试模式):[unsigned int]整个buffer长度+[unsigned int]当前堆栈可使用空间的偏移量+[FsObjectList*]记录初始化的链表+[FsObjectList*]记录调用的链表+[unsigned int]尾部关联的变量个数
     *     每级堆栈数据:支持两种模式(同一函数中两种方式不能混用)
     *         一个固定内存申请,如Fs_ShareBuffer_simple_malloc,在申请的空间需要被使用时,共享buffer不能传给子函数使用(即传给子函数时保证本函数不使用共享buffer)
     *             数据结构类似:[数据还在本函数中使用]头+堆栈1+堆栈2+堆栈3+...+堆栈(n-1_+本函数n中使用的固定内存
     *                         [数据不在本函数中使用]头+堆栈1+堆栈2+堆栈3+...+堆栈(n-1)+堆栈(n+1)+...
     *         堆栈方式申请:可把本堆栈之后的共享buffer传给子函数使用,调用子函数前需要调用FsLocal_ShareBuffer_call_before宏,子函数执行完后需要调用FsLocal_ShareBuffer_call_after宏
     *             数据结构类似:头+堆栈1+堆栈2+堆栈3+...+堆栈(n-1)+堆栈n(本函数为n)+堆栈(n+1)+...
     *             在作为堆栈方式使用时,要定义FsFunctionName宏,如定义FsFunctionName test
     *             则必须定义(应放在FsLocal_ShareBuffer_init之后)
     *                 Fs_ShareBuffer_var宏,宏的内容指示本函数中哪些指针变量是指向共享buffer的(在共享buffer调整时,这些指针变量的值会被重置),变量名应以_in结尾,多个变量使用逗号隔开
     *                 Fs_ShareBuffer_var_check宏,宏的内容指示本函数中哪些指针变量可能是指向共享buffer的(在共享buffer调整时,对确实是指向共享buffer指针变量的值会被重置),变量名应以_cin结尾,多个变量使用逗号隔开
     *                 这两个宏的区别在于Fs_ShareBuffer_var_check会检查指针变量是否确实指向共享buffer,Fs_ShareBuffer_var_check一般用于函数的形参
     */
#define FsShareBuffer char*
#define Fs_ShareBuffer_var // 定义与共享buffer--FsLocal_ShareBuffer关联的局部变量,以_in结尾,多个变量使用逗号隔开,如#define Fs_ShareBuffer_var a_in_acin,b_in
#define Fs_ShareBuffer_var_check // 定义可能与共享buffer--FsLocal_ShareBuffer关联的局部变量,以_cin结尾,多个变量使用逗号隔开,如#define Fs_ShareBuffer_var_check a_cin_acin,b_cin
#define Fs_ShareBuffer_var_array // 定义与共享buffer--FsLocal_ShareBuffer关联的局部指针数组变量,以_ain结尾,需要指明数组成员的个数,多个变量使用逗号隔开,如#define Fs_ShareBuffer_var_array a_cin_ain,2
#define Fs_ShareBuffer_var_array_check // 定义可能与共享buffer--FsLocal_ShareBuffer关联的局部指针数组变量,以_acin结尾,需要指明数组成员的个数,多个变量使用逗号隔开,如#define Fs_ShareBuffer_var_array_check a_cin_acin,2
    /* 共享buffer头长度 */

#define ___FsShareBufferHeadLen (sizeof(unsigned int)*2+sizeof(FsObjectList*)*2)

    /* 共享buffer长度,含头长度 */
#define Fs_ShareBuffer_Len(___shareBuffer) (((unsigned int*) (___shareBuffer))[0])
    /* 共享buffer数据区已使用的位置 */
#define Fs_ShareBuffer_pos(___shareBuffer) (((unsigned int*) (___shareBuffer))[1])
    /* 调试模式下共享buffer记录初始化的链表 */
#define Fs_ShareBuffer_debug_listInit(___shareBuffer) (((FsObjectList**)(((unsigned int*) (___shareBuffer))+2))[0])
    /* 调试模式下共享buffer记录调用的链表 */
#define Fs_ShareBuffer_debug_listCall(___shareBuffer) (((FsObjectList**)(((unsigned int*) (___shareBuffer))+2))[1])

    /* 共享buffer的简单使用,申请的空间业务上不再使用后可再次申请,一般用于固定空间大小的临时变量,即在使用此空间时不能使用FsLocal_ShareBuffer_init及把__pShareBuffer传给其他函数使用 */

#define  Fs_ShareBuffer_simple_fsMalloc(/* 指向FsShareBuffer对象的地址的指针,不能为空 */___pShareBuffer,/* 要申请空间的长度 */___len,/* 对齐方式,实际值不能大于内存分配能提供的对齐精度:1,2,4,8,16,32,64 */___align_v) \
(*(char**)(___pShareBuffer)!=NULL?(\
        (((Fs_ShareBuffer_pos(*(char**)(___pShareBuffer))+(___align_v)-1)&(~((unsigned int)((___align_v)-1))))+(___len))<=Fs_ShareBuffer_Len(*(char**)(___pShareBuffer))?(\
            ((*(char**)(___pShareBuffer))+((Fs_ShareBuffer_pos(*(char**)(___pShareBuffer))+(___align_v)-1)&(~((unsigned int)((___align_v)-1)))))\
        ):(\
            Fs_ShareBuffer_Len(*(char**)(___pShareBuffer))=(Fs_ShareBuffer_pos(*(char**)(___pShareBuffer))+___align_v+(___len) + 63)&(~63U)\
            ,*(char**)(___pShareBuffer)=(char*)fsRealloc(*(char**)(___pShareBuffer),Fs_ShareBuffer_Len(*(char**)(___pShareBuffer)))\
            ,((*(char**)(___pShareBuffer))+((Fs_ShareBuffer_pos(*(char**)(___pShareBuffer))+(___align_v)-1)&(~((unsigned int)((___align_v)-1)))))\
        )\
    ):(\
        *(char**)(___pShareBuffer)=(char*)fsMalloc((___FsShareBufferHeadLen+(___align_v)+(___len) + 63)&(~63U))\
        ,Fs_ShareBuffer_Len(*(char**)(___pShareBuffer))=(___FsShareBufferHeadLen+(___align_v)+(___len) + 63)&(~63U)\
        ,Fs_ShareBuffer_pos(*(char**)(___pShareBuffer))=___FsShareBufferHeadLen\
        ,/* 调试 */Fs_ShareBuffer_debug_listInit(*(char**)(___pShareBuffer))=fs_ObjectList_new_unsafe__IO(4)\
        ,/* 调试 */Fs_ShareBuffer_debug_listCall(*(char**)(___pShareBuffer))=fs_ObjectList_new_unsafe__IO(4)\
        ,((*(char**)(___pShareBuffer))+((___FsShareBufferHeadLen+(___align_v)-1)&(~((unsigned int)((___align_v)-1)))))\
    )\
)

    /* 共享buffer的简单使用,申请的空间业务上不再使用后可再次申请,一般用于固定空间大小的临时变量,即在使用此空间时不能使用FsLocal_ShareBuffer_init及把__pShareBuffer传给其他函数使用
     * ,与Fs_ShareBuffer_simple_malloc不同的是,此宏会返回申请空间的实际可用空间大小 */

#define  Fs_ShareBuffer_simple_malloc_with_realSize(/* 储存申请空间的真实大小 */___rstRealSize\
    ,/* 指向FsShareBuffer对象的地址的指针,不能为空 */___pShareBuffer,/* 要申请空间的长度 */___len,/* 对齐方式,实际值不能大于内存分配能提供的对齐精度:1,2,4,8,16,32,64 */___align_v) \
(*(char**)(___pShareBuffer)!=NULL?(\
        (((___rstRealSize)=((Fs_ShareBuffer_pos(*(char**)(___pShareBuffer))+(___align_v)-1)&(~((unsigned int)((___align_v)-1)))))+(___len))<=Fs_ShareBuffer_Len(*(char**)(___pShareBuffer))?(\
            ((___rstRealSize)=Fs_ShareBuffer_Len(*(char**)(___pShareBuffer))-(___rstRealSize))\
            ,((*(char**)(___pShareBuffer))+(Fs_ShareBuffer_Len(*(char**)(___pShareBuffer))-(___rstRealSize)))\
        ):(\
            Fs_ShareBuffer_Len(*(char**)(___pShareBuffer))=(Fs_ShareBuffer_pos(*(char**)(___pShareBuffer))+___align_v+(___len) + 63)&(~63U)\
            ,*(char**)(___pShareBuffer)=(char*)fsRealloc(*(char**)(___pShareBuffer),Fs_ShareBuffer_Len(*(char**)(___pShareBuffer)))\
            ,((___rstRealSize)=Fs_ShareBuffer_Len(*(char**)(___pShareBuffer))-(___rstRealSize))\
            ,((*(char**)(___pShareBuffer))+(Fs_ShareBuffer_Len(*(char**)(___pShareBuffer))-(___rstRealSize)))\
        )\
    ):(\
        ((___rstRealSize)=((___FsShareBufferHeadLen+(___align_v)+(___len) + 63)&(~63U)))\
        ,*(char**)(___pShareBuffer)=(char*)fsMalloc(___rstRealSize)\
        ,Fs_ShareBuffer_Len(*(char**)(___pShareBuffer))=___rstRealSize\
        ,Fs_ShareBuffer_pos(*(char**)(___pShareBuffer))=___FsShareBufferHeadLen\
        ,/* 调试 */Fs_ShareBuffer_debug_listInit(*(char**)(___pShareBuffer))=fs_ObjectList_new_unsafe__IO(4)\
        ,/* 调试 */Fs_ShareBuffer_debug_listCall(*(char**)(___pShareBuffer))=fs_ObjectList_new_unsafe__IO(4)\
        ,((___rstRealSize)=(___rstRealSize)-((___FsShareBufferHeadLen+(___align_v)-1)&(~((unsigned int)((___align_v)-1)))))\
        ,((*(char**)(___pShareBuffer))+(Fs_ShareBuffer_Len(*(char**)(___pShareBuffer))-(___rstRealSize)))\
    )\
)

    /* 要使用共享buffer必须在函数内调用此宏初始化以下局部变量,以FsLocal_打头的宏表示依赖函数内部的局部变量 */
    /* 初始化共享buffer */

#define FsLocal_ShareBuffer_init(/* 指向FsShareBuffer对象的地址的指针,可以为空 */___pShareBuffer,/* 对齐方式,实际值不能大于内存分配能提供的对齐精度:1,2,4,8,16,32,64 */___align)\
    char *___FsShareBuffer=(___pShareBuffer!=NULL)?*(char**) (___pShareBuffer):NULL;\
    char *___FsShareBuffer_old=___FsShareBuffer/* 用于在调用子函数前后判断___FsShareBuffer的值是否发生改变,在___FsShareBuffer后应赋值给___FsShareBuffer_old */;\
    const unsigned int ___FsShareBufferPos=((___FsShareBuffer?Fs_ShareBuffer_pos(___FsShareBuffer):___FsShareBufferHeadLen)+(___align)-1)&(~((unsigned int)((___align)-1)));\
    unsigned int ___FsShareBufferLen=___FsShareBuffer?Fs_ShareBuffer_Len(___FsShareBuffer):0;\
    /* 调试 */if (___FsShareBuffer) {\
        /* 调试 */if(NULL==Fs_ShareBuffer_debug_listInit(___FsShareBuffer)){\
            /* 调试 */Fs_ShareBuffer_debug_listInit(___FsShareBuffer) = fs_ObjectList_new_unsafe__IO(4);\
            /* 调试 */Fs_ShareBuffer_debug_listCall(___FsShareBuffer) = fs_ObjectList_new_unsafe__IO(4);\
        /* 调试 */}\
        /* 调试 */if (Fs_ShareBuffer_debug_listInit(___FsShareBuffer)->nodeCount != Fs_ShareBuffer_debug_listCall(___FsShareBuffer)->nodeCount) {\
            /* 调试 */FsPrintf(5, "FsLocal_ShareBuffer_init,invalid nodeCount(=%lu/%lu),len=%u/%u.\n", Fs_ShareBuffer_debug_listInit(___FsShareBuffer)->nodeCount\
                /* 调试 */, Fs_ShareBuffer_debug_listCall(___FsShareBuffer)->nodeCount,___FsShareBufferLen,___FsShareBufferPos), fflush(stdout);\
            /* 调试 */FsErrorDump();\
        /* 调试 */} else if (Fs_ShareBuffer_debug_listInit(___FsShareBuffer)->nodeCount > 0 && strcmp(\
            /* 调试 */(const char*) Fs_ShareBuffer_debug_listInit(___FsShareBuffer)->pNode[Fs_ShareBuffer_debug_listInit(___FsShareBuffer)->startIndex + Fs_ShareBuffer_debug_listInit(___FsShareBuffer)->nodeCount - 1]\
            /* 调试 */,(const char*) Fs_ShareBuffer_debug_listCall(___FsShareBuffer)->pNode[Fs_ShareBuffer_debug_listCall(___FsShareBuffer)->startIndex + Fs_ShareBuffer_debug_listCall(___FsShareBuffer)->nodeCount - 1]\
        /* 调试 */) != 0) {\
            /* 调试 */FsPrintf(5, "FsLocal_ShareBuffer_init,invalid funname:\"%s\"/\"%s\",nodeCount=%lu,len=%u/%u.\n"\
                /* 调试 */, (const char*) Fs_ShareBuffer_debug_listInit(___FsShareBuffer)->pNode[Fs_ShareBuffer_debug_listInit(___FsShareBuffer)->startIndex + Fs_ShareBuffer_debug_listInit(___FsShareBuffer)->nodeCount - 1]\
                /* 调试 */, (const char*) Fs_ShareBuffer_debug_listCall(___FsShareBuffer)->pNode[Fs_ShareBuffer_debug_listCall(___FsShareBuffer)->startIndex + Fs_ShareBuffer_debug_listCall(___FsShareBuffer)->nodeCount - 1]\
                /* 调试 */, Fs_ShareBuffer_debug_listInit(___FsShareBuffer)->nodeCount,___FsShareBufferLen,___FsShareBufferPos), fflush(stdout);\
            /* 调试 */FsErrorDump();\
        /* 调试 */}\
        /* 调试 */fs_ObjectList_insert_tail(Fs_ShareBuffer_debug_listInit(___FsShareBuffer), (void*) __FUNCTION__);\
    /* 调试 */}

    /* 共享buffer头指针,以FsLocal_打头的宏表示依赖函数内部的局部变量 */
#define FsLocal_ShareBuffer ___FsShareBuffer
    /* 共享buffer长度,以FsLocal_打头的宏表示依赖函数内部的局部变量 */
#define FsLocal_ShareBufferLen ___FsShareBufferLen
    /* 共享buffer可用数据的偏移量,以FsLocal_打头的宏表示依赖函数内部的局部变量 */
#define FsLocal_ShareBufferPos ___FsShareBufferPos
    //_____________________________________________________________________________//
    /* 定义修改*_ShareBuffer_var宏中定义的变量 */
#if 1
#define __FsLocal_ShareBuffer_vr_do(/* 原来的地址,char* */___old,/* 现在的地址,char* */___new,___var) do{(___var)+=0;char **const ___p=(char**)&(___var);(*___p)+=(___new)-(char*)(___old);}while(0)
#define __FsLocal_ShareBuffer_vr_0(/* 原来的地址,char* */___old,/* 现在的地址,char* */___new, ...)
#define __FsLocal_ShareBuffer_vr_1( ___old,___new,___p, ...) __FsLocal_ShareBuffer_vr_do(___old,___new,___p);__FsLocal_ShareBuffer_vr_0( ___old,___new,##__VA_ARGS__)
#define __FsLocal_ShareBuffer_vr_2( ___old,___new,___p, ...) __FsLocal_ShareBuffer_vr_do(___old,___new,___p);__FsLocal_ShareBuffer_vr_1( ___old,___new,__VA_ARGS__)
#define __FsLocal_ShareBuffer_vr_3( ___old,___new,___p, ...) __FsLocal_ShareBuffer_vr_do(___old,___new,___p);__FsLocal_ShareBuffer_vr_2( ___old,___new,__VA_ARGS__)
#define __FsLocal_ShareBuffer_vr_4( ___old,___new,___p, ...) __FsLocal_ShareBuffer_vr_do(___old,___new,___p);__FsLocal_ShareBuffer_vr_3( ___old,___new,__VA_ARGS__)
#define __FsLocal_ShareBuffer_vr_5( ___old,___new,___p, ...) __FsLocal_ShareBuffer_vr_do(___old,___new,___p);__FsLocal_ShareBuffer_vr_4( ___old,___new,__VA_ARGS__)
#define __FsLocal_ShareBuffer_vr_6( ___old,___new,___p, ...) __FsLocal_ShareBuffer_vr_do(___old,___new,___p);__FsLocal_ShareBuffer_vr_5( ___old,___new,__VA_ARGS__)
#define __FsLocal_ShareBuffer_vr_7( ___old,___new,___p, ...) __FsLocal_ShareBuffer_vr_do(___old,___new,___p);__FsLocal_ShareBuffer_vr_6( ___old,___new,__VA_ARGS__)
#define __FsLocal_ShareBuffer_vr_8( ___old,___new,___p, ...) __FsLocal_ShareBuffer_vr_do(___old,___new,___p);__FsLocal_ShareBuffer_vr_7( ___old,___new,__VA_ARGS__)
#define __FsLocal_ShareBuffer_vr_9( ___old,___new,___p, ...) __FsLocal_ShareBuffer_vr_do(___old,___new,___p);__FsLocal_ShareBuffer_vr_8( ___old,___new,__VA_ARGS__)
#define __FsLocal_ShareBuffer_vr_10(___old,___new,___p, ...) __FsLocal_ShareBuffer_vr_do(___old,___new,___p);__FsLocal_ShareBuffer_vr_9( ___old,___new,__VA_ARGS__)
#define __FsLocal_ShareBuffer_vr_11(___old,___new,___p, ...) __FsLocal_ShareBuffer_vr_do(___old,___new,___p);__FsLocal_ShareBuffer_vr_10(___old,___new,__VA_ARGS__)
#define __FsLocal_ShareBuffer_vr_12(___old,___new,___p, ...) __FsLocal_ShareBuffer_vr_do(___old,___new,___p);__FsLocal_ShareBuffer_vr_11(___old,___new,__VA_ARGS__)
#define __FsLocal_ShareBuffer_vr_13(___old,___new,___p, ...) __FsLocal_ShareBuffer_vr_do(___old,___new,___p);__FsLocal_ShareBuffer_vr_12(___old,___new,__VA_ARGS__)
#define __FsLocal_ShareBuffer_vr_14(___old,___new,___p, ...) __FsLocal_ShareBuffer_vr_do(___old,___new,___p);__FsLocal_ShareBuffer_vr_13(___old,___new,__VA_ARGS__)
#define __FsLocal_ShareBuffer_vr_15(___old,___new,___p, ...) __FsLocal_ShareBuffer_vr_do(___old,___new,___p);__FsLocal_ShareBuffer_vr_14(___old,___new,__VA_ARGS__)
#define __FsLocal_ShareBuffer_vr_16(___old,___new,___p, ...) __FsLocal_ShareBuffer_vr_do(___old,___new,___p);__FsLocal_ShareBuffer_vr_15(___old,___new,__VA_ARGS__)
#define __FsLocal_ShareBuffer_vr_17(___old,___new,___p, ...) __FsLocal_ShareBuffer_vr_do(___old,___new,___p);__FsLocal_ShareBuffer_vr_16(___old,___new,__VA_ARGS__)
#define __FsLocal_ShareBuffer_vr_18(___old,___new,___p, ...) __FsLocal_ShareBuffer_vr_do(___old,___new,___p);__FsLocal_ShareBuffer_vr_17(___old,___new,__VA_ARGS__)
#define __FsLocal_ShareBuffer_vr_19(___old,___new,___p, ...) __FsLocal_ShareBuffer_vr_do(___old,___new,___p);__FsLocal_ShareBuffer_vr_18(___old,___new,__VA_ARGS__)
#define __FsLocal_ShareBuffer_vr_20(___old,___new,___p, ...) __FsLocal_ShareBuffer_vr_do(___old,___new,___p);__FsLocal_ShareBuffer_vr_19(___old,___new,__VA_ARGS__)
#define __FsLocal_ShareBuffer_vr_21(___old,___new,___p, ...) __FsLocal_ShareBuffer_vr_do(___old,___new,___p);__FsLocal_ShareBuffer_vr_20(___old,___new,__VA_ARGS__)
#define __FsLocal_ShareBuffer_vr_22(___old,___new,___p, ...) __FsLocal_ShareBuffer_vr_do(___old,___new,___p);__FsLocal_ShareBuffer_vr_21(___old,___new,__VA_ARGS__)
#define __FsLocal_ShareBuffer_vr_23(___old,___new,___p, ...) __FsLocal_ShareBuffer_vr_do(___old,___new,___p);__FsLocal_ShareBuffer_vr_22(___old,___new,__VA_ARGS__)
#define __FsLocal_ShareBuffer_vr_24(___old,___new,___p, ...) __FsLocal_ShareBuffer_vr_do(___old,___new,___p);__FsLocal_ShareBuffer_vr_23(___old,___new,__VA_ARGS__)
#define __FsLocal_ShareBuffer_vr_25(___old,___new,___p, ...) __FsLocal_ShareBuffer_vr_do(___old,___new,___p);__FsLocal_ShareBuffer_vr_24(___old,___new,__VA_ARGS__)
#define __FsLocal_ShareBuffer_vr_26(___old,___new,___p, ...) __FsLocal_ShareBuffer_vr_do(___old,___new,___p);__FsLocal_ShareBuffer_vr_25(___old,___new,__VA_ARGS__)
#define __FsLocal_ShareBuffer_vr_27(___old,___new,___p, ...) __FsLocal_ShareBuffer_vr_do(___old,___new,___p);__FsLocal_ShareBuffer_vr_26(___old,___new,__VA_ARGS__)
#define __FsLocal_ShareBuffer_vr_28(___old,___new,___p, ...) __FsLocal_ShareBuffer_vr_do(___old,___new,___p);__FsLocal_ShareBuffer_vr_27(___old,___new,__VA_ARGS__)
#define __FsLocal_ShareBuffer_vr_29(___old,___new,___p, ...) __FsLocal_ShareBuffer_vr_do(___old,___new,___p);__FsLocal_ShareBuffer_vr_28(___old,___new,__VA_ARGS__)
#define __FsLocal_ShareBuffer_vr_30(___old,___new,___p, ...) __FsLocal_ShareBuffer_vr_do(___old,___new,___p);__FsLocal_ShareBuffer_vr_29(___old,___new,__VA_ARGS__)
#define __FsLocal_ShareBuffer_vr_31(___old,___new,___p, ...) __FsLocal_ShareBuffer_vr_do(___old,___new,___p);__FsLocal_ShareBuffer_vr_30(___old,___new,__VA_ARGS__)
#define __FsLocal_ShareBuffer_vr_32(___old,___new,___p, ...) __FsLocal_ShareBuffer_vr_do(___old,___new,___p);__FsLocal_ShareBuffer_vr_31(___old,___new,__VA_ARGS__)
#endif
    /* 定义修改*_ShareBuffer_var_check宏中定义的变量 */
#if 1
#define __FsLocal_ShareBuffer_vcr_do(/* 原来的地址,char* */___old,/* 原来的结束位置,char* */___old_end,/* 现在的地址,char* */___new,___var) \
    do{if((char*)(___var)>(char*)(___old)&&(char*)(___var)<(char*)(___old_end)){(___var)+=0;char **const ___p=(char**)&(___var);(*___p)+=(___new)-(char*)(___old);}}while(0)
#define __FsLocal_ShareBuffer_vcr_0(/* 原来的地址,char* */___old,/* 原来的结束位置,char* */___old_end,/* 现在的地址,char* */___new, ...)
#define __FsLocal_ShareBuffer_vcr_1( ___old,___old_end,___new,___p, ...) __FsLocal_ShareBuffer_vcr_do(___old,___old_end,___new,___p);__FsLocal_ShareBuffer_vcr_0( ___old,___old_end,___new,##__VA_ARGS__)
#define __FsLocal_ShareBuffer_vcr_2( ___old,___old_end,___new,___p, ...) __FsLocal_ShareBuffer_vcr_do(___old,___old_end,___new,___p);__FsLocal_ShareBuffer_vcr_1( ___old,___old_end,___new,__VA_ARGS__)
#define __FsLocal_ShareBuffer_vcr_3( ___old,___old_end,___new,___p, ...) __FsLocal_ShareBuffer_vcr_do(___old,___old_end,___new,___p);__FsLocal_ShareBuffer_vcr_2( ___old,___old_end,___new,__VA_ARGS__)
#define __FsLocal_ShareBuffer_vcr_4( ___old,___old_end,___new,___p, ...) __FsLocal_ShareBuffer_vcr_do(___old,___old_end,___new,___p);__FsLocal_ShareBuffer_vcr_3( ___old,___old_end,___new,__VA_ARGS__)
#define __FsLocal_ShareBuffer_vcr_5( ___old,___old_end,___new,___p, ...) __FsLocal_ShareBuffer_vcr_do(___old,___old_end,___new,___p);__FsLocal_ShareBuffer_vcr_4( ___old,___old_end,___new,__VA_ARGS__)
#define __FsLocal_ShareBuffer_vcr_6( ___old,___old_end,___new,___p, ...) __FsLocal_ShareBuffer_vcr_do(___old,___old_end,___new,___p);__FsLocal_ShareBuffer_vcr_5( ___old,___old_end,___new,__VA_ARGS__)
#define __FsLocal_ShareBuffer_vcr_7( ___old,___old_end,___new,___p, ...) __FsLocal_ShareBuffer_vcr_do(___old,___old_end,___new,___p);__FsLocal_ShareBuffer_vcr_6( ___old,___old_end,___new,__VA_ARGS__)
#define __FsLocal_ShareBuffer_vcr_8( ___old,___old_end,___new,___p, ...) __FsLocal_ShareBuffer_vcr_do(___old,___old_end,___new,___p);__FsLocal_ShareBuffer_vcr_7( ___old,___old_end,___new,__VA_ARGS__)
#define __FsLocal_ShareBuffer_vcr_9( ___old,___old_end,___new,___p, ...) __FsLocal_ShareBuffer_vcr_do(___old,___old_end,___new,___p);__FsLocal_ShareBuffer_vcr_8( ___old,___old_end,___new,__VA_ARGS__)
#define __FsLocal_ShareBuffer_vcr_10(___old,___old_end,___new,___p, ...) __FsLocal_ShareBuffer_vcr_do(___old,___old_end,___new,___p);__FsLocal_ShareBuffer_vcr_9( ___old,___old_end,___new,__VA_ARGS__)
#define __FsLocal_ShareBuffer_vcr_11(___old,___old_end,___new,___p, ...) __FsLocal_ShareBuffer_vcr_do(___old,___old_end,___new,___p);__FsLocal_ShareBuffer_vcr_10(___old,___old_end,___new,__VA_ARGS__)
#define __FsLocal_ShareBuffer_vcr_12(___old,___old_end,___new,___p, ...) __FsLocal_ShareBuffer_vcr_do(___old,___old_end,___new,___p);__FsLocal_ShareBuffer_vcr_11(___old,___old_end,___new,__VA_ARGS__)
#define __FsLocal_ShareBuffer_vcr_13(___old,___old_end,___new,___p, ...) __FsLocal_ShareBuffer_vcr_do(___old,___old_end,___new,___p);__FsLocal_ShareBuffer_vcr_12(___old,___old_end,___new,__VA_ARGS__)
#define __FsLocal_ShareBuffer_vcr_14(___old,___old_end,___new,___p, ...) __FsLocal_ShareBuffer_vcr_do(___old,___old_end,___new,___p);__FsLocal_ShareBuffer_vcr_13(___old,___old_end,___new,__VA_ARGS__)
#define __FsLocal_ShareBuffer_vcr_15(___old,___old_end,___new,___p, ...) __FsLocal_ShareBuffer_vcr_do(___old,___old_end,___new,___p);__FsLocal_ShareBuffer_vcr_14(___old,___old_end,___new,__VA_ARGS__)
#define __FsLocal_ShareBuffer_vcr_16(___old,___old_end,___new,___p, ...) __FsLocal_ShareBuffer_vcr_do(___old,___old_end,___new,___p);__FsLocal_ShareBuffer_vcr_15(___old,___old_end,___new,__VA_ARGS__)
#define __FsLocal_ShareBuffer_vcr_17(___old,___old_end,___new,___p, ...) __FsLocal_ShareBuffer_vcr_do(___old,___old_end,___new,___p);__FsLocal_ShareBuffer_vcr_16(___old,___old_end,___new,__VA_ARGS__)
#define __FsLocal_ShareBuffer_vcr_18(___old,___old_end,___new,___p, ...) __FsLocal_ShareBuffer_vcr_do(___old,___old_end,___new,___p);__FsLocal_ShareBuffer_vcr_17(___old,___old_end,___new,__VA_ARGS__)
#define __FsLocal_ShareBuffer_vcr_19(___old,___old_end,___new,___p, ...) __FsLocal_ShareBuffer_vcr_do(___old,___old_end,___new,___p);__FsLocal_ShareBuffer_vcr_18(___old,___old_end,___new,__VA_ARGS__)
#define __FsLocal_ShareBuffer_vcr_20(___old,___old_end,___new,___p, ...) __FsLocal_ShareBuffer_vcr_do(___old,___old_end,___new,___p);__FsLocal_ShareBuffer_vcr_19(___old,___old_end,___new,__VA_ARGS__)
#define __FsLocal_ShareBuffer_vcr_21(___old,___old_end,___new,___p, ...) __FsLocal_ShareBuffer_vcr_do(___old,___old_end,___new,___p);__FsLocal_ShareBuffer_vcr_20(___old,___old_end,___new,__VA_ARGS__)
#define __FsLocal_ShareBuffer_vcr_22(___old,___old_end,___new,___p, ...) __FsLocal_ShareBuffer_vcr_do(___old,___old_end,___new,___p);__FsLocal_ShareBuffer_vcr_21(___old,___old_end,___new,__VA_ARGS__)
#define __FsLocal_ShareBuffer_vcr_23(___old,___old_end,___new,___p, ...) __FsLocal_ShareBuffer_vcr_do(___old,___old_end,___new,___p);__FsLocal_ShareBuffer_vcr_22(___old,___old_end,___new,__VA_ARGS__)
#define __FsLocal_ShareBuffer_vcr_24(___old,___old_end,___new,___p, ...) __FsLocal_ShareBuffer_vcr_do(___old,___old_end,___new,___p);__FsLocal_ShareBuffer_vcr_23(___old,___old_end,___new,__VA_ARGS__)
#define __FsLocal_ShareBuffer_vcr_25(___old,___old_end,___new,___p, ...) __FsLocal_ShareBuffer_vcr_do(___old,___old_end,___new,___p);__FsLocal_ShareBuffer_vcr_24(___old,___old_end,___new,__VA_ARGS__)
#define __FsLocal_ShareBuffer_vcr_26(___old,___old_end,___new,___p, ...) __FsLocal_ShareBuffer_vcr_do(___old,___old_end,___new,___p);__FsLocal_ShareBuffer_vcr_25(___old,___old_end,___new,__VA_ARGS__)
#define __FsLocal_ShareBuffer_vcr_27(___old,___old_end,___new,___p, ...) __FsLocal_ShareBuffer_vcr_do(___old,___old_end,___new,___p);__FsLocal_ShareBuffer_vcr_26(___old,___old_end,___new,__VA_ARGS__)
#define __FsLocal_ShareBuffer_vcr_28(___old,___old_end,___new,___p, ...) __FsLocal_ShareBuffer_vcr_do(___old,___old_end,___new,___p);__FsLocal_ShareBuffer_vcr_27(___old,___old_end,___new,__VA_ARGS__)
#define __FsLocal_ShareBuffer_vcr_29(___old,___old_end,___new,___p, ...) __FsLocal_ShareBuffer_vcr_do(___old,___old_end,___new,___p);__FsLocal_ShareBuffer_vcr_28(___old,___old_end,___new,__VA_ARGS__)
#define __FsLocal_ShareBuffer_vcr_30(___old,___old_end,___new,___p, ...) __FsLocal_ShareBuffer_vcr_do(___old,___old_end,___new,___p);__FsLocal_ShareBuffer_vcr_29(___old,___old_end,___new,__VA_ARGS__)
#define __FsLocal_ShareBuffer_vcr_31(___old,___old_end,___new,___p, ...) __FsLocal_ShareBuffer_vcr_do(___old,___old_end,___new,___p);__FsLocal_ShareBuffer_vcr_30(___old,___old_end,___new,__VA_ARGS__)
#define __FsLocal_ShareBuffer_vcr_32(___old,___old_end,___new,___p, ...) __FsLocal_ShareBuffer_vcr_do(___old,___old_end,___new,___p);__FsLocal_ShareBuffer_vcr_31(___old,___old_end,___new,__VA_ARGS__)
#endif
    /* 定义修改*_ShareBuffer_var_array宏定义指针数组变量的成员 */
#if 1
#define __FsLocal_ShareBuffer_var_do(/* 原来的地址,char* */___old,/* 现在的地址,char* */___new,___array,___arraySize) do{\
    int arraySize=___arraySize;\
    while(arraySize-->0){__FsLocal_ShareBuffer_vr_do(___old,___new,___array[arraySize]);}\
}while(0)
#define __FsLocal_ShareBuffer_var_0(/* 原来的地址,char* */___old,/* 现在的地址,char* */___new, ...)
#define __FsLocal_ShareBuffer_var_2( ___old,___new,___a,___as, ...) __FsLocal_ShareBuffer_var_do(___old,___new,___a,___as);__FsLocal_ShareBuffer_var_0( ___old,___new,##__VA_ARGS__)
#define __FsLocal_ShareBuffer_var_4( ___old,___new,___a,___as, ...) __FsLocal_ShareBuffer_var_do(___old,___new,___a,___as);__FsLocal_ShareBuffer_var_2( ___old,___new,__VA_ARGS__)
#define __FsLocal_ShareBuffer_var_6( ___old,___new,___a,___as, ...) __FsLocal_ShareBuffer_var_do(___old,___new,___a,___as);__FsLocal_ShareBuffer_var_4( ___old,___new,__VA_ARGS__)
#define __FsLocal_ShareBuffer_var_8( ___old,___new,___a,___as, ...) __FsLocal_ShareBuffer_var_do(___old,___new,___a,___as);__FsLocal_ShareBuffer_var_6( ___old,___new,__VA_ARGS__)
#define __FsLocal_ShareBuffer_var_10(___old,___new,___a,___as, ...) __FsLocal_ShareBuffer_var_do(___old,___new,___a,___as);__FsLocal_ShareBuffer_var_8( ___old,___new,__VA_ARGS__)
#define __FsLocal_ShareBuffer_var_12(___old,___new,___a,___as, ...) __FsLocal_ShareBuffer_var_do(___old,___new,___a,___as);__FsLocal_ShareBuffer_var_10(___old,___new,__VA_ARGS__)
#define __FsLocal_ShareBuffer_var_14(___old,___new,___a,___as, ...) __FsLocal_ShareBuffer_var_do(___old,___new,___a,___as);__FsLocal_ShareBuffer_var_12(___old,___new,__VA_ARGS__)
#define __FsLocal_ShareBuffer_var_16(___old,___new,___a,___as, ...) __FsLocal_ShareBuffer_var_do(___old,___new,___a,___as);__FsLocal_ShareBuffer_var_14(___old,___new,__VA_ARGS__)
#define __FsLocal_ShareBuffer_var_18(___old,___new,___a,___as, ...) __FsLocal_ShareBuffer_var_do(___old,___new,___a,___as);__FsLocal_ShareBuffer_var_16(___old,___new,__VA_ARGS__)
#define __FsLocal_ShareBuffer_var_20(___old,___new,___a,___as, ...) __FsLocal_ShareBuffer_var_do(___old,___new,___a,___as);__FsLocal_ShareBuffer_var_18(___old,___new,__VA_ARGS__)
#define __FsLocal_ShareBuffer_var_22(___old,___new,___a,___as, ...) __FsLocal_ShareBuffer_var_do(___old,___new,___a,___as);__FsLocal_ShareBuffer_var_20(___old,___new,__VA_ARGS__)
#define __FsLocal_ShareBuffer_var_24(___old,___new,___a,___as, ...) __FsLocal_ShareBuffer_var_do(___old,___new,___a,___as);__FsLocal_ShareBuffer_var_22(___old,___new,__VA_ARGS__)
#define __FsLocal_ShareBuffer_var_26(___old,___new,___a,___as, ...) __FsLocal_ShareBuffer_var_do(___old,___new,___a,___as);__FsLocal_ShareBuffer_var_24(___old,___new,__VA_ARGS__)
#define __FsLocal_ShareBuffer_var_28(___old,___new,___a,___as, ...) __FsLocal_ShareBuffer_var_do(___old,___new,___a,___as);__FsLocal_ShareBuffer_var_26(___old,___new,__VA_ARGS__)
#define __FsLocal_ShareBuffer_var_30(___old,___new,___a,___as, ...) __FsLocal_ShareBuffer_var_do(___old,___new,___a,___as);__FsLocal_ShareBuffer_var_28(___old,___new,__VA_ARGS__)
#define __FsLocal_ShareBuffer_var_32(___old,___new,___a,___as, ...) __FsLocal_ShareBuffer_var_do(___old,___new,___a,___as);__FsLocal_ShareBuffer_var_30(___old,___new,__VA_ARGS__)
#endif
    /* 定义修改*_ShareBuffer_var_array_check宏定义指针数组变量的成员 */
#if 1
#define __FsLocal_ShareBuffer_vacr_do(/* 原来的地址,char* */___old,/* 原来的结束位置,char* */___old_end,/* 现在的地址,char* */___new,___array,___arraySize) do{\
    int arraySize=___arraySize;\
    while(arraySize-->0){__FsLocal_ShareBuffer_vcr_do(___old,___old_end,___new,___array[arraySize]);}\
}while(0)
#define __FsLocal_ShareBuffer_vacr_0(/* 原来的地址,char* */___old,/* 原来的结束位置,char* */___old_end,/* 现在的地址,char* */___new, ...)
#define __FsLocal_ShareBuffer_vacr_2( ___old,___old_end,___new,___a,___as, ...) __FsLocal_ShareBuffer_vacr_do(___old,___old_end,___new,___a,___as);__FsLocal_ShareBuffer_vacr_0( ___old,___old_end,___new,##__VA_ARGS__)
#define __FsLocal_ShareBuffer_vacr_4( ___old,___old_end,___new,___a,___as, ...) __FsLocal_ShareBuffer_vacr_do(___old,___old_end,___new,___a,___as);__FsLocal_ShareBuffer_vacr_2( ___old,___old_end,___new,__VA_ARGS__)
#define __FsLocal_ShareBuffer_vacr_6( ___old,___old_end,___new,___a,___as, ...) __FsLocal_ShareBuffer_vacr_do(___old,___old_end,___new,___a,___as);__FsLocal_ShareBuffer_vacr_4( ___old,___old_end,___new,__VA_ARGS__)
#define __FsLocal_ShareBuffer_vacr_8( ___old,___old_end,___new,___a,___as, ...) __FsLocal_ShareBuffer_vacr_do(___old,___old_end,___new,___a,___as);__FsLocal_ShareBuffer_vacr_6( ___old,___old_end,___new,__VA_ARGS__)
#define __FsLocal_ShareBuffer_vacr_10(___old,___old_end,___new,___a,___as, ...) __FsLocal_ShareBuffer_vacr_do(___old,___old_end,___new,___a,___as);__FsLocal_ShareBuffer_vacr_8( ___old,___old_end,___new,__VA_ARGS__)
#define __FsLocal_ShareBuffer_vacr_12(___old,___old_end,___new,___a,___as, ...) __FsLocal_ShareBuffer_vacr_do(___old,___old_end,___new,___a,___as);__FsLocal_ShareBuffer_vacr_10(___old,___old_end,___new,__VA_ARGS__)
#define __FsLocal_ShareBuffer_vacr_14(___old,___old_end,___new,___a,___as, ...) __FsLocal_ShareBuffer_vacr_do(___old,___old_end,___new,___a,___as);__FsLocal_ShareBuffer_vacr_12(___old,___old_end,___new,__VA_ARGS__)
#define __FsLocal_ShareBuffer_vacr_16(___old,___old_end,___new,___a,___as, ...) __FsLocal_ShareBuffer_vacr_do(___old,___old_end,___new,___a,___as);__FsLocal_ShareBuffer_vacr_14(___old,___old_end,___new,__VA_ARGS__)
#define __FsLocal_ShareBuffer_vacr_18(___old,___old_end,___new,___a,___as, ...) __FsLocal_ShareBuffer_vacr_do(___old,___old_end,___new,___a,___as);__FsLocal_ShareBuffer_vacr_16(___old,___old_end,___new,__VA_ARGS__)
#define __FsLocal_ShareBuffer_vacr_20(___old,___old_end,___new,___a,___as, ...) __FsLocal_ShareBuffer_vacr_do(___old,___old_end,___new,___a,___as);__FsLocal_ShareBuffer_vacr_18(___old,___old_end,___new,__VA_ARGS__)
#define __FsLocal_ShareBuffer_vacr_22(___old,___old_end,___new,___a,___as, ...) __FsLocal_ShareBuffer_vacr_do(___old,___old_end,___new,___a,___as);__FsLocal_ShareBuffer_vacr_20(___old,___old_end,___new,__VA_ARGS__)
#define __FsLocal_ShareBuffer_vacr_24(___old,___old_end,___new,___a,___as, ...) __FsLocal_ShareBuffer_vacr_do(___old,___old_end,___new,___a,___as);__FsLocal_ShareBuffer_vacr_22(___old,___old_end,___new,__VA_ARGS__)
#define __FsLocal_ShareBuffer_vacr_26(___old,___old_end,___new,___a,___as, ...) __FsLocal_ShareBuffer_vacr_do(___old,___old_end,___new,___a,___as);__FsLocal_ShareBuffer_vacr_24(___old,___old_end,___new,__VA_ARGS__)
#define __FsLocal_ShareBuffer_vacr_28(___old,___old_end,___new,___a,___as, ...) __FsLocal_ShareBuffer_vacr_do(___old,___old_end,___new,___a,___as);__FsLocal_ShareBuffer_vacr_26(___old,___old_end,___new,__VA_ARGS__)
#define __FsLocal_ShareBuffer_vacr_30(___old,___old_end,___new,___a,___as, ...) __FsLocal_ShareBuffer_vacr_do(___old,___old_end,___new,___a,___as);__FsLocal_ShareBuffer_vacr_28(___old,___old_end,___new,__VA_ARGS__)
#define __FsLocal_ShareBuffer_vacr_32(___old,___old_end,___new,___a,___as, ...) __FsLocal_ShareBuffer_vacr_do(___old,___old_end,___new,___a,___as);__FsLocal_ShareBuffer_vacr_30(___old,___old_end,___new,__VA_ARGS__)
#endif
    //_____________________________________________________________________________//
#define __FsLocal_ShareBuffer_var_reset(/* 原来的地址,char* */___old,/* 现在的地址,char* */___new) do{\
    {FsFun(FsMacrosValue2(__FsLocal_ShareBuffer_vr_,FsArgc(Fs_ShareBuffer_var)),___old,___new,Fs_ShareBuffer_var);}\
}while (0)
#define __FsLocal_ShareBuffer_var_check_reset(/* 原来的地址,char* */___old,/* 原来的结束位置,char* */___old_end,/* 现在的地址,char* */___new) do{\
    {FsFun(FsMacrosValue2(__FsLocal_ShareBuffer_vcr_,FsArgc(Fs_ShareBuffer_var_check)),___old,___old_end,___new,Fs_ShareBuffer_var_check);}\
}while (0)
#define __FsLocal_ShareBuffer_var_array_reset(/* 原来的地址,char* */___old,/* 现在的地址,char* */___new) do{\
    {FsFun(FsMacrosValue2(__FsLocal_ShareBuffer_var_,FsArgc(Fs_ShareBuffer_var_array)),___old,___new,Fs_ShareBuffer_var_array);}\
}while (0)
#define __FsLocal_ShareBuffer_var_array_check_reset(/* 原来的地址,char* */___old,/* 原来的结束位置,char* */___old_end,/* 现在的地址,char* */___new) do{\
    {FsFun(FsMacrosValue2(__FsLocal_ShareBuffer_vacr_,FsArgc(Fs_ShareBuffer_var_array_check)),___old,___old_end,___new,Fs_ShareBuffer_var_array_check);}\
}while (0)

    /* 本函数在使用共享buffer的同时把共享buffer传给其他函数前调用,子函数调用后应调用FsLocal_ShareBuffer_call_after,共享buffer必须以FsLocal_ShareBuffer_init初始化 */

#define FsLocal_ShareBuffer_call_before(/* 表示本函数使用的空间的结束位置相对于___FsShareBuffer的偏移量,不小于FsLocal_ShareBufferPos,没使用值应为FsLocal_ShareBufferPos */___pos) do{\
    const unsigned int ___FsShareBufferPosNew=(unsigned int)(___pos);\
    if(NULL==___FsShareBuffer){\
        ___FsShareBufferLen=(___FsShareBufferPosNew+63)&(~63U);\
        ___FsShareBuffer_old=___FsShareBuffer=(char*)fsMalloc(___FsShareBufferLen);\
        /* 修改*_ShareBuffer_var宏定义指针 */__FsLocal_ShareBuffer_var_reset(NULL,___FsShareBuffer);\
        /* 修改*_ShareBuffer_var_check宏定义指针 */__FsLocal_ShareBuffer_var_check_reset(NULL,NULL,___FsShareBuffer);\
        /* 修改*_ShareBuffer_var_array宏定义指针数组的成员 */__FsLocal_ShareBuffer_var_array_reset(___FsShareBuffer_old,___FsShareBuffer);\
        /* 修改*_ShareBuffer_var_array_check宏定义指针数组成员 */__FsLocal_ShareBuffer_var_array_check_reset(___FsShareBuffer_old,___FsShareBuffer_old+___FsShareBufferLen,___FsShareBuffer);\
        /* 调试 */Fs_ShareBuffer_debug_listInit(___FsShareBuffer) = fs_ObjectList_new_unsafe__IO(4);\
        /* 调试 */Fs_ShareBuffer_debug_listCall(___FsShareBuffer) = fs_ObjectList_new_unsafe__IO(4);\
        /* 调试 */fs_ObjectList_insert_tail(Fs_ShareBuffer_debug_listInit(___FsShareBuffer), (void*) __FUNCTION__);\
    }\
    Fs_ShareBuffer_Len(___FsShareBuffer)=___FsShareBufferLen;\
    Fs_ShareBuffer_pos(___FsShareBuffer)=___FsShareBufferPosNew;\
    /* 调试 */if(___FsShareBufferPosNew<FsLocal_ShareBufferPos){\
        /* 调试 */FsPrintf(5, "Invalid ___pos(=%u/%u).\n",___FsShareBufferPosNew,FsLocal_ShareBufferPos);\
        /* 调试 */FsErrorDump();\
    /* 调试 */}\
    /* 调试 */if(Fs_ShareBuffer_debug_listInit(___FsShareBuffer)->nodeCount!=Fs_ShareBuffer_debug_listCall(___FsShareBuffer)->nodeCount+1){\
        /* 调试 */FsPrintf(5,"FsLocal_ShareBuffer_call_before,invalid nodeCount(=%lu/%lu),len=%u/%u.\n",Fs_ShareBuffer_debug_listInit(___FsShareBuffer)->nodeCount\
        /* 调试 */,Fs_ShareBuffer_debug_listCall(___FsShareBuffer)->nodeCount,___FsShareBufferLen,___FsShareBufferPos),fflush(stdout);\
        /* 调试 */FsErrorDump();\
    /* 调试 */}else if(strcmp(__FUNCTION__\
        /* 调试 */,(const char*)Fs_ShareBuffer_debug_listInit(___FsShareBuffer)->pNode[Fs_ShareBuffer_debug_listInit(___FsShareBuffer)->startIndex+Fs_ShareBuffer_debug_listInit(___FsShareBuffer)->nodeCount-1]\
        /* 调试 */)!=0){\
        /* 调试 */FsPrintf(5,"FsLocal_ShareBuffer_call_before,invalid funname:\"%s\",nodeCount=%lu,len=%u/%u.\n"\
            /* 调试 */,(const char*)Fs_ShareBuffer_debug_listInit(___FsShareBuffer)->pNode[Fs_ShareBuffer_debug_listInit(___FsShareBuffer)->startIndex+Fs_ShareBuffer_debug_listCall(___FsShareBuffer)->nodeCount-1]\
            /* 调试 */,Fs_ShareBuffer_debug_listInit(___FsShareBuffer)->nodeCount,___FsShareBufferLen,___FsShareBufferPos),fflush(stdout);\
       /* 调试 */ FsErrorDump();\
    /* 调试 */}\
    /* 调试 */fs_ObjectList_insert_tail(Fs_ShareBuffer_debug_listCall(___FsShareBuffer), (void*)__FUNCTION__);\
}while(0)

    /* 本函数在使用共享buffer的同时把共享buffer传给其他函数后调用,子函数调用前应调用FsLocal_ShareBuffer_call_before,共享buffer必须以FsLocal_ShareBuffer_init初始化 */

#define FsLocal_ShareBuffer_call_after() do{\
    /* 调试 */if(NULL==___FsShareBuffer_old||NULL==___FsShareBuffer){\
        /* 调试 */FsPrintf(5,"Buffer is NULL(%p/%p),len=%u/%u.\n",___FsShareBuffer_old,___FsShareBuffer,___FsShareBufferLen,___FsShareBufferPos),fflush(stdout);\
        /* 调试 */FsErrorDump();\
    /* 调试 */}\
    if(___FsShareBuffer_old!=___FsShareBuffer){\
        /* 内存重新分配过 */\
        /* 修改*_ShareBuffer_var宏定义指针 */__FsLocal_ShareBuffer_var_reset(___FsShareBuffer_old,___FsShareBuffer);\
        /* 修改*_ShareBuffer_var_check宏定义指针 */__FsLocal_ShareBuffer_var_check_reset(___FsShareBuffer_old,___FsShareBuffer_old+___FsShareBufferLen,___FsShareBuffer);\
        /* 修改*_ShareBuffer_var_array宏定义指针数组的成员 */__FsLocal_ShareBuffer_var_array_reset(___FsShareBuffer_old,___FsShareBuffer);\
        /* 修改*_ShareBuffer_var_array_check宏定义指针数组成员 */__FsLocal_ShareBuffer_var_array_check_reset(___FsShareBuffer_old,___FsShareBuffer_old+___FsShareBufferLen,___FsShareBuffer);\
        ___FsShareBuffer_old=___FsShareBuffer;\
    }\
    ___FsShareBufferLen=Fs_ShareBuffer_Len(___FsShareBuffer);\
    /* 调试 */if(Fs_ShareBuffer_debug_listInit(___FsShareBuffer)->nodeCount!=Fs_ShareBuffer_debug_listCall(___FsShareBuffer)->nodeCount||0==Fs_ShareBuffer_debug_listInit(___FsShareBuffer)->nodeCount){\
        /* 调试 */FsPrintf(5,"FsLocal_ShareBuffer_init,invalid nodeCount(=%lu/%lu),len=%u/%u.\n",Fs_ShareBuffer_debug_listInit(___FsShareBuffer)->nodeCount\
            /* 调试 */,Fs_ShareBuffer_debug_listCall(___FsShareBuffer)->nodeCount,___FsShareBufferLen,___FsShareBufferPos),fflush(stdout);\
        /* 调试 */FsErrorDump();\
    /* 调试 */}else if(strcmp(\
        /* 调试 */(const char*)Fs_ShareBuffer_debug_listInit(___FsShareBuffer)->pNode[Fs_ShareBuffer_debug_listInit(___FsShareBuffer)->startIndex+Fs_ShareBuffer_debug_listInit(___FsShareBuffer)->nodeCount-1]\
        /* 调试 */,(const char*)Fs_ShareBuffer_debug_listCall(___FsShareBuffer)->pNode[Fs_ShareBuffer_debug_listCall(___FsShareBuffer)->startIndex+Fs_ShareBuffer_debug_listCall(___FsShareBuffer)->nodeCount-1]\
        /* 调试 */)!=0){\
        /* 调试 */FsPrintf(5,"FsLocal_ShareBuffer_init,invalid funname:\"%s\"/\"%s\",nodeCount=%lu,len=%u/%u.\n"\
            /* 调试 */,(const char*)Fs_ShareBuffer_debug_listInit(___FsShareBuffer)->pNode[Fs_ShareBuffer_debug_listInit(___FsShareBuffer)->startIndex+Fs_ShareBuffer_debug_listInit(___FsShareBuffer)->nodeCount-1]\
            /* 调试 */,(const char*)Fs_ShareBuffer_debug_listCall(___FsShareBuffer)->pNode[Fs_ShareBuffer_debug_listCall(___FsShareBuffer)->startIndex+Fs_ShareBuffer_debug_listCall(___FsShareBuffer)->nodeCount-1]\
            /* 调试 */,Fs_ShareBuffer_debug_listInit(___FsShareBuffer)->nodeCount,___FsShareBufferLen,___FsShareBufferPos),fflush(stdout);\
        /* 调试 */FsErrorDump();\
    /* 调试 */}\
    /* 调试 */fs_ObjectList_remove_tail(Fs_ShareBuffer_debug_listCall(___FsShareBuffer));\
}while(0)

    /* 释放共享buffer,共享buffer必须以FsLocal_ShareBuffer_init初始化 */

#define FsLocal_ShareBuffer_release(/* 指向当前buffer空间的地址的指针,为空表示内存在本函数中申请 */___pShareBuffer\
    ,/* 保留空间,0-不保留本函数扩展的空间,1-保留本函数扩展的空间(在需要把本函数扩展的空间给上一个函数使用时需要设置为1) */___keepData) do{\
    /* 调试 */if(___FsShareBuffer!=NULL){\
        /* 调试 */if(Fs_ShareBuffer_debug_listInit(___FsShareBuffer)->nodeCount!=Fs_ShareBuffer_debug_listCall(___FsShareBuffer)->nodeCount+1){\
            /* 调试 */FsPrintf(5,"FsLocal_ShareBuffer_call_before,invalid nodeCount(=%lu/%lu),len=%u/%u.\n",Fs_ShareBuffer_debug_listInit(___FsShareBuffer)->nodeCount\
                /* 调试 */,Fs_ShareBuffer_debug_listCall(___FsShareBuffer)->nodeCount,___FsShareBufferLen,___FsShareBufferPos),fflush(stdout);\
            /* 调试 */FsErrorDump();\
        /* 调试 */}else if(strcmp(__FUNCTION__\
            /* 调试 */,(const char*)Fs_ShareBuffer_debug_listInit(___FsShareBuffer)->pNode[Fs_ShareBuffer_debug_listInit(___FsShareBuffer)->startIndex+Fs_ShareBuffer_debug_listInit(___FsShareBuffer)->nodeCount-1]\
            /* 调试 */)!=0){\
            /* 调试 */FsPrintf(5,"FsLocal_ShareBuffer_call_before,invalid funname:\"%s\",nodeCount=%lu,len=%u/%u.\n"\
                /* 调试 */,(const char*)Fs_ShareBuffer_debug_listInit(___FsShareBuffer)->pNode[Fs_ShareBuffer_debug_listInit(___FsShareBuffer)->startIndex+Fs_ShareBuffer_debug_listInit(___FsShareBuffer)->nodeCount-1]\
                /* 调试 */,Fs_ShareBuffer_debug_listInit(___FsShareBuffer)->nodeCount,___FsShareBufferLen,___FsShareBufferPos),fflush(stdout);\
           /* 调试 */FsErrorDump();\
        /* 调试 */}\
        /* 调试 */fs_ObjectList_remove_tail(Fs_ShareBuffer_debug_listInit(___FsShareBuffer));\
        /* 调试 */if(0==Fs_ShareBuffer_debug_listInit(___FsShareBuffer)->nodeCount){\
            /* 调试 */fs_ObjectList_delete_unsafe__OI(Fs_ShareBuffer_debug_listInit(___FsShareBuffer)),Fs_ShareBuffer_debug_listInit(___FsShareBuffer)=NULL;\
            /* 调试 */fs_ObjectList_delete_unsafe__OI(Fs_ShareBuffer_debug_listCall(___FsShareBuffer)),Fs_ShareBuffer_debug_listCall(___FsShareBuffer)=NULL;\
        /* 调试 */}\
    /* 调试 */}\
    if(NULL==(___pShareBuffer)){\
        if(___FsShareBuffer!=NULL)fsFree(___FsShareBuffer);\
    }else if(___FsShareBuffer!=NULL){\
        Fs_ShareBuffer_pos(___FsShareBuffer)=___FsShareBufferPos;\
        if(0==(___keepData)&&___FsShareBufferLen>___FsShareBufferPos&&(--___FsShareBufferLen&63U)==0){\
            *(char**)(___pShareBuffer)=___FsShareBuffer=(char*) fsRealloc(___FsShareBuffer,___FsShareBufferLen);\
        }else *(char**)(___pShareBuffer)=___FsShareBuffer;\
        Fs_ShareBuffer_Len(___FsShareBuffer)=___FsShareBufferLen;\
    }\
}while (0)

    /* 共享buffer,前4字节表示整个buffer的长度,5-8字节表示buffer可用位置相对于头的偏移量,如buffer的大小不够,重新按64字节对齐方式分配pShareBuffer,共享buffer必须以FsLocal_ShareBuffer_init初始化 */

#define FsLocal_ShareBuffer_resize(/* 需要的最小大小,从Fs_ShareBuffer_pos开始计算 */___size) do {\
    if(___FsShareBuffer){\
        if (___FsShareBufferLen<___FsShareBufferPos+(___size)){\
            ___FsShareBufferLen=(___FsShareBufferLen+63)&(~63U);\
            if(___FsShareBufferLen<___FsShareBufferPos+(___size)){\
                const unsigned int ___FsShareBufferLen_=(___FsShareBufferPos+(___size) + 63)&(~63U);\
                ___FsShareBuffer = (char*) fsRealloc(___FsShareBuffer,___FsShareBufferLen_);\
                if(___FsShareBuffer_old!=___FsShareBuffer){\
                    /* 修改*_ShareBuffer_var宏定义指针 */__FsLocal_ShareBuffer_var_reset(___FsShareBuffer_old,___FsShareBuffer);\
                    /* 修改*_ShareBuffer_var_check宏定义指针 */__FsLocal_ShareBuffer_var_check_reset(___FsShareBuffer_old,___FsShareBuffer_old+___FsShareBufferLen,___FsShareBuffer);\
                    /* 修改*_ShareBuffer_var_array宏定义指针数组的成员 */__FsLocal_ShareBuffer_var_array_reset(___FsShareBuffer_old,___FsShareBuffer);\
                    /* 修改*_ShareBuffer_var_array_check宏定义指针数组成员 */__FsLocal_ShareBuffer_var_array_check_reset(___FsShareBuffer_old,___FsShareBuffer_old+___FsShareBufferLen,___FsShareBuffer);\
                    ___FsShareBuffer_old=___FsShareBuffer;\
                }\
                ___FsShareBufferLen=___FsShareBufferLen_;\
            }\
        }\
    }else{\
        ___FsShareBufferLen=(___FsShareBufferPos+(___size) + 63)&(~63U);\
        ___FsShareBuffer_old=___FsShareBuffer= (char*) fsMalloc(___FsShareBufferLen);\
        /* 修改*_ShareBuffer_var宏定义指针 */__FsLocal_ShareBuffer_var_reset(NULL,___FsShareBuffer);\
        /* 修改*_ShareBuffer_var_check宏定义指针 */__FsLocal_ShareBuffer_var_check_reset(NULL,NULL,___FsShareBuffer);\
        /* 修改*_ShareBuffer_var_array宏定义指针数组的成员 */__FsLocal_ShareBuffer_var_array_reset(___FsShareBuffer_old,___FsShareBuffer);\
        /* 修改*_ShareBuffer_var_array_check宏定义指针数组成员 */__FsLocal_ShareBuffer_var_array_check_reset(___FsShareBuffer_old,___FsShareBuffer_old+___FsShareBufferLen,___FsShareBuffer);\
        /* 调试 */Fs_ShareBuffer_debug_listInit(___FsShareBuffer) = fs_ObjectList_new_unsafe__IO(4);\
        /* 调试 */Fs_ShareBuffer_debug_listCall(___FsShareBuffer) = fs_ObjectList_new_unsafe__IO(4);\
        /* 调试 */fs_ObjectList_insert_tail(Fs_ShareBuffer_debug_listInit(___FsShareBuffer), (void*) __FUNCTION__);\
    }\
}while (0)

    //#define FsMallocEnable
#ifdef FsMallocEnable
    void * _fsMalloc(/* 申请的空间大小 */size_t _Size, /* 调用此函数的函数名 */const char functionname[], /* 调用此函数的行号 */const int line);
#define fsMalloc(_Size) _fsMalloc(_Size,__FUNCTION__,__LINE__)
    void *_fsRealloc(void *_Memory, size_t _NewSize, /* 调用此函数的函数名 */const char functionname[], /* 调用此函数的行号 */const int line);
#define fsRealloc(_Memory,_NewSize) _fsRealloc(_Memory,_NewSize,__FUNCTION__,__LINE__)
    void _fsFree(void *_Memory, /* 调用此函数的函数名 */const char functionname[], /* 调用此函数的行号 */const int line);
#define fsFree(_Memory) _fsFree(_Memory,__FUNCTION__,__LINE__)
    /* 输出fsMalloc分配的内存信息 */
    void fsMalloc_out(FILE * const fd);
    /* 检查是否需要输出fsMalloc的内存信息,需要输出时会删除标记文件,标记文件内存的是本进程的进程号才会输出 */
    void fsMalloc_out_check(/* 标记文件名 */const char tagFile[]);
    /* 退出时检查哪些内存还未释放 */
    void fsMalloc_exit_check();
#else
#define fsMalloc(_Size) malloc(_Size)
#define fsRealloc(_Memory,_Size) realloc(_Memory,_Size)
#define fsFree(_Memory) free(_Memory)
#define _fsFree free
#endif

    /* 一个管理多个内存的对象,支持批量删除内存,删除顺序是倒序删除的 */
    typedef struct {
        FsStructList list;
    } FsMemeryFree;

    /*
     * 创建一个FsMemeryFree实例;
     * 返回创建的实例指针.
     */
    FsMemeryFree* fs_memeryFree_new__IO();
    /* 删除pMemeryFree指向的实例对象 */
    void fs_memeryFree_delete__OI(FsMemeryFree *pMemeryFree);
    /* 清理pMemeryFree对象,全调用对象的删除方法 */
    void fs_memeryFree_clear(FsMemeryFree* pMemeryFree);
    /* 添加一个对象的删除 */
    void fs_memeryFree_add_object(FsMemeryFree *pMemeryFree, /* 释放内存的函数指针 */void (*pDelete)(void *p), /* 对象指针 */ void *pData);
    /* 添加一个线程对象的删除 */
    void fs_memeryFree_add_thread(FsMemeryFree *pMemeryFree, /* 停止线程的函数指针 */void (*pStopThread)(void *p), /* 释放内存的函数指针 */void (*pDelete)(void *p), /* 对象指针 */ void *pData);
    /* 把head后的rightMoveLen与leftMoveLen交换,原始数据排布为rightMoveLen+leftMoveLen,移动后变为leftMoveLen+rightMoveLen */
    void fs_memery_switch(void *head, /* head这片空间的大小 */unsigned int rightMoveLen, /* 要移动的数据大小,应小于len */unsigned int leftMoveLen);
   
#ifdef __cplusplus
}
#endif

#endif /* MEMERY_H */


