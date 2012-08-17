ROOT_DIR:=../../..

include $(ROOT_DIR)/Makefile.include

FILE=$(wildcard *.cpp)
MODULES:=$(FILE:%.cpp=%.o) $(MODULES_B)

INCLUDE:=-I$(ROOT_DIR)/$(ENGINE_DIR)/include -I$(ROOT_DIR)/$(ENGINE_DIR)/sdk/inc -I$(ROOT_DIR)/$(IMPLIB_DIR)/inc
TARGET_LIB:=$(ROOT_DIR)/$(BIN_DIR)/$(BUILD_FLAG)/libDatabase.so

DEPEND_LIB:=-lmysql

all: $(TARGET_LIB)

$(TARGET_LIB) : $(MODULES)
	@$(ECHO) $(STR_SHARED)
	@$(CC) $(CSHARED) $(TARGET_LIB) *.o -L$(ROOT_DIR)/$(BIN_DIR)/$(BUILD_FLAG) -L$(ROOT_DIR)/$(LIB_DIR)/$(BUILD_FLAG) $(DEPEND_LIB)

ifneq ($(MAKECMDGOALS),clean)
include $(DEPENDENCIES:.o=.d)
endif

clean:
	@$(ECHO) $(STR_CLEAN)
	@$(RM) -f *.o *.d $(TARGET_LIB)
	@$(RM) -f $(TARGET_LIB)

