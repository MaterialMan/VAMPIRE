#!/bin/bash
run_folder=$PWD
rm -r Plots
mkdir Plots
rm -r plotdata
mkdir plotdata

tail -n +14 "cells-coords.cfg" > "$FILE.tmp" && mv "$FILE.tmp" "cells-coords.cfg"
awk 'BEGIN { FS="\t"; OFS="\t"; } {print $1,$2=$2,$3*1e-22,$4*1e-23,$5}' cells-coords.cfg > adjustedcells-coords.cfg

for i in `seq -f "%08g" 0 198`
do
tail -n +14 "cells-$i.cfg" > "$FILE.tmp" && mv "$FILE.tmp" "cells-$i.cfg"

paste adjustedcells-coords.cfg cells-$i.cfg cells_sources_output-$i.txt> plot$i.data
done
mv plot0* plotdata
rm cells*
rm adjustedcells-coords.cfg
