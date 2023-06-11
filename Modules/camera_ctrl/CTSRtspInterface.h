/*
流媒体播放和控制接口头文件-多通道版
Date:2015-03-31
Author：周勇
Version:1.0.0.0-----本版本支持连接多个相机并得到每个相机缩放后的RGB565图像
remarks
        【1】显示RTSP媒体图像
                        CTSAddCamera 增加一个流媒体通道     然后调用CTSStartViewCamera开启获取对应流媒体通道的视频    然后再回调函数中去显示视频
        【2】根据窗口大小调整图像大小
                        通过CTSSetParam 设置窗口的大小 输出的视频会调整到窗口大小
Date:2015-04-14
Author:周勇
Version:1.0.1.0-----本版本支持对相机云台的控制
remarks:通过CTSSetParam控制相机的动作

Date:2015-06-17
Author:周勇
Version:1.0.1.1-----本版本支持对D-Link DVS-n3020 的控制
                    当相机类型为D-link的时候，需要传入预览视频窗口句柄，并且CTS_CBKFUN回调函数不需要自己现实视频

Date:2015-06-24
Author:周勇
Version:1.0.1.2
remarks:通过CTSSetParam控制相机的动作
        增加相机信息的时候 配置相机参数可以分为:

        海康相机：    预览:camip,uid,pwd,camport,previewchan


        中光学相机、红光顶载相机：    预览:camip,uid,pwd,camport,previewchan
                                    控制:controlip,controlport
        D-Link相机：预览:camip,uid,pwd,camport,previewchan,nHwnd(实时)

        三星、索尼：暂不支持

        其它相机：    预览:rtspurl


Date:2015-07-01
Author:周勇
Version:1.0.1.3
remarks:修改D-Link DVS-N3020设备预览功能--与海康相机等设备保持一致--通过回调绘制图像


Date:2015-07-14
Author:周勇
Version:1.0.1.4
remarks:修改接口调用方式--外部调用不再使用相机id来作为参数进行操作,而修改为使用增加相机的返回句柄作为后续调用参数

Date:2015-07-21
Author:周勇
Version:1.0.1.5
remarks:修改接口--CTSSetParam和CTSGetParam 不在需要errorstring传入


Date:2015-08-05
Author:周勇
Version:1.0.1.6
remarks:设置ptz的时候可以选择是否自动停止ptz

Date:2015-08-31
Author:周勇
Version:1.0.1.7
remarks:修改FFmpeg连接rtsp 的Dictionary，防止连接不上rtsp(有些不支持tcp)

Date:2015-11-10
Author:张睿
Version:1.0.1.8
remarks:修改视频预览-视频显示的时间比视频实际时间晚一秒再显示

Date:2015-12-23
Author:周勇
Version:1.0.1.9
remarks:增加对流媒体服务器的视频快进、快退等控制选项的支持
*/



#ifndef __PLAY_RTSP_INTERFACE_H__
#define __PLAY_RTSP_INTERFACE_H__

#include "ZGXAPI.h"
typedef unsigned int uint;

//////////////////////////////////////////////////////宏//////////////////////////////////////////////////////////
#define CUR_DLL_VERSION            "1.0.1.7"             //当前动态库版本信息
#define MAX_CMINFO_LEN             250                   //相机信息字符串最大长度
#define MAX_CAMERA_PORT            65535                 //模拟相机端口号
#define MAX_IMG_H                  2048                  //视频最大高度
#define MAX_IMG_W                  2448                  //视频最大宽度
#define MAX_FRAME_RATE             25                    //视频最大帧率
#define MAX_PTZ_SPEED              7                     //PTZ转动最大速度
#define MIN_PTZ_SPEED              1                     //PTZ转动最小速度

//typedef unsigned int uint;
typedef uint  CTSHANDLE;


//////////////////////////////////////////////////////枚举//////////////////////////////////////////////////////////

//图像类型
enum IMAGE_TYPE
{
    IMAGE_YUV420,                                        //图像4:1:1YYYYYYYYYYYYYYYYUUUUVVVV
    IMAGE_YUV422,                                        //图像4:2:2YYYYYYYYYYYYYYYYUUUUUUUUVVVVVVVV
    IMAGE_RGB16,                                         //图像RGB565
    IMAGE_RGB24,                                         //图像RGB888
    IMAGE_RGB32                                          //图像RGB888A

};

//异常回调函数中的异常类型
enum CTSEXCAP_TYPE
{
    CTSEXCAP_NOT_READY,
    CTSEXCAP_CONNECT_OK,                                 //连接成功
    CTSEXCAP_CONNECTTIMEOUT,                             //连接超时
    CTSEXCAP_LOSTCONNECT                                 //失去连接
};

typedef CTSEXCAP_TYPE *PCTSEXCAP_TYPE;
//////////////////////////////////////////////////////回调显示函数//////////////////////////////////////////////////////////
typedef    void(*CTS_CBKFUN)(void *pImage, uint nBufSize, uint nImgW, uint nImgH, void *pUser);

typedef    void(*CTS_EXCAPFUN)( CTSEXCAP_TYPE t, void *pUser);


//////////////////////////////////////////////////////结构体//////////////////////////////////////////////////////////
//相机信息
struct CAMERA_INFO
{
    camera_t        t;                                   //相机类型
    char            szRtspUrl[MAX_CMINFO_LEN];           //rtsp地址
    char            szCameraIP[MAX_CMINFO_LEN];          //IP地址
    char            szCameraUid[MAX_CMINFO_LEN];         //用户名
    char            szCameraPwd[MAX_CMINFO_LEN];         //密码
    char            szControlIP[MAX_CMINFO_LEN];         //密码
    char            szCamName[MAX_CMINFO_LEN];           //相机名称
    uint            nFramerate;                          //模拟、数字相机的采样帧率0~MAX_FRAME_RATE
    uint            nImageW;                             //缩放后的图像宽度--通常指窗体大小0~MAX_IMG_W
    uint            nImageH;                             //缩放后的图像高度--通常指窗体大小0~MAX_IMG_H
    uint            nCameraPort;                         //SOCKET端口号-中光学相机使用
    uint            nControlPort;                        //SOCKET端口号-中光学相机使用
    uint            nHwnd;                               //窗口句柄--V1.0.1.4及以后的版本都不使用这个参数
    uint            nPreViewChan;                        //预览通道ID--流媒体服务器可能有多个通道
    uint            nChannelID;                          //相机通道ID0~MAX_CHANNEL_NUM--V1.0.1.4及以后的版本都不使用这个参数
    uint            nDBID;                               //数据库ID
    uint            nRes[19];                            //预留字节
};

//方向参数信息
struct PTZ_PARAM_INFO
{
    uint            bStop;                               //是否停止相机转动1-停止 0继续工作不停止
    uint            nDirectSpeed;                        //方向转动速度--范围MIN_PTZ_SPEED~MAX_PTZ_SPEED
    uint            bNeedAutoStop;                       //是否自动停止
    uint            nRes[19];                            //预留字节
};

//////////////////////////////////////////////////////接口//////////////////////////////////////////////////////////
#endif//__PLAY_RTSP_INTERFACE_H__

