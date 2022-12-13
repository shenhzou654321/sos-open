/* 
 * File:   vibe_proc.h
 * Author: Administrator
 *
 * Created on 2017年6月20日, 上午10:23
 */

#ifndef VIBE_PROC_H
#define	VIBE_PROC_H


#define DIMAGEH             620
#define DIMAGEW             720

#define POINTCOUNT_1        500

#define POINTCOUNT          200
#define BOATCOUNT           10
#define TRIGGERCOUNT        BOATCOUNT/2
#define PAUSEFRAMECOUNT     15 * 60 * 2  //2分钟

#ifndef WIN32
#define MASKCOUNT           3 //模板个数
#endif

#include "vibe-background-sequential.h"

#ifdef	__cplusplus
extern "C" {
#endif

    typedef struct vibe_inParam {
        int modw;
        int modh;
        int stopframecount;
        int headth;
        int tailth;
        int moveth;
        int cplex_moveth[5]; //复杂背景阈值：moveth[0]:水域； moveth[1]:河岸(有绿化带)

        int objminWidth;
        int objminHeight;

        int objmaxWidth;
        int objmaxHeight;

        int cloudminWidth;
        int cloudminHeight;

        int obj_min_h;
        int loop_length;
        int topth;
        int lowth;
        int loop_start_left;
        int loop_start_right;
        int loop_stay_time; //目标在线圈上保持最短时间(单位：秒)

        int vibe_expand_times; //运动空心填充次数

        int vibe_obj_keepTime; //vibe检测到目标后，目标存在的最短时间
        int vibe_obj_moveMinDis;

        int vibe_left; //检测区域
        int vibe_right;
        int vibe_top;
        int vibe_bottom;

        int vibe_output_left; //目标输出区域限制(当目标进入该区域后才输出)
        int vibe_output_right;
        int vibe_output_top;
        int vibe_output_bottom;

        char cam_uuid[128];

#ifndef WIN32    
        unsigned char maskImage[MASKCOUNT][DIMAGEH*DIMAGEW];
        unsigned char movethImage[DIMAGEH*DIMAGEW];
#else
        unsigned char maskImage[DIMAGEH*DIMAGEW];
#endif    
    } vibe_inParam;

    typedef struct vibe_kakou_inParam {
        int modw; //鎼滅储杩愬姩鍧楃殑绐楀彛澶у皬
        int modh;

        int moveth; //vibe妫�祴鑳屾櫙宸槇鍊�

        int obj_left_min_h; //宸︿晶杩涘叆鐩爣鐨勬渶灏忛珮搴�灏忎簬姝ら槇鍊煎垽瀹氫负骞叉壈)
        int obj_right_min_h; //鍙充晶杩涘叆鐩爣鐨勬渶灏忛珮搴�灏忎簬姝ら槇鍊煎垽瀹氫负骞叉壈)

        int loop_left_length; //宸︿晶绾垮湀闀垮害
        int loop_right_length; //鍙充晶绾垮湀闀垮害

        int top_left_th; //宸︿晶瑙﹀彂绾垮湀闃堝�
        int low_left_th; //宸︿晶閲婃斁绾垮湀闃堝�
        int top_right_th; //鍙充晶瑙﹀彂绾垮湀闃堝� 
        int low_right_th; //鍙充晶閲婃斁绾垮湀闃堝�

        int loop_left_start; //宸︿晶绾垮湀寮�浣嶇疆
        int loop_right_start; //鍙充晶绾垮湀寮�浣嶇疆

        int vibe_left; //妫�祴鍖哄煙
        int vibe_right;
        int vibe_top;
        int vibe_bottom;

        int vibe_output_left; //鐩爣杈撳嚭鍖哄煙闄愬埗(褰撶洰鏍囪繘鍏ヨ鍖哄煙鍚庢墠杈撳嚭)
        int vibe_output_right;
        int vibe_output_top;
        int vibe_output_bottom;

        char cam_uuid[128];

#ifndef WIN32    
        unsigned char maskImage[MASKCOUNT][DIMAGEH*DIMAGEW];
#else
        unsigned char maskImage[DIMAGEH*DIMAGEW];
#endif    
    } vibe_kakou_inParam;

    typedef struct boatInfo {
        int l; //?????λ??
        int r;
        int b;
        int t;

        int pos; //1:??? 2????β
        int direction; //1:????2??????

        int isfull; //????????? 0:??м??????1:?????
        int isvalid;
    } boatInfo;

    typedef struct boatPos {
        unsigned short l[POINTCOUNT]; //保存200个点
        unsigned short r[POINTCOUNT];
        unsigned short t[POINTCOUNT];
        unsigned short b[POINTCOUNT];

        unsigned short init_l; //初始坐标位置
        unsigned short init_r;
        unsigned short init_t;
        unsigned short init_b;

        unsigned short curr_area_l; //当前目标的最大面积(用于)
        unsigned short curr_area_r;
        unsigned short curr_area_t;
        unsigned short curr_area_b;

        unsigned short draw_area_l; //当前目标的最大面积(用于)
        unsigned short draw_area_r;
        unsigned short draw_area_t;
        unsigned short draw_area_b;

        double bestSizeTime; //目标尺寸达到最合适的开始时间

        int chosen; //被选中
        int direction; // 1 ????  2 ??????ж???????????
        int count;

        int objtype; //1:疑似目标；2：确定目标； 3：删除目标

    } boatPos;

    typedef struct objInfo {
        int used; //是否在使用
        int m_status; //1：可以被加载到输出队列，0：已经加载到输出队列

        int left;
        int right;
        int top;
        int bottom;
        int lenth;

        int bottomtrigger;
        int toptrigger;
        int triggercount; //触发持续时间
        char name[64]; //名字，用来调试时使用

        int m_dir; //1:正方向(左侧入,右侧入)；0：反向向(左侧出，右侧出)

        int count;

    } objInfo;

    typedef struct trigVideo {
        int used; //是否已经被占用
        double incVideoStartTime; //用于存储录像
        double incVideoEndTime;
    } trigVideo;

    //目标触发结构体(用来定位目标准确位置和后续跟踪)

    typedef struct trigInfo {
        int used; //是否已经被占用

        int trig_error; //跟踪错误(结束标志)

        int unsafe; //是否安全:0 安全  1：不安全
        int related_index[TRIGGERCOUNT]; //与之关联的目标序号
        int related_index_count; //与之关联的个数
        double prepush_speed; //预推速度，只计算一次


        int index; //目标编号(唯一标识)
        int dealed; //是否已经联动处理(0未处理，1开始处理-发送信息给球机，2结束处理-球机发送结果回来，-1: 不能处理)

        unsigned short l[POINTCOUNT]; //保存1000个点
        unsigned short r[POINTCOUNT];
        unsigned short t[POINTCOUNT];
        unsigned short b[POINTCOUNT];

        unsigned short init_l; //初始坐标位置
        unsigned short init_r;
        unsigned short init_t;
        unsigned short init_b;

        unsigned short changed_l; //修正坐标值：最终传给球机转向的坐标
        unsigned short changed_r;
        unsigned short changed_t;
        unsigned short changed_b;

        unsigned short prepush_l; //预推坐标值
        unsigned short prepush_r;
        unsigned short prepush_t;
        unsigned short prepush_b;
        double prepushLen;

        int flag_meet; //两船相遇
        int flag_meet_index;

        int flag_go_out_left;   //离开左侧标志
        int flag_go_out_right;  //离开右侧标志
        
        int flag_go_back_left;  //从左侧往回走标志
        int flag_go_back_right; //从右侧往回走标志    

        int count; //目标点个数
        int type; //0:左侧出，1：左侧入，2：右侧出，3：右侧入
        int m_export_count; //是否可以输出联动
        char name[64]; //名字，用来调试时使用

        double incVideoStartTime; //用于存储录像
        double incVideoEndTime;

        double startTime; //开始时间
        unsigned int difTime[POINTCOUNT]; //每一个点相对于开始时间的差值(毫秒)

    } trigInfo;

    typedef struct VibeParam {
        int m_bInitOK;

        int w;
        int h;

        int framecount;
        int framedx;

        int moveth;

        int modw; //目标运动块的最小尺寸(过滤掉其他目标)
        int modh;

        int vibe_obj_keepTime; //vibe检测到目标后，目标存在的最短时间

        int vibe_obj_moveMinDis; //目标移动的最短距离(小图像素点)

        int objminW; //最佳目标输出尺寸阈值
        int objminH;
        int objmaxW; //最佳目标输出尺寸阈值
        int objmaxH;

        int cloudminW; //云阴影导致的最小目标
        int cloudminH;

        int lastaveGray; //上一次平均灰度
        int unusual;

        int objtest; //测试标记,暂时用，之后删除

        int vibe_left; //检测区域
        int vibe_right;
        int vibe_top;
        int vibe_bottom;

        double dealtime; //当前处理帧的时间

        double initime; //初始化时间

        int throwing; //倍选中的目标消失后，该标记置为1
        int checked; //被选中
        int lastframedx;

        vibeModel_Sequential_t *model; //vibe???

        unsigned char segmentation_Image[DIMAGEH*DIMAGEW];
        unsigned char segmentation_Image_1[DIMAGEH*DIMAGEW];

        unsigned char segmentation_Image_2[DIMAGEH*DIMAGEW];

        unsigned char segmentation_Image_3[DIMAGEH*DIMAGEW];

        unsigned char segmentation_Image_4[DIMAGEH*DIMAGEW];

        int blockgray[15 * 9];


        int debug_cout;
        char cam_uuid[128]; //相机uuid，方便调试
        //////////
        trigInfo m_triginfo[TRIGGERCOUNT]; //存放目标触发数据

        //trigVideo m_trigvideo[TRIGGERCOUNT]; //存放目标触发数据

        objInfo m_Lobjinfo[BOATCOUNT]; //左侧检测目标
        objInfo m_Robjinfo[BOATCOUNT]; //右侧检测目标

        int shinning_pointCount[6];


        int topth;
        int lowth;
        int m_boatminH; //船的最小高度
        int m_boatminL; //船的最小长度
        int m_loopLength; //检测线圈长度
        int m_loopstartL; //左侧线圈开始位置
        int m_loopstartR; //右侧线圈开始位置

        int loop_stay_time; //线圈上保持最短时间(单位：秒)
        int vibe_expand_times; //运动空心填充次数

        int bLProject[2][DIMAGEH]; //左侧投影
        int bRProject[2][DIMAGEH]; //右侧投影
        int bProjTmp[DIMAGEH];
        //////////

        unsigned char segmentation_Image_temp[DIMAGEH*DIMAGEW];

        unsigned char checkdir_head_image[DIMAGEH*DIMAGEW]; //???????????м????
        unsigned char checkdir_tail_image[DIMAGEH*DIMAGEW]; //??β????????м????

        int m_boatHeadCount; //???????????
        int m_boatTailCount; //??β????????

        int maskimagevalid; //掩码图是否有效(掩码图全是0，则判定无效)
        unsigned char maskImage[DIMAGEH*DIMAGEW];
        unsigned char movethImage[DIMAGEH*DIMAGEW];

        boatPos m_boatheadPos[BOATCOUNT]; //????????????λ??仯
        boatPos m_boattailPos[BOATCOUNT]; //?????β??????λ??仯

    } VibeParam;

    typedef struct VibeKakouParam {
        int m_bInitOK;

        int w;
        int h;


        int modw; //鐩爣杩愬姩鍧楃殑鏈�皬灏哄(杩囨护鎺夊叾浠栫洰鏍�
        int modh;

        int moveth;

        int top_left_th; //宸︿晶瑙﹀彂绾垮湀闃堝�
        int low_left_th; //宸︿晶閲婃斁绾垮湀闃堝�
        int top_right_th; //鍙充晶瑙﹀彂绾垮湀闃堝� 
        int low_right_th; //鍙充晶閲婃斁绾垮湀闃堝�

        int m_boatminHL; //鑸瑰湪宸︿晶鏈�皬楂樺害
        int m_boatminHR; //鑸瑰湪鍙充晶鏈�皬楂樺害

        int m_loopLengthL; //妫�祴绾垮湀闀垮害
        int m_loopLengthR; //妫�祴绾垮湀闀垮害

        int m_loopstartL; //宸︿晶绾垮湀寮�浣嶇疆
        int m_loopstartR; //鍙充晶绾垮湀寮�浣嶇疆

        int m_shadow_flag; //鏄惁鍘诲�褰憋細0涓嶅幓锛�锛氬�褰卞湪涓嬫柟; 2:鍊掑奖鍦ㄤ笂鏂�3:鍊掑奖鍦ㄥ乏渚�4:鍊掑奖鍦ㄥ彸渚�


        int lastaveGray; //涓婁竴娆″钩鍧囩伆搴�
        int unusual;


        int vibe_left; //妫�祴鍖哄煙
        int vibe_right;
        int vibe_top;
        int vibe_bottom;
        int maskimagevalid; //鎺╃爜鍥炬槸鍚︽湁鏁�鎺╃爜鍥惧叏鏄�锛屽垯鍒ゅ畾鏃犳晥)

        double dealtime; //褰撳墠澶勭悊甯х殑鏃堕棿


        int blockgray[15 * 9];
        int bLProject[2][DIMAGEH]; //宸︿晶鎶曞奖
        int bRProject[2][DIMAGEH]; //鍙充晶鎶曞奖
        int bProjTmp[DIMAGEH];

        trigInfo m_triginfo[TRIGGERCOUNT]; //瀛樻斁鐩爣瑙﹀彂鏁版嵁
        objInfo m_Lobjinfo[BOATCOUNT]; //宸︿晶妫�祴鐩爣
        objInfo m_Robjinfo[BOATCOUNT]; //鍙充晶妫�祴鐩爣


        vibeModel_Sequential_t *model; //vibe

        unsigned char segmentation_Image[DIMAGEH*DIMAGEW];
        unsigned char segmentation_Image_1[DIMAGEH*DIMAGEW];
        unsigned char segmentation_Image_2[DIMAGEH*DIMAGEW];

        unsigned char maskImage[DIMAGEH*DIMAGEW];


    } VibeKakouParam;

    int vibe_smoothImage(unsigned char *pUnchImage, int nWidth, int nHeight);

    void vibe_Light_Init(VibeParam* pthis, unsigned char * SrcImage, int w, int h, vibe_inParam m_vibe_inparam, int index);
    int vibe_Light_Proc(VibeParam* pthis, unsigned char* SrcImage, int framedx, int w, int h, int moveth, int headth, int tailth, int* gray, boatInfo* boatinfo, int *debug);


    void vibe_infrared_Init(VibeParam* pthis, unsigned char * SrcImage, int w, int h, vibe_inParam m_vibe_inparam, int index);
    int vibe_infrared_Proc(VibeParam* pthis, unsigned char* SrcImage, int framedx, int w, int h, int* gray, boatInfo* boatinfo, int *debug);

    //联动
    // void vibe_kakou_Init_linkage(VibeKakouParam* pthis, unsigned char * SrcImage, int w, int h, vibe_kakou_inParam m_vibe_inparam);
    // int  vibe_kakou_Proc_linkage(VibeKakouParam* pthis, unsigned char* SrcImage, int framedx, int w, int h, int* gray, trigInfo* trigOutInfo, int *debug);

    //    void vibe_Init_linkage(VibeParam* pthis, unsigned char * SrcImage, int w, int h, vibe_inParam m_vibe_inparam,int index) ;
    //    int vibe_Proc_linkage(VibeParam* pthis, unsigned char* SrcImage, int framedx, int w, int h, int moveth, int headth, int tailth, int* gray, trigInfo* trigOutInfo, int *debug);
    //   int vibe_Proc_linkage(VibeParam* pthis, unsigned char* SrcImage, double currtime, int framedx, int w, int h, int moveth, int headth, int tailth, int* gray, trigInfo* trigOutInfo, int *debug);
    int vibe_Proc_linkage(VibeParam* pthis, unsigned char* SrcImage, double currtime, int framedx, int w, int h, int moveth, int headth, int tailth, int* gray, int *pointcount, trigInfo* trigOutInfo, int *debug);

    void vibe_Init_linkage(VibeParam* pthis, unsigned char * SrcImage, double currtime, int w, int h, vibe_inParam m_vibe_inparam, int index);
    //int vibe_handle_obj_info(VibeParam* pthis,int index,int w, int h, double* speedx, double* speedy);
    //int vibe_handle_obj_info_1(VibeParam* pthis, int index, int *outl, int *outr, int *outt, int *outb,int* debug) ;

    void vibe_Del(VibeParam* pthis);


#ifdef	__cplusplus
}
#endif

#endif	/* VIBE_PROC_H */

