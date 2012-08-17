ROOT_DIR:=../../..

include $(ROOT_DIR)/Makefile.include

FILE=$(wildcard *.cpp)
OBJECTS:=$(FILE:%.cpp=%.o)
INCLUDE:=-I./ -I../CoreCommon -I$(ROOT_DIR)/$(ENGINE_DIR)/include -I$(ROOT_DIR)/$(ENGINE_DIR)/sdk/inc -I$(ROOT_DIR)/$(IMPLIB_DIR)/inc -I$(ROOT_DIR)/$(IMPLIB_DIR)/inc/tinyxml

TARGET_LIB:=$(ROOT_DIR)/$(BIN_DIR)/$(BUILD_FLAG)/libCoreStress.so

all: $(TARGET_LIB) 

$(TARGET_LIB) : $(OBJECTS)
	@$(ECHO) $(STR_SHARED)
	@$(CC) $(CSHARED) $(TARGET_LIB) *.o -L$(ROOT_DIR)/$(BIN_DIR)/$(BUILD_FLAG) -L$(ROOT_DIR)/$(LIB_DIR)/$(BUILD_FLAG) $(DEPEND_LIB)


ifneq ($(MAKECMDGOALS),clean)
include $(OBJECTS:.o=.d)
endif

clean:
	@$(ECHO) $(STR_CLEAN)
	@$(RM) -f *.o *.a *.d *.so
	@$(RM) -f $(TARGET_LIB)

