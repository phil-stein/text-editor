# silent echo calls
# MAKEFLAGS += --silent
# instead silencing individual calls with '@'


# compiler plus flags, -g: debug info, -Wall: all warnings
CC = @gcc -g -Wall

NAME = app.exe

SRC_DIR  = src
CORE_DIR = $(SRC_DIR)/core
MATH_DIR = $(SRC_DIR)/math
TEXT_DIR = $(SRC_DIR)/text
APP_DIR  = $(SRC_DIR)/app

OBJ_DIR = bin

# core
_OBJS =					\
	glad.o				\
	window.o			\
	file_io.o			\
	shader.o			\
	input.o				\
	texture.o			
# text
_OBJS += 				\
	text.o				\
	text_draw.o
# app
_OBJS += 				\
	main.o				\
	app.o					\
	cmd.o

# put the OBJ_DIR in front of the obj names
OBJS = $(patsubst %,$(OBJ_DIR)/%,$(_OBJS))
OBJS_WIN = $(patsubst %,$(OBJ_DIR)\\%,$(_OBJS))

LIB_DIR = libs

# name: libglfw.a, libfreetype.dll.a
_LIBS = 						\
	glfw3dll 					\
	freetype
LIBS     = $(patsubst %,-l %,$(_LIBS))
_INC = 							  \
		external					\
		external/FREETYPE \
		$(SRC_DIR)				\
		$(SRC_DIR)/core		\
		$(SRC_DIR)/math		\
		$(SRC_DIR)/text		\
		$(SRC_DIR)/app 
INC = $(patsubst %,-I %,$(_INC))

LIB_LINK = $(INC) -L $(LIB_DIR) $(LIBS) 


all: $(NAME)
	@echo ---- done ----

# compile all object files to .exe
$(NAME): $(OBJS) 
	$(CC) -o $(NAME) $(OBJS) $(LIB_LINK)

# compile all .c files to .o files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	echo $< -> $@	
	$(CC) -c $< -o $@ $(INC)

$(OBJ_DIR)/%.o: $(CORE_DIR)/%.c
	echo $< -> $@	
	$(CC) -c $< -o $@ $(INC)

$(OBJ_DIR)/%.o: $(TEXT_DIR)/%.c
	echo $< -> $@	
	$(CC) -c $< -o $@ $(INC)

$(OBJ_DIR)/%.o: $(APP_DIR)/%.c
	echo $< -> $@	
	$(CC) -c $< -o $@ $(INC)


# prevents make from fideling with files named clean
.PHONY: clean

clean:
	del $(NAME)
	del $(OBJS_WIN) 
	@echo ---- clean done ---- 

# clean and them make all
cleanm: clean all

run: clean all
	@echo --- $(NAME) --- 
	@$(NAME)
