#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Environment
MKDIR=mkdir
CP=cp
GREP=grep
NM=nm
CCADMIN=CCadmin
RANLIB=ranlib
CC=gcc
CCC=g++
CXX=g++
FC=gfortran
AS=as

# Macros
CND_PLATFORM=GNU-Linux-x86
CND_DLIB_EXT=so
CND_CONF=linux-Release
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/Main/main.o \
	${OBJECTDIR}/Main/vsys/capture.o \
	${OBJECTDIR}/Main/vsys/decode.o \
	${OBJECTDIR}/Main/vsys/encode.o \
	${OBJECTDIR}/Main/vsys/record.o \
	${OBJECTDIR}/Main/vsys/vsys.o \
	${OBJECTDIR}/PublicTools/libFunction.o


# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=
CXXFLAGS=

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=-Llib libfslib.so Modules/camera_ctrl/camera_ctrl_hk_http.o Modules/camera_rtmp/camera_rtmp.o

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ../dist/linux/sos

../dist/linux/sos: libfslib.so

../dist/linux/sos: Modules/camera_ctrl/camera_ctrl_hk_http.o

../dist/linux/sos: Modules/camera_rtmp/camera_rtmp.o

../dist/linux/sos: ${OBJECTFILES}
	${MKDIR} -p ../dist/linux
	${LINK.cc} -o ../dist/linux/sos ${OBJECTFILES} ${LDLIBSOPTIONS} -lpthread -ldl -lrt

${OBJECTDIR}/Main/main.o: Main/main.c 
	${MKDIR} -p ${OBJECTDIR}/Main
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Main/main.o Main/main.c

${OBJECTDIR}/Main/vsys/capture.o: Main/vsys/capture.c 
	${MKDIR} -p ${OBJECTDIR}/Main/vsys
	${RM} "$@.d"
	$(COMPILE.cc) -g -O2 -w -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Main/vsys/capture.o Main/vsys/capture.c

${OBJECTDIR}/Main/vsys/decode.o: Main/vsys/decode.c 
	${MKDIR} -p ${OBJECTDIR}/Main/vsys
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Main/vsys/decode.o Main/vsys/decode.c

${OBJECTDIR}/Main/vsys/encode.o: Main/vsys/encode.c 
	${MKDIR} -p ${OBJECTDIR}/Main/vsys
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Main/vsys/encode.o Main/vsys/encode.c

${OBJECTDIR}/Main/vsys/record.o: Main/vsys/record.c 
	${MKDIR} -p ${OBJECTDIR}/Main/vsys
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Main/vsys/record.o Main/vsys/record.c

${OBJECTDIR}/Main/vsys/vsys.o: Main/vsys/vsys.c 
	${MKDIR} -p ${OBJECTDIR}/Main/vsys
	${RM} "$@.d"
	$(COMPILE.cc) -g -O2 -w -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Main/vsys/vsys.o Main/vsys/vsys.c

${OBJECTDIR}/PublicTools/libFunction.o: PublicTools/libFunction.c 
	${MKDIR} -p ${OBJECTDIR}/PublicTools
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/PublicTools/libFunction.o PublicTools/libFunction.c

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}
	${RM} ../dist/linux/sos

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
