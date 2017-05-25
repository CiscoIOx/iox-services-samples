ODIR=./obj
LDIR=./target
#INC_DIR=/usr/include
LIB_DIR=./sdk/libs
#LIB_DIR=/usr/libs

INC_DIR=sdk/include

#INC_FLAGS=-I $(INC_DIR)/jansson -I $(INC_DIR)/libuv -I $(INC_DIR)/mlib_api -I $(INC_DIR)/service_sdk -I $(INC_DIR)/protocol_infra -I $(INC_DIR)/metrics_infra -I $(INC_DIR)/utils -I $(INC_DIR)/dslink/col -I ./include

INC_FLAGS=-I $(INC_DIR) -I $(INC_DIR)/mlib_api -I $(INC_DIR)/service_sdk -I $(INC_DIR)/protocol_infra -I $(INC_DIR)/metrics_infra -I $(INC_DIR)/utils -I $(INC_DIR)/dslink/col -I ./include

CFLAGS=-std=gnu99 $(INC_FLAGS)

#LIB_OPTIONS=-lmlib -lcppmetricsshared -lsdk_dslink_c -lzlog -lglog -lboost_chrono -lboost_thread -lunwind
LIB_OPTIONS=-L $(LIB_DIR) -lmlib -lcppmetricsshared -lsdk_dslink_c -lzlog -lglog -lunwind -lboost_chrono -lboost_thread -lpthread

_OBJ = rpc_svc.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))
BIN = $(LDIR)/rpc_handler

$(BIN): $(OBJ)
	@echo "++++  Building the PH ==>> $@  ++++"
	$(CC) -o $@ $(OBJ) $(LIB_OPTIONS)
	cp resources/* $(LDIR)

$(ODIR)/%.o: %.c
	@echo "++++  Building the Object ==>> $@  ++++"
	$(CC) -c -o $@ $< $(CFLAGS) $(CPP_FLAGS)

all: $(OBJ) $(BIN)

.PHONY: clean

clean:
	@echo "++++  Cleaning the Objects  ++++"
	rm -f $(OBJ) $(LDIR)/*
