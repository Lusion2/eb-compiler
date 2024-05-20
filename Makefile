CC = gcc
CFLAGS = -Wall -Wextra

# change address to leak for mem leaks
DEBUG_MEM_INFO = #-fsanitize=leak

GEN_DIR = generate/
PARSE_DIR = parse/
TOKEN_DIR = token/

SRC_DIR = ./src/
OBJ_DIR = ./objs/

SRCS = $(wildcard $(SRC_DIR)*/*.c)
SRCS += $(wildcard $(SRC_DIR)*.c)
OBJS = $(SRCS:$(SRC_DIR)%.c=$(OBJ_DIR)%.o)

all : eb exec

eb : $(OBJS)
	$(CC) -std=c11 $(DEBUG_MEM_INFO) -o eb $(OBJS) -g

$(OBJS): $(OBJ_DIR)%.o: $(SRC_DIR)%.c Makefile
	$(CC) $(CFLAGS) -c -o $@ $< -ggdb3

exec : \
	./eb test.eb
