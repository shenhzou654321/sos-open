#ifndef	_V_TRACK_RTG_H_
#define _V_TRACK_RTG_H_

//#define  PRINT_TRACK_DEBUG
#define MAX_W		720//512//640
#define MAX_H		576//300//520

#define TRACK_LEN 32
#define HIST_LEN (TRACK_LEN*4)

#define MAX_OBJ	2

#ifdef	__cplusplus
extern "C" {
#endif
typedef struct TrackParam
{
	int bUsed;
	int nProcType;	//0:��ʼ����1:����
	
	int disp;
	int frameIdx;
	
	int il;					//��ʼ��λ��
	int it;
	int ir;
	int ib;
	
	int track_count;
	int track_error;
	
	int ol;				//�����ǰ֡λ��
	int ot;
	int orr;
	int ob;
	
        
	int x2;			//�����һ֡x����λ��
	int y2;			//�����һ֡y�²�λ��
	
	int x3;			//�������һ֡x����λ��
	int y3;			//�������һ֡y�²�λ��	

	int predict_cx;
	int predict_cy;
	int search_width;
	int search_height;
	
}TrackParam;

typedef struct STRACKAREA
{
	int l;
	int r;
	int t;
	int b;

}STRACKAREA;

typedef struct vTrack
{
	int		rw;
	int		rh;
	
	int 		histmod[HIST_LEN];
	int 		histsmp[25][HIST_LEN];
	
	int		track_count;
	STRACKAREA      area_rect[10240];
	int		match_dif[10240];
	int		findex[10240];
	
        int             lastctrlFramedx;  //记录上一次球机上下运动的帧号
        int             ctrlCount;        //记录上下运动的次数
        
        int             step_cx[10240]; //统计在x和y方向上的运动步长
        int             step_cy[10240];
        int             sta_count;
        
	int predict_cx;
	int predict_cy;
	int search_width;
	int search_height;

	int update_ratio;
	int predict_ratio;
	int disappear_point;
	
	TrackParam ta;
	
}vTrack;

typedef struct vProc
{
	int		w;
	int		h;

	int		rw;
	int		rh;
	
	//unsigned char simage[MAX_W*MAX_H*2];
        
//	unsigned char rectgray[MAX_W*MAX_H/4];
//	short		vedge[MAX_W*MAX_H/4];
//	short		hedge[MAX_W*MAX_H/4];
//	
//	int 		ihe[MAX_W*MAX_H/4];	
//	int		ihg[MAX_W*MAX_H/4];
//	int		ive[MAX_W*MAX_H/4];
//	int		ivg[MAX_W*MAX_H/4];
        
        ///
        
        
        unsigned char g_rectgray[MAX_W*MAX_H/2];
        short		g_vedge[MAX_W*MAX_H/2];
        short		g_hedge[MAX_W*MAX_H/2];	
        int 		g_ihe[MAX_W*MAX_H/2];	
        int		g_ihg[MAX_W*MAX_H/2];
        int		g_ive[MAX_W*MAX_H/2];
        int		g_ivg[MAX_W*MAX_H/2];
        ///
        
	
	vTrack		m_track[MAX_OBJ];
	
}vProc;


void vEpl_create(vProc *pthis);


/*
 * unsigned char * image, int w, int h: 图像 ，图像尺寸
 * vProc *pthis：中间变量值
 * TrackParam * pm：跟踪结果结构体
 * int framedx：帧号
 * int camera_type: 相机类型： 1：光电仪  2：海康球机
 * int lastctrldx： 上一次发送转动命令的帧号
 * int lastctrlxdx：上一次发送左右转动命令的帧号
 * int lastctrlydx：上一次发送上下转动命令的帧号
 * 
 * int movedirx：   x轴方向的移动方向：1：向左   2：向右
 * int movediry：   y轴方向的移动方向：1：向上   2：向下
 * int stopTime：   目标多长时间(帧数)没有运动，就结束跟踪
 */
void vEpl_proc(unsigned char * image, int w, int h,vProc *pthis,TrackParam * pm, int framedx,int camera_type, 
        int lastctrldx,int lastctrlxdx,int lastctrlydx,int movedirx,int movediry,int stopTime,int * debug);

void vEpl_debug(unsigned char *pd, int *p0, int *pc);
#ifdef	__cplusplus
}
#endif
#endif	//_V_TRACK_RTG_H_
