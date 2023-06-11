#ifndef	_V_TRACK_H_
#define _V_TRACK_H_

#define MAX_CAR_H		520	
#define MAX_CAR_W		672 //42*16  

#define TRACK_LEN 32
#define HIST_LEN (TRACK_LEN*4)

#define MAX_OBJ	30

typedef struct TrackCarParam
{
	int bUsed;
	int nProcType;	//0:��ʼ����1:����
	
	int disp;
	int frameIdx;
	
	int il;					//��ʼ��λ��
	int it;
	int ir;
	int ib;
	
	int	track_count;
	int track_error;
	int track_debug_error;
	
	int ol;				//�����ǰ֡λ��
	int ot;
	int oor;
	int ob;
	
	int x2;			//�����һ֡x����λ��
	int y2;			//�����һ֡y�²�λ��
	
	int x3;			//�������һ֡x����λ��
	int y3;			//�������һ֡y�²�λ��	

	int predict_cx;
	int predict_cy;
	int search_width;
	int search_height;
	
}TrackCarParam;

typedef struct STRACKAREA1
{
	int l;
	int r;
	int t;
	int b;

}STRACKAREA1;

typedef struct vTrack1
{
	int		rw;
	int		rh;
	
	int 		histmod[HIST_LEN];
	int 		histsmp[25][HIST_LEN];
	
	int					track_count;
	STRACKAREA1  area_rect[1024];
	int					match_dif[1024];
	int					findex[1024];

	int predict_cx;
	int predict_cy;
	int search_width;
	int search_height;

	int update_ratio;
	int predict_ratio;
	int disappear_point;
	
	TrackCarParam ta;
	
}vTrack1;

typedef struct vCarProc
{
	int		w;
	int		h;

	int		rw;
	int		rh;
	
	//unsigned char simage[MAX_W*MAX_H*2];

	unsigned char rectgray[MAX_CAR_H*MAX_CAR_W/4];
	short		vedge[MAX_CAR_H*MAX_CAR_W/4];
	short		hedge[MAX_CAR_H*MAX_CAR_W/4];
	
	int 		ihe[MAX_CAR_H*MAX_CAR_W/4];	
	int			ihg[MAX_CAR_H*MAX_CAR_W/4];
	int			ive[MAX_CAR_H*MAX_CAR_W/4];
	int			ivg[MAX_CAR_H*MAX_CAR_W/4];
	
	vTrack1		m_track[MAX_OBJ];
	
}vCarProc;


void vTrackcar_create();

int vTrackcar_init(vTrack1 *p, unsigned char *simage, int w, int h);

int vTrackcar_track(vTrack1 *p, unsigned char *simage, int w, int h);
								
int vTrackcar_error(vTrack1 * p,int stopLinePostion,int sh, int *debug_error);								
								

int vTrackcar_proc(unsigned char * image,int w, int h,TrackCarParam * pm,int * debug);


#endif	//_V_TRACK_H_
