# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: nmartin <nmartin@student.42.fr>            +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/02/05 15:41:03 by nmartin           #+#    #+#              #
#    Updated: 2025/12/11 19:06:29 by nmartin          ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

YELLOW = \033[33m
GREEN = \033[32m
PURPLE = \033[35m
BLUE = \033[34m
RESET = \033[0m

CC = c++
CFLAGS = -std=c++98 -Wall -Werror -Wextra -MMD -MP -g3
NAME = webserv
WEB_PATH = ./web/
WEB_FILES = main.cpp Data.cpp
WEB := $(addprefix $(WEB_PATH), $(WEB_FILES))
REQUEST_PATH = ./request/
REQUEST_FILES = Connection.cpp get.cpp post.cpp CGI.cpp
REQUEST := $(addprefix $(REQUEST_PATH), $(REQUEST_FILES))
OBJ_PATH = ./objs/
OBJ := $(addprefix $(OBJ_PATH), $(WEB_FILES:.cpp=.o)) \
		$(addprefix $(OBJ_PATH), $(REQUEST_FILES:.cpp=.o))
SRC_BNS_PATH =
SRC_BNS_FILES =
SRC_BNS := $(addprefix $(SRC_BNS_PATH), $(SRC_BNS_FILES))
OBJ_BNS := $(addprefix $(OBJ_PATH), $(SRC_BNS_FILES:.cpp=.o))
HEADERS = -Iheaders
DEPS := $(OBJ:.o=.d)

all : $(NAME)

bonus : all

$(NAME) : $(OBJ)
	@printf "$(PUPLE)Creating $(NAME)...$(RESET)"
	@$(CC) $(CFLAGS) $(OBJ) -o $(NAME)
	@printf "\r\033[K"

$(OBJ_PATH)%.o : $(WEB_PATH)%.cpp
	@printf "$(BLUE)Compiling $(NAME) /webserv: [$<] $(RESET)"
	@mkdir -p $(OBJ_PATH)
	@$(CC) $(CFLAGS) $(HEADERS) -c $< -o $@
	@printf "\r\033[K"

$(OBJ_PATH)%.o : $(REQUEST_PATH)%.cpp
	@printf "$(BLUE)Compiling $(NAME) /request: [$<] $(RESET)"
	@mkdir -p $(OBJ_PATH)
	@$(CC) $(CFLAGS) $(HEADERS) -c $< -o $@
	@printf "\r\033[K"

$(OBJ_BNS_PATH)%.o : $(SRC_BNS_PATH)%.cpp
	@printf "$(BLUE)Compiling $(NAME) bonus: [$<] $(RESET)"
	@mkdir -p $(OBJ_BNS_PATH)
	@$(CC) $(CFLAGS) $(HEADERS) -c $< -o $@
	@printf "\r\033[K"

clean :
	@rm -rf $(OBJ_PATH)
	@printf "$(YELLOW)Cleaned successfully\n$(RESET)"

fclean :
	@rm -rf $(OBJ_PATH)
	@rm -rf $(NAME)
	@printf "\r\033[K$(YELLOW)Full cleaned successfully\n$(RESET)"

re : fclean all


exec : fclean all
	@make clean --no-print-directory

.PHONY : all clean fclean re	

-include $(DEPS)
