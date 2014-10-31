set Sim(Trace) ON
set Sim(AgentTrace) OFF
set Sim(NumDevices) 4
set Sim(Transport) [list TCP/Reno TCP/Reno UDP]

set Sim(xpos_) [list 1.68 5.64 7.14 2.52]
set Sim(ypos_) [list 3.28 0.42 5.32 6.78]

set Sim(Application) [list FTP Telnet Traffic/Exponential]
set InqTimeout 32768
set NumResponses 3

set SimulationTime 20.0
set StartTime [list 0.0 0.1 0.2 0.3]

set InqScanOffset [list 3200 6400 2560]

set Sim(NamFile) "out.nam"
source ./proc.tcl
source ./run.tcl
