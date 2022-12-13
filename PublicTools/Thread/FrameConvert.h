/* 
 * File:   FrameConvert.h
 * Author: fslib
 *
 * Created on 2013年7月19日, 上午9:11
 */

#if !defined FRAMECONVERT_H && !defined JAVA_IN_C

#define FRAMECONVERT_H
#include "../../PublicTools/publicDefine.h"

#include "../../PublicTools/Fs/ObjectList.h"
#include "../../PublicTools/Image/ImageFrame.h"
#ifdef __cplusplus
extern "C" {
#endif
    struct FrameConvert_item;

    /* 
     * 多线程图像帧转换对象.
     */
    typedef struct {

        /* 
         * 结构约定:
         *     r中为外部可读变量(仅限当前指针所指向的结构,内部指针所指向的内容是否可读由内部结构决定,即r中的变量值在赋值后可能会修改),在集群应用时,用于存放公共变量;
         *     ro中为外部只读变量(仅限当前指针所指向的结构,内部指针所指向的内容是否可读由内部结构决定,即ro中的变量值在赋值后不会修改);
         *     rw中为外部可读写变量(仅限当前指针所指向的结构,内部指针所指向的内容是否可读写由内部结构决定,即rw中的变量值可在外部及内部直接修改,如是指针类变量应提供锁机制);
         *     p中为私有变量,外部不可读也不可写,(即p中的变量可能会被内部修改,对指针类变量,外部不能通过其指针直接访问指针指向的数据);
         * 变量命名约定:
         *     结构中所有变量名在ro,rw,p中要唯一,如不能出现ro中的变量与rw中的变量同名;
         *     变量名不能使用_前缀后缀区分不同变量名,__为分隔符,分隔符后的不作为变量名,如_aa_bb__cc表示aa_bb变量;
         *     凡需要(非0)初始化的变量,都要加上_的前缀;
         *     凡需要内存释放的变量,都要加上__的前缀;
         *     整形变量使用_结尾表示变量值为相对结构体尾部的一个偏移量,最大值表示无效值;
         *     指针变量使用_结尾表示变量指向的对象丧失安全设置,如需要使用其他变量的安全设置应使用__连接其他变量名,如本变量aa使用bb变量的安全设置,应命名为aa__bb.
         */
        struct {
            /* 监控的线程指针对象,开始线程后把创建的监控节点添加到此对象中 */
            void *_pMonitor;
            /* 多线程共有的运行状态,如果此值为空,则引用默认的全局运行状态变量,本线程的运行状态只有与此值相同才能运行 */
            signed char *_externRunStatus;
            /* 父线程的线程号,由systemThreadTid获得,为0表示无父线程 */
            pthread_t _parentTid;
            /* 线程名 */
#define FrameConvert_get_threadName(___pFrameConvert) (((char*)((___pFrameConvert)+1))+(___pFrameConvert)->ro._threadName_)
            unsigned char _threadName_;
            /* ThreadSocketIPv4线程的tid号 */
            pthread_t *_stid;
            /* 添加或删除连接项(0:没有动作;1:添加;2:删除;3:清空连接数据) */
            unsigned char changeItem : 2;
            /* 线程同步锁,仅用于添加或删除连接项 */
            pthread_mutex_t __mutex;
            /* 同时开启的线程数,最小值为1,填0会死机 */
            unsigned short _threadCount;
            /* 各线程可处理的转换项的个数 */
            unsigned int *_threadItemCount;
            /* 线程是否在运行(0:没有运行,1:在运行,2:在运行但同时在接受外部的增删操作),纬数与线程数相同 */
            unsigned char *_inRun;
            /* 转换的条目链表 */
            FsObjectList *__itemList;
            /* 真正需要转换的条目链表,在有数据传入时,把FrameConvert_item中的_convertType不为0的添加到此链表中 */
            FsObjectList *__itemListConvert;
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
#define __FrameConvertLog(___Log,___loglevel,___format, ...)\
    ___Log(___loglevel,5,___format",p=%p,runStatus=%hhd/%hhu,tid=%p/%ld\n  pMonitor=%p\n  externRunStatus=%p\n  parentTid=%ld\n  threadName:\"%s\"\n\
    changeItem=%hhu\n  threadCount=%hu\n  threadItemCount=%p/%hu\n  inRun=%p/%hhu\n  itemList=%p/%lu\n  itemListConvert=%p/%lu\n",##__VA_ARGS__\
        ,pFrameConvert,((FrameConvert*)(pFrameConvert))->rw.runStatus,((FrameConvert*)(pFrameConvert))->ro._inRun[0],((FrameConvert*)(pFrameConvert))->ro._stid,((FrameConvert*)(pFrameConvert))->ro._stid[0],((FrameConvert*)(pFrameConvert))->ro._pMonitor\
        ,((FrameConvert*)(pFrameConvert))->ro._externRunStatus,((FrameConvert*)(pFrameConvert))->ro._parentTid,FrameConvert_get_threadName(pFrameConvert)\
        ,((FrameConvert*)(pFrameConvert))->ro.changeItem,((FrameConvert*)(pFrameConvert))->ro._threadCount\
        ,((FrameConvert*)(pFrameConvert))->ro._threadItemCount,((FrameConvert*)(pFrameConvert))->ro._threadItemCount[0]\
        ,((FrameConvert*)(pFrameConvert))->ro._inRun,((FrameConvert*)(pFrameConvert))->ro._inRun[0]\
        ,((FrameConvert*)(pFrameConvert))->ro.__itemList,((FrameConvert*)(pFrameConvert))->ro.__itemList->nodeCount\
        ,((FrameConvert*)(pFrameConvert))->ro.__itemListConvert,((FrameConvert*)(pFrameConvert))->ro.__itemListConvert->nodeCount)
#define FrameConvertLog(___loglevel,___format, ...) __FrameConvertLog(FsLog,___loglevel,___format,##__VA_ARGS__)
#define FrameConvertLog2(___loglevel,___format, ...) __FrameConvertLog(FsLog2,___loglevel,___format,##__VA_ARGS__)

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
         *     r中为外部可读变量(仅限当前指针所指向的结构,内部指针所指向的内容是否可读由内部结构决定,即r中的变量值在赋值后可能会修改),在集群应用时,用于存放公共变量;
         *     ro中为外部只读变量(仅限当前指针所指向的结构,内部指针所指向的内容是否可读由内部结构决定,即ro中的变量值在赋值后不会修改);
         *     rw中为外部可读写变量(仅限当前指针所指向的结构,内部指针所指向的内容是否可读写由内部结构决定,即rw中的变量值可在外部及内部直接修改,如是指针类变量应提供锁机制);
         *     p中为私有变量,外部不可读也不可写,(即p中的变量可能会被内部修改,对指针类变量,外部不能通过其指针直接访问指针指向的数据);
         * 变量命名约定:
         *     结构中所有变量名在ro,rw,p中要唯一,如不能出现ro中的变量与rw中的变量同名;
         *     变量名不能使用_前缀后缀区分不同变量名,__为分隔符,分隔符后的不作为变量名,如_aa_bb__cc表示aa_bb变量;
         *     凡需要(非0)初始化的变量,都要加上_的前缀;
         *     凡需要内存释放的变量,都要加上__的前缀;
         *     整形变量使用_结尾表示变量值为相对结构体尾部的一个偏移量,最大值表示无效值;
         *     指针变量使用_结尾表示变量指向的对象丧失安全设置,如需要使用其他变量的安全设置应使用__连接其他变量名,如本变量aa使用bb变量的安全设置,应命名为aa__bb.
         */
        struct {
            /* 转换后待传出的图像帧链表 */
            FsObjectList * __framelistOut;
            /* 转换后的数据类型,与ImageFrame中定义相同,在_convertType为0时只分配FrameConvert_item_lite大小的空间 */
            unsigned long long _convertType;
            /* 码率(h264,h265) */
            unsigned int _kbps;
            /* 关键帧间隔(h264,h265),包含关键帧,即1表示每帧都为关键帧 */
            unsigned char _keyInterval;
        } ro;

        struct {
            /* 数据进入方式(0-正序,1-倒序) */
            unsigned char mode : 1;
            /* 处理速度的参考值,主要用于视频播放 */
            float speed;
        } rw;

        struct {
            /* 再添加到FrameConvert后会把此值赋为pFrameConvert,移除时置NULL */
            FrameConvert *pFrameConvert;
#define FrameConvert_item_lite FsPointer_to_long(&((struct FrameConvert_item*) 0)->p._cb_convert) // 在convertType为0时只分配FrameConvert_item_lite大小的空间
            /* 在内部进行了转换后,会调用此回调函数,可为空 */
            void (*_cb_convert)(void *p);
            /* 调用cb_convert函数时传入的外部指针 */
            void *_cb_convertP;
            /* 在convertType不为0时标记是否已添加到pFrameConvert->ro.__itemListConvert,使用__framelistOut的互斥锁,0-未添加,1-已添加 */
            unsigned char addToItemListConvert__framelistOut;
            /* 同时开启的线程数,最小值为1,填0会死机,必须与要加入的FrameConvert相同 */
            unsigned short _threadCount;
            /* 在添加到转换库时会指定本转换项从转换库的哪个下标开始 */
            unsigned short startIndex;
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
            /* 当前265编码线程加入的帧数 */
            unsigned short thread_inCount_h265[3];
            /* h265编码的图像帧链表,纬数必须与要加入的FrameConvert相同 */
            FsObjectList *(*__encodeList_h265)[3];
            /* h265编码句柄,纬数必须与要加入的FrameConvert相同 */
            void* (*__encodeHandle_h265)[3];
        } p;
    };
#define __FrameConvert_itemLog(___Log,___loglevel,___format, ...)\
    ___Log(___loglevel,5,___format",p=%p/%p,framelistOut=%p/%lu,convertType=%llx,mode=%hhd,speed=%f\n",##__VA_ARGS__\
        ,pFrameConvert_item,pFrameConvert_item->p.pFrameConvert,pFrameConvert_item->ro.__framelistOut,pFrameConvert_item->ro.__framelistOut->nodeCount\
        ,pFrameConvert_item->ro._convertType,pFrameConvert_item->rw.mode,pFrameConvert_item->rw.speed)
#define FrameConvert_itemLog(___loglevel,___format, ...) __FrameConvert_itemLog(FsLog,___loglevel,___format,##__VA_ARGS__)
#define FrameConvert_itemLog2(___loglevel,___format, ...) __FrameConvert_itemLog(FsLog2,___loglevel,___format,##__VA_ARGS__)

    /* 
     * 创建一个新的FrameConvert多线程图像帧转换对象;
     * 返回FrameConvert指针.
     */

    FrameConvert * frameConvert_new__IO(/* 监控的线程指针对象,开始线程后把创建的监控节点添加到此对象中 */ void *const pMonitor
            , /* 多线程共有的运行状态,如果此值为空,则引用默认的全局运行状态变量,本线程的运行状态只有与此值相同才能运行 */signed char *const externRunStatus
            , /* 父线程的线程号,由systemThreadTid获得,为0表示无父线程 */ const pthread_t parentTid, /* 线程名 */const char threadName[]
            , /* 同时开启的线程数,最小值为1,填0会死机 */ const unsigned short threadCount);

    /* 删除pFrameConvert指向的实例对象 */

    void frameConvert_delete__OI(FrameConvert * const pFrameConvert);

    /* 开始线程,必须先设置好初始化时为可写全过程可读区的所有参数 */

    void frameConvert_startThread(FrameConvert * const pFrameConvert
            , /* 线程分时优先级,有效值为[-1-99],越大优先级越高,0表示不改变优先级,继承父线程的优先级,-1-表示强制线程的调度模式为分时调度策略,正数强制线程的调度模式为实时分时调度策略 */const signed char thisNice);

    /* 结束线程 */

    void frameConvert_stopThread(FrameConvert * const pFrameConvert);

    /* 
     * 创建一个连接项;
     * 返回FrameConvert_item指针.
     */

    struct FrameConvert_item *frameConvert_item_new__IO(/* 转换后的数据类型,与ImageFrame中定义相同 */ const unsigned long convertType
            , /* 同时开启的线程数,最小值为1,填0会死机,建议不要大于要加入的FrameConvert中的线程数 */ const unsigned short threadCount
            , /* 关键帧间隔(h264,h265),包含关键帧,即1表示每帧都为关键帧 */ const unsigned char keyInterval, /* 码率(h264,h265) */ const unsigned int kbps
            , /* 在内部进行了转换后,会调用此回调函数,可为空 */ void (*const cb_convert) (void *p), /* 调用cb_convert函数时传入的外部指针 */ void *const cb_convertP);

    /* 删除pFrameConvert指向的实例对象 */

    void frameConvert_item_delete__OI(struct FrameConvert_item * const pFrameConvert_item);

    /* 向pFrameConvert中添加一个转换项 */

    void frameConvert_add_item__OI_2(FrameConvert * const pFrameConvert, struct FrameConvert_item * const pFrameConvert_item);

    /* 从pFrameConvert中移除一个转换项 */

    void frameConvert_remove_item__IO_2(FrameConvert * const pFrameConvert, struct FrameConvert_item * const pFrameConvert_item);

    /* 传入数据帧 */

    void frameConvert_item_frame_in_pthreadSafety__OI_2(/* 转换项,不可为空 */ struct FrameConvert_item * const pFrameConvert_item, FsObjectImageFrame * const pFrame);

    /* 传出数据帧,把数据帧的指针放在ppFrame数组中,返回获取到的数据帧数量 */

    unsigned int frameConvert_item_frame_out_pthreadSafety__IO_4(/* 转换项,不可为空 */ struct FrameConvert_item * const pFrameConvert_item
            , /* ppFrame可储存的数据帧数量 */unsigned int frameCount, FsObjectImageFrame * ppFrame[]);

    /* 检查是否有数据可传出,有数据返回1,无返回-1 */

    signed char frameConvert_item_frame_out_countCheck_pthreadSafety(/* 转换项,不可为空 */ struct FrameConvert_item * const pFrameConvert_item);

    /* 清空转换项中的所有数据帧 */

    void frameConvert_item_clean(/* 转换项,不可为空 */ struct FrameConvert_item * const pFrameConvert_item);


#ifdef __cplusplus
}
#endif

#endif /* FRAMECONVERT_H */

