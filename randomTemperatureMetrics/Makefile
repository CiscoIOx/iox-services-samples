ODIR=./obj
LDIR=./target
INC_DIR=sdk/include
LIB_DIR=./sdk/libs

INC_FLAGS=-I $(INC_DIR) -I $(INC_DIR)/mlib_api -I $(INC_DIR)/service_sdk -I $(INC_DIR)/protocol_infra -I $(INC_DIR)/metrics_infra -I $(INC_DIR)/utils -I $(INC_DIR)/dslink/col

CFLAGS=-std=gnu99 $(INC_FLAGS)

LIB_OPTIONS=-L $(LIB_DIR) -lmlib -lcppmetricsshared -lsdk_dslink_c -lzlog -lglog -lunwind -lboost_chrono -lboost_thread

_OBJ = random_tmp_metrics.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))
BIN = $(LDIR)/random_tmp_metrics

$(BIN): $(OBJ)
	@echo "++++  Building the PH ==>> $@  ++++"
	$(CC) -o $@ $(OBJ) $(LIB_OPTIONS)

$(ODIR)/%.o: %.c
	@echo "++++  Building the Object ==>> $@  ++++"
	$(CC) -c -o $@ $< $(CFLAGS) $(CPP_FLAGS)
	cp ./resources/* $(LDIR)

all: $(OBJ) $(BIN)

.PHONY: clean

clean:
	@echo "++++  Cleaning the Objects  ++++"
	rm -f $(OBJ) $(LDIR)/*
