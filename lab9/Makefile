CC = gcc
CFLAGS = -Wall -Werror
AR = ar
LDFLAGS = -Lobj -lfilemanager -lncurses
SRC_DIR = include
OBJ_DIR = obj

SRCS = change_directory.c display_panels.c init_panels.c load_files.c menu.c
OBJS = $(SRCS:.c=.o)
OBJS := $(addprefix $(OBJ_DIR)/, $(OBJS))
LIB = $(OBJ_DIR)/libfilemanager.a

RUNS = lab_9_1 lab_9_2

all: $(RUNS)

lab_9_1: lab9_1.c
	$(CC) $(CFLAGS) -o $@ $<

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(OBJ_DIR) 
	$(CC) -c $< -o $@

$(LIB): $(OBJS)
	$(AR) rc $@ $^

lab_9_2: lab_9_2.c $(LIB)
	$(CC) lab_9_2.c -o $@ $(LDFLAGS)

clean:
	rm -f $(RUNS)
	rm -rf $(OBJ_DIR)

.PHONY: all clean

