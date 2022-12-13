/* 
 * File:   camera_ctrl_hk_http.h
 * Author: fslib
 *
 * Created on 2013年6月15日, 上午11:19
 */
#if !defined CAMERA_CTRL_HK_HTTP_H && !defined JAVA_IN_C

#define CAMERA_CTRL_HK_HTTP_H
#include "../../PublicTools/publicDefine.h"

#include "../../PublicTools/Thread/camera_ctrl.h"
#include "../../PublicTools/Thread/SocketPool.h"
#ifdef __cplusplus
extern "C" {
#endif
    struct Camera_ctrl_hk_http_item;

    /* 海康相机控制 */
    struct Camera_ctrl_hk_http {

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
            /* 线程名 */
            char *_threadName;
            /* 监控的线程指针对象,开始线程后把创建的监控节点添加到此对象中 */
            void *_pMonitor;
            /* 父线程的线程号,由systemThreadTid获得,为0表示无父线程 */
            pthread_t _parentTid;
            /* 多线程共有的运行状态,如果此值为空,则引用默认的全局运行状态变量,本线程的运行状态只有与此值相同才能运行 */
            signed char *_externRunStatus;
            /* 连接池,为空表示内部使用独立的连接池 */
            SocketPool *_pSocketPool;
            /* 处理的相机链表 */
            FsObjectList *__cameralist;
            /* 添加或删除连接项(0:没有动作;1:添加;2:删除) */
            unsigned char changeItem : 2;
            /* 线程是否在运行(0:没有运行,1:在运行) */
            unsigned char inRun : 1;
            /* ThreadSocketIPv4线程的tid号 */
            pthread_t stid;
            /* 线程同步锁,仅用于添加或删除连接项 */
            pthread_mutex_t __mutex;
            /* 管道,用于添加和删除文件或发送数据 */
            int __pipe[2];
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
            /* 最长没有发送和接收数据的时间 */
        } rw;

        struct {
            /* 将要添加或删除的连接项 */
            struct Camera_ctrl_hk_http_item *pCamera_ctrl_hk_http_item_change;
        } p;
    };

    /* 海康相机控制项 */
    struct Camera_ctrl_hk_http_item {

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
            /* 用户名,可为空 */
            char* _username;
            /* 密码,可为空 */
            char* _password;
            /* 用户名和密码用base64编码后的值 */
            char *_userAndPasswordBase64;
            /* 默认的控制类型,0-表示自动探测,0x1表示探测结果最多只能包含任意转 */
            unsigned long long _controlMaskDefault;
            /* 数据连接对象 */
            struct SocketPool_item *__pSocketPool_item;
            /* 互斥锁 */
            pthread_mutex_t __mutex;
        } ro;

        struct {
        } rw;

        struct {
            /* 所属的海康相机控制对象 */
            struct Camera_ctrl_hk_http *pCamera_ctrl_hk;
            /* 掩码,按位排布,0-任意转,1-左右转,2-上下转,3-45度斜角转,4-变倍,5-聚焦,6-预置位,7-3d定位 */
            unsigned long long mask;
            /* 状态,0-未知,1-空闲,2-任意转,3-左转,4-右转,5-上转,6-下转,7-左上,8-左下,9-右上,10-右下,11-变倍,12-聚焦,13-设置预置位,14-调用预置位,15-清除预置位,16-3d定位 */
            unsigned char status;
            /* 设置的状态,2-任意转,3-左转,4-右转,5-上转,6-下转,7-左上,8-左下,9-右上,10-右下,11-变倍,12-聚焦,13-设置预置位,14-调用预置位,15-清除预置位,16-3d定位 */
            unsigned char status_set;
            /* 校验和 */
            int sum;
            /* 新的校验和 */
            int sum_set;
            /* 设置的参数1 */
            short parameter1_set;
            /* 设置的参数2 */
            short parameter2_set;
            /* 设置的参数3 */
            short parameter3_set;
            /* 设置的参数4 */
            short parameter4_set;
            /* 可以做下一个动作的时间 */
            double time_next;
            /* 动作最长可以执行的时间 */
            double time_end;
        } p;
    };

    /* 
     * 创建一个新的Camera_ctrl_hk_http对象;
     * 返回Camera_ctrl_hk_http指针.
     */

    struct Camera_ctrl_hk_http *camera_ctrl_hk_http_new__IO(/* 线程名 */const char threadName[],
            /* 监控的线程指针对象,开始线程后把创建的监控节点添加到此对象中 */ void *pMonitor,
            /* 父线程的线程号,由systemThreadTid获得,为0表示无父线程 */ const pthread_t parentTid,
            /* 多线程共有的运行状态,如果此值为空,则引用默认的全局运行状态变量,本线程的运行状态只有与此值相同才能运行 */signed char *externRunStatus,
            /* 连接池,为空表示内部使用独立的连接池 */ SocketPool * const pSocketPool);

    /* 删除pCamera_ctrl_hk指向的实例对象 */

    void camera_ctrl_hk_http_delete__OI(struct Camera_ctrl_hk_http * const pCamera_ctrl_hk_http);

    /* 开始线程,必须先设置好初始化时为可写全过程可读区的所有参数 */

    void camera_ctrl_hk_http_startThread(struct Camera_ctrl_hk_http * const pCamera_ctrl_hk_http,
            /* 线程分时优先级,有效值为[-1-99],越大优先级越高,0表示不改变优先级,继承父线程的优先级,-1-表示强制线程的调度模式为分时调度策略,正数强制线程的调度模式为实时分时调度策略 */const signed char thisNice);

    /* 结束线程 */

    void camera_ctrl_hk_http_stopThread(struct Camera_ctrl_hk_http * const pCamera_ctrl_hk_http);

    /* 
     * 创建一个新的Camera_ctrl_hk_item对象;
     * 返回Camera_ctrl_hk_item指针.
     */

    struct Camera_ctrl_hk_http_item *camera_ctrl_hk_http_item_new__IO(/* ipv4地址 */const unsigned int ipv4, /* ipv4地址端口号 */const unsigned short ipv4Port,
            /* 用户名,可为空 */const char username[], /* 密码,可为空 */const char password[], /* 默认的控制类型,0-表示自动探测,0x1表示探测结果最多只能包含任意转 */ const unsigned long long controlMaskDefault);

    /* 删除pCamera_ctrl_hk_item指向的实例对象 */

    void camera_ctrl_hk_http_item_delete__OI(struct Camera_ctrl_hk_http_item * const pCamera_ctrl_hk_item);

    /* 向pCamera_ctrl_hk_http中添加一个控制项 */

    void camera_ctrl_hk_http_add_item__OI_2(struct Camera_ctrl_hk_http * const pCamera_ctrl_hk_http, struct Camera_ctrl_hk_http_item * const pCamera_ctrl_hk_item);

    /* 从pCamera_ctrl_hk_http中移除一个控制项 */

    void camera_ctrl_hk_http_remove_item__IO_2(struct Camera_ctrl_hk_http * const pCamera_ctrl_hk_http, struct Camera_ctrl_hk_http_item * const pCamera_ctrl_hk_item);

    /* 获取掩码,按位排布,0-任意转,1-左右转,2-上下转,3-45度斜角转,4-变倍,5-聚焦,6-预置位,7-3d定位 */

    unsigned long long camera_ctrl_hk_http_item_mask_get(struct Camera_ctrl_hk_http_item * const pCamera_ctrl_hk_item, /* 不为空时使用*pProtocol_name返回控制协议协议名,如onvif */const char * * const pProtocol_name
            , /* 设备型号,不为空时把设备名拷贝输出,无设备名输出\0 */ char device_type[], /* device_type空间大小 */const unsigned int device_typeLen
            , /* 设备序列号,不为空时把设备序列号拷贝输出,无设备序列号输出\0 */ char device_sn[], /* device_sn空间大小 */const unsigned int device_snLen
            , /* 设备版本号,不为空时把设备版本号拷贝输出,无设备版本号输出\0 */ char device_version[], /* device_version空间大小 */const unsigned int device_versionLen);

    /* 控制,任意转 */

    void camera_ctrl_hk_http_item_ctrl_turn_pthreadSafety(struct Camera_ctrl_hk_http_item * const pCamera_ctrl_hk_item, /* 参考值精度为23040,负数向左,正数向右 */const short speedx,
            /* 参考值精度为23040,负数向上,正数向下 */const short speedy, /* 转动时间 */const float time);

    /* 控制,左转 */

    void camera_ctrl_hk_http_item_ctrl_turnLeft_pthreadSafety(struct Camera_ctrl_hk_http_item * const pCamera_ctrl_hk_item, /* 参考值精度为23040 */const unsigned short speed, /* 转动时间 */const float time);

    /* 控制,右转 */

    void camera_ctrl_hk_http_item_ctrl_turnRight_pthreadSafety(struct Camera_ctrl_hk_http_item * const pCamera_ctrl_hk_item, /* 参考值精度为23040 */const unsigned short speed, /* 转动时间 */const float time);

    /* 控制,上转 */

    void camera_ctrl_hk_http_item_ctrl_turnUp_pthreadSafety(struct Camera_ctrl_hk_http_item * const pCamera_ctrl_hk_item, /* 参考值精度为23040 */const unsigned short speed, /* 转动时间 */const float time);

    /* 控制,下转 */

    void camera_ctrl_hk_http_item_ctrl_turnDown_pthreadSafety(struct Camera_ctrl_hk_http_item * const pCamera_ctrl_hk_item, /* 参考值精度为23040 */const unsigned short speed, /* 转动时间 */const float time);

    /* 控制,左上转 */

    void camera_ctrl_hk_http_item_ctrl_turnLeftUp_pthreadSafety(struct Camera_ctrl_hk_http_item * const pCamera_ctrl_hk_item, /* 参考值精度为23040 */const unsigned short speed, /* 转动时间 */const float time);

    /* 控制,左下转 */

    void camera_ctrl_hk_http_item_ctrl_turnLeftDown_pthreadSafety(struct Camera_ctrl_hk_http_item * const pCamera_ctrl_hk_item, /* 参考值精度为23040 */const unsigned short speed, /* 转动时间 */const float time);

    /* 控制,右上转 */

    void camera_ctrl_hk_http_item_ctrl_turnRightUp_pthreadSafety(struct Camera_ctrl_hk_http_item * const pCamera_ctrl_hk_item, /* 参考值精度为23040 */const unsigned short speed, /* 转动时间 */const float time);

    /* 控制,右下转 */

    void camera_ctrl_hk_http_item_ctrl_turnRightDown_pthreadSafety(struct Camera_ctrl_hk_http_item * const pCamera_ctrl_hk_item, /* 参考值精度为23040 */const unsigned short speed, /* 转动时间 */const float time);

    /* 控制,变倍 */

    void camera_ctrl_hk_http_item_ctrl_zoom_pthreadSafety(struct Camera_ctrl_hk_http_item * const pCamera_ctrl_hk_item, /* 参考值精度为23040,负数缩小,正数放大 */const short speed, /* 变倍时间 */const float time);

    /* 控制,聚焦 */

    void camera_ctrl_hk_http_item_ctrl_focus_pthreadSafety(struct Camera_ctrl_hk_http_item * const pCamera_ctrl_hk_item, /* 参考值精度为23040,负数-,正数+ */const short speed, /* 聚焦时间 */const float time);

    /* 控制,设置预置位 */

    void camera_ctrl_hk_http_item_ctrl_preset_set_pthreadSafety(struct Camera_ctrl_hk_http_item * const pCamera_ctrl_hk_item, /* 预置位号 */const unsigned short presetIndex);

    /* 控制,调用预置位 */

    void camera_ctrl_hk_http_item_ctrl_preset_goto_pthreadSafety(struct Camera_ctrl_hk_http_item * const pCamera_ctrl_hk_item, /* 预置位号 */const unsigned short presetIndex);

    /* 控制,清除预置位 */

    void camera_ctrl_hk_http_item_ctrl_preset_clean_pthreadSafety(struct Camera_ctrl_hk_http_item * const pCamera_ctrl_hk_item, /* 预置位号 */const unsigned short presetIndex);

    /* 控制,3D定位 */

    void camera_ctrl_hk_http_item_ctrl_zoom3D_pthreadSafety(struct Camera_ctrl_hk_http_item * const pCamera_ctrl_hk_item, /* 相对图大小参考值23040 */const unsigned short x1,
            /* 相对图大小参考值23040 */const unsigned short y1, /* 相对图大小参考值23040 */const unsigned short x2, /* 相对图大小参考值23040 */const unsigned short y2);

    /* 控制,goto PTZ,成功返回1,失败返回-1 */

    int camera_ctrl_hk_http_item_ctrl_ptzGoto_pthreadSafety(struct Camera_ctrl_hk_http_item * const pCamera_ctrl_hk_item, /* 相对参考值23040 */const unsigned short P
            , /* 相对参考值23040 */const unsigned short T, /* 相对参考值23040 */const unsigned short Z);

    /* 控制,获取PTZ,成功返回1,失败返回-1 */

    int camera_ctrl_hk_http_item_ctrl_ptzGet_pthreadSafety(struct Camera_ctrl_hk_http_item * const pCamera_ctrl_hk_item, /* 相对参考值23040 */ unsigned short*const P
            , /* 相对参考值23040 */ unsigned short* const T, /* 相对参考值23040 */ unsigned short* const Z, /* 是否更新,1-更新,0-直接读取内存 */const unsigned char update);

    /* 控制,设置图像参数,成功返回1,失败返回-1 */

    int camera_ctrl_hk_http_item_ctrl_imageInfo_set_pthreadSafety(struct Camera_ctrl_hk_http_item * const pCamera_ctrl_hk_item, /* 哪些参数是有效的,按位摆布,如controlMode为0 */unsigned int setMask
            , /* 图像信息 */ struct Camera_ctrl_imageInfo * const imageInfo);

    /* 控制,获取图像参数,成功返回1,失败返回-1 */

    int camera_ctrl_hk_http_item_ctrl_imageInfoGet_pthreadSafety(struct Camera_ctrl_hk_http_item * const pCamera_ctrl_hk_item, /* 图像信息 */ struct Camera_ctrl_imageInfo * const imageInfo, /* 是否更新,1-更新,0-直接读取内存 */const unsigned char update);

    /* 控制,向机芯直接发送命令,成功返回1,失败返回-1 */

    int camera_ctrl_hk_http_item_ctrl_chipCmd_pthreadSafety(struct Camera_ctrl_hk_http_item * const pCamera_ctrl_hk_item, /* 命令的数据类型,0-字符串,1-二进制(cmd以字符串输入时使用base64编码) */ const unsigned char dataType, /* 命令 */const char cmd[], /* 命令长度 */const unsigned int cmdLen);

    /* 控制,快门重置,成功返回1,失败返回-1 */

    int camera_ctrl_hk_http_item_ctrl_shutterReset_pthreadSafety(struct Camera_ctrl_hk_http_item * const pCamera_ctrl_hk_item);



#ifdef __cplusplus
}
#endif

#endif /* CAMERA_CTRL_HK_HTTP_H */

