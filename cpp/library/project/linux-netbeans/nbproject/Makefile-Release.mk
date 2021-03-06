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
	${OBJECTDIR}/_ext/56252444/AllocatedClass.o \
	${OBJECTDIR}/_ext/56252444/Allocator.o \
	${OBJECTDIR}/_ext/56252444/Angle.o \
	${OBJECTDIR}/_ext/56252444/Base64.o \
	${OBJECTDIR}/_ext/56252444/BinaryDataChunkReader.o \
	${OBJECTDIR}/_ext/56252444/BinaryDataChunkWriter.o \
	${OBJECTDIR}/_ext/56252444/ByteBuffer.o \
	${OBJECTDIR}/_ext/56252444/Chrono.o \
	${OBJECTDIR}/_ext/56252444/ChunkName.o \
	${OBJECTDIR}/_ext/56252444/CommandLineArgsProcessor.o \
	${OBJECTDIR}/_ext/56252444/CommandLineSettingsDescription.o \
	${OBJECTDIR}/_ext/56252444/ConfigDataChunkReader.o \
	${OBJECTDIR}/_ext/56252444/ConfigDataChunkWriter.o \
	${OBJECTDIR}/_ext/56252444/ConfigDocumentReader.o \
	${OBJECTDIR}/_ext/56252444/ConfigDocumentWriter.o \
	${OBJECTDIR}/_ext/56252444/Convert.o \
	${OBJECTDIR}/_ext/56252444/CpuFeatures.o \
	${OBJECTDIR}/_ext/56252444/DataChunkReader.o \
	${OBJECTDIR}/_ext/56252444/DataChunkReaderCallbacks.o \
	${OBJECTDIR}/_ext/56252444/DataChunkReaderController.o \
	${OBJECTDIR}/_ext/56252444/DataChunkWriter.o \
	${OBJECTDIR}/_ext/56252444/DataChunkWriterController.o \
	${OBJECTDIR}/_ext/56252444/DataElementWriter.o \
	${OBJECTDIR}/_ext/56252444/DebugLog.o \
	${OBJECTDIR}/_ext/56252444/DirEntFileFinder.o \
	${OBJECTDIR}/_ext/56252444/DirectoryVirtualFileSystemRoot.o \
	${OBJECTDIR}/_ext/56252444/Distance.o \
	${OBJECTDIR}/_ext/56252444/DomainInformation.o \
	${OBJECTDIR}/_ext/56252444/Error.o \
	${OBJECTDIR}/_ext/56252444/FiberCondition.o \
	${OBJECTDIR}/_ext/56252444/FiberJobQueue.o \
	${OBJECTDIR}/_ext/56252444/FiberJobScheduler.o \
	${OBJECTDIR}/_ext/56252444/FiberMutex.o \
	${OBJECTDIR}/_ext/56252444/FiberSchedulingAlgorithm.o \
	${OBJECTDIR}/_ext/56252444/FiberSpinLock.o \
	${OBJECTDIR}/_ext/56252444/FileAccessor.o \
	${OBJECTDIR}/_ext/56252444/FileSystemEntry.o \
	${OBJECTDIR}/_ext/56252444/FinjinPrecompiled.o \
	${OBJECTDIR}/_ext/56252444/GeneralAllocator.o \
	${OBJECTDIR}/_ext/56252444/Hash.o \
	${OBJECTDIR}/_ext/56252444/JPEGReader.o \
	${OBJECTDIR}/_ext/56252444/JPEGWriter.o \
	${OBJECTDIR}/_ext/56252444/JobFiber.o \
	${OBJECTDIR}/_ext/56252444/JobFuture.o \
	${OBJECTDIR}/_ext/56252444/JobSystem.o \
	${OBJECTDIR}/_ext/56252444/JobThread.o \
	${OBJECTDIR}/_ext/56252444/JsonDataChunkReader.o \
	${OBJECTDIR}/_ext/56252444/JsonDataChunkWriter.o \
	${OBJECTDIR}/_ext/56252444/JsonDataDocumentWriter.o \
	${OBJECTDIR}/_ext/56252444/JsonDataElementWriter.o \
	${OBJECTDIR}/_ext/56252444/JsonDocument.o \
	${OBJECTDIR}/_ext/56252444/LibraryLoader.o \
	${OBJECTDIR}/_ext/56252444/LinuxUtilities.o \
	${OBJECTDIR}/_ext/56252444/LogLevel.o \
	${OBJECTDIR}/_ext/56252444/LogicalCpu_Linux.o \
	${OBJECTDIR}/_ext/56252444/Luid.o \
	${OBJECTDIR}/_ext/56252444/Math.o \
	${OBJECTDIR}/_ext/56252444/MemoryArchitecture.o \
	${OBJECTDIR}/_ext/56252444/MemoryArena.o \
	${OBJECTDIR}/_ext/56252444/MemorySize.o \
	${OBJECTDIR}/_ext/56252444/NamePrefix.o \
	${OBJECTDIR}/_ext/56252444/PNGReader.o \
	${OBJECTDIR}/_ext/56252444/PNGWriter.o \
	${OBJECTDIR}/_ext/56252444/Path.o \
	${OBJECTDIR}/_ext/56252444/PerformanceCounter.o \
	${OBJECTDIR}/_ext/56252444/ProcessRuntimeInformation.o \
	${OBJECTDIR}/_ext/56252444/Settings.o \
	${OBJECTDIR}/_ext/56252444/SingleEvent.o \
	${OBJECTDIR}/_ext/56252444/StandardPaths.o \
	${OBJECTDIR}/_ext/56252444/StandardPaths_Linux.o \
	${OBJECTDIR}/_ext/56252444/StreamingFileFormat.o \
	${OBJECTDIR}/_ext/56252444/StringMatcher.o \
	${OBJECTDIR}/_ext/56252444/SystemAllocator.o \
	${OBJECTDIR}/_ext/56252444/TextDataChunkReader.o \
	${OBJECTDIR}/_ext/56252444/TextDataChunkWriter.o \
	${OBJECTDIR}/_ext/56252444/ThisThread.o \
	${OBJECTDIR}/_ext/56252444/Thread.o \
	${OBJECTDIR}/_ext/56252444/TypeDescription.o \
	${OBJECTDIR}/_ext/56252444/Uri.o \
	${OBJECTDIR}/_ext/56252444/UsageFormatter.o \
	${OBJECTDIR}/_ext/56252444/UserInformation.o \
	${OBJECTDIR}/_ext/56252444/Utf8String.o \
	${OBJECTDIR}/_ext/56252444/Utf8StringFormatter.o \
	${OBJECTDIR}/_ext/56252444/Uuid.o \
	${OBJECTDIR}/_ext/56252444/Version.o \
	${OBJECTDIR}/_ext/56252444/VirtualFileHandle.o \
	${OBJECTDIR}/_ext/56252444/VirtualFileOperationHandle.o \
	${OBJECTDIR}/_ext/56252444/VirtualFileSystem.o \
	${OBJECTDIR}/_ext/56252444/VirtualFileSystemOperationQueue.o \
	${OBJECTDIR}/_ext/56252444/VirtualFileSystemRoot.o \
	${OBJECTDIR}/_ext/56252444/WrappedFileReader.o \
	${OBJECTDIR}/_ext/56252444/WrappedFileWriter.o \
	${OBJECTDIR}/_ext/56252444/XmlDataDocumentWriter.o \
	${OBJECTDIR}/_ext/56252444/XmlDataElementWriter.o \
	${OBJECTDIR}/_ext/56252444/XmlDocument.o \
	${OBJECTDIR}/_ext/56252444/ZIPReader.o \
	${OBJECTDIR}/_ext/56252444/ZipArchiveVirtualFileSystemRoot.o \
	${OBJECTDIR}/_ext/b0297d5f/jump_x86_64_sysv_elf_gas.o \
	${OBJECTDIR}/_ext/b0297d5f/make_x86_64_sysv_elf_gas.o \
	${OBJECTDIR}/_ext/f9f80fec/jaricom.o \
	${OBJECTDIR}/_ext/f9f80fec/jcapimin.o \
	${OBJECTDIR}/_ext/f9f80fec/jcapistd.o \
	${OBJECTDIR}/_ext/f9f80fec/jcarith.o \
	${OBJECTDIR}/_ext/f9f80fec/jccoefct.o \
	${OBJECTDIR}/_ext/f9f80fec/jccolor.o \
	${OBJECTDIR}/_ext/f9f80fec/jcdctmgr.o \
	${OBJECTDIR}/_ext/f9f80fec/jchuff.o \
	${OBJECTDIR}/_ext/f9f80fec/jcinit.o \
	${OBJECTDIR}/_ext/f9f80fec/jcmainct.o \
	${OBJECTDIR}/_ext/f9f80fec/jcmarker.o \
	${OBJECTDIR}/_ext/f9f80fec/jcmaster.o \
	${OBJECTDIR}/_ext/f9f80fec/jcomapi.o \
	${OBJECTDIR}/_ext/f9f80fec/jcparam.o \
	${OBJECTDIR}/_ext/f9f80fec/jcprepct.o \
	${OBJECTDIR}/_ext/f9f80fec/jcsample.o \
	${OBJECTDIR}/_ext/f9f80fec/jctrans.o \
	${OBJECTDIR}/_ext/f9f80fec/jdapimin.o \
	${OBJECTDIR}/_ext/f9f80fec/jdapistd.o \
	${OBJECTDIR}/_ext/f9f80fec/jdarith.o \
	${OBJECTDIR}/_ext/f9f80fec/jdatadst.o \
	${OBJECTDIR}/_ext/f9f80fec/jdatasrc.o \
	${OBJECTDIR}/_ext/f9f80fec/jdcoefct.o \
	${OBJECTDIR}/_ext/f9f80fec/jdcolor.o \
	${OBJECTDIR}/_ext/f9f80fec/jddctmgr.o \
	${OBJECTDIR}/_ext/f9f80fec/jdhuff.o \
	${OBJECTDIR}/_ext/f9f80fec/jdinput.o \
	${OBJECTDIR}/_ext/f9f80fec/jdmainct.o \
	${OBJECTDIR}/_ext/f9f80fec/jdmarker.o \
	${OBJECTDIR}/_ext/f9f80fec/jdmaster.o \
	${OBJECTDIR}/_ext/f9f80fec/jdmerge.o \
	${OBJECTDIR}/_ext/f9f80fec/jdpostct.o \
	${OBJECTDIR}/_ext/f9f80fec/jdsample.o \
	${OBJECTDIR}/_ext/f9f80fec/jdtrans.o \
	${OBJECTDIR}/_ext/f9f80fec/jerror.o \
	${OBJECTDIR}/_ext/f9f80fec/jfdctflt.o \
	${OBJECTDIR}/_ext/f9f80fec/jfdctfst.o \
	${OBJECTDIR}/_ext/f9f80fec/jfdctint.o \
	${OBJECTDIR}/_ext/f9f80fec/jidctflt.o \
	${OBJECTDIR}/_ext/f9f80fec/jidctfst.o \
	${OBJECTDIR}/_ext/f9f80fec/jidctint.o \
	${OBJECTDIR}/_ext/f9f80fec/jmemmgr.o \
	${OBJECTDIR}/_ext/f9f80fec/jmemnobs.o \
	${OBJECTDIR}/_ext/f9f80fec/jquant1.o \
	${OBJECTDIR}/_ext/f9f80fec/jquant2.o \
	${OBJECTDIR}/_ext/f9f80fec/jutils.o \
	${OBJECTDIR}/_ext/f9f80fec/rdbmp.o \
	${OBJECTDIR}/_ext/f9f80fec/rdcolmap.o \
	${OBJECTDIR}/_ext/f9f80fec/rdgif.o \
	${OBJECTDIR}/_ext/f9f80fec/rdppm.o \
	${OBJECTDIR}/_ext/f9f80fec/rdrle.o \
	${OBJECTDIR}/_ext/f9f80fec/rdswitch.o \
	${OBJECTDIR}/_ext/f9f80fec/rdtarga.o \
	${OBJECTDIR}/_ext/f9f80fec/transupp.o \
	${OBJECTDIR}/_ext/f9f80fec/wrbmp.o \
	${OBJECTDIR}/_ext/f9f80fec/wrgif.o \
	${OBJECTDIR}/_ext/f9f80fec/wrppm.o \
	${OBJECTDIR}/_ext/f9f80fec/wrrle.o \
	${OBJECTDIR}/_ext/f9f80fec/wrtarga.o \
	${OBJECTDIR}/_ext/f9f8f9c1/png.o \
	${OBJECTDIR}/_ext/f9f8f9c1/pngerror.o \
	${OBJECTDIR}/_ext/f9f8f9c1/pngget.o \
	${OBJECTDIR}/_ext/f9f8f9c1/pngmem.o \
	${OBJECTDIR}/_ext/f9f8f9c1/pngpread.o \
	${OBJECTDIR}/_ext/f9f8f9c1/pngread.o \
	${OBJECTDIR}/_ext/f9f8f9c1/pngrio.o \
	${OBJECTDIR}/_ext/f9f8f9c1/pngrtran.o \
	${OBJECTDIR}/_ext/f9f8f9c1/pngrutil.o \
	${OBJECTDIR}/_ext/f9f8f9c1/pngset.o \
	${OBJECTDIR}/_ext/f9f8f9c1/pngtrans.o \
	${OBJECTDIR}/_ext/f9f8f9c1/pngwio.o \
	${OBJECTDIR}/_ext/f9f8f9c1/pngwrite.o \
	${OBJECTDIR}/_ext/f9f8f9c1/pngwtran.o \
	${OBJECTDIR}/_ext/f9f8f9c1/pngwutil.o \
	${OBJECTDIR}/_ext/a859e9a1/iostream.o \
	${OBJECTDIR}/_ext/2fda0f13/tinyxml2.o \
	${OBJECTDIR}/_ext/5de1235c/UriCommon.o \
	${OBJECTDIR}/_ext/5de1235c/UriCompare.o \
	${OBJECTDIR}/_ext/5de1235c/UriEscape.o \
	${OBJECTDIR}/_ext/5de1235c/UriFile.o \
	${OBJECTDIR}/_ext/5de1235c/UriIp4.o \
	${OBJECTDIR}/_ext/5de1235c/UriIp4Base.o \
	${OBJECTDIR}/_ext/5de1235c/UriNormalize.o \
	${OBJECTDIR}/_ext/5de1235c/UriNormalizeBase.o \
	${OBJECTDIR}/_ext/5de1235c/UriParse.o \
	${OBJECTDIR}/_ext/5de1235c/UriParseBase.o \
	${OBJECTDIR}/_ext/5de1235c/UriQuery.o \
	${OBJECTDIR}/_ext/5de1235c/UriRecompose.o \
	${OBJECTDIR}/_ext/5de1235c/UriResolve.o \
	${OBJECTDIR}/_ext/5de1235c/UriShorten.o \
	${OBJECTDIR}/_ext/747b22b2/xxhash.o \
	${OBJECTDIR}/_ext/f9ff474f/adler32.o \
	${OBJECTDIR}/_ext/f9ff474f/compress.o \
	${OBJECTDIR}/_ext/f9ff474f/crc32.o \
	${OBJECTDIR}/_ext/f9ff474f/deflate.o \
	${OBJECTDIR}/_ext/f9ff474f/gzclose.o \
	${OBJECTDIR}/_ext/f9ff474f/gzlib.o \
	${OBJECTDIR}/_ext/f9ff474f/gzread.o \
	${OBJECTDIR}/_ext/f9ff474f/gzwrite.o \
	${OBJECTDIR}/_ext/f9ff474f/infback.o \
	${OBJECTDIR}/_ext/f9ff474f/inffast.o \
	${OBJECTDIR}/_ext/f9ff474f/inflate.o \
	${OBJECTDIR}/_ext/f9ff474f/inftrees.o \
	${OBJECTDIR}/_ext/f9ff474f/trees.o \
	${OBJECTDIR}/_ext/f9ff474f/uncompr.o \
	${OBJECTDIR}/_ext/f9ff474f/zutil.o \
	${OBJECTDIR}/_ext/b69e269c/dir.o \
	${OBJECTDIR}/_ext/b69e269c/err.o \
	${OBJECTDIR}/_ext/b69e269c/fetch.o \
	${OBJECTDIR}/_ext/b69e269c/file.o \
	${OBJECTDIR}/_ext/b69e269c/fseeko.o \
	${OBJECTDIR}/_ext/b69e269c/info.o \
	${OBJECTDIR}/_ext/b69e269c/memdisk.o \
	${OBJECTDIR}/_ext/b69e269c/mmapped.o \
	${OBJECTDIR}/_ext/b69e269c/plugin.o \
	${OBJECTDIR}/_ext/b69e269c/stat.o \
	${OBJECTDIR}/_ext/b69e269c/write.o \
	${OBJECTDIR}/_ext/b69e269c/zip.o


# C Compiler Flags
CFLAGS=-m64

# CC Compiler Flags
CCFLAGS=-m64
CXXFLAGS=-m64

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libfinjin-common.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libfinjin-common.a: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${RM} ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libfinjin-common.a
	${AR} -rv ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libfinjin-common.a ${OBJECTFILES} 
	$(RANLIB) ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libfinjin-common.a

${OBJECTDIR}/_ext/56252444/AllocatedClass.o: ../../src/AllocatedClass.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/56252444
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -D_ZZIP_NO_INLINE -D_zzip_inline -I. -I../../include -I../../src -I/home/build/finjin-3rd-party/cpp/ -I/home/build/finjin-3rd-party/cpp/nowide_standalone -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/zziplib -I/home/build/finjin-3rd-party/cpp/zlib -I/home/build/finjin-3rd-party/cpp/lpng -I/home/build/finjin-3rd-party/cpp/uriparser/include -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/56252444/AllocatedClass.o ../../src/AllocatedClass.cpp

${OBJECTDIR}/_ext/56252444/Allocator.o: ../../src/Allocator.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/56252444
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -D_ZZIP_NO_INLINE -D_zzip_inline -I. -I../../include -I../../src -I/home/build/finjin-3rd-party/cpp/ -I/home/build/finjin-3rd-party/cpp/nowide_standalone -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/zziplib -I/home/build/finjin-3rd-party/cpp/zlib -I/home/build/finjin-3rd-party/cpp/lpng -I/home/build/finjin-3rd-party/cpp/uriparser/include -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/56252444/Allocator.o ../../src/Allocator.cpp

${OBJECTDIR}/_ext/56252444/Angle.o: ../../src/Angle.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/56252444
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -D_ZZIP_NO_INLINE -D_zzip_inline -I. -I../../include -I../../src -I/home/build/finjin-3rd-party/cpp/ -I/home/build/finjin-3rd-party/cpp/nowide_standalone -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/zziplib -I/home/build/finjin-3rd-party/cpp/zlib -I/home/build/finjin-3rd-party/cpp/lpng -I/home/build/finjin-3rd-party/cpp/uriparser/include -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/56252444/Angle.o ../../src/Angle.cpp

${OBJECTDIR}/_ext/56252444/Base64.o: ../../src/Base64.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/56252444
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -D_ZZIP_NO_INLINE -D_zzip_inline -I. -I../../include -I../../src -I/home/build/finjin-3rd-party/cpp/ -I/home/build/finjin-3rd-party/cpp/nowide_standalone -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/zziplib -I/home/build/finjin-3rd-party/cpp/zlib -I/home/build/finjin-3rd-party/cpp/lpng -I/home/build/finjin-3rd-party/cpp/uriparser/include -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/56252444/Base64.o ../../src/Base64.cpp

${OBJECTDIR}/_ext/56252444/BinaryDataChunkReader.o: ../../src/BinaryDataChunkReader.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/56252444
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -D_ZZIP_NO_INLINE -D_zzip_inline -I. -I../../include -I../../src -I/home/build/finjin-3rd-party/cpp/ -I/home/build/finjin-3rd-party/cpp/nowide_standalone -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/zziplib -I/home/build/finjin-3rd-party/cpp/zlib -I/home/build/finjin-3rd-party/cpp/lpng -I/home/build/finjin-3rd-party/cpp/uriparser/include -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/56252444/BinaryDataChunkReader.o ../../src/BinaryDataChunkReader.cpp

${OBJECTDIR}/_ext/56252444/BinaryDataChunkWriter.o: ../../src/BinaryDataChunkWriter.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/56252444
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -D_ZZIP_NO_INLINE -D_zzip_inline -I. -I../../include -I../../src -I/home/build/finjin-3rd-party/cpp/ -I/home/build/finjin-3rd-party/cpp/nowide_standalone -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/zziplib -I/home/build/finjin-3rd-party/cpp/zlib -I/home/build/finjin-3rd-party/cpp/lpng -I/home/build/finjin-3rd-party/cpp/uriparser/include -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/56252444/BinaryDataChunkWriter.o ../../src/BinaryDataChunkWriter.cpp

${OBJECTDIR}/_ext/56252444/ByteBuffer.o: ../../src/ByteBuffer.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/56252444
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -D_ZZIP_NO_INLINE -D_zzip_inline -I. -I../../include -I../../src -I/home/build/finjin-3rd-party/cpp/ -I/home/build/finjin-3rd-party/cpp/nowide_standalone -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/zziplib -I/home/build/finjin-3rd-party/cpp/zlib -I/home/build/finjin-3rd-party/cpp/lpng -I/home/build/finjin-3rd-party/cpp/uriparser/include -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/56252444/ByteBuffer.o ../../src/ByteBuffer.cpp

${OBJECTDIR}/_ext/56252444/Chrono.o: ../../src/Chrono.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/56252444
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -D_ZZIP_NO_INLINE -D_zzip_inline -I. -I../../include -I../../src -I/home/build/finjin-3rd-party/cpp/ -I/home/build/finjin-3rd-party/cpp/nowide_standalone -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/zziplib -I/home/build/finjin-3rd-party/cpp/zlib -I/home/build/finjin-3rd-party/cpp/lpng -I/home/build/finjin-3rd-party/cpp/uriparser/include -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/56252444/Chrono.o ../../src/Chrono.cpp

${OBJECTDIR}/_ext/56252444/ChunkName.o: ../../src/ChunkName.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/56252444
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -D_ZZIP_NO_INLINE -D_zzip_inline -I. -I../../include -I../../src -I/home/build/finjin-3rd-party/cpp/ -I/home/build/finjin-3rd-party/cpp/nowide_standalone -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/zziplib -I/home/build/finjin-3rd-party/cpp/zlib -I/home/build/finjin-3rd-party/cpp/lpng -I/home/build/finjin-3rd-party/cpp/uriparser/include -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/56252444/ChunkName.o ../../src/ChunkName.cpp

${OBJECTDIR}/_ext/56252444/CommandLineArgsProcessor.o: ../../src/CommandLineArgsProcessor.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/56252444
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -D_ZZIP_NO_INLINE -D_zzip_inline -I. -I../../include -I../../src -I/home/build/finjin-3rd-party/cpp/ -I/home/build/finjin-3rd-party/cpp/nowide_standalone -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/zziplib -I/home/build/finjin-3rd-party/cpp/zlib -I/home/build/finjin-3rd-party/cpp/lpng -I/home/build/finjin-3rd-party/cpp/uriparser/include -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/56252444/CommandLineArgsProcessor.o ../../src/CommandLineArgsProcessor.cpp

${OBJECTDIR}/_ext/56252444/CommandLineSettingsDescription.o: ../../src/CommandLineSettingsDescription.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/56252444
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -D_ZZIP_NO_INLINE -D_zzip_inline -I. -I../../include -I../../src -I/home/build/finjin-3rd-party/cpp/ -I/home/build/finjin-3rd-party/cpp/nowide_standalone -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/zziplib -I/home/build/finjin-3rd-party/cpp/zlib -I/home/build/finjin-3rd-party/cpp/lpng -I/home/build/finjin-3rd-party/cpp/uriparser/include -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/56252444/CommandLineSettingsDescription.o ../../src/CommandLineSettingsDescription.cpp

${OBJECTDIR}/_ext/56252444/ConfigDataChunkReader.o: ../../src/ConfigDataChunkReader.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/56252444
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -D_ZZIP_NO_INLINE -D_zzip_inline -I. -I../../include -I../../src -I/home/build/finjin-3rd-party/cpp/ -I/home/build/finjin-3rd-party/cpp/nowide_standalone -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/zziplib -I/home/build/finjin-3rd-party/cpp/zlib -I/home/build/finjin-3rd-party/cpp/lpng -I/home/build/finjin-3rd-party/cpp/uriparser/include -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/56252444/ConfigDataChunkReader.o ../../src/ConfigDataChunkReader.cpp

${OBJECTDIR}/_ext/56252444/ConfigDataChunkWriter.o: ../../src/ConfigDataChunkWriter.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/56252444
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -D_ZZIP_NO_INLINE -D_zzip_inline -I. -I../../include -I../../src -I/home/build/finjin-3rd-party/cpp/ -I/home/build/finjin-3rd-party/cpp/nowide_standalone -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/zziplib -I/home/build/finjin-3rd-party/cpp/zlib -I/home/build/finjin-3rd-party/cpp/lpng -I/home/build/finjin-3rd-party/cpp/uriparser/include -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/56252444/ConfigDataChunkWriter.o ../../src/ConfigDataChunkWriter.cpp

${OBJECTDIR}/_ext/56252444/ConfigDocumentReader.o: ../../src/ConfigDocumentReader.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/56252444
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -D_ZZIP_NO_INLINE -D_zzip_inline -I. -I../../include -I../../src -I/home/build/finjin-3rd-party/cpp/ -I/home/build/finjin-3rd-party/cpp/nowide_standalone -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/zziplib -I/home/build/finjin-3rd-party/cpp/zlib -I/home/build/finjin-3rd-party/cpp/lpng -I/home/build/finjin-3rd-party/cpp/uriparser/include -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/56252444/ConfigDocumentReader.o ../../src/ConfigDocumentReader.cpp

${OBJECTDIR}/_ext/56252444/ConfigDocumentWriter.o: ../../src/ConfigDocumentWriter.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/56252444
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -D_ZZIP_NO_INLINE -D_zzip_inline -I. -I../../include -I../../src -I/home/build/finjin-3rd-party/cpp/ -I/home/build/finjin-3rd-party/cpp/nowide_standalone -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/zziplib -I/home/build/finjin-3rd-party/cpp/zlib -I/home/build/finjin-3rd-party/cpp/lpng -I/home/build/finjin-3rd-party/cpp/uriparser/include -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/56252444/ConfigDocumentWriter.o ../../src/ConfigDocumentWriter.cpp

${OBJECTDIR}/_ext/56252444/Convert.o: ../../src/Convert.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/56252444
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -D_ZZIP_NO_INLINE -D_zzip_inline -I. -I../../include -I../../src -I/home/build/finjin-3rd-party/cpp/ -I/home/build/finjin-3rd-party/cpp/nowide_standalone -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/zziplib -I/home/build/finjin-3rd-party/cpp/zlib -I/home/build/finjin-3rd-party/cpp/lpng -I/home/build/finjin-3rd-party/cpp/uriparser/include -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/56252444/Convert.o ../../src/Convert.cpp

${OBJECTDIR}/_ext/56252444/CpuFeatures.o: ../../src/CpuFeatures.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/56252444
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -D_ZZIP_NO_INLINE -D_zzip_inline -I. -I../../include -I../../src -I/home/build/finjin-3rd-party/cpp/ -I/home/build/finjin-3rd-party/cpp/nowide_standalone -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/zziplib -I/home/build/finjin-3rd-party/cpp/zlib -I/home/build/finjin-3rd-party/cpp/lpng -I/home/build/finjin-3rd-party/cpp/uriparser/include -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/56252444/CpuFeatures.o ../../src/CpuFeatures.cpp

${OBJECTDIR}/_ext/56252444/DataChunkReader.o: ../../src/DataChunkReader.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/56252444
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -D_ZZIP_NO_INLINE -D_zzip_inline -I. -I../../include -I../../src -I/home/build/finjin-3rd-party/cpp/ -I/home/build/finjin-3rd-party/cpp/nowide_standalone -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/zziplib -I/home/build/finjin-3rd-party/cpp/zlib -I/home/build/finjin-3rd-party/cpp/lpng -I/home/build/finjin-3rd-party/cpp/uriparser/include -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/56252444/DataChunkReader.o ../../src/DataChunkReader.cpp

${OBJECTDIR}/_ext/56252444/DataChunkReaderCallbacks.o: ../../src/DataChunkReaderCallbacks.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/56252444
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -D_ZZIP_NO_INLINE -D_zzip_inline -I. -I../../include -I../../src -I/home/build/finjin-3rd-party/cpp/ -I/home/build/finjin-3rd-party/cpp/nowide_standalone -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/zziplib -I/home/build/finjin-3rd-party/cpp/zlib -I/home/build/finjin-3rd-party/cpp/lpng -I/home/build/finjin-3rd-party/cpp/uriparser/include -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/56252444/DataChunkReaderCallbacks.o ../../src/DataChunkReaderCallbacks.cpp

${OBJECTDIR}/_ext/56252444/DataChunkReaderController.o: ../../src/DataChunkReaderController.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/56252444
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -D_ZZIP_NO_INLINE -D_zzip_inline -I. -I../../include -I../../src -I/home/build/finjin-3rd-party/cpp/ -I/home/build/finjin-3rd-party/cpp/nowide_standalone -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/zziplib -I/home/build/finjin-3rd-party/cpp/zlib -I/home/build/finjin-3rd-party/cpp/lpng -I/home/build/finjin-3rd-party/cpp/uriparser/include -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/56252444/DataChunkReaderController.o ../../src/DataChunkReaderController.cpp

${OBJECTDIR}/_ext/56252444/DataChunkWriter.o: ../../src/DataChunkWriter.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/56252444
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -D_ZZIP_NO_INLINE -D_zzip_inline -I. -I../../include -I../../src -I/home/build/finjin-3rd-party/cpp/ -I/home/build/finjin-3rd-party/cpp/nowide_standalone -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/zziplib -I/home/build/finjin-3rd-party/cpp/zlib -I/home/build/finjin-3rd-party/cpp/lpng -I/home/build/finjin-3rd-party/cpp/uriparser/include -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/56252444/DataChunkWriter.o ../../src/DataChunkWriter.cpp

${OBJECTDIR}/_ext/56252444/DataChunkWriterController.o: ../../src/DataChunkWriterController.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/56252444
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -D_ZZIP_NO_INLINE -D_zzip_inline -I. -I../../include -I../../src -I/home/build/finjin-3rd-party/cpp/ -I/home/build/finjin-3rd-party/cpp/nowide_standalone -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/zziplib -I/home/build/finjin-3rd-party/cpp/zlib -I/home/build/finjin-3rd-party/cpp/lpng -I/home/build/finjin-3rd-party/cpp/uriparser/include -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/56252444/DataChunkWriterController.o ../../src/DataChunkWriterController.cpp

${OBJECTDIR}/_ext/56252444/DataElementWriter.o: ../../src/DataElementWriter.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/56252444
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -D_ZZIP_NO_INLINE -D_zzip_inline -I. -I../../include -I../../src -I/home/build/finjin-3rd-party/cpp/ -I/home/build/finjin-3rd-party/cpp/nowide_standalone -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/zziplib -I/home/build/finjin-3rd-party/cpp/zlib -I/home/build/finjin-3rd-party/cpp/lpng -I/home/build/finjin-3rd-party/cpp/uriparser/include -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/56252444/DataElementWriter.o ../../src/DataElementWriter.cpp

${OBJECTDIR}/_ext/56252444/DebugLog.o: ../../src/DebugLog.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/56252444
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -D_ZZIP_NO_INLINE -D_zzip_inline -I. -I../../include -I../../src -I/home/build/finjin-3rd-party/cpp/ -I/home/build/finjin-3rd-party/cpp/nowide_standalone -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/zziplib -I/home/build/finjin-3rd-party/cpp/zlib -I/home/build/finjin-3rd-party/cpp/lpng -I/home/build/finjin-3rd-party/cpp/uriparser/include -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/56252444/DebugLog.o ../../src/DebugLog.cpp

${OBJECTDIR}/_ext/56252444/DirEntFileFinder.o: ../../src/DirEntFileFinder.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/56252444
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -D_ZZIP_NO_INLINE -D_zzip_inline -I. -I../../include -I../../src -I/home/build/finjin-3rd-party/cpp/ -I/home/build/finjin-3rd-party/cpp/nowide_standalone -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/zziplib -I/home/build/finjin-3rd-party/cpp/zlib -I/home/build/finjin-3rd-party/cpp/lpng -I/home/build/finjin-3rd-party/cpp/uriparser/include -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/56252444/DirEntFileFinder.o ../../src/DirEntFileFinder.cpp

${OBJECTDIR}/_ext/56252444/DirectoryVirtualFileSystemRoot.o: ../../src/DirectoryVirtualFileSystemRoot.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/56252444
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -D_ZZIP_NO_INLINE -D_zzip_inline -I. -I../../include -I../../src -I/home/build/finjin-3rd-party/cpp/ -I/home/build/finjin-3rd-party/cpp/nowide_standalone -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/zziplib -I/home/build/finjin-3rd-party/cpp/zlib -I/home/build/finjin-3rd-party/cpp/lpng -I/home/build/finjin-3rd-party/cpp/uriparser/include -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/56252444/DirectoryVirtualFileSystemRoot.o ../../src/DirectoryVirtualFileSystemRoot.cpp

${OBJECTDIR}/_ext/56252444/Distance.o: ../../src/Distance.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/56252444
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -D_ZZIP_NO_INLINE -D_zzip_inline -I. -I../../include -I../../src -I/home/build/finjin-3rd-party/cpp/ -I/home/build/finjin-3rd-party/cpp/nowide_standalone -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/zziplib -I/home/build/finjin-3rd-party/cpp/zlib -I/home/build/finjin-3rd-party/cpp/lpng -I/home/build/finjin-3rd-party/cpp/uriparser/include -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/56252444/Distance.o ../../src/Distance.cpp

${OBJECTDIR}/_ext/56252444/DomainInformation.o: ../../src/DomainInformation.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/56252444
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -D_ZZIP_NO_INLINE -D_zzip_inline -I. -I../../include -I../../src -I/home/build/finjin-3rd-party/cpp/ -I/home/build/finjin-3rd-party/cpp/nowide_standalone -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/zziplib -I/home/build/finjin-3rd-party/cpp/zlib -I/home/build/finjin-3rd-party/cpp/lpng -I/home/build/finjin-3rd-party/cpp/uriparser/include -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/56252444/DomainInformation.o ../../src/DomainInformation.cpp

${OBJECTDIR}/_ext/56252444/Error.o: ../../src/Error.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/56252444
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -D_ZZIP_NO_INLINE -D_zzip_inline -I. -I../../include -I../../src -I/home/build/finjin-3rd-party/cpp/ -I/home/build/finjin-3rd-party/cpp/nowide_standalone -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/zziplib -I/home/build/finjin-3rd-party/cpp/zlib -I/home/build/finjin-3rd-party/cpp/lpng -I/home/build/finjin-3rd-party/cpp/uriparser/include -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/56252444/Error.o ../../src/Error.cpp

${OBJECTDIR}/_ext/56252444/FiberCondition.o: ../../src/FiberCondition.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/56252444
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -D_ZZIP_NO_INLINE -D_zzip_inline -I. -I../../include -I../../src -I/home/build/finjin-3rd-party/cpp/ -I/home/build/finjin-3rd-party/cpp/nowide_standalone -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/zziplib -I/home/build/finjin-3rd-party/cpp/zlib -I/home/build/finjin-3rd-party/cpp/lpng -I/home/build/finjin-3rd-party/cpp/uriparser/include -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/56252444/FiberCondition.o ../../src/FiberCondition.cpp

${OBJECTDIR}/_ext/56252444/FiberJobQueue.o: ../../src/FiberJobQueue.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/56252444
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -D_ZZIP_NO_INLINE -D_zzip_inline -I. -I../../include -I../../src -I/home/build/finjin-3rd-party/cpp/ -I/home/build/finjin-3rd-party/cpp/nowide_standalone -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/zziplib -I/home/build/finjin-3rd-party/cpp/zlib -I/home/build/finjin-3rd-party/cpp/lpng -I/home/build/finjin-3rd-party/cpp/uriparser/include -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/56252444/FiberJobQueue.o ../../src/FiberJobQueue.cpp

${OBJECTDIR}/_ext/56252444/FiberJobScheduler.o: ../../src/FiberJobScheduler.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/56252444
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -D_ZZIP_NO_INLINE -D_zzip_inline -I. -I../../include -I../../src -I/home/build/finjin-3rd-party/cpp/ -I/home/build/finjin-3rd-party/cpp/nowide_standalone -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/zziplib -I/home/build/finjin-3rd-party/cpp/zlib -I/home/build/finjin-3rd-party/cpp/lpng -I/home/build/finjin-3rd-party/cpp/uriparser/include -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/56252444/FiberJobScheduler.o ../../src/FiberJobScheduler.cpp

${OBJECTDIR}/_ext/56252444/FiberMutex.o: ../../src/FiberMutex.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/56252444
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -D_ZZIP_NO_INLINE -D_zzip_inline -I. -I../../include -I../../src -I/home/build/finjin-3rd-party/cpp/ -I/home/build/finjin-3rd-party/cpp/nowide_standalone -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/zziplib -I/home/build/finjin-3rd-party/cpp/zlib -I/home/build/finjin-3rd-party/cpp/lpng -I/home/build/finjin-3rd-party/cpp/uriparser/include -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/56252444/FiberMutex.o ../../src/FiberMutex.cpp

${OBJECTDIR}/_ext/56252444/FiberSchedulingAlgorithm.o: ../../src/FiberSchedulingAlgorithm.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/56252444
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -D_ZZIP_NO_INLINE -D_zzip_inline -I. -I../../include -I../../src -I/home/build/finjin-3rd-party/cpp/ -I/home/build/finjin-3rd-party/cpp/nowide_standalone -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/zziplib -I/home/build/finjin-3rd-party/cpp/zlib -I/home/build/finjin-3rd-party/cpp/lpng -I/home/build/finjin-3rd-party/cpp/uriparser/include -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/56252444/FiberSchedulingAlgorithm.o ../../src/FiberSchedulingAlgorithm.cpp

${OBJECTDIR}/_ext/56252444/FiberSpinLock.o: ../../src/FiberSpinLock.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/56252444
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -D_ZZIP_NO_INLINE -D_zzip_inline -I. -I../../include -I../../src -I/home/build/finjin-3rd-party/cpp/ -I/home/build/finjin-3rd-party/cpp/nowide_standalone -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/zziplib -I/home/build/finjin-3rd-party/cpp/zlib -I/home/build/finjin-3rd-party/cpp/lpng -I/home/build/finjin-3rd-party/cpp/uriparser/include -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/56252444/FiberSpinLock.o ../../src/FiberSpinLock.cpp

${OBJECTDIR}/_ext/56252444/FileAccessor.o: ../../src/FileAccessor.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/56252444
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -D_ZZIP_NO_INLINE -D_zzip_inline -I. -I../../include -I../../src -I/home/build/finjin-3rd-party/cpp/ -I/home/build/finjin-3rd-party/cpp/nowide_standalone -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/zziplib -I/home/build/finjin-3rd-party/cpp/zlib -I/home/build/finjin-3rd-party/cpp/lpng -I/home/build/finjin-3rd-party/cpp/uriparser/include -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/56252444/FileAccessor.o ../../src/FileAccessor.cpp

${OBJECTDIR}/_ext/56252444/FileSystemEntry.o: ../../src/FileSystemEntry.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/56252444
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -D_ZZIP_NO_INLINE -D_zzip_inline -I. -I../../include -I../../src -I/home/build/finjin-3rd-party/cpp/ -I/home/build/finjin-3rd-party/cpp/nowide_standalone -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/zziplib -I/home/build/finjin-3rd-party/cpp/zlib -I/home/build/finjin-3rd-party/cpp/lpng -I/home/build/finjin-3rd-party/cpp/uriparser/include -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/56252444/FileSystemEntry.o ../../src/FileSystemEntry.cpp

${OBJECTDIR}/_ext/56252444/FinjinPrecompiled.o: ../../src/FinjinPrecompiled.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/56252444
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -D_ZZIP_NO_INLINE -D_zzip_inline -I. -I../../include -I../../src -I/home/build/finjin-3rd-party/cpp/ -I/home/build/finjin-3rd-party/cpp/nowide_standalone -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/zziplib -I/home/build/finjin-3rd-party/cpp/zlib -I/home/build/finjin-3rd-party/cpp/lpng -I/home/build/finjin-3rd-party/cpp/uriparser/include -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/56252444/FinjinPrecompiled.o ../../src/FinjinPrecompiled.cpp

${OBJECTDIR}/_ext/56252444/GeneralAllocator.o: ../../src/GeneralAllocator.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/56252444
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -D_ZZIP_NO_INLINE -D_zzip_inline -I. -I../../include -I../../src -I/home/build/finjin-3rd-party/cpp/ -I/home/build/finjin-3rd-party/cpp/nowide_standalone -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/zziplib -I/home/build/finjin-3rd-party/cpp/zlib -I/home/build/finjin-3rd-party/cpp/lpng -I/home/build/finjin-3rd-party/cpp/uriparser/include -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/56252444/GeneralAllocator.o ../../src/GeneralAllocator.cpp

${OBJECTDIR}/_ext/56252444/Hash.o: ../../src/Hash.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/56252444
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -D_ZZIP_NO_INLINE -D_zzip_inline -I. -I../../include -I../../src -I/home/build/finjin-3rd-party/cpp/ -I/home/build/finjin-3rd-party/cpp/nowide_standalone -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/zziplib -I/home/build/finjin-3rd-party/cpp/zlib -I/home/build/finjin-3rd-party/cpp/lpng -I/home/build/finjin-3rd-party/cpp/uriparser/include -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/56252444/Hash.o ../../src/Hash.cpp

${OBJECTDIR}/_ext/56252444/JPEGReader.o: ../../src/JPEGReader.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/56252444
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -D_ZZIP_NO_INLINE -D_zzip_inline -I. -I../../include -I../../src -I/home/build/finjin-3rd-party/cpp/ -I/home/build/finjin-3rd-party/cpp/nowide_standalone -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/zziplib -I/home/build/finjin-3rd-party/cpp/zlib -I/home/build/finjin-3rd-party/cpp/lpng -I/home/build/finjin-3rd-party/cpp/uriparser/include -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/56252444/JPEGReader.o ../../src/JPEGReader.cpp

${OBJECTDIR}/_ext/56252444/JPEGWriter.o: ../../src/JPEGWriter.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/56252444
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -D_ZZIP_NO_INLINE -D_zzip_inline -I. -I../../include -I../../src -I/home/build/finjin-3rd-party/cpp/ -I/home/build/finjin-3rd-party/cpp/nowide_standalone -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/zziplib -I/home/build/finjin-3rd-party/cpp/zlib -I/home/build/finjin-3rd-party/cpp/lpng -I/home/build/finjin-3rd-party/cpp/uriparser/include -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/56252444/JPEGWriter.o ../../src/JPEGWriter.cpp

${OBJECTDIR}/_ext/56252444/JobFiber.o: ../../src/JobFiber.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/56252444
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -D_ZZIP_NO_INLINE -D_zzip_inline -I. -I../../include -I../../src -I/home/build/finjin-3rd-party/cpp/ -I/home/build/finjin-3rd-party/cpp/nowide_standalone -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/zziplib -I/home/build/finjin-3rd-party/cpp/zlib -I/home/build/finjin-3rd-party/cpp/lpng -I/home/build/finjin-3rd-party/cpp/uriparser/include -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/56252444/JobFiber.o ../../src/JobFiber.cpp

${OBJECTDIR}/_ext/56252444/JobFuture.o: ../../src/JobFuture.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/56252444
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -D_ZZIP_NO_INLINE -D_zzip_inline -I. -I../../include -I../../src -I/home/build/finjin-3rd-party/cpp/ -I/home/build/finjin-3rd-party/cpp/nowide_standalone -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/zziplib -I/home/build/finjin-3rd-party/cpp/zlib -I/home/build/finjin-3rd-party/cpp/lpng -I/home/build/finjin-3rd-party/cpp/uriparser/include -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/56252444/JobFuture.o ../../src/JobFuture.cpp

${OBJECTDIR}/_ext/56252444/JobSystem.o: ../../src/JobSystem.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/56252444
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -D_ZZIP_NO_INLINE -D_zzip_inline -I. -I../../include -I../../src -I/home/build/finjin-3rd-party/cpp/ -I/home/build/finjin-3rd-party/cpp/nowide_standalone -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/zziplib -I/home/build/finjin-3rd-party/cpp/zlib -I/home/build/finjin-3rd-party/cpp/lpng -I/home/build/finjin-3rd-party/cpp/uriparser/include -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/56252444/JobSystem.o ../../src/JobSystem.cpp

${OBJECTDIR}/_ext/56252444/JobThread.o: ../../src/JobThread.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/56252444
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -D_ZZIP_NO_INLINE -D_zzip_inline -I. -I../../include -I../../src -I/home/build/finjin-3rd-party/cpp/ -I/home/build/finjin-3rd-party/cpp/nowide_standalone -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/zziplib -I/home/build/finjin-3rd-party/cpp/zlib -I/home/build/finjin-3rd-party/cpp/lpng -I/home/build/finjin-3rd-party/cpp/uriparser/include -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/56252444/JobThread.o ../../src/JobThread.cpp

${OBJECTDIR}/_ext/56252444/JsonDataChunkReader.o: ../../src/JsonDataChunkReader.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/56252444
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -D_ZZIP_NO_INLINE -D_zzip_inline -I. -I../../include -I../../src -I/home/build/finjin-3rd-party/cpp/ -I/home/build/finjin-3rd-party/cpp/nowide_standalone -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/zziplib -I/home/build/finjin-3rd-party/cpp/zlib -I/home/build/finjin-3rd-party/cpp/lpng -I/home/build/finjin-3rd-party/cpp/uriparser/include -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/56252444/JsonDataChunkReader.o ../../src/JsonDataChunkReader.cpp

${OBJECTDIR}/_ext/56252444/JsonDataChunkWriter.o: ../../src/JsonDataChunkWriter.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/56252444
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -D_ZZIP_NO_INLINE -D_zzip_inline -I. -I../../include -I../../src -I/home/build/finjin-3rd-party/cpp/ -I/home/build/finjin-3rd-party/cpp/nowide_standalone -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/zziplib -I/home/build/finjin-3rd-party/cpp/zlib -I/home/build/finjin-3rd-party/cpp/lpng -I/home/build/finjin-3rd-party/cpp/uriparser/include -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/56252444/JsonDataChunkWriter.o ../../src/JsonDataChunkWriter.cpp

${OBJECTDIR}/_ext/56252444/JsonDataDocumentWriter.o: ../../src/JsonDataDocumentWriter.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/56252444
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -D_ZZIP_NO_INLINE -D_zzip_inline -I. -I../../include -I../../src -I/home/build/finjin-3rd-party/cpp/ -I/home/build/finjin-3rd-party/cpp/nowide_standalone -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/zziplib -I/home/build/finjin-3rd-party/cpp/zlib -I/home/build/finjin-3rd-party/cpp/lpng -I/home/build/finjin-3rd-party/cpp/uriparser/include -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/56252444/JsonDataDocumentWriter.o ../../src/JsonDataDocumentWriter.cpp

${OBJECTDIR}/_ext/56252444/JsonDataElementWriter.o: ../../src/JsonDataElementWriter.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/56252444
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -D_ZZIP_NO_INLINE -D_zzip_inline -I. -I../../include -I../../src -I/home/build/finjin-3rd-party/cpp/ -I/home/build/finjin-3rd-party/cpp/nowide_standalone -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/zziplib -I/home/build/finjin-3rd-party/cpp/zlib -I/home/build/finjin-3rd-party/cpp/lpng -I/home/build/finjin-3rd-party/cpp/uriparser/include -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/56252444/JsonDataElementWriter.o ../../src/JsonDataElementWriter.cpp

${OBJECTDIR}/_ext/56252444/JsonDocument.o: ../../src/JsonDocument.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/56252444
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -D_ZZIP_NO_INLINE -D_zzip_inline -I. -I../../include -I../../src -I/home/build/finjin-3rd-party/cpp/ -I/home/build/finjin-3rd-party/cpp/nowide_standalone -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/zziplib -I/home/build/finjin-3rd-party/cpp/zlib -I/home/build/finjin-3rd-party/cpp/lpng -I/home/build/finjin-3rd-party/cpp/uriparser/include -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/56252444/JsonDocument.o ../../src/JsonDocument.cpp

${OBJECTDIR}/_ext/56252444/LibraryLoader.o: ../../src/LibraryLoader.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/56252444
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -D_ZZIP_NO_INLINE -D_zzip_inline -I. -I../../include -I../../src -I/home/build/finjin-3rd-party/cpp/ -I/home/build/finjin-3rd-party/cpp/nowide_standalone -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/zziplib -I/home/build/finjin-3rd-party/cpp/zlib -I/home/build/finjin-3rd-party/cpp/lpng -I/home/build/finjin-3rd-party/cpp/uriparser/include -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/56252444/LibraryLoader.o ../../src/LibraryLoader.cpp

${OBJECTDIR}/_ext/56252444/LinuxUtilities.o: ../../src/LinuxUtilities.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/56252444
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -D_ZZIP_NO_INLINE -D_zzip_inline -I. -I../../include -I../../src -I/home/build/finjin-3rd-party/cpp/ -I/home/build/finjin-3rd-party/cpp/nowide_standalone -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/zziplib -I/home/build/finjin-3rd-party/cpp/zlib -I/home/build/finjin-3rd-party/cpp/lpng -I/home/build/finjin-3rd-party/cpp/uriparser/include -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/56252444/LinuxUtilities.o ../../src/LinuxUtilities.cpp

${OBJECTDIR}/_ext/56252444/LogLevel.o: ../../src/LogLevel.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/56252444
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -D_ZZIP_NO_INLINE -D_zzip_inline -I. -I../../include -I../../src -I/home/build/finjin-3rd-party/cpp/ -I/home/build/finjin-3rd-party/cpp/nowide_standalone -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/zziplib -I/home/build/finjin-3rd-party/cpp/zlib -I/home/build/finjin-3rd-party/cpp/lpng -I/home/build/finjin-3rd-party/cpp/uriparser/include -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/56252444/LogLevel.o ../../src/LogLevel.cpp

${OBJECTDIR}/_ext/56252444/LogicalCpu_Linux.o: ../../src/LogicalCpu_Linux.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/56252444
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -D_ZZIP_NO_INLINE -D_zzip_inline -I. -I../../include -I../../src -I/home/build/finjin-3rd-party/cpp/ -I/home/build/finjin-3rd-party/cpp/nowide_standalone -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/zziplib -I/home/build/finjin-3rd-party/cpp/zlib -I/home/build/finjin-3rd-party/cpp/lpng -I/home/build/finjin-3rd-party/cpp/uriparser/include -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/56252444/LogicalCpu_Linux.o ../../src/LogicalCpu_Linux.cpp

${OBJECTDIR}/_ext/56252444/Luid.o: ../../src/Luid.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/56252444
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -D_ZZIP_NO_INLINE -D_zzip_inline -I. -I../../include -I../../src -I/home/build/finjin-3rd-party/cpp/ -I/home/build/finjin-3rd-party/cpp/nowide_standalone -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/zziplib -I/home/build/finjin-3rd-party/cpp/zlib -I/home/build/finjin-3rd-party/cpp/lpng -I/home/build/finjin-3rd-party/cpp/uriparser/include -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/56252444/Luid.o ../../src/Luid.cpp

${OBJECTDIR}/_ext/56252444/Math.o: ../../src/Math.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/56252444
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -D_ZZIP_NO_INLINE -D_zzip_inline -I. -I../../include -I../../src -I/home/build/finjin-3rd-party/cpp/ -I/home/build/finjin-3rd-party/cpp/nowide_standalone -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/zziplib -I/home/build/finjin-3rd-party/cpp/zlib -I/home/build/finjin-3rd-party/cpp/lpng -I/home/build/finjin-3rd-party/cpp/uriparser/include -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/56252444/Math.o ../../src/Math.cpp

${OBJECTDIR}/_ext/56252444/MemoryArchitecture.o: ../../src/MemoryArchitecture.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/56252444
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -D_ZZIP_NO_INLINE -D_zzip_inline -I. -I../../include -I../../src -I/home/build/finjin-3rd-party/cpp/ -I/home/build/finjin-3rd-party/cpp/nowide_standalone -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/zziplib -I/home/build/finjin-3rd-party/cpp/zlib -I/home/build/finjin-3rd-party/cpp/lpng -I/home/build/finjin-3rd-party/cpp/uriparser/include -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/56252444/MemoryArchitecture.o ../../src/MemoryArchitecture.cpp

${OBJECTDIR}/_ext/56252444/MemoryArena.o: ../../src/MemoryArena.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/56252444
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -D_ZZIP_NO_INLINE -D_zzip_inline -I. -I../../include -I../../src -I/home/build/finjin-3rd-party/cpp/ -I/home/build/finjin-3rd-party/cpp/nowide_standalone -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/zziplib -I/home/build/finjin-3rd-party/cpp/zlib -I/home/build/finjin-3rd-party/cpp/lpng -I/home/build/finjin-3rd-party/cpp/uriparser/include -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/56252444/MemoryArena.o ../../src/MemoryArena.cpp

${OBJECTDIR}/_ext/56252444/MemorySize.o: ../../src/MemorySize.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/56252444
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -D_ZZIP_NO_INLINE -D_zzip_inline -I. -I../../include -I../../src -I/home/build/finjin-3rd-party/cpp/ -I/home/build/finjin-3rd-party/cpp/nowide_standalone -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/zziplib -I/home/build/finjin-3rd-party/cpp/zlib -I/home/build/finjin-3rd-party/cpp/lpng -I/home/build/finjin-3rd-party/cpp/uriparser/include -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/56252444/MemorySize.o ../../src/MemorySize.cpp

${OBJECTDIR}/_ext/56252444/NamePrefix.o: ../../src/NamePrefix.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/56252444
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -D_ZZIP_NO_INLINE -D_zzip_inline -I. -I../../include -I../../src -I/home/build/finjin-3rd-party/cpp/ -I/home/build/finjin-3rd-party/cpp/nowide_standalone -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/zziplib -I/home/build/finjin-3rd-party/cpp/zlib -I/home/build/finjin-3rd-party/cpp/lpng -I/home/build/finjin-3rd-party/cpp/uriparser/include -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/56252444/NamePrefix.o ../../src/NamePrefix.cpp

${OBJECTDIR}/_ext/56252444/PNGReader.o: ../../src/PNGReader.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/56252444
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -D_ZZIP_NO_INLINE -D_zzip_inline -I. -I../../include -I../../src -I/home/build/finjin-3rd-party/cpp/ -I/home/build/finjin-3rd-party/cpp/nowide_standalone -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/zziplib -I/home/build/finjin-3rd-party/cpp/zlib -I/home/build/finjin-3rd-party/cpp/lpng -I/home/build/finjin-3rd-party/cpp/uriparser/include -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/56252444/PNGReader.o ../../src/PNGReader.cpp

${OBJECTDIR}/_ext/56252444/PNGWriter.o: ../../src/PNGWriter.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/56252444
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -D_ZZIP_NO_INLINE -D_zzip_inline -I. -I../../include -I../../src -I/home/build/finjin-3rd-party/cpp/ -I/home/build/finjin-3rd-party/cpp/nowide_standalone -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/zziplib -I/home/build/finjin-3rd-party/cpp/zlib -I/home/build/finjin-3rd-party/cpp/lpng -I/home/build/finjin-3rd-party/cpp/uriparser/include -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/56252444/PNGWriter.o ../../src/PNGWriter.cpp

${OBJECTDIR}/_ext/56252444/Path.o: ../../src/Path.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/56252444
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -D_ZZIP_NO_INLINE -D_zzip_inline -I. -I../../include -I../../src -I/home/build/finjin-3rd-party/cpp/ -I/home/build/finjin-3rd-party/cpp/nowide_standalone -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/zziplib -I/home/build/finjin-3rd-party/cpp/zlib -I/home/build/finjin-3rd-party/cpp/lpng -I/home/build/finjin-3rd-party/cpp/uriparser/include -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/56252444/Path.o ../../src/Path.cpp

${OBJECTDIR}/_ext/56252444/PerformanceCounter.o: ../../src/PerformanceCounter.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/56252444
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -D_ZZIP_NO_INLINE -D_zzip_inline -I. -I../../include -I../../src -I/home/build/finjin-3rd-party/cpp/ -I/home/build/finjin-3rd-party/cpp/nowide_standalone -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/zziplib -I/home/build/finjin-3rd-party/cpp/zlib -I/home/build/finjin-3rd-party/cpp/lpng -I/home/build/finjin-3rd-party/cpp/uriparser/include -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/56252444/PerformanceCounter.o ../../src/PerformanceCounter.cpp

${OBJECTDIR}/_ext/56252444/ProcessRuntimeInformation.o: ../../src/ProcessRuntimeInformation.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/56252444
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -D_ZZIP_NO_INLINE -D_zzip_inline -I. -I../../include -I../../src -I/home/build/finjin-3rd-party/cpp/ -I/home/build/finjin-3rd-party/cpp/nowide_standalone -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/zziplib -I/home/build/finjin-3rd-party/cpp/zlib -I/home/build/finjin-3rd-party/cpp/lpng -I/home/build/finjin-3rd-party/cpp/uriparser/include -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/56252444/ProcessRuntimeInformation.o ../../src/ProcessRuntimeInformation.cpp

${OBJECTDIR}/_ext/56252444/Settings.o: ../../src/Settings.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/56252444
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -D_ZZIP_NO_INLINE -D_zzip_inline -I. -I../../include -I../../src -I/home/build/finjin-3rd-party/cpp/ -I/home/build/finjin-3rd-party/cpp/nowide_standalone -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/zziplib -I/home/build/finjin-3rd-party/cpp/zlib -I/home/build/finjin-3rd-party/cpp/lpng -I/home/build/finjin-3rd-party/cpp/uriparser/include -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/56252444/Settings.o ../../src/Settings.cpp

${OBJECTDIR}/_ext/56252444/SingleEvent.o: ../../src/SingleEvent.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/56252444
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -D_ZZIP_NO_INLINE -D_zzip_inline -I. -I../../include -I../../src -I/home/build/finjin-3rd-party/cpp/ -I/home/build/finjin-3rd-party/cpp/nowide_standalone -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/zziplib -I/home/build/finjin-3rd-party/cpp/zlib -I/home/build/finjin-3rd-party/cpp/lpng -I/home/build/finjin-3rd-party/cpp/uriparser/include -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/56252444/SingleEvent.o ../../src/SingleEvent.cpp

${OBJECTDIR}/_ext/56252444/StandardPaths.o: ../../src/StandardPaths.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/56252444
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -D_ZZIP_NO_INLINE -D_zzip_inline -I. -I../../include -I../../src -I/home/build/finjin-3rd-party/cpp/ -I/home/build/finjin-3rd-party/cpp/nowide_standalone -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/zziplib -I/home/build/finjin-3rd-party/cpp/zlib -I/home/build/finjin-3rd-party/cpp/lpng -I/home/build/finjin-3rd-party/cpp/uriparser/include -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/56252444/StandardPaths.o ../../src/StandardPaths.cpp

${OBJECTDIR}/_ext/56252444/StandardPaths_Linux.o: ../../src/StandardPaths_Linux.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/56252444
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -D_ZZIP_NO_INLINE -D_zzip_inline -I. -I../../include -I../../src -I/home/build/finjin-3rd-party/cpp/ -I/home/build/finjin-3rd-party/cpp/nowide_standalone -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/zziplib -I/home/build/finjin-3rd-party/cpp/zlib -I/home/build/finjin-3rd-party/cpp/lpng -I/home/build/finjin-3rd-party/cpp/uriparser/include -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/56252444/StandardPaths_Linux.o ../../src/StandardPaths_Linux.cpp

${OBJECTDIR}/_ext/56252444/StreamingFileFormat.o: ../../src/StreamingFileFormat.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/56252444
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -D_ZZIP_NO_INLINE -D_zzip_inline -I. -I../../include -I../../src -I/home/build/finjin-3rd-party/cpp/ -I/home/build/finjin-3rd-party/cpp/nowide_standalone -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/zziplib -I/home/build/finjin-3rd-party/cpp/zlib -I/home/build/finjin-3rd-party/cpp/lpng -I/home/build/finjin-3rd-party/cpp/uriparser/include -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/56252444/StreamingFileFormat.o ../../src/StreamingFileFormat.cpp

${OBJECTDIR}/_ext/56252444/StringMatcher.o: ../../src/StringMatcher.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/56252444
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -D_ZZIP_NO_INLINE -D_zzip_inline -I. -I../../include -I../../src -I/home/build/finjin-3rd-party/cpp/ -I/home/build/finjin-3rd-party/cpp/nowide_standalone -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/zziplib -I/home/build/finjin-3rd-party/cpp/zlib -I/home/build/finjin-3rd-party/cpp/lpng -I/home/build/finjin-3rd-party/cpp/uriparser/include -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/56252444/StringMatcher.o ../../src/StringMatcher.cpp

${OBJECTDIR}/_ext/56252444/SystemAllocator.o: ../../src/SystemAllocator.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/56252444
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -D_ZZIP_NO_INLINE -D_zzip_inline -I. -I../../include -I../../src -I/home/build/finjin-3rd-party/cpp/ -I/home/build/finjin-3rd-party/cpp/nowide_standalone -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/zziplib -I/home/build/finjin-3rd-party/cpp/zlib -I/home/build/finjin-3rd-party/cpp/lpng -I/home/build/finjin-3rd-party/cpp/uriparser/include -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/56252444/SystemAllocator.o ../../src/SystemAllocator.cpp

${OBJECTDIR}/_ext/56252444/TextDataChunkReader.o: ../../src/TextDataChunkReader.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/56252444
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -D_ZZIP_NO_INLINE -D_zzip_inline -I. -I../../include -I../../src -I/home/build/finjin-3rd-party/cpp/ -I/home/build/finjin-3rd-party/cpp/nowide_standalone -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/zziplib -I/home/build/finjin-3rd-party/cpp/zlib -I/home/build/finjin-3rd-party/cpp/lpng -I/home/build/finjin-3rd-party/cpp/uriparser/include -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/56252444/TextDataChunkReader.o ../../src/TextDataChunkReader.cpp

${OBJECTDIR}/_ext/56252444/TextDataChunkWriter.o: ../../src/TextDataChunkWriter.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/56252444
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -D_ZZIP_NO_INLINE -D_zzip_inline -I. -I../../include -I../../src -I/home/build/finjin-3rd-party/cpp/ -I/home/build/finjin-3rd-party/cpp/nowide_standalone -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/zziplib -I/home/build/finjin-3rd-party/cpp/zlib -I/home/build/finjin-3rd-party/cpp/lpng -I/home/build/finjin-3rd-party/cpp/uriparser/include -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/56252444/TextDataChunkWriter.o ../../src/TextDataChunkWriter.cpp

${OBJECTDIR}/_ext/56252444/ThisThread.o: ../../src/ThisThread.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/56252444
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -D_ZZIP_NO_INLINE -D_zzip_inline -I. -I../../include -I../../src -I/home/build/finjin-3rd-party/cpp/ -I/home/build/finjin-3rd-party/cpp/nowide_standalone -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/zziplib -I/home/build/finjin-3rd-party/cpp/zlib -I/home/build/finjin-3rd-party/cpp/lpng -I/home/build/finjin-3rd-party/cpp/uriparser/include -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/56252444/ThisThread.o ../../src/ThisThread.cpp

${OBJECTDIR}/_ext/56252444/Thread.o: ../../src/Thread.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/56252444
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -D_ZZIP_NO_INLINE -D_zzip_inline -I. -I../../include -I../../src -I/home/build/finjin-3rd-party/cpp/ -I/home/build/finjin-3rd-party/cpp/nowide_standalone -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/zziplib -I/home/build/finjin-3rd-party/cpp/zlib -I/home/build/finjin-3rd-party/cpp/lpng -I/home/build/finjin-3rd-party/cpp/uriparser/include -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/56252444/Thread.o ../../src/Thread.cpp

${OBJECTDIR}/_ext/56252444/TypeDescription.o: ../../src/TypeDescription.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/56252444
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -D_ZZIP_NO_INLINE -D_zzip_inline -I. -I../../include -I../../src -I/home/build/finjin-3rd-party/cpp/ -I/home/build/finjin-3rd-party/cpp/nowide_standalone -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/zziplib -I/home/build/finjin-3rd-party/cpp/zlib -I/home/build/finjin-3rd-party/cpp/lpng -I/home/build/finjin-3rd-party/cpp/uriparser/include -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/56252444/TypeDescription.o ../../src/TypeDescription.cpp

${OBJECTDIR}/_ext/56252444/Uri.o: ../../src/Uri.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/56252444
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -D_ZZIP_NO_INLINE -D_zzip_inline -I. -I../../include -I../../src -I/home/build/finjin-3rd-party/cpp/ -I/home/build/finjin-3rd-party/cpp/nowide_standalone -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/zziplib -I/home/build/finjin-3rd-party/cpp/zlib -I/home/build/finjin-3rd-party/cpp/lpng -I/home/build/finjin-3rd-party/cpp/uriparser/include -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/56252444/Uri.o ../../src/Uri.cpp

${OBJECTDIR}/_ext/56252444/UsageFormatter.o: ../../src/UsageFormatter.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/56252444
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -D_ZZIP_NO_INLINE -D_zzip_inline -I. -I../../include -I../../src -I/home/build/finjin-3rd-party/cpp/ -I/home/build/finjin-3rd-party/cpp/nowide_standalone -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/zziplib -I/home/build/finjin-3rd-party/cpp/zlib -I/home/build/finjin-3rd-party/cpp/lpng -I/home/build/finjin-3rd-party/cpp/uriparser/include -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/56252444/UsageFormatter.o ../../src/UsageFormatter.cpp

${OBJECTDIR}/_ext/56252444/UserInformation.o: ../../src/UserInformation.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/56252444
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -D_ZZIP_NO_INLINE -D_zzip_inline -I. -I../../include -I../../src -I/home/build/finjin-3rd-party/cpp/ -I/home/build/finjin-3rd-party/cpp/nowide_standalone -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/zziplib -I/home/build/finjin-3rd-party/cpp/zlib -I/home/build/finjin-3rd-party/cpp/lpng -I/home/build/finjin-3rd-party/cpp/uriparser/include -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/56252444/UserInformation.o ../../src/UserInformation.cpp

${OBJECTDIR}/_ext/56252444/Utf8String.o: ../../src/Utf8String.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/56252444
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -D_ZZIP_NO_INLINE -D_zzip_inline -I. -I../../include -I../../src -I/home/build/finjin-3rd-party/cpp/ -I/home/build/finjin-3rd-party/cpp/nowide_standalone -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/zziplib -I/home/build/finjin-3rd-party/cpp/zlib -I/home/build/finjin-3rd-party/cpp/lpng -I/home/build/finjin-3rd-party/cpp/uriparser/include -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/56252444/Utf8String.o ../../src/Utf8String.cpp

${OBJECTDIR}/_ext/56252444/Utf8StringFormatter.o: ../../src/Utf8StringFormatter.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/56252444
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -D_ZZIP_NO_INLINE -D_zzip_inline -I. -I../../include -I../../src -I/home/build/finjin-3rd-party/cpp/ -I/home/build/finjin-3rd-party/cpp/nowide_standalone -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/zziplib -I/home/build/finjin-3rd-party/cpp/zlib -I/home/build/finjin-3rd-party/cpp/lpng -I/home/build/finjin-3rd-party/cpp/uriparser/include -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/56252444/Utf8StringFormatter.o ../../src/Utf8StringFormatter.cpp

${OBJECTDIR}/_ext/56252444/Uuid.o: ../../src/Uuid.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/56252444
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -D_ZZIP_NO_INLINE -D_zzip_inline -I. -I../../include -I../../src -I/home/build/finjin-3rd-party/cpp/ -I/home/build/finjin-3rd-party/cpp/nowide_standalone -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/zziplib -I/home/build/finjin-3rd-party/cpp/zlib -I/home/build/finjin-3rd-party/cpp/lpng -I/home/build/finjin-3rd-party/cpp/uriparser/include -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/56252444/Uuid.o ../../src/Uuid.cpp

${OBJECTDIR}/_ext/56252444/Version.o: ../../src/Version.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/56252444
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -D_ZZIP_NO_INLINE -D_zzip_inline -I. -I../../include -I../../src -I/home/build/finjin-3rd-party/cpp/ -I/home/build/finjin-3rd-party/cpp/nowide_standalone -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/zziplib -I/home/build/finjin-3rd-party/cpp/zlib -I/home/build/finjin-3rd-party/cpp/lpng -I/home/build/finjin-3rd-party/cpp/uriparser/include -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/56252444/Version.o ../../src/Version.cpp

${OBJECTDIR}/_ext/56252444/VirtualFileHandle.o: ../../src/VirtualFileHandle.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/56252444
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -D_ZZIP_NO_INLINE -D_zzip_inline -I. -I../../include -I../../src -I/home/build/finjin-3rd-party/cpp/ -I/home/build/finjin-3rd-party/cpp/nowide_standalone -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/zziplib -I/home/build/finjin-3rd-party/cpp/zlib -I/home/build/finjin-3rd-party/cpp/lpng -I/home/build/finjin-3rd-party/cpp/uriparser/include -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/56252444/VirtualFileHandle.o ../../src/VirtualFileHandle.cpp

${OBJECTDIR}/_ext/56252444/VirtualFileOperationHandle.o: ../../src/VirtualFileOperationHandle.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/56252444
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -D_ZZIP_NO_INLINE -D_zzip_inline -I. -I../../include -I../../src -I/home/build/finjin-3rd-party/cpp/ -I/home/build/finjin-3rd-party/cpp/nowide_standalone -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/zziplib -I/home/build/finjin-3rd-party/cpp/zlib -I/home/build/finjin-3rd-party/cpp/lpng -I/home/build/finjin-3rd-party/cpp/uriparser/include -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/56252444/VirtualFileOperationHandle.o ../../src/VirtualFileOperationHandle.cpp

${OBJECTDIR}/_ext/56252444/VirtualFileSystem.o: ../../src/VirtualFileSystem.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/56252444
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -D_ZZIP_NO_INLINE -D_zzip_inline -I. -I../../include -I../../src -I/home/build/finjin-3rd-party/cpp/ -I/home/build/finjin-3rd-party/cpp/nowide_standalone -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/zziplib -I/home/build/finjin-3rd-party/cpp/zlib -I/home/build/finjin-3rd-party/cpp/lpng -I/home/build/finjin-3rd-party/cpp/uriparser/include -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/56252444/VirtualFileSystem.o ../../src/VirtualFileSystem.cpp

${OBJECTDIR}/_ext/56252444/VirtualFileSystemOperationQueue.o: ../../src/VirtualFileSystemOperationQueue.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/56252444
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -D_ZZIP_NO_INLINE -D_zzip_inline -I. -I../../include -I../../src -I/home/build/finjin-3rd-party/cpp/ -I/home/build/finjin-3rd-party/cpp/nowide_standalone -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/zziplib -I/home/build/finjin-3rd-party/cpp/zlib -I/home/build/finjin-3rd-party/cpp/lpng -I/home/build/finjin-3rd-party/cpp/uriparser/include -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/56252444/VirtualFileSystemOperationQueue.o ../../src/VirtualFileSystemOperationQueue.cpp

${OBJECTDIR}/_ext/56252444/VirtualFileSystemRoot.o: ../../src/VirtualFileSystemRoot.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/56252444
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -D_ZZIP_NO_INLINE -D_zzip_inline -I. -I../../include -I../../src -I/home/build/finjin-3rd-party/cpp/ -I/home/build/finjin-3rd-party/cpp/nowide_standalone -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/zziplib -I/home/build/finjin-3rd-party/cpp/zlib -I/home/build/finjin-3rd-party/cpp/lpng -I/home/build/finjin-3rd-party/cpp/uriparser/include -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/56252444/VirtualFileSystemRoot.o ../../src/VirtualFileSystemRoot.cpp

${OBJECTDIR}/_ext/56252444/WrappedFileReader.o: ../../src/WrappedFileReader.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/56252444
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -D_ZZIP_NO_INLINE -D_zzip_inline -I. -I../../include -I../../src -I/home/build/finjin-3rd-party/cpp/ -I/home/build/finjin-3rd-party/cpp/nowide_standalone -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/zziplib -I/home/build/finjin-3rd-party/cpp/zlib -I/home/build/finjin-3rd-party/cpp/lpng -I/home/build/finjin-3rd-party/cpp/uriparser/include -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/56252444/WrappedFileReader.o ../../src/WrappedFileReader.cpp

${OBJECTDIR}/_ext/56252444/WrappedFileWriter.o: ../../src/WrappedFileWriter.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/56252444
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -D_ZZIP_NO_INLINE -D_zzip_inline -I. -I../../include -I../../src -I/home/build/finjin-3rd-party/cpp/ -I/home/build/finjin-3rd-party/cpp/nowide_standalone -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/zziplib -I/home/build/finjin-3rd-party/cpp/zlib -I/home/build/finjin-3rd-party/cpp/lpng -I/home/build/finjin-3rd-party/cpp/uriparser/include -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/56252444/WrappedFileWriter.o ../../src/WrappedFileWriter.cpp

${OBJECTDIR}/_ext/56252444/XmlDataDocumentWriter.o: ../../src/XmlDataDocumentWriter.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/56252444
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -D_ZZIP_NO_INLINE -D_zzip_inline -I. -I../../include -I../../src -I/home/build/finjin-3rd-party/cpp/ -I/home/build/finjin-3rd-party/cpp/nowide_standalone -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/zziplib -I/home/build/finjin-3rd-party/cpp/zlib -I/home/build/finjin-3rd-party/cpp/lpng -I/home/build/finjin-3rd-party/cpp/uriparser/include -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/56252444/XmlDataDocumentWriter.o ../../src/XmlDataDocumentWriter.cpp

${OBJECTDIR}/_ext/56252444/XmlDataElementWriter.o: ../../src/XmlDataElementWriter.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/56252444
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -D_ZZIP_NO_INLINE -D_zzip_inline -I. -I../../include -I../../src -I/home/build/finjin-3rd-party/cpp/ -I/home/build/finjin-3rd-party/cpp/nowide_standalone -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/zziplib -I/home/build/finjin-3rd-party/cpp/zlib -I/home/build/finjin-3rd-party/cpp/lpng -I/home/build/finjin-3rd-party/cpp/uriparser/include -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/56252444/XmlDataElementWriter.o ../../src/XmlDataElementWriter.cpp

${OBJECTDIR}/_ext/56252444/XmlDocument.o: ../../src/XmlDocument.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/56252444
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -D_ZZIP_NO_INLINE -D_zzip_inline -I. -I../../include -I../../src -I/home/build/finjin-3rd-party/cpp/ -I/home/build/finjin-3rd-party/cpp/nowide_standalone -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/zziplib -I/home/build/finjin-3rd-party/cpp/zlib -I/home/build/finjin-3rd-party/cpp/lpng -I/home/build/finjin-3rd-party/cpp/uriparser/include -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/56252444/XmlDocument.o ../../src/XmlDocument.cpp

${OBJECTDIR}/_ext/56252444/ZIPReader.o: ../../src/ZIPReader.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/56252444
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -D_ZZIP_NO_INLINE -D_zzip_inline -I. -I../../include -I../../src -I/home/build/finjin-3rd-party/cpp/ -I/home/build/finjin-3rd-party/cpp/nowide_standalone -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/zziplib -I/home/build/finjin-3rd-party/cpp/zlib -I/home/build/finjin-3rd-party/cpp/lpng -I/home/build/finjin-3rd-party/cpp/uriparser/include -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/56252444/ZIPReader.o ../../src/ZIPReader.cpp

${OBJECTDIR}/_ext/56252444/ZipArchiveVirtualFileSystemRoot.o: ../../src/ZipArchiveVirtualFileSystemRoot.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/56252444
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -D_ZZIP_NO_INLINE -D_zzip_inline -I. -I../../include -I../../src -I/home/build/finjin-3rd-party/cpp/ -I/home/build/finjin-3rd-party/cpp/nowide_standalone -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/zziplib -I/home/build/finjin-3rd-party/cpp/zlib -I/home/build/finjin-3rd-party/cpp/lpng -I/home/build/finjin-3rd-party/cpp/uriparser/include -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/56252444/ZipArchiveVirtualFileSystemRoot.o ../../src/ZipArchiveVirtualFileSystemRoot.cpp

${OBJECTDIR}/_ext/b0297d5f/jump_x86_64_sysv_elf_gas.o: /home/build/finjin-3rd-party/cpp/boost/libs/context/src/asm/jump_x86_64_sysv_elf_gas.S 
	${MKDIR} -p ${OBJECTDIR}/_ext/b0297d5f
	$(AS) $(ASFLAGS) -o ${OBJECTDIR}/_ext/b0297d5f/jump_x86_64_sysv_elf_gas.o /home/build/finjin-3rd-party/cpp/boost/libs/context/src/asm/jump_x86_64_sysv_elf_gas.S

${OBJECTDIR}/_ext/b0297d5f/make_x86_64_sysv_elf_gas.o: /home/build/finjin-3rd-party/cpp/boost/libs/context/src/asm/make_x86_64_sysv_elf_gas.S 
	${MKDIR} -p ${OBJECTDIR}/_ext/b0297d5f
	$(AS) $(ASFLAGS) -o ${OBJECTDIR}/_ext/b0297d5f/make_x86_64_sysv_elf_gas.o /home/build/finjin-3rd-party/cpp/boost/libs/context/src/asm/make_x86_64_sysv_elf_gas.S

${OBJECTDIR}/_ext/f9f80fec/jaricom.o: /home/build/finjin-3rd-party/cpp/jpeg/jaricom.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/f9f80fec
	${RM} "$@.d"
	$(COMPILE.c) -O2 -D_ZZIP_NO_INLINE -D_zzip_inline -I. -I../../include -I../../src -I/home/build/finjin-3rd-party/cpp/ -I/home/build/finjin-3rd-party/cpp/cpp-netlib -I/home/build/finjin-3rd-party/cpp/curl/include -I/home/build/finjin-3rd-party/cpp/civetweb/include -I/home/build/finjin-3rd-party/cpp/nowide_standalone -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/zziplib -I/home/build/finjin-3rd-party/cpp/zlib -I/home/build/finjin-3rd-party/cpp/lpng -I/home/build/finjin-3rd-party/cpp/uriparser/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f9f80fec/jaricom.o /home/build/finjin-3rd-party/cpp/jpeg/jaricom.c

${OBJECTDIR}/_ext/f9f80fec/jcapimin.o: /home/build/finjin-3rd-party/cpp/jpeg/jcapimin.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/f9f80fec
	${RM} "$@.d"
	$(COMPILE.c) -O2 -D_ZZIP_NO_INLINE -D_zzip_inline -I. -I../../include -I../../src -I/home/build/finjin-3rd-party/cpp/ -I/home/build/finjin-3rd-party/cpp/cpp-netlib -I/home/build/finjin-3rd-party/cpp/curl/include -I/home/build/finjin-3rd-party/cpp/civetweb/include -I/home/build/finjin-3rd-party/cpp/nowide_standalone -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/zziplib -I/home/build/finjin-3rd-party/cpp/zlib -I/home/build/finjin-3rd-party/cpp/lpng -I/home/build/finjin-3rd-party/cpp/uriparser/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f9f80fec/jcapimin.o /home/build/finjin-3rd-party/cpp/jpeg/jcapimin.c

${OBJECTDIR}/_ext/f9f80fec/jcapistd.o: /home/build/finjin-3rd-party/cpp/jpeg/jcapistd.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/f9f80fec
	${RM} "$@.d"
	$(COMPILE.c) -O2 -D_ZZIP_NO_INLINE -D_zzip_inline -I. -I../../include -I../../src -I/home/build/finjin-3rd-party/cpp/ -I/home/build/finjin-3rd-party/cpp/cpp-netlib -I/home/build/finjin-3rd-party/cpp/curl/include -I/home/build/finjin-3rd-party/cpp/civetweb/include -I/home/build/finjin-3rd-party/cpp/nowide_standalone -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/zziplib -I/home/build/finjin-3rd-party/cpp/zlib -I/home/build/finjin-3rd-party/cpp/lpng -I/home/build/finjin-3rd-party/cpp/uriparser/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f9f80fec/jcapistd.o /home/build/finjin-3rd-party/cpp/jpeg/jcapistd.c

${OBJECTDIR}/_ext/f9f80fec/jcarith.o: /home/build/finjin-3rd-party/cpp/jpeg/jcarith.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/f9f80fec
	${RM} "$@.d"
	$(COMPILE.c) -O2 -D_ZZIP_NO_INLINE -D_zzip_inline -I. -I../../include -I../../src -I/home/build/finjin-3rd-party/cpp/ -I/home/build/finjin-3rd-party/cpp/cpp-netlib -I/home/build/finjin-3rd-party/cpp/curl/include -I/home/build/finjin-3rd-party/cpp/civetweb/include -I/home/build/finjin-3rd-party/cpp/nowide_standalone -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/zziplib -I/home/build/finjin-3rd-party/cpp/zlib -I/home/build/finjin-3rd-party/cpp/lpng -I/home/build/finjin-3rd-party/cpp/uriparser/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f9f80fec/jcarith.o /home/build/finjin-3rd-party/cpp/jpeg/jcarith.c

${OBJECTDIR}/_ext/f9f80fec/jccoefct.o: /home/build/finjin-3rd-party/cpp/jpeg/jccoefct.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/f9f80fec
	${RM} "$@.d"
	$(COMPILE.c) -O2 -D_ZZIP_NO_INLINE -D_zzip_inline -I. -I../../include -I../../src -I/home/build/finjin-3rd-party/cpp/ -I/home/build/finjin-3rd-party/cpp/cpp-netlib -I/home/build/finjin-3rd-party/cpp/curl/include -I/home/build/finjin-3rd-party/cpp/civetweb/include -I/home/build/finjin-3rd-party/cpp/nowide_standalone -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/zziplib -I/home/build/finjin-3rd-party/cpp/zlib -I/home/build/finjin-3rd-party/cpp/lpng -I/home/build/finjin-3rd-party/cpp/uriparser/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f9f80fec/jccoefct.o /home/build/finjin-3rd-party/cpp/jpeg/jccoefct.c

${OBJECTDIR}/_ext/f9f80fec/jccolor.o: /home/build/finjin-3rd-party/cpp/jpeg/jccolor.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/f9f80fec
	${RM} "$@.d"
	$(COMPILE.c) -O2 -D_ZZIP_NO_INLINE -D_zzip_inline -I. -I../../include -I../../src -I/home/build/finjin-3rd-party/cpp/ -I/home/build/finjin-3rd-party/cpp/cpp-netlib -I/home/build/finjin-3rd-party/cpp/curl/include -I/home/build/finjin-3rd-party/cpp/civetweb/include -I/home/build/finjin-3rd-party/cpp/nowide_standalone -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/zziplib -I/home/build/finjin-3rd-party/cpp/zlib -I/home/build/finjin-3rd-party/cpp/lpng -I/home/build/finjin-3rd-party/cpp/uriparser/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f9f80fec/jccolor.o /home/build/finjin-3rd-party/cpp/jpeg/jccolor.c

${OBJECTDIR}/_ext/f9f80fec/jcdctmgr.o: /home/build/finjin-3rd-party/cpp/jpeg/jcdctmgr.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/f9f80fec
	${RM} "$@.d"
	$(COMPILE.c) -O2 -D_ZZIP_NO_INLINE -D_zzip_inline -I. -I../../include -I../../src -I/home/build/finjin-3rd-party/cpp/ -I/home/build/finjin-3rd-party/cpp/cpp-netlib -I/home/build/finjin-3rd-party/cpp/curl/include -I/home/build/finjin-3rd-party/cpp/civetweb/include -I/home/build/finjin-3rd-party/cpp/nowide_standalone -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/zziplib -I/home/build/finjin-3rd-party/cpp/zlib -I/home/build/finjin-3rd-party/cpp/lpng -I/home/build/finjin-3rd-party/cpp/uriparser/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f9f80fec/jcdctmgr.o /home/build/finjin-3rd-party/cpp/jpeg/jcdctmgr.c

${OBJECTDIR}/_ext/f9f80fec/jchuff.o: /home/build/finjin-3rd-party/cpp/jpeg/jchuff.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/f9f80fec
	${RM} "$@.d"
	$(COMPILE.c) -O2 -D_ZZIP_NO_INLINE -D_zzip_inline -I. -I../../include -I../../src -I/home/build/finjin-3rd-party/cpp/ -I/home/build/finjin-3rd-party/cpp/cpp-netlib -I/home/build/finjin-3rd-party/cpp/curl/include -I/home/build/finjin-3rd-party/cpp/civetweb/include -I/home/build/finjin-3rd-party/cpp/nowide_standalone -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/zziplib -I/home/build/finjin-3rd-party/cpp/zlib -I/home/build/finjin-3rd-party/cpp/lpng -I/home/build/finjin-3rd-party/cpp/uriparser/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f9f80fec/jchuff.o /home/build/finjin-3rd-party/cpp/jpeg/jchuff.c

${OBJECTDIR}/_ext/f9f80fec/jcinit.o: /home/build/finjin-3rd-party/cpp/jpeg/jcinit.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/f9f80fec
	${RM} "$@.d"
	$(COMPILE.c) -O2 -D_ZZIP_NO_INLINE -D_zzip_inline -I. -I../../include -I../../src -I/home/build/finjin-3rd-party/cpp/ -I/home/build/finjin-3rd-party/cpp/cpp-netlib -I/home/build/finjin-3rd-party/cpp/curl/include -I/home/build/finjin-3rd-party/cpp/civetweb/include -I/home/build/finjin-3rd-party/cpp/nowide_standalone -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/zziplib -I/home/build/finjin-3rd-party/cpp/zlib -I/home/build/finjin-3rd-party/cpp/lpng -I/home/build/finjin-3rd-party/cpp/uriparser/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f9f80fec/jcinit.o /home/build/finjin-3rd-party/cpp/jpeg/jcinit.c

${OBJECTDIR}/_ext/f9f80fec/jcmainct.o: /home/build/finjin-3rd-party/cpp/jpeg/jcmainct.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/f9f80fec
	${RM} "$@.d"
	$(COMPILE.c) -O2 -D_ZZIP_NO_INLINE -D_zzip_inline -I. -I../../include -I../../src -I/home/build/finjin-3rd-party/cpp/ -I/home/build/finjin-3rd-party/cpp/cpp-netlib -I/home/build/finjin-3rd-party/cpp/curl/include -I/home/build/finjin-3rd-party/cpp/civetweb/include -I/home/build/finjin-3rd-party/cpp/nowide_standalone -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/zziplib -I/home/build/finjin-3rd-party/cpp/zlib -I/home/build/finjin-3rd-party/cpp/lpng -I/home/build/finjin-3rd-party/cpp/uriparser/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f9f80fec/jcmainct.o /home/build/finjin-3rd-party/cpp/jpeg/jcmainct.c

${OBJECTDIR}/_ext/f9f80fec/jcmarker.o: /home/build/finjin-3rd-party/cpp/jpeg/jcmarker.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/f9f80fec
	${RM} "$@.d"
	$(COMPILE.c) -O2 -D_ZZIP_NO_INLINE -D_zzip_inline -I. -I../../include -I../../src -I/home/build/finjin-3rd-party/cpp/ -I/home/build/finjin-3rd-party/cpp/cpp-netlib -I/home/build/finjin-3rd-party/cpp/curl/include -I/home/build/finjin-3rd-party/cpp/civetweb/include -I/home/build/finjin-3rd-party/cpp/nowide_standalone -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/zziplib -I/home/build/finjin-3rd-party/cpp/zlib -I/home/build/finjin-3rd-party/cpp/lpng -I/home/build/finjin-3rd-party/cpp/uriparser/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f9f80fec/jcmarker.o /home/build/finjin-3rd-party/cpp/jpeg/jcmarker.c

${OBJECTDIR}/_ext/f9f80fec/jcmaster.o: /home/build/finjin-3rd-party/cpp/jpeg/jcmaster.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/f9f80fec
	${RM} "$@.d"
	$(COMPILE.c) -O2 -D_ZZIP_NO_INLINE -D_zzip_inline -I. -I../../include -I../../src -I/home/build/finjin-3rd-party/cpp/ -I/home/build/finjin-3rd-party/cpp/cpp-netlib -I/home/build/finjin-3rd-party/cpp/curl/include -I/home/build/finjin-3rd-party/cpp/civetweb/include -I/home/build/finjin-3rd-party/cpp/nowide_standalone -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/zziplib -I/home/build/finjin-3rd-party/cpp/zlib -I/home/build/finjin-3rd-party/cpp/lpng -I/home/build/finjin-3rd-party/cpp/uriparser/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f9f80fec/jcmaster.o /home/build/finjin-3rd-party/cpp/jpeg/jcmaster.c

${OBJECTDIR}/_ext/f9f80fec/jcomapi.o: /home/build/finjin-3rd-party/cpp/jpeg/jcomapi.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/f9f80fec
	${RM} "$@.d"
	$(COMPILE.c) -O2 -D_ZZIP_NO_INLINE -D_zzip_inline -I. -I../../include -I../../src -I/home/build/finjin-3rd-party/cpp/ -I/home/build/finjin-3rd-party/cpp/cpp-netlib -I/home/build/finjin-3rd-party/cpp/curl/include -I/home/build/finjin-3rd-party/cpp/civetweb/include -I/home/build/finjin-3rd-party/cpp/nowide_standalone -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/zziplib -I/home/build/finjin-3rd-party/cpp/zlib -I/home/build/finjin-3rd-party/cpp/lpng -I/home/build/finjin-3rd-party/cpp/uriparser/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f9f80fec/jcomapi.o /home/build/finjin-3rd-party/cpp/jpeg/jcomapi.c

${OBJECTDIR}/_ext/f9f80fec/jcparam.o: /home/build/finjin-3rd-party/cpp/jpeg/jcparam.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/f9f80fec
	${RM} "$@.d"
	$(COMPILE.c) -O2 -D_ZZIP_NO_INLINE -D_zzip_inline -I. -I../../include -I../../src -I/home/build/finjin-3rd-party/cpp/ -I/home/build/finjin-3rd-party/cpp/cpp-netlib -I/home/build/finjin-3rd-party/cpp/curl/include -I/home/build/finjin-3rd-party/cpp/civetweb/include -I/home/build/finjin-3rd-party/cpp/nowide_standalone -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/zziplib -I/home/build/finjin-3rd-party/cpp/zlib -I/home/build/finjin-3rd-party/cpp/lpng -I/home/build/finjin-3rd-party/cpp/uriparser/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f9f80fec/jcparam.o /home/build/finjin-3rd-party/cpp/jpeg/jcparam.c

${OBJECTDIR}/_ext/f9f80fec/jcprepct.o: /home/build/finjin-3rd-party/cpp/jpeg/jcprepct.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/f9f80fec
	${RM} "$@.d"
	$(COMPILE.c) -O2 -D_ZZIP_NO_INLINE -D_zzip_inline -I. -I../../include -I../../src -I/home/build/finjin-3rd-party/cpp/ -I/home/build/finjin-3rd-party/cpp/cpp-netlib -I/home/build/finjin-3rd-party/cpp/curl/include -I/home/build/finjin-3rd-party/cpp/civetweb/include -I/home/build/finjin-3rd-party/cpp/nowide_standalone -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/zziplib -I/home/build/finjin-3rd-party/cpp/zlib -I/home/build/finjin-3rd-party/cpp/lpng -I/home/build/finjin-3rd-party/cpp/uriparser/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f9f80fec/jcprepct.o /home/build/finjin-3rd-party/cpp/jpeg/jcprepct.c

${OBJECTDIR}/_ext/f9f80fec/jcsample.o: /home/build/finjin-3rd-party/cpp/jpeg/jcsample.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/f9f80fec
	${RM} "$@.d"
	$(COMPILE.c) -O2 -D_ZZIP_NO_INLINE -D_zzip_inline -I. -I../../include -I../../src -I/home/build/finjin-3rd-party/cpp/ -I/home/build/finjin-3rd-party/cpp/cpp-netlib -I/home/build/finjin-3rd-party/cpp/curl/include -I/home/build/finjin-3rd-party/cpp/civetweb/include -I/home/build/finjin-3rd-party/cpp/nowide_standalone -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/zziplib -I/home/build/finjin-3rd-party/cpp/zlib -I/home/build/finjin-3rd-party/cpp/lpng -I/home/build/finjin-3rd-party/cpp/uriparser/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f9f80fec/jcsample.o /home/build/finjin-3rd-party/cpp/jpeg/jcsample.c

${OBJECTDIR}/_ext/f9f80fec/jctrans.o: /home/build/finjin-3rd-party/cpp/jpeg/jctrans.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/f9f80fec
	${RM} "$@.d"
	$(COMPILE.c) -O2 -D_ZZIP_NO_INLINE -D_zzip_inline -I. -I../../include -I../../src -I/home/build/finjin-3rd-party/cpp/ -I/home/build/finjin-3rd-party/cpp/cpp-netlib -I/home/build/finjin-3rd-party/cpp/curl/include -I/home/build/finjin-3rd-party/cpp/civetweb/include -I/home/build/finjin-3rd-party/cpp/nowide_standalone -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/zziplib -I/home/build/finjin-3rd-party/cpp/zlib -I/home/build/finjin-3rd-party/cpp/lpng -I/home/build/finjin-3rd-party/cpp/uriparser/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f9f80fec/jctrans.o /home/build/finjin-3rd-party/cpp/jpeg/jctrans.c

${OBJECTDIR}/_ext/f9f80fec/jdapimin.o: /home/build/finjin-3rd-party/cpp/jpeg/jdapimin.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/f9f80fec
	${RM} "$@.d"
	$(COMPILE.c) -O2 -D_ZZIP_NO_INLINE -D_zzip_inline -I. -I../../include -I../../src -I/home/build/finjin-3rd-party/cpp/ -I/home/build/finjin-3rd-party/cpp/cpp-netlib -I/home/build/finjin-3rd-party/cpp/curl/include -I/home/build/finjin-3rd-party/cpp/civetweb/include -I/home/build/finjin-3rd-party/cpp/nowide_standalone -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/zziplib -I/home/build/finjin-3rd-party/cpp/zlib -I/home/build/finjin-3rd-party/cpp/lpng -I/home/build/finjin-3rd-party/cpp/uriparser/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f9f80fec/jdapimin.o /home/build/finjin-3rd-party/cpp/jpeg/jdapimin.c

${OBJECTDIR}/_ext/f9f80fec/jdapistd.o: /home/build/finjin-3rd-party/cpp/jpeg/jdapistd.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/f9f80fec
	${RM} "$@.d"
	$(COMPILE.c) -O2 -D_ZZIP_NO_INLINE -D_zzip_inline -I. -I../../include -I../../src -I/home/build/finjin-3rd-party/cpp/ -I/home/build/finjin-3rd-party/cpp/cpp-netlib -I/home/build/finjin-3rd-party/cpp/curl/include -I/home/build/finjin-3rd-party/cpp/civetweb/include -I/home/build/finjin-3rd-party/cpp/nowide_standalone -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/zziplib -I/home/build/finjin-3rd-party/cpp/zlib -I/home/build/finjin-3rd-party/cpp/lpng -I/home/build/finjin-3rd-party/cpp/uriparser/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f9f80fec/jdapistd.o /home/build/finjin-3rd-party/cpp/jpeg/jdapistd.c

${OBJECTDIR}/_ext/f9f80fec/jdarith.o: /home/build/finjin-3rd-party/cpp/jpeg/jdarith.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/f9f80fec
	${RM} "$@.d"
	$(COMPILE.c) -O2 -D_ZZIP_NO_INLINE -D_zzip_inline -I. -I../../include -I../../src -I/home/build/finjin-3rd-party/cpp/ -I/home/build/finjin-3rd-party/cpp/cpp-netlib -I/home/build/finjin-3rd-party/cpp/curl/include -I/home/build/finjin-3rd-party/cpp/civetweb/include -I/home/build/finjin-3rd-party/cpp/nowide_standalone -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/zziplib -I/home/build/finjin-3rd-party/cpp/zlib -I/home/build/finjin-3rd-party/cpp/lpng -I/home/build/finjin-3rd-party/cpp/uriparser/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f9f80fec/jdarith.o /home/build/finjin-3rd-party/cpp/jpeg/jdarith.c

${OBJECTDIR}/_ext/f9f80fec/jdatadst.o: /home/build/finjin-3rd-party/cpp/jpeg/jdatadst.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/f9f80fec
	${RM} "$@.d"
	$(COMPILE.c) -O2 -D_ZZIP_NO_INLINE -D_zzip_inline -I. -I../../include -I../../src -I/home/build/finjin-3rd-party/cpp/ -I/home/build/finjin-3rd-party/cpp/cpp-netlib -I/home/build/finjin-3rd-party/cpp/curl/include -I/home/build/finjin-3rd-party/cpp/civetweb/include -I/home/build/finjin-3rd-party/cpp/nowide_standalone -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/zziplib -I/home/build/finjin-3rd-party/cpp/zlib -I/home/build/finjin-3rd-party/cpp/lpng -I/home/build/finjin-3rd-party/cpp/uriparser/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f9f80fec/jdatadst.o /home/build/finjin-3rd-party/cpp/jpeg/jdatadst.c

${OBJECTDIR}/_ext/f9f80fec/jdatasrc.o: /home/build/finjin-3rd-party/cpp/jpeg/jdatasrc.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/f9f80fec
	${RM} "$@.d"
	$(COMPILE.c) -O2 -D_ZZIP_NO_INLINE -D_zzip_inline -I. -I../../include -I../../src -I/home/build/finjin-3rd-party/cpp/ -I/home/build/finjin-3rd-party/cpp/cpp-netlib -I/home/build/finjin-3rd-party/cpp/curl/include -I/home/build/finjin-3rd-party/cpp/civetweb/include -I/home/build/finjin-3rd-party/cpp/nowide_standalone -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/zziplib -I/home/build/finjin-3rd-party/cpp/zlib -I/home/build/finjin-3rd-party/cpp/lpng -I/home/build/finjin-3rd-party/cpp/uriparser/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f9f80fec/jdatasrc.o /home/build/finjin-3rd-party/cpp/jpeg/jdatasrc.c

${OBJECTDIR}/_ext/f9f80fec/jdcoefct.o: /home/build/finjin-3rd-party/cpp/jpeg/jdcoefct.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/f9f80fec
	${RM} "$@.d"
	$(COMPILE.c) -O2 -D_ZZIP_NO_INLINE -D_zzip_inline -I. -I../../include -I../../src -I/home/build/finjin-3rd-party/cpp/ -I/home/build/finjin-3rd-party/cpp/cpp-netlib -I/home/build/finjin-3rd-party/cpp/curl/include -I/home/build/finjin-3rd-party/cpp/civetweb/include -I/home/build/finjin-3rd-party/cpp/nowide_standalone -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/zziplib -I/home/build/finjin-3rd-party/cpp/zlib -I/home/build/finjin-3rd-party/cpp/lpng -I/home/build/finjin-3rd-party/cpp/uriparser/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f9f80fec/jdcoefct.o /home/build/finjin-3rd-party/cpp/jpeg/jdcoefct.c

${OBJECTDIR}/_ext/f9f80fec/jdcolor.o: /home/build/finjin-3rd-party/cpp/jpeg/jdcolor.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/f9f80fec
	${RM} "$@.d"
	$(COMPILE.c) -O2 -D_ZZIP_NO_INLINE -D_zzip_inline -I. -I../../include -I../../src -I/home/build/finjin-3rd-party/cpp/ -I/home/build/finjin-3rd-party/cpp/cpp-netlib -I/home/build/finjin-3rd-party/cpp/curl/include -I/home/build/finjin-3rd-party/cpp/civetweb/include -I/home/build/finjin-3rd-party/cpp/nowide_standalone -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/zziplib -I/home/build/finjin-3rd-party/cpp/zlib -I/home/build/finjin-3rd-party/cpp/lpng -I/home/build/finjin-3rd-party/cpp/uriparser/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f9f80fec/jdcolor.o /home/build/finjin-3rd-party/cpp/jpeg/jdcolor.c

${OBJECTDIR}/_ext/f9f80fec/jddctmgr.o: /home/build/finjin-3rd-party/cpp/jpeg/jddctmgr.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/f9f80fec
	${RM} "$@.d"
	$(COMPILE.c) -O2 -D_ZZIP_NO_INLINE -D_zzip_inline -I. -I../../include -I../../src -I/home/build/finjin-3rd-party/cpp/ -I/home/build/finjin-3rd-party/cpp/cpp-netlib -I/home/build/finjin-3rd-party/cpp/curl/include -I/home/build/finjin-3rd-party/cpp/civetweb/include -I/home/build/finjin-3rd-party/cpp/nowide_standalone -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/zziplib -I/home/build/finjin-3rd-party/cpp/zlib -I/home/build/finjin-3rd-party/cpp/lpng -I/home/build/finjin-3rd-party/cpp/uriparser/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f9f80fec/jddctmgr.o /home/build/finjin-3rd-party/cpp/jpeg/jddctmgr.c

${OBJECTDIR}/_ext/f9f80fec/jdhuff.o: /home/build/finjin-3rd-party/cpp/jpeg/jdhuff.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/f9f80fec
	${RM} "$@.d"
	$(COMPILE.c) -O2 -D_ZZIP_NO_INLINE -D_zzip_inline -I. -I../../include -I../../src -I/home/build/finjin-3rd-party/cpp/ -I/home/build/finjin-3rd-party/cpp/cpp-netlib -I/home/build/finjin-3rd-party/cpp/curl/include -I/home/build/finjin-3rd-party/cpp/civetweb/include -I/home/build/finjin-3rd-party/cpp/nowide_standalone -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/zziplib -I/home/build/finjin-3rd-party/cpp/zlib -I/home/build/finjin-3rd-party/cpp/lpng -I/home/build/finjin-3rd-party/cpp/uriparser/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f9f80fec/jdhuff.o /home/build/finjin-3rd-party/cpp/jpeg/jdhuff.c

${OBJECTDIR}/_ext/f9f80fec/jdinput.o: /home/build/finjin-3rd-party/cpp/jpeg/jdinput.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/f9f80fec
	${RM} "$@.d"
	$(COMPILE.c) -O2 -D_ZZIP_NO_INLINE -D_zzip_inline -I. -I../../include -I../../src -I/home/build/finjin-3rd-party/cpp/ -I/home/build/finjin-3rd-party/cpp/cpp-netlib -I/home/build/finjin-3rd-party/cpp/curl/include -I/home/build/finjin-3rd-party/cpp/civetweb/include -I/home/build/finjin-3rd-party/cpp/nowide_standalone -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/zziplib -I/home/build/finjin-3rd-party/cpp/zlib -I/home/build/finjin-3rd-party/cpp/lpng -I/home/build/finjin-3rd-party/cpp/uriparser/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f9f80fec/jdinput.o /home/build/finjin-3rd-party/cpp/jpeg/jdinput.c

${OBJECTDIR}/_ext/f9f80fec/jdmainct.o: /home/build/finjin-3rd-party/cpp/jpeg/jdmainct.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/f9f80fec
	${RM} "$@.d"
	$(COMPILE.c) -O2 -D_ZZIP_NO_INLINE -D_zzip_inline -I. -I../../include -I../../src -I/home/build/finjin-3rd-party/cpp/ -I/home/build/finjin-3rd-party/cpp/cpp-netlib -I/home/build/finjin-3rd-party/cpp/curl/include -I/home/build/finjin-3rd-party/cpp/civetweb/include -I/home/build/finjin-3rd-party/cpp/nowide_standalone -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/zziplib -I/home/build/finjin-3rd-party/cpp/zlib -I/home/build/finjin-3rd-party/cpp/lpng -I/home/build/finjin-3rd-party/cpp/uriparser/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f9f80fec/jdmainct.o /home/build/finjin-3rd-party/cpp/jpeg/jdmainct.c

${OBJECTDIR}/_ext/f9f80fec/jdmarker.o: /home/build/finjin-3rd-party/cpp/jpeg/jdmarker.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/f9f80fec
	${RM} "$@.d"
	$(COMPILE.c) -O2 -D_ZZIP_NO_INLINE -D_zzip_inline -I. -I../../include -I../../src -I/home/build/finjin-3rd-party/cpp/ -I/home/build/finjin-3rd-party/cpp/cpp-netlib -I/home/build/finjin-3rd-party/cpp/curl/include -I/home/build/finjin-3rd-party/cpp/civetweb/include -I/home/build/finjin-3rd-party/cpp/nowide_standalone -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/zziplib -I/home/build/finjin-3rd-party/cpp/zlib -I/home/build/finjin-3rd-party/cpp/lpng -I/home/build/finjin-3rd-party/cpp/uriparser/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f9f80fec/jdmarker.o /home/build/finjin-3rd-party/cpp/jpeg/jdmarker.c

${OBJECTDIR}/_ext/f9f80fec/jdmaster.o: /home/build/finjin-3rd-party/cpp/jpeg/jdmaster.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/f9f80fec
	${RM} "$@.d"
	$(COMPILE.c) -O2 -D_ZZIP_NO_INLINE -D_zzip_inline -I. -I../../include -I../../src -I/home/build/finjin-3rd-party/cpp/ -I/home/build/finjin-3rd-party/cpp/cpp-netlib -I/home/build/finjin-3rd-party/cpp/curl/include -I/home/build/finjin-3rd-party/cpp/civetweb/include -I/home/build/finjin-3rd-party/cpp/nowide_standalone -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/zziplib -I/home/build/finjin-3rd-party/cpp/zlib -I/home/build/finjin-3rd-party/cpp/lpng -I/home/build/finjin-3rd-party/cpp/uriparser/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f9f80fec/jdmaster.o /home/build/finjin-3rd-party/cpp/jpeg/jdmaster.c

${OBJECTDIR}/_ext/f9f80fec/jdmerge.o: /home/build/finjin-3rd-party/cpp/jpeg/jdmerge.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/f9f80fec
	${RM} "$@.d"
	$(COMPILE.c) -O2 -D_ZZIP_NO_INLINE -D_zzip_inline -I. -I../../include -I../../src -I/home/build/finjin-3rd-party/cpp/ -I/home/build/finjin-3rd-party/cpp/cpp-netlib -I/home/build/finjin-3rd-party/cpp/curl/include -I/home/build/finjin-3rd-party/cpp/civetweb/include -I/home/build/finjin-3rd-party/cpp/nowide_standalone -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/zziplib -I/home/build/finjin-3rd-party/cpp/zlib -I/home/build/finjin-3rd-party/cpp/lpng -I/home/build/finjin-3rd-party/cpp/uriparser/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f9f80fec/jdmerge.o /home/build/finjin-3rd-party/cpp/jpeg/jdmerge.c

${OBJECTDIR}/_ext/f9f80fec/jdpostct.o: /home/build/finjin-3rd-party/cpp/jpeg/jdpostct.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/f9f80fec
	${RM} "$@.d"
	$(COMPILE.c) -O2 -D_ZZIP_NO_INLINE -D_zzip_inline -I. -I../../include -I../../src -I/home/build/finjin-3rd-party/cpp/ -I/home/build/finjin-3rd-party/cpp/cpp-netlib -I/home/build/finjin-3rd-party/cpp/curl/include -I/home/build/finjin-3rd-party/cpp/civetweb/include -I/home/build/finjin-3rd-party/cpp/nowide_standalone -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/zziplib -I/home/build/finjin-3rd-party/cpp/zlib -I/home/build/finjin-3rd-party/cpp/lpng -I/home/build/finjin-3rd-party/cpp/uriparser/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f9f80fec/jdpostct.o /home/build/finjin-3rd-party/cpp/jpeg/jdpostct.c

${OBJECTDIR}/_ext/f9f80fec/jdsample.o: /home/build/finjin-3rd-party/cpp/jpeg/jdsample.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/f9f80fec
	${RM} "$@.d"
	$(COMPILE.c) -O2 -D_ZZIP_NO_INLINE -D_zzip_inline -I. -I../../include -I../../src -I/home/build/finjin-3rd-party/cpp/ -I/home/build/finjin-3rd-party/cpp/cpp-netlib -I/home/build/finjin-3rd-party/cpp/curl/include -I/home/build/finjin-3rd-party/cpp/civetweb/include -I/home/build/finjin-3rd-party/cpp/nowide_standalone -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/zziplib -I/home/build/finjin-3rd-party/cpp/zlib -I/home/build/finjin-3rd-party/cpp/lpng -I/home/build/finjin-3rd-party/cpp/uriparser/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f9f80fec/jdsample.o /home/build/finjin-3rd-party/cpp/jpeg/jdsample.c

${OBJECTDIR}/_ext/f9f80fec/jdtrans.o: /home/build/finjin-3rd-party/cpp/jpeg/jdtrans.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/f9f80fec
	${RM} "$@.d"
	$(COMPILE.c) -O2 -D_ZZIP_NO_INLINE -D_zzip_inline -I. -I../../include -I../../src -I/home/build/finjin-3rd-party/cpp/ -I/home/build/finjin-3rd-party/cpp/cpp-netlib -I/home/build/finjin-3rd-party/cpp/curl/include -I/home/build/finjin-3rd-party/cpp/civetweb/include -I/home/build/finjin-3rd-party/cpp/nowide_standalone -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/zziplib -I/home/build/finjin-3rd-party/cpp/zlib -I/home/build/finjin-3rd-party/cpp/lpng -I/home/build/finjin-3rd-party/cpp/uriparser/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f9f80fec/jdtrans.o /home/build/finjin-3rd-party/cpp/jpeg/jdtrans.c

${OBJECTDIR}/_ext/f9f80fec/jerror.o: /home/build/finjin-3rd-party/cpp/jpeg/jerror.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/f9f80fec
	${RM} "$@.d"
	$(COMPILE.c) -O2 -D_ZZIP_NO_INLINE -D_zzip_inline -I. -I../../include -I../../src -I/home/build/finjin-3rd-party/cpp/ -I/home/build/finjin-3rd-party/cpp/cpp-netlib -I/home/build/finjin-3rd-party/cpp/curl/include -I/home/build/finjin-3rd-party/cpp/civetweb/include -I/home/build/finjin-3rd-party/cpp/nowide_standalone -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/zziplib -I/home/build/finjin-3rd-party/cpp/zlib -I/home/build/finjin-3rd-party/cpp/lpng -I/home/build/finjin-3rd-party/cpp/uriparser/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f9f80fec/jerror.o /home/build/finjin-3rd-party/cpp/jpeg/jerror.c

${OBJECTDIR}/_ext/f9f80fec/jfdctflt.o: /home/build/finjin-3rd-party/cpp/jpeg/jfdctflt.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/f9f80fec
	${RM} "$@.d"
	$(COMPILE.c) -O2 -D_ZZIP_NO_INLINE -D_zzip_inline -I. -I../../include -I../../src -I/home/build/finjin-3rd-party/cpp/ -I/home/build/finjin-3rd-party/cpp/cpp-netlib -I/home/build/finjin-3rd-party/cpp/curl/include -I/home/build/finjin-3rd-party/cpp/civetweb/include -I/home/build/finjin-3rd-party/cpp/nowide_standalone -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/zziplib -I/home/build/finjin-3rd-party/cpp/zlib -I/home/build/finjin-3rd-party/cpp/lpng -I/home/build/finjin-3rd-party/cpp/uriparser/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f9f80fec/jfdctflt.o /home/build/finjin-3rd-party/cpp/jpeg/jfdctflt.c

${OBJECTDIR}/_ext/f9f80fec/jfdctfst.o: /home/build/finjin-3rd-party/cpp/jpeg/jfdctfst.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/f9f80fec
	${RM} "$@.d"
	$(COMPILE.c) -O2 -D_ZZIP_NO_INLINE -D_zzip_inline -I. -I../../include -I../../src -I/home/build/finjin-3rd-party/cpp/ -I/home/build/finjin-3rd-party/cpp/cpp-netlib -I/home/build/finjin-3rd-party/cpp/curl/include -I/home/build/finjin-3rd-party/cpp/civetweb/include -I/home/build/finjin-3rd-party/cpp/nowide_standalone -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/zziplib -I/home/build/finjin-3rd-party/cpp/zlib -I/home/build/finjin-3rd-party/cpp/lpng -I/home/build/finjin-3rd-party/cpp/uriparser/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f9f80fec/jfdctfst.o /home/build/finjin-3rd-party/cpp/jpeg/jfdctfst.c

${OBJECTDIR}/_ext/f9f80fec/jfdctint.o: /home/build/finjin-3rd-party/cpp/jpeg/jfdctint.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/f9f80fec
	${RM} "$@.d"
	$(COMPILE.c) -O2 -D_ZZIP_NO_INLINE -D_zzip_inline -I. -I../../include -I../../src -I/home/build/finjin-3rd-party/cpp/ -I/home/build/finjin-3rd-party/cpp/cpp-netlib -I/home/build/finjin-3rd-party/cpp/curl/include -I/home/build/finjin-3rd-party/cpp/civetweb/include -I/home/build/finjin-3rd-party/cpp/nowide_standalone -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/zziplib -I/home/build/finjin-3rd-party/cpp/zlib -I/home/build/finjin-3rd-party/cpp/lpng -I/home/build/finjin-3rd-party/cpp/uriparser/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f9f80fec/jfdctint.o /home/build/finjin-3rd-party/cpp/jpeg/jfdctint.c

${OBJECTDIR}/_ext/f9f80fec/jidctflt.o: /home/build/finjin-3rd-party/cpp/jpeg/jidctflt.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/f9f80fec
	${RM} "$@.d"
	$(COMPILE.c) -O2 -D_ZZIP_NO_INLINE -D_zzip_inline -I. -I../../include -I../../src -I/home/build/finjin-3rd-party/cpp/ -I/home/build/finjin-3rd-party/cpp/cpp-netlib -I/home/build/finjin-3rd-party/cpp/curl/include -I/home/build/finjin-3rd-party/cpp/civetweb/include -I/home/build/finjin-3rd-party/cpp/nowide_standalone -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/zziplib -I/home/build/finjin-3rd-party/cpp/zlib -I/home/build/finjin-3rd-party/cpp/lpng -I/home/build/finjin-3rd-party/cpp/uriparser/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f9f80fec/jidctflt.o /home/build/finjin-3rd-party/cpp/jpeg/jidctflt.c

${OBJECTDIR}/_ext/f9f80fec/jidctfst.o: /home/build/finjin-3rd-party/cpp/jpeg/jidctfst.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/f9f80fec
	${RM} "$@.d"
	$(COMPILE.c) -O2 -D_ZZIP_NO_INLINE -D_zzip_inline -I. -I../../include -I../../src -I/home/build/finjin-3rd-party/cpp/ -I/home/build/finjin-3rd-party/cpp/cpp-netlib -I/home/build/finjin-3rd-party/cpp/curl/include -I/home/build/finjin-3rd-party/cpp/civetweb/include -I/home/build/finjin-3rd-party/cpp/nowide_standalone -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/zziplib -I/home/build/finjin-3rd-party/cpp/zlib -I/home/build/finjin-3rd-party/cpp/lpng -I/home/build/finjin-3rd-party/cpp/uriparser/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f9f80fec/jidctfst.o /home/build/finjin-3rd-party/cpp/jpeg/jidctfst.c

${OBJECTDIR}/_ext/f9f80fec/jidctint.o: /home/build/finjin-3rd-party/cpp/jpeg/jidctint.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/f9f80fec
	${RM} "$@.d"
	$(COMPILE.c) -O2 -D_ZZIP_NO_INLINE -D_zzip_inline -I. -I../../include -I../../src -I/home/build/finjin-3rd-party/cpp/ -I/home/build/finjin-3rd-party/cpp/cpp-netlib -I/home/build/finjin-3rd-party/cpp/curl/include -I/home/build/finjin-3rd-party/cpp/civetweb/include -I/home/build/finjin-3rd-party/cpp/nowide_standalone -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/zziplib -I/home/build/finjin-3rd-party/cpp/zlib -I/home/build/finjin-3rd-party/cpp/lpng -I/home/build/finjin-3rd-party/cpp/uriparser/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f9f80fec/jidctint.o /home/build/finjin-3rd-party/cpp/jpeg/jidctint.c

${OBJECTDIR}/_ext/f9f80fec/jmemmgr.o: /home/build/finjin-3rd-party/cpp/jpeg/jmemmgr.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/f9f80fec
	${RM} "$@.d"
	$(COMPILE.c) -O2 -D_ZZIP_NO_INLINE -D_zzip_inline -I. -I../../include -I../../src -I/home/build/finjin-3rd-party/cpp/ -I/home/build/finjin-3rd-party/cpp/cpp-netlib -I/home/build/finjin-3rd-party/cpp/curl/include -I/home/build/finjin-3rd-party/cpp/civetweb/include -I/home/build/finjin-3rd-party/cpp/nowide_standalone -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/zziplib -I/home/build/finjin-3rd-party/cpp/zlib -I/home/build/finjin-3rd-party/cpp/lpng -I/home/build/finjin-3rd-party/cpp/uriparser/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f9f80fec/jmemmgr.o /home/build/finjin-3rd-party/cpp/jpeg/jmemmgr.c

${OBJECTDIR}/_ext/f9f80fec/jmemnobs.o: /home/build/finjin-3rd-party/cpp/jpeg/jmemnobs.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/f9f80fec
	${RM} "$@.d"
	$(COMPILE.c) -O2 -D_ZZIP_NO_INLINE -D_zzip_inline -I. -I../../include -I../../src -I/home/build/finjin-3rd-party/cpp/ -I/home/build/finjin-3rd-party/cpp/cpp-netlib -I/home/build/finjin-3rd-party/cpp/curl/include -I/home/build/finjin-3rd-party/cpp/civetweb/include -I/home/build/finjin-3rd-party/cpp/nowide_standalone -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/zziplib -I/home/build/finjin-3rd-party/cpp/zlib -I/home/build/finjin-3rd-party/cpp/lpng -I/home/build/finjin-3rd-party/cpp/uriparser/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f9f80fec/jmemnobs.o /home/build/finjin-3rd-party/cpp/jpeg/jmemnobs.c

${OBJECTDIR}/_ext/f9f80fec/jquant1.o: /home/build/finjin-3rd-party/cpp/jpeg/jquant1.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/f9f80fec
	${RM} "$@.d"
	$(COMPILE.c) -O2 -D_ZZIP_NO_INLINE -D_zzip_inline -I. -I../../include -I../../src -I/home/build/finjin-3rd-party/cpp/ -I/home/build/finjin-3rd-party/cpp/cpp-netlib -I/home/build/finjin-3rd-party/cpp/curl/include -I/home/build/finjin-3rd-party/cpp/civetweb/include -I/home/build/finjin-3rd-party/cpp/nowide_standalone -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/zziplib -I/home/build/finjin-3rd-party/cpp/zlib -I/home/build/finjin-3rd-party/cpp/lpng -I/home/build/finjin-3rd-party/cpp/uriparser/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f9f80fec/jquant1.o /home/build/finjin-3rd-party/cpp/jpeg/jquant1.c

${OBJECTDIR}/_ext/f9f80fec/jquant2.o: /home/build/finjin-3rd-party/cpp/jpeg/jquant2.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/f9f80fec
	${RM} "$@.d"
	$(COMPILE.c) -O2 -D_ZZIP_NO_INLINE -D_zzip_inline -I. -I../../include -I../../src -I/home/build/finjin-3rd-party/cpp/ -I/home/build/finjin-3rd-party/cpp/cpp-netlib -I/home/build/finjin-3rd-party/cpp/curl/include -I/home/build/finjin-3rd-party/cpp/civetweb/include -I/home/build/finjin-3rd-party/cpp/nowide_standalone -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/zziplib -I/home/build/finjin-3rd-party/cpp/zlib -I/home/build/finjin-3rd-party/cpp/lpng -I/home/build/finjin-3rd-party/cpp/uriparser/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f9f80fec/jquant2.o /home/build/finjin-3rd-party/cpp/jpeg/jquant2.c

${OBJECTDIR}/_ext/f9f80fec/jutils.o: /home/build/finjin-3rd-party/cpp/jpeg/jutils.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/f9f80fec
	${RM} "$@.d"
	$(COMPILE.c) -O2 -D_ZZIP_NO_INLINE -D_zzip_inline -I. -I../../include -I../../src -I/home/build/finjin-3rd-party/cpp/ -I/home/build/finjin-3rd-party/cpp/cpp-netlib -I/home/build/finjin-3rd-party/cpp/curl/include -I/home/build/finjin-3rd-party/cpp/civetweb/include -I/home/build/finjin-3rd-party/cpp/nowide_standalone -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/zziplib -I/home/build/finjin-3rd-party/cpp/zlib -I/home/build/finjin-3rd-party/cpp/lpng -I/home/build/finjin-3rd-party/cpp/uriparser/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f9f80fec/jutils.o /home/build/finjin-3rd-party/cpp/jpeg/jutils.c

${OBJECTDIR}/_ext/f9f80fec/rdbmp.o: /home/build/finjin-3rd-party/cpp/jpeg/rdbmp.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/f9f80fec
	${RM} "$@.d"
	$(COMPILE.c) -O2 -D_ZZIP_NO_INLINE -D_zzip_inline -I. -I../../include -I../../src -I/home/build/finjin-3rd-party/cpp/ -I/home/build/finjin-3rd-party/cpp/cpp-netlib -I/home/build/finjin-3rd-party/cpp/curl/include -I/home/build/finjin-3rd-party/cpp/civetweb/include -I/home/build/finjin-3rd-party/cpp/nowide_standalone -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/zziplib -I/home/build/finjin-3rd-party/cpp/zlib -I/home/build/finjin-3rd-party/cpp/lpng -I/home/build/finjin-3rd-party/cpp/uriparser/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f9f80fec/rdbmp.o /home/build/finjin-3rd-party/cpp/jpeg/rdbmp.c

${OBJECTDIR}/_ext/f9f80fec/rdcolmap.o: /home/build/finjin-3rd-party/cpp/jpeg/rdcolmap.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/f9f80fec
	${RM} "$@.d"
	$(COMPILE.c) -O2 -D_ZZIP_NO_INLINE -D_zzip_inline -I. -I../../include -I../../src -I/home/build/finjin-3rd-party/cpp/ -I/home/build/finjin-3rd-party/cpp/cpp-netlib -I/home/build/finjin-3rd-party/cpp/curl/include -I/home/build/finjin-3rd-party/cpp/civetweb/include -I/home/build/finjin-3rd-party/cpp/nowide_standalone -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/zziplib -I/home/build/finjin-3rd-party/cpp/zlib -I/home/build/finjin-3rd-party/cpp/lpng -I/home/build/finjin-3rd-party/cpp/uriparser/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f9f80fec/rdcolmap.o /home/build/finjin-3rd-party/cpp/jpeg/rdcolmap.c

${OBJECTDIR}/_ext/f9f80fec/rdgif.o: /home/build/finjin-3rd-party/cpp/jpeg/rdgif.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/f9f80fec
	${RM} "$@.d"
	$(COMPILE.c) -O2 -D_ZZIP_NO_INLINE -D_zzip_inline -I. -I../../include -I../../src -I/home/build/finjin-3rd-party/cpp/ -I/home/build/finjin-3rd-party/cpp/cpp-netlib -I/home/build/finjin-3rd-party/cpp/curl/include -I/home/build/finjin-3rd-party/cpp/civetweb/include -I/home/build/finjin-3rd-party/cpp/nowide_standalone -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/zziplib -I/home/build/finjin-3rd-party/cpp/zlib -I/home/build/finjin-3rd-party/cpp/lpng -I/home/build/finjin-3rd-party/cpp/uriparser/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f9f80fec/rdgif.o /home/build/finjin-3rd-party/cpp/jpeg/rdgif.c

${OBJECTDIR}/_ext/f9f80fec/rdppm.o: /home/build/finjin-3rd-party/cpp/jpeg/rdppm.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/f9f80fec
	${RM} "$@.d"
	$(COMPILE.c) -O2 -D_ZZIP_NO_INLINE -D_zzip_inline -I. -I../../include -I../../src -I/home/build/finjin-3rd-party/cpp/ -I/home/build/finjin-3rd-party/cpp/cpp-netlib -I/home/build/finjin-3rd-party/cpp/curl/include -I/home/build/finjin-3rd-party/cpp/civetweb/include -I/home/build/finjin-3rd-party/cpp/nowide_standalone -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/zziplib -I/home/build/finjin-3rd-party/cpp/zlib -I/home/build/finjin-3rd-party/cpp/lpng -I/home/build/finjin-3rd-party/cpp/uriparser/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f9f80fec/rdppm.o /home/build/finjin-3rd-party/cpp/jpeg/rdppm.c

${OBJECTDIR}/_ext/f9f80fec/rdrle.o: /home/build/finjin-3rd-party/cpp/jpeg/rdrle.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/f9f80fec
	${RM} "$@.d"
	$(COMPILE.c) -O2 -D_ZZIP_NO_INLINE -D_zzip_inline -I. -I../../include -I../../src -I/home/build/finjin-3rd-party/cpp/ -I/home/build/finjin-3rd-party/cpp/cpp-netlib -I/home/build/finjin-3rd-party/cpp/curl/include -I/home/build/finjin-3rd-party/cpp/civetweb/include -I/home/build/finjin-3rd-party/cpp/nowide_standalone -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/zziplib -I/home/build/finjin-3rd-party/cpp/zlib -I/home/build/finjin-3rd-party/cpp/lpng -I/home/build/finjin-3rd-party/cpp/uriparser/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f9f80fec/rdrle.o /home/build/finjin-3rd-party/cpp/jpeg/rdrle.c

${OBJECTDIR}/_ext/f9f80fec/rdswitch.o: /home/build/finjin-3rd-party/cpp/jpeg/rdswitch.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/f9f80fec
	${RM} "$@.d"
	$(COMPILE.c) -O2 -D_ZZIP_NO_INLINE -D_zzip_inline -I. -I../../include -I../../src -I/home/build/finjin-3rd-party/cpp/ -I/home/build/finjin-3rd-party/cpp/cpp-netlib -I/home/build/finjin-3rd-party/cpp/curl/include -I/home/build/finjin-3rd-party/cpp/civetweb/include -I/home/build/finjin-3rd-party/cpp/nowide_standalone -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/zziplib -I/home/build/finjin-3rd-party/cpp/zlib -I/home/build/finjin-3rd-party/cpp/lpng -I/home/build/finjin-3rd-party/cpp/uriparser/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f9f80fec/rdswitch.o /home/build/finjin-3rd-party/cpp/jpeg/rdswitch.c

${OBJECTDIR}/_ext/f9f80fec/rdtarga.o: /home/build/finjin-3rd-party/cpp/jpeg/rdtarga.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/f9f80fec
	${RM} "$@.d"
	$(COMPILE.c) -O2 -D_ZZIP_NO_INLINE -D_zzip_inline -I. -I../../include -I../../src -I/home/build/finjin-3rd-party/cpp/ -I/home/build/finjin-3rd-party/cpp/cpp-netlib -I/home/build/finjin-3rd-party/cpp/curl/include -I/home/build/finjin-3rd-party/cpp/civetweb/include -I/home/build/finjin-3rd-party/cpp/nowide_standalone -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/zziplib -I/home/build/finjin-3rd-party/cpp/zlib -I/home/build/finjin-3rd-party/cpp/lpng -I/home/build/finjin-3rd-party/cpp/uriparser/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f9f80fec/rdtarga.o /home/build/finjin-3rd-party/cpp/jpeg/rdtarga.c

${OBJECTDIR}/_ext/f9f80fec/transupp.o: /home/build/finjin-3rd-party/cpp/jpeg/transupp.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/f9f80fec
	${RM} "$@.d"
	$(COMPILE.c) -O2 -D_ZZIP_NO_INLINE -D_zzip_inline -I. -I../../include -I../../src -I/home/build/finjin-3rd-party/cpp/ -I/home/build/finjin-3rd-party/cpp/cpp-netlib -I/home/build/finjin-3rd-party/cpp/curl/include -I/home/build/finjin-3rd-party/cpp/civetweb/include -I/home/build/finjin-3rd-party/cpp/nowide_standalone -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/zziplib -I/home/build/finjin-3rd-party/cpp/zlib -I/home/build/finjin-3rd-party/cpp/lpng -I/home/build/finjin-3rd-party/cpp/uriparser/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f9f80fec/transupp.o /home/build/finjin-3rd-party/cpp/jpeg/transupp.c

${OBJECTDIR}/_ext/f9f80fec/wrbmp.o: /home/build/finjin-3rd-party/cpp/jpeg/wrbmp.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/f9f80fec
	${RM} "$@.d"
	$(COMPILE.c) -O2 -D_ZZIP_NO_INLINE -D_zzip_inline -I. -I../../include -I../../src -I/home/build/finjin-3rd-party/cpp/ -I/home/build/finjin-3rd-party/cpp/cpp-netlib -I/home/build/finjin-3rd-party/cpp/curl/include -I/home/build/finjin-3rd-party/cpp/civetweb/include -I/home/build/finjin-3rd-party/cpp/nowide_standalone -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/zziplib -I/home/build/finjin-3rd-party/cpp/zlib -I/home/build/finjin-3rd-party/cpp/lpng -I/home/build/finjin-3rd-party/cpp/uriparser/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f9f80fec/wrbmp.o /home/build/finjin-3rd-party/cpp/jpeg/wrbmp.c

${OBJECTDIR}/_ext/f9f80fec/wrgif.o: /home/build/finjin-3rd-party/cpp/jpeg/wrgif.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/f9f80fec
	${RM} "$@.d"
	$(COMPILE.c) -O2 -D_ZZIP_NO_INLINE -D_zzip_inline -I. -I../../include -I../../src -I/home/build/finjin-3rd-party/cpp/ -I/home/build/finjin-3rd-party/cpp/cpp-netlib -I/home/build/finjin-3rd-party/cpp/curl/include -I/home/build/finjin-3rd-party/cpp/civetweb/include -I/home/build/finjin-3rd-party/cpp/nowide_standalone -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/zziplib -I/home/build/finjin-3rd-party/cpp/zlib -I/home/build/finjin-3rd-party/cpp/lpng -I/home/build/finjin-3rd-party/cpp/uriparser/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f9f80fec/wrgif.o /home/build/finjin-3rd-party/cpp/jpeg/wrgif.c

${OBJECTDIR}/_ext/f9f80fec/wrppm.o: /home/build/finjin-3rd-party/cpp/jpeg/wrppm.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/f9f80fec
	${RM} "$@.d"
	$(COMPILE.c) -O2 -D_ZZIP_NO_INLINE -D_zzip_inline -I. -I../../include -I../../src -I/home/build/finjin-3rd-party/cpp/ -I/home/build/finjin-3rd-party/cpp/cpp-netlib -I/home/build/finjin-3rd-party/cpp/curl/include -I/home/build/finjin-3rd-party/cpp/civetweb/include -I/home/build/finjin-3rd-party/cpp/nowide_standalone -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/zziplib -I/home/build/finjin-3rd-party/cpp/zlib -I/home/build/finjin-3rd-party/cpp/lpng -I/home/build/finjin-3rd-party/cpp/uriparser/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f9f80fec/wrppm.o /home/build/finjin-3rd-party/cpp/jpeg/wrppm.c

${OBJECTDIR}/_ext/f9f80fec/wrrle.o: /home/build/finjin-3rd-party/cpp/jpeg/wrrle.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/f9f80fec
	${RM} "$@.d"
	$(COMPILE.c) -O2 -D_ZZIP_NO_INLINE -D_zzip_inline -I. -I../../include -I../../src -I/home/build/finjin-3rd-party/cpp/ -I/home/build/finjin-3rd-party/cpp/cpp-netlib -I/home/build/finjin-3rd-party/cpp/curl/include -I/home/build/finjin-3rd-party/cpp/civetweb/include -I/home/build/finjin-3rd-party/cpp/nowide_standalone -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/zziplib -I/home/build/finjin-3rd-party/cpp/zlib -I/home/build/finjin-3rd-party/cpp/lpng -I/home/build/finjin-3rd-party/cpp/uriparser/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f9f80fec/wrrle.o /home/build/finjin-3rd-party/cpp/jpeg/wrrle.c

${OBJECTDIR}/_ext/f9f80fec/wrtarga.o: /home/build/finjin-3rd-party/cpp/jpeg/wrtarga.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/f9f80fec
	${RM} "$@.d"
	$(COMPILE.c) -O2 -D_ZZIP_NO_INLINE -D_zzip_inline -I. -I../../include -I../../src -I/home/build/finjin-3rd-party/cpp/ -I/home/build/finjin-3rd-party/cpp/cpp-netlib -I/home/build/finjin-3rd-party/cpp/curl/include -I/home/build/finjin-3rd-party/cpp/civetweb/include -I/home/build/finjin-3rd-party/cpp/nowide_standalone -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/zziplib -I/home/build/finjin-3rd-party/cpp/zlib -I/home/build/finjin-3rd-party/cpp/lpng -I/home/build/finjin-3rd-party/cpp/uriparser/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f9f80fec/wrtarga.o /home/build/finjin-3rd-party/cpp/jpeg/wrtarga.c

${OBJECTDIR}/_ext/f9f8f9c1/png.o: /home/build/finjin-3rd-party/cpp/lpng/png.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/f9f8f9c1
	${RM} "$@.d"
	$(COMPILE.c) -O2 -D_ZZIP_NO_INLINE -D_zzip_inline -I. -I../../include -I../../src -I/home/build/finjin-3rd-party/cpp/ -I/home/build/finjin-3rd-party/cpp/cpp-netlib -I/home/build/finjin-3rd-party/cpp/curl/include -I/home/build/finjin-3rd-party/cpp/civetweb/include -I/home/build/finjin-3rd-party/cpp/nowide_standalone -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/zziplib -I/home/build/finjin-3rd-party/cpp/zlib -I/home/build/finjin-3rd-party/cpp/lpng -I/home/build/finjin-3rd-party/cpp/uriparser/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f9f8f9c1/png.o /home/build/finjin-3rd-party/cpp/lpng/png.c

${OBJECTDIR}/_ext/f9f8f9c1/pngerror.o: /home/build/finjin-3rd-party/cpp/lpng/pngerror.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/f9f8f9c1
	${RM} "$@.d"
	$(COMPILE.c) -O2 -D_ZZIP_NO_INLINE -D_zzip_inline -I. -I../../include -I../../src -I/home/build/finjin-3rd-party/cpp/ -I/home/build/finjin-3rd-party/cpp/cpp-netlib -I/home/build/finjin-3rd-party/cpp/curl/include -I/home/build/finjin-3rd-party/cpp/civetweb/include -I/home/build/finjin-3rd-party/cpp/nowide_standalone -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/zziplib -I/home/build/finjin-3rd-party/cpp/zlib -I/home/build/finjin-3rd-party/cpp/lpng -I/home/build/finjin-3rd-party/cpp/uriparser/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f9f8f9c1/pngerror.o /home/build/finjin-3rd-party/cpp/lpng/pngerror.c

${OBJECTDIR}/_ext/f9f8f9c1/pngget.o: /home/build/finjin-3rd-party/cpp/lpng/pngget.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/f9f8f9c1
	${RM} "$@.d"
	$(COMPILE.c) -O2 -D_ZZIP_NO_INLINE -D_zzip_inline -I. -I../../include -I../../src -I/home/build/finjin-3rd-party/cpp/ -I/home/build/finjin-3rd-party/cpp/cpp-netlib -I/home/build/finjin-3rd-party/cpp/curl/include -I/home/build/finjin-3rd-party/cpp/civetweb/include -I/home/build/finjin-3rd-party/cpp/nowide_standalone -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/zziplib -I/home/build/finjin-3rd-party/cpp/zlib -I/home/build/finjin-3rd-party/cpp/lpng -I/home/build/finjin-3rd-party/cpp/uriparser/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f9f8f9c1/pngget.o /home/build/finjin-3rd-party/cpp/lpng/pngget.c

${OBJECTDIR}/_ext/f9f8f9c1/pngmem.o: /home/build/finjin-3rd-party/cpp/lpng/pngmem.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/f9f8f9c1
	${RM} "$@.d"
	$(COMPILE.c) -O2 -D_ZZIP_NO_INLINE -D_zzip_inline -I. -I../../include -I../../src -I/home/build/finjin-3rd-party/cpp/ -I/home/build/finjin-3rd-party/cpp/cpp-netlib -I/home/build/finjin-3rd-party/cpp/curl/include -I/home/build/finjin-3rd-party/cpp/civetweb/include -I/home/build/finjin-3rd-party/cpp/nowide_standalone -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/zziplib -I/home/build/finjin-3rd-party/cpp/zlib -I/home/build/finjin-3rd-party/cpp/lpng -I/home/build/finjin-3rd-party/cpp/uriparser/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f9f8f9c1/pngmem.o /home/build/finjin-3rd-party/cpp/lpng/pngmem.c

${OBJECTDIR}/_ext/f9f8f9c1/pngpread.o: /home/build/finjin-3rd-party/cpp/lpng/pngpread.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/f9f8f9c1
	${RM} "$@.d"
	$(COMPILE.c) -O2 -D_ZZIP_NO_INLINE -D_zzip_inline -I. -I../../include -I../../src -I/home/build/finjin-3rd-party/cpp/ -I/home/build/finjin-3rd-party/cpp/cpp-netlib -I/home/build/finjin-3rd-party/cpp/curl/include -I/home/build/finjin-3rd-party/cpp/civetweb/include -I/home/build/finjin-3rd-party/cpp/nowide_standalone -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/zziplib -I/home/build/finjin-3rd-party/cpp/zlib -I/home/build/finjin-3rd-party/cpp/lpng -I/home/build/finjin-3rd-party/cpp/uriparser/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f9f8f9c1/pngpread.o /home/build/finjin-3rd-party/cpp/lpng/pngpread.c

${OBJECTDIR}/_ext/f9f8f9c1/pngread.o: /home/build/finjin-3rd-party/cpp/lpng/pngread.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/f9f8f9c1
	${RM} "$@.d"
	$(COMPILE.c) -O2 -D_ZZIP_NO_INLINE -D_zzip_inline -I. -I../../include -I../../src -I/home/build/finjin-3rd-party/cpp/ -I/home/build/finjin-3rd-party/cpp/cpp-netlib -I/home/build/finjin-3rd-party/cpp/curl/include -I/home/build/finjin-3rd-party/cpp/civetweb/include -I/home/build/finjin-3rd-party/cpp/nowide_standalone -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/zziplib -I/home/build/finjin-3rd-party/cpp/zlib -I/home/build/finjin-3rd-party/cpp/lpng -I/home/build/finjin-3rd-party/cpp/uriparser/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f9f8f9c1/pngread.o /home/build/finjin-3rd-party/cpp/lpng/pngread.c

${OBJECTDIR}/_ext/f9f8f9c1/pngrio.o: /home/build/finjin-3rd-party/cpp/lpng/pngrio.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/f9f8f9c1
	${RM} "$@.d"
	$(COMPILE.c) -O2 -D_ZZIP_NO_INLINE -D_zzip_inline -I. -I../../include -I../../src -I/home/build/finjin-3rd-party/cpp/ -I/home/build/finjin-3rd-party/cpp/cpp-netlib -I/home/build/finjin-3rd-party/cpp/curl/include -I/home/build/finjin-3rd-party/cpp/civetweb/include -I/home/build/finjin-3rd-party/cpp/nowide_standalone -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/zziplib -I/home/build/finjin-3rd-party/cpp/zlib -I/home/build/finjin-3rd-party/cpp/lpng -I/home/build/finjin-3rd-party/cpp/uriparser/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f9f8f9c1/pngrio.o /home/build/finjin-3rd-party/cpp/lpng/pngrio.c

${OBJECTDIR}/_ext/f9f8f9c1/pngrtran.o: /home/build/finjin-3rd-party/cpp/lpng/pngrtran.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/f9f8f9c1
	${RM} "$@.d"
	$(COMPILE.c) -O2 -D_ZZIP_NO_INLINE -D_zzip_inline -I. -I../../include -I../../src -I/home/build/finjin-3rd-party/cpp/ -I/home/build/finjin-3rd-party/cpp/cpp-netlib -I/home/build/finjin-3rd-party/cpp/curl/include -I/home/build/finjin-3rd-party/cpp/civetweb/include -I/home/build/finjin-3rd-party/cpp/nowide_standalone -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/zziplib -I/home/build/finjin-3rd-party/cpp/zlib -I/home/build/finjin-3rd-party/cpp/lpng -I/home/build/finjin-3rd-party/cpp/uriparser/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f9f8f9c1/pngrtran.o /home/build/finjin-3rd-party/cpp/lpng/pngrtran.c

${OBJECTDIR}/_ext/f9f8f9c1/pngrutil.o: /home/build/finjin-3rd-party/cpp/lpng/pngrutil.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/f9f8f9c1
	${RM} "$@.d"
	$(COMPILE.c) -O2 -D_ZZIP_NO_INLINE -D_zzip_inline -I. -I../../include -I../../src -I/home/build/finjin-3rd-party/cpp/ -I/home/build/finjin-3rd-party/cpp/cpp-netlib -I/home/build/finjin-3rd-party/cpp/curl/include -I/home/build/finjin-3rd-party/cpp/civetweb/include -I/home/build/finjin-3rd-party/cpp/nowide_standalone -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/zziplib -I/home/build/finjin-3rd-party/cpp/zlib -I/home/build/finjin-3rd-party/cpp/lpng -I/home/build/finjin-3rd-party/cpp/uriparser/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f9f8f9c1/pngrutil.o /home/build/finjin-3rd-party/cpp/lpng/pngrutil.c

${OBJECTDIR}/_ext/f9f8f9c1/pngset.o: /home/build/finjin-3rd-party/cpp/lpng/pngset.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/f9f8f9c1
	${RM} "$@.d"
	$(COMPILE.c) -O2 -D_ZZIP_NO_INLINE -D_zzip_inline -I. -I../../include -I../../src -I/home/build/finjin-3rd-party/cpp/ -I/home/build/finjin-3rd-party/cpp/cpp-netlib -I/home/build/finjin-3rd-party/cpp/curl/include -I/home/build/finjin-3rd-party/cpp/civetweb/include -I/home/build/finjin-3rd-party/cpp/nowide_standalone -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/zziplib -I/home/build/finjin-3rd-party/cpp/zlib -I/home/build/finjin-3rd-party/cpp/lpng -I/home/build/finjin-3rd-party/cpp/uriparser/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f9f8f9c1/pngset.o /home/build/finjin-3rd-party/cpp/lpng/pngset.c

${OBJECTDIR}/_ext/f9f8f9c1/pngtrans.o: /home/build/finjin-3rd-party/cpp/lpng/pngtrans.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/f9f8f9c1
	${RM} "$@.d"
	$(COMPILE.c) -O2 -D_ZZIP_NO_INLINE -D_zzip_inline -I. -I../../include -I../../src -I/home/build/finjin-3rd-party/cpp/ -I/home/build/finjin-3rd-party/cpp/cpp-netlib -I/home/build/finjin-3rd-party/cpp/curl/include -I/home/build/finjin-3rd-party/cpp/civetweb/include -I/home/build/finjin-3rd-party/cpp/nowide_standalone -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/zziplib -I/home/build/finjin-3rd-party/cpp/zlib -I/home/build/finjin-3rd-party/cpp/lpng -I/home/build/finjin-3rd-party/cpp/uriparser/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f9f8f9c1/pngtrans.o /home/build/finjin-3rd-party/cpp/lpng/pngtrans.c

${OBJECTDIR}/_ext/f9f8f9c1/pngwio.o: /home/build/finjin-3rd-party/cpp/lpng/pngwio.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/f9f8f9c1
	${RM} "$@.d"
	$(COMPILE.c) -O2 -D_ZZIP_NO_INLINE -D_zzip_inline -I. -I../../include -I../../src -I/home/build/finjin-3rd-party/cpp/ -I/home/build/finjin-3rd-party/cpp/cpp-netlib -I/home/build/finjin-3rd-party/cpp/curl/include -I/home/build/finjin-3rd-party/cpp/civetweb/include -I/home/build/finjin-3rd-party/cpp/nowide_standalone -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/zziplib -I/home/build/finjin-3rd-party/cpp/zlib -I/home/build/finjin-3rd-party/cpp/lpng -I/home/build/finjin-3rd-party/cpp/uriparser/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f9f8f9c1/pngwio.o /home/build/finjin-3rd-party/cpp/lpng/pngwio.c

${OBJECTDIR}/_ext/f9f8f9c1/pngwrite.o: /home/build/finjin-3rd-party/cpp/lpng/pngwrite.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/f9f8f9c1
	${RM} "$@.d"
	$(COMPILE.c) -O2 -D_ZZIP_NO_INLINE -D_zzip_inline -I. -I../../include -I../../src -I/home/build/finjin-3rd-party/cpp/ -I/home/build/finjin-3rd-party/cpp/cpp-netlib -I/home/build/finjin-3rd-party/cpp/curl/include -I/home/build/finjin-3rd-party/cpp/civetweb/include -I/home/build/finjin-3rd-party/cpp/nowide_standalone -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/zziplib -I/home/build/finjin-3rd-party/cpp/zlib -I/home/build/finjin-3rd-party/cpp/lpng -I/home/build/finjin-3rd-party/cpp/uriparser/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f9f8f9c1/pngwrite.o /home/build/finjin-3rd-party/cpp/lpng/pngwrite.c

${OBJECTDIR}/_ext/f9f8f9c1/pngwtran.o: /home/build/finjin-3rd-party/cpp/lpng/pngwtran.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/f9f8f9c1
	${RM} "$@.d"
	$(COMPILE.c) -O2 -D_ZZIP_NO_INLINE -D_zzip_inline -I. -I../../include -I../../src -I/home/build/finjin-3rd-party/cpp/ -I/home/build/finjin-3rd-party/cpp/cpp-netlib -I/home/build/finjin-3rd-party/cpp/curl/include -I/home/build/finjin-3rd-party/cpp/civetweb/include -I/home/build/finjin-3rd-party/cpp/nowide_standalone -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/zziplib -I/home/build/finjin-3rd-party/cpp/zlib -I/home/build/finjin-3rd-party/cpp/lpng -I/home/build/finjin-3rd-party/cpp/uriparser/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f9f8f9c1/pngwtran.o /home/build/finjin-3rd-party/cpp/lpng/pngwtran.c

${OBJECTDIR}/_ext/f9f8f9c1/pngwutil.o: /home/build/finjin-3rd-party/cpp/lpng/pngwutil.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/f9f8f9c1
	${RM} "$@.d"
	$(COMPILE.c) -O2 -D_ZZIP_NO_INLINE -D_zzip_inline -I. -I../../include -I../../src -I/home/build/finjin-3rd-party/cpp/ -I/home/build/finjin-3rd-party/cpp/cpp-netlib -I/home/build/finjin-3rd-party/cpp/curl/include -I/home/build/finjin-3rd-party/cpp/civetweb/include -I/home/build/finjin-3rd-party/cpp/nowide_standalone -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/zziplib -I/home/build/finjin-3rd-party/cpp/zlib -I/home/build/finjin-3rd-party/cpp/lpng -I/home/build/finjin-3rd-party/cpp/uriparser/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f9f8f9c1/pngwutil.o /home/build/finjin-3rd-party/cpp/lpng/pngwutil.c

${OBJECTDIR}/_ext/a859e9a1/iostream.o: /home/build/finjin-3rd-party/cpp/nowide_standalone/src/iostream.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/a859e9a1
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -D_ZZIP_NO_INLINE -D_zzip_inline -I. -I../../include -I../../src -I/home/build/finjin-3rd-party/cpp/ -I/home/build/finjin-3rd-party/cpp/nowide_standalone -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/zziplib -I/home/build/finjin-3rd-party/cpp/zlib -I/home/build/finjin-3rd-party/cpp/lpng -I/home/build/finjin-3rd-party/cpp/uriparser/include -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/a859e9a1/iostream.o /home/build/finjin-3rd-party/cpp/nowide_standalone/src/iostream.cpp

${OBJECTDIR}/_ext/2fda0f13/tinyxml2.o: /home/build/finjin-3rd-party/cpp/tinyxml/tinyxml2.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/2fda0f13
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -D_ZZIP_NO_INLINE -D_zzip_inline -I. -I../../include -I../../src -I/home/build/finjin-3rd-party/cpp/ -I/home/build/finjin-3rd-party/cpp/nowide_standalone -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/zziplib -I/home/build/finjin-3rd-party/cpp/zlib -I/home/build/finjin-3rd-party/cpp/lpng -I/home/build/finjin-3rd-party/cpp/uriparser/include -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/2fda0f13/tinyxml2.o /home/build/finjin-3rd-party/cpp/tinyxml/tinyxml2.cpp

${OBJECTDIR}/_ext/5de1235c/UriCommon.o: /home/build/finjin-3rd-party/cpp/uriparser/src/UriCommon.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/5de1235c
	${RM} "$@.d"
	$(COMPILE.c) -O2 -D_ZZIP_NO_INLINE -D_zzip_inline -I. -I../../include -I../../src -I/home/build/finjin-3rd-party/cpp/ -I/home/build/finjin-3rd-party/cpp/cpp-netlib -I/home/build/finjin-3rd-party/cpp/curl/include -I/home/build/finjin-3rd-party/cpp/civetweb/include -I/home/build/finjin-3rd-party/cpp/nowide_standalone -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/zziplib -I/home/build/finjin-3rd-party/cpp/zlib -I/home/build/finjin-3rd-party/cpp/lpng -I/home/build/finjin-3rd-party/cpp/uriparser/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/5de1235c/UriCommon.o /home/build/finjin-3rd-party/cpp/uriparser/src/UriCommon.c

${OBJECTDIR}/_ext/5de1235c/UriCompare.o: /home/build/finjin-3rd-party/cpp/uriparser/src/UriCompare.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/5de1235c
	${RM} "$@.d"
	$(COMPILE.c) -O2 -D_ZZIP_NO_INLINE -D_zzip_inline -I. -I../../include -I../../src -I/home/build/finjin-3rd-party/cpp/ -I/home/build/finjin-3rd-party/cpp/cpp-netlib -I/home/build/finjin-3rd-party/cpp/curl/include -I/home/build/finjin-3rd-party/cpp/civetweb/include -I/home/build/finjin-3rd-party/cpp/nowide_standalone -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/zziplib -I/home/build/finjin-3rd-party/cpp/zlib -I/home/build/finjin-3rd-party/cpp/lpng -I/home/build/finjin-3rd-party/cpp/uriparser/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/5de1235c/UriCompare.o /home/build/finjin-3rd-party/cpp/uriparser/src/UriCompare.c

${OBJECTDIR}/_ext/5de1235c/UriEscape.o: /home/build/finjin-3rd-party/cpp/uriparser/src/UriEscape.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/5de1235c
	${RM} "$@.d"
	$(COMPILE.c) -O2 -D_ZZIP_NO_INLINE -D_zzip_inline -I. -I../../include -I../../src -I/home/build/finjin-3rd-party/cpp/ -I/home/build/finjin-3rd-party/cpp/cpp-netlib -I/home/build/finjin-3rd-party/cpp/curl/include -I/home/build/finjin-3rd-party/cpp/civetweb/include -I/home/build/finjin-3rd-party/cpp/nowide_standalone -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/zziplib -I/home/build/finjin-3rd-party/cpp/zlib -I/home/build/finjin-3rd-party/cpp/lpng -I/home/build/finjin-3rd-party/cpp/uriparser/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/5de1235c/UriEscape.o /home/build/finjin-3rd-party/cpp/uriparser/src/UriEscape.c

${OBJECTDIR}/_ext/5de1235c/UriFile.o: /home/build/finjin-3rd-party/cpp/uriparser/src/UriFile.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/5de1235c
	${RM} "$@.d"
	$(COMPILE.c) -O2 -D_ZZIP_NO_INLINE -D_zzip_inline -I. -I../../include -I../../src -I/home/build/finjin-3rd-party/cpp/ -I/home/build/finjin-3rd-party/cpp/cpp-netlib -I/home/build/finjin-3rd-party/cpp/curl/include -I/home/build/finjin-3rd-party/cpp/civetweb/include -I/home/build/finjin-3rd-party/cpp/nowide_standalone -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/zziplib -I/home/build/finjin-3rd-party/cpp/zlib -I/home/build/finjin-3rd-party/cpp/lpng -I/home/build/finjin-3rd-party/cpp/uriparser/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/5de1235c/UriFile.o /home/build/finjin-3rd-party/cpp/uriparser/src/UriFile.c

${OBJECTDIR}/_ext/5de1235c/UriIp4.o: /home/build/finjin-3rd-party/cpp/uriparser/src/UriIp4.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/5de1235c
	${RM} "$@.d"
	$(COMPILE.c) -O2 -D_ZZIP_NO_INLINE -D_zzip_inline -I. -I../../include -I../../src -I/home/build/finjin-3rd-party/cpp/ -I/home/build/finjin-3rd-party/cpp/cpp-netlib -I/home/build/finjin-3rd-party/cpp/curl/include -I/home/build/finjin-3rd-party/cpp/civetweb/include -I/home/build/finjin-3rd-party/cpp/nowide_standalone -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/zziplib -I/home/build/finjin-3rd-party/cpp/zlib -I/home/build/finjin-3rd-party/cpp/lpng -I/home/build/finjin-3rd-party/cpp/uriparser/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/5de1235c/UriIp4.o /home/build/finjin-3rd-party/cpp/uriparser/src/UriIp4.c

${OBJECTDIR}/_ext/5de1235c/UriIp4Base.o: /home/build/finjin-3rd-party/cpp/uriparser/src/UriIp4Base.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/5de1235c
	${RM} "$@.d"
	$(COMPILE.c) -O2 -D_ZZIP_NO_INLINE -D_zzip_inline -I. -I../../include -I../../src -I/home/build/finjin-3rd-party/cpp/ -I/home/build/finjin-3rd-party/cpp/cpp-netlib -I/home/build/finjin-3rd-party/cpp/curl/include -I/home/build/finjin-3rd-party/cpp/civetweb/include -I/home/build/finjin-3rd-party/cpp/nowide_standalone -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/zziplib -I/home/build/finjin-3rd-party/cpp/zlib -I/home/build/finjin-3rd-party/cpp/lpng -I/home/build/finjin-3rd-party/cpp/uriparser/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/5de1235c/UriIp4Base.o /home/build/finjin-3rd-party/cpp/uriparser/src/UriIp4Base.c

${OBJECTDIR}/_ext/5de1235c/UriNormalize.o: /home/build/finjin-3rd-party/cpp/uriparser/src/UriNormalize.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/5de1235c
	${RM} "$@.d"
	$(COMPILE.c) -O2 -D_ZZIP_NO_INLINE -D_zzip_inline -I. -I../../include -I../../src -I/home/build/finjin-3rd-party/cpp/ -I/home/build/finjin-3rd-party/cpp/cpp-netlib -I/home/build/finjin-3rd-party/cpp/curl/include -I/home/build/finjin-3rd-party/cpp/civetweb/include -I/home/build/finjin-3rd-party/cpp/nowide_standalone -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/zziplib -I/home/build/finjin-3rd-party/cpp/zlib -I/home/build/finjin-3rd-party/cpp/lpng -I/home/build/finjin-3rd-party/cpp/uriparser/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/5de1235c/UriNormalize.o /home/build/finjin-3rd-party/cpp/uriparser/src/UriNormalize.c

${OBJECTDIR}/_ext/5de1235c/UriNormalizeBase.o: /home/build/finjin-3rd-party/cpp/uriparser/src/UriNormalizeBase.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/5de1235c
	${RM} "$@.d"
	$(COMPILE.c) -O2 -D_ZZIP_NO_INLINE -D_zzip_inline -I. -I../../include -I../../src -I/home/build/finjin-3rd-party/cpp/ -I/home/build/finjin-3rd-party/cpp/cpp-netlib -I/home/build/finjin-3rd-party/cpp/curl/include -I/home/build/finjin-3rd-party/cpp/civetweb/include -I/home/build/finjin-3rd-party/cpp/nowide_standalone -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/zziplib -I/home/build/finjin-3rd-party/cpp/zlib -I/home/build/finjin-3rd-party/cpp/lpng -I/home/build/finjin-3rd-party/cpp/uriparser/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/5de1235c/UriNormalizeBase.o /home/build/finjin-3rd-party/cpp/uriparser/src/UriNormalizeBase.c

${OBJECTDIR}/_ext/5de1235c/UriParse.o: /home/build/finjin-3rd-party/cpp/uriparser/src/UriParse.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/5de1235c
	${RM} "$@.d"
	$(COMPILE.c) -O2 -D_ZZIP_NO_INLINE -D_zzip_inline -I. -I../../include -I../../src -I/home/build/finjin-3rd-party/cpp/ -I/home/build/finjin-3rd-party/cpp/cpp-netlib -I/home/build/finjin-3rd-party/cpp/curl/include -I/home/build/finjin-3rd-party/cpp/civetweb/include -I/home/build/finjin-3rd-party/cpp/nowide_standalone -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/zziplib -I/home/build/finjin-3rd-party/cpp/zlib -I/home/build/finjin-3rd-party/cpp/lpng -I/home/build/finjin-3rd-party/cpp/uriparser/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/5de1235c/UriParse.o /home/build/finjin-3rd-party/cpp/uriparser/src/UriParse.c

${OBJECTDIR}/_ext/5de1235c/UriParseBase.o: /home/build/finjin-3rd-party/cpp/uriparser/src/UriParseBase.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/5de1235c
	${RM} "$@.d"
	$(COMPILE.c) -O2 -D_ZZIP_NO_INLINE -D_zzip_inline -I. -I../../include -I../../src -I/home/build/finjin-3rd-party/cpp/ -I/home/build/finjin-3rd-party/cpp/cpp-netlib -I/home/build/finjin-3rd-party/cpp/curl/include -I/home/build/finjin-3rd-party/cpp/civetweb/include -I/home/build/finjin-3rd-party/cpp/nowide_standalone -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/zziplib -I/home/build/finjin-3rd-party/cpp/zlib -I/home/build/finjin-3rd-party/cpp/lpng -I/home/build/finjin-3rd-party/cpp/uriparser/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/5de1235c/UriParseBase.o /home/build/finjin-3rd-party/cpp/uriparser/src/UriParseBase.c

${OBJECTDIR}/_ext/5de1235c/UriQuery.o: /home/build/finjin-3rd-party/cpp/uriparser/src/UriQuery.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/5de1235c
	${RM} "$@.d"
	$(COMPILE.c) -O2 -D_ZZIP_NO_INLINE -D_zzip_inline -I. -I../../include -I../../src -I/home/build/finjin-3rd-party/cpp/ -I/home/build/finjin-3rd-party/cpp/cpp-netlib -I/home/build/finjin-3rd-party/cpp/curl/include -I/home/build/finjin-3rd-party/cpp/civetweb/include -I/home/build/finjin-3rd-party/cpp/nowide_standalone -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/zziplib -I/home/build/finjin-3rd-party/cpp/zlib -I/home/build/finjin-3rd-party/cpp/lpng -I/home/build/finjin-3rd-party/cpp/uriparser/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/5de1235c/UriQuery.o /home/build/finjin-3rd-party/cpp/uriparser/src/UriQuery.c

${OBJECTDIR}/_ext/5de1235c/UriRecompose.o: /home/build/finjin-3rd-party/cpp/uriparser/src/UriRecompose.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/5de1235c
	${RM} "$@.d"
	$(COMPILE.c) -O2 -D_ZZIP_NO_INLINE -D_zzip_inline -I. -I../../include -I../../src -I/home/build/finjin-3rd-party/cpp/ -I/home/build/finjin-3rd-party/cpp/cpp-netlib -I/home/build/finjin-3rd-party/cpp/curl/include -I/home/build/finjin-3rd-party/cpp/civetweb/include -I/home/build/finjin-3rd-party/cpp/nowide_standalone -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/zziplib -I/home/build/finjin-3rd-party/cpp/zlib -I/home/build/finjin-3rd-party/cpp/lpng -I/home/build/finjin-3rd-party/cpp/uriparser/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/5de1235c/UriRecompose.o /home/build/finjin-3rd-party/cpp/uriparser/src/UriRecompose.c

${OBJECTDIR}/_ext/5de1235c/UriResolve.o: /home/build/finjin-3rd-party/cpp/uriparser/src/UriResolve.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/5de1235c
	${RM} "$@.d"
	$(COMPILE.c) -O2 -D_ZZIP_NO_INLINE -D_zzip_inline -I. -I../../include -I../../src -I/home/build/finjin-3rd-party/cpp/ -I/home/build/finjin-3rd-party/cpp/cpp-netlib -I/home/build/finjin-3rd-party/cpp/curl/include -I/home/build/finjin-3rd-party/cpp/civetweb/include -I/home/build/finjin-3rd-party/cpp/nowide_standalone -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/zziplib -I/home/build/finjin-3rd-party/cpp/zlib -I/home/build/finjin-3rd-party/cpp/lpng -I/home/build/finjin-3rd-party/cpp/uriparser/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/5de1235c/UriResolve.o /home/build/finjin-3rd-party/cpp/uriparser/src/UriResolve.c

${OBJECTDIR}/_ext/5de1235c/UriShorten.o: /home/build/finjin-3rd-party/cpp/uriparser/src/UriShorten.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/5de1235c
	${RM} "$@.d"
	$(COMPILE.c) -O2 -D_ZZIP_NO_INLINE -D_zzip_inline -I. -I../../include -I../../src -I/home/build/finjin-3rd-party/cpp/ -I/home/build/finjin-3rd-party/cpp/cpp-netlib -I/home/build/finjin-3rd-party/cpp/curl/include -I/home/build/finjin-3rd-party/cpp/civetweb/include -I/home/build/finjin-3rd-party/cpp/nowide_standalone -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/zziplib -I/home/build/finjin-3rd-party/cpp/zlib -I/home/build/finjin-3rd-party/cpp/lpng -I/home/build/finjin-3rd-party/cpp/uriparser/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/5de1235c/UriShorten.o /home/build/finjin-3rd-party/cpp/uriparser/src/UriShorten.c

${OBJECTDIR}/_ext/747b22b2/xxhash.o: /home/build/finjin-3rd-party/cpp/xxHash/xxhash.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/747b22b2
	${RM} "$@.d"
	$(COMPILE.c) -O2 -D_ZZIP_NO_INLINE -D_zzip_inline -I. -I../../include -I../../src -I/home/build/finjin-3rd-party/cpp/ -I/home/build/finjin-3rd-party/cpp/cpp-netlib -I/home/build/finjin-3rd-party/cpp/curl/include -I/home/build/finjin-3rd-party/cpp/civetweb/include -I/home/build/finjin-3rd-party/cpp/nowide_standalone -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/zziplib -I/home/build/finjin-3rd-party/cpp/zlib -I/home/build/finjin-3rd-party/cpp/lpng -I/home/build/finjin-3rd-party/cpp/uriparser/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/747b22b2/xxhash.o /home/build/finjin-3rd-party/cpp/xxHash/xxhash.c

${OBJECTDIR}/_ext/f9ff474f/adler32.o: /home/build/finjin-3rd-party/cpp/zlib/adler32.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/f9ff474f
	${RM} "$@.d"
	$(COMPILE.c) -O2 -D_ZZIP_NO_INLINE -D_zzip_inline -I. -I../../include -I../../src -I/home/build/finjin-3rd-party/cpp/ -I/home/build/finjin-3rd-party/cpp/cpp-netlib -I/home/build/finjin-3rd-party/cpp/curl/include -I/home/build/finjin-3rd-party/cpp/civetweb/include -I/home/build/finjin-3rd-party/cpp/nowide_standalone -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/zziplib -I/home/build/finjin-3rd-party/cpp/zlib -I/home/build/finjin-3rd-party/cpp/lpng -I/home/build/finjin-3rd-party/cpp/uriparser/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f9ff474f/adler32.o /home/build/finjin-3rd-party/cpp/zlib/adler32.c

${OBJECTDIR}/_ext/f9ff474f/compress.o: /home/build/finjin-3rd-party/cpp/zlib/compress.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/f9ff474f
	${RM} "$@.d"
	$(COMPILE.c) -O2 -D_ZZIP_NO_INLINE -D_zzip_inline -I. -I../../include -I../../src -I/home/build/finjin-3rd-party/cpp/ -I/home/build/finjin-3rd-party/cpp/cpp-netlib -I/home/build/finjin-3rd-party/cpp/curl/include -I/home/build/finjin-3rd-party/cpp/civetweb/include -I/home/build/finjin-3rd-party/cpp/nowide_standalone -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/zziplib -I/home/build/finjin-3rd-party/cpp/zlib -I/home/build/finjin-3rd-party/cpp/lpng -I/home/build/finjin-3rd-party/cpp/uriparser/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f9ff474f/compress.o /home/build/finjin-3rd-party/cpp/zlib/compress.c

${OBJECTDIR}/_ext/f9ff474f/crc32.o: /home/build/finjin-3rd-party/cpp/zlib/crc32.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/f9ff474f
	${RM} "$@.d"
	$(COMPILE.c) -O2 -D_ZZIP_NO_INLINE -D_zzip_inline -I. -I../../include -I../../src -I/home/build/finjin-3rd-party/cpp/ -I/home/build/finjin-3rd-party/cpp/cpp-netlib -I/home/build/finjin-3rd-party/cpp/curl/include -I/home/build/finjin-3rd-party/cpp/civetweb/include -I/home/build/finjin-3rd-party/cpp/nowide_standalone -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/zziplib -I/home/build/finjin-3rd-party/cpp/zlib -I/home/build/finjin-3rd-party/cpp/lpng -I/home/build/finjin-3rd-party/cpp/uriparser/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f9ff474f/crc32.o /home/build/finjin-3rd-party/cpp/zlib/crc32.c

${OBJECTDIR}/_ext/f9ff474f/deflate.o: /home/build/finjin-3rd-party/cpp/zlib/deflate.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/f9ff474f
	${RM} "$@.d"
	$(COMPILE.c) -O2 -D_ZZIP_NO_INLINE -D_zzip_inline -I. -I../../include -I../../src -I/home/build/finjin-3rd-party/cpp/ -I/home/build/finjin-3rd-party/cpp/cpp-netlib -I/home/build/finjin-3rd-party/cpp/curl/include -I/home/build/finjin-3rd-party/cpp/civetweb/include -I/home/build/finjin-3rd-party/cpp/nowide_standalone -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/zziplib -I/home/build/finjin-3rd-party/cpp/zlib -I/home/build/finjin-3rd-party/cpp/lpng -I/home/build/finjin-3rd-party/cpp/uriparser/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f9ff474f/deflate.o /home/build/finjin-3rd-party/cpp/zlib/deflate.c

${OBJECTDIR}/_ext/f9ff474f/gzclose.o: /home/build/finjin-3rd-party/cpp/zlib/gzclose.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/f9ff474f
	${RM} "$@.d"
	$(COMPILE.c) -O2 -D_ZZIP_NO_INLINE -D_zzip_inline -I. -I../../include -I../../src -I/home/build/finjin-3rd-party/cpp/ -I/home/build/finjin-3rd-party/cpp/cpp-netlib -I/home/build/finjin-3rd-party/cpp/curl/include -I/home/build/finjin-3rd-party/cpp/civetweb/include -I/home/build/finjin-3rd-party/cpp/nowide_standalone -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/zziplib -I/home/build/finjin-3rd-party/cpp/zlib -I/home/build/finjin-3rd-party/cpp/lpng -I/home/build/finjin-3rd-party/cpp/uriparser/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f9ff474f/gzclose.o /home/build/finjin-3rd-party/cpp/zlib/gzclose.c

${OBJECTDIR}/_ext/f9ff474f/gzlib.o: /home/build/finjin-3rd-party/cpp/zlib/gzlib.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/f9ff474f
	${RM} "$@.d"
	$(COMPILE.c) -O2 -D_ZZIP_NO_INLINE -D_zzip_inline -I. -I../../include -I../../src -I/home/build/finjin-3rd-party/cpp/ -I/home/build/finjin-3rd-party/cpp/cpp-netlib -I/home/build/finjin-3rd-party/cpp/curl/include -I/home/build/finjin-3rd-party/cpp/civetweb/include -I/home/build/finjin-3rd-party/cpp/nowide_standalone -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/zziplib -I/home/build/finjin-3rd-party/cpp/zlib -I/home/build/finjin-3rd-party/cpp/lpng -I/home/build/finjin-3rd-party/cpp/uriparser/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f9ff474f/gzlib.o /home/build/finjin-3rd-party/cpp/zlib/gzlib.c

${OBJECTDIR}/_ext/f9ff474f/gzread.o: /home/build/finjin-3rd-party/cpp/zlib/gzread.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/f9ff474f
	${RM} "$@.d"
	$(COMPILE.c) -O2 -D_ZZIP_NO_INLINE -D_zzip_inline -I. -I../../include -I../../src -I/home/build/finjin-3rd-party/cpp/ -I/home/build/finjin-3rd-party/cpp/cpp-netlib -I/home/build/finjin-3rd-party/cpp/curl/include -I/home/build/finjin-3rd-party/cpp/civetweb/include -I/home/build/finjin-3rd-party/cpp/nowide_standalone -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/zziplib -I/home/build/finjin-3rd-party/cpp/zlib -I/home/build/finjin-3rd-party/cpp/lpng -I/home/build/finjin-3rd-party/cpp/uriparser/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f9ff474f/gzread.o /home/build/finjin-3rd-party/cpp/zlib/gzread.c

${OBJECTDIR}/_ext/f9ff474f/gzwrite.o: /home/build/finjin-3rd-party/cpp/zlib/gzwrite.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/f9ff474f
	${RM} "$@.d"
	$(COMPILE.c) -O2 -D_ZZIP_NO_INLINE -D_zzip_inline -I. -I../../include -I../../src -I/home/build/finjin-3rd-party/cpp/ -I/home/build/finjin-3rd-party/cpp/cpp-netlib -I/home/build/finjin-3rd-party/cpp/curl/include -I/home/build/finjin-3rd-party/cpp/civetweb/include -I/home/build/finjin-3rd-party/cpp/nowide_standalone -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/zziplib -I/home/build/finjin-3rd-party/cpp/zlib -I/home/build/finjin-3rd-party/cpp/lpng -I/home/build/finjin-3rd-party/cpp/uriparser/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f9ff474f/gzwrite.o /home/build/finjin-3rd-party/cpp/zlib/gzwrite.c

${OBJECTDIR}/_ext/f9ff474f/infback.o: /home/build/finjin-3rd-party/cpp/zlib/infback.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/f9ff474f
	${RM} "$@.d"
	$(COMPILE.c) -O2 -D_ZZIP_NO_INLINE -D_zzip_inline -I. -I../../include -I../../src -I/home/build/finjin-3rd-party/cpp/ -I/home/build/finjin-3rd-party/cpp/cpp-netlib -I/home/build/finjin-3rd-party/cpp/curl/include -I/home/build/finjin-3rd-party/cpp/civetweb/include -I/home/build/finjin-3rd-party/cpp/nowide_standalone -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/zziplib -I/home/build/finjin-3rd-party/cpp/zlib -I/home/build/finjin-3rd-party/cpp/lpng -I/home/build/finjin-3rd-party/cpp/uriparser/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f9ff474f/infback.o /home/build/finjin-3rd-party/cpp/zlib/infback.c

${OBJECTDIR}/_ext/f9ff474f/inffast.o: /home/build/finjin-3rd-party/cpp/zlib/inffast.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/f9ff474f
	${RM} "$@.d"
	$(COMPILE.c) -O2 -D_ZZIP_NO_INLINE -D_zzip_inline -I. -I../../include -I../../src -I/home/build/finjin-3rd-party/cpp/ -I/home/build/finjin-3rd-party/cpp/cpp-netlib -I/home/build/finjin-3rd-party/cpp/curl/include -I/home/build/finjin-3rd-party/cpp/civetweb/include -I/home/build/finjin-3rd-party/cpp/nowide_standalone -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/zziplib -I/home/build/finjin-3rd-party/cpp/zlib -I/home/build/finjin-3rd-party/cpp/lpng -I/home/build/finjin-3rd-party/cpp/uriparser/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f9ff474f/inffast.o /home/build/finjin-3rd-party/cpp/zlib/inffast.c

${OBJECTDIR}/_ext/f9ff474f/inflate.o: /home/build/finjin-3rd-party/cpp/zlib/inflate.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/f9ff474f
	${RM} "$@.d"
	$(COMPILE.c) -O2 -D_ZZIP_NO_INLINE -D_zzip_inline -I. -I../../include -I../../src -I/home/build/finjin-3rd-party/cpp/ -I/home/build/finjin-3rd-party/cpp/cpp-netlib -I/home/build/finjin-3rd-party/cpp/curl/include -I/home/build/finjin-3rd-party/cpp/civetweb/include -I/home/build/finjin-3rd-party/cpp/nowide_standalone -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/zziplib -I/home/build/finjin-3rd-party/cpp/zlib -I/home/build/finjin-3rd-party/cpp/lpng -I/home/build/finjin-3rd-party/cpp/uriparser/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f9ff474f/inflate.o /home/build/finjin-3rd-party/cpp/zlib/inflate.c

${OBJECTDIR}/_ext/f9ff474f/inftrees.o: /home/build/finjin-3rd-party/cpp/zlib/inftrees.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/f9ff474f
	${RM} "$@.d"
	$(COMPILE.c) -O2 -D_ZZIP_NO_INLINE -D_zzip_inline -I. -I../../include -I../../src -I/home/build/finjin-3rd-party/cpp/ -I/home/build/finjin-3rd-party/cpp/cpp-netlib -I/home/build/finjin-3rd-party/cpp/curl/include -I/home/build/finjin-3rd-party/cpp/civetweb/include -I/home/build/finjin-3rd-party/cpp/nowide_standalone -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/zziplib -I/home/build/finjin-3rd-party/cpp/zlib -I/home/build/finjin-3rd-party/cpp/lpng -I/home/build/finjin-3rd-party/cpp/uriparser/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f9ff474f/inftrees.o /home/build/finjin-3rd-party/cpp/zlib/inftrees.c

${OBJECTDIR}/_ext/f9ff474f/trees.o: /home/build/finjin-3rd-party/cpp/zlib/trees.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/f9ff474f
	${RM} "$@.d"
	$(COMPILE.c) -O2 -D_ZZIP_NO_INLINE -D_zzip_inline -I. -I../../include -I../../src -I/home/build/finjin-3rd-party/cpp/ -I/home/build/finjin-3rd-party/cpp/cpp-netlib -I/home/build/finjin-3rd-party/cpp/curl/include -I/home/build/finjin-3rd-party/cpp/civetweb/include -I/home/build/finjin-3rd-party/cpp/nowide_standalone -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/zziplib -I/home/build/finjin-3rd-party/cpp/zlib -I/home/build/finjin-3rd-party/cpp/lpng -I/home/build/finjin-3rd-party/cpp/uriparser/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f9ff474f/trees.o /home/build/finjin-3rd-party/cpp/zlib/trees.c

${OBJECTDIR}/_ext/f9ff474f/uncompr.o: /home/build/finjin-3rd-party/cpp/zlib/uncompr.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/f9ff474f
	${RM} "$@.d"
	$(COMPILE.c) -O2 -D_ZZIP_NO_INLINE -D_zzip_inline -I. -I../../include -I../../src -I/home/build/finjin-3rd-party/cpp/ -I/home/build/finjin-3rd-party/cpp/cpp-netlib -I/home/build/finjin-3rd-party/cpp/curl/include -I/home/build/finjin-3rd-party/cpp/civetweb/include -I/home/build/finjin-3rd-party/cpp/nowide_standalone -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/zziplib -I/home/build/finjin-3rd-party/cpp/zlib -I/home/build/finjin-3rd-party/cpp/lpng -I/home/build/finjin-3rd-party/cpp/uriparser/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f9ff474f/uncompr.o /home/build/finjin-3rd-party/cpp/zlib/uncompr.c

${OBJECTDIR}/_ext/f9ff474f/zutil.o: /home/build/finjin-3rd-party/cpp/zlib/zutil.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/f9ff474f
	${RM} "$@.d"
	$(COMPILE.c) -O2 -D_ZZIP_NO_INLINE -D_zzip_inline -I. -I../../include -I../../src -I/home/build/finjin-3rd-party/cpp/ -I/home/build/finjin-3rd-party/cpp/cpp-netlib -I/home/build/finjin-3rd-party/cpp/curl/include -I/home/build/finjin-3rd-party/cpp/civetweb/include -I/home/build/finjin-3rd-party/cpp/nowide_standalone -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/zziplib -I/home/build/finjin-3rd-party/cpp/zlib -I/home/build/finjin-3rd-party/cpp/lpng -I/home/build/finjin-3rd-party/cpp/uriparser/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f9ff474f/zutil.o /home/build/finjin-3rd-party/cpp/zlib/zutil.c

${OBJECTDIR}/_ext/b69e269c/dir.o: /home/build/finjin-3rd-party/cpp/zziplib/zzip/dir.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/b69e269c
	${RM} "$@.d"
	$(COMPILE.c) -O2 -D_ZZIP_NO_INLINE -D_zzip_inline -I. -I../../include -I../../src -I/home/build/finjin-3rd-party/cpp/ -I/home/build/finjin-3rd-party/cpp/cpp-netlib -I/home/build/finjin-3rd-party/cpp/curl/include -I/home/build/finjin-3rd-party/cpp/civetweb/include -I/home/build/finjin-3rd-party/cpp/nowide_standalone -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/zziplib -I/home/build/finjin-3rd-party/cpp/zlib -I/home/build/finjin-3rd-party/cpp/lpng -I/home/build/finjin-3rd-party/cpp/uriparser/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/b69e269c/dir.o /home/build/finjin-3rd-party/cpp/zziplib/zzip/dir.c

${OBJECTDIR}/_ext/b69e269c/err.o: /home/build/finjin-3rd-party/cpp/zziplib/zzip/err.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/b69e269c
	${RM} "$@.d"
	$(COMPILE.c) -O2 -D_ZZIP_NO_INLINE -D_zzip_inline -I. -I../../include -I../../src -I/home/build/finjin-3rd-party/cpp/ -I/home/build/finjin-3rd-party/cpp/cpp-netlib -I/home/build/finjin-3rd-party/cpp/curl/include -I/home/build/finjin-3rd-party/cpp/civetweb/include -I/home/build/finjin-3rd-party/cpp/nowide_standalone -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/zziplib -I/home/build/finjin-3rd-party/cpp/zlib -I/home/build/finjin-3rd-party/cpp/lpng -I/home/build/finjin-3rd-party/cpp/uriparser/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/b69e269c/err.o /home/build/finjin-3rd-party/cpp/zziplib/zzip/err.c

${OBJECTDIR}/_ext/b69e269c/fetch.o: /home/build/finjin-3rd-party/cpp/zziplib/zzip/fetch.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/b69e269c
	${RM} "$@.d"
	$(COMPILE.c) -O2 -D_ZZIP_NO_INLINE -D_zzip_inline -I. -I../../include -I../../src -I/home/build/finjin-3rd-party/cpp/ -I/home/build/finjin-3rd-party/cpp/cpp-netlib -I/home/build/finjin-3rd-party/cpp/curl/include -I/home/build/finjin-3rd-party/cpp/civetweb/include -I/home/build/finjin-3rd-party/cpp/nowide_standalone -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/zziplib -I/home/build/finjin-3rd-party/cpp/zlib -I/home/build/finjin-3rd-party/cpp/lpng -I/home/build/finjin-3rd-party/cpp/uriparser/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/b69e269c/fetch.o /home/build/finjin-3rd-party/cpp/zziplib/zzip/fetch.c

${OBJECTDIR}/_ext/b69e269c/file.o: /home/build/finjin-3rd-party/cpp/zziplib/zzip/file.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/b69e269c
	${RM} "$@.d"
	$(COMPILE.c) -O2 -D_ZZIP_NO_INLINE -D_zzip_inline -I. -I../../include -I../../src -I/home/build/finjin-3rd-party/cpp/ -I/home/build/finjin-3rd-party/cpp/cpp-netlib -I/home/build/finjin-3rd-party/cpp/curl/include -I/home/build/finjin-3rd-party/cpp/civetweb/include -I/home/build/finjin-3rd-party/cpp/nowide_standalone -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/zziplib -I/home/build/finjin-3rd-party/cpp/zlib -I/home/build/finjin-3rd-party/cpp/lpng -I/home/build/finjin-3rd-party/cpp/uriparser/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/b69e269c/file.o /home/build/finjin-3rd-party/cpp/zziplib/zzip/file.c

${OBJECTDIR}/_ext/b69e269c/fseeko.o: /home/build/finjin-3rd-party/cpp/zziplib/zzip/fseeko.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/b69e269c
	${RM} "$@.d"
	$(COMPILE.c) -O2 -D_ZZIP_NO_INLINE -D_zzip_inline -I. -I../../include -I../../src -I/home/build/finjin-3rd-party/cpp/ -I/home/build/finjin-3rd-party/cpp/cpp-netlib -I/home/build/finjin-3rd-party/cpp/curl/include -I/home/build/finjin-3rd-party/cpp/civetweb/include -I/home/build/finjin-3rd-party/cpp/nowide_standalone -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/zziplib -I/home/build/finjin-3rd-party/cpp/zlib -I/home/build/finjin-3rd-party/cpp/lpng -I/home/build/finjin-3rd-party/cpp/uriparser/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/b69e269c/fseeko.o /home/build/finjin-3rd-party/cpp/zziplib/zzip/fseeko.c

${OBJECTDIR}/_ext/b69e269c/info.o: /home/build/finjin-3rd-party/cpp/zziplib/zzip/info.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/b69e269c
	${RM} "$@.d"
	$(COMPILE.c) -O2 -D_ZZIP_NO_INLINE -D_zzip_inline -I. -I../../include -I../../src -I/home/build/finjin-3rd-party/cpp/ -I/home/build/finjin-3rd-party/cpp/cpp-netlib -I/home/build/finjin-3rd-party/cpp/curl/include -I/home/build/finjin-3rd-party/cpp/civetweb/include -I/home/build/finjin-3rd-party/cpp/nowide_standalone -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/zziplib -I/home/build/finjin-3rd-party/cpp/zlib -I/home/build/finjin-3rd-party/cpp/lpng -I/home/build/finjin-3rd-party/cpp/uriparser/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/b69e269c/info.o /home/build/finjin-3rd-party/cpp/zziplib/zzip/info.c

${OBJECTDIR}/_ext/b69e269c/memdisk.o: /home/build/finjin-3rd-party/cpp/zziplib/zzip/memdisk.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/b69e269c
	${RM} "$@.d"
	$(COMPILE.c) -O2 -D_ZZIP_NO_INLINE -D_zzip_inline -I. -I../../include -I../../src -I/home/build/finjin-3rd-party/cpp/ -I/home/build/finjin-3rd-party/cpp/cpp-netlib -I/home/build/finjin-3rd-party/cpp/curl/include -I/home/build/finjin-3rd-party/cpp/civetweb/include -I/home/build/finjin-3rd-party/cpp/nowide_standalone -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/zziplib -I/home/build/finjin-3rd-party/cpp/zlib -I/home/build/finjin-3rd-party/cpp/lpng -I/home/build/finjin-3rd-party/cpp/uriparser/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/b69e269c/memdisk.o /home/build/finjin-3rd-party/cpp/zziplib/zzip/memdisk.c

${OBJECTDIR}/_ext/b69e269c/mmapped.o: /home/build/finjin-3rd-party/cpp/zziplib/zzip/mmapped.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/b69e269c
	${RM} "$@.d"
	$(COMPILE.c) -O2 -D_ZZIP_NO_INLINE -D_zzip_inline -I. -I../../include -I../../src -I/home/build/finjin-3rd-party/cpp/ -I/home/build/finjin-3rd-party/cpp/cpp-netlib -I/home/build/finjin-3rd-party/cpp/curl/include -I/home/build/finjin-3rd-party/cpp/civetweb/include -I/home/build/finjin-3rd-party/cpp/nowide_standalone -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/zziplib -I/home/build/finjin-3rd-party/cpp/zlib -I/home/build/finjin-3rd-party/cpp/lpng -I/home/build/finjin-3rd-party/cpp/uriparser/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/b69e269c/mmapped.o /home/build/finjin-3rd-party/cpp/zziplib/zzip/mmapped.c

${OBJECTDIR}/_ext/b69e269c/plugin.o: /home/build/finjin-3rd-party/cpp/zziplib/zzip/plugin.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/b69e269c
	${RM} "$@.d"
	$(COMPILE.c) -O2 -D_ZZIP_NO_INLINE -D_zzip_inline -I. -I../../include -I../../src -I/home/build/finjin-3rd-party/cpp/ -I/home/build/finjin-3rd-party/cpp/cpp-netlib -I/home/build/finjin-3rd-party/cpp/curl/include -I/home/build/finjin-3rd-party/cpp/civetweb/include -I/home/build/finjin-3rd-party/cpp/nowide_standalone -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/zziplib -I/home/build/finjin-3rd-party/cpp/zlib -I/home/build/finjin-3rd-party/cpp/lpng -I/home/build/finjin-3rd-party/cpp/uriparser/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/b69e269c/plugin.o /home/build/finjin-3rd-party/cpp/zziplib/zzip/plugin.c

${OBJECTDIR}/_ext/b69e269c/stat.o: /home/build/finjin-3rd-party/cpp/zziplib/zzip/stat.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/b69e269c
	${RM} "$@.d"
	$(COMPILE.c) -O2 -D_ZZIP_NO_INLINE -D_zzip_inline -I. -I../../include -I../../src -I/home/build/finjin-3rd-party/cpp/ -I/home/build/finjin-3rd-party/cpp/cpp-netlib -I/home/build/finjin-3rd-party/cpp/curl/include -I/home/build/finjin-3rd-party/cpp/civetweb/include -I/home/build/finjin-3rd-party/cpp/nowide_standalone -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/zziplib -I/home/build/finjin-3rd-party/cpp/zlib -I/home/build/finjin-3rd-party/cpp/lpng -I/home/build/finjin-3rd-party/cpp/uriparser/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/b69e269c/stat.o /home/build/finjin-3rd-party/cpp/zziplib/zzip/stat.c

${OBJECTDIR}/_ext/b69e269c/write.o: /home/build/finjin-3rd-party/cpp/zziplib/zzip/write.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/b69e269c
	${RM} "$@.d"
	$(COMPILE.c) -O2 -D_ZZIP_NO_INLINE -D_zzip_inline -I. -I../../include -I../../src -I/home/build/finjin-3rd-party/cpp/ -I/home/build/finjin-3rd-party/cpp/cpp-netlib -I/home/build/finjin-3rd-party/cpp/curl/include -I/home/build/finjin-3rd-party/cpp/civetweb/include -I/home/build/finjin-3rd-party/cpp/nowide_standalone -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/zziplib -I/home/build/finjin-3rd-party/cpp/zlib -I/home/build/finjin-3rd-party/cpp/lpng -I/home/build/finjin-3rd-party/cpp/uriparser/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/b69e269c/write.o /home/build/finjin-3rd-party/cpp/zziplib/zzip/write.c

${OBJECTDIR}/_ext/b69e269c/zip.o: /home/build/finjin-3rd-party/cpp/zziplib/zzip/zip.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/b69e269c
	${RM} "$@.d"
	$(COMPILE.c) -O2 -D_ZZIP_NO_INLINE -D_zzip_inline -I. -I../../include -I../../src -I/home/build/finjin-3rd-party/cpp/ -I/home/build/finjin-3rd-party/cpp/cpp-netlib -I/home/build/finjin-3rd-party/cpp/curl/include -I/home/build/finjin-3rd-party/cpp/civetweb/include -I/home/build/finjin-3rd-party/cpp/nowide_standalone -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/zziplib -I/home/build/finjin-3rd-party/cpp/zlib -I/home/build/finjin-3rd-party/cpp/lpng -I/home/build/finjin-3rd-party/cpp/uriparser/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/b69e269c/zip.o /home/build/finjin-3rd-party/cpp/zziplib/zzip/zip.c

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}
	${RM} ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libfinjin-common.a

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
