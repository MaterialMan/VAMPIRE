#!/bin/bash
run_folder=$PWD

cd plotdata

for i in `seq -f "%08g" 0 198`
do


cat <<EOF >plot.gnu
set terminal pngcairo size 1200,600 font "Helvetica,14" background rgb 'gray'
set output 'out_config.png'
set size square
set cbrange [-1:1]
set xrange [0:5e-20]
set yrange [0:5e-21]
set zrange [-4e-22:4e-22]
set palette defined (-1 "blue", 0 "white", 1 "red")
set xlabel "x";set ylabel "y"; set zlabel "z"
sp "plot$i.data" u 3:4:5:(\$6):(\$7):(\$8):12 w vectors palette lw 3 t 'M'
EOF

gnuplot plot.gnu
mv out_config.png ../Plots/config_$i'.png'


done
