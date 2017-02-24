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
CND_CONF=Release
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/_ext/56252444/main.o


# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=-m64
CXXFLAGS=-m64

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=-L..//dist/Release/GNU-Linux-x86 -L/home/build/finjin-3rd-party/cpp/boost/stage/lib -L/home/build/finjin-3rd-party/cpp/curl/lib -Wl,--whole-archive ../../../library/project/linux-netbeans/dist/Release/GNU-Linux/libfinjin-common.a -Wl,--no-whole-archive -lpthread -lboost_system -lboost_filesystem -lboost_thread -lboost_regex -lboost_chrono -lboost_timer -lboost_locale -lboost_program_options -lboost_log -lcurl -lrt -ldl

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ./bin/linux64-gcc-release/finjin-common-test

./bin/linux64-gcc-release/finjin-common-test: ../../../library/project/linux-netbeans/dist/Release/GNU-Linux/libfinjin-common.a

./bin/linux64-gcc-release/finjin-common-test: ${OBJECTFILES}
	${MKDIR} -p ./bin/linux64-gcc-release
	${LINK.cc} -o ./bin/linux64-gcc-release/finjin-common-test ${OBJECTFILES} ${LDLIBSOPTIONS}

${OBJECTDIR}/_ext/56252444/main.o: ../../src/main.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/56252444
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DFINJIN_COMMON_STATIC_LIBRARY -I../../../../../finjin-common/cpp/library/include -I../../../library/include -I../../../library/src -I/home/build/finjin-3rd-party/cpp/boost -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/56252444/main.o ../../src/main.cpp

# Subprojects
.build-subprojects:
	cd ../../../library/project/linux-netbeans && ${MAKE}  -f Makefile CONF=Release

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}
	${RM} ./bin/linux64-gcc-release/finjin-common-test

# Subprojects
.clean-subprojects:
	cd ../../../library/project/linux-netbeans && ${MAKE}  -f Makefile CONF=Release clean

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
