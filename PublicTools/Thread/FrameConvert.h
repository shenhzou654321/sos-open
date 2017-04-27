/* 
 * File:   FrameConvert.h
 * Author: fslib
 *
 * Created on 2013年7月19日, 上午9:11
 */

#ifndef FRAMECONVERT_H
#define	FRAMECONVERT_H
#include "../Fs/ObjectList.h"
#include "../Image/ImageFrame.h"
#ifdef	__cplusplus
extern "C" {
#endif
    struct FrameConvert_item;

    /* 
     * 多线程图像帧转换对象.
     */
    typedef struct {

        /* 
         * 结构约定:
         *     ro中为外部只读变量(仅限当前指针所指向的结构,内部指针所指向的内容是否可读由内部结构决定);
         *     rw中为外部可读写变量(仅限当前指针所指向的结构,内部指针所指向的内容是否可读写由内部结构决定);
         *     p中为私有变量,外部不可读也不可写;
         * 变量命名约定:
         *     凡需要(非0)初始化的变量,都要加上_的前缀;
         *     凡需要内存释放的变量,都要加上__的前缀. 
         */
        struct {
            /* 线程名 */
            char *_threadName;
            /* 监控的线程指针对象,开始线程后把创建的监控节点添加到此对象中 */
            void *_pMonitor;
            /* 父线程的线程号,由systemThreadTid获得,为0表示无父线程 */
            pthread_t _parentTid;
            /* 多线程共有的运行状态,如果此值为空,则引用默认的全局运行状态变量,本线程的运行状态只有与此值相同才能运行 */
            signed char *_externRunStatus;
            /* ThreadSocketIPv4线程的tid号 */
            pthread_t *_stid;
            /* 添加或删除连接项(0:没有动作;1:添加;2:删除) */
            unsigned char changeItem : 2;
            /* 线程同步锁,仅用于添加或删除连接项 */
            pthread_mutex_t __mutex;
            /* 同时开启的线程数,最小值为1,填0会死机 */
            unsigned short _threadCount;
            /* 各线程可处理的转换项的个数 */
            unsigned short *_threadItemCount;
            /* 线程是否在运行(0:没有运行,1:在运行,2:在运行但同时在接受外部的增删操作),纬数与线程数相同 */
            unsigned char *_inRun;
            /* 转换的条目链表 */
            FsObjectList *__itemList;
        } ro; /* 初始化时为可写全过程可读区 */

        /* 全过程只读区,写可能会出错 */
        struct {
            /* 运行状态:
             *     与_externRunStatus指向值相同-运行(一般此值为1);
             *     0-停止(手动退出后的状态值);
             *     2-重启(不一定实现);
             *     大于127-引用此值的所有线程都会退出;
             * 引用的指针类型必须是(char*);
             * 引用此值的其他线程应实现当自己的运行状态值[0-255]与此值[-128-127]不相等时退出.
             */
            unsigned char runStatus;
        } rw; /* 全过程只读区,写可能会出错 */
    } FrameConvert;

    /* 
     * 转换项
     * 解码流:p-->
     *     decodeList[0]-->encodeList[0][0]-->encodeList[0][1]-->...-->encodeList[0][n]-->
     *     decodeList[1]-->encodeList[1][0]-->encodeList[1][1]-->...-->encodeList[1][n]-->
     *     decodeList[2]-->encodeList[2][0]-->encodeList[2][1]-->...-->encodeList[2][n]
     */
    struct FrameConvert_item {

        /* 
         * 结构约定:
         *     ro中为外部只读变量(仅限当前指针所指向的结构,内部指针所指向的内容是否可读由内部结构决定);
         *     rw中为外部可读写变量(仅限当前指针所指向的结构,内部指针所指向的内容是否可读写由内部结构决定);
         *     p中为私有变量,外部不可读也不可写;
         * 变量命名约定:
         *     凡需要(非0)初始化的变量,都要加上_的前缀;
         *     凡需要内存释放的变量,都要加上__的前缀. 
         */
        struct {
            /* 转换后的数据类型,与ImageFrame中定义相同 */
            unsigned long _convertType;
            /* 同时开启的线程数,最小值为1,填0会死机,必须与要加入的FrameConvert相同 */
            unsigned short _threadCount;
            /* 转换后待传出的图像帧链表 */
            FsObjectList * __framelistOut;
        } ro;

        struct {
            /* 数据进入方式(0--正序,1-倒序) */
            unsigned char mode : 1;
            /* 处理速度的参考值,主要用于视频播放 */
            float speed;
        } rw;

        struct {
            /* 在添加到转换库时会指定本转换项从转换库的哪个下标开始 */
            unsigned short startIndex;
            /* 再添加到FrameConvert后会把此值赋为pFrameConvert,移除时置NULL */
            FrameConvert *pFrameConvert;
            /* 当前加入的解码线程 */
            unsigned short threadIndex_decode[3];
            /* 解码的图像帧链表,纬数必须与要加入的FrameConvert相同 */
            FsObjectList *(*__decodeList)[3];
            /* 解码句柄,纬数必须与要加入的FrameConvert相同 */
            void* (*__decodeHandle)[3];
            /* 删除解码句柄的方法,纬数必须与要加入的FrameConvert相同 */
            void (*decodeHandle_delete[3])(void *);
            /* 当前264编码线程加入的帧数 */
            unsigned short thread_inCount_h264[3];
            /* h264编码的图像帧链表,纬数必须与要加入的FrameConvert相同 */
            FsObjectList *(*__encodeList_h264)[3];
            /* h264编码句柄,纬数必须与要加入的FrameConvert相同 */
            void* (*__encodeHandle_h264)[3];
        } p;
    };
    /* 
     * 创建一个新的FrameConvert多线程图像帧转换对象;
     * 返回FrameConvert指针.
     */
    FrameConvert * frameConvert_new__IO(/* 线程名 */const char threadName[],
            /* 监控的线程指针对象,开始线程后把创建的监控节点添加到此对象中 */ void *const pMonitor,
            /* 父线程的线程号,由systemThreadTid获得,为0表示无父线程 */ const pthread_t parentTid,
            /* 多线程共有的运行状态,如果此值为空,则引用默认的全局运行状态变量,本线程的运行状态只有与此值相同才能运行 */signed char *const externRunStatus,
            /* 同时开启的线程数,最小值为1,填0会死机 */ const unsigned short threadCount);
    /* 删除pFrameConvert指向的实例对象 */
    void frameConvert_delete__IO(FrameConvert * const pFrameConvert);
    /* 开始线程,必须先设置好初始化时为可写全过程可读区的所有参数 */
    void frameConvert_startThread(FrameConvert * const pFrameConvert,
            /* 线程分时优先级,有效值为[-1-99],越大优先级越高,0表示不改变优先级,继承父线程的优先级,-1-表示强制线程的调度模式为分时调度策略,正数强制线程的调度模式为实时分时调度策略 */const signed char nice);
    /* 结束线程 */
    void frameConvert_stopThread(FrameConvert * const pFrameConvert);
    /* 
     * 创建一个连接项;
     * 返回FrameConvert_item指针.
     */
    struct FrameConvert_item *frameConvert_item_new__OI(/* 转换后的数据类型,与ImageFrame中定义相同 */ const unsigned long convertType,
            /* 同时开启的线程数,最小值为1,填0会死机,建议不要大于要加入的FrameConvert中的线程数 */ const unsigned short threadCount);
    /* 删除pFrameConvert指向的实例对象 */
    void frameConvert_item_delete__IO(struct FrameConvert_item * const pFrameConvert_item);
    /* 向pFrameConvert中添加一个转换项 */
    void frameConvert_add_item__OI_2(FrameConvert * const pFrameConvert, struct FrameConvert_item * const pFrameConvert_item);
    /* 从pFrameConvert中移除一个转换项 */
    void frameConvert_remove_item__IO_2(FrameConvert * const pFrameConvert, struct FrameConvert_item * const pFrameConvert_item);
    /* 传入数据帧 */
    void frameConvert_item_frame_in_pthreadSafety_OI(/* 转换项,不可为空 */ struct FrameConvert_item * const pFrameConvert_item, FsObjectImageFrame * const pFrame);
    /* 传出数据帧,有数据返回FsObjectImageFrame指针,无数据返回NULL */
    FsObjectImageFrame *frameConvert_item_frame_out_pthreadSafety_IO(/* 转换项,不可为空 */ struct FrameConvert_item * const pFrameConvert_item);
    /* 检查是否有数据可传出,有数据返回1,无返回-1 */
    signed char frameConvert_item_frame_out_countCheck_pthreadSafety(/* 转换项,不可为空 */ struct FrameConvert_item * const pFrameConvert_item);
    /* 清空转换项中的所有数据帧 */
    void frameConvert_item_clean(/* 转换项,不可为空 */ struct FrameConvert_item * const pFrameConvert_item);
#ifdef	__cplusplus
}
#endif

#endif	/* FRAMECONVERT_H */

