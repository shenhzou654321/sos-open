/*******************************************************************
* @��Ȩ��Ϣ��
* @�ļ���ƣ�CTSCamera.h
* @ժ    Ҫ��������ͷ�ļ�
* @��    �ߣ�����
* @��ǰ�汾��1.0.0
* @��    �ڣ�2015��4��2��
* @��    ע��ʵ������Ԥ���������ơ������ȡ�Ȳ���
*******************************************************************/

#ifndef  __CTS_CAMERA_H__
#define __CTS_CAMERA_H__

#include "CTSRtspInterface.h"
#include "HCNetSDK.h"
#include "ZGXAPI.h"
#pragma warning (disable:4996)

//���Ӳ��ϵ�ʱ��
#define MAX_NOTCONNECT_TIME    2000

//������ʱ��
#define MAX_RECONNECT_TIME 10000

//����Ƶ���õ�ʱ��
#define    MAX_FUN_FREQ_TIME    200

//PTZ����ʱδִ�н��������ʱ��
#define MAX_PTZSTOP_IMEOUT    500

//����YUVת��������С
#define MAX_YUVSWITCH_DATA_LEN                (MAX_IMG_H*MAX_IMG_W*2)

#define MAX_WAIT_TIME_OUT                             1000              //����ִ�г�ʱʱ�� ����

//��������
enum  TIMEOUT_FUN_TYPE
{
    FUN_SET_PARAM,                                            //��ȡ������
    FUN_GET_PARAM                                             //���ò�����
};



//IP�����
class CIPCamera
{
public:
    CIPCamera();
    virtual ~CIPCamera();
public:
    //���ò��� �����߳�
    virtual void Init(CAMERA_INFO *pInfo);
    //�ͷ���Դ �����߳�
    void Uninit();
    //��ʱ��Ӧ����--ֹͣ��ʾ
//protected:
    //���������ز���-��ͨ��һЩ�������ı������Ϊ
    virtual cts_res_t SetParam(camera_param_t t, void *data);
    //��ȡ������ز���
    virtual cts_res_t GetParam(camera_param_t t, void *data);
    //����������
    virtual bool ConnnectControl();
    //�ͷ�����������
    virtual bool DisConneControl();
    //��rtsp��ַ�л�ȡip��ַ--���������е�ip��Ϊ���ڿ���IP��ʱ�����Ԥ��IP��ַ����Ҫ��rtsp��������
    bool FindIPByRtspUrl();
    //��rtsp��ַ�л�ȡͨ����--���������е�ip��Ϊ���ڿ���IP��ʱ�����Ԥ��IP��ַ����Ҫ��rtsp��������
    bool FindChannelByRtspUrl();
	//ת����Ӧ�ĵص�ȥ
	virtual cts_res_t TurnToPtz(void * pData);
	//��ȡĿ��ʵ��ˮƽƫ�ƽǶ�-180~180
	virtual float GetPAngelOff(ptz_turnto_param_t * pParam);
	//��ȡĿ��ʵ�ʴ�ֱƫ�ƽǶ�-90~90
	virtual float GetTAngelOff(ptz_turnto_param_t * pParam);
	//��ȡĿ��ʵ�ʱ䱶ϵ��
	virtual float GetZAngelOff(ptz_turnto_param_t * pParam);

    CTS_PTZ                        m_ptz;                                                //ʵʱPTZ
private:
	//ת��һ��PTZ���ȥ
	virtual cts_res_t GotoPtz(float fP,float fT,float fZ);
	//��ȡĿ�������PTZ 0���P���
	virtual float GetTargitP(ptz_turnto_param_t * pParam);
	//��ȡĿ�������PTZ 0���T���
	virtual float GetTargitT(ptz_turnto_param_t * pParam);
	//��ȡĿ�������PTZ 0���Z���
	virtual float GetTargitZ(ptz_turnto_param_t * pParam);

protected:
    CAMERA_INFO                   m_Info;                                               //����������Ϣ
    //CTS_PTZ                        m_ptz;                                                //ʵʱPTZ
    char                            m_szMsg[MAX_CMINFO_LEN];                              //������Ϣ�Ĵ�����

    bool                            m_bNeedAutoStopPtz;                                        //�Ƿ���Ҫ�Զ�ֹͣptz
    bool                            m_bInit;                                                   //�Ƿ��г�ʼ�����ù����

	UINT							m_nCameraPort;												 //�������˿ں� ��������������
    uint                            m_nChannelID;                                              //���ͨ��ID0~MAX_CHANNEL_NUM
    uint                            m_nExplosure;                                              //�ع��
    uint                            m_nContrast;                                               //�Աȶ�
    uint                            m_nGain;                                                   //����
    uint                            m_dwTimeBegin;                                             //��ʼʱ��
    uint                            m_dwTimeBeginDec;
    uint                            m_nDecFrmCnt;
    uint                            m_nControlPort;                                           //���ƶ˿�
    uint                            m_nPreviewChannel;                                        //Ԥ��ͨ��--��Ժ������
    uint                            m_nLastParamTime;                                         //��һ�����ò����ʱ��
    char                            m_szRtspUrl[MAX_CMINFO_LEN];                              //rtsp��ַ
    char                            m_szCameraIP[MAX_CMINFO_LEN];                             //IP��ַ
    char                            m_szControlIP[MAX_CMINFO_LEN];                            //IP��ַ
	char							m_szCamName[MAX_CMINFO_LEN];
    char                            m_szCameraUid[MAX_CMINFO_LEN];                            //�û���
    char                            m_szCameraPwd[MAX_CMINFO_LEN];                            //����

    camera_param_t               m_nLastParamKey;                                         //��һ�����ò���ļ�ֵ
};


#endif//__CTS_CAMERA_H__
