CC = gcc
LDFLAGS = -Lobj -lcalculator
SHAREDLIB = 

SRC_DIR = include
OBJ_DIR = obj
BIN_DIR = bin

SRCS = add.c div.c sub.c mult.c
OBJS = $(SRCS:.c=.o)
OBJS := $(addprefix $(OBJ_DIR)/, $(OBJS))
LIB = $(OBJ_DIR)/libcalculator.so
MAIN = main

all: $(MAIN)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(OBJ_DIR) 
	$(CC) -fPIC -c $< -o $@

$(LIB): $(OBJS)
	$(CC) -shared -o $@  $^

$(MAIN): main.c $(LIB)
	$(CC) main.c -o $@ $(LDFLAGS)

run: $(MAIN)
	LD_LIBRARY_PATH=$(OBJ_DIR):$$LD_LIBRARY_PATH ./$(MAIN)

clean:
	rm -rf $(OBJ_DIR) $(LIB) $(MAIN)
