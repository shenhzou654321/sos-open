#include "capture.h"
#include <errno.h>
#include <malloc.h>
#include <stdlib.h>
#include <string.h>
#include <sys/poll.h>
#include <unistd.h>
#include "../../PublicTools/Fs/Log.h"
#include "../../PublicTools/Fs/Network.h"
#include "../../PublicTools/Fs/Socket.h"
#include "../../PublicTools/Fs/SystemInfo.h"
#include "../../PublicTools/Media/VideoH265.h"
#include "../../PublicTools/Thread/camera_ctrl.h"
#include "../../PublicTools/Thread/ConfigManager.h"
#include "../../PublicTools/Thread/monitor.h"
#include "../../Modules/camera_gb28181/camera_gb28181.h"
#include "../../Modules/camera_infIray/camera_inflray.h"
#include "../../Modules/camera_rtsp/camera_rtsp.h" 
#include  "../../Modules/camera_ctrl/ZGXAPI.h"
////////////////////////////////////////////////////////////////////////////////
///////////////////////////定义外部变量与函数开始/////////////////////////////////
#ifdef FsPrintfIndex
#undef FsPrintfIndex
#endif 
#define FsPrintfIndex 1
///////////////////////////定义外部变量与函数结束/////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
///////////////////////////定义全局变量开始//////////////////////////////////////
///////////////////////////定义全局变量结束//////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
///////////////////////////定义私有结构开始//////////////////////////////////////
/* 本模块使用独立日志级别 */
#if 0
static unsigned int moduleLoglevel = FsNumberMax(loglevel); // 本模块使用自定义的loglevel,最大值表示未加到日志级别管理模块中
#define loglevel moduleLoglevel
#endif
static void *camera_rtsp_ctrl_function[] = {
    /* 取消调用者保持信息 */
    (void*) camera_rtsp_item_cancelKey_pthreadSafety,
    /* 掩码,按位排布,0-任意转,1-左右转,2-上下转,3-45度斜角转,4-变倍,5-聚焦,6-预置位,7-3d定位,8-gotoPTZ和getPTZ,9-图像参数设置与获取,10-直接向机芯发送命令 */
    (void*) camera_rtsp_item_mask_get_pthreadSafety,
    /* 控制,任意转,成功返回1,失败返回-1,优先级不够返回-2 */
    (void*) camera_rtsp_item_ctrl_turn_pthreadSafety,
    /* 控制,左转,成功返回1,失败返回-1,优先级不够返回-2 */
    (void*) camera_rtsp_item_ctrl_turnLeft_pthreadSafety,
    /* 控制,右转,成功返回1,失败返回-1,优先级不够返回-2 */
    (void*) camera_rtsp_item_ctrl_turnRight_pthreadSafety,
    /* 控制,上转,成功返回1,失败返回-1,优先级不够返回-2 */
    (void*) camera_rtsp_item_ctrl_turnUp_pthreadSafety,
    /* 控制,下转,成功返回1,失败返回-1,优先级不够返回-2 */
    (void*) camera_rtsp_item_ctrl_turnDown_pthreadSafety,
    /* 控制,左上转,成功返回1,失败返回-1,优先级不够返回-2 */
    (void*) camera_rtsp_item_ctrl_turnLeftUp_pthreadSafety,
    /* 控制,左下转,成功返回1,失败返回-1,优先级不够返回-2 */
    (void*) camera_rtsp_item_ctrl_turnLeftDown_pthreadSafety,
    /* 控制,右上转,成功返回1,失败返回-1,优先级不够返回-2 */
    (void*) camera_rtsp_item_ctrl_turnRightUp_pthreadSafety,
    /* 控制,右下转,成功返回1,失败返回-1,优先级不够返回-2 */
    (void*) camera_rtsp_item_ctrl_turnRightDown_pthreadSafety,
    /* 控制,变倍,成功返回1,失败返回-1,优先级不够返回-2 */
    (void*) camera_rtsp_item_ctrl_zoom_pthreadSafety,
    /* 控制,聚焦,成功返回1,失败返回-1,优先级不够返回-2 */
    (void*) camera_rtsp_item_ctrl_focus_pthreadSafety,
    /* 控制,设置预置位,成功返回1,失败返回-1,优先级不够返回-2 */
    (void*) camera_rtsp_item_ctrl_preset_set_pthreadSafety,
    /* 控制,调用预置位,成功返回1,失败返回-1,优先级不够返回-2 */
    (void*) camera_rtsp_item_ctrl_preset_goto_pthreadSafety,
    /* 控制,清除预置位,成功返回1,失败返回-1,优先级不够返回-2 */
    (void*) camera_rtsp_item_ctrl_preset_clean_pthreadSafety,
    /* 控制,3D定位,成功返回1,失败返回-1,优先级不够返回-2 */
    (void*) camera_rtsp_item_ctrl_zoom3D_pthreadSafety,
    /* 控制,goto PTZ,成功返回1,失败返回-1,优先级不够返回-2 */
    (void*) camera_rtsp_item_ctrl_ptz_goto_pthreadSafety,
    /* 控制,获取PTZ,成功返回1,失败返回-1,对没有优先级不够的客户端会直接读取缓存返回数据 */
    (void*) camera_rtsp_item_ctrl_ptz_get_pthreadSafety,
    /* 控制,设置图像参数,成功返回1,失败返回-1,优先级不够返回-2 */
    (void*) camera_rtsp_item_ctrl_imageInfo_set_pthreadSafety,
    /* 控制,获取图像参数,成功返回1,失败返回-1,对没有优先级不够的客户端会直接读取缓存返回数据 */
    (void*) camera_rtsp_item_ctrl_imageInfo_get_pthreadSafety,
    /* 控制,向机芯直接发送命令,成功返回1,失败返回-1,优先级不够返回-2 */
    (void*) camera_rtsp_item_ctrl_chipCmd_pthreadSafety,
    /* 控制,快门重置,成功返回1,失败返回-1,优先级不够返回-2 */
    (void*) camera_rtsp_item_ctrl_shuterReset_pthreadSafety
};
static void *camera_gb28181_ctl_function[] = {
    /* 取消调用者保持信息 */
    (void*) camera_gb28181_item_cancelKey_pthreadSafety,
    /* 获取掩码,按位排布,0-任意转,1-左右转,2-上下转,3-45度斜角转,4-变倍,5-聚焦,6-预置位,7-3d定位 */
    (void*) camera_gb28181_item_mask_get_pthreadSafety,
    /* 控制,任意转,成功返回1,失败返回-1,优先级不够返回-2 */
    (void*) camera_gb28181_item_ctrl_turn_pthreadSafety,
    /* 控制,左转,成功返回1,失败返回-1,优先级不够返回-2 */
    (void*) camera_gb28181_item_ctrl_turnLeft_pthreadSafety,
    /* 控制,右转,成功返回1,失败返回-1,优先级不够返回-2 */
    (void*) camera_gb28181_item_ctrl_turnRight_pthreadSafety,
    /* 控制,上转,成功返回1,失败返回-1,优先级不够返回-2 */
    (void*) camera_gb28181_item_ctrl_turnUp_pthreadSafety,
    /* 控制,下转,成功返回1,失败返回-1,优先级不够返回-2 */
    (void*) camera_gb28181_item_ctrl_turnDown_pthreadSafety,
    /* 控制,左上转,成功返回1,失败返回-1,优先级不够返回-2 */
    (void*) camera_gb28181_item_ctrl_turnLeftUp_pthreadSafety,
    /* 控制,左下转,成功返回1,失败返回-1,优先级不够返回-2 */
    (void*) camera_gb28181_item_ctrl_turnLeftDown_pthreadSafety,
    /* 控制,右上转,成功返回1,失败返回-1,优先级不够返回-2 */
    (void*) camera_gb28181_item_ctrl_turnRightUp_pthreadSafety,
    /* 控制,右下转,成功返回1,失败返回-1,优先级不够返回-2 */
    (void*) camera_gb28181_item_ctrl_turnRightDown_pthreadSafety,
    /* 控制,变倍,成功返回1,失败返回-1,优先级不够返回-2 */
    (void*) camera_gb28181_item_ctrl_zoom_pthreadSafety,
    /* 控制,聚焦,成功返回1,失败返回-1,优先级不够返回-2 */
    (void*) camera_gb28181_item_ctrl_focus_pthreadSafety,
    /* 控制,设置预置位,成功返回1,失败返回-1,优先级不够返回-2 */
    (void*) camera_gb28181_item_ctrl_preset_set_pthreadSafety,
    /* 控制,调用预置位,成功返回1,失败返回-1,优先级不够返回-2 */
    (void*) camera_gb28181_item_ctrl_preset_goto_pthreadSafety,
    /* 控制,清除预置位,成功返回1,失败返回-1,优先级不够返回-2 */
    (void*) camera_gb28181_item_ctrl_preset_clean_pthreadSafety,
    /* 控制,3D定位,成功返回1,失败返回-1,优先级不够返回-2 */
    (void*) camera_gb28181_item_ctrl_zoom3D_pthreadSafety,
    /* 控制,goto PTZ,成功返回1,失败返回-1,优先级不够返回-2 */
    (void*) camera_gb28181_item_ctrl_ptz_goto_pthreadSafety,
    /* 控制,获取PTZ,成功返回1,失败返回-1,对没有优先级不够的客户端会直接读取缓存返回数据 */
    (void*) camera_gb28181_item_ctrl_ptz_get_pthreadSafety,
    /* 控制,设置图像参数,成功返回1,失败返回-1,优先级不够返回-2 */
    (void*) camera_gb28181_item_ctrl_imageInfo_set_pthreadSafety,
    /* 控制,获取图像参数,成功返回1,失败返回-1,对没有优先级不够的客户端会直接读取缓存返回数据 */
    (void*) camera_gb28181_item_ctrl_imageInfo_get_pthreadSafety,
    /* 控制,向机芯直接发送命令,成功返回1,失败返回-1,优先级不够返回-2 */
    (void*) camera_gb28181_item_ctrl_chipCmd_pthreadSafety,
    /* 控制,快门重置,成功返回1,失败返回-1,优先级不够返回-2 */
    (void*) camera_gb28181_item_ctrl_shuterReset_pthreadSafety
};
static void *camera_inflray_ctrl_function[] = {
    /* 取消调用者保持信息 */
    (void*) camera_inflray_item_cancelKey_pthreadSafety,
    /* 掩码,按位排布,0-任意转,1-左右转,2-上下转,3-45度斜角转,4-变倍,5-聚焦,6-预置位,7-3d定位,8-gotoPTZ和getPTZ,9-图像参数设置与获取,10-直接向机芯发送命令 */
    (void*) camera_inflray_item_mask_get_pthreadSafety,
    /* 控制,任意转,成功返回1,失败返回-1,优先级不够返回-2 */
    (void*) camera_inflray_item_ctrl_turn_pthreadSafety,
    /* 控制,左转,成功返回1,失败返回-1,优先级不够返回-2 */
    (void*) camera_inflray_item_ctrl_turnLeft_pthreadSafety,
    /* 控制,右转,成功返回1,失败返回-1,优先级不够返回-2 */
    (void*) camera_inflray_item_ctrl_turnRight_pthreadSafety,
    /* 控制,上转,成功返回1,失败返回-1,优先级不够返回-2 */
    (void*) camera_inflray_item_ctrl_turnUp_pthreadSafety,
    /* 控制,下转,成功返回1,失败返回-1,优先级不够返回-2 */
    (void*) camera_inflray_item_ctrl_turnDown_pthreadSafety,
    /* 控制,左上转,成功返回1,失败返回-1,优先级不够返回-2 */
    (void*) camera_inflray_item_ctrl_turnLeftUp_pthreadSafety,
    /* 控制,左下转,成功返回1,失败返回-1,优先级不够返回-2 */
    (void*) camera_inflray_item_ctrl_turnLeftDown_pthreadSafety,
    /* 控制,右上转,成功返回1,失败返回-1,优先级不够返回-2 */
    (void*) camera_inflray_item_ctrl_turnRightUp_pthreadSafety,
    /* 控制,右下转,成功返回1,失败返回-1,优先级不够返回-2 */
    (void*) camera_inflray_item_ctrl_turnRightDown_pthreadSafety,
    /* 控制,变倍,成功返回1,失败返回-1,优先级不够返回-2 */
    (void*) camera_inflray_item_ctrl_zoom_pthreadSafety,
    /* 控制,聚焦,成功返回1,失败返回-1,优先级不够返回-2 */
    (void*) camera_inflray_item_ctrl_focus_pthreadSafety,
    /* 控制,设置预置位,成功返回1,失败返回-1,优先级不够返回-2 */
    (void*) camera_inflray_item_ctrl_preset_set_pthreadSafety,
    /* 控制,调用预置位,成功返回1,失败返回-1,优先级不够返回-2 */
    (void*) camera_inflray_item_ctrl_preset_goto_pthreadSafety,
    /* 控制,清除预置位,成功返回1,失败返回-1,优先级不够返回-2 */
    (void*) camera_inflray_item_ctrl_preset_clean_pthreadSafety,
    /* 控制,3D定位,成功返回1,失败返回-1,优先级不够返回-2 */
    (void*) camera_inflray_item_ctrl_zoom3D_pthreadSafety,
    /* 控制,goto PTZ,成功返回1,失败返回-1,优先级不够返回-2 */
    (void*) camera_inflray_item_ctrl_ptz_goto_pthreadSafety,
    /* 控制,获取PTZ,成功返回1,失败返回-1,对没有优先级不够的客户端会直接读取缓存返回数据 */
    (void*) camera_inflray_item_ctrl_ptz_get_pthreadSafety,
    /* 控制,设置图像参数,成功返回1,失败返回-1,优先级不够返回-2 */
    (void*) camera_inflray_item_ctrl_imageInfo_set_pthreadSafety,
    /* 控制,获取图像参数,成功返回1,失败返回-1,对没有优先级不够的客户端会直接读取缓存返回数据 */
    (void*) camera_inflray_item_ctrl_imageInfo_get_pthreadSafety,
    /* 控制,向机芯直接发送命令,成功返回1,失败返回-1,优先级不够返回-2 */
    (void*) camera_inflray_item_ctrl_chipCmd_pthreadSafety,
    /* 控制,快门重置,成功返回1,失败返回-1,优先级不够返回-2 */
    (void*) camera_inflray_item_ctrl_shutterReset_pthreadSafety
};

static int ZGX_Connect(char* ip, unsigned int port, struct timeval* Timeout) {
    return fs_Socket_tcpIPv4_connect_byStringIP__IO(ip, port, 1, Timeout->tv_sec + Timeout->tv_usec / 1000000.0);
}

static int ZGX_Disconnect(int socket) {
    close(socket);
    return 1;
}

static int ZGX_control(int socket, int camtype, int command, int speed, int time) {
    if (socket < 0) return -1;

    camera_t cam;
    if (camtype == 1) {
        cam = CAM_FACTOR_ZGXLIGHT;
    }

    ptz_param_info_t ptz_p;
    ptz_p.nDirectSpeed = speed;

    camera_param_t cam_p;

    switch (command) {
        case 0x0002: //向右
            cam_p = CAM_PARAM_DIRECT_RIGHT;
            break;
        case 0x0004: //向左
            cam_p = CAM_PARAM_DIRECT_LEFT;
            break;
        case 0x0008: //向上
            cam_p = CAM_PARAM_DIRECT_UP;
            break;
        case 0x0010: //向下
            cam_p = CAM_PARAM_DIRECT_DOWN;
            break;
        case 0x000C: //左上
            cam_p = CAM_PARAM_DIRECT_LEFTUP;
            break;
        case 0x0014: //左下
            cam_p = CAM_PARAM_DIRECT_LEFTDOWN;
            break;
        case 0x000A: //右上
            cam_p = CAM_PARAM_DIRECT_RIGHTUP;
            break;
        case 0x0012: //右下
            cam_p = CAM_PARAM_DIRECT_RIGHTDOWN;
            break;
        case 0x0020: //变倍大
            cam_p = CAM_PARAM_ZOOM_IN;
            break;
        case 0x0040: //变倍小
            cam_p = CAM_PARAM_ZOOM_OUT;
            break;
        case 0x0080: //聚焦远
            cam_p = CAM_PARAM_FOCUS_FAR;
            break;
        case 0x0100: //聚焦近
            cam_p = CAM_PARAM_FOCUS_NEAR;
            break;
        case 0x0200: //光圈大
            cam_p = CAM_PARAM_IRIS_OPEN;
            break;
        case 0x0400: //光圈小
            cam_p = CAM_PARAM_IRIS_CLOSE;
            break;
        case 0x8100: //自动除雾开

            break;
        case 0x8200: //自动除雾关

            break;
        case 0x8300: //激光器开

            break;
        case 0x8400: //激光器关

            break;
        case 0x0000: //停止码
            cam_p = CAM_PARAM_STOP;
            break;
    }

    struct timeval v_holder_tv; //云台socket等待时间
    v_holder_tv.tv_sec = 1;
    v_holder_tv.tv_usec = 200 * 1000;
    cts_res_t ret = camera_ctl_setParam(socket, cam, cam_p, &ptz_p, &v_holder_tv);
    if (ret != CTS_SUCCESS) {

        printf("(line:%d) cam ctl error!\n", __LINE__);
        camera_disconnect(socket);

        return -1;
    }
    return 0;
}


///////////////////////////定义私有结构结束//////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
///////////////////////////定义私有函数开始//////////////////////////////////////

/* rtsp相机的回调函数,成功返回1,失败返回-1 */
static int capture_P_camera_rtsp_cb(FsObject * const pObject, /* 采集项 */ struct Capture_item *pCapture_item, MonitorItem *pMonitorItem) {
    FsObjectImageFrame *pFrame;
    if (FsObject_type_h264 == pObject->type) {
        FsObjectH264 *pH264 = (FsObjectH264*) pObject;
        if (pCapture_item->ro.imageWidth != pH264->width || pCapture_item->ro.imageHeight != pH264->height) {
            pCapture_item->ro.imageWidth = pH264->width;
            pCapture_item->ro.imageHeight = pH264->height;
            if (pCapture_item->ro.classIndex < FsNumberMax(pCapture_item->ro.classIndex)) pCapture_item->ro.classIndex++;
            else pCapture_item->ro.classIndex = 0;
        }
        pMonitorItem->rw.line = 40000 + __LINE__;
        pH264->classIndex = pCapture_item->ro.classIndex;
        pFrame = image_frame_new__IO(pCapture_item->ro.imageWidth, pCapture_item->ro.imageHeight
                , pCapture_item->ro.imageWidth >> 1, pCapture_item->ro.imageHeight >> 1
                , pCapture_item->ro.imageWidth >> 2, pCapture_item->ro.imageHeight >> 2
                , NULL, pH264->frameIndex, pH264->stitchIndex, pH264->cutLeft, pH264->cutRight, pCapture_item->ro.classIndex, pH264->ipsrc, pH264->frameRate, pH264->capture_uptime, pH264->capture_gmtTime);
    } else if (FsObject_type_h265 == pObject->type) {
        FsObjectH265 *pH265 = (FsObjectH265*) pObject;
        if (pCapture_item->ro.imageWidth != pH265->width || pCapture_item->ro.imageHeight != pH265->height) {
            pCapture_item->ro.imageWidth = pH265->width;
            pCapture_item->ro.imageHeight = pH265->height;
            if (pCapture_item->ro.classIndex < FsNumberMax(pCapture_item->ro.classIndex)) pCapture_item->ro.classIndex++;
            else pCapture_item->ro.classIndex = 0;
        }
        pMonitorItem->rw.line = 40000 + __LINE__;
        pH265->classIndex = pCapture_item->ro.classIndex;
        pFrame = image_frame_new__IO(pCapture_item->ro.imageWidth, pCapture_item->ro.imageHeight
                , pCapture_item->ro.imageWidth >> 1, pCapture_item->ro.imageHeight >> 1
                , pCapture_item->ro.imageWidth >> 2, pCapture_item->ro.imageHeight >> 2
                , NULL, pH265->frameIndex, pH265->stitchIndex, pH265->cutLeft, pH265->cutRight, pCapture_item->ro.classIndex, pH265->ipsrc, pH265->frameRate, pH265->capture_uptime, pH265->capture_gmtTime);
    } else {
        FsLog(FsLogType_fatal, FsPrintfIndex, "Invalid type(=%hu),%p.\n", pObject->type, pObject);
        FsLogTag(100);
        fflush(stdout);
        FsLogExit();
        return -1;
    }
    fs_Object_addRefer_pthreadSafety(pObject, __LINE__);
    image_frame_set__OI_2(pFrame, (FsObject*) pObject, 0);
    pMonitorItem->rw.line = 40000 + __LINE__;
    fs_ObjectList_insert_tail_pthreadSafety(pCapture_item->ro.__framelistOut, pFrame);
    Fs_GroupSqrtOut_value_set(pCapture_item->rw._pGroupSqrtOut, &pCapture_item->ro._pCapture->ro._pGroupSqrtOut->groupSqrt_mutex);
    static int lostdata = 0;
    if (pCapture_item->ro.__framelistOut->nodeCount > 32) {
        if (lostdata++ > 1024) {
            pthread_mutex_lock(&pCapture_item->ro.__framelistOut->mutex);
            if (pCapture_item->ro.__framelistOut->nodeCount > 32) {
                static int tttt = 0;
                if (tttt-- < 0) {
                    FsPrintf(1, "List count(=%lu) too long.\n", pCapture_item->ro.__framelistOut->nodeCount);
                } else tttt = 1000;
                fs_Object_delete_pthreadSafety__OI(fs_ObjectList_remove_head(pCapture_item->ro.__framelistOut));
            }
            pthread_mutex_unlock(&pCapture_item->ro.__framelistOut->mutex);
        }
    } else if (lostdata > 0)lostdata--;
    pMonitorItem->rw.line = 40000 + __LINE__;
    return 1;
}

/* rtsp相机的回调函数(主码流),成功返回1,失败返回-1 */
static signed char capture_P_camera_rtsp_cb_main(FsObject* pObject, /* 采集项 */ struct Capture_item *pCapture_item, MonitorItem *pMonitorItem) {
    if (FsObject_type_h264 == pObject->type) {
        FsObjectH264 *pH264 = (FsObjectH264*) pObject;
        if (pCapture_item->ro.imageWidth != pH264->width || pCapture_item->ro.imageHeight != pH264->height) {
            pCapture_item->ro.imageWidth = pH264->width;
            pCapture_item->ro.imageHeight = pH264->height;
            if (pCapture_item->ro.classIndex < FsNumberMax(pCapture_item->ro.classIndex)) pCapture_item->ro.classIndex++;
            else pCapture_item->ro.classIndex = 0;
            /* 清空缓存 */
            if (pCapture_item->ro.__pSubStream->dataCount != 0) {
                int count = pCapture_item->ro.__pSubStream->dataCount > 0 ? pCapture_item->ro.__pSubStream->dataCount : - pCapture_item->ro.__pSubStream->dataCount;
                pCapture_item->ro.__pSubStream->dataCount = 0;
                while (count-- > 0) {
                    fs_Object_delete_pthreadSafety__OI(pCapture_item->ro.__pSubStream->__pData[count]);
                }
            }
        }
        pthread_mutex_lock(&pCapture_item->ro.__framelistOut->mutex);
        pMonitorItem->rw.line = 40000 + __LINE__;
        pH264->classIndex = pCapture_item->ro.classIndex;
        if (pCapture_item->ro.__pSubStream->dataCount > 0) {
            /* 有缓存子码流,输出 */
            FsObjectH264 * pH264Sub = (FsObjectH264*) pCapture_item->ro.__pSubStream->__pData[0];
            /* 有多帧数据时使用最后一帧的时间可减小解码时的误差 */
            FsObjectImageFrame *pFrame;
            if (pCapture_item->ro.__pSubStream->dataCount < 3) {
                pFrame = image_frame_new__IO(pCapture_item->ro.imageWidth, pCapture_item->ro.imageHeight
                        , pCapture_item->ro.imageWidth >> 1, pCapture_item->ro.imageHeight >> 1
                        , pCapture_item->ro.__pSubStream->imageWidth, pCapture_item->ro.__pSubStream->imageHeight
                        , NULL, pH264Sub->frameIndex, pH264Sub->stitchIndex, pH264Sub->cutLeft, pH264Sub->cutRight, pCapture_item->ro.classIndex, pH264Sub->ipsrc, pH264Sub->frameRate, pH264Sub->capture_uptime, pH264Sub->capture_gmtTime);
            } else if (3 == pCapture_item->ro.__pSubStream->dataCount) {
                pH264Sub = (FsObjectH264*) pCapture_item->ro.__pSubStream->__pData[1];
                pFrame = image_frame_new__IO(pCapture_item->ro.imageWidth, pCapture_item->ro.imageHeight,
                        pCapture_item->ro.imageWidth >> 1, pCapture_item->ro.imageHeight >> 1,
                        pCapture_item->ro.__pSubStream->imageWidth, pCapture_item->ro.__pSubStream->imageHeight,
                        NULL, pH264Sub->frameIndex, pH264Sub->stitchIndex, pH264Sub->cutLeft, pH264Sub->cutRight, pCapture_item->ro.classIndex, pH264Sub->ipsrc, pH264Sub->frameRate, pH264Sub->capture_uptime, pH264Sub->capture_gmtTime);
                pH264Sub = (FsObjectH264*) pCapture_item->ro.__pSubStream->__pData[0];
            } else {
                pH264Sub = (FsObjectH264*) pCapture_item->ro.__pSubStream->__pData[2];
                pFrame = image_frame_new__IO(pCapture_item->ro.imageWidth, pCapture_item->ro.imageHeight,
                        pCapture_item->ro.imageWidth >> 1, pCapture_item->ro.imageHeight >> 1,
                        pCapture_item->ro.__pSubStream->imageWidth, pCapture_item->ro.__pSubStream->imageHeight,
                        NULL, pH264Sub->frameIndex, pH264Sub->stitchIndex, pH264Sub->cutLeft, pH264Sub->cutRight, pCapture_item->ro.classIndex, pH264Sub->ipsrc, pH264Sub->frameRate, pH264Sub->capture_uptime, pH264Sub->capture_gmtTime);
                pH264Sub = (FsObjectH264*) pCapture_item->ro.__pSubStream->__pData[0];
            }
            fs_Object_addRefer_pthreadSafety(pH264, __LINE__);
            image_frame_set__OI_2(pFrame, (FsObject*) pH264, 0);
            image_frame_set__OI_2(pFrame, (FsObject*) pH264Sub, 2);
            if (pCapture_item->ro.__pSubStream->dataCount-- > 1) {
                if (1 == pCapture_item->ro.__pSubStream->dataCount) {
                    pCapture_item->ro.__pSubStream->__pData[0] = pCapture_item->ro.__pSubStream->__pData[1];
                } else {
                    FsPrintf(1, "Data buffer sub is too many(=%hhd),frameRate=%f.\n", pCapture_item->ro.__pSubStream->dataCount, pH264->frameRate);
                    fflush(stdout);
                    if (2 == pCapture_item->ro.__pSubStream->dataCount) {
                        pCapture_item->ro.__pSubStream->__pData[0] = pCapture_item->ro.__pSubStream->__pData[2];
                        image_frame_set__OI_2(pFrame, pCapture_item->ro.__pSubStream->__pData[1], 2);
                    } else {
                        pCapture_item->ro.__pSubStream->__pData[0] = pCapture_item->ro.__pSubStream->__pData[3];
                        image_frame_set__OI_2(pFrame, pCapture_item->ro.__pSubStream->__pData[1], 2);
                        image_frame_set__OI_2(pFrame, pCapture_item->ro.__pSubStream->__pData[2], 2);
                    }
                    pCapture_item->ro.__pSubStream->dataCount = 1;
                }
            }
            pMonitorItem->rw.line = 40000 + __LINE__;
            fs_ObjectList_insert_tail(pCapture_item->ro.__framelistOut, pFrame);
            Fs_GroupSqrtOut_value_set(pCapture_item->rw._pGroupSqrtOut, &pCapture_item->ro._pCapture->ro._pGroupSqrtOut->groupSqrt_mutex);
            static int lostdata = 0;
            if (pCapture_item->ro.__framelistOut->nodeCount > 32) {
                if (lostdata++ > 1024) {
                    static int tttt = 0;
                    if (tttt-- < 0) {
                        FsPrintf(1, "List count(=%lu) too long.\n", pCapture_item->ro.__framelistOut->nodeCount);
                    } else tttt = 1000;
                    fs_Object_delete_pthreadSafety__OI(fs_ObjectList_remove_head(pCapture_item->ro.__framelistOut));
                }
            } else if (lostdata > 0)lostdata--;
            pMonitorItem->rw.line = 40000 + __LINE__;
        } else if (0 == pCapture_item->ro.__pSubStream->dataCount) {
            /* 无缓存数据,把数据写入缓存 */
            fs_Object_addRefer_pthreadSafety(pH264, __LINE__);
            pCapture_item->ro.__pSubStream->__pData[0] = (FsObject*) pH264;
            pCapture_item->ro.__pSubStream->dataCount = -1;
        } else {
            /* 有缓存主码流 */
            if (-4 == pCapture_item->ro.__pSubStream->dataCount) {
                /* 缓存已满,清空最早的数据 */
                FsPrintf(1, "Data buffer Main is full,frameRate=%f.\n", pH264->frameRate);
                fflush(stdout);
                pCapture_item->ro.__pSubStream->dataCount++;
                fs_Object_delete_pthreadSafety__OI(pCapture_item->ro.__pSubStream->__pData[0]);
                pCapture_item->ro.__pSubStream->__pData[0] = pCapture_item->ro.__pSubStream->__pData[1];
                pCapture_item->ro.__pSubStream->__pData[1] = pCapture_item->ro.__pSubStream->__pData[2];
                pCapture_item->ro.__pSubStream->__pData[2] = pCapture_item->ro.__pSubStream->__pData[3];
            }
            fs_Object_addRefer_pthreadSafety(pH264, __LINE__);
            pCapture_item->ro.__pSubStream->__pData[-(pCapture_item->ro.__pSubStream->dataCount--)] = (FsObject*) pH264;
        }
        pthread_mutex_unlock(&pCapture_item->ro.__framelistOut->mutex);
        return 1;
    } else {
        FsPrintf(1, "Invalid type(=%hu),%p.\n", pObject->type, pObject);
        fflush(stdout);
        return -1;
    }
}

/* rtsp相机的回调函数(子码流),成功返回1,失败返回-1 */
static signed char capture_P_camera_rtsp_cb_sub(FsObject* pObject, /* 采集项 */ struct Capture_item *pCapture_item, MonitorItem *pMonitorItem) {
    if (FsObject_type_h264 == pObject->type) {
        FsObjectH264 *pH264 = (FsObjectH264*) pObject;
        if (pCapture_item->ro.__pSubStream->imageWidth != pH264->width || pCapture_item->ro.__pSubStream->imageHeight != pH264->height) {
            pCapture_item->ro.__pSubStream->imageWidth = pH264->width;
            pCapture_item->ro.__pSubStream->imageHeight = pH264->height;
            if (pCapture_item->ro.classIndex < FsNumberMax(pCapture_item->ro.classIndex)) pCapture_item->ro.classIndex++;
            else pCapture_item->ro.classIndex = 0;
            /* 清空缓存 */
            if (pCapture_item->ro.__pSubStream->dataCount != 0) {
                int count = pCapture_item->ro.__pSubStream->dataCount > 0 ? pCapture_item->ro.__pSubStream->dataCount : - pCapture_item->ro.__pSubStream->dataCount;
                pCapture_item->ro.__pSubStream->dataCount = 0;
                while (count-- > 0) {
                    fs_Object_delete_pthreadSafety__OI(pCapture_item->ro.__pSubStream->__pData[count]);
                }
            }
        }
        pthread_mutex_lock(&pCapture_item->ro.__framelistOut->mutex);
        pMonitorItem->rw.line = 40000 + __LINE__;
        pH264->classIndex = pCapture_item->ro.classIndex;
        if (pCapture_item->ro.__pSubStream->dataCount < 0) {
            /* 有缓存主码流,输出 */
            FsObjectH264 * const pH264Main = (FsObjectH264*) pCapture_item->ro.__pSubStream->__pData[0];
            FsObjectImageFrame *pFrame = image_frame_new__IO(pCapture_item->ro.imageWidth, pCapture_item->ro.imageHeight
                    , pCapture_item->ro.imageWidth >> 1, pCapture_item->ro.imageHeight >> 1
                    , pCapture_item->ro.__pSubStream->imageWidth, pCapture_item->ro.__pSubStream->imageHeight
                    , NULL, pH264->frameIndex, pH264->stitchIndex, pH264->cutLeft, pH264->cutRight, pCapture_item->ro.classIndex, pH264->ipsrc, pH264->frameRate, pH264->capture_uptime, pH264->capture_gmtTime);
            image_frame_set__OI_2(pFrame, (FsObject*) pH264Main, 0);
            fs_Object_addRefer_pthreadSafety(pH264, __LINE__);
            image_frame_set__OI_2(pFrame, (FsObject*) pH264, 2);
            if (pCapture_item->ro.__pSubStream->dataCount++ < -1) {
                if (-1 == pCapture_item->ro.__pSubStream->dataCount) {
                    pCapture_item->ro.__pSubStream->__pData[0] = pCapture_item->ro.__pSubStream->__pData[1];
                } else {
                    FsPrintf(1, "Data buffer main is too many(=%hhd),frameRate=%f.\n", pCapture_item->ro.__pSubStream->dataCount, pH264->frameRate);
                    fflush(stdout);
                    if (-2 == pCapture_item->ro.__pSubStream->dataCount) {
                        pCapture_item->ro.__pSubStream->__pData[0] = pCapture_item->ro.__pSubStream->__pData[2];
                        image_frame_set__OI_2(pFrame, pCapture_item->ro.__pSubStream->__pData[1], 0);
                    } else {
                        pCapture_item->ro.__pSubStream->__pData[0] = pCapture_item->ro.__pSubStream->__pData[3];
                        image_frame_set__OI_2(pFrame, pCapture_item->ro.__pSubStream->__pData[1], 0);
                        image_frame_set__OI_2(pFrame, pCapture_item->ro.__pSubStream->__pData[2], 0);
                    }
                    pCapture_item->ro.__pSubStream->dataCount = -1;
                }
            }
            pMonitorItem->rw.line = 40000 + __LINE__;
            fs_ObjectList_insert_tail(pCapture_item->ro.__framelistOut, pFrame);
            Fs_GroupSqrtOut_value_set(pCapture_item->rw._pGroupSqrtOut, &pCapture_item->ro._pCapture->ro._pGroupSqrtOut->groupSqrt_mutex);
            static int lostdata = 0;
            if (pCapture_item->ro.__framelistOut->nodeCount > 32) {
                if (lostdata++ > 1024) {
                    static int tttt = 0;
                    if (tttt-- < 0) {
                        FsPrintf(1, "List count(=%lu) too long.\n", pCapture_item->ro.__framelistOut->nodeCount);
                    } else tttt = 1000;
                    fs_Object_delete_pthreadSafety__OI(fs_ObjectList_remove_head(pCapture_item->ro.__framelistOut));
                }
            } else if (lostdata > 0)lostdata--;
            pMonitorItem->rw.line = 40000 + __LINE__;
        } else if (0 == pCapture_item->ro.__pSubStream->dataCount) {
            /* 无缓存数据,把数据写入缓存 */
            fs_Object_addRefer_pthreadSafety(pH264, __LINE__);
            pCapture_item->ro.__pSubStream->__pData[0] = (FsObject*) pH264;
            pCapture_item->ro.__pSubStream->dataCount = 1;
        } else {
            /* 有缓存子码流 */
            if (4 == pCapture_item->ro.__pSubStream->dataCount) {
                /* 缓存已满,清空最早的数据 */
                FsPrintf(1, "Data buffer Sub is full,frameRate=%f.\n", pH264->frameRate);
                fflush(stdout);
                pCapture_item->ro.__pSubStream->dataCount--;
                fs_Object_delete_pthreadSafety__OI(pCapture_item->ro.__pSubStream->__pData[0]);
                pCapture_item->ro.__pSubStream->__pData[0] = pCapture_item->ro.__pSubStream->__pData[1];
                pCapture_item->ro.__pSubStream->__pData[1] = pCapture_item->ro.__pSubStream->__pData[2];
                pCapture_item->ro.__pSubStream->__pData[2] = pCapture_item->ro.__pSubStream->__pData[3];
            }
            fs_Object_addRefer_pthreadSafety(pH264, __LINE__);
            pCapture_item->ro.__pSubStream->__pData[pCapture_item->ro.__pSubStream->dataCount++] = (FsObject*) pH264;
        }
        pthread_mutex_unlock(&pCapture_item->ro.__framelistOut->mutex);

        return 1;
    } else {
        FsPrintf(1, "Invalid type(=%hu),%p.\n", pObject->type, pObject);
        fflush(stdout);
        return -1;
    }
}

/* localPush相机的回调函数,成功返回1,失败返回-1 */
static int capture_P_camera_push_cb(FsObjectImageFrame * const pFrame, /* 采集项 */ struct Capture_item * const pCapture_item, MonitorItem * const pMonitorItem) {
#ifndef __capture_P_camera_push_cb // 2022-03-28.1.20
    fs_Object_addRefer_pthreadSafety(pFrame, __LINE__);
    fs_ObjectList_insert_tail_pthreadSafety(pCapture_item->ro.__framelistOut, pFrame);
    Fs_GroupSqrtOut_value_set(pCapture_item->rw._pGroupSqrtOut, &pCapture_item->ro._pCapture->ro._pGroupSqrtOut->groupSqrt_mutex);
    static int lostdata = 0;
    if (pCapture_item->ro.__framelistOut->nodeCount > 32) {
        if (lostdata++ > 1024) {
            pthread_mutex_lock(&pCapture_item->ro.__framelistOut->mutex);
            if (pCapture_item->ro.__framelistOut->nodeCount > 32) {
                static int tttt = 0;
                if (tttt-- < 0) {
                    FsPrintf(1, "List count(=%lu) too long.\n", pCapture_item->ro.__framelistOut->nodeCount);
                } else tttt = 1000;
                fs_Object_delete_pthreadSafety__OI(fs_ObjectList_remove_head(pCapture_item->ro.__framelistOut));
            }
            pthread_mutex_unlock(&pCapture_item->ro.__framelistOut->mutex);
        }
    } else if (lostdata > 0)lostdata--;
    return 1;
#endif
}

/* localPuLL相机的回调函数,成功返回1,失败返回-1 */
static int capture_P_camera_pull_cb(FsObjectImageFrame * const pFrame, /* 采集项 */ struct Capture_item * const pCapture_item, MonitorItem * const pMonitorItem) {
#ifndef __capture_P_camera_push_cb // 2022-03-28.1.20
    fs_Object_addRefer_pthreadSafety(pFrame, __LINE__);
    fs_ObjectList_insert_tail_pthreadSafety(pCapture_item->ro.__framelistOut, pFrame);
    Fs_GroupSqrtOut_value_set(pCapture_item->rw._pGroupSqrtOut, &pCapture_item->ro._pCapture->ro._pGroupSqrtOut->groupSqrt_mutex);
    static int lostdata = 0;
    if (pCapture_item->ro.__framelistOut->nodeCount > 32) {
        if (lostdata++ > 1024) {
            pthread_mutex_lock(&pCapture_item->ro.__framelistOut->mutex);
            if (pCapture_item->ro.__framelistOut->nodeCount > 32) {
                static int tttt = 0;
                if (tttt-- < 0) {
                    FsPrintf(1, "List count(=%lu) too long.\n", pCapture_item->ro.__framelistOut->nodeCount);
                } else tttt = 1000;
                fs_Object_delete_pthreadSafety__OI(fs_ObjectList_remove_head(pCapture_item->ro.__framelistOut));
            }
            pthread_mutex_unlock(&pCapture_item->ro.__framelistOut->mutex);
        }
    } else if (lostdata > 0)lostdata--;
    return 1;
#endif
}

/* inflray相机的回调函数,成功返回1,失败返回-1 */
static int capture_P_camera_inflray_cb(FsObjectImageFrame * const pFrame, /* 采集项 */ struct Capture_item * const pCapture_item, MonitorItem * const pMonitorItem) {
#ifndef __capture_P_camera_push_cb // 2022-03-28.1.20
    fs_Object_addRefer_pthreadSafety(pFrame, __LINE__);
    fs_ObjectList_insert_tail_pthreadSafety(pCapture_item->ro.__framelistOut, pFrame);
    Fs_GroupSqrtOut_value_set(pCapture_item->rw._pGroupSqrtOut, &pCapture_item->ro._pCapture->ro._pGroupSqrtOut->groupSqrt_mutex);
    static int lostdata = 0;
    if (pCapture_item->ro.__framelistOut->nodeCount > 32) {
        if (lostdata++ > 1024) {
            pthread_mutex_lock(&pCapture_item->ro.__framelistOut->mutex);
            if (pCapture_item->ro.__framelistOut->nodeCount > 32) {
                static int tttt = 0;
                if (tttt-- < 0) {
                    FsPrintf(1, "List count(=%lu) too long.\n", pCapture_item->ro.__framelistOut->nodeCount);
                } else tttt = 1000;
                fs_Object_delete_pthreadSafety__OI(fs_ObjectList_remove_head(pCapture_item->ro.__framelistOut));
            }
            pthread_mutex_unlock(&pCapture_item->ro.__framelistOut->mutex);
        }
    } else if (lostdata > 0)lostdata--;
    return 1;
#endif
}

/* 从配置获取通道数 */
static unsigned int capture_P_get_channelCount(/* 可为空 */FsConfig * const pConfig, /* 本地ip地址 */const FsArray * const ipList) {
#ifndef __get_channelCount_vsys_vsysChannel 
    if (NULL == pConfig) {
#ifdef __get_channelCount_vsys_vsysChannel_in_vsys
        *rst_pVsysChannel0 = NULL;
        return fs_ObjectList_new__IO(4);
#else
        return 0;
#endif    
    }
    /* 通道数 */
    const void *vsys0 = pConfig;
    const void *vsys;
    {
        FsObjectList * const list = fs_Config_node_template__IO(pConfig, &vsys0, pConfig, 0,ipList, 0, "vsys");
        if (NULL == list) {
#ifdef __get_channelCount_vsys_vsysChannel_in_vsys
            *rst_pVsysChannel0 = NULL;
            return fs_ObjectList_new__IO(4);
#else
            return 0;
#endif   
        }
        vsys = list->pNode[list->startIndex];
        fs_ObjectList_delete__OI(list);
    }
    {
#ifdef __get_channelCount_vsys_vsysChannel_in_vsys
        *pCronfremethreshold = fs_Config_node_integer_get_first(pConfig, vsys0, vsys, "cronframethreshold", 1, NULL);
        *pCannelstatuscheckmode = fs_Config_node_integer_get_first(pConfig, vsys0, vsys, "channelstatuscheckmode", 0, NULL);

#endif
#ifdef __get_channelCount_vsys_vsysChannel_in_record
        if (pRecord->p.gbsdkConfig__videoInfoDataClientList) {
#define __record_P_update_sdk_gb28181
#ifndef __record_P_new_sdk_gb28181 
            if (Vsys_sdkPlayBack_is_process()) {
#ifdef __record_P_update_sdk_gb28181
                FsEbml * const gbsdkConfig = pRecord->p.gbsdkConfig__videoInfoDataClientList;
                struct FsEbml_node * const gb28181Config = fs_Ebml_node_get_first(gbsdkConfig, (struct FsEbml_node*) gbsdkConfig, "gb28181Config");
#else
                struct FsEbml_node * const gb28181Config = fs_Ebml_node_addChild(gbsdkConfig, (struct FsEbml_node*) gbsdkConfig, "gb28181Config", FsEbmlNodeType_Struct);
#endif
                {
                    const FsString * const ipv4_streamPort_PlayBack = fs_Config_node_string_get_first_String(pConfig, vsys0, vsys, "gb28181Config ipv4_streamPort_PlayBack", NULL);
#ifdef __record_P_update_sdk_gb28181
                    struct FsEbml_node * const pEbml_node_ipv4_streamPort_PlayBack = fs_Ebml_node_get_first(gbsdkConfig, gb28181Config, "ipv4_streamPort_PlayBack");
                    if (pEbml_node_ipv4_streamPort_PlayBack->data.lenth != ipv4_streamPort_PlayBack->lenth
                            || memcmp(pEbml_node_ipv4_streamPort_PlayBack->data.buffer, ipv4_streamPort_PlayBack->buffer, ipv4_streamPort_PlayBack->lenth) != 0) {
                        pthread_mutex_lock(&pRecord->ro.__videoInfoDataClientList->mutex);
                        fs_Ebml_node_data_set(pEbml_node_ipv4_streamPort_PlayBack, ipv4_streamPort_PlayBack->lenth, ipv4_streamPort_PlayBack->buffer);
                        pthread_mutex_unlock(&pRecord->ro.__videoInfoDataClientList->mutex);
                        Record_sdkConfigUpdate_set_0_1(pRecord->p._sdkConfigUpdate, Record_sdkConfigUpdate_index_gb);
                    }
#else             
                    fs_Ebml_node_data_set(
#ifdef Record_sdkConfig_comment
                            fs_Ebml_node_addChild_with_a_property
#else
                            fs_Ebml_node_addChild
#endif
                            (gbsdkConfig, gb28181Config, "ipv4_streamPort_PlayBack", FsEbmlNodeType_String
#ifdef Record_sdkConfig_comment
                            , "comment", "回放使用的端口号"
#endif
                            ), ipv4_streamPort_PlayBack->lenth, ipv4_streamPort_PlayBack->buffer);
#endif
                }
                {
                    FsObjectList * const addrmapList = fs_Config_node_string_get_list__IO(pConfig, vsys0, vsys, "addrmap", NULL);
#ifdef __record_P_update_sdk_gb28181
                    FsObjectList * const addrmapList1 = fs_Ebml_node_get__IO(gbsdkConfig, gb28181Config, "addrmap");
                    if (addrmapList) {
                        if (addrmapList1 != NULL) {
                            const FsString **ppNode = (const FsString **) addrmapList->pNode + addrmapList->startIndex;
                            struct FsEbml_node **ppNode1 = (struct FsEbml_node **) addrmapList1->pNode + addrmapList1->startIndex;
                            unsigned int ui = addrmapList1->nodeCount;
                            if (ui > (unsigned int) addrmapList->nodeCount) {
                                pthread_mutex_lock(&pRecord->ro.__videoInfoDataClientList->mutex);
                                do {
                                    fs_Ebml_node_delete(gbsdkConfig, ppNode1[--ui]);
                                } while (ui > (unsigned int) addrmapList->nodeCount);
                                pthread_mutex_unlock(&pRecord->ro.__videoInfoDataClientList->mutex);
                                Record_sdkConfigUpdate_set_0_1(pRecord->p._sdkConfigUpdate, Record_sdkConfigUpdate_index_gb);
                            }
                            while (ui-- > 0) {
                                const FsString * const addrmap = *ppNode++;
                                struct FsEbml_node * const addrmap1 = *ppNode1++;
                                if (addrmap1->data.lenth != addrmap->lenth || memcmp(addrmap1->data.buffer, addrmap->buffer, addrmap->lenth) != 0) {
                                    pthread_mutex_lock(&pRecord->ro.__videoInfoDataClientList->mutex);
                                    fs_Ebml_node_data_set(addrmap1, addrmap->lenth, addrmap->buffer);
                                    pthread_mutex_unlock(&pRecord->ro.__videoInfoDataClientList->mutex);
                                    Record_sdkConfigUpdate_set_0_1(pRecord->p._sdkConfigUpdate, Record_sdkConfigUpdate_index_gb);
                                }
                            }
                            if (addrmapList->nodeCount > addrmapList1->nodeCount) {
                                ui = addrmapList->nodeCount - addrmapList1->nodeCount;
                                pthread_mutex_lock(&pRecord->ro.__videoInfoDataClientList->mutex);
                                do {
                                    const FsString * const addrmap = *ppNode++;
                                    fs_Ebml_node_data_set(
#ifdef Record_sdkConfig_comment
                                            fs_Ebml_node_addChild_with_a_property
#else
                                            fs_Ebml_node_addChild
#endif
                                            (gbsdkConfig, gb28181Config, "addrmap", FsEbmlNodeType_String
#ifdef Record_sdkConfig_comment
                                            , "comment", "本地IP映射,在一些应用场景中无法知道本机映射的ip时使用此数据作为映射,格式:'ip/掩码 映射地址 权重',映射地址可为域名,权重空时值为1"
#endif
                                            )
                                            , addrmap->lenth, addrmap->buffer);
                                } while (--ui > 0);
                                pthread_mutex_unlock(&pRecord->ro.__videoInfoDataClientList->mutex);
                                Record_sdkConfigUpdate_set_0_1(pRecord->p._sdkConfigUpdate, Record_sdkConfigUpdate_index_gb);
                            }
                            fs_ObjectList_delete__OI(addrmapList);
                            fs_ObjectList_delete__OI(addrmapList1);
                        } else {
                            pthread_mutex_lock(&pRecord->ro.__videoInfoDataClientList->mutex);
#ifndef __record_P_new_sdk_gb28181_do_addrmapList 
                            const FsString **ppNode = (const FsString **) addrmapList->pNode + addrmapList->startIndex;
                            unsigned int ui = addrmapList->nodeCount;
                            while (ui-- > 0) {
                                const FsString * const addrmap = *ppNode++;
                                fs_Ebml_node_data_set(
#ifdef Record_sdkConfig_comment
                                        fs_Ebml_node_addChild_with_a_property
#else
                                        fs_Ebml_node_addChild
#endif
                                        (gbsdkConfig, gb28181Config, "addrmap", FsEbmlNodeType_String
#ifdef Record_sdkConfig_comment
                                        , "comment", "本地IP映射,在一些应用场景中无法知道本机映射的ip时使用此数据作为映射,格式:'ip/掩码 映射地址 权重',映射地址可为域名,权重空时值为1"
#endif
                                        )
                                        , addrmap->lenth, addrmap->buffer);
                            }
                            fs_ObjectList_delete__OI(addrmapList);
#endif
                            pthread_mutex_unlock(&pRecord->ro.__videoInfoDataClientList->mutex);
                            Record_sdkConfigUpdate_set_0_1(pRecord->p._sdkConfigUpdate, Record_sdkConfigUpdate_index_gb);
                        }
                    } else if (addrmapList1) {
                        fs_ObjectList_delete__OI(addrmapList1);
                        pthread_mutex_lock(&pRecord->ro.__videoInfoDataClientList->mutex);
                        fs_Ebml_node_delete_child_byString(gbsdkConfig, gb28181Config, "addrmap");
                        pthread_mutex_unlock(&pRecord->ro.__videoInfoDataClientList->mutex);
                        Record_sdkConfigUpdate_set_0_1(pRecord->p._sdkConfigUpdate, Record_sdkConfigUpdate_index_gb);
                    }
#else
                    if (addrmapList) {
#ifndef __record_P_new_sdk_gb28181_do_addrmapList
                        const FsString **ppNode = (const FsString **) addrmapList->pNode + addrmapList->startIndex;
                        unsigned int ui = addrmapList->nodeCount;
                        while (ui-- > 0) {
                            const FsString * const addrmap = *ppNode++;
                            fs_Ebml_node_data_set(
#ifdef Record_sdkConfig_comment
                                    fs_Ebml_node_addChild_with_a_property
#else
                                    fs_Ebml_node_addChild
#endif
                                    (gbsdkConfig, gb28181Config, "addrmap", FsEbmlNodeType_String
#ifdef Record_sdkConfig_comment
                                    , "comment", "本地IP映射,在一些应用场景中无法知道本机映射的ip时使用此数据作为映射,格式:'ip/掩码 映射地址 权重',映射地址可为域名,权重空时值为1"
#endif
                                    )
                                    , addrmap->lenth, addrmap->buffer);
                        }
                        fs_ObjectList_delete__OI(addrmapList);
#endif
                    }
#endif
                }
            }
#ifdef __record_P_update_sdk_gb28181
#undef __record_P_update_sdk_gb28181
#endif
#endif
        }
        pRecord->rw._snapbuffertimeout = fs_Config_node_float_get_first(pConfig, vsys0, vsys, "snapbuffertimeout", 0.0, NULL);
#endif
        FsObjectList * const list = fs_Config_node_template__IO(pConfig, &vsys0, vsys, 0,NULL, 0, "vsysChannel");
        if (NULL == list) {
#ifdef __get_channelCount_vsys_vsysChannel_in_vsys
            *rst_pVsysChannel0 = NULL;
            return fs_ObjectList_new__IO(4);
#else
            return 0;
#endif   
        } else {
#ifdef __get_channelCount_vsys_vsysChannel_in_vsys
            *rst_pVsysChannel0 = vsys0;
            return list;
#else
            const unsigned int rst = list->nodeCount;
            fs_ObjectList_delete__OI(list);
            return rst;
#endif   
        }
    }
#ifndef __get_channelCount_vsys_vsysChannel_in_vsys
#undef __get_channelCount_vsys_vsysChannel_in_vsys
#endif
#ifdef __get_channelCount_vsys_vsysChannel_in_record
#undef __get_channelCount_vsys_vsysChannel_in_record
#endif
#endif
}

/* 发送相机流控制,其中streamCtrlStatusClientList已被加锁 */
static FsEbml * const capture_P_item_get_streamCtrlStatus__IO(struct Capture_item * const pCapture_item, /* 流控制状态,0-控制流停止,1-控制流开始 */unsigned char streamCtrlStatus) {
    FsEbml * const pEbml = fs_Ebml_new__IO(FsEbmlEncodeMethod_UTF8, 0);
    fs_Ebml_node_data_set_string(fs_Ebml_node_addChild_with_a_property(pEbml, (struct FsEbml_node*) pEbml, "uuid", FsEbmlNodeType_String
            , "comment", "相机的uuid"), pCapture_item->ro._uuid);
    * (long long*) fs_Ebml_node_addChild_with_a_property(pEbml, (struct FsEbml_node*) pEbml, "streamCtrlStatus", FsEbmlNodeType_Integer
            , "comment", "流控制状态,0-控制流停止,1-控制流开始")->data.buffer = streamCtrlStatus;
    return pEbml;
}

/* 发送相机流控制,其中streamCtrlStatusClientList已被加锁 */
static void capture_P_item_send_streamCtrlStatus_locked(struct Capture_item * const pCapture_item, /* 流控制状态,0-控制流停止,1-控制流开始 */unsigned char streamCtrlStatus
        , void* const pConfigManager, /* 获取拉流状态的客户端链表 */ FsStructList * const streamCtrlStatusClientList_
        , /* 缓存Buffer,可为空 */FsObjectBaseBuffer * const pObjectBaseBuffer) {
    if (0 == streamCtrlStatusClientList_->nodeCount)return;
    FsEbml * const pEbml = capture_P_item_get_streamCtrlStatus__IO(pCapture_item, streamCtrlStatus);
    if (pObjectBaseBuffer) configManager_conncet_refer_send(pConfigManager, streamCtrlStatusClientList_, NULL, NULL, NULL, pEbml, 0x2, pObjectBaseBuffer);
    else {
        FsObjectBaseBuffer baseBuffer;
        fs_ObjectBaseBuffer_init(&baseBuffer, 1);
        configManager_conncet_refer_send(pConfigManager, streamCtrlStatusClientList_, NULL, NULL, NULL, pEbml, 0x2, &baseBuffer);
        fs_ObjectBaseBuffer_release(&baseBuffer);
    }
    fs_Ebml_delete__OI(pEbml, NULL);
}

/* 删除pCapture_item指向的实例对象 */
static void capture_P_item_delete__OI(struct Capture_item * const pCapture_item, /* 相机控制 */ Camera_ctrl * * const ppCamera_ctrl
        , /* 缓存Buffer,不为空 */FsObjectBaseBuffer * const pObjectBaseBuffer, /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer) {
    if (NULL == pCapture_item)return;
    {
#define __capture_P_item_delete__OI_1
#ifndef __capture_P_item_delete__OI 
#ifndef __capture_P_item_delete__OI_1
        if (0 == pCapture_item->ro._ipv4) configManager_cmd_logoff(pCapture_item->ro._pCapture->ro._pConfigManager, "camctl", pCapture_item->ro._uuid, 0, pCapture_item, pShareBuffer);
        configManager_cmd_logoff(pCapture_item->ro._pCapture->ro._pConfigManager, "cameractrl", pCapture_item->ro._uuid, pCapture_item->ro._ipv4, pCapture_item, pShareBuffer);
        configManager_cmd_logoff(pCapture_item->ro._pCapture->ro._pConfigManager, "errorInfo", pCapture_item->ro._uuid, pCapture_item->ro._ipv4, pCapture_item, pShareBuffer);
#endif
        if (0 == pCapture_item->ro._ipv4) {
#ifndef __capture_P_item_delete__OI_1
            /* 断开实时视频掩码通信 */
            configManager_mask_disconnect_pthreadSafety(pCapture_item->ro._pCapture->ro._pConfigManager, "realStream", pCapture_item->ro._uuid, pCapture_item);
            /* 断开获取错误信息的指令 */
            configManager_transfer_disconnect(pCapture_item->ro._pCapture->ro._pConfigManager, "error_distribution", pCapture_item->ro._uuid, pCapture_item);
            /* 删除相机 */
            pCapture_item->ro._camera_remove_and_delete_item_and_camera(pCapture_item->ro._ppCamera, pCapture_item->ro.__pCamera_item, pShareBuffer);
#endif

#ifndef __capture_P_item_delete__OI_1
            /* 子码流,为空表示不存在子码流 */
            if (pCapture_item->ro.__pSubStream) {
                /* 相机项 */
                pCapture_item->ro.__pSubStream->_camera_remove_and_delete_item_and_emptyCamera(pCapture_item->ro.__pSubStream->_pCameraList_, pCapture_item->ro.__pSubStream->__pCamera_item);
                /* 缓存的未输出的数据帧 */
                if (pCapture_item->ro.__pSubStream->dataCount > 0) {
                    do {
                        fs_Object_delete__OI(pCapture_item->ro.__pSubStream->__pData[--pCapture_item->ro.__pSubStream->dataCount]);
                    } while (pCapture_item->ro.__pSubStream->dataCount > 0);
                } else if (pCapture_item->ro.__pSubStream->dataCount < 0) {
                    do {
                        fs_Object_delete__OI(pCapture_item->ro.__pSubStream->__pData[-(++pCapture_item->ro.__pSubStream->dataCount)]);
                    } while (pCapture_item->ro.__pSubStream->dataCount < 0);
                }
                fsFree(pCapture_item->ro.__pSubStream);
            }
#endif
            /* 传出的数据帧链表 */
#ifndef __capture_P_item_delete__OI_1
            fs_ObjectList_delete_custom__OI(pCapture_item->ro.__framelistOut, fs_Object_delete_pthreadSafety__OI);
#endif
            /* 缓存的控制客户端连接 */
#ifndef __capture_P_item_delete__OI_1
#ifdef FsDebug
            if ((0 == pCapture_item->p.ctrlPriority && (pCapture_item->p.__ctrlClientList != NULL && pCapture_item->p.__ctrlClientList->nodeCount != 0)) || (pCapture_item->p.ctrlPriority != 0 && (NULL == pCapture_item->p.__ctrlClientList || 0 == pCapture_item->p.__ctrlClientList->nodeCount))) {
                FsLog(FsLogType_fatal, FsPrintfIndex, "(0 == pCapture_item->p.ctrlPriority(=%hu) && (pCapture_item->p.__ctrlClientList(%p) != NULL && pCapture_item->p.__ctrlClientList->nodeCount(%lu) != 0)) || (pCapture_item->p.ctrlPriority(=%hu) != 0 && (NULL == pCapture_item->p.__ctrlClientList(=%p) || 0 == pCapture_item->p.__ctrlClientList->nodeCount(=%lu))).\n",
                        pCapture_item->p.ctrlPriority, pCapture_item->p.__ctrlClientList, pCapture_item->p.__ctrlClientList != NULL ? pCapture_item->p.__ctrlClientList->nodeCount : 0, pCapture_item->p.ctrlPriority, pCapture_item->p.__ctrlClientList, pCapture_item->p.__ctrlClientList != NULL ? pCapture_item->p.__ctrlClientList->nodeCount : 0);
                fflush(stdout);
                FsLogExit();
            }
#endif
            if (pCapture_item->p.__ctrlClientList) {
                if (pCapture_item->p.ctrlPriority != 0) {
                    pCapture_item->ro._ctrl_function->cancelKey_pthreadSafety(pCapture_item->ro.__pCamera_item, pCapture_item->ro.__pCamera_ctrl_item, pCapture_item, pObjectBaseBuffer, pShareBuffer);
                    //pCapture_item->p.ctrlPriority = 0;
                }
                //FsPrintf(1, "-----------------------pCapture_item->p.__ctrolClientList.c=%lu\n", pCapture_item->p.__ctrolClientList->nodeCount);
                unsigned int (*ppNode)[6] = ((unsigned int (*)[6])pCapture_item->p.__ctrlClientList->pNode) + pCapture_item->p.__ctrlClientList->startIndex;
                unsigned ui = pCapture_item->p.__ctrlClientList->nodeCount;
                for (; ui > 0; ui--) {
                    unsigned int *const requestData = *ppNode++;
                    //                unsigned int ipv4;
                    //                unsigned short ipv4Port;
                    //                configManager_connect_get_addr(&ipv4, &ipv4Port, requestData);
                    //                FsPrintf2(1, "client:%#lx,%x:%hhu.%hhu.%hhu.%hhu:%hu\n", *(long*) requestData, requestData[2],
                    //                        ((unsigned char*) &ipv4)[0], ((unsigned char*) &ipv4)[1], ((unsigned char*) &ipv4)[2], ((unsigned char*) &ipv4)[3], ipv4Port);
                    configManager_connect_release(pCapture_item->ro._pCapture->ro._pConfigManager, requestData, 1);
                }
                fs_StructList_delete__OI(pCapture_item->p.__ctrlClientList);
            }
#endif
            /* 获取错误数据的客户端 */
#ifndef __capture_P_item_delete__OI_1
            if (pCapture_item->p.__errorInfoClientList) {
                configManager_conncet_refer_close(pCapture_item->ro._pCapture->ro._pConfigManager, pCapture_item->p.__errorInfoClientList, 1);
                fs_StructList_delete__OI(pCapture_item->p.__errorInfoClientList);
            }
#endif
#ifndef __capture_P_item_delete__OI_1
            {
                struct Capture * const pCapture = pCapture_item->ro._pCapture;
                if (pCapture->p.streamCtrlStartItemListOrder__streamCtrlStatusClientList->nodeCount > 0) {
                    pthread_mutex_lock(&pCapture->p.streamCtrlStatusClientList->mutex);
                    if (fs_ObjectList_remove_order(pCapture->p.streamCtrlStartItemListOrder__streamCtrlStatusClientList, pCapture_item) >= 0) {
                        /* 是正在拉流的相机,发送停止指令 */
                        capture_P_item_send_streamCtrlStatus_locked(pCapture_item, 0, pCapture->ro._pConfigManager, pCapture->p.streamCtrlStatusClientList, pObjectBaseBuffer);
                    }
                    pthread_mutex_unlock(&pCapture->p.streamCtrlStatusClientList->mutex);
                }
            }
#endif
            /* 内部控制的函数指针,可用于集群,通知收到的相机控制指令,连接其他本系统的平台进行相机控制 */
            if (pCapture_item->ro.__pCamera_ctrl_item) {
#ifdef __capture_P_item_delete__OI_1
                camera_ctrl_remove_item_noWait__IO_2(*ppCamera_ctrl, pCapture_item->ro.__pCamera_ctrl_item);
#else
                camera_ctrl_item_delete__OI(pCapture_item->ro.__pCamera_ctrl_item);
                if (0 == (*ppCamera_ctrl)->ro.__itemListOrder_->nodeCount) {
                    camera_ctrl_stopThread(*ppCamera_ctrl);
                    camera_ctrl_delete__OI(*ppCamera_ctrl);
                    *ppCamera_ctrl = NULL;
                }
#endif
            }
            /* 发送流给本地拉流对象 */
#ifndef __capture_P_item_delete__OI_1
            /* 发送流给本地拉流对象 */
            configManager_transfer_logoff(pCapture_item->ro._pCapture->ro._pConfigManager, pCapture_item->ro._pTransfer_error, pCapture_item);
#endif
        }
#ifdef __capture_P_item_delete__OI_1
#undef __capture_P_item_delete__OI_1
#endif
#endif
    }
    {
#ifndef __capture_P_item_delete__OI 
#ifndef __capture_P_item_delete__OI_1
        if (0 == pCapture_item->ro._ipv4) configManager_cmd_logoff(pCapture_item->ro._pCapture->ro._pConfigManager, "camctl", pCapture_item->ro._uuid, 0, pCapture_item, pShareBuffer);
        configManager_cmd_logoff(pCapture_item->ro._pCapture->ro._pConfigManager, "cameractrl", pCapture_item->ro._uuid, pCapture_item->ro._ipv4, pCapture_item, pShareBuffer);
        configManager_cmd_logoff(pCapture_item->ro._pCapture->ro._pConfigManager, "errorInfo", pCapture_item->ro._uuid, pCapture_item->ro._ipv4, pCapture_item, pShareBuffer);
#endif
        if (0 == pCapture_item->ro._ipv4) {
#ifndef __capture_P_item_delete__OI_1
            /* 断开实时视频掩码通信 */
            configManager_mask_disconnect_pthreadSafety(pCapture_item->ro._pCapture->ro._pConfigManager, "realStream", pCapture_item->ro._uuid, pCapture_item);
            /* 断开获取错误信息的指令 */
            configManager_transfer_disconnect(pCapture_item->ro._pCapture->ro._pConfigManager, "error_distribution", pCapture_item->ro._uuid, pCapture_item);
            /* 删除相机 */
            pCapture_item->ro._camera_remove_and_delete_item_and_camera(pCapture_item->ro._ppCamera, pCapture_item->ro.__pCamera_item, pShareBuffer);
#endif

#ifndef __capture_P_item_delete__OI_1
            /* 子码流,为空表示不存在子码流 */
            if (pCapture_item->ro.__pSubStream) {
                /* 相机项 */
                pCapture_item->ro.__pSubStream->_camera_remove_and_delete_item_and_emptyCamera(pCapture_item->ro.__pSubStream->_pCameraList_, pCapture_item->ro.__pSubStream->__pCamera_item);
                /* 缓存的未输出的数据帧 */
                if (pCapture_item->ro.__pSubStream->dataCount > 0) {
                    do {
                        fs_Object_delete__OI(pCapture_item->ro.__pSubStream->__pData[--pCapture_item->ro.__pSubStream->dataCount]);
                    } while (pCapture_item->ro.__pSubStream->dataCount > 0);
                } else if (pCapture_item->ro.__pSubStream->dataCount < 0) {
                    do {
                        fs_Object_delete__OI(pCapture_item->ro.__pSubStream->__pData[-(++pCapture_item->ro.__pSubStream->dataCount)]);
                    } while (pCapture_item->ro.__pSubStream->dataCount < 0);
                }
                fsFree(pCapture_item->ro.__pSubStream);
            }
#endif
            /* 传出的数据帧链表 */
#ifndef __capture_P_item_delete__OI_1
            fs_ObjectList_delete_custom__OI(pCapture_item->ro.__framelistOut, fs_Object_delete_pthreadSafety__OI);
#endif
            /* 缓存的控制客户端连接 */
#ifndef __capture_P_item_delete__OI_1
#ifdef FsDebug
            if ((0 == pCapture_item->p.ctrlPriority && (pCapture_item->p.__ctrlClientList != NULL && pCapture_item->p.__ctrlClientList->nodeCount != 0)) || (pCapture_item->p.ctrlPriority != 0 && (NULL == pCapture_item->p.__ctrlClientList || 0 == pCapture_item->p.__ctrlClientList->nodeCount))) {
                FsLog(FsLogType_fatal, FsPrintfIndex, "(0 == pCapture_item->p.ctrlPriority(=%hu) && (pCapture_item->p.__ctrlClientList(%p) != NULL && pCapture_item->p.__ctrlClientList->nodeCount(%lu) != 0)) || (pCapture_item->p.ctrlPriority(=%hu) != 0 && (NULL == pCapture_item->p.__ctrlClientList(=%p) || 0 == pCapture_item->p.__ctrlClientList->nodeCount(=%lu))).\n",
                        pCapture_item->p.ctrlPriority, pCapture_item->p.__ctrlClientList, pCapture_item->p.__ctrlClientList != NULL ? pCapture_item->p.__ctrlClientList->nodeCount : 0, pCapture_item->p.ctrlPriority, pCapture_item->p.__ctrlClientList, pCapture_item->p.__ctrlClientList != NULL ? pCapture_item->p.__ctrlClientList->nodeCount : 0);
                fflush(stdout);
                FsLogExit();
            }
#endif
            if (pCapture_item->p.__ctrlClientList) {
                if (pCapture_item->p.ctrlPriority != 0) {
                    pCapture_item->ro._ctrl_function->cancelKey_pthreadSafety(pCapture_item->ro.__pCamera_item, pCapture_item->ro.__pCamera_ctrl_item, pCapture_item, pObjectBaseBuffer, pShareBuffer);
                    //pCapture_item->p.ctrlPriority = 0;
                }
                //FsPrintf(1, "-----------------------pCapture_item->p.__ctrolClientList.c=%lu\n", pCapture_item->p.__ctrolClientList->nodeCount);
                unsigned int (*ppNode)[6] = ((unsigned int (*)[6])pCapture_item->p.__ctrlClientList->pNode) + pCapture_item->p.__ctrlClientList->startIndex;
                unsigned ui = pCapture_item->p.__ctrlClientList->nodeCount;
                for (; ui > 0; ui--) {
                    unsigned int *const requestData = *ppNode++;
                    //                unsigned int ipv4;
                    //                unsigned short ipv4Port;
                    //                configManager_connect_get_addr(&ipv4, &ipv4Port, requestData);
                    //                FsPrintf2(1, "client:%#lx,%x:%hhu.%hhu.%hhu.%hhu:%hu\n", *(long*) requestData, requestData[2],
                    //                        ((unsigned char*) &ipv4)[0], ((unsigned char*) &ipv4)[1], ((unsigned char*) &ipv4)[2], ((unsigned char*) &ipv4)[3], ipv4Port);
                    configManager_connect_release(pCapture_item->ro._pCapture->ro._pConfigManager, requestData, 1);
                }
                fs_StructList_delete__OI(pCapture_item->p.__ctrlClientList);
            }
#endif
            /* 获取错误数据的客户端 */
#ifndef __capture_P_item_delete__OI_1
            if (pCapture_item->p.__errorInfoClientList) {
                configManager_conncet_refer_close(pCapture_item->ro._pCapture->ro._pConfigManager, pCapture_item->p.__errorInfoClientList, 1);
                fs_StructList_delete__OI(pCapture_item->p.__errorInfoClientList);
            }
#endif
#ifndef __capture_P_item_delete__OI_1
            {
                struct Capture * const pCapture = pCapture_item->ro._pCapture;
                if (pCapture->p.streamCtrlStartItemListOrder__streamCtrlStatusClientList->nodeCount > 0) {
                    pthread_mutex_lock(&pCapture->p.streamCtrlStatusClientList->mutex);
                    if (fs_ObjectList_remove_order(pCapture->p.streamCtrlStartItemListOrder__streamCtrlStatusClientList, pCapture_item) >= 0) {
                        /* 是正在拉流的相机,发送停止指令 */
                        capture_P_item_send_streamCtrlStatus_locked(pCapture_item, 0, pCapture->ro._pConfigManager, pCapture->p.streamCtrlStatusClientList, pObjectBaseBuffer);
                    }
                    pthread_mutex_unlock(&pCapture->p.streamCtrlStatusClientList->mutex);
                }
            }
#endif
            /* 内部控制的函数指针,可用于集群,通知收到的相机控制指令,连接其他本系统的平台进行相机控制 */
            if (pCapture_item->ro.__pCamera_ctrl_item) {
#ifdef __capture_P_item_delete__OI_1
                camera_ctrl_remove_item_noWait__IO_2(*ppCamera_ctrl, pCapture_item->ro.__pCamera_ctrl_item);
#else
                camera_ctrl_item_delete__OI(pCapture_item->ro.__pCamera_ctrl_item);
                if (0 == (*ppCamera_ctrl)->ro.__itemListOrder_->nodeCount) {
                    camera_ctrl_stopThread(*ppCamera_ctrl);
                    camera_ctrl_delete__OI(*ppCamera_ctrl);
                    *ppCamera_ctrl = NULL;
                }
#endif
            }
            /* 发送流给本地拉流对象 */
#ifndef __capture_P_item_delete__OI_1
            /* 发送流给本地拉流对象 */
            configManager_transfer_logoff(pCapture_item->ro._pCapture->ro._pConfigManager, pCapture_item->ro._pTransfer_error, pCapture_item);
#endif
        }
#ifdef __capture_P_item_delete__OI_1
#undef __capture_P_item_delete__OI_1
#endif
#endif
    }
    fsFree(pCapture_item);
}

/* 在有用户请求此命令字时的调用函数,成功返回1,失败返回-1 */
static int camctl_private_cmd_cb(/* 与请求相关的信息,用于识别是发给哪个客户端的数据,用3个int来储存 */const unsigned int requestID_3[], /* 收到数据的前4字节 */unsigned int head
        , /* 收到的数据 */FsEbml *pEbml, /* 客户端发送请求的数据类型,1-ebml数据,2-xml数据,3-json数据 */ char requestDataType, /* 调用函数的指针 */ void* p
        , /* 缓存Buffer,不为空 */FsObjectBaseBuffer * const pObjectBaseBuffer, /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer) {
    struct Capture_item* pCapture_item = (struct Capture_item *) p;

    if (1) {
        fs_Ebml_analyzeAll(pEbml, (struct FsEbml_node*) pEbml);
        FsJson *pJson_1 = fs_Ebml_convert_to_Json__IO(pEbml);

        FsString * const info = fs_Json_to_String__IO(pJson_1, 0, 0, FsStringLenData("\r\n"), 0, NULL, 4);

        printf("\n%s\n", info->buffer);
        fs_Json_delete__OI(pJson_1, pShareBuffer);
        fs_String_delete__OI(info);
    }

    struct FsEbml_node *cmd = fs_Ebml_node_get_first(pEbml, (struct FsEbml_node*) pEbml, "command_type");
    if (strcmp("camctl", cmd->data.buffer) == 0) {

        struct FsEbml_node *m_node = fs_Ebml_node_get_first(pEbml, (struct FsEbml_node*) pEbml, "action_type");

        printf("0 m_action: %d \n", pCapture_item->rw.m_action);

        if (strcmp("0002H", m_node->data.buffer) == 0) pCapture_item->rw.m_action = 1; //右
        if (strcmp("0004H", m_node->data.buffer) == 0) pCapture_item->rw.m_action = 2; //左
        if (strcmp("0008H", m_node->data.buffer) == 0) pCapture_item->rw.m_action = 3; //上
        if (strcmp("0010H", m_node->data.buffer) == 0) pCapture_item->rw.m_action = 4; //下
        if (strcmp("000CH", m_node->data.buffer) == 0) pCapture_item->rw.m_action = 5; //左上
        if (strcmp("0014H", m_node->data.buffer) == 0) pCapture_item->rw.m_action = 6; //左下
        if (strcmp("000AH", m_node->data.buffer) == 0) pCapture_item->rw.m_action = 7; //右上
        if (strcmp("0012H", m_node->data.buffer) == 0) pCapture_item->rw.m_action = 8; //右下
        if (strcmp("0020H", m_node->data.buffer) == 0) pCapture_item->rw.m_action = 9; //变倍大
        if (strcmp("0040H", m_node->data.buffer) == 0) pCapture_item->rw.m_action = 10; //变倍小
        if (strcmp("0080H", m_node->data.buffer) == 0) pCapture_item->rw.m_action = 11; //聚焦远
        if (strcmp("0100H", m_node->data.buffer) == 0) pCapture_item->rw.m_action = 12; //聚焦近
        if (strcmp("0200H", m_node->data.buffer) == 0) pCapture_item->rw.m_action = 13; //光圈大
        if (strcmp("0400H", m_node->data.buffer) == 0) pCapture_item->rw.m_action = 14; //光圈小
        if (strcmp("0000H", m_node->data.buffer) == 0) pCapture_item->rw.m_action = 15; //停止码
        if (strcmp("8100H", m_node->data.buffer) == 0) pCapture_item->rw.m_action = 16; //自动除雾开
        if (strcmp("8200H", m_node->data.buffer) == 0) pCapture_item->rw.m_action = 17; //自动除雾关
        if (strcmp("8300H", m_node->data.buffer) == 0) pCapture_item->rw.m_action = 18; //激光器开
        if (strcmp("8400H", m_node->data.buffer) == 0) pCapture_item->rw.m_action = 19; //激光器关
        if (strcmp("0500H", m_node->data.buffer) == 0) pCapture_item->rw.m_action = 20; //转向ptz
        if (strcmp("0600H", m_node->data.buffer) == 0) pCapture_item->rw.m_action = 21; //获取ptz
        if (strcmp("0900H", m_node->data.buffer) == 0) pCapture_item->rw.m_action = 22; //红光开
        if (strcmp("0800H", m_node->data.buffer) == 0) pCapture_item->rw.m_action = 23; //红光关

        printf("m_action: %d\n", pCapture_item->rw.m_action);

        m_node = fs_Ebml_node_get_first(pEbml, (struct FsEbml_node*) pEbml, "action_speed");
        pCapture_item->rw.camera_speed = atoi(m_node->data.buffer);
        if (pCapture_item->rw.camera_speed > 7) pCapture_item->rw.camera_speed = 7;
        m_node = fs_Ebml_node_get_first(pEbml, (struct FsEbml_node*) pEbml, "action_time");
        pCapture_item->rw.camera_time = atoi(m_node->data.buffer);

        m_node = fs_Ebml_node_get_first(pEbml, (struct FsEbml_node*) pEbml, "m_auto_focus");
        pCapture_item->rw.m_auto_focus = atoi(m_node->data.buffer);
        m_node = fs_Ebml_node_get_first(pEbml, (struct FsEbml_node*) pEbml, "m_target_id");
        pCapture_item->rw.m_target_id = atoi(m_node->data.buffer);

        m_node = fs_Ebml_node_get_first(pEbml, (struct FsEbml_node*) pEbml, "return_type");
        if (strcmp("json", m_node->data.buffer) == 0) pCapture_item->rw.m_reply_type = 1;
        if (strcmp("xml", m_node->data.buffer) == 0) pCapture_item->rw.m_reply_type = 2;


        m_node = fs_Ebml_node_get_first(pEbml, (struct FsEbml_node*) pEbml, "P");
        pCapture_item->rw.goto_p = atof(m_node->data.buffer);
        m_node = fs_Ebml_node_get_first(pEbml, (struct FsEbml_node*) pEbml, "T");
        pCapture_item->rw.goto_t = atof(m_node->data.buffer);
        m_node = fs_Ebml_node_get_first(pEbml, (struct FsEbml_node*) pEbml, "Z");
        pCapture_item->rw.goto_z = atof(m_node->data.buffer);

        printf("1 m_action: %d  _uuid:%s \n", pCapture_item->rw.m_action, pCapture_item->ro._uuid);

        //回执信息，默认都是成功
        if (1) {

            char temp[54];
            FsEbml *rst = fs_Ebml_new__IO(FsEbmlEncodeMethod_UTF8, 0);
            struct FsEbml_node* pEbml_node = fs_Ebml_node_addChild(rst, (struct FsEbml_node*) rst, "code", FsEbmlNodeType_String);
            fs_Ebml_node_data_set_string(pEbml_node, "1000");
            pEbml_node = fs_Ebml_node_addChild(rst, (struct FsEbml_node*) rst, "msg", FsEbmlNodeType_String);
            //sprintf(temp, "%s", "");
            //fs_Ebml_node_data_set_string(pEbml_node, temp);

            pEbml_node = fs_Ebml_node_addChild(rst, (struct FsEbml_node*) rst, "P", FsEbmlNodeType_String);
            sprintf(temp, "%f", pCapture_item->rw.camera_c_p);
            fs_Ebml_node_data_set_string(pEbml_node, temp);
            pEbml_node = fs_Ebml_node_addChild(rst, (struct FsEbml_node*) rst, "T", FsEbmlNodeType_String);
            sprintf(temp, "%f", pCapture_item->rw.camera_c_t);
            fs_Ebml_node_data_set_string(pEbml_node, temp);
            pEbml_node = fs_Ebml_node_addChild(rst, (struct FsEbml_node*) rst, "Z", FsEbmlNodeType_String);
            sprintf(temp, "%f", pCapture_item->rw.camera_c_z);
            fs_Ebml_node_data_set_string(pEbml_node, temp);

            pEbml_node = fs_Ebml_node_addChild(rst, (struct FsEbml_node*) rst, "command_for_uuid", FsEbmlNodeType_String);
            sprintf(temp, "%s", pCapture_item->ro._uuid);
            fs_Ebml_node_data_set_string(pEbml_node, temp);
            pEbml_node = fs_Ebml_node_addChild(rst, (struct FsEbml_node*) rst, "action_type", FsEbmlNodeType_String);
            m_node = fs_Ebml_node_get_first(pEbml, (struct FsEbml_node*) pEbml, "action_type");
            sprintf(temp, "%s", m_node->data.buffer);
            fs_Ebml_node_data_set_string(pEbml_node, temp);
            pEbml_node = fs_Ebml_node_addChild(rst, (struct FsEbml_node*) rst, "image_byte", FsEbmlNodeType_String);
            //sprintf(temp, "%s", "");
            //fs_Ebml_node_data_set_string(pEbml_node, temp);           

            FsJson *pJson = fs_Ebml_convert_to_Json__IO(rst);
            fs_Ebml_delete__OI(rst, pShareBuffer);

            FsObjectBase * const pObjectBase = fs_Json_to_ObjectBase__IO(pJson, sizeof (FsObjectBase) + 8, Memery_Alignment(sizeof (unsigned int)*3), FsStringLenData("\r\n"), 0, NULL, 4);
            fs_Json_delete__OI(pJson, pShareBuffer);
            pObjectBase->data -= 8;
            pObjectBase->len = pObjectBase->len - Memery_Alignment(sizeof (unsigned int)*3) + 8;
            ((unsigned int *) pObjectBase->data)[0] = (head & 0xFFFFFFF0U) | 0xAU, ((unsigned int *) pObjectBase->data)[1] = pObjectBase->len - 8;
            memcpy(pObjectBase->data + Memery_Alignment(pObjectBase->len), requestID_3, sizeof (unsigned int)*3);
            configManager_send_pthreadSafety__OI_2(pCapture_item->ro._pCapture->ro._pConfigManager, pObjectBase);
        }

        if (write(pCapture_item->ro._pCapture->ro.__pipe[1], "1", 1) != 1) {

            FsPrintf(5, "write pipe(=%d) error:\"%s\".\n", pCapture_item->ro._pCapture->ro.__pipe[1], strerror(errno));
            fflush(stdout);
        }
    }

    return 1;
}

/* 断开控制客户端的连接,除了当前连接requestID_3 */
static inline void capture_private_item_ctrlClient_release(struct Capture_item * const pCapture_item, const unsigned int requestID_3[]) {
    unsigned int *requestData, (*ppNode)[6] = ((unsigned int (*)[6])pCapture_item->p.__ctrlClientList->pNode) + pCapture_item->p.__ctrlClientList->startIndex;
    unsigned int i = pCapture_item->p.__ctrlClientList->nodeCount;
    unsigned int ipv4;
    unsigned short ipv4Port;
    FsObjectBase *sendData;
    for (; i > 0; i--) {
        requestData = *ppNode++;
        if (requestData[0] == requestID_3[0] && requestData[1] == requestID_3[1] && requestData[2] == requestID_3[2]) {
            /* 包含当前连接 */
            pCapture_item->p.__ctrlClientList->startIndex += pCapture_item->p.__ctrlClientList->nodeCount - i;
            pCapture_item->p.__ctrlClientList->nodeCount = 1;
            for (i--; i > 0; i--) {
                requestData = *ppNode++;
                /* 发送消息通知客户端控制被抢占了 */
                sendData = (FsObjectBase*) fsMalloc(sizeof (FsObjectBase) + 8 + Memery_Alignment(sizeof ("Control has been preempted by client:255.255.255.255:65535.\n") - 1) + sizeof (unsigned int)*3);
                fs_ObjectBase_init(sendData, sizeof (FsObjectBase) + 8 + Memery_Alignment(sizeof ("Control has been preempted by client:255.255.255.255:65535.\n") - 1) + sizeof (unsigned int)*3,
                        sizeof (FsObjectBase));
                configManager_connect_get_addr(&ipv4, &ipv4Port, requestData);
                sendData->len = 8 + sprintf(sendData->data + 8, "Control has been preempted by client:%hhu.%hhu.%hhu.%hhu:%hu",
                        Fs_Network_Ipv4_print(ipv4), ipv4Port);
                *((unsigned int *) sendData->data) = (requestData[3]& 0xFFFFFFF0U) | 0x6U, *((unsigned int *) (sendData->data + 4)) = sendData->len - 8;
                memcpy(sendData->data + Memery_Alignment(sendData->len), requestData, sizeof (unsigned int)*3);
                configManager_send_pthreadSafety__OI_2(pCapture_item->ro._pCapture->ro._pConfigManager, sendData);
                configManager_connect_release(pCapture_item->ro._pCapture->ro._pConfigManager, requestData, 0);
            }
            return;
        } else {

            /* 发送消息通知客户端控制被抢占了 */
            sendData = (FsObjectBase*) fsMalloc(sizeof (FsObjectBase) + 8 + Memery_Alignment(sizeof ("Control has been preempted by client:255.255.255.255:65535.\n") - 1) + sizeof (unsigned int)*3);
            fs_ObjectBase_init(sendData, sizeof (FsObjectBase) + 8 + Memery_Alignment(sizeof ("Control has been preempted by client:255.255.255.255:65535.\n") - 1) + sizeof (unsigned int)*3,
                    sizeof (FsObjectBase));
            configManager_connect_get_addr(&ipv4, &ipv4Port, requestData);
            sendData->len = 8 + sprintf(sendData->data + 8, "Control has been preempted by client:%hhu.%hhu.%hhu.%hhu:%hu",
                    Fs_Network_Ipv4_print(ipv4), ipv4Port);
            *((unsigned int *) sendData->data) = (requestData[3]& 0xFFFFFFF0U) | 0x6U, *((unsigned int *) (sendData->data + 4)) = sendData->len - 8;
            memcpy(sendData->data + Memery_Alignment(sendData->len), requestData, sizeof (unsigned int)*3);
            configManager_send_pthreadSafety__OI_2(pCapture_item->ro._pCapture->ro._pConfigManager, sendData);
            configManager_connect_release(pCapture_item->ro._pCapture->ro._pConfigManager, requestData, 0);
        }
    }
    fs_StructList_clear(pCapture_item->p.__ctrlClientList);
}

/* 在有连接断开时调用函数,成功返回1,失败返回-1,需要引用此连接返回-128,为空表示此命令字不允许远程调用 */
static int capture_P_cb_connect_error(/* 与请求相关的信息,用于识别是发给哪个客户端的数据,用3个int来储存 */ const unsigned int requestID_3[]
        , /* 调用函数的指针 */ struct Capture * const pCapture
        , /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer) {
    pCapture->p.netClientHasError = 1;
    pthread_mutex_lock(&pCapture->p.streamCtrlStatusClientList->mutex);
    configManager_conncet_refer_check(pCapture->ro._pConfigManager, pCapture->p.streamCtrlStatusClientList);
    pthread_mutex_unlock(&pCapture->p.streamCtrlStatusClientList->mutex);
    return 1;
}

/* 在有用户请求此命令字时的调用函数,成功返回1,失败返回-1,需要引用此连接返回-128 */
static int capture_P_cmd_cb_streamCtrlStatus_get(/* 与请求相关的信息,用于识别是发给哪个客户端的数据,用3个int来储存 */const unsigned int requestID_3[], /* 收到数据的前4字节 */unsigned int head
        , /* 收到的数据 */FsEbml *pEbml, /* 客户端发送请求的数据类型,1-ebml数据,2-xml数据,3-json数据 */ char requestDataType, /* 调用函数的指针 */ struct Capture * const pCapture
        , /* 缓存Buffer,不为空 */FsObjectBaseBuffer * const pObjectBaseBuffer, /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer) {
    {
#ifndef __cmd_cb_cal_return_type 
        const char *const return_type = fs_Ebml_node_get_first_string(pEbml, (struct FsEbml_node*) pEbml, "return_type", NULL);
        if (return_type) {
            if (0 == strcmp("ebml", return_type)) requestDataType = 1;
            else if (0 == strcmp("xml", return_type)) requestDataType = 2;
            else if (0 == strcmp("json", return_type)) requestDataType = 3;
        }
#endif
    }
    const unsigned char history_get = fs_Ebml_node_get_first_Integer(pEbml, (struct FsEbml_node*) pEbml, "history_get", 0);
    pthread_mutex_lock(&pCapture->p.streamCtrlStatusClientList->mutex);
    if (history_get) {
        struct Capture_item **ppNode = (struct Capture_item **) pCapture->p.streamCtrlStartItemListOrder__streamCtrlStatusClientList->pNode + pCapture->p.streamCtrlStartItemListOrder__streamCtrlStatusClientList->startIndex;
        unsigned int ui = pCapture->p.streamCtrlStartItemListOrder__streamCtrlStatusClientList->nodeCount;
        while (ui-- > 0) {
            struct Capture_item * const pCapture_item = *ppNode++;
            FsEbml * const pEbml = capture_P_item_get_streamCtrlStatus__IO(pCapture_item, 1);
            configManager_conncet_refer_sendData(NULL, NULL, NULL, pEbml, requestID_3, head & 0x2, requestDataType, pObjectBaseBuffer);
            fs_Ebml_delete__OI(pEbml, pShareBuffer);
        }
    }
    unsigned int data[6] = {requestID_3[0], requestID_3[1], requestID_3[2], head, (unsigned int) requestDataType, 0};
    if (fs_StructList_insert_order(pCapture->p.streamCtrlStatusClientList, data) < 0) {
        pthread_mutex_unlock(&pCapture->p.streamCtrlStatusClientList->mutex);
        return 1;
    } else {
        pthread_mutex_unlock(&pCapture->p.streamCtrlStatusClientList->mutex);
        return -128;
    }
}

static FsConfig *capture_P_protocol_streamCtrlStatus_get() {
    FsConfig * const pConfig = fs_Config_new__IO();
    {
        void *const pNode = fs_Config_node_string_add(pConfig, pConfig, "command_type", "命令类型", "命令类型,此字段为开发人员分配,各接口唯一", 0, 0x7, 0, 20, 1);
        fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "streamCtrlStatus_get", "streamCtrlStatus_get", "streamCtrlStatus_get");
    }
    {
        void *const pNode = fs_Config_node_string_add(pConfig, pConfig, "command_for_uuid", "资源的uuid", "资源的uuid,表示使用此接口操作的资源编号", 0, 0x7, 0, 32, 1);
        fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "capture", "capture", "capture");
    }
    {
        void *const pNode = fs_Config_node_integer_add(pConfig, pConfig, "history_get", "获取历史信息", "是否获取历史信息", FsConfig_nodeShowType_default, 0, 0x7, 0, 1, 1);
        fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_default, 1, "1-获取", "1-获取");
        fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_optional, 0, "0-不获取", "0-不获取");
    }
    {

        void *const pNode = fs_Config_node_string_add(pConfig, pConfig, "return_type", "回执类型", "回执类型,缺失或无效时回执的数据类型不确定,支持ebml(压缩数据),xml,json", 0, 0x7, 3, 4, 1);
        fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "ebml", "ebml", "ebml");
        fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_optional, "xml", "xml", "xml");
        fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_optional, "json", "json", "json");
    }

    return pConfig;
}

static FsConfig *capture_P_protocol_ctrlRelease() {
    FsConfig * const pConfig = fs_Config_new__IO();
    {
        void *const pNode = fs_Config_node_string_add(pConfig, pConfig, "command_type", "命令类型", "命令类型,此字段为开发人员分配,各接口唯一", 0, 0x7, 0, 19, 1);
        fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "cameractrl", "cameractrl", "cameractrl");
    }
    {
        void *const pNode = fs_Config_node_string_add(pConfig, pConfig, "command_for_uuid", "资源的uuid", "资源的uuid,表示使用此接口操作的资源编号", 0, 0x7, 0, 32, 1);
        fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "1", "1", "1");
    }
    {
        void *const pNode = fs_Config_node_string_add(pConfig, pConfig, "ctrl_type", "控制类型", "控制类型", 0, 0x7, 0, 16, 1);
        fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "release", "释放相机控制权限", "释放相机控制权限");
    }
    return pConfig;
}

static FsConfig *capture_P_protocol_ctrlmaskget() {
    FsConfig * const pConfig = fs_Config_new__IO();
    {
        void *const pNode = fs_Config_node_string_add(pConfig, pConfig, "command_type", "命令类型", "命令类型,此字段为开发人员分配,各接口唯一", 0, 0x7, 0, 19, 1);
        fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "cameractrl", "cameractrl", "cameractrl");
    }
    {
        void *const pNode = fs_Config_node_string_add(pConfig, pConfig, "command_for_uuid", "资源的uuid", "资源的uuid,表示使用此接口操作的资源编号", 0, 0x7, 0, 32, 1);
        fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "1", "1", "1");
    }
    {
        void *const pNode = fs_Config_node_string_add(pConfig, pConfig, "ctrl_type", "控制类型", "控制类型", 0, 0x7, 0, 16, 1);
        fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "maskget", "控制掩码获取", "控制掩码获取");
    }
    {

        void *const pNode = fs_Config_node_string_add(pConfig, pConfig, "return_type", "回执类型", "回执类型,缺失或无效时回执的数据类型不确定,支持ebml(压缩数据),xml,json", 0, 0x7, 3, 4, 1);
        fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "ebml", "ebml", "ebml");
        fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_optional, "xml", "xml", "xml");
        fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_optional, "json", "json", "json");
    }

    return pConfig;
}

static FsConfig *capture_P_protocol_turn() {
    FsConfig * const pConfig = fs_Config_new__IO();
    {
        void *const pNode = fs_Config_node_string_add(pConfig, pConfig, "command_type", "命令类型", "命令类型,此字段为开发人员分配,各接口唯一", 0, 0x7, 0, 19, 1);
        fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "cameractrl", "cameractrl", "cameractrl");
    }
    {
        void *const pNode = fs_Config_node_string_add(pConfig, pConfig, "command_for_uuid", "资源的uuid", "资源的uuid,表示使用此接口操作的资源编号", 0, 0x7, 0, 16, 1);
        fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "1", "1", "1");
    }
    {
        void *const pNode = fs_Config_node_string_add(pConfig, pConfig, "ctrl_type", "控制类型", "控制类型", 0, 0x7, 0, 16, 1);
        fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "turn", "转动", "转动");
    }
    {
        void *const pNode = fs_Config_node_integer_add(pConfig, pConfig, "priority", "控制优先级", "优先级,0-默认,不缓存调用者信息,1-比分析优先级低的定义,2-255:保留,256-29999:比分析优先级低的定义,\n"
                "30000-34999:视频分析专用的优先级,35000-64999:比分析优先级高的定义,65000-65534:保留;非0优先级中,\n"
                "    偶数表示可合并的优先级,即如有多个拥有相同偶数优先级的客户端访问同一相机,这些所有客户端的指令会按照新指令覆盖旧指令方式生效;\n"
                "    奇数表示独占优先级,即如有多个拥有相同奇数优先级的客户端访问同一相机,只有第一个客户端的指令会生效,其他客户端的指令会被拒绝", FsConfig_nodeShowType_default, 0, 0x7, 0, 65534, 1);
        fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_default, 0, "0", "0");
        fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_optional, 256, "256", "256");
        fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_optional, 257, "257", "257");
    }
    {
        void *const pNode = fs_Config_node_integer_add(pConfig, pConfig, "speedx", "左右转动的速度", "参考值精度为23040,负数向左,正数向右", FsConfig_nodeShowType_default, 0, 0x7, -23040, 23040, 1);
        fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_default, 23040, "23040", "23040");
        fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_optional, -23040, "-23040", "-23040");
        fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_optional, 0, "0", "0");
    }
    {
        void *const pNode = fs_Config_node_integer_add(pConfig, pConfig, "speedy", "上下转动的速度", "参考值精度为23040,负数向上,正数向下", FsConfig_nodeShowType_default, 0, 0x7, -23040, 23040, 1);
        fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_default, 0, "0", "0");
        fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_optional, 23040, "23040", "23040");
        fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_optional, -23040, "-23040", "-23040");
    }
    {

        void *const pNode = fs_Config_node_float_add(pConfig, pConfig, "time", "转动的时间(S)", "转动的时间,单位秒,为0表示停止", 0, 0x7, 0.0, 50.0, 1);
        fs_Config_node_float_add_value(pConfig, pNode, FsConfig_nodeValue_default, 1.0, "1.0", "1.0");
        fs_Config_node_float_add_value(pConfig, pNode, FsConfig_nodeValue_optional, 0.0, "0.0", "0.0");
    }

    return pConfig;
}

static FsConfig *capture_P_protocol_zoom() {
    FsConfig * const pConfig = fs_Config_new__IO();
    {
        void *const pNode = fs_Config_node_string_add(pConfig, pConfig, "command_type", "命令类型", "命令类型,此字段为开发人员分配,各接口唯一", 0, 0x7, 0, 19, 1);
        fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "cameractrl", "cameractrl", "cameractrl");
    }
    {
        void *const pNode = fs_Config_node_string_add(pConfig, pConfig, "command_for_uuid", "资源的uuid", "资源的uuid,表示使用此接口操作的资源编号", 0, 0x7, 0, 16, 1);
        fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "1", "1", "1");
    }
    {
        void *const pNode = fs_Config_node_string_add(pConfig, pConfig, "ctrl_type", "控制类型", "控制类型", 0, 0x7, 0, 16, 1);
        fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "zoom", "变倍", "变倍");
    }
    {
        void *const pNode = fs_Config_node_integer_add(pConfig, pConfig, "priority", "控制优先级", "优先级,0-默认,不缓存调用者信息,1-比分析优先级低的定义,2-255:保留,256-29999:比分析优先级低的定义,\n"
                "30000-34999:视频分析专用的优先级,35000-64999:比分析优先级高的定义,65000-65534:保留;非0优先级中,\n"
                "    偶数表示可合并的优先级,即如有多个拥有相同偶数优先级的客户端访问同一相机,这些所有客户端的指令会按照新指令覆盖旧指令方式生效;\n"
                "    奇数表示独占优先级,即如有多个拥有相同奇数优先级的客户端访问同一相机,只有第一个客户端的指令会生效,其他客户端的指令会被拒绝", FsConfig_nodeShowType_default, 0, 0x7, 0, 65534, 1);
        fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_default, 0, "0", "0");
        fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_optional, 256, "256", "256");
        fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_optional, 257, "257", "257");
    }
    {
        void *const pNode = fs_Config_node_integer_add(pConfig, pConfig, "speed", "变倍速度", "参考值精度为23040,负数缩小,正数放大,0停止", FsConfig_nodeShowType_default, 0, 0x7, -23040, 23040, 1);
        fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_default, 23040, "23040", "23040");
        fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_optional, -23040, "-23040", "-23040");
        fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_optional, 0, "0", "0");
    }
    {

        void *const pNode = fs_Config_node_float_add(pConfig, pConfig, "time", "变倍时间(S)", "变倍的时间,单位秒,为0表示停止", 0, 0x7, 0.0, 50.0, 1);
        fs_Config_node_float_add_value(pConfig, pNode, FsConfig_nodeValue_default, 1.0, "1.0", "1.0");
        fs_Config_node_float_add_value(pConfig, pNode, FsConfig_nodeValue_optional, 0.0, "0.0", "0.0");
    }

    return pConfig;
}

static FsConfig *capture_P_protocol_focus() {
    FsConfig * const pConfig = fs_Config_new__IO();
    {
        void *const pNode = fs_Config_node_string_add(pConfig, pConfig, "command_type", "命令类型", "命令类型,此字段为开发人员分配,各接口唯一", 0, 0x7, 0, 19, 1);
        fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "cameractrl", "cameractrl", "cameractrl");
    }
    {
        void *const pNode = fs_Config_node_string_add(pConfig, pConfig, "command_for_uuid", "资源的uuid", "资源的uuid,表示使用此接口操作的资源编号", 0, 0x7, 0, 16, 1);
        fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "1", "1", "1");
    }
    {
        void *const pNode = fs_Config_node_string_add(pConfig, pConfig, "ctrl_type", "控制类型", "控制类型", 0, 0x7, 0, 16, 1);
        fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "focus", "聚焦", "聚焦");
    }
    {
        void *const pNode = fs_Config_node_integer_add(pConfig, pConfig, "priority", "控制优先级", "优先级,0-默认,不缓存调用者信息,1-比分析优先级低的定义,2-255:保留,256-29999:比分析优先级低的定义,\n"
                "30000-34999:视频分析专用的优先级,35000-64999:比分析优先级高的定义,65000-65534:保留;非0优先级中,\n"
                "    偶数表示可合并的优先级,即如有多个拥有相同偶数优先级的客户端访问同一相机,这些所有客户端的指令会按照新指令覆盖旧指令方式生效;\n"
                "    奇数表示独占优先级,即如有多个拥有相同奇数优先级的客户端访问同一相机,只有第一个客户端的指令会生效,其他客户端的指令会被拒绝", FsConfig_nodeShowType_default, 0, 0x7, 0, 65534, 1);
        fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_default, 0, "0", "0");
        fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_optional, 256, "256", "256");
        fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_optional, 257, "257", "257");
    }
    {
        void *const pNode = fs_Config_node_integer_add(pConfig, pConfig, "speed", "聚焦速度", "参考值精度为23040,负数-,正数+,0停止", FsConfig_nodeShowType_default, 0, 0x7, -23040, 23040, 1);
        fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_default, 23040, "23040", "23040");
        fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_optional, -23040, "-23040", "-23040");
        fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_optional, 0, "0", "0");
    }
    {

        void *const pNode = fs_Config_node_float_add(pConfig, pConfig, "time", "聚焦时间(S)", "聚焦的时间,单位秒,为0表示停止", 0, 0x7, 0.0, 50.0, 1);
        fs_Config_node_float_add_value(pConfig, pNode, FsConfig_nodeValue_default, 1.0, "1.0", "1.0");
        fs_Config_node_float_add_value(pConfig, pNode, FsConfig_nodeValue_optional, 0.0, "0.0", "0.0");
    }

    return pConfig;
}

static FsConfig *capture_P_protocol_preset_set() {
    FsConfig * const pConfig = fs_Config_new__IO();
    {
        void *const pNode = fs_Config_node_string_add(pConfig, pConfig, "command_type", "命令类型", "命令类型,此字段为开发人员分配,各接口唯一", 0, 0x7, 0, 19, 1);
        fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "cameractrl", "cameractrl", "cameractrl");
    }
    {
        void *const pNode = fs_Config_node_string_add(pConfig, pConfig, "command_for_uuid", "资源的uuid", "资源的uuid,表示使用此接口操作的资源编号", 0, 0x7, 0, 16, 1);
        fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "1", "1", "1");
    }
    {
        void *const pNode = fs_Config_node_string_add(pConfig, pConfig, "ctrl_type", "控制类型", "控制类型", 0, 0x7, 0, 16, 1);
        fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "preset_set", "设置预置位", "设置预置位");
    }
    {
        void *const pNode = fs_Config_node_integer_add(pConfig, pConfig, "priority", "控制优先级", "优先级,0-默认,不缓存调用者信息,1-比分析优先级低的定义,2-255:保留,256-29999:比分析优先级低的定义,\n"
                "30000-34999:视频分析专用的优先级,35000-64999:比分析优先级高的定义,65000-65534:保留;非0优先级中,\n"
                "    偶数表示可合并的优先级,即如有多个拥有相同偶数优先级的客户端访问同一相机,这些所有客户端的指令会按照新指令覆盖旧指令方式生效;\n"
                "    奇数表示独占优先级,即如有多个拥有相同奇数优先级的客户端访问同一相机,只有第一个客户端的指令会生效,其他客户端的指令会被拒绝", FsConfig_nodeShowType_default, 0, 0x7, 0, 65534, 1);
        fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_default, 0, "0", "0");
        fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_optional, 256, "256", "256");
        fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_optional, 257, "257", "257");
    }
    {

        void *const pNode = fs_Config_node_integer_add(pConfig, pConfig, "presetIndex", "预置位号", "预置位号", FsConfig_nodeShowType_default, 0, 0x7, 0, 128, 1);
        fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_default, 0, "0", "0");
        fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_optional, 1, "1", "1");
        fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_optional, 2, "2", "2");
        fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_optional, 3, "3", "3");
        fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_optional, 4, "4", "4");
    }

    return pConfig;
}

static FsConfig *capture_P_protocol_preset_goto() {
    FsConfig * const pConfig = fs_Config_new__IO();
    {
        void *const pNode = fs_Config_node_string_add(pConfig, pConfig, "command_type", "命令类型", "命令类型,此字段为开发人员分配,各接口唯一", 0, 0x7, 0, 19, 1);
        fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "cameractrl", "cameractrl", "cameractrl");
    }
    {
        void *const pNode = fs_Config_node_string_add(pConfig, pConfig, "command_for_uuid", "资源的uuid", "资源的uuid,表示使用此接口操作的资源编号", 0, 0x7, 0, 16, 1);
        fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "1", "1", "1");
    }
    {
        void *const pNode = fs_Config_node_string_add(pConfig, pConfig, "ctrl_type", "控制类型", "控制类型", 0, 0x7, 0, 16, 1);
        fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "preset_goto", "调用预置位", "调用预置位");
    }
    {
        void *const pNode = fs_Config_node_integer_add(pConfig, pConfig, "priority", "控制优先级", "优先级,0-默认,不缓存调用者信息,1-比分析优先级低的定义,2-255:保留,256-29999:比分析优先级低的定义,\n"
                "30000-34999:视频分析专用的优先级,35000-64999:比分析优先级高的定义,65000-65534:保留;非0优先级中,\n"
                "    偶数表示可合并的优先级,即如有多个拥有相同偶数优先级的客户端访问同一相机,这些所有客户端的指令会按照新指令覆盖旧指令方式生效;\n"
                "    奇数表示独占优先级,即如有多个拥有相同奇数优先级的客户端访问同一相机,只有第一个客户端的指令会生效,其他客户端的指令会被拒绝", FsConfig_nodeShowType_default, 0, 0x7, 0, 65534, 1);
        fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_default, 0, "0", "0");
        fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_optional, 256, "256", "256");
        fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_optional, 257, "257", "257");
    }
    {

        void *const pNode = fs_Config_node_integer_add(pConfig, pConfig, "presetIndex", "预置位号", "预置位号", FsConfig_nodeShowType_default, 0, 0x7, 0, 128, 1);
        fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_default, 0, "0", "0");
        fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_optional, 1, "1", "1");
        fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_optional, 2, "2", "2");
        fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_optional, 3, "3", "3");
        fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_optional, 4, "4", "4");
    }

    return pConfig;
}

static FsConfig *capture_P_protocol_preset_clean() {
    FsConfig * const pConfig = fs_Config_new__IO();
    {
        void *const pNode = fs_Config_node_string_add(pConfig, pConfig, "command_type", "命令类型", "命令类型,此字段为开发人员分配,各接口唯一", 0, 0x7, 0, 19, 1);
        fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "cameractrl", "cameractrl", "cameractrl");
    }
    {
        void *const pNode = fs_Config_node_string_add(pConfig, pConfig, "command_for_uuid", "资源的uuid", "资源的uuid,表示使用此接口操作的资源编号", 0, 0x7, 0, 16, 1);
        fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "1", "1", "1");
    }
    {
        void *const pNode = fs_Config_node_string_add(pConfig, pConfig, "ctrl_type", "控制类型", "控制类型", 0, 0x7, 0, 16, 1);
        fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "preset_clean", "清除预置位", "清除预置位");
    }
    {
        void *const pNode = fs_Config_node_integer_add(pConfig, pConfig, "priority", "控制优先级", "优先级,0-默认,不缓存调用者信息,1-比分析优先级低的定义,2-255:保留,256-29999:比分析优先级低的定义,\n"
                "30000-34999:视频分析专用的优先级,35000-64999:比分析优先级高的定义,65000-65534:保留;非0优先级中,\n"
                "    偶数表示可合并的优先级,即如有多个拥有相同偶数优先级的客户端访问同一相机,这些所有客户端的指令会按照新指令覆盖旧指令方式生效;\n"
                "    奇数表示独占优先级,即如有多个拥有相同奇数优先级的客户端访问同一相机,只有第一个客户端的指令会生效,其他客户端的指令会被拒绝", FsConfig_nodeShowType_default, 0, 0x7, 0, 65534, 1);
        fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_default, 0, "0", "0");
        fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_optional, 256, "256", "256");
        fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_optional, 257, "257", "257");
    }
    {

        void *const pNode = fs_Config_node_integer_add(pConfig, pConfig, "presetIndex", "预置位号", "预置位号", FsConfig_nodeShowType_default, 0, 0x7, 0, 128, 1);
        fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_default, 0, "0", "0");
        fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_optional, 1, "1", "1");
        fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_optional, 2, "2", "2");
        fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_optional, 3, "3", "3");
        fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_optional, 4, "4", "4");
    }

    return pConfig;
}

static FsConfig *capture_P_protocol_zoom3D() {
    FsConfig * const pConfig = fs_Config_new__IO();
    {
        void *const pNode = fs_Config_node_string_add(pConfig, pConfig, "command_type", "命令类型", "命令类型,此字段为开发人员分配,各接口唯一", 0, 0x7, 0, 19, 1);
        fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "cameractrl", "cameractrl", "cameractrl");
    }
    {
        void *const pNode = fs_Config_node_string_add(pConfig, pConfig, "command_for_uuid", "资源的uuid", "资源的uuid,表示使用此接口操作的资源编号", 0, 0x7, 0, 16, 1);
        fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "1", "1", "1");
    }
    {
        void *const pNode = fs_Config_node_string_add(pConfig, pConfig, "ctrl_type", "控制类型", "控制类型", 0, 0x7, 0, 16, 1);
        fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "zoom3D", "3D定位", "3D定位");
    }
    {
        void *const pNode = fs_Config_node_integer_add(pConfig, pConfig, "priority", "控制优先级", "优先级,0-默认,不缓存调用者信息,1-比分析优先级低的定义,2-255:保留,256-29999:比分析优先级低的定义,\n"
                "30000-34999:视频分析专用的优先级,35000-64999:比分析优先级高的定义,65000-65534:保留;非0优先级中,\n"
                "    偶数表示可合并的优先级,即如有多个拥有相同偶数优先级的客户端访问同一相机,这些所有客户端的指令会按照新指令覆盖旧指令方式生效;\n"
                "    奇数表示独占优先级,即如有多个拥有相同奇数优先级的客户端访问同一相机,只有第一个客户端的指令会生效,其他客户端的指令会被拒绝", FsConfig_nodeShowType_default, 0, 0x7, 0, 65534, 1);
        fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_default, 0, "0", "0");
        fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_optional, 256, "256", "256");
        fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_optional, 257, "257", "257");
    }
    {
        void *const pNode = fs_Config_node_integer_add(pConfig, pConfig, "x1", "第一点x坐标", "第一点x坐标,相对图大小参考值23040", FsConfig_nodeShowType_default, 0, 0x7, 0, 23040, 1);
        fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_default, 1000, "1000", "1000");
    }
    {
        void *const pNode = fs_Config_node_integer_add(pConfig, pConfig, "y1", "第一点y坐标", "第一点y坐标,相对图大小参考值23040", FsConfig_nodeShowType_default, 0, 0x7, 0, 23040, 1);
        fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_default, 1000, "1000", "1000");
    }
    {
        void *const pNode = fs_Config_node_integer_add(pConfig, pConfig, "x2", "第二点x坐标", "第二点x坐标,相对图大小参考值23040", FsConfig_nodeShowType_default, 0, 0x7, 0, 23040, 1);
        fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_default, 13040, "13040", "13040");
    }
    {

        void *const pNode = fs_Config_node_integer_add(pConfig, pConfig, "y2", "第二点y坐标", "第二点y坐标,相对图大小参考值23040", FsConfig_nodeShowType_default, 0, 0x7, 0, 23040, 1);
        fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_default, 13040, "13040", "13040");
    }

    return pConfig;
}

static FsConfig *capture_P_protocol_ptzgoto() {
    FsConfig * const pConfig = fs_Config_new__IO();
    {
        void *const pNode = fs_Config_node_string_add(pConfig, pConfig, "command_type", "命令类型", "命令类型,此字段为开发人员分配,各接口唯一", 0, 0x7, 0, 19, 1);
        fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "cameractrl", "cameractrl", "cameractrl");
    }
    {
        void *const pNode = fs_Config_node_string_add(pConfig, pConfig, "command_for_uuid", "资源的uuid", "资源的uuid,表示使用此接口操作的资源编号", 0, 0x7, 0, 16, 1);
        fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "1", "1", "1");
    }
    {
        void *const pNode = fs_Config_node_string_add(pConfig, pConfig, "ctrl_type", "控制类型", "控制类型", 0, 0x7, 0, 16, 1);
        fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "ptzgoto", "设置PTZ", "设置PTZ");
    }
    {
        void *const pNode = fs_Config_node_integer_add(pConfig, pConfig, "priority", "控制优先级", "优先级,0-默认,不缓存调用者信息,1-比分析优先级低的定义,2-255:保留,256-29999:比分析优先级低的定义,\n"
                "30000-34999:视频分析专用的优先级,35000-64999:比分析优先级高的定义,65000-65534:保留;非0优先级中,\n"
                "    偶数表示可合并的优先级,即如有多个拥有相同偶数优先级的客户端访问同一相机,这些所有客户端的指令会按照新指令覆盖旧指令方式生效;\n"
                "    奇数表示独占优先级,即如有多个拥有相同奇数优先级的客户端访问同一相机,只有第一个客户端的指令会生效,其他客户端的指令会被拒绝", FsConfig_nodeShowType_default, 0, 0x7, 0, 65534, 1);
        fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_default, 0, "0", "0");
        fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_optional, 256, "256", "256");
        fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_optional, 257, "257", "257");
    }
    {
        void *const pNode = fs_Config_node_integer_add(pConfig, pConfig, "p", "P值", "P值,相对参考值23040", FsConfig_nodeShowType_default, 0, 0x7, 0, 23040, 1);
        fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_default, 1000, "1000", "1000");
    }
    {
        void *const pNode = fs_Config_node_integer_add(pConfig, pConfig, "t", "T值", "T值,相对参考值23040", FsConfig_nodeShowType_default, 0, 0x7, 0, 23040, 1);
        fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_default, 1000, "1000", "1000");
    }
    {

        void *const pNode = fs_Config_node_integer_add(pConfig, pConfig, "z", "Z值", "Z值,相对参考值23040", FsConfig_nodeShowType_default, 0, 0x7, 0, 23040, 1);
        fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_default, 13040, "13040", "13040");
    }

    return pConfig;
}

static FsConfig *capture_P_protocol_ptzget() {
    FsConfig * const pConfig = fs_Config_new__IO();
    {
        void *const pNode = fs_Config_node_string_add(pConfig, pConfig, "command_type", "命令类型", "命令类型,此字段为开发人员分配,各接口唯一", 0, 0x7, 0, 19, 1);
        fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "cameractrl", "cameractrl", "cameractrl");
    }
    {
        void *const pNode = fs_Config_node_string_add(pConfig, pConfig, "command_for_uuid", "资源的uuid", "资源的uuid,表示使用此接口操作的资源编号", 0, 0x7, 0, 16, 1);
        fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "1", "1", "1");
    }
    {
        void *const pNode = fs_Config_node_string_add(pConfig, pConfig, "ctrl_type", "控制类型", "控制类型", 0, 0x7, 0, 16, 1);
        fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "ptzget", "获取PTZ", "获取PTZ,获取到的值的参考精度为23040");
    }
    {
        void *const pNode = fs_Config_node_integer_add(pConfig, pConfig, "priority", "控制优先级", "优先级,0-默认,不缓存调用者信息,1-比分析优先级低的定义,2-255:保留,256-29999:比分析优先级低的定义,\n"
                "30000-34999:视频分析专用的优先级,35000-64999:比分析优先级高的定义,65000-65534:保留;非0优先级中,\n"
                "    偶数表示可合并的优先级,即如有多个拥有相同偶数优先级的客户端访问同一相机,这些所有客户端的指令会按照新指令覆盖旧指令方式生效;\n"
                "    奇数表示独占优先级,即如有多个拥有相同奇数优先级的客户端访问同一相机,只有第一个客户端的指令会生效,其他客户端的指令会被拒绝", FsConfig_nodeShowType_default, 0, 0x7, 0, 65534, 1);
        fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_default, 0, "0", "0");
        fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_optional, 256, "256", "256");
        fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_optional, 257, "257", "257");
    }
    {

        void *const pNode = fs_Config_node_string_add(pConfig, pConfig, "return_type", "回执类型", "回执类型,缺失或无效时回执的数据类型不确定,支持ebml(压缩数据),xml,json", 0, 0x7, 3, 4, 1);
        fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "ebml", "ebml", "ebml");
        fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_optional, "xml", "xml", "xml");
        fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_optional, "json", "json", "json");
    }

    return pConfig;
}

static FsConfig *capture_P_protocol_imageinfoset() {
    FsConfig * const pConfig = fs_Config_new__IO();
    {
        void *const pNode = fs_Config_node_string_add(pConfig, pConfig, "command_type", "命令类型", "命令类型,此字段为开发人员分配,各接口唯一", 0, 0x7, 0, 19, 1);
        fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "cameractrl", "cameractrl", "cameractrl");
    }
    {
        void *const pNode = fs_Config_node_string_add(pConfig, pConfig, "command_for_uuid", "资源的uuid", "资源的uuid,表示使用此接口操作的资源编号", 0, 0x7, 0, 16, 1);
        fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "1", "1", "1");
    }
    {
        void *const pNode = fs_Config_node_string_add(pConfig, pConfig, "ctrl_type", "控制类型", "控制类型", 0, 0x7, 0, 16, 1);
        fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "imageinfoset", "图像信息设置", "图像信息设置");
    }
    {
        void *const pNode = fs_Config_node_integer_add(pConfig, pConfig, "priority", "控制优先级", "优先级,0-默认,不缓存调用者信息,1-比分析优先级低的定义,2-255:保留,256-29999:比分析优先级低的定义,\n"
                "30000-34999:视频分析专用的优先级,35000-64999:比分析优先级高的定义,65000-65534:保留;非0优先级中,\n"
                "    偶数表示可合并的优先级,即如有多个拥有相同偶数优先级的客户端访问同一相机,这些所有客户端的指令会按照新指令覆盖旧指令方式生效;\n"
                "    奇数表示独占优先级,即如有多个拥有相同奇数优先级的客户端访问同一相机,只有第一个客户端的指令会生效,其他客户端的指令会被拒绝", FsConfig_nodeShowType_default, 0, 0x7, 0, 65534, 1);
        fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_default, 0, "0", "0");
        fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_optional, 256, "256", "256");
        fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_optional, 257, "257", "257");
    }
    {
        void *const pNode = fs_Config_node_integer_add(pConfig, pConfig, "autoFocus", "是否开启自动聚焦", "是否开启自动聚焦", FsConfig_nodeShowType_default, 0, 0x7, 0, 1, 1);
        fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_default, 0, "0-自动聚焦", "0-自动聚焦");
        fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_optional, 1, "1-手动聚焦", "1-手动聚焦");
    }
    {
        void *const pNode = fs_Config_node_integer_add(pConfig, pConfig, "controlMode", "相机快门,增益,光圈的控制模式", "相机快门,增益,光圈的控制模式", FsConfig_nodeShowType_default, 0, 0x7, 0, 1, 1);
        fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_default, 0, "0-自动控制", "0-自动控制");
        fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_optional, 1, "1-手动控制", "1-手动控制");
    }
    {
        void *const pNode = fs_Config_node_integer_add(pConfig, pConfig, "shutter", "快门,曝光时间(微秒)", "快门,曝光时间(微秒),不设置表示沿用原来的值,controlMode为1时有效", FsConfig_nodeShowType_default, 0, 0x7, 0, 100000, 1);
        fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_default, 1000, "1000", "1000");
        fs_Config_condition_add_static(pConfig, fs_Config_condition_group_add(pConfig, pNode), 1, "controlMode", FsConfig_Condition_equal, "1");
    }
    {
        void *const pNode = fs_Config_node_integer_add(pConfig, pConfig, "gain", "增益", "增益,归一化到0-100,不设置表示沿用原来的值,controlMode为1时有效", FsConfig_nodeShowType_default, 0, 0x7, 0, 99, 1);
        fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_default, 0, "0", "0");
        fs_Config_condition_add_static(pConfig, fs_Config_condition_group_add(pConfig, pNode), 1, "controlMode", FsConfig_Condition_equal, "1");
    }
    {
        void *const pNode = fs_Config_node_integer_add(pConfig, pConfig, "iris", "光圈", "光圈,归一化到0-100,不设置表示沿用原来的值,controlMode为1时有效", FsConfig_nodeShowType_default, 0, 0x7, 0, 99, 1);
        fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_default, 0, "0", "0");
        fs_Config_condition_add_static(pConfig, fs_Config_condition_group_add(pConfig, pNode), 1, "controlMode", FsConfig_Condition_equal, "1");
    }
    {
        void *const pNode = fs_Config_node_integer_add(pConfig, pConfig, "whiteBalanceMode", "白平衡的控制模式", "白平衡的控制模式,0-自动控制,1-手动控制", FsConfig_nodeShowType_default, 0, 0x7, 0, 1, 1);
        fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_default, 0, "0-自动控制", "0-自动控制");
        fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_optional, 1, "1-手动控制", "1-手动控制");
    }
    {
        void *const pNode = fs_Config_node_integer_add(pConfig, pConfig, "whiteBalance_B", "白平衡蓝色", "白平衡蓝色,0-255,不设置表示沿用原来的值,whiteBalanceMode为1时有效", FsConfig_nodeShowType_default, 0, 0x7, 0, 99, 1);
        fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_default, 40, "40", "40");
        fs_Config_condition_add_static(pConfig, fs_Config_condition_group_add(pConfig, pNode), 1, "whiteBalanceMode", FsConfig_Condition_equal, "1");
    }
    {
        void *const pNode = fs_Config_node_integer_add(pConfig, pConfig, "whiteBalance_R", "白平衡红色", "白平衡红色,0-255,不设置表示沿用原来的值,whiteBalanceMode为1时有效", FsConfig_nodeShowType_default, 0, 0x7, 0, 99, 1);
        fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_default, 99, "99", "99");
        fs_Config_condition_add_static(pConfig, fs_Config_condition_group_add(pConfig, pNode), 1, "whiteBalanceMode", FsConfig_Condition_equal, "1");
    }
    {
        void *const pNode = fs_Config_node_integer_add(pConfig, pConfig, "imageMode", "图像亮度,色度,对比度,清晰度的控制模式", "图像亮度,色度,对比度,清晰度的控制模式,-1-第二种自动控制,0-自动控制,1-手动控制", FsConfig_nodeShowType_default, 0, 0x7, 0, 1, 1);
        fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_default, 0, "0-自动控制", "0-自动控制");
        fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_optional, -1, "-1-第二种自动控制", "-1-第二种自动控制");
        fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_optional, 1, "1-手动控制", "1-手动控制");
    }
    {
        void *const pNode = fs_Config_node_integer_add(pConfig, pConfig, "brightness", "亮度", "亮度,归一化到0-100,不设置表示沿用原来的值", FsConfig_nodeShowType_default, 0, 0x7, 0, 99, 1);
        fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_default, 50, "50", "50");
        fs_Config_condition_add_static(pConfig, fs_Config_condition_group_add(pConfig, pNode), 1, "imageMode", FsConfig_Condition_equal, "1");
    }
    {
        void *const pNode = fs_Config_node_integer_add(pConfig, pConfig, "colorSaturation", "色度", "色度,归一化到0-100,不设置表示沿用原来的值", FsConfig_nodeShowType_default, 0, 0x7, 0, 99, 1);
        fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_default, 50, "50", "50");
        fs_Config_condition_add_static(pConfig, fs_Config_condition_group_add(pConfig, pNode), 1, "imageMode", FsConfig_Condition_equal, "1");
    }
    {
        void *const pNode = fs_Config_node_integer_add(pConfig, pConfig, "contrast", "对比度", "对比度,归一化到0-100,不设置表示沿用原来的值", FsConfig_nodeShowType_default, 0, 0x7, 0, 99, 1);
        fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_default, 50, "50", "50");
        fs_Config_condition_add_static(pConfig, fs_Config_condition_group_add(pConfig, pNode), 1, "imageMode", FsConfig_Condition_equal, "1");
    }
    {
        void *const pNode = fs_Config_node_integer_add(pConfig, pConfig, "sharpness", "清晰度", "清晰度,归一化到0-100,不设置表示沿用原来的值", FsConfig_nodeShowType_default, 0, 0x7, 0, 99, 1);
        fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_default, 50, "50", "50");
        fs_Config_condition_add_static(pConfig, fs_Config_condition_group_add(pConfig, pNode), 1, "imageMode", FsConfig_Condition_equal, "1");
    }
    {
        void *const pNode = fs_Config_node_integer_add(pConfig, pConfig, "intensityAdjust", "明暗度矫正,归一化到0-100", "明暗度矫正,归一化到0-100", FsConfig_nodeShowType_default, 0, 0x7, 0, 100, 1);
        fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_default, 0, "0", "0");
        fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_optional, 1, "220", "220");
    }
    {

        void *const pNode = fs_Config_node_integer_add(pConfig, pConfig, "bwValid", "黑白热模式是否有效", "黑白热模式是否有效,0-无效,1-有效", FsConfig_nodeShowType_default, 0, 0x7, 0, 1, 1);
        fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_default, 0, "0", "0");
        fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_optional, 1, "1", "1");
    }
    {
        void *const pNode = fs_Config_node_integer_add(pConfig, pConfig, "bw", "黑白热模式", "黑白热模式", FsConfig_nodeShowType_default, 0, 0x7, 0, 1, 1);
        fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_default, 0, "0-白热", "0-白热");
        fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_optional, 1, "1-黑热", "1-黑热");
    }
    {
        void *const pNode = fs_Config_node_integer_add(pConfig, pConfig, "cutLeft", "图像拼接的左裁剪像素", "图像拼接的左裁剪像素,-1表示使用相机传过来的值", FsConfig_nodeShowType_default, 0, 0x7, -1, 300, 1);
        fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_default, -1, "-1", "-1");
    }
    {
        void *const pNode = fs_Config_node_integer_add(pConfig, pConfig, "cutRight", "图像拼接的右裁剪像素", "图像拼接的右裁剪像素,-1表示使用相机传过来的值", FsConfig_nodeShowType_default, 0, 0x7, -1, 300, 1);
        fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_default, -1, "-1", "-1");
    }
    {
        void *const pNode = fs_Config_node_integer_add(pConfig, pConfig, "cycleMove", "转圈", "转圈,0-表示未旋转,1-表示正在旋转", FsConfig_nodeShowType_default, 0, 0x7, 0, 1, 1);
        fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_default, 1, "旋转", "旋转");
        fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_optional, 0, "不旋转", "不旋转");
    }
    return pConfig;
}

static FsConfig *capture_P_protocol_imageinfoget() {
    FsConfig * const pConfig = fs_Config_new__IO();
    {
        void *const pNode = fs_Config_node_string_add(pConfig, pConfig, "command_type", "命令类型", "命令类型,此字段为开发人员分配,各接口唯一", 0, 0x7, 0, 19, 1);
        fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "cameractrl", "cameractrl", "cameractrl");
    }
    {
        void *const pNode = fs_Config_node_string_add(pConfig, pConfig, "command_for_uuid", "资源的uuid", "资源的uuid,表示使用此接口操作的资源编号", 0, 0x7, 0, 16, 1);
        fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "1", "1", "1");
    }
    {
        void *const pNode = fs_Config_node_string_add(pConfig, pConfig, "ctrl_type", "控制类型", "控制类型", 0, 0x7, 0, 16, 1);
        fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "imageinfoget", "图像信息获取", "图像信息获取");
    }
    {
        void *const pNode = fs_Config_node_integer_add(pConfig, pConfig, "priority", "控制优先级", "优先级,0-默认,不缓存调用者信息,1-比分析优先级低的定义,2-255:保留,256-29999:比分析优先级低的定义,\n"
                "30000-34999:视频分析专用的优先级,35000-64999:比分析优先级高的定义,65000-65534:保留;非0优先级中,\n"
                "    偶数表示可合并的优先级,即如有多个拥有相同偶数优先级的客户端访问同一相机,这些所有客户端的指令会按照新指令覆盖旧指令方式生效;\n"
                "    奇数表示独占优先级,即如有多个拥有相同奇数优先级的客户端访问同一相机,只有第一个客户端的指令会生效,其他客户端的指令会被拒绝", FsConfig_nodeShowType_default, 0, 0x7, 0, 65534, 1);
        fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_default, 0, "0", "0");
        fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_optional, 256, "256", "256");
        fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_optional, 257, "257", "257");
    }
    {
        void *const pNode = fs_Config_node_string_add(pConfig, pConfig, "return_type", "回执类型", "回执类型,缺失或无效时回执的数据类型不确定,支持ebml(压缩数据),xml,json", 0, 0x7, 3, 4, 1);
        fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "ebml", "ebml", "ebml");
        fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_optional, "xml", "xml", "xml");
        fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_optional, "json", "json", "json");
    }

    return pConfig;
}

static FsConfig *capture_P_protocol_chipCmd() {
    FsConfig * const pConfig = fs_Config_new__IO();
    {
        void *const pNode = fs_Config_node_string_add(pConfig, pConfig, "command_type", "命令类型", "命令类型,此字段为开发人员分配,各接口唯一", 0, 0x7, 0, 19, 1);
        fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "cameractrl", "cameractrl", "cameractrl");
    }
    {
        void *const pNode = fs_Config_node_string_add(pConfig, pConfig, "command_for_uuid", "资源的uuid", "资源的uuid,表示使用此接口操作的资源编号", 0, 0x7, 0, 16, 1);
        fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "1", "1", "1");
    }
    {
        void *const pNode = fs_Config_node_string_add(pConfig, pConfig, "ctrl_type", "控制类型", "控制类型", 0, 0x7, 0, 16, 1);
        fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "chipCmd", "向机芯直接发送命令", "向机芯直接发送命令");
    }
    {
        void *const pNode = fs_Config_node_integer_add(pConfig, pConfig, "priority", "控制优先级", "优先级,0-默认,不缓存调用者信息,1-比分析优先级低的定义,2-255:保留,256-29999:比分析优先级低的定义,\n"
                "30000-34999:视频分析专用的优先级,35000-64999:比分析优先级高的定义,65000-65534:保留;非0优先级中,\n"
                "    偶数表示可合并的优先级,即如有多个拥有相同偶数优先级的客户端访问同一相机,这些所有客户端的指令会按照新指令覆盖旧指令方式生效;\n"
                "    奇数表示独占优先级,即如有多个拥有相同奇数优先级的客户端访问同一相机,只有第一个客户端的指令会生效,其他客户端的指令会被拒绝", FsConfig_nodeShowType_default, 0, 0x7, 0, 65534, 1);
        fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_default, 0, "0", "0");
        fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_optional, 256, "256", "256");
        fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_optional, 257, "257", "257");
    }
    {
        void *const pNode = fs_Config_node_integer_add(pConfig, pConfig, "dataType", "命令的数据类型", "命令的数据类型", FsConfig_nodeShowType_default, 0, 0x7, 0, 1, 1);
        fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_default, 0, "字符串", "字符串");
        fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_optional, 1, "二进制(cmd以字符串输入时使用base64编码)", "二进制(cmd以字符串输入时使用base64编码)");
    }
    {
        void *const pNode = fs_Config_node_string_add(pConfig, pConfig, "cmd", "命令", "命令", 0, 0x7, 0, 64, 1);
        fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "AA 05 00 2D 01 00 DD EB AA", "inflray 白热", "inflray 白热");
        fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_optional, "AA 05 00 2D 01 01 DD EB AA", "inflray 黑热", "inflray 黑热");
        fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_optional, "AA 06 01 A3 01 03 32 8A EB AA", "inflray 外同步(每秒16帧左右)", "inflray 外同步(每秒16帧左右)");
        fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_optional, "AA 06 01 A3 01 00 1E 73 EB AA", "inflray 内同步(每秒30帧左右)", "inflray 内同步(每秒30帧左右)");
    }

    return pConfig;
}

static FsConfig *capture_P_protocol_shutterReset() {
    FsConfig * const pConfig = fs_Config_new__IO();
    {
        void *const pNode = fs_Config_node_string_add(pConfig, pConfig, "command_type", "命令类型", "命令类型,此字段为开发人员分配,各接口唯一", 0, 0x7, 0, 19, 1);
        fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "cameractrl", "cameractrl", "cameractrl");
    }
    {
        void *const pNode = fs_Config_node_string_add(pConfig, pConfig, "command_for_uuid", "资源的uuid", "资源的uuid,表示使用此接口操作的资源编号", 0, 0x7, 0, 16, 1);
        fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "1", "1", "1");
    }
    {
        void *const pNode = fs_Config_node_string_add(pConfig, pConfig, "ctrl_type", "控制类型", "控制类型", 0, 0x7, 0, 16, 1);
        fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "shutterReset", "重置快门", "重置快门");
    }
    {
        void *const pNode = fs_Config_node_integer_add(pConfig, pConfig, "priority", "控制优先级", "优先级,0-默认,不缓存调用者信息,1-比分析优先级低的定义,2-255:保留,256-29999:比分析优先级低的定义,\n"
                "30000-34999:视频分析专用的优先级,35000-64999:比分析优先级高的定义,65000-65534:保留;非0优先级中,\n"
                "    偶数表示可合并的优先级,即如有多个拥有相同偶数优先级的客户端访问同一相机,这些所有客户端的指令会按照新指令覆盖旧指令方式生效;\n"
                "    奇数表示独占优先级,即如有多个拥有相同奇数优先级的客户端访问同一相机,只有第一个客户端的指令会生效,其他客户端的指令会被拒绝", FsConfig_nodeShowType_default, 0, 0x7, 0, 65534, 1);
        fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_default, 0, "0", "0");
        fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_optional, 256, "256", "256");
        fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_optional, 257, "257", "257");
    }
    return pConfig;
}

static FsConfig *capture_P_protocol_errorInfo() {
    FsConfig * const pConfig = fs_Config_new__IO();
    {
        void *const pNode = fs_Config_node_string_add(pConfig, pConfig, "command_type", "命令类型", "命令类型,此字段为开发人员分配,各接口唯一", 0, 0x7, 0, 19, 1);
        fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "errorInfo", "errorInfo", "errorInfo");
    }
    {
        void *const pNode = fs_Config_node_string_add(pConfig, pConfig, "command_for_uuid", "资源的uuid", "资源的uuid,表示使用此接口操作的资源编号", 0, 0x7, 0, 16, 1);
        fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "1", "1", "1");
    }
    {
        void *const pNode = fs_Config_node_string_add(pConfig, pConfig, "return_type", "回执类型", "回执类型,缺失或无效时回执的数据类型不确定,支持ebml(压缩数据),xml,json", 0, 0x7, 3, 4, 1);
        fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "ebml", "ebml", "ebml");
        fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_optional, "xml", "xml", "xml");
        fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_optional, "json", "json", "json");
    }
    return pConfig;
}


/* 在有用户请求此命令字时的调用函数,成功返回1,失败返回-1,需要引用此连接返回-128,为空表示此命令字不允许远程调用 */
static int capture_P_cmd_cb(/* 与请求相关的信息,用于识别是发给哪个客户端的数据,用3个int来储存 */const unsigned int requestID_3[], /* 收到数据的前4字节 */unsigned int head
        , /* 收到的数据 */FsEbml *pEbml, /* 客户端发送请求的数据类型,1-ebml数据,2-xml数据,3-json数据 */ char requestDataType, /* 调用函数的指针 */ struct Capture_item * const pCapture_item
        , /* 缓存Buffer,不为空 */FsObjectBaseBuffer * const pObjectBaseBuffer, /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer) {
#undef FsFunctionName
#define FsFunctionName capture_P_cmd_cb
    fs_Ebml_out_debug(pEbml, stdout, pShareBuffer), printf("\n");
    /* 缓存的控制客户端连接 */
    if (NULL == pCapture_item->p.__ctrlClientList) pCapture_item->p.__ctrlClientList = fs_StructList_new__IO(6, sizeof (int)*6);
    const char *const ctrl_type = fs_Ebml_node_get_first_string(pEbml, (struct FsEbml_node*) pEbml, "ctrl_type", " ");
    ////////////////////////////////////////////////////////////////////////////
#define __capture_P_cmd_cb_error 0
#define __capture_P_cmd_cb_chipCmd 1
#define __capture_P_cmd_cb_chipCmd_lt 0
#define __capture_P_cmd_cb_chipCmd_gt 0
#define __capture_P_cmd_cb_focus 2
#define __capture_P_cmd_cb_focus_lt 1
#define __capture_P_cmd_cb_focus_gt 3
#define __capture_P_cmd_cb_imageinfoget 3
#define __capture_P_cmd_cb_imageinfoget_lt 0
#define __capture_P_cmd_cb_imageinfoget_gt 0
#define __capture_P_cmd_cb_imageinfoset 4
#define __capture_P_cmd_cb_imageinfoset_lt 2
#define __capture_P_cmd_cb_imageinfoset_gt 6  
#define __capture_P_cmd_cb_maskget 5
#define __capture_P_cmd_cb_maskget_lt 0
#define __capture_P_cmd_cb_maskget_gt 0      
#define __capture_P_cmd_cb_preset_clean 6
#define __capture_P_cmd_cb_preset_clean_lt 5
#define __capture_P_cmd_cb_preset_clean_gt 7
#define __capture_P_cmd_cb_preset_goto 7
#define __capture_P_cmd_cb_preset_goto_lt 0
#define __capture_P_cmd_cb_preset_goto_gt 0
#define __capture_P_cmd_cb_preset_set 8
#define __capture_P_cmd_cb_preset_set_lt 4
#define __capture_P_cmd_cb_preset_set_gt 12
#define __capture_P_cmd_cb_ptzget 9
#define __capture_P_cmd_cb_ptzget_lt 0
#define __capture_P_cmd_cb_ptzget_gt 0
#define __capture_P_cmd_cb_ptzgoto 10
#define __capture_P_cmd_cb_ptzgoto_lt 9
#define __capture_P_cmd_cb_ptzgoto_gt 11 
#define __capture_P_cmd_cb_release 11
#define __capture_P_cmd_cb_release_lt 0
#define __capture_P_cmd_cb_release_gt 0
#define __capture_P_cmd_cb_shutterReset 12
#define __capture_P_cmd_cb_shutterReset_lt 10
#define __capture_P_cmd_cb_shutterReset_gt 14
#define __capture_P_cmd_cb_turn 13
#define __capture_P_cmd_cb_turn_lt 0
#define __capture_P_cmd_cb_turn_gt 0
#define __capture_P_cmd_cb_zoom 14
#define __capture_P_cmd_cb_zoom_lt 13
#define __capture_P_cmd_cb_zoom_gt 15
#define __capture_P_cmd_cb_zoom3D 15
#define __capture_P_cmd_cb_zoom3D_lt 0
#define __capture_P_cmd_cb_zoom3D_gt 0
    goto FsMacrosFunctionTag(8);
    ////////////////////////////////////////////////////////////////////////////
#if FsMacrosFunction(error) >= 0
    FsMacrosFunctionTag(FsMacrosFunction(error)) :;
    FsLog(FsLogType_error, FsPrintfIndex, "Invalid data:\n");
    if (FsLogTypeCheck(FsLogType_error)) fs_Ebml_out_debug(pEbml, stdout, pShareBuffer), printf("\n");
    fflush(stdout);
    return -1;
#endif
    ////////////////////////////////////////////////////////////////////////////
#if FsMacrosFunction(turn) > 0 // 转动
    FsMacrosFunctionTag(FsMacrosFunction(turn)) :;
    {
        const int _rv = strcmp(ctrl_type, "turn");
        if (0 == _rv) {
#if 1        
#ifndef __capture_P_cmd_cb_ctrl_priority_check 
            const unsigned short priority = fs_Ebml_node_get_first_Integer(pEbml, (struct FsEbml_node*) pEbml, "priority", 0);
            pthread_mutex_lock(&pCapture_item->p.__ctrlClientList->mutex);
            configManager_conncet_refer_check(pCapture_item->ro._pCapture->ro._pConfigManager, pCapture_item->p.__ctrlClientList);
            if ((priority & 0xFFFE) < pCapture_item->p.ctrlPriority && (pCapture_item->p.__ctrlClientList->nodeCount > 1 || (1 == pCapture_item->p.__ctrlClientList->nodeCount &&
                    memcmp(requestID_3, pCapture_item->p.__ctrlClientList->pNode + pCapture_item->p.__ctrlClientList->startIndex * sizeof (unsigned int)*6, sizeof (unsigned int)*3) != 0))) {
                /* 连接优先级不够 */
                if (fs_StructList_remove_order_custom(pCapture_item->p.__ctrlClientList, requestID_3, sizeof (unsigned int)*3)>-1)
                    configManager_connect_release(pCapture_item->ro._pCapture->ro._pConfigManager, requestID_3, 0);
                unsigned int ipv4;
                unsigned short ipv4Port;
                configManager_connect_get_addr(&ipv4, &ipv4Port, requestID_3);
                FsPrintf2(1, "Priority(=%hu/%hu) less,client:%p,%x:%hhu.%hhu.%hhu.%hhu:%hu,connectCount=%lu,client:\n", priority, pCapture_item->p.ctrlPriority, *(void**) requestID_3, requestID_3[2]
                        , Fs_Network_Ipv4_print(ipv4), ipv4Port, pCapture_item->p.__ctrlClientList->nodeCount);
                FsObjectBase * const sendData = (FsObjectBase*) fsMalloc(sizeof (FsObjectBase) +
                        8 + Memery_Alignment(sizeof ("Priority(=65535/65535) less,connectCount=1234567890,client[]") - 1 + 22 * pCapture_item->p.__ctrlClientList->nodeCount) + sizeof (unsigned int)*3);
                fs_ObjectBase_init(sendData, sizeof (FsObjectBase) +
                        8 + Memery_Alignment(sizeof ("Priority(=65535/65535) less,connectCount=1234567890,client[]") - 1 + 22 * pCapture_item->p.__ctrlClientList->nodeCount) + sizeof (unsigned int)*3,
                        sizeof (FsObjectBase));
                sendData->len = 8 + sprintf(sendData->data + 8, "Priority(=%hu/%hu) less,connectCount=%lu,client[", priority, pCapture_item->p.ctrlPriority, pCapture_item->p.__ctrlClientList->nodeCount);
                {
                    unsigned int (*ppNode)[6] = ((unsigned int (*)[6])pCapture_item->p.__ctrlClientList->pNode) + pCapture_item->p.__ctrlClientList->startIndex;
                    unsigned int i = 0;
                    for (; i < pCapture_item->p.__ctrlClientList->nodeCount; i++) {
                        configManager_connect_get_addr(&ipv4, &ipv4Port, *ppNode++);
                        printf("[%u]:%hhu.%hhu.%hhu.%hhu:%hu\n", i, Fs_Network_Ipv4_print(ipv4), ipv4Port);
                        sendData->len += sprintf(sendData->data + sendData->len, "%hhu.%hhu.%hhu.%hhu:%hu,", Fs_Network_Ipv4_print(ipv4), ipv4Port);
                    }
                }
                pthread_mutex_unlock(&pCapture_item->p.__ctrlClientList->mutex);
                sendData->data[sendData->len - 1] = ']';
                *((unsigned int *) sendData->data) = (head & 0xFFFFFFF0U) | 0x6U, *((unsigned int *) (sendData->data + 4)) = sendData->len - 8;
                memcpy(sendData->data + Memery_Alignment(sendData->len), requestID_3, sizeof (unsigned int)*3);
                configManager_send_pthreadSafety__OI_2(pCapture_item->ro._pCapture->ro._pConfigManager, sendData);
                return -1;
            }
            pthread_mutex_unlock(&pCapture_item->p.__ctrlClientList->mutex);
#endif
            short speedx = fs_Ebml_node_get_first_Integer(pEbml, (struct FsEbml_node*) pEbml, "speedx", 0);
            short speedy = fs_Ebml_node_get_first_Integer(pEbml, (struct FsEbml_node*) pEbml, "speedy", 0);
            float time = fs_Ebml_node_get_first_Float(pEbml, (struct FsEbml_node*) pEbml, "time", 0.0);
            int rv;
            const unsigned long long mask = pCapture_item->ro._ctrl_function->mask_get_pthreadSafety(pCapture_item->ro.__pCamera_item, pCapture_item->ro.__pCamera_ctrl_item, NULL, NULL, 0, NULL, NULL, 0, NULL, NULL, 0, NULL, NULL, 0, NULL, NULL, pObjectBaseBuffer, pShareBuffer);
            if (mask & Camera_ctrl_Mask_turn)rv = pCapture_item->ro._ctrl_function->turn_pthreadSafety(pCapture_item->ro.__pCamera_item, pCapture_item->ro.__pCamera_ctrl_item, pCapture_item, priority, speedx, speedy, time, pObjectBaseBuffer, pShareBuffer);
            else if (speedx != 0) {
                if (speedx > 0) {
                    if (speedy == speedx) rv = pCapture_item->ro._ctrl_function->turnRightDown_pthreadSafety(pCapture_item->ro.__pCamera_item, pCapture_item->ro.__pCamera_ctrl_item, pCapture_item, priority, speedx, time, pObjectBaseBuffer, pShareBuffer);
                    else if (speedy == -speedx)rv = pCapture_item->ro._ctrl_function->turnRightUp_pthreadSafety(pCapture_item->ro.__pCamera_item, pCapture_item->ro.__pCamera_ctrl_item, pCapture_item, priority, speedx, time, pObjectBaseBuffer, pShareBuffer);
                    else if (0 == speedy)rv = pCapture_item->ro._ctrl_function->turnRight_pthreadSafety(pCapture_item->ro.__pCamera_item, pCapture_item->ro.__pCamera_ctrl_item, pCapture_item, priority, speedx, time, pObjectBaseBuffer, pShareBuffer);
                    else rv = -1;
                } else {
                    if (speedy == speedx) rv = pCapture_item->ro._ctrl_function->turnLeftUp_pthreadSafety(pCapture_item->ro.__pCamera_item, pCapture_item->ro.__pCamera_ctrl_item, pCapture_item, priority, -speedx, time, pObjectBaseBuffer, pShareBuffer);
                    else if (speedy == -speedx)rv = pCapture_item->ro._ctrl_function->turnLeftDown_pthreadSafety(pCapture_item->ro.__pCamera_item, pCapture_item->ro.__pCamera_ctrl_item, pCapture_item, priority, -speedx, time, pObjectBaseBuffer, pShareBuffer);
                    else if (0 == speedy)rv = pCapture_item->ro._ctrl_function->turnLeft_pthreadSafety(pCapture_item->ro.__pCamera_item, pCapture_item->ro.__pCamera_ctrl_item, pCapture_item, priority, -speedx, time, pObjectBaseBuffer, pShareBuffer);
                    else rv = -1;
                }
            } else if (speedy > 0) rv = pCapture_item->ro._ctrl_function->turnDown_pthreadSafety(pCapture_item->ro.__pCamera_item, pCapture_item->ro.__pCamera_ctrl_item, pCapture_item, priority, speedy, time, pObjectBaseBuffer, pShareBuffer);
            else if (speedy < 0) rv = pCapture_item->ro._ctrl_function->turnUp_pthreadSafety(pCapture_item->ro.__pCamera_item, pCapture_item->ro.__pCamera_ctrl_item, pCapture_item, priority, -speedy, time, pObjectBaseBuffer, pShareBuffer);
            else {
                if (mask & Camera_ctrl_Mask_turn_0)rv = pCapture_item->ro._ctrl_function->turnLeft_pthreadSafety(pCapture_item->ro.__pCamera_item, pCapture_item->ro.__pCamera_ctrl_item, pCapture_item, priority, speedx, time, pObjectBaseBuffer, pShareBuffer);
                else if (mask & Camera_ctrl_Mask_turn_90)rv = pCapture_item->ro._ctrl_function->turnUp_pthreadSafety(pCapture_item->ro.__pCamera_item, pCapture_item->ro.__pCamera_ctrl_item, pCapture_item, priority, speedy, time, pObjectBaseBuffer, pShareBuffer);
                else if (mask & Camera_ctrl_Mask_turn_45)rv = pCapture_item->ro._ctrl_function->turnLeftUp_pthreadSafety(pCapture_item->ro.__pCamera_item, pCapture_item->ro.__pCamera_ctrl_item, pCapture_item, priority, speedx, time, pObjectBaseBuffer, pShareBuffer);
                else rv = 0;
            }
            if (rv != 1) {
#ifndef __capture_P_cmd_cb_ctrl_cb_failed 
                Capture_itemLog(FsLogType_error, "Ctrl camera failed,data:\n");
                fs_Ebml_out_debug(pEbml, stdout, pShareBuffer), printf("\n");
                pthread_mutex_lock(&pCapture_item->p.__ctrlClientList->mutex);
                if (fs_StructList_remove_order_custom(pCapture_item->p.__ctrlClientList, requestID_3, sizeof (unsigned int)*3)>-1)
                    configManager_connect_release(pCapture_item->ro._pCapture->ro._pConfigManager, requestID_3, 0);
                if (0 == pCapture_item->p.__ctrlClientList->nodeCount && pCapture_item->p.ctrlPriority != 0) {
                    pCapture_item->ro._ctrl_function->cancelKey_pthreadSafety(pCapture_item->ro.__pCamera_item, pCapture_item->ro.__pCamera_ctrl_item, pCapture_item, pObjectBaseBuffer, pShareBuffer);
                    pCapture_item->p.ctrlPriority = 0;
                }
                pthread_mutex_unlock(&pCapture_item->p.__ctrlClientList->mutex);
                FsObjectBase * const sendData = (FsObjectBase*) fsMalloc(sizeof (FsObjectBase) + 8 + Memery_Alignment(sizeof ("Cb failed.\n") - 1) + sizeof (unsigned int)*3);
                fs_ObjectBase_init(sendData, sizeof (FsObjectBase) + 8 + Memery_Alignment(sizeof ("Cb failed.\n") - 1) + sizeof (unsigned int)*3, sizeof (FsObjectBase));
                sendData->len = 8 + sizeof ("Cb failed.\n") - 1;
                *((unsigned int *) sendData->data) = (head & 0xFFFFFFF0U) | 0x6U, *((unsigned int *) (sendData->data + 4)) = sendData->len - 8;
                memcpy(sendData->data + 8, "Cb failed.\n", sizeof ("Cb failed.\n") - 1);
                memcpy(sendData->data + Memery_Alignment(sendData->len), requestID_3, sizeof (unsigned int)*3);
                configManager_send_pthreadSafety__OI_2(pCapture_item->ro._pCapture->ro._pConfigManager, sendData);
                return -1;
#endif
            }
            ////////////////////////////////////////////////////////////////////////
            /* 发回执数据 */
#ifndef __capture_P_cmd_cb_ctrl_cb_ok 
            configManager_conncet_refer_send_buffer(FsStringLenData("ok"), requestID_3, (head & 0xFFFFFFF0U) | 0x2U, pObjectBaseBuffer);
            if (0 == priority) {
                if (pCapture_item->p.ctrlPriority != 0) {
                    pCapture_item->p.ctrlPriority = 0;
                    if (1 == pCapture_item->p.__ctrlClientList->nodeCount) {
                        configManager_connect_release(pCapture_item->ro._pCapture->ro._pConfigManager, requestID_3, 0);
                        fs_StructList_clear(pCapture_item->p.__ctrlClientList);
                    }
                }
                return 1;
            } else {
                if (pCapture_item->p.ctrlPriority != priority) {
                    pCapture_item->p.ctrlPriority = priority;
                    capture_private_item_ctrlClient_release(pCapture_item, requestID_3);
                }
                unsigned int data[6] = {requestID_3[0], requestID_3[1], requestID_3[2], head, (unsigned int) requestDataType, 0};
                if (fs_StructList_insert_order_custom_update_pthreadSafety(pCapture_item->p.__ctrlClientList, data, sizeof (unsigned int)*3) < 0) return 1;
                else return -128;
            }
#endif
#endif
        } else if (_rv < 0)goto FsMacrosFunctionTag(FsMacrosFunction(turn_lt));
        else goto FsMacrosFunctionTag(FsMacrosFunction(turn_gt));
    }
#endif
    ////////////////////////////////////////////////////////////////////////////
#if FsMacrosFunction(zoom) > 0 // 变倍
    FsMacrosFunctionTag(FsMacrosFunction(zoom)) :;
    {
        const int _rv = strcmp(ctrl_type, "zoom");
        if (0 == _rv) {
#if 1        
#ifndef __capture_P_cmd_cb_ctrl_priority_check 
            const unsigned short priority = fs_Ebml_node_get_first_Integer(pEbml, (struct FsEbml_node*) pEbml, "priority", 0);
            pthread_mutex_lock(&pCapture_item->p.__ctrlClientList->mutex);
            configManager_conncet_refer_check(pCapture_item->ro._pCapture->ro._pConfigManager, pCapture_item->p.__ctrlClientList);
            if ((priority & 0xFFFE) < pCapture_item->p.ctrlPriority && (pCapture_item->p.__ctrlClientList->nodeCount > 1 || (1 == pCapture_item->p.__ctrlClientList->nodeCount &&
                    memcmp(requestID_3, pCapture_item->p.__ctrlClientList->pNode + pCapture_item->p.__ctrlClientList->startIndex * sizeof (unsigned int)*6, sizeof (unsigned int)*3) != 0))) {
                /* 连接优先级不够 */
                if (fs_StructList_remove_order_custom(pCapture_item->p.__ctrlClientList, requestID_3, sizeof (unsigned int)*3)>-1)
                    configManager_connect_release(pCapture_item->ro._pCapture->ro._pConfigManager, requestID_3, 0);
                unsigned int ipv4;
                unsigned short ipv4Port;
                configManager_connect_get_addr(&ipv4, &ipv4Port, requestID_3);
                FsPrintf2(1, "Priority(=%hu/%hu) less,client:%p,%x:%hhu.%hhu.%hhu.%hhu:%hu,connectCount=%lu,client:\n", priority, pCapture_item->p.ctrlPriority, *(void**) requestID_3, requestID_3[2]
                        , Fs_Network_Ipv4_print(ipv4), ipv4Port, pCapture_item->p.__ctrlClientList->nodeCount);
                FsObjectBase * const sendData = (FsObjectBase*) fsMalloc(sizeof (FsObjectBase) +
                        8 + Memery_Alignment(sizeof ("Priority(=65535/65535) less,connectCount=1234567890,client[]") - 1 + 22 * pCapture_item->p.__ctrlClientList->nodeCount) + sizeof (unsigned int)*3);
                fs_ObjectBase_init(sendData, sizeof (FsObjectBase) +
                        8 + Memery_Alignment(sizeof ("Priority(=65535/65535) less,connectCount=1234567890,client[]") - 1 + 22 * pCapture_item->p.__ctrlClientList->nodeCount) + sizeof (unsigned int)*3,
                        sizeof (FsObjectBase));
                sendData->len = 8 + sprintf(sendData->data + 8, "Priority(=%hu/%hu) less,connectCount=%lu,client[", priority, pCapture_item->p.ctrlPriority, pCapture_item->p.__ctrlClientList->nodeCount);
                {
                    unsigned int (*ppNode)[6] = ((unsigned int (*)[6])pCapture_item->p.__ctrlClientList->pNode) + pCapture_item->p.__ctrlClientList->startIndex;
                    unsigned int i = 0;
                    for (; i < pCapture_item->p.__ctrlClientList->nodeCount; i++) {
                        configManager_connect_get_addr(&ipv4, &ipv4Port, *ppNode++);
                        printf("[%u]:%hhu.%hhu.%hhu.%hhu:%hu\n", i, Fs_Network_Ipv4_print(ipv4), ipv4Port);
                        sendData->len += sprintf(sendData->data + sendData->len, "%hhu.%hhu.%hhu.%hhu:%hu,", Fs_Network_Ipv4_print(ipv4), ipv4Port);
                    }
                }
                pthread_mutex_unlock(&pCapture_item->p.__ctrlClientList->mutex);
                sendData->data[sendData->len - 1] = ']';
                *((unsigned int *) sendData->data) = (head & 0xFFFFFFF0U) | 0x6U, *((unsigned int *) (sendData->data + 4)) = sendData->len - 8;
                memcpy(sendData->data + Memery_Alignment(sendData->len), requestID_3, sizeof (unsigned int)*3);
                configManager_send_pthreadSafety__OI_2(pCapture_item->ro._pCapture->ro._pConfigManager, sendData);
                return -1;
            }
            pthread_mutex_unlock(&pCapture_item->p.__ctrlClientList->mutex);
#endif
            struct FsEbml_node* pEbml_node = fs_Ebml_node_get_first(pEbml, (struct FsEbml_node*) pEbml, "speed");
            if (NULL == pEbml_node || 0 == pEbml_node->data.lenth) {
                FsPrintf(1, "Invalid speedy,data:\n");
                fs_Ebml_out_debug(pEbml, stdout, pShareBuffer), printf("\n");
#ifndef __capture_P_cmd_cb_ctrl_data_check_error 
                pthread_mutex_lock(&pCapture_item->p.__ctrlClientList->mutex);
                if (fs_StructList_remove_order_custom(pCapture_item->p.__ctrlClientList, requestID_3, sizeof (unsigned int)*3)>-1)
                    configManager_connect_release(pCapture_item->ro._pCapture->ro._pConfigManager, requestID_3, 0);
                if (0 == pCapture_item->p.__ctrlClientList->nodeCount && pCapture_item->p.ctrlPriority != 0) {
                    pCapture_item->ro._ctrl_function->cancelKey_pthreadSafety(pCapture_item->ro.__pCamera_item, pCapture_item->ro.__pCamera_ctrl_item, pCapture_item, pObjectBaseBuffer, pShareBuffer);
                    pCapture_item->p.ctrlPriority = 0;
                }
                pthread_mutex_unlock(&pCapture_item->p.__ctrlClientList->mutex);
                return -1;
#endif
            }
            short speed = (pEbml_node->type & 0x7) == FsEbmlNodeType_Integer ? *((long long*) pEbml_node->data.buffer) : atoi(pEbml_node->data.buffer);
            pEbml_node = fs_Ebml_node_get_first(pEbml, (struct FsEbml_node*) pEbml, "time");
            if (NULL == pEbml_node || 0 == pEbml_node->data.lenth) {
                FsPrintf(1, "Invalid y,data:\n");
                fs_Ebml_out_debug(pEbml, stdout, pShareBuffer), printf("\n");
#ifndef __capture_P_cmd_cb_ctrl_data_check_error 
                pthread_mutex_lock(&pCapture_item->p.__ctrlClientList->mutex);
                if (fs_StructList_remove_order_custom(pCapture_item->p.__ctrlClientList, requestID_3, sizeof (unsigned int)*3)>-1)
                    configManager_connect_release(pCapture_item->ro._pCapture->ro._pConfigManager, requestID_3, 0);
                if (0 == pCapture_item->p.__ctrlClientList->nodeCount && pCapture_item->p.ctrlPriority != 0) {
                    pCapture_item->ro._ctrl_function->cancelKey_pthreadSafety(pCapture_item->ro.__pCamera_item, pCapture_item->ro.__pCamera_ctrl_item, pCapture_item, pObjectBaseBuffer, pShareBuffer);
                    pCapture_item->p.ctrlPriority = 0;
                }
                pthread_mutex_unlock(&pCapture_item->p.__ctrlClientList->mutex);
                return -1;
#endif
            }
            float time = (pEbml_node->type & 0x7) == FsEbmlNodeType_Float ? *((double*) pEbml_node->data.buffer) : atof(pEbml_node->data.buffer);
            if (pCapture_item->ro._ctrl_function->zoom_pthreadSafety(pCapture_item->ro.__pCamera_item, pCapture_item->ro.__pCamera_ctrl_item, pCapture_item, priority, speed, time, pObjectBaseBuffer, pShareBuffer) != 1) {
#ifndef __capture_P_cmd_cb_ctrl_cb_failed 
                Capture_itemLog(FsLogType_error, "Ctrl camera failed,data:\n");
                fs_Ebml_out_debug(pEbml, stdout, pShareBuffer), printf("\n");
                pthread_mutex_lock(&pCapture_item->p.__ctrlClientList->mutex);
                if (fs_StructList_remove_order_custom(pCapture_item->p.__ctrlClientList, requestID_3, sizeof (unsigned int)*3)>-1)
                    configManager_connect_release(pCapture_item->ro._pCapture->ro._pConfigManager, requestID_3, 0);
                if (0 == pCapture_item->p.__ctrlClientList->nodeCount && pCapture_item->p.ctrlPriority != 0) {
                    pCapture_item->ro._ctrl_function->cancelKey_pthreadSafety(pCapture_item->ro.__pCamera_item, pCapture_item->ro.__pCamera_ctrl_item, pCapture_item, pObjectBaseBuffer, pShareBuffer);
                    pCapture_item->p.ctrlPriority = 0;
                }
                pthread_mutex_unlock(&pCapture_item->p.__ctrlClientList->mutex);
                FsObjectBase * const sendData = (FsObjectBase*) fsMalloc(sizeof (FsObjectBase) + 8 + Memery_Alignment(sizeof ("Cb failed.\n") - 1) + sizeof (unsigned int)*3);
                fs_ObjectBase_init(sendData, sizeof (FsObjectBase) + 8 + Memery_Alignment(sizeof ("Cb failed.\n") - 1) + sizeof (unsigned int)*3, sizeof (FsObjectBase));
                sendData->len = 8 + sizeof ("Cb failed.\n") - 1;
                *((unsigned int *) sendData->data) = (head & 0xFFFFFFF0U) | 0x6U, *((unsigned int *) (sendData->data + 4)) = sendData->len - 8;
                memcpy(sendData->data + 8, "Cb failed.\n", sizeof ("Cb failed.\n") - 1);
                memcpy(sendData->data + Memery_Alignment(sendData->len), requestID_3, sizeof (unsigned int)*3);
                configManager_send_pthreadSafety__OI_2(pCapture_item->ro._pCapture->ro._pConfigManager, sendData);
                return -1;
#endif
            }
            ////////////////////////////////////////////////////////////////////////
            /* 发回执数据 */
#ifndef __capture_P_cmd_cb_ctrl_cb_ok 
            configManager_conncet_refer_send_buffer(FsStringLenData("ok"), requestID_3, (head & 0xFFFFFFF0U) | 0x2U, pObjectBaseBuffer);
            if (0 == priority) {
                if (pCapture_item->p.ctrlPriority != 0) {
                    pCapture_item->p.ctrlPriority = 0;
                    if (1 == pCapture_item->p.__ctrlClientList->nodeCount) {
                        configManager_connect_release(pCapture_item->ro._pCapture->ro._pConfigManager, requestID_3, 0);
                        fs_StructList_clear(pCapture_item->p.__ctrlClientList);
                    }
                }
                return 1;
            } else {
                if (pCapture_item->p.ctrlPriority != priority) {
                    pCapture_item->p.ctrlPriority = priority;
                    capture_private_item_ctrlClient_release(pCapture_item, requestID_3);
                }
                unsigned int data[6] = {requestID_3[0], requestID_3[1], requestID_3[2], head, (unsigned int) requestDataType, 0};
                if (fs_StructList_insert_order_custom_update_pthreadSafety(pCapture_item->p.__ctrlClientList, data, sizeof (unsigned int)*3) < 0) return 1;
                else return -128;
            }
#endif
#endif
        } else if (_rv < 0)goto FsMacrosFunctionTag(FsMacrosFunction(zoom_lt));
        else goto FsMacrosFunctionTag(FsMacrosFunction(zoom_gt));
    }
#endif
    ////////////////////////////////////////////////////////////////////////////
#if FsMacrosFunction(focus) > 0 // 聚焦
    FsMacrosFunctionTag(FsMacrosFunction(focus)) :;
    {
        const int _rv = strcmp(ctrl_type, "focus");
        if (0 == _rv) {
#if 1        
#ifndef __capture_P_cmd_cb_ctrl_priority_check 
            const unsigned short priority = fs_Ebml_node_get_first_Integer(pEbml, (struct FsEbml_node*) pEbml, "priority", 0);
            pthread_mutex_lock(&pCapture_item->p.__ctrlClientList->mutex);
            configManager_conncet_refer_check(pCapture_item->ro._pCapture->ro._pConfigManager, pCapture_item->p.__ctrlClientList);
            if ((priority & 0xFFFE) < pCapture_item->p.ctrlPriority && (pCapture_item->p.__ctrlClientList->nodeCount > 1 || (1 == pCapture_item->p.__ctrlClientList->nodeCount &&
                    memcmp(requestID_3, pCapture_item->p.__ctrlClientList->pNode + pCapture_item->p.__ctrlClientList->startIndex * sizeof (unsigned int)*6, sizeof (unsigned int)*3) != 0))) {
                /* 连接优先级不够 */
                if (fs_StructList_remove_order_custom(pCapture_item->p.__ctrlClientList, requestID_3, sizeof (unsigned int)*3)>-1)
                    configManager_connect_release(pCapture_item->ro._pCapture->ro._pConfigManager, requestID_3, 0);
                unsigned int ipv4;
                unsigned short ipv4Port;
                configManager_connect_get_addr(&ipv4, &ipv4Port, requestID_3);
                FsPrintf2(1, "Priority(=%hu/%hu) less,client:%p,%x:%hhu.%hhu.%hhu.%hhu:%hu,connectCount=%lu,client:\n", priority, pCapture_item->p.ctrlPriority, *(void**) requestID_3, requestID_3[2]
                        , Fs_Network_Ipv4_print(ipv4), ipv4Port, pCapture_item->p.__ctrlClientList->nodeCount);
                FsObjectBase * const sendData = (FsObjectBase*) fsMalloc(sizeof (FsObjectBase) +
                        8 + Memery_Alignment(sizeof ("Priority(=65535/65535) less,connectCount=1234567890,client[]") - 1 + 22 * pCapture_item->p.__ctrlClientList->nodeCount) + sizeof (unsigned int)*3);
                fs_ObjectBase_init(sendData, sizeof (FsObjectBase) +
                        8 + Memery_Alignment(sizeof ("Priority(=65535/65535) less,connectCount=1234567890,client[]") - 1 + 22 * pCapture_item->p.__ctrlClientList->nodeCount) + sizeof (unsigned int)*3,
                        sizeof (FsObjectBase));
                sendData->len = 8 + sprintf(sendData->data + 8, "Priority(=%hu/%hu) less,connectCount=%lu,client[", priority, pCapture_item->p.ctrlPriority, pCapture_item->p.__ctrlClientList->nodeCount);
                {
                    unsigned int (*ppNode)[6] = ((unsigned int (*)[6])pCapture_item->p.__ctrlClientList->pNode) + pCapture_item->p.__ctrlClientList->startIndex;
                    unsigned int i = 0;
                    for (; i < pCapture_item->p.__ctrlClientList->nodeCount; i++) {
                        configManager_connect_get_addr(&ipv4, &ipv4Port, *ppNode++);
                        printf("[%u]:%hhu.%hhu.%hhu.%hhu:%hu\n", i, Fs_Network_Ipv4_print(ipv4), ipv4Port);
                        sendData->len += sprintf(sendData->data + sendData->len, "%hhu.%hhu.%hhu.%hhu:%hu,", Fs_Network_Ipv4_print(ipv4), ipv4Port);
                    }
                }
                pthread_mutex_unlock(&pCapture_item->p.__ctrlClientList->mutex);
                sendData->data[sendData->len - 1] = ']';
                *((unsigned int *) sendData->data) = (head & 0xFFFFFFF0U) | 0x6U, *((unsigned int *) (sendData->data + 4)) = sendData->len - 8;
                memcpy(sendData->data + Memery_Alignment(sendData->len), requestID_3, sizeof (unsigned int)*3);
                configManager_send_pthreadSafety__OI_2(pCapture_item->ro._pCapture->ro._pConfigManager, sendData);
                return -1;
            }
            pthread_mutex_unlock(&pCapture_item->p.__ctrlClientList->mutex);
#endif
            struct FsEbml_node* pEbml_node = fs_Ebml_node_get_first(pEbml, (struct FsEbml_node*) pEbml, "speed");
            if (NULL == pEbml_node || 0 == pEbml_node->data.lenth) {
                FsPrintf(1, "Invalid speedy,data:\n");
                fs_Ebml_out_debug(pEbml, stdout, pShareBuffer), printf("\n");
#ifndef __capture_P_cmd_cb_ctrl_data_check_error 
                pthread_mutex_lock(&pCapture_item->p.__ctrlClientList->mutex);
                if (fs_StructList_remove_order_custom(pCapture_item->p.__ctrlClientList, requestID_3, sizeof (unsigned int)*3)>-1)
                    configManager_connect_release(pCapture_item->ro._pCapture->ro._pConfigManager, requestID_3, 0);
                if (0 == pCapture_item->p.__ctrlClientList->nodeCount && pCapture_item->p.ctrlPriority != 0) {
                    pCapture_item->ro._ctrl_function->cancelKey_pthreadSafety(pCapture_item->ro.__pCamera_item, pCapture_item->ro.__pCamera_ctrl_item, pCapture_item, pObjectBaseBuffer, pShareBuffer);
                    pCapture_item->p.ctrlPriority = 0;
                }
                pthread_mutex_unlock(&pCapture_item->p.__ctrlClientList->mutex);
                return -1;
#endif
            }
            short speed = (pEbml_node->type & 0x7) == FsEbmlNodeType_Integer ? *((long long*) pEbml_node->data.buffer) : atoi(pEbml_node->data.buffer);
            pEbml_node = fs_Ebml_node_get_first(pEbml, (struct FsEbml_node*) pEbml, "time");
            if (NULL == pEbml_node || 0 == pEbml_node->data.lenth) {
                FsPrintf(1, "Invalid y,data:\n");
                fs_Ebml_out_debug(pEbml, stdout, pShareBuffer), printf("\n");
#ifndef __capture_P_cmd_cb_ctrl_data_check_error 
                pthread_mutex_lock(&pCapture_item->p.__ctrlClientList->mutex);
                if (fs_StructList_remove_order_custom(pCapture_item->p.__ctrlClientList, requestID_3, sizeof (unsigned int)*3)>-1)
                    configManager_connect_release(pCapture_item->ro._pCapture->ro._pConfigManager, requestID_3, 0);
                if (0 == pCapture_item->p.__ctrlClientList->nodeCount && pCapture_item->p.ctrlPriority != 0) {
                    pCapture_item->ro._ctrl_function->cancelKey_pthreadSafety(pCapture_item->ro.__pCamera_item, pCapture_item->ro.__pCamera_ctrl_item, pCapture_item, pObjectBaseBuffer, pShareBuffer);
                    pCapture_item->p.ctrlPriority = 0;
                }
                pthread_mutex_unlock(&pCapture_item->p.__ctrlClientList->mutex);
                return -1;
#endif
            }
            float time = (pEbml_node->type & 0x7) == FsEbmlNodeType_Float ? *((double*) pEbml_node->data.buffer) : atof(pEbml_node->data.buffer);
            if (pCapture_item->ro._ctrl_function->focus_pthreadSafety(pCapture_item->ro.__pCamera_item, pCapture_item->ro.__pCamera_ctrl_item, pCapture_item, priority, speed, time, pObjectBaseBuffer, pShareBuffer) != 1) {
#ifndef __capture_P_cmd_cb_ctrl_cb_failed 
                Capture_itemLog(FsLogType_error, "Ctrl camera failed,data:\n");
                fs_Ebml_out_debug(pEbml, stdout, pShareBuffer), printf("\n");
                pthread_mutex_lock(&pCapture_item->p.__ctrlClientList->mutex);
                if (fs_StructList_remove_order_custom(pCapture_item->p.__ctrlClientList, requestID_3, sizeof (unsigned int)*3)>-1)
                    configManager_connect_release(pCapture_item->ro._pCapture->ro._pConfigManager, requestID_3, 0);
                if (0 == pCapture_item->p.__ctrlClientList->nodeCount && pCapture_item->p.ctrlPriority != 0) {
                    pCapture_item->ro._ctrl_function->cancelKey_pthreadSafety(pCapture_item->ro.__pCamera_item, pCapture_item->ro.__pCamera_ctrl_item, pCapture_item, pObjectBaseBuffer, pShareBuffer);
                    pCapture_item->p.ctrlPriority = 0;
                }
                pthread_mutex_unlock(&pCapture_item->p.__ctrlClientList->mutex);
                FsObjectBase * const sendData = (FsObjectBase*) fsMalloc(sizeof (FsObjectBase) + 8 + Memery_Alignment(sizeof ("Cb failed.\n") - 1) + sizeof (unsigned int)*3);
                fs_ObjectBase_init(sendData, sizeof (FsObjectBase) + 8 + Memery_Alignment(sizeof ("Cb failed.\n") - 1) + sizeof (unsigned int)*3, sizeof (FsObjectBase));
                sendData->len = 8 + sizeof ("Cb failed.\n") - 1;
                *((unsigned int *) sendData->data) = (head & 0xFFFFFFF0U) | 0x6U, *((unsigned int *) (sendData->data + 4)) = sendData->len - 8;
                memcpy(sendData->data + 8, "Cb failed.\n", sizeof ("Cb failed.\n") - 1);
                memcpy(sendData->data + Memery_Alignment(sendData->len), requestID_3, sizeof (unsigned int)*3);
                configManager_send_pthreadSafety__OI_2(pCapture_item->ro._pCapture->ro._pConfigManager, sendData);
                return -1;
#endif
            }
            ////////////////////////////////////////////////////////////////////////
            /* 发回执数据 */
#ifndef __capture_P_cmd_cb_ctrl_cb_ok 
            configManager_conncet_refer_send_buffer(FsStringLenData("ok"), requestID_3, (head & 0xFFFFFFF0U) | 0x2U, pObjectBaseBuffer);
            if (0 == priority) {
                if (pCapture_item->p.ctrlPriority != 0) {
                    pCapture_item->p.ctrlPriority = 0;
                    if (1 == pCapture_item->p.__ctrlClientList->nodeCount) {
                        configManager_connect_release(pCapture_item->ro._pCapture->ro._pConfigManager, requestID_3, 0);
                        fs_StructList_clear(pCapture_item->p.__ctrlClientList);
                    }
                }
                return 1;
            } else {
                if (pCapture_item->p.ctrlPriority != priority) {
                    pCapture_item->p.ctrlPriority = priority;
                    capture_private_item_ctrlClient_release(pCapture_item, requestID_3);
                }
                unsigned int data[6] = {requestID_3[0], requestID_3[1], requestID_3[2], head, (unsigned int) requestDataType, 0};
                if (fs_StructList_insert_order_custom_update_pthreadSafety(pCapture_item->p.__ctrlClientList, data, sizeof (unsigned int)*3) < 0) return 1;
                else return -128;
            }
#endif
#endif
        } else if (_rv < 0)goto FsMacrosFunctionTag(FsMacrosFunction(focus_lt));
        else goto FsMacrosFunctionTag(FsMacrosFunction(focus_gt));
    }
#endif
    ////////////////////////////////////////////////////////////////////////////
#if FsMacrosFunction(preset_set) > 0 // 设置预置位
    FsMacrosFunctionTag(FsMacrosFunction(preset_set)) :;
    {
        const int _rv = strcmp(ctrl_type, "preset_set");
        if (0 == _rv) {
#if 1
#ifndef __capture_P_cmd_cb_ctrl_priority_check 
            const unsigned short priority = fs_Ebml_node_get_first_Integer(pEbml, (struct FsEbml_node*) pEbml, "priority", 0);
            pthread_mutex_lock(&pCapture_item->p.__ctrlClientList->mutex);
            configManager_conncet_refer_check(pCapture_item->ro._pCapture->ro._pConfigManager, pCapture_item->p.__ctrlClientList);
            if ((priority & 0xFFFE) < pCapture_item->p.ctrlPriority && (pCapture_item->p.__ctrlClientList->nodeCount > 1 || (1 == pCapture_item->p.__ctrlClientList->nodeCount &&
                    memcmp(requestID_3, pCapture_item->p.__ctrlClientList->pNode + pCapture_item->p.__ctrlClientList->startIndex * sizeof (unsigned int)*6, sizeof (unsigned int)*3) != 0))) {
                /* 连接优先级不够 */
                if (fs_StructList_remove_order_custom(pCapture_item->p.__ctrlClientList, requestID_3, sizeof (unsigned int)*3)>-1)
                    configManager_connect_release(pCapture_item->ro._pCapture->ro._pConfigManager, requestID_3, 0);
                unsigned int ipv4;
                unsigned short ipv4Port;
                configManager_connect_get_addr(&ipv4, &ipv4Port, requestID_3);
                FsPrintf2(1, "Priority(=%hu/%hu) less,client:%p,%x:%hhu.%hhu.%hhu.%hhu:%hu,connectCount=%lu,client:\n", priority, pCapture_item->p.ctrlPriority, *(void**) requestID_3, requestID_3[2]
                        , Fs_Network_Ipv4_print(ipv4), ipv4Port, pCapture_item->p.__ctrlClientList->nodeCount);
                FsObjectBase * const sendData = (FsObjectBase*) fsMalloc(sizeof (FsObjectBase) +
                        8 + Memery_Alignment(sizeof ("Priority(=65535/65535) less,connectCount=1234567890,client[]") - 1 + 22 * pCapture_item->p.__ctrlClientList->nodeCount) + sizeof (unsigned int)*3);
                fs_ObjectBase_init(sendData, sizeof (FsObjectBase) +
                        8 + Memery_Alignment(sizeof ("Priority(=65535/65535) less,connectCount=1234567890,client[]") - 1 + 22 * pCapture_item->p.__ctrlClientList->nodeCount) + sizeof (unsigned int)*3,
                        sizeof (FsObjectBase));
                sendData->len = 8 + sprintf(sendData->data + 8, "Priority(=%hu/%hu) less,connectCount=%lu,client[", priority, pCapture_item->p.ctrlPriority, pCapture_item->p.__ctrlClientList->nodeCount);
                {
                    unsigned int (*ppNode)[6] = ((unsigned int (*)[6])pCapture_item->p.__ctrlClientList->pNode) + pCapture_item->p.__ctrlClientList->startIndex;
                    unsigned int i = 0;
                    for (; i < pCapture_item->p.__ctrlClientList->nodeCount; i++) {
                        configManager_connect_get_addr(&ipv4, &ipv4Port, *ppNode++);
                        printf("[%u]:%hhu.%hhu.%hhu.%hhu:%hu\n", i, Fs_Network_Ipv4_print(ipv4), ipv4Port);
                        sendData->len += sprintf(sendData->data + sendData->len, "%hhu.%hhu.%hhu.%hhu:%hu,", Fs_Network_Ipv4_print(ipv4), ipv4Port);
                    }
                }
                pthread_mutex_unlock(&pCapture_item->p.__ctrlClientList->mutex);
                sendData->data[sendData->len - 1] = ']';
                *((unsigned int *) sendData->data) = (head & 0xFFFFFFF0U) | 0x6U, *((unsigned int *) (sendData->data + 4)) = sendData->len - 8;
                memcpy(sendData->data + Memery_Alignment(sendData->len), requestID_3, sizeof (unsigned int)*3);
                configManager_send_pthreadSafety__OI_2(pCapture_item->ro._pCapture->ro._pConfigManager, sendData);
                return -1;
            }
            pthread_mutex_unlock(&pCapture_item->p.__ctrlClientList->mutex);
#endif
            const short presetIndex = fs_Ebml_node_get_first_Integer(pEbml, (struct FsEbml_node*) pEbml, "presetIndex", -1);
            if (presetIndex < 0) {
                Capture_itemLog(FsLogType_error, "Invalid presetIndex(=%hd),data:\n", presetIndex);
                fs_Ebml_out_debug(pEbml, stdout, pShareBuffer), printf("\n");
#ifndef __capture_P_cmd_cb_ctrl_data_check_error 
                pthread_mutex_lock(&pCapture_item->p.__ctrlClientList->mutex);
                if (fs_StructList_remove_order_custom(pCapture_item->p.__ctrlClientList, requestID_3, sizeof (unsigned int)*3)>-1)
                    configManager_connect_release(pCapture_item->ro._pCapture->ro._pConfigManager, requestID_3, 0);
                if (0 == pCapture_item->p.__ctrlClientList->nodeCount && pCapture_item->p.ctrlPriority != 0) {
                    pCapture_item->ro._ctrl_function->cancelKey_pthreadSafety(pCapture_item->ro.__pCamera_item, pCapture_item->ro.__pCamera_ctrl_item, pCapture_item, pObjectBaseBuffer, pShareBuffer);
                    pCapture_item->p.ctrlPriority = 0;
                }
                pthread_mutex_unlock(&pCapture_item->p.__ctrlClientList->mutex);
                return -1;
#endif
            }
            if (pCapture_item->ro._ctrl_function->preset_set_pthreadSafety(pCapture_item->ro.__pCamera_item, pCapture_item->ro.__pCamera_ctrl_item, pCapture_item, priority, presetIndex, pObjectBaseBuffer, pShareBuffer) != 1) {
#ifndef __capture_P_cmd_cb_ctrl_cb_failed 
                Capture_itemLog(FsLogType_error, "Ctrl camera failed,data:\n");
                fs_Ebml_out_debug(pEbml, stdout, pShareBuffer), printf("\n");
                pthread_mutex_lock(&pCapture_item->p.__ctrlClientList->mutex);
                if (fs_StructList_remove_order_custom(pCapture_item->p.__ctrlClientList, requestID_3, sizeof (unsigned int)*3)>-1)
                    configManager_connect_release(pCapture_item->ro._pCapture->ro._pConfigManager, requestID_3, 0);
                if (0 == pCapture_item->p.__ctrlClientList->nodeCount && pCapture_item->p.ctrlPriority != 0) {
                    pCapture_item->ro._ctrl_function->cancelKey_pthreadSafety(pCapture_item->ro.__pCamera_item, pCapture_item->ro.__pCamera_ctrl_item, pCapture_item, pObjectBaseBuffer, pShareBuffer);
                    pCapture_item->p.ctrlPriority = 0;
                }
                pthread_mutex_unlock(&pCapture_item->p.__ctrlClientList->mutex);
                FsObjectBase * const sendData = (FsObjectBase*) fsMalloc(sizeof (FsObjectBase) + 8 + Memery_Alignment(sizeof ("Cb failed.\n") - 1) + sizeof (unsigned int)*3);
                fs_ObjectBase_init(sendData, sizeof (FsObjectBase) + 8 + Memery_Alignment(sizeof ("Cb failed.\n") - 1) + sizeof (unsigned int)*3, sizeof (FsObjectBase));
                sendData->len = 8 + sizeof ("Cb failed.\n") - 1;
                *((unsigned int *) sendData->data) = (head & 0xFFFFFFF0U) | 0x6U, *((unsigned int *) (sendData->data + 4)) = sendData->len - 8;
                memcpy(sendData->data + 8, "Cb failed.\n", sizeof ("Cb failed.\n") - 1);
                memcpy(sendData->data + Memery_Alignment(sendData->len), requestID_3, sizeof (unsigned int)*3);
                configManager_send_pthreadSafety__OI_2(pCapture_item->ro._pCapture->ro._pConfigManager, sendData);
                return -1;
#endif
            }
            ////////////////////////////////////////////////////////////////////////
            /* 发回执数据 */
#ifndef __capture_P_cmd_cb_ctrl_cb_ok 
            configManager_conncet_refer_send_buffer(FsStringLenData("ok"), requestID_3, (head & 0xFFFFFFF0U) | 0x2U, pObjectBaseBuffer);
            if (0 == priority) {
                if (pCapture_item->p.ctrlPriority != 0) {
                    pCapture_item->p.ctrlPriority = 0;
                    if (1 == pCapture_item->p.__ctrlClientList->nodeCount) {
                        configManager_connect_release(pCapture_item->ro._pCapture->ro._pConfigManager, requestID_3, 0);
                        fs_StructList_clear(pCapture_item->p.__ctrlClientList);
                    }
                }
                return 1;
            } else {
                if (pCapture_item->p.ctrlPriority != priority) {
                    pCapture_item->p.ctrlPriority = priority;
                    capture_private_item_ctrlClient_release(pCapture_item, requestID_3);
                }
                unsigned int data[6] = {requestID_3[0], requestID_3[1], requestID_3[2], head, (unsigned int) requestDataType, 0};
                if (fs_StructList_insert_order_custom_update_pthreadSafety(pCapture_item->p.__ctrlClientList, data, sizeof (unsigned int)*3) < 0) return 1;
                else return -128;
            }
#endif
#endif
        } else if (_rv < 0)goto FsMacrosFunctionTag(FsMacrosFunction(preset_set_lt));
        else goto FsMacrosFunctionTag(FsMacrosFunction(preset_set_gt));
    }
#endif    
    ////////////////////////////////////////////////////////////////////////////
#if FsMacrosFunction(preset_goto) > 0 // 调用预置位
    FsMacrosFunctionTag(FsMacrosFunction(preset_goto)) :;
    {
        const int _rv = strcmp(ctrl_type, "preset_goto");
        if (0 == _rv) {
#if 1
#ifndef __capture_P_cmd_cb_ctrl_priority_check 
            const unsigned short priority = fs_Ebml_node_get_first_Integer(pEbml, (struct FsEbml_node*) pEbml, "priority", 0);
            pthread_mutex_lock(&pCapture_item->p.__ctrlClientList->mutex);
            configManager_conncet_refer_check(pCapture_item->ro._pCapture->ro._pConfigManager, pCapture_item->p.__ctrlClientList);
            if ((priority & 0xFFFE) < pCapture_item->p.ctrlPriority && (pCapture_item->p.__ctrlClientList->nodeCount > 1 || (1 == pCapture_item->p.__ctrlClientList->nodeCount &&
                    memcmp(requestID_3, pCapture_item->p.__ctrlClientList->pNode + pCapture_item->p.__ctrlClientList->startIndex * sizeof (unsigned int)*6, sizeof (unsigned int)*3) != 0))) {
                /* 连接优先级不够 */
                if (fs_StructList_remove_order_custom(pCapture_item->p.__ctrlClientList, requestID_3, sizeof (unsigned int)*3)>-1)
                    configManager_connect_release(pCapture_item->ro._pCapture->ro._pConfigManager, requestID_3, 0);
                unsigned int ipv4;
                unsigned short ipv4Port;
                configManager_connect_get_addr(&ipv4, &ipv4Port, requestID_3);
                FsPrintf2(1, "Priority(=%hu/%hu) less,client:%p,%x:%hhu.%hhu.%hhu.%hhu:%hu,connectCount=%lu,client:\n", priority, pCapture_item->p.ctrlPriority, *(void**) requestID_3, requestID_3[2]
                        , Fs_Network_Ipv4_print(ipv4), ipv4Port, pCapture_item->p.__ctrlClientList->nodeCount);
                FsObjectBase * const sendData = (FsObjectBase*) fsMalloc(sizeof (FsObjectBase) +
                        8 + Memery_Alignment(sizeof ("Priority(=65535/65535) less,connectCount=1234567890,client[]") - 1 + 22 * pCapture_item->p.__ctrlClientList->nodeCount) + sizeof (unsigned int)*3);
                fs_ObjectBase_init(sendData, sizeof (FsObjectBase) +
                        8 + Memery_Alignment(sizeof ("Priority(=65535/65535) less,connectCount=1234567890,client[]") - 1 + 22 * pCapture_item->p.__ctrlClientList->nodeCount) + sizeof (unsigned int)*3,
                        sizeof (FsObjectBase));
                sendData->len = 8 + sprintf(sendData->data + 8, "Priority(=%hu/%hu) less,connectCount=%lu,client[", priority, pCapture_item->p.ctrlPriority, pCapture_item->p.__ctrlClientList->nodeCount);
                {
                    unsigned int (*ppNode)[6] = ((unsigned int (*)[6])pCapture_item->p.__ctrlClientList->pNode) + pCapture_item->p.__ctrlClientList->startIndex;
                    unsigned int i = 0;
                    for (; i < pCapture_item->p.__ctrlClientList->nodeCount; i++) {
                        configManager_connect_get_addr(&ipv4, &ipv4Port, *ppNode++);
                        printf("[%u]:%hhu.%hhu.%hhu.%hhu:%hu\n", i, Fs_Network_Ipv4_print(ipv4), ipv4Port);
                        sendData->len += sprintf(sendData->data + sendData->len, "%hhu.%hhu.%hhu.%hhu:%hu,", Fs_Network_Ipv4_print(ipv4), ipv4Port);
                    }
                }
                pthread_mutex_unlock(&pCapture_item->p.__ctrlClientList->mutex);
                sendData->data[sendData->len - 1] = ']';
                *((unsigned int *) sendData->data) = (head & 0xFFFFFFF0U) | 0x6U, *((unsigned int *) (sendData->data + 4)) = sendData->len - 8;
                memcpy(sendData->data + Memery_Alignment(sendData->len), requestID_3, sizeof (unsigned int)*3);
                configManager_send_pthreadSafety__OI_2(pCapture_item->ro._pCapture->ro._pConfigManager, sendData);
                return -1;
            }
            pthread_mutex_unlock(&pCapture_item->p.__ctrlClientList->mutex);
#endif
            struct FsEbml_node* pEbml_node = fs_Ebml_node_get_first(pEbml, (struct FsEbml_node*) pEbml, "presetIndex");
            if (NULL == pEbml_node || 0 == pEbml_node->data.lenth) {
                FsPrintf(1, "Invalid speedy,data:\n");
                fs_Ebml_out_debug(pEbml, stdout, pShareBuffer), printf("\n");
#ifndef __capture_P_cmd_cb_ctrl_data_check_error 
                pthread_mutex_lock(&pCapture_item->p.__ctrlClientList->mutex);
                if (fs_StructList_remove_order_custom(pCapture_item->p.__ctrlClientList, requestID_3, sizeof (unsigned int)*3)>-1)
                    configManager_connect_release(pCapture_item->ro._pCapture->ro._pConfigManager, requestID_3, 0);
                if (0 == pCapture_item->p.__ctrlClientList->nodeCount && pCapture_item->p.ctrlPriority != 0) {
                    pCapture_item->ro._ctrl_function->cancelKey_pthreadSafety(pCapture_item->ro.__pCamera_item, pCapture_item->ro.__pCamera_ctrl_item, pCapture_item, pObjectBaseBuffer, pShareBuffer);
                    pCapture_item->p.ctrlPriority = 0;
                }
                pthread_mutex_unlock(&pCapture_item->p.__ctrlClientList->mutex);
                return -1;
#endif
            }
            short presetIndex = (pEbml_node->type & 0x7) == FsEbmlNodeType_Integer ? *((long long*) pEbml_node->data.buffer) : atoi(pEbml_node->data.buffer);
            if (pCapture_item->ro._ctrl_function->preset_goto_pthreadSafety(pCapture_item->ro.__pCamera_item, pCapture_item->ro.__pCamera_ctrl_item, pCapture_item, priority, presetIndex, pObjectBaseBuffer, pShareBuffer) != 1) {
#ifndef __capture_P_cmd_cb_ctrl_cb_failed 
                Capture_itemLog(FsLogType_error, "Ctrl camera failed,data:\n");
                fs_Ebml_out_debug(pEbml, stdout, pShareBuffer), printf("\n");
                pthread_mutex_lock(&pCapture_item->p.__ctrlClientList->mutex);
                if (fs_StructList_remove_order_custom(pCapture_item->p.__ctrlClientList, requestID_3, sizeof (unsigned int)*3)>-1)
                    configManager_connect_release(pCapture_item->ro._pCapture->ro._pConfigManager, requestID_3, 0);
                if (0 == pCapture_item->p.__ctrlClientList->nodeCount && pCapture_item->p.ctrlPriority != 0) {
                    pCapture_item->ro._ctrl_function->cancelKey_pthreadSafety(pCapture_item->ro.__pCamera_item, pCapture_item->ro.__pCamera_ctrl_item, pCapture_item, pObjectBaseBuffer, pShareBuffer);
                    pCapture_item->p.ctrlPriority = 0;
                }
                pthread_mutex_unlock(&pCapture_item->p.__ctrlClientList->mutex);
                FsObjectBase * const sendData = (FsObjectBase*) fsMalloc(sizeof (FsObjectBase) + 8 + Memery_Alignment(sizeof ("Cb failed.\n") - 1) + sizeof (unsigned int)*3);
                fs_ObjectBase_init(sendData, sizeof (FsObjectBase) + 8 + Memery_Alignment(sizeof ("Cb failed.\n") - 1) + sizeof (unsigned int)*3, sizeof (FsObjectBase));
                sendData->len = 8 + sizeof ("Cb failed.\n") - 1;
                *((unsigned int *) sendData->data) = (head & 0xFFFFFFF0U) | 0x6U, *((unsigned int *) (sendData->data + 4)) = sendData->len - 8;
                memcpy(sendData->data + 8, "Cb failed.\n", sizeof ("Cb failed.\n") - 1);
                memcpy(sendData->data + Memery_Alignment(sendData->len), requestID_3, sizeof (unsigned int)*3);
                configManager_send_pthreadSafety__OI_2(pCapture_item->ro._pCapture->ro._pConfigManager, sendData);
                return -1;
#endif
            }
            ////////////////////////////////////////////////////////////////////////
            /* 发回执数据 */
#ifndef __capture_P_cmd_cb_ctrl_cb_ok 
            configManager_conncet_refer_send_buffer(FsStringLenData("ok"), requestID_3, (head & 0xFFFFFFF0U) | 0x2U, pObjectBaseBuffer);
            if (0 == priority) {
                if (pCapture_item->p.ctrlPriority != 0) {
                    pCapture_item->p.ctrlPriority = 0;
                    if (1 == pCapture_item->p.__ctrlClientList->nodeCount) {
                        configManager_connect_release(pCapture_item->ro._pCapture->ro._pConfigManager, requestID_3, 0);
                        fs_StructList_clear(pCapture_item->p.__ctrlClientList);
                    }
                }
                return 1;
            } else {
                if (pCapture_item->p.ctrlPriority != priority) {
                    pCapture_item->p.ctrlPriority = priority;
                    capture_private_item_ctrlClient_release(pCapture_item, requestID_3);
                }
                unsigned int data[6] = {requestID_3[0], requestID_3[1], requestID_3[2], head, (unsigned int) requestDataType, 0};
                if (fs_StructList_insert_order_custom_update_pthreadSafety(pCapture_item->p.__ctrlClientList, data, sizeof (unsigned int)*3) < 0) return 1;
                else return -128;
            }
#endif
#endif
        } else if (_rv < 0)goto FsMacrosFunctionTag(FsMacrosFunction(preset_goto_lt));
        else goto FsMacrosFunctionTag(FsMacrosFunction(preset_goto_gt));
    }
#endif    
    ////////////////////////////////////////////////////////////////////////////
#if FsMacrosFunction(preset_clean) > 0 // 设置预置位
    FsMacrosFunctionTag(FsMacrosFunction(preset_clean)) :;
    {
        const int _rv = strcmp(ctrl_type, "preset_clean");
        if (0 == _rv) {
#if 1
#ifndef __capture_P_cmd_cb_ctrl_priority_check 
            const unsigned short priority = fs_Ebml_node_get_first_Integer(pEbml, (struct FsEbml_node*) pEbml, "priority", 0);
            pthread_mutex_lock(&pCapture_item->p.__ctrlClientList->mutex);
            configManager_conncet_refer_check(pCapture_item->ro._pCapture->ro._pConfigManager, pCapture_item->p.__ctrlClientList);
            if ((priority & 0xFFFE) < pCapture_item->p.ctrlPriority && (pCapture_item->p.__ctrlClientList->nodeCount > 1 || (1 == pCapture_item->p.__ctrlClientList->nodeCount &&
                    memcmp(requestID_3, pCapture_item->p.__ctrlClientList->pNode + pCapture_item->p.__ctrlClientList->startIndex * sizeof (unsigned int)*6, sizeof (unsigned int)*3) != 0))) {
                /* 连接优先级不够 */
                if (fs_StructList_remove_order_custom(pCapture_item->p.__ctrlClientList, requestID_3, sizeof (unsigned int)*3)>-1)
                    configManager_connect_release(pCapture_item->ro._pCapture->ro._pConfigManager, requestID_3, 0);
                unsigned int ipv4;
                unsigned short ipv4Port;
                configManager_connect_get_addr(&ipv4, &ipv4Port, requestID_3);
                FsPrintf2(1, "Priority(=%hu/%hu) less,client:%p,%x:%hhu.%hhu.%hhu.%hhu:%hu,connectCount=%lu,client:\n", priority, pCapture_item->p.ctrlPriority, *(void**) requestID_3, requestID_3[2]
                        , Fs_Network_Ipv4_print(ipv4), ipv4Port, pCapture_item->p.__ctrlClientList->nodeCount);
                FsObjectBase * const sendData = (FsObjectBase*) fsMalloc(sizeof (FsObjectBase) +
                        8 + Memery_Alignment(sizeof ("Priority(=65535/65535) less,connectCount=1234567890,client[]") - 1 + 22 * pCapture_item->p.__ctrlClientList->nodeCount) + sizeof (unsigned int)*3);
                fs_ObjectBase_init(sendData, sizeof (FsObjectBase) +
                        8 + Memery_Alignment(sizeof ("Priority(=65535/65535) less,connectCount=1234567890,client[]") - 1 + 22 * pCapture_item->p.__ctrlClientList->nodeCount) + sizeof (unsigned int)*3,
                        sizeof (FsObjectBase));
                sendData->len = 8 + sprintf(sendData->data + 8, "Priority(=%hu/%hu) less,connectCount=%lu,client[", priority, pCapture_item->p.ctrlPriority, pCapture_item->p.__ctrlClientList->nodeCount);
                {
                    unsigned int (*ppNode)[6] = ((unsigned int (*)[6])pCapture_item->p.__ctrlClientList->pNode) + pCapture_item->p.__ctrlClientList->startIndex;
                    unsigned int i = 0;
                    for (; i < pCapture_item->p.__ctrlClientList->nodeCount; i++) {
                        configManager_connect_get_addr(&ipv4, &ipv4Port, *ppNode++);
                        printf("[%u]:%hhu.%hhu.%hhu.%hhu:%hu\n", i, Fs_Network_Ipv4_print(ipv4), ipv4Port);
                        sendData->len += sprintf(sendData->data + sendData->len, "%hhu.%hhu.%hhu.%hhu:%hu,", Fs_Network_Ipv4_print(ipv4), ipv4Port);
                    }
                }
                pthread_mutex_unlock(&pCapture_item->p.__ctrlClientList->mutex);
                sendData->data[sendData->len - 1] = ']';
                *((unsigned int *) sendData->data) = (head & 0xFFFFFFF0U) | 0x6U, *((unsigned int *) (sendData->data + 4)) = sendData->len - 8;
                memcpy(sendData->data + Memery_Alignment(sendData->len), requestID_3, sizeof (unsigned int)*3);
                configManager_send_pthreadSafety__OI_2(pCapture_item->ro._pCapture->ro._pConfigManager, sendData);
                return -1;
            }
            pthread_mutex_unlock(&pCapture_item->p.__ctrlClientList->mutex);
#endif
            struct FsEbml_node* pEbml_node = fs_Ebml_node_get_first(pEbml, (struct FsEbml_node*) pEbml, "presetIndex");
            if (NULL == pEbml_node || 0 == pEbml_node->data.lenth) {
                FsPrintf(1, "Invalid speedy,data:\n");
                fs_Ebml_out_debug(pEbml, stdout, pShareBuffer), printf("\n");
#ifndef __capture_P_cmd_cb_ctrl_data_check_error 
                pthread_mutex_lock(&pCapture_item->p.__ctrlClientList->mutex);
                if (fs_StructList_remove_order_custom(pCapture_item->p.__ctrlClientList, requestID_3, sizeof (unsigned int)*3)>-1)
                    configManager_connect_release(pCapture_item->ro._pCapture->ro._pConfigManager, requestID_3, 0);
                if (0 == pCapture_item->p.__ctrlClientList->nodeCount && pCapture_item->p.ctrlPriority != 0) {
                    pCapture_item->ro._ctrl_function->cancelKey_pthreadSafety(pCapture_item->ro.__pCamera_item, pCapture_item->ro.__pCamera_ctrl_item, pCapture_item, pObjectBaseBuffer, pShareBuffer);
                    pCapture_item->p.ctrlPriority = 0;
                }
                pthread_mutex_unlock(&pCapture_item->p.__ctrlClientList->mutex);
                return -1;
#endif
            }
            short presetIndex = (pEbml_node->type & 0x7) == FsEbmlNodeType_Integer ? *((long long*) pEbml_node->data.buffer) : atoi(pEbml_node->data.buffer);
            if (pCapture_item->ro._ctrl_function->preset_clean_pthreadSafety(pCapture_item->ro.__pCamera_item, pCapture_item->ro.__pCamera_ctrl_item, pCapture_item, priority, presetIndex, pObjectBaseBuffer, pShareBuffer) != 1) {
#ifndef __capture_P_cmd_cb_ctrl_cb_failed 
                Capture_itemLog(FsLogType_error, "Ctrl camera failed,data:\n");
                fs_Ebml_out_debug(pEbml, stdout, pShareBuffer), printf("\n");
                pthread_mutex_lock(&pCapture_item->p.__ctrlClientList->mutex);
                if (fs_StructList_remove_order_custom(pCapture_item->p.__ctrlClientList, requestID_3, sizeof (unsigned int)*3)>-1)
                    configManager_connect_release(pCapture_item->ro._pCapture->ro._pConfigManager, requestID_3, 0);
                if (0 == pCapture_item->p.__ctrlClientList->nodeCount && pCapture_item->p.ctrlPriority != 0) {
                    pCapture_item->ro._ctrl_function->cancelKey_pthreadSafety(pCapture_item->ro.__pCamera_item, pCapture_item->ro.__pCamera_ctrl_item, pCapture_item, pObjectBaseBuffer, pShareBuffer);
                    pCapture_item->p.ctrlPriority = 0;
                }
                pthread_mutex_unlock(&pCapture_item->p.__ctrlClientList->mutex);
                FsObjectBase * const sendData = (FsObjectBase*) fsMalloc(sizeof (FsObjectBase) + 8 + Memery_Alignment(sizeof ("Cb failed.\n") - 1) + sizeof (unsigned int)*3);
                fs_ObjectBase_init(sendData, sizeof (FsObjectBase) + 8 + Memery_Alignment(sizeof ("Cb failed.\n") - 1) + sizeof (unsigned int)*3, sizeof (FsObjectBase));
                sendData->len = 8 + sizeof ("Cb failed.\n") - 1;
                *((unsigned int *) sendData->data) = (head & 0xFFFFFFF0U) | 0x6U, *((unsigned int *) (sendData->data + 4)) = sendData->len - 8;
                memcpy(sendData->data + 8, "Cb failed.\n", sizeof ("Cb failed.\n") - 1);
                memcpy(sendData->data + Memery_Alignment(sendData->len), requestID_3, sizeof (unsigned int)*3);
                configManager_send_pthreadSafety__OI_2(pCapture_item->ro._pCapture->ro._pConfigManager, sendData);
                return -1;
#endif
            }
            ////////////////////////////////////////////////////////////////////////
            /* 发回执数据 */
#ifndef __capture_P_cmd_cb_ctrl_cb_ok 
            configManager_conncet_refer_send_buffer(FsStringLenData("ok"), requestID_3, (head & 0xFFFFFFF0U) | 0x2U, pObjectBaseBuffer);
            if (0 == priority) {
                if (pCapture_item->p.ctrlPriority != 0) {
                    pCapture_item->p.ctrlPriority = 0;
                    if (1 == pCapture_item->p.__ctrlClientList->nodeCount) {
                        configManager_connect_release(pCapture_item->ro._pCapture->ro._pConfigManager, requestID_3, 0);
                        fs_StructList_clear(pCapture_item->p.__ctrlClientList);
                    }
                }
                return 1;
            } else {
                if (pCapture_item->p.ctrlPriority != priority) {
                    pCapture_item->p.ctrlPriority = priority;
                    capture_private_item_ctrlClient_release(pCapture_item, requestID_3);
                }
                unsigned int data[6] = {requestID_3[0], requestID_3[1], requestID_3[2], head, (unsigned int) requestDataType, 0};
                if (fs_StructList_insert_order_custom_update_pthreadSafety(pCapture_item->p.__ctrlClientList, data, sizeof (unsigned int)*3) < 0) return 1;
                else return -128;
            }
#endif
#endif
        } else if (_rv < 0)goto FsMacrosFunctionTag(FsMacrosFunction(preset_clean_lt));
        else goto FsMacrosFunctionTag(FsMacrosFunction(preset_clean_gt));
    }
#endif    
    ////////////////////////////////////////////////////////////////////////////
#if FsMacrosFunction(zoom3D) > 0 // 3D定位
    FsMacrosFunctionTag(FsMacrosFunction(zoom3D)) :;
    {
        const int _rv = strcmp(ctrl_type, "zoom3D");
        if (0 == _rv) {
#if 1
#ifndef __capture_P_cmd_cb_ctrl_priority_check 
            const unsigned short priority = fs_Ebml_node_get_first_Integer(pEbml, (struct FsEbml_node*) pEbml, "priority", 0);
            pthread_mutex_lock(&pCapture_item->p.__ctrlClientList->mutex);
            configManager_conncet_refer_check(pCapture_item->ro._pCapture->ro._pConfigManager, pCapture_item->p.__ctrlClientList);
            if ((priority & 0xFFFE) < pCapture_item->p.ctrlPriority && (pCapture_item->p.__ctrlClientList->nodeCount > 1 || (1 == pCapture_item->p.__ctrlClientList->nodeCount &&
                    memcmp(requestID_3, pCapture_item->p.__ctrlClientList->pNode + pCapture_item->p.__ctrlClientList->startIndex * sizeof (unsigned int)*6, sizeof (unsigned int)*3) != 0))) {
                /* 连接优先级不够 */
                if (fs_StructList_remove_order_custom(pCapture_item->p.__ctrlClientList, requestID_3, sizeof (unsigned int)*3)>-1)
                    configManager_connect_release(pCapture_item->ro._pCapture->ro._pConfigManager, requestID_3, 0);
                unsigned int ipv4;
                unsigned short ipv4Port;
                configManager_connect_get_addr(&ipv4, &ipv4Port, requestID_3);
                FsPrintf2(1, "Priority(=%hu/%hu) less,client:%p,%x:%hhu.%hhu.%hhu.%hhu:%hu,connectCount=%lu,client:\n", priority, pCapture_item->p.ctrlPriority, *(void**) requestID_3, requestID_3[2]
                        , Fs_Network_Ipv4_print(ipv4), ipv4Port, pCapture_item->p.__ctrlClientList->nodeCount);
                FsObjectBase * const sendData = (FsObjectBase*) fsMalloc(sizeof (FsObjectBase) +
                        8 + Memery_Alignment(sizeof ("Priority(=65535/65535) less,connectCount=1234567890,client[]") - 1 + 22 * pCapture_item->p.__ctrlClientList->nodeCount) + sizeof (unsigned int)*3);
                fs_ObjectBase_init(sendData, sizeof (FsObjectBase) +
                        8 + Memery_Alignment(sizeof ("Priority(=65535/65535) less,connectCount=1234567890,client[]") - 1 + 22 * pCapture_item->p.__ctrlClientList->nodeCount) + sizeof (unsigned int)*3,
                        sizeof (FsObjectBase));
                sendData->len = 8 + sprintf(sendData->data + 8, "Priority(=%hu/%hu) less,connectCount=%lu,client[", priority, pCapture_item->p.ctrlPriority, pCapture_item->p.__ctrlClientList->nodeCount);
                {
                    unsigned int (*ppNode)[6] = ((unsigned int (*)[6])pCapture_item->p.__ctrlClientList->pNode) + pCapture_item->p.__ctrlClientList->startIndex;
                    unsigned int i = 0;
                    for (; i < pCapture_item->p.__ctrlClientList->nodeCount; i++) {
                        configManager_connect_get_addr(&ipv4, &ipv4Port, *ppNode++);
                        printf("[%u]:%hhu.%hhu.%hhu.%hhu:%hu\n", i, Fs_Network_Ipv4_print(ipv4), ipv4Port);
                        sendData->len += sprintf(sendData->data + sendData->len, "%hhu.%hhu.%hhu.%hhu:%hu,", Fs_Network_Ipv4_print(ipv4), ipv4Port);
                    }
                }
                pthread_mutex_unlock(&pCapture_item->p.__ctrlClientList->mutex);
                sendData->data[sendData->len - 1] = ']';
                *((unsigned int *) sendData->data) = (head & 0xFFFFFFF0U) | 0x6U, *((unsigned int *) (sendData->data + 4)) = sendData->len - 8;
                memcpy(sendData->data + Memery_Alignment(sendData->len), requestID_3, sizeof (unsigned int)*3);
                configManager_send_pthreadSafety__OI_2(pCapture_item->ro._pCapture->ro._pConfigManager, sendData);
                return -1;
            }
            pthread_mutex_unlock(&pCapture_item->p.__ctrlClientList->mutex);
#endif
            struct FsEbml_node* pEbml_node = fs_Ebml_node_get_first(pEbml, (struct FsEbml_node*) pEbml, "x1");
            if (NULL == pEbml_node || 0 == pEbml_node->data.lenth) {
                FsPrintf(1, "Invalid speedy,data:\n");
                fs_Ebml_out_debug(pEbml, stdout, pShareBuffer), printf("\n");
#ifndef __capture_P_cmd_cb_ctrl_data_check_error 
                pthread_mutex_lock(&pCapture_item->p.__ctrlClientList->mutex);
                if (fs_StructList_remove_order_custom(pCapture_item->p.__ctrlClientList, requestID_3, sizeof (unsigned int)*3)>-1)
                    configManager_connect_release(pCapture_item->ro._pCapture->ro._pConfigManager, requestID_3, 0);
                if (0 == pCapture_item->p.__ctrlClientList->nodeCount && pCapture_item->p.ctrlPriority != 0) {
                    pCapture_item->ro._ctrl_function->cancelKey_pthreadSafety(pCapture_item->ro.__pCamera_item, pCapture_item->ro.__pCamera_ctrl_item, pCapture_item, pObjectBaseBuffer, pShareBuffer);
                    pCapture_item->p.ctrlPriority = 0;
                }
                pthread_mutex_unlock(&pCapture_item->p.__ctrlClientList->mutex);
                return -1;
#endif
            }
            short x1 = (pEbml_node->type & 0x7) == FsEbmlNodeType_Integer ? *((long long*) pEbml_node->data.buffer) : atoi(pEbml_node->data.buffer);
            pEbml_node = fs_Ebml_node_get_first(pEbml, (struct FsEbml_node*) pEbml, "y1");
            if (NULL == pEbml_node || 0 == pEbml_node->data.lenth) {
                FsPrintf(1, "Invalid speedy,data:\n");
                fs_Ebml_out_debug(pEbml, stdout, pShareBuffer), printf("\n");
#ifndef __capture_P_cmd_cb_ctrl_data_check_error 
                pthread_mutex_lock(&pCapture_item->p.__ctrlClientList->mutex);
                if (fs_StructList_remove_order_custom(pCapture_item->p.__ctrlClientList, requestID_3, sizeof (unsigned int)*3)>-1)
                    configManager_connect_release(pCapture_item->ro._pCapture->ro._pConfigManager, requestID_3, 0);
                if (0 == pCapture_item->p.__ctrlClientList->nodeCount && pCapture_item->p.ctrlPriority != 0) {
                    pCapture_item->ro._ctrl_function->cancelKey_pthreadSafety(pCapture_item->ro.__pCamera_item, pCapture_item->ro.__pCamera_ctrl_item, pCapture_item, pObjectBaseBuffer, pShareBuffer);
                    pCapture_item->p.ctrlPriority = 0;
                }
                pthread_mutex_unlock(&pCapture_item->p.__ctrlClientList->mutex);
                return -1;
#endif
            }
            short y1 = (pEbml_node->type & 0x7) == FsEbmlNodeType_Integer ? *((long long*) pEbml_node->data.buffer) : atoi(pEbml_node->data.buffer);
            pEbml_node = fs_Ebml_node_get_first(pEbml, (struct FsEbml_node*) pEbml, "x2");
            if (NULL == pEbml_node || 0 == pEbml_node->data.lenth) {
                FsPrintf(1, "Invalid speedy,data:\n");
                fs_Ebml_out_debug(pEbml, stdout, pShareBuffer), printf("\n");
#ifndef __capture_P_cmd_cb_ctrl_data_check_error 
                pthread_mutex_lock(&pCapture_item->p.__ctrlClientList->mutex);
                if (fs_StructList_remove_order_custom(pCapture_item->p.__ctrlClientList, requestID_3, sizeof (unsigned int)*3)>-1)
                    configManager_connect_release(pCapture_item->ro._pCapture->ro._pConfigManager, requestID_3, 0);
                if (0 == pCapture_item->p.__ctrlClientList->nodeCount && pCapture_item->p.ctrlPriority != 0) {
                    pCapture_item->ro._ctrl_function->cancelKey_pthreadSafety(pCapture_item->ro.__pCamera_item, pCapture_item->ro.__pCamera_ctrl_item, pCapture_item, pObjectBaseBuffer, pShareBuffer);
                    pCapture_item->p.ctrlPriority = 0;
                }
                pthread_mutex_unlock(&pCapture_item->p.__ctrlClientList->mutex);
                return -1;
#endif
            }
            short x2 = (pEbml_node->type & 0x7) == FsEbmlNodeType_Integer ? *((long long*) pEbml_node->data.buffer) : atoi(pEbml_node->data.buffer);
            pEbml_node = fs_Ebml_node_get_first(pEbml, (struct FsEbml_node*) pEbml, "y2");
            if (NULL == pEbml_node || 0 == pEbml_node->data.lenth) {
                FsPrintf(1, "Invalid speedy,data:\n");
                fs_Ebml_out_debug(pEbml, stdout, pShareBuffer), printf("\n");
#ifndef __capture_P_cmd_cb_ctrl_data_check_error 
                pthread_mutex_lock(&pCapture_item->p.__ctrlClientList->mutex);
                if (fs_StructList_remove_order_custom(pCapture_item->p.__ctrlClientList, requestID_3, sizeof (unsigned int)*3)>-1)
                    configManager_connect_release(pCapture_item->ro._pCapture->ro._pConfigManager, requestID_3, 0);
                if (0 == pCapture_item->p.__ctrlClientList->nodeCount && pCapture_item->p.ctrlPriority != 0) {
                    pCapture_item->ro._ctrl_function->cancelKey_pthreadSafety(pCapture_item->ro.__pCamera_item, pCapture_item->ro.__pCamera_ctrl_item, pCapture_item, pObjectBaseBuffer, pShareBuffer);
                    pCapture_item->p.ctrlPriority = 0;
                }
                pthread_mutex_unlock(&pCapture_item->p.__ctrlClientList->mutex);
                return -1;
#endif
            }
            short y2 = (pEbml_node->type & 0x7) == FsEbmlNodeType_Integer ? *((long long*) pEbml_node->data.buffer) : atoi(pEbml_node->data.buffer);
            if (pCapture_item->ro._ctrl_function->zoom3D_pthreadSafety(pCapture_item->ro.__pCamera_item, pCapture_item->ro.__pCamera_ctrl_item, pCapture_item, priority, x1, y1, x2, y2, pObjectBaseBuffer, pShareBuffer) != 1) {
#ifndef __capture_P_cmd_cb_ctrl_cb_failed 
                Capture_itemLog(FsLogType_error, "Ctrl camera failed,data:\n");
                fs_Ebml_out_debug(pEbml, stdout, pShareBuffer), printf("\n");
                pthread_mutex_lock(&pCapture_item->p.__ctrlClientList->mutex);
                if (fs_StructList_remove_order_custom(pCapture_item->p.__ctrlClientList, requestID_3, sizeof (unsigned int)*3)>-1)
                    configManager_connect_release(pCapture_item->ro._pCapture->ro._pConfigManager, requestID_3, 0);
                if (0 == pCapture_item->p.__ctrlClientList->nodeCount && pCapture_item->p.ctrlPriority != 0) {
                    pCapture_item->ro._ctrl_function->cancelKey_pthreadSafety(pCapture_item->ro.__pCamera_item, pCapture_item->ro.__pCamera_ctrl_item, pCapture_item, pObjectBaseBuffer, pShareBuffer);
                    pCapture_item->p.ctrlPriority = 0;
                }
                pthread_mutex_unlock(&pCapture_item->p.__ctrlClientList->mutex);
                FsObjectBase * const sendData = (FsObjectBase*) fsMalloc(sizeof (FsObjectBase) + 8 + Memery_Alignment(sizeof ("Cb failed.\n") - 1) + sizeof (unsigned int)*3);
                fs_ObjectBase_init(sendData, sizeof (FsObjectBase) + 8 + Memery_Alignment(sizeof ("Cb failed.\n") - 1) + sizeof (unsigned int)*3, sizeof (FsObjectBase));
                sendData->len = 8 + sizeof ("Cb failed.\n") - 1;
                *((unsigned int *) sendData->data) = (head & 0xFFFFFFF0U) | 0x6U, *((unsigned int *) (sendData->data + 4)) = sendData->len - 8;
                memcpy(sendData->data + 8, "Cb failed.\n", sizeof ("Cb failed.\n") - 1);
                memcpy(sendData->data + Memery_Alignment(sendData->len), requestID_3, sizeof (unsigned int)*3);
                configManager_send_pthreadSafety__OI_2(pCapture_item->ro._pCapture->ro._pConfigManager, sendData);
                return -1;
#endif
            }
            ////////////////////////////////////////////////////////////////////////
            /* 发回执数据 */
#ifndef __capture_P_cmd_cb_ctrl_cb_ok 
            configManager_conncet_refer_send_buffer(FsStringLenData("ok"), requestID_3, (head & 0xFFFFFFF0U) | 0x2U, pObjectBaseBuffer);
            if (0 == priority) {
                if (pCapture_item->p.ctrlPriority != 0) {
                    pCapture_item->p.ctrlPriority = 0;
                    if (1 == pCapture_item->p.__ctrlClientList->nodeCount) {
                        configManager_connect_release(pCapture_item->ro._pCapture->ro._pConfigManager, requestID_3, 0);
                        fs_StructList_clear(pCapture_item->p.__ctrlClientList);
                    }
                }
                return 1;
            } else {
                if (pCapture_item->p.ctrlPriority != priority) {
                    pCapture_item->p.ctrlPriority = priority;
                    capture_private_item_ctrlClient_release(pCapture_item, requestID_3);
                }
                unsigned int data[6] = {requestID_3[0], requestID_3[1], requestID_3[2], head, (unsigned int) requestDataType, 0};
                if (fs_StructList_insert_order_custom_update_pthreadSafety(pCapture_item->p.__ctrlClientList, data, sizeof (unsigned int)*3) < 0) return 1;
                else return -128;
            }
#endif
#endif
        } else if (_rv < 0)goto FsMacrosFunctionTag(FsMacrosFunction(zoom3D_lt));
        else goto FsMacrosFunctionTag(FsMacrosFunction(zoom3D_gt));
    }
#endif    
    ////////////////////////////////////////////////////////////////////////////
#if FsMacrosFunction(ptzgoto) > 0 // GOTO PTZ
    FsMacrosFunctionTag(FsMacrosFunction(ptzgoto)) :;
    {
        const int _rv = strcmp(ctrl_type, "ptzgoto");
        if (0 == _rv) {
#if 1
#ifndef __capture_P_cmd_cb_ctrl_priority_check 
            const unsigned short priority = fs_Ebml_node_get_first_Integer(pEbml, (struct FsEbml_node*) pEbml, "priority", 0);
            pthread_mutex_lock(&pCapture_item->p.__ctrlClientList->mutex);
            configManager_conncet_refer_check(pCapture_item->ro._pCapture->ro._pConfigManager, pCapture_item->p.__ctrlClientList);
            if ((priority & 0xFFFE) < pCapture_item->p.ctrlPriority && (pCapture_item->p.__ctrlClientList->nodeCount > 1 || (1 == pCapture_item->p.__ctrlClientList->nodeCount &&
                    memcmp(requestID_3, pCapture_item->p.__ctrlClientList->pNode + pCapture_item->p.__ctrlClientList->startIndex * sizeof (unsigned int)*6, sizeof (unsigned int)*3) != 0))) {
                /* 连接优先级不够 */
                if (fs_StructList_remove_order_custom(pCapture_item->p.__ctrlClientList, requestID_3, sizeof (unsigned int)*3)>-1)
                    configManager_connect_release(pCapture_item->ro._pCapture->ro._pConfigManager, requestID_3, 0);
                unsigned int ipv4;
                unsigned short ipv4Port;
                configManager_connect_get_addr(&ipv4, &ipv4Port, requestID_3);
                FsPrintf2(1, "Priority(=%hu/%hu) less,client:%p,%x:%hhu.%hhu.%hhu.%hhu:%hu,connectCount=%lu,client:\n", priority, pCapture_item->p.ctrlPriority, *(void**) requestID_3, requestID_3[2]
                        , Fs_Network_Ipv4_print(ipv4), ipv4Port, pCapture_item->p.__ctrlClientList->nodeCount);
                FsObjectBase * const sendData = (FsObjectBase*) fsMalloc(sizeof (FsObjectBase) +
                        8 + Memery_Alignment(sizeof ("Priority(=65535/65535) less,connectCount=1234567890,client[]") - 1 + 22 * pCapture_item->p.__ctrlClientList->nodeCount) + sizeof (unsigned int)*3);
                fs_ObjectBase_init(sendData, sizeof (FsObjectBase) +
                        8 + Memery_Alignment(sizeof ("Priority(=65535/65535) less,connectCount=1234567890,client[]") - 1 + 22 * pCapture_item->p.__ctrlClientList->nodeCount) + sizeof (unsigned int)*3,
                        sizeof (FsObjectBase));
                sendData->len = 8 + sprintf(sendData->data + 8, "Priority(=%hu/%hu) less,connectCount=%lu,client[", priority, pCapture_item->p.ctrlPriority, pCapture_item->p.__ctrlClientList->nodeCount);
                {
                    unsigned int (*ppNode)[6] = ((unsigned int (*)[6])pCapture_item->p.__ctrlClientList->pNode) + pCapture_item->p.__ctrlClientList->startIndex;
                    unsigned int i = 0;
                    for (; i < pCapture_item->p.__ctrlClientList->nodeCount; i++) {
                        configManager_connect_get_addr(&ipv4, &ipv4Port, *ppNode++);
                        printf("[%u]:%hhu.%hhu.%hhu.%hhu:%hu\n", i, Fs_Network_Ipv4_print(ipv4), ipv4Port);
                        sendData->len += sprintf(sendData->data + sendData->len, "%hhu.%hhu.%hhu.%hhu:%hu,", Fs_Network_Ipv4_print(ipv4), ipv4Port);
                    }
                }
                pthread_mutex_unlock(&pCapture_item->p.__ctrlClientList->mutex);
                sendData->data[sendData->len - 1] = ']';
                *((unsigned int *) sendData->data) = (head & 0xFFFFFFF0U) | 0x6U, *((unsigned int *) (sendData->data + 4)) = sendData->len - 8;
                memcpy(sendData->data + Memery_Alignment(sendData->len), requestID_3, sizeof (unsigned int)*3);
                configManager_send_pthreadSafety__OI_2(pCapture_item->ro._pCapture->ro._pConfigManager, sendData);
                return -1;
            }
            pthread_mutex_unlock(&pCapture_item->p.__ctrlClientList->mutex);
#endif
            const short P = fs_Ebml_node_get_first_Float(pEbml, (struct FsEbml_node*) pEbml, "p", -1.0);
            const short T = fs_Ebml_node_get_first_Float(pEbml, (struct FsEbml_node*) pEbml, "t", -1.0);
            const short Z = fs_Ebml_node_get_first_Float(pEbml, (struct FsEbml_node*) pEbml, "z", -1.0);
            if (P < 0 || T < 0 || Z < 0) {
                Capture_itemLog(FsLogType_error, "Invalid P(=%hd),T(=%hd),Z(=%hd),data:\n", P, T, Z);
                if (FsLogTypeCheck(FsLogType_error)) fs_Ebml_out_debug(pEbml, stdout, pShareBuffer), printf("\n");
                fflush(stdout);
                if (fs_StructList_remove_order_custom(pCapture_item->p.__ctrlClientList, requestID_3, sizeof (unsigned int)*3)>-1)
                    configManager_connect_release(pCapture_item->ro._pCapture->ro._pConfigManager, requestID_3, 0);
                if (0 == pCapture_item->p.__ctrlClientList->nodeCount && pCapture_item->p.ctrlPriority != 0) {
                    pCapture_item->ro._ctrl_function->cancelKey_pthreadSafety(pCapture_item->ro.__pCamera_item, pCapture_item->ro.__pCamera_ctrl_item, pCapture_item, pObjectBaseBuffer, pShareBuffer);
                    pCapture_item->p.ctrlPriority = 0;
                }
                return -1;
            }
            if (pCapture_item->ro._ctrl_function->ptz_goto_pthreadSafety(pCapture_item->ro.__pCamera_item, pCapture_item->ro.__pCamera_ctrl_item, pCapture_item, priority, P, T, Z, pObjectBaseBuffer, pShareBuffer) != 1) {
#ifndef __capture_P_cmd_cb_ctrl_cb_failed 
                Capture_itemLog(FsLogType_error, "Ctrl camera failed,data:\n");
                fs_Ebml_out_debug(pEbml, stdout, pShareBuffer), printf("\n");
                pthread_mutex_lock(&pCapture_item->p.__ctrlClientList->mutex);
                if (fs_StructList_remove_order_custom(pCapture_item->p.__ctrlClientList, requestID_3, sizeof (unsigned int)*3)>-1)
                    configManager_connect_release(pCapture_item->ro._pCapture->ro._pConfigManager, requestID_3, 0);
                if (0 == pCapture_item->p.__ctrlClientList->nodeCount && pCapture_item->p.ctrlPriority != 0) {
                    pCapture_item->ro._ctrl_function->cancelKey_pthreadSafety(pCapture_item->ro.__pCamera_item, pCapture_item->ro.__pCamera_ctrl_item, pCapture_item, pObjectBaseBuffer, pShareBuffer);
                    pCapture_item->p.ctrlPriority = 0;
                }
                pthread_mutex_unlock(&pCapture_item->p.__ctrlClientList->mutex);
                FsObjectBase * const sendData = (FsObjectBase*) fsMalloc(sizeof (FsObjectBase) + 8 + Memery_Alignment(sizeof ("Cb failed.\n") - 1) + sizeof (unsigned int)*3);
                fs_ObjectBase_init(sendData, sizeof (FsObjectBase) + 8 + Memery_Alignment(sizeof ("Cb failed.\n") - 1) + sizeof (unsigned int)*3, sizeof (FsObjectBase));
                sendData->len = 8 + sizeof ("Cb failed.\n") - 1;
                *((unsigned int *) sendData->data) = (head & 0xFFFFFFF0U) | 0x6U, *((unsigned int *) (sendData->data + 4)) = sendData->len - 8;
                memcpy(sendData->data + 8, "Cb failed.\n", sizeof ("Cb failed.\n") - 1);
                memcpy(sendData->data + Memery_Alignment(sendData->len), requestID_3, sizeof (unsigned int)*3);
                configManager_send_pthreadSafety__OI_2(pCapture_item->ro._pCapture->ro._pConfigManager, sendData);
                return -1;
#endif
            }
            ////////////////////////////////////////////////////////////////////////
            /* 发回执数据 */
#ifndef __capture_P_cmd_cb_ctrl_cb_ok 
            configManager_conncet_refer_send_buffer(FsStringLenData("ok"), requestID_3, (head & 0xFFFFFFF0U) | 0x2U, pObjectBaseBuffer);
            if (0 == priority) {
                if (pCapture_item->p.ctrlPriority != 0) {
                    pCapture_item->p.ctrlPriority = 0;
                    if (1 == pCapture_item->p.__ctrlClientList->nodeCount) {
                        configManager_connect_release(pCapture_item->ro._pCapture->ro._pConfigManager, requestID_3, 0);
                        fs_StructList_clear(pCapture_item->p.__ctrlClientList);
                    }
                }
                return 1;
            } else {
                if (pCapture_item->p.ctrlPriority != priority) {
                    pCapture_item->p.ctrlPriority = priority;
                    capture_private_item_ctrlClient_release(pCapture_item, requestID_3);
                }
                unsigned int data[6] = {requestID_3[0], requestID_3[1], requestID_3[2], head, (unsigned int) requestDataType, 0};
                if (fs_StructList_insert_order_custom_update_pthreadSafety(pCapture_item->p.__ctrlClientList, data, sizeof (unsigned int)*3) < 0) return 1;
                else return -128;
            }
#endif
#endif
        } else if (_rv < 0)goto FsMacrosFunctionTag(FsMacrosFunction(ptzgoto_lt));
        else goto FsMacrosFunctionTag(FsMacrosFunction(ptzgoto_gt));
    }
#endif    
    ////////////////////////////////////////////////////////////////////////////
#if FsMacrosFunction(ptzget) > 0 // 获取PTZ
    FsMacrosFunctionTag(FsMacrosFunction(ptzget)) :;
    {
        const int _rv = strcmp(ctrl_type, "ptzget");
        if (0 == _rv) {
#if 1
#ifndef __capture_P_cmd_cb_ctrl_priority_check_no_return 
            unsigned short priority = fs_Ebml_node_get_first_Integer(pEbml, (struct FsEbml_node*) pEbml, "priority", 0);
            pthread_mutex_lock(&pCapture_item->p.__ctrlClientList->mutex);
            configManager_conncet_refer_check(pCapture_item->ro._pCapture->ro._pConfigManager, pCapture_item->p.__ctrlClientList);
            if ((priority & 0xFFFE) < pCapture_item->p.ctrlPriority && (pCapture_item->p.__ctrlClientList->nodeCount > 1 || (1 == pCapture_item->p.__ctrlClientList->nodeCount &&
                    memcmp(requestID_3, pCapture_item->p.__ctrlClientList->pNode + pCapture_item->p.__ctrlClientList->startIndex * sizeof (unsigned int)*6, sizeof (unsigned int)*3) != 0))) {
                /* 连接优先级不够 */
                if (fs_StructList_remove_order_custom(pCapture_item->p.__ctrlClientList, requestID_3, sizeof (unsigned int)*3)>-1)
                    configManager_connect_release(pCapture_item->ro._pCapture->ro._pConfigManager, requestID_3, 0);
                unsigned int ipv4;
                unsigned short ipv4Port;
                configManager_connect_get_addr(&ipv4, &ipv4Port, requestID_3);
                FsPrintf2(1, "Priority(=%hu/%hu) less,client:%p,%x:%hhu.%hhu.%hhu.%hhu:%hu,connectCount=%lu,client:\n", priority, pCapture_item->p.ctrlPriority, *(void **) requestID_3, requestID_3[2]
                        , Fs_Network_Ipv4_print(ipv4), ipv4Port, pCapture_item->p.__ctrlClientList->nodeCount);
                //            FsObjectBase * const sendData = (FsObjectBase*) fsMalloc(sizeof (FsObjectBase) +
                //                    8 + Memery_Alignment(sizeof ("Priority(=65535/65535) less,connectCount=1234567890,client[]") - 1 + 22 * pCapture_item->p.__ctrolClientList->nodeCount) + sizeof (unsigned int)*3);
                //            fs_ObjectBase_init(sendData, sizeof (FsObjectBase) +
                //                    8 + Memery_Alignment(sizeof ("Priority(=65535/65535) less,connectCount=1234567890,client[]") - 1 + 22 * pCapture_item->p.__ctrolClientList->nodeCount) + sizeof (unsigned int)*3,
                //                    sizeof (FsObjectBase));
                //            sendData->len = 8 + sprintf(sendData->data + 8, "Priority(=%hu/%hu) less,connectCount=%lu,client[", priority, pCapture_item->p.ctrlPriority, pCapture_item->p.__ctrolClientList->nodeCount);
                //            {
                //                unsigned int (*ppNode)[6] = ((unsigned int (*)[6])pCapture_item->p.__ctrolClientList->pNode) + pCapture_item->p.__ctrolClientList->startIndex;
                //                unsigned int i = 0;
                //                for (; i < pCapture_item->p.__ctrolClientList->nodeCount; i++) {
                //                    configManager_connect_get_addr(&ipv4, &ipv4Port, *ppNode++);
                //                    printf("[%u]:%hhu.%hhu.%hhu.%hhu:%hu\n", i, Fs_Network_Ipv4_print(ipv4), ipv4Port);
                //                    sendData->len += sprintf(sendData->data + sendData->len, "%hhu.%hhu.%hhu.%hhu:%hu,", Fs_Network_Ipv4_print(ipv4), ipv4Port);
                //                }
                //            }
                // pthread_mutex_unlock(&pCapture_item->p.__ctrlClientList->mutex);
                //            sendData->data[sendData->len - 1] = ']';
                //            *((unsigned int *) sendData->data) = (head & 0xFFFFFFF0U) | 0x6U, *((unsigned int *) (sendData->data + 4)) = sendData->len - 8;
                //            memcpy(sendData->data + Memery_Alignment(sendData->len), requestID_3, sizeof (unsigned int)*3);
                //            configManager_send_pthreadSafety__OI_2(pCapture_item->ro._pCapture->ro._pConfigManager, sendData);
                //            return -1;
            }
            pthread_mutex_unlock(&pCapture_item->p.__ctrlClientList->mutex);
#endif
            unsigned short p, t, z;
            if (pCapture_item->ro._ctrl_function->ptz_get_pthreadSafety(pCapture_item->ro.__pCamera_item, pCapture_item->ro.__pCamera_ctrl_item, pCapture_item, priority, &p, &t, &z, pObjectBaseBuffer, pShareBuffer) != 1) {
#ifndef __capture_P_cmd_cb_ctrl_cb_failed 
                Capture_itemLog(FsLogType_error, "Ctrl camera failed,data:\n");
                fs_Ebml_out_debug(pEbml, stdout, pShareBuffer), printf("\n");
                pthread_mutex_lock(&pCapture_item->p.__ctrlClientList->mutex);
                if (fs_StructList_remove_order_custom(pCapture_item->p.__ctrlClientList, requestID_3, sizeof (unsigned int)*3)>-1)
                    configManager_connect_release(pCapture_item->ro._pCapture->ro._pConfigManager, requestID_3, 0);
                if (0 == pCapture_item->p.__ctrlClientList->nodeCount && pCapture_item->p.ctrlPriority != 0) {
                    pCapture_item->ro._ctrl_function->cancelKey_pthreadSafety(pCapture_item->ro.__pCamera_item, pCapture_item->ro.__pCamera_ctrl_item, pCapture_item, pObjectBaseBuffer, pShareBuffer);
                    pCapture_item->p.ctrlPriority = 0;
                }
                pthread_mutex_unlock(&pCapture_item->p.__ctrlClientList->mutex);
                FsObjectBase * const sendData = (FsObjectBase*) fsMalloc(sizeof (FsObjectBase) + 8 + Memery_Alignment(sizeof ("Cb failed.\n") - 1) + sizeof (unsigned int)*3);
                fs_ObjectBase_init(sendData, sizeof (FsObjectBase) + 8 + Memery_Alignment(sizeof ("Cb failed.\n") - 1) + sizeof (unsigned int)*3, sizeof (FsObjectBase));
                sendData->len = 8 + sizeof ("Cb failed.\n") - 1;
                *((unsigned int *) sendData->data) = (head & 0xFFFFFFF0U) | 0x6U, *((unsigned int *) (sendData->data + 4)) = sendData->len - 8;
                memcpy(sendData->data + 8, "Cb failed.\n", sizeof ("Cb failed.\n") - 1);
                memcpy(sendData->data + Memery_Alignment(sendData->len), requestID_3, sizeof (unsigned int)*3);
                configManager_send_pthreadSafety__OI_2(pCapture_item->ro._pCapture->ro._pConfigManager, sendData);
                return -1;
#endif
            }
            ////////////////////////////////////////////////////////////////////////
            /* 发回执数据 */
            FsEbml *rst = fs_Ebml_new__IO(FsEbmlEncodeMethod_UTF8, 0);
            *(unsigned long long *) fs_Ebml_node_addChild_with_a_property(rst, (struct FsEbml_node*) rst, "p", FsEbmlNodeType_Integer,
                    "comment", "P值,最大为23040")->data.buffer = p;
            *(unsigned long long *) fs_Ebml_node_addChild_with_a_property(rst, (struct FsEbml_node*) rst, "t", FsEbmlNodeType_Integer,
                    "comment", "T值,最大为23040")->data.buffer = t;
            *(unsigned long long *) fs_Ebml_node_addChild_with_a_property(rst, (struct FsEbml_node*) rst, "z", FsEbmlNodeType_Integer,
                    "comment", "Z值,最大为23040")->data.buffer = z;
            struct FsEbml_node* pEbml_node = fs_Ebml_node_get_first(pEbml, (struct FsEbml_node*) pEbml, "return_type");
            if (pEbml_node && 0 < pEbml_node->data.lenth) {
                if (0 == strcmp("ebml", pEbml_node->data.buffer)) requestDataType = 1;
                else if (0 == strcmp("xml", pEbml_node->data.buffer)) requestDataType = 2;
                else if (0 == strcmp("json", pEbml_node->data.buffer)) requestDataType = 3;
            }
            configManager_send_pthreadSafety__OI_2_default(pCapture_item->ro._pCapture->ro._pConfigManager, rst, requestID_3, head, requestDataType, pShareBuffer);
            return 1;
#endif
        } else if (_rv < 0)goto FsMacrosFunctionTag(FsMacrosFunction(ptzget_lt));
        else goto FsMacrosFunctionTag(FsMacrosFunction(ptzget_gt));
    }
#endif    
    ////////////////////////////////////////////////////////////////////////////
#if FsMacrosFunction(imageinfoset) > 0 // 图像信息设置
    FsMacrosFunctionTag(FsMacrosFunction(imageinfoset)) :;
    {
        const int _rv = strcmp(ctrl_type, "imageinfoset");
        if (0 == _rv) {
#if 1
#ifndef __capture_P_cmd_cb_ctrl_priority_check 
            const unsigned short priority = fs_Ebml_node_get_first_Integer(pEbml, (struct FsEbml_node*) pEbml, "priority", 0);
            pthread_mutex_lock(&pCapture_item->p.__ctrlClientList->mutex);
            configManager_conncet_refer_check(pCapture_item->ro._pCapture->ro._pConfigManager, pCapture_item->p.__ctrlClientList);
            if ((priority & 0xFFFE) < pCapture_item->p.ctrlPriority && (pCapture_item->p.__ctrlClientList->nodeCount > 1 || (1 == pCapture_item->p.__ctrlClientList->nodeCount &&
                    memcmp(requestID_3, pCapture_item->p.__ctrlClientList->pNode + pCapture_item->p.__ctrlClientList->startIndex * sizeof (unsigned int)*6, sizeof (unsigned int)*3) != 0))) {
                /* 连接优先级不够 */
                if (fs_StructList_remove_order_custom(pCapture_item->p.__ctrlClientList, requestID_3, sizeof (unsigned int)*3)>-1)
                    configManager_connect_release(pCapture_item->ro._pCapture->ro._pConfigManager, requestID_3, 0);
                unsigned int ipv4;
                unsigned short ipv4Port;
                configManager_connect_get_addr(&ipv4, &ipv4Port, requestID_3);
                FsPrintf2(1, "Priority(=%hu/%hu) less,client:%p,%x:%hhu.%hhu.%hhu.%hhu:%hu,connectCount=%lu,client:\n", priority, pCapture_item->p.ctrlPriority, *(void**) requestID_3, requestID_3[2]
                        , Fs_Network_Ipv4_print(ipv4), ipv4Port, pCapture_item->p.__ctrlClientList->nodeCount);
                FsObjectBase * const sendData = (FsObjectBase*) fsMalloc(sizeof (FsObjectBase) +
                        8 + Memery_Alignment(sizeof ("Priority(=65535/65535) less,connectCount=1234567890,client[]") - 1 + 22 * pCapture_item->p.__ctrlClientList->nodeCount) + sizeof (unsigned int)*3);
                fs_ObjectBase_init(sendData, sizeof (FsObjectBase) +
                        8 + Memery_Alignment(sizeof ("Priority(=65535/65535) less,connectCount=1234567890,client[]") - 1 + 22 * pCapture_item->p.__ctrlClientList->nodeCount) + sizeof (unsigned int)*3,
                        sizeof (FsObjectBase));
                sendData->len = 8 + sprintf(sendData->data + 8, "Priority(=%hu/%hu) less,connectCount=%lu,client[", priority, pCapture_item->p.ctrlPriority, pCapture_item->p.__ctrlClientList->nodeCount);
                {
                    unsigned int (*ppNode)[6] = ((unsigned int (*)[6])pCapture_item->p.__ctrlClientList->pNode) + pCapture_item->p.__ctrlClientList->startIndex;
                    unsigned int i = 0;
                    for (; i < pCapture_item->p.__ctrlClientList->nodeCount; i++) {
                        configManager_connect_get_addr(&ipv4, &ipv4Port, *ppNode++);
                        printf("[%u]:%hhu.%hhu.%hhu.%hhu:%hu\n", i, Fs_Network_Ipv4_print(ipv4), ipv4Port);
                        sendData->len += sprintf(sendData->data + sendData->len, "%hhu.%hhu.%hhu.%hhu:%hu,", Fs_Network_Ipv4_print(ipv4), ipv4Port);
                    }
                }
                pthread_mutex_unlock(&pCapture_item->p.__ctrlClientList->mutex);
                sendData->data[sendData->len - 1] = ']';
                *((unsigned int *) sendData->data) = (head & 0xFFFFFFF0U) | 0x6U, *((unsigned int *) (sendData->data + 4)) = sendData->len - 8;
                memcpy(sendData->data + Memery_Alignment(sendData->len), requestID_3, sizeof (unsigned int)*3);
                configManager_send_pthreadSafety__OI_2(pCapture_item->ro._pCapture->ro._pConfigManager, sendData);
                return -1;
            }
            pthread_mutex_unlock(&pCapture_item->p.__ctrlClientList->mutex);
#endif
#ifndef __ctrl_read_imageInfo 
            struct Camera_ctrl_imageInfo imageInfo;
            unsigned int setMask = 0;
            {
                const int autoFocus = fs_Ebml_node_get_first_Integer(pEbml, (struct FsEbml_node*) pEbml, "autoFocus", -1);
                if (autoFocus >= 0) {
                    imageInfo.autoFocus = autoFocus;
                    setMask |= Camera_ctrl_imageInfo_mask_autoFocus;
                }
            }
            {
                const int controlMode = fs_Ebml_node_get_first_Integer(pEbml, (struct FsEbml_node*) pEbml, "controlMode", -1);
                if (controlMode >= 0) {
                    imageInfo.controlMode = controlMode;
                    setMask |= Camera_ctrl_imageInfo_mask_controlMode;
                    if (controlMode > 0) {
                        {
                            const int shutter = fs_Ebml_node_get_first_Integer(pEbml, (struct FsEbml_node*) pEbml, "shutter", -1);
                            if (shutter >= 0) {
                                imageInfo.shutter = shutter;
                                setMask |= Camera_ctrl_imageInfo_mask_shutter;
                            }
                        }
                        {
                            const int gain = fs_Ebml_node_get_first_Integer(pEbml, (struct FsEbml_node*) pEbml, "gain", -1);
                            if (gain >= 0) {
                                imageInfo.gain = gain > 100 ? 100 : gain;
                                setMask |= Camera_ctrl_imageInfo_mask_gain;
                            }
                        }
                        {
                            const int iris = fs_Ebml_node_get_first_Integer(pEbml, (struct FsEbml_node*) pEbml, "iris", -1);
                            if (iris >= 0) {
                                imageInfo.iris = iris > 100 ? 100 : iris;
                                setMask |= Camera_ctrl_imageInfo_mask_iris;
                            }
                        }
                    }
                }
            }
            {
                const int whiteBalanceMode = fs_Ebml_node_get_first_Integer(pEbml, (struct FsEbml_node*) pEbml, "whiteBalanceMode", -1);
                if (whiteBalanceMode >= 0) {
                    imageInfo.whiteBalanceMode = whiteBalanceMode;
                    setMask |= Camera_ctrl_imageInfo_mask_whiteBalanceMode;
                    if (whiteBalanceMode > 0) {
                        {
                            const int whiteBalance_B = fs_Ebml_node_get_first_Integer(pEbml, (struct FsEbml_node*) pEbml, "whiteBalance_B", -1);
                            if (whiteBalance_B >= 0) {
                                imageInfo.whiteBalance_B = whiteBalance_B;
                                setMask |= Camera_ctrl_imageInfo_mask_whiteBalance_B;
                            }
                        }
                        {
                            const int whiteBalance_R = fs_Ebml_node_get_first_Integer(pEbml, (struct FsEbml_node*) pEbml, "whiteBalance_R", -1);
                            if (whiteBalance_R >= 0) {
                                imageInfo.whiteBalance_R = whiteBalance_R;
                                setMask |= Camera_ctrl_imageInfo_mask_whiteBalance_R;
                            }
                        }
                    }
                }
            }
            {
                const int imageMode = fs_Ebml_node_get_first_Integer(pEbml, (struct FsEbml_node*) pEbml, "imageMode", -2);
                if (imageMode >= -1) {
                    imageInfo.imageMode = imageMode;
                    setMask |= Camera_ctrl_imageInfo_mask_imageMode;
                    if (imageMode > 0) {
                        {
                            const int brightness = fs_Ebml_node_get_first_Integer(pEbml, (struct FsEbml_node*) pEbml, "brightness", -1);
                            if (brightness >= 0) {
                                imageInfo.brightness = brightness > 100 ? 100 : brightness;
                                setMask |= Camera_ctrl_imageInfo_mask_brightness;
                            }
                        }
                        {
                            const int colorSaturation = fs_Ebml_node_get_first_Integer(pEbml, (struct FsEbml_node*) pEbml, "colorSaturation", -1);
                            if (colorSaturation >= 0) {
                                imageInfo.colorSaturation = colorSaturation > 100 ? 100 : colorSaturation;
                                setMask |= Camera_ctrl_imageInfo_mask_colorSaturation;
                            }
                        }
                        {
                            const int contrast = fs_Ebml_node_get_first_Integer(pEbml, (struct FsEbml_node*) pEbml, "contrast", -1);
                            if (contrast >= 0) {
                                imageInfo.contrast = contrast > 100 ? 100 : contrast;
                                setMask |= Camera_ctrl_imageInfo_mask_contrast;
                            }
                        }
                        {
                            const int sharpness = fs_Ebml_node_get_first_Integer(pEbml, (struct FsEbml_node*) pEbml, "sharpness", -1);
                            if (sharpness >= 0) {
                                imageInfo.sharpness = sharpness > 100 ? 100 : sharpness;
                                setMask |= Camera_ctrl_imageInfo_mask_sharpness;
                            }
                        }
                    }
                }
            }
            {
                const int intensityAdjust = fs_Ebml_node_get_first_Integer(pEbml, (struct FsEbml_node*) pEbml, "intensityAdjust", -1);
                if (intensityAdjust >= 0) {
                    imageInfo.intensityAdjust = intensityAdjust > 100 ? 100 : intensityAdjust;
                    setMask |= Camera_ctrl_imageInfo_mask_intensityAdjust;
                }
            }
            {
                const int bwValid = fs_Ebml_node_get_first_Integer(pEbml, (struct FsEbml_node*) pEbml, "bwValid", -1);
                if (1 == bwValid) {
                    const int bw = fs_Ebml_node_get_first_Integer(pEbml, (struct FsEbml_node*) pEbml, "bw", -1);
                    if (bw >= 0 && bw <= 1) {
                        imageInfo.bwValid = 1;
                        imageInfo.bw = bw;
                        setMask |= Camera_ctrl_imageInfo_mask_bw;
                    }
                }
            }
            {
                int cutLeft = fs_Ebml_node_get_first_Integer(pEbml, (struct FsEbml_node*) pEbml, "cutLeft", -2);
                int cutRight = fs_Ebml_node_get_first_Integer(pEbml, (struct FsEbml_node*) pEbml, "cutRight", -2);
                if ((cutLeft >= -1 && cutLeft <= 32767) || (cutRight >= -1 && cutRight <= 32767)) {
                    if (cutLeft < -1 || cutLeft > 32767)cutLeft = -1;
                    if (cutRight < -1 || cutRight > 32767)cutRight = -1;
                    imageInfo.cutLeft = cutLeft;
                    imageInfo.cutRight = cutRight;
                    setMask |= Camera_ctrl_imageInfo_mask_cut;
                }
            }
            {
                const int cycleMove = fs_Ebml_node_get_first_Integer(pEbml, (struct FsEbml_node*) pEbml, "cycleMove", -1);
                if (cycleMove >= 0 && cycleMove <= 1) {
                    imageInfo.cycleMove = cycleMove;
                    setMask |= Camera_ctrl_imageInfo_mask_cycleMove;
                }
            }
#endif
            if (pCapture_item->ro._ctrl_function->imageInfo_set_pthreadSafety(pCapture_item->ro.__pCamera_item, pCapture_item->ro.__pCamera_ctrl_item, pCapture_item, priority, setMask, &imageInfo, pObjectBaseBuffer, pShareBuffer) != 1) {
#ifndef __capture_P_cmd_cb_ctrl_cb_failed 
                Capture_itemLog(FsLogType_error, "Ctrl camera failed,data:\n");
                fs_Ebml_out_debug(pEbml, stdout, pShareBuffer), printf("\n");
                pthread_mutex_lock(&pCapture_item->p.__ctrlClientList->mutex);
                if (fs_StructList_remove_order_custom(pCapture_item->p.__ctrlClientList, requestID_3, sizeof (unsigned int)*3)>-1)
                    configManager_connect_release(pCapture_item->ro._pCapture->ro._pConfigManager, requestID_3, 0);
                if (0 == pCapture_item->p.__ctrlClientList->nodeCount && pCapture_item->p.ctrlPriority != 0) {
                    pCapture_item->ro._ctrl_function->cancelKey_pthreadSafety(pCapture_item->ro.__pCamera_item, pCapture_item->ro.__pCamera_ctrl_item, pCapture_item, pObjectBaseBuffer, pShareBuffer);
                    pCapture_item->p.ctrlPriority = 0;
                }
                pthread_mutex_unlock(&pCapture_item->p.__ctrlClientList->mutex);
                FsObjectBase * const sendData = (FsObjectBase*) fsMalloc(sizeof (FsObjectBase) + 8 + Memery_Alignment(sizeof ("Cb failed.\n") - 1) + sizeof (unsigned int)*3);
                fs_ObjectBase_init(sendData, sizeof (FsObjectBase) + 8 + Memery_Alignment(sizeof ("Cb failed.\n") - 1) + sizeof (unsigned int)*3, sizeof (FsObjectBase));
                sendData->len = 8 + sizeof ("Cb failed.\n") - 1;
                *((unsigned int *) sendData->data) = (head & 0xFFFFFFF0U) | 0x6U, *((unsigned int *) (sendData->data + 4)) = sendData->len - 8;
                memcpy(sendData->data + 8, "Cb failed.\n", sizeof ("Cb failed.\n") - 1);
                memcpy(sendData->data + Memery_Alignment(sendData->len), requestID_3, sizeof (unsigned int)*3);
                configManager_send_pthreadSafety__OI_2(pCapture_item->ro._pCapture->ro._pConfigManager, sendData);
                return -1;
#endif
            }
            ////////////////////////////////////////////////////////////////////////
            /* 发回执数据 */
#ifndef __capture_P_cmd_cb_ctrl_cb_ok 
            configManager_conncet_refer_send_buffer(FsStringLenData("ok"), requestID_3, (head & 0xFFFFFFF0U) | 0x2U, pObjectBaseBuffer);
            if (0 == priority) {
                if (pCapture_item->p.ctrlPriority != 0) {
                    pCapture_item->p.ctrlPriority = 0;
                    if (1 == pCapture_item->p.__ctrlClientList->nodeCount) {
                        configManager_connect_release(pCapture_item->ro._pCapture->ro._pConfigManager, requestID_3, 0);
                        fs_StructList_clear(pCapture_item->p.__ctrlClientList);
                    }
                }
                return 1;
            } else {
                if (pCapture_item->p.ctrlPriority != priority) {
                    pCapture_item->p.ctrlPriority = priority;
                    capture_private_item_ctrlClient_release(pCapture_item, requestID_3);
                }
                unsigned int data[6] = {requestID_3[0], requestID_3[1], requestID_3[2], head, (unsigned int) requestDataType, 0};
                if (fs_StructList_insert_order_custom_update_pthreadSafety(pCapture_item->p.__ctrlClientList, data, sizeof (unsigned int)*3) < 0) return 1;
                else return -128;
            }
#endif
#endif
        } else if (_rv < 0)goto FsMacrosFunctionTag(FsMacrosFunction(imageinfoset_lt));
        else goto FsMacrosFunctionTag(FsMacrosFunction(imageinfoset_gt));
    }
#endif    
    ////////////////////////////////////////////////////////////////////////////
#if FsMacrosFunction(imageinfoget) > 0 // 图像信息获取
    FsMacrosFunctionTag(FsMacrosFunction(imageinfoget)) :;
    {
        const int _rv = strcmp(ctrl_type, "imageinfoget");
        if (0 == _rv) {
#if 1
#ifndef __capture_P_cmd_cb_ctrl_priority_check_no_return 
            unsigned short priority = fs_Ebml_node_get_first_Integer(pEbml, (struct FsEbml_node*) pEbml, "priority", 0);
            pthread_mutex_lock(&pCapture_item->p.__ctrlClientList->mutex);
            configManager_conncet_refer_check(pCapture_item->ro._pCapture->ro._pConfigManager, pCapture_item->p.__ctrlClientList);
            if ((priority & 0xFFFE) < pCapture_item->p.ctrlPriority && (pCapture_item->p.__ctrlClientList->nodeCount > 1 || (1 == pCapture_item->p.__ctrlClientList->nodeCount &&
                    memcmp(requestID_3, pCapture_item->p.__ctrlClientList->pNode + pCapture_item->p.__ctrlClientList->startIndex * sizeof (unsigned int)*6, sizeof (unsigned int)*3) != 0))) {
                /* 连接优先级不够 */
                if (fs_StructList_remove_order_custom(pCapture_item->p.__ctrlClientList, requestID_3, sizeof (unsigned int)*3)>-1)
                    configManager_connect_release(pCapture_item->ro._pCapture->ro._pConfigManager, requestID_3, 0);
                unsigned int ipv4;
                unsigned short ipv4Port;
                configManager_connect_get_addr(&ipv4, &ipv4Port, requestID_3);
                FsPrintf2(1, "Priority(=%hu/%hu) less,client:%p,%x:%hhu.%hhu.%hhu.%hhu:%hu,connectCount=%lu,client:\n", priority, pCapture_item->p.ctrlPriority, *(void **) requestID_3, requestID_3[2]
                        , Fs_Network_Ipv4_print(ipv4), ipv4Port, pCapture_item->p.__ctrlClientList->nodeCount);
                //            FsObjectBase * const sendData = (FsObjectBase*) fsMalloc(sizeof (FsObjectBase) +
                //                    8 + Memery_Alignment(sizeof ("Priority(=65535/65535) less,connectCount=1234567890,client[]") - 1 + 22 * pCapture_item->p.__ctrolClientList->nodeCount) + sizeof (unsigned int)*3);
                //            fs_ObjectBase_init(sendData, sizeof (FsObjectBase) +
                //                    8 + Memery_Alignment(sizeof ("Priority(=65535/65535) less,connectCount=1234567890,client[]") - 1 + 22 * pCapture_item->p.__ctrolClientList->nodeCount) + sizeof (unsigned int)*3,
                //                    sizeof (FsObjectBase));
                //            sendData->len = 8 + sprintf(sendData->data + 8, "Priority(=%hu/%hu) less,connectCount=%lu,client[", priority, pCapture_item->p.ctrlPriority, pCapture_item->p.__ctrolClientList->nodeCount);
                //            {
                //                unsigned int (*ppNode)[6] = ((unsigned int (*)[6])pCapture_item->p.__ctrolClientList->pNode) + pCapture_item->p.__ctrolClientList->startIndex;
                //                unsigned int i = 0;
                //                for (; i < pCapture_item->p.__ctrolClientList->nodeCount; i++) {
                //                    configManager_connect_get_addr(&ipv4, &ipv4Port, *ppNode++);
                //                    printf("[%u]:%hhu.%hhu.%hhu.%hhu:%hu\n", i, Fs_Network_Ipv4_print(ipv4), ipv4Port);
                //                    sendData->len += sprintf(sendData->data + sendData->len, "%hhu.%hhu.%hhu.%hhu:%hu,", Fs_Network_Ipv4_print(ipv4), ipv4Port);
                //                }
                //            }
                // pthread_mutex_unlock(&pCapture_item->p.__ctrlClientList->mutex);
                //            sendData->data[sendData->len - 1] = ']';
                //            *((unsigned int *) sendData->data) = (head & 0xFFFFFFF0U) | 0x6U, *((unsigned int *) (sendData->data + 4)) = sendData->len - 8;
                //            memcpy(sendData->data + Memery_Alignment(sendData->len), requestID_3, sizeof (unsigned int)*3);
                //            configManager_send_pthreadSafety__OI_2(pCapture_item->ro._pCapture->ro._pConfigManager, sendData);
                //            return -1;
            }
            pthread_mutex_unlock(&pCapture_item->p.__ctrlClientList->mutex);
#endif
            struct Camera_ctrl_imageInfo imageInfo;
            if (pCapture_item->ro._ctrl_function->imageInfo_get_pthreadSafety(pCapture_item->ro.__pCamera_item, pCapture_item->ro.__pCamera_ctrl_item, pCapture_item, priority, &imageInfo, pObjectBaseBuffer, pShareBuffer) != 1) {
#ifndef __capture_P_cmd_cb_ctrl_cb_failed 
                Capture_itemLog(FsLogType_error, "Ctrl camera failed,data:\n");
                fs_Ebml_out_debug(pEbml, stdout, pShareBuffer), printf("\n");
                pthread_mutex_lock(&pCapture_item->p.__ctrlClientList->mutex);
                if (fs_StructList_remove_order_custom(pCapture_item->p.__ctrlClientList, requestID_3, sizeof (unsigned int)*3)>-1)
                    configManager_connect_release(pCapture_item->ro._pCapture->ro._pConfigManager, requestID_3, 0);
                if (0 == pCapture_item->p.__ctrlClientList->nodeCount && pCapture_item->p.ctrlPriority != 0) {
                    pCapture_item->ro._ctrl_function->cancelKey_pthreadSafety(pCapture_item->ro.__pCamera_item, pCapture_item->ro.__pCamera_ctrl_item, pCapture_item, pObjectBaseBuffer, pShareBuffer);
                    pCapture_item->p.ctrlPriority = 0;
                }
                pthread_mutex_unlock(&pCapture_item->p.__ctrlClientList->mutex);
                FsObjectBase * const sendData = (FsObjectBase*) fsMalloc(sizeof (FsObjectBase) + 8 + Memery_Alignment(sizeof ("Cb failed.\n") - 1) + sizeof (unsigned int)*3);
                fs_ObjectBase_init(sendData, sizeof (FsObjectBase) + 8 + Memery_Alignment(sizeof ("Cb failed.\n") - 1) + sizeof (unsigned int)*3, sizeof (FsObjectBase));
                sendData->len = 8 + sizeof ("Cb failed.\n") - 1;
                *((unsigned int *) sendData->data) = (head & 0xFFFFFFF0U) | 0x6U, *((unsigned int *) (sendData->data + 4)) = sendData->len - 8;
                memcpy(sendData->data + 8, "Cb failed.\n", sizeof ("Cb failed.\n") - 1);
                memcpy(sendData->data + Memery_Alignment(sendData->len), requestID_3, sizeof (unsigned int)*3);
                configManager_send_pthreadSafety__OI_2(pCapture_item->ro._pCapture->ro._pConfigManager, sendData);
                return -1;
#endif
            }
            ////////////////////////////////////////////////////////////////////////
            /* 发回执数据 */
            FsEbml *rst = fs_Ebml_new__IO(FsEbmlEncodeMethod_UTF8, 0);
            *(long long *) fs_Ebml_node_addChild_with_a_property(rst, (struct FsEbml_node*) rst, "autoFocus", FsEbmlNodeType_Integer
                    , "comment", "自动聚焦,0-自动控制,1-手动控制")->data.buffer = imageInfo.autoFocus;
            *(long long *) fs_Ebml_node_addChild_with_a_property(rst, (struct FsEbml_node*) rst, "controlMode", FsEbmlNodeType_Integer
                    , "comment", "相机快门,增益,光圈的控制模式,0-自动控制,1-手动控制")->data.buffer = imageInfo.controlMode;
            *(long long *) fs_Ebml_node_addChild_with_a_property(rst, (struct FsEbml_node*) rst, "shutter", FsEbmlNodeType_Integer
                    , "comment", "快门,曝光时间,微妙,controlMode为1时有效")->data.buffer = imageInfo.shutter;
            *(long long *) fs_Ebml_node_addChild_with_a_property(rst, (struct FsEbml_node*) rst, "gain", FsEbmlNodeType_Integer
                    , "comment", "增益,归一化到0-100,controlMode为1时有效")->data.buffer = imageInfo.gain;
            *(long long *) fs_Ebml_node_addChild_with_a_property(rst, (struct FsEbml_node*) rst, "iris", FsEbmlNodeType_Integer
                    , "comment", "光圈,归一化到0-100,controlMode为1时有效")->data.buffer = imageInfo.iris;
            *(long long *) fs_Ebml_node_addChild_with_a_property(rst, (struct FsEbml_node*) rst, "whiteBalanceMode", FsEbmlNodeType_Integer
                    , "comment", "相机白平衡的控制模式,0-自动控制,1-手动控制")->data.buffer = imageInfo.whiteBalanceMode;
            *(long long *) fs_Ebml_node_addChild_with_a_property(rst, (struct FsEbml_node*) rst, "whiteBalance_B", FsEbmlNodeType_Integer
                    , "comment", "白平衡蓝色,0-255,whiteBalanceMode为1时有效")->data.buffer = imageInfo.whiteBalance_B;
            *(long long *) fs_Ebml_node_addChild_with_a_property(rst, (struct FsEbml_node*) rst, "whiteBalance_R", FsEbmlNodeType_Integer
                    , "comment", "白平衡红色,0-255,whiteBalanceMode为1时有效")->data.buffer = imageInfo.whiteBalance_R;
            *(long long *) fs_Ebml_node_addChild_with_a_property(rst, (struct FsEbml_node*) rst, "imageMode", FsEbmlNodeType_Integer
                    , "comment", "图像亮度,色度,对比度,清晰度的控制模式,-1-第二种自动控制模式,0-自动控制,1-手动控制")->data.buffer = imageInfo.imageMode;
            *(long long *) fs_Ebml_node_addChild_with_a_property(rst, (struct FsEbml_node*) rst, "brightness", FsEbmlNodeType_Integer
                    , "comment", "亮度,归一化到0-100,imageMode为1时有效")->data.buffer = imageInfo.brightness;
            *(long long *) fs_Ebml_node_addChild_with_a_property(rst, (struct FsEbml_node*) rst, "colorSaturation", FsEbmlNodeType_Integer
                    , "comment", "色度,归一化到0-100,imageMode为1时有效")->data.buffer = imageInfo.colorSaturation;
            *(long long *) fs_Ebml_node_addChild_with_a_property(rst, (struct FsEbml_node*) rst, "contrast", FsEbmlNodeType_Integer
                    , "comment", "对比度,归一化到0-100,imageMode为1时有效")->data.buffer = imageInfo.contrast;
            *(long long *) fs_Ebml_node_addChild_with_a_property(rst, (struct FsEbml_node*) rst, "sharpness", FsEbmlNodeType_Integer
                    , "comment", "清晰度,归一化到0-100,imageMode为1时有效")->data.buffer = imageInfo.sharpness;
            *(long long *) fs_Ebml_node_addChild_with_a_property(rst, (struct FsEbml_node*) rst, "intensityAdjust", FsEbmlNodeType_Integer
                    , "comment", "明暗度矫正,归一化到0-100")->data.buffer = imageInfo.intensityAdjust;
            *(long long *) fs_Ebml_node_addChild_with_a_property(rst, (struct FsEbml_node*) rst, "bwValid", FsEbmlNodeType_Integer
                    , "comment", "黑白热模式是否有效,0-无效,1-有效")->data.buffer = imageInfo.bwValid;
            if (imageInfo.bwValid) {
                *(long long *) fs_Ebml_node_addChild_with_a_property(rst, (struct FsEbml_node*) rst, "bw", FsEbmlNodeType_Integer
                        , "comment", "黑白热模式,0-白热,1-黑热")->data.buffer = imageInfo.bw;
            }
            *(long long *) fs_Ebml_node_addChild_with_a_property(rst, (struct FsEbml_node*) rst, "cutLeft", FsEbmlNodeType_Integer
                    , "comment", "图像拼接的左裁剪像素")->data.buffer = imageInfo.cutLeft;
            *(long long *) fs_Ebml_node_addChild_with_a_property(rst, (struct FsEbml_node*) rst, "cutRight", FsEbmlNodeType_Integer
                    , "comment", "图像拼接的右裁剪像素")->data.buffer = imageInfo.cutRight;
            *(long long *) fs_Ebml_node_addChild_with_a_property(rst, (struct FsEbml_node*) rst, "cycleMove", FsEbmlNodeType_Integer
                    , "comment", "转圈,0-表示未旋转,1-表示正在旋转")->data.buffer = imageInfo.cycleMove;
            struct FsEbml_node* pEbml_node = fs_Ebml_node_get_first(pEbml, (struct FsEbml_node*) pEbml, "return_type");
            if (pEbml_node && 0 < pEbml_node->data.lenth) {
                if (0 == strcmp("ebml", pEbml_node->data.buffer)) requestDataType = 1;
                else if (0 == strcmp("xml", pEbml_node->data.buffer)) requestDataType = 2;
                else if (0 == strcmp("json", pEbml_node->data.buffer)) requestDataType = 3;
            }
            configManager_send_pthreadSafety__OI_2_default(pCapture_item->ro._pCapture->ro._pConfigManager, rst, requestID_3, head, requestDataType, pShareBuffer);
            return 1;
#endif
        } else if (_rv < 0)goto FsMacrosFunctionTag(FsMacrosFunction(imageinfoget_lt));
        else goto FsMacrosFunctionTag(FsMacrosFunction(imageinfoget_gt));
    }
#endif     
    ////////////////////////////////////////////////////////////////////////////
#if FsMacrosFunction(chipCmd) > 0 // 向机芯直接发送命令
    FsMacrosFunctionTag(FsMacrosFunction(chipCmd)) :;
    {
        const int _rv = strcmp(ctrl_type, "chipCmd");
        if (0 == _rv) {
#if 1        
#ifndef __capture_P_cmd_cb_ctrl_priority_check 
            const unsigned short priority = fs_Ebml_node_get_first_Integer(pEbml, (struct FsEbml_node*) pEbml, "priority", 0);
            pthread_mutex_lock(&pCapture_item->p.__ctrlClientList->mutex);
            configManager_conncet_refer_check(pCapture_item->ro._pCapture->ro._pConfigManager, pCapture_item->p.__ctrlClientList);
            if ((priority & 0xFFFE) < pCapture_item->p.ctrlPriority && (pCapture_item->p.__ctrlClientList->nodeCount > 1 || (1 == pCapture_item->p.__ctrlClientList->nodeCount &&
                    memcmp(requestID_3, pCapture_item->p.__ctrlClientList->pNode + pCapture_item->p.__ctrlClientList->startIndex * sizeof (unsigned int)*6, sizeof (unsigned int)*3) != 0))) {
                /* 连接优先级不够 */
                if (fs_StructList_remove_order_custom(pCapture_item->p.__ctrlClientList, requestID_3, sizeof (unsigned int)*3)>-1)
                    configManager_connect_release(pCapture_item->ro._pCapture->ro._pConfigManager, requestID_3, 0);
                unsigned int ipv4;
                unsigned short ipv4Port;
                configManager_connect_get_addr(&ipv4, &ipv4Port, requestID_3);
                FsPrintf2(1, "Priority(=%hu/%hu) less,client:%p,%x:%hhu.%hhu.%hhu.%hhu:%hu,connectCount=%lu,client:\n", priority, pCapture_item->p.ctrlPriority, *(void**) requestID_3, requestID_3[2]
                        , Fs_Network_Ipv4_print(ipv4), ipv4Port, pCapture_item->p.__ctrlClientList->nodeCount);
                FsObjectBase * const sendData = (FsObjectBase*) fsMalloc(sizeof (FsObjectBase) +
                        8 + Memery_Alignment(sizeof ("Priority(=65535/65535) less,connectCount=1234567890,client[]") - 1 + 22 * pCapture_item->p.__ctrlClientList->nodeCount) + sizeof (unsigned int)*3);
                fs_ObjectBase_init(sendData, sizeof (FsObjectBase) +
                        8 + Memery_Alignment(sizeof ("Priority(=65535/65535) less,connectCount=1234567890,client[]") - 1 + 22 * pCapture_item->p.__ctrlClientList->nodeCount) + sizeof (unsigned int)*3,
                        sizeof (FsObjectBase));
                sendData->len = 8 + sprintf(sendData->data + 8, "Priority(=%hu/%hu) less,connectCount=%lu,client[", priority, pCapture_item->p.ctrlPriority, pCapture_item->p.__ctrlClientList->nodeCount);
                {
                    unsigned int (*ppNode)[6] = ((unsigned int (*)[6])pCapture_item->p.__ctrlClientList->pNode) + pCapture_item->p.__ctrlClientList->startIndex;
                    unsigned int i = 0;
                    for (; i < pCapture_item->p.__ctrlClientList->nodeCount; i++) {
                        configManager_connect_get_addr(&ipv4, &ipv4Port, *ppNode++);
                        printf("[%u]:%hhu.%hhu.%hhu.%hhu:%hu\n", i, Fs_Network_Ipv4_print(ipv4), ipv4Port);
                        sendData->len += sprintf(sendData->data + sendData->len, "%hhu.%hhu.%hhu.%hhu:%hu,", Fs_Network_Ipv4_print(ipv4), ipv4Port);
                    }
                }
                pthread_mutex_unlock(&pCapture_item->p.__ctrlClientList->mutex);
                sendData->data[sendData->len - 1] = ']';
                *((unsigned int *) sendData->data) = (head & 0xFFFFFFF0U) | 0x6U, *((unsigned int *) (sendData->data + 4)) = sendData->len - 8;
                memcpy(sendData->data + Memery_Alignment(sendData->len), requestID_3, sizeof (unsigned int)*3);
                configManager_send_pthreadSafety__OI_2(pCapture_item->ro._pCapture->ro._pConfigManager, sendData);
                return -1;
            }
            pthread_mutex_unlock(&pCapture_item->p.__ctrlClientList->mutex);
#endif
            const unsigned char dataType = fs_Ebml_node_get_first_Integer(pEbml, (struct FsEbml_node*) pEbml, "dataType", 0);
            struct FsEbml_node* pEbml_node = fs_Ebml_node_get_first(pEbml, (struct FsEbml_node*) pEbml, "cmd");
            if (NULL == pEbml_node || pEbml_node->data.lenth < 1) {
                FsLog(FsLogType_error, FsPrintfIndex, "Invalid cmd,data:\n");
                if (FsLogTypeCheck(FsLogType_error)) fs_Ebml_out_debug(pEbml, stdout, pShareBuffer), printf("\n");
#ifndef __capture_P_cmd_cb_ctrl_data_check_error 
                pthread_mutex_lock(&pCapture_item->p.__ctrlClientList->mutex);
                if (fs_StructList_remove_order_custom(pCapture_item->p.__ctrlClientList, requestID_3, sizeof (unsigned int)*3)>-1)
                    configManager_connect_release(pCapture_item->ro._pCapture->ro._pConfigManager, requestID_3, 0);
                if (0 == pCapture_item->p.__ctrlClientList->nodeCount && pCapture_item->p.ctrlPriority != 0) {
                    pCapture_item->ro._ctrl_function->cancelKey_pthreadSafety(pCapture_item->ro.__pCamera_item, pCapture_item->ro.__pCamera_ctrl_item, pCapture_item, pObjectBaseBuffer, pShareBuffer);
                    pCapture_item->p.ctrlPriority = 0;
                }
                pthread_mutex_unlock(&pCapture_item->p.__ctrlClientList->mutex);
                return -1;
#endif
            }
            if (/* 参数是字符串 */0 == dataType) {
                if (FsEbmlNodeType_is_String(pEbml_node->type)) {
                    if (pCapture_item->ro._ctrl_function->chipCmd_pthreadSafety(pCapture_item->ro.__pCamera_item, pCapture_item->ro.__pCamera_ctrl_item, pCapture_item, priority, 0, pEbml_node->data.buffer, pEbml_node->data.lenth - 1, pObjectBaseBuffer, pShareBuffer) != 1) {
#ifndef __capture_P_cmd_cb_ctrl_cb_failed 
                        Capture_itemLog(FsLogType_error, "Ctrl camera failed,data:\n");
                        fs_Ebml_out_debug(pEbml, stdout, pShareBuffer), printf("\n");
                        pthread_mutex_lock(&pCapture_item->p.__ctrlClientList->mutex);
                        if (fs_StructList_remove_order_custom(pCapture_item->p.__ctrlClientList, requestID_3, sizeof (unsigned int)*3)>-1)
                            configManager_connect_release(pCapture_item->ro._pCapture->ro._pConfigManager, requestID_3, 0);
                        if (0 == pCapture_item->p.__ctrlClientList->nodeCount && pCapture_item->p.ctrlPriority != 0) {
                            pCapture_item->ro._ctrl_function->cancelKey_pthreadSafety(pCapture_item->ro.__pCamera_item, pCapture_item->ro.__pCamera_ctrl_item, pCapture_item, pObjectBaseBuffer, pShareBuffer);
                            pCapture_item->p.ctrlPriority = 0;
                        }
                        pthread_mutex_unlock(&pCapture_item->p.__ctrlClientList->mutex);
                        FsObjectBase * const sendData = (FsObjectBase*) fsMalloc(sizeof (FsObjectBase) + 8 + Memery_Alignment(sizeof ("Cb failed.\n") - 1) + sizeof (unsigned int)*3);
                        fs_ObjectBase_init(sendData, sizeof (FsObjectBase) + 8 + Memery_Alignment(sizeof ("Cb failed.\n") - 1) + sizeof (unsigned int)*3, sizeof (FsObjectBase));
                        sendData->len = 8 + sizeof ("Cb failed.\n") - 1;
                        *((unsigned int *) sendData->data) = (head & 0xFFFFFFF0U) | 0x6U, *((unsigned int *) (sendData->data + 4)) = sendData->len - 8;
                        memcpy(sendData->data + 8, "Cb failed.\n", sizeof ("Cb failed.\n") - 1);
                        memcpy(sendData->data + Memery_Alignment(sendData->len), requestID_3, sizeof (unsigned int)*3);
                        configManager_send_pthreadSafety__OI_2(pCapture_item->ro._pCapture->ro._pConfigManager, sendData);
                        return -1;
#endif
                    }
                } else {
                    FsLog(FsLogType_error, FsPrintfIndex, "Invalid pEbml_node->type(=%x),data:\n", pEbml_node->type);
                    if (FsLogTypeCheck(FsLogType_error)) fs_Ebml_out_debug(pEbml, stdout, pShareBuffer), printf("\n");
#ifndef __capture_P_cmd_cb_ctrl_data_check_error 
                    pthread_mutex_lock(&pCapture_item->p.__ctrlClientList->mutex);
                    if (fs_StructList_remove_order_custom(pCapture_item->p.__ctrlClientList, requestID_3, sizeof (unsigned int)*3)>-1)
                        configManager_connect_release(pCapture_item->ro._pCapture->ro._pConfigManager, requestID_3, 0);
                    if (0 == pCapture_item->p.__ctrlClientList->nodeCount && pCapture_item->p.ctrlPriority != 0) {
                        pCapture_item->ro._ctrl_function->cancelKey_pthreadSafety(pCapture_item->ro.__pCamera_item, pCapture_item->ro.__pCamera_ctrl_item, pCapture_item, pObjectBaseBuffer, pShareBuffer);
                        pCapture_item->p.ctrlPriority = 0;
                    }
                    pthread_mutex_unlock(&pCapture_item->p.__ctrlClientList->mutex);
                    return -1;
#endif
                }
            } else if (/* 参数是16进制 */1 == dataType) {
                if (FsEbmlNodeType_is_Binary(pEbml_node->type)) {
                    if (pCapture_item->ro._ctrl_function->chipCmd_pthreadSafety(pCapture_item->ro.__pCamera_item, pCapture_item->ro.__pCamera_ctrl_item, pCapture_item, priority, 1, pEbml_node->data.buffer, pEbml_node->data.lenth, pObjectBaseBuffer, pShareBuffer) != 1) {
#ifndef __capture_P_cmd_cb_ctrl_cb_failed 
                        Capture_itemLog(FsLogType_error, "Ctrl camera failed,data:\n");
                        fs_Ebml_out_debug(pEbml, stdout, pShareBuffer), printf("\n");
                        pthread_mutex_lock(&pCapture_item->p.__ctrlClientList->mutex);
                        if (fs_StructList_remove_order_custom(pCapture_item->p.__ctrlClientList, requestID_3, sizeof (unsigned int)*3)>-1)
                            configManager_connect_release(pCapture_item->ro._pCapture->ro._pConfigManager, requestID_3, 0);
                        if (0 == pCapture_item->p.__ctrlClientList->nodeCount && pCapture_item->p.ctrlPriority != 0) {
                            pCapture_item->ro._ctrl_function->cancelKey_pthreadSafety(pCapture_item->ro.__pCamera_item, pCapture_item->ro.__pCamera_ctrl_item, pCapture_item, pObjectBaseBuffer, pShareBuffer);
                            pCapture_item->p.ctrlPriority = 0;
                        }
                        pthread_mutex_unlock(&pCapture_item->p.__ctrlClientList->mutex);
                        FsObjectBase * const sendData = (FsObjectBase*) fsMalloc(sizeof (FsObjectBase) + 8 + Memery_Alignment(sizeof ("Cb failed.\n") - 1) + sizeof (unsigned int)*3);
                        fs_ObjectBase_init(sendData, sizeof (FsObjectBase) + 8 + Memery_Alignment(sizeof ("Cb failed.\n") - 1) + sizeof (unsigned int)*3, sizeof (FsObjectBase));
                        sendData->len = 8 + sizeof ("Cb failed.\n") - 1;
                        *((unsigned int *) sendData->data) = (head & 0xFFFFFFF0U) | 0x6U, *((unsigned int *) (sendData->data + 4)) = sendData->len - 8;
                        memcpy(sendData->data + 8, "Cb failed.\n", sizeof ("Cb failed.\n") - 1);
                        memcpy(sendData->data + Memery_Alignment(sendData->len), requestID_3, sizeof (unsigned int)*3);
                        configManager_send_pthreadSafety__OI_2(pCapture_item->ro._pCapture->ro._pConfigManager, sendData);
                        return -1;
#endif
                    }
                } else if (FsEbmlNodeType_is_String(pEbml_node->type)) {
#undef Fs_ShareBuffer_var    
#undef Fs_ShareBuffer_var_check
#undef Fs_ShareBuffer_var_array
#undef Fs_ShareBuffer_var_array_check
                    /* 初始化共享buffer的使用环境 */ FsLocal_ShareBuffer_init(pShareBuffer, 1);
                    /* 定义与共享buffer--FsLocal_ShareBuffer关联的局部变量,以_in结尾,多个变量使用逗号隔开,如#define Fs_ShareBuffer_var a_in_acin,b_in */
#define Fs_ShareBuffer_var
                    /* 定义可能与共享buffer--FsLocal_ShareBuffer关联的局部变量,以_cin结尾,多个变量使用逗号隔开,如#define Fs_ShareBuffer_var_check a_cin_acin,b_cin */
#define Fs_ShareBuffer_var_check
                    /* 定义与共享buffer--FsLocal_ShareBuffer关联的局部指针数组变量,以_ain结尾,需要指明数组成员的个数,多个变量使用逗号隔开,如#define Fs_ShareBuffer_var_array a_cin_ain,2 */
#define Fs_ShareBuffer_var_array
                    /* 定义可能与共享buffer--FsLocal_ShareBuffer关联的局部指针数组变量,以_acin结尾,需要指明数组成员的个数,多个变量使用逗号隔开,如#define Fs_ShareBuffer_var_array_check a_cin_acin,2 */
#define Fs_ShareBuffer_var_array_check
                    FsLocal_ShareBuffer_resize((pEbml_node->data.lenth + 3) / 4 * 3);
                    int len = fs_String_buffer_decode_from_base64(FsLocal_ShareBuffer + FsLocal_ShareBufferPos, pEbml_node->data.lenth - 1, pEbml_node->data.buffer);
                    if (len < 0) {
                        /* 释放共享buffer */ FsLocal_ShareBuffer_release(pShareBuffer, 0);
                        FsLog(FsLogType_error, FsPrintfIndex, "Decode base64 failed,data:\n");
                        if (FsLogTypeCheck(FsLogType_error)) fs_Ebml_out_debug(pEbml, stdout, pShareBuffer), printf("\n");
#ifndef __capture_P_cmd_cb_ctrl_data_check_error 
                        pthread_mutex_lock(&pCapture_item->p.__ctrlClientList->mutex);
                        if (fs_StructList_remove_order_custom(pCapture_item->p.__ctrlClientList, requestID_3, sizeof (unsigned int)*3)>-1)
                            configManager_connect_release(pCapture_item->ro._pCapture->ro._pConfigManager, requestID_3, 0);
                        if (0 == pCapture_item->p.__ctrlClientList->nodeCount && pCapture_item->p.ctrlPriority != 0) {
                            pCapture_item->ro._ctrl_function->cancelKey_pthreadSafety(pCapture_item->ro.__pCamera_item, pCapture_item->ro.__pCamera_ctrl_item, pCapture_item, pObjectBaseBuffer, pShareBuffer);
                            pCapture_item->p.ctrlPriority = 0;
                        }
                        pthread_mutex_unlock(&pCapture_item->p.__ctrlClientList->mutex);
                        return -1;
#endif
                    } else {
                        FsLocal_ShareBuffer_call_before(FsLocal_ShareBufferPos + len);
                        if (pCapture_item->ro._ctrl_function->chipCmd_pthreadSafety(pCapture_item->ro.__pCamera_item, pCapture_item->ro.__pCamera_ctrl_item, pCapture_item, priority, 1, pEbml_node->data.buffer, pEbml_node->data.lenth, pObjectBaseBuffer, &FsLocal_ShareBuffer) != 1) {
                            FsLocal_ShareBuffer_call_after();
                            /* 释放共享buffer */ FsLocal_ShareBuffer_release(pShareBuffer, 0);
#ifndef __capture_P_cmd_cb_ctrl_cb_failed 
                            Capture_itemLog(FsLogType_error, "Ctrl camera failed,data:\n");
                            fs_Ebml_out_debug(pEbml, stdout, pShareBuffer), printf("\n");
                            pthread_mutex_lock(&pCapture_item->p.__ctrlClientList->mutex);
                            if (fs_StructList_remove_order_custom(pCapture_item->p.__ctrlClientList, requestID_3, sizeof (unsigned int)*3)>-1)
                                configManager_connect_release(pCapture_item->ro._pCapture->ro._pConfigManager, requestID_3, 0);
                            if (0 == pCapture_item->p.__ctrlClientList->nodeCount && pCapture_item->p.ctrlPriority != 0) {
                                pCapture_item->ro._ctrl_function->cancelKey_pthreadSafety(pCapture_item->ro.__pCamera_item, pCapture_item->ro.__pCamera_ctrl_item, pCapture_item, pObjectBaseBuffer, pShareBuffer);
                                pCapture_item->p.ctrlPriority = 0;
                            }
                            pthread_mutex_unlock(&pCapture_item->p.__ctrlClientList->mutex);
                            FsObjectBase * const sendData = (FsObjectBase*) fsMalloc(sizeof (FsObjectBase) + 8 + Memery_Alignment(sizeof ("Cb failed.\n") - 1) + sizeof (unsigned int)*3);
                            fs_ObjectBase_init(sendData, sizeof (FsObjectBase) + 8 + Memery_Alignment(sizeof ("Cb failed.\n") - 1) + sizeof (unsigned int)*3, sizeof (FsObjectBase));
                            sendData->len = 8 + sizeof ("Cb failed.\n") - 1;
                            *((unsigned int *) sendData->data) = (head & 0xFFFFFFF0U) | 0x6U, *((unsigned int *) (sendData->data + 4)) = sendData->len - 8;
                            memcpy(sendData->data + 8, "Cb failed.\n", sizeof ("Cb failed.\n") - 1);
                            memcpy(sendData->data + Memery_Alignment(sendData->len), requestID_3, sizeof (unsigned int)*3);
                            configManager_send_pthreadSafety__OI_2(pCapture_item->ro._pCapture->ro._pConfigManager, sendData);
                            return -1;
#endif
                        } else {
                            FsLocal_ShareBuffer_call_after();
                            /* 释放共享buffer */ FsLocal_ShareBuffer_release(pShareBuffer, 0);
                        }
                    }
                } else {
                    FsLog(FsLogType_error, FsPrintfIndex, "Invalid pEbml_node->type(=%x),data:\n", pEbml_node->type);
                    if (FsLogTypeCheck(FsLogType_error)) fs_Ebml_out_debug(pEbml, stdout, pShareBuffer), printf("\n");
#ifndef __capture_P_cmd_cb_ctrl_data_check_error 
                    pthread_mutex_lock(&pCapture_item->p.__ctrlClientList->mutex);
                    if (fs_StructList_remove_order_custom(pCapture_item->p.__ctrlClientList, requestID_3, sizeof (unsigned int)*3)>-1)
                        configManager_connect_release(pCapture_item->ro._pCapture->ro._pConfigManager, requestID_3, 0);
                    if (0 == pCapture_item->p.__ctrlClientList->nodeCount && pCapture_item->p.ctrlPriority != 0) {
                        pCapture_item->ro._ctrl_function->cancelKey_pthreadSafety(pCapture_item->ro.__pCamera_item, pCapture_item->ro.__pCamera_ctrl_item, pCapture_item, pObjectBaseBuffer, pShareBuffer);
                        pCapture_item->p.ctrlPriority = 0;
                    }
                    pthread_mutex_unlock(&pCapture_item->p.__ctrlClientList->mutex);
                    return -1;
#endif
                }
            } else {
                FsLog(FsLogType_error, FsPrintfIndex, "Invalid dataType(=%hhu),data:\n", dataType);
                if (FsLogTypeCheck(FsLogType_error)) fs_Ebml_out_debug(pEbml, stdout, pShareBuffer), printf("\n");
#ifndef __capture_P_cmd_cb_ctrl_data_check_error 
                pthread_mutex_lock(&pCapture_item->p.__ctrlClientList->mutex);
                if (fs_StructList_remove_order_custom(pCapture_item->p.__ctrlClientList, requestID_3, sizeof (unsigned int)*3)>-1)
                    configManager_connect_release(pCapture_item->ro._pCapture->ro._pConfigManager, requestID_3, 0);
                if (0 == pCapture_item->p.__ctrlClientList->nodeCount && pCapture_item->p.ctrlPriority != 0) {
                    pCapture_item->ro._ctrl_function->cancelKey_pthreadSafety(pCapture_item->ro.__pCamera_item, pCapture_item->ro.__pCamera_ctrl_item, pCapture_item, pObjectBaseBuffer, pShareBuffer);
                    pCapture_item->p.ctrlPriority = 0;
                }
                pthread_mutex_unlock(&pCapture_item->p.__ctrlClientList->mutex);
                return -1;
#endif
            }
            ////////////////////////////////////////////////////////////////////////
            /* 发回执数据 */
#ifndef __capture_P_cmd_cb_ctrl_cb_ok 
            configManager_conncet_refer_send_buffer(FsStringLenData("ok"), requestID_3, (head & 0xFFFFFFF0U) | 0x2U, pObjectBaseBuffer);
            if (0 == priority) {
                if (pCapture_item->p.ctrlPriority != 0) {
                    pCapture_item->p.ctrlPriority = 0;
                    if (1 == pCapture_item->p.__ctrlClientList->nodeCount) {
                        configManager_connect_release(pCapture_item->ro._pCapture->ro._pConfigManager, requestID_3, 0);
                        fs_StructList_clear(pCapture_item->p.__ctrlClientList);
                    }
                }
                return 1;
            } else {
                if (pCapture_item->p.ctrlPriority != priority) {
                    pCapture_item->p.ctrlPriority = priority;
                    capture_private_item_ctrlClient_release(pCapture_item, requestID_3);
                }
                unsigned int data[6] = {requestID_3[0], requestID_3[1], requestID_3[2], head, (unsigned int) requestDataType, 0};
                if (fs_StructList_insert_order_custom_update_pthreadSafety(pCapture_item->p.__ctrlClientList, data, sizeof (unsigned int)*3) < 0) return 1;
                else return -128;
            }
#endif
#endif
        } else if (_rv < 0)goto FsMacrosFunctionTag(FsMacrosFunction(chipCmd_lt));
        else goto FsMacrosFunctionTag(FsMacrosFunction(chipCmd_gt));
    }
#endif
    ////////////////////////////////////////////////////////////////////////////
#if FsMacrosFunction(shutterReset) > 0 // 重置快门
    FsMacrosFunctionTag(FsMacrosFunction(shutterReset)) :;
    {
        const int _rv = strcmp(ctrl_type, "shutterReset");
        if (0 == _rv) {
#if 1        
#ifndef __capture_P_cmd_cb_ctrl_priority_check 
            const unsigned short priority = fs_Ebml_node_get_first_Integer(pEbml, (struct FsEbml_node*) pEbml, "priority", 0);
            pthread_mutex_lock(&pCapture_item->p.__ctrlClientList->mutex);
            configManager_conncet_refer_check(pCapture_item->ro._pCapture->ro._pConfigManager, pCapture_item->p.__ctrlClientList);
            if ((priority & 0xFFFE) < pCapture_item->p.ctrlPriority && (pCapture_item->p.__ctrlClientList->nodeCount > 1 || (1 == pCapture_item->p.__ctrlClientList->nodeCount &&
                    memcmp(requestID_3, pCapture_item->p.__ctrlClientList->pNode + pCapture_item->p.__ctrlClientList->startIndex * sizeof (unsigned int)*6, sizeof (unsigned int)*3) != 0))) {
                /* 连接优先级不够 */
                if (fs_StructList_remove_order_custom(pCapture_item->p.__ctrlClientList, requestID_3, sizeof (unsigned int)*3)>-1)
                    configManager_connect_release(pCapture_item->ro._pCapture->ro._pConfigManager, requestID_3, 0);
                unsigned int ipv4;
                unsigned short ipv4Port;
                configManager_connect_get_addr(&ipv4, &ipv4Port, requestID_3);
                FsPrintf2(1, "Priority(=%hu/%hu) less,client:%p,%x:%hhu.%hhu.%hhu.%hhu:%hu,connectCount=%lu,client:\n", priority, pCapture_item->p.ctrlPriority, *(void**) requestID_3, requestID_3[2]
                        , Fs_Network_Ipv4_print(ipv4), ipv4Port, pCapture_item->p.__ctrlClientList->nodeCount);
                FsObjectBase * const sendData = (FsObjectBase*) fsMalloc(sizeof (FsObjectBase) +
                        8 + Memery_Alignment(sizeof ("Priority(=65535/65535) less,connectCount=1234567890,client[]") - 1 + 22 * pCapture_item->p.__ctrlClientList->nodeCount) + sizeof (unsigned int)*3);
                fs_ObjectBase_init(sendData, sizeof (FsObjectBase) +
                        8 + Memery_Alignment(sizeof ("Priority(=65535/65535) less,connectCount=1234567890,client[]") - 1 + 22 * pCapture_item->p.__ctrlClientList->nodeCount) + sizeof (unsigned int)*3,
                        sizeof (FsObjectBase));
                sendData->len = 8 + sprintf(sendData->data + 8, "Priority(=%hu/%hu) less,connectCount=%lu,client[", priority, pCapture_item->p.ctrlPriority, pCapture_item->p.__ctrlClientList->nodeCount);
                {
                    unsigned int (*ppNode)[6] = ((unsigned int (*)[6])pCapture_item->p.__ctrlClientList->pNode) + pCapture_item->p.__ctrlClientList->startIndex;
                    unsigned int i = 0;
                    for (; i < pCapture_item->p.__ctrlClientList->nodeCount; i++) {
                        configManager_connect_get_addr(&ipv4, &ipv4Port, *ppNode++);
                        printf("[%u]:%hhu.%hhu.%hhu.%hhu:%hu\n", i, Fs_Network_Ipv4_print(ipv4), ipv4Port);
                        sendData->len += sprintf(sendData->data + sendData->len, "%hhu.%hhu.%hhu.%hhu:%hu,", Fs_Network_Ipv4_print(ipv4), ipv4Port);
                    }
                }
                pthread_mutex_unlock(&pCapture_item->p.__ctrlClientList->mutex);
                sendData->data[sendData->len - 1] = ']';
                *((unsigned int *) sendData->data) = (head & 0xFFFFFFF0U) | 0x6U, *((unsigned int *) (sendData->data + 4)) = sendData->len - 8;
                memcpy(sendData->data + Memery_Alignment(sendData->len), requestID_3, sizeof (unsigned int)*3);
                configManager_send_pthreadSafety__OI_2(pCapture_item->ro._pCapture->ro._pConfigManager, sendData);
                return -1;
            }
            pthread_mutex_unlock(&pCapture_item->p.__ctrlClientList->mutex);
#endif
            if (pCapture_item->ro._ctrl_function->shutterReset_pthreadSafety(pCapture_item->ro.__pCamera_item, pCapture_item->ro.__pCamera_ctrl_item, pCapture_item, priority, pObjectBaseBuffer, pShareBuffer) != 1) {
#ifndef __capture_P_cmd_cb_ctrl_cb_failed 
                Capture_itemLog(FsLogType_error, "Ctrl camera failed,data:\n");
                fs_Ebml_out_debug(pEbml, stdout, pShareBuffer), printf("\n");
                pthread_mutex_lock(&pCapture_item->p.__ctrlClientList->mutex);
                if (fs_StructList_remove_order_custom(pCapture_item->p.__ctrlClientList, requestID_3, sizeof (unsigned int)*3)>-1)
                    configManager_connect_release(pCapture_item->ro._pCapture->ro._pConfigManager, requestID_3, 0);
                if (0 == pCapture_item->p.__ctrlClientList->nodeCount && pCapture_item->p.ctrlPriority != 0) {
                    pCapture_item->ro._ctrl_function->cancelKey_pthreadSafety(pCapture_item->ro.__pCamera_item, pCapture_item->ro.__pCamera_ctrl_item, pCapture_item, pObjectBaseBuffer, pShareBuffer);
                    pCapture_item->p.ctrlPriority = 0;
                }
                pthread_mutex_unlock(&pCapture_item->p.__ctrlClientList->mutex);
                FsObjectBase * const sendData = (FsObjectBase*) fsMalloc(sizeof (FsObjectBase) + 8 + Memery_Alignment(sizeof ("Cb failed.\n") - 1) + sizeof (unsigned int)*3);
                fs_ObjectBase_init(sendData, sizeof (FsObjectBase) + 8 + Memery_Alignment(sizeof ("Cb failed.\n") - 1) + sizeof (unsigned int)*3, sizeof (FsObjectBase));
                sendData->len = 8 + sizeof ("Cb failed.\n") - 1;
                *((unsigned int *) sendData->data) = (head & 0xFFFFFFF0U) | 0x6U, *((unsigned int *) (sendData->data + 4)) = sendData->len - 8;
                memcpy(sendData->data + 8, "Cb failed.\n", sizeof ("Cb failed.\n") - 1);
                memcpy(sendData->data + Memery_Alignment(sendData->len), requestID_3, sizeof (unsigned int)*3);
                configManager_send_pthreadSafety__OI_2(pCapture_item->ro._pCapture->ro._pConfigManager, sendData);
                return -1;
#endif
            }
            ////////////////////////////////////////////////////////////////////////
            /* 发回执数据 */
#ifndef __capture_P_cmd_cb_ctrl_cb_ok 
            configManager_conncet_refer_send_buffer(FsStringLenData("ok"), requestID_3, (head & 0xFFFFFFF0U) | 0x2U, pObjectBaseBuffer);
            if (0 == priority) {
                if (pCapture_item->p.ctrlPriority != 0) {
                    pCapture_item->p.ctrlPriority = 0;
                    if (1 == pCapture_item->p.__ctrlClientList->nodeCount) {
                        configManager_connect_release(pCapture_item->ro._pCapture->ro._pConfigManager, requestID_3, 0);
                        fs_StructList_clear(pCapture_item->p.__ctrlClientList);
                    }
                }
                return 1;
            } else {
                if (pCapture_item->p.ctrlPriority != priority) {
                    pCapture_item->p.ctrlPriority = priority;
                    capture_private_item_ctrlClient_release(pCapture_item, requestID_3);
                }
                unsigned int data[6] = {requestID_3[0], requestID_3[1], requestID_3[2], head, (unsigned int) requestDataType, 0};
                if (fs_StructList_insert_order_custom_update_pthreadSafety(pCapture_item->p.__ctrlClientList, data, sizeof (unsigned int)*3) < 0) return 1;
                else return -128;
            }
#endif
#endif
        } else if (_rv < 0)goto FsMacrosFunctionTag(FsMacrosFunction(shutterReset_lt));
        else goto FsMacrosFunctionTag(FsMacrosFunction(shutterReset_gt));
    }
#endif
    ////////////////////////////////////////////////////////////////////////////    
#if FsMacrosFunction(maskget) > 0 // 控制掩码获取
    FsMacrosFunctionTag(FsMacrosFunction(maskget)) :;
    {
        const int _rv = strcmp(ctrl_type, "maskget");
        if (0 == _rv) {
#if 1
            FsEbml * const rst = fs_Ebml_new__IO(FsEbmlEncodeMethod_UTF8, 0);
            {
#undef Fs_ShareBuffer_var    
#undef Fs_ShareBuffer_var_check
#undef Fs_ShareBuffer_var_array
#undef Fs_ShareBuffer_var_array_check
                /* 初始化共享buffer的使用环境 */ FsLocal_ShareBuffer_init(pShareBuffer, 1);
                /* 定义与共享buffer--FsLocal_ShareBuffer关联的局部变量,以_in结尾,多个变量使用逗号隔开,如#define Fs_ShareBuffer_var a_in_acin,b_in */
#define Fs_ShareBuffer_var protocol_name_in_in,device_type_out_in,device_type_in_in,device_sn_out_in,device_sn_in_in,device_version_out_in,device_version_in_in
                /* 定义可能与共享buffer--FsLocal_ShareBuffer关联的局部变量,以_cin结尾,多个变量使用逗号隔开,如#define Fs_ShareBuffer_var_check a_cin_acin,b_cin */
#define Fs_ShareBuffer_var_check
                /* 定义与共享buffer--FsLocal_ShareBuffer关联的局部指针数组变量,以_ain结尾,需要指明数组成员的个数,多个变量使用逗号隔开,如#define Fs_ShareBuffer_var_array a_cin_ain,2 */
#define Fs_ShareBuffer_var_array
                /* 定义可能与共享buffer--FsLocal_ShareBuffer关联的局部指针数组变量,以_acin结尾,需要指明数组成员的个数,多个变量使用逗号隔开,如#define Fs_ShareBuffer_var_array_check a_cin_acin,2 */
#define Fs_ShareBuffer_var_array_check                
                const char* protocol_name_out;
                char *protocol_name_in_in = FsLocal_ShareBuffer + FsLocal_ShareBufferPos;
                char *device_type_out_in = protocol_name_in_in + 32;
                char *device_type_in_in = device_type_out_in + 32;
                char *device_sn_out_in = device_type_in_in + 32;
                char *device_sn_in_in = device_sn_out_in + 32;
                char *device_version_out_in = device_sn_in_in + 32;
                char *device_version_in_in = device_version_out_in + 32;
                FsLocal_ShareBuffer_resize(32 + 32 * 2 + 32 * 2 + 32 * 2);
                unsigned long long mask_out, mask_in;
                FsLocal_ShareBuffer_call_before(FsLocal_ShareBufferPos + 32 + 32 * 2 + 32 * 2 + 32 * 2);
                const unsigned long long mask = pCapture_item->ro._ctrl_function->mask_get_pthreadSafety(pCapture_item->ro.__pCamera_item, pCapture_item->ro.__pCamera_ctrl_item, &protocol_name_out
                        , protocol_name_in_in, 32, device_type_out_in, device_type_in_in, 32, device_sn_out_in, device_sn_in_in, 32, device_version_out_in, device_version_in_in, 32
                        , &mask_out, &mask_in, pObjectBaseBuffer, &FsLocal_ShareBuffer);
                FsLocal_ShareBuffer_call_after();
                *(unsigned long long *) fs_Ebml_node_addChild_with_a_property(rst, (struct FsEbml_node*) rst, "mask", FsEbmlNodeType_Integer
                        , "comment", "掩码,按位排布,0-任意转,1-左右转,2-上下转,3-45度斜角转,4-变倍,5-聚焦,6-预置位,7-3d定位,8-gotoPTZ和getPTZ,9-图像参数设置与获取,10-直接向机芯发送命令,11-快门重置")->data.buffer = mask;
                *(unsigned long long *) fs_Ebml_node_addChild_with_a_property(rst, (struct FsEbml_node*) rst, "mask_out", FsEbmlNodeType_Integer
                        , "comment", "外部接口的掩码,值类型同mask字段")->data.buffer = mask_out;
                fs_Ebml_node_data_set_string(fs_Ebml_node_addChild_with_a_property(rst, (struct FsEbml_node*) rst, "protocol_name_out", FsEbmlNodeType_String
                        , "comment", "外部接口的协议类型"), protocol_name_out);
                fs_Ebml_node_data_set_string(fs_Ebml_node_addChild_with_a_property(rst, (struct FsEbml_node*) rst, "device_type_out", FsEbmlNodeType_String
                        , "comment", "外部接口的设备型号"), device_type_out_in);
                fs_Ebml_node_data_set_string(fs_Ebml_node_addChild_with_a_property(rst, (struct FsEbml_node*) rst, "device_sn_out", FsEbmlNodeType_String
                        , "comment", "外部接口的设备序列号"), device_sn_out_in);
                fs_Ebml_node_data_set_string(fs_Ebml_node_addChild_with_a_property(rst, (struct FsEbml_node*) rst, "device_version_out", FsEbmlNodeType_String
                        , "comment", "外部接口的设备版本号"), device_version_out_in);
                *(unsigned long long *) fs_Ebml_node_addChild_with_a_property(rst, (struct FsEbml_node*) rst, "mask_in", FsEbmlNodeType_Integer
                        , "comment", "内部接口的掩码,值类型同mask字段")->data.buffer = mask_in;
                fs_Ebml_node_data_set_string(fs_Ebml_node_addChild_with_a_property(rst, (struct FsEbml_node*) rst, "protocol_name_in", FsEbmlNodeType_String
                        , "comment", "内部接口的协议类型"), protocol_name_in_in);
                fs_Ebml_node_data_set_string(fs_Ebml_node_addChild_with_a_property(rst, (struct FsEbml_node*) rst, "device_type_in", FsEbmlNodeType_String
                        , "comment", "内部接口的设备型号"), device_type_in_in);
                fs_Ebml_node_data_set_string(fs_Ebml_node_addChild_with_a_property(rst, (struct FsEbml_node*) rst, "device_sn_in", FsEbmlNodeType_String
                        , "comment", "内部接口的设备序列号"), device_sn_in_in);
                fs_Ebml_node_data_set_string(fs_Ebml_node_addChild_with_a_property(rst, (struct FsEbml_node*) rst, "device_version_in", FsEbmlNodeType_String
                        , "comment", "内部接口的设备版本号"), device_version_in_in);
                /* 释放共享buffer */ FsLocal_ShareBuffer_release(pShareBuffer, 0);
            }

            struct FsEbml_node * return_type = fs_Ebml_node_get_first(pEbml, (struct FsEbml_node*) pEbml, "return_type");
            if (return_type && 0 < return_type->data.lenth) {
                if (0 == strcmp("ebml", return_type->data.buffer)) requestDataType = 1;
                else if (0 == strcmp("xml", return_type->data.buffer)) requestDataType = 2;
                else if (0 == strcmp("json", return_type->data.buffer)) requestDataType = 3;
            }
            configManager_send_pthreadSafety__OI_2_default(pCapture_item->ro._pCapture->ro._pConfigManager, rst, requestID_3, head, requestDataType, pShareBuffer);
            return 1;
#endif
        } else if (_rv < 0)goto FsMacrosFunctionTag(FsMacrosFunction(maskget_lt));
        else goto FsMacrosFunctionTag(FsMacrosFunction(maskget_gt));
    }
#endif    
    ////////////////////////////////////////////////////////////////////////////
#if FsMacrosFunction(release) > 0 // 释放相机控制权限
    FsMacrosFunctionTag(FsMacrosFunction(release)) :;
    {
        const int _rv = strcmp(ctrl_type, "release");
        if (0 == _rv) {
#if 1            
            if (1 == pCapture_item->p.__ctrlClientList->nodeCount &&
                    memcmp(requestID_3, pCapture_item->p.__ctrlClientList->pNode + pCapture_item->p.__ctrlClientList->startIndex * sizeof (unsigned int)*6, sizeof (unsigned int)*3) == 0) {
                fs_StructList_clear(pCapture_item->p.__ctrlClientList);
                configManager_connect_release(pCapture_item->ro._pCapture->ro._pConfigManager, requestID_3, 0);
#ifdef FsDebug
                if (0 == pCapture_item->p.ctrlPriority) {
                    Capture_itemLog(FsLogType_fatal, "0 == pCapture_item->p.ctrlPriority(=%hu)", pCapture_item->p.ctrlPriority);
                    fflush(stdout);
                    FsLogExit();
                }
#endif
                pCapture_item->ro._ctrl_function->cancelKey_pthreadSafety(pCapture_item->ro.__pCamera_item, pCapture_item->ro.__pCamera_ctrl_item, pCapture_item, pObjectBaseBuffer, pShareBuffer);
                pCapture_item->p.ctrlPriority = 0;
            } else if (pCapture_item->p.__ctrlClientList->nodeCount > 1) {
                if (fs_StructList_remove_order_custom(pCapture_item->p.__ctrlClientList, requestID_3, sizeof (unsigned int)*3)>-1)
                    configManager_connect_release(pCapture_item->ro._pCapture->ro._pConfigManager, requestID_3, 0);
            }
            ////////////////////////////////////////////////////////////////////////
            /* 发回执数据 */
            configManager_conncet_refer_send_buffer(FsStringLenData("ok"), requestID_3, (head & 0xFFFFFFF0U) | 0x2U, pObjectBaseBuffer);
            return 1;
#endif
        } else if (_rv < 0)goto FsMacrosFunctionTag(FsMacrosFunction(release_lt));
        else goto FsMacrosFunctionTag(FsMacrosFunction(release_gt));
    }
#endif    
}

/* 在有用户请求此命令字时的调用函数,成功返回1,失败返回-1,需要引用此连接返回-128,为空表示此命令字不允许远程调用 */
static int capture_P_cmd_cb_errorInfo(/* 与请求相关的信息,用于识别是发给哪个客户端的数据,用3个int来储存 */const unsigned int requestID_3[], /* 收到数据的前4字节 */unsigned int head
        , /* 收到的数据 */FsEbml *pEbml, /* 客户端发送请求的数据类型,1-ebml数据,2-xml数据,3-json数据 */ char requestDataType, /* 调用函数的指针 */ struct Capture_item * const pCapture_item
        , /* 缓存Buffer,不为空 */FsObjectBaseBuffer * const pObjectBaseBuffer, /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer) {
    /* 获取错误数据的客户端 */
    if (NULL == pCapture_item->p.__errorInfoClientList) pCapture_item->p.__errorInfoClientList = fs_StructList_new__IO(6, sizeof (int)*6);
    {
#ifndef __cmd_cb_cal_return_type 
        const char *const return_type = fs_Ebml_node_get_first_string(pEbml, (struct FsEbml_node*) pEbml, "return_type", NULL);
        if (return_type) {
            if (0 == strcmp("ebml", return_type)) requestDataType = 1;
            else if (0 == strcmp("xml", return_type)) requestDataType = 2;
            else if (0 == strcmp("json", return_type)) requestDataType = 3;
        }
#endif
    }
    pthread_mutex_lock(&pCapture_item->p.__errorInfoClientList->mutex);
    unsigned int data[6] = {requestID_3[0], requestID_3[1], requestID_3[2], head, (unsigned int) requestDataType, 0};
    if (fs_StructList_insert_order(pCapture_item->p.__errorInfoClientList, data) < 0) {
        pthread_mutex_unlock(&pCapture_item->p.__errorInfoClientList->mutex);
        return 1;
    } else {
        pthread_mutex_unlock(&pCapture_item->p.__errorInfoClientList->mutex);
        return -128;
    }
}

/* 控制相机取流的回调函数 */
static void capture_P_item_cb_streamControl(/* 掩码名 */ const char maskName[], /* uuid */ const char uuid[], /* 状态,0-全0,1-部分为1 */const unsigned char status
        , /* 是否是在connect中回调,connect连接时会把当前的掩码状态回调出来,0-不是,1-是 */const unsigned char isConnectCb, /* 关联的对象 */ struct Capture_item * const pCapture_item) {
    // FsPrintf(1, "status=%hhu/%hhu,uuid=%s,rst=%p,%p\n", status, isConnectCb, uuid, pCapture_item, pCapture_item->ro._streamControl);
    //fs_Bd_backtrace_out(stdout);
    //FsPrintf(1,"%p,%p\n",pCapture_item,pCapture_item->ro.__pCamera_item);
    pCapture_item->ro._streamControl(pCapture_item->ro.__pCamera_item, status);
    if (pCapture_item->ro.__pSubStream)
        pCapture_item->ro.__pSubStream->_streamControl(pCapture_item->ro.__pSubStream->__pCamera_item, status);
    ////////////////////////////////////////////////////////////////////////////
    {
        struct Capture * const pCapture = pCapture_item->ro._pCapture;
        pthread_mutex_lock(&pCapture->p.streamCtrlStatusClientList->mutex);
        if (status) {
            /* 流开始 */
            if (fs_ObjectList_insert_order(pCapture->p.streamCtrlStartItemListOrder__streamCtrlStatusClientList, pCapture_item) < 1) {
                Capture_itemLog(FsLogType_fatal, "Insert failed,streamCtrlStartItemListOrder__streamCtrlStatusClientList=%p/%lu,isConnectCb=%hhu"
                        , pCapture->p.streamCtrlStartItemListOrder__streamCtrlStatusClientList, pCapture->p.streamCtrlStartItemListOrder__streamCtrlStatusClientList->nodeCount, isConnectCb);
                fflush(stdout);
                FsLogExit();
            }
            capture_P_item_send_streamCtrlStatus_locked(pCapture_item, status, pCapture->ro._pConfigManager, pCapture->p.streamCtrlStatusClientList, NULL);
        } else {
            /* 流结束 */
            if (0 == isConnectCb) {
                if (fs_ObjectList_remove_order(pCapture->p.streamCtrlStartItemListOrder__streamCtrlStatusClientList, pCapture_item) < 0) {
                    Capture_itemLog(FsLogType_fatal, "Remove failed,streamCtrlStartItemListOrder__streamCtrlStatusClientList=%p/%lu,isConnectCb=%hhu"
                            , pCapture->p.streamCtrlStartItemListOrder__streamCtrlStatusClientList, pCapture->p.streamCtrlStartItemListOrder__streamCtrlStatusClientList->nodeCount, isConnectCb);
                    fflush(stdout);
                    FsLogExit();
                }
                capture_P_item_send_streamCtrlStatus_locked(pCapture_item, status, pCapture->ro._pConfigManager, pCapture->p.streamCtrlStatusClientList, NULL);
            } else {
#ifdef FsDebug
                if (fs_ObjectList_check_exist_order(pCapture->p.streamCtrlStartItemListOrder__streamCtrlStatusClientList, pCapture_item) >= 0) {
                    Capture_itemLog(FsLogType_fatal, "Check failed,streamCtrlStartItemListOrder__streamCtrlStatusClientList=%p/%lu,isConnectCb=%hhu"
                            , pCapture->p.streamCtrlStartItemListOrder__streamCtrlStatusClientList, pCapture->p.streamCtrlStartItemListOrder__streamCtrlStatusClientList->nodeCount, isConnectCb);
                    fflush(stdout);
                    FsLogExit();
                }
#endif
            }
        }
        pthread_mutex_unlock(&pCapture->p.streamCtrlStatusClientList->mutex);
    }
}

/* 在调用configManager_transfer_out函数时使用哪个函数回调数据,成功返回1,失败返回-1 */
static int capture_P_item_transfer_cb_error_distribution(struct Capture_item * const pCapture_item, /* 数据长度 */const unsigned int dataLen, /* 传送的数据 */const char data[]
        , /* 缓存Buffer,不为空 */FsObjectBaseBuffer * const pObjectBaseBuffer, /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer) {
    if (pCapture_item->p.__errorInfoClientList) {
        FsEbml * const pEbmlSend = fs_Ebml_new__IO(FsEbmlEncodeMethod_UTF8, 0);
        fs_Ebml_node_data_set_string(fs_Ebml_node_addChild_with_a_property(pEbmlSend, (struct FsEbml_node*) pEbmlSend, "uuid", FsEbmlNodeType_String
                , "comment", "uuid"), pCapture_item->ro._uuid);
        fs_Ebml_node_data_set(fs_Ebml_node_addChild_with_a_property(pEbmlSend, (struct FsEbml_node*) pEbmlSend, "data", FsEbmlNodeType_Binary
                , "comment", "故障数据"), dataLen, data);
        pthread_mutex_lock(&pCapture_item->p.__errorInfoClientList->mutex);
        configManager_conncet_refer_send(pCapture_item->ro._pCapture->ro._pConfigManager, pCapture_item->p.__errorInfoClientList
                , NULL, NULL, NULL, pEbmlSend, 0x2, pObjectBaseBuffer);
        pthread_mutex_unlock(&pCapture_item->p.__errorInfoClientList->mutex);
        fs_Ebml_delete__OI(pEbmlSend, pShareBuffer);
    }
    //FsPrintf(1, "RecvData dataLen=%d\n", dataLen);
    return 1;
}

static void capture_P_item_new(struct Capture * const pCapture, /* 通道号,从1开始,为0用于集群 */const unsigned int channel, /* 本地ip地址 */const FsArray * const ipList
        , /* 原来的采集项 */struct Capture_item * * const ppCapture_item, /* rtsp相机指针 */FsObjectList * const camera_rtsp_list_
        , /* gb28181相机指针 */FsObjectList * const camera_gb28181_list_
        , /* inflray相机 */struct Camera_inflray * * const ppCamera_inflray
        , /* 相机控制 */ Camera_ctrl * * const ppCamera_ctrl, MonitorItem * const pMonitorItem
        , /* 缓存Buffer,不为空 */FsObjectBaseBuffer * const pObjectBaseBuffer, /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer) {
#undef FsFunctionName
#define FsFunctionName capture_P_item_new
    FsConfig * const pConfig = ((ConfigManager*) pCapture->ro._pConfigManager)->ro.__pConfig;
    if (NULL == pConfig)return;
    const void *vsys0 = pConfig;
    FsObjectList * const clusterList = fs_Config_node_template_orderFirst__IO(pConfig, &vsys0, pConfig, 0, ipList, 0 == channel, "vsys");
    if (clusterList) {
        void **ppNodeCluster = clusterList->pNode + clusterList->startIndex;
        unsigned int uj = clusterList->nodeCount, index = 0;
        do {
            const void *vsysChannel0 = vsys0;
            const void* const vsys = *ppNodeCluster++;
            FsObjectList * const list = 0 == channel ? fs_Config_node_template_orderFirst__IO(pConfig, &vsysChannel0, vsys, 0, NULL, 0, "vsysChannel")
                    : (FsObjectList *) fs_Config_node_template_get_orderFirst(pConfig, &vsysChannel0, vsys, 0, NULL, 0, "vsysChannel", channel - 1);
            if (list) {
                void ** ppNode;
                unsigned int ui, ipv4;
                if (0 == channel) {
                    ppNode = list->pNode + list->startIndex;
                    ui = list->nodeCount;
                } else ui = 1;
                struct Capture_item **ppNodeCapture_item;
                if (0 == channel) {
                    if (NULL == pCapture->p.clusterItemList_) pCapture->p.clusterItemList_ = fs_ObjectList_new_unsafe__IO(4);
                    while (pCapture->p.clusterItemList_->nodeCount < index + ui)fs_ObjectList_insert_tail(pCapture->p.clusterItemList_, NULL);
                    ppNodeCapture_item = (struct Capture_item**) pCapture->p.clusterItemList_->pNode + pCapture->p.clusterItemList_->startIndex;
                    {
                        const char *const hostname = fs_Config_node_string_get_first(pConfig, vsys0, vsys, "hostname", NULL);
                        const char *const pd = strchr(hostname, ':');
                        ipv4 = fs_ipv4_network_get(pd ? pd - hostname : strlen(hostname), hostname);
                    }
                } else ipv4 = 0;
                do {
                    void* vsysChannel, *sumNode;
                    if (0 == channel) {
                        sumNode = vsysChannel = *ppNode++;
                        if (ppNodeCapture_item[index] != NULL && ppNodeCapture_item[index]->ro._sum == fs_Config_get_sum((FsEbml*) pConfig, (struct FsEbml_node*) sumNode)) {
                            index++;
                            continue;
                        }
                    } else {
                        sumNode = vsysChannel = list;
                        if (*ppCapture_item != NULL && (*ppCapture_item)->ro._sum == fs_Config_get_sum((FsEbml*) pConfig, (struct FsEbml_node*) sumNode))break;
                    }
                    ////////////////////////////////////////////////////////////
                    pMonitorItem->rw.line = __LINE__;
                    //FsPrintf(FsPrintfIndex, "channel=%u.\n", channel);
                    if (0 == channel) {
                        if (ppNodeCapture_item[index])capture_P_item_delete__OI(ppNodeCapture_item[index], ppCamera_ctrl, pObjectBaseBuffer, pShareBuffer);
                    } else if (*ppCapture_item)capture_P_item_delete__OI(*ppCapture_item, ppCamera_ctrl, pObjectBaseBuffer, pShareBuffer);
                    pMonitorItem->rw.line = __LINE__;
                    ////////////////////////////////////////////////////////////
                    struct Capture_item * rst;
                    if (0)rst = NULL;
                    else {
                        const FsString * const uuid = fs_Config_node_string_get_first_String(pConfig, vsysChannel0, vsysChannel, "uuid", NULL);
                        {
                            char *pd;
                            unsigned int len = uuid->lenth;
                            if (channel > 0) {
                                rst = (struct Capture_item*) fsMalloc(sizeof (struct Capture_item) +len);
                                memset(rst, 0, sizeof (struct Capture_item));
                                pd = (char*) (rst + 1);
                            } else {
                                rst = (struct Capture_item*) fsMalloc(Capture_item_ClusterLen + len);
                                memset(rst, 0, Capture_item_ClusterLen);
                                pd = ((char*) rst) + Capture_item_ClusterLen;
                            }
                            /* 本对象的uuid值,一般是从配置中读入的 */
                            rst->ro._uuid = pd, len = uuid->lenth, memcpy(pd, uuid->buffer, len), pd += len;
                            /* 所属主机的ip地址,用于集群,本机为0 */
                            rst->ro._ipv4 = ipv4;
                            /* 所属的记录对象 */
                            rst->ro._pCapture = pCapture;
                        }
                        /* 整个通道依赖的所有数据的校验和 */
                        rst->ro._sum = fs_Config_get_sum((FsEbml*) pConfig, (struct FsEbml_node*) sumNode);
                        if (channel > 0) {
                            /* 当有数据可输出时写的字节,使用Fs_GroupSqrtOut_value_set宏写此字节 */
                            Fs_GroupSqrtOut_set_pthreadSafety(rst->rw._pGroupSqrtOut, pCapture->ro._pGroupSqrtOut->groupSqrt_member, pCapture->ro._pGroupSqrtOut->groupSqrt_group
                                    , pCapture->ro._pGroupSqrtOut->groupValue, channel - 1, &pCapture->ro._pGroupSqrtOut->groupSqrt_mutex);
                        }
                        ////////////////////////////////////////////////////////////
                        if (channel > 0) {
                            /* 传出的数据帧链表 */
                            rst->ro.__framelistOut = fs_ObjectList_new__IO(6);
                            /* 分发错误信息的对象 */
                            rst->ro._pTransfer_error = configManager_transfer_register(pCapture->ro._pConfigManager, "error_distribution", uuid->buffer, rst);

                            const void *cameraConfig0 = vsysChannel0;
                            const void *const cameraConfig = fs_Config_node_get_first(pConfig, &cameraConfig0, vsysChannel, "cameraConfig");
                            const char *const cameraType = fs_Config_node_string_get_first(pConfig, cameraConfig0, cameraConfig, "cameraType", NULL);
                            const unsigned long long streamMask = fs_Config_node_integer_get_mask(pConfig, cameraConfig0, cameraConfig, "streamMask", NULL);
                            const unsigned char streamOver = fs_Config_node_integer_get_mask(pConfig, cameraConfig0, cameraConfig, "streamOver", NULL);
                            ////////////////////////////////////////////////////
                            /* 内部控制的函数指针,可用于集群,通知收到的相机控制指令,连接其他本系统的平台进行相机控制 */
                            if (streamMask & 0x4) {
                                if (NULL == *ppCamera_ctrl) {
                                    *ppCamera_ctrl = camera_ctrl_new__IO(pCapture->ro._pMonitor, pCapture->ro._externRunStatus, systemThreadTid, "Camera_ctrl", pCapture->ro._pConfigManager, NULL);
                                    camera_ctrl_startThread(*ppCamera_ctrl, -1);
                                }
                                struct Camera_ctrl_item * const pCamera_ctrl_item = camera_ctrl_item_new__IO(uuid->buffer, NULL, 0, 0, NULL, NULL
                                        , 0 != 0 ? fs_Config_node_string_get_first(pConfig, cameraConfig0, cameraConfig, "channelUUID", NULL) : NULL);
                                camera_ctrl_add_item__OI_2(*ppCamera_ctrl, pCamera_ctrl_item);
                                /* 对应的相机的二级指针,为空表示是本地数据推送,老版本表示对应的相机对象链表 */
                                rst->ro.__pCamera_ctrl_item = pCamera_ctrl_item;
                            }
                            ////////////////////////////////////////////////////
#define __capture_P_item_new_cameraType_error 0
#define __capture_P_item_new_cameraType_gb28181 1
#define __capture_P_item_new_cameraType_gb28181_lt 0
#define __capture_P_item_new_cameraType_gb28181_gt 2
#define __capture_P_item_new_cameraType_inflray 2 
#define __capture_P_item_new_cameraType_inflray_lt 0
#define __capture_P_item_new_cameraType_inflray_gt 0
#define __capture_P_item_new_cameraType_localPull 3
#define __capture_P_item_new_cameraType_localPull_lt 1
#define __capture_P_item_new_cameraType_localPull_gt 5
#define __capture_P_item_new_cameraType_localPush 4
#define __capture_P_item_new_cameraType_localPush_lt 0
#define __capture_P_item_new_cameraType_localPush_gt 0
#define __capture_P_item_new_cameraType_rtmp 5
#define __capture_P_item_new_cameraType_rtmp_lt 4
#define __capture_P_item_new_cameraType_rtmp_gt 6
#define __capture_P_item_new_cameraType_rtsp 6 
#define __capture_P_item_new_cameraType_rtsp_lt 0
#define __capture_P_item_new_cameraType_rtsp_gt 0
#ifdef FsDebug
                            if (sizeof (camera_rtsp_ctrl_function) != sizeof (struct Camera_ctrl_functionUser) || sizeof (camera_gb28181_ctl_function) != sizeof (struct Camera_ctrl_functionUser) || sizeof (camera_inflray_ctrl_function) != sizeof (struct Camera_ctrl_functionUser)) {
                                FsLog(FsLogType_fatal, FsPrintfIndex, "Error,sizeof (struct Camera_ctrl_functionUser)=%lu,sizeof (camera_rtsp_ctrl_function)=%lu,sizeof (camera_gb28181_ctl_function)=%lu,sizeof (camera_inflray_ctrl_function)=%lu\n"
                                        , sizeof (struct Camera_ctrl_functionUser), sizeof (camera_rtsp_ctrl_function), sizeof (camera_gb28181_ctl_function), sizeof (camera_inflray_ctrl_function));
                                FsLogTag(0);
                                fflush(stdout);
                                FsLogExit();
                            }
#endif
                            goto FsMacrosFunctionTag(3);
                            for (;;) {
                                ////////////////////////////////////////////////////
#if FsMacrosFunction(cameraType_error) >= 0
                                FsMacrosFunctionTag(FsMacrosFunction(cameraType_error)) :;
                                {

                                    struct Camera_rtsp *pCamera_rtsp, **ppNode_Camera_rtsp;
                                    struct Camera_rtsp_item *pCamera_rtsp_item, *pCamera_rtsp_item2 = NULL;
                                    {
                                        unsigned int k = camera_rtsp_list_->nodeCount;
                                        //判断单个线程的通道数是否>nodeCount, 否则再打开一个线程 
                                        if (k > 0) {
                                            ppNode_Camera_rtsp = (struct Camera_rtsp **) camera_rtsp_list_->pNode + camera_rtsp_list_->startIndex + camera_rtsp_list_->nodeCount - 1;
                                            for (; k > 0; k--) {
                                                pCamera_rtsp = *ppNode_Camera_rtsp--;
                                                if (pCamera_rtsp->ro.__cameralist_->nodeCount < pCapture->ro._channelCountPerThread)break;
                                            }
                                        }
                                        if (0 == k) {
                                            pCamera_rtsp = camera_rtsp_new__IO(pCapture->ro._pMonitor, pCapture->ro._externRunStatus, systemThreadTid, "Camera_rtsp", pCapture->ro._pRtspClient);
                                            camera_rtsp_startThread(pCamera_rtsp, -1);
                                            fs_ObjectList_insert_tail(camera_rtsp_list_, pCamera_rtsp);
                                        }
                                        const FsString * const ipAddr = fs_Config_node_string_get_first_String(pConfig, cameraConfig0, cameraConfig, "ipv4", NULL);
                                        unsigned int camera_ipv4;
                                        if (ipAddr) {
                                            camera_ipv4 = fs_ipv4_network_get(ipAddr->lenth - 1, ipAddr->buffer);
                                            // if (FS_INVALID_IPV4 == camera_ipv4) FsPrintf(1, "Channel(=%u) get \"%s\" failed.\n", channel, pd), fflush(stdout);
                                        } else camera_ipv4 = FS_INVALID_IPV4;

                                        const unsigned short rtspPort = fs_Config_node_integer_get_first(pConfig, cameraConfig0, cameraConfig, "rtspPort", 554, NULL);

                                        const char *const rtspURL = fs_Config_node_string_get_first(pConfig, cameraConfig0, cameraConfig, "rtspURL", NULL);
                                        const char *const rtspURL2 = fs_Config_node_string_get_first(pConfig, cameraConfig0, cameraConfig, "rtspURL2", NULL);
                                        const char *const rstpUsername = fs_Config_node_string_get_first(pConfig, cameraConfig0, cameraConfig, "rstpUsername", NULL);
                                        const char *const rstpPasswd = fs_Config_node_string_get_first(pConfig, cameraConfig0, cameraConfig, "rstpPasswd", NULL);
                                        const unsigned long long controlMask = fs_Config_node_integer_get_mask(pConfig, cameraConfig0, cameraConfig, "controlMask", NULL);
                                        const char *const channelUUID = fs_Config_node_string_get_first(pConfig, cameraConfig0, cameraConfig, "channelUUID", NULL);
                                        const unsigned short holderPort = fs_Config_node_integer_get_first(pConfig, cameraConfig0, cameraConfig, "holderPort", 0, NULL);
                                        const unsigned long long controlMaskDefault = fs_Config_node_integer_get_mask(pConfig, cameraConfig0, cameraConfig, "controlMaskDefault", NULL);
                                        {
                                            if (NULL == rtspURL2 || 0 == (streamMask & 0x3)) {
                                                pCamera_rtsp_item = camera_rtsp_item_new__IO(streamMask, controlMask, controlMaskDefault, FS_INVALID_IPV4 == camera_ipv4 ? (ipAddr ? ipAddr->buffer : (0 == rtspPort ? NULL : "$$Invlaid domain rtsp1")) : NULL
                                                        , FS_INVALID_IPV4 == camera_ipv4 ? 0 : camera_ipv4, rtspPort, rstpUsername, rstpPasswd, rtspURL, NULL, 0, holderPort, channelUUID, streamOver
                                                        , (signed char (*)(FsObject*, void*, MonitorItem*))capture_P_camera_rtsp_cb, rst);
                                            } else {
                                                pCamera_rtsp_item = camera_rtsp_item_new__IO(streamMask, controlMask, controlMaskDefault, FS_INVALID_IPV4 == camera_ipv4 ? (ipAddr ? ipAddr->buffer : (0 == rtspPort ? NULL : "$$Invlaid domain rtsp2")) : NULL
                                                        , FS_INVALID_IPV4 == camera_ipv4 ? 0 : camera_ipv4, rtspPort, rstpUsername, rstpPasswd, rtspURL, NULL, 0, holderPort, channelUUID, streamOver
                                                        , (signed char (*)(FsObject*, void*, MonitorItem*))capture_P_camera_rtsp_cb_main, rst);

                                                pCamera_rtsp_item2 = camera_rtsp_item_new__IO(streamMask & 0x3, 0, 0, FS_INVALID_IPV4 == camera_ipv4 ? (ipAddr ? ipAddr->buffer : (0 == rtspPort ? NULL : "$$Invlaid domain rtsp3")) : NULL
                                                        , FS_INVALID_IPV4 == camera_ipv4 ? 0 : camera_ipv4, rtspPort, rstpUsername, rstpPasswd, rtspURL2, NULL, 0, holderPort, NULL, streamOver
                                                        , (signed char (*)(FsObject*, void*, MonitorItem*))capture_P_camera_rtsp_cb_sub, rst);
                                            }
                                        }
                                    }
                                    /* 对应的相机对象链表 */
                                    rst->ro._ppCamera = (void**) camera_rtsp_list_;
                                    /* 相机项 */
                                    rst->ro.__pCamera_item = pCamera_rtsp_item;
                                    //FsPrintf(1,"%p,%p\n",rst,pCamera_rtsp_item);
                                    /* 从相机中删除相机项并删除相机链表中无相机的相机对象的函数指针,并把相机指针置0 */
                                    rst->ro._camera_remove_and_delete_item_and_camera = (void (*)(void* * const, void*const, FsShareBuffer * const))camera_rtsp_remove_and_delete_item_and_emptyCamera;
                                    camera_rtsp_add_item__OI_2(pCamera_rtsp, pCamera_rtsp_item);
                                    /* 控制相机流的函数指针 */
                                    rst->ro._streamControl = (void (*)(void*, unsigned char)) camera_rtsp_item_streamControl_pthreadSafety;
                                    /* 子码流,为空表示不存在子码流 */
                                    if (pCamera_rtsp_item2) {
                                        rst->ro.__pSubStream = (struct Capture_item_SubStream*) fsMalloc(sizeof (struct Capture_item_SubStream));
                                        memset(rst->ro.__pSubStream, 0, sizeof (struct Capture_item_SubStream));
                                        /* 图像宽度,0为无效值 */
                                        //unsigned int imageWidth;
                                        /* 图像高度,0为无效值 */
                                        //unsigned int imageHeight;
                                        /* 对应的相机对象链表,为空表示是本地数据推送 */
                                        rst->ro.__pSubStream->_pCameraList_ = camera_rtsp_list_;
                                        /* 相机项 */
                                        rst->ro.__pSubStream->__pCamera_item = pCamera_rtsp_item2;
                                        /* 从相机中删除相机项并删除相机链表中无相机的相机对象的函数指针,并把相机指针置0 */
                                        rst->ro.__pSubStream->_camera_remove_and_delete_item_and_emptyCamera = (void (*)(void*const, void*const))camera_rtsp_remove_and_delete_item_and_emptyCamera;
                                        camera_rtsp_add_item__OI_2(pCamera_rtsp, pCamera_rtsp_item2);
                                        /* 控制相机流的函数指针 */
                                        rst->ro.__pSubStream->_streamControl = (void (*)(void*, unsigned char)) camera_rtsp_item_streamControl_pthreadSafety;
                                        /* 缓存的未输出的数据帧 */
                                        //FsObject * __pData[4];
                                        /* 缓存的数据帧个数,负数表示缓存方码流,正数表示缓存子码流 */
                                        //signed char dataCount;
                                    }
                                    /* 连接实时视频掩码通信 */
                                    configManager_mask_connect_pthreadSafety(pCapture->ro._pConfigManager, "realStream", uuid->buffer, rst
                                            , (void (*)(const char*, const char*, unsigned char, unsigned char, void*)) capture_P_item_cb_streamControl, rst);
                                    /* 控制 */
                                    rst->ro._ctrl_function = (struct Camera_ctrl_functionUser*) camera_rtsp_ctrl_function;
                                    break;
                                }
#endif
                                ////////////////////////////////////////////////////
#if FsMacrosFunction(cameraType_gb28181) > 0
                                FsMacrosFunctionTag(FsMacrosFunction(cameraType_gb28181)) :;
                                {
                                    int _rv = strcmp(cameraType, "gb28181");
                                    if (0 == _rv) {
                                        /* 是gb28181相机,因统一使用一个端口号,故gb28181不支持创建多对象 */
                                        struct Camera_gb28181 *pCamera_gb28181;
                                        struct Camera_gb28181_item *pCamera_gb28181_item;
                                        {
                                            if (1 == camera_gb28181_list_->nodeCount)pCamera_gb28181 = (struct Camera_gb28181 *) camera_gb28181_list_->pNode[camera_gb28181_list_->startIndex];
                                            else {
                                                pCamera_gb28181 = camera_gb28181_new__IO(pCapture->ro._pMonitor, pCapture->ro._externRunStatus, systemThreadTid, "Camera_gb28181", pCapture->ro._pGB28181Client);
                                                camera_gb28181_startThread(pCamera_gb28181, -1);
                                                fs_ObjectList_insert_tail(camera_gb28181_list_, pCamera_gb28181);
                                            }
                                            const char *const gb28181_id = fs_Config_node_string_get_first(pConfig, cameraConfig0, cameraConfig, "gb28181_id", NULL);
                                            const char *const gb28181_sip_id = fs_Config_node_string_get_first(pConfig, cameraConfig0, cameraConfig, "gb28181_sip_id", NULL);
                                            const char *const gb28181_sip_domain = fs_Config_node_string_get_first(pConfig, cameraConfig0, cameraConfig, "gb28181_sip_domain", NULL);
                                            const char *const rstpUsername = fs_Config_node_string_get_first(pConfig, cameraConfig0, cameraConfig, "rstpUsername", NULL);
                                            const char *const rstpPasswd = fs_Config_node_string_get_first(pConfig, cameraConfig0, cameraConfig, "rstpPasswd", NULL);
                                            const char *const rtspURL = fs_Config_node_string_get_first(pConfig, cameraConfig0, cameraConfig, "rtspURL", NULL);
                                            unsigned int camera_ipv4;
                                            {
                                                const FsString * const ipAddr = fs_Config_node_string_get_first_String(pConfig, cameraConfig0, cameraConfig, "ipv4", NULL);
                                                if (ipAddr) {
                                                    camera_ipv4 = fs_ipv4_network_get(ipAddr->lenth - 1, ipAddr->buffer);
                                                    if (FS_INVALID_IPV4 == camera_ipv4) FsPrintf(1, "Channel(=%u) get \"%s\" failed.\n", channel, ipAddr->buffer), fflush(stdout);
                                                } else camera_ipv4 = 0;
                                            }
                                            const unsigned short rtspPort = fs_Config_node_integer_get_first(pConfig, cameraConfig0, cameraConfig, "rtspPort", 554, NULL);
                                            const unsigned char gb28181_sipOver = fs_Config_node_integer_get_mask(pConfig, cameraConfig0, cameraConfig, "gb28181_sipOver", NULL);
                                            const unsigned long long controlMaskDefault = fs_Config_node_integer_get_mask(pConfig, cameraConfig0, cameraConfig, "controlMaskDefault", NULL);
                                            pCamera_gb28181_item = camera_gb28181_item_new__IO(streamMask, controlMaskDefault, gb28181_id, gb28181_sip_id, gb28181_sip_domain, rstpUsername, rstpPasswd, rtspURL
                                                    , NULL
                                                    , camera_ipv4, rtspPort, gb28181_sipOver
                                                    , streamOver

                                                    , (signed char (*)(FsObject*, void*, MonitorItem*))capture_P_camera_rtsp_cb, rst);
                                        }
                                        /* 对应的相机对象链表 */
                                        rst->ro._ppCamera = (void**) camera_gb28181_list_;
                                        /* 相机项 */
                                        rst->ro.__pCamera_item = pCamera_gb28181_item;
                                        /* 从相机中删除相机项并删除相机链表中无相机的相机对象的函数指针,并把相机指针置0 */
                                        rst->ro._camera_remove_and_delete_item_and_camera = (void (*)(void* * const, void*const, FsShareBuffer * const))camera_gb28181_remove_and_delete_item_and_emptyCamera;
                                        camera_gb28181_add_item__OI_2(pCamera_gb28181, pCamera_gb28181_item);
                                        /* 控制相机流的函数指针 */
                                        rst->ro._streamControl = (void (*)(void*, unsigned char)) camera_gb28181_item_streamControl_pthreadSafety;
                                        /* 连接实时视频掩码通信 */
                                        configManager_mask_connect_pthreadSafety(pCapture->ro._pConfigManager, "realStream", uuid->buffer, rst
                                                , (void (*)(const char*, const char*, unsigned char, unsigned char, void*)) capture_P_item_cb_streamControl, rst);
                                        /* 控制 */
                                        rst->ro._ctrl_function = (struct Camera_ctrl_functionUser*) camera_gb28181_ctl_function;
                                        break;
                                    } else if (_rv < 0)goto FsMacrosFunctionTag(FsMacrosFunction(cameraType_gb28181_lt));
                                    else goto FsMacrosFunctionTag(FsMacrosFunction(cameraType_gb28181_gt));
                                }
#endif
                                ////////////////////////////////////////////////////
#if FsMacrosFunction(cameraType_inflray) > 0
                                FsMacrosFunctionTag(FsMacrosFunction(cameraType_inflray)) :;
                                {
                                    int _rv = strcmp(cameraType, "inflray");
                                    if (0 == _rv) {
                                        if (NULL == *ppCamera_inflray) {
                                            *ppCamera_inflray = camera_inflray_new__IO(pCapture->ro._pMonitor, pCapture->ro._externRunStatus, systemThreadTid, "Camera_pull", pShareBuffer);
                                            camera_inflray_startThread(*ppCamera_inflray, -1);
                                        }
                                        struct Camera_inflray_item *pCamera_inflray_item;
                                        {
                                            const FsString * const ipAddr = fs_Config_node_string_get_first_String(pConfig, cameraConfig0, cameraConfig, "ipv4", NULL);
                                            unsigned int camera_ipv4;
                                            if (ipAddr) {
                                                camera_ipv4 = fs_ipv4_network_get(ipAddr->lenth - 1, ipAddr->buffer);
                                                // if (FS_INVALID_IPV4 == camera_ipv4) FsPrintf(1, "Channel(=%u) get \"%s\" failed.\n", channel, pd), fflush(stdout);
                                            } else camera_ipv4 = FS_INVALID_IPV4;

                                            const unsigned short rtspPort = fs_Config_node_integer_get_first(pConfig, cameraConfig0, cameraConfig, "rtspPort", 6666, NULL);
                                            const unsigned short holderPort = fs_Config_node_integer_get_first(pConfig, cameraConfig0, cameraConfig, "holderPort", 0, NULL);
                                            const unsigned short shutterRest = fs_Config_node_integer_get_first(pConfig, cameraConfig0, cameraConfig, "shutterRest", 0, NULL);
                                            const unsigned long long controlMaskDefault = fs_Config_node_integer_get_mask(pConfig, cameraConfig0, cameraConfig, "controlMaskDefault", NULL);
                                            pCamera_inflray_item = camera_inflray_item_new__IO(streamMask, controlMaskDefault, FS_INVALID_IPV4 == camera_ipv4 ? (ipAddr ? ipAddr->buffer : "Invlaid domain inflray") : NULL,
                                                    FS_INVALID_IPV4 == camera_ipv4 ? 0 : camera_ipv4, rtspPort, holderPort, shutterRest, rst->ro.__pCamera_ctrl_item
                                                    , (int (*)(FsObjectImageFrame *pFrame, /* 外部传入的指针 */ void *p, MonitorItem * pMonitorItem))capture_P_camera_inflray_cb, rst, rst->ro._pTransfer_error);
                                        }
                                        /* 对应的相机的二级指针,为空表示是本地数据推送,老版本表示对应的相机对象链表 */
                                        rst->ro._ppCamera = (void**) ppCamera_inflray;
                                        /* 相机项 */
                                        rst->ro.__pCamera_item = pCamera_inflray_item;
                                        /* 从相机中删除相机项,在相机项全部被删除后把相机也删除,老版本表示从相机中删除相机项并删除相机链表中无相机的相机对象的函数指针,并把相机指针置0 */
                                        rst->ro._camera_remove_and_delete_item_and_camera = (void (*)(void* * const, void*const, FsShareBuffer * const))camera_inflray_delete_item_and_camera;
                                        camera_inflray_add_item__OI_2(*ppCamera_inflray, pCamera_inflray_item);
                                        /* 控制相机流的函数指针 */
                                        rst->ro._streamControl = (void (*)(void*, unsigned char)) camera_inflray_item_streamControl_pthreadSafety;
                                        /* 连接实时视频掩码通信 */
                                        configManager_mask_connect_pthreadSafety(pCapture->ro._pConfigManager, "realStream", uuid->buffer, rst
                                                , (void (*)(const char*, const char*, unsigned char, unsigned char, void*)) capture_P_item_cb_streamControl, rst);
                                        /* 控制 */
                                        rst->ro._ctrl_function = (struct Camera_ctrl_functionUser*) camera_inflray_ctrl_function;
                                        break;
                                    } else if (_rv < 0)goto FsMacrosFunctionTag(FsMacrosFunction(cameraType_inflray_lt));
                                    else goto FsMacrosFunctionTag(FsMacrosFunction(cameraType_inflray_gt));
                                }
#endif
                                ////////////////////////////////////////////////////
#if FsMacrosFunction(cameraType_localPull) > 0
                                FsMacrosFunctionTag(FsMacrosFunction(cameraType_localPull)) :;
                                {
                                    int _rv = strcmp(cameraType, "localPull");
                                    if (0 == _rv) {
                                        goto FsMacrosFunctionTag(FsMacrosFunction(cameraType_error));
                                    } else if (_rv < 0)goto FsMacrosFunctionTag(FsMacrosFunction(cameraType_localPull_lt));
                                    else goto FsMacrosFunctionTag(FsMacrosFunction(cameraType_localPull_gt));
                                }
#endif
                                ////////////////////////////////////////////////////
#if FsMacrosFunction(cameraType_localPush) > 0
                                FsMacrosFunctionTag(FsMacrosFunction(cameraType_localPush)) :;
                                {
                                    int _rv = strcmp(cameraType, "localPush");
                                    if (0 == _rv) {
                                        goto FsMacrosFunctionTag(FsMacrosFunction(cameraType_error));
                                    } else if (_rv < 0)goto FsMacrosFunctionTag(FsMacrosFunction(cameraType_localPush_lt));
                                    else goto FsMacrosFunctionTag(FsMacrosFunction(cameraType_localPush_gt));
                                }
#endif
                                ////////////////////////////////////////////////////
#if FsMacrosFunction(cameraType_rtmp) > 0
                                FsMacrosFunctionTag(FsMacrosFunction(cameraType_rtmp)) :;
                                {
                                    int _rv = strcmp(cameraType, "rtmp");
                                    if (0 == _rv) {
                                        goto FsMacrosFunctionTag(FsMacrosFunction(cameraType_error));
                                    } else if (_rv < 0)goto FsMacrosFunctionTag(FsMacrosFunction(cameraType_rtmp_lt));
                                    else goto FsMacrosFunctionTag(FsMacrosFunction(cameraType_rtmp_gt));
                                }
#endif

                                ////////////////////////////////////////////////////
#if FsMacrosFunction(cameraType_rtsp) > 0
                                FsMacrosFunctionTag(FsMacrosFunction(cameraType_rtsp)) :;
                                {
                                    int _rv = strcmp(cameraType, "rtsp");
                                    if (0 == _rv) {
                                        /* 默认按rtsp处理 */
                                        goto FsMacrosFunctionTag(FsMacrosFunction(cameraType_error));
                                    } else if (_rv < 0)goto FsMacrosFunctionTag(FsMacrosFunction(cameraType_rtsp_lt));
                                    else goto FsMacrosFunctionTag(FsMacrosFunction(cameraType_rtsp_gt));
                                }
#endif
                            }
                            ////////////////////////////////////////////////////                         
                            {
                                //相机控制指令函数指针 
                                rst->ro._Redcamera_close = (int (*)(void *, unsigned int, double))capture_Redcamera_close;
                                rst->ro._Redcamera_open = (int (*)(void *, unsigned int, double))capture_Redcamera_open;
                                rst->ro._camera_left = (int (*)(void *, unsigned int, double))capture_camera_left;
                                rst->ro._camera_right = (int (*)(void *, unsigned int, double))capture_camera_right;
                                rst->ro._camera_up = (int (*)(void *, unsigned int, double))capture_camera_up;
                                rst->ro._camera_down = (int (*)(void *, unsigned int, double))capture_camera_down;
                                rst->ro._camera_leftup = (int (*)(void *, unsigned int, double))capture_camera_leftup;
                                rst->ro._camera_leftdown = (int (*)(void *, unsigned int, double))capture_camera_leftdown;
                                rst->ro._camera_rightup = (int (*)(void *, unsigned int, double))capture_camera_rightup;
                                rst->ro._camera_rightdown = (int (*)(void *, unsigned int, double))capture_camera_rightdown;
                                rst->ro._camera_stopturn = (int (*)(void *, unsigned int, double))capture_camera_stopturn;

                                rst->ro._camera_iris_close = (int (*)(void *, unsigned int, double))capture_camera_iris_close;
                                rst->ro._camera_iris_open = (int (*)(void *, unsigned int, double))capture_camera_iris_open;
                                rst->ro._camera_focusfar = (int (*)(void *, unsigned int, double))capture_camera_focusfar;
                                rst->ro._camera_focusnear = (int (*)(void *, unsigned int, double))capture_camera_focusnear;
                                rst->ro._camera_zoomfar = (int (*)(void *, unsigned int, double))capture_camera_zoomfar;
                                rst->ro._camera_zoomnear = (int (*)(void *, unsigned int, double))capture_camera_zoomnear;

                                rst->ro._camera_throughfrog = (int (*)(void *, unsigned int, double))capture_camera_throughfrog;

                                rst->ro._camera_autofocus = (int (*)(void *, unsigned int, double))capture_camera_autofocus;

                                rst->ro._camera_gotoptz = (int (*)(void *, unsigned int, float, float, float, double))capture_camera_gotoPTZ;
                                rst->ro._camera_getptz = (int (*)(void *, float *, float *, float *))capture_camera_getPTZ;


                                {
                                    //获取云台ip和端口
                                    const char* holderipv4 = fs_Config_node_string_get_first(pConfig, cameraConfig0, cameraConfig, "holderipv4", NULL);
                                    if (NULL == holderipv4) holderipv4 = fs_Config_node_string_get_first(pConfig, cameraConfig0, cameraConfig, "ipv4", NULL);
                                    if (holderipv4) {
                                        sprintf(rst->p.camera_ip, "%s", holderipv4);
                                    } else rst->p.camera_ip[0] = 0;
                                }

                                rst->p.camera_port = fs_Config_node_integer_get_first(pConfig, cameraConfig0, cameraConfig, "holderPort", 0, NULL);

                                rst->p.shot_type = fs_Config_node_integer_get_first(pConfig, cameraConfig0, cameraConfig, "shotType", 0, NULL);

                                rst->rw.camera_c_p = 0.0;
                                rst->rw.camera_c_t = 0.0;
                                rst->rw.camera_c_z = 0.0;
                                rst->rw.goto_p = 0.0;
                                rst->rw.goto_t = 0.0;
                                rst->rw.goto_z = 0.0;
                                rst->rw.m_action = 0;
                                rst->rw.m_stopAction = 100;
                                rst->rw.camera_speed = 0;
                                rst->rw.camera_time = 0;
                                rst->rw.m_auto_focus = 0;

                                /* 注册命令字 */
                                configManager_cmd_register(pCapture->ro._pConfigManager, "camctl", rst->ro._uuid, 0, rst, 0, camctl_private_cmd_cb, NULL, rst, pShareBuffer);
                            }

                            //configManager_cmd_register(pCapture->ro._pConfigManager, "cameractrl", rst->ro._uuid, ipv4, rst, 0 == ipv4 ? capture_private_cmd_cb : NULL, NULL, rst);
                        }
                        ////////////////////////////////////////////////////////////////////////////
                        /* 注册命令字 */
                        configManager_cmd_register(pCapture->ro._pConfigManager, "cameractrl", rst->ro._uuid, ipv4, rst, 0 == ipv4 ? 1 : 0
                                , 0 == ipv4 ? (int (*)(const unsigned int*, unsigned int, FsEbml*, char, void*, FsObjectBaseBuffer*, char**))capture_P_cmd_cb : NULL, 0 == ipv4 ? rst : NULL, rst, pShareBuffer);
                        configManager_cmd_register(pCapture->ro._pConfigManager, "errorInfo", rst->ro._uuid, ipv4, rst, 0 == ipv4 ? 1 : 0
                                , 0 == ipv4 ? (int (*)(const unsigned int*, unsigned int, FsEbml*, char, void*, FsObjectBaseBuffer*, char**))capture_P_cmd_cb_errorInfo : NULL, 0 == ipv4 ? rst : NULL, rst, pShareBuffer);

                        ////////////////////////////////////////////////////////////////////////////
                        /* 绑定命令字 */
                        ////////////////////////////////////////////////////////
                        /* 获取相机错误信息 */
                        if (0 == ipv4) configManager_transfer_connect(pCapture->ro._pConfigManager, "error_distribution", uuid->buffer, rst
                                , (int (* const) (void * const, const unsigned int, void **, FsObjectBaseBuffer * const, FsShareBuffer * const))capture_P_item_transfer_cb_error_distribution, rst);
                    }
                    if (channel > 0)*ppCapture_item = rst;
                    else ppNodeCapture_item[index++] = rst;
                    ////////////////////////////////////////////////////////////////////////////
                    /* 释放内存空间 */
                } while (--ui > 0 && 0 == channel);
                if (0 == channel) fs_ObjectList_delete__OI(list);
            }
        } while (--uj > 0 && 0 == channel);
        fs_ObjectList_delete__OI(clusterList);
        if (0 == channel && pCapture->p.clusterItemList_) {
            while (pCapture->p.clusterItemList_->nodeCount > index)capture_P_item_delete__OI((struct Capture_item*) fs_ObjectList_remove_tail(pCapture->p.clusterItemList_), ppCamera_ctrl, pObjectBaseBuffer, pShareBuffer);
            if (0 == index) {
                fs_ObjectList_delete_unsafe__OI(pCapture->p.clusterItemList_);
                pCapture->p.clusterItemList_ = NULL;
            }
        }
    }
}

static void *capture_P_T(struct Capture * const pCapture) {
#undef FsFunctionName
#define FsFunctionName capture_P_T
    /* 创建线程监控 */
    void *pMonitor = pCapture->ro._pMonitor;
    if (NULL == pMonitor)pMonitor = monitor_new__IO(NULL, 0, "Monitor", NULL, 0, 5.0, 0.1, 0, NULL, NULL, NULL);
    MonitorItem * const pMonitorItem = monitorItem_new_and_add(pMonitor, Capture_get_threadName(pCapture), NULL, pCapture, pCapture->ro._pMonitor ? pCapture->ro._parentTid : 0, NULL);
    thread_join_add_check_pthreadSafety(MonitorItem_get_threadName(pMonitorItem), NULL);
    pMonitorItem->rw.line = __LINE__;
    ////////////////////////////////////////////////////////////////////////////
    /* 引用变量 */
    signed char *const externRunStatus = pCapture->ro._externRunStatus;
    ConfigManager * const pConfigManager = (ConfigManager*) pCapture->ro._pConfigManager;
    FsObjectList * const itemList_ = pCapture->ro.__itemList_;
    ////////////////////////////////////////////////////////////////////////////
    /*
     * 1类数据:可在线程标记为结束状态后释放或析构数据,使用*_clean1_*标记;
     * 2类数据:需要在线程标记为结束状态前释放或析构数据,使用*_clean2_*标记;
     * 未标记的数据可能不需要析构或需要在处理完1类数据前手动处理(可在2类数据前或2类数据后); 
     */
    ////////////////////////////////////////////////////////////////////////////
    /* 格式化变量 */
    /* 共享buffer,本函数不使用,只在子函数中使用 */
    FsShareBuffer shareBuffer = NULL;
#define __capture_P_T_clean1_1 {if(shareBuffer!=NULL)fsFree(shareBuffer);}
    /* 不可跨线程的缓存buffer */
    //    FsObjectSingleBuffer singleBuffer;
    //    fs_ObjectSingleBuffer_init(&singleBuffer, 2);
#define __capture_P_T_clean1_2 //fs_ObjectSingleBuffer_release(&singleBuffer);
    /* 可跨线程的缓存buffer,一般用于收数据或传递数据到其他线程 */
    FsObjectBaseBuffer baseBuffer;
    fs_ObjectBaseBuffer_init(&baseBuffer, 1);
#define __capture_P_T_clean1_3 fs_ObjectBaseBuffer_release(&baseBuffer);
    ////////////////////////////////////////////////////////////////////////////
    /* 全局变量 */
    // 通道数
    unsigned int channelCount = 0;
    unsigned int sleepTime = 0;
    // 相机对象
    FsObjectList * const camera_rtsp_list_ = fs_ObjectList_new_unsafe__IO(2);
#define __capture_P_T_clean1_4 {if(camera_rtsp_list_->nodeCount!=0){FsLog(FsLogType_fatal,FsPrintfIndex, "Error Camera_rtsp,count=%lu.\n", camera_rtsp_list_->nodeCount); fflush(stdout);FsLogExit();}fs_ObjectList_delete_unsafe__OI(camera_rtsp_list_);};

#define __capture_P_T_clean1_5
    FsObjectList * const camera_gb28181_list_ = fs_ObjectList_new_unsafe__IO(2);
#define __capture_P_T_clean1_6 {if(camera_rtsp_list_->nodeCount!=0){FsLog(FsLogType_fatal,FsPrintfIndex, "Error Camera_gb28181,count=%lu.\n", camera_gb28181_list_->nodeCount); fflush(stdout);FsLogExit();}fs_ObjectList_delete_unsafe__OI(camera_gb28181_list_);};
    struct Camera_inflray * pCamera_inflray = NULL;
    Camera_ctrl * pCamera_ctrl = NULL;
    struct pollfd __fds = {pCapture->ro.__pipe[0], POLLIN, 0};
    ////////////////////////////////////////////////////////////////////////////
    /* 初始化一些变量 */
    /* 获取流控制状态的客户端链表 */
    pCapture->p.streamCtrlStatusClientList = fs_StructList_new__IO(6, sizeof (int)*6);
#define __capture_P_T_clean1_7 {configManager_conncet_refer_close(pConfigManager, pCapture->p.streamCtrlStatusClientList, 1);\
    fs_StructList_delete__OI(pCapture->p.streamCtrlStatusClientList);}
    /* 储存已开始流的相机指针 */
    pCapture->p.streamCtrlStartItemListOrder__streamCtrlStatusClientList = fs_ObjectList_new_unsafe__IO(6);
#ifdef FsDebug
#define __capture_P_T_clean1_8 {if(pCapture->p.streamCtrlStartItemListOrder__streamCtrlStatusClientList->nodeCount!=0)\
    FsPrintf(FsPrintfIndex,"streamCtrlStartItemListOrder__streamCtrlStatusClientList->nodeCount=%lu!=0\n",pCapture->p.streamCtrlStartItemListOrder__streamCtrlStatusClientList->nodeCount),fflush(stdout);\
    fs_ObjectList_delete_unsafe__OI(pCapture->p.streamCtrlStartItemListOrder__streamCtrlStatusClientList);}
#else
#define __capture_P_T_clean1_8 {fs_ObjectList_delete_unsafe__OI(pCapture->p.streamCtrlStartItemListOrder__streamCtrlStatusClientList);}
#endif 
#define __capture_P_T_clean1_count 8
    // 注册连接断开时的回调通知
    configManager_connect_error_register(pConfigManager, (int(*)(const unsigned int*, void*, char**))capture_P_cb_connect_error, pCapture);
#define __capture_P_T_clean2_1 {configManager_connect_error_logoff(pConfigManager, (int(*)(const unsigned int*, void*, char**))capture_P_cb_connect_error, pCapture);}
    /* 注册获取线程信息的的命令字,cmd+uuid+ipv4必须是唯一值 */
    configManager_cmd_register_and_protocol_publish(pConfigManager, "streamCtrlStatus_get", "capture", 0, pCapture, 0
            , (int (* const) (const unsigned int *, unsigned int, FsEbml * const, char, void * const, FsObjectBaseBuffer * const, char * * const))capture_P_cmd_cb_streamCtrlStatus_get, NULL, pCapture
            ////////////////////////////////////////////////////////////////
            , "streamCtrlStatus_get", "流控制状态获取", ConfigManager_Module5_protocol_classIndex0, capture_P_protocol_streamCtrlStatus_get, &shareBuffer);
#define __capture_P_T_clean2_2 {configManager_cmd_logoff_and_protocol_cancel(pConfigManager, "streamCtrlStatus_get", "capture", 0, pCapture,"streamCtrlStatus_get",&shareBuffer);}
    // 根据
    // 注册命令字
    configManager_protocol_publish(pConfigManager, "cameractrl_release", "释放相机控制权限", 1, capture_P_protocol_ctrlRelease);
#define __capture_P_T_clean2_3 {configManager_protocol_cancel(pConfigManager, "cameractrl_release");}
    configManager_protocol_publish(pConfigManager, "cameractrl_maskget", "相机控制掩码获取", 1, capture_P_protocol_ctrlmaskget);
#define __capture_P_T_clean2_4 {configManager_protocol_cancel(pConfigManager, "cameractrl_maskget");}
    configManager_protocol_publish(pConfigManager, "cameractrl_turn", "控制相机转动", 1, capture_P_protocol_turn);
#define __capture_P_T_clean2_5 {configManager_protocol_cancel(pConfigManager, "cameractrl_turn");}
    configManager_protocol_publish(pConfigManager, "cameractrl_zoom", "控制相机变倍", 1, capture_P_protocol_zoom);
#define __capture_P_T_clean2_6 {configManager_protocol_cancel(pConfigManager, "cameractrl_zoom");}
    configManager_protocol_publish(pConfigManager, "cameractrl_focus", "控制相机聚焦", 1, capture_P_protocol_focus);
#define __capture_P_T_clean2_7 {configManager_protocol_cancel(pConfigManager, "cameractrl_focus");}
    configManager_protocol_publish(pConfigManager, "cameractrl_preset_set", "控制相机预置位设置", 1, capture_P_protocol_preset_set);
#define __capture_P_T_clean2_8 {configManager_protocol_cancel(pConfigManager, "cameractrl_preset_set");}
    configManager_protocol_publish(pConfigManager, "cameractrl_preset_goto", "控制相机预置位调用", 1, capture_P_protocol_preset_goto);
#define __capture_P_T_clean2_9 {configManager_protocol_cancel(pConfigManager, "cameractrl_preset_goto");}
    configManager_protocol_publish(pConfigManager, "cameractrl_preset_clean", "控制相机预置位清除", 1, capture_P_protocol_preset_clean);
#define __capture_P_T_clean2_10 {configManager_protocol_cancel(pConfigManager, "cameractrl_preset_clean");}
    configManager_protocol_publish(pConfigManager, "cameractrl_zoom3D", "控制相机3D定位", 1, capture_P_protocol_zoom3D);
#define __capture_P_T_clean2_11 {configManager_protocol_cancel(pConfigManager, "cameractrl_zoom3D");}
    configManager_protocol_publish(pConfigManager, "cameractrl_ptzgoto", "控制相机设置PTZ", 1, capture_P_protocol_ptzgoto);
#define __capture_P_T_clean2_12 {configManager_protocol_cancel(pConfigManager, "cameractrl_ptzgoto");}
    configManager_protocol_publish(pConfigManager, "cameractrl_ptzget", "控制相机获取PTZ", 1, capture_P_protocol_ptzget);
#define __capture_P_T_clean2_13 {configManager_protocol_cancel(pConfigManager, "cameractrl_ptzget");}
    configManager_protocol_publish(pConfigManager, "cameractrl_imageinfoset", "控制相机图像信息设置", 1, capture_P_protocol_imageinfoset);
#define __capture_P_T_clean2_14 {configManager_protocol_cancel(pConfigManager, "cameractrl_imageinfoset");}
    configManager_protocol_publish(pConfigManager, "cameractrl_imageinfoget", "控制相机图像信息获取", 1, capture_P_protocol_imageinfoget);
#define __capture_P_T_clean2_15 {configManager_protocol_cancel(pConfigManager, "cameractrl_imageinfoget");}
    configManager_protocol_publish(pConfigManager, "cameractrl_chipCmd", "控制相机向机芯直接发送命令", 1, capture_P_protocol_chipCmd);
#define __capture_P_T_clean2_16 {configManager_protocol_cancel(pConfigManager, "cameractrl_chipCmd");}
    configManager_protocol_publish(pConfigManager, "cameractrl_shutterReset", "控制相机重置快门", 1, capture_P_protocol_shutterReset);
#define __capture_P_T_clean2_17 {configManager_protocol_cancel(pConfigManager, "cameractrl_shutterReset");}
    configManager_protocol_publish(pConfigManager, "errorInfo", "故障信息获取", 1, capture_P_protocol_errorInfo);
#define __capture_P_T_clean2_18 {configManager_protocol_cancel(pConfigManager, "errorInfo");}

#define __capture_P_T_clean2_count 18

    ////////////////////////////////////////////////////////////////////////////
    /* 主循环 */
    while (pCapture->rw.runStatus == *externRunStatus) {
        pMonitorItem->rw.cycleAlive = 1;
        monitorItem_checkStack(pMonitorItem);
        if (sleepTime < 1000000)sleepTime = (sleepTime + 1) << 1;
        ////////////////////////////////////////////////////////////////////////
        if (pCapture->rw._resetChannel>-1) {
            sleepTime = 1000;
            /* 读取通道数 */
            if (0 == pCapture->rw._resetChannel) {
                channelCount = capture_P_get_channelCount(pConfigManager->ro.__pConfig, pConfigManager->ro.__ipList_local);
                /* 删除通道 */
                while (itemList_->nodeCount > channelCount) {
                    struct Capture_item * const pCapture_item = (struct Capture_item *) fs_ObjectList_remove_tail(itemList_);
                    if (pCapture_item)capture_P_item_delete__OI(pCapture_item, &pCamera_ctrl, &baseBuffer, &shareBuffer);
                }
                /* 添加通道 */
                while (itemList_->nodeCount < channelCount) fs_ObjectList_insert_tail(itemList_, NULL);
                /* 重置集群 */
                pCapture->rw._resetChannel = -1;

                capture_P_item_new(pCapture, 0, pConfigManager->ro.__ipList_local
                        , NULL, camera_rtsp_list_
                        , camera_gb28181_list_
                        , &pCamera_inflray, &pCamera_ctrl, pMonitorItem, &baseBuffer, &shareBuffer);
            }
            /* 重置 */
            if (pCapture->rw._resetChannel > 0) {
                if (itemList_->nodeCount < (unsigned int) pCapture->rw._resetChannel) {
                    /* 出错 */
                    FsPrintf(1, "Invalid channel(=%u/%lu).\n", pCapture->rw._resetChannel, itemList_->nodeCount);
                    fflush(stdout);
                }
                pMonitorItem->rw.line = __LINE__;
                capture_P_item_new(pCapture, pCapture->rw._resetChannel, pConfigManager->ro.__ipList_local
                        , (struct Capture_item **) (itemList_->pNode + itemList_->startIndex + pCapture->rw._resetChannel - 1), camera_rtsp_list_
                        , camera_gb28181_list_
                        , &pCamera_inflray, &pCamera_ctrl, pMonitorItem, &baseBuffer, &shareBuffer);
                pMonitorItem->rw.line = __LINE__;
                pCapture->rw._resetChannel = -1;
            }
        }
        ////////////////////////////////////////////////////////////////////////
        // 在这里进行相机所有操作....
        {
            struct Capture_item **pCapture_itemNode = (struct Capture_item **) itemList_->pNode + itemList_->startIndex;
            int channelindex = channelCount;
            const unsigned char netClientHasError = pCapture->p.netClientHasError;
            pCapture->p.netClientHasError = 0;
            for (; channelindex > 0; channelindex--) {
                struct Capture_item * const pCapture_item = *pCapture_itemNode++;
                if (pCapture_item == NULL)continue;
                ////////////////////////////////////////////////////////////////                
                if (netClientHasError) {
                    /* 检查各相机控制的客户端连接是否已断开 */
                    if (pCapture_item->p.__ctrlClientList != NULL && pCapture_item->p.__ctrlClientList->nodeCount > 0) {
                        pthread_mutex_lock(&pCapture_item->p.__ctrlClientList->mutex);
                        if (pCapture_item->p.__ctrlClientList->nodeCount > 0) {
                            configManager_conncet_refer_check(pConfigManager, pCapture_item->p.__ctrlClientList);
                            if (0 == pCapture_item->p.__ctrlClientList->nodeCount) {
                                pCapture_item->ro._ctrl_function->cancelKey_pthreadSafety(pCapture_item->ro.__pCamera_item, pCapture_item->ro.__pCamera_ctrl_item, pCapture_item, &baseBuffer, &shareBuffer);
                                pCapture_item->p.ctrlPriority = 0;
                            }
                        }
                        pthread_mutex_unlock(&pCapture_item->p.__ctrlClientList->mutex);
                    }
                    /* 检查获取故障代码的客户端连接是否已断开 */
                    if (pCapture_item->p.__errorInfoClientList != NULL && pCapture_item->p.__errorInfoClientList->nodeCount > 0) {
                        pthread_mutex_lock(&pCapture_item->p.__errorInfoClientList->mutex);
                        configManager_conncet_refer_check(pConfigManager, pCapture_item->p.__errorInfoClientList);
                        pthread_mutex_unlock(&pCapture_item->p.__errorInfoClientList->mutex);
                    }
                }
                ////////////////////////////////////////////////////////////////
                if (1) {

                    if (pCapture_item->rw.m_action > 0) {

                        switch (pCapture_item->rw.m_action) {
                            case 1: //向右
                            {
                                int re = pCapture_item->ro._camera_right(pCapture_item, pCapture_item->rw.camera_speed, pCapture_item->rw.camera_time);
                                if (re == -1)
                                    pCapture_item->ro._camera_right(pCapture_item, pCapture_item->rw.camera_speed, pCapture_item->rw.camera_time);
                            }
                                break;
                            case 2: //向左
                            {
                                int re = pCapture_item->ro._camera_left(pCapture_item, pCapture_item->rw.camera_speed, pCapture_item->rw.camera_time);
                                if (re == -1)
                                    pCapture_item->ro._camera_left(pCapture_item, pCapture_item->rw.camera_speed, pCapture_item->rw.camera_time);
                            }
                                break;
                            case 3: //向上
                            {
                                int re = pCapture_item->ro._camera_up(pCapture_item, pCapture_item->rw.camera_speed, pCapture_item->rw.camera_time);
                                if (re == -1)
                                    pCapture_item->ro._camera_up(pCapture_item, pCapture_item->rw.camera_speed, pCapture_item->rw.camera_time);
                            }
                                break;
                            case 4: //向下
                            {
                                int re = pCapture_item->ro._camera_down(pCapture_item, pCapture_item->rw.camera_speed, pCapture_item->rw.camera_time);
                                if (re == -1)
                                    pCapture_item->ro._camera_down(pCapture_item, pCapture_item->rw.camera_speed, pCapture_item->rw.camera_time);
                            }
                                break;
                            case 5: //左上
                            {
                                FsPrintf(1, "*************_uuid:%s  m_action:_camera_leftup**********\n", pCapture_item->ro._uuid);
                                fflush(stdout);
                                int re = pCapture_item->ro._camera_leftup(pCapture_item, pCapture_item->rw.camera_speed, pCapture_item->rw.camera_time);
                                if (re == -1)
                                    pCapture_item->ro._camera_leftup(pCapture_item, pCapture_item->rw.camera_speed, pCapture_item->rw.camera_time);
                            }
                                break;
                            case 6: //左下
                            {
                                int re = pCapture_item->ro._camera_leftdown(pCapture_item, pCapture_item->rw.camera_speed, pCapture_item->rw.camera_time);
                                if (re == -1)
                                    pCapture_item->ro._camera_leftdown(pCapture_item, pCapture_item->rw.camera_speed, pCapture_item->rw.camera_time);
                            }
                                break;
                            case 7: //右上
                            {
                                int re = pCapture_item->ro._camera_rightup(pCapture_item, pCapture_item->rw.camera_speed, pCapture_item->rw.camera_time);
                                if (re == -1)
                                    pCapture_item->ro._camera_rightup(pCapture_item, pCapture_item->rw.camera_speed, pCapture_item->rw.camera_time);
                            }
                                break;
                            case 8: //右下
                            {
                                int re = pCapture_item->ro._camera_rightdown(pCapture_item, pCapture_item->rw.camera_speed, pCapture_item->rw.camera_time);
                                if (re == -1)
                                    pCapture_item->ro._camera_rightdown(pCapture_item, pCapture_item->rw.camera_speed, pCapture_item->rw.camera_time);
                            }
                                break;
                            case 9: //变倍大
                            {
                                int re = pCapture_item->ro._camera_zoomnear(pCapture_item, pCapture_item->rw.camera_speed, pCapture_item->rw.camera_time);
                                if (re == -1)
                                    pCapture_item->ro._camera_zoomnear(pCapture_item, pCapture_item->rw.camera_speed, pCapture_item->rw.camera_time);
                            }
                                break;
                            case 10: //变倍小
                            {
                                int re = pCapture_item->ro._camera_zoomfar(pCapture_item, pCapture_item->rw.camera_speed, pCapture_item->rw.camera_time);
                                if (re == -1)
                                    pCapture_item->ro._camera_zoomfar(pCapture_item, pCapture_item->rw.camera_speed, pCapture_item->rw.camera_time);
                            }
                                break;
                            case 11: //聚焦远
                            {
                                int re = pCapture_item->ro._camera_focusfar(pCapture_item, pCapture_item->rw.camera_speed, pCapture_item->rw.camera_time);
                                if (re == -1)
                                    pCapture_item->ro._camera_focusfar(pCapture_item, pCapture_item->rw.camera_speed, pCapture_item->rw.camera_time);
                            }
                                break;
                            case 12: //聚焦近
                            {
                                int re = pCapture_item->ro._camera_focusnear(pCapture_item, pCapture_item->rw.camera_speed, pCapture_item->rw.camera_time);
                                if (re == -1)
                                    pCapture_item->ro._camera_focusnear(pCapture_item, pCapture_item->rw.camera_speed, pCapture_item->rw.camera_time);
                            }
                                break;
                            case 13: //光圈大
                            {
                                int re = pCapture_item->ro._camera_iris_open(pCapture_item, pCapture_item->rw.camera_speed, pCapture_item->rw.camera_time);
                                if (re == -1)
                                    pCapture_item->ro._camera_iris_open(pCapture_item, pCapture_item->rw.camera_speed, pCapture_item->rw.camera_time);
                            }
                                break;
                            case 14: //光圈小
                            {
                                int re = pCapture_item->ro._camera_iris_close(pCapture_item, pCapture_item->rw.camera_speed, pCapture_item->rw.camera_time);
                                if (re == -1)
                                    pCapture_item->ro._camera_iris_close(pCapture_item, pCapture_item->rw.camera_speed, pCapture_item->rw.camera_time);
                            }
                                break;
                            case 15: //停止码
                            {
                                FsPrintf(1, "*************_uuid:%s  m_action:_camera_stopturn**********\n", pCapture_item->ro._uuid);
                                fflush(stdout);
                                pCapture_item->rw.m_stopAction = pCapture_item->ro._camera_stopturn(pCapture_item, pCapture_item->rw.camera_speed, pCapture_item->rw.camera_time);
                                if (pCapture_item->rw.m_stopAction == -1)
                                    pCapture_item->rw.m_stopAction = pCapture_item->ro._camera_stopturn(pCapture_item, pCapture_item->rw.camera_speed, pCapture_item->rw.camera_time);

                                usleep(1000 * 20);
                                pCapture_item->ro._camera_getptz(pCapture_item, &pCapture_item->rw.camera_c_p, &pCapture_item->rw.camera_c_t, &pCapture_item->rw.camera_c_z);
                                FsPrintf(1, "*************_uuid:%s  m_action:_camera_getptz**********\n", pCapture_item->ro._uuid);
                                fflush(stdout);
                            }
                                break;
                            case 16: //自动除雾开     
                            {
                                int re = pCapture_item->ro._camera_throughfrog(pCapture_item, pCapture_item->rw.camera_speed, pCapture_item->rw.camera_time);
                                if (re == -1)
                                    pCapture_item->ro._camera_throughfrog(pCapture_item, pCapture_item->rw.camera_speed, pCapture_item->rw.camera_time);
                            }
                                break;
                            case 17: //自动除雾关                          
                                break;
                            case 18: //激光器开
                                break;
                            case 19: //激光器关
                                break;
                            case 20: //goto ptz      
                            {
                                int re = pCapture_item->ro._camera_gotoptz(pCapture_item, pCapture_item->rw.camera_speed, pCapture_item->rw.goto_p, pCapture_item->rw.goto_t,
                                        pCapture_item->rw.goto_z, pCapture_item->rw.camera_time);
                                if (re == -1)
                                    pCapture_item->ro._camera_gotoptz(pCapture_item, pCapture_item->rw.camera_speed, pCapture_item->rw.goto_p, pCapture_item->rw.goto_t,
                                        pCapture_item->rw.goto_z, pCapture_item->rw.camera_time);
                            }
                                break;
                            case 21:
                                //pCapture_item->ro._camera_getptz(pCapture_item,&pCapture_item->rw.camera_c_p,&pCapture_item->rw.camera_c_t,&pCapture_item->rw.camera_c_z);
                                break;
                            case 22: //红光开
                            {
                                int re = pCapture_item->ro._Redcamera_open(pCapture_item, pCapture_item->rw.camera_speed, pCapture_item->rw.camera_time);
                                if (re == -1)
                                    pCapture_item->ro._Redcamera_open(pCapture_item, pCapture_item->rw.camera_speed, pCapture_item->rw.camera_time);
                            }
                                break;
                            case 23: //红光关
                            {
                                int re = pCapture_item->ro._Redcamera_close(pCapture_item, pCapture_item->rw.camera_speed, pCapture_item->rw.camera_time);
                                if (re == -1)
                                    pCapture_item->ro._Redcamera_close(pCapture_item, pCapture_item->rw.camera_speed, pCapture_item->rw.camera_time);
                            }
                                break;
                            default:
                                break;
                        }

                        //如果需要自动聚焦，且不是gotoptz指令(函数自带)，在这里调用自动聚焦函数
                        if (pCapture_item->rw.m_auto_focus == 1 && pCapture_item->rw.m_action != 20) {
                            int re = pCapture_item->ro._camera_autofocus(pCapture_item, pCapture_item->rw.camera_speed, pCapture_item->rw.camera_time);
                            if (re == -1)
                                pCapture_item->ro._camera_autofocus(pCapture_item, pCapture_item->rw.camera_speed, pCapture_item->rw.camera_time);
                        }


                        //关闭socket
                        camera_disconnect(pCapture_item->p.camera_socket);
                        pCapture_item->p.camera_socket = -1;

                        pCapture_item->rw.m_action = 0;
                    }
                }
            }
        }
        if (poll(&__fds, 1, sleepTime / 1000) > 0) {
            FsPrintf(1, "***********select*********\n");
            fflush(stdout);
            if (read(__fds.fd, &__fds.revents, 1) != 1) {
                FsPrintf(6, "Read error:\"%s\".", strerror(errno));
                fflush(stdout);
            }
        }
        /////////////////////////////////////////////////////////////////////
    }
    pMonitorItem->rw.line = __LINE__;
    ////////////////////////////////////////////////////////////////////////////
    if (pCapture->rw._resetChannel != -1) {
        FsPrintf(FsPrintfIndex, "Invalid resetChannel(=%d)", pCapture->rw._resetChannel);
        fflush(stdout);
    }
    /* 2类数据:需要在线程标记为结束状态前释放或析构数据,使用*_clean2_*标记 */
    pMonitorItem->rw.line = __LINE__;
    //FsPrintf(FsPrintfIndex,"%s\n", FsMacrosString(FsClean(2))); // 打印析构脚本
    FsClean(2);
    pMonitorItem->rw.line = __LINE__;
    //________________________________________________________________________// 
    /* 删除各通道 */
    {
        {
            struct Capture_item **ppNode = (struct Capture_item **) itemList_->pNode + itemList_->startIndex;
            unsigned int ui = itemList_->nodeCount;
            for (; ui > 0; ui--) {
                struct Capture_item * const pCapture_item = *ppNode++;
                if (pCapture_item)capture_P_item_delete__OI(pCapture_item, &pCamera_ctrl, &baseBuffer, &shareBuffer);
            }
        }
        fs_ObjectList_clear(itemList_);
        /* 删除集群 */
        if (pCapture->p.clusterItemList_) {
            while (pCapture->p.clusterItemList_->nodeCount > 0)capture_P_item_delete__OI((struct Capture_item*) fs_ObjectList_remove_tail(pCapture->p.clusterItemList_), &pCamera_ctrl, &baseBuffer, &shareBuffer);
            fs_ObjectList_delete_unsafe__OI(pCapture->p.clusterItemList_);
            pCapture->p.clusterItemList_ = NULL;
        }
    }
    /* 释放空间 */
    /* 释放全局变量空间 */
    ////////////////////////////////////////////////////////////////////////////
    /* 1类数据:可在线程标记为结束状态后释放或析构数据,使用*_clean1_*标记 */
    pMonitorItem->rw.line = __LINE__;
    //FsPrintf(FsPrintfIndex,"%s\n", FsMacrosString(FsClean(1))); // 打印析构脚本
    FsClean(1);
    pMonitorItem->rw.line = __LINE__;
#ifdef FsDebug
    FsLog2(FsLogType_threadDebug, FsPrintfIndex, "Thread:\"%s\" exit,runStatus=%hhd.\n", Capture_get_threadName(pCapture), pCapture->rw.runStatus);
#endif 
    /* 删除线程监控 */
    monitorItem_remove_and_delete(pMonitor, pMonitorItem, NULL);
    if (NULL == pCapture->ro._pMonitor)monitor_delete__OI(pMonitor);

    return NULL;
}
///////////////////////////定义私有函数结束//////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
///////////////////////////定义公共函数开始//////////////////////////////////////

/* 创建配置 */
void capture_createConfig(FsConfig * const pConfig, /* 掩码 */const unsigned long long mask, /* 通道数 */const unsigned int channelCount, void * parent) {
    parent = fs_Config_node_node_add(pConfig, parent, "cameraConfig", "相机配置", "相机配置", 0, 0x7);
    {
        void *const pNode = fs_Config_node_string_add(pConfig, parent, "cameraType", "相机类型", "相机类型", 0, 0x3, 1, 16, 1);
        fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "rtsp", "rtsp相机", "rtsp相机");
        fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_optional, "gb28181", "gb28181平台或相机", "gb28181平台或相机");
        fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_optional, "inflray", "inflray相机", "inflray相机");
    }
    {
        void *const pNode = fs_Config_node_integer_add(pConfig, parent, "streamMask", "数据流掩码", "数据流掩码,1-视频,2-音频,4-相机控制", FsConfig_nodeShowType_default, 0, 0x7, 0, (0x1 << 3) - 1, 3);
        fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_default, 0x1 << 0, "视频", "视频");
        fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_optional, 0x1 << 1, "音频", "音频");
        fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_optional, 0x1 << 2, "相机控制", "相机控制");
        void *const conditionGroup = fs_Config_condition_group_add(pConfig, pNode);
        fs_Config_condition_add_static(pConfig, conditionGroup, 1, "cameraType", FsConfig_Condition_equal_false, "localPull");
    }
    {
        void *const pNode = fs_Config_node_string_add(pConfig, parent, "gb28181_id", "gb28181本机ID", "gb28181本机ID", 0, 0x7, 1, 20, 1);
        fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "34020000002000000001", "34020000002000000001", "34020000002000000001");
        fs_Config_condition_add_static(pConfig, fs_Config_condition_group_add(pConfig, pNode), 1, "cameraType", FsConfig_Condition_equal, "gb28181");
    }
    {
        void *const pNode = fs_Config_node_string_add(pConfig, parent, "gb28181_sip_id", "gb28181平台或相机ID", "gb28181平台或相机ID", 0, 0x7, 1, 20, 1);
        fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "34020000001320000001", "34020000001320000001", "34020000001320000001");
        fs_Config_condition_add_static(pConfig, fs_Config_condition_group_add(pConfig, pNode), 1, "cameraType", FsConfig_Condition_equal, "gb28181");
    }
    {
        void *const pNode = fs_Config_node_string_add(pConfig, parent, "gb28181_sip_domain", "gb28181平台或相机域", "gb28181平台或相机域,本机不校验此值", 0, 0x7, 1, 20, 1);
        fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "3402000000", "3402000000", "3402000000");
        fs_Config_condition_add_static(pConfig, fs_Config_condition_group_add(pConfig, pNode), 1, "cameraType", FsConfig_Condition_equal, "gb28181");
    }
    {
        void *const pNode = fs_Config_node_string_add(pConfig, parent, "ipv4", "相机或平台的IP或域名", "相机或平台的IP或域名,本机做gb28181平台上级时允许为空", 0, 0x7, 1, 32, 1);
        fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_optional, "192.168.50.252", "192.168.50.252", "192.168.50.252");
        void *const conditionGroup = fs_Config_condition_group_add(pConfig, pNode);
    }
    {
        void *const pNode = fs_Config_node_integer_add(pConfig, parent, "rtspPort", "rts[m]p或平台端口", "rts[m]p或平台端口,推流此处填0,本机做gb28181平台上级时允许为0", FsConfig_nodeShowType_default, 0, 0x7, 0, 65534, 1);
        fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_default, 554, "554", "554");
        fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_optional, 6666, "6666", "6666");
        void *const conditionGroup = fs_Config_condition_group_add(pConfig, pNode);
    }
    {
        void *const pNode = fs_Config_node_integer_add(pConfig, parent, "gb28181_sipOver", "sip连接协议", "sip连接协议", FsConfig_nodeShowType_default, 0, 0x7, 1, 3, 2);
        fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_default, 1 << 0, "udp", "udp");
        fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_optional, 1 << 1, "tcp", "tcp");
        void *const conditionGroup = fs_Config_condition_group_add(pConfig, pNode);
        fs_Config_condition_add_static(pConfig, conditionGroup, 1, "cameraType", FsConfig_Condition_equal, "gb28181");
        fs_Config_condition_add_static(pConfig, conditionGroup, 1, "ipv4", FsConfig_Condition_equal_false, NULL);
        fs_Config_condition_add_static(pConfig, conditionGroup, 1, "rtspPort", FsConfig_Condition_equal_false, "0");
    }
    {
        void *const pNode = fs_Config_node_string_add(pConfig, parent, "rtspURL", "rts[m]pURL或平台的通道编号", "rts[m]pURL,必须以/开头,不能为空", 0, 0x7, 1, 128, 1);
        fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "/", "/", "/");
        fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_optional, "34020000001320000002", "34020000001320000002", "34020000001320000002");
        void *const conditionGroup = fs_Config_condition_group_add(pConfig, pNode);
        fs_Config_condition_add_static(pConfig, conditionGroup, 1, "cameraType", FsConfig_Condition_equal_false, "inflray");
    }
    {
        void *const pNode = fs_Config_node_string_add(pConfig, parent, "rtspURL2", "rtsp子码流的URL", "rtsp子码流的URL,必须以/开头,为空表示不采集子码流", 0, 0x7, 1, 128, 1);
        fs_Config_condition_add_static(pConfig, fs_Config_condition_group_add(pConfig, pNode), 1, "cameraType", FsConfig_Condition_equal, "rtsp");
    }
    {
        void *const pNode = fs_Config_node_string_add(pConfig, parent, "rstpUsername", "rtsp或平台用户名", "rtsp或平台用户名,可为空", 0, 0x7, 0, 128, 1);
        fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_optional, "34020000001320000001", "34020000001320000001", "34020000001320000001");
        void *const conditionGroup = fs_Config_condition_group_add(pConfig, pNode);
        fs_Config_condition_add_static(pConfig, conditionGroup, 1, "cameraType", FsConfig_Condition_equal_false, "inflray");
    }
    {
        void *const pNode = fs_Config_node_string_add(pConfig, parent, "rstpPasswd", "rtsp或平台密码", "rtsp或平台密码,可为空", 0, 0x7, 0, 128, 1);
        fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_optional, "12345678", "12345678", "12345678");
        void *const conditionGroup = fs_Config_condition_group_add(pConfig, pNode);
        fs_Config_condition_add_static(pConfig, conditionGroup, 1, "cameraType", FsConfig_Condition_equal_false, "inflray");
    }
    {
        void *const pNode = fs_Config_node_integer_add(pConfig, parent, "streamOver", "流优先传输协议", "流优先传输协议,优先使用哪种协议传输数据流", FsConfig_nodeShowType_default, 0, 0x7, 0, 0xF, 4);
        fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_default, 1, "tcp(gb中为被动模式)", "tcp(gb中为被动模式)");
        fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_optional, 2, "udp", "udp");
        fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_optional, 4, "kcp", "kcp");
        fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_optional, 8, "tcp主动模式", "tcp主动模式");
        fs_Config_condition_add_static(pConfig, fs_Config_condition_group_add(pConfig, pNode), 1, "cameraType", FsConfig_Condition_equal, "rtsp");
        fs_Config_condition_add_static(pConfig, fs_Config_condition_group_add(pConfig, pNode), 1, "cameraType", FsConfig_Condition_equal, "gb28181");

    }
    {
        void *const pNode = fs_Config_node_integer_add(pConfig, parent, "controlMask", "控制协议", "控制协议", FsConfig_nodeShowType_default, 0, 0x7, 0, 0xF, 3);
        fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_default, 0xF, "默认", "默认");
        fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_optional, 0x1 << 0, "onvif", "onvif");
        fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_optional, 0x1 << 1, "宇视HTTP", "宇视HTTP");
        fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_optional, 0x1 << 2, "海康Http相机", "海康Http相机");
        fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_optional, 0x1 << 3, "Inflray相机", "Inflray相机");
        {
            void *const condition_group = fs_Config_condition_group_add(pConfig, pNode);
            fs_Config_condition_add_static(pConfig, condition_group, 1, "streamMask", FsConfig_Condition_orAnd, "4");
            fs_Config_condition_add_static(pConfig, condition_group, 1, "cameraType", FsConfig_Condition_equal, "rtsp");
        }
        {
            void *const condition_group = fs_Config_condition_group_add(pConfig, pNode);
            fs_Config_condition_add_static(pConfig, condition_group, 1, "streamMask", FsConfig_Condition_orAnd, "4");
            fs_Config_condition_add_static(pConfig, condition_group, 1, "cameraType", FsConfig_Condition_equal, "rtmp");
        }
    }
    {
        void *const pNode = fs_Config_node_integer_add(pConfig, parent, "controlMaskDefault", "支持的控制类型", "支持的控制类型,0-表示自动探测,0x1表示探测结果最多只能包含任意转", FsConfig_nodeShowType_default, 0, 0x7, -(0x8000000000000000LLU), 0x7FFFFFFFFFFFFFFFLLU, 9);
        fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_default, 0, "默认", "默认");
        fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_optional, 0x1LLU << Camera_ctrl_Mask_check_disabled, "63<<禁用控制类型自动探测", "63<<禁用控制类型自动探测");
        fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_optional, 0x1LLU << Camera_ctrl_Mask_turnAutoStop_disabled, "62<<禁用转动自动停止", "62<<禁用转动自动停止");
        fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_optional, 0x1LLU << Camera_ctrl_Mask_turn, "0<<任意转", "0<<任意转");
        fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_optional, 0x1LLU << Camera_ctrl_Mask_turn_0, "1<<左右转", "1<<左右转");
        fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_optional, 0x1LLU << Camera_ctrl_Mask_turn_90, "2<<上下转", "2<<上下转");
        fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_optional, 0x1LLU << Camera_ctrl_Mask_turn_45, "3<<45度斜角转", "3<<45度斜角转");
        fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_optional, 0x1LLU << Camera_ctrl_Mask_Zoom, "4<<变倍", "4<<变倍");
        fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_optional, 0x1LLU << Camera_ctrl_Mask_Focus, "5<<聚焦", "5<<聚焦");
        fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_optional, 0x1LLU << Camera_ctrl_Mask_Preset, "6<<预置位", "6<<预置位");
        fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_optional, 0x1LLU << Camera_ctrl_Mask_zoom3D, "7<<3d定位", "7<<3d定位");
        fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_optional, 0x1LLU << Camera_ctrl_Mask_PTZ_get_set, "8<<gotoPTZ和getPTZ", "8<<gotoPTZ和getPTZ");
        fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_optional, 0x1LLU << Camera_ctrl_Mask_ImageInfo, "9<<图像参数设置与获取", "9<<图像参数设置与获取");
        fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_optional, 0x1LLU << Camera_ctrl_Mask_chipCmd, "10<<直接向机芯发送命令", "10<<直接向机芯发送命令");
        {
            void *const conditionGroup = fs_Config_condition_group_add(pConfig, pNode);
            fs_Config_condition_add_static(pConfig, conditionGroup, 1, "streamMask", FsConfig_Condition_orAnd, "4");
        }
    }
    {
        void *const pNode = fs_Config_node_string_add(pConfig, parent, "channelUUID", "控制标识", "控制标识,可为空,对同一ip有多相机的控制可能需要些参数", 0, 0x7, 0, 128, 1);
        {
            void *const condition_group = fs_Config_condition_group_add(pConfig, pNode);
            fs_Config_condition_add_static(pConfig, condition_group, 1, "streamMask", FsConfig_Condition_orAnd, "4");
            fs_Config_condition_add_static(pConfig, condition_group, 1, "cameraType", FsConfig_Condition_equal, "rtsp");
            fs_Config_condition_add_static(pConfig, condition_group, 1, "controlMask", FsConfig_Condition_equal, "tt");
        }
        {
            void *const condition_group = fs_Config_condition_group_add(pConfig, pNode);
            fs_Config_condition_add_static(pConfig, condition_group, 1, "streamMask", FsConfig_Condition_orAnd, "4");
            fs_Config_condition_add_static(pConfig, condition_group, 1, "cameraType", FsConfig_Condition_equal, "rtmp");
            fs_Config_condition_add_static(pConfig, condition_group, 1, "controlMask", FsConfig_Condition_equal, "tt");
        }
    }
    {
        void *const pNode = fs_Config_node_string_add(pConfig, parent, "holderipv4", "云台IP", "云台IP,为空表示使用取流的ip", 0, 0x7, 1, 16, 1);
        fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_optional, "192.168.50.252", "192.168.50.252", "192.168.50.252");
        fs_Config_condition_add_static(pConfig, fs_Config_condition_group_add(pConfig, pNode), 1, "controlMask", FsConfig_Condition_equal, "tt");
    }
    {
        void *const pNode = fs_Config_node_integer_add(pConfig, parent, "holderPort", "云台端口", "云台端口,不设置表示根据协议使用默认的端口", FsConfig_nodeShowType_default, 0, 0x7, 1, 65534, 1);
        fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_optional, 4001, "4001", "4001");
        fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_optional, 8000, "8000", "8000");
        fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_optional, 80, "80", "80");
        fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_optional, 1984, "1984", "1984");
        {
            void *const condition_group = fs_Config_condition_group_add(pConfig, pNode);
            fs_Config_condition_add_static(pConfig, condition_group, 1, "streamMask", FsConfig_Condition_orAnd, "4");
            fs_Config_condition_add_static(pConfig, condition_group, 1, "cameraType", FsConfig_Condition_equal, "rtsp");
            fs_Config_condition_add_static(pConfig, condition_group, 1, "controlMask", FsConfig_Condition_orAnd, "1");
        }
        {
            void *const condition_group = fs_Config_condition_group_add(pConfig, pNode);
            fs_Config_condition_add_static(pConfig, condition_group, 1, "streamMask", FsConfig_Condition_orAnd, "4");
            fs_Config_condition_add_static(pConfig, condition_group, 1, "cameraType", FsConfig_Condition_equal, "rtmp");
            fs_Config_condition_add_static(pConfig, condition_group, 1, "controlMask", FsConfig_Condition_orAnd, "1");
        }
        {
            void *const condition_group = fs_Config_condition_group_add(pConfig, pNode);
            fs_Config_condition_add_static(pConfig, condition_group, 1, "streamMask", FsConfig_Condition_orAnd, "4");
            fs_Config_condition_add_static(pConfig, condition_group, 1, "cameraType", FsConfig_Condition_equal, "inflray");
        }
    }
    {
        void *const pNode = fs_Config_node_integer_add(pConfig, parent, "cameraBrand", "相机品牌", "相机品牌", FsConfig_nodeShowType_default, 0, 0x7, 0, 65534, 1);
        fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_default, 0, "海康威视", "海康威视");
        fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_optional, 1, "中光学", "中光学");
        fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_optional, 2, "上赛", "上赛");
        fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_optional, 3, "霍尼韦尔", "霍尼韦尔");
        fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_optional, 4, "南方海岸光电仪", "南方海岸光电仪");
        // fs_Config_condition_add_static(pConfig, fs_Config_condition_group_add(pConfig, pNode), 1, "cameraType", FsConfig_Condition_equal, "rtsp");
        fs_Config_condition_add_static(pConfig, fs_Config_condition_group_add(pConfig, pNode), 1, "controlMask", FsConfig_Condition_equal, "tt");
    }
    {
        void *const pNode = fs_Config_node_integer_add(pConfig, parent, "cameraModel", "相机型号", "相机型号:0-白光球机,1-红外球机,2-白光枪机,3-红外枪机,4-白光光电仪,5-红外光电仪,6-白光全景,7-红外全景", FsConfig_nodeShowType_default, 0, 0x7, 0, 65534, 1);
        fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_default, 0, "白光球机", "白光球机");
        fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_optional, 1, "红外球机", "红外球机");
        fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_optional, 2, "白光枪机", "白光枪机");
        fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_optional, 3, "红外枪机", "红外枪机");
        fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_optional, 4, "白光光电仪", "白光光电仪");
        fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_optional, 5, "红外光电仪", "红外光电仪");
        fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_optional, 6, "白光全景", "白光全景");
        fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_optional, 7, "红外全景", "红外全景");
        //fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_default, 0, "0", "0");
        //fs_Config_condition_add_static(pConfig, fs_Config_condition_group_add(pConfig, pNode), 1, "cameraType", FsConfig_Condition_equal, "rtsp");
        fs_Config_condition_add_static(pConfig, fs_Config_condition_group_add(pConfig, pNode), 1, "controlMask", FsConfig_Condition_equal, "tt");
    }
    //    pNode = fs_Config_node_string_add(pConfig, parent, "ImageSize", "相机采集图像尺寸(w-h)", "相机采集图像尺寸(w-h)", 0, 0x7, 1, 16, 1);
    //    fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "1920-1080", "1920-1080", "1920-1080");
    //    fs_Config_condition_add_static(pConfig, fs_Config_condition_group_add(pConfig, pNode), 1, "cameraType", FsConfig_Condition_equal, "rtsp");

    //    pNode = fs_Config_node_integer_add(pConfig, parent, "ImageSize", "图像尺寸", "图像尺寸", FsConfig_nodeShowType_default, 0, 0x7, 0, 65534, 1);
    //    fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_default,  0, "白光球机", "白光球机");
    //    fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_optional, 1, "红外球机", "红外球机");
    //    fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_optional, 2, "白光枪机", "白光枪机");
    //    fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_optional, 3, "红外枪机", "红外枪机");
    //    fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_optional, 4, "白光光电仪", "白光光电仪");
    //    fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_optional, 5, "红外光电仪", "红外光电仪");
    //    fs_Config_condition_add_static(pConfig, fs_Config_condition_group_add(pConfig, pNode), 1, "cameraType", FsConfig_Condition_equal, "rtsp");

    {
        void *const pNode = fs_Config_node_integer_add(pConfig, parent, "shotType", "光电仪镜头类型(0：反比例，1：正比例)", "光电仪镜头类型", FsConfig_nodeShowType_default, 0, 0x7, 0, 65534, 1);
        fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_default, 0, "0", "0");
        fs_Config_condition_add_static(pConfig, fs_Config_condition_group_add(pConfig, pNode), 1, "cameraType", FsConfig_Condition_equal, "rtsp");
    }
    {
        void *const pNode = fs_Config_node_integer_add(pConfig, parent, "shutterRest", "快门重置间隔(S)", "快门重置间隔", FsConfig_nodeShowType_default, 0, 0x7, 0, 600, 1);
        fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_default, 0, "禁用", "禁用");
        fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_optional, 300, "300", "300");
        {
            void *const condition_group = fs_Config_condition_group_add(pConfig, pNode);
            fs_Config_condition_add_static(pConfig, condition_group, 1, "streamMask", FsConfig_Condition_orAnd, "4");
            fs_Config_condition_add_static(pConfig, condition_group, 1, "cameraType", FsConfig_Condition_equal, "inflray");
        }
    }
    // localPull/localPush相机的参数
    {
        {
#ifdef FsAuthority //(Camera_pull, "本地拉取相机;0-不包含;1-包含", 0) == 1
            void *const pNode = fs_Config_node_string_add(pConfig, parent, "resolution", "分辨率", "分辨率,必须是4的倍数,长宽之间用逗号隔开如 1920,1080", 0, 0x7, 1, 16, 1);
            fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "0,0", "自动,仅一个相机时适用", "自动,仅一个相机时适用");
            fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_optional, "1920,1080", "1920,1080", "1920,1080");
#endif
        }
        {
#ifdef FsAuthority //(Camera_pull, "本地拉取相机;0-不包含;1-包含", 0) == 1
            void *const pNode = fs_Config_node_float_add(pConfig, parent, "pull_fps", "帧率", "帧率,为0表示使用自动帧率", 0, 0x7, 0.0, 50.0, 1);
            fs_Config_node_float_add_value(pConfig, pNode, FsConfig_nodeValue_default, 0.0, "自动帧率", "自动帧率");
            fs_Config_node_float_add_value(pConfig, pNode, FsConfig_nodeValue_optional, 25.0, "25.0", "25.0");
#endif
        }
    }
}

/* 检查pCapture模块中指定通道的配置是否发生改变,改变返回1,未改变返回0 */
int capture_check_channel_changed(struct Capture * const pCapture, /* 通道编号,从0开始 */const unsigned int index
        , /* item0对应的描述节点,为空内部会重新查找 */const void *const item0, /* 调用者传入的配置中的一节点 */const void *const item
        , /* item节点的校验和 */ const unsigned long long sum, /* 调用者传入的节点路径如"vsys vsysChannel" */const char itemPath[]) {
#define __check_channel_changed_Server Capture
#ifndef __check_channel_changed 
    FsConfig * const pConfig = ((ConfigManager*) /* pRecognition */FsMacrosValue2(p, __check_channel_changed_Server)->ro._pConfigManager)->ro.__pConfig;
    struct /* Recognition_item */ FsMacrosValue2(__check_channel_changed_Server, _item) * const /* pRecognition_item */ FsMacrosValue3(p, __check_channel_changed_Server, _item)
            = (struct /* Recognition_item */ FsMacrosValue2(__check_channel_changed_Server, _item) *) /* pRecognition */FsMacrosValue2(p, __check_channel_changed_Server)->ro.
#ifdef __check_channel_changed_itemListLock
            __itemList
#else
            __itemList_
#endif
            ->pNode[/* pRecognition */FsMacrosValue2(p, __check_channel_changed_Server)->ro.
#ifdef __check_channel_changed_itemListLock
            __itemList
#else
            __itemList_
#endif
            ->startIndex + index];
    if (strcmp("vsys vsysChannel", itemPath) == 0) {
        if (NULL == /* pRecognition_item */ FsMacrosValue3(p, __check_channel_changed_Server, _item)) {
            if (/* Recognition */FsMacrosValue2(__check_channel_changed_Server, _Mask) != 0
                    && (/* Recognition */FsMacrosValue2(__check_channel_changed_Server, _Mask) & fs_Config_node_integer_get_mask(pConfig, item0, item, "moduleMask", NULL)) == 0)return 0;
            return 1;
        }
        if (sum == /* pRecognition_item */ FsMacrosValue3(p, __check_channel_changed_Server, _item)->ro._sum
#ifdef __check_channel_changed_checkTimeControl
                && fs_Config_get_sum_timeControl(pConfig, item0, item, __check_channel_changed_checkTimeControl) == /* pRecognition_item */ FsMacrosValue3(p, __check_channel_changed_Server, _item)->ro._timeControlSum
#endif
                ) return 0;
    } else {
        FsLog(FsLogType_error, FsPrintfIndex, "Invalid itemPath:\"%s\".\n", itemPath);
        fflush(stdout);
        if (NULL == /* pRecognition_item */ FsMacrosValue3(p, __check_channel_changed_Server, _item)) return 1;
        const void *parent0;
        const void *parent;
        {
            parent0 = pConfig;
            FsObjectList *list = fs_Config_node_template__IO(pConfig, &parent0, pConfig, 0, ((ConfigManager*) /* pRecognition */FsMacrosValue2(p, __check_channel_changed_Server)->ro._pConfigManager)->ro.__ipList_local, 0, "vsys");
            parent = list->pNode[list->startIndex];
            fs_ObjectList_delete__OI(list);
            list = fs_Config_node_template__IO(pConfig, &parent0, parent, 0, NULL, 0, "vsysChannel");
            parent = list->pNode[list->startIndex + index];
            fs_ObjectList_delete__OI(list);
        }
        if (fs_Config_get_sum((FsEbml*) pConfig, (struct FsEbml_node *) parent) == /* pRecognition_item */ FsMacrosValue3(p, __check_channel_changed_Server, _item)->ro._sum
#ifdef __check_channel_changed_checkTimeControl
                && fs_Config_get_sum_timeControl(pConfig, item0, item, __check_channel_changed_checkTimeControl) == /* pRecognition_item */ FsMacrosValue3(p, __check_channel_changed_Server, _item)->ro._timeControlSum
#endif
                )return 0;
    }
#ifdef FsDebug
    FsLog2(FsLogType_info, FsPrintfIndex, "Item(=%p) has changed,index=%u,itemPath:\"%s\"/%p,sum=%llx/%llx\n"
            , /* pRecognition_item */ FsMacrosValue3(p, __check_channel_changed_Server, _item), index, itemPath, item, sum, /* pRecognition_item */ FsMacrosValue3(p, __check_channel_changed_Server, _item)->ro._sum);
#endif
    return 1;
#ifdef __check_channel_changed_itemListLock
#undef __check_channel_changed_itemListLock
#endif
#ifdef __check_channel_changed_checkTimeControl
#undef __check_channel_changed_checkTimeControl
#endif
#undef __check_channel_changed_Server
#endif
}

/* 
 * 创建一个新的Capture对象;
 * 返回Capture指针.
 */
struct Capture * capture_new__IO(/* 监控的线程指针对象,开始线程后把创建的监控节点添加到此对象中 */ void *const pMonitor
        , /* 多线程共有的运行状态,如果此值为空,则引用默认的全局运行状态变量,本线程的运行状态只有与此值相同才能运行 */signed char *const externRunStatus
        , /* 父线程的线程号,由systemThreadTid获得,为0表示无父线程 */ const pthread_t parentTid, /* 线程名 */const char threadName[]
        , /* 配置管理对象 */ void *const pConfigManager
        , /* 每个线程最多处理的通道数 */ const unsigned short channelCountPerThread
        , /* 在有输出数据时向哪个开方分组写入信号 */FsGroupSqrtPthreadSafety * const pGroupSqrtOut
        , /* rtsp客户端对象 */ Rtsp * const pRtspClient
        , /* GB28181客户端对象 */ GB28181 * const pGB28181Client) {
    unsigned int len = strlen(threadName) + 1;
    struct Capture * const rst = (struct Capture*) fsMalloc(sizeof ( struct Capture) +len);
    memset(rst, 0, sizeof ( struct Capture));
    char *pd = (char*) (rst + 1);
    /* 监控的线程指针对象,开始线程后把创建的监控节点添加到此对象中 */
    rst->ro._pMonitor = pMonitor;
    /* 多线程共有的运行状态,如果此值为空,则引用默认的全局运行状态变量,本线程的运行状态只有与此值相同才能运行 */
    if (externRunStatus) rst->ro._externRunStatus = externRunStatus;
    else rst->ro._externRunStatus = &defaultRunStatus;
    /* 父线程的线程号,由systemThreadTid获得,为0表示无父线程 */
    rst->ro._parentTid = parentTid;
#define __pipe_init_var rst->ro.__pipe
#ifndef __pipe_init 
    if (pipe(__pipe_init_var)) {
        FsLog(FsLogType_fatal, FsPrintfIndex, "Pipe error[%d/%d]:\"%s\".\n", errno, WSAGetLastError(), strerror(errno));
        fflush(stdout);
        __pipe_init_var[0] = -1, __pipe_init_var[1] = -1;
    } else {
        /* 设置非阻塞模式 */
        {
            int rv = 1;
            if (ioctl(__pipe_init_var[0], FIONBIO, &rv) != 0) {
                FsLog(FsLogType_fatal, FsPrintfIndex, "Ioctl error[%d/%d]:\"%s\",fd=%d.\n", errno, WSAGetLastError(), strerror(errno), __pipe_init_var[0]);
                fflush(stdout);
            }
        }
        {
            int rv = 1;
            if (rv = 1, ioctl(__pipe_init_var[1], FIONBIO, &rv) != 0) {
                FsLog(FsLogType_fatal, FsPrintfIndex, "Ioctl error[%d/%d]:\"%s\",fd=%d.\n", errno, WSAGetLastError(), strerror(errno), __pipe_init_var[1]);
                fflush(stdout);
            }
        }
    }

#ifdef FsFdOut
    FsPrintf2(FsPrintfIndex, "FsFdOut,fd=%d/%d.\n", __pipe_init_var[0], __pipe_init_var[1]);
#endif
#undef __pipe_init_var
#endif
    /* 线程名 */
    rst->ro._threadName_ = pd - (char*) (rst + 1), len = strlen(threadName) + 1, memcpy(pd, threadName, len);
    /* 配置 */
    rst->ro._pConfigManager = pConfigManager;
    /* 通道链表 */
    rst->ro.__itemList_ = fs_ObjectList_new_unsafe__IO(8);
    /* 每个线程最多处理的通道数 */
    rst->ro._channelCountPerThread = channelCountPerThread;
    /* 在有输出数据时向哪个开方分组写入信号 */
    rst->ro._pGroupSqrtOut = pGroupSqrtOut;
    /* rtsp客户端对象 */
    rst->ro._pRtspClient = pRtspClient;
    /* GB28181客户端对象 */
    rst->ro._pGB28181Client = pGB28181Client;
    /* 重置通道,负数表示重置完成,初始化为-1 */
    rst->rw._resetChannel = -1;

    return rst;
}

/* 删除pCapture指向的实例对象 */
void capture_delete__OI(struct Capture * const pCapture) {
    /* 通道链表 */
    fs_ObjectList_delete_unsafe__OI(pCapture->ro.__itemList_);

    /* 管道, 唤醒主线程*/
    closesocket(pCapture->ro.__pipe[0]);
    closesocket(pCapture->ro.__pipe[1]);

    fsFree(pCapture);
}

/* 开始线程,必须先设置好初始化时为可写全过程可读区的所有参数 */
void capture_startThread(struct Capture * const pCapture
        , /* 线程分时优先级,有效值为[-1-99],越大优先级越高,0表示不改变优先级,继承父线程的优先级,-1-表示强制线程的调度模式为分时调度策略,正数强制线程的调度模式为实时分时调度策略 */const signed char thisNice) {
    //#define __inRun_set_in_thread_start
    //#define __inRun_set_in_thread_end
#define __inRun_set_noinRun
#define __inRun_set_var pCapture
#ifndef __inRun_set 
#ifdef FsDebug
#ifndef __inRun_set_noinRun
    if ((__inRun_set_var)->p.inRun !=
#ifdef __inRun_set_in_thread_start
            1
#elif defined __inRun_set_in_thread_end
            3
#else
            0
#endif
            ) {
        FsLog(FsLogType_fatal, FsPrintfIndex, "Invalid inRun(=%hhu).\n", (__inRun_set_var)->p.inRun);
        fflush(stdout);
        FsLogExit();
    }
#endif  
#if !defined __inRun_set_in_thread_start && !defined __inRun_set_in_thread_end
    if ((__inRun_set_var)->p.stid != 0) {
        FsLog(FsLogType_fatal, FsPrintfIndex, "Invalid stid(=%ld).\n", (__inRun_set_var)->p.stid);
        fflush(stdout);
        FsLogExit();
    }
#endif
#endif
#ifndef __inRun_set_noinRun
#ifdef __inRun_set_in_thread_start
    (__inRun_set_var)->p.inRun = 3;
#elif defined __inRun_set_in_thread_end
    (__inRun_set_var)->p.inRun = 2;
#else
    (__inRun_set_var)->p.inRun = 1;
#endif
#endif
#ifdef __inRun_set_in_thread_start
#undef __inRun_set_in_thread_start
#endif
#ifdef __inRun_set_in_thread_end
#undef __inRun_set_in_thread_end
#endif
#ifdef __inRun_set_noinRun
#undef __inRun_set_noinRun
#endif
#undef __inRun_set_var
#endif
    pCapture->rw.runStatus = 1;
    pCapture->rw.thisNice = thisNice;
#ifdef FsDebug
    CaptureLog2(FsLogType_threadDebug, "Init Parameter");
#endif
    //#define __fs_thread_attr_stacksize 32*1024
#ifndef __fs_thread_attr
    pthread_attr_t attr;
    /* 初始化线程属性 */
    pthread_attr_init(&attr);
    /* 设置分离状态 */
    //pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
    /* 设置栈地址 */
    //pthread_attr_setstackaddr ( pthread_attr_t *attr, void *addr );
    /* 设置栈大小 */
#ifndef __fs_thread_attr_stacksize
    pthread_attr_setstacksize(&attr, FsStackBase + 32 * 1024);
#else
    pthread_attr_setstacksize(&attr, FsStackBase + __fs_thread_attr_stacksize);
#endif
    /* 设置线程优先级 */
    if (thisNice > 0 && thisNice < 100) {
        /* 修改为实时分时调度策略 */
        pthread_attr_setschedpolicy(&attr, SCHED_RR);
        struct sched_param param;
        param.sched_priority = thisNice;
        pthread_attr_setschedparam(&attr, &param);
    } else if (0 == thisNice) {
        /* 继承父进程优先级 */
#ifndef ANDROID
        pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED);
        pthread_attr_setschedpolicy(&attr, SCHED_OTHER);
#endif
    } else if (-1 == thisNice) {
        /* 修改为分时调度策略 */
        pthread_attr_setschedpolicy(&attr, SCHED_OTHER);
    } else {
        FsLog(FsLogType_fatal, FsPrintfIndex, "Invalid thisNice(=%hhd).\n", thisNice);
        fflush(stdout);
        FsLogExit();
    }
#ifdef __fs_thread_attr_stacksize
#undef __fs_thread_attr_stacksize
#endif
#endif           
    if (pthread_create(&pCapture->p.stid, &attr, (void* (*)(void*))capture_P_T, pCapture)) {
        FsLog(FsLogType_fatal, FsPrintfIndex, "Create thread error[%d/%d]:\"%s\".\n", errno, WSAGetLastError(), strerror(errno));
        fflush(stdout);
        FsLogExit();
    }
    pthread_attr_destroy(&attr);
}

/* 结束线程,只发送信号,不回收线程资源 */
void capture_stopThread_noWait(struct Capture * const pCapture) {
#define __stopThread_var pCapture
#define __stopThread_noWait
#define __stopThread_noPipe
#define __stopThread_noinRun
#ifndef __stopThread 
    (__stopThread_var)->rw.runStatus = 0;
    if ((__stopThread_var)->p.stid) {
#ifndef __stopThread_noPipe
        for (;;) {
#define  __control_pipe_write_var (__stopThread_var)->ro.__pipe[1]
#ifndef __control_pipe_write 
            if (write(__control_pipe_write_var, "w", 1) != 1)

            {
                FsLog(FsLogType_error, FsPrintfIndex, "Write pipe(=%d) error[%d/%d]:\"%s\".\n", __control_pipe_write_var, errno, WSAGetLastError(), strerror(errno));
                fflush(stdout);
            } else break;
#undef __control_pipe_write_var
#endif
        }
#endif
#ifndef __stopThread_noWait
#ifdef FsDebug
        FsLog2(FsLogType_threadDebug, FsPrintfIndex, "Stop Start.\n");
#endif
        pthread_join((__stopThread_var)->p.stid, 0);
        thread_join_remove_check_pthreadSafety((__stopThread_var)->p.stid);
        (__stopThread_var)->p.stid = 0;
#ifndef __stopThread_noinRun
#ifdef FsDebug
        if ((__stopThread_var)->p.inRun != 2) {

            FsLog(FsLogType_fatal, FsPrintfIndex, "Invalid inRun(=%hhu).\n", (__stopThread_var)->p.inRun);
            fflush(stdout);
            FsLogExit();
        }
#endif
        (__stopThread_var)->p.inRun = 0;
#endif
#ifdef FsDebug
        FsLog2(FsLogType_threadDebug, FsPrintfIndex, "Stop successfully.\n");
#endif
#endif
    }
#undef __stopThread_var
#ifdef __stopThread_noWait
#undef __stopThread_noWait
#endif
#ifdef __stopThread_noPipe
#undef __stopThread_noPipe
#endif
#ifdef __stopThread_noinRun
#undef __stopThread_noinRun
#endif
#endif
}

/* 结束线程 */
void capture_stopThread(struct Capture * const pCapture) {
#define __stopThread_var pCapture
    //#define __stopThread_noWait
#define __stopThread_noPipe
#define __stopThread_noinRun
#ifndef __stopThread 
    (__stopThread_var)->rw.runStatus = 0;
    if ((__stopThread_var)->p.stid) {
#ifndef __stopThread_noPipe
        for (;;) {
#define  __control_pipe_write_var (__stopThread_var)->ro.__pipe[1]
#ifndef __control_pipe_write 
            if (write(__control_pipe_write_var, "w", 1) != 1)

            {
                FsLog(FsLogType_error, FsPrintfIndex, "Write pipe(=%d) error[%d/%d]:\"%s\".\n", __control_pipe_write_var, errno, WSAGetLastError(), strerror(errno));
                fflush(stdout);
            } else break;
#undef __control_pipe_write_var
#endif
        }
#endif
#ifndef __stopThread_noWait
#ifdef FsDebug
        FsLog2(FsLogType_threadDebug, FsPrintfIndex, "Stop Start.\n");
#endif
        pthread_join((__stopThread_var)->p.stid, 0);
        thread_join_remove_check_pthreadSafety((__stopThread_var)->p.stid);
        (__stopThread_var)->p.stid = 0;
#ifndef __stopThread_noinRun
#ifdef FsDebug
        if ((__stopThread_var)->p.inRun != 2) {

            FsLog(FsLogType_fatal, FsPrintfIndex, "Invalid inRun(=%hhu).\n", (__stopThread_var)->p.inRun);
            fflush(stdout);
            FsLogExit();
        }
#endif
        (__stopThread_var)->p.inRun = 0;
#endif
#ifdef FsDebug
        FsLog2(FsLogType_threadDebug, FsPrintfIndex, "Stop successfully.\n");
#endif
#endif
    }
#undef __stopThread_var
#ifdef __stopThread_noWait
#undef __stopThread_noWait
#endif
#ifdef __stopThread_noPipe
#undef __stopThread_noPipe
#endif
#ifdef __stopThread_noinRun
#undef __stopThread_noinRun
#endif
#endif
}

/* 传出数据帧,把数据帧的指针放在ppFrame数组中,返回获取到的数据帧数量 */
unsigned int capture_item_frame_out_pthreadSafety__IO_4(struct Capture * const pCapture, /* 获取的通道编号,从0开始 */unsigned int index
        , /* ppFrame可储存的数据帧数量 */unsigned int frameCount, FsObjectImageFrame * ppFrame[]) {
#define __item_frame_out_pthreadSafety__IO_4_Server Capture
#ifndef __item_frame_out_pthreadSafety__IO_4 
    struct /* Recognition_item */ FsMacrosValue2(__item_frame_out_pthreadSafety__IO_4_Server, _item) * const /* pRecognition_item */ FsMacrosValue3(p, __item_frame_out_pthreadSafety__IO_4_Server, _item)
            = (struct /* Recognition_item */ FsMacrosValue2(__item_frame_out_pthreadSafety__IO_4_Server, _item) *) /* pRecognition */FsMacrosValue2(p, __item_frame_out_pthreadSafety__IO_4_Server)->ro.
#ifdef __item_frame_out_pthreadSafety__IO_4_itemListLock
            __itemList
#else
            __itemList_
#endif
            ->pNode[/* pRecognition */FsMacrosValue2(p, __item_frame_out_pthreadSafety__IO_4_Server)->ro.
#ifdef __item_frame_out_pthreadSafety__IO_4_itemListLock
            __itemList
#else
            __itemList_
#endif
            ->startIndex + index];
#ifdef FsDebug
    if (NULL == /* pRecognition_item */ FsMacrosValue3(p, __item_frame_out_pthreadSafety__IO_4_Server, _item)) {
        FsLog(FsLogType_fatal, FsPrintfIndex, "item is NULL,index=%u.\n", index);
        FsLogTag(100);
        fflush(stdout);
        FsLogExit();
    }
    //    if (/* pRecognition_item */ FsMacrosValue3(p, __item_frame_out_pthreadSafety__IO_4_Server, _item)->ro.__framelistOut->nodeCount > frameCount) {
    //        FsLog(FsLogType_matchError, FsPrintfIndex, "Can out frame(=%lu/%u) too big,index=%u.\n", /* pRecognition_item */ FsMacrosValue3(p, __item_frame_out_pthreadSafety__IO_4_Server, _item)->ro.__framelistOut->nodeCount, frameCount, index);
    //        fflush(stdout);
    //    }
#endif
    if (/* pRecognition_item */ FsMacrosValue3(p, __item_frame_out_pthreadSafety__IO_4_Server, _item)->ro.__framelistOut->nodeCount > 0) {
        pthread_mutex_lock(&/* pRecognition_item */ FsMacrosValue3(p, __item_frame_out_pthreadSafety__IO_4_Server, _item)->ro.__framelistOut->mutex);
        index = /* pRecognition_item */ FsMacrosValue3(p, __item_frame_out_pthreadSafety__IO_4_Server, _item)->ro.__framelistOut->nodeCount;
        if (index > frameCount)index = frameCount;
        FsObjectImageFrame **ppNode = (FsObjectImageFrame**) /* pRecognition_item */ FsMacrosValue3(p, __item_frame_out_pthreadSafety__IO_4_Server, _item)->ro.__framelistOut->pNode + /* pRecognition_item */ FsMacrosValue3(p, __item_frame_out_pthreadSafety__IO_4_Server, _item)->ro.__framelistOut->startIndex;
        for (frameCount = index; frameCount > 0; frameCount--) *ppFrame++ = *ppNode++;
        fs_ObjectList_remove_head_n(/* pRecognition_item */ FsMacrosValue3(p, __item_frame_out_pthreadSafety__IO_4_Server, _item)->ro.__framelistOut, index);
        pthread_mutex_unlock(&/* pRecognition_item */ FsMacrosValue3(p, __item_frame_out_pthreadSafety__IO_4_Server, _item)->ro.__framelistOut->mutex);
        return index;
    }
    return 0;
#undef __item_frame_out_pthreadSafety__IO_4_Server
#ifdef __item_frame_out_pthreadSafety__IO_4_itemListLock
#undef __item_frame_out_pthreadSafety__IO_4_itemListLock
#endif
#endif
}

/* 获取数据帧的数量 */
unsigned int capture_item_get_frame_count_pthreadSafety(struct Capture * const pCapture, /* 获取的通道编号,从0开始 */unsigned int index) {
    struct Capture_item * const pCapture_item = (struct Capture_item *) pCapture->ro.__itemList_->pNode[pCapture->ro.__itemList_->startIndex + index];
    if (NULL == pCapture_item)return 0;
    return pCapture_item->ro.__framelistOut->nodeCount;
}

/* 重置模块内通道对应的开方分组 */
void capture_item_reset_groupSqrtOut(struct Capture * const pCapture, /* 获取的通道编号,从0开始 */unsigned int index
        , /* 新组数据的起始指针 */unsigned char *const groupValue, /* 新组的参数 */ const FsGroupSqrt * const pGroupSqrt) {
#define __reset_groupSqrtOut_Server Capture
#ifndef __reset_groupSqrtOut 
    struct /* Recognition_item */ FsMacrosValue2(__reset_groupSqrtOut_Server, _item) * const /* pRecognition_item */ FsMacrosValue3(p, __reset_groupSqrtOut_Server, _item)
            = (struct /* Recognition_item */ FsMacrosValue2(__reset_groupSqrtOut_Server, _item) *) /* pRecognition */FsMacrosValue2(p, __reset_groupSqrtOut_Server)->ro.
#ifdef __reset_groupSqrtOut_itemListLock
            __itemList
#else
            __itemList_
#endif
            ->pNode[/* pRecognition */FsMacrosValue2(p, __reset_groupSqrtOut_Server)->ro.
#ifdef __reset_groupSqrtOut_itemListLock
            __itemList
#else
            __itemList_
#endif
            ->startIndex + index];
    if (/* pRecognition_item */ FsMacrosValue3(p, __reset_groupSqrtOut_Server, _item))
        Fs_GroupSqrtOut_set(/* pRecognition_item */ FsMacrosValue3(p, __reset_groupSqrtOut_Server, _item)->rw._pGroupSqrtOut, pGroupSqrt->groupSqrt_member, pGroupSqrt->groupSqrt_group, groupValue, index);
#ifdef __reset_groupSqrtOut_itemListLock
#undef __reset_groupSqrtOut_itemListLock
#endif
#undef __reset_groupSqrtOut_Server
#endif
}

//红光相机开

int capture_Redcamera_open(struct Capture_item *pCamera_rtsp_item, unsigned int speed, double time) {
    if (pCamera_rtsp_item->p.camera_socket <= 0) {
        pCamera_rtsp_item->p.camera_tv.tv_sec = 0;
        pCamera_rtsp_item->p.camera_tv.tv_usec = 200 * 1000;
        pCamera_rtsp_item->p.camera_socket = ZGX_Connect(pCamera_rtsp_item->p.camera_ip, pCamera_rtsp_item->p.camera_port, &pCamera_rtsp_item->p.camera_tv);

        if (pCamera_rtsp_item->p.camera_socket <= 0) {
            printf("(%s:%d) cam connect error!\n", __FUNCTION__, __LINE__);
            return -1;
        }
    }

    camera_t cam = CAM_FACTOR_ZGXRED;

    if (pCamera_rtsp_item->p.camera_type == 1)
        cam = CAM_FACTOR_ZGXLIGHT;

    cam = CAM_FACTOR_ZGXRED;
    //ptz_param_info_t ptz_p;
    //ptz_p.nDirectSpeed = speed;

    camera_param_t cam_p;
    cam_p = CAM_PARAM_POWERON;

    UINT nPower = 1;

    printf("(%s:%d) camera_ip:%s nPower:%d \n", __FUNCTION__, __LINE__, pCamera_rtsp_item->p.camera_ip, nPower);
    cts_res_t ret = camera_ctl_setParam(pCamera_rtsp_item->p.camera_socket, cam, cam_p, &nPower, &pCamera_rtsp_item->p.camera_tv);
    if (ret != CTS_SUCCESS) {

        printf("(%s:%d) cam ctl error!\n", __FUNCTION__, __LINE__);
        camera_disconnect(pCamera_rtsp_item->p.camera_socket);
        pCamera_rtsp_item->p.camera_socket = -1;

        return -1;
    }

    return 0;
}

//红光相机关

int capture_Redcamera_close(struct Capture_item *pCamera_rtsp_item, unsigned int speed, double time) {
    if (pCamera_rtsp_item->p.camera_socket <= 0) {
        pCamera_rtsp_item->p.camera_tv.tv_sec = 0;
        pCamera_rtsp_item->p.camera_tv.tv_usec = 200 * 1000;
        pCamera_rtsp_item->p.camera_socket = ZGX_Connect(pCamera_rtsp_item->p.camera_ip, pCamera_rtsp_item->p.camera_port, &pCamera_rtsp_item->p.camera_tv);

        if (pCamera_rtsp_item->p.camera_socket <= 0) {
            printf("(%s:%d) cam connect error!\n", __FUNCTION__, __LINE__);
            return -1;
        }
    }

    camera_t cam = CAM_FACTOR_ZGXRED;

    if (pCamera_rtsp_item->p.camera_type == 1)
        cam = CAM_FACTOR_ZGXLIGHT;

    cam = CAM_FACTOR_ZGXRED;
    //ptz_param_info_t ptz_p;  
    //ptz_p.nDirectSpeed = speed;

    camera_param_t cam_p;
    cam_p = CAM_PARAM_POWERON;

    UINT nPower = 0;

    printf("(%s:%d) camera_ip:%s nPower:%d \n", __FUNCTION__, __LINE__, pCamera_rtsp_item->p.camera_ip, nPower);
    cts_res_t ret = camera_ctl_setParam(pCamera_rtsp_item->p.camera_socket, cam, cam_p, &nPower, &pCamera_rtsp_item->p.camera_tv);
    if (ret != CTS_SUCCESS) {

        printf("(%s:%d) cam ctl error!\n", __FUNCTION__, __LINE__);
        camera_disconnect(pCamera_rtsp_item->p.camera_socket);
        pCamera_rtsp_item->p.camera_socket = -1;

        return -1;
    }

    return 0;
}

//左转

int capture_camera_left(struct Capture_item *pCamera_rtsp_item, unsigned int speed, double time) {
    if (pCamera_rtsp_item->p.camera_socket <= 0) {
        pCamera_rtsp_item->p.camera_tv.tv_sec = 0;
        pCamera_rtsp_item->p.camera_tv.tv_usec = 200 * 1000;
        pCamera_rtsp_item->p.camera_socket = ZGX_Connect(pCamera_rtsp_item->p.camera_ip, pCamera_rtsp_item->p.camera_port, &pCamera_rtsp_item->p.camera_tv);

        if (pCamera_rtsp_item->p.camera_socket <= 0) {
            printf("(%s:%d) cam connect error!\n", __FUNCTION__, __LINE__);
            return -1;
        }
    }

    camera_t cam = CAM_FACTOR_ZGXLIGHT;

    if (pCamera_rtsp_item->p.camera_type == 1)
        cam = CAM_FACTOR_ZGXLIGHT;

    ptz_param_info_t ptz_p;
    ptz_p.nDirectSpeed = speed;

    camera_param_t cam_p;
    cam_p = CAM_PARAM_DIRECT_LEFT;

    printf("(%s:%d) camera_ip:%s speed:%d \n", __FUNCTION__, __LINE__, pCamera_rtsp_item->p.camera_ip, speed);
    cts_res_t ret = camera_ctl_setParam(pCamera_rtsp_item->p.camera_socket, cam, cam_p, &ptz_p, &pCamera_rtsp_item->p.camera_tv);
    if (ret != CTS_SUCCESS) {

        printf("(%s:%d) cam ctl error!\n", __FUNCTION__, __LINE__);
        camera_disconnect(pCamera_rtsp_item->p.camera_socket);
        pCamera_rtsp_item->p.camera_socket = -1;

        return -1;
    }

    return 0;
}

//右转

int capture_camera_right(struct Capture_item *pCamera_rtsp_item, unsigned int speed, double time) {
    if (pCamera_rtsp_item->p.camera_socket <= 0) {
        pCamera_rtsp_item->p.camera_tv.tv_sec = 0;
        pCamera_rtsp_item->p.camera_tv.tv_usec = 200 * 1000;
        pCamera_rtsp_item->p.camera_socket = ZGX_Connect(pCamera_rtsp_item->p.camera_ip, pCamera_rtsp_item->p.camera_port, &pCamera_rtsp_item->p.camera_tv);

        if (pCamera_rtsp_item->p.camera_socket <= 0) {
            printf("(%s:%d) cam connect error!\n", __FUNCTION__, __LINE__);
            return -1;
        }
    }

    camera_t cam = CAM_FACTOR_ZGXLIGHT;

    if (pCamera_rtsp_item->p.camera_type == 1)
        cam = CAM_FACTOR_ZGXLIGHT;

    ptz_param_info_t ptz_p;
    ptz_p.nDirectSpeed = speed;


    camera_param_t cam_p;
    cam_p = CAM_PARAM_DIRECT_RIGHT;

    printf("(%s:%d) camera_ip:%s speed:%d \n", __FUNCTION__, __LINE__, pCamera_rtsp_item->p.camera_ip, speed);
    cts_res_t ret = camera_ctl_setParam(pCamera_rtsp_item->p.camera_socket, cam, cam_p, &ptz_p, &pCamera_rtsp_item->p.camera_tv);
    if (ret != CTS_SUCCESS) {

        printf("(%s:%d) cam ctl error!\n", __FUNCTION__, __LINE__);
        camera_disconnect(pCamera_rtsp_item->p.camera_socket);
        pCamera_rtsp_item->p.camera_socket = -1;

        return -1;
    }

    return 0;
}

//上转

int capture_camera_up(struct Capture_item *pCamera_rtsp_item, unsigned int speed, double time) {
    if (pCamera_rtsp_item->p.camera_socket <= 0) {
        pCamera_rtsp_item->p.camera_tv.tv_sec = 1;
        pCamera_rtsp_item->p.camera_tv.tv_usec = 200 * 1000;
        pCamera_rtsp_item->p.camera_socket = ZGX_Connect(pCamera_rtsp_item->p.camera_ip, pCamera_rtsp_item->p.camera_port, &pCamera_rtsp_item->p.camera_tv);

        if (pCamera_rtsp_item->p.camera_socket <= 0) {
            printf("(%s:%d) cam connect error!\n", __FUNCTION__, __LINE__);
            return -1;
        }
    }

    camera_t cam = CAM_FACTOR_ZGXLIGHT;

    if (pCamera_rtsp_item->p.camera_type == 1)
        cam = CAM_FACTOR_ZGXLIGHT;

    ptz_param_info_t ptz_p;
    ptz_p.nDirectSpeed = speed;

    camera_param_t cam_p;
    cam_p = CAM_PARAM_DIRECT_UP;

    cts_res_t ret = camera_ctl_setParam(pCamera_rtsp_item->p.camera_socket, cam, cam_p, &ptz_p, &pCamera_rtsp_item->p.camera_tv);
    if (ret != CTS_SUCCESS) {

        printf("(%s:%d) cam ctl error!\n", __FUNCTION__, __LINE__);
        camera_disconnect(pCamera_rtsp_item->p.camera_socket);
        pCamera_rtsp_item->p.camera_socket = -1;

        return -1;
    }

    return 0;
}

//下转

int capture_camera_down(struct Capture_item *pCamera_rtsp_item, unsigned int speed, double time) {
    if (pCamera_rtsp_item->p.camera_socket <= 0) {
        pCamera_rtsp_item->p.camera_tv.tv_sec = 1;
        pCamera_rtsp_item->p.camera_tv.tv_usec = 200 * 1000;
        pCamera_rtsp_item->p.camera_socket = ZGX_Connect(pCamera_rtsp_item->p.camera_ip, pCamera_rtsp_item->p.camera_port, &pCamera_rtsp_item->p.camera_tv);

        if (pCamera_rtsp_item->p.camera_socket <= 0) {
            printf("(%s:%d) cam connect error!\n", __FUNCTION__, __LINE__);
            return -1;
        }
    }

    camera_t cam = CAM_FACTOR_ZGXLIGHT;

    if (pCamera_rtsp_item->p.camera_type == 1)
        cam = CAM_FACTOR_ZGXLIGHT;

    ptz_param_info_t ptz_p;
    ptz_p.nDirectSpeed = speed;

    camera_param_t cam_p;
    cam_p = CAM_PARAM_DIRECT_DOWN;

    cts_res_t ret = camera_ctl_setParam(pCamera_rtsp_item->p.camera_socket, cam, cam_p, &ptz_p, &pCamera_rtsp_item->p.camera_tv);
    if (ret != CTS_SUCCESS) {

        printf("(%s:%d) cam ctl error!\n", __FUNCTION__, __LINE__);
        camera_disconnect(pCamera_rtsp_item->p.camera_socket);
        pCamera_rtsp_item->p.camera_socket = -1;

        return -1;
    }

    return 0;
}

//左上

int capture_camera_leftup(struct Capture_item *pCamera_rtsp_item, unsigned int speed, double time) {
    if (pCamera_rtsp_item->p.camera_socket <= 0) {
        pCamera_rtsp_item->p.camera_tv.tv_sec = 1;
        pCamera_rtsp_item->p.camera_tv.tv_usec = 200 * 1000;
        pCamera_rtsp_item->p.camera_socket = ZGX_Connect(pCamera_rtsp_item->p.camera_ip, pCamera_rtsp_item->p.camera_port, &pCamera_rtsp_item->p.camera_tv);

        if (pCamera_rtsp_item->p.camera_socket <= 0) {
            printf("(%s:%d) cam connect error!\n", __FUNCTION__, __LINE__);
            return -1;
        }
    }

    camera_t cam = CAM_FACTOR_ZGXLIGHT;

    if (pCamera_rtsp_item->p.camera_type == 1)
        cam = CAM_FACTOR_ZGXLIGHT;

    ptz_param_info_t ptz_p;
    ptz_p.nDirectSpeed = speed;

    camera_param_t cam_p;
    cam_p = CAM_PARAM_DIRECT_LEFTUP;

    printf("(%s:%d) camera_socket:%d speed:%d \n", __FUNCTION__, __LINE__, pCamera_rtsp_item->p.camera_socket, speed);

    cts_res_t ret = camera_ctl_setParam(pCamera_rtsp_item->p.camera_socket, cam, cam_p, &ptz_p, &pCamera_rtsp_item->p.camera_tv);
    if (ret != CTS_SUCCESS) {

        printf("(%s:%d) cam ctl error!\n", __FUNCTION__, __LINE__);
        camera_disconnect(pCamera_rtsp_item->p.camera_socket);
        pCamera_rtsp_item->p.camera_socket = -1;

        return -1;
    }

    return 0;
}

//左下

int capture_camera_leftdown(struct Capture_item *pCamera_rtsp_item, unsigned int speed, double time) {
    if (pCamera_rtsp_item->p.camera_socket <= 0) {
        pCamera_rtsp_item->p.camera_tv.tv_sec = 1;
        pCamera_rtsp_item->p.camera_tv.tv_usec = 200 * 1000;
        pCamera_rtsp_item->p.camera_socket = ZGX_Connect(pCamera_rtsp_item->p.camera_ip, pCamera_rtsp_item->p.camera_port, &pCamera_rtsp_item->p.camera_tv);

        if (pCamera_rtsp_item->p.camera_socket <= 0) {
            printf("(%s:%d) cam connect error!\n", __FUNCTION__, __LINE__);
            return -1;
        }
    }

    camera_t cam = CAM_FACTOR_ZGXLIGHT;

    if (pCamera_rtsp_item->p.camera_type == 1)
        cam = CAM_FACTOR_ZGXLIGHT;

    ptz_param_info_t ptz_p;
    ptz_p.nDirectSpeed = speed;

    camera_param_t cam_p;
    cam_p = CAM_PARAM_DIRECT_LEFTDOWN;

    cts_res_t ret = camera_ctl_setParam(pCamera_rtsp_item->p.camera_socket, cam, cam_p, &ptz_p, &pCamera_rtsp_item->p.camera_tv);
    if (ret != CTS_SUCCESS) {

        printf("(%s:%d) cam ctl error!\n", __FUNCTION__, __LINE__);
        camera_disconnect(pCamera_rtsp_item->p.camera_socket);
        pCamera_rtsp_item->p.camera_socket = -1;

        return -1;
    }

    return 0;
}

//右上

int capture_camera_rightup(struct Capture_item *pCamera_rtsp_item, unsigned int speed, double time) {
    if (pCamera_rtsp_item->p.camera_socket <= 0) {
        pCamera_rtsp_item->p.camera_tv.tv_sec = 1;
        pCamera_rtsp_item->p.camera_tv.tv_usec = 200 * 1000;
        pCamera_rtsp_item->p.camera_socket = ZGX_Connect(pCamera_rtsp_item->p.camera_ip, pCamera_rtsp_item->p.camera_port, &pCamera_rtsp_item->p.camera_tv);

        if (pCamera_rtsp_item->p.camera_socket <= 0) {
            printf("(%s:%d) cam connect error!\n", __FUNCTION__, __LINE__);
            return -1;
        }
    }

    camera_t cam = CAM_FACTOR_ZGXLIGHT;

    if (pCamera_rtsp_item->p.camera_type == 1)
        cam = CAM_FACTOR_ZGXLIGHT;

    ptz_param_info_t ptz_p;
    ptz_p.nDirectSpeed = speed;

    camera_param_t cam_p;
    cam_p = CAM_PARAM_DIRECT_RIGHTUP;

    cts_res_t ret = camera_ctl_setParam(pCamera_rtsp_item->p.camera_socket, cam, cam_p, &ptz_p, &pCamera_rtsp_item->p.camera_tv);
    if (ret != CTS_SUCCESS) {

        printf("(%s:%d) cam ctl error!\n", __FUNCTION__, __LINE__);
        camera_disconnect(pCamera_rtsp_item->p.camera_socket);
        pCamera_rtsp_item->p.camera_socket = -1;

        return -1;
    }

    return 0;
}

//右下

int capture_camera_rightdown(struct Capture_item *pCamera_rtsp_item, unsigned int speed, double time) {
    if (pCamera_rtsp_item->p.camera_socket <= 0) {
        pCamera_rtsp_item->p.camera_tv.tv_sec = 1;
        pCamera_rtsp_item->p.camera_tv.tv_usec = 200 * 1000;
        pCamera_rtsp_item->p.camera_socket = ZGX_Connect(pCamera_rtsp_item->p.camera_ip, pCamera_rtsp_item->p.camera_port, &pCamera_rtsp_item->p.camera_tv);

        if (pCamera_rtsp_item->p.camera_socket <= 0) {
            printf("(%s:%d) cam connect error!\n", __FUNCTION__, __LINE__);
            return -1;
        }
    }

    camera_t cam = CAM_FACTOR_ZGXLIGHT;

    if (pCamera_rtsp_item->p.camera_type == 1)
        cam = CAM_FACTOR_ZGXLIGHT;

    ptz_param_info_t ptz_p;
    ptz_p.nDirectSpeed = speed;

    camera_param_t cam_p;
    cam_p = CAM_PARAM_DIRECT_RIGHTDOWN;

    cts_res_t ret = camera_ctl_setParam(pCamera_rtsp_item->p.camera_socket, cam, cam_p, &ptz_p, &pCamera_rtsp_item->p.camera_tv);
    if (ret != CTS_SUCCESS) {

        printf("(%s:%d) cam ctl error!\n", __FUNCTION__, __LINE__);
        camera_disconnect(pCamera_rtsp_item->p.camera_socket);
        pCamera_rtsp_item->p.camera_socket = -1;

        return -1;
    }

    return 0;
}

//停止转动

int capture_camera_stopturn(struct Capture_item *pCamera_rtsp_item, unsigned int speed, double time) {
    if (pCamera_rtsp_item->p.camera_socket <= 0) {
        pCamera_rtsp_item->p.camera_tv.tv_sec = 1;
        pCamera_rtsp_item->p.camera_tv.tv_usec = 200 * 1000;
        pCamera_rtsp_item->p.camera_socket = ZGX_Connect(pCamera_rtsp_item->p.camera_ip, pCamera_rtsp_item->p.camera_port, &pCamera_rtsp_item->p.camera_tv);

        if (pCamera_rtsp_item->p.camera_socket <= 0) {
            printf("(%s:%d) cam connect error!\n", __FUNCTION__, __LINE__);
            return -1;
        }
    }

    camera_t cam = CAM_FACTOR_ZGXLIGHT;

    if (pCamera_rtsp_item->p.camera_type == 1)
        cam = CAM_FACTOR_ZGXLIGHT;

    ptz_param_info_t ptz_p;
    ptz_p.nDirectSpeed = speed;

    camera_param_t cam_p;
    cam_p = CAM_PARAM_STOP;

    cts_res_t ret = camera_ctl_setParam(pCamera_rtsp_item->p.camera_socket, cam, cam_p, &ptz_p, &pCamera_rtsp_item->p.camera_tv);
    if (ret != CTS_SUCCESS) {

        printf("(%s:%d) cam ctl error!\n", __FUNCTION__, __LINE__);
        camera_disconnect(pCamera_rtsp_item->p.camera_socket);
        pCamera_rtsp_item->p.camera_socket = -1;

        return -1;
    }

    return 0;
}

//缩

int capture_camera_zoomnear(struct Capture_item *pCamera_rtsp_item, unsigned int speed, double time) {
    if (pCamera_rtsp_item->p.camera_socket <= 0) {
        pCamera_rtsp_item->p.camera_tv.tv_sec = 1;
        pCamera_rtsp_item->p.camera_tv.tv_usec = 200 * 1000;
        pCamera_rtsp_item->p.camera_socket = ZGX_Connect(pCamera_rtsp_item->p.camera_ip, pCamera_rtsp_item->p.camera_port, &pCamera_rtsp_item->p.camera_tv);

        if (pCamera_rtsp_item->p.camera_socket <= 0) {
            printf("(%s:%d) cam connect error!\n", __FUNCTION__, __LINE__);
            return -1;
        }
    }

    camera_t cam = CAM_FACTOR_ZGXLIGHT;

    if (pCamera_rtsp_item->p.camera_type == 1)
        cam = CAM_FACTOR_ZGXLIGHT;

    ptz_param_info_t ptz_p;
    ptz_p.nDirectSpeed = speed;

    camera_param_t cam_p;
    cam_p = CAM_PARAM_ZOOM_IN;

    cts_res_t ret = camera_ctl_setParam(pCamera_rtsp_item->p.camera_socket, cam, cam_p, &ptz_p, &pCamera_rtsp_item->p.camera_tv);
    if (ret != CTS_SUCCESS) {

        printf("(%s:%d) cam ctl error!\n", __FUNCTION__, __LINE__);
        camera_disconnect(pCamera_rtsp_item->p.camera_socket);
        pCamera_rtsp_item->p.camera_socket = -1;

        return -1;
    }

    return 0;
}
//放

int capture_camera_zoomfar(struct Capture_item *pCamera_rtsp_item, unsigned int speed, double time) {
    if (pCamera_rtsp_item->p.camera_socket <= 0) {
        pCamera_rtsp_item->p.camera_tv.tv_sec = 1;
        pCamera_rtsp_item->p.camera_tv.tv_usec = 200 * 1000;
        pCamera_rtsp_item->p.camera_socket = ZGX_Connect(pCamera_rtsp_item->p.camera_ip, pCamera_rtsp_item->p.camera_port, &pCamera_rtsp_item->p.camera_tv);

        if (pCamera_rtsp_item->p.camera_socket <= 0) {
            printf("(%s:%d) cam connect error!\n", __FUNCTION__, __LINE__);
            return -1;
        }
    }

    camera_t cam = CAM_FACTOR_ZGXLIGHT;

    if (pCamera_rtsp_item->p.camera_type == 1)
        cam = CAM_FACTOR_ZGXLIGHT;

    ptz_param_info_t ptz_p;
    ptz_p.nDirectSpeed = speed;

    camera_param_t cam_p;
    cam_p = CAM_PARAM_ZOOM_OUT;

    cts_res_t ret = camera_ctl_setParam(pCamera_rtsp_item->p.camera_socket, cam, cam_p, &ptz_p, &pCamera_rtsp_item->p.camera_tv);
    if (ret != CTS_SUCCESS) {

        printf("(%s:%d) cam ctl error!\n", __FUNCTION__, __LINE__);
        camera_disconnect(pCamera_rtsp_item->p.camera_socket);
        pCamera_rtsp_item->p.camera_socket = -1;

        return -1;
    }

    return 0;
}

//聚焦+

int capture_camera_focusnear(struct Capture_item *pCamera_rtsp_item, unsigned int speed, double time) {
    if (pCamera_rtsp_item->p.camera_socket <= 0) {
        pCamera_rtsp_item->p.camera_tv.tv_sec = 1;
        pCamera_rtsp_item->p.camera_tv.tv_usec = 200 * 1000;
        pCamera_rtsp_item->p.camera_socket = ZGX_Connect(pCamera_rtsp_item->p.camera_ip, pCamera_rtsp_item->p.camera_port, &pCamera_rtsp_item->p.camera_tv);

        if (pCamera_rtsp_item->p.camera_socket <= 0) {
            printf("(%s:%d) cam connect error!\n", __FUNCTION__, __LINE__);
            return -1;
        }
    }

    camera_t cam = CAM_FACTOR_ZGXLIGHT;

    if (pCamera_rtsp_item->p.camera_type == 1)
        cam = CAM_FACTOR_ZGXLIGHT;

    ptz_param_info_t ptz_p;
    ptz_p.nDirectSpeed = speed;

    camera_param_t cam_p;
    cam_p = CAM_PARAM_FOCUS_NEAR;

    cts_res_t ret = camera_ctl_setParam(pCamera_rtsp_item->p.camera_socket, cam, cam_p, &ptz_p, &pCamera_rtsp_item->p.camera_tv);
    if (ret != CTS_SUCCESS) {

        printf("(%s:%d) cam ctl error!\n", __FUNCTION__, __LINE__);
        camera_disconnect(pCamera_rtsp_item->p.camera_socket);
        pCamera_rtsp_item->p.camera_socket = -1;

        return -1;
    }

    return 0;
}

//聚焦-

int capture_camera_focusfar(struct Capture_item *pCamera_rtsp_item, unsigned int speed, double time) {
    if (pCamera_rtsp_item->p.camera_socket <= 0) {
        pCamera_rtsp_item->p.camera_tv.tv_sec = 1;
        pCamera_rtsp_item->p.camera_tv.tv_usec = 200 * 1000;
        pCamera_rtsp_item->p.camera_socket = ZGX_Connect(pCamera_rtsp_item->p.camera_ip, pCamera_rtsp_item->p.camera_port, &pCamera_rtsp_item->p.camera_tv);

        if (pCamera_rtsp_item->p.camera_socket <= 0) {
            printf("(%s:%d) cam connect error!\n", __FUNCTION__, __LINE__);
            return -1;
        }
    }

    camera_t cam = CAM_FACTOR_ZGXLIGHT;

    if (pCamera_rtsp_item->p.camera_type == 1)
        cam = CAM_FACTOR_ZGXLIGHT;

    ptz_param_info_t ptz_p;
    ptz_p.nDirectSpeed = speed;

    camera_param_t cam_p;
    cam_p = CAM_PARAM_FOCUS_FAR;

    cts_res_t ret = camera_ctl_setParam(pCamera_rtsp_item->p.camera_socket, cam, cam_p, &ptz_p, &pCamera_rtsp_item->p.camera_tv);
    if (ret != CTS_SUCCESS) {

        printf("(%s:%d) cam ctl error!\n", __FUNCTION__, __LINE__);
        camera_disconnect(pCamera_rtsp_item->p.camera_socket);
        pCamera_rtsp_item->p.camera_socket = -1;

        return -1;
    }

    return 0;
}

//开光圈

int capture_camera_iris_open(struct Capture_item *pCamera_rtsp_item, unsigned int speed, double time) {
    if (pCamera_rtsp_item->p.camera_socket <= 0) {
        pCamera_rtsp_item->p.camera_tv.tv_sec = 1;
        pCamera_rtsp_item->p.camera_tv.tv_usec = 200 * 1000;
        pCamera_rtsp_item->p.camera_socket = ZGX_Connect(pCamera_rtsp_item->p.camera_ip, pCamera_rtsp_item->p.camera_port, &pCamera_rtsp_item->p.camera_tv);

        if (pCamera_rtsp_item->p.camera_socket <= 0) {
            printf("(%s:%d) cam connect error!\n", __FUNCTION__, __LINE__);
            return -1;
        }
    }

    camera_t cam = CAM_FACTOR_ZGXLIGHT;

    if (pCamera_rtsp_item->p.camera_type == 1)
        cam = CAM_FACTOR_ZGXLIGHT;

    ptz_param_info_t ptz_p;
    ptz_p.nDirectSpeed = speed;

    camera_param_t cam_p;
    cam_p = CAM_PARAM_IRIS_OPEN;

    cts_res_t ret = camera_ctl_setParam(pCamera_rtsp_item->p.camera_socket, cam, cam_p, &ptz_p, &pCamera_rtsp_item->p.camera_tv);
    if (ret != CTS_SUCCESS) {

        printf("(%s:%d) cam ctl error!\n", __FUNCTION__, __LINE__);
        camera_disconnect(pCamera_rtsp_item->p.camera_socket);
        pCamera_rtsp_item->p.camera_socket = -1;

        return -1;
    }

    return 0;
}

//关光圈

int capture_camera_iris_close(struct Capture_item *pCamera_rtsp_item, unsigned int speed, double time) {
    if (pCamera_rtsp_item->p.camera_socket <= 0) {
        pCamera_rtsp_item->p.camera_tv.tv_sec = 1;
        pCamera_rtsp_item->p.camera_tv.tv_usec = 200 * 1000;
        pCamera_rtsp_item->p.camera_socket = ZGX_Connect(pCamera_rtsp_item->p.camera_ip, pCamera_rtsp_item->p.camera_port, &pCamera_rtsp_item->p.camera_tv);

        if (pCamera_rtsp_item->p.camera_socket <= 0) {
            printf("(%s:%d) cam connect error!\n", __FUNCTION__, __LINE__);
            return -1;
        }
    }

    camera_t cam = CAM_FACTOR_ZGXLIGHT;

    if (pCamera_rtsp_item->p.camera_type == 1)
        cam = CAM_FACTOR_ZGXLIGHT;

    ptz_param_info_t ptz_p;
    ptz_p.nDirectSpeed = speed;

    camera_param_t cam_p;
    cam_p = CAM_PARAM_IRIS_CLOSE;

    cts_res_t ret = camera_ctl_setParam(pCamera_rtsp_item->p.camera_socket, cam, cam_p, &ptz_p, &pCamera_rtsp_item->p.camera_tv);
    if (ret != CTS_SUCCESS) {

        printf("(%s:%d) cam ctl error!\n", __FUNCTION__, __LINE__);
        camera_disconnect(pCamera_rtsp_item->p.camera_socket);
        pCamera_rtsp_item->p.camera_socket = -1;

        return -1;
    }

    return 0;
}

//透雾功能

int capture_camera_throughfrog(struct Capture_item *pCamera_rtsp_item, unsigned int speed, double time) {
    if (pCamera_rtsp_item->p.camera_socket <= 0) {
        pCamera_rtsp_item->p.camera_tv.tv_sec = 1;
        pCamera_rtsp_item->p.camera_tv.tv_usec = 200 * 1000;
        pCamera_rtsp_item->p.camera_socket = ZGX_Connect(pCamera_rtsp_item->p.camera_ip, pCamera_rtsp_item->p.camera_port, &pCamera_rtsp_item->p.camera_tv);

        if (pCamera_rtsp_item->p.camera_socket <= 0) {
            printf("(%s:%d) cam connect error!\n", __FUNCTION__, __LINE__);
            return -1;
        }
    }

    camera_t cam = CAM_FACTOR_ZGXLIGHT;

    if (pCamera_rtsp_item->p.camera_type == 1)
        cam = CAM_FACTOR_ZGXLIGHT;

    ptz_param_info_t ptz_p;
    ptz_p.nDirectSpeed = speed;

    camera_param_t cam_p;
    cam_p = CAM_PARAM_ZGXCLBL;

    cts_res_t ret = camera_ctl_setParam(pCamera_rtsp_item->p.camera_socket, cam, cam_p, &ptz_p, &pCamera_rtsp_item->p.camera_tv);
    if (ret != CTS_SUCCESS) {

        printf("(%s:%d) cam ctl error!\n", __FUNCTION__, __LINE__);
        camera_disconnect(pCamera_rtsp_item->p.camera_socket);
        pCamera_rtsp_item->p.camera_socket = -1;

        return -1;
    }

    return 0;
}

//转向指定ptz位置

int capture_camera_gotoPTZ(struct Capture_item *pCamera_rtsp_item, unsigned int speed, float p, float t, float z, double time) {
    if (pCamera_rtsp_item->p.camera_socket <= 0) {
        pCamera_rtsp_item->p.camera_tv.tv_sec = 1;
        pCamera_rtsp_item->p.camera_tv.tv_usec = 200 * 1000;
        pCamera_rtsp_item->p.camera_socket = ZGX_Connect(pCamera_rtsp_item->p.camera_ip, pCamera_rtsp_item->p.camera_port, &pCamera_rtsp_item->p.camera_tv);

        if (pCamera_rtsp_item->p.camera_socket <= 0) {
            printf("(%s:%d) cam connect error!\n", __FUNCTION__, __LINE__);
            return -1;
        }
    }

    camera_t cam = CAM_FACTOR_ZGXLIGHT;

    if (pCamera_rtsp_item->p.camera_type == 1)
        cam = CAM_FACTOR_ZGXLIGHT;


    camera_param_t cam_p = CAM_PARAM_PTZ;
    CTS_PTZ ptz_ptz;
    ptz_ptz.nPtzOpt = PRESET_OPT_GOTO;
    ptz_ptz.P = p;
    ptz_ptz.T = t;
    ptz_ptz.Z = z;
    ptz_ptz.nshotType = pCamera_rtsp_item->p.shot_type; // 南方海岸为 =1

    cts_res_t ret = camera_ctl_setParam(pCamera_rtsp_item->p.camera_socket, cam, cam_p, &ptz_ptz, &pCamera_rtsp_item->p.camera_tv);
    if (ret != CTS_SUCCESS) {

        printf("(%s:%d) cam ctl error!\n", __FUNCTION__, __LINE__);
        camera_disconnect(pCamera_rtsp_item->p.camera_socket);
        pCamera_rtsp_item->p.camera_socket = -1;

        return -1;
    }

    return 0;
}


//获取当前ptz值

int capture_camera_getPTZ(struct Capture_item *pCamera_rtsp_item, float* p, float* t, float* z) {

    FsPrintf(1, "*************_uuid:%s  m_action:_camera_getptz  camera_socket:%d*********\n", pCamera_rtsp_item->ro._uuid, pCamera_rtsp_item->p.camera_socket);
    fflush(stdout);
    //没有建立socket连接，或者x方向上已经停止转动
    if (pCamera_rtsp_item->p.camera_socket <= 0) {
        pCamera_rtsp_item->p.camera_tv.tv_sec = 1;
        pCamera_rtsp_item->p.camera_tv.tv_usec = 200 * 1000;
        pCamera_rtsp_item->p.camera_socket = ZGX_Connect(pCamera_rtsp_item->p.camera_ip, pCamera_rtsp_item->p.camera_port, &pCamera_rtsp_item->p.camera_tv);

        FsPrintf(1, "*************(ip:%s port:%d)_uuid:%s  m_action:_camera_getptz  camera_socket:%d*********\n", pCamera_rtsp_item->p.camera_ip, pCamera_rtsp_item->p.camera_port,
                pCamera_rtsp_item->ro._uuid, pCamera_rtsp_item->p.camera_socket);
        fflush(stdout);

        if (pCamera_rtsp_item->p.camera_socket <= 0) {
            return -1;
        }
    }

    FsPrintf(1, "*************_uuid:%s  m_action:_camera_getptz  camera_socket:%d*********\n", pCamera_rtsp_item->ro._uuid, pCamera_rtsp_item->p.camera_socket);
    fflush(stdout);

    camera_t cam = CAM_FACTOR_ZGXLIGHT;

    if (pCamera_rtsp_item->p.camera_type == 1)
        cam = CAM_FACTOR_ZGXLIGHT;

    CTS_PTZ ptz;
    cts_res_t ret = get_ptz(pCamera_rtsp_item->p.camera_socket, cam, &ptz, &pCamera_rtsp_item->p.camera_tv);
    if (ret != CTS_SUCCESS) {
        printf("(%s:%d) cam ctl error!\n", __FUNCTION__, __LINE__);
        return -1;
    }

    *p = (float) ptz.P;
    *t = (float) ptz.T;
    *z = (float) ptz.Z;

    printf("(%s:%d) get p:%f t:%f z:%f \n", __FUNCTION__, __LINE__, *p, *t, *z);

    return 0;
}


//自动聚焦功能

int capture_camera_autofocus(struct Capture_item *pCamera_rtsp_item, unsigned int speed, double time) {
    if (pCamera_rtsp_item->p.camera_socket <= 0) {
        pCamera_rtsp_item->p.camera_tv.tv_sec = 1;
        pCamera_rtsp_item->p.camera_tv.tv_usec = 200 * 1000;
        pCamera_rtsp_item->p.camera_socket = ZGX_Connect(pCamera_rtsp_item->p.camera_ip, pCamera_rtsp_item->p.camera_port, &pCamera_rtsp_item->p.camera_tv);

        if (pCamera_rtsp_item->p.camera_socket <= 0) {
            printf("(%s:%d) cam connect error!\n", __FUNCTION__, __LINE__);
            return -1;
        }
    }

    camera_t cam = CAM_FACTOR_ZGXLIGHT;

    if (pCamera_rtsp_item->p.camera_type == 1)
        cam = CAM_FACTOR_ZGXLIGHT;

    ptz_param_info_t ptz_p;
    ptz_p.nDirectSpeed = speed;

    camera_param_t cam_p;
    cam_p = CAM_PARAM_FOCUS_AUTO;

    cts_res_t ret = camera_ctl_setParam(pCamera_rtsp_item->p.camera_socket, cam, cam_p, &ptz_p, &pCamera_rtsp_item->p.camera_tv);
    if (ret != CTS_SUCCESS) {

        printf("(%s:%d) cam ctl error!\n", __FUNCTION__, __LINE__);
        camera_disconnect(pCamera_rtsp_item->p.camera_socket);
        pCamera_rtsp_item->p.camera_socket = -1;

        return -1;
    }

    return 0;
}
///////////////////////////定义公共函数结束//////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
