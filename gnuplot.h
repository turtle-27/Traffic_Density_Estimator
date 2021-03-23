#ifndef GNUPLOT_H
#define GNUPLOT_H
#include <stdio.h>
#include <iostream>
#include <string>
// included header files
using namespace std;
class gnuplot {

// aclass gnuplot is created
public:
        gnuplot();
        ~gnuplot();
        void operator () (const string & command);
protected:
FILE *gnuplotpipe;
};
gnuplot::gnuplot()
{
    gnuplotpipe = popen("gnuplot -persist", "w");
    if (!gnuplotpipe)
    cerr<<("Gnuplot not found !\n");
}

gnuplot::~gnuplot()
{
    fprintf(gnuplotpipe, "exit\n");
    pclose(gnuplotpipe);
}


void gnuplot::operator() (const string & command)
{
    fprintf(gnuplotpipe, "%s\n",command.c_str());
    fflush(gnuplotpipe);
}
#endif