#!/bin/bash

array=(0 0.1 0.25 0.5 0.75 0.875 0.9 0.925 0.95 0.98 0.99 1);
list=(basic quick race diag);

for item in ${array[*]}
do
	for type in ${list[*]}
	do
		echo "reset
set term postscript
set output \"./results/graphs/$type-$item.ps\"

set xdata
set key reverse Left outside
set grid
set title 'Track: $type Lambda: $item'
set ylabel 'Score'
set xlabel 'Number of training sessions'
set xrange [0:1000]
set style data lines
set autoscale y
#set yrange [-40:0]
plot \"./results/$type-$item.log\" using 1:4:3:7:6 with candlesticks lc rgb \"blue\" title \"General Data\" whiskerbars 0.5, \"\" using 1:5:5:5:5 with candlesticks lw 2 lt -1 title \"Median\", \"\" using 1:9 title \"Mean\" lc rgb \"red\", \"\" using 1:2 title \"Worst Case\", \"\" using 1:8 lc rgb \"green\" title \"Best Case\""  | gnuplot
	done
done 

