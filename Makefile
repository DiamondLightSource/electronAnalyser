#Makefile at top of application tree
TOP = .
include $(TOP)/configure/CONFIG
DIRS := $(DIRS) $(filter-out $(DIRS), configure)
DIRS := $(DIRS) $(filter-out $(DIRS), $(wildcard *App))
DIRS := $(DIRS) $(filter-out $(DIRS), $(wildcard iocBoot))

ifeq ($(EPICS_HOST_ARCH),linux-x86)
        # Comment out the following line to creation of example iocs and documentation
	DIRS := $(DIRS) $(filter-out $(DIRS), $(wildcard etc))
        # Comment out the following line to disable building of example iocs
	DIRS := $(DIRS) $(filter-out $(DIRS), $(wildcard iocs))
endif

include $(TOP)/configure/RULES_TOP

