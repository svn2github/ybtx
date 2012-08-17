ROOT_DIR:=../../../..

include $(ROOT_DIR)/Makefile.include

FILE=$(wildcard *.cpp)
MODULES:=$(FILE:%.cpp=%.o)

INCLUDE:=-I.. -I$(ROOT_DIR)/$(ENGINE_DIR)/include -I$(ROOT_DIR)/$(ENGINE_DIR)/sdk/inc -I$(ROOT_DIR)/$(IMPLIB_DIR)/inc -I$(ROOT_DIR)/$(IMPLIB_DIR)/inc/tinyxml

all: $(MODULES)
	@$(ECHO) $(STR_COPY)
	@$(CP) *.o ../

%.d: %.cpp
	@$(CC) -MM $(INCLUDE) $< > $@.tmp;\
	sed  's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.tmp > $@;\
	rm -f $@.tmp


%.o: %.cpp
	@$(ECHO) $(STR_COMPILE): $@
	@$(CC) $(CMODULE) $< $(INCLUDE)

ifneq ($(MAKECMDGOALS),clean)
include $(MODULES:.o=.d)
endif
	
clean:
	@$(ECHO) $(STR_CLEAN)
	@$(RM) -f *.o *.d
