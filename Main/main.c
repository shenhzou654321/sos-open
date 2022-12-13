/* 
 * File:   main.c
 * Author: fslib
 *
 * Created on 2013年3月7日, 下午4:00
 */
#include "main.h"
#include <dlfcn.h>
#include <fcntl.h>
#include <malloc.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/sysinfo.h>
#include <unistd.h>
#include "../PublicTools/publicDefine.h"
#include "../PublicTools/Fs/Digest.h"
#include "../PublicTools/Fs/FileTool.h"
#include "../PublicTools/Fs/Log.h"
#include "../PublicTools/Fs/Memery.h"
#include "../PublicTools/Fs/PlayBackFun.h"

#include "../PublicTools/Fs/Server.h"
#include "../PublicTools/Fs/SystemInfo.h"
#include "../PublicTools/Thread/BindClassifyServer.h"


#include "../PublicTools/Thread/ConfigManager.h"

#include "../PublicTools/Thread/error.h"
#include "../PublicTools/Thread/FileStream.h"
#include "../PublicTools/Thread/GB28181.h"

#include "../PublicTools/Thread/monitor.h"

#include "../PublicTools/Thread/Rtmp.h"

#include "../PublicTools/Thread/Rtsp.h"

#include "../Modules/camera_ctrl/camera_ctrl_ys_http.h"
#include "../Modules/camera_infIray/Inflray.h"

#include "../Modules/dhsdk/dhsdk.h"
#include "../Modules/gbsdk/gbsdk.h"

#include "../Modules/sdkPlayBack/sdkPlayBack.h"

#include "../Modules/track/v_tool.h"


#include "vsys/vsys.h"

////////////////////////////////////////////////////////////////////////////////
///////////////////////////定义外部变量与函数开始/////////////////////////////////
#ifdef FsPrintfIndex
#undef FsPrintfIndex
#endif 
#define FsPrintfIndex 1
///////////////////////////定义外部变量与函数结束/////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
///////////////////////////定义全局变量开始//////////////////////////////////////
///////////////////////////定义全局变量结束//////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
///////////////////////////定义私有结构开始//////////////////////////////////////
/* 本模块使用独立日志级别 */
#if 0
static unsigned int moduleLoglevel = FsNumberMax(loglevel); // 本模块使用自定义的loglevel,最大值表示未加到日志级别管理模块中
#define loglevel moduleLoglevel
#endif
///////////////////////////定义私有结构结束//////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
///////////////////////////定义私有函数开始//////////////////////////////////////


static void main_cb_threadMulti(/* 倍频因子修改标识,0-减小,1-增加 */char add, /* 当前线程数 */unsigned int threadCount, RootStruct * const pRootStruct
        , /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer) {
    //FsLog(FsLogType_error, FsPrintfIndex, "add=%hhd,threadCount=%u,pRootStruct->p._threadMulti=%d\n", add, threadCount, pRootStruct->p._threadMulti);
    if (1 == add) {
        if (pRootStruct->p._threadMulti * 16 < threadCount) {
            pRootStruct->p._threadMulti *= 2;
            if (fs_server_histroy_runInfo_update_threadMulti(pRootStruct->p._threadMulti, pShareBuffer) != 1) {
                FsLog(FsLogType_error, FsPrintfIndex, "Save threadMulti failed,%u,%u.\n", pRootStruct->p._threadMulti, threadCount);
                fflush(stdout);
            }
            pRootStruct->rw.runStatus = 2;
        }
    } else if (pRootStruct->p._threadMulti > 1) {
        pRootStruct->p._threadMulti /= 2;
        if (fs_server_histroy_runInfo_update_threadMulti(pRootStruct->p._threadMulti, pShareBuffer) != 1) {
            FsLog(FsLogType_error, FsPrintfIndex, "Save threadMulti failed,%u,%u.\n", pRootStruct->p._threadMulti, threadCount);
            fflush(stdout);
        }
        pRootStruct->rw.runStatus = 2;
    }

}
///////////////////////////定义私有函数结束//////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
///////////////////////////定义公共函数开始//////////////////////////////////////

/*
 * 创建一个RootStruct实例;
 * 返回创建的实例指针. 
 */
RootStruct* rootStruct_new__IO() {
    RootStruct* rst = (RootStruct*) fsMalloc(sizeof (RootStruct));
    memset(rst, 0, sizeof (RootStruct));

    /* 多线程共有的运行状态,如果此值为空,则引用默认的全局运行状态变量,本线程的运行状态只有与此值相同才能运行 */
    rst->ro._externRunStatus = &defaultRunStatus;
    /* 线程倍频,最小值为1 */
    rst->p._threadMulti = fs_server_histroy_runInfo_get_threadMulti(1, NULL);

    return rst;
}

/* 删除pRootStruct指向的实例对象  */
void rootStruct_delete__OI(RootStruct* pRootStruct) {
    fsFree(pRootStruct);
}
#include "../PublicTools/Image/ImageJpeg.h"
#include "../PublicTools/Media/VideoH264.h"

void bayer2RGB(/* 宽度 */unsigned int width, /* 高度 */unsigned int height,
        /* RGB图的储存宽度 */unsigned int dstLineBits, /* 储存结果的数据指针 */unsigned char dstBuffer[],
        /* 原图的储存宽度 */unsigned int srcLineBits, /* 原图的数据指针 */unsigned char bayer[]) {

}

void Bayer2RGB_P(unsigned char* pBayer, unsigned char* pRGB, int nWidth, int nHeight) {
    // GRGRGRGR
    // BGBGBGBG
    // GRGRGRGR
    // BGBGBGBG
    // 第一行和最后一行不处理

    unsigned char* ps1 = (unsigned char*) (pBayer);
    unsigned char* ps2 = (unsigned char*) (pBayer + nWidth);
    unsigned char* ps3 = (unsigned char*) (pBayer + nWidth * 2);
    unsigned char* ps4 = (unsigned char*) (pBayer + nWidth * 3);

    unsigned char* pr1 = (unsigned char*) (pRGB + nWidth);
    unsigned char* pr2 = (unsigned char*) (pRGB + 2 * nWidth);
    unsigned char* pg1 = (unsigned char*) (pRGB + nWidth + nWidth * nHeight);
    unsigned char* pg2 = (unsigned char*) (pRGB + 2 * nWidth + nWidth * nHeight);
    unsigned char* pb1 = (unsigned char*) (pRGB + nWidth + nWidth * nHeight * 2);
    unsigned char* pb2 = (unsigned char*) (pRGB + 2 * nWidth + nWidth * nHeight * 2);

    unsigned long long a1, a2, a3, a4;
    unsigned long long r1, r2, r3, r4;
    unsigned long long g1, g2, g3, g4, g22, g33;
    unsigned long long b1, b2, b3, b4;

    unsigned long long m1 = 0x00ff00ff00ff00ff;
    unsigned long long m2 = 0xff00ff00ff00ff00;
    int j, i;
    for (j = 1; j < nHeight - 2; j += 2) {
        for (i = 0; i < nWidth; i += 6) {
            a1 = *(unsigned long long *) (ps1 + i);
            a2 = *(unsigned long long *) (ps2 + i);
            a3 = *(unsigned long long *) (ps3 + i);
            a4 = *(unsigned long long*) (ps4 + i);

            g1 = a1 & m1;
            g2 = a2 & m2;
            g3 = a3 & m1;
            g4 = a4 & m2;

            //			g22 = g2 >> 8;
            //			g22 = (((g22 + (g22 >> 16)) >> 1) & m1) << 16;
            //			g22 = (g22 + (((g1+g3)>>1) & m1) >> 1) & m1;
            //			g33 = g3;
            //			g33 = ((g33 + (g33 >> 16)) >> 1) & m1;
            //			g33 = ((g33 + ((((g2>>8)+(g4>>8))>>1) & m1))>> 1) & m1;
            //			*(unsigned long*)(pg1+i+1) = (g2 | g22) >> 8;
            //			*(unsigned long*)(pg2+i+1) = (g3 | (g33<<8)) >> 8;

            *(unsigned long long*) (pg1 + i + 1) = (g2 | (((g1 + g3) >> 1) & m1)) >> 8;
            *(unsigned long long*) (pg2 + i + 1) = (g3 | (((g2 + g4) >> 1) & m2)) >> 8;

            b2 = a2 & m1;
            b4 = a4 & m1;
            b3 = ((b2 + b4) >> 1) & m1;
            b2 = b2 | (((b2 + (b2 >> 16)) << 7) & m2);
            b3 = b3 | (((b3 + (b3 >> 16)) << 7) & m2);
            *(unsigned long long*) (pb1 + i + 1) = b2 >> 8;
            *(unsigned long long*) (pb2 + i + 1) = b3 >> 8;

            r1 = (a1 >> 8) & m1;
            r3 = (a3 >> 8) & m1;
            r2 = ((r1 + r3) >> 1) & m1;
            r2 = r2 | (((r2 + (r2 >> 16)) << 7) & m2);
            r3 = r3 | (((r3 + (r3 >> 16)) << 7) & m2);
            *(unsigned long long*) (pr1 + i + 1) = r2 >> 8;
            *(unsigned long long*) (pr2 + i + 1) = r3 >> 8;
        }

        ps1 += nWidth * 2;
        ps2 += nWidth * 2;
        ps3 += nWidth * 2;
        ps4 += nWidth * 2;

        pr1 += nWidth * 2;
        pr2 += nWidth * 2;
        pg1 += nWidth * 2;
        pg2 += nWidth * 2;
        pb1 += nWidth * 2;
        pb2 += nWidth * 2;
    }
}

void ttt(char fileName[]) {
    ImageDib *pDib1 = image_dib_new_from_file__IO(fileName, ImageColorType_Gray, 0);
    {
        unsigned char *ps = pDib1->data;
        int i = pDib1->width * pDib1->height;
        //        for (; i > 0; i -= 8, ps += 8) {
        //            ps[0] ^= ps[7];
        //            ps[7] ^= ps[0];
        //            ps[0] ^= ps[7];
        //
        //            ps[1] ^= ps[6];
        //            ps[6] ^= ps[1];
        //            ps[1] ^= ps[6];
        //
        //            ps[2] ^= ps[5];
        //            ps[5] ^= ps[2];
        //            ps[2] ^= ps[5];
        //
        //            ps[3] ^= ps[4];
        //            ps[4] ^= ps[3];
        //            ps[3] ^= ps[4];
        //        }
    }
    image_dib_svae_to_file(pDib1, "D:/a_1.bmp");
    ImageDib *pDib2 = image_dib_new__IO(pDib1->width, pDib1->height, ImageColorType_RGB, 0);
    ImageDib *pDib3 = image_dib_new__IO(pDib1->width, pDib1->height, ImageColorType_RGB, 0);
    //bayer2RGB(pDib2->width,pDib2->height,pDib2->lineBits,pDib2->data,pDib1->lineBits,pDib1->data);
    Bayer2RGB_P(pDib1->data + pDib1->width + 1, pDib2->data, pDib1->width, pDib2->height - 2);
    image_dib_delete__OI(&pDib1);
    // image_dib_svae_to_file(pDib2, "_1.bmp");
    int i = pDib2->dataLenth / 3;
    unsigned char *r = pDib2->data;
    unsigned char *g = r + i;
    unsigned char *b = g + i;
    unsigned char *pd = pDib3->data;
    while (i-- > 0) {
        *pd++ = *r++;
        *pd++ = *g++;
        *pd++ = *b++;
    }
    image_dib_delete__OI(&pDib2);
    char *buf = (char*) fsMalloc(strlen(fileName) + 5);
    sprintf(buf, "%s.bmp", fileName);
    image_dib_svae_to_file(pDib3, buf);
    fsFree(buf);
    image_dib_delete__OI(&pDib3);
}
#include "../PublicTools/Fs/Shell.h"

#include "../PublicTools/Fs/Time.h"
#define FIBER_STACK 8192
int a;

int do_something(void *aa) {
    printf("This is son, the pid is:%d, the a is: %d\n", getpid(), ++a);
    //fsFree(stack); //这里我也不清楚，如果这里不释放，不知道子线程死亡后，该内存是否会释放，知情者可以告诉下,谢谢
    exit(0);
}
#include "../PublicTools/Fs/FileTool.h"
#include "../PublicTools/Fs/Json.h"
#include "../PublicTools/Fs/Socket.h"
#include "../PublicTools/Thread/FileStream.h"
#include "../PublicTools/Thread/ConfigManager.h"
#include "../PublicTools/Fs/Memery.h"
#include "../PublicTools/Fs/Bd.h"

#include "../PublicTools/Media/AudioAAC.h"

#include "../PublicTools/Media/VideoMKV.h"
#include "../PublicTools/Fs/TypeFace.h"
#include "../Modules/camera_ctrl/camera_ctrl_hk_http.h"
#include "../PublicTools/Thread/Hls.h"

#include "../PublicTools/Thread/Onvif.h"
#include "../PublicTools/Thread/Rtmp.h"

#include "../PublicTools/Fs/Dns.h"
#include "../PublicTools/Fs/Calculation.h"
#include <sys/time.h>
#include "../PublicTools/Thread/GB28181.h"
#include <iconv.h>
#include <errno.h>

//#include <libiptc/libiptc.h>
//#include <iptables.h>
//#include <linux/netfilter/nf_nat.h>
//
//int x_append_rule(struct xtc_handle * t_handle, char *chain, struct ipt_entry *e) {
//    if (iptc_append_entry(chain, e, t_handle) != 1) {
//        printf("Error x_append_rule: %s\n", iptc_strerror(errno));
//        return 0;
//
//
//    }
//    iptc_commit(t_handle);
//    return 1;
//}
//
//struct ipt_entry *api_iptc_entry_get(struct sockaddr_in src,
//        struct sockaddr_in dst, struct sockaddr_in nto, const char *option) {
//    struct ipt_entry *fw = NULL;
//
//    struct ipt_entry_match *match = NULL;
//    struct ipt_tcp *tcpinfo = NULL;
//
//    struct ipt_entry_target *target = NULL;
//    struct nf_nat_ipv4_multi_range_compat *mr = NULL;
//
//    unsigned int size1 = XT_ALIGN(sizeof (struct ipt_entry));
//    unsigned int size2 = XT_ALIGN(sizeof (struct ipt_entry_match) + sizeof (struct ipt_tcp));
//    unsigned int size3 = XT_ALIGN(sizeof (struct ipt_entry_target) + sizeof (struct nf_nat_ipv4_multi_range_compat));
//
//    if (!option) {
//        printf("NULL\n");
//        return NULL;
//    }
//
//    fw = calloc(1, size1 + size2 + size3);
//    if (!fw) {
//        printf("Malloc failure");
//        return NULL;
//    }
//
//    /* Offsets to the other bits */
//    fw->target_offset = size1 + size2;
//    fw->next_offset = size1 + size2 + size3;
//
//    /* Set up packet matching rules */
//    if ((fw->ip.src.s_addr = src.sin_addr.s_addr) == INADDR_ANY) {
//        fw->ip.smsk.s_addr = 0;
//    } else {
//        fw->ip.smsk.s_addr = inet_addr("255.255.255.255");
//    }
//
//    if ((fw->ip.dst.s_addr = dst.sin_addr.s_addr) == INADDR_ANY) {
//        fw->ip.dmsk.s_addr = 0;
//    } else {
//        fw->ip.dmsk.s_addr = inet_addr("255.255.255.255");
//    }
//
//    fw->ip.proto = IPPROTO_TCP;
//    fw->nfcache = NFC_UNKNOWN; /*Think this stops caching. */
//
//
//    /* TCP specific matching(ie. ports) */
//    match = (struct ipt_entry_match *) fw->elems;
//    match->u.match_size = size2;
//    strcpy(match->u.user.name, "tcp");
//
//
//    tcpinfo = (struct ipt_tcp *) match->data;
//
//
//    if (src.sin_port == 0) {
//        tcpinfo->spts[0] = ntohs(0);
//        tcpinfo->spts[1] = ntohs(0xFFFF);
//    } else {
//        tcpinfo->spts[0] = tcpinfo->spts[1] = ntohs(src.sin_port);
//    }
//
//
//    if (dst.sin_port == 0) {
//        tcpinfo->dpts[0] = ntohs(0);
//        tcpinfo->dpts[1] = ntohs(0xFFFF);
//    } else {
//        tcpinfo->dpts[0] = tcpinfo->dpts[1] = ntohs(dst.sin_port);
//    }
//
//
//    /* And the target */
//    target = (struct ipt_entry_target *) (fw->elems + size2);
//    target->u.target_size = size3;
//    strcpy(target->u.user.name, option);
//
//
//    mr = (struct nf_nat_ipv4_multi_range_compat *) target->data;
//    mr->rangesize = 1;
//
//    mr->range[0].flags = NF_NAT_RANGE_PROTO_SPECIFIED | NF_NAT_RANGE_MAP_IPS;
//    mr->range[0].min.tcp.port = mr->range[0].max.tcp.port = nto.sin_port;
//    mr->range[0].min_ip = mr->range[0].max_ip = nto.sin_addr.s_addr;
//
//    return fw;
//}
//
//int api_iptc_entry_add(const struct ipt_entry *fw, const char *chain) {
//    int ret = -1;
//    struct xtc_handle *phandle = NULL;
//
//    if (!fw || !chain) {
//        FsPrintf(1, "NULL\n");
//        return -1;
//    }
//
//    if ((phandle = iptc_init("nat")) &&
//            iptc_append_entry(chain, fw, phandle) &&
//            iptc_commit(phandle)) {
//
//        ret = 0;
//    } else {
//
//        FsPrintf(1, "%s\n", iptc_strerror(errno));
//        iptc_append_entry(chain, fw, phandle);
//        iptc_commit(phandle);
//        ret = -1;
//    }
//    return ret;
//}
//
//int test_snat(int off, unsigned int times) {
//    int ret = -1;
//    int ix = 0;
//
//    struct ipt_entry *fw = NULL;
//
//    struct sockaddr_in src;
//    struct sockaddr_in dst;
//    struct sockaddr_in sto;
//
//    src.sin_addr.s_addr = inet_addr("1.1.1.1") + off;
//    dst.sin_addr.s_addr = INADDR_ANY;
//    sto.sin_addr.s_addr = inet_addr("1.1.1.3") + off;
//
//    for (ix = 0; ix < times; ix++) {
//        src.sin_port = htons(1 + ix);
//        dst.sin_port = htons(2 + ix);
//        sto.sin_port = htons(3 + ix);
//
//        fw = api_iptc_entry_get(src, dst, sto, "MASQUERADE");
//        api_iptc_entry_add(fw, "POSTROUTING");
//    }
//
//    fsFree(fw);
//    ret = 0;
//_E1:
//
//    return ret;
//}
//
//struct ipt_natinfo {
//    struct xt_entry_target t;
//    struct nf_nat_ipv4_multi_range_compat mr;
//};
//
//static int insert_forward_rule(const char *table, const char *chain, unsigned int src, unsigned int dest, const char * ports, const char *target1, char * proto) {
//    struct xtc_handle *h;
//    struct ipt_entry *en = NULL;
//    struct ipt_entry * e;
//
//    struct ipt_entry_match * match_proto, * match_limit;
//    struct ipt_entry_target * target;
//    unsigned int size_ipt_entry, size_ipt_entry_match, size_ipt_entry_target, total_length;
//
//    size_ipt_entry = XT_ALIGN(sizeof (struct ipt_entry));
//    size_ipt_entry_match = XT_ALIGN(sizeof (struct ipt_entry_match)) + sizeof (struct ipt_tcp) + sizeof (int);
//    size_ipt_entry_target = XT_ALIGN(sizeof (struct ipt_entry_target) + sizeof (IPTC_LABEL_ACCEPT));
//    total_length = 216; //size_ipt_entry + size_ipt_entry_match + size_ipt_entry_target + IPT_ALIGN(sizeof (struct ipt_natinfo)); //size_ipt_entry + 48 + 40
//    //printf("size of ipt ebtry=%u,match=%u,target=%u,total=%u\n", size_ipt_entry, size_ipt_entry_match, size_ipt_entry_target, total_length);
//    //memory allocation for all structs that represent the netfilter rule we want to insert
//
//    e = calloc(1, total_length + 2560);
//    if (e == NULL) {
//        printf("malloc failure");
//        exit(1);
//    }
//    int size = 160;
//    //offsets to the other bits:
//    //target struct begining
//    e->target_offset = size; //size_ipt_entry + size_ipt_entry_match; //+ size_ipt_tcp + size_rateinfo + size_physdevinfo;
//    //next "e" struct, end of the current one
//    e->next_offset = total_length;
//    e->nfcache = NFC_IP_DST_PT;
//
//    if (dest) {
//        e->ip.dst.s_addr = dest;
//        e->ip.dmsk.s_addr = 0xFFFFFFFF;
//
//    }
//    match_limit = (struct ipt_entry_match *) (e->elems); //+ match_proto->u.match_size
//    match_limit->u.user.match_size = size_ipt_entry_match; //size_ipt_entry_match*3; //+ size_rateinfo;
//    strncpy(match_limit->u.user.name, "tcp", sizeof (match_limit->u.user.name)); //set name of the module, we will use in this match
//    struct ipt_tcp *info = (struct ipt_tcp *) match_limit->data;
//    unsigned int i = 0;
//    if (strchr(ports, ':') != NULL) {
//        char aPort[2][10];
//        sscanf(ports, "%[^:], %[^:]", aPort[0], aPort[1]);
//
//        info->dpts[0] = atoi(aPort[0]);
//        info->dpts[1] = atoi(aPort[1]);
//    } else {
//        info->dpts[0] = info->dpts[1] = atoi(ports);
//
//    }
//    info->spts[0] = 0;
//    info->spts[1] = 0xFFFF;
//    e->ip.proto = IPPROTO_TCP;
//    printf("Target offset=%d,next offset=%d\n", e->target_offset, e->next_offset);
//    target = (struct ipt_entry_target *) ((void*) e + e->target_offset); // + size_ipt_entry_match); //+ size_ipt_tcp + size_rateinfo + size_physdevinfo
//    size = 56;
//    target->u.user.target_size = size_ipt_entry_target + XT_ALIGN(sizeof (struct nf_nat_ipv4_range)); // size_ipt_entry_target;
//    target->u.user.revision = 0;
//    strncpy(target->u.user.name, target1, sizeof (target->u.user.name));
//    struct ipt_natinfo *inf = (struct ipt_natinfo*) target->data;
//    struct nf_nat_ipv4_range range;
//    memset(&range, 0, sizeof (range));
//    range.flags |= NF_NAT_RANGE_MAP_IPS;
//    range.min_ip = range.max_ip = src;
//    inf->mr.rangesize++;
//    inf->mr.range[inf->mr.rangesize] = range;
//    inf->t.u.target_size = XT_ALIGN(sizeof (*inf) + inf->mr.rangesize * sizeof (struct nf_nat_ipv4_range));
//
//
//    printf("after s_addr\n\n");
//
//
//    //All the functions, mentioned below could be found in "Querying libiptc HOWTO" manual
//    h = iptc_init(table);
//    printf("h=%d\n", h);
//    if (!h) {
//        printf("Error initializing: %s\n", iptc_strerror(errno));
//        return 0;
//    }
//
//    printf("target_size=%d,match=%d,chain=%s,table=%s,target=%s\n", target->u.user.target_size, match_limit->u.user.match_size, chain, table, target1);
//
//    int x = iptc_append_entry(chain, e, h);
//    if (!x) {
//        printf("Error append_entry: %s\n", iptc_strerror(errno));
//        goto ERROR;
//    }
//    printf("%d,chain=%s,target=%s\n", target->u.user.target_size, chain, table);
//    int y = iptc_commit(h);
//    if (!y) {
//        printf("Error no=%d,commit: %s\n", errno, iptc_strerror(errno));
//        goto ERROR;
//    }
//    iptc_fsFree(h);
//    if (e != NULL) {
//        fsFree(e);
//        e = NULL;
//    }
//    return 1;
//ERROR:
//    iptc_fsFree(h);
//    if (e != NULL) {
//
//        fsFree(e);
//        e = NULL;
//    }
//    return 0;
//}
#include <xmmintrin.h>


int __a(int b) {
    return b + 1;
}
void v_tt(void *const pObject);
#include <sys/statfs.h>

int main(int argc, char** argv) {

#undef FsFunctionName
#define FsFunctionName main
#ifdef FsDebug  
    if (0) {
        ImageDib *pdib = image_dib_new__IO(1280, 1024, ImageColorType_Gray, 1280);
        FsString *pString = fs_String_new_form_file__IO("/aa/tt1.xx");
        memcpy(pdib->data, pString->buffer, pdib->lineBits * pdib->height);
        image_dib_svae_to_file(pdib, "/aa/tt1.bmp");
        return 0;
    }
    if (0) {

        struct {
            int a : 30;
            int b : 2;
        } t;
        t.a = 12;
        t.b = 1;
        t.a++;
        t.b++;
        int a1 = t.a;
        int b1 = t.b;
        printf("%d,%d\n", a1, b1);
        return 0;
    }
    if (0) {
        FsObjectBase *rst = NULL;
        rst = fs_ObjectBase_new__IO(&rst, 100, sizeof (FsObjectBase));
        v_tt(rst);
        rst->_delete(rst);
        return 0;
    }
    if (0) {
        FILE *fd = fopen("/aat", "ro");
        while (1) {
            sleep(1);
            if (fseek(fd, 1000, SEEK_SET) != 0) {
                FsPrintf(FsPrintfIndex, "fseek error:\"%s\".\n", strerror(errno));
                fflush(stdout);
                continue;
            }
            sleep(1);
            char buffer[20];
            FsPrintf(FsPrintfIndex, "fread len=%ld \"%s\".\n", fread(buffer, 1, 20, fd), strerror(errno));
        }
        fs_dir_out("/", stdout, NULL);
        return 0;
    }
    if (0) {
        globalInit(NULL, NULL, NULL, 0);
        if (1) {
            exceptionRegister_pthreadSafety(NULL, NULL, ExceptionErrorFileName2_Default, (signed char*) &defaultRunStatus, NULL);
            exception_skip(SIGCLD);
            exception_skip(SIGPIPE);
        }
        dhsdk_test(NULL);
        /* 释放异常捕捉内部变量的空间 */
        exceptionDelete();
        globalRelease();
        return 0;
    }
    if (0) {
        FsPrintf(1, "ttttt,%llu\n", fs_cpu_frequency_get());
        return 0;
    }
    if (0) {
        fs_system_test(NULL);
        return 0;
    }
    if (0) {
        int count = 3;
        int i = 0;
        for (i = 0; i < count; i++) {
            int v = (255 / count)*(i + 1);
            printf("%d\n", v);
        }
        return 0;
    }
    if (0) {
        char rst_24[32];
        int t = fs_time_to_string(rst_24, 2147483647.9999998807908);
        rst_24[t] = 0;
        FsPrintf(1, "%d,%s\n", t, rst_24);
        return 0;
    }
    if (0) {
        int aa = 10, bb, cc;
        __asm__ __volatile__(
                "movl %2,%%eax;"
                "movl %%eax,%0;"
                //"movl 30,%%ebx;"
                "addl %%eax,%%ebx;"
                "addl %%eax,%%ebx;"
                "movl %%ebx,%1;"
                : "=r"(bb), "=r"(cc) /* 输出 */
                : "r"(aa) /* 输入 */
                : "%eax", "%ebx" /* 被改变了的寄存器 */
                );
        FsPrintf(1, "bb=%d,cc=%d\n", bb, cc);
        float a[] __attribute__ ((aligned(16))) = {1.0f, 2.0f, 3.0f, 4.0f};
        float b[] __attribute__ ((aligned(16))) = {5.0f, 6.0f, 7.0f, 8.0f};
        const int count = 1000000000 / 100;

        float c[4] __attribute__ ((aligned(16))) = {0, 0, 0, 0};
        double t1 = fs_time_uptime_get();
        int i, j;
        for (i = 0; i < count; i++)
            for (j = 0; j < 4; j++)
                c[j] = a[j] + b[j];
        FsPrintf(1, "normal,time:%lf,%f,%f,%f,%f\n", fs_time_uptime_get() - t1, c[0], c[1], c[2], c[3]);
        c[0] = 0, c[1] = 0, c[2] = 0, c[3] = 0;
        t1 = fs_time_uptime_get();
        //        __asm__ __volatile__(
        //                "movaps (%2),%%xmm1;\n\t"
        //                : "=r"(bb), "=r"(cc) /* 输出 */
        //                : "r"(a) /* 输入 */
        //                : "%eax", "%ebx" /* 被改变了的寄存器 */
        //                );
        for (i = 0; i < count; i++) {
            __asm__ __volatile__(
                    "movaps (%1),%%xmm0;"
                    "movaps (%2),%%xmm1;"
                    "addps %%xmm1,%%xmm0;"
                    "movaps %%xmm0,%0;"
                    : "=m"(c) /* 输出 */
                    : "r"(a), "r"(b) /* 输入 */
                    : "%xmm0", "%xmm1" /* 被改变了的寄存器 */
                    );
        }
        FsPrintf(1, "sse,time:%lf,%f,%f,%f,%f\n", fs_time_uptime_get() - t1, c[0], c[1], c[2], c[3]);
        c[0] = 0, c[1] = 0, c[2] = 0, c[3] = 0;
        t1 = fs_time_uptime_get();
        __m128 a1, b2;
        __m128 c1;
        for (i = 0; i < count; i++) {
            a1 = _mm_load_ps(a);
            b2 = _mm_load_ps(b);
            c1 = _mm_add_ps(a1, b2);
        }
        FsPrintf(1, "Intrinsic,time:%lf,%f,%f,%f,%f\n", fs_time_uptime_get() - t1, c[0], c[1], c[2], c[3]);
        c[0] = 0, c[1] = 0, c[2] = 0, c[3] = 0;
        t1 = fs_time_uptime_get();
        return 0;
    }

    if (0) {
        int __b = 3;
        int __c = __a(__b);
        int __d = __c + 1;
        image_dib_test();
        return 0;
    }
    //    if (0) {
    //        const char *path = "/usr/lib64/xtables/libipt_MASQUERADE.so";
    //        if (dlopen(path, RTLD_NOW) == NULL) {
    //            fprintf(stderr, "%s: %s\n", path, dlerror());
    //        }
    //        return 0;
    //    }
    //    if (0) {
    //        struct xtc_handle * t_handle = iptc_init("nat");
    //
    //
    //        if (t_handle == NULL) {
    //            printf("Error initializing: %s\n", iptc_strerror(errno));
    //            return errno;
    //        }
    //
    //        const char *chain;
    //        //del nat table all rule
    //        for (chain = iptc_first_chain(t_handle); chain; chain = iptc_next_chain(t_handle)) {
    //            iptc_flush_entries(chain, t_handle);
    //            printf("Clean %s rule \n", chain);
    //        }//for 
    //        iptc_commit(t_handle);
    //        iptc_fsFree(t_handle);
    //        //return 1;
    //        t_handle = iptc_init("nat");
    //
    //        {
    //            char _t[] = {
    //                0x38, 0x00, 0x4D, 0x41, 0x53, 0x51, 0x55, 0x45, 0x52, 0x41, 0x44, 0x45, 0x00, 0x00, 0x00, 0x00,
    //                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    //                0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    //                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    //            };
    //            int size = sizeof (struct ipt_entry);
    //            int _target_size = sizeof (_t);
    //            struct ipt_entry *e = fsMalloc(size + _target_size);
    //            memset(e, 0, size);
    //            e->ip.src.s_addr = Fs_Network_Ipv4(192, 168, 40, 0);
    //            e->ip.smsk.s_addr = Fs_Network_Ipv4(255, 255, 255, 0);
    //            e->ip.dst.s_addr = Fs_Network_Ipv4(0, 0, 0, 0);
    //            e->ip.dmsk.s_addr = Fs_Network_Ipv4(0, 0, 0, 0);
    //            e->target_offset = size;
    //            e->next_offset = size + _target_size;
    //            size = 0;
    //            memcpy(e->elems + size, _t, _target_size);
    //            {
    //                int _i = 0;
    //                int _size = sizeof (struct ipt_entry)+_target_size;
    //                printf("_size=%d,_target_size=%d\n", _size, _target_size);
    //                for (_i = 0; _i < _size; _i++) {
    //                    if (0 == (_i & 0xF)) printf("\n");
    //                    printf("0x%02hhX,", ((char*) e)[_i]);
    //                }
    //                printf("\n");
    //            }
    //
    //            if (iptc_append_entry("POSTROUTING", e, t_handle) != 1) {
    //                printf("Error x_append_rule: %s\n", iptc_strerror(errno));
    //                return 0;
    //            }
    //            if (iptc_commit(t_handle) != 1) {
    //                printf("Error x_append_rule: %s\n", iptc_strerror(errno));
    //                return 0;
    //            }
    //        }
    //        if (0) {
    //            struct ipt_entry_target *a_t;
    //            struct ipt_entry_match *a_m;
    //            struct ipt_entry *a_e;
    //            struct ipt_tcp *ptcp;
    //            struct ipt_ip *pip;
    //            size_t target_size, match_size, entry_size;
    //
    //            //iptables -A PREROUTING -d 0.0.0.0/0 -p tcp --dport 80 -j DNAT --to--destination 172.16.1.1
    //            //init size
    //            match_size = sizeof (struct ipt_entry_match) + sizeof (struct ipt_tcp);
    //            target_size = sizeof (struct ipt_entry_target) + sizeof (int);
    //            entry_size = sizeof (*a_e) + target_size + match_size;
    //
    //            //full struct ipt_entry
    //            a_e = fsMalloc(entry_size);
    //            memset(a_e, 0, entry_size);
    //            a_e->ip.src.s_addr = Fs_Network_Ipv4(192, 168, 4, 2);
    //            a_e->ip.dmsk.s_addr = -1;
    //            a_e->ip.dst.s_addr = 0;
    //            a_e->ip.dmsk.s_addr = 0;
    //            a_e->ip.proto = 6; //tcp
    //            a_e->target_offset = sizeof (*a_e) + match_size;
    //            a_e->next_offset = entry_size;
    //
    //
    //            //            //full struct ip_entry_match
    //            //            a_m = (struct ipt_entry_match*) a_e->elems;
    //            //            a_m->u.user.match_size = match_size;
    //            //            strcpy(a_m->u.user.name, "tcp");
    //            //
    //            //
    //            //            //full struct ipt_tcp
    //            //            ptcp = (struct ipt_tcp*) a_m->data;
    //            //
    //            //            ptcp->spts[0] = -1;
    //            //            ptcp->spts[1] = -1; //any sport
    //            //
    //            //            ptcp->dpts[0] = 80;
    //            //            ptcp->dpts[1] = 80; //one port
    //
    //
    //            //full struct ipt_entry_target
    //            a_t = (struct ipt_entry_target*) (a_e->elems + match_size);
    //            a_t->u.user.target_size = target_size;
    //
    //
    //            strcpy(a_t->u.user.name, "MASQUERADE");
    //
    //
    //            //do_command
    //            x_append_rule(t_handle, "PREROUTING", a_e);
    //        }
    //
    //        //iptc_commit(t_handle);
    //        iptc_fsFree(t_handle);
    //        // test_snat(0, 1);
    //        //        unsigned int a, b;
    //        //        inet_pton(AF_INET, "20.10.10.254", &a);
    //        //        inet_pton(AF_INET, "20.10.10.100", &b);
    //        //        insert_forward_rule("nat", "POSTROUTING", b, a, "1111", "DNAT", "tcp");
    //
    //
    //        return 1;
    //
    //
    //
    //    }
    if (0) {
        fs_socket_raw_send_udp(Fs_Network_Ipv4(192, 168, 99, 10), 22, Fs_Network_Ipv4(192, 168, 60, 33), 66, "1234567890", 10);
        return 0;
    }
    if (0) {
        const unsigned int ipv4 = Fs_Network_Ipv4(192, 168, 60, 23);
        const unsigned short ipv4Port = 266;
        unsigned int ipv4_local = Fs_Network_Ipv4(192, 168, 199, 140);
        unsigned short ipv4Port_local = 722;
        int s = fs_Socket_udpIPv4_connect__IO(ipv4, ipv4Port, &ipv4_local, &ipv4Port_local, 1);
        if (send(s, "1234567890", 10, 0) != 10) {
            FsPrintf(1, "Error:\"%s\"\n", strerror(errno));
        }
        sleep(100);
        closesocket(s);

        return 0;
    }
    if (0) {
        const char *t = "2018-06-25T16:50:59";
        FsPrintf(1, "%s-%lf\n", t, fs_time_get_string(t));
        return 0;
    }
    if (0) {
        fs_Router_test();
        return 0;
    }
    if (0) {
        unsigned int *p;
        unsigned int i;
        p = (&i) - 128;
        for (i = 0; i < 30; i++)p[i] = i + 1;
        for (i = 0; i < 30; i++)printf("p[%u]=%u\n", i, p[i]);
        printf("&i=%p,&p=%p\n", &i, &p);
        return 0;
    }
    if (0) {
        video_h265_test();
        return 0;
    }
    if (0) {
        iconv_t cd = iconv_open("GBK", "UTF-8");
        if (cd > 0) {
            char name[1000] = "gbk后的长度", bb[200];
            size_t srcLenth = strlen(name) + 1;
            size_t dstLenth = srcLenth;
            char *ps = (char*) name;
            char *pd = bb;
            if (iconv(cd, &ps, &srcLenth, &pd, &dstLenth) == 0) {
                *pd = 0;
                FsPrintf(5, "Convert :\"%s\" from UTF-8 to GBK ok,rst:\"%s\".\n", name, bb);
                fflush(stdout);
            } else {
                FsPrintf(5, "Convert :\"%s\" from UTF-8 to GBK failed.\n", name);
                fflush(stdout);
            }
            iconv_close(cd);
        }
        return 0;
    }
    if (0) {
        fs_Ebml_test();
        return 0;
    }
    if (0) {
        double theTime = 124234234234.1223243543534534;
        FsPrintf(1, "tt=%hhd,%lf,%lf.\n", fs_time_timezone_get(), fs_time_get_string("2018-03-26/14:28:00.007"),
                theTime - (((long long) ((theTime - (unsigned long long) theTime) * 10000000)) % 10000)*0.0000001);
        return 0;
    }
    if (0) {
        globalInit(NULL, NULL, NULL, 0);
        if (0) {
            exceptionRegister_pthreadSafety(NULL, NULL, ExceptionErrorFileName2_Default, (signed char*) &defaultRunStatus, NULL);
            exception_skip(SIGCLD);
            exception_skip(SIGPIPE);
        }
        video_h264_test();
        /* 释放异常捕捉内部变量的空间 */
        exceptionDelete();
        globalRelease();
        return 0;
    }
    if (0) {
        fs_Calculation_test();
        return 0;
    }
    if (0) {
        globalInit(NULL, NULL, NULL, 0);
        if (0) {
            exceptionRegister_pthreadSafety(NULL, NULL, ExceptionErrorFileName2_Default, (signed char*) &defaultRunStatus, NULL);
            exception_skip(SIGCLD);
            exception_skip(SIGPIPE);
        }
        Monitor *pMonitor = monitor_new__IO(NULL, 0, "main.monitor", NULL/* "/tmp/running/" */, 0, 5.0, 0.2, 0, NULL, NULL, NULL);
        fs_String_test();
        monitor_delete__OI(pMonitor);
        /* 释放异常捕捉内部变量的空间 */
        exceptionDelete();
        globalRelease();
        return 0;
    }
    if (0) {
        unsigned int tt = 0xFFFF00FFU - 100;
        int i = 0;
        unsigned int *p = &tt;
        for (; i < 100; i++) {
            FsPrintf(1, "[%d]:%x\n", i, tt);
            fflush(stdout);
            *p += 90000 / 25.1;
        }
        return 0;
    }
    if (0) {
        globalInit(NULL, NULL, NULL, 0);
        if (1) {
            exceptionRegister_pthreadSafety(NULL, NULL, ExceptionErrorFileName2_Default, (signed char*) &defaultRunStatus, NULL);
            exception_skip(SIGCLD);
            exception_skip(SIGPIPE);
        }
        char temp[128];
        FsStringBuilder *sb = fs_StringBuilder_new__IO();
        fs_StringBuilder_apend(sb, "<root commet=\"fslib.org\"><process>");
        fs_StringBuilder_apend(sb, "<pid>");
        sprintf(temp, "%d", getpid());
        fs_StringBuilder_apend(sb, temp);
        fs_StringBuilder_apend(sb, "</pid>");
        fs_StringBuilder_apend(sb, "<createTime>");
        fs_StringBuilder_apend(sb, "2017-08-29 11:20:30.950018");
        fs_StringBuilder_apend(sb, "</createTime>");
        fs_StringBuilder_apend(sb, "<threadCount commet=\"totall thread count\">00000</threadCount>");
        fs_StringBuilder_apend(sb, "<registeredThreadCount commet=\"registered thread count)\">00000</registeredThreadCount>");
        fs_StringBuilder_apend(sb, "<cpuTime commet=\"process use cpu time(s)\">00000000000000000000</cpuTime>");
        fs_StringBuilder_apend(sb, "<registeredCpuTime commet=\"registered thread use cpu time(s)\">00000000000000000000</registeredCpuTime>");
        fs_StringBuilder_apend(sb, "<cpuTimeRate commet=\"process cpu occupy rate(%)\">0000000</cpuTimeRate>");
        fs_StringBuilder_apend(sb, "<registeredCpuTimeRate commet=\"registered thread cpu occupy rate(%)\">0000000</registeredCpuTimeRate>");

        fs_StringBuilder_apend(sb, "<utime commet=\"process utime use cpu time(s)\">00000000000000000000</utime>");
        FsPrintf(1, "TTTTTTTTTTTTTTTTTTTTTTT\n");
        FsString *pString = fs_StringBuilder_to_String__IO(sb);
        FsPrintf(1, "TTTTTTTTTTTTTTTTTTTTTTT\n");
        fs_String_delete__OI(pString);
        FsPrintf(1, "TTTTTTTTTTTTTTTTTTTTTTT\n");
        fs_StringBuilder_delete__OI(sb);

        /* 释放异常捕捉内部变量的空间 */
        exceptionDelete();
        globalRelease();
        return 0;
    }
    if (0) {
        fs_Bd_backtrace_out(stdout);
        FsPrintf(1, "(%ld),(%ld),(%ld)\n", systemThreadTid, pthread_self(), syscall(224));
        return 0;
    }
    if (0) {
        globalInit(NULL, NULL, NULL, 0);
        fs_String_test();
        /* 释放异常捕捉内部变量的空间 */
        exceptionDelete();
        globalRelease();
        return 0;
    }
    if (0) {
        globalInit(NULL, NULL, NULL, 0);
        onvif_test(NULL);
        /* 释放异常捕捉内部变量的空间 */
        exceptionDelete();
        globalRelease();
        return 0;
    }
    if (0) {
#define Hls_private_get_snapshot_bmp00 "HTTP/1.1 200 OK\r\n\
Server: nginx/1.8.0\r\n\
Date: Fri, 19 May 2017 10:30:09 GMT\r\n\
Content-Type: image/jpeg\r\n\
Content-Length: 6220854\r\n\
Last-Modified: Fri, 19 May 2017 10:25:42 GMT\r\n\
Connection: keep-alive\r\n\
ETag: \"591ec826-5eec36\r\n\
Accept-Ranges: bytes\r\n\r\n"
        int bs = fs_Socket_tcpIpv4_bind__IO(0, 80, 0, 10);
        int s = fs_Socket_unix_accept(NULL, NULL, bs, 0, 100);
        char buf[1000];
        recv(s, buf, 100, 0);
        int len = strlen(Hls_private_get_snapshot_bmp00);
        FsString *pString = fs_String_new_form_file__IO("/aa/6.bmp");
        FsString *sendData = fs_String_new__IO(pString->lenth + len);
        memcpy(sendData->buffer, Hls_private_get_snapshot_bmp00, len);
        memcpy(sendData->buffer + len, pString->buffer, pString->lenth);
        for (len = 0; len < (int) sendData->lenth;) {
            int rv = send(s, sendData->buffer + len, sendData->lenth - len, 0);
            len += rv;
        }
        printf("send OK\n");
        while (1)sleep(10);
    }
    if (0) {
        globalInit(NULL, NULL, NULL, 0);
        fs_dns_test();
        /* 释放异常捕捉内部变量的空间 */
        exceptionDelete();
        /* 函数中的公共变量释放 */
        globalRelease();
        return EXIT_SUCCESS;
    }
    if (0) {
        unsigned int ipv4_local = 0;
        unsigned short ipv4Port_local = 1234;
        // ipv4Port_local=0;
        int s1 = fs_Socket_udpIPv4_connect__IO((172) | (20 << 8) | (0 << 16) | (2 << 24), 33, &ipv4_local, &ipv4Port_local, 1);
        FsPrintf(5, "%d-%hhu.%hhu.%hhu.%hhu:%hu.\n", s1,
                ((FsNetworkIPv4*) (&ipv4_local))->n.n1, ((FsNetworkIPv4*) (&ipv4_local))->n.n2,
                ((FsNetworkIPv4*) (&ipv4_local))->n.n3, ((FsNetworkIPv4*) (&ipv4_local))->n.n4, ipv4Port_local);
        fflush(stdout);
        ipv4_local = 0;
        //ipv4Port_local=0;
        s1 = fs_Socket_udpIPv4_connect__IO((172) | (20 << 8) | (0 << 16) | (3 << 24), 33, &ipv4_local, &ipv4Port_local, 1);
        FsPrintf(5, "%d-%hhu.%hhu.%hhu.%hhu:%hu.\n", s1,
                ((FsNetworkIPv4*) (&ipv4_local))->n.n1, ((FsNetworkIPv4*) (&ipv4_local))->n.n2,
                ((FsNetworkIPv4*) (&ipv4_local))->n.n3, ((FsNetworkIPv4*) (&ipv4_local))->n.n4, ipv4Port_local);
        fflush(stdout);
        return 0;
    }
    if (0) {
        time_t tm = time(NULL);
        struct tm ti;

        printf("%s\n", ctime(&tm));
        return 0;
    }
    if (0) {
        //        unsigned char tt[]={0x01, 0x02 ,0x03 ,0x04};
        //        FsPrintf(1, "%#X\n", fs_String_buffer_crc32_mpeg(tt, 1));
        //        return 1;
        globalInit(NULL, NULL, NULL, 0);
        hls_test();
        /* 释放异常捕捉内部变量的空间 */
        exceptionDelete();
        /* 函数中的公共变量释放 */
        globalRelease();
        return EXIT_SUCCESS;
    }
    if (0) {
        globalInit(NULL, NULL, NULL, 0);
        camera_ctrl_hk_http_test();
        /* 释放异常捕捉内部变量的空间 */
        exceptionDelete();
        /* 函数中的公共变量释放 */
        globalRelease();
        return EXIT_SUCCESS;
    }
    if (0) {
        unsigned int ipv4 = 0;
        unsigned short port = 1876;
        int sock = fs_Socket_udpIPv4_connect__IO(192 | (168 << 8) | (8 << 16) | (222 << 24), 16863, &ipv4, &port, 1);
        FsPrintf(5, "%hhu.%hhu.%hhu.%hhu:%hu\n",
                ((FsNetworkIPv4 *) & ipv4)->n.n1, ((FsNetworkIPv4 *) & ipv4)->n.n2, ((FsNetworkIPv4 *) & ipv4)->n.n3, ((FsNetworkIPv4 *) & ipv4)->n.n4, port);

        closesocket(sock);
        return 0;
    }
    if (0) {
        FsPrintf(1, "%hhd,%hhd\n", fs_time_timezone_get(), fs_timeZone_get());
        return 0;
    }
    if (0) {
        FsObjectList *list = fs_networkBridge_get__IO();
        int i = 0;
        for (i = 0; i < (int) list->nodeCount; i++) {
            FsPrintf(1, "%d/%lu-%s-%s-%hu.\n", i, list->nodeCount, ((FsNetworkInterface*) list->pNode[list->startIndex + i])->name, ((FsNetworkInterface*) list->pNode[list->startIndex + i])->mac,
                    ((FsNetworkInterface*) list->pNode[list->startIndex + i])->mtu);
        }
        fs_ObjectList_clear_allSequence(list);
        return 0;
    }
    if (0) {
        image_jpeg_test(0, 0);
        globalRelease();
        return 0;
    }
    if (0) {
        fs_TypeFace_test();
        globalRelease();
        return 0;
    }
    if (0) {
        fs_fileNO_set(0);
        sleep(30);
        return 0;
    }
    if (0) {
        globalInit(NULL, NULL, NULL, 0);
        video_h264_test();
        globalRelease();
        return 0;
    }
    if (0) {
        globalInit(NULL, NULL, NULL, 0);
        audio_aac_test();
        globalRelease();
        return 0;
    }
    if (0) {
        FsPrintf(1, "%lf-%lf\n", fs_time_ntp_get_byString("127.0.0.1", 123), fs_time_GMT_get());
        return 0;
    }
    if (0) {
        FsEbml *pEbml_send = fs_Ebml_new__IO(FsEbmlEncodeMethod_UTF8, 0);
        fs_Ebml_node_data_set_string(fs_Ebml_node_addChild(pEbml_send, (struct FsEbml_node*) pEbml_send, "redirectHost", FsEbmlNodeType_String), "192.168.1.114");
        FsJson *pJson = fs_Ebml_convert_to_Json__IO(pEbml_send);
        FsString *pString = fs_Json_to_String__IO(pJson, 8, sizeof (unsigned int)*3, FsStringLenData("\r\n"), 0, NULL, 4);
        FsPrintf2(5, "data:\n%s\n", pString->buffer + 8);
        FsArray *pArray = fs_Json_node_get_allData__IO(pJson, (struct FsJson_node*) pJson, "redirectHost", NULL);
        FsPrintf2(5, "data:\n%s\n", pArray->data[0]);
        fsFree(pArray);
        fs_Json_delete__OI(pJson, NULL);
        fs_String_delete__OI(pString);
        return 0;
    }
    if (0) {
        fs_network_test();
        return 0;
    }
    if (0) {
        globalInit(NULL, NULL, NULL, 0);
        fs_Bd_test();
        globalRelease();
        return 0;
    }
    if (0) {
        char buffer[] = "video20151009150400.mkv";
        struct tm ti;
        sscanf(buffer, "video%4d%2d%2d%2d%2d%2d*", &ti.tm_year, &ti.tm_mon, &ti.tm_mday, &ti.tm_hour, &ti.tm_min, &ti.tm_sec);
        char aa = 1;
        return 0;
    }
    if (0) {
        FsStringList *list = fs_dir_between_ASC__IO(strlen("/fs/project/data/vsys/2/video/"), "/fs/project/data/vsys/2/video/",
                "20151120/13/video20151120133650.mkv", "20151130/13/video20151120135808.mkv", NULL);
        if (list) {
            FsPrintf2(1, "nodeCount=%lu.\n", list->list.nodeCount);
            unsigned int i = list->list.nodeCount;
            char **ppNode = (char**) list->list.pNode + list->list.startIndex;
            while (i-- > 0) {
                printf("%s\n", *ppNode++);
            }
            fs_StringList_delete__OI(list);
        }
        return 0;
    }
    if (0) {
        FsPrintf(1, "ntp=%lf,gmt=%lf\n", fs_time_ntp_get(192 | (168 << 8) | (60 << 16) | (240 << 24), 123), fs_time_GMT_get());
        return 0;
    }
    if (0) {
        fs_Ebml_test();
        return 0;
    }
    if (0) {
        Monitor *pMonitor = monitor_new__IO(NULL, 0, "main.monitor", NULL/* "/tmp/running/" */, 0, 5.0, 0.2, 0, NULL, NULL, NULL);
        configManager_test(pMonitor);
        monitor_delete__OI(pMonitor);
        /* 释放异常捕捉内部变量的空间 */
        exceptionDelete();
        /* 函数中的公共变量释放 */
        globalRelease();
        return EXIT_SUCCESS;
    }
    if (0) {
        //char name[] = "/fs/project/data/vsys/2/video/20151204/08/video20151204080000.mkv";
        char name[] = "/fs/project/data/vsys/1/video/99999999/08/video20151204080000.mkv";
        char *buffer = (char*) fsMalloc(sizeof (name)*2 + sizeof ("99999999/08/video20151204080000.mkv")*2);
        char *rst_1 = buffer + sizeof ("99999999/08/video20151204080000.mkv")*2;
        char *rst_2 = buffer + sizeof ("99999999/08/video20151204080000.mkv")*2 + sizeof (name);
        fs_dir_match_prefect(rst_1, rst_2, "/fs/project/data/vsys/1/video/", name + strlen("/fs/project/data/vsys/1/video/"), buffer, 0);
        FsPrintf(1, "rst_1:\"%s\"\n", rst_1);
        FsPrintf(1, "rst_2:\"%s\"\n", rst_2);
        FsPrintf(1, "\n name:\"%s\"\nrst_1:\"%s\"\nrst_2:\"%s\"\n", name, rst_1, rst_2);
        fflush(stdout);
        fsFree(buffer);
        return 0;
    }
    if (0) {
        char name[] = "/fs/project/data/vsys/2/video/20151201/17/video20151210172012.mkv";
        FsStringList *list = fs_dir_match_prefect__IO("/fs/project/data/vsys/2/video/", name + strlen("/fs/project/data/vsys/2/video/"), 1);
        FsPrintf(1, "name:\n%s\n", name);
        fs_StringList_out(list, stdout, NULL, "\n");
        fflush(stdout);
        fs_StringList_delete__OI(list);
        return 0;
    }
    if (0) {
        fs_network_test();
        return 0;
    }
#endif

    MakeLog(NULL, 1);
    /* 函数中的公共变量初始化 */
    globalInit(NULL, NULL, NULL, 0);
    /* 修改支持的最大句柄数 */
    fs_fileNO_set(0);
    /* 创建根对象 */
    RootStruct * const pRootStruct = rootStruct_new__IO();
    pRootStruct->rw.runStatus = 1;
    ////////////////////////////////////////////////////////////////////////////
    /* 注册异常捕捉 */
    if (1) {
        FsPrintf(1, "tttttt\n");
        exceptionRegister_pthreadSafety(NULL, NULL, ExceptionErrorFileName2_Default, (signed char*) &pRootStruct->rw.runStatus, (signed char*) &pRootStruct->rw.runStatusExit);
        FsPrintf(1, "tttttt\n");
        exception_skip(SIGCLD);
        exception_skip(SIGPIPE);
        FsPrintf(1, "tttttt\n");
    }


    /* 创建线程监控对象 */
    Monitor * const pMonitor = monitor_new__IO((signed char*) &pRootStruct->rw.runStatus, 0, "main.monitor", "/tmp/running/", 1, 8.0, 0.2, fs_system_cpu_thread_get()*16
            , (void (*)(char, unsigned int, void *, char**)) main_cb_threadMulti, pRootStruct, NULL);
    /* 添加线程监控 */
    MonitorItem * const pMonitorItem = monitorItem_new_and_add(pMonitor, "main", NULL, pRootStruct, 0, NULL);
    //thread_join_add_check_pthreadSafety(MonitorItem_get_threadName(pMonitorItem), NULL);
    pMonitorItem->rw.line = __LINE__;
    /* 注销循环死锁监控 */
    pMonitorItem->rw.cycleAlive = -1;
    ////////////////////////////////////////////////////////////////////////////
    /* 全局变量 */
    signed char restart = 1;
    ////////////////////////////////////////////////////////////////////////////
    pMonitorItem->rw.line = __LINE__;
    while (restart) {
        ////////////////////////////////////////////////////////////////////////
        /* 等待资源初始化完成 */
        // 网络资源
        while (access("/tmp/sysinfo/networkOK", F_OK))usleep(10000);
        // 硬盘资源
        while (access("/tmp/sysinfo/diskOK", F_OK))usleep(10000);
        // 激活码
        while (access("/tmp/sysinfo/licOK", F_OK))usleep(10000);
        pMonitorItem->rw.line = __LINE__;
        pRootStruct->rw.runStatusExit = 1;
        pRootStruct->rw.runStatus = 1;
        ////////////////////////////////////////////////////////////////////////
        pMonitorItem->rw.line = __LINE__;
        BindClassifyServer * pBindClassifyServer;
#define __main_clean1_1 {if(pBindClassifyServer!=NULL)bindClassifyServer_stopThread(pBindClassifyServer),bindClassifyServer_delete__OI(pBindClassifyServer);}
#define __main_clean2_1 {(void*)bindClassifyServer_stopThread_noWait, pBindClassifyServer},
        if (strstr(argv[0], "/zos") || strcmp("zos", argv[0]) == 0 || strcmp("zos.new", argv[0]) == 0) {
            pBindClassifyServer = bindClassifyServer_new__IO(pMonitor, (signed char*) &pRootStruct->rw.runStatus, systemThreadTid, "BindClassifyS");
            bindClassifyServer_startThread(pBindClassifyServer, 0);
        } else pBindClassifyServer = NULL;

        pMonitorItem->rw.line = __LINE__;
        ConfigManager * pConfigManager;
#define __main_clean1_2 {if(pConfigManager!=NULL)configManager_stopThread(pConfigManager),configManager_delete__OI(pConfigManager,NULL);}
#define __main_clean2_2 {(void*)configManager_stopThread_noWait, pConfigManager},
        if (strstr(argv[0], "/zos") || strcmp("zos", argv[0]) == 0 || strcmp("zos.new", argv[0]) == 0) {
            struct ConfigManager_User * pConfigManager_User = (struct ConfigManager_User *) fsMalloc(sizeof ( struct ConfigManager_User)*4);
            pConfigManager_User[0].authority = 0x1, pConfigManager_User[0].shortcut = "ROOT权限", pConfigManager_User[0].comment = NULL;
            pConfigManager_User[1].authority = 0x2, pConfigManager_User[1].shortcut = "配置修改权限", pConfigManager_User[1].comment = NULL;
            pConfigManager_User[2].authority = 0x0;
            pConfigManager = configManager_new__IO__OI_9(pMonitor, (signed char*) &pRootStruct->rw.runStatus, systemThreadTid, "ConfigManager"
                    , "/fs/project/etc/config-v0.cfg", "/tmp/sysinfo", 0, 16863, pConfigManager_User, pBindClassifyServer, 0x2U, "/tmp/sysinfo/licInfo", 0xF0U, NULL);
            configManager_startThread(pConfigManager, 0);
        } else pConfigManager = NULL;

#define __main_clean1_3
#define __main_clean2_3
#define __main_clean1_4
#define __main_clean2_4
#define __main_clean1_5
#define __main_clean2_5

        pMonitorItem->rw.line = __LINE__;
        SdkPlayBack * pSdkPlayBack;
#define __main_clean1_6 {if(pSdkPlayBack!=NULL)sdkPlayBack_stopThread(pSdkPlayBack),sdkPlayBack_delete__OI(pSdkPlayBack);}
#define __main_clean2_6 {(void*)sdkPlayBack_stopThread_noWait, pSdkPlayBack},
        if (strstr(argv[0], "/sdkPlayBack") || strcmp("sdkPlayBack", argv[0]) == 0 || strcmp("sdkPlayBack.new", argv[0]) == 0
                || ((strstr(argv[0], "/zos") || strcmp("zos", argv[0]) == 0 || strcmp("zos.new", argv[0]) == 0)
                && access("/usr/lib/systemd/system/sdkPlayBack.service", F_OK) != 0 && access("/fs/bin/sdkPlayBack", F_OK) != 0)) {
            pSdkPlayBack = sdkPlayBack_new__IO(pMonitor, (signed char*) &pRootStruct->rw.runStatus, systemThreadTid, "mSdkPlayBack");
            sdkPlayBack_startThread(pSdkPlayBack, 0);
            pMonitorItem->rw.line = __LINE__;
            /* 添加大华回放sdk */
            if (1) {
                DHSDK * const pDHSDK = dhsdk_new__IO(pMonitor, (signed char*) &pRootStruct->rw.runStatus, systemThreadTid, "dhsdk", 64 * pRootStruct->p._threadMulti);
                struct SdkPlayBack_item * const pSdkPlayBack_item = sdkPlayBack_item_new__IO__OI_2("/tmp/config/dhsdk_", pDHSDK, NULL, (void (*const) (void *const))dhsdk_delete__OI
                        , (void (* const) (void * const, const signed char))dhsdk_startThread, (void (*const) (void *const))dhsdk_stopThread_noWait, (void (*const) (void *const))dhsdk_stopThread
                        , (void * (* const) (const char *, const char *))dhsdk_PlayBack_item_new_byUrl__IO, (void (*const) (void *const))dhsdk_PlayBack_item_delete__OI
                        , (void (*const) (void *const, void *const)) dhsdk_PlayBack_add_item__OI_2, (void (*const) (void *const, void *const))dhsdk_PlayBack_remove_item_noWait__IO_2
                        , (void (* const) (void * const, void * const, FsObjectSdkPlayBackTask * const))dhsdk_PlayBack_item_updatePlayBackTask_pthreadSafety
                        , (void (* const) (void * const, void * const, FsObjectSdkPlayBackTask * const))dhsdk_PlayBack_item_deletePlayBackTask_pthreadSafety__OI_2);
                sdkPlayBack_add_item__OI_2(pSdkPlayBack, pSdkPlayBack_item);
            }
            if (1 && Vsys_sdkPlayBack_is_process()) {
                /* 添加GB28181回放sdk */
                GBSDK * const pGBSDK = gbsdk_new__IO(pMonitor, (signed char*) &pRootStruct->rw.runStatus, systemThreadTid, "gbsdk", 64 * pRootStruct->p._threadMulti, pRootStruct->p._threadMulti, NULL, NULL, NULL);
                struct SdkPlayBack_item * const pSdkPlayBack_item = sdkPlayBack_item_new__IO__OI_2("/tmp/config/gbsdk_", pGBSDK
                        , (void (*const) (void *const, FsEbml * const, char * * const))gbsdk_update_parameter, (void (*const) (void *const))gbsdk_delete__OI
                        , (void (* const) (void * const, const signed char))gbsdk_startThread, (void (*const) (void *const))gbsdk_stopThread_noWait, (void (*const) (void *const))gbsdk_stopThread
                        , (void * (* const) (const char *, const char *))gbsdk_PlayBack_item_new_byUrl__IO, (void (*const) (void *const))gbsdk_PlayBack_item_delete__OI
                        , (void (*const) (void *const, void *const)) gbsdk_PlayBack_add_item__OI_2, (void (*const) (void *const, void *const))gbsdk_PlayBack_remove_item_noWait__IO_2
                        , (void (* const) (void * const, void * const, FsObjectSdkPlayBackTask * const))gbsdk_PlayBack_item_updatePlayBackTask_pthreadSafety
                        , (void (* const) (void * const, void * const, FsObjectSdkPlayBackTask * const))gbsdk_PlayBack_item_deletePlayBackTask_pthreadSafety__OI_2);
                sdkPlayBack_add_item__OI_2(pSdkPlayBack, pSdkPlayBack_item);
            }
            pMonitorItem->rw.line = __LINE__;
        } else pSdkPlayBack = NULL;

        pMonitorItem->rw.line = __LINE__;
        struct Vsys * pVsys;
#define __main_clean1_7 {if(pVsys!=NULL)vsys_stopThread(pVsys),vsys_delete__OI(pVsys);}
#define __main_clean2_7 {(void*)vsys_stopThread_noWait, pVsys},
        if (strstr(argv[0], "/zos") || strcmp("zos", argv[0]) == 0 || strcmp("zos.new", argv[0]) == 0) {
            pVsys = vsys_new__IO(pMonitor, (signed char*) &pRootStruct->rw.runStatus, systemThreadTid, "vsys", pRootStruct->p._threadMulti, pConfigManager, pBindClassifyServer
                    , pSdkPlayBack

                    );
            vsys_startThread(pVsys, 1);
        } else pVsys = NULL;

#define __main_clean1_8
#define __main_clean2_8


#define __main_clean1_9
#define __main_clean2_9


#define __main_clean1_10
#define __main_clean2_10

#define __main_clean1_count 10
#define __main_clean2_count 10
        pMonitorItem->rw.line = __LINE__;
        //FsPrintf(1, "%s\n", FsMacrosString(FsClean(2))); // 打印析构脚本
        void * stop_thread_noWait[][2] = {
            FsClean(2)
            {
                NULL, NULL
            }
        };
        ////////////////////////////////////////////////////////////////////////
        monitor_startThread(pMonitor, 0, stop_thread_noWait);
        pMonitorItem->rw.line = __LINE__;
        ////////////////////////////////////////////////////////////////////////
        /* 检查是否重启 */
        if (pRootStruct->rw.runStatus != 2 || (pRootStruct->rw.runStatusExit != 2 && pRootStruct->rw.runStatusExit != 1)) {
            restart = 0;
        } else {
            FsPrintf(FsPrintfIndex, "Restart.\n##################################################\n##################################################\n");
        }
        pRootStruct->rw.runStatus = 255;
        ////////////////////////////////////////////////////////////////////////
        pMonitorItem->rw.line = __LINE__;
        FsClean(1);
        pMonitorItem->rw.line = __LINE__;
        monitor_stopThread(pMonitor);
        pMonitorItem->rw.line = __LINE__;
    }
    pMonitorItem->rw.line = __LINE__;
    ////////////////////////////////////////////////////////////////////////////
    MakeLog(NULL, 2);
    /* 停止线程监控 */
    monitor_stopThread(pMonitor);
    /* 删除线程监控 */
    monitorItem_remove_and_delete(pMonitor, pMonitorItem, NULL);
    /* 删除线程监控对象 */
    monitor_delete__OI(pMonitor);
    ////////////////////////////////////////////////////////////////////////////
    /* 删除根对象 */
    rootStruct_delete__OI(pRootStruct);
    /* 释放异常捕捉内部变量的空间 */
    exceptionDelete();
    /* 函数中的公共变量释放 */
    globalRelease();
    return EXIT_SUCCESS;
}
///////////////////////////定义公共函数结束//////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
