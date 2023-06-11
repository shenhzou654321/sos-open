#ifndef __HIKIPCAMERA_H__
#define __HIKIPCAMERA_H__

#include "CTSCamera.h"
#include "HCNetSDK.h"
#include <list>

//using namespace std;
//海康相机������
class CHIKIPCamera :public CIPCamera
{
//海康相机控制端口
#define        DEF_HIK_IPC_PORT                                8000
//默认PTZ控制速度
#define        DEF_HIK_PTZ_SPEED                                    1
//播放解码缓冲
#define    MAX_M4_DEC_BUF_SIZE                        (4*1024*1024)

public:
    CHIKIPCamera();
    ~CHIKIPCamera();
public:
    //连接像机
    virtual bool ConnectCamera();
    //断开连接
    virtual bool DisconnectCamera();
    
protected:
    cts_res_t GotoPtz(float fP,float fT,float fZ);
    
protected:
    LONG                                                            m_nUserID;
    LONG                                                            m_nControlChannel;
    //LONG                                                            m_nRealPlayHandle;
    //LONG                                                          m_nM4Port;
    NET_DVR_DEVICEINFO_V30                                        m_sDeviceInfo;
    //SwsContext                                                   *m_pM4Sws;
	//std::list<void*>                                             m_pavfrmList;  //��������Ƶ֡����
};


#endif
