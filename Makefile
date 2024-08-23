SRCS = main.c sort.c tabular.c

SRC_DIR = srcs/
OBJS = $(SRCS:.c=.o)
OBJ_DIR = objs/

CC = gcc
CFLAGS = -Wextra -Wall -Werror

NAME = ft_ls
INCLUDES = -Iincludes

$(OBJ_DIR)%o: $(SRC_DIR)%c
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

all: $(NAME)

$(NAME): $(addprefix $(OBJ_DIR), $(OBJS))
	$(CC) $(addprefix $(OBJ_DIR), $(OBJS)) $(INCLUDES) $(LIBS) -o $(NAME)

clean:
	rm -rf $(addprefix $(OBJ_DIR), $(OBJS))

fclean: clean
	rm -rf $(NAME)

re: fclean all
