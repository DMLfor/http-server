##########################################################
#	关于编译环境的一些设定
##########################################################
CXX:=g++

CFLAGS:= -O2 -g -Wall -std=c++11 -pthread
LIBS:= 
LIBPATH:=../lib
INCLUDE:= ../src  ../include
SRC:=  ./*.c 
TARGETS:= http
##########################################################
#	从这里开始,不用改变任何东西
##########################################################

INCLUDES:=$(addprefix -I,$(INCLUDE))
CFLAGS:=$(CFLAGS) $(INCLUDES)
SOURCES:=$(wildcard $(SRC))  
OBJS:=$(patsubst %.c,%.o,$(SOURCES))

all:$(TARGETS)


.PHONY:clean cleanobj cleantarget cleandeps all

cleanobj:
	-rm -f $(OBJS)

cleantarget:
	-rm -f $(TARGETS)


clean:cleanobj cleantarget


$(TARGETS):$(OBJS)
	$(CXX) $(CFLAGS)   $^ -o $@ $(addprefix -L,$(LIBPATH)) $(addprefix -l,$(LIBS))


%.o:%.c
	$(CXX) -c $(CFLAGS)   $^ -o $@





