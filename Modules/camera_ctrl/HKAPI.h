#ifndef _CAM_CTRL_H_
#define	_CAM_CTRL_H_

#include <stdio.h>
#include <sys/socket.h>
#include <time.h>
#include <sys/types.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>


typedef unsigned char BYTE;
typedef unsigned int  UINT;
typedef unsigned short WORD;
typedef unsigned long  ULONG_PTR;
typedef ULONG_PTR  DWORD_PTR;
typedef unsigned long DWORD;


#define     MIN_ZGX_PTZ_SPEED                            50//转速     50~250
#define     MAX_ZGX_PTZ_SPEED                            250//转速     50~250
#define     MIN_ZGX_YUNTAI_SPEED                     1
#define     MAX_ZGX_YUNTAI_SPEED                     30
#define MAX_PTZ_SPEED              7                     //PTZ转动最大速度
#define MIN_PTZ_SPEED              1                     //PTZ转动最小速度
#define MAX_CMINFO_LEN             250                   //相机信息字符串最大长度
#define  MAX_CONTROL_CMD_LEN                            250//通信控制命令长度
#define MAX_CONNECT_TIMEOUT                    100//socket连接超时时间
#define MAX_WAIT_TIME_ZOOMINOUT           5000//拉近或拉远的最大时间



#define MY_PI											(3.14159265358979323846)
#define EARTH_RADIUS									(6372795.0)




#define LOWORD(l)           ((WORD)((DWORD_PTR)(l) & 0xffff))
#define HIWORD(l)           ((WORD)((DWORD_PTR)(l) >> 16) & 0xffff)
#define LOBYTE(l)           ((BYTE)(WORD)(l))
#define HIBYTE(l)           ((BYTE)((WORD)(l) >> 8))
#define MAKEWORD(a, b) ((WORD)(((BYTE)(((DWORD_PTR)(a)) & 0xff)) | ((WORD)((BYTE)(((DWORD_PTR)(b)) & 0xff))) << 8))

typedef enum CAMERA_TYPE
{
    CAM_FACTOR_HIKVISION,                                //海康威视相机
    CAM_FACTOR_ZGXLIGHT,                                 //中光学白光相机
    CAM_FACTOR_ZGXRED,                                   //中光学红光相机
    CAM_FACTOR_HGKJDZ,                                   //红光科技顶载像机
    CAM_FACOTR_SUMSUN,                                   //三星像机
    CAM_FACTOR_SONY,                                     //索尼像机 
    CAM_FACTOR_DVSN3020,                                 //D-LINK DVS-N3020编码器
    CAM_FACTOR_RTSP,                                     //其它类型相机-仅rtsp预览        
    CAM_FACTOR_ZCHXFMSVIDEO                              //置晨海信流媒体服务器文件点播
}camera_t;


//镜头控制行为
typedef enum 
{
    CAMLEN_ACTION_NEAR,                                //相机控制拉近
    CAMLEN_ACTION_FAR                                //相机控制拉远
} camera_action_t;
//相机PTZ控制
typedef enum 
{
    CAMPTZ_STOP,
    CAMPTZ_LEFT,
    CAMPTZ_RIGHT,
    CAMPTZ_UP,
    CAMPTZ_DOWN,
    CAMPTZ_LEFTUP,
    CAMPTZ_RIGHTUP,
    CAMPTZ_LEFTDOWN,
    CAMPTZ_RIGHTDOWN
} camptz_action_t;

typedef enum 
{
        /*枚举类型                   data对应的数据类型                解释 */
    CAM_PARAM_NULL,                    /*NULL*/                        //无效 
    CAM_PARAM_DISPLAY_RECT,            /*CTS_RECT*/                    //设置缩放到显示区域的size 
    CAM_PARAM_FOCUS_RECT,              /*CTS_RECT*/                    //需要聚焦的目标框 目标将会呈现在区域最中央 
    CAM_PARAM_PTZ,                     /*CTS_PTZ */                    //PTZ坐标信息
    CAM_PARAM_STOP,                    /*NULL*/                        //停止转动
    CAM_PARAM_DIRECT_UP,               /*PTZ_PARAM_INFO*/              //方向-上
    CAM_PARAM_DIRECT_DOWN,             /*PTZ_PARAM_INFO*/              //方向-下
    CAM_PARAM_DIRECT_LEFT,             /*PTZ_PARAM_INFO*/              //方向-左
    CAM_PARAM_DIRECT_RIGHT,            /*PTZ_PARAM_INFO*/              //方向-右
    CAM_PARAM_DIRECT_LEFTUP,           /*PTZ_PARAM_INFO*/              //方向-左上
    CAM_PARAM_DIRECT_LEFTDOWN,         /*PTZ_PARAM_INFO*/              //方向-左下
    CAM_PARAM_DIRECT_RIGHTUP,          /*PTZ_PARAM_INFO*/              //方向-右上
    CAM_PARAM_DIRECT_RIGHTDOWN,        /*PTZ_PARAM_INFO*/              //方向-右下
    CAM_PARAM_ZOOM_IN,                 /*PTZ_PARAM_INFO*/              //拉近
    CAM_PARAM_ZOOM_MIDDLE,             /*PTZ_PARAM_INFO*/              //拉到中间位置
    CAM_PARAM_ZOOM_OUT,                /*PTZ_PARAM_INFO*/              //拉远
    CAM_PARAM_FOCUS_NEAR,              /*PTZ_PARAM_INFO*/              //调焦距清晰度聚焦+
    CAM_PARAM_FOCUS_FAR,               /*PTZ_PARAM_INFO*/              //调焦距清晰度聚焦-
    CAM_PARAM_IRIS_OPEN,               /*PTZ_PARAM_INFO*/              //光圈以速度SS扩大
    CAM_PARAM_IRIS_CLOSE,              /*PTZ_PARAM_INFO*/              //光圈以速度SS缩小
    CAM_PARAM_EXPOSURE,                /*UINT*/                        //曝光值
    CAM_PARAM_CONTRAST,                /*UINT*/                        //对比度
    CAM_PARAM_GAIN,                    /*UINT*/                        //增益
    CAM_PARAM_PRESET,                  /*CTS_PTZ*/                     //预置点 当设置的时候会返回ptz作为结果传出来如果平台本身支持预置位则返回的ptz无效
    CAM_PARAM_POWERON ,                /*UINT*/                        //上电-只有中光学红光相机才有这个功能
    CAM_PARAM_CBKDATATYPE,             /*IMAGE_TYPE*/                  //回调数据类型0=默认为RGB565 1 YUV420
    CAM_PARAM_FOCUS_AUTO,              /*PTZ_PARAM_INFO*/              //自动调焦
	CAM_PARAM_ZGXVIEWCHECK,            /*UINT*/                        //中光学视场校正---只有中光学红光像机有用
	CAM_PARAM_ZGXCLBL,				   /*UINT*/						   //中光学可见光透雾--任意值
	CAM_PARAM_ZGXOPENCROSSLINE,		   /*UINT*/						   //中光学相机-十字 0  消除---1显示
	CAM_PARAM_ZGXSETVIEWSCENCE,		   /*UINT*/						   //中光学相机-设置大视场-小视场 0-小 1大
	CAM_PARAM_ZGXIMAGEENHANCE,		   /*UINT*/						   //中光学相机-图像增强 0  关闭图像增强---1打开图像增强
	CAM_PARAM_ELEZOOM,				   /*UINT*/						   //中光学相机-电子放大 0 不放大  1放大
	CAM_PARAM_WHITEBLANK,              /*UINT*/						   //中光学相机热成像--0白热 1 黑热
	CAM_PARAM_BACKWARD,                /*float*/                       //倒退播放 速率 1/16 1/8 1/4/ 1/2 1 2 4 8 16 
    CAM_PARAM_PAUSE,                   /*UINT*/                        //暂停播放 0正常播放 1暂停
    CAM_PARAM_FORWARD,                 /*float*/                       //正向播放 速率 1/16 1/8 1/4/ 1/2 1 2 4 8 16 
    CAM_PARAM_STEPBACKWARD,            /*UINT*/                        //单帧后退 任意值
    CAM_PARAM_STEPFORWARD,             /*UINT*/                        //单帧正向 任意值
	CAM_PARAM_TURNTOPTZ,               /*PTZ_TURNTO_PARAM*/            //PTZ转向参数
	CAM_PARAM_HIKPTZAUTOTRACK          /*UINT*/						   //海康球机智能跟踪0-关闭跟踪 1-开启跟踪
} camera_param_t;


typedef enum 
{
    CTS_SUCCESS,                                         //成功
    CTS_NOTINIT,                                         //未初始化
    CTS_EXCEPTION,                                       //异常失败
    CTS_PARAM_ERROR,                                     //参数错误
    CTS_CONNECT_FAILED,                                  //连接失败
    CTS_NO_SUCH_CHANNEL,                                 //无此通道
    CTS_CAM_ALREADY_EXISIST,                             //通道已存在
    CTS_CAMERA_CONTROL_NOT_CONNECT,                      //相机控制连接未建立
    CTS_SET_PARAM_ERROR,                                 //设置参数失败
    CTS_GET_PARAM_ERROR,                                 //获取参数失败
    CTS_SET_PARAM_TO_CAM_ERROR,                          //给相机设置参数失败
    CTS_GET_PARAM_FROM_CAM_ERROR,                        //从相机获取参数失败
    CTS_SEND_CMD_FAILED,                                 //发送相机控制命令失败
    CTS_RECV_CMD_FAILED,                                 //接收相机控制命令返回消息失败
    CTS_CHECK_CMD_FAILED,                                //校验相机控制命令失败
    CTS_FUN_TIMEOUT,                                     //函数执行超时
    CTS_FUN_TOO_FREQ                                     //函数执行太频繁
} cts_res_t;


//转到PTZ的参数
typedef struct 
{
	double			fCamFlon;						    //相机经度
	double			fCamFlat;							//相机纬度
	double			fCamFarViewWidth;					//相机拉到最远处的场景宽度
	double			fCamNearViewWidth;					//相机拉到最近处的场景宽度
	double			fCamH;								//相机视角与地面的距离
	double			fCamCentDist;						//相机距离地面中心点的距离
	double			fDestFlon;							//目的经度
	double			fDestFlat;							//目的纬度
	double			fNorthAngle;						//与正北方向夹角
	double			fTargitW;							//目标实际宽度
}ptz_turnto_param_t, *ptz_turnto_param_ptr;


typedef struct 
{
    UINT            bStop;                               //是否停止相机转动1-停止 0继续工作不停止
    UINT            nDirectSpeed;                        //方向转动速度--范围MIN_PTZ_SPEED~MAX_PTZ_SPEED
    UINT            bNeedAutoStop;                       //是否自动停止
    UINT            nRes[19];                            //预留字节
}ptz_param_info_t, *ptz_param_info_ptr;


typedef enum 
{
    PRESET_OPT_SET,                                      //设置预置位
    PRESET_OPT_GOTO,                                     //转到预置位
    PRESET_OPT_DEL                                       //删除预置位  
} PRESET_OPT_TYPE;

typedef struct 
{
    float                             P;                  //水平
    float                             T;                  //垂直
    float                             Z;                  //远近
    UINT                             nPTZIndex;          //当且只有设置和获取预置点的时候有效
    PRESET_OPT_TYPE                  nPtzOpt;            //预置位操作方法
    UINT                             nRes[18];           //预留字节
} CTS_PTZ, *PCTS_PTZ;


//缩放后的图像size
typedef struct 
{
    UINT            w;                                   //宽
    UINT            h;                                   //高
    UINT            lx;                                  //左x
    UINT            ty;                                  //顶y
    UINT            nRes[20];                            //预留字节
}CTS_RECT, *PCTS_RECT;



/*******************************************************************************************************/
int camera_connect(char* ip, UINT port, struct timeval* time_out);
int camera_disconnect(int socket);

cts_res_t camera_ctl(int socket, camera_t camera, camera_param_t cmd, void* data, struct timeval* time_out);

cts_res_t get_ptz(int socket, camera_t camera, void* data, struct timeval* time_out);

cts_res_t SetParam(int socket, camera_t camera, camera_param_t t, void *data, struct timeval* time_out);

#endif
