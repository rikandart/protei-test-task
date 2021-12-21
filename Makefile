SERVER = server
CLIENT = client
PARSER_TST = parser_tst
LIB_SRC = $(wildcard src/lib.*)
SRC_SERVER = $(wildcard src/server.cpp src/parser.cpp src/lib.cpp)
SRC_CLIENT = $(wildcard src/client.cpp src/lib.cpp)
SRC_PARSER_TST = $(wildcard tests/parser_tst.cpp src/parser.cpp)
BIN_SERVER = $(addprefix bin/, $(SERVER))
BIN_CLIENT = $(addprefix bin/, $(CLIENT))
BIN_PARSER_TST = $(addprefix bin/, $(PARSER_TST))
FLAGS = -Wall -Isrc/ -std=c++1z -pthread
TEST_FLAGS = -Igoogletest/googletest/include -Igoogletest/googlemock/include \
			-lgtest -lgtest_main -lgmock -lgmock_main
			#-Lgoogletest/build/lib 
			

all: $(BIN_SERVER) $(BIN_CLIENT) $(BIN_PARSER_TST)
$(BIN_SERVER): $(SRC_SERVER)
	g++ $^ -o $@ $(FLAGS)

$(BIN_CLIENT): $(SRC_CLIENT)
	g++ $^ -o $@ $(FLAGS)

$(BIN_PARSER_TST): $(SRC_PARSER_TST)
	g++ $^ -o $@ $(FLAGS) $(TEST_FLAGS)
