/* 
 * File:   camera_ctrl.h
 * Author: fslib
 *
 * Created on 2013年6月15日, 上午11:19
 */
#if !defined CAMERA_CTRL_H && !defined JAVA_IN_C

#define CAMERA_CTRL_H
#include "../../PublicTools/publicDefine.h"
#include "../../PublicTools/Thread/SocketPool.h"
/* 默认的控制函数 */
extern void *const camera_ctrl_default_ctrl_function[];
#ifdef __cplusplus
extern "C" {
#endif

    struct Camera_ctrl_imageInfo {
#define Camera_ctrl_imageInfo_mask_autoFocus (0x1U<<0)
        /* 自动聚焦,0-自动控制,1-手动控制 */
        signed char autoFocus : 2;
#define Camera_ctrl_imageInfo_mask_controlMode (0x1U<<1)
        /* 相机快门,增益,光圈的控制模式,0-自动控制,1-手动控制 */
        signed char controlMode : 2;
#define Camera_ctrl_imageInfo_mask_whiteBalanceMode (0x1U<<2)
        /* 相机白平衡的控制模式,0-自动控制,1-手动控制 */
        signed char whiteBalanceMode : 2;
#define Camera_ctrl_imageInfo_mask_imageMode (0x1U<<3)
        /* 图像亮度,色度,对比度,清晰度的控制模式,-1-第二种自动控制模式,0-自动控制,1-手动控制 */
        signed char imageMode : 2;
        /* 当前快门,曝光时间,微妙 */
#define Camera_ctrl_imageInfo_mask_shutter (0x1U<<4)
        unsigned short shutter;
        /* 当前增益,归一化到0-100 */
#define Camera_ctrl_imageInfo_mask_gain (0x1U<<5)
        unsigned char gain;
        /* 当前光圈,归一化到0-100 */
#define Camera_ctrl_imageInfo_mask_iris (0x1U<<6)
        signed char iris;
        /* 白平衡蓝色,0-255 */
#define Camera_ctrl_imageInfo_mask_whiteBalance_B (0x1U<<7)
        unsigned char whiteBalance_B;
        /* 白平衡红色,0-255 */
#define Camera_ctrl_imageInfo_mask_whiteBalance_R (0x1U<<8)
        unsigned char whiteBalance_R;
        /* 亮度,归一化到0-100 */
#define Camera_ctrl_imageInfo_mask_brightness (0x1U<<9)
        unsigned char brightness;
        /* 色度,归一化到0-100 */
#define Camera_ctrl_imageInfo_mask_colorSaturation (0x1U<<10)
        unsigned char colorSaturation;
        /* 对比度,归一化到0-100 */
#define Camera_ctrl_imageInfo_mask_contrast (0x1U<<11)
        unsigned char contrast;
        /* 清晰度,归一化到0-100 */
#define Camera_ctrl_imageInfo_mask_sharpness (0x1U<<12)
        unsigned char sharpness;
#define Camera_ctrl_imageInfo_mask_intensityAdjust (0x1U<<13)
        /* 明暗度矫正,归一化到0-100 */
        unsigned char intensityAdjust;
#define Camera_ctrl_imageInfo_mask_bw (0x1U<<14)
        /* 黑白热模式是否有效 */
        unsigned char bwValid : 1;
        /* 黑白热模式,0-白热,1-黑热 */
        unsigned char bw : 1;
        /* 图像拼接的裁剪系数,-1表示设置为相机默认值 */
#define Camera_ctrl_imageInfo_mask_cut (0x1U<<15)
        short cutLeft;
        short cutRight;
#define Camera_ctrl_imageInfo_mask_cycleMove (0x1U<<16)
        /* 转圈,0-表示未旋转,1-表示正在旋转 */
        unsigned char cycleMove : 1;
    };
#define Camera_ctrl_imageInfo_copy(___dst,___src,/* 哪些参数是有效的,按位摆布,如controlMode为0 */ ___setMask) do{\
    if ((___setMask) & Camera_ctrl_imageInfo_mask_autoFocus) (___dst).autoFocus = (___src).autoFocus;\
    if ((___setMask) & Camera_ctrl_imageInfo_mask_controlMode)(___dst).controlMode = (___src).controlMode;\
    if ((___dst).controlMode>0) {\
        if ((___setMask) & Camera_ctrl_imageInfo_mask_shutter) (___dst).shutter = (___src).shutter;\
        if ((___setMask) & Camera_ctrl_imageInfo_mask_gain) (___dst).gain = (___src).gain;\
        if ((___setMask) & Camera_ctrl_imageInfo_mask_iris) (___dst).iris = (___src).iris;\
    }\
    if ((___setMask) & Camera_ctrl_imageInfo_mask_whiteBalanceMode) (___dst).whiteBalanceMode = (___src).whiteBalanceMode;\
    if ((___dst).whiteBalanceMode>0) {\
        if ((___setMask) & Camera_ctrl_imageInfo_mask_whiteBalance_B) (___dst).whiteBalance_B = (___src).whiteBalance_B;\
        if ((___setMask) & Camera_ctrl_imageInfo_mask_whiteBalance_R) (___dst).whiteBalance_R = (___src).whiteBalance_R;\
    }\
    if ((___setMask) & Camera_ctrl_imageInfo_mask_imageMode) (___dst).imageMode = (___src).imageMode;\
    if ((___dst).imageMode>0) {\
        if ((___setMask) & Camera_ctrl_imageInfo_mask_brightness) (___dst).brightness = (___src).brightness;\
        if ((___setMask) & Camera_ctrl_imageInfo_mask_colorSaturation) (___dst).colorSaturation = (___src).colorSaturation;\
        if ((___setMask) & Camera_ctrl_imageInfo_mask_contrast) (___dst).contrast = (___src).contrast;\
        if ((___setMask) & Camera_ctrl_imageInfo_mask_sharpness) (___dst).sharpness = (___src).sharpness;\
    }\
    if ((___setMask) & Camera_ctrl_imageInfo_mask_intensityAdjust) (___dst).intensityAdjust = (___src).intensityAdjust;\
    if ((___setMask) & Camera_ctrl_imageInfo_mask_bw) (___dst).bwValid = (___src).bwValid,(___dst).bw = (___src).bw;\
    if ((___setMask) & Camera_ctrl_imageInfo_mask_cut) (___dst).cutLeft = (___src).cutLeft,(___dst).cutRight = (___src).cutRight;\
    if ((___setMask) & Camera_ctrl_imageInfo_mask_cycleMove) (___dst).cycleMove = (___src).cycleMove;\
}while(0)
    /* 图像设置信息根据掩码填充,指示___dst中哪些是有效的,无效部分使用___src中的值进行填充 */
#define Camera_ctrl_imageInfo_copy_fill(___dst,___src,/* 哪些参数是有效的,按位排布,如controlMode为0 */ ___setMask) do{\
    if (0==((___setMask) & Camera_ctrl_imageInfo_mask_autoFocus)) (___dst).autoFocus = (___src).autoFocus;\
    if (0==((___setMask) & Camera_ctrl_imageInfo_mask_controlMode)) (___dst).controlMode = (___src).controlMode;\
    if ((___dst).controlMode>0) {\
        if (0==((___setMask) &Camera_ctrl_imageInfo_mask_shutter)) (___dst).shutter = (___src).shutter;\
        if (0==((___setMask) &Camera_ctrl_imageInfo_mask_gain)) (___dst).gain = (___src).gain;\
        if (0==((___setMask) &Camera_ctrl_imageInfo_mask_iris)) (___dst).iris = (___src).iris;\
    } else {\
        (___dst).shutter = (___src).shutter;\
        (___dst).gain = (___src).gain;\
        (___dst).iris = (___src).iris;\
    }\
    if (0==((___setMask) & Camera_ctrl_imageInfo_mask_whiteBalanceMode)) (___dst).whiteBalanceMode = (___src).whiteBalanceMode;\
    if ((___dst).whiteBalanceMode>0) {\
        if (0==((___setMask) & Camera_ctrl_imageInfo_mask_whiteBalance_B)) (___dst).whiteBalance_B = (___src).whiteBalance_B;\
        if (0==((___setMask) & Camera_ctrl_imageInfo_mask_whiteBalance_R)) (___dst).whiteBalance_R = (___src).whiteBalance_R;\
    } else {\
        (___dst).whiteBalance_B = (___src).whiteBalance_B;\
        (___dst).whiteBalance_R = (___src).whiteBalance_R;\
    }\
    if (0==((___setMask) & Camera_ctrl_imageInfo_mask_imageMode))  (___dst).imageMode = (___src).imageMode;\
    if ((___dst).imageMode>0) {\
        if (0==((___setMask) & Camera_ctrl_imageInfo_mask_brightness)) (___dst).brightness = (___src).brightness;\
        if (0==((___setMask) & Camera_ctrl_imageInfo_mask_colorSaturation)) (___dst).colorSaturation = (___src).colorSaturation;\
        if (0==((___setMask) & Camera_ctrl_imageInfo_mask_contrast)) (___dst).contrast = (___src).contrast;\
        if (0==((___setMask) & Camera_ctrl_imageInfo_mask_sharpness)) (___dst).sharpness = (___src).sharpness;\
    } else {\
        (___dst).brightness = (___src).brightness;\
        (___dst).colorSaturation = (___src).colorSaturation;\
        (___dst).contrast = (___src).contrast;\
        (___dst).sharpness = (___src).sharpness;\
    }\
    if (0==((___setMask) & Camera_ctrl_imageInfo_mask_intensityAdjust)) (___dst).intensityAdjust = (___src).intensityAdjust;\
    if (0==((___setMask) & Camera_ctrl_imageInfo_mask_bw)) (___dst).bwValid = (___src).bwValid,(___dst).bw = (___src).bw;\
    if (0==((___setMask) & Camera_ctrl_imageInfo_mask_cut)) (___dst).cutLeft = (___src).cutLeft,(___dst).cutRight = (___src).cutRight;\
    if (0==((___setMask) & Camera_ctrl_imageInfo_mask_cycleMove)) (___dst).cycleMove = (___src).cycleMove;\
}while(0)
    struct Camera_ctrl_item;

    /* 用户控制函数结构的定义,实质为数组,不能改变顺序 */
    struct Camera_ctrl_functionUser {
        /* 取消调用者保持信息 */
        void (*cancelKey_pthreadSafety)(void * const pCamera_item, /* 为空表示不支持内部控制 */struct Camera_ctrl_item * const pCamera_ctrl_item, /* 调用者的信息,不为空 */void *const key
                , /* 缓存Buffer,不为空 */FsObjectBaseBuffer * const pObjectBaseBuffer, /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);
        /* 任意转 */
#define Camera_ctrl_Mask_turn 0
        /* 左右转 */
#define Camera_ctrl_Mask_turn_0 1
        /* 上下转 */
#define Camera_ctrl_Mask_turn_90 2
        /* 45度斜角转 */
#define Camera_ctrl_Mask_turn_45 3
        /* 变倍 */
#define Camera_ctrl_Mask_Zoom 4
        /* 聚焦 */
#define Camera_ctrl_Mask_Focus 5
        /* 预置位 */
#define Camera_ctrl_Mask_Preset 6
        /* 3d定位 */
#define Camera_ctrl_Mask_zoom3D 7
        /* gotoPTZ和getPTZ */
#define Camera_ctrl_Mask_PTZ_get_set 8
        /* 图像参数设置与获取 */
#define Camera_ctrl_Mask_ImageInfo 9
        /* 直接向机芯发送命令 */
#define Camera_ctrl_Mask_chipCmd 10
        /* 快门重置 */
#define Camera_ctrl_Mask_shutterReset 11
        /* 禁用转动自动停止 */
#define Camera_ctrl_Mask_turnAutoStop_disabled 62
        /* 禁用控制类型自动探测 */
#define Camera_ctrl_Mask_check_disabled 63
#define Camera_ctrl_Mask_ALL (~((1LLU<<Camera_ctrl_Mask_turnAutoStop_disabled)|(1LLU<<Camera_ctrl_Mask_check_disabled)))
        /* 掩码,按位排布,0-任意转,1-左右转,2-上下转,3-45度斜角转,4-变倍,5-聚焦,6-预置位,7-3d定位,8-gotoPTZ和getPTZ,9-图像参数设置与获取,10-直接向机芯发送命令 */
        unsigned long long (*mask_get_pthreadSafety)(void * const pCamera_item, /* 为空表示不支持内部控制 */struct Camera_ctrl_item * const pCamera_ctrl_item
                , /* 不为空时使用*pProtocol_name返回外部控制协议协议名,如onvif */const char * * const pProtocol_name_out
                , /* 内部控制协议协议名,不为空时把内部控制协议协议名拷贝输出,无设备名输出\0 */ char protocol_name_in_cin[], /* protocol_name_in空间大小 */const unsigned int protocol_name_inLen
                , /* 设备型号,不为空时把外部控制协议设备名拷贝输出,无设备名输出\0 */ char device_type_out_cin[], /* 设备型号,不为空时把内部控制协议设备名拷贝输出,无协议名输出\0 */ char device_type_in_cin[], /* device_type空间大小 */const unsigned int device_typeLen
                , /* 设备序列号,不为空时把外部控制协议设备序列号拷贝输出,无设备序列号输出\0 */ char device_sn_out_cin[], /* 设备序列号,不为空时把内部控制协议设备序列号拷贝输出,无设备序列号输出\0 */ char device_sn_in_cin[], /* device_sn空间大小 */const unsigned int device_snLen
                , /* 设备版本号,不为空时把外部控制协议设备版本号拷贝输出,无设备版本号输出\0 */ char device_version_out_cin[], /* 设备版本号,不为空时把内部控制协议设备版本号拷贝输出,无设备版本号输出\0 */ char device_version_in_cin[], /* device_version空间大小 */const unsigned int device_versionLen
                , /* 不为空时使用*mask_out返回外部控制协议的掩码,如onvif的掩码 */ unsigned long long *const mask_out, /* 不为空时使用*mask_in返内外部控制协议的掩码 */ unsigned long long *const mask_in
                , /* 缓存Buffer,不为空 */FsObjectBaseBuffer * const pObjectBaseBuffer, /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);
        /* 控制,任意转,成功返回1,失败返回-1,优先级不够返回-2 */
        int (*turn_pthreadSafety)(void * const pCamera_item
                , /* 为空表示不支持内部控制 */struct Camera_ctrl_item * const pCamera_ctrl_item, /* 调用者的信息,为空表示不使用调用者信息 */void *const key
                , /* 优先级,0-默认,不缓存调用者信息,1-比分析优先级低的定义,2-255:保留,256-29999:比分析优先级低的定义,30000-34999:视频分析专用的优先级,35000-64999:比分析优先级高的定义,65000-65534:保留;非0优先级中,
                 *   偶数表示可合并的优先级,即如有多个拥有相同偶数优先级的客户端访问同一相机,这些所有客户端的指令会按照新指令覆盖旧指令方式生效;
                 *   奇数表示独占优先级,即如有多个拥有相同奇数优先级的客户端访问同一相机,只有第一个客户端的指令会生效,其他客户端的指令会被拒绝 */const unsigned short priority
                , /* 参考值精度为23040,负数向左,正数向右 */const short speedx, /* 参考值精度为23040,负数向上,正数向下 */const short speedy, /* 转动时间 */const float time
                , /* 缓存Buffer,不为空 */FsObjectBaseBuffer * const pObjectBaseBuffer, /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);
        /* 控制,左转,成功返回1,失败返回-1,优先级不够返回-2 */
        int (*turnLeft_pthreadSafety)(void * const pCamera_item
                , /* 为空表示不支持内部控制 */struct Camera_ctrl_item * const pCamera_ctrl_item, /* 调用者的信息,为空表示不使用调用者信息 */void *const key
                , /* 优先级,0-默认,不缓存调用者信息,1-比分析优先级低的定义,2-255:保留,256-29999:比分析优先级低的定义,30000-34999:视频分析专用的优先级,35000-64999:比分析优先级高的定义,65000-65534:保留;非0优先级中,
                 *   偶数表示可合并的优先级,即如有多个拥有相同偶数优先级的客户端访问同一相机,这些所有客户端的指令会按照新指令覆盖旧指令方式生效;
                 *   奇数表示独占优先级,即如有多个拥有相同奇数优先级的客户端访问同一相机,只有第一个客户端的指令会生效,其他客户端的指令会被拒绝 */const unsigned short priority
                , /* 参考值精度为23040 */const unsigned short speed, /* 转动时间 */const float time
                , /* 缓存Buffer,不为空 */FsObjectBaseBuffer * const pObjectBaseBuffer, /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);
        /* 控制,右转,成功返回1,失败返回-1,优先级不够返回-2 */
        int (*turnRight_pthreadSafety)(void * const pCamera_item
                , /* 为空表示不支持内部控制 */struct Camera_ctrl_item * const pCamera_ctrl_item, /* 调用者的信息,为空表示不使用调用者信息 */void *const key
                , /* 优先级,0-默认,不缓存调用者信息,1-比分析优先级低的定义,2-255:保留,256-29999:比分析优先级低的定义,30000-34999:视频分析专用的优先级,35000-64999:比分析优先级高的定义,65000-65534:保留;非0优先级中,
                 *   偶数表示可合并的优先级,即如有多个拥有相同偶数优先级的客户端访问同一相机,这些所有客户端的指令会按照新指令覆盖旧指令方式生效;
                 *   奇数表示独占优先级,即如有多个拥有相同奇数优先级的客户端访问同一相机,只有第一个客户端的指令会生效,其他客户端的指令会被拒绝 */const unsigned short priority
                , /* 参考值精度为23040 */const unsigned short speed, /* 转动时间 */const float time
                , /* 缓存Buffer,不为空 */FsObjectBaseBuffer * const pObjectBaseBuffer, /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);
        /* 控制,上转,成功返回1,失败返回-1,优先级不够返回-2 */
        int (*turnUp_pthreadSafety)(void * const pCamera_item
                , /* 为空表示不支持内部控制 */struct Camera_ctrl_item * const pCamera_ctrl_item, /* 调用者的信息,为空表示不使用调用者信息 */void *const key
                , /* 优先级,0-默认,不缓存调用者信息,1-比分析优先级低的定义,2-255:保留,256-29999:比分析优先级低的定义,30000-34999:视频分析专用的优先级,35000-64999:比分析优先级高的定义,65000-65534:保留;非0优先级中,
                 *   偶数表示可合并的优先级,即如有多个拥有相同偶数优先级的客户端访问同一相机,这些所有客户端的指令会按照新指令覆盖旧指令方式生效;
                 *   奇数表示独占优先级,即如有多个拥有相同奇数优先级的客户端访问同一相机,只有第一个客户端的指令会生效,其他客户端的指令会被拒绝 */const unsigned short priority
                , /* 参考值精度为23040 */const unsigned short speed, /* 转动时间 */const float time
                , /* 缓存Buffer,不为空 */FsObjectBaseBuffer * const pObjectBaseBuffer, /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);
        /* 控制,下转,成功返回1,失败返回-1,优先级不够返回-2 */
        int (*turnDown_pthreadSafety)(void * const pCamera_item
                , /* 为空表示不支持内部控制 */struct Camera_ctrl_item * const pCamera_ctrl_item, /* 调用者的信息,为空表示不使用调用者信息 */void *const key
                , /* 优先级,0-默认,不缓存调用者信息,1-比分析优先级低的定义,2-255:保留,256-29999:比分析优先级低的定义,30000-34999:视频分析专用的优先级,35000-64999:比分析优先级高的定义,65000-65534:保留;非0优先级中,
                 *   偶数表示可合并的优先级,即如有多个拥有相同偶数优先级的客户端访问同一相机,这些所有客户端的指令会按照新指令覆盖旧指令方式生效;
                 *   奇数表示独占优先级,即如有多个拥有相同奇数优先级的客户端访问同一相机,只有第一个客户端的指令会生效,其他客户端的指令会被拒绝 */const unsigned short priority
                , /* 参考值精度为23040 */const unsigned short speed, /* 转动时间 */const float time
                , /* 缓存Buffer,不为空 */FsObjectBaseBuffer * const pObjectBaseBuffer, /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);
        /* 控制,左上转,成功返回1,失败返回-1,优先级不够返回-2 */
        int (*turnLeftUp_pthreadSafety)(void * const pCamera_item
                , /* 为空表示不支持内部控制 */struct Camera_ctrl_item * const pCamera_ctrl_item, /* 调用者的信息,为空表示不使用调用者信息 */void *const key
                , /* 优先级,0-默认,不缓存调用者信息,1-比分析优先级低的定义,2-255:保留,256-29999:比分析优先级低的定义,30000-34999:视频分析专用的优先级,35000-64999:比分析优先级高的定义,65000-65534:保留;非0优先级中,
                 *   偶数表示可合并的优先级,即如有多个拥有相同偶数优先级的客户端访问同一相机,这些所有客户端的指令会按照新指令覆盖旧指令方式生效;
                 *   奇数表示独占优先级,即如有多个拥有相同奇数优先级的客户端访问同一相机,只有第一个客户端的指令会生效,其他客户端的指令会被拒绝 */const unsigned short priority
                , /* 参考值精度为23040 */const unsigned short speed, /* 转动时间 */const float time
                , /* 缓存Buffer,不为空 */FsObjectBaseBuffer * const pObjectBaseBuffer, /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);
        /* 控制,左下转,成功返回1,失败返回-1,优先级不够返回-2 */
        int (*turnLeftDown_pthreadSafety)(void * const pCamera_item
                , /* 为空表示不支持内部控制 */struct Camera_ctrl_item * const pCamera_ctrl_item, /* 调用者的信息,为空表示不使用调用者信息 */void *const key
                , /* 优先级,0-默认,不缓存调用者信息,1-比分析优先级低的定义,2-255:保留,256-29999:比分析优先级低的定义,30000-34999:视频分析专用的优先级,35000-64999:比分析优先级高的定义,65000-65534:保留;非0优先级中,
                 *   偶数表示可合并的优先级,即如有多个拥有相同偶数优先级的客户端访问同一相机,这些所有客户端的指令会按照新指令覆盖旧指令方式生效;
                 *   奇数表示独占优先级,即如有多个拥有相同奇数优先级的客户端访问同一相机,只有第一个客户端的指令会生效,其他客户端的指令会被拒绝 */const unsigned short priority
                , /* 参考值精度为23040 */const unsigned short speed, /* 转动时间 */const float time
                , /* 缓存Buffer,不为空 */FsObjectBaseBuffer * const pObjectBaseBuffer, /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);
        /* 控制,右上转,成功返回1,失败返回-1,优先级不够返回-2 */
        int (*turnRightUp_pthreadSafety)(void * const pCamera_item
                , /* 为空表示不支持内部控制 */struct Camera_ctrl_item * const pCamera_ctrl_item, /* 调用者的信息,为空表示不使用调用者信息 */void *const key
                , /* 优先级,0-默认,不缓存调用者信息,1-比分析优先级低的定义,2-255:保留,256-29999:比分析优先级低的定义,30000-34999:视频分析专用的优先级,35000-64999:比分析优先级高的定义,65000-65534:保留;非0优先级中,
                 *   偶数表示可合并的优先级,即如有多个拥有相同偶数优先级的客户端访问同一相机,这些所有客户端的指令会按照新指令覆盖旧指令方式生效;
                 *   奇数表示独占优先级,即如有多个拥有相同奇数优先级的客户端访问同一相机,只有第一个客户端的指令会生效,其他客户端的指令会被拒绝 */const unsigned short priority
                , /* 参考值精度为23040 */const unsigned short speed, /* 转动时间 */const float time
                , /* 缓存Buffer,不为空 */FsObjectBaseBuffer * const pObjectBaseBuffer, /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);
        /* 控制,右下转,成功返回1,失败返回-1,优先级不够返回-2 */
        int (*turnRightDown_pthreadSafety)(void * const pCamera_item
                , /* 为空表示不支持内部控制 */struct Camera_ctrl_item * const pCamera_ctrl_item, /* 调用者的信息,为空表示不使用调用者信息 */void *const key
                , /* 优先级,0-默认,不缓存调用者信息,1-比分析优先级低的定义,2-255:保留,256-29999:比分析优先级低的定义,30000-34999:视频分析专用的优先级,35000-64999:比分析优先级高的定义,65000-65534:保留;非0优先级中,
                 *   偶数表示可合并的优先级,即如有多个拥有相同偶数优先级的客户端访问同一相机,这些所有客户端的指令会按照新指令覆盖旧指令方式生效;
                 *   奇数表示独占优先级,即如有多个拥有相同奇数优先级的客户端访问同一相机,只有第一个客户端的指令会生效,其他客户端的指令会被拒绝 */const unsigned short priority
                , /* 参考值精度为23040 */const unsigned short speed, /* 转动时间 */const float time
                , /* 缓存Buffer,不为空 */FsObjectBaseBuffer * const pObjectBaseBuffer, /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);
        /* 控制,变倍,成功返回1,失败返回-1,优先级不够返回-2 */
        int (*zoom_pthreadSafety)(void * const pCamera_item
                , /* 为空表示不支持内部控制 */struct Camera_ctrl_item * const pCamera_ctrl_item, /* 调用者的信息,为空表示不使用调用者信息 */void *const key
                , /* 优先级,0-默认,不缓存调用者信息,1-比分析优先级低的定义,2-255:保留,256-29999:比分析优先级低的定义,30000-34999:视频分析专用的优先级,35000-64999:比分析优先级高的定义,65000-65534:保留;非0优先级中,
                 *   偶数表示可合并的优先级,即如有多个拥有相同偶数优先级的客户端访问同一相机,这些所有客户端的指令会按照新指令覆盖旧指令方式生效;
                 *   奇数表示独占优先级,即如有多个拥有相同奇数优先级的客户端访问同一相机,只有第一个客户端的指令会生效,其他客户端的指令会被拒绝 */const unsigned short priority
                , /* 参考值精度为23040,负数缩小,正数放大 */const short speed, /* 变倍时间 */const float time
                , /* 缓存Buffer,不为空 */FsObjectBaseBuffer * const pObjectBaseBuffer, /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);
        /* 控制,聚焦,成功返回1,失败返回-1,优先级不够返回-2 */
        int (*focus_pthreadSafety)(void * const pCamera_item
                , /* 为空表示不支持内部控制 */struct Camera_ctrl_item * const pCamera_ctrl_item, /* 调用者的信息,为空表示不使用调用者信息 */void *const key
                , /* 优先级,0-默认,不缓存调用者信息,1-比分析优先级低的定义,2-255:保留,256-29999:比分析优先级低的定义,30000-34999:视频分析专用的优先级,35000-64999:比分析优先级高的定义,65000-65534:保留;非0优先级中,
                 *   偶数表示可合并的优先级,即如有多个拥有相同偶数优先级的客户端访问同一相机,这些所有客户端的指令会按照新指令覆盖旧指令方式生效;
                 *   奇数表示独占优先级,即如有多个拥有相同奇数优先级的客户端访问同一相机,只有第一个客户端的指令会生效,其他客户端的指令会被拒绝 */const unsigned short priority
                , /* 参考值精度为23040,负数-,正数+ */const short speed, /* 聚焦时间 */const float time
                , /* 缓存Buffer,不为空 */FsObjectBaseBuffer * const pObjectBaseBuffer, /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);
        /* 控制,设置预置位,成功返回1,失败返回-1,优先级不够返回-2 */
        int (*preset_set_pthreadSafety)(void * const pCamera_item
                , /* 为空表示不支持内部控制 */struct Camera_ctrl_item * const pCamera_ctrl_item, /* 调用者的信息,为空表示不使用调用者信息 */void *const key
                , /* 优先级,0-默认,不缓存调用者信息,1-比分析优先级低的定义,2-255:保留,256-29999:比分析优先级低的定义,30000-34999:视频分析专用的优先级,35000-64999:比分析优先级高的定义,65000-65534:保留;非0优先级中,
                 *   偶数表示可合并的优先级,即如有多个拥有相同偶数优先级的客户端访问同一相机,这些所有客户端的指令会按照新指令覆盖旧指令方式生效;
                 *   奇数表示独占优先级,即如有多个拥有相同奇数优先级的客户端访问同一相机,只有第一个客户端的指令会生效,其他客户端的指令会被拒绝 */const unsigned short priority
                , /* 预置位号 */const unsigned short presetIndex
                , /* 缓存Buffer,不为空 */FsObjectBaseBuffer * const pObjectBaseBuffer, /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);
        /* 控制,调用预置位,成功返回1,失败返回-1,优先级不够返回-2 */
        int (*preset_goto_pthreadSafety)(void * const pCamera_item
                , /* 为空表示不支持内部控制 */struct Camera_ctrl_item * const pCamera_ctrl_item, /* 调用者的信息,为空表示不使用调用者信息 */void *const key
                , /* 优先级,0-默认,不缓存调用者信息,1-比分析优先级低的定义,2-255:保留,256-29999:比分析优先级低的定义,30000-34999:视频分析专用的优先级,35000-64999:比分析优先级高的定义,65000-65534:保留;非0优先级中,
                 *   偶数表示可合并的优先级,即如有多个拥有相同偶数优先级的客户端访问同一相机,这些所有客户端的指令会按照新指令覆盖旧指令方式生效;
                 *   奇数表示独占优先级,即如有多个拥有相同奇数优先级的客户端访问同一相机,只有第一个客户端的指令会生效,其他客户端的指令会被拒绝 */const unsigned short priority
                , /* 预置位号 */const unsigned short presetIndex
                , /* 缓存Buffer,不为空 */FsObjectBaseBuffer * const pObjectBaseBuffer, /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);
        /* 控制,清除预置位,成功返回1,失败返回-1,优先级不够返回-2 */
        int (*preset_clean_pthreadSafety)(void * const pCamera_item
                , /* 为空表示不支持内部控制 */struct Camera_ctrl_item * const pCamera_ctrl_item, /* 调用者的信息,为空表示不使用调用者信息 */void *const key
                , /* 优先级,0-默认,不缓存调用者信息,1-比分析优先级低的定义,2-255:保留,256-29999:比分析优先级低的定义,30000-34999:视频分析专用的优先级,35000-64999:比分析优先级高的定义,65000-65534:保留;非0优先级中,
                 *   偶数表示可合并的优先级,即如有多个拥有相同偶数优先级的客户端访问同一相机,这些所有客户端的指令会按照新指令覆盖旧指令方式生效;
                 *   奇数表示独占优先级,即如有多个拥有相同奇数优先级的客户端访问同一相机,只有第一个客户端的指令会生效,其他客户端的指令会被拒绝 */const unsigned short priority
                , /* 预置位号 */const unsigned short presetIndex
                , /* 缓存Buffer,不为空 */FsObjectBaseBuffer * const pObjectBaseBuffer, /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);
        /* 控制,3D定位,成功返回1,失败返回-1,优先级不够返回-2 */
        int (*zoom3D_pthreadSafety)(void * const pCamera_item
                , /* 为空表示不支持内部控制 */struct Camera_ctrl_item * const pCamera_ctrl_item, /* 调用者的信息,为空表示不使用调用者信息 */void *const key
                , /* 优先级,0-默认,不缓存调用者信息,1-比分析优先级低的定义,2-255:保留,256-29999:比分析优先级低的定义,30000-34999:视频分析专用的优先级,35000-64999:比分析优先级高的定义,65000-65534:保留;非0优先级中,
                 *   偶数表示可合并的优先级,即如有多个拥有相同偶数优先级的客户端访问同一相机,这些所有客户端的指令会按照新指令覆盖旧指令方式生效;
                 *   奇数表示独占优先级,即如有多个拥有相同奇数优先级的客户端访问同一相机,只有第一个客户端的指令会生效,其他客户端的指令会被拒绝 */const unsigned short priority
                , /* 相对图大小参考值23040 */const unsigned short x1, /* 相对图大小参考值23040 */const unsigned short y1, /* 相对图大小参考值23040 */const unsigned short x2, /* 相对图大小参考值23040 */const unsigned short y2
                , /* 缓存Buffer,不为空 */FsObjectBaseBuffer * const pObjectBaseBuffer, /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);
        /* 控制,goto PTZ,成功返回1,失败返回-1,优先级不够返回-2 */
        int (*ptz_goto_pthreadSafety)(void * const pCamera_item
                , /* 为空表示不支持内部控制 */struct Camera_ctrl_item * const pCamera_ctrl_item, /* 调用者的信息,为空表示不使用调用者信息 */void *const key
                , /* 优先级,0-默认,不缓存调用者信息,1-比分析优先级低的定义,2-255:保留,256-29999:比分析优先级低的定义,30000-34999:视频分析专用的优先级,35000-64999:比分析优先级高的定义,65000-65534:保留;非0优先级中,
                 *   偶数表示可合并的优先级,即如有多个拥有相同偶数优先级的客户端访问同一相机,这些所有客户端的指令会按照新指令覆盖旧指令方式生效;
                 *   奇数表示独占优先级,即如有多个拥有相同奇数优先级的客户端访问同一相机,只有第一个客户端的指令会生效,其他客户端的指令会被拒绝 */const unsigned short priority
                , /* 相对参考值23040 */const unsigned short P, /* 相对参考值23040 */const unsigned short T, /* 相对参考值23040 */const unsigned short Z
                , /* 缓存Buffer,不为空 */FsObjectBaseBuffer * const pObjectBaseBuffer, /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);
        /* 控制,获取PTZ,成功返回1,失败返回-1,对没有优先级不够的客户端会直接读取缓存返回数据 */
        int (*ptz_get_pthreadSafety)(void * const pCamera_item
                , /* 为空表示不支持内部控制 */struct Camera_ctrl_item * const pCamera_ctrl_item, /* 调用者的信息,为空表示不使用调用者信息 */void *const key
                , /* 优先级,0-默认,不缓存调用者信息,1-比分析优先级低的定义,2-255:保留,256-29999:比分析优先级低的定义,30000-34999:视频分析专用的优先级,35000-64999:比分析优先级高的定义,65000-65534:保留;非0优先级中,
                 *   偶数表示可合并的优先级,即如有多个拥有相同偶数优先级的客户端访问同一相机,这些所有客户端的指令会按照新指令覆盖旧指令方式生效;
                 *   奇数表示独占优先级,即如有多个拥有相同奇数优先级的客户端访问同一相机,只有第一个客户端的指令会生效,其他客户端的指令会被拒绝 */const unsigned short priority, /* 相对参考值23040 */ unsigned short*const P
                , /* 相对参考值23040 */ unsigned short* const T, /* 相对参考值23040 */ unsigned short* const Z
                , /* 缓存Buffer,不为空 */FsObjectBaseBuffer * const pObjectBaseBuffer, /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);
        /* 控制,设置图像参数,成功返回1,失败返回-1,对没有优先级不够的客户端会直接读取缓存返回数据 */
        int (*imageInfo_set_pthreadSafety)(void * const pCamera_item
                , /* 为空表示不支持内部控制 */struct Camera_ctrl_item * const pCamera_ctrl_item, /* 调用者的信息,为空表示不使用调用者信息 */void *const key
                , /* 优先级,0-默认,不缓存调用者信息,1-比分析优先级低的定义,2-255:保留,256-29999:比分析优先级低的定义,30000-34999:视频分析专用的优先级,35000-64999:比分析优先级高的定义,65000-65534:保留;非0优先级中,
                 *   偶数表示可合并的优先级,即如有多个拥有相同偶数优先级的客户端访问同一相机,这些所有客户端的指令会按照新指令覆盖旧指令方式生效;
                 *   奇数表示独占优先级,即如有多个拥有相同奇数优先级的客户端访问同一相机,只有第一个客户端的指令会生效,其他客户端的指令会被拒绝 */const unsigned short priority
                , /* 哪些参数是有效的,按位摆布,如controlMode为0 */unsigned int setMask, /* 图像信息 */ struct Camera_ctrl_imageInfo * const imageInfo
                , /* 缓存Buffer,不为空 */FsObjectBaseBuffer * const pObjectBaseBuffer, /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);
        /* 控制,获取图像参数,成功返回1,失败返回-1,对没有优先级不够的客户端会直接读取缓存返回数据 */
        int (*imageInfo_get_pthreadSafety)(void * const pCamera_item
                , /* 为空表示不支持内部控制 */struct Camera_ctrl_item * const pCamera_ctrl_item, /* 调用者的信息,为空表示不使用调用者信息 */void *const key
                , /* 优先级,0-默认,不缓存调用者信息,1-比分析优先级低的定义,2-255:保留,256-29999:比分析优先级低的定义,30000-34999:视频分析专用的优先级,35000-64999:比分析优先级高的定义,65000-65534:保留;非0优先级中,
                 *   偶数表示可合并的优先级,即如有多个拥有相同偶数优先级的客户端访问同一相机,这些所有客户端的指令会按照新指令覆盖旧指令方式生效;
                 *   奇数表示独占优先级,即如有多个拥有相同奇数优先级的客户端访问同一相机,只有第一个客户端的指令会生效,其他客户端的指令会被拒绝 */const unsigned short priority
                , /* 图像信息 */ struct Camera_ctrl_imageInfo * const imageInfo
                , /* 缓存Buffer,不为空 */FsObjectBaseBuffer * const pObjectBaseBuffer, /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);
        /* 控制,向机芯直接发送命令,成功返回1,失败返回-1,优先级不够返回-2 */
        int (*chipCmd_pthreadSafety)(void * const pCamera_item
                , /* 为空表示不支持内部控制 */struct Camera_ctrl_item * const pCamera_ctrl_item, /* 调用者的信息,为空表示不使用调用者信息 */void *const key
                , /* 优先级,0-默认,不缓存调用者信息,1-比分析优先级低的定义,2-255:保留,256-29999:比分析优先级低的定义,30000-34999:视频分析专用的优先级,35000-64999:比分析优先级高的定义,65000-65534:保留;非0优先级中,
                 *   偶数表示可合并的优先级,即如有多个拥有相同偶数优先级的客户端访问同一相机,这些所有客户端的指令会按照新指令覆盖旧指令方式生效;
                 *   奇数表示独占优先级,即如有多个拥有相同奇数优先级的客户端访问同一相机,只有第一个客户端的指令会生效,其他客户端的指令会被拒绝 */const unsigned short priority
                , /* 命令的数据类型,0-字符串,1-二进制(cmd以字符串输入时使用base64编码) */ const unsigned char dataType, /* 命令 */const char cmd[], /* 命令长度 */const unsigned int cmdLen
                , /* 缓存Buffer,不为空 */FsObjectBaseBuffer * const pObjectBaseBuffer, /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);
        /* 控制,快门重置,成功返回1,失败返回-1,优先级不够返回-2 */
        int (*shutterReset_pthreadSafety)(void * const pCamera_item
                , /* 为空表示不支持内部控制 */struct Camera_ctrl_item * const pCamera_ctrl_item, /* 调用者的信息,为空表示不使用调用者信息 */void *const key
                , /* 优先级,0-默认,不缓存调用者信息,1-比分析优先级低的定义,2-255:保留,256-29999:比分析优先级低的定义,30000-34999:视频分析专用的优先级,35000-64999:比分析优先级高的定义,65000-65534:保留;非0优先级中,
                 *   偶数表示可合并的优先级,即如有多个拥有相同偶数优先级的客户端访问同一相机,这些所有客户端的指令会按照新指令覆盖旧指令方式生效;
                 *   奇数表示独占优先级,即如有多个拥有相同奇数优先级的客户端访问同一相机,只有第一个客户端的指令会生效,其他客户端的指令会被拒绝 */const unsigned short priority
                , /* 缓存Buffer,不为空 */FsObjectBaseBuffer * const pObjectBaseBuffer, /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);
    };

    /* 控制函数结构的定义,实质为数组,不能改变顺序 */
    struct Camera_ctrl_function {
        /* 掩码,按位排布,0-任意转,1-左右转,2-上下转,3-45度斜角转,4-变倍,5-聚焦,6-预置位,7-3d定位,8-gotoPTZ和getPTZ,9-图像参数设置与获取,10-直接向机芯发送命令 */
        unsigned long long (*mask_get)(void * const pCamera_item, /* 不为空时使用*pProtocol_name返回控制协议协议名,如onvif */const char * * const pProtocol_name
                , /* 设备型号,不为空时把设备名拷贝输出,无设备名输出\0 */ char device_type[], /* device_type空间大小 */const unsigned int device_typeLen
                , /* 设备序列号,不为空时把设备序列号拷贝输出,无设备序列号输出\0 */ char device_sn[], /* device_sn空间大小 */const unsigned int device_snLen
                , /* 设备版本号,不为空时把设备版本号拷贝输出,无设备版本号输出\0 */ char device_version[], /* device_version空间大小 */const unsigned int device_versionLen);
        /* 控制,任意转,成功返回1,失败返回-1 */
        int (*turn_pthreadSafety)(void * const pCamera_item, /* 参考值精度为23040,负数向左,正数向右 */const short speedx
                , /* 参考值精度为23040,负数向上,正数向下 */const short speedy, /* 转动时间 */const float time);
        /* 控制,左转,成功返回1,失败返回-1 */
        int (*turnLeft_pthreadSafety)(void * const pCamera_item, /* 参考值精度为23040 */const unsigned short speed, /* 转动时间 */const float time);
        /* 控制,右转,成功返回1,失败返回-1 */
        int (*turnRight_pthreadSafety)(void * const pCamera_item, /* 参考值精度为23040 */const unsigned short speed, /* 转动时间 */const float time);
        /* 控制,上转,成功返回1,失败返回-1 */
        int (*turnUp_pthreadSafety)(void * const pCamera_item, /* 参考值精度为23040 */const unsigned short speed, /* 转动时间 */const float time);
        /* 控制,下转,成功返回1,失败返回-1 */
        int (*turnDown_pthreadSafety)(void * const pCamera_item, /* 参考值精度为23040 */const unsigned short speed, /* 转动时间 */const float time);
        /* 控制,左上转,成功返回1,失败返回-1 */
        int (*turnLeftUp_pthreadSafety)(void * const pCamera_item, /* 参考值精度为23040 */const unsigned short speed, /* 转动时间 */const float time);
        /* 控制,左下转,成功返回1,失败返回-1 */
        int (*turnLeftDown_pthreadSafety)(void * const pCamera_item, /* 参考值精度为23040 */const unsigned short speed, /* 转动时间 */const float time);
        /* 控制,右上转,成功返回1,失败返回-1 */
        int (*turnRightUp_pthreadSafety)(void * const pCamera_item, /* 参考值精度为23040 */const unsigned short speed, /* 转动时间 */const float time);
        /* 控制,右下转,成功返回1,失败返回-1 */
        int (*turnRightDown_pthreadSafety)(void * const pCamera_item, /* 参考值精度为23040 */const unsigned short speed, /* 转动时间 */const float time);
        /* 控制,变倍,成功返回1,失败返回-1 */
        int (*zoom_pthreadSafety)(void * const pCamera_item, /* 参考值精度为23040,负数缩小,正数放大 */const short speed, /* 变倍时间 */const float time);
        /* 控制,聚焦,成功返回1,失败返回-1 */
        int (*focus_pthreadSafety)(void * const pCamera_item, /* 参考值精度为23040,负数-,正数+ */const short speed, /* 聚焦时间 */const float time);
        /* 控制,设置预置位,成功返回1,失败返回-1 */
        int (*preset_set_pthreadSafety)(void * const pCamera_item, /* 预置位号 */const unsigned short presetIndex);
        /* 控制,调用预置位,成功返回1,失败返回-1 */
        int (*preset_goto_pthreadSafety)(void * const pCamera_item, /* 预置位号 */const unsigned short presetIndex);
        /* 控制,清除预置位,成功返回1,失败返回-1 */
        int (*preset_clean_pthreadSafety)(void * const pCamera_item, /* 预置位号 */const unsigned short presetIndex);
        /* 控制,3D定位,成功返回1,失败返回-1 */
        int (*zoom3D_pthreadSafety)(void * const pCamera_item, /* 相对图大小参考值23040 */const unsigned short x1
                , /* 相对图大小参考值23040 */const unsigned short y1, /* 相对图大小参考值23040 */const unsigned short x2, /* 相对图大小参考值23040 */const unsigned short y2);
        /* 控制,goto PTZ,成功返回1,失败返回-1 */
        int (*ptz_goto_pthreadSafety)(void * const pCamera_item, /* 相对参考值23040,大于23040表示无效值 */const unsigned short P, /* 相对参考值23040,大于23040表示无效值 */const unsigned short T, /* 相对参考值23040,大于23040表示无效值 */const unsigned short Z);
        /* 控制,获取PTZ,成功返回1,失败返回-1 */
        int (*ptz_get_pthreadSafety) (void * const pCamera_item, /* 相对参考值23040 */ unsigned short*const P
                , /* 相对参考值23040 */ unsigned short* const T, /* 相对参考值23040 */ unsigned short* const Z, /* 是否更新,1-更新,0-直接读取内存 */const unsigned char update);
        /* 控制,设置图像参数,成功返回1,失败返回-1 */
        int (*imageInfo_set_pthreadSafety)(void * const pCamera_item, /* 哪些参数是有效的,按位摆布,如controlMode为0 */unsigned int setMask, /* 图像信息 */ struct Camera_ctrl_imageInfo * const imageInfo);
        /* 控制,获取图像参数,成功返回1,失败返回-1 */
        int (*imageInfo_get_pthreadSafety)(void * const pCamera_item, /* 图像信息 */ struct Camera_ctrl_imageInfo * const imageInfo, /* 是否更新,1-更新,0-直接读取内存 */const unsigned char update);
        /* 控制,向机芯直接发送命令,成功返回1,失败返回-1 */
        int (*chipCmd_pthreadSafety)(void * const pCamera_item, /* 命令的数据类型,0-字符串,1-二进制(cmd以字符串输入时使用base64编码) */ const unsigned char dataType, /* 命令 */const char cmd[], /* 命令长度 */const unsigned int cmdLen);
        /* 控制,快门重置,成功返回1,失败返回-1 */
        int (*shutterReset_pthreadSafety)(void * const pCamera_item);
    };

    /* 相机控制 */
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
            /* 管道,用于添加和删除通道或发送数据 */
            int __pipe[2];
            /* 线程名 */
#define Camera_ctrl_get_threadName(___pCamera_ctrl) (((char*)((___pCamera_ctrl)+1))+(___pCamera_ctrl)->ro._threadName_)
            unsigned char _threadName_;
            /* 配置管理对象,为空表示没有pConfigManager,不能通知外部相机控制 */
            void *_pConfigManager;
            /* 连接池,为空内部使用独立的连接池 */
            SocketPool *_pSocketPool;
            /* 处理的相机链表,按指针排序 */
            FsObjectList *__itemListOrder_;
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
            /* 外部和内部线程异步通信的链表,结构为Camera_ctrl_P_control */
            FsStructList *__controlList;
        } rw;

        struct {
            /* 线程是否在运行(初始化为0,开始线程时置1,线程内部置为3后开始运行,线程退出时置为2,调用结束函数后置为0) */
            unsigned char inRun : 2;
            /* 线程的tid号 */
            pthread_t stid;
            /* 缓存的获取控制信息客户端的连接,结构为Camera_ctrl_P_referConnect */
            FsStructList *ctrlClientListOrder;
            /* 把需要检查是与哪个引用连接的相关的Camera_ctrl_item指针存于与此链表中,使用ctrlClientListOrder的互斥锁 */
            FsObjectList *checkReferConnectList__ctrlClientListOrder;
            /* 使用struct ConfigManager_connectNode_useOnce结构缓存一些请求,用于报告一些线程参数,使用ctrlClientListOrder的线程锁 */
            void * pConnectNode__ctrlClientListOrder;
        } p;
    } Camera_ctrl;
#define __Camera_ctrlLog(___Log,___loglevel,___format, ...)\
    ___Log(___loglevel,5,___format",p=%p,runStatus=%hhd/%hhu,tid=%ld\n  pMonitor=%p\n  externRunStatus=%p\n  parentTid=%ld\n  pipe=%d/%d\n  threadName:\"%s\"\n\
  pConfigManager=%p,pSocketPool=%p,itemList_=%p/%lu,controlList=%p/%lu\n",##__VA_ARGS__\
        ,pCamera_ctrl,((Camera_ctrl*)(pCamera_ctrl))->rw.runStatus,((Camera_ctrl*)(pCamera_ctrl))->p.inRun,((Camera_ctrl*)(pCamera_ctrl))->p.stid,((Camera_ctrl*)(pCamera_ctrl))->ro._pMonitor\
        ,((Camera_ctrl*)(pCamera_ctrl))->ro._externRunStatus,((Camera_ctrl*)(pCamera_ctrl))->ro._parentTid,((Camera_ctrl*)(pCamera_ctrl))->ro.__pipe[0], ((Camera_ctrl*)(pCamera_ctrl))->ro.__pipe[1],Camera_ctrl_get_threadName(pCamera_ctrl)\
        ,((Camera_ctrl*)(pCamera_ctrl))->ro._pConfigManager,((Camera_ctrl*)(pCamera_ctrl))->ro._pSocketPool,((Camera_ctrl*)(pCamera_ctrl))->ro.__itemListOrder_,((Camera_ctrl*)(pCamera_ctrl))->ro.__itemListOrder_->nodeCount\
        ,((Camera_ctrl*)(pCamera_ctrl))->rw.__controlList,((Camera_ctrl*)(pCamera_ctrl))->rw.__controlList->nodeCount)
#define Camera_ctrlLog(___loglevel,___format, ...) __Camera_ctrlLog(FsLog,___loglevel,___format,##__VA_ARGS__)
#define Camera_ctrlLog2(___loglevel,___format, ...) __Camera_ctrlLog(FsLog2,___loglevel,___format,##__VA_ARGS__)

    /* 相机控制项 */
    struct Camera_ctrl_item {
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
        /* 在ipv4Port为0时,本结构分配的空间 */
#define Camera_ctrl_item_get_structLen_ipv4Port0 FsPointer_to_integer(&((struct Camera_ctrl_item*)0)->p._ipv4)

        struct {
            /* 通道标识,可为空 */
#define Camera_ctrl_item_get_uuid(___pCamera_ctrl_item,___nullValue) \
    (!FsNumberIsMax((___pCamera_ctrl_item)->ro._uuid_)?((((char*)(___pCamera_ctrl_item))+Camera_ctrl_item_get_structLen_ipv4Port0)+(___pCamera_ctrl_item)->ro._uuid_):(___nullValue))
            unsigned short _uuid_;
            /* 端口,为0表示不主动对外连接 */
            unsigned short _ipv4Port;
            //            /* 数据连接对象,为空表示无须对外连接 */
            //            struct SocketPool_item *__pSocketPool_item;
            /* 互斥锁,在修改 */
            pthread_mutex_t __mutex;
        } ro;

        struct {
            /* 当前PTZ */
            unsigned short P, T, Z;
            /* 当前图像信息,使用mutex互斥锁 */
            struct Camera_ctrl_imageInfo imageInfo__mutex;
        } rw;

        struct {
            /* 所属的pCamera_ctrl对象,不能根据其是否为空来判断是否加入或移除线程,即此指针仅在添加时赋值 */
            Camera_ctrl *pCamera_ctrl;
            /* 是否繁忙,1-繁忙,0-空闲,外部接口设置为1,内部线程设置为0 */
#define Camera_ctrl_item_wait(___pCamera_ctrl_item) do{while((___pCamera_ctrl_item)->p.busy)FsWaitSyncDo();}while(0)
            unsigned char busy : 1;
            /* 控制状态,值仅在接口函数中修改,内部线程不修改也不读取此值,(controlStatus&0x1)-表示是否添加或移除 */
            unsigned char controlStatus : 2;
            //            unsigned char : 5;
            //            /* 心跳类型
            //             * 0-ptz停止;
            //             * 1-获取ptz;
            //             */
#define Camera_ctrl_item_send_heartbeat_interval 0.5 // 发送心跳的间隔时间,主要用于获取相机参数
            /*  requestID_3的状态,0-表示无效,1-表示尝试关联中,3-表示关联正常 */
            unsigned char requestID_status;
            /* 在ipv4Port为0时此处表示与哪个引用连接关联,需要查找使用关键字在Camera_ctrl_P_referConnect中查找后才能使用
             * 在ipv4Port为0时不为0时此处为SocketPool_item的连接指针
             */
            unsigned int requestID_3[3];
            //#define Camera_ctrl_item_send_heartbeat_cycle 2 // 一个周期的数量
            //#define Camera_ctrl_ys_http_item_send_heartbeat_set_ptzStop(___pCamera_ctrl_ys_http_item) (___pCamera_ctrl_ys_http_item)->p.heartbeatType=0 // ptz停止时应设置heartbeatType为多少
            //#define Camera_ctrl_ys_http_item_send_heartbeat_set_ptzGet(___pCamera_ctrl_ys_http_item) (___pCamera_ctrl_ys_http_item)->p.heartbeatType=1 // ptzGet时应设置heartbeatType为多少
            //            unsigned char heartbeatType : 1;
            /* 掩码,按位排布,0-任意转,1-左右转,2-上下转,3-45度斜角转,4-变倍,5-聚焦,6-预置位,7-3d定位,8-gotoPTZ和getPTZ,9-图像参数设置与获取,10-直接向机芯发送命令 */
            unsigned long long mask;
            /* 控制协议协议名,为空表示没有,使用__protocol_name_device_type_device_sn_device_version__mutex的空间 */
            char *protocol_name;
            /* 设备型号,为空表示没有,使用__protocol_name_device_type_device_sn_device_version__mutex的空间 */
            char *device_type;
            /* 设备序列号,为空表示没有,使用__protocol_name_device_type_device_sn_device_version__mutex的空间 */
            char *device_sn;
            /* 设备版本号,为空表示没有,使用__protocol_name_device_type_device_sn_device_version__mutex的空间 */
            char *device_version;
            /* protocol_name,device_type,device_sn,device_version用的空间,使用mutex互斥锁 */
            char *__protocol_name_device_type_device_sn_device_version__mutex;
            /* ip地址,为0表示未设置,不应出现域名和ip同时为空的情况 */
            unsigned int _ipv4;
            /* 域名,可为空 */
#define Camera_ctrl_item_get_domain(___pCamera_ctrl_item,___nullValue) \
    ((___pCamera_ctrl_item)->ro._ipv4Port!=0&&!FsNumberIsMax((___pCamera_ctrl_item)->p._domain_)?((((char*)(___pCamera_ctrl_item))+Camera_ctrl_item_get_structLen_ipv4Port0)+(___pCamera_ctrl_item)->p._domain_):(___nullValue))
            unsigned short _domain_;
            /* 用户名,可为空 */
#define Camera_ctrl_item_get_username(___pCamera_ctrl_item,___nullValue) \
    ((___pCamera_ctrl_item)->ro._ipv4Port!=0&&!FsNumberIsMax((___pCamera_ctrl_item)->p._username_)?((((char*)(___pCamera_ctrl_item))+Camera_ctrl_item_get_structLen_ipv4Port0)+(___pCamera_ctrl_item)->p._username_):(___nullValue))
            unsigned short _username_;
            /* 密码,可为空 */
#define Camera_ctrl_item_get_password(___pCamera_ctrl_item,___nullValue) \
    ((___pCamera_ctrl_item)->ro._ipv4Port!=0&&!FsNumberIsMax((___pCamera_ctrl_item)->p._password_)?((((char*)(___pCamera_ctrl_item))+Camera_ctrl_item_get_structLen_ipv4Port0)+(___pCamera_ctrl_item)->p._password_):(___nullValue))
            unsigned short _password_;
            /* 通道id,可为空 */
#define Camera_ctrl_item_get_channelUUID(___pCamera_ctrl_item,___nullValue) \
    ((___pCamera_ctrl_item)->ro._ipv4Port!=0&&!FsNumberIsMax((___pCamera_ctrl_item)->p._channelUUID_)?((((char*)(___pCamera_ctrl_item))+Camera_ctrl_item_get_structLen_ipv4Port0)+(___pCamera_ctrl_item)->p._channelUUID_):(___nullValue))
            unsigned short _channelUUID_;
        } p;
    };
#define __Camera_ctrl_itemLog(___Log,___loglevel,___format, ...)\
    ___Log(___loglevel,5,___format",p=%p/%p,uuid:\"%s\",server:\"%s%s%s%s%s%s%hhu.%hhu.%hhu.%hhu%s:%hu/%s\"\
,busy=%hhu,controlStatus=%hhu,requestID_status=%hhx,requestID_3=%x/%x/%x,mask=%llx.\n",##__VA_ARGS__\
        ,pCamera_ctrl_item,pCamera_ctrl_item->p.pCamera_ctrl,Camera_ctrl_item_get_uuid(pCamera_ctrl_item,"")\
        ,pCamera_ctrl_item->ro._ipv4Port!=0?Camera_ctrl_item_get_username(pCamera_ctrl_item,""):""\
        ,(pCamera_ctrl_item->ro._ipv4Port!=0&&Camera_ctrl_item_get_username(pCamera_ctrl_item,NULL)!=NULL)?":":""\
        ,pCamera_ctrl_item->ro._ipv4Port!=0?Camera_ctrl_item_get_password(pCamera_ctrl_item,""):""\
        ,(pCamera_ctrl_item->ro._ipv4Port!=0&&Camera_ctrl_item_get_username(pCamera_ctrl_item,NULL)!=NULL)?"@":""\
        ,pCamera_ctrl_item->ro._ipv4Port!=0?Camera_ctrl_item_get_domain(pCamera_ctrl_item,""):""\
        ,(pCamera_ctrl_item->ro._ipv4Port!=0&&Camera_ctrl_item_get_domain(pCamera_ctrl_item,NULL)!=NULL)?"(":""\
        ,pCamera_ctrl_item->ro._ipv4Port!=0?((unsigned char*)&pCamera_ctrl_item->p._ipv4)[0]:0\
        ,pCamera_ctrl_item->ro._ipv4Port!=0?((unsigned char*)&pCamera_ctrl_item->p._ipv4)[1]:0\
        ,pCamera_ctrl_item->ro._ipv4Port!=0?((unsigned char*)&pCamera_ctrl_item->p._ipv4)[2]:0\
        ,pCamera_ctrl_item->ro._ipv4Port!=0?((unsigned char*)&pCamera_ctrl_item->p._ipv4)[3]:0\
        ,(pCamera_ctrl_item->ro._ipv4Port!=0&&Camera_ctrl_item_get_domain(pCamera_ctrl_item,NULL)!=NULL)?")":""\
        ,pCamera_ctrl_item->ro._ipv4Port,Camera_ctrl_item_get_channelUUID(pCamera_ctrl_item,"")\
        ,pCamera_ctrl_item->p.busy,pCamera_ctrl_item->p.controlStatus,pCamera_ctrl_item->p.requestID_status\
        ,pCamera_ctrl_item->p.requestID_3[0],pCamera_ctrl_item->p.requestID_3[1],pCamera_ctrl_item->p.requestID_3[2],pCamera_ctrl_item->p.mask)
#define Camera_ctrl_itemLog(___loglevel,___format, ...) __Camera_ctrl_itemLog(FsLog,___loglevel,___format,##__VA_ARGS__)
#define Camera_ctrl_itemLog2(___loglevel,___format, ...) __Camera_ctrl_itemLog(FsLog2,___loglevel,___format,##__VA_ARGS__)

    /* 
     * 创建一个新的Camera_ctrl对象;
     * 返回Camera_ctrl指针.
     */

    Camera_ctrl *camera_ctrl_new__IO(/* 监控的线程指针对象,开始线程后把创建的监控节点添加到此对象中 */ void *const pMonitor
            , /* 多线程共有的运行状态,如果此值为空,则引用默认的全局运行状态变量,本线程的运行状态只有与此值相同才能运行 */signed char *const externRunStatus
            , /* 父线程的线程号,由systemThreadTid获得,为0表示无父线程 */ const pthread_t parentTid, /* 线程名 */const char threadName[]
            , /* 配置管理对象,为空表示没有pConfigManager,不能通知外部相机控制 */ void *const pConfigManager
            , /* 连接池,为空内部使用独立的连接池 */ SocketPool * const pSocketPool);

    /* 删除pCamera_ctrl指向的实例对象 */

    void camera_ctrl_delete__OI(Camera_ctrl * const pCamera_ctrl);

    /* 开始线程,必须先设置好初始化时为可写全过程可读区的所有参数 */

    void camera_ctrl_startThread(Camera_ctrl * const pCamera_ctrl
            , /* 线程分时优先级,有效值为[-1-99],越大优先级越高,0表示不改变优先级,继承父线程的优先级,-1-表示强制线程的调度模式为分时调度策略,正数强制线程的调度模式为实时分时调度策略 */const signed char thisNice);

    /* 结束线程,只发送信号,不回收线程资源 */

    void camera_ctrl_stopThread_noWait(Camera_ctrl * const pCamera_ctrl);

    /* 结束线程 */

    void camera_ctrl_stopThread(Camera_ctrl * const pCamera_ctrl);

    /* 
     * 创建一个新的Camera_ctrl_item对象;
     * 返回Camera_ctrl_item指针.
     */

    struct Camera_ctrl_item *camera_ctrl_item_new__IO(/* 通道标识,可为空 */const char uuid[], /* 域名,可为空 */ const char domain[]
            , /* IPV4地址,为0表示无效,不应出现域名和ip同时无效的情况 */const unsigned int ipv4, /* 端口,为0表示不主动对外连接 */const unsigned short ipv4Port
            , /* 用户名,可为空 */const char username[], /* 密码,可为空 */const char password[], /* 通道id,为空时使用uuid的值 */const char channelUUID[]);

    /* 删除pCamera_ctrl_item指向的实例对象 */

    void camera_ctrl_item_delete__OI(struct Camera_ctrl_item * const pCamera_ctrl_item);

    /* 向pCamera_ctrl中添加一个控制项 */

    void camera_ctrl_add_item__OI_2(Camera_ctrl * const pCamera_ctrl, struct Camera_ctrl_item * const pCamera_ctrl_item);

    /* 从pCamera_ctrl中移除一个pCamera_ctrl_item,未等待线程真正移除 */

    void camera_ctrl_remove_item_noWait__IO_2(Camera_ctrl * const pCamera_ctrl, struct Camera_ctrl_item * const pCamera_ctrl_item);

    /* 从pCamera_ctrl中移除一个控制项 */

    void camera_ctrl_remove_item__IO_2(Camera_ctrl * const pCamera_ctrl, struct Camera_ctrl_item * const pCamera_ctrl_item);

    /* 释放占有的控制 */

    void camera_ctrl_item_ctrl_release_pthreadSafety(struct Camera_ctrl_item * const pCamera_ctrl_item
            , /* 缓存Buffer,不为空 */FsObjectBaseBuffer * const pObjectBaseBuffer, /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);

    /* 控制,获取掩码,按位排布,0-任意转,1-左右转,2-上下转,3-45度斜角转,4-变倍,5-聚焦,6-预置位,7-3d定位,8-gotoPTZ和getPTZ,9-图像参数设置与获取,10-直接向机芯发送命令 */

    unsigned long long camera_ctrl_item_ctrl_mask_get_pthreadSafety(struct Camera_ctrl_item * const pCamera_ctrl_item, /* 控制协议协议名,不为空时把控制协议协议名拷贝输出,无协议名输出\0 */ char protocol_name[], /* protocol_name空间大小 */const unsigned int protocol_nameLen
            , /* 设备型号,不为空时把设备名拷贝输出,无设备名输出\0 */ char device_type[], /* device_type空间大小 */const unsigned int device_typeLen
            , /* 设备序列号,不为空时把设备序列号拷贝输出,无设备序列号输出\0 */ char device_sn[], /* device_sn空间大小 */const unsigned int device_snLen
            , /* 设备版本号,不为空时把设备版本号拷贝输出,无设备版本号输出\0 */ char device_version[], /* device_version空间大小 */const unsigned int device_versionLen
            , /* 缓存Buffer,不为空 */FsObjectBaseBuffer * const pObjectBaseBuffer, /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);

    /* 控制,任意转,成功返回1,失败返回-1 */

    int camera_ctrl_item_ctrl_turn_pthreadSafety(struct Camera_ctrl_item * const pCamera_ctrl_item, /* 0-表示本指令不是控制,仅是通知信息,1-表示本指令为控制 */const unsigned char control
            , /* 优先级,0-默认,不缓存调用者信息,1-比分析优先级低的定义,2-255:保留,256-29999:比分析优先级低的定义,30000-34999:视频分析专用的优先级,35000-64999:比分析优先级高的定义,65000-65534:保留;非0优先级中,
                 *   偶数表示可合并的优先级,即如有多个拥有相同偶数优先级的客户端访问同一相机,这些所有客户端的指令会按照新指令覆盖旧指令方式生效;
                 *   奇数表示独占优先级,即如有多个拥有相同奇数优先级的客户端访问同一相机,只有第一个客户端的指令会生效,其他客户端的指令会被拒绝 */const unsigned short priority
            , /* 参考值精度为23040,负数向左,正数向右 */const short speedx, /* 参考值精度为23040,负数向上,正数向下 */const short speedy, /* 转动时间 */const float time
            , /* 缓存Buffer,不为空 */FsObjectBaseBuffer * const pObjectBaseBuffer, /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);

    /* 控制,左转,成功返回1,失败返回-1 */

    int camera_ctrl_item_ctrl_turnLeft_pthreadSafety(struct Camera_ctrl_item * const pCamera_ctrl_item, /* 0-表示本指令不是控制,仅是通知信息,1-表示本指令为控制 */const unsigned char control
            , /* 优先级,0-默认,不缓存调用者信息,1-比分析优先级低的定义,2-255:保留,256-29999:比分析优先级低的定义,30000-34999:视频分析专用的优先级,35000-64999:比分析优先级高的定义,65000-65534:保留;非0优先级中,
                 *   偶数表示可合并的优先级,即如有多个拥有相同偶数优先级的客户端访问同一相机,这些所有客户端的指令会按照新指令覆盖旧指令方式生效;
                 *   奇数表示独占优先级,即如有多个拥有相同奇数优先级的客户端访问同一相机,只有第一个客户端的指令会生效,其他客户端的指令会被拒绝 */const unsigned short priority
            , /* 参考值精度为23040 */const unsigned short speed, /* 转动时间 */const float time
            , /* 缓存Buffer,不为空 */FsObjectBaseBuffer * const pObjectBaseBuffer, /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);

    /* 控制,右转,成功返回1,失败返回-1 */

    int camera_ctrl_item_ctrl_turnRight_pthreadSafety(struct Camera_ctrl_item * const pCamera_ctrl_item, /* 0-表示本指令不是控制,仅是通知信息,1-表示本指令为控制 */const unsigned char control
            , /* 优先级,0-默认,不缓存调用者信息,1-比分析优先级低的定义,2-255:保留,256-29999:比分析优先级低的定义,30000-34999:视频分析专用的优先级,35000-64999:比分析优先级高的定义,65000-65534:保留;非0优先级中,
                 *   偶数表示可合并的优先级,即如有多个拥有相同偶数优先级的客户端访问同一相机,这些所有客户端的指令会按照新指令覆盖旧指令方式生效;
                 *   奇数表示独占优先级,即如有多个拥有相同奇数优先级的客户端访问同一相机,只有第一个客户端的指令会生效,其他客户端的指令会被拒绝 */const unsigned short priority
            , /* 参考值精度为23040 */const unsigned short speed, /* 转动时间 */const float time
            , /* 缓存Buffer,不为空 */FsObjectBaseBuffer * const pObjectBaseBuffer, /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);

    /* 控制,上转,成功返回1,失败返回-1 */

    int camera_ctrl_item_ctrl_turnUp_pthreadSafety(struct Camera_ctrl_item * const pCamera_ctrl_item, /* 0-表示本指令不是控制,仅是通知信息,1-表示本指令为控制 */const unsigned char control
            , /* 优先级,0-默认,不缓存调用者信息,1-比分析优先级低的定义,2-255:保留,256-29999:比分析优先级低的定义,30000-34999:视频分析专用的优先级,35000-64999:比分析优先级高的定义,65000-65534:保留;非0优先级中,
                 *   偶数表示可合并的优先级,即如有多个拥有相同偶数优先级的客户端访问同一相机,这些所有客户端的指令会按照新指令覆盖旧指令方式生效;
                 *   奇数表示独占优先级,即如有多个拥有相同奇数优先级的客户端访问同一相机,只有第一个客户端的指令会生效,其他客户端的指令会被拒绝 */const unsigned short priority
            , /* 参考值精度为23040 */const unsigned short speed, /* 转动时间 */const float time
            , /* 缓存Buffer,不为空 */FsObjectBaseBuffer * const pObjectBaseBuffer, /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);

    /* 控制,下转,成功返回1,失败返回-1 */

    int camera_ctrl_item_ctrl_turnDown_pthreadSafety(struct Camera_ctrl_item * const pCamera_ctrl_item, /* 0-表示本指令不是控制,仅是通知信息,1-表示本指令为控制 */const unsigned char control
            , /* 优先级,0-默认,不缓存调用者信息,1-比分析优先级低的定义,2-255:保留,256-29999:比分析优先级低的定义,30000-34999:视频分析专用的优先级,35000-64999:比分析优先级高的定义,65000-65534:保留;非0优先级中,
                 *   偶数表示可合并的优先级,即如有多个拥有相同偶数优先级的客户端访问同一相机,这些所有客户端的指令会按照新指令覆盖旧指令方式生效;
                 *   奇数表示独占优先级,即如有多个拥有相同奇数优先级的客户端访问同一相机,只有第一个客户端的指令会生效,其他客户端的指令会被拒绝 */const unsigned short priority
            , /* 参考值精度为23040 */const unsigned short speed, /* 转动时间 */const float time
            , /* 缓存Buffer,不为空 */FsObjectBaseBuffer * const pObjectBaseBuffer, /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);

    /* 控制,左上转,成功返回1,失败返回-1 */

    int camera_ctrl_item_ctrl_turnLeftUp_pthreadSafety(struct Camera_ctrl_item * const pCamera_ctrl_item, /* 0-表示本指令不是控制,仅是通知信息,1-表示本指令为控制 */const unsigned char control
            , /* 优先级,0-默认,不缓存调用者信息,1-比分析优先级低的定义,2-255:保留,256-29999:比分析优先级低的定义,30000-34999:视频分析专用的优先级,35000-64999:比分析优先级高的定义,65000-65534:保留;非0优先级中,
                 *   偶数表示可合并的优先级,即如有多个拥有相同偶数优先级的客户端访问同一相机,这些所有客户端的指令会按照新指令覆盖旧指令方式生效;
                 *   奇数表示独占优先级,即如有多个拥有相同奇数优先级的客户端访问同一相机,只有第一个客户端的指令会生效,其他客户端的指令会被拒绝 */const unsigned short priority
            , /* 参考值精度为23040 */const unsigned short speed, /* 转动时间 */const float time
            , /* 缓存Buffer,不为空 */FsObjectBaseBuffer * const pObjectBaseBuffer, /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);

    /* 控制,左下转,成功返回1,失败返回-1 */

    int camera_ctrl_item_ctrl_turnLeftDown_pthreadSafety(struct Camera_ctrl_item * const pCamera_ctrl_item, /* 0-表示本指令不是控制,仅是通知信息,1-表示本指令为控制 */const unsigned char control
            , /* 优先级,0-默认,不缓存调用者信息,1-比分析优先级低的定义,2-255:保留,256-29999:比分析优先级低的定义,30000-34999:视频分析专用的优先级,35000-64999:比分析优先级高的定义,65000-65534:保留;非0优先级中,
                 *   偶数表示可合并的优先级,即如有多个拥有相同偶数优先级的客户端访问同一相机,这些所有客户端的指令会按照新指令覆盖旧指令方式生效;
                 *   奇数表示独占优先级,即如有多个拥有相同奇数优先级的客户端访问同一相机,只有第一个客户端的指令会生效,其他客户端的指令会被拒绝 */const unsigned short priority
            , /* 参考值精度为23040 */const unsigned short speed, /* 转动时间 */const float time
            , /* 缓存Buffer,不为空 */FsObjectBaseBuffer * const pObjectBaseBuffer, /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);

    /* 控制,右上转,成功返回1,失败返回-1 */

    int camera_ctrl_item_ctrl_turnRightUp_pthreadSafety(struct Camera_ctrl_item * const pCamera_ctrl_item, /* 0-表示本指令不是控制,仅是通知信息,1-表示本指令为控制 */const unsigned char control
            , /* 优先级,0-默认,不缓存调用者信息,1-比分析优先级低的定义,2-255:保留,256-29999:比分析优先级低的定义,30000-34999:视频分析专用的优先级,35000-64999:比分析优先级高的定义,65000-65534:保留;非0优先级中,
                 *   偶数表示可合并的优先级,即如有多个拥有相同偶数优先级的客户端访问同一相机,这些所有客户端的指令会按照新指令覆盖旧指令方式生效;
                 *   奇数表示独占优先级,即如有多个拥有相同奇数优先级的客户端访问同一相机,只有第一个客户端的指令会生效,其他客户端的指令会被拒绝 */const unsigned short priority
            , /* 参考值精度为23040 */const unsigned short speed, /* 转动时间 */const float time
            , /* 缓存Buffer,不为空 */FsObjectBaseBuffer * const pObjectBaseBuffer, /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);

    /* 控制,右下转,成功返回1,失败返回-1 */

    int camera_ctrl_item_ctrl_turnRightDown_pthreadSafety(struct Camera_ctrl_item * const pCamera_ctrl_item, /* 0-表示本指令不是控制,仅是通知信息,1-表示本指令为控制 */const unsigned char control
            , /* 优先级,0-默认,不缓存调用者信息,1-比分析优先级低的定义,2-255:保留,256-29999:比分析优先级低的定义,30000-34999:视频分析专用的优先级,35000-64999:比分析优先级高的定义,65000-65534:保留;非0优先级中,
                 *   偶数表示可合并的优先级,即如有多个拥有相同偶数优先级的客户端访问同一相机,这些所有客户端的指令会按照新指令覆盖旧指令方式生效;
                 *   奇数表示独占优先级,即如有多个拥有相同奇数优先级的客户端访问同一相机,只有第一个客户端的指令会生效,其他客户端的指令会被拒绝 */const unsigned short priority
            , /* 参考值精度为23040 */const unsigned short speed, /* 转动时间 */const float time
            , /* 缓存Buffer,不为空 */FsObjectBaseBuffer * const pObjectBaseBuffer, /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);

    /* 控制,变倍,成功返回1,失败返回-1 */

    int camera_ctrl_item_ctrl_zoom_pthreadSafety(struct Camera_ctrl_item * const pCamera_ctrl_item, /* 0-表示本指令不是控制,仅是通知信息,1-表示本指令为控制 */const unsigned char control
            , /* 优先级,0-默认,不缓存调用者信息,1-比分析优先级低的定义,2-255:保留,256-29999:比分析优先级低的定义,30000-34999:视频分析专用的优先级,35000-64999:比分析优先级高的定义,65000-65534:保留;非0优先级中,
                 *   偶数表示可合并的优先级,即如有多个拥有相同偶数优先级的客户端访问同一相机,这些所有客户端的指令会按照新指令覆盖旧指令方式生效;
                 *   奇数表示独占优先级,即如有多个拥有相同奇数优先级的客户端访问同一相机,只有第一个客户端的指令会生效,其他客户端的指令会被拒绝 */const unsigned short priority
            , /* 参考值精度为23040,负数缩小,正数放大 */const short speed, /* 变倍时间 */const float time
            , /* 缓存Buffer,不为空 */FsObjectBaseBuffer * const pObjectBaseBuffer, /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);

    /* 控制,聚焦,成功返回1,失败返回-1 */

    int camera_ctrl_item_ctrl_focus_pthreadSafety(struct Camera_ctrl_item * const pCamera_ctrl_item, /* 0-表示本指令不是控制,仅是通知信息,1-表示本指令为控制 */const unsigned char control
            , /* 优先级,0-默认,不缓存调用者信息,1-比分析优先级低的定义,2-255:保留,256-29999:比分析优先级低的定义,30000-34999:视频分析专用的优先级,35000-64999:比分析优先级高的定义,65000-65534:保留;非0优先级中,
                 *   偶数表示可合并的优先级,即如有多个拥有相同偶数优先级的客户端访问同一相机,这些所有客户端的指令会按照新指令覆盖旧指令方式生效;
                 *   奇数表示独占优先级,即如有多个拥有相同奇数优先级的客户端访问同一相机,只有第一个客户端的指令会生效,其他客户端的指令会被拒绝 */const unsigned short priority
            , /* 参考值精度为23040,负数-,正数+ */const short speed, /* 聚焦时间 */const float time
            , /* 缓存Buffer,不为空 */FsObjectBaseBuffer * const pObjectBaseBuffer, /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);

    /* 控制,设置预置位,成功返回1,失败返回-1 */

    int camera_ctrl_item_ctrl_preset_set_pthreadSafety(struct Camera_ctrl_item * const pCamera_ctrl_item, /* 0-表示本指令不是控制,仅是通知信息,1-表示本指令为控制 */const unsigned char control
            , /* 优先级,0-默认,不缓存调用者信息,1-比分析优先级低的定义,2-255:保留,256-29999:比分析优先级低的定义,30000-34999:视频分析专用的优先级,35000-64999:比分析优先级高的定义,65000-65534:保留;非0优先级中,
                 *   偶数表示可合并的优先级,即如有多个拥有相同偶数优先级的客户端访问同一相机,这些所有客户端的指令会按照新指令覆盖旧指令方式生效;
                 *   奇数表示独占优先级,即如有多个拥有相同奇数优先级的客户端访问同一相机,只有第一个客户端的指令会生效,其他客户端的指令会被拒绝 */const unsigned short priority
            , /* 预置位号 */const unsigned short presetIndex
            , /* 缓存Buffer,不为空 */FsObjectBaseBuffer * const pObjectBaseBuffer, /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);

    /* 控制,调用预置位,成功返回1,失败返回-1 */

    int camera_ctrl_item_ctrl_preset_goto_pthreadSafety(struct Camera_ctrl_item * const pCamera_ctrl_item, /* 0-表示本指令不是控制,仅是通知信息,1-表示本指令为控制 */const unsigned char control
            , /* 优先级,0-默认,不缓存调用者信息,1-比分析优先级低的定义,2-255:保留,256-29999:比分析优先级低的定义,30000-34999:视频分析专用的优先级,35000-64999:比分析优先级高的定义,65000-65534:保留;非0优先级中,
                 *   偶数表示可合并的优先级,即如有多个拥有相同偶数优先级的客户端访问同一相机,这些所有客户端的指令会按照新指令覆盖旧指令方式生效;
                 *   奇数表示独占优先级,即如有多个拥有相同奇数优先级的客户端访问同一相机,只有第一个客户端的指令会生效,其他客户端的指令会被拒绝 */const unsigned short priority
            , /* 预置位号 */const unsigned short presetIndex
            , /* 缓存Buffer,不为空 */FsObjectBaseBuffer * const pObjectBaseBuffer, /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);

    /* 控制,清除预置位,成功返回1,失败返回-1 */

    int camera_ctrl_item_ctrl_preset_clean_pthreadSafety(struct Camera_ctrl_item * const pCamera_ctrl_item, /* 0-表示本指令不是控制,仅是通知信息,1-表示本指令为控制 */const unsigned char control
            , /* 优先级,0-默认,不缓存调用者信息,1-比分析优先级低的定义,2-255:保留,256-29999:比分析优先级低的定义,30000-34999:视频分析专用的优先级,35000-64999:比分析优先级高的定义,65000-65534:保留;非0优先级中,
                 *   偶数表示可合并的优先级,即如有多个拥有相同偶数优先级的客户端访问同一相机,这些所有客户端的指令会按照新指令覆盖旧指令方式生效;
                 *   奇数表示独占优先级,即如有多个拥有相同奇数优先级的客户端访问同一相机,只有第一个客户端的指令会生效,其他客户端的指令会被拒绝 */const unsigned short priority
            , /* 预置位号 */const unsigned short presetIndex
            , /* 缓存Buffer,不为空 */FsObjectBaseBuffer * const pObjectBaseBuffer, /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);

    /* 控制,3D定位,成功返回1,失败返回-1 */

    int camera_ctrl_item_ctrl_zoom3D_pthreadSafety(struct Camera_ctrl_item * const pCamera_ctrl_item, /* 0-表示本指令不是控制,仅是通知信息,1-表示本指令为控制 */const unsigned char control
            , /* 优先级,0-默认,不缓存调用者信息,1-比分析优先级低的定义,2-255:保留,256-29999:比分析优先级低的定义,30000-34999:视频分析专用的优先级,35000-64999:比分析优先级高的定义,65000-65534:保留;非0优先级中,
                 *   偶数表示可合并的优先级,即如有多个拥有相同偶数优先级的客户端访问同一相机,这些所有客户端的指令会按照新指令覆盖旧指令方式生效;
                 *   奇数表示独占优先级,即如有多个拥有相同奇数优先级的客户端访问同一相机,只有第一个客户端的指令会生效,其他客户端的指令会被拒绝 */const unsigned short priority
            , /* 相对图大小参考值23040 */const unsigned short x1, /* 相对图大小参考值23040 */const unsigned short y1, /* 相对图大小参考值23040 */const unsigned short x2, /* 相对图大小参考值23040 */const unsigned short y2
            , /* 缓存Buffer,不为空 */FsObjectBaseBuffer * const pObjectBaseBuffer, /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);

    /* 控制,goto PTZ,成功返回1,失败返回-1 */

    int camera_ctrl_item_ctrl_ptz_goto_pthreadSafety(struct Camera_ctrl_item * const pCamera_ctrl_item, /* 0-表示本指令不是控制,仅是通知信息,1-表示本指令为控制 */const unsigned char control
            , /* 优先级,0-默认,不缓存调用者信息,1-比分析优先级低的定义,2-255:保留,256-29999:比分析优先级低的定义,30000-34999:视频分析专用的优先级,35000-64999:比分析优先级高的定义,65000-65534:保留;非0优先级中,
                 *   偶数表示可合并的优先级,即如有多个拥有相同偶数优先级的客户端访问同一相机,这些所有客户端的指令会按照新指令覆盖旧指令方式生效;
                 *   奇数表示独占优先级,即如有多个拥有相同奇数优先级的客户端访问同一相机,只有第一个客户端的指令会生效,其他客户端的指令会被拒绝 */const unsigned short priority
            , /* 相对参考值23040 */const unsigned short P, /* 相对参考值23040 */const unsigned short T, /* 相对参考值23040 */const unsigned short Z
            , /* 缓存Buffer,不为空 */FsObjectBaseBuffer * const pObjectBaseBuffer, /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);

    /* 控制,获取PTZ,成功返回1,失败返回-1 */

    int camera_ctrl_item_ctrl_ptz_get_pthreadSafety(struct Camera_ctrl_item * const pCamera_ctrl_item, /* 0-表示本指令不是控制,仅是通知信息,1-表示本指令为控制 */const unsigned char control
            , /* 优先级,0-默认,不缓存调用者信息,1-比分析优先级低的定义,2-255:保留,256-29999:比分析优先级低的定义,30000-34999:视频分析专用的优先级,35000-64999:比分析优先级高的定义,65000-65534:保留;非0优先级中,
                 *   偶数表示可合并的优先级,即如有多个拥有相同偶数优先级的客户端访问同一相机,这些所有客户端的指令会按照新指令覆盖旧指令方式生效;
                 *   奇数表示独占优先级,即如有多个拥有相同奇数优先级的客户端访问同一相机,只有第一个客户端的指令会生效,其他客户端的指令会被拒绝 */const unsigned short priority
            , /* 相对参考值23040,为空不返回 */ unsigned short*const P, /* 相对参考值23040,为空不返回 */ unsigned short* const T, /* 相对参考值23040,为空不返回 */ unsigned short* const Z
            , /* 缓存Buffer,不为空 */FsObjectBaseBuffer * const pObjectBaseBuffer, /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);

    /* 控制,设置图像参数,成功返回1,失败返回-1 */

    int camera_ctrl_item_ctrl_imageInfo_set_pthreadSafety(struct Camera_ctrl_item * const pCamera_ctrl_item, /* 0-表示本指令不是控制,仅是通知信息,1-表示本指令为控制 */const unsigned char control
            , /* 优先级,0-默认,不缓存调用者信息,1-比分析优先级低的定义,2-255:保留,256-29999:比分析优先级低的定义,30000-34999:视频分析专用的优先级,35000-64999:比分析优先级高的定义,65000-65534:保留;非0优先级中,
                 *   偶数表示可合并的优先级,即如有多个拥有相同偶数优先级的客户端访问同一相机,这些所有客户端的指令会按照新指令覆盖旧指令方式生效;
                 *   奇数表示独占优先级,即如有多个拥有相同奇数优先级的客户端访问同一相机,只有第一个客户端的指令会生效,其他客户端的指令会被拒绝 */const unsigned short priority
            , /* 哪些参数是有效的,按位摆布,如controlMode为0 */unsigned int setMask, /* 图像信息 */ struct Camera_ctrl_imageInfo * const imageInfo
            , /* 缓存Buffer,不为空 */FsObjectBaseBuffer * const pObjectBaseBuffer, /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);

    /* 控制,获取图像参数,成功返回1,失败返回-1 */

    int camera_ctrl_item_ctrl_imageInfo_get_pthreadSafety(struct Camera_ctrl_item * const pCamera_ctrl_item, /* 0-表示本指令不是控制,仅是通知信息,1-表示本指令为控制 */const unsigned char control
            , /* 优先级,0-默认,不缓存调用者信息,1-比分析优先级低的定义,2-255:保留,256-29999:比分析优先级低的定义,30000-34999:视频分析专用的优先级,35000-64999:比分析优先级高的定义,65000-65534:保留;非0优先级中,
                 *   偶数表示可合并的优先级,即如有多个拥有相同偶数优先级的客户端访问同一相机,这些所有客户端的指令会按照新指令覆盖旧指令方式生效;
                 *   奇数表示独占优先级,即如有多个拥有相同奇数优先级的客户端访问同一相机,只有第一个客户端的指令会生效,其他客户端的指令会被拒绝 */const unsigned short priority
            , /* 图像信息,为空不返回 */ struct Camera_ctrl_imageInfo * const imageInfo
            , /* 缓存Buffer,不为空 */FsObjectBaseBuffer * const pObjectBaseBuffer, /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);

    /* 控制,向机芯直接发送命令,成功返回1,失败返回-1 */

    int camera_ctrl_item_ctrl_chipCmd_pthreadSafety(struct Camera_ctrl_item * const pCamera_ctrl_item, /* 0-表示本指令不是控制,仅是通知信息,1-表示本指令为控制 */const unsigned char control
            , /* 优先级,0-默认,不缓存调用者信息,1-比分析优先级低的定义,2-255:保留,256-29999:比分析优先级低的定义,30000-34999:视频分析专用的优先级,35000-64999:比分析优先级高的定义,65000-65534:保留;非0优先级中,
                 *   偶数表示可合并的优先级,即如有多个拥有相同偶数优先级的客户端访问同一相机,这些所有客户端的指令会按照新指令覆盖旧指令方式生效;
                 *   奇数表示独占优先级,即如有多个拥有相同奇数优先级的客户端访问同一相机,只有第一个客户端的指令会生效,其他客户端的指令会被拒绝 */const unsigned short priority
            , /* 命令的数据类型,0-字符串,1-二进制(cmd以字符串输入时使用base64编码) */ const unsigned char dataType, /* 命令 */const char cmd[], /* 命令长度 */const unsigned int cmdLen
            , /* 缓存Buffer,不为空 */FsObjectBaseBuffer * const pObjectBaseBuffer, /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);

    /* 控制,快门重置,成功返回1,失败返回-1 */

    int camera_ctrl_item_ctrl_shuterReset_pthreadSafety(struct Camera_ctrl_item * const pCamera_ctrl_item, /* 0-表示本指令不是控制,仅是通知信息,1-表示本指令为控制 */const unsigned char control
            , /* 优先级,0-默认,不缓存调用者信息,1-比分析优先级低的定义,2-255:保留,256-29999:比分析优先级低的定义,30000-34999:视频分析专用的优先级,35000-64999:比分析优先级高的定义,65000-65534:保留;非0优先级中,
                 *   偶数表示可合并的优先级,即如有多个拥有相同偶数优先级的客户端访问同一相机,这些所有客户端的指令会按照新指令覆盖旧指令方式生效;
                 *   奇数表示独占优先级,即如有多个拥有相同奇数优先级的客户端访问同一相机,只有第一个客户端的指令会生效,其他客户端的指令会被拒绝 */const unsigned short priority
            , /* 缓存Buffer,不为空 */FsObjectBaseBuffer * const pObjectBaseBuffer, /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);



#ifdef __cplusplus
}
#endif

#endif /* CAMERA_CTRL_H */

