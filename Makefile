###################################################
# OVERAL SETTINGS
###################################################
#naming
PROJ_NAME := networking

DIR_SRC:=src/
DIR_INC:=inc/
DIR_OUT:=out/$(TYPE)_$(PLATFORM)/
DIR_DEP:=out/$(TYPE)_$(PLATFORM)/
DIR_BIN:=bin/$(PLATFORM)/

$(shell mkdir -p $(DIR_OUT))
$(shell mkdir -p $(DIR_DEP))
$(shell mkdir -p $(DIR_BIN))

###################################################
#Compile settings
OPT=0

include Platform-$(PLATFORM).mk

CFLAGS  = -O$(OPT)
CFLAGS  = -Iinc/
CFLAGS += -Wall -Werror -Wtype-limits -Wuninitialized -Wunused-parameter -Wunused-but-set-parameter -Wempty-body   
LDFLAGS = -Os -Wl,-gc-sections -pthread 

CFLAGS += -D $(shell echo $(PLATFORM) | tr a-z A-Z)=1


ifdef TYPE
	CFLAGS += -D $(shell echo $(TYPE) | tr a-z A-Z)=1
	PROJ_NAME := $(PROJ_NAME)_$(TYPE)
endif

LOCAL_INCLUDES =
###################################################
include Rules.mk
