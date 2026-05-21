# verified boost version: 1.74
all: test_ostream test_istream
%.o: %.cpp
	g++ -c -g -std=c++14 $< -o $@
%: %.o
	g++ -g $< -o $@ -lboost_serialization
clean:
	-rm -f test_ostream test_istream *.o
include .dep
