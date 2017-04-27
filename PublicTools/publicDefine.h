/* 
 * File:   publicDefine.h
 * Author: fslib
 *
 * Created on 2013年1月30日, 下午3:11
 */
#ifndef PUBLICDEFINE_H
#define	PUBLICDEFINE_H
////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////通用定义 开始///////////////////////////////////
//#define WINDOWS
#ifdef WINDOWS
//#define WINDOWSVC
//#define MinGW
#endif
//#define ARM
#define FsDebug
////////////////////////////////////////////////////////////////////////////////
#ifdef WINDOWS
#define systemThreadTid pthread_self() //syscall(186)
#define UsePollForEpoll
#ifdef MinGW
#define UseSelectForEpoll
#endif
#else
#define systemThreadTid  syscall(186)
/* 32位 */
//#define systemThreadTid  syscall(224)
#endif
/* 内存对齐,不理解不要使用 */
#if defined WINDOWS && defined __x86_64__
#define Memery_Alignment(l) (((long long)((l)+0x7LL))&(~0x7LL))
#else
#define Memery_Alignment(l) (((long)((l)+0x7L))&(~0x7L))
#endif
#define Memery_Alignment4(l) (((unsigned int)((l)+0x3U))&(~0x3U))
///* 强制使用POLL */
//#ifndef UsePollForEpoll
//#ifndef MinGW
//#define UsePollForEpoll
//#endif
//#endif
/* 强制使用select */
//#ifdef UsePollForEpoll
//#ifndef UseSelectForEpoll
//#define UseSelectForEpoll
//#endif
//#endif
#ifdef	__cplusplus
extern "C" {
#endif
    unsigned int __get_printfTime();
#ifdef MinGW
#include <process.h>
#endif
#ifndef MinGW1
#ifdef MinGW
#define getppid() 0
#endif
#define FsPrintf(index,__format, ...) printf("\33[36m%d\33[33m%05d\33[36m/\33[35m%010u\33[36m|\33[35m%d\33[36m/\33[35m%d\33[36m(\33[33m%s\33[36m)\33[35m--\33[31m" __format"\33[0m",index,__LINE__,__get_printfTime(),getpid(),getppid(),__FUNCTION__, ##__VA_ARGS__) 
    //#define FsPrintf(index,__format, ...) printf("\33[36m%d\33[33m%05d\33[36m/\33[35m%010u\33[36m|\33[35m%d\33[36m/\33[35m%d\33[36m(\33[33m%s\33[35m->\33[33m%s\33[36m)\33[35m--\33[31m" __format"\33[0m",index,__LINE__,__get_printfTime(),getpid(),getppid(),__FILE__,__FUNCTION__, ##__VA_ARGS__) 
#else
#define FsPrintf(index,__format, ...) printf("%d%05d/%010u|%d/%d(%s)--" __format,index,__LINE__,__get_printfTime(),getpid(),0,__FUNCTION__, ##__VA_ARGS__) 
#endif
#ifndef MinGW1
#define FsPrintf2(index,__format, ...) printf("\33[37m%d\33[36m%05d\33[33m/\33[35m%010u\33[33m|\33[35m%d\33[33m/\33[35m%d\33[33m[\33[36m%s\33[33m]\33[35m--\33[32m" __format"\33[0m",index,__LINE__,__get_printfTime(),getpid(),getppid(),__FUNCTION__, ##__VA_ARGS__)
    //#define FsPrintf2(index,__format, ...) printf("\33[37m%d\33[36m%05d\33[33m/\33[35m%010u\33[33m|\33[35m%d\33[33m/\33[35m%d\33[33m[\33[36m%s\33[35m->\33[36m%s\33[33m]\33[35m--\33[32m" __format"\33[0m",index,__LINE__,__get_printfTime(),getpid(),getppid(),__FILE__,__FUNCTION__, ##__VA_ARGS__)
#else
#define FsPrintf2(index,__format, ...) printf("%d%05d/%010u|%d/%d[%s]--" __format,index,__LINE__,__get_printfTime(),getpid(),0,__FUNCTION__, ##__VA_ARGS__)
#endif
#define FsPrintfTag "\33[36m%05d\33[33m/\33[35m%010u\33[0m"
#define FsPrintfOut __LINE__,__get_printfTime() 
#ifdef FsDebug
#define FsPrintfOut2 __LINE__,__get_printfTime() 
#else
#define FsPrintfOut2 0,0U
#endif   
#ifndef MinGW
#define closesocket close
#else
#endif
#pragma pack(push,1)

    /* 内存友好型数组,这仅的一个概念,须各函数自己实现,要求在对象用完后能只用一次free就能释放所有空间 */
    typedef struct {
        /* 数组的个数 */
        unsigned int count;
        /* 指针数组,可储存任意类型的变量,要求array[i]指向的对象内存连续 */
        char *data[];
    } FsArray;
#pragma pack(pop)
    ///////////////////////////通用定义 结束/////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////
    ///////////////////////////定义全局变量开始//////////////////////////////////
    /* 如果线程没有引用其它线程的状态,则都引用此值,当自己的状态与此值不同时应退出线程,引用的指针类型必须是(signed char*) */
    extern signed char defaultRunStatus;
    ///////////////////////////定义全局变量结束//////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////
    ///////////////////////////公共变量的内存操作 开始///////////////////////////
    /* 
     * 公共变量的内存操作,及标准输出和错误的重定向;
     * 主要用完成一些在程序中要用到的函数所对应的表的初始化,主要与优化相关;
     * 必须在main函数开始处中调用. 
     */
    void globalInit(/* 在缓存中没找到要分配的大小的内存时,检查缓存是否超过maxMemeryBufferSize,超过则释放一定的缓存 */const unsigned long maxMemeryBufferSize,
            /* 标准输入重定向,为空表示不重定向 */ const char stdinFile[], /* 标准输出重定向,为空表示不重定向 */ const char stdoutFile[], /* 标准错误重定向,为空表示不重定向 */ const char stderrFile[],
            /* 重定向的模式,redirectMode&0x1不为0表示只在标准输入无效时才重定向;redirectMode&0x2不为0表示只在标准输出无效时才重定向;redirectMode&0x4不为0表示只在标准错误无效时才重定向 */const unsigned char redirectMode);

    /* 
     * 释放已申请的公共变量;
     * 主要用完成一些在程序中要用到的函数所对应的表的内存释放,主要与优化相关;
     * 必须在main函数结束处中调用. 
     */
    void globalRelease();
    ////////////////////////////////////////////////////////////////////////////
#ifdef UsePollForEpoll    

#include <stdint.h>

    /* Flags to be passed to epoll_create2.  */
    enum {
        EPOLL_CLOEXEC = 02000000,
#define EPOLL_CLOEXEC EPOLL_CLOEXEC
        EPOLL_NONBLOCK = 04000
#define EPOLL_NONBLOCK EPOLL_NONBLOCK
    };

    enum EPOLL_EVENTS {
        EPOLLIN = 0x001,
#define EPOLLIN EPOLLIN
        EPOLLPRI = 0x002,
#define EPOLLPRI EPOLLPRI
        EPOLLOUT = 0x004,
#define EPOLLOUT EPOLLOUT
        EPOLLRDNORM = 0x040,
#define EPOLLRDNORM EPOLLRDNORM
        EPOLLRDBAND = 0x080,
#define EPOLLRDBAND EPOLLRDBAND
        EPOLLWRNORM = 0x100,
#define EPOLLWRNORM EPOLLWRNORM
        EPOLLWRBAND = 0x200,
#define EPOLLWRBAND EPOLLWRBAND
        EPOLLMSG = 0x400,
#define EPOLLMSG EPOLLMSG
        EPOLLERR = 0x008,
#define EPOLLERR EPOLLERR
        EPOLLHUP = 0x010,
#define EPOLLHUP EPOLLHUP
        EPOLLRDHUP = 0x2000,
#define EPOLLRDHUP EPOLLRDHUP
        EPOLLONESHOT = (1 << 30),
#define EPOLLONESHOT EPOLLONESHOT
        EPOLLET = (1 << 31)
#define EPOLLET EPOLLET
    };
#define EPOLL_CTL_ADD 1	/* Add a file decriptor to the interface.  */
#define EPOLL_CTL_DEL 2	/* Remove a file decriptor from the interface.  */
#define EPOLL_CTL_MOD 3	/* Change file decriptor epoll_event structure.  */

    /* Windows下没有epoll */
    typedef union epoll_data {
        void *ptr;
        int fd;
        uint32_t u32;
        uint64_t u64;
    } epoll_data_t;

    struct epoll_event {
        uint32_t events; /* Epoll events */
        epoll_data_t data; /* User data variable */
    } __attribute__((__packed__));
    /* 创建一个epoll,返回值是一个地址,必须用long long来储存,函数不会失败.linux下的可用int储存 */
    extern long long epoll_create1(int __flags);
    extern int epoll_ctl(long long __epfd, int __op, int __fd, struct epoll_event *__event);
    extern int epoll_wait(long long __epfd, struct epoll_event *__events, int __maxevents, int __timeout);
    /* linux下没有此函数,是用close关闭epoll的 */
    extern void epoll_delete(long long __epfd);
#else
#define epoll_delete(__epfd) close(__epfd)
#endif
    ///////////////////////////公共变量的内存操作 结束////////////////////////////
    ////////////////////////////////////////////////////////////////////////////
#ifdef	__cplusplus
}

#endif

#endif	/* PUBLICDEFINE_H */