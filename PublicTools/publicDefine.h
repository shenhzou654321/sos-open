/* 
 * File:   publicDefine.h
 * Author: fslib
 *
 * Created on 2013年1月30日, 下午3:11
 */
#ifndef PUBLICDEFINE_H
#define PUBLICDEFINE_H

////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////通用定义 开始///////////////////////////////////
#define FsErrorDump() do{*((double *)0)=0; }while(0)
#define FsDl

//#define CJAVA // 定义后表示开启java扩展功能
#define Fs_epoll_event_init0(__epollEvent)

#define FsDebug
//#define FsFdOut // 定义后打印句柄数据
//#define FsShareBuffer_debug
#define FsParamterFillType long long // 在参数可能很多时,填充参数的类型(在调用函数时传入参数比实际参数个数少,函数内部可能会访问到父级函数的栈)
#define FsParamterFillType1 FsParamterFillType
#define FsParamterFillType2 FsParamterFillType,FsParamterFillType1
#define FsParamterFillType3 FsParamterFillType,FsParamterFillType2
#define FsParamterFillType4 FsParamterFillType,FsParamterFillType3
#define FsParamterFillType5 FsParamterFillType,FsParamterFillType4
#define FsParamterFillType6 FsParamterFillType,FsParamterFillType5
#define FsParamterFillType7 FsParamterFillType,FsParamterFillType6
#define FsParamterFillType8 FsParamterFillType,FsParamterFillType7
#define FsParamterFillType9 FsParamterFillType,FsParamterFillType8
#define FsParamterFillValue 0LLU // 在参数可能很多时,填充参数的值(在调用函数时传入参数比实际参数个数少,函数内部可能会访问到父级函数的栈)
#define FsParamterFillValue1 FsParamterFillValue
#define FsParamterFillValue2 FsParamterFillValue,FsParamterFillValue1
#define FsParamterFillValue3 FsParamterFillValue,FsParamterFillValue2
#define FsParamterFillValue4 FsParamterFillValue,FsParamterFillValue3
#define FsParamterFillValue5 FsParamterFillValue,FsParamterFillValue4
#define FsParamterFillValue6 FsParamterFillValue,FsParamterFillValue5
#define FsParamterFillValue7 FsParamterFillValue,FsParamterFillValue6
#define FsParamterFillValue8 FsParamterFillValue,FsParamterFillValue7
#define FsParamterFillValue9 FsParamterFillValue,FsParamterFillValue8

/* 定义是否对函数指针检查 */
#define FsFunctionPointerCheck
/* 定义对线程是否join的检查 */
#define FsThreadJoinCheck

////////////////////////////////////////////////////////////////////////////////
#define FsStackBase 8*1024*1024 // 线程堆栈的基础大小
#define FsWaitSyncDo() usleep(1)
#define FsReconnectTime 2.0 // 重连的时间间隔
#define FsFileSizeRatio 1.01 // 文件实际占用空间与文件真实大小的比值
////////////////////////////////////////////////////////////////////////////////

/* tcp调用send函数时一次最大发送的数据量 */
#define FsTcpSendLen(___len) (___len)
#ifdef __x86_64__
#define systemThreadTid  syscall(186)
#else
/* 32位 */
#define systemThreadTid  syscall(224)
#endif


/* 内存对齐,不理解不要使用 */
/* 长度___l按___size大小对齐 */
#define Memery_AlignmentI(___l,/* 对齐块大小 1,2,4,8,16 ... */___size) (((___l)+(___size)-1)&(~((((___l)^(___l))|(___size))-1)))
#define Memery_Alignment(___l) Memery_AlignmentI((___l),8)
#define Memery_Alignment4(___l)  Memery_AlignmentI((___l),4)

///* 强制使用POLL */
//#ifndef UsePollForEpoll
//#ifndef MinGW
//#define UsePollForEpoll
//#endif
//#endif
/* 强制使用select */
//#ifdef UsePollForEpoll
//#ifndef UseSelectForEpoll
//#define UseSelectForEpoll
//#endif
//#endif
/* 检查两指针是否只有最后2bit不一样,适合内存是4字节对齐的指针判断相同,最后2bit不同可能用于标记 */
#define FsPointer_is_diff2lastBit(___p1,___p2) (((((char*)(___p1))-((char*)0))|0x3)==((((char*)(___p2))-((char*)0))|0x3))
/* 整数转void指针 */
#define FsPointer(___n) (((char*)0)+(___n))
#define FsPointer_to_integer(___p) (((char*)(___p))-((char*)0))
#define FsPointer_is_Invalid(___p) (((char*)(___p))==FsPointer(-1))
#define FsPointer_to_long(___p) ((long)(___p))
#ifdef __x86_64__
#define FsPointer_to_longlong(___p) ((long long)(___p))
#else
#define FsPointer_to_longlong(___p) ((long long)(long)(___p))
#endif

static inline void* fs_pointer_to_void_pointer(void *const p){return (void*)p;}
#define FsFun(___fun,...) ___fun(__VA_ARGS__)
#define __FsMacrosValueString(___v) #___v
#define FsMacrosValueString(___v) __FsMacrosValueString(___v)
#define __FsMacrosValue(___a) ___a
#define __FsMacrosValue2(___a,___b) __FsMacrosValue(___a##___b)
#define FsMacrosValue2(___a,___b) __FsMacrosValue2(___a,___b)
#define __FsMacrosValue3(___a,___b,___c) __FsMacrosValue(___a##___b##___c)
#define FsMacrosValue3(___a,___b,___c) __FsMacrosValue3(___a,___b,___c)
#define __FsMacrosValue4(___a,___b,___c,___d) __FsMacrosValue(___a##___b##___c##___d)
#define FsMacrosValue4(___a,___b,___c,___d) __FsMacrosValue4(___a,___b,___c,___d)
#define ____FsMacrosFunctionValue(function,macros) __FsMacrosValue(__##function##_##macros)
#define ____FsMacrosFunctionValue1(function,macros) ____FsMacrosFunctionValue(function,macros)
#define __FsMacrosFunctionValue(function,macros) ____FsMacrosFunctionValue1(function,macros)
#define FsMacrosFunction(macros) __FsMacrosFunctionValue(FsFunctionName,macros)
#define ____FsMacrosFunctionTagValue(function,macros) __FsMacrosValue(__##function##_tag_##macros)
#define __FsMacrosFunctionTagValue(function,macros) ____FsMacrosFunctionTagValue(function,macros)
#define FsMacrosFunctionTag(macros) __FsMacrosFunctionTagValue(FsFunctionName,macros)
#define ____FsMacrosFunctionTagValue2(function,tag,macros) __FsMacrosValue(__##function##_tag_##tag##_##macros)
#define __FsMacrosFunctionTagValue2(function,tag,macros) ____FsMacrosFunctionTagValue2(function,tag,macros)
#define FsMacrosFunctionTag2(tag,macros) __FsMacrosFunctionTagValue2(FsFunctionName,tag,macros)
#define FsGotoFunctionTag
/* 设置__macros宏对应的下一个状态到状态机中或直接跳转到下一个状态,__state:状态机变量,__macros:下一个状态机的宏,__next:下一个状态之后的状态 */
#ifdef FsGotoFunctionTag
#define FsMacrosSetStates_OR_GotoFunctionTag(__state,__macros,__next) __state=(__next);goto __FsMacrosFunctionTagValue(FsFunctionName,__macros)
#else
#define FsMacrosSetStates_OR_GotoFunctionTag(__state,__macros,__next) __state=__FsMacrosFunctionValue(FsFunctionName,__macros)|(__next);
#endif
/* 设置__macros宏对应的下一个状态到状态机中或直接跳转到下一个状态,__state:状态机变量,__macros:下一个状态机的宏 */
#ifdef FsGotoFunctionTag
#define FsMacrosSetState_OR_GotoFunctionTag(__state,__macros) goto __FsMacrosFunctionTagValue(FsFunctionName,__macros)
#else
#define FsMacrosSetState_OR_GotoFunctionTag(__state,__macros) __state=__FsMacrosFunctionValue(FsFunctionName,__macros)
#endif
#define __FsMacrosString(...) #__VA_ARGS__
#define FsMacrosString(...) __FsMacrosString(__VA_ARGS__)
/* 判断无符号整型变量un是否已达到最大值 */
#define FsNumberMax(___un) ((~((___un)^(___un)))^((~((___un)^(___un)))<<sizeof(___un)<<sizeof(___un)<<sizeof(___un)<<sizeof(___un)<<sizeof(___un)<<sizeof(___un)<<sizeof(___un)<<sizeof(___un)))
#define FsNumberIsMax(___un) ((___un)==FsNumberMax(___un))
/* 计算___b 到 ___a的差值,___max为最大值,最大值+1应等于0 */
#define FsNumberDiff(___a,___b,___max) ((___a) > (___b) ? ((___a) - (___b)) : ((___max) - ((___b) - (___a)) + 1))
/* 如max比value小,则把value的值赋给max,最后都会把value置为defaltValue */
#define FsNumber_GetMax_add_setDefault(max,value,defaltValue) if((max)<(value))(max)=(value);(value)=(defaltValue);
/* 计算两个循环值的差,如FsNubmer_cycle_diff(65535,0,65536)=1,FsNubmer_cycle_diff(0,65535,65536)=65535 */
#define FsNubmer_cycle_diff(___a,___b,/* 当值为___max表示其值为0 */___max) ((___b)>(___a)?(___b)-(___a):(___max)-((___a)-(___b)))
/* 临时宏变量 */
#define FsTmp1
#define FsTmp2
#define FsTmp3
#define FsFunctionName 
//_____________________________________________________________________________//
/* 计算可变参数的个数 */
#define __FsArgc_n( _0,_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14,_15,_16,_17,_18,_19,_20,_21,_22,_23,_24,_25,_26,_27,_28,_29,_30,_31,___N,...) ___N
#define __FsArgc_cnt( ... ) __FsArgc_n( __VA_ARGS__ )
#define __FsArgc_seqs() 31,30,29,28,27,26,25,24,23,22,21,20,19,18,17,16,15,14,13,12,11,10,9,8,7,6,5,4,3,2,1,0
#define __FsArgc(...) __FsArgc_cnt( 0, ##__VA_ARGS__, __FsArgc_seqs())
#define FsArgc(...) __FsArgc(__VA_ARGS__)
#define __FsArgs0___0_0(...) ___0
/* 如果输入的参数个数为0会输出___0参数 */
#define FsArgs0___0(...) FsMacrosValue2(__FsArgs0___0,FsArgc(##__VA_ARGS__))(__VA_ARGS__)
//_____________________________________________________________________________//
/* 定义清理某函数中申请的变量空间,倒序删除 */
#if 1
#define __FsCleanValue(___m,___index) __FsMacrosValue(___m##_##___index)
#define __FsClean_0(___m)
#define __FsClean_1(___m)  __FsCleanValue(___m,1)
#define __FsClean_2(___m)  __FsCleanValue(___m,2 ) __FsClean_1 (___m)
#define __FsClean_3(___m)  __FsCleanValue(___m,3 ) __FsClean_2 (___m)
#define __FsClean_4(___m)  __FsCleanValue(___m,4 ) __FsClean_3 (___m)
#define __FsClean_5(___m)  __FsCleanValue(___m,5 ) __FsClean_4 (___m)
#define __FsClean_6(___m)  __FsCleanValue(___m,6 ) __FsClean_5 (___m)
#define __FsClean_7(___m)  __FsCleanValue(___m,7 ) __FsClean_6 (___m)
#define __FsClean_8(___m)  __FsCleanValue(___m,8 ) __FsClean_7 (___m)
#define __FsClean_9(___m)  __FsCleanValue(___m,9 ) __FsClean_8 (___m)
#define __FsClean_10(___m) __FsCleanValue(___m,10) __FsClean_9 (___m)
#define __FsClean_11(___m) __FsCleanValue(___m,11) __FsClean_10(___m)
#define __FsClean_12(___m) __FsCleanValue(___m,12) __FsClean_11(___m)
#define __FsClean_13(___m) __FsCleanValue(___m,13) __FsClean_12(___m)
#define __FsClean_14(___m) __FsCleanValue(___m,14) __FsClean_13(___m)
#define __FsClean_15(___m) __FsCleanValue(___m,15) __FsClean_14(___m)
#define __FsClean_16(___m) __FsCleanValue(___m,16) __FsClean_15(___m)
#define __FsClean_17(___m) __FsCleanValue(___m,17) __FsClean_16(___m)
#define __FsClean_18(___m) __FsCleanValue(___m,18) __FsClean_17(___m)
#define __FsClean_19(___m) __FsCleanValue(___m,19) __FsClean_18(___m)
#define __FsClean_20(___m) __FsCleanValue(___m,20) __FsClean_19(___m)
#define __FsClean_21(___m) __FsCleanValue(___m,21) __FsClean_20(___m)
#define __FsClean_22(___m) __FsCleanValue(___m,22) __FsClean_21(___m)
#define __FsClean_23(___m) __FsCleanValue(___m,23) __FsClean_22(___m)
#define __FsClean_24(___m) __FsCleanValue(___m,24) __FsClean_23(___m)
#define __FsClean_25(___m) __FsCleanValue(___m,25) __FsClean_24(___m)
#define __FsClean_26(___m) __FsCleanValue(___m,26) __FsClean_25(___m)
#define __FsClean_27(___m) __FsCleanValue(___m,27) __FsClean_26(___m)
#define __FsClean_28(___m) __FsCleanValue(___m,28) __FsClean_27(___m)
#define __FsClean_29(___m) __FsCleanValue(___m,29) __FsClean_28(___m)
#define __FsClean_30(___m) __FsCleanValue(___m,30) __FsClean_29(___m)
#define __FsClean_31(___m) __FsCleanValue(___m,31) __FsClean_30(___m)
#define __FsClean_32(___m) __FsCleanValue(___m,32) __FsClean_31(___m)
#define __FsClean5(___m,___count) __FsClean##_##___count(___m)
#define __FsClean4(___m,___count) __FsClean5(___m,___count)
#define __FsClean3(___m) __FsClean4(___m,___m##_count)
#define __FsClean2(___function,___group) __FsClean3(__##___function##_clean##___group)
#define __FsClean1(___function,___group) __FsClean2(___function,___group)
#endif
/* 清理某函数中申请的变量空间,倒序删除 */
#define FsClean(/* 清除的组号 */___group) __FsClean1(FsFunctionName,___group)
//_____________________________________________________________________________//
#if 1
#define __FsCompare_char_do0(___s1,___s2) (((___s1))[0]==((___s2))[0])
#define __FsCompare_char_doOther(___s1,___s2) ((___s1)[0]>(___s2)[0]?1:-1)
#define __FsCompare_char_equal_do0(___s1,___s2) (((___s1))[0]==((___s2))[0])
#define __FsCompare_char_equal_doOther(___s1,___s2) 1
#define __FsCompareCaseInsensitive_do0(___s1,___s2) ((___s1)[0]==(___s2)[0]||(((___s1)[0]|0x20)==((___s2)[0]|0x20)&&((___s1)[0]|0x20)>='a'&&((___s1)[0]|0x20)<='z'))
#define __FsCompareCaseInsensitive_doOther(___s1,___s2) (((___s1)[0]>='a'&&(___s1)[0]<='z')?(((___s1)[1]>='a'&&(___s1)[1]<='z')?((___s1)[0]>(___s2)[0]?1:-1):(((___s1)[0]^0x20)>(___s2)[0]?1:-1))\
:(((___s1)[1]>='a'&&(___s1)[1]<='z')?((___s1)[0]>((___s2)[0]^0x2)?1:-1):((___s1)[0]>(___s2)[0]?1:-1)))
#define __FsCompareCaseInsensitive_equal_do0(___s1,___s2) __FsCompareCaseInsensitive_do0(___s1,___s2)
#define __FsCompareCaseInsensitive_equal_doOther(___s1,___s2) 1

#define __FsCompare_0(___s1,___s2,___FsCompare_do)  0
#define __FsCompare_1(___s1,___s2,___FsCompare_do)  (__FsCompare##___FsCompare_do##_do0(___s1,___s2)?__FsCompare_0((___s1)+1,(___s2)+1,___FsCompare_do):__FsCompare##___FsCompare_do##_doOther(___s1,___s2))
#define __FsCompare_2(___s1,___s2,___FsCompare_do)  (__FsCompare##___FsCompare_do##_do0(___s1,___s2)?__FsCompare_1((___s1)+1,(___s2)+1,___FsCompare_do):__FsCompare##___FsCompare_do##_doOther(___s1,___s2))
#define __FsCompare_3(___s1,___s2,___FsCompare_do)  (__FsCompare##___FsCompare_do##_do0(___s1,___s2)?__FsCompare_2((___s1)+1,(___s2)+1,___FsCompare_do):__FsCompare##___FsCompare_do##_doOther(___s1,___s2))
#define __FsCompare_4(___s1,___s2,___FsCompare_do)  (__FsCompare##___FsCompare_do##_do0(___s1,___s2)?__FsCompare_3((___s1)+1,(___s2)+1,___FsCompare_do):__FsCompare##___FsCompare_do##_doOther(___s1,___s2))
#define __FsCompare_5(___s1,___s2,___FsCompare_do)  (__FsCompare##___FsCompare_do##_do0(___s1,___s2)?__FsCompare_4((___s1)+1,(___s2)+1,___FsCompare_do):__FsCompare##___FsCompare_do##_doOther(___s1,___s2))
#define __FsCompare_6(___s1,___s2,___FsCompare_do)  (__FsCompare##___FsCompare_do##_do0(___s1,___s2)?__FsCompare_5((___s1)+1,(___s2)+1,___FsCompare_do):__FsCompare##___FsCompare_do##_doOther(___s1,___s2))
#define __FsCompare_7(___s1,___s2,___FsCompare_do)  (__FsCompare##___FsCompare_do##_do0(___s1,___s2)?__FsCompare_6((___s1)+1,(___s2)+1,___FsCompare_do):__FsCompare##___FsCompare_do##_doOther(___s1,___s2))
#define __FsCompare_8(___s1,___s2,___FsCompare_do)  (__FsCompare##___FsCompare_do##_do0(___s1,___s2)?__FsCompare_7((___s1)+1,(___s2)+1,___FsCompare_do):__FsCompare##___FsCompare_do##_doOther(___s1,___s2))
#define __FsCompare_9(___s1,___s2,___FsCompare_do)  (__FsCompare##___FsCompare_do##_do0(___s1,___s2)?__FsCompare_8((___s1)+1,(___s2)+1,___FsCompare_do):__FsCompare##___FsCompare_do##_doOther(___s1,___s2))
#define __FsCompare_10(___s1,___s2,___FsCompare_do) (__FsCompare##___FsCompare_do##_do0(___s1,___s2)?__FsCompare_9((___s1)+1,(___s2)+1,___FsCompare_do):__FsCompare##___FsCompare_do##_doOther(___s1,___s2))
#define __FsCompare_11(___s1,___s2,___FsCompare_do) (__FsCompare##___FsCompare_do##_do0(___s1,___s2)?__FsCompare_10((___s1)+1,(___s2)+1,___FsCompare_do):__FsCompare##___FsCompare_do##_doOther(___s1,___s2))
#define __FsCompare_12(___s1,___s2,___FsCompare_do) (__FsCompare##___FsCompare_do##_do0(___s1,___s2)?__FsCompare_11((___s1)+1,(___s2)+1,___FsCompare_do):__FsCompare##___FsCompare_do##_doOther(___s1,___s2))
#define __FsCompare_13(___s1,___s2,___FsCompare_do) (__FsCompare##___FsCompare_do##_do0(___s1,___s2)?__FsCompare_12((___s1)+1,(___s2)+1,___FsCompare_do):__FsCompare##___FsCompare_do##_doOther(___s1,___s2))
#define __FsCompare_14(___s1,___s2,___FsCompare_do) (__FsCompare##___FsCompare_do##_do0(___s1,___s2)?__FsCompare_13((___s1)+1,(___s2)+1,___FsCompare_do):__FsCompare##___FsCompare_do##_doOther(___s1,___s2))
#define __FsCompare_15(___s1,___s2,___FsCompare_do) (__FsCompare##___FsCompare_do##_do0(___s1,___s2)?__FsCompare_14((___s1)+1,(___s2)+1,___FsCompare_do):__FsCompare##___FsCompare_do##_doOther(___s1,___s2))
#define __FsCompare_16(___s1,___s2,___FsCompare_do) (__FsCompare##___FsCompare_do##_do0(___s1,___s2)?__FsCompare_15((___s1)+1,(___s2)+1,___FsCompare_do):__FsCompare##___FsCompare_do##_doOther(___s1,___s2))
#define __FsCompare_17(___s1,___s2,___FsCompare_do) (__FsCompare##___FsCompare_do##_do0(___s1,___s2)?__FsCompare_16((___s1)+1,(___s2)+1,___FsCompare_do):__FsCompare##___FsCompare_do##_doOther(___s1,___s2))
#define __FsCompare_18(___s1,___s2,___FsCompare_do) (__FsCompare##___FsCompare_do##_do0(___s1,___s2)?__FsCompare_17((___s1)+1,(___s2)+1,___FsCompare_do):__FsCompare##___FsCompare_do##_doOther(___s1,___s2))
#define __FsCompare_19(___s1,___s2,___FsCompare_do) (__FsCompare##___FsCompare_do##_do0(___s1,___s2)?__FsCompare_18((___s1)+1,(___s2)+1,___FsCompare_do):__FsCompare##___FsCompare_do##_doOther(___s1,___s2))
#define __FsCompare_20(___s1,___s2,___FsCompare_do) (__FsCompare##___FsCompare_do##_do0(___s1,___s2)?__FsCompare_19((___s1)+1,(___s2)+1,___FsCompare_do):__FsCompare##___FsCompare_do##_doOther(___s1,___s2))
#define __FsCompare_21(___s1,___s2,___FsCompare_do) (__FsCompare##___FsCompare_do##_do0(___s1,___s2)?__FsCompare_20((___s1)+1,(___s2)+1,___FsCompare_do):__FsCompare##___FsCompare_do##_doOther(___s1,___s2))
#define __FsCompare_22(___s1,___s2,___FsCompare_do) (__FsCompare##___FsCompare_do##_do0(___s1,___s2)?__FsCompare_21((___s1)+1,(___s2)+1,___FsCompare_do):__FsCompare##___FsCompare_do##_doOther(___s1,___s2))
#define __FsCompare_23(___s1,___s2,___FsCompare_do) (__FsCompare##___FsCompare_do##_do0(___s1,___s2)?__FsCompare_22((___s1)+1,(___s2)+1,___FsCompare_do):__FsCompare##___FsCompare_do##_doOther(___s1,___s2))
#define __FsCompare_24(___s1,___s2,___FsCompare_do) (__FsCompare##___FsCompare_do##_do0(___s1,___s2)?__FsCompare_23((___s1)+1,(___s2)+1,___FsCompare_do):__FsCompare##___FsCompare_do##_doOther(___s1,___s2))
#define __FsCompare_25(___s1,___s2,___FsCompare_do) (__FsCompare##___FsCompare_do##_do0(___s1,___s2)?__FsCompare_24((___s1)+1,(___s2)+1,___FsCompare_do):__FsCompare##___FsCompare_do##_doOther(___s1,___s2))
#define __FsCompare_26(___s1,___s2,___FsCompare_do) (__FsCompare##___FsCompare_do##_do0(___s1,___s2)?__FsCompare_25((___s1)+1,(___s2)+1,___FsCompare_do):__FsCompare##___FsCompare_do##_doOther(___s1,___s2))
#define __FsCompare_27(___s1,___s2,___FsCompare_do) (__FsCompare##___FsCompare_do##_do0(___s1,___s2)?__FsCompare_26((___s1)+1,(___s2)+1,___FsCompare_do):__FsCompare##___FsCompare_do##_doOther(___s1,___s2))
#define __FsCompare_28(___s1,___s2,___FsCompare_do) (__FsCompare##___FsCompare_do##_do0(___s1,___s2)?__FsCompare_27((___s1)+1,(___s2)+1,___FsCompare_do):__FsCompare##___FsCompare_do##_doOther(___s1,___s2))
#define __FsCompare_29(___s1,___s2,___FsCompare_do) (__FsCompare##___FsCompare_do##_do0(___s1,___s2)?__FsCompare_28((___s1)+1,(___s2)+1,___FsCompare_do):__FsCompare##___FsCompare_do##_doOther(___s1,___s2))
#define __FsCompare_30(___s1,___s2,___FsCompare_do) (__FsCompare##___FsCompare_do##_do0(___s1,___s2)?__FsCompare_29((___s1)+1,(___s2)+1,___FsCompare_do):__FsCompare##___FsCompare_do##_doOther(___s1,___s2))
#define __FsCompare_31(___s1,___s2,___FsCompare_do) (__FsCompare##___FsCompare_do##_do0(___s1,___s2)?__FsCompare_30((___s1)+1,(___s2)+1,___FsCompare_do):__FsCompare##___FsCompare_do##_doOther(___s1,___s2))
#define __FsCompare_32(___s1,___s2,___FsCompare_do) (__FsCompare##___FsCompare_do##_do0(___s1,___s2)?__FsCompare_31((___s1)+1,(___s2)+1,___FsCompare_do):__FsCompare##___FsCompare_do##_doOther(___s1,___s2))

#endif
#if 1

#endif
/* 定义按字节比较,___s1>___s2返回1,___s1=___s2返回0,___s1<___s2返回-1 */
#define FsCompare_char(___s1,___s2,/* 长度必须是编译前的定值数字,如1 */___len) __FsCompare_##___len((unsigned char*)___s1,(unsigned char*)___s2,_char)
/* 定义按字节比较,___s1=___s2返回1,否则返回0 */
#define FsCompare_char_equal(___s1,___s2,/* 长度必须是编译前的定值数字,如1 */___len) (__FsCompare_##___len((unsigned char*)___s1,(unsigned char*)___s2,_char_equal)==0)
/* 定义按字节比较(大小写不敏感,转大写匹配),___s1>___s2返回1,___s1=___s2返回0,___s1<___s2返回-1 */
#define FsCompareCaseInsensitive(___s1,___s2,/* 长度必须是编译前的定值数字,如1 */___len) __FsCompare_##___len((unsigned char*)___s1,(unsigned char*)___s2,CaseInsensitive)
/* 定义按字节比较(大小写不敏感),___s1=___s2返回1,否则返回0 */
#define FsCompareCaseInsensitive_equal(___s1,___s2,/* 长度必须是编译前的定值数字,如1 */___len) (__FsCompare_##___len((unsigned char*)___s1,(unsigned char*)___s2,CaseInsensitive_equal)==0)
//_____________________________________________________________________________//
#ifdef __cplusplus
extern "C" {
#endif
    unsigned int __get_printfTime();

#ifndef MinGW1

#define FsPrintfData(__format, ...) printf("\33[31m" __format"\33[0m",##__VA_ARGS__) 
#define FsPrintf(index,__format, ...) printf("\33[36m%x\33[33m%05d\33[36m/\33[35m%010u\33[36m|\33[35m%d\33[36m/\33[35m%d\33[36m(\33[33m%s\33[36m)\33[35m--\33[31m" __format"\33[0m",index,__LINE__,__get_printfTime(),getpid(),getppid(),__FUNCTION__, ##__VA_ARGS__) 
    //#define FsPrintf(index,__format, ...) printf("\33[36m%x\33[33m%05d\33[36m/\33[35m%010u\33[36m|\33[35m%d\33[36m/\33[35m%d\33[36m(\33[33m%s\33[35m->\33[33m%s\33[36m)\33[35m--\33[31m" __format"\33[0m",index,__LINE__,__get_printfTime(),getpid(),getppid(),__FILE__,__FUNCTION__, ##__VA_ARGS__) 
#else
#define FsPrintfData(__format, ...) printf(__format,##__VA_ARGS__) 
#define FsPrintf(index,__format, ...) printf("%x%05d/%010u|%d/%d(%s)--" __format,index,__LINE__,__get_printfTime(),getpid(),0,__FUNCTION__, ##__VA_ARGS__) 
#endif
#ifndef MinGW1
#define FsPrintf2Data(__format, ...) printf("\33[32m" __format"\33[0m",##__VA_ARGS__) 
#define FsPrintf2(index,__format, ...) printf("\33[37m%x\33[36m%05d\33[33m/\33[35m%010u\33[33m|\33[35m%d\33[33m/\33[35m%d\33[33m[\33[36m%s\33[33m]\33[35m--\33[32m" __format"\33[0m",index,__LINE__,__get_printfTime(),getpid(),getppid(),__FUNCTION__, ##__VA_ARGS__)
    //#define FsPrintf2(index,__format, ...) printf("\33[37m%x\33[36m%05d\33[33m/\33[35m%010u\33[33m|\33[35m%d\33[33m/\33[35m%d\33[33m[\33[36m%s\33[35m->\33[36m%s\33[33m]\33[35m--\33[32m" __format"\33[0m",index,__LINE__,__get_printfTime(),getpid(),getppid(),__FILE__,__FUNCTION__, ##__VA_ARGS__)
#else
#define FsPrintf2Data(__format, ...) printf(__format,##__VA_ARGS__) 
#define FsPrintf2(index,__format, ...) printf("%x%05d/%010u|%d/%d[%s]--" __format,index,__LINE__,__get_printfTime(),getpid(),0,__FUNCTION__, ##__VA_ARGS__)
#endif
#define FsPrintfTag "\33[36m%05d\33[33m/\33[35m%010u\33[0m"
#define FsPrintfOut __LINE__,__get_printfTime() 
#ifdef FsDebug
#define FsPrintfOut2 __LINE__,__get_printfTime() 
#else
#define FsPrintfOut2 0,0U
#endif       
#define FsSnprintf(/* FsLocal_ShareBuffer已使用的空间大小 */___pos,___format, ...) do{\
    int n=snprintf(FsLocal_ShareBuffer+(___pos), FsLocal_ShareBufferLen-(___pos), ___format, ##__VA_ARGS__);\
    if(n+(___pos)>=(int)FsLocal_ShareBufferLen){\
        FsLocal_ShareBuffer_resize(n+(___pos)+1);\
        n=snprintf(FsLocal_ShareBuffer+(___pos), FsLocal_ShareBufferLen-(___pos), ___format, ##__VA_ARGS__);\
    }\
    ___pos+=n;\
}while(0)
#define Fs_num_min(___a,___b) ((___a)<(___b)?(___a):(___b))
#define Fs_num_max(___a,___b) ((___a)>(___b)?(___a):(___b))
#define Fs_num_abs(___v) ((___v)>=0?(___v):-(___v))
#define Fs_num_grade_change(/* 结果储存变量 */___rst,/* 转换后的精度 */___grade_dst,/* 转换后的最小刻度间隔 */___step_dst,/* 原始值 */___v,/* 原始精度 */___grade_src) do{\
    (___rst)=((1.0*(___grade_dst)/(___grade_src)*(___v)+0.5*(___step_dst))/(___step_dst));\
    (___rst)*=(___step_dst);\
    if((___rst)>(___grade_dst))(___rst)=(___grade_dst);\
}while(0)
    /* 年转为字符串的最大值,函数应实现轮圈机制,即在判断a<b时,要在b小于临界值或a大于临界值时才成立 */
#define Fs_date_year_string_max "9999"
    /* 1970-1-1同1900-1-1间隔的秒数 */
#define Fs_date_JAN_1970 2208988800U
#define closesocket close
#define WSAGetLastError() errno

#define Fs_PI 3.14159265358979323846

    /* 内存友好型数组,这仅的一个概念,须各函数自己实现,要求在对象用完后能只用一次free就能释放所有空间 */
    typedef struct {
        /* 数组的个数 */
        unsigned int count;
        /* 指针数组,可储存任意类型的变量,要求array[i]指向的对象内存连续 */
        char *data[];
    } FsArray;

    typedef struct {
        /* 引用计数,为0时删除 */
        unsigned int count;
    } FsRefer;
#define Fs_Refer_addRefer(pRefer) ((FsRefer*)(pRefer))->count++;
#define Fs_Refer_delete(pRefer) do{\
    if(1==((FsRefer*)(pRefer))->count--)fsFree(((void*)(pRefer)));\
}while(0)
#define Fs_Array_len(array) (sizeof(array)/sizeof(array[0]))
    ///////////////////////////通用定义 结束/////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////
    ///////////////////////////定义全局变量开始//////////////////////////////////
    /* 如果线程没有引用其它线程的状态,则都引用此值,当自己的状态与此值不同时应退出线程,引用的指针类型必须是(signed char*) */
    extern signed char defaultRunStatus;
    /* 日志级别,按位排布,参考FsLogType定义,最大值表示未加到日志级别管理模块中 */
    extern unsigned int loglevel;
    ///////////////////////////定义全局变量结束//////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////
    ///////////////////////////公共变量的内存操作 开始///////////////////////////
#ifdef FsThreadJoinCheck
#include <pthread.h>
    /* 线程安全地添加线程创建后有无使用pthread_join回收线程资源的检查 */
    void thread_join_add_check_pthreadSafety(/* 线程名 */const char threadName[], /* 后缀,加到threadName后组成新的threadName,可为空 */const char suffix[]);
    /* 线程安全地移除线程创建后有无使用pthread_join回收线程资源的检查,应在pthread_join后调用 */
    void thread_join_remove_check_pthreadSafety(/* pthread_join的线程号 */const pthread_t tid);
#else 
#define thread_join_add_check_pthreadSafety(threadName,suffix)
#define thread_join_remove_check_pthreadSafety(tid)
#endif
    /* 
     * 公共变量的内存操作,及标准输出和错误的重定向;
     * 主要用完成一些在程序中要用到的函数所对应的表的初始化,主要与优化相关;
     * 必须在main函数开始处中调用. 
     */
    void globalInit(/* 标准输入重定向,为空表示不重定向 */ const char stdinFile[], /* 标准输出重定向,为空表示不重定向 */ const char stdoutFile[], /* 标准错误重定向,为空表示不重定向 */ const char stderrFile[]
            , /* 重定向的模式,redirectMode&0x1不为0表示只在标准输入无效时才重定向;redirectMode&0x2不为0表示只在标准输出无效时才重定向;redirectMode&0x4不为0表示只在标准错误无效时才重定向 */const unsigned char redirectMode);
    /* 
     * 释放已申请的公共变量;
     * 主要用完成一些在程序中要用到的函数所对应的表的内存释放,主要与优化相关;
     * 必须在main函数结束处中调用. 
     */
    void globalRelease();
    ////////////////////////////////////////////////////////////////////////////
    /* 调整pEpollEvent的大小,在epoll_wait后调用 */
#ifdef FsDebug
#define fsEpollEvent_updteSize(___pEpollEvent,/* 储存当前pEpollEvent数量的变量 */___pEpollEventCount,/* ___pEpollEventCount变化的参考变量,float类型 */___pEpollEventCountBase\
    ,/* epoll_wait的返回值,必须保证此值大于0 */___rv_epoll_wait) do{\
   if((___rv_epoll_wait)<=0){\
       FsLog(FsLogType_fatal,FsPrintfIndex,"Invalid rv(=%d).\n",___rv_epoll_wait);\
       fflush(stdout);\
       FsLogExit();\
   }\
   if(___pEpollEventCount<(___rv_epoll_wait)){\
       ___pEpollEventCountBase+=((___rv_epoll_wait)+1.0-___pEpollEventCountBase)/2.0;\
       if(___pEpollEventCount<___pEpollEventCountBase){\
           ___pEpollEventCount=(___pEpollEventCountBase+(___rv_epoll_wait))/2.0;\
           ___pEpollEvent=(struct epoll_event *)fsRealloc(___pEpollEvent,sizeof (struct epoll_event)* ___pEpollEventCount);\
       }\
   }else{\
       ___pEpollEventCountBase+=((___rv_epoll_wait)-___pEpollEventCountBase)/8.0;\
       if(___pEpollEventCount/2>___pEpollEventCountBase+4){\
           ___pEpollEventCount=Fs_num_max(___pEpollEventCountBase,4);\
           ___pEpollEvent=(struct epoll_event *)fsRealloc(___pEpollEvent,sizeof (struct epoll_event)* ___pEpollEventCount);\
       }\
   }\
}while(0)
#else
#define fsEpollEvent_updteSize(___pEpollEvent,/* 储存当前pEpollEvent数量的变量 */___pEpollEventCount,/* ___pEpollEventCount变化的参考变量,float类型 */___pEpollEventCountBase\
    ,/* epoll_wait的返回值,必须保证此值大于0 */___rv_epoll_wait) do{\
   if(___pEpollEventCount<(___rv_epoll_wait)){\
       ___pEpollEventCountBase+=((___rv_epoll_wait)+1.0-___pEpollEventCountBase)/2.0;\
       if(___pEpollEventCount<___pEpollEventCountBase){\
           ___pEpollEventCount=(___pEpollEventCountBase+(___rv_epoll_wait))/2.0;\
           ___pEpollEvent=(struct epoll_event *)fsRealloc(___pEpollEvent,sizeof (struct epoll_event)* ___pEpollEventCount);\
       }\
   }else{\
       ___pEpollEventCountBase+=((___rv_epoll_wait)-___pEpollEventCountBase)/8.0;\
       if(___pEpollEventCount/2>___pEpollEventCountBase+4){\
           ___pEpollEventCount=Fs_num_max(___pEpollEventCountBase,4);\
           ___pEpollEvent=(struct epoll_event *)fsRealloc(___pEpollEvent,sizeof (struct epoll_event)* ___pEpollEventCount);\
       }\
   }\
}while(0)
#endif
#define FsFileError_try_agin() (EINTR == errno)
#define FsSockError_try_again() (EINTR == errno || EWOULDBLOCK == errno || EAGAIN == errno)
#define FsSockError_waitconnect() (EINPROGRESS == errno || EWOULDBLOCK == errno)


#include <sys/epoll.h>
#define epoll_delete(__epfd) close(__epfd)


    /* 对象的删除方法
     * 作为其他结构的成员变量使用,且必须是第一个成员变量或第三个成员变量,为第三个成员变量时,第一个成员变量和第二个成员变量必须为指针变量且第一个成员变量指针值不能为奇数
     * 作为其他结构的成员变量时变量储存的值为实际值+1一个字节(即为奇数,以基数标注此指针是FsDelete类型的变量)
     */
    typedef struct {
        /* 对象p在主线程的删除方法,且需要互斥的对象已被锁住,一般指p对象与外界无关联关系,直接删除即可,外部delete方法中应调用此函数 */
        void (*delete_in_mainThread_locked)(/* 对象指针 */void *p, /* 是否已移除,0-未移除,1-已移除 */char hasRemoved
                , /* pEpollEvent的个数 */unsigned int epollEventCount, /* 不为空时表示需要查找对应的socket是否在pEpollEvent中,在要从pEpollEvent中清除 */struct epoll_event *pEpollEvent
                , /* epollfd,小于0表示不从epollfd中删除 */ long long epollfd, /* 自定义用户数据 */void *pUser);
        /* 对象p在工作线程的删除方法,且需要互斥的对象已被锁住,一般指p对象与外界无关联关系,直接删除即可 */
        void (*delete_in_workThread_locked)(/* 对象指针 */void *p, /* 是否已移除,0-未移除,1-已移除 */char hasRemoved
                , /* pEpollEvent的个数 */unsigned int epollEventCount, /* 不为空时表示需要查找对应的socket是否在pEpollEvent中,在要从pEpollEvent中清除 */struct epoll_event *pEpollEvent
                , /* epollfd,小于0表示不从epollfd中删除 */ long long epollfd, /* 自定义用户数据 */void *pUser);
        /* 对象p在主线程的删除方法 */
        void (*delete_in_mainThread)(/* 对象指针 */void *p
                , /* pEpollEvent的个数 */unsigned int epollEventCount, /* 不为空时表示需要查找对应的socket是否在pEpollEvent中,在要从pEpollEvent中清除 */struct epoll_event *pEpollEvent
                , /* epollfd,小于0表示不从epollfd中删除 */ long long epollfd, /* 自定义用户数据 */void *pUser);
        /* 对象p在工作线程的删除方法 */
        void (*delete_in_workThread)(/* 对象指针 */void *p
                , /* pEpollEvent的个数 */unsigned int epollEventCount, /* 不为空时表示需要查找对应的socket是否在pEpollEvent中,在要从pEpollEvent中清除 */struct epoll_event *pEpollEvent
                , /* epollfd,小于0表示不从epollfd中删除 */ long long epollfd, /* 自定义用户数据 */void *pUser);
    } FsDelete;
    /* 获取__p对象中的FsDelete指针 */
#define Fs_Delete_get(___p) ((FsDelete*)((*(((char**)(___p))+((((char*)(___p))[0]&0x1)?0:2)))-1)) //((FsDelete*)(((char**)(___p))[(((char*)(___p))[0]&0x1)?0:2]-1))
    /* 设置__pFsDelete的值 */
#define Fs_Delete_set(___pFsDelete,___v) (___pFsDelete)=((FsDelete*)(((char*)(___v))+1))
    ///////////////////////////公共变量的内存操作 结束////////////////////////////
    ////////////////////////////////////////////////////////////////////////////   

#ifdef __cplusplus
}

#endif
/* 标准业务返回值
 * -101:客户端在控制视频播放速度
 * -102:重定向
 * -103:丢包率过高导致出错
 * -104:需要关闭(如http指定为短连接)
 * -105:表示数据被内部引用(如h264解码时,传入的数据帧可能不会立即输出)
 * -120:表示繁忙
 */
#endif /* PUBLICDEFINE_H */