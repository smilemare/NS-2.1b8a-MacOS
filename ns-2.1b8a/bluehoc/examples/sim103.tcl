set Sim(Trace) ON
set Sim(AgentTrace) OFF
set Sim(NumDevices) 5
set Sim(Transport) [list TCP/Reno UDP UDP UDP]

set Sim(xpos_) [list 1.3 3.76 5.4 5.9 0.8]
set Sim(ypos_) [list 3.92 1.28 2.9 5.14 6.26]

set Sim(Application) [list FTP Traffic/Exponential Traffic/Exponential Traffic/Exponential]
set InqTimeout 32768
set NumResponses 4

set SimulationTime 20.0
set StartTime [list 0.0 0.1 0.2 0.3 0.4]

set InqScanOffset [list 3200 6400 3200 6400]

set Sim(NamFile) "out.nam"
source ./proc.tcl
source ./run.tcl
