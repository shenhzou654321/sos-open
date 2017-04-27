/* 
 * File:   encode.h
 * Author: fslib
 *
 * Created on 2016年3月24日, 上午11:19
 */

#ifndef ENCODE_H
#define	ENCODE_H
#include "../../PublicTools/publicDefine.h"
#include "../../PublicTools/Fs/Config.h"
#include "../../PublicTools/Fs/TypeFace.h"
#include "../../PublicTools/Thread/FrameConvert.h"
#ifdef	__cplusplus
extern "C" {
#endif

    /* 编码 */
    struct Encode {

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
            pthread_t stid;
            /* 配置管理对象 */
            void *_pConfigManager;
            /* 通道链表,节点仅在控制线程可读 */
            FsObjectList *__itemList;
        } ro;

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
            /* 线程分时优先级,有效值为[0-99],越大优先级越高,0表示不改变优先级,此值会影响到子线程,再开始线程时会设置此值为用本线程的优先级 */
            unsigned char nice;
            /* 重置通道,负数表示重置完成 */
            int resetChannel;
        } rw;

        struct {
        } p;
    };

    /* 编码项 */
    struct Encode_item {

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
            /* 图像宽度,0为无效值 */
            unsigned int imageWidth;
            /* 图像高度,0为无效值 */
            unsigned int imageHeight;
            /* 分类号,不同的分类号表示数据已无关系,与之相关的调用须重新初始化 */
            unsigned short classIndex;
            /* 水印开始位置的X坐标 */
            unsigned short _watermarking_startX;
            /* 水印开始位置的Y坐标 */
            unsigned short _watermarking_startY;
            /* 水印颜色,RGB */
            unsigned int _watermarking_color;
            /* 水印字符 */
            char *_watermarkingData;
            /* 直正的解码对象 */
            struct FrameConvert_item *__pFrameConvert_item;
            /* 文字叠加 */
            FsTypeFace *__pTypeFace;
        } ro;

        struct {
        } rw;

        struct {
        } p;
    };
    /* 创建配置 */
    void encode_createConfig(FsConfig * const pConfig, /* 掩码 */const unsigned long long mask, /* 通道数 */const unsigned int channelCount,void * parent);
    /* 
     * 创建一个新的Encode对象;
     * 返回Encode指针.
     */
    struct Encode *encode_new__IO(/* 线程名 */const char threadName[],
            /* 监控的线程指针对象,开始线程后把创建的监控节点添加到此对象中 */ void *const pMonitor,
            /* 父线程的线程号,由systemThreadTid获得,为0表示无父线程 */ const pthread_t parentTid,
            /* 多线程共有的运行状态,如果此值为空,则引用默认的全局运行状态变量,本线程的运行状态只有与此值相同才能运行 */signed char *const externRunStatus,
            /* 配置管理对象 */ void *const pConfigManager);
    /* 删除pEncode指向的实例对象 */
    void encode_delete__OI(struct Encode * const pEncode);
    /* 开始线程,必须先设置好初始化时为可写全过程可读区的所有参数 */
    void encode_startThread(struct Encode * const pEncode,
            /* 线程分时优先级,有效值为[-1-99],越大优先级越高,0表示不改变优先级,继承父线程的优先级,-1-表示强制线程的调度模式为分时调度策略,正数强制线程的调度模式为实时分时调度策略 */const signed char nice);
    /* 结束线程 */
    void encode_stopThread(struct Encode * const pEncode);
    /* 传入数据帧 */
    void encode_item_frame_in_pthreadSafety_OI(/* 编码项,可为空 */ struct Encode_item * const pEncode_item, FsObjectImageFrame * const pFrame);
    /* 传出数据帧,有数据返回FsObjectImageFrame指针,无数据返回NULL */
    FsObjectImageFrame *encode_item_frame_out_pthreadSafety_IO(/* 解码项,可为空 */struct Encode_item * const pEncode_item);
#ifdef	__cplusplus
}
#endif

#endif	/* ENCODE_H */

