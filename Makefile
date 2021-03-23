output: rough3.cpp 
	g++ rough3.cpp -o output -pthread -std=c++11 `pkg-config --cflags --libs opencv` 	

.PHONY: clean 
clean:
	rm output