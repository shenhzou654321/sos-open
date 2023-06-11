/* 
 * File:   ObjectTrack.h
 * Author: fslib
 *
 * Created on 2013年6月15日, 上午11:19
 */
#if !defined OBJRCTTRACK_H && !defined JAVA_IN_C

#define OBJRCTTRACK_H

#include "../../PublicTools/publicDefine.h"
#include "../../PublicTools/Fs/ObjectList.h"
#include "../../PublicTools/Fs/StructList.h"
#ifdef __cplusplus
extern "C" {
#endif

    struct ObjectTrack_pos_node {
        /* 坐标 */
        int x1, y1, x2, y2;
        /* 采集时间之gmt时间 */
        double capture_gmtTime;
        /* 坐标对应的的帧号,只使用3个字节 */
        unsigned int frameIndex;
        /* 下一个 */
        struct ObjectTrack_pos_node *next;
    };
#define __ObjectTrack_pos_nodePrintf(___Printf,___format, ...)\
    ___Printf(5,___format",p=%p,pos=(%d,%d)(%d,%d),frameIndex=%u,next=%p\n",##__VA_ARGS__\
        ,pObjectTrack_pos_node,((struct ObjectTrack_pos_node*)(pObjectTrack_pos_node))->x1,((struct ObjectTrack_pos_node*)(pObjectTrack_pos_node))->y1\
        ,((struct ObjectTrack_pos_node*)(pObjectTrack_pos_node))->x2,((struct ObjectTrack_pos_node*)(pObjectTrack_pos_node))->y2\
        ,((struct ObjectTrack_pos_node*)(pObjectTrack_pos_node))->frameIndex,((struct ObjectTrack_pos_node*)(pObjectTrack_pos_node))->next)
#define ObjectTrack_pos_nodePrintf(___format, ...) __ObjectTrack_pos_nodePrintf(FsPrintf,___format,##__VA_ARGS__)
#define ObjectTrack_pos_nodePrintf2(___format, ...) __ObjectTrack_pos_nodePrintf(FsPrintf2,___format,##__VA_ARGS__)

    /* 根据位置信息关联的跟踪对象 */
    struct ObjectTrack_pos_item {
        /* 1024帧数据的偏差 */
        int dx1024, dy1024;
        /* 当前的位置数量 */
        unsigned int posCount;
        /* 记录最后一次的偏差,为0表示无偏差 */
        float diff;
        /* 第一个节点 */
        struct ObjectTrack_pos_node *head;
        /* 在tail被更新时,把tail坐标写到此 */
        int x1_tailBefore, y1_tailBefore, x2_tailBefore, y2_tailBefore;
        /* 最后一个节点 */
        struct ObjectTrack_pos_node *tail;
    };

    struct ObjectTrack_pos_data {
        /* 0-表示未检测,可以再次使用 */
        int tag;
        /* 坐标 */
        int x1, y1, x2, y2;
    };


    void objectTrack_pos_data_set(struct ObjectTrack_pos_data * const pObjectTrack_pos_data, /* 坐标 */ const int x1, const int y1, const int x2, const int y2);

    /* 初始化pObjectTrack_pos_item */

    void objectTrack_pos_item_init(struct ObjectTrack_pos_item * const pObjectTrack_pos_item, /* 采集时间之gmt时间 */const double capture_gmtTime, /* 当前更新使用的帧号,只使用3个字节 */const unsigned int frameIndex
            , const int x1, const int y1, const int x2, const int y2, /* 不能小于struct ObjectTrack_pos_node的大小 */const unsigned int nodeSize);

    /* 初始化pObjectTrack_pos_item */

    void objectTrack_pos_item_init_by_pos_data(struct ObjectTrack_pos_item * const pObjectTrack_pos_item, /* 采集时间之gmt时间 */const double capture_gmtTime, /* 当前更新使用的帧号,只使用3个字节 */const unsigned int frameIndex
            , /* 使用pObjectTrack_pos_data初始化pObjectTrack_pos_node中除ObjectTrack_pos_node结构中定义的数据,可为空 */void (*const pos_node_init_extern) (struct ObjectTrack_pos_node* pObjectTrack_pos_node, struct ObjectTrack_pos_data * pObjectTrack_pos_data)
            , /* 位置信息,可以是集成的子类,tag为0才会匹配,匹配会tag会变为1 */struct ObjectTrack_pos_data * const data, /* 不能小于struct ObjectTrack_pos_node的大小 */const unsigned int nodeSize);

    /* 新建一个根据位置信息关联的跟踪对象 */

    struct ObjectTrack_pos_item *objectTrack_pos_item_new__IO(int x1, int y1, int x2, int y2, /* 采集时间之gmt时间 */const double capture_gmtTime, /* 当前更新使用的帧号,只使用3个字节 */const unsigned int frameIndex
            , /* 不能小于struct ObjectTrack_pos_node的大小 */const unsigned int nodeSize);

    /* 删除pObjectTrack_pos_item指向的实例对象 */

    void objectTrack_pos_item_delete__OI(struct ObjectTrack_pos_item * const pObjectTrack_pos_item);

    /* 自定义删除pObjectTrack_pos_item指向的实例对象 */

    void objectTrack_pos_item_delete_costom__OI(struct ObjectTrack_pos_item * const pObjectTrack_pos_item
            , /* 在ObjectTrack_pos_item被清除之前清除用户自定义数据的方法 */void (*const clean_item_costom) (struct ObjectTrack_pos_item * const pObjectTrack_pos_item));

    /* 使用新位置更新list_里的所有ObjectTrack_pos_item对象,返回被更新的对象数量,并清除list_里面最后一次更新帧号同当前帧号大于frameIndexCleanDiff的ObjectTrack_pos_item项 */

    unsigned int objectTrack_pos_update(/* 成员为ObjectTrack_pos_item及子类 */FsObjectList * const list_, /* 采集时间之gmt时间 */const double capture_gmtTime, /* 当前更新使用的帧号,只使用3个字节 */const unsigned int frameIndex
            , /* 本函数执行后会清掉帧号与本帧号差大于此值的项 */const unsigned int frameIndexCleanDiff, /* 不能小于struct ObjectTrack_pos_node的大小 */const unsigned int nodeSize
            , /* 使用pObjectTrack_pos_data初始化pObjectTrack_pos_node中除ObjectTrack_pos_node结构中定义的数据,可为空 */void (*const pos_node_init_extern) (struct ObjectTrack_pos_node* pObjectTrack_pos_node, struct ObjectTrack_pos_data * pObjectTrack_pos_data)
            , /* 在ObjectTrack_pos_item被清除之前清除用户自定义数据的方法,可为空 */void (*const clean_item_costom) (struct ObjectTrack_pos_item * const pObjectTrack_pos_item)
            , /* 位置信息,可以是集成的子类,tag为0才会匹配,匹配会tag会变为1 */struct ObjectTrack_pos_data * const data, /* ObjectTrack_pos_data的结构大小,不小于ObjectTrack_pos_data */const unsigned int dataSizeCount, /* 位置的数量 */const unsigned int posCount
            , /* 轨迹数量的最大值 */const unsigned int posCountMax, /* 匹配阈值 */const float threshold);

    /* 使用新位置更新list_里的所有ObjectTrack_pos_item对象,返回被更新的对象数量,并清除list_里面最后一次更新帧号同当前帧号大于frameIndexCleanDiff的ObjectTrack_pos_item项,使用id匹配 */

    unsigned int objectTrack_pos_update_by_id(/* 成员为ObjectTrack_pos_item及子类 */FsObjectList * const list_, /* 从ObjectTrack_pos_data对象中获取id的方法 */int (*const pos_data_get_id) (struct ObjectTrack_pos_data * pObjectTrack_pos_data)
            , /* 从ObjectTrack_pos_item对象中获取id的方法 */int (*const pos_item_get_id) (struct ObjectTrack_pos_item * pObjectTrack_pos_item)
            , /* 采集时间之gmt时间 */const double capture_gmtTime, /* 当前更新使用的帧号,只使用3个字节 */const unsigned int frameIndex
            , /* 本函数执行后会清掉帧号与本帧号差大于此值的项 */const unsigned int frameIndexCleanDiff, /* 不能小于struct ObjectTrack_pos_node的大小 */const unsigned int nodeSize
            , /* 使用pObjectTrack_pos_data初始化pObjectTrack_pos_node中除ObjectTrack_pos_node结构中定义的数据,可为空 */void (*const pos_node_init_extern) (struct ObjectTrack_pos_node* pObjectTrack_pos_node, struct ObjectTrack_pos_data * pObjectTrack_pos_data)
            , /* 在ObjectTrack_pos_item被清除之前清除用户自定义数据的方法,可为空 */void (*const clean_item_costom) (struct ObjectTrack_pos_item * pObjectTrack_pos_item)
            , /* 位置信息,可以是集成的子类,tag为0才会匹配,匹配会tag会变为1 */struct ObjectTrack_pos_data * const data, /* ObjectTrack_pos_data的结构大小,不小于ObjectTrack_pos_data */const unsigned int dataSizeCount, /* 位置的数量 */const unsigned int posCount
            , /* 轨迹数量的最大值 */const unsigned int posCountMax);

#ifdef __cplusplus
}
#endif

#endif /* OBJRCTTRACK_H */

