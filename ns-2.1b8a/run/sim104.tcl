set Sim(Trace) ON
set Sim(AgentTrace) OFF
set Sim(NumDevices) 6
set Sim(Transport) [list TCP/Reno UDP UDP UDP UDP]

set Sim(xpos_) [list 1.68 2.08 4.38 6.28 6.46 2.92]
set Sim(ypos_) [list 5.14 1.38 1.7 3.42 5.54 7.06]

set Sim(Application) [list FTP Traffic/Exponential Traffic/Exponential Traffic/Exponential Traffic/Exponential]
set InqTimeout 32768
set NumResponses 5

set SimulationTime 20.0
set StartTime [list 0.0 0.1 0.2 0.3 0.5 0.6]

set InqScanOffset [list 3200 960 1280 3200 2880]

set Sim(NamFile) "out.nam"
source ./proc.tcl
source ./run.tcl
