/* 
 * File:   Tasks.h
 * Author: fslib
 *
 * Created on 2013年6月15日, 上午11:19
 */

#if !defined TASKS_H && !defined JAVA_IN_C

#define TASKS_H
#include "../../PublicTools/publicDefine.h"

#include "../../PublicTools/Fs/ObjectList.h"
#include "../../PublicTools/Fs/StructList.h"
#include "../../PublicTools/Fs/Network.h"

#ifdef __cplusplus
extern "C" {
#endif

#pragma pack(push,1)

#pragma pack(pop)

    /* Tasks
     *   用于把多个任务给其他线程处理,主要用于解决一些同步接口问题;
     *   增加的任务无法删除,故只能在线程运行期间才能增加任务;
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
            //            /* 管道,用于添加和删除通道或发送数据 */
            //            int __pipe[2];
            /* 线程名 */
#define Tasks_get_threadName(___pTasks) (((char*)((___pTasks)+1))+(___pTasks)->ro._threadName_)
            unsigned char _threadName_;
            /* 线程数量的最大量 */
            unsigned short _threadCountMax;
            /* 任务链表,节点为函数指针+参数指针 */
            FsStructList *__taskList;
            /* 管道链表 */
            FsObjectList *__pipeList_taskList;

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
            signed char thisNice;
            /* 期望使用的线程数 */
            unsigned short threadCount;
            /* 调整线程数量时用的辅助参数,计算规则为:
             *     初始化为0;
             *     在添加任务时
             *         如出现不能添加,如taskLayer是小于layer的,则此值加上queueLengthMax+1,如threadCal大于1000且threadCount小于threadCountMax,则threadCount加1;
             *         如添加成功且taskLayer是大于layer的则此值减1,如threadCal小于-1000且threadCount大于1,则threadCount减1
             *     
             */
            signed short threadCal;
            /* 信号发给哪个线程处理,轮询 */
            unsigned short pipeIndex;
            /* 当前的层数,当添加的任务层数与此值的差值大于此最大值的1/4时认为其与本值相同 */
            unsigned short layer;
            /* 当添加的任务的队列按如下方式计算
             * task_add_pthreadSafety函数的传入参数为queueLengthMax,层数为taskLayer
             *  if(taskLayer 小于等于 layer-layerDiff){
             *     queueLengthMax+=layer-taskLayer;
             *     if(queueLengthMax>taskList->nodeCount){
             *         插入任务,返回成功
             *     }else{
             *         layerDiff=layer-taskLayer;
             *         threadCal+=queueLengthMax+1;
             *         if(threadCal>1000){
             *             threadCal=0;
             *             if(threadCount小于threadCountMax)threadCount++;
             *         }
             *         不插入任务,返回错误
             *     }
             *  }else{
             *      if(layerDiff>0)layerDiff--;
             *      if(taskLayer 小于等于 layer){
             *          queueLengthMax+=layer-taskLayer-layerDiff; 
             *          if(queueLengthMax>taskList->nodeCount){
             *              插入任务,返回成功
             *          }else{
             *              threadCal+=queueLengthMax+1;
             *              if(threadCal>1000){
             *                  threadCal=0;
             *                  if(threadCount 小于 threadCountMax)threadCount++;
             *              }
             *              不插入任务,返回错误
             *          }
             *      }else if(0==layerDiff){
             *          queueLengthMax-=taskLayer-layer; 
             *          if(queueLengthMax>taskList->nodeCount){           
             *              if(--threadCal 小于 -1000){
             *                  threadCal=0;
             *                  if(threadCount > 1)threadCount--;
             *              }
             *              插入任务,返回成功
             *          }else{
             *              if(0==taskList->nodeCount) layer++;
             *              不插入任务,返回错误
             *          }
             *      }else{
             *          不插入任务,返回错误
             *      }
             *  }
             */
            unsigned short layerDiff;
        } rw;

        struct {
            /* 线程是否在运行(0:没有运行,1:在运行) */
            unsigned char inRun_taskList : 1;
            /* 当前客户端的工作线程数 */
            unsigned short workThreadClient;
            /* 线程的tid号 */
            pthread_t stid;
            /* 主线程的线程号,由systemThreadTid获得 */
            pthread_t mainTid;
        } p;
    } Tasks;
#define __TasksLog(___Log,___loglevel,___format, ...)\
    ___Log(___loglevel,3,___format",p=%p,runStatus=%hhd/%hhu,tid=%ld/%ld\n  pMonitor=%p\n  externRunStatus=%p\n  parentTid=%ld\n  threadName:\"%s\"\n\
  taskList=%p/%lu\n  pipeList=%p/%lu/%hu\n  threadCount=%hu/%hu/%hu/%hu\n  layer=%hu/%hu\n",##__VA_ARGS__\
        ,pTasks,((Tasks*)(pTasks))->rw.runStatus,((Tasks*)(pTasks))->p.inRun_taskList,((Tasks*)(pTasks))->p.stid,((Tasks*)(pTasks))->p.mainTid\
        ,((Tasks*)(pTasks))->ro._pMonitor,((Tasks*)(pTasks))->ro._externRunStatus,((Tasks*)(pTasks))->ro._parentTid,Tasks_get_threadName(pTasks)\
        ,((Tasks*)(pTasks))->ro.__taskList,((Tasks*)(pTasks))->ro.__taskList->nodeCount,((Tasks*)(pTasks))->ro.__pipeList_taskList, ((Tasks*)(pTasks))->ro.__pipeList_taskList->nodeCount,((Tasks*)(pTasks))->rw.pipeIndex\
        ,((Tasks*)(pTasks))->rw.threadCount,((Tasks*)(pTasks))->p.workThreadClient,((Tasks*)(pTasks))->rw.threadCal,((Tasks*)(pTasks))->ro._threadCountMax,((Tasks*)(pTasks))->rw.layer,((Tasks*)(pTasks))->rw.layerDiff)
#define TasksLog(___loglevel,___format, ...) __TasksLog(FsLog,___loglevel,___format,##__VA_ARGS__)
#define TasksLog2(___loglevel,___format, ...) __TasksLog(FsLog2,___loglevel,___format,##__VA_ARGS__)

    /* 
     * 创建一个新的Tasks对象;
     * 返回Tasks指针.
     */

    Tasks *tasks_new__IO(/* 监控的线程指针对象,开始线程后把创建的监控节点添加到此对象中 */ void *const pMonitor
            , /* 多线程共有的运行状态,如果此值为空,则引用默认的全局运行状态变量,本线程的运行状态只有与此值相同才能运行 */signed char *const externRunStatus
            , /* 父线程的线程号,由systemThreadTid获得,为0表示无父线程 */ const pthread_t parentTid, /* 线程名 */const char threadName[]
            , /* 线程数量的最大量 */ const unsigned short threadCountMax, /* 初始化时使用多少线程 */ const unsigned short threadCount);

    /* 删除pTasks指向的实例对象 */

    void tasks_delete__OI(Tasks * const pTasks);

    /* 设置pTasks的参数 */

    void tasks_set_parameter_pthreadSafety(Tasks * const pTasks, /* 使用多少线程 */ const unsigned short threadCount);

    /* 开始线程,必须先设置好初始化时为可写全过程可读区的所有参数 */

    void tasks_startThread(Tasks * const pTasks
            , /* 线程分时优先级,有效值为[-1-99],越大优先级越高,0表示不改变优先级,继承父线程的优先级,-1-表示强制线程的调度模式为分时调度策略,正数强制线程的调度模式为实时分时调度策略 */const signed char thisNice);

    /* 结束线程 */

    void tasks_stopThread(Tasks * const pTasks);

    /* 添加一个任务,成功返回1,失败返回-1,成功时下次再执行此任务时传的taskLayer应为pTasks->p.layer+1
     * 如添加查询某相机的录像信息任务,应等其他相机也查一次后再进行下一次查询,兼顾公平性;
     */

    int tasks_add_pthreadSafety(Tasks * const pTasks, /* 最多的允许待处理数 */ unsigned long queueLengthMax, /* 任务层数 */ const unsigned short taskLayer
            , /* 要执行的函数 */void (*const fun) (void *p,const double thisUptime, /* 共享buffer,不为空 */ FsShareBuffer * const pShareBuffer), /* 执行函数用的参数 */void *const p);
  

    void tasks_test(void *pMonitor);

#ifdef __cplusplus
}
#endif

#endif /* TASKS_H */


