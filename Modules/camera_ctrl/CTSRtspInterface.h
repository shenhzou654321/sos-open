/*
��ý�岥�źͿ��ƽӿ�ͷ�ļ�-��ͨ����
Date:2015-03-31
Author������
Version:1.0.0.0-----���汾֧�����Ӷ��������õ�ÿ��������ź��RGB565ͼ��
remarks
        ��1����ʾRTSPý��ͼ��
                        CTSAddCamera ����һ����ý��ͨ��     Ȼ�����CTSStartViewCamera������ȡ��Ӧ��ý��ͨ������Ƶ    Ȼ���ٻص�������ȥ��ʾ��Ƶ
        ��2�����ݴ��ڴ�С����ͼ���С
                        ͨ��CTSSetParam ���ô��ڵĴ�С �������Ƶ����������ڴ�С
Date:2015-04-14
Author:����
Version:1.0.1.0-----���汾֧�ֶ������̨�Ŀ���
remarks:ͨ��CTSSetParam��������Ķ���

Date:2015-06-17
Author:����
Version:1.0.1.1-----���汾֧�ֶ�D-Link DVS-n3020 �Ŀ���
                    ���������ΪD-link��ʱ����Ҫ����Ԥ����Ƶ���ھ��������CTS_CBKFUN�ص���������Ҫ�Լ���ʵ��Ƶ

Date:2015-06-24
Author:����
Version:1.0.1.2
remarks:ͨ��CTSSetParam��������Ķ���
        ���������Ϣ��ʱ�� ��������������Է�Ϊ:

        ���������    Ԥ��:camip,uid,pwd,camport,previewchan


        �й�ѧ�������ⶥ�������    Ԥ��:camip,uid,pwd,camport,previewchan
                                    ����:controlip,controlport
        D-Link�����Ԥ��:camip,uid,pwd,camport,previewchan,nHwnd(ʵʱ)

        ���ǡ����᣺�ݲ�֧��

        ���������    Ԥ��:rtspurl


Date:2015-07-01
Author:����
Version:1.0.1.3
remarks:�޸�D-Link DVS-N3020�豸Ԥ������--�뺣��������豸����һ��--ͨ���ص�����ͼ��


Date:2015-07-14
Author:����
Version:1.0.1.4
remarks:�޸Ľӿڵ��÷�ʽ--�ⲿ���ò���ʹ�����id����Ϊ�������в���,���޸�Ϊʹ����������ķ��ؾ����Ϊ�������ò���

Date:2015-07-21
Author:����
Version:1.0.1.5
remarks:�޸Ľӿ�--CTSSetParam��CTSGetParam ������Ҫerrorstring����


Date:2015-08-05
Author:����
Version:1.0.1.6
remarks:����ptz��ʱ�����ѡ���Ƿ��Զ�ֹͣptz

Date:2015-08-31
Author:����
Version:1.0.1.7
remarks:�޸�FFmpeg����rtsp ��Dictionary����ֹ���Ӳ���rtsp(��Щ��֧��tcp)

Date:2015-11-10
Author:���
Version:1.0.1.8
remarks:�޸���ƵԤ��-��Ƶ��ʾ��ʱ�����Ƶʵ��ʱ����һ������ʾ

Date:2015-12-23
Author:����
Version:1.0.1.9
remarks:���Ӷ���ý�����������Ƶ��������˵ȿ���ѡ���֧��
*/



#ifndef __PLAY_RTSP_INTERFACE_H__
#define __PLAY_RTSP_INTERFACE_H__

#include "ZGXAPI.h"
typedef unsigned int uint;

//////////////////////////////////////////////////////��//////////////////////////////////////////////////////////
#define CUR_DLL_VERSION            "1.0.1.7"             //��ǰ��̬��汾��Ϣ
#define MAX_CMINFO_LEN             250                   //�����Ϣ�ַ�����󳤶�
#define MAX_CAMERA_PORT            65535                 //ģ������˿ں�
#define MAX_IMG_H                  2048                  //��Ƶ���߶�
#define MAX_IMG_W                  2448                  //��Ƶ�����
#define MAX_FRAME_RATE             25                    //��Ƶ���֡��
#define MAX_PTZ_SPEED              7                     //PTZת������ٶ�
#define MIN_PTZ_SPEED              1                     //PTZת����С�ٶ�

//typedef unsigned int uint;
typedef uint  CTSHANDLE;


//////////////////////////////////////////////////////ö��//////////////////////////////////////////////////////////

//ͼ������
enum IMAGE_TYPE
{
    IMAGE_YUV420,                                        //ͼ��4:1:1YYYYYYYYYYYYYYYYUUUUVVVV
    IMAGE_YUV422,                                        //ͼ��4:2:2YYYYYYYYYYYYYYYYUUUUUUUUVVVVVVVV
    IMAGE_RGB16,                                         //ͼ��RGB565
    IMAGE_RGB24,                                         //ͼ��RGB888
    IMAGE_RGB32                                          //ͼ��RGB888A

};

//�쳣�ص������е��쳣����
enum CTSEXCAP_TYPE
{
    CTSEXCAP_NOT_READY,
    CTSEXCAP_CONNECT_OK,                                 //���ӳɹ�
    CTSEXCAP_CONNECTTIMEOUT,                             //���ӳ�ʱ
    CTSEXCAP_LOSTCONNECT                                 //ʧȥ����
};

typedef CTSEXCAP_TYPE *PCTSEXCAP_TYPE;
//////////////////////////////////////////////////////�ص���ʾ����//////////////////////////////////////////////////////////
typedef    void(*CTS_CBKFUN)(void *pImage, uint nBufSize, uint nImgW, uint nImgH, void *pUser);

typedef    void(*CTS_EXCAPFUN)( CTSEXCAP_TYPE t, void *pUser);


//////////////////////////////////////////////////////�ṹ��//////////////////////////////////////////////////////////
//�����Ϣ
struct CAMERA_INFO
{
    camera_t        t;                                   //�������
    char            szRtspUrl[MAX_CMINFO_LEN];           //rtsp��ַ
    char            szCameraIP[MAX_CMINFO_LEN];          //IP��ַ
    char            szCameraUid[MAX_CMINFO_LEN];         //�û���
    char            szCameraPwd[MAX_CMINFO_LEN];         //����
    char            szControlIP[MAX_CMINFO_LEN];         //����
    char            szCamName[MAX_CMINFO_LEN];           //�������
    uint            nFramerate;                          //ģ�⡢��������Ĳ���֡��0~MAX_FRAME_RATE
    uint            nImageW;                             //���ź��ͼ����--ͨ��ָ�����С0~MAX_IMG_W
    uint            nImageH;                             //���ź��ͼ��߶�--ͨ��ָ�����С0~MAX_IMG_H
    uint            nCameraPort;                         //SOCKET�˿ں�-�й�ѧ���ʹ��
    uint            nControlPort;                        //SOCKET�˿ں�-�й�ѧ���ʹ��
    uint            nHwnd;                               //���ھ��--V1.0.1.4���Ժ�İ汾����ʹ���������
    uint            nPreViewChan;                        //Ԥ��ͨ��ID--��ý������������ж��ͨ��
    uint            nChannelID;                          //���ͨ��ID0~MAX_CHANNEL_NUM--V1.0.1.4���Ժ�İ汾����ʹ���������
    uint            nDBID;                               //���ݿ�ID
    uint            nRes[19];                            //Ԥ���ֽ�
};

//���������Ϣ
struct PTZ_PARAM_INFO
{
    uint            bStop;                               //�Ƿ�ֹͣ���ת��1-ֹͣ 0����������ֹͣ
    uint            nDirectSpeed;                        //����ת���ٶ�--��ΧMIN_PTZ_SPEED~MAX_PTZ_SPEED
    uint            bNeedAutoStop;                       //�Ƿ��Զ�ֹͣ
    uint            nRes[19];                            //Ԥ���ֽ�
};

//////////////////////////////////////////////////////�ӿ�//////////////////////////////////////////////////////////
#endif//__PLAY_RTSP_INTERFACE_H__

