###############################################################################
# Makefile
###############################################################################

NAME=map
BIN=target
TARGET=$(BIN)/lib$(NAME)
TEST_TARGET=$(BIN)/test
SRC=map.c
TEST=test.c
OBJ=$(BIN)/$(SRC:.c=.o)
CC=gcc
OLEVEL= -O3
CFLAGS= -mavx -march=native
WFLAGS= -Wall -Wextra -Werror -Wunused

all: shared static

static: $(BIN)
	@$(CC) $(WFLAGS) $(CFLAGS) $(OLEVEL) -c $(SRC) -o $(OBJ)
	@ar -rcs $(TARGET).a $(OBJ)
	@echo "compiled: $(TARGET).a"

shared: $(BIN)
	@$(CC) -fPIC $(WFLAGS) $(CFLAGS) $(OLEVEL) -c $(SRC) -o $(OBJ)
	@$(CC) -shared -o $(TARGET).so $(OBJ)
	@echo "compiled: $(TARGET).so"

$(BIN):
	@mkdir -p $(BIN)

test: fclean static test-shared
	@$(CC) $(WFLAGS) $(CFLAGS) $(OLEVEL)  $(TEST) $(TARGET).a -o $(TEST_TARGET)
	@echo "compiled: tests for $(TARGET).a"
	@./$(TEST_TARGET)

test-shared: fclean shared
	@$(CC) $(WFLAGS) $(CFLAGS) $(OLEVEL)  $(TEST) -Wl,-rpath=$(BIN) -L$(BIN) -l$(NAME) -o $(TEST_TARGET)
	@echo "compiled: tests for $(TARGET).so"
	@./$(TEST_TARGET)

clean:
	@rm -f $(BIN)/*.o
	@echo "clean"

fclean:
	@rm -rf $(BIN)
	@echo "fclean"

re: fclean all

.PHONY: all static shared clean fclean test test-shared