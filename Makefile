###################################################
# OVERAL SETTINGS
###################################################
#naming
PROJ_NAME := networking

DIR_SRC:=src/
DIR_INC:=inc/
DIR_OUT:=out/
DIR_DEP:=out/
DIR_BIN:=bin/

$(shell mkdir -p $(DIR_OUT))
$(shell mkdir -p $(DIR_DEP))
$(shell mkdir -p $(DIR_BIN))

###################################################
#Compile settings
OPT=0

CC=arm-linux-gnueabihf-gcc
CXX=arm-linux-gnueabihf-g++
OBJCOPY=arm-linux-gnueabihf-objcopy

CFLAGS  = -O$(OPT)
CFLAGS  = -Iinc/
CFLAGS += -Wall -Werror -Wtype-limits -Wuninitialized -Wunused-parameter -Wunused-but-set-parameter -Wempty-body   
LDFLAGS = -Os -Wl,-gc-sections 


ifeq ($(TYPE),server)
	CFLAGS += -D SERVER=1
	PROJ_NAME := $(PROJ_NAME)_server
endif

LOCAL_INCLUDES =
###################################################
include Rules.mk
