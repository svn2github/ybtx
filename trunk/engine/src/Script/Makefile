ROOT_DIR:=../../..

include $(ROOT_DIR)/Makefile.include

FILE=$(wildcard *.cpp)

#LUAFILE=$(wildcard lua/*.c)
#LUATEMPOBJ=$(LUAFILE:%.c=%.o)
#LUAOBJ=$(subst lua/,,$(LUATEMPOBJ))	

LUAPROFILERFILE=$(wildcard luaprofiler/*.cpp)
LUAPROFILERTEMPOBJ=$(LUAPROFILERFILE:%.cpp=%.o)
LUAPROFILEROBJ=$(subst luaprofiler/,,$(LUAPROFILERTEMPOBJ))

MODULES:=$(FILE:%.cpp=%.o) $(LUAPROFILEROBJ) #$(LUAOBJ)
DEPEND_LIB=-Wl,-E -ldl -lreadline -lhistory -lncurses $(LIB_LUA)

INCLUDE:=-I$(ROOT_DIR)/$(ENGINE_DIR)/include -I$(ROOT_DIR)/$(ENGINE_DIR)/sdk/inc -I$(ROOT_DIR)/$(IMPLIB_DIR)/inc

TARGET_LIB:=$(ROOT_DIR)/$(BIN_DIR)/$(BUILD_FLAG)/libScript.so

#由于使用的是lua5.14 + coco 1.16，根据coco的makefile设置，发现lua必须用O2选项进行编译，因为coco的64位汇编是根据O2选项写的
#参考coco自己的编译选项，我们设置lua的编译选项如下
LUACC:=gcc -O2 -Wall -fpic -c 

all: $(TARGET_LIB)

$(TARGET_LIB) : $(DEPENDENCIES)
	@$(ECHO) $(STR_SHARED)
	@$(CC) $(CSHARED) $(TARGET_LIB) *.o -L$(ROOT_DIR)/$(BIN_DIR)/$(BUILD_FLAG) -L$(ROOT_DIR)/$(LIB_DIR)/$(BUILD_FLAG) $(DEPEND_LIB)


%.o:luaprofiler/%.cpp
	@$(ECHO) $(STR_COMPILE): $<
	@$(CC) $(CMODULE) $< $(INCLUDE)


%.d:luaprofiler/%.cpp
	@$(CC) -MM $(INCLUDE) $< > $@.tmp;\
	sed  's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.tmp > $@;\
	rm -f $@.tmp

ifneq ($(MAKECMDGOALS),clean)
include $(MODULES:.o=.d)
endif

clean:
	@$(ECHO) $(STR_CLEAN)
	@$(RM) -f *.o *.a *.d *.so
	@$(RM) -f $(TARGET_LIB)
