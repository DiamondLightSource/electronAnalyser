TOP=../..
include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE

#----------------------------------------------------
#  Optimization of db files using dbst (DEFAULT: NO)
#DB_OPT = YES

#----------------------------------------------------

DB += electronAnalyser.template
DB += electronAnalyserViewer.template
DB += electronAnalyserExample.db

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

# FIXME: This is a custom rule to use msi to expand the substitutions file
#        into a database for this IOC. The reason for the custom rule is that
#        convertDlsRelease.pl does not currently handle expanding the support
#        modules paths properly on Windows W: Once that is fixed we can remove
#        this rule and go back to using the dls rules...
electronAnalyserExample.db: ../electronAnalyserExample.substitutions
	msi -I.. -I$(FFMPEGSERVER)/db -I$(ASYN)/db -I$(AREADETECTOR)/db -I$(EPICS_BASE)/db -S$? <$(DEV_NULL) > $@

