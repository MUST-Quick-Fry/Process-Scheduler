target:=utils job monitor scheduler scheduler_main
objs:=$(target:=.o)
ver:=test

CPP	:=g++
STD:= -std=c++11
CPPFLAG:=-Wall 
DEBUG :=-g -Ddebug
RELEASE:=-O2  
BUILD_DIR:=./build
OBJ_DIR:=./obj
EXEC_DIR:=./exec
EXEC=Scheduler
deps:=$(patsudst %.o,%.d,$(odjs))
-include $(deps)
DEPFLAG=-MMD -MF $(@:.o=.d)

ifeq ($(ver),debug)
ALL: $(EXEC)
%.o:%.cpp
	$(CPP) $(STD) $(CPPFLAG) $(DEBUG) -c $< $(DEPFLAG)
else ifeq ($(ver),release)
ALL: $(EXEC)
%.o:%.cpp
	$(CPP) $(STD) $(CPPFLAG) $(RELEASE) -c $< $(DEPFLAG)
else
ALL: $(EXEC)
%.o:%.cpp
	$(CPP) $(STD) $(CPPFLAG) -c $< $(DEPFLAG)
endif

.PHONY:$(EXEC)
$(EXEC):$(objs)
	@if [ ! -d $(OBJ_DIR) ];then mkdir -p $(OBJ_DIR);fi 
	@if [ ! -d $(BUILD_DIR) ];then mkdir -p $(BUILD_DIR);fi
	@if [ ! -d $(EXEC_DIR) ];then mkdir -p $(EXEC_DIR);fi  
	$(CPP) $(STD) $(CPPFLAG) -o $@ $^
	mv *.d $(BUILD_DIR)
	mv *.o $(OBJ_DIR)
	mv $@ $(EXEC_DIR)

.PHONY:clean
clean:	
	rm -rf $(EXEC_DIR) $(EXEC)
	find . -name "*.o" | xargs rm -f
	find . -name "*.d" | xargs rm -f