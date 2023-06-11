#ifndef __CWave__
#define __CWave__

#define WaveW		512//640
#define WaveH		400//520


#ifdef	__cplusplus
extern "C" {
#endif
    
typedef struct {
    int theWidth;
    int theHeight;
    int m_RegionCount;
    int block;
    int ASIZE;

    int zoom;

    unsigned char srcImage[WaveW*WaveH];
    unsigned char Hedge[WaveW*WaveH];
    unsigned char Vedge[WaveW*WaveH];

    unsigned char Hedge_D[WaveW*WaveH];
    unsigned char Vedge_D[WaveW*WaveH];

    double theGrad[WaveW*WaveH];
    double i_Image[WaveW*WaveH];
    int orient[WaveW*WaveH];

    int theSum;
    int tempNum;

    short tempRect_X[4];
    short tempRect_Y[4];
    double tempResult[0];
    short tempSize[4];
    
    short vehicleRect_X[4];
    short vehicleRect_Y[4];
    double vehicleResult[4];
    short vehicleSize[4];  
    int vehicleNum;

    //double feature1[901];
    double feature2[1297];
    double result64;
    double result76;

    unsigned int theLeft;
    unsigned int theRight;
    unsigned int theTop;
    unsigned int theBottom;

   // double theBlock[WaveW/2*WaveH/2*2];
   // double TheGrade[WaveW/2*WaveH/2*10];
    
} vWaveParam;

int DetectionWave(unsigned char *image,int daynight,int vehicleX[],int vehicleY[],int size[],double result[]);
int IniPara_Wave(unsigned char * pImage,int imageWidth,int imageHeight,int left,int right,int top,int bottom);

#ifdef	__cplusplus
}
#endif
#endif
