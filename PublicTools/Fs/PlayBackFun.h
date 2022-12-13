/* 
 * File:   PlayBackFun.h
 * Author: fslib
 *
 * Created on 2013年2月22日, 下午5:03
 */
#if !defined PLAYBACKFUN_H && !defined JAVA_IN_C

#define PLAYBACKFUN_H
#include "../../PublicTools/publicDefine.h"
#include "../../PublicTools/Fs/StructList.h"


#ifdef __cplusplus
extern "C" {
#endif

#pragma pack(push,1)

#pragma pack(pop)
       /*
     * 录像检查采用分段检查机制,从当前时间向前检查,检查分三类
     *     A模式-向后检查(更新最新数据):最长时间不超过一个时间周期
     *     B模式-向前检查(更新历史数据和巡查已缓存的数据区是否有数据删除):最长时间不超过一个时间周期
     *     C模式-recordVideoList中第一条数据检查(检查数据是否有被删除):最长时间不超过一个时间周期
     * 初始化为A模式,在recordVideoList中无数据时,运行为A->B->A模式,在recordVideoList中有数据时,运行为A->B->A->C模式
     * B模式和C模式都可能增加或者删除数据
     * C模式的存在一个最小值,在巡查一轮后,如没有数据被删除则最小值减去当前时间与最小值的一个差值再减去一个分段时间,在有数据被删除时把最小值限定到recordVideoList中的第一条数据(在发现删除时立即修改)
     * 
     */

    /* 
     * 合并recordVideoListBuffer_中记录的数据.
     */

    void fs_playBackFun_do_combine_recordVideoListBuffer(FsStructList * const recordVideoListBuffer_);

    /* 
     * 在[startTime,endTime]时间范围内合并recordVideoListBuffer_中记录的数据到recordVideoList_中,最后清空recordVideoListBuffer_中的数据;
     * 返回0表示无数据更新;
     * 返回&0x1表示有非最后一条数据向后增加的更新(设置0x4时也会设置0x1);
     * 返回&0x2表示是最后一条数据向后增加的更新(视频在向后走);
     * 返回&0x4表示有数据删除.
     */

    int fs_playBackFun_do_combine_recordVideoList_recordVideoListBuffer(/* 节点数不为0 */FsStructList * const recordVideoList_, /* 节点数不为0 */FsStructList * const recordVideoListBuffer_, const double startTime, const double endTime);

    /* 
     * 清除recordVideoList_中[startTime,endTime]时间范围的数据;
     * 返回0表示无数据更新;
     * 返回&0x1表示有非最后一条数据向后增加的更新(设置0x4时也会设置0x1);
     * 返回&0x2表示是最后一条数据向后增加的更新(视频在向后走);
     * 返回&0x4表示有数据删除.
     */

    int fs_playBackFun_do_clean_recordVideoList(/* 节点数不为0 */FsStructList * const recordVideoList_, const double startTime, const double endTime);



#ifdef __cplusplus
}
#endif

#endif /* PLAYBACKFUN_H */

