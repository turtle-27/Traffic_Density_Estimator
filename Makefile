output: subtask2.cpp 
	g++ subtask2.cpp -o output -pthread -std=c++11 `pkg-config --cflags --libs opencv`	

.PHONY: clean 
clean:
	rm output
	rm Density.csv


