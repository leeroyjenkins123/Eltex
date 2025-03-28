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

RUNS = reverse_reading_string file_manager

all: $(RUNS)

file_manager: file_manager.c
	$(CC) $(CFLAGS) -o $@ $<

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(OBJ_DIR) 
	$(CC) -c $< -o $@

$(LIB): $(OBJS)
	$(AR) rc $@ $^

reverse_reading_string: reverse_reading_string.c $(LIB)
	$(CC) reverse_reading_string.c -o $@ $(LDFLAGS)

clean:
	rm -f $(RUNS)
	rm -rf $(OBJ_DIR)

.PHONY: all clean

