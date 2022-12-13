/* 
 * File:   CarDetect.h
 * Author: fslib
 *
 * Created on 2013年6月15日, 上午11:19
 */

#ifndef CARDETECT_H
#define	CARDETECT_H
#include "../publicDefine.h"
#include "../Fs/Memery.h"
#include "../Fs/Object.h"
#ifdef	__cplusplus
extern "C" {
#endif

    /* 车辆检测 */
    typedef struct {

        /* 
         * 结构约定:
         *     r中为外部可读变量(仅限当前指针所指向的结构,内部指针所指向的内容是否可读由内部结构决定,即r中的变量值在赋值后可能会修改),在集群应用时,用于存放公共变量;
         *     ro中为外部只读变量(仅限当前指针所指向的结构,内部指针所指向的内容是否可读由内部结构决定,即ro中的变量值在赋值后不会修改);
         *     rw中为外部可读写变量(仅限当前指针所指向的结构,内部指针所指向的内容是否可读写由内部结构决定,即rw中的变量值可在外部及内部直接修改,如是指针类变量应提供锁机制);
         *     p中为私有变量,外部不可读也不可写,(即p中的变量可能会被内部修改,对指针类变量,外部不能通过其指针直接访问指针指向的数据);
         * 变量命名约定:
         *     结构中所有变量名在ro,rw,p中要唯一,如不能出现ro中的变量与rw中的变量同名;
         *     变量名不能使用_前缀后缀区分不同变量名,__为分隔符,分隔符后的不作为变量名,如_aa_bb__cc表示aa_bb变量;
         *     凡需要(非0)初始化的变量,都要加上_的前缀;
         *     凡需要内存释放的变量,都要加上__的前缀;
         *     整形变量使用_结尾表示变量值为相对结构体尾部的一个偏移量,最大值表示无效值;
         *     指针变量使用_结尾表示变量指向的对象丧失安全设置,如需要使用其他变量的安全设置应使用__连接其他变量名,如本变量aa使用bb变量的安全设置,应命名为aa__bb.
         */
        struct {
            /* 图像宽度 */
            unsigned short _width;
            /* 图像高度 */
            unsigned short _height;
            /* 运动检测阈值,默认20 */
            unsigned short _threshold;
            /* 检测区域 */
            unsigned short _x1, _y1, _x2, _y2;
            /* 掩码图,0-不检测,非0代表不同区域 */
            unsigned char *_mask;
            /* 掩码图的储存宽度 */
            unsigned int _maskLineBits;
            /* 灭点 */
            unsigned int _vanishingPoint;
        } ro;

        struct {
        } rw;

        struct {
            /* 帧间隔 */
            unsigned int _frameInterval;
            /* frameInterval补偿 */
            float frameIntervalCompensate;
            /* 帧率 */
            float _frameRate;
            /* 调试数据掩码
             *     0-帧差数据
             *     1-平滑后的边缘数据
             *     2-平滑后的二值化数据
             *     3-目标提取图像
             *     4-目标框
             */
            unsigned int debugMask;
            /* 调试数据中data的偏移位置,不小于sizeof(FsObjectBasePthreadSafety) */
            unsigned int debugDataOffset[5];
            /* 调试数据中实际数据结束位置距离返回结果的data尾的字节数 */
            unsigned int debugSuffixCount[5];
            /* 调试数据 */
            FsObjectBase *debugData[5];
        } p;
    } CarDetect;
    /* 新建一个车辆检测对象 */
    CarDetect *carDetect_new__IO(/* 图像宽度 */unsigned short width, /* 图像高度 */unsigned short height, /* 运动检测阈值,默认20 */ unsigned short threshold
            , /* 灭点 */ const unsigned int vanishingPoint, /* 掩码图,0-不检测,非0代表不同区域 */ unsigned char mask[]
            , /* 掩码图的储存宽度 */unsigned int lineBits, /* 外部掩码图是否可以被内部共享,0-不共享,1-共享 */ char maskShare);
    /* 删除车辆检测对象 */
    void carDetect_delete__OI(CarDetect * const pCarDetect);
    /* 新建一个车辆检测对象,返回检测到的车辆数 */
    int carDetect_detect(CarDetect * const pCarDetect, /* 是多可输出的目标数量 */unsigned int targetCount, /* 输出目标id */unsigned int id[]
            , /* 输出目标左上角的x坐标 */unsigned int x1[], /* 输出目标左上角的y坐标 */ unsigned int y1[]
            , /* 输出目标右下角的x坐标 */unsigned int x2[], /* 输出目标右下角的y坐标 */unsigned int y2[]
            , /* 当前图的水平边缘 */short hedge[], /* 当前图的帧号 */ unsigned int frameIndex, /* 上一张图的水平边缘 */short hedgeLast1[], /* 上一张图的帧号 */ unsigned int frameIndexLast1
            , /* 上上一张图的水平边缘 */short hedgeLast2[], /* 上上一张图的帧号 */ unsigned int frameIndexLast2
            , /* 储存宽度,必须为2的倍数 */unsigned int lineBits, /* 帧率 */ float frameRate
            , /* 缓存Buffer,不为空 */FsObjectBaseBuffer * const pObjectBaseBuffer
            , /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);
    /* 获取调试数据,成功返回FsObjectBasePthreadSafety指针,失败返回NULL */
    FsObjectBasePthreadSafety *carDetect_debug__IO(CarDetect * const pCarDetect, /* 调试数据类型,0-帧差数据,1-平滑后的边缘数据,2-平滑后的二值化数据,3-目标提取图像,4-目标框 */const int type
            , /* 指data的偏移位置,不小于sizeof(FsObjectBase) */ const unsigned int dataOffset, /* 实际数据结束位置距离返回结果的data尾的字节数 */const unsigned int suffixCount);
#ifdef	__cplusplus
}
#endif

#endif	/* CARDETECT_H */

