//ˮ�����1.0

#ifndef __CWave__
#define __CWave__

class CWave
{
public:
	CWave();
	~CWave();
 
	int IniPara_Wave(int imageWidth,int imageHeight,int left,int right,int top,int bottom);
	int DetectionWave(unsigned char *image,int daynight,int vehicleX[],int vehicleY[],int size[],double result[]);

public:
	void process();
	void CreateIntegration(double *ps, int width, int height, int xstep, double ing[]);
	void gotTheID_vehicle(char ID[]);
	void theDebug(unsigned char * debugImage,int k,int projection[]);
	void GetEdge(unsigned char *image,int daynight);


	int theWidth;
	int theHeight;
	int m_RegionCount;
	int block;
	int ASIZE;


	unsigned char *Hedge;
	unsigned char *Vedge;

	unsigned char *Hedge_D;
	unsigned char *Vedge_D;

	double * theGrad;
	double * i_Image;
	int *orient;

	int theSum;


	int tempNum;

	short  tempRect_X[10000];
	short  tempRect_Y[10000];
	double tempResult[10000];
	int tempSize[10000];

	
	short vehicleRect_X[100];
	short vehicleRect_Y[100];
	double vehicleResult[100];
	int vehicleSize[100];
	int vehicleNum;

	double feature1[901];
	double feature2[1297];
	double result64;
	double result76;

	int theLeft;
	int theRight;
	int theTop;
	int theBottom;

	unsigned char *image1;
	unsigned char *image2;
	double *move_image;
	double *ii_Image;
};


#endif
