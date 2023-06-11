
#ifndef DHCONFIGSDK_H
#define DHCONFIGSDK_H

#include "avglobal.h"
#if (defined(WIN32) || defined(_WIN32) || defined(_WIN64))
    #include <windows.h>

    #ifdef CONFIGSDK_EXPORTS
        #define CLIENT_CFG_API      __declspec(dllexport) 
    #else
        #define CLIENT_CFG_API      __declspec(dllimport)
    #endif

    #define CALLBACK                __stdcall
    #define CALL_METHOD             __stdcall  //__cdecl

    #ifndef LLONG
        #if (defined(WIN32) || defined(_WIN32) || defined(_WIN64))
            #ifdef _WIN64
                #define LLONG       __int64
            #else //WIN32 
                #define LLONG       LONG
            #endif
        #else	//Linux
            #define LLONG           long 
        #endif
    #endif

    #ifndef LDWORD
        #if (defined(WIN32) || defined(_WIN32) || defined(_WIN64))
            #ifdef _WIN64
                #define LDWORD      __int64
            #else //WIN32 
                #define LDWORD      DWORD
            #endif
        #else	//Linux
            #define LDWORD          long 
        #endif
    #endif

#else	//Linux

    #ifndef INTERNAL_COMPILE
        #define CFG_RELEASE_HEADER
    #endif

    #ifndef CFG_RELEASE_HEADER 
        #include "../Platform/platform.h"
    #endif

    #define CLIENT_CFG_API	extern "C"
    #define CALL_METHOD
    #define CALLBACK

#endif


#ifdef __cplusplus
extern "C" {
#endif

/************************************************************************
 ** ��������
 ***********************************************************************/

#define MAX_CHANNEL_COUNT                       16
#define MAX_VIDEO_CHANNEL_NUM                   256             // ���ͨ����256
#define MAX_CHANNELNAME_LEN                     64              // ���ͨ�����Ƴ���
#define MAX_VIDEOSTREAM_NUM                     4               // �����������
#define MAX_VIDEO_COVER_NUM                     16              // ����ڵ��������
#define WEEK_DAY_NUM                            7               // һ�ܵ�����
#define MAX_REC_TSECT                           6               // ¼��ʱ��θ���
#define MAX_REC_TSECT_EX                        10              // ¼��ʱ�����չ����
#define MAX_WATERMARK_LEN                       4096            // ����ˮӡ������󳤶�
#define MAX_MOTION_ROW                          32              // ��̬������������
#define MAX_MOTION_COL                          32              // ��̬������������
#define MAX_IMAGESIZE_NUM                       256             // ���֧�ֵķֱ��ʸ���
#define MAX_FPS_NUM                             1024            // ���֧�ֵ�֡�ʸ���
#define MAX_QUALITY_NUM                         32              // ���֧�ֵĻ��ʸ���
#define MAX_ADDRESS_LEN                         256             // ���ĵ�ַ����
#define MAX_USERNAME_LEN                        64              // ����û�������
#define MAX_PASSWORD_LEN                        64              // ������볤��
#define MAX_DIRECTORY_LEN                       256             // �ļ��������ַ�������
#define MAX_NAS_TIME_SECTION                    2               // ����洢ʱ��θ���
#define MAX_NAME_LEN                            128             // ͨ�������ַ�������
#define	MAX_SCENE_TYPE_LIST_SIZE                8               // �����б������֧�ֵĳ�������
#define MAX_DECPRO_LIST_SIZE                    100             // ������Э���б��������
#define MAX_SCENE_LIST_SIZE                     32              // ��Ƶ�����豸֧�ֵĳ��������б��������
#define MAX_OBJECT_LIST_SIZE                    16              // ��Ƶ�����豸֧�ֵļ�����������б��������
#define MAX_RULE_LIST_SIZE                      128             // ��Ƶ�����豸֧�ֵĹ����б��������
#define	MAX_SUPPORTED_COMP_SIZE                 4               // ���֧�ֵĳ��������
#define	MAX_SUPPORTED_COMP_DATA                 8               // ÿ������������֧�ֵĳ�������
#define MAX_ANALYSE_MODULE_NUM                  16              // ��Ƶ�����豸�����ģ�����
#define MAX_ANALYSE_RULE_NUM                    32              // ��Ƶ�����豸���������
#define MAX_POLYGON_NUM                         20              // ��Ƶ�����豸���򶥵��������
#define MAX_POLYLINE_NUM                        20              // ��Ƶ�����豸���߶����������
#define MAX_TEMPLATEREGION_NUM                  32              // ��Ƶ�����豸ģ��������Ϣ��Ը�������
#define POINT_PAIR_NUM                          2               // ��Ƶ�����豸ģ�������԰����ĵ����
#define MAX_VEHICLE_SIZE_LIST                   4               // ��Ƶ�����豸������С��������
#define MAX_VEHICLE_TYPE_LIST                   4               // ��Ƶ�����豸�������͸�������
#define MAX_PLATE_TYPE_LIST                     32              // ��Ƶ�����豸�������͸�������
#define MAX_LANE_NUM                            8               // ��Ƶ�����豸ÿ��ͨ����Ӧ����������
#define MAX_STAFF_NUM                           20              // ��Ƶ�����豸ÿ��ͨ����Ӧ�ı��������
#define MAX_CALIBRATEAREA_NUM                   20              // ��Ƶ�����豸�궨���������
#define MAX_EXCLUDEREGION_NUM                   10              // ���ܷ��������������Ҫ�ų�������������� 
#define MAX_CALIBRATEBOX_NUM                    10              // ���ܷ���У׼���������
#define MAX_SPECIALDETECT_NUM                   10              // ���ܷ�����������������
#define MAX_HUMANFACE_LIST_SIZE                 8               // ��Ƶ�����豸֧�ֵ�������������б��������
#define MAX_FEATURE_LIST_SIZE					32				// ��Ƶ�����豸֧�ֵ����������б��������
#define	MAX_SEVER_NUM                           16              // ������������
#define MAX_SERVER_NAME_LEN                     16              // ���������ַ�����С
#define MAX_POWER_NUM                           8               // ��Դ��������
#define MAX_FUN_NUM                             8               // ���ȸ�������
#define MAX_CPU_NUM                             8               // cpu��������
#define MAX_HARDDISK_NUM                        32              // Ӳ������
#define MAX_TANK_NUM                            16              // ���洢������
#define MAX_CHAN_NUM                            256             // ���ͨ��������
#define MAX_RAID_NUM                            16              // ��������������
#define MAX_DEV_NUM                             16              // ����豸����
#define MAX_STORAGEPOOL_NUM                     16              // ���洢������
#define MAX_STRORAGEPOS_NUM                     16              // ���洢λ������
#define	MAX_VIDEODEV_NUM                        256             // ǰ���豸����
#define MAX_REMOTEDEVICENAME_LEN                32              // ���Զ���豸���ֳ���
#define	MAX_REMOTE_DEV_NUM                      256             // ���Զ���豸����
#define MAX_PLATEHINT_NUM                       8               // �����ַ���ʾ��������
#define MAX_LIGHT_NUM                           8               // ��ͨ�Ƹ�������
#define MAX_LIGHTGROUP_NUM                      8               // ��ͨ�����������
#define MAX_LIGHT_TYPE                          8               // ��ͨ����������
#define MAX_LIGHT_DIRECTION                     8               // ��ͨ��ָʾ����������
#define MAX_TRIGGERMODE_NUM                     32              // ��ͨ·�ڹ��򴥷�ģʽ���� 
#define MAX_VIOLATIONCODE                       16              // ���ܽ�ͨΥ�´��볤������
#define MAX_DETECTOR                            6               // ���ܽ�ͨ��������������
#define MAX_COILCONFIG                          3               // ���ܽ�ͨ��������Ȧ��������
#define MAX_DEVICE_ADDRESS                      256             // TrafficSnapshot���ܽ�ͨ�豸��ַ
#define MAX_DEPARTMENT                          256             // Department���ܽ�ͨ�豸������λ
#define MAX_ROADWAYNO                           128             // ��·���	��32�����ֺ���ĸ����
#define MAX_VIOLATIONCODE_DESCRIPT              64              // ���ܽ�ͨΥ�´��볤������
#define MAX_DRIVINGDIRECTION                    256             // ��ʻ�����ַ�������
#define MAX_ACTIVEUSER_NUM                      64              // ����û���Ϣ��
#define MAX_POLYGON_NUM10                       10              // ��Ƶ�����豸���򶥵��������
#define MAX_VIDEODIAGNOSIS_DETECT_TYPE          11              // ��Ƶ������͸�������
#define MAX_ACTION_LIST_SIZE                    16              // ��Ƶ�����豸֧�ֵĹ���Ķ��������б��������
#define MAX_STORAGEGROUPNAME_LEN                32              // �洢�����ƻ���������
#define MAX_CALIBRATEAREA_TYPE_NUM              4               // �궨������������
#define MAX_PROTOCOL_NAME_LEN                   32              // Э�����Ƴ���
#define	MAX_COMM_NUM                            16              // ��󴮿�����
#define MAX_DNS_SERVER_NUM                      2               // DNS�������
#define MAX_NETWORK_INTERFACE_NUM               32              // �����������
#define	MAX_NAS_NUM                             16              // ����洢�������������
#define MAX_STORAGEPOINT_NUM                    32              // ¼��洢��ӳ���������
#define MAX_TRACKSCENE_NUM                      10              // ���ܸ��ٳ����������
#define MAX_STATUS_NUM                          16              // ��ͨ�豸״̬������
#define MAX_SERVICE_NUM                         128             // ������֧�ֵ���������
#define MAX_DBKEY_NUM                           64              // ���ݿ�ؼ������ֵ
#define MAX_SUMMARY_LEN                         1024            // ���ӵ�JPEGͼƬ��ժҪ��Ϣ��󳤶�
#define MAX_MOTION_WINDOW                       10              // ����֧�ֵ���Ƶ����ֵ
#define MAX_OSD_SUMMARY_LEN                     256             // osd����������󳤶�    
#define MAX_OSD_TITLE_LEN                       128             // osd���ӱ�����󳤶�
#define MAX_CUSTOMCASE_NUM                      16              // �Զ���˾������������
#define MAX_GLOBAL_MSTERSLAVE_NUM               64              //����ʽ���������ȫ��������
#define MAX_OBJECT_ATTRIBUTES_SIZE              16              // ��Ƶ�����豸֧�ֵļ���������������б��������
#define MAX_MODEL_LEN                           32              // �豸�ͺų���
#define MAX_BURNING_DEV_NUM                     32              // ����¼�豸����
#define MAX_NET_TYPE_NUM                        8               // ����������͸���
#define MAX_NET_TYPE_LEN                        64              // ���������ַ������� 
#define MAX_DEVICE_NAME_LEN                     64              // ��������
#define MAX_DEV_ID_LEN_EX                       128             // �豸ID��󳤶�
#define MONTH_OF_YEAR                           12              // һ�����·���
#define MAX_SERVER_NUM                          10              // ������������
#define MAX_REGISTER_NUM                        10              // ����ע������������ 
#define MAX_VIDEO_IN_ZOOM                       32              // ��ͨ�����������ø���
#define	MAX_ANALYSE_SCENE_NUM                   32              // ��Ƶ����ȫ�����ó����������
#define	MAX_CONFIG_NUM                          32              // ÿ����̨�����������
#define	MAX_PTZ_PRESET_NAME_LEN                 64              // ��̨Ԥ�õ����Ƴ���
#define CFG_COMMON_STRING_8                     8               // ͨ���ַ�������8
#define CFG_COMMON_STRING_16                    16              // ͨ���ַ�������16
#define CFG_COMMON_STRING_32                    32              // ͨ���ַ�������32
#define CFG_COMMON_STRING_64                    64              // ͨ���ַ�������64
#define CFG_COMMON_STRING_128                   128             // ͨ���ַ�������128
#define CFG_COMMON_STRING_256                   256             // ͨ���ַ�������256
#define CFG_COMMON_STRING_512                   512             // ͨ���ַ�������512
#define AV_CFG_Channel_Name_Len                 64              // ͨ�����Ƴ���
#define CFG_MAX_CHANNEL_NAME_LEN                256             // ͨ��������󳤶�    
#define AV_CFG_Weekday_Num                      7               // һ������
#define AV_CFG_Max_TimeSection                  6               // ʱ�������
#define AV_CFG_Device_ID_Len                    64              // �豸ID����
#define AV_CFG_IP_Address_Len                   32              // ip����
#define AV_CFG_IP_Address_Len_EX                40              // ��չIP��ַ�ַ�������, ֧��IPV6
#define AV_CFG_User_Name_Len                    64              // �û�������
#define AV_CFG_Password_Len                     64              // ���볤��
#define AV_CFG_Protocol_Len                     32              // Э��������
#define AV_CFG_Serial_Len                       32              // ���кų���
#define AV_CFG_Device_Class_Len                 16              // �豸���ͳ���
#define AV_CFG_Device_Type_Len                  32              // �豸�����ͺų���
#define AV_CFG_Device_Name_Len                  128             // ��������
#define AV_CFG_Address_Len                      128             // ��������ص�
#define AV_CFG_Max_Path                         260             // ·������
#define AV_CFG_Max_Split_Window                 128             // ���ָ������
#define	AV_CFG_Monitor_Favorite_In_Channel      64              // ÿ�����ͨ���������ѵ�����ղ�����
#define AV_CFG_Monitor_Favorite_Name_Len        64              // �����ղ����Ƴ���
#define AV_CFG_Max_Monitor_Favorite_Window      64              // �����ղص���󴰿�����
#define AV_CFG_Max_Split_Group                  64              // �ָ�����������
#define AV_CFG_Max_Split_Mode                   32              // �ָ�ģʽ�������
#define AV_CFG_Raid_Name_Len                    64              // RAID���Ƴ���
#define AV_CFG_Max_Rail_Member                  32              // ����RAID����������
#define AV_CFG_Max_Encode_Main_Format           3               // ����������������
#define AV_CFG_Max_Encode_Extra_Format          3               // ����������������
#define	AV_CFG_Max_Encode_Snap_Format           3               // ץͼ����������
#define AV_CFG_Max_VideoColor                   24              // ÿ��ͨ�������Ƶ������ɫ��������
#define AV_CFG_Custom_Title_Len                 1024            // �Զ���������Ƴ���(���䵽1024)
#define AV_CFG_Custom_TitleType_Len             32              // �Զ���������ͳ���
#define AV_CFG_Max_Video_Widget_Cover           16              // �������򸲸��������
#define AV_CFG_Max_Video_Widget_Custom_Title    8               // ��������Զ�������������
#define AV_CFG_Max_Video_Widget_Sensor_Info     2               // ����������Ӵ�������Ϣ�������Ŀ
#define AV_CFG_Max_Description_Num              4               // ��������������Ϣ��������
#define AV_CFG_Group_Name_Len                   64              // �������Ƴ���
#define AV_CFG_DeviceNo_Len                     32              // �豸��ų���
#define AV_CFG_Group_Memo_Len                   128             // ����˵������
#define AV_CFG_Max_Channel_Num                  1024            // ���ͨ������
#define AV_CFG_Time_Format_Len                  32              // ʱ���ʽ����
#define AV_CFG_Max_White_List                   1024            // ����������
#define AV_CFG_Max_Black_List                   1024            // ����������
#define AV_CFG_Filter_IP_Len                    96              // ����IP��󳤶�
#define AV_CFG_Max_ChannelRule                  32              // ͨ���洢������󳤶�, ��ͨ������
#define AV_CFG_Max_DBKey_Num                    64              // �¼��ؼ�������
#define AV_CFG_DBKey_Len                        32              // �¼��ؼ��ֳ���
#define AV_CFG_Max_Summary_Len                  1024            // ժҪ����
#define AV_CFG_Max_Event_Title_Num              32              // �¼������������
#define AV_CFG_Max_Tour_Link_Num                128             // ������Ѳ�������
#define AV_CFG_PIP_BASE                         1000            // ���л��ָ�ģʽ����ֵ
#define DES_KEY_LEN                             8               // DES��Կ���ֽڳ���
#define DES_KEY_NUM                             3               // 3DES��Կ�ĸ���
#define AES_KEY_LEN                             32              // AES��Կ���ֽڳ���
#define MAX_TIME_SCHEDULE_NUM                   8               // ʱ���Ԫ�ظ���
#define MAX_SCENE_SUBTYPE_LEN                   64              // �����������ַ�������
#define MAX_SCENE_SUBTYPE_NUM                   32              // ����������������    
#define MAX_VIDEO_IN_FOCUS                      32              // ��ͨ�����۽����ø���
#define MAX_TIMESPEEDLIMIT_NUM                  16              // ���ʱ����������ø���
#define MAX_VOICEALERT_NUM                      64              // ���������ʾ���ø���
#define CFG_MAX_LOWER_MATRIX_NUM                16              // �����λ��������
#define CFG_MAX_LOWER_MATRIX_INPUT              64              // �����λ��������ͨ����
#define CFG_MAX_LOWER_MATRIX_OUTPUT             32              // �����λ�������ͨ����
#define CFG_MAX_AUDIO_MATRIX_INPUT              32              // ��Ƶ�����������ͨ����
#define CFG_MAX_AUDIO_OUTPUT_CHN                32              // ��Ƶ����������ͨ����
#define CFG_MAX_AUDIO_MATRIX_NUM                4               // �����Ƶ��������
#define CFG_MAX_AUDIO_MATRIX_OUTPUT             8               // ÿ����Ƶ����֧�ֵ�������ͨ����
#define CFG_MAX_VIDEO_IN_DEFOG                  3               // ÿ��ͨ�����͸�����ø���
#define CFG_MAX_INFRARED_BOARD_TEMPLATE_NUM     16              // ���������ģ������
#define	CFG_MAX_INFRARED_KEY_NUM                128             // ��������尴������
#define	CFG_MAX_INFRARED_BOARD_NUM              16              // �������������
#define CFG_MAX_VTO_NUM                         128             // ����ſڻ�����
#define MAX_PHONE_NUMBER_LEN                    32              // �绰������󳤶�
#define MAX_AUDIO_OUTPUT_NUM                    16              // ��Ƶ������ͨ����
#define MAX_AUDIO_INPUT_NUM                     32              // ��Ƶ�������ͨ����
#define MAX_LIGHT_GLOBAL_NUM					16				// �ֳ�״̬�������Ŀ
#define MAX_AUDIO_MIX_NUM                       16              // �����Ƶ���ͨ����
#define MAX_PSTN_SERVER_NUM                     8               // ��󱨾��绰��������
#define MAX_ALARM_CHANNEL_NUM                   32              // ��󱨾�ͨ����
#define MAX_ALARM_DEFENCE_TYPE_NUM              8               // ��󱨾�����������
#define MAX_ALARM_SENSE_METHOD_NUM              16              // ��󱨾���Ӧ����ʽ��
#define MAX_EXALARMBOX_PROTOCOL_NUM             8               // ���֧����չ������Э����
#define MAX_EXALARM_CHANNEL_NUM                 256             // ��󱨾�ͨ����
#define MAX_EXALARMBOX_NUM                      8               // ��󱨾�������
#define MAX_MAILTITLE_LEN                       256             // ����ʼ����ⳤ��
#define MAX_DEVICE_ID_LEN                       48              // ����豸���볤��
#define MAX_DEVICE_MARK_LEN                     64              // ����豸��������
#define MAX_BRAND_NAME_LEN                      64              // ����豸Ʒ�Ƴ���
#define MAX_ADDRESS_NUM                         16              // ��󴮿ڵ�ַ����
#define MAX_AIRCONDITION_NUM                    16              // ���յ��豸����
#define CFG_MAX_COLLECTION_NUM                  64              // ���Ԥ����
#define MAX_FLOOR_NUM                           128             // ���¥����
#define MAX_SEAT_NUM                            8               // �����λ��
#define AV_CFG_Local_Device_ID                  "Local"         // �����豸ID
#define AV_CFG_Remote_Devce_ID                  "Remote"        // Զ���豸ID   
#define MAX_LANE_CONFIG_NUMBER                  32              // ����������
#define MAX_PRIORITY_NUMBER                     256             // Υ�����ȼ�����Υ��������
#define MAX_CATEGORY_TYPE_NUMBER                128             // �����������
#define MAX_TRIGGER_MODE_NUMBER                 64              // ����ģʽ����
#define MAX_ABNORMAL_DETECT_TYPE                32              // �쳣���������
#define	MAX_ABNORMAL_THRESHOLD_LEN              32              // �쳣�����ֵ������
#define TS_POINT_NUM                            3               // ������У׼����
#define CFG_FILTER_IP_LEN                       96              // ����IP��󳤶�
#define CFG_MAX_TRUST_LIST                      1024            // ����������
#define CFG_MAX_BANNED_LIST                     1024            // ����������
#define VIDEOIN_TSEC_NUM                        3               // VideoIn ϵ��Э��ʱ��θ�����Ŀǰ����ͨ�����졢��ҹ����
#define	MAX_RECT_COUNT                          4               // ����ͨ��֧�ֵ�����������������
#define CFG_MAX_SSID_LEN                        36              // SSID��󳤶�
#define	MAX_OUTAUDIO_CHANNEL_COUNT              16              // �����Ƶ���ͨ����
#define MAX_INAUDIO_CHANNEL_COUNT               32              // �����Ƶ����ͨ����
#define MAX_FREQUENCY_COUNT                     16              // ���Ƶ�ʶθ���
#define MAX_NTP_SERVER                          4               // �����NTP��������ַ
#define MAX_ACCESS_TEXTDISPLAY_LEN              32              // ����Ž�������ʾ���ֳ���
#define CFG_MAX_NVR_ENCRYPT_COUNT               4               // ÿ��ͨ�����������ø���
#define MAX_IP_ADDR_LEN                         16              // IP��ַ�ַ�������
#define MAX_PRIVACY_MASKING_COUNT               64              // ����ͨ����˽�ڵ����ø���
#define MAX_ALL_SNAP_CAR_COUNT					32				// ���г���բ�������
#define CFG_MAX_PLATE_NUMBER_LEN				32				// ����ƺ��볤��
#define CFG_MAX_SN_LEN							32				// ����豸���кų���
#define CFG_MAX_ACCESS_CONTROL_ADDRESS_LEN		64				// ���ĵ�ַ����
#define MAX_CFG_APN_NAME						32				// Wireless�н�������������
#define	MAX_CFG_DAIL_NUMBER						32				// Wireless�в��ź��볤��
#define	MAX_GROUP_ID_LEN						64				// ��󲼿���ID����
#define	MAX_COLOR_NAME_LEN						32				// �����ɫ������
#define	MAX_COLOR_HEX_LEN						8				// ���HEX��ɫ����
#define	MAX_LINK_GROUP_NUM						20				// �����Ĳ������������
#define MAX_CROWD_DISTRI_MAP_REGION_POINT_NUM	20				// �������������
#define MAX_PEOPLESTATREGIONS_NUM				8				// ����ͳ���������
#define MAX_AUDIOCHANNELS_ARRAY_NUM             8               // ��Ƶͨ���İ���ͨ�����б������������
#define MAX_NUMBER_STAT_MAULT_NUM				32				// ��������ͳ�Ƴ���PD����
#define CFG_MAX_DAY_NIGHT_MODE					3				// ÿ����Ƶ����ͨ����Ӧ�������(�ֱ��ʾ���졢��ҹ����ͨ)
/************************************************************************
 ** �������� ��ӦCLIENT_GetNewDevConfig��CLIENT_SetNewDevConfig�ӿ�
 ***********************************************************************/

#define CFG_CMD_ENCODE                          "Encode"                    // ͼ��ͨ����������(��Ӧ CFG_ENCODE_INFO)
#define CFG_CMD_RECORD                          "Record"                    // ��ʱ¼������(��Ӧ CFG_RECORD_INFO)
#define CFG_CMD_ALARMINPUT                      "Alarm"                     // �ⲿ���뱨������(��Ӧ CFG_ALARMIN_INFO)
#define CFG_CMD_NETALARMINPUT                   "NetAlarm"                  // ���籨������(��Ӧ CFG_NETALARMIN_INFO)
#define CFG_CMD_MOTIONDETECT                    "MotionDetect"              // ��̬��ⱨ������(��Ӧ CFG_MOTION_INFO)
#define CFG_CMD_VIDEOLOST                       "LossDetect"                // ��Ƶ��ʧ��������(��Ӧ CFG_VIDEOLOST_INFO)
#define CFG_CMD_VIDEOBLIND                      "BlindDetect"               // ��Ƶ�ڵ���������(��Ӧ CFG_SHELTER_INFO)
#define CFG_CMD_STORAGENOEXIST                  "StorageNotExist"           // �޴洢�豸��������(��Ӧ CFG_STORAGENOEXIST_INFO)
#define CFG_CMD_STORAGEFAILURE                  "StorageFailure"            // �洢�豸���ʳ���������(��Ӧ CFG_STORAGEFAILURE_INFO)
#define CFG_CMD_STORAGELOWSAPCE                 "StorageLowSpace"           // �洢�豸�ռ䲻�㱨������(��Ӧ CFG_STORAGELOWSAPCE_INFO)
#define CFG_CMD_NETABORT                        "NetAbort"                  // ����Ͽ���������(��Ӧ CFG_NETABORT_INFO)	
#define CFG_CMD_IPCONFLICT                      "IPConflict"                // IP��ͻ��������(��Ӧ CFG_IPCONFLICT_INFO)
#define CFG_CMD_SNAPCAPINFO                     "SnapInfo"                  // ץͼ������ѯ(��Ӧ CFG_SNAPCAPINFO_INFO)
#define CFG_CMD_NAS                             "NAS"                       // ����洢����������(��Ӧ CFG_NAS_INFO)
#define CFG_CMD_PTZ                             "Ptz"                       // ��̨����(��Ӧ CFG_PTZ_INFO)
#define	CFG_CMD_PTZ_AUTO_MOVEMENT               "PtzAutoMovement"           // ��̨��ʱ��������(��Ӧ CFG_PTZ_AUTOMOVE_INFO)
#define CFG_CMD_WATERMARK                       "WaterMark"                 // ��Ƶˮӡ����(��Ӧ CFG_WATERMARK_INFO)
#define CFG_CMD_ANALYSEGLOBAL                   "VideoAnalyseGlobal"        // ��Ƶ����ȫ������(��Ӧ CFG_ANALYSEGLOBAL_INFO)
#define CFG_CMD_ANALYSEMODULE                   "VideoAnalyseModule"        // ����ļ��ģ������(��Ӧ CFG_ANALYSEMODULES_INFO)
#define CFG_CMD_ANALYSERULE                     "VideoAnalyseRule"          // ��Ƶ������������(��Ӧ CFG_ANALYSERULES_INFO)
#define CFG_CMD_ANALYSESOURCE                   "VideoAnalyseSource"        // ��Ƶ������Դ����(��Ӧ CFG_ANALYSESOURCE_INFO)
#define CFG_CMD_RAINBRUSH                       "RainBrush"                 // ��ˢ����(��Ӧ CFG_RAINBRUSH_INFO)
#define CFG_CMD_INTELLECTIVETRAFFIC             "TrafficSnapshot"           // ���ܽ�ͨץ��(CFG_TRAFFICSNAPSHOT_INFO ֻΪ������Э�飻��ʹ��CFG_CMD_TRAFFICSNAPSHOT_MULTI)
#define CFG_CMD_TRAFFICGLOBAL                   "TrafficGlobal"             // ���ܽ�ͨȫ������(CFG_TRAFFICGLOBAL_INFO)
#define CFG_CMD_DEV_GENERRAL                    "General"                   // ��ͨ���� (��Ӧ CFG_DEV_DISPOSITION_INFO)
#define CFG_CMD_ATMMOTION                       "FetchMoneyOverTime"        // ATMȡ�ʱ����(��Ӧ CFG_ATMMOTION_INFO)
#define CFG_CMD_DEVICESTATUS                    "DeviceStatus"              // �豸״̬��Ϣ(��Ӧ CFG_DEVICESTATUS_INFO)
#define CFG_CMD_HARDDISKTANK                    "HardDiskTank"              // ��չ����Ϣ(��Ӧ CFG_HARDISKTANKGROUP_INFO)
#define CFG_CMD_RAIDGROUP                       "RaidGroup"                 // Raid����Ϣ(��Ӧ CFG_RAIDGROUP_INFO)
#define CFG_CMD_STORAGEPOOLGROUP                "StoragePoolGroup"          // �洢������Ϣ(��Ӧ CFG_STORAGEPOOLGROUP_INFO)
#define CFG_CMD_STORAGEPOSITIONGROUP            "StoragePositionGroup"      // �ļ�ϵͳ����Ϣ(��Ӧ CFG_STORAGEPOSITIONGROUP_INFO)
#define CFG_CMD_VIDEOINDEVGROUP                 "VideoInDevGroup"           // ǰ���豸����Ϣ(��Ӧ CFG_VIDEOINDEVGROUP_INFO)
#define CFG_CMD_DEVRECORDGROUP                  "DevRecordGroup"            // ͨ��¼����״̬(��Ӧ CFG_DEVRECORDGROUP_INFO)
#define CFG_CMD_IPSSERVER                       "IpsServer"                 // ����״̬(��Ӧ CFG_IPSERVER_STATUS)
#define CFG_CMD_SNAPSOURCE                      "SnapSource"                // ץͼԴ����(��Ӧ CFG_SNAPSOURCE_INFO)
#define CFG_CMD_DHRADER                         "DahuaRadar"                // ���״����ã�͸�� json ����
#define CFG_CMD_DHRADER_PP                      "DahuaRadar"                // ���״����ã�����Ϊ�ṹ�壬��Ӧ CFG_DAHUA_RADAR��
#define CFG_CMD_TRANSRADER                      "TransRadar"                // �����״�����
#define CFG_CMD_LANDUNRADER                     "LanDunRadar"               // �����״�����
#define CFG_CMD_LANDUNCOILS                     "LanDunCoils"               // ������Ȧ����
#define CFG_CMD_MATRIX_SPOT                     "SpotMatrix"                // Spot��������(��Ӧ CFG_VIDEO_MATRIX)
#define CFG_CMD_HDVR_DSP                        "DspEncodeCap"              // HDVR��ÿ������ͨ����dsp��Ϣ ����ͨ��������IPC��DVR Ҳ����IPC��DVR������(��ӦCFG_DSPENCODECAP_INFO)
#define CFG_CMD_HDVR_ATTR_CFG                   "SystemAttr"                // HDVR��ÿ������ͨ���������豸����Ϣ
#define CFG_CMD_CHANNEL_HOLIDAY                 "HolidaySchedule"           // ����¼��ƻ�(��Ӧ�ṹ�� CFG_HOLIDAY_SCHEDULE����)
#define CFG_CMD_HEALTH_MAIL                     "HealthMail"                // �����ʼ�
#define CFG_CMD_CAMERAMOVE                      "IntelliMoveDetect"         // �������λ������� 
#define CFG_CMD_SPLITTOUR                       "SplitTour"                 // ��Ƶ�ָ���Ѳ����(��Ӧ CFG_VIDEO_MATRIX)
#define CFG_CMD_VIDEOENCODEROI                  "VideoEncodeROI"            // ��Ƶ����ROI(Region of Intrest)����
#define CFG_CMD_VIDEO_INMETERING                "VideoInMetering"           // �������(��Ӧ CFG_VIDEO_INMETERING_INFO)
#define CFG_CMD_TRAFFIC_FLOWSTAT                "TrafficFlowStat"           // ��ͨ����ͳ������(��Ӧ CFG_TRAFFIC_FLOWSTAT_INFO)
#define CFG_CMD_HDMIMATRIX                      "HDMIMatrix"                // HDMI��Ƶ��������
#define CFG_CMD_VIDEOINOPTIONS	                "VideoInOptions"            // ��Ƶ����ǰ��ѡ��(��Ӧ CFG_VIDEO_IN_OPTIONS)
#define CFG_CMD_RTSP                            "RTSP"                      // RTSP������( ��Ӧ CFG_RTSP_INFO_IN��CFG_RTSP_INFO_OUT )
#define CFG_CMD_TRAFFICSNAPSHOT                 "TrafficSnapshotNew"        // ���ܽ�ͨץ��(CFG_TRAFFICSNAPSHOT_INFO �ϳ�����ʹ�� CFG_CMD_TRAFFICSNAPSHOT_MULTI_EX)
#define CFG_CMD_TRAFFICSNAPSHOT_MULTI           "TrafficSnapshotNew"        // ���ܽ�ͨץ��(CFG_TRAFFICSNAPSHOT_NEW_INFO �ϳ�����ʹ�� CFG_CMD_TRAFFICSNAPSHOT_MULTI_EX)
#define CFG_CMD_TRAFFICSNAPSHOT_MULTI_EX        "TrafficSnapshotNew"        // ���ܽ�ͨץ��(CFG_TRAFFICSNAPSHOT_NEW_EX_INFO)
#define CFG_CMD_MULTICAST                       "Multicast"                 // �鲥���������(��Ӧ CFG_MULTICASTS_INFO_IN��CFG_MULTICASTS_INFO_OUT)
#define CFG_CMD_VIDEODIAGNOSIS_PROFILE          "VideoDiagnosisProfile"     // ��Ƶ��ϲ�����(CFG_VIDEODIAGNOSIS_PROFILE)
#define CFG_CMD_VIDEODIAGNOSIS_TASK             "VideoDiagnosisTask"        // ��Ƶ��������(CFG_VIDEODIAGNOSIS_TASK)
#define CFG_CMD_VIDEODIAGNOSIS_PROJECT          "VideoDiagnosisProject"     // ��Ƶ��ϼƻ���(CFG_VIDEODIAGNOSIS_PROJECT)
#define CFG_CMD_VIDEODIAGNOSIS_REALPROJECT      "VideoDiagnosisRealProject" // ��Ƶ���ʵʱ�ƻ���(CFG_VIDEODIAGNOSIS_REALPROJECT)
#define CFG_CMD_VIDEODIAGNOSIS_GLOBAL           "VideoDiagnosisGlobal"      // ��Ƶ���ȫ�ֱ�(CFG_VIDEODIAGNOSIS_GLOBAL)
#define CFG_CMD_VIDEODIAGNOSIS_TASK_ONE         "VideoDiagnosisTask.x"      // ��Ƶ��������(CFG_VIDEODIAGNOSIS_TASK)
#define CFG_CMD_TRAFFIC_WORKSTATE               "WorkState"                 // �豸����״̬�������(��Ӧ CFG_TRAFFIC_WORKSTATE_INFO)
#define CFG_CMD_STORAGEDEVGROUP                 "StorageDevGroup"           // ���̴洢������(��Ӧ CFG_STORAGEGROUP_INFO)
#define CFG_CMD_RECORDTOGROUP                   "RecordToGroup"             // ¼���ŵĴ洢������(��Ӧ CFG_RECORDTOGROUP_INFO)
#define CFG_CMD_INTELLITRACKSCENE               "IntelliTrackScene"         // ���ܸ��ٳ�������(CFG_INTELLITRACKSCENE_INFO) 
#define CFG_CMD_IVSFRAM_RULE                    "IVSFramRule"               // ����֡��������(��Ӧ CFG_ANALYSERULES_INFO)
#define CFG_CMD_RECORD_STORAGEPOINT             "RecordStoragePoint"        // ¼��洢��ӳ������(��Ӧ CFG_RECORDTOSTORAGEPOINT_INFO)
#define CFG_CMD_RECORD_STORAGEPOINT_EX			"RecordStoragePoint"		// ¼��洢��ӳ��������չ (��Ӧ CFG_RECORDTOSTORAGEPOINT_EX_INFO)
#define CFG_CMD_MD_SERVER                       "MetaDataServer"            // Ԫ���ݷ���������(��Ӧ CFG_METADATA_SERVER�ṹ��)
#define CFG_CMD_CHANNELTITLE                    "ChannelTitle"              // ͨ������(��Ӧ AV_CFG_ChannelName)
#define	CFG_CMD_RECORDMODE                      "RecordMode"                // ¼��ģʽ(��Ӧ AV_CFG_RecordMode)
#define CFG_CMD_VIDEOOUT                        "VideoOut"                  // ��Ƶ�������(��Ӧ AV_CFG_VideoOutAttr)
#define CFG_CMD_REMOTEDEVICE                    "RemoteDevice"              // Զ���豸��Ϣ(��Ӧ AV_CFG_RemoteDevice����, ͨ���޹�)
#define CFG_CMD_REMOTECHANNEL                   "RemoteChannel"             // Զ��ͨ��(��Ӧ AV_CFG_RemoteChannel)
#define CFG_CMD_MONITORTOUR                     "MonitorTour"               // ������ѵ����(��Ӧ AV_CFG_MonitorTour)
#define CFG_CMD_MONITORCOLLECTION               "MonitorCollection"         // �����ղ�����(��Ӧ AV_CFG_MonitorCollection)
#define CFG_CMD_DISPLAYSOURCE                   "DisplaySource"             // ����ָ���ʾԴ����(��Ӧ AV_CFG_ChannelDisplaySource)(�ϳ���������ʹ��)
#define CFG_CMD_RAID                            "Raid"                      // �洢��������(��Ӧ AV_CFG_Raid����, ͨ���޹�)
#define CFG_CMD_RECORDSOURCE                    "RecordSource"              // ¼��Դ����(��Ӧ AV_CFG_RecordSource)
#define CFG_CMD_VIDEOCOLOR                      "VideoColor"                // ��Ƶ������ɫ����(��Ӧ AV_CFG_ChannelVideoColor)
#define CFG_CMD_VIDEOWIDGET                     "VideoWidget"               // ��Ƶ�����������(��Ӧ AV_CFG_VideoWidget)
#define CFG_CMD_STORAGEGROUP                    "StorageGroup"              // �洢����Ϣ(��Ӧ AV_CFG_StorageGroup����, ͨ���޹�)
#define CFG_CMD_LOCALS                          "Locales"                   // ��������(��Ӧ AV_CFG_Locales)
#define CFG_CMD_LANGUAGE                        "Language"                  // ����ѡ��(��Ӧ AV_CFG_Language)
#define CFG_CMD_ACCESSFILTER                    "AccessFilter"              // ���ʵ�ַ����(��Ӧ AV_CFG_AccessFilter)
#define CFG_CMD_AUTOMAINTAIN                    "AutoMaintain"              // �Զ�ά��(��Ӧ AV_CFG_AutoMaintain)
#define CFG_CMD_REMOTEEVENT                     "RemoteEvent"               // Զ���豸�¼�����(��Ӧ AV_CFG_RemoteEvent����)
#define CFG_CMD_MONITORWALL                     "MonitorWall"               // ����ǽ����(��Ӧ AV_CFG_MonitorWall����, ͨ���޹�)
#define	CFG_CMD_SPLICESCREEN                    "VideoOutputComposite"      // �ں�������(��Ӧ AV_CFG_SpliceScreen����, ͨ���޹�)
#define CFG_CMD_TEMPERATUREALARM                "TemperatureAlarm"          // �¶ȱ�������(��Ӧ AV_CFG_TemperatureAlarm, ͨ���޹�)
#define	CFG_CMD_FANSPEEDALARM                   "FanSpeedAlarm"             // ����ת�ٱ�������(��Ӧ AV_CFG_FanSpeedAlarm, ͨ���޹�)
#define CFG_CMD_RECORDBACKUP                    "RecordBackupRestore"       // ¼��ش�����(��Ӧ AV_CFG_RecordBackup, ͨ���޹�)
#define CFG_CMD_RECORDDOWNLOADSPEED             "RecordDownloadSpeed"       // ¼�������ٶ�����(��ӦCFG_RecordDownloadSpeed)
#define CFG_CMD_COMM                            "Comm"                      // ��������(��Ӧ CFG_COMMGROUP_INFO)
#define	CFG_CMD_NETWORK                         "Network"                   // ��������(��Ӧ CFG_NETWORK_INFO)
#define CFG_CMD_NASEX                           "NAS"                       // ����洢����������, �������(��Ӧ CFG_NAS_INFO_EX)
#define CFG_CMD_LDAP                            "LDAP"                      // LDAP����
#define CFG_CMD_ACTIVE_DIR                      "ActiveDirectory"           // �Ŀ¼����
#define CFG_CMD_FLASH                           "FlashLight"                // ���������(��Ӧ CFG_FLASH_LIGHT)
#define CFG_CMD_AUDIO_ANALYSERULE               "AudioAnalyseRule"          // ��Ƶ������������(��Ӧ CFG_ANALYSERULES_INFO)
#define CFG_CMD_JUDICATURE                      "Judicature"                // ˾����¼����(��Ӧ CFG_JUDICATURE_INFO)
#define CFG_CMD_GOODS_WEIGHT                    "CQDTSet"                   // ���ػ�������(��Ӧ CFG_GOOD_WEIGHT_INFO)
#define CFG_CMD_VIDEOIN                         "VideoIn"                   // ����ͨ������(��Ӧ CFG_VIDEO_IN_INFO)
#define CFG_CMD_ENCODEPLAN                      "EncodePlan"                // ��¼���̱���ƻ�(��Ӧ CFG_ENCODE_PLAN_INFO)
#define CFG_CMD_PICINPIC                        "PicInPic"                  // ˾����Ѷ���л�(��Ӧ CFG_PICINPIC_INFO)��Ϊ���鷽ʽ��������ǰ�������ã����ݳ�������
#define CFG_CMD_BURNFULL                        "BurnFull"                  // ��¼���¼�����(��Ӧ CFG_BURNFULL_INFO)
#define CFG_CMD_MASTERSLAVE_GLOBAL              "MasterSlaveTrackerGlobal"  // ����ʽȫ������(��Ӧ CFG_MASTERSLAVE_GLOBAL_INFO)
#define CFG_CMD_MASTERSLAVE_LINKAGE             "MasterSlaveGlobal"         // ǹ������ȫ������(��Ӧ CFG_MASTERSLAVE_LINKAGE_INFO)
#define CFG_CMD_MASTERSLAVE_GROUP               "MasterSlaveGroup"          // ǹ�������󶨹�ϵ����(��Ӧ CFG_MASTERSLAVE_GROUP_INFO)
#define CFG_CMD_ANALYSEWHOLE                    "VideoAnalyseWhole"         // ��Ƶ������������(��Ӧ CFG_ANALYSEWHOLE_INFO)
#define CFG_CMD_VIDEO_IN_BOUNDARY               "VideoInBoundary"           // ��Ƶ����߽�����(��Ӧ CFG_VIDEO_IN_BOUNDARY)
#define	CFG_CMD_MONITORWALL_COLLECTION          "MonitorWallCollection"     // ����ǽԤ��(��Ӧ CFG_MONITORWALL_COLLECTION����)
#define CFG_CMD_ANALOGMATRIX                    "AnalogMatrix"              // ģ�����(��Ӧ CFG_ANALOG_MATRIX_INFO)
#define CFG_CMD_ANALOG_MATRIX_PROTOCOL          "AnalogMatrixProtocol"      // ģ�����Э������(��Ӧ CFG_ANALOG_MATRIX_PROTOCOL����)
#define	CFG_CMD_VIDEO_OUT_TITLE                 "VideoOutputTitle"          // ��Ƶ�������(��Ӧ CFG_VIDEO_OUT_TITLE)
#define CFG_CMD_DISK_FLUX_ALARM                 "DiskFluxAlarm"             // Ӳ������������������(��Ӧ CFG_DISK_FLUX_INFO)
#define CFG_CMD_NET_FLUX_ALARM                  "NetFluxAlarm"              // ��������������������(��Ӧ CFG_NET_FLUX_INFO)
#define CFG_CMD_DVRIP                           "DVRIP"                     // ����Э������(��Ӧ CFG_DVRIP_INFO)
#define CFG_CMD_PARKINGSPACE_SCREEN             "ParkingSpaceScreen"        // ������Խӵ�����������(��Ӧ CFG_PARKINGSPACE_SCREEN_INFO ,�����飬Ԫ�����ֵ��DH_PRODUCTION_DEFNITION.nMaxParkingSpaceScreen)
#define CFG_CMD_PARKINGSPACE_STATUS             "ParkingSpaceCellStatus"    // ��λ����(ר�г�λ����ͨ��λ����)(��Ӧ CFG_PARKINGSPACE_STATUS_INFO �������飬Ԫ�����ֵ��DH_PRODUCTION_DEFNITION.nMaxRoadWays)
#define CFG_CMD_CLIENT_CUSTOM_DATA              "ClientCustomData"          // ƽ̨�Զ�����Ϣ(��Ӧ CFG_CLIENT_CUSTOM_INFO)
#define CFG_CMD_BURN_RECORD_FORMAT              "BurnRecordFormat"          // ��¼��ʽ����(��Ӧ CFG_BURN_RECORD_FORMAT)
#define CFG_CMD_MULTIBURN                       "MultiBurn"                 // �����ͬ����¼(��Ӧ CFG_MULTIBURN_INFO)��һ�����飬ÿһ��Ԫ�ر�ʾһ������ͬ����¼��У��
#define	CFG_CMD_ENCODE_ENCRYPT                  "EncodeEncrypt"             // �������������Ϣ( CFG_ENCODE_ENCRYPT_CHN_INFO )
#define CFG_CMD_VIDEO_IN_ZOOM                   "VideoInZoom"               // ��̨ͨ���䱶����(��Ӧ CFG_VIDEO_IN_ZOOM)
#define CFG_CMD_SNAP                            "Snap"                      // ץͼ����(��Ӧ CFG_SNAP_INFO)
#define CFG_CMD_REMOTE_STORAGE_LIMIT            "RemoteStorageLimit"        // ����洢��������������(��Ӧ CFG_REMOTE_STORAGELIMIT_GROUP)
#define CFG_CMD_SPECIAL_DIR                     "SpecialDirectoryDefine"    // ������;Ŀ¼����(��Ӧ CFG_SPECIAL_DIR_INFO)
#define CFG_CMD_AUTO_STARTUP_DELAY              "AutoStartupDelay"          // �ػ�����ʱ�Զ���������(��Ӧ CFG_AUTO_STARTUP_DELAY_INFO)
#define CFG_CMD_CANFILTER                       "CANFilter"                 // CAN͸������(��Ӧ CFG_CANFILTER_LIST)
#define CFG_CMD_VIDEOIN_FOCUS                   "VideoInFocus"              // �۽�����(��Ӧ CFG_VIDEO_IN_FOCUS)
#define CFG_CMD_ENCODE_ADAPT                    "EncodeAdapt"               // ��������Ӧ����(��Ӧ CFG_ENCODE_ADAPT_INFO)
#define CFG_CMD_VIDEOANALYSE_CALIBRATE          "VideoAnalyseCalibrate"     // ��Ƶ�����궨����(��Ӧ CFG_VIDEO_ANALYSE_CALIBRATEAREA)
#define CFG_CMD_PTZ_PRESET                      "PtzPreset"                 // ��̨Ԥ�õ�����(��Ӧ�ṹ PTZ_PRESET_INFO)
#define CFG_CMD_TIMESPEEDLIMIT                  "TimeSpeedLimit"            // ʱ�������ֵ����(��Ӧ�ṹ�� CFG_TIMESPEEDLIMIT_LIST)
#define CFG_CMD_VOICEALERT                      "VoiceAlert"                // ������ʾ����(��Ӧ�ṹ�� CFG_VOICEALERT_LIST)
#define CFG_CMD_DEVICEKEEPALIVE                 "DeviceKeepAlive"           // �豸��������(��Ӧ�ṹ�� CFG_DEVICEKEEPALIVELIST)

#define CFG_CMD_AUDIO_SPIRIT                    "AudioSpirit"               // ��������(��Ӧ CFG_AUDIO_SPIRIT)
#define CFG_CMD_AUDIO_MATRIX_SILENCE            "AudioMatrixSilence"        // ������������(��Ӧ CFG_AUDIO_MATRIX_SILENCE)
#define CFG_CMD_AUDIO_MATRIX                    "AudioMatrixConfig"         // ��Ƶ��������(��Ӧ CFG_AUDIO_MATRIX)
#define CFG_CMD_COMPOSE_CHANNEL                 "ComposeChannel"            // �ϳ�ͨ������(��Ӧ CFG_COMPOSE_CHANNEL)
#define CFG_CMD_COMPOSE_LINKAGE                 "ComposeLinkage"            // �ϳ�ͨ������  ͥ������ʹ�ã�֤���л�����(��Ӧ CFG_COMPOSE_CHANNEL)
#define CFG_CMD_LOWER_MATRIX                    "LowerMatrix"               // ��λ��������(��Ӧ CFG_LOWER_MATRIX_LIST) 
#define CFG_CMD_INFRARED_BOARD_TEMPLATE	        "InfraredBoardTemplate"	    // �������ģ��(��Ӧ CFG_INFRARED_BOARD_TEMPLATE_GROUP)
#define CFG_CMD_INFRARED_BOARD                  "InfraredBoard"	            // �������ģ��(��Ӧ CFG_INFRARED_BOARD_GROUP)
#define CFG_CMD_VIDEOIN_EXPOSURE                "VideoInExposure"           // �ع�����(��Ӧ CFG_VIDEOIN_EXPOSURE_INFO)
#define CFG_CMD_VIDEOIN_BACKLIGHT               "VideoInBacklight"          // ���߻�������(��ӦCFG_VIDEOIN_BACKLIGHT_INFO)

#define CFG_CMD_ACCESS_GENERAL                  "AccessControlGeneral"      // �Ž���������(��Ӧ CFG_ACCESS_GENERAL_INFO)
#define CFG_CMD_ACCESS_EVENT                    "AccessControl"             // �Ž��¼�����(��Ӧ CFG_ACCESS_EVENT_INFO ����)     
#define CFG_CMD_WIRELESS                        "Wireless"                  // ����������������(��Ӧ CFG_WIRELESS_INFO)
#define CFG_CMD_ALARMSERVER                     "AlarmServer"               // ��������������(��Ӧ CFG_ALARMCENTER_INFO)
#define CFG_CMD_COMMGLOBAL                      "CommGlobal"                // ����ȫ������(��Ӧ CFG_COMMGLOBAL_INFO)
#define CFG_CMD_ANALOGALARM                     "AnalogAlarm"               // ģ��������ͨ������(��Ӧ CFG_ANALOGALARM_INFO)
#define CFG_CMD_ALARMOUT                        "AlarmOut"                  // �������ͨ������(��Ӧ CFG_ALARMOUT_INFO)
#define CFG_CMD_NTP                             "NTP"                       // ʱ��ͬ��������(��Ӧ CFG_NTP_INFO)
#define CFG_CMD_ALARMBELL                       "AlarmBell"                 // ��������(��Ӧ CFG_ALARMBELL_INFO)
#define CFG_CMD_MOBILE                          "Mobile"                    // �ƶ����ҵ������(��Ӧ CFG_MOBILE_INFO)
#define CFG_CMD_PHONEEVENTNOTIFY                "PhoneEventNotify"          // (��Ӧ CFG_PHONEEVENTNOTIFY_INFO)
#define CFG_CMD_PSTN_ALARM_SERVER               "PSTNAlarmServer"           // �绰������������(CFG_PSTN_ALARM_CENTER_INFO)
#define CFG_CMD_AUDIO_OUTPUT_VOLUME             "AudioOutputVolume"         // ��Ƶ�������(CFG_AUDIO_OUTPUT_VOLUME)
#define CFG_CMD_AUDIO_INPUT_VOLUME              "AudioInputVolume"          // ��Ƶ��������(CFG_AUDIO_INPUT_VOLUME)
#define CFG_CMD_LIGHT_GLOBAL					"LightGlobal"				// ָʾ�ƿ������� (CFG_LIGHT_GLOBAL)
#define	CFG_CMD_AUDIO_MIX_VOLUME                "AudioMixVolume"            // �����Ƶ����������ͥ������(CFG_AUDIO_MIX_VOLUME)
#define CFG_CMD_ALARMKEYBOARD                   "AlarmKeyboard"             // ������������(CFG_ALARMKEYBOARD_INFO)
#define CFG_CMD_POWERFAULT                      "PowerFault"                // ��Դ��������(CFG_POWERFAULT_INFO)
#define CFG_CMD_CHASSISINTRUSION                "ChassisIntrusion"          // �������ֱ���(���𱨾�)����(CFG_CHASSISINTRUSION_INFO)
#define CFG_CMD_EXALARMBOX                      "ExAlarmBox"                // ��չ���������� (CFG_EXALARMBOX_INFO)
#define CFG_CMD_EXALARMOUTPUT                   "ExAlarmOut"                // ��չ�����������(CFG_EXALARMOUTPUT_INFO)
#define CFG_CMD_EXALARMINPUT                    "ExAlarm"                   // ��չ������������(CFG_EXALARMINPUT_INFO)
#define CFG_CMD_ACCESSTIMESCHEDULE              "AccessTimeSchedule"        // �Ž�ˢ��ʱ���(CFG_ACCESS_TIMESCHEDULE_INFO)
#define CFG_CMD_URGENCY                         "Emergency"                 // �����¼�����(CFG_URGENCY_INFO)
#define CFG_CMD_SENSORSAMPLING                  "SensorSampling"            // ����������(CFG_SENSORSAMPLING_INFO)
#define CFG_CMD_STP                             "STP"                       // ��������(CFG_STP_INFO)
#define CFG_CMD_ALARM_SUBSYSTEM                 "AlarmSubSystem"            // ������ϵͳ����(CFG_ALARM_SUBSYSTEM_INFO)
#define CFG_CMD_BATTERY_LOW_POWER               "BatteryLowPowerAlarm"      // ��ص���������(CFG_BATTERY_LOW_POWER_INFO)
#define CFG_CMD_SNAPLIKAGE                      "SnapLinkage"               // ץͼͨ��������������(CFG_SNAPLINKAGE_INFO)
#define CFG_CMD_AUDIOINPUT                      "AudioInput"                // ��Ƶ��������(CFG_AUDIO_INPUT)
#define CFG_CMD_EMAIL                           "Email"                     // �ʼ���������(CFG_EMAIL_INFO)
#define CFG_CMD_TRAFFIC_TRANSFER_OFFLINE        "TrafficTransferOffline"    // ���������ļ�����(TRAFFIC_TRANSFER_OFFLINE_INFO)
#define CFG_CMD_COMMSUBSCRIBE                   "CommSubscribe"             // ���Ĵ�����������(CFG_DEVCOMM_SUBSCRIBE)
#define CFG_CMD_PARKINGSPACE_LIGHT_STATE        "ParkingSpaceLightState"    // ��λ״̬��Ӧ�ĳ�λָʾ��(CFG_PARKINGSPACE_LIGHT_STATE)
#define CFG_CMD_AIRCONDITION                    "AirCondition"              // �յ��豸����(CFG_AIRCONDITION_INFO)
#define CFG_CMD_COMPRESS_PLAY                   "CompressPlay"              // ѹ���ط�����(CFG_COMPRESS_PLAY_INFO)
#define CFG_CMD_BUILDING                        "Building"                  // VTO¥������(CFG_BUILDING_INFO)
#define CFG_CMD_BUILDING_EXTERNAL               "BuildingExternal"          // VTO¥����չ����(CFG_BUILDING_EXTERNAL_INFO)
#define CFG_CMD_DIALRULE                        "DialRule"                  // ���Ź���(CFG_DIALRULE_INFO)
#define CFG_CMD_OIL_MASS_INFO                   "OilMassInfo"               // ������������(CFG_OIL_MASS_INFO)
#define CFG_CMD_FISHEYE_INFO                    "FishEye"                   // ������ϸ��Ϣ����(CFG_FISHEYE_DETAIL_INFO)
#define CFG_CMD_VTNOANSWER_FORWARD              "VTNoAnswerForward"         // ƽ̨�·�������Ӧ��ת�������б�(CFG_VT_NOANSWER_FORWARD_INFO)
#define CFG_CMD_VTO_CALL                        "VTOCall"                   // VTO��������(CFG_VTO_CALL_INFO)
#define CFG_CMD_MACCONFLICT                     "MacConflict"               // MAC��ͻ��������(CFG_MACCONFLICT_INFO)
#define CFG_CMD_IDLEMOTION_INFO                 "IdleMotion"                // ���ж�������(CFG_IDLE_MOTION_INFO)
#define CFG_CMD_MONITORWALL_COLL_TOUR           "MonitorWallCollectionTour" // ����ǽԤ����Ѳ����(CFG_MONITORWALL_COLLECTION_TOUR_INFO)
#define CFG_CMD_PSTN_BREAK_LINE                 "PSTNBreakLine"             // PSTN�����¼�����(CFG_PSTN_BREAK_LINE_INFO)
#define CFG_CMD_NET_COLLECTION                  "NetCollection"             // ����ɼ��豸����(CFG_NET_COLLECTION_INFO)
#define CFG_CMD_ALARM_SLOT_BOND                 "AlarmSlotBond"             // ����Slot�ڵ�����������豸�Ķ�Ӧ��ϵ(CFG_ALARM_SLOT_BOND_INFO)
#define CFG_CMD_TRAFFICSTROBE                   "TrafficStrobe"             // ��բ����(CFG_TRAFFICSTROBE_INFO)
#define CFG_CMD_TRAFFICVOICE                    "TrafficVoiceBroadcast"     // ���ܽ�ͨ������������( CFG_TRAFFICVOICE_BROADCAST)
#define CFG_CMD_STANDING_TIME                   "StandingTime"              // ͣ��ʱ������(CFG_STANDING_TIME_INFO)
#define CFG_CMD_ENCLOSURE_TIME_SCHEDULE         "EnclosureTimeSchedule"     // ����Χ������ʱ�������(CFG_ENCLOSURE_TIME_SCHEDULE_INFO)
#define CFG_CMD_ECKCONFIG                       "ECKConfig"                 // ͣ��������ڿ���������(CFG_ECKCONFIG_INFO)
#define CFG_CMD_PARKING_CARD                    "ParkingCard"               // ͣ���������ˢ�������¼�����(CFG_PARKING_CARD_INFO)
#define CFG_CMD_RCEMERGENCY_CALL                "RCEmergencyCall"           // �������б����¼�����(CFG_RCEMERGENCY_CALL_INFO)
#define CFG_CMD_LANES_STATE_REPORT              "LanesStateReport"          // ������Ϣ�ϱ�����(CFG_LANES_STATE_REPORT)
#define CFG_CMD_OPEN_DOOR_GROUP                 "OpenDoorGroup"             // ���˶࿪�ŷ�ʽ�������(CFG_OPEN_DOOR_GROUP_INFO)
#define CFG_CMD_OPEN_DOOR_ROUTE                 "OpenDoorRoute"             // ����·�߼��ϣ���Ʒ���Ǳ·������(CFG_OPEN_DOOR_ROUTE_INFO)
#define CFG_CMD_BURNPLAN                        "BurnPlan"                  // ��¼�ƻ�����(��Ӧ CFG_BURNPLAN_INFO)
#define CFG_CMD_SCADA_DEV                       "SCADADev"                  // ���ɼ��豸����(CFG_SCADA_DEV_INFO)
#define CFG_CMD_VSP_GAYS                        "VSP_GAYS"                  // ����һ��ƽ̨��������(CFG_VSP_GAYS_INFO)
#define CFG_CMD_AUDIODETECT                     "AudioDetect"               // ��Ƶ��ⱨ������(CFG_AUDIO_DETECT_INFO����)
#define CFG_CMD_GUIDESCREEN                     "GuideScreen"               // �յ���ϵͳ����(CFG_GUIDESCREEN_INFO)
#define CFG_CMD_VTS_CALL_INFO                   "VTSCallInfo"               // VTS��������(CFG_VTS_CALL_INFO)
#define CFG_CMD_DEV_LIST                        "DevList"                   // �豸�б�����(CFG_DEV_LIST_INFO)
#define CFG_CMD_CALIBRATE_MATRIX                "CalibrateMatrix"           // ����ʽ�������궨��������(CFG_CALIBRATE_MATRIX_INFO, �����ö�ӦCFG_CALIBRATE_MATRIX_EX_INFO)
#define CFG_CMD_DEFENCE_AREA_DELAY              "DefenceAreaDelay"          // ������ʱ����(CFG_DEFENCE_AREA_DELAY_INFO)
#define CFG_CMD_THERMO_GRAPHY                   "ThermographyOptions"       // �ȳ�������ͷ��������(CFG_THERMOGRAPHY_INFO)
#define CFG_CMD_THERMOMETRY_RULE                "ThermometryRule"           // �ȳ�����¹�������(CFG_RADIOMETRY_RULE_INFO)
#define CFG_CMD_TEMP_STATISTICS                 "TemperatureStatistics"     // �¶�ͳ������(CFG_TEMP_STATISTICS_INFO)
#define CFG_CMD_THERMOMETRY                     "HeatImagingThermometry"    // �ȳ������ȫ������(CFG_THERMOMETRY_INFO)
#define CFG_CMD_FLAT_FIELD_CORRECTION           "FlatFieldCorrection"       // �ȳ���ƽ���۽�У׼����(CFG_FLAT_FIELD_CORRECTION_INFO)
#define CFG_CMD_THERMO_FUSION                   "ThermographyFusion"        // �ȳ�����Ƶ�ں�����(CFG_THERMO_FUSION_INFO)
#define CFG_CMD_LCE_STATE                       "LCEState"                  // �ȳ������л�о������ͷ���Եľֲ��Աȶ���ǿ����(CFG_LCE_STATE_INFO)
#define CFG_CMD_LIGHTING                        "Lighting"                  // �ƹ�����(CFG_LIGHTING_INFO)
#define CFG_CMD_RAINBRUSHMODE                   "RainBrushMode"             // ��ˢģʽ�������(��Ӧ CFG_RAINBRUSHMODE_INFO����)
#define CFG_CMD_LIGHTINGSCHEDULE                "LightingSchedule"          // �ƹ�ƻ�����(��Ӧ CFG_LIGHTINGSCHEDULE_INFO)
#define CFG_CMD_EMERGENCY_RECORD_FOR_PULL       "EmergencyRecordForPull"    // ����¼��洢���ã����ڿͻ����������ķ�ʽ���ڿͻ��������洢�쳣֮�󣬽��н���¼��洢(CFG_EMERGENCY_RECORD_FOR_PULL_INFO)
#define CFG_CMD_ALARM_SHIELD_RULE               "AlarmShieldRule"           // �澯���ι���( CFG_ALARM_SHIELD_RULE_INFO)
#define CFG_CMD_VIDEOIN_ANALYSERULE             "VideoInAnalyseRule"        // ��Ƶͨ�����ܹ�������(CFG_VIDEOIN_ANALYSE_RULE_INFO)
#define CFG_CMD_ACCESS_WORK_MODE                "AccessWorkMode"            // ��������ģʽ(��Ӧ CFG_ACCESS_WORK_MODE_INFO ����)
#define CFG_CMD_VIDEO_TALK_PHONE_GENERAL        "VideoTalkPhoneGeneral"     // ��Ƶ�Խ��绰ͨ������(CFG_VIDEO_TALK_PHONE_GENERAL)
#define CFG_CMD_TRAFFIC_SNAP_MOSAIC             "TrafficSnapMosaic"         // ץͼ�ϳ�����(��Ӧ CFG_TRAFFIC_SNAP_MOSAIC_INFO)
#define CFG_CMD_SCENE_SNAP_RULE                 "SceneSnapShotWithRule"     // ����ץ������(��Ӧ CFG_SCENE_SNAPSHOT_RULE_INFO)
#define CFG_CMD_PTZTOUR                         "PtzTour"                   // ��̨Ѳ��·������(��Ӧ CFG_PTZTOUR_INFO)
#define CFG_CMD_VTO_INFO                        "VTOInfo"                   // �ſڻ�����(��Ӧ CFG_VTO_LIST)
#define CFG_CMD_TS_POINT                        "TSPoint"                   // ������У׼����(��Ӧ CFG_TSPOINT_INFO)
#define CFG_CMD_VTH_NUMBER_INFO                 "VTHNumberInfo"             // ���ڻ�������Ϣ(��Ӧ CFG_VTH_NUMBER_INFO)
#define CFG_CMD_GPS                             "GPS"                       // GPS����(��Ӧ CFG_GPS_INFO_ALL)
#define CFG_CMD_VTO_BASIC_INFO                  "VTOBasicInfo"              // VTO������Ϣ (��Ӧ CFG_VTO_BASIC_INFO)
#define CFG_CMD_SHORTCUT_CALL                   "ShortcutCall"              // ��ݺ����� (��Ӧ CFG_SHORTCUT_CALL_INFO)
#define CFG_CMD_GPS_LOCATION_VER                "GPSLocationVersion"        // ��¼��GPSLocation�İ汾��(��Ӧ CFG_LOCATION_VER_INFO)
#define CFG_CMD_PARKING_SPACE_ACCESS_FILTER     "ParkingSpaceAccessFilter"  // �豸�ɷ��ʵ�ַ��������(��Ӧ CFG_PARKING_SPACE_ACCESS_FILTER_INFO)
#define CFG_CMD_WORK_TIME                       "WorkTime"                  // ����ʱ������(��Ӧ CFG_WORK_TIME_INFO)
#define CFG_CMD_PARKING_SPACE_LIGHT_GROUP       "ParkingSpaceLightGroup"    // ��λָʾ�Ʊ�������(��Ӧ CFG_PARKING_SPACE_LIGHT_GROUP_INFO_ALL)
#define CFG_CMD_CUSTOM_AUDIO                    "CustomAudio"               // �Զ�����Ƶ����(CFG_CUSTOM_AUDIO)
#define CFG_CMD_WIFI_SEARCH                     "AroudWifiSearch"           // �豸ͨ��wifiģ��ɨ����Χ�����豸����(��Ӧ CFG_WIFI_SEARCH_INFO)
#define CFG_CMD_G3G4AUTOCHANGE                  "G3G4AutoChange"            // �����豸ͨ��ģ��3G 4G�Զ��л�(CFG_G3G4AUTOCHANGE)
#define CFG_CMD_CHECKCODE                       "CheckCode"                 // ˢ����У����У������(��Ӧ CFG_CHECKCODE_INFO)
#define CFG_CMD_VSP_SCYDKD                      "VSP_SCYDKD"                // �Ĵ��ƶ���������ƽ̨��������(��Ӧ CFG_VSP_SCYDKD_INFO)
#define CFG_CMD_VIDEOIN_DAYNIGHT                "VideoInDayNight"           // �����о��ҹ����(��Ӧ CFG_VIDEOIN_DAYNIGHT_INFO)
#define CFG_CMD_PTZ_POWERUP                     "PowerUp"                   // ��̨������������(��Ӧ CFG_PTZ_POWERUP_INFO)
#define CFG_CMD_AUDIO_MIX_CHANNEL               "AudioMixChannel"           // ���ö���ÿ������Ƶͨ�������(��Ӧ CFG_AUDIO_MIX_CHANNEL_INFO_ALL)
#define CFG_CMD_AUDIO_TOUCH                     "AudioTouch"                // ���������������б仯(��Ӧ CFG_AUDIO_TOUCH_INFO_ALL)
#define	CFG_CMD_VIDEO_MOSAIC                    "VideoMosaic"               // �����˵�������(��ӦCFG_VIDEO_MOSAIC_INFO)
#define CFG_CMD_VTH_REMOTE_IPC_INFO             "VTHRemoteIPCInfo"          // VTH��Զ��IPC����(��Ӧ CFG_VTH_REMOTE_IPC_INFO)����������ȫ�ֵģ�������ͨ��
#define CFG_CMD_UNFOCUSDETECT                   "UnFocusDetect"             // �齹�������(��ӦCFG_UNFOCUSDETECT_INFO)
#define	CFG_CMD_MOVE_DETECT                     "MovedDetect"               // ��������������(��ӦCFG_MOVE_DETECT_INFO)
#define CFG_CMD_FLOODLIGHT                      "Floodlight"                // �����������ƿ�������(��Ӧ CFG_FLOODLIGHT_CONTROLMODE_INFO)
#define CFG_CMD_AIRFAN                          "AirFan"                    // �����շ��ȿ�������(��Ӧ CFG_AIRFAN_CONTROLMODE_INFO)
#define CFG_CMD_WLAN                            "WLan"                      // WLAN����(��Ӧ CFG_NETAPP_WLAN)
#define CFG_CMD_SMART_ENCODE                    "SmartEncode"               // Smart H264���뷽ʽ(��Ӧ CFG_SMART_ENCODE_INFO)
#define CFG_CMD_VEHICLE_HIGH_SPEED              "HighSpeed"                 // ���ظ��ٱ�������(��Ӧ CFG_VEHICLE_HIGHSPEED_INFO )
#define CFG_CMD_VEHICLE_LOW_SPEED               "LowSpeed"                  // ���ص��ٱ�������(��Ӧ CFG_VEHICLE_LOWSPEED_INFO )
#define CFG_CMD_PSTN_PERSON_SERVER              "PSTNPersonServer"          // ���˵绰�ӻ�����(��Ӧ CFG_PSTN_PERSON_SERVER_INFO_ALL )
#define CFG_CMD_ARM_LINK                        "ArmLink"                   // ��������������(��Ӧ CFG_ARMLINK_INFO )
#define CFG_CMD_CABINLED_TIME_SCHEDULE          "CabinLedTimeSchedule"      // ������Led��ʾ�ƻ�����(��Ӧ CFG_CABINLED_TIME_SCHEDULE)
#define CFG_CMD_PSTN_TESTPLAN                   "PSTNTestPlan"              // PSTN ���Լƻ�����(��Ӧ CFG_PSTN_TESTPLAN_INFO)
#define CFG_CMD_DEFENCE_ARMMODE                 "DefenceArmMode"            // ������������ʹ������(��Ӧ CFG_DEFENCE_ARMMODE_INFO)
#define CFG_CMD_SENSORMODE                      "SensorMode"                // ̽������װ����ģʽ����(��Ӧ CFG_SENSORMODE_INFO)
#define CFG_CMD_ALARMLAMP                       "AlarmLamp"                 // ��������(��Ӧ CFG_ALARMLAMP_INFO)
#define CFG_CMD_RADAR_SPEED_MEASURE             "RadarSpeedMeasure"         // �״�������� ����¥��ר��(��Ӧ CFG_RADAR_SPEED_MEASURE_INFO)
#define CFG_CMD_VIDEOINDEFOG                    "VideoInDefog"              // ͸����������(��Ӧ�ṹ�� CFG_VIDEOINDEFOG_LIST)
#define CFG_CMD_RTMP                            "RTMP"                      // RTMP����(��Ӧ�ṹ�� CFG_RTMP_INFO)
#define CFG_CMD_AUDIO_OUT_EQUALIZER             "AudioOutEqualizer"         // ��Ƶ�������������(��Ӧ�ṹ��CFG_AUDIO_OUTEQUALIZER_INFO)
#define CFG_CMD_AUDIO_OUT_SUPPRESSION           "AudioOutSuppression"       // ��Ƶ��������(��Ӧ�ṹ��CFG_AUDIO_SUPPRESSION_INFO)
#define CFG_CMD_AUDIO_IN_CONTROL                "AudioInControl"            // ��Ƶ�������(��Ӧ�ṹ��CFG_AUDIO_INCONTROL_INFO)
#define CFG_CMD_LASER_DIST_MEASURE              "LaserDistMeasure"          // ������������(��Ӧ�ṹ�� CFG_LASER_DIST_MEASURE_INFO)
#define CFG_CMD_OIL_4G_OVERFLOW                 "Oil4GFlow"                 // ��ɽ����4G������ֵ��ģʽ����(��Ӧ�ṹ�� CFG_OIL_4G_OVERFLOW_INFO)
#define CFG_CMD_OIL_VIDEOWIDGET_4G_FLOW         "VideoWidget4GFlow"         // ��ɽ����4G����OSD��������(��Ӧ�ṹ�� CFG_OIL_VIDEOWIDGET_4G_FLOW_INFO)
#define CFG_CMD_ATMOSPHERE_OSD                  "AtmosphereOSD"             // ������Ϣ��������(��Ӧ�ṹ�� CFG_CMD_ATMOSPHERE_OSD_INFO)
#define CFG_CMD_PARK_SPACE_OUT_LIGHT            "ParkSpaceOutLight"         // ���ָʾ������(��Ӧ�ṹ�� CFG_PARK_SPACE_OUT_LIGHT_INFO)
#define CFD_CMD_VTO_CALL_INFO_EXTEND            "VTOCallInfo"               // VTO����������չ(��Ӧ�ṹ��CFG_VTO_CALL_INFO_EXTEND)
#define CFG_CMD_ACCESS_TEXTDISPLAY              "AccessControlTextDisplay"  // �Ž�������ʾ��ʾ����(��Ӧ�ṹ�� CFG_ACCESS_TEXTDISPLAY_INFO)
#define	CFG_CMD_NETNVR_ENCRYPT                  "NvrEncrypt"                // ��ʦ����Ƶ������Ŀ������Ϣ���漰IPC��NVR( CFG_NETNVR_ENCRYPT_INFO )
#define CFG_CMD_LIGHT                           "Light"                     // �ƹ��豸���� (��Ӧ�ṹ�� CFG_LIGHT_INFO)
#define CFG_CMD_CURTAIN                         "Curtain"                   // ��������(��Ӧ�ṹ�� CFG_CURTAIN_INFO)
#define CFG_CMD_FRESH_AIR                       "FreshAir"                  // �·�����(��Ӧ�ṹ�� CFG_FRESH_AIR_INFO)
#define CFG_CMD_GROUND_HEAT                     "GroundHeat"                // ��ů����(��Ӧ�ṹ�� CFG_GROUND_HEAT_INFO)
#define CFG_CMD_SCENE_MODE                      "SceneMode"                 // �龰ģʽ(��Ӧ�ṹ�� CFG_SCENE_MODE_INFO)
#define CFG_CMD_AIO_APP_CONFIG                  "AIOAppConfig"              // �山�ǻ�������������(��Ӧ�ṹ�� CFG_AIO_APP_CONFIG_INFO)
#define CFG_CMD_HTTPS                           "Https"                     // Https��������(��Ӧ�ṹ�� CFG_HTTPS_INFO)
#define CFG_CMD_NETAUTOADAPTORENCODE            "NetAutoAdaptEncode"        // ��������Ӧ��������(��Ӧ�ṹ�� CFG_NET_AUTO_ADAPT_ENCODE)
#define CFG_CMD_USERLOCKALARM                   "UserLockAlarm"             // ��½��������(��Ӧ�ṹ�� CFG_USERLOCKALARM_INFO)
#define CFG_CMD_STROBOSCOPIC_LAMP               "StroboscopicLamp"          // Ƶ��������(��Ӧ�ṹ�� CFG_STROBOSCOPIC_LAMP_INFO)
#define CFG_CMD_FREECOMBINATION                 "FreeCombination"           // ���ɷָ�ģʽ�Ĵ������� CFG_FREECOMBINATION_INFO )
#define CFG_CMD_IOT_INFRARED_DETECT             "IOT_InfraredDetect"        // ����������������(��Ӧ�ṹ��CFG_IOT_INFRARED_DETECT_INFO)
#define CFG_CMD_IOT_RECORD_HANDLE               "IOT_RecordHandle"          // ������¼����������(��Ӧ�ṹ��CFG_IOT_RECORD_HANDLE_INFO)
#define CFG_CMD_IOT_SNAP_HANDLE                 "IOT_SnapHandle"            // ������ץͼ��������(��Ӧ�ṹ��CFG_IOT_SNAP_HANDLE_INFO)
#define CFG_CMD_PLATFORM_MONITOR_IPC            "PlatformMonitorIPC"        // ƽ̨�����IPC���� CFG_PLATFORMMONITORIPC_INFO )
#define CFG_CMD_CALLFORWARD                     "CallForward"               // ����ת������(��Ӧ�ṹ�� CFG_CALLFORWARD_INFO)
#define CFD_CMD_DOORBELLSOUND                   "DoorBellSound"             // ��������(��Ӧ�ṹ��CFG_DOOR_BELLSOUND_INFO)
#define CFG_CMD_TELNET                          "Telnet"                    // telnet����(��Ӧ�ṹ��CFG_TELNET_INFO)
#define CFG_CMD_OSDSYSABNORMAL                  "OSDSysAbnormal"            // ��ʾ�쳣��Ϣ�ĵ�������(��Ӧ�ṹ�� CFG_OSD_SYSABNORMAL_INFO)
#define CFG_CMD_VIDEO_WIDGET2                   "VideoWidget2"              // ��Ƶ�����������(��Ӧ�ṹ�� CFG_VIDEO_WIDGET2_INFO)
#define CFG_CMD_VIDEOWIDGET_NUMBERSTAT          "VideoWidgetNumberStat"     // ����ͳ�Ƶ���OSD����(��Ӧ�ṹ�� CFG_VIDEOWIDGET_NUMBERSTAT_INFO)
#define CFG_CMD_PRIVACY_MASKING	                "PrivacyMasking"            // ��˽�ڵ�����(��Ӧ�ṹ�� CFG_PRIVACY_MASKING_INFO)
#define CFG_CMD_DEVICE_INFO                     "DeviceInfo"                // �豸��Ϣ(��Ӧ�ṹ�� CFG_DEVICE_INFO)
#define CFG_CMD_POLICEID_MAP_INFO               "PoliceMap"                 // ��ԱID���豸ͨ��ӳ���ϵ��Ϣ(��Ӧ�ṹ�� CFG_POLICEID_MAP_INFO)
#define CFG_CMD_GPS_NOT_ALIGNED                 "GpsNotAligned"             // GPSδ��λ����(��Ӧ�ṹ�� CFG_GPS_NOT_ALIGNED_INFO) 
#define CFG_CMD_WIRELESS_NOT_CONNECTED          "WireLessNotConnected"      // ����δ���ӣ�����wifi��3G/4G������(��Ӧ�ṹ�� CFG_WIRELESS_NOT_CONNECTED_INFO)
#define CFG_CMD_MCS_GENERAL_CAPACITY_LOW		"MCSGeneralCapacityLow"		// ΢�Ƴ��������澯����(��Ӧ�ṹ��CFG_MCS_GENERAL_CAPACITY_LOW)
#define CFG_CMD_MCS_DATA_NODE_OFFLINE			"MCSDataNodeOffline"		// ΢�ƴ洢�ڵ�����(��Ӧ�ṹ��CFG_MCS_DATA_NODE_OFFLINE)
#define CFG_CMD_MCS_DISK_OFFLINE				"MCSDiskOffline"			// ΢�ƴ������߸澯����(��Ӧ�ṹ��CFG_MCS_DISK_OFFLINE)
#define CFG_CMD_MCS_DISK_SLOW					"MCSDiskSlow"				// ΢�ƴ��̱����澯����(��Ӧ�ṹ��CFG_MCS_DISK_SLOW)
#define CFG_CMD_MCS_DISK_BROKEN					"MCSDiskBroken"				// ΢�ƴ����𻵸澯����(��Ӧ�ṹ��CFG_MCS_DISK_BROKEN)
#define CFG_CMD_MCS_DISK_UNKNOW_ERROR			"MCSDiskUnknowError"		// ΢�ƴ���δ֪����澯����(��Ӧ�ṹ��CFG_MCS_DISK_UNKNOW_ERROR)
#define CFG_CMD_MCS_METADATA_SERVER_ABNORMAL	"MCSMetadataServerAbnormal" // ΢��Ԫ���ݷ������쳣�澯����(��Ӧ�ṹ��CFG_MCS_METADATA_SERVER_ABNORMAL)
#define CFG_CMD_MCS_CATALOG_SERVER_ABNORMAL		"MCSCatalogServerAbnormal"	// ΢��Ŀ¼�������쳣�澯����(��Ӧ�ṹ��CFG_MCS_CATALOG_SERVER_ABNORMAL)
#define CFG_CMD_MCS_GENERAL_CAPACITY_RESUME		"MCSGeneralCapacityResume"	// ΢�Ƴ��������澯�ָ��澯����(��Ӧ�ṹ��CFG_MCS_GENERAL_CAPACITY_RESUME)
#define CFG_CMD_MCS_DATA_NODE_ONLINE			"MCSDataNodeOnline"			// ΢�ƴ洢�ڵ����߸澯����(��Ӧ�ṹ��CFG_MCS_DATA_NODE_ONLINE)
#define CFG_CMD_MCS_DISK_ONLINE					"MCSDiskOnline"				// ΢�ƴ������߸澯����(��Ӧ�ṹ��CFG_MCS_DISK_ONLINE)
#define CFG_CMD_MCS_METADATA_SLAVE_ONLINE		"MCSMetadataSlaveOnline"	// ΢��Ԫ���ݱ������߸澯����(��Ӧ�ṹ��CFG_MCS_METADATA_SLAVE_ONLINE)
#define CFG_CMD_MCS_CATALOG_SERVER_ONLINE		"MCSCatalogServerOnline"	// ΢��Ŀ¼���������߸澯����(��Ӧ�ṹ��CFG_MCS_CATALOG_SERVER_ONLINE)
#define CFG_CMD_SECURITY_ALARMS_PRIVACY         "SecurityAlarmsPrivacy"     // SecurityAlarms�ͻ����ƹ��ܣ���˽����(��Ӧ�ṹ��CFG_SECURITY_ALARMS_PRIVACY)
#define CFG_CMD_NO_FLY_TIME                     "NoFlyTime"                 // ���˻�����ʱ������ ( ��Ӧ�ṹ�� CFG_NO_FLY_TIME_INFO )
#define CFG_CMD_PWD_RESET                       "PwdReset"                  // �������ù���ʹ������ ( ��Ӧ�ṹ�� CFG_PWD_RESET_INFO )
#define	CFG_CMD_NET_MONITOR_ABORT				"NetMonitorAbort"			// ��������ж��¼�����( ��Ӧ�ṹ�� CFG_NET_MONITOR_ABORT_INFO )
#define CFG_CMD_LOCAL_EXT_ALARM                 "LocalExtAlarm"             // ������չ�������� ( ��Ӧ�ṹ�� CFG_LOCAL_EXT_ALARME_INFO )
#define CFG_CMD_ACCESSCONTROL_DELAYSTRATEGY     "DelayStrategy"             // �Ž���Ƿ����ԤǷ��״̬����(��Ӧ�ṹ�� CFG_ACCESSCONTROL_DELAYSTRATEGY)
#define	CFG_CMD_VIDEO_TALK_PHONE_BASIC			"VideoTalkPhoneBasic"		// ��Ƶ�Խ��绰��������( ��Ӧ�ṹ�� CFG_VIDEO_TALK_PHONE_BASIC_INFO )
#define CFG_CMD_APP_EVENT_LANGUAGE				"AppEventLanguage"			// �ֻ�������Ϣ�ķ���Ŀ����������(��Ӧ�ṹ�� CFG_APP_EVENT_LANGUAGE_INFO )
#define CFG_CMD_LOGIN_FAILURE_ALARM				"LoginFailureAlarm"			// ��½ʧ�ܱ�������(��Ӧ�ṹ�� CFG_LOGIN_FAILURE_ALARM)
#define CFG_CMD_DROPBOXTOKEN                    "DropBoxToken"              // Dropbox Token ����( ��Ӧ�ṹ�� CFG_DROPBOXTOKEN_INFO)
#define CFG_CMD_IDLINGTIME						"IdlingTime"				// ��������(��Ӧ�ṹ�� CFG_IDLINGTIME_INFO) 
#define CFG_CMD_CARDIVERSTATE					"CarDiverState"				// ������ʻ״̬����(��Ӧ�ṹ�� CFG_CARDIVERSTATE_INFO)
#define CFG_CMD_VEHICLE							"Vehicle"					// ��������(��Ӧ�ṹ�� CFG_VEHICLE_INFO)
#define CFG_CMD_PTZDEVICE                       "PtzDevice"                 // ģ����̨����( ��Ӧ�ṹ�� CFG_PTZDEVICE_INFO)
#define CFG_CMD_DEVLOCATION                     "DevLocation"               // �豸��װλ�õ�GPS������Ϣ( ��Ӧ�ṹ�� CFG_DEVLOCATION_INFO)
#define CFG_CMD_SMARTHOME_SCENELIST				"SmartHomeSceneList"		//���ĵز����ܼҾӳ����б�(��Ӧ�ṹ��CFG_SMARTHOME_SCENELIST_INFO)
#define CFG_CMD_LIGHTING_V2						"Lighting_V2"				//ȫ��������������������(��Ӧ�ṹ�� CFG_LIGHTING_V2_INFO)
#define CFG_CMD_KBUSER_PASSWORD					"KbuserPassword"               // ���̲���Ա�û�����(��Ӧ�ṹ��CFG_KBUSER_USERS_INFO)
#define	CFG_CMD_ACCESS_OEM						"AccessOEMSettings"			//���н���Ž���Ϣ���ã���Ӧ�ṹ��CFG_ACCESS_OEM_INFO
#define CFG_CMD_FIRE_WARNING_EXT				"FireWarningExt"			// ��������չ����Ӧ�ṹ��CFG_FIREWARNING_EXT_INFO��
#define CFG_CMD_LOCATION_CALIBRATE				"LocationCalibrate"			// �豸�����궨����(��Ӧ�ṹ��CFG_LOCATION_CALIBRATE_INFO)
#define	CFG_CMD_THERM_DENOISE					"ThermDenoise"				//�ȳ������еĻ�о���룬��Ӧ�ṹ��CFG_THERM_DENOISE
#define	CFG_CMD_CONSTANT_LAMP					"ConstantLamp"				//���ܽ�ͨ�ƹ����ã���Ӧ�ṹ��CFG_CONSTANT_LAMP_INFO
#define	CFG_CMD_TRAFFIC_IO					    "TrafficIO"				    //��ȦIO���ã���Ӧ�ṹ��CFG_TRAFFIC_IO
#define	CFG_CMD_MONITOR_WALL_COLLECTION_MAP		"MonitorWallCollectionMap"  // ����ǽԤ��������,��Ӧ�ṹ��CFG_MONITOR_WALL_COLLECTION_MAP_INFO 
#define	CFG_CMD_VIDEO_INPUT_GROUP		        "VideoInputGroup"           // ��Ƶ����������,��Ӧ�ṹ��CFG_VIDEO_INPUT_GROUP_INFO 


/************************************************************************
 ** ����������  ��ӦCLIENT_QueryNewSystemInfo
 ***********************************************************************/

#define CFG_CAP_CMD_VIDEOANALYSE                "devVideoAnalyse.getCaps"                   // ��Ƶ����������(��ӦCFG_CAP_ANALYSE_INFO)
#define CFG_CAP_CMD_VIDEOANALYSE_EX             "devVideoAnalyse.getCapsEx"                 // ��Ƶ����������(��ӦCFG_CAP_ANALYSE_INFO_EX)
#define CFG_NETAPP_REMOTEDEVICE	                "netApp.getRemoteDeviceStatus"              // ��ȡ����豸�ĵ�����״̬(��ӦCFG_REMOTE_DEVICE_STATUS)
#define CFG_CAP_CMD_PRODUCTDEFINITION           "magicBox.getProductDefinition"             // �����豸��Ϣ(��Ӧ CFG_PRODUCT_DEFINITION_INFO)
#define CFG_DEVICE_CAP_CMD_VIDEOANALYSE         "intelli.getVideoAnalyseDeviceChannels"     // �豸���ܷ�������(��ӦCFG_CAP_DEVICE_ANALYSE_INFO)�������豸
#define CFG_DEVICE_CAP_NEW_CMD_VIDEOANALYSE     "devVideoAnalyse.factory.getCollect"        // �豸���ܷ�������(��ӦCFG_CAP_DEVICE_ANALYSE_INFO)
#define CFG_CAP_CMD_CPU_COUNT                   "magicBox.getCPUCount"                      // ���CPU����
#define CFG_CAP_CMD_CPU_USAGE                   "magicBox.getCPUUsage"                      // ��ȡCPUռ����
#define CFG_CAP_CMD_MEMORY_INFO                 "magicBox.getMemoryInfo"                    // ����ڴ�����
#define	CFG_CAP_CMD_DEVICE_CLASS 				"magicBox.getDeviceClass"					// ��ȡ�豸����(��ӦCFG_DEVICE_CLASS_INFO)
#define CFG_CAP_CMD_DEVICE_STATE                "trafficSnap.getDeviceStatus"               // ��ȡ�豸״̬��Ϣ (��ӦCFG_CAP_TRAFFIC_DEVICE_STATUS)
#define CFG_CAP_CMD_VIDEOINPUT                  "devVideoInput.getCaps"                     // ��Ƶ����������(��ӦCFG_CAP_VIDEOINPUT_INFO)
#define CFG_USERMANAGER_ACTIVEUSER              "userManager.getActiveUserInfoAll"          // �õ����л���û���Ϣ(��Ӧ CFG_ACTIVEALLUSER_INFO)
#define CFG_CAP_VIDEOSTAT_SUMMARY               "videoStatServer.getSummary"                // ��ȡ��Ƶͳ��ժҪ��Ϣ(��ӦCFG_VIDEOSATA_SUMMARY_INFO)
#define CFG_CAP_CMD_VIDEODIAGNOSIS_SERVER       "videoDiagnosisServer.getCaps"              // ��ȡ��Ƶ��Ϸ�������(CFG_VIDEODIAGNOSIS_CAPS_INFO)
#define CFG_CMD_VIDEODIAGNOSIS_GETCOLLECT       "videoDiagnosisServer.factory.getCollect"   // ��ȡ��Ƶ���ͨ����Ŀ(CFG_VIDEODIAGNOSIS_GETCOLLECT_INFO)
#define CFG_CMD_VIDEODIAGNOSIS_GETSTATE         "videoDiagnosisServer.getState"             // ��ȡ��Ƶ��Ͻ���״̬(CFG_VIDEODIAGNOSIS_STATE_INFO)
#define CFG_CAP_CMD_SERVICE_LIST                "system.listService"                        //��ȡ��������֧�ֵķ����б�(CFG_DEV_SERVICE_LIST)
#define CFG_CAP_CMD_EVENTHANDLER                "capsManager.get&EventManagerEventHandler"  // ��ȡ��������������������(CFG_CAP_EVENTHANDLER_INFO)
#define CFG_CAP_ALARM                           "alarm.getAlarmCaps"                        // ��ȡ����������(CFG_CAP_ALARM_INFO)
#define CFG_CAP_CMD_AUDIO_ANALYSE               "devAudioAnalyse.getCaps"                   // ��ȡ��Ƶ����������(CFG_CAP_AUDIO_ANALYSE_INFO)
#define CFG_CMD_MASTERSLAVE_GETCOLLECT          "masterSlaveTracker.factory.getCollect"     // ��ȡ����ʽ������ͨ����Ŀ(��Ӧ�ṹ��CFG_MASTERSLAVETRACKER_INFO)
#define CFG_CAP_CMD_MASTERSLAVE                 "capsManager.get&MasterSlaveTracker"        // ��ȡ����ʽ�豸������(CFG_CAP_MASTERSLAVE_INFO)
#define CFG_CAP_CMD_FOCUS_STATE                 "devVideoInput.getFocusStatus"              // ��ȡ��ͷ�۽�״̬��Ϣ (��ӦCFG_CAP_FOCUS_STATUS)
#define CFG_CAP_CMD_NETAPP                      "netApp.getCaps"                            // ��ȡ����Ӧ��������(��ӦCFG_CAP_NETAPP)
#define CFG_CAP_CMD_PTZ_ENABLE                  "ptz.factory.instance"                      // ��ȡ��̨֧����Ϣ(CFG_CAP_PTZ_ENABLEINFO )
#define CFG_CAP_CMD_RECORD                      "recordManager.getCaps"                     // ��ȡ¼��������(CFG_CAP_RECORD_INFO)
#define CFG_CAP_CMD_BURN_MANAGER                "BurnManager.getCaps"                       // ��ȡ��¼����������( CFG_CAP_BURN_MANAGER)
#define CFG_CAP_CMD_PTZ                         "ptz.getCurrentProtocolCaps"                // ��ȡ��̨������(CFG_PTZ_PROTOCOL_CAPS_INFO)
#define CFG_CMD_ENCODE_GETCAPS                  "encode.getCaps"                            // ��ȡ����������(CFG_ENCODECAP)
#define CFG_CAP_CMD_VIDEOINPUT_EX               "devVideoInput.getCapsEx"                   // ��Ƶ������������չ(��ӦCFG_CAP_VIDEOINPUT_INFO_EX)
#define CFG_CAP_CMD_ANALYSE_MODE                "intelli.getCaps.AnalyseMode"               // ��ȡ�豸���ܷ���ģʽ(��Ӧ CFG_ANALYSE_MODE)
#define CFG_CAP_CMD_EVENTMANAGER                "eventManager.getCaps"                      // ��ȡ�豸������������,��Э���ѷϳ�,�¿�����ʹ�ø��ֶ�(CFG_CAP_EVENTMANAGER_INFO)
#define CFG_CAP_CMD_FILEMANAGER	                "FileManager.getCaps"                       // ��ȡ�ļ�����(CFG_CAP_FILEMANAGER)
#define	CFG_CAP_CMD_LOG                         "log.getCaps"                               // ��ȡ��־��������(CFG_CAP_LOG)
#define CFG_CAP_CMD_SPEAK                       "speak.getCaps"                             // ��������������(CFG_CAP_SPEAK)
#define CFG_CAP_CMD_ACCESSCONTROLMANAGER        "accessControlManager.getCaps"              // �Ž�����(CFG_CAP_ACCESSCONTROL)
#define CFG_CAP_CMD_EXALARM                     "alarm.getExAlarmCaps"                      // ��ȡ��չ����������, (��Ӧ�ṹ�� CFG_CAP_EXALARM_INFO)
#define CFG_CAP_CMD_EXALARMBOX                  "alarm.getExAlarmBoxCaps"                   // ��ȡ��չ������������, (��Ӧ�ṹ�� CFG_CAP_EXALARMBOX_INFO)
#define CFG_CAP_CMD_RECORDFINDER                "RecordFinder.getCaps"                      // ��ȡ��ѯ��¼������, (��Ӧ�ṹ�� CFG_CAP_RECORDFINDER_INFO)
#define CFG_CAP_CMD_ANALOGALARM	                "AnalogAlarm.getCaps"                       // ģ������������ͨ������(��Ӧ�ṹ�� CFG_CAP_ANALOGALARM)
#define CFG_CAP_CMD_LOWRATEWPAN	                "LowRateWPAN.getCaps"                       // ��ȡLowRateWPAN����(��Ӧ�Ľṹ�� CFG_CAP_LOWRATEWPAN)
#define CFG_CAP_CMD_ADAPTENCODE                 "encode.getNAACaps"                         // ��ȡ��������Ӧ��������(��Ӧ�Ľṹ�� CFG_CAP_ADAPT_ENCODE_INFO)
#define CFG_CAP_CMD_PTZPROTOCAL	                "ptz.getProtocol"                           // ��ȡ����̨ʵ���ܹ�ʹ�õ�Э�飬���������� (��Ӧ�ṹ�� CFG_CAP_PTZ_PROTOCOL)
#define CFG_CAP_CMD_MEDIACROP                   "encode.getCropCaps"                        // ��ѯ�Ƿ�֧����Ƶ�ü����� (��Ӧ�ṹ�� CFG_CAP_MEDIA_CROP)
#define CFG_CAP_CMD_OSDMANAGER					"OSDManager.getCaps"			            // ��ȡOSD��������(��Ӧ�ṹ��CFG_CAP_OSDMANAGER_INFO)
#define CFG_CAP_CMD_CUSTOM						"OSDManager.getCustomCaps"		            //  ��ȡ�Զ������������(��Ӧ�ṹ��CFG_CAP_CUSTOM_OSD_INFO)


/************************************************************************
** ����ģ����������  ��ӦCLIENT_GetVideoInAnalyse
***********************************************************************/
#define CFG_VIDEOINANALYSE_RULE                 "VideoInAnalyse.getTemplateRule"            // ��ȡ���ܹ�������ģ���Ĭ��ֵ(��Ӧ�ṹ��CFG_ANALYSERULES_INFO)
#define CFG_VIDEOINANALYSE_GLOBAL               "VideoInAnalyse.getTemplateGlobal"          // ��ȡ����ȫ������ģ���Ĭ��ֵ(��Ӧ�ṹ��CFG_VIDEOINANALYSE_GLOBAL_INFO)
#define	CFG_VIDEOINANALYSE_MODULE               "VideoInAnalyse.getTemplateModule"          // ��ȡ���ܼ��������ģ���Ĭ��ֵ(��Ӧ�ṹ��CFG_VIDEOINANALYSE_MODULE_INFO)

// ��־����
typedef struct tagCFG_CAP_LOG
{
	DWORD		dwMaxLogItems;		// �����־����
	DWORD		dwMaxPageItems;		// ÿ����ҳ�������־����
	BOOL		bSupportStartNo;	// ��ѯ�����Ƿ�֧����ʼ���
	BOOL		bSupportTypeFilter;	// ��ѯ�����Ƿ�֧�����͹���
	BOOL		bSupportTimeFilter;	// ��ѯ�����Ƿ�֧��ʱ�����
}CFG_CAP_LOG;


#define MAX_AUDIO_PROPERTY_NUM       32          // ��Ƶ����������  
#define MAX_AUDIO_FORMAT_NUM         16          // ��Ƶ��ʽ������ 

// ��Ƶ����ѹ����ʽ
enum EM_TALK_AUDIO_TYPE
{
	EM_TALK_AUDIO_PCM,
	EM_TALK_AUDIO_ADPCM, 
	EM_TALK_AUDIO_G711A, 
	EM_TALK_AUDIO_G711Mu, 
	EM_TALK_AUDIO_G726, 
	EM_TALK_AUDIO_G729, 
	EM_TALK_AUDIO_MPEG2, 
	EM_TALK_AUDIO_AMR, 
	EM_TALK_AUDIO_AAC, 
};

// ��Ƶ����
typedef struct CFG_AUDIO_PROPERTY
{
	int            nBitRate;               // ������С����λ:kbps������192kbps
	int            nSampleBit;             // ������λ���8��16
	int            nSampleRate;            // �����ʣ���λ:Hz������44100Hz
}CFG_AUDIO_PROPERTY;

// ֧�ֵ���Ƶ��ʽ
typedef struct CFG_CAP_AUDIO_FORMAT
{
	EM_TALK_AUDIO_TYPE  emCompression;          // ��Ƶѹ����ʽ�������ö��AV_Talk_Audio_Type
	int		            nPropertyNum;           // ��Ƶ���Ը���
	CFG_AUDIO_PROPERTY  stuProperty[MAX_AUDIO_PROPERTY_NUM]; // ��Ƶ����
}CFG_CAP_AUDIO_FORMAT;

// ����������
typedef struct CFG_CAP_SPEAK
{
	int						nAudioCapNum;           // ֧�ֵ���Ƶ��ʽ����
	CFG_CAP_AUDIO_FORMAT	stuAudioCap[MAX_AUDIO_FORMAT_NUM]; // ֧�ֵ���Ƶ��ʽ 
}CFG_CAP_SPEAK;

// AccessControlCustomPassword��¼��������ı��淽ʽ
typedef enum tagEM_CUSTOM_PASSWORD_ENCRYPTION_MODE
{
	EM_CUSTOM_PASSWORD_ENCRYPTION_MODE_UNKNOWN,			// δ֪��ʽ
	EM_CUSTOM_PASSWORD_ENCRYPTION_MODE_PLAINTEXT,		// ����
	EM_CUSTOM_PASSWORD_ENCRYPTION_MODE_MD5,				// MD5���ܷ�ʽ
}EM_CUSTOM_PASSWORD_ENCRYPTION_MODE;

// �Ƿ�֧��ָ�ƹ���
typedef enum tagEM_SUPPORTFINGERPRINT
{
	EM_SUPPORTFINGERPRINT_UNKNOWN,				// δ֪
	EM_SUPPORTFINGERPRINT_NONSUPPORT,			// ��֧��ָ�ƹ���
	EM_SUPPORTFINGERPRINT_SUPPORT,				// ֧��ָ�ƹ���
}EM_SUPPORTFINGERPRINT; 

// ���ռƻ�
typedef struct tagNET_SPECIAL_DAYS_SCHEDULE
{
	BOOL								bSupport;						// �Ƿ�֧�ּ��ռƻ�
	int									nMaxSpecialDaysSchedules;		// �豸֧�ֵ����ƻ�����
	int									nMaxTimePeriodsPerDay;			// ÿ������ʱ���
	int									nMaxSpecialDayGroups;			// �豸֧�ֵ�����������
	int									nMaxDaysInSpecialDayGroup;		// ÿ�������������ļ�����
	BYTE								byReserved[128];				// �����ֽ�
} NET_SPECIAL_DAYS_SCHEDULE;

// �Ƿ�֧���Ž����ٵ��빦��
typedef enum tagEM_SUPPORT_FAST_IMPORT_TYPE
{
	EM_SUPPORT_FAST_IMPORT_UNKNOWN	= -1,			// δ֪
	EM_SUPPORT_FAST_IMPORT_NONSUPPORT,				// ��֧��
	EM_SUPPORT_FAST_IMPORT_SUPPORT,					// ֧��
} EM_SUPPORT_FAST_IMPORT_TYPE;

// �Ƿ�֧���Ž����ٸ��˹���
typedef enum tagEM_SUPPORT_FAST_CHECK_TYPE
{
	EM_SUPPORT_FAST_CHECK_UNKNOWN = -1,				// δ֪
	EM_SUPPORT_FAST_CHECK_NONSUPPORT,				// ��֧��
	EM_SUPPORT_FAST_CHECK_SUPPORT,					// ֧��
} EM_SUPPORT_FAST_CHECK_TYPE;

// �Ž���������
typedef struct tagCFG_CAP_ACCESSCONTROL
{
	int									nAccessControlGroups;			// �Ž�������
    BOOL								bSupAccessControlAlarmRecord;   // �Ƿ�֧���Ž�������־��¼�ڼ�¼����
	EM_CUSTOM_PASSWORD_ENCRYPTION_MODE  emCustomPasswordEncryption;		// AccessControlCustomPassword��¼��������ı��淽ʽ
	EM_SUPPORTFINGERPRINT				emSupportFingerPrint;			// �Ƿ�֧��ָ�ƹ���
    BOOL								bOnlySingleDoorAuth;            // �Ƿ�ֻ֧�ֵ�����Ȩ(����)
    BOOL								bAsynAuth;                      // �Ƿ�֧����Ȩ�첽����      
	NET_SPECIAL_DAYS_SCHEDULE			stSpecialDaysSchedule;			// ���ռƻ�
    BOOL                                bSupportMultiUserMultiTypeAuth; // �Ƿ�֧�ֶ��˶�������Ͽ���
    EM_SUPPORT_FAST_IMPORT_TYPE			bSupportFastImport;				// �Ƿ�֧���Ž����ٵ��빦��
    EM_SUPPORT_FAST_CHECK_TYPE			bSupportFastCheck;				// �Ƿ�֧���Ž����ٸ��˹���
	BOOL								bSupportCallLift;				// �Ƿ�֧���ݿع���
}CFG_CAP_ACCESSCONTROL;

// ��������Ӧ��ʽö������
typedef enum tagEM_SENSE_METHOD
{
    EM_SENSE_UNKNOWN = -1,		// δ֪����
	EM_SENSE_DOOR=0,			// �Ŵ�
	EM_SENSE_PASSIVEINFRA,		// ��������
	EM_SENSE_GAS,				// ����
	EM_SENSE_SMOKING,			// �̸�
	EM_SENSE_WATER,				// ˮ��
	EM_SENSE_ACTIVEFRA,			//  ��������
	EM_SENSE_GLASS,				// ��������
	EM_SENSE_EMERGENCYSWITCH,	// ��������
	EM_SENSE_SHOCK,				// ��
	EM_SENSE_DOUBLEMETHOD,		// ˫��(����+΢��)
	EM_SENSE_THREEMETHOD,		// ������
	EM_SENSE_TEMP,				// �¶�
	EM_SENSE_HUMIDITY,			// ʪ��
    EM_SENSE_WIND,				// ����
	EM_SENSE_CALLBUTTON,		// ���а�ť
	EM_SENSE_GASPRESSURE,		// ����ѹ��
	EM_SENSE_GASCONCENTRATION,	// ȼ��Ũ��
	EM_SENSE_GASFLOW,			// ��������
    EM_SENSE_OIL,				// �������
    EM_SENSE_MILEAGE,			// ��������
	EM_SENSE_OTHER,				// ����
	EM_SEHSE_CO2,				// ������̼Ũ�ȼ��
	EM_SEHSE_SOUND,				// �������
	EM_SEHSE_PM25,				// PM2.5���
	EM_SEHSE_SF6,				// ��������Ũ�ȼ��
	EM_SEHSE_O3,				// ����
	EM_SEHSE_AMBIENTLIGHT,		// �������ռ��
	EM_SEHSE_INFRARED,			// ���ⱨ��
	EM_SEHSE_TEMP1500,			// 1500�¶ȴ�����
	EM_SEHSE_TEMPDS18B20,		// DS18B20�¶ȴ�����
	EM_SEHSE_HUMIDITY1500,		// 1500ʪ�ȴ�����
	EM_SEHSE_URGENCYBUTTON,		// ������ť
	EM_SEHSE_STEAL,				// ����
	EM_SEHSE_PERIMETER,			// �ܽ�
	EM_SEHSE_PREVENTREMOVE,		// ����
	EM_SEHSE_DOORBELL,			// ����
	EM_SEHSE_ALTERVOLT,			// ������ѹ������
	EM_SEHSE_DIRECTVOLT,		// ֱ����ѹ������
	EM_SEHSE_ALTERCUR,			// ��������������
	EM_SEHSE_DIRECTCUR,			// ֱ������������
	EM_SEHSE_RSUGENERAL,		// ������ͨ��ģ����
	EM_SEHSE_RSUDOOR,			// �������Ž���Ӧ
	EM_SEHSE_RSUPOWEROFF,		// �����˶ϵ��Ӧ
	EM_SEHSE_CURTAINSENSOR,		// Ļ��������
	EM_SEHSE_MOBILESENSOR,		// �ƶ�������
	EM_SEHSE_FIREALARM,			// ��
    EM_SEHSE_LOCKTONGUE,        // ����
	EM_SENSE_NUM				// ö����������,ע�⣺���ֵ������Ϊ����ʹ��
}EM_SENSE_METHOD;

// ������������ʽ
typedef struct tagCFG_EXALARM_SENSE_METHOD
{
	int                 nSupportSenseMethodNum;								// ֧�ֵĴ�������ʽ��
	EM_SENSE_METHOD     emSupportSenseMethod[MAX_ALARM_SENSE_METHOD_NUM];   // ֧�ֵĴ�������ʽ
}CFG_EXALARM_SENSE_METHOD;

// ģ������������ͨ������
typedef struct tagCFG_CAP_ANALOGALARM 
{
	DWORD						dwLocalNum;										// ����ģ������������ͨ����
	DWORD						dwExLocalNum;									// ��չģ������������ͨ����
	int							nAnalogAlarmChannelNum;							// ģ��������ͨ����, ��Ϊ0ʱ��ʾÿ��ͨ��֧���������͵Ĵ�����
	CFG_EXALARM_SENSE_METHOD	stuAnalogAlarmChannel[MAX_EXALARM_CHANNEL_NUM];	// ����ͨ������
}CFG_CAP_ANALOGALARM;

// ��ȡLowRateWPAN�����ṹ��
typedef struct tagCFG_CAP_LOWRATEWPAN
{
	UINT						nMaxPageSize;						// ����ҳ��ѯ�Ķ�������
	UINT						n433GatewayCount;					// ��ȡ֧�ֵ�433������Ŀ, 0��ʾ��֧�������豸
}CFG_CAP_LOWRATEWPAN;

// ����Ӧ����
typedef struct tagCFG_CAP_ADAPT_ENCODE
{
    BOOL        bSupportedNAAEncode;                                // ����Ӧ����������� ���������ݲ�֧��
    BOOL        bSupportedIPSmoothness;                             // �豸����ƽ����������
}CFG_CAP_ADAPT_ENCODE;

// ��ȡ��������Ӧ����
typedef struct tagCFG_CAP_ADAPT_ENCODE_INFO
{
    CFG_CAP_ADAPT_ENCODE    stuMainStream[MAX_VIDEOSTREAM_NUM];	    // ��������0����ͨ¼��1-����¼��2������¼��
    CFG_CAP_ADAPT_ENCODE	stuExtraStream[MAX_VIDEOSTREAM_NUM];    // ��������0��������1��1��������2��2��������3
}CFG_CAP_ADAPT_ENCODE_INFO;

// ��ѯ�Ƿ�֧����Ƶ�ü�����
typedef struct tagCFG_CAP_MEDIA_CROP
{
	BOOL bExtra1SupportCrop;										// ������1�ü�����
	BOOL bExtra2SupportCrop;										// ������2�ü�����
}CFG_CAP_MEDIA_CROP;

// osd��������
typedef struct CFG_CAP_OSDMANAGER_INFO
{
	BOOL					bLineSpacing;						// �Ƿ�֧��OSD�����м�࣬ѡ��
	BOOL					bSysAbnormal;						// �Ƿ�֧���쳣��Ϣ��ʾ
	BOOL					bNetAbort;							// �Ƿ���ʾ����Ͽ�
	BOOL					bIPConflict;						// �Ƿ���ʾIP��ͻ
	BOOL					bVoltageStatus;						// �Ƿ���ʾ��ѹ�쳣
	BOOL					bSDHotPlug;							// �Ƿ���ʾSD�����롢�γ� �쳣
	BOOL					bSDLowSpace;						// �Ƿ���ʾSD���ռ䲻��
	BOOL					bSDFailure;							// �Ƿ���ʾSD������
	BOOL					bDestroy;							// �Ƿ���ʾ�ƻ����
	BOOL					bOvercurrent;						// �Ƿ���ʾ���⹩���������
	BOOL					bTaskName;							// �Ƿ�֧�ֶ�����������
	unsigned int			nMaxSize;							// ͼƬ����ֽ���,��λ�ֽ�
	unsigned int			nMaxWidth;							// ͼƬ�����, ��λ����
	unsigned int			nMaxHeight;							// ͼƬ���߶�, ��λ����
} CFG_CAP_OSDMANAGER_INFO;

// �Զ������������
typedef struct tagCFG_CAP_CUSTOM_OSD_INFO
{
	BOOL			abGeography;			// bGeography�Ƿ���Ч
	BOOL			bGeography;				//�Ƿ�֧�ֵ�����Ϣ����

	BOOL			abTitleLine;			// nTitleLine�Ƿ���Ч
	unsigned int	nTitleLine;				// ������Ϣ����, Ĭ��Ϊ5
} CFG_CAP_CUSTOM_OSD_INFO;

//Ӧ�ó���, ������EM_SCENE_CLASS_TYPEһ��
typedef enum tagEM_SCENE_TYPE
{
	EM_SCENE_UNKNOW,			// δ֪
	EM_SCENE_NORMAL,			// "Normal" ��ͨ����
	EM_SCENE_TRAFFIC,			// "Traffic" ��ͨ����
	EM_SCENE_TRAFFIC_PATROL,	// "TrafficPatrol" ��ͨѲ��
	EM_SCENE_FACEDETECTION,		// "FaceDetection" �������/����ʶ��
	EM_SCENE_ATM,				// "ATM"
	EM_SCENE_INDOOR,			// "Indoor"  ������Ϊ����������ͨ������ͬ�������ڳ������㷨�Ż���
	EM_SCENE_FACERECOGNITION,	// "FaceRecognition" ����ʶ��
	EM_SCENE_PRISON,			// "Prison" ����
	EM_SCENE_NUMBERSTAT,		// "NumberStat" ������ͳ��
	EM_SCENE_HEAT_MAP,			// "HeatMap" �ȶ�ͼ
	EM_SCENE_VIDEODIAGNOSIS,	// "VideoDiagnosis" ��Ƶ���
	EM_SCENE_VEHICLEANALYSE,	// "VehicleAnalyse" ��������������
	EM_SCENE_COURSERECORD,		// "CourseRecord" �Զ�¼��
	EM_SCENE_VEHICLE,			// "Vehicle" ���س���(������ҵ�ã���ͬ�����ܽ�ͨ��Traffic)
	EM_SCENE_STANDUPDETECTION,	// "StandUpDetection" �������
	EM_SCENE_GATE,				// "Gate" ����
	EM_SCENE_SDFACEDETECTION,	// "SDFaceDetect"  ��Ԥ�õ�������⣬����һ�����򵫿����ڲ�ͬԤ�õ�����Ч
	EM_SCENE_HEAT_MAP_PLAN,		// "HeatMapPlan" ����ȶ�ͼ�ƻ�
	EM_SCENE_NUMBERSTAT_PLAN,	// "NumberStatPlan"	���������ͳ�Ƽƻ�
	EM_SCENE_ATMFD,				// "ATMFD"����������⣬���������������쳣����������������ͷ�����������ATM���������Ż�
	EM_SCENE_HIGHWAY,			// "Highway" ���ٽ�ͨ�¼����
	EM_SCENE_CITY,				// "City" ���н�ͨ�¼����
	EM_SCENE_LETRACK,			// "LeTrack" ���ü��׸���
	EM_SCENE_SCR,				// "SCR"������
	EM_SCENE_STEREO_VISION,     // "StereoVision"�����Ӿ�(˫Ŀ)
	EM_SCENE_HUMANDETECT,       // "HumanDetect"������
	EM_SCENE_FACEANALYSIS,		// "FaceAnalysis" ��������(ͬʱ֧��������⡢����ʶ���������Ե��ۺ���ҵ��)
	EM_SCENE_XRAY_DETECTION,	// "XRayDetection" X����
	EM_SCENE_STEREO_NUMBER,		// "StereoNumber" ˫Ŀ���������ͳ��
	EM_SCENE_CROWD_DISTRI_MAP,	// "CrowdDistriMap" ��Ⱥ�ֲ�ͼ
	EM_SCENE_OBJECTDETECT,	    // "ObjectDetect" Ŀ����(���˻��ǵ�����)
	EM_SCENE_FACEATTRIBUTE,		// "FaceAttribute" IVSS�������
	EM_SCENE_FACECOMPARE,		// "FaceCompare" IVSS����ʶ��
	EM_SCENE_STEREO_BEHAVIOR,	// "StereoBehavior" ������Ϊ����(���ͳ���ATM��)
	EM_SCENE_INTELLICITYMANAGER,// "IntelliCityMgr" �ǻ۳ǹ�
	EM_SCENE_PROTECTIVECABIN,	// "ProtectiveCabin" �����գ�ATM���ڣ�
	EM_SCENE_AIRPLANEDETECT,	// "AirplaneDetect" �ɻ���Ϊ���
	EM_SCENE_CROWDPOSTURE,		// "CrowdPosture" ��Ⱥ̬�ƣ���Ⱥ�ֲ�ͼ����
	EM_SCENE_PHONECALLDETECT,	// "PhoneCallDetect"  ��绰���
	EM_SCENE_SMOKEDETECTION,	// "SmokeDetection"   ������
	EM_SCENE_BOATDETECTION,		// "BoatDetection"	  ��ֻ���
	EM_SCENE_SMOKINGDETECT,		// "SmokingDetect"	  ���̼��
	EM_SCENE_WATERMONITOR,		// "WaterMonitor"	  ˮ�����
	EM_SCENE_GENERATEGRAPHDETECTION, // GenerateGraphDetection ����ͼ����
	EM_SCENE_TRAFFIC_PARK,		// "TrafficPark"		��ͨͣ��
    EM_SCENE_OPERATEMONITOR, 	// "OperateMonitor"     ��ҵ���  
    EM_SCENE_INTELLI_RETAIL,	// "IntelliRetail" �ǻ����۴���
    EM_SCENE_CLASSROOM_ANALYSE,	// "ClassroomAnalyse" �����ǻۿ���
	EM_SCENE_FEATURE_ABSTRACT, // "FeatureAbstract" ����������ȡ����
	EM_SCENE_FACEBODY_DETECT,	// "FaceBodyDetect"������
	EM_SCENE_FACEBODY_ANALYSE,	// "FaceBodyAnalyse"����ʶ��
	EM_SCENE_VEHICLES_DISTRI,	// "VehiclesDistri"�����ܶ�
	EM_SCENE_INTELLI_BREED,     // "IntelliBreed" �ǻ���ֳ���
	EM_SCENE_INTELLI_PRISON,    // "IntelliPrison" ������Ϊ����
	EM_SCENE_ELECTRIC_DETECT,   // "ElectricDetect" �������
	EM_SCENE_RADAR_DETECT,		// "RadarDetect" �׼���
    EM_SCENE_PARKINGSPACE,      // "ParkingSpace" ��λ������
}EM_SCENE_TYPE;

 /************************************************************************
 ** fAnalyzerData�ص�����dwDataType���Ͷ���
 ***********************************************************************/
#define    TYPE_CB_VIDEOSTAT            0x1000000      // �ص�������Ƶ��ϢժҪ���ͣ���Ӧ�ṹ��ΪCFG_VIDEOSATA_SUMMARY_INFO 
#define    TYPE_CB_ANARULES             0x1000001      // �ص�������Ƶ����ͨ����Ӧ�������¼��������ͣ���Ӧ�ṹ��ΪCFG_ANALYSERULES_INFO
#define    TYPE_CB_VIDEOSYNOPSIS        0x1000002	   // �ص�����Ũ����Ƶ������֡����Ӧ�ṹ��DH_MSG_OBJECT
#define	   TYPE_CB_TRAFFICFLOWINFO      0x1000003	   // �ص�������ͨ������Ϣ����Ӧ�ṹ��DEV_EVENT_TRAFFIC_FLOWSTAT_INFO	
#define    TYPE_CB_TRAFFICFLOWRULE      0x1000004      // �ص�������ͨ����������Ϣ����Ӧ�ṹ�� CFG_TRAFFIC_FLOWSTAT_INFO 
#define    TYPE_CB_VIDEOSYNOPSIS_RULE   0x1000005      // �ص�����Ũ����Ƶ�Ĺ�����Ϣ����Ӧ�ṹ�� NET_VIDEOSYNOPSISRULE_INFO
	
/************************************************************************
 ** ������Ϣ(ÿ���ṹ�嶼��������Ϣ�������ֲ�ͬ�豸����汾�Ͳ�ͬ�ͺ��豸)
 ***********************************************************************/
// Э��汾��Ϣ
enum 
{
	CFG_PROTOCOL_VERSION_2 = 2,						// ����Э��
	CFG_PROTOCOL_VERSION_3 = 3						// ����Э��
};

// ��Ƶ����ͨ��
struct CFG_RemoteDeviceVideoInput 
{
	BOOL			bEnable;
	char			szName[MAX_DEVICE_NAME_LEN];
	char			szControlID[MAX_DEV_ID_LEN_EX];
	char			szMainUrl[MAX_PATH];			// ������url��ַ
	char			szExtraUrl[MAX_PATH];			// ������url��ַ
	int				nServiceType;					// ��������, 0-TCP, 1-UDP, 2-MCAST, -1-AUTO
};

// Զ���豸
struct AV_CFG_RemoteDevice 
{
	AV_int32			nStructSize;
	AV_BOOL				bEnable;						// ʹ��
	char				szID[AV_CFG_Device_ID_Len];		// �豸ID
	char				szIP[AV_CFG_IP_Address_Len];	// �豸IP
	AV_int32			nPort;							// �˿�
	char				szProtocol[AV_CFG_Protocol_Len];// Э������
	char				szUser[AV_CFG_User_Name_Len];	// �û���
	char				szPassword[AV_CFG_Password_Len];// ����
	char				szSerial[AV_CFG_Serial_Len];	// �豸���к�
	char				szDevClass[AV_CFG_Device_Class_Len];// �豸����
	char				szDevType[AV_CFG_Device_Type_Len];	// �豸�ͺ�
	char				szName[AV_CFG_Device_Name_Len];	// ��������
	char				szAddress[AV_CFG_Address_Len];	// ��������ص�
	char				szGroup[AV_CFG_Group_Name_Len];	// ��������
	AV_int32			nDefinition;					// ������, 0-����, 1-����
	AV_int32			nVideoChannel;					// ��Ƶ����ͨ����
	AV_int32			nAudioChannel;					// ��Ƶ����ͨ����
	AV_int32            nRtspPort;                      // Rtsp�˿ں�
	char                szVendor[MAX_PATH];             // �豸��������
	CFG_RemoteDeviceVideoInput *pVideoInput;            // ��Ƶ����ͨ�����û�����nMaxVideoInputs��CFG_RemoteDeviceVideoInput�ռ�
	int                 nMaxVideoInputs;
	int                 nRetVideoInputs;
	int					nHttpPort;						// http�˿ں�
														// ����3��Ϊ���ʽ��뷽ʽ���
	AV_BOOL				bGB28181;						// �Ƿ��й��ʽ��뷽ʽ
	int					nDevLocalPort;					// �豸���ض˿�
	char				szDeviceNo[AV_CFG_DeviceNo_Len];// �豸���
	int					nLoginType;						// ��¼��ʽ 0 : TCPֱ��(Ĭ�Ϸ�ʽ)  6 : ����ע��  7 : P2P��ʽ���˷�ʽʱͨ��SerialNo���豸ͨ��P2P����
};

//-----------------------------ͼ��ͨ������-------------------------------

// ����
typedef enum tagCFG_IMAGE_QUALITY
{
	IMAGE_QUALITY_Q10 = 1,							// ͼ������10%
	IMAGE_QUALITY_Q30,								// ͼ������30%
	IMAGE_QUALITY_Q50,								// ͼ������50%
	IMAGE_QUALITY_Q60,								// ͼ������60%
	IMAGE_QUALITY_Q80,								// ͼ������80%
	IMAGE_QUALITY_Q100,								// ͼ������100%
} CFG_IMAGE_QUALITY;

// ��Ƶѹ����ʽ
typedef enum tagCFG_VIDEO_COMPRESSION
{
	VIDEO_FORMAT_MPEG4,								// MPEG4
	VIDEO_FORMAT_MS_MPEG4,							// MS-MPEG4
	VIDEO_FORMAT_MPEG2,								// MPEG2
	VIDEO_FORMAT_MPEG1,								// MPEG1
	VIDEO_FORMAT_H263,								// H.263
	VIDEO_FORMAT_MJPG,								// MJPG
	VIDEO_FORMAT_FCC_MPEG4,							// FCC-MPEG4
	VIDEO_FORMAT_H264,								// H.264
    VIDEO_FORMAT_H265,								// H.265
	VIDEO_FORMAT_SVAC,								// SVAC
} CFG_VIDEO_COMPRESSION;
// ��Ƶ����ģʽ
typedef enum tatCFG_AUDIO_FORAMT
{
	AUDIO_FORMAT_G711A,                              // G711a
    AUDIO_FORMAT_PCM,                                // PCM
    AUDIO_FORMAT_G711U,                              // G711u
    AUDIO_FORMAT_AMR,                                // AMR
    AUDIO_FORMAT_AAC,                                // AAC
} CFG_AUDIO_FORMAT;

// ��������ģʽ
typedef enum tagCFG_BITRATE_CONTROL
{
	BITRATE_CBR,									// �̶�����
	BITRATE_VBR,									// �ɱ�����
} CFG_BITRATE_CONTROL;

// H264 ���뼶��
typedef enum tagCFG_H264_PROFILE_RANK
{
	PROFILE_BASELINE = 1,                       // �ṩI/P֡����֧��progressive(����ɨ��)��CAVLC
	PROFILE_MAIN,                               // �ṩI/P/B֡��֧��progressiv��interlaced���ṩCAVLC��CABAC
	PROFILE_EXTENDED,                           // �ṩI/P/B/SP/SI֡����֧��progressive(����ɨ��)��CAVLC
	PROFILE_HIGH,                               // ��FRExt��Main_Profile������������8x8 intra prediction(8x8 ֡��Ԥ��), custom 
												// quant(�Զ�������), lossless video coding(������Ƶ����), �����yuv��ʽ
}CFG_H264_PROFILE_RANK;

// �ֱ���ö��
typedef enum tagCFG_CAPTURE_SIZE
{
	IMAGE_SIZE_D1,								// 704*576(PAL)  704*480(NTSC)
	IMAGE_SIZE_HD1,								// 352*576(PAL)  352*480(NTSC)
	IMAGE_SIZE_BCIF,							// 704*288(PAL)  704*240(NTSC)
	IMAGE_SIZE_CIF,								// 352*288(PAL)  352*240(NTSC)
	IMAGE_SIZE_QCIF,							// 176*144(PAL)  176*120(NTSC)
	IMAGE_SIZE_VGA,								// 640*480
	IMAGE_SIZE_QVGA,							// 320*240
	IMAGE_SIZE_SVCD,							// 480*480
	IMAGE_SIZE_QQVGA,							// 160*128
	IMAGE_SIZE_SVGA,							// 800*592
	IMAGE_SIZE_XVGA,							// 1024*768
	IMAGE_SIZE_WXGA,							// 1280*800
	IMAGE_SIZE_SXGA,							// 1280*1024  
	IMAGE_SIZE_WSXGA,							// 1600*1024  
	IMAGE_SIZE_UXGA,							// 1600*1200
	IMAGE_SIZE_WUXGA,							// 1920*1200
	IMAGE_SIZE_LTF,								// 240*192
	IMAGE_SIZE_720,								// 1280*720
	IMAGE_SIZE_1080,							// 1920*1080
	IMAGE_SIZE_1_3M,							// 1280*960
	IMAGE_SIZE_2M,							    // 1872*1408
	IMAGE_SIZE_5M,						   	    // 3744*1408
	IMAGE_SIZE_3M,							    // 2048*1536
	IMAGE_SIZE_5_0M,                            // 2432*2050
	IMAGE_SIZE_1_2M,							// 1216*1024
	IMAGE_SIZE_1408_1024,                       // 1408*1024
	IMAGE_SIZE_8M,                              // 3296*2472
	IMAGE_SIZE_2560_1920,                       // 2560*1920(5M)
	IMAGE_SIZE_960H,                            // 960*576(PAL) 960*480(NTSC)
	IMAGE_SIZE_960_720,                         // 960*720
	IMAGE_SIZE_NHD,							    // 640*360
	IMAGE_SIZE_QNHD,							// 320*180
	IMAGE_SIZE_QQNHD,							// 160*90
	IMAGE_SIZE_NR  
} CFG_CAPTURE_SIZE;

// ��Ƶ��ʽ
typedef struct tagCFG_VIDEO_FORMAT
{
	// ����
	bool				abCompression;
	bool				abWidth;
	bool				abHeight;
	bool				abBitRateControl;
	bool				abBitRate;
	bool				abFrameRate;
	bool				abIFrameInterval;
	bool				abImageQuality;
	bool				abFrameType;
	bool				abProfile;

	// ��Ϣ
	CFG_VIDEO_COMPRESSION emCompression;			// ��Ƶѹ����ʽ
	int					nWidth;						// ��Ƶ���
	int					nHeight;					// ��Ƶ�߶�
	CFG_BITRATE_CONTROL	emBitRateControl;			// ��������ģʽ
	int					nBitRate;					// ��Ƶ����(kbps)
	float				nFrameRate;					// ��Ƶ֡��
	int					nIFrameInterval;			// I֡���(1-100)������50��ʾÿ49��B֡��P֡������һ��I֡��
	CFG_IMAGE_QUALITY	emImageQuality;				// ͼ������
	int					nFrameType;					// ���ģʽ��0��DHAV��1��"PS"
    CFG_H264_PROFILE_RANK emProfile;                // H.264���뼶��
} CFG_VIDEO_FORMAT;

// ��Ƶ��ʽ
typedef struct tagCFG_AUDIO_FORMAT 
{
	// ����
	bool				abCompression;
	bool				abDepth;
	bool				abFrequency;
	bool				abMode;
	bool				abFrameType;
	bool				abPacketPeriod;
    bool                abChannels;
    bool                abMix;

	// ��Ϣ
	CFG_AUDIO_FORMAT	emCompression;				// ��Ƶѹ��ģʽ
	AV_int32			nDepth;						// ��Ƶ�������
	AV_int32			nFrequency;					// ��Ƶ����Ƶ��
	AV_int32			nMode;						// ��Ƶ����ģʽ
	AV_int32			nFrameType;					// ��Ƶ���ģʽ, 0-DHAV, 1-PS
	AV_int32			nPacketPeriod;				// ��Ƶ�������, ms
    AV_int32            nChannelsNum;               // ��Ƶͨ���İ���ͨ�����б����
    AV_uint32           arrChannels[MAX_AUDIOCHANNELS_ARRAY_NUM];   // ��Ƶͨ���İ���ͨ�����б�
    BOOL                bMix;                       // �Ƿ�ͬԴ
} CFG_AUDIO_ENCODE_FORMAT;

// ��Ƶ�������
typedef struct tagCFG_VIDEOENC_OPT
{
	// ����
	bool				abVideoEnable;
	bool				abAudioEnable;
	bool				abSnapEnable;
	bool                abAudioAdd;                 // ��Ƶ��������
	bool				abAudioFormat;

	// ��Ϣ
	BOOL				bVideoEnable;				// ��Ƶʹ��
	CFG_VIDEO_FORMAT	stuVideoFormat;				// ��Ƶ��ʽ
	BOOL				bAudioEnable;				// ��Ƶʹ��
	BOOL				bSnapEnable;				// ��ʱץͼʹ��
	BOOL                bAudioAddEnable;            // ��Ƶ����ʹ��
	CFG_AUDIO_ENCODE_FORMAT	stuAudioFormat;			// ��Ƶ��ʽ
} CFG_VIDEOENC_OPT;

// RGBA��Ϣ
typedef struct tagCFG_RGBA
{
	int					nRed;
	int					nGreen;
	int					nBlue;
	int					nAlpha;
} CFG_RGBA;

// ������Ϣ
typedef struct tagCFG_RECT
{
	int					nLeft;
	int					nTop;
    int					nRight;
    int					nBottom;				
} CFG_RECT;


// ���򶥵���Ϣ
typedef struct tagCFG_POLYGON
{
	int					nX; //0~8191
	int					nY;		
} CFG_POLYGON;

// ������Ϣ
typedef struct tagCFG_REGION
{
	int         nPointNum;
	CFG_POLYGON stuPolygon[MAX_POLYGON_NUM];
}CFG_REGION;

// ���ߵĶ˵���Ϣ
typedef struct tagCFG_POLYLINE
{
	int					nX; //0~8191
	int					nY;		
} CFG_POLYLINE;

typedef enum tagCFG_FLOWSTAT_DIRECTION
{
	CFG_DRIVING_DIR_APPROACH ,		//���У����������豸�����Խ��Խ��
	CFG_DRIVING_DIR_LEAVE ,				//���У����������豸�����Խ��ԽԶ
}CFG_FLOWSTAT_DIRECTION;

#define  CFG_FLOWSTAT_ADDR_NAME			16						//�����еص�����
//��������ͳ�Ƴ���������Ϣ 
typedef struct tagCFG_TRAFFIC_FLOWSTAT_DIR_INFO
{
	CFG_FLOWSTAT_DIRECTION		emDrivingDir;							//��ʻ����
	char						szUpGoing[CFG_FLOWSTAT_ADDR_NAME];			//���еص� 
	char						szDownGoing[CFG_FLOWSTAT_ADDR_NAME];		//���еص� 
}CFG_TRAFFIC_FLOWSTAT_DIR_INFO;

// ��·�ȼ�
typedef enum tagEM_LANE_RANK_TYPE
{
	EM_LANE_RANK_UNKNOWN,			// δ֪����
	EM_LANE_RANK_RAPID,				// ����·
	EM_LANE_RANK_TRUNK,				// ����·
	EM_LANE_RANK_SUBTRUNK,			// �θ�·
	EM_LANE_RANK_BRANCH,			// ֧·
} EM_LANE_RANK_TYPE;

// ������Ϣ
typedef struct tagCFG_LANE
{
	int                nLaneId;                           // �������
	int                nDirection;                        // ��������(�������ķ���),0-�� 1-���� 2-�� 3-���� 4-�� 5-���� 6-�� 7-����
	CFG_POLYLINE       stuLeftLine[MAX_POLYLINE_NUM];     // �󳵵��ߣ������ߵķ����ʾ���������س���������ߵĳ�Ϊ�󳵵���
	int                nLeftLineNum;                      // �󳵵��߶�����
	CFG_POLYLINE       stuRightLine[MAX_POLYLINE_NUM];    // �ҳ����ߣ������ߵķ����ʾ���������س��������ұߵĳ�Ϊ�ҳ�����
	int                nRightLineNum;                     // �ҳ����߶�����
	int                nLeftLineType;                     // �󳵵������ԣ�1-��ʾ��ʵ�ߣ�2- �����ߣ�3- ����
	int                nRightLineType;                    // �ҳ��������ԣ�1-��ʾ��ʵ�ߣ�2- �����ߣ�3- ����
	BOOL               bDriveDirectionEnable;             // ������ʻ����ʹ��
	int                nDriveDirectionNum;                     // ������ʻ������ 
	char               szDriveDirection[MAX_LIGHT_DIRECTION][MAX_NAME_LEN];  // ������ʻ����"Straight" ֱ�У�"TurnLeft" ��ת��"TurnRight" ��ת,"U-Turn":��ͷ
	int                nStopLineNum;                      // ������Ӧֹͣ�߶�����
	CFG_POLYLINE       stuStopLine[MAX_POLYLINE_NUM];     // ������Ӧֹͣ��
	int                nTrafficLightNumber;               // ������Ӧ�ĺ��̵�����

	bool               abDetectLine;                      // ��Ӧ������
	bool               abPreLine;
	bool               abPostLine;
	BYTE               byReserved[1];

	int                nDetectLine;
	CFG_POLYLINE       stuDetectLine[MAX_POLYLINE_NUM];   // ������Ӧ�ļ����
	int                nPreLine;
	CFG_POLYLINE       stuPreLine[MAX_POLYLINE_NUM];      // ������Ӧ��ǰ����
	int                nPostLine;
	CFG_POLYLINE       stuPostLine[MAX_POLYLINE_NUM];     // ������Ӧ�ĺ�����
	CFG_TRAFFIC_FLOWSTAT_DIR_INFO stuTrafficFlowDir;	  // ����������Ϣ
	EM_LANE_RANK_TYPE  emRankType;						  // ��·�ȼ������ڳ�����ͳ���ϱ���ͨ״̬
}CFG_LANE;

// ��ͨ������
typedef struct tagCFG_LIGHTATTRIBUTE
{
	BOOL             bEnable;                           // ��ǰ��ͨ���Ƿ���Ч���복��ͨ���޹صĽ�ͨ��Ҫ������Ч
	int              nTypeNum; 
	char		     szLightType[MAX_LIGHT_TYPE][MAX_NAME_LEN];      // ��ǰ��ͨ���������ݣ�����:��-Red,��-Yellow,��-Green,����ʱ-Countdown������ĳ��ͨ�ƿ�����ʾ�����������ɫ��ĳ��ͨ��ֻ��ʾ����ʱ
	int              nDirectionNum;
	char             szDirection[MAX_LIGHT_DIRECTION][MAX_NAME_LEN];      // ��ͨ��ָʾ���г�����,"Straight": ֱ�У�"TurnLeft":��ת��"TurnRight":��ת��"U-Turn": ��ͷ
	int              nYellowTime;                       // �Ƶ���ʱ��
}CFG_LIGHTATTRIBUTE;

// ��ͨ����������Ϣ 
typedef struct tagCFG_LIGHTGROUPS
{
	int                nLightGroupId;                     // ������
	CFG_RECT           stuLightLocation;                  // ��������
	int                nDirection;                        // ����ķ���,1- ����ˮƽ��,2- ���鴹ֱ��
	BOOL               bExternalDetection;                // �Ƿ�Ϊ��Ӻ��̵��ź�,����Ӻ��̵�ʱ��������ź�Ϊ�ж����ݡ�����ź�ÿ������ʱ֪ͨ
	BOOL               bSwingDetection;                   // �Ƿ�֧������Ӧ����ҡ�ڼ��,�ڷ紵���������𶯵ĳ����£�λ�û����һ���ĸ���ƫ�������㷨���м�⣬�����Ӽ��ʱ��
	int                nLightNum;                         // �����н�ͨ�Ƶ�����
	CFG_LIGHTATTRIBUTE stuLightAtrributes[MAX_LIGHT_NUM]; // �����и���ͨ�Ƶ�����

}CFG_LIGHTGROUPS;

enum EM_STAFF_TYPE
{
	EM_STAFF_TYPE_ERR,
		EM_STAFF_TYPE_HORIZONTAL,     // "Horizontal" ˮƽ�߶�
		EM_STAFF_TYPE_VERTICAL,       // "Vertical" ��ֱ�߶�
		EM_STAFF_TYPE_ANY,            // "Any" �����߶�
		EM_STAFF_TYPE_CROSS,          // "Cross" ��ֱ�潻�߶�
};

enum EM_CALIBRATEAREA_TYPE
{
	EM_CALIBRATEAREA_TYPE_ERR,
		EM_CALIBRATEAREA_TYPE_GROUD,                         // "Groud" : ���棬��ҪN����ֱ�߶�+M��ˮƽ�߶Σ���N=3��M=1������N=2��M=2���������չ����
		EM_CALIBRATEAREA_TYPE_HORIZONTAL,                    // "Horizontal"  : ˮƽ�棬��Ҫˮƽ����һ�㵽������ͶӰ��ֱ�߶Ρ�		
		EM_CALIBRATEAREA_TYPE_VERTICAL,                      // "Vertical" : ��ֱ�棬��Ҫ��ֱ�������Ľ��ߡ�
		EM_CALIBRATEAREA_TYPE_ANY,                           // "Any" ����ƽ�棬N����ֱ�߶Σ���ÿ�����ȣ�N=3���������չ����
};

// �����������������
enum EM_SEPCIALREGION_PROPERTY_TYPE
{
	EM_SEPCIALREGION_PROPERTY_TYPE_HIGHLIGHT = 1,            // ���������̼��������д�����
	EM_SEPCIALREGION_PROPERTY_TYPE_REGULARBLINK,             // ���ɵ���˸���忨������д�����
	EM_SEPCIALREGION_PROPERTY_TYPE_IREGULARBLINK,            // �����ɵ���˸����Ļ������д�����
	EM_SEPCIALREGION_PROPERTY_TYPE_NUM,
};


// �����������
enum EM_FACEDETECTION_TYPE
{
	EM_FACEDETECTION_TYPE_ERR,
	EM_FACEDETECTION_TYPE_SMALLFACE,						//С�����ͣ���������Ƶ�еı��ش��17%
	EM_FACEDETECTION_TYPE_LARGEFACE,						//�������ͣ���������Ƶ�еı��ش��33%
	EM_FACEDETECTION_TYPE_BOTH,								//�������
}; 

typedef struct tagCFG_STAFF
{
	CFG_POLYLINE       stuStartLocation;      // ��ʼ�����
	CFG_POLYLINE       stuEndLocation;        // ��ֹ�����
	float                nLenth;              // ʵ�ʳ���,��λ��
	EM_STAFF_TYPE	   emType;                // �������
}CFG_STAFF;

// Size
typedef struct tagCFG_SIZE
{
	union
	{
		float				nWidth;			// ��
		float				nArea;			// ���
	};
	float					nHeight;		// ��
	
} CFG_SIZE;

// �ڵ���Ϣ
typedef struct tagCFG_COVER_INFO
{
	// ����
	bool				abBlockType;
	bool				abEncodeBlend;
	bool				abPreviewBlend;

	// ��Ϣ
	CFG_RECT			stuRect;					// ���ǵ���������
	CFG_RGBA			stuColor;					// ���ǵ���ɫ
	int					nBlockType;					// ���Ƿ�ʽ��0���ڿ飬1��������
	int					nEncodeBlend;				// ���뼶�ڵ���1����Ч��0������Ч
	int					nPreviewBlend;				// Ԥ���ڵ���1����Ч��0������Ч
} CFG_COVER_INFO;

// �������ڵ�����
typedef struct tagCFG_VIDEO_COVER
{
	int                 nTotalBlocks;						// ֧�ֵ��ڵ�����
	int					nCurBlocks;							// �����õĿ���
	CFG_COVER_INFO		stuCoverBlock[MAX_VIDEO_COVER_NUM];	// ���ǵ�����	
} CFG_VIDEO_COVER;

// OSD��Ϣ
typedef struct tagCFG_OSD_INFO
{
	// ����
	bool				abShowEnable;

	// ��Ϣ
	CFG_RGBA			stuFrontColor;				// ǰ����ɫ
	CFG_RGBA			stuBackColor;				// ������ɫ
	CFG_RECT			stuRect;					// ��������
	BOOL				bShowEnable;				// ��ʾʹ��
} CFG_OSD_INFO;

// ������ɫ����
typedef struct tagCFG_COLOR_INFO
{
	int					nBrightness;				// ����(0-100)
	int					nContrast;					// �Աȶ�(0-100)
	int					nSaturation;				// ���Ͷ�(0-100)
	int					nHue;						// ɫ��(0-100)
	int					nGain;						// ����(0-100)
	BOOL				bGainEn;					// ����ʹ��
} CFG_COLOR_INFO;

// ͼ��ͨ��������Ϣ
typedef struct tagCFG_ENCODE_INFO
{
	int                 nChannelID;							// ͨ����(0��ʼ),��ȡʱ�����ֶ���Ч������ʱ�����ֶ���Ч
	char				szChnName[MAX_CHANNELNAME_LEN];		// ��Ч�ֶ�
	CFG_VIDEOENC_OPT	stuMainStream[MAX_VIDEOSTREAM_NUM];	// ��������0����ͨ¼��1-����¼��2������¼��
	int                 nValidCountMainStream;              // ��������������Ч�ĸ���
	CFG_VIDEOENC_OPT	stuExtraStream[MAX_VIDEOSTREAM_NUM];// ��������0��������1��1��������2��2��������3
	int                 nValidCountExtraStream;             // ��������������Ч�ĸ���
	CFG_VIDEOENC_OPT	stuSnapFormat[MAX_VIDEOSTREAM_NUM];	// ץͼ��0����ͨץͼ��1������ץͼ��2������ץͼ
	int                 nValidCountSnapFormat;              // ץͼ��������Ч�ĸ���
	DWORD				dwCoverAbilityMask;					// ��Ч�ֶ�
	DWORD				dwCoverEnableMask;					// ��Ч�ֶ�
	CFG_VIDEO_COVER		stuVideoCover;						// ��Ч�ֶ�
	CFG_OSD_INFO		stuChnTitle;						// ��Ч�ֶ�
	CFG_OSD_INFO		stuTimeTitle;						// ��Ч�ֶ�
	CFG_COLOR_INFO		stuVideoColor;						// ��Ч�ֶ�
	CFG_AUDIO_FORMAT    emAudioFormat;                      // ��Ч�ֶ�
	int					nProtocolVer;						// Э��汾��, ֻ��,��ȡʱ�����ֶ���Ч������ʱ�����ֶ���Ч
} CFG_ENCODE_INFO;

// ��Ƶ����ǰ��������
typedef struct tagCFG_VIDEO_ENCODECAP
{
	int		nMaxCIFFrame;			// CIF P֡���ֵ ��λKbits, Ĭ��ֵ40
	int		nMinCIFFrame;			// CIF P֡��Сֵ ��λKbits, Ĭ��ֵ7
	int     nMaxEncodeAudios;       // ֧�ֵ���������Ƶ��������˫��Ƶ,Ĭ��1
	BYTE    byReserved[4];          // �����ֽڣ������ֽڶ���
}CFG_VIDEO_ENCODECAP;

// �໭��Ԥ������ģʽ
typedef enum tagCFG_EM_PREVIEW_MODE
{
    CFG_EM_PREVIEW_MODE_UNKNOWN = 0,        // 
    CFG_EM_PREVIEW_MODE_SNAPSHOT,           // ���շ�ʽ
    CFG_EM_PREVIEW_MODE_SPLITENCODE,        // �ָ���뷽ʽ
    CFG_EM_PREVIEW_MODE_SPLITSNAP,          // �ָ���շ�ʽ
}CFG_EM_PREVIEW_MODE;

#define MAX_PREVIEW_MODE_SPLIT_TYPE_NUM     8                           // ���໭��Ԥ�����ڷָ�������

// ����������
typedef struct tagCFG_ENCODECAP
{
	int                     nChannelNum;                                // ʵ��ͨ����
	CFG_VIDEO_ENCODECAP     stuVideoEncodeCap[MAX_VIDEO_CHANNEL_NUM];	// ��ͨ������������Ϣ����
    CFG_EM_PREVIEW_MODE     emPreviewMode;                              // �໭��Ԥ������ģʽ
    int                     nSplitModeNum;                              // ��Ч�Ķ໭��Ԥ�����ڷָ�������
    int                     anSplitMode[MAX_PREVIEW_MODE_SPLIT_TYPE_NUM];// �໭��Ԥ�����ڷָ�����Ϣ, ����Ϊ1, 4, 6, 8, 9, 16, 25, 36...
                                                                        // -1��ʾĬ��[1, 4, 8, 9, 16, ��ģ��ͨ������], ΪС��ģ��ͨ������N��ƽ����, ���ģ��ͨ������8, Ҳ����8
    int                     nAudioFrequence[16];                        // ֧�ֵ���Ƶ���������
    int                     nAudioFrequenceCount;                       // ֧�ֵ���Ƶ��������ʵ�ʵ�ʸ���
}CFG_ENCODECAP;

// ˮӡ����
typedef struct tagCFG_WATERMARK_INFO 
{
	int                 nChannelID;					// ͨ����(0��ʼ)
	BOOL				bEnable;					// ʹ�ܿ���
	int					nStreamType;				// ��������(1��n)��0����������
	int					nDataType;					// �������ͣ�1�����֣�2��ͼƬ
	char				pData[MAX_WATERMARK_LEN];	// �ַ���ˮӡ����
} CFG_WATERMARK_INFO;

// dsp����
typedef struct tagCFG_DSPENCODECAP_INFO{
	DWORD				dwVideoStandardMask;	// ��Ƶ��ʽ���룬��λ��ʾ�豸�ܹ�֧�ֵ���Ƶ��ʽ
	DWORD				dwImageSizeMask;		// �ֱ������룬��λ��ʾ�豸�ܹ�֧�ֵķֱ���
	DWORD				dwEncodeModeMask;		// ����ģʽ���룬��λ��ʾ�豸�ܹ�֧�ֵı���ģʽ
	DWORD				dwStreamCap;			// ��λ��ʾ�豸֧�ֵĶ�ý�幦�ܣ�
                                             	// ��һλ��ʾ֧��������
	                                         	// �ڶ�λ��ʾ֧�ָ�����1
	                                         	// ����λ��ʾ֧�ָ�����2
	                                         	// ����λ��ʾ֧��jpgץͼ
	DWORD				dwImageSizeMask_Assi[32];// ��ʾ������Ϊ���ֱ���ʱ��֧�ֵĸ������ֱ������롣
	DWORD				dwMaxEncodePower;		// DSP֧�ֵ���߱������� 
	WORD				wMaxSupportChannel;		// ÿ��DSP֧�����������Ƶͨ���� 
	WORD				wChannelMaxSetSync;		// DSPÿͨ���������������Ƿ�ͬ����0����ͬ����1��ͬ��
	BYTE				bMaxFrameOfImageSize[256];// ��ͬ�ֱ����µ����ɼ�֡�ʣ���dwVideoStandardMask��λ��Ӧ
	BYTE				bEncodeCap;				// ��־������ʱҪ����������������������ò�����Ч��
	                                         	// 0���������ı�������+�������ı������� <= �豸�ı���������
	                                         	// 1���������ı�������+�������ı������� <= �豸�ı���������
	                                         	// �������ı������� <= �������ı���������
	                                         	// �������ķֱ��� <= �������ķֱ��ʣ�
	                                         	// �������͸�������֡�� <= ǰ����Ƶ�ɼ�֡��
	                                         	// 2��N5�ļ��㷽��
	                                         	// �������ķֱ��� <= �������ķֱ���
												// ��ѯ֧�ֵķֱ��ʺ���Ӧ���֡��
	BYTE				byResolutionNum;			// bResolution�ĳ���
	BYTE				byResolutionNum_1;		// bResolution_1�ĳ���
	BYTE				byReserved;
	BYTE				byResolution[256];		//������,���շֱ��ʽ������������֧�ָ÷ֱ��ʣ���bResolution[n]����֧�ֵ����֡��.����Ϊ0.							
	BYTE				byResolution_1[256];		//��������1,ͬ������˵��.
	char				reserved[28];
	BYTE				byImageSize[256];		// ֧�ֵķֱ�������,�μ�enum CAPTURE_SIZE
	BYTE				byImageSize_Assi[256][256]; //��ʾ������Ϊ���ֱ���ʱ��֧�ֵĸ������ֱ��ʣ������±��ʾ��������֧�ֵķֱ���ֵ��
}CFG_DSPENCODECAP_INFO;

// ��̨����
typedef enum tagEM_PTZ_ACTION {
    EM_PTZ_ACTION_UNKNOWN,                                      // δ֪
    EM_PTZ_ACTION_SCAN,                                         // ɨ��
    EM_PTZ_ACTION_PRESET,                                       // Ԥ�õ�
    EM_PTZ_ACTION_PATTERN,                                      // Ѳ��
    EM_PTZ_ACTION_TOUR,                                         // Ѳ��
} EM_PTZ_ACTION;

//-------------------------------¼������---------------------------------

// ʱ�����Ϣ
typedef struct tagCFG_TIME_SECTION 
{
	DWORD				dwRecordMask;						// ¼�����룬��λ�ֱ�Ϊ��̬���¼�񡢱���¼�񡢶�ʱ¼��Bit3~Bit15������Bit16��̬���ץͼ��Bit17����ץͼ��Bit18��ʱץͼ
	int					nBeginHour;
	int					nBeginMin;
	int					nBeginSec;
	int					nEndHour;
	int					nEndMin;
	int					nEndSec;
} CFG_TIME_SECTION;

// ʱ�����Ϣ
typedef struct tagCFG_TIME_SCHEDULE
{
    BOOL                bEnableHoliday;                     // �Ƿ�֧�ֽڼ������ã�Ĭ��Ϊ��֧�֣����ǻ�ȡ���ú󷵻�ΪTRUE����Ҫʹ�ܼ�������
	CFG_TIME_SECTION	stuTimeSection[MAX_TIME_SCHEDULE_NUM][MAX_REC_TSECT]; // ��һάǰ7��Ԫ�ض�Ӧÿ��7�죬��8��Ԫ�ض�Ӧ�ڼ��գ�ÿ�����6��ʱ���
} CFG_TIME_SCHEDULE;

// ��ʱ¼��������Ϣ
typedef struct tagCFG_RECORD_INFO
{
	int                 nChannelID;					// ͨ����(0��ʼ)
	CFG_TIME_SECTION	stuTimeSection[WEEK_DAY_NUM][MAX_REC_TSECT]; // ʱ���
	int					nPreRecTime;				// Ԥ¼ʱ�䣬Ϊ��ʱ��ʾ�ر�(0~300)
	BOOL				bRedundancyEn;				// ¼�����࿪��
	int					nStreamType;				// 0����������1��������1��2��������2��3��������3
	int					nProtocolVer;				// Э��汾��, ֻ��

	// ����
	BOOL				abHolidaySchedule;          // Ϊtrueʱ�м���������Ϣ��bHolidayEn��stuHolTimeSection����Ч;
    BOOL                bHolidayEn;                 // ����¼��ʹ��TRUE:ʹ��,FALSE:δʹ��
    CFG_TIME_SECTION    stuHolTimeSection[MAX_REC_TSECT];          // ����¼��ʱ���
} CFG_RECORD_INFO;

//-------------------------------��������---------------------------------

// ��̨��������
typedef enum tagCFG_LINK_TYPE
{
	LINK_TYPE_NONE,						    		// ������
	LINK_TYPE_PRESET,								// ����Ԥ�õ�
	LINK_TYPE_TOUR,									// ����Ѳ��
	LINK_TYPE_PATTERN,								// �����켣
} CFG_LINK_TYPE;

// ������̨��Ϣ
typedef struct tagCFG_PTZ_LINK
{
	CFG_LINK_TYPE		emType;						// ��������
	int					nValue;						// ����ȡֵ�ֱ��ӦԤ�õ�ţ�Ѳ���ŵȵ�
} CFG_PTZ_LINK;

// ������̨��Ϣ��չ
typedef struct tagCFG_PTZ_LINK_EX
{
	CFG_LINK_TYPE	emType;				// �������� 
	int			    nParam1;			// ��������1
	int			    nParam2;            // ��������2
	int			    nParam3;            // ��������3
	int			    nChannelID;         // ��������̨ͨ��
} CFG_PTZ_LINK_EX;

// �¼��������ݽṹ��
typedef struct tagCFG_EVENT_TITLE
{
	char				szText[MAX_CHANNELNAME_LEN];
	CFG_POLYGON			stuPoint;			// �������Ͻ�����, ����0-8191�������ϵ
	CFG_SIZE     		stuSize;			// ����Ŀ�Ⱥ͸߶�,����0-8191�������ϵ��ĳ���������Ϊ0��ʾ������������Ӧ���
    CFG_RGBA			stuFrontColor;		// ǰ����ɫ
    CFG_RGBA			stuBackColor;		// ������ɫ
} CFG_EVENT_TITLE;

// �ʼ���������
typedef enum tagCFG_ATTACHMENT_TYPE
{
		ATTACHMENT_TYPE_PIC,							// ͼƬ����
		ATTACHMENT_TYPE_VIDEO,							// ��Ƶ����
		ATTACHMENT_TYPE_NUM,							// ������������
} CFG_ATTACHMENT_TYPE;
// �ʼ���ϸ����
typedef struct tagCFG_MAIL_DETAIL
{
	   CFG_ATTACHMENT_TYPE emAttachType;                 // ��������
       int                 nMaxSize;                     // �ļ���С���ޣ���λkB
       int                 nMaxTimeLength;               // ���¼��ʱ�䳤�ȣ���λ�룬��video��Ч
}CFG_MAIL_DETAIL;

// �ָ�ģʽ
typedef enum tagCFG_SPLITMODE
{
	    SPLITMODE_1  = 1,						// 1����
	    SPLITMODE_2  = 2,						// 2����
	    SPLITMODE_4 = 4,						// 4����
		SPLITMODE_6 = 6,						// 6����
		SPLITMODE_8 = 8,						// 8����
		SPLITMODE_9 = 9,						// 9����
		SPLITMODE_12 = 12,				    	// 12����
		SPLITMODE_16 = 16,				    	// 16����
		SPLITMODE_20 = 20,				    	// 20����
		SPLITMODE_25 = 25,					    // 25����
		SPLITMODE_36 = 36,					    // 36����
		SPLITMODE_64 = 64,					    // 64����
		SPLITMODE_144 = 144,					// 144����
		SPLITMODE_PIP = 1000,                   // ���л��ָ�ģʽ����ֵ
		SPLITMODE_PIP1 = SPLITMODE_PIP + 1,		// ���л�ģʽ, 1��ȫ������+1��С���洰��
		SPLITMODE_PIP3 = SPLITMODE_PIP + 3,		// ���л�ģʽ, 1��ȫ������+3��С���洰��
		SPLITMODE_FREE = SPLITMODE_PIP * 2,	// ���ɿ���ģʽ���������ɴ������رմ��ڣ��������ô���λ�ú�Z�����
		SPLITMODE_COMPOSITE_1 = SPLITMODE_PIP * 3 + 1,	// �ں�����Ա1�ָ�
		SPLITMODE_COMPOSITE_4 = SPLITMODE_PIP * 3 + 4,	// �ں�����Ա4�ָ�
		SPLITMODE_3  = 10,						// 3����
		SPLITMODE_3B = 11,						// 3���浹Ʒ
		SPLITMODE_EOF,                          // ������ʶ
} CFG_SPLITMODE;

// ��Ѳ��������
typedef struct tagCFG_TOURLINK
{
	BOOL				bEnable;			             // ��Ѳʹ��
	CFG_SPLITMODE	    emSplitMode;		             // ��Ѳʱ�ķָ�ģʽ
	int			        nChannels[MAX_VIDEO_CHANNEL_NUM];  // ��Ѳͨ�����б�
	int			        nChannelCount;		             // ��Ѳͨ������
} CFG_TOURLINK;

// �Ž���������
enum EM_CFG_ACCESSCONTROLTYPE
{
	EM_CFG_ACCESSCONTROLTYPE_NULL = 0,					// ��������
	EM_CFG_ACCESSCONTROLTYPE_AUTO,						// �Զ�
	EM_CFG_ACCESSCONTROLTYPE_OPEN,						// ����
	EM_CFG_ACCESSCONTROLTYPE_CLOSE,						// ����
	EM_CFG_ACCESSCONTROLTYPE_OPENALWAYS,				// ��Զ����
	EM_CFG_ACCESSCONTROLTYPE_CLOSEALWAYS,				// ��Զ�ر�
};

// �Ž��������������

#define MAX_ACCESSCONTROL_NUM	8						// ����Ž������������


// �������з���
typedef enum
{
	EM_CALLER_DEVICE = 0,								// �豸����
}EM_CALLER_TYPE;

// ����Э��
typedef enum
{
	EM_CALLER_PROTOCOL_CELLULAR = 0,					// �ֻ���ʽ
}EM_CALLER_PROTOCOL_TYPE;


// ��������������Ϣ
typedef struct tagCFG_TALKBACK_INFO
{
	BOOL				bCallEnable;					// ��������ʹ��
	EM_CALLER_TYPE		emCallerType;					// �������з���
	EM_CALLER_PROTOCOL_TYPE	emCallerProtocol;			// ��������Э��
}CFG_TALKBACK_INFO;

// �绰��������������Ϣ
typedef struct tagCFG_PSTN_ALARM_SERVER
{
	BOOL				bNeedReport;						// �Ƿ��ϱ����绰��������
	int					nServerCount;						// �绰��������������					
	BYTE 				byDestination[MAX_PSTN_SERVER_NUM];	// �ϱ��ı��������±�,�������CFG_PSTN_ALARM_CENTER_INFO
}CFG_PSTN_ALARM_SERVER;

// ����������Ϣ
typedef struct tagCFG_ALARM_MSG_HANDLE
{
	//����
	bool				abRecordMask;
	bool				abRecordEnable;
	bool				abRecordLatch;
	bool				abAlarmOutMask;
	bool				abAlarmOutEn;
	bool				abAlarmOutLatch;	
	bool				abExAlarmOutMask;
	bool				abExAlarmOutEn;
	bool				abPtzLinkEn;
	bool				abTourMask;
	bool				abTourEnable;
	bool				abSnapshot;
	bool				abSnapshotEn;
	bool				abSnapshotPeriod;
	bool				abSnapshotTimes;
	bool				abTipEnable;
	bool				abMailEnable;
	bool				abMessageEnable;
	bool				abBeepEnable;
	bool				abVoiceEnable;
	bool				abMatrixMask;
	bool				abMatrixEnable;
	bool				abEventLatch;
	bool				abLogEnable;
	bool				abDelay;
	bool				abVideoMessageEn;
	bool				abMMSEnable;
	bool				abMessageToNetEn;
	bool				abTourSplit;
	bool				abSnapshotTitleEn;

    bool                abChannelCount;
	bool                abAlarmOutCount;
	bool                abPtzLinkEx;
	bool                abSnapshotTitle;
	bool                abMailDetail;
	bool                abVideoTitleEn;
	bool                abVideoTitle;
	bool                abTour;
	bool                abDBKeys;
	bool                abJpegSummary;
	bool                abFlashEn;
	bool                abFlashLatch;
	BYTE				byReserved1[2];                             // ����


	//��Ϣ
	int					nChannelCount;								 // �豸����Ƶͨ����
	int					nAlarmOutCount;								 // �豸�ı����������
	DWORD				dwRecordMask[MAX_CHANNEL_COUNT];			 // ¼��ͨ������(��λ)
	BOOL				bRecordEnable;								 // ¼��ʹ��
	int					nRecordLatch;								 // ¼����ʱʱ��(��)
	DWORD				dwAlarmOutMask[MAX_CHANNEL_COUNT];			 // �������ͨ������
	BOOL				bAlarmOutEn;								 // �������ʹ��
	int					nAlarmOutLatch;								 // ���������ʱʱ��(��)
	DWORD				dwExAlarmOutMask[MAX_CHANNEL_COUNT];		 // ��չ�������ͨ������
	BOOL				bExAlarmOutEn;								 // ��չ�������ʹ��
	CFG_PTZ_LINK		stuPtzLink[MAX_VIDEO_CHANNEL_NUM];			 // ��̨������
	BOOL				bPtzLinkEn;									 // ��̨����ʹ��
	DWORD				dwTourMask[MAX_CHANNEL_COUNT];				 // ��ѯͨ������
	BOOL				bTourEnable;								 // ��ѯʹ��
	DWORD				dwSnapshot[MAX_CHANNEL_COUNT];				 // ����ͨ��������
	BOOL				bSnapshotEn;								 // ����ʹ��
	int					nSnapshotPeriod;							 // ��������(��)
	int					nSnapshotTimes;								 // ���Ĵ���
	BOOL				bTipEnable;									 // ������Ϣ����ʾ
	BOOL				bMailEnable;								 // �����ʼ��������ͼƬ����Ϊ����
	BOOL				bMessageEnable;								  // �ϴ�������������
	BOOL				bBeepEnable;							 	  // ����
	BOOL				bVoiceEnable;								  // ������ʾ
	DWORD				dwMatrixMask[MAX_CHANNEL_COUNT];			  // ������Ƶ����ͨ������
	BOOL				bMatrixEnable;								  // ������Ƶ����
	int					nEventLatch;								  // ������ʼ��ʱʱ��(��)��0��15
	BOOL				bLogEnable;									  // �Ƿ��¼��־
	int					nDelay;										  // ����ʱ����ʱ����Ч����λΪ��
	BOOL				bVideoMessageEn;							  // ������ʾ��Ļ����Ƶ�����ӵ���Ļ�����¼����ͣ�ͨ���ţ����ʱ��
	BOOL				bMMSEnable;									  // ���Ͳ���ʹ��
	BOOL				bMessageToNetEn;							  // ��Ϣ�ϴ�������ʹ��
	int					nTourSplit;									  // ��Ѳʱ�ķָ�ģʽ 0: 1����; 1: 8����
	BOOL				bSnapshotTitleEn;							  // �Ƿ����ͼƬ����
	int                 nPtzLinkExNum;                                // ��̨������
	CFG_PTZ_LINK_EX     stuPtzLinkEx[MAX_VIDEO_CHANNEL_NUM];          // ��չ��̨��Ϣ
	int                 nSnapTitleNum;                                // ͼƬ����������
	CFG_EVENT_TITLE     stuSnapshotTitle[MAX_VIDEO_CHANNEL_NUM];      // ͼƬ��������
	CFG_MAIL_DETAIL     stuMailDetail;                                // �ʼ���ϸ����
	BOOL                bVideoTitleEn;                                // �Ƿ������Ƶ���⣬��Ҫָ������
    int                 nVideoTitleNum;                               // ��Ƶ����������Ŀ
	CFG_EVENT_TITLE     stuVideoTitle[MAX_VIDEO_CHANNEL_NUM];         // ��Ƶ��������
	int                 nTourNum;                                     // ��ѯ������Ŀ
	CFG_TOURLINK        stuTour[MAX_VIDEO_CHANNEL_NUM];               // ��ѯ��������
	int                 nDBKeysNum;			                          // ָ�����ݿ�ؼ��ֵ���Ч��
	char                szDBKeys[MAX_DBKEY_NUM][MAX_CHANNELNAME_LEN]; // ָ���¼���ϸ��Ϣ����Ҫд�����ݿ�Ĺؼ���
	BYTE                byJpegSummary[MAX_SUMMARY_LEN];               // ���ӵ�JPEGͼƬ��ժҪ��Ϣ
	BOOL                bFlashEnable;                                 // �Ƿ�ʹ�ܲ����
	int                 nFlashLatch;                                  // �������ʱʱ��(��),��ʱʱ�䷶Χ��[10,300]
	
	bool				abAudioFileName;
	bool				abAlarmBellEn;
	bool				abAccessControlEn;
	bool				abAccessControl;

	char				szAudioFileName[MAX_PATH];					// ���������ļ�����·��
	BOOL				bAlarmBellEn;								// ����ʹ��
	BOOL				bAccessControlEn;							// �Ž�ʹ��

	DWORD				dwAccessControl;							// �Ž�����
	EM_CFG_ACCESSCONTROLTYPE	emAccessControlType[MAX_ACCESSCONTROL_NUM];	// �Ž�����������Ϣ
	
	bool				abTalkBack;	
    BYTE				byReserved2[3];                             // ����
	CFG_TALKBACK_INFO	stuTalkback;								// ��������������Ϣ

	bool				abPSTNAlarmServer;
    BYTE				byReserved3[3];                             // ����
	CFG_PSTN_ALARM_SERVER	stuPSTNAlarmServer;						// �绰��������������Ϣ
    CFG_TIME_SCHEDULE       stuTimeSection;                         // �¼���Ӧʱ���
	bool				abAlarmBellLatch;
    BYTE				byReserved4[3];                             // ����
	int					nAlarmBellLatch;							// ���������ʱʱ��(10-300��)

    bool				abAudioPlayTimes;
    bool				abAudioLinkTime;
    BYTE				byReserved5[2];                             // ����
    UINT                nAudioPlayTimes;                            // �����������Ŵ���
    UINT                nAudioLinkTime;                             // �����������ŵ�ʱ��, ��λ����
} CFG_ALARM_MSG_HANDLE;



// ����ʹ�ܿ��Ʒ�ʽö������
typedef enum tagEM_CTRL_ENABLE
{
	EM_CTRL_NORMAL=0,   //������ʹ��
	EM_CTRL_ALWAYS_EN,  //����ʹ��
	EM_CTRL_ONCE_DIS,   //��·
	EM_CTRL_ALWAYS_DIS, //�Ƴ�
	EM_CTRL_NUM         //ö����������
}EM_CTRL_ENABLE;


// ��������
typedef enum tagEM_CFG_DEFENCEAREATYPE
{
	EM_CFG_DefenceAreaType_Unknown = 0,     // δ֪����
	EM_CFG_DefenceAreaType_InTime,          // ��ʱ���� 
	EM_CFG_DefenceAreaType_Delay,           // ��ʱ����
	EM_CFG_DefenceAreaType_FullDay,         // 24Сʱ����
    EM_CFG_DefenceAreaType_Follow,          // �������
    EM_CFG_DefenceAreaType_Medical,         // ҽ�ƽ�������
    EM_CFG_DefenceAreaType_Panic,           // �ֻŷ���
    EM_CFG_DefenceAreaType_Fire,            // �𾯷���
    EM_CFG_DefenceAreaType_FullDaySound,    // 24Сʱ��������
    EM_CFG_DefenceAreaType_FullDaySlient,   // 24Сʱ��������
    EM_CFG_DefenceAreaType_Entrance1,       // �������1
    EM_CFG_DefenceAreaType_Entrance2,       // �������2
    EM_CFG_DefenceAreaType_InSide,          // �ڲ�����
    EM_CFG_DefenceAreaType_OutSide,         // �ⲿ����
    EM_CFG_DefenceAreaType_PeopleDetect,    // ��Ա������
	EM_CFG_DefenceAreaType_Robbery,			// �˾�����
}EM_CFG_DEFENCEAREATYPE;

// �ⲿ��������
typedef struct tagCFG_ALARMIN_INFO
{
	int					nChannelID;									// ����ͨ����(0��ʼ)
	BOOL				bEnable;									// ʹ�ܿ���
	char				szChnName[MAX_CHANNELNAME_LEN];				// ����ͨ������
	int					nAlarmType;									// ���������ͣ�0�����գ�1������
	CFG_ALARM_MSG_HANDLE stuEventHandler;							// ��������
	CFG_TIME_SECTION	stuTimeSection[WEEK_DAY_NUM][MAX_REC_TSECT];// �¼���Ӧʱ��Σ�ʱ��λ�ȡ�������Դ˳�ԱΪ׼������ stuEventHandler �е� stuTimeSection
	BOOL				abDevID;									// 
	char				szDevID[MAX_NAME_LEN];						// �豸ID
	int                 nPole;                                      // ����������ģʽ, 0:����Ч��1����Ч��
	                                                                // �������Ϊ�������ӵ�or��Դ����nAlarmType���ʹ��
	EM_SENSE_METHOD     emSense;                                    // ��������Ӧ��ʽ
	EM_CTRL_ENABLE      emCtrl;                                     // ����ʹ�ܿ��Ʒ�ʽ
	int                 nDisDelay;                                  // ��ʱ����ʱ��,��������Ϊ"Delay"(��ʱ����)ʱ����Ч, ��λ: ��, ���ʱ��ͨ����ѯ�������
	                                                                // emCtrlֵΪEM_CTRL_NORMAL��EM_CTRL_ALWAYS_EN ʱ��Ч��

	EM_CFG_DEFENCEAREATYPE emDefenceAreaType;						// ��������, ����֧�ֵ�����ͨ����ѯ�������
	int					nEnableDelay;								// ��ʱ����ʱ��, ��������Ϊ"Delay"(��ʱ����)ʱ����Ч, ��λ: ��, ���ʱ��ͨ����ѯ�������    
    int                 nSlot;                                      // ����ַ, -1��ʾ��Ч, 0��ʾ����ͨ��, 1��ʾ�����ڵ�һ�������ϵ���չͨ��, 2��3...�Դ�����
    int                 nLevel1;                                    // ��һ��������ַ, ��ʾ�����ڵ�nSlot�����ϵĵ�nLevel1��̽�������Ǳ�, -1��ʾ��Ч, ��0��ʼ
    bool                abLevel2;                                   // ��ʾnLevel2�ֶ��Ƿ����
    int                 nLevel2;                                    // �ڶ���������ַ, ��ʾ�����ڵ�nLevel1�����Ǳ��ϵ�̽�������, -1��ʾ��Ч, ��0��ʼ
    int                 nDoorNotClosedTimeout;                      // ��δ�س�ʱʱ��,��λΪs,��Χ 15s-300s
} CFG_ALARMIN_INFO;

// �������뱨������
typedef struct tagCFG_NETALARMIN_INFO 
{
	int					nChannelID;									// ����ͨ����(0��ʼ)
	BOOL				bEnable;									// ʹ�ܿ���
	char				szChnName[MAX_CHANNELNAME_LEN];				// ����ͨ������
	int					nAlarmType;									// ���������ͣ�0�����գ�1������
	CFG_ALARM_MSG_HANDLE stuEventHandler;							// ��������
	CFG_TIME_SECTION	stuTimeSection[WEEK_DAY_NUM][MAX_REC_TSECT];// �¼���Ӧʱ��Σ�ʱ��λ�ȡ�������Դ˳�ԱΪ׼������ stuEventHandler �е� stuTimeSection
} CFG_NETALARMIN_INFO;


// ����֧�ֵ���Ƶ��������
typedef struct tagCFG_MOTION_WINDOW
{
	int                  nThreshold;           // �����ֵ��ȡֵ[0, 100]
	int                  nSensitive;           // �����ȣ�ȡֵ[0, 100]
	CFG_RECT             stuWindow;            // ���촰������, ����λ��ȡֵ[0, 8192)
}CFG_MOTION_WINDOW;

typedef struct tagCFG_DETECT_REGION
{
    int				     nRegionID;									    // ����ID
    char                 szRegionName[CFG_COMMON_STRING_64];            // ��̬��������
    int                  nThreshold;                                    // �����ֵ��ȡֵ[0, 100]
	int					 nSenseLevel;								    // ������1��6
	int					 nMotionRow;									// ��̬������������
	int					 nMotionCol;									// ��̬������������
	BYTE				 byRegion[MAX_MOTION_ROW][MAX_MOTION_COL];	    // ����������32*32������ 
}CFG_DETECT_REGION;

// ��̬��ⱨ������
typedef struct tagCFG_MOTION_INFO 
{
	int					 nChannelID;									// ����ͨ����(0��ʼ), nVersion=1ʱ�����ֶ���Ч
	BOOL				 bEnable;									    // ʹ�ܿ���
	int					 nSenseLevel;								    // һ������������1��6
	int					 nMotionRow;									// һ����̬������������
	int					 nMotionCol;									// һ����̬������������
	BYTE				 byRegion[MAX_MOTION_ROW][MAX_MOTION_COL];	    // һ������������32*32������
	CFG_ALARM_MSG_HANDLE stuEventHandler;							    // ��������
	CFG_TIME_SECTION	 stuTimeSection[WEEK_DAY_NUM][MAX_REC_TSECT];   // �¼���Ӧʱ��Σ�ʱ��λ�ȡ�������Դ˳�ԱΪ׼������ stuEventHandler �е� stuTimeSection
	int			         nVersion;                                      // 0, 1, ��������ȷ��, ֻ��, ����1ʱ�����ֶ���Ч
	BOOL                 bSenseLevelEn;                                 // ֻ��������nSenseLevel�Ƿ���Ч
	BOOL                 bVRatioEn;                                     // ֻ�������ռ�ñ�ʹ�ܣ� ����nVolumeRatio�Ƿ���Ч
	int                  nVolumeRatio;                                  // һ����������ռ�ñ�,1-100
	BOOL                 bSRatioEn;                                     // ֻ����������ʹ�ܣ�����nSubRatioֵ�Ƿ���Ч
	int                  nSubRatio;                                     // һ������Ĳв���ֵ, 1-100
	BOOL                 abWindow;                                      // ���ֶμ����������ֶ��ѷ���
	int                  nWindowCount;                                  // ��Ƶ���ڸ���
	CFG_MOTION_WINDOW    stuWindows[MAX_MOTION_WINDOW];                 // ��Ƶ��������
	BOOL                 abDetectRegion;                                // ֻ����1��nRegionCount��stuRegion��Ч
                                                                        //       0��nMotionRow��nMotionCol��byRegion��Ч
	int                  nRegionCount;                                  // ������̬����������
	CFG_DETECT_REGION    stuRegion[MAX_MOTION_WINDOW];                  // ������̬�������
} CFG_MOTION_INFO;

// ��Ƶ��ʧ��������
typedef struct tagCFG_VIDEOLOST_INFO 
{
	int					nChannelID;									// ����ͨ����(0��ʼ)
	BOOL				bEnable;									// ʹ�ܿ���
	CFG_ALARM_MSG_HANDLE stuEventHandler;							// ��������
	CFG_TIME_SECTION	stuTimeSection[WEEK_DAY_NUM][MAX_REC_TSECT];// �¼���Ӧʱ��Σ�ʱ��λ�ȡ�������Դ˳�ԱΪ׼������ stuEventHandler �е� stuTimeSection
} CFG_VIDEOLOST_INFO;

// ��Ƶ�ڵ���������
typedef struct tagCFG_SHELTER_INFO 
{
	int					nChannelID;									// ����ͨ����(0��ʼ)
	BOOL				bEnable;									// ʹ�ܿ���
	int					nSenseLevel;								// ������
	CFG_ALARM_MSG_HANDLE stuEventHandler;							// ��������
	CFG_TIME_SECTION	stuTimeSection[WEEK_DAY_NUM][MAX_REC_TSECT];// �¼���Ӧʱ��Σ�ʱ��λ�ȡ�������Դ˳�ԱΪ׼������ stuEventHandler �е� stuTimeSection
} CFG_SHELTER_INFO;

// �޴洢�豸��������
typedef struct tagCFG_STORAGENOEXIST_INFO 
{
	BOOL				bEnable;						// ʹ�ܿ���
	CFG_ALARM_MSG_HANDLE stuEventHandler;				// ��������
} CFG_STORAGENOEXIST_INFO;

// �洢�豸���ʳ���������
typedef struct tagCFG_STORAGEFAILURE_INFO 
{
	BOOL				bEnable;						// ʹ�ܿ���
	CFG_ALARM_MSG_HANDLE stuEventHandler;				// ��������
} CFG_STORAGEFAILURE_INFO;

// �洢�豸�ռ䲻�㱨������
typedef struct tagCFG_STORAGELOWSAPCE_INFO 
{
	BOOL				bEnable;						// ʹ�ܿ���
	int					nLowerLimit;					// Ӳ��ʣ���������ޣ��ٷ���(0~99)
	CFG_ALARM_MSG_HANDLE stuEventHandler;				// ��������
} CFG_STORAGELOWSAPCE_INFO;

// ����Ͽ���������
typedef struct tagCFG_NETABORT_INFO 
{
	BOOL				bEnable;						// ʹ�ܿ���
	CFG_ALARM_MSG_HANDLE stuEventHandler;				// ��������
} CFG_NETABORT_INFO;

// IP��ͻ��������
typedef struct tagCFG_IPCONFLICT_INFO 
{
	BOOL				bEnable;						// ʹ�ܿ���
	CFG_ALARM_MSG_HANDLE stuEventHandler;				// ��������
} CFG_IPCONFLICT_INFO;

// CLIENT_QueryNewSystemInfo �ӿڵ����� CFG_CAP_ALARM (��ȡ����������)��Ӧ�ṹ��

// ������������ʽ
typedef struct tagCFG_ALARM_SENSE_METHOD
{
	int                 nSupportSenseMethodNum;								// ֧�ֵĴ�������ʽ��
	EM_SENSE_METHOD     emSupportSenseMethod[MAX_ALARM_SENSE_METHOD_NUM];   // ֧�ֵĴ�������ʽ
}CFG_ALARM_SENSE_METHOD;

typedef struct tagCFG_CAP_ALARM_INFO
{
	BOOL                 bAlarmPir;									// �豸�Ƿ�֧��PIR(������ʽ��Ӧ��)����,�ⲿ������һ��
	BOOL                 bFlashLight;								// �豸�Ƿ�֧�ֲ����
	int					 nDefenceAreaTypeNum;						// ��������������,Ϊ0��ʾ�޴��ֶ�ʱ�������벻��Ϊ����ʹ�á�
	EM_CFG_DEFENCEAREATYPE	emDefenceAreaType[MAX_ALARM_DEFENCE_TYPE_NUM];			// ֧�ַ������� 
	int				 	 nMaxDelay;									// ��ʱ�����������ʱʱ��,��λΪ�룬ֻ��֧����ʱ����ʱ���ֶβ���Ч��
	int					 nAlarmChannelNum;							// ����ͨ����,Ϊ�˱��ּ�����,��Ϊ0ʱ��ʾÿ��ͨ��֧���������͵Ĵ�����
	CFG_ALARM_SENSE_METHOD stuAlarmChannel[MAX_ALARM_CHANNEL_NUM];	// ����ͨ������
	int					 nAlarmBellCount;							// ���Ÿ���
	int					 nMaxBackupAlarmServer;						// ����ñ���������,�޴��ֶ�ʱĬ��Ϊ0,0��ʾ��֧�ֱ��ñ�������
	int					 nMaxPSTNAlarmServer;						// ���绰����������, �޴��ֶ�ʱĬ��Ϊ0,0��ʾ��֧�ֵ绰�������ġ�
}CFG_CAP_ALARM_INFO;

// ���������
typedef struct tagCFG_FLASH_LIGHT
{
	BOOL                bEnable;                        // ʹ�ܿ���
	int                 nBrightness;                    // ���� 0~100
	CFG_TIME_SECTION	stuTimeSection[WEEK_DAY_NUM][MAX_REC_TSECT];// ����ƹ���ʱ���
}CFG_FLASH_LIGHT;

typedef struct tagCFG_CUSTOMCASE
{
	char                szCaseTitle[MAX_OSD_TITLE_LEN];    // ��������
	char                szCaseContent[MAX_OSD_SUMMARY_LEN];  // ��������
	BOOL                bCaseNoOsdEn;                        // ������ŵ���ʹ��
}CFG_CUSTOMCASE;

// ʱ��
typedef struct tagCFG_NET_TIME
{
    int                 nStructSize;
    DWORD				dwYear;					// ��
    DWORD				dwMonth;				// ��
    DWORD				dwDay;					// ��
    DWORD				dwHour;					// ʱ
    DWORD				dwMinute;				// ��
    DWORD				dwSecond;				// ��
} CFG_NET_TIME;

// ˾����¼����
typedef struct tagCFG_JUDICATURE_INFO
{
	char                szDiskPassword[MAX_PASSWORD_LEN];  // ��������(����, ʹ��szPassword��nPasswordLen)
	char                szCaseNo[MAX_OSD_SUMMARY_LEN];     // �������
	BOOL                bCaseNoOsdEn;                      // ������ŵ���ʹ��
	char                szCaseTitle[MAX_OSD_SUMMARY_LEN];  // ��������
	BOOL                bCaseTitleOsdEn;                   // �������Ƶ���ʹ��
	char                szOfficer[MAX_OSD_SUMMARY_LEN];    // �참��Ա
	BOOL                bOfficerOsdEn;                     // �참��Ա����ʹ��
	char                szLocation[MAX_OSD_SUMMARY_LEN];   // �참�ص�
	BOOL                bLocationOsdEn;                    // �참�ص����ʹ��
	char                szRelatedMan[MAX_OSD_SUMMARY_LEN]; // �永��Ա
	BOOL                bRelatedManOsdEn;                  // �永��Ա����ʹ��
	char                szDiskNo[MAX_OSD_SUMMARY_LEN];     // ���̱��
	BOOL                bDiskNoOsdEn;                      // ���̱�ŵ���ʹ��

	BOOL                bCustomCase;                       // TRUE:�Զ���˾��������Ϣ,FALSE: �ϱ�szCaseNo���ֶ���Ч
	int                 nCustomCase;                       // ʵ��CFG_CUSTOMCASE����
	CFG_CUSTOMCASE      stuCustomCases[MAX_CUSTOMCASE_NUM];// �Զ���˾��������Ϣ

	BOOL                bDataCheckOsdEn;                   // ���̿�¼����У������ ����ʹ��
	BOOL                bAttachFileEn;                     // �����ϴ�ʹ��
    char                szPassword[MAX_PASSWORD_LEN];      // ����, ��¼����ʱ�����ö���������
    int                 nPasswordLen;                      // ���볤��
    CFG_NET_TIME        stuStartTime;                      // Ƭͷ��Ϣ���ӿ�ʼʱ��
    int                 nPeriod;                           // Ƭͷ��Ϣ����ʱ�䳤�ȣ���λ������
}CFG_JUDICATURE_INFO;

// ��¼���¼�����
typedef struct tagCFG_BURNFULL_ONE
{
	char                szBurnDisk[MAX_NAME_LEN];          // ��¼�豸
	BOOL                bEnable;                           // ʹ�ܿ���
	unsigned int        nLowerLimit;                       // ����ʣ����������	��λ��M
	BOOL                bBurnStop;                         // ��¼ֹͣʹ��
	CFG_ALARM_MSG_HANDLE stuEventHandler;							// ��������
	CFG_TIME_SECTION	stuTimeSection[WEEK_DAY_NUM][MAX_REC_TSECT];// �¼���Ӧʱ���
    BOOL                bChangeDisk;                       // ����ʹ��
}CFG_BURNFULL_ONE;

typedef struct tagCFG_BURNFULL_INFO
{
	unsigned int        nBurnDev;                          // ʵ�ʿ�¼�豸����
	CFG_BURNFULL_ONE    stuBurns[MAX_BURNING_DEV_NUM];     // ÿ��Ԫ�ض�Ӧһ����¼�豸
}CFG_BURNFULL_INFO;

//-------------------------------ץͼ��������---------------------------------
// ץͼ��������
typedef struct tagCFG_SNAPCAPINFO_INFO  
{
	int					nChannelID;						// ץͼͨ����(0��ʼ)
	DWORD				dwIMageSizeNum;					// ֧�ֵķֱ�����Ϣ
	CFG_CAPTURE_SIZE	emIMageSizeList[MAX_IMAGESIZE_NUM];
	DWORD				dwFramesPerSecNum;				// ֧�ֵ�֡����Ϣ
	int					nFramesPerSecList[MAX_FPS_NUM];
	DWORD				dwQualityMun;					// ֧�ֵĻ�����Ϣ
	CFG_IMAGE_QUALITY	emQualityList[MAX_QUALITY_NUM];

	DWORD               dwMode;							// ģʽ,��λ����һλ����ʱ���ڶ�λ���ֶ���
	DWORD               dwFormat;						// ͼƬ��ʽģʽ,��λ����һλ��bmp���ڶ�λ��jpg��
} CFG_SNAPCAPINFO_INFO;


//-------------------------------����洢����������---------------------------------
// ����洢����������
typedef struct tagCFG_CHANNEL_TIME_SECTION 
{
	CFG_TIME_SECTION stuTimeSection[WEEK_DAY_NUM][MAX_NAS_TIME_SECTION];//�洢ʱ���
} CFG_CHANNEL_TIME_SECTION;

typedef struct tagCFG_NAS_INFO
{
	BOOL						bEnable;									// ʹ�ܿ���
	int							nVersion;									// ����洢�������汾0=�ϵ�FTP��1=NAS�洢
	int							nProtocol;									// Э������0=FTP 1=SMB
	char						szAddress[MAX_ADDRESS_LEN];					// IP��ַ��������
	int							nPort;										// �˿ں�
	char						szUserName[MAX_USERNAME_LEN];				// �ʻ���
	char						szPassword[MAX_PASSWORD_LEN];				// ����
	char						szDirectory[MAX_DIRECTORY_LEN];				// �����Ŀ¼��
	int							nFileLen;									// �ļ�����
	int							nInterval;									// �����ļ�ʱ����
	CFG_CHANNEL_TIME_SECTION	stuChnTime[MAX_VIDEO_CHANNEL_NUM];			// �洢ʱ���
	int                         nChnTimeCount;                              // ��Ч�Ĵ洢ʱ�����
} CFG_NAS_INFO;

//------------------------------��̨����--------------------------------------------
// Э����
typedef struct tagCFG_PRONAME
{
	char				name[MAX_NAME_LEN];			// Э����
} CFG_PRONAME;

// ���ڻ�������
typedef struct tagCFG_COMM_PROP
{
	BYTE				byDataBit;					// ����λ��0��5��1��6��2��7��3��8
	BYTE				byStopBit;					// ֹͣλ��0��1λ��1��1.5λ��2��2λ
	BYTE				byParity;					// У��λ��0����У�飬1����У�飻2��żУ��
	BYTE				byBaudRate;					// �����ʣ�0��300��1��600��2��1200��3��2400��4��4800��													
	                                                // 5��9600��6��19200��7��38400��8��57600��9��115200
} CFG_COMM_PROP;

// ��λԤ�õ�����
typedef struct tagCFG_PRESET_HOMING
{
	int          nPtzPresetId;   //��̨Ԥ�õ���	0~65535
 //-1��ʾ��Ч
	int          nFreeSec;       //���е�ʱ�䣬��λΪ��
}CFG_PRESET_HOMING;

// ��̨����
typedef struct tagCFG_PTZ_INFO
{
	// ����
	bool				abMartixID;
	bool				abCamID;
	bool				abPTZType;

	// ��Ϣ
	int					nChannelID;								// ͨ����(0��ʼ)
	BOOL                bEnable;                                // ʹ�ܿ���
	char                szProName[MAX_NAME_LEN];                // Э������
	int					nDecoderAddress;						// ��������ַ��0 - 255
	CFG_COMM_PROP		struComm;
	int					nMartixID;								// �����
	int					nPTZType;								// ��̨����0-���ݣ�������̨ 1-Զ��������̨
	int					nCamID;									// ����ͷID
	int                 nPort;                                  // ʹ�õĴ��ڶ˿ں�
	CFG_PRESET_HOMING   stuPresetHoming;                        // һ��ʱ�䲻������̨���Զ���λ��ĳ��Ԥ�õ�
	int                 nControlMode;                           // ����ģʽ, 0:"RS485"���ڿ���(Ĭ��);1:"Coaxial" ͬ��ڿ���
} CFG_PTZ_INFO;  

//��ʱ��������
typedef enum tagCFG_PTZ_FUNC_TYPE
{
	FUNC_TYPE_TOUR = 0 ,		//Ѳ��
	FUNC_TYPE_PATTERN  ,		//�Զ�Ѳ��
	FUNC_TYPE_PRE	   ,		//Ԥ�õ�
	FUNC_TYPE_SCAN	   ,		//�Զ���ɨ
	FUNC_TYPE_MAX	   ,		
}CFG_PTZ_FUNC_TYPE;


//�Զ���λ
typedef struct tagCFG_AUTO_HOMING
{
	BOOL	bEnable;		// �Զ���λ����
	int		nTime;			// ����ʱ��,��ʾ���е�ʱ�䣬��λΪ��
}CFG_AUTO_HOMING;

//��ʱ��������
typedef struct tagCFG_PTZ_PER_AUTOMOVE_INFO
{
	BOOL				bEnable;				//	��ʱ�������ر�־ TRUE ����FALSE ��
	CFG_TIME_SCHEDULE	stTimeSchedule;			//  ʱ�������
	CFG_PTZ_FUNC_TYPE	emFuncType;				//  ��ʱ����
	int					nTourId;				// 	Ѳ�����,		��Χ����CFG_PTZ_PROTOCOL_CAPS_INFO��wTourMin��wTourMax
	int					nPatternId;				// 	�Զ�Ѳ�����,	��Χ����CFG_PTZ_PROTOCOL_CAPS_INFO��wPatternMin��wPatternMax
	int					nPresetId;				//  Ԥ�õ���,		��Χ����CFG_PTZ_PROTOCOL_CAPS_INFO��wPresetMin��wPresetMax
	int					nScanId;				//	�Զ���ɨ���,	��Χ����CFG_PTZ_PROTOCOL_CAPS_INFO��wAutoScanMin��wAutoScanMax
	CFG_AUTO_HOMING		stAutoHoming;			//  �Զ���λ,һ��ʱ�䲻������̨���Զ���λ����ʱ����ڵĶ���
	BOOL				bSnapshotEnable;		//	Ԥ�õ����ʹ��,ֻ�е�emFuncTypeΪFUNC_TYPE_PREʱ����Ч
	int					nSnapshortDelayTime;	//	Ԥ�õ�ץ����ʱʱ��,��λ��,ֻ�е�emFuncTypeΪFUNC_TYPE_PREʱ����Ч
}CFG_PTZ_PER_AUTOMOVE_INFO;

typedef struct tagCFG_PTZ_ALL_AUTOMOVE_INFO
{
	CFG_PTZ_PER_AUTOMOVE_INFO stPTZPerInfo[MAX_CONFIG_NUM];		//������Ϣ
	int						  nCfgNum;							//��ȡ�������ø���
}CFG_PTZ_ALL_AUTOMOVE_INFO;

//��̨��ʱ�������ã�ע�⣬�˽ṹ����豸
typedef struct tagCFG_PTZ_AUTOMOVE_INFO
{
	CFG_PTZ_ALL_AUTOMOVE_INFO *pstPTZAutoConfig;				//��̨��������Ϣ
	int						   nMaxPTZNum;						//�ṹ���������̨����(���ڶ�ͨ����ѯ�����벻С���豸ͨ�������ڵ�ͨ����ѯ��һ���͹���)
	int						   nReturnPTZNum;					//�豸���ص���̨����(һ��Ϊ�豸ͨ����)
}CFG_PTZ_AUTOMOVE_INFO;

//----------------------------------��Ƶ�����豸����------------------------------------------
// ������������
typedef enum tagEM_FACEFEATURE_TYPE
{
	EM_FACEFEATURE_UNKNOWN,			// δ֪
	EM_FACEFEATURE_SEX,				// �Ա�
	EM_FACEFEATURE_AGE,				// ����
	EM_FACEFEATURE_EMOTION,			// ����
	EM_FACEFEATURE_GLASSES,			// �۾�״̬
	EM_FACEFEATURE_RACE,			// ����
	EM_FACEFEATURE_EYE,				// �۾�״̬
	EM_FACEFEATURE_MOUTH,			// ���״̬
	EM_FACEFEATURE_MASK,			// ����״̬
	EM_FACEFEATURE_BEARD,			// ����״̬
	EM_FACEFEATURE_ATTRACTIVE,		// ����ֵ
} EM_FACEFEATURE_TYPE;

// ����ͳ�ƴ��ೡ��
typedef struct tagCFG_NUMBER_STAT_INFO
{
	UINT					   nCameraType;		// ������ͣ�0����ʾ�ϵ�Ŀ���� 1����ʾ��ͳһ��Ŀ���� 2����ʾ��ͳһ˫Ŀ����
}CFG_NUMBER_STAT_INFO;

// ������ͳ��
typedef struct tagCFG_NUMBERSTAT_RULE_INFO
{
	UINT					nMaxRules;			// ֧�ֹ����������
}CFG_NUMBERSTAT_RULE_INFO;

// �ŶӼ��
typedef struct tagCFG_QUEUEDETECTION_RULE_INFO
{
	UINT					nMaxRules;			// ֧�ֹ����������
}CFG_QUEUEDETECTION_RULE_INFO;

// ����������ͳ��
typedef struct tagCFG_MANNUMDETECTION_RULE_INFO
{
	UINT					nMaxRules;			// ֧�ֹ����������
}CFG_MANNUMDETECTION_RULE_INFO;


// �����⼰����ʶ��֧�ֵ��������������ƣ�
typedef enum tagCFG_EM_FACE_TRAIT
{
	CFG_EM_FACE_TRAIT_UNKNOWN,							// δ֪
	CFG_EM_FACE_TRAIT_SEX,								// �Ա�					
	CFG_EM_FACE_TRAIT_AGE,								// ����
	CFG_EM_FACE_TRAIT_GLASSES,							// �۾�
	CFG_EM_FACE_TRAIT_BEARD,							// ����
	CFG_EM_FACE_TRAIT_COMPLEXION,						// ��ɫ
	CFG_EM_FACE_TRAIT_MASK,								// ����
	CFG_EM_FACE_TRAIT_EMOTION,							// ����
}CFG_EM_FACE_TRAIT;


// �����⼰����ʶ��֧�ֵ��������������ƣ�
typedef enum tagCFG_EM_BODY_TRAIT
{
	CFG_EM_BODY_TRAIT_UNKNOWN,							// δ֪
	CFG_EM_BODY_TRAIT_SEX,								// �Ա�					
	CFG_EM_BODY_TRAIT_AGEGROUP,							// ������
	CFG_EM_BODY_TRAIT_COATTYPE,							// ����
	CFG_EM_BODY_TRAIT_TROUSERSTYPE,						// ����
	CFG_EM_BODY_TRAIT_ANGLE,							// ����Ƕ�
	CFG_EM_BODY_TRAIT_UPPERPATTERN,						// ����ģʽ
	CFG_EM_BODY_TRAIT_TROUSERSCOLOR,					// ������ɫ
	CFG_EM_BODY_TRAIT_UMBRELLA,							// ��ɡ				
	CFG_EM_BODY_TRAIT_UMBRELLACOLOR,					// ��ɡ��ɫ
	CFG_EM_BODY_TRAIT_HOLD,								// ��ǰ������
	CFG_EM_BODY_TRAIT_TROUSERSPATTERN,					// ����ģʽ
	CFG_EM_BODY_TRAIT_HATTYPE,							// ñ�ӿ�ʽ
	CFG_EM_BODY_TRAIT_HATCOLOR,							// ñ����ɫ
	CFG_EM_BODY_TRAIT_UPPERTYPE,						// ���¿�ʽ
	CFG_EM_BODY_TRAIT_COATCOLOR,						// ������ɫ
	CFG_EM_BODY_TRAIT_HAIRSTYLE,						// ����
	CFG_EM_BODY_TRAIT_HAIRCOLOR,						// ͷ����ɫ				
	CFG_EM_BODY_TRAIT_SHOESTYPE,						// Ь�ӿ�ʽ
	CFG_EM_BODY_TRAIT_SHOESCOLOR,						// Ь����ɫ
	CFG_EM_BODY_TRAIT_BAG,								// �����ʽ
	CFG_EM_BODY_TRAIT_BAGCOLOR,							// �����ɫ
	CFG_EM_BODY_TRAIT_MASK,								// ����
	CFG_EM_BODY_TRAIT_MASKCOLOR,						// ������ɫ
}CFG_EM_BODY_TRAIT;

// �����⼰����ʶ��֧�ֵ�����ץ�Ĳ��ԣ����ƣ�
typedef enum tagCFG_EM_FACE_SNAP_POLICY
{
	CFG_EM_FACE_SNAP_POLICY_UNKNOWN,					// δ֪
	CFG_EM_FACE_SNAP_POLICY_REALTIME,					// ʵʱץ��ģʽ(��ǰ��ͨ��ģʽ��ʵʱ������)
	CFG_EM_FACE_SNAP_POLICY_OPTIMAL,					// ��ѡץ��ģʽ�����趨����ʱ����(OptimalTime)����ѡ������ߵ�ץͼ��׼ȷ�����ȵ���ʱ�ϴ�
	CFG_EM_FACE_SNAP_POLICY_QUALITY,					// ����ץ��ģʽ����Optimal�Ļ����ϣ����ͼƬ����������ֵ��ǰ������ѡ�����ʵʱ��
	CFG_EM_FACE_SNAP_POLICY_RECOGNITION,				// ʶ������ץ��ģʽ������ѡʱ���ڣ���һ�����֡����ν��бȶԣ�һ���ȶԳɹ�������������ѡ������߶Աȳɹ��ʣ�ȡ����������ģʽ��
	CFG_EM_FACE_SNAP_POLICY_QUICK,						// ������ѡ���Ӽ�⵽����/���忪ʼ��ץ��һ��֡���ڵ�������õ�����������,����
	CFG_EM_FACE_SNAP_POLICY_FULLTRACK,					// ȫ����ѡ��ץ��ȫ��������õ��������壬����
	CFG_EM_FACE_SNAP_POLICY_INTERVAL,					// ���ץ�ģ�����
	CFG_EM_FACE_SNAP_POLICY_SINGLE,						// ����ģʽ���������Ž�������
	CFG_EM_FACE_SNAP_POLICY_PRECISION,					// �߾���ģʽ����ǿ����ʶ�𣬶���
}CFG_EM_FACE_SNAP_POLICY;

// �����������������ƣ�
typedef struct tagFACEBODY_DETECT_CAPS
{
	CFG_EM_FACE_TRAIT				szFaceFeatureList[32];			// ֧�ּ�����������
	unsigned int					nFaceFeature;					// ���ص��������Ը���
	CFG_EM_BODY_TRAIT				szBodyFeatureList[32];			// ֧�ּ�����������
	unsigned int					nBodyFeature;					// ���ص��������Ը���
	CFG_EM_FACE_SNAP_POLICY			szSnapPolicyList[16];			// ֧��ץ������
	unsigned int					nSnapPolicy;					// ���ص��������Ը���	
	BYTE							byReserved[256];				// Ԥ���ֶ�
}FACEBODY_DETECT_CAPS;

// ����ʶ�������������ƣ�
typedef struct tagFACEBODY_ANALYSE_CAPS
{
	CFG_EM_FACE_TRAIT				szFaceFeatureList[32];			// ֧�ּ�����������
	unsigned int					nFaceFeature;					// ���ص��������Ը���
	CFG_EM_BODY_TRAIT				szBodyFeatureList[32];			// ֧�ּ�����������
	unsigned int					nBodyFeature;					// ���ص��������Ը���
	CFG_EM_FACE_SNAP_POLICY			szSnapPolicyList[16];			// ֧��ץ������
	unsigned int					nSnapPolicy;					// ���ص��������Ը���
	BYTE							byReserved[256];				// Ԥ���ֶ�
}FACEBODY_ANALYSE_CAPS;

// ��ѡ����дģʽ
typedef enum tagEM_SUPPORT_CLOSE_UP_TYPE
{
    EM_SUPPORT_CLOSE_UP_TYPE_UNKNOWN,                               // δ֪
    EM_SUPPORT_CLOSE_UP_TYPE_TRACK_MODE,                            // ����ģʽ
    EM_SUPPORT_CLOSE_UP_TYPE_FIXED_MODE,                            // �̶�ģʽ
    EM_SUPPORT_CLOSE_UP_TYPE_DESIGNED_REGION_MODE,                  // ָ������ģʽ
} EM_SUPPORT_CLOSE_UP_TYPE;
// ���װ�������б�
typedef enum tagEM_FEATURE_LIST_TYPE
{
	EM_FEATURE_LIST_UNKNOWN,		// δ֪
	EM_FEATURE_LIST_HAS_MASK,		// �Ƿ������
	EM_FEATURE_LIST_HAS_CHEF_HAT,	// �Ƿ����ʦñ
	EM_FEATURE_LIST_HAS_CHEF_CLOTHES,//�Ƿ񴩳�ʦ��
	EM_FEATURE_LIST_CHEF_CLOTHES_COLOR,	// ��ʦ������ɫ
}EM_FEATURE_LIST_TYPE;

// ������Ҫ��ĳ�ʦ����ɫ������(�޴��ֶα�ʾ������ʦ����ɫ)
typedef enum tagEM_SUPPORTED_COLOR_LIST_TYPE
{
	EM_SUPPORTED_COLOR_LIST_TYPE_UNKNOWN,	// δ֪
	EM_SUPPORTED_COLOR_LIST_TYPE_BLACK,		// ��ɫ
	EM_SUPPORTED_COLOR_LIST_TYPE_WHITE,		// ��ɫ
	EM_SUPPORTED_COLOR_LIST_TYPE_RED,		// ��ɫ
}EM_SUPPORTED_COLOR_LIST_TYPE;

// ����֧�ֵĹ���
typedef struct
{
	DWORD             	dwSupportedRule;                                    		// ��������
	int               	nSupportedObjectTypeNum;                        			// ֧�ֵļ���������͸���
	char              	szObjectTypeName[MAX_OBJECT_LIST_SIZE][MAX_NAME_LEN];		// ֧�ֵļ�����������б�
	int               	nSupportedActionsNum;                                    	// ֧�ֵļ�����嶯������
	char              	szSupportedActions[MAX_ACTION_LIST_SIZE][MAX_NAME_LEN];  	// ֧�ֵļ�����嶯���б�
	int               	nSupportedDetectTypeNum;                                 	// ֧�ֵļ�����͸���
	char              	szSupportedDetectTypes[MAX_ACTION_LIST_SIZE][MAX_NAME_LEN];	// ֧�ֵļ�������б�
	int               	nSupportFaceTypeNum;                                       	// ֧�ֵ�����������͸���
	char              	szSupportFaceTypes[MAX_HUMANFACE_LIST_SIZE][MAX_NAME_LEN]; 	// ֧�ֵ�������������б�
	BOOL              	bTriggerPosition;                                        	// �Ƿ�֧�ִ�������λ��
	BOOL			  	bTriggerTrack;												// �Ƿ�֧�ִ�������
	BOOL			  	bFaceDetection;												// �Ƿ��������ÿ�����������ʶ��
	int				  	nSupportFeactureNum;										// ֧�ֵ������������͸���
	EM_FACEFEATURE_TYPE	emFeatureList[MAX_FEATURE_LIST_SIZE];						// ֧�ֵ��������������б�
    int                 nMaxRules;                                                  // ֧�ֹ����������
	FACEBODY_DETECT_CAPS			stuFaceBodyDetectCaps;							// ���������������ƣ�
	FACEBODY_ANALYSE_CAPS			stuFaceBodyAnalyseCaps;							// ����ʶ�����������ƣ�
    UINT			  	nFeatureSupport;											// �Ƿ���������FeatureEnable,0-��֧��(Ĭ��)1-֧��(���ڰ��߼���¼��;����������¼�)
    BOOL                bSupportMinRegion;                                          // ��Ա�ۼ�����Ƿ�֧��"��С�ۼ�����"����,Ĭ��TRUE
    int                              nSupportCloseUp;                               // ֧�ֵ�������д����� 0:��ʾû����ֶλ��߲�֧�֣�1��ʾ֧��
    int                              nSupportCloseUpTypeNum;                        // ��дģʽ����
    EM_SUPPORT_CLOSE_UP_TYPE         emSupportCloseUpType[32];                      // ��ѡ����дģʽ
	EM_FEATURE_LIST_TYPE emFeature[32];												// ���װ�������б�
	int					 nDetectFeatureCount;										// ��⵽����������
	EM_SUPPORTED_COLOR_LIST_TYPE emSupportedColorList[8];							// ��⵽�ĳ�ʦ��װ��ɫ�б�
	int							 nDetectSupportedColorCount;						// ��⵽�ĳ�ʦ��װ��ɫ����
}SCENE_SUPPORT_RULE;

// �궨����������Ϣ
typedef struct tagCFG_CAP_CELIBRATE_AREA
{
	EM_CALIBRATEAREA_TYPE  emType;												 // �궨��������
	BYTE                   byMaxHorizontalStaffNum;                              // ֧�ֵ�ˮƽ���������
	BYTE                   byMinHorizontalStaffNum;                              // ֧�ֵ�ˮƽ�����С����
	BYTE                   byMaxVerticalStaffNum;                                // ֧�ֵĴ�ֱ���������
	BYTE                   byMinVerticalStaffNum;                                // ֧�ֵĴ�ֱ�����С����
}CFG_CAP_CELIBRATE_AREA;

// �������໮������
typedef enum tagEM_VEHICLE_TYPE_SPLIT
{
    EM_VEHICLE_TYPE_SPLIT_UNKNOWN,              // δ֪
    EM_VEHICLE_TYPE_SPLIT_FUNC,                 // �����ܻ���
    EM_VEHICLE_TYPE_SPLIT_STANDARD,             // ����׼����
} EM_VEHICLE_TYPE_SPLIT;

//��˽��������������
typedef enum tagEM_PRIVACY_MASK_TYPE
{
    EM_PRIVACY_MASK_UNKNOWN,		// δ֪����
    EM_PRIVACY_MASK_HUMAN_FACE,		// ����
    EM_PRIVACY_MASK_HUMAN,			// ����
}EM_PRIVACY_MASK_TYPE;

// ��������
typedef struct tagCFG_CAP_SCENE
{
    char               szSceneName[MAX_PATH];                                   // ��������
    int                nSupportedObjectTypeNum;                                 // ��ǰ��������֧�ֵļ���������͸���
    char               szObjectTypeName[MAX_OBJECT_LIST_SIZE][MAX_NAME_LEN];	// ��ǰ��������֧�ֵļ�����������б�
    int                nSupportRules;                                           // ֧�ֵĹ������
    SCENE_SUPPORT_RULE stSpportRules[MAX_RULE_LIST_SIZE];                       // ֧�ֵĹ����б�

    //֧�ֵ�ģ�����
    bool               bDisturbance;                                            // �Ƿ�֧���Ŷ�ǿ������
    bool               bAntiDisturbance;                                        // �Ƿ�֧��ȥ�Ŷ�����
    bool               bBacklight;                                              // �Ƿ�֧����⴦��
    bool               bShadow;                                                 // �Ƿ�֧����Ӱ����
    bool               bAccuracy;                                               // �Ƿ�֧�ּ�⾫��
    bool               bMovingStep;                                             // �Ƿ�֧�ּ�ⲽ��
    bool               bScalingFactor;                                          // �Ƿ�֧�ּ������
    bool               bThresholdY;                                             // �Ƿ�֧��Y�����ж���ֵ
    bool               bThresholdUV;                                            // �Ƿ�֧��UV�����ж���ֵ
    bool               bThresholdEdge;                                          // �Ƿ�֧�ֱ�Ե����ж���ֵ
    bool               bDetectBalance;                                          // �Ƿ�֧�ּ��ƽ��
    bool               bAlgorithmIndex;                                         // �Ƿ�֧���㷨���
    bool               bHighlight;                                              // �Ƿ�֧�ָ߹⴦����Backgroud���� 
    bool               bSnapshot;                                               // �Ƿ�֧������ץͼ
    bool               bDetectRegion;                                           // �Ƿ�֧�ּ����������
    bool               bExcludeRegion;                                          // �Ƿ�֧���ų���������
    bool               bContourAssistantTrack;                                  // �Ƿ�֧��������������

    //֧�ֵĳ�������
    bool               bCameraAspect;                                            // �Ƿ�����ͷλ�ò���
    bool               bJitter;                                                  // �Ƿ�֧�ֶ�������
    bool               bDejitter;                                                // �Ƿ�֧��ȥ�����������

    // ֧�ֵı궨������
    int               nMaxCalibrateAreaNum;                                      // ���궨�������
    int               nCalibrateAreaNum;                                         // �궨����������Ϣ����
    CFG_CAP_CELIBRATE_AREA stCalibrateAreaCap[MAX_CALIBRATEAREA_TYPE_NUM];       // �궨����������Ϣ

    // �ߴ������������ʽ
    bool              bPixel;                                                    // �Ƿ�֧�����ؼ�����ʽ
    bool              bMetric;                                                   // �Ƿ�֧��ʵ�ʳ��ȼ�����ʽ
    bool              bCalibratedPixel;                                          // �Ƿ�֧��Զ�˽��˱궨������ؼ�����ʽ

    char              szSubType[MAX_SCENE_SUBTYPE_NUM][MAX_SCENE_SUBTYPE_LEN];  // ��ǰ������֧�ֵ��ӳ�������
    int               nSubTypeNum;                                              // �ӳ������͸���
    int               nMaxRules;                                                // ÿ������֧�ֵ�����������
    BOOL              bSupportedSetModule;                                      // �Ƿ�֧���ؽ�������Ĭ�ϲ�֧��(false)
    BOOL              bSupportFightCalibrate;                                   // �Ƿ�֧�ִ�ܱ궨����,Ĭ��FALSE
    UINT              nVehicleTypeSplitNum;                                     // �������໮�����͸���
    EM_VEHICLE_TYPE_SPLIT emVehicleTypeSplit[16];                               // �������໮������

    BOOL              bPrivacyMaskSupport;                                      // �Ƿ�֧����˽����, false-��֧��(Ĭ��);true-֧��
    UINT              nPrivacyMaskCount;                                        // ��˽�������������͵�����
    EM_PRIVACY_MASK_TYPE emPrivacyMaskTypes[32];                                // ��˽�������������� 
	char				 szClassAlias[16];										// ����ҵ�񷽰�����
	BYTE              byReserved[804];                                          // �����ֽ�
}CFG_CAP_SCENE;

// �����б�
typedef struct tagCFG_VACAP_SUPPORTEDSCENES
{
    int                     nScenes;                                                // ֧�ֵĳ�������
    CFG_CAP_SCENE           stScenes[MAX_SCENE_LIST_SIZE];                          // ֧�ֵĳ����б�
    CFG_NUMBER_STAT_INFO    stuNumberStat;                                          // ����ͳ�ƴ��ೡ��
}CFG_VACAP_SUPPORTEDSCENES;

#define CFG_DEVICE_ANALYSE_INFO CFG_CAP_DEVICE_ANALYSE_INFO
//��Ƶ�����豸������Ϣ
typedef struct tagCFG_CAP_DEVICE_ANALYSE_INFO
{
	int                nMaxChannels;											 // ֧�����ܷ��������ͨ����
}CFG_CAP_DEVICE_ANALYSE_INFO;

//�豸����
typedef enum tagNET_EM_DEVICE_TYPE
{
	NET_EM_DEVICE_UNKNOWN,						// δ֪����
	NET_EM_DEVICE_CAMERA,						// ģ�������
	NET_EM_DEVICE_DOME,							// ģ�����
	NET_EM_DEVICE_MATRIX,						// ģ�����
	NET_EM_DEVICE_IPC,							// ���������
	NET_EM_DEVICE_NVS,							// ��Ƶ������
	NET_EM_DEVICE_SD,							// �������
	NET_EM_DEVICE_ITSE,							// ���ܴ洢��
	NET_EM_DEVICE_ITC,							// ���������
	NET_EM_DEVICE_TPC,							// �ȳ��������
	NET_EM_DEVICE_DVR,							// ����Ӳ��¼���
	NET_EM_DEVICE_HDVR,							// ���������Ӳ��¼���
	NET_EM_DEVICE_HCVR,							// ����CVI�ӿ�¼���
	NET_EM_DEVICE_NVR,							// ����¼���
	NET_EM_DEVICE_PCNVR,						// ������PC�ϵ��������¼���
	NET_EM_DEVICE_NVD,							// ���������
	NET_EM_DEVICE_SNVD,							// ������������
	NET_EM_DEVICE_UDS,							// ���ܽ�����
	NET_EM_DEVICE_SVR,							// �洢������ʽ¼���
	NET_EM_DEVICE_M,							// ��Ƶ�ۺ�ƽ̨
	NET_EM_DEVICE_IVS,							// IVS������
	NET_EM_DEVICE_VNCSERVER,					// �������������
	NET_EM_DEVICE_VNCCLIENT,					// ��������ͻ���
	NET_EM_DEVICE_DSCON,						// Ƕ��ʽ����������
	NET_EM_DEVICE_PC,							// ͨ�ø��˼����
	NET_EM_DEVICE_EVS,							// ������Ƶ�洢������
	NET_EM_DEVICE_VCS,							// ��Ƶת�������
	NET_EM_DEVICE_A,							// ��������, 15 ��ǰ
	NET_EM_DEVICE_ARC,							// ��������, 15���
	NET_EM_DEVICE_SDHNI,						// �������
	NET_EM_DEVICE_BSC,							// �Ž�����
	NET_EM_DEVICE_BSR,							// �Ž�������
	NET_EM_DEVICE_MGATEWAY,						// ý������(���밲׿�豸)
	NET_EM_DEVICE_VTO,							// �����ſڻ��豸
	NET_EM_DEVICE_VTH,							// �������ڻ��豸
	NET_EM_DEVICE_VTT,							// ���ӶԽ��ն�
	NET_EM_DEVICE_VTS,							// �����
	NET_EM_DEVICE_VTNC,							// ������
	NET_EM_DEVICE_CE,							// �����࿨Ƭ����Ʒ
	NET_EM_DEVICE_MPT,							// �ֳ��ն�
	NET_EM_DEVICE_ATM,							// ����ATM
	NET_EM_DEVICE_SHG,							// ������ͥ����
	NET_EM_DEVICE_ARM,							// �����տ�����
	NET_EM_DEVICE_ASM,							// �����м��豸
	NET_EM_DEVICE_VTA,							// ������
	NET_EM_DEVICE_UAVGA,						// ������̨
	NET_EM_DEVICE_UAVR,							// һ�廯ң����
	NET_EM_DEVICE_AIRCRAFT,						// ���˻�
	NET_EM_DEVICE_UAVS,							// ����վ
	NET_EM_DEVICE_TPCSD,						// �ȳ�����̨�����
	NET_EM_DEVICE_TPCBF,						// �ȳ���ǹ��
	NET_EM_DEVICE_RADAR,						// �״�
	NET_EM_DEVICE_RADAR_PTZ,					// �״�ϵͳ
	NET_EM_DEVICE_RADAR_CAM,					// �����״�
    NET_EM_DEVICE_KVM_IN,                       // KVM����ڵ�
    NET_EM_DEVICE_KVM_OUT                       // KVM����ڵ�
} NET_EM_DEVICE_TYPE;

// �豸������Ϣ
typedef struct tagCFG_DEVICE_CLASS_INFO
{
	DWORD				dwSize;
	NET_EM_DEVICE_TYPE	emDeviceType;			// �豸����
} CFG_DEVICE_CLASS_INFO;

// ĳ������֧�ֵ���������
typedef struct
{
	DWORD               dwRuleType;                                             // ��ǰ��������
	int					nSupportedObjectTypeNum;								// ��ǰ��������֧�ֵļ���������͸���
	char				szObjectTypeName[MAX_OBJECT_LIST_SIZE][MAX_NAME_LEN];	// ��ǰ��������֧�ֵļ�����������б�
}RULE_SUPPORTED_OBJECT_TYPE;

// �������
typedef struct tagCFG_SUPPORTED_COMP
{
	int 	nSupportedData;													// �����������֧�ֵĳ�������
	char	szSupportedData[MAX_SUPPORTED_COMP_DATA][CFG_COMMON_STRING_16];	// �����������֧�ֵĳ����б�
}CFG_SUPPORTED_COMP;

// ����ͷ��װ�Ƕ���ʾ��ʽ
typedef enum tagEM_DISPLAY_CAMERA_ANGLE_TYPE
{
	EM_DISPLAY_CAMERA_ANGLE_UNKNOWN,		// δ֪����ʾ��ʽ
	EM_DISPLAY_CAMERA_ANGLE_NUM,			// ���Ƕ�ֵ����(Ĭ��)
	EM_DISPLAY_CAMERA_ANGLE_MODE,			// ��ģʽ���ã�0~20չʾΪ��װ,21~90չʾΪбװ������ʱ��60�·�
} EM_DISPLAY_CAMERA_ANGLE_TYPE;

// ֧�ֵĹ���
typedef struct tagCFG_SUPPORTED_RULES_INFO
{
	CFG_NUMBERSTAT_RULE_INFO			stuNumberStat;		// ������ͳ�ƹ���
	CFG_QUEUEDETECTION_RULE_INFO		stuQueueDetection;	// �ŶӼ�����
	CFG_MANNUMDETECTION_RULE_INFO		stuManNumDetection;	// ����������ͳ�ƹ���
}CFG_SUPPORTED_RULES_INFO;

// ������ͳ��PD
typedef struct tagCFG_NUMBERSTART_MULT_INFO
{
	UINT							nMaxRules;			// ��󷵻ع������
	BOOL							bHiddenDetailEnable;// ���ز��ֲ�������ʹ�� true-����StereoNumber/NumberStat�㷨���࣬����StereoCalibrate�궨���ú͹����MaxHeight/MinHeight
	UINT							nCameraType;		// ������ͣ�0����ʾ�ϵ�Ŀ���� 1����ʾ��ͳһ��Ŀ���� 2����ʾ��ͳһ˫Ŀ����
	int								nMaxSupportedRulesNum;	// ֧�ֵ��¼����͹������
	CFG_SUPPORTED_RULES_INFO		stuSupportedRules[MAX_RULE_LIST_SIZE]; // ֧�ֵ��¼����͹����б��¼����ͣ����dhnetsdk.h��"���ܷ����¼�����"
}CFG_NUMBERSTAT_MULT_INFO;

// ��Ƶ����������
typedef struct _tagVA_CAPS_INFO
{
	char				szSceneName[MAX_SCENE_LIST_SIZE][MAX_NAME_LEN];			// ֧�ֵĳ����б�
	int					nSupportedSceneNum;										// ֧�ֵĳ�������
	BYTE				byReserved[4];											// Ԥ���ֶ�
}VA_CAPS_INFO;


// ��Ƶ����������
typedef struct tagCFG_CAP_ANALYSE_INFO
{	
    int                 nSupportedSceneNum;                                     // ֧�ֵĳ�������
    char                szSceneName[MAX_SCENE_LIST_SIZE][MAX_NAME_LEN];         // ֧�ֵĳ����б�
    int                 nMaxMoudles;                                            // ÿͨ��֧��������ģ����
    int                 nSupportedObjectTypeNum;                                // ֧�ֵļ���������͸���
    char                szObjectTypeName[MAX_OBJECT_LIST_SIZE][MAX_NAME_LEN];   // ֧�ֵļ�����������б�
    int                 nMaxRules;                                              // ÿͨ��֧������������
    int                 nSupportedRulesNum;                                     // ֧�ֵ��¼����͹������
    DWORD               dwRulesType[MAX_RULE_LIST_SIZE];                        // ֧�ֵ��¼����͹����б��¼����ͣ����dhnetsdk.h��"���ܷ����¼�����"
    int                 nMaxStaffs;                                             // ֧�ֵ�����߸���
    int                 nMaxPointOfLine;                                        // ������󶥵���
    int                 nMaxPointOfRegion;                                      // ������󶥵���
    int                 nMaxInternalOptions;                                    // ����ڲ�ѡ�����
    bool                bComplexSizeFilter;                                     // �Ƿ�֧�ָ��ӳߴ������	���ӳߴ������ʹ�ö�����������˺Ϳ�߱ȹ��˲�����      
    bool                bSpecifiedObjectFilter;                                 // �Ƿ�֧���ض������������
    int                 nMaxExcludeRegionNum;                                   // ֧��ģ���е�����ų��������
    int                 nMaxCalibrateBoxNum;                                    // ֧�ֵ�ģ���е����У׼�����
    int                 nMinCalibrateBoxNum;                                    // ģ����������Ҫ���õ�У׼�����
    int                 nMaxSepcialDetectRegions;                               // ģ����֧�ֵ�����������������
    int                 nSupportedTimeSectionNum;                               // ֧�ֵ�ÿ���ʱ��θ��� 
    int                 nSpecialDetectPropertyNum;                              // ֧�ֵ������������Ը���
    int                 nSupportedSpecialDetectPropertys[EM_SEPCIALREGION_PROPERTY_TYPE_NUM]; // ֧�ֵ�������������
    DWORD               nSupportedVideoChannelType;                             // ֧�ֵ���Ƶͨ������ѡ��   ��0λΪ1��ʾ֧����������,��1λΪ1��ʾ֧��ģ������,��2Ϊ1��ʾ֧�ֻ��(���ֺ�ģ��)				
    bool                bSupportMultiSceneSwitch;                               // �Ƿ�֧�ֶೡ���л�
    CFG_VACAP_SUPPORTEDSCENES  stSupportScenes;                                 // ֧�ֵĳ���������
    int                 nSupportedComp;                                         // ֧�ֵĳ�����������
    CFG_SUPPORTED_COMP  stuSupportedComp[MAX_SUPPORTED_COMP_SIZE];              // ֧�ֵĳ���������б�
    BYTE                byFilterMask;                                           // 0 Module/Rule���þ�Ҫ����(Ĭ�ϣ����ܷ�����), 1 ��Rule����Ҫ����(IPC/xVR)
    EM_DISPLAY_CAMERA_ANGLE_TYPE emDisCameraAngleType;                          // ����ͷ��װ�Ƕ���ʾ��ʽ
    int                          nMaxNumberStatMultNum;                         // ʵ�ʷ��ؿ�����ͳ�Ƴ���PD����
    CFG_NUMBERSTAT_MULT_INFO     stuNumberStatMult[MAX_NUMBER_STAT_MAULT_NUM];  // ������ͳ�Ƴ���PD

    VA_CAPS_INFO        stuFullCaps;                                            // ȫ����������ʾ��Ƶͨ������ full������Ϊ�ǲ����

} CFG_CAP_ANALYSE_INFO;

// ��Ƶ����֧�ֵĶ�������
typedef enum tagEM_VIDEO_ANALYSE_OBJECT_TYPE
{
    EM_VIDEO_ANALYSE_OBJECT_TYPE_UNKNOWN,           // δ֪��
    EM_VIDEO_ANALYSE_OBJECT_TYPE_ALL_OBJECT,        // ��������
    EM_VIDEO_ANALYSE_OBJECT_TYPE_HUMAN,             // ��       
    EM_VIDEO_ANALYSE_OBJECT_TYPE_VEHICLE,           // ����
    EM_VIDEO_ANALYSE_OBJECT_TYPE_FIRE,              // ��
    EM_VIDEO_ANALYSE_OBJECT_TYPE_SMOKE,             // ����
    EM_VIDEO_ANALYSE_OBJECT_TYPE_PLATE,             // Ƭ״����             
    EM_VIDEO_ANALYSE_OBJECT_TYPE_HUMANFACE,         // ����
    EM_VIDEO_ANALYSE_OBJECT_TYPE_CONTAINER,         // ����
    EM_VIDEO_ANALYSE_OBJECT_TYPE_ANIMAL,            // ����
    EM_VIDEO_ANALYSE_OBJECT_TYPE_TRAFFICLIGHT,      // ���̵�
    EM_VIDEO_ANALYSE_OBJECT_TYPE_PASTEPAPER,        // ��ֽ ��Ƭ
    EM_VIDEO_ANALYSE_OBJECT_TYPE_HUMANHEAD,         // �˵�ͷ��
    EM_VIDEO_ANALYSE_OBJECT_TYPE_ENTITY,            // ��ͨ����
}EM_VIDEO_ANALYSE_OBJECT_TYPE;

// ��Ƶ����֧�ֵĶ�������
typedef enum tagEM_VIDEO_ANALYSE_ACTION_TYPE
{                                                   
    EM_VIDEO_ANALYSE_ACTION_TYPE_UNKNOWN,           // δ֪��Ϊ        
    EM_VIDEO_ANALYSE_ACTION_TYPE_APPEAR,            // ���֣���Ե���Ŀ�꣬���־ͱ���
    EM_VIDEO_ANALYSE_ACTION_TYPE_DISAPPEAR,         // ��ʧ
    EM_VIDEO_ANALYSE_ACTION_TYPE_INSIDE,            // �������ڣ���Ҫ������Ŀ������ﵽָ��������ָ��ʱ��ű���
    EM_VIDEO_ANALYSE_ACTION_TYPE_CROSS,             // ��Խ����
}EM_VIDEO_ANALYSE_ACTION_TYPE;

// ��Ƶ����֧�ֵļ������
typedef enum tagEM_VIDEO_ANALYSE_DETECT_TYPE
{
    EM_VIDEO_ANALYSE_DETECT_TYPE_UNKNOWN,           // δ֪����
    EM_VIDEO_ANALYSE_DETECT_TYPE_LOSS,              // ��Ƶ��ʧ
    EM_VIDEO_ANALYSE_DETECT_TYPE_COVER,             // ��Ƶ�ڵ�
    EM_VIDEO_ANALYSE_DETECT_TYPE_FROZEN,            // ���涳��
    EM_VIDEO_ANALYSE_DETECT_TYPE_LIGHT,             // ����
    EM_VIDEO_ANALYSE_DETECT_TYPE_DARK,              // ����
    EM_VIDEO_ANALYSE_DETECT_TYPE_SCENECHANGE,       // �����仯
    EM_VIDEO_ANALYSE_DETECT_TYPE_STRIATION,         // ���Ƽ��
    EM_VIDEO_ANALYSE_DETECT_TYPE_NOISE,             // �������
    EM_VIDEO_ANALYSE_DETECT_TYPE_UNBALANCE,         // ƫɫ���
    EM_VIDEO_ANALYSE_DETECT_TYPE_BLUR,              // ��Ƶģ�����
    EM_VIDEO_ANALYSE_DETECT_TYPE_CONTRAST,          // �Աȶ��쳣���
    EM_VIDEO_ANALYSE_DETECT_TYPE_MOTION,            // ��Ƶ�˶�
    EM_VIDEO_ANALYSE_DETECT_TYPE_FILCKERING,        // ��Ƶ��˸
    EM_VIDEO_ANALYSE_DETECT_TYPE_COLOR,             // ��Ƶ��ɫ�仯
    EM_VIDEO_ANALYSE_DETECT_TYPE_UNFOCUS,           // �齹���
    EM_VIDEO_ANALYSE_DETECT_TYPE_OVEREXPOSURE,      // ���ؼ�� 
}EM_VIDEO_ANALYSE_DETECT_TYPE;

// ��Ƶ֧�ֵ���������
typedef enum tagEM_VIDEO_ANALYSE_HUMANFACE_TYPE
{
    EM_VIDEO_ANALYSE_HUMANFACE_TYPE_UNKNOWN,        // δ֪����
    EM_VIDEO_ANALYSE_HUMANFACE_TYPE_NORMAL,         // ��ͨ����
    EM_VIDEO_ANALYSE_HUMANFACE_TYPE_HIDEEYE,        // �۲��ڵ�
    EM_VIDEO_ANALYSE_HUMANFACE_TYPE_HIDENOSE,       // �����ڵ�
    EM_VIDEO_ANALYSE_HUMANFACE_TYPE_HIDEMOUTH,      // �첿�ڵ�
    EM_VIDEO_ANALYSE_HUMANFACE_TYPE_ADJACENTFACE,   // �������� (ע��ATM��β��)
    EM_VIDEO_ANALYSE_HUMANFACE_TYPE_ABNORMALFACE,   // �쳣����
    EM_VIDEO_ANALYSE_HUMANFACE_TYPE_HELMETFACE,     // ͷ������(��Ħ�г���)
}EM_VIDEO_ANALYSE_HUMANFACE_TYPE;

// ����֧�ֵĹ���
typedef struct
{
    DWORD                                       dwSupportedRule;                                         // ��������
    int                                         nSupportedObjectTypeNum;                                 // ֧�ֵļ���������͸���
    EM_VIDEO_ANALYSE_OBJECT_TYPE                emObjectType[MAX_OBJECT_LIST_SIZE];	                     // ֧�ֵļ�����������б�
    int                                         nSupportedActionsNum;                                    // ֧�ֵļ�����嶯������
    EM_VIDEO_ANALYSE_ACTION_TYPE                emSupportedAction[MAX_ACTION_LIST_SIZE];                 // ֧�ֵļ�����嶯���б�
    int                                         nSupportedDetectTypeNum;                                 // ֧�ֵļ�����͸���
    EM_VIDEO_ANALYSE_DETECT_TYPE                emSupportedDetectType[MAX_ACTION_LIST_SIZE];             // ֧�ֵļ�������б�
    int                                         nSupportFaceTypeNum;                                     // ֧�ֵ�����������͸���
    EM_VIDEO_ANALYSE_HUMANFACE_TYPE             emSupportFaceType[MAX_HUMANFACE_LIST_SIZE];              // ֧�ֵ�������������б�
    BOOL                                        bTriggerPosition;                                        // �Ƿ�֧�ִ�������λ��
    BOOL			                            bTriggerTrack;											 // �Ƿ�֧�ִ�������
	FACEBODY_DETECT_CAPS						stuFaceBodyDetectCaps;									 // ���������������ƣ�
	FACEBODY_ANALYSE_CAPS						stuFaceBodyAnalyseCaps;							         // ����ʶ�����������ƣ�
	EM_FEATURE_LIST_TYPE						emFeature[32];											 // ���װ�������б�
	int											nDetectFeatureCount;									 // ��⵽����������
	EM_SUPPORTED_COLOR_LIST_TYPE				emSupportedColorList[8];								 // ��⵽�ĳ�ʦ��װ��ɫ�б�
	int											nDetectSupportedColorCount;								 // ��⵽�ĳ�ʦ��װ��ɫ����
}SCENE_SUPPORT_RULE_EX;

// ��������
typedef struct tagCFG_CAP_SCENE_EX
{
    char               szSceneName[MAX_PATH];                                   // ��������
    int                nSupportedObjectTypeNum;                                 // ��ǰ��������֧�ֵļ���������͸���
    char               szObjectTypeName[MAX_OBJECT_LIST_SIZE][MAX_NAME_LEN];	// ��ǰ��������֧�ֵļ�����������б�
    int                nSupportRules;                                           // ֧�ֵĹ������
    SCENE_SUPPORT_RULE_EX stSpportRules[MAX_RULE_LIST_SIZE];                    // ֧�ֵĹ����б�

    //֧�ֵ�ģ�����
    bool               bDisturbance;                                            // �Ƿ�֧���Ŷ�ǿ������
    bool               bAntiDisturbance;                                        // �Ƿ�֧��ȥ�Ŷ�����
    bool               bBacklight;                                              // �Ƿ�֧����⴦��
    bool               bShadow;                                                 // �Ƿ�֧����Ӱ����
    bool               bAccuracy;                                               // �Ƿ�֧�ּ�⾫��
    bool               bMovingStep;												// �Ƿ�֧�ּ�ⲽ��
    bool               bScalingFactor;                                          // �Ƿ�֧�ּ������
    bool               bThresholdY;                                             // �Ƿ�֧��Y�����ж���ֵ
    bool               bThresholdUV;                                            // �Ƿ�֧��UV�����ж���ֵ
    bool               bThresholdEdge;                                          // �Ƿ�֧�ֱ�Ե����ж���ֵ
    bool               bDetectBalance;                                          // �Ƿ�֧�ּ��ƽ��
    bool               bAlgorithmIndex;                                         // �Ƿ�֧���㷨���
    bool               bHighlight;                                              // �Ƿ�֧�ָ߹⴦����Backgroud���� 
    bool               bSnapshot;                                               // �Ƿ�֧������ץͼ
    bool               bDetectRegion;                                           // �Ƿ�֧�ּ����������
    bool               bExcludeRegion;                                          // �Ƿ�֧���ų���������
    bool               bContourAssistantTrack;                                  // �Ƿ�֧��������������

    //֧�ֵĳ�������
    bool               bCameraAspect;                                            // �Ƿ�����ͷλ�ò���
    bool               bJitter;                                                  // �Ƿ�֧�ֶ�������
    bool               bDejitter;                                                // �Ƿ�֧��ȥ�����������

    // ֧�ֵı궨������
    int               nMaxCalibrateAreaNum;                                      // ���궨�������
    int               nCalibrateAreaNum;                                         // �궨����������Ϣ����
    CFG_CAP_CELIBRATE_AREA stCalibrateAreaCap[MAX_CALIBRATEAREA_TYPE_NUM];       // �궨����������Ϣ

    // �ߴ������������ʽ
    bool              bPixel;                                                    // �Ƿ�֧�����ؼ�����ʽ
    bool              bMetric;                                                   // �Ƿ�֧��ʵ�ʳ��ȼ�����ʽ
    bool              bCalibratedPixel;                                          // �Ƿ�֧��Զ�˽��˱궨������ؼ�����ʽ

    char              szSubType[MAX_SCENE_SUBTYPE_NUM][MAX_SCENE_SUBTYPE_LEN];  // ��ǰ������֧�ֵ��ӳ�������
    int               nSubTypeNum;                                              // �ӳ������͸���
    int				  nMaxRules;												// ÿ������֧�ֵ�����������
	BOOL			  bSupportedSetModule;										// �Ƿ�֧���ؽ�������Ĭ�ϲ�֧��(false)	
    BOOL              bSupportFightCalibrate;                                   // �Ƿ�֧�ִ�ܱ궨����,Ĭ��FALSE
	char			  szClassAlias[16];											// ����ҵ�񷽰�����  
	BYTE              byReserved[1008];                                         // �����ֽ�
}CFG_CAP_SCENE_EX;

// �����б�
typedef struct tagCFG_VACAP_SUPPORTEDSCENES_EX
{
    int                nScenes;													   // ֧�ֵĳ�������
    CFG_CAP_SCENE_EX   stScenes[MAX_SCENE_LIST_SIZE];                              // ֧�ֵĳ����б�
}CFG_VACAP_SUPPORTEDSCENES_EX;

// ��Ƶ�����������������ֶ�stSupportScenes�⣬�����ֶκ�CFG_CAP_ANALYSE_INFOһ����
typedef struct tagCFG_CAP_ANALYSE_INFO_EX
{
    int					nSupportedSceneNum;										// ֧�ֵĳ�������
    char				szSceneName[MAX_SCENE_LIST_SIZE][MAX_NAME_LEN];			// ֧�ֵĳ����б�
    int					nMaxMoudles;											// ÿͨ��֧��������ģ����
    int					nSupportedObjectTypeNum;								// ֧�ֵļ���������͸���
    char				szObjectTypeName[MAX_OBJECT_LIST_SIZE][MAX_NAME_LEN];	// ֧�ֵļ�����������б�
    int					nMaxRules;												// ÿͨ��֧������������
    int					nSupportedRulesNum;										// ֧�ֵ��¼����͹������
    DWORD				dwRulesType[MAX_RULE_LIST_SIZE];	             		// ֧�ֵ��¼����͹����б��¼����ͣ����dhnetsdk.h��"���ܷ����¼�����"
    int					nMaxStaffs;												// ֧�ֵ�����߸���
    int					nMaxPointOfLine;										// ������󶥵���
    int					nMaxPointOfRegion;										// ������󶥵���
    int                 nMaxInternalOptions;                                    // ����ڲ�ѡ�����
    bool                bComplexSizeFilter;                                     // �Ƿ�֧�ָ��ӳߴ������	���ӳߴ������ʹ�ö�����������˺Ϳ�߱ȹ��˲�����      
    bool                bSpecifiedObjectFilter;                                 // �Ƿ�֧���ض������������
    int                 nMaxExcludeRegionNum;                                   // ֧��ģ���е�����ų��������
    int                 nMaxCalibrateBoxNum;                                    // ֧�ֵ�ģ���е����У׼�����
    int                 nMinCalibrateBoxNum;                                    // ģ����������Ҫ���õ�У׼�����
    int                 nMaxSepcialDetectRegions;                               // ģ����֧�ֵ�����������������
    int                 nSupportedTimeSectionNum;                               // ֧�ֵ�ÿ���ʱ��θ��� 
    int                 nSpecialDetectPropertyNum;                              // ֧�ֵ������������Ը���
    int			        nSupportedSpecialDetectPropertys[EM_SEPCIALREGION_PROPERTY_TYPE_NUM]; // ֧�ֵ�������������
    DWORD				nSupportedVideoChannelType;								// ֧�ֵ���Ƶͨ������ѡ��   ��0λΪ1��ʾ֧����������,��1λΪ1��ʾ֧��ģ������,��2Ϊ1��ʾ֧�ֻ��(���ֺ�ģ��)				
    bool				bSupportMultiSceneSwitch;								// �Ƿ�֧�ֶೡ���л�
    CFG_VACAP_SUPPORTEDSCENES_EX  stSupportScenes;                                 // ֧�ֵĳ���������
    int 				nSupportedComp; 										// ֧�ֵĳ�����������
    CFG_SUPPORTED_COMP	stuSupportedComp[MAX_SUPPORTED_COMP_SIZE];				// ֧�ֵĳ���������б�
    BYTE				byFilterMask;											// 0 Module/Rule���þ�Ҫ����(Ĭ�ϣ����ܷ�����), 1 ��Rule����Ҫ����(IPC/xVR)
    EM_DISPLAY_CAMERA_ANGLE_TYPE emDisCameraAngleType;							// ����ͷ��װ�Ƕ���ʾ��ʽ

}CFG_CAP_ANALYSE_INFO_EX;

// У׼����Ϣ
typedef struct tagCFG_CALIBRATEBOX_INFO
{
	CFG_POLYGON         stuCenterPoint;                       // У׼�����ĵ�����(��������һ����[0,8191]����)
	float               fRatio;                               // ��Ի�׼У׼��ı���(����1��ʾ��׼���С��0.5��ʾ��׼���С��һ��)
}CFG_CALIBRATEBOX_INFO;

// �궨��ʽ
typedef enum tagEM_METHOD_TYPE
{
	EM_METHOD_UNKNOWN,		// δ֪��ʽ
	EM_METHOD_SCENE,		// ����궨
	EM_METHOD_LANE,			// �����궨
	EM_METHOD_PLATE,		// ���Ʊ궨
	EM_METHOD_STEREO,		// ˫Ŀ�궨
} EM_METHOD_TYPE;

// �궨����,��ͨ����ʹ��
typedef struct tagCFG_CALIBRATEAREA_INFO
{
	int						nLinePoint;						// ˮƽ�������߶�����
	CFG_POLYGON				stuLine[MAX_POLYLINE_NUM];		// ˮƽ��������
	float					fLenth;                 		// ʵ�ʳ���
	CFG_REGION				stuArea;                  		// ����
	int                 	nStaffNum;                 		// ��ֱ�����
	CFG_STAFF           	stuStaffs[MAX_STAFF_NUM];		// ��ֱ���         
	EM_CALIBRATEAREA_TYPE 	emType;							// ��������
	EM_METHOD_TYPE			emMethodType;					// �궨��ʽ
}CFG_CALIBRATEAREA_INFO;

// ����ʶ�𳡾�
typedef struct tagCFG_FACERECOGNITION_SCENCE_INFO
{
	double				dbCameraHeight;							// ����ͷ��ظ߶� ��λ����
	double				dbCameraDistance;						// ����ͷ��������������ĵ�ˮƽ���� ��λ����
	int                 nMainDirection;                         // ������Ҫ���򶥵���
	CFG_POLYGON         stuMainDirection[MAX_POLYLINE_NUM];     // ������Ҫ���򣬵�һ��������ʼ�㣬�ڶ���������ֹ��
	char                byFaceAngleDown;                        // ��Ҫ���������������ƫ��, ��λ�ȣ�-45~45��������ʾ���������ϱߣ�������ʾ���������±ߣ�0��ʾ������ֱ����������������ͷ��
	char                byFaceAngleUp;                          // ��Ҫ���������������ƫ��,��λ�ȣ�-45~45��������ʾ���������ϱߣ�������ʾ���������±ߣ�0��ʾ������ֱ����������������ͷ��
 	char                byFaceAngleLeft;                        // ��Ҫ���������������ƫ��,��λ�ȣ�-45~45��������ʾ����������ߣ�������ʾ���������ұߣ�0��ʾ����ˮƽ����������������ͷ
	char                byFaceAngleRight;                       // ��Ҫ���������������ƫ��,��λ�ȣ�-45~45��������ʾ����������ߣ�������ʾ���������ұߣ�0��ʾ����ˮƽ����������������ͷ
	EM_FACEDETECTION_TYPE	emDetectType;						// �����������
}CFG_FACERECOGNITION_SCENCE_INFO;

// ������ⳡ��
typedef struct tagCFG_FACEDETECTION_SCENCE_INFO
{
	double				dbCameraHeight;							// ����ͷ��ظ߶� ��λ����
	double				dbCameraDistance;						// ����ͷ��������������ĵ�ˮƽ���� ��λ����
	int                 nMainDirection;                         // ������Ҫ���򶥵���
	CFG_POLYGON         stuMainDirection[MAX_POLYLINE_NUM];     // ������Ҫ���򣬵�һ��������ʼ�㣬�ڶ���������ֹ��
	char                byFaceAngleDown;                        // ��Ҫ���������������ƫ��, ��λ�ȣ�-45~45��������ʾ���������ϱߣ�������ʾ���������±ߣ�0��ʾ������ֱ����������������ͷ��
	char                byFaceAngleUp;                          // ��Ҫ���������������ƫ��,��λ�ȣ�-45~45��������ʾ���������ϱߣ�������ʾ���������±ߣ�0��ʾ������ֱ����������������ͷ��
	char                byFaceAngleLeft;                        // ��Ҫ���������������ƫ��,��λ�ȣ�-45~45��������ʾ����������ߣ�������ʾ���������ұߣ�0��ʾ����ˮƽ����������������ͷ
	char                byFaceAngleRight;                       // ��Ҫ���������������ƫ��,��λ�ȣ�-45~45��������ʾ����������ߣ�������ʾ���������ұߣ�0��ʾ����ˮƽ����������������ͷ
	EM_FACEDETECTION_TYPE	emDetectType;						// �����������
}CFG_FACEDETECTION_SCENCE_INFO;


// ��ͨ����ɫУ�����ã�������
typedef struct tagADJUST_LEVEL_SEP
{
    int                 nType;                                  // 0��δ���壬1����Ƶ��2��ͼƬ
    int                 nTime;                                  // 0��δ���壬1�����죬2��ҹ��
    int                 nLevel;                                 // ��Χ0~100����ֵԽ�����Խ����
} ADJUST_LEVEL_SEP;

// ��ͨ����ɫУ������
typedef struct tagCFG_ADJUST_LIGHT_COLOR
{
    int                 nMode;                                  // �����ɫУ��ģʽ 0:δ���� 1:���̵Ʋ�У�� 2:һֱУ��
    BOOL                bEnable;                                // �Ƿ�����ͼƬ���̵���ɫУ��
    int                 nLevel;                                 // У���ȼ� ��Χ0~100����ֵԽ�����Խ����
    BOOL                bVideoEnable;                           // �Ƿ�������ƵͿ�칦�ܣ����ڴ���ʱ��Levelֵʹ��LevelSeparate�µ�Levelֵ                                
    ADJUST_LEVEL_SEP    stLevelSep[4];                          // �����ȼ���ĿǰΪ4��
} CFG_ADJUST_LIGHT_COLOR;

#define  MAX_PARKING_SPACE_NUM          6       //�������6����λ��Ϣ
#define  MAX_SHIELD_AREA_NUM            16      //1����λ����Ӧ16����������
typedef struct tagCFG_PARKING_SPACE
{
    int             nNumber;                // ��λ���
    CFG_REGION      stArea;                 // �������
    int             nShieldAreaNum;                              // ��Ч����������
    CFG_REGION      stShieldArea[MAX_SHIELD_AREA_NUM];           // ��������
    char            szCustomParkNo[CFG_COMMON_STRING_32];        // �Զ��峵λ����
}CFG_PARKING_SPACE;

// ��ͨ����
typedef struct tagCFG_TRAFFIC_SCENE_INFO 
{
	BOOL                abCompatibleMode;	
	int                 nCompatibleMode;                        // 0:"OldTrafficRule" : ��ͨ�Ϲ������ģʽ;1:"NewTrafficRule" :  ��ͨ�¹������ģʽ;-1:�ַ�������
	float				fCameraHeight;							// ����ͷ��ظ߶�	��λ����
	float				fCameraDistance;						// ����ͷ��������������ĵ�ˮƽ����	��λ����
	char                szSubType[MAX_NAME_LEN];                // ��ͨ������������,"Gate" ��������,"Junction" ·������ 
																// "Tunnel"������� , "ParkingSpace"��λ�������
																// "Bridge"��������
																// "Freeway"���ٹ�·����                 
	CFG_POLYGON         stuNearDetectPoint;                     // ��������
	CFG_POLYGON         stuFarDectectPoint;                     // Զ������
	int                 nNearDistance;                          // NearDetectPoint,ת����ʵ�ʳ�����ʱ,������ͷ��ֱ�ߵ�ˮƽ����
	int                 nFarDistance;                           // FarDectectPoint,ת����ʵ�ʳ�����ʱ,������ͷ��ֱ�ߵ�ˮƽ����
	int                 nPlateHintNum;                          // �����ַ���ʾ����
	char                szPlateHints[MAX_PLATEHINT_NUM][MAX_NAME_LEN]; // �����ַ���ʾ���飬������ͼƬ�����ϲ��ʶ��ȷ��ʱ�����ݴ������е��ַ�����ƥ�䣬�����±�ԽС��ƥ�����ȼ�Խ��
	int                 nLaneNum;                               // ������
	CFG_LANE            stuLanes[MAX_LANE_NUM];                 // ������Ϣ
    int                 nLightGroupNum;                         // ������
	CFG_LIGHTGROUPS     stLightGroups[MAX_LIGHTGROUP_NUM];      // ����������Ϣ
    BOOL                bHangingWordPlate;                      // �Ƿ�ʶ����ֳ���
    BOOL                bNonStdPolicePlate;                     // �Ƿ�ʶ��Ǳ�׼����
    BOOL                bYellowPlateLetter;                     // �Ƿ�ʶ�������ĸ
    int                 nReportMode;                            // �ϱ�ģʽ��0��δ���壬1���ϱ�����Υ�³�����2���ϱ�����ʶ��ɹ���Υ�³���
    int                 nPlateMatch;                            // ����ƥ���ʣ�0~100�����ʶ����ʹ��,��ʾ�ظ�ʶ���ƺ�ǰһ��ʶ���Ƶ�һ����Ҫ��
    int                 nJudgment;                              // Υ���ж����ݣ�0��δ���壬1����������2������ʻ����3������������ʻ����
    int                 nLeftDivisionPtCount;                   // ��ת��ֽ��ߵ���
    CFG_POLYLINE        stLeftDivisionLine[MAX_POLYLINE_NUM];   // ��ת��ֽ���
    int                 nRightDivisionPtCount;                  // ��ת��ֽ��ߵ���
    CFG_POLYLINE        stRightDivisionLine[MAX_POLYLINE_NUM];  // ��ת��ֽ���
    CFG_ADJUST_LIGHT_COLOR  stAdjustLightColor;                 // ��ͨ����ɫУ������
    int                  nParkingSpaceNum;                      // ��λ��                             
    CFG_PARKING_SPACE    stParkingSpaces[MAX_PARKING_SPACE_NUM];// ��λ������Ϣ,ÿ��Ԫ�ش���һ����λ

} CFG_TRAFFIC_SCENE_INFO;

// �ձ鳡��
typedef struct tagCFG_NORMAL_SCENE_INFO
{
	float				fCameraHeight;							// ����ͷ��ظ߶�	��λ����
	float				fCameraAngle;							// ����ͷ�봹����ļн�	��λ�ȣ�0~90��
	float				fCameraDistance;						// ����ͷ��������������ĵ�ˮƽ����	��λ����
	CFG_POLYGON			stuLandLineStart;						// ��ƽ���߶���ʼ��(������������һ����[0,8192)���䡣)
	CFG_POLYGON			stuLandLineEnd;							// ��ƽ���߶���ֹ��(������������һ����[0,8192)���䡣)
} CFG_NORMAL_SCENE_INFO;

// ��ͨѲ�ӳ���
typedef struct tagCFG_TRAFFIC_TOUR_SCENE_INFO 
{
	int                 nPlateHintNum;                          // �����ַ���ʾ����
	char                szPlateHints[MAX_PLATEHINT_NUM][MAX_NAME_LEN]; // �����ַ���ʾ���飬������ͼƬ�����ϲ��ʶ��ȷ��ʱ�����ݴ������е��ַ�����ƥ�䣬�����±�ԽС��ƥ�����ȼ�Խ��
} CFG_TRAFFIC_TOUR_SCENE_INFO;

// ��Ⱥ̬�ƺ���Ⱥ�ܶȳ���
typedef struct tagCFG_CROWD_SCENE_INFO 
{
	float				fCameraHeight;							// ����ͷ��ظ߶�	��λ����
} CFG_CROWD_SCENE_INFO;

// ���澰������
typedef enum tagEM_DEPTH_TYPE
{
	EM_DEPTH_UNKNOWN,		//unknown
	EM_DEPTH_NORMAL,		// "Normal" δ֪
	EM_DEPTH_FAR,			// "Far" Զ��
	EM_DEPTH_MIDDLE,		// "Middle" �о�
	EM_DEPTH_NEAR			// "Near" ����
}EM_DEPTH_TYPE;


// ͳһ��������,TypeList����ʱ���ô˳���
typedef struct tagCFG_INTELLI_UNIFORM_SCENE{
	char                szSubType[MAX_NAME_LEN];                // ��ͨ������������,"Gate" ��������,"Junction" ·������ 
																// "Tunnel"������� , "ParkingSpace"��λ�������
																// "Bridge"��������
																// "Freeway"���ٹ�·����                 
	int                 nPlateHintNum;                          // �����ַ���ʾ����
	char                szPlateHints[MAX_PLATEHINT_NUM][MAX_NAME_LEN]; // �����ַ���ʾ���飬������ͼƬ�����ϲ��ʶ��ȷ��ʱ�����ݴ������е��ַ�����ƥ�䣬�����±�ԽС��ƥ�����ȼ�Խ��
	int                 nLaneNum;                               // ������
	CFG_LANE            stuLanes[MAX_LANE_NUM];                 // ������Ϣ
}CFG_INTELLI_UNIFORM_SCENE;


// ��Ƶ����ȫ�����ó���
typedef struct tagCFG_ANALYSEGLOBAL_SCENE
{
	char				szSceneType[MAX_NAME_LEN];				// Ӧ�ó���,���"֧�ֵĳ����б�"

	// ����Ϊ����������Ϣ, ����szSceneType�����ĸ�������Ч
	union
	{
		CFG_FACEDETECTION_SCENCE_INFO	stuFaceDetectionScene;	// ������ⳡ��/����ʶ����
		CFG_TRAFFIC_SCENE_INFO			stuTrafficScene;		// ��ͨ����
		CFG_NORMAL_SCENE_INFO			stuNormalScene;			// ��ͨ����/Զ������/�о�����/��������/���ڳ���/����ͳ�Ƴ���
		CFG_TRAFFIC_TOUR_SCENE_INFO		stuTrafficTourScene;	// ��ͨѲ�ӳ���
		CFG_CROWD_SCENE_INFO			stuCrowdScene;			// ��Ⱥ̬�ƺ���Ⱥ�ܶȳ���
	};

	EM_DEPTH_TYPE		emDepthType;							// ���澰��
	int					nPtzPresetId;							// ��̨Ԥ�õ��ţ�0~255
	// �������ж������ҵ������
	int					nSceneListCount;						// ʵ�ʳ�������
	char				szSceneTypeList[MAX_SCENE_TYPE_LIST_SIZE][CFG_COMMON_STRING_16];// �����б�ͬһ��Ƶͨ�������ö������ʱ����ʾ��2��֮��ķ���
	// �������ҵ��ʱ��Ч
	CFG_INTELLI_UNIFORM_SCENE 	stuUniformScene; 				// ͳһ��������
} CFG_ANALYSEGLOBAL_SCENE;

typedef struct tagCFG_TIME
{
	DWORD				dwHour;					// ʱ
	DWORD				dwMinute;				// ��
	DWORD				dwSecond;				// ��
}CFG_TIME;

typedef struct tagCFG_TIME_PERIOD
{
	CFG_TIME	stuStartTime;				
	CFG_TIME	stuEndTime;			
}CFG_TIME_PERIOD;

// �ೡ���궨�������û�����Ԫ
typedef struct tagCFG_CALIBRATEAREA_SCENE_UNIT
{
	unsigned int           nCalibrateAreaNum;                                // �˳����ڱ궨������
	CFG_CALIBRATEAREA_INFO stuCalibrateArea[MAX_CALIBRATEBOX_NUM];           // �궨����(�����ֶβ����ڣ�������������Ϊ�궨����)
}
CFG_CALIBRATEAREA_SCENE_UNIT;

// �ೡ���궨��������
typedef struct tagCFG_CALIBRATEAREA_SCENE_INFO
{
	DWORD                        dwMaxSceneCalibrateAreaNum;                 // �ೡ���궨����������(��Ҫ����˴�С�ڴ�)
	DWORD                        dwRetSceneCalibrateAreaNum;                 // ʵ�ʰ����ೡ���궨�������
	CFG_CALIBRATEAREA_SCENE_UNIT *pstuCalibrateArea;                         // �ೡ���궨����Ԫ
}
CFG_CALIBRATEAREA_SCENE_INFO;

// ��̨�������굥Ԫ
typedef struct tagCFG_PTZ_SPACE_UNIT
{
	int                    nPositionX;           //��̨ˮƽ�˶�λ�ã���Ч��Χ��[0,3600]
	int                    nPositionY;           //��̨��ֱ�˶�λ�ã���Ч��Χ��[-1800,1800]
	int                    nZoom;                //��̨��Ȧ�䶯λ�ã���Ч��Χ��[0,128]
}CFG_PTZ_SPACE_UNIT;

// �궨��������
typedef struct tagCFG_CALIBRATEAREA_UNIT
{
	int                          nSceneID;                                 // �������
	int                          nPtzPresetID;                             // Ԥ�õ���
	BOOL                         abCalibrateArea;                          // �궨�����Ƿ����
	CFG_CALIBRATEAREA_SCENE_UNIT stuCalibrateArea;                         // �궨����
	CFG_PTZ_SPACE_UNIT           stuPtzPosition;                           // �궨����̨λ��
}
CFG_VIDEO_ANALYSE_CALIBRATEAREA_UNIT;

// �궨��������
typedef struct tagCFG_VIDEO_ANALYSE_CALIBRATEAREA
{

	DWORD                                dwMaxSceneCalibrateNum;           // �ೡ���궨����������(��Ҫ����˴�С�ڴ�)
	DWORD                                dwRetSceneCalibrateNum;           // ʵ�ʰ����ೡ���궨�������
	CFG_VIDEO_ANALYSE_CALIBRATEAREA_UNIT *pstuCalibrateAreaUnit;           // �궨����
}
CFG_VIDEO_ANALYSE_CALIBRATEAREA;

// ��ҹ�㷨�л�ģʽ
typedef enum tagCFG_TIMEPERIOD_SWITCH_MODE
{
	CFG_TIMEPERIOD_SWITCH_MODE_UNKNOWN,                                     // δ֪
	CFG_TIMEPERIOD_SWITCH_MODE_BYCOLOR,                                    	// ͨ��ɫ���л�                     
	CFG_TIMEPERIOD_SWITCH_MODE_BYBRIGHTNESS,                               	// ͨ�������л�
	CFG_TIMEPERIOD_SWITCH_MODE_BYPOS,										// ͨ����γ�ȼ����ճ�����ʱ���л�
}CFG_TIMEPERIOD_SWITCH_MODE;

// �ೡ���궨����ͺ�ҹ���û�����Ԫ
typedef struct tagCFG_TIME_PERIOD_SCENE_UNIT
{
	CFG_TIME_PERIOD        stuDayTimePeriod;								 	// �궨�����ʱ���.(8,20),��ʾ��8�㵽����20��Ϊ����
	CFG_TIME_PERIOD        stuNightTimePeriod;								 	// �궨��ҹ��ʱ���.(20,7)����ʾ����8�㵽�賿7��Ϊ��ҹ
	CFG_TIMEPERIOD_SWITCH_MODE emSwitchMode;                                  	// ��ҹ�㷨�л�ģʽ�����CFG_TIMEPERIOD_SWITCH_MODE
	UINT					unLongitude;										// ���� ��λ�����֮һ��
	UINT					unLatitude;											// γ�� ��λ�����֮һ��
}
CFG_TIME_PERIOD_SCENE_UNIT;

// �ೡ���궨����ͺ�ҹ����
typedef struct tagCFG_TIME_PERIOD_SCENE_INFO
{
	DWORD                        dwMaxTimePeriodSceneNum;                    // �ೡ���궨����ͺ�ҹ����������(��Ҫ����˴�С�ڴ�)
	DWORD                        dwRetTimePeriodSceneNum;                    // ʵ�ʰ����ೡ���궨����ͺ�ҹ���ø���
	CFG_TIME_PERIOD_SCENE_UNIT   *pstuTimePeriodScene;                       // �ೡ�������ͺ�ҹ������Ԫ,���û������ڴ棬��СΪsizeof(CFG_TIME_PERIOD_SCENE_UNIT)*dwMaxTimePeriodSceneNum
}CFG_TIME_PERIOD_SCENE_INFO;

// ��Ƶ����ȫ������
typedef struct tagCFG_ANALYSEGLOBAL_INFO
{
	// ��Ϣ
	char				            szSceneType[MAX_NAME_LEN];				// Ӧ�ó���,���"֧�ֵĳ����б�"

	//��ͨ������Ϣ
    double				            CameraHeight;							// ����ͷ��ظ߶�	��λ����
	double				            CameraDistance;						    // ����ͷ��������������ĵ�ˮƽ����	��λ����
	CFG_POLYGON                     stuNearDetectPoint;                     // ��������
	CFG_POLYGON                     stuFarDectectPoint;                     // Զ������
	int                             nNearDistance;                          // NearDetectPoint,ת����ʵ�ʳ�����ʱ,������ͷ��ֱ�ߵ�ˮƽ����
	int                             nFarDistance;                           // FarDectectPoint,ת����ʵ�ʳ�����ʱ,������ͷ��ֱ�ߵ�ˮƽ����
	char                            szSubType[MAX_NAME_LEN];                // ��ͨ������������,"Gate",��������,"Junction" ·������,"ParkingSpace" ��λ�������                             
	int                             nLaneNum;                               // ������
	CFG_LANE                        stuLanes[MAX_LANE_NUM];                 // ������Ϣ
    int                             nPlateHintNum;                          // �����ַ���ʾ����
    char                            szPlateHints[MAX_PLATEHINT_NUM][MAX_NAME_LEN]; // �����ַ���ʾ���飬������ͼƬ�����ϲ��ʶ��ȷ��ʱ�����ݴ������е��ַ�����ƥ�䣬�����±�ԽС��ƥ�����ȼ�Խ��
	int                             nLightGroupNum;                         // ������
	CFG_LIGHTGROUPS                 stLightGroups[MAX_LIGHTGROUP_NUM];      // ����������Ϣ
    BOOL                            bHangingWordPlate;                      // �Ƿ�ʶ����ֳ���
    BOOL                            bNonStdPolicePlate;                     // �Ƿ�ʶ��Ǳ�׼����
    BOOL                            bYellowPlateLetter;                     // �Ƿ�ʶ�������ĸ
    int                             nReportMode;                            // �ϱ�ģʽ��0��δ���壬1���ϱ�����Υ�³�����2���ϱ�����ʶ��ɹ���Υ�³���
    int                             nPlateMatch;                            // ����ƥ���ʣ�0~100�����ʶ����ʹ��,��ʾ�ظ�ʶ���ƺ�ǰһ��ʶ���Ƶ�һ����Ҫ��
    int                             nJudgment;                              // Υ���ж����ݣ�0��δ���壬1����������2������ʻ����3������������ʻ����
    int                             nLeftDivisionPtCount;                   // ��ת��ֽ��ߵ���
    CFG_POLYLINE                    stLeftDivisionLine[MAX_POLYLINE_NUM];   // ��ת��ֽ���
    int                             nRightDivisionPtCount;                  // ��ת��ֽ��ߵ���
    CFG_POLYLINE                    stRightDivisionLine[MAX_POLYLINE_NUM];  // ��ת��ֽ���
    CFG_ADJUST_LIGHT_COLOR          stAdjustLightColor;                     // ��ͨ����ɫУ������
    int                             nParkingSpaceNum;                       // ��λ��                             
    CFG_PARKING_SPACE               stParkingSpaces[MAX_PARKING_SPACE_NUM];  // ��λ������Ϣ,ÿ��Ԫ�ش���һ����λ


	// һ�㳡����Ϣ 
	int                             nStaffNum;                              // �����
	CFG_STAFF                       stuStaffs[MAX_STAFF_NUM];               // ���

	unsigned int                    nCalibrateAreaNum;                      // �궨������
	CFG_CALIBRATEAREA_INFO          stuCalibrateArea[MAX_CALIBRATEBOX_NUM]; // �궨����(�����ֶβ����ڣ�������������Ϊ�궨����)

	BOOL                            bFaceRecognition;                       // ����ʶ�𳡾��Ƿ���Ч
	CFG_FACERECOGNITION_SCENCE_INFO stuFaceRecognitionScene;                // ����ʶ�𳡾�
		
	bool                            abJitter;
	bool                            abDejitter;	
	BYTE                            bReserved[2];                           // �����ֶ�

	int					            nJitter;								// ����������� : ����������ʣ�ȡֵ0-100����Ӧ��ֹ����������̶ȣ�����Խ������ֵԽ��
	BOOL                            bDejitter;                              // �Ƿ���ȥ����ģ�� Ŀǰ��ʵ��

	BOOL                            abCompatibleMode;	
	int                             nCompatibleMode;                        // 0:"OldTrafficRule" : ��ͨ�Ϲ������ģʽ;1:"NewTrafficRule" :  ��ͨ�¹������ģʽ;-1:�ַ�������

	int                             nCustomDataLen;                         // ʵ�����ݳ��ȣ����ܴ���1024
	BYTE                            byCustomData[1024];                     // �������Զ�����������
	double				            CameraAngle;							// ����ͷ�봹����ļн�
	CFG_POLYGON			            stuLandLineStart;						// ��ƽ���߶���ʼ��(������������һ����[0,8192)���䡣)
	CFG_POLYGON			            stuLandLineEnd;							// ��ƽ���߶���ֹ��(������������һ����[0,8192)���䡣)

	BOOL				            bFaceDetection;							// ������ⳡ���Ƿ���Ч
	CFG_FACEDETECTION_SCENCE_INFO   stuFaceDetectionScene;		            // ������ⳡ��
	CFG_TIME_PERIOD		            stuDayTimePeriod;						// �궨�����ʱ���.(8,20),��ʾ��8�㵽����20��Ϊ����
	CFG_TIME_PERIOD		            stuNightTimePeriod;						// �궨��ҹ��ʱ���.(20,7)����ʾ����8�㵽�賿7��Ϊ��ҹ
	CFG_TIME_PERIOD_SCENE_INFO      stuTimePeriodSceneInfo;                 // �ೡ���궨����ͺ�ҹʱ���
	CFG_CALIBRATEAREA_SCENE_INFO    stuCalibrateAreaSceneInfo;              // �ೡ���궨����������Ϣ
	CFG_TIMEPERIOD_SWITCH_MODE      emSwitchMode;                           // ��ҹ�㷨�л�ģʽ,���CFG_TIMEPERIOD_SWITCH_MODE

	int					            nSceneNum;								// ������, >0ʱ��ʾ֧�ֶೡ��, stuMultiScene��Ч
	CFG_ANALYSEGLOBAL_SCENE	        stuMultiScene[MAX_ANALYSE_SCENE_NUM];	// �ೡ������

	int								nSceneCount;							// ʵ�ʳ�������
	char							szSceneTypeList[MAX_SCENE_TYPE_LIST_SIZE][CFG_COMMON_STRING_16];// �����б�ͬһ��Ƶͨ�������ö������ʱ����ʾ��2��֮��ķ���(��ѡ)
	EM_DEPTH_TYPE					emDepthType;							// ���澰��
	int								nPtzPresetId;							// ��̨Ԥ�õ��ţ�0~255
	UINT							unLongitude;							// ���� ��λ�����֮һ��
	UINT							unLatitude;								// γ�� ��λ�����֮һ��
} CFG_ANALYSEGLOBAL_INFO;

// �ߴ������
typedef struct tagCFG_SIZEFILTER_INFO
{
	int                   nCalibrateBoxNum;                       // У׼�����
	CFG_CALIBRATEBOX_INFO stuCalibrateBoxs[MAX_CALIBRATEBOX_NUM]; // У׼��(Զ�˽��˱궨ģʽ����Ч)
	bool                bMeasureModeEnable;                       // ������ʽ�����Ƿ���Ч
	BYTE                bMeasureMode;                             // ������ʽ,0-���أ�����ҪԶ�ˡ����˱궨, 1-ʵ�ʳ��ȣ���λ����, 2-Զ�˽��˱궨�������
	bool                bFilterTypeEnable;                        // �������Ͳ����Ƿ���Ч
	// ByArea,ByRatio�������ݣ�ʹ�ö�����ByArea��ByRatioѡ����� 2012/03/06
	BYTE				bFilterType;			 				  // ��������:0:"ByLength",1:"ByArea", 2"ByWidthHeight"
	BYTE                bReserved[2];                             // �����ֶ�
	bool                bFilterMinSizeEnable;                     // ������С�ߴ�����Ƿ���Ч
	bool                bFilterMaxSizeEnable;                     // �������ߴ�����Ƿ���Ч
	CFG_SIZE			stuFilterMinSize;						  // ������С�ߴ� "ByLength"ģʽ�±�ʾ��ߵĳߴ磬"ByArea"ģʽ�¿��ʾ���������Ч(Զ�˽��˱궨ģʽ�±�ʾ��׼��Ŀ�߳ߴ�)��
	CFG_SIZE			stuFilterMaxSize;				          // �������ߴ� "ByLength"ģʽ�±�ʾ��ߵĳߴ磬"ByArea"ģʽ�¿��ʾ���������Ч(Զ�˽��˱궨ģʽ�±�ʾ��׼��Ŀ�߳ߴ�)��
	
	bool                abByArea;
	bool                abMinArea;
	bool                abMaxArea;
	bool                abMinAreaSize;
	bool                abMaxAreaSize;
	bool                bByArea;                                  // �Ƿ�������� ͨ������ComplexSizeFilter�ж��Ƿ����
	bool                bReserved1[2];                            // ����
	float               nMinArea;                                 // ��С���
	float               nMaxArea;                                 // ������
	CFG_SIZE            stuMinAreaSize;                           // ��С������ο�ߴ� "������ʽ"Ϊ"����"ʱ����ʾ��С������ο�Ŀ�߳ߴ磻"������ʽ"Ϊ"Զ�˽��˱궨ģʽ"ʱ����ʾ��׼�����С��߳ߴ磻
	CFG_SIZE            stuMaxAreaSize;                           // ���������ο�ߴ�, ͬ��
	
	bool                abByRatio;
	bool                abMinRatio;
	bool                abMaxRatio;
	bool                abMinRatioSize;
	bool                abMaxRatioSize;
	bool                bByRatio;                                 // �Ƿ񰴿�߱ȹ��� ͨ������ComplexSizeFilter�ж��Ƿ����
	bool                bReserved2[6];                            // ����
	double              dMinRatio;                                // ��С��߱�
	double              dMaxRatio;                                // ����߱�
	CFG_SIZE            stuMinRatioSize;                          // ��С��߱Ⱦ��ο�ߴ磬��С��߱ȶ�Ӧ���ο�Ŀ�߳ߴ硣
	CFG_SIZE            stuMaxRatioSize;                          // ����߱Ⱦ��ο�ߴ磬ͬ��
	
	int                 nAreaCalibrateBoxNum;                     // ���У׼�����
	CFG_CALIBRATEBOX_INFO stuAreaCalibrateBoxs[MAX_CALIBRATEBOX_NUM];// ���У׼��
	int                 nRatioCalibrateBoxs;                      // ���У׼�����
	CFG_CALIBRATEBOX_INFO stuRatioCalibrateBoxs[MAX_CALIBRATEBOX_NUM];// ���У׼��
	
	bool                abBySize;                                 // �������ʹ�ܲ����Ƿ���Ч
	bool                bBySize;                                  // �������ʹ��
	BYTE                bReserved3[6];                            // �����ֶ�
}CFG_SIZEFILTER_INFO;

// ���������ض��Ĺ�����
typedef struct tagCFG_OBJECT_SIZEFILTER_INFO
{
	char				szObjectType[MAX_NAME_LEN];              // ��������
	CFG_SIZEFILTER_INFO stSizeFilter;                            // ��Ӧ�ĳߴ������
}CFG_OBJECT_SIZEFILTER_INFO;

// ������������ָ�Ӽ���������ֳ����������������Ե�����
typedef struct tagCFG_SPECIALDETECTREGION_INFO
{
	int                nDetectNum;                              // ������򶥵���
	CFG_POLYGON        stDetectRegion[MAX_POLYGON_NUM];         // �������
	int                nPropertyNum;                            // �����������Ը���
	int                nPropertys[EM_SEPCIALREGION_PROPERTY_TYPE_NUM];      // ������������
}CFG_SPECIALDETECT_INFO;

//���������������
typedef enum tagCFG_CATEGORY_TYPE
{
    CFG_CATEGORY_TYPE_UNKNOW,                            // δ֪����
    //������������
    CFG_CATEGORY_VEHICLE_TYPE_MOTOR,                     // "Motor" ������           
    CFG_CATEGORY_VEHICLE_TYPE_NON_MOTOR,                 // "Non-Motor"�ǻ�����        
    CFG_CATEGORY_VEHICLE_TYPE_BUS,                       // "Bus"������        
    CFG_CATEGORY_VEHICLE_TYPE_BICYCLE,                   // "Bicycle" ���г�        
    CFG_CATEGORY_VEHICLE_TYPE_MOTORCYCLE,                // "Motorcycle"Ħ�г�        
    CFG_CATEGORY_VEHICLE_TYPE_UNLICENSEDMOTOR,           // "UnlicensedMotor": ���ƻ�����
    CFG_CATEGORY_VEHICLE_TYPE_LARGECAR,                  // "LargeCar"  ��������
    CFG_CATEGORY_VEHICLE_TYPE_MICROCAR,                  // "MicroCar" С������
    CFG_CATEGORY_VEHICLE_TYPE_EMBASSYCAR,                // "EmbassyCar" ʹ������
    CFG_CATEGORY_VEHICLE_TYPE_MARGINALCAR,               // "MarginalCar" �������
    CFG_CATEGORY_VEHICLE_TYPE_AREAOUTCAR,                // "AreaoutCar" ��������
    CFG_CATEGORY_VEHICLE_TYPE_FOREIGNCAR,                // "ForeignCar" �⼮����
    CFG_CATEGORY_VEHICLE_TYPE_DUALTRIWHEELMOTORCYCLE,    // "DualTriWheelMotorcycle"��������Ħ�г�
    CFG_CATEGORY_VEHICLE_TYPE_LIGHTMOTORCYCLE,           // "LightMotorcycle" ���Ħ�г�
    CFG_CATEGORY_VEHICLE_TYPE_EMBASSYMOTORCYCLE,         // "EmbassyMotorcycle "ʹ��Ħ�г�
    CFG_CATEGORY_VEHICLE_TYPE_MARGINALMOTORCYCLE,        // "MarginalMotorcycle "���Ħ�г�
    CFG_CATEGORY_VEHICLE_TYPE_AREAOUTMOTORCYCLE,         // "AreaoutMotorcycle "����Ħ�г�
    CFG_CATEGORY_VEHICLE_TYPE_FOREIGNMOTORCYCLE,         // "ForeignMotorcycle "�⼮Ħ�г�
    CFG_CATEGORY_VEHICLE_TYPE_FARMTRANSMITCAR,           // "FarmTransmitCar" ũ�����䳵
    CFG_CATEGORY_VEHICLE_TYPE_TRACTOR,                   // "Tractor" ������
    CFG_CATEGORY_VEHICLE_TYPE_TRAILER,                   // "Trailer"  �ҳ�
    CFG_CATEGORY_VEHICLE_TYPE_COACHCAR,                  // "CoachCar"��������
    CFG_CATEGORY_VEHICLE_TYPE_COACHMOTORCYCLE,           // "CoachMotorcycle "����Ħ�г�
    CFG_CATEGORY_VEHICLE_TYPE_TRIALCAR,                  // "TrialCar" ��������
    CFG_CATEGORY_VEHICLE_TYPE_TRIALMOTORCYCLE,           // "TrialMotorcycle "����Ħ�г�
    CFG_CATEGORY_VEHICLE_TYPE_TEMPORARYENTRYCAR,         // "TemporaryEntryCar"��ʱ�뾳����
    CFG_CATEGORY_VEHICLE_TYPE_TEMPORARYENTRYMOTORCYCLE,  // "TemporaryEntryMotorcycle"��ʱ�뾳Ħ�г�
    CFG_CATEGORY_VEHICLE_TYPE_TEMPORARYSTEERCAR,         // "TemporarySteerCar"��ʱ��ʻ��
    CFG_CATEGORY_VEHICLE_TYPE_PASSENGERCAR,              // "PassengerCar" �ͳ�
    CFG_CATEGORY_VEHICLE_TYPE_LARGETRUCK,                // "LargeTruck" �����
    CFG_CATEGORY_VEHICLE_TYPE_MIDTRUCK,                  // "MidTruck" �л���
    CFG_CATEGORY_VEHICLE_TYPE_SALOONCAR,                 // "SaloonCar" �γ�
    CFG_CATEGORY_VEHICLE_TYPE_MICROBUS,                  // "Microbus"�����
    CFG_CATEGORY_VEHICLE_TYPE_MICROTRUCK,                // "MicroTruck"С����
    CFG_CATEGORY_VEHICLE_TYPE_TRICYCLE,                  // "Tricycle"���ֳ�
    CFG_CATEGORY_VEHICLE_TYPE_PASSERBY,                  // "Passerby" ����
	CFG_CATEGORY_VEHICLE_TYPE_OILTANKTRUCK,				 // "OilTankTruck" �͹޳�
	CFG_CATEGORY_VEHICLE_TYPE_TANKCAR,					 // "TankCar" �۹޳�
	CFG_CATEGORY_VEHICLE_TYPE_SUV,					     // "SUV" SUV
	CFG_CATEGORY_VEHICLE_TYPE_MPV,					     // "MPV" MPV

    //������������
    CFG_CATEGORY_PLATE_TYPE_NORMAL,                      // "Normal" ���ƺ���
    CFG_CATEGORY_PLATE_TYPE_YELLOW,                      // "Yellow" ����
    CFG_CATEGORY_PLATE_TYPE_DOUBLEYELLOW,                // "DoubleYellow" ˫���β��
    CFG_CATEGORY_PLATE_TYPE_POLICE,                      // "Police" ����
    CFG_CATEGORY_PLATE_TYPE_ARMED,                       // "Armed" �侯��
    CFG_CATEGORY_PLATE_TYPE_MILITARY,                    // "Military" ���Ӻ���
    CFG_CATEGORY_PLATE_TYPE_DOUBLEMILITARY,              // "DoubleMilitary" ����˫��
    CFG_CATEGORY_PLATE_TYPE_SAR,                         // "SAR" �۰���������    
    CFG_CATEGORY_PLATE_TYPE_TRAINNING,                   // "Trainning" ����������
    CFG_CATEGORY_PLATE_TYPE_PERSONAL,                    // "Personal" ���Ժ���
    CFG_CATEGORY_PLATE_TYPE_AGRI,                        // "Agri" ũ����
    CFG_CATEGORY_PLATE_TYPE_EMBASSY,                     // "Embassy" ʹ�ݺ���
    CFG_CATEGORY_PLATE_TYPE_MOTO,                        // "Moto" Ħ�г�����
    CFG_CATEGORY_PLATE_TYPE_TRACTOR,                     // "Tractor" ����������
    CFG_CATEGORY_PLATE_TYPE_OFFICIALCAR,                 // "OfficialCar " ����
    CFG_CATEGORY_PLATE_TYPE_PERSONALCAR,                 // "PersonalCar" ˽�ҳ�
    CFG_CATEGORY_PLATE_TYPE_WARCAR,                      // "WarCar"  ����
    CFG_CATEGORY_PLATE_TYPE_OTHER,                       // "Other" ��������
    CFG_CATEGORY_PLATE_TYPE_CIVILAVIATION,               // "Civilaviation" �񺽺���
    CFG_CATEGORY_PLATE_TYPE_BLACK,                       // "Black" ����
	CFG_CATEGORY_PLATE_TYPE_PURENEWENERGYMICROCAR,       // "PureNewEnergyMicroCar" ���綯����ԴС��
	CFG_CATEGORY_PLATE_TYPE_MIXEDNEWENERGYMICROCAR,      // "MixedNewEnergyMicroCar" �������ԴС��
	CFG_CATEGORY_PLATE_TYPE_PURENEWENERGYLARGECAR,       // "PureNewEnergyLargeCar" ���綯����Դ��
	CFG_CATEGORY_PLATE_TYPE_MIXEDNEWENERGYLARGECAR,      // "MixedNewEnergyLargeCar" �������Դ��

}CFG_CATEGORY_TYPE;

// ������ʽ
typedef enum tagEM_CFG_MEASURE_MODE_TYPE
{
	EM_CFG_NEASURE_MODE_TYPE_UNKNOW,				// δ֪
	EM_CFG_NEASURE_MODE_TYPE_PIXEL,					// ����
	EM_CFG_NEASURE_MODE_TYPE_METRIC,				// ʵ�ʳ���
}EM_CFG_MEASURE_MODE_TYPE;

// ��������
typedef enum tagCFG_FILTER_HEIGHT_TYPE
{
	CFG_FILTER_HEIGHT_TYPE_UNKNOW,				// δ֪
	CFG_FILTER_HEIGHT_TYPE_BYHEIGHT,			// �߶�
}CFG_FILTER_HEIGHT_TYPE;

// �������ͳ��ȹ�����
typedef struct tagCFG_LENGTH_FILTER_INFO
{
	EM_CFG_MEASURE_MODE_TYPE	emMeasureMode;	// ������ʽ
	CFG_FILTER_HEIGHT_TYPE		emFilterType;	// ��������
	int							nDetectType;	// 0:������С�� 1:���ڻ������С�ڻ���� 2:������С�ڻ���� 3:���ڻ������С��
	int							nMinLen;		// ��С��ⳤ�ȣ���λ��cm
	int							nMaxLen;		// ����ⳤ�ȣ���λ��cm
}CFG_LENGTH_FILETER_INFO;

// ��ͬ���������������ļ��ģ������
typedef struct tagCFG_MODULE_INFO
{
	// ��Ϣ
	char				szObjectType[MAX_NAME_LEN];				// Ĭ����������,���"֧�ֵļ�����������б�"
	bool				bSnapShot;								// �Ƿ��ʶ������ץͼ
	BYTE                bSensitivity;                           // ������,ȡֵ1-10��ֵԽС������Խ��
	bool                bMeasureModeEnable;                     // ������ʽ�����Ƿ���Ч
	BYTE                bMeasureMode;                           // ������ʽ,0-���أ�����ҪԶ�ˡ����˱궨, 1-ʵ�ʳ��ȣ���λ����, 2-Զ�˽��˱궨�������
	int					nDetectRegionPoint;						// ������򶥵���
	CFG_POLYGON			stuDetectRegion[MAX_POLYGON_NUM];		// �������
	int					nTrackRegionPoint;						// �������򶥵���
	CFG_POLYGON			stuTrackRegion[MAX_POLYGON_NUM];		// ��������
	bool                bFilterTypeEnable;                      // �������Ͳ����Ƿ���Ч
	// ByArea,ByRatio�������ݣ�ʹ�ö�����ByArea��ByRatioѡ����� 2012/03/06
	BYTE				nFilterType;							// ��������:0:"ByLength",1:"ByArea", 2:"ByWidthHeight", 3:"ByRatio": ���տ�߱ȣ���ȳ��Ը߶ȵĽ��С��ĳ��ֵ���ߴ���ĳ��ֵ�����⽫�����˵���
	bool                bBackgroudEnable;                       // ����ı������Ͳ����Ƿ���Ч
	BYTE                bBackgroud;                             // ����ı�������, 0-��ͨ����, 1-�߹�����
	bool                abBySize;                               // �������ʹ�ܲ����Ƿ���Ч
	bool                bBySize;                                // �������ʹ��
	bool                bFilterMinSizeEnable;                   // ������С�ߴ�����Ƿ���Ч
	bool                bFilterMaxSizeEnable;                   // �������ߴ�����Ƿ���Ч
	CFG_SIZE			stuFilterMinSize;						// ������С�ߴ� "ByLength"ģʽ�±�ʾ��ߵĳߴ磬"ByArea"ģʽ�¿��ʾ���������Ч��
	CFG_SIZE			stuFilterMaxSize;						// �������ߴ� "ByLength"ģʽ�±�ʾ��ߵĳߴ磬"ByArea"ģʽ�¿��ʾ���������Ч��
	int                 nExcludeRegionNum;                      // �ų�������
	CFG_REGION          stuExcludeRegion[MAX_EXCLUDEREGION_NUM];// �ų�����
	int                 nCalibrateBoxNum;                       // У׼�����
	CFG_CALIBRATEBOX_INFO stuCalibrateBoxs[MAX_CALIBRATEBOX_NUM];// У׼��(Զ�˽��˱궨ģʽ����Ч)
	bool                bAccuracy;                               // ��⾫���Ƿ���Ч
	BYTE                byAccuracy;                              // ��⾫��
	bool                bMovingStep;                             // �㷨�ƶ������Ƿ���Ч                        
	BYTE                byMovingStep;                            // �㷨�ƶ�����
	bool                bScalingFactor;							 // �㷨���������Ƿ���Ч
	BYTE                byScalingFactor;						 // �㷨��������

	BYTE                bReserved2[1];                           // �����ֶ�
	bool                abDetectBalance;						 // ©������ƽ������Ƿ���Ч
	int					nDetectBalance;							 // ©������ƽ��	0-����ģʽ(Ĭ��)1-©�����2-������
	
	bool                abByRatio;
	bool                abMinRatio;
	bool                abMaxRatio;
	bool                abMinAreaSize;
	bool                abMaxAreaSize;
	bool                bByRatio;                                // �Ƿ񰴿�߱ȹ��� ͨ������ComplexSizeFilter�ж��Ƿ���� ���Ժ�nFilterType����
	bool                bReserved1[2];
	double              dMinRatio;                               // ��С��߱�
	double              dMaxRatio;                               // ����߱�
	CFG_SIZE            stuMinAreaSize;                           // ��С������ο�ߴ� "������ʽ"Ϊ"����"ʱ����ʾ��С������ο�Ŀ�߳ߴ磻"������ʽ"Ϊ"Զ�˽��˱궨ģʽ"ʱ����ʾ��׼�����С��߳ߴ磻
	CFG_SIZE            stuMaxAreaSize;                           // ���������ο�ߴ�, ͬ��
	
	bool                abByArea;
	bool                abMinArea;
	bool                abMaxArea;
	bool                abMinRatioSize;
	bool                abMaxRatioSize;
	bool                bByArea;                                 // �Ƿ�������� ͨ������ComplexSizeFilter�ж��Ƿ����  ���Ժ�nFilterType����
	bool                bReserved3[2];
	float               nMinArea;                                // ��С���
	float               nMaxArea;                                // ������
	CFG_SIZE            stuMinRatioSize;                          // ��С��߱Ⱦ��ο�ߴ磬��С��߱ȶ�Ӧ���ο�Ŀ�߳ߴ硣
	CFG_SIZE            stuMaxRatioSize;                          // ����߱Ⱦ��ο�ߴ磬ͬ��
	
	int                 nAreaCalibrateBoxNum;                    // ���У׼�����
	CFG_CALIBRATEBOX_INFO stuAreaCalibrateBoxs[MAX_CALIBRATEBOX_NUM];// ���У׼��
	int                 nRatioCalibrateBoxs;                      // ����У׼�����
	CFG_CALIBRATEBOX_INFO stuRatioCalibrateBoxs[MAX_CALIBRATEBOX_NUM];// ����У׼�����

	bool                bAntiDisturbance;                        // �Ƿ���ȥ�Ŷ�ģ��
	bool                bBacklight;                              // �Ƿ������
	bool                bShadow;                                 // �Ƿ�����Ӱ
	bool                bContourAssistantTrack;                  // �Ƿ��������������٣�����������ʶ��ʱ����ͨ����������������ʶ������
	int                 nPtzPresetId;                            // ��̨Ԥ�õ㣬0~255��0��ʾ�̶�����������Ԥ�õ㡣����0��ʾ�ڴ�Ԥ�õ�ʱģ����Ч

	int                 nObjectFilterNum;                        // �����ض��Ĺ���������
	CFG_OBJECT_SIZEFILTER_INFO stObjectFilter[MAX_OBJECT_LIST_SIZE]; // �����ض��Ĺ�������Ϣ

	BOOL                abObjectImageSize;                        
	CFG_SIZE            stObjectImageSize;                       // ��֤����ͼ��ߴ���ͬ,��λ������,��֧��С����ȡֵ��>=0,  0 ��ʾ�Զ�������С

	int                 nSpecailDetectNum;                                // �������������
	CFG_SPECIALDETECT_INFO stSpecialDetectRegions[MAX_SPECIALDETECT_NUM];  // ����������Ϣ

	unsigned int        nAttribute;                              // ��Ҫʶ����������Ը���
	char                szAttributes[MAX_OBJECT_ATTRIBUTES_SIZE][MAX_NAME_LEN];   // ��Ҫʶ������������б���Category�� �������ࣻ��Size�� �����С����Color�� ������ɫ����Speed�� �����ٶȣ���Sign�� �����־���Գ������ԣ�ָ���ꣻ��Action�� ���嶯��
	
	BOOL                abPlateAnalyseMode;                      // nPlateAnalyseMode�Ƿ���Ч
	int                 nPlateAnalyseMode;                       // ����ʶ��ģʽ��0-ֻʶ��ͷ���� 1-ֻʶ��β���� 2-��ͷ�������ȣ������д󲿷ֳ����ǳ�ͷ���գ�3-��β�������ȣ������д󲿷ֳ����ǳ�β���գ�

    //szAttributes���Դ���"Category"ʱ��Ч
    unsigned int        nCategoryNum;                            // ��Ҫʶ�����������������
    CFG_CATEGORY_TYPE   emCategoryType[MAX_CATEGORY_TYPE_NUMBER]; // ��������Ϣ
	char				szSceneType[CFG_COMMON_STRING_16];		// ������������ڵĳ�������
	CFG_LENGTH_FILETER_INFO		stuLengthFilter;				// �������͹����������ָ���µĹ��������µ�Ϊ׼
} CFG_MODULE_INFO;

typedef struct tagCFG_ANALYSEMODULES_INFO
{
	int					nMoudlesNum;							    // ���ģ����
	CFG_MODULE_INFO     stuModuleInfo[MAX_ANALYSE_MODULE_NUM];	    // ÿ����Ƶ����ͨ����Ӧ�ĸ�����������ļ��ģ������

} CFG_ANALYSEMODULES_INFO;

// ��Ƶ�����¼�����������ؽṹ�嶨��
enum CFG_EM_DETECTMODE_T{
	CFG_EM_DETECTMODE_NONE,            // �޴��ֶ�
	CFG_EM_DETECTMODE_BY_VIDEO,        // ��Ƶ���ģʽ
	CFG_EM_DETECTMODE_BY_BYAUDIO,      // ��Ƶ���ģʽ
	CFG_EM_DETECTMODE_BY_BOTH,         // ����Ƶ���ϼ��ģʽ
	CFG_EM_DETECTMODE_ERR=255          // �ֶ����ݴ���
};


typedef struct tag_VIDEOINANALYSE_GLOBAL_COMM_INFO
{
	EM_SCENE_TYPE	emClassType;		// Ӧ�ó���
	int				nPtzPresetId;		// Ԥ�õ�
} VIDEOINANALYSE_GLOBAL_COMM_INFO;

typedef struct tagCFG_VIDEOINANALYSE_GLOBAL_SCENE
{
	VIDEOINANALYSE_GLOBAL_COMM_INFO		stuCommInfo;	 						// ������Ϣ
	// ����Ϊ����������Ϣ, ����stuCommInfo�е�emSceneType�����ĸ�������Ч
	union
	{
		CFG_FACEDETECTION_SCENCE_INFO	stuFaceDetectionScene;	// ������ⳡ��/����ʶ����
		CFG_TRAFFIC_SCENE_INFO			stuTrafficScene;		// ��ͨ����
		CFG_NORMAL_SCENE_INFO			stuNormalScene;			// ��ͨ����/Զ������/�о�����/��������/���ڳ���/����ͳ�Ƴ���
		CFG_TRAFFIC_TOUR_SCENE_INFO		stuTrafficTourScene;	// ��ͨѲ�ӳ���
	};
} CFG_VIDEOINANALYSE_GLOBAL_SCENE;

// ȫ������ģ���Ĭ��ֵ
typedef struct tagCFG_VIDEOINANALYSE_GLOBAL_INFO
{
	unsigned int                    	nCalibrateAreaNum;                      // �궨������
	CFG_CALIBRATEAREA_INFO          	stuCalibrateArea[MAX_CALIBRATEBOX_NUM]; // �궨����(�����ֶβ����ڣ�������������Ϊ�궨����)

	CFG_VIDEOINANALYSE_GLOBAL_SCENE		stuVideoInAnalyseScene;					// �ೡ������
}CFG_VIDEOINANALYSE_GLOBAL_INFO;

// ���ܼ��������ģ���Ĭ��ֵ
typedef struct tagCFG_VIDEOINANALYSE_MODULE_INFO
{
	CFG_MODULE_INFO			stuModuleInfo;		// ��ȡ�������ܼ��������ģ���Ĭ��ֵ
}CFG_VIDEOINANALYSE_MODULE_INFO;

// ��������
typedef enum tagEM_CFG_CATEGORY_TYPE
{
	EM_CFG_CATEGORY_UNKNOWN,					// δ֪
	EM_CFG_CATEGORY_OTHER,						// ���� 
	EM_CFG_CATEGORY_MOTOR,						// ������
	EM_CFG_CATEGORY_BUS,						// ������
	EM_CFG_CATEGORY_UNLICENSED_MOTOR,			// ���ƻ�����
	EM_CFG_CATEGORY_LARGE_CAR,					// ��������
	EM_CFG_CATEGORY_MICRO_CAR,					// С������
	EM_CFG_CATEGORY_EMBASSY_CAR,				// ʹ������
	EM_CFG_CATEGORY_MARGINAL_CAR,				// �������
	EM_CFG_CATEGORY_AREAOUT_CAR,				// ��������
	EM_CFG_CATEGORY_FOREIGN_CAR,				// �⼮����
	EM_CFG_CATEGORY_FARMTRANSMIT_CAR,			// ũ�����䳵
	EM_CFG_CATEGORY_TRACTOR,					// ������
	EM_CFG_CATEGORY_TRAILER,					// �ҳ�
	EM_CFG_CATEGORY_COACH_CAR,					// ��������
	EM_CFG_CATEGORY_TRIAL_CAR,					// ��������
	EM_CFG_CATEGORY_TEMPORARY_ENTRY_CAR,		// ��ʱ�뾳����
	EM_CFG_CATEGORY_TEMPORARY_ENTRY_MOTORCYCLE,	// ��ʱ�뾳Ħ��
	EM_CFG_CATEGORY_TEMPORARY_STEER_CAR,		// ��ʱ��ʻ��
	EM_CFG_CATEGORY_LARGE_TRUCK,				// �����
	EM_CFG_CATEGORY_MID_TRUCK,					// �л���
	EM_CFG_CATEGORY_MICRO_TRUCK,				// С����
	EM_CFG_CATEGORY_MICROBUS,					// �����
	EM_CFG_CATEGORY_SALOON_CAR,					// �γ�
	EM_CFG_CATEGORY_CARRIAGE,					// С�γ�
	EM_CFG_CATEGORY_MINI_CARRIAGE,				// ΢�ͽγ�
	EM_CFG_CATEGORY_SUV_MPV,					// SUV����MPV
	EM_CFG_CATEGORY_SUV,						// SUV
	EM_CFG_CATEGORY_MPV,						// MPV
	EM_CFG_CATEGORY_PASSENGER_CAR,				// �ͳ�
	EM_CFG_CATEGORY_MOTOR_BUS,					// ���
	EM_CFG_CATEGORY_MID_PASSENGER_CAR,			// �пͳ�
	EM_CFG_CATEGORY_MINI_BUS,					// С�ͳ�
	EM_CFG_CATEGORY_PICKUP,						// Ƥ����
	EM_CFG_CATEGORY_OILTANK_TRUCK,				// �͹޳�
	EM_CFG_CATEGORY_TANK_CAR,					// Σ��Ʒ����
	EM_CFG_CATEGORY_SLOT_TANK_CAR,				// �۹޳�
	EM_CFG_CATEGORY_DREGS_CAR,					// ������
	EM_CFG_CATEGORY_CONCRETE_MIXER_TRUCK,		// ���������賵
	EM_CFG_CATEGORY_TAXI,						// ���⳵
	EM_CFG_CATEGORY_POLICE,						// ����
	EM_CFG_CATEGORY_AMBULANCE,					// �Ȼ���
	EM_CFG_CATEGORY_GENERAL,					// ��ͨ��
	EM_CFG_CATEGORY_WATERING_CAR,				// ��ˮ��
	EM_CFG_CATEGORY_FIRE_ENGINE,				// ������
	EM_CFG_CATEGORY_MACHINE_TRUCK,				// ���̳�
	EM_CFG_CATEGORY_POWER_LOT_VEHICLE,			// �������ϳ�
	EM_CFG_CATEGORY_SUCTION_SEWAGE_TRUCK,		// ���۳�
	EM_CFG_CATEGORY_NORMAL_TANK_TRUCK,			// ��ͨ�޳�
	EM_CFG_CATEGORY_SCHOOL_BUS,					// У��
	EM_CFG_CATEGORY_EXCAVATOR,					// �ھ�
	EM_CFG_CATEGORY_BULLDOZER,					// ������
	EM_CFG_CATEGORY_CRANE,						// ����
	EM_CFG_CATEGORY_PIMP_TRUCK,					// �ó�
} EM_CFG_CATEGORY_TYPE;

// ��Ƶ�����¼���������
// �¼����� EVENT_IVS_CROSSLINEDETECTION (�������¼�)��Ӧ�Ĺ�������
typedef struct tagCFG_CROSSLINE_INFO
{
	// ��Ϣ
	char					szRuleName[MAX_NAME_LEN];								// ��������,��ͬ����������
	bool					bRuleEnable;											// ����ʹ��
    BYTE                	bTrackEnable;                                           // ��������ʹ��,���԰��ߣ����ֹ�����Ч
	BYTE                	bReserved[2];                                           // �����ֶ� 
	int						nObjectTypeNum;											// ��Ӧ�������͸���
	char					szObjectTypes[MAX_OBJECT_LIST_SIZE][MAX_NAME_LEN];		// ��Ӧ���������б�
	int						nDirection;												// ��ⷽ��:0:��������;1:��������;2:���߶�����
	int						nDetectLinePoint;										// �����߶�����
	CFG_POLYLINE			stuDetectLine[MAX_POLYLINE_NUM];						// ������
	CFG_ALARM_MSG_HANDLE 	stuEventHandler;										// ��������
	
	CFG_TIME_SECTION		stuTimeSection[WEEK_DAY_NUM][MAX_REC_TSECT_EX];			// �¼���Ӧʱ���
 	BOOL                	bDisableTimeSection;                                    // stuTimeSection�ֶ��Ƿ���ã�Ĭ��FALSE�������ã�TRUE�����ã��û�����

	int                 	nPtzPresetId;											// ��̨Ԥ�õ���	0~65535
	BOOL                	bSizeFileter;                                           // �����ض��ĳߴ�������Ƿ���Ч
	CFG_SIZEFILTER_INFO 	stuSizeFileter;                                         // �����ض��ĳߴ������
	int                 	nTriggerPosition;                                       // ��������λ����
	BYTE                	bTriggerPosition[8];                                    // ��������λ��,0-Ŀ����ӿ�����, 1-Ŀ����ӿ��������, 2-Ŀ����ӿ򶥶�����, 3-Ŀ����ӿ��Ҷ�����, 4-Ŀ����ӿ�׶�����
	int						nTrackDuration;											// ���ٳ���ʱ��,0��:һֱ����,1~300��:���ٳ���ʱ��
	UINT					nVehicleSubTypeNum;										// ���ĳ��������͸���
	EM_CFG_CATEGORY_TYPE	emVehicleSubType[128];									// ���ĳ����������б�
    BOOL                    bFeatureEnable;                                         // �Ƿ���Ŀ������ʶ��,Ĭ��false
} CFG_CROSSLINE_INFO;

//�¼�����EVENT_IVS_CROSSFENCEDETECTION(��ԽΧ������)��Ӧ�Ĺ�������
typedef struct tagCFG_CROSSFENCEDETECTION_INFO
{
	// ��Ϣ
	char				szRuleName[MAX_NAME_LEN];								// ��������,��ͬ����������
	bool				bRuleEnable;											// ����ʹ��
    BYTE                bTrackEnable;                                           // ��������ʹ��,���԰��ߣ����ֹ�����Ч
	BYTE                bReserved[2];                                           // �����ֶ� 
	int					nObjectTypeNum;											// ��Ӧ�������͸���
	char				szObjectTypes[MAX_OBJECT_LIST_SIZE][MAX_NAME_LEN];		// ��Ӧ���������б�
	int                 nPtzPresetId;											// ��̨Ԥ�õ���	0~65535
	int					nDirection;												// ��ԽΧ������:0:��������;1:��������;2:���߶�����
	int					nUpstairsLinePointNumber;								// Χ�����߶�����
	CFG_POLYLINE		stuUpstairsLine[MAX_POLYLINE_NUM];						// Χ������
	int					nDownstairsLinePointNumber;								// Χ�����߶�����
	CFG_POLYLINE		stuDownstairsLine[MAX_POLYLINE_NUM];					// Χ������
	BOOL                bSizeFileter;                                           // �����ض��ĳߴ�������Ƿ���Ч
	CFG_SIZEFILTER_INFO stuSizeFileter;                                         // �����ض��ĳߴ������
	int                 nTriggerPosition;                                       // ��������λ����
	BYTE                bTriggerPosition[8];                                    // ��������λ��,0-Ŀ����ӿ�����, 1-Ŀ����ӿ��������, 2-Ŀ����ӿ򶥶�����, 3-Ŀ����ӿ��Ҷ�����, 4-Ŀ����ӿ�׶�����
	CFG_ALARM_MSG_HANDLE stuEventHandler;										// ��������
	CFG_TIME_SECTION	stuTimeSection[WEEK_DAY_NUM][MAX_REC_TSECT_EX];			// �¼���Ӧʱ���
	int					nTrackDuration;											// ���ٳ���ʱ��,0��:һֱ����,1~300��:���ٳ���ʱ��
} CFG_CROSSFENCEDETECTION_INFO;

// �¼�����EVENT_IVS_CROSSREGIONDETECTION(�������¼�)��Ӧ�Ĺ�������
typedef struct tagCFG_CROSSREGION_INFO
{
	// ��Ϣ
	char					szRuleName[MAX_NAME_LEN];								// ��������,��ͬ����������
	bool					bRuleEnable;											// ����ʹ��
    BYTE                	bTrackEnable;                                           // ��������ʹ��,���԰��ߣ����ֹ�����Ч
	BYTE                	bReserved[2];                                           // �����ֶ�
	int						nObjectTypeNum;											// ��Ӧ�������͸���
	char					szObjectTypes[MAX_OBJECT_LIST_SIZE][MAX_NAME_LEN];		// ��Ӧ���������б�
	int						nDirection;												// ��ⷽ��:0:Enter;1:Leave;2:Both
	int						nDetectRegionPoint;										// ������������
	CFG_POLYGON				stuDetectRegion[MAX_POLYGON_NUM];						// ������
	CFG_ALARM_MSG_HANDLE 	stuEventHandler;										// ��������

	CFG_TIME_SECTION		stuTimeSection[WEEK_DAY_NUM][MAX_REC_TSECT_EX];			// �¼���Ӧʱ���
 	BOOL                	bDisableTimeSection;                                    // stuTimeSection�ֶ��Ƿ���ã�Ĭ��FALSE�������ã�TRUE�����ã��û�����

	int                 	nPtzPresetId;											// ��̨Ԥ�õ���	0~65535
	BOOL                	bSizeFileter;                                           // �����ض��ĳߴ�������Ƿ���Ч
	CFG_SIZEFILTER_INFO 	stuSizeFileter;                                         // �����ض��ĳߴ������
	int                 	nActionType;                                            // ��⶯����
	BYTE                	bActionType[4];                                         // ��⶯���б�,0-���� 1-��ʧ 2-�������� 3-��Խ����
	int                 	nMinTargets;                                            // ��СĿ�����(��bActionType�а���"2-��������"ʱ��Ч)
	int                 	nMaxTargets;                                            // ���Ŀ�����(��bActionType�а���"2-��������"ʱ��Ч)
	int                 	nMinDuration;                                           // ��̳���ʱ�䣬 ��λ��(��bActionType�а���"2-��������"ʱ��Ч)
	int                 	nReportInterval;                                        // ����ʱ������ ��λ��(��bActionType�а���"2-��������"ʱ��Ч)
	int						nTrackDuration;											// ���ٳ���ʱ��,0��:һֱ����,1~300��:���ٳ���ʱ��	
	UINT					nVehicleSubTypeNum;										// ���ĳ��������͸���
	EM_CFG_CATEGORY_TYPE	emVehicleSubType[128];									// ���ĳ����������б�
    BOOL                    bFeatureEnable;                                         // �Ƿ���Ŀ������ʶ��,Ĭ��false
} CFG_CROSSREGION_INFO;

// �¼�����EVENT_IVS_PASTEDETECTION(ATM�����¼�)��Ӧ�Ĺ�������
typedef struct tagCFG_PASTE_INFO
{
	// ��Ϣ
	char				szRuleName[MAX_NAME_LEN];								// ��������,��ͬ����������
	bool				bRuleEnable;											// ����ʹ��
	BYTE                bReserved[3];                                           // �����ֶ�
	int					nObjectTypeNum;											// ��Ӧ�������͸���
	char				szObjectTypes[MAX_OBJECT_LIST_SIZE][MAX_NAME_LEN];		// ��Ӧ���������б�
	int					nMinDuration;											// ��̳���ʱ��	��λ���룬0~65535
	int					nDetectRegionPoint;										// ������򶥵���
	CFG_POLYGON			stuDetectRegion[MAX_POLYGON_NUM];						// �������
	CFG_ALARM_MSG_HANDLE stuEventHandler;										// ��������
	CFG_TIME_SECTION	stuTimeSection[WEEK_DAY_NUM][MAX_REC_TSECT_EX];			// �¼���Ӧʱ���
	int                 nPtzPresetId;											// ��̨Ԥ�õ���	0~65535
    int                 nSensitivity;                                           // ������,��Χ[1,10],������Խ��Խ���׼��	
} CFG_PASTE_INFO;

// �¼�����EVENT_IVS_LEFTDETECTION(��Ʒ�����¼�)��Ӧ�Ĺ�������
typedef struct tagCFG_LEFT_INFO 
{
	// ��Ϣ
	char				szRuleName[MAX_NAME_LEN];								// ��������,��ͬ����������
	bool				bRuleEnable;											// ����ʹ��
	BYTE                bTrackEnable;                                           // ��������ʹ��,���԰��ߣ����ֹ�����Ч
	BYTE                bReserved[2];                                           // �����ֶ�
	int					nObjectTypeNum;											// ��Ӧ�������͸���
	char				szObjectTypes[MAX_OBJECT_LIST_SIZE][MAX_NAME_LEN];		// ��Ӧ���������б�
	int					nMinDuration;											// ��̳���ʱ��	��λ���룬0~65535
	int					nDetectRegionPoint;										// ������򶥵���
	CFG_POLYGON			stuDetectRegion[MAX_POLYGON_NUM];						// �������
	CFG_ALARM_MSG_HANDLE stuEventHandler;										// ��������
	CFG_TIME_SECTION	stuTimeSection[WEEK_DAY_NUM][MAX_REC_TSECT_EX];			// �¼���Ӧʱ���
	int                 nPtzPresetId;											// ��̨Ԥ�õ���	0~65535
	int                 nTriggerPosition;                                       // ��������λ����
	BYTE                bTriggerPosition[8];                                    // ��������λ��,0-Ŀ����ӿ�����, 1-Ŀ����ӿ��������, 2-Ŀ����ӿ򶥶�����, 3-Ŀ����ӿ��Ҷ�����, 4-Ŀ����ӿ�׶�����
	BOOL                bSizeFileter;                                           // �����ض��ĳߴ�������Ƿ���Ч
	CFG_SIZEFILTER_INFO stuSizeFileter;                                         // �����ض��ĳߴ������
	int					nTrackDuration;											// ���ٳ���ʱ��,0��:һֱ����,1~300��:���ٳ���ʱ��	
} CFG_LEFT_INFO;

// �¼�����EVENT_IVS_TAKENAWAYDETECTION(��Ʒ���ƹ�������)��Ӧ�Ĺ�������
typedef struct tagCFG_TAKENAWAYDETECTION_INFO 
{
	// ��Ϣ
	char				szRuleName[MAX_NAME_LEN];								// ��������,��ͬ����������
	bool				bRuleEnable;											// ����ʹ��
	BYTE                bTrackEnable;                                           // ��������ʹ��
	BYTE                bReserved[2];                                           // �����ֶ�
	int					nObjectTypeNum;											// ��Ӧ�������͸���
	char				szObjectTypes[MAX_OBJECT_LIST_SIZE][MAX_NAME_LEN];		// ��Ӧ���������б�
	int					nMinDuration;											// ��̳���ʱ��	��λ���룬0~65535
	int					nDetectRegionPoint;										// ������򶥵���
	CFG_POLYGON			stuDetectRegion[MAX_POLYGON_NUM];						// �������
	CFG_ALARM_MSG_HANDLE stuEventHandler;										// ��������
	CFG_TIME_SECTION	stuTimeSection[WEEK_DAY_NUM][MAX_REC_TSECT_EX];			// �¼���Ӧʱ���
	int                 nPtzPresetId;											// ��̨Ԥ�õ���	0~65535
	int                 nTriggerPosition;                                       // ��������λ����
	BYTE                bTriggerPosition[8];                                    // ��������λ��,0-Ŀ����ӿ�����, 1-Ŀ����ӿ��������, 2-Ŀ����ӿ򶥶�����, 3-Ŀ����ӿ��Ҷ�����, 4-Ŀ����ӿ�׶�����
	BOOL                bSizeFileter;                                           // �����ض��ĳߴ�������Ƿ���Ч
	CFG_SIZEFILTER_INFO stuSizeFileter;                                         // �����ض��ĳߴ������
	int					nTrackDuration;											// ���ٳ���ʱ��,0��:һֱ����,1~300��:���ٳ���ʱ��
} CFG_TAKENAWAYDETECTION_INFO;

// �¼�����EVENT_IVS_PARKINGDETECTION (�Ƿ�ͣ��)��Ӧ�Ĺ�������
typedef struct tagCFG_PARKINGDETECTION_INFO 
{
	// ��Ϣ
	char				szRuleName[MAX_NAME_LEN];								// ��������,��ͬ����������
	bool				bRuleEnable;											// ����ʹ��
	BYTE                bTrackEnable;                                           // ��������ʹ��,���԰��ߣ����ֹ�����Ч
	BYTE                bReserved[2];                                           // �����ֶ�
	int					nObjectTypeNum;											// ��Ӧ�������͸���
	char				szObjectTypes[MAX_OBJECT_LIST_SIZE][MAX_NAME_LEN];		// ��Ӧ���������б�
	int					nMinDuration;											// ��̳���ʱ��	��λ���룬0~65535
	int					nDetectRegionPoint;										// ������򶥵���
	CFG_POLYGON			stuDetectRegion[MAX_POLYGON_NUM];						// �������
	CFG_ALARM_MSG_HANDLE stuEventHandler;										// ��������
	CFG_TIME_SECTION	stuTimeSection[WEEK_DAY_NUM][MAX_REC_TSECT_EX];			// �¼���Ӧʱ���
	int                 nPtzPresetId;											// ��̨Ԥ�õ���	0~65535
	int                 nTriggerPosition;                                       // ��������λ����
	BYTE                bTriggerPosition[8];                                    // ��������λ��,0-Ŀ����ӿ�����, 1-Ŀ����ӿ��������, 2-Ŀ����ӿ򶥶�����, 3-Ŀ����ӿ��Ҷ�����, 4-Ŀ����ӿ�׶�����
	BOOL                bSizeFileter;                                           // �����ض��ĳߴ�������Ƿ���Ч
	CFG_SIZEFILTER_INFO stuSizeFileter;                                         // �����ض��ĳߴ������
	int					nTrackDuration;											// ���ٳ���ʱ��,0��:һֱ����,1~300��:���ٳ���ʱ��
} CFG_PARKINGDETECTION_INFO;

// �¼�����EVENT_IVS_PRESERVATION(��Ʒ��ȫ�¼�)��Ӧ�Ĺ�������
typedef struct tagCFG_PRESERVATION_INFO 
{
	// ��Ϣ
	char				szRuleName[MAX_NAME_LEN];								// ��������,��ͬ����������
	bool				bRuleEnable;											// ����ʹ��
	BYTE                bReserved[3];                                           // �����ֶ�
	int					nObjectTypeNum;											// ��Ӧ�������͸���
	char				szObjectTypes[MAX_OBJECT_LIST_SIZE][MAX_NAME_LEN];		// ��Ӧ���������б�
	int					nMinDuration;											// ��̳���ʱ��	��λ���룬0~65535
	int					nDetectRegionPoint;										// ������򶥵���
	CFG_POLYGON			stuDetectRegion[MAX_POLYGON_NUM];						// �������
	CFG_ALARM_MSG_HANDLE stuEventHandler;										// ��������
	CFG_TIME_SECTION	stuTimeSection[WEEK_DAY_NUM][MAX_REC_TSECT_EX];			// �¼���Ӧʱ���
	int                 nPtzPresetId;											// ��̨Ԥ�õ���	0~65535

} CFG_PRESERVATION_INFO;

// �¼�����EVENT_IVS_STAYDETECTION(ͣ���¼�/����)��Ӧ�Ĺ�������
typedef struct tagCFG_STAY_INFO 
{
	// ��Ϣ
	char				szRuleName[MAX_NAME_LEN];								// ��������,��ͬ����������
	bool				bRuleEnable;											// ����ʹ��
	BYTE                bTrackEnable;                                           // ��������ʹ��,���԰��ߣ����ֹ�����Ч
	BYTE                bReserved[2];                                           // �����ֶ�
	int					nObjectTypeNum;											// ��Ӧ�������͸���
	char				szObjectTypes[MAX_OBJECT_LIST_SIZE][MAX_NAME_LEN];		// ��Ӧ���������б�
	int					nMinDuration;											// ��̳���ʱ��	��λ���룬0~65535
	int					nDetectRegionPoint;										// ������򶥵���
	CFG_POLYGON			stuDetectRegion[MAX_POLYGON_NUM];						// �������
	CFG_ALARM_MSG_HANDLE stuEventHandler;										// ��������
	CFG_TIME_SECTION	stuTimeSection[WEEK_DAY_NUM][MAX_REC_TSECT_EX];			// �¼���Ӧʱ���
	int                 nPtzPresetId;											// ��̨Ԥ�õ���	0~65535
	int                 nTriggerPosition;                                       // ��������λ����
	BYTE                bTriggerPosition[8];                                    // ��������λ��,0-Ŀ����ӿ�����, 1-Ŀ����ӿ��������, 2-Ŀ����ӿ򶥶�����, 3-Ŀ����ӿ��Ҷ�����, 4-Ŀ����ӿ�׶�����
	int                 nTriggerTargetsNumber;                                  // �����������ǻ�����������
	int                 nReportInterval;                                        // ����ʱ����,��λ��
	BOOL                bSizeFileter;                                           // �����ض��ĳߴ�������Ƿ���Ч
	CFG_SIZEFILTER_INFO stuSizeFileter;                                         // �����ض��ĳߴ������
	int					nTrackDuration;											// ���ٳ���ʱ��,0��:һֱ����,1~300��:���ٳ���ʱ��
} CFG_STAY_INFO;

// �¼�����EVENT_IVS_WANDERDETECTION(�ǻ��¼�)��Ӧ�Ĺ�������
typedef struct tagCFG_WANDER_INFO 
{
	// ��Ϣ
	char				szRuleName[MAX_NAME_LEN];								// ��������,��ͬ����������
	bool				bRuleEnable;											// ����ʹ��
	BYTE                bTrackEnable;                                           // ��������ʹ��,���԰��ߣ����ֹ�����Ч
	BYTE                bReserved[2];                                           // �����ֶ�
	int					nObjectTypeNum;											// ��Ӧ�������͸���
	char				szObjectTypes[MAX_OBJECT_LIST_SIZE][MAX_NAME_LEN];		// ��Ӧ���������б�
	int					nMinDuration;											// ��̳���ʱ��	��λ���룬0~65535
	int					nDetectRegionPoint;										// ������򶥵���
	CFG_POLYGON			stuDetectRegion[MAX_POLYGON_NUM];						// �������
	CFG_ALARM_MSG_HANDLE stuEventHandler;										// ��������
	CFG_TIME_SECTION	stuTimeSection[WEEK_DAY_NUM][MAX_REC_TSECT_EX];			// �¼���Ӧʱ���
	int                 nPtzPresetId;											// ��̨Ԥ�õ���	0~65535
	int                 nTriggerPosition;                                       // ��������λ����
	BYTE                bTriggerPosition[8];                                    // ��������λ��,0-Ŀ����ӿ�����, 1-Ŀ����ӿ��������, 2-Ŀ����ӿ򶥶�����, 3-Ŀ����ӿ��Ҷ�����, 4-Ŀ����ӿ�׶�����
	int                 nTriggerTargetsNumber;                                  // �����������ǻ�����������
	int                 nReportInterval;                                        // ����ʱ����,��λ��
	BOOL                bSizeFileter;                                           // �����ض��ĳߴ�������Ƿ���Ч
	CFG_SIZEFILTER_INFO stuSizeFileter;                                         // �����ض��ĳߴ������
	int					nTrackDuration;											// ���ٳ���ʱ��,0��:һֱ����,1~300��:���ٳ���ʱ��
} CFG_WANDER_INFO;

// �¼�����EVENT_IVS_MOVEDETECTION(�ƶ��¼�/�˶����)��Ӧ�Ĺ�������
typedef struct tagCFG_MOVE_INFO 
{
	// ��Ϣ
	char				szRuleName[MAX_NAME_LEN];								// ��������,��ͬ����������
	bool				bRuleEnable;											// ����ʹ��
	BYTE                bSensitivity;                                           // �����ȣ�ȡֵ1-10��ֵԽС������Խ��
	BYTE                bTrackEnable;                                           // ��������ʹ��,���԰��ߣ����ֹ�����Ч
	BYTE                bReserved[1];                                           // �����ֶ�
	int					nObjectTypeNum;											// ��Ӧ�������͸���
	char				szObjectTypes[MAX_OBJECT_LIST_SIZE][MAX_NAME_LEN];		// ��Ӧ���������б�
	int					nMinDuration;											// ��̳���ʱ��	��λ���룬0~65535
	int					nDetectRegionPoint;										// ������򶥵���
	CFG_POLYGON			stuDetectRegion[MAX_POLYGON_NUM];						// �������
	CFG_ALARM_MSG_HANDLE stuEventHandler;										// ��������
	CFG_TIME_SECTION	stuTimeSection[WEEK_DAY_NUM][MAX_REC_TSECT_EX];			// �¼���Ӧʱ���
	int                 nPtzPresetId;											// ��̨Ԥ�õ���	0~65535
	int                 nTriggerSpeed;                                          // �����ٶȣ�km/h
	int                 nTriggerPosition;                                       // ��������λ����
	BYTE                bTriggerPosition[8];                                    // ��������λ��,0-Ŀ����ӿ�����, 1-Ŀ����ӿ��������, 2-Ŀ����ӿ򶥶�����, 3-Ŀ����ӿ��Ҷ�����, 4-Ŀ����ӿ�׶�����
	BOOL                bSizeFileter;                                           // �����ض��ĳߴ�������Ƿ���Ч
	CFG_SIZEFILTER_INFO stuSizeFileter;                                         // �����ض��ĳߴ������	
	int                 nTypeNumber;                                            // ���������
	BYTE                bDetectType[4];                                         // ��⶯���б�,0-�����˶� 1-ͻȻ���� 2-ͻȻ����
	int					nTrackDuration;											// ���ٳ���ʱ��,0��:һֱ����,1~300��:���ٳ���ʱ��
} CFG_MOVE_INFO;

// �¼�����EVENT_IVS_TAILDETECTION(β���¼�)��Ӧ�Ĺ�������
typedef struct tagCFG_TAIL_INFO 
{
	// ��Ϣ
	char				szRuleName[MAX_NAME_LEN];								// ��������,��ͬ����������
	bool				bRuleEnable;											// ����ʹ��
	BYTE                bReserved[3];                                           // �����ֶ�
	int					nObjectTypeNum;											// ��Ӧ�������͸���
	char				szObjectTypes[MAX_OBJECT_LIST_SIZE][MAX_NAME_LEN];		// ��Ӧ���������б�
	int					nMinDuration;											// ��̳���ʱ��	��λ���룬0~65535
	int					nDetectRegionPoint;										// ������򶥵���
	CFG_POLYGON			stuDetectRegion[MAX_POLYGON_NUM];						// �������
	CFG_ALARM_MSG_HANDLE stuEventHandler;										// ��������
	CFG_TIME_SECTION	stuTimeSection[WEEK_DAY_NUM][MAX_REC_TSECT_EX];			// �¼���Ӧʱ���
	int                 nPtzPresetId;											// ��̨Ԥ�õ���	0~65535

	int					nDistance;												// ��࣬β��������ʹ�ã���Χ[0, 500]
	int					nSensitivity;											// ������,����Χ[1, 10]
	BYTE                byReserved[512]; 										// �����ֽ�
} CFG_TAIL_INFO;

// �¼�����EVENT_IVS_RIOTERDETECTION(�����¼�)��Ӧ�Ĺ�������
typedef struct tagCFG_RIOTER_INFO 
{
	// ��Ϣ
	char				szRuleName[MAX_NAME_LEN];								// ��������,��ͬ����������
	bool				bRuleEnable;											// ����ʹ��
	BYTE                bAreaPercent;                                           // �ۼ���ռ��������ٷֱ�
	BYTE                bSensitivity;                                           // �����ȣ�ȡֵ1-10��ֵԽС������Խ�ͣ���Ӧ��Ⱥ���ܼ��̶�Խ��(ȡ��bAreaPercent)
	BYTE                bReserved;												// �����ֶ�
	int					nObjectTypeNum;											// ��Ӧ�������͸���
	char				szObjectTypes[MAX_OBJECT_LIST_SIZE][MAX_NAME_LEN];		// ��Ӧ���������б�
	int					nMinDuration;											// ��̳���ʱ��	��λ���룬0~65535
	int					nDetectRegionPoint;										// ������򶥵���
	CFG_POLYGON			stuDetectRegion[MAX_POLYGON_NUM];						// �������
	CFG_ALARM_MSG_HANDLE stuEventHandler;										// ��������
	CFG_TIME_SECTION	stuTimeSection[WEEK_DAY_NUM][MAX_REC_TSECT_EX];			// �¼���Ӧʱ���
	int                 nPtzPresetId;											// ��̨Ԥ�õ���	0~65535

	int                 nReportInterval;                                        // ����ʱ����,��λ�� [1,600] Ĭ��0��ʾ���ظ�����
	CFG_POLYGON         stuMinDetectRect[2];                                    // ��С�ۼ�������ο򣬷ֱ��Ӧ���ο�����Ϻ����µ㣬���ο��ʵ�������ʾ������������С��Ⱥ�ۼ����
	int					nTrackDuration;											// ���ٳ���ʱ��,0��:һֱ����,1~300��:���ٳ���ʱ��
	BYTE				bTrackEnable;											// ��������ʹ��
	int					nRioterThreshold;										// �ۼ�������ֵ, �ۼ�����������ֵ����ʼ����
} CFG_RIOTER_INFO;

// �¼�����EVENT_IVS_DENSITYDETECTION(��Ⱥ�ܶȼ���¼�)��Ӧ�Ĺ�������
typedef struct tagCFG_DENSITYDETECTION_INFO 
{
	// ��Ϣ
	char				szRuleName[MAX_NAME_LEN];								// ��������,��ͬ����������
	bool				bRuleEnable;											// ����ʹ��
	BYTE                byUnit;													// �ܶȼ����ֵ��λGB30147��Ҫ	0 �ܶȵȼ�������Ϊ�ĸ�����ϡ�裬ϡ�裬�ܼ������ܼ���1 �ܶȰٷֱ�
	BYTE                bSensitivity;                                           // ������(ȡֵ1-10��ֵԽС������Խ�ͣ���Ӧ��Ⱥ���ܼ��̶�Խ��)
	BYTE                byMinAlarmDensity;                                      // ��С�����ܶ�	ȡֵ1-100,��������ֵ�ٷֱ�
	int					nObjectTypeNum;											// ��Ӧ�������͸���
	char				szObjectTypes[MAX_OBJECT_LIST_SIZE][MAX_NAME_LEN];		// ��Ӧ���������б�
 	int					nMinDuration;											// ��̳���ʱ��	��λ���룬0~65535
	int					nDetectRegionPoint;										// ������򶥵���
	CFG_POLYGON			stuDetectRegion[MAX_POLYGON_NUM];						// �������
	CFG_ALARM_MSG_HANDLE stuEventHandler;										// ��������
	CFG_TIME_SECTION	stuTimeSection[WEEK_DAY_NUM][MAX_REC_TSECT_EX];			// �¼���Ӧʱ���
	int                 nPtzPresetId;											// ��̨Ԥ�õ���	0~65535
} CFG_DENSITYDETECTION_INFO;


// �¼�����EVENT_IVS_FIGHTDETECTION(��Ź�¼�)��Ӧ�Ĺ�������
typedef struct tagCFG_FIGHT_INFO 
{
	// ��Ϣ
	char				szRuleName[MAX_NAME_LEN];								// ��������,��ͬ����������
	bool				bRuleEnable;											// ����ʹ��
	BYTE                bReserved[3];                                           // �����ֶ�
	int					nObjectTypeNum;											// ��Ӧ�������͸���
	char				szObjectTypes[MAX_OBJECT_LIST_SIZE][MAX_NAME_LEN];		// ��Ӧ���������б�
	int					nMinDuration;											// ��̳���ʱ��	��λ���룬0~65535
	int					nDetectRegionPoint;										// ������򶥵���
	CFG_POLYGON			stuDetectRegion[MAX_POLYGON_NUM];						// �������
	CFG_ALARM_MSG_HANDLE stuEventHandler;										// ��������
	CFG_TIME_SECTION	stuTimeSection[WEEK_DAY_NUM][MAX_REC_TSECT_EX];			// �¼���Ӧʱ���
	int                 nPtzPresetId;											// ��̨Ԥ�õ���	0~65535
	int					nSensitivity;											// ������, 1-10
    CFG_EM_DETECTMODE_T emDetectMode;                                           // ���ģʽ

} CFG_FIGHT_INFO;

// �¼�����EVENT_IVS_FIREDETECTION(���¼�/������)��Ӧ�Ĺ�������
typedef struct tagCFG_FIRE_INFO 
{
	// ��Ϣ
	char				szRuleName[MAX_NAME_LEN];								// ��������,��ͬ����������
	bool				bRuleEnable;											// ����ʹ��
	BYTE                bReserved[3];                                           // �����ֶ�
	int					nObjectTypeNum;											// ��Ӧ�������͸���
	char				szObjectTypes[MAX_OBJECT_LIST_SIZE][MAX_NAME_LEN];		// ��Ӧ���������б�
	int					nMinDuration;											// ��̳���ʱ��	��λ���룬0~65535
	int					nDetectRegionPoint;										// ������򶥵���
	CFG_POLYGON			stuDetectRegion[MAX_POLYGON_NUM];						// �������
	CFG_ALARM_MSG_HANDLE stuEventHandler;										// ��������
	CFG_TIME_SECTION	stuTimeSection[WEEK_DAY_NUM][MAX_REC_TSECT_EX];			// �¼���Ӧʱ���
	int                 nPtzPresetId;											// ��̨Ԥ�õ���	0~65535
	int					nSensitivity;											// ������ 1~10
} CFG_FIRE_INFO;

// �¼�����EVENT_IVS_ELECTROSPARKDETECTION(����¼�)��Ӧ�Ĺ�������
typedef struct tagCFG_ELECTROSPARK_INFO 
{
	// ��Ϣ
	char				szRuleName[MAX_NAME_LEN];								// ��������,��ͬ����������
	bool				bRuleEnable;											// ����ʹ��
	BYTE                bReserved[3];                                           // �����ֶ�
	int					nObjectTypeNum;											// ��Ӧ�������͸���
	char				szObjectTypes[MAX_OBJECT_LIST_SIZE][MAX_NAME_LEN];		// ��Ӧ���������б�
	int					nMinDuration;											// ��̳���ʱ��	��λ���룬0~65535
	int					nDetectRegionPoint;										// ������򶥵���
	CFG_POLYGON			stuDetectRegion[MAX_POLYGON_NUM];						// �������
	CFG_ALARM_MSG_HANDLE stuEventHandler;										// ��������
	CFG_TIME_SECTION	stuTimeSection[WEEK_DAY_NUM][MAX_REC_TSECT_EX];			// �¼���Ӧʱ���
	int                 nPtzPresetId;											// ��̨Ԥ�õ���	0~65535
	
} CFG_ELECTROSPARK_INFO;

// �¼����� EVENT_IVS_SMOKEDETECTION (�������¼�)��Ӧ�Ĺ�������
typedef struct tagCFG_SMOKE_INFO 
{
	// ��Ϣ
	char				szRuleName[MAX_NAME_LEN];								// ��������,��ͬ����������
	bool				bRuleEnable;											// ����ʹ��
	BYTE                bReserved[3];                                           // �����ֶ�
	int					nObjectTypeNum;											// ��Ӧ�������͸���
	char				szObjectTypes[MAX_OBJECT_LIST_SIZE][MAX_NAME_LEN];		// ��Ӧ���������б�
	int					nMinDuration;											// ��̳���ʱ��	��λ���룬0~65535
	int					nDetectRegionPoint;										// ������򶥵���
	CFG_POLYGON			stuDetectRegion[MAX_POLYGON_NUM];						// �������
	CFG_ALARM_MSG_HANDLE stuEventHandler;										// ��������
	CFG_TIME_SECTION	stuTimeSection[WEEK_DAY_NUM][MAX_REC_TSECT_EX];			// �¼���Ӧʱ���
	int                 nPtzPresetId;											// ��̨Ԥ�õ���	0~65535
	int					nSensitivity;											// ������ 1~10
} CFG_SMOKE_INFO;

// �¼�����EVENT_IVS_FLOWSTAT(����ͳ���¼�)��Ӧ�Ĺ�������
typedef struct tagCFG_FLOWSTAT_INFO
{
	// ��Ϣ
	char				szRuleName[MAX_NAME_LEN];								// ��������,��ͬ����������
	bool				bRuleEnable;											// ����ʹ��
	BYTE                bReserved[3];                                           // �����ֶ�
	int					nObjectTypeNum;											// ��Ӧ�������͸���
	char				szObjectTypes[MAX_OBJECT_LIST_SIZE][MAX_NAME_LEN];		// ��Ӧ���������б�
	int					nUpperLimit;											// ��������	��λ����/�룬1~65535
	int					nPeriod;												// �ϱ�����	��λ���룬1~65535
	int					nDetectLinePoint;										// ����߶�����
	CFG_POLYLINE		stuDetectLine[MAX_POLYLINE_NUM];						// �����
	CFG_ALARM_MSG_HANDLE stuEventHandler;										// ��������
	CFG_TIME_SECTION	stuTimeSection[WEEK_DAY_NUM][MAX_REC_TSECT_EX];			// �¼���Ӧʱ���
	int                 nPtzPresetId;											// ��̨Ԥ�õ���	0~65535
		
} CFG_FLOWSTAT_INFO;

enum NET_NUMBERSTAT_TYPE
{
	NET_EM_NUMSTAT_TYPE_REGION,                  // "Region" ��������
		NET_EM_NUMSTAT_TYPE_ENTRANCE,            // "Entrance" ���������
		NET_EM_NUMSTAT_TYPE_OTHER,               // other
};
// �¼�����EVENT_IVS_NUMBERSTAT(����ͳ���¼�)��Ӧ�Ĺ�������
typedef struct tagCFG_NUMBERSTAT_INFO
{
	// ��Ϣ
	char				szRuleName[MAX_NAME_LEN];								// ��������,��ͬ����������
	bool				bRuleEnable;											// ����ʹ��
	BYTE                bReserved[3];                                           // �����ֶ�
	int					nObjectTypeNum;											// ��Ӧ�������͸���
	char				szObjectTypes[MAX_OBJECT_LIST_SIZE][MAX_NAME_LEN];		// ��Ӧ���������б�
	int					nUpperLimit;											// �������� ��λ������1~65535
	int					nPeriod;												// �ϱ�����	��λ���룬1~65535
	int					nDetectRegionPoint;										// �����������
	CFG_POLYGON			stuDetectRegion[MAX_POLYGON_NUM];						// �����
	CFG_ALARM_MSG_HANDLE stuEventHandler;										// ��������
	CFG_TIME_SECTION	stuTimeSection[WEEK_DAY_NUM][MAX_REC_TSECT_EX];			// �¼���Ӧʱ���
	int                 nPtzPresetId;											// ��̨Ԥ�õ���	0~65535
	NET_NUMBERSTAT_TYPE emType;                                                 // ����ͳ������	֧�֣�"Region" �������� "Entrance" ���������
	CFG_POLYGON         stDirection[2];                                         // �����˶�����, ���մ˷�����ʻ�����򣬷�����������	ֻ�Գ����������Ч����һ��������㣬�ڶ��������յ㣬��������һ����[0,8191]���䡣
    int                 nRecordInterval;                                        // ��¼���ڣ���λ�����ӣ�1~10
	int				nEnterThreshold;										// ������������ֵ,0��ʾ������
	int				nExitThreshold;											// �뿪��������ֵ,0��ʾ������
	int				nInsideThreshold;										// ������������ֵ,0��ʾ������
    UINT                nPlanID;                                                // �ƻ�ID,�������Ч,��1��ʼ
	UINT				nAreaID;												// ����ID,һ��Ԥ�õ���Զ�Ӧ�������
	int					nMaxDetectLineNum;							            // �����ʵ��������
	CFG_POLYLINE		stuDetectLine[MAX_POLYLINE_NUM];			            // �뿪���������
    UINT                nStayMinDuration;                                        // ͣ����ʱʱ��
} CFG_NUMBERSTAT_INFO;

//�¼�����EVENT_IVS_RETROGRADEDETECTION(��Ա�����¼�)��Ӧ�Ĺ�������
typedef struct tagCFG_RETROGRADEDETECTION_INFO
{
	// ��Ϣ
	char				szRuleName[MAX_NAME_LEN];								// ��������,��ͬ����������
	bool				bRuleEnable;											// ����ʹ��
	BYTE                bSensitivity;                                           // ������, ȡֵ1-10��ֵԽС������Խ��
	BYTE                bReserved[2];                                           // �����ֶ� 
	int					nObjectTypeNum;											// ��Ӧ�������͸���
	char				szObjectTypes[MAX_OBJECT_LIST_SIZE][MAX_NAME_LEN];		// ��Ӧ���������б�
	int                 nPtzPresetId;											// ��̨Ԥ�õ���	0~65535
	int					nDetectRegionPoint;										// ������򶥵���
	CFG_POLYGON			stuDetectRegion[MAX_POLYGON_NUM];						// �������
	int                 nDirectionPoint;                                        // ������ʻ���򶥵���
	CFG_POLYGON			stuDirection[MAX_POLYLINE_NUM];					      	// ������ʻ����, ���մ˷�����ʻ�����򣬷�����������(��һ��������㣬�ڶ��������յ㣬��������һ����[0,8191]����)
	BOOL                bSizeFileter;                                           // �����ض��ĳߴ�������Ƿ���Ч
	CFG_SIZEFILTER_INFO stuSizeFileter;                                         // �����ض��ĳߴ������
	int                 nTriggerPosition;                                       // ��������λ����
	BYTE                bTriggerPosition[8];                                    // ��������λ��,0-Ŀ����ӿ�����, 1-Ŀ����ӿ��������, 2-Ŀ����ӿ򶥶�����, 3-Ŀ����ӿ��Ҷ�����, 4-Ŀ����ӿ�׶�����
	CFG_ALARM_MSG_HANDLE stuEventHandler;										// ��������
	CFG_TIME_SECTION	stuTimeSection[WEEK_DAY_NUM][MAX_REC_TSECT_EX];			// �¼���Ӧʱ���
	
} CFG_RETROGRADEDETECTION_INFO;

//�¼�����EVENT_IVS_ABNORMALRUNDETECTION(�쳣�����¼�)��Ӧ�Ĺ�������
typedef struct tagCFG_ABNORMALRUNDETECTION_INFO
{
	// ��Ϣ
	char				szRuleName[MAX_NAME_LEN];								// ��������,��ͬ����������
	bool				bRuleEnable;											// ����ʹ��
	BYTE                bSensitivity;                                           // ������, ȡֵ1-10��ֵԽС������Խ��
	BYTE                bTrackEnable;                                           // ��������ʹ��,���԰��ߣ����ֹ�����Ч
	BYTE                bReserved[1];                                           // �����ֶ� 
	int					nObjectTypeNum;											// ��Ӧ�������͸���
	char				szObjectTypes[MAX_OBJECT_LIST_SIZE][MAX_NAME_LEN];		// ��Ӧ���������б�
	int                 nPtzPresetId;											// ��̨Ԥ�õ���	0~65535
	int					nDetectRegionPoint;										// ������򶥵���
	CFG_POLYGON			stuDetectRegion[MAX_POLYGON_NUM];						// �������
	BOOL                bSizeFileter;                                           // �����ض��ĳߴ�������Ƿ���Ч
	CFG_SIZEFILTER_INFO stuSizeFileter;                                         // �����ض��ĳߴ������
	int                 nTriggerPosition;                                       // ��������λ����
	BYTE                bTriggerPosition[8];                                    // ��������λ��,0-Ŀ����ӿ�����, 1-Ŀ����ӿ��������, 2-Ŀ����ӿ򶥶�����, 3-Ŀ����ӿ��Ҷ�����, 4-Ŀ����ӿ�׶�����
	int                 nDetectType;                                            // ���������
	BYTE                bDetectType[4];                                         // �������,0-���ٱ���, 1-ͻȻ����, 2-ͻȻ����
	int                 nTriggerSpeed;                                          // �����ٶȣ����ڸ��ٶȱ���	��λ��km/h
	int                 nMinDuration;                                           // ��̳���ʱ��	��λ���룬0~65535
	CFG_ALARM_MSG_HANDLE stuEventHandler;										// ��������
	CFG_TIME_SECTION	stuTimeSection[WEEK_DAY_NUM][MAX_REC_TSECT_EX];			// �¼���Ӧʱ���
	
} CFG_ABNORMALRUNDETECTION_INFO;

//�¼�����EVENT_IVS_VIDEOABNORMALDETECTION(��Ƶ�쳣)��Ӧ�Ĺ�������
typedef struct tagCFG_VIDEOABNORMALDETECTION_INFO
{
	// ��Ϣ
	char				szRuleName[MAX_NAME_LEN];								// ��������,��ͬ����������
	bool				bRuleEnable;											// ����ʹ��
	BYTE                bSensitivity;                                           // ������, ȡֵ1-10��ֵԽС������Խ��(ֻ�Լ��������Ƶ�ڵ��������������������仯��Ч)
	BYTE                bReserved[2];                                           // �����ֶ� 
	int					nObjectTypeNum;											// ��Ӧ�������͸���
	char				szObjectTypes[MAX_OBJECT_LIST_SIZE][MAX_NAME_LEN];		// ��Ӧ���������б�
	int                 nPtzPresetId;											// ��̨Ԥ�õ���	0~65535
	int                 nDetectType;                                            // ���������
	BYTE                bDetectType[MAX_ABNORMAL_DETECT_TYPE];                  // �������,0-��Ƶ��ʧ, 1-��Ƶ�ڵ�, 2-���涳��, 3-����, 4-����, 5-�����仯
                                                                                // 6-���Ƽ�� , 7-������� , 8-ƫɫ��� , 9-��Ƶģ����� , 10-�Աȶ��쳣���
                                                                                // 11-��Ƶ�˶� , 12-��Ƶ��˸ , 13-��Ƶ��ɫ , 14-�齹��� , 15-���ؼ��
	int					nMinDuration;											// ��̳���ʱ��	��λ���룬0~65535
	CFG_ALARM_MSG_HANDLE stuEventHandler;										// ��������
	CFG_TIME_SECTION	stuTimeSection[WEEK_DAY_NUM][MAX_REC_TSECT_EX];			// �¼���Ӧʱ���
	int					nDetectRegionPoint;										// �����������
	CFG_POLYGON			stuDetectRegion[MAX_POLYGON_NUM];						// �����
	int					nThresholdNum;											// �쳣�����ֵ����
	int					nThreshold[MAX_ABNORMAL_THRESHOLD_LEN];					// �쳣�����ֵ,��Χ1~100
	
} CFG_VIDEOABNORMALDETECTION_INFO;

// �¼�����EVENT_IVS_FACERECOGNITION(����ʶ��)��Ӧ�Ĺ�������
typedef struct tagCFG_FACERECOGNITION_INFO
{
	// ��Ϣ
	char				 szRuleName[MAX_NAME_LEN];								// ��������,��ͬ����������
	bool				 bRuleEnable;											// ����ʹ��
	BYTE                 bReserved[2];                                           // �����ֶ� 
	int					 nObjectTypeNum;											// ��Ӧ�������͸���
	char				 szObjectTypes[MAX_OBJECT_LIST_SIZE][MAX_NAME_LEN];		// ��Ӧ���������б�
	int                  nPtzPresetId;											// ��̨Ԥ�õ���	0~65535
	BYTE                 bySimilarity;                                          // ���ƶȣ�������ڸ���ʶ�Ȳű���(1~100)
	BYTE                 byAccuracy;                                            // ʶ�𾫶�(ȡֵ1~10������ֵ���󣬼�⾫����ߣ�����ٶ��½�����СֵΪ1 ��ʾ����ٶ����ȣ����ֵΪ10��ʾ��⾫������)
	BYTE                 byMode;                                                // �Ա�ģʽ, 0-����, 1-ָ�������������, 2-����ģʽ(�㷨��������������������Զ�ѡȡ���)
	BYTE                 byImportantRank;                                       // ��ѯ��Ҫ�ȼ����ڵ��ڴ˵ȼ�����Ա(1~10,��ֵԽ��Խ��Ҫ)
	int                  nAreaNum;                                              // ������  
	BYTE                 byAreas[8];                                            // �����������, 0-üë��1-�۾���2-���ӣ�3-��ͣ�4-����(�˲����ڶԱ�ģʽΪ1ʱ��Ч)
	int                  nMaxCandidate;                                         // ��������ƥ��ͼƬ����
	CFG_ALARM_MSG_HANDLE stuEventHandler;										// ��������
	CFG_TIME_SECTION	 stuTimeSection[WEEK_DAY_NUM][MAX_REC_TSECT_EX];			// �¼���Ӧʱ���
	
} CFG_FACERECOGNITION_INFO;


// �¼�����EVENT_IVS_TRAFFICCONTROL(��ͨ�����¼�)��Ӧ�Ĺ�������
typedef struct tagCFG_TRAFFICCONTROL_INFO 
{
	// ��Ϣ
	char				szRuleName[MAX_NAME_LEN];									// ��������,��ͬ����������
	bool				bRuleEnable;												// ����ʹ��
	BYTE                bReserved[3];                                               // �����ֶ�
	int					nObjectTypeNum;												// ��Ӧ�������͸���
	char				szObjectTypes[MAX_OBJECT_LIST_SIZE][MAX_NAME_LEN];			// ��Ӧ���������б�
	int					nDetectLinePoint;							            	// ����߶�����
	CFG_POLYLINE		stuDetectLine[MAX_POLYLINE_NUM];			            	// �����
	CFG_TIME_SECTION	stuTimeSchedule[WEEK_DAY_NUM][MAX_REC_TSECT];           	// ����ʱ���
	int					nVehicleSizeNum;							            	// ������С���͸���
	char				szVehicleSizeList[MAX_VEHICLE_SIZE_LIST][MAX_NAME_LEN];		// ������С�����б�"Light-duty":С�ͳ�;	"Medium":���ͳ�; "Oversize":���ͳ�
	int					nPlateTypeNum;												// �������͸���
	char				szPlateTypesList[MAX_PLATE_TYPE_LIST][MAX_NAME_LEN];		// ���������б�"Unknown" δ֪; "Normal" ���ƺ���; "Yellow" ����; "DoubleYellow" ˫���β��
																					// "Police" ����; "Armed" �侯��; "Military" ���Ӻ���; "DoubleMilitary" ����˫��
																					// "SAR" �۰���������; "Trainning" ����������; "Personal" ���Ժ���; "Agri" ũ����
																					// "Embassy" ʹ�ݺ���; "Moto" Ħ�г�����; "Tractor" ����������; "Other" ��������
																					// "Civilaviation"�񺽺���,"Black"����,"PureNewEnergyMicroCar"���綯����ԴС��
																					// "MixedNewEnergyMicroCar,"�������ԴС��,"PureNewEnergyLargeCar",���綯����Դ��
																					// "MixedNewEnergyLargeCar"�������Դ��
	int					nPlateNumber;												// ���Ƶ�˫�� 0:����; 1:˫��; 2:��˫��;	
	CFG_ALARM_MSG_HANDLE stuEventHandler;											// ��������
	CFG_TIME_SECTION	stuTimeSection[WEEK_DAY_NUM][MAX_REC_TSECT_EX];				// �¼���Ӧʱ���
	int                 nPtzPresetId;												// ��̨Ԥ�õ���	0~65535
} CFG_TRAFFICCONTROL_INFO;

// �¼�����EVENT_IVS_TRAFFICACCIDENT(��ͨ�¹��¼�)��Ӧ�Ĺ�������
typedef struct tagCFG_TRAACCIDENT_INFO
{
	// ��Ϣ
	char				szRuleName[MAX_NAME_LEN];								// ��������,��ͬ����������
	bool				bRuleEnable;											// ����ʹ��
	BYTE                bReserved[3];                                           // �����ֶ�
	int					nObjectTypeNum;											// ��Ӧ�������͸���
	char				szObjectTypes[MAX_OBJECT_LIST_SIZE][MAX_NAME_LEN];		// ��Ӧ���������б�
	int					nDetectRegionPoint;										// �����������
	CFG_POLYGON			stuDetectRegion[MAX_POLYGON_NUM];						// �����
	CFG_ALARM_MSG_HANDLE stuEventHandler;										// ��������
	CFG_TIME_SECTION	stuTimeSection[WEEK_DAY_NUM][MAX_REC_TSECT_EX];			// �¼���Ӧʱ���
	int                 nPtzPresetId;											// ��̨Ԥ�õ���	0~65535
	
} CFG_TRAACCIDENT_INFO;

// �¼�����EVENT_IVS_TRAFFICJUNCTION(��ͨ·���Ϲ����¼�)��Ӧ�Ĺ�������
typedef struct tagCFG_TRAJUNCTION_INFO
{
	// ��Ϣ
	char				szRuleName[MAX_NAME_LEN];				// ��������,��ͬ����������
	bool				bRuleEnable;							// ����ʹ��
	int					nObjectTypeNum;							// ��Ӧ�������͸���
	char				szObjectTypes[MAX_OBJECT_LIST_SIZE][MAX_NAME_LEN];		// ��Ӧ���������б�
	int					nLane;									// �������
	int                 nDirection;                             // ��������(�������ķ���),0-�� 1-���� 2-�� 3-���� 4-�� 5-���� 6-�� 7-����
	int					nPreLinePoint;							// ǰ�ü���߶�����
	CFG_POLYLINE		stuPreLine[MAX_POLYLINE_NUM];			// ǰ�ü����
	int					nMiddleLinePoint;						// �м����߶�����
	CFG_POLYLINE		stuMiddleLine[MAX_POLYLINE_NUM];		// �м�����
	int					nPostLinePoint;							// ���ü���߶�����
	CFG_POLYLINE		stuPostLine[MAX_POLYLINE_NUM];			// ���ü����
	int                 nFlowLimit;                             // �������ޣ���λ��/��
	int                 nSpeedDownLimit;                        // �ٶ����ޣ���Ϊ0�����ʾ�������ޣ�km/h
	int                 nSpeedUpLimit;                          // �ٶ����ޣ���Ϊ0�����ʾ�������ޣ�km/h
	int                 nTriggerModeNum;                        // ����ģʽ����
	char                szTriggerMode[MAX_TRIGGERMODE_NUM][MAX_NAME_LEN];  // ����ģʽ��"Passing" : ����·��(���м�����Ϊ׼��ֻ�ܵ���ʹ��),"RunRedLight" : �����
																// "Overline":ѹ�׳�����,"OverYellowLine": ѹ����, "Retrograde":����
																// "TurnLeft":Υ����ת, "TurnRight":Υ����ת, "CrossLane":Υ�±��
																// "U-Turn" Υ�µ�ͷ, "Parking":Υ��ͣ��, "WaitingArea" Υ�½��������
																// "OverSpeed": ����,"UnderSpeed":Ƿ��,"Overflow" : ��������
																// "Human":����,"NoMotor":�ǻ�����

	CFG_ALARM_MSG_HANDLE stuEventHandler;							// ��������
	CFG_TIME_SECTION	stuTimeSection[WEEK_DAY_NUM][MAX_REC_TSECT_EX];// �¼���Ӧʱ���
	int                 nPtzPresetId;											// ��̨Ԥ�õ���	0~65535
	BOOL                bMaskRetrograde;         	             // �Ƿ��������У��������е�����������
			
} CFG_TRAJUNCTION_INFO;

// �¼�����EVENT_IVS_TRAFFICGATE(��ͨ�����Ϲ����¼�)��Ӧ�Ĺ�������
typedef struct tagCFG_TRAFFICGATE_INFO
{
	// ��Ϣ
	char				szRuleName[MAX_NAME_LEN];								// ��������,��ͬ����������
	bool				bRuleEnable;											// ����ʹ��
	BYTE                bReserved[3];                                           // �����ֶ�
	int					nObjectTypeNum;											// ��Ӧ�������͸���
	char				szObjectTypes[MAX_OBJECT_LIST_SIZE][MAX_NAME_LEN];		// ��Ӧ���������б�
	int					nLane;													// �������
	int					nDetectLinePoint1;										// �����(������Ȧ)1������
	CFG_POLYLINE		stuDetectLine1[MAX_POLYLINE_NUM];						// �����1
	int					nDetectLinePoint2;										// �����(������Ȧ)2������
	CFG_POLYLINE		stuDetectLine2[MAX_POLYLINE_NUM];						// �����2
	int					nLeftLinePoint;											// �󳵵��߶�����
	CFG_POLYLINE		stuLeftLine[MAX_POLYLINE_NUM];							// �󳵵���
	int					nRightLinePoint;										// �ҳ����߶�����
	CFG_POLYLINE		stuRightLine[MAX_POLYLINE_NUM];							// �ҳ�����
	int					nSpeedWeight;											// �ٶ�Ȩ��ϵ��(���ճ���=��������*Ȩ��ϵ��)
	double				MetricDistance;											// ���������ʵ�ʾ���,��λ����
	int					nSpeedUpperLimit;										// �ٶ����� 0��ʾ�������� ��λ��km/h
	int					nSpeedLowerLimit;										// �ٶ����� 0��ʾ�������� ��λ��km/h
	CFG_ALARM_MSG_HANDLE stuEventHandler;										// ��������
	CFG_TIME_SECTION	stuTimeSection[WEEK_DAY_NUM][MAX_REC_TSECT_EX];			// �¼���Ӧʱ���
	int                 nDirection;                                             // ��������(�������ķ���)��0-�� 1-���� 2-�� 3-���� 4-�� 5-���� 6-�� 7-����
	int                 nTriggerModeNum;                                        // ����ģʽ����
	char                szTriggerMode[MAX_TRIGGERMODE_NUM][MAX_NAME_LEN];       // ����ģʽ��"OverLine":ѹ��,"Retrograde":����,"OverSpeed":����,"UnderSpeed":Ƿ��
	                                                                            // "Passing":����·�ڣ�������ץ��, "WrongRunningRoute":�г�ռ��(����ʹ��),"YellowVehicleInRoute": ����ռ��
	                                                                            // "OverYellowLine":ѹ����
	int                 nPtzPresetId;											// ��̨Ԥ�õ���	0~65535
	BOOL                bMaskRetrograde;         	             // �Ƿ��������У��������е�����������
} CFG_TRAFFICGATE_INFO;

// �¼�����EVENT_IVS_TRAFFIC_TOLLGATE(��ͨ�����¹����¼�)��Ӧ�Ĺ�������
typedef struct tagCFG_TRAFFICTOLLGATE_INFO
{
	char				szRuleName[MAX_NAME_LEN];								// ��������,��ͬ����������
	bool				bRuleEnable;											// ����ʹ��
	BYTE                bReserved[3];                                           // �����ֶ�
	int					nObjectTypeNum;											// ��Ӧ�������͸���
	char				szObjectTypes[MAX_OBJECT_LIST_SIZE][MAX_NAME_LEN];		// ��Ӧ���������б�
	int                 nPtzPresetId;											// ��̨Ԥ�õ���	0~65535

	int					nLane;													// �������
	int					nDetectLinePoint;										// ���ڱ����߶�����
	CFG_POLYLINE		stuDetectLine[MAX_POLYLINE_NUM];						// ���ڱ�����

	CFG_ALARM_MSG_HANDLE stuEventHandler;										// ��������
	CFG_TIME_SECTION	stuTimeSection[WEEK_DAY_NUM][MAX_REC_TSECT_EX];			// �¼���Ӧʱ���
} CFG_TRAFFICTOLLGATE_INFO;

// �¼�����EVENT_IVS_FACEDETECT(��������¼�)��Ӧ�Ĺ�������
typedef struct tagCFG_FACEDETECT_INFO
{
	char				szRuleName[MAX_NAME_LEN];								// ��������,��ͬ����������
	bool				bRuleEnable;											// ����ʹ��
	BYTE                bReserved[3];                                           // �����ֶ�
	int					nObjectTypeNum;											// ��Ӧ�������͸���
	char				szObjectTypes[MAX_OBJECT_LIST_SIZE][MAX_NAME_LEN];		// ��Ӧ���������б�
	int					nDetectRegionPoint;										// �����������
	CFG_POLYGON			stuDetectRegion[MAX_POLYGON_NUM];						// �����
	int                 nHumanFaceTypeCount;                                    // �����¼����������͸���
	char                szHumanFaceType[MAX_HUMANFACE_LIST_SIZE][MAX_NAME_LEN]; // �����¼�����������
	CFG_ALARM_MSG_HANDLE stuEventHandler;										// ��������
	CFG_TIME_SECTION	stuTimeSection[WEEK_DAY_NUM][MAX_REC_TSECT_EX];			// �¼���Ӧʱ���
	int                 nPtzPresetId;											// ��̨Ԥ�õ���	0~65535
	int                 nMinDuration;                                           // ��̴���ʱ��,��λ���� 
    int                 nSensitivity;                                           // ������,��Χ[1,10],������Խ��Խ���׼��
    int                 nReportInterval;                                        // �ظ��������,��λ:��,[0,600](����0��ʾ���ظ�����)
    BOOL                bSizeFileter;                                           // �����ض��ĳߴ�������Ƿ���Ч
	CFG_SIZEFILTER_INFO stuSizeFileter;                                         // �����ض��ĳߴ������
	BOOL				bFeatureEnable;											// �Ƿ�����������ʶ��, IPC����
																				//(ͨ��FaceDetection�����е�FeatureSupport��ȷ���������Ƿ��������)
	int					nFaceFeatureNum;										// ��Ҫ�����������Ը���
	EM_FACEFEATURE_TYPE emFaceFeatureType[MAX_FEATURE_LIST_SIZE];				// �������������, ͨ��FaceDetection��������ȡ֧����Щ��������
}CFG_FACEDETECT_INFO;

// �����Ĳ�����
typedef struct tagCFG_LINKGROUP_INFO
{
	BOOL 					bEnable;							// �������Ƿ�����
	char					szGroupID[MAX_GROUP_ID_LEN];		// ������ID
	BYTE                	bySimilarity;                       // ���ƶ���ֵ
	char					szColorName[MAX_COLOR_NAME_LEN];	// �¼�����ʱ�������������ɫ
	BOOL 					bShowTitle;							// �¼�����ʱ��������Ƿ���ʾ��������
	BOOL 					bShowPlate;							// �¼�����ʱ�Ƿ���ʾ�ȶ����
	BYTE					bReserved[511];						// �����ֶ�	
	CFG_ALARM_MSG_HANDLE 	stuEventHandler;					// ��������
}CFG_LINKGROUP_INFO;

// İ���˲���ģʽ
typedef struct tagCFG_STRANGERMODE_INFO
{
	BOOL 					bEnable;							// ģʽ�Ƿ�����
	char					szColorHex[MAX_COLOR_HEX_LEN];		// �¼�����ʱ�������������ɫ
	BOOL 					bShowTitle;							// �¼�����ʱ��������Ƿ���ʾ��������
	BOOL 					bShowPlate;							// �¼�����ʱ�Ƿ���ʾ�ȶ����
	BYTE					bReserved[512];						// �����ֶ�	
	CFG_ALARM_MSG_HANDLE 	stuEventHandler;					// ��������
}CFG_STRANGERMODE_INFO;

// �¼�����EVENT_IVS_FACEANALYSIS(���������¼�) ��Ӧ�Ĺ�������
typedef struct tagCFG_FACEANALYSIS_INFO
{
	char					szRuleName[MAX_NAME_LEN];								// ��������,��ͬ����������
	bool					bRuleEnable;											// ����ʹ��
	BYTE                	bReserved[3];                                           // �����ֶ�
	int						nObjectTypeNum;											// ��Ӧ�������͸���
	char					szObjectTypes[MAX_OBJECT_LIST_SIZE][MAX_NAME_LEN];		// ��Ӧ���������б�
	int						nDetectRegionPoint;										// �����������
	CFG_POLYGON				stuDetectRegion[MAX_POLYGON_NUM];						// �����
	CFG_ALARM_MSG_HANDLE 	stuEventHandler;										// ��������
	CFG_TIME_SECTION		stuTimeSection[WEEK_DAY_NUM][MAX_REC_TSECT_EX];			// �¼���Ӧʱ���
	int                 	nPtzPresetId;											// ��̨Ԥ�õ���	0~65535
	int                 	nSensitivity;                                           // ������,��Χ[1,10],������Խ��Խ���׼��
	int						nLinkGroupNum;											// �������ظ��� 
	CFG_LINKGROUP_INFO		stuLinkGroup[MAX_LINK_GROUP_NUM];						// �����Ĳ�����
	CFG_STRANGERMODE_INFO	stuStrangerMode;										// İ���˲���ģʽ
    BOOL                	bSizeFileter;                                           // �����ض��ĳߴ�������Ƿ���Ч
	CFG_SIZEFILTER_INFO 	stuSizeFileter;                                         // �����ض��ĳߴ������
	BOOL					bFeatureEnable;											// �Ƿ�����������ʶ��, IPC����
	int						nFaceFeatureNum;										// ��Ҫ�����������Ը���
	EM_FACEFEATURE_TYPE 	emFaceFeatureType[MAX_FEATURE_LIST_SIZE];				// �������������
	BOOL					bFeatureFilter;											// ���������Կ���ǰ���£��������ͼ������̫��Ƿ��ϱ�����
																					// true-ͼ��̫��ϱ����� false-ͼ��ܲ�Ҳ�ϱ�����(���ܻ�ǳ���׼��Ӱ���û�����)
	int						nMinQuality;											// ����ͼƬ������ֵ,��bFeatureFilterһ��ʹ�� ��Χ[1,100]
} CFG_FACEANALYSIS_INFO;

// �¼�����EVENT_IVSS_FACEATTRIBUTE(IVSS��������¼�) ��Ӧ�Ĺ�������
typedef struct tagCFG_FACEATTRIBUTE_INFO
{
	char				szRuleName[MAX_NAME_LEN];								// ��������,��ͬ����������
	bool				bRuleEnable;											// ����ʹ��
	BYTE                bReserved[3];                                           // �����ֶ�
	int					nObjectTypeNum;											// ��Ӧ�������͸���
	char				szObjectTypes[MAX_OBJECT_LIST_SIZE][MAX_NAME_LEN];		// ��Ӧ���������б�
	int					nDetectRegionPoint;										// �����������
	CFG_POLYGON			stuDetectRegion[MAX_POLYGON_NUM];						// �����
	CFG_ALARM_MSG_HANDLE stuEventHandler;										// ��������
	CFG_TIME_SECTION	stuTimeSection[WEEK_DAY_NUM][MAX_REC_TSECT_EX];			// �¼���Ӧʱ���
	int                 nPtzPresetId;											// ��̨Ԥ�õ���	0~65535
	int                 nMinDuration;                                           // ��̴���ʱ��,��λ���� 
	int                 nTriggerTargetsNumber;                                  // ������������������
	int                 nSensitivity;                                           // ������,��Χ[1,10],������Խ��Խ���׼��(Ĭ��ֵ5)
    int                 nReportInterval;                                        // �ظ��������,��λ:��,[0,600] ����0��ʾ���ظ�����(Ĭ��ֵ30)
    BOOL                bSizeFileter;                                           // �����ض��ĳߴ�������Ƿ���Ч
	CFG_SIZEFILTER_INFO stuSizeFileter;                                         // �����ض��ĳߴ������
	int					nFaceFeatureNum;										// ��Ҫ�����������Ը���
	EM_FACEFEATURE_TYPE emFaceFeatureType[MAX_FEATURE_LIST_SIZE];				// �������������
	BOOL                bFeatureFilter;                                         // ���������Կ���ǰ���£��������ͼ������̫��Ƿ��ϱ�����
	int                 nMinQuality;                                            // ����ͼƬ������ֵ,��bFeatureFilterһ��ʹ�� ��Χ[0,100]
} CFG_FACEATTRIBUTE_INFO;

// �¼�����EVENT_IVSS_FACECOMPARE(IVSS����ʶ���¼�) ��Ӧ�Ĺ�������
typedef struct tagCFG_FACECOMPARE_INFO
{
	char					szRuleName[MAX_NAME_LEN];								// ��������,��ͬ����������
	bool					bRuleEnable;											// ����ʹ��
	BYTE                	bReserved[3];                                           // �����ֶ�
	int						nObjectTypeNum;											// ��Ӧ�������͸���
	char					szObjectTypes[MAX_OBJECT_LIST_SIZE][MAX_NAME_LEN];		// ��Ӧ���������б�
	CFG_ALARM_MSG_HANDLE 	stuEventHandler;										// ��������
	CFG_TIME_SECTION		stuTimeSection[WEEK_DAY_NUM][MAX_REC_TSECT_EX];			// �¼���Ӧʱ���
	int                 	nPtzPresetId;											// ��̨Ԥ�õ���	0~65535
	int						nLinkGroupNum;											// �������ظ��� 
	CFG_LINKGROUP_INFO		stuLinkGroup[MAX_LINK_GROUP_NUM];						// �����Ĳ�����
	CFG_STRANGERMODE_INFO	stuStrangerMode;										// İ���˲���ģʽ
} CFG_FACECOMPARE_INFO;


//�¼�����EVENT_IVS_TRAFFIC_NOPASSING(��ͨΥ��-��ֹͨ���¼�)��Ӧ�����ݿ�������Ϣ
typedef struct tagCFG_NOPASSING_INFO
{
	char				szRuleName[MAX_NAME_LEN];								// ��������,��ͬ����������
	bool				bRuleEnable;											// ����ʹ��
	BYTE                bReserved[3];                                           // �����ֶ�
}CFG_NOPASSING_INFO;

// �¼�����EVENT_IVS_TRAFFICJAM (��ͨӵ���¼�)��Ӧ�Ĺ�������
typedef struct tagCFG_TRAFFICJAM_INFO
{
	char				szRuleName[MAX_NAME_LEN];								// ��������,��ͬ����������
	bool				bRuleEnable;											// ����ʹ��
    BYTE                bJamLineMargin;                                         // ռ����ֵ��ӵ�³���ռ����������ֵ��0-100
    BYTE                bSensitivity;                                           // �����ȣ�ֵԽС������Խ�͡�ȡֵ1-10
	BYTE                bReserved[1];                                           // �����ֶ�
	int					nObjectTypeNum;											// ��Ӧ�������͸���
	char				szObjectTypes[MAX_OBJECT_LIST_SIZE][MAX_NAME_LEN];		// ��Ӧ���������б�
	int                 nPtzPresetId;											// ��̨Ԥ�õ���	0~65535
	int					nLane;													// �������
	int                 nDelayTime;                                             // ��⵽������������ʼ�ϱ���ʱ��, ��λ���룬��Χ1~65535
	int                 nIntervalTime;                                          // �������ʱ��, ��λ���룬��Χ1~65535
	int                 nReportTimes;                                           // �ϱ�����, 1~255
	CFG_ALARM_MSG_HANDLE stuEventHandler;										// ��������
	CFG_TIME_SECTION	stuTimeSection[WEEK_DAY_NUM][MAX_REC_TSECT_EX];			// �¼���Ӧʱ���
	int		            nDetectRegionPoint;                                     // �����������
	CFG_POLYGON         stuDetectRegion[MAX_POLYGON_NUM];                       // �������
}CFG_TRAFFICJAM_INFO;

// �¼�����EVENT_IVS_TRAFFIC_NONMOTORINMOTORROUTE(�ǻ�����ռ��������������)��Ӧ�Ĺ�������
typedef struct tagCFG_TRAFFIC_NONMOTORINMOTORROUTE_INFO
{
	// ��Ϣ
    char                szRuleName[MAX_NAME_LEN];                               // ��������,��ͬ����������
    bool                bRuleEnable;                                            // ����ʹ��
    bool                bSnapMotorcycle;                                        // �Ƿ�ץ��Ħ�г�,ͬһ����Ƶͨ�����ֹ�����Ҫץ��Ħ�г�����������Ҫ������ʹ�ù���
    BYTE                bReserved[2];                                           // �����ֶ�
    int                 nObjectTypeNum;	                                        // ��Ӧ�������͸���
    char                szObjectTypes[MAX_OBJECT_LIST_SIZE][MAX_NAME_LEN];      // ��Ӧ���������б�
    int                 nPtzPresetId;                                           // ��̨Ԥ�õ���	0~65535
    int                 nLaneNumber;                                            // �������
    CFG_POLYGON         stDirection[2];                                         // ������ʻ����, ���մ˷�����ʻ�����򣬷�����������	��һ��������㣬�ڶ��������յ㣬��������һ����[0,8192)���䡣
    CFG_ALARM_MSG_HANDLE stuEventHandler;                                       // ��������
    CFG_TIME_SECTION    stuTimeSection[WEEK_DAY_NUM][MAX_REC_TSECT_EX];         // �¼���Ӧʱ���
} CFG_TRAFFIC_NONMOTORINMOTORROUTE_INFO;

// �¼�����EVENT_IVS_TRAFFIC_NONMOTOR_OVERLOAD(�ǻ�������������)��Ӧ�Ĺ�������
typedef struct tagCFG_TRAFFIC_NONMOTOR_OVERLOAD_INFO
{
	// ��Ϣ
	char                szRuleName[MAX_NAME_LEN];                               // ��������,��ͬ����������
	bool                bRuleEnable;                                            // ����ʹ��
	BYTE                bReserved[3];                                           // �����ֶ�
	int                 nObjectTypeNum;	                                        // ��Ӧ�������͸���
	char                szObjectTypes[MAX_OBJECT_LIST_SIZE][MAX_NAME_LEN];      // ��Ӧ���������б�
	int                 nPtzPresetId;                                           // ��̨Ԥ�õ���	0~65535
	CFG_ALARM_MSG_HANDLE stuEventHandler;                                       // ��������
	CFG_TIME_SECTION    stuTimeSection[WEEK_DAY_NUM][MAX_REC_TSECT_EX];         // �¼���Ӧʱ���

	int                 nLaneNumber;                                            // �������
}CFG_TRAFFIC_NONMOTOR_OVERLOAD_INFO;

// �¼�����EVENT_IVS_TRAFFIC_NONMOTOR_HOLDUMBRELLA(�ǻ�����װ��ɡ���¼�)��Ӧ�Ĺ�������
typedef struct tagCFG_TRAFFIC_NONMOTOR_HOLDUMBRELLA_INFO
{
	// ��Ϣ
	char                szRuleName[MAX_NAME_LEN];                               // ��������,��ͬ����������
	bool                bRuleEnable;                                            // ����ʹ��
	BYTE                bReserved[3];                                           // �����ֶ�
	int                 nObjectTypeNum;	                                        // ��Ӧ�������͸���
	char                szObjectTypes[MAX_OBJECT_LIST_SIZE][MAX_NAME_LEN];      // ��Ӧ���������б�
	int                 nPtzPresetId;                                           // ��̨Ԥ�õ���	0~65535
	CFG_ALARM_MSG_HANDLE stuEventHandler;                                       // ��������
	CFG_TIME_SECTION    stuTimeSection[WEEK_DAY_NUM][MAX_REC_TSECT_EX];         // �¼���Ӧʱ���

	int                 nLaneNumber;                                            // �������
	int                 nSnapInterval;                                          // ץ�ļ��ʱ��
	BOOL                bSnapMotorcycle;                                        // �Ƿ�ץ��Ħ�г�
}CFG_TRAFFIC_NONMOTOR_HOLDUMBRELLA_INFO;

// �¼�����EVENT_IVS_TRAFFIC_NONMOTOR_WITHOUTSAFEHAT (�ǻ�����δ����ȫñ����)��Ӧ�Ĺ�������
typedef struct tagCFG_TRAFFIC_NONMOTOR_WITHOUTSAFEHAT_INFO
{
	// ��Ϣ
	char                szRuleName[MAX_NAME_LEN];                               // ��������,��ͬ����������
	bool                bRuleEnable;                                            // ����ʹ��
	BYTE                bReserved[3];                                           // �����ֶ�
	int                 nObjectTypeNum;	                                        // ��Ӧ�������͸���
	char                szObjectTypes[MAX_OBJECT_LIST_SIZE][MAX_NAME_LEN];      // ��Ӧ���������б�
	int                 nPtzPresetId;                                           // ��̨Ԥ�õ���	0~65535
	CFG_ALARM_MSG_HANDLE stuEventHandler;                                       // ��������
	CFG_TIME_SECTION    stuTimeSection[WEEK_DAY_NUM][MAX_REC_TSECT_EX];         // �¼���Ӧʱ���

	int                 nLaneNumber;                                            // �������
}CFG_TRAFFIC_NONMOTOR_WITHOUTSAFEHAT_INFO;

// �¼�����EVENT_IVS_CITY_MOTORPARKING (���л�����Υͣ�¼�)��Ӧ�Ĺ�������
typedef struct tagCFG_CITY_MOTORPARKING_INFO
{
	// ��Ϣ
	char                szRuleName[MAX_NAME_LEN];                               // ��������,��ͬ����������
	BOOL                bRuleEnable;                                            // ����ʹ��
	int                 nObjectTypeNum;	                                        // ��Ӧ�������͸���
	char                szObjectTypes[MAX_OBJECT_LIST_SIZE][MAX_NAME_LEN];      // ��Ӧ���������б�
	int                 nPtzPresetId;                                           // ��̨Ԥ�õ���	0~65535
	CFG_ALARM_MSG_HANDLE stuEventHandler;                                       // ��������
	CFG_TIME_SECTION    stuTimeSection[WEEK_DAY_NUM][MAX_REC_TSECT_EX];         // �¼���Ӧʱ���

	int                 nDetectRegionPoint;                                     // �����������
	CFG_POLYGON         stuDetectRegion[MAX_POLYGON_NUM];                       // �����

	int                 nMinDuration;											// ��̳���ʱ��,��λ���� ��Χ[0�� 3600]
	int                 nTrackDuration;											// ���ٳ���ʱ�� ��Χ[0�� 3600]
}CFG_CITY_MOTORPARKING_INFO;

// �¼�����EVENT_IVS_CITY_NONMOTORPARKING (���зǻ�����Υͣ�¼�)��Ӧ�Ĺ�������
typedef struct tagCFG_CITY_NONMOTORPARKING_INFO
{
	// ��Ϣ
	char                szRuleName[MAX_NAME_LEN];                               // ��������,��ͬ����������
	BOOL                bRuleEnable;                                            // ����ʹ��
	int                 nObjectTypeNum;	                                        // ��Ӧ�������͸���
	char                szObjectTypes[MAX_OBJECT_LIST_SIZE][MAX_NAME_LEN];      // ��Ӧ���������б�
	int                 nPtzPresetId;                                           // ��̨Ԥ�õ���	0~65535
	CFG_ALARM_MSG_HANDLE stuEventHandler;                                       // ��������
	CFG_TIME_SECTION    stuTimeSection[WEEK_DAY_NUM][MAX_REC_TSECT_EX];         // �¼���Ӧʱ���

	int                 nDetectRegionPoint;                                     // �����������
	CFG_POLYGON         stuDetectRegion[MAX_POLYGON_NUM];                       // �����

	int                 nAlarmNum;												// ����������ֵ����λ���� ��Χ[1�� 100]
	int                 nMinDuration;											// ��̳���ʱ��,��λ���� ��Χ[0�� 3600]
	int                 nTrackDuration;											// ���ٳ���ʱ�� ��Χ[0�� 3600]
}CFG_CITY_NONMOTORPARKING_INFO;

// �¼�����EVENT_IVS_FLOWBUSINESS (����̯���¼�)��Ӧ�Ĺ�������
typedef struct tagCFG_FLOWBUSINESS_INFO
{
	// ��Ϣ
	char                szRuleName[MAX_NAME_LEN];                               // ��������,��ͬ����������
	BOOL                bRuleEnable;                                            // ����ʹ��
	int                 nObjectTypeNum;	                                        // ��Ӧ�������͸���
	char                szObjectTypes[MAX_OBJECT_LIST_SIZE][MAX_NAME_LEN];      // ��Ӧ���������б�
	int                 nPtzPresetId;                                           // ��̨Ԥ�õ���	0~65535
	CFG_ALARM_MSG_HANDLE stuEventHandler;                                       // ��������
	CFG_TIME_SECTION    stuTimeSection[WEEK_DAY_NUM][MAX_REC_TSECT_EX];         // �¼���Ӧʱ���

	int                 nDetectRegionPoint;                                     // �����������
	CFG_POLYGON         stuDetectRegion[MAX_POLYGON_NUM];                       // �����

	int                 nSensitivity;											// �����ȣ�ֵԽС������Խ�� ��Χ[1�� 10]
	int                 nMinDuration;											// ��̳���ʱ��,��λ���� ��Χ[0�� 3600]
	int                 nTrackDuration;											// ���ٳ���ʱ�� ��Χ[0�� 3600]
}CFG_FLOWBUSINESS_INFO;

// �¼�����EVENT_IVS_SHOPPRESENCE (����ռ���¼�)��Ӧ�Ĺ�������
typedef struct tagCFG_SHOPPRESENCE_INFO
{
	// ��Ϣ
	char                szRuleName[MAX_NAME_LEN];                               // ��������,��ͬ����������
	BOOL                bRuleEnable;                                            // ����ʹ��
	int                 nObjectTypeNum;	                                        // ��Ӧ�������͸���
	char                szObjectTypes[MAX_OBJECT_LIST_SIZE][MAX_NAME_LEN];      // ��Ӧ���������б�
	int                 nPtzPresetId;                                           // ��̨Ԥ�õ���	0~65535
	CFG_ALARM_MSG_HANDLE stuEventHandler;                                       // ��������
	CFG_TIME_SECTION    stuTimeSection[WEEK_DAY_NUM][MAX_REC_TSECT_EX];         // �¼���Ӧʱ���

	int                 nDetectRegionPoint;                                     // �����������
	CFG_POLYGON         stuDetectRegion[MAX_POLYGON_NUM];                       // �����

	int                 nSensitivity;											// �����ȣ�ֵԽС������Խ�� ��Χ[1�� 10]
	int                 nMinDuration;											// ��̳���ʱ��,��λ���� ��Χ[0�� 3600]
	int                 nTrackDuration;											// ���ٳ���ʱ�� ��Χ[0�� 3600]
	char				szShopAddress[256];										// ���̵�ַ
}CFG_SHOPPRESENCE_INFO;

// �¼�����EVENT_IVS_TRAFFIC_IDLE (��ͨ�����¼�)��Ӧ�Ĺ�������
typedef struct tagCFG_TRAFFIC_IDLE_INFO
{
	char				szRuleName[MAX_NAME_LEN];								// ��������,��ͬ����������
	bool				bRuleEnable;											// ����ʹ��
    BYTE                bJamLineMargin;                                         // ռ����ֵ��ӵ�³���ռ����������ֵ��0-100
	BYTE                bReserved[2];                                           // �����ֶ�
	int					nObjectTypeNum;											// ��Ӧ�������͸���
	char				szObjectTypes[MAX_OBJECT_LIST_SIZE][MAX_NAME_LEN];		// ��Ӧ���������б�
	int                 nPtzPresetId;											// ��̨Ԥ�õ���	0~65535
	int					nLane;													// �������
	int                 nDelayTime;                                             // ��⵽������������ʼ�ϱ���ʱ��, ��λ���룬��Χ1~65535
	int                 nIntervalTime;                                          // �������ʱ��, ��λ���룬��Χ1~65535
	int                 nReportTimes;                                           // �ϱ�����, 1~255
	CFG_ALARM_MSG_HANDLE stuEventHandler;										// ��������
	CFG_TIME_SECTION	stuTimeSection[WEEK_DAY_NUM][MAX_REC_TSECT_EX];			// �¼���Ӧʱ���
	int		            nDetectRegionPoint;                                     // �����������
	CFG_POLYGON         stuDetectRegion[MAX_POLYGON_NUM];                       // �������
}CFG_TRAFFIC_IDLE_INFO;


// �¼�����EVENT_IVS_TRAFFIC_PARKING (��ͨΥ��ͣ���¼�) / EVENT_IVS_TRAFFIC_PARKING_B (B�ཻͨΥ��ͣ���¼�) /
// EVENT_IVS_TRAFFIC_PARKING_C (C�ཻͨΥ��ͣ���¼�) / EVENT_IVS_TRAFFIC_PARKING_D (D�ཻͨΥ��ͣ���¼�)/
// EVENT_IVS_TRAFFIC_PARKING_MANUAL(�ֶ�ȡ֤��ͨΥ��)��Ӧ�Ĺ�������
typedef struct tagCFG_TRAFFIC_PARKING_INFO
{
	char				szRuleName[MAX_NAME_LEN];								// ��������,��ͬ����������
	bool				bRuleEnable;											// ����ʹ��
    bool                bTrackEnable;                                           // ��������ʹ��,�԰���,���ֵ��������
    BYTE                bSensitivity;                                           // ������, ȡֵ1-10��ֵԽС������Խ��(ֻ�Լ��������Ƶ�ڵ��������������������仯��Ч)
	BYTE                bParkingNumThreshold;                                   // ͣ������ֵ��������ͣ���������ڴ�ֵʱ������,ȡֵ1-128
	int					nObjectTypeNum;											// ��Ӧ�������͸���
	char				szObjectTypes[MAX_OBJECT_LIST_SIZE][MAX_NAME_LEN];		// ��Ӧ���������б�
	int                 nPtzPresetId;											// ��̨Ԥ�õ���	0~65535
	int					nLane;													// �������
	int                 nDelayTime;                                             // ��⵽������������ʼ�ϱ���ʱ��, ��λ���룬��Χ1~65535
	int                 nParkingAllowedTime;                                    // ����ͣ��ʱ��, ��λ���룬��Χ1~65535
	int                 nReportTimes;                                           // �ϱ�����, 1~255
	CFG_ALARM_MSG_HANDLE stuEventHandler;										// ��������
	CFG_TIME_SECTION	stuTimeSection[WEEK_DAY_NUM][MAX_REC_TSECT_EX];			// �¼���Ӧʱ���
		
	int					nDetectRegionPoint;										// ������򶥵���
	CFG_POLYGON			stuDetectRegion[MAX_POLYGON_NUM];						// �������
}CFG_TRAFFIC_PARKING_INFO;

// �¼�����EVENT_IVS_TRAFFIC_PARKING_SPACEDETECTION(Υͣ������Ƶ���λ���)��������
typedef struct tagCFG_TRAFFIC_PARKING_SPACEDETECTION_INFO
{
	char				szRuleName[MAX_NAME_LEN];								// ��������,��ͬ����������
	bool				bRuleEnable;											// ����ʹ��
	bool                bTrackEnable;                                           // ��������ʹ��,�԰���,���ֵ��������
	BYTE                bSensitivity;                                           // ������, ȡֵ1-10��ֵԽС������Խ��(ֻ�Լ��������Ƶ�ڵ��������������������仯��Ч)
	BYTE                bReserved;                                              // �����ֶ�
	int					nObjectTypeNum;											// ��Ӧ�������͸���
	char				szObjectTypes[MAX_OBJECT_LIST_SIZE][MAX_NAME_LEN];		// ��Ӧ���������б�
	int                 nPtzPresetId;											// ��̨Ԥ�õ���	0~65535
	int					nLane;													// �������
	int                 nDelayTime;                                             // ��⵽������������ʼ�ϱ���ʱ��, ��λ���룬��Χ1~65535
	int                 nParkingAllowedTime;                                    // ����ͣ��ʱ��, ��λ���룬��Χ1~65535
	int                 nReportTimes;                                           // �ϱ�����, 1~255
	CFG_ALARM_MSG_HANDLE stuEventHandler;										// ��������
	CFG_TIME_SECTION	stuTimeSection[WEEK_DAY_NUM][MAX_REC_TSECT_EX];			// �¼���Ӧʱ���

	int					nDetectRegionPoint;										// ������򶥵���
	CFG_POLYGON			stuDetectRegion[MAX_POLYGON_NUM];						// �������
}CFG_TRAFFIC_PARKING_SPACEDETECTION_INFO;

// �¼����� EVENT_IVS_TRAFFIC_RUNREDLIGHT (��ͨΥ��-������¼�)��Ӧ�Ĺ�������
typedef struct tagCFG_TRAFFIC_RUNREDLIGHT_INFO
{
	char				szRuleName[MAX_NAME_LEN];								// ��������,��ͬ����������
	bool				bRuleEnable;											// ����ʹ��
	BYTE                bReserved[3];                                           // �����ֶ�
	int					nObjectTypeNum;											// ��Ӧ�������͸���
	char				szObjectTypes[MAX_OBJECT_LIST_SIZE][MAX_NAME_LEN];		// ��Ӧ���������б�
	int                 nPtzPresetId;											// ��̨Ԥ�õ���	0~65535
	int					nLane;													// �������
	int                 nDirection;                                             // ��������(�������ķ���)��0-�� 1-���� 2-�� 3-���� 4-�� 5-���� 6-�� 7-����
	int					nPreLinePoint;							                // ǰ�ü���߶�����
	CFG_POLYLINE		stuPreLine[MAX_POLYLINE_NUM];			                // ǰ�ü����
	int					nMiddleLinePoint;						                // �м����߶�����
	CFG_POLYLINE		stuMiddleLine[MAX_POLYLINE_NUM];		                // �м�����
	int					nPostLinePoint;							                // ���ü���߶�����
	CFG_POLYLINE		stuPostLine[MAX_POLYLINE_NUM];			                // ���ü����
	CFG_ALARM_MSG_HANDLE stuEventHandler;										// ��������
	CFG_TIME_SECTION	stuTimeSection[WEEK_DAY_NUM][MAX_REC_TSECT_EX];			// �¼���Ӧʱ���
	
}CFG_TRAFFIC_RUNREDLIGHT_INFO;

// �¼����� EVENT_IVS_TRAFFIC_PEDESTRAINRUNREDLIGHT  (��ͨΥ��-���˴�����¼�)��Ӧ�Ĺ�������
typedef struct tagCFG_TRAFFIC_PEDESTRAINRUNREDLIGHT_INFO
{
    char				szRuleName[MAX_NAME_LEN];								// ��������,��ͬ����������
    bool				bRuleEnable;											// ����ʹ��
    BYTE                bReserved[3];                                           // �����ֶ�
    int					nObjectTypeNum;											// ��Ӧ�������͸���
    char				szObjectTypes[MAX_OBJECT_LIST_SIZE][MAX_NAME_LEN];		// ��Ӧ���������б�
    int                 nPtzPresetId;											// ��̨Ԥ�õ���	0~65535
    int					nLane;													// �������
    int                 nDirection;                                             // ��������(�������ķ���)��0-�� 1-���� 2-�� 3-���� 4-�� 5-���� 6-�� 7-����
    int					nPreLinePoint;							                // ǰ�ü���߶�����
    CFG_POLYLINE		stuPreLine[MAX_POLYLINE_NUM];			                // ǰ�ü����
    int					nMiddleLinePoint;						                // �м����߶�����
    CFG_POLYLINE		stuMiddleLine[MAX_POLYLINE_NUM];		                // �м�����
    int					nPostLinePoint;							                // ���ü���߶�����
    CFG_POLYLINE		stuPostLine[MAX_POLYLINE_NUM];			                // ���ü����
    CFG_ALARM_MSG_HANDLE stuEventHandler;										// ��������
    CFG_TIME_SECTION	stuTimeSection[WEEK_DAY_NUM][MAX_REC_TSECT_EX];			// �¼���Ӧʱ���
}CFG_TRAFFIC_PEDESTRAINRUNREDLIGHT_INFO;

// �¼����� EVENT_IVS_TRAFFIC_WRONGROUTE (��ͨΥ��-����������ʻ�¼�)��Ӧ�Ĺ�������
typedef struct tagCFG_TRAFFIC_WRONGROUTE_INFO
{
    char				szRuleName[MAX_NAME_LEN];								// ��������,��ͬ����������
    bool				bRuleEnable;											// ����ʹ��
    BYTE                bSensitivity;                                           // �����ȣ�ȡֵ1-10����ֵԽ�����������Խ��
    bool                bTrackEnable;                                           // ��������ʹ��,�԰���,���ֵ��������
    BYTE                bReserved;                                           // �����ֶ�
    int					nObjectTypeNum;											// ��Ӧ�������͸���
    char				szObjectTypes[MAX_OBJECT_LIST_SIZE][MAX_NAME_LEN];		// ��Ӧ���������б�
    int                 nPtzPresetId;											// ��̨Ԥ�õ���	0~65535
    int					nLane;													// ������� 
    CFG_ALARM_MSG_HANDLE stuEventHandler;										// ��������
    CFG_TIME_SECTION	stuTimeSection[WEEK_DAY_NUM][MAX_REC_TSECT_EX];			// �¼���Ӧʱ���
    
    int                 nDetectRegionPoint;                                     // �����������
    CFG_POLYGON         stuDetectRegion[MAX_POLYGON_NUM];                       // �����
    int                 nDirectionLinPoint;                                     // ������ʻ���򶥵���
    CFG_POLYLINE		stuDirectionLine[MAX_POLYLINE_NUM];						// ������ʻ���򣬵�һ��������㣬���һ�������յ�
	
}CFG_TRAFFIC_WRONGROUTE_INFO;

// �¼����� EVENT_IVS_TRAFFIC_CROSSLANE (��ͨΥ��-Υ�±��)��Ӧ�Ĺ�������
typedef struct tagCFG_TRAFFIC_CROSSLANE_INFO
{
    char				szRuleName[MAX_NAME_LEN];								// ��������,��ͬ����������
    bool				bRuleEnable;											// ����ʹ��
    BYTE                bSensitivity;                                           // �����ȣ�ȡֵ1-10����ֵԽ�����������Խ��
    BYTE                bReserved[2];                                           // �����ֶ�
    int					nObjectTypeNum;											// ��Ӧ�������͸���
    char				szObjectTypes[MAX_OBJECT_LIST_SIZE][MAX_NAME_LEN];		// ��Ӧ���������б�
    int                 nPtzPresetId;											// ��̨Ԥ�õ���	0~65535
    int					nLane;													// ������� 
    CFG_ALARM_MSG_HANDLE stuEventHandler;										// ��������
    CFG_TIME_SECTION	stuTimeSection[WEEK_DAY_NUM][MAX_REC_TSECT_EX];			// �¼���Ӧʱ���
    
    int                 nDetectRegionPoint;                                     // �����������
    CFG_POLYGON         stuDetectRegion[MAX_POLYGON_NUM];                       // �����
    int                 nDirectionLinPoint;                                     // ������ʻ���򶥵���
    CFG_POLYLINE		stuDirectionLine[MAX_POLYLINE_NUM];						// ������ʻ���򣬵�һ��������㣬���һ�������յ�
	
}CFG_TRAFFIC_CROSSLANE_INFO;

// �¼����� EVENT_IVS_TRAFFIC_OVERLINE (��ͨΥ��-ѹ������)��Ӧ�Ĺ�������
typedef struct tagCFG_TRAFFIC_OVERLINE_INFO
{
    char				szRuleName[MAX_NAME_LEN];								// ��������,��ͬ����������
    bool				bRuleEnable;											// ����ʹ��
    BYTE                bSensitivity;                                           // �����ȣ�ȡֵ1-10����ֵԽ�����������Խ��
    BYTE                bReserved[2];                                           // �����ֶ�
    int					nObjectTypeNum;											// ��Ӧ�������͸���
    char				szObjectTypes[MAX_OBJECT_LIST_SIZE][MAX_NAME_LEN];		// ��Ӧ���������б�
    int                 nPtzPresetId;											// ��̨Ԥ�õ���	0~65535
    int					nLane;													// ������� 
    CFG_ALARM_MSG_HANDLE stuEventHandler;										// ��������
    CFG_TIME_SECTION	stuTimeSection[WEEK_DAY_NUM][MAX_REC_TSECT_EX];			// �¼���Ӧʱ���
    
    int                 nDetectRegionPoint;                                     // �����������
    CFG_POLYGON         stuDetectRegion[MAX_POLYGON_NUM];                       // �����
    int                 nDirectionLinPoint;                                     // ������ʻ���򶥵���
    CFG_POLYLINE		stuDirectionLine[MAX_POLYLINE_NUM];						// ������ʻ���򣬵�һ��������㣬���һ�������յ�

	
}CFG_TRAFFIC_OVERLINE_INFO;

// �¼����� EVENT_IVS_TRAFFIC_OVERYELLOWLINE (��ͨΥ��-ѹ����)��Ӧ�Ĺ�������
typedef struct tagCFG_TRAFFIC_OVERYELLOWLINE_INFO
{
	char				szRuleName[MAX_NAME_LEN];								// ��������,��ͬ����������
	bool				bRuleEnable;											// ����ʹ��
	BYTE                bReserved[3];                                           // �����ֶ�
	int					nObjectTypeNum;											// ��Ӧ�������͸���
	char				szObjectTypes[MAX_OBJECT_LIST_SIZE][MAX_NAME_LEN];		// ��Ӧ���������б�
	int                 nPtzPresetId;											// ��̨Ԥ�õ���	0~65535
	int					nLane;													// �������
	CFG_ALARM_MSG_HANDLE stuEventHandler;										// ��������
	CFG_TIME_SECTION	stuTimeSection[WEEK_DAY_NUM][MAX_REC_TSECT_EX];			// �¼���Ӧʱ���

	int					nDetectRegionPoint;										// ������򶥵���
	CFG_POLYGON			stuDetectRegion[MAX_POLYGON_NUM];						// �������

}CFG_TRAFFIC_OVERYELLOWLINE_INFO;

// �¼����� EVENT_IVS_TRAFFIC_RETROGRADE (��ͨΥ��-�����¼�)��Ӧ�Ĺ�������
typedef struct tagCFG_TRAFFIC_RETROGRADE_INFO
{
	char				szRuleName[MAX_NAME_LEN];								// ��������,��ͬ����������
	bool				bRuleEnable;											// ����ʹ��
    bool                bTrackEnable;                                           // ��������ʹ��,�԰���,���ֵ��������
	BYTE                bReserved[2];                                           // �����ֶ�
	int					nObjectTypeNum;											// ��Ӧ�������͸���
	char				szObjectTypes[MAX_OBJECT_LIST_SIZE][MAX_NAME_LEN];		// ��Ӧ���������б�
	int                 nPtzPresetId;											// ��̨Ԥ�õ���	0~65535
	int					nLane;													// �������
	BOOL                bLegal;                                                 // TRUE: ���кϷ�;FALSE:���в��Ϸ�
	CFG_ALARM_MSG_HANDLE stuEventHandler;										// ��������
	CFG_TIME_SECTION	stuTimeSection[WEEK_DAY_NUM][MAX_REC_TSECT_EX];			// �¼���Ӧʱ���

	int					nDetectRegionPoint;										// ������򶥵���
	CFG_POLYGON			stuDetectRegion[MAX_POLYGON_NUM];						// �������
	int                 nDirectionLinPoint;                                     // ������ʻ���򶥵���
	CFG_POLYLINE		stuDirectionLine[MAX_POLYLINE_NUM];						// ������ʻ���򣬵�һ��������㣬���һ�������յ�
	int					nMinDuration;											// ��̴���ʱ��	��λ����
}CFG_TRAFFIC_RETROGRADE_INFO;

// �¼����� EVENT_IVS_TRAFFIC_TURNLEFT (��ͨΥ��-Υ����ת�¼�)��Ӧ�Ĺ�������
typedef struct tagCFG_TRAFFIC_TURNLEFT_INFO
{
    char				szRuleName[MAX_NAME_LEN];								// ��������,��ͬ����������
    bool				bRuleEnable;											// ����ʹ��
    BYTE                bSensitivity;                                           // �����ȣ�ȡֵ1-10����ֵԽ�����������Խ��
    BYTE                bReserved[2];                                           // �����ֶ�
    int					nObjectTypeNum;											// ��Ӧ�������͸���
    char				szObjectTypes[MAX_OBJECT_LIST_SIZE][MAX_NAME_LEN];		// ��Ӧ���������б�
    int                 nPtzPresetId;											// ��̨Ԥ�õ���	0~65535
    int					nLane;													// ������� 
    CFG_ALARM_MSG_HANDLE stuEventHandler;										// ��������
    CFG_TIME_SECTION	stuTimeSection[WEEK_DAY_NUM][MAX_REC_TSECT_EX];			// �¼���Ӧʱ���
    
    int                 nDetectRegionPoint;                                     // �����������
    CFG_POLYGON         stuDetectRegion[MAX_POLYGON_NUM];                       // �����
    int                 nDirectionLinPoint;                                     // ������ʻ���򶥵���
    CFG_POLYLINE		stuDirectionLine[MAX_POLYLINE_NUM];						// ������ʻ���򣬵�һ��������㣬���һ�������յ�

	
}CFG_TRAFFIC_TURNLEFT_INFO;

// �¼����� EVENT_IVS_TRAFFIC_TURNRIGHT (��ͨΥ��-Υ����ת)��Ӧ�Ĺ�������
typedef struct tagCFG_TRAFFIC_TURNRIGHT_INFO
{
    char				szRuleName[MAX_NAME_LEN];								// ��������,��ͬ����������
    bool				bRuleEnable;											// ����ʹ��
    BYTE                bSensitivity;                                           // �����ȣ�ȡֵ1-10����ֵԽ�����������Խ��
    BYTE                bReserved[2];                                           // �����ֶ�
    int					nObjectTypeNum;											// ��Ӧ�������͸���
    char				szObjectTypes[MAX_OBJECT_LIST_SIZE][MAX_NAME_LEN];		// ��Ӧ���������б�
    int                 nPtzPresetId;											// ��̨Ԥ�õ���	0~65535
    int					nLane;													// ������� 
    CFG_ALARM_MSG_HANDLE stuEventHandler;										// ��������
    CFG_TIME_SECTION	stuTimeSection[WEEK_DAY_NUM][MAX_REC_TSECT_EX];			// �¼���Ӧʱ���
    
    int                 nDetectRegionPoint;                                     // �����������
    CFG_POLYGON         stuDetectRegion[MAX_POLYGON_NUM];                       // �����
    int                 nDirectionLinPoint;                                     // ������ʻ���򶥵���
    CFG_POLYLINE		stuDirectionLine[MAX_POLYLINE_NUM];						// ������ʻ���򣬵�һ��������㣬���һ�������յ�
}CFG_TRAFFIC_TURNRIGHT_INFO;

// �¼����� EVENT_IVS_TRAFFIC_UTURN (��ͨΥ��-Υ�µ�ͷ)��Ӧ�Ĺ�������
typedef struct tagCFG_TRAFFIC_UTURN_INFO
{
	char				szRuleName[MAX_NAME_LEN];								// ��������,��ͬ����������
	bool				bRuleEnable;											// ����ʹ��
    bool                bTrackEnable;                                           // ��������ʹ��,�԰��ߡ����ֵ��������
	BYTE                bReserved[2];                                           // �����ֶ�
	int					nObjectTypeNum;											// ��Ӧ�������͸���
	char				szObjectTypes[MAX_OBJECT_LIST_SIZE][MAX_NAME_LEN];		// ��Ӧ���������б�
	int                 nPtzPresetId;											// ��̨Ԥ�õ���	0~65535
	int					nLane;													// �������
	CFG_ALARM_MSG_HANDLE stuEventHandler;										// ��������
	CFG_TIME_SECTION	stuTimeSection[WEEK_DAY_NUM][MAX_REC_TSECT_EX];			// �¼���Ӧʱ���


	int					nDetectRegionPoint;										// ������򶥵���
	CFG_POLYGON			stuDetectRegion[MAX_POLYGON_NUM];						// �������
	int					nTriggerAngle;											// ��ͷ�����Ƕ�,ȡֵ��Χ(90,180)��Ŀ���ͷǰ����˶��н�
	BOOL                abSizeFileter;                                           // �����ض��ĳߴ�������Ƿ���Ч
	CFG_SIZEFILTER_INFO stuSizeFileter;                                         // �����ض��ĳߴ������
	
}CFG_TRAFFIC_UTURN_INFO;

// �¼����� EVENT_IVS_TRAFFIC_OVERSPEED (��ͨΥ��-����)��Ӧ�Ĺ�������
typedef struct tagCFG_TRAFFIC_OVERSPEED_INFO
{
	char				szRuleName[MAX_NAME_LEN];								// ��������,��ͬ����������
	bool				bRuleEnable;											// ����ʹ��
	BYTE                bReserved[3];                                           // �����ֶ�
	int					nObjectTypeNum;											// ��Ӧ�������͸���
	char				szObjectTypes[MAX_OBJECT_LIST_SIZE][MAX_NAME_LEN];		// ��Ӧ���������б�
	int                 nPtzPresetId;											// ��̨Ԥ�õ���	0~65535
	int					nLane;													// �������
	int                 nSpeedUpperLimit;                                       // �ٶ�����
	int                 nSpeedLowerLimit;                                       // �ٶ�����
	int					nMinDuration;											// ��̴���ʱ��	��λ����
	CFG_ALARM_MSG_HANDLE stuEventHandler;										// ��������
	CFG_TIME_SECTION	stuTimeSection[WEEK_DAY_NUM][MAX_REC_TSECT_EX];			// �¼���Ӧʱ���
	
}CFG_TRAFFIC_OVERSPEED_INFO;

// �¼����� EVENT_IVS_TRAFFIC_UNDERSPEED (��ͨΥ��-Ƿ��)��Ӧ�Ĺ�������
typedef struct tagCFG_TRAFFIC_UNDERSPEED_INFO
{
	char				szRuleName[MAX_NAME_LEN];								// ��������,��ͬ����������
	bool				bRuleEnable;											// ����ʹ��
	BYTE                bReserved[3];                                           // �����ֶ�
	int					nObjectTypeNum;											// ��Ӧ�������͸���
	char				szObjectTypes[MAX_OBJECT_LIST_SIZE][MAX_NAME_LEN];		// ��Ӧ���������б�
	int                 nPtzPresetId;											// ��̨Ԥ�õ���	0~65535
	int					nLane;													// �������
	int                 nSpeedUpperLimit;                                       // �ٶ�����
	int                 nSpeedLowerLimit;                                       // �ٶ�����
	int					nMinDuration;											// ��̴���ʱ��	��λ����
	CFG_ALARM_MSG_HANDLE stuEventHandler;										// ��������
	CFG_TIME_SECTION	stuTimeSection[WEEK_DAY_NUM][MAX_REC_TSECT_EX];			// �¼���Ӧʱ���
	
}CFG_TRAFFIC_UNDERSPEED_INFO;

// �¼����� EVENT_IVS_TRAFFIC_YELLOWPLATEINLANE (��ͨΥ��-���Ƴ�ռ���¼�)��Ӧ�Ĺ�������
typedef struct tagCFG_TRAFFIC_YELLOWPLATEINLANE_INFO
{
    char				szRuleName[MAX_NAME_LEN];								// ��������,��ͬ����������
    bool				bRuleEnable;											// ����ʹ��
    BYTE                bSensitivity;                                           // �����ȣ�ȡֵ1-10����ֵԽ�����������Խ��
    BYTE                bReserved[2];                                           // �����ֶ�
    int					nObjectTypeNum;											// ��Ӧ�������͸���
    char				szObjectTypes[MAX_OBJECT_LIST_SIZE][MAX_NAME_LEN];		// ��Ӧ���������б�
    int                 nPtzPresetId;											// ��̨Ԥ�õ���	0~65535
    int					nLane;													// ������� 
    CFG_ALARM_MSG_HANDLE stuEventHandler;										// ��������
    CFG_TIME_SECTION	stuTimeSection[WEEK_DAY_NUM][MAX_REC_TSECT_EX];			// �¼���Ӧʱ���
    
    int                 nDetectRegionPoint;                                     // �����������
    CFG_POLYGON         stuDetectRegion[MAX_POLYGON_NUM];                       // �����
    int                 nDirectionLinPoint;                                     // ������ʻ���򶥵���
    CFG_POLYLINE		stuDirectionLine[MAX_POLYLINE_NUM];						// ������ʻ���򣬵�һ��������㣬���һ�������յ�

}CFG_TRAFFIC_YELLOWPLATEINLANE_INFO;

// �¼����� EVENT_IVS_TRAFFIC_VEHICLEINROUTE (��ͨΥ��-�г�ռ���¼�)��Ӧ�Ĺ�������
typedef struct tagCFG_TRAFFIC_VEHICLEINROUTE_INFO
{
	char				szRuleName[MAX_NAME_LEN];								// ��������,��ͬ����������
    bool				bRuleEnable;											// ����ʹ��
    BYTE                bSensitivity;                                           // �����ȣ�ȡֵ1-10����ֵԽ�����������Խ��
    BYTE                bReserved[2];                                           // �����ֶ�
    int					nObjectTypeNum;											// ��Ӧ�������͸���
    char				szObjectTypes[MAX_OBJECT_LIST_SIZE][MAX_NAME_LEN];		// ��Ӧ���������б�
    int                 nPtzPresetId;											// ��̨Ԥ�õ���	0~65535
    int					nLane;													// ������� 
    CFG_ALARM_MSG_HANDLE stuEventHandler;										// ��������
    CFG_TIME_SECTION	stuTimeSection[WEEK_DAY_NUM][MAX_REC_TSECT_EX];			// �¼���Ӧʱ���
    
    int                 nDetectRegionPoint;                                     // �����������
    CFG_POLYGON         stuDetectRegion[MAX_POLYGON_NUM];                       // �����
    int                 nDirectionLinPoint;                                     // ������ʻ���򶥵���
    CFG_POLYLINE		stuDirectionLine[MAX_POLYLINE_NUM];						// ������ʻ���򣬵�һ��������㣬���һ�������յ�
	
}CFG_TRAFFIC_VEHICLEINROUTE_INFO;

// �¼���Դ
typedef enum tagEM_CFG_EVENT_ORIGIN
{
	EM_CFG_EVENT_ORIGIN_UNKNOWN = -1,											// δ֪
	EM_CFG_EVENT_ORIGIN_VIDEO,													// ��Ƶ���
	EM_CFG_EVENT_ORIGIN_NET,													// ���� 
}EM_CFG_EVENT_ORIGIN;

// �¼����� EVENT_IVS_TRAFFIC_JAM_FORBID_INTO (����ӵ�½����¼�)��Ӧ�Ĺ�������
typedef struct tagCFG_TRAFFIC_JAM_FORBID_INTO_INFO
{
	char				szRuleName[MAX_NAME_LEN];								// ��������,��ͬ����������
	BOOL				bRuleEnable;											// ����ʹ��
	int					nObjectTypeNum;											// ��Ӧ�������͸���
	char				szObjectTypes[MAX_OBJECT_LIST_SIZE][MAX_NAME_LEN];		// ��Ӧ���������б�
	int                 nPtzPresetId;											// ��̨Ԥ�õ���	0~65535
	CFG_ALARM_MSG_HANDLE stuEventHandler;										// ��������
	CFG_TIME_SECTION	stuTimeSection[WEEK_DAY_NUM][MAX_REC_TSECT_EX];			// �¼���Ӧʱ���

	int					nLaneNumber;											// ������� 
	int					nTriggerTime;											// ǰ��ĳ�ͣ�˶���ʱ�����ӵ��,����ĳ���������ӵ�½���,��λ�� 
	int					nFollowTime;											// ����ʱ����ֵ,����ǰ������·�ڵĲ���ӵ�½���Υ��,��λ�� 
	int					nDelayTime;												// �̵������ʱץ�ĵ�����ʱ��,��λ��
	EM_CFG_EVENT_ORIGIN	emEventOrigin;											// ӵ���¼���Դ 
	int                 nJamForbidIntoLineCount;								// ӵ���¼��߽��ߵ���
	CFG_POLYLINE        stuJamForbidIntoLine[MAX_POLYLINE_NUM];					// ӵ���¼��߽���, �����һ����[0,8192)����
}CFG_TRAFFIC_JAM_FORBID_INTO_INFO;

// �¼����� EVENT_IVS_TRAFFIC_JAM_STOP_ON_ZEBRACROSSING (ӵ�������������¼�)��Ӧ�Ĺ�������
typedef struct tagCFG_TRAFFIC_JAM_STOP_ON_ZEBRACROSSING_INFO
{
	char				szRuleName[MAX_NAME_LEN];								// ��������,��ͬ����������
	BOOL				bRuleEnable;											// ����ʹ��
	int					nObjectTypeNum;											// ��Ӧ�������͸���
	char				szObjectTypes[MAX_OBJECT_LIST_SIZE][MAX_NAME_LEN];		// ��Ӧ���������б�
	int                 nPtzPresetId;											// ��̨Ԥ�õ���	0~65535
	CFG_ALARM_MSG_HANDLE stuEventHandler;										// ��������
	CFG_TIME_SECTION	stuTimeSection[WEEK_DAY_NUM][MAX_REC_TSECT_EX];			// �¼���Ӧʱ���

	int					nLaneNumber;											// ������� 
	int					nJamStopDelay;											// ӵ��������ʱʱ��
	int					nSnapInterval;											// ץ�ļ��ʱ��
	BOOL				bSnapMotorcycle;										// �Ƿ�ץ��Ħ�г�

	int					nDetectRegionPoint;										// �����������
	CFG_POLYGON			stuDetectRegion[MAX_POLYGON_NUM];						// �����
}CFG_TRAFFIC_JAM_STOP_ON_ZEBRACROSSING_INFO;

// 

// �¼����� EVENT_IVS_PRISONERRISEDETECTION (�������������¼�)��Ӧ�Ĺ�������
typedef struct tagCFG_PRISONRISEDETECTION_INFO
{
	char				szRuleName[MAX_NAME_LEN];								// ��������,��ͬ����������
	bool				bRuleEnable;											// ����ʹ��
	BYTE                bReserved[3];                                           // �����ֶ�
	int					nObjectTypeNum;											// ��Ӧ�������͸���
	char				szObjectTypes[MAX_OBJECT_LIST_SIZE][MAX_NAME_LEN];		// ��Ӧ���������б�
	int                 nPtzPresetId;											// ��̨Ԥ�õ���	0~65535
	int					nDetectRegionPoint;										// ������������
	CFG_POLYGON			stuDetectRegion[MAX_POLYGON_NUM10];						// ������
	CFG_POLYGON			stDirection[2];											// ��˯������	��һ��������㣬�ڶ��������յ㣬��������һ����[0,8191]����
	CFG_POLYGON			stBedMiddleLine[2];										// ��������	��������һ����[0,8191]����
	int					nBedSide;												// ���̵ķ�λ	1�����λ,2�ұ���λ
	int					nMinDuration;											// ��̴���ʱ��	��λ����
	int					nReportInterval;										// �ظ��������	��λ����
	BOOL                bSizeFileter;                                           // �����ض��ĳߴ�������Ƿ���Ч
	CFG_SIZEFILTER_INFO stuSizeFileter;                                         // �����ض��ĳߴ������
	CFG_ALARM_MSG_HANDLE stuEventHandler;										// ��������
	CFG_TIME_SECTION	stuTimeSection[WEEK_DAY_NUM][MAX_REC_TSECT_EX];			// �¼���Ӧʱ���
}CFG_PRISONRISEDETECTION_INFO;

// �������õĸ���
#define MAX_AREACONFIG_NUMBER	8

// �������������˷���
typedef enum tagEM_CFG_PEOPLE_DIRECTION
{
	EM_CFG_PEOPLE_DIRECTION_LEFT = 1,											// ��
	EM_CFG_PEOPLE_DIRECTION_RIGHT = 2,											// ��
}EM_CFG_PEOPLE_DIRECTION;

// �����������
typedef struct tagCFG_AREACONFIG
{
	int					nPeopleTargets;											// �������������˸���
	UINT				nPeopleDirectionMask;									// �������������˷���, EM_CFG_PEOPLE_DIRECTION �� λ���ϣ�����nPeopleDirectionMask |= EM_CFG_PEOPLE_DIRECTION_LEFT;
	BYTE                byReserved[1024];                                       // �����ֶ�
}CFG_AREACONFIG;

// �¼����� EVENT_IVS_TRAFFIC_PEDESTRAINPRIORITY (�������ü��/���������������¼�)��Ӧ�Ĺ�������
typedef struct tagCFG_TRAFFIC_PEDESTRAINPRIORITY_INFO
{
	char                szRuleName[MAX_NAME_LEN];                               // ��������,��ͬ����������
	bool                bRuleEnable;                                            // ����ʹ��
	BYTE                bReserved[3];                                           // �����ֶ�
	int                 nObjectTypeNum;                                         // ��Ӧ�������͸���
	char                szObjectTypes[MAX_OBJECT_LIST_SIZE][MAX_NAME_LEN];      // ��Ӧ���������б�
	int                 nPtzPresetId;                                           // ��̨Ԥ�õ���	0~65535
	int                 nLane;                                                  // �������
	CFG_ALARM_MSG_HANDLE stuEventHandler;                                       // ��������
	CFG_TIME_SECTION    stuTimeSection[WEEK_DAY_NUM][MAX_REC_TSECT_EX];         // �¼���Ӧʱ���

	// 
	int					nAreaConfigNum;											// �����Ӧ�ļ���������õĸ���
	CFG_AREACONFIG		stuAreaConfigList[MAX_AREACONFIG_NUMBER];				// �����Ӧ�ļ����������
	int					nSpeedLimit;											// ���ٷ�ֵ
	int					nFollowTime;											// ����ʱ�䣬��λ��
	BOOL				bSnapMotorcycle;										// �Ƿ�ץ��Ħ�г�,ͬһ����Ƶͨ�����ֹ�����Ҫץ��Ħ�г�����������Ҫ������ʹ�ù���
	int					nAlarmDistanceRate;										// �������������������˵ļ�����
	int					nSnapPicHavePriCategory;								// Υ��ץ�ĵ�ͼƬ�Ƿ�Ҫ�����˻�ǻ�����������ֵÿ��bitλ���壬1��ץ��ͼƬҪ�����ˣ�0��ץ��ͼƬ����������
																				// bit0:��һ��ͼƬ�Ƿ���Ҫ������
																				// bit1:�ڶ���ͼƬ�Ƿ���Ҫ������
																				// bit2:������ͼƬ�Ƿ���Ҫ������
}CFG_TRAFFIC_PEDESTRAINPRIORITY_INFO;

// �¼����� EVENT_IVS_QUEUEDETECTION (�ŶӼ���¼�)��Ӧ�Ĺ�������
typedef struct tagCFG_IVS_QUEUEDETECTION_INFO
{
	char				szRuleName[MAX_NAME_LEN];								// ��������,��ͬ����������
	bool				bRuleEnable;											// ����ʹ��
	BYTE                bReserved[3];                                           // �����ֶ�
	int					nObjectTypeNum;											// ��Ӧ�������͸���
	char				szObjectTypes[MAX_OBJECT_LIST_SIZE][MAX_NAME_LEN];		// ��Ӧ���������б�
	int                 nPtzPresetId;											// ��̨Ԥ�õ���	0~65535
	CFG_ALARM_MSG_HANDLE stuEventHandler;										// ��������
	CFG_TIME_SECTION	stuTimeSection[WEEK_DAY_NUM][MAX_REC_TSECT_EX];			// �¼���Ӧʱ���
	
	CFG_POLYLINE		stuDetectLine[2];						                // ������
	int                 nOccupyLineMargin;                                      // ռ����ֵ�����鳤��ռ�߱�����ֵ
	int                 nTriggerTime;                                           // ��λ�룬�ŶӼ�ⱨ��ʱ�䣬�Ŷӳ�����ֵ��״̬ά����"�ŶӼ�ⱨ��ʱ��"�Ժ󱨾�
	int					nMaxDetectRegionNum;									// ���������ĸ���
	CFG_POLYGON			stuDetectRegion[MAX_POLYGON_NUM];						// �������
	int					nThreshold;												// �쳣������ֵ
	int					nDetectType;											// �����쳣��ⷽʽ
	int					nPlanID;												// �ƻ�ID
	int					nAreaID;												// ����ID��һ��Ԥ�õ���Զ�Ӧ�������ID��
	BOOL				bStayDetectEnable;										// ������ⱨ��ʹ�ܣ���Ӧ����Ϊ EVENT_IVS_STAYDETECTION
	int 				nStayMinDuration;										// �����������ʱ�䣬��λ��[1, 1800], ����������ⱨ��ʱ������
	BOOL				bManNumAlarmEnable;										// �����쳣����ʹ��, Ĭ��ΪTRUE����Ӧ����Ϊ EVENT_IVS_MAN_NUM_DETECTION
}CFG_IVS_QUEUEDETECTION_INFO;

// �¼����� EVENT_IVS_CLIMBDETECTION (�ʸ߼���¼�)��Ӧ�Ĺ�������
typedef struct tagCFG_IVS_CLIMBDETECTION_INFO
{
	char				szRuleName[MAX_NAME_LEN];								// ��������,��ͬ����������
	bool				bRuleEnable;											// ����ʹ��
	BYTE                bReserved[3];                                           // �����ֶ�
	int					nObjectTypeNum;											// ��Ӧ�������͸���
	char				szObjectTypes[MAX_OBJECT_LIST_SIZE][MAX_NAME_LEN];		// ��Ӧ���������б�
	int                 nPtzPresetId;											// ��̨Ԥ�õ���	0~65535
	CFG_ALARM_MSG_HANDLE stuEventHandler;										// ��������
	CFG_TIME_SECTION	stuTimeSection[WEEK_DAY_NUM][MAX_REC_TSECT_EX];			// �¼���Ӧʱ���

	int                 nDetectLine;                                            // ����߶������
	CFG_POLYLINE        stuDetectLine[MAX_POLYLINE_NUM];                        // ��Ӧ�ļ������,[0,8192)
	BOOL                bSizeFileter;                                           // �����ض��ĳߴ�������Ƿ���Ч
	CFG_SIZEFILTER_INFO stuSizeFileter;                                         // �����ض��ĳߴ������
}CFG_IVS_CLIMBDETECTION_INFO;

// �¼����� EVENT_IVS_LEAVEDETECTION (��ڼ���¼�)��Ӧ�Ĺ�������
enum CFG_EM_LEAVEDETECTION_MODE{
	CFG_EM_LEAVEDETECTION_MODE_NONE,
	CFG_EM_LEAVEDETECTION_MODE_PATROL,    // Ѳ��ģʽ
	CFG_EM_LEAVEDETECTION_MODE_SENTRY,    // ����ģʽ
	CFG_EM_LEAVEDETECTION_MODE_ERR=255
};

typedef enum EM_CFG_LEAVEDETECTION_TRIGGERMODE{
    CFG_LEAVEDETECTION_TRIGGERMODE_UNKNOWN,         //δ֪
    CFG_LEAVEDETECTION_TRIGGERMODE_NOPERSON,        //����
    CFG_LEAVEDETECTION_TRIGGERMODE_LEAVE,           //�뿪
    CFG_LEAVEDETECTION_TRIGGERMODE_STATIC,          //��ֹ(˯��) 
    CFG_LEAVEDETECTION_TRIGGERMODE_OUTPERSONLIMIT,  // ��������ֵ��������Χ��(ֵ�������쳣)
}CFG_LEAVEDETECTION_TRIGGERMODE;

typedef struct tagCFG_IVS_LEAVEDETECTION_INFO
{
	char				szRuleName[MAX_NAME_LEN];								// ��������,��ͬ����������
	bool				bRuleEnable;											// ����ʹ��
	BYTE                bReserved[3];                                           // �����ֶ�
	int					nObjectTypeNum;											// ��Ӧ�������͸���
	char				szObjectTypes[MAX_OBJECT_LIST_SIZE][MAX_NAME_LEN];		// ��Ӧ���������б�
	int                 nPtzPresetId;											// ��̨Ԥ�õ���	0~65535
	CFG_ALARM_MSG_HANDLE stuEventHandler;										// ��������
	CFG_TIME_SECTION	stuTimeSection[WEEK_DAY_NUM][MAX_REC_TSECT_EX];			// �¼���Ӧʱ���

	CFG_EM_LEAVEDETECTION_MODE emMode;                                          // ���ģʽ
	unsigned int        nMinDuration;                                           // ��̳���ʱ��,��λ����
	unsigned int        nReportInterval;                                        // ����ʱ����,��λ����
	BOOL                bSizeFileter;                                           // �����ض��ĳߴ�������Ƿ���Ч
	CFG_SIZEFILTER_INFO stuSizeFileter;                                         // �����ض��ĳߴ������

	int					nDetectRegionPoint;										// �����������
	CFG_POLYGON			stuDetectRegion[MAX_POLYGON_NUM];						// �����
    int                 nTriggerModeCount;										// ����ģʽ������Ч����
    CFG_LEAVEDETECTION_TRIGGERMODE  emTriggerMode[MAX_TRIGGER_MODE_NUMBER];                              // ����ģʽ����
    int                 anPersonLimit[2];                                       // ����ֵ��������Χ, ���ڴ˷�Χ��ʱ����,�����һ��Ԫ��Ϊ��Сֵ,�ڶ���Ԫ��Ϊ���ֵ,���ֵΪ0ʱ��ʾ����������
}CFG_IVS_LEAVEDETECTION_INFO;

//�¼�����	EVENT_IVS_TRAFFIC_PARKINGONYELLOWBOX(��������ץ���¼�)��Ӧ�Ĺ�������
typedef struct tagCFG_TRAFFIC_PARKINGONYELLOWBOX_INFO
{
	char				szRuleName[MAX_NAME_LEN];								// ��������,��ͬ����������
	bool				bRuleEnable;											// ����ʹ��
	BYTE                bReserved[3];                                           // �����ֶ�
	int					nObjectTypeNum;											// ��Ӧ�������͸���
	char				szObjectTypes[MAX_OBJECT_LIST_SIZE][MAX_NAME_LEN];		// ��Ӧ���������б�
	int                 nPtzPresetId;											// ��̨Ԥ�õ���	0~65535
	CFG_ALARM_MSG_HANDLE stuEventHandler;										// ��������
	CFG_TIME_SECTION	stuTimeSection[WEEK_DAY_NUM][MAX_REC_TSECT_EX];			// �¼���Ӧʱ���

	int					nLane;													// ������
	int					nInterval1;												// �ڶ��ź͵�һ�ŵ���ʱʱ�䣬��λ��
	int					nInterval2;												// �����ź͵ڶ��ŵ���ʱʱ�䣬��λ��
	int					nFollowTime;											// ����ʱ�䣬���һ������ǰһ��������������ʱ���С�ڴ�ֵ������Ϊ�Ǹ������룬����������������ͣ������Υ��

}CFG_TRAFFIC_PARKINGONYELLOWBOX_INFO;

//�¼�����	EVENT_IVS_TRAFFIC_PARKINGSPACEPARKING(��λ�г��¼�)��Ӧ�Ĺ�������
typedef struct tagCFG_TRAFFIC_PARKINGSPACEPARKING_INFO
{
	char				szRuleName[MAX_NAME_LEN];								// ��������,��ͬ����������
	bool				bRuleEnable;											// ����ʹ��
	BYTE                bReserved[3];                                           // �����ֶ�
	int					nObjectTypeNum;											// ��Ӧ�������͸���
	char				szObjectTypes[MAX_OBJECT_LIST_SIZE][MAX_NAME_LEN];		// ��Ӧ���������б�
	int                 nPtzPresetId;											// ��̨Ԥ�õ���	0~65535
	CFG_ALARM_MSG_HANDLE stuEventHandler;										// ��������
	CFG_TIME_SECTION	stuTimeSection[WEEK_DAY_NUM][MAX_REC_TSECT_EX];			// �¼���Ӧʱ���
	
	int					nLane;													// ��λ��
	int                 nDelayTime;                                             // ��⵽������������ʼ�ϱ���ʱ��, ��λ���룬��Χ1~65535
	int					nDetectRegionPoint;										// ������򶥵���
	CFG_POLYGON			stuDetectRegion[MAX_POLYGON_NUM];						// �������
    int                 nPlateSensitivity;                                      // ���Ƽ��������(����ץ��)
    int                 nNoPlateSensitivity;                                    // ���Ƽ�������ȣ�����ץ�ģ�
    int                 nLightPlateSensitivity;                                 // ���Ƽ�������ȣ����Ƴ�λ״̬�ƣ�
    int                 nLightNoPlateSensitivity;                               // ���Ƽ�������ȣ����Ƴ�λ״̬�ƣ�
}CFG_TRAFFIC_PARKINGSPACEPARKING_INFO;

//�¼�����	EVENT_IVS_TRAFFIC_PARKINGSPACENOPARKING(��λ�޳��¼�)��Ӧ�Ĺ�������
typedef struct tagCFG_TRAFFIC_PARKINGSPACENOPARKING_INFO
{
	char				szRuleName[MAX_NAME_LEN];								// ��������,��ͬ����������
	bool				bRuleEnable;											// ����ʹ��
	BYTE                bReserved[3];                                           // �����ֶ�
	int					nObjectTypeNum;											// ��Ӧ�������͸���
	char				szObjectTypes[MAX_OBJECT_LIST_SIZE][MAX_NAME_LEN];		// ��Ӧ���������б�
	int                 nPtzPresetId;											// ��̨Ԥ�õ���	0~65535
	CFG_ALARM_MSG_HANDLE stuEventHandler;										// ��������
	CFG_TIME_SECTION	stuTimeSection[WEEK_DAY_NUM][MAX_REC_TSECT_EX];			// �¼���Ӧʱ���
	
	int					nLane;													// ��λ��
	int                 nDelayTime;                                             // ��⵽������������ʼ�ϱ���ʱ��, ��λ���룬��Χ1~65535
	int					nDetectRegionPoint;										// ������򶥵���
	CFG_POLYGON			stuDetectRegion[MAX_POLYGON_NUM];						// �������
    int                 nPlateSensitivity;                                      // ���Ƽ��������(����ץ��)
    int                 nNoPlateSensitivity;                                    // ���Ƽ�������ȣ�����ץ�ģ�
    int                 nLightPlateSensitivity;                                 // ���Ƽ�������ȣ����Ƴ�λ״̬�ƣ�
    int                 nLightNoPlateSensitivity;                               // ���Ƽ�������ȣ����Ƴ�λ״̬�ƣ�
}CFG_TRAFFIC_PARKINGSPACENOPARKING_INFO;

//�¼�����EVENT_IVS_TRAFFIC_PARKINGSPACEOVERLINE(��λѹ���¼�)��Ӧ�Ĺ�������
typedef struct tagCFG_TRAFFIC_PARKINGSPACEOVERLINE_INFO
{
	char				szRuleName[MAX_NAME_LEN];								// ��������,��ͬ����������
	bool				bRuleEnable;											// ����ʹ��
	bool                bAllowParkingSpaceParkingSnap;                          // �Ƿ�����λ�г��ϴ�ץ�ģ�true-��ʾ����(Ĭ�ϲ�����)
	BYTE                byReserved[2];                                           // �����ֶ�
	int					nObjectTypeNum;											// ��Ӧ�������͸���
	char				szObjectTypes[MAX_OBJECT_LIST_SIZE][MAX_NAME_LEN];		// ��Ӧ���������б�
	int                 nPtzPresetId;											// ��̨Ԥ�õ���	0~65535
	CFG_ALARM_MSG_HANDLE stuEventHandler;										// ��������
	CFG_TIME_SECTION	stuTimeSection[WEEK_DAY_NUM][MAX_REC_TSECT_EX];			// �¼���Ӧʱ���
	
	int					nLane;													// ��λ��
}CFG_TRAFFIC_PARKINGSPACEOVERLINE_INFO;

//�¼����� EVENT_IVS_TRAFFIC_PEDESTRAIN (��ͨ�����¼�)��Ӧ�Ĺ�������
typedef struct tagCFG_TRAFFIC_PEDESTRAIN_INFO
{
    char                szRuleName[MAX_NAME_LEN];                               // ��������,��ͬ����������
    bool                bRuleEnable;                                            // ����ʹ��
    BYTE                bReserved[3];                                           // �����ֶ�
    int                 nObjectTypeNum;                                         // ��Ӧ�������͸���
    char                szObjectTypes[MAX_OBJECT_LIST_SIZE][MAX_NAME_LEN];      // ��Ӧ���������б�
    int                 nPtzPresetId;                                           // ��̨Ԥ�õ��� 0~65535
    CFG_ALARM_MSG_HANDLE stuEventHandler;                                       // ��������
    CFG_TIME_SECTION    stuTimeSection[WEEK_DAY_NUM][MAX_REC_TSECT_EX];         // �¼���Ӧʱ���
    
    int                 nLane;                                                  // �������
    int                 nDetectRegionPoint;                                     // �����������
    CFG_POLYGON         stuDetectRegion[MAX_POLYGON_NUM];                       // �����
    int                 nMinDuration;                                           // ��̴���ʱ�䣬��λ����
}CFG_TRAFFIC_PEDESTRAIN_INFO;

//�¼����� EVENT_IVS_TRAFFIC_THROW (��ͨ�����¼�)��Ӧ�Ĺ�������
typedef struct tagCFG_TRAFFIC_THROW_INFO
{
    char                szRuleName[MAX_NAME_LEN];                               // ��������,��ͬ����������
    bool                bRuleEnable;                                            // ����ʹ��
    BYTE                bReserved[3];                                           // �����ֶ�
    int                 nObjectTypeNum;                                         // ��Ӧ�������͸���
    char                szObjectTypes[MAX_OBJECT_LIST_SIZE][MAX_NAME_LEN];      // ��Ӧ���������б�
    int                 nPtzPresetId;                                           // ��̨Ԥ�õ��� 0~65535
    CFG_ALARM_MSG_HANDLE stuEventHandler;                                       // ��������
    CFG_TIME_SECTION    stuTimeSection[WEEK_DAY_NUM][MAX_REC_TSECT_EX];         // �¼���Ӧʱ���
    
    int                 nLane;                                                  // �������
    int                 nDetectRegionPoint;                                     // �����������
    CFG_POLYGON         stuDetectRegion[MAX_POLYGON_NUM];                       // �����
    int                 nMinDuration;                                           // ��̴���ʱ�䣬��λ����
}CFG_TRAFFIC_THROW_INFO;

// �¼����� EVENT_IVS_TRAFFIC_BACKING (Υ�µ����¼�)��Ӧ�Ĺ�������
typedef struct tagCFG_TRAFFIC_BACKING_INFO
{
    char				szRuleName[MAX_NAME_LEN];								// ��������,��ͬ����������
    bool				bRuleEnable;											// ����ʹ��
    BYTE                bSensitivity;                                           // �����ȣ�ȡֵ1-10����ֵԽ�����������Խ��
    bool                bTrackEnable;                                           // ��������ʹ��,�԰���,���ֵ��������
    BYTE                bReserved;                                              // �����ֶ�
    int					nObjectTypeNum;											// ��Ӧ�������͸���
    char				szObjectTypes[MAX_OBJECT_LIST_SIZE][MAX_NAME_LEN];		// ��Ӧ���������б�
    int                 nPtzPresetId;											// ��̨Ԥ�õ���	0~65535
    int					nLane;													// ������� 
    CFG_ALARM_MSG_HANDLE stuEventHandler;										// ��������
	CFG_TIME_SECTION	stuTimeSection[WEEK_DAY_NUM][MAX_REC_TSECT_EX];			// �¼���Ӧʱ���

    int                 nDetectRegionPoint;                                     // �����������
    CFG_POLYGON         stuDetectRegion[MAX_POLYGON_NUM];                       // �����
    int                 nDirectionLinPoint;                                     // ������ʻ���򶥵���
    CFG_POLYLINE		stuDirectionLine[MAX_POLYLINE_NUM];						// ������ʻ���򣬵�һ��������㣬���һ�������յ�

}CFG_TRAFFIC_BACKING_INFO;

// �¼����� EVENT_IVS_TRAFFIC_VEHICLEINBUSROUTE(ռ�ù��������¼�)��Ӧ��������
typedef struct tagCFG_TRAFFIC_VEHICLEINBUSROUTE_INFO
{
    char				szRuleName[MAX_NAME_LEN];								// ��������,��ͬ����������
    bool				bRuleEnable;											// ����ʹ��
    BYTE                byCarType;                                              // ץ�ĳ������ͣ�0���ǻ��Ƴ� 1�����Ƴ� 2���Ǵ�ͳ�
    BYTE                bSensitivity;                                           // �����ȣ�ȡֵ1-10����ֵԽ�����������Խ��
    BYTE                bEnclosure;                                             // ����Χ������, 1-�� 0-�ر�
    bool                bTrackEnable;                                           // ��������ʹ��,�԰���,���ֵ��������
    int					nObjectTypeNum;											// ��Ӧ�������͸���
    char				szObjectTypes[MAX_OBJECT_LIST_SIZE][MAX_NAME_LEN];		// ��Ӧ���������б�
    int                 nPtzPresetId;											// ��̨Ԥ�õ���	0~65535
    int					nLane;													// ������� 
    int                 nDetectRegionPoint;                                     // �����������
    CFG_POLYGON         stuDetectRegion[MAX_POLYGON_NUM];                       // �����
    CFG_ALARM_MSG_HANDLE stuEventHandler;										// ��������
	CFG_TIME_SECTION	stuTimeSection[WEEK_DAY_NUM][MAX_REC_TSECT_EX];			// �¼���Ӧʱ���
}CFG_TRAFFIC_VEHICLEINBUSROUTE_INFO;

typedef enum tagCFG_SAFEBELT_SEAT
{
    CSS_UNKNOW ,
    CSS_MAIN_SEAT     ,         //����ʻ��λ
    CSS_SLAVE_SEAT    ,         //����ʻ��λ
}CFG_SAFEBELT_SEAT;

// �¼����� EVENT_IVS_TRAFFIC_WITHOUT_SAFEBELT(��ͨδϵ��ȫ���¼�)��Ӧ��������
typedef struct tagCFG_TRAFFIC_WITHOUT_SAFEBELT
{
    char                szRuleName[MAX_NAME_LEN];                               // ��������,��ͬ����������
    bool                bRuleEnable;                                            // ����ʹ��
    BYTE                bReserved[3];                                           // �����ֶ�
    int                 nObjectTypeNum;                                         // ��Ӧ�������͸���
    char                szObjectTypes[MAX_OBJECT_LIST_SIZE][MAX_NAME_LEN];      // ��Ӧ���������б�
    int                 nPtzPresetId;                                           // ��̨Ԥ�õ��� 0~65535
    CFG_ALARM_MSG_HANDLE stuEventHandler;                                       // ��������
    CFG_TIME_SECTION    stuTimeSection[WEEK_DAY_NUM][MAX_REC_TSECT_EX];         // �¼���Ӧʱ���

    int                 nLane;                                                  // �������
    int                 nSeatNum;                                               // ��ȡ����λ��
    CFG_SAFEBELT_SEAT   emSnapSeat[MAX_SEAT_NUM];                               // ��Ҫץ�ĵļ�ʻ����λ
}CFG_TRAFFIC_WITHOUT_SAFEBELT;

// �¼����� EVENT_IVS_GETOUTBEDDETECTION(�������´��¼�)��Ӧ��������
typedef struct tagCFG_IVS_GETOUTBEDDETECTION_INFO
{
    char				szRuleName[MAX_NAME_LEN];								// ��������,��ͬ����������
    bool				bRuleEnable;											// ����ʹ��
    BYTE                bReserved[3];                                           // �����ֶ�
    int					nObjectTypeNum;											// ��Ӧ�������͸���
    char				szObjectTypes[MAX_OBJECT_LIST_SIZE][MAX_NAME_LEN];		// ��Ӧ���������б�
    int                 nPtzPresetId;											// ��̨Ԥ�õ���	0~65535
    CFG_ALARM_MSG_HANDLE stuEventHandler;										// ��������
    CFG_TIME_SECTION	stuTimeSection[WEEK_DAY_NUM][MAX_REC_TSECT_EX];			// �¼���Ӧʱ���

    int					nDetectRegionPoint;										// �����������
    CFG_POLYGON			stuDetectRegion[MAX_POLYGON_NUM];						// �����,��������ͣ��������ÿ������������һ����[0,8192)����
    unsigned int        nMinDuration;                                           // ��̳���ʱ��,��λ����
    unsigned int        nReportInterval;                                        // ����ʱ����,��λ����
    int                 nAssisDectLinePointNum;                                 // ��������ߵ���
    CFG_POLYLINE        stuAssisDectLine[MAX_POLYLINE_NUM];                     // ���������
}CFG_IVS_GETOUTBEDDETECTION_INFO;

// �¼����� EVENT_IVS_PATROLDETECTION(Ѳ�߼���¼�)��Ӧ��������
typedef struct tagCFG_IVS_PATROLDETECTION_INFO
{
    char				szRuleName[MAX_NAME_LEN];								// ��������,��ͬ����������
    bool				bRuleEnable;											// ����ʹ��
    BYTE                bReserved[3];                                           // �����ֶ�
    int					nObjectTypeNum;											// ��Ӧ�������͸���
    char				szObjectTypes[MAX_OBJECT_LIST_SIZE][MAX_NAME_LEN];		// ��Ӧ���������б�
    int                 nPtzPresetId;											// ��̨Ԥ�õ���	0~65535
    CFG_ALARM_MSG_HANDLE stuEventHandler;										// ��������
    CFG_TIME_SECTION	stuTimeSection[WEEK_DAY_NUM][MAX_REC_TSECT_EX];			// �¼���Ӧʱ���
    
    int					nDetectRegionPoint;										// ������������
    CFG_POLYGON			stuDetectRegion[MAX_POLYGON_NUM];						// ������,��������ͣ��������ÿ������������һ����[0,8192)����
    unsigned int        nMinDuration;                                           // ��̳���ʱ��,��λ����
    unsigned int        nReportInterval;                                        // ����ʱ����,��λ����
    unsigned int        nPatrolTime;                                            // Ѳ��ʱ��,��λ����
	BOOL                bSizeFileter;                                           // �����ض��ĳߴ�������Ƿ���Ч
	CFG_SIZEFILTER_INFO stuSizeFileter;                                         // �����ض��ĳߴ������
}CFG_IVS_PATROLDETECTION_INFO;

// �¼����� EVENT_IVS_ONDUTYDETECTION(վ�ڼ���¼�)��Ӧ��������
typedef struct tagCFG_IVS_ONDUTYDETECTION_INFO
{
    char				szRuleName[MAX_NAME_LEN];								// ��������,��ͬ����������
    bool				bRuleEnable;											// ����ʹ��
    BYTE                bReserved[3];                                           // �����ֶ�
    int					nObjectTypeNum;											// ��Ӧ�������͸���
    char				szObjectTypes[MAX_OBJECT_LIST_SIZE][MAX_NAME_LEN];		// ��Ӧ���������б�
    int                 nPtzPresetId;											// ��̨Ԥ�õ���	0~65535
    CFG_ALARM_MSG_HANDLE stuEventHandler;										// ��������
    CFG_TIME_SECTION	stuTimeSection[WEEK_DAY_NUM][MAX_REC_TSECT_EX];			// �¼���Ӧʱ���
    
    int					nDetectRegionPoint;										// ������������
    CFG_POLYGON			stuDetectRegion[MAX_POLYGON_NUM];						// ������,��������ͣ��������ÿ������������һ����[0,8192)����
    unsigned int        nMinDuration;                                           // ��̳���ʱ��,��λ����
    unsigned int        nReportInterval;                                        // ����ʱ����,��λ����
    int                 nTemplateRegionPointPairNum;                            // ģ��������Ϣ��Ը���
    CFG_POLYGON         stuTemplateRegion[MAX_TEMPLATEREGION_NUM][POINT_PAIR_NUM];  // ģ��������Ϣ,������Ӧ��ģ��ͼƬ����
}CFG_IVS_ONDUTYDETECTION_INFO;

// �¼����� EVENT_IVS_TRAFFIC_DRIVER_SMOKING(��ʻԱ�����¼�)��Ӧ��������
typedef struct tagCFG_TRAFFIC_DRIVER_SMOKING
{
    char                szRuleName[MAX_NAME_LEN];                               // ��������,��ͬ����������
    bool                bRuleEnable;                                            // ����ʹ��
    BYTE                bReserved[3];                                           // �����ֶ�
    int                 nObjectTypeNum;                                         // ��Ӧ�������͸���
    char                szObjectTypes[MAX_OBJECT_LIST_SIZE][MAX_NAME_LEN];      // ��Ӧ���������б�
    int                 nPtzPresetId;                                           // ��̨Ԥ�õ��� 0~65535
    CFG_ALARM_MSG_HANDLE stuEventHandler;                                       // ��������
    CFG_TIME_SECTION    stuTimeSection[WEEK_DAY_NUM][MAX_REC_TSECT_EX];         // �¼���Ӧʱ���
    
    int                 nLane;                                                  // �������
}CFG_TRAFFIC_DRIVER_SMOKING;

// �¼����� EVNET_IVS_TRAFFIC_DRIVER_CALLING(��ʻԱ��绰�¼�)��Ӧ��������
typedef struct tagCFG_TRAFFIC_DRIVER_CALLING
{
    char                szRuleName[MAX_NAME_LEN];                               // ��������,��ͬ����������
    bool                bRuleEnable;                                            // ����ʹ��
    BYTE                bReserved[3];                                           // �����ֶ�
    int                 nObjectTypeNum;                                         // ��Ӧ�������͸���
    char                szObjectTypes[MAX_OBJECT_LIST_SIZE][MAX_NAME_LEN];      // ��Ӧ���������б�
    int                 nPtzPresetId;                                           // ��̨Ԥ�õ��� 0~65535
    CFG_ALARM_MSG_HANDLE stuEventHandler;                                       // ��������
    CFG_TIME_SECTION    stuTimeSection[WEEK_DAY_NUM][MAX_REC_TSECT_EX];         // �¼���Ӧʱ���
    
    int                 nLane;                                                  // �������
}CFG_TRAFFIC_DRIVER_CALLING;

// �¼����� EVENT_IVS_TRAFFIC_PASSNOTINORDER(δ���涨����ͨ��)��Ӧ��������
typedef struct tagCFG_TRAFFIC_PASSNOTINORDER_INFO
{
    char                szRuleName[MAX_NAME_LEN];                               // ��������,��ͬ����������
    bool                bRuleEnable;                                            // ����ʹ��
    BYTE                bReserved[3];                                           // �����ֶ�
    int                 nObjectTypeNum;                                         // ��Ӧ�������͸���
    char                szObjectTypes[MAX_OBJECT_LIST_SIZE][MAX_NAME_LEN];      // ��Ӧ���������б�
    int                 nPtzPresetId;                                           // ��̨Ԥ�õ��� 0~65535
    CFG_ALARM_MSG_HANDLE stuEventHandler;                                       // ��������
    CFG_TIME_SECTION    stuTimeSection[WEEK_DAY_NUM][MAX_REC_TSECT_EX];         // �¼���Ӧʱ���
    
    int                 nLane;                                                  // �������
    int                 nFollowTime;                                            // ����ʱ����ֵ,���ڴ�ֵ����Υ�� ��λ�����루ms��
}CFG_TRAFFIC_PASSNOTINORDER_INFO;

// �¼����� EVENT_IVS_HEATMAP/EVENT_IVS_HEATMAP_PLAN(�ȶ�ͼ/�ȶ�ͼ�ƻ�)��Ӧ��������
typedef struct tagCFG_IVS_HEATMAP_INFO
{
    char				szRuleName[MAX_NAME_LEN];								// ��������,��ͬ����������
    bool				bRuleEnable;											// ����ʹ��
    BYTE                bReserved[3];                                           // �����ֶ�
    int					nObjectTypeNum;											// ��Ӧ�������͸���
    char				szObjectTypes[MAX_OBJECT_LIST_SIZE][MAX_NAME_LEN];		// ��Ӧ���������б�
    int                 nPtzPresetId;											// ��̨Ԥ�õ���	0~65535
    CFG_ALARM_MSG_HANDLE stuEventHandler;										// ��������
    CFG_TIME_SECTION	stuTimeSection[WEEK_DAY_NUM][MAX_REC_TSECT_EX];			// �¼���Ӧʱ���
    
    int					nDetectRegionPoint;										// ������������
    CFG_POLYGON			stuDetectRegion[MAX_POLYGON_NUM];						// ������,��������ͣ��������ÿ������������һ����[0,8192)����
    BOOL                bSizeFileter;                                           // �����ض��ĳߴ�������Ƿ���Ч
	CFG_SIZEFILTER_INFO stuSizeFileter;                                         // �����ض��ĳߴ������
    UINT                nPlanID;                                                // �ƻ�ID,�������Ч,��1��ʼ (�ȶ�ͼͳһ֮ǰʹ�ã�ͳһ֮��ʹ��nPtzPresetId)
}CFG_IVS_HEATMAP_INFO;

typedef struct tagCFG_REGION_FILTER
{
	CFG_RECT			stuMaxRect;												// �����ͷ���� 
	CFG_RECT			stuMinRect;												// ��С��ͷ����
}CFG_REGION_FILTER;

// �¼����� EVENT_IVS_STANDUPDETECTION ��������� ��Ӧ��������
typedef struct tagCFG_IVS_STANDUPDETECTION_INFO
{
    char				szRuleName[MAX_NAME_LEN];								// ��������,��ͬ����������
    bool				bRuleEnable;											// ����ʹ��
    BYTE                bReserved[3];                                           // �����ֶ�
    int					nObjectTypeNum;											// ��Ӧ�������͸���
    char				szObjectTypes[MAX_OBJECT_LIST_SIZE][MAX_NAME_LEN];		// ��Ӧ���������б�
    int                 nPtzPresetId;											// ��̨Ԥ�õ���	0~65535
    CFG_ALARM_MSG_HANDLE stuEventHandler;										// ��������
    CFG_TIME_SECTION	stuTimeSection[WEEK_DAY_NUM][MAX_REC_TSECT_EX];			// �¼���Ӧʱ���
									
	CFG_REGION			stuHumanRegion;											// ���������	
	int					nStandUpDirectionNum;									// �������򶥵���
	CFG_POLYLINE		stuStandUpDirection[MAX_POLYLINE_NUM];					// ��������, ��һ��Ϊ���, �ڶ���Ϊ�յ�, ǰ������Ч
	CFG_REGION_FILTER	stuRegionFilter;										// ������
}CFG_IVS_STANDUPDETECTION_INFO;

// �¼����� EVENT_IVS_SHOOTINGSCORERECOGNITION �������¼� ��Ӧ��������
typedef struct tagCFG_IVS_SHOOTINGSCORERECOGNITION_INFO
{
	char				szRuleName[MAX_NAME_LEN];								// ��������,��ͬ����������
	bool				bRuleEnable;											// ����ʹ��
	BYTE                bReserved[3];                                           // �����ֶ�
	int					nObjectTypeNum;											// ��Ӧ�������͸���
	char				szObjectTypes[MAX_OBJECT_LIST_SIZE][MAX_NAME_LEN];		// ��Ӧ���������б�
	int                 nPtzPresetId;											// ��̨Ԥ�õ���	0~65535
	CFG_ALARM_MSG_HANDLE stuEventHandler;										// ��������
	CFG_TIME_SECTION	stuTimeSection[WEEK_DAY_NUM][MAX_REC_TSECT_EX];			// �¼���Ӧʱ���

	CFG_REGION			stuDetectRegion;										// �������	�μ�����������DetectRegion
	int					nRound;													// �������ڼ�1�󵯿�������´�1��ʼ����
	int					nSensitivity;											// ������
	int                 nCaliber;											    // ���׿ھ���0:5mm�ھ�, 1:7~8mm�ھ�, 2:9mm�ھ�
}CFG_IVS_SHOOTINGSCORERECOGNITION_INFO;

// �¼�����EVENT_IVS_VEHICLEANALYSE(��������������)��Ӧ��������
typedef struct tagCFG_IVS_VEHICLEANALYSE_INFO
{
	char                szRuleName[MAX_NAME_LEN];                               // ��������,��ͬ����������
    bool                bRuleEnable;                                            // ����ʹ��
	BYTE                bReserved[3];                                           // �����ֶ�
    int                 nObjectTypeNum;                                         // ��Ӧ�������͸���
    char                szObjectTypes[MAX_OBJECT_LIST_SIZE][MAX_NAME_LEN];      // ��Ӧ���������б�
	int                 nPtzPresetId;											// ��̨Ԥ�õ���	0~65535
	CFG_ALARM_MSG_HANDLE stuEventHandler;									    // ��������
	CFG_TIME_SECTION    stuTimeSection[WEEK_DAY_NUM][MAX_REC_TSECT_EX];		    // �¼���Ӧʱ���
	  
	int					nDetectRegionPoint;										// ������������
	CFG_POLYGON			stuDetectRegion[MAX_POLYGON_NUM];						// ������
	BOOL                bSizeFileter;                                           // �����ض��ĳߴ�������Ƿ���Ч
	CFG_SIZEFILTER_INFO stuSizeFileter;                                         // �����ض��ĳߴ������
	
} CFG_IVS_VEHICLEANALYSE_INFO;

// �¼�����EVENT_IVS_LETRACK(�򵥸����¼�)��Ӧ��������
typedef struct tagCFG_IVS_LETRACK_INFO
{
	char				szRuleName[MAX_NAME_LEN];								// ��������,��ͬ����������
	BOOL				bRuleEnable;											// ����ʹ��
	int					nObjectTypeNum;											// ��Ӧ�������͸���
	char				szObjectTypes[MAX_OBJECT_LIST_SIZE][MAX_NAME_LEN];		// ��Ӧ���������б�
	int                 nPtzPresetId;											// ��̨Ԥ�õ���	0~65535
	CFG_ALARM_MSG_HANDLE stuEventHandler;										// ��������
	CFG_TIME_SECTION	stuTimeSection[WEEK_DAY_NUM][MAX_REC_TSECT_EX];			// �¼���Ӧʱ���
	BYTE                bReserved[512]; 										// �����ֽ�
} CFG_IVS_LETRACK_INFO;

// �¼�����EVENT_IVS_MAN_STAND_DETECTION(�����Ӿ�վ���¼�)
// EVENT_IVS_NEAR_DISTANCE_DETECTION(������Ӵ��¼�)��Ӧ�Ĺ�������
typedef struct tagCFG_STEREO_VISION_INFO
{
	// ��Ϣ
	char				szRuleName[MAX_NAME_LEN];								// ��������,��ͬ����������
	BOOL				bRuleEnable;											// ����ʹ��
    BYTE                bTrackEnable;                                           // ��������ʹ��,���԰��ߣ����ֹ�����Ч
	BYTE                bReserved1[3];                                          // �����ֶ� 
	int					nObjectTypeNum;											// ��Ӧ�������͸���
	char				szObjectTypes[MAX_OBJECT_LIST_SIZE][MAX_NAME_LEN];		// ��Ӧ���������б�
	CFG_ALARM_MSG_HANDLE stuEventHandler;										// ��������
	CFG_TIME_SECTION	stuTimeSection[WEEK_DAY_NUM][MAX_REC_TSECT_EX];			// �¼���Ӧʱ���
	int                 nPtzPresetId;											// ��̨Ԥ�õ���	0~65535

	int					nDetectLinePoint;										// ������򶥵���
	CFG_POLYLINE		stuDetectLine[MAX_POLYLINE_NUM];						// �������
	int					nDetectRegionPoint;										// ������򶥵���
	CFG_POLYGON			stuDetectRegion[MAX_POLYGON_NUM];						// �����������
	int					nThreshold;												// �쳣��ֵ
	int					nDetectType;											// �����쳣��ⷽʽ������������������ͳ�ƣ�
																				// 0-С�ڵ�����ֵ���� 1-���ڵ�����ֵ����
	int					nSensitivity;											// ������,[0-100]
	int					nMaxHeight;												// �����߶�,��λcm
	int 				nMinHeight;												// ��С���߶�,��λcm
	BYTE                bReserved[512]; 										// �����ֽ�
} CFG_STEREO_VISION_INFO;

// �¼�����EVENT_IVS_MAN_NUM_DETECTION(�����Ӿ�����������ͳ���¼�)��Ӧ�Ĺ�������
typedef struct tagCFG_IVS_MAN_NUM_DETECTION_INFO
{
	// ��Ϣ
	char				szRuleName[MAX_NAME_LEN];								// ��������,��ͬ����������
	BOOL				bRuleEnable;											// ����ʹ��
    BYTE                bTrackEnable;                                           // ��������ʹ��,���԰��ߣ����ֹ�����Ч
	BYTE                bReserved1[3];                                          // �����ֶ� 
	int					nObjectTypeNum;											// ��Ӧ�������͸���
	char				szObjectTypes[MAX_OBJECT_LIST_SIZE][MAX_NAME_LEN];		// ��Ӧ���������б�
	CFG_ALARM_MSG_HANDLE stuEventHandler;										// ��������
	CFG_TIME_SECTION	stuTimeSection[WEEK_DAY_NUM][MAX_REC_TSECT_EX];			// �¼���Ӧʱ���
	int                 nPtzPresetId;											// ��̨Ԥ�õ���	0~65535

	int					nDetectRegionPoint;										// ������򶥵���
	CFG_POLYGON			stuDetectRegion[MAX_POLYGON_NUM];						// �����������
	int					nThreshold;												// �쳣������ֵ
	int					nDetectType;											// �����쳣��ⷽʽ
																				// 0-С�ڵ�����ֵ���� 1-���ڵ�����ֵ����
	int					nSensitivity;											// ������,[1-10]
	int					nMaxHeight;												// �����߶�,��λcm
	int 				nMinHeight;												// ��С���߶�,��λcm
	BOOL				bStayDetectEnable;										// �Ƿ���������ⱨ������, ��Ӧ����ΪStayDetection��
	int 				nStayMinDuration;										// �����������ʱ�䣬��λ��[2, 65535], ����������ⱨ��ʱ������
	BOOL				bManNumAlarmEnable;										// �Ƿ��������쳣��������, Ĭ��ΪTRUE����Ӧ����ΪManNumDetection
	UINT				nAreaID;												// ����ID��һ��Ԥ�õ��Ӧ�������ID��
	UINT				nPlanId;												// �ƻ�ID����Χ 1~MaxNumberStatPlan
	BYTE                bReserved[504]; 										// �����ֽ�
} CFG_IVS_MAN_NUM_DETECTION_INFO;

// �¼�����EVENT_IVS_STEREO_NUMBERSTAT(�����Ӿ�����ͳ���¼�)��Ӧ�Ĺ�������
typedef struct tagCFG_IVS_STEREO_NUMBERSTAT_INFO
{
	// ��Ϣ
	char				szRuleName[MAX_NAME_LEN];								// ��������,��ͬ����������
	BOOL				bRuleEnable;											// ����ʹ��
    BYTE                bTrackEnable;                                           // ��������ʹ��,���԰��ߣ����ֹ�����Ч
	BYTE                bReserved1[3];                                          // �����ֶ� 
	int					nObjectTypeNum;											// ��Ӧ�������͸���
	char				szObjectTypes[MAX_OBJECT_LIST_SIZE][MAX_NAME_LEN];		// ��Ӧ���������б�
	CFG_ALARM_MSG_HANDLE stuEventHandler;										// ��������
	CFG_TIME_SECTION	stuTimeSection[WEEK_DAY_NUM][MAX_REC_TSECT_EX];			// �¼���Ӧʱ���
	int                 nPtzPresetId;											// ��̨Ԥ�õ���	0~65535

	int					nDetectLinePoint;										// ����߶�����
	CFG_POLYLINE		stuDetectLine[MAX_POLYLINE_NUM];						// �����
	int					nDetectRegionPoint;										// ������򶥵���
	CFG_POLYGON			stuDetectRegion[MAX_POLYGON_NUM];						// �����������
	int					nSensitivity;											// ������, 1-10
	int					nMaxHeight;												// �����߶�,��λcm
	int 				nMinHeight;												// ��С���߶�,��λcm
	int					nEnterThreshold;										// ��������������ֵ, 0��ʾ������
	int					nExitThreshold;											// �뿪����������ֵ, 0��ʾ������
	int					nInsideThreshold;										// ��������������ֵ, 0��ʾ������
	char				szDirection[CFG_COMMON_STRING_8];						// ��ⷽ��
	BYTE                bReserved[512]; 										// �����ֽ�
} CFG_IVS_STEREO_NUMBERSTAT_INFO;

// ��������
typedef enum tagEM_CFG_ACTION_TYPE
{
    EM_CFG_ACTION_TYPE_UNKNOWN,		        // δ֪
    EM_CFG_ACTION_TYPE_DRASTIC_MOTION,		// �����˶�
    EM_CFG_ACTION_TYPE_SINGLE_FIGHT,		// ���˴���
    EM_CFG_ACTION_TYPE_MULTI_FIGHT,		    // ���˴��
}EM_CFG_ACTION_TYPE;

// �¼�����EVENT_IVS_STEREO_FIGHTDETECTION(������Ϊ�������/�����˶����)��Ӧ�Ĺ�������
typedef struct tagCFG_IVS_STEREO_FIGHTDETECTION_INFO
{
	// ��Ϣ
	char				szRuleName[MAX_NAME_LEN];								// ��������,��ͬ����������
	BOOL				bRuleEnable;											// ����ʹ��
    BYTE                bTrackEnable;                                           // ��������ʹ��,���԰��ߣ����ֹ�����Ч
	BYTE                bReserved1[3];                                          // �����ֶ� 
	int					nObjectTypeNum;											// ��Ӧ�������͸���
	char				szObjectTypes[MAX_OBJECT_LIST_SIZE][MAX_NAME_LEN];		// ��Ӧ���������б�
	CFG_ALARM_MSG_HANDLE stuEventHandler;										// ��������
	CFG_TIME_SECTION	stuTimeSection[WEEK_DAY_NUM][MAX_REC_TSECT_EX];			// �¼���Ӧʱ���
	int                 nPtzPresetId;											// ��̨Ԥ�õ���	0~65535

	int					nDetectRegionPoint;										// ������򶥵���
	CFG_POLYGON			stuDetectRegion[MAX_POLYGON_NUM];						// �������
	int					nSensitivity;											// ������, 1-10, Ĭ��ֵ5
	int					nMaxHeight;												// �����߶�,��λcm
	int 				nMinHeight;												// ��С���߶�,��λcm
    EM_CFG_ACTION_TYPE  emActionType;                                           // ��������       
	BYTE                bReserved[508]; 										// �����ֽ�
} CFG_IVS_STEREO_FIGHTDETECTION_INFO;

// �¼�����EVENT_IVS_STEREO_STEREOFALLDETECTION(������Ϊ�����������)��Ӧ�Ĺ�������
typedef struct tagCFG_IVS_STEREO_STEREOFALLDETECTION_INFO
{
	// ��Ϣ
	char				szRuleName[MAX_NAME_LEN];								// ��������,��ͬ����������
	BOOL				bRuleEnable;											// ����ʹ��
    BYTE                bTrackEnable;                                           // ��������ʹ��,���԰��ߣ����ֹ�����Ч
	BYTE                bReserved1[3];                                          // �����ֶ� 
	int					nObjectTypeNum;											// ��Ӧ�������͸���
	char				szObjectTypes[MAX_OBJECT_LIST_SIZE][MAX_NAME_LEN];		// ��Ӧ���������б�
	CFG_ALARM_MSG_HANDLE stuEventHandler;										// ��������
	CFG_TIME_SECTION	stuTimeSection[WEEK_DAY_NUM][MAX_REC_TSECT_EX];			// �¼���Ӧʱ���
	int                 nPtzPresetId;											// ��̨Ԥ�õ���	0~65535

	int					nDetectRegionPoint;										// ������򶥵���
	CFG_POLYGON			stuDetectRegion[MAX_POLYGON_NUM];						// �������
	int					nMinDuration;											// ��̳���ʱ�䣬��λ����,���ڸ�ʱ�䲻����, Ĭ��1 ��Χ[1,60]
	int					nReportInterval;										// �����ظ�����ʱ��,Ϊ0���ظ�����, Ĭ��0 ��Χ[0,300]����λ����
	int					nSensitivity;											// ������, 1-10�� Ĭ��ֵ5
	int					nMaxHeight;												// �����߶�,��λcm
	int 				nMinHeight;												// ��С���߶�,��λcm
	BYTE                bReserved[512]; 										// �����ֽ�
} CFG_IVS_STEREO_STEREOFALLDETECTION_INFO;

// �¼�����EVENT_IVS_STEREO_STAYDETECTION(������Ϊ������Ա�������)��Ӧ�Ĺ�������
typedef struct tagCFG_IVS_STEREO_STAYDETECTION_INFO
{
	// ��Ϣ
	char				szRuleName[MAX_NAME_LEN];								// ��������,��ͬ����������
	BOOL				bRuleEnable;											// ����ʹ��
    BYTE                bTrackEnable;                                           // ��������ʹ��,���԰��ߣ����ֹ�����Ч
	BYTE                bReserved1[3];                                          // �����ֶ� 
	int					nObjectTypeNum;											// ��Ӧ�������͸���
	char				szObjectTypes[MAX_OBJECT_LIST_SIZE][MAX_NAME_LEN];		// ��Ӧ���������б�
	CFG_ALARM_MSG_HANDLE stuEventHandler;										// ��������
	CFG_TIME_SECTION	stuTimeSection[WEEK_DAY_NUM][MAX_REC_TSECT_EX];			// �¼���Ӧʱ���
	int                 nPtzPresetId;											// ��̨Ԥ�õ���	0~65535

	int					nDetectRegionPoint;										// ������򶥵���
	CFG_POLYGON			stuDetectRegion[MAX_POLYGON_NUM];						// �������
	int					nMinDuration;											// ����ʱ����ֵ����λ����,���ڸ�ʱ�䲻����, Ĭ��300, ��Χ[1,3600]
	int					nReportInterval;										// �����ظ�����ʱ��,Ϊ0���ظ�����, Ĭ��0 ��Χ[0,300]����λ����
	int					nSensitivity;											// ������, 1-10, Ĭ��5
	int					nMaxHeight;												// �����߶�,��λcm
	int 				nMinHeight;												// ��С���߶�,��λcm
	BYTE                bReserved[512]; 										// �����ֽ�
} CFG_IVS_STEREO_STAYDETECTION_INFO;

// �¼�����EVENT_IVS_STEREO_DISTANCE_DETECTION(������Ϊ��������쳣/��Ա�������)��Ӧ�Ĺ�������
typedef struct tagCFG_IVS_STEREO_DISTANCE_DETECTION_INFO
{
	// ��Ϣ
	char				szRuleName[MAX_NAME_LEN];								// ��������,��ͬ����������
	BOOL				bRuleEnable;											// ����ʹ��
    BYTE                bTrackEnable;                                           // ��������ʹ��,���԰��ߣ����ֹ�����Ч
	BYTE                bReserved1[3];                                          // �����ֶ� 
	int					nObjectTypeNum;											// ��Ӧ�������͸���
	char				szObjectTypes[MAX_OBJECT_LIST_SIZE][MAX_NAME_LEN];		// ��Ӧ���������б�
	CFG_ALARM_MSG_HANDLE stuEventHandler;										// ��������
	CFG_TIME_SECTION	stuTimeSection[WEEK_DAY_NUM][MAX_REC_TSECT_EX];			// �¼���Ӧʱ���
	int                 nPtzPresetId;											// ��̨Ԥ�õ���	0~65535

	int					nDetectRegionPoint;										// ������򶥵���
	CFG_POLYGON			stuDetectRegion[MAX_POLYGON_NUM];						// �������
	int					nMinDuration;											// ��̳���ʱ��, ��λ����,���ڸ�ʱ�䲻����, Ĭ��2, ��Χ[1,60]
	int					nReportInterval;										// �ظ�����ʱ��, ��λ����, Ϊ0���ظ�����, Ĭ��0, ��Χ[0,300]
	int					nSensitivity;											// ������, 1-10, Ĭ��5
	int 				nMinHeight;												// ��С���߶�,��λcm
	int					nMaxHeight;												// �����߶�,��λcm
	int					nDetectType;											// ���ģʽ, ȡֵ��0-������ֵ���� 1-С����ֵ����, Ĭ��Ϊ 1
	int					nDistance;												// �����ֵ, ��λ:cm, ȡֵ10-600��Ĭ��ֵ100
	BYTE                bReserved[512]; 										// �����ֽ�
} CFG_IVS_STEREO_DISTANCE_DETECTION_INFO;

// �¼�����EVENT_IVS_STEREO_MANNUM_DETECTION(������Ϊ���������쳣���)��Ӧ�Ĺ�������
typedef struct tagCFG_IVS_STEREO_MANNUM_DETECTION_INFO
{
	// ��Ϣ
	char				szRuleName[MAX_NAME_LEN];								// ��������,��ͬ����������
	BOOL				bRuleEnable;											// ����ʹ��
    BYTE                bTrackEnable;                                           // ��������ʹ��,���԰��ߣ����ֹ�����Ч
	BYTE                bReserved1[3];                                          // �����ֶ� 
	int					nObjectTypeNum;											// ��Ӧ�������͸���
	char				szObjectTypes[MAX_OBJECT_LIST_SIZE][MAX_NAME_LEN];		// ��Ӧ���������б�
	CFG_ALARM_MSG_HANDLE stuEventHandler;										// ��������
	CFG_TIME_SECTION	stuTimeSection[WEEK_DAY_NUM][MAX_REC_TSECT_EX];			// �¼���Ӧʱ���
	int                 nPtzPresetId;											// ��̨Ԥ�õ���	0~65535

	int					nDetectRegionPoint;										// ������򶥵���
	CFG_POLYGON			stuDetectRegion[MAX_POLYGON_NUM];						// �������
	int					nMinDuration;											// ��̳���ʱ��, ��λ����,���ڸ�ʱ�䲻����, Ĭ��2, ��Χ[1,60]
	int					nReportInterval;										// �ظ�����ʱ��, ��λ����, Ϊ0���ظ�����, Ĭ��0, ��Χ[0,300]
	int					nSensitivity;											// ������, 1-10, Ĭ��5
	int 				nMinHeight;												// ��С���߶�,��λcm
	int					nMaxHeight;												// �����߶�,��λcm
	int					nDetectType;											// ���ģʽ, ȡֵ��0-������ֵ���� 1-С����ֵ���� 2-��������ֵ���� 3-������ֵ����
	int					nThresholdPerson;										// ����������ֵ, ȡֵ0-10��Ĭ��ֵ2
	BYTE                bReserved[512]; 										// �����ֽ�
} CFG_IVS_STEREO_MANNUM_DETECTION_INFO;

// �¼�����EVENT_IVS_ROAD_CONSTRUCTION (��·ʩ������¼�) ��EVENT_IVS_ROAD_BLOCK(·�ϼ���¼�)��Ӧ�Ĺ�������
typedef struct tagCFG_ROAD_DETECTION_INFO
{
	// ��Ϣ
	char				szRuleName[MAX_NAME_LEN];								// ��������,��ͬ����������
	BOOL				bRuleEnable;											// ����ʹ��
	int					nObjectTypeNum;											// ��Ӧ�������͸���
	char				szObjectTypes[MAX_OBJECT_LIST_SIZE][MAX_NAME_LEN];		// ��Ӧ���������б�
	int                 nPtzPresetId;											// ��̨Ԥ�õ���	0~65535
	CFG_ALARM_MSG_HANDLE stuEventHandler;										// ��������
	CFG_TIME_SECTION	stuTimeSection[WEEK_DAY_NUM][MAX_REC_TSECT_EX];			// �¼���Ӧʱ���

	int					nDetectRegionPoint;										// ������򶥵���
	CFG_POLYGON			stuDetectRegion[MAX_POLYGON_NUM];						// �������
	int					nMinDuration;											// ��̳���ʱ��, ��λ��, 0-65535
	int					nSensitivity;											// ������, 1-10
} CFG_ROAD_DETECTION_INFO;

// ����ͳ������
typedef struct tagPEOPLE_STAT_REGIONS_INFO
{
	CFG_POLYGON			stuRegion[MAX_CROWD_DISTRI_MAP_REGION_POINT_NUM];		// ����������
	UINT				nPointNum;												// ������Ч����
	unsigned int		nPeopleNumLimit;										// ��������������,��λ:��
	char				szName[32];												// ��������
	UINT				nMinDuration;											// ��̳���ʱ�䣬��λ��, ��Χ[1, 600](CrowdPosture�����֧��)
	UINT				nReportInterval;										// �ظ������������λ��, ��Χ[0, 3600](CrowdPosture�����֧��)
	UINT				nSensitivity;											// ������, ��Χ[1, 10](CrowdPosture�����֧��)
	BYTE				byReserved[336];										// �����ֽ�
}PEOPLE_STAT_REGIONS_INFO;

// �¼�����EVENT_IVS_CROWDDETECTION(��Ⱥ�ܶȼ���¼�)��Ӧ�Ĺ�������
typedef struct tagCFG_CROWDDISTRIMAP_INFO
{
	char				szRuleName[MAX_NAME_LEN];								// ��������,��ͬ����������
	BOOL				bRuleEnable;											// ����ʹ��
	BYTE                bTrackEnable;                                           // ��������ʹ��,���԰��ߣ����ֹ�����Ч
	BYTE                bReserved1[3];                                          // �����ֶ� 
	int					nObjectTypeNum;											// ��Ӧ�������͸���
	char				szObjectTypes[MAX_OBJECT_LIST_SIZE][MAX_NAME_LEN];		// ��Ӧ���������б�
	CFG_ALARM_MSG_HANDLE stuEventHandler;										// ��������
	CFG_TIME_SECTION	stuTimeSection[WEEK_DAY_NUM][MAX_REC_TSECT_EX];			// �¼���Ӧʱ���
	int                 nPtzPresetId;											// ��̨Ԥ�õ���	0~65535
	
	BOOL				bGeneralAlarmEnable;									// ȫ����Ⱥ�ܶȳ��ޱ���ʹ��
	unsigned int		nCrowdDensity;											// ȫ����Ⱥ�ܶȱ�����ֵ,��λ:��/ƽ����
	BOOL				bRegionAlarmEnable;										// ������Ա�������ޱ���ʹ��
	int					nPeopleStatRegionsNum;									// ����ͳ���������
	PEOPLE_STAT_REGIONS_INFO  stuPeopleStatRegions[MAX_PEOPLESTATREGIONS_NUM];  // ����ͳ������
	BOOL				bSlaveTrackEnable;										// ǹ����������ʹ��
	UINT				nMinDuration;											// ��̳���ʱ��, ��λ��, ��Χ[1, 600](CrowdPosture�����֧��)
	UINT				nReportInterval;										// �ظ��������, ��λ��, ��Χ[0, 3600](CrowdPosture�����֧��)
	UINT				nSensitivity;											// ������, ��Χ[1, 10](CrowdPosture�����֧��)
	BYTE                bReserved[500]; 										// �����ֽ�
}CFG_CROWDDISTRIMAP_INFO;

// �¼�����EVENT_IVS_BANNER_DETECTION(������¼�)��Ӧ�Ĺ�������
typedef struct tagCFG_BANNER_DETECTION_INFO
{
	char				szRuleName[MAX_NAME_LEN];								// ��������,��ͬ����������
	BOOL				bRuleEnable;											// ����ʹ��
	int					nObjectTypeNum;											// ��Ӧ�������͸���
	char				szObjectTypes[MAX_OBJECT_LIST_SIZE][MAX_NAME_LEN];		// ��Ӧ���������б�
	CFG_ALARM_MSG_HANDLE stuEventHandler;										// ��������
	CFG_TIME_SECTION	stuTimeSection[WEEK_DAY_NUM][MAX_REC_TSECT_EX];			// �¼���Ӧʱ���
	int                 nPtzPresetId;											// ��̨Ԥ�õ���	0~65535
	
	int					nDetectRegionPoint;										// ������򶥵���
	CFG_POLYGON			stuDetectRegion[MAX_POLYGON_NUM];						// �������
	int					nMinDuration;											// ��̳���ʱ��, ��λ:�룬��Χ1-600, Ĭ��30
	int					nReportInterval;										// �ظ��������,��λ:��,��Χ0-600,Ĭ��30,Ϊ0��ʾ���ظ�
	int					nSensitivity;											// ���������,��Χ1-10
	BYTE                bReserved[512]; 										// �����ֽ�
} CFG_BANNER_DETECTION_INFO;

// �¼�����EVENT_IVS_NORMAL_FIGHTDETECTION(��ͨ��Ź�¼�)��Ӧ�Ĺ�������
typedef struct tagCFG_NORMAL_FIGHT_INFO 
{
	// ��Ϣ
	char				szRuleName[MAX_NAME_LEN];								// ��������,��ͬ����������
	bool				bRuleEnable;											// ����ʹ��
	int					nObjectTypeNum;											// ��Ӧ�������͸���
	char				szObjectTypes[MAX_OBJECT_LIST_SIZE][MAX_NAME_LEN];		// ��Ӧ���������б�
	CFG_ALARM_MSG_HANDLE stuEventHandler;										// ��������
	CFG_TIME_SECTION	stuTimeSection[WEEK_DAY_NUM][MAX_REC_TSECT_EX];			// �¼���Ӧʱ���
	int                 nPtzPresetId;											// ��̨Ԥ�õ���	0~65535
	
	int					nMinDuration;											// ��̳���ʱ��	��λ���룬��Χ1-600, Ĭ��ֵ30
	int					nDetectRegionPoint;										// ������򶥵���
	CFG_POLYGON			stuDetectRegion[MAX_POLYGON_NUM];						// �������
	int					nTrackDuration;											// ��������ʱ��, ��λ:��, ��Χ5-300
	int					nSensitivity;											// ������, 1-10
    CFG_EM_DETECTMODE_T emDetectMode;                                           // ���ģʽ
    BYTE                bReserved[512]; 										// �����ֽ�
} CFG_NORMAL_FIGHT_INFO;

// �¼�����EVENT_IVS_ELEVATOR_ABNORMAL(�綯���������쳣�¼�)��Ӧ�Ĺ�������
typedef struct tagCFG_ELEVATOR_ABNORMAL_INFO
{
	char				szRuleName[MAX_NAME_LEN];								// ��������,��ͬ����������
	BOOL				bRuleEnable;											// ����ʹ��
	BYTE                bTrackEnable;                                           // ��������ʹ��,���԰��ߣ����ֹ�����Ч
	BYTE                bReserved1[3];                                          // �����ֶ� 
	int					nDetectRegionPoint;										// �����������
	CFG_POLYGON			stuDetectRegion[MAX_POLYGON_NUM];						// �����
	int					nObjectTypeNum;											// ��Ӧ�������͸���
	char				szObjectTypes[MAX_OBJECT_LIST_SIZE][MAX_NAME_LEN];		// ��Ӧ���������б�
	CFG_ALARM_MSG_HANDLE stuEventHandler;										// ��������
	CFG_TIME_SECTION	stuTimeSection[WEEK_DAY_NUM][MAX_REC_TSECT_EX];			// �¼���Ӧʱ���
	int                 nPtzPresetId;											// ��̨Ԥ�õ���	0~65535
	
	int					nMinDuration;											// ��������ʱ����ֵ
	int					nReportInterval;										// ���������ظ��������������Ϊ0���ظ�����
	int					nSensitivity;											// ������, ȡֵ1-10, Ĭ��ֵ6
	CFG_POLYLINE		stuDetectLineBegin;										// �������з�������
	CFG_POLYLINE		stuDetectLineEnd;										// �������з�����յ�
	BYTE                bReserved[512]; 										// �����ֽ�
} CFG_ELEVATOR_ABNORMAL_INFO;

// �ǻ�������������
typedef enum tagEM_CFG_NONMOTOR_FEATURE
{
    EM_CFG_NONMOTOR_FEATURE_HELMET,                     //ͷ��
    EM_CFG_NONMOTOR_FEATURE_NUMOFCYCLING,               //��������
    EM_CFG_NONMOTOR_FEATURE_NONMOTORCOLOR,              //������ɫ
    EM_CFG_NONMOTOR_FEATURE_SEX,                        //�Ա�
    EM_CFG_NONMOTOR_FEATURE_AGE,                        //����
    EM_CFG_NONMOTOR_FEATURE_HAT,                        //ñ��
    EM_CFG_NONMOTOR_FEATURE_BAG,                        //����
    EM_CFG_NONMOTOR_FEATURE_CARRIERBAG,                 //�����
    EM_CFG_NONMOTOR_FEATURE_UMBRELLA,                   //��ɡ
    EM_CFG_NONMOTOR_FEATURE_UPPERBODYCOLOR,             //������ɫ
    EM_CFG_NONMOTOR_FEATURE_LOWERBODYCOLOR,             //������ɫ
    EM_CFG_NONMOTOR_FEATURE_UPCLOTHES,                  //��������
    EM_CFG_NONMOTOR_FEATURE_DOWNCLOTHES,                //��������
    EM_CFG_NONMOTOR_FEATURE_CATEGORY,                   //������
}EM_CFG_NONMOTOR_FEATURE;

#define	MAX_NONMOTOR_ATTRIBUTE_NUM              32                                      // ֧�ֵķǻ��������Ը���
// �¼�����EVENT_IVS_NONMOTORDETECT(�ǻ������¼�)��Ӧ�Ĺ�������
typedef struct tagCFG_NONMOTORDETECT_INFO
{
    // ͨ������
    char				        szRuleName[MAX_NAME_LEN];								// ��������,��ͬ����������
    BOOL				        bRuleEnable;											// ����ʹ��
    int					        nObjectTypeNum;											// ��Ӧ�������͸���
    char				        szObjectTypes[MAX_OBJECT_LIST_SIZE][MAX_NAME_LEN];		// ��Ӧ���������б�
    int                         nPtzPresetId;                                           // ��̨Ԥ�õ���	0~65535
    CFG_ALARM_MSG_HANDLE        stuEventHandler;										// ��������
    CFG_TIME_SECTION	        stuTimeSection[WEEK_DAY_NUM][MAX_REC_TSECT_EX];			// �¼���Ӧʱ���
    
    // ��������
    int					         nFeatureNum;								            // ֧�ֵķǻ��������Ը���
    EM_CFG_NONMOTOR_FEATURE      emFeatureList[MAX_NONMOTOR_ATTRIBUTE_NUM];             // ֧�ֵķǻ����������б�
    BYTE                         bReserved[512]; 							            // �����ֽ�
}CFG_NONMOTORDETECT_INFO;

// �¼�����EVENT_IVS_HUMANTRAIT(��Ա������)��Ӧ�Ĺ�������
typedef struct tagCFG_HUMANTRAIT_INFO
{
    // ͨ������
    char				        szRuleName[MAX_NAME_LEN];								// ��������,��ͬ����������
    BOOL				        bRuleEnable;											// ����ʹ��
    int					        nObjectTypeNum;											// ��Ӧ�������͸���
    char				        szObjectTypes[MAX_OBJECT_LIST_SIZE][MAX_NAME_LEN];		// ��Ӧ���������б�
    int                         nPtzPresetId;                                           // ��̨Ԥ�õ���	0~65535
    CFG_ALARM_MSG_HANDLE        stuEventHandler;										// ��������
    CFG_TIME_SECTION	        stuTimeSection[WEEK_DAY_NUM][MAX_REC_TSECT_EX];			// �¼���Ӧʱ���

    // ��������
    int                             nHumanFaceTypeNum;                                  // ֧�ֵ�����������͸���
    EM_VIDEO_ANALYSE_HUMANFACE_TYPE emHumanFaceType[MAX_HUMANFACE_LIST_SIZE];           // ֧�ֵ�������������б�

    int                         nMinDuration;                                           // ��̴���ʱ��,��λ����
    int                         nTriggerTargets;                                        // ������������������
    int                         nSensitivity;                                           // ������,Խ��Խ���׼��, 0-10
    BOOL                        bSizeFileter;                                           // �Ƿ�ʹ�óߴ������
    CFG_SIZEFILTER_INFO         stuSizeFileter;                                         // �����ض��ĳߴ������������жϾ���
    BOOL                        bFeatureEnable;                                         // �Ƿ�����������ʶ��
    BOOL                        bFeatureFilter;                                         // ���������Կ���ǰ���£��������ͼ������̫��Ƿ��ϱ�����  true-ͼ��̫��ϱ�����;false-ͼ��ܲ�Ҳ�ϱ�����(���ܻ�ǳ���׼��Ӱ���û�����)
    int                         nMinQuality;                                            // ����ͼƬ������ֵ,��bFeatureFilterһ��ʹ��

    int					        nFaceFeatureNum;										// ��Ҫ�����������Ը���
    EM_FACEFEATURE_TYPE         emFaceFeatureType[MAX_FEATURE_LIST_SIZE];				// �������������

    BYTE                        bReserved[512]; 							            // �����ֽ�
}CFG_HUMANTRAIT_INFO;

// �¼�����EVENT_IVS_VEHICLEDETECT(�������¼�)��Ӧ�Ĺ�������
typedef struct tagCFG_VEHICLEDETECT_INFO
{
    // ͨ������
    char				        szRuleName[MAX_NAME_LEN];								// ��������,��ͬ����������
    BOOL				        bRuleEnable;											// ����ʹ��
    int					        nObjectTypeNum;											// ��Ӧ�������͸���
    char				        szObjectTypes[MAX_OBJECT_LIST_SIZE][MAX_NAME_LEN];		// ��Ӧ���������б�
    int                         nPtzPresetId;                                           // ��̨Ԥ�õ���	0~65535
    CFG_ALARM_MSG_HANDLE        stuEventHandler;										// ��������
    CFG_TIME_SECTION	        stuTimeSection[WEEK_DAY_NUM][MAX_REC_TSECT_EX];			// �¼���Ӧʱ���

    // ��������
    int                         nSnapThreshold;                                         // ����ץ����ֵ,��һ���ٷ�������ʾ����������߶�ռ����ٷֱȴ��������ֵʱ�������������ץ�ġ�
    BYTE                        bReserved[512]; 							            // �����ֽ�
}CFG_VEHICLEDETECT_INFO;

// �¼�����EVENT_IVS_TUMBLE_DETECTION(�����¼�)��Ӧ�Ĺ�������
typedef struct tagCFG_TUMBLE_DETECTION_INFO
{
	char				szRuleName[MAX_NAME_LEN];								// ��������,��ͬ����������
	BOOL				bRuleEnable;											// ����ʹ��
	BYTE                bTrackEnable;                                           // ��������ʹ��,���԰��ߣ����ֹ�����Ч
	BYTE                bReserved1[3];                                          // �����ֶ� 
	int					nDetectRegionPoint;										// �����������
	CFG_POLYGON			stuDetectRegion[MAX_POLYGON_NUM];						// �����
	int					nObjectTypeNum;											// ��Ӧ�������͸���
	char				szObjectTypes[MAX_OBJECT_LIST_SIZE][MAX_NAME_LEN];		// ��Ӧ���������б�
	CFG_ALARM_MSG_HANDLE stuEventHandler;										// ��������
	CFG_TIME_SECTION	stuTimeSection[WEEK_DAY_NUM][MAX_REC_TSECT_EX];			// �¼���Ӧʱ���
	int                 nPtzPresetId;											// ��̨Ԥ�õ���	0~65535

	int					nMinDuration;											// �����״α���ʱ����ֵ�����ڸ�ʱ�䲻��������Χ[0,30]����λ����
	int					nReportInterval;										// �����ظ�����ʱ��,Ϊ0���ظ���������Χ[1,300]����λ����
	int					nMinHeight;												// ��С���߶ȡ���λ������
	int					nMaxHeight;												// �����߶ȡ���λ������
	int					nSensitivity;											// ������,��Χ[1,10]
	BYTE                byReserved[512]; 										// �����ֽ�
}CFG_TUMBLE_DETECTION_INFO;

//��ⷽ��
typedef enum tagEM_CFG_DIRECTION_TYPE
{
	EM_CFG_DIRECTION_UNKNOWN = 0,												// δ֪
	EM_CFG_DIRECTION_LEFT_TO_RIGHT,												// ��������
	EM_CFG_DIRECTION_RIGHT_TO_LEFT,												// ��������
	EM_CFG_DIRECTION_BOTH														// ȫ��
}EM_CFG_DIRECTION_TYPE;

// �¼�����EVENT_IVS_SPILLEDMATERIAL_DETECTION(���������¼�)��Ӧ�Ĺ�������
typedef struct tagCFG_SPILLEDMATERIAL_DETECTION_INFO
{
	char				        szRuleName[MAX_NAME_LEN];								// ��������,��ͬ����������
	BOOL				        bRuleEnable;											// ����ʹ��
	int					        nObjectTypeNum;											// ��Ӧ�������͸���
	char				        szObjectTypes[MAX_OBJECT_LIST_SIZE][MAX_NAME_LEN];		// ��Ӧ���������б�
	int                         nPtzPresetId;                                           // ��̨Ԥ�õ���	0~65535
	CFG_ALARM_MSG_HANDLE        stuEventHandler;										// ��������
	CFG_TIME_SECTION	        stuTimeSection[WEEK_DAY_NUM][MAX_REC_TSECT_EX];			// �¼���Ӧʱ���
	
	int							nDetectLinePointNum;									// �����߶˵����
	CFG_POLYLINE				stuDetectLine[MAX_POLYGON_NUM];							// �����ߡ� �������ͣ�������ÿ���˵�������һ����[0,8192)���䡣
	EM_CFG_DIRECTION_TYPE		emDirection;											// ��ⷽ��
	CFG_SIZEFILTER_INFO			stuSizeFilter;											// �����ض��ĳߴ��������Ϊ��߹����жϾ���
	BYTE						byReserved[4096]; 										// �����ֽ�
}CFG_SPILLEDMATERIAL_DETECTION_INFO;

// �¼����� EVENT_IVS_AIRPLANE_DETECTION (�ɻ���Ϊ���)��Ӧ�Ĺ�������
typedef struct tagCFG_AIRPLANE_DETECTION_INFO
{
	// ��Ϣ
	char				szRuleName[MAX_NAME_LEN];								// ��������,��ͬ����������
	BOOL				bRuleEnable;											// ����ʹ��
	int					nObjectTypeNum;											// ��Ӧ�������͸���
	char				szObjectTypes[MAX_OBJECT_LIST_SIZE][MAX_NAME_LEN];		// ��Ӧ���������б�
	int                 nPtzPresetId;											// ��̨Ԥ�õ���	0~65535
	CFG_ALARM_MSG_HANDLE stuEventHandler;										// ��������
	CFG_TIME_SECTION	stuTimeSection[WEEK_DAY_NUM][MAX_REC_TSECT_EX];			// �¼���Ӧʱ���

	CFG_POLYLINE		stuGuideLine[2];										// ������
	CFG_POLYLINE		stuStopLine[10][2];										// ֹͣ��
	int					nStopLineNum;											// ֹͣ������
	BYTE				byReserved[4096]; 										// �����ֽ�
} CFG_AIRPLANE_DETECTION_INFO;

// �¼����� DEV_EVENT_GENERATEGRAPH_DETECTION_INFO�����ɹ���ͼ�¼�����Ӧ�Ĺ�������
typedef struct tagCFG_GENERATEGRAPH_DETECTION_INFO
{
	// ͨ������
	char				        szRuleName[MAX_NAME_LEN];								// ��������,��ͬ����������
	BOOL				        bRuleEnable;											// ����ʹ��
	int					        nObjectTypeNum;											// ��Ӧ�������͸���
	char				        szObjectTypes[MAX_OBJECT_LIST_SIZE][MAX_NAME_LEN];		// ��Ӧ���������б�
	int                         nPtzPresetId;                                           // ��̨Ԥ�õ���	0~65535
	CFG_ALARM_MSG_HANDLE        stuEventHandler;										// ��������
	CFG_TIME_SECTION	        stuTimeSection[WEEK_DAY_NUM][MAX_REC_TSECT_EX];			// �¼���Ӧʱ���
	
	// ��������
    UINT						nPlanID;                                                // �ƻ�ID,�������Ч,��1��ʼ
}CFG_GENERATEGRAPH_DETECTION_INFO;

// �¼����� EVENT_IVS_FLOATINGOBJECT_DETECTION (Ư������)��Ӧ�Ĺ�������
typedef struct tagCFG_FLOATINGOBJECT_DETECTION_INFO
{
	// ��Ϣ
	char				szRuleName[MAX_NAME_LEN];								// ��������,��ͬ����������
	BOOL				bRuleEnable;											// ����ʹ��
	int					nObjectTypeNum;											// ��Ӧ�������͸���
	char				szObjectTypes[MAX_OBJECT_LIST_SIZE][MAX_NAME_LEN];		// ��Ӧ���������б�
	int                 nPtzPresetId;											// ��̨Ԥ�õ���	0~65535
	CFG_ALARM_MSG_HANDLE stuEventHandler;										// ��������
	CFG_TIME_SECTION	stuTimeSection[WEEK_DAY_NUM][MAX_REC_TSECT_EX];			// �¼���Ӧʱ���

	int					nDetectRegionPoint;										// �����������
	CFG_POLYGON			stuDetectRegion[MAX_POLYGON_NUM];						// �����

	float				fAlarmThreshold;										// ������ֵ������������ڵ�Ư����ռ�ȳ�����ֵʱ���������;��λ:%��ȡֵ��Χ(0, 100]
	UINT				nAlarmInterval;											// ����ʱ����������λ���룩��ȡֵ��Χ[60, 86400]
	BOOL				bDataUpload;											// �Ƿ��ϱ�ʵʱ���ݡ�
	UINT				nUpdateInterval;										// ʵʱ�����ϱ������(��λ����)��ȡֵ��Χ[60, 86400]

	BYTE				byReserved[4096]; 										// �����ֽ�
} CFG_FLOATINGOBJECT_DETECTION_INFO;

// �¼����� EVENT_IVS_WATER_LEVEL_DETECTION (ˮλ����¼�)��Ӧ�Ĺ�������
typedef struct tagCFG_WATER_LEVEL_DETECTION_INFO
{
	// ��Ϣ
	char				szRuleName[MAX_NAME_LEN];								// ��������,��ͬ����������
	BOOL				bRuleEnable;											// ����ʹ��
	int					nObjectTypeNum;											// ��Ӧ�������͸���
	char				szObjectTypes[MAX_OBJECT_LIST_SIZE][MAX_NAME_LEN];		// ��Ӧ���������б�
	int                 nPtzPresetId;											// ��̨Ԥ�õ���	0~65535
	CFG_ALARM_MSG_HANDLE stuEventHandler;										// ��������
	CFG_TIME_SECTION	stuTimeSection[WEEK_DAY_NUM][MAX_REC_TSECT_EX];			// �¼���Ӧʱ���

	int					nBaseLinePointNum;										// ƴ�ӵ�ˮλ���������
	CFG_POLYGON			stuBaseLine[16];										// ����ƴ�ӵ�ˮλ�ߣ���ҳ���ϱ궨��ƴ�Ӵ�������ƴ���������
																				// һ��4���㣬��󲻳���16����,ÿ���������һ����[0,8192]����
	float				fBaseLevel;												// ��ӦBaseLine�궨��ʵ��ˮλֵ������λ���ף�
	float				fHighLevel;												// ˮλ���ޱ�����ֵ(��λ����)
	float				fLowLevel;												// ˮλ���ޱ�����ֵ(��λ����)
	UINT				nAlarmInterval;											// ����ʱ����������λ���룩��ȡֵ��Χ[60, 86400]
	BOOL				bDataUpload;											// �Ƿ��ϱ�ʵʱ���ݡ�
	UINT				nUpdateInterval;										// ʵʱ�����ϱ������(��λ����)��ȡֵ��Χ[60, 86400]

	BYTE				byReserved[4096]; 										// �����ֽ�
} CFG_WATER_LEVEL_DETECTION_INFO;

// �¼����� EVENT_IVS_PHONECALL_DETECT (��绰�����¼�)��Ӧ�Ĺ�������
typedef struct tagCFG_PHONECALL_DETECT_INFO
{
	char				szRuleName[MAX_NAME_LEN];								// ��������,��ͬ����������
	BOOL				bRuleEnable;											// ����ʹ��
	int					nObjectTypeNum;											// ��Ӧ�������͸���
	char				szObjectTypes[MAX_OBJECT_LIST_SIZE][MAX_NAME_LEN];		// ��Ӧ���������б�
	int                 nPtzPresetId;											// ��̨Ԥ�õ���	0~65535
	CFG_ALARM_MSG_HANDLE stuEventHandler;										// ��������
	CFG_TIME_SECTION	stuTimeSection[WEEK_DAY_NUM][MAX_REC_TSECT_EX];			// �¼���Ӧʱ���
	
	int					nDetectRegionPoint;										// �����������
	CFG_POLYGON			stuDetectRegion[MAX_POLYGON_NUM];						// �����
	int				    nMinDuration;											// ��̳���ʱ�䣬��λ��
	int				    nSensitivity;											// ������
	int				    nReportInterval;										// ����ʱ����
	BYTE				byReserved[4096]; 										// �����ֽ�
} CFG_PHONECALL_DETECT_INFO;

// �¼����� EVENT_IVS_SMOKING_DETECT (���̼�ⱨ���¼�)��Ӧ�Ĺ�������
typedef struct tagCFG_SMOKING_DETECT_INFO
{
	char				szRuleName[MAX_NAME_LEN];								// ��������,��ͬ����������
	BOOL				bRuleEnable;											// ����ʹ��
	int					nObjectTypeNum;											// ��Ӧ�������͸���
	char				szObjectTypes[MAX_OBJECT_LIST_SIZE][MAX_NAME_LEN];		// ��Ӧ���������б�
	int                 nPtzPresetId;											// ��̨Ԥ�õ���	0~65535
	CFG_ALARM_MSG_HANDLE stuEventHandler;										// ��������
	CFG_TIME_SECTION	stuTimeSection[WEEK_DAY_NUM][MAX_REC_TSECT_EX];			// �¼���Ӧʱ���

	int					nDetectRegionPoint;										// �����������
	CFG_POLYGON			stuDetectRegion[MAX_POLYGON_NUM];						// �����
	int				    nMinDuration;											// ��̳���ʱ�䣬��λ��
	int				    nSensitivity;											// ������
	int				    nReportInterval;										// ����ʱ����
	BYTE				byReserved[4096]; 										// �����ֽ�
} CFG_SMOKING_DETECT_INFO;


// �¼����� EVENT_IVS_HELMET_DETECTION (��ȫñ���)��Ӧ�Ĺ�������
typedef struct tagCFG_HELMET_DETECTION_INFO
{
    char				szRuleName[MAX_NAME_LEN];								// ��������,��ͬ����������
    BOOL				bRuleEnable;											// ����ʹ��
    int					nObjectTypeNum;											// ��Ӧ�������͸���
    char				szObjectTypes[MAX_OBJECT_LIST_SIZE][MAX_NAME_LEN];		// ��Ӧ���������б�
    int                 nPtzPresetId;											// ��̨Ԥ�õ���	0~65535
    CFG_ALARM_MSG_HANDLE stuEventHandler;										// ��������
    CFG_TIME_SECTION	stuTimeSection[WEEK_DAY_NUM][MAX_REC_TSECT_EX];			// �¼���Ӧʱ���

    int                 nDetectRegionPoint;                                     // �����������
    CFG_POLYGON         stuDetectRegion[MAX_POLYGON_NUM];                       // �������
    int                 nDectRegionID;                                          // ���������
    BYTE				byReserved[4096]; 										// �����ֽ�
}CFG_HELMET_DETECTION_INFO;

//�¼�����EVENT_IVS_HOLD_UMBRELLA(Υ���ɡ�¼�)��Ӧ��������
typedef struct tagCFG_HOLDUMBRELLA_INFO
{
	// ��Ϣ
	char                szRuleName[MAX_NAME_LEN];                               // ��������,��ͬ����������
	BOOL                bRuleEnable;                                            // ����ʹ��
	int                 nObjectTypeNum;	                                        // ��Ӧ�������͸���
	char                szObjectTypes[MAX_OBJECT_LIST_SIZE][MAX_NAME_LEN];      // ��Ӧ���������б�
	int                 nPtzPresetId;                                           // ��̨Ԥ�õ���	0~65535
	CFG_ALARM_MSG_HANDLE stuEventHandler;                                       // ��������
	CFG_TIME_SECTION    stuTimeSection[WEEK_DAY_NUM][MAX_REC_TSECT_EX];         // �¼���Ӧʱ���

	int                 nDetectRegionPoint;                                     // �����������
	CFG_POLYGON         stuDetectRegion[MAX_POLYGON_NUM];                       // �����

	int                 nSensitivity;											// �����ȣ�ֵԽС������Խ�� ��Χ[1�� 10]
	int                 nMinDuration;											// ��̳���ʱ��,��λ���� ��Χ[0�� 3600]
	int                 nTrackDuration;											// ���ٳ���ʱ�� ��Χ[0�� 3600]
	UINT				nDetectRegionNumber;									// ��������
	BYTE				byReserved[4096]; 										// �����ֽ�
}CFG_HOLDUMBRELLA_INFO;

//�¼�����EVENT_IVS_GARBAGE_EXPOSURE(������¶�¼�)��Ӧ��������
typedef struct tagCFG_GARBAGEEXPOSURE_INFO
{
	// ��Ϣ
	char                szRuleName[MAX_NAME_LEN];                               // ��������,��ͬ����������
	BOOL                bRuleEnable;                                            // ����ʹ��
	int                 nObjectTypeNum;	                                        // ��Ӧ�������͸���
	char                szObjectTypes[MAX_OBJECT_LIST_SIZE][MAX_NAME_LEN];      // ��Ӧ���������б�
	int                 nPtzPresetId;                                           // ��̨Ԥ�õ���	0~65535
	CFG_ALARM_MSG_HANDLE stuEventHandler;                                       // ��������
	CFG_TIME_SECTION    stuTimeSection[WEEK_DAY_NUM][MAX_REC_TSECT_EX];         // �¼���Ӧʱ���

	int                 nDetectRegionPoint;                                     // �����������
	CFG_POLYGON         stuDetectRegion[MAX_POLYGON_NUM];                       // �����

	int                 nSensitivity;											// �����ȣ�ֵԽС������Խ�� ��Χ[1�� 10]
	int                 nMinDuration;											// ��̳���ʱ��,��λ���� ��Χ[0�� 3600]
	int                 nTrackDuration;											// ���ٳ���ʱ�� ��Χ[0�� 3600]
	UINT				nDetectRegionNumber;									// ��������
	BYTE				byReserved[4096]; 										// �����ֽ�
}CFG_GARBAGEEXPOSURE_INFO;

//�¼�����EVENT_IVS_DUSTBIN_OVER_FLOW(����Ͱ�����¼�)��Ӧ��������
typedef struct tagCFG_DUSTBIN_OVERFLOW_INFO
{
	// ��Ϣ
	char                szRuleName[MAX_NAME_LEN];                               // ��������,��ͬ����������
	BOOL                bRuleEnable;                                            // ����ʹ��
	int                 nObjectTypeNum;	                                        // ��Ӧ�������͸���
	char                szObjectTypes[MAX_OBJECT_LIST_SIZE][MAX_NAME_LEN];      // ��Ӧ���������б�
	int                 nPtzPresetId;                                           // ��̨Ԥ�õ���	0~65535
	CFG_ALARM_MSG_HANDLE stuEventHandler;                                       // ��������
	CFG_TIME_SECTION    stuTimeSection[WEEK_DAY_NUM][MAX_REC_TSECT_EX];         // �¼���Ӧʱ���

	int                 nDetectRegionPoint;                                     // �����������
	CFG_POLYGON         stuDetectRegion[MAX_POLYGON_NUM];                       // �����

	int                 nSensitivity;											// �����ȣ�ֵԽС������Խ�� ��Χ[1�� 10]
	int                 nMinDuration;											// ��̳���ʱ��,��λ���� ��Χ[0�� 3600]
	int                 nTrackDuration;											// ���ٳ���ʱ�� ��Χ[0�� 3600]
	UINT				nDetectRegionNumber;									// ��������
	BYTE				byReserved[4096]; 										// �����ֽ�
}CFG_DUSTBIN_OVERFLOW_INFO;

//�¼�����EVENT_IVS_DOOR_FRONT_DIRTY(��ǰ�����¼�)��Ӧ��������
typedef struct tagCFG_DOORFRONT_DIRTY_INFO
{
	// ��Ϣ
	char                szRuleName[MAX_NAME_LEN];                               // ��������,��ͬ����������
	BOOL                bRuleEnable;                                            // ����ʹ��
	int                 nObjectTypeNum;	                                        // ��Ӧ�������͸���
	char                szObjectTypes[MAX_OBJECT_LIST_SIZE][MAX_NAME_LEN];      // ��Ӧ���������б�
	int                 nPtzPresetId;                                           // ��̨Ԥ�õ���	0~65535
	CFG_ALARM_MSG_HANDLE stuEventHandler;                                       // ��������
	CFG_TIME_SECTION    stuTimeSection[WEEK_DAY_NUM][MAX_REC_TSECT_EX];         // �¼���Ӧʱ���

	int                 nDetectRegionPoint;                                     // �����������
	CFG_POLYGON         stuDetectRegion[MAX_POLYGON_NUM];                       // �����

	int                 nSensitivity;											// �����ȣ�ֵԽС������Խ�� ��Χ[1�� 10]
	int                 nMinDuration;											// ��̳���ʱ��,��λ���� ��Χ[0�� 3600]
	int                 nTrackDuration;											// ���ٳ���ʱ�� ��Χ[0�� 3600]
	char				szShopAddress[256];										// ���̵�ַ
	UINT				nDetectRegionNumber;									// ��������
	BYTE				byReserved[4096]; 										// �����ֽ�
}CFG_DOORFRONT_DIRTY_INFO;

//�¼����� EVENT_IVS_STAY_ALONE_DETECTION (���˶����¼�)��Ӧ�Ĺ�������
typedef struct tagCFG_STAY_ALONE_DETECTION_INFO
{
    char				szRuleName[MAX_NAME_LEN];								// ��������,��ͬ����������
    BOOL				bRuleEnable;											// ����ʹ��
    int					nObjectTypeNum;											// ��Ӧ�������͸���
    char				szObjectTypes[MAX_OBJECT_LIST_SIZE][MAX_NAME_LEN];		// ��Ӧ���������б�
    int                 nPtzPresetId;											// ��̨Ԥ�õ���	0~65535
    CFG_ALARM_MSG_HANDLE stuEventHandler;										// ��������
    CFG_TIME_SECTION	stuTimeSection[WEEK_DAY_NUM][MAX_REC_TSECT_EX];			// �¼���Ӧʱ���

    int                                     nDetectRegionNum;                   // ����μ�����򶥵����
    CFG_POLYGON                             stuDetectRegion[MAX_POLYGON_NUM];   // ����μ������
    UINT                                    nMinDuration;                       // ��̳���ʱ��, ��λ:��
    UINT                                    nReportInterval;                    // ����ʱ����, ��λ:��, Ϊ0ʱ��ʾֻ��һ��
    BOOL                	                bSizeFileter;                       // �����ض��ĳߴ�������Ƿ���Ч
    CFG_SIZEFILTER_INFO 	                stuSizeFileter;                     // �����ض��ĳߴ������
    BYTE				                    byReserved[4096]; 					// �����ֽ�
}CFG_STAY_ALONE_DETECTION_INFO;


// �¼�����EVENT_IVS_INTELLI_SHELF(���ܲ����¼�)��Ӧ�Ĺ�������
typedef struct tagCFG_INTELLI_SHELF_INFO
{
    char				szRuleName[MAX_NAME_LEN];								// ��������,��ͬ����������
    BOOL				bRuleEnable;											// ����ʹ��
    int					nObjectTypeNum;											// ��Ӧ�������͸���
    char				szObjectTypes[MAX_OBJECT_LIST_SIZE][MAX_NAME_LEN];		// ��Ӧ���������б�
    int                 nPtzPresetId;											// ��̨Ԥ�õ���	0~65535
    CFG_ALARM_MSG_HANDLE stuEventHandler;										// ��������
    CFG_TIME_SECTION	stuTimeSection[WEEK_DAY_NUM][MAX_REC_TSECT_EX];			// �¼���Ӧʱ���

    int                 nDetectRegionPoint;                                     // �����������
    CFG_POLYGON         stuDetectRegion[MAX_POLYGON_NUM];                       // �����
    unsigned short      nThreshold;                                             // �����ϻ���ٷֱȵ�����ֵ�򱨾�, 0��ʾ������
    unsigned short      nAreaID;                                                // ����ID����ͨ����PresetID�µ�ĳ������,ȡֵ��Χ1-5
    BYTE				byReserved[4096]; 					                    // �����ֽ�
}CFG_INTELLI_SHELF_INFO;

// �¼�����EVENT_IVS_SHOP_WINDOW_POST(���������¼�)��Ӧ�Ĺ�������
typedef struct tagCFG_SHOP_WINDOW_POST_INFO
{
    char				szRuleName[MAX_NAME_LEN];								// ��������,��ͬ����������
    BOOL				bRuleEnable;											// ����ʹ��
    int					nObjectTypeNum;											// ��Ӧ�������͸���
    char				szObjectTypes[MAX_OBJECT_LIST_SIZE][MAX_NAME_LEN];		// ��Ӧ���������б�
    int                 nPtzPresetId;											// ��̨Ԥ�õ���	0~65535
    CFG_ALARM_MSG_HANDLE stuEventHandler;										// ��������
    CFG_TIME_SECTION	stuTimeSection[WEEK_DAY_NUM][MAX_REC_TSECT_EX];			// �¼���Ӧʱ���

    int                 nDetectRegionPoint;                                     // �����������
    CFG_POLYGON         stuDetectRegion[MAX_POLYGON_NUM];                       // �����

    unsigned short      nMinDuration;                                           // ��̳���ʱ��,��λ����0-3600s
    unsigned short      nTrackDuration;                                         // ���ٳ���ʱ�� 0-3600s
    int                 nSensitivity;                                           // �����ȣ�ֵԽС������Խ��  ȡֵ1-10
    UINT                nDetectRegionNumber;                                    // ��������
    char                szShopAddress[256];                                     // ���̵�ַ
    BYTE				byReserved[1024]; 					                    // �����ֽ�
}CFG_SHOP_WINDOW_POST_INFO;


// �¼�����EVENT_IVS_SHOP_SIGN_ABNORMAL(�����쳣�¼�)��Ӧ�Ĺ�������
typedef struct tagCFG_SHOP_SIGN_ABNORMAL_INFO
{
    char				szRuleName[MAX_NAME_LEN];								// ��������,��ͬ����������
    BOOL				bRuleEnable;											// ����ʹ��
    int					nObjectTypeNum;											// ��Ӧ�������͸���
    char				szObjectTypes[MAX_OBJECT_LIST_SIZE][MAX_NAME_LEN];		// ��Ӧ���������б�
    int                 nPtzPresetId;											// ��̨Ԥ�õ���	0~65535
    CFG_ALARM_MSG_HANDLE stuEventHandler;										// ��������
    CFG_TIME_SECTION	stuTimeSection[WEEK_DAY_NUM][MAX_REC_TSECT_EX];			// �¼���Ӧʱ���

    int                 nDetectRegionPoint;                                     // �����������
    CFG_POLYGON         stuDetectRegion[MAX_POLYGON_NUM];                       // �����

    unsigned short      nMinDuration;                                           // ��̳���ʱ��,��λ����0-3600s
    unsigned short      nTrackDuration;                                         // ���ٳ���ʱ�� 0-3600s
    int                 nSensitivity;                                           // �����ȣ�ֵԽС������Խ��  ȡֵ1-10
    UINT                nDetectRegionNumber;                                    // ��������
    char                szShopAddress[256];                                     // ���̵�ַ
    BYTE				byReserved[1024]; 					                    // �����ֽ�
}CFG_SHOP_SIGN_ABNORMAL_INFO;


// �¼�����EVENT_IVS_ANIMAL_DETECTION(�������¼�)��Ӧ�Ĺ�������
typedef struct tagCFG_ANIMAL_DETECTION_INFO
{
    char				szRuleName[MAX_NAME_LEN];								// ��������,��ͬ����������
    BOOL				bRuleEnable;											// ����ʹ��
    int					nObjectTypeNum;											// ��Ӧ�������͸���
    char				szObjectTypes[MAX_OBJECT_LIST_SIZE][MAX_NAME_LEN];		// ��Ӧ���������б�
    int                 nPtzPresetId;											// ��̨Ԥ�õ���	0~65535
    CFG_ALARM_MSG_HANDLE stuEventHandler;										// ��������
    CFG_TIME_SECTION	stuTimeSection[WEEK_DAY_NUM][MAX_REC_TSECT_EX];			// �¼���Ӧʱ���

    int                 nDetectRegionPoint;                                     // �����������
    CFG_POLYGON         stuDetectRegion[MAX_POLYGON_NUM];                       // �����
    int                 nReportInterval;                                        // �������ڣ���λ��,��Χ(1��3600),Ĭ��5sȡֵ��Χ1-5

    BYTE				byReserved[1024]; 					                    // �����ֽ�
}CFG_ANIMAL_DETECTION_INFO;



// �¼�����EVENT_IVS_CONGESTION_DETECTION(��·��������ӵ�±����¼�)��Ӧ�Ĺ�������
typedef struct tagCFG_CONGESTION_DETECTION_INFO
{
	char				szRuleName[MAX_NAME_LEN];								// ��������,��ͬ����������
	BOOL				bRuleEnable;											// ����ʹ��
	int					nObjectTypeNum;											// ��Ӧ�������͸���
	char				szObjectTypes[MAX_OBJECT_LIST_SIZE][MAX_NAME_LEN];		// ��Ӧ���������б�
	CFG_ALARM_MSG_HANDLE stuEventHandler;										// ��������
	CFG_TIME_SECTION	stuTimeSection[WEEK_DAY_NUM][MAX_REC_TSECT_EX];			// �¼���Ӧʱ���
	int                 nPtzPresetId;											// ��̨Ԥ�õ���	0~65535

	int                 nDetectRegionPoint;                                     // �����������
	CFG_POLYGON         stuDetectRegion[32];									// �����
	BOOL				bCongestionAlarmEnable;									// ӵ�±���ʹ��
	BOOL				bQueueAlarmEnable;										// �Ŷӳ��ȱ���ʹ��
	UINT				nCongestionThreshold;									// ӵ�³���������ֵ,��λ����ȡֵ[10,1000]
	UINT				nQueueLengthThreshold;									// �����Ŷӳ��ȱ�����ֵ,��λ�ף�ȡֵ[10,1000]
	UINT				nCongestionDuration;									// ���ӵ��ʱ����ֵ ��λ���ӣ�ȡֵ[1,100]
	UINT				nAlarmInterval;											// ����ʱ����,ȡ0ֻ����һ�� ��λ�룬ȡֵ[0��65535]
	BYTE				byReserved[4096]; 					                    // �����ֽ�
}CFG_CONGESTION_DETECTION_INFO;

// �¼�����EVENT_IVS_VEHICLELIMIT_DETECTION(ͣ����������ͣ���������ޱ���)��Ӧ�Ĺ�������
typedef struct tagCFG_VEHICLELIMIT_DETECTION_INFO
{
	char				szRuleName[MAX_NAME_LEN];								// ��������,��ͬ����������
	BOOL				bRuleEnable;											// ����ʹ��
	int					nObjectTypeNum;											// ��Ӧ�������͸���
	char				szObjectTypes[MAX_OBJECT_LIST_SIZE][MAX_NAME_LEN];		// ��Ӧ���������б�
	CFG_ALARM_MSG_HANDLE stuEventHandler;										// ��������
	CFG_TIME_SECTION	stuTimeSection[WEEK_DAY_NUM][MAX_REC_TSECT_EX];			// �¼���Ӧʱ���
	int                 nPtzPresetId;											// ��̨Ԥ�õ���	0~65535

	int                 nDetectRegionPoint;                                     // �����������
	CFG_POLYGON         stuDetectRegion[32];									// �����
	BOOL				bLimitAlarmEnable;										// �������ޱ���ʹ��
	UINT				nVehicleThreshold;										// ����������ֵ,��λ����ȡֵ[10,1000]
	UINT				nAlarmInterval;											// ����ʱ����,ȡ0ֻ����һ�� ��λ�룬ȡֵ[0��65535]
	BYTE				byReserved[4096]; 					                    // �����ֽ�
}CFG_VEHICLELIMIT_DETECTION_INFO;

// ���������Ϣ
typedef struct tagNET_DETECT_REGION_INFO
{
    int                     nDetectRegionPoint;                                     // �����������
    CFG_POLYGON             stuDetectRegion[MAX_POLYGON_NUM];                       // �����
    BYTE                    byReserved[252];                                        // �����ֽ�
} NET_DETECT_REGION_INFO;

// �¼�����EVENT_IVS_BREED_DETECTION(�ǻ���ֳ����¼�)��Ӧ�Ĺ�������
typedef struct tagCFG_BREED_DETECTION_INFO
{
    char                    szRuleName[MAX_NAME_LEN];                               // ��������,��ͬ����������
    BOOL                    bRuleEnable;                                            // ����ʹ��
    int                     nObjectTypeNum;                                         // ��Ӧ�������͸���
    char                    szObjectTypes[MAX_OBJECT_LIST_SIZE][MAX_NAME_LEN];      // ��Ӧ���������б�
    int                     nPtzPresetId;                                           // ��̨Ԥ�õ���   0~65535
    CFG_ALARM_MSG_HANDLE    stuEventHandler;                                        // ��������
    CFG_TIME_SECTION        stuTimeSection[WEEK_DAY_NUM][MAX_REC_TSECT_EX];         // �¼���Ӧʱ���

    UINT                    nSensitivity;                                           // ������, 1-10
    UINT                    nMinDuration;                                           // ���Ƶ�ʣ�ָ��ʱ���ڼ��һ�Σ���λ����
    float                   fCameraHeight;                                          // �������װ�߶ȣ���λ:m, 2-5
    float                   fHeightMultiple;                                        // �������װ�߶ȱ����������㷨�������ֵ, 0.5-2
    UINT                    nDetectRegionNum;                                       // ���������
    NET_DETECT_REGION_INFO  stuDetectRegionInfo[8];                                 // �������Ϣ
    BOOL                    bSizeFileter;                                           // �����ض��ĳߴ�������Ƿ���Ч
    CFG_SIZEFILTER_INFO     stuSizeFileter;                                         // �����ض��ĳߴ������
    BYTE                    byReserved[4096];                                       // �����ֽ�
} CFG_BREED_DETECTION_INFO;

// ��װ��ɫ
typedef enum tagEM_WORKCLOTHES_COLOR_TYPE
{
    EM_WORKCLOTHES_COLOR_UNKNOWN,						// δ֪
    EM_WORKCLOTHES_COLOR_WHITE,						    // ��ɫ
    EM_WORKCLOTHES_COLOR_ORANGE,						// ��ɫ
    EM_WORKCLOTHES_COLOR_PINK,							// ��ɫ
    EM_WORKCLOTHES_COLOR_BLACK,						    // ��ɫ
    EM_WORKCLOTHES_COLOR_RED,							// ��ɫ
    EM_WORKCLOTHES_COLOR_YELLOW,						// ��ɫ
    EM_WORKCLOTHES_COLOR_GRAY,							// ��ɫ
    EM_WORKCLOTHES_COLOR_BLUE,							// ��ɫ
    EM_WORKCLOTHES_COLOR_GREEN,						    // ��ɫ
    EM_WORKCLOTHES_COLOR_PURPLE,						// ��ɫ
    EM_WORKCLOTHES_COLOR_BROWN,						    // ��ɫ
}EM_WORKCLOTHES_COLOR_TYPE;

// ��ȫñ���
typedef struct tagNET_WORK_HELMET
{
    BOOL                        bEnbale;            // �Ƿ��ⰲȫñ
    BOOL                        bWeared;            // �Ƿ񴩴���ȫñ
    BOOL                        bDetectColor;       // �Ƿ�ָ�������ɫ
    EM_WORKCLOTHES_COLOR_TYPE   emColor;            // ��ȫñ��ɫ��DetectColorΪtrueʱ��Ч
    BYTE                        byReserved[64];     // �����ֽ�
}NET_WORK_HELMET;

// ���������
typedef struct tagNET_WORK_CLOTHES
{

    BOOL                        bEnbale;            // �Ƿ��⹤����
    BOOL                        bWeared;            // �Ƿ��Ź�����
    BOOL                        bDetectColor;       // �Ƿ�ָ�������ɫ
    EM_WORKCLOTHES_COLOR_TYPE   emColor;            // ��������ɫ��DetectColorΪtrueʱ��Ч
    BYTE                        byReserved[64];     // �����ֽ�
}NET_WORK_CLOTHES;

// ��׼��
typedef struct tagNET_WORK_VEST
{

    BOOL                        bEnbale;            // �Ƿ������
    BOOL                        bWeared;            // �Ƿ������
    BOOL                        bDetectColor;       // �Ƿ�ָ�������ɫ
    EM_WORKCLOTHES_COLOR_TYPE   emColor;            // �����ɫ��DetectColorΪtrueʱ��Ч
    BYTE                        byReserved[64];     // �����ֽ�
}NET_WORK_VEST;

// ��װ��������
typedef struct tagNET_WORK_CLOTHES_DESCRIPTION
{
    NET_WORK_HELMET     stuWorkHelmet;                  // ��ȫñ���
    NET_WORK_CLOTHES    stuWorkClothes;                 // ���������
    NET_WORK_VEST       stuWorkVest;                    // ��׼��
    BYTE				bReserved[512];					// �����ֽ�
}NET_WORK_CLOTHES_DESCRIPTION;

// �¼�����EVENT_IVS_WORKCLOTHES_DETECT(��װ(��ȫñ/��������)����¼�)��Ӧ�Ĺ�������
typedef struct tagCFG_WORKCLOTHES_DETECT_INFO
{
    char				szRuleName[MAX_NAME_LEN];								// ��������,��ͬ����������
    BOOL				bRuleEnable;											// ����ʹ��
    int					nObjectTypeNum;											// ��Ӧ�������͸���
    char				szObjectTypes[MAX_OBJECT_LIST_SIZE][MAX_NAME_LEN];		// ��Ӧ���������б�
    CFG_ALARM_MSG_HANDLE stuEventHandler;										// ��������
    CFG_TIME_SECTION	stuTimeSection[WEEK_DAY_NUM][MAX_REC_TSECT_EX];			// �¼���Ӧʱ���
    int                 nPtzPresetId;											// ��̨Ԥ�õ���	0~65535

    UINT                nDetectRegionPoint;                                     // �����������
    CFG_POLYGON         stuDetectRegion[MAX_POLYGON_NUM];                       // �����
    UINT                nWorkClothesDescriptionCount;                           // ��װ�������������ʵ�ʸ���
    NET_WORK_CLOTHES_DESCRIPTION  stuWorkClothesDescription[64];                // ��װ��������,�������Ԫ��64����
    BYTE				byReserved[1028]; 					                    // �����ֽ�
} CFG_WORKCLOTHES_DETECT_INFO;

// �¼�����EVENT_IVS_AIRPORT_VEHICLE_DETECT(�������ܱ��ϳ�������¼�)��Ӧ�Ĺ�������
typedef struct tagCFG_AIRPORT_VEHICLE_DETECT_INFO
{
    char                    szRuleName[MAX_NAME_LEN];                               // ��������,��ͬ����������
    BOOL                    bRuleEnable;                                            // ����ʹ��
    int                     nObjectTypeNum;                                         // ��Ӧ�������͸���
    char                    szObjectTypes[MAX_OBJECT_LIST_SIZE][MAX_NAME_LEN];      // ��Ӧ���������б�
    CFG_ALARM_MSG_HANDLE    stuEventHandler;                                        // ��������
	CFG_TIME_SECTION        stuTimeSection[WEEK_DAY_NUM][MAX_REC_TSECT_EX];         // �¼���Ӧʱ���
	int                     nPtzPresetId;                                           // ��̨Ԥ�õ���   0~65535

	UINT					nDetectRegionPoint;										// �����������
	CFG_POLYGON				stuDetectRegion[20];									// �����

	BYTE                    byReserved[4096];                                       // �����ֽ�
} CFG_AIRPORT_VEHICLE_DETECT_INFO;

// �¼�����EVENT_IVS_PIG_TEMP_DETECT(�ǻ���ֳ�����¼��)��Ӧ�Ĺ�������
typedef struct tagCFG_PIG_TEMPERATURE_DETECT_INFO
{
    char                    szRuleName[MAX_NAME_LEN];                               // ��������,��ͬ����������
    BOOL                    bRuleEnable;                                            // ����ʹ��
    int                     nObjectTypeNum;                                         // ��Ӧ�������͸���
    char                    szObjectTypes[MAX_OBJECT_LIST_SIZE][MAX_NAME_LEN];      // ��Ӧ���������б�
    int                     nPtzPresetId;                                           // ��̨Ԥ�õ���   0~65535
    CFG_ALARM_MSG_HANDLE    stuEventHandler;                                        // ��������
    CFG_TIME_SECTION        stuTimeSection[WEEK_DAY_NUM][MAX_REC_TSECT_EX];         // �¼���Ӧʱ���

    UINT                    nSensitivity;                                           // ������, 1-10
    UINT                    nMaxTemperatureNum;                                     // �����°�����������¸���
    int                     nDetectRegionPoint;                                     // �����������
    CFG_POLYGON             stuDetectRegion[MAX_POLYGON_NUM];                       // �����
    BYTE                    byReserved1[4];                                         // ����
    BOOL                    bSizeFileter;                                           // �����ض��ĳߴ�������Ƿ���Ч
    CFG_SIZEFILTER_INFO     stuSizeFileter;                                         // �����ض��ĳߴ������
    BYTE                    byReserved[4096];                                       // �����ֽ�
} CFG_PIG_TEMPERATURE_DETECT_INFO;

// ���鳵λ���з���
typedef enum tagEM_PARKING_DIRECTION
{
    EM_PARKING_DIRECTION_UNKNOWN,             // δ֪
    EM_PARKING_DIRECTION_HORIZONTAL,          // ����
    EM_PARKING_DIRECTION_VERTICAL,            // ����
} EM_PARKING_DIRECTION;

// ͣ����λ��Ϣ
typedef struct tagCFG_PARKING_INFO
{
    int                    nID;                                                      // ��λID����Χ:[0,99]
    char                   szName[32];                                               // ��λ����
    int                    nDetectRegionPoint;                                       // �����������
    CFG_POLYGON            stuDetectRegion[MAX_POLYGON_NUM];                         // �������
    BYTE                   byReserved[512];                                          // �����ֽ�
} CFG_PARKING_INFO;

// ����ͣ��λ����Ϣ
typedef struct tagCFG_PARKING_GROUP
{
    EM_PARKING_DIRECTION    emDirection;                                             // ���鳵λ���з���
    int                     nParkingNum;                                             // ͣ����λ����
    CFG_PARKING_INFO        stuParkingInfo[100];                                     // ͣ����λ��Ϣ
    BYTE                    byReserved[512];                                         // �����ֽ�
} CFG_PARKING_GROUP;

// �¼����� EVENT_IVS_PARKING_LOT_STATUS_DETECTION (����ͣ��λ״̬���)��Ӧ�Ĺ�������
typedef struct tagCFG_PARKING_LOT_STATUS_DETECTION
{
    char                   szRuleName[MAX_NAME_LEN];                                 // ��������,��ͬ����������
    BOOL                   bRuleEnable;                                              // ����ʹ��
    int                    nObjectTypeNum;                                           // ��Ӧ�������͸���
    char                   szObjectTypes[MAX_OBJECT_LIST_SIZE][MAX_NAME_LEN];        // ��Ӧ���������б�
    CFG_ALARM_MSG_HANDLE   stuEventHandler;                                          // ��������
    CFG_TIME_SECTION       stuTimeSection[WEEK_DAY_NUM][MAX_REC_TSECT_EX];           // �¼���Ӧʱ���
    int                    nPtzPresetId;                                             // ��̨Ԥ�õ���   0~65535
    
    int                    nReportInterval;                                          // ����ʱ����, ��λ�룬��Χ:[5,3600]
    int                    nConfirmTime;                                             // ͣ��״̬ȷ��ʱ�䷧ֵ, ��λΪ��, ��Χ:[0,20]
    BOOL                   bSubscribePicture;                                        // �Ƿ���ͼƬ
    int                    nParkingGroupNumber;                                      // ����ͣ��λ�����
    CFG_PARKING_GROUP      stuParkingGroup[100];                                     // ����ͣ��λ����Ϣ
    BYTE                   byReserved[1024];                                         // �����ֽ�
} CFG_PARKING_LOT_STATUS_DETECTION;

// �¼����� EVENT_IVS_DREGS_UNCOVERED (������δ�ڸ��ػ�����¼�)��Ӧ�Ĺ�������
typedef struct tagCFG_DREGS_UNCOVERED_DETECTION
{
    char                   szRuleName[MAX_NAME_LEN];                                 // ��������,��ͬ����������
    BOOL                   bRuleEnable;                                              // ����ʹ��
    int                    nObjectTypeNum;                                           // ��Ӧ�������͸���
    char                   szObjectTypes[MAX_OBJECT_LIST_SIZE][MAX_NAME_LEN];        // ��Ӧ���������б�
    CFG_ALARM_MSG_HANDLE   stuEventHandler;                                          // ��������
    CFG_TIME_SECTION       stuTimeSection[WEEK_DAY_NUM][MAX_REC_TSECT_EX];           // �¼���Ӧʱ���
    int                    nPtzPresetId;                                             // ��̨Ԥ�õ���   0~65535

    int                    nDetectRegionPoint;                                       // �����������
    CFG_POLYGON            stuDetectRegion[MAX_POLYGON_NUM];                         // �������
    BYTE                   byReserved[1020];                                         // �����ֽ�
} CFG_DREGS_UNCOVERED_DETECTION;


// �¼����� EVENT_IVS_VEHICLE_COMPARE (ǰ���ܼ��+�����ܱȶ�)��Ӧ�Ĺ�������
typedef struct tagCFG_VEHICLE_COMPARE
{
    char                   szRuleName[MAX_NAME_LEN];                                 // ��������,��ͬ����������
    BOOL                   bRuleEnable;                                              // ����ʹ��
    int                    nObjectTypeNum;                                           // ��Ӧ�������͸���
    char                   szObjectTypes[MAX_OBJECT_LIST_SIZE][MAX_NAME_LEN];        // ��Ӧ���������б�
    CFG_ALARM_MSG_HANDLE   stuEventHandler;                                          // ��������
    CFG_TIME_SECTION       stuTimeSection[WEEK_DAY_NUM][MAX_REC_TSECT_EX];           // �¼���Ӧʱ���
    int                    nPtzPresetId;                                             // ��̨Ԥ�õ���   0~65535
    
    int                    nLinkGroupNum;                                            // �������ظ��� 
    CFG_LINKGROUP_INFO     stuLinkGroup[MAX_LINK_GROUP_NUM];                         // �����Ĳ�����
    BYTE                   byReserved[1024];                                         // �����ֽ�
} CFG_VEHICLE_COMPARE;

// �¼�����EVENT_IVS_HIGH_TOSS_DETECT(�߿�������)��Ӧ�Ĺ�������
typedef struct tagCFG_HIGH_TOSS_DETECT_INFO
{
    char                    szRuleName[MAX_NAME_LEN];                               // ��������,��ͬ����������
    BOOL                    bRuleEnable;                                            // ����ʹ��
    int                     nObjectTypeNum;                                         // ��Ӧ�������͸���
    char                    szObjectTypes[MAX_OBJECT_LIST_SIZE][MAX_NAME_LEN];      // ��Ӧ���������б�
    int                     nPtzPresetId;                                           // ��̨Ԥ�õ���   0~65535
    CFG_ALARM_MSG_HANDLE    stuEventHandler;                                        // ��������
    CFG_TIME_SECTION        stuTimeSection[WEEK_DAY_NUM][MAX_REC_TSECT_EX];         // �¼���Ӧʱ���

    UINT					nDetectRegionPoint;										// �����������
	CFG_POLYGON				stuDetectRegion[20];									// �����
    BYTE                    byReserved[4096];                                       // �����ֽ�
} CFG_HIGH_TOSS_DETECT_INFO;

// �״���˶�������
typedef enum tagCFG_EM_RADAR_OBJECT_FILTER_TYPE
{
	CFG_EM_RADAR_OBJECT_FILTER_UNKNOWN,											// δ֪
	CFG_EM_RADAR_OBJECT_FILTER_HUMAN,											// ��
	CFG_EM_RADAR_OBJECT_FILTER_VEHICLE											// ��
}CFG_EM_RADAR_OBJECT_FILTER_TYPE;

// �״ﾯ���߱�������
typedef enum tagCFG_EM_RADAR_ALARM_TYPE
{
	CFG_EM_RADAR_ALARM_UNKNOWN,													// δ֪
	CFG_EM_RADAR_ALARM_ALARM,													// ����
	CFG_EM_RADAR_ALARM_WARNING,													// Ԥ��
	CFG_EM_RADAR_ALARM_SHIELD,													// ����
}CFG_EM_RADAR_ALARM_TYPE;

// Ŀ����������
typedef enum tagCFG_EM_SPEED_LIMIT_TYPE
{
	CFG_EM_SPEED_LIMIT_TYPE_UNKNOWN,											// δ֪
	CFG_EM_SPEED_LIMIT_TYPE_BELOW,												// ����
	CFG_EM_SPEED_LIMIT_TYPE_ABOVE,												// ����
	CFG_EM_SPEED_LIMIT_TYPE_EQUAL,												// ����
}CFG_EM_SPEED_LIMIT_TYPE;

// Ŀ�����ٹ�����
typedef struct CFG_MOVESPEED_FILTER
{
	BOOL							bEnable;								// Ŀ������ʹ��
	CFG_EM_SPEED_LIMIT_TYPE			emSpeedLimitType;						// Ŀ����������
	int								nSpeedValue;							// Ŀ�����٣���λ0.01km/h
	BYTE							byReserved[500];						// Ԥ���ֶ�
}CFG_MOVESPEED_FILTER;

// ���������
typedef struct tagCFG_RADAR_OBJECT_FILTER
{
	CFG_MOVESPEED_FILTER			stuMoveSpeedFilter;						// Ŀ�����ٹ�����
	CFG_EM_RADAR_OBJECT_FILTER_TYPE	emTypes[16];							// �״��������
	int								nTypeNum;								// ����ĸ�����nTypeNumΪ0ʱ��ʾ������
	BYTE							byReserved[992];						// Ԥ���ֶ�
}CFG_RADAR_OBJECT_FILTER;

// �¼�����EVENT_IVS_RADAR_LINE_DETECTION(�״ﾯ����/���߼���¼�)��Ӧ�Ĺ�������
typedef struct tagCFG_RADAR_LINE_DETECTION_INFO
{
    char                    szRuleName[MAX_NAME_LEN];                               // ��������,��ͬ����������
    BOOL                    bRuleEnable;                                            // ����ʹ��
    int                     nObjectTypeNum;                                         // ��Ӧ�������͸���
    char                    szObjectTypes[MAX_OBJECT_LIST_SIZE][MAX_NAME_LEN];      // ��Ӧ���������б�
    CFG_ALARM_MSG_HANDLE    stuEventHandler;                                        // ��������
	CFG_TIME_SECTION        stuTimeSection[WEEK_DAY_NUM][MAX_REC_TSECT_EX];         // �¼���Ӧʱ���
	int                     nPtzPresetId;                                           // ��̨Ԥ�õ���   0~65535

	int						nDetectLineNum;											// �����߶˵����
	CFG_POLYLINE			stuDetectLine[MAX_POLYGON_NUM];							// �����ߡ� �������ͣ�������ÿ���˵�������һ����[0,8192)���䡣
	CFG_RADAR_OBJECT_FILTER stuObjectFilter;										// ���������
	EM_CFG_DIRECTION_TYPE	emDirection;											// ��ⷽ��
	CFG_SIZEFILTER_INFO		stuSizeFilter;											// �����ض��ĳߴ��������Ϊ��߹����жϾ���
	BOOL					bSizeFilterEnable;										// �ߴ������ʹ��
    BYTE                    byReserved[4092];                                       // �����ֽ�
} CFG_RADAR_LINE_DETECTION_INFO;

// ����Խ����
typedef enum tagCFG_EM_CROSS_DIRECTION_TYPE
{
	CFG_EM_CROSS_DIRECTION_UNKNOWN,													// δ֪
	CFG_EM_CROSS_DIRECTION_ENTER,													// ����
	CFG_EM_CROSS_DIRECTION_LEAVE,													// �뿪
	CFG_EM_CROSS_DIRECTION_BOTH,													// �����뿪
}CFG_EM_CROSS_DIRECTION_TYPE;

// �¼�����EVENT_IVS_RADAR_REGION_DETECTION(�״ﾯ��������¼�)��Ӧ�Ĺ�������
typedef struct tagCFG_RADAR_REGION_DETECTION_INFO
{
    char                    szRuleName[MAX_NAME_LEN];                               // ��������,��ͬ����������
    BOOL                    bRuleEnable;                                            // ����ʹ��
    int                     nObjectTypeNum;                                         // ��Ӧ�������͸���
    char                    szObjectTypes[MAX_OBJECT_LIST_SIZE][MAX_NAME_LEN];      // ��Ӧ���������б�
    CFG_ALARM_MSG_HANDLE    stuEventHandler;                                        // ��������
	CFG_TIME_SECTION        stuTimeSection[WEEK_DAY_NUM][MAX_REC_TSECT_EX];         // �¼���Ӧʱ���
	int                     nPtzPresetId;                                           // ��̨Ԥ�õ���   0~65535

	int						nDetectRegionNum;										// �������˵����
	CFG_POLYLINE			stuDetectRegion[MAX_POLYGON_NUM];						// �������� ��������ͣ��������ÿ���˵�������һ����[0,8192)���䡣
	CFG_RADAR_OBJECT_FILTER stuObjectFilter;										// ���������
	CFG_EM_CROSS_DIRECTION_TYPE emDirection;										// ��ⷽ��
	CFG_EM_RADAR_ALARM_TYPE emAlarmType;											// ��������
	EM_VIDEO_ANALYSE_ACTION_TYPE  emActions[MAX_ACTION_LIST_SIZE];					// ֧�ֵļ�����嶯���б�
	int						nActionNum;												// �����б�Ԫ�صĸ���
	BOOL					bSizeFilterEnable;										// �ߴ������ʹ��
	CFG_SIZEFILTER_INFO		stuSizeFilter;											// �����ض��ĳߴ��������Ϊ��߹����жϾ���
    BYTE                    byReserved[4096];                                       // �����ֽ�
} CFG_RADAR_REGION_DETECTION_INFO;

// �¼�����EVENT_IVS_WALK_DETECTION(�߶�����¼�)��Ӧ�Ĺ�������
typedef struct tagCFG_WALK_DETECTION_INFO
{
    char                    szRuleName[MAX_NAME_LEN];                               // ��������,��ͬ����������
    BOOL                    bRuleEnable;                                            // ����ʹ��
    int                     nObjectTypeNum;                                         // ��Ӧ�������͸���
    char                    szObjectTypes[MAX_OBJECT_LIST_SIZE][MAX_NAME_LEN];      // ��Ӧ���������б�
    CFG_ALARM_MSG_HANDLE    stuEventHandler;                                        // ��������
    CFG_TIME_SECTION        stuTimeSection[WEEK_DAY_NUM][MAX_REC_TSECT_EX];         // �¼���Ӧʱ���
    int                     nPtzPresetId;                                           // ��̨Ԥ�õ���   0~65535

    int                     nDetectRegionNum;                                       // �������˵����
    CFG_POLYLINE            stuDetectRegion[MAX_POLYGON_NUM];                       // �������� ��������ͣ��������ÿ���˵�������һ����[0,8192)���䡣
    UINT                    nMinDuration;                                           // ��̳���ʱ��, ��λ s, Ĭ��2����Χ[1,3600]
    UINT                    nSensitivity;                                           // ������������,[1, 10], Ĭ��ֵ5
    int                     nMaxHeight;                                             // �����߶�,��λcm
    int                     nMinHeight;                                             // ��С���߶�,��λcm
    UINT                    nReportInterval;                                        // �ظ��������,Ϊ0���ظ�����, ��λ���룬Ĭ��0����Χ[0,300]

    BYTE                    byReserved[4096];                                       // �����ֽ�
} CFG_WALK_DETECTION_INFO;
  
// �¼����� EVENT_IVS_SMART_KITCHEN_CLOTHES_DETECTION (�ǻ۳������ż��)��Ӧ�Ĺ�������
typedef struct tagCFG_SMART_KITCHEN_CLOTHES_DETECTION_INFO
{
	char                    szRuleName[MAX_NAME_LEN];                               // ��������,��ͬ����������
	BOOL                    bRuleEnable;                                            // ����ʹ��
	int                     nObjectTypeNum;                                         // ��Ӧ�������͸���
	char                    szObjectTypes[MAX_OBJECT_LIST_SIZE][MAX_NAME_LEN];      // ��Ӧ���������б�
	CFG_ALARM_MSG_HANDLE    stuEventHandler;                                        // ��������
	CFG_TIME_SECTION        stuTimeSection[WEEK_DAY_NUM][MAX_REC_TSECT_EX];         // �¼���Ӧʱ���
	int                     nPtzPresetId;                                           // ��̨Ԥ�õ���   0~65535

	BOOL					bMaskEnable;											// �Ƿ������ּ��  ��TRUE:���� FALSE:�رգ�				
	BOOL					bChefHatEnable;											// �Ƿ�����ʦñ��⣨TRUE:���� FALSE:�رգ�
	BOOL					bChefClothesEnable;										// �Ƿ�����ʦ����⣨TRUE:���� FALSE:�رգ�
	EM_SUPPORTED_COLOR_LIST_TYPE	emChefClothesColors[8];							// ������Ҫ��ĳ�ʦ����ɫ������
	int						nChefClothesColorsNum;									// ���ϼ�ⲻ��Ҫ�����ĳ�ʦ����ɫ����
	UINT					nReportInterval;										// ����ʱ����,��λ�� [0,600] Ĭ��30,0��ʾ���ظ����� 
	BYTE                    byReserved[4096];                                       // �����ֽ�
} CFG_SMART_KITCHEN_CLOTHES_DETECTION;


// �¼�����EVENT_IVS_BACK_TO_DETECTION(���Լ���¼�)��Ӧ�Ĺ�������
typedef struct tagCFG_BACK_TO_DETECTION_INFO
{
    char                    szRuleName[MAX_NAME_LEN];                               // ��������,��ͬ����������
    BOOL                    bRuleEnable;                                            // ����ʹ��
    int                     nObjectTypeNum;                                         // ��Ӧ�������͸���
    char                    szObjectTypes[MAX_OBJECT_LIST_SIZE][MAX_NAME_LEN];      // ��Ӧ���������б�
    CFG_ALARM_MSG_HANDLE    stuEventHandler;                                        // ��������
    CFG_TIME_SECTION        stuTimeSection[WEEK_DAY_NUM][MAX_REC_TSECT_EX];         // �¼���Ӧʱ���
    int                     nPtzPresetId;                                           // ��̨Ԥ�õ���   0~65535

    int                     nDetectRegionNum;                                       // �������˵����
    CFG_POLYLINE            stuDetectRegion[MAX_POLYGON_NUM];                       // �������� ��������ͣ��������ÿ���˵�������һ����[0,8192)���䡣
    UINT                    nMinDuration;                                           // ��̳���ʱ��, ��λ s, Ĭ��2����Χ[1,3600]
    UINT                    nSensitivity;                                           // ������������,[1, 10], Ĭ��ֵ5
    int                     nMaxHeight;                                             // �����߶�,��λcm
    int                     nMinHeight;                                             // ��С���߶�,��λcm
    UINT                    nReportInterval;                                        // �ظ��������,Ϊ0���ظ�����, ��λ���룬Ĭ��0����Χ[0,300]
    BYTE                    byReserved[4096];                                       // �����ֽ�
} CFG_BACK_TO_DETECTION_INFO;

// ϵͳ������дģʽ
typedef enum tagEM_CLOSE_UP_MODE
{
    EM_CLOSE_UP_MODE_UNKNOWN,                               // δ֪
    EM_CLOSE_UP_MODE_NO_LINKAGE,                            // ������
    EM_CLOSE_UP_MODE_TRACK_MODE,                            // ����ģʽ
    EM_CLOSE_UP_MODE_FIXED_MODE,                            // �̶�ģʽ
    EM_CLOSE_UP_MODE_DESIGNED_REGION_MODE,                  // ָ������ģʽ
} EM_CLOSE_UP_MODE;

// ������������
typedef enum tagEM_LINKAGE_STREAM_TYPE
{
    EM_LINKAGE_STREAM_TYPE_UNKNOWN,                         // δ֪
    EM_LINKAGE_STREAM_TYPE_MAIN,                            // ������
    EM_LINKAGE_STREAM_TYPE_EXTRA_1,                         // ������1
    EM_LINKAGE_STREAM_TYPE_EXTRA_2,                         // ������2
    EM_LINKAGE_STREAM_TYPE_EXTRA_3,                         // ������3
} EM_LINKAGE_STREAM_TYPE;

// �¼�����EVENT_IVS_WRITE_ON_THE_BOARD_DETECTION(�������¼�)��Ӧ�Ĺ�������
typedef struct tagCFG_WRITE_ON_THE_BOARD_DETECTION_INFO
{
    char                    szRuleName[MAX_NAME_LEN];                               // ��������,��ͬ����������
    BOOL                    bRuleEnable;                                            // ����ʹ��
    int                     nObjectTypeNum;                                         // ��Ӧ�������͸���
    char                    szObjectTypes[MAX_OBJECT_LIST_SIZE][MAX_NAME_LEN];      // ��Ӧ���������б�
    CFG_ALARM_MSG_HANDLE    stuEventHandler;                                        // ��������
    CFG_TIME_SECTION        stuTimeSection[WEEK_DAY_NUM][MAX_REC_TSECT_EX];         // �¼���Ӧʱ���
    int                     nPtzPresetId;                                           // ��̨Ԥ�õ���   0~65535

    int                     nDetectRegionNum;                                       // �������˵����
    CFG_POLYLINE            stuDetectRegion[MAX_POLYGON_NUM];                       // �������� ��������ͣ��������ÿ���˵�������һ����[0,8192)���䡣
    UINT                    nMinDuration;                                           // ��̳���ʱ��, ��λ s, Ĭ��2����Χ[1,3600]
    UINT                    nSensitivity;                                           // ������������,[1, 10], Ĭ��ֵ5
    int                     nMaxHeight;                                             // �����߶�,��λcm
    int                     nMinHeight;                                             // ��С���߶�,��λcm
    EM_CLOSE_UP_MODE        emCloseUpMode;                                          // ϵͳ������дģʽ, ΪUNKNOWNʱ���·�
    int                     nChannelID;                                             // ������ͨ����
    EM_LINKAGE_STREAM_TYPE  emStreamType;                                           // ������������
    UINT                    nDesignedRegionPoint;                                   // �����������
    CFG_POLYGON             stuDesignedRegion[2];                                   // �����
    BYTE                    byReserved[4096];                                       // �����ֽ�
} CFG_WRITE_ON_THE_BOARD_DETECTION_INFO;

// ����ͨ����Ϣ
typedef struct tagCFG_RULE_COMM_INFO
{
	BYTE				bRuleId;							// ������
	EM_SCENE_TYPE		emClassType;						// ���������ĳ���
	BYTE				bReserved[512];						// �����ֽ�
}CFG_RULE_COMM_INFO;

typedef struct tagCFG_RULE_INFO
{
	DWORD				dwRuleType;							// �¼����ͣ����dhnetsdk.h��"���ܷ����¼�����"
	int					nRuleSize;							// ���¼����͹������ýṹ���С
	CFG_RULE_COMM_INFO  stuRuleCommInfo;					// ����ͨ����Ϣ
} CFG_RULE_INFO;

// ÿ����Ƶ����ͨ����Ӧ�������¼����򣺻�����pRuleBuf������¼�������Ϣ��ÿ���¼�������Ϣ����ΪCFG_RULE_INFO+"�¼����Ͷ�Ӧ�Ĺ������ýṹ��"��
typedef struct tagCFG_ANALYSERULES_INFO
{
	int					nRuleCount;							// �¼��������
	char*				pRuleBuf;							// ÿ����Ƶ����ͨ����Ӧ����Ƶ�����¼��������û���
	int					nRuleLen;							// �����С

} CFG_ANALYSERULES_INFO;

// ��Ƶ������Դ����
typedef enum tagCFG_VIDEO_SOURCE_TYPE
{
	CFG_VIDEO_SOURCE_REALSTREAM,                            // ʵʱ��
	CFG_VIDEO_SOURCE_FILESTREAM,                            // �ļ���
}CFG_VIDEO_SOURCE_TYPE;

// ����Դ�ļ�����
typedef enum tagCFG_SOURCE_FILE_TYPE
{
	CFG_SOURCE_FILE_UNKNOWN,                                // δ֪����
	CFG_SOURCE_FILE_RECORD,                                 // ¼���ļ�
	CFG_SOURCE_FILE_PICTURE,                                    // ͼƬ�ļ�
}CFG_SOURCE_FILE_TYPE;

// ��Ƶ����Դ�ļ���Ϣ
typedef struct tagCFG_SOURCE_FILE_INFO
{
	char                szFilePath[MAX_PATH];               // �ļ�·��
	CFG_SOURCE_FILE_TYPE emFileType;                        // �ļ����ͣ����CFG_SOURCE_FILE_TYPE
}CFG_SOURCE_FILE_INFO;

// ÿ����Ƶ����ͨ����Ӧ����Ƶ������Դ������Ϣ
typedef struct tagCFG_ANALYSESOURCE_INFO
{
	bool				bEnable;								// ��Ƶ����ʹ��
	BYTE				bReserved[3];							// ��������
	int					nChannelID;								// ���ܷ�����ǰ����Ƶͨ����
	int					nStreamType;							// ���ܷ�����ǰ����Ƶ�������ͣ�0:ץͼ����; 1:������; 2:������1; 3:������2; 4:������3; 5:������
	char				szRemoteDevice[MAX_NAME_LEN];			// �豸��
	BOOL				abDeviceInfo;				        	// �豸��Ϣ�Ƿ���Ч
	AV_CFG_RemoteDevice stuDeviceInfo;					        // �豸��Ϣ
	CFG_VIDEO_SOURCE_TYPE emSourceType;                         // ��Ƶ����Դ���ͣ����CFG_VIDEO_SOURCE_TYPE
	CFG_SOURCE_FILE_INFO stuSourceFile;                          // ����Ƶ����Դ����ΪCFG_VIDEO_SOURCE_FILESTREAMʱ����Ч
} CFG_ANALYSESOURCE_INFO;

//��Ƶ�����������ã�Ӱ���豸�������������Ϊ
typedef struct tagCFG_ANALYSEWHOLE_INFO
{	
	unsigned  int nVideoChannelType;		// ��Ƶͨ������ѡ�� 0:���֣�1;ģ�⣬2:���ֺ�ģ��
	
}CFG_ANALYSEWHOLE_INFO;




typedef struct tagCFG_RAINBRUSH_INFO
{
	bool                bEnable;                                 // ��ˢʹ��
	BYTE                bSpeedRate;                              // ��ˢ�ٶ�,1:����;2:����;3:����
	BYTE                bReserved[2];                            // ��������
	CFG_TIME_SECTION	stuTimeSection[WEEK_DAY_NUM][MAX_REC_TSECT];// �¼���Ӧʱ���
	int					nInterval;									// ��ˢ�˶�����¼�, ��λ: ��
	BOOL				bProtectEnable;								// ��ˢ����ʹ��: true ��������, false �����ر�
	int					nProtectTime;								// ����ʱ��, ��λ: ��
}CFG_RAINBRUSH_INFO;

// BreakingSnapTimes
typedef struct tagBREAKINGSNAPTIMES_INFO
{
	int               nNormal;                          // ����
	int               nRunRedLight;                     // �����
	int			      nOverLine;						// ѹ��
	int				  nOverYellowLine;			  	    // ѹ����
	int				  nRetrograde;			    	    // ����
	int				  nUnderSpeed;					    // Ƿ��
    int				  nOverSpeed;						// ����
	int			      nWrongRunningRoute;				// �г�ռ��
	int			      nYellowInRoute;					// ����ռ��
	int		    	  nSpecialRetrograde;				// ��������
	int			      nTurnLeft;						// Υ����ת
	int			      nTurnRight;						// Υ����ת
	int			      nCrossLane;						// Υ�±��
	int			      nU_Turn;						    // Υ�µ�ͷ
	int			      nParking;						    // Υ��ͣ��
	int               nWaitingArea;						// Υ�½��������
	int			      nWrongRoute;					    // ����������ʻ		
	int               nParkingSpaceParking;             // ��λ�г�
	int               nParkingSpaceNoParking;           // ��λ�޳�

	int               nRunYellowLight;					// ���Ƶ�
	int               nStay;							// Υ��ͣ��
	int               nPedestrainPriority;	            // ��������������Υ��
	int               nVehicleInBusRoute;               // Υ��ռ��
	int               nBacking;                         // Υ�µ���
	int				  nOverStopLine;					// ѹֹͣ��
	int               nParkingOnYellowBox;           	// ��������ͣ��	
	int               nRestrictedPlate;					// ���޳���	
	int               nNoPassing;						// ����	
    int               nWithoutSafeBelt;                 // ��ϵ��ȫ��
    int               nDriverSmoking;                   // ��ʻԱ����
    int               nDriverCalling;                   // ��ʻԱ��绰
    int               nPedestrianRunRedLight;           // ���˴����
    int               nPassNotInOrder;                  // δ���涨����ͨ��
}BREAKINGSNAPTIMES_INFO;


//OSD�ڱ�
typedef struct tagBLACK_REGION_INFO
{
	int nHeight;			//�ڱ߸߶� ȡֵ��Χ��0 ~ ( 8192-ԭͼƬ�߶�)
	int nOSDPosition;		//�ڱ�λ�� 0=δ֪ , 1=���� , 2=�ײ�
}BLACK_REGION_INFO;

#define MAX_OSD_CUSTOM_SORT_NUM       8
#define MAX_OSD_CUSTOM_SORT_ELEM_NUM  8
#define MAX_OSD_CUSTOM_GENERAL_NUM    8
#define MAX_OSD_ITEM_ATTR_NUM         8 
#define MAX_PRE_POX_STR_LEN           32
#define MAX_OSD_CUSTOM_NAME_LEN       32
#define MAX_OSD_CUSTOM_VALUE_LEN      256

//ȫ��OSD�������
typedef struct tagOSD_WHOLE_ATTR
{
    BOOL        bPositionAsBlackRegion;     //λ���Ƿ�ͬ�ڱ���ͬ,trueʱ�������Position��Ч
    CFG_RECT    stPostion;                  //λ��,���ܳ���ͼƬ��Χ
    BOOL        bNewLine;                   //�������η�Χ�Ƿ���,bPositionAsBlackRegionΪtrueʱ��Ч
    BOOL        bLoneVehicle;                //������Ϣ������ʾ,true һ����ʾһ������Ϣ,false �����������Ϣ��ʾ��һ��
}OSD_WHOLE_ATTR;

typedef union tagOSD_ATTR_SCHEME
{
    OSD_WHOLE_ATTR stWholeAttr;
}OSD_ATTR_SCHEME;

//OSD�������Ԫ��
typedef struct tagOSD_CUSTOM_ELEMENT
{
    int  nNameType;                          //��������,	0:szName�ֶκ������szOSDOrder�ֶζ������
                                             //             1:"Name"�ֶα�ʾ�Զ�����������
    char szName[MAX_OSD_CUSTOM_VALUE_LEN];                           // ��������
    char szPrefix[MAX_PRE_POX_STR_LEN];      // ����ǰ׺�ַ���	
    char szPostfix[MAX_PRE_POX_STR_LEN];     //���Ӻ�׺�ַ���
    int  nSeperaterCount;                    //������ӷָ�������
    
}OSD_CUSTOM_ELEMENT;

//OSD���������Զ�������
typedef struct tagOSD_CUSTOM_SORT
{
    OSD_CUSTOM_ELEMENT   stElements[MAX_OSD_CUSTOM_SORT_ELEM_NUM];     //�������Ԫ��
    int                  nElementNum;
}OSD_CUSTOM_SORT;


typedef struct tagOSD_CUSTOM_GENERAL_INFO
{
    BOOL    bEnable;            //�Ƿ����
}OSD_CUSTOM_GENERAL_INFO;

//OSD�Զ�����
typedef struct tagOSD_CUSTOM_INFO
{
    OSD_CUSTOM_GENERAL_INFO  stGeneralInfos[MAX_OSD_CUSTOM_GENERAL_NUM];     //�������Ԫ��
    int                      nGeneralInfoNum;
}OSD_CUSTOM_INFO;

#define  MAX_CONF_CHAR 256
//OSD����
typedef struct tagOSD_INFO
{
	BLACK_REGION_INFO	stBackRegionInfo;		//OSD�ڱ�����
	int					nOSDAttrScheme;		    //OSD�������÷��� 0=δ֪ , 1=ȫ��OSD������� , 2=ÿ��OSD��һ������
	OSD_ATTR_SCHEME		stOSDAttrScheme;		//OSD�������÷�������
    OSD_CUSTOM_SORT     stOSDCustomSorts[MAX_OSD_CUSTOM_SORT_NUM];    //OSD���������Զ�������
    int                 nOSDCustomSortNum;
    int                 nRedLightTimeDisplay;   //OSD���ʱ������ 0=δ֪,1=Υ�����һ��,2=������
    char                cSeperater;             //OSD��ͬ��֮��ķָ���
    BYTE		        bReserved[3];           //�ֽڶ���
    char                szOSDOrder[MAX_CONF_CHAR];    
    int                 nOSDContentScheme;      //0=δ֪, 1=Mask , 2=CustomizeSort
    OSD_CUSTOM_INFO     stOSDCustomInfo;        //OSD�Զ�����
}OSD_INFO;

// Detector -- BEGIN
// CoilConfig
typedef struct tagCOILCONFIG_INFO
{
    int               nDelayFlashID;                 // ��ʱ��������	ÿ����Ȧ��Ӧ����ʱ���ص���ţ���Χ0~5��0��ʾ����ʱ�κ������
    int               nFlashSerialNum;               // ��������	��Χ0~5��0��ʾ��������ƣ�۴����Ŀ�ã�
    int               nRedDirection;                 // ��Ʒ���	ÿ����Ȧ��Ӧ�ĺ�Ʒ���0-������,1-��ת���,2-ֱ�к��,3-��ת���,4-����,5-��ֱ,6-����, ֻ�ڵ羯����Ч
    int               nTriggerMode ;                 // ��Ȧ����ģʽ	����ģʽ��0-����Ȧ����1-����Ȧ������۴����Ŀ�ã�
    int				  nFlashSerialNum2;				 //��ץ�ڶ��Ŷ�Ӧ�������� ��Χ0~5��0��ʾ���������
    int				  nFlashSerialNum3;				 //��ץ�����Ŷ�Ӧ�������� ��Χ0~5��0��ʾ���������
}COILCONFIG_INFO;

// ��Ȧӳ��
typedef struct tagCOIL_MAP_INFO
{
    UINT        nPhyCoilID;         // ������Ȧ��
    UINT        nLogicCoilID;       // �߼���Ȧ��
}COIL_MAP_INFO;

typedef struct tagDETECTOR_INFO
{
    int                nDetectBreaking;                  // Υ����������	�ӵ�λ����λ�����ǣ�0-����1-�����2-ѹ��3-����4-Ƿ��5-����6-�г�ռ��
                                                         // 7-����ռ�� 8-���Ƶ� 9-Υ��ռ�������� 10-��ϵ��ȫ�� 11-��ʻԱ���� 12-��ʻԱ��绰

	COILCONFIG_INFO    arstCoilCfg[MAX_COILCONFIG];      // ��Ȧ��������
	int				   nRoadwayNumber;				     // ������	1-16 ; 0��ʾ������
	int                nRoadwayDirection;                // �������򣨳������ķ���	0-���� 1-�����򶫱� 2-�� 3-�������� 4-������ 5-���������� 6-������ 7-���������� 8-����
	int                nRedLightCardNum;                 // ����ͼƬ���	��ʾ���羯�е�ĳһ��ͼƬ��Ϊ����ͼƬ�������ţ���0��ʾ�����ã�1~3,��ʾ���ö�Ӧ��ŵ�ͼƬ
	int                nCoilsNumber;                     // ��Ȧ����	1-3
	int                nOperationType;                   // ҵ��ģʽ	0-���ڵ羯1-�羯2-����
	int                arnCoilsDistance[3];              // ������Ȧ�ļ��	��Χ0-1000����λΪ����
	int                nCoilsWidth;                      // ÿ����Ȧ�Ŀ��	0~200cm
	int                arnSmallCarSpeedLimit[2];         // С�ͳ����ٶ����޺�����	0~255km/h�������ô�С������ʱ��Ϊ��ͨ��������
	int				   arnBigCarSpeedLimit[2];           // ���ͳ����ٶ����޺�����	0~255km/h�����ô�С������ʱ��Ч
	int				   nOverSpeedMargin;			     // �޸��ٿ���ֵ	��λ��km/h
	int                nBigCarOverSpeedMargin;           // ���޸��ٿ���ֵ	��λ��km/h�����ô�С������ʱ��Ч
	int				   nUnderSpeedMargin;			     // �޵��ٿ���ֵ	��λ��km/h
	int                nBigCarUnderSpeedMargin;          // ���޵��ٿ���ֵ	��λ��km/h�����ô�С������ʱ��Ч
	bool               bSpeedLimitForSize;               // �Ƿ����ô�С������
	bool			   bMaskRetrograde;				     // �����Ƿ���ΪΥ����Ϊ
	BYTE               byReserved[2];                    // ��������
	char               szDrivingDirection[3][MAX_DRIVINGDIRECTION]; //
													     // "DrivingDirection" : ["Approach", "�Ϻ�", "����"],��ʻ����
													     // "Approach"-���У����������豸�����Խ��Խ����"Leave"-���У�
														 // ���������豸�����Խ��ԽԶ���ڶ��͵����������ֱ�������к�
														 // ���е������ص㣬UTF-8����
	int                nOverPercentage;                  // ���ٰٷֱȣ��������ٰٷֱȺ�ץ��
	int                nCarScheme;                       // ����ķ���Index,���巽������βο��������local.png;����CFG_TRAFFICSNAPSHOT_INFO.abSchemeRange�ֶ��������÷�Χ
	int                nSigScheme;                       // ͬ�ϣ��ǿ���ʹ��
	BOOL               bEnable;                          // �����Ƿ���Ч��ֻ����Чʱ��ץ��

	int				   nYellowSpeedLimit[2];			 //���Ƴ��������޺����� ��Χ0~255km/h
	int				   nRoadType;						 //����·�� 0 ��ͨ��· 1 ���ٹ�·
	int				   nSnapMode;						 //ץ��ģʽ 0-ȫ��ץ�� 1-����ץ�� 2-����ץ�ģ�۴����Ŀ�ã�
    int                nDelayMode;						 //��ʱץ�ķ��� 0-ʹDelaySnapshotDistance��1-ʹ��DelayTime
	int                nDelayTime;						 //��ʱץ��ʱ�� ����Ƶ�����ץ��λ�þ������һ����Ȧ��ʱ�䣬��λ����
	int				   nTriggerMode;					 //����ģʽ 0-����Ȧ���� 1-����Ȧ���� 2-���붼ץ�� 3-�ر�
	int			       nErrorRange;						 //�ٶ����ֵ������Ȧ2�����Ȧ3֮����ٶ����ֵ����ʵ�������ڻ���ڸ�ֵ�����ٶ���Ч������ȡƽ���ٶ� 0-20
	double			   dSpeedCorrection;				 //�ٶ�У��ϵ�������ٶ�ֵΪ�����ֵ���Ը�ϵ�� 
    int                nDirection[2];                    //��Գ���������Ҫ�ϱ�������ʻ����,nDirection[0] 0--�� 1--���� ; nDirection[1] 0--�� 1--����	
    char               szCustomParkNo[CFG_COMMON_STRING_32 + 1]; // �Զ��峵λ�ţ�ͣ�����ã�
    BYTE               btReserved[3];
    int                nCoilMap;                        // �ж��ٶ���Ȧӳ���ϵ
    COIL_MAP_INFO      stuCoilMap[16];                  // ��Ȧ��ӳ���ϵ
}DETECTOR_INFO;
// Detector -- END

typedef struct tagRADAR_INFO
{
    int     nAngle;                 //�Ƕ�,���������״�̽ͷ��װ�ĽǶ���ɵ��ٶ����,��Χ[0,90]
    int     nAntiJammingValue;      //�������ż�ֵ
    int     nComeInValue;           //��������ż�ֵ,ȡֵ��Χ[0,65535]
    int     nComeOutValue;          //�����뿪�ż�ֵ
    int     nDelayTime;             //�״���ʱ,��λms����Χ[0,255]
    DWORD   nDetectBreaking;        //Υ����������,�ӵ�λ����λ������:
                                    //0-����,1-�����, 2-ѹ��, 3-����,4-Ƿ��
                                    //5-����,6-�г�ռ��,7-����ռ��,8-���Ƶ�,9-Υ��ռ��������
    int     nDetectMode;            //���ģʽ  0-��ͷ��� 1-��β��� 2-˫����
    int     nInstallMode;           //�״ﰲװ��ʽ  0-���氲װ 1-�Ϸ���װ
    int     nLevel;                 //������,0�����������,��Χ[0,5]
    int     nMultiTargetFilter;     //��Ŀ�����ģʽ,0-���� 1-����
    int     nWentEndValue;          //ȥ���źŽ����ż�ֵ
    int     nWentInValue;           //ȥ������ż�ֵ
    int     nWentOutValue;          //ȥ���뿪�ż�ֵ
}RADAR_INFO;

// CFG_CMD_INTELLECTIVETRAFFIC
typedef struct tagCFG_TRAFFICSNAPSHOT_INFO
{
	char                     szDeviceAddress[MAX_DEVICE_ADDRESS];  // �豸��ַ	UTF-8���룬256�ֽ�
	DWORD                    nVideoTitleMask;                      // OSD������������	�ӵ�λ����λ�ֱ��ʾ��0-ʱ�� 1-�ص� 2-����3-���� 4-���� 5-����6-������ 7-С������8-���� 9-Υ������10-������ 11-������ɫ 12-�������� 13-������ɫ14-��Ƶ���ʱ�� 15-Υ������ 16-�״﷽�� 17-�豸��� 18-�궨����ʱ�� 19-���� 20-��ʻ����
	int                      nRedLightMargin;                      // ���������ʱ��	��ƿ�ʼ��һ��ʱ���ڣ�����ͨ�в��㴳��ƣ���λ����
	float				     fLongVehicleLengthLevel;              // ������������С��ֵ	��λ���ף�����
	float					 arfLargeVehicleLengthLevel[2];        // �󳵳�����ֵ	��λ���ף�����Сֵ
	float					 arfMediumVehicleLengthLevel[2];       // ���ͳ�������ֵ	��λ���ף�����Сֵ
	float					 arfSmallVehicleLengthLevel[2];        // С��������ֵ	��λ���ף�����Сֵ
	float					 fMotoVehicleLengthLevel;              // Ħ�г����������ֵ	��λ���ף�������
	BREAKINGSNAPTIMES_INFO   stBreakingSnapTimes;                  // Υ��ץ������
	DETECTOR_INFO            arstDetector[MAX_DETECTOR];           // ���������ã��±��ǳ�����
	int 					 nCarType;			               	   // ץ�ĳ�������	0-��С����ץ��1-ץ��С��2-ץ�Ĵ�3-��С������ץ��
	int						 nMaxSpeed;			               	   // ����õ��ٶȳ�������ٶ�ʱ����������ٶȼ�	0~255km/h
	int						 nFrameMode;		               	   // ֡���ģʽ	1-�ٶ�����Ӧ�������ٶ�����ȡ0����������ٶ�����ȡ2������м�ȡ1�����2-��������������
	int                      arnAdaptiveSpeed[2];                  // �ٶ�����Ӧ���޺�����
	CFG_ALARM_MSG_HANDLE     stuEventHandler;	                   // ��ͨץ����������
	BOOL                     abSchemeRange;                        // TRUE:����������,�Գ���0��ֵΪ׼;FALSE:������Գ������������޸Ĵ��ֶ�����, ֻ�ڲ�ʹ��

    DWORD                    nVideoTitleMask1;					   // �ӵ�λ����λ�ֱ��ʾ��0-���� 1-��ƽ���ʱ�� 2-�豸���쳧�� 3-С�������� 4-�󳵵����� 5-С�������� 6-�󳵸����� 7-�豸����ģʽ 8-ͨ���Զ��� 9-�����Զ��� 10-ץ�Ĵ���Դ 11-ͣ��������12-��������(��������γ��ȵ�) 13-�г������� 14-�г������� 15-��·���� 16-GPS��Ϣ
    DWORD				     nMergeVideoTitleMask;                 // �ϳ�ͼƬOSD������������	����nVideoTitleMask�ֶ�
    DWORD				     nMergeVideoTitleMask1;				   // �ϳ�ͼƬOSD������������	����nVideoTitleMask1�ֶ�
    int                      nTriggerSource;					   // ����Դ���� 0-RS232 1-RS485 2-IO 3-Video 4-Net
    int						 nSnapMode;							   // ץ��ģʽ 0-ȫ��ץ�� 1-����ץ�� 2-����ץ�� 3-PKģʽ
    int                      nWorkMode;							   // ����ģʽ 0-�Զ�ģʽ��1-��Ȧץ��ģʽ��2-��Ȧץ��ʶ��3-��Ƶץ�ģ�4-��Ƶʶ��, 5-���ץ�ģ���ʶ��
    int                      nCarThreShold;						   // ������ֵ  ���ִ�С��������ֵ����λ: cm
    int                      nSnapType;							   // ץ�Ļ�ץ¼ѡ�� 0-����ץ��ģʽ 1-��Ƶץ��ģʽ 2-������ץģʽ
    int                      nCustomFrameInterval[3];			   // �Զ���ץ��֡��� ��һ��Ԫ��ָ����С���ٶ�����Ӧ����ʱ��ץ��֡�������������
    int                      nKeepAlive;				           // ���״��������Ĭ�ϱ������� ��λ��
    OSD_INFO				 stOSD;								   // ԭʼͼƬOSD��������
    OSD_INFO                 stMergeOSD;                           // �ϳ�ͼƬOSD��������
    CFG_NET_TIME             stValidUntilTime;					   // �궨����ʱ�䣬ָ��ʱ���֮ǰץ����Ƭ��Ч
    RADAR_INFO               stRadar;
    char                     szRoadwayCode[MAX_ROADWAYNO];         // ��·����
    DWORD                    nVideoTitleMask2;					   // ԭʼͼƬOSD������������2 �ӵ�λ����λ�ֱ��ʾ��0-���� 1-β������    
    DWORD				     nMergeVideoTitleMask2;				   // �ϳ�ͼƬOSD������������2 ����nVideoTitleMask2�ֶ�
	int                      nParkType;                            // ��������ͣ�0-Ĭ��( ������ǰ�������ֳ���/��� )��1-�������� 2-�������
}CFG_TRAFFICSNAPSHOT_INFO;

typedef struct tagCFG_TRAFFICSNAPSHOT_NEW_INFO
{
	int							nCount;								// ��Ч��Ա����
	CFG_TRAFFICSNAPSHOT_INFO	stInfo[8];							// ��ͨץ�ı�����
}CFG_TRAFFICSNAPSHOT_NEW_INFO;

// ��ͨץ��(CFG_CMD_TRAFFICSNAPSHOT_MULTI_EX)
typedef struct tagCFG_TRAFFICSNAPSHOT_NEW_EX_INFO
{
	CFG_TRAFFICSNAPSHOT_INFO*	pstInfo;							// ��ͨץ�ı�ָ�룬�û����䣬��СΪsizeof(CFG_TRAFFICSNAPSHOT_INFO) * nMaxInfoNum
	int							nMaxInfoNum;						// �û�����CFG_TRAFFICSNAPSHOT_INFO��Ա����
	int							nRetInfoNum;						// ��ȡ����ʱ��ʾ��ȡ��CFG_TRAFFICSNAPSHOT_INFO��Ա����
}CFG_TRAFFICSNAPSHOT_NEW_EX_INFO;

typedef struct tagCFG_DATA_TIME
{
    DWORD				dwYear;					// ��
    DWORD				dwMonth;				// ��
    DWORD				dwDay;					// ��
    DWORD				dwHour;					// ʱ
    DWORD				dwMinute;				// ��
    DWORD				dwSecond;				// ��
    DWORD               dwReserved[2];          // �����ֶ�
} CFG_DATA_TIME;

// ��ͨ���� (CFG_CMD_DEV_GENERRAL) General 
typedef struct tagCFG_DEV_DISPOSITION_INFO
{
	int                      nLocalNo;              // ������ţ���Ҫ����ң�������ֲ�ͬ�豸	0~998
	char                     szMachineName[256];    // �������ƻ���
	char                     szMachineAddress[256]; // ��������ص㼴��·����
	char                     szMachineGroup[256];   // �����������豸������λ	Ĭ��Ϊ�գ��û����Խ���ͬ���豸��Ϊһ�飬���ڹ������ظ���
	char					 szMachineID[64];		// �������, ����ƽ̨��Ψһ
	int						 nLockLoginTimes;		// ��½ʧ�ܿɳ��Դ���
	int						 nLoginFailLockTime;    // ��½ʧ������ʱ��
	BOOL					 bLockLoginEnable;		// ��½ʧ�ܿɳ��Դ���ʹ��
    CFG_DATA_TIME            stuActivationTime;     // ����ʱ��
	BYTE		             bReserved[916];        // �����ֽ�
}CFG_DEV_DISPOSITION_INFO;

typedef struct tagOVERSPEED_INFO
{
	int                      nSpeedingPercentage[2];                        // ���ٰٷֱ�����Ҫ�����䲻���ص�����ЧֵΪ0,����,-1��-1��ʾ�����ֵ
	                                                                        // �����Ƿ�٣�Ҫ�����䲻���ص�����ЧֵΪ0,����,-1��-1��ʾ�����ֵ��Ƿ�ٰٷֱȵļ��㷽ʽ���޵���-ʵ�ʳ���/�޵���
	char                     szCode[MAX_VIOLATIONCODE];                     // Υ�´���
	char                     szDescription[MAX_VIOLATIONCODE_DESCRIPT];     // Υ������

}CFG_OVERSPEED_INFO;

typedef CFG_OVERSPEED_INFO CFG_OVERSPEED_HIGHWAY_INFO;
typedef CFG_OVERSPEED_INFO CFG_UNDERSPEED_INFO;

//ViolationCode Υ�´������ñ�
typedef struct tagVIOLATIONCODE_INFO
{
    char                szRetrograde[MAX_VIOLATIONCODE];			        // ����
	char                szRetrogradeDesc[MAX_VIOLATIONCODE_DESCRIPT];              // Υ��������Ϣ
    char                szRetrogradeShowName[MAX_VIOLATIONCODE_DESCRIPT]; //��ʾ����

	char				szRetrogradeHighway[MAX_VIOLATIONCODE];		// ����-���ٹ�·
	char				szRetrogradeHighwayDesc[MAX_VIOLATIONCODE_DESCRIPT];		// Υ��������Ϣ

	char                szRunRedLight[MAX_VIOLATIONCODE];			// �����
	char                szRunRedLightDesc[MAX_VIOLATIONCODE_DESCRIPT];			   // Υ��������Ϣ

	char                szCrossLane[MAX_VIOLATIONCODE];				// Υ�±��
	char                szCrossLaneDesc[MAX_VIOLATIONCODE_DESCRIPT];			   // Υ��������Ϣ
    char                szCrossLaneShowName[MAX_VIOLATIONCODE_DESCRIPT];    //Υ�±����ʾ����

	char                szTurnLeft[MAX_VIOLATIONCODE];				// Υ����ת
	char                szTurnLeftDesc[MAX_VIOLATIONCODE_DESCRIPT];				   // Υ��������Ϣ

	char                szTurnRight[MAX_VIOLATIONCODE];				// Υ����ת
	char                szTurnRightDesc[MAX_VIOLATIONCODE_DESCRIPT];				// Υ��������Ϣ

	char                szU_Turn[MAX_VIOLATIONCODE];				// Υ�µ�ͷ
	char                szU_TurnDesc[MAX_VIOLATIONCODE_DESCRIPT];				   // Υ��������Ϣ
    char                szU_TurnShowName[MAX_VIOLATIONCODE_DESCRIPT]; //��ʾ��Ϣ

	char                szJam[MAX_VIOLATIONCODE];					// ��ͨӵ��
	char                szJamDesc[MAX_VIOLATIONCODE_DESCRIPT];					   // Υ��������Ϣ

	char                szParking[MAX_VIOLATIONCODE];				// Υ��ͣ��
	char                szParkingDesc[MAX_VIOLATIONCODE_DESCRIPT];		 	      	// Υ��������Ϣ
    char                szParkingShowName[MAX_VIOLATIONCODE_DESCRIPT];   //Υ��ͣ����ʾ����

	// ���� �� ���ٱ��� ֻ���ұ�����һ������
	char                szOverSpeed[MAX_VIOLATIONCODE];				// ����
	char                szOverSpeedDesc[MAX_VIOLATIONCODE_DESCRIPT];				// Υ��������Ϣ
	CFG_OVERSPEED_INFO  stOverSpeedConfig[5];                       // ���ٱ�������

	// ����(���ٹ�·) �� ���ٱ���(���ٹ�·) ֻ���ұ�����һ������
	char                szOverSpeedHighway[MAX_VIOLATIONCODE];		// ����-���ٹ�·
	char                szOverSpeedHighwayDesc[MAX_VIOLATIONCODE_DESCRIPT];	      	// ����-Υ��������Ϣ
	CFG_OVERSPEED_HIGHWAY_INFO stOverSpeedHighwayConfig[5];         // ���ٱ�������

	// Ƿ�� �� Ƿ�ٱ��� ֻ���ұ�����һ������
	char                szUnderSpeed[MAX_VIOLATIONCODE];	        // Ƿ��
	char                szUnderSpeedDesc[MAX_VIOLATIONCODE_DESCRIPT];	            // Υ��������Ϣ
	CFG_UNDERSPEED_INFO stUnderSpeedConfig[5];                            // Ƿ��������Ϣ	��һ�����飬��ͬ��Ƿ�ٱ�Υ�´��벻ͬ��Ϊ�ձ�ʾΥ�´��벻���ֳ��ٱ�

	char                szOverLine[MAX_VIOLATIONCODE];				// ѹ��
	char                szOverLineDesc[MAX_VIOLATIONCODE_DESCRIPT];			    	// Υ��������Ϣ
    char                szOverLineShowName[MAX_VIOLATIONCODE_DESCRIPT];  //ѹ����ʾ����

	char                szOverYellowLine[MAX_VIOLATIONCODE];	    // ѹ����
	char                szOverYellowLineDesc[MAX_VIOLATIONCODE_DESCRIPT];	    	// Υ��������Ϣ

	char                szYellowInRoute[MAX_VIOLATIONCODE];			                // ����ռ��
	char                szYellowInRouteDesc[MAX_VIOLATIONCODE_DESCRIPT];			// ����ռ��Υ��������Ϣ

	char                szWrongRoute[MAX_VIOLATIONCODE];			                // ����������ʻ
	char                szWrongRouteDesc[MAX_VIOLATIONCODE_DESCRIPT];				// ����������ʻΥ��������Ϣ

	char                szDrivingOnShoulder[MAX_VIOLATIONCODE];		                // ·����ʻ
	char                szDrivingOnShoulderDesc[MAX_VIOLATIONCODE_DESCRIPT];		// ·����ʻΥ��������Ϣ

	char                szPassing[MAX_VIOLATIONCODE];                               // ������ʻ
	char                szPassingDesc[MAX_VIOLATIONCODE_DESCRIPT];               	// ������ʻΥ��������Ϣ

	char                szNoPassing[MAX_VIOLATIONCODE];                             // ��ֹ��ʻ
	char                szNoPassingDesc[MAX_VIOLATIONCODE_DESCRIPT]; 				// ��ֹ��ʻΥ��������Ϣ

	char                szFakePlate[MAX_VIOLATIONCODE];                             // ����
	char                szFakePlateDesc[MAX_VIOLATIONCODE_DESCRIPT]; 				// ����Υ��������Ϣ
	
	char                szParkingSpaceParking[MAX_VIOLATIONCODE];                   // ��λ�г�
	char                szParkingSpaceParkingDesc[MAX_VIOLATIONCODE_DESCRIPT]; 		// ��λ�г�Υ��������Ϣ��

	char                szParkingSpaceNoParking[MAX_VIOLATIONCODE];                 // ��λ�޳�
	char                szParkingSpaceNoParkingDesc[MAX_VIOLATIONCODE_DESCRIPT]; 	// ��λ�޳�Υ��������Ϣ

    char                szWithoutSafeBelt[MAX_VIOLATIONCODE];                       // ��ϵ��ȫ��
    char                szWithoutSafeBeltShowName[MAX_VIOLATIONCODE_DESCRIPT];      // ��ϵ��ȫ����ʾ����
    char                szWithoutSafeBeltDesc[MAX_VIOLATIONCODE_DESCRIPT]; 	        // ��ϵ��ȫ��Υ��������Ϣ

    char                szDriverSmoking[MAX_VIOLATIONCODE];                          // ��ʻԱ����
    char                szDriverSmokingShowName[MAX_VIOLATIONCODE_DESCRIPT];         // ��ʻԱ������ʾ����
    char                szDriverSmokingDesc[MAX_VIOLATIONCODE_DESCRIPT]; 	         // ��ʻԱ���̴�Υ��������Ϣ

    char                szDriverCalling[MAX_VIOLATIONCODE];                         // ��ʻԱ��绰
    char                szDriverCallingShowName[MAX_VIOLATIONCODE_DESCRIPT];        // ��ʻԱ��绰��ʾ����
    char                szDriverCallingDesc[MAX_VIOLATIONCODE_DESCRIPT]; 	        // ��ʻԱ��绰Υ��������Ϣ

    char                szBacking[MAX_VIOLATIONCODE];                               // Υ�µ���
    char                szBackingShowName[MAX_VIOLATIONCODE_DESCRIPT];	            // Υ�µ�����ʾ����
    char                szBackingDesc[MAX_VIOLATIONCODE_DESCRIPT];                  // Υ�µ���������Ϣ

    char                szVehicleInBusRoute[MAX_VIOLATIONCODE];                     // Υ��ռ��
    char                szVehicleInBusRouteShowName[MAX_VIOLATIONCODE_DESCRIPT];    // Υ��ռ����ʾ����
    char                szVehicleInBusRouteDesc[MAX_VIOLATIONCODE_DESCRIPT];        // Υ��ռ��������Ϣ

    char                szPedestrianRunRedLight[MAX_VIOLATIONCODE];                     // ���˴����
    char                szPedestrianRunRedLightShowName[MAX_VIOLATIONCODE_DESCRIPT];    // ���˴������ʾ����
    char                szPedestrianRunRedLightDesc[MAX_VIOLATIONCODE_DESCRIPT];        // ���˴����������Ϣ
    
    char                szPassNotInOrder[MAX_VIOLATIONCODE];                            // δ���涨����ͨ��
    char                szPassNotInOrderShowName[MAX_VIOLATIONCODE_DESCRIPT];           // δ���涨����ͨ����ʾ����
    char                szPassNotInOrderDesc[MAX_VIOLATIONCODE_DESCRIPT];               // δ���涨����ͨ��������Ϣ
	
	char				szTrafficBan[MAX_VIOLATIONCODE];								// ������Υ�������ʶ
	char				szTrafficBanShowName[MAX_VIOLATIONCODE_DESCRIPT];				// ������Υ�������ʶ��ʾ����
	char				szTrafficBanDesc[MAX_VIOLATIONCODE_DESCRIPT];					// ������Ϣ

	char                szParkingB[MAX_VIOLATIONCODE];						// B��Υ��ͣ��
	char                szParkingBDesc[MAX_VIOLATIONCODE_DESCRIPT];		 	// B��Υ��������Ϣ
    char                szParkingBShowName[MAX_VIOLATIONCODE_DESCRIPT];   	// B��Υ��ͣ����ʾ����

	char                szParkingC[MAX_VIOLATIONCODE];						// C��Υ��ͣ��
	char                szParkingCDesc[MAX_VIOLATIONCODE_DESCRIPT];		 	// C��Υ��������Ϣ
    char                szParkingCShowName[MAX_VIOLATIONCODE_DESCRIPT];   	// C��Υ��ͣ����ʾ����

	char                szParkingD[MAX_VIOLATIONCODE];						// D��Υ��ͣ��
	char                szParkingDDesc[MAX_VIOLATIONCODE_DESCRIPT];		 	// D��Υ��������Ϣ
    char                szParkingDShowName[MAX_VIOLATIONCODE_DESCRIPT];   	// D��Υ��ͣ����ʾ����

	char                szNonMotorHoldUmbrella[MAX_VIOLATIONCODE];						// �ǻ�����װ��ɡ�ߴ���
	char                szNonMotorHoldUmbrellaDesc[MAX_VIOLATIONCODE_DESCRIPT];		 	// �ǻ�����װ��ɡ��������Ϣ
	char                szNonMotorHoldUmbrellaShowName[MAX_VIOLATIONCODE_DESCRIPT];   	// �ǻ�����װ��ɡ����ʾ����
}VIOLATIONCODE_INFO;

// �����������
typedef enum tagEmCheckType
{
    EM_CHECK_TYPE_UNKNOWN,             // ��ʶ��ļ������
    EM_CHECK_TYPE_PHYSICAL,            // ������
    EM_CHECK_TYPE_VIDEO,               // ��Ƶ���
}EM_CHECK_TYPE;

typedef struct tagTrafficEventCheckInfo
{
    BOOL                abTrafficGate;                  // �Ƿ�Я����ͨ������Ϣ
    EM_CHECK_TYPE       emTrafficGate;                  // ��ͨ���ڼ������

    BOOL                abTrafficJunction;              // �Ƿ�Я����ͨ·����Ϣ
    EM_CHECK_TYPE       emTrafficJunction;              // ��ͨ·�ڼ������

    BOOL                abTrafficTollGate;              // �Ƿ�Я���½�ͨ������Ϣ
    EM_CHECK_TYPE       emTrafficTollGate;              // �½�ͨ���ڼ������

    BOOL                abTrafficRunRedLight;           // �Ƿ�Я����ͨ�������Ϣ
    EM_CHECK_TYPE       emTrafficRunRedLight;           // ��ͨ����Ƽ������
    
    BOOL                abTrafficRunYellowLight;        // �Ƿ�Я����ͨ���Ƶ���Ϣ
    EM_CHECK_TYPE       emTrafficRunYellowLight;        // ��ͨ���ƵƼ������

    BOOL                abTrafficOverLine;              // �Ƿ�Я����ͨѹ����Ϣ
    EM_CHECK_TYPE       emTrafficOverLine;              // ��ͨѹ�߼������

    BOOL                abTrafficOverYellowLine;        // �Ƿ�Я����ͨѹ������Ϣ
    EM_CHECK_TYPE       emTrafficOverYellowLine;        // ��ͨѹ���߼������

    BOOL                abTrafficRetrograde;            // �Ƿ�Я����ͨ������Ϣ
    EM_CHECK_TYPE       emTrafficRetrograde;            // ��ͨ���м������

    BOOL                abTrafficTurnLeft;              // �Ƿ�Я����ͨΥ����ת��Ϣ
    EM_CHECK_TYPE       emTrafficTurnLeft;              // ��ͨΥ����ת�������

    BOOL                abTrafficTurnRight;             // �Ƿ�Я����ͨΥ����ת��Ϣ
    EM_CHECK_TYPE       emTrafficTurnRight;             // ��ͨ·��Υ����ת����

    BOOL                abTrafficU_Turn;                // �Ƿ�Я����ͨΥ�µ�ͷ��Ϣ
    EM_CHECK_TYPE       emTrafficU_Turn;                // ��ͨΥ�µ�ͷ�������

    BOOL                abTrafficCrossLane;             // �Ƿ�Я����ͨΥ�±����Ϣ
    EM_CHECK_TYPE       emTrafficCrossLane;             // ��ͨΥ�±���������

    BOOL                abTrafficParking;               // �Ƿ�Я����ͨΥ��ͣ����Ϣ
    EM_CHECK_TYPE       emTrafficParking;               // ��ͨΥ��ͣ���������

    BOOL                abTrafficJam;                   // �Ƿ�Я����ͨӵ����Ϣ
    EM_CHECK_TYPE       emTrafficJam;                   // ��ͨӵ�¼������

    BOOL                abTrafficIdle;                  // �Ƿ�Я����ͨ��ͨ������Ϣ
    EM_CHECK_TYPE       emTrafficIdle;                  // ��ͨ��ͨ���м������

    BOOL                abTrafficWaitingArea;           // �Ƿ�Я����ͨΥ��ʻ���������Ϣ
    EM_CHECK_TYPE       emTrafficWaitingArea;           // ��ͨΥ��ʻ��������������

    BOOL                abTrafficUnderSpeed;            // �Ƿ�Я����ͨǷ����Ϣ
    EM_CHECK_TYPE       emTrafficUnderSpeed;            // ��ͨǷ�ټ������

    BOOL                abTrafficOverSpeed;             // �Ƿ�Я����ͨ������Ϣ
    EM_CHECK_TYPE       emTrafficOverSpeed;             // ��ͨ���ټ������

    BOOL                abTrafficWrongRoute;            // �Ƿ�Я����ͨ����������ʻ��Ϣ
    EM_CHECK_TYPE       emTrafficWrongRoute;            // ��ͨ����������ʻ�������

    BOOL                abTrafficYellowInRoute;         // �Ƿ�Я����ͨ����ռ����Ϣ
    EM_CHECK_TYPE       emTrafficYellowInRoute;         // ��ͨ����ռ���������

    BOOL                abTrafficVehicleInRoute;        // �Ƿ�Я����ͨ�г�ռ����Ϣ
    EM_CHECK_TYPE       emTrafficVehicleInRoute;        // ��ͨ�г�ռ���������

    BOOL                abTrafficControl;               // �Ƿ�Я����ͨ��ͨ������Ϣ
    EM_CHECK_TYPE       emTrafficControl;               // ��ͨ��ͨ���Ƽ������

    BOOL                abTrafficObjectAlarm;           // �Ƿ�Я����ָͨ������ץ����Ϣ
    EM_CHECK_TYPE       emTrafficObjectAlarm;           // ��ָͨ������ץ�ļ������

    BOOL                abTrafficAccident;              // �Ƿ�Я����ͨ��ͨ�¹���Ϣ
    EM_CHECK_TYPE       emTrafficAccident;              // ��ͨ��ͨ�¹ʼ������

    BOOL                abTrafficStay;                  // �Ƿ�Я����ͨ��ͨͣ��/������Ϣ
    EM_CHECK_TYPE       emTrafficStay;                  // ��ͨ��ͨͣ��/�����������

    BOOL                abTrafficPedestrainPriority;    // �Ƿ�Я����ͨ����������������Ϣ
    EM_CHECK_TYPE       emTrafficPedestrainPriority;    // ��ͨ�������������ȼ������

    BOOL                abTrafficPedestrain;            // �Ƿ�Я����ͨ��ͨ�����¼���Ϣ
    EM_CHECK_TYPE       emTrafficPedestrain;            // ��ͨ��ͨ�����¼��������

    BOOL                abTrafficThrow;                 // �Ƿ�Я����ͨ��ͨ������Ʒ�¼���Ϣ
    EM_CHECK_TYPE       emTrafficThrow;                 // ��ͨ��ͨ������Ʒ�¼��������

    BOOL                abTrafficVehicleInBusRoute;     // �Ƿ�Я����ͨΥ��ռ����Ϣ
    EM_CHECK_TYPE       emTrafficVehicleInBusRoute;     // ��ͨΥ��ռ���������

    BOOL                abTrafficBacking;               // �Ƿ�Я����ͨΥ�µ�����Ϣ
    EM_CHECK_TYPE       emTrafficBacking;               // ��ͨΥ�µ����������

    BOOL                abTrafficOverStopLine;          // �Ƿ�Я����ͨѹֹͣ����Ϣ
    EM_CHECK_TYPE       emTrafficOverStopLine;          // ��ͨѹֹͣ�߼������

    BOOL                abTrafficParkingOnYellowBox;    // �Ƿ�Я����ͨ��������ץ����Ϣ
    EM_CHECK_TYPE       emTrafficParkingOnYellowBox;    // ��ͨ��������ץ�ļ������

    BOOL                abTrafficParkingSpaceParking;   // �Ƿ�Я����ͨ��λ�г���Ϣ
    EM_CHECK_TYPE       emTrafficParkingSpaceParking;   // ��ͨ��λ�г��������

    BOOL                abTrafficParkingSpaceNoParking; // �Ƿ�Я����ͨ��λ�޳���Ϣ
    EM_CHECK_TYPE       emTrafficParkingSpaceNoParking; // ��ͨ��λ�޳��������

    BOOL                abTrafficParkingSpaceOverLine;  // �Ƿ�Я����ͨ��λ�г�ѹ����Ϣ
    EM_CHECK_TYPE       emTrafficParkingSpaceOverLine;  // ��ͨ��λ�г�ѹ�߼������

    BOOL                abParkingSpaceDetection;        // �Ƿ�Я����ͨ��ͣ��λ״̬�����Ϣ
    EM_CHECK_TYPE       emParkingSpaceDetection;        // ��ͨ��ͣ��λ״̬���������

    BOOL                abTrafficRestrictedPlate;       // �Ƿ�Я����ͨ���޳�����Ϣ
    EM_CHECK_TYPE       emTrafficRestrictedPlate;       // ��ͨ���޳��Ƽ������

    BOOL                abTrafficWithoutSafeBelt;       // �Ƿ�Я����ͨ��ϵ��ȫ����Ϣ
    EM_CHECK_TYPE       emTrafficWithoutSafeBelt;       // ��ͨ��ϵ��ȫ���������

    BOOL                abTrafficNoPassing;             // �Ƿ�Я����ͨ������Ϣ
    EM_CHECK_TYPE       emTrafficNoPassing;             // ��ͨ���м������

    BOOL                abVehicleAnalyse;               // �Ƿ�Я����ͨ����������������Ϣ
    EM_CHECK_TYPE       emVehicleAnalyse;               // ��ͨ���������������

    BOOL                abCrossLineDetection;           // �Ƿ�Я����ͨ��������Ϣ
    EM_CHECK_TYPE       emCrossLineDetection;           // ��ͨ�����߼������

    BOOL                abCrossFenceDetection;          // �Ƿ�Я����ͨ��ԽΧ����Ϣ
    EM_CHECK_TYPE       emCrossFenceDetection;          // ��ͨ��ԽΧ���������

    BOOL                abCrossRegionDetection;         // �Ƿ�Я����ͨ��������Ϣ
    EM_CHECK_TYPE       emCrossRegionDetection;         // ��ͨ�������������

    BOOL                abPasteDetection;               // �Ƿ�Я����ͨATM������Ϣ
    EM_CHECK_TYPE       emPasteDetection;               // ��ͨATM�����������

    BOOL                abLeftDetection;                // �Ƿ�Я����ͨ��Ʒ������Ϣ
    EM_CHECK_TYPE       emLeftDetection;                // ��ͨ��Ʒ�����������

    BOOL                abPreservation;                 // �Ƿ�Я����ͨ��Ʒ��ȫ��Ϣ
    EM_CHECK_TYPE       emPreservation;                 // ��ͨ��Ʒ��ȫ�������

    BOOL                abTakenAwayDetection;           // �Ƿ�Я����ͨ��Ʒ������Ϣ
    EM_CHECK_TYPE       emTakenAwayDetection;           // ��ͨ��Ʒ���Ƽ������

    BOOL                abStayDetection;                // �Ƿ�Я����ͨͣ��/������Ϣ
    EM_CHECK_TYPE       emStayDetection;                // ��ͨͣ��/�����������

    BOOL                abParkingDetection;             // �Ƿ�Я����ͨ�Ƿ�ͣ����Ϣ
    EM_CHECK_TYPE       emParkingDetection;             // ��ͨ�Ƿ�ͣ���������

    BOOL                abWanderDetection;              // �Ƿ�Я����ͨ�ǻ���Ϣ
    EM_CHECK_TYPE       emWanderDetection;              // ��ͨ�ǻ��������

    BOOL                abMoveDetection;                // �Ƿ�Я����ͨ�˶���Ϣ
    EM_CHECK_TYPE       emMoveDetection;                // ��ͨ�˶��������

    BOOL                abTailDetection;                // �Ƿ�Я����ͨβ����Ϣ
    EM_CHECK_TYPE       emTailDetection;                // ��ͨβ��������

    BOOL                abRioterDetection;              // �Ƿ�Я����ͨ�ۼ���Ϣ
    EM_CHECK_TYPE       emRioterDetection;              // ��ͨ�ۼ��������

    BOOL                abFightDetection;               // �Ƿ�Я����ͨ�����Ϣ
    EM_CHECK_TYPE       emFightDetection;               // ��ͨ��ܼ������

    BOOL                abRetrogradeDetection;          // �Ƿ�Я����ͨ������Ϣ
    EM_CHECK_TYPE       emRetrogradeDetection;          // ��ͨ���м������

    BOOL                abFireDetection;                // �Ƿ�Я����ͨ������Ϣ
    EM_CHECK_TYPE       emFireDetection;                // ��ͨ����������

    BOOL                abSmokeDetection;               // �Ƿ�Я����ͨ������Ϣ
    EM_CHECK_TYPE       emSmokeDetection;               // ��ͨ����������

    BOOL                abNumberStat;                   // �Ƿ�Я����ͨ����ͳ����Ϣ
    EM_CHECK_TYPE       emNumberStat;                   // ��ͨ����ͳ�Ƽ������

    BOOL                abVideoAbnormalDetection;       // �Ƿ�Я����ͨ��Ƶ�쳣��Ϣ
    EM_CHECK_TYPE       emVideoAbnormalDetection;       // ��ͨ��Ƶ�쳣�������

    BOOL                abPrisonerRiseDetection;        // �Ƿ�Я��������������������Ϣ
    EM_CHECK_TYPE       emPrisonerRiseDetection;        // ����������������������

    BOOL                abFaceDetection;                // �Ƿ�Я�����������Ϣ
    EM_CHECK_TYPE       emFaceDetection;                // �������������

    BOOL                abFaceRecognition;              // �Ƿ�Я������ʶ����Ϣ
    EM_CHECK_TYPE       emFaceRecognition;              // ����ʶ��������

    BOOL                abDensityDetection;             // �Ƿ�Я���ܼ��ȼ����Ϣ
    EM_CHECK_TYPE       emDensityDetection;             // �ܼ��ȼ��������

    BOOL                abQueueDetection;               // �Ƿ�Я���ŶӼ����Ϣ
    EM_CHECK_TYPE       emQueueDetection;               // �ŶӼ��������

    BOOL                abClimbDetection;               // �Ƿ�Я���ʸ߼����Ϣ
    EM_CHECK_TYPE       emClimbDetection;               // �ʸ߼������

    BOOL                abLeaveDetection;               // �Ƿ�Я����ڼ����Ϣ
    EM_CHECK_TYPE       emLeaveDetection;               // ��ڼ������

    BOOL                abVehicleOnPoliceCar;           // �Ƿ�Я�����ؾ�����Ϣ
    EM_CHECK_TYPE       emVehicleOnPoliceCar;           // ���ؾ����������

    BOOL                abVehicleOnBus;                 // �Ƿ�Я�����ع�����Ϣ
    EM_CHECK_TYPE       emVehicleOnBus;                 // ���ع����������

    BOOL                abVehicleOnSchoolBus;           // �Ƿ�Я������У����Ϣ
    EM_CHECK_TYPE       emVehicleOnSchoolBus;           // ����У���������  

	BOOL				abStandUpDetection;				// �Ƿ�Я��ѧ��������Ϣ
	EM_CHECK_TYPE		emStandUpDetection;				// ѧ�������������
}TRAFFIC_EVENT_CHECK_INFO;  

typedef struct tagTrafficEventCheckMask
{
    BOOL                abTrafficGate;                  // �Ƿ�Я����ͨ������Ϣ
    int                 nTrafficGate;                   // ��ͨ���ڼ��ģʽ����

    BOOL                abTrafficJunction;              // �Ƿ�Я����ͨ·����Ϣ
    int                 nTrafficJunction;               // ��ͨ·�ڼ��ģʽ����

    BOOL                abTrafficTollGate;              // �Ƿ�Я���½�ͨ������Ϣ
    int                 nTrafficTollGate;               // �½�ͨ���ڼ��ģʽ����

    BOOL                abTrafficRunRedLight;           // �Ƿ�Я����ͨ�������Ϣ
    int                 nTrafficRunRedLight;            // ��ͨ����Ƽ��ģʽ����
    
    BOOL                abTrafficRunYellowLight;        // �Ƿ�Я����ͨ���Ƶ���Ϣ
    int                 nTrafficRunYellowLight;         // ��ͨ���ƵƼ��ģʽ����

    BOOL                abTrafficOverLine;              // �Ƿ�Я����ͨѹ����Ϣ
    int                 nTrafficOverLine;               // ��ͨѹ�߼��ģʽ����

    BOOL                abTrafficOverYellowLine;        // �Ƿ�Я����ͨѹ������Ϣ
    int                 nTrafficOverYellowLine;         // ��ͨѹ���߼��ģʽ����

    BOOL                abTrafficRetrograde;            // �Ƿ�Я����ͨ������Ϣ
    int                 nTrafficRetrograde;             // ��ͨ���м��ģʽ����

    BOOL                abTrafficTurnLeft;              // �Ƿ�Я����ͨΥ����ת��Ϣ
    int                 nTrafficTurnLeft;               // ��ͨΥ����ת���ģʽ����

    BOOL                abTrafficTurnRight;             // �Ƿ�Я����ͨΥ����ת��Ϣ
    int                 nTrafficTurnRight;              // ��ͨ·��Υ����ת����

    BOOL                abTrafficU_Turn;                // �Ƿ�Я����ͨΥ�µ�ͷ��Ϣ
    int                 nTrafficU_Turn;                 // ��ͨΥ�µ�ͷ���ģʽ����

    BOOL                abTrafficCrossLane;             // �Ƿ�Я����ͨΥ�±����Ϣ
    int                 nTrafficCrossLane;              // ��ͨΥ�±�����ģʽ����

    BOOL                abTrafficParking;               // �Ƿ�Я����ͨΥ��ͣ����Ϣ
    int                 nTrafficParking;                // ��ͨΥ��ͣ�����ģʽ����

    BOOL                abTrafficJam;                   // �Ƿ�Я����ͨӵ����Ϣ
    int                 nTrafficJam;                    // ��ͨӵ�¼��ģʽ����

    BOOL                abTrafficIdle;                  // �Ƿ�Я����ͨ��ͨ������Ϣ
    int                 nTrafficIdle;                   // ��ͨ��ͨ���м��ģʽ����

    BOOL                abTrafficWaitingArea;           // �Ƿ�Я����ͨΥ��ʻ���������Ϣ
    int                 nTrafficWaitingArea;            // ��ͨΥ��ʻ����������ģʽ����

    BOOL                abTrafficUnderSpeed;            // �Ƿ�Я����ͨǷ����Ϣ
    int                 nTrafficUnderSpeed;             // ��ͨǷ�ټ��ģʽ����

    BOOL                abTrafficOverSpeed;             // �Ƿ�Я����ͨ������Ϣ
    int                 nTrafficOverSpeed;              // ��ͨ���ټ��ģʽ����

    BOOL                abTrafficWrongRoute;            // �Ƿ�Я����ͨ����������ʻ��Ϣ
    int                 nTrafficWrongRoute;             // ��ͨ����������ʻ���ģʽ����

    BOOL                abTrafficYellowInRoute;         // �Ƿ�Я����ͨ����ռ����Ϣ
    int                 nTrafficYellowInRoute;          // ��ͨ����ռ�����ģʽ����

    BOOL                abTrafficVehicleInRoute;        // �Ƿ�Я����ͨ�г�ռ����Ϣ
    int                 nTrafficVehicleInRoute;         // ��ͨ�г�ռ�����ģʽ����

    BOOL                abTrafficControl;               // �Ƿ�Я����ͨ��ͨ������Ϣ
    int                 nTrafficControl;                // ��ͨ��ͨ���Ƽ��ģʽ����

    BOOL                abTrafficObjectAlarm;           // �Ƿ�Я����ָͨ������ץ����Ϣ
    int                 nTrafficObjectAlarm;            // ��ָͨ������ץ�ļ��ģʽ����

    BOOL                abTrafficAccident;              // �Ƿ�Я����ͨ��ͨ�¹���Ϣ
    int                 nTrafficAccident;               // ��ͨ��ͨ�¹ʼ��ģʽ����

    BOOL                abTrafficStay;                  // �Ƿ�Я����ͨ��ͨͣ��/������Ϣ
    int                 nTrafficStay;                   // ��ͨ��ͨͣ��/�������ģʽ����

    BOOL                abTrafficPedestrainPriority;    // �Ƿ�Я����ͨ����������������Ϣ
    int                 nTrafficPedestrainPriority;     // ��ͨ�������������ȼ��ģʽ����

    BOOL                abTrafficPedestrain;            // �Ƿ�Я����ͨ��ͨ�����¼���Ϣ
    int                 nTrafficPedestrain;             // ��ͨ��ͨ�����¼����ģʽ����

    BOOL                abTrafficThrow;                 // �Ƿ�Я����ͨ��ͨ������Ʒ�¼���Ϣ
    int                 nTrafficThrow;                  // ��ͨ��ͨ������Ʒ�¼����ģʽ����

    BOOL                abTrafficVehicleInBusRoute;     // �Ƿ�Я����ͨΥ��ռ����Ϣ
    int                 nTrafficVehicleInBusRoute;      // ��ͨΥ��ռ�����ģʽ����

    BOOL                abTrafficBacking;               // �Ƿ�Я����ͨΥ�µ�����Ϣ
    int                 nTrafficBacking;                // ��ͨΥ�µ������ģʽ����

    BOOL                abTrafficOverStopLine;          // �Ƿ�Я����ͨѹֹͣ����Ϣ
    int                 nTrafficOverStopLine;           // ��ͨѹֹͣ�߼��ģʽ����

    BOOL                abTrafficParkingOnYellowBox;    // �Ƿ�Я����ͨ��������ץ����Ϣ
    int                 nTrafficParkingOnYellowBox;     // ��ͨ��������ץ�ļ��ģʽ����

    BOOL                abTrafficParkingSpaceParking;   // �Ƿ�Я����ͨ��λ�г���Ϣ
    int                 nTrafficParkingSpaceParking;    // ��ͨ��λ�г����ģʽ����

    BOOL                abTrafficParkingSpaceNoParking; // �Ƿ�Я����ͨ��λ�޳���Ϣ
    int                 nTrafficParkingSpaceNoParking;  // ��ͨ��λ�޳����ģʽ����

    BOOL                abTrafficParkingSpaceOverLine;  // �Ƿ�Я����ͨ��λ�г�ѹ����Ϣ
    int                 nTrafficParkingSpaceOverLine;   // ��ͨ��λ�г�ѹ�߼��ģʽ����

    BOOL                abParkingSpaceDetection;        // �Ƿ�Я����ͨ��ͣ��λ״̬�����Ϣ
    int                 nParkingSpaceDetection;         // ��ͨ��ͣ��λ״̬�����ģʽ����

    BOOL                abTrafficRestrictedPlate;       // �Ƿ�Я����ͨ���޳�����Ϣ
    int                 nTrafficRestrictedPlate;        // ��ͨ���޳��Ƽ��ģʽ����

    BOOL                abTrafficWithoutSafeBelt;       // �Ƿ�Я����ͨ��ϵ��ȫ����Ϣ
    int                 nTrafficWithoutSafeBelt;        // ��ͨ��ϵ��ȫ�����ģʽ����

    BOOL                abTrafficNoPassing;             // �Ƿ�Я����ͨ������Ϣ
    int                 nTrafficNoPassing;              // ��ͨ���м��ģʽ����

    BOOL                abVehicleAnalyse;               // �Ƿ�Я����ͨ����������������Ϣ
    int                 nVehicleAnalyse;                // ��ͨ�����������ģʽ����

    BOOL                abCrossLineDetection;           // �Ƿ�Я����ͨ��������Ϣ
    int                 nCrossLineDetection;            // ��ͨ�����߼��ģʽ����

    BOOL                abCrossFenceDetection;          // �Ƿ�Я����ͨ��ԽΧ����Ϣ
    int                 nCrossFenceDetection;           // ��ͨ��ԽΧ�����ģʽ����

    BOOL                abCrossRegionDetection;         // �Ƿ�Я����ͨ��������Ϣ
    int                 nCrossRegionDetection;          // ��ͨ���������ģʽ����

    BOOL                abPasteDetection;               // �Ƿ�Я����ͨATM������Ϣ
    int                 nPasteDetection;                // ��ͨATM�������ģʽ����

    BOOL                abLeftDetection;                // �Ƿ�Я����ͨ��Ʒ������Ϣ
    int                 nLeftDetection;                 // ��ͨ��Ʒ�������ģʽ����

    BOOL                abPreservation;                 // �Ƿ�Я����ͨ��Ʒ��ȫ��Ϣ
    int                 nPreservation;                  // ��ͨ��Ʒ��ȫ���ģʽ����

    BOOL                abTakenAwayDetection;           // �Ƿ�Я����ͨ��Ʒ������Ϣ
    int                 nTakenAwayDetection;            // ��ͨ��Ʒ���Ƽ��ģʽ����

    BOOL                abStayDetection;                // �Ƿ�Я����ͨͣ��/������Ϣ
    int                 nStayDetection;                 // ��ͨͣ��/�������ģʽ����

    BOOL                abParkingDetection;             // �Ƿ�Я����ͨ�Ƿ�ͣ����Ϣ
    int                 nParkingDetection;              // ��ͨ�Ƿ�ͣ�����ģʽ����

    BOOL                abWanderDetection;              // �Ƿ�Я����ͨ�ǻ���Ϣ
    int                 nWanderDetection;               // ��ͨ�ǻ����ģʽ����

    BOOL                abMoveDetection;                // �Ƿ�Я����ͨ�˶���Ϣ
    int                 nMoveDetection;                 // ��ͨ�˶����ģʽ����

    BOOL                abTailDetection;                // �Ƿ�Я����ͨβ����Ϣ
    int                 nTailDetection;                 // ��ͨβ����ģʽ����

    BOOL                abRioterDetection;              // �Ƿ�Я����ͨ�ۼ���Ϣ
    int                 nRioterDetection;               // ��ͨ�ۼ����ģʽ����

    BOOL                abFightDetection;               // �Ƿ�Я����ͨ�����Ϣ
    int                 nFightDetection;                // ��ͨ��ܼ��ģʽ����

    BOOL                abRetrogradeDetection;          // �Ƿ�Я����ͨ������Ϣ
    int                 nRetrogradeDetection;           // ��ͨ���м��ģʽ����

    BOOL                abFireDetection;                // �Ƿ�Я����ͨ������Ϣ
    int                 nFireDetection;                 // ��ͨ������ģʽ����

    BOOL                abSmokeDetection;               // �Ƿ�Я����ͨ������Ϣ
    int                 nSmokeDetection;                // ��ͨ������ģʽ����

    BOOL                abNumberStat;                   // �Ƿ�Я����ͨ����ͳ����Ϣ
    int                 nNumberStat;                    // ��ͨ����ͳ�Ƽ��ģʽ����

    BOOL                abVideoAbnormalDetection;       // �Ƿ�Я����ͨ��Ƶ�쳣��Ϣ
    int                 nVideoAbnormalDetection;        // ��ͨ��Ƶ�쳣���ģʽ����

    BOOL                abPrisonerRiseDetection;        // �Ƿ�Я��������������������Ϣ
    int                 nPrisonerRiseDetection;         // ������������������ģʽ����

    BOOL                abFaceDetection;                // �Ƿ�Я�����������Ϣ
    int                 nFaceDetection;                 // ���������ģʽ����

    BOOL                abFaceRecognition;              // �Ƿ�Я������ʶ����Ϣ
    int                 nFaceRecognition;               // ����ʶ����ģʽ����

    BOOL                abDensityDetection;             // �Ƿ�Я���ܼ��ȼ����Ϣ
    int                 nDensityDetection;              // �ܼ��ȼ����ģʽ����

    BOOL                abQueueDetection;               // �Ƿ�Я���ŶӼ����Ϣ
    int                 nQueueDetection;                // �ŶӼ����ģʽ����

    BOOL                abClimbDetection;               // �Ƿ�Я���ʸ߼����Ϣ
    int                 nClimbDetection;                // �ʸ߼��ģʽ����

    BOOL                abLeaveDetection;               // �Ƿ�Я����ڼ����Ϣ
    int                 nLeaveDetection;                // ��ڼ��ģʽ����

    BOOL                abVehicleOnPoliceCar;           // �Ƿ�Я�����ؾ�����Ϣ
    int                 nVehicleOnPoliceCar;            // ���ؾ������ģʽ����

    BOOL                abVehicleOnBus;                 // �Ƿ�Я�����ع�����Ϣ
    int                 nVehicleOnBus;                  // ���ع������ģʽ����

    BOOL                abVehicleOnSchoolBus;           // �Ƿ�Я������У����Ϣ
    int                 nVehicleOnSchoolBus;            // ����У�����ģʽ���� 
}TRAFFIC_EVENT_CHECK_MASK;  

// Υ��ץ��ʱ�����ñ�
typedef struct tagTimeScheduleInfo
{
    BOOL                bEnable;                                              // �Ƿ�����ʱ���
    CFG_TIME_SECTION    stuTimeSchedule[WEEK_DAY_NUM][MAX_REC_TSECT];         // ʱ���
}TIME_SCHEDULE_INFO;

// Υ��ץ���Զ���ʱ������
typedef struct tagViolationTimeSchedule
{
    BOOL                abTrafficGate;                  // �Ƿ�Я����ͨ������Ϣ
    TIME_SCHEDULE_INFO  stTrafficGate;                  // ��ͨ����ʱ������

    BOOL                abTrafficJunction;              // �Ƿ�Я����ͨ·����Ϣ
    TIME_SCHEDULE_INFO  stTrafficJunction;              // ��ͨ·��ʱ������

    BOOL                abTrafficTollGate;              // �Ƿ�Я���½�ͨ������Ϣ
    TIME_SCHEDULE_INFO  stTrafficTollGate;              // �½�ͨ����ʱ������

    BOOL                abTrafficRunRedLight;           // �Ƿ�Я����ͨ�������Ϣ
    TIME_SCHEDULE_INFO  stTrafficRunRedLight;           // ��ͨ�����ʱ������
    
    BOOL                abTrafficRunYellowLight;        // �Ƿ�Я����ͨ���Ƶ���Ϣ
    TIME_SCHEDULE_INFO  stTrafficRunYellowLight;        // ��ͨ���Ƶ�ʱ������

    BOOL                abTrafficOverLine;              // �Ƿ�Я����ͨѹ����Ϣ
    TIME_SCHEDULE_INFO  stTrafficOverLine;              // ��ͨѹ��ʱ������

    BOOL                abTrafficOverYellowLine;        // �Ƿ�Я����ͨѹ������Ϣ
    TIME_SCHEDULE_INFO  stTrafficOverYellowLine;        // ��ͨѹ����ʱ������

    BOOL                abTrafficRetrograde;            // �Ƿ�Я����ͨ������Ϣ
    TIME_SCHEDULE_INFO  stTrafficRetrograde;            // ��ͨ����ʱ������

    BOOL                abTrafficTurnLeft;              // �Ƿ�Я����ͨΥ����ת��Ϣ
    TIME_SCHEDULE_INFO  stTrafficTurnLeft;              // ��ͨΥ����תʱ������

    BOOL                abTrafficTurnRight;             // �Ƿ�Я����ͨΥ����ת��Ϣ
    TIME_SCHEDULE_INFO  stTrafficTurnRight;             // ��ͨ·��Υ����ת����

    BOOL                abTrafficU_Turn;                // �Ƿ�Я����ͨΥ�µ�ͷ��Ϣ
    TIME_SCHEDULE_INFO  stTrafficU_Turn;                // ��ͨΥ�µ�ͷʱ������

    BOOL                abTrafficCrossLane;             // �Ƿ�Я����ͨΥ�±����Ϣ
    TIME_SCHEDULE_INFO  stTrafficCrossLane;             // ��ͨΥ�±��ʱ������

    BOOL                abTrafficParking;               // �Ƿ�Я����ͨΥ��ͣ����Ϣ
    TIME_SCHEDULE_INFO  stTrafficParking;               // ��ͨΥ��ͣ��ʱ������

    BOOL                abTrafficJam;                   // �Ƿ�Я����ͨӵ����Ϣ
    TIME_SCHEDULE_INFO  stTrafficJam;                   // ��ͨӵ��ʱ������

    BOOL                abTrafficIdle;                  // �Ƿ�Я����ͨ��ͨ������Ϣ
    TIME_SCHEDULE_INFO  stTrafficIdle;                  // ��ͨ��ͨ����ʱ������

    BOOL                abTrafficWaitingArea;           // �Ƿ�Я����ͨΥ��ʻ���������Ϣ
    TIME_SCHEDULE_INFO  stTrafficWaitingArea;           // ��ͨΥ��ʻ�������ʱ������

    BOOL                abTrafficUnderSpeed;            // �Ƿ�Я����ͨǷ����Ϣ
    TIME_SCHEDULE_INFO  stTrafficUnderSpeed;            // ��ͨǷ��ʱ������

    BOOL                abTrafficOverSpeed;             // �Ƿ�Я����ͨ������Ϣ
    TIME_SCHEDULE_INFO  stTrafficOverSpeed;             // ��ͨ����ʱ������

    BOOL                abTrafficWrongRoute;            // �Ƿ�Я����ͨ����������ʻ��Ϣ
    TIME_SCHEDULE_INFO  stTrafficWrongRoute;            // ��ͨ����������ʻʱ������

    BOOL                abTrafficYellowInRoute;         // �Ƿ�Я����ͨ����ռ����Ϣ
    TIME_SCHEDULE_INFO  stTrafficYellowInRoute;         // ��ͨ����ռ��ʱ������

    BOOL                abTrafficVehicleInRoute;        // �Ƿ�Я����ͨ�г�ռ����Ϣ
    TIME_SCHEDULE_INFO  stTrafficVehicleInRoute;        // ��ͨ�г�ռ��ʱ������

    BOOL                abTrafficControl;               // �Ƿ�Я����ͨ��ͨ������Ϣ
    TIME_SCHEDULE_INFO  stTrafficControl;               // ��ͨ��ͨ����ʱ������

    BOOL                abTrafficObjectAlarm;           // �Ƿ�Я����ָͨ������ץ����Ϣ
    TIME_SCHEDULE_INFO  stTrafficObjectAlarm;           // ��ָͨ������ץ��ʱ������

    BOOL                abTrafficAccident;              // �Ƿ�Я����ͨ��ͨ�¹���Ϣ
    TIME_SCHEDULE_INFO  stTrafficAccident;              // ��ͨ��ͨ�¹�ʱ������

    BOOL                abTrafficStay;                  // �Ƿ�Я����ͨ��ͨͣ��/������Ϣ
    TIME_SCHEDULE_INFO  stTrafficStay;                  // ��ͨ��ͨͣ��/����ʱ������

    BOOL                abTrafficPedestrainPriority;    // �Ƿ�Я����ͨ����������������Ϣ
    TIME_SCHEDULE_INFO  stTrafficPedestrainPriority;    // ��ͨ��������������ʱ������

    BOOL                abTrafficPedestrain;            // �Ƿ�Я����ͨ��ͨ�����¼���Ϣ
    TIME_SCHEDULE_INFO  stTrafficPedestrain;            // ��ͨ��ͨ�����¼�ʱ������

    BOOL                abTrafficThrow;                 // �Ƿ�Я����ͨ��ͨ������Ʒ�¼���Ϣ
    TIME_SCHEDULE_INFO  stTrafficThrow;                 // ��ͨ��ͨ������Ʒ�¼�ʱ������

    BOOL                abTrafficVehicleInBusRoute;     // �Ƿ�Я����ͨΥ��ռ����Ϣ
    TIME_SCHEDULE_INFO  stTrafficVehicleInBusRoute;     // ��ͨΥ��ռ��ʱ������

    BOOL                abTrafficBacking;               // �Ƿ�Я����ͨΥ�µ�����Ϣ
    TIME_SCHEDULE_INFO  stTrafficBacking;               // ��ͨΥ�µ���ʱ������

    BOOL                abTrafficOverStopLine;          // �Ƿ�Я����ͨѹֹͣ����Ϣ
    TIME_SCHEDULE_INFO  stTrafficOverStopLine;          // ��ͨѹֹͣ��ʱ������

    BOOL                abTrafficParkingOnYellowBox;    // �Ƿ�Я����ͨ��������ץ����Ϣ
    TIME_SCHEDULE_INFO  stTrafficParkingOnYellowBox;    // ��ͨ��������ץ��ʱ������

    BOOL                abTrafficParkingSpaceParking;   // �Ƿ�Я����ͨ��λ�г���Ϣ
    TIME_SCHEDULE_INFO  stTrafficParkingSpaceParking;   // ��ͨ��λ�г�ʱ������

    BOOL                abTrafficParkingSpaceNoParking; // �Ƿ�Я����ͨ��λ�޳���Ϣ
    TIME_SCHEDULE_INFO  stTrafficParkingSpaceNoParking; // ��ͨ��λ�޳�ʱ������

    BOOL                abTrafficParkingSpaceOverLine;  // �Ƿ�Я����ͨ��λ�г�ѹ����Ϣ
    TIME_SCHEDULE_INFO  stTrafficParkingSpaceOverLine;  // ��ͨ��λ�г�ѹ��ʱ������

    BOOL                abParkingSpaceDetection;        // �Ƿ�Я����ͨ��ͣ��λ״̬�����Ϣ
    TIME_SCHEDULE_INFO  stParkingSpaceDetection;        // ��ͨ��ͣ��λ״̬���ʱ������

    BOOL                abTrafficRestrictedPlate;       // �Ƿ�Я����ͨ���޳�����Ϣ
    TIME_SCHEDULE_INFO  stTrafficRestrictedPlate;       // ��ͨ���޳���ʱ������

    BOOL                abTrafficWithoutSafeBelt;       // �Ƿ�Я����ͨ��ϵ��ȫ����Ϣ
    TIME_SCHEDULE_INFO  stTrafficWithoutSafeBelt;       // ��ͨ��ϵ��ȫ��ʱ������

    BOOL                abTrafficNoPassing;             // �Ƿ�Я����ͨ������Ϣ
    TIME_SCHEDULE_INFO  stTrafficNoPassing;             // ��ͨ����ʱ������

    BOOL                abVehicleAnalyse;               // �Ƿ�Я����ͨ����������������Ϣ
    TIME_SCHEDULE_INFO  stVehicleAnalyse;               // ��ͨ��������ʱ������

    BOOL                abCrossLineDetection;           // �Ƿ�Я����ͨ��������Ϣ
    TIME_SCHEDULE_INFO  stCrossLineDetection;           // ��ͨ������ʱ������

    BOOL                abCrossFenceDetection;          // �Ƿ�Я����ͨ��ԽΧ����Ϣ
    TIME_SCHEDULE_INFO  stCrossFenceDetection;          // ��ͨ��ԽΧ��ʱ������

    BOOL                abCrossRegionDetection;         // �Ƿ�Я����ͨ��������Ϣ
    TIME_SCHEDULE_INFO  stCrossRegionDetection;         // ��ͨ������ʱ������

    BOOL                abPasteDetection;               // �Ƿ�Я����ͨATM������Ϣ
    TIME_SCHEDULE_INFO  stPasteDetection;               // ��ͨATM����ʱ������

    BOOL                abLeftDetection;                // �Ƿ�Я����ͨ��Ʒ������Ϣ
    TIME_SCHEDULE_INFO  stLeftDetection;                // ��ͨ��Ʒ����ʱ������

    BOOL                abPreservation;                 // �Ƿ�Я����ͨ��Ʒ��ȫ��Ϣ
    TIME_SCHEDULE_INFO  stPreservation;                 // ��ͨ��Ʒ��ȫʱ������

    BOOL                abTakenAwayDetection;           // �Ƿ�Я����ͨ��Ʒ������Ϣ
    TIME_SCHEDULE_INFO  stTakenAwayDetection;           // ��ͨ��Ʒ����ʱ������

    BOOL                abStayDetection;                // �Ƿ�Я����ͨͣ��/������Ϣ
    TIME_SCHEDULE_INFO  stStayDetection;                // ��ͨͣ��/����ʱ������

    BOOL                abParkingDetection;             // �Ƿ�Я����ͨ�Ƿ�ͣ����Ϣ
    TIME_SCHEDULE_INFO  stParkingDetection;             // ��ͨ�Ƿ�ͣ��ʱ������

    BOOL                abWanderDetection;              // �Ƿ�Я����ͨ�ǻ���Ϣ
    TIME_SCHEDULE_INFO  stWanderDetection;              // ��ͨ�ǻ�ʱ������

    BOOL                abMoveDetection;                // �Ƿ�Я����ͨ�˶���Ϣ
    TIME_SCHEDULE_INFO  stMoveDetection;                // ��ͨ�˶�ʱ������

    BOOL                abTailDetection;                // �Ƿ�Я����ͨβ����Ϣ
    TIME_SCHEDULE_INFO  stTailDetection;                // ��ͨβ��ʱ������

    BOOL                abRioterDetection;              // �Ƿ�Я����ͨ�ۼ���Ϣ
    TIME_SCHEDULE_INFO  stRioterDetection;              // ��ͨ�ۼ�ʱ������

    BOOL                abFightDetection;               // �Ƿ�Я����ͨ�����Ϣ
    TIME_SCHEDULE_INFO  stFightDetection;               // ��ͨ���ʱ������

    BOOL                abRetrogradeDetection;          // �Ƿ�Я����ͨ������Ϣ
    TIME_SCHEDULE_INFO  stRetrogradeDetection;          // ��ͨ����ʱ������

    BOOL                abFireDetection;                // �Ƿ�Я����ͨ������Ϣ
    TIME_SCHEDULE_INFO  stFireDetection;                // ��ͨ����ʱ������

    BOOL                abSmokeDetection;               // �Ƿ�Я����ͨ������Ϣ
    TIME_SCHEDULE_INFO  stSmokeDetection;               // ��ͨ����ʱ������

    BOOL                abNumberStat;                   // �Ƿ�Я����ͨ����ͳ����Ϣ
    TIME_SCHEDULE_INFO  stNumberStat;                   // ��ͨ����ͳ��ʱ������

    BOOL                abVideoAbnormalDetection;       // �Ƿ�Я����ͨ��Ƶ�쳣��Ϣ
    TIME_SCHEDULE_INFO  stVideoAbnormalDetection;       // ��ͨ��Ƶ�쳣ʱ������

    BOOL                abPrisonerRiseDetection;        // �Ƿ�Я��������������������Ϣ
    TIME_SCHEDULE_INFO  stPrisonerRiseDetection;        // ����������������ʱ������

    BOOL                abFaceDetection;                // �Ƿ�Я�����������Ϣ
    TIME_SCHEDULE_INFO  stFaceDetection;                // �������ʱ������

    BOOL                abFaceRecognition;              // �Ƿ�Я������ʶ����Ϣ
    TIME_SCHEDULE_INFO  stFaceRecognition;              // ����ʶ��ʱ������

    BOOL                abDensityDetection;             // �Ƿ�Я���ܼ��ȼ����Ϣ
    TIME_SCHEDULE_INFO  stDensityDetection;             // �ܼ��ȼ��ʱ������

    BOOL                abQueueDetection;               // �Ƿ�Я���ŶӼ����Ϣ
    TIME_SCHEDULE_INFO  stQueueDetection;               // �ŶӼ��ʱ������

    BOOL                abClimbDetection;               // �Ƿ�Я���ʸ߼����Ϣ
    TIME_SCHEDULE_INFO  stClimbDetection;               // �ʸ�ʱ������

    BOOL                abLeaveDetection;               // �Ƿ�Я����ڼ����Ϣ
    TIME_SCHEDULE_INFO  stLeaveDetection;               // ���ʱ������

    BOOL                abVehicleOnPoliceCar;           // �Ƿ�Я�����ؾ�����Ϣ
    TIME_SCHEDULE_INFO  stVehicleOnPoliceCar;           // ���ؾ���ʱ������

    BOOL                abVehicleOnBus;                 // �Ƿ�Я�����ع�����Ϣ
    TIME_SCHEDULE_INFO  stVehicleOnBus;                 // ���ع���ʱ������

    BOOL                abVehicleOnSchoolBus;           // �Ƿ�Я������У����Ϣ
    TIME_SCHEDULE_INFO  stVehicleOnSchoolBus;           // ����У��ʱ������

	BOOL                abTrafficNonMotorHoldUmbrella;  // �Ƿ�Я���ǻ�����װ��ɡ��ʱ������
	TIME_SCHEDULE_INFO  stTrafficNonMotorHoldUmbrella;  // �ǻ�����װ��ɡ��ʱ������
}VIOLATION_TIME_SCHEDULE;

// MixModeConfig�й��ڳ���������Ϣ
typedef struct tagMixModeLaneInfo
{
    unsigned int                nLaneNum;                           // �������ø���
    TRAFFIC_EVENT_CHECK_INFO    stCheckInfo[MAX_LANE_CONFIG_NUMBER];     // �������ö�Ӧ�¼������Ϣ
}MIX_MODE_LANE_INFO;

// MixModeConfig ���ģʽΥ������
typedef struct tagMIX_MODE_CONFIG
{
	BOOL                        bLaneDiffEnable;                    // �Ƿ񰴳�������
	MIX_MODE_LANE_INFO          stLaneInfo;
    TRAFFIC_EVENT_CHECK_INFO    stCheckInfo;
}MIX_MODE_CONFIG;

typedef struct tagPeriodOfValidity
{
    CFG_NET_TIME            stBeginTime;                    // �궨��ʼʱ�� 
    CFG_NET_TIME            stEndTime;                      // �궨����ʱ��
}PERIOD_OF_VALIDITY;

// ��ͨȫ�����ö�Ӧ�궨�������
typedef struct tagTrafficCalibrationInfo
{
    char                    szUnit[CFG_COMMON_STRING_256];              // �궨��λ
    char                    szCertificate[CFG_COMMON_STRING_256];       // �궨֤��
    PERIOD_OF_VALIDITY      stPeriodOfValidity;                         // �궨��Ч��
}TRAFFIC_CALIBRATION_INFO;

// ��ͨ���ö�Ӧ�������
typedef enum tagEmTransferPolicy
{
    EM_TRAFFIC_TRANSFER_UNKNOWN,            // δ֪����
    EM_TRAFFIC_TRANSFER_REALTIME,           // "RealtimePriority" ʵʱ����
    EM_TRAFFIC_TRANSFER_SEQUENCE,           // "SequencePriority" ˳������

}EM_TRANSFER_POLICY;

// ��ͨȫ�����ö�ӦͼƬ������ʽ��������
typedef struct tagTrafficNamingFormat
{
    char                    szFormat[CFG_COMMON_STRING_256];            // ͼƬ��ʽ
}TRAFFIC_NAMING_FORMAT;

// ��ͨȫ�����ö�Ӧ����״̬����
typedef struct tagEnableLightStateInfo
{
    BOOL                    bEnable;      // �Ƿ�����Ӧ�ò��յ��ĵ���״̬���ײ�
}ENABLE_LIGHT_STATE_INFO;

// CFG_CMD_TRAFFICGLOBAL ��ͨȫ���������ñ�
typedef struct tagCFG_TRAFFICGLOBAL_INFO
{
	VIOLATIONCODE_INFO      stViolationCode;                            // Υ�´������ñ�                          
    BOOL                    bEnableRedList;                             // ʹ�ܺ�������⣬ʹ�ܺ������ڳ���Υ�²��ϱ�

    BOOL                    abViolationTimeSchedule;                    // �Ƿ�Я��Υ��ץ���Զ���ʱ������
    VIOLATION_TIME_SCHEDULE stViolationTimeSchedule;                    // Υ��ץ���Զ���ʱ������
    
    BOOL                    abEnableBlackList;                          // �Ƿ�Я��ʹ�ܺ����������Ϣ
    BOOL                    bEnableBlackList;                           // ʹ�ܺ��������

    BOOL                    abPriority;                                 // �Ƿ�Я��Υ�����ȼ�����
    unsigned int            nPriority;                                  // Υ�����ȼ�����
    char                    szPriority[MAX_PRIORITY_NUMBER][CFG_COMMON_STRING_256]; // Υ�����ȼ�, 0Ϊ������ȼ�    

    BOOL                    abNamingFormat;                             // �Ƿ�Я��ͼƬ������ʽ����
    TRAFFIC_NAMING_FORMAT   stNamingFormat;                             // ͼƬ������ʽ��������

    BOOL                    abVideoNamingFormat;                        // �Ƿ�Я��¼��������ʽ����
    TRAFFIC_NAMING_FORMAT   stVideoNamingFormat;                        // ¼��������ʽ��������

    BOOL                    abCalibration;                              // �Ƿ�Я���궨��Ϣ
    TRAFFIC_CALIBRATION_INFO stCalibration;                             // �궨��Ϣ
    
    BOOL                    abAddress;                                  // �Ƿ�Я����ѯ��ַ����
    char                    szAddress[CFG_COMMON_STRING_256];           // ��ѯ��ַ��UTF-8����

    BOOL                    abTransferPolicy;                           // �Ƿ�Я��������Բ���
    EM_TRANSFER_POLICY      emTransferPolicy;                           // �������

    BOOL                    abSupportModeMaskConfig;                    // �Ƿ�Я��Υ������ 
    TRAFFIC_EVENT_CHECK_MASK stSupportModeMaskConfig;                   // Υ������֧�ֵļ��ģʽ��������

    BOOL                    abIsEnableLightState;                       // �Ƿ�Я������״̬
    ENABLE_LIGHT_STATE_INFO stIsEnableLightState;                       // ��ͨȫ�����ö�ӦͼƬ������ʽ��������
    
    BOOL                    abMixModeInfo;                              // �Ƿ��л��ģʽ����
    MIX_MODE_CONFIG         stMixModeInfo;                              // ���ģʽ����
}CFG_TRAFFICGLOBAL_INFO;

// CFG_CMD_VIDEOENCODEROI ��Ƶ����ROI(Region of Intrest)����
#define DH_MAX_QUALITY_REGION_NUM 8
typedef struct tagCFG_VIDEOENCODEROI_INFO
{
	int      nRegionNum;                               // �Ż��������
	CFG_RECT stRegions[DH_MAX_QUALITY_REGION_NUM];     // �Ż����� ֧�ֶ���Ż�����ʹ�����������ϵ��ȡֵ��Ϊ0~8191
	BYTE     byQuality;        // ͼ������ ȡֵ1~6��ֵԽ��ͼ������Խ��
	bool     bMain;            // �Ż���������Ƶ����
	bool     bExtra1;          // �Ż�������1��Ƶ����	
	bool     bExtra2;          // �Ż�������2��Ƶ����
	bool     bExtra3;          // �Ż�������3��Ƶ����
	bool     bSnapshot;        // �Ż�ץͼ����
	BYTE     byReserved2[2];   // ����
}CFG_VIDEOENCODEROI_INFO;

// ATMȡ�ʱ����
typedef struct tagCFG_ATMMOTION_INFO
{
	int nTimeLimit;            // ȡ�ʱʱ��,��λ:��
}CFG_ATMMOTION_INFO;

typedef struct tagCFG_SNAPSOURCE_INFO_SINGLE_CHANNEL
{
	bool		bEnable;			// ʹ��
	BYTE		bReserved1[3];		// �����ֶΣ�����
	BYTE		bDevice[64];		// �豸��
	DWORD		dwChannel;			// ��Ƶͨ����
	DWORD		dwLinkVideoChannel; // ץͼͨ����Ӧ����Ƶͨ����
	BYTE		bReserved[512];		// �����ֶ�
}SNAPSOURCE_INFO_SINGLE_CHANNEL;

typedef struct tagCFG_SNAPSOURCE_INFO
{
	DWORD dwCount;												//Ҫ���õ�ͨ���ĸ���
	SNAPSOURCE_INFO_SINGLE_CHANNEL singleChnSanpInfo[MAX_VIDEO_CHANNEL_NUM];	//SNAPSOURCE_INFO_SINGLE_CHANNEL����ĵ�ַ
}CFG_SNAPSOURCE_INFO;

typedef struct tagDEVICE_STATUS
{
	char	szDeviceName[MAX_REMOTEDEVICENAME_LEN];	//Զ���豸������
	BYTE	bDeviceStatus;							//Զ���豸��״̬ 0������ 1������
	BYTE	bReserved[63];							//�����ֽ�
}DEVICE_STATUS;

typedef struct tagCFG_REMOTE_DEVICE_STATUS
{
	DEVICE_STATUS devStatus[MAX_REMOTE_DEV_NUM]; //�豸״̬
	DWORD		  dwDevCount;					 //�豸����
	BYTE		  bReserved[1024];				 //�����ֽ�
}CFG_REMOTE_DEVICE_STATUS;

// ͬ�ᱨ��������
#define MAX_COAXIAL_ALARM_COUNT		64

// ͬ�ᱨ������
typedef enum tagEM_COAXIAL_ALARM_TYPE
{
	EM_COAXIAL_ALARM_TYPE_UNKNOWN,					// δ֪
	EM_COAXIAL_ALARM_TYPE_EXT_ALARM,				// ͬ���Դ��ⲿ����
	EM_COAXIAL_ALARM_TYPE_MOTION_ALARM,				// ͬ���Դ����챨��
	EM_COAXIAL_ALARM_TYPE_VOLTAGE_ALARM,			// ͬ���Դ���ѹ����
	EM_COAXIAL_ALARM_TYPE_ALARM_LOCAL,				// ͬ�����ط�������������
	EM_COAXIAL_ALARM_TYPE_ALARM_BEL,				// ͬ�����ر�������
	EM_COAXIAL_ALARM_TYPE_TEMPERATURE_ALARM,		// ͬ���¶ȱ���
	EM_COAXIAL_ALARM_TYPE_HUMIDITY_ALARM,			// ͬ��ʪ�ȱ���
} EM_COAXIAL_ALARM_TYPE;

// ��Ʒ������Ϣ
typedef struct tagCFG_PRODUCT_DEFINITION_INFO
{
	int						nCoaxialAlarmCount;									// �豸֧�ֵı�������
	EM_COAXIAL_ALARM_TYPE	emCoaxialAlarm[MAX_COAXIAL_ALARM_COUNT];			// �豸֧�ֵı���
} CFG_PRODUCT_DEFINITION_INFO;

//-----------------------------��Ƶ�����������------------------------------
// �¼����� EVENT_IVS_AUDIO_ABNORMALDETECTION (�����쳣����¼�)��Ӧ��������
typedef struct tagCFG_IVS_AUDIO_ABNORMALDETECTION_INFO
{
	char				szRuleName[MAX_NAME_LEN];								// ��������,��ͬ����������
	bool				bRuleEnable;											// ����ʹ��
	BYTE                bReserved[3];                                           // �����ֶ�
	int                 nDecibel;                                               // ����ǿ�ȣ���λ db
	int                 nFrequency;                                             // ����Ƶ��, ��λ Hz
	int                 nMinDuration;                                           // ��̳���ʱ��, ��λ s
	CFG_ALARM_MSG_HANDLE stuEventHandler;										// ��������
	CFG_TIME_SECTION	stuTimeSection[WEEK_DAY_NUM][MAX_REC_TSECT_EX];			// �¼���Ӧʱ���
}CFG_IVS_AUDIO_ABNORMALDETECTION_INFO;

typedef struct tagCFG_CAP_AUDIO_ANALYSE_INFO
{
	int                 nSupportedRuleNum;                                      // ֧�ֵĹ���������
	DWORD               dwSupportedRules[MAX_RULE_LIST_SIZE];                   // ����֧�ֵĹ�������
}CFG_CAP_AUDIO_ANALYSE_INFO;

//-------------------------IPS && SVR------------------------------

// �豸״̬��Ϣ
typedef struct tagCFG_DEVICESTATUS_INFO
{
	int                 nPowerNum;                               // ��Դ����
	BYTE                byPowerStatus[MAX_POWER_NUM];            // ��Դ״̬��1:���� 2:�쳣 3:δ֪
	int                 nCPUNum;                                 // CPU����
	int                 nCPUTemperature[MAX_CPU_NUM];            // CPU�¶�
	int                 nFanNum;                                 // ���ȸ���
	int                 nRotatoSpeed[MAX_FUN_NUM];               // ����ת��
}CFG_DEVICESTATUS_INFO;

// ��չ����Ϣ
typedef struct tagCFG_HARDDISK_INFO 
{
	int                     nChannel;                        // ͨ����
	int                     nCapacity;                       // Ӳ������ 
	BYTE                    byStatus;                        // Ӳ��״̬��0:unknown 1:running 2:fail 3.spare�ȱ�
															 // 4.syncͬ�� 5.faultyʧЧ 6.rebuilding�ؽ� 7.remove�Ƴ� 8.sleep����
	BYTE                    byUsedStatus;                    // ʹ��״̬��1.���� 2.���� 3.δ֪
    BYTE                    byHotBack;                       // �Ƿ����ȱ��̣�0:�ȱ��� 1:���ȱ���
	BYTE                    byReserved;                      // �ֽڶ���
	char                    szRaidName[MAX_NAME_LEN];        // ����Raid(������)������	"RaidName" : "Raid0",
															 // ����Raid(������)�����ơ��粻�����κ�Raid�����ֶ�Ϊnull��
															 // �����ȱ��̣�����ȫ���ȱ��̵ģ���null��
	char                    szType[32];                      // Ӳ���ͺ�
	int                     nTank;                           // ��չ��, 0:����;1:��չ��1; 2:��չ��2	����
	int						nRemainSpace;					 // ʣ����������λM
}CFG_HARDDISK_INFO;

typedef struct tagCFG_HARDDISKTANK_INFO
{
	char                    szTankName[MAX_NAME_LEN];           // �洢������
	int                     nHardDiskNum;                       // Ӳ�̸���
	CFG_HARDDISK_INFO       stuHarddiskInfo[MAX_HARDDISK_NUM];  // Ӳ����Ϣ
}CFG_HARDDISKTANK_INFO;

typedef struct tagCFG_HARDISKTANKGROUP_INFO
{
	int                      nTankNum;                       // Ӳ�̴洢�����
	CFG_HARDDISKTANK_INFO    stuHarddisktank[MAX_TANK_NUM];  // Ӳ�̴洢������
}CFG_HARDISKTANKGROUP_INFO;

// Raid����Ϣ
typedef struct tagCFG_RAID_INFO
{
	char                    szRaidName[MAX_NAME_LEN];         // Raid����
	BYTE                    byType;                           // ���� 1:Jbob, 2:Raid0, 3:Raid1, 4:Raid5
	BYTE                    byStatus;                         // ״̬  0:unknown, 1:active, 2:degraded, 3:inactive, 4: Resyncing
															  // 5.Clean 6.Unknow 7.Recovering 8.Reshaping 9.Checking 10.NotStarted
	BYTE                    byStatusCount;                    // ״̬����, ״̬��>=1ʱ, ��byStatusGroup��ʾ״̬
	BYTE                    byReserved;						  // �ֽڶ���
	int                     nMember[MAX_CHAN_NUM];            // ��ɴ���ͨ��
	int                     nDiskNUM;                         // ���̸���
	int                     nCapacity;                        // ����
	int                     nTank;                            // ��չ��
	int						nRemainSpace;					  // ʣ����������λM
	BYTE					byStatusGroup[MAX_STATUS_NUM];	  // ״̬����, ����ֵͬbyStatus
}CFG_RAID_INFO;

typedef struct tagCFG_RAIDGROUP_INFO
{
	int                     nRaidNum;                         // Raid����
	CFG_RAID_INFO           stuRaidInfo[MAX_RAID_NUM];        // Raid����Ϣ
}CFG_RAIDGROUP_INFO;

// �洢������Ϣ
typedef struct tagCFG_STORAGEPOOL_INFO
{
	char                   szName[MAX_NAME_LEN];               // �洢������
	int                    nMemberNum;                         // �豸����
	char                   szMember[MAX_DEV_NUM][MAX_NAME_LEN];// ����豸
	int                    nUsed;                              // ��������
    int                    nCapacity;                          // ������
    int                    nStatus;                            // ״̬	0:unknown 1:active 2:degraded 3:inactive
    int                    nTank;                              // ��չ��	0:����, 1:��չ��1, 2:��չ��2 ����
}CFG_STORAGEPOOL_INFO;

typedef struct tagCFG_STORAGEPOOLGROUP_INFO
{
	int                     nStroagePoolNum;                           // �洢�ظ���
	CFG_STORAGEPOOL_INFO    stuStoragePoolInfo[MAX_STORAGEPOOL_NUM];   // �洢����Ϣ
}CFG_STORAGEPOOLGROUP_INFO;

// �ļ�ϵͳ����Ϣ
typedef struct tagCFG_STORAGEPOSITION_INFO
{
	char                   szName[MAX_NAME_LEN];               // �洢λ������	
    char                   szStoragePoolName[MAX_NAME_LEN];    // �洢������	
    int                    nUsedCapacity;                      // ��������,��λG	
    int                    nTotalCapacity;                     // ����,��λG	
	BYTE                   byStatus;                           // ״̬ 0.δ֪ 1.���� 2.�����쳣 3.�����쳣
    BYTE                   byReserved[3];                      // �ֽڶ���
}CFG_STORAGEPOSITION_INFO;

typedef struct tafCFG_STORAGEPOSITIONGROUP_INFO
{
	int                      nStoragePositionNum;                         // �洢��Ϣ����
	CFG_STORAGEPOSITION_INFO stuStoragePositionInfo[MAX_STRORAGEPOS_NUM]; // �ļ�ϵͳ����Ϣ      
}CFG_STORAGEPOSITIONGROUP_INFO;

// ǰ���豸����Ϣ
typedef struct tagCFG_VIDEOINDEV_INFO
{
	char                   szDevName[MAX_NAME_LEN];             // ǰ���豸����	
    char                   szDevID[MAX_NAME_LEN];               // �豸ID	
    char                   szDevType[MAX_NAME_LEN];             // �豸����
    int                    nTotalChan;                          // ��ͨ����
	int                    nTotalAlarmChan;                     // ����ͨ������
    char                   szIP[MAX_ADDRESS_LEN];               // �豸IP
	BYTE                   byStatus;                            // ״̬ 0:δ֪ 1:���� 2:����
	BYTE                   byReserved[3];                       // �ֽڶ���
}CFG_VIDEOINDEV_INFO;

typedef struct tafCFG_VIDEOINDEVGROUP_INFO
{
	int                    nVideoDevNum;                              // ǰ���豸����
	CFG_VIDEOINDEV_INFO    stuVideoInDevInfo[MAX_VIDEODEV_NUM];       // ǰ���豸����Ϣ      
}CFG_VIDEOINDEVGROUP_INFO;

// ͨ��¼����״̬
typedef struct tagCFG_DEVRECORD_INFO
{
	char                   szDevName[MAX_NAME_LEN];               // �豸����
	char                   szIP[MAX_ADDRESS_LEN];                 // �豸IP	
	char                   szChannel[MAX_NAME_LEN];               // ͨ����
    char                   szChannelName[MAX_NAME_LEN];           // ͨ������	
    char                   szStoragePosition[MAX_NAME_LEN];       // �洢λ����Ϣ
	BYTE                   byStatus;                              // ״̬ 0:δ֪ 1:¼�� 2:ֹͣ
	BYTE                   byReserved[3];                         // �ֽڶ���
}CFG_DEVRECORD_INFO;

typedef struct tagCFG_DEVRECORDGROUP_INFO
{
	int                    nChannelNum;                          // ͨ������
	CFG_DEVRECORD_INFO     stuDevRecordInfo[MAX_CHAN_NUM];       // ͨ��¼��״̬��Ϣ
}CFG_DEVRECORDGROUP_INFO;

#define byDahuaII byPrivateII
#define byDahuaIId byPrivateIId

// ����״̬
typedef struct tagCFG_IPSERVER_STATUS
{
	int                    nSupportedServerNum;                  // �ṩ�ķ������
	char                   szSupportServer[MAX_SEVER_NUM][MAX_NAME_LEN];  // �ṩ�ķ������� Svr Svrd(SVR�ػ�����) DataBase DataBased(DataBase�ػ�����) NtpServer NtpServerd(NtpServer�ػ�����) DahuaII DahuaIId(DahuaII�ػ�����) Samba Nfs Ftp iScsi 
	int                    nSvrSuppSubServerNum;                          // Svr�ṩ���ӷ�����Ϣ����
	char                   szSvrSuppSubServer[MAX_SEVER_NUM][MAX_NAME_LEN];           // Svr�ṩ���ӷ�����Ϣ CMS DMS	MTS	SS RMS DBR
	BYTE                   byCMS;                                // 0:δ֪ 1:���� 2:δ����
	BYTE                   byDMS;                                // 0:δ֪ 1:���� 2:δ����
	BYTE                   byMTS;                                // 0:δ֪ 1:���� 2:δ����
	BYTE                   bySS;                                 // 0:δ֪ 1:���� 2:δ����
	BYTE                   byRMS;                                // 0:δ֪ 1:���� 2:δ����
	BYTE                   byDBR;                                // 0:δ֪ 1:���� 2:δ����
	BYTE                   bySvrd;                               // 0:δ֪ 1:���� 2:δ����
	BYTE                   byDataBase;                           // 0:δ֪ 1:���� 2:δ����
	BYTE                   byDataBased;                          // 0:δ֪ 1:���� 2:δ����
	BYTE                   byNtpServer;                          // 0:δ֪ 1:���� 2:δ����
	BYTE                   byNtpServerd;                         // 0:δ֪ 1:���� 2:δ����
	BYTE                   byPrivateII;                          // 0:δ֪ 1:���� 2:δ����
	BYTE                   byPrivateIId;                         // 0:δ֪ 1:���� 2:δ����
	BYTE                   bySAMBA;                              // 0:δ֪ 1:���� 2:δ����
	BYTE                   byNFS;                                // 0:δ֪ 1:���� 2:δ����
	BYTE                   byFTP;                                // 0:δ֪ 1:���� 2:δ����
	BYTE                   byISCSI;                              // 0:δ֪ 1:���� 2:δ����
	BYTE                   byReserved[3];                        // �ֽڶ���
}CFG_IPSERVER_STATUS;

//---------------------------��Ƶ����-----------------------------------
#define MAX_SUPPORT_SPLIT_MODE_NUM    16             // ֧�ֵĻ���ָ�����������
#define MAX_MATRIX_PLAN               4              // �����󷽰���
#define MAX_TOUR_MODE                 16             // �����Ѳ������

typedef enum tagMATRIX_VIEW_SPLITMODE
{
	MATRIX_VIEW_SPLIT1 = 0,
	MATRIX_VIEW_SPLIT2,	
	MATRIX_VIEW_SPLIT4,
	MATRIX_VIEW_SPLIT8,
	MATRIX_VIEW_SPLIT9,	
	MATRIX_VIEW_SPLIT16,
	MATRIX_VIEW_SPLITPIP,
	MATRIX_VIEW_SPLIT6,
	MATRIX_VIEW_SPLIT12,
	MATRIX_VIEW_SPLIT25,
	MATRIX_VIEW_SPLIT36,
	MATRIX_VIEW_SPLIT_NR,
}MATRIX_VIEW_SPLITMODE;

// ��Ѳģʽ
typedef struct tagCFG_TOUR_MODE
{
	int               nViewMode;                                    // ����ָ�ģʽ,�ο�ö������MATRIX_VIEW_SPLITMODE
	unsigned int      dwViewSplitMask;                     // ��ʾViewModeָ��ģʽ��,ʹ�ܵļ����ָ�����,ʹ�������﷽ʽ
													       // ����:0x00000007:��ʾģʽ3(SPLIT8)�ķָ�1,2,3ʹ�ܿ���,����δʹ��,0x0000000F��ʾ�ָ�1,2,3,4ʹ��
}CFG_TOUR_MODE;

// SPOT��Ƶ���󷽰�
typedef struct tagCFG_VIDEO_MATRIX_PLAN
{
	BOOL            bEnable;                                          // �������÷���ʹ��
	int             nTourPeriod;                                      // ������Ѳ���,��λ��,>=1
	int             nTourModeNum;                                     // ��Ѳ���и���
	CFG_TOUR_MODE   stuTourMode[MAX_TOUR_MODE];                       // ��Ѳ����
}CFG_VIDEO_MATRIX_PLAN;

// SPOT��������
typedef struct tagCFG_VIDEO_MATRIX
{
	int                         nSupportSplitModeNumber;         // ֧�ֵĻ���ָ��������
	BYTE                        bySupportSplitMode[MAX_SUPPORT_SPLIT_MODE_NUM];          // ֧�ֵĻ���ָ������
	int                         nMatrixPlanNumber;               // ���󷽰���
	CFG_VIDEO_MATRIX_PLAN       stuMatrixPlan[MAX_MATRIX_PLAN];                // ���󷽰�
}CFG_VIDEO_MATRIX;

///////////////////////////////////��Ƶ�������///////////////////////////////////////
// ��Ƶ�������
typedef struct tagCFG_VIDEO_DITHER_DETECTION
{
	BOOL							bEnable;									// ʹ������
	int								nMinDuration;								// ��̳���ʱ�� ��λ���� 0~65535
	BYTE							byThrehold1;								// Ԥ����ֵ ȡֵ1-100
	BYTE							byThrehold2;								// ������ֵ ȡֵ1-100
}CFG_VIDEO_DITHER_DETECTION;
// ���Ƽ��
typedef struct tagCFG_VIDEO_STRIATION_DETECTION 
{
	BOOL							bEnable;									// ʹ������
	int								nMinDuration;								// ��̳���ʱ�� ��λ���� 0~65535
	BYTE							byThrehold1;								// Ԥ����ֵ ȡֵ1-100
	BYTE							byThrehold2;								// ������ֵ ȡֵ1-100
	BYTE							byReserved1[2];								// �ֽڶ���
	BOOL							bUVDetection;								// UV�����Ƿ���					
}CFG_VIDEO_STRIATION_DETECTION;
// ��Ƶ��ʧ���
typedef struct tagCFG_VIDEO_LOSS_DETECTION
{
	BOOL							bEnable;									// ʹ������
	int								nMinDuration;								// ��̳���ʱ�� ��λ���� 0~65535
}CFG_VIDEO_LOSS_DETECTION;
// ��Ƶ�ڵ����
typedef struct tagCFG_VIDEO_COVER_DETECTION
{
	BOOL							bEnable;									// ʹ������
	int								nMinDuration;								// ��̳���ʱ�� ��λ���� 0~65535
	BYTE							byThrehold1;								// Ԥ����ֵ ȡֵ1-100
	BYTE							byThrehold2;								// ������ֵ ȡֵ1-100
}CFG_VIDEO_COVER_DETECTION;
// ���涳����
typedef struct tagCFG_VIDEO_FROZEN_DETECTION
{
	BOOL							bEnable;									// ʹ������
	int								nMinDuration;								// ��̳���ʱ�� ��λ���� 0~65535
}CFG_VIDEO_FROZEN_DETECTION;
// �����쳣���
typedef struct tagCFG_VIDEO_BRIGHTNESS_DETECTION
{	
	BOOL							bEnable;									// ʹ������
	int								nMinDuration;								// ��̳���ʱ�� ��λ���� 0~65535
	BYTE							bylowerThrehold1;							// Ԥ����ֵ ȡֵ1-100
	BYTE							bylowerThrehold2;							// ������ֵ ȡֵ1-100
	BYTE							byUpperThrehold1;							// Ԥ����ֵ ȡֵ1-100
	BYTE							byUpperThrehold2;							// ������ֵ ȡֵ1-100
}CFG_VIDEO_BRIGHTNESS_DETECTION;
// �Աȶ��쳣���
typedef struct tagCFG_VIDEO_CONTRAST_DETECTION
{	
	BOOL							bEnable;									// ʹ������
	int								nMinDuration;								// ��̳���ʱ�� ��λ���� 0~65535
	BYTE							bylowerThrehold1;							// Ԥ����ֵ ȡֵ1-100
	BYTE							bylowerThrehold2;							// ������ֵ ȡֵ1-100
	BYTE							byUpperThrehold1;							// Ԥ����ֵ ȡֵ1-100
	BYTE							byUpperThrehold2;							// ������ֵ ȡֵ1-100
}CFG_VIDEO_CONTRAST_DETECTION;
// ƫɫ���
typedef struct tagCFG_VIDEO_UNBALANCE_DETECTION
{	
	BOOL							bEnable;									// ʹ������
	int								nMinDuration;								// ��̳���ʱ�� ��λ���� 0~65535
	BYTE							byThrehold1;								// Ԥ����ֵ ȡֵ1-100
	BYTE							byThrehold2;								// ������ֵ ȡֵ1-100
}CFG_VIDEO_UNBALANCE_DETECTION;
// �������
typedef struct tagCFG_VIDEO_NOISE_DETECTION
{	
	BOOL							bEnable;									// ʹ������
	int								nMinDuration;								// ��̳���ʱ�� ��λ���� 0~65535
	BYTE							byThrehold1;								// Ԥ����ֵ ȡֵ1-100
	BYTE							byThrehold2;								// ������ֵ ȡֵ1-100
}CFG_VIDEO_NOISE_DETECTION;
// ģ�����
typedef struct tagCFG_VIDEO_BLUR_DETECTION
{
	BOOL							bEnable;									// ʹ������
	int								nMinDuration;								// ��̳���ʱ�� ��λ���� 0~65535
	BYTE							byThrehold1;								// Ԥ����ֵ ȡֵ1-100
	BYTE							byThrehold2;								// ������ֵ ȡֵ1-100
}CFG_VIDEO_BLUR_DETECTION;
// �����仯���
typedef struct tagCFG_VIDEO_SCENECHANGE_DETECTION
{	
	BOOL							bEnable;									// ʹ������
	int								nMinDuration;								// ��̳���ʱ�� ��λ���� 0~65535
	BYTE							byThrehold1;								// Ԥ����ֵ ȡֵ1-100
	BYTE							byThrehold2;								// ������ֵ ȡֵ1-100
}CFG_VIDEO_SCENECHANGE_DETECTION;

// ��Ƶ��ʱ���
typedef struct tagCFG_VIDEO_DELAY_DETECTION
{
    BOOL    bEnable;                // ʹ������
}CFG_VIDEO_DELAY_DETECTION;

// ��̨�ƶ����
typedef struct tagCFG_PTZ_MOVING_DETECTION
{
    BOOL    bEnable;                // ʹ������
}CFG_PTZ_MOVING_DETECTION;

// �ڰ�ͼ����
typedef struct tagCFG_VIDEO_BLACKWHITE_DETECTION
{
	BOOL	bEnable;				// ʹ������
	int		nEarlyWarning;			// Ԥ����ֵ
	int		nAlarm;					// ������ֵ
	int		nMinDuration;			// ��̳���ʱ��
} CFG_VIDEO_BLACKWHITE_DETECTION;

// ���������
typedef struct tagCFG_VIDEO_DRAMATICCHANGE_DETECTION
{
	BOOL	bEnable;				// ʹ������
	int		nEarlyWarning;			// Ԥ����ֵ
	int		nAlarm;					// ������ֵ
	int		nMinDuration;			// ��̳���ʱ��
} CFG_VIDEO_DRAMATICCHANGE_DETECTION;

// ��Ƶ����ʼ��
typedef struct tagCFG_VIDEO_AVAILABILITY_DETECTION
{
	BOOL	bEnable;				// ʹ������
}CFG_VIDEO_AVAILABILITY_DETECTION;

typedef struct tagCFG_VIDEO_DIAGNOSIS_PROFILE
{
	char								szName[MAX_PATH];							// ����Ansi����

	CFG_VIDEO_DITHER_DETECTION*			pstDither;									// ��Ƶ�������
	CFG_VIDEO_STRIATION_DETECTION*		pstStriation;								// ��Ƶ���Ƽ��
	CFG_VIDEO_LOSS_DETECTION*			pstLoss;									// ��Ƶ��ʧ���
	CFG_VIDEO_COVER_DETECTION*			pstCover;									// ��Ƶ�ڵ����
	CFG_VIDEO_FROZEN_DETECTION*			pstFrozen;									// ��Ƶ������
	CFG_VIDEO_BRIGHTNESS_DETECTION*		pstBrightness;								// ��Ƶ�����쳣���
	CFG_VIDEO_CONTRAST_DETECTION*		pstContrast;								// �Աȶ��쳣���
	CFG_VIDEO_UNBALANCE_DETECTION*		pstUnbalance;								// ƫɫ�쳣���
	CFG_VIDEO_NOISE_DETECTION*			pstNoise;									// �������
	CFG_VIDEO_BLUR_DETECTION*			pstBlur;									// ģ�����
	CFG_VIDEO_SCENECHANGE_DETECTION* 	pstSceneChange;								// �����仯���
    CFG_VIDEO_DELAY_DETECTION*       	pstVideoDelay;                             	// ��Ƶ��ʱ���
    CFG_PTZ_MOVING_DETECTION*        	pstPTZMoving;                              	// ��̨�ƶ����
	CFG_VIDEO_BLACKWHITE_DETECTION*	 	pstBlackAndWhite;							// �ڰ�ͼ����
	CFG_VIDEO_DRAMATICCHANGE_DETECTION*	pstDramaticChange;							// ���������
	CFG_VIDEO_AVAILABILITY_DETECTION*	pstVideoAvailability;					    // ��Ƶ����ʼ��
}CFG_VIDEO_DIAGNOSIS_PROFILE;

// ��Ƶ��ϲ�����(CFG_CMD_VIDEODIAGNOSIS_PROFILE)��֧�ֶ��ֲ������ñ�����������   �����������ڴ沢��ʼ��
typedef struct tagCFG_VIDEODIAGNOSIS_PROFILE
{
	int								nTotalProfileNum;							// �����߷���������� ������������ȡ
	int								nReturnProfileNum;							// ���ص�ʵ�ʲ�������
	CFG_VIDEO_DIAGNOSIS_PROFILE*	pstProfiles;								// �����߷���nTotalProfileNum��CFG_VIDEO_DIAGNOSIS_PROFILE
}CFG_VIDEODIAGNOSIS_PROFILE;

///// ��Ƶ�������
enum CFG_EM_STREAM_TYPE
{
	CFG_EM_STREAM_ERR,                  // ����
	CFG_EM_STREAM_MAIN,					// "Main"-������
	CFG_EM_STREAM_EXTRA_1,				// "Extra1"-������1
	CFG_EM_STREAM_EXTRA_2,				// "Extra2"-������2
	CFG_EM_STREAM_EXTRA_3,				// "Extra3"-������3
	CFG_EM_STREAM_SNAPSHOT,				// "Snapshot"-ץͼ����
	CFG_EM_STREAM_OBJECT,				// "Object"-������
};

// �豸��ϸ��Ϣ
typedef struct tagCFG_TASK_REMOTEDEVICE
{
	char                            szAddress[MAX_PATH];                        // �豸��ַ������
	unsigned int                    dwPort;                                     // �˿ں�
	char                            szUserName[MAX_PATH];                       // �û���
	char                            szPassword[MAX_PATH];                       // ��������
	char                            szProtocolType[MAX_PATH];                   // �����豸��Э������
	CFG_RemoteDeviceVideoInput      *pVideoInput;                               // ��Ƶ����ͨ�����û�����nMaxVideoInputs��CFG_RemoteDeviceVideoInput�ռ�
	int				                nMaxVideoInputs;					        // ��Ƶ����ͨ�������
	int				                nRetVideoInputs;					        // ���ص���Ƶ����ͨ����
}CFG_TASK_REMOTEDEVICE;

// ��Ƶ���¼�񱣴�λ��
typedef enum tagEM_SOURCE_VIDEO_LOCATION
{
    EM_SOURCE_VIDEO_LOCATION_UNKNOWN,            // δ֪
    EM_SOURCE_VIDEO_LOCATION_DEVICE,             // �����豸
    EM_SOURCE_VIDEO_LOCATION_CENTER,             // ����ƽ̨
    EM_SOURCE_VIDEO_LOCATION_THIRD,              // ���ڵ�������
} EM_SOURCE_VIDEO_LOCATION;

// ¼�������
typedef enum tagEM_VIDEO_TYPE
{
    EM_VIDEO_TYPE_UNKNOWN,           // δ֪
    EM_VIDEO_TYPE_ALL,               // ȫ��
    EM_VIDEO_TYPE_MANUAL,            // �ֶ������¼��
    EM_VIDEO_TYPE_ALARM,             // ����¼��
} EM_VIDEO_TYPE;

// ��Ƶ���¼����ϸ��Ϣ
typedef struct tagNET_VIDEO_DIAGNOSIS_RECORD_INFO
{
    EM_SOURCE_VIDEO_LOCATION    emSourceLocation;           // ¼�񱣴��λ��
    EM_VIDEO_TYPE               emRecordType;               // ��Ƶ���¼������
    CFG_NET_TIME                stuStartTime;               // ��Ƶ���¼��ʼʱ��
    int                         nPreSeconds;                // ��Ƶ���¼��ʼʱ��Ϊ��ǰʵ�ʼ��ʱ���ǰPreSeconds�루�����ֶ���StartTimeͬʱ����ʱ���Ը��ֶ����ȣ�
} NET_VIDEO_DIAGNOSIS_RECORD_INFO;

// ��ƵԴ���뷽ʽ
typedef enum tagEM_VIDEO_CHANNEL_SOURCE_INPUT_TYPE
{
    EM_VIDEO_CHANNEL_SOURCE_INPUT_UNKNOWN,          // δ֪
    EM_VIDEO_CHANNEL_SOURCE_INPUT_STD,              // STD
    EM_VIDEO_CHANNEL_SOURCE_INPUT_AHD,              // AHD
    EM_VIDEO_CHANNEL_SOURCE_INPUT_HDCVR,            // HDCVR
    EM_VIDEO_CHANNEL_SOURCE_INPUT_VGA,              // VGA
} EM_VIDEO_CHANNEL_SOURCE_INPUT_TYPE;

typedef struct tagCFG_TAST_SOURCES
{
	// ����
	bool							abDeviceID;                                 // abDeviceID(ʹ��szDeviceID) �� abRemoteDevice(ʹ��stRemoteDevice) ������һ��Ϊtrue������Sources��null
	bool							abRemoteDevice;

	char							szDeviceID[MAX_PATH];						// �豸ID
	CFG_TASK_REMOTEDEVICE           stRemoteDevice;                             // �豸��ϸ��Ϣ
	int								nVideoChannel;								// ��Ƶͨ����
	CFG_EM_STREAM_TYPE				emVideoStream;								// ��Ƶ��������
	int								nDuration;									// �������ʱ��
    BOOL                            abStartTime;                                // ��ʾstuStartTime�ֶ��Ƿ���Ч; ��ԴΪ¼���ļ�������ΪTRUE, �������ó�FALSE.
    CFG_NET_TIME                    stuStartTime;                               // ��abStartTimeΪTRUEʱ��Ч����ʾҪ������¼��Ŀ�ʼʱ��
                                                                                // stuStartTime�ֶ����ϣ�������ʹ��stuDiagnosisRecordInfo���������ֶ�ֻΪ�������豸
    BOOL                            abDiagnosisRecordInfo;                      // ��ʾstuDiagnosisRecordInfo�ֶ��Ƿ���Ч; ��ԴΪ¼���ļ�������ΪTRUE, �������ó�FALSE.
    NET_VIDEO_DIAGNOSIS_RECORD_INFO stuDiagnosisRecordInfo;                     // ��Ƶ���¼����Ϣ, �� abDiagnosisRecordInfo ΪTRUE��Ч
    EM_VIDEO_CHANNEL_SOURCE_INPUT_TYPE emSourceInputType;                       // ��ƵԴ���뷽ʽ
}CFG_TAST_SOURCES;

typedef struct tagCFG_DIAGNOSIS_TASK
{
	char							szTaskName[MAX_PATH];						// ��������Ansi����
	char							szProfileName[MAX_PATH]; 					// ������ʹ�õ���ϲ�������Ansi����
	int								nTotalSourceNum;						    // �����߷�����������Դ�ĸ���  ������������ȡ
	int								nReturnSourceNum;							// ����ʵ����������Դ�ĸ���
	CFG_TAST_SOURCES*				pstSources;									// ��������Դ �����߷����ڴ�nTotalSourceNum��
}CFG_DIAGNOSIS_TASK;

// ��Ƶ��������(CFG_CMD_VIDEODIAGNOSIS_TASK),��ͬ������ͨ����������  �����������ڴ沢��ʼ��
typedef struct tagCFG_VIDEODIAGNOSIS_TASK
{
	int								nTotalTaskNum; 								// �����߷����������  ������������ȡ
	int								nReturnTaskNum; 							// ����ʵ���������
	CFG_DIAGNOSIS_TASK*				pstTasks;									// �������� �����߷����ڴ�nTotalTaskNum��
}CFG_VIDEODIAGNOSIS_TASK;

// ��Ƶ��ϼƻ�
typedef struct tagPROJECT_TASK
{
	BOOL							bEnable;									// �����Ƿ�ʹ��
	char							szTaskName[MAX_PATH];						// ��������Ansi����
	CFG_TIME_SECTION				stTimeSection[WEEK_DAY_NUM][MAX_REC_TSECT];	// ����ʱ���
	BOOL                            bIsCycle;                                   // �����Ƿ�ѭ��,TRUE��ʾѭ��,FALSE��ʾ��ѭ��
}CFG_PROJECT_TASK;

typedef struct tagDIAGNOSIS_PROJECT
{
	char							szProjectName[MAX_PATH];					// �ƻ�����Ansi����
	int								nTotalTaskNum;								// �����߷��������б����  ������������ȡ
	int								nReturnTaskNum;					    		// ����ʵ�������б����
	CFG_PROJECT_TASK*				pstProjectTasks;								// �����б� �����߷����ڴ�nTotalTaskNum��
}CFG_DIAGNOSIS_PROJECT;
// Ƶ��ϼƻ���(CFG_CMD_VIDEODIAGNOSIS_PROJECT),��ͬ�ļƻ�ͨ���������� �����������ڴ沢��ʼ��
typedef struct tagCFG_VIDEODIAGNOSIS_PROJECT
{
	int								nTotalProjectNum;							// �����߷���ƻ�����  ������������ȡ
	int								nReturnProjectNum;							// ����ʵ�ʼƻ�����
	CFG_DIAGNOSIS_PROJECT*			pstProjects;									// �ƻ����� �����߷����ڴ�nTotalProjectNum��
}CFG_VIDEODIAGNOSIS_PROJECT;

// ʵʱ�ƻ������Ϣ
typedef struct tagCFG_DIAGNOSIS_REALPROJECT
{
	char							szProjectName[MAX_PATH];					// �ƻ�����Ansi����
	CFG_DIAGNOSIS_TASK			    stProjectTask;							    // ������Ϣ
}CFG_DIAGNOSIS_REALPROJECT;

// ��Ƶ���ʵʱ�ƻ���
typedef struct tagCFG_VIDEODIAGNOSIS_REALPROJECT 
{
	int								nProjectNum;							    // ʵʱ�ƻ�����
	CFG_DIAGNOSIS_REALPROJECT*		pstProjects;							    // �ƻ����� �����߷����ڴ�nProjectNum��
}CFG_VIDEODIAGNOSIS_REALPROJECT;

// ��Ƶ���ȫ�ֱ�(CFG_CMD_VIDEODIAGNOSIS_GLOBAL),ÿ��ͨ��֧��һ��������� 
typedef struct tagCFG_VIDEODIAGNOSIS_GLOBAL_CHNNL
{
	BOOL                            abProjectName;                              // �ƻ������Ƿ���Ч 
	char							szProjectName[MAX_PATH];					// �ƻ����ƣ�Ansi����
	BOOL                            abRealProjectName;                          // ʵʱ�ƻ������Ƿ���Ч
	char                            szRealProjectName[MAX_PATH];                // ʵʱ�ƻ����ƣ�Ansi����   
}CFG_VIDEODIAGNOSIS_GLOBAL_CHNNL;

typedef struct tagCFG_VIDEODIAGNOSIS_GLOBAL
{
	int								nTotalGlobalNum;							// �����߷���ȫ�����ø���  ������������ȡ
	int								nReturnGlobalNum;							// ����ʵ��ȫ�����ø���
	CFG_VIDEODIAGNOSIS_GLOBAL_CHNNL	*pstGlobals;								// ��Ƶ���ȫ������ �����߷����ڴ�nTotalGlobalNum��CFG_VIDEODIAGNOSIS_GLOBAL_CHNNL
}CFG_VIDEODIAGNOSIS_GLOBAL;

// ��Ƶ��Ϸ���������(CFG_CAP_CMD_VIDEODIAGNOSIS_SERVER)
typedef struct tagCFG_VIDEODIAGNOSIS_CAPS_INFO
{
	int								nTypeCount;									// ֧�ֵ���Ƶ������� ����
	char						    szSupportedType[MAX_VIDEODIAGNOSIS_DETECT_TYPE][MAX_PATH];// ֧�ֵ���Ƶ�������
	int								nMaxProfiles;								// �����������
	int								nMaxTasks;									// ����������
	int								nMaxSourcesOfTask;							// ��󵥸��������ƵԴ����
	int								nMaxProjects;								// ��󷽰�����
}CFG_VIDEODIAGNOSIS_CAPS_INFO;

// ��ȡ��Ƶ���ͨ����Ŀ( CFG_CMD_VIDEODIAGNOSIS_GETCOLLECT )��Ӧ�ṹ��
typedef struct tagCFG_VIDEODIAGNOSIS_COLLECT_INFO
{
	int                             nDiagnosisChannels;                                   // ��Ƶ���ͨ����Ŀ
}CFG_VIDEODIAGNOSIS_COLLECT_INFO;

// ��ȡ��Ƶ��Ͻ���״̬( CFG_CMD_VIDEODIAGNOSIS_GETSTATE )��Ӧ�ṹ��
typedef struct tagCFG_VIDEODIAGNOSIS_STATE_INFO
{
	BOOL                            bEnable;                                     // �Ƿ�ʹ��
	BOOL                            bRunning;                                    // �Ƿ���������	ʹ�ܿ��������ҵ�ǰ�й����������ڽ��У�û�д�������
	char                            szCurrentProject[MAX_PATH];                  // ��ǰ�ƻ�����
	char                            szCurrentTask[MAX_PATH];                     // ��ǰ��������
	char                            szCurrentProfile[MAX_PATH];                  // ��ǰ�������ò���������
	int                             nCurrentSourceCount;                         // ��ǰ������ѵ��ƵԴ����
	int                             nCurrentSourceIndex;                         // ��ǰ����ǰ��ƵԴ���	��0��ʼ
	CFG_TIME_SECTION                stCurrentTimeSection;                        // ��ǰ�ƻ�ʱ���
	int                             nTaskCountOfProject;                         // ��ǰ�ƻ���������
	int                             nIndexOfCurrentTask;                         // ��ǰ������� ��0��ʼ
}CFG_VIDEODIAGNOSIS_STATE_INFO;

// ������֧�ֵķ����б�
typedef struct tagCFG_DEV_SERVICE_LIST
{
	int                             nServiceNum;                                 // ֧�ֵķ�����
	char                            szService[MAX_SERVICE_NUM][MAX_NAME_LEN];    // ������֧�־���ķ�����
}CFG_DEV_SERVICE_LIST;

//��ȡ����ʽ��������Ŀ
typedef struct tagCFG_MASTERSLAVETRACKER_INFO
{
        int                nStructSize;
	    int                nChannels;	//����ʽ������ͨ����Ŀ
}CFG_MASTERSLAVETRACKER_INFO;

#define  MAX_CAMERA_TYPE	64
#define	 MAX_TYPE_STRING	64

typedef struct tagCFG_CAMERA_PER_TYPE_INFO
{
	char				szCameraType[MAX_TYPE_STRING];	//����ͺ�
	char				szLensType[MAX_TYPE_STRING];	//�����ͷ�ͺ�	��ͷ�ͺź�����ͺ���أ�����Ϊ��ֵ
}CFG_CAMERA_PER_TYPE_INFO;

typedef struct tagCFG_CAMERA_TYPE_INFO
{
	int							nCameraTypeNum;						//�����͵���������еĲ�ͬ�ͺ���
	CFG_CAMERA_PER_TYPE_INFO	stCameraTypeInfos[MAX_CAMERA_TYPE]; //������ͺ���Ϣ
}CFG_CAMERA_TYPE_INFO;

// �䱶����
typedef enum tagCFG_ZOOM_TYPE
{
	ZOOM_TYPE_EXPECTMULTIPLE =0,     // �̶����������䱶
	ZOOM_TYPE_IVSADAPTIVE,           // ���������豸�������Ӧ�䱶
	ZOOM_TYPE_MULTIPLEADAPTIVE,      // ����Ӧ�䱶
	ZOOM_TYPE_NUM
}CFG_ZOOM_TYPE;

// ����ģʽ
typedef enum tagCFG_TRACKING_MODE
{
    TRACKING_MODE_ALARM=0,        // ��������
	TRACKING_MODE_MANUALOBJECT,   // �ֶ�ѡ���������
	TRACKING_MODE_AUTOOBJECT,     // �Զ�ѡ���������
	TRACKING_MODE_SELECTPOINT,    // �������
	TRACKING_MODE_MIX,            // ��ϸ���(�������ָ���ģʽͬʱ֧��)
	TRACKING_MODE_SLAVESELF,      // ����������ƣ���Ҫ�����û���������ƣ�
	TRACKING_MODE_NUM
}CFG_TRACKING_MODE;


// ����ʽ������ȫ�����û����ṹ��
typedef struct tagCFG_MASTERSLAVE_GLOBAL_INFO
{
	int						nStructSize;
	float					fMaxZoom;						// ���䱶����, ֻ����������С��������
	float					fMinZoom;						// ��С�䱶����, ֻ����������С��������
	float					fExpectMultiple;				// ������������׼����������С�䱶����<��������<���䱶����, ֻ����������С��������
	float					fPollTrackTime;					// ��ѯ����ʱ������λ���룻>0.0
	float					fPointTrackTime;				// �������ʱ��,��λ���룻>0.0�ڱ������١��Զ�ѡ��������١���ϸ���ģʽ����Ч
    float					fObjectTrackTime;				// �ֶ�ѡ���������ʱ��, ��λ���룻>0.0�ڱ������١��Զ�ѡ��������١���ϸ���ģʽ����Ч
	float					fExpectAngleX;					// ����������Ӧ����Ƕ�x(ˮƽ), ֻ����������С��������
    float					fExpectAngleY;					// ����������Ӧ����Ƕ�y(��ֱ), ֻ����������С��������
	CFG_SIZE				stTargetFaceSize;				// ����ߴ�(��λ������), ����豸������������й�, ֻ����������С��������
	float					fMinFocus;						// ��С����(��λ������), ֻ����������С��������
	float					fMaxFocus;						// ��󽹾�(��λ������), ֻ����������С��������
	CFG_ZOOM_TYPE			emZoomType;						// �䱶����
	CFG_TRACKING_MODE		emTrackingMode;					// ����ģʽ
	CFG_CAMERA_TYPE_INFO	stCameraInfo;					// ǹʽ�����
	CFG_CAMERA_TYPE_INFO	stDomeInfo;						// ��ʽ�����
	int						nTrackZoomLevel;				// ���ٱ����ȼ�	-5<= TrackZoomLevel <=5 0��ʾ��Ĭ���㷨��ʽ
	BOOL					bReturnPtzPreset;				// TRUE�����ٺ󷵻�Ԥ�õ� , FALSE�����ٺ󲻷���Ԥ�õ�
}CFG_MASTERSLAVE_GLOBAL_INFO;
	
// ��ȡ����ʽ�豸������
typedef struct tagCFG_CAP_MASTERSLAVE_INFO
{
	int                     nStructSize;
	BYTE                    byTrackingMode[TRACKING_MODE_NUM]; // �����±��ʾCFG_TRACKING_MODEö�����Ӧģʽ��1��ʾ֧��
	CFG_CAMERA_TYPE_INFO	stCameraInfo;  // ǹʽ�����
	CFG_CAMERA_TYPE_INFO	stDomeInfo;	   // ��ʽ�����
}CFG_CAP_MASTERSLAVE_INFO;


// ��ȡ��������������������
typedef struct tagCFG_CAP_EVENTHANDLER_INFO
{
	BOOL				bTimeSectionEnable;                                      // 
	BOOL				bRecordEnable;                                           // �Ƿ�֧��¼��
	BOOL				bAlarmOutEnable;                                         // �Ƿ�֧�ֱ������
	BOOL				bPtzLinkEnable;                                          // �Ƿ�֧�ֱ������
	BOOL				bSnapshotEnable;                                         // �Ƿ�֧�ֿ���
	BOOL				bMailEnable;                                             // �Ƿ�֧�ַ����ʼ�
	BOOL				bBeepEnable;                                             // �Ƿ�֧�ַ���
	BOOL				bDejitterEnable;                                         // �Ƿ�֧��ȥ����
	BOOL                bTipEnable;                                              // �Ƿ�֧�ֱ�����Ϣ����ʾ
	BOOL                bMonitorTourEnable;                                      // �Ƿ�֧����ѯ��������
}CFG_CAP_EVENTHANDLER_INFO;

#define MASTERSLAVE_LIST_MAX_NUM            5                              // ǹ������������ӻ�����
#define MASTERSLAVE_AREA_MAX_NUM            8                              // ǹ������������ȼ��������
#define MASTERSLAVE_NAME_LEN                64                             // ��ص�������Ƴ���
#define MASTERSLAVE_DEV_SERIALNO_LEN        48                             // �豸���к���󳤶�

// ǹ������ȫ������ (CFG_CMD_MASTERSLAVE_LINKAGE)
typedef struct tagCFG_MASTERSLAVE_LINKAGE_INFO
{
    BOOL                        bEnable;                                   // ǹ������ʹ���ܿ���
    BOOL                        bAutoTrack;                                // �Զ�����ʹ��
    int                         nAutoTrackTime[2];                         // �Զ�����ʱ����Χ, [��Сʱ��, ���ʱ��], Ĭ��[1, 300], ��λ��
    int                         nLocateUnlockTime;                         // �ֶ���λ����ʱ��(��), 0 ��ʾ��������, Ĭ��30��. ����������ڸ���
    int                         nPriorityAreaNum;                          // ���ȼ��������
    CFG_RECT                    stuPriorityArea[MASTERSLAVE_AREA_MAX_NUM]; // ���ȼ�����, ����Խ��ǰ���������ȼ�Խ�ߣ���Ϊ��
}CFG_MASTERSLAVE_LINKAGE_INFO;

typedef enum tagEM_MASTER_TYPE
{
    EM_MASTER_UNKNOWN,                                                     // δ֪����
    EM_MASTER_FISHEYE,                                                     // ��������
    EM_MASTER_CAMERA,                                                      // ǹ������
}EM_MASTER_TYPE;

typedef struct tagCFG_MASTER_DEV_INFO
{
    int                    nChannel;                                       // ����ͨ����, ��0��ʼ
    char                   szDevSerial[MASTERSLAVE_DEV_SERIALNO_LEN];      // �豸���к�
}CFG_MASTER_DEV_INFO;

typedef struct tagCFG_SLAVER_DEV_INFO
{
    int                     nChannel;                                      // ����ͨ����, ��0��ʼ
    char                    szDevSerial[MASTERSLAVE_DEV_SERIALNO_LEN];     // �豸���к�
    CFG_REGION              stuControlRegion;                              // �ӻ��ܿ�����
}CFG_SLAVER_DEV_INFO;

// ǹ�������󶨹�ϵ���� (CFG_CMD_MASTERSLAVE_GROUP)
typedef struct tagCFG_MASTERSLAVE_GROUP_INFO
{
    char                    szName[MASTERSLAVE_NAME_LEN];                  // ��ص�����
    int                     nGroupID;                                      // ���
    EM_MASTER_TYPE          emMasterType;                                  // ���豸����
    int                     nMasterNum;                                    // �������� 
    CFG_MASTER_DEV_INFO     stuMasterList[MASTERSLAVE_LIST_MAX_NUM];       // �����б�
    int                     nSlaverNum;                                    // �ӻ�����
    CFG_SLAVER_DEV_INFO     stuSlaverList[MASTERSLAVE_LIST_MAX_NUM];       // �ӻ��б�
}CFG_MASTERSLAVE_GROUP_INFO;

//----------------------------------��Ƶ��������------------------------------------------
#define DH_MAX_INMETERING_REGION_NUM 8

typedef struct tagVIDEO_INMETERING_INFO_CHANNEL
{
	// ����
	bool            bRegion;
	bool            bMode;
	BYTE			bReserved1[2];					         // �����ֶ�1
	
	int				nRegionNum;                                 // ����������
	CFG_RECT    	stuRegions[DH_MAX_INMETERING_REGION_NUM];   // �������, �ֲ����ʹ�ã�֧�ֶ���������ʹ�����������ϵ��ȡֵ��Ϊ0~8191
	BYTE  			byMode;				                     // ���ģʽ,0:ƽ�����,1:�ֲ����
	BYTE  			bReserved2[3];				             // �����ֶ�2
	BYTE			bReserved3[32];					         // �����ֶ�3
}VIDEO_INMETERING_INFO_CHANNEL;

// �������(CFG_CMD_VIDEO_INMETERING)��һ�����飬ÿ����Ƶ����ͨ��һ������
typedef struct tagCFG_VIDEO_INMETERING_INFO
{
	int				nChannelNum; // ͨ����
	VIDEO_INMETERING_INFO_CHANNEL	stuMeteringMode[MAX_VIDEO_CHANNEL_NUM]; // ÿ��ͨ���Ĳ����Ϣ���±��Ӧͨ����
}CFG_VIDEO_INMETERING_INFO;

// ��Ƶ����������(CFG_CAP_CMD_VIDEOINPUT)����
typedef struct tagCFG_CAP_VIDEOINPUT_INFO
{
	int				nMeteringRegionCount;                   // ������������
	BOOL            bFishEye;                               // �Ƿ�֧������
	BOOL			bElectricFocus;							// �Ƿ�֧�ֵ綯����
	DWORD			dwExposureMode;							// ֧�ֵ��Զ��ع�ģʽ����  �ӵ�λ����λ���α�ʾ��0-�Զ��ع⣬1-�����������棩���ȣ�2-����Ӱ�����ţ����ȣ� 3-50HZ����˸��4-60HZ����˸
	int             nWideDynamicRange;						// �Ƿ�֧�ֿ�̬ 0-��֧�֣�1-֧�����ֿ�̬
	int			    nGlareInhibition;						// �Ƿ�֧��ǿ������ 0-��֧�֣�1-֧��
}CFG_CAP_VIDEOINPUT_INFO;

// ����ͳ�Ʊ�����Ϣ����
typedef struct tagCFG_TRAFFIC_FLOWSTAT_ALARM_INFO
{
	bool                            bEnable;                              // �Ƿ�ʹ��
	int                             nPeriod;                              // ͳ�����ڣ���λ������
	int                             nLimit;                               // ͳ�������ڳ����������ޣ���λ����
	int                             nRestore;                             // ͳ�������ڱ����ָ�����������λ���� 
    int                             nDelay;                               // ��⵽������������ʼ�ϱ���ʱ��, ��λ:��,��Χ1~65535
	int                             nInterval;                            // �������ʱ��, ��λ:��, ��Χ1~65535
	int                             nReportTimes;                         // �ϱ�����,1~255
	CFG_TIME_SECTION                stCurrentTimeSection[WEEK_DAY_NUM][MAX_REC_TSECT_EX]; // ��ǰ�ƻ�ʱ���
	CFG_ALARM_MSG_HANDLE            stuEventHandler;					  // ��������
}CFG_TRAFFIC_FLOWSTAT_ALARM_INFO;

typedef struct tagCFG_TRAFFIC_FLOWSTAT_INFO_CHNL
{
	// ����
	bool	                		 abEnable;

	bool			                 bEnable;								// �Ƿ�ʹ��
	CFG_TRAFFIC_FLOWSTAT_ALARM_INFO  stuAlarmUpperInfo;                     // �������߲���
	CFG_TRAFFIC_FLOWSTAT_ALARM_INFO  stuAlarmLowInfo;                       // �������߲���
    int                              nDetectRegionPoint;                    // �����������
    CFG_POLYGON                      stuDetectRegion[MAX_POLYGON_NUM];      // �������
	char                             szName[MAX_NAME_LEN];                  // ��ǰ�����������ƣ���ͬ����������
	int								 nPresetID;								// ��̨����Ԥ�õ���(�������Ч)
	BOOL							 bIsDetectLine;							// ��ʾ������������Ƿ���Ч
	CFG_POLYGON						 stuDetectLine[POINT_PAIR_NUM];			// �����������,�������ã��㷨�Զ����ɼ����
}CFG_TRAFFIC_FLOWSTAT_INFO_LANE;

// ��ͨ����ͳ������(CFG_CMD_TRAFFIC_FLOWSTAT)
typedef struct tagCFG_TRAFFIC_FLOWSTAT_INFO
{
	// ����
	bool							abPeriod;
	BYTE							bReserved1[3];						        // �����ֶ�1

	int								nPeriod;									// ͳ�����ڣ���λ����
	int								nLaneNum;									// ������
	CFG_TRAFFIC_FLOWSTAT_INFO_LANE	stuTrafficFlowstat[MAX_LANE_NUM];			// ÿ���������������ã��±��Ӧ������
	DWORD                           dwLaneExtraMaxNum;                           // �������������ֵ(���������ڴ�),�ೡ��Ӧ�ö���ĳ�����Ϣ��Ҫ
	DWORD                           dwLaneExtraRetNum;                           // �ೡ��Ӧ�ö���ĳ�����ʵ�ʸ���
	CFG_TRAFFIC_FLOWSTAT_INFO_LANE  *pstuTrafficFlowstat;                       // �ೡ���ĳ�����, stuTrafficFlowstat����Ų����ĳ���������������棬��Ŵӳ���MAX_LANE_NUM+1��ʼ
}CFG_TRAFFIC_FLOWSTAT_INFO;

//��ƵŨ����������(CFG_CMD_SYNOPSISANALYSE_RULE_INFO)
typedef struct tagCFG_VIDOE_SYNOPSIS_ANALYSE_RULE_INFO
{
    CFG_ANALYSERULES_INFO   stDetailRuleInfo;       //��ϸ����                       
}CFG_VIDOE_SYNOPSIS_ANALYSE_RULE_INFO;

//-----------------------------------��Ƶ����ǰ��ѡ��-------------------------------------------

// ��Ƶ����ҹ����������ѡ������Ϲ��߽ϰ�ʱ�Զ��л���ҹ������ò���
typedef struct tagCFG_VIDEO_IN_NIGHT_OPTIONS
{
	BYTE				bySwitchMode;			//�ѷ���,ʹ��CFG_VIDEO_IN_OPTIONS�����bySwitchMode
                                                //0-���л�������ʹ�ð������ã�1-���������л���2-����ʱ���л���3-���л�������ʹ��ҹ�����ã�4-ʹ����ͨ����
	BYTE				byProfile;				// ��ǰʹ�õ������ļ�.
												// 0-����
												// 1-����
												// 2-Normal
												// 0��1,2��Ϊ��ʱ���ã�ʹͼ����Ч�����ڲ鿴ͼ�����Ч���������ȷ�����뿪ҳ�治�������豸��
												///3-����ʱ���ã����ȷ���󱣴����豸����SwitchMode���ʹ�ã�����SwitchMode����������Ч�����á�
												// SwitchMode=0��Profile=3�����ð������õ��豸��
												// SwitchMode=1��Profile=3��������ҹ�����õ��豸
												// SwitchMode=2��Profile=3�������ճ�����ʱ����л�������ʱ���ʹ�ð������ã�ҹ��ʱ���ʹ��ҹ�����ã��������豸��
												// SwitchMode=4��Profile=3��ʹ����ͨ���ã��������豸
	BYTE				byBrightnessThreshold ;	// ������ֵ 0~100	
	BYTE				bySunriseHour;			// �����ճ�������ʱ�䣬����֮���ճ�֮ǰ��������ҹ����������á�
	BYTE				bySunriseMinute;		// 00:00:00 ~ 23:59:59
	BYTE				bySunriseSecond;
	BYTE				bySunsetHour;
	BYTE				bySunsetMinute;
	BYTE				bySunsetSecond;  
	BYTE				byGainRed;				// ��ɫ������ڣ���ƽ��Ϊ"Custom"ģʽ����Ч 0~100
	BYTE				byGainBlue;				// ��ɫ������ڣ���ƽ��Ϊ"Custom"ģʽ����Ч 0~100
	BYTE				byGainGreen;			// ��ɫ������ڣ���ƽ��Ϊ"Custom"ģʽ����Ч 0~100
	BYTE				byExposure;				// �ع�ģʽ��ȡֵ��Χȡ�����豸��������0-�Զ��ع⣬1-�ع�ȼ�1��2-�ع�ȼ�2��n-1����ع�ȼ��� n��ʱ�������޵��Զ��ع� n+1�Զ���ʱ���ֶ��ع� (n==byExposureEn��
	float				fExposureValue1;		// �Զ��ع�ʱ�����޻����ֶ��ع��Զ���ʱ��,����Ϊ��λ��ȡֵ0.1ms~80ms
	float				fExposureValue2;		// �Զ��ع�ʱ������,����Ϊ��λ��ȡֵ0.1ms~80ms	
	BYTE				byWhiteBalance ;		// ��ƽ��, 0-"Disable", 1-"Auto", 2-"Custom", 3-"Sunny", 4-"Cloudy", 5-"Home", 6-"Office", 7-"Night", 8-"HighColorTemperature", 9-"LowColorTemperature", 10-"AutoColorTemperature", 11-"CustomColorTemperature"
	BYTE				byGain;					// 0~100, GainAutoΪtrueʱ��ʾ�Զ���������ޣ������ʾ�̶�������ֵ
	bool				bGainAuto;				// �Զ�����
	bool				bIrisAuto;				// �Զ���Ȧ
	float               fExternalSyncPhase;     // ��ͬ������λ���� 0~360
	BYTE				byGainMin;				// ��������
	BYTE				byGainMax;				// ��������
	BYTE				byBacklight;			// ���ⲹ����ȡֵ��Χȡ�����豸��������0-�ر�1-����2-ָ�����򱳹ⲹ��
	BYTE				byAntiFlicker;			// ����˸ģʽ 0-Outdoor 1-50Hz����˸ 2-60Hz����˸
	BYTE				byDayNightColor;		// ��/ҹģʽ��0-���ǲ�ɫ��1-���������Զ��л���2-���Ǻڰ�
	BYTE				byExposureMode;         // �ع�ģʽ���� �ع�ȼ�Ϊ�Զ��ع�ʱ��Ч��ȡֵ��0-Ĭ���Զ���1-�������ȣ�2-��������
	BYTE				byRotate90;				// 0-����ת��1-˳ʱ��90�㣬2-��ʱ��90��
	bool				bMirror;				// ����
	BYTE                byWideDynamicRange;     // ��ֵ̬ 0-�رգ�1~100-Ϊ��ʵ��Χֵ
	BYTE                byGlareInhibition;		// ǿ������ 0-�رգ� 1~100Ϊ��Χֵ
	CFG_RECT			stuBacklightRegion;     // ���ⲹ������
	BYTE                byFocusMode;            // 0-�رգ�1-�����۽���2-�Զ��۽�
	bool				bFlip;					// ��ת
	BYTE				reserved[74];			// ����
} CFG_VIDEO_IN_NIGHT_OPTIONS;

typedef struct tagCFG_VIDEO_IN_NORMAL_OPTIONS
{
	BYTE				byGainRed;				// ��ɫ������ڣ���ƽ��Ϊ"Custom"ģʽ����Ч 0~100
	BYTE				byGainBlue;				// ��ɫ������ڣ���ƽ��Ϊ"Custom"ģʽ����Ч 0~100
	BYTE				byGainGreen;			// ��ɫ������ڣ���ƽ��Ϊ"Custom"ģʽ����Ч 0~100
	BYTE				byExposure;				// �ع�ģʽ��ȡֵ��Χȡ�����豸��������0-�Զ��ع⣬1-�ع�ȼ�1��2-�ع�ȼ�2��n-1����ع�ȼ��� n��ʱ�������޵��Զ��ع� n+1�Զ���ʱ���ֶ��ع� (n==byExposureEn��
	float				fExposureValue1;		// �Զ��ع�ʱ�����޻����ֶ��ع��Զ���ʱ��,����Ϊ��λ��ȡֵ0.1ms~80ms
	float				fExposureValue2;		// �Զ��ع�ʱ������,����Ϊ��λ��ȡֵ0.1ms~80ms	
	BYTE				byWhiteBalance ;		// ��ƽ��, 0-"Disable", 1-"Auto", 2-"Custom", 3-"Sunny", 4-"Cloudy", 5-"Home", 6-"Office", 7-"Night", 8-"HighColorTemperature", 9-"LowColorTemperature", 10-"AutoColorTemperature", 11-"CustomColorTemperature"
	BYTE				byGain;					// 0~100, GainAutoΪtrueʱ��ʾ�Զ���������ޣ������ʾ�̶�������ֵ
	bool				bGainAuto;				// �Զ�����
	bool				bIrisAuto;				// �Զ���Ȧ
	float               fExternalSyncPhase;     // ��ͬ������λ���� 0~360
	BYTE				byGainMin;				// ��������
	BYTE				byGainMax;				// ��������
	BYTE				byBacklight;			// ���ⲹ����ȡֵ��Χȡ�����豸��������0-�ر�1-����2-ָ�����򱳹ⲹ��
	BYTE				byAntiFlicker;			// ����˸ģʽ 0-Outdoor 1-50Hz����˸ 2-60Hz����˸
	BYTE				byDayNightColor;		// ��/ҹģʽ��0-���ǲ�ɫ��1-���������Զ��л���2-���Ǻڰ�
	BYTE				byExposureMode;         // �ع�ģʽ���� �ع�ȼ�Ϊ�Զ��ع�ʱ��Ч��ȡֵ��0-Ĭ���Զ���1-�������ȣ�2-��������
	BYTE				byRotate90;				// 0-����ת��1-˳ʱ��90�㣬2-��ʱ��90��
	bool				bMirror;				// ����
	BYTE                byWideDynamicRange;     // ��ֵ̬ 0-�رգ�1~100-Ϊ��ʵ��Χֵ
	BYTE                byGlareInhibition;		// ǿ������ 0-�رգ� 1~100Ϊ��Χֵ
	CFG_RECT			stuBacklightRegion;     // ���ⲹ������
	BYTE                byFocusMode;            // 0-�رգ�1-�����۽���2-�Զ��۽�
	bool				bFlip;					// ��ת
	BYTE				reserved[74];			// ����
}CFG_VIDEO_IN_NORMAL_OPTIONS;



// ���������
typedef struct tagCFG_FLASH_CONTROL
{
	BYTE				byMode;					// ����ģʽ��0-��ֹ���⣬1-ʼ�����⣬2-�Զ�����
	BYTE				byValue;				// ����ֵ, 0-0us, 1-64us, 2-128us, 3-192...15-960us
	BYTE				byPole;					// ����ģʽ, 0-�͵�ƽ 1-�ߵ�ƽ 2-������ 3-�½���
	BYTE                byPreValue;				// ����Ԥ��ֵ  ����0~100
	BYTE				byDutyCycle;			// ռ�ձ�, 0~100
	BYTE				byFreqMultiple;			// ��Ƶ, 0~10
	BYTE				reserved[122];			// ����
}CFG_FLASH_CONTROL;

// ץ�Ĳ�����������
typedef struct tagCFG_VIDEO_IN_SNAPSHOT_OPTIONS
{
	BYTE				byGainRed;				// ��ɫ������ڣ���ƽ��Ϊ"Custom"ģʽ����Ч 0~100
	BYTE				byGainBlue;				// ��ɫ������ڣ���ƽ��Ϊ"Custom"ģʽ����Ч 0~100
	BYTE				byGainGreen;			// ��ɫ������ڣ���ƽ��Ϊ"Custom"ģʽ����Ч 0~100
	BYTE				byExposure;				// �ع�ģʽ��ȡֵ��Χȡ�����豸��������0-�Զ��ع⣬1-�ع�ȼ�1��2-�ع�ȼ�2��n-1����ع�ȼ��� n��ʱ�������޵��Զ��ع� n+1�Զ���ʱ���ֶ��ع� (n==byExposureEn��
	float				fExposureValue1;		// �Զ��ع�ʱ�����޻����ֶ��ع��Զ���ʱ��,����Ϊ��λ��ȡֵ0.1ms~80ms
	float				fExposureValue2;		// �Զ��ع�ʱ������,����Ϊ��λ��ȡֵ0.1ms~80ms	
	BYTE				byWhiteBalance;			// ��ƽ��, 0-"Disable", 1-"Auto", 2-"Custom", 3-"Sunny", 4-"Cloudy", 5-"Home", 6-"Office", 7-"Night", 8-"HighColorTemperature", 9-"LowColorTemperature", 10-"AutoColorTemperature", 11-"CustomColorTemperature"
	BYTE				byColorTemperature;		// ɫ�µȼ�, ��ƽ��Ϊ"CustomColorTemperature"ģʽ����Ч
	bool				bGainAuto;				// �Զ�����
	BYTE				byGain;					// �������, GainAutoΪtrueʱ��ʾ�Զ���������ޣ������ʾ�̶�������ֵ
	BYTE				reversed[112];			// ����
} CFG_VIDEO_IN_SNAPSHOT_OPTIONS;

// ���۽���ģʽ
typedef enum
{
    CFG_CALIBRATE_MODE_UNKOWN,                  // δ֪ģʽ 
    CFG_CALIBRATE_MODE_ORIGIAL,                 // ԭʼͼ��ģʽ
    CFG_CALIBRATE_MODE_CONFIG,                  // ����ģʽ
    CFG_CALIBRATE_MODE_PANORAMA,                // ȫ��ģʽ
    CFG_CALIBRATE_MODE_DOUBLEPANORAMA,          // ˫ȫ��ģʽ
    CFG_CALIBRATE_MODE_ORIGIALPLUSTHREEEPTZREGION, // 1+3ģʽ��һ��ԭʼ����ͼ���3��EPtzͼ��
    CFG_CALIBRATE_MODE_SINGLE,                  // ������EPtzģʽ��ֻ��һ��EPtz���棩
    CFG_CALIBRATE_MODE_FOUREPTZREGION,          // 4����ģʽ��4��EPtz���ƻ��棩
    CFG_CALIBRATE_MODE_NORMAL,                  // ��ͨģʽ
}CFG_CALIBRATE_MODE;

// ���۾�ͷ����
typedef struct tagCFG_FISH_EYE
{
    CFG_POLYGON         stuCenterPoint;         // ����Բ������,��Χ[0,8192]
	unsigned int        nRadius;                // ���۰뾶��С,��Χ[0,8192]
	float               fDirection;             // ��ͷ��ת����,��ת�Ƕ�[0,360.0]
	BYTE                byPlaceHolder;          // ��ͷ��װ��ʽ	1��װ��2��װ��3��װ,Ĭ��1
    BYTE                byCalibrateMode;        // ���۽���ģʽ,���CFG_CALIBRATE_MODEö��ֵ
	BYTE				reversed[31];			// ����
}CFG_FISH_EYE;
// ��Ƶ����ǰ��ѡ��
typedef struct tagCFG_VIDEO_IN_OPTIONS
{
	BYTE				byBacklight;			// ���ⲹ����ȡֵ��Χȡ�����豸��������0-�ر�1-����2-ָ�����򱳹ⲹ��
	BYTE				byDayNightColor;		// ��/ҹģʽ��0-���ǲ�ɫ��1-���������Զ��л���2-���Ǻڰ�
	BYTE				byWhiteBalance;			// ��ƽ��, 0-"Disable", 1-"Auto", 2-"Custom", 3-"Sunny", 4-"Cloudy", 5-"Home", 6-"Office", 7-"Night", 8-"HighColorTemperature", 9-"LowColorTemperature", 10-"AutoColorTemperature", 11-"CustomColorTemperature"
	BYTE				byColorTemperature;		// ɫ�µȼ�, ��ƽ��Ϊ"CustomColorTemperature"ģʽ����Ч
	bool				bMirror;				// ����
	bool				bFlip;					// ��ת
	bool				bIrisAuto;				// �Զ���Ȧ
	bool				bInfraRed;				// ���ݻ������Զ��������ⲹ����	
	BYTE				byGainRed;				// ��ɫ������ڣ���ƽ��Ϊ"Custom"ģʽ����Ч 0~100
	BYTE				byGainBlue;				// ��ɫ������ڣ���ƽ��Ϊ"Custom"ģʽ����Ч 0~100
	BYTE				byGainGreen;			// ��ɫ������ڣ���ƽ��Ϊ"Custom"ģʽ����Ч 0~100
	BYTE				byExposure;				// �ع�ģʽ��ȡֵ��Χȡ�����豸��������0-�Զ��ع⣬1-�ع�ȼ�1��2-�ع�ȼ�2��n-1����ع�ȼ��� n��ʱ�������޵��Զ��ع� n+1�Զ���ʱ���ֶ��ع� (n==byExposureEn��
	float				fExposureValue1;		// �Զ��ع�ʱ�����޻����ֶ��ع��Զ���ʱ��,����Ϊ��λ��ȡֵ0.1ms~80ms
	float				fExposureValue2;		// �Զ��ع�ʱ������,����Ϊ��λ��ȡֵ0.1ms~80ms	
	bool				bGainAuto;				// �Զ�����
	BYTE				byGain;					// �������, GainAutoΪtrueʱ��ʾ�Զ���������ޣ������ʾ�̶�������ֵ
	BYTE				bySignalFormat;			// �źŸ�ʽ, 0-Inside(�ڲ�����) 1-BT656 2-720p 3-1080p  4-1080i  5-1080sF
	BYTE				byRotate90;				// 0-����ת��1-˳ʱ��90�㣬2-��ʱ��90��	
	float               fExternalSyncPhase;		// ��ͬ������λ���� 0~360	
	BYTE                byExternalSync;			// �ⲿͬ���ź�����,0-�ڲ�ͬ�� 1-�ⲿͬ��
	BYTE				bySwitchMode;			//0-���л�������ʹ�ð������ã�1-���������л���2-����ʱ���л���3-���л�������ʹ��ҹ�����ã�4-ʹ����ͨ����
	BYTE				byDoubleExposure;		// ˫����, 0-�����ã�1-˫����ȫ֡�ʣ���ͼ�����Ƶֻ�п��Ų�����ͬ��2-˫���Ű�֡�ʣ���ͼ�����Ƶ���ż���ƽ���������ͬ
	BYTE                byWideDynamicRange;     // ��ֵ̬
	CFG_VIDEO_IN_NIGHT_OPTIONS stuNightOptions;	// ҹ�����
	CFG_FLASH_CONTROL	stuFlash;				// ���������
	CFG_VIDEO_IN_SNAPSHOT_OPTIONS stuSnapshot;	// ץ�Ĳ���, ˫����ʱ��Ч
	CFG_FISH_EYE        stuFishEye;             // ���۾�ͷ
	BYTE                byFocusMode;            // 0-�رգ�1-�����۽���2-�Զ��۽�
	BYTE                reserved[28];           // ����
	BYTE				byGainMin;				// ��������
	BYTE				byGainMax;				// ��������
	BYTE				byAntiFlicker;			// ����˸ģʽ 0-Outdoor 1-50Hz����˸ 2-60Hz����˸
	BYTE				byExposureMode;         // �ع�ģʽ���� �ع�ȼ�Ϊ�Զ��ع�ʱ��Ч��ȡֵ��0-Ĭ���Զ���1-�������ȣ�2-��������,4-�ֶ�
	BYTE                byGlareInhibition;		// ǿ������ 0-�رգ� 1~100Ϊ��Χֵ
	CFG_RECT			stuBacklightRegion;     // ���ⲹ������            
	CFG_VIDEO_IN_NORMAL_OPTIONS stuNormalOptions;//��ͨ����
} CFG_VIDEO_IN_OPTIONS;

// RTSP�������������������ýṹ��
typedef struct tagCFG_MULTICAST_INFO
{
	int  nStructSize;
	bool abStreamType;
	BYTE byReserved[3];
	BOOL bEnable;      // �Ƿ�ʹ��
	char szMulticastAddr[MAX_ADDRESS_LEN]; //�鲥��ַ
	int  nPort;        //�鲥�˿�
	char szLocalAddr[MAX_ADDRESS_LEN];   //������ַ�������鲥ָ����ϸ����
	int  nChannelID;   // ͨ����
	int  nStreamType;  // �������� 0-������, 1-������1,2-������2,3-������3
}CFG_MULTICAST_INFO;

typedef struct tagCFG_MULTICASTS_INFO
{
	int nStructSize;
	CFG_MULTICAST_INFO stuMultiInfo[MAX_CHAN_NUM]; //����鲥����
	int nCount;                                    //��Ч�������
}CFG_MULTICASTS_INFO;

typedef struct tagCFG_RTSP_INFO_IN
{
	int         nStructSize;
	BOOL        bEnable;       // ���������Ƿ�ʹ��
	int         nPort;         // RTSP����˿�
	int         nRtpStartPort; // RTP��ʼ�˿�
	int         nRtpEndPort;   // RTP�����˿�
	BOOL        bHttpEnable;   // RtspOverHttpʹ��
	int         nHttpPort;     // RtspOverHttp�˿�
}CFG_RTSP_INFO_IN;

typedef struct tagCFG_RTSP_INFO_OUT
{
	int         nStructSize;
	BOOL        bEnable;       // ���������Ƿ�ʹ��
	int         nPort;         // RTSP����˿�
	int         nRtpStartPort; // RTP��ʼ�˿�
	int         nRtpEndPort;   // RTP�����˿�
	BOOL        bHttpEnable;   // RtspOverHttpʹ��
	int         nHttpPort;     // RtspOverHttp�˿�
}CFG_RTSP_INFO_OUT;

typedef struct tagCFG_MULTICASTS_INFO_IN
{
	int                  nStructSize;
	CFG_MULTICAST_INFO   *pTSMulticast;   //TS���鲥����
	int                  nTSCount;        //��ЧTS�������
	CFG_MULTICAST_INFO   *pRTPMulticast;  //RTP���鲥����
	int                  nRTPCount;       //��ЧRTP�������
	CFG_MULTICAST_INFO   *pDHIIMulticast; //DHII���鲥����
	int                  nDHIICount;      //��ЧDHII�������
    CFG_MULTICAST_INFO   *pRTPAudio;      //RTP��Ƶ�鲥����
    int                  nRTPAudioCount;  //��ЧRTP��Ƶ�������
}CFG_MULTICASTS_INFO_IN;

typedef struct tagCFG_MULTICASTS_INFO_OUT
{
	int                  nStructSize;
	CFG_MULTICASTS_INFO  stuTSMulticast;   //TS���鲥����
	CFG_MULTICASTS_INFO  stuRTPMulticast;  //RTP���鲥����
	CFG_MULTICASTS_INFO  stuDHIIMulticast; //DHII���鲥����
    CFG_MULTICASTS_INFO  stuRTPAudioMulticast; //RTP��Ƶ�鲥����
}CFG_MULTICASTS_INFO_OUT;

typedef struct tagCFG_ACTIVEUSER_INFO
{
	int nStructSize;
	int nUserID;                         //��û�ID��һ���ǻỰID
	char szUser[MAX_USERNAME_LEN];       // �û���
	char szGroupName[MAX_USERNAME_LEN];  // �û���������
	int  nGroupLevel;                    // �û�������ȼ�
	char szClientType[MAX_USERNAME_LEN]; // �ͻ�������
	char szClientAddr[MAX_ADDRESS_LEN];  // �ͻ���IP��ַ
	CFG_NET_TIME  stuLoginTime;              // �û�����ʱ��
}CFG_ACTIVEUSER_INFO;

typedef struct tagCFG_ACTIVEALLUSER_INFO
{
	int nStructSize;
	int nCount;                                                //��Ч�������
	CFG_ACTIVEUSER_INFO stuActiveUserInfo[MAX_ACTIVEUSER_NUM]; //  ����û��б�
}CFG_ACTIVEALLUSER_INFO;

typedef struct tagCFG_NET_TIME_EX
{
	DWORD				dwYear;					// ��
	DWORD				dwMonth;				// ��
	DWORD				dwDay;					// ��
	DWORD				dwHour;					// ʱ
	DWORD				dwMinute;				// ��
	DWORD				dwSecond;				// ��
	DWORD               dwMillisecond;          // ����
	DWORD               dwReserved[2];          // �����ֶ�
} CFG_NET_TIME_EX;

#define MAX_EXITMAN_NUM	32		// ֧�ֵ������뿪����

// ��������
typedef enum tagEM_CFG_RULE_TYPE
{
	EM_CFG_RULE_UNKNOWN,			// δ֪
	EM_CFG_RULE_NUMBER_STAT,		// ����ͳ��
	EM_CFG_RULE_MAN_NUM_DETECTION,	// ����������ͳ��
} EM_CFG_RULE_TYPE;

// �뿪��Ա������ʱ����Ϣ
typedef struct tagCFG_EXITMAN_STAY_STAT
{
	CFG_NET_TIME_EX	stuEnterTime;		// ��Ա��������ʱ��
	CFG_NET_TIME_EX	stuExitTime;		// ��Ա�뿪����ʱ��
	BYTE			reserved[104];		// �����ֽ�
} CFG_EXITMAN_STAY_STAT;

//��ȡ��Ƶͳ��ժҪ��Ϣ�ṹ��
typedef struct tagCFG_CFG_VIDEOSATA_SUMMARY_INFO
{
	int         			nStructSize;
	int         			nChannelID;               //ͳ��ͨ����
	char        			szRuleName[MAX_NAME_LEN]; //��������
	CFG_NET_TIME_EX 		stuStatTime;          	  //ͳ��ʱ�䣬ת����UTC
	int         			nEnteredTotal;            //�����ܼ�
	int         			nEnteredToday;            //��������ܼ�
	int         			nEnteredMonth;            //���½����ܼ�
	int         			nEnteredYear;             //��������ܼ�
	int         			nEnteredDaily;            //ÿ���������ܼ�
	int         			nExitedTotal;             //��ȥ�ܼ�
	int         			nExitedToday;             //�����ȥ�ܼ�
	int         			nExitedMonth;             //���³�ȥ�ܼ�
	int         			nExitedYear;              //�����ȥ�ܼ�
	int         			nExitedDaily;             //ÿ������ȥ�ܼ�
	int         			nAvgTotal;                //ƽ�����б���ͳ��(��ȥ��ֵ)
	int         			nAvgToday;                //ƽ�����챣��ͳ��
	int         			nAvgMonth;                //ƽ�����±���ͳ��
	int         			nAvgYear;                 //ƽ�����걣��ͳ��         
	int         			nMaxTotal;                //������б���ͳ��(��ȥ��ֵ)
	int         			nMaxToday;                //�����챣��ͳ��
	int         			nMaxMonth;                //����±���ͳ��
	int         			nMaxYear;                 //�����걣��ͳ��
    int         			nInsideSubTotal;          //����������
	EM_CFG_RULE_TYPE 		emRuleType;		  		  //��������
	int						nRetExitManNum;			  //�뿪��Ա������
	CFG_EXITMAN_STAY_STAT 	stuExitManStayInfo[MAX_EXITMAN_NUM];   //�뿪��Ա������ʱ����Ϣ
}CFG_VIDEOSATA_SUMMARY_INFO;

// ���������ٽṹ��
typedef struct tagCFG_SINGLESCENE
{
	int                 nStructSize;
	int					nSingleAlarmDelay;					//������ʱ:1~600��
	int					nSinglePresetID;					//Ԥ�õ��ţ�1~255(-1�����޶���)
}CFG_SINGLESCENE;
// Ѳ��·���ṹ��
typedef struct tagCFG_TOURPATH
{
	int                 nStructSize;
	int					nMultiPresetID;						//����Ԥ�õ���
	int					nMultiDuration;						//ͣ��ʱ��30~900��
}CFG_TOURPATH;

//�ೡ����������ģʽ
typedef enum tagCFG_MULTSCENE_PRIOR_MODE
{
    EM_MULTSCENE_PRIOR_MODE_UNKNOW = 0  , 
    EM_MULTSCENE_PRIOR_MODE_DETECT      , //�������,�ȴ�ϵͳ�����Ϻ����л�����
    EM_MULTSCENE_PRIOR_MODE_SWITCH      , //�л�����,ֱ��ͣ��ϵͳ���,�л�����һ������
}CFG_MULTSCENE_PRIOR_MODE;

// �ೡ�����ٽṹ��
typedef struct tagCFG_MULTISCENE
{
	int                         nStructSize;
	int					        nMultiAlarmDelay;					    //������ʱ��1~600��
	int					        nTourPathCount;						    //Ѳ��·������
	CFG_TOURPATH	            stuMultiTourPath[MAX_TRACKSCENE_NUM];   //Ѳ��·��
    CFG_MULTSCENE_PRIOR_MODE    emPriorMode;                            //����ģʽ
}CFG_MULTISCENE;

// ȫ�������ٽṹ��
typedef struct tagCFG_FULLSCENE
{
	int                 nStructSize;
	int					nFullAlarmDelay;					//������ʱ��1~600��
	int					nFullPresetID;						//Ԥ�õ���,ȫ������Ԥ�õ�̶�Ϊ0���û���������
	int					nFullDuration;						//ȫ�����ٳ���ʱ��,0��: һֱ����;1~300:���ٳ���ʱ��
}CFG_FULLSCENE;
// ��λ�����ṹ��
typedef struct tag_CFG_POSITION_LIMIT
{
	int                 nStructSize;
	BOOL				bEnable;							//������λ����
	int					nType;								//��λֵ������ʽ��0: ����̨����λ���ò���ʱ�������ݣ���λֵ����������
	                                                        //1����λֵ�ýǶ�(��λ0.1��)��ʾ���ø�ֵ���øó�������λ
	int					nTop;								//����λֵ, 0~900:����λ0.1�ȣ�
	int					nBottom;							//����λֵ��0~900:����λ0.1�ȣ�
	int					nLeft;								//����λֵ��0~3600:����λ0.1�ȣ�
	int					nRight;								//����λֵ��0~3600:����λ0.1�ȣ�
}CFG_POSITION_LIMIT;

//�������õı�Ƿ���
typedef enum tagCFG_SCENE_DIRECTION_INFO
{
    EM_SCENE_DIRECTION_UNKNOW =0    , 
    EM_SCENE_DIRECTION_APPROACH     ,   //����
    EM_SCENE_DIRECTION_LEAVE        ,   //����
    EM_SCENE_DIRECTION_BOTH         ,   //˫��
    EM_SCENE_DIRECTION_EAST2WEST    ,   //�ɶ�����
    EM_SCENE_DIRECTION_WEST2EAST    ,   //������
    EM_SCENE_DIRECTION_SOUTH2NORTH  ,   //������
    EM_SCENE_DIRECTION_NORTH2SOUTH  ,   //�ɱ�����
    EM_SCENE_DIRECTION_OTHER        ,   //����
}CFG_SCENE_DIRECTION_INFO;

// ������Ϣ�ṹ��
typedef struct tagCFG_SCENE
{
	int                         nStructSize;
	int					        nScenePresetID;						//����Ԥ�õ㣬1~255��ͨ��Ԥ�õ��Ź�������Ӧ������
	char			            szSceneName[MAX_NAME_LEN];		    //������
	int					        nSceneDuration;						//���ٳ���ʱ��:0��: һֱ����;1~300:���ٳ���ʱ��
	double		                dSceneMaxZoom;					    //��һ����0~1;Ϊnull��ʾʹ��ȫ��MaxZoomֵ
	CFG_POSITION_LIMIT	        stuPositionLimit;                   //��λ����
    int                         nSceneRegression;                   //��λʱ�䣬1~10��:���ٶ�ʧ��ص�ԭ������ʱ��
    CFG_SCENE_DIRECTION_INFO    emDirectionInfo;                    ///�������õı�Ƿ���
}CFG_SCENE;

// ���ܸ���ģʽ
typedef enum tagCFG_INTELLI_TRACE_MODE
{
    EM_INTELLI_TRACE_MODE_UNKNOW = 0 ,
    EM_INTELLI_TRACE_MODE_INDOOR     , //���ڸ���(��ʱ��)
    EM_INTELLI_TRACE_MODE_OUTDOOR    , //�������
    EM_INTELLI_TRACE_MODE_CLASSROOM  , //���Ҹ���(��ʱ������Ҫ����궨)
}CFG_INTELLI_TRACE_MODE;

// ���ܸ��ٳ������ýṹ��
typedef struct tagCFG_INTELLITRACKSCENE_INFO
{
	int                     nStructSize;
	BOOL		            bEnableTrack;				       //�������ܸ���
	int				        nTrackType;						   //0,�������٣�1���ֶ�����
	double			        dMaxZoom;						   //�����ٱ��ʣ���һ����0~1��Ϊnull��ʾ������
	char			        szCategory[MAX_NAME_LEN];		   //��ǰ�������ͣ�"SingleScene"�����������٣�"MultiScene"���ೡ�����٣�"FullScene"��ȫ������
	CFG_SINGLESCENE         stuSingleTrack;					   //����������
	CFG_MULTISCENE	        stuMultiTrack;	                   //�ೡ������
	CFG_FULLSCENE	        stuFullTrack;					   //ȫ������
	int				        nSceneCount;					   //֧�ֵĳ�������
	CFG_SCENE		        stuScene[MAX_TRACKSCENE_NUM];	   //�����б�
    double                  dCameraHeight;                     //����ͷ����洹ֱ����,��λ:��
    int                     nIdleDelay;                        //���еȴ�ʱ��,��λ:��(���û�ֹͣ�����豸�󣬵����ܼ�����Ч�Ŀ�����ʱʱ��)
    CFG_INTELLI_TRACE_MODE  emTraceMode;                       //����ģʽ
}CFG_INTELLITRACKSCENE_INFO;

// ������ϸ����

#define   CFG_MAX_FISHEYE_WINDOW_NUM               8         // ������۴�����
#define   CFG_MAX_FISHEYE_MODE_NUM                 8         // �������ģʽ��   
// ���۴���λ����Ϣ
typedef struct tagCFG_FISHEYE_WINDOW_INFO
{
    DWORD             dwWindowID;                        // ����ID
    int               nFocusX;                           // EPtz��������̨���Ľ��������
    int               nFocusY;                           // EPtz (������̨���Ľ���������   
    int               nHorizontalAngle;                  // EPtz��ˮƽ�Ƕ�
    int               nVerticalAngle;                    // EPtz�Ĵ�ֱ�Ƕ�
}CFG_FISHEYE_WINDOW_INFO;

// ���۸�ģʽ�Ĵ���λ����Ϣ
typedef struct tagCFG_FISHEYE_MODE_INFO
{
    int                nModeType;                        // ģʽ���ͣ���� CFG_CALIBRATE_MODE
    int                nWindowNum;                       // ��ǰģʽ�µĴ�����
    CFG_FISHEYE_WINDOW_INFO stuWindwos[CFG_MAX_FISHEYE_WINDOW_NUM]; // ���崰����Ϣ 
}CFG_FISHEYE_MODE_INFO;

typedef struct tagCFG_FISHEYE_DETAIL_INFO
{
    int                nModeNum;                         // ģ����
    CFG_FISHEYE_MODE_INFO stuModes[CFG_MAX_FISHEYE_MODE_NUM]; // ����ģ����Ϣ  
}CFG_FISHEYE_DETAIL_INFO;

// ƽ̨�·�������Ӧ��ת�������б�
#define   CFG_MAX_NOANSWER_FORWARD_GROUP_NUM                    32         // �����Ӧ��ǰת�б����
#define   CFG_MAX_FORWARD_NUMBERS_NUM                           32         // �����Ӧ��ת�ƺ������

// ��Ӧ��ת������
typedef struct tagCFG_VT_NOANSWER_FORWARD_GROUP
{
    char                    szRoomNumber[CFG_COMMON_STRING_32];          // �����к���
    int                     nForwardNumbersNum;                          // ��Ӧ��ת�ƺ������
    char                    szForwardNumbers[CFG_MAX_FORWARD_NUMBERS_NUM][CFG_COMMON_STRING_64]; // ��Ӧ��ת�ƺ����б�  
}CFG_VT_NOANSWER_FORWARD_GROUP;

// ƽ̨�·�������Ӧ��ת�������б���Ϣ
typedef struct tagCFG_VT_NOANSWER_FORWARD_INFO
{
    BOOL                            bEnable;                                        // ��Ӧ��ת��ʹ��
    int                             nGroupNum;                                      // ��Ӧ��ת�������б���Ч��������Χ��0 ~ CFG_MAX_NOANSWER_FORWARD_GROUP_NUM
    CFG_VT_NOANSWER_FORWARD_GROUP   stuGroups[CFG_MAX_NOANSWER_FORWARD_GROUP_NUM];  // ��Ӧ��ת�������б�  
}CFG_VT_NOANSWER_FORWARD_INFO;

// VTO��������
typedef struct tagCFG_VTO_CALL_INFO
{
    BOOL                            bAreaEnable;                                        // ����������ʹ��
    char                            szAreaNumber[CFG_COMMON_STRING_32];                  // �������
    BOOL                            bMiddleNumberCallEnable;                            // �кź��п���
    char                            szVTHMiddleNumberPrefix[CFG_COMMON_STRING_16];       // VTH�к�ǰ׺���̶�����4λ
    char                            szVTHLongNumberPrefix[CFG_COMMON_STRING_32];         // VTH����ǰ׺���̶�����8λ
    char                            szVTOShortNumber[CFG_COMMON_STRING_16];              // VTO�̺ţ��̶�����4λ
    char                            szVTOMiddleNumberPrefix[CFG_COMMON_STRING_16];       // VTO�к�ǰ׺���̶�����4λ
    char                            szVTOLongNumbrPrefix[CFG_COMMON_STRING_32];          // VTO����ǰ׺���̶�����12λ
    char                            szVTSLongNumber[CFG_COMMON_STRING_32];               // VTS���ţ��̶�����18λ
    char                            szVillaCallVTHNum[CFG_COMMON_STRING_32];             // ���������к���
}CFG_VTO_CALL_INFO;

/************************����״̬����**********************************/

// ץ��ģʽ
typedef enum tagCFG_TRAFFIC_SNAP_MODE
{
	TRAFFIC_SNAP_MODE_AUTO = 0,				       // �Զ�ץ��
	TRAFFIC_SNAP_MODE_COIL,				           // ��Ȧץ��
	TRAFFIC_SNAP_MODE_COIL_PICANALYSIS,		       // ��Ȧץ��, ͼƬ����
	TRAFFIC_SNAP_MODE_STREAM,				       // ��Ƶץ��
	TRAFFIC_SNAP_MODE_STREAM_IDENTIFY,		       // ��Ƶץ��, ����ʶ��
	TRAFFIC_SNAP_MODE_MIX_IDENTIFY,		           // ���ץ��, ����ʶ��
} CFG_TRAFFIC_SNAP_MODE;

// �豸����״̬��Ϣ
typedef struct tagCFG_TRAFFIC_WORKSTATE_INFO
{
	int                     nChannelID;  // ͨ����
	CFG_TRAFFIC_SNAP_MODE   emSnapMode;  // ץ��ģʽ
	int                     nMatchMode;  // ץ��ƥ��ģʽ: 0-��ʵʱƥ�䷽ʽ���ȱ�����ץ�ģ�ץ��֡���Ǳ���֡;  1-ʵʱƥ��ģʽ������֡��ץ��֡��ͬһ֡ 
}CFG_TRAFFIC_WORKSTATE_INFO;

// ��ȡ�豸����״̬�Ƿ�����(CFG_CAP_CMD_DEVICE_STATE ��Ӧ�Ľṹ��)
typedef struct tagCFG_TRAFFIC_DEVICE_STATUS
{
	char                 szType[MAX_PATH];          // �豸����	֧�֣�"Radar","Detector","SigDetector","StroboscopicLamp"," FlashLamp"
	char                 szSerialNo[MAX_PATH];      // �豸���
	char                 szVendor[MAX_PATH];        // ��������
	int                  nWokingState;              // ����״̬	0-����,1-��������
	BYTE				 byLightState;				// RS485�Ƶ�����״̬��Type Ϊ"DhrsStroboscopicLamp"����"DhrsSteadyLamp"ʱ��Ч
													// 0-δ֪, 1-����, 2-����
	BYTE				 byReserved[3];             // Ԥ���ֽ�
				 
}CFG_TRAFFIC_DEVICE_STATUS;

typedef struct tagCFG_CAP_TRAFFIC_DEVICE_STATUS
{
	int                        nStatus;             // stuStatus ʵ�ʸ���
	CFG_TRAFFIC_DEVICE_STATUS  stuStatus[MAX_STATUS_NUM];
}CFG_CAP_TRAFFIC_DEVICE_STATUS;

/************************¼����������**********************************/

// �洢����Ϣ
typedef struct tagCFG_STORAGEGROUP_INFO
{
	char                  szGroupName[MAX_STORAGEGROUPNAME_LEN];   // �洢������
	BYTE*                 byDisks;                                 // ���������Ż�����
	int                   nBufSize;                                // ������byDisks�ĳ���
	int                   nDiskNum;                                // �洢���еĴ�����
	int                   nGroupIndex;                             // �洢�����(1~���Ӳ����)
}CFG_STORAGEGROUP_INFO;

// ¼��-�洢�� ��Ӧ��Ϣ
typedef struct tagCFG_RECORDTOGROUP_INFO
{
	int                   nChannelID;                               // ͨ����
	BOOL                  bEnable;                                  // ʹ��                       
	char                  szGroupName[MAX_STORAGEGROUPNAME_LEN];    // �洢������, ֻ��
	int                   nGroupIndex;                              // �洢�����(1~�������, 0���ʾ�޶�Ӧ����)��ͨ���˲�����CFG_STORAGE_GROUP_INFO����
}CFG_RECORDTOGROUP_INFO;

enum EM_STORAGEPOINT_TYPE
{
       	EM_STORAGE_TIMINGRECORD,        //��ʱ¼��洢������
		EM_STORAGE_MANUALRECORD,        //�ֶ�¼��洢������
		EM_STORAGE_VIDEODETECTRECORD,   //��Ƶ���¼��洢������
		EM_STORAGE_ALARMRECORD,         //����¼��洢������
		EM_STORAGE_CARDRECORD,          //����¼��洢������
		EM_STORAGE_EVENTRECORD,         // �����¼�¼��洢������                
		EM_STORAGE_TIMINGSNAPSHOT,      // ��ʱץͼ�洢������
		EM_STORAGE_MANUALSNAPSHOT,      // �ֶ�ץͼ�洢������
		EM_STORAGE_VIDEODETECTSNAPSHOT, // ��Ƶ���ץͼ�洢������
		EM_STORAGE_ALARMSNAPSHOT,       // ����ץͼ�洢������
		EM_STORAGE_CARDSNAPSHOT,        // ����ץͼ�洢������
		EM_STORAGE_EVENTSNAPSHOT,       // �����¼�ץͼ�洢������
		EM_STORAGE_TIMINGEXTRA1RECORD,  // ������1¼��洢������
};
// �洢��������Ϣ
typedef struct tagCFG_STORAGEPOINT_INFO
{
	DWORD dwSize;
	EM_STORAGEPOINT_TYPE emStoragePointType;// �洢������
	char nLocalDir;                         //���ع���Ŀ¼������, �ձ�ʾ��¼������
	char szCompressDir[MAX_DIRECTORY_LEN];  //ʵʱѹ���洢Ŀ¼�飬�ձ�ʾ��ʹ��ʵʱѹ���洢��
	char szRedundantDir[MAX_DIRECTORY_LEN]; //���๤��Ŀ¼������,�ձ�ʾû������¼��
	char szRemoteDir[MAX_DIRECTORY_LEN];   //Զ�̹���Ŀ¼������,�ձ�ʾ��¼��Զ�̡�
	BOOL bAutoSync;                         //Զ�̴洢������ϻָ����Ƿ��Զ������ش洢������ͬ����Զ�̴洢��
	UINT nAutoSyncRange;                    //������ָ���ʱ�̿�ʼ����Ҫ��ǰͬ��������ʱ�䷶Χ��СʱΪ��λ��0��ʾͬ���������ݡ�
	BOOL bLocalEmergency;                   //Զ��Ŀ¼�޷�����ʱ���Ƿ񱣴浽����Ŀ¼��
	UINT nCompressBefore;                   //���ý�������֮ǰ��¼���ļ�����ѹ����
}CFG_STORAGEPOINT_INFO;

// ¼��洢��ӳ��������Ϣ
typedef struct tagCFG_RECORDTOSTORAGEPOINT_INFO
{
	int	nStoragePointNum;                                        //�洢����Ŀ
	CFG_STORAGEPOINT_INFO stStoragePoints[MAX_STORAGEPOINT_NUM]; // �洢��������Ϣ
}CFG_RECORDTOSTORAGEPOINT_INFO;

// ¼��洢��ӳ��������չ��Ϣ
typedef struct tagCFG_RECORDTOSTORAGEPOINT_EX_INFO
{
	DWORD							dwSize;
	int								nMaxChannelRecord; // ���¼��ͨ�����ø���
	int								nRetChannelRecord; // ʵ�ʷ���ͨ�����ø���
	CFG_RECORDTOSTORAGEPOINT_INFO	*pstRecordStorage; // ͨ���洢��������Ϣ, ���û������ڴ�, ��СΪsizeof(CFG_RECORDTOSTORAGEPOINT_INFO)*nMaxChannelRecord
}CFG_RECORDTOSTORAGEPOINT_EX_INFO;

//iscsiֱ�棬Ԫ���ݷ��������ýӿ�
typedef struct __tagCFG_METADATA_SERVER
{
	DWORD       nStructSize;
	BOOL        bEnable;                              //ʹ�ܿ���
	char        szAddress[MAX_ADDRESS_LEN];           //IP��ַ��������
	DWORD       dwPort;                               //�˿ں�
	char        szUserName[MAX_USERNAME_LEN];         //�ʻ���  
	char        szPassword[MAX_PASSWORD_LEN];         //����
	char        szDeviceID[MAX_REMOTEDEVICENAME_LEN]; //Ϊʹ��Ԫ���ݷ��������豸ͳһ�����ID���豸�Դ�ID��ʶ�Լ�
}CFG_METADATA_SERVER;

// ���ػ�������
typedef struct __tagCFG_GOOD_WEIGHT_INFO
{
	DWORD		dwMaxGoodsWeight;					// ������(kg)
	DWORD		dwMinGoodsWeight;					// ��С����(kg)
	DWORD		dwSelfWeight;						// ����(kg)
	DWORD		dwStandardWeight;					// ����(kg)
	DWORD		dwAlarmWeight;						// ������ֵ(kg)
	int			nCheckTime;							// ��ֹ�ɼ�ʱ��(s)
} CFG_GOOD_WEIGHT_INFO;

//��ͷ�۽�״̬��Ϣ
typedef struct tagCFG_CAP_FOCUS_STATUS
{
	int					nAutofocusPeak;							// ��ǰAF��ֵ	�����۽�ģʽ����Ч
	double		        dFocus;									// �۽�λ��	��һ����0~1
	double		        dZoom;									// �Ŵ���	��һ����0~1
	int					nStatus;								// �۽�״̬, 0 ����״̬ 1 �����Զ��۽�
}CFG_CAP_FOCUS_STATUS;

//��̨֧��������Ϣ
typedef struct tagCFG_CAP_PTZ_ENABLEINFO 
{
	BOOL bEnable;												//��ͨ���Ƿ�֧����̨
	
}CFG_CAP_PTZ_ENABLEINFO;


// ����Ӧ��������
typedef struct tagCFG_CAP_NETAPP
{
	int                nNetCardCount;                           // ��������
	int                nNetTypeNumber;                          // �������͸���
	char               szNetType[MAX_NET_TYPE_NUM][MAX_NET_TYPE_LEN]; // ������������, "PPPoE"��"WIFI"��"3G"��"LAN"
}CFG_CAP_NETAPP;



///////////////////////////////////����Э������///////////////////////////////////////
// ¼��ģʽ
struct AV_CFG_RecordMode
{
	AV_int32			nStructSize;
	AV_int32			nMode;							// ¼��ģʽ, 0-�Զ�¼��1-�ֶ�¼��2-�ر�¼��
    AV_int32			nModeExtra1;					// ������1¼��ģʽ, 0-�Զ�¼��1-�ֶ�¼��2-�ر�¼��
    AV_int32			nModeExtra2;					// ������2¼��ģʽ, 0-�Զ�¼��1-�ֶ�¼��2-�ر�¼��
};

// ͨ������
struct AV_CFG_ChannelName
{
	AV_int32			nStructSize;
	AV_int32			nSerial;						// ����ͷΨһ���
	char				szName[CFG_MAX_CHANNEL_NAME_LEN];// ͨ����
};

// ��Ƶ�������
struct AV_CFG_VideoOutAttr
{
	AV_int32			nStructSize;
	AV_int32			nMarginLeft;					// ��߾�, ����, 0~100
	AV_int32			nMarginTop;						// �ϱ߾�, ����, 0~100
	AV_int32			nMarginRight;					// �ұ߾�, ����, 0~100
	AV_int32			nMarginBottom;					// �±߾�, ����, 0~100
	AV_int32			nBrightness;					// ����, 0~100
	AV_int32			nContrast;						// �Աȶ�, 0~100
	AV_int32			nSaturation;					// ���Ͷ�, 0~100
	AV_int32			nHue;							// ɫ��, 0~100
	AV_int32			nWidth;							// ˮƽ�ֱ���
	AV_int32			nHeight;						// ��ֱ�ֱ���
	AV_int32			nBPP;							// ��ɫ���
	AV_int32			nFormat;						// -1-δ֪, 0-Auto, 1-TV, 2-VGA, 3-DVI, 4-HDMI, 5-SPOT
	AV_int32			nRefreshRate;					// ˢ��Ƶ��
	AV_BOOL				bIQIMode;						// ���ͼ����ǿ
	AV_int32			nScanFormat;					// ɨ��ģʽ, 0-����, 1-����
};

// ʱ���
struct AV_CFG_TimeSection
{
	AV_int32			nStructSize;
	AV_int32			nMask;							// ����
	AV_int32			nBeginHour;						// ��ʼʱ��
	AV_int32			nBeginMinute;
	AV_int32			nBeginSecond;
	AV_int32			nEndHour;						// ����ʱ��
	AV_int32			nEndMinute;
	AV_int32			nEndSecond;
};

// Զ��ͨ��
struct AV_CFG_RemoteChannel 
{
	AV_int32			nStructSize;
	AV_BOOL				bEnable;						// ʹ��
	char				szDeviceID[AV_CFG_Device_ID_Len];	// �豸ID
	AV_int32			nChannel;						// ͨ����
};

// ��ʾԴ
struct AV_CFG_DisplaySource 
{
	AV_int32			nStructSize;
	AV_int32			nWindowID;						// ����ID
	AV_BOOL				bEnable;						// ʹ��
	char				szDeviceID[AV_CFG_Device_ID_Len];// �豸ID
	AV_int32			nVideoChannel;					// ��Ƶͨ����
	AV_int32			nVideoStream;					// ��Ƶ����
	AV_int32			nAudioChannle;					// ��Ƶͨ����
	AV_int32			nAudioStream;					// ��Ƶ����
	AV_BOOL				bVideoEnable;			        // ��ʾ�ô����Ƿ�����ƵԴ
};

// ͨ���ָ���ʾԴ
struct AV_CFG_ChannelDisplaySource 
{
	AV_int32			nStructSize;
	AV_int32			nWindowNum;						// �ָ������
	AV_CFG_DisplaySource stuSource[AV_CFG_Max_Split_Window];// �ָ����ʾԴ
};

// ������ѵ�·ָ�ģʽ�ķ���ʹ��״̬
struct AV_CFG_MonitorTourMask
{
	AV_int32			nStructSize;
	CFG_SPLITMODE		emSplitMode;					// �ָ�ģʽ
	AV_int32			nGroupNum;						// ��������
	AV_int32			nGroup[AV_CFG_Max_Split_Group];	// �÷ָ�ģʽ�¼�����ѵ�ķ���
};

// ������ѵ
struct AV_CFG_MonitorTour 
{
	AV_int32			nStructSize;
	AV_BOOL				bEnable;						// ʹ��
	AV_int32			nInterval;						// ʱ����, 5~120s
	AV_int32			nSplitMaskNum;					// �ָ����ʹ������
	AV_CFG_MonitorTourMask	stuSplitMask[AV_CFG_Max_Split_Mode];// ���ָ�ģʽ��Ӧ�ķ���ʹ��״̬
	AV_int32			nCollectionNum;					// �ղ�����
	char				szCollection[AV_CFG_Monitor_Favorite_In_Channel][AV_CFG_Monitor_Favorite_Name_Len];// �����ղ�����
};

// ���ӻ����ղ�
struct AV_CFG_MonitorFavorite 
{
	AV_int32			nStructSize;
	AV_BOOL				bDir;										// �Ƿ�ΪĿ¼	
	char				szName[AV_CFG_Monitor_Favorite_Name_Len];	// ����
	char				szPath[AV_CFG_Max_Path];					// Ŀ¼·��, ��ʽ����: .name1.name2.name3
	CFG_SPLITMODE		emMode;										// �ָ�ģʽ
	AV_int32			nWindowNum;									// ��������
	AV_CFG_DisplaySource stuWindow[AV_CFG_Max_Monitor_Favorite_Window];	// ������ʾԴ����
};

// ���ӻ����ղؼ���
struct AV_CFG_MonitorCollection 
{
	AV_int32			nStructSize;
	AV_CFG_MonitorFavorite*	pstuFavorite;				// �����ղ�����
	AV_int32			nMaxCount;						// �����ղ������С
	AV_int32			nRetCount;						// ���صĻ����ղ�����
};

// Raid��Ϣ
struct AV_CFG_Raid 
{
	AV_int32			nStructSize;
	char				szName[AV_CFG_Raid_Name_Len];	// ����
	AV_int32			nLevel;							// �ȼ�
	AV_int32			nMemberNum;						// ���̳�Ա����
	char				szMembers[AV_CFG_Max_Rail_Member][AV_CFG_Max_Path];	// ���̳�Ա
};

// ¼��Դ
struct AV_CFG_RecordSource
{
	AV_int32			nStructSize;
	AV_BOOL				bEnable;						// ʹ��
	char				szDeviceID[AV_CFG_Device_ID_Len];// �豸ID
	AV_int32			nVideoChannel;					// ��Ƶͨ����
	AV_int32			nVideoStream;					// ��Ƶ����
	AV_int32			nAudioChannle;					// ��Ƶͨ����
	AV_int32			nAudioStream;					// ��Ƶ����
};

// �����ʽ, ������Ƶ����Ƶ
struct AV_CFG_EncodeFormat
{
	AV_int32			nStructSize;
	AV_BOOL				bAudioEnable;					// ��Ƶʹ��
	AV_int32			nAudioBitRate;					// ��Ƶ������
	CFG_AUDIO_FORMAT	emAudioCompression;				// ��Ƶѹ��ģʽ
	AV_int32			nAudioDepth;					// ��Ƶ�������
	AV_int32			nAudioFrequency;				// ��Ƶ����Ƶ��
	AV_int32			nAudioMode;						// ��Ƶ����ģʽ
	AV_int32			nAudioPack;						// ��Ƶ���ģʽ, 0-DHAV, 1-PS

	AV_BOOL				bVideoEnable;					// ��Ƶʹ��
	AV_int32			nVideoBitRate;					// ��Ƶ������
	CFG_BITRATE_CONTROL	emVideoBitRateControl;			// ��������ģʽ
	CFG_VIDEO_COMPRESSION emVideoCompression;			// ��Ƶѹ��ģʽ
	AV_int32			nVideoFPS;						// ��Ƶ֡��
	AV_int32			nVideoGOP;						// ��ƵI֡���
	AV_int32			nVideoWidth;					// ��Ƶ���
	AV_int32			nVideoHeight;					// ��Ƶ�߶�
	CFG_IMAGE_QUALITY	emVideoQuality;					// ��Ƶͼ������
	AV_int32			nVideoPack;						// ��Ƶ���ģʽ, 0-DHAV, 1-PS	
};

// ��������
struct AV_CFG_Encode 
{
	AV_int32			nStructSize;
	AV_CFG_EncodeFormat stuMainFormat[AV_CFG_Max_Encode_Main_Format];	// ������, ������ͨ����, �������, ��������
	AV_CFG_EncodeFormat	stuExtraFormat[AV_CFG_Max_Encode_Extra_Format];	// ������, ����������1, ������2, ������3
	AV_CFG_EncodeFormat	stuSnapFormat[AV_CFG_Max_Encode_Snap_Format];	// ץͼ, ������ͨץͼ, ����ץͼ, ����ץͼ
};

// videocolor style
typedef enum tagCFG_ENUM_VIDEOCOLOR_STYLE
{
    CFG_ENUM_VIDEOCOLOR_STYLE_UNKNOWN,                // δ֪
    CFG_ENUM_VIDEOCOLOR_STYLE_GENTLE,                 // ���
    CFG_ENUM_VIDEOCOLOR_STYLE_STANDARD,               // ��׼
    CFG_ENUM_VIDEOCOLOR_STYLE_FLAMBOYANT,             // ����
}CFG_ENUM_VIDEOCOLOR_STYLE;

// ���ճ�������
typedef enum tagEM_LIGHT_SCENE
{
    EM_LIGHT_SCENE_UNKNOWN,             // δ֪
    EM_LIGHT_SCENE_DAY,                 // ����
    EM_LIGHT_SCENE_NIGHT,               // ҹ��
    EM_LIGHT_SCENE_NORMAL,              // ��ͨ
    EM_LIGHT_SCENE_FRONT_LIGHT,         // ˳��
    EM_LIGHT_SCENE_BACK_LIGHT,          // һ�����
    EM_LIGHT_SCENE_STRONG_LIGHT,        // ǿ���
    EM_LIGHT_SCENE_LOW_LIGHT,           // ���ն�
    EM_LIGHT_SCENE_CUSTOM,              // �Զ���
}EM_LIGHT_SCENE;


// ��Ƶ������ɫ����, ÿ����Ƶ����ͨ����Ӧ�����ɫ����
struct AV_CFG_VideoColor 
{
	AV_int32			nStructSize;
	AV_CFG_TimeSection	stuTimeSection;					// ʱ���
	AV_int32			nBrightness;					// ����, 0~100
	AV_int32			nContrast;						// �Աȶ�, 0~100
	AV_int32			nSaturation;					// ���Ͷ�, 0~100
	AV_int32			nHue;							// ɫ��, 0~100
	AV_int32			nGamma;							// ����, 0~100
    AV_int32            nChromaSuppress;                // ɫ�����Ƶȼ� 0~100
    CFG_ENUM_VIDEOCOLOR_STYLE emColorStyle;             // ɫ�ʷ��
};

// ͨ����Ƶ������ɫ����
struct AV_CFG_ChannelVideoColor 
{
	AV_int32			nStructSize;
	AV_int32			nColorNum;						// ͨ����ɫ������
	AV_CFG_VideoColor	stuColor[AV_CFG_Max_VideoColor];// ͨ����ɫ����, ÿ��ͨ����Ӧ�����ɫ����
};

// ��ɫ
struct AV_CFG_Color
{
	AV_int32			nStructSize;
	AV_int32			nRed;							// ��
	AV_int32			nGreen;							// ��
	AV_int32			nBlue;							// ��
	AV_int32			nAlpha;							// ͸��
};

// ����
struct AV_CFG_Rect 
{
	AV_int32			nStructSize;
	AV_int32			nLeft;
	AV_int32			nTop;
	AV_int32			nRight;
	AV_int32			nBottom;	
};

// �������-���򸲸�����
struct AV_CFG_VideoWidgetCover 
{
	AV_int32			nStructSize;	
	AV_BOOL				bEncodeBlend;					// ���ӵ�������
	AV_BOOL				bEncodeBlendExtra1;				// ���ӵ�������1
	AV_BOOL				bEncodeBlendExtra2;				// ���ӵ�������2
	AV_BOOL				bEncodeBlendExtra3;				// ���ӵ�������3
	AV_BOOL				bEncodeBlendSnapshot;			// ���ӵ�ץͼ
	AV_CFG_Color		stuFrontColor;					// ǰ��ɫ
	AV_CFG_Color		stuBackColor;					// ����ɫ
	AV_CFG_Rect			stuRect;						// ����, ����ȡֵ0~8191
	AV_BOOL				bPreviewBlend;					// ���ӵ�Ԥ����Ƶ
};

// ͨ�����������Ϣ
typedef enum tagEM_TITLE_TEXT_ALIGN
{
    EM_TEXT_ALIGN_INVALID,                              // ��Ч�Ķ��뷽ʽ
    EM_TEXT_ALIGN_LEFT,                                 // �����
    EM_TEXT_ALIGN_XCENTER,                              // X�����ж���
    EM_TEXT_ALIGN_YCENTER,                              // Y�����ж���
    EM_TEXT_ALIGN_CENTER,                               // ����
    EM_TEXT_ALIGN_RIGHT,                                // �Ҷ���
    EM_TEXT_ALIGN_TOP,                                  // ���ն�������
    EM_TEXT_ALIGN_BOTTOM,                               // ���յײ�����
    EM_TEXT_ALIGN_LEFTTOP,                              // �������ϽǶ���
    EM_TEXT_ALIGN_CHANGELINE,                           // ���ж���
}EM_TITLE_TEXT_ALIGN;

// �������-ͨ������
struct AV_CFG_VideoWidgetChannelTitle
{
	AV_int32			nStructSize;
	AV_BOOL				bEncodeBlend;					// ���ӵ�������
	AV_BOOL				bEncodeBlendExtra1;				// ���ӵ�������1
	AV_BOOL				bEncodeBlendExtra2;				// ���ӵ�������2
	AV_BOOL				bEncodeBlendExtra3;				// ���ӵ�������3
	AV_BOOL				bEncodeBlendSnapshot;			// ���ӵ�ץͼ
	AV_CFG_Color		stuFrontColor;					// ǰ��ɫ
	AV_CFG_Color		stuBackColor;					// ����ɫ
	AV_CFG_Rect			stuRect;						// ����, ����ȡֵ0~8191, ��ʹ��left��topֵ, ��(left,top)Ӧ��(right,bottom)���ó�ͬ���ĵ�
	AV_BOOL				bPreviewBlend;					// ���ӵ�Ԥ����Ƶ
};

// �������-ʱ�����
struct AV_CFG_VideoWidgetTimeTitle
{
	AV_int32			nStructSize;
	AV_BOOL				bEncodeBlend;					// ���ӵ�������
	AV_BOOL				bEncodeBlendExtra1;				// ���ӵ�������1
	AV_BOOL				bEncodeBlendExtra2;				// ���ӵ�������2
	AV_BOOL				bEncodeBlendExtra3;				// ���ӵ�������3
	AV_BOOL				bEncodeBlendSnapshot;			// ���ӵ�ץͼ
	AV_CFG_Color		stuFrontColor;					// ǰ��ɫ
	AV_CFG_Color		stuBackColor;					// ����ɫ
	AV_CFG_Rect			stuRect;						// ����, ����ȡֵ0~8191, ��ʹ��left��topֵ, ��(left,top)Ӧ��(right,bottom)���ó�ͬ���ĵ�
	AV_BOOL				bShowWeek;						// �Ƿ���ʾ����
	AV_BOOL				bPreviewBlend;					// ���ӵ�Ԥ����Ƶ
};
	
// �������-�Զ������
struct AV_CFG_VideoWidgetCustomTitle 
{
	AV_int32			nStructSize;
	AV_BOOL				bEncodeBlend;					// ���ӵ�������
	AV_BOOL				bEncodeBlendExtra1;				// ���ӵ�������1
	AV_BOOL				bEncodeBlendExtra2;				// ���ӵ�������2
	AV_BOOL				bEncodeBlendExtra3;				// ���ӵ�������3
	AV_BOOL				bEncodeBlendSnapshot;			// ���ӵ�ץͼ
	AV_CFG_Color		stuFrontColor;					// ǰ��ɫ
	AV_CFG_Color		stuBackColor;					// ����ɫ
	AV_CFG_Rect			stuRect;						// ����, ����ȡֵ0~8191, ��ʹ��left��topֵ, ��(left,top)Ӧ��(right,bottom)���ó�ͬ���ĵ�
	char				szText[AV_CFG_Custom_Title_Len];// ��������
	AV_BOOL				bPreviewBlend;					// ���ӵ�Ԥ����Ƶ
    char                szType[AV_CFG_Custom_TitleType_Len];// �������� "Rtinfo" ʵʱ��¼��Ϣ "Custom" �Զ�����ӡ���ʪ�ȵ��� "Title" :Ƭͷ��Ϣ "Check"  У����
                                                            // ������Ϣ "Geography"  ATM������Ϣ "ATMCardInfo" �������� "CameraID" 
    EM_TITLE_TEXT_ALIGN emTextAlign;                    // ������뷽ʽ
};

//  �������-���Ӵ�������Ϣ-������������
struct AV_CFG_VideoWidgetSensorInfo_Description 
{
	AV_int32			nStructSize;
	AV_int32			nSensorID;						// ��Ҫ�����Ĵ�������ID(��ģ��������ͨ����)
    char                szDevID[CFG_COMMON_STRING_32];  // �豸ID
    char                szPointID[CFG_COMMON_STRING_32];// ���ID
    char                szText[CFG_COMMON_STRING_256];  // ��Ҫ���ӵ�����
};

// �������-���Ӵ�������Ϣ
struct AV_CFG_VideoWidgetSensorInfo
{
	AV_int32			nStructSize;
	AV_BOOL				bPreviewBlend;					// ���ӵ�Ԥ����Ƶ
	AV_BOOL				bEncodeBlend;					// ���ӵ���������Ƶ����
	AV_CFG_Rect			stuRect;						// ����, ����ȡֵ0~8191
	AV_int32			nDescriptionNum;				// ��������������Ŀ
	AV_CFG_VideoWidgetSensorInfo_Description stuDescription[AV_CFG_Max_Description_Num];// ��������������Ϣ
};

// OSD�е����巽��FontSolution
typedef enum tagEM_FONT_SOLUTION
{
    EM_FONT_UNKNOWN,			// δ֪ 
    EM_FONT_DFAULT,             // Ĭ������ "default-font"
    EM_FONT_SIMKAI,			    // ���� "simkai"
    EM_FONT_SIMSUN,		        // ����"simsun"
}EM_FONT_SOLUTION;

// ��Ƶ�����������
struct AV_CFG_VideoWidget 
{
	AV_int32						nStructSize;
	AV_CFG_VideoWidgetChannelTitle	stuChannelTitle;	// ͨ������
	AV_CFG_VideoWidgetTimeTitle		stuTimeTitle;		// ʱ�����
	AV_int32						nConverNum;			// ���򸲸�����
	AV_CFG_VideoWidgetCover			stuCovers[AV_CFG_Max_Video_Widget_Cover];				// ��������
	AV_int32						nCustomTitleNum;	// �Զ����������
	AV_CFG_VideoWidgetCustomTitle	stuCustomTitle[AV_CFG_Max_Video_Widget_Custom_Title];	// �Զ������
	AV_int32						nSensorInfo;		// ��������Ϣ����������Ŀ
	AV_CFG_VideoWidgetSensorInfo	stuSensorInfo[AV_CFG_Max_Video_Widget_Sensor_Info];		// ��������Ϣ����������Ϣ
    double                          fFontSizeScale;            //���������С�Ŵ����
                                                               //��fFontSizeScale��0ʱ,nFontSize��������
                                                               //��fFontSizeScale=0ʱ,nFontSize������
                                                               //�豸Ĭ��fFontSizeScale=1.0
                                                               //�����Ҫ�޸ı������޸ĸ�ֵ
                                                               //�����Ҫ�����������ã����ø�ֵΪ0��nFontSize��ֵ��Ч
    AV_int32                        nFontSize;                 //���ӵ��������ϵ�ȫ�������С,��λ px.
                                                               //��fFontSizeScale��ͬ����
    AV_int32                        nFontSizeExtra1;           //���ӵ�������1�ϵ�ȫ�������С,��λ px
    AV_int32                        nFontSizeExtra2;           //���ӵ�������2�ϵ�ȫ�������С,��λ px
    AV_int32                        nFontSizeExtra3;           //���ӵ�������3�ϵ�ȫ�������С,��λ px
    AV_int32                        nFontSizeSnapshot;         //���ӵ�ץͼ���ϵ�ȫ�������С, ��λ px
    AV_int32                        nFontSizeMergeSnapshot;    //���ӵ�ץͼ���Ϻϳ�ͼƬ�������С,��λ px
    EM_FONT_SOLUTION                emFontSolutionSnapshot;    //���ӵ�ץͼ���ϵ����巽��
};

// �洢��ͨ���������
struct AV_CFG_StorageGroupChannel 
{
	AV_int32			nStructSize;
	AV_int32			nMaxPictures;					// ÿ��ͨ���ļ���ͼƬ�洢����, �����͸���
	char				szPath[AV_CFG_Max_ChannelRule];	// ͨ����������������ַ�����ʾ, %c��Ӧ������
};

// �洢������
struct AV_CFG_StorageGroup 
{
	AV_int32			nStructSize;	
	char				szName[AV_CFG_Group_Name_Len];	// ��������
	char				szMemo[AV_CFG_Group_Memo_Len];	// ����˵��
	AV_int32			nFileHoldTime;					// �ļ�����ʱ��
	AV_BOOL				bOverWrite;						// �洢�ռ����Ƿ񸲸�
	char				szRecordPathRule[AV_CFG_Max_Path];	// ¼���ļ�·����������
	char				szPicturePathRule[AV_CFG_Max_Path];	// ͼƬ�ļ�·����������
															// %y��, %M��, %d��, %hʱ, %m��, %s��, %cͨ��·��
															// ���������ʱ�����������, ��һ�α�ʾ��ʼʱ��, �ڶ��α�ʾ����ʱ��
	AV_CFG_StorageGroupChannel	stuChannels[AV_CFG_Max_Channel_Num];// ͨ��������� 
	AV_int32			nChannelCount;				        // ͨ��������
    char                szCustomName[AV_CFG_Group_Name_Len];        // �Զ������ƣ���Ϊ��ʹ��szName
    char                szSubDevices[MAX_DEV_NUM][MAX_DEVICE_NAME_LEN];     // ���豸�б�
    AV_int32            nSubDevices;                        // ���豸����
};

// DSTʱ��
struct AV_CFG_DSTTime 
{
	AV_int32			nStructSize;
	AV_int32			nYear;							// ��, 2000~2038
	AV_int32			nMonth;							// ��, 1~12
	AV_int32			nWeek;							// �ڼ���, 1-��һ��,2-�ڶ���,...,-1-���һ��,0-�����ڼ���
	AV_int32			nDay;							// ���ڼ�������
														// ���ܼ���ʱ, 0-����, 1-��һ,..., 6-����
														// ��������ʱ, ��ʾ����, 1~31
	AV_int32			nHour;							// Сʱ
	AV_int32			nMinute;						// ����
};

// ��������
struct AV_CFG_Locales 
{
	AV_int32			nStructSize;
	char				szTimeFormat[AV_CFG_Time_Format_Len];	// ʱ���ʽ
	AV_BOOL				bDSTEnable;						// ����ʱʱ��
	AV_CFG_DSTTime		stuDstStart;					// ����ʱ��ʼʱ��
	AV_CFG_DSTTime		stuDstEnd;						// ����ʱ����ʱ��
    AV_BOOL             bWeekEnable;                    // �����Ƿ�ʹ��
    unsigned char       ucWorkDay;                      // ��λ��ʾһ�ܵĹ��������룬bit0��ʾ���գ�bit1��ʾ��һ
};

// ��������
enum AV_CFG_LanguageType
{
	AV_CFG_Language_English,							// Ӣ��
	AV_CFG_Language_SimpChinese,						// ��������
	AV_CFg_Language_TradChinese,						// ��������
	AV_CFG_Language_Italian,							// �������
	AV_CFG_Language_Spanish,							// ��������
	AV_CFG_Language_Janpanese,							// ����
	AV_CFG_Language_Russian,							// ����
	AV_CFG_Language_French,								// ����
	AV_CFG_Language_German,								// ����
    AV_CFG_Language_Portugal,							// ��������
    AV_CFG_Language_Turkey,								// ��������
    AV_CFG_Language_Poland,								// ������
    AV_CFG_Language_Romanian,							// ��������
    AV_CFG_Language_Hungarian,							// ��������
    AV_CFG_Language_Finnish,							// ������
    AV_CFG_Language_Estonian,                           // ��ɳ������    
    AV_CFG_Language_Korean,                             // ����    
    AV_CFG_Language_Farsi,                              // ��˹��     
    AV_CFG_Language_Dansk,                              // ������
    AV_CFG_Language_Czechish,                           // �ݿ���
    AV_CFG_Language_Bulgaria,                           // ����������
    AV_CFG_Language_Slovakian,                          // ˹�工����
    AV_CFG_Language_Slovenia,                           // ˹����������
    AV_CFG_Language_Croatian,                           // ���޵�����
    AV_CFG_Language_Dutch,                              // ������
    AV_CFG_Language_Greek,                              // ϣ����
    AV_CFG_Language_Ukrainian,                          // �ڿ�����
    AV_CFG_Language_Swedish,                            // �����
    AV_CFG_Language_Serbian,                            // ����ά����
    AV_CFG_Language_Vietnamese,                         // Խ����
    AV_CFG_Language_Lithuanian,                         // ��������
    AV_CFG_Language_Filipino,                           // ���ɱ���
    AV_CFG_Language_Arabic,                             // ��������
    AV_CFG_Language_Catalan,                            // ��̩��������
    AV_CFG_Language_Latvian,                            // ����ά����
    AV_CFG_Language_Thai,                               // ̩��
    AV_CFG_Language_Hebrew,                             // ϣ������
    AV_CFG_Language_Bosnian,                            // ��˹������
};

// ��������
struct AV_CFG_Language 
{
	AV_int32			nStructSize;
	AV_CFG_LanguageType emLanguage;						// ��ǰ����
};

// ���ʵ�ַ����
struct AV_CFG_AccessFilter
{
	AV_int32			nStructSize;
	AV_BOOL				bEnable;						// ʹ��
	AV_int32			nType;							// ����, 0-������, 1-������
	AV_int32			nWhiteListNum;					// ������IP����
	char				szWhiteList[AV_CFG_Max_White_List][AV_CFG_Filter_IP_Len];	// ������
	AV_int32			nBlackListNum;					// ������IP��IP������
	char				szBlackList[AV_CFG_Max_Black_List][AV_CFG_Filter_IP_Len];	// ������
	AV_BOOL             bIndividual[AV_CFG_Max_White_List];   //������:��Ե���IP�����Ƿ�ʹ�ܡ�
															//ͨ��CLIENT_GetDevCaps/NET_USER_MNG_CAPS�鿴�Ƿ�֧��.                                                             
																// ���֧��:�ɻ�ȡ�����ø��ֶ�;����֧�֣�����ֶ���Ч
	AV_BOOL				bIndividualInBlackList[AV_CFG_Max_Black_List];  //������:��Ե���IP�����Ƿ�ʹ��
																// ͨ��CLIENT_GetDevCaps/NET_USER_MNG_CAPS�鿴�Ƿ�֧��.
															  // ���֧�֣��ɻ�ȡ�����ø��ֶ�;����֧�֣�����ֶ���Ч
};

// �Զ�ά��
struct AV_CFG_AutoMaintain 
{
	AV_int32			nStructSize;
	AV_int32			nAutoRebootDay;					// �Զ���������, -1����, 0~6����~����, 7ÿ��
	AV_int32			nAutoRebootHour;				// �Զ�����Сʱ, 0~23
	AV_int32			nAutoRebootMinute;				// �Զ���������, 0~59
	AV_int32			nAutoShutdownDay;				// �Զ��ػ�����
	AV_int32			nAutoShutdownHour;				// �Զ��ػ�Сʱ
	AV_int32			nAutoShutdownMinute;			// �Զ��ػ�����
	AV_int32			nAutoStartupDay;				// �Զ���������
	AV_int32			nAutoStartupHour;				// �Զ�����Сʱ
	AV_int32			nAutoStartupMinute;				// �Զ���������
	AV_BOOL				bAutoRebootEnable;				// true��ʾ�����Զ�������false��ʾ�ر��Զ�����
};

#define AV_CFG_Monitor_Name_Len		64			// ����ǽ���Ƴ���
#define AV_CFG_Max_TV_In_Block		128			// ������TV���������
#define AV_CFG_Max_Block_In_Wall	128			// ����ǽ��������������

// ����ǽ���ͨ����Ϣ
struct AV_CFG_MonitorWallTVOut
{
	AV_int32		nStructSize;
	char			szDeviceID[AV_CFG_Device_ID_Len];	// �豸ID, Ϊ�ջ�"Local"��ʾ�����豸
	AV_int32		nChannelID;							// ͨ��ID
	char			szName[AV_CFG_Channel_Name_Len];	// ��Ļ����
};

// ����ǽ����
struct AV_CFG_MonitorWallBlock 
{
	AV_int32				nStructSize;
	AV_int32				nLine;				// ����TVռ����������
	AV_int32				nColumn;			// ����TVռ����������
	AV_CFG_Rect				stuRect;			// �������������
	AV_int32				nTVCount;			// TV����
	AV_CFG_MonitorWallTVOut	stuTVs[AV_CFG_Max_TV_In_Block];					// TV����
	CFG_TIME_SECTION		stuTimeSection[WEEK_DAY_NUM][MAX_REC_TSECT];	// ���ػ�ʱ��
	char					szName[AV_CFG_Channel_Name_Len];				// ��������
	char					szCompositeID[AV_CFG_Device_ID_Len];			// �ں���ID
	char                    szBlockType[CFG_COMMON_STRING_32];   // ��ʾ��Ԫ������,Ϊ֧���ɽ��տ���ɵ�Ԫ��С���LED��������Ӹ��ֶ�,�������͵�������дΪ"LCD",�粻���ڸ��ֶ�,Ĭ�ϲ���LCD
};

// ����ǽ
struct AV_CFG_MonitorWall
{
	AV_int32			nStructSize;
	char				szName[AV_CFG_Monitor_Name_Len];	// ����
	AV_int32			nLine;								// ��������
	AV_int32			nColumn;							// ��������
	AV_int32			nBlockCount;						// ��������
	AV_CFG_MonitorWallBlock stuBlocks[AV_CFG_Max_Block_In_Wall];// ��������
    BOOL                bDisable;                           // �Ƿ����, 0-�õ���ǽ��Ч, 1-�õ���ǽ��Ч
    char                szDesc[CFG_COMMON_STRING_256];      // ����ǽ������Ϣ
};

// ƴ����
struct AV_CFG_SpliceScreen
{
	AV_int32			nStructSize;
	char				szName[AV_CFG_Channel_Name_Len];	// ƴ��������	
	char				szWallName[AV_CFG_Monitor_Name_Len];// ��������ǽ����
	AV_int32			nBlockID;							// �����������
	AV_CFG_Rect			stuRect;							// ��������(0~8191)
};

// ��̨��������
typedef enum tagAV_CFG_PtzLinkType
{
	AV_CFG_PtzLink_None,				// ������
	AV_CFG_PtzLink_Preset,				// ����Ԥ�õ�
	AV_CFG_PtzLink_Tour,				// ����Ѳ��
	AV_CFG_PtzLink_Pattern,				// �����켣
} AV_CFG_PtzLinkType;

// ������̨��Ϣ
typedef struct tagAV_CFG_PtzLink
{
	AV_int32            nStructSize;
	AV_CFG_PtzLinkType	emType;				// �������� 
	AV_int32			nParam1;			// ��������1
	AV_int32			nParam2;            // ��������2
	AV_int32			nParam3;            // ��������3
	AV_int32			nChannelID;         // ��������̨ͨ��
} AV_CFG_PtzLink;

// �����
typedef struct tagAV_CFG_Point
{
	AV_int32			nStructSize;
	AV_int32			nX;
	AV_int32			nY;
} AV_CFG_Point;

// ���
typedef struct tagAV_CFG_Size
{
	AV_int32			nStructSize;
	AV_uint32			nWidth;
	AV_uint32			nHeight;
} AV_CFG_Size;	

// �¼���������
typedef struct tagAV_CFG_EventTitle
{
	AV_int32			nStructSize;
	char				szText[AV_CFG_Channel_Name_Len];	// �����ı�
	AV_CFG_Point		stuPoint;			// �������Ͻ�����, ����0-8191�������ϵ
	AV_CFG_Size			stuSize;			// ����Ŀ�Ⱥ͸߶�,����0-8191�������ϵ��ĳ���������Ϊ0��ʾ������������Ӧ���
    AV_CFG_Color		stuFrontColor;		// ǰ����ɫ
    AV_CFG_Color		stuBackColor;		// ������ɫ
} AV_CFG_EventTitle;


// ��Ѳ��������
typedef struct tagAV_CFG_TourLink
{
	AV_int32			nStructSize;
	AV_BOOL				bEnable;			// ��Ѳʹ��
	CFG_SPLITMODE		emSplitMode;		// ��Ѳʱ�ķָ�ģʽ
	AV_int32			nChannels[AV_CFG_Max_Channel_Num];  // ��Ѳͨ�����б�
	AV_int32			nChannelCount;		// ��Ѳͨ������
} AV_CFG_TourLink;

// ��������
typedef struct tagAV_CFG_EventHandler
{
	AV_int32			nStructSize;
	AV_CFG_TimeSection  stuTimeSect[AV_CFG_Weekday_Num][AV_CFG_Max_TimeSection]; // �¼���Ӧʱ���
	AV_BOOL             bRecordEnable;		// ¼��ʹ��
	AV_uint32           nRecordMask[AV_CFG_Max_Channel_Num];	// ¼��ͨ�����б�
	AV_BOOL             abRecordLatch;		// ������, ��ʶnRecordLatch�Ƿ���Ч
	AV_int32            nRecordLatch;		// ¼����ʱʱ�䣨10~300�룩
	AV_BOOL				bAlarmOutEn;		// �������ʹ��
	AV_uint32			nAlarmOutMask[AV_CFG_Max_Channel_Num];	// �������ͨ�����б�
	AV_BOOL             abAlarmOutLatch;	// ������, ��ʶnAlarmOutLatch�Ƿ���Ч
	AV_int32			nAlarmOutLatch;		// ���������ʱʱ�䣨10~300�룩
	AV_BOOL				bExAlarmOutEn;		// ��չ�������ʹ��
	AV_uint32			nExAlarmOutMask[AV_CFG_Max_Channel_Num]; // ��չ�������ͨ���б�
	AV_BOOL				bPtzLinkEn;			// ��̨����ʹ��
	AV_int32            nPtzLinkNum;		// ��Ч��������Ŀ
	AV_CFG_PtzLink		stuPtzLink[AV_CFG_Max_Channel_Num];		// ��̨������
	AV_BOOL				bSnapshotEn;		// ����ʹ��
	AV_uint32			nSnapshotMask[AV_CFG_Max_Channel_Num];  // ����ͨ�����б�	
	AV_BOOL             abSnapshotPeriod;	// ������, ��ʶnSnapshotPeriod�Ƿ���Ч
	AV_int32			nSnapshotPeriod;	// ֡�����ÿ������֡ץһ��ͼƬ��һ��ʱ����ץ�ĵ���������ץͼ֡���йء�0��ʾ����֡������ץ�ġ�
	AV_BOOL             abSnapshotTimes;	// ������, nSnapshotTimes��Ч��
	AV_int32			nSnapshotTimes;		// ���Ĵ���, ��SnapshotEnableΪtrue������£�SnapshotTimesΪ0���ʾ����ץ�ģ�ֱ���¼�������
	AV_BOOL             bSnapshotTitleEn;	// �Ƿ����ͼƬ����
	AV_int32            nSnapTitleNum;      // ��ЧͼƬ������Ŀ
	AV_CFG_EventTitle   stuSnapTitles[AV_CFG_Max_Event_Title_Num]; // ͼƬ��������
	AV_BOOL				bTipEnable;			// ������Ϣ����ʾ
	AV_BOOL				bMailEnable;		// �����ʼ��������ͼƬ����Ϊ����
	AV_BOOL				bMessageEnable;		// �ϴ�������������
	AV_BOOL				bBeepEnable;		// ����
	AV_BOOL				bVoiceEnable;		// ������ʾ
	AV_BOOL             abDejitter;			// ������, nDejitter��Ч��
	AV_int32            nDejitter;			// �ź�ȥ����ʱ�䣬��λΪ��,0~100
	AV_BOOL             bLogEnable;			// �Ƿ��¼��־
	AV_BOOL             abDelay;			// nDelay��Ч��
	AV_int32            nDelay;				// ����ʱ����ʱ����Ч, ��λΪ��
	AV_BOOL             bVideoTitleEn;		// �Ƿ������Ƶ���⣬��Ҫָ������
	AV_int32            nVideoTitleNum;		// ��Ч��Ƶ������Ŀ
	AV_CFG_EventTitle	stuVideoTitles[AV_CFG_Max_Event_Title_Num]; // ��Ƶ��������
	AV_BOOL				bMMSEnable;			// ���Ͳ���ʹ��
	AV_int32            nTourNum;			// ��Ѳ������Ŀ������Ƶ���һ��
	AV_CFG_TourLink	    stuTour[AV_CFG_Max_Tour_Link_Num];		// ��Ѳ��������, ÿ����Ƶ�����Ӧһ������
	AV_int32            nDBKeysNum;			// �ؼ�������
	char	            szDBKeys[AV_CFG_Max_DBKey_Num][AV_CFG_DBKey_Len]; // �ؼ���
	AV_BOOL             abJpegSummary;		// ������, ��ʶbyJpegSummary�Ƿ���Ч
	AV_BYTE             byJpegSummary[AV_CFG_Max_Summary_Len]; // ���ӵ�JPEGͼƬ��ժҪ��Ϣ
} AV_CFG_EventHandler;

#define AV_CFG_Event_Code_Len	64		// �¼��볤��

// Զ���豸�¼�����
struct AV_CFG_RemoteEvent 
{
	AV_int32            nStructSize;
	AV_BOOL             bEnable;						 // ʹ��
	char                szDeviceID[AV_CFG_Device_ID_Len];// �豸ID
	char                szCode[AV_CFG_Event_Code_Len];	 // �¼���
	AV_int32            nIndex;							 // ���
	AV_BOOL             abSensorType;                    // nSensorType�Ƿ���Ч
	AV_uint32           nSensorType;                     // ���������� ����:1 or ����:0
	AV_CFG_EventHandler stuEventHandler;				 // ��������
};

// �¶ȱ�������
typedef struct tagAV_CFG_TemperatureAlarm
{
	AV_int32			nStructSize;
	AV_BOOL				bEnable;							// ����ʹ��
	char				szName[AV_CFG_Device_Name_Len];		// ����������
	AV_float			fNormalTempMin;						// �����¶���Сֵ
	AV_float			fNormalTempMax;						// �����¶����ֵ
	AV_CFG_EventHandler stuEventHandler;					// ��������
} AV_CFG_TemperatureAlarm;

// ����ת�ٱ�������
typedef struct tagAV_CFG_FanSpeedAlarm
{
	AV_int32			nStructSize;
	AV_BOOL				bEnable;							// ����ʹ��
	char				szName[AV_CFG_Device_Name_Len];		// ����������
	AV_uint32			nNormalSpeedMin;					// ����ת����Сֵ
	AV_uint32			nNormalSpeedMax;					// ����ת�����ֵ
	AV_CFG_EventHandler stuEventHandler;					// ��������
} AV_CFG_FanSpeedAlarm;

// Ӳ��������������
typedef struct tagCFG_DISK_FLUX_INFO 
{
	BOOL				bEnable;							// ����ʹ��
	unsigned int		nNormalFluxMin;						// ����������Сֵ, ��λKB
	unsigned int		nNormalFluxMax;						// �����������ֵ, ��λKB
	CFG_ALARM_MSG_HANDLE stuEventHandler;					// ��������
	CFG_TIME_SECTION	stuTimeSection[WEEK_DAY_NUM][MAX_REC_TSECT_EX];	// �¼���Ӧʱ���
} CFG_DISK_FLUX_INFO;

// ����������������
typedef struct tagCFG_NET_FLUX_INFO
{
	BOOL				bEnable;							// ����ʹ��
	unsigned int		nNormalFluxMin;						// ����������Сֵ, ��λKB
	unsigned int		nNormalFluxMax;						// �����������ֵ, ��λKB
	CFG_ALARM_MSG_HANDLE stuEventHandler;					// ��������
	CFG_TIME_SECTION	stuTimeSection[WEEK_DAY_NUM][MAX_REC_TSECT_EX];	// �¼���Ӧʱ���
} CFG_NET_FLUX_INFO;

// ������
typedef struct tagCFG_SERVER_INFO
{
	int                 nPort;                             // �������˿ں�
	char                szAddress[MAX_ADDRESS_LEN];        // IP��ַ��������
}CFG_SERVER_INFO;

// ����ע������
typedef struct tagCFG_REGISTER_SERVER_INFO
{
	BOOL                bEnable;                            // ����ע��ʹ��
	char                szDeviceID[MAX_ADDRESS_LEN];        // �豸ID
	int                 nServersNum;                        // ����������
	CFG_SERVER_INFO     stuServers[MAX_SERVER_NUM];         // ����������
}CFG_REGISTER_SERVER_INFO;

// ������ʱ��������
typedef enum tagSTREAM_POLICY
{
    STREAM_POLICY_UNKNOWN,
    STREAM_POLICY_NONE = 1,								// �޲���,������ʹ��"None"
    STREAM_POLICY_QUALITY,                              // ��������"Quality"  
    STREAM_POLICY_FLUENCY,                              // ����������"Fluency"
    STREAM_POLICY_AUTOADAPT,                            // �Զ�"AutoAdapt"
}EM_STREAM_POLICY;

// �ϴ�����
typedef enum tagEM_CFG_SENDPOLICY
{
	EM_SNEDPOLICY_UNKNOWN = -1,
	EM_SENDPOLICY_TIMING,								// ��ʱ�ϱ�
	EM_SENDPOLICY_EVENT,								// �¼������ϱ�
}EM_CFG_SENDPOLICY;

// ����ר������ע������
typedef struct tagCFG_REGISTERSERVER_VEHICLE
{
	BOOL				bEnable;						// ����ע��ʹ��
	BOOL				bRepeatEnable;					// �Ƿ�����ͬ��������
	char				szDeviceID[MAX_ADDRESS_LEN];	// ���豸ID
	int					nSendInterval;					// ���ͼ��, ��λ����
	char				szAddress[MAX_ADDRESS_LEN];		// IP��ַ��������
	int					nPort;							// �˿ں�
	EM_CFG_SENDPOLICY	emSendPolicy;					// �ϴ�����
	char				szTestAddress[MAX_ADDRESS_LEN];	// ����IP��ַ��������
	int					nTestPort;						// ���Զ˿ں�
	BYTE				byReserved[1024];				// �����ֽ�
}CFG_REGISTERSERVER_VEHICLE;

// ����Э������
typedef struct tagCFG_DVRIP_INFO
{
	int                 nTcpPort;                           // TCP����˿�,1025~65535
	int                 nSSLPort;                           // SSL����˿�,1025~65535
	int                 nUDPPort;                           // UDP����˿�,1025~65535
	int                 nMaxConnections;                    // ��������� 
	BOOL                bMCASTEnable;                       // �鲥ʹ��
	int                 nMCASTPort;                         // �鲥�˿ں�
	char                szMCASTAddress[MAX_ADDRESS_LEN];    // �鲥��ַ
	int                 nRegistersNum;                      // ����ע�����ø���
	CFG_REGISTER_SERVER_INFO stuRegisters[MAX_REGISTER_NUM];// ����ע������ 
    EM_STREAM_POLICY    emStreamPolicy;                     // ������ʱ��������
	CFG_REGISTERSERVER_VEHICLE	stuRegisterServerVehicle;	// ����ר������ע������
}CFG_DVRIP_INFO;

// AIO IP ����
typedef struct tagCFG_AIO_APP_CONFIG_INFO
{
	char				szAddress[MAX_IP_ADDR_LEN];			// ����ƽ̨�ĵ�ip��ַ
}CFG_AIO_APP_CONFIG_INFO;

// ����¼��ƻ�
typedef struct tagCFG_HOLIDAY_SCHEDULE
{
	BOOL				bEnable;
	int					nChannel;
	DWORD				dwMonthSchedule[MONTH_OF_YEAR];		// ÿ���µļ��ڰ���, ÿ��Ԫ�ر�ʾһ����, 
															// ����ӵ͵������α�ʾÿ��1-28/29/30/31���Ƿ�Ϊ����
															// 0:���Ǽ���, 1:�Ǽ���
	CFG_TIME_SECTION	stuTimeSection[MAX_REC_TSECT];		// ¼��ʱ���
} CFG_HOLIDAY_SCHEDULE;

// ¼�������ٶ�����
typedef struct tagCFG_RecordDownloadSpeed
{
	DWORD				nStructSize;						// �ṹ���С
	UINT				nMaxDownloadSpeedNum;				// ¼�������ٶ�ֵ��С���û���д
	UINT				*pDownloadSpeedValue;				// ¼�������ٶ�ֵ��Ч��Χ���ڴ����û����䣩����ָ���ڴ�ֵ1024*sizeof(UINT) ��λKB/S
	UINT				nDownloadSpeedNum;					// ���ص�¼�������ٶ�ֵ����(Ӧ�� <= nMaxDownloadSpeedNum)
} CFG_RecordDownloadSpeed;

// ¼��ش�����
typedef struct tagAV_CFG_RecordBackup
{
	AV_int32			nStructSize;
	AV_uint32			nBitrateLimit;						// �����������, ��λKbps
	AV_CFG_RemoteDevice	*pstuDevices;						// �����豸����, �û������ڴ�,��СΪsizeof(AV_CFG_RemoteDevice)*nMaxDeviceCount
	int					nMaxDeviceCount;					// �������������С, �û���д
	int					nRetDeviceCount;					// ���ص��豸����
} AV_CFG_RecordBackup;

typedef struct tagALARM_EVENT_INFO
{
	DWORD	        	dwEventType;					    // �¼����ͣ����dhnetsdk.h��"���ܷ����¼�����"
	int		         	nEventSize;							// ���¼����͹������ýṹ���С
} ALARM_EVENT__INFO;

// ÿ����Ƶ����ͨ����Ӧ�������¼���������pEventBuf������¼���Ϣ��ÿ���¼�������Ϣ����ΪALARM_EVENT_INFO+"�¼����Ͷ�Ӧ�Ĺ������ýṹ��"��
typedef struct tagALARM_ANALYSEEVENTS_INFO
{
	int	         		nEventCount;						// �¼�����
	char*				pEventBuf;							// ÿ����Ƶ����ͨ����Ӧ����Ƶ�����¼�����
	int		        	nEventBufLen;						// �����С
	
} ALARM_ANALYSEEVENTS_INFO;

// ������Ϣ
typedef struct tagCFG_COMM_INFO
{
	char				szProtocolName[MAX_PROTOCOL_NAME_LEN];	// ����Э��
	CFG_COMM_PROP		stuAttribute;							// ��������
	int					nAddress;								// �豸��ַ
} CFG_COMM_INFO;

// ��������
typedef struct tagCFG_COMMGROUP_INFO
{
	int					nCommNum;						// ��������
	CFG_COMM_INFO		stuComms[MAX_COMM_NUM];			// ��������
} CFG_COMMGROUP_INFO;

// ���紫��ģʽ
typedef enum tagCFG_ENUM_NET_TRANSMISSION_MODE
{
    CFG_ENUM_NET_MODE_ADAPT,                            // ����Ӧ
    CFG_ENUM_NET_MODE_HALF10M,                          // 10M��˫��
    CFG_ENUM_NET_MODE_FULL10M,                          // 10Mȫ˫��
    CFG_ENUM_NET_MODE_HALF100M,                         // 100M��˫��
    CFG_ENUM_NET_MODE_FULL100M,                         // 100Mȫ˫��
}CFG_ENUM_NET_TRANSMISSION_MODE;

// ��������
typedef enum tagCFG_ENUM_NET_INTERFACE_TYPE
{
    CFG_ENUM_NET_INTERFACE_TYPE_UNKNOWN,                // δ֪
    CFG_ENUM_NET_INTERFACE_TYPE_STANDARD,               // ��׼����
    CFG_ENUM_NET_INTERFACE_TYPE_MANAGER,                // ��������
    CFG_ENUM_NET_INTERFACE_TYPE_EXTEND,                 // ��չ����
}CFG_ENUM_NET_INTERFACE_TYPE;

//��̬��������
typedef enum tagCFG_THREE_STATUS_BOOL
{
    CFG_BOOL_STATUS_UNKNOWN = -1,  //δ֪
    CFG_BOOL_STATUS_FALSE  = 0 , 
    CFG_BOOL_STATUS_TRUE   = 1 ,
}CFG_THREE_STATUS_BOOL;

// ����ӿ�
typedef struct tagCFG_NETWORK_INTERFACE 
{
	char				szName[MAX_NAME_LEN];			// ����ӿ�����
	char				szIP[MAX_ADDRESS_LEN];			// ip��ַ
	char				szSubnetMask[MAX_ADDRESS_LEN];	// ��������
	char				szDefGateway[MAX_ADDRESS_LEN];	// Ĭ������
	BOOL				bDhcpEnable;					// �Ƿ���DHCP
	BOOL				bDnsAutoGet;					// DNS��ȡ��ʽ��dhcpʹ��ʱ��������Ϊtrue��֧��ͨ��dhcp��ȡ
	char				szDnsServers[MAX_DNS_SERVER_NUM][MAX_ADDRESS_LEN];	// DNS��������ַ
	int					nMTU;							// ��������䵥Ԫ
    char				szMacAddress[MAX_ADDRESS_LEN];	// mac��ַ
    BOOL                bInterfaceEnable;               // ����ӿ�ʹ�ܿ��أ���ʾ�����������Ƿ���Ч������Чʱ��IP��ַ�����õ������ϡ�
    BOOL                bReservedIPEnable;              // DHCPʧ��ʱ�Ƿ�ʹ�ñ���IP��ʹ�ñ���IPʱ��������DHCP����
    CFG_ENUM_NET_TRANSMISSION_MODE emNetTranmissionMode;// ���紫��ģʽ��Ĭ��adapt����Ӧģʽ
    CFG_ENUM_NET_INTERFACE_TYPE    emInterfaceType;     // ��������
    CFG_THREE_STATUS_BOOL          bBond;               // �Ƿ����������
} CFG_NETWORK_INTERFACE;

// ����ӿ�����
typedef struct tagCFG_NETWORK_INFO 
{
	char				szHostName[MAX_NAME_LEN];		// ��������
	char				szDomain[MAX_NAME_LEN];			// ������
	char				szDefInterface[MAX_NAME_LEN];	// Ĭ��ʹ�õ�����
	int					nInterfaceNum;					// ��������
	CFG_NETWORK_INTERFACE	stuInterfaces[MAX_NETWORK_INTERFACE_NUM];	// �����б�
} CFG_NETWORK_INFO;

// �ƴ洢Э������
typedef enum tagEM_CFG_NAS_CLOUDPROTOCOL_TYPE
{
    EM_CFG_NAS_CLOUDPROTOCOL_TYPE_UNKNOWN,              // δ֪
    EM_CFG_NAS_CLOUDPROTOCOL_TYPE_BAIDU_CLOUD,          // "BaiduCloud"
    EM_CFG_NAS_CLOUDPROTOCOL_TYPE_GOOGLE_DRIVE,         // "GoogleDrive"
    EM_CFG_NAS_CLOUDPROTOCOL_TYPE_DROPBOX,              // "Dropbox"
}EM_CFG_NAS_CLOUDPROTOCOL_TYPE;

// ����洢����
typedef struct tagCFG_NAS_INFO_NEW
{
	BOOL				            bEnable;						// ʹ��
	char				            szName[MAX_NAME_LEN];			// ����
	int					            nPortocol;						// Э������, 0-FTP, 1-SMB, 2-ISCSI, 3-NFS, 4-Cloud ,Э�������� 4-Cloudʱ��������Э�鶨��� emCloudProtocol
	char				            szAddress[MAX_ADDRESS_LEN];		// IP��ַ����ַ
    int					            nPort;							// �˿�
    char				            szUser[MAX_USERNAME_LEN];		// �û���
    char				            szPassword[MAX_PASSWORD_LEN];	// ����
    char				            szDirectory[MAX_NAME_LEN];		// �����Ŀ¼��
    int					            nCharEncoding;					// �ַ������ʽ, 0-UTF8, 1-GB2312
    int					            nTimeOut;						// ��ʱʱ��, ��λ����
    unsigned int		            nStreamID;						// ������ID
    CFG_NET_TIME_EX		            stuUpdateTime;					// ����ʱ��, ���ֶ�ֵ�仯����Ҫ���³�ʼ��iSCSI����
    EM_CFG_NAS_CLOUDPROTOCOL_TYPE   emCloudProtocol;                // �ƴ洢Э������
    char                            szSubDirectory[CFG_COMMON_STRING_256];               // �ͻ����豸�ڷ������ϵĴ洢��Ŀ¼��������IP��ַ���豸���кţ��豸�����ţ�Ϊ��ʹ���豸������
} CFG_NAS_INFO_NEW;

// ����洢����������, �������������
typedef struct tagCFG_NAS_GROUP_INFO
{
	int					nNasNum;						// �洢����������
	CFG_NAS_INFO_NEW	stuNasInfo[MAX_NAS_NUM];		// �洢����������	
} CFG_NAS_GROUP_INFO;

// ����洢����������
typedef struct tagCFG_NAS_INFO_EX 
{
	int					nVersion;						// 0, 1, ��������ȷ��, ֻ��
	CFG_NAS_INFO		stuNasOld;						// nVersion == 0ʱ��Ч, ��֧�ֵ�������
	CFG_NAS_GROUP_INFO	stuNasGroup;					// nVersion == 1ʱ��Ч, ֧�ֶ��������
} CFG_NAS_INFO_EX;

// �����ź�����
typedef enum _EM_CFG_VIDEO_SIGNAL_TYPE
{
    EM_CFG_VIDEO_SIGNAL_UNKNOWN, 
    EM_CFG_VIDEO_SIGNAL_CVBS, 
    EM_CFG_VIDEO_SIGNAL_SDI, 
    EM_CFG_VIDEO_SIGNAL_VGA, 
    EM_CFG_VIDEO_SIGNAL_DVI, 
    EM_CFG_VIDEO_SIGNAL_HDMI, 
    EM_CFG_VIDEO_SIGNAL_YPBPR, 
    EM_CFG_VIDEO_SIGNAL_SFP, 
    EM_CFG_VIDEO_SIGNAL_HDCVI, 
    EM_CFG_VIDEO_SIGNAL_DUALLINK,
    EM_CFG_VIDEO_SIGNAL_AHD,
    EM_CFG_VIDEO_SIGNAL_AUTO,
    EM_CFG_VIDEO_SIGNAL_TVI, 
} EM_CFG_VIDEO_SIGNAL_TYPE;

enum EM_CFG_VIDEO_LINE_TYPE
{
    EM_CFG_VIDEO_LINE_TYPE_UNKNOWN,
    EM_CFG_VIDEO_LINE_TYPE_COAXIAL,						//ͬ����
    EM_CFG_VIDEO_LINE_TYPE_TP10,						//10ŷķ�迹˫����
    EM_CFG_VIDEO_LINE_TYPE_TP17,						//17ŷķ�迹˫����
    EM_CFG_VIDEO_LINE_TYPE_TP25,						//25ŷķ�迹˫����
    EM_CFG_VIDEO_LINE_TYPE_TP35,						//35ŷķ�迹˫����
};

// ����ͨ����������
typedef struct tagCFG_VIDEO_IN_INFO 
{
	char				szDevType[MAX_NAME_LEN];		// ͨ�����ͣ�ָͨ�����ӵ��豸���ͣ�
	char				szDevID[MAX_NAME_LEN];			// ����ͷΨһ���
	char				szChnName[MAX_NAME_LEN];		// ͨ������
	char				szManufacturer[MAX_NAME_LEN];	// ����
	char				szModel[MAX_NAME_LEN];			// �豸�ͺ�
	char				szAddress[MAX_ADDRESS_LEN];		// ��װ��ַ
	char				szCivilCode[MAX_NAME_LEN];		// ��������
	char				szOwner[MAX_NAME_LEN];			// �豸����
	BOOL				bParental;						// �Ƿ������豸
	BOOL				bEnable;						// ͨ��ʹ��
	int					nRegisterWay;					// ע�᷽ʽ
														// 0-����sip3261��׼����֤ע��ģʽ
														// 1-���ڿ����˫����֤ע��ģʽ
														// 2-��������֤���˫����֤ע��ģʽ
	BOOL				bSecrecy;						// ��������, FALSE������, TRUE����
	char				szUpperDevID[MAX_NAME_LEN];		// �ϼ������豸�豸ID
	int					nUpperDevOutChn;				// �ϼ������豸���ͨ����
    char				szRemoteName[MAX_NAME_LEN];		// Զ��ͨ������
    EM_CFG_VIDEO_SIGNAL_TYPE emSignalType;              // �����ź�����
    EM_CFG_VIDEO_LINE_TYPE  emLineType;                 // ͨ���������µ�����
} CFG_VIDEO_IN_INFO;

// ��¼���̱���ƻ�(��Ӧ���� CFG_CMD_ENCODEPLAN)��ÿ��ͨ��(�������л�ͨ��)һ�����ýṹ��
typedef struct tagCFG_ENCODE_PLAN_INFO
{
	BOOL                bEnable;                        // ͨ���Ƿ���Ҫ����,FALSE:û�д�ͨ��,TRUE:��ͨ������
	unsigned int        nExpectTime;                    // ҵ��Ԥ��ʱ��	 ����,��λ������
	CFG_CAPTURE_SIZE    emResolution;                   // ��Ƶ�ֱ���
	unsigned int        nBitRate;                       // ��Ƶ�̶�����ֵ(kbps)
}CFG_ENCODE_PLAN_INFO;

//////////////////////////////////////////////////////////////////////////
// ˾����Ѷ���л�����
// С���洰����Ϣ
typedef struct tagCFG_SMALLPIC_INFO
{
	char                szDeviceID[AV_CFG_Device_ID_Len];// �豸ID
	int                 nChannelID;						 // ͨ����(0��ʼ)
	BOOL                bAudio;                          // �����Ƿ���С������Ƶ
    CFG_RECT            stuPosition;                     // ʹ�����������ϵ��ȡֵ��Ϊ0-8192,��������Ļ�ϵ�λ��
}CFG_SMALLPIC_INFO;

// �ָ�ͨ��
typedef struct tagCFG_SPLIT_CHANNEL_INFO
{
	BOOL                bEnable;                         // ʹ��
	char                szDeviceID[AV_CFG_Device_ID_Len];// �豸ID
	int                 nChannelID;						 // ͨ����(0��ʼ)
	unsigned int        nMaxSmallChannels;               // С����ͨ��������ÿ��ͨ��һ��CFG_SMALLPIC_INFO,�������Ӧ�����豸ͨ������һ
	unsigned int        nReturnSmallChannels;            // �������صĻ��װ���͵�С����ͨ������
	CFG_SMALLPIC_INFO  *pPicInfo;                        // С������Ϣ
}CFG_SPLIT_CHANNEL_INFO;

// �ָ��
typedef struct tagCFG_SPLIT_INFO
{
	CFG_SPLITMODE       emSplitMode;        // �ָ�ģʽ��ͨ��CLIENT_GetSplitCaps�ӿڻ�ȡ����emSplitMode
	unsigned int        nMaxChannels;       // �����ڴ�CFG_SPLIT_CHANNEL_INFO����,������16��ͨ����nMaxChannels����16��SPLITMODE_4ģʽ����˳�����η�Ϊ4��
	unsigned int        nReturnChannels;    // ��������ͨ������,Ҫ��װ���͵�ͨ������
	CFG_SPLIT_CHANNEL_INFO* pSplitChannels; // �ָ�ͨ����Ϣ
}CFG_SPLIT_INFO;

// ���л�����
typedef struct tagCFG_PICINPIC_INFO
{
	unsigned int        nMaxSplit;          // �ڴ������CFG_SPLIT_INFO����,���ֵͨ��CLIENT_GetSplitCaps�ӿڻ�ȡ����nModeCount
	unsigned int        nReturnSplit;       // �����õ�ʵ��ʹ�õĻ��װ���͵�CFG_SPLIT_INFO����
	CFG_SPLIT_INFO     *pSplits;            // �ָ��
}CFG_PICINPIC_INFO;

// ģ������ⲿ����Դ
typedef struct tagCFG_ANALOG_MATRIX_INPUT_CHN_SRC 
{
	char				szName[MAX_NAME_LEN];			// ����ͨ������
	char				szControlID[MAX_NAME_LEN];		// ���Ʊ��
	BOOL				bActive;						// �Ƿ񼤻�����
	int					nFormat;						// ����, 0-VGA
} CFG_ANALOG_MATRIX_INPUT_CHN_SRC;

// ����
typedef struct tagCFG_COM_PORT 
{
	int					nPortIndex;						// ����������
	int					nAddress;						// �豸��ַ
	CFG_COMM_PROP		stComProp;						// ��������
} CFG_COM_PORT;

// ģ�����
typedef struct tagCFG_ANALOG_MATRIX_INFO
{
	char				szDevID[MAX_NAME_LEN];			// �豸ID
	char				szName[MAX_NAME_LEN];			// �豸����
	char				szModel[MAX_MODEL_LEN];			// �ͺ�
	char				szUserName[MAX_USERNAME_LEN];	// �û���
	char				szPassword[MAX_PASSWORD_LEN];	// ����
	int					nChnInCount;					// ����ͨ������
	int					nChnOutCount;					// ���ͨ������
	CFG_ANALOG_MATRIX_INPUT_CHN_SRC* pstuChnInSrc;		// �ⲿ����Դ����, �û������ڴ�
	int					nChnInSrcCount;					// �ⲿ����Դ�����С
	int					nRetChnInSrcCount;				// ʵ�ʷ��ص��ⲿ����Դ����, һ��ͬnChnInCount
	CFG_COM_PORT		stComPort;						// ��������
	char				szProtocol[MAX_NAME_LEN];		// Э������
} CFG_ANALOG_MATRIX_INFO;

// ģ�����Э������
typedef struct tagCFG_ANALOG_MATRIX_PROTOCOL 
{
	char				szName[MAX_NAME_LEN];			// Э������
	char				szProtocolRule[MAX_NAME_LEN];	// Э������, ����%A-��ַ�룬%I-����˿ںţ�%O-����˿ں�
} CFG_ANALOG_MATRIX_PROTOCOL;

// ��Ƶ�������
typedef struct tagCFG_VIDEO_OUT_TITLE 
{
	char				szName[MAX_NAME_LEN];			// ͨ������
} CFG_VIDEO_OUT_TITLE;

// ��ʾԴ
typedef struct tagCFG_DISPLAY_SOURCE
{
	BOOL				bEnable;						// �Ƿ�ʹ��
	char				szDeviceID[MAX_NAME_LEN];		// �豸ID
	char				szControlID[MAX_NAME_LEN];		// ����ID
	int					nVideoChannel;					// ��Ƶͨ����
	int					nVideoStream;					// ��Ƶ����, -1-�Զ�, 0-������, 1-������1, 2-������2, 3-������3, 5-Ԥ������
	int					nAudioChannel;					// ��Ƶͨ����
	int					nAudioStream;					// ��Ƶ����, -1-�Զ�, 0-������, 1-������1, 2-������2, 3-������3, 5-Ԥ������
	BOOL				abDeviceInfo;					// �豸��Ϣ�Ƿ���Ч
	AV_CFG_RemoteDevice stuDeviceInfo;					// �豸��Ϣ
} CFG_DISPLAY_SOURCE;

// ����λ��
typedef struct tagCFG_SPLIT_WINDOW_POSITION 
{
	CFG_RECT			stuRect;						// ��������
	BOOL				bDirectable;					// �Ƿ�����ֱͨ����, ֱͨ��ָƴ������ʽ�£��˴�����������Ϊ����������
	int					nZorder;						// Z����
} CFG_SPLIT_WINDOW_POSITION;

// ƴ����������Ϣ
typedef struct tagCFG_SPLIT_WINDOW 
{
	BOOL				bEnable;						// �����Ƿ�����ƵԴ, ΪFALSEʱ��ʾû����Ƶ��Ҳ���ô���Source����
	int					nWindowID;						// ����ID
	char				szControlID[MAX_NAME_LEN];		// ����ID
	CFG_SPLIT_WINDOW_POSITION stuPosition;				// ����λ��
	CFG_DISPLAY_SOURCE	stuSource;						// ��ƵԴ
} CFG_SPLIT_WINDOW;

// ƴ��������
typedef struct tagCFG_BLOCK_COLLECTION 
{
	char				szBlockID[MAX_NAME_LEN];		// ƴ����ID
	char				szControlID[MAX_NAME_LEN];		// ����ID
	CFG_SPLITMODE		emSplitMode;					// �ָ�ģʽ
	CFG_SPLIT_WINDOW*	pstuWnds;						// ������Ϣ����, �û������ڴ�,��СΪsizeof(CFG_SPLIT_WINDOW)*nMaxWndCount
	int					nMaxWndCount;					// ������Ϣ�����С, �û���д
	int					nRetWndCount;					// ���صĴ�������
} CFG_BLOCK_COLLECTION;

// Ԥ������
typedef enum tagEM_COLLECTION_TYPE
{
    EM_COLLECTION_UNKNOWN,            // δ֪
    EM_COLLECTION_VIDEO,              // ��ƵԤ��
    EM_COLLECTION_ADDITION,           // ����Ԥ��
}EM_COLLECTION_TYPE;

// ����ǽ�е�ĳ��Ԥ������
typedef struct tagCFG_MONITORWALL_COLLECTION_SINGLE
{
	char				szName[MAX_NAME_LEN];			// ����
	char				szControlID[MAX_NAME_LEN];		// ����ID
	AV_CFG_MonitorWall	stuMonitorWall;					// ����ǽ����
	CFG_BLOCK_COLLECTION* pstuBlockCollections;			// ƴ����������Ϣ����, �û������ڴ�,��СΪsizeof(CFG_BLOCK_COLLECTION)*nMaxBlockCollectionCount
	int					nMaxBlockCollectionCount;		// ƴ�������������С, �û���д
	int					nRetBlockCollectionCount;		// ���ص�ƴ������������
    EM_COLLECTION_TYPE  emType;                         // Ԥ������
} CFG_MONITORWALL_COLLECTION_SINGLE;

// ����ǽԤ���б�, ��ʾĳ������ǽ��Ӧ�Ķ��Ԥ��
typedef struct tagCFG_MONITORWALL_COLLECTION_LIST
{
	CFG_MONITORWALL_COLLECTION_SINGLE* pstuCollections;	// Ԥ������, �û������ڴ�,��СΪsizeof(CFG_MONITORWALL_COLLECTION_SINGLE)*nMaxCollectionCount
	int					nMaxCollectionCount;			// Ԥ�������С, �û���д
	int					nRetCollectionCount;			// ���ص�Ԥ������
} CFG_MONITORWALL_COLLECTION;

// ��Ƶ����߽�����
typedef struct tagCFG_VIDEO_IN_BOUNDARY 
{
	CFG_RECT			stuBoundary;					// �߽�
} CFG_VIDEO_IN_BOUNDARY;

// ������Խӵ�����������,�����飬Ԫ�����ֵ��DH_PRODUCTION_DEFNITION.nMaxParkingSpaceScreen)
typedef struct tagCFG_PARKINGSPACE_SCREEN_INFO
{
	char                szAddress[MAX_PATH];            // ��������ַ������
	int                 nPort;                          // �������˿�
	char                szAuthorization[MAX_PATH];      // Ȩ����
    char                szDeviceNo[MAX_PATH];           // �豸���
	BOOL                bEnable;                        // �������Ƿ�����
} CFG_PARKINGSPACE_SCREEN_INFO;

// ��λ����(ר�г�λ����ͨ��λ����)�������飬Ԫ�����ֵ��DH_PRODUCTION_DEFNITION.nMaxRoadWays)
typedef struct tagCFG_PARKINGSPACE_STATUS_INFO
{
	int                  nLaneID;                        // ��λ��
	int                  nLaneType;                      // 0����ͨ��λ 1��ר�г�λ
} CFG_PARKINGSPACE_STATUS_INFO;

#define CFG_MAX_BINARY_NUM      128						// �Զ�������������

// ƽ̨�Զ�����Ϣ
typedef struct tagCFG_CLIENT_CUSTOM_INFO
{
	char                szDescription[MAX_PATH];        // ������Ϣ,��ͬƽ̨�Զ���������Ϣ,Ŀǰ֧��"LiFang"
	char*               pData;                          // �Զ������ݻ���, �����м䲻�ܴ��ڶ����Ƶ�0, Ҳ�����ַ�'\0'
	int                 nDataLen;                       // �û�����ĳ���, ��ͬƽ̨�Լ�ȷ��
    bool                abBinary;                       // false��ʾpData��nDataLen��Ч, ��nBinaryNum��dwBinary��Ч
                                                        // true��ʾpData��nDataLen��Ч, ��nBinaryNum��dwBinary��Ч
    BYTE                byReserved[3];
    int                 nBinaryNum;                     // dwBinary������Ч��Ա����
    DWORD               dwBinary[CFG_MAX_BINARY_NUM];   // �Զ�������
} CFG_CLIENT_CUSTOM_INFO;

// ��¼��ʽ����,�����Կ��ǣ��޴�����ʱĬ��DHAV��ʽ
typedef struct tagCFG_BURN_RECORD_FORMAT
{
	char                 szRecordPack[MAX_PATH];         // �������ģʽ:Ŀǰ֧��"DHAV","PS"
}CFG_BURN_RECORD_FORMAT;

// �����ͬ����¼(��Ӧ CFG_MULTIBURN_INFO) ��һ�����飬ÿһ��Ԫ�ر�ʾһ������ͬ����¼��У��
typedef struct tagCFG_MULTIBURN_INFO
{
	BOOL                 bEnable;                        // ������ʹ�ܿ���
    BOOL                 bDataCheck;                     // �����һ����У��ʹ��
    char                 szDataCheckSpeed[MAX_PATH];     // У���ٶ� "High":���٣�ͷβ����У�飩,"Normal":�������������У�飩,"Low":���� ��ȫ������У�飩
}CFG_MULTIBURN_INFO;

// ��������洢��������������
typedef struct tagCFG_REMOTE_STORAGE_LIMIT
{
    char				        szName[MAX_NAME_LEN];                       // ����
    int							nFileLen;									// �ļ����� ��λ,MB
	int							nInterval;									// �����ļ�ʱ���� ��λ,��
	CFG_CHANNEL_TIME_SECTION	*pstuChnTime;        	                    // �洢ʱ������飬�û������ڴ�,��СΪsizeof(CFG_CHANNEL_TIME_SECTION)*nMaxChnCount
	int                         nMaxChnCount;                               // ���ͨ�������û���д
    int                         nRetChnCount;                               // ���ص�ͨ����,��������ʱ����Чͨ����
}CFG_REMOTE_STORAGE_LIMIT;

// ����洢��������������
typedef struct tagCFG_REMOTE_STORAGELIMIT_GROUP
{
	CFG_REMOTE_STORAGE_LIMIT    stuRemoteStorageLimit[MAX_NAS_NUM];         // ����洢��������������
    int                         nLimitCount;                                // ��Ч�������������
}CFG_REMOTE_STORAGELIMIT_GROUP;

// ��ȡ��¼����������
typedef struct tagCFG_CAP_BURN_MANAGER	
{
    BOOL                 bSupportMultiBurn;              // �Ƿ�֧�ֶ����ͬ����¼
    int                  nMultiBurnGroups;               // ֧�ּ���ͬ����¼
}CFG_CAP_BURN_MANAGER;

// ¼��������Ϣ
typedef struct tagCFG_CAP_RECORD_INFO	
{
	BOOL  bSupportHoliday;				                 //¼��ƻ��Ƿ�֧�ּ��չ���
	BOOL  bSupportExtraRecordMode;						 //�Ƿ�֧�����ø�����¼��ģʽ	
	DWORD dwMaxPreRecordTime;							 //���Ԥ¼ʱ��
	DWORD dwMaxRemoteBitrate;							 //������������,��λkbps
	DWORD dwMaxRemoteRawBitrate;						 //�����뵐����������·�����ֱ��ʡ�֡�ʣ�ɫ�16bpp���㣩
    DWORD dwMaxStreams;                                  //���¼����������
} CFG_CAP_RECORD_INFO;

//-------------------------------�������---------------------------------
typedef  enum tagCFG_ENCRYPT_ALGO_TYPE
{
	ALGO_TYPE_AES	 , 
	ALGO_TYPE_UNKNOW ,
}CFG_ENCRYPT_ALGO_TYPE;

// ���������㷨����ģʽ
typedef enum  tagCFG_ENCRYPT_ALGO_WORKMODE
{
	ENCRYPT_ALGO_WORKMODE_ECB,     // ECBģʽ
	ENCRYPT_ALGO_WORKMODE_CBC,     // CBCģʽ
	ENCRYPT_ALGO_WORKMODE_CFB,     // CFBģʽ
	ENCRYPT_ALGO_WORKMODE_OFB,     // OFBģʽ
	ENCRYPT_ALGO_WORKMODE_UNKNOW ,
}CFG_ENCRYPT_ALGO_WORKMODE;

typedef union tagCFG_ENCRYPT_KEY_INFO
{
	BYTE byAesKey[AES_KEY_LEN];							// aes��Կ
}CFG_ENCRYPT_KEY_INFO;

//��������������Ϣ
typedef struct tagCFG_STREAM_ENCRYPT_INFO
{
	BOOL						bEncryptEnable;		// �Ƿ����
	CFG_ENCRYPT_ALGO_TYPE		emEncrptAlgoType;	// �����㷨����: �μ�ö������CFG_ENCRYPT_ALGO_TYPE
	CFG_ENCRYPT_ALGO_WORKMODE	emAlgoWorkMode;		// ����ģʽ,�ο�ö������ CFG_ENCRYPT_ALGO_WORKMODE
	WORD						wEncryptLenth;		// ��Կ���ȣ���ǰΪAES�㷨����ʱ����ʾ��Կλ��(Ŀǰ֧��128��192��256λ����, ��: wEncryptLenthΪ128������Կ��ϢENCRYPT_KEY_INFO���byAesKey[0]~[15])
	CFG_ENCRYPT_KEY_INFO		stuEncryptKeys;		// ͨ������Կ��Ϣ 
	CFG_NET_TIME				stuPreTime;			// ���ܼƻ��Ŀ�ʼʱ��
}CFG_STREAM_ENCRYPT_INFO;

//������������ͨ����Ϣ
typedef struct tagCFG_ENCODE_ENCRYPT_CHN_INFO
{
	int						 nChannelID;					// ͨ����(0��ʼ)
	CFG_STREAM_ENCRYPT_INFO *pstEncodeEncryptInfo;			// ÿ��ͨ���������б�,�û�������
												            // �����ڴ��СΪsizeof(CFG_STREAM_ENCRYPT_INFO)*nMaxEncryptInputs
	BYTE					 nMaxEncryptInputs; 
	BYTE					 nRetEncryptOutputs;			// ������CFG_STREAM_ENCRYPT�ĸ���
}CFG_ENCODE_ENCRYPT_CHN_INFO;


//��������Ӧ��������
typedef  enum tagCFG_ENCRYPT_POLICY_TYPE
{
	ENCRYPT_POLICY_UNKNOWN ,		//δ֪
	ENCRYPT_POLICY_NONE ,		//�޲��ԣ�����������Ӧ����
	ENCRYPT_POLICY_QUALITY ,    //�������ȣ����ȱ�֤�ֱ��ʣ�����֡��
	ENCRYPT_POLICY_FLUENCY ,	//�������ȣ����ȱ�֤�����ԣ����ͷֱ���
}CFG_ENCRYPT_POLICY_TYPE;

//�����������Ȳ���
typedef enum tagCFG_ENCRYPT_FLUENCY_MODE
{
	ENCRYPT_FLUENCY_UNKNOWN ,		//δ֪
	ENCRYPT_FLUENCY_FAST ,			//��ָ�(���ٱ��ʱ�������ָ�)
	ENCRYPT_FLUENCY_SLOW ,			//���ָ�(ͬһʱ��ֻ����һ��ͨ���ָ�)
	ENCRYPT_FLUENCY_SYNC ,			//ͬ���ָ�(�����ͨ���½���������ͨ�����ָ�)
	ENCRYPT_FLUENCY_SLOWSYNC ,		//��ͬ���ָ�(������ָ���ͬ���ָ�)
}CFG_ENCRYPT_FLUENCY_MODE;

//��������Ӧ����
typedef struct tagCFG_ENCODE_ADAPT_INFO
{
	CFG_ENCRYPT_POLICY_TYPE		emPolicyType;		//����Ӧ��������
	CFG_ENCRYPT_FLUENCY_MODE	emFluencyMode;		//�������Ȳ���
}CFG_ENCODE_ADAPT_INFO;

// �䱶���û�����Ϣ��Ԫ
typedef struct tagCFG_VIDEO_IN_ZOOM_UNIT
{
	int         nSpeed;									 //�䱶����(0~7)
	BOOL		bDigitalZoom;							 //�Ƿ����ֱ䱶
	int			nZoomLimit;								 //��ǰ���������䱶����(0~13)��
    EM_LIGHT_SCENE  emLightScene;                        //���ճ�������
} CFG_VIDEO_IN_ZOOM_UNIT;

// ��ͨ���䱶���û�����Ϣ
typedef struct tagCFG_VIDEO_IN_ZOOM	
{
	int						nChannelIndex;				 //ͨ����
	int						nVideoInZoomRealNum;		 //����ʹ�ø���
	CFG_VIDEO_IN_ZOOM_UNIT  stVideoInZoomUnit[MAX_VIDEO_IN_ZOOM];		 //ͨ���������õ�Ԫ��Ϣ
	
} CFG_VIDEO_IN_ZOOM;

// ץͼ����
typedef struct tagCFG_SNAP_INFO 
{
	CFG_TIME_SCHEDULE		stuTimeSchedule;			// ʱ���, ÿ��ʱ������밴λ��������: 
														// Bit0-��ʱץͼ
														// Bit1-��̬���ץͼ
														// Bit2-����ץͼ
														// Bit3-����ץͼ
														// Bit4~Bit31-����
} CFG_SNAP_INFO;

//��̨ת���Ƕȷ�Χ����λ����
typedef struct tagCFG_PTZ_MOTION_RANGE
{
	int                 nHorizontalAngleMin;       // ˮƽ�Ƕȷ�Χ��Сֵ,��λ:��
	int                 nHorizontalAngleMax;       // ˮƽ�Ƕȷ�Χ���ֵ,��λ:��
	int                 nVerticalAngleMin;          // ��ֱ�Ƕȷ�Χ��Сֵ,��λ:��
	int                 nVerticalAngleMax;          // ��ֱ�Ƕȷ�Χ���ֵ,��λ:��
}CFG_PTZ_MOTION_RANGE;

//֧�ֵĽ���ģʽ��Ӧö��
typedef enum tagSUPPORT_FOCUS_MODE
{
    ENUM_SUPPORT_FOCUS_CAR        = 1,                // ���峵ģʽ         
	ENUM_SUPPORT_FOCUS_PLATE      = 2,                // ���峵��ģʽ 
	ENUM_SUPPORT_FOCUS_PEOPLE     = 3,                // ������ģʽ     
	ENUM_SUPPORT_FOCUS_FACE       = 4,                // ��������ģʽ     
}EM_SUPPORT_FOCUS_MODE;

//��̨ת���Ƕȷ�Χ����λ����
typedef struct tagCFG_PTZ_LIGHTING_CONTROL
{
	char                szMode[CFG_COMMON_STRING_32];  // �ֶ��ƹ����ģʽ
	                                                   // on-off"��ֱ�ӿ���ģʽ,
	                                                   // "adjustLight"���ֶ���������ģʽ

	DWORD               dwNearLightNumber;             // �����������
	DWORD               dwFarLightNumber;              // Զ���������
}CFG_PTZ_LIGHTING_CONTROL;

//��̨-����ɨ��������
typedef struct tagCFG_PTZ_AREA_SCAN
{
	BOOL                bIsSupportAutoAreaScan;     // �Ƿ�֧������ɨ��	
	WORD				wScanNum;		    		// ����ɨ��ĸ���
}CFG_PTZ_AREA_SCAN;

// �ڵ�����״����
typedef enum tagNET_EM_MASK_TYPE
{
	NET_EM_MASK_UNKNOWN,			// δ֪
	NET_EM_MASK_RECT,				// ����
	NET_EM_MASK_POLYGON,			// �����
} NET_EM_MASK_TYPE;

// ����������
typedef enum tagNET_EM_MOSAIC_TYPE
{
	NET_EM_MOSAIC_UNKNOWN	= 0,				// δ֪
	NET_EM_MOSAIC_8			= 8,				// [8x8��С] ������
	NET_EM_MOSAIC_16		= 16,				// [16x16��С] ������
	NET_EM_MOSAIC_24		= 24,				// [24x24��С] ������
	NET_EM_MOSAIC_32		= 32,				// [32x32��С] ������
} NET_EM_MOSAIC_TYPE;

#define MAX_MASKTYPE_COUNT	8
#define MAX_MOSAICTYPE_COUNT	8
// ��˽�ڵ�������
typedef struct tagCFG_PTZ_PRIVACY_MASKING
{
	BOOL				bPrivacyMasking;					// �Ƿ�֧����˽�ڵ�����
	BOOL				bSetColorSupport;					// �Ƿ�֧���ڵ�����ɫ����
	BOOL				abMaskType;							// emMaskType�Ƿ���Ч
	int					nMaskTypeCount;						// ʵ��֧�ֵ��ڵ�����״����
	NET_EM_MASK_TYPE	emMaskType[MAX_MASKTYPE_COUNT];		// ֧�ֵ��ڵ�����״��û�и�������ʱĬ��֧�־���
	BOOL				bSetMosaicSupport;					// �Ƿ�֧���������ڵ�������
	BOOL				bSetColorIndependent;				// �Ƿ�֧���ڵ�����ɫ�໥����(bSetColorSupportΪtrueʱ��������Ч)
	BOOL				abMosaicType;						// emMosaicType�Ƿ���Ч
	int					nMosaicTypeCount;					// ʵ��֧�ֵ����������͸���
	NET_EM_MOSAIC_TYPE	emMosaicType[MAX_MOSAICTYPE_COUNT];	// ֧�ֵ�����������(SetMosaicSupportΪtrueʱ��������Ч��û�и�������ʱĬ��֧��24x24��С������)
} CFG_PTZ_PRIVACY_MASKING;

// ͼ��������
typedef struct tagCFG_PTZ_MEASURE_DISTANCE
{
	BOOL				bSupport;							// �Ƿ�֧��ͼ����
	BOOL				bOsdEnable;							// �Ƿ�ͼ���������ݵ���������
	int					nDisplayMin;						// ͼ������Ϣ����С��ʾʱ��, ��λ��
	int					nDisplayMax;						// ͼ������Ϣ�������ʾʱ��, ��λ��
} CFG_PTZ_MEASURE_DISTANCE;

//��ȡ��̨��������Ϣ
typedef struct tagCFG_PTZ_PROTOCOL_CAPS_INFO
{
	int                 nStructSize;
    BOOL                bPan;                       // �Ƿ�֧����̨ˮƽ�ڶ�
	BOOL                bTile;                      // �Ƿ�֧����̨��ֱ�ڶ�
	BOOL                bZoom;                      // �Ƿ�֧����̨�䱶
	BOOL                bIris;                      // �Ƿ�֧����̨��Ȧ����
	BOOL                bPreset;                    // �Ƿ�֧��Ԥ�õ�
	BOOL                bRemovePreset;              // �Ƿ�֧�����Ԥ�õ�
	BOOL                bTour;                      // �Ƿ�֧���Զ�Ѳ����·
	BOOL                bRemoveTour;                // �Ƿ�֧�����Ѳ��
	BOOL                bPattern;                   // �Ƿ�֧�ֹ켣��·
	BOOL                bAutoPan;                   // �Ƿ�֧���Զ�ˮƽ�ڶ�
	BOOL                bAutoScan;                  // �Ƿ�֧���Զ�ɨ��
	BOOL                bAux;                       // �Ƿ�֧�ָ�������
	BOOL                bAlarm;                     // �Ƿ�֧�ֱ�������
	BOOL                bLight;                     // �Ƿ�֧�ֵƹ�, ���ݼ�����"stuPtzLightingControl"�����ֶ��ѷϳ�ʹ��
	BOOL                bWiper;                     // �Ƿ�֧����ˢ
	BOOL                bFlip;                      // �Ƿ�֧�־�ͷ��ת
	BOOL                bMenu;                      // �Ƿ�֧����̨���ò˵�
	BOOL                bMoveRelatively;            // �Ƿ�֧����̨��������궨λ
	BOOL                bMoveAbsolutely;            // �Ƿ�֧����̨���������궨λ
    BOOL                bMoveDirectly;              // �Ƿ�֧����̨����ά���궨λ
	BOOL                bReset;                     // �Ƿ�֧����̨��λ
	BOOL                bGetStatus;                 // �Ƿ�֧�ֻ�ȡ��̨�˶�״̬����λ����
	BOOL                bSupportLimit;              // �Ƿ�֧����λ
	BOOL                bPtzDevice;                 // �Ƿ�֧����̨�豸
	BOOL                bIsSupportViewRange;        // �Ƿ�֧����̨������

	WORD				wCamAddrMin;		    	// ͨ����ַ����Сֵ
	WORD				wCamAddrMax;			    // ͨ����ַ�����ֵ
	WORD				wMonAddrMin;    			// ���ӵ�ַ����Сֵ
	WORD				wMonAddrMax;	    		// ���ӵ�ַ�����ֵ
	WORD				wPresetMin;			    	// Ԥ�õ����Сֵ
	WORD				wPresetMax;				    // Ԥ�õ�����ֵ
	WORD				wTourMin;    				// �Զ�Ѳ����·����Сֵ
	WORD				wTourMax;	    			// �Զ�Ѳ����·�����ֵ
	WORD				wPatternMin;	    		// �켣��·����Сֵ
	WORD				wPatternMax;		    	// �켣��·�����ֵ
	WORD				wTileSpeedMin;			    // ��ֱ�ٶȵ���Сֵ
	WORD				wTileSpeedMax;    			// ��ֱ�ٶȵ����ֵ
	WORD				wPanSpeedMin;	    		// ˮƽ�ٶȵ���Сֵ
	WORD				wPanSpeedMax;		    	// ˮƽ�ٶȵ����ֵ
	WORD				wAutoScanMin;			    // �Զ�ɨ�����Сֵ
	WORD				wAutoScanMax;    			// �Զ�ɨ������ֵ
	WORD				wAuxMin;		    		// �������ܵ���Сֵ
	WORD				wAuxMax;			    	// �������ܵ����ֵ

	DWORD				dwInterval;				    // ���������ʱ����
	DWORD				dwType;				        // Э������ͣ�0-������̨��1-Զ����̨
	DWORD				dwAlarmLen;				    // Э��ı�������
	DWORD				dwNearLightNumber;		    // �����������,0~4,Ϊ0ʱ��ʾ��֧��
	DWORD				dwFarLightNumber;		    // Զ���������,0~4,Ϊ0ʱ��ʾ��֧��

	DWORD               dwSupportViewRangeType;     // ֧�ֵĿ��������ݻ�ȡ��ʽ����,�ӵ�λ����λ������,Ŀǰ֧��
	                                                // ��1λ:Ϊ1��ʾ֧��"ElectronicCompass" �������̷�ʽ

	DWORD               dwSupportFocusMode;         // ֧�ֵ�֧�ֵĽ���ģʽ����,�ӵ�λ����λ������,��#EM_SUPPORT_FOCUS_MODE
	                  
	char				szName[MAX_PROTOCOL_NAME_LEN];                       // ������Э����
	char                szAuxs[CFG_COMMON_STRING_32][CFG_COMMON_STRING_32];  //��̨�������������б�

	CFG_PTZ_MOTION_RANGE stuPtzMotionRange;         // ��̨ת���Ƕȷ�Χ����λ����
	CFG_PTZ_LIGHTING_CONTROL stuPtzLightingControl; // �ƹ�������ݣ����ֶ��ѷϳ�ʹ��
	BOOL				bSupportPresetTimeSection;	// �Ƿ�֧��Ԥ�õ�ʱ������õĹ���
    BOOL                bFocus;                     // �Ƿ�֧����̨�佹
	CFG_PTZ_AREA_SCAN	stuPtzAreaScan;				// ����ɨ��������
	CFG_PTZ_PRIVACY_MASKING		stuPtzPrivacyMasking;	// ��˽�ڵ�������
	CFG_PTZ_MEASURE_DISTANCE	stuPtzMeasureDistance;	// ͼ����������
	BOOL				bSupportPtzPatternOSD;		// �Ƿ�֧����̨Ѳ��OSD����
	BOOL				bSupportPtzRS485DetectOSD;	// �Ƿ�֧����̨RS485���OSD����
	BOOL				bSupportPTZCoordinates;		// �Ƿ�֧����̨�������
	BOOL				bSupportPTZZoom;			// �Ƿ�֧����̨�䱶����
	BOOL				bDirectionDisplay;			// �Ƿ�֧����̨����״̬��ʾ
	DWORD				dwZoomMax;					// �䱶���ֵ
	DWORD				dwZoomMin;					// �䱶��Сֵ
}CFG_PTZ_PROTOCOL_CAPS_INFO;

//����֧�ֵ���̨Э��
typedef struct tagRS_PROTOCOL_InFO
{
	int					nRSProtocolNum;														//����֧�ֵ���̨Э�����
	char				szRSProtocol[CFG_COMMON_STRING_32][CFG_COMMON_STRING_32];			//����֧�ֵ���̨Э������
}RS_PROTOCOL_InFO;

//ͬ��֧�ֵ���̨Э��
typedef struct tagCOAXOAL_PROTOTOL_INFO
{
	int					nCoaxialProtocolNum;													//ͬ��֧�ֵ���̨Э�����
	char				szCoaxialProtocol[CFG_COMMON_STRING_32][CFG_COMMON_STRING_32];			//ͬ��֧�ֵ���̨Э������
}COAXOAL_PROTOTOL_INFO;

//��ȡ����̨ʵ���ܹ�ʹ�õ�Э�飬����������
typedef struct tagCFG_CAP_PTZ_PROTOCOL
{
	unsigned int			nStructSize;
	RS_PROTOCOL_InFO		stuRSProtocolInfo;			//����֧�ֵ���̨Э����Ϣ
	COAXOAL_PROTOTOL_INFO	stuCoaxialProtocolInfo;		//ͬ��֧�ֵ���̨Э����Ϣ
}CFG_CAP_PTZ_PROTOCOL;

// ������;Ŀ¼����
typedef struct tagCFG_SPECIAL_DIR_INFO 
{
	char				szBackgroudImageDir[MAX_PATH];	// ��Ļ��ͼĿ¼
	char				szWindowIconDir[MAX_PATH];		// ����ͼ��Ŀ¼
	char				szEventVoiceDir[MAX_PATH];		// ����������ʾ��Ƶ�ļ����Ŀ¼
} CFG_SPECIAL_DIR_INFO;


// �ػ�����ʱ�Զ���������
typedef struct tagCFG_AUTO_STARTUP_DELAY_INFO
{
	BOOL	bEnable;		//�����Ƿ��           
	int		nDelay;			//�ػ�����ʱ����ʱ��,��λ������(ȡֵ��Χ[30, 255])       
}CFG_AUTO_STARTUP_DELAY_INFO;

// ͸��ģʽö��
typedef enum tagCFG_DEFOG_MODE
{
	EM_DEFOG_MODE_UNKNOW, //δ֪��ʽ
	EM_DEFOG_MODE_OFF,    //�ر�
	EM_DEFOG_MODE_AUTO,	  //�Զ�
	EM_DEFOG_MODE_MANAL   //�ֶ�
}CFG_DEFOG_MODE;

//������ģʽö��
typedef enum tagCFG_LIGHT_INTENSITY_MODE
{
	EM_LIGHT_INTENSITY_MODE_UNKNOW,//δ֪��ʽ
	EM_LIGHT_INTENSITY_MODE_AUTO,  //�Զ�
	EM_LIGHT_INTENSITY_MODE_MANUL, //�ֶ�
}CFG_LIGHT_INTENSITY_MODE;


//͸������������Ϣ,��Ӧ�ṹ�� CFG_VIDEOINDEFOG
typedef struct tagCFG_VIDEOINDEFOG
{
	CFG_DEFOG_MODE				emDefogMode;//͸��ģʽ
	int							nIntensity; //ǿ��(0-100)
	CFG_LIGHT_INTENSITY_MODE	emLightIntensityMode;//������ģʽ
	int							nLightIntensityLevel;//������ǿ��(0-15)
	BOOL						bCamDefogEnable;//��ѧȥ��ʹ�� (TRUEʹ�ܣ�FALSEȥʹ��)
}CFG_VIDEOINDEFOG;

//����ͨ����Ӧ��͸��������Ϣ 
typedef struct tagCFG_VIDEOINDEFOG_LIST
{
	int							nVideoInDefogNum;						// ÿ��ͨ��ʵ��͸�����ø���
	CFG_VIDEOINDEFOG			stVideoInDefog[CFG_MAX_VIDEO_IN_DEFOG];	//ÿ��ͨ����Ӧ��͸����������
}CFG_VIDEOINDEFOG_LIST;

// RTMP����
// ������Ҫ��������������Flash Media Server��������
// �ṹ�������һ����������url
typedef struct tagCFG_RTMP_INFO
{
	BOOL		bEnable;									// RTMP�����Ƿ���
	char		szAddr[MAX_ADDRESS_LEN];					// RTMP��������ַ
	int			nPort;										// RTMP�������˿�
	int			nMainChnNum;								// ������ͨ������
	int			szMainChannel[AV_CFG_Max_Channel_Num];		// ����������ͨ�����б�:ÿ����Ա��ʾ��Ӧ��ͨ����Ҫ�ϴ���RTMP������,ͨ���Ŵ�0��ʼ
	int			nExtraChnNum;								// ������ͨ������
	int			szExtraChannel[AV_CFG_Max_Channel_Num];		// ���ø�����ͨ�����б�:ÿ����Ա��ʾ��Ӧ��ͨ����Ҫ�ϴ���RTMP������,ͨ���Ŵ�0��ʼ
	char		szCustomPath[MAX_ADDRESS_LEN];				// ����·����
	char		szStreamPath[MAX_ADDRESS_LEN];				// ����·��ǰ׺:��ͬͨ���Ժ�׺��������
}CFG_RTMP_INFO;

// ÿ��Ƶ�ʶ��ϵľ�����������Ϣ
typedef struct tagCFG_EQUALIZER_EACH_FREQUENCY
{
	int		nMinFrequency;		// ���������ε���СƵ�ʣ���λHZ����С��ȡ��0
	int		nMaxFrequency;		// ���������ε����Ƶ�ʣ���λHZ������ȡ��20K
	int		nGain;				// ���������ε�����
} CFG_EQUALIZER_EACH_FREQUENCY;

// ��Ӧ��Ƶ���ͨ��������Ƶ�ʶ��ϵ�����
typedef struct tagCFG_EQUALIZER_INFO
{
	int								nAudioOutChn;								// ��Ƶ���ͨ���ţ����ͨ����Ϊ16
	int								nFrequencyCount;							// ʵ��Ƶ�ʶθ���
	CFG_EQUALIZER_EACH_FREQUENCY	stuFrequency[MAX_FREQUENCY_COUNT];			// ��Ӧ��Ƶ���ͨ��������Ƶ�ʶ��ϵ�����
} CFG_EQUALIZER_INFO;

// ��Ƶ�������������(��ӦCFG_CMD_AUDIO_OUTEQUALIZER)
typedef struct tagCFG_AUDIO_OUTEQUALIZER_INFO
{
	int						nChannelNum;										// ��Ҫ���õ���Ƶͨ����
	CFG_EQUALIZER_INFO		stuEqualizerInfo[MAX_OUTAUDIO_CHANNEL_COUNT];		// ÿ����Ƶ���ͨ���ϵľ�������������Ϣ
} CFG_AUDIO_OUT_EQUALIZER_INFO;

// ÿ����Ƶ���ͨ���ϵ���Ƶ��������
typedef struct tagCFG_SUPPRESSION_INFO
{
	int			nAudioOutChn;				// ��Ƶ���ͨ�������ͨ����Ϊ16
	BOOL		bAutoFrequencyControl;		// Х������ʹ��
	BOOL		bAutoEchoCancellation;		// ��������ʹ��
} CFG_SUPPRESSION_INFO;

// ��Ƶ����������Ϣ(��ӦCFG_CMD_AUDIO_OUT_SUPPRESSION)
typedef struct tagCFG_AUDIO_OUT_SUPPRESSION_INFO
{
	int							nChannelNum;									// ��Ҫ���õ���Ƶͨ����
	CFG_SUPPRESSION_INFO		stuSuppressionInfo[MAX_OUTAUDIO_CHANNEL_COUNT];	// ÿ����Ƶ���ͨ���ϵ���Ƶ��������
} CFG_AUDIO_OUT_SUPPRESSION_INFO;

// ÿ����Ƶ����ͨ���ϵ���Ƶ��������
typedef struct tagCFG_IN_CONTROL_EACH_AUDIO
{
	int 		nAudioInChn;			// ��Ƶ����ͨ���ţ����ͨ���� Ϊ32
	BOOL		bPower;					// ����ʹ��
	BOOL		bSquishSuppression;		// ���빦��ʹ��
} CFG_IN_CONTROL_EACH_AUDIO;

// ��Ƶ�����������(��ӦCFG_CMD_AUDIO_INCONTROL)
typedef struct tagCFG_AUDIO_IN_CONTROL_INFO
{
	int								nChannelNum;										// ��Ҫ���õ���Ƶͨ����
	CFG_IN_CONTROL_EACH_AUDIO		stuInControl[MAX_INAUDIO_CHANNEL_COUNT];			// ÿ����Ƶ����ͨ���ϵ���Ƶ��������
} CFG_AUDIO_IN_CONTROL_INFO;

// ��Ƶ����������(CFG_CAP_CMD_VIDEOINPUT_EX)��չ����
typedef struct tagCFG_CAP_VIDEOINPUT_INFO_EX
{
	BOOL            bSupportDefog;                          // �Ƿ�֧��͸����
	BOOL			bSupportABFFunction;					// �Ƿ�֧���Զ����󽹹���
	BOOL			bSupportImageStabilization;				// �Ƿ�֧�ֵ��ӷ�������

	BOOL			bSupportKillShutLine;					// �Ƿ�֧�����������߹���
	int             nMinKillShutLine;						// �������Сֵ
	int			    nMaxKillShutLine;						// ��������ֵ

    BOOL            bSupportVideoInColor;	                //�Ƿ�֧��ͼ����������		 
    BOOL            bSupportBrightness;                 	//�Ƿ�֧������	
    BOOL            bSupportContrast;                       //�Ƿ�֧�ֶԱȶ�       	
    BOOL            bSupportSaturation;                     //�Ƿ�֧�ֱ��Ͷ�	
    BOOL            bSupportHue;                            //�Ƿ�֧��ɫ��	
    BOOL            bSupportVideoInFocus;                   //�Ƿ�֧�־۽�����
    BOOL			bLightingControl;						//�Ƿ�֧�ֺ���ƿ���
	BYTE            bReserved[3];                           //�ֽڶ���
}CFG_CAP_VIDEOINPUT_INFO_EX;

// CAN��������
#define MAX_CAN_COUNT	16			// ���CAN����

// CAN ���˲���
typedef enum tagCFG_CAN_POLICY_METHOD
{
	CFG_CAN_POLICY_METHOD_UNKONW,		// δ֪����
	CFG_CAN_POLICY_METHOD_NONE,			// �޹���
	CFG_CAN_POLICY_METHOD_WHITE,		// ������
	CFG_CAN_POLICY_METHOD_BLACK,		// ������
}CFG_CAN_POLICY_METHOD;

typedef struct tagCFG_CANFILTER_INFO
{
	int						nResponseTimeout;			// �ȴ�Ӧ��ʱ(��λ:����)�� �������ݺ󣬵ȴ�3000����󣬽�����·͸��
	int						nRequestTimeout;			// ��������Ӧ��ʱ(��λ:����)�� ����ָ��ʱ��δ�յ����ݣ���Ϊ�������
	int						nFilterPIDCount;			// ���ݺŸ���
	int						nRetFiterPIDCount;			// �������ݺŸ���
	int*					pnFilterPID;				// �������ݺ�
	CFG_CAN_POLICY_METHOD	emPolicy;					// ���˲���
														// ����ǰ��������ϱ�����PID������
														// ����Ǻ��������ϱ�����PID������
														// �޹������������ݶ��ϱ�																
}CFG_CANFILTER_INFO;

typedef struct tagCFG_CANFILTER_LIST
{
	int						nCANCount;			// CAN����
	int						nRetCANCount;		// �豸����CAN����
	CFG_CANFILTER_INFO		stuCANFilter[MAX_CAN_COUNT];			
}CFG_CANFILTER_LIST;

// �۽����޶�Ӧö��
typedef enum tagEM_FOCUS_LIMIT_SELECT_MODE
{
    EM_FOCUS_LIMIT_MODE_MANUAL,                // "Manual" �Զ�
    EM_FOCUS_LIMIT_MODE_AUTO,                  // "Auto"�ֶ�
    EM_FOCUS_LIMIT_MODE_INVALID
}EM_FOCUS_LIMIT_SELECT_MODE;

// �۽����û�����Ϣ��Ԫ
typedef struct tagCFG_VIDEO_IN_FOCUS_UNIT
{
	int                        nMode;                   // �۽�ģʽ, 0-�ر�, 1-�����۽�, 2-�Զ��۽�, 3-���Զ��۽�, 4-�ֶ��۽�
	int                        nSensitivity;            // �۽�������, 0-��, 1-Ĭ��, 2-��
	int                        nIRCorrection;           // �����۽�����, 0-������, 1-����, 2-�Զ�����
	EM_FOCUS_LIMIT_SELECT_MODE emFocusMode;             // �۽�����
	int	                       nFocusLimit;				// �۽�����ֵ, ��λ����
} CFG_VIDEO_IN_FOCUS_UNIT;

// ��ͨ���۽����û�����Ϣ
typedef struct tagCFG_VIDEO_IN_FOCUS	
{
	int						 nChannelIndex;				             // ͨ����
	int						 nVideoInFocusRealNum;		             // ����ʹ�ø���
	CFG_VIDEO_IN_FOCUS_UNIT  stVideoInFocusUnit[MAX_VIDEO_IN_FOCUS]; //ͨ���۽����õ�Ԫ��Ϣ
	
} CFG_VIDEO_IN_FOCUS;

typedef enum tagEM_ANALYSE_MODE
{
	EM_ANALYSE_MODE_REAL_STREAM,                            // ʵʱ��ģʽ     
	EM_ANALYSE_MODE_RECORDFILE_STREAM,                      // ¼���ļ���ģʽ
	EM_ANALYSE_MODE_PICFILE_STREAM,                         // ͼƬ�ļ���ģʽ
}EM_ANALYSE_MODE;

// ���ܷ���ģʽ
typedef struct tagCFG_ANALYSE_MODE
{
	EM_ANALYSE_MODE emMode;                                 // ����ģʽ�����EM_ANALYSE_MODE 
}CFG_ANALYSE_MODE;

// ��̨����Ԥ�õ�ṹ
typedef struct tagPTZ_PRESET
{
	BOOL                 bEnable;                             // ��Ԥ�õ��Ƿ���Ч
	char                 szName[MAX_PTZ_PRESET_NAME_LEN];     // Ԥ�õ�����
	CFG_PTZ_SPACE_UNIT   stPosition;                          // Ԥ�õ������ͷŴ���
}PTZ_PRESET;

// ��̨Ԥ�õ����ö�Ӧ�ṹ
typedef struct tagPTZ_PRESET_INFO
{
	DWORD              dwMaxPtzPresetNum;                     // ���Ԥ�õ����
	DWORD              dwRetPtzPresetNum;                     // ʵ��ʹ��Ԥ�õ����
	PTZ_PRESET         *pstPtzPreset;                         // Ԥ�õ���Ϣ(���������������ڴ�)
}PTZ_PRESET_INFO;

// ����������Ƶͨ������
typedef struct tagCFG_AUDIO_SPIRIT_CHANNE
{
	int				nChannel;				// ��������������Ƶͨ����
	int				nPriority;				// �����������ȼ�, 1~10
	int				nPreset;				// ��Ӧ����Ƶ�������Ԥ�õ�, ����̨һ��0~255
} CFG_AUDIO_SPIRIT_CHANNEL;

// ��������������
typedef struct tagCFG_AUDIO_SPIRIT_LINKAGE 
{
	int				nOutputNum;				// �������������
	int				nOutputChanel[CFG_MAX_LOWER_MATRIX_OUTPUT];	// ͬ�����������(���)���������
} CFG_AUDIO_SPIRIT_LINKAGE;

// ��������
typedef struct tagCFG_AUDIO_SPIRIT 
{
	BOOL				bEnable;			// ʹ��
	int					nAudioLimit;		// ����������ֵ, 0~100
	int					nDelayTime;			// ������ʱʱ��, ��λ: ��
	CFG_AUDIO_SPIRIT_CHANNEL* pstuChannels;	// ����������Ƶͨ������,���û������ڴ�,��СΪsizeof(CFG_AUDIO_SPIRIT_CHANNEL)*nChannelCount
	int					nChannelCount;		// ��Ƶͨ����
	CFG_AUDIO_SPIRIT_LINKAGE stuLinkage;	// ����������
} CFG_AUDIO_SPIRIT;

// ��Ƶ���ͨ����Ӧ������ͨ����Ϣ
typedef struct tagCFG_AUDIO_OUT_CHN
{
	BOOL	bEnable;									// ��ʾ��ǰ���ͨ���Ƿ���Ч
	int		nInputChn;									// ��ǰ���ͨ���ϵ�����ͨ������(bEnableΪTRUEʱ��Ч)
	short	snInputChannels[CFG_MAX_AUDIO_MATRIX_INPUT];// ��ǰ���ͨ���ϵ�����ͨ������(bEnableΪTRUEʱ��Ч)
} CFG_AUDIO_OUT_CHN;

// ��Ƶ�����µ����ͨ����Ϣ
typedef struct tagCFG_AUDIO_MATRIX_INFO
{
	BOOL				bEnable;										// ��ʾ��ǰ�����Ƿ���Ч
	int 				nAudioOutChn;									// ��ǰ�����µ���Ƶ���ͨ������(bEnableΪTRUEʱ��Ч)
	CFG_AUDIO_OUT_CHN	stuAudioOutChn[CFG_MAX_AUDIO_MATRIX_OUTPUT];	// ��ǰ�����µ���Ƶ���ͨ������(bEnableΪTRUEʱ��Ч)
} CFG_AUDIO_MATRIX_INFO;

// ��Ƶ����������Ϣ
typedef struct tagCFG_AUDIO_MATRIX
{
	int						nAudioMatrix;	// ��ȡ/���õ���Ч��Ƶ�������������4��
	CFG_AUDIO_MATRIX_INFO	stuAudioMatrix[CFG_MAX_AUDIO_MATRIX_NUM];	// ��Ƶ������������
} CFG_AUDIO_MATRIX;


//һ����Ƶ���ͨ����Ӧ�ľ�������ͨ��
typedef struct tagCFG_AUDIO_SILENCE_INPUTCHN
{
	int 		nMatrix;		//����Ƶ���ͨ�������ľ���ţ���0��ʼ
	int 		nOutChannel;	//����Ƶ���ͨ���ţ���0��ʼ
	int 		nInputChnConut;	//������������ͨ���ĸ���
	short  		snInputChannel[CFG_MAX_AUDIO_MATRIX_INPUT]; // ������������ͨ��
}CFG_AUDIO_SILENCE_INPUT_CHN;


//��������������Ϣ
typedef struct tagCFG_AUDIO_MATRIX_SILENCE
{
	int 						 nMaxInputListCount;	// ��Ҫ��ȡ/���õľ��������ͨ�����������û����䲢����pstSilenceInputChn��CFG_AUDIO_SILENCE_INPUTCHN �ṹ����
	int							 nRetInputListCountOut;	// ʵ�ʻ�ȡ�������ͨ����������������ʱ�˳�Ա������
	CFG_AUDIO_SILENCE_INPUT_CHN *pstSilenceInputChn; 	// ������Ƶ���ͨ����Ӧ�ľ�������ͨ�����û�����,��СΪsizeof(CFG_AUDIO_SILENCE_INPUT_CHN)*nMaxInputListCount
} CFG_AUDIO_MATRIX_SILENCE;

// �ϳ�ͨ������(��ӦCFG_CMD_COMPOSE_CHANNEL)
typedef struct tagCFG_COMPOSE_CHANNEL 
{
	CFG_SPLITMODE	emSplitMode;			// �ָ�ģʽ
	int				nChannelCombination[MAX_VIDEO_CHANNEL_NUM]; // ��ģʽ�µĸ��Ӵ�����ʾ����
	int				nChannelCount;			// �ָ������
} CFG_COMPOSE_CHANNEL;

// ��λ�����������
typedef struct tagCFG_LOWER_MATRIX_OUTPUT
{
	char					szName[CFG_COMMON_STRING_128]; // ���ͨ������
	CFG_SPLITMODE			emSplitMode;		// �ָ�ģʽ
	int						nInputs[CFG_MAX_LOWER_MATRIX_INPUT]; // ����ͨ����
	int						nInputCount;		// ����ͨ����
} CFG_LOWER_MATRIX_OUTPUT;

// ��λ������Ϣ
typedef struct tagCFG_LOWER_MATRIX_INFO 
{
	int						nOutputCount;			// ���ͨ����
	CFG_LOWER_MATRIX_OUTPUT	stuOutputs[CFG_MAX_LOWER_MATRIX_OUTPUT]; // ���ͨ������
} CFG_LOWER_MATRIX_INFO;

// ��λ��������
typedef struct tagCFG_LOWER_MATRIX_LIST
{
	int						nMatrixCount;		// ��λ��������
	CFG_LOWER_MATRIX_INFO	stuMatrix[CFG_MAX_LOWER_MATRIX_NUM]; // ��λ��������
} CFG_LOWER_MATRIX_LIST;

// ��ʼ���������
typedef struct tagCFG_DAYTIME_INFO
{
	int					nBeginMonth;		// ��ʼʱ�� �·�
	int					nBeginDay;			// ��ʼʱ�� ����
	int					nEndMonth;			// ����ʱ�� �·�
	int					nEndDay;			// ����ʱ��	����
}CFG_DAYTIME_INFO;

// ʱ�������ֵ����
typedef struct tagCFG_TIMESPEEDLIMIT_INFO
{
	BOOL				bEnable;			// �ٶ����ƿ���
	CFG_DAYTIME_INFO	stuDayTime;			// ʱ��ο�ʼ���������
	int					nDaySpeedLimit;		// ��������ֵ����λkm/h
	int					nNightSpeedLimit;	// ��������ֵ����λkm/h
	CFG_TIME_SECTION	stuTimeSection;		// ����ʱ��η�Χ��������Ч
}CFG_TIMESPEEDLIMIT_INFO;

typedef struct tagCFG_TIMESPEEDLIMIT_LIST
{
	int						nTimeSpeedLimitCount;		// ����ʱ��θ���
	CFG_TIMESPEEDLIMIT_INFO stuTimeSpeedLimit[MAX_TIMESPEEDLIMIT_NUM];
}CFG_TIMESPEEDLIMIT_LIST;

// ������ʾʱ��
typedef struct tagCFG_ALERTTIME_INFO
{
	int				nHour;				// Сʱ
	int				nMinute;			// ����
}CFG_ALERTTIME_INFO;

// ������ʾ����
typedef struct tagCFG_VOICEALERT_INFO
{
	CFG_ALERTTIME_INFO	stuAlertTime;							// ��������ʱ��㣬��ʱ����
	int					nIntervalTime;							// ���Ѽ��ʱ����λ��
	char				szWarnText[CFG_COMMON_STRING_256];		// �����������ݣ��û��Զ���
}CFG_VOICEALERT_INFO;

typedef struct tagCFG_VOICEALERT_LIST
{
	int					nVoiceAlertCount;					// ������ʾ����
	CFG_VOICEALERT_INFO stuVoiceAlert[MAX_VOICEALERT_NUM];
}CFG_VOICEALERT_LIST;



// ������尴��
typedef struct tagCFG_INFRARED_KEY 
{
	char			szName[CFG_COMMON_STRING_32];	// ��尴������
	char			szText[CFG_COMMON_STRING_64];	// ��尴����ʾ���֣���ťlabel
} CFG_INFRARED_KEY;

// �������ģ��
typedef struct tagCFG_INFRARED_BOARD_TEMPLATE 
{
	unsigned int	nID;							// ���ģ����
	char			szName[CFG_COMMON_STRING_64];	// ���Ʒ���ͺŶ���
	unsigned int	nCategory;						// �������, ����, 0-DVD, 1-TV
	int				nKeyNum;						// ��尴������
	CFG_INFRARED_KEY stuKeys[CFG_MAX_INFRARED_KEY_NUM];	// ��尴��
} CFG_INFRARED_BOARD_TEMPLATE;

// �������ģ����
typedef struct tagCFG_INFRARED_BOARD_TEMPLATE_GROUP 
{
	int			nTemplateNum;		// ģ������
	CFG_INFRARED_BOARD_TEMPLATE stuTemplates[CFG_MAX_INFRARED_BOARD_TEMPLATE_NUM];	// �������ģ��
} CFG_INFRARED_BOARD_TEMPLATE_GROUP;

// �������
typedef struct tagCFG_INFRARED_BOARD 
{
	unsigned int	nID;							// �����, Ψһ��ʾ
	char			szName[CFG_COMMON_STRING_64];	// ��������������, �û��Զ���
	unsigned int	nCategory;						// �������, 0-DVD, 1-TV
	int				nCommPort;						// ����ͨѶ�˿�
	unsigned int	nTemplateID;					// ������ģ��ID
} CFG_INFRARED_BOARD;

// ���������
typedef struct tagCFG_INFRARED_BOARD_GROUP
{
	int				nBoardNum;						// �����������
	CFG_INFRARED_BOARD stuBoards[CFG_MAX_INFRARED_BOARD_NUM];	// ���������Ϣ		
} CFG_INFRARED_BOARD_GROUP;

// �豸��������
typedef struct tagCFG_DEVICEKEEPALIVE_INFO
{
	BOOL	bEnable;		// ��������ʹ��
	int		nInterval;		// ����ʱ������������>=10��ֵ����λ����
	int		nFailTimes;		// ����ʧ�ܴ���������ʧ�ܴ��������ֵ��Ϊ�豸����
	int		nWaitBootTime;	// �ȴ�����ӵ���������Ᵽ����������λ����
}CFG_DEVICEKEEPALIVE_INFO;

// �豸�ع����û�����Ϣ
typedef struct tagCFG_VIDEOIN_EXPOSURE_BASE
{
    BOOL        bSlowShutter;           // ������ʹ��

    BYTE        byExposureMode;         // �ع�ģʽ����, 0- Ĭ���Զ�,1- ������,2- ����Ӱ,3- 50Hz����˸
                                        // 4- 60Hz����˸, 5- ��Ȧ����, 6- �ֶ�, 7- ��������, 8- ��������

    BYTE        byAntiFlicker;			// ����˸ģʽ 0-Outdoor 1-50Hz����˸ 2-60Hz����˸
    BYTE        byCompensation;         // �عⲹ��, [0~14]
    BYTE        byAutoGainMax;          // �Զ���������, [0, 2]
    BYTE        byGain;                 // ���浵λ, [0, 15]
    BYTE        bySlowAutoExposure;     // ���ع�, [0, 15]
    BYTE        bybyReserv[2];          // �ֽڶ���
    int         byExposureSpeed;        // �ع��ٶȵȼ���0-�Զ��ع⣬1-�ع�ȼ�1��2-�ع�ȼ�2��n-1����ع�ȼ��� n��ʱ�������޵��Զ��ع� n+1�Զ���ʱ���ֶ��ع� (nΪ֧�ֵ��ع�ȼ�)
    BYTE        bySlowSpeed;            // ���ع�ȼ�
    BYTE        byIris;                 // ��Ȧ����, [0~100]
    BYTE        byBacklight;            // ���ⲹ��: ȡֵ��Χȡ�����豸������: 0-�ر� 1-���� 2-ָ�����򱳹ⲹ��
    BYTE        byWideDynamicRange;     // ��ֵ̬ 0-�رգ�1~100-Ϊ��ʵ��Χֵ
    BYTE        byWideDynamicRangeMode; // ��̬ģʽ, 0:�ر�; 1:�ֶ�����; 2:�Զ�����
    BYTE        byGlareInhibition;      // ǿ������, 0:�ر�; 1:��; 2:��; 3:ǿ
    BYTE        byDoubleExposure;       // ˫����֧��, 0:��֧��; 1:֧��˫����ȫ֡��; 2:֧��˫���Ű�֡��; 3:֧��˫����ȫ֡�ʺͰ�֡��
    BYTE        byReserved;             // �ֽڶ���
    int         nRecoveryTime;          // �Զ��ع�ָ�, 0-��ʾ���ָ�, ������ʾ�ָ���ʱ��, ��λΪ��

    float       fValue1;                // �Զ��ع�ʱ�����޻����ֶ��ع��Զ���ʱ��, [0.1~80ms]
    float       fValue2;                // �Զ��ع�ʱ������, �費С������, [0.1~80ms]

    CFG_RECT    stuBacklightRegion;     // ���ⲹ������
    BYTE		byIrisMin;				// ��Ȧ��������[0~100]
    BYTE		byIrisMax;				// ��Ȧ��������,ȡֵ��С��byIrisMin[0~100]
    BYTE		byGainMin;				// �ֶ�(����)��½����
    BYTE		byGainMax;				// �ֶ�(����)��½����
}CFG_VIDEOIN_EXPOSURE_BASE;

// �豸�ع�����
typedef struct tagCFG_VIDEOIN_EXPOSURE_INFO
{
    DWORD                       dwMaxExposureNum;       // ���õ�������
    DWORD                       dwRetExposureNum;       // ʵ��ʹ�õ�������(��ѯʱ����ʵ�ʽ�������, ��װʱ���մ�ֵ������װ)
    CFG_VIDEOIN_EXPOSURE_BASE   *pstuVideoInExposure;   // �ع�������Ϣ,���û������ڴ�,��СΪsizeof(CFG_VIDEOIN_EXPOSURE_BASE)*dwMaxExposureNum
}CFG_VIDEOIN_EXPOSURE_INFO;

#define BACKLIGHT_CONFIG_COUNT	3	// ���߻������ø���

// ����ģʽ
typedef enum tagEM_VIDEOIN_BACKLIGHT_MODE
{
	EM_BACKLIGHT_UNKNOW,			// δ֪ģʽ
	EM_BACKLIGHT_OFF,				// �ر�
	EM_BACKLIGHT_BACKLIGHT,			// ���ⲹ��
	EM_BACKLIGHT_GLAREINHIBITION,	// ǿ������
	EM_BACKLIGHT_WIDEDYNAMIC,		// ��̬
	EM_BACKLIGHT_SSA,				// ��������Ӧ
} EM_VIDEOIN_BACKLIGHT_MODE;

// ���ⲹ��ģʽ��������ģʽΪBacklightʱ��Ч
typedef enum tagEM_BACKLIGHT_MODE
{
	EM_BACKLIGHT_UNKONW,			// δ֪ģʽ
	EM_BACKLIGHT_DEFAULT,			// Ĭ��ģʽ
	EM_BACKLIGHT_REGION,			// �Զ�������ģʽ
} EM_BACKLIGHT_MODE;

// SSA�Աȶȵ���ģʽ,������ģʽΪSSAʱ��Ч
typedef enum tagEM_INTENSITY_MODE
{
	EM_INTENSITY_UNKNOW,			// δ֪ģʽ
	EM_INTENSITY_DISABLE,			// �ر�
	EM_INTENSITY_AUTO,				// �Զ�
	EM_INTENSITY_MANUAL,			// �ֶ�
} EM_INTENSITY_MODE;

// ��������������Ϣ
typedef struct tagCFG_VIDEOIN_BACKLIGHT_BASE
{
	EM_VIDEOIN_BACKLIGHT_MODE		emMode;				// ����ģʽ
	EM_BACKLIGHT_MODE				emBacklightMode;	// ���ⲹ��ģʽ����emMode == EM_BACKLIGHT_BACKLIGHTʱ��Ч
	CFG_RECT						stuBacklightRegion; // ���ⲹ�����򣬵�emBacklightMode == EM_BACKLIGHT_REGIONʱ��Ч
	int								nWideDynamicRange;	// ��ֵ̬��emMode == EM_BACKLIGHT_WIDEDYNAMICʱ��Ч
	int								nGlareInhibition;	// ǿ�����ƣ�emMode == EM_BACKLIGHT_GLAREINHIBITIONʱ��Ч
	EM_INTENSITY_MODE				emInitensityMode;	// SSA�Աȶȵ���ģʽ��emMode==EM_BACKLIGHT_SSAʱ��Ч
	int								nIntensity;			// SSA�ֶ�����ǿ��ֵ��emInitensityMode==EM_INTENSITY_MANUALʱ��Ч
} CFG_VIDEOIN_BACKLIGHT_BASE;

// ������������
typedef struct tagCFG_VIDEOIN_BACKLIGHT_INFO
{
	// ��������������Ϣ,0-����1-ҹ�� 2-��ͨ
	CFG_VIDEOIN_BACKLIGHT_BASE		stuVideoInBackLight[BACKLIGHT_CONFIG_COUNT];
} CFG_VIDEOIN_BACKLIGHT_INFO;

//////////////////////////////////////////////////////////////////////////
// �Ž���������

// �Զ��忪�ŷ�ʽ
typedef enum tagCFG_DOOR_OPEN_METHOD
{
	CFG_DOOR_OPEN_METHOD_UNKNOWN                        = 0,
	CFG_DOOR_OPEN_METHOD_PWD_ONLY						= 1,    // ֻ�������뿪��
	CFG_DOOR_OPEN_METHOD_CARD							= 2,    // ֻ����ˢ������
	CFG_DOOR_OPEN_METHOD_PWD_OR_CARD					= 3,    // �����ˢ������
	CFG_DOOR_OPEN_METHOD_CARD_FIRST						= 4,    // ��ˢ�������뿪��
	CFG_DOOR_OPEN_METHOD_PWD_FIRST						= 5,    // �������ˢ������
	CFG_DOOR_OPEN_METHOD_SECTION						= 6,    // ��ʱ�ο���
    CFG_DOOR_OPEN_METHOD_FINGERPRINTONLY                = 7,    // ��ָ�ƿ���
    CFG_DOOR_OPEN_METHOD_PWD_OR_CARD_OR_FINGERPRINT     = 8,    // �����ˢ����ָ�ƿ���
    CFG_DOOR_OPEN_METHOD_PWD_AND_CARD_AND_FINGERPINT    = 9,    // ����+ˢ��+ָ����Ͽ���
    CFG_DOOR_OPEN_METHOD_PWD_AND_FINGERPRINT            = 10,   // ����+ָ����Ͽ���
    CFG_DOOR_OPEN_METHOD_CARD_AND_FINGERPRINT           = 11,   // ˢ��+ָ�ƿ���
    CFG_DOOR_OPEN_METHOD_MULTI_PERSON                   = 12,   // ���˿���
    CFG_DOOR_OPEN_METHOD_FACEIDCARD						= 13,   // ��֤�Ա�
    CFG_DOOR_OPEN_METHOD_FACEIDCARD_AND_IDCARD			= 14,	// ���֤+ ��֤�ȶ�
    CFG_DOOR_OPEN_METHOD_FACEIDCARD_OR_CARD_OR_FINGER	= 15,	// ��֤�ȶԻ�ˢ����ָ��
    CFG_DOOR_OPEN_METHOD_FACEIPCARDANDIDCARD_OR_CARD_OR_FINGER	= 16,	// (���֤+��֤�ȶ�)��ˢ����ָ��
    CFG_DOOR_OPEN_METHOD_USERID_AND_PWD                 = 17,   // UserID+����
    CFG_DOOR_OPEN_METHOD_FACE_ONLY                      = 18,   // ֻ������������
    CFG_DOOR_OPEN_METHOD_FACE_AND_PWD                   = 19,   // ����+���뿪��
    CFG_DOOR_OPEN_METHOD_FINGERPRINT_AND_PWD            = 20,   // ָ��+���뿪��
    CFG_DOOR_OPEN_METHOD_FINGERPRINT_AND_FACE           = 21,   // ָ��+��������
    CFG_DOOR_OPEN_METHOD_CARD_AND_FACE                  = 22,   // ˢ��+��������
    CFG_DOOR_OPEN_METHOD_FACE_OR_PWD                    = 23,   // ���������뿪��
    CFG_DOOR_OPEN_METHOD_FINGERPRINT_OR_PWD             = 24,   // ָ�ƻ����뿪��
    CFG_DOOR_OPEN_METHOD_FINGERPRINT_OR_FACE            = 25,   // ָ�ƻ���������
    CFG_DOOR_OPEN_METHOD_CARD_OR_FACE                   = 26,   // ˢ������������
    CFG_DOOR_OPEN_METHOD_CARD_OR_FINGERPRINT            = 27,   // ˢ����ָ�ƿ���
    CFG_DOOR_OPEN_METHOD_FINGERPRINT_AND_FACE_AND_PWD   = 28,   // ָ��+����+���뿪��
    CFG_DOOR_OPEN_METHOD_CARD_AND_FACE_AND_PWD          = 29,   // ˢ��+����+���뿪��
    CFG_DOOR_OPEN_METHOD_CARD_AND_FINGERPRINT_AND_PWD   = 30,   // ˢ��+ָ��+���뿪��
    CFG_DOOR_OPEN_METHOD_CARD_AND_PWD_AND_FACE          = 31,   // ��+ָ��+������Ͽ���
    CFG_DOOR_OPEN_METHOD_FINGERPRINT_OR_FACE_OR_PWD     = 32,   // ָ�ƻ�����������
    CFG_DOOR_OPEN_METHOD_CARD_OR_FACE_OR_PWD               = 33,   // �������������뿪��
    CFG_DOOR_OPEN_METHOD_CARD_OR_FINGERPRINT_OR_FACE    = 34,   // ����ָ�ƻ���������
    CFG_DOOR_OPEN_METHOD_CARD_AND_FINGERPRINT_AND_FACE_AND_PWD  = 35,   // ��+ָ��+����+������Ͽ��� 
    CFG_DOOR_OPEN_METHOD_CARD_OR_FINGERPRINT_OR_FACE_OR_PWD     = 36,   // ����ָ�ƻ����������뿪��
    CFG_DOOR_OPEN_METHOD_FACEIPCARDANDIDCARD_OR_CARD_OR_FACE    = 37,   //(���֤+��֤�ȶ�)�� ˢ�� �� ����
    CFG_DOOR_OPEN_METHOD_FACEIDCARD_OR_CARD_OR_FACE        = 38,   // ��֤�ȶ� �� ˢ��(��ά��) �� ����
	CFG_DOOR_OPEN_METHOD_CARDANDPWD_OR_FINGERPRINTANDPWD   = 39,   // (��+���룩��ָ��+���룩Ĭ��Ϊ2
	CFG_DOOR_OPEN_METHOD_PHOTO_OR_FACE					= 40,	//��֤(��Ƭ)������
	CFG_DOOR_OPEN_METHOD_FINGERPRINT					= 41,	//��֤(ָ��)
	CFG_DOOR_OPEN_METHOD_PHOTO_AND_FINGERPRINT			= 42,	//��֤(��Ƭ+ָ��)
}CFG_DOOR_OPEN_METHOD;


// �Ž���˫����������
typedef enum tagCFG_ACCESS_PROPERTY_TYPE
{
    CFG_ACCESS_PROPERTY_UNKNOWN = 0,
    CFG_ACCESS_PROPERTY_BIDIRECT,       // ˫���Ž�
    CFG_ACCESS_PROPERTY_UNIDIRECT,      // �����Ž�
}CFG_ACCESS_PROPERTY_TYPE;

#define CFG_MAX_ABLOCK_DOORS_NUM        4                       // �����Ž��Ļ�����ͨ����

// �Ž���AB��������
typedef struct tagCFG_ABLOCK_DOOR_INFO 
{
    int                 nDoor;                                  // ��Ч�����ŵĸ���
    int                 anDoor[CFG_MAX_ABLOCK_DOORS_NUM];       // �������ŵ�ͨ����
}CFG_ABLOCK_DOOR_INFO;

#define CFG_MAX_ABLOCK_GROUP_NUM        8                       // ���Ļ�������

// �Ž���AB��������, �����ͨ����ֻ������Bͨ�����رգ�Ψһ��Aͨ���Ŵ�
typedef struct tagCFG_ABLOCK_INFO 
{
    BOOL                bEnable;                                // ʹ��
    int                 nDoors;                                 // ��Ч��������
    CFG_ABLOCK_DOOR_INFO stuDoors[CFG_MAX_ABLOCK_GROUP_NUM];    // ��������Ϣ
}CFG_ABLOCK_INFO;

// ͨ��ģʽ
typedef enum tagEM_PASS_MODE
{
    EM_PASS_MODE_UNKNOWN = -1,                                   // δ֪
    EM_PASS_MODE_IN_AUTHORIZATION_OUT_AUTHORIZATION,             // ����Ȩ����Ȩ 
    EM_PASS_MODE_IN_AUTHORIZATION_OUT_FREE,                      // ����Ȩ������ 
    EM_PASS_MODE_IN_AUTHORIZATION_OUT_FORBID,                    // ����Ȩ����ֹ 
    EM_PASS_MODE_IN_FREE_OUT_AUTHORIZATION,                      // �����ɳ���Ȩ
    EM_PASS_MODE_IN_FREE_OUT_FREE,                               // �����ɳ����� 
    EM_PASS_MODE_IN_FREE_OUT_FORBID,                             // �����ɳ���ֹ
    EM_PASS_MODE_IN_FORBID_OUT_AUTHORIZATION,                    // ����ֹ����Ȩ
    EM_PASS_MODE_IN_FORBID_OUT_FREE,                             // ����ֹ������
    EM_PASS_MODE_IN_FORBID_OUT_FORBID,                           // ����ֹ����ֹ
} EM_PASS_MODE;

// բ������ײģʽ
typedef enum tagEM_COLLISION_MODE
{
    EM_COLLISION_MODE_UNKNOWN = -1,                               
    EM_COLLISION_MODE_2S,                                       // 2s�ɿ����                            
    EM_COLLISION_MODE_5S,                                       // 5s�ɿ����
    EM_COLLISION_MODE_LEAVING,                                  // ��Ա�뿪�ɿ����
} EM_COLLISION_MODE;

// բ���ϵ��Ժ��Űڶ�����
typedef enum tagEM_DIRECTION_AFTER_POWER_OFF
{
    EM_DIRECTION_AFTER_POWER_OFF_UNKNOWN = -1,                  // δ֪         
    EM_DIRECTION_AFTER_POWER_OFF_IN,                            // ���ŷ���                                
    EM_DIRECTION_AFTER_POWER_OFF_OUT,                           // ���ŷ���    
} EM_DIRECTION_AFTER_POWER_OFF;

// ����ģʽ
typedef enum tagEM_ASG_WORK_MODE
{
    EM_ASG_WORK_MODE_UNKNOWN  = -1,                             // δ֪
    EM_ASG_WORK_MODE_CLOSE,                                     // ����ģʽ
    EM_ASG_WORK_MODE_OPEN,                                      // ����ģʽ        
} EM_ASG_WORK_MODE;

// ����ģʽ
typedef enum tagEM_STARTUP_MODE
{   
    EM_STARTUP_MODE_UNKNOWN  = -1,                              // δ֪
    EM_STARTUP_MODE_NORMAL,                                     // ��������ģʽ
    EM_STARTUP_MODE_ZERO_SETTING,                               // ��λ��������ģʽ
    EM_STARTUP_MODE_AGING_TEST,                                 // �ϻ���������ģʽ   
} EM_STARTUP_MODE;

// բ������    
typedef enum tagEM_GATE_TYPE
{   
    EM_GATE_TYPE_UNKNOWN  = -1,                                 // δ֪
    EM_GATE_TYPE_SWING,                                         // ��բ
    EM_GATE_TYPE_WING,                                          // ��բ   
} EM_GATE_TYPE;

// ͨ�����
typedef enum tagEM_CHANNEL_WIDTH
{
    EM_CHANNEL_WIDTH_UNKNOWN = -1,                              // δ֪
    EM_CHANNEL_WIDTH_600,                                       // 600mm
    EM_CHANNEL_WIDTH_700,                                       // 700mm
    EM_CHANNEL_WIDTH_800,                                       // 800mm
    EM_CHANNEL_WIDTH_900,                                       // 900mm
    EM_CHANNEL_WIDTH_1000,                                      // 1000mm
    EM_CHANNEL_WIDTH_1100,                                      // 1100mm
} EM_CHANNEL_WIDTH;

// ��ͨ������բ������
typedef struct tagCFG_ACCESS_CONTROL_ASG
{
    EM_PASS_MODE                                    emPassMode;                              // ͨ��ģʽ
    UINT                                            nOpenDoorSpeed;                          // �����ٶȵ�λ, ��Χ:0-7, ��Ӧ1-8��
    UINT                                            nPassTimeOut;                            // ͨ�г�ʱʱ��, ��Χ:2-60, ��λ����
    UINT                                            nCloseDelayTime;                         // ��ʱ����ʱ��, ��Χ:0-60, ��λ����
    UINT                                            nSecurityLevel;                          // բ����ȫ�ȼ�, ��Χ:0-4, ��Ӧ1-5��
    BOOL                                            bSecondOpenEnable;                       // ���ο������ܿ���;False���ر�, True������
    BOOL                                            bMemoryModeEnable;                       // ���书�ܿ���;False���ر�, True������
    EM_COLLISION_MODE                               emCollisionMode;                         // բ������ײģʽ
    UINT                                            nVolumeLevel;                            // �豸�����ȼ���8��, ��Χ:0-7, 0����ʾ����
    EM_DIRECTION_AFTER_POWER_OFF                    emDirectionAfterPowerOff;                // բ���ϵ��Ժ��Űڶ�����
    EM_ASG_WORK_MODE                                emWorkMode;                              // ����ģʽ
    EM_STARTUP_MODE                                 emStartUpMode;                           // ����ģʽ
    int                                             nMasterWingAngleAdjust;                  // ������������λ΢���Ƕȣ����ŷ������ֱ�Ϊ����������
    int                                             nSlaveWingAngleAdjust;                   // �ӻ���������λ΢���Ƕȣ����ŷ������ֱ�Ϊ�ӻ�������
    EM_GATE_TYPE                                    emGateType;                              // բ������   
    EM_CHANNEL_WIDTH                                emChannelWidth;                          // ͨ�����
    BYTE                                            byReserved[128];                         // �����ֽ�                                       
} CFG_ACCESS_CONTROL_ASG;

// ����ID
typedef enum tagCFG_EM_VOICE_ID
{
	CFG_EM_VOICE_ID_UNKNOWN = -1,						// δ֪
	CFG_EM_VOICE_ID_VERIFY_SUCCESS,						// ��֤�ɹ�(Ĭ��)
	CFG_EM_VOICE_ID_OPENDOOR_SUCCESS,					// ���ųɹ�
	CFG_EM_VOICE_ID_UNLOCK_SUCCESS,						// �����ɹ�
	CFG_EM_VOICE_ID_SWIPECARD_SUCCESS,					// ˢ���ɹ�
	CFG_EM_VOICE_ID_WELCOME,							// ��ӭ����
	CFG_EM_VOICE_ID_WELCOME_BACK,						// ��ӭ�ٴι���
	CFG_EM_VOICE_ID_THANKS,								// лл
	CFG_EM_VOICE_ID_CUSTOMIZED_VOICE,					// �Զ���
}CFG_EM_VOICE_ID;

// �Ž�����������Ϣ
typedef struct tagCFG_ACCESS_VOICE_INFO
{
	CFG_EM_VOICE_ID			emVoiceID;					// ����ID
	char					szVoiceContent[128];		// ��������
	char					szFileName[128];			// �����ļ�·��������
	BYTE					byReserved[1024];			// Ԥ���ֶ�
}CFG_ACCESS_VOICE_INFO;

// �Ž�������ʾ��
typedef struct tagCFG_ACCESS_VOICE
{
	CFG_EM_VOICE_ID			emCurrentVoiceID;			// ��ǰ��������ID
	CFG_ACCESS_VOICE_INFO	arrayVoiceInfo[16];			// �����б�	
	UINT					nVoiceCount;				// arrayVoiceInfo ���� 
	BYTE					byReserved[1024];			// Ԥ���ֶ�
}CFG_ACCESS_VOICE;

// �Ž���������
typedef struct tagCFG_ACCESS_GENERAL_INFO
{
	char                szOpenDoorAudioPath[MAX_ADDRESS_LEN];   // ������Ƶ�ļ�·��
	char                szCloseDoorAudioPath[MAX_ADDRESS_LEN];  // ������Ƶ�ļ�·��
	char                szInUsedAuidoPath[MAX_ADDRESS_LEN];     // ������Ƶ�ļ�·��
	char                szPauseUsedAudioPath[MAX_ADDRESS_LEN];  // ��ͣʹ����Ƶ�ļ�·�� 
	char                szNotClosedAudioPath[MAX_ADDRESS_LEN];  // ��δ����Ƶ�ļ�·��
	char                szWaitingAudioPath[MAX_ADDRESS_LEN];    // �ȴ���ʾ��Ƶ�ļ�·��
	int                 nUnlockReloadTime;                      // ����������Ӧ���ʱ��,��λ�룬ȡֵ10��15(Ĭ��ֵ)��20
	int                 nUnlockHoldTime;                        // �����������ʱ��,��λ�룬ȡֵ1��2(Ĭ��)��3��4��5��6��9��15�� 

	// ����
	bool                abProjectPassword;
    bool                abAccessProperty;
    bool                abABLockInfo;
	BYTE				byReserved;

	char				szProjectPassword[MAX_PASSWORD_LEN];	// ��������
    CFG_ACCESS_PROPERTY_TYPE emAccessProperty;                  // �Ž�ͨ����˫������
    CFG_ABLOCK_INFO     stuABLockInfo;                          // AB������Ϣ
	char				szDuressPassword[MAX_PASSWORD_LEN];		// в������
	BOOL				bDuressEnable;							// в��ʹ��
    BOOL				bCustomPasswordEnable;                // �Ƿ����ø��Ի�����
    char				szCommonPassword[MAX_PASSWORD_LEN];	// ��������
    UINT                nPeakTimeSection;                   // �ݿظ߷�ʱ���, ֵΪCFG_ACCESS_TIMESCHEDULE_INFO���õ������±�
    BOOL                bPeakState;                         // �Ƿ�Ϊ�ݿظ߷���, TRUE:�߷���, FALSE:ƽ����
    UINT                nRemoteAuthTimeOut;                 // Զ��У�鳬ʱʱ��,��λ:��, Ĭ��ֵ:5�룬��Χ[1-15]
	BYTE				arrFloorPermission[64];				// ¥��Ȩ����Ϣ��ÿһ��bit��Ӧһ��¥�㣬bitֵ1��ʾ�����㣬0��ʾȨ��¥��	
	int					nFloorPermission;					// arrFloorPermission �������
    CFG_ACCESS_CONTROL_ASG   stuAccessControlASG;               // ��ͨ������բ������
	CFG_ACCESS_VOICE	stuAccessVoice;						// �Ž�������ʾ��
}CFG_ACCESS_GENERAL_INFO;

// �Ž�״̬
typedef enum CFG_ACCESS_STATE
{
	ACCESS_STATE_NORMAL,                                        // ��ͨ
	ACCESS_STATE_CLOSEALWAYS,                                   // ����
    ACCESS_STATE_OPENALWAYS,                                    // ���� 
                                                                // ��������״̬��,Opendoor������Ч.
    ACCESS_STATE_NOPERSONNC,                                    // ����״̬����
    ACCESS_STATE_NOPERSONNO                                     // ����״̬����
}CFG_ACCESS_STATE; 

// �Ž�ģʽ
typedef enum CFG_ACCESS_MODE
{
	ACCESS_MODE_HANDPROTECTED,                                  // ����ģʽ
	ACCESS_MODE_SAFEROOM,                                       // ��������ģʽ    
	ACCESS_MODE_OTHER,                                          // ����
}CFG_ACCESS_MODE;


// ��ʱ�ο���
typedef struct tagCFG_DOOROPEN_TIMESECTION_INFO
{
	CFG_TIME_PERIOD			stuTime;				// ʱ���
	CFG_DOOR_OPEN_METHOD	emDoorOpenMethod;		// ����ģʽ
}CFG_DOOROPEN_TIMESECTION_INFO;

#define MAX_DOOR_TIME_SECTION		4				// �Ž�ÿ���ʱʱ���������

// �׿�Ȩ����֤ͨ������Ž�״̬
typedef enum CFG_ACCESS_FIRSTENTER_STATUS
{
	ACCESS_FIRSTENTER_STATUS_UNKNOWN,	// δ֪״̬
	ACCESS_FIRSTENTER_STATUS_KEEPOPEN,	// KeepOpen-�׿�Ȩ����֤ͨ�����ű��ֳ���
	ACCESS_FIRSTENTER_STATUS_NORMAL		// Normal-�׿�Ȩ����֤ͨ���������û�����ˢ��(ָ�Ƶ�)��֤ͨ��
}CFG_ACCESS_FIRSTENTER_STATUS;

// �׿�������Ϣ
typedef struct tagCFG_ACCESS_FIRSTENTER_INFO
{
	BOOL							bEnable;	// ��ָ����ʱ��,ֻ��ӵ���׿�Ȩ�޵��û���֤ͨ�����������û�����ˢ��(ָ�Ƶ�)���룬TRUE ʹ�� FALSE �ر�
	CFG_ACCESS_FIRSTENTER_STATUS	emStatus;	// �׿�Ȩ����֤ͨ������Ž�״̬
	int								nTimeIndex;	// ��Ҫ�׿���֤��ʱ���, ֵΪͨ����
}CFG_ACCESS_FIRSTENTER_INFO;

// Զ�̿�����֤
typedef struct tagCFG_REMOTE_DETAIL_INFO 
{
    int                 nTimeOut;                       // ��ʱʱ��, 0��ʾ���õȴ�, ����ֵ��ʾ��ʱʱ��(��λΪ��)
    BOOL                bTimeOutDoorStatus;             // ��ʱ�����״̬, TRUE:��, FALSE:�ر�
}CFG_REMOTE_DETAIL_INFO;

// TimeOut info for handicap
typedef struct tagCFG_HANDICAP_TIMEOUT_INFO
{
    int                 nUnlockHoldInterval;            // ��������ʱ��(�Զ�����ʱ��),��λ����,[250, 60000]  
    int                 nCloseTimeout;                  // ���ų�ʱʱ��, ������ֵδ�ػᴥ����������λ��,[0,9999];0��ʾ����ⳬʱ
}CFG_HANDICAP_TIMEOUT_INFO;

// ����Զ����֤
typedef struct tagCFG_AUTO_REMOTE_CHECK_INFO
{
    BOOL                bEnable;                        // ʹ����, TRUE: ����, FALSE: �ر�
    int                 nTimeSechdule;                  // ��ӦCFG_CMD_ACCESSTIMESCHEDULE���õ�ͨ����
}CFG_AUTO_REMOTE_CHECK_INFO;

// �Ž�Э��
typedef enum tagCFG_EM_ACCESS_PROTOCOL
{
	CFG_EM_ACCESS_PROTOCOL_UNKNOWN,						// δ֪
	CFG_EM_ACCESS_PROTOCOL_LOCAL,						// ��������������
	CFG_EM_ACCESS_PROTOCOL_SERIAL,						// ����Э��
	CFG_EM_ACCESS_PROTOCOL_REMOTE,						// �Ž�udp����
}CFG_EM_ACCESS_PROTOCOL;

// ����Э���µľ���Э�鹦��
typedef enum tagCFG_EM_SERIAL_PROTOCOL_TYPE
{
	CFG_EM_SERIAL_PROTOCOL_TYPE_UNKNOWN = -1,					// δ֪
	CFG_EM_SERIAL_PROTOCOL_TYPE_UNUSED,							// δʹ��
	CFG_EM_SERIAL_PROTOCOL_TYPE_DAHUA_ACCESS_485,				// �Ž�485
	CFG_EM_SERIAL_PROTOCOL_TYPE_LADDER_CONTROL,					// �ݿ�
	CFG_EM_SERIAL_PROTOCOL_TYPE_REMOTE_READ_HEAD,				// Զ�����ͷ
}CFG_EM_SERIAL_PROTOCOL_TYPE;

// ���Ž�udp������Ϣ
typedef struct tagCFG_ACCESS_CONTROL_UDP_INFO
{
	char				szAddress[CFG_MAX_ACCESS_CONTROL_ADDRESS_LEN];			// ��ַ
	int					nPort;													// �˿�
}CFG_ACCESS_CONTROL_UDP_INFO;

// ��ǰ�Ųɼ�״̬
typedef enum tagEM_CFG_CARD_STATE
{
	EM_CFG_CARD_STATE_UNKNOWN = -1,				// δ֪
	EM_CFG_CARD_STATE_SWIPE,					// �Ž�ˢ��
	EM_CFG_CARD_STATE_COLLECTION,				// �Ž��ɼ���
}EM_CFG_CARD_STATE;

// �Ž��¼�����
typedef struct tagCFG_ACCESS_EVENT_INFO
{
	char                szChannelName[MAX_NAME_LEN];	// �Ž�ͨ������
	CFG_ACCESS_STATE    emState;						// �Ž�״̬
	CFG_ACCESS_MODE     emMode;							// �Ž�ģʽ
	int					nEnableMode;					// �Ž�ʹ�ܵ�ƽֵ, 0:�͵�ƽ��Ч(�ϵ�����); 1:�ߵ�ƽ��Ч(ͨ������);
	BOOL                bSnapshotEnable;				// �¼�����ץͼʹ��
	
    // ����
	bool                abDoorOpenMethod;
    bool                abUnlockHoldInterval;
	bool                abCloseTimeout;
	bool                abOpenAlwaysTimeIndex;
    bool                abCloseAlwaysTimeIndex;
	bool                abHolidayTimeIndex;
	bool                abBreakInAlarmEnable;
	bool				abRepeatEnterAlarmEnable;
	bool                abDoorNotClosedAlarmEnable;
	bool				abDuressAlarmEnable;
	bool                abDoorTimeSection;
	bool				abSensorEnable;
	bool				abFirstEnterEnable;
    bool                abRemoteCheck;
    bool                abRemoteDetail;
    bool                abHandicapTimeOut;
    bool                abCheckCloseSensor;
    bool                abAutoRemoteCheck;
    BYTE                reverse[2];

	CFG_DOOR_OPEN_METHOD	emDoorOpenMethod;			// ���ŷ�ʽ
	int					nUnlockHoldInterval;			// ��������ʱ��(�Զ�����ʱ��),��λ����,[250, 20000]
	int					nCloseTimeout;					// ���ų�ʱʱ��, ������ֵδ�ػᴥ����������λ��,[0,9999];0��ʾ����ⳬʱ
	int					nOpenAlwaysTimeIndex;			// ����ʱ���, ֵΪCFG_ACCESS_TIMESCHEDULE_INFO���õ������±�
    int                 nCloseAlwaysTimeIndex;          // ����ʱ���, ֵΪCFG_ACCESS_TIMESCHEDULE_INFO���õ������±�
	int					nHolidayTimeRecoNo;				// ������ʱ���, ֵΪ���ռ�¼���ļ�¼��ţ���ӦNET_RECORDSET_HOLIDAY��nRecNo
	BOOL				bBreakInAlarmEnable;			// ���뱨��ʹ��
	BOOL				bRepeatEnterAlarm;				// ��Ǳ����ʹ��
	BOOL				bDoorNotClosedAlarmEnable;		// ��δ�ر���ʹ��
	BOOL				bDuressAlarmEnable;				// в�ȱ���ʹ��
	CFG_DOOROPEN_TIMESECTION_INFO	stuDoorTimeSection[WEEK_DAY_NUM][MAX_DOOR_TIME_SECTION];// ��ʱ�ο�����Ϣ
	BOOL				bSensorEnable;					// �Ŵ�ʹ��
	CFG_ACCESS_FIRSTENTER_INFO stuFirstEnterInfo;		// �׿�������Ϣ
    BOOL                bRemoteCheck;                   // �Ƿ���Ҫƽ̨��֤, TRUE��ʾȨ��ͨ�������Ҫƽ̨��֤����ܿ���, FALSE��ʾȨ����֤ͨ�������������
    CFG_REMOTE_DETAIL_INFO  stuRemoteDetail;            // ��bRemoteCheck���ʹ��, ���Զ����֤δӦ��, �趨���豸��ʱʱ�䵽��, ���������Ż��ǲ�����
    CFG_HANDICAP_TIMEOUT_INFO   stuHandicapTimeOut;     // ��Բм��˵Ŀ��Ų���
    BOOL                bCloseCheckSensor;              // ����ǰ�Ƿ����Ŵ�
                                                        // true:�򵱿�������ʱ���ʱ������ֻ�м�⵽��Ч�Ŵ��ź�ʱ���ſ��Իָ��ر����Ķ�����
                                                        // ��֮�������������ʱ���ѵ�����δ��⵽��Ч�Ŵ��źţ���һֱ���ֿ���״̬��
                                                        // false(Ĭ��):��ֱ�Ӱ����趨�Ŀ�������ʱ����п������ֺͻָ��رյĶ�����
    CFG_AUTO_REMOTE_CHECK_INFO  stuAutoRemoteCheck;     // ����Զ����֤, �������, �ڸ�ʱ�����, �豸ͨ��������Ͽ����¼�֪ͨ��ƽ̨ȷ���Ƿ���Կ���
    BOOL                bLocalControlEnable;            // ���ؿ�������, TRUE  ����   FALSE ͣ��
    BOOL                bRemoteControlEnable;           // Զ�̿�������, TRUE  ����   FALSE ͣ��
    int                 nSensorDelay;                   // �����������ʱ��������ʱ���ж�����, ��λ���롣 0~10
    int                 nHumanStatusSensitivity;        // ��״̬�仯���������,�ڴ�ʱ���ڣ��ж����� ��λ�� �롣 0~300
    int                 nDetectSensitivity;             // ����������ļ��������  ��λ��%�� 0~100
	BOOL				bLockTongueEnable;				// ����ʹ��
	int                 nABLockRoute;                   // AB����·����AB������index��Ӧ��-1������Ч
    int                 nDoorNotClosedReaderAlarmTime;  // ��δ�س�ʱ����������, ��λ����
	BOOL				bEnable;						// ʹ����,��ͨ�������Ƿ�����,TRUEΪʹ��,FALSEΪ�ر�
	char				szSN[CFG_MAX_SN_LEN];			// �����豸���к�,ֻ��ȡ����������
	int					nCloseDuration;					// �űպ�ʱ��,��λ����
	int					nUnlockReloadInterval;			// ����������Ӧ���ʱ��,��λ:����
	CFG_EM_ACCESS_PROTOCOL		  emAccessProtocol;			// �Ž�Э��
	CFG_EM_SERIAL_PROTOCOL_TYPE	  emProtocolType;			// ����Э���µľ���Э�鹦��,��emAccessProtocolΪCFG_EM_ACCESS_PROTOCOL_SERIALʱ��Ч
	CFG_ACCESS_CONTROL_UDP_INFO	  stuAccessControlUdpInfo;  // �Ž�udp������Ϣ,��emAccessProtocolΪCFG_EM_ACCESS_PROTOCOL_REMOTEʱ��Ч
	unsigned int				  nEntranceLockChannel;		// �Ž��������µ���ͨ��,��emAccessProtocolΪCFG_EM_ACCESS_PROTOCOL_REMOTEʱ��Ч
	BOOL						  bSnapshotUpload;			// ʹ����,ץͼ�Ƿ��ϴ�,TRUEΪץͼ�ϴ�,FALSEΪץͼ���ϴ�
	unsigned int				  nSnapUploadPos;			// ץͼ�ϴ���ַ,��Ӧ����NAS��������±꣬NAS����ΪCFG_CMD_NASEX
    BOOL                bCustomPasswordEnable;              // �Ƿ����ø��Ի�����
    int                 nRepeatEnterTime;                   // �ظ�����ʱ��,0~180�룬0��ʾ������
    int                 nCardNoConvert;                     // ����ת����0:����Ҫת��,1:�ֽ�ȡ��,2:��HIDproת��
    BOOL                bUnAuthorizedMaliciousSwipEnable;   // δ��Ȩ����ˢ���¼�ʹ��
    BOOL                bFakeLockedAlarmEnable;             // ��������ʹ��
	EM_CFG_CARD_STATE	emReadCardState;					// ��ǰ�Ųɼ�״̬
	BOOL				bHelmetEnable;						// �Ƿ��ⰲȫñ
}CFG_ACCESS_EVENT_INFO;


// �Ž�ˢ��ʱ��Σ��Դ����ã�ͨ����ʵ�ʱ�ʾ��������
typedef struct tagCFG_ACCESS_TIMESCHEDULE_INFO 
{
	CFG_TIME_SECTION		stuTime[WEEK_DAY_NUM][MAX_DOOR_TIME_SECTION];	// ˢ��ʱ���
	BOOL					bEnable;										// ʱ���ʹ�ܿ���
    char                    szName[CFG_COMMON_STRING_128];                  // �Զ�������
}CFG_ACCESS_TIMESCHEDULE_INFO;

//////////////////////////////////////////////////////////////////////////
// ����������������

// ÿ���������Ʋ���
enum EM_CFG_DAY3GFLUXTACTIC
{
	EM_CFG_DAY3GFLUXTACTIC_BYFLUX = 0,	// ������
	EM_CFG_DAY3GFLUXTACTIC_BYTIME,		// ��ʱ��
};

// �������������
enum EM_CFG_APN
{
	EM_CFG_APN_CTNET = 0,				// �й�����
	EM_CFG_APN_CMNET,					// �й��ƶ�
	EM_CFG_APN_UNINET,					// �й���ͨ
};

// ������������
enum EM_CFG_DAY3GFLUXACTION
{
	EM_CFG_DAY3GFLUXACTION_NOTHING = 0,	// �޶���
	EM_CFG_DAY3GFLUXACTION_3GNETDOWN,	// 3G����
};

// ����ʹ�ò���
typedef enum tagEM_CFG_3GFLUXTACTIC
{
	EM_3GFLUXTACTIC_UNKNOWN = -1,		// δ֪����
	EM_3GFLUXTACTIC_BYFLUX,				// ���°�����
	EM_3GFLUXTACTIC_BYTIME,				// ���°�ʱ��
}EM_CFG_3GFLUXTACTIC;

// ��Ȩģʽ
typedef enum tagEM_CFG_AUTHMODE
{
	EM_AUTHMODE_NO,						// ����Ҫ��Ȩ
	EM_AUTHMODE_PAP,					// PAP��Ȩ
	EM_AUTHMODE_CHAP,					// CHAP��Ȩ
}EM_CFG_AUTHMODE;

// ����ģʽѡ��
typedef enum tagEM_CFG_WORKMODE
{
	EM_WORKMODE_UNKNOWN = -1,
	EM_WORKMODE_CDMA1X,					// "CDMA1x"
	EM_WORKMODE_EVDO,					// "EVDO"
	EM_WORKMODE_TDSCDMA,				// "TD-SCDMA"
	EM_WORKMODE_WCDMA,					// "WCDMA"
	EM_WORKMODE_EDGE,					// "EDGE"
	EM_WORKMODE_TDDLTE,					// "TDD-LTE"
	EM_WORKMODE_FDDLTE,					// "FDD-LTE"
}EM_CFG_WORKMODE;

typedef struct tagCFG_WIRELESS_INFO
{
	BOOL			        bEnable;					                // 2G����ʹ��
	int				        nKeepAlive;						            // ����ʱ��, ��λΪ�룬0��ʾһֱ���ӣ����Զ��Ͽ�
	EM_CFG_APN		        emAPN;							            // �����2G��������
	char			        szUseName[MAX_USERNAME_LEN];	            // �û���
	char			        szPassword[MAX_PASSWORD_LEN];	            // ����
	EM_CFG_DAY3GFLUXTACTIC	emDay3GFluxTactic;		                    // ÿ���������Ʋ���
	DWORD			        dwDay3GFluxUp;					            // ÿ������ʹ������, MB���߷���
	DWORD			        dwDay3GFluxUse;					            // ���յ���������, MB���߷���
    EM_CFG_DAY3GFLUXACTION	emDay3GFluxAction;		                    // ������������
	CFG_TIME_SECTION        stuTimeSection[WEEK_DAY_NUM][MAX_REC_TSECT];// ����ʱ���
	EM_CFG_AUTHMODE			emAuthMode;									// ��Ȩģʽ
	char					szAPNName[MAX_CFG_APN_NAME];				// ����������
	unsigned int			n3GFlux;									// ʵ��ʹ������, [0,65535]MB���߷���
	EM_CFG_3GFLUXTACTIC		em3GFluxTactic;								// ����ʹ�ò���
	unsigned int			n3GFluxUp;									// ����ʹ������
	EM_CFG_WORKMODE			emWorkMode;									// ����ģʽѡ��
	char					szDailNumber[MAX_CFG_DAIL_NUMBER];			// ���ź���
	BOOL					bActivate;									// �Ƿ��Ѿ�����������ż���
}CFG_WIRELESS_INFO;

#define MAX_CONNECTION_TYPE_LEN		32	// ���ӷ�ʽ�����ֳ���

// �Ǳ�ʱ������
enum emCFG_REPORTWEEKDAY
{
	emReportWeekDay_DoNotReport = -1,	// ���Ǳ�
	emReportWeekDay_Sunday,				// ÿ���ն�ʱ�Ǳ�
	emReportWeekDay_Monday,				// ÿ��һ��ʱ�Ǳ�
	emReportWeekDay_Tuesday,			// ÿ�ܶ���ʱ�Ǳ�
	emReportWeekDay_Wednesday,			// ÿ������ʱ�Ǳ�
	emReportWeekDay_Thursday,			// ÿ���Ķ�ʱ�Ǳ�
	emReportWeekDay_Friday,				// ÿ���嶨ʱ�Ǳ�
	emReportWeekDay_Saturday,			// ÿ������ʱ�Ǳ�
	emReportWeekDay_Everyday,			// ÿ�춨ʱ�Ǳ�
};

// ����������������
typedef struct tagCFG_ALARMSERVER_INFO
{
	BOOL			bEnable;								// ʹ��
	char			szProtocol[MAX_PROTOCOL_NAME_LEN];		// Э������, Ŀǰ֧��"DAHUA"
	char			szConnection[MAX_CONNECTION_TYPE_LEN];	// ���ӷ�ʽ
	char			szAddress[MAX_ADDRESS_LEN];				// IP��ַ��������
	int				nPort;									// �˿ں�
	char			szUserName[MAX_USERNAME_LEN];			// �û���
	char			szPsw[MAX_PASSWORD_LEN];				// ����
	emCFG_REPORTWEEKDAY	emReportWeekDay;					// �Ǳ���
	int				nHour;									// �Ǳ�ʱ����ʱ
	int				nMinute;								// �Ǳ�ʱ���ķ�
	int				nSecond;								// �Ǳ�ʱ������
}CFG_ALARMSERVER_INFO;

#define MAX_ALARMSERVER_NUM		8

// ��������ʹ�õı������ļ��������ĵ�����
typedef struct tagCFG_ALARMCENTER_INFO
{
	CFG_ALARMSERVER_INFO	stuMainServer;						// ��������ʹ�õı������ķ�����
	int						nBackupAlarmServerNum;				// ���ñ������ķ���������Ŀ
	CFG_ALARMSERVER_INFO	stuBackupAlarmServer[MAX_ALARMSERVER_NUM];	// ���ñ������ķ�����������
}CFG_ALARMCENTER_INFO;

//////////////////////////////////////////////////////////////////////////
// ��������������

#define	MAX_SCENE_COUNT	8		//	����龰ģʽ����

// �龰ģʽ
typedef enum tagemCFG_SCENE_MODE
{	
	emCFG_SCENE_MODE_UNKNOWN,			// δ֪ģʽ
	emCFG_SCENE_MODE_OUTDOOR,			// ���ģʽ
	emCFG_SCENE_MODE_INDOOR,			// �ڼ�ģʽ
    emCFG_SCENE_MODE_WHOLE ,            // ȫ��ģʽ
    emCFG_SCENE_MODE_RIGHTNOW,          // ����ģʽ
    emCFG_SCENE_MODE_AUTO,              // �Զ�ģʽ
    emCFG_SCENE_MODE_FORCE,             // ǿ��ģʽ
	emCFG_SCENE_MODE_SLEEPING,			// ����ģʽ
	emCFG_SCENE_MODE_CUSTOM,			// �Զ���ģʽ
}emCFG_SCENE_MODE;

typedef struct tagCFG_SCENE_INFO
{
	emCFG_SCENE_MODE emName;							// ģʽ��
	int				 nAlarmInChannelsCount;				// ����ͨ������
	int				 nRetAlarmInChannelsCount;			// ʵ�ʷ��صı���ͨ������
	int*			 pnAlarmInChannels;					// ���õı�������ͨ�����б����û������ڴ�,��СΪsizeof(int)*nAlarmInChannelsCount
}CFG_SCENE_INFO;

// ������������, ��Ӧ����(CFG_CMD_COMMGLOBAL)
// ��Ʒ�ͺŲ�ΪAS5008ʱ, ����bSceneEnable��emCurrentScene, ������nSceneCount��stuScense[MAX_SCENE_COUNT]
// ��Ʒ�ͺ�ΪAS5008ʱ, ������bSceneEnable��emCurrentScene, ����nSceneCount��stuScense[MAX_SCENE_COUNT]
typedef struct tagCFG_COMMGLOBAL_INFO
{
    BOOL                bEnable;                            // TRUE: ����; FALSE: ����; ��������̨�豸��������ͨ��
    BOOL                bSceneEnable;                       // �Ƿ������龰ģʽ
    emCFG_SCENE_MODE    emCurrentScene;                     // ��ǰѡ����龰ģʽ
    int                 nSceneCount;                        // �龰ģʽ��Ч����
    CFG_SCENE_INFO      stuScense[MAX_SCENE_COUNT];         // �龰ģʽ���壬ÿ��ģʽ��Ӧһ������
}CFG_COMMGLOBAL_INFO;

#define MAX_ALARM_LIMITS_NUM    8                                   // ������ֵ������

// ģ������������ͨ������
typedef struct tagCFG_ANALOGALARM_INFO	// =>CFG_CMD_ANALOGALARM
{
	BOOL				bEnable;									// ʹ�ܿ���(��̨�豸���ڲ����Ҹ�ͨ����ʹ��ʱΪ��·)
	char				szChnName[MAX_CHANNELNAME_LEN];				// ����ͨ������
	float				fUpperLimit;								// ��������
	float				fLowerLimit;								// ��������
	int					nSensitivity;								// ������, ��λ���ٷֱ�
	float				fCompensation;								// ����ֵ, ���ݴ��������Ͷ���
	float				fLimit1;									// ������1, ������ʹ��, ����ʹ��fAlarmLimits�ֶ�
	float				fLimit2;									// ������2, ������ʹ��, ����ʹ��fAlarmLimits�ֶ�
	float				fLimit3;									// ������3, ������ʹ��, ����ʹ��fAlarmLimits�ֶ�
	float				fLimit4;									// ������4, ������ʹ��, ����ʹ��fAlarmLimits�ֶ�
	BYTE				byMode;										// ����ģʽ, �����ʾ, ��������ֵ1111��1110��1100��1000��0000
																	// 1��ʾ����������ֵ��0��ʾ���ڱ�����ֵ
																	// ģʽ�����ҷֱ��Ӧ4��������ֵ, ���ε���
																	// ����1110��ʾ������1��2��3��������ֵ, �����ڵ�4��������ֵ
	BYTE				byReserve[3];								// �����ֽ�
	CFG_ALARM_MSG_HANDLE	stuEventHandler;						// ��������
	CFG_TIME_SECTION	stuTimeSection[WEEK_DAY_NUM][MAX_REC_TSECT];// �¼���Ӧʱ��Σ�ʱ��λ�ȡ�������Դ˳�ԱΪ׼������ stuEventHandler �е� stuTimeSection
	EM_SENSE_METHOD		emSense;									// ��������ʽ
	char				szSensorType[CFG_COMMON_STRING_64];			// ����������
    int                 nSlot;                                      // ����ַ, 0��ʾ����ͨ��, 1��ʾ�����ڵ�һ�������ϵ���չͨ��, 2��3...�Դ�����, -1��ʾ��Ч
	int                 nLevel1;                                    // ��һ��������ַ, ��ʾ�����ڵ�nSlot�����ϵĵ�nLevel1��̽�������Ǳ�, ��0��ʼ, -1��ʾ��Ч
    bool                abLevel2;                                   // ��ʾnLevel2�ֶ��Ƿ����
    int                 nLevel2;                                    // �ڶ���������ַ, ��ʾ�����ڵ�nLevel1�����Ǳ��ϵ�̽�������, ��0��ʼ
    int                 nAlamrLimits;                               // ��Ч�ı�����ֵ����
    float               fAlarmLimits[MAX_ALARM_LIMITS_NUM];         // ������ֵ
    int                 nNotifyInterval;                            // ģ�����ϴ�����, ��λ: ��
    int                 nAlarmInterval;                             // ���������޺���ϴ�����, ��λ: ��
}CFG_ANALOGALARM_INFO;

// �������ͨ����״̬������
typedef struct tagCFG_ALARMOUT_INFO		// =>CFG_CMD_ALARMOUT
{
	int					nChannelID;									// ����ͨ����(0��ʼ)
	char				szChnName[MAX_CHANNELNAME_LEN];				// ����ͨ������
	char				szOutputType[MAX_NAME_LEN];					// �������, �û��Զ���
	int					nOutputMode;								// ���ģʽ, 0-�Զ�����, 1-ǿ�Ʊ���, 2-�رձ���, 3- ����ģʽ
    int                 nPulseDelay;                                // ����ģʽ���ʱ��, ��λΪ��(0-255��)
    int                 nSlot;                                      // ����ַ, 0��ʾ����ͨ��, 1��ʾ�����ڵ�һ�������ϵ���չͨ��, 2��3...�Դ�����, -1��ʾ��Ч
    int                 nLevel1;                                    // ��һ��������ַ, ��ʾ�����ڵ�nSlot�����ϵĵ�nLevel1��̽�������Ǳ�, ��0��ʼ, -1��ʾ��Ч
    bool                abLevel2;                                   // ��ʾnLevel2�ֶ��Ƿ����
    int                 nLevel2;                                    // �ڶ���������ַ, ��ʾ�����ڵ�nLevel1�����Ǳ��ϵ�̽�������, ��0��ʼ
}CFG_ALARMOUT_INFO;


// ʱ������(NTP)
typedef enum __EM_CFG_TIME_ZONE_TYPE
{
	EM_CFG_TIME_ZONE_0,								// {0, 0*3600,"GMT+00:00"}
	EM_CFG_TIME_ZONE_1,								// {1, 1*3600,"GMT+01:00"}
	EM_CFG_TIME_ZONE_2,								// {2, 2*3600,"GMT+02:00"}
	EM_CFG_TIME_ZONE_3,								// {3, 3*3600,"GMT+03:00"}
	EM_CFG_TIME_ZONE_4,								// {4, 3*3600+1800,"GMT+03:30"}
	EM_CFG_TIME_ZONE_5,								// {5, 4*3600,"GMT+04:00"}
	EM_CFG_TIME_ZONE_6,								// {6, 4*3600+1800,"GMT+04:30"}
	EM_CFG_TIME_ZONE_7,								// {7, 5*3600,"GMT+05:00"}
	EM_CFG_TIME_ZONE_8,								// {8, 5*3600+1800,"GMT+05:30"}
	EM_CFG_TIME_ZONE_9,								// {9, 5*3600+1800+900,"GMT+05:45"}
	EM_CFG_TIME_ZONE_10,							// {10, 6*3600,"GMT+06:00"}
	EM_CFG_TIME_ZONE_11,							// {11, 6*3600+1800,"GMT+06:30"}
	EM_CFG_TIME_ZONE_12,							// {12, 7*3600,"GMT+07:00"}
	EM_CFG_TIME_ZONE_13,							// {13, 8*3600,"GMT+08:00"}
	EM_CFG_TIME_ZONE_14,							// {14, 9*3600,"GMT+09:00"}
	EM_CFG_TIME_ZONE_15,							// {15, 9*3600+1800,"GMT+09:30"}
	EM_CFG_TIME_ZONE_16,							// {16, 10*3600,"GMT+10:00"}
	EM_CFG_TIME_ZONE_17,							// {17, 11*3600,"GMT+11:00"}
	EM_CFG_TIME_ZONE_18,							// {18, 12*3600,"GMT+12:00"}
	EM_CFG_TIME_ZONE_19,							// {19, 13*3600,"GMT+13:00"}
	EM_CFG_TIME_ZONE_20,							// {20, -1*3600,"GMT-01:00"}
	EM_CFG_TIME_ZONE_21,							// {21, -2*3600,"GMT-02:00"}
	EM_CFG_TIME_ZONE_22,							// {22, -3*3600,"GMT-03:00"}
	EM_CFG_TIME_ZONE_23,							// {23, -3*3600-1800,"GMT-03:30"}
	EM_CFG_TIME_ZONE_24,							// {24, -4*3600,"GMT-04:00"}
	EM_CFG_TIME_ZONE_25,							// {25, -5*3600,"GMT-05:00"}
	EM_CFG_TIME_ZONE_26,							// {26, -6*3600,"GMT-06:00"}
	EM_CFG_TIME_ZONE_27,							// {27, -7*3600,"GMT-07:00"}
	EM_CFG_TIME_ZONE_28,							// {28, -8*3600,"GMT-08:00"}
	EM_CFG_TIME_ZONE_29,							// {29, -9*3600,"GMT-09:00"}
	EM_CFG_TIME_ZONE_30,							// {30, -10*3600,"GMT-10:00"}
	EM_CFG_TIME_ZONE_31,							// {31, -11*3600,"GMT-11:00"}
	EM_CFG_TIME_ZONE_32,							// {32, -12*3600,"GMT-12:00"}
    EM_CFG_TIME_ZONE_33,							// {33, -4*3600-1800,"GMT-4:30"}
    EM_CFG_TIME_ZONE_34,							// {34, 10.5*3600,"GMT+10:30"}
    EM_CFG_TIME_ZONE_35,							// {35, 14*3600, "GMT+14:00"}
    EM_CFG_TIME_ZONE_36,							// {36, -9*3600-1800,"GMT-09:30"}
    EM_CFG_TIME_ZONE_37,							// {37, 8*3600+1800,"GMT+08:30"}
    EM_CFG_TIME_ZONE_38,							// {38, 8*3600+2700,"GMT+08:45"}
    EM_CFG_TIME_ZONE_39,							// {39, 12*3600+2700,"GMT+12:45"}
}EM_CFG_TIME_ZONE_TYPE;

// NTP������
typedef struct tagCFG_NTP_SERVER
{
    BOOL                bEnable;
    char				szAddress[MAX_ADDRESS_LEN];					// IP��ַ��������
	int					nPort;										// �˿ں�
}CFG_NTP_SERVER;

// ʱ��ͬ������������
typedef struct tagCFG_NTP_INFO
{
	BOOL				bEnable;									// ʹ�ܿ���
	char				szAddress[MAX_ADDRESS_LEN];					// IP��ַ��������
	int					nPort;										// �˿ں�
	int					nUpdatePeriod;								// �������ڣ���λΪ����
	EM_CFG_TIME_ZONE_TYPE emTimeZoneType;							// ʱ��
	char				szTimeZoneDesc[MAX_NAME_LEN];				// ʱ������
    int                 nSandbyServerNum;                           // ʵ�ʱ���NTP����������
    CFG_NTP_SERVER      stuStandbyServer[MAX_NTP_SERVER];           // ��ѡNTP��������ַ
    int                 nTolerance;                                 // (������ʹ��)��ʾ���õ�ʱ��͵�ǰʱ����ݲ��λΪ�룬������õ�ʱ��͵�ǰ��ʱ�����ݲΧ�ڣ��򲻸��µ�ǰʱ�䡣0 ��ʾÿ�ζ��޸ġ�
}CFG_NTP_INFO;

// ��������
typedef struct tagCFG_ALARMBELL_INFO
{
    int                 nPersistentTime;                            // �����������ʱ��, ��λ: ����(0-99), 0 ��ʾ��һ�¾�ֹͣ
}CFG_ALARMBELL_INFO;

#define MAX_MSG_NUMBER_LEN	32				// ���ź������󳤶�
#define MAX_RECEIVER_NUM	100				// ���ն����û��������Ŀ

// ��������
typedef enum tagEM_MSG_TYPE
{
	EM_MSG_UNKNOWN = 0,												// δ֪����
	EM_MSG_SMS,														// SMS����
	EM_MSG_MMS,														// MMS����
}EM_MSG_TYPE;

// ϵͳ�¼���������/���ŷ��͵�����
typedef struct tagCFG_EVENT_MSG_SENDING_INFO
{
	BOOL				bEnable;									// ʹ�ܿ���
	EM_MSG_TYPE			emMsgType;									// ���͵���Ϣ������
	char				szTitle[MAX_NAME_LEN];						// ����, Ҳ���Ƕ���/���ŵĵ�һ������
	DWORD				dwReceiverCount;							// �����˺���ĸ���
	char				szReceiverNumbersList[MAX_RECEIVER_NUM][MAX_MSG_NUMBER_LEN];// �����˺���
}CFG_EVENT_MSG_SENDING_INFO;

// �ƶ����ҵ������
typedef struct tagCFG_MOBILE_INFO
{
	CFG_EVENT_MSG_SENDING_INFO	stuEventMsgSending;					// ϵͳʱ�䴥���ķ��Ͷ���/���ŵ�����
}CFG_MOBILE_INFO;

#define MAX_CONTACT_NUM		100				// ��ϵ�������Ŀ

// �绰������������
typedef struct tagCFG_PHONEEVENTNOTIFY_INFO
{
	BOOL				bEnable;									// ʹ�ܿ���
	DWORD				dwContactCount;								// ���к���ĸ���
	char				szContactNumbersList[MAX_CONTACT_NUM][MAX_MSG_NUMBER_LEN];// ���к����б�
}CFG_PHONEEVENTNOTIFY_INFO;

// �绰������������
enum CFG_EM_SIGNALTRANSMIT_MODE
{
	CFG_EM_SIGNALTRANSMIT_ERROR = 0,								// δ֪ģʽ
	CFG_EM_SIGNALTRANSMIT_DTMF_5S,									// "DTMF 5/S" - DTMFģʽ����
	CFG_EM_SIGNALTRANSMIT_DTMF_10S,									// "DTMF 10/S" - DTMFģʽ�첦
};
enum CFG_EM_PSTN_PROTOCOL_TYPE
{
	CFG_EM_PSTN_PROTOCOL_ERROR = 0,									// δ֪����
	CFG_EM_PSTN_PROTOCOL_CID,										// "CID" - Contact ID Protocol
};
typedef struct tagCFG_PSTN_ALARM_SERVER_INFO
{
	char							szName[MAX_NAME_LEN];						// �绰������������
	char							szNumber[MAX_PHONE_NUMBER_LEN];				// �绰�������Ľ��ջ�����
																				// "057188888888PPP888",��ʽΪ[����][ͣ��ʱ��][�ֻ�����]������P��ʾͣ��2�롣
	CFG_EM_PSTN_PROTOCOL_TYPE		emProtocol;									// Э�����ͣ�
	CFG_EM_SIGNALTRANSMIT_MODE		emMode;										// �źŴ���ģʽ																															
	int								nTryCount;									// ���ų��Դ���
	int								nDelay;										// ������ʱ����λ����
	char							szAccount[MAX_PHONE_NUMBER_LEN];			// �û�����
}CFG_PSTN_ALARM_SERVER_INFO;

// �¼��ϱ����ȼ�
typedef enum tagCFG_EM_PSTN_ALARM_CENTER_RULE
{
	CFG_EM_PSTN_ALARM_CENTER_UNKNOWN,		// δ֪
	CFG_EM_PSTN_ALARM_CENTER_NONE,			// ���ϱ�
	CFG_EM_PSTN_ALARM_CENTER_REGULARTEL,	// �绰����1
	CFG_EM_PSTN_ALARM_CENTER_SPLITTEL,		// �ȵ绰����1��ʧ�ܵ绰����2
	CFG_EM_PSTN_ALARM_CENTER_DOUBLETEL,		// �绰����1��2ͬʱ����
	CFG_EM_PSTN_ALARM_CENTER_REGULARIP,		// ��·����1
	CFG_EM_PSTN_ALARM_CENTER_SPLITIP,		// ����������1��ʧ����������2
	CFG_EM_PSTN_ALARM_CENTER_DOUBLEIP,		// ��������1��2ͬʱ�ϱ�
	CFG_EM_PSTN_ALARM_CENTER_DOUBLEMIX,		// ��������1�͵绰����1
	CFG_EM_PSTN_ALARM_CENTER_IPTOTEL,		// ����������1��ʧ�ܵ绰����1
	CFG_EM_PSTN_ALARM_CENTER_TELTOIP		// �ȵ绰���ģ�ʧ����������
}CFG_EM_PSTN_ALARM_CENTER_RULE;
typedef struct tagCFG_PSTN_ALARM_CENTER_INFO
{
	BOOL						bEnable;								// ʹ�ܿ���
	int							nServerCount;							// �绰��������������
	CFG_PSTN_ALARM_SERVER_INFO	stuPSTNAlarmServer[MAX_PSTN_SERVER_NUM];		// �绰����������
	CFG_EM_PSTN_ALARM_CENTER_RULE	emRule;								// �¼��ϱ����ȼ�
}CFG_PSTN_ALARM_CENTER_INFO;

// ������������
typedef struct tagCFG_AUDIO_INPUT_VOLUME
{
	int				nAudioInputCount;									// ʵ����Ƶ����ͨ������
	char			szAudioInputVolume[MAX_AUDIO_INPUT_NUM];			// ÿ��Ԫ�ض�Ӧһ����Ƶ����ͨ��������ֵ,��Χ[0, 100]
}CFG_AUDIO_INPUT_VOLUME;


// �����������
typedef struct tagCFG_AUDIO_OUTPUT_VOLUME
{
	int				nAudioOutputCount;									// ʵ����Ƶ���ͨ������
	char			szAudioOutputVolume[MAX_AUDIO_OUTPUT_NUM];			// ÿ��Ԫ�ض�Ӧһ����Ƶ���ͨ��������ֵ,��Χ[0, 100]
}CFG_AUDIO_OUTPUT_VOLUME;

//ָʾ�ƿ�������
typedef struct tagCFG_LIGHT_GLOBAL
{
	int				nLightGlobalCount;									// ָʾ������
	BOOL			bLightEnable[MAX_LIGHT_GLOBAL_NUM];					// ָʾ�ƿ���״̬����
}CFG_LIGHT_GLOBAL;

// �����Ƶ��������
typedef struct tagCFG_AUDIO_MIX_VOLUME
{
	int				nAudioMixCount;									// ʵ�ʻ����Ƶͨ������
	char			szAudioMixVolume[MAX_AUDIO_MIX_NUM];			// ÿ��Ԫ�ض�Ӧһ��ר�õ���Ƶ����ͨ��������ֵ,��Χ[0, 100]
}CFG_AUDIO_MIX_VOLUME;



// ������������
typedef struct tagCFG_ALARMKEYBOARD_INFO
{
	BOOL			bEnable;								// ʹ�ܿ���
	char			szProtocolName[MAX_PROTOCOL_NAME_LEN];	// Э������
	int				nPort;									// ʹ�õĴ��ڶ˿ں�
	int				nAddress;								// �豸��ַ����������Ϲ��˶�������豸��ͨ�������ַ����
	CFG_COMM_PROP	stuCommAttr;							// ��������
}CFG_ALARMKEYBOARD_INFO;

// ��ȡ�ļ���������
typedef struct tagCFG_CAP_FILEMANAGER
{
	int			nMaxUploadFileSize;		// �����ϴ��ļ��Ĵ�С, ��λ: �ֽ�
}CFG_CAP_FILEMANAGER;

// ¼����ʱ������Ϣ
typedef struct tagCFG_RECORD_LATCH
{
	BOOL			bEnable;				// ��ʾ���޴��true��ʾ�д��false��ʾ�޴���
	int 			nRecordLatchMin;		// ¼����ʱ��Сֵ
	int				nRecordLatchMax;		// ¼����ʱ���ֵ
} CFG_RECORD_LATCH;

// ���������ʱ������Ϣ
typedef struct tagCFG_ALARMOUT_LATCH
{
	BOOL			bEnable;				// ��ʾ���޴��true��ʾ�д��false��ʾ�޴���
	int				nAlarmOutLatchMin;		// ���������ʱ��Сֵ
	int				nAlarmOutLatchMax;		// ���������ʱ���ֵ
} CFG_ALARMOUT_LATCH;

// ȥ����������Ϣ
typedef struct tagCFG_DEJITTER_RANGE
{
	BOOL			bEnable;				// ��ʾ���޴��true��ʾ�д��false��ʾ�޴���
	int				nDejitterMin;			// ȥ������С����ֵ
	int				nDejitterMax;			// ȥ�����������ֵ
} CFG_DEJITTER_RANGE;

// ��ȡ�豸������������
typedef struct tagCFG_CAP_EVENTMANAGER_INFO
{
	BOOL				bTimeSectionEnable;                               // �Ƿ�֧��ʱ���¼���Ӧ
	BOOL				bRecordEnable;                                    // �Ƿ�֧��¼��
	BOOL				bAlarmOutEnable;                                  // �Ƿ�֧�ֱ������
	BOOL				bPtzLinkEnable;                                   // �Ƿ�֧����̨����
	BOOL				bSnapshotEnable;                                  // �Ƿ�֧�ֿ���
	BOOL				bMailEnable;                                      // �Ƿ�֧�ַ����ʼ�
	BOOL				bBeepEnable;                                      // �Ƿ�֧�ַ���
	BOOL				bDejitterEnable;                                  // �Ƿ�֧��ȥ����
	BOOL				bTipEnable;										  // �Ƿ�֧�ֱ�����Ϣ����ʾ
	BOOL				bMonitorTourEnable;								  // �Ƿ�֧����ѯ��������
	BOOL				bMMSEnable;										  // �Ƿ�֧�ֶ���Ϣ
	BOOL				bSupportAlarmBell;								  // �Ƿ�֧�־������
	BOOL				bSupportAccessControl;							  // �Ƿ�֧���Ž�����
	BOOL				bSupportAlarmServer;							  // �Ƿ�֧���ϴ�������������
	BOOL				bSupportPtzLinkDelay;							  // �Ƿ�֧����̨�����ӳ�
	BOOL				bSupportPSTNAlarmServer;						  // �Ƿ�֧���ϴ����绰��������
    BOOL                bVoiceEnable;                                     // �Ƿ�֧������������ʾ
	BOOL				bLogEnable;										  // �Ƿ�֧��������־
	CFG_RECORD_LATCH	stuRecordLatch;									  // ¼����ʱ��Ϣ
	CFG_ALARMOUT_LATCH	stuAlarmOutLatch;								  // ���������ʱ��Ϣ
	CFG_DEJITTER_RANGE	stuDejitterRange;								  // ȥ������Ϣ
	DWORD				nSupportDisableLinkage;							  // ֧����Щ������һ������
																		  // bit0��ʾ֧�ֳ�������
																		  // bit1��ʾ֧�ֳ���������ʾ
																		  // bit2��ʾ֧�ֳ����������
																		  // bit3��ʾ֧�ֳ����ʼ�
																		  // bit4��ʾ֧�ֳ��������ϴ�
}CFG_CAP_EVENTMANAGER_INFO;

// ��Դ��������
typedef struct tagCFG_POWERFAULT_ONE
{
	BOOL			bEnable;									// ʹ�ܿ���
	CFG_ALARM_MSG_HANDLE stuEventHandler;						// ��������
    int             nDetectionPeriod;                           // ��Դ���ϼ������ ��λ��
    BOOL            bEncodeBlend;                               // �Ƿ����OSD����ͼ��    
    CFG_RECT        stuPosition;                                // λ��
}CFG_POWERFAULT_ONE;

typedef struct tagCFG_POWERFAULT_INFO
{
	int				nPowerCount;								// ��Դ����
	CFG_POWERFAULT_ONE stuPowerFault[MAX_POWER_NUM];			// ��������
}CFG_POWERFAULT_INFO;

// �������ֱ���(���𱨾�)����
typedef struct tagCFG_CHASSISINTRUSION_INFO
{
	BOOL			bEnable;									// ʹ�ܿ���
	CFG_ALARM_MSG_HANDLE stuEventHandler;						// ��������
}CFG_CHASSISINTRUSION_INFO;

// �����¼�����
typedef struct tagCFG_URGENCY_INFO 
{
	BOOL			bEnable;									// ʹ�ܿ���
	CFG_ALARM_MSG_HANDLE stuEventHandler;						// ��������
}CFG_URGENCY_INFO;

/************************************************************************
** ��չ����ģ��
***********************************************************************/
// ��չģ�鱨����������
typedef struct tagCFG_EXALARMINPUT_INFO
{
	CFG_ALARMIN_INFO		stuAlarmIn;						// ���������������� CFG_ALARMIN_INFO
	
}CFG_EXALARMINPUT_INFO;

// ��չģ�鱨���������
typedef struct tagCFG_EXALARMOUTPUT_INFO
{
	char		szChnName[MAX_CHANNELNAME_LEN];				// ����ͨ������
	int			nOutputMode;								// ���ģʽ, 0-�Զ�����, 1-ǿ�Ʊ���, 2-�رձ���
}CFG_EXALARMOUTPUT_INFO;

// ��չģ�鱨��������
typedef struct tagCFG_EXALARMBOX_INFO
{
	BOOL			bEnable;								// ʹ�ܿ���
	char			szProtocolName[MAX_PROTOCOL_NAME_LEN];	// Э������
	int				nPort;									// ʹ�õĴ��ڶ˿ں�
	int				nAddress;								// �豸��ַ
	CFG_COMM_PROP	stuCommAttr;							// ��������
}CFG_EXALARMBOX_INFO;

// ��չ������Э������
enum CFG_EM_EXALARM_PROTOCOL_TYPE
{
	CFG_EM_EXALARM_PROTOCOL_ERROR,								// δ֪����
	CFG_EM_EXALARM_PROTOCOL_DH_ALARMBOX,						// DH_AlarmBox �󻪱�����Э��
};


// ��չģ�鱨��������
typedef struct tagCFG_CAP_EXALARM_INFO
{	
	int					 nAlarmInCount;											// ��չ����ģ���������
	int					 nAlarmOutCount;										// ��չ����ģ���ٳ�����
	int				 	 nMaxDelay;												// ��ʱ�����������ʱʱ��,��λΪ�룬ֻ��֧����ʱ����ʱ���ֶβ���Ч��
	int					 nDefenceAreaTypeNum;									// ��������������,Ϊ0��ʾ�޴��ֶ�ʱ�������벻��Ϊ����ʹ�á�
	EM_CFG_DEFENCEAREATYPE	emDefenceAreaType[MAX_ALARM_DEFENCE_TYPE_NUM];		// ֧�ַ������� 
	int					 nExAlarmChannelNum;									// ��չģ�鱨��ͨ����,Ϊ�˱��ּ�����,��Ϊ0ʱ��ʾÿ��ͨ��֧���������͵Ĵ�����
	CFG_EXALARM_SENSE_METHOD stuExAlarmChannel[MAX_EXALARM_CHANNEL_NUM];		// ����ͨ������
	int					 nProtocolNum;											// ֧�ֵ�Э������
	CFG_EM_EXALARM_PROTOCOL_TYPE emProtocolType[MAX_EXALARMBOX_PROTOCOL_NUM];	// ��չ����ģ�鱨����֧�ֵ�Э������	
}CAP_EXALARM_INFO;

// ��չģ�鱨����������
typedef struct tagCFG_CAP_EXALARMBOX_INFO 
{
	int nAlarmInCount; // ��չ����ģ���������
	int nAlarmOutCount; // ��չ����ģ��������� 
}CFG_CAP_EXALARMBOX_INFO; 

// ��ѯ��¼������������
typedef struct tagCFG_CAP_RECORDFINDER_INFO 
{
	int nMaxPageSize; // ����ҳ���� 
}CFG_CAP_RECORDFINDER_INFO;


// ���������� ==>CFG_CMD_SENSORSAMPLING
typedef struct  tagCFG_SENSORSAMPLING_INFO 
{
	int			nDetectionPeriod;			// �������, ��λ���룬���������ʵʱ�ϴ��¶�״̬
	int			nStorageItem;				// �洢��Ϣ��, ��λ���������豸������д�ļ���ʽ���ɵ�����0~5000��
}CFG_SENSORSAMPLING_INFO;

// ���� ==>CFG_CMD_STP
typedef struct tagCFG_STP_INFO 
{
	BOOL			bEnable;				// ʹ��
}CFG_STP_INFO;

#define DH_MAX_ZONE_NUM					(256)	// ��������Ŀ
#define DH_MAX_PUBLIC_SUBSYSTEM_NUM		(256)    // ��󹫹���ϵͳ��Ŀ

typedef struct tagCFG_ALARM_SUBSYSTEM_MSG_HANDLE
{
    BOOL          bAlarmOutEnable;             // �������ʹ��
    BOOL          bAlarmBellEnable;            // �������ʹ��
    int           nAlarmOutChannelNum;         // �������ͨ���Ÿ���
    int           nAlarmOutChannels[256];      // �������ͨ�����б�
}CFG_ALARM_SUBSYSTEM_MSG_HANDLE;

// ������ϵͳ���� ==>CFG_CMD_ALARM_SUBSYSTEM
typedef struct tagCFG_ALARM_SUBSYSTEM_INFO 
{
	char		szName[CFG_COMMON_STRING_128];	// ����
	int			nZoneNum;						// ���ط�����Ŀ
	int			anZone[DH_MAX_ZONE_NUM];		// ���ط�����
	int			nExZoneNum;						// ��չ������Ŀ
	int			anExZone[DH_MAX_ZONE_NUM];		// ��չ������
	int			nDisableDelay;					// ��ʱ����ʱ�䣨������ʱ��, ��λΪ��
	int			nEnableDelay;					// ��ʱ����ʱ�䣨�˳���ʱ��, ��λΪ��
	BOOL		bIsPublic;						// �Ƿ�Ϊ������ϵͳ
	int			nPublicSubSystem;				// ������������ϵͳ��Ŀ
	int			anPublicSubSystem[DH_MAX_PUBLIC_SUBSYSTEM_NUM];// ���������Ĺ�����ϵͳ
    CFG_ALARM_SUBSYSTEM_MSG_HANDLE stuEventHandler;       // ��������
}CFG_ALARM_SUBSYSTEM_INFO;

// ��ص�ѹ������ ==>CFG_CMD_BATTERY_LOW_POWER
typedef struct tagCFG_BATTERY_LOW_POWER_INFO 
{
	BOOL		bEnable;						// ʹ��
	CFG_ALARM_MSG_HANDLE stuEventHandler;		// ��������
}CFG_BATTERY_LOW_POWER_INFO;


// ץͼͨ�������������� ==>CFG_CMD_SNAPLINKAGE
typedef struct tagCFG_SNAPLINKAGE_INFO 
{
    int         nChannelNum;                    // ��Ƶͨ��������ģ����ͨ����Ŀ
    int         anAnalogAlarm[DH_MAX_ZONE_NUM]; // ��Ƶͨ��������ģ��ͨ����
}CFG_SNAPLINKAGE_INFO;

// ��Ƶ��������
typedef struct tagCFG_AUDIO_INPUT 
{
    char        szAudioSource[CFG_COMMON_STRING_256];   // ������ƵԴ. �����Ƶͨ�����������ɶ�·�ϳɣ���|�ָ�.
                                                        // ����"Mic|LineIn|Remote" ��ʾ����Ƶͨ����Mic��LineIn��Զ��ͨ������Ƶ���빹��.
                                                        // "Coaxial" ͬ�����Ƶ
                                                        // "BNC" ����BNC����Ƶ
                                                        // "HDCVI_BNC" Զ��HDCVI�豸��Ƶ
                                                        // "LineIn" ��������
                                                        // "Mic" ��˷�����
                                                        // "MicOut" ������
                                                        // "Remote" Զ��ͨ��(���Ի��л�ͨ�������壬��ʾ���л�������ΪԶ��ͨ��ʱ������ǰԶ��ͨ������Ƶ��Ϊ��Ƶ����)
}CFG_AUDIO_INPUT;

// �ʼ��ϱ��豸״��
typedef struct tagCFG_HEALTHREPORT_INFO 
{
    BOOL			bEnable;				// ʹ��
    int             nInterval;              // �ʼ����ͼ��,��λΪ��,��Χ��0~3600
}CFG_HEALTHREPORT_INFO;

// �ʼ��������� ==>CFG_CMD_EMAIL
typedef struct tagCFG_EMAIL_INFO 
{
    BOOL			bEnable;				                            // ʹ��
    BOOL            bSslEnable;                                         // �Ƿ���ҪSSL����
    BOOL            bTlsEnable;                                         // �Ƿ���ҪTLS����
    char            szAddress[MAX_ADDRESS_LEN];                         // SMTP������IP��ַ��������
    int             nPort;                                              // �˿ں�
    char            szUserName[MAX_USERNAME_LEN];                       // �ʻ���
    char            szPassword[MAX_PASSWORD_LEN];                       // ����
    BOOL            bAnonymous;                                         // �����ʼ���true������false������
    char            szSendAddress[MAX_ADDRESS_LEN];                     // �����˵�ַ
    int             nRetReciversNum;                                    // ��Ч�ռ�����
    char            szReceivers[MAX_RECEIVER_NUM][MAX_ADDRESS_LEN];     // �ռ����б�
    char            szTitle[MAX_MAILTITLE_LEN];                         // �ʼ�����ؼ���
    BOOL            bOnlyAttachment;                                    // �¼������ʼ���ֻ���ʹ��������ʼ�
    BOOL            bAttachEnable;                                      // �ʼ�����ʹ��
    int             nSendInterv;                                        // ͬһ�¼�����С�ʼ����ͼ������λΪ��
    
    bool            abHealthReport;                                     // ����,Ϊtrueʱ,stuHealthReport��Ч
    BYTE            byReserved[3];
    CFG_HEALTHREPORT_INFO   stuHealthReport;                            // �ʼ��ϱ��豸״��
	BOOL				bAuthentication;								// �Ƿ����ʼ���Ȩ,TRUE��ʾ��Ҫ���û����������ȵ��뵽SMTP������;FALSE��ʾ����Ҫ�û��������룬ֱ��ͨ��SMTP������ת�������ʼ�
}CFG_EMAIL_INFO;

#define MAX_TRANSFER_SERVER_NUM          10                             // ��������������
// ���������ļ�����
typedef struct tagTRAFFIC_TRANSFER_OFFLINE_INFO
{
    BOOL            bEnable;                                                // ʹ��
    int             nType;                                                  // 0: ʹ��IP��ַ; 1:ʹ��MAC��ַ
    int             nIpAddrNum;                                             // ���ص�IP��ַ����
    char            szIpAddr[MAX_TRANSFER_SERVER_NUM][MAX_ADDRESS_LEN];     // IP��ַ��������
    int             nMacAddrNum;                                            // ���ص�mac��ַ����
    char            szClientID[MAX_TRANSFER_SERVER_NUM][MAX_ADDRESS_LEN];   // mac��ַ

}TRAFFIC_TRANSFER_OFFLINE_INFO;

#define MAX_DEVCOMM_NUM          16                                     // ��󴮿ڸ���
// ���Ĵ�����������-������������
typedef struct tagCFG_DEVCOMM_SUBSCRIBE_INFO
{
    int             nReadCycle;                                         // ���ڶ�ȡ���,��λ: ��    
}CFG_DEVCOMM_SUBSCRIBE_INFO;

// ���Ĵ�����������
typedef struct tagCFG_DEVCOMM_SUBSCRIBE
{
    int                             nSubscribeInfoNum;                  // �����������ø���
    CFG_DEVCOMM_SUBSCRIBE_INFO      stuSubscribeInfo[MAX_DEVCOMM_NUM];  // ���Ĵ����������ã���һ�����飬ÿ��Ԫ�ض�Ӧһ������    
}CFG_DEVCOMM_SUBSCRIBE;

// ��λ״̬��Ӧ�ĳ�λָʾ��
typedef enum tagEM_CFG_PARKINGSPACE_LIGHT_COLOR
{
    EM_CFG_PARKINGSPACE_LIGHT_RED,                                      // ��ɫ
    EM_CFG_PARKINGSPACE_LIGHT_YELLOW,                                   // ��ɫ
    EM_CFG_PARKINGSPACE_LIGHT_BLUE,                                     // ��ɫ 
    EM_CFG_PARKINGSPACE_LIGHT_GREEN,                                    // ��ɫ
    EM_CFG_PARKINGSPACE_LIGHT_PURPLE,                                   // ��ɫ
    EM_CFG_PARKINGSPACE_LIGHT_WHITE,                                    // ��ɫ
    EM_CFG_PARKINGSPACE_LIGHT_PINK,                                     // ��ɫ
}EM_CFG_PARKINGSPACE_LIGHT_COLOR;

// ָʾ��״̬
typedef enum tagEM_CFG_PARKINGSPACE_LIGHT_STATE
{
    EM_CFG_PARKINGSPACE_LIGHT_OFF,                                      // ��
    EM_CFG_PARKINGSPACE_LIGHT_ON,                                       // ��
    EM_CFG_PARKINGSPACE_LIGHT_GLINT,                                    // ��˸
}EM_CFG_PARKINGSPACE_LIGHT_STATE;

#define CFG_MAX_PARKINGSPACE_LIGHT_NUM      8
#define CFG_MAX_NET_PORT_NUM                4 
typedef struct tagCFG_PARKINGSPACE_LIGHT_STATE
{
    BYTE                            bySpaceFreeLinght[CFG_MAX_PARKINGSPACE_LIGHT_NUM];      // ��λ����״̬��ɫ,��ɫö��ֵ��Ϊ�����±꣬����Ԫ��ֵ��ʾָʾ��״̬����bySpaceFreeLinght[0]=1,��ʾ��ɫָʾ����
    BYTE                            bySpaceFullLinght[CFG_MAX_PARKINGSPACE_LIGHT_NUM];      // ��λ��״̬��ɫ����ɫö��ֵ��Ϊ�����±꣬����Ԫ��ֵ��ʾָʾ��״̬����bySpaceFullLinght[1]=1,��ʾ��ɫָʾ����
    BYTE                            bySpaceOverLineLight[CFG_MAX_PARKINGSPACE_LIGHT_NUM];   // ��λѹ��״̬��ɫ,��ɫö��ֵ��Ϊ�����±꣬����Ԫ��ֵ��ʾָʾ��״̬
    BYTE                            bySpaceOrderLight[CFG_MAX_PARKINGSPACE_LIGHT_NUM];      // ��λԤ��״̬��ɫ,��ɫö��ֵ��Ϊ�����±꣬����Ԫ��ֵ��ʾָʾ��״̬
    int                             nNetPortNum;                                            // ������
    BYTE                            byNetPortAbortLight[CFG_MAX_NET_PORT_NUM][CFG_MAX_PARKINGSPACE_LIGHT_NUM];    // ���ڶϿ�״̬��ɫ
    BYTE                            bySpaceSpecialLight[CFG_MAX_PARKINGSPACE_LIGHT_NUM];    // ��λר��״̬��ɫ,��ɫö��ֵ��Ϊ�����±�����Ԫ��ֵ��ʾָʾ��״̬
}CFG_PARKINGSPACE_LIGHT_STATE;

// �յ�����ģʽ
typedef enum tagEM_CFG_AIRCONDITION_MODE
{
    EM_CFG_AIRCONDITION_MODE_UNKNOWN = 0,
    EM_CFG_AIRCONDITION_MODE_AUTO,             // �Զ�
    EM_CFG_AIRCONDITION_MODE_HOT,              // ����
    EM_CFG_AIRCONDITION_MODE_COLD,             // ����
    EM_CFG_AIRCONDITION_MODE_WET,              // ��ʪ
    EM_CFG_AIRCONDITION_MODE_WIND,             // ͨ��
} EM_CFG_AIRCONDITION_MODE;

// �յ��ͷ�ģʽ
typedef enum tagEM_CFG_AIRCONDITION_WINDMODE
{
    EM_CFG_AIRCONDITION_WINDMODE_UNKNOWN = 0,
    EM_CFG_AIRCONDITION_WINDMODE_STOP,            // ֹͣ
    EM_CFG_AIRCONDITION_WINDMODE_AUTO,            // �Զ�
    EM_CFG_AIRCONDITION_WINDMODE_HIGH,            // ����
    EM_CFG_AIRCONDITION_WINDMODE_MIDDLE,          // ����
    EM_CFG_AIRCONDITION_WINDMODE_LOW,             // ����
} EM_CFG_AIRCONDITION_WINDMODE;

// ���ڵ�ַ
typedef struct tagCFG_COMMADDR_INFO
{
    int                 nAddressNum;                            // ���ڵ�ַ����
    int 				nAddress[MAX_ADDRESS_NUM];              // ��ַ����,��ͬ���̵�ַλ��ͬ���������ʾ
}CFG_COMMADDR_INFO;

// �յ��豸��������
typedef struct tagCFG_AIRCONDITION_DETAIL
{
    char				szDeviceID[MAX_DEVICE_ID_LEN];			// �豸����,Ωһ��ʶ��
    char                szName[MAX_DEVICE_MARK_LEN];            // �豸����
    char                szBrand[MAX_BRAND_NAME_LEN];            // �豸Ʒ��
    CFG_COMMADDR_INFO   stuCommAddr;                            // ���ڵ�ַ
    int                 nState;                                 // �豸״̬: 1-��,0-�ر�
    int                 nRange;                                 // ����ֵ(�¶�)��λ�����϶�
    EM_CFG_AIRCONDITION_MODE        emAirconditionMode;         // �յ�����ģʽ
    EM_CFG_AIRCONDITION_WINDMODE    emAirconditionWindMode;     // �յ��ͷ�ģʽ
}CFG_AIRCONDITION_DETAIL;

// �յ��豸����(��ӦCFG_CMD_AIRCONDITION����)
typedef struct tagCFG_AIRCONDITION_INFO
{
    int                         nAirConditionNum;                        // �յ��豸����
    CFG_AIRCONDITION_DETAIL 	stuAirConditions[MAX_AIRCONDITION_NUM];	 // �յ��豸��������, �����ʾ
}CFG_AIRCONDITION_INFO;

// �ƹ��豸����
typedef enum tagEM_LIGHT_TYPE
{
    EM_LIGHT_TYPE_UNKNOWN,                                      // δ֪����
    EM_LIGHT_TYPE_COMMLIGHT,                                    // ��ͨ�ƹ�
    EM_LIGHT_TYPE_LEVELLIGHT,                                   // �ɵ���
}EM_LIGHT_TYPE;

// �ƹ��豸������Ϣ (��Ӧ CFG_CMD_LIGHT )
typedef struct tagCFG_LIGHT_INFO
{
    char                szDeviceID[MAX_DEVICE_ID_LEN];          // �豸����,Ωһ��ʶ��
    char                szName[MAX_DEVICE_MARK_LEN];            // �豸����
    char                szBrand[MAX_BRAND_NAME_LEN];            // �豸Ʒ��
    CFG_COMMADDR_INFO   stuCommAddr;                            // ���ڵ�ַ
    int                 nPositionID;                            // �豸�������б��
    CFG_POLYGON         stuPosition;                            // ����
    int                 nState;                                 // �豸״̬: 1-��,0-�ر�
    int                 nRange;                                 // �����ȷ���ֵ 0-7 , emType Ϊ EM_LIGHT_TYPE_ADJUSTABLE ������
    EM_LIGHT_TYPE       emType;                                 // �ƹ��豸����
}CFG_LIGHT_INFO;

// �����豸������Ϣ (��Ӧ CFG_CMD_CURTAIN )
typedef struct tagCFG_CURTAIN_INFO
{
    char                szDeviceID[MAX_DEVICE_ID_LEN];          // �豸����,Ωһ��ʶ��
    char                szName[MAX_DEVICE_MARK_LEN];            // �豸����
    char                szBrand[MAX_BRAND_NAME_LEN];            // �豸Ʒ��
    CFG_COMMADDR_INFO   stuCommAddr;                            // ���ڵ�ַ
    int                 nPositionID;                            // �豸�������б��
    CFG_POLYGON         stuPosition;                            // ����
    int                 nState;                                 // �豸״̬: 1-��,0-�ر� 
}CFG_CURTAIN_INFO;

// �·�������Ϣ (��Ӧ CFG_CMD_FRESH_AIR )
typedef struct tagCFG_FRESH_AIR_INFO
{
    char                szDeviceID[MAX_DEVICE_ID_LEN];          // �豸����,Ωһ��ʶ��
    char                szName[MAX_DEVICE_MARK_LEN];            // �豸����
    char                szBrand[MAX_BRAND_NAME_LEN];            // �豸Ʒ��
    CFG_COMMADDR_INFO   stuCommAddr;                            // ���ڵ�ַ   
}CFG_FRESH_AIR_INFO;

// ��ů������Ϣ (��Ӧ CFG_CMD_GROUND_HEAT)
typedef struct tagCFG_GROUND_HEAT_INFO
{
    char                szDeviceID[MAX_DEVICE_ID_LEN];          // �豸����,Ωһ��ʶ��
    char                szName[MAX_DEVICE_MARK_LEN];            // �豸����
    char                szBrand[MAX_BRAND_NAME_LEN];            // �豸Ʒ��
    CFG_COMMADDR_INFO   stuCommAddr;                            // ���ڵ�ַ   
    int                 nState;                                 // �豸״̬: 1-��,0-�ر�   
    int                 nRange;                                 // ����ֵ(�¶�)��λ�����϶�
}CFG_GROUND_HEAT_INFO;

// ���ܼҾ��龰ģʽ
typedef enum tagEM_SMARTHOME_SCENE_MODE
{
    EM_SMARTHOME_SCENE_MODE_UNKNOWN,                            // δ֪
    EM_SMARTHOME_SCENE_MODE_AT_HOME,                            // �ڼ�
    EM_SMARTHOME_SCENE_MODE_LEAVE_HOME,                         // �뿪
    EM_SMARTHOME_SCENE_MODE_IN_SLEEPING,                        // ˯��
}EM_SMARTHOME_SCENE_MODE;

// �龰��ϸ��Ϣ
typedef struct tagCFG_PROFILE_INFO
{
    int                 nSceneID;                               // �龰ID
    char                szBrand[MAX_BRAND_NAME_LEN];            // ��������
    EM_SMARTHOME_SCENE_MODE emScene;                            // �龰ģʽ
    CFG_COMMADDR_INFO   stuCommAddr;                            // ���ڵ�ַ
}CFG_PROFILE_INFO;

#define MAX_SMARTHOME_PROFILE_COUNT     54                      // ���ܼҾ�����龰ģʽ����

// �龰ģʽ���� (��Ӧ CFG_CMD_SCENE_MODE)
typedef struct tagCFG_SCENE_MODE_INFO
{
    int                nCurrentID;                              // ��ǰ�龰ģʽID��
    int                nProfileCount;                           // �龰ģʽ����
    CFG_PROFILE_INFO   stuProfiles[MAX_SMARTHOME_PROFILE_COUNT];// �龰ģʽ��Ϣ
}CFG_SCENE_MODE_INFO;

//ѹ���ط�������Ϣ
typedef struct tagCFG_COMPRESS_PLAY_INFO
{
    BOOL                bEnable;                // ʹ��
	CFG_CAPTURE_SIZE    emResolution;           // ��Ƶ�ֱ���
	unsigned int        nBitRate;               // ��Ƶ�̶�����ֵ(kbps), ��Χ��192~1024
}CFG_COMPRESS_PLAY_INFO;

// ϵͳ����
typedef enum tagEM_CFG_BUILDING_SYSTEM_TYPE
{
    EM_CFG_BUILDING_SYSTEM_TYPE_UNKNOWN = 0,        // δ֪
    EM_CFG_BUILDING_SYSTEM_TYPE_DIGITAL,            // ����ϵͳ
    EM_CFG_BUILDING_SYSTEM_TYPE_ANALOG,             // ģ��ϵͳ
}EM_CFG_BUILDING_SYSTEM_TYPE;

// VTO ¥������(��Ӧ CFG_CMD_BUILDING ����)
typedef struct tagCFG_BUILDING_INFO
{
    char                            szIssueNumber[CFG_COMMON_STRING_16];                // �ںţ���Χ��0 ~ 99
    char                            szSectionNumber[CFG_COMMON_STRING_16];              // ���ţ���Χ��0 ~ 99
    char                            szBuildingNumber[CFG_COMMON_STRING_16];             // ¥�ţ���Χ��0 ~ 999
    char                            szBuildingUnitNumber[CFG_COMMON_STRING_16];         // ¥�ڵ�Ԫ�ţ���Χ��0 ~ 9
    char                            szSectionUnitNumber[CFG_COMMON_STRING_16];          // С����Ԫ��ţ���Χ��0 ~ 999
    int                             nUnitFloorNumber;                                   // ��Ԫ¥��������Χ��0 ~ 99
    int                             nFloorPerRoomNumber;                                // һ�㷿��������Χ��0 ~ 99
    BOOL                            bCreateRoomEnable;                                  // ���������ʹ��
    EM_CFG_BUILDING_SYSTEM_TYPE     emSystemType;                                       // ϵͳ����
    BOOL                            bEnableSection;                                     // ����ʹ��
    int                             nBuildingNumBit;                                    // ¥�ų���
    int                             nTotalRoomNumber;                                   // �������������ֵΪ9999
    char                            szBuildingName[CFG_COMMON_STRING_64];               // �ſڻ�����
}CFG_BUILDING_INFO;

//VTO ¥����չ����(��Ӧ CFG_CMD_BUILDING_EXTERNAL ����)
typedef struct tagCFG_BUILDING_EXTERNAL_INFO
{
    int                                     nFloorCount;                                            // ��Ԫ�ܲ���
    int                                     nRoomCount;                                             // ÿ�㷿����
    int                                     nBeginNumberCount;                                      // ��ȡ/����ʱ����Ӧ�Ŀ�ʼ����ŵ���Ч����
    // ��ȡʱ������ȡ�Ŀ�ʼ����Ÿ�������MAX_FLOOR_NUM��ǰMAX_FLOOR_NUM����Ч
    // ����ʱ�������õĿ�ʼ����Ÿ�������MAX_FLOOR_NUM��ǰMAX_FLOOR_NUM����Ч
    char                                    szBeginNumber[MAX_FLOOR_NUM][CFG_COMMON_STRING_16];     // ��ʼ�ķ���ţ�nBeginNumber[0] Ϊ��һ�㿪ʼ����ţ�nBeginNumber[1] Ϊ�ڶ��㿪ʼ����ţ�
    int                                     nTotalBuildingNumber;                                   // С���ܴ�������Χ��0 ~ 99
    int                                     nBuildingPerUintNumber;                                 // ÿ����Ԫ������Χ��0 ~ 9
}CFG_BUILDING_EXTERNAL_INFO;

//���Ź���(��Ӧ CFG_CMD_DIALRULE ����)
typedef struct tagCFG_DIALRULE_INFO
{
    BOOL                            bBuildingModeEnable;            //��ģʽʹ��
    BOOL                            bUnitModeEnable;                //��Ԫģʽʹ��
    BOOL                            bExtentionModeEnable;           //�ֻ�ģʽʹ��
    char                            cSeperator;                     //���ŷָ���, �� "#", "-"
    BYTE				            byReserved[3];				    //�ֽڶ���
}CFG_DIALRULE_INFO;

// ������������
typedef struct tagCFG_OIL_MASS_INFO
{
    int                 nTankVolume;            // �����ݻ�����λ����
    int                 nNotifyIntervalTime;    // �ϱ�������Ϣ���ʱ��,��λ����
    int                 nLowOil;                // ����Һλ�ͱ����ٷֱ�(��������20%�򱨾�),��λ���ٷֱȣ�0��100
    int                 nAlarmIntervalTime;     // �ϱ�����Һλ�͵ı���ʱ����(��������ÿ��һ��ʱ����ظ�����),��λ����
}CFG_OIL_MASS_INFO; 

// MAC��ͻ�¼���������
typedef struct tagCFG_MACCONFLICT_INFO 
{
    BOOL                bEnable;                // ʹ�ܿ���
    CFG_ALARM_MSG_HANDLE stuEventHandler;       // ��������
}CFG_MACCONFLICT_INFO;

// ��½��������(��Ӧ CFG_CMD_USERLOCKALARM)
typedef struct tagCFG_USERLOCKALARM_INFO
{
	CFG_ALARM_MSG_HANDLE stuEventHandler;       // ��������
}CFG_USERLOCKALARM_INFO;

// ��½ʧ�ܱ�������(��Ӧ CFG_CMD_LOGIN_FAILURE_ALARM)
typedef struct tagCFG_LOGIN_FAILURE_ALARM
{
	BOOL				 bEnable;			    // ��½ʧ�ܱ���ʹ�ܿ��أ�TRUEΪ�򿪣�FALSEΪ�ر�
	int					 nTryLoginTimes;		// ���Ե�½����������½�����������ﵽ���Դ�����������������
	CFG_ALARM_MSG_HANDLE stuEventHandler;       // ��������
}CFG_LOGIN_FAILURE_ALARM;

// ���ж�������ö��
typedef enum tagEM_CFG_IDLEMOTION_FUNCTION
{
    EM_CFG_IDLEMOTION_FUNCTION_NONE = 0,         // ��
    EM_CFG_IDLEMOTION_FUNCTION_PRESET,           // Ԥ�õ�
    EM_CFG_IDLEMOTION_FUNCTION_SCAN,             // �Զ���ɨ
    EM_CFG_IDLEMOTION_FUNCTION_TOUR,             // Ѳ��
    EM_CFG_IDLEMOTION_FUNCTION_PATTERN,          // �Զ�Ѳ��
} EM_CFG_IDLEMOTION_FUNCTION;

// ���ж���������Ϣ
typedef struct tagCFG_IDLE_MOTION_INFO
{
    BOOL                        bEnable;         // ʹ��
    int                         nTime;           // �������ж�����ʱ��1~60����
    EM_CFG_IDLEMOTION_FUNCTION  emFunction;      // ���ж�������,��ö�� EM_CFG_IDLEMOTION_FUNCTION
    int                         nPresetId;       // Ԥ�õ���,   ��Χ����CFG_PTZ_PROTOCOL_CAPS_INFO��wPresetMin��wPresetMax
    int                         nScanId;         // �Զ���ɨ���, ��Χ����CFG_PTZ_PROTOCOL_CAPS_INFO��wAutoScanMin��wAutoScanMax
    int                         nTourId;         // Ѳ�����,     ��Χ����CFG_PTZ_PROTOCOL_CAPS_INFO��wTourMin��wTourMax
    int                         nPatternId;      // �Զ�Ѳ�����, ��Χ����CFG_PTZ_PROTOCOL_CAPS_INFO��wPatternMin��wPatternMax
}CFG_IDLE_MOTION_INFO;

// ����ǽԤ����Ѳ����
typedef struct tagCFG_MONITORWALL_COLLECTION_TOUR_INFO
{
    int                 nInterval;               // ��Ѳ���ʱ��, ��λ��
    int                 nCollectionNum;          // Ԥ����Ѳ���а�����Ԥ������
    char                szCollectionGroup[CFG_MAX_COLLECTION_NUM][CFG_COMMON_STRING_128]; // Ԥ����Ѳ������, ÿһ����Ԥ��������
}CFG_MONITORWALL_COLLECTION_TOUR_INFO;

// PSTN�����¼�����
typedef struct tagCFG_PSTN_BREAK_LINE_INFO
{
    BOOL                bEnable;                // ʹ�ܿ���
    CFG_ALARM_MSG_HANDLE stuEventHandler;       // ��������
}CFG_PSTN_BREAK_LINE_INFO;

// ����ɼ��豸����, ��ӦCFG_CMD_NET_COLLECTION����
typedef struct tagCFG_NET_COLLECTION_INFO
{
    char                szIp[MAX_ADDRESS_LEN];  // IP
	int                 nPort;                  // �˿�
}CFG_NET_COLLECTION_INFO;

// ����ӿ�����
typedef enum tagEM_CFG_INTERFACE_TYPE
{
    EM_CFG_INTERFACE_TYPE_UNKNOWN = 0,
	EM_CFG_INTERFACE_TYPE_COMM,                 // Comm����, ������, ��ӦCFG_COMMGROUP_INFO����
	EM_CFG_INTERFACE_TYPE_NETCOLLECTION,        // NetCollection����, ������ɼ���, ��ӦCFG_NET_COLLECTION_INFO����
}EM_CFG_INTERFACE_TYPE;

// ����Slot�ڵ�����������豸�Ķ�Ӧ��ϵ, ��ӦCFG_CMD_ALARM_SLOT_BOND����
// ���ڵ�һ��ͨ��(��ͨ��0)�Ѿ��ڱ���ͨ���󶨣����޷�����;
// ����ͨ����ӦSlot�ڵ����, Ԫ������Ϊ��Ӧ����ӿ�
typedef struct tagCFG_ALARM_SLOT_BOND_INFO
{
    EM_CFG_INTERFACE_TYPE   emType;             // ����ӿ�����
	int                     nIndex;	            // ͨ����, ��0��ʼ
	                                            // ����ΪEM_CFG_INTERFACE_TYPE_COMMʱ, ��ʾCFG_COMMGROUP_INFO���õ������±�;
	                                            // ����ΪEM_CFG_INTERFACE_TYPE_NETCOLLECTIONʱ, ��ʾCFG_NET_COLLECTION_INFO���õ������±�;
}CFG_ALARM_SLOT_BOND_INFO;

#define CFG_MAX_CTRLTYPE_NUM       16            // ����բ���Ʒ�ʽ
// ��բ���Ʒ�ʽö��
typedef enum tagEM_CFG_TRAFFICSTROBE_CTRTYPE
{
    EM_CFG_CTRTYPE_UNKNOWN = 0,                 // δ����
    EM_CFG_CTRTYPE_TRAFFICTRUSTLIST,            // ͨ�������������Ƿ�բ��ֻ�а������ڳ����ſ�բ
    EM_CFG_CTRTYPE_ALLSNAPCAR,                  // �������ץ�ĳ�������բ
    EM_CFG_CTRTYPE_ORDER,                       // ͨ���ϲ��·������բ
} EM_CFG_TRAFFICSTROBE_CTRTYPE;

// ���г���բ����
typedef enum tagEM_CFG_ALL_SNAP_CAR
{
	EM_CFG_ALL_SNAP_CAR_UNKNOWN,				// δ֪��բ����
	EM_CFG_ALL_SNAP_CAR_PLATE,					// �������Ƴ�����
	EM_CFG_ALL_SNAP_CAR_NOPLATE,				// �������Ƴ�����
}EM_CFG_ALL_SNAP_CAR;

// ��բ������Ϣ(��Ӧ CFG_CMD_TRAFFICSTROBE ����)
typedef struct tagCFG_TRAFFICSTROBE_INFO
{
    BOOL                            bEnable;                            // ʹ��
    int                             nCtrlTypeCount;                     // ��բ���Ʒ�ʽ����
    EM_CFG_TRAFFICSTROBE_CTRTYPE    emCtrType[CFG_MAX_CTRLTYPE_NUM];    // ��բ���Ʒ�ʽ,��ö�� EM_CFG_TRAFFICSTROBE_CTRTYPE
    CFG_ALARM_MSG_HANDLE            stuEventHandler;					// ������բ��������
    CFG_ALARM_MSG_HANDLE            stuEventHandlerClose;               // �رյ�բ��������
	int								nAllSnapCarCount;					// ���г���բ�������
	EM_CFG_ALL_SNAP_CAR				emAllSnapCar[MAX_ALL_SNAP_CAR_COUNT];	// ���г���բ����				
}CFG_TRAFFICSTROBE_INFO;

#define CFG_MAX_PLATEENABLE_NUM       16        // ���ʹ�ܹ������Ʋ�������
#define CFG_MAX_TRAFFICVOICE_NUM      8         // ��������������ø���
// ʹ�ܹ������Ʋ�������ö��
typedef enum tagEM_CFG_PLATEENABLE_TYPE
{
    EM_CFG_PLATEENABLE_UNKNOWN = 0,             // δ����
    EM_CFG_PLATEENABLE_TIPS,                    // ���Ż�ӭ��
    EM_CFG_PLATEENABLE_PLATE,                   // ���ų���
} EM_CFG_PLATEENABLE_TYPE;

// ���ܽ�ͨ����������Ϣ
typedef struct tagCFG_TRAFFICVOICE_BROADCAST_INFO
{
    int                             nEnableCount;                       // ʹ�ܲ�������
    EM_CFG_PLATEENABLE_TYPE         emEnable[CFG_MAX_PLATEENABLE_NUM];  // ʹ�ܹ������Ʋ���,��ö�� EM_CFG_PLATEENABLE_TYPE
    char                            szNormalCar[MAX_PATH];              // ��ͨ����������������,����:���������ļ�"��ӭ����.wav"
    char                            szTrustCar[MAX_PATH];               // ���γ���������������,����:���������ļ�"��ӭ����.wav"
    char                            szSuspiciousCar[MAX_PATH];          // ���ɳ���������������,����:���������ļ�"��ע�ᳵ��.wav"
}CFG_TRAFFICVOICE_BROADCAST_INFO;

// ���ܽ�ͨ������������(��Ӧ CFG_CMD_TRAFFICVOICE ����)
typedef struct tagCFG_TRAFFICVOICE_BROADCAST
{
    int                             nTrafficVoiceNum;                           // �����������ø���
    CFG_TRAFFICVOICE_BROADCAST_INFO stuTrafficVoices[CFG_MAX_TRAFFICVOICE_NUM];	// ����������������, �����ʾ
}CFG_TRAFFICVOICE_BROADCAST;

// ͣ��ʱ������
typedef struct tagCFG_STANDING_TIME_INFO
{
    BOOL                        bEnable;         // �Ƿ�����ͣ����ʱ��������
    int                         nUpperlimit;     // ͣ��ʱ������, ��λ������
    int                         nInterval;       // DH_ALARM_VEHICLE_STANDING_OVER_TIME�¼��ϱ����, ��λ����, -1��ʾ����Ҫ�ظ��ϱ�
} CFG_STANDING_TIME_INFO;

// ����Χ������ʱ�����Ϣ
typedef struct tagENCLOSURE_TIME_SCHEDULE_INFO
{
    int                         nEnclosureID;                                          // Χ����ID��
    CFG_TIME_SECTION            stuTimeSection[MAX_TIME_SCHEDULE_NUM][MAX_REC_TSECT];  // �¼���Ӧʱ���
} ENCLOSURE_TIME_SCHEDULE_INFO;

// ����Χ���������
#define MAX_ENCLOSURE_NUM 128

// ����Χ������ʱ�������
typedef struct tagCFG_ENCLOSURE_TIME_SCHEDULE_INFO
{
    int                                nEnclosureInfoCount;                     // Χ������
    ENCLOSURE_TIME_SCHEDULE_INFO       stuEnclosureInfo[MAX_ENCLOSURE_NUM];     // Χ����Ϣ
    char                               szVersion[CFG_COMMON_STRING_64];         // Χ��ʱ������ð汾��
} CFG_ENCLOSURE_TIME_SCHEDULE_INFO;

// ͣ��������ڿ���������ģʽ
typedef enum tagEM_ECK_WORK_MODE_TYPE
{
    EM_ECK_WORK_MODE_UNKNOWN = 0,
    EM_ECK_WORK_MODE_IMPORT,                                    // ��ڿ��ƻ�
    EM_ECK_WORK_MODE_EXPORT,                                    // ���ڿ��ƻ�
}EM_ECK_WORK_MODE_TYPE;

// ͣ��������ڿ�������բģʽ
typedef enum tagEM_ECK_STROBE_MODE_TYPE
{
    EM_ECK_STROBE_MODE_UNKNOWN = 0,
    EM_ECK_STROBE_MODE_ON,                                      // ʼ�տ�բ
    EM_ECK_STROBE_MODE_OFF,                                     // ʼ�չ�բ
    EM_ECK_STROBE_MODE_AUTO,                                    // �Զ�����բ
}EM_ECK_STROBE_MODE_TYPE;

// ͣ��������ڿ�������������ʼ�����ٶ�
typedef enum tagEM_ECK_LED_SPEED_TYPE
{
    EM_ECK_LED_SPEED_UNKNOWN = 0,
    EM_ECK_LED_SPEED_LOW,                                       // ��
    EM_ECK_LED_SPEED_NORMAL,                                    // ����
    EM_ECK_LED_SPEED_HIGH,                                      // ��
}EM_ECK_LED_SPEED_TYPE;

// ͣ��������ڿ�����LED����������
typedef struct tagCFG_ECK_LED_INFO 
{
    EM_ECK_LED_SPEED_TYPE   emLEDSpeed;                         // LED��������ʼ�����ٶ�
    char                    szCustomData[CFG_COMMON_STRING_512];// LED��������ʼ�Զ����ַ�, ��Ч����0-256�ֽ�
}CFG_ECK_LED_INFO;

// ͣ��������ڿ���������
typedef struct tagCFG_ECKCONFIG_INFO 
{
    EM_ECK_WORK_MODE_TYPE   emWorkMode;                         // ����ģʽ
    EM_ECK_STROBE_MODE_TYPE emStrobeMode;                       // ��բģʽ
    BOOL                    bForceOffline;                      // ǿ���ѻ�����, TRUE:ǿ���ѻ�����, FALSE:����ƽ̨���ƹ���
    BOOL                    bLoopSence;                         // �ظй��ܿ���, TRUE:�����ظй���, FALSE:�رյظй���
    CFG_ECK_LED_INFO        stuLED;                             // ������Ļ����
}CFG_ECKCONFIG_INFO;

// ��ͷָ��
typedef enum tagEM_DIRECTION
{
    EM_DIRECTION_UNKNOWN,
    EM_DIRECTION_LEFT,                                          // ��
    EM_DIRECTION_UPLEFT,                                        // ����
    EM_DIRECTION_UP,                                            // ��
    EM_DIRECTION_UPRIGHT,                                       // ����
    EM_DIRECTION_RIGHT,                                         // ��
    EM_DIRECTION_DOWNRIGHT,                                     // ����
    EM_DIRECTION_DOWN,                                          // ��
    EM_DIRECTION_DOWNLEFT,                                      // ����
}EM_DIRECTION;

// ��ͷ��������ֵ�λ��
typedef enum tagEM_GUIDESCREEN_POS
{
    EM_GUIDESCREEN_POS_UNKNOWN,
    EM_GUIDESCREEN_POS_LEFT,                                    // ���
    EM_GUIDESCREEN_POS_RIGHT,                                   // �Ҳ�
}EM_GUIDESCREEN_POS;

// �յ��������Ƿ����
typedef enum tagEM_GUIDESCREEN_ROLL
{
    EM_GUIDESCREEN_ROLL_UNKNOWN,
    EM_GUIDESCREEN_ROLL_DISABLE,                                // ������
    EM_GUIDESCREEN_ROLL_ENABLE,                                 // ����
}EM_GUIDESCREEN_ROLL;

// �յ�����ͷ����
typedef struct tagCFG_GUISCREEN_ARROW
{
    EM_DIRECTION            emDirect;                           // ��ͷָ��
    EM_GUIDESCREEN_POS      emPos;                              // ��ͷ��������ֵ�λ��
    EM_GUIDESCREEN_ROLL     emRoll;                             // �Ƿ����
}CFG_GUISCREEN_ARROW;

#define CFG_GUIDE_SCREEN_NUM_MAX (8)                            // �յ����������

// �յ���ϵͳ����(��Ӧ CFG_CMD_GUIDESCREEN ����)
typedef struct tagCFG_GUIDESCREEN_INFO
{
    char                    szText[CFG_COMMON_STRING_128];      // ������ʱ,������ʾ����
                                                                // Ӣ�Ļ������ĺ��֣�������ҪGB2312���룬���Ȳ�����64�ֽ�
    int                     nArrowNum;                          // ��ͷ��־����
    CFG_GUISCREEN_ARROW     stuArrow[CFG_GUIDE_SCREEN_NUM_MAX]; // ��ͷ��־����, ��Ӧ��������������������
}CFG_GUIDESCREEN_INFO;

// ͣ���������ˢ�������¼�����(��ӦCFG_CMD_PARKING_CARD����)
typedef struct tagCFG_PARKING_CARD_INFO
{
    BOOL                            bEnable;                // �Ƿ�����
    CFG_ALARM_MSG_HANDLE            stuEventHandler;        // ��������
} CFG_PARKING_CARD_INFO;

// ������ʽ, ��ͨ��ʲô��ʽ�����ı���
typedef enum tagEM_CFG_RCEMERGENCY_MODE_TYPE
{
    EM_CFG_RCEMERGENCY_MODE_UNKNOWN = 0,
    EM_CFG_RCEMERGENCY_MODE_KEYBOARD,                       // ����
    EM_CFG_RCEMERGENCY_MODE_WIRELESS_CONTROL,               // ң����
}EM_CFG_RCEMERGENCY_MODE_TYPE;

// ����������������
typedef enum tagEM_CFG_RCEMERGENCY_CALL_TYPE
{
    EM_CFG_RCEMERGENCY_CALL_UNKNOWN = 0,
    EM_CFG_RCEMERGENCY_CALL_FIRE,                           // ��
    EM_CFG_RCEMERGENCY_CALL_DURESS,                         // в��
    EM_CFG_RCEMERGENCY_CALL_ROBBER,                         // �˾�
    EM_CFG_RCEMERGENCY_CALL_MEDICAL,                        // ҽ��
}EM_CFG_RCEMERGENCY_CALL_TYPE;

// �������б����¼�����(��Ӧ CFG_CMD_RCEMERGENCYCALL ����)
typedef struct tagCFG_RCEMERGENCY_CALL_INFO 
{
    BOOL                            bEnable;                // �Ƿ�����
    EM_CFG_RCEMERGENCY_MODE_TYPE    emMode;                 // ������ʽ
    EM_CFG_RCEMERGENCY_CALL_TYPE    emAlarmType;            // ��������
    CFG_ALARM_MSG_HANDLE            stuEventHandler;        // ��������
}CFG_RCEMERGENCY_CALL_INFO;

// ������Ϣ�ϱ�����(��Ӧ CFG_CMD_LANES_STATE_REPORT ����)
typedef struct tagCFG_LANES_STATE_REPORT
{
    BOOL                bEnable;        //�ϱ�ʹ��	true:�ϱ�ʹ�� ,false:�ϱ���ʹ��
    int                 nPeriod;        //�ϱ�����	��λ����
}CFG_LANES_STATE_REPORT;

#define CFG_MAX_USER_ID_LEN                     32              // �Ž����û�ID��󳤶�
#define CFG_MAX_METHODEX_NUM                    4               // ���ŷ�ʽ��չ������

// ������Ͽ��ŵĿ��ŷ�ʽ
typedef enum tagEM_CFG_OPEN_DOOR_GROUP_METHOD
{
    EM_CFG_OPEN_DOOR_GROUP_METHOD_UNKNOWN = 0,
    EM_CFG_OPEN_DOOR_GROUP_METHOD_CARD,                         // ˢ��
    EM_CFG_OPEN_DOOR_GROUP_METHOD_PWD,                          // ����
    EM_CFG_OPEN_DOOR_GROUP_METHOD_FINGERPRINT,                  // ָ��
    EM_CFG_OPEN_DOOR_GROUP_METHOD_ANY,                          // ������Ϸ�ʽ����
    EM_CFG_OPEN_DOOR_GROUP_METHOD_FACE,                         // ����
}EM_CFG_OPEN_DOOR_GROUP_METHOD;

// ������Ͽ�������ϸ��Ϣ
typedef struct tagCFG_OPEN_DOOR_GROUP_DETAIL 
{
    char                            szUserID[CFG_MAX_USER_ID_LEN];       // �û�ID
    EM_CFG_OPEN_DOOR_GROUP_METHOD   emMethod;                            // ���ŷ�ʽ
    int                             nMethodExNum;                        // ���ŷ�ʽ��չ����
    EM_CFG_OPEN_DOOR_GROUP_METHOD   emMethodEx[CFG_MAX_METHODEX_NUM];    // ���ŷ�ʽ��չ
}CFG_OPEN_DOOR_GROUP_DETAIL;

#define CFG_MAX_OPEN_DOOR_GROUP_DETAIL_NUM      64               // ÿһ����˿�����ϵ��������

// ������Ͽ�������Ϣ
typedef struct tagCFG_OPEN_DOOR_GROUP 
{
    int                         nUserCount;                     // �û���Ŀ����ʾ��Ҫ��ϲ��ܿ��ŵ�����
    int                         nGroupNum;                      // ��Ч����Ŀ
    CFG_OPEN_DOOR_GROUP_DETAIL  stuGroupDetail[CFG_MAX_OPEN_DOOR_GROUP_DETAIL_NUM];// ������Ͽ��������ϸ��Ϣ
    BOOL                        bGroupDetailEx;                         //  TRUE: stuGroupDetail �ֶ���Ч��pstuGroupDetailEx�ֶ���Ч, FALSE: stuGroupDetail �ֶ���Ч��pstuGroupDetailEx�ֶ���Ч
    int					        nMaxGroupDetailNum;              // ������Ͽ��������ϸ��Ϣ������
    CFG_OPEN_DOOR_GROUP_DETAIL	    *pstuGroupDetailEx;              /* ������Ͽ��������ϸ��Ϣ��չ, ���û������ڴ�, ��СΪsizeof(CFG_OPEN_DOOR_GROUP_DETAIL)*nMaxUserCount, 
                                                                    ��������Ͽ��������ϸ��Ϣ�������� CFG_MAX_OPEN_DOOR_GROUP_DETAIL_NUM ʱʹ�ô��ֶ� */
}CFG_OPEN_DOOR_GROUP;

#define CFG_MAX_OPEN_DOOR_GROUP_NUM             4               // ���˿�����ϵ���������

// ���˶࿪�ŷ�ʽ���(��Ӧ CFG_CMD_OPEN_DOOR_GROUP ����),��ʾÿ��ͨ���������Ϣ��
// ��һ��ͨ������ϵĿ������ȼ���ߣ��������εݼ�
typedef struct tagCFG_OPEN_DOOR_GROUP_INFO 
{
    int                         nGroup;                         // ��Ч�����
    CFG_OPEN_DOOR_GROUP         stuGroupInfo[CFG_MAX_OPEN_DOOR_GROUP_NUM];// ���˿��������Ϣ
}CFG_OPEN_DOOR_GROUP_INFO;

#define MAX_READER_ID_LEN                       32              // ������ID��󳤶�

// ����·�ߵ�ÿ���ڵ����Ϣ
typedef struct tagCFG_DOOR_ROUTE_NODE_INFO
{
    char                        szReaderID[MAX_READER_ID_LEN];  // ������ID�����Ž������������Ķ�������ID
}CFG_DOOR_ROUTE_NODE_INFO;

// ÿ������·�����ڵ�(��������Ŀ)
#define MAX_DOOR_ROUTE_NODE_NUM     8

// �����鼯����Ϣ�����������±��С����˳�򣬱�ʾ����·��
typedef struct tagCFG_DOOR_ROUTE_INFO
{
    int                         nDoors;                             // ����·����Ч�ڵ���
    CFG_DOOR_ROUTE_NODE_INFO    stuDoors[MAX_DOOR_ROUTE_NODE_NUM];  // ����·�߽ڵ���Ϣ
	UINT                        nResetTime;                         // ��Ǳ����ʱ�䣬��λ�����ӣ�0��ʾ������ 
}CFG_DOOR_ROUTE_INFO;

#define MAX_OPEN_DOOR_ROUTE_NUM                 16                  // �����·����Ŀ

// ����·�߼��ϣ���Ʒ���Ǳ·������(��Ӧ CFG_CMD_OPEN_DOOR_ROUTE ����)
typedef struct tagCFG_OPEN_DOOR_ROUTE_INFO
{
    int                         nDoorList;
    CFG_DOOR_ROUTE_INFO         stuDoorList[MAX_OPEN_DOOR_ROUTE_NUM];
	int							nTimeSection;                       // ��Ǳ·����Ӧʱ���, ��CFG_ACCESS_TIMESCHEDULE_INFO�������±�   
	UINT                        nResetTime;                         // ��Ǳ����ʱ�䣬��λ�����ӣ�0��ʾ�����á���Ч�ֶΣ��ϳ�
}CFG_OPEN_DOOR_ROUTE_INFO;

#define MAX_BURNPLAN_DEVICES_NUM                     32
#define MAX_BURNPLAN_CHANNELS_NUM                     32

// ��¼����ʽ
typedef enum tagEM_CFG_BURNPLAN_RECORDPACK_TYPE
{
    EM_CFG_BURNPLAN_RECORDPACK_TYPE_UNKNOWN,        // δ֪
    EM_CFG_BURNPLAN_RECORDPACK_TYPE_DHAV,           // "DHAV"
    EM_CFG_BURNPLAN_RECORDPACK_TYPE_PS,             // "PS"
    EM_CFG_BURNPLAN_RECORDPACK_TYPE_ASF,            // "ASF"
    EM_CFG_BURNPLAN_RECORDPACK_TYPE_MP4,            // "MP4"
    EM_CFG_BURNPLAN_RECORDPACK_TYPE_TS,             // "TS"
}EM_CFG_BURNPLAN_RECORDPACK_TYPE;

// ��¼ģʽ
typedef enum tagEM_CFG_BURNPLAN_MODE
{
    EM_CFG_BURNPLAN_MODE_UNKNOWN,           // δ֪
    EM_CFG_BURNPLAN_MODE_SYNC,              // ͬ���� "Sync"
    EM_CFG_BURNPLAN_MODE_TURN,              // ����:  "Turn"
    EM_CFG_BURNPLAN_MODE_CYCLE,             // ѭ��:  "Cycle"
}EM_CFG_BURNPLAN_MODE;

// ��¼�ƻ���Ϣ
typedef struct tagCFG_BURNPLAN_DETAIL
{
    int                                 nSessionId;                             // ��Ѷ��
    int                                 nDevicesNum;                            // ����ûỰ���豸�б���Ч����
    int                                 nDevices[MAX_BURNPLAN_DEVICES_NUM];     // ����ûỰ���豸�б�
    int                                 nChannelsNum;                           // ��¼����Ƶͨ����Ч����
    int                                 nChannels[MAX_BURNPLAN_CHANNELS_NUM];   // ��¼����Ƶͨ��
    EM_CFG_BURNPLAN_RECORDPACK_TYPE     emRecordpack;                           // ��¼����ʽ
    EM_CFG_BURNPLAN_MODE                emMode;                                 // ��¼ģʽ
    CFG_NET_TIME                        stuStarttime;                           // ��¼��ʼʱ��
}CFG_BURNPLAN_DETAIL;

#define MAX_BURNPLAN_DETAIL_NUM          32

// ��¼�ƻ�����(��ӦCFG_BURNPLAN_INFO)
typedef struct tagCFG_BURNPLAN_INFO
{
    int                         nBurnPlanDetailNum;                                 // ��¼�ƻ���Ϣ�б���Ч����
    CFG_BURNPLAN_DETAIL         stuBurnPlanDetail[MAX_BURNPLAN_DETAIL_NUM];         // ��¼�ƻ���Ϣ�б�
}CFG_BURNPLAN_INFO;

typedef struct tagCFG_SCADA_DEV_INFO 
{
    BOOL                            bEnable;                // �Ƿ�����
    char                            szDevType[CFG_COMMON_STRING_64];// �豸����
    char                            szDevName[CFG_COMMON_STRING_64];// �豸����, Ψһ��ʾ�豸��
    int                             nSlot;                  // �����λ��, ���AlarmSlotBond����
    int                             nLevel;                 // ���Slot�󶨵���NetCollection���͵Ļ������ֶ�Ϊ-1
    CFG_ALARM_MSG_HANDLE            stuEventHandler;        // ��������
}CFG_SCADA_DEV_INFO;

// ͨ�������Ϣ
typedef struct tagCFG_VSP_GAYS_CHANNEL_INFO
{
    char                            szId[CFG_COMMON_STRING_64];             // ͨ�����	�ַ�����24λ��
    int                             nAlarmLevel;                            // ��������[1,6]	����
}CFG_VSP_GAYS_CHANNEL_INFO;

// ���������Ϣ
typedef struct tagCFG_VSP_GAYS_ALARM_INFO
{
    char                            szId[CFG_COMMON_STRING_64];             // ͨ�����	�ַ�����24λ��
    int                             nAlarmLevel;                            // ��������[1,6]	����
}CFG_VSP_GAYS_ALARM_INFO;

// ����һ��ƽ̨��������(CFG_CMD_VSP_GAYS)
typedef struct tagCFG_VSP_GAYS_INFO
{
    BOOL                            bEnable;                                // �Ƿ�ʹ�ܣ�TRUEʹ�ܣ�FALSE��ʹ��
    char                            szSipSvrId[CFG_COMMON_STRING_64];       // SIP���������	�ַ�����24λ��
    char                            szDomain[CFG_COMMON_STRING_256];        // SIP��	�ַ�����128λ��
    char                            szSipSvrIp[CFG_COMMON_STRING_32];       // SIP������IP	�ַ�����16λ��
    char                            szDeviceId[CFG_COMMON_STRING_64];       // �豸��ţ�Ҳ��ע���û���	�ַ�����24λ��
    char                            szPassword[CFG_COMMON_STRING_64];       // ע������,����	�ַ�����24λ��
    unsigned short                  nLocalSipPort;                          // ����SIP����˿�	�޷��Ŷ�����
    unsigned short                  nSipSvrPort;                            // SIP�������˿�	�޷��Ŷ�����
    int                             nSipRegExpires;                         // ע����Ч��	����
    int                             nKeepAliveCircle;                       // ��������	����
    int                             nMaxTimeoutTimes;                       // ���������ʱ����	����
    char                            szCivilCode[CFG_COMMON_STRING_64];      // ������������	�ַ�����24λ��
    char                            szIntervideoID[CFG_COMMON_STRING_64];   // ����ģ��ʶ����	�ַ�����24λ��
    unsigned short                  nChannelSum;                            // ͨ����	�޷��Ŷ�����
    unsigned short                  nAlarmInSum;                            // �ⲿ����������	�޷��Ŷ�����
    CFG_VSP_GAYS_CHANNEL_INFO       stuChannelInfo[MAX_VIDEO_CHANNEL_NUM];  // ͨ�������Ϣ	���飬��Ч����ͬchannelSum
    CFG_VSP_GAYS_ALARM_INFO         stuAlarmInfo[MAX_ALARM_CHANNEL_NUM];    // ���������Ϣ	���飬��Ч����ͬalarmInSum
}CFG_VSP_GAYS_INFO;

// ��Ƶ��ⱨ������(CFG_CMD_AUDIODETECT)
typedef struct tagCFG_AUDIO_DETECT_INFO
{
    BOOL                            bEnable;                                // �Ƿ�ʹ�ܣ�TRUEʹ�ܣ�FALSE��ʹ��
    int                             nMinVolume;                             // ��������
    int                             nMaxVolume;                             // ��������
    BOOL                            bAnomalyDetect;                         // �����쳣���ʹ��
    int                             nAnomalySensitive;                      // �����쳣���������ֵ	ȡֵ��Χ1~100
    BOOL                            bMutationDetect;                        // ��ǿͻ����ʹ��
    int                             nMutationThreold;                       // ��ǿͻ������ֵ	ȡֵ��Χ1~100
    CFG_ALARM_MSG_HANDLE            stuEventHandler;						// ��������
    BOOL                            bIntensityDetect;                       // ��Ƶǿ�ȼ��ʹ��
    unsigned int                    nIntensityDecibelGate;                  // ��Ƶǿ�ȼ��ֱ���ֵ, ��λ�ֱ�, ȡֵ��Χ0~90
}CFG_AUDIO_DETECT_INFO;

// VTS��������(��Ӧ���� CFG_CMD_VTS_CALL_INFO)
typedef struct tagCFG_VTS_CALL_INFO 
{
    CFG_TIME_SECTION                stuCallEnableTime;                      // �������VTS��ʱ���
}CFG_VTS_CALL_INFO;

// �豸�б�������ÿ���豸����Ϣ
typedef struct tagCFG_DEV_LIST
{
    char                            szDevType[CFG_COMMON_STRING_128];       // �豸����, ֧��: "VTT", "VTS"
    char                            szDevName[CFG_COMMON_STRING_128];       // �豸����
    char                            szIP[MAX_ADDRESS_LEN];                  // ip��ַ
    int                             nPort;                                  // �˿ں�
    char                            szUser[CFG_COMMON_STRING_64];           // �û���
    char                            szPassword[CFG_COMMON_STRING_64];       // ����
    char                            szDevID[CFG_COMMON_STRING_128];         // �豸ID��, ����Ψһ
    char                            szParentID[CFG_COMMON_STRING_128];      // ���ڵ�ID, ����Ψһ
}CFG_DEV_LIST;

// �豸�б�����(��Ӧ���� CFG_CMD_DEV_LIST_INFO)
typedef struct tagCFG_DEV_LIST_INFO 
{
    CFG_DEV_LIST*                   pstuDevList;                            // ������Ϣ, �û������ڴ�,��СΪsizeof(CFG_DEV_LIST)*dwDevListMaxCount
    DWORD                           dwDevListMaxCount;                      // pstuDevList�����ڴ����Ч�ṹ�����
    DWORD                           dwDevListRetCount;                      // ʵ�ʷ��صĽṹ�����, �����ɽṹ��ʱ��Ч
}CFG_DEV_LIST_INFO;

#define MAX_CALIBRATE_MATRIX_LEN    8       // �궨������󳤶�

#define MAX_CALIBRATE_MATRIX_EX_LEN 32      // �궨������󳤶�(��չ)
#define MAX_POINT_PAIR_LEN			10		// ���Ӹ��ٱ궨���������󳤶�

// ����ʽ�������궨��������(��Ӧ CFG_CMD_CALIBRATE_MATRIX ����)
typedef struct tagCFG_CALIBRATE_MATRIX_INFO 
{
    char                szSlaveSN[CFG_COMMON_STRING_128];                   // ���豸���к�
    DWORD               dwMatrixLen;                                        // �궨��������Чdword����
    DWORD               dwMatrix[MAX_CALIBRATE_MATRIX_LEN];                 // �궨����, ��һ��128�ֽڵĶ����ƴ�, Ϊ��߽���Ч��, ����Ϊ32��uint32_tֵ
}CFG_CALIBRATE_MATRIX_INFO;

// ������궨������(8192����ϵ)
typedef struct tagCFG_MASTER_POINT
{
	int 	nX;
	int		nY;
} CFG_MASTER_POINT;

// �����(���)�궨������(ptz��һ������)
typedef struct tagCFG_SLAVE_POINT
{
	float                    fPositionX;           //��̨ˮƽ�˶�λ��
	float                    fPositionY;           //��̨��ֱ�˶�λ��
	float                    fZoom;
} CFG_SLAVE_POINT;


//���Ӹ��ٱ궨�������
typedef struct tagCFG_POINT_PAIR
{
    CFG_MASTER_POINT     	stuMasterPoint;           // ������궨��,8192����ϵ
    CFG_SLAVE_POINT			stuSlavePoint;            // �����(���)�궨��,PTZ��һ������
} CFG_POINT_PAIR;

// ���Ӹ������궨���þ���ֵ
typedef struct tagCFG_CALIBRATE_MATRIX_ELEMENT
{
	char                szMasterSN[CFG_COMMON_STRING_128];		// ���豸���к�(��ѡ)
    char                szSlaveSN[CFG_COMMON_STRING_128];		// ���豸���к�
    DWORD               dwMatrixLen;							// �궨��������Чdword����
    DWORD               dwMatrix[MAX_CALIBRATE_MATRIX_EX_LEN];	// �궨����, ��һ��128�ֽڵĶ����ƴ�, Ϊ��߽���Ч��, ����Ϊ32��uint32_tֵ
    DWORD               dwPointPairNum;							// ���Ӹ��ٱ궨��Ե�����
    CFG_POINT_PAIR      stuPointPair[MAX_POINT_PAIR_LEN];		// ���Ӹ��ٱ궨���(��ѡ),��һ�����飬��ౣ��10��궨���
	BOOL				bExpectRatio;							// ��ʾ�Ƿ���Ҫ������������
	float               szExpectRatio[2];						// ��������(��ѡ����bExpectRatioΪTRUEʱ��Ч), ��һ��ֵΪ��̨T���꣬��һ��[-1.0, 1.0]���ڶ���ֵΪ����
} CFG_CALIBRATE_MATRIX_ELEMENT;

// ����ʽ�������궨������չ����(��Ӧ CFG_CMD_CALIBRATE_MATRIX����)
typedef struct tagCFG_CALIBRATE_MATRIX_EX_INFO 
{
    DWORD							 nMaxCalibElement;			// ���Ӹ������궨��������Ԫ��������
	DWORD							 nRetCalibElement;			// ���Ӹ������궨��������Ԫ��ʵ�ʸ���
	CFG_CALIBRATE_MATRIX_ELEMENT	*pstCaliMatrixElement;		// ���Ӹ������궨��������ÿ��Ԫ��ֵ�����û������ڴ�,��СΪsizeof(CFG_CALIBRATE_MATRIX_ELEMENT)*nMaxCalibElement
} CFG_CALIBRATE_MATRIX_EX_INFO;

// ������ʱ����(��Ӧ CFG_CMD_DEFENCE_AREA_DELAY ����)
typedef struct tagCFG_DEFENCE_AREA_DELAY_INFO 
{
    int                 nEnterDelay1;           // �н�����ʱ�ķ������ͣ�����������EM_CFG_DefenceAreaType_Entrance2��, ��ʹ�ô˽�����ʱ
    int                 nEnterDelay2;           // ֻ�з�������ΪEM_CFG_DefenceAreaType_Entrance2, �Ż�ʹ�ô˽�����ʱ
    int                 nExitDelay;             // �������˳���ʱ�ķ���, ��ʹ�ô��˳���ʱ
}CFG_DEFENCE_AREA_DELAY_INFO;

// �ȳ�������ģʽ
typedef enum tagCFG_THERMO_GAIN_MODE 
{
    CFG_THERMO_GAIN_MODE_UNKNOWN,
    CFG_THERMO_GAIN_MODE_HIGHTEMP,                      // ����
    CFG_THERMO_GAIN_MODE_LOWTEMP,                       // ����
    CFG_THERMO_GAIN_MODE_AUTO,                          // �Զ�
} CFG_THERMO_GAIN_MODE;

// �ȳ����Զ���������
typedef struct tagCFG_THERMO_AUTO_GAIN
{
    int                         nLowToHigh;             // �¶ȳ������趨ֵʱ���Զ��л�������ģʽ
    int                         nLHROI;                 // �ɵ����л�������ʱ��ROI �ٷֱ�0~100
    int                         nHighToLow;             // �¶��½������趨ֵʱ���Զ��л�������ģʽ
    int                         nHLROI;             	// �ɸ����л�������ʱ��ROI �ٷֱ�0~100
} CFG_THERMO_AUTO_GAIN;

//��������
typedef struct tagCFG_THERMO_GAIN
{
    int                         nAgc;                   // �Զ�������� [0-255]����ȡֵ��Χ����������
    int                         nAgcMaxGain;            // ����Զ����� [0-255]����ȡֵ��Χ����������
    int                         nAgcPlateau;            // ������� ����ȡֵ��Χ����������
}CFG_THERMO_GAIN;

// �ȳ������ã�����ģʽ������
typedef struct tagCFG_THERMOGRAPHY_OPTION
{
	int                         nEZoom;                 // ����
	int                         nThermographyGamma;     // ٤��ֵ
	int                         nColorization;	        // α��ɫ���� NET_THERMO_COLORIZATION
	int                         nSmartOptimizer;        // ���ܳ����Ż�ָ�� 0 ~100�� ����ȡֵ��Χ����������
    BOOL                        bOptimizedRegion;       // �Ƿ�������Ȥ����ֻ�и���Ȥ�����ڵ���Ϣ�ᱻ����ͳ���������Զ����ȵ�����AGC�� 
	int                         nOptimizedROIType;      // ����Ȥ�������ͣ��� NET_THERMO_ROI
	int                         nCustomRegion;          // �Զ����������
	CFG_RECT                    stCustomRegions[64];    // �Զ������򣬽��� nOptimizedROIType Ϊ NET_THERMO_ROI_CUSTOM ʱ��Ч
    char                        Reserved[256];          // �˱����ֶ�ȷ���˽ṹ������ NET_THERMO_GRAPHY_INFO ��ͬ
    CFG_THERMO_GAIN             stuLowTempGain;         // �����µ���������
    int                         nGainMode;              // ����ģʽ���μ� CFG_THERMO_GAIN_MODE
    CFG_THERMO_AUTO_GAIN        stAutoGain;             // �Զ��������ã�ֻ������ģʽΪ CFG_THERMO_GAIN_MODE_AUTO ��Ч
    CFG_THERMO_GAIN             stuHighTempGain;        // �����µ���������
    int                         nBaseBrightness;        // ��׼����
    int                         nStretchIntensity;      // ����ǿ��
    CFG_RECT                    stuContrastRect;        // ������ǿλ��,���ӱ��������ܱߵĶԱȶ�,8192����ϵ
} CFG_THERMOGRAPHY_OPTION;

// �ȳ�������
typedef struct tagCFG_THERMOGRAPHY_INFO
{
	int                         nModeCount;             // ģʽ������Ŀǰֻ��һ��
	CFG_THERMOGRAPHY_OPTION     stOptions[16];          // ��Ӧ��ͬģʽ������
} CFG_THERMOGRAPHY_INFO;

// ƽ���۽�У׼ģʽ
typedef enum tagEM_FLAT_FIELD_CORRECTION_MODE
{
    EM_FLAT_FIELD_CORRECTION_UNKNOWN,                   //  δ֪ģʽ,���ڱ�ʾGetNewDevConfig��ȡ����δ֪ģʽ,SetNewDevConfig����ʹ�ø�ģʽ
    EM_FLAT_FIELD_CORRECTION_AUTO,                      // "Auto":�Զ�ƽ���۽�У׼
    EM_FLAT_FIELD_CORRECTION_MANUAL,                    // "Manual":�ֶ�ƽ���۽�У׼
}EM_FLAT_FIELD_CORRECTION_MODE;

// ƽ���۽�У׼
typedef struct tagCFG_FLAT_FIELD_CORRECTION_INFO
{
    EM_FLAT_FIELD_CORRECTION_MODE   emMode;             // ƽ���۽�У׼ģʽ,���EM_FLAT_FIELD_CORRECTION_MODE
    int                             nPeriod;            // �Զ��л����ڣ���λΪ�룬��ֵ��Χ�ο�NET_OUT_THERMO_GETCAPS�е�stFFCPeriod
} CFG_FLAT_FIELD_CORRECTION_INFO;

// �ȳ�����Ƶ�ں�����
typedef struct tagCFG_THERMO_FUSION_INFO
{
    unsigned int                unMode;                 // �ȳ���ͼ���ں�ģʽ,0ԭʼ�ȳ���ͼ��,1 ��ɼ����ں�ģʽ
    unsigned int                unRate;                 // �ȳ���ͼ���ں��ʣ���Mode=1ʱ��Ч,0~100��ֵԽ��ɼ����ռ��Խ��
} CFG_THERMO_FUSION_INFO;

// �ȳ�������ͷ���Եľֲ��Աȶ���ǿ����
typedef struct tagCFG_LCE_STATE_INFO
{
    unsigned int              unLCEValue;               // �ֲ��Աȶ���ǿ״ֵ̬ ��Χ[0,128]
    unsigned int              unHistGramValue;          // ֱ��ͼY���������Զ�,ֵԽ��ͼ��Ҷȷֲ�Խ��ֵԽС�Ҷȷֲ�Խ����,��Χ[0,32]
} CFG_LCE_STATE_INFO;

// ͳ��������
typedef enum tagCFG_STATISTIC_TYPE
{
	CFG_STATISTIC_TYPE_UNKNOWN,
	CFG_STATISTIC_TYPE_VAL, // ����ֵ
	CFG_STATISTIC_TYPE_MAX, // ���
	CFG_STATISTIC_TYPE_MIN, // ��С
	CFG_STATISTIC_TYPE_AVR, // ƽ��
	CFG_STATISTIC_TYPE_STD, // ��׼
	CFG_STATISTIC_TYPE_MID, // �м�
	CFG_STATISTIC_TYPE_ISO, // ISO
} CFG_STATISTIC_TYPE;

// �Ƚ�������
typedef enum tagCFG_COMPARE_RESULT
{
	CFG_COMPARE_RESULT_UNKNOWN,
	CFG_COMPARE_RESULT_BELOW, // ����
	CFG_COMPARE_RESULT_MATCH, // ƥ��
	CFG_COMPARE_RESULT_ABOVE, // ����
} CFG_COMPARE_RESULT;

// ���µ㱨������
typedef struct tagRADIOMETRY_ALARMSETTING 
{
	int                         nId;                    // ����Ψһ���	�������ͳһ����
	BOOL                        bEnable;                // �Ƿ����õ㱨��
    int                         nResultType;            // ���±���������ͣ��� CFG_STATISTIC_TYPE����ȡֵ��
                                                        // ����£�����ֵ��
                                                        // �߲��£����, ��С, ƽ��
                                                        // ������£����, ��С, ƽ��, ��׼, �м�, ISO
	int                         nAlarmCondition;        // ������������ CFG_COMPARE_RESULT
	float                       fThreshold;             // ������ֵ�¶�	������
	float                       fHysteresis;            // �¶���������������0.1 ��ʾ���������0.1��Χ��
	int                         nDuration;              // ��ֵ�¶ȳ���ʱ��	��λ����
} CFG_RADIOMETRY_ALARMSETTING;

// ���¹��򱾵ز�������
typedef struct tagRADIOMETRY_LOCALPARAM
{
	BOOL                        bEnable;                // �Ƿ����ñ�������
	float                       fObjectEmissivity;      // Ŀ�����ϵ��	������ 0~1
	int                         nObjectDistance;        // Ŀ�����	
	int                         nRefalectedTemp;        // Ŀ�귴���¶�
} CFG_RADIOMETRY_LOCALPARAM;

// ������µ�������
typedef enum tagEM_CFG_AREA_SUBTYPE
{
	EM_CFG_AREA_SUBTYPE_UNKNOWN,		
	EM_CFG_AREA_SUBTYPE_RECT,		// ����
	EM_CFG_AREA_SUBTYPE_ELLIPSE,	// ��Բ
	EM_CFG_AREA_SUBTYPE_POLYGON,	// �����
} EM_CFG_AREA_SUBTYPE;

// ���¹���
typedef struct tagCFG_RADIOMETRY_RULE
{
	BOOL                        bEnable;                // ����ʹ��
	int                         nPresetId;              // Ԥ�õ���
	int                         nRuleId;                // ������
	char                        szName[128];            // �Զ�������
	int                         nMeterType;             // ����ģʽ�����ͣ��� NET_RADIOMETRY_METERTYPE
	CFG_POLYGON                 stCoordinates[64];      // ���µ�����	ʹ�����������ϵ��ȡֵ��Ϊ0~8191
	int                         nCoordinateCnt;         // ���µ�����ʵ�ʸ���
	int                         nSamplePeriod;          // �¶Ȳ�������	��λ : ��
	CFG_RADIOMETRY_ALARMSETTING stAlarmSetting[64];     // ���µ㱨������
	int                         nAlarmSettingCnt;       // ���µ㱨������ʵ�ʸ���
	CFG_RADIOMETRY_LOCALPARAM   stLocalParameters;      // ���ز�������
	EM_CFG_AREA_SUBTYPE			emAreaSubType;			// ������µ�������			
} CFG_RADIOMETRY_RULE;

// ���¹������ýṹ
typedef struct tagCFG_RADIOMETRY_RULE_INFO
{
    int                         nCount;                 // �������
    CFG_RADIOMETRY_RULE         stRule[512];			// ���¹���
} CFG_RADIOMETRY_RULE_INFO;

// �¶�ͳ��
typedef struct tagCFG_TEMP_STATISTICS
{
	BOOL                        bEnable;                // �Ƿ����¶�ͳ��
	char                        szName[128];            // �����������
	int                         nMeterType;             // ����ģʽ�����ͣ��� NET_RADIOMETRY_METERTYPE
	int                         nPeriod;                // �����¶���������
} CFG_TEMP_STATISTICS;

// �¶�ͳ�����ýṹ
typedef struct tagCFG_TEMP_STATISTICS_INFO
{
	int                         nCount;                 // ����
	CFG_TEMP_STATISTICS         stStatistics[64];       // �¶�ͳ��
} CFG_TEMP_STATISTICS_INFO;

// �¶ȵ�λ
typedef enum tagCFG_TEMPERATURE_UNIT
{
	TEMPERATURE_UNIT_UNKNOWN,
	TEMPERATURE_UNIT_CENTIGRADE, // ���϶�
	TEMPERATURE_UNIT_FAHRENHEIT, // ���϶�
} CFG_TEMPERATURE_UNIT;

// �ȳ������ȫ������
typedef struct tagCFG_THERMOMETRY_INFO
{
    int                         nRelativeHumidity;          // ���ʪ��
    float                       fAtmosphericTemperature;    // �����¶�
    float                       fObjectEmissivity;          // �������ϵ��
    int                         nObjectDistance;            // �������
    float                       fReflectedTemperature;      // ���巴���¶�
    int                         nTemperatureUnit;           // �¶ȵ�λ���� TEMPERATURE_UNIT
    BOOL                        bIsothermEnable;            // ɫ�깦��ʹ��
    int                         nMinLimitTemp;              // �����������¶�ֵ
    int                         nMediumTemp;                // ��������λ�¶�ֵ	
    int                         nMaxLimitTemp;              // �����������¶�ֵ
    int                         nSaturationTemp;            // �����߱����¶�ֵ
    CFG_RECT                    stIsothermRect;             // ɫ������������OSD λ�ã���ʹ�����������ϵ��ȡֵ��Ϊ0-8191
    BOOL                        bColorBarDisplay;           // �Ƿ���ʾɫ������OSD ���ӣ�
    BOOL                        bHotSpotFollow;             // �Ƿ����ȵ�̽��׷��ʹ��
    BOOL                        bTemperEnable;              // ���¿���
    CFG_RGBA                    stHighCTMakerColor;         // ��ɫ�±�ע��ɫ
    CFG_RGBA                    stLowCTMakerColor;          // ��ɫ�±�ע��ɫ
} CFG_THERMOMETRY_INFO;

// �������Ϣ
typedef struct tagCFG_NEARLIGHT_INFO
{
    BOOL                bEnable;                // �Ƿ�ʹ�ܣ�TRUEʹ�ܣ�FALSE��ʹ��
    DWORD               dwLightPercent;         // �ƹ����Ȱٷֱ�ֵ(0~100)
    DWORD               dwAnglePercent;         // �ƹ�ǶȰٷֱ�ֵ(0~100)
}CFG_NEARLIGHT_INFO;

// Զ�����Ϣ
typedef struct tagCFG_FARLIGHT_INFO 
{
    BOOL                bEnable;                // �Ƿ�ʹ�ܣ�TRUEʹ�ܣ�FALSE��ʹ��
    DWORD               dwLightPercent;         // �ƹ����Ȱٷֱ�ֵ(0~100)
    DWORD               dwAnglePercent;         // �ƹ�ǶȰٷֱ�ֵ(0~100)
}CFG_FARLIGHT_INFO;

// �ƹ�ģʽ
typedef enum tagEM_CFG_LIGHTING_MODE
{
    EM_CFG_LIGHTING_MODE_UNKNOWN,               // δ֪
    EM_CFG_LIGHTING_MODE_MANUAL,                // �ֶ�
    EM_CFG_LIGHTING_MODE_ZOOMPRIO,              // ��������
    EM_CFG_LIGHTING_MODE_TIMING,                // ��ʱģʽ
    EM_CFG_LIGHTING_MODE_AUTO,                  // �Զ�
    EM_CFG_LIGHTING_MODE_OFF,                   // �ر�ģʽ
}EM_CFG_LIGHTING_MODE;

#define MAX_LIGHTING_NUM         16

// �ƹ���������
typedef struct tagCFG_LIGHTING_DETAIL
{
    int                                 nCorrection;                     // �ƹⲹ�� (0~4) ��������ʱ��Ч
    int                                 nSensitive;                      // �ƹ�������(0~5)��������ʱ��Ч��Ĭ��Ϊ3
    EM_CFG_LIGHTING_MODE                emMode;                          // �ƹ�ģʽ
    int                                 nNearLight;                      // �������Ч����
    CFG_NEARLIGHT_INFO                  stuNearLights[MAX_LIGHTING_NUM]; // ������б�
    int                                 nFarLight;                       // Զ�����Ч����
    CFG_FARLIGHT_INFO                   stuFarLights[MAX_LIGHTING_NUM];  // Զ����б�
}CFG_LIGHTING_DETAIL;

#define MAX_LIGHTING_DETAIL_NUM  16

// �ƹ�����(��Ӧ CFG_CMD_LIGHTING ����)
typedef struct tagCFG_LIGHTING_INFO
{
    int                         nLightingDetailNum;                         // �ƹ�������Ч����
    CFG_LIGHTING_DETAIL         stuLightingDetail[MAX_LIGHTING_DETAIL_NUM]; // �ƹ�������Ϣ�б�
}CFG_LIGHTING_INFO;

// �ƹ�ƻ�����(��Ӧ CFG_CMD_LIGHTINGSCHEDULE ����)���� CFG_CMD_LIGHTING ������ModeΪTimingʱ��Ч
typedef struct tagCFG_LIGHTINGSCHEDULE_INFO
{
    CFG_TIME_SCHEDULE           stuTimeSchedule;                            // �����ƻ�
}CFG_LIGHTINGSCHEDULE_INFO;

// ��ˢ����ģʽ
typedef enum tagEM_CFG_RAINBRUSHMODE_MODE
{
    EM_CFG_RAINBRUSHMODE_MODE_UNKNOWN,          // δ֪
    EM_CFG_RAINBRUSHMODE_MODE_MANUAL,           // �ֶ�ģʽ
    EM_CFG_RAINBRUSHMODE_MODE_TIMING,           // ��ʱģʽ
	EM_CFG_RAINBRUSHMODE_MODE_AUTO,				// �Զ���Ӧģʽ
}EM_CFG_RAINBRUSHMODE_MODE;

// ��ˢʹ�ܵ�ƽģʽ
typedef enum tagEM_CFG_RAINBRUSHMODE_ENABLEMODE
{
    EM_CFG_RAINBRUSHMODE_ENABLEMODE_UNKNOWN,     // δ֪
    EM_CFG_RAINBRUSHMODE_ENABLEMODE_LOW,         // �͵�ƽ��Ч�����գ�
    EM_CFG_RAINBRUSHMODE_ENABLEMODE_HIGH,        // �ߵ�ƽ��Ч��������
}EM_CFG_RAINBRUSHMODE_ENABLEMODE;

// ��ˢģʽ�������(��Ӧ CFG_RAINBRUSHMODE_INFO ����)
typedef struct tagCFG_RAINBRUSHMODE_INFO
{
    EM_CFG_RAINBRUSHMODE_MODE       emMode;         // ��ˢ����ģʽ
    EM_CFG_RAINBRUSHMODE_ENABLEMODE emEnableMode;   // ��ˢʹ�ܵ�ƽģʽ
    int                             nPort;          // ��ˢʹ�õ�IO�˿�,-1��ʾδ�����豸,-2��ʾ���ֶ���Ч���豸δ���͸��ֶΣ�
	int								nSensitivity;	// ��ˢ������, ֻ�е�modeΪAutoʱ��Ч, ��Χ[0, 10]
}CFG_RAINBRUSHMODE_INFO;

#define MAX_EMERGENCY_REOCRD_CLIENT_NUM         8       // ���Խ���¼��洢�����ͻ�����

// �ͻ��˵Ľ���¼��������Ϣ
typedef struct tagCFG_EMERGENCY_RECORD_CLIENT 
{
    char        szDevID[CFG_COMMON_STRING_128];         // �ͻ����ַ�����ʶ��Ϊ�������ƣ������ǿͻ���mac��ַ�����ͻ������豸����ʹ���豸���к�
    BOOL        bEnable;                                // ʹ��
    int         nMaxTime;                               // �����쳣�Ͽ�֮����н����洢�����ʱ��, 
}CFG_EMERGENCY_RECORD_CLIENT;

// ����¼��洢����(��Ӧ CFG_CMD_EMERGENCY_RECORD_FOR_PULL)
typedef struct tagCFG_EMERGENCY_RECORD_FOR_PULL_INFO 
{
    int         nClientNum;                             // ��Ч�ͻ��˽���¼�����ø���
    CFG_EMERGENCY_RECORD_CLIENT stuEmRecordInfo[MAX_EMERGENCY_REOCRD_CLIENT_NUM];// �ͻ��˽���¼��������Ϣ 
}CFG_EMERGENCY_RECORD_FOR_PULL_INFO;

// ��Ƶ�α���
typedef struct tagCFG_HIGH_FREQUENCY 
{
    int         nPeriod;                            // ͳ������, ����Ϊ��λ, Ĭ��30����(1800s)
    int         nMaxCount;                          // �ڶ�Ӧͳ����������������ϱ�������
}CFG_HIGH_FREQUENCY;

// �澯���ι�������(��Ӧ CFG_CMD_ALARM_SHIELD_RULE)
typedef struct tagCFG_ALARM_SHIELD_RULE_INFO 
{
    CFG_HIGH_FREQUENCY  stuHighFreq;                // ��Ƶ�α���, ��һ�������������ϱ��ı����������Դ˹��˶��ڱ�����Ƶ���ϱ�������Ϣ����
}CFG_ALARM_SHIELD_RULE_INFO;

#define CFG_MAX_VIDEOIN_ANALYSER_NUM        3       // ÿ����Ƶͨ���ڵ������Ƶ����������
#define CFG_MAX_ANALYSE_RULE_GROUP_NUM      8       // ��Ƶ�����������������

// ���ܷ���������, һ�����а���ͬ���͵Ķ�������
typedef struct tagCFG_ANALYSERULE_GROUP
{
    DWORD				dwRuleType;					// �������ͣ����dhnetsdk.h��"���ܷ����¼�����"
    int                 nRuleStructSize;            // �������Ͷ�Ӧ�Ĺ���ṹ�Ĵ�С, parseʱ���������, packetʱ���������
    BOOL                bEnable;                    // ʹ��
    BOOL                bEventHandlerEnable;        // ��������ʹ��, TRUE�����鱨������ʹ��, �������й���ʹ�ù�ͬ��������; FALSEʹ�ù����и��Ե�������
    CFG_ALARM_MSG_HANDLE stuEventHandler;			// �����鱨������
    CFG_TIME_SCHEDULE   stuEventTimeSection;        // �����鱨������ʱ���
    int                 nRuleNum;                   // ��������, parseʱ���������, packetʱ���������
    char*               pRuleBuf;                   // ���򻺳�, ��Ҫ�û������ڴ�, ���ڴ�Ź�����Ϣ
                                                    // ���������Ϊ������, pRuleBuf�����ݾ��Ƕ��CFG_CROSSLINE_INFO
    int                 nRuleBufSize;               // ���򻺳��С, �û���д
}CFG_ANALYSERULE_GROUP;

// ��Ƶ��������Ϣ
typedef struct tagCFG_VIDEO_IN_ANALYSER_INFO
{
    CFG_ANALYSEGLOBAL_INFO  stuGlobal;              // ȫ������
    CFG_ANALYSEMODULES_INFO stuModules;             // ģ������
    CFG_ANALYSERULE_GROUP	stuRuleGroup[CFG_MAX_ANALYSE_RULE_GROUP_NUM]; // ��������, ������Ԫ�صĹ��򻺳���Ҫ�û������ڴ�
    int                     nRuleGroupNum;          // ����������
}CFG_VIDEOIN_ANALYSER_INFO;

// ��Ƶͨ���ڵ����ܹ���, �ýṹ��ܴ�, ʹ��ʱ�ڶ��з����ڴ�
typedef struct tagCFG_VIDEO_IN_ANALYSE_RULE_INFO
{
    int                     nAnalyserNum;                               // ����������
    CFG_VIDEOIN_ANALYSER_INFO stuAnalysers[CFG_MAX_VIDEOIN_ANALYSER_NUM]; // ��������Ϣ
}CFG_VIDEOIN_ANALYSE_RULE_INFO;

typedef enum tagEM_CFG_ACCESS_WORD_MODE
{
    EM_CFG_ACCESS_WORD_MODE_UNKNOWN,                // δ֪
    EM_CFG_ACCESS_WORD_MODE_NORMAL,                 // "Normal" ����ģʽ
    EM_CFG_ACCESS_WORD_MODE_CLEAN,                  // "Clean"  ��ɨģʽ
    EM_CFG_ACCESS_WORD_MODE_DECORATION,             // "Decoration"  װ��ģʽ
    EM_CFG_ACCESS_WORD_MODE_UNUSED,                 // "Unused" ͣ��ģʽ
    EM_CFG_ACCESS_WORD_MODE_OUTSTANDING,            // "Outstanding" Ƿ��ģʽ
}EM_CFG_ACCESS_WORD_MODE;

// ��������ģʽ(��Ӧ CFG_CMD_ACCESS_WORK_MODE)
typedef struct tagCFG_ACCESS_WORK_MODE_INFO 
{
    EM_CFG_ACCESS_WORD_MODE  emMode;                // ����ģʽ
}CFG_ACCESS_WORK_MODE_INFO;

// �Խ�Զ�̷�ʱ������
typedef struct tagCFG_VIDEO_TALK_TIME_LIMIT
{
    int                                 nMaxRingingTime;      // �����������ʱ������ʱ�Զ��Ҷ�, 1-600��
    int                                 nMaxConnectingTime;   // ���ͨ��ʱ������ʱ�Զ��Ҷ�, 1-600����
    UINT                                nMaxLeaveWordTime;    // ����Զ�����ʱ��,��ʱ�Զ�������1-600��
} CFG_VIDEO_TALK_TIME_LIMIT;

// Զ���豸����
typedef enum tagEM_CFG_VT_TYPE
{
    EM_CFG_VT_TYPE_VTH = 0,                                   // VTH
    EM_CFG_VT_TYPE_VTO,                                       // VTO
    EM_CFG_VT_TYPE_VTS,                                       // VTS
    EM_CFG_VT_TYPE_MAX,
} EM_CFG_VT_TYPE;

// ��Ƶ�Խ��绰ͨ������(CFG_CMD_VIDEO_TALK_PHONE_GENERAL)
typedef struct tagCFG_VIDEO_TALK_PHONE_GENERAL
{
    char                                szRingFile[MAX_PATH];               // �����ļ�·��
    int                                 nRingVolume;                        // ��������, 0-100
    int                                 nTalkVolume;                        // ͨ������, 0-100
    CFG_NET_TIME                        stuDisableRingStartTime;            // ���ſ�ʼʱ��
    CFG_NET_TIME                        stuDisableRingEndTime;              // ���Ž���ʱ��
    CFG_VIDEO_TALK_TIME_LIMIT           stuTimeLimit[EM_CFG_VT_TYPE_MAX];   // ÿ��Ԫ�ض�Ӧһ��Զ���豸���ͣ���ǰֻ��VTS��Ч
                                                                            // 0-VTH,1-VTO,2-VTS
    int                                 nMaxMonitorTime;                    // �������ʱ������ʱ�Զ�����, 1-600����
    int                                 nMaxRecordTime;                     // ¼�����ʱ������ʱ�Զ�����, 15-300��
    BOOL                                bSnapEnable;                        // ��������ץͼʹ��
                                                                            // ץͼ���ϴ�·����������RecordStoragePoint��EventSnapShot
    BOOL                                bRingSoundEnable;                   // ��������ʹ��
    char                                szVthRingFile[CFG_COMMON_STRING_128];   // ���ڻ������ڻ����е������ļ�
    UINT                                nVthRingVolume;                     // ���ڻ������ڻ�������������
    BOOL                                bLeaveMsgSoundEnable;               // ��Ӱ������ʾ��ʹ��
    BOOL                                bSilenceEnable;                     // �Ƿ���
    UINT                                nMaxMonitorIPCTime;                 // IPC�������ʱ������ʱ�Զ�������0-10Сʱ,��λΪ��
    UINT                                nReviseTime;                        // ����ʱ��ʹ�ã���ϵͳʱ��ı��ʱ��,�������ʱ���������µ����Ž���ʱ��,��λ��
    BOOL                                bTalkRecordUpload;                  // ���м�¼�Ƿ��ϴ�
    UINT                                nSnapShotCount;                     // ����ץ��ͼƬ������1-5��
    BOOL                                bRemoteControl;                     // �Ƿ����Զ�̿��Ʋ���
                                                                            // trueʱ,�ڿͻ��˿�Զ�̲����豸�����ܼҾ��豸���豸�Ĳ�����������
    BOOL                                bLeaveMessageEnable;                // ���Լ�¼�Ƿ���
    BOOL                                bLeaveMessageUpload;                // ���Լ�¼�Ƿ��ϴ�
    BOOL                                bLocalVideoEnable;                  // ������Ƶ�Ƿ���
    BOOL                                bPublishInfoOverlayingEnable;       // ������Ϣ����ʹ�ܿ���                                 
} CFG_VIDEO_TALK_PHONE_GENERAL;

// ץͼ�ϳ�����
typedef struct tagCFG_TRAFFIC_SNAP_MOSAIC_INFO
{
    BOOL                                bEnable;                            // �Ƿ���ͼƬ�ϳ�
    int                                 nPicCompressRate;                   // ͼƬѹ���ȣ�10-100
    BOOL                                bUploadNormalSnap;                  // �Ƿ��ϴ�ԭʼͼƬ
} CFG_TRAFFIC_SNAP_MOSAIC_INFO;

// ��ͨץ�Ĵ���
#define TRAFFIC_SNAP_COUNT 4

// ץ�����ã���Ӧһ������
typedef struct tagCFG_SNAPSHOT_RULE
{
    int                                 nType;                              // �� dhnetsdk.h �С����ܷ����¼����͡��궨��
    int                                 nRepeatInterval;                    // 1~1440����λ�����ӣ�Ĭ��1440
    int                                 nSnapShotType[TRAFFIC_SNAP_COUNT];  // ץ���������飬0��δ���壬1��������2��Զ��
    int                                 nSingleInterval[TRAFFIC_SNAP_COUNT];// ץͼʱ�������飬
                                                                            // ��һ��ʱ�䣺5~180 Ĭ��10 ��Υ��ͣ��������Ч����λ��
                                                                            // �ڶ��������ĸ�ʱ�䣺1��~60��Ĭ��20��
} CFG_SNAPSHOT_RULE;

// ץ�����ã���Ӧһ������
typedef struct tagCFG_SCENE_SNAPSHOT_RULE
{
    int                                 nPresetID;                          // ����Ԥ�õ��
    int                                 nCount;                             // �������
    CFG_SNAPSHOT_RULE                   stRule[32];                         // ����������
} CFG_SCENE_SNAPSHOT_RULE;

// ����ץ������
typedef struct tagCFG_SCENE_SNAPSHOT_RULE_INFO
{
    int                                 nCount;                             // ��������
    CFG_SCENE_SNAPSHOT_RULE             stScene[32];                        // ����������
} CFG_SCENE_SNAPSHOT_RULE_INFO;

#define CFG_MAX_PTZTOUR_NUM             64          // Ѳ��·������
#define CFG_MAX_PTZTOUR_PRESET_NUM      64          // Ѳ��·��������Ԥ�õ�����

// Ѳ��·���е�Ԥ�õ�
typedef struct tagCFG_PTZTOUR_PRESET 
{
    int             nPresetID;              // Ԥ�õ���
    int             nDuration;              // �ڸ�Ԥ�õ��ͣ��ʱ��, ��λ��
    int             nSpeed;                 // �����Ԥ�õ��ת���ٶ�, 1~10
} CFG_PTZTOUR_PRESET;

// Ѳ��·��
typedef struct tagCFG_PTZTOUR_SINGLE 
{
    BOOL                        bEnable;                                    // ʹ��
    char                        szName[CFG_COMMON_STRING_64];               // ����
    int                         nPresetsNum;                                // Ԥ�õ�����
    CFG_PTZTOUR_PRESET          stPresets[CFG_MAX_PTZTOUR_PRESET_NUM];      // ��·��������Ԥ�õ����
} CFG_PTZTOUR_SINGLE;

// ��̨Ѳ��·������
typedef struct tagCFG_PTZTOUR_INFO 
{
    int                         nCount;                                     // Ѳ��·������
    CFG_PTZTOUR_SINGLE          stTours[CFG_MAX_PTZTOUR_NUM];               // Ѳ��·��, ÿ��ͨ����������Ѳ��·��
} CFG_PTZTOUR_INFO;

// �ſڻ�����
typedef enum tagEM_CFG_VTO_TYPE
{
    EM_CFG_VTO_TYPE_UNKNOW =0   , //δ֪
    EM_CFG_VTO_TYPE_VTO         , //��Ԫ�ſڻ�
    EM_CFG_VTO_TYPE_WALL        , //Χǽ��
	EM_CFG_VTO_TYPE_MAX			,
}EM_CFG_VTO_TYPE;

// �ſڻ���Ϣ
typedef struct tagCFG_VTO_INFO 
{
    BOOL            bEnable;                                    // ʹ��
    char            szID[CFG_COMMON_STRING_64];                 // �ſڻ���ʶ
    char            szIP[CFG_COMMON_STRING_64];                 // IP��ַ
    unsigned int    nPort;                                      // �˿�
    EM_CFG_VTO_TYPE emType;                                     // �ſڻ�����
    char            szMidNum[CFG_COMMON_STRING_32];             // �ſڻ��к�
    char            szMachineAddress[CFG_COMMON_STRING_128];    // �ſڻ�����λ��
    char            szLoginUsername[CFG_COMMON_STRING_32];      // ������¼�û���
    char            szLoginPassWord[CFG_COMMON_STRING_32];      // ������¼����    
    char            szRingFile[CFG_COMMON_STRING_256];          //�ſڻ��������ڻ������ڻ��������ļ�	
    int             nRingVolume;                                // ���ڻ���������С
}CFG_VTO_INFO;

// �ſڻ���Ϣ�б�
typedef struct tagCFG_VTO_LIST 
{
    int             nVTONum;                            // �ſڻ�����
    CFG_VTO_INFO    stuVTOInfos[CFG_MAX_VTO_NUM];       // �ſڻ���Ϣ
}CFG_VTO_LIST;

//�����������
typedef struct tagCFG_TS_POINT
{
    unsigned int     nX;     //���귶Χ[0 - 65535]
    unsigned int     nY;     //���귶Χ[0 - 65535]
}CFG_TS_POINT;

//������У׼����
typedef struct tagCFG_TSPOINT_INFO
{
    CFG_TS_POINT         stDisplay[TS_POINT_NUM];         //��ʾ����,����ֻ֧��3����
    CFG_TS_POINT         stScreen[TS_POINT_NUM];          //��Ļ����,����ֻ֧��3����
}CFG_TSPOINT_INFO;

//���ڻ�����
typedef enum tagEM_CFG_VTH_TYPE
{
    EM_CFG_VTH_TYPE_UNKNOW=0 ,   // δ֪
    EM_CFG_VTH_TYPE_MAIN     ,   // �����ڻ�
    EM_CFG_VTH_TYPE_SUB      ,   // �����ڻ�
}EM_CFG_VTH_TYPE;

//�豸ʹ�ó���
typedef enum tagEM_CFG_VTH_APPTYPE
{
    EM_CFG_VTH_APPTYPE_UNKNOW=0 , //δ֪
    EM_CFG_VTH_APPTYPE_COMMON   , //�������ڻ�
    EM_CFG_VTH_APPTYPE_MANAGER  , //��������
}EM_CFG_VTH_APPTYPE;

//���ڻ�������Ϣ
typedef struct tagCFG_VTH_NUMBER_INFO
{
    char                szShortNumber[CFG_COMMON_STRING_32];    // �̺� ��Χ[1,9999],��ʾ��ͬ���ڻ�;��������ڷֻ�,�̺���Ϊ"9901-N" NΪ1,2,3.
    char                szMiddleNumber[CFG_COMMON_STRING_32];   // �к� ��ʽ:С����(1λ)������(2λ)����Ԫ��(1λ)���̺�(4λ)
    char                szVirtualNumber[CFG_COMMON_STRING_32];  // ���ڻ���� �������ʱ,���豸����ĺ��룬��ͬ�ֻ�����
    EM_CFG_VTH_TYPE     emVthType;                              // ���ڻ�����
    char                szMainNumber[CFG_COMMON_STRING_32];     // ������� ��Χ[1,9999] emVthTypeΪEM_CFG_VTH_TYPE_SUBʱ��Ч 
    char                szSubNumber[CFG_COMMON_STRING_32];      // �ֻ���   ��Χ[1,5]    emVthTypeΪ��EM_CFG_VTH_TYPE_SUBʱ��Ч
    char                szMainDeviceIP[CFG_COMMON_STRING_64];   // �����ڻ�IP  ���������ڵ�����ͬ���������ڻ� emVthType ��EM_CFG_VTH_TYPE_SUBʱ��Ч��
    EM_CFG_VTH_APPTYPE  emVthAppType;                           // �豸ʹ�ó���
	int					nLockState;								// ����״̬��0��ʾ������,Ĭ��ֵ; 1��ʾ����
	char			    szUsername[MAX_USERNAME_LEN];			// �����û���
	char				szPassword[MAX_PASSWORD_LEN];			// ��������
}CFG_VTH_NUMBER_INFO;

// GPS����ģʽ
typedef enum tagEM_CFG_GPS_MODE
{
    EM_CFG_GPS_MODE_UNKNOWN,
    EM_CFG_GPS_MODE_GPS,                                                    // GPS
    EM_CFG_GPS_MODE_BEIDOU,                                                 // BEIDOU
    EM_CFG_GPS_MODE_GLONASS,                                                // GLONASS
    EM_CFG_GPS_MODE_MIX,                                                    // MIX
    EM_CFG_GPS_MODE_END,                                                    // �����ڱ�ʶ����
} EM_CFG_GPS_MODE;

// ƽ̨����
typedef enum tagEM_CFG_TRANSFER_PLATFORM
{
    EM_CFG_TRANSFER_PLATFORM_UNKNOWN,
    EM_CFG_TRANSFER_PLATFORM_DSS,                                           // DSS
    EM_CFG_TRANSFER_PLATFORM_JTBB,                                          // JTBB
    EM_CFG_TRANSFER_PLATFORM_END,                                           // �����ڱ�ʶ����
} EM_CFG_TRANSFER_PLATFORM;

// ����GPS����
typedef struct tagCFG_GPS_INFO
{
    BOOL                        bEnable;                                    // �Ƿ�ʹ��
    int                         nSampleInterval;                            // Ӳ���ɼ�����, ��λ����
    int                         nNotifyInterval;                            // Ӧ�ó���֪ͨ����, ��λ����
    BOOL                        bSyncTime;                                  // �Ƿ�GPSʱ��ͬ�����豸
    EM_CFG_GPS_MODE             emCurMode;                                  // ģ�鵱ǰ����ģʽ
    int                         nSupportModes;                              // ֧�ֵĶ�λģʽ����
    EM_CFG_GPS_MODE             emSupportModes[EM_CFG_GPS_MODE_END];        // ģ��֧�ֵĶ�λģʽ
    char                        szCurModule[CFG_COMMON_STRING_64];          // ��ǰģ�鳧�����ͱ�־
    BOOL                        bShowLocation;                              // �Ƿ�֧�ֵ���λ����ʾ
    int                         nSaveDays;                                  // GPS�������������ڲ���, ��λ���죬��Χ��0-365
    int                         nTransferPlatform;                          // ֧�ֲ�����ƽ̨����
    EM_CFG_TRANSFER_PLATFORM    emTransferPlatform[EM_CFG_TRANSFER_PLATFORM_END];   // ֧�ֲ�����ƽ̨�����ݽ��շ�
} CFG_GPS_INFO;

#define CFG_GPS_INFO_MAX        16

// GPS����
typedef struct tagCFG_GPS_INFO_ALL
{
    int                         nGps;                                       // GPS����
    CFG_GPS_INFO                stGps[CFG_GPS_INFO_MAX];                    // GPS����, ÿ��GPS�豸��Ӧһ������
} CFG_GPS_INFO_ALL;


// VTO�豸����
typedef enum tagNET_CFG_VTO_TYPE
{
	NET_CFG_VTO_TYPE_UNKNOWN = 0,							// δ֪
	NET_CFG_VTO_TYPE_VILLA,									// 1 ������
	NET_CFG_VTO_TYPE_DOOR,                                  // 2 �ſڻ�
	NET_CFG_VTO_TYPE_LADDER,                                // 3 �ݿػ�
    NET_CFG_VTO_TYPE_APART,                                 // 4 ģ�黯�ſڻ�
    NET_CFG_VTO_TYPE_SECOND,                                // 5 ����ȷ�ϻ�
    NET_CFG_VTO_TYPE_FACE,                                  // 6 ����¼���
    NET_CFG_VTO_TYPE_CONTROLLER,                            // 7 ������
	NET_CFG_VTO_TYPE_MAX,
} NET_CFG_VTO_TYPE;

// �ſڻ�����

//VTO������Ϣ
typedef struct tagCFG_VTO_BASIC_INFO
{
	 char			  szNumber[AV_CFG_DeviceNo_Len];	// �ſڻ����
	 NET_CFG_VTO_TYPE emCfgVtoType;						// �豸����
	 EM_CFG_VTO_TYPE  emType;							// �ſڻ����� 1��Ԫ�ſڻ� 2Χǽ��
	 char			  szAnalogVersion[CFG_COMMON_STRING_64]; // ģ��ϵͳ�汾
	 BOOL			  bFaceDetect;						// ����ʶ��ʹ 1���� 0�ر�
	 int			  nPositon;							// VTO����¥��λ�� Int32, 0 ��ʾ��Ч												
														// 1,2,����ʾ����һ�㣬���� -1,-2,����ʾ����һ�㣬����
}CFG_VTO_BASIC_INFO;

typedef enum tagEM_CFG_SHORTCUT_CALL_POSITION_TYPE
{
    EM_CFG_SHORTCUT_CALL_POSITION_TYPE_UNKNOWN,         // δ֪
    EM_CFG_SHORTCUT_CALL_POSITION_TYPE_SUPERMARKET,     // ����
    EM_CFG_SHORTCUT_CALL_POSITION_TYPE_RESTAURANT,      // ����
    EM_CFG_SHORTCUT_CALL_POSITION_TYPE_HOSPITAL,        // ҽԺ
    EM_CFG_SHORTCUT_CALL_POSITION_TYPE_MANAGER,         // ��������
    EM_CFG_SHORTCUT_CALL_POSITION_TYPE_CUSTOM,          // �Զ���
    EM_CFG_SHORTCUT_CALL_POSITION_TYPE_FRUITSTORE,      // ˮ����
}EM_CFG_SHORTCUT_CALL_POSITION_TYPE;

//��ݺ���Ϣ
typedef struct tagCFG_SHORTCUT_CALL
{
    char			                    szCallNumber[CFG_COMMON_STRING_64];	// ��ݺ�
    EM_CFG_SHORTCUT_CALL_POSITION_TYPE  emPositionType;						// �ص�����
}CFG_SHORTCUT_CALL;

//��ݺ�����
typedef struct tagCFG_SHORTCUT_CALL_INFO
{
    int                 nMaxNum;                        // ���ṹ��������� pShortcutCallInfo ָ��ṹ���������һ��
    CFG_SHORTCUT_CALL*  pShortcutCallInfo;              // ���ڴ洢��ݺ���Ϣ�Ľṹ��ָ�룬���û�����
    int                 nValidNum;                      // ��Ч�ṹ���������ȡʱ��sdk��д������ʱ���û���д
}CFG_SHORTCUT_CALL_INFO;

// ��¼��GPSLocation�İ汾��
typedef struct tagCFG_LOCATION_VER_INFO
{
    unsigned int        nVer;                           // �汾��
} CFG_LOCATION_VER_INFO;

// �豸�ɷ��ʵ�ַ�������� CFG_CMD_PARKING_SPACE_ACCESS_FILTER
typedef struct tagCFG_PARKING_SPACE_ACCESS_FILTER_INFO
{
    BOOL                    bEnable;                                                // ����ʹ��
    int                     nTrustListNum;                                          // ������IP����
    char                    szTrustList[CFG_MAX_TRUST_LIST][CFG_FILTER_IP_LEN];	    // ������ ��һ�����飬ÿһ����IP
} CFG_PARKING_SPACE_ACCESS_FILTER_INFO;

// ����ʱ������ CFG_CMD_WORK_TIME
typedef struct tagCFG_WORK_TIME_INFO
{
    BOOL                bEnable;                        // ΪTRUEʱ��������Ч���ǹ���ʱ���ϱ�DH_ALARM_BUS_DRIVE_AFTER_WORK�¼���ΪFALSEʱ��������Ч
    CFG_TIME            stWorkTimeBegin;                // ��ʼ����ʱ��
    CFG_TIME            stWorkTimeEnd;                  // ��������ʱ��
    DWORD               dwOutTimeBegin;                 // ��ʼ�����ά��ʱ��, utc��
    DWORD               dwOutTimeEnd;                   // ���������ά��ʱ��, utc��
} CFG_WORK_TIME_INFO;

// ��λ���״̬
typedef enum tagEM_CFG_LANE_STATUS
{
    EM_CFG_LANE_STATUS_UNKOWN = -1,                     // ״̬δ֪
    EM_CFG_LANE_STATUS_UNSUPERVISE = 0,                 // �����
    EM_CFG_LANE_STATUS_SUPERVISE = 1,                   // ���
}EM_CFG_LANE_STATUS;

#define MAX_LANES_NUM               64                  // �����ܳ�λ��������

// ������λָʾ�Ʊ������� 
typedef struct tagCFG_PARKING_SPACE_LIGHT_GROUP_INFO
{
    BOOL                bEnable;                            // ΪTRUEʱ��������Ч��ΪFALSEʱ��������Ч
    EM_CFG_LANE_STATUS  emLaneStatus[MAX_LANES_NUM];        // �����ܵĳ�λ���±��ʾ��λ��
    int                 nLanesNum;                          // ��Ч�ĳ�λ������������Ϊ��ܻ򲻼�ܵĳ�λ������
    BOOL                bAcceptNetCtrl;                     // �Ƿ����Զ�̿���    
}CFG_PARKING_SPACE_LIGHT_GROUP_INFO;

#define MAX_LIGHT_GROUP_INFO_NUM        8                   // ��λָʾ�Ʊ������õ�������

// ��λָʾ�Ʊ������� CFG_CMD_PARKING_SPACE_LIGHT_GROUP
typedef struct tagCFG_PARKING_SPACE_LIGHT_GROUP_INFO_ALL
{
    int                                         nCfgNum;                                            // ��ȡ�������ø���
    CFG_PARKING_SPACE_LIGHT_GROUP_INFO          stuLightGroupInfo[MAX_LIGHT_GROUP_INFO_NUM];        // ��λָʾ�Ʊ�������
}CFG_PARKING_SPACE_LIGHT_GROUP_INFO_ALL;

// �Զ�����Ƶ����(CFG_CMD_CUSTOM_AUDIO)
typedef struct tagCFG_CUSTOM_AUDIO 
{
    char                szPlateNotInDB[MAX_PATH];           // ����δ�����ݿ�ĸ澯��Ƶ·��
}CFG_CUSTOM_AUDIO;


// �״����á�������
typedef struct tagCFG_RADAR_CARSPEED 
{
    int                 nTriggerLower;                  // ����ֵ����
    int                 nTriggerUpper;                  // ����ֵ����
    int                 nLimitLower;                    // ����ֵ����
    int                 nLimitUpper;                    // ����ֵ����
} CFG_RADAR_CARSPEED;

// ���״����� CFG_CMD_DHRADER_PP
typedef struct tagCFG_DAHUA_RADAR
{
    int                 nAngle;                         // �Ƕȣ����������״�̽ͷ��װ�ĽǶ���ɵ��ٶ���� ��λ�ȣ���Χ0~45
    BOOL                bLowSpeed;                      // �Ƿ����õ����٣�Ϊ TRUE ʱ��CFG_RADAR_CARSPEED �ṹ�и�����ֵ��Ч
    BOOL                bSpeedForSize;                  // �Ƿ����ô�С�����٣�Ϊ FALSE ʱ��С���ٶ����ã�stuSmallCarSpeed����Ч
    CFG_RADAR_CARSPEED  stuSmallCarSpeed;               // С���ٶ����ã�bSpeedForSize Ϊ TRUE ʱ nTriggerLower ��ԱͬʱҲ�����г��ʹ󳵵Ĵ���ֵ����
    CFG_RADAR_CARSPEED  stuMediumCarSpeed;              // �г��ٶ����ã�bSpeedForSize Ϊ TRUE ʱ��Ч���� nTriggerLower ��Աʼ����Ч
    CFG_RADAR_CARSPEED  stuBigCarSpeed;                 // ���ٶ����ã�bSpeedForSize Ϊ TRUE ʱ��Ч���� nTriggerLower ��Աʼ����Ч
    char                szName[256];                    // �豸��װ��ַ
    int                 nSensitivity;                   // ������ ��Χ0~5��0���
    int                 nDetectMode;                    // ���ģʽ��ȡֵ��-1 ������ 0 ǰ������ 1 ǰ��ȥ�� 2 �������� 3 ����ȥ�� 4 ǰ��˫�� 5 ����˫��
} CFG_DAHUA_RADAR;

// �豸ͨ��wifiģ��ɨ����Χ�����豸���� CFG_CMD_WIFI_SEARCH
typedef struct tagCFG_WIFI_SEARCH_INFO
{
    BOOL                bEnable;                        // �Ƿ���Ч
    int                 nPeriod;                        // �¼��ϱ�����, ��λ��
	BOOL				bOptimizNotification;			// Ĭ��ֵ��false����falseʱ�����������豸״̬ÿ�ξ�ȫ���ϱ�,��trueʱ�����������豸״̬��δ����PD��OptimizationPeriodȡֵ����ʱ��ֻ�ϱ�����/�뿪���豸״̬������PD��OptimizationPeriodȡֵ����ʱ���������ϱ���������ȫ���豸״̬

} CFG_WIFI_SEARCH_INFO;

// �����豸ͨ��ģ��34G�л�ʹ������(CFG_CMD_G3G4AUTOCHANGE)       
typedef struct tagCFG_G3G4AUTOCHANGE
{
    BOOL                bEnable;                            // �Ƿ���3G��4G�Զ��л�����  ΪTRUEʱ��������Ч��ΪFALSEʱ��������Ч       
}CFG_G3G4AUTOCHANGE;


#define MAX_POS_CODE_LENGTH             32                                      // ˢ����У������󳤶�
#define MAX_ARRAY_POS_CODE              10                                      // ˢ����У��������������

// ˢ����У����У������(��Ӧ CFG_CMD_CHECKCODE)
typedef struct tagCFG_CHECKCODE_INFO 
{
    BOOL                    bEnable;                                            // ʹ��
    int                     nCodeCount;                                         // �������� ��Чֵ �ĸ���
    char                    szCode[MAX_ARRAY_POS_CODE][MAX_POS_CODE_LENGTH];    // ˢ����У��������
}CFG_CHECKCODE_INFO;

// �Ĵ��ƶ���������ƽ̨����(��Ӧ CFG_CMD_VSP_SCYDKD)
typedef struct tagCFG_VSP_SCYDKD_INFO
{
    BOOL                    bEnable;                            // ʹ��
    char                    szServerIP[128];                    // ע������ķ�����IP��ַ
    int                     nServerPort;                        // ע������ķ������˿ں�
    char                    szDeviceID[32];                     // ��Ƶ�����豸ID
    char                    szUserName[32];                     // ����ƽ̨�û���
}CFG_VSP_SCYDKD_INFO;

// ��̨������������
typedef struct tagCFG_PTZ_POWERUP_INFO 
{
    BOOL                    bEnable;                            // �����������ر�־
    int                     nFunction;                          // �������ܣ�-1 ��ʾ�����ϵ�ǰ�Ķ�����������ȡֵ�� EM_PTZ_ACTION
    int                     nScanId;                            // �Զ���ɨ���
    int                     nPresetId;                          // Ԥ�õ���
    int                     nPatternId;                         // �Զ�Ѳ�����
    int                     nTourId;                            // Ѳ�����
} CFG_PTZ_POWERUP_INFO;

// VTH��Զ��IPC����
#define MAX_REMOTE_IPC_NUM 64    // �������õ�Զ��IPC����

// Զ��IPCЭ������
typedef enum tagEM_CFG_REMOTE_IPC_DEVICE_PROTOCOL
{
    EM_CFG_REMOTE_IPC_DEVICE_PROTOCOL_UNKNOWN = 0,      // δ֪
    EM_CFG_REMOTE_IPC_DEVICE_PROTOCOL_HIKVISION,        // ����
    EM_CFG_REMOTE_IPC_DEVICE_PROTOCOL_PRIVATE3,         // ˽������Э��
}EM_CFG_REMOTE_IPC_DEVICE_PROTOCOL;

// Զ��IPC��������
typedef enum tagEM_CFG_REMOTE_IPC_DEVICE_STREAMTYPE
{
    EM_CFG_REMOTE_IPC_DEVICE_STREAM_TYPE_UNKNOWN = 0,      // δ֪
    EM_CFG_REMOTE_IPC_DEVICE_STREAM_TYPE_MAIN,             // ������
    EM_CFG_REMOTE_IPC_DEVICE_STREAM_TYPE_EXTRA1,           // ������
}EM_CFG_REMOTE_IPC_DEVICE_STREAMTYPE;

#define MAX_REMOTEIPCINFO_IPADDR_LEN        128     // Զ��IPC IP��ַ��󳤶�
#define MAX_REMOTEIPCINFO_MACADDR_LEN       64      // Զ��IPC MAC��ַ��󳤶�
#define MAX_REMOTEIPCINFO_USERNAME_LEN      128     // Զ��IPC�û�����󳤶�
#define MAX_REMOTEIPCINFO_USERPSW_LENGTH    128     // Զ��IPC������󳤶�

// Զ��IPC�����豸����
typedef enum tagEM_CFG_REMOTE_IPC_RELY_TYPE
{
    EM_CFG_REMOTE_IPC_RELY_TYPE_UNKNOWN = 0,      // δ֪
    EM_CFG_REMOTE_IPC_RELY_TYPE_IPC,              // IPC
    EM_CFG_REMOTE_IPC_RELY_TYPE_NVR,              // NVR
}EM_CFG_REMOTE_IPC_RELY_TYPE;

// Զ��IPC��Ϣ
typedef struct tagCFG_REMOTE_IPC_INFO
{
    char		                        szIP[MAX_REMOTEIPCINFO_IPADDR_LEN];	            // �豸IP
    char                                szMac[MAX_REMOTEIPCINFO_MACADDR_LEN];           // MAC��ַ
    char		                        szUser[MAX_REMOTEIPCINFO_USERNAME_LEN];	        // �û���
	char		                        szPassword[MAX_REMOTEIPCINFO_USERPSW_LENGTH];   // ����
	int	                                nPort;					                        // �˿�
	EM_CFG_REMOTE_IPC_DEVICE_PROTOCOL   emProtocol;                                     // Э������
	EM_CFG_REMOTE_IPC_DEVICE_STREAMTYPE emStreamType;                                   // ý��������
	int                                 nChannel;                                       // ͨ����
	EM_CFG_REMOTE_IPC_RELY_TYPE         emIPCRely;                                      // �����豸����
}CFG_REMOTE_IPC_INFO;

// VTH��Զ��IPC���ýṹ��, ��Ӧ CFG_CMD_VTH_REMOTE_IPC_INFO
// ��������ȫ�ֵģ�������ͨ��
typedef struct tagCFG_VTH_REMOTE_IPC_INFO
{
    int nRemoteIPCNum;                                              // Զ��IPC����
    CFG_REMOTE_IPC_INFO stuCfgRemoteIpcInfo[MAX_REMOTE_IPC_NUM];    // Զ��IPC��Ϣ
}CFG_VTH_REMOTE_IPC_INFO;

// �����о��ҹ��ʱ�������
typedef struct tagDAYNIGHT_INFO
{
    int                     nType;                              // �л����ͣ�1 - ���ӣ�2 - ��е��ͨ�������˹�Ƭ��ICR��
    int                     nMode;                              // ��ת��ģʽ��1-���ǲ�ɫ��2-�Զ������������л���3-���Ǻڰף�4-�������裬5-���������л���6-�ⲿ�������룬7�C�ⲿIO����
    int                     nSensitivity;                       // ��ת�������ȣ���ȡֵ 0~7
    int                     nDelay;                             // ��ҹģʽ�л���ʱʱ�䣬��ȡֵ3~30����λ��
} DAYNIGHT_INFO;

// �����о��ҹ����
typedef struct tagCFG_VIDEOIN_DAYNIGHT_INFO
{
    DAYNIGHT_INFO           stuSection[VIDEOIN_TSEC_NUM];       // ��ͬʱ������ã���˳��ֱ��Ӧ����ͨ�����졢��ҹ
} CFG_VIDEOIN_DAYNIGHT_INFO;

// ��������Ƶͨ�������
typedef struct tagCFG_AUDIO_MIX_CHANNEL_INFO
{
    int                    nChannelNum;                              // ��Ƶ����ͨ������
    int                    nCombination[32];                         // ��Ƶ����ͨ���б�
}CFG_AUDIO_MIX_CHANNEL_INFO;

// ���ö���ÿ������Ƶͨ�������(��Ӧ CFG_CMD_AUDIO_MIX_CHANNEL)
typedef struct tagCFG_AUDIO_MIX_CHANNEL_INFO_ALL
{

    int                           nMixChnInfoNum;                     // ����Ƶͨ�����ø���
    CFG_AUDIO_MIX_CHANNEL_INFO    stuMixChnInfo[16];                  // ����Ƶͨ������ 
}CFG_AUDIO_MIX_CHANNEL_INFO_ALL;

#define CFG_AUDIOTOUCH_PITCH_INVALID_VALUE    -999                    //���������е�nPitch����Чֵ
#define CFG_AUDIOTOUCH_PITCH_MIN_VALUE        -50                     //���������е�nPitch����Сֵ
#define CFG_AUDIOTOUCH_PITCH_MAX_VALUE         50                     //���������е�nPitch�����ֵ
// ������Ƶͨ����������
typedef struct tagCFG_AUDIO_TOUCH_INFO
{
    int                           nPitch;                             // ������ֵ -50~50,0��ʾ������  ����ͨ����û�б�������ʱ����nPitchΪ��Чֵ CFG_AUDIOTOUCH_INVALID_VALUE     
}CFG_AUDIO_TOUCH_INFO;

// ��Ƶͨ���������� (��Ӧ CFG_CMD_AUDIO_TOUCH)
typedef struct tagCFG_AUDIO_TOUCH_INFO_ALL
{
    BOOL                          bEnable;                            // ʹ��
    int                           nAudioTouchNum;                     // ��Ƶͨ���������ø���
    CFG_AUDIO_TOUCH_INFO          stuTouchInfo[64];                   // ��Ƶͨ����������
}CFG_AUDIO_TOUCH_INFO_ALL;

//�齹�������
typedef struct tagCFG_UNFOCUSDETECT_INFO
{
    BOOL                    bEnable;                        //�齹���ʹ�ܣ�TRUE ������FALSE �ر�
    int                     nSensitivity;                   //������    1~100
    CFG_ALARM_MSG_HANDLE    stuEventHandler;			    //��������
    CFG_TIME_SECTION	    stuTimeSection[WEEK_DAY_NUM][MAX_REC_TSECT];   // �¼���Ӧʱ��Σ�ʱ��λ�ȡ�������Դ˳�ԱΪ׼������ stuEventHandler �е� stuTimeSection
}CFG_UNFOCUSDETECT_INFO;

// �����˵�������(��ӦCFG_CMD_VIDEO_MOSAIC)
/*��Ƶ���������ˣ���PrivacyMasking(��̨��˽�ڵ�)�Ĳ�����ڣ�
VideoMosaic���ӵ�������λ���ǹ̶��ģ�����̨���ӵ�������
����̨��ת������������ᷴ��ƫ�ƣ���ʼ���ڵ�סͬһ���塣*/
typedef struct tagCFG_VIDEO_MOSAIC_INFO
{
	BOOL			bEnable;					// �����˵���ʹ��
	int				nMosaic;					// �����˿�����С����������ȡֵ������Խ����ԭʼͼ�����Խ�󣬷�֮Խ�ӽ�
	int				nRectCount;					// ʵ�����õ��������������
	CFG_RECT		stuRect[MAX_RECT_COUNT];	// ����������,ʹ�����������ϵ��ȡֵ��Ϊ0~8192
} CFG_VIDEO_MOSAIC_INFO;

// ��������������
typedef struct tagCFG_MOVE_DETECT_INFO
{
	BOOL					bEnable;			// ����������ʹ��
	int						nSensitivity;		// ���������, 0~100
	CFG_ALARM_MSG_HANDLE	stuEventHandler;	// ���������Ϣ
} CFG_MOVE_DETECT_INFO;

typedef enum tagEM_CFG_ATMCONTROLLER_MODE
{
    EM_CFG_ATMCONTROLLER_MODE_UNKNOWN = 0,        //δ֪
    EM_CFG_ATMCONTROLLER_MODE_CLOSE,              //�ر�
    EM_CFG_ATMCONTROLLER_MODE_OPEN,               //����
    EM_CFG_ATMCONTROLLER_MODE_LINKAGE,            //����̽������ģʽ
}EM_CFG_ATMCONTROLLER_MODE;

// �����������ƿ�������
typedef struct tagCFG_FLOODLIGHT_CONTROLMODE_INFO
{
    EM_CFG_ATMCONTROLLER_MODE emControlMode;          //����ģʽ
}CFG_FLOODLIGHT_CONTROLMODE_INFO;

//�����շ��ȿ�������
typedef struct tagCFG_AIRFAN_CONTROLMODE_INFO
{
    EM_CFG_ATMCONTROLLER_MODE emControlMode;          //����ģʽ
}CFG_AIRFAN_CONTROLMODE_INFO;

// EAP����
typedef enum tagEM_CFG_EAP_METHOD
{
    EM_CFG_EAP_METHOD_UNKNOWN,                            // UnKnown
    EM_CFG_EAP_METHOD_PEAP,                               // PEAP
    EM_CFG_EAP_METHOD_TLS,                                // TLS
    EM_CFG_EAP_METHOD_TTLS,                               // TTLS
} EM_CFG_EAP_METHOD;

// EAP�����֤����
typedef enum tagEM_CFG_EAP_AUTH_TYPE
{
    EM_CFG_EAP_AUTH_TYPE_UNKNOWN,                            // UnKnown
    EM_CFG_EAP_AUTH_TYPE_NONE,                               // NONE
    EM_CFG_EAP_AUTH_TYPE_PAP,                                // PAP
    EM_CFG_EAP_AUTH_TYPE_MSCHAP,                             // MSCHAP
    EM_CFG_EAP_AUTH_TYPE_MSCHAPV2,                           // MSCHAPV2
    EM_CFG_EAP_AUTH_TYPE_GTC,                                // GTC
} EM_CFG_EAP_AUTH_TYPE;

// �����豸��֤��ʽ
typedef enum tagEM_CFG_WIRELESS_AUTHENTICATION
{
    EM_CFG_WIRELESS_AUTHENTICATION_UNKNOWN,                             // UnKnown
    EM_CFG_WIRELESS_AUTHENTICATION_OPEN,                                // OPEN
    EM_CFG_WIRELESS_AUTHENTICATION_SHARED,                              // SHARED
    EM_CFG_WIRELESS_AUTHENTICATION_WPA,                                 // WPA
    EM_CFG_WIRELESS_AUTHENTICATION_WPAPSK,                              // WPA-PSK
    EM_CFG_WIRELESS_AUTHENTICATION_WPA2,                                // WPA2
    EM_CFG_WIRELESS_AUTHENTICATION_WPA2PSK,                             // WPA2-PSK
    EM_CFG_WIRELESS_AUTHENTICATION_WPANONE,                             // WPA-NONE
    EM_CFG_WIRELESS_AUTHENTICATION_WPAPSK_WPA2PSK,                      // WPA-PSK|WPA2-PSK
    EM_CFG_WIRELESS_AUTHENTICATION_WPA_WPA2,                            // WPA|WPA2
    EM_CFG_WIRELESS_AUTHENTICATION_WPA_WPAPSK,                          // WPA | WPA-PSK
    EM_CFG_WIRELESS_AUTHENTICATION_WPA2_WPA2PSK,                        // WPA2|WPA2-PSK
    EM_CFG_WIRELESS_AUTHENTICATION_WPA_WPAPSK_WPA2_WPA2PSK,             // WPA|WPA-PSK|WPA2|WPA2-PSK
} EM_CFG_WIRELESS_AUTHENTICATION;

// �������ݼ��ܷ�ʽ
typedef enum tagEM_CFG_WIRELESS_DATA_ENCRYPT
{
    EM_CFG_WIRELESS_DATA_ENCRYPT_UNKNOWN,                            // UnKnown
    EM_CFG_WIRELESS_DATA_ENCRYPT_NONE,                               // NONE
    EM_CFG_WIRELESS_DATA_ENCRYPT_WEP,                                // WEP
    EM_CFG_WIRELESS_DATA_ENCRYPT_TKIP,                               // TKIP
    EM_CFG_WIRELESS_DATA_ENCRYPT_AES,                                // AES(CCMP)
    EM_CFG_WIRELESS_DATA_ENCRYPT_TKIP_AES,                           // TKIP+AES
} EM_CFG_WIRELESS_DATA_ENCRYPT;

// ����WLAN����EAP
typedef struct tagCFG_WLAN_EAP
{
    EM_CFG_EAP_METHOD       emMethod;                           // EAP����
    EM_CFG_EAP_AUTH_TYPE    emAuthType;                         // EAP�����֤����
    char                    szIdentity[MAX_USERNAME_LEN];       // ���
    char                    szAnonymousID[MAX_USERNAME_LEN];    // �������
    char                    szPassword[MAX_PASSWORD_LEN];       // ����
    char                    szCaCert[512];                      // CA֤��
    char                    szUserCert[512];                    // �û�֤�� 
} CFG_WLAN_EAP;

// ����WLAN����Network
typedef struct tagCFG_WLAN_NETWORK
{
    char                    szIPAddress[AV_CFG_IP_Address_Len_EX];        // IP
    char                    szSubnetMask[AV_CFG_IP_Address_Len_EX];       // ��������
    char                    szDefaultGateway[AV_CFG_IP_Address_Len_EX];   // ����
    BOOL                    bDhcpEnable;                              // �Ƿ���DHCP
    char                    szDnsServers[2][AV_CFG_IP_Address_Len_EX];    // DNS������
} CFG_WLAN_NETWORK;

// ����WLAN����
typedef struct tagCFG_WLAN_INFO
{
    char                    szWlanName[CFG_COMMON_STRING_32];   // Wlan����, ֻ�ܻ�ȡ�����޸�
    BOOL                    bEnable;                            // WIFI����ʹ�ܿ���, TRUE��, FALSE�ر�
    char                    szSSID[CFG_MAX_SSID_LEN];           // ��������(SSID)
    BOOL                    bConnectEnable;                     // �ֶ����ӿ���, TRUE�ֶ�����, FALSE�ֶ��Ͽ�
    BOOL                    bLinkEnable;                        // �Զ����ӿ���, TRUE���Զ�����, FALSE�Զ�����, IPC������
    int                     nLinkMode;                          // ����ģʽ, 0: auto, 1: adhoc, 2: Infrastructure
    int                     nEncryption;                        // ����ģʽ, 0: off, 1: on, 2: WEP-OPEN, 3: WEP-SHARED, 4: WPA-TKIP, 5: WPA-PSK-TKIP, 6: WPA2-TKIP, 7: WPA2-PSK-TKIP, 8: WPA-AES, 9: WPA-PSK-AES, 10: WPA2-AES, 11: WPA2-PSK-AES, 12: Auto
                                                                /* ����byAuthMode  , byEncrAlgr  ������ӳ���ϵ
                                                                                                                Authentication��֤��ʽ  DataEncryption���ݼ��ܷ�ʽ  Encryption����ģʽ
                                                                                                                OPEN                    NONE                        "On" 
                                                                                                                OPEN                    WEP                         "WEP-OPEN"
                                                                                                                SHARD                   WEP                         "WEP-SHARED"
                                                                                                                WPA                     TKIP                        "WPA-TKIP"
                                                                                                                WPA-PSK                 TKIP                        "WPA-PSK-TKIP"
                                                                                                                WPA2                    TKIP                        "WPA2-TKIP"
                                                                                                                WPA2-PSK                TKIP                        "WPA2-PSK-TKIP"
                                                                                                                WPA                     AES(CCMP)                   "WPA-AES"
                                                                                                                WPA-PSK                 AES(CCMP)                   "WPA-PSK-AES"
                                                                                                                WPA2                    AES(CCMP)                   "WPA2-AES"
                                                                                                                WPA2-PSK                AES(CCMP)                   "WPA2-PSK-AES"
                                                                                                                WPA                     TKIP+AES( mix Mode)         "WPA-TKIP"����"WPA-AES"
                                                                                                                WPA-PSK                 TKIP+AES( mix Mode)         "WPA-PSK-TKIP"����"WPA-PSK-AES"
                                                                                                                WPA2                    TKIP+AES( mix Mode)         "WPA2-TKIP"����"WPA2-AES"
                                                                                                                WPA2-PSK                TKIP+AES( mix Mode)         "WPA2-PSK-TKIP"����"WPA2-PSK-AES"
                                                                                                                */
    EM_CFG_WIRELESS_AUTHENTICATION emAuthentication;            // ��֤��ʽ,  ��ʱû��
    EM_CFG_WIRELESS_DATA_ENCRYPT   emDataEncryption;            // ���ݼ��ܷ�ʽ, ��ʱû��
    int                     nKeyType;                           // ��������, 0: Hex, 1: ASCII
    int                     nKeyID;                             // ��Կ����, ȡֵ0~3
    char                    szKeys[4][32];                      // ��������
    BOOL                    bKeyFlag;                           // �����Ƿ��Ѿ�����
    CFG_WLAN_EAP            stuEap;                             // EAP
    CFG_WLAN_NETWORK        stuNetwork;                         // Network
} CFG_WLAN_INFO;

// WLAN����(��Ӧ CFG_CMD_WLAN)
typedef struct tagCFG_NETAPP_WLAN
{
    int                     nNum;                               // stuWlanInfo��Ч��WLAN���ø���
    CFG_WLAN_INFO           stuWlanInfo[8];                     // WLAN������Ϣ
} CFG_NETAPP_WLAN;

// ����Smart264 ���뷽ʽ
typedef struct tagCFG_SMART_ENCODE_INFO
{
	BOOL				bSmartH264;							    //��ʶ�Ƿ���SmartH264
	BYTE                byReserved[256];						//Ԥ���ֶ�
} CFG_SMART_ENCODE_INFO;

// ���ظ��ٱ�������
typedef struct tagCFG_VEHICLE_HIGHSPEED_INFO
{
	BOOL                    bEnable;							// ���ٱ���ʹ��
	int						nSpeedHighLine;						// �������ޱ���ֵ ��λ��Km/h
	int						nTime;								// ���ٺ�����೤ʱ�䱨�� ��λ����
	int						nAlertSpeed;						// Ԥ���ٶ�  ��λ��Km/h
	int						nInstantSpeed;						// ˲ʱ�����ٶ�  ��λ��Km/h
	CFG_ALARM_MSG_HANDLE    stuEventHandler;			        // ��������
} CFG_VEHICLE_HIGHSPEED_INFO;

// ���ص��ٱ�������
typedef struct tagCFG_VEHICLE_LOWSPEED_INFO
{
	BOOL                    bEnable;							// ���ٱ���ʹ��
	int						nSpeedLowLine;						// �������ޱ���ֵ ��λ��Km/h
	int						nTime;								// ���ٺ�����೤ʱ�䱨�� ��λ����
	int						nSpeedMotion;						// ���ٶ����ٶ�ֵ�����ڴ�ֵ��������  ��λ��Km/h
	CFG_ALARM_MSG_HANDLE    stuEventHandler;			        // ��������
} CFG_VEHICLE_LOWSPEED_INFO;

// �������˵绰�ӻ�������Ϣ
typedef struct tagCFG_PSTN_PERSON_SERVER_INFO
{
    char					szName[MAX_NAME_LEN];						// ���˵绰����
    char					szNumber[MAX_PHONE_NUMBER_LEN];				// ���˵绰���ջ�����
}CFG_PSTN_PERSON_SERVER_INFO;

// ���˵绰�ӻ�������Ϣ
typedef struct tagCFG_PSTN_PERSON_SERVER_INFO_ALL
{
    BOOL                            bEnable;                                    // ���˵绰�ӻ�����ʹ��
    int                             nServerCount;                               // ��Ч�ĸ��˵绰�ӻ�������Ϣ����,Ŀǰ���֧��3��
    CFG_PSTN_PERSON_SERVER_INFO     stuPSTNPersonServer[MAX_PSTN_SERVER_NUM];   // ���˵绰�ӻ�������Ϣ����,ÿ��Ԫ�ض�Ӧһ�����˵绰����
                                                                                // ���֧�ֶ��ٸ��˵绰,ͨ����ȡ����������(CLIENT_QueryNewSystemInfo��Ӧ��CFG_CAP_ALARM) ��ȡ��Ч�绰����
}CFG_PSTN_PERSON_SERVER_INFO_ALL;

// ����������������Ϣ
typedef struct tagCFG_ARMLINK_INFO
{
    BOOL                            bMMSEnable;                                 // �Ƿ��Ͷ���Ϣ(Ĭ�Ϸ���)
    CFG_PSTN_ALARM_SERVER	        stuPSTNAlarmServer;						    // �绰��������������Ϣ
    BOOL                            bPersonAlarmEnable;                         // �Ƿ���˽����ϵ�˴�绰(��PSTNPersonServer�����е���ϵ�˷���������)
	int								nAlarmOutChannels[256];						// �������ͨ�����б�
	int								nAlarmOutChannelNum;						// �������ͨ���Ÿ���
}CFG_ARMLINK_INFO;

// PSTN ���Լƻ�����
typedef struct tagCFG_PSTN_TESTPLAN_INFO
{
    BOOL                  bAlarmEnable;                             // ����ʹ��
    int                   nPeriod;                                  // �������� ��λ��Сʱ
    int                   nAlarmServerCount;                        // ��Ҫ���Եı����绰���ĸ���
    int                   nAlarmServer[MAX_PSTN_SERVER_NUM];        // ��Ҫ���Եı����绰������ţ���0��ʼ
}CFG_PSTN_TESTPLAN_INFO;

//������������ʹ������
typedef struct tagCFG_DEFENCE_ARMMODE_INFO
{
    BOOL                bEnableDefenceArm;                          // ������������ʹ��
}CFG_DEFENCE_ARMMODE_INFO;

//̽������װ����ģʽ
typedef enum tagEM_CFG_SENSORMODE_TYPE
{
    EM_CFG_SENSORMODE_TYPE_UNKNOWN,
    EM_CFG_SENSORMODE_TYPE_TWOSTATE,                                // ��̬
    EM_CFG_SENSORMODE_TYPE_FOURSTATE,                               // ��̬
}EM_CFG_SENSORMODE_TYPE;

//̽������װ����ģʽ����
typedef struct tagCFG_SENSORMODE_INFO
{
    EM_CFG_SENSORMODE_TYPE                 emState;                 //̽������װ����ģʽ         
}CFG_SENSORMODE_INFO;

// ������Led��ʾ�ƻ�����
typedef struct tagCFG_CABINLED_TIME_SCHEDULE
{
    BOOL                    bEnable;                                // ������Led��ʾ�ƻ�����ʹ����
}CFG_CABINLED_TIME_SCHEDULE;

// ����״̬
typedef enum tagEM_ALARMLAMP_MODE
{
    EM_ALARMLAMP_MODE_UNKNOWN = -1,                                 // δ֪
    EM_ALARMLAMP_MODE_OFF,                                          // ��
    EM_ALARMLAMP_MODE_ON,                                           // ��
    EM_ALARMLAMP_MODE_BLINK,                                        // ��˸
}EM_ALARMLAMP_MODE;

// ��������(��Ӧ CFG_CMD_ALARMLAMP)
typedef struct tagCFG_ALARMLAMP_INFO
{
    EM_ALARMLAMP_MODE      emAlarmLamp;                             // ����״̬
}CFG_ALARMLAMP_INFO;

// �״�������� ����¥��ר��(��Ӧ CFG_CMD_RADAR_SPEED_MEASURE)
typedef struct tagCFG_RADAR_SPEED_MEASURE_INFO
{
    float                   fSpeedMeasureLimit;                     // ��������ֵ,ֻ�дﵽ���ٶ�ֵ,�״���ܼ��(��λ:km/h)
    float                   fSpeedUpperLimit;                       // ���ٱ���ֵ,������ֵ�ϱ�����(��λ:km/h)

}CFG_RADAR_SPEED_MEASURE_INFO;


// ����������
typedef struct tagCFG_LASER_DIST_MEASURE_INFO
{
    BOOL                            bEncodeBlend;                   // ���ӵ���������Ƶ����    
    BOOL                            bEnable;                        // ʹ��,�Ƿ�򿪼����๦��
}CFG_LASER_DIST_MEASURE_INFO;




// ������Ϣ��������
typedef struct tagCFG_CFG_CMD_ATMOSPHERE_OSD_INFO
{
    BOOL                            bEncodeBlendEnable;             // �Ƿ���ӵ���Ƶ������true������false�ر�
    UINT                            nMask;                          // 1��ʾ 0����ʾ
                                                                    // bit0: ���� 
                                                                    // bit1: ����
                                                                    // bit2: ʪ��
                                                                    // bit3���¶�
                                                                    // bit4������ѹǿ
                                                                    // bit5������ǿ��
}CFG_CMD_ATMOSPHERE_OSD_INFO;

// ��ɽ����4G������ֵ��ģʽ����(��Ӧ CFG_CMD_OIL_4G_OVERFLOW)
typedef struct tagCFG_OIL_4G_OVERFLOW_INFO
{
	int								nFlowRateMode;					// 4G������ģʽ��1�ֶ���0-�Զ���Ĭ���Զ�
	int								nFlowRateMax;					// 4G��������ֵ����λΪ(MB), Ĭ��ֵΪ60*1024(MB)
}CFG_OIL_4G_OVERFLOW_INFO;

// ��ɽ����4G����OSD��������(��Ӧ CFG_CMD_OIL_VIDEOWIDGET_4G_FLOW)
typedef struct tagCFG_OIL_VIDEOWIDGET_4G_FLOW_INFO
{
    BOOL                            bEncodeBlend;                   // ���ӵ���������Ƶ����ʹ��
	CFG_RECT						stuRect;						// ���ӵ���������
}CFG_OIL_VIDEOWIDGET_4G_FLOW_INFO;

#define CFG_PARK_SPACE_STATE_TYPE_NUM      3                        // ��λ״̬���͸��� ��������Ϊ ��λ�޳� ��λ�г� ��λԤ��

// ��λ��������ָʾ��������Ϣ
typedef struct tagCFG_PARK_SPACE_OUT_LIGHT_INFO
{
    EM_CFG_PARKINGSPACE_LIGHT_STATE stNormalSpace[CFG_PARK_SPACE_STATE_TYPE_NUM][CFG_MAX_PARKINGSPACE_LIGHT_NUM];  // ��ͨ��λ��״̬��Ϣ
                                                                                                                   // �����һά�ȱ�ʾ��λ״̬ 0-��λ�޳� 1-��λ�г� 2-��λԤ��
                                                                                                                   // ����ڶ�ά�ȱ�ʾָʾ����ɫ ��0��5�ֱ��ʾ �� �� �� �� �� ��  
    EM_CFG_PARKINGSPACE_LIGHT_STATE stSpecialSpace[CFG_PARK_SPACE_STATE_TYPE_NUM][CFG_MAX_PARKINGSPACE_LIGHT_NUM]; // ���⳵λ��״̬��Ϣ ����ͬ��
}CFG_PARK_SPACE_OUT_LIGHT_INFO;

//�Ž�������ʾ��ʾ����(��ӦCFG_CMD_ACCESS_TEXTDISPLAY)
typedef struct tagCFG_ACCESS_TEXTDISPLAY_INFO
{
	char szOpenSucceedText[MAX_ACCESS_TEXTDISPLAY_LEN];              //���ųɹ���ʾ��������Ϣ
	char szOpenFailedText[MAX_ACCESS_TEXTDISPLAY_LEN];               //����ʧ����ʾ��������Ϣ
}CFG_ACCESS_TEXTDISPLAY_INFO;

// �ſڻ����й���
typedef enum tagEM_CFG_ROOM_RULE
{
    EM_CFG_ROOM_RULE_UNKNOW,                                        // δ֪
	EM_CFG_ROOM_RULE_SERIAL,                                        // Serial ��������ţ���Щ����ʹ��1~100�ķ���
	EM_CFG_ROOM_RULE_NOSERIAL,                                      // NoSerial ����������ţ�������ڵ�301,502
}EM_CFG_ROOM_RULE;

// VTO����������չ��Ϣ
typedef struct tagCFG_VTO_CALL_INFO_EXTEND
{
	char                            szMainVTOIP[MAX_ADDRESS_LEN];               //���ſڻ�IP
	BOOL                            bGroupCallEnable;                           //Ⱥ������
	char                            szManagerNumber[MAX_PHONE_NUMBER_LEN];      //���������
	char                            szUrgentNumber[MAX_PHONE_NUMBER_LEN];       //�������к���
	CFG_TIME_SECTION                stuTimeSection;                             //����VTSʱ��� 
	unsigned int                    nMaxExtensionIndex;                         //�ſڻ��Ͽ������õ�������ڻ��ֻ���� ��Ŵ�1��ʼ
    EM_CFG_ROOM_RULE                emRoomRule;                                 //�ſڻ�֧����������ź��кͷ���������ź���2�׹���
}CFG_VTO_CALL_INFO_EXTEND;

//��ʦ����Ƶ������Ŀ���漰IPC��NVR ����������Ϣ,
typedef struct tagCFG_NETNVR_ENCRYPT_CHANNEL_INFO
{
	BOOL						bEncryptEnable;					// ����Ƶ�Ƿ����
	BOOL						bMatchNvrEnable;				// NVR���ʹ��
	char						szNvrAddress[16];				// NVR ��IP��ַ
	DWORD						nMatchPort;						// ��Զ˿�
	DWORD						nTalkPort;						// Э�̶˿�
	char						szDevID[32];					// IPC�豸���к�	
}CFG_NETNVR_ENCRYPT_CHANNEL_INFO;

//��ʦ����Ƶ������Ŀ���漰IPC��NVR ����������Ϣ,,��Ӧ CFG_CMD_NETNVR_ENCRYPT
typedef struct tagCFG_NETNVR_ENCRYPT_INFO
{
	int									nChannelID;									// ͨ����(0��ʼ)
	int									nNvrEncryptNum;								// ÿ��ͨ��ʵ�ʼ�����Ϣ���ø���
	CFG_NETNVR_ENCRYPT_CHANNEL_INFO		stuNvrEncryptInfo[CFG_MAX_NVR_ENCRYPT_COUNT];// ÿ��ͨ����Ӧ�ļ�����Ϣ��������,������1��2��3����
}CFG_NETNVR_ENCRYPT_INFO;

// Ƶ���ƶ˿���Ϣ
typedef struct tagCFG_LAMP_PORT_INFO
{
    int					nValidPortNum;						// Ƶ�������ø���
    char				szPort[8];							// Ƶ���ƵĶ˿ں�,�����ڿ��ܰ��������
}CFG_LAMP_PORT_INFO;

// ����Ƶ����������Ϣ
typedef struct tagCFG_SINGLE_STROBOSCOPIC_LAMP_INFO
{
	CFG_FLASH_CONTROL	stuLampInfo;						// Ƶ���Ƶ�������Ϣ
	int					nStroboscopicMode;					// Ƶ��ģʽ,�����������Ƶ�ʣ�0��ʾ������Ҫ����
	int					nPreTime;							// Ƶ������ǰʱ��,��λus
	int					nPulseWidth;						// Ƶ��������,��λus
	int					nDelayTime;							// ��ʱ��ʱ��,��λus��������ʾ��ǰ����
	int					nFrequency;							// Ƶ���ƵĹ���Ƶ�ʣ���λHZ,Ʃ��50HZ, 100HZ 
	CFG_LAMP_PORT_INFO	stuPortInfo;						// Ƶ���ƶ˿�������Ϣ
}CFG_SINGLE_STROBOSCOPIC_LAMP_INFO;

// Ƶ��������(��Ӧ CFG_CMD_STROBOSCOPIC_LAMP)
typedef struct tagCFG_STROBOSCOPIC_LAMP_INFO
{
    int									nValidConfigNum;	// Ƶ�������ø���
    CFG_SINGLE_STROBOSCOPIC_LAMP_INFO   stuStroboInfo[8];	// Ƶ����������Ϣ
}CFG_STROBOSCOPIC_LAMP_INFO;

// Https��������
typedef struct tagCFG_HTTPS_INFO
{
    BOOL                                bEnable;                                    // ����ʹ��
    int                                 nPort;                                      // �˿ں�
    char                                szRootCertificatePath[CFG_COMMON_STRING_128];                 // ��֤�����·��
    char                                szCountry[CFG_COMMON_STRING_8];                               // ����
    char                                szState[CFG_COMMON_STRING_64];                                // ������ʡ��
    char                                szLocatity[CFG_COMMON_STRING_64];                             // λ��
    char                                szOrganization[CFG_COMMON_STRING_64];                         // ��֯����
    char                                szOrganizationUnit[CFG_COMMON_STRING_64];                    // ��֯��Ԫ����
    char                                szCommonName[CFG_COMMON_STRING_128];                          // CN�ֶΣ�����������IP
    char                                szEmail[CFG_COMMON_STRING_128];                               // ����
    char                                szName[CFG_COMMON_STRING_128];                                // ��˾����
    char                                szPath[CFG_COMMON_STRING_128];                                // ������֤���ַ
    int                                 nUsefulLife;                                // ֤����Ч��
}CFG_HTTPS_INFO;

// ��������
typedef struct tagCFG_ADAPT_ENCODE_INFO
{
    int                             nIPSmoothness;                   // ����I/P֡��ƽ���ȣ���Χ1~100,1��ʾƽ����100��ʾ��������
    int                             nAdjustEnable;                   // ����Ӧ����ʹ��,1���� 0�رա��������ݲ�֧�ִ��ֶ�
}CFG_ADAPT_ENCODE_INFO;

// ��������Ӧ��������
typedef struct tagCFG_NET_AUTO_ADAPT_ENCODE
{
    CFG_ADAPT_ENCODE_INFO    stuMainStream[MAX_VIDEOSTREAM_NUM];	 // ��������0����ͨ¼��1-����¼��2������¼��,��ֻ֧����ͨ¼��
    CFG_ADAPT_ENCODE_INFO	 stuExtraStream[MAX_VIDEOSTREAM_NUM];    // ��������0��������1��1��������2��2��������3
}CFG_NET_AUTO_ADAPT_ENCODE;

// ���ͨ��
typedef struct tagCFG_COMBINATION_CHANNEL
{
    int                                 nMaxChannel;    // ���ͨ����
    int                                 nReturnChannel; // ʵ�ʷ���ͨ����
    int                                 *pnChannel;     // ͨ����Ϣ
}CFG_COMBINATION_CHANNEL;

// �ָ�ģʽ
typedef struct tagCFG_FREESPLIT_INFO
{
    CFG_SPLITMODE                       emSplitMode;        // �ָ�ģʽ
    int                                 nMaxCombination;    // �����ϸ���
    int                                 nReturnCombination; // ʵ����ϸ���
    CFG_COMBINATION_CHANNEL             *pstuCombination;   // �����Ϣ
}CFG_FREESPLIT_INFO;

// ���ɷָ�ģʽ�Ĵ�������
typedef struct tagCFG_FREECOMBINATION_INFO
{
    int                 nMaxSplit;      // ���ָ�ģʽ��
    int                 nReturnSplit;   // ʵ�ʷָ�ģʽ��
    CFG_FREESPLIT_INFO  *pstuSplit;     // ���ɷָ�ģʽ��Ϣ
}CFG_FREECOMBINATION_INFO;

// �豸Э������
typedef enum tagCFG_EM_DEVICE_PROTOCOL
{
	CFG_EM_PROTOCOL_PRIVATE3,                   // ˽��3��Э��,Dahua3
	CFG_EM_PROTOCOL_ONVIF,                      // Onvif    		
}CFG_EM_DEVICE_PROTOCOL;

// ƽ̨�����IPC���� CFG_CMD_PLATFORM_MONITOR_IPC
typedef struct tagCFG_PLATFORMMONITORIPC_INFO
{
	BOOL					bEnable;                        // ʹ��
	char					szDevIpAddress[MAX_NAME_LEN];	// �豸ip��ַ
	int						nPort;                          // �˿�
	char					szUserName[MAX_USERNAME_LEN];	// �û���
	char					szPassword[MAX_PASSWORD_LEN];	// ����
	CFG_EM_STREAM_TYPE		emStreamType;                   // ��������
	CFG_EM_DEVICE_PROTOCOL  emProtocol;                     // Э������
	int						nChannelID;                     // ͨ����
}CFG_PLATFORMMONITORIPC_INFO;

#define MAX_AREA_COUNT	8		// ����������

// ����������������(��ӦCFG_CMD_IOT_INFRARED_DETECT)
typedef struct tagCFG_IOT_INFRARED_DETECT_INFO
{
	BOOL					bEnable[MAX_AREA_COUNT];				// ����ʹ�ܣ���ʾ�Ƿ������,��8������
	int						nDetectRadius;							// ���뾶����λ����
} CFG_IOT_INFRARED_DETECT_INFO;

// ������¼����������(��ӦCFG_CMD_IOT_RECORD_HANDLE)
typedef struct tagCFG_IOT_RECORD_HANDLE_INFO
{
	BOOL					bEnable;								//  ʹ�ܣ���ʾ�Ƿ���¼������
	int						nRecordTime;							// ����¼��ʱ�䣬��λ��
} CFG_IOT_RECORD_HANDLE_INFO;

// ������ץͼ��������(��ӦCFG_CMD_IOT_SNAP_HANDLE)
typedef struct tagCFG_IOT_SNAP_HANDLE_INFO
{
	BOOL					bEnable;								// ʹ�ܣ���ʾ�Ƿ���ץͼ����
	int						nSnapNum;								// ����ץͼ����
} CFG_IOT_SNAP_HANDLE_INFO;

// ����ת������
typedef struct tagCFG_CALLFORWARD_INFO
{
    BOOL                    bAlwaysForwardEnable;                           // ǿ��ת��ʹ��
    char                    szAlwaysForwardNumber[MAX_PHONE_NUMBER_LEN];    // ǿ��ת�ƺ���
    BOOL                    bBusyForwardEnable;                             // ������æת��ʹ��
    char                    szBusyForwardNumber[MAX_PHONE_NUMBER_LEN];      // ������æת�ƺ���
    BOOL                    bNoAnswerForwardEnable;                         // ������Ӧ��ת��ʹ��
    char                    szNoAnswerForwardNumber[MAX_PHONE_NUMBER_LEN];  // ������Ӧ��ת�ƺ���
}CFG_CALLFORWARD_INFO;

// �������ö�ӦCFD_CMD_DOORBELLSOUND
typedef struct tagCFG_DOOR_BELLSOUND_INFO
{
    BOOL                    bSilenceEnable;                   // �Ƿ���true ����false ������
    UINT                    nRingVolume;                      // ��������ȡֵ��Χ0~100
    char                    szRingFile[CFG_COMMON_STRING_64]; // �����ļ�����ֻ֧��"A","B","C"�����ļ�ֻ��ѡ��һ���ļ�
}CFG_DOOR_BELLSOUND_INFO;

//telnet ���ö�ӦCFG_CMD_TELNET
typedef struct tagCFG_TELNET_INFO
{
    BOOL                    bEnable;                   // telnetʹ�ܿ���trueʹ��falseȥʹ��
}CFG_TELNET_INFO;

//��ʾ�쳣��Ϣ�ĵ������� ��Ӧ�ṹ�� CFG_OSD_SYSABNORMAL_INFO
typedef struct tagCFG_OSD_SYSABNORMAL_INFO
{
	AV_CFG_Color		stuBackColor;					// ����ɫ
	AV_BOOL				bEncodeBlend;					// ���ӵ�������
	AV_CFG_Color		stuFrontColor;					// ǰ��ɫ
	AV_BOOL				bPreviewBlend;					// ���ӵ�Ԥ����Ƶ
	AV_CFG_Rect			stuRect;						// ����, ����ȡֵ0~8191
	int					nDisplayTime;					// ��ʾʱ�䣬��λΪs�����Ϊ0��ʾһֱ��ʾ
	AV_BOOL				bNetAbort;						// �Ƿ���ʾ����Ͽ���trueΪ��ʾ��falseΪ����ʾ
	AV_BOOL				bIPConflict;					// �Ƿ���ʾIP��ͻ��trueΪ��ʾ��falseΪ����ʾ
	AV_BOOL				bVoltageStatus;					// �Ƿ���ʾ��ѹ�쳣��trueΪ��ʾ��falseΪ����ʾ
	AV_BOOL             bOutputCurrentStatus;			// �Ƿ���ʾ���⹩������쳣��trueΪ��ʾ��falseΪ����ʾ
	AV_BOOL				bSDHotPlug;						// �Ƿ���ʾSD�����롢�γ����쳣��trueΪ��ʾ��falseΪ����ʾ
	AV_BOOL				bSDLowSpace;					// �Ƿ���ʾSD���ռ䲻��,trueΪ��ʾ��falseΪ����ʾ
	AV_BOOL				bSDFailure;						// �Ƿ���ʾSD������,trueΪ��ʾ��falseΪ����ʾ
	AV_BOOL				bDestroy;						// �Ƿ���ʾ�ƻ����,trueΪ��ʾ��falseΪ����ʾ
}CFG_OSD_SYSABNORMAL_INFO;

// ��Ƶ�����������(��Ӧ�ṹ�� CFG_VIDEO_WIDGET2_INFO)
typedef struct tagCFG_VIDEO_WIDGET2_INFO
{
	int					nOSDLineSpacing;				//�м�౶���������Ļ�׼Ĭ���ǵ�ǰ����߶ȵ�ʮ��֮һ��ȡֵ��ΧΪ0~5��Ĭ��ֵΪ0
}CFG_VIDEO_WIDGET2_INFO;

// ����ͳ�Ƶ���OSD����(��Ӧ�ṹ�� CFG_VIDEOWIDGET_NUMBERSTAT_INFO)
typedef struct tagCFG_VIDEOWIDGET_NUMBERSTAT_INFO
{
	AV_BOOL				bEncodeBlend;					// ���ӵ�������,ֵΪfalse�ı��ⲻ�����õ��ײ�API
	AV_CFG_Color		stuFrontColor;					// ǰ��ɫ
	AV_CFG_Color		stuBackColor;					// ����ɫ
	EM_TITLE_TEXT_ALIGN emTextAlign;                    // ������뷽ʽ
	AV_CFG_Rect			stuRect;						// ����, ����ȡֵ0~8192
	AV_BOOL				bShowEnterNum;					// �Ƿ���ʾ��������
	AV_BOOL				bShowExitNum;					// �Ƿ���ʾ�뿪����
}CFG_VIDEOWIDGET_NUMBERSTAT_INFO;

// ����������
typedef enum tagEM_MOSAIC_TYPE
{
	EM_MOSAIC_TYPE_INVALID,								// ��Ч������������
	EM_MOSAIC_TYPE_NO,									// ����������
	EM_MOSAIC_TYPE_8,									// [8x8��С] ������
	EM_MOSAIC_TYPE_16,									// [16x16��С] ������
	EM_MOSAIC_TYPE_24,									// [24x24��С] ������
	EM_MOSAIC_TYPE_32,									// [32x32��С] ������
}EM_MOSAIC_TYPE;

// ��״����
typedef enum tagEM_SHAPE_TYPE
{
	EM_SHAPE_TYPE_INVALID,								// ��Ч������
	EM_SHAPE_TYPE_RECT,									// ����
	EM_SHAPE_TYPE_POLYGON,								// �����
}EM_SHAPE_TYPE;

//��̨�������굥Ԫ
typedef struct tagPTZ_SPEED
{
    float                  fPositionX;           //��̨ˮƽ��������,��һ����-1~1
    float                  fPositionY;           //��̨��ֱ��������,��һ����-1~1
    float                  fZoom;                //��̨��Ȧ�Ŵ���,��һ���� 0~1
    char                   szReserve[32];        //Ԥ��32�ֽ�
}PTZ_SPEED;

// ���򣻸��߾ఴ����8192�ı���
typedef struct 
{
	long             left;
	long             top;
	long             right;
	long             bottom;
} DH_RECT_REGION, *LPDH_RECT_REGION;

// ��˽�ڵ�������Ϣ
typedef struct tagCGF_MASKING_INFO
{
	BOOL					bEnable;							// ��˽�ڵ����ر�־ true����false��
	PTZ_SPEED				stuPosition;						// ��̨������Ŵ���
	EM_SHAPE_TYPE			stuShapeType;						// ��״����
	DH_RECT_REGION			stuRect;							// ��������, ����ȡֵ0~8192
	int						nPolygonPoint;						// ����ζ�����
	CFG_POLYGON				stuPolygon[MAX_POLYGON_NUM];		// ����ζ�������,����ȡֵ0~8192
	AV_CFG_Color			stuColor;							// �ڵ���ɫ
	EM_MOSAIC_TYPE			stuMosaicType;						// �ڵ������������ͣ�������ʱ�ڵ���ɫ�ֶ���Ч
    double                  dViewAngle;                         // ���ˮƽ�ӳ���, ��Χ[0.0, 360.0]����λ����
}CGF_MASKING_INFO;

// ��ͨ����˽�ڵ�����(��Ӧ�ṹ�� CFG_PRIVACY_MASKING_INFO)
typedef struct tagCFG_PRIVACY_MASKING_INFO
{
	int nMaskingInfoCount;														// ��˽�ڵ����ø���
	CGF_MASKING_INFO stuMaskingInfo[MAX_PRIVACY_MASKING_COUNT];					// ��˽�ڵ�������Ϣ
}CFG_PRIVACY_MASKING_INFO;

// �豸��Ϣ����(��Ӧ�ṹ�� CFG_DEVICE_INFO)
typedef struct tagCFG_DEVICE_INFO
{
	char szSerial[CFG_COMMON_STRING_64];				// ��Ʒϵ�����ƣ�IPC�ͺ�IPC��ͷ������ͺ�SD��ͷ��������ͬ
	char szMode[CFG_COMMON_STRING_64];					// �豸�ͺ�
	char szResolution[CFG_COMMON_STRING_16];            // ��о�ֱ���
	char szCameraID[CFG_COMMON_STRING_16];              // ��оID
}CFG_DEVICE_INFO;

#define CFG_MAX_POLICE_ID_MAP_NUM 64                    // ���ԱID���豸ͨ��ӳ���ϵ����

// ��ԱID���豸ͨ��ӳ���ϵ��Ϣ(��Ӧ CFG_CMD_POLICEID_MAP_INFO)
typedef struct tagCFG_POLICEID_MAP_INFO
{
    char    szPoliceID[CFG_MAX_POLICE_ID_MAP_NUM][CFG_COMMON_STRING_32];				// ��ԱID
    int     nIDCount;                                                                   // ID��Ŀ
    
}CFG_POLICEID_MAP_INFO;

//GPSδ��λ����(��Ӧ CFG_CMD_GPS_NOT_ALIGNED)
typedef struct tagCFG_GPS_NOT_ALIGNED_INFO
{
	BOOL		                 bEnable;						// ʹ��
	DWORD      	                 dwNotifyTimes;                 // GPSδ��λ����ʱ�䣬������ʱ���򴥷���������λ��
	CFG_ALARM_MSG_HANDLE         stuEventHandler;	        	// �������� 
}CFG_GPS_NOT_ALIGNED_INFO;

//����δ���ӣ�����wifi��3G/4G������(��Ӧ CFG_CMD_WIRELESS_NOT_CONNECTED)
typedef struct tagCFG_WIRELESS_NOT_CONNECTED_INFO
{
	BOOL		                 bEnable;						// ʹ��
	DWORD      	                 dwNotifyTimes;                 // ����δ���ӣ�����wifi��3G/4G������ʱ�䣬������ʱ���򴥷���������λ��
	CFG_ALARM_MSG_HANDLE         stuEventHandler;		        // �������� 
}CFG_WIRELESS_NOT_CONNECTED_INFO;

// ΢�Ƴ��������澯����(��Ӧ CFG_CMD_MCS_GENERAL_CAPACITY_LOW)
typedef struct tagCFG_MCS_GENERAL_CAPACITY_LOW
{
	BOOL						bEnable;						// ʹ�ܿ��أ�trueΪ��;falseΪ�رգ�Ĭ��ֵ 	
	CFG_ALARM_MSG_HANDLE        stuEventHandler;				// ��������     
}CFG_MCS_GENERAL_CAPACITY_LOW;

// ΢�ƴ洢�ڵ���������(��Ӧ CFG_CMD_MCS_DATA_NODE_OFFLINE)
typedef struct tagCFG_MCS_DATA_NODE_OFFLINE
{
	BOOL						bEnable;						// ʹ�ܿ��أ�trueΪ��;falseΪ�رգ�Ĭ��ֵ 	
	CFG_ALARM_MSG_HANDLE        stuEventHandler;				// ��������     
}CFG_MCS_DATA_NODE_OFFLINE;

// ΢�ƴ������߸澯����(��Ӧ CFG_CMD_MCS_DISK_OFFLINE)
typedef struct tagCFG_MCS_DISK_OFFLINE
{
	BOOL						bEnable;						// ʹ�ܿ��أ�trueΪ��;falseΪ�رգ�Ĭ��ֵ 	
	CFG_ALARM_MSG_HANDLE        stuEventHandler;				// ��������     
}CFG_MCS_DISK_OFFLINE;

// ΢�ƴ��̱����澯����(��Ӧ CFG_CMD_MCS_DISK_SLOW)
typedef struct tagCFG_MCS_DISK_SLOW
{
	BOOL						bEnable;						// ʹ�ܿ��أ�trueΪ��;falseΪ�رգ�Ĭ��ֵ 	
	CFG_ALARM_MSG_HANDLE        stuEventHandler;				// ��������     
}CFG_MCS_DISK_SLOW;

//΢�ƴ����𻵸澯����(��Ӧ CFG_CMD_MCS_DISK_BROKEN)
typedef struct tagCFG_MCS_DISK_BROKEN
{
	BOOL						bEnable;						// ʹ�ܿ��أ�trueΪ��;falseΪ�رգ�Ĭ��ֵ 	
	CFG_ALARM_MSG_HANDLE        stuEventHandler;				// ��������     
}CFG_MCS_DISK_BROKEN;

// ΢�ƴ���δ֪����澯����(��Ӧ CFG_CMD_MCS_DISK_UNKNOW_ERROR)
typedef struct tagCFG_MCS_DISK_UNKNOW_ERROR
{
	BOOL						bEnable;						// ʹ�ܿ��أ�trueΪ��;falseΪ�رգ�Ĭ��ֵ 	
	CFG_ALARM_MSG_HANDLE        stuEventHandler;				// ��������     
}CFG_MCS_DISK_UNKNOW_ERROR;

// ΢��Ԫ���ݷ������쳣�澯����(��Ӧ CFG_CMD_MCS_METADATA_SERVER_ABNORMAL)
typedef struct tagCFG_MCS_METADATA_SERVER_ABNORMAL
{
	BOOL						bEnable;						// ʹ�ܿ��أ�trueΪ��;falseΪ�رգ�Ĭ��ֵ 	
	CFG_ALARM_MSG_HANDLE        stuEventHandler;				// ��������     
}CFG_MCS_METADATA_SERVER_ABNORMAL;

// ΢��Ŀ¼�������쳣�澯����(��Ӧ CFG_CMD_MCS_CATALOG_SERVER_ABNORMAL)
typedef struct tagCFG_MCS_CATALOG_SERVER_ABNORMAL
{
	BOOL						bEnable;						// ʹ�ܿ��أ�trueΪ��;falseΪ�رգ�Ĭ��ֵ 	
	CFG_ALARM_MSG_HANDLE        stuEventHandler;				// ��������     
}CFG_MCS_CATALOG_SERVER_ABNORMAL;

// ΢�Ƴ��������澯�ָ��澯����(��Ӧ CFG_CMD_MCS_GENERAL_CAPACITY_RESUME)
typedef struct tagCFG_MCS_GENERAL_CAPACITY_RESUME
{
	BOOL						bEnable;						// ʹ�ܿ��أ�trueΪ��;falseΪ�رգ�Ĭ��ֵ 	
	CFG_ALARM_MSG_HANDLE        stuEventHandler;				// ��������     
}CFG_MCS_GENERAL_CAPACITY_RESUME;

// ΢�ƴ洢�ڵ����߸澯����(��Ӧ CFG_CMD_MCS_DATA_NODE_ONLINE)
typedef struct tagCFG_MCS_DATA_NODE_ONLINE
{
	BOOL						bEnable;						// ʹ�ܿ��أ�trueΪ��;falseΪ�رգ�Ĭ��ֵ 	
	CFG_ALARM_MSG_HANDLE        stuEventHandler;				// ��������     
}CFG_MCS_DATA_NODE_ONLINE;

// ΢�ƴ������߸澯����(��Ӧ CFG_CMD_MCS_DISK_ONLINE)
typedef struct tagCFG_MCS_DISK_ONLINE
{
	BOOL						bEnable;						// ʹ�ܿ��أ�trueΪ��;falseΪ�رգ�Ĭ��ֵ 	
	CFG_ALARM_MSG_HANDLE        stuEventHandler;				// ��������     
}CFG_MCS_DISK_ONLINE;

// ΢��Ԫ���ݱ������߸澯����(��Ӧ CFG_CMD_MCS_METADATA_SLAVE_ONLINE)
typedef struct tagCFG_MCS_METADATA_SLAVE_ONLINE
{
	BOOL						bEnable;						// ʹ�ܿ��أ�trueΪ��;falseΪ�رգ�Ĭ��ֵ 	
	CFG_ALARM_MSG_HANDLE        stuEventHandler;				// ��������     
}CFG_MCS_METADATA_SLAVE_ONLINE;

// ΢��Ŀ¼���������߸澯����(��Ӧ CFG_CMD_MCS_CATALOG_SERVER_ONLINE)
typedef struct tagCFG_MCS_CATALOG_SERVER_ONLINE
{
	BOOL						bEnable;						// ʹ�ܿ��أ�trueΪ��;falseΪ�رգ�Ĭ��ֵ 	
	CFG_ALARM_MSG_HANDLE        stuEventHandler;				// ��������     
}CFG_MCS_CATALOG_SERVER_ONLINE;

// SecurityAlarms�ͻ����ƹ��ܣ���˽����
typedef struct tagCFG_SECURITY_ALARMS_PRIVACY
{
    char                        szCode[CFG_COMMON_STRING_64];   // ��˽��������
    BOOL                        bEnable;                        // �Ƿ�����˽����
}CFG_SECURITY_ALARMS_PRIVACY;

// �Ž���ԤǷ������
typedef struct tagCFG_ACCESSCONTROL_DELAYSTRATEGY
{
    BOOL                    bEnable;                                    // �Ƿ�����
    int                     nPreArrearsTerm;                            // ԤǷ�����ޣ���λ����
    int                     nArrearsTerm;                               // Ƿ�����ޣ���λ����
}CFG_ACCESSCONTROL_DELAYSTRATEGY;


// ���˻�����ʱ�� ���� ( ��Ӧ CFG_CMD_NO_FLY_TIME )
typedef struct tagCFG_NO_FLY_TIME_INFO
{
    BOOL		                 bEnable;						// ����ʱ��ʹ��
    CFG_ALARM_MSG_HANDLE         stuEventHandler;		        // �������� 
}CFG_NO_FLY_TIME_INFO;


// �������ù��� ���� ( ��Ӧ CFG_CMD_PWD_RESET )
typedef struct tagCFG_PWD_RESET_INFO
{
    BOOL		                 bEnable;						// �������ù���ʹ��
}CFG_PWD_RESET_INFO;


// ��������ж��¼�����( ��Ӧ�ṹ�� CFG_NET_MONITOR_ABORT_INFO )
typedef struct tagCFG_NET_MONITOR_ABORT_INFO
{
	BOOL		                bEnable;			// ʹ�ܿ���
    CFG_ALARM_MSG_HANDLE		stuEventHandler;	// �������� 
} CFG_NET_MONITOR_ABORT_INFO;

// ������չ���� ���� ( ��Ӧ CFG_CMD_LOCAL_EXT_ALARM )
typedef struct tagCFG_LOCAL_EXT_ALARME_INFO
{
    BOOL		            bEnable;						        // ʹ��
    int                     nSensorType;					        // ���������� 0,���� 1,����
    char                    szAlarmChannelName[CFG_COMMON_STRING_64];      // ����ͨ������
    CFG_ALARM_MSG_HANDLE    stuEventHandler;		                // �������� 
}CFG_LOCAL_EXT_ALARME_INFO;

#define		MAX_INVITE_NUMBER_LIST		16
// ��Ƶ�Խ��绰��������( ��Ӧ CFG_CMD_VIDEO_TALK_PHONE_BASIC )
typedef struct tagCFG_VIDEO_TALK_PHONE_BASIC_INFO
{
	BOOL					bAlarmOutputEnable;													// ���������������ʹ��
	BOOL					bNoAnswerTransferPlatformEnable;									// ������Ӧ��ת��ƽ̨ʹ��
	BOOL					bEachCallEnable;													// ����ͨ��ʹ��
	BOOL					bAutoAnswerEnable;													// �Զ�����ʹ��
	int						nInviteNumberList;													// ���з����б����
	char					szInviteNumberList[MAX_INVITE_NUMBER_LIST][CFG_COMMON_STRING_16];	// ���з����б�
    BOOL                    bVTOCallSoundEnable;                                                // �Ƿ����ſڻ�����ʹ��
    char                    szVTOCallSound[CFG_COMMON_STRING_128];                              // �ſڻ���������·��
    BOOL                    bEnableCall;                                                        // ����ʹ�� 
} CFG_VIDEO_TALK_PHONE_BASIC_INFO;

// ��������
typedef enum tagEM_LANGUAGE_TYPE
{
    EM_LANGUAGE_ENGLISH,                // Ӣ��    
    EM_LANGUAGE_CHINESE_SIMPLIFIED,     // ��������    
    EM_LANGUAGE_CHINESE_TRADITIONAL,    // ��������    
    EM_LANGUAGE_ITALIAN,                // �������    
    EM_LANGUAGE_SPANISH,                // ��������
    EM_LANGUAGE_JAPANESE,               // ���İ�    
    EM_LANGUAGE_RUSSIAN,                // ���İ�        
    EM_LANGUAGE_FRENCH,                 // ���İ�        
    EM_LANGUAGE_GERMAN,                 // ���İ�        
    EM_LANGUAGE_PORTUGUESE,             // ��������    
    EM_LANGUAGE_TURKEY,                 // ��������    
    EM_LANGUAGE_POLISH,                 // ������    
    EM_LANGUAGE_ROMANIAN,               // ��������    
    EM_LANGUAGE_HUNGARIAN,              // ��������    
    EM_LANGUAGE_FINNISH,                // ������    
    EM_LANGUAGE_ESTONIAN,               // ��ɳ������    
    EM_LANGUAGE_KOREAN,                 // ����    
    EM_LANGUAGE_FARSI,                  // ��˹��     
    EM_LANGUAGE_DANSK,                  // ������
    EM_LANGUAGE_CZECHISH,               // �ݿ���
    EM_LANGUAGE_BULGARIA,               // ����������
    EM_LANGUAGE_SLOVAKIAN,              // ˹�工����
    EM_LANGUAGE_SLOVENIA,               // ˹����������
    EM_LANGUAGE_CROATIAN,               // ���޵�����
    EM_LANGUAGE_DUTCH,                  // ������
    EM_LANGUAGE_GREEK,                  // ϣ����
    EM_LANGUAGE_UKRAINIAN,              // �ڿ�����
    EM_LANGUAGE_SWEDISH,                // �����
    EM_LANGUAGE_SERBIAN,                // ����ά����
    EM_LANGUAGE_VIETNAMESE,             // Խ����
    EM_LANGUAGE_LITHUANIAN,             // ��������
    EM_LANGUAGE_FILIPINO,               // ���ɱ���
    EM_LANGUAGE_ARABIC,                 // ��������
    EM_LANGUAGE_CATALAN,                // ��̩��������
    EM_LANGUAGE_LATVIAN,                // ����ά����
    EM_LANGUAGE_THAI,                   // ̩��
    EM_LANGUAGE_HEBREW,                 // ϣ������
    EM_LANGUAGE_Bosnian,                // ��˹������
} EM_LANGUAGE_TYPE;

// ֧�ֵ�������Ը���
#define	MAX_SUPPORTED_LANGUAGE	32
// �ֻ�������Ϣ�ķ���Ŀ���������� (��Ӧ CFG_APP_EVENT_LANGUAGE_INFO )
typedef struct tagCFG_APP_EVENT_LANGUAGE_INFO
{
	EM_LANGUAGE_TYPE		emCurrLanguage;														// ������Ϣ��Ҫ����ɵ�Ŀ������
	int						nSupportLanguageCount;												// �豸֧�ֵ����Ը���
	EM_LANGUAGE_TYPE		emLanguage[MAX_SUPPORTED_LANGUAGE];									// �豸֧�ַ���������б�
} CFG_APP_EVENT_LANGUAGE_INFO;

// dropbox token����( ��ӦCFG_CMD_DROPBOXTOKEN)
typedef struct tagCFG_DROPBOXTOKEN_INFO
{
    char                    szDropBoxToken[CFG_COMMON_STRING_128];                               // dropbox token
    BOOL                    bKeyValid;                                                           // token valid
}CFG_DROPBOXTOKEN_INFO;

// PtzDevice ����( ��Ӧ CFG_CMD_PTZDEVICE)
typedef struct tagCFG_PTZDEVICE_INFO
{
    CFG_COMM_PROP		    stuAttribute;							                             // ��������
	int					    nAddress;								                             // �豸��ַ
    int                     nPort;                                                               // ���ڶ˿ں�
    int                     nCurrentMode;                                                        // ����ģʽ��0�����ģʽ 1��Ӳ��ģʽ
    char				    szProtocolName[MAX_NAME_LEN];			                             // Э����
}CFG_PTZDEVICE_INFO;

// DevLocation ����( ��ӦCFG_CMD_DEVLOCATION)
typedef struct tagCFG_DEVLOCATION_INFO
{
    UINT							unLongitude;							                    // ���� ��λ�����֮һ�ȣ���Χ0-360��
	UINT							unLatitude;								                    // γ�� ��λ�����֮һ�ȣ���Χ0-360��
    double                          dbAltitude;                                                 // ���� ��λ��
    BOOL                            bConfigEnable;                                              // true��GPS��Ϣ���������ϱ�
	float							fHeight;													// �豸�߶� ��λ��
}CFG_DEVLOCATION_INFO;

// FireWarningExt ����( ��ӦCFG_CMD_FIRE_WARNING_EXT)
typedef struct tagCFG_FIREWARNING_EXT_INFO
{
	BOOL			bVisualOverviewEnable;	// �ɼ���ͨ���Ƿ���Ҫȫ��ͼ
	BOOL			bVisualDetailEnable;	// �ɼ���ͨ���Ƿ���Ҫϸ��ͼ
	UINT			nThermoSnapTimes;		// �ȳ���ͨ��ץͼ����
}CFG_FIREWARNING_EXT_INFO;

// �궨��Ϣ��Ԫ
typedef struct tagCFG_CALIBRATE_UNIT_INFO
{
	UINT		nHeight;			// �ֱ��ʸ�
	UINT		nWidth;				// �ֱ��ʿ�
	float		nPosition[3];		// ��̨λ��(P/T/Z ��һ��)
	UINT		nLocation[2];		// �궨������
	UINT		nHFOV;				// �����ӳ���(��λ��0.01��)
	UINT		nVFOV;				// �����ӳ���(��λ��0.01��)
}CFG_CALIBRATE_UNIT_INFO;

// �궨������Ϣ
typedef struct tagCFG_CALIBRATE_INFO
{
	CFG_CALIBRATE_UNIT_INFO stuCalibrateUnitInfo[5][2];		//�궨��Ϣ��Ԫ
	int nInfoNum;			//�궨����
}CFG_CALIBRATE_INFO;

// �궨����Ϣ
typedef struct tagCFG_LOCATION_CALIBRATE_POINT_INFO
{
	UINT		nID;							// �궨��ID
	char		szName[CFG_COMMON_STRING_128];	// �궨������
	BOOL		bEnable;						// ʹ��
	UINT		nLongitude;						// ����
	UINT		nLatitude;						// γ��
	double		fAltitude;						// ����
	CFG_CALIBRATE_INFO	stuCalibrateInfo;		// �궨�������Ϣ
	BYTE		byReserved1[4];					// �ֽڶ���
}CFG_LOCATION_CALIBRATE_POINT_INFO;

//LocationCalibrate ����( ��ӦCFG_CMD_LOCATION_CALIBRATE)
#define MAX_CALIBRATE_POINT_NUM 64 // ���궨�����
typedef struct tagCFG_LOCATION_CALIBRATE_INFO
{
	UINT		nVisualMaxHFOV;				// �ɼ�������ӽ�
	UINT		nVisualMaxVFOV;				// �ɼ��������ӽ�
	UINT		nThermoMaxHFOV;				// �ȳ�������ӽ�
	UINT		nThermoMaxVFOV;				// �ȳ��������ӽ�
	CFG_LOCATION_CALIBRATE_POINT_INFO	stuPointInfo[MAX_CALIBRATE_POINT_NUM];	// �궨����Ϣ
	int			nPointNum;					// �궨�����
	BYTE		byReserved1[4];				// �ֽڶ���
}CFG_LOCATION_CALIBRATE_INFO;


// ��������(��Ӧ CFG_CMD_IDLINGTIME)
typedef struct tagCFG_IDLINGTIME_INFO
{
	int						nIdlingTimeThreshold;		// ����ͣ����ֵ,��λ:��
	int						nSpeedThreshold;			// �����ж�����ٶ�,��λ:km/h
}CFG_IDLINGTIME_INFO;

// ������ʻ״̬����(��Ӧ CFG_CMD_CARDIVERSTATE)
typedef struct tagCFG_CARDIVERSTATE_INFO
{
	int						nAccelerationThreshold;		// �����ٷ�ֵ,��λ:0.1m/s^2
	int						nDecelerationThreshold;		// �����ٷ�ֵ,��λ:0.1m/s^2
}CFG_CARDIVERSTATE_INFO;

// ��������(��Ӧ CFG_CMD_VEHICLE)
typedef struct tagCFG_VEHICLE_INFO
{
	char					szPlateNumber[CFG_MAX_PLATE_NUMBER_LEN];			// ��������
}CFG_VEHICLE_INFO;

//���ܼҾӳ����б�����(��ӦCFG_CMD_SMARTHOME_SCENELIST)
typedef struct tagCFG_SMARTHOME_SCENELIST_INFO
{
	DWORD							dwSize;				//�ṹ���С
	char							szSceneID[32];		//����ID
	char 							szName[128];		//��������
}CFG_SMARTHOME_SCENELIST_INFO;


typedef enum tagEM_CFG_LC_LIGHT_TYPE
{
	EM_CFG_LC_LIGHT_TYPEUNKNOWN,						//δ֪
	EM_CFG_LC_LIGHT_TYPE_INFRAREDLIGHT,					//�����
	EM_CFG_LC_LIGHT_TYPE_WIHTELIGHT,					//�׹��
	EM_CFG_LC_LIGHT_TYPE_LASERLIGHT,					//�����
}EM_CFG_LC_LIGHT_TYPE;


typedef enum tagEM_CFG_LC_MODE
{
	EM_CFG_LC_MODE_UNKNOWN,								//δ֪
	EM_CFG_LC_MODE_MANUAL,								//�ֶ�
	EM_CFG_LC_MODE_ZOOMPRIO,							//��������
	EM_CFG_LC_MODE_TIMING,								//��ʱ
	EM_CFG_LC_MODE_AUTO,								//�Զ�
	EM_CFG_LC_MODE_OFF,									//�رյƹ�
	EM_CFG_LC_MODE_EXCLUSIVEMANUAL,						//֧�ֶ��ֵƹ�
	EM_CFG_LC_MODE_SMARTLIGHT,							//���ܵƹ�
	EM_CFG_LC_MODE_LINKING,								//�¼�����
	EM_CFG_LC_MODE_DUSKTODAWN							//����
}EM_CFG_LC_MODE;

#define CFG_LC_LIGHT_COUNT 4			//�ƹ���ƹ�����

//�ƹ���Ϣ
typedef struct tagNET_LIGHT_INFO
{
	int			nLight;						//���Ȱٷֱ�
	int			nAngle;						//����ƽǶȹ�һ��ֵ
}NET_LIGHT_INFO;

//�����������������Ϣ��Ԫ
typedef struct tagCFG_LIGHTING_V2_UNIT
{
	EM_CFG_LC_LIGHT_TYPE	emLightType;					//�ƹ�����
	EM_CFG_LC_MODE			emMode;							//�ƹ�ģʽ
	int					nCorrection;					//�ƹⲹ��
	int					nSensitive;						//�ƹ�������
	int					nLightSwitchDelay;				//����ƿ�����ʱ
	NET_LIGHT_INFO		anNearLight[CFG_LC_LIGHT_COUNT];	//���������Ϣ
	int					nNearLightLen;					//�����������
	NET_LIGHT_INFO		anMiddleLight[CFG_LC_LIGHT_COUNT];	//�й������Ϣ
	int					nMiddleLightLen;				//�й��������
	NET_LIGHT_INFO		anFarLight[CFG_LC_LIGHT_COUNT];		//Զ�������Ϣ
	int					nFarLightLen;					//Զ���������
    BYTE                byReserved[128];			    //Ԥ���ֽ�	
}CFG_LIGHTING_V2_UNIT;
#define LC_LIGHT_TYPE_NUM 3
//�����ҹ���������������
typedef struct tagCFG_LIGHTING_V2_DAYNIGHT
{
	CFG_LIGHTING_V2_UNIT		anLightInfo[LC_LIGHT_TYPE_NUM];			//�����͵ƹ���Ϣ
	int							nLightInfoLen;							//�ƹ���������
}CFG_LIGHTING_V2_DAYNIGHT;

#define CFG_LC_LIGHT_CONFIG 8                               // �����ҹ��Ӧ�ƹ�����������
//���������������
typedef struct tagCFG_LIGHTING_V2_INFO
{
	int							nChannel;					// ͨ��
    int                         nDNLightInfoNum;            // �����ҹ��Ӧ�ƹ���������             
	CFG_LIGHTING_V2_DAYNIGHT	anDNLightInfo[CFG_LC_LIGHT_CONFIG];			// �����ҹ��Ӧ�ƹ�����
                                                                            // ��Ԫ��0��ʼ�ֱ��ʾ ���졢ҹ����ͨ��˳�⡢һ����⡢ǿ��⡢���նȡ��Զ���
}CFG_LIGHTING_V2_INFO;

#define MAX_KBUSER_NUM				    100				    // ����Ա�û�������

//����Ա�û���Ϣ
typedef struct tagCFG_KBUSER_PASSWORD_INFO
{
    UINT                                       nUserCode;                        // �����û����
    char                                       szPassword[7];                     // �����û�����
    BYTE                                      bReserved[257];                   // �����ֽ�
}CFG_KBUSER_PASSWORD_INFO; 

//���̲���Ա�û�����(��ӦCFG_CMD_KBUSER_PASSWORD)
typedef struct tagCFG_KBUSER_USERS_INFO
{
    DWORD							             dwSize;				          // �ṹ���С
    int                                          nKbuserNum;                         // ����Ա�û�������һ��Ĭ��֧�ֵ�������Ϊ32
    CFG_KBUSER_PASSWORD_INFO                       stuKbuserPassword[MAX_KBUSER_NUM];             // ����Ա�û���Ϣ
}CFG_KBUSER_USERS_INFO;
//������״̬
typedef enum tagEM_GOLD_SERVICE
{
	EM_GOLD_SERVICE_UNKNOWN = -1,		//δ֪
	EM_GOLD_SERVICE_CLOSE,				//�ر�
	EM_GOLD_SERVICE_OPEN,				//����
	EM_GOLD_SERVICE_OTHER,				//����
}EM_GOLD_SERVICE;
//�Ž�����
typedef enum tagEM_GUARD_AREA
{
	EM_GUARD_AREA_UNKNOWN,			//δ֪
	EM_GUARD_AREA_HANDOVER,			//������
	EM_GUARD_AREA_CASH,				//�ֽ���
	EM_GUARD_AREA_NOBLEMETAL,		//�������
	EM_GUARD_AREA_DEPOSIT,			//�Ĵ��
	EM_GUARD_AREA_CUSTODY,			//�����ܿ�
	EM_GUARD_AREA_CHECK,			//���������
	EM_GUARD_AREA_GOLDSPLIT,		//�ƽ�ָ���
	EM_GUARD_AREA_GODLSALE,			//�ƽ������
	EM_GUARD_AREA_LOAD,				//װж��
	EM_GUARD_AREA_BUSINESS,			//Ӫҵ��
	EM_GUARD_AREA_OFFICE,			//�칫��
	EM_GUARD_AREA_DUTYROOM,			//���ֵ����
	EM_GUARD_AREA_OTHER,			//����
	EM_GUARD_AREA_VOUCHER,			//ƾ֤��
	EM_GUARD_AREA_ISSUE,			//���п�
	EM_GUARD_AREA_ACCOUNT,			//��������
}EM_GUARD_AREA;
//�Ž�����
typedef enum tagEM_GUARD_TYPE
{
	EM_GUARD_TYPE_UNKNOWN,			//δ֪
	EM_GUARD_TYPE_VAULT,			//�����
	EM_GUARD_TYPE_WORK,				//������
}EM_GUARD_TYPE;
//OEM����
typedef struct tagCFG_ACCESS_OEM_DATA
{
	char				szZoneNo[6];			//������
	BYTE				byReserved1[2];			//�����ֽ�
	char				szBrNo[6];				//�����
	BYTE				byReserved2[2];			//�����ֽ�
	EM_GUARD_AREA		emGuardArea;			//�Ž�����
	BYTE				byReserved3[1];			//�����ֽ�
	EM_GUARD_TYPE		emGuardType;			//�Ž�����
	BYTE				byReserved4[1];			//�����ֽ�
	char				szCorpId[11];			//���̱�ʶ
	BYTE				byReserved5[1];			//�����ֽ�
	char				szDeviceType[6];		//�豸����
	BYTE				byReserved6[2];			//�����ֽ�
	char				szLastModTime[20];		//����޸�ʱ��
	char				szLastModUser[11];		//����޸���
	BYTE				byReserved7[1];			//�����ֽ�
	EM_GOLD_SERVICE		emService;				//������״̬
	BYTE				byReserved[128];		//�����ֽ�
}CFG_ACCESS_OEM_DATA;
//���н���Ž�����
typedef struct tagCFG_ACCESS_OEM_INFO
{
	char					szCorporation[CFG_COMMON_STRING_32];		//OEM����
	unsigned int			nSuiteNumber;								//OEM�׼���
	CFG_ACCESS_OEM_DATA		stuData;									//OEM��Ϣ
	BYTE					byReserved[1024];							//�����ֽ�
}CFG_ACCESS_OEM_INFO;
//�ȳ������еĻ�о��������
typedef struct tagCFG_THERM_DENOISE_INFO
{
	BOOL							bEnable;					// ��о����ʹ��
	UINT							nDenoiseValue;				// ����ȼ�0-100, bEnableΪTRUEʱ��Ч
	BYTE							byReserved[128];			// �����ֽ�
}CFG_THERM_DENOISE_INFO;

typedef struct tagCFG_THERM_DENOISE
{
	CFG_THERM_DENOISE_INFO			stuThermDenoiseInfo[3];		// [0]:���죬[1]:��ҹ��[2]:��ͨ
	BYTE							byReserved[1024];			// �����ֽ�
}CFG_THERM_DENOISE;

// �ƹ⹤��ģʽ
typedef enum tagEM_LAMP_WORK_MODE
{
    EM_LAMP_UNKNOWN = -1,		// δ֪
    EM_LAMP_NO_FLASH,			// ��ֹ����
    EM_LAMP_ALWAYS_FLASH,		// ʼ������
    EM_LAMP_AUTO_FLASH,         // �Զ�����
}EM_LAMP_WORK_MODE;

// �ƹ��Զ�ģʽ
typedef enum tagEM_LAMP_AUTO_TYPE
{
    EM_LAMP_AUTO_UNKNOWN,			// δ֪
    EM_LAMP_AUTO_TIMEING,			// ��ʱ����ƿ���
    EM_LAMP_ALWAYS_BRIGHTNESS,		// ������
    EM_LAMP_AUTO_SPACE_STATE,       // ������ʻ��ʱ�豸���ƣ�һ��ʱ���رա�ʱ��ֵ�ɳ������ȡ����ֵ���ݲ����š�
    EM_LAMP_AUTO_ICRCUT,            // ���ư���ICR�л��ϣ�����ICR״̬����
}EM_LAMP_AUTO_TYPE;

// ��ͨ�ƹ������е�ʱ���
typedef struct tagTRAFFIC_LAMP_TIME
{
    CFG_TIME  stuTimeBegin;     // ��ʼʱ��
    CFG_TIME  stuTimeEnd;       // ����ʱ��
}TRAFFIC_LAMP_TIME;

// ��ͨ�ƹ�����
typedef struct tagTRAFFIC_CONSTATE_LAMP_CONFIG
{
    UINT                    nLightMask;                 // �ƿ�����,ĳЩ������6������,�����ò��ֵ�������λ��ʾ��1��ʾ����0��ʾ��
    UINT                    nBrightness;                // ����������[0,100]
    UINT                    nPreValue;                  // ����Ԥ��ֵ, �������ȵ��ڴ�ֵ�������ƿ�ʼ����
    EM_LAMP_WORK_MODE		emLampMode;			        // �ƹ⹤��ģʽ
    EM_LAMP_AUTO_TYPE       emAutoMode;                 // �Զ�ģʽ�µĹ�������
    int                     nNumOfTime;                 // ʱ��εĸ���
    TRAFFIC_LAMP_TIME       stuTimeSection[7];          // ʱ�������
    BYTE					byReserved[1024];	        // �����ֽ�
}TRAFFIC_CONSTATE_LAMP_CONFIG;

// ���ܽ�ͨ�ƹ�����
typedef struct tagCFG_CONSTANT_LAMP_INFO
{
    UINT                         nTrafficLampNum;    // ���ܽ�ͨ�ƹ����ø���
    TRAFFIC_CONSTATE_LAMP_CONFIG stuTrafficLamp[16];	 // �ƹ����������
}CFG_CONSTANT_LAMP_INFO;

// ��Ȧ����ģʽ
typedef enum tagEM_TRAFFOCIO_WORKMODE
{
    EM_TRAFFOCIO_UNKNOWN = -1,			// δ֪
    EM_TRAFFOCIO_SINGLE_COIL,			// ����Ȧ����ģʽ
    EM_TRAFFOCIO_DOUBLE_COIL_SPEED,		// ˫��Ȧ����ģʽ
    EM_TRAFFOCIO_DOUBLE_COIL_TRAFFIC,	// ˫��Ȧ��ͨģʽ
}EM_TRAFFOCIO_WORKMODE;


// ��Ȧ������ʽ
typedef enum tagEM_COIL_TRIGGER_MODE
{
    EM_TRIGGER_UNKNOWN = -1,		// δ֪
    EM_TRIGGER_NO_TRIGGER,			// ������
    EM_TRIGGER_RISING_EDGE,			// ������
    EM_TRIGGER_FALLING_EDGE,		// �½���
    EM_TRIGGER_RISING_FALLING,		// �����½���
}EM_COIL_TRIGGER_MODE;

// ��Ȧ��������
typedef enum tagEM_COIL_CONFIG_TYEP
{
    EM_COIL_CONFIG_UNKNOWN = -1,		// δ֪
    EM_COIL_CONFIG_GATE,	    		// ������������
    EM_COIL_CONFIG_ELE_POLICE,			// �羯��������
}EM_COIL_CONFIG_TYEP;


// ץ��ʱ��
typedef enum tagEM_SNAP_TRIGGER_MODE
{
    EM_SNAP_TRIGGER_UNKNOWN = -1,		// δ֪
    EM_SNAP_TRIGGER_NO_TRIGGER,		    // ������
    EM_SNAP_TRIGGER_IN_TRIGGER,		    // ����Ȧ����
    EM_SNAP_TRIGGER_OUT_TRIGGER,		// ����Ȧ����
    EM_SNAP_TRIGGER_INOUT_TRIGGER,		// ������Ȧ������
}EM_SNAP_TRIGGER_MODE;

// ��Ʒ���
typedef enum tagEM_RED_DIRECTION
{
    EM_RED_DIRECTION_UNKNOWN = -1,		// δ֪
    EM_RED_DIRECTION_NO_RELATED,        // ������
    EM_RED_DIRECTION_LEFT,              // ��ת���
    EM_RED_DIRECTION_STRAIGT,           // ֱ�к��
    EM_RED_DIRECTION_RIGHT,             // ��ת���
    EM_RED_DIRECTION_WAIT_LEFT,         // ����
    EM_RED_DIRECTION_WAIT_RIGHT,        // ����
    EM_RED_DIRECTION_STRAIGT_LEFT,      // ֱ����ת
    EM_RED_DIRECTION_STRAIGT_RIGHT,     // ֱ����ת���羯��Ч
}EM_RED_DIRECTION;


// ��������Ȧ����
typedef struct tagCFG_TRAFFICIO_LANES_COILS
{
    UINT                        nCoilID;                // ��ȦID
    EM_COIL_TRIGGER_MODE        emTriggerMode;          // ������ʽ
    EM_COIL_CONFIG_TYEP         emType;                 // ��������
    UINT                        nDelayFlashID;          // ��ʱ��������,��Χ0~5��0��ʾ����ʱ�κ������
    UINT                        nFlashSerialID;         // ��������,��Χ0~5��0��ʾ���������
    EM_RED_DIRECTION            emRedDirection;         // ��Ʒ���
    UINT                        nNextTriggerTime;       // �´δ���ʱ��
    UINT                        nDelayTriggerTime;      // ��ʱ����ʱ��,msΪ��λ��0��ʾ��������
    EM_SNAP_TRIGGER_MODE        emSnapTriggerMode;      // ץ��ʱ��
}CFG_TRAFFICIO_LANES_COILS;

// ��ͨ��Ȧ�ĳ�������
typedef struct tagCFG_TRAFFICIO_LANES
{
    UINT                        nLaneNumber;            // ������,�����ŷ�Χ��0-7
    UINT                        nCarScheme;             // �������÷���,����ķ������������巽������ο��������local.png
    EM_TRAFFOCIO_WORKMODE       emWorkMode;             // ������ʽ
    int                         nLanesCoils;            // ��Ȧ���õĸ���
    CFG_TRAFFICIO_LANES_COILS   stuLanesCoils[32];      // ��Ȧ����
    BYTE					    byReserved[2048];	    // �����ֽ�
}CFG_TRAFFICIO_LANES;

// ��ȦIO����
typedef struct tagCFG_TRAFFIC_IO
{
    BOOL                bEnable;                 // ʹ��
    UINT                nLanes;                  // ��������
    CFG_TRAFFICIO_LANES stuTrafficIoLanes[16];   // ��������
}CFG_TRAFFIC_IO;

// Ԥ��������һ������
typedef struct tagNET_COLLECTION_NAME_INFOS
{
    char        szName[32];                         // ��������
    char        szVideoName[32];                    // ��ƵԤ��������
    char        szAdditionalName[32];               // ����Ԥ��������
    char        szControlID[32];                    // ���Ʊ��
    BYTE		byReserved[64];	                    // �����ֽ�
}NET_COLLECTION_NAME_INFOS;

// Ԥ��������һ������
typedef struct tagNET_MONITOR_WALL_COLLECTION_MAP_INFO
{
   NET_COLLECTION_NAME_INFOS        stuCollection[32];          // Ԥ������,һ������ǽ��������32��Ԥ��
   int                              nCollectionCount;           // Ԥ�����õĸ���
   BYTE		                        byReserved[132];	        // �����ֽ�
}NET_MONITOR_WALL_COLLECTION_MAP_INFO;

// ����ǽԤ��������,һά���飬ÿһ��Ԫ�ض�Ӧһ������ǽ��Ӧ�ṹ��
typedef struct tagCFG_MONITOR_WALL_COLLECTION_MAP_INFO
{   
    DWORD                                   dwSize;                             // �ṹ���С
    int                                     nWallCount;                         // ����ǽ�ĸ���
    NET_MONITOR_WALL_COLLECTION_MAP_INFO    stuMonitorWallCollectioInfo[16];    // ����ǽԤ����Ϣ��16��ʾ���֧��16������ǽǽ��Ԥ������
}CFG_MONITOR_WALL_COLLECTION_MAP_INFO;

// ��Ƶ��������
typedef enum tagEM_INPUT_STREAM_TYPE
{
    EM_INPUT_STREAM_UNKNOWN,		    // δ֪
    EM_INPUT_STREAM_MAIN,		        // ��Main��-������
    EM_INPUT_STREAM_EXTRA1,		        // ��Extra1��-������1
    EM_INPUT_STREAM_EXTRA2,		        // ��Extra2��-������2
    EM_INPUT_STREAM_EXTRA3,		        // ��Extra3��-������3
}EM_INPUT_STREAM_TYPE;

// �豸���ӷ�ʽ
typedef enum tagEM_CONNECTING_METHOD
{
    EM_CONNECTING_UNKNOWN,		        // δ֪
    EM_CONNECTING_DIRECT,		        // ��Direct��, ֱ���豸
    EM_CONNECTING_VIADSS,		        // ��ViaDSS��, ������ƽ̨������
}EM_CONNECTING_METHOD;

// ����Ȩ����֤��Ϣ������Ԥ����
typedef struct tagNET_CASCADE_AUTHENTICATOR
{
    char            szUserName[128];        // �û���
    char            szPassWord[128];        // ����
    char            szSerialNo[64];         // ��֤�豸���к�
    BYTE            bReserver[128];         // �����ֽ�
}NET_CASCADE_AUTHENTICATOR;

// ������Ƶ��Ϣ
typedef struct tagNET_VIDEO_INPUT_INFO
{
    char                            szDevice[32];           // �豸Ψһ��־
    UINT                            nChannel;               // ��Ƶ����ͨ����
    UINT                            nInterval;              // ��Ѳʱ����,��λ��,��Χ[5,120]
    EM_INPUT_STREAM_TYPE            emStream;               // ��Ƶ��������
    EM_CONNECTING_METHOD            emConnectingMethod;     // �豸���ӷ�ʽ
    NET_CASCADE_AUTHENTICATOR       stuCascadeAuthenticator; //����Ȩ����֤��Ϣ������Ԥ����
    AV_CFG_RemoteDevice             stuDevice;              // �豸��ϸ��Ϣ,ֻ�ɻ�ȡ���ɸ���
    BYTE		                    byReserved[256];	    // �����ֽ�
}NET_VIDEO_INPUT_INFO;

// Ԥ��������һ������
typedef struct tagNET_VIDEO_INPUT_GROUP_INFO
{
    char                            szName[32];             // ����������
    char                            szControlID[32];        // ���Ʊ��
    BYTE		                    byReserved1[4];	        // �����ֽ�,�����ֽڶ���
    int                             nListCount;             // ��Ƶ�����б������        
    NET_VIDEO_INPUT_INFO            stuVideoInput[64];      // ��Ƶ������Ϣ���飬���֧��64��
    BYTE		                    byReserved[256];	    // �����ֽ�
}NET_VIDEO_INPUT_GROUP_INFO;

// ��Ƶ����������
typedef struct tagCFG_VIDEO_INPUT_GROUP_INFO
{   
    DWORD                           dwSize;                 // �ṹ���С
    BYTE		                    byReserved1[4];	        // �����ֽ�,�����ֽڶ���
    int					            nMaxGroupCount;			// ����Ϣ���������С, �û���д
    int					            nRetGroupCount;			// ���ص�����Ϣ����
    NET_VIDEO_INPUT_GROUP_INFO*     pGroupInfo;             // ���������,�û��Լ������ڴ�
}CFG_VIDEO_INPUT_GROUP_INFO;

/************************************************************************
** �ӿڶ���
***********************************************************************/
// �����¼����͸���dhnetsdk.h���������ı�������(pAlarmInfo, pBuffer�ڴ���SDK�ڲ������ͷ�)
typedef int  (CALLBACK *fALARMEvent)(DWORD dwAlarmType, void* pAlarmInfo, DWORD dwStructSize, BYTE *pBuffer, DWORD dwBufSize, LDWORD dwUser);

// �ص���ʽ�������ݽ�����dwDataType��ʾ���ݽ����õ�����(pDataInfo, pBuffer�ڴ���SDK�ڲ������ͷ�) 
typedef int  (CALLBACK *fAnalyzerData)(DWORD dwDataType, void* pDataInfo, DWORD dwStructSize, BYTE *pBuffer, DWORD dwBufSize, LDWORD dwUser);

// ������ѯ����������Ϣ(szInBuffer��lpOutBuffer�ڴ����û������ͷ�)
CLIENT_CFG_API BOOL  CALL_METHOD CLIENT_ParseData(char* szCommand, char* szInBuffer, LPVOID lpOutBuffer, DWORD dwOutBufferSize, void* pReserved);

// ���Ҫ���õ�������Ϣ(lpInBuffer��szOutBuffer�ڴ����û������ͷ�)
CLIENT_CFG_API BOOL  CALL_METHOD CLIENT_PacketData(char* szCommand, LPVOID lpInBuffer, DWORD dwInBufferSize, char* szOutBuffer, DWORD dwOutBufferSize);

// ¼�񱨾��¼�����(lpInBuffer�ڴ����û������ͷ�)
CLIENT_CFG_API BOOL  CALL_METHOD CLIENT_ParseAnalyzerEventData(LPVOID lpInBuffer, DWORD dwInBufferSize, fALARMEvent cbEvent, LDWORD dwUser);

// �ص���ʽ���ݽ���(lpInBuffer�ڴ����û������ͷ�)
CLIENT_CFG_API BOOL CALL_METHOD CLIENT_ParseDataByCallback(LPVOID lpInBuffer, DWORD dwInBufferSize, fAnalyzerData cbAnalyzer, LDWORD dwUser);

// ������ȡ��������ȫ������/����ģ������/ָ�������µ����ܹ�������(szInBuffer�ڴ����û������ͷ�)
CLIENT_CFG_API BOOL CALL_METHOD CLIENT_ParseVideoInAnalyse(char* szCommand, EM_SCENE_TYPE emSceneType, char *szInBuffer, DWORD dwInBufferSize, LPVOID lpOutParam, DWORD dwOutParamSize);


#ifdef __cplusplus
}
#endif

#endif // DHCONFIGSDK_H



