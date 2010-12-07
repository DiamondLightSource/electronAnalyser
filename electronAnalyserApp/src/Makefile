TOP=../..

include $(TOP)/configure/CONFIG

# -------------------------------
# Build an Diamond Support Module
# -------------------------------

#define the library name
LIBRARY_IOC += electronAnalyserSupport

# xxxRecord.h will be created from xxxRecord.dbd
#DBDINC += xxx.h

# xxxRecord.dbd will be installed into <top>/dbd
DBD += electronAnalyserSupport.dbd

# Any specific external resources (not epics) that we need to link to
# need to be defined as follows.
# Keep in mind that we dont specifically need to link to libraries at this
# point (in a support module) as linking will happen when we build the IOC binary.
USR_CFLAGS_WIN32 += 
USR_INCLUDES_WIN32 += -I../include/
#USR_LIBS_WIN32 +=

USR_CFLAGS_Linux +=
USR_INCLUDES_Linux +=
#USR_LIBS_Linux +=
# In the case of usr lib nnn, point to its dir like this:
#nnn_DIR =

# The following are compiled and added to the support library
electronAnalyserSupport_SRCS_WIN32 += cexports.cpp werror.cpp wevent.cpp wlibrary.cpp wmutex.cpp wsesinstrument.cpp wvariable.cpp wseswrapperbase.cpp wseswrappermain.cpp electronAnalyser.cpp
#electronAnalyserSupport_SRCS_Linux += electronAnalyser_sim.cpp
electronAnalyserSupport_SRCS += drvElectronAnalyserRegistrar.c

# ------------------------
# Build an IOC Application
# ------------------------

PROD_IOC = electronAnalyser

# electronAnalyser.dbd will be installed into <top>/dbd
DBD += electronAnalyser.dbd

# electronAnalyser.dbd will be created from these files
electronAnalyser_DBD += base.dbd
electronAnalyser_DBD += asyn.dbd
electronAnalyser_DBD += ADSupport.dbd
electronAnalyser_DBD += pluginSupport.dbd
electronAnalyser_DBD += electronAnalyserSupport.dbd
electronAnalyser_DBD += busySupport.dbd
#electronAnalyser_DBD += xxx.dbd

# electronAnalyser_registerRecordDeviceDriver.cpp will be created
# electronAnalyser.dbd
electronAnalyser_SRCS += electronAnalyser_registerRecordDeviceDriver.cpp

# These two lines are needed for non-vxWorks builds, such as Linux
electronAnalyser_SRCS_DEFAULT += electronAnalyserMain.cpp
electronAnalyser_SRCS_vxWorks += -nil-

# Add locally compiled object code
#electronAnalyser_SRCS +=

# The following adds object code from base/src/vxWorks
electronAnalyser_OBJS_vxWorks += $(EPICS_BASE_BIN)/vxComLibrary

# This line says that this IOC Application depends on the
# electronAnalyser Support Module

# A little linking hack to ensure that nafxcwd is linked first (preventing a LNK2005 error)
electronAnalyser_LDFLAGS_WIN32 += /NOD:nafxcwd.lib
electronAnalyser_SYS_LIBS_WIN32 += nafxcwd

electronAnalyser_LIBS += asyn
electronAnalyser_LIBS += ADBase NDPlugin netCDF
electronAnalyser_LIBS += electronAnalyserSupport
electronAnalyser_LIBS += busy

# Linux system libraries:
electronAnalyser_SYS_LIBS_Linux += tiff jpeg z
# Linux libraries supplied by areaDetector
electronAnalyser_LIBS_Linux += NeXus mfhdf df hdf5 mxml

# Windows libraries supplied by areaDetector
electronAnalyser_LIBS_WIN32 += libtiff jpeg libNeXus
# Windows libraries for Electron Analyser
#electronAnalyser_LIBS_WIN32 += SESWrapper SESInstrument
#SESWrapper_DIR = ../lib
#SESInstrument_DIR = ../lib

# This is probably not necessary as we build statically against the .lib files
#BIN_INSTALLS_WIN32 += ../dll/SESWrapper.dll ../dll/SESInstrument.dll $(AREA_DETECTOR)/ADApp/tiffSupport/libtiff3.dll

# We need to link this IOC Application against the EPICS Base libraries
electronAnalyser_LIBS += $(EPICS_BASE_IOC_LIBS)

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
