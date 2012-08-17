ROOT_DIR:=../../..

include $(ROOT_DIR)/Makefile.include

FILE=$(wildcard *.cpp)
OBJECTS:=$(FILE:%.cpp=%.o)
INCLUDE:=-I$(ROOT_DIR)/$(ENGINE_DIR)/include -I$(ROOT_DIR)/$(ENGINE_DIR)/sdk/inc -I$(ROOT_DIR)/$(IMPLIB_DIR)/inc -I$(ROOT_DIR)/$(ENGINE_DIR)/src/CoreServer -I$(ROOT_DIR)/$(GAME_DIR)/gac_gas/ComGacGas -I$(ROOT_DIR)/$(ENGINE_DIR)/src/CoreCommon

DEPEND_LIB:=-lComGacGas -lShellServer -lShellCommon -lCoreServer -lCoreCommon -lScript -lDatabase -lNetwork -lModule -lCommon -lyauth64 -lssl
SCRIPT_DEP:=$(ROOT_DIR)/$(BIN_DIR)/$(BUILD_FLAG)/libShellCommon.a $(ROOT_DIR)/$(BIN_DIR)/$(BUILD_FLAG)/libShellServer.a
TARGET:=$(ROOT_DIR)/$(BIN_DIR)/$(BUILD_FLAG)/libGameGas.so

all: $(TARGET)

$(TARGET) : $(OBJECTS) $(SCRIPT_DEP) Makefile
	@$(ECHO) $(STR_SHARED)
	@$(CC) $(CSHARED) $(TARGET) *.o -L$(ROOT_DIR)/$(IMPLIB_DIR)/lib/$(BUILD_FLAG) -L$(ROOT_DIR)/$(BIN_DIR)/$(BUILD_FLAG) -L$(ROOT_DIR)/$(LIB_DIR)/$(BUILD_FLAG) $(DEPEND_LIB)

ifneq ($(MAKECMDGOALS),clean)
include $(OBJECTS:.o=.d)
endif

clean:
	@$(ECHO) $(STR_CLEAN)
	@$(RM) -f *.o *.a *.d *.so
	@$(RM) -f $(TARGET)

