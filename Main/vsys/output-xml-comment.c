#include "output-xml-comment.h"
#include <malloc.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include "../../PublicTools/Fs/Time.h"
#include "../../PublicTools/Fs/FileTool.h"
#include "recognition.h"
////////////////////////////////////////////////////////////////////////////////
///////////////////////////定义私有结构开始//////////////////////////////////////
#ifdef FsPrintfIndex
#undef FsPrintfIndex
#endif 
#define FsPrintfIndex 1
///////////////////////////定义私有结构结束//////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
///////////////////////////定义私有函数开始//////////////////////////////////////

///////////////////////////定义私有函数结束//////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
///////////////////////////定义公共函数开始//////////////////////////////////////

/* 记录事件,成功返回保存的文件名,文件名的内存空间在*pBuffer内,失败返回NULL */
char * output_xml_comment_watch(struct Record_item * const pRecord_item, struct Record_item_Watch * const pWatch
        , /* 共享buffer,不为空 */ FsShareBuffer * const pShareBuffer, MonitorItem * const pMonitorItem) {
#undef Fs_ShareBuffer_var    
#undef Fs_ShareBuffer_var_check
#undef Fs_ShareBuffer_var_array
#undef Fs_ShareBuffer_var_array_check
    /* 初始化共享buffer的使用环境 */ FsLocal_ShareBuffer_init(pShareBuffer, 1);
    /* 定义与共享buffer--FsLocal_ShareBuffer关联的局部变量,以_in结尾,多个变量使用逗号隔开,如#define Fs_ShareBuffer_var a_in_acin,b_in */
#define Fs_ShareBuffer_var
    /* 定义可能与共享buffer--FsLocal_ShareBuffer关联的局部变量,以_cin结尾,多个变量使用逗号隔开,如#define Fs_ShareBuffer_var_check a_cin_acin,b_cin */
#define Fs_ShareBuffer_var_check
    /* 定义与共享buffer--FsLocal_ShareBuffer关联的局部指针数组变量,以_ain结尾,需要指明数组成员的个数,多个变量使用逗号隔开,如#define Fs_ShareBuffer_var_array a_cin_ain,2 */
#define Fs_ShareBuffer_var_array
    /* 定义可能与共享buffer--FsLocal_ShareBuffer关联的局部指针数组变量,以_acin结尾,需要指明数组成员的个数,多个变量使用逗号隔开,如#define Fs_ShareBuffer_var_array_check a_cin_acin,2 */
#define Fs_ShareBuffer_var_array_check
    const double starttime = pWatch->_ppFrame[0]->capture_gmtTime;
    double endtime = pWatch->_ppFrame[0]->capture_gmtTime;
    int pos = FsLocal_ShareBufferPos;
    // uuid
    FsSnprintf(pos, "<uuid comment=\"通道编号,服务器的唯一标识符\">%s</uuid>\r\n", pRecord_item->ro._uuid);
    // 设备编号
    FsSnprintf(pos, "<deviceno comment=\"设备编号\">%s</deviceno>\r\n", pRecord_item->ro._deviceno);
    // 设备名
    FsSnprintf(pos, "<devicename comment=\"设备名\">%s</devicename>\r\n", pRecord_item->ro._devicename);
    // 安装位置
    FsSnprintf(pos, "<positionname comment=\"安装位置\">%s</positionname>\r\n", pRecord_item->ro._positionname);
    // 位置信息,用经纬度表示
    FsSnprintf(pos, "<position_jwd comment=\"位置信息,用经纬度表示,如10,10\">%s</position_jwd>\r\n", pRecord_item->ro._position_jwd);
    // 目标编号
    FsSnprintf(pos, "<targetno comment=\"目标编号\">%llu</targetno>\r\n", pWatch->_objIndex);
    // 目标输出编号
    FsSnprintf(pos, "<targetoutindex comment=\"目标输出编号,相同目标编号,应使用输出编号大的信息作为目标数据\">%u</targetoutindex>\r\n", pWatch->_recordIndex);
    // 目标位置
    FsSnprintf(pos, "<targetpos_x1 comment=\"目标位置,左上角的x坐标\">%hd</targetpos_x1>\r\n", pWatch->x1);
    FsSnprintf(pos, "<targetpos_y1 comment=\"目标位置,左上角的y坐标\">%hd</targetpos_y1>\r\n", pWatch->y1);
    FsSnprintf(pos, "<targetpos_x2 comment=\"目标位置,右下角的x坐标\">%hd</targetpos_x2>\r\n", pWatch->x2);
    FsSnprintf(pos, "<targetpos_y2 comment=\"目标位置,右下角的y坐标\">%hd</targetpos_y2>\r\n", pWatch->y2);
    /* 车牌信息 */
    const char *plateno = NULL;
    if (pWatch->_pPlate) {
        struct Recognition_item_Plate * const pPlate = pWatch->_pPlate;
        FsSnprintf(pos, "<plate comment=\"车牌信息\">\r\n");
        if (3 == pPlate->_tag) {
            plateno = pPlate->plateNo;
            FsSnprintf(pos, "<plateno comment=\"车牌号\">%s</plateno>\r\n", plateno);
            FsSnprintf(pos, "<platepos_x1 comment=\"车牌位置,左上角的x坐标\">%hd</platepos_x1>\r\n", pPlate->platepos_x1);
            FsSnprintf(pos, "<platepos_y1 comment=\"车牌位置,左上角的y坐标\">%hd</platepos_y1>\r\n", pPlate->platepos_y1);
            FsSnprintf(pos, "<platepos_x2 comment=\"车牌位置,右下角的x坐标\">%hd</platepos_x2>\r\n", pPlate->platepos_x2);
            FsSnprintf(pos, "<platepos_y2 comment=\"车牌位置,右下角的y坐标\">%hd</platepos_y2>\r\n", pPlate->platepos_y2);
        }
        FsSnprintf(pos, "</plate>\r\n");
    }
    if (NULL == plateno)plateno = "-------";
    /* 图片数据 */
    {
        const char *picName[] = {"1.jpg"};
        int i;
        for (i = 0; i < (int) Fs_Array_len(pWatch->_ppFrame); i++) {
            FsSnprintf(pos, "<pic%d comment=\"第%d张图片\">\r\n", i + 1, i + 1);
            FsSnprintf(pos, "<name comment=\"图片名,如1.jpg\">%s</name>\r\n", picName[i]);
            time_t ts = (time_t) pWatch->_ppFrame[i]->capture_gmtTime;
            struct tm ti_s;
            gmtime_r(&ts, &ti_s);
            FsSnprintf(pos, "<capturetime comment=\"抓拍时间,0时区的时间,如2017-06-19/18:00:00.123456\">%04d-%02d-%02d/%02d:%02d:%02d.%06d</capturetime>\r\n"
                    , 1900 + ti_s.tm_year, ti_s.tm_mon + 1, ti_s.tm_mday, ti_s.tm_hour, ti_s.tm_min, ti_s.tm_sec, (int) (1000000 * (pWatch->_ppFrame[i]->capture_gmtTime - ts)));
            FsSnprintf(pos, "<position_jwd comment=\"位置信息,用经纬度表示,如10,10\">%s</position_jwd>\r\n", pRecord_item->ro._position_jwd);
            FsSnprintf(pos, "<img_width comment=\"图像宽度\">%u</img_width>\r\n", pWatch->_ppFrame[i]->width[0]);
            FsSnprintf(pos, "<img_height comment=\"图像高度\">%u</img_height>\r\n", pWatch->_ppFrame[i]->height[0]);
            ts = (time_t) starttime;
            localtime_r(&ts, &ti_s);
            if (endtime > 0.0) ts = (time_t) endtime;
            struct tm ti_e;
            localtime_r(&ts, &ti_e);
            FsSnprintf(pos, "<$$data comment=\"图像数据,对xml和json均使用base64编码,编码数据不会填充=,需要使用=填充的解码器请先填充后解码\">"
                    "watch%04d%02d%02d%02d%02d%02d-%04d%02d%02d%02d%02d%02d-%10llu%03u%02hhu(%s)%d.jpg</$$data>\r\n"
                    , 1900 + ti_s.tm_year, ti_s.tm_mon + 1, ti_s.tm_mday, ti_s.tm_hour, ti_s.tm_min, ti_s.tm_sec
                    , 1900 + ti_e.tm_year, ti_e.tm_mon + 1, ti_e.tm_mday, ti_e.tm_hour, ti_e.tm_min, ti_e.tm_sec
                    , pWatch->_objIndex, pWatch->_recordIndex, pRecord_item->p.recordIndex, plateno, i);
            FsSnprintf(pos, "</pic%d>\r\n", i + 1);
            /* 写图片 */
            {
                int fileNamePos = pos;
                FsSnprintf(fileNamePos, "%s%s/watch/%04d%02d%02d/%02d/watch%04d%02d%02d%02d%02d%02d-%04d%02d%02d%02d%02d%02d-%10llu%03u%02hhu(%s)%d.jpg"
                        , pRecord_item->ro._pRecord->ro._dir, pRecord_item->ro._uuid, 1900 + ti_s.tm_year, ti_s.tm_mon + 1, ti_s.tm_mday, ti_s.tm_hour
                        , 1900 + ti_s.tm_year, ti_s.tm_mon + 1, ti_s.tm_mday, ti_s.tm_hour, ti_s.tm_min, ti_s.tm_sec
                        , 1900 + ti_e.tm_year, ti_e.tm_mon + 1, ti_e.tm_mday, ti_e.tm_hour, ti_e.tm_min, ti_e.tm_sec
                        , pWatch->_objIndex, pWatch->_recordIndex, pRecord_item->p.recordIndex, plateno, i);
            }
            {
                FsObjectImageJpeg * const pJpeg = (FsObjectImageJpeg *) image_frame_get_pthreadSafety__IO(pWatch->_ppFrame[i], ImageFrame_Jpeg_0);
                if (NULL == pJpeg) {
                    FsPrintf(FsPrintfIndex, "Prapare jpeg for file:\"%s\" failed,dataLen=%d.\n", FsLocal_ShareBuffer + pos, pos);
                    fflush(stdout);
                    /* 释放共享buffer */ FsLocal_ShareBuffer_release(pShareBuffer, 0);
                    return NULL;
                }
                fs_dir_make(FsLocal_ShareBuffer + pos, 0777);
                if (fs_String_buffer_save_to_file(pJpeg->datalenth, pJpeg->data, FsLocal_ShareBuffer + pos) != 1) {
                    fs_Object_delete_pthreadSafety__OI(pJpeg);
                    FsPrintf(FsPrintfIndex, "Save data to the file:\"%s\" failed,dataLen=%u.\n", FsLocal_ShareBuffer + pos, pJpeg->datalenth);
                    fflush(stdout);
                    /* 释放共享buffer */ FsLocal_ShareBuffer_release(pShareBuffer, 0);
                    return NULL;
                }
                fs_Object_delete_pthreadSafety__OI(pJpeg);
            }
        }
    }
    /* 准备文件名 */
    {
        time_t ts = (time_t) starttime;
        struct tm ti_s;
        localtime_r(&ts, &ti_s);
        if (endtime > 0.0) ts = (time_t) endtime;
        struct tm ti_e;
        localtime_r(&ts, &ti_e);
        {
            int fileNamePos = pos;
            FsSnprintf(fileNamePos, "%s%s/watch/%04d%02d%02d/%02d/watch%04d%02d%02d%02d%02d%02d-%04d%02d%02d%02d%02d%02d-%10llu%03u%02hhu(%s).ui"
                    , pRecord_item->ro._pRecord->ro._dir, pRecord_item->ro._uuid, 1900 + ti_s.tm_year, ti_s.tm_mon + 1, ti_s.tm_mday, ti_s.tm_hour
                    , 1900 + ti_s.tm_year, ti_s.tm_mon + 1, ti_s.tm_mday, ti_s.tm_hour, ti_s.tm_min, ti_s.tm_sec
                    , 1900 + ti_e.tm_year, ti_e.tm_mon + 1, ti_e.tm_mday, ti_e.tm_hour, ti_e.tm_min, ti_e.tm_sec
                    , pWatch->_objIndex, pWatch->_recordIndex, pRecord_item->p.recordIndex, plateno);
        }
        fs_dir_make(FsLocal_ShareBuffer + pos, 0777);
    }
    /* 释放共享buffer */ FsLocal_ShareBuffer_release(pShareBuffer, 1);
    FsPrintf(FsPrintfIndex, "file:\"%s\"\n", FsLocal_ShareBuffer + pos);
    if (fs_String_buffer_save_to_file(pos - FsLocal_ShareBufferPos, FsLocal_ShareBuffer + FsLocal_ShareBufferPos, FsLocal_ShareBuffer + pos) != 1) {
        FsPrintf(FsPrintfIndex, "Save data to the file:\"%s\" failed,dataLen=%u.\n", FsLocal_ShareBuffer + pos, pos - FsLocal_ShareBufferPos);
        fflush(stdout);
        return NULL;
    } else return FsLocal_ShareBuffer + pos;
}

/* 记录事件,成功返回保存的文件名,文件名的内存空间在pShareBuffer内,失败返回NULL */
char *output_xml_comment_inc(struct Record_item * const pRecord_item, struct Record_item_Inc * const pInc
        , /* 共享buffer,不为空 */ FsShareBuffer * const pShareBuffer, MonitorItem * const pMonitorItem) {
#undef Fs_ShareBuffer_var    
#undef Fs_ShareBuffer_var_check
#undef Fs_ShareBuffer_var_array
#undef Fs_ShareBuffer_var_array_check
    /* 初始化共享buffer的使用环境 */ FsLocal_ShareBuffer_init(pShareBuffer, 1);
    /* 定义与共享buffer--FsLocal_ShareBuffer关联的局部变量,以_in结尾,多个变量使用逗号隔开,如#define Fs_ShareBuffer_var a_in_acin,b_in */
#define Fs_ShareBuffer_var
    /* 定义可能与共享buffer--FsLocal_ShareBuffer关联的局部变量,以_cin结尾,多个变量使用逗号隔开,如#define Fs_ShareBuffer_var_check a_cin_acin,b_cin */
#define Fs_ShareBuffer_var_check
    /* 定义与共享buffer--FsLocal_ShareBuffer关联的局部指针数组变量,以_ain结尾,需要指明数组成员的个数,多个变量使用逗号隔开,如#define Fs_ShareBuffer_var_array a_cin_ain,2 */
#define Fs_ShareBuffer_var_array
    /* 定义可能与共享buffer--FsLocal_ShareBuffer关联的局部指针数组变量,以_acin结尾,需要指明数组成员的个数,多个变量使用逗号隔开,如#define Fs_ShareBuffer_var_array_check a_cin_acin,2 */
#define Fs_ShareBuffer_var_array_check
    static const char* typeDoc[] = {
        NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
        "qqldgcxx"
    };
    double starttime = pInc->_starttime - fs_time_uptime_get() + fs_time_GMT_get();
    double endtime = pInc->_endtime > 0.0 ? pInc->_endtime - pInc->_starttime + starttime : pInc->_endtime;
    time_t ts;
    struct tm ti_s, ti_e;
    int pos = FsLocal_ShareBufferPos, i, fileNamePos;
    /* 设备编号 */
    FsSnprintf(pos, "<deviceno comment=\"设备编号\">%s</deviceno>\r\n", pRecord_item->ro._deviceno);
    /* 设备名 */
    FsSnprintf(pos, "<devicename comment=\"设备名\">%s</devicename>\r\n", pRecord_item->ro._devicename);
    /* 事件类型 */
    FsSnprintf(pos, "<typedoc comment=\"事件类型,qqldgcxx(枪球联动过船信息)\">%s</typedoc>\r\n", typeDoc[pInc->_type]);
    /* 事件编号 */
    FsSnprintf(pos, "<datano comment=\"事件编号\">%s-%ld-%02hhu</datano>\r\n", pRecord_item->ro._uuid, (long) starttime, pRecord_item->p.recordIndex);
    /* 开始时间 */
    ts = (time_t) starttime;
    gmtime_r(&ts, &ti_s);
    FsSnprintf(pos, "<starttime comment=\"开始时间,0时区的时间,如2017-06-19/18:00:00.123456\">%04d-%02d-%02d/%02d:%02d:%02d.%06d</starttime>\r\n",
            1900 + ti_s.tm_year, ti_s.tm_mon + 1, ti_s.tm_mday, ti_s.tm_hour, ti_s.tm_min, ti_s.tm_sec, (int) (1000000 * (starttime - ts)));
    /* 结束时间 */
    if (endtime > 0.0) {
        ts = (time_t) endtime;
        gmtime_r(&ts, &ti_e);
        FsSnprintf(pos, "<endtime comment=\"结束时间,为空或节点不存在表示此事件没有结束时间,0时区的时间,如2017-06-19/18:00:00.123456\">%04d-%02d-%02d/%02d:%02d:%02d.%06d</endtime>\r\n",
                1900 + ti_e.tm_year, ti_e.tm_mon + 1, ti_e.tm_mday, ti_e.tm_hour, ti_e.tm_min, ti_e.tm_sec, (int) (1000000 * (endtime - ts)));
    } else {
        FsSnprintf(pos, "<endtime comment=\"结束时间,为空或节点不存在表示此事件没有结束时间,0时区的时间,如2017-06-19/18:00:00.123456\"></endtime>\r\n");
    }
    /* 实时流地址 */
    FsSnprintf(pos, "<realurl comment=\"实时流地址,只包含url信息,如/1,使用时应根据具体协议拼装\">%s</realurl>\r\n", pRecord_item->ro.__pRtspServer_item->r._url);
    /* 事件录像 */
    ts = (time_t) starttime;
    localtime_r(&ts, &ti_s);
    if (endtime > 0.0) ts = (time_t) endtime;
    localtime_r(&ts, &ti_e);
    FsSnprintf(pos, "<incvideo comment=\"事件录像,只包含url信息,如/1.mkv,使用时应根据具体协议拼装,加rtsp可使用rtsp点播,加ftp可使用ftp下载\">"
            "%s%s/incident/%04d%02d%02d/%02d/inc%04d%02d%02d%02d%02d%02d-%04d%02d%02d%02d%02d%02d-%02hhx%02hhu.mkv"
            "</incvideo>\r\n", pRecord_item->ro._pRecord->ro._dir + strlen("/fs/project/data"),
            pRecord_item->ro._uuid, 1900 + ti_s.tm_year, ti_s.tm_mon + 1, ti_s.tm_mday, ti_s.tm_hour,
            1900 + ti_s.tm_year, ti_s.tm_mon + 1, ti_s.tm_mday, ti_s.tm_hour, ti_s.tm_min, ti_s.tm_sec,
            1900 + ti_e.tm_year, ti_e.tm_mon + 1, ti_e.tm_mday, ti_e.tm_hour, ti_e.tm_min, ti_e.tm_sec,
            pInc->_type, pRecord_item->p.recordIndex);
    fileNamePos = pos;
    FsSnprintf(fileNamePos, "%s%s/incident/%04d%02d%02d/%02d/inc%04d%02d%02d%02d%02d%02d-%04d%02d%02d%02d%02d%02d-%02hhx%02hhu.mkv", pRecord_item->ro._pRecord->ro._dir,
            pRecord_item->ro._uuid, 1900 + ti_s.tm_year, ti_s.tm_mon + 1, ti_s.tm_mday, ti_s.tm_hour,
            1900 + ti_s.tm_year, ti_s.tm_mon + 1, ti_s.tm_mday, ti_s.tm_hour, ti_s.tm_min, ti_s.tm_sec,
            1900 + ti_e.tm_year, ti_e.tm_mon + 1, ti_e.tm_mday, ti_e.tm_hour, ti_e.tm_min, ti_e.tm_sec,
            pInc->_type, pRecord_item->p.recordIndex);
    record_item_add_partVideo_pthreadSafety(pRecord_item, FsLocal_ShareBuffer + pos, pInc->_ppFrame[0]->capture_uptime - 10.0, pInc->_ppFrame[0]->capture_uptime + 20.0, NULL);
    /* 触发信息 */
    FsSnprintf(pos, "<triggerinfo comment=\"触发信息\">\r\n");
    {
        /* 触发编号 */
        FsSnprintf(pos, "<triggerno comment=\"触发编号,如qjjc_1233,格式为触发方式(枪机检测)+'_'+目标编号\">qjjc_%s-%ld-%02hhu</triggerno>\r\n",
                pRecord_item->ro._uuid, (long) starttime, pRecord_item->p.recordIndex);
        /* 触发的方向,目标在画面中的运动方向,0:表示从左往右运动;1表示从右往左运动 */
        FsSnprintf(pos, "<obj_dir comment=\"触发的方向,目标在画面中的运动方向,0:表示从左往右运动;1表示从右往左运动\">%hhu</obj_dir>\r\n", pInc->trigger.qqld.direction);
        /* 触发的区域,用经纬度表示 */
        FsSnprintf(pos, "<area_jwd comment=\"触发的区域,用经纬度表示,如(10,10)(20,20) 每个点用括号括起来,点的个数不定\">%s</area_jwd>\r\n", pInc->trigger.qqld.area_jwd);
        /* 触发的设备名 */
        FsSnprintf(pos, "<devicename comment=\"触发的设备名\">%s</devicename>\r\n", pInc->trigger.qqld.devicename);
        /* 触发的ip地址 */
        FsSnprintf(pos, "<ipaddr comment=\"触发的ip地址\">%hhu.%hhu.%hhu.%hhu</ipaddr>\r\n",
                ((unsigned char*) &pInc->trigger.qqld.ipaddr)[0], ((unsigned char*) &pInc->trigger.qqld.ipaddr)[1], ((unsigned char*) &pInc->trigger.qqld.ipaddr)[2], ((unsigned char*) &pInc->trigger.qqld.ipaddr)[3]);
        /* 触发的实时流地址 */
        FsSnprintf(pos, "<realurl comment=\"触发的实时流地址,只包含url信息,如/1,使用时应根据具体协议拼装\">%s</realurl>\r\n", pInc->trigger.qqld.realurl);
        /* 触发的事件录像 */
        FsSnprintf(pos, "<incvideo comment=\"触发的事件录像,只包含url信息,如/1.mkv,使用时应根据具体协议拼装,加rtsp可使用rtsp点播,加ftp可使用ftp下载\">%s</incvideo>\r\n",
                pInc->trigger.qqld.incVideo);
    }
    FsSnprintf(pos, "</triggerinfo>\r\n");
    /* 图片数据 */
    const char *picName[] = {"shipstern_001.jpg", "shipbow_001.jpg", "shipbody_001.jpg", "other_001.jpg", "other_002.jpg"};
    for (i = 0; i < (int) Fs_Array_len(pInc->_ppFrame); i++) {
        FsSnprintf(pos, "<pic%d comment=\"第%d张图片\">\r\n", i + 1, i + 1);
        FsSnprintf(pos, "<name comment=\"图片名,如shipstern_001.jpg;shipbow_001.jpg;shipbody_001.jpg;other_xxx.jpg\">%s</name>\r\n", picName[i]);
        ts = (time_t) pInc->_ppFrame[i]->capture_gmtTime;
        gmtime_r(&ts, &ti_s);
        FsSnprintf(pos, "<capturetime comment=\"抓拍时间,0时区的时间,如2017-06-19/18:00:00.123456\">%04d-%02d-%02d/%02d:%02d:%02d.%06d</capturetime>\r\n",
                1900 + ti_s.tm_year, ti_s.tm_mon + 1, ti_s.tm_mday, ti_s.tm_hour, ti_s.tm_min, ti_s.tm_sec, (int) (1000000 * (pInc->_ppFrame[i]->capture_gmtTime - ts)));
        FsSnprintf(pos, "<position_jwd comment=\"位置信息,用经纬度表示,如10,10\">%s</position_jwd>\r\n", pRecord_item->ro._position_jwd);
        FsSnprintf(pos, "<img_width comment=\"图像宽度\">%u</img_width>\r\n", pInc->_ppFrame[i]->width[0]);
        FsSnprintf(pos, "<img_height comment=\"图像高度\">%u</img_height>\r\n", pInc->_ppFrame[i]->height[0]);
        ts = (time_t) starttime;
        localtime_r(&ts, &ti_s);
        if (endtime > 0.0) ts = (time_t) endtime;
        localtime_r(&ts, &ti_e);
        FsSnprintf(pos, "<$$data comment=\"图像数据,对xml和json均使用base64编码,编码数据不会填充=,需要使用=填充的解码器请先填充后解码\">"
                "inc%04d%02d%02d%02d%02d%02d-%04d%02d%02d%02d%02d%02d-%02hhx%02hhu%d.jpg</$$data>\r\n",
                1900 + ti_s.tm_year, ti_s.tm_mon + 1, ti_s.tm_mday, ti_s.tm_hour, ti_s.tm_min, ti_s.tm_sec,
                1900 + ti_e.tm_year, ti_e.tm_mon + 1, ti_e.tm_mday, ti_e.tm_hour, ti_e.tm_min, ti_e.tm_sec,
                pInc->_type, pRecord_item->p.recordIndex, i);
        FsSnprintf(pos, "</pic%d>\r\n", i + 1);
        /* 写图片 */
        fileNamePos = pos;
        FsSnprintf(fileNamePos, "%s%s/incident/%04d%02d%02d/%02d/inc%04d%02d%02d%02d%02d%02d-%04d%02d%02d%02d%02d%02d-%02hhx%02hhu%d.jpg", pRecord_item->ro._pRecord->ro._dir,
                pRecord_item->ro._uuid, 1900 + ti_s.tm_year, ti_s.tm_mon + 1, ti_s.tm_mday, ti_s.tm_hour,
                1900 + ti_s.tm_year, ti_s.tm_mon + 1, ti_s.tm_mday, ti_s.tm_hour, ti_s.tm_min, ti_s.tm_sec,
                1900 + ti_e.tm_year, ti_e.tm_mon + 1, ti_e.tm_mday, ti_e.tm_hour, ti_e.tm_min, ti_e.tm_sec,
                pInc->_type, pRecord_item->p.recordIndex, i);
        {
            FsObjectImageJpeg * const pJpeg = (FsObjectImageJpeg *) image_frame_get_pthreadSafety__IO(pInc->_ppFrame[i], ImageFrame_Jpeg_0);
            if (NULL == pJpeg) {
                FsPrintf(FsPrintfIndex, "Prapare jpeg for file:\"%s\" failed,dataLen=%d.\n", FsLocal_ShareBuffer + pos, pos);
                fflush(stdout);
                /* 释放共享buffer */ FsLocal_ShareBuffer_release(pShareBuffer, 0);
                return NULL;
            }
            fs_dir_make(FsLocal_ShareBuffer + pos, 0777);
            if (fs_String_buffer_save_to_file(pJpeg->datalenth, pJpeg->data, FsLocal_ShareBuffer + pos) != 1) {
                fs_Object_delete_pthreadSafety__OI(pJpeg);
                FsPrintf(FsPrintfIndex, "Save data to the file:\"%s\" failed,dataLen=%u.\n", FsLocal_ShareBuffer + pos, pJpeg->datalenth);
                fflush(stdout);
                /* 释放共享buffer */ FsLocal_ShareBuffer_release(pShareBuffer, 0);
                return NULL;
            }
            fs_Object_delete_pthreadSafety__OI(pJpeg);
        }
    }
    /* 准备文件名 */
    fileNamePos = pos;
    ts = (time_t) starttime;
    localtime_r(&ts, &ti_s);
    if (endtime > 0.0) ts = (time_t) endtime;
    localtime_r(&ts, &ti_e);
    FsSnprintf(fileNamePos, "%s%s/incident/%04d%02d%02d/%02d/inc%04d%02d%02d%02d%02d%02d-%04d%02d%02d%02d%02d%02d-%02hhx%02hhu.ui", pRecord_item->ro._pRecord->ro._dir,
            pRecord_item->ro._uuid, 1900 + ti_s.tm_year, ti_s.tm_mon + 1, ti_s.tm_mday, ti_s.tm_hour,
            1900 + ti_s.tm_year, ti_s.tm_mon + 1, ti_s.tm_mday, ti_s.tm_hour, ti_s.tm_min, ti_s.tm_sec,
            1900 + ti_e.tm_year, ti_e.tm_mon + 1, ti_e.tm_mday, ti_e.tm_hour, ti_e.tm_min, ti_e.tm_sec,
            pInc->_type, pRecord_item->p.recordIndex);
    fs_dir_make(FsLocal_ShareBuffer + pos, 0777);
    /* 释放共享buffer */ FsLocal_ShareBuffer_release(pShareBuffer, 1);
    if (fs_String_buffer_save_to_file(pos - FsLocal_ShareBufferPos, FsLocal_ShareBuffer + FsLocal_ShareBufferPos, FsLocal_ShareBuffer + pos) != 1) {
        FsPrintf(FsPrintfIndex, "Save data to the file:\"%s\" failed,dataLen=%u.\n", FsLocal_ShareBuffer + pos, pos - FsLocal_ShareBufferPos);
        fflush(stdout);
        return NULL;
    } else return FsLocal_ShareBuffer + pos;
}
///////////////////////////定义公共函数结束//////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
