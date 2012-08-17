ROOT_DIR:=../../..

include $(ROOT_DIR)/Makefile.include

FILE=$(wildcard *.cpp)
MODULES:=$(FILE:%.cpp=%.o)

INCLUDE:=-I. -I$(ROOT_DIR)/$(ENGINE_DIR)/include -I$(ROOT_DIR)/$(ENGINE_DIR)/sdk/inc -I$(ROOT_DIR)/$(IMPLIB_DIR)/inc
TARGET_BIN:=$(ROOT_DIR)/$(BIN_DIR)/$(BUILD_FLAG)/RunScript
DEPEND_LIB:=-lScript -lCommon -lModule

all: $(TARGET_BIN)
	@$(ECHO) $(STR_BIN)
	@$(ECHO) $(STR_COPY)



$(TARGET_BIN) : $(DEPENDENCIES)
	@$(CC) $(CNORMAL) $(TARGET_BIN) *.o -Wl,-rpath-link,$(ROOT_DIR)/$(BIN_DIR)/$(BUILD_FLAG) -L$(ROOT_DIR)/$(BIN_DIR)/$(BUILD_FLAG) -L$(ROOT_DIR)/$(LIB_DIR)/$(BUILD_FLAG) $(DEPEND_LIB)



ifneq ($(MAKECMDGOALS),clean)
include $(DEPENDENCIES:.o=.d)
endif

clean:
	@$(ECHO) $(STR_CLEAN)
	@$(RM) -f *.o *.d $(TARGET_BIN)
	@$(RM) -f $(ROOT_DIR)/$(BIN_DIR)/$(BUILD_FLAG)/$(TARGET_BIN)

