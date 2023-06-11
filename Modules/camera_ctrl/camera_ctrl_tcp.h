/* 
 * File:   camera_ctrl_hk_http.h
 * Author: fslib
 *
 * Created on 2013年6月15日, 上午11:19
 */

#ifndef CAMERA_CTRL_TCP_H
#define	CAMERA_CTRL_TCP_H
#include "../../Modules/camera_ctrl/HCNetSDK.h"
#include "../../PublicTools/Fs/TypeFace.h"
#include "../../Modules/camera_ctrl/ZGXAPI.h"


#ifdef	__cplusplus
extern "C" {
#endif

    struct camera_ctrl_hk_item {
        // char m_szCameraIP[64]; //IP��
        char m_szControlIP[64];
        char m_szCameraUid[64];
        char m_szCameraPwd[64];
        int m_nUserID;
        int m_nControlChannel;
        unsigned int m_nCameraPort;
        NET_DVR_DEVICEINFO_V30 m_sDeviceInfo;
    };

    struct camera_ctrl_zgx_item {
        char m_szCameraIP[64]; //IP��  
        unsigned int m_nCameraPort;
        struct timeval v_holder_tv; //云台socket等待时间
        int socket;
    };

    struct camera_ctrl_shangsai_item {
        char m_szCameraIP[64]; //IP��  
        unsigned int m_nCameraPort[2];
        struct timeval v_holder_tv; //云台socket等待时间        
        int socket[2];
    };

    /* 相机相机控制项 */
    struct Camera_ctrl_tcp_item {

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
            /* 数据连接对象 */
            struct SocketPool_item *__pSocketPool_item;
        } ro;

        struct {
            camera_t m_cam_type; //相机类型
            camera_ctrl_hk_item hkCam_ctl_item; //海康相机控制信息
            camera_ctrl_zgx_item zgxCam_ctl_item; //中光学相机控制信息
            camera_ctrl_shangsai_item shsCam_ctl_item; //上赛相机控制信息

            /* 相机项 */
            void *__pCamera_item;
            /* 控制 */
            struct Camera_ctrl_functionUser *_ctl_function;

            /* 状态,0-未知,1-空闲,2-任意转,3-左转,4-右转,5-上转,6-下转,7-左上,8-左下,9-右上,10-右下,11-变倍,12-聚焦,13-设置预置位,14-调用预置位,15-清除预置位,16-3d定位 */
            unsigned char status;
            /* 设置的状态,2-任意转,3-左转,4-右转,5-上转,6-下转,7-左上,8-左下,9-右上,10-右下,11-变倍,12-聚焦,13-设置预置位,14-调用预置位,15-清除预置位,16-3d定位 */
            unsigned char status_set;
        } rw;
    };

    void camera_ctrl_tcp_delete__OI(struct Camera_ctrl_tcp_item * pCamera_ctrl_tcp_item);
    /* 
     * 创建一个新的Camera_ctrl_hk_item对象;
     * 返回Camera_ctrl_hk_item指针.
     */
    struct Camera_ctrl_tcp_item *camera_ctrl_tcp_item_new__IO(/*相机品牌*/unsigned int cambrand, /*相机品牌*/unsigned int cammodel, /* ipv4地址 */const char camera_ctrl_ip[], /*地址端口号 */const unsigned short camera_ctrl_port,
            /* 用户名,可为空 */const char username[], /* 密码,可为空 */const char password[], const unsigned short controlchannel);

    /* 
     * 创建一个新的Camera_ctrl_hk_item对象;
     * 返回Camera_ctrl_hk_item指针.
     */
    int camera_ctrl_tcp_connect(struct Camera_ctrl_tcp_item * pCamera_ctrl_tcp_item);

    int camera_crtl_tcp_FocusRect(struct Camera_ctrl_tcp_item * pCamera_ctrl_tcp_item, double l, double r, double t, double b);

    //BOOL camera_ctrl_tcp_GetDVRConfig(struct Camera_ctrl_tcp_item * pCamera_ctrl_tcp_item);

    int camera_ctrl_tcp_goPTZ(struct Camera_ctrl_tcp_item * pCamera_ctrl_tcp_item, float p, float t, float z);

    int camera_ctrl_tcp_getPTZ(struct Camera_ctrl_tcp_item * pCamera_ctrl_tcp_item, float* p, float* t, float* z);

    int camera_ctrl_tcp_setParam(struct Camera_ctrl_tcp_item * pCamera_ctrl_tcp_item, int speed);

#ifdef	__cplusplus
}
#endif

#endif	/* CAMERA_CTRL_HK_HTTP_H */

