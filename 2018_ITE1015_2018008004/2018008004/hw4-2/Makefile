all: print_people my_list

print_people:print_people.o
	g++ -o print_people print_people.o
print_people.o:print_people.cc
	g++ -c print_people.cc
my_list:my_list.o
	g++ -o my_list my_list.o
my_list.o:my_list.cc
	g++ -c my_list.cc
clean:
	rm print_people print_people.o my_list my_list.o

