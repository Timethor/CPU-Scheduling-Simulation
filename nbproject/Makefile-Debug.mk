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
CND_CONF=Debug
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/BurstNode.o \
	${OBJECTDIR}/BurstNodeList.o \
	${OBJECTDIR}/DeviceDescriptor.o \
	${OBJECTDIR}/DeviceDescriptorList.o \
	${OBJECTDIR}/FileReader.o \
	${OBJECTDIR}/FileWriter.o \
	${OBJECTDIR}/LineInterpreter.o \
	${OBJECTDIR}/LogEntry.o \
	${OBJECTDIR}/LogEntryList.o \
	${OBJECTDIR}/Logger.o \
	${OBJECTDIR}/ProcessControlBlock.o \
	${OBJECTDIR}/ProcessControlBlockList.o \
	${OBJECTDIR}/ProcessQueue.o \
	${OBJECTDIR}/ProcessQueueList.o \
	${OBJECTDIR}/Settings.o \
	${OBJECTDIR}/VirtualCpu.o \
	${OBJECTDIR}/main.o


# C Compiler Flags
CFLAGS=-std=c99

# CC Compiler Flags
CCFLAGS=
CXXFLAGS=

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/cpu_scheduler_simulator

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/cpu_scheduler_simulator: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${LINK.c} -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/cpu_scheduler_simulator ${OBJECTFILES} ${LDLIBSOPTIONS}

${OBJECTDIR}/BurstNode.o: BurstNode.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.c) -g -std=c99 -std=c99 -MMD -MP -MF $@.d -o ${OBJECTDIR}/BurstNode.o BurstNode.c

${OBJECTDIR}/BurstNodeList.o: BurstNodeList.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.c) -g -std=c99 -std=c99 -MMD -MP -MF $@.d -o ${OBJECTDIR}/BurstNodeList.o BurstNodeList.c

${OBJECTDIR}/DeviceDescriptor.o: DeviceDescriptor.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.c) -g -std=c99 -std=c99 -MMD -MP -MF $@.d -o ${OBJECTDIR}/DeviceDescriptor.o DeviceDescriptor.c

${OBJECTDIR}/DeviceDescriptorList.o: DeviceDescriptorList.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.c) -g -std=c99 -std=c99 -MMD -MP -MF $@.d -o ${OBJECTDIR}/DeviceDescriptorList.o DeviceDescriptorList.c

${OBJECTDIR}/FileReader.o: FileReader.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.c) -g -std=c99 -std=c99 -MMD -MP -MF $@.d -o ${OBJECTDIR}/FileReader.o FileReader.c

${OBJECTDIR}/FileWriter.o: FileWriter.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.c) -g -std=c99 -std=c99 -MMD -MP -MF $@.d -o ${OBJECTDIR}/FileWriter.o FileWriter.c

${OBJECTDIR}/LineInterpreter.o: LineInterpreter.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.c) -g -std=c99 -std=c99 -MMD -MP -MF $@.d -o ${OBJECTDIR}/LineInterpreter.o LineInterpreter.c

${OBJECTDIR}/LogEntry.o: LogEntry.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.c) -g -std=c99 -std=c99 -MMD -MP -MF $@.d -o ${OBJECTDIR}/LogEntry.o LogEntry.c

${OBJECTDIR}/LogEntryList.o: LogEntryList.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.c) -g -std=c99 -std=c99 -MMD -MP -MF $@.d -o ${OBJECTDIR}/LogEntryList.o LogEntryList.c

${OBJECTDIR}/Logger.o: Logger.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.c) -g -std=c99 -std=c99 -MMD -MP -MF $@.d -o ${OBJECTDIR}/Logger.o Logger.c

${OBJECTDIR}/ProcessControlBlock.o: ProcessControlBlock.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.c) -g -std=c99 -std=c99 -MMD -MP -MF $@.d -o ${OBJECTDIR}/ProcessControlBlock.o ProcessControlBlock.c

${OBJECTDIR}/ProcessControlBlockList.o: ProcessControlBlockList.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.c) -g -std=c99 -std=c99 -MMD -MP -MF $@.d -o ${OBJECTDIR}/ProcessControlBlockList.o ProcessControlBlockList.c

${OBJECTDIR}/ProcessQueue.o: ProcessQueue.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.c) -g -std=c99 -std=c99 -MMD -MP -MF $@.d -o ${OBJECTDIR}/ProcessQueue.o ProcessQueue.c

${OBJECTDIR}/ProcessQueueList.o: ProcessQueueList.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.c) -g -std=c99 -std=c99 -MMD -MP -MF $@.d -o ${OBJECTDIR}/ProcessQueueList.o ProcessQueueList.c

${OBJECTDIR}/Settings.o: Settings.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.c) -g -std=c99 -std=c99 -MMD -MP -MF $@.d -o ${OBJECTDIR}/Settings.o Settings.c

${OBJECTDIR}/VirtualCpu.o: VirtualCpu.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.c) -g -std=c99 -std=c99 -MMD -MP -MF $@.d -o ${OBJECTDIR}/VirtualCpu.o VirtualCpu.c

${OBJECTDIR}/main.o: main.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.c) -g -std=c99 -std=c99 -MMD -MP -MF $@.d -o ${OBJECTDIR}/main.o main.c

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}
	${RM} ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/cpu_scheduler_simulator

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
