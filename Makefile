#NAME OF THE PROG#
NAME = 		dumb_packer

#FLAG AND COMPILATOR#
CC =		gcc
CFLAGS = 	-Wall -Werror -Wextra -g

NASM=		nasm
NFLAGS=		-f elf64

#PATH#
OBJ_PATH =  obj
SRC_PATH =  src
INC_PATH =  inc

#SUBDIR#
PARSER = parser/
INJECTOR = injector/

SRCS= 			read_elf.c \
			write_bin.c \
			add_section.c \
			loader_utils.c \
			cypher.c \
			main.c

OBJ = $(SRC:$(SRC_PATH)/%.c=$(OBJ_PATH)/%.o)
SRC = $(addprefix $(SRC_PATH)/,$(SRCS))

all: printmess payload $(NAME)

$(NAME): payload $(OBJ)
	gcc $(CFLAGS) -I$(INC_PATH) $(OBJ) -o $(NAME)

$(OBJ): $(OBJ_PATH)/%.o : $(SRC_PATH)/%.c
	@mkdir -p $(dir $@)
	gcc -o $@ $(CFLAGS) -I$(INC_PATH) -c $<

payload:
	nasm -f elf64 -o src/payload.o src/payload.asm
	ld -o payload src/payload.o
	nasm -f elf64 -o src/uncypher.o src/uncypher.asm
	ld -o uncypher src/uncypher.o

debug: $(OBJ)
	gcc -fsanitize=address -I$(INC_PATH) $(OBJ) -o $(NAME)

clean:
	/bin/rm -rf $(OBJ_PATH)

fclean: clean
	/bin/rm -f $(NAME)
	/bin/rm -f payload

re: fclean all

printmess:
	@echo "Compiling dumb packer"

.PHONY: all clean fclean re
