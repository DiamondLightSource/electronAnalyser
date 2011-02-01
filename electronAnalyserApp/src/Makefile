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

#define the library name
LIBRARY_IOC += electronAnalyserSupport

# Any specific external resources (not epics) that we need to link to
USR_CFLAGS +=   -DWIN32
USR_CXXFLAGS += -DWIN32
USR_CFLAGS_WIN32 += 
USR_INCLUDES_WIN32 += -I../include
USR_LIBS_WIN32 +=


# The following are compiled and added to the support library
electronAnalyserSupport_SRCS_WIN32 += cexports.cpp werror.cpp wevent.cpp wlibrary.cpp wmutex.cpp wsesinstrument.cpp wvariable.cpp wseswrapperbase.cpp wseswrappermain.cpp electronAnalyser.cpp
electronAnalyserSupport_SRCS += drvElectronAnalyserRegistrar.c

# ------------------------
# Build an IOC Application
# ------------------------

PROD_IOC += electronAnalyser

# electronAnalyser.dbd will be installed into <top>/dbd
# <name>.dbd will be created from <name>Include.dbd
DBD += electronAnalyser.dbd
DBD += electronAnalyserSupport.dbd

# electronAnalyser.dbd will be created from these files
electronAnalyser_DBD += base.dbd
electronAnalyser_DBD += asyn.dbd
electronAnalyser_DBD += ADSupport.dbd
electronAnalyser_DBD += pluginSupport.dbd
electronAnalyser_DBD += electronAnalyserSupport.dbd
electronAnalyser_DBD += busySupport.dbd

# <name>_registerRecordDeviceDriver.cpp will be created from <name>.dbd
electronAnalyser_SRCS += electronAnalyser_registerRecordDeviceDriver.cpp
# These two lines are needed for non-vxWorks builds, such as Linux
electronAnalyser_SRCS_DEFAULT += electronAnalyserMain.cpp
electronAnalyser_SRCS_vxWorks += -nil-

# Add locally compiled object code
electronAnalyser_LIBS += electronAnalyserSupport
electronAnalyser_LIBS += NDPlugin ADBase netCDF
electronAnalyser_LIBS_Linux += PvAPI ImageLib
electronAnalyser_LIBS_WIN32 += PvAPI ImageLib
electronAnalyser_LIBS += calc busy sscan autosave mca
electronAnalyser_LIBS += asyn
electronAnalyser_LIBS += $(EPICS_BASE_IOC_LIBS)

# A little linking hack to ensure that nafxcw is linked first (preventing a LNK2005 error)
electronAnalyser_LDFLAGS_WIN32 +=/NOD:nafxcw.lib
electronAnalyser_SYS_LIBS_WIN32 +=nafxcw

# To use versions of these libraries supplied with areaDetector use this line
electronAnalyser_SYS_LIBS_cygwin32 += rpc
electronAnalyser_LIBS_WIN32        += libtiff jpeg libNeXus.dll
electronAnalyser_LIBS_cygwin32     += libtiff jpeg libNeXus.dll

# To use versions of these libraries supplied with areaDetector use this line
electronAnalyser_LIBS_Linux        += NeXus mfhdf df hdf5 mxml
# To use system versions of these libraries use this line
#electronAnalyser_SYS_LIBS_Linux    += NeXus mfhdf df hdf5 mxml

# To use versions of these libraries supplied with areaDetector use this line
#electronAnalyser_LIBS_Linux        += tiff jpeg z
# To use system versions of these libraries use this line
electronAnalyser_SYS_LIBS_Linux    += tiff jpeg z

electronAnalyser_LIBS_solaris        += NeXus hdf5 jpeg z
electronAnalyser_SYS_LIBS_solaris    += tiff

# Windows libraries for Electron Analyser - non available if needed need to generate from .dll
#electronAnalyser_LIBS_WIN32 += SESInstrument
#SESInstrument_DIR = ../lib

# This is probably not necessary as we build statically against the .lib files
#BIN_INSTALLS_WIN32 += ../dll/SESWrapper.dll ../dll/SESInstrument.dll $(AREA_DETECTOR)/ADApp/tiffSupport/libtiff3.dll

# We need to link this IOC Application against the EPICS Base libraries


# ---------------------------------------------------

# NOTE: To build SNL programs, SNCSEQ must be defined
# in the <top>/configure/RELEASE file
#
# The difference between a file with an "st" extension
# and one with an "stt" extension is:
# The "st" extension files get passed through the C preprocessor
# whereas the "stt" files do not.

#ifneq ($(SNCSEQ),)
    # This builds sncExample as a component of electronAnalyser
#    electronAnalyser_SNCFLAGS += +r
#    electronAnalyser_DBD      += sncExample.dbd
#    electronAnalyser_SRCS     += sncExample.stt
#    electronAnalyser_LIBS     += seq pv

    # The following builds sncExample as a standalone application
#    PROD_HOST           += sncExample
#    sncExample_SNCFLAGS += +m
#    sncExample_SRCS     += sncExample.stt
#    sncExample_LIBS     += seq pv
    # Here, we need to specify the dependency on EPICS Base libraries
    # again because this is a standalone application and
    # not part of electronAnalyser
#    sncExample_LIBS     += $(EPICS_BASE_HOST_LIBS)
#endif

include $(TOP)/configure/RULES
