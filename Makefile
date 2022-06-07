# @file Makefile
# @author 11796444 - Giovanni Shibaki Camargo 
# @author 11796451 - Pedro Kenzo Muramatsu Carmo
# @version 0.3
# @date 2021-07-10
# 
# @copyright Copyright (c) 2021

LIB = lib/clientManager.cpp lib/messageManager.cpp
SERVER_MAIN = src/server.cpp
CLIENT_MAIN = src/client.cpp
SERVER_BINARY = server
CLIENT_BINARY = client

FLAGS = -Werror
FLAGS2 = -Werror -Wall -Wextra -Wpedantic -Wpointer-arith -O3 -march=native
VFLAGS= --leak-check=full --show-leak-kinds=all --track-origins=yes

all:
	@g++ $(FLAGS) $(CLIENT_MAIN) $(LIB) -o $(CLIENT_BINARY) -lm -I src/ -I lib/ -g
	@g++ $(FLAGS) $(SERVER_MAIN) $(LIB) -o $(SERVER_BINARY) -lm -I src/ -I lib/ -g
run_client:
	./$(CLIENT_BINARY)
run_server:
	./$(SERVER_BINARY)
valval: all
	@echo "Looking for leaks!"
	@valgrind $(VFLAGS) ./$(CLIENT_BINARY)

deb_hex: all
	@echo "Hexdumping it all!"
	@./$(BINARY) < test/0.in &> test/0.out
	@ hexdump -vC bin/veiculo.bin > 0.hexdump
	@ code 0.hexdump

clean:
	@echo "Cleaning the mess!"
	@rm $(BINARY)
	@rm *.zip

zip:
	@echo "Creating a zip folder with all the important files!"
	@zip -r trabalho_internet_relay_chat.zip $(CLIENT_MAIN) $(SERVER_MAIN) ./lib/* ./src/* ./assets/* Makefile README.md