#ifndef _V_FREEWALKER_H_
#define _V_FREEWALKER_H_



#ifdef	__cplusplus
extern "C" {
#endif


#define IMAGE_H		1080
#define IMAGE_W		1920

#define SCALE_COUNT  29*2		//�̶ȼ���

#define MAX_Z  3600				//��С�Ŵ���
#define MIN_Z   100				//��󷽷�����


#define P_NUMBER  4000			//球机刻度
#define T_NUMBER  2000			//球机刻度
#define Z_NUMBER  2000			//球机刻度

#define MAX_CHANNEL  30			

    typedef struct FreeWalkerParam {
        int m_InitType; //�Ƿ��ʼ�� 

        int ImageWidth; //ͼ��ĳߴ�
        int ImageHeight;


        int offsetValid; //是否需要计算偏差

	unsigned short m_pScale[P_NUMBER];		//ptz的刻度值
	unsigned short m_tScale[T_NUMBER];
	unsigned short m_zScale[Z_NUMBER];
	unsigned short m_pNumber;	
	unsigned short m_tNumber;
	unsigned short m_zNumber;

        unsigned short m_count;
        int m_z[SCALE_COUNT]; //z�ĵȼ�
        float m_tValue[SCALE_COUNT]; //һ��t�ȼ��ڼ������ص�
        float m_pValue[SCALE_COUNT]; //һ��p�ȼ��ڼ������ص�

        float m_offsetX[SCALE_COUNT][SCALE_COUNT];
        float m_offsetY[SCALE_COUNT][SCALE_COUNT];


    } FreeWalkerParam;

    int FreeWalkerInit_1(int channel,int w, int h, char model_path[], char pScale_path[], char tScale_path[], char zScale_path[], char offset_path[]);
   // int FreeWalkerInit_1(int channel, char model_path[], char pScale_path[], char tScale_path[], char zScale_path[], char offset_path[]);
    //int FreeWalkerProc(int channel, float currP, float currT, float currZ, int x1, int y1, int x2, int y2, int zoom, float* outP, float* outT, float* outZ);
 //   int FreeWalkerProc(int channel, unsigned short currP, unsigned short currT, unsigned short currZ, int x1, int y1, int x2, int y2, int zoom, float* outP, float* outT, float* outZ);
    //int FreeWalkerProc(int channel, unsigned short currP, unsigned short currT, unsigned short currZ, int x1, int y1, int x2, int y2, int zoomdif, int* outP, int* outT, int* outZ);
    int FreeWalkerProc(int channel, unsigned short currP, unsigned short currT, unsigned short currZ, int x1, int y1, int x2, int y2, int zoomdif, int* outP, int* outT, int* outZ, int *outWidth, int * outHeight);

    int FreeWalkerInit_test(int channel, int w, int h);
    int Test1(int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4, int xx1, int yy1, int xx2, int yy2, int xx3, int yy3, int xx4, int yy4, int xi, int yi);
    int Test(int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4, int xx1, int yy1, int xx2, int yy2, int xx3, int yy3, int xx4, int yy4, int xi1, int yi1, int xi2, int yi2);

#ifdef	__cplusplus
}
#endif
#endif
