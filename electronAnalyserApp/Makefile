TOP = ..
include $(TOP)/configure/CONFIG

DIRS += wsesSrc
DIRS += src

src_DEPEND_DIRS = wsesSrc

#DIRS := $(DIRS) $(filter-out $(DIRS), $(wildcard *src*))
#DIRS := $(DIRS) $(filter-out $(DIRS), $(wildcard *Src*))
DIRS := $(DIRS) $(filter-out $(DIRS), $(wildcard *db*))
DIRS := $(DIRS) $(filter-out $(DIRS), $(wildcard *Db*))
DIRS := $(DIRS) $(filter-out $(DIRS), $(wildcard *opi*))

include $(TOP)/configure/RULES_DIRS

