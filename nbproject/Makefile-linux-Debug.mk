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
CND_PLATFORM=GNU-Linux
CND_DLIB_EXT=so
CND_CONF=linux-Debug
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
	${OBJECTDIR}/Main/vsys/inflrayObject.o \
	${OBJECTDIR}/Main/vsys/output-ebml.o \
	${OBJECTDIR}/Main/vsys/output-xml-comment.o \
	${OBJECTDIR}/Main/vsys/recognition.o \
	${OBJECTDIR}/Main/vsys/record.o \
	${OBJECTDIR}/Main/vsys/targetCheck.o \
	${OBJECTDIR}/Main/vsys/vdomelinkage.o \
	${OBJECTDIR}/Main/vsys/vehicleDetect.o \
	${OBJECTDIR}/Main/vsys/vgunlinkage.o \
	${OBJECTDIR}/Main/vsys/vsys.o \
	${OBJECTDIR}/Main/vsys/vtrack.o \
	${OBJECTDIR}/PublicTools/Fs/CycleList.o


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
LDLIBSOPTIONS=-lHCAlarm -lHCCore -lHCCoreDevCfg -lHCDisplay -lHCGeneralCfgMgr -lHCIndustry -lHCPlayBack -lHCPreview -lHCVoiceTalk -lStreamTransClient -lSystemTransform -lanalyzedata -lhcnetsdk -lhpr libzos.a

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ../dist/linux/zos

../dist/linux/zos: libzos.a

../dist/linux/zos: ${OBJECTFILES}
	${MKDIR} -p ../dist/linux
	${LINK.c} -o ../dist/linux/zos ${OBJECTFILES} ${LDLIBSOPTIONS} -lpthread -ldl -lrt

${OBJECTDIR}/Main/main.o: Main/main.c
	${MKDIR} -p ${OBJECTDIR}/Main
	${RM} "$@.d"
	$(COMPILE.c) -g -Wall -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Main/main.o Main/main.c

${OBJECTDIR}/Main/vsys/capture.o: Main/vsys/capture.c
	${MKDIR} -p ${OBJECTDIR}/Main/vsys
	${RM} "$@.d"
	$(COMPILE.c) -g -Wall -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Main/vsys/capture.o Main/vsys/capture.c

${OBJECTDIR}/Main/vsys/decode.o: Main/vsys/decode.c
	${MKDIR} -p ${OBJECTDIR}/Main/vsys
	${RM} "$@.d"
	$(COMPILE.c) -g -Wall -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Main/vsys/decode.o Main/vsys/decode.c

${OBJECTDIR}/Main/vsys/encode.o: Main/vsys/encode.c
	${MKDIR} -p ${OBJECTDIR}/Main/vsys
	${RM} "$@.d"
	$(COMPILE.c) -g -Wall -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Main/vsys/encode.o Main/vsys/encode.c

${OBJECTDIR}/Main/vsys/inflrayObject.o: Main/vsys/inflrayObject.c
	${MKDIR} -p ${OBJECTDIR}/Main/vsys
	${RM} "$@.d"
	$(COMPILE.c) -g -Wall -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Main/vsys/inflrayObject.o Main/vsys/inflrayObject.c

${OBJECTDIR}/Main/vsys/output-ebml.o: Main/vsys/output-ebml.c
	${MKDIR} -p ${OBJECTDIR}/Main/vsys
	${RM} "$@.d"
	$(COMPILE.c) -g -Wall -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Main/vsys/output-ebml.o Main/vsys/output-ebml.c

${OBJECTDIR}/Main/vsys/output-xml-comment.o: Main/vsys/output-xml-comment.c
	${MKDIR} -p ${OBJECTDIR}/Main/vsys
	${RM} "$@.d"
	$(COMPILE.c) -g -Wall -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Main/vsys/output-xml-comment.o Main/vsys/output-xml-comment.c

${OBJECTDIR}/Main/vsys/recognition.o: Main/vsys/recognition.c
	${MKDIR} -p ${OBJECTDIR}/Main/vsys
	${RM} "$@.d"
	$(COMPILE.c) -g -Wall -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Main/vsys/recognition.o Main/vsys/recognition.c

${OBJECTDIR}/Main/vsys/record.o: Main/vsys/record.c
	${MKDIR} -p ${OBJECTDIR}/Main/vsys
	${RM} "$@.d"
	$(COMPILE.c) -g -Wall -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Main/vsys/record.o Main/vsys/record.c

${OBJECTDIR}/Main/vsys/targetCheck.o: Main/vsys/targetCheck.c
	${MKDIR} -p ${OBJECTDIR}/Main/vsys
	${RM} "$@.d"
	$(COMPILE.c) -g -Wall -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Main/vsys/targetCheck.o Main/vsys/targetCheck.c

${OBJECTDIR}/Main/vsys/vdomelinkage.o: Main/vsys/vdomelinkage.c
	${MKDIR} -p ${OBJECTDIR}/Main/vsys
	${RM} "$@.d"
	$(COMPILE.c) -g -Wall -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Main/vsys/vdomelinkage.o Main/vsys/vdomelinkage.c

${OBJECTDIR}/Main/vsys/vehicleDetect.o: Main/vsys/vehicleDetect.c
	${MKDIR} -p ${OBJECTDIR}/Main/vsys
	${RM} "$@.d"
	$(COMPILE.c) -g -Wall -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Main/vsys/vehicleDetect.o Main/vsys/vehicleDetect.c

${OBJECTDIR}/Main/vsys/vgunlinkage.o: Main/vsys/vgunlinkage.c
	${MKDIR} -p ${OBJECTDIR}/Main/vsys
	${RM} "$@.d"
	$(COMPILE.c) -g -Wall -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Main/vsys/vgunlinkage.o Main/vsys/vgunlinkage.c

${OBJECTDIR}/Main/vsys/vsys.o: Main/vsys/vsys.c
	${MKDIR} -p ${OBJECTDIR}/Main/vsys
	${RM} "$@.d"
	$(COMPILE.c) -g -Wall -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Main/vsys/vsys.o Main/vsys/vsys.c

${OBJECTDIR}/Main/vsys/vtrack.o: Main/vsys/vtrack.c
	${MKDIR} -p ${OBJECTDIR}/Main/vsys
	${RM} "$@.d"
	$(COMPILE.c) -g -Wall -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Main/vsys/vtrack.o Main/vsys/vtrack.c

${OBJECTDIR}/PublicTools/Fs/CycleList.o: PublicTools/Fs/CycleList.c
	${MKDIR} -p ${OBJECTDIR}/PublicTools/Fs
	${RM} "$@.d"
	$(COMPILE.c) -g -Wall -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/PublicTools/Fs/CycleList.o PublicTools/Fs/CycleList.c

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
