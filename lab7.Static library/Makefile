CC = gcc
AR = ar
LDFLAGS = -Lobj -lcalculator -lncurses

SRC_DIR = include
OBJ_DIR = obj
BIN_DIR = bin

SRCS = add.c div.c sub.c mult.c
OBJS = $(SRCS:.c=.o)
OBJS := $(addprefix $(OBJ_DIR)/, $(OBJS))
LIB = $(OBJ_DIR)/libcalculator.a
MAIN = main

all: $(MAIN)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(OBJ_DIR) 
	$(CC) -c $< -o $@

$(LIB): $(OBJS)
	$(AR) rc $@ $^

$(MAIN): main.c $(LIB)
	$(CC) main.c -o $@ $(LDFLAGS)

clean:
	rm -rf $(OBJ_DIR) $(LIB) $(MAIN)
