#ifndef __CAPV__
#define __CAPV__

class CAPV
{
public:
	CAPV();
	~CAPV();
 
	void IniPara(int LaneCount,int checkModel[],int imageWidth,int imageHeight,
				   int VehicleLength,int VehicleWidth,int FVehicleLength,int FVehicleWidth,
				   int headLoop_Length,int headCap_Position,int fheadLoop_Length,int fheadCap_Position,
				   int backLoop_Length,int backCap_Position,int TopTH,int LowTH,int MoveTH,int FMoveTH,int Loop_Bottom[],
				   int *XLeft[],int *XRight[],double theDistance,int theFrame);

	//void DetectionVehicle(short *HimageD, short *HimageN,short *Vimage,int b[],int x1[],int x2[],int y1[],int y2[],int FlashBack_Num[],double Speed[],int SpeedPosition[],int thePos1[],int thePos2[],int daynight);
        void DetectionVehicle(unsigned char *SrcImage,int result[],int x1[],int x2[],int y1[],int y2[],int FlashBack_Num[],double Speed[],int SpeedPosition[],int thePos1[],int thePos2[],int daynight) ;
public:
	void gotTheID_capture(char ID[]);
	void theDebug(unsigned char * debugImage,int k,int projection[]);
	bool HeadCapture_ni(int aP[],bool *TopTrigger,bool *BottomTrigger, bool *theStatus,bool *theStatus_b,double Speed[],int k,int thePos1[],int thePos2[]);

	bool BackCapture_ni(int RoadNum,int aP[],bool *TopTrigger, bool *BottomTrigger, bool *theStatus,bool *theStatus_h,int & FlashCount,int & Bottom1,int & Bottom2,double Speed[],int k,int thePos1[],int thePos2[]);
	int VehicleSize_H(int RoadNum,int x1[],int x2[],int y1[],int y2[]);
	int VehicleSize_B(int RoadNum,int x1[],int x2[]);
	bool BackCapture(int RoadNum,int aP[], bool *TopTrigger, bool *BottomTrigger, bool *theStatus,int & FlashCount,int & Bottom1,int & Bottom2,double Speed[],int k,int thePos1[],int thePos2[]);
	int theDirection_Back(double Speed[],int k,int thePos1[],int thePos2[]);

	void GetEdge(short *HimageD, short *HimageN,int daynight);
	void GetDiff3(short* ps1, short* ps2, short* ps3, short* pd, int w, int h);
	void GetThreshold(short *image,int daynight);
	int  VehicleSize_H_F(int RoadNum, int x1[], int x2[], int y1[], int y2[]);
	void Projection(int daynight);
	void sp_Smooth(int a[], int n);
	bool HeadCapture(int aP[],bool *TopTrigger,bool *BottomTrigger, bool *theStatus,double Speed[],int k,int thePos1[],int thePos2[]);

	bool FHeadCapture(int aP[],bool *TopTrigger,bool *BottomTrigger, bool *theStatus,double Speed[],int k,int thePos1[],int thePos2[]);
	int theDirection_Head(double Speed[],int k,int thePos1[],int thePos2[]);
	void GetThreshold_F(short *image,int daynight);
        
        void Edge_Detect(unsigned char *SrcImage,int imgWidth,int imgHeight,int lineBits);
	int theWidth;
	int theHeight;

	int theHeadLoopLength;
	int theBackLoopLength;
	int headcapturePosition;
	int backcapturePosition;

	int theFHeadLoop_Length;   //����Զ��ץ��ʱ����Ȧ����
	int theFHeadCap_Position;  //����Զ��ץ��ʱ����Ȧ����

 
	int topTH;
	int lowTH ;

	int theTH;

	int theFTH;			     //����Զ��ץ��ʱ����Ҫ�õ�����ֵ

	int theVehicleWidth;
	int theVehicleLength;

	int theFVehicleWidth;	//����Զ��ץ��ʱ����С
	int theFVehicleLength;

	

	int m_nLaneCount;

	int checkmodel[8];

	int * xLeft[8];
	int * xRight[8];

	int * bProject[8];

	int * backProject[8];
	int * nightProjectB[8];
	int * headProject[8];
	int * nightProjectH[8];

	int * fheadProject[8];

	short * move_Image;

	int theBottomY[8];

	int block;

	short *frame1;
	short *frame2;
	short *frame3;

	int frameCount;

	short *de3;
	short *de3_1;

	int *edge1[8];
	int *edge2[8];
	int *edge3[8];
        
        short   *hedgeD;
	short   *vedge;
	short 	*hedgeN;
        

	bool topTrigger_H[8];
	bool bottomTrigger_H[8];
	bool theStatus_H[8];
	bool theStatus_B[8];
	bool theStatus_H_ni[8];
	bool theStatus_B_ni[8];


	bool topTrigger_B[8];
	bool bottomTrigger_B[8];

	int pos1;
	int pos2;
	int pos3;
	int Frame;

	double Distance;

	int HeadCount[8];
	int BackCount[8];

	int connect_len;

	int saved_Bottom1[8];
	int saved_Bottom2[8];
	int saved_flashback[8];
	int speedposition[8];

	int ASIZE;

	int * theRoadwidthHead[8];
	int * theRoadwidthBack[8];
	int * roadwidth[8];


};


#endif
