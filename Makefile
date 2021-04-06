target:=monitor project
objs:=$(target:=.o)
ver:=test

CPP	:=g++
STD:= -std=c++11
CPPFLAG:=-Wall 
DEBUG :=-g -Ddebug
RELEASE:=-O2  
BUILD_DIR:=./build
OBJ_DIR:=./obj
deps:=$(patsudst %.o,%.d,$(odjs))
-include $(deps)
DEPFLAG=-MMD -MF $(@:.o=.d)

ifeq ($(ver),debug)
ALL: Monitor
%.o:%.cpp
	$(CPP) $(STD) $(CPPFLAG) $(DEBUG) -c $< $(DEPFLAG)
else ifeq ($(ver),release)
ALL: Monitor
%.o:%.cpp
	$(CPP) $(STD) $(CPPFLAG) $(RELEASE) -c $< $(DEPFLAG)
else
ALL: Monitor
%.o:%.cpp
	$(CPP) $(STD) $(CPPFLAG) -c $< $(DEPFLAG)
endif

.PHONY:Monitor
Monitor:$(objs)
	@if [ ! -d $(OBJ_DIR) ];then mkdir -p $(OBJ_DIR);fi 
	@if [ ! -d $(BUILD_DIR) ];then mkdir -p $(BUILD_DIR);fi 
	$(CPP) $(STD) $(CPPFLAG) -o $@ $^
	mv *.d $(BUILD_DIR)
	mv *.o $(OBJ_DIR)

.PHONY:clean
clean:	
	rm -f Monitor 
	find . -name "*.o" | xargs rm -f
	find . -name "*.d" | xargs rm -f