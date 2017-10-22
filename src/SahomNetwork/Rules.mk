#save local variables (directory path etc)
sp				:= $(sp).x
dirstack_$(sp)	:= $(d)
d				:= $(dir)

#include subdirectories
dirs_$(d)		:= $(dir $(wildcard $(d)*/Rules.mk))
$(foreach dir,$(dirs_$(d)), $(eval include $(dir)Rules.mk))

#find all source files
OBJS_$(d)		:= $(wildcard $(d)*.c)
OBJS_$(d)		+= $(wildcard $(d)*.cpp)
OBJS_$(d)		:= $(patsubst %.c, %.o, $(patsubst %.cpp, %.o, $(OBJS_$(d))))
DEPS_$(d)		:= $(OBJS_$(d):.o=.d)

#change paths
OBJS_$(d)		:= $(OBJS_$(d):$(DIR_SRC)%=$(DIR_OUT)%)
DEPS_$(d)		:= $(DEPS_$(d):$(DIR_SRC)%=$(DIR_DEP)%)
OBJS_BIN		:= $(OBJS_BIN) $(OBJS_$(d))

#add files in cleanup
CLEAN			:= $(CLEAN) $(OBJS_$(d)) $(DEPS_$(d))

#local includes
$(OBJS_$(d)):	LOCAL_INCLUDES := -I$(d:$(DIR_SRC)%=$(DIR_INC)%)

#include DEPS if exists
-include $(DEPS_$(d))

#strip down vars
d		:= $(dirstack_$(sp))
sp		:= $(basename $(sp))