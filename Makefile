rollingtorus : torus.o main.o
	gcc -Wall -O3 -funroll-loops -lgsl -lcblas -latlas -lglut -lGL -lGLU -lm -o rollingtorus torus.o main.o

main.o : main.c
	gcc -Wall -O3 -funroll-loops -c main.c

torus.o : torus.c torus.h
	gcc -Wall -O3 -funroll-loops -c torus.c

clean :
	rm -rf torus.o main.o rollingtorus
