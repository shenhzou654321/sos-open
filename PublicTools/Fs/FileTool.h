/* 
 * File:   FileTool.h
 * Author: fslib
 *
 * Created on 2013年1月9日, 上午9:25
 */

#ifndef FILETOOL_H
#define	FILETOOL_H
#include "../publicDefine.h"
#include "StringList.h"


#ifdef	__cplusplus
extern "C" {
#endif
    /* 创建目录dir中的每一级目录 */
    void fs_dir_make(/* 要创建的目录名,只有/以前的才会被创建 */const char dir[], /* mode:创建目录的访问权限，默认0777 */unsigned int mode /* = 0777 */);
   
#ifdef	__cplusplus
}
#endif

#endif	/* FILETOOL_H */

