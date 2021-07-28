include ./sub.mk

OUT = libtlv.a

CPPFLAGS += $(INC_DIR) -Wall

CPPOBJS += $(patsubst %.cpp, %.o, $(CPPSRCS))

$(OUT) : $(CPPOBJS)
	ar -rv $(OUT) $^ 
	rm $(CPPOBJS)

$(CPPOBJS) : %.o : %.cpp
	g++ -c $(CPPFLAGS) $< -o $@

#.PHONY ： clean
clean :
	rm $(CPPOBJS) $(OUT)