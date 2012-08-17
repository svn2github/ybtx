ROOT_DIR:=../../..

include $(ROOT_DIR)/Makefile.include

FILE=$(wildcard *.cpp)
OBJECTS:=$(FILE:%.cpp=%.o)
INCLUDE:=-I./ -I$(ROOT_DIR)/$(ENGINE_DIR)/include \
	-I$(ROOT_DIR)/$(ENGINE_DIR)/sdk/inc -I$(ROOT_DIR)/$(IMPLIB_DIR)/inc 

TARGET_LIB:=$(ROOT_DIR)/$(BIN_DIR)/$(BUILD_FLAG)/libShellCommon.a

all: $(TARGET_LIB)

$(TARGET_LIB) : $(OBJECTS)
	@$(ECHO) $(STR_ARCH)
	@$(AR) $(TARGET_LIB) *.o

ifneq ($(MAKECMDGOALS),clean)
include $(OBJECTS:.o=.d)
endif

clean:
	@$(ECHO) $(STR_CLEAN)
	@$(RM) -f *.o *.a *.d *.so
	@$(RM) -f $(TARGET_LIB)

