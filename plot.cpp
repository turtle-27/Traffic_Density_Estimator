#include<iostream>
#include<fstream>
#include<stdio.h>
#include "gnuplot.h" //gnuplot.h is included "" is used since it is user defined header file
using namespace std;
int main()
{
    gnuplot p;
    // object of class gnuplot is created
    p("set term postscript eps"); 
    p("set output \"out.eps\" ");
    p("plot \'./utility_method1.dat\' u 1:2 w l ");
    return 0;
}