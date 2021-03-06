TARGETS = mcts_test.x tictactoe_test.x ultimate_tictactoe_test.x benchmark.x bidding_game_test.x

OBJECTS = tictactoe.o tictactoe_utils.o ultimate_tictactoe.o thread_pool.o bidding_game.o

HEADER_ONLY = common.hpp memory_utils.hpp select.hpp default_policy.hpp backup.hpp mcts.hpp utils.hpp

CPPFLAGS=-Wall -Wextra -pedantic -Wno-sign-compare --std=c++17 -g -pthread
#CPPFLAGS=-Wall -Wextra -pedantic -Wno-sign-compare --std=c++17 -O3 -pthread

all: $(TARGETS) $(OBJECTS)

$(OBJECTS): %.o: %.cpp %.hpp
	g++ $(CPPFLAGS) -c $< -o $@

$(TARGETS): %.x: %.cpp $(OBJECTS) $(HEADER_ONLY)
	g++ $(CPPFLAGS) $< $(OBJECTS) -o $@

clean:
	rm -f $(TARGETS) $(OBJECTS)
