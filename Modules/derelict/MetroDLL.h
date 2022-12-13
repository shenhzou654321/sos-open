#include "CamDetect.h"
#ifndef _METRO_H_
#define _METRO_H_

#ifdef	__cplusplus
extern "C" {
#endif

int vAVS_InitParam(int nChannel,  unsigned char *restrict pbkFrame, int nWidth, int nHeight, vParam *restrict pData);
int vAVS_ProcFrame(int nChannel, unsigned int nFrameNo, unsigned int nTime,  unsigned char *restrict pFrame, int nWidth, int nHeight, vOutData * restrict pOut);
void vAVS_Debug(int nChannel, int k, unsigned char * restrict pFrame, int * restrict nFrameLen, int * restrict pDebugData);

#ifdef	__cplusplus
}
#endif
#endif
