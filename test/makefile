include ../sub.mk
BIN = tlv_test

CPPFLAGS += $(INC_DIR) -Wall

CPPSRCS += ./main.cpp

CPPOBJS += $(patsubst %.cpp, %.o, $(CPPSRCS))

$(BIN) : $(CPPOBJS)
	g++ $^ -o $(BIN)
	rm $(CPPOBJS)

$(CPPOBJS) : %.o : %.cpp
	g++ -c $(CPPFLAGS) $< -o $@

#.PHONY ： clean
clean :
	rm $(CPPOBJS)