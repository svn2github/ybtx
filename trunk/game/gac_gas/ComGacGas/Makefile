ROOT_DIR:=../../..

include $(ROOT_DIR)/Makefile.include

FILE=$(wildcard *.cpp)
OBJECTS:=$(FILE:%.cpp=%.o)
INCLUDE:=-I$(ROOT_DIR)/$(ENGINE_DIR)/include -I$(ROOT_DIR)/$(ENGINE_DIR)/sdk/inc -I$(ROOT_DIR)/$(IMPLIB_DIR)/inc

TARGET:=$(ROOT_DIR)/$(BIN_DIR)/$(BUILD_FLAG)/libComGacGas.so

all: $(TARGET)


$(TARGET) : $(OBJECTS)
	@$(ECHO) $(STR_SHARED)
	@$(CC) $(CSHARED) $(TARGET) *.o -L$(ROOT_DIR)/$(BIN_DIR)/$(BUILD_FLAG) -L$(ROOT_DIR)/$(LIB_DIR)/$(BUILD_FLAG) $(DEPEND_LIB)


ifneq ($(MAKECMDGOALS),clean)
include $(OBJECTS:.o=.d)
endif

clean:
	@$(ECHO) $(STR_CLEAN)
	@$(RM) -f *.o *.a *.d *.so
	@$(RM) -f $(TARGET)
