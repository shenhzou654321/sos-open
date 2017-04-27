/* 
 * File:   main.h
 * Author: fslib
 *
 * Created on 2013年12月17日, 下午4:20
 */

#ifndef MAIN_H
#define	MAIN_H

#ifdef	__cplusplus
extern "C" {
#endif

    /* main线程中的根节点对象,方法在main.c或main.cpp中必须实现 */
    typedef struct {

                /* 
         * 结构约定:
         *     ro中为外部只读变量(仅限当前指针所指向的结构,内部指针所指向的内容是否可读由内部结构决定);
         *     rw中为外部可读写变量(仅限当前指针所指向的结构,内部指针所指向的内容是否可读写由内部结构决定);
         *     p中为私有变量,外部不可读也不可写;
         * 变量命名约定:
         *     凡需要(非0)初始化的变量,都要加上_的前缀;
         *     凡需要内存释放的变量,都要加上__的前缀. 
         */
        struct {
            /* 多线程共有的运行状态,如果此值为空,则引用默认的全局运行状态变量,本线程的运行状态只有与此值相同才能运行;
             * 仅当设置此值为2时,系统可能会重启,其它正数系统可能退出,负数系统一定退出 
             */
            signed char *_externRunStatus;
        } ro;

        struct {
            /* 运行状态:
             *     与_externRunStatus指向值相同-运行(一般此值为1);
             *     0-停止(手动退出后的状态值);
             *     2-重启(不一定实现);
             *     大于127-引用此值的所有线程都会退出;
             * 引用的指针类型必须是(char*);
             * 引用此值的其他线程应实现当自己的运行状态值[0-255]与此值[-128-127]不相等时退出.
             */
            unsigned char runStatus;
        } rw;
    } RootStruct;
    /*
     * 创建一个RootStruct实例;
     * 返回创建的实例指针. 
     */
    RootStruct* rootStruct_new__IO();
    /* 删除pRootStruct指向的实例对象  */
    void rootStruct_delete__OI(RootStruct* pRootStruct);

#ifdef	__cplusplus
}
#endif

#endif	/* MAIN_H */

