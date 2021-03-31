# This folder contains: 
    - source code: subtask3.cpp 
    - Makefile 
    - "Baseline.txt" containing values to queue density values for baseline method.

# Terminal commands to:
## 1. To compile the source code 
    - use make
    - builds an output file named "output"

## 2. Run output file 
    - for "videofilename" and "method"   : ./output videofilename  method
    - for example:- for "trafficvideo.mp4" and "method 1" : ./output trafficvideo.mp4 1
    - not using this format will display the format to be used on the terminal
    - on running the executable in desired format it will produce a dat file "Utility_method<Method_no.>.dat"     containing the queue density values obtained through that particular method. 
    - while the code is running "loading..." will be displayed on the terminal and "Done." will be displayed after completion.
 
## 3. Delete output files
    - to delete output files: make clean
