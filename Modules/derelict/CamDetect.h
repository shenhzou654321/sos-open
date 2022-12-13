#ifndef _CAM_DETECT_H_
#define _CAM_DETECT_H_

#define  restrict

#define DETECT_WIDTH		500//400//1600//1368//1132//720
#define DETECT_HEIGHT		300//300//1200//768//520//576
#define DETECT_VSIZE		(DETECT_WIDTH * DETECT_HEIGHT)

//////////////////////////////////////////////////////////
#define S_WIDTH  128  //�����λ��������  
#define S_HEIGHT 128

#define A_WIDTH  32  //�����λ��ⴰ��  
#define A_HEIGHT 32

#define MAXOBJ	2





///////////////////////////////////////////////////////
///////////////////////////////////////////////////////
#define MAXLINE	4

#define AVEW	9
#define AVEH	9
#define THETIME  10  //Ӧ���и�Ϊ30

#define  WLEN  32
#define  HLEN  32

#ifdef	__cplusplus
extern "C" {
#endif
    
typedef struct CRect
{
	int bottom;
	int top;
	int left;
	int right;
}CRect;


typedef struct CameraMove
{
	double  _data[2][128];     //����ģ��
	CRect rect;
	double dif;
	int left,right,top,bottom; //�ƶ��������
	int sleft,sright,stop,sbottom; //�ƶ��������

	//���ټ������Ļ��ͼ
	int integration_hedge[S_WIDTH*S_HEIGHT]; 
	int integration_vedge[S_WIDTH*S_HEIGHT];
	int integration_hgrey[S_WIDTH*S_HEIGHT];
	int integration_vgrey[S_WIDTH*S_HEIGHT];

	//���ټ����ı�Եͼ
	unsigned char Himage[S_WIDTH*S_HEIGHT];
	unsigned char Vimage[S_WIDTH*S_HEIGHT];
	unsigned char Pimage[S_WIDTH*S_HEIGHT];

}CameraMove;

////////////////////////////////////////////////////////////
//����ڵ��ṹ
typedef struct CameraHide
{
	int grey[256];
	int greynow[256];

	//for test
	int hide;
}CameraHide;

////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////
//�������ṹ
typedef struct CameraThrow 
{
	int theTime;
	int moveTime;
	int theFrame;

	int theCheckValue;
	int theobjectTH;

	int theRegionCount;
	int theRegionSize;

	int pos[MAXOBJ];
	int posx[MAXOBJ];
	int posy[MAXOBJ];
	int objectWidth[MAXOBJ];
	int objectHeight[MAXOBJ];
	int objectFlag[MAXOBJ];
	int objectGrey[MAXOBJ];
	int objectCount[MAXOBJ];
	int objectMovetime[MAXOBJ];
	int incStatus[MAXOBJ];

	int block;
	int theMoveTH;

	int	 back_bits[DETECT_WIDTH*DETECT_HEIGHT];

	int	i_Image[(DETECT_WIDTH+1)*(DETECT_HEIGHT+1)];

	int	 work_a[DETECT_WIDTH*DETECT_HEIGHT];

	unsigned char  eventMask[DETECT_WIDTH*DETECT_HEIGHT];

	unsigned char	 frame1[DETECT_WIDTH*DETECT_HEIGHT];
	unsigned char	 frame2[DETECT_WIDTH*DETECT_HEIGHT];
	unsigned char	 frame3[DETECT_WIDTH*DETECT_HEIGHT];

	int back_bits_current[DETECT_WIDTH*DETECT_HEIGHT];

	int *de1;
	int *de2;

	unsigned char de3[DETECT_WIDTH*DETECT_HEIGHT];

	int	 move_Image[DETECT_WIDTH*DETECT_HEIGHT];

	int	 inc_bits[DETECT_WIDTH*DETECT_HEIGHT];
	int	 serial_count[DETECT_WIDTH*DETECT_HEIGHT];

}CameraThrow;

typedef struct AreaHide
{
	int sl;
	int st;
	int sr;
	int sb;

	//for test
        int x1;
    	int x2;
	int y1;
	int y2;
        int dif;
}AreaHide;

///////////////////////////////////////////////////////////

typedef struct vParam
{
	int  s1[2][8];			//2�����������������꣨����x������y������x������y������x������y������x������y��
	int  s2[2];				//��λ������ĵ���꣨x��y����
	int  s3[4];				//1��ָ���ڵ����������꣨���ϡ��ҡ��£�
	int  frameRate;			//��Ƶ����֡��
	int  eventTime;			//�¼�������ʱ��,��λΪ��
	int  nReserved[16];		//�����ֶ�
}vParam;

typedef struct vOutData
{
	int  nlen;				//��ݰ��
	int  nChannel;			//����ͨ��
	int  nFrameNo;			//����֡��
	int  nType;				//�������ͣ�ͬ����ֵ
	int	 nRect[2][4];		//���������꣨���ϡ��ҡ��£�
	int  nReserved[16];		//�����ֶ�
}vOutData;


typedef struct vCamDetecter
{
	int  bInit;	
	
	int  width;
	int  height;
	int  framesize;
	
	int  bs0init;	//������0
	int  bs0;
	int  bs0out;
	int  bs0time;
	
	int  bs1init;	//������1
	int  bs1;		
	int  bs1out;
	int  bs1time;
	
	int  bs2init;	//�������λ
	int  bs2;
	int  bs2out;
	int  bs2time;
	
	int  bs3init;	//�����ڵ�
	int  bs3;
	int  bs3out;
	int  bs3time;
	
	int  bs4init;	//������ڵ�
	int  bs4;
	int  bs4out;
	int  bs4time;
	
	int  dx;
	int  dy;

	vParam		sparam;
	CameraHide  zparam;       //����ڵ�����
	AreaHide	aparam;       //���ֲ��ڵ�����
	CameraMove  cameramove;
	CameraThrow camerathrow;
	
	unsigned char fm[DETECT_VSIZE];	   //�����ģ��
	unsigned char bgFrame[DETECT_VSIZE];  
	unsigned char edgeFrame[DETECT_VSIZE];

	vOutData outd;
	
}vCamDetecter;

void Normalise(double _data[]);
void CreateHistogram(double _data[],int integration_hedge[],int integration_vedge[],int integration_hgrey[],int integration_vgrey[],CRect targetRoi);
void CreateHIntegration(unsigned char* ps,int width,int height,int xstep,int integration[]);
void CreateVIntegration(unsigned char* ps,int width,int height,int xstep,int integration[]);
void Ini_CamMove(int nChannel,unsigned char* Simage,int width,int height,int xstep,int x1,int y1,int x2,int y2, int sx1,int sy1,int sx2,int sy2);
void Proc_CamMove(int nChannel,unsigned char* Simage,int width,int height,int xstep,int* dx,int *dy,double *dif);
void Init_CameraHide(int nChannel, unsigned char *pData, int nWidth, int nHeight);
int _DetectCameraHide(int nChannel, unsigned char *pData, int nWidth, int nHeight);
int Init_AreaHide(int nChannel, unsigned char *pData, int nWidth, int nHeight, int l, int t, int r, int b);
int _DetectAreaHide(int nChannel, unsigned char *pData, int nWidth, int nHeight, int l, int t, int r, int b);
void CreateIntegration(int* ps,int width,int height,int xstep,int ing[]);
void AverageDIB(int nChannel,int *pSBits, int nWidth, int nHeight, int nWinWidth, int nWinHeight, int *pDBits);
void MaxValue55(unsigned char* ps,int w,int h,int* pd);
void theEdge_3(int nChannel,unsigned char *Vimage);
void gotTheID_derelict(char ID[]);
void theDebug(int nChannel,unsigned char *debugImage, int k);
void deFrames(int nChannel,int *first, int *second, int *result);
void GetThreshold(int nChannel,int *image);
void GetDiff3(unsigned char *ps1, unsigned char *ps2, unsigned char *ps3, int *pd, int w, int h);

void theDayEdge(unsigned char *restrict greyImage, unsigned char *restrict pd, int w, int y1, int y2);

void theEdge(unsigned char *greyImage, int w, int h, unsigned char *edgeImage);
void theEdgeInc(int nChannel, unsigned char *edgeImage);
void theNew(int nChannel);
void theFinished(int nChannel);
void CurrentBackground(int nChannel,int x, int y);
void IniPara_derelict(int nChannel,unsigned char *Pre_Image,int RegionCount,int RegionSize,unsigned char *MaskImage,int imageWidth,int imageHeight,int checkValue,int moveTH,int time,int frame);
int Detection_derelict(int nChannel,unsigned char *Edge_Image,int incidentStatus[],int incpos[],int theLeft[],int theRight[],int theTop[],int theBottom[],int x,int y);
void DrawLine(unsigned char *pd, int sw, int sh, int c, int x1, int y1, int x2, int y2);



#ifdef	__cplusplus
}
#endif
#endif
