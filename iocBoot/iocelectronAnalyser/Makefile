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
EXPAND_VARS += ELECTRON_ANALYSER_INSTALL=$(INSTALL_LOCATION_APP)
# The EPICS Installation on deployment PC.
EXPAND_VARS += EPICS_INSTALL=W:

EXPAND_VARS += ELECTRON_ANALYSER=$(shell perl $(TOOLS)/fullPathName.pl $(TOP))
EXPAND_VARS += AREA_DETECTOR=$(AREA_DETECTOR)
EXPAND_VARS += ASYN=$(ASYN)

# build parameters for Pixium MEDM or EDM GUI - startgui.sh - for Linux.  
EXPAND_VARS += ELECTRON_ANALYSER_LINUX=/dls_sw/work/R3.14.10/support/electronAnalyser
EXPAND_VARS += AREA_DETECTOR_LINUX=/dls_sw/prod/R3.14.10/support/areaDetector/1-5
EXPAND_VARS += ASYN_LINUX=/dls_sw/prod/R3.14.10/support/asyn/4-12

######################################################
# File to be expanded.                               #
# Filename must not contain the ending character @   #
######################################################  
EXPAND+=copyandrun.bat
EXPAND+=profile.bat
EXPAND+=startgui.sh
EXPAND+=startElectronAnalyserIOC.bat

include $(TOP)/configure/RULES
