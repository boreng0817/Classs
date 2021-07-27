all: string dynamic_array function_overloading dynamic_min_max

string: string.o
	g++ -o string string.o
string.o: string.cc
	g++ -c string.cc
dynamic_array: dynamic_array.o
	g++ -o dynamic_array dynamic_array.o
dynamic_array.o: dynamic_array.cc
	g++ -c dynamic_array.cc
function_overloading: function_overloading.o
	g++ -o function_overloading function_overloading.o
function_overloading.o: function_overloading.cc
	g++ -c function_overloading.cc
dynamic_min_max: dynamic_min_max.o
	g++ -o dynamic_min_max dynamic_min_max.o
dynamic_min_max.o: dynamic_min_max.cc
	g++ -c dynamic_min_max.cc
clean:
	rm string string.o dynamic_array dynamic_array.o function_overloading function_overloading.o dynamic_min_max dynamic_min_max.o

