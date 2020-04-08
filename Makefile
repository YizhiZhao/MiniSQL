SRC := $(wildcard src/*/*.cpp)
OBJ := $(patsubst src%.cpp, obj%.o, $(SRC))

minisql: $(OBJ)
	@echo linking
	g++ $(OBJ) -o minisql

obj/%.o: src/%.cpp
	@echo compiling
	@mkdir -p $(dir $@)
	g++ -c $^ -o $@

clean:
	rm -rf obj/
