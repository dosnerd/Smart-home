.SUFFIXES:
.SUFFIXES:		.c .o

.PHONY: 				all
all:					$(DIR_BIN)$(PROJ_NAME)
							@echo "Done"

dir 	:= 		$(DIR_SRC)
include 		$(DIR_SRC)Rules.mk


#targets
$(DIR_OUT)%.o:			$(DIR_SRC)%.cpp
							mkdir -p  $(dir $@) 
							$(CXX) $(CFLAGS) $(LOCAL_INCLUDES) -c -o $@ $< -std=c++11;
							rm -f $(DIR_DEP)$*.d
							$(CXX) $(CFLAGS) $(LOCAL_INCLUDES) -MM -MT $@ -MD $< -MF $(DIR_DEP)$*.d -std=c++11;
							@echo "--> Compiled "$<"!"
							@echo

$(DIR_OUT)%.o:			$(DIR_SRC)%.c
							mkdir -p  $(dir $@) 	
							$(CC) $(CFLAGS) $(LOCAL_INCLUDES) -c -o $@ $<
							rm -f $(DIR_DEP)$*.d
							$(CC) $(CFLAGS) $(LOCAL_INCLUDES) -MM -MT $@ -MD $< -MF $(DIR_DEP)$*.d;
							@echo "--> Compiled "$<"!"
							@echo

$(DIR_BIN)$(PROJ_NAME):	$(OBJS_BIN)
							$(CXX) $(CFLAGS) $(LDFLAGS) $^ -o $@ -L$(DIR_OUT) -std=c++11
							@echo "--> Linker complete"

.PHONY:		clean
clean:
			rm -f $(CLEAN)
			rm -f $(DIR_BIN)$(PROJ_NAME)
###################################################