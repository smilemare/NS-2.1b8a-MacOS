set Sim(Trace) ON
set Sim(AgentTrace) OFF
set Sim(NumDevices) 8
set Sim(Transport) [list UDP UDP UDP UDP UDP UDP UDP]

set Sim(xpos_) [list 1.46 2.96 3.76 7.7 7.32 7.12 6.02 1.96]
set Sim(ypos_) [list 4.54 1.4 3.16 2.66 4.14 5.76 6.96 6.74]

set Sim(Application) [list Traffic/Exponential Traffic/Exponential Traffic/Exponential Traffic/Exponential Traffic/Exponential Traffic/Exponential Traffic/Exponential]
set InqTimeout 32768
set NumResponses 7

set SimulationTime 20.0
set StartTime [list 0.0 0.2 0.1 0.3 0.4 0.5 0.6 0.7]

set InqScanOffset [list 6400 3200 3840 4160 640 3840 2880]

set Sim(NamFile) "out.nam"
source ./proc.tcl
source ./run.tcl
