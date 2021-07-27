all: array_2dim touch_points cal_complex

array_2dim: array_2dim.o
	g++ -o array_2dim array_2dim.o
array_2dim.o: array_2dim.cc
	g++ -c array_2dim.cc
touch_points: touch_points.o
	g++ -o touch_points touch_points.o
touch_points.o: touch_points.cc
	g++ -c touch_points.cc
cal_complex: cal_complex.o
	g++ -o cal_complex cal_complex.o
cal_complex.o: cal_complex.cc
	g++ -c cal_complex.cc
clean:
	rm array_2dim touch_points cal_complex array_2dim.o touch_points.o cal_complex.o 

