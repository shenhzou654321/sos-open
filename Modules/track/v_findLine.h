#ifndef	_FINDLINE_H_
#define _FINDLINE_H_



//480*270
#define SIMAGEW		720//512//640
#define SIMAGEH		576//300//520

#ifdef	__cplusplus
extern "C" {
#endif

    typedef struct  vFindLine
    {
        int pnGradX[SIMAGEW*SIMAGEH];  // 指向x方向导数的指针 
        int pnGradY[SIMAGEW*SIMAGEH];  // 指向y方向导数的指针
        int pnGradMag[SIMAGEW*SIMAGEH];// 梯度的幅度 
        
        double pdTmp[SIMAGEW*SIMAGEH];
        unsigned char  pUnchEdge[SIMAGEW*SIMAGEH];
        // 经过高斯滤波后的图象数据 
	unsigned char  pUnchSmooth[SIMAGEW*SIMAGEH];
	int boatY[1024];
        int boatL[1024];
        int boatR[1024];
	int boatnum;
        
        int Line[SIMAGEH];
        
    }vFindLine;
    
   void Canny(vFindLine* v_findline,unsigned char *pUnchImage, int nWidth, int nHeight, double sigma,double dRatioLow, double dRatioHigh) ;
   
   int FindLine(vFindLine* v_findline,unsigned char *pUnchImage, int nWidth, int nHeight,int x1,int y1,int x2,int y2);
   
   void smoothImage(unsigned char *pUnchImage, int nWidth, int nHeight);
    
#ifdef	__cplusplus
}
#endif
#endif



