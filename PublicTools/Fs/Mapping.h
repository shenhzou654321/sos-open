/* 
 * File:   Mapping.h
 * Author: fsflib
 * 信息映射对象
 * Created on 2019年4月14日, 下午1:01
 */

#ifndef MAPPING_H
#define MAPPING_H

#include "../publicDefine.h"
#ifdef __cplusplus
extern "C" {
#endif

    /* 映射对象,每个节点一个int,其中高8位为特有标记 */
    typedef struct {
        /* 节点数量 */
        unsigned int nodeCount;
        /* 节点 */
        unsigned int node[0];
    } Mapping_I8;

#ifdef __cplusplus
}
#endif

#endif /* MAPPING_H */

