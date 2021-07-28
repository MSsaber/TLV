CURR_PATH =  $(abspath $(lastword $(MAKEFILE_LIST)))
CURR_DIR = $(dir $(CURR_PATH))

INC_DIR=-I$(CURR_DIR)

CPPSRCS += $(wildcard $(CURR_DIR)*.cpp)