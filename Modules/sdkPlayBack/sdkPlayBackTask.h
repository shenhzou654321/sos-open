/* 
 * File:   sdkPlayBackTask.h
 * Author: fslib
 *
 * Created on 2013年6月15日, 上午11:19
 */

#if !defined SDKPLAYBACKTASK_H && !defined JAVA_IN_C

#define SDKPLAYBACKTASK_H
#include "../../PublicTools/publicDefine.h"

#include "../../PublicTools/Fs/Memery.h"
#include "../../PublicTools/Fs/Object.h"
#include "../../PublicTools/Fs/StructList.h"
#include "../../PublicTools/Media/VideoMKV.h"
#ifdef __cplusplus
extern "C" {
#endif

#pragma pack(push,1)

#pragma pack(pop)
#define SdkPlayBackTask_Debug // 开启调试
    struct SdkPlayBackTaskNode;

    /* 继承FsObject回放任务结构,只能加到一个SdkPlayBackTask中 */

    typedef struct {
        FsObject object;

        /* 只能在SdkPlayBackTask中访问的变量 */
        struct {
            /* 储存关联的SdkPlayBackTaskNode节点按指针排序 */
            FsObjectList *taskNodeListOrder_;
            /* 已保存文件的最大开始时间(为小于等于0.0时在需要的优先级为0的最后一个文件时才赋值),可减小顺序播放的延时 */
            double hasSaveFileStartTimeMax;
            /* 当pTaskNode1收到新文件时拷贝数据到本任务的专用空间中再把此值赋值为pTaskNode2,置空pTaskNode2,如此值为空写相应的标记 */
            struct SdkPlayBackTaskNode *pTaskNode1;
            /* 当pTaskNode2收到新文件时拷贝数据到本任务的专用空间,再把此值置空 */
            struct SdkPlayBackTaskNode *pTaskNode2;
        } p;

        /* 数据就绪后向哪个文件写入就绪信息(满足条件的最后一个文件从网络开始写入时要写此tag但tag文件的内容为此文件的时间缀),如/vsys/tt/video/remote/v2020091514/qgo/5247850_0x7f49580721a0_/__tag */
        char *tag;
        /* 数据就绪后向tag写的数据内容,如aaaaealmamnfnhebaa */
        char *tagdata;
        /* 当前的速度,正数表示正向,负数表示逆向 */
        double speed;
        /* 当前播放的时间,0时区的时间 */
        double playTime;
        /* 当前使用的文件开始时间,在正序播放时,如最后一个文件就是当前文件,应该向后再准备一个文件,没有填0 */
        double thisFileStartTime;
#ifdef SdkPlayBackTask_Debug
        /* 上一次更新的时间 */
        double lastUptime;
#endif
        /* 回放任务的目录,以/结尾,如/vsys/tt/video/remote/v2020091514/qgo/5247850_0x7f49580721a0_/ */
        char dirName[];
    } FsObjectSdkPlayBackTask;
#define __FsObjectSdkPlayBackTaskLog(___Log,___loglevel,___format, ...)\
     ___Log(___loglevel,3,___format",p=%p/%d/%p/%lf/%p/%p,tag:\"%s\",tagdata:\"%s\",dirName:\"%s\",speed=%lf,playTime=%lf,thisFileStartTime=%lf.\n",##__VA_ARGS__\
        ,pTask,fs_ObjectBase_referCount_get(pTask),pTask->p.taskNodeListOrder_,pTask->p.hasSaveFileStartTimeMax,pTask->p.pTaskNode1,pTask->p.pTaskNode2,pTask->tag,pTask->tagdata,pTask->dirName,pTask->speed,pTask->playTime,pTask->thisFileStartTime)
#define FsObjectSdkPlayBackTaskLog(___loglevel,___format, ...) __FsObjectSdkPlayBackTaskLog(FsLog,___loglevel,___format,##__VA_ARGS__)
#define FsObjectSdkPlayBackTaskLog2(___loglevel,___format, ...) __FsObjectSdkPlayBackTaskLog(FsLog2,___loglevel,___format,##__VA_ARGS__)
    /*
     * 播放数据设计
     * 在有播放任务时,创建或更新多个播放任务节点
     * 一个视频流采用一个时间轴,时间轴上挂多个播放任务节点,一个节点属于多个播放任务
     * 如前一个任务节点正在取数据且前一个节点的数据可能与本节点重合,则本节点不能开始数据
     * 如本节点正在取数据,但取的数据不足规定的最小值,且前一的节点的数据可能与本节点重合,则前一节点不能开始取数据
     * 无数据自动衔接逻辑:
     *     if(当前节点已取到数据(已找到开始数据)){
     *         删除本节点创建的文件,包含连续标记;
     *     }
     *     关闭取流的句柄
     *     读取重新获取文件的开始时间节点(当前时间+一个文件时长SdkPlayBackTaskNode_priority2_len)
     *     重新取流;
     */

    /* 一个播放任务节点 */
    struct SdkPlayBackTaskNode {
        FsObjectSingle objectSingle;
        /* 所属的回放任务,不会增加FsObjectSdkPlayBackTask的引用计数,为空时表示本对象可删除了 */
        FsObjectList *taskListOrder_;
#define SdkPlayBackTaskNode_data_is_finished(___pTaskNode) ((___pTaskNode)->timeLen>0.0&&(NULL==(___pTaskNode)->pHandle||3==(___pTaskNode)->status)) // 判断pTaskNode的数据是否已完成
        /* 当前mkv对象,仅在status为为2时不为空,此对象不为空时,fd肯定不为空 */
        VideoMKV * pMKV;
        /* 当前写入文件的句柄(要使用读写模式打开),为空表示没有,在pMKV为空时timeLen不能为0.0,不为空时streamMaskFd不能为0 */
#define SdkPlayBackTaskNode_file_is_ready(___pTaskNode) ((___pTaskNode)->fd!=NULL&&(NULL==(___pTaskNode)->pHandle||3==(___pTaskNode)->status)) // 判断pTaskNode的文件是否已就绪
        FILE *fd;
        /* 回放的句柄,为空表示回放句柄未创建,真实空间附在本结构体之后,不为空时,status必须大于0 */
#define SdkPlayBackTaskNode_pHandle_memery(___pTaskNode) ((void*)(((struct SdkPlayBackTaskNode*)(___pTaskNode))+1))
        void *pHandle;
        /* 开始时间,创建时设置,中途可调整 */
        double startTime;
        /* 时长,可调整,时长不为0时,fd肯定不为空,且status不能为1,其长度不能小于一个精度 */
        double timeLen;
        /* 上一个文件的时间,用于网络数据获取时使用,不为0.0时网络句柄也不为0,非0.0指示本节点获取到数据后要写上一个文件的nextFileTime,且写硬盘的连续标记,时间大于0.0表示本节点与上一节点连续,0.0表示强制与上一个文件标记的连续(校验失败)
         * 此值不为0.0时,肯定存在上一个任务,且本节点的status不可为0,pHandle不可为空;
         * 此值不为0.0时,此值肯定为上一节点的时间(可能中相反数),上一节占的数据肯定是就绪状态;
         */
        double previorsFileTime;
        /* 下一个文件的开始时间,0.0表示没有下一个文件,非0表示连续,但负数表示校验是失败的
         * 此值不为空时,肯定存在下一个任务,且本节点的status为0,pHandle为空;
         */
        double nextFileTime;
        /* 当前文件可输出的条件(满足之一):
         *    1,当前任务的nextFileTime不为0(一般是从硬盘加载后本文件不是连续的最后一个文件);
         *    2,当前任务是由网络获取的(这样会立即产生下一个任务);
         *    3,当前任务的下一个任务的previorsFileTime指向当前任务(下一个任务与当前任务是连续的,但下一个任务还未完成,未生成连续标记);
         */
        /* 网络操作时的超时时间,开始建立连接时,时间为此时间的2倍 */
#define SdkPlayBackTaskNode_networkTimeOut 4.0
        double networkTimeOut;
#define SdkPlayBackTaskNode_get_fileTime(___fileTime) ((unsigned long long)(___fileTime)) // 根据开始时间获取文件名时间
#define SdkPlayBackTaskNode_get_fileTimeNextMin(___fileTime) ((unsigned long long)(___fileTime+1.0)) // 根据开始时间获取下一个文件名的最小时间
#define SdkPlayBackTaskNode_precision 2.0 // 匹配时的查找范围,必须为文件名最小差值的倍数
#define SdkPlayBackTaskNode_precision_align 1.0 // 定义后启用时间按SdkPlayBackTaskNode_precision对齐    
        /* 缓存的数据帧链表,在网络状态新建,不是网络状态时删除 */
#define SdkPlayBackTaskNode_frameList_init(___frameList) (___frameList)=fs_ObjectList_new__IO(5)
        FsObjectList *frameList;
        /* 上一次写数据后frameList内数据帧的量,应在变化时写数据帧 */
        unsigned int lastFrameListCount;
        /* 流的掩码,从本地文件读取的数据的掩码,也是写入掩码,fd不为空时streamMaskFd为空表示fd已校验通过(此情况仅发生在status为1的情况) */
        unsigned int streamMaskFd;
        /* 流的掩码,是下载数据的掩码,在pHandle为空时必须为0 */
        unsigned int streamMask;
#define SdkPlayBackTaskNode_priority0_len (SdkPlayBackTaskNode_precision*2.0) // 优先级为0的任务节点最少取的流长度,不做对齐处理,应大于SdkPlayBackTaskNode_precision
#define SdkPlayBackTaskNode_priority1_len (SdkPlayBackTaskNode_precision*2.0) // 优先级为1的任务节点最少取的流长度,不做对齐处理,1级数据不写盘,应大于SdkPlayBackTaskNode_precision
#define SdkPlayBackTaskNode_priority2_len (SdkPlayBackTaskNode_precision*30.0) // 优先级为2的任务节点最少取的流长度,对齐处理,但不能小于SdkPlayBackTaskNode_priority0_len的长度
#define SdkPlayBackTaskNode_ContinueLen (SdkPlayBackTaskNode_priority2_len*2.0) // 相邻两个任务节点间隔多远应续接
        /* 
         * 优先级
         *   偶数表示外部配置变化时创建使用的优先级;
         *   奇数为内部创建,或内部调整; 
         *   0-为最高优先级,最大值表示已没有播放任务关联了,对于优先级0在把数据拷贝到对应的FsObjectSdkPlayBackTask后,修改优先级为1
         *   已完成的后续创建使用的优先级为254;
         */
        unsigned char priority;
        /* 状态,0-未开始,1-校验开始数据中(此时数据长度必须为0.0),2-校验结束数据中(如数据校验完成则会变为暂停),3-暂停(本节点写数据结束,可能是校验完成,也可能是本节点不需要了,如优先级为0则要立即开始下一个任务节点) */
        unsigned char status : 2;
        /* 标记是否已从硬盘读取过数据 */
        unsigned char hasReadDisk : 1;
        /* 本节点的后续数据是否中断,0-未中断,1-中断(下一个节点与此节点的的结束时间不能小于最小分片时间+一个精度的时间),中断需要使用本文件名+".brk"的软连接在硬盘上标注
         *     在本任务网络数据出错后,确实无法建立网络数据做下一任务,本任务需要删除,前一标记为连续的任务要标记为一个中断点(以后不再尝试去校验数据上的连续性,在有与前面时差一个最小分片的任务时,设置其强制连续);
         *     任务出错时,应删除已写入硬盘的数据,对0级任务要清除硬连,使用视频信息向前(在未标记本任务与前一任务连续的情况下才向前查找)向后查找时间进行重定位;
         */
        unsigned char dataBreak : 1;
        /* 文件数据是否有建立硬连接,有在删除时应拷贝数据(如不拷贝数据,在复写数据时可能损坏具体播放任务中的数据导致流化进程出错),仅在状态为2时会出现数据建立硬连接的情况 */
        unsigned char dataLink : 1;
        /* 标记pHandle需要删除,1-删除(正在获取数据,但有优先级更高的任务需要处理,稍候从此处重新开始),2-因数据同下一个文件续接而需要删除(此时status必为3) */
        unsigned char pHandleNeedDelete : 2;
    };
#define __SdkPlayBackTaskNodeLog(___Log,___loglevel,___format, ...)\
     ___Log(___loglevel,3,___format",p=%p,taskListOrder_=%p/%lu,pMKV=%p,fd=%p,pHandle=%p/%hhu,startTime=%lf/%lf\
,previorsFileTime=%lf,nextFileTime=%lf,networkTimeOut=%lf,frameList=%p/%lu/%u\
,streamMask=%x/%x,priority=%hhu,status=%hhu,hasReadDisk=%hhu,dataLink=%hhu,dataBreak=%hhu.\n",##__VA_ARGS__\
        ,pTaskNode,pTaskNode->taskListOrder_,pTaskNode->taskListOrder_->nodeCount,pTaskNode->pMKV,pTaskNode->fd,pTaskNode->pHandle,pTaskNode->pHandleNeedDelete,pTaskNode->startTime,pTaskNode->timeLen\
        ,pTaskNode->previorsFileTime,pTaskNode->nextFileTime,pTaskNode->networkTimeOut,pTaskNode->frameList,pTaskNode->frameList?pTaskNode->frameList->nodeCount:0,pTaskNode->lastFrameListCount\
        ,pTaskNode->streamMaskFd,pTaskNode->streamMask,pTaskNode->priority,pTaskNode->status,pTaskNode->hasReadDisk,pTaskNode->dataLink,pTaskNode->dataBreak)
#define SdkPlayBackTaskNodeLog(___loglevel,___format, ...) __SdkPlayBackTaskNodeLog(FsLog,___loglevel,___format,##__VA_ARGS__)
#define SdkPlayBackTaskNodeLog2(___loglevel,___format, ...) __SdkPlayBackTaskNodeLog(FsLog2,___loglevel,___format,##__VA_ARGS__)

    /* 回放任务对象,整个时间轴
     * 工作流程:
     *     在增加回放任务时按期望增加SdkPlayBackTaskNode(仅设置期望时间与优先级)
     */

    struct SdkPlayBackTask {
        /* 具体的播放任务链表,节点为FsObjectSdkPlayBackTask,按dirName排序 */
        FsObjectList *taskListOrder_;
        /* 时间轴链表,节点为SdkPlayBackTaskNode按开始时间排序 */
        FsObjectList *taskNodeListOrder_;
    };
#define __SdkPlayBackTaskLog(___Log,___loglevel,___format, ...)\
     ___Log(___loglevel,3,___format",p=%p,taskListOrder_=%p/%lu,taskNodeListOrder_=%p/%lu.\n",##__VA_ARGS__\
        ,pSdkPlayBackTask,pSdkPlayBackTask->taskListOrder_,pSdkPlayBackTask->taskListOrder_->nodeCount,pSdkPlayBackTask->taskNodeListOrder_,pSdkPlayBackTask->taskNodeListOrder_->nodeCount)
#define SdkPlayBackTaskLog(___loglevel,___format, ...) __SdkPlayBackTaskLog(FsLog,___loglevel,___format,##__VA_ARGS__)
#define SdkPlayBackTaskLog2(___loglevel,___format, ...) __SdkPlayBackTaskLog(FsLog2,___loglevel,___format,##__VA_ARGS__)


    /* 
     * 创建一个新的FsObjectSdkPlayBackTask对象;
     * 返回FsObjectSdkPlayBackTask指针.
     */

    FsObjectSdkPlayBackTask * sdkPlayBackTask_object_new__IO(/* 回放任务的目录,如/vsys/tt/video/remote/v2020091514/qgo/5247850_0x7f49580721a0_/ */ const char dir[]
            , /* 数据就绪后向哪个文件写入就绪信息,如/vsys/tt/video/remote/v2020091514/qgo/5247850_0x7f49580721a0_/__tag */ const char tag[]
            , /* 数据就绪后向tag写的数据内容,如aaaaealmamnfnhebaa */ const char tagdata[]
            , /* 当前的速度,正数表示正向,负数表示逆向 */ const double speed, /* 当前播放的时间,0时区的时间 */ const double playTime, /* 当前使用的文件开始时间,在正序播放时,如最后一个文件就是当前文件,应该向后再准备一个文件,没有填0 */ const double thisFileStartTime);

    /* 向pTaskNode添加数据 */

    void sdkPlayBackTaskNode_addFrame__OI_2(struct SdkPlayBackTaskNode * const pTaskNode, /* 要添加的数据,支持FsObjectH264,必须设置"组轮询检测" */ FsObject * const pObject);

    /* 
     * 创建一个新的SdkPlayBackTask对象;
     * 返回SdkPlayBackTask指针.
     */

    struct SdkPlayBackTask *sdkPlayBackTask_new__IO();

    /* 删除pSdkPlayBackTask指向的实例对象 */

    void sdkPlayBackTask_delete__OI(struct SdkPlayBackTask * const pSdkPlayBackTask, /* 数据目录,如/vsys/aa/video/ */const char dirName[]
            , /* 删除一个回放句柄的方法 */ void (*const playBackHandle_delete__OI) (/* 回放句柄 */void *pPlayBackHandle)
            , /* 共享buffer,不可为空 */ FsShareBuffer * const pShareBuffer);

    /* 重置,把SdkPlayBackTaskNode的数据清空,重新加载 */

    void sdkPlayBackTask_reset_nodeData(struct SdkPlayBackTask * const pSdkPlayBackTask, /* 数据目录,如/vsys/aa/video/ */const char dirName[]
            , /* 删除一个回放句柄的方法 */ void (*const playBackHandle_delete__OI) (/* 回放句柄 */void *pPlayBackHandle)
            , /* 共享buffer,不可为空 */ FsShareBuffer * const pShareBuffer);

    /* 创建一个回放任务,内部应检查dirName是否重复,相同的dirName应视为相同的回放任务,更新参数 */

    /* 创建一个回放任务,内部应检查dirName是否重复,相同的dirName应视为相同的回放任务,更新参数 */
    void sdkPlayBackTask_updateTask__OI_2(struct SdkPlayBackTask * const pSdkPlayBackTask, /* 回放task */FsObjectSdkPlayBackTask * const pTask, /* 当前的运行时间 */const double thisUptime
            , /* 回放句柄需要的空间大小 */const unsigned int playBackHandleSize, /* 播放暂停的方法 */ void (*const playBackHandle_play) (/* 回放句柄 */void *pPlayBackHandle, /* 播放速度,0.0表示暂停 */float speed)
            , /* 删除一个回放句柄的方法 */ void (*const playBackHandle_delete__OI) (/* 回放句柄 */void *pPlayBackHandle)
            , /* 单线程缓存Buffer,不为空 */FsObjectSingleBuffer * const pSingleBuffer, /* 共享buffer,不可为空 */ FsShareBuffer * const pShareBuffer);

    /* 删除一个回放任务,内部会先检查pTask是否在内部,在内部会删除 */

    void sdkPlayBackTask_deleteTask__OI_2(struct SdkPlayBackTask * const pSdkPlayBackTask, /* 回放task */FsObjectSdkPlayBackTask * const pTask
            , /* 删除一个回放句柄的方法 */ void (*const playBackHandle_delete__OI) (/* 回放句柄 */void *pPlayBackHandle));

    /* 处理pSdkPlayBackTask中的数据,包含句柄创建删除及数据写盘 */

    void sdkPlayBackTask_do(struct SdkPlayBackTask * const pSdkPlayBackTask, /* 数据目录,如/vsys/aa/video/ */const char dirName[]
            , /* 回放句柄需要的空间大小 */const unsigned int playBackHandleSize, /* 当前的运行时间 */const double thisUptime
            , /* 获取还能创建多少个回放句柄的方法,当可用回放句柄数小于1时是不能创建回放句柄的 */ int (*const playBackHandle_getAvailableCount) (/* 外部传入指针 */void * p)
            , /* 创建一个回放句柄的方法,成功返回pHandleMemery,失败返回NULL,调用之前应确定有可用的句柄 */ void* (*const playBackHandle_new__IO) (/* 欲创建的回放句柄储存的位置 */void *pHandleMemery
            , /* 为哪个pTaskNode创建回放句柄 */struct SdkPlayBackTaskNode *pTaskNode, /* 播放时间 */double playTime, /* 外部传入指针 */void * p, /* 共享buffer,不可为空 */ FsShareBuffer * const pShareBuffer)
            , /* 删除一个回放句柄的方法 */ void (*const playBackHandle_delete__OI) (/* 回放句柄 */void *pPlayBackHandle)
            , /* 播放暂停的方法 */ void (*const playBackHandle_play) (/* 回放句柄 */void *pPlayBackHandle, /* 播放速度,0.0表示暂停 */float speed)
            , /* 获取一个不大于playTime且后面的连续时长不小于timeLenMin的播放时间,成功返回开始时间,失败返回-1.0 */
            double (* const playBackHandle_get_startTime_previous) (/* 外部传入指针 */void * p, /* 播放时间 */ double playTime, /* 播放时间后需要的最小时间段 */ double timeLenMin)
            , /* 获取播放时间对应时间段的开始时间,没有匹配的返回下一个时段的开始时间,成功返回开始时间,失败返回-1.0 */
            double (* const playBackHandle_get_startTime) (/* 外部传入指针 */void * p, /* 播放时间 */ double playTime, /* 播放时间后需要的最小时间段 */ double timeLenMin)
            , /* 外部传入指针 */void *const externP
            , /* 单线程缓存Buffer,不为空 */FsObjectSingleBuffer * const pSingleBuffer, /* 共享buffer,不可为空 */ FsShareBuffer * const pShareBuffer);

    /* 输出pSdkPlayBackTask中的数据 */

    void sdkPlayBackTask_out(struct SdkPlayBackTask * const pSdkPlayBackTask, FILE * const fd
            , /* 传入对象的pTaskNode_frameListLocked->frameList已加锁 */ struct SdkPlayBackTaskNode * const pTaskNode_frameListLocked);



#ifdef __cplusplus
}
#endif

#endif /* SDKPLAYBACKTASK_H */


