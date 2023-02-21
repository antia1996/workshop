#!/bin/bash

in=/lustre/r3b/202104_s515/lmd

for i in $in/main*.lmd
do
    o=$(echo $i | sed 's,/\([^/]*lmd\),/stitched/ig4000,')
    $UCESB_DIR/empty/empty --input-buffer=100Mi $i --time-stitch=wr,4000 --output=$o --allow-errors
    
done
