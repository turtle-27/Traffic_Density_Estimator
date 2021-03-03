output: homography.cpp 
	g++ homography.cpp -o output -pthread -std=c++11 `pkg-config --cflags --libs opencv`	

.PHONY: clean 
clean:
	rm output
	rm cropped_transformed_traffic.jpg
	rm transformed_traffic.jpg

