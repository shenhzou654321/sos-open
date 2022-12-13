//////////////////////////////////////////////////////////////////////
#ifndef _TOOL_H_
#define _TOOL_H_



#ifdef	__cplusplus
extern "C" {
#endif
 
typedef struct tag_ITG_RECT {
    int left;
    int right;
    int top;
    int bottom;

} ITG_RECT;

void vPasteY_1(unsigned char* pFrame, unsigned char* pY, int w, int h, int sw, int y1, int y2, int x, int y);
void vPasteY_2(unsigned char* pFrame, unsigned char* pY, int w, int h, int sw, int y1, int y2, int x, int y);
void vPasteY(unsigned char* pFrame, unsigned char* pY, int w, int h, int sw, int y1, int y2, int x, int y) ;
void ITG_DrawLine(unsigned char* yuv_y, int width, int len, int x0, int y0, int x1, int y1) ;
void ITG_DrawRect_YUV(char* yuv_y, char* yuv_u, char* yuv_v, int width, int height, ITG_RECT* region, int y, int cb, int cr, int rate, int LineWidth);
void ITG_DrawRect_y(char* yuv_y, char* yuv_u, char* yuv_v, int width, ITG_RECT* region, int y, int cb, int cr, int rate, int LineWidth);
void ITG_DrawRect(char* yuv_y, int width, ITG_RECT* region, int color);
    
void ITG_DrawLine_y(unsigned char* yuv_y, int width, int len, int x0, int y0, int x1, int y1,int y, int cb, int cr );
void ITG_DrawLine_uv(unsigned char* u,unsigned char* v, int width, int len, int x0, int y0, int x1, int y1,int y, int cb, int cr) ;
void PrepareDir(char dir[]) ;

int getAveGray(unsigned char* py,int w,int h,int l,int r,int t,int b);

int ReadProfileStr(char* pSecName, char* pKeyName, char* pValue, int nSize, char* pFileName);
void MakeLog(char info[], int type);

int getsmall_multiple(int w,int h);

#ifdef	__cplusplus
}
#endif
#endif




