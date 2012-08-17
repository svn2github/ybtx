ROOT_DIR:=../../..

include $(ROOT_DIR)/Makefile.include

INCLUDE:=-I./ -I$(ROOT_DIR)/$(ENGINE_DIR)/include -I$(ROOT_DIR)/$(ENGINE_DIR)/sdk/inc -I$(ROOT_DIR)/$(IMPLIB_DIR)/inc

MODULES:=\
	Container \
	Tick \
	Random \
	Package \
	Math \
	File \
	Trace \
	Value \
	Pattern \
	Log \
	String \
	Script \
	Path \
	CodeCvs \
	Misc 

TARGET_LIB:=$(ROOT_DIR)/$(BIN_DIR)/$(BUILD_FLAG)/libCommon.so
DEPEND_LIB:=$(LIB_PTHREAD) -lrt -lcppunit -lbz2 -lz

FILE=$(wildcard *.cpp)
OBJECTS:=$(FILE:%.cpp=%.o) 

all: subsystem $(TARGET_LIB)

subsystem:
	@for i in $(MODULES); do \
		set -e ; cd $$i ; $(MAKE) ; cd ..; \
	done;

$(TARGET_LIB) : $(OBJECTS) subsystem
	@$(ECHO) $(STR_SHARED)
	@$(CC) $(CSHARED) $(TARGET_LIB) *.o -L$(ROOT_DIR)/$(BIN_DIR)/$(BUILD_FLAG) -L$(ROOT_DIR)/$(LIB_DIR)/$(BUILD_FLAG) $(DEPEND_LIB)


ifneq ($(MAKECMDGOALS),clean)
include $(OBJECTS:.o=.d)
endif

clean:
	@for i in $(MODULES); do \
		set -e ; cd $$i ; $(MAKE) clean; cd ..; \
	done;
	@$(ECHO) $(STR_CLEAN)
	@$(RM) -f *.o *.a *.so
	@$(RM) -f $(TARGET_LIB)

