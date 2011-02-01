TOP = ../..
include $(TOP)/configure/CONFIG
#########################################
# scripts to install in ${TOP}/bin/ dir #
#########################################
SCRIPTS+=stelectronAnalyser.boot
SCRIPTS+=copyandrun.bat
SCRIPTS+=profile.bat
SCRIPTS+=startgui.sh
SCRIPTS+=startElectronAnalyserIOC.bat
DATA+=electronAnalyserclient.py
##########################################################################
# Define variables to expand in files that end with @ in this directory  #
# These variables will override those defined in RELEASE and RELEASE.xxx #
# To use these varibles surround them with a starting @ and an ending @  #
# in the files t be expanded.                                            #
##########################################################################
# The electronAnalyser installation directory on deployment PC.
#EXPAND_VARS += ELECTRON_ANALYSER_INSTALL=$(ELECTRON_ANALYSER_INSTALL)
# The EPICS Installation on deployment PC.
EXPAND_VARS += EPICS_INSTALL=W:

EXPAND_VARS += ELECTRON_ANALYSER=$(shell perl $(TOOLS)/fullPathName.pl $(TOP))
EXPAND_VARS += AREA_DETECTOR=w:/prod/$(EPICS_RELEASE)/support/areaDetector/$(AREA_DETECTOR_RELEASE)
EXPAND_VARS += ASYN=w:/prod/$(EPICS_RELEASE)/support/asyn/$(ASYN_RELEASE)

# build parameters for Pixium MEDM or EDM GUI - startgui.sh - for Linux.  
EXPAND_VARS += ELECTRON_ANALYSER_LINUX=/dls_sw/work/$(EPICS_RELEASE)/support/electronAnalyser
EXPAND_VARS += AREA_DETECTOR_LINUX=/dls_sw/prod/$(EPICS_RELEASE)/support/areaDetector/$(AREA_DETECTOR_RELEASE)
EXPAND_VARS += ASYN_LINUX=/dls_sw/prod/$(EPICS_RELEASE)/support/asyn/$(ASYN_RELEASE)

######################################################
# File to be expanded.                               #
# Filename must not contain the ending character @   #
######################################################  
EXPAND+=copyandrun.bat
EXPAND+=profile.bat
EXPAND+=startgui.sh
EXPAND+=startElectronAnalyserIOC.bat

include $(TOP)/configure/RULES
