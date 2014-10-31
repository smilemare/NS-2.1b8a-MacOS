# To run this:  "csh FlowAllB.v2.com".
#
set randomseed=12345
set filename=fairflow.xgr
set num=20; set file=Flow4B
# With RED: 
../../ns $file.v2.tcl combined $randomseed
## To make S graphs:
csh Diagonal1.com $filename.c $num Combined All
##
## To make S-graphs for the combined flow:
csh Diagonal3.com $filename.c.4 $num
csh Time.com fairflow.rpt.sec $num
csh Drops.com fairflow.rpt.drop fairflow.rpt.forced $num
csh DropRatio.com fairflow2.xgr $num
csh ArrivalRate.com $filename.c.5 $num
cp *.$num.ps ~/paper/red/penalty/figures
