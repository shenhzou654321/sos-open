/* 
 * File:   detectAlgorithmLib.h
 * Author: fslib
 *
 * Created on 2013年2月22日, 下午5:03
 */
#if !defined DETECTALGORITHMLIB_H && !defined JAVA_IN_C

#define DETECTALGORITHMLIB_H
#include "../../PublicTools/Fs/Config.h"
#include "../../PublicTools/Analyze/ObjectTrack.h"


#ifdef __cplusplus
extern "C" {
#endif

#pragma pack(push,1)

#pragma pack(pop)

    struct DetectAlgorithmLib_ConventionalDetect_pos_data {
        struct ObjectTrack_pos_data pos_data;
        /* 目标id */
        unsigned int id;
        /* 类别的概率,(需要开启识别才有效,没有开启时为0.85) */
        float prob;
        /* 类型(需要开启识别才有效),-1未知,1-人 */
        signed char type;
        /* 是否检测到,算法在一定范围内未检测到也会输出,未检测到此值为0,检测到此值为1 */
        signed char is_detected;
        /* 是否要画轨迹,算法初期检测到目标可能还未确定目标,未确定时此值为0,确定此值为1 */
        signed char isDrawing_tracks;
    };

    /* 
     * 创建一个ConventionalDetect检测对象,成功返回ConventionalDetect,失败返回NULL;
     */

   void *detectAlgorithmLib_ConventionalDetect_new__IO(/* 图片的宽度 */const unsigned int width, /* 图片的高度 */const unsigned int height
        , /* 算法类型,0-天空中的小目标,1-检测道路上的车和人(常用算法),2-高斯：检测道路上的车和人,3-暂时不用,4-火源检测 */const unsigned char eDetectAlgorithm, /* 目标类型检测,0-关闭,1-开启 */const char classCheck
        , /* 漂移半径 */const int iShiftRadius, /* 最小目标像素X */const int min_size_x, /* 最小目标像素Y */const int min_size_y, /* 最大目标像素X */const int max_size_x, /* 最大目标像素Y */const int max_size_y, /* 长宽比 */const int iBarRatio
        , /* 目标跟踪,0-关闭,1-开启 */const char trackEnable, /* 最大转弯角度 */const int iMaxAngle, /* 匹配阈值计算控制 */ const double dLimitGauss, /*n次连续轨迹显示*/const int learnCount,/*n次轨迹预测显示*/const int skipCount,/*轨迹剔除*/const int bIsTrackOut
        , /* IRAY_DETECT_SCENE_UseForSky 算法特殊参数 礼帽算法阈值 */const int iUseForSkyTHthresh, /* IRAY_DETECT_SCENE_UseForSky 算法特殊参数 机芯伪彩指示变量 0：白热，1：黑热； */ const int iUseForSkyColorType
        , /* IRAY_DETECT_SCENE_Vibe 算法特殊参数 当前帧与背景模型作差的阈值（可变） */ const int iVibeRadius
        , /* IRAY_DETECT_SCENE_Gmm 算法特殊参数 偏差阈值 */ const float fGmmD, /* IRAY_DETECT_SCENE_Gmm 算法特殊参数 初始化标准差 */ const float fGmmSdInit
        , /* IRAY_DETECT_SCENE_GrimsonGmm 算法特殊参数 ,阈值偏差 */ const int iGrimsonGmmMatchThreshold, /* IRAY_DETECT_SCENE_GrimsonGmm 算法特殊参数 ,中值滤波的尺寸 */ const int iGrimsonGmmMedianBlurKernelSize
        , /* IRAY_DETECT_SCENE_Fire 算法特殊参数 ,当前帧与背景模型作差的阈值（可变） */ const int iFireRadius
        , /* 区域 */FsConfigResult_area * const pArea, /* 剔除区域,可为空 */FsConfigResult_area * const pAreaExclude
        , /* 区域中的点坐标参考的0点坐标,point_x+area_x0作为最终坐标 */const int area_x0, /* 区域中的点坐标参考的0点坐标,point_y+area_y0作为最终坐标 */const int area_y0);

    /* 
     * 删除pConventionalDetect;
     */

    extern void (*detectAlgorithmLib_ConventionalDetect_delete__OI)(void *const pConventionalDetect);

    /* 
     * 进行检测,返回检测到的目标数量;
     */

    extern int (*detectAlgorithmLib_ConventionalDetect_detect)(void *const pConventionalDetect, /* 是多可输出的目标数量 */unsigned int targetCount, struct DetectAlgorithmLib_ConventionalDetect_pos_data pos_data[]
            , /* 数据 */const unsigned char data[], /* 储存宽度 */const unsigned int lineBits);

    /* 从ObjectTrack_pos_data对象中获取id的方法 */

    int detectAlgorithmLib_ConventionalDetect_pos_data_get_id(struct DetectAlgorithmLib_ConventionalDetect_pos_data * pDetectAlgorithmLib_ConventionalDetect_pos_data);



#ifdef __cplusplus
}
#endif

#endif /* DETECTALGORITHMLIB_H */

