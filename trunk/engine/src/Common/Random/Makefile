ROOT_DIR:=../../../..

include $(ROOT_DIR)/Makefile.include

FILE=$(wildcard *.cpp)
MODULES:=$(FILE:%.cpp=%.o)

INCLUDE:=-I.. -I$(ROOT_DIR)/$(ENGINE_DIR)/include -I$(ROOT_DIR)/$(ENGINE_DIR)/sdk/inc -I$(ROOT_DIR)/$(IMPLIB_DIR)/inc

all: $(MODULES)
	@$(ECHO) $(STR_COPY)
	@$(CP) *.o ../

ifneq ($(MAKECMDGOALS),clean)
include $(MODULES:.o=.d)
endif

clean:
	@$(ECHO) $(STR_CLEAN)
	@$(RM) -f *.o *.d
