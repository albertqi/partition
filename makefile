partition: partition.cpp
	g++ -std=c++17 -O3 -o partition partition.cpp

clean:
	rm -f partition partition.o