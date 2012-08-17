ROOT_DIR:=../../..

include $(ROOT_DIR)/Makefile.include

FILE=$(wildcard *.cpp)
SERVERFILE=$(wildcard ./Server/*.cpp)
COMMONFILE=$(wildcard ./Common/*.cpp)
STRESSFILE=$(wildcard ./Stress/*.cpp)

SERVERTEMP_OBJ=$(SERVERFILE:%.cpp=%.o)
SERVEROBJECTS=$(subst ./Server/,,$(SERVERTEMP_OBJ))

STRESSTEMP_OBJ=$(STRESSFILE:%.cpp=%.o)
STRESSOBJECTS=$(subst ./Stress/,,$(STRESSTEMP_OBJ))

COMMONTEMP_OBJ=$(COMMONFILE:%.cpp=%.o)
COMMONOBJECTS=$(subst ./Common/,,$(COMMONTEMP_OBJ))
OBJECTS:= $(FILE:%.cpp=%.o) $(SERVEROBJECTS) $(STRESSOBJECTS) $(COMMONOBJECTS)


INCLUDE:=-I./Server -I./Stress -I./Common -I./ -I$(ROOT_DIR)/$(ENGINE_DIR)/include \
	-I$(ROOT_DIR)/$(ENGINE_DIR)/sdk/inc -I$(ROOT_DIR)/$(IMPLIB_DIR)/inc 

TARGET_LIB:=$(ROOT_DIR)/$(BIN_DIR)/$(BUILD_FLAG)/libCoreScript.a

all: $(TARGET_LIB)

$(TARGET_LIB) : $(OBJECTS)
	@$(ECHO) $(STR_SHARED)
	@$(AR) $(TARGET_LIB) *.o


%.o:./Common/%.cpp
	@$(ECHO) $(STR_COMPILE): $<
	@$(CC) $(CMODULE) $< $(INCLUDE)

%.o:./Server/%.cpp
	@$(ECHO) $(STR_COMPILE): $<
	@$(CC) $(CMODULE) $< $(INCLUDE)

%.o:./Stress/%.cpp
	@$(ECHO) $(STR_COMPILE): $<
	@$(CC) $(CMODULE) $< $(INCLUDE)

%.d: ./Stress/%.cpp
	@$(CC) -MM $(INCLUDE) $< > $@.tmp;\
	sed  's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.tmp > $@;\
	rm -f $@.tmp

%.d: ./Server/%.cpp
	@$(CC) -MM $(INCLUDE) $< > $@.tmp;\
	sed  's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.tmp > $@;\
	rm -f $@.tmp

%.d: ./Common/%.cpp
	@$(CC) -MM $(INCLUDE) $< > $@.tmp;\
	sed  's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.tmp > $@;\
	rm -f $@.tmp

ifneq ($(MAKECMDGOALS),clean)
include $(OBJECTS:.o=.d)
endif

clean:
	@$(ECHO) $(STR_CLEAN)
	@$(RM) -f *.o *.a *.d *.so
	@$(RM) -f $(TARGET_LIB)
