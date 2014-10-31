set Sim(Trace) ON
set Sim(AgentTrace) OFF
set Sim(NumDevices) 4
set Sim(Transport) [list TCP/Reno UDP UDP]

set Sim(xpos_) [list 1.52 4.62 6.1 4.78]
set Sim(ypos_) [list 4.1 1.56 3.92 7.52]

set Sim(Application) [list FTP Traffic/Exponential Traffic/Exponential]
set InqTimeout 32768
set NumResponses 3

set SimulationTime 20.0
set StartTime [list 0.0 0.1 0.2 0.3]

set InqScanOffset [list 3200 4160 3200]

set Sim(NamFile) "out.nam"
source ./proc.tcl
source ./run.tcl
