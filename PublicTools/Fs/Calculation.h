/* 
 * File:   Calculation.h
 * Author: fsroot
 *
 * Created on 2013年5月20日, 下午1:01
 */

#ifndef CALCULATION_H
#define CALCULATION_H

#include "Network.h"
#include "StringList.h"

#ifdef __cplusplus
extern "C" {
#endif
    /* 获取2字节数据的有符号整型值,v是指针 */
#define Fs_Number_L2(v) (((short)(*((unsigned char*)(v))))|(((short)(*(((unsigned char*)(v))+1)))<<8))
    /* 获取2字节数据的无符号整型值,v是指针 */
#define Fs_Number_L2U(v) (((unsigned short)(*((unsigned char*)(v))))|(((unsigned short)(*(((unsigned char*)(v))+1)))<<8))
    /* 获取4字节数据的有符号整型值,v是指针 */
#define Fs_Number_L4(v) (((int)(*((unsigned char*)(v))))|(((int)(*(((unsigned char*)(v))+1)))<<8)|(((int)(*(((unsigned char*)(v))+2)))<<16)|(((int)(*(((unsigned char*)(v))+3)))<<24))
    /* 获取4字节数据的无符号整型值,v是指针 */
#define Fs_Number_L4U(v) (((unsigned int)(*((unsigned char*)(v))))|(((unsigned int)(*(((unsigned char*)(v))+1)))<<8)|(((unsigned int)(*(((unsigned char*)(v))+2)))<<16)|(((unsigned int)(*(((unsigned char*)(v))+3)))<<24))
    /* 获取8字节数据的有符号整型值,v是指针 */
#define Fs_Number_L8(v) (((long long)(*((unsigned char*)(v))))|(((long long)(*(((unsigned char*)(v))+1)))<<8)|(((long long)(*(((unsigned char*)(v))+2)))<<16)|(((long long)(*(((unsigned char*)(v))+3)))<<24)\
|(((long long)(*(((unsigned char*)(v))+4)))<<32)|(((long long)(*(((unsigned char*)(v))+5)))<<40)|(((long long)(*(((unsigned char*)(v))+6)))<<48)|(((long long)(*(((unsigned char*)(v))+7)))<<56))
    /* 获取8字节数据的无符号整型值,v是指针 */
#define Fs_Number_L8U(v) (((unsigned long long)(*((unsigned char*)(v))))|(((unsigned long long)(*(((unsigned char*)(v))+1)))<<8)|(((unsigned long long)(*(((unsigned char*)(v))+2)))<<16)|(((unsigned long long)(*(((unsigned char*)(v))+3)))<<24)\
|(((unsigned long long)(*(((unsigned char*)(v))+4)))<<32)|(((unsigned long long)(*(((unsigned char*)(v))+5)))<<40)|(((unsigned long long)(*(((unsigned char*)(v))+6)))<<48)|(((unsigned long long)(*(((unsigned char*)(v))+7)))<<56))

    /* 大数字对象,仅支持整数,数据储存方式:低位在前 */
    typedef struct {
        /* 数据长度 */
        unsigned int lenth;
        /* 数据 */
        unsigned char *data;
    } FsNumber;
    /*
     * 创建一个新的FsNumber对象;
     * 返回FsNumber指针.
     */
    FsNumber *fs_Number_new__IO();
    /* 删除pNumber指向的实例对象 */
    void fs_Number_delete__OI(FsNumber *pNumber);
    /* 把pNumberSrc的值赋值给pNumber */
    void fs_Number_set(FsNumber *pNumber, const FsNumber *pNumberSrc);
    /* 把pNumber1和pNumber2相加,得到一个新的数字 */
    FsNumber *fs_Calculation_add__IO(const FsNumber *pNumber1, const FsNumber *pNumber2);
    /* 把pNumber1和pNumber2相加,结果储存在pNumber1中 */
    void fs_Calculation_add(FsNumber *pNumber1, const FsNumber *pNumber2);
    /* 初始化按bit读取数据所需要的left和block,在读取过程中滑块在掩码的值如与mask相同,则多读1一字节,滑块只能忽略最后一字节 */
#define fs_Number_bit_init(/* 数据开始位置 const unsigned char * */data_v, /* 数据长度,包含data当前指向字节 unsigned char */ dataLenth_v, /* data当前指向的字节还有多少bit未读取 unsigned char */ left_v,\
    /* 4字节的滑动块 unsigned int */ block_v,/* 忽略值 unsigned int */black,/* 忽略值的掩码 unsigned int */mask) do{\
    if((dataLenth_v)>0){\
        block_v=((unsigned int)((const unsigned char*)(data_v))[0])<<24;\
        if((mask)==(0xFF000000U&(mask)) && ((block_v)&(mask))==(black) && (dataLenth_v)>1){\
            (data_v)++;\
            block_v=(((unsigned int)((block_v)<<8))>>8)|(((unsigned int)((const unsigned char*)(data_v))[0])<<24);\
            (dataLenth_v)--;\
        }\
        if((dataLenth_v)>1){\
            block_v = ((block_v)>>8) | (((unsigned int)((const unsigned char*)(data_v))[1])<<24);\
            if((mask)==(0xFFFF0000U&(mask)) && ((block_v)&(mask))==(black) && (dataLenth_v)>2){\
                (data_v)++;\
                block_v=(((unsigned int)((block_v)<<8))>>8)|(((unsigned int)((const unsigned char*)(data_v))[1])<<24);\
                (dataLenth_v)--;\
            }\
            if((dataLenth_v)>2){\
                block_v = ((block_v)>>8) | (((unsigned int)((const unsigned char*)(data_v))[2])<<24);\
                if((mask)==(0xFFFFFF00U&(mask)) && ((block_v)&(mask))==(black) && (dataLenth_v)>3){\
                    (data_v)++;\
                    block_v=(((unsigned int)((block_v)<<8))>>8)|(((unsigned int)((const unsigned char*)(data_v))[2])<<24);\
                    (dataLenth_v)--;\
                }\
                if((dataLenth_v)>3){\
                    block_v = ((block_v)>>8) | (((unsigned int)((const unsigned char*)(data_v))[3])<<24);\
                    if((mask)==(0xFFFFFF00U&(mask)) && ((block_v)&(mask))==(black) && (dataLenth_v)>4){\
                        (data_v)++;\
                        block_v=(((unsigned int)((block_v)<<8))>>8)|(((unsigned int)((const unsigned char*)(data_v))[3])<<24);\
                        (dataLenth_v)--;\
                    }\
                }else (block_v) >>= 8;\
            }else (block_v) >>= 16;\
        }else (block_v) >>= 24;\
        left_v=8;\
        (data_v)++;\
        (dataLenth_v)--;\
    }else{\
        FsPrintf(5,"Invalid dataLenth(=%u),data=%p.\n",dataLenth_v,data_v);\
        fflush(stdout);\
    }\
}while(0)
    /* 从data中读取1位数据,在读取过程中滑块在掩码的值如与mask相同,则多读1一字节,滑块只能忽略最后一字节 */
#define fs_Number_bit_read_u1(/* unsigned char */rst_v,/* 数据开始位置 const unsigned char * */data_v, /* 数据长度,不包含data当前指向字节 unsigned char */ dataLenth_v, /* data当前指向的字节还有多少bit未读取 unsigned char */ left_v,\
    /* 4字节的滑动块 unsigned int */ block_v,/* 忽略值 unsigned int */black,/* 忽略值的掩码 unsigned int */mask) do{\
    rst_v=((block_v)>>--(left_v))&0x1;\
    if(0==(left_v)&&(dataLenth_v)>0){\
        if((dataLenth_v)>3){\
            block_v=((block_v)>>8)|(((unsigned int)((const unsigned char*)(data_v))[3])<<24);\
            if(((block_v)&(mask))==(black)&&(dataLenth_v)>3){\
                (data_v)++;\
                (dataLenth_v)--;\
                block_v=(((unsigned int)((block_v)<<8))>>8)|(((unsigned int)((const unsigned char*)(data_v))[3])<<24);\
            }\
        }else (block_v) >>=8;\
        left_v=8;\
        (data_v)++;\
        (dataLenth_v)--;\
    }\
}while(0)
    /* 从data中读取1位数据丢弃,在读取过程中滑块在掩码的值如与mask相同,则多读1一字节,滑块只能忽略最后一字节 */
#define fs_Number_bit_skip_u1(/* 数据开始位置 const unsigned char * */data_v, /* 数据长度,不包含data当前指向字节 unsigned char */ dataLenth_v, /* data当前指向的字节还有多少bit未读取 unsigned char */ left_v,\
    /* 4字节的滑动块 unsigned int */ block_v,/* 忽略值 unsigned int */black,/* 忽略值的掩码 unsigned int */mask) do{\
    if(0== --(left_v)&&(dataLenth_v)>0){\
        if((dataLenth_v)>3){\
            block_v=((block_v)>>8)|(((unsigned int)((const unsigned char*)(data_v))[3])<<24);\
            if(((block_v)&(mask))==(black)&&(dataLenth_v)>3){\
                (data_v)++;\
                (dataLenth_v)--;\
                block_v=(((unsigned int)((block_v)<<8))>>8)|(((unsigned int)((const unsigned char*)(data_v))[3])<<24);\
            }\
        }else (block_v) >>=8;\
        left_v=8;\
        (data_v)++;\
        (dataLenth_v)--;\
    }\
}while(0)
    /* 从data中读取1位数据,不会改变读取位置 */
#define fs_Number_bit_peek_u1(/* unsigned char */rst_v,/* data当前指向的字节还有多少bit未读取 unsigned char */ left_v,  /* 4字节的滑动块 unsigned int */ block_v) do{\
    rst_v=((block_v)>>((left_v)-1))&0x1;\
}while(0)
    /* 从data中读取n位数据,在读取过程中滑块在掩码的值如与mask相同,则多读1一字节,滑块只能忽略最后一字节 */
#define fs_Number_bit_read_u(/* unsigned int */rst_v,/* 数据开始位置 const unsigned char * */data_v, /* 数据长度,不包含data当前指向字节 unsigned char */ dataLenth_v, /* data当前指向的字节还有多少bit未读取 unsigned char */ left_v,\
  /* 读取的位数,最大32 unsigned char */ n, /* 4字节的滑动块 unsigned int */ block_v,/* 忽略值 unsigned int */black,/* 忽略值的掩码 unsigned int */mask) do{\
    rst_v=0;\
    unsigned char i=0;\
    unsigned int __v;\
    for (; i < n; i++) {\
        fs_Number_bit_read_u1(__v,data_v, dataLenth_v, left_v,block_v,black,mask);\
        (rst_v) |= __v << ((unsigned char)(n - i - 1));\
        if (0==(left_v)){\
            rst_v=0;\
            break;\
        }\
    }\
}while(0)
    /* 从data中读取n位数据丢弃,在读取过程中滑块在掩码的值如与mask相同,则多读1一字节,滑块只能忽略最后一字节 */
#define fs_Number_bit_skip_u(/* 数据开始位置 const unsigned char * */data_v,  /* 数据长度,不包含data当前指向字节 unsigned char */ dataLenth_v, /* data当前指向的字节还有多少bit未读取 unsigned char */ left_v,\
   /* 读取的位数,最大32 unsigned char */ n, /* 4字节的滑动块 unsigned int */ block_v,/* 忽略值 unsigned int */black,/* 忽略值的掩码 unsigned int */mask) do{\
    unsigned char i=0;\
    for (; i < n; i++) {\
        fs_Number_bit_skip_u1(data_v,dataLenth_v,left_v,block_v,black,mask);\
        if (0==(left_v))break;\
    }\
}while(0)
    /* 从data中读取8位数据,在读取过程中滑块在掩码的值如与mask相同,则多读1一字节,滑块只能忽略最后一字节 */
#define fs_Number_bit_read_u8(/* unsigned char */rst_v,/* 数据开始位置 const unsigned char * */data_v, /* 数据长度,不包含data当前指向字节 unsigned char */ dataLenth_v, /* data当前指向的字节还有多少bit未读取 unsigned char */ left_v,\
    /* 4字节的滑动块 unsigned int */ block_v,/* 忽略值 unsigned int */black,/* 忽略值的掩码 unsigned int */mask) do{\
    if(8==(left_v)){\
        rst_v=(block_v)&0xFFU;\
        if((dataLenth_v)>0){\
            if((dataLenth_v)>3){\
                block_v=((block_v)>>8)|(((unsigned int)((const unsigned char*)(data_v))[3])<<24);\
                if(((block_v)&(mask))==(black)&&(dataLenth_v)>3){\
                    (data_v)++;\
                    (dataLenth_v)--;\
                    block_v=(((unsigned int)((block_v)<<8))>>8)|(((unsigned int)((const unsigned char*)(data_v))[3])<<24);\
                }\
            }else (block_v) >>=8;\
            (data_v)++;\
            (dataLenth_v)--;\
        }else left_v=0;\
    }else fs_Number_bit_read_u(rst_v,data_v, dataLenth_v, left_v,8,block_v,black,mask);\
}while(0)
    /* 从data中读取8位数据丢弃,在读取过程中滑块在掩码的值如与mask相同,则多读1一字节,滑块只能忽略最后一字节 */
#define fs_Number_bit_skip_u8(/* 数据开始位置 const unsigned char * */data_v, /* 数据长度,不包含data当前指向字节 unsigned char */ dataLenth_v, /* data当前指向的字节还有多少bit未读取 unsigned char */ left_v,\
    /* 4字节的滑动块 unsigned int */ block_v,/* 忽略值 unsigned int */black,/* 忽略值的掩码 unsigned int */mask) do{\
    if(8==(left_v)){\
        if((dataLenth_v)>0){\
            if((dataLenth_v)>3){\
                block_v=((block_v)>>8)|(((unsigned int)((const unsigned char*)(data_v))[3])<<24);\
                if(((block_v)&(mask))==(black)&&(dataLenth_v)>3){\
                    (data_v)++;\
                    (dataLenth_v)--;\
                    block_v=(((unsigned int)((block_v)<<8))>>8)|(((unsigned int)((const unsigned char*)(data_v))[3])<<24);\
                }\
            }else (block_v) >>=8;\
            (data_v)++;\
            (dataLenth_v)--;\
        }else left_v=0;\
    }else fs_Number_bit_skip_u(data_v, dataLenth_v, left_v,8,block_v,black,mask);\
}while(0)
    /* 从data中按0的个数读取位数读取数据,在读取过程中滑块在掩码的值如与mask相同,则多读1一字节,滑块只能忽略最后一字节 */
#define fs_Number_bit_read_ue(/* unsigned int */rst_v,/* 数据开始位置 const unsigned char * */data_v, /* 数据长度,不包含data当前指向字节 unsigned char */ dataLenth_v, /* data当前指向的字节还有多少bit未读取 unsigned char */ left_v,\
    /* 4字节的滑动块 unsigned int */ block_v,/* 忽略值 unsigned int */black,/* 忽略值的掩码 unsigned int */mask) do{\
    unsigned char __n=0,__v=0;\
    for (;left_v>0;) {\
        fs_Number_bit_read_u1(__v,data_v, dataLenth_v, left_v,block_v,black,mask);\
        if(0==__v&&__n<32)__n++;\
        else break;\
    }\
    fs_Number_bit_read_u(rst_v,data_v, dataLenth_v, left_v,__n,block_v,black,mask);\
    (rst_v) += (unsigned int)((1 << __n) - 1);\
}while(0)
    /* 从data中按0的个数读取位数读取数据丢弃,在读取过程中滑块在掩码的值如与mask相同,则多读1一字节,滑块只能忽略最后一字节 */
#define fs_Number_bit_skip_ue(/* 数据开始位置 const unsigned char * */data_v,  /* 数据长度,不包含data当前指向字节 unsigned char */ dataLenth_v, /* data当前指向的字节还有多少bit未读取 unsigned char */ left_v,\
    /* 4字节的滑动块 unsigned int */ block_v,/* 忽略值 unsigned int */black,/* 忽略值的掩码 unsigned int */mask) do{\
    unsigned char __n=0,__v=0;\
    for (;left_v>0;) {\
        fs_Number_bit_read_u1(__v,data_v, dataLenth_v, left_v,block_v,black,mask);\
        if(0==__v&&__n<32)__n++;\
        else break;\
    }\
    fs_Number_bit_skip_u(data_v,dataLenth_v,left_v,__n,block_v,black,mask);\
}while(0)
    /* 获取数组中第___index位的值,从0开始 */
#define fs_NumberArray_getBit_true(/* mask数组 */___mask,/* 获取位掩码的第几位,从0开始 */___index) \
    (((___mask)[(___index)/(sizeof((___mask)[0])<<3)]>>((___index)&((sizeof((___mask)[0])<<3)-1)))&0x1)
    /* 设置数组中第___index位为1,从0开始 */
#define fs_NumberArray_setBit_true(/* mask数组 */___mask,/* 获取位掩码的第几位,从0开始 */___index) \
    (___mask)[(___index)/(sizeof((___mask)[0])<<3)] |= ((((___mask)[0]^(___mask)[0])|0x1)<<((___index)&((sizeof((___mask)[0])<<3)-1)))
    /* 设置数组中第___index位为0,从0开始 */
#define fs_NumberArray_setBit_false(/* mask数组 */___mask,/* 获取位掩码的第几位,从0开始 */___index) \
    (___mask)[(___index)/(sizeof((___mask)[0])<<3)] &=~((((___mask)[0]^(___mask)[0])|0x1)<<((___index)&((sizeof((___mask)[0])<<3)-1)))
    /* 对数组中第___index位为取反,从0开始 */
#define fs_NumberArray_setBit_not(/* mask数组 */___mask,/* 获取位掩码的第几位,从0开始 */___index) \
    (___mask)[(___index)/(sizeof((___mask)[0])<<3)] ^=((((___mask)[0]^(___mask)[0])|0x1)<<((___index)&((sizeof((___mask)[0])<<3)-1)))

    /* 位掩码 */
    typedef struct {
        /* 掩码 */
        unsigned long long *mask;
        /* 当前掩码可以容纳的bit数 */
        unsigned int maskCount;
    } FsNumberBitMask;
    /* 释放位掩码空间 */
#define fs_NumberBitMask_delete(___mask) do{\
    if((___mask).mask)fsFree((___mask).mask);\
}while (0)

    /* 调整位掩码容量 */
#define fs_NumberBitMask_resize(___mask,  /* 设置的位掩码容量(bit数) unsigned int */ ___maskCount) do{\
    if((___mask).maskCount!=(((___maskCount)+(sizeof((___mask).mask[0])<<3)-1)&(~((sizeof((___mask).mask[0])<<3)-1)))){\
        (___mask).maskCount!=((___maskCount)+(sizeof((___mask).mask[0])<<3)-1)&(~((sizeof((___mask).mask[0])<<3)-1));\
        if((___mask).maskCount>0)(___mask).mask=(unsigned long long*)fsRealloc((___mask).mask,(___mask).maskCount>>3);\
    }\
}while (0)
    /* 获取位掩码的第___index位值,从0开始 */
#define fs_NumberBitMask_getBit_true(___mask,/* 获取位掩码的第几位,从0开始 */ ___index) fs_NumberArray_getBit_true((___mask).mask,___index)
    /* 设置位掩码的第___index位为1,从0开始 */
#define fs_NumberBitMask_setBit_true(___mask,/* 设置位掩码的第几位,从0开始 */ ___index) fs_NumberArray_setBit_true((___mask).mask,___index)
    /* 设置位掩码的第___index位为0,从0开始 */
#define fs_NumberBitMask_setBit_false(___mask,/* 设置位掩码的第几位,从0开始 */ ___index) fs_NumberArray_setBit_false((___mask).mask,___index)
    /* 对位掩码的第___index位为取反,从0开始 */
#define fs_NumberBitMask_setBit_not(___mask,/* 设置位掩码的第几位,从0开始 */ ___index) fs_NumberArray_setBit_not((___mask).mask,___index)
    /* 计算4字节数字v中有多少位的值为1 */
    int fs_number_countbit_v1_4(unsigned int v);
    void fs_Calculation_test();


#ifdef __cplusplus
}
#endif

#endif /* CALCULATION_H */

