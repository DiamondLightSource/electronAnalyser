TOP=../..

include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE
#========================================

# -------------------------------
# Build an Diamond Support Module
# -------------------------------

# Install header files in include directory
INC += common.h
INC += constants.h
INC += sestypes.h
INC += types.h
INC += werror.h
INC += wevent.h 
INC += wlibrary.h
INC += wsesinstrument.h
INC += wseswrapperbase.h
INC += wseswrappermain.h

# EPICS Base rules currently don't support including .hpp files
#INC += common.hpp
#INC += wcommand.hpp
#INC += wvariable.hpp

#define the library name
LIBRARY_IOC_WIN32 += wses

# Any specific external resources (not epics) that we need to link to
USR_CFLAGS +=   -DWIN32
USR_CXXFLAGS += -DWIN32
USR_CFLAGS_WIN32 += 
USR_INCLUDES_WIN32 += 
USR_LIBS_WIN32 +=

# The following are compiled and added to the support library
wses_SRCS_WIN32 += cexports.cpp werror.cpp wevent.cpp wlibrary.cpp wsesinstrument.cpp wvariable.cpp wseswrapperbase.cpp wseswrappermain.cpp


include $(TOP)/configure/RULES
