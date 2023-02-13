target= main
src= $(wildcard *.cpp)
object= $(patsubst %.cpp, %.o, $(src))
CC= g++

$(target): $(object)
	$(CC) -o $@ $^ -I ../benchmark -I magic_template -std=c++17

%.o: %.c 
	$(CC) -o $@ -c $< -std=c++17

.PHONY: clean
clean:
	-rm -f $(target) $(object)