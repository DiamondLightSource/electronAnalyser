TOP=../..

include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE
#========================================

# The following is needed on win32-x86-debug because the Prosilica library 
# wants to use LIBCMT, and that conflicts with LIBCMTD
#ifeq ($(T_A), win32-x86)
#USR_LDFLAGS += /NODEFAULTLIB:LIBCMT
#endif

# -------------------------------
# Build an Diamond Support Module
# -------------------------------

# This line is to pick up .hpp files from wsesSrc (because they can't be installed into the includes directory
USR_INCLUDES += -I../../wsesSrc/

#define the library name
LIBRARY_IOC_WIN32 += electronAnalyserSupport

# Set the compiler and linker flags required to give a debug build 
# Note that the /Od flag (disable compiler optimisations) gets overridden by
# /Ox coming from the global win32_x86 compiler settings. I haven't found a 
# way to override this global flag for just the win32_x86 build - the only
# thing that seems to work is to override OPT_CFLAGS_YES globally, which 
# breaks builds for other targets (notably the Linux build needed for the
# viewer).
# It still seems to be possible to follow the code in the debugger with Ox on though.
USR_CFLAGS_WIN32 += -DWIN32 -DDEBUG /Zi /Od
USR_CPPFLAGS_WIN32 += -DWIN32 -DDEBUG /Zi /Od
USR_LDFLAGS_WIN32 += /INCREMENTAL:NO /DEBUG /OPT:REF /OPT:ICF
USR_INCLUDES_WIN32 += 
USR_LIBS_WIN32 +=


# The following are compiled and added to the support library
electronAnalyserSupport_SRCS += drvElectronAnalyserRegistrar.c
electronAnalyserSupport_SRCS += electronAnalyser.cpp

# -------------------------------
# Build an Diamond Support Module
# -------------------------------

#define the library name
LIBRARY_IOC_Linux += electronAnalyserViewerSupport

# The following are compiled and added to the support library
electronAnalyserViewerSupport_SRCS += drvElectronAnalyserViewerRegistrar.c
electronAnalyserViewerSupport_SRCS += electronAnalyserViewer.cpp
electronAnalyserViewerSupport_SRCS += dgframe.pb.cc
electronAnalyserViewerSupport_LIBS += zmq
electronAnalyserViewerSupport_LIBS += protobuf

zmq_DIR = $(ZMQ)/lib
protobuf_DIR = $(PROTOBUF)/lib

DBD += electronAnalyserViewerSupport.dbd

USR_INCLUDES += -I$(ZMQ)/include
USR_INCLUDES += -I$(PROTOBUF)/include
# ------------------------
# Build an IOC Application
# ------------------------

#PROD_IOC_WIN32 += electronAnalyser

# electronAnalyser.dbd will be installed into <top>/dbd
# <name>.dbd will be created from <name>Include.dbd
#DBD += electronAnalyser.dbd
DBD += electronAnalyserSupport.dbd

# electronAnalyser.dbd will be created from these files
#electronAnalyser_DBD += commonDriverSupport.dbd
#electronAnalyser_DBD += electronAnalyserSupport.dbd
#electronAnalyser_DBD += NDPluginSupport.dbd
#electronAnalyser_DBD += ffmpegServer.dbd

# <name>_registerRecordDeviceDriver.cpp will be created from <name>.dbd
#electronAnalyser_SRCS += electronAnalyser_registerRecordDeviceDriver.cpp
# These two lines are needed for non-vxWorks builds, such as Linux
#electronAnalyser_SRCS_DEFAULT += electronAnalyserMain.cpp
#electronAnalyser_SRCS_vxWorks += -nil-

# Add locally compiled object code
#electronAnalyser_LIBS += electronAnalyserSupport
#electronAnalyser_LIBS += NDPlugin 
#electronAnalyser_LIBS_WIN32 += wses
#electronAnalyser_LIBS += avdevice avformat avcodec avutil swscale ffmpegServer

# A little linking hack to ensure that nafxcw is linked first (preventing a LNK2005 error)
#electronAnalyser_LDFLAGS_WIN32 +=/NOD:nafxcw.lib
#electronAnalyser_SYS_LIBS_WIN32 +=nafxcw

# Windows libraries for Electron Analyser - non available if needed need to generate from .dll
#electronAnalyser_LIBS_WIN32 += SESInstrument
#SESInstrument_DIR = ../lib

# This is probably not necessary as we build statically against the .lib files
#BIN_INSTALLS_WIN32 += ../dll/SESWrapper.dll ../dll/SESInstrument.dll $(AREADETECTOR)/ADApp/tiffSupport/libtiff3.dll

# We need to link this IOC Application against the EPICS Base libraries

# install the areaDetector and dxp DLL libraries locally in our bin dir

#BIN_INSTALLS_WIN32 += $(wildcard $(AREADETECTOR)/bin/$(EPICS_HOST_ARCH)/*.dll)
#BIN_INSTALLS_WIN32 += $(wildcard $(ADBINARIES)/bin/$(EPICS_HOST_ARCH)/*.dll)
#BIN_INSTALLS_WIN32 += $(wildcard $(FFMPEGSERVER)/bin/$(EPICS_HOST_ARCH)/*.dll)

# install caRepeater.exe in the bin dir so we dont depend on EPICS being preinstalled
# on our Windows target machine
#BIN_INSTALLS_WIN32 += $(EPICS_BASE)/bin/$(EPICS_HOST_ARCH)/caRepeater.exe

# Ensure all the common libraries used by areaDetector are included in the build
# of both library and IOC executable.
include $(AREADETECTOR)/ADApp/commonDriverMakefile

include $(TOP)/configure/RULES
