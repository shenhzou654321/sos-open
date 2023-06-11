#ifndef	_VSEARCH_H_
#define _VSEARCH_H_

#include "v_findLine.h"
#include "vibe-background-sequential.h"

//#define  PRINT_SEARCH_DEBUG
#define  PRINT_SEARCH_DEBUG_1
//480*270
#define SIMAGEW		720//512//640
#define SIMAGEH		576//300//520

#ifdef	__cplusplus
extern "C" {
#endif
typedef struct  peImage
{
	int framedx;
	unsigned char edgeImage[SIMAGEH*SIMAGEW];

}peImage;

typedef struct
{
	int m_bInitOK;      //此标记用来做第一次运动块搜索使用
        
    int m_bInitOK_1;    //此标记用来做二次运动块搜索使�?
    int m_reFindMoveFramedx; //二次搜索都运动块的帧�?
        
	int imagewidth;
	int imageheight;

	int m_findobj;				// 0:û���ҵ��˶��飬1���ҵ��˶��飬-1���ڹ涨֡�����Ҳ����˶���

	int m_searchwidth;			//������Χ����ȫͼ���ĵ�Ϊ���������ĵ㣬���¸߶�ΪsearchWidth�����ҿ��ΪsearchHeight
	int m_searchheight;
        
    int m_search_x1;
    int m_search_y1;
    int m_search_x2;
    int m_search_y2;
        
	int m_modWidth;				//�������ڵĴ�С
	int m_modHeight;
	int m_scale;				//�˶����ڴ�������ռ����

	int m_framedx;				//�ڲ�����֡��
	int m_stopFrameCount;		//�ڶ���֡�����˶���⣬�������֡�����ʾ���ʧ�ܣ�m_findobj = -1��

	int m_Moveth;				//�˶����С��Ĭ�ϣ�?5

	int m_effect;	
	int left_flag;	//������?
	int right_flag; //�Ҳ���Ч

	int LineUp;				   //���˶���ĵײ�y������Ѱ���ִ��ĵײ�
	int LineDown;

	int boatbottom_leftY;	//����ִ��ײ�Ԥ��λ��?
	int boatbottom_rightY;  //�Ҳ��ִ��ײ�Ԥ��λ��

	unsigned short Histy_left[SIMAGEH];	//��y���ϵ�ͶӰ
	unsigned short Histy_right[SIMAGEH];

	unsigned short Histx_left[SIMAGEW];	//��x���ϵ�ͶӰ
	unsigned short Histx_right[SIMAGEW];
	//int Histx_1[516];
	//int Histx_2[516];

	int Hist_left_l;	//�����߽�
	int Hist_left_r;   //����ұ߽�?

	int Hist_right_l;  //�Ҳ���߽�?
	int Hist_right_r; //�Ҳ��ұ߽�

    unsigned short    histy[SIMAGEH];
    unsigned short    histy_array[SIMAGEH];
    unsigned short    maxLength[200];       
        
    unsigned short    boatlen[10];              //统计10次船底检�?
    unsigned short    boatlenCnt;               //统计次数
    unsigned short    referboatlen;             //船底检测参考�?
    unsigned short    shortboatCnt;             //检测出船底小于阈值的次数(如果连续3次船底长度都小于阈值，则判�?
        
    unsigned short    firstEdgeCount;           //统计开始跟踪时，在跟踪框内的边缘像素点的个�?像素值大�?00�?

	short             hedgeD[SIMAGEH*SIMAGEW];
	short             vedge[SIMAGEH*SIMAGEW];
	short 	          hedgeN[SIMAGEH*SIMAGEW];
        
	unsigned char moveImage[SIMAGEH*SIMAGEW];

	short moveiImage[SIMAGEH*SIMAGEW];
	unsigned char projImage[SIMAGEH*SIMAGEW];
	unsigned char fillImage[SIMAGEH*SIMAGEW];

	short moveiImage_test[SIMAGEH*SIMAGEW];
	//unsigned char moveiImage_test1[SIMAGEH*SIMAGEW];
        
    unsigned char edgeImageSec[SIMAGEH*SIMAGEW+SIMAGEW];

	unsigned char segmentation_Image[SIMAGEH*SIMAGEW];
       // unsigned char temp_Image[SIMAGEH*SIMAGEW];

	vibeModel_Sequential_t *model; /* Model used by ViBe. */
        
    vFindLine  m_findLine;  //全图找直线模�?
        
	peImage p_image[4];


}vSeach;

//��һ�������ڣ���һ���̶���С���ڽ������������ѵ���ĳ��λ�ã������ڵ��˶���ռ���ڵ�һ������ʱ�����ж���������һ��Ŀ���˶���
typedef struct  vSeachConfig
{
	
	int searchWidth;		//������Χ����ȫͼ���ĵ�Ϊ���������ĵ㣬���¸߶�ΪsearchWidth�����ҿ��ΪsearchHeight
	int searchHeight;

	int modWidth;		  //�������ڵĴ�С
	int modHeight;
	int scale;			  //�˶����ڴ�������ռ����

	int stopFrameCount;	 //�ڶ���֡�����˶���⣬�������֡�����ʾ���ʧ�ܣ�m_findobj = -1��

	int Moveth;			//�˶����С��Ĭ�ϣ�?5
        
        int isAssistCheck;          
        int isAssistfindbottom;
        int assistparam1;
        int assistparam2;
        int assistparam3;
        
}vSeachConfig;


//��������ʱ����һ�Σ�ȫ���ֻ�ܵ���һ��?
void vSearchCreate(vSeach * pthis);

//��ʼ������
void InitSearchObj(vSeach* vseach,unsigned char*  pImage,int w,int h,vSeachConfig* p);

void DeleteSearchObj(vSeach* vseach);


//int AssistSearchObjProc(vSeach* vseach,unsigned char*  pImage,int w,int h,int max_x,int max_y,int* m_boatheight,int *m_boat_left,int *m_boat_right,int *m_boat_bottom_y,int *m_boat_enterall,int *m_effective);
//int AssistSearchObjProc(vSeach* vseach,unsigned char*  pImage,int w,int h,int *max_x,int *max_y,int* m_boatheight,int *m_boat_left,int *m_boat_right,int *m_boat_bottom_y,int *m_boat_enterall,int *m_effective,int *debug);
//int AssistSearchObjProc(vSeach* vseach,unsigned char*  pImage,int w,int h,int *max_x,int *max_y,int* m_boatheight,int *m_boat_left,int *m_boat_right,int *m_boat_bottom_y,int *m_boat_enterall,int *m_effective,int m_isassist,int *debug);
//int AssistSearchObjProc(vSeach* vseach,unsigned char*  pImage,int w,int h,int *max_x,int *max_y,int* m_boatheight,int *m_boat_left,int *m_boat_right,int *m_boat_bottom_y,int *m_boat_enterall,int *m_effective,int m_isassist,int m_assistparam1,int *debug);
//int AssistSearchObjProc(vSeach* vseach,unsigned char*  pImage,int w,int h,int *max_x,int *max_y,int* m_boatheight,int *m_boat_left,int *m_boat_right,int *m_boat_bottom_y,int *m_boat_enterall,int *m_effective,int m_isassist,int m_assistparam1,int m_assistparam2,int *debug);
int AssistSearchObjProc(vSeach* vseach,unsigned char*  pImage,int w,int h,long long tracknum,int *max_x,int *max_y,int* m_boatheight,int *m_boat_left,int *m_boat_right,int *m_boat_bottom_y,int *m_boat_enterall,int *m_effective,int m_isassist,int m_assistparam1,int m_assistparam2,int *debug);

int AssistSearchObjProc_0924(vSeach* vseach,unsigned char*  pImage,int w,int h,int *max_x,int *max_y,int* m_boatheight,int *m_boat_left,int *m_boat_right,int *m_boat_bottom_y,int *m_boat_enterall,int *m_effective,int m_isassist,int m_assistparam1,int *debug);

int AssistSearchObjInit(vSeach* vseach,unsigned char*  pImage,int w,int h);

//��������
//int SearchObjProc(vSeach* vseach,unsigned char*  pImage,int *max_x,int *max_y,int* m_boatheight,int *m_boat_left,int *m_boat_right,int *m_boat_bottom_y,int *m_boat_enterall,int *m_effective);
//int SearchObjProc(vSeach* vseach,unsigned char*  pImage,int *max_x,int *max_y,int* m_boatheight,int *m_boat_left,int *m_boat_right,int *m_boat_bottom_y,int *m_boat_enterall,int *m_effective,int *m_litter);
int SearchObjProc(vSeach* vseach,unsigned char*  pImage,int *max_x,int *max_y,int* m_boatheight,int *m_boat_left,int *m_boat_right,int *m_boat_bottom_y,int *m_boat_enterall,int *m_effective,int *m_litter,int* debug_x,int* debug_y );


void vSearchDebug(vSeach * pthis,unsigned char *p0, unsigned char *p1, unsigned char *p2);

//int SecondCheck(vSeach* vseach,unsigned char*  pImage,int m_effective,int m_boat_bottom_y,int m_last_left,int m_last_right,int *m_curr_left,int *m_curr_right);
int SecondCheck(vSeach* vseach,unsigned char*  pImage,int sw,int sh,int m_effective,int m_boat_bottom_y,int m_last_left,int m_last_right,int *m_curr_left,int *m_curr_right);
int Check_Edge(vSeach* vseach,unsigned char*  pImage);
int CheckTheEdgeCount(vSeach* vseach,int first,int framedx,int x1,int y1,int x2,int y2);
int TrackTheboatBottom(vSeach* vseach,int framedx,int x1,int y1,int x2,int y2,float* rate);

void reInitSearchObj(vSeach* vseach,int w,int h,vSeachConfig* p,int x1,int y1,int x2,int y2);
int reSearchObjProc(vSeach* vseach,unsigned char*  pImage,int framedx);
#ifdef	__cplusplus
}
#endif
#endif



