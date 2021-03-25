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
ALL: Project
%.o:%.cpp
	$(CPP) $(STD) $(CPPFLAG) $(DEBUG) -c $< $(DEPFLAG)
else ifeq ($(ver),release)
ALL: Project
%.o:%.cpp
	$(CPP) $(STD) $(CPPFLAG) $(RELEASE)-c $< $(DEPFLAG)
else
ALL: Project
%.o:%.cpp
	$(CPP) $(STD) $(CPPFLAG) -c $< $(DEPFLAG)
endif

.PHONY:Project
Project:$(objs)
	$(CPP) $(STD) $(CPPFLAG) -o $@ $^
	
.PHONY:clean
clean:	
	rm -f Project $(objs) $(deps) *.d 
