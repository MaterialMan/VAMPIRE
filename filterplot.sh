#!/bin/bash
run_folder=$PWD

rm -r filterplots
mkdir filterplots

cd plotdata

for i in `seq -f "%08g" 0 90`
do


cat <<EOF >plot.gnu
set terminal pngcairo size 1200,600 font "Helvetica,14" background rgb 'gray'
set output 'out_config.png'
set size square
set cbrange [-4e-22:4e-22]
set xrange [0:12.5e-20]
set yrange [0:12.5e-21]
set zrange [-4e-22:4e-22]
set palette defined (-1 "white", 0 "grey", 1 "black")
set xlabel "x";set ylabel "y"; set zlabel "z"
plot "plot$i.data" u 3:4:8 w image
EOF

gnuplot plot.gnu
mv out_config.png ../filterplots/config_$i'.png'


done
