target:=monitor project
objs:=$(target:=.o)
ver:=test

CPP	:=g++
STD:= -std=c++11
CPPFLAG:=-Wall 
DEBUG :=-g -Ddebug
RELEASE:=-O2  

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
	$(CPP) $(STD) $(CPPFLAG) -o $@ $^
	
.PHONY:clean
clean:	
	rm -f Monitor $(objs) $(deps) *.d 
