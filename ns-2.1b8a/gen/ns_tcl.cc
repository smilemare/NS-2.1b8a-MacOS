static const char code[] = "\n\
\n\
\n\
\n\
\n\
\n\
set slinks_(0:0) 0\n\
set nconn_ 0 \n\
set conn_(0) 0:0\n\
\n\
proc warn {msg} {\n\
global warned_\n\
if {![info exists warned_($msg)]} {\n\
puts stderr \"warning: $msg\"\n\
set warned_($msg) 1\n\
}\n\
}\n\
\n\
if {[info commands debug] == \"\"} {\n\
proc debug args {\n\
warn {Script debugging disabled.  Reconfigure with --with-tcldebug, and recompile.}\n\
}\n\
}\n\
\n\
proc assert args {\n\
if [catch \"expr $args\" ret] {\n\
set ret [eval expr $args]\n\
}\n\
if {! $ret} {\n\
error \"assertion failed: $args\"\n\
}\n\
}\n\
\n\
proc find-max list {\n\
set max 0\n\
foreach val $list {\n\
if {$val > $max} {\n\
set max $val\n\
}\n\
}\n\
return $max\n\
}\n\
\n\
proc bw_parse { bspec } {\n\
if { [scan $bspec \"%f%s\" b unit] == 1 } {\n\
set unit bps\n\
}\n\
regsub {[/p]s(ec)?$} $unit {} unit\n\
if [string match {*B} $unit] {\n\
set b [expr $b*8]\n\
set unit \"[string trimright B $unit]b\"\n\
}\n\
switch $unit {\n\
b { return $b }\n\
kb { return [expr $b*1000] }\n\
Mb { return [expr $b*1000000] }\n\
Gb { return [expr $b*1000000000] }\n\
default { \n\
puts \"error: bw_parse: unknown unit `$unit'\" \n\
exit 1\n\
}\n\
}\n\
}\n\
\n\
proc time_parse { spec } {\n\
if { [scan $spec \"%f%s\" t unit] == 1 } {\n\
set unit s\n\
}\n\
regsub {sec$} $unit {s} unit\n\
switch $unit {\n\
s { return $t }\n\
ms { return [expr $t*1e-3] }\n\
us { return [expr $t*1e-6] }\n\
ns { return [expr $t*1e-9] }\n\
ps { return [expr $t*1e-12] }\n\
default { \n\
puts \"error: time_parse: unknown unit `$unit'\" \n\
exit 1\n\
}\n\
}\n\
}\n\
\n\
proc delay_parse { spec } {\n\
return [time_parse $spec]\n\
}\n\
\n\
\n\
\n\
\n\
proc checkout_executable {exe_var best alternate text} {\n\
global $exe_var\n\
set $exe_var $best\n\
if {\"$best\" == \"\" || ![file executable $best]} {\n\
puts stderr $text\n\
set $exe_var $alternate\n\
}\n\
}\n\
\n\
checkout_executable PERL \"/usr/bin/perl\" perl \"\\\n\
When configured, ns found the right version of perl in\\\n\
/usr/bin/perl\n\
but it doesn't seem to be there anymore, so\\\n\
ns will fall back on running the first perl in your path.\\\n\
The wrong version of perl may break the test suites.\\\n\
Reconfigure and rebuild ns if this is a problem.\\\n\
\"\n\
checkout_executable TCLSH \"/Users/smilemare/project/ns-allinone-2.1b8a/bin/tclsh8.3\" tclsh \"\\\n\
When configured, ns found the right version of tclsh in\\\n\
/Users/smilemare/project/ns-allinone-2.1b8a/bin/tclsh8.3\n\
but it doesn't seem to be there anymore, so\\\n\
ns will fall back on running the first tclsh in your path.\\\n\
The wrong version of tclsh may break the test suites.\\\n\
Reconfigure and rebuild ns if this is a problem.\\\n\
\"\n\
\n\
\n\
\n\
\n\
\n\
\n\
Class AllocAddrBits\n\
\n\
Simulator proc address-format {} {\n\
return [Simulator set AddressFormat_]\n\
}\n\
\n\
Simulator proc default-addr? {} {\n\
if { [Simulator set AddressFormat_] == \"DEF\" } {\n\
return 1\n\
} else { \n\
return 0\n\
}\n\
}\n\
\n\
Simulator proc hier-addr? {} {\n\
if { [Simulator set AddressFormat_] == \"HIER\" } {\n\
return 1\n\
} else {\n\
return 0\n\
}\n\
}\n\
\n\
Simulator instproc set-address-format {opt args} {\n\
set len [llength $args]\n\
if {[string compare $opt \"def\"] == 0} {\n\
$self set-address 32\n\
set mcastshift [AddrParams McastShift]\n\
Simulator set McastAddr_ [expr 1 << $mcastshift]\n\
mrtObject expandaddr\n\
Simulator set AddressFormat_ DEF\n\
} elseif {[string compare $opt \"expanded\"] == 0} {\n\
puts \"set-address-format expanded is obsoleted by 32-bit addressing.\"\n\
} elseif {[string compare $opt \"hierarchical\"] == 0 && $len == 0} {\n\
if [$self multicast?] {\n\
$self set-hieraddress 3 9 11 11\n\
} else {\n\
$self set-hieraddress 3 10 11 11\n\
}\n\
} elseif {[string compare $opt \"hierarchical\"] == 0 && $len > 0} {\n\
eval $self set-hieraddress [lindex $args 0] \\\n\
[lrange $args 1 [expr $len - 1]]\n\
} else {\n\
error \"ns-address.tcl:set-address-format: Unknown address format $opt\"\n\
}\n\
}\n\
\n\
Simulator instproc set-hieraddress {hlevel args} {\n\
set a [$self get-AllocAddrBits \"new\"]\n\
$a set size_ [AllocAddrBits set MAXADDRSIZE_]\n\
Simulator set AddressFormat_ HIER\n\
Node enable-module \"Hier\"\n\
if [$self multicast?] {\n\
$a set-mcastbits 1\n\
}\n\
eval $a set-idbits $hlevel $args\n\
}\n\
\n\
Simulator instproc set-address {node} {\n\
set a [$self get-AllocAddrBits \"new\"]\n\
$a set size_ [AllocAddrBits set DEFADDRSIZE_]\n\
if {[expr $node] > [$a set size_]} {\n\
$a set size_ [AllocAddrBits set MAXADDRSIZE_]\n\
}\n\
\n\
$a set-mcastbits 1\n\
set lastbit [expr $node - [$a set mcastsize_]]\n\
$a set-idbits 1 $lastbit\n\
}\n\
\n\
Simulator instproc get-AllocAddrBits {prog} {\n\
$self instvar allocAddr_\n\
if ![info exists allocAddr_] {\n\
set allocAddr_ [new AllocAddrBits]\n\
} elseif ![string compare $prog \"new\"] {\n\
set allocAddr_ [new AllocAddrBits]\n\
}\n\
return $allocAddr_\n\
}\n\
\n\
Simulator instproc expand-port-field-bits nbits {\n\
puts \"Warning: Simulator::expand-port-field-bits is obsolete.  Ports are 32 bits wide\"\n\
return\n\
}\n\
\n\
Simulator instproc expand-address {} {\n\
puts \"Warning: Simulator::expand-address is obsolete.  The node address is 32 bits wides\"\n\
return\n\
}\n\
\n\
\n\
AllocAddrBits instproc init {} {\n\
eval $self next\n\
$self instvar size_ portsize_ idsize_ mcastsize_\n\
set size_ 0\n\
set portsize_ 0\n\
set idsize_ 0\n\
set mcastsize_ 0\n\
}\n\
\n\
AllocAddrBits instproc get-AllocAddr {} {\n\
$self instvar addrStruct_\n\
if ![info exists addrStruct_] {\n\
set addrStruct_ [new AllocAddr]\n\
}\n\
return $addrStruct_\n\
}\n\
\n\
AllocAddrBits instproc get-Address {} {\n\
$self instvar address_\n\
if ![info exists address_] {\n\
set address_ [new Address]\n\
}\n\
return $address_\n\
}\n\
\n\
AllocAddrBits instproc chksize {bit_num prog} {\n\
$self instvar size_ portsize_ idsize_ mcastsize_  \n\
if {$bit_num <= 0 } {\n\
error \"$prog : \\# bits less than 1\"\n\
}\n\
set totsize [expr $portsize_ + $idsize_ + $mcastsize_]\n\
if {$totsize > [AllocAddrBits set MAXADDRSIZE_]} {\n\
error \"$prog : Total \\# bits exceed MAXADDRSIZE\"\n\
}\n\
if { $size_ < [AllocAddrBits set MAXADDRSIZE_]} {\n\
if {$totsize > [AllocAddrBits set DEFADDRSIZE_]} {\n\
set size_ [AllocAddrBits set MAXADDRSIZE_]\n\
return 1\n\
} \n\
}\n\
return 0\n\
\n\
}\n\
\n\
AllocAddrBits instproc set-portbits {bit_num} {\n\
puts \"Warning: AllocAddrBits::set-portbits is obsolete.  Ports are 32 bits wide.\"\n\
return\n\
}\n\
\n\
AllocAddrBits instproc expand-portbits nbits {\n\
puts \"Warning: AllocAddrBits::expand-portbits is obsolete.  Ports are 32 bits wide.\"\n\
return\n\
}\n\
\n\
AllocAddrBits instproc set-mcastbits {bit_num} {\n\
$self instvar size_ mcastsize_\n\
if {$bit_num != 1} {\n\
error \"setmcast : mcastbit > 1\"\n\
}\n\
set mcastsize_ $bit_num\n\
\n\
if [$self chksize mcastsize_ \"setmcast\"] {\n\
error \"set-mcastbits: size_ has been changed.\"\n\
}\n\
set a [$self get-AllocAddr] \n\
set v [$a setbit $bit_num $size_]\n\
AddrParams McastMask [lindex $v 0]\n\
AddrParams McastShift [lindex $v 1]\n\
\n\
set ad [$self get-Address]\n\
$ad mcastbits-are [AddrParams McastShift] [AddrParams McastMask]\n\
}\n\
\n\
AllocAddrBits instproc set-idbits {nlevel args} {\n\
$self instvar size_ portsize_ idsize_ hlevel_ hbits_\n\
if {$nlevel != [llength $args]} {\n\
error \"setid: hlevel < 1 OR nlevel and \\# args donot match\"\n\
}\n\
set a [$self get-AllocAddr] \n\
set old 0\n\
set idsize_ 0\n\
set nodebits 0\n\
AddrParams hlevel $nlevel\n\
set hlevel_ $nlevel\n\
for {set i 0} {$i < $nlevel} {incr i} {\n\
set bpl($i) [lindex $args $i]\n\
set idsize_ [expr $idsize_ + $bpl($i)]\n\
\n\
set chk [$self chksize $bpl($i) \"setid\"]\n\
if {$chk > 0} {\n\
error \"set-idbits: size_ has been changed.\"\n\
}\n\
set v [$a setbit $bpl($i) $size_]\n\
AddrParams NodeMask [expr $i+1] [lindex $v 0]\n\
set m([expr $i+1]) [lindex $v 0]\n\
AddrParams NodeShift [expr $i+1] [lindex $v 1]\n\
set s([expr $i+1]) [lindex $v 1]\n\
lappend hbits_ $bpl($i)\n\
\n\
}\n\
AddrParams nodebits $idsize_\n\
set ad [$self get-Address]\n\
eval $ad idsbits-are [array get s]\n\
eval $ad idmbits-are [array get m]\n\
eval $ad bpl-are $hbits_\n\
}\n\
\n\
\n\
AddrParams proc addr2id addrstr {\n\
if [Simulator hier-addr?] {\n\
set addressObj [[[Simulator instance] get-AllocAddrBits \"\"] \\\n\
get-Address]\n\
return [$addressObj str2addr $addrstr]\n\
} else {\n\
return [expr $addrstr & [AddrParams NodeMask 1] << \\\n\
[AddrParams NodeShift 1]]\n\
}\n\
}\n\
\n\
AddrParams proc id2addr addr {\n\
for {set i 1} {$i <= [AddrParams hlevel]} {incr i} {\n\
set a [expr ($addr >> [AddrParams NodeShift $i]) & \\\n\
[AddrParams NodeMask $i]]\n\
lappend str $a\n\
}\n\
return $str\n\
}\n\
\n\
AddrParams proc split-addrstr addrstr {\n\
return [split $addrstr .]\n\
}\n\
\n\
AddrParams proc elements-in-level? {nodeaddr level} {\n\
AddrParams instvar domain_num_ cluster_num_ nodes_num_ \n\
set L [AddrParams split-addrstr $nodeaddr]\n\
set level [expr $level + 1]\n\
\n\
if { $level == 1} {\n\
return $domain_num_\n\
}\n\
if { $level == 2} {\n\
return [lindex $cluster_num_ [lindex $L 0]]\n\
}\n\
if { $level == 3} {\n\
set C 0\n\
set index 0\n\
while {$C < [lindex $L 0]} {\n\
set index [expr $index + [lindex $cluster_num_ $C]]\n\
incr C\n\
}\n\
return [lindex $nodes_num_ [expr $index + [lindex $L 1]]]\n\
}\n\
}\n\
\n\
Node set nn_ 0\n\
Node proc getid {} {\n\
set id [Node set nn_]\n\
Node set nn_ [expr $id + 1]\n\
return $id\n\
}\n\
\n\
Node set module_list_ { Base }\n\
\n\
Node proc enable-module { mod_name } {\n\
Node instvar module_list_\n\
if { [lsearch $module_list_ $mod_name] < 0 } {\n\
lappend module_list_ $mod_name\n\
}\n\
}\n\
\n\
Node proc disable-module { mod_name } {\n\
Node instvar module_list_\n\
set pos [lsearch $module_list_ $mod_name]\n\
if { $pos >= 0 } {\n\
set module_list_ [lreplace $module_list_ $pos $pos]\n\
}\n\
}\n\
\n\
Node instproc init args {\n\
eval $self next $args\n\
\n\
$self instvar id_ agents_ dmux_ neighbor_ rtsize_ address_ \\\n\
nodetype_ multiPath_ ns_ rtnotif_ ptnotif_\n\
\n\
set ns_ [Simulator instance]\n\
set id_ [Node getid]\n\
$self nodeid $id_	;# Propagate id_ into c++ space\n\
\n\
if {[llength $args] != 0} {\n\
set address_ [lindex $args 0]\n\
} else {\n\
set address_ $id_\n\
}\n\
$self cmd addr $address_; # new by tomh\n\
set neighbor_ \"\"\n\
set agents_ \"\"\n\
set dmux_ \"\"\n\
set rtsize_ 0\n\
set ptnotif_ {}\n\
set rtnotif_ {}\n\
set nodetype_ [$ns_ get-nodetype]\n\
\n\
$self mk-default-classifier\n\
\n\
set multiPath_ [$class set multiPath_]\n\
}\n\
\n\
Node instproc mk-default-classifier {} {\n\
Node instvar module_list_\n\
foreach modname [Node set module_list_] {\n\
$self register-module [new RtModule/$modname]\n\
}\n\
}\n\
\n\
Node instproc id {} {\n\
return [$self set id_]\n\
}\n\
\n\
Node instproc node-addr {} {\n\
return [$self set address_]\n\
}\n\
\n\
Node instproc node-type {} {\n\
return [$self set nodetype_]\n\
}\n\
\n\
\n\
Node instproc register-module { mod } {\n\
$self instvar reg_module_\n\
$mod register $self\n\
set reg_module_([$mod module-name]) $mod\n\
}\n\
\n\
Node instproc unregister-module { mod } {\n\
$self instvar reg_module_\n\
$mod unregister\n\
unset reg_module_([$mod module-name])\n\
delete $mod\n\
}\n\
\n\
Node instproc list-modules {} {\n\
$self instvar reg_module_\n\
set ret \"\"\n\
foreach n [array names reg_module_] {\n\
lappend ret $reg_module_($n)\n\
}\n\
return $ret\n\
}\n\
\n\
Node instproc get-module { name } {\n\
$self instvar reg_module_\n\
if [info exists reg_module_($name)] {\n\
return $reg_module_($name)\n\
} else {\n\
return \"\"\n\
}\n\
}\n\
\n\
\n\
\n\
Node instproc incr-rtgtable-size {} {\n\
$self instvar rtsize_\n\
incr rtsize_\n\
}\n\
\n\
Node instproc decr-rtgtable-size {} {\n\
$self instvar rtsize_\n\
incr rtsize_ -1\n\
}\n\
\n\
Node instproc entry {} {\n\
return [$self set classifier_]\n\
}\n\
\n\
Node instproc insert-entry { module clsfr {hook \"\"} } {\n\
$self instvar classifier_ mod_assoc_ hook_assoc_\n\
if { $hook != \"\" } {\n\
set hook_assoc_($clsfr) $classifier_\n\
if { $hook == \"target\" } {\n\
$clsfr target $classifier_\n\
} elseif { $hook != \"\" } {\n\
$clsfr install $hook $classifier_\n\
}\n\
}\n\
set mod_assoc_($clsfr) $module\n\
set classifier_ $clsfr\n\
}\n\
\n\
Node instproc install-entry { module clsfr {hook \"\"} } {\n\
$self instvar classifier_ mod_assoc_ hook_assoc_\n\
if [info exists classifier_] {\n\
if [info exists mod_assoc_($classifier_)] {\n\
$self unregister-module $mod_assoc_($classifier_)\n\
unset mod_assoc_($classifier_)\n\
}\n\
if [info exists hook_assoc_($classifier_)] {\n\
if { $hook == \"target\" } {\n\
$clsfr target $hook_assoc($classifier_)\n\
} elseif { $hook != \"\" } {\n\
$clsfr install $hook $hook_assoc_($classifier_)\n\
}\n\
set hook_assoc_($clsfr) $hook_assoc_($classifier_)\n\
unset hook_assoc_($classifier_)\n\
}\n\
}\n\
set mod_assoc_($clsfr) $module\n\
set classifier_ $clsfr\n\
}\n\
\n\
\n\
Node instproc route-notify { module } {\n\
$self instvar rtnotif_\n\
if {$rtnotif_ == \"\"} {\n\
set rtnotif_ $module\n\
} else {\n\
$rtnotif_ route-notify $module\n\
}\n\
\n\
$module cmd route-notify $self\n\
}\n\
\n\
Node instproc unreg-route-notify { module } {\n\
$self instvar rtnotif_\n\
if {$rtnotif_ != \"\"} {\n\
if {$rtnotif_ == $module} {\n\
set rtnotif_ [$rtnotif_ set next_rtm_]\n\
} else {\n\
$rtnotif_ unreg-route-notify $module\n\
}\n\
}\n\
\n\
$module cmd unreg-route-notify $self\n\
}\n\
\n\
Node instproc add-route { dst target } {\n\
$self instvar rtnotif_\n\
\n\
if {$rtnotif_ != \"\"} {\n\
$rtnotif_ add-route $dst $target\n\
}\n\
$self incr-rtgtable-size\n\
}\n\
\n\
Node instproc delete-route args {\n\
$self instvar rtnotif_\n\
if {$rtnotif_ != \"\"} {\n\
eval $rtnotif_ delete-route $args\n\
}\n\
$self decr-rtgtable-size\n\
}\n\
\n\
Node instproc init-routing rtObject {\n\
$self instvar multiPath_ routes_ rtObject_\n\
set nn [$class set nn_]\n\
for {set i 0} {$i < $nn} {incr i} {\n\
set routes_($i) 0\n\
}\n\
if ![info exists rtObject_] {\n\
$self set rtObject_ $rtObject\n\
}\n\
$self set rtObject_\n\
}\n\
\n\
Node instproc rtObject? {} {\n\
$self instvar rtObject_\n\
if ![info exists rtObject_] {\n\
return \"\"\n\
} else {\n\
return $rtObject_\n\
}\n\
}\n\
\n\
Node instproc intf-changed {} {\n\
$self instvar rtObject_\n\
if [info exists rtObject_] {	;# i.e. detailed dynamic routing\n\
$rtObject_ intf-changed\n\
}\n\
}\n\
\n\
\n\
\n\
\n\
Node instproc add-routes {id ifs} {\n\
$self instvar classifier_ multiPath_ routes_ mpathClsfr_\n\
if !$multiPath_ {\n\
if {[llength $ifs] > 1} {\n\
warn \"$class::$proc cannot install multiple routes\"\n\
set ifs [lindex $ifs 0]\n\
}\n\
$self add-route $id [$ifs head]\n\
set routes_($id) 1\n\
return\n\
}\n\
if {$routes_($id) <= 0 && [llength $ifs] == 1 && \\\n\
![info exists mpathClsfr_($id)]} {\n\
$self add-route $id [$ifs head]\n\
set routes_($id) 1\n\
} else {\n\
if ![info exists mpathClsfr_($id)] {\n\
set mpathClsfr_($id) [new Classifier/MultiPath]\n\
if {$routes_($id) > 0} {\n\
assert \"$routes_($id) == 1\"\n\
$mpathClsfr_($id) installNext \\\n\
[$classifier_ in-slot? $id]\n\
}\n\
$classifier_ install $id $mpathClsfr_($id)\n\
}\n\
foreach L $ifs {\n\
$mpathClsfr_($id) installNext [$L head]\n\
incr routes_($id)\n\
}\n\
}\n\
}\n\
\n\
Node instproc delete-routes {id ifs nullagent} {\n\
$self instvar mpathClsfr_ routes_\n\
if [info exists mpathClsfr_($id)] {\n\
foreach L $ifs {\n\
set nonLink([$L head]) 1\n\
}\n\
foreach {slot link} [$mpathClsfr_($id) adjacents] {\n\
if [info exists nonLink($link)] {\n\
$mpathClsfr_($id) clear $slot\n\
incr routes_($id) -1\n\
}\n\
}\n\
} else {\n\
$self delete-route $id $nullagent\n\
incr routes_($id) -1\n\
}\n\
}\n\
\n\
Node instproc enable-mcast args {\n\
$self register-module [new RtModule/Mcast]\n\
\n\
}\n\
\n\
\n\
\n\
Node instproc alloc-port { nullagent } {\n\
return [[$self set dmux_] alloc-port $nullagent]\n\
}\n\
\n\
Node instproc agent port {\n\
$self instvar agents_\n\
foreach a $agents_ {\n\
if { [$a set agent_port_] == $port } {\n\
return $a\n\
}\n\
}\n\
return \"\"\n\
}\n\
\n\
Node instproc demux {} {\n\
return [$self set dmux_]\n\
}\n\
\n\
Node instproc install-demux { demux {port \"\"} } {\n\
$self instvar dmux_ address_\n\
if { $dmux_ != \"\" } {\n\
$self delete-route $dmux_\n\
if { $port != \"\" } {\n\
$demux install $port $dmux_\n\
}\n\
}\n\
set dmux_ $demux\n\
$self add-route $address_ $dmux_\n\
}\n\
\n\
Node instproc port-notify { module } {\n\
$self instvar ptnotif_\n\
lappend ptnotif_ $module\n\
}\n\
\n\
Node instproc unreg-port-notify { module } {\n\
$self instvar ptnotif_\n\
set pos [lsearch $ptnotif_ $module]\n\
if { $pos >= 0 } {\n\
set ptnotif_ [lreplace $ptnotif_ $pos $pos]\n\
}\n\
}\n\
\n\
Node instproc attach { agent { port \"\" } } {\n\
$self instvar agents_ address_ dmux_ \n\
lappend agents_ $agent\n\
$agent set node_ $self\n\
$agent set agent_addr_ [AddrParams addr2id $address_]\n\
if { $dmux_ == \"\" } {\n\
set dmux_ [new Classifier/Port]\n\
$self add-route $address_ $dmux_\n\
}\n\
if { $port == \"\" } {\n\
set port [$dmux_ alloc-port [[Simulator instance] nullagent]]\n\
}\n\
$agent set agent_port_ $port\n\
\n\
$self add-target $agent $port\n\
}\n\
\n\
Node instproc add-target { agent port } {\n\
$self instvar ptnotif_\n\
foreach m [$self set ptnotif_] {\n\
$m attach $agent $port\n\
}\n\
}\n\
\n\
Node instproc detach { agent nullagent } {\n\
$self instvar agents_ dmux_\n\
set k [lsearch -exact $agents_ $agent]\n\
if { $k >= 0 } {\n\
set agents_ [lreplace $agents_ $k $k]\n\
}\n\
$agent set node_ \"\"\n\
$agent set agent_addr_ 0\n\
$agent target $nullagent\n\
$dmux_ install [$agent set agent_port_] $nullagent\n\
\n\
foreach m [$self set ptnotif_] {\n\
$m detach $agent $nullagent\n\
}\n\
}\n\
\n\
Node instproc reset {} {\n\
$self instvar agents_\n\
foreach a $agents_ {\n\
$a reset\n\
}\n\
foreach m [$self list-modules] {\n\
$m reset\n\
}\n\
}\n\
\n\
\n\
Node instproc neighbors {} {\n\
$self instvar neighbor_\n\
return [lsort $neighbor_]\n\
}\n\
\n\
Node instproc add-neighbor {p {pushback 0}} {\n\
$self instvar neighbor_\n\
lappend neighbor_ $p\n\
\n\
if { $pushback == 1 } {\n\
$self cmd add-neighbor $p\n\
}\n\
}\n\
\n\
Node instproc is-neighbor { node } {\n\
$self instvar neighbor_\n\
return [expr [lsearch $neighbor_ $node] != -1]\n\
}\n\
\n\
RtModule instproc register { node } {\n\
$self attach-node $node\n\
$node route-notify $self\n\
$node port-notify $self\n\
}\n\
\n\
RtModule instproc init {} {\n\
$self next\n\
$self instvar classifier_ next_rtm_\n\
set next_rtm_ \"\"\n\
set classifier_ \"\"\n\
}\n\
\n\
RtModule instproc unregister {} {\n\
$self instvar classifier_\n\
delete $classifier_\n\
[$self node] unreg-route-notify $self\n\
[$self node] unreg-port-notify $self\n\
}\n\
\n\
RtModule instproc route-notify { module } {\n\
$self instvar next_rtm_\n\
if {$next_rtm_ == \"\"} {\n\
set next_rtm_ $module\n\
} else {\n\
$next_rtm_ route-notify $module\n\
}\n\
}\n\
\n\
RtModule instproc unreg-route-notify { module } {\n\
$self instvar next_rtm_\n\
if {$next_rtm_ != \"\"} {\n\
if {$next_rtm_ == $module} {\n\
set next_rtm_ [$next_rtm_ set next_rtm_]\n\
} else {\n\
$next_rtm_ unreg-route-notify $module\n\
}\n\
}\n\
}\n\
\n\
RtModule instproc add-route { dst target } {\n\
$self instvar next_rtm_\n\
[$self set classifier_] install $dst $target\n\
if {$next_rtm_ != \"\"} {\n\
$next_rtm_ add-route $dst $target\n\
}\n\
}\n\
\n\
RtModule instproc delete-route { dst nullagent} {\n\
$self instvar next_rtm_\n\
[$self set classifier_] install $dst $nullagent\n\
if {$next_rtm_ != \"\"} {\n\
$next_rtm_ delete-route $dst $nullagent\n\
}\n\
}\n\
\n\
RtModule instproc attach { agent port } {\n\
$agent target [[$self node] entry]\n\
[[$self node] demux] install $port $agent\n\
}\n\
\n\
RtModule instproc detach { agent nullagent } {\n\
}\n\
\n\
RtModule instproc reset {} {\n\
}\n\
\n\
\n\
\n\
RtModule/Base instproc register { node } {\n\
$self next $node\n\
\n\
$self instvar classifier_\n\
set classifier_ [new Classifier/Hash/Dest 32]\n\
$classifier_ set mask_ [AddrParams NodeMask 1]\n\
$classifier_ set shift_ [AddrParams NodeShift 1]\n\
\n\
$node install-entry $self $classifier_\n\
}\n\
\n\
\n\
RtModule/Mcast instproc register { node } {\n\
$self next $node\n\
$self instvar classifier_\n\
\n\
$self set classifier_ [$node entry]\n\
\n\
\n\
$node set switch_ [new Classifier/Addr]\n\
\n\
[$node set switch_] set mask_ [AddrParams McastMask]\n\
[$node set switch_] set shift_ [AddrParams McastShift]\n\
\n\
$node set multiclassifier_ [new Classifier/Multicast/Replicator]\n\
[$node set multiclassifier_] set node_ $node\n\
\n\
$node set mrtObject_ [new mrtObject $node]\n\
\n\
$node insert-entry $self [$node set switch_] 0\n\
[$node set switch_] install 1 [$node set multiclassifier_]\n\
}\n\
\n\
RtModule/Hier instproc register { node } {\n\
$self next $node\n\
$self instvar classifier_\n\
set classifier_ [new Classifier/Hier]\n\
$node install-entry $self $classifier_\n\
}\n\
\n\
RtModule/Hier instproc delete-route args {\n\
eval [$self set classifier_] clear $args\n\
}\n\
\n\
Classifier/Hier instproc init {} {\n\
$self next\n\
for {set n 1} {$n <= [AddrParams hlevel]} {incr n} {\n\
set classifier [new Classifier/Addr]\n\
$classifier set mask_ [AddrParams NodeMask $n]\n\
$classifier set shift_ [AddrParams NodeShift $n]\n\
$self cmd add-classifier $n $classifier\n\
}\n\
}\n\
\n\
Classifier/Hier instproc destroy {} {\n\
for {set n 1} {$n <= [AddrParams hlevel]} {incr n} {\n\
delete [$self cmd classifier $n]\n\
}\n\
$self next\n\
}\n\
\n\
Classifier/Hier instproc clear args {\n\
set l [llength $args]\n\
[$self cmd classifier $l] clear [lindex $args [expr $l-1]] \n\
}\n\
\n\
Classifier/Hier instproc install { dst target } {\n\
set al [AddrParams split-addrstr $dst]\n\
set l [llength $al]\n\
for {set i 1} {$i < $l} {incr i} {\n\
set d [lindex $al [expr $i-1]]\n\
[$self cmd classifier $i] install $d \\\n\
[$self cmd classifier [expr $i+1]]\n\
}\n\
[$self cmd classifier $l] install [lindex $al [expr $l-1]] $target\n\
}\n\
\n\
\n\
RtModule/Manual instproc register { node } {\n\
$self next $node\n\
$self instvar classifier_	\n\
set classifier_ [new Classifier/Hash/Dest 2]\n\
$classifier_ set mask_ [AddrParams NodeMask 1]\n\
$classifier_ set shift_ [AddrParams NodeShift 1]\n\
$node install-entry $self $classifier_\n\
}\n\
\n\
RtModule/Manual instproc add-route {dst_address target} {\n\
$self instvar classifier_ \n\
set slot [$classifier_ installNext $target]\n\
if {$dst_address == \"default\"} {\n\
$classifier_ set default_ $slot\n\
} else {\n\
set encoded_dst_address [expr $dst_address << [AddrParams NodeShift 1]]\n\
$classifier_ set-hash auto 0 $encoded_dst_address 0 $slot\n\
}\n\
}\n\
\n\
RtModule/Manual instproc add-route-to-adj-node { args } {\n\
$self instvar classifier_ \n\
\n\
set dst \"\"\n\
if {[lindex $args 0] == \"-default\"} {\n\
set dst default\n\
set args [lrange $args 1 end]\n\
}\n\
if {[llength $args] != 1} {\n\
error \"ManualRtNode::add-route-to-adj-node [-default] node\"\n\
}\n\
set target_node $args\n\
if {$dst == \"\"} {\n\
set dst [$target_node set address_]\n\
}\n\
set ns [Simulator instance]\n\
set link [$ns link [$self node] $target_node]\n\
set target [$link head]\n\
return [$self add-route $dst $target]\n\
}\n\
\n\
RtModule/Source instproc register { node } {\n\
$self next $node\n\
\n\
$self instvar classifier_\n\
$self set classifier_ [$node entry]\n\
\n\
\n\
$node set src_classifier_ [new Classifier/SR]\n\
$node set src_agent_ [new Agent/SRAgent]\n\
$node set switch_ [$node set src_classifier_]\n\
\n\
\n\
\n\
\n\
\n\
$node insert-entry $self [$node set switch_] 1\n\
\n\
[$node set switch_]  install 0 [$node set src_agent_]\n\
$node attach [$node set src_agent_]\n\
\n\
\n\
}\n\
\n\
\n\
RtModule/VC instproc register { node } {\n\
$self instvar classifier_\n\
\n\
$self attach-node $node\n\
$node port-notify $self\n\
\n\
set classifier_ [new Classifier/Virtual]\n\
$classifier_ set node_ $node\n\
$classifier_ set mask_ [AddrParams NodeMask 1]\n\
$classifier_ set shift_ [AddrParams NodeShift 1]\n\
$classifier_ nodeaddr [$node node-addr]\n\
$node install-entry $self $classifier_ \n\
}\n\
\n\
RtModule/VC instproc add-route { dst target } {\n\
}\n\
\n\
Classifier/Virtual instproc find dst {\n\
$self instvar node_\n\
if {[$node_ id] == $dst} {\n\
return [$node_ set dmux_]\n\
} else {\n\
return [[[Simulator instance] link $node_ \\\n\
[[Simulator instance] set Node_($dst)]] head]\n\
}\n\
}\n\
\n\
Classifier/Virtual instproc install {dst target} {\n\
}\n\
\n\
Class RtModule/Nix -superclass RtModule\n\
\n\
RtModule/Nix instproc register { node } {\n\
$self next $node\n\
$self instvar classifier_\n\
set classifier_ [new Classifier/Nix]\n\
$classifier_ set-node-id [$node set id_]\n\
$node install-entry $self $classifier_\n\
}\n\
\n\
RtModule/Nix instproc route-notify { module } { }\n\
\n\
\n\
\n\
\n\
\n\
Class HierNode -superclass Node\n\
\n\
HierNode instproc init {args} {\n\
$self instvar address_\n\
set haddress $args\n\
set address_ $args\n\
set args [lreplace $args 0 1]\n\
$self next $args\n\
set address_ $haddress\n\
}\n\
\n\
HierNode instproc mk-default-classifier {} {\n\
$self instvar classifiers_ \n\
$self set classifier_ [new Classifier/Hier]\n\
}\n\
\n\
\n\
\n\
\n\
ARPTable instproc init args {\n\
eval $self next $args		;# parent class constructor\n\
}\n\
\n\
ARPTable set bandwidth_         0\n\
ARPTable set delay_             5us\n\
\n\
\n\
Node/MobileNode instproc init args {\n\
\n\
eval $self next $args\n\
\n\
$self instvar nifs_ arptable_ X_ Y_ Z_ nodetype_\n\
set X_ 0.0\n\
set Y_ 0.0\n\
set Z_ 0.0\n\
set arptable_ \"\"                ;# no ARP table yet\n\
set nifs_	0		;# number of network interfaces\n\
$self makemip-New$nodetype_\n\
}\n\
\n\
\n\
\n\
Node/MobileNode instproc makemip-NewMobile {} {\n\
}\n\
\n\
Node/MobileNode instproc makemip-NewBase {} {\n\
}\n\
\n\
Node/MobileNode instproc makemip-New {} {\n\
}\n\
\n\
Node/MobileNode instproc makemip-NewMIPBS {} {\n\
$self instvar regagent_ encap_ decap_ agents_ id_\n\
\n\
set dmux [new Classifier/Port/Reserve]\n\
$dmux set mask_ 0x7fffffff\n\
$dmux set shift_ 0\n\
$self install-demux $dmux\n\
\n\
set regagent_ [new Agent/MIPBS $self]\n\
$self attach $regagent_ [Node/MobileNode set REGAGENT_PORT]\n\
$self attach-encap \n\
$self attach-decap\n\
}\n\
\n\
Node/MobileNode instproc attach-encap {} {\n\
$self instvar encap_ \n\
\n\
set encap_ [new MIPEncapsulator]\n\
\n\
$encap_ set mask_ [AddrParams NodeMask 1]\n\
$encap_ set shift_ [AddrParams NodeShift 1]\n\
set nodeaddr [AddrParams addr2id [$self node-addr]]\n\
$encap_ set addr_ [expr ( ~([AddrParams NodeMask 1] << \\\n\
[AddrParams NodeShift 1]) & $nodeaddr )]\n\
$encap_ set port_ 1\n\
$encap_ target [$self entry]\n\
$encap_ set node_ $self\n\
}\n\
\n\
Node/MobileNode instproc attach-decap {} {\n\
$self instvar decap_ dmux_ agents_\n\
set decap_ [new Classifier/Addr/MIPDecapsulator]\n\
lappend agents_ $decap_\n\
$decap_ set mask_ [AddrParams NodeMask 1]\n\
$decap_ set shift_ [AddrParams NodeShift 1]\n\
$dmux_ install [Node/MobileNode set DECAP_PORT] $decap_\n\
}\n\
\n\
Node/MobileNode instproc makemip-NewMIPMH {} {\n\
$self instvar regagent_\n\
\n\
set dmux [new Classifier/Port/Reserve]\n\
$dmux set mask_ 0x7fffffff\n\
$dmux set shift_ 0\n\
$self install-demux $dmux\n\
\n\
set regagent_ [new Agent/MIPMH $self]\n\
$self attach $regagent_ [Node/MobileNode set REGAGENT_PORT]\n\
$regagent_ set mask_ [AddrParams NodeMask 1]\n\
$regagent_ set shift_ [AddrParams NodeShift 1]\n\
$regagent_ set dst_addr_ [expr (~0) << [AddrParams NodeShift 1]]\n\
$regagent_ set dst_port_ 0\n\
$regagent_ node $self\n\
}\n\
\n\
\n\
Node/MobileNode instproc reset {} {\n\
$self instvar arptable_ nifs_ netif_ mac_ ifq_ ll_ imep_\n\
for {set i 0} {$i < $nifs_} {incr i} {\n\
$netif_($i) reset\n\
$mac_($i) reset\n\
$ll_($i) reset\n\
$ifq_($i) reset\n\
if { [info exists opt(imep)] && $opt(imep) == \"ON\" } { \n\
$imep_($i) reset \n\
}\n\
}\n\
if { $arptable_ != \"\" } {\n\
$arptable_ reset \n\
}\n\
}\n\
\n\
Node/MobileNode instproc add-target { agent port } {\n\
$self instvar dmux_ imep_ toraDebug_ \n\
\n\
set ns [Simulator instance]\n\
set newapi [$ns imep-support]\n\
\n\
$agent set sport_ $port\n\
\n\
set toraonly [string first \"TORA\" [$agent info class]] \n\
if {$toraonly != -1 } {\n\
$agent if-queue [$self set ifq_(0)]  ;# ifq between LL and MAC\n\
$agent imep-agent [$self set imep_(0)]\n\
[$self set imep_(0)] rtagent $agent\n\
}\n\
\n\
set aodvonly [string first \"AODV\" [$agent info class]] \n\
if {$aodvonly != -1 } {\n\
$agent if-queue [$self set ifq_(0)]   ;# ifq between LL and MAC\n\
}\n\
\n\
if { $port == [Node set rtagent_port_] } {			\n\
$self add-target-rtagent $agent $port\n\
return\n\
}\n\
\n\
set namfp [$ns get-nam-traceall]\n\
if { [Simulator set AgentTrace_] == \"ON\" } {\n\
if {$newapi != \"\"} {\n\
set sndT [$self mobility-trace Send \"AGT\"]\n\
} else {\n\
set sndT [cmu-trace Send AGT $self]\n\
}\n\
if { $namfp != \"\" } {\n\
$sndT namattach $namfp\n\
}\n\
$sndT target [$self entry]\n\
$agent target $sndT\n\
if {$newapi != \"\"} {\n\
set rcvT [$self mobility-trace Recv \"AGT\"]\n\
} else {\n\
set rcvT [cmu-trace Recv AGT $self]\n\
}\n\
if { $namfp != \"\" } {\n\
$rcvT namattach $namfp\n\
}\n\
$rcvT target $agent\n\
$dmux_ install $port $rcvT\n\
} else {\n\
$agent target [$self entry]\n\
$dmux_ install $port $agent\n\
}\n\
}\n\
\n\
Node/MobileNode instproc add-target-rtagent { agent port } {\n\
$self instvar imep_ toraDebug_ \n\
\n\
set ns [Simulator instance]\n\
set newapi [$ns imep-support]\n\
set namfp [$ns get-nam-traceall]\n\
\n\
set dmux_ [$self demux]\n\
set classifier_ [$self entry]\n\
\n\
if { [Simulator set RouterTrace_] == \"ON\" } {\n\
if {$newapi != \"\"} {\n\
set sndT [$self mobility-trace Send \"RTR\"]\n\
} else {\n\
set sndT [cmu-trace Send \"RTR\" $self]\n\
}\n\
if { $namfp != \"\" } {\n\
$sndT namattach $namfp\n\
}\n\
if { $newapi == \"ON\" } {\n\
$agent target $imep_(0)\n\
$imep_(0) sendtarget $sndT\n\
if { [info exists toraDebug_] && $toraDebug_ == \"ON\"} {\n\
set sndT2 [$self mobility-trace Send \"TRP\"]\n\
$sndT2 target $imep_(0)\n\
$agent target $sndT2\n\
}\n\
} else {  ;#  no IMEP\n\
$agent target $sndT\n\
}\n\
$sndT target [$self set ll_(0)]\n\
if {$newapi != \"\"} {\n\
set rcvT [$self mobility-trace Recv \"RTR\"]\n\
} else {\n\
set rcvT [cmu-trace Recv \"RTR\" $self]\n\
}\n\
if { $namfp != \"\" } {\n\
$rcvT namattach $namfp\n\
}\n\
if {$newapi == \"ON\" } {\n\
[$self set ll_(0)] up-target $imep_(0)\n\
$classifier_ defaulttarget $agent\n\
if {[info exists toraDebug_] && $toraDebug_ == \"ON\" } {\n\
set rcvT2 [$self mobility-trace Recv \"TRP\"]\n\
$rcvT2 target $agent\n\
$classifier_ defaulttarget $rcvT2\n\
}\n\
} else {\n\
$rcvT target $agent\n\
$classifier_ defaulttarget $rcvT\n\
$dmux_ install $port $rcvT\n\
}\n\
} else {\n\
if { $newapi == \"ON\" } {\n\
$agent target $imep_(0)\n\
if { [info exists toraDebug_] && $toraDebug_ == \"ON\"} {\n\
set sndT2 [$self mobility-trace Send \"TRP\"]\n\
$sndT2 target $imep_(0)\n\
$agent target $sndT2\n\
}\n\
$imep_(0) sendtarget [$self set ll_(0)]\n\
\n\
} else {  ;#  no IMEP\n\
$agent target [$self set ll_(0)]\n\
}    \n\
if {$newapi == \"ON\" } {\n\
[$self set ll_(0)] up-target $imep_(0)\n\
$classifier_ defaulttarget $agent\n\
if {[info exists toraDebug_] && $toraDebug_ == \"ON\" } {\n\
set rcvT2 [$self mobility-trace Recv \"TRP\"]\n\
$rcvT2 target $agent\n\
[$self set classifier_] defaulttarget $rcvT2\n\
}\n\
} else {\n\
$classifier_ defaulttarget $agent\n\
$dmux_ install $port $agent\n\
}\n\
}\n\
}\n\
\n\
Node/MobileNode instproc add-interface { channel pmodel lltype mactype \\\n\
qtype qlen iftype anttype errproc fecproc} {\n\
$self instvar arptable_ nifs_ netif_ mac_ ifq_ ll_ imep_ err_ fec_\n\
\n\
set ns [Simulator instance]\n\
set imepflag [$ns imep-support]\n\
set t $nifs_\n\
incr nifs_\n\
\n\
set netif_($t)	[new $iftype]		;# interface\n\
set mac_($t)	[new $mactype]		;# mac layer\n\
set ifq_($t)	[new $qtype]		;# interface queue\n\
set ll_($t)	[new $lltype]		;# link layer\n\
set ant_($t)    [new $anttype]\n\
\n\
set err_($t) \"\"\n\
if {$errproc != \"\"} {\n\
set err_($t) [$errproc]\n\
}\n\
set fec_($t) \"\"\n\
if {$fecproc != \"\"} {\n\
set fec_($t) [$fecproc]\n\
}\n\
\n\
set namfp [$ns get-nam-traceall]\n\
if {$imepflag == \"ON\" } {              \n\
set imep_($t) [new Agent/IMEP [$self id]]\n\
set imep $imep_($t)\n\
set drpT [$self mobility-trace Drop \"RTR\"]\n\
if { $namfp != \"\" } {\n\
$drpT namattach $namfp\n\
}\n\
$imep drop-target $drpT\n\
$ns at 0.[$self id] \"$imep_($t) start\"   ;# start beacon timer\n\
}\n\
set nullAgent_ [$ns set nullAgent_]\n\
set netif $netif_($t)\n\
set mac $mac_($t)\n\
set ifq $ifq_($t)\n\
set ll $ll_($t)\n\
\n\
set err $err_($t)\n\
set fec $fec_($t)\n\
if { $arptable_ == \"\" } {\n\
set arptable_ [new ARPTable $self $mac]\n\
if {$imepflag != \"\"} {\n\
set drpT [$self mobility-trace Drop \"IFQ\"]\n\
} else {\n\
set drpT [cmu-trace Drop \"IFQ\" $self]\n\
}\n\
$arptable_ drop-target $drpT\n\
if { $namfp != \"\" } {\n\
$drpT namattach $namfp\n\
}\n\
}\n\
$ll arptable $arptable_\n\
$ll mac $mac\n\
$ll down-target $ifq\n\
\n\
if {$imepflag == \"ON\" } {\n\
$imep recvtarget [$self entry]\n\
$imep sendtarget $ll\n\
$ll up-target $imep\n\
} else {\n\
$ll up-target [$self entry]\n\
}\n\
$ifq target $mac\n\
$ifq set limit_ $qlen\n\
if {$imepflag != \"\"} {\n\
set drpT [$self mobility-trace Drop \"IFQ\"]\n\
} else {\n\
set drpT [cmu-trace Drop \"IFQ\" $self]\n\
}\n\
$ifq drop-target $drpT\n\
if { $namfp != \"\" } {\n\
$drpT namattach $namfp\n\
}\n\
$mac netif $netif\n\
$mac up-target $ll\n\
\n\
if {$err == \"\" && $fec == \"\"} {\n\
$mac down-target $netif\n\
} elseif {$err != \"\" && $fec == \"\"} {\n\
$mac down-target $err\n\
$err target $netif\n\
} elseif {$err == \"\" && $fec != \"\"} {\n\
$mac down-target $fec\n\
$fec down-target $netif\n\
} else {\n\
$mac down-target $fec\n\
$fec down-target $err\n\
$err target $netif\n\
}\n\
\n\
set god_ [God instance]\n\
if {$mactype == \"Mac/802_11\"} {\n\
$mac nodes [$god_ num_nodes]\n\
}\n\
$netif channel $channel\n\
if {$fec == \"\"} {\n\
$netif up-target $mac\n\
} else {\n\
$netif up-target $fec\n\
$fec up-target $mac\n\
}\n\
\n\
$netif propagation $pmodel	;# Propagation Model\n\
$netif node $self		;# Bind node <---> interface\n\
$netif antenna $ant_($t)\n\
$channel addif $netif\n\
\n\
\n\
if { [Simulator set MacTrace_] == \"ON\" } {\n\
if {$imepflag != \"\"} {\n\
set rcvT [$self mobility-trace Recv \"MAC\"]\n\
} else {\n\
set rcvT [cmu-trace Recv \"MAC\" $self]\n\
}\n\
$mac log-target $rcvT\n\
if { $namfp != \"\" } {\n\
$rcvT namattach $namfp\n\
}\n\
if {$imepflag != \"\"} {\n\
set sndT [$self mobility-trace Send \"MAC\"]\n\
} else {\n\
set sndT [cmu-trace Send \"MAC\" $self]\n\
}\n\
$sndT target [$mac down-target]\n\
$mac down-target $sndT\n\
if { $namfp != \"\" } {\n\
$sndT namattach $namfp\n\
}\n\
if {$imepflag != \"\"} {\n\
set rcvT [$self mobility-trace Recv \"MAC\"]\n\
} else {\n\
set rcvT [cmu-trace Recv \"MAC\" $self]\n\
}\n\
$rcvT target [$mac up-target]\n\
$mac up-target $rcvT\n\
if { $namfp != \"\" } {\n\
$rcvT namattach $namfp\n\
}\n\
if {$imepflag != \"\"} {\n\
set drpT [$self mobility-trace Drop \"MAC\"]\n\
} else {\n\
set drpT [cmu-trace Drop \"MAC\" $self]`\n\
}\n\
$mac drop-target $drpT\n\
if { $namfp != \"\" } {\n\
$drpT namattach $namfp\n\
}\n\
} else {\n\
$mac log-target [$ns set nullAgent_]\n\
$mac drop-target [$ns set nullAgent_]\n\
}\n\
\n\
\n\
$self addif $netif\n\
}\n\
\n\
Node/MobileNode instproc setPt { val } {\n\
$self instvar netif_\n\
$netif_(0) setTxPower $val\n\
}\n\
\n\
Node/MobileNode instproc setPr { val } {\n\
$self instvar netif_\n\
$netif_(0) setRxPower $val\n\
}\n\
\n\
Node/MobileNode instproc setPidle { val } {\n\
$self instvar netif_\n\
$netif_(0) setIdlePower $val\n\
}\n\
\n\
Node/MobileNode instproc mobility-trace { ttype atype } {\n\
set ns [Simulator instance]\n\
set tracefd [$ns get-ns-traceall]\n\
if { $tracefd == \"\" } {\n\
puts \"Warning: You have not defined you tracefile yet!\"\n\
puts \"Please use trace-all command to define it.\"\n\
return \"\"\n\
}\n\
set T [new CMUTrace/$ttype $atype]\n\
$T newtrace [Simulator set WirelessNewTrace_]\n\
$T target [$ns nullagent]\n\
$T attach $tracefd\n\
$T set src_ [$self id]\n\
$T node $self\n\
return $T\n\
}\n\
\n\
Node/MobileNode instproc nodetrace { tracefd } {\n\
set T [new Trace/Generic]\n\
$T target [[Simulator instance] set nullAgent_]\n\
$T attach $tracefd\n\
$T set src_ [$self id]\n\
$self log-target $T    \n\
}\n\
\n\
Node/MobileNode instproc agenttrace {tracefd} {\n\
set ns [Simulator instance]\n\
set ragent [$self set ragent_]\n\
set drpT [$self mobility-trace Drop \"RTR\"]\n\
set namfp [$ns get-nam-traceall]\n\
if { $namfp != \"\"} {\n\
$drpT namattach $namfp\n\
}\n\
$ragent drop-target $drpT\n\
set T [new Trace/Generic]\n\
$T target [$ns set nullAgent_]\n\
$T attach $tracefd\n\
$T set src_ [$self id]\n\
$ragent tracetarget $T\n\
set imepflag [$ns imep-support]\n\
if {$imepflag == \"ON\"} {\n\
[$self set imep_(0)] log-target $T\n\
}\n\
}\n\
\n\
Node/MobileNode instproc mip-call {ragent} {\n\
$self instvar regagent_\n\
if [info exists regagent_] {\n\
$regagent_ ragent $ragent\n\
}\n\
}\n\
\n\
Node/MobileNode instproc attach-gafpartner {} {\n\
\n\
$self instvar gafpartner_ address_ ll_ \n\
\n\
set gafpartner_ [new GAFPartner]\n\
\n\
$gafpartner_ set mask_ [AddrParams NodeMask 1]\n\
$gafpartner_ set shift_ [AddrParams NodeShift 1]\n\
set nodeaddr [AddrParams addr2id [$self node-addr]]\n\
\n\
\n\
\n\
$gafpartner_ set addr_ $nodeaddr\n\
$gafpartner_ set port_ 254\n\
\n\
\n\
$gafpartner_ target [$self entry]\n\
$ll_(0) up-target $gafpartner_\n\
}\n\
\n\
Node/MobileNode instproc unset-gafpartner {} {\n\
$self instvar gafpartner_\n\
\n\
$gafpartner_ set-gafagent 0\n\
\n\
}\n\
\n\
\n\
Class SRNodeNew -superclass Node/MobileNode\n\
\n\
SRNodeNew instproc init args {\n\
$self instvar dsr_agent_ dmux_ entry_point_ address_\n\
\n\
set ns [Simulator instance]\n\
\n\
eval $self next $args	;# parent class constructor\n\
\n\
if {$dmux_ == \"\" } {\n\
set dmux_ [new Classifier/Port]\n\
}\n\
set dsr_agent_ [new Agent/DSRAgent]\n\
\n\
$dsr_agent_ addr $address_\n\
$dsr_agent_ node $self\n\
if [Simulator set mobile_ip_] {\n\
$dsr_agent_ port-dmux [$self set dmux_]\n\
}\n\
$self addr $address_\n\
\n\
if { [Simulator set RouterTrace_] == \"ON\" } {\n\
set rcvT [$self mobility-trace Recv \"RTR\"]\n\
set namfp [$ns get-nam-traceall]\n\
if {  $namfp != \"\" } {\n\
$rcvT namattach $namfp\n\
}\n\
$rcvT target $dsr_agent_\n\
set entry_point_ $rcvT	\n\
} else {\n\
set entry_point_ $dsr_agent_\n\
}\n\
\n\
$self set ragent_ $dsr_agent_\n\
$dsr_agent_ target $dmux_\n\
\n\
set nullAgent_ [$ns set nullAgent_]\n\
$dmux_ install [Node set rtagent_port_] $nullAgent_\n\
\n\
$self instvar classifier_\n\
set classifier_ \"srnode made illegal use of classifier_\"\n\
\n\
return $self\n\
}\n\
\n\
SRNodeNew instproc start-dsr {} {\n\
$self instvar dsr_agent_\n\
$dsr_agent_ startdsr\n\
}\n\
\n\
SRNodeNew instproc entry {} {\n\
$self instvar entry_point_\n\
return $entry_point_\n\
}\n\
\n\
SRNodeNew instproc add-interface args {\n\
eval $self next $args\n\
\n\
$self instvar dsr_agent_ ll_ mac_ ifq_\n\
\n\
set ns [Simulator instance]\n\
$dsr_agent_ mac-addr [$mac_(0) id]\n\
\n\
if { [Simulator set RouterTrace_] == \"ON\" } {\n\
set sndT [$self mobility-trace Send \"RTR\"]\n\
set namfp [$ns get-nam-traceall]\n\
if {$namfp != \"\" } {\n\
$sndT namattach $namfp\n\
}\n\
$sndT target $ll_(0)\n\
$dsr_agent_ add-ll $sndT $ifq_(0)\n\
} else {\n\
$dsr_agent_ add-ll $ll_(0) $ifq_(0)\n\
}\n\
$dsr_agent_ install-tap $mac_(0)\n\
}\n\
\n\
SRNodeNew instproc reset args {\n\
$self instvar dsr_agent_\n\
eval $self next $args\n\
$dsr_agent_ reset\n\
}\n\
\n\
\n\
Trace set namBTTrace_ 0\n\
BTHost set master_ 0\n\
BTHost set bd_addr_ 0\n\
BTHost set inqTimeout_ 0\n\
BTHost set numResponses_ 0\n\
Baseband set namTrace_ 0\n\
Baseband set bd_addr_ 0\n\
Baseband set giac_ 0\n\
Baseband set dac_ 0\n\
Baseband set masterIndex_ 0\n\
Baseband set xpos_ 0\n\
Baseband set ypos_ 0\n\
Baseband set seed_ 0\n\
Baseband set isOffset_ 0\n\
BT_DRR set cycleTime_ 0\n\
LinkController set tx_thresh_ 0\n\
LinkController set am_addr_ 0\n\
LMP set master_ 0\n\
LMP set am_addr_ 0\n\
LBF set conn_handle_ 0\n\
Baseband set debug_ 0\n\
BTHost set debug_ 0\n\
BT_DRR set debug_ 0\n\
LinkController set debug_ 0\n\
L2CAP set debug_ 0\n\
LMP set debug_ 0\n\
LBF set debug_ 0\n\
Baseband set debug_ 0\n\
Classifier/BTAddr set debug_ 0\n\
\n\
\n\
Class BTNode -superclass Node\n\
\n\
BTNode instproc init {ip_addr bd_addr master X_ Y_} {\n\
eval $self next $ip_addr\n\
$self instvar bthost_ l2cap_ tb_ lc_ sched_ lm_ classifier_\n\
\n\
set bthost_ [new BTHost]\n\
set l2cap_ [new L2CAP]\n\
set lm_ [new Baseband]\n\
\n\
$bthost_ set master_ $master\n\
$bthost_ set bd_addr_ $bd_addr\n\
$bthost_ target [$self entry]\n\
$bthost_ setL2CAP $l2cap_\n\
$bthost_ setLM $lm_\n\
$l2cap_ up-target $bthost_\n\
$lm_ set bd_addr_ $bd_addr\n\
$lm_ set giac_ 0x9e8b33\n\
$lm_ set dac_ $bd_addr\n\
$lm_ set masterIndex_ $master\n\
$lm_ set xpos_ $X_\n\
$lm_ set ypos_ $Y_\n\
$lm_ bthost $bthost_\n\
\n\
$classifier_ defaulttarget $bthost_	\n\
\n\
if {$master == 1} {\n\
set sched_ [new BT_DRR]\n\
$sched_ set cycleTime_ 0.1\n\
set down_dmux_  [new Classifier/BTAddr]\n\
$lm_ up-target $sched_\n\
\n\
for {set i 0} {$i < 7} {incr i} {\n\
set lc_($i) [new LinkController]\n\
set lq_($i) [new LMP]\n\
set tb_($i) [new LBF]\n\
\n\
$sched_ install $i $lc_($i)\n\
\n\
$lc_($i) set tx_thresh_ 10\n\
$lc_($i) set am_addr_ [expr $i+1]\n\
$lc_($i) down-target $sched_\n\
$lc_($i) up-target $lq_($i)\n\
\n\
$lq_($i) set master_ 1\n\
$lq_($i) down-target $lc_($i)\n\
$lq_($i) up-target $l2cap_\n\
$lq_($i) sched $sched_\n\
$lq_($i) bthost $bthost_\n\
$lq_($i) tbf $tb_($i)\n\
$lq_($i) set am_addr_ [expr $i+1]\n\
\n\
$tb_($i) set conn_handle_ $i\n\
$tb_($i) down-target $lq_($i)\n\
$tb_($i) up-target $l2cap_\n\
\n\
$l2cap_ down-target $down_dmux_\n\
$bthost_ linkq $i $lq_($i)\n\
\n\
$down_dmux_ install $i $tb_($i)\n\
\n\
$lm_ install-linkq $i $lq_($i)\n\
}\n\
} else {\n\
set lc_ [new LinkController]\n\
set lq_ [new LMP]\n\
$lq_ set master_ 0\n\
\n\
$lc_ set tx_thresh_ 10\n\
$lc_ down-target $lm_\n\
$lc_ up-target $lq_\n\
\n\
$lq_ down-target $lc_\n\
$lq_ up-target $l2cap_\n\
$lq_ bthost $bthost_\n\
$l2cap_ down-target $lq_\n\
$bthost_ linkq 0 $lq_\n\
\n\
$lm_ up-target $lc_\n\
$lm_ install-linkq 0 $lq_\n\
}	\n\
}\n\
\n\
BTNode instproc set-target {target} {\n\
set lm [$self set lm_]\n\
$lm down-target $target\n\
}\n\
\n\
BTNode instproc set-droptarget {target} {\n\
$self instvar lm_\n\
$lm_ drop-target $target\n\
}\n\
\n\
BTNode instproc attach-app {slot app} {\n\
$self instvar bthost_\n\
$bthost_ connect-app $slot $app\n\
}\n\
\n\
BTNode instproc setISoffset {offset} {\n\
$self instvar lm_\n\
$lm_ set isOffset_ $offset\n\
}\n\
\n\
\n\
\n\
Class Node/MobileNode/BaseStationNode -superclass Node/MobileNode\n\
\n\
Node/MobileNode/BaseStationNode instproc init args {\n\
$self next $args\n\
}\n\
\n\
Node/MobileNode/BaseStationNode instproc mk-default-classifier {} {\n\
$self instvar classifiers_ \n\
set levels [AddrParams hlevel]\n\
for {set n 1} {$n <= $levels} {incr n} {\n\
set classifiers_($n) [new Classifier/Hash/Dest/Bcast 32]\n\
$classifiers_($n) set mask_ [AddrParams NodeMask $n]\n\
$classifiers_($n) set shift_ [AddrParams NodeShift $n]\n\
}\n\
}\n\
\n\
\n\
Node/MobileNode/BaseStationNode instproc entry {} {\n\
$self instvar ns_\n\
if ![info exist ns_] {\n\
set ns_ [Simulator instance]\n\
}\n\
if [$ns_ multicast?] { \n\
$self instvar switch_\n\
return $switch_\n\
}\n\
$self instvar classifiers_\n\
return $classifiers_(1)\n\
}\n\
\n\
Node/MobileNode/BaseStationNode instproc add-target {agent port } {\n\
$self instvar dmux_ classifiers_\n\
$agent set sport_ $port\n\
set level [AddrParams hlevel]\n\
\n\
if { $port == [Node set rtagent_port_] } {	\n\
if { [Simulator set RouterTrace_] == \"ON\" } {\n\
set sndT [cmu-trace Send \"RTR\" $self]\n\
$sndT target [$self set ll_(0)]\n\
$agent target $sndT\n\
set rcvT [cmu-trace Recv \"RTR\" $self]\n\
$rcvT target $agent\n\
for {set i 1} {$i <= $level} {incr i} {\n\
$classifiers_($i) defaulttarget $rcvT\n\
$classifiers_($i) bcast-receiver $rcvT\n\
}\n\
$dmux_ install $port $rcvT\n\
} else {\n\
$agent target [$self set ll_(0)]\n\
for {set i 1} {$i <= $level} {incr i} {\n\
$classifiers_($i) bcast-receiver $agent\n\
$classifiers_($i) defaulttarget $agent\n\
}\n\
$dmux_ install $port $agent\n\
}\n\
} else {\n\
if { [Simulator set AgentTrace_] == \"ON\" } {\n\
set sndT [cmu-trace Send AGT $self]\n\
$sndT target [$self entry]\n\
$agent target $sndT\n\
set rcvT [cmu-trace Recv AGT $self]\n\
$rcvT target $agent\n\
$dmux_ install $port $rcvT\n\
} else {\n\
$agent target [$self entry]\n\
$dmux_ install $port $agent\n\
}\n\
}\n\
}\n\
\n\
Class Link\n\
\n\
Link set nl_ 0\n\
\n\
Link instproc init { src dst } {\n\
$self next\n\
\n\
$self instvar id_\n\
set id_ [Link set nl_]\n\
Link set nl_ [expr $id_ + 1]\n\
\n\
\n\
$self instvar trace_ fromNode_ toNode_ color_ oldColor_\n\
set fromNode_ $src\n\
set toNode_   $dst\n\
set color_ \"black\"\n\
set oldColor_ \"black\"\n\
\n\
set trace_ \"\"\n\
}\n\
\n\
Link instproc head {} {\n\
$self instvar head_\n\
return $head_\n\
}\n\
\n\
Link instproc add-to-head { connector } {\n\
$self instvar head_\n\
$connector target [$head_ target]\n\
$head_ target $connector\n\
}\n\
\n\
Link instproc queue {} {\n\
$self instvar queue_\n\
return $queue_\n\
}\n\
\n\
Link instproc link {} {\n\
$self instvar link_\n\
return $link_\n\
}\n\
\n\
Link instproc src {}	{ $self set fromNode_	}\n\
Link instproc dst {}	{ $self set toNode_	}\n\
Link instproc cost c	{ $self set cost_ $c	}\n\
\n\
Link instproc cost? {} {\n\
$self instvar cost_\n\
if ![info exists cost_] {\n\
set cost_ 1\n\
}\n\
set cost_\n\
}\n\
\n\
Link instproc id {} 	{ $self set id_ }\n\
Link instproc setid { x } { $self set id_ $x }\n\
Link instproc bw {} { $self set bandwidth_ }\n\
\n\
Link instproc if-label? {} {\n\
$self instvar iif_\n\
$iif_ label\n\
}\n\
\n\
Link instproc up { } {\n\
$self instvar dynamics_ dynT_\n\
if ![info exists dynamics_] return\n\
$dynamics_ set status_ 1\n\
if [info exists dynT_] {\n\
foreach tr $dynT_ {\n\
$tr format link-up {$src_} {$dst_}\n\
set ns [Simulator instance]\n\
$self instvar fromNode_ toNode_\n\
$tr ntrace \"l -t [$ns now] -s [$fromNode_ id] -d [$toNode_ id] -S UP\"\n\
$tr ntrace \"v -t [$ns now] link-up [$ns now] [$fromNode_ id] [$toNode_ id]\"\n\
}\n\
}\n\
}\n\
\n\
Link instproc down { } {\n\
$self instvar dynamics_ dynT_\n\
if ![info exists dynamics_] {\n\
puts stderr \"$class::$proc Link $self was not declared dynamic, and cannot be taken down.  ignored\"\n\
return\n\
}\n\
$dynamics_ set status_ 0\n\
$self all-connectors reset\n\
if [info exists dynT_] {\n\
foreach tr $dynT_ {\n\
$tr format link-down {$src_} {$dst_}\n\
set ns [Simulator instance]\n\
$self instvar fromNode_ toNode_\n\
$tr ntrace \"l -t [$ns now] -s [$fromNode_ id] -d [$toNode_ id] -S DOWN\"\n\
$tr ntrace \"v -t [$ns now] link-down [$ns now] [$fromNode_ id] [$toNode_ id]\"\n\
}\n\
}\n\
}\n\
\n\
Link instproc up? {} {\n\
$self instvar dynamics_\n\
if [info exists dynamics_] {\n\
return [$dynamics_ status?]\n\
} else {\n\
return \"up\"\n\
}\n\
}\n\
\n\
Link instproc all-connectors op {\n\
foreach c [$self info vars] {\n\
$self instvar $c\n\
if ![info exists $c] continue\n\
if [array size $c] continue\n\
foreach var [$self set $c] {\n\
if [catch \"$var info class\"] {\n\
continue\n\
}\n\
if ![$var info class Node] { ;# $op on most everything\n\
catch \"$var $op\";# in case var isn't a connector\n\
}\n\
}\n\
}\n\
}\n\
\n\
Link instproc install-error {em} {\n\
$self instvar link_\n\
$em target [$link_ target]\n\
$link_ target $em\n\
}\n\
\n\
Class SimpleLink -superclass Link\n\
\n\
SimpleLink instproc init { src dst bw delay q {lltype \"DelayLink\"} } {\n\
$self next $src $dst\n\
$self instvar link_ queue_ head_ toNode_ ttl_\n\
$self instvar drophead_\n\
\n\
set ns [Simulator instance]\n\
set drophead_ [new Connector]\n\
$drophead_ target [$ns set nullAgent_]\n\
\n\
set head_ [new Connector]\n\
$head_ set link_ $self\n\
\n\
if { [[$q info class] info heritage ErrModule] == \"ErrorModule\" } {\n\
$head_ target [$q classifier]\n\
} else {\n\
$head_ target $q\n\
}\n\
\n\
set queue_ $q\n\
set link_ [new $lltype]\n\
$link_ set bandwidth_ $bw\n\
$link_ set delay_ $delay\n\
\n\
$queue_ target $link_\n\
$link_ target [$dst entry]\n\
$queue_ drop-target $drophead_\n\
\n\
set ttl_ [new TTLChecker]\n\
$ttl_ target [$link_ target]\n\
$self ttl-drop-trace\n\
$link_ target $ttl_\n\
\n\
if { [$ns multicast?] } {\n\
$self enable-mcast $src $dst\n\
}\n\
$ns instvar srcRt_\n\
if [info exists srcRt_] {\n\
if { $srcRt_ == 1 } {\n\
$self enable-src-rt $src $dst $head_\n\
}\n\
}\n\
\n\
}\n\
\n\
SimpleLink instproc enable-src-rt {src dst head} {\n\
$self instvar ttl_\n\
$src instvar src_agent_\n\
$ttl_ target [$dst entry]\n\
$src_agent_ install_slot $head [$dst id]\n\
}\n\
\n\
\n\
SimpleLink instproc enable-mcast {src dst} {\n\
$self instvar iif_ ttl_\n\
set iif_ [new NetworkInterface]\n\
$iif_ target [$ttl_ target]\n\
$ttl_ target $iif_\n\
\n\
$src add-oif [$self head]  $self\n\
$dst add-iif [$iif_ label] $self\n\
}\n\
\n\
\n\
SimpleLink instproc bw {} { \n\
$self instvar link_\n\
$link_ set bandwidth_ \n\
\n\
}\n\
\n\
SimpleLink instproc delay {} {\n\
$self instvar link_\n\
$link_ set delay_\n\
}\n\
\n\
SimpleLink instproc qsize {} {\n\
[$self queue] set limit_\n\
}\n\
\n\
SimpleLink instproc nam-trace { ns f } {\n\
$self instvar enqT_ deqT_ drpT_ rcvT_ dynT_\n\
\n\
if [info exists enqT_] {\n\
$enqT_ namattach $f\n\
if [info exists deqT_] {\n\
$deqT_ namattach $f\n\
}\n\
if [info exists drpT_] {\n\
$drpT_ namattach $f\n\
}\n\
if [info exists rcvT_] {\n\
$rcvT_ namattach $f\n\
}\n\
if [info exists dynT_] {\n\
foreach tr $dynT_ {\n\
$tr namattach $f\n\
}\n\
}\n\
} else {\n\
$self trace $ns $f \"nam\"\n\
}\n\
}\n\
\n\
SimpleLink instproc trace { ns f {op \"\"} } {\n\
\n\
$self instvar enqT_ deqT_ drpT_ queue_ link_ fromNode_ toNode_\n\
$self instvar rcvT_ ttl_ trace_\n\
$self instvar drophead_		;# idea stolen from CBQ and Kevin\n\
\n\
set trace_ $f\n\
set enqT_ [$ns create-trace Enque $f $fromNode_ $toNode_ $op]\n\
set deqT_ [$ns create-trace Deque $f $fromNode_ $toNode_ $op]\n\
set drpT_ [$ns create-trace Drop $f $fromNode_ $toNode_ $op]\n\
set rcvT_ [$ns create-trace Recv $f $fromNode_ $toNode_ $op]\n\
\n\
$self instvar drpT_ drophead_\n\
set nxt [$drophead_ target]\n\
$drophead_ target $drpT_\n\
$drpT_ target $nxt\n\
\n\
$queue_ drop-target $drophead_\n\
\n\
\n\
$deqT_ target [$queue_ target]\n\
$queue_ target $deqT_\n\
\n\
$self add-to-head $enqT_\n\
\n\
$rcvT_ target [$ttl_ target]\n\
$ttl_ target $rcvT_\n\
\n\
$self instvar dynamics_\n\
if [info exists dynamics_] {\n\
$self trace-dynamics $ns $f $op\n\
}\n\
}\n\
\n\
SimpleLink instproc trace-dynamics { ns f {op \"\"}} {\n\
$self instvar dynT_ fromNode_ toNode_\n\
lappend dynT_ [$ns create-trace Generic $f $fromNode_ $toNode_ $op]\n\
$self transit-drop-trace\n\
$self linkfail-drop-trace\n\
}\n\
\n\
SimpleLink instproc ttl-drop-trace args {\n\
$self instvar ttl_\n\
if ![info exists ttl_] return\n\
if {[llength $args] != 0} {\n\
$ttl_ drop-target [lindex $args 0]\n\
} else {\n\
$self instvar drophead_\n\
$ttl_ drop-target $drophead_\n\
}\n\
}\n\
\n\
SimpleLink instproc transit-drop-trace args {\n\
$self instvar link_\n\
if {[llength $args] != 0} {\n\
$link_ drop-target [lindex $args 0]\n\
} else {\n\
$self instvar drophead_\n\
$link_ drop-target $drophead_\n\
}\n\
}\n\
\n\
SimpleLink instproc linkfail-drop-trace args {\n\
$self instvar dynamics_\n\
if ![info exists dynamics_] return\n\
if {[llength $args] != 0} {\n\
$dynamics_ drop-target [lindex $args 0]\n\
} else {\n\
$self instvar drophead_\n\
$dynamics_ drop-target $drophead_\n\
}\n\
}\n\
\n\
SimpleLink instproc trace-callback {ns cmd} {\n\
$self trace $ns {}\n\
foreach part {enqT_ deqT_ drpT_ rcvT_} {\n\
$self instvar $part\n\
set to [$self set $part]\n\
$to set callback_ 1\n\
$to proc handle a \"$cmd \\$a\"\n\
}\n\
}\n\
\n\
SimpleLink instproc attach-monitors { insnoop outsnoop dropsnoop qmon } {\n\
$self instvar drpT_ queue_ snoopIn_ snoopOut_ snoopDrop_\n\
$self instvar qMonitor_ drophead_\n\
\n\
set snoopIn_ $insnoop\n\
set snoopOut_ $outsnoop\n\
set snoopDrop_ $dropsnoop\n\
\n\
$self add-to-head $snoopIn_\n\
\n\
$snoopOut_ target [$queue_ target]\n\
$queue_ target $snoopOut_\n\
\n\
set nxt [$drophead_ target]\n\
$drophead_ target $snoopDrop_\n\
$snoopDrop_ target $nxt\n\
\n\
\n\
$snoopIn_ set-monitor $qmon\n\
$snoopOut_ set-monitor $qmon\n\
$snoopDrop_ set-monitor $qmon\n\
set qMonitor_ $qmon\n\
}\n\
\n\
SimpleLink instproc attach-taggers { insnoop qmon } {\n\
$self instvar drpT_ queue_ head_ snoopIn_ snoopOut_ snoopDrop_\n\
$self instvar qMonitor_ drophead_\n\
\n\
set snoopIn_ $insnoop\n\
\n\
$snoopIn_ target $head_\n\
set head_ $snoopIn_\n\
\n\
$snoopIn_ set-monitor $qmon\n\
\n\
\n\
set qMonitor_ $qmon\n\
\n\
}\n\
\n\
SimpleLink instproc init-monitor { ns qtrace sampleInterval} {\n\
$self instvar qMonitor_ ns_ qtrace_ sampleInterval_\n\
\n\
set ns_ $ns\n\
set qtrace_ $qtrace\n\
set sampleInterval_ $sampleInterval\n\
set qMonitor_ [new QueueMonitor]\n\
\n\
$self attach-monitors [new SnoopQueue/In] \\\n\
[new SnoopQueue/Out] [new SnoopQueue/Drop] $qMonitor_\n\
\n\
set bytesInt_ [new Integrator]\n\
$qMonitor_ set-bytes-integrator $bytesInt_\n\
set pktsInt_ [new Integrator]\n\
$qMonitor_ set-pkts-integrator $pktsInt_\n\
return $qMonitor_\n\
}\n\
\n\
SimpleLink instproc start-tracing { } {\n\
$self instvar qMonitor_ ns_ qtrace_ sampleInterval_\n\
$self instvar fromNode_ toNode_\n\
\n\
if {$qtrace_ != 0} {\n\
$qMonitor_ trace $qtrace_\n\
}\n\
$qMonitor_ set-src-dst [$fromNode_ id] [$toNode_ id]\n\
} \n\
\n\
SimpleLink instproc queue-sample-timeout { } {\n\
$self instvar qMonitor_ ns_ qtrace_ sampleInterval_\n\
$self instvar fromNode_ toNode_\n\
\n\
set qavg [$self sample-queue-size]\n\
if {$qtrace_ != 0} {\n\
puts $qtrace_ \"[$ns_ now] [$fromNode_ id] [$toNode_ id] $qavg\"\n\
}\n\
$ns_ at [expr [$ns_ now] + $sampleInterval_] \"$self queue-sample-timeout\"\n\
}\n\
\n\
SimpleLink instproc sample-queue-size { } {\n\
$self instvar qMonitor_ ns_ qtrace_ sampleInterval_ lastSample_\n\
\n\
set now [$ns_ now]\n\
set qBytesMonitor_ [$qMonitor_ get-bytes-integrator]\n\
set qPktsMonitor_ [$qMonitor_ get-pkts-integrator]\n\
\n\
$qBytesMonitor_ newpoint $now [$qBytesMonitor_ set lasty_]\n\
set bsum [$qBytesMonitor_ set sum_]\n\
\n\
$qPktsMonitor_ newpoint $now [$qPktsMonitor_ set lasty_]\n\
set psum [$qPktsMonitor_ set sum_]\n\
\n\
if ![info exists lastSample_] {\n\
set lastSample_ 0\n\
}\n\
set dur [expr $now - $lastSample_]\n\
if { $dur != 0 } {\n\
set meanBytesQ [expr $bsum / $dur]\n\
set meanPktsQ [expr $psum / $dur]\n\
} else {\n\
set meanBytesQ 0\n\
set meanPktsQ 0\n\
}\n\
$qBytesMonitor_ set sum_ 0.0\n\
$qPktsMonitor_ set sum_ 0.0\n\
set lastSample_ $now\n\
\n\
\n\
$qMonitor_ instvar pdrops_ pdepartures_ parrivals_ bdrops_ bdepartures_ barrivals_\n\
\n\
return \"$meanBytesQ $meanPktsQ $parrivals_ $pdepartures_ $pdrops_ $barrivals_ $bdepartures_ $bdrops_\"	\n\
\n\
}	\n\
\n\
\n\
SimpleLink instproc dynamic {} {\n\
$self instvar dynamics_\n\
\n\
if [info exists dynamics_] return\n\
\n\
set dynamics_ [new DynamicLink]\n\
$self add-to-head $dynamics_\n\
\n\
$self transit-drop-trace\n\
$self all-connectors isDynamic\n\
}\n\
\n\
SimpleLink instproc errormodule args {\n\
$self instvar errmodule_ queue_ drophead_\n\
if { $args == \"\" } {\n\
return $errmodule_\n\
}\n\
\n\
set em [lindex $args 0]\n\
set errmodule_ $em\n\
\n\
$self add-to-head $em\n\
\n\
$em drop-target $drophead_\n\
}\n\
\n\
SimpleLink instproc insert-linkloss args { \n\
$self instvar link_errmodule_ queue_ drophead_ deqT_ \n\
if { $args == \"\" } {\n\
return $link_errmodule_\n\
}\n\
\n\
set em [lindex $args 0]\n\
if [info exists link_errmodule_] {\n\
delete link_errmodule_\n\
}\n\
set link_errmodule_ $em\n\
\n\
if [info exists deqT_] {\n\
$em target [$deqT_ target]\n\
$deqT_ target $em\n\
} else {\n\
$em target [$queue_ target]\n\
$queue_ target $em\n\
}\n\
\n\
$em drop-target $drophead_\n\
}\n\
\n\
\n\
\n\
\n\
\n\
Class Application/FTP -superclass Application\n\
\n\
Application/FTP instproc init {} {\n\
$self next\n\
}\n\
\n\
Application/FTP instproc start {} {\n\
[$self agent] send -1\n\
}\n\
\n\
Application/FTP instproc stop {} {\n\
[$self agent] advance 0\n\
[$self agent] close\n\
}\n\
\n\
Application/FTP instproc send {nbytes} {\n\
[$self agent] send $nbytes\n\
}\n\
\n\
Application/FTP instproc produce { pktcnt } {\n\
[$self agent] advance $pktcnt\n\
}\n\
\n\
Application/FTP instproc producemore { pktcnt } {\n\
[$self agent] advanceby $pktcnt\n\
}\n\
\n\
Application/Traffic instproc set args {\n\
$self instvar packetSize_ rate_\n\
if { [lindex $args 0] == \"packet_size_\" } {\n\
if { [llength $args] == 2 } {\n\
set packetSize_ [lindex $args 1]\n\
return\n\
} elseif { [llength $args] == 1 } {\n\
return $packetSize_\n\
}\n\
}\n\
eval $self next $args\n\
}\n\
Application/Traffic/CBR instproc set args {\n\
$self instvar packetSize_ rate_\n\
if { [lindex $args 0] == \"interval_\" } {\n\
if { [llength $args] == 2 } {\n\
set ns_ [Simulator instance]\n\
set interval_ [$ns_ delay_parse [lindex $args 1]]\n\
$self set rate_ [expr $packetSize_ * 8.0/$interval_]\n\
return\n\
} elseif { [llength $args] == 1 } {\n\
return [expr $packetSize_ * 8.0/$rate_]\n\
}\n\
}\n\
eval $self next $args\n\
}\n\
\n\
\n\
Class Agent/CBR -superclass Agent/UDP\n\
Class Agent/CBR/UDP -superclass Agent/UDP\n\
Class Agent/CBR/RTP -superclass Agent/RTP\n\
Class Agent/CBR/UDP/SA -superclass Agent/SA\n\
\n\
Agent/SA instproc attach-traffic tg {\n\
$tg attach-agent $self\n\
eval $self cmd attach-traffic $tg\n\
}\n\
\n\
Agent/CBR/UDP instproc attach-traffic tg {\n\
$self instvar trafgen_\n\
$tg attach-agent $self\n\
set trafgen_ $tg\n\
}\n\
\n\
Agent/CBR/UDP instproc done {} { }\n\
\n\
Agent/CBR/UDP instproc start {} {\n\
$self instvar trafgen_\n\
$trafgen_ start\n\
}\n\
\n\
Agent/CBR/UDP instproc stop {} {\n\
$self instvar trafgen_\n\
$trafgen_ stop\n\
}\n\
\n\
Agent/CBR/UDP instproc advance args {\n\
$self instvar trafgen_\n\
eval $trafgen_ advance $args\n\
}\n\
\n\
Agent/CBR/UDP instproc advanceby args {\n\
$self instvar trafgen_\n\
eval $trafgen_ advanceby $args\n\
}\n\
\n\
Agent/CBR instproc init {} {\n\
$self next\n\
$self instvar trafgen_ interval_ random_ packetSize_ maxpkts_\n\
set packetSize_ 210\n\
set random_ 0\n\
set maxpkts_ 268435456	\n\
set interval_ 0.00375\n\
set trafgen_ [new Application/Traffic/CBR]\n\
$trafgen_ attach-agent $self\n\
$trafgen_ set rate_ [expr $packetSize_ * 8.0/ $interval_]\n\
$trafgen_ set random_ [$self set random_]\n\
$trafgen_ set maxpkts_ [$self set maxpkts_]\n\
$trafgen_ set packetSize_ [$self set packetSize_]\n\
if {[Simulator set nsv1flag] == 0} { \n\
\n\
puts \"using backward compatible Agent/CBR; use Application/Traffic/CBR instead\"\n\
}    \n\
}\n\
\n\
\n\
\n\
Agent/CBR instproc done {} { }\n\
\n\
Agent/CBR instproc start {} {\n\
$self instvar trafgen_\n\
$trafgen_ start\n\
}\n\
\n\
Agent/CBR instproc stop {} {\n\
$self instvar trafgen_\n\
$trafgen_ stop\n\
}\n\
\n\
Agent/CBR instproc advance args {\n\
$self instvar trafgen_\n\
eval $trafgen_ advance $args\n\
}\n\
\n\
Agent/CBR instproc advanceby args {\n\
$self instvar trafgen_\n\
eval $trafgen_ advanceby $args\n\
}\n\
\n\
Agent/CBR instproc set args {\n\
$self instvar interval_ random_ packetSize_ maxpkts_ trafgen_\n\
if { [info exists trafgen_] } {\n\
if { [lindex $args 0] == \"packetSize_\" } {\n\
if { [llength $args] == 2 } {\n\
$trafgen_ set packetSize_ [lindex $args 1]\n\
set packetSize_ [lindex $args 1]\n\
$trafgen_ set rate_ [expr $packetSize_ * 8.0/ $interval_]\n\
return \n\
} elseif { [llength $args] == 1 } {\n\
return $packetSize_\n\
}\n\
} elseif { [lindex $args 0] == \"random_\" } {\n\
if { [llength $args] == 2 } {\n\
$trafgen_ set random_ [lindex $args 1]\n\
set random_ [lindex $args 1]\n\
return\n\
} elseif { [llength $args] == 1 } {\n\
return $random_\n\
}\n\
} elseif { [lindex $args 0] == \"maxpkts_\" } {\n\
if { [llength $args] == 2 } {\n\
$trafgen_ set maxpkts_ [lindex $args 1]\n\
set maxpkts_ [lindex $args 1]\n\
return\n\
} elseif { [llength $args] == 1 } {\n\
return $maxpkts_\n\
}\n\
} elseif { [lindex $args 0] == \"interval_\" } {\n\
if { [llength $args] == 2 } {\n\
set ns_ [Simulator instance]\n\
set interval_ [$ns_ delay_parse [lindex $args 1]]\n\
$trafgen_ set rate_ [expr $packetSize_ * 8.0/ $interval_]\n\
return\n\
} elseif { [llength $args] == 1 } {\n\
return $interval_\n\
}\n\
}\n\
}\n\
eval $self next $args\n\
}\n\
\n\
Class Traffic/Expoo -superclass Application/Traffic/Exponential\n\
Class Traffic/Pareto -superclass Application/Traffic/Pareto\n\
Class Traffic/RealAudio -superclass Application/Traffic/RealAudio\n\
Class Traffic/Trace -superclass Application/Traffic/Trace\n\
\n\
Traffic/Expoo instproc set args {\n\
$self instvar packetSize_ burst_time_ idle_time_ rate_ \n\
if { [lindex $args 0] == \"packet-size\" } {\n\
if { [llength $args] == 2 } {\n\
$self set packetSize_ [lindex $args 1]\n\
return \n\
} elseif { [llength $args] == 1 } {\n\
return $packetSize_\n\
}\n\
} elseif { [lindex $args 0] == \"burst-time\" } {\n\
if { [llength $args] == 2 } {\n\
$self set burst_time_ [lindex $args 1]\n\
return \n\
} elseif { [llength $args] == 1 } {\n\
return $burst_time_\n\
}\n\
} elseif { [lindex $args 0] == \"idle-time\" } {\n\
if { [llength $args] == 2 } {\n\
$self set idle_time_ [lindex $args 1]\n\
return \n\
} elseif { [llength $args] == 1 } {\n\
return $idle_time_\n\
}\n\
} elseif { [lindex $args 0] == \"rate\" } {\n\
if { [llength $args] == 2 } {\n\
$self set rate_ [lindex $args 1]\n\
return \n\
} elseif { [llength $args] == 1 } {\n\
return $rate_\n\
}\n\
}\n\
eval $self next $args\n\
}\n\
\n\
Traffic/Pareto instproc set args {\n\
$self instvar packetSize_ burst_time_ idle_time_ rate_ shape_\n\
if { [lindex $args 0] == \"packet-size\" } {\n\
if { [llength $args] == 2 } {\n\
$self set packetSize_ [lindex $args 1]\n\
return \n\
} elseif { [llength $args] == 1 } {\n\
return $packetSize_\n\
}\n\
} elseif { [lindex $args 0] == \"burst-time\" } {\n\
if { [llength $args] == 2 } {\n\
$self set burst_time_ [lindex $args 1]\n\
return \n\
} elseif { [llength $args] == 1 } {\n\
return $burst_time_\n\
}\n\
} elseif { [lindex $args 0] == \"idle-time\" } {\n\
if { [llength $args] == 2 } {\n\
$self set idle_time_ [lindex $args 1]\n\
return \n\
} elseif { [llength $args] == 1 } {\n\
return $idle_time_\n\
}\n\
} elseif { [lindex $args 0] == \"rate\" } {\n\
if { [llength $args] == 2 } {\n\
$self set rate_ [lindex $args 1]\n\
return \n\
} elseif { [llength $args] == 1 } {\n\
return $rate_\n\
}\n\
} elseif { [lindex $args 0] == \"shape\" } {\n\
if { [llength $args] == 2 } {\n\
$self set shape_ [lindex $args 1]\n\
return \n\
} elseif { [llength $args] == 1 } {\n\
return $shape_\n\
}\n\
}\n\
eval $self next $args\n\
}\n\
\n\
Traffic/RealAudio instproc set args {\n\
$self instvar packetSize_ burst_time_ idle_time_ rate_ \n\
if { [lindex $args 0] == \"packet-size\" } {\n\
if { [llength $args] == 2 } {\n\
$self set packetSize_ [lindex $args 1]\n\
return \n\
} elseif { [llength $args] == 1 } {\n\
return $packetSize_\n\
}\n\
} elseif { [lindex $args 0] == \"burst-time\" } {\n\
if { [llength $args] == 2 } {\n\
$self set burst_time_ [lindex $args 1]\n\
return \n\
} elseif { [llength $args] == 1 } {\n\
return $burst_time_\n\
}\n\
} elseif { [lindex $args 0] == \"idle-time\" } {\n\
if { [llength $args] == 2 } {\n\
$self set idle_time_ [lindex $args 1]\n\
return \n\
} elseif { [llength $args] == 1 } {\n\
return $idle_time_\n\
}\n\
} elseif { [lindex $args 0] == \"rate\" } {\n\
if { [llength $args] == 2 } {\n\
$self set rate_ [lindex $args 1]\n\
return \n\
} elseif { [llength $args] == 1 } {\n\
return $rate_\n\
}\n\
}\n\
eval $self next $args\n\
}\n\
\n\
Class Source/FTP -superclass Application\n\
Source/FTP set maxpkts_ 268435456\n\
\n\
Source/FTP instproc attach o {\n\
$self instvar agent_\n\
set agent_ $o\n\
$self attach-agent $o\n\
}\n\
\n\
Source/FTP instproc init {} {\n\
$self next\n\
$self instvar maxpkts_ agent_\n\
set maxpkts_ 268435456\n\
}\n\
\n\
Source/FTP instproc start {} {\n\
$self instvar agent_ maxpkts_\n\
$agent_ advance $maxpkts_\n\
}\n\
\n\
Source/FTP instproc stop {} {\n\
$self instvar agent_\n\
$agent_ advance 0\n\
}\n\
\n\
Source/FTP instproc produce { pktcnt } {\n\
$self instvar agent_ \n\
$agent_ advance $pktcnt\n\
}\n\
\n\
Source/FTP instproc producemore { pktcnt } {\n\
$self instvar agent_\n\
$agent_ advanceby $pktcnt\n\
}\n\
\n\
\n\
Class Source/Telnet -superclass Application/Telnet\n\
\n\
Source/Telnet set maxpkts_ 268435456\n\
\n\
Source/Telnet instproc attach o {\n\
$self instvar agent_\n\
set agent_ $o\n\
$self attach-agent $o\n\
}\n\
\n\
\n\
Class OldSim -superclass Simulator\n\
\n\
proc ns args {\n\
OldSim ns\n\
eval ns $args\n\
}\n\
\n\
OldSim instproc default_catch { varName index op } {\n\
if { $index == \"\" } {\n\
error \"ns-1 compat: default change caught, but not a default! (varName: $varName)\"\n\
exit 1\n\
}\n\
\n\
if { $op == \"r\" || $op == \"u\" } {\n\
error \"ns-1 compat: default change caught a $op operation\"\n\
exit 1\n\
}\n\
set vname ${varName}($index)\n\
upvar $vname var\n\
$self default_assign $varName $index $var\n\
}\n\
\n\
\n\
OldSim instproc default_assign {aname index newval} {\n\
$self instvar classMap_ queueMap_\n\
if { $index == \"\" } {\n\
puts \"something funny with default traces\"\n\
exit 1\n\
}\n\
set obj [string trimleft $aname ns_]\n\
if { $obj == \"link\" } {\n\
if { $index == \"queue-limit\" } {\n\
Queue set limit_ $newval\n\
return\n\
}\n\
set ivar \"$index\\_\"\n\
if { [lsearch [DelayLink info vars] $ivar] >= 0 } {\n\
DelayLink set $ivar $newval\n\
return\n\
}\n\
error \"warning: ns-1 compatibility library cannot set link default ${aname}($index)\"\n\
return\n\
}\n\
\n\
if ![info exists classMap_($obj)] {\n\
if ![info exists queueMap_($obj)] {\n\
puts \"error: ns-2 compatibility library cannot set ns-v1 default ${aname}($index)\"\n\
exit 1\n\
} else {\n\
set ns2obj \"Queue/$queueMap_($obj)\"\n\
}\n\
} else {\n\
set ns2obj $classMap_($obj)\n\
}\n\
SplitObject instvar varMap_ \n\
if ![info exists varMap_($index)] {\n\
puts \"error: ns-2 compatibility library cannot map instvar $index in class $ns2obj\"\n\
exit 1\n\
}\n\
$ns2obj set $varMap_($index) $newval\n\
\n\
}\n\
\n\
OldSim instproc map_ns_defaults old_arr {\n\
global $old_arr ; # these were all globals in ns-1\n\
SplitObject instvar varMap_\n\
\n\
foreach el [array names $old_arr] {\n\
set val [expr \"$${old_arr}($el)\"]\n\
$self default_assign $old_arr $el $val\n\
}\n\
\n\
trace variable $old_arr rwu \"$self default_catch\"\n\
}\n\
\n\
OldSim instproc trace_old_defaults {} {\n\
$self map_ns_defaults ns_tcp\n\
$self map_ns_defaults ns_tcpnewreno\n\
$self map_ns_defaults ns_trace\n\
$self map_ns_defaults ns_fulltcp\n\
$self map_ns_defaults ns_red\n\
$self map_ns_defaults ns_cbq\n\
$self map_ns_defaults ns_class\n\
$self map_ns_defaults ns_sink\n\
$self map_ns_defaults ns_delsink\n\
$self map_ns_defaults ns_sacksink\n\
$self map_ns_defaults ns_cbr\n\
$self map_ns_defaults ns_rlm\n\
$self map_ns_defaults ns_ivs\n\
$self map_ns_defaults ns_source\n\
$self map_ns_defaults ns_telnet\n\
$self map_ns_defaults ns_bursty\n\
$self map_ns_defaults ns_message\n\
$self map_ns_defaults ns_facktcp\n\
$self map_ns_defaults ns_link\n\
$self map_ns_defaults ns_lossy_uniform\n\
$self map_ns_defaults ns_lossy_patt\n\
$self map_ns_defaults ns_queue\n\
$self map_ns_defaults ns_srm\n\
}\n\
\n\
OldSim instproc init args {\n\
eval $self next $args\n\
puts stderr \"warning: using backward compatibility mode\"\n\
$self instvar classMap_\n\
\n\
Simulator set nsv1flag 1\n\
\n\
$self instvar scheduler_\n\
set scheduler_ [new Scheduler/List]\n\
\n\
Queue/CBQ instproc set args {\n\
$self instvar compat_qlim_\n\
if { [lindex $args 0] == \"queue-limit\" || \\\n\
[lindex $args 0] == \"limit_\" } { \n\
if { [llength $args] == 2 } {\n\
set val [lindex $args 1]\n\
set compat_qlim_ $val\n\
return $val\n\
}\n\
return $compat_qlim_\n\
} elseif { [lindex $args 0] == \"algorithm_\" } {\n\
$self algorithm [lindex $args 1]\n\
}\n\
eval $self next $args\n\
}\n\
Queue/DropTail instproc set args {\n\
if { [llength $args] == 2 &&\n\
[lindex $args 0] == \"queue-limit\" } {\n\
$self set limit_ [lindex $args 1]\n\
return\n\
}\n\
eval $self next $args\n\
}\n\
Queue/RED instproc set args {\n\
if { [llength $args] == 2 &&\n\
[lindex $args 0] == \"queue-limit\" } {\n\
$self set limit_ [lindex $args 1]\n\
return\n\
}\n\
eval $self next $args\n\
}\n\
Queue/RED instproc enable-vartrace file {\n\
$self trace ave_\n\
$self trace prob_\n\
$self trace curq_\n\
$self attach $file\n\
}\n\
Source/FTP instproc set args {\n\
if { [llength $args] == 2 &&\n\
[lindex $args 0] == \"maxpkts\" } {\n\
$self set maxpkts_ [lindex $args 1]\n\
return\n\
}\n\
eval $self next $args\n\
}\n\
Source/Telnet instproc set args {\n\
if { [llength $args] == 2 &&\n\
[lindex $args 0] == \"interval\" } {\n\
$self set interval_ [lindex $args 1]\n\
return\n\
}\n\
eval $self next $args\n\
}\n\
Agent/TCP instproc source type {\n\
if { $type == \"ftp\" } {\n\
set type FTP\n\
}\n\
if { $type == \"telnet\" } {\n\
set type Telnet\n\
}\n\
set src [new Source/$type]\n\
$src attach $self\n\
return $src\n\
}\n\
Agent/TCP set restart_bugfix_ false\n\
SplitObject instproc set args {\n\
SplitObject instvar varMap_\n\
set var [lindex $args 0] \n\
if [info exists varMap_($var)] {\n\
set var $varMap_($var)\n\
set args \"$var [lrange $args 1 end]\"\n\
}\n\
$self instvar -parse-part1 $var\n\
if {[llength $args] == 1} {\n\
return [subst $[subst $var]]\n\
} else {\n\
return [set $var [lrange $args 1 end]]\n\
}\n\
}\n\
SplitObject instproc get {var} {\n\
SplitObject instvar varMap_\n\
if [info exists varMap_($var)] {\n\
return [$self set $varMap_($var)]\n\
} else {\n\
return [$self next $var]\n\
}\n\
}\n\
TclObject set varMap_(addr) addr_\n\
TclObject set varMap_(dst) dst_\n\
TclObject set varMap_(cls) fid_\n\
\n\
TclObject set varMap_(src) src_\n\
TclObject set varMap_(show_tcphdr) show_tcphdr_\n\
\n\
TclObject set varMap_(window) window_\n\
TclObject set varMap_(window-init) windowInit_\n\
TclObject set varMap_(window-option) windowOption_\n\
TclObject set varMap_(window-constant) windowConstant_\n\
TclObject set varMap_(window-thresh) windowThresh_\n\
TclObject set varMap_(overhead) overhead_\n\
TclObject set varMap_(tcp-tick) tcpTick_\n\
TclObject set varMap_(ecn) ecn_\n\
TclObject set varMap_(bug-fix) bugFix_\n\
TclObject set varMap_(maxburst) maxburst_\n\
TclObject set varMap_(maxcwnd) maxcwnd_\n\
TclObject set varMap_(dupacks) dupacks_\n\
TclObject set varMap_(seqno) seqno_\n\
TclObject set varMap_(ack) ack_\n\
TclObject set varMap_(cwnd) cwnd_\n\
TclObject set varMap_(awnd) awnd_\n\
TclObject set varMap_(ssthresh) ssthresh_\n\
TclObject set varMap_(rtt) rtt_\n\
TclObject set varMap_(srtt) srtt_\n\
TclObject set varMap_(rttvar) rttvar_\n\
TclObject set varMap_(backoff) backoff_\n\
TclObject set varMap_(v-alpha) v_alpha_\n\
TclObject set varMap_(v-beta) v_beta_\n\
TclObject set varMap_(v-gamma) v_gamma_\n\
\n\
TclObject set varMap_(changes) newreno_changes_\n\
\n\
TclObject set varMap_(rampdown) rampdown_ \n\
TclObject set varMap_(ss-div4) ss-div4_\n\
\n\
TclObject set varMap_(limit) limit_\n\
\n\
TclObject set varMap_(limit) maxqueue_\n\
TclObject set varMap_(buckets) buckets_\n\
\n\
TclObject set varMap_(bytes) bytes_\n\
TclObject set varMap_(thresh) thresh_\n\
TclObject set varMap_(maxthresh) maxthresh_\n\
TclObject set varMap_(mean_pktsize) meanPacketSize_\n\
TclObject set varMap_(q_weight) queueWeight_\n\
TclObject set varMap_(wait) wait_\n\
TclObject set varMap_(linterm) linterm_\n\
TclObject set varMap_(setbit) setbit_\n\
TclObject set varMap_(drop-tail) dropTail_\n\
TclObject set varMap_(doubleq) doubleq_\n\
TclObject set varMap_(dqthresh) dqthresh_\n\
TclObject set varMap_(subclasses) subclasses_\n\
TclObject set varMap_(algorithm) algorithm_\n\
TclObject set varMap_(max-pktsize) maxpkt_\n\
TclObject set varMap_(priority) priority_\n\
TclObject set varMap_(maxidle) maxidle_\n\
TclObject set varMap_(extradelay) extradelay_\n\
\n\
TclObject set varMap_(packet-size) packetSize_\n\
TclObject set varMap_(interval) interval_\n\
\n\
TclObject set varMap_(random) random_\n\
\n\
TclObject set varMap_(S) S_\n\
TclObject set varMap_(R) R_\n\
TclObject set varMap_(state) state_\n\
TclObject set varMap_(rttShift) rttShift_\n\
TclObject set varMap_(keyShift) keyShift_\n\
TclObject set varMap_(key) key_\n\
TclObject set varMap_(maxrtt) maxrtt_\n\
\n\
Class traceHelper\n\
traceHelper instproc attach f {\n\
$self instvar file_\n\
set file_ $f\n\
}\n\
\n\
Class linkHelper\n\
linkHelper instproc init args {\n\
$self instvar node1_ node2_ linkref_ queue_\n\
set node1_ [lindex $args 0]\n\
set node2_ [lindex $args 1]\n\
set lid [$node1_ id]:[$node2_ id]	    \n\
set linkref_ [ns set link_($lid)]\n\
set queue_ [$linkref_ queue]\n\
set sqi [new SnoopQueue/In]\n\
set sqo [new SnoopQueue/Out]\n\
set sqd [new SnoopQueue/Drop]\n\
set dsamples [new Samples]\n\
set qmon [new QueueMonitor/Compat]\n\
$qmon set-delay-samples $dsamples\n\
$linkref_ attach-monitors $sqi $sqo $sqd $qmon\n\
$linkref_ set bytesInt_ [new Integrator]\n\
$linkref_ set pktsInt_ [new Integrator]\n\
$qmon set-bytes-integrator [$linkref_ set bytesInt_]\n\
$qmon set-pkts-integrator [$linkref_ set pktsInt_]\n\
}\n\
linkHelper instproc trace traceObj {\n\
$self instvar node1_ node2_\n\
$self instvar queue_\n\
set tfile [$traceObj set file_]\n\
ns trace-queue $node1_ $node2_ $tfile\n\
if { [string first Queue/RED [$queue_ info class]] == 0 } {\n\
$queue_ enable-vartrace $tfile\n\
}\n\
}\n\
linkHelper instproc callback {fn} {\n\
$self instvar linkref_\n\
foreach part {enqT_ deqT_ drpT_} {\n\
set to [$linkref_ set $part]\n\
$to set callback_ 1\n\
$to proc handle {args} \"$fn \\$args\"\n\
}\n\
}\n\
linkHelper instproc set { var val } {\n\
\n\
$self instvar linkref_ queue_\n\
set qvars [$queue_ info vars]\n\
set linkvars [$linkref_ info vars]\n\
set linkdelayvars [[$linkref_ link] info vars]\n\
if { [string last _ $var] != ( [string length $var] - 1) } {\n\
set var ${var}_\n\
}\n\
if { $var == \"queue-limit_\" } {\n\
set var \"limit_\"\n\
}\n\
if { [lsearch $qvars $var] >= 0 } {\n\
$queue_ set $var $val\n\
} elseif { [lsearch $linkvars $var] >= 0 } {\n\
$linkref_ set $var $val\n\
} elseif { [lsearch $linkdelayvars $var] >= 0 } {\n\
[$linkref_ link] set $var $val\n\
} else {\n\
puts stderr \"linkHelper warning: couldn't set unknown variable $var\"\n\
}\n\
}\n\
\n\
linkHelper instproc get var {\n\
\n\
$self instvar linkref_ queue_\n\
set qvars [$queue_ info vars]\n\
set linkvars [$linkref_ info vars]\n\
set linkdelayvars [[$linkref_ link] info vars]\n\
if { [string last _ $var] != ( [string length $var] - 1) } {\n\
set var ${var}_\n\
}\n\
if { $var == \"queue-limit_\" } {\n\
set var \"limit_\"\n\
}\n\
if { [lsearch $qvars $var] >= 0 } {\n\
return [$queue_ set $var]\n\
} elseif { [lsearch $linkvars $var] >= 0 } {\n\
return [$linkref_ set $var]\n\
} elseif { [lsearch $linkdelayvars $var] >= 0 } {\n\
return [[$linkref_ link] set $var]\n\
} else {\n\
puts stderr \"linkHelper warning: couldn't set unknown variable $var\"\n\
return \"\"\n\
}\n\
return \"\"\n\
}\n\
\n\
linkHelper instproc try { obj operation argv } {\n\
set op [eval list $obj $operation $argv]\n\
set ocl [$obj info class]\n\
set iprocs [$ocl info instcommands]\n\
set oprocs [$obj info commands]\n\
if { $operation != \"cmd\" } {\n\
if { [lsearch $iprocs $operation] >= 0 } {\n\
return [eval $op]\n\
}\n\
if { [lsearch $oprocs $operation] >= 0 } {\n\
return [eval $op]\n\
}\n\
}\n\
if [catch $op ret] {\n\
return -1\n\
}\n\
return $ret\n\
}\n\
linkHelper instproc unknown { m args } {\n\
$self instvar linkref_ queue_\n\
set oldbody [TclObject info instbody unknown]\n\
TclObject instproc unknown args {\n\
if { [lindex $args 0] == \"cmd\" } {\n\
puts stderr \"Can't dispatch $args\"\n\
exit 1\n\
}\n\
eval $self cmd $args\n\
}\n\
\n\
set rval [$self try $queue_ $m $args]\n\
if { $rval != -1 } {\n\
TclObject instproc unknown args $oldbody\n\
return $rval\n\
}\n\
set rval [$self try $queue_ cmd [list $m $args]]\n\
if { $rval != -1 } {\n\
TclObject instproc unknown args $oldbody\n\
return $rval\n\
}\n\
set rval [$self try $linkref_ $m $args]\n\
if { $rval != -1 } {\n\
TclObject instproc unknown args $oldbody\n\
return $rval\n\
}\n\
set rval [$self try $linkref_ cmd [list $m $args]]\n\
if { $rval != -1 } {\n\
TclObject instproc unknown args $oldbody\n\
return $rval\n\
}\n\
set dlink [$linkref_ link]\n\
set rval [$self try $dlink $m $args]\n\
if { $rval != -1 } {\n\
TclObject instproc unknown args $oldbody\n\
return $rval\n\
}\n\
set rval [$self try $dlink cmd [list $m $args]]\n\
if { $rval != -1 } {\n\
TclObject instproc unknown args $oldbody\n\
return $rval\n\
}\n\
TclObject instproc unknown args $oldbody\n\
puts stderr \"Unknown operation $m or subbordinate operation failed\"\n\
exit 1\n\
}\n\
linkHelper instproc stat { classid item } {\n\
$self instvar linkref_\n\
set qmon [$linkref_ set qMonitor_]\n\
if { $item == \"packets\" } {\n\
return [$qmon pkts $classid]\n\
} elseif { $item == \"bytes\" } {\n\
return [$qmon bytes $classid]\n\
} elseif { $item == \"drops\"} {\n\
return [$qmon drops $classid]\n\
} elseif { $item == \"mean-qdelay\" } {\n\
set dsamp [$qmon get-class-delay-samples $classid]\n\
if { [$dsamp cnt] > 0 } {\n\
return [$dsamp mean]\n\
} else {\n\
return NaN\n\
}\n\
} else {\n\
puts stderr \"linkHelper: unknown stat op $item\"\n\
exit 1\n\
}\n\
}\n\
linkHelper instproc integral { itype } {\n\
$self instvar linkref_\n\
if { $itype == \"qsize\" } {\n\
set integ [$linkref_ set bytesInt_]\n\
} elseif { $itype == \"qlen\" } {\n\
set integ [$linkref_ set pktsInt_]\n\
}\n\
\n\
return [$integ set sum_]\n\
}\n\
\n\
\n\
set classMap_(tcp) Agent/TCP\n\
set classMap_(tcp-reno) Agent/TCP/Reno\n\
set classMap_(tcp-vegas) Agent/TCP/Vegas\n\
set classMap_(tcp-full) Agent/TCP/FullTcp\n\
set classMap_(fulltcp) Agent/TCP/FullTcp\n\
set classMap_(tcp-fack) Agent/TCP/Fack\n\
set classMap_(facktcp) Agent/TCP/Fack\n\
set classMap_(tcp-newreno) Agent/TCP/Newreno\n\
set classMap_(tcpnewreno) Agent/TCP/Newreno\n\
set classMap_(cbr) Agent/CBR\n\
set classMap_(tcp-sink) Agent/TCPSink\n\
set classMap_(tcp-sack1) Agent/TCP/Sack1\n\
set classMap_(sack1-tcp-sink) Agent/TCPSink/Sack1\n\
set classMap_(tcp-sink-da) Agent/TCPSink/DelAck\n\
set classMap_(sack1-tcp-sink-da) Agent/TCPSink/Sack1/DelAck\n\
set classMap_(sink) Agent/TCPSink\n\
set classMap_(delsink) Agent/TCPSink/DelAck\n\
set classMap_(sacksink) Agent/TCPSink ; # sacksink becomes TCPSink here\n\
set classMap_(loss-monitor) Agent/LossMonitor\n\
set classMap_(class) CBQClass\n\
set classMap_(ivs) Agent/IVS/Source\n\
set classMap_(trace) Trace\n\
set classMap_(srm) Agent/SRM\n\
\n\
$self instvar queueMap_\n\
set queueMap_(drop-tail) DropTail\n\
set queueMap_(sfq) SFQ\n\
set queueMap_(red) RED\n\
set queueMap_(cbq) CBQ\n\
set queueMap_(wrr-cbq) CBQ/WRR\n\
\n\
$self trace_old_defaults\n\
\n\
global tcl_version\n\
if {$tcl_version < 8} {\n\
set class_name \"class\"\n\
} else {\n\
set class_name \"::class\"\n\
}\n\
proc $class_name args {\n\
set arglen [llength $args]\n\
if { $arglen < 2 } {\n\
return\n\
}\n\
set op [lindex $args 0]\n\
set id [lindex $args 1]\n\
if { $op != \"create\" } {\n\
error \"ns-v1 compat: malformed class operation: op $op\"\n\
return\n\
}\n\
eval CBQClass create $id [lrange $args 2 [expr $arglen - 1]]\n\
}\n\
}\n\
\n\
OldSim instproc simplex-link-compat { n1 n2 bw delay qtype } {\n\
set linkhelp [$self link-threeargs $n1 $n2 $qtype]\n\
$linkhelp set bandwidth_ $bw\n\
$linkhelp set delay_ $delay\n\
}\n\
\n\
OldSim instproc duplex-link-compat { n1 n2 bw delay type } {\n\
ns simplex-link-compat $n1 $n2 $bw $delay $type\n\
ns simplex-link-compat $n2 $n1 $bw $delay $type\n\
}\n\
\n\
OldSim instproc get-queues { n1 n2 } {\n\
$self instvar link_\n\
set n1 [$n1 id]\n\
set n2 [$n2 id]\n\
return \"[$link_($n1:$n2) queue] [$link_($n2:$n1) queue]\"\n\
}\n\
\n\
OldSim instproc create-agent { node type pktClass } {\n\
\n\
$self instvar classMap_\n\
if ![info exists classMap_($type)] {\n\
puts stderr \\\n\
\"backward compat bug: need to update classMap for $type\"\n\
exit 1\n\
}\n\
set agent [new $classMap_($type)]\n\
$agent set fid_ $pktClass\n\
$self attach-agent $node $agent\n\
\n\
\n\
return $agent\n\
}\n\
\n\
OldSim instproc agent { type node } {\n\
return [$self create-agent $node $type 0]\n\
}\n\
\n\
OldSim instproc create-connection \\\n\
{ srcType srcNode sinkType sinkNode pktClass } {\n\
\n\
set src [$self create-agent $srcNode $srcType $pktClass]\n\
set sink [$self create-agent $sinkNode $sinkType $pktClass]\n\
$self connect $src $sink\n\
\n\
return $src\n\
}\n\
\n\
proc ns_connect { src sink } {\n\
return [ns connect $src $sink]\n\
}\n\
\n\
OldSim instproc link args {\n\
set nargs [llength $args]\n\
set arg0 [lindex $args 0]\n\
set arg1 [lindex $args 1]\n\
if { $nargs == 2 } {\n\
return [$self link-twoargs $arg0 $arg1]\n\
} elseif { $nargs == 3 } {\n\
return [$self link-threeargs $arg0 $arg1 [lindex $args 2]]\n\
}\n\
}\n\
OldSim instproc link-twoargs { n1 n2 } {\n\
$self instvar LH_\n\
if ![info exists LH_($n1:$n2)] {\n\
set LH_($n1:$n2) 1\n\
linkHelper LH_:$n1:$n2 $n1 $n2\n\
}\n\
return LH_:$n1:$n2\n\
}\n\
\n\
OldSim instproc link-threeargs { n1 n2 qtype } {\n\
$self simplex-link $n1 $n2 0 0 $qtype\n\
return [$self link-twoargs $n1 $n2]\n\
}\n\
OldSim instproc trace {} {\n\
return [new traceHelper]\n\
}\n\
\n\
OldSim instproc random { seed } {\n\
return [ns-random $seed]\n\
}\n\
\n\
proc ns_simplex { n1 n2 bw delay type } {\n\
puts stderr \"ns_simplex: no backward compat\"\n\
exit 1\n\
}\n\
\n\
proc ns_duplex { n1 n2 bw delay type } {\n\
ns duplex-link-compat $n1 $n2 $bw $delay $type\n\
return [ns get-queues $n1 $n2]\n\
}\n\
\n\
proc ns_create_connection { srcType srcNode sinkType sinkNode pktClass } {\n\
ns create-connection $srcType $srcNode $sinkType \\\n\
$sinkNode $pktClass\n\
}\n\
\n\
proc ns_create_cbr { srcNode sinkNode pktSize interval fid } {\n\
set s [ns create-connection cbr $srcNode loss-monitor \\\n\
$sinkNode $fid]\n\
$s set interval_ $interval\n\
$s set packetSize_ $pktSize\n\
return $s\n\
}\n\
\n\
proc ns_create_class { parent borrow allot maxidle notused prio depth xdelay } {\n\
set cl [new CBQClass]\n\
if { $prio < 8 } {\n\
set qtype [CBQClass set def_qtype_]\n\
set q [new Queue/$qtype]\n\
$cl install-queue $q\n\
}\n\
set depth [expr $depth + 1]\n\
if { $borrow == \"none\" } {\n\
set borrowok false\n\
} elseif { $borrow == $parent } {\n\
set borrowok true\n\
} else {\n\
puts stderr \"CBQ: borrowing from non-parent not supported\"\n\
exit 1\n\
}\n\
\n\
$cl setparams $parent $borrowok $allot $maxidle $prio $depth $xdelay\n\
return $cl\n\
}\n\
\n\
proc ns_create_class1 { parent borrow allot maxidle notused prio depth xdelay Mb } {\n\
set cl [ns_create_class $parent $borrow $allot $maxidle $notused $prio $depth $xdelay]\n\
ns_class_maxIdle $cl $allot $maxidle $prio $Mb\n\
return $cl\n\
}\n\
\n\
proc ns_class_params { cl parent borrow allot maxidle notused prio depth xdelay Mb } {\n\
set depth [expr $depth + 1]\n\
if { $borrow == \"none\" } {\n\
set borrowok false\n\
} elseif { $borrow == $parent } {\n\
set borrowok true\n\
} else {\n\
puts stderr \"CBQ: borrowing from non-parent not supported\"\n\
exit 1\n\
}\n\
$cl setparams $parent $borrowok $allot $maxidle $prio $depth $xdelay\n\
ns_class_maxIdle $cl $allot $maxidle $prio $Mb\n\
return $cl\n\
}\n\
\n\
proc ns_class_maxIdle { cl allot maxIdle priority Mbps } {\n\
if { $maxIdle == \"auto\" } {\n\
set g 0.9375\n\
set n [expr 8 * $priority]\n\
set gTOn [expr pow($g, $n)]\n\
set first [expr ((1/$allot) - 1) * (1-$gTOn) / $gTOn ]\n\
set second [expr (1 - $g)]\n\
set packetsize 1000\n\
set t [expr ($packetsize * 8)/($Mbps * 1000000) ]\n\
if { $first > $second } {\n\
$cl set maxidle_ [expr $t * $first]\n\
} else {\n\
$cl set maxidle_ [expr $t * $second]\n\
}\n\
} else {\n\
$cl set maxidle_ $maxIdle\n\
}\n\
return $cl\n\
}\n\
Agent instproc connect d {\n\
$self set dst_ $d\n\
}\n\
\n\
Agent/Message instproc recv msg {\n\
$self handle $msg\n\
}\n\
\n\
Queue/RED proc set { var {arg \"\"} } {\n\
if { $var == \"queue-in-bytes_\" } {\n\
warn \"Warning: use `queue_in_bytes_' rather than `queue-in-bytes_'\"\n\
set var \"queue_in_bytes_\"\n\
} elseif { $var == \"drop-tail_\" } {\n\
warn \"Warning: use `drop_tail_' rather than `drop-tail_'\"\n\
set var \"drop_tail_\"\n\
} elseif { $var == \"drop-front_\" } {\n\
warn \"Warning: use `drop_front_' rather than `drop-front_'\"\n\
set var \"drop_front_\"\n\
} elseif { $var == \"drop-rand_\" } {\n\
warn \"Warning: use `drop_rand_' rather than `drop-rand_'\"\n\
set var \"drop_rand_\"\n\
} elseif { $var == \"ns1-compat_\" } {\n\
warn \"Warning: use `ns1_compat_' rather than `ns1-compat_'\"\n\
set var \"ns1_compat_\"\n\
}\n\
eval $self next $var $arg\n\
}\n\
\n\
Queue/DropTail proc set { var {arg \"\"} } {\n\
if { $var == \"drop-front_\" } {\n\
warn \"Warning: use `drop_front_' rather than `drop-front_'\"\n\
set var \"drop_front_\"\n\
}\n\
eval $self next $var $arg\n\
}\n\
\n\
\n\
PacketHeaderManager set hdrlen_ 0\n\
\n\
PacketHeaderManager set tab_(Common) 1\n\
\n\
proc add-packet-header args {\n\
foreach cl $args {\n\
PacketHeaderManager set tab_(PacketHeader/$cl) 1\n\
}\n\
}\n\
\n\
proc add-all-packet-headers {} {\n\
foreach cl [PacketHeader info subclass] {\n\
PacketHeaderManager set tab_($cl) 1\n\
}\n\
}\n\
\n\
proc remove-packet-header args {\n\
foreach cl $args {\n\
if { $cl == \"Common\" } {\n\
warn \"Cannot exclude common packet header.\"\n\
continue\n\
}\n\
PacketHeaderManager unset tab_(PacketHeader/$cl)\n\
}\n\
}\n\
\n\
proc remove-all-packet-headers {} {\n\
foreach cl [PacketHeader info subclass] {\n\
if { $cl != \"PacketHeader/Common\" } {\n\
PacketHeaderManager unset tab_($cl)\n\
}\n\
}\n\
}\n\
\n\
foreach prot {\n\
AODV\n\
ARP\n\
aSRM \n\
Common \n\
CtrMcast \n\
Diffusion\n\
Encap\n\
Flags\n\
IMEP\n\
IP\n\
IPinIP \n\
IVS\n\
LDP\n\
LL\n\
mcastCtrl\n\
MFTP\n\
MPLS\n\
Mac \n\
Message\n\
MIP \n\
Ping\n\
RAP \n\
RTP\n\
Resv \n\
rtProtoDV\n\
rtProtoLS\n\
SR\n\
SRM \n\
SRMEXT\n\
Snoop\n\
TCP\n\
TCPA\n\
TFRC\n\
TFRC_ACK\n\
TORA\n\
GAF\n\
BT\n\
L2CAP\n\
UMP \n\
PUSHBACK\n\
NV\n\
} {\n\
add-packet-header $prot\n\
}\n\
\n\
proc PktHdr_offset { hdrName {field \"\"} } {\n\
set offset [$hdrName offset]\n\
if { $field != \"\" } {\n\
incr offset [$hdrName set offset_($field)]\n\
}\n\
return $offset\n\
}\n\
\n\
Simulator instproc create_packetformat { } {\n\
PacketHeaderManager instvar tab_\n\
set pm [new PacketHeaderManager]\n\
foreach cl [PacketHeader info subclass] {\n\
if [info exists tab_($cl)] {\n\
set off [$pm allochdr $cl]\n\
$cl offset $off\n\
}\n\
}\n\
$self set packetManager_ $pm\n\
}\n\
\n\
PacketHeaderManager instproc allochdr cl {\n\
set size [$cl set hdrlen_]\n\
\n\
$self instvar hdrlen_\n\
set NS_ALIGN 8\n\
set incr [expr ($size + ($NS_ALIGN-1)) & ~($NS_ALIGN-1)]\n\
set base $hdrlen_\n\
incr hdrlen_ $incr\n\
\n\
return $base\n\
}\n\
\n\
\n\
\n\
\n\
\n\
\n\
\n\
\n\
\n\
\n\
Class CBQLink -superclass SimpleLink\n\
CBQLink instproc init { src dst bw delay q cl {lltype \"DelayLink\"} } {\n\
$self next $src $dst $bw $delay $q $lltype ; # SimpleLink ctor\n\
$self instvar head_ queue_ link_\n\
$self instvar  classifier_	; # not found in a SimpleLink\n\
\n\
$queue_ link $link_ ; # queue_ set by SimpleLink ctor, CBQ needs $link_\n\
set classifier_ $cl\n\
$head_ target $classifier_\n\
\n\
set defalg [Queue/CBQ set algorithm_]\n\
$queue_ set algorithm_ $defalg\n\
$queue_ algorithm $defalg\n\
}\n\
\n\
\n\
\n\
\n\
CBQLink instproc classifier {} {\n\
$self instvar classifier_\n\
return $classifier_\n\
}\n\
\n\
CBQLink instproc bind args {\n\
\n\
$self instvar classifier_\n\
set nargs [llength $args]\n\
set cbqcl [lindex $args 0]\n\
set a [lindex $args 1]\n\
if { $nargs == 3 } {\n\
set b [lindex $args 2]\n\
} else {\n\
set b $a\n\
}\n\
while { $a <= $b } {\n\
set slot [$classifier_ installNext $cbqcl]\n\
$classifier_ set-hash auto 0 0 $a $slot\n\
incr a\n\
}\n\
}\n\
\n\
CBQLink instproc insert args {\n\
$self instvar queue_ drophead_ link_\n\
set nargs [llength $args]\n\
set cbqcl [lindex $args 0]\n\
set qdisc [$cbqcl qdisc]\n\
if { $nargs == 1 } {\n\
set qmon [new QueueMonitor]\n\
} else {\n\
set qmon [lindex $args 1]\n\
}\n\
\n\
\n\
if { $qmon == \"\" } {\n\
error \"CBQ requires a q-monitor for class $cbqcl\"\n\
}\n\
if { $qdisc != \"\" } {\n\
set in [new SnoopQueue/In]\n\
set out [new SnoopQueue/Out]\n\
set drop [new SnoopQueue/Drop]\n\
$in set-monitor $qmon\n\
$out set-monitor $qmon\n\
$drop set-monitor $qmon\n\
\n\
$in target $qdisc\n\
$cbqcl target $in\n\
\n\
$qdisc drop-target $drop\n\
$drop target $drophead_\n\
\n\
$qdisc target $out\n\
$out target $queue_\n\
$cbqcl qmon $qmon\n\
}\n\
\n\
\n\
$cbqcl instvar maxidle_\n\
\n\
if { $maxidle_ == \"auto\" } {\n\
$cbqcl automaxidle [$link_ set bandwidth_] \\\n\
[$queue_ set maxpkt_]\n\
set maxidle_ [$cbqcl set maxidle_]\n\
}\n\
$cbqcl maxidle $maxidle_\n\
\n\
$queue_ insert-class $cbqcl\n\
}\n\
\n\
CBQClass instproc init {} {\n\
$self next\n\
$self instvar automaxidle_gain_\n\
set automaxidle_gain_ [$class set automaxidle_gain_]\n\
}\n\
\n\
CBQClass instproc automaxidle { linkbw maxpkt } {\n\
$self instvar automaxidle_gain_ maxidle_\n\
$self instvar priority_\n\
\n\
\n\
set allot [$self allot]\n\
\n\
\n\
set g $automaxidle_gain_\n\
set n [expr 8 * $priority_]\n\
\n\
if { $g == 0 || $allot == 0 || $linkbw == 0 } {\n\
set maxidle_ 0.0\n\
return\n\
}\n\
set gTOn [expr pow($g, $n)]\n\
set first [expr ((1/$allot) - 1) * (1-$gTOn) / $gTOn ]\n\
set second [expr (1 - $g)]\n\
set t [expr ($maxpkt * 8.0)/$linkbw]\n\
if { $first > $second } {\n\
set maxidle_ [expr $t * $first]\n\
} else {\n\
set maxidle_ [expr $t * $second]\n\
}\n\
return $maxidle_\n\
}\n\
\n\
\n\
CBQClass instproc setparams { parent okborrow allot maxidle prio level xdelay } {\n\
\n\
$self allot $allot\n\
$self parent $parent\n\
\n\
$self set okborrow_ $okborrow\n\
$self set maxidle_ $maxidle\n\
$self set priority_ $prio\n\
$self set level_ $level\n\
$self set extradelay_ $xdelay\n\
\n\
return $self\n\
}\n\
\n\
\n\
CBQClass instproc install-queue q {\n\
$q set blocked_ true\n\
$q set unblock_on_resume_ false\n\
$self qdisc $q\n\
}\n\
\n\
\n\
QueueMonitor instproc reset {} {\n\
$self instvar size_ pkts_\n\
$self instvar parrivals_ barrivals_\n\
$self instvar pdepartures_ bdepartures_\n\
$self instvar pdrops_ bdrops_\n\
\n\
set parrivals_ 0\n\
set barrivals_ 0\n\
set pdepartures_ 0\n\
set bdepartures_ 0\n\
set pdrops_ 0\n\
set bdrops_ 0\n\
\n\
set bint [$self get-bytes-integrator]\n\
if { $bint != \"\" } {\n\
$bint reset\n\
}\n\
\n\
set pint [$self get-pkts-integrator]\n\
if { $pint != \"\" } {\n\
$pint reset\n\
}\n\
\n\
set samp [$self get-delay-samples]\n\
if { $samp != \"\" } {\n\
$samp reset\n\
}\n\
}\n\
\n\
QueueMonitor/ED instproc reset {} {\n\
$self next\n\
$self instvar epdrops_ ebdrops_ mon_epdrops_ mon_ebdrops_\n\
set epdrops_ 0\n\
set ebdrops_ 0\n\
set mon_epdrops_ 0\n\
set mon_ebdrops_ 0\n\
}\n\
\n\
Class AckReconsClass -superclass Agent\n\
\n\
AckReconsControllerClass instproc demux { src dst } {\n\
$self instvar reconslist_ queue_\n\
set addr $src:$dst\n\
if { ![info exists reconslist_($addr)] } {\n\
set recons [new Agent/AckReconsClass $src $dst]\n\
$recons target $queue_\n\
set reconslist_($addr) $recons\n\
}\n\
return $reconslist_($addr)\n\
}\n\
\n\
\n\
\n\
Agent/AckReconsClass instproc spacing { ack } {\n\
$self instvar ackInterArr_ ackSpacing_ delack_ \\\n\
lastAck_ lastRealAck_ lastRealTime_ adaptive_ size_\n\
global ns\n\
\n\
set deltaTime [expr [$ns now] - $lastRealTime_]\n\
set deltaAck [expr $ack - $lastAck_]\n\
if {$adaptive_} {\n\
set bw [expr $deltaAck*$size_/$deltaTime]\n\
set ackSpacing_ $ackInterArr_\n\
if { $deltaAck > 0 } {\n\
}\n\
} else {\n\
set deltaT [expr $deltaTime / ($deltaAck/$delack_ +1)]\n\
set ackSpacing_ $deltaT\n\
}\n\
}\n\
\n\
Agent/AckReconsClass instproc ackbw {ack time} {\n\
$self instvar ackInterArr_ lastRealTime_ lastRealAck_ alpha_\n\
\n\
set sample [expr $time - $lastRealTime_]\n\
set ackInterArr_ [expr $alpha_*$sample + (1-$alpha_)*$ackInterArr_]\n\
}\n\
\n\
Class Classifier/Hash/Fid/FQ -superclass Classifier/Hash/Fid\n\
\n\
Classifier/Hash/Fid/FQ instproc unknown-flow { src dst fid } {\n\
$self instvar fq_\n\
$fq_ new-flow $src $dst $fid\n\
}\n\
\n\
Class FQLink -superclass SimpleLink\n\
\n\
FQLink instproc init { src dst bw delay q } {\n\
$self next $src $dst $bw $delay $q\n\
$self instvar link_ queue_ head_ toNode_ ttl_ classifier_ \\\n\
nactive_ \n\
$self instvar drophead_		;# idea stolen from CBQ and Kevin\n\
\n\
set nactive_ 0\n\
\n\
set classifier_ [new Classifier/Hash/Fid/FQ 33]\n\
$classifier_ set fq_ $self\n\
\n\
$head_ target $classifier_\n\
\n\
\n\
$queue_ set secsPerByte_ [expr 8.0 / [$link_ set bandwidth_]]\n\
}\n\
FQLink instproc new-flow { src dst fid } {\n\
$self instvar classifier_ nactive_ queue_ link_ drpT_\n\
incr nactive_\n\
\n\
set type [$class set queueManagement_]\n\
set q [new Queue/$type]\n\
\n\
if { $type == \"RED\" } {\n\
set bw [$link_ set bandwidth_]\n\
$q set ptc_ [expr $bw / (8. * [$q set mean_pktsize_])]\n\
}\n\
$q drop-target $drpT_\n\
\n\
set slot [$classifier_ installNext $q]\n\
$classifier_ set-hash auto $src $dst $fid $slot\n\
$q target $queue_\n\
$queue_ install $fid $q\n\
}\n\
FQLink instproc up? { } {\n\
return up\n\
}\n\
\n\
Queue/RED/PD instproc makeflowmon { link {cltype \"SrcDestFid\"} {cslots 29}} {\n\
\n\
set flowmon [new QueueMonitor/ED/Flowmon]\n\
set cl [new Classifier/Hash/$cltype $cslots]\n\
\n\
$cl proc unknown-flow { src dst fid } {\n\
set nflow [new QueueMonitor/ED/Flow/RedPD]\n\
set slot [$self installNext $nflow]\n\
$self set-hash auto $src $dst $fid $slot\n\
}\n\
\n\
$cl proc no-slot slotnum {\n\
puts stderr \"classifier $self, no-slot for slotnum $slotnum\"\n\
}\n\
\n\
$flowmon classifier $cl\n\
$self attach-flowmon $flowmon\n\
\n\
set isnoop [new SnoopQueue/In]\n\
set osnoop [new SnoopQueue/Out]\n\
set dsnoop [new SnoopQueue/Drop]\n\
set edsnoop [new SnoopQueue/EDrop]\n\
\n\
$link attach-monitors $isnoop $osnoop $dsnoop $flowmon\n\
$edsnoop set-monitor $flowmon\n\
$self early-drop-target $edsnoop \n\
set ns [Simulator instance]\n\
$edsnoop target [$ns set nullAgent_]\n\
\n\
\n\
$self drop-target $dsnoop\n\
\n\
return $flowmon\n\
\n\
}	\n\
\n\
\n\
Queue instproc attach-nam-traces {src dst file} {\n\
\n\
\n\
$self attach-traces $src $dst $file \"nam\"\n\
}\n\
\n\
Queue instproc attach-traces {src dst file {op \"\"}} {\n\
}\n\
\n\
Queue/RED instproc attach-traces {src dst file {op \"\"}} {\n\
\n\
set ns [Simulator instance]\n\
set type [$self trace-type]\n\
\n\
if {$op == \"nam\"} {\n\
set type \"Drop\"\n\
}\n\
\n\
\n\
set newtrace [$ns create-trace $type $file $src $dst $op]\n\
set oldTrace [$self edrop-trace]\n\
\n\
if {$oldTrace!=0} {\n\
$newtrace target $oldTrace\n\
} else {\n\
$newtrace target [$ns set nullAgent_]\n\
}\n\
\n\
$self edrop-trace $newtrace\n\
}\n\
\n\
Queue/RED/PD instproc attach-traces {src dst file {op \"\"}} {\n\
\n\
$self next $src $dst $file $op\n\
\n\
set ns [Simulator instance]\n\
set type [$self mon-trace-type]\n\
\n\
if {$op == \"nam\"} {\n\
set type \"Drop\"\n\
}\n\
\n\
set medtrace [$ns create-trace $type $file $src $dst $op]\n\
\n\
set oldTrace [$self mon-edrop-trace]\n\
if {$oldTrace!=0} {\n\
puts \"exists\"\n\
$medtrace target $oldTrace\n\
} else {\n\
$medtrace target [$ns set nullAgent_]\n\
}\n\
\n\
$self mon-edrop-trace $medtrace\n\
\n\
}\n\
\n\
\n\
Trace instproc init type {\n\
$self next $type\n\
$self instvar type_\n\
set type_ $type\n\
}\n\
\n\
Trace instproc format args {\n\
\n\
$self instvar type_ fp_ src_ dst_\n\
\n\
if [info exists fp_] {\n\
set ns [Simulator instance]\n\
puts $fp_ [eval list $type_ [$ns now] [eval concat $args]]\n\
}\n\
}\n\
\n\
Trace instproc attach fp {\n\
$self instvar fp_\n\
\n\
set fp_ $fp\n\
$self cmd attach $fp_\n\
}\n\
\n\
\n\
BaseTrace instproc attach fp {\n\
$self instvar fp_\n\
\n\
set fp_ $fp\n\
$self cmd attach $fp_\n\
}\n\
\n\
Class Trace/Hop -superclass Trace\n\
Trace/Hop instproc init {} {\n\
$self next \"h\"\n\
}\n\
\n\
Class Trace/Enque -superclass Trace\n\
Trace/Enque instproc init {} {\n\
$self next \"+\"\n\
}\n\
\n\
Trace/Deque instproc init {} {\n\
$self next \"-\"\n\
}\n\
\n\
Class Trace/EDrop -superclass Trace\n\
Trace/EDrop instproc init {} {\n\
$self next \"e\"\n\
}\n\
\n\
Class Trace/MEDrop -superclass Trace\n\
Trace/MEDrop instproc init {} {\n\
$self next \"m\"\n\
}\n\
\n\
\n\
Class Trace/SessEnque -superclass Trace\n\
Trace/SessEnque instproc init {} {\n\
$self next \"E\"	;# Should use '='? :)\n\
}\n\
\n\
Class Trace/SessDeque -superclass Trace\n\
Trace/SessDeque instproc init {} {\n\
$self next \"D\"	;# Should use '_'?\n\
}\n\
\n\
Class Trace/Recv -superclass Trace \n\
Trace/Recv instproc init {} {\n\
$self next \"r\"\n\
}\n\
\n\
Class Trace/Drop -superclass Trace\n\
Trace/Drop instproc init {} {\n\
$self next \"d\"\n\
}\n\
\n\
Class Trace/Generic -superclass Trace\n\
Trace/Generic instproc init {} {\n\
$self next \"v\"\n\
}\n\
\n\
Class Trace/Collision -superclass Trace\n\
Trace/Collision instproc init {} {\n\
$self next \"c\"\n\
}\n\
\n\
Class Trace/Var -superclass Trace\n\
Trace/Var instproc init {} {\n\
$self next \"f\"\n\
}\n\
\n\
proc f-time t {\n\
format \"%7.4f\" $t\n\
}\n\
\n\
proc f-node n {\n\
set node [expr $n >> 8]\n\
set port [expr $n & 0xff]\n\
return \"$node.$port\"\n\
}\n\
\n\
proc gc o {\n\
set ret \"NULL_OBJECT\"\n\
if { $o != \"\" } {\n\
set ret \"\"\n\
foreach i $o {\n\
if ![catch \"$i info class\" val] {\n\
lappend ret $val\n\
}\n\
}\n\
}\n\
set ret\n\
}\n\
\n\
Node instproc tn {} {\n\
$self instvar id_\n\
return \"${self}(id $id_)\"\n\
}\n\
\n\
Simulator instproc gen-map {} {\n\
\n\
$self instvar Node_ link_ MobileNode_\n\
\n\
set nn [Node set nn_]\n\
for {set i 0} {$i < $nn} {incr i} {\n\
if ![info exists Node_($i)] {\n\
continue\n\
}\n\
set n $Node_($i)\n\
puts \"Node [$n tn]\"\n\
foreach nc [$n info vars] {\n\
switch $nc {\n\
ns_		continue\n\
id_		continue\n\
neighbor_	continue\n\
agents_		continue\n\
routes_		continue\n\
np_		continue\n\
default {\n\
if [$n array exists $nc] {\n\
puts \"\\t\\t$nc\\t[$n array get $nc]\"\n\
} else {\n\
set v [$n set $nc]\n\
puts \"\\t\\t$nc${v}([gc $v])\"\n\
}\n\
}\n\
}\n\
}\n\
if {[llength [$n set agents_]] > 0} {\n\
puts \"\\n\\tAgents at node (possibly in order of creation):\"\n\
foreach a [$n set agents_] {\n\
puts \"\\t\\t$a\\t[gc $a]\\t\\tdst-addr/port: [$a set dst_addr_]/[$a set dst_port_]\"\n\
}\n\
}\n\
puts \"\"\n\
foreach li [array names link_ [$n id]:*] {\n\
set L [split $li :]\n\
set nbr [[$self get-node-by-id [lindex $L 1]] entry]\n\
set ln $link_($li)\n\
puts \"\\tLink $ln, fromNode_ [[$ln set fromNode_] tn] -> toNode_ [[$ln set toNode_] tn]\"\n\
puts \"\\tComponents (in order) head first\"\n\
for {set c [$ln head]} {$c != $nbr} {set c [$c target]} {\n\
puts \"\\t\\t$c\\t[gc $c]\"\n\
}\n\
}\n\
puts \"---\"\n\
}\n\
}\n\
\n\
\n\
\n\
Simulator instproc maybeEnableTraceAll {obj args} {\n\
foreach {file tag} {\n\
traceAllFile_           {}\n\
namtraceAllFile_        nam\n\
} {\n\
$self instvar $file\n\
if [info exists $file] {\n\
$obj trace [set $file] $args $tag\n\
}\n\
}\n\
}\n\
\n\
proc exponential {args} {\n\
global defaultRNG\n\
eval [list $defaultRNG exponential] $args\n\
}\n\
\n\
proc uniform {args} {\n\
global defaultRNG\n\
eval [list $defaultRNG uniform] $args\n\
}\n\
\n\
proc integer {args} {\n\
global defaultRNG\n\
eval [list $defaultRNG integer] $args\n\
}\n\
\n\
RNG instproc init {} {\n\
$self next\n\
$self instvar z2\n\
set z2 0\n\
}\n\
\n\
RNG instproc uniform {a b} {\n\
expr $a + (($b - $a) * ([$self next-random] * 1.0 / 0x7fffffff))\n\
}\n\
\n\
RNG instproc integer k {\n\
expr [$self next-random] % abs($k)\n\
}\n\
\n\
RNG instproc exponential {{mu 1.0}} {\n\
expr - $mu * log(([$self next-random] + 1.0) / (0x7fffffff + 1.0))\n\
}\n\
\n\
\n\
\n\
RandomVariable instproc test count {\n\
for {set i 0} {$i < $count} {incr i} {\n\
puts stdout [$self value]\n\
}\n\
}\n\
\n\
\n\
set defaultRNG [new RNG]\n\
$defaultRNG seed 1\n\
$defaultRNG default\n\
trace variable defaultRNG w { abort \"cannot update defaultRNG once assigned\"; }\n\
\n\
\n\
Class RandomVariable/TraceDriven -superclass RandomVariable\n\
\n\
RandomVariable/TraceDriven instproc init {} {\n\
$self instvar filename_ file_\n\
}\n\
\n\
RandomVariable/TraceDriven instproc value {} {\n\
$self instvar file_ filename_\n\
\n\
if ![info exist file_] {\n\
if [info exist filename_] {\n\
set file_ [open $filename_ r]\n\
} else {\n\
puts \"RandomVariable/TraceDriven: Filename is not given\"\n\
exit 0\n\
}\n\
}\n\
\n\
if ![eof $file_] {\n\
gets $file_ tmp\n\
return $tmp\n\
} else {\n\
close $file_\n\
puts \"Error: RandomVariable/TraceDriven: Reached the end of the trace fi\n\
le \"\n\
exit 0\n\
}\n\
}\n\
\n\
\n\
Agent instproc set args {\n\
if { [lindex $args 0] == \"dst_\" } {\n\
puts \"Warning dst_ is no longer being supported in NS. $args\"\n\
puts \"Use dst_addr_ and dst_port_ instead\"\n\
$self instvar dst_addr_ dst_port_\n\
set addr [lindex $args 1]\n\
set baseAddr [Simulator set McastBaseAddr_]\n\
if { $addr >= $baseAddr } {\n\
$self set dst_addr_ $addr\n\
$self set dst_port_ 0\n\
} else {\n\
$self set dst_addr_ [expr ($addr >> 8) ]\n\
$self set dst_port_ [expr ($addr % 256) ]\n\
exit\n\
}\n\
return\n\
}\n\
eval $self next $args\n\
}\n\
\n\
\n\
Agent instproc init {} {\n\
}\n\
\n\
Agent instproc nodeid {} { \n\
[$self set node_] id\n\
}\n\
\n\
Agent instproc port {} {\n\
$self instvar agent_port_\n\
return $agent_port_\n\
}\n\
\n\
Agent instproc dst-port {} {\n\
$self instvar dst_port_\n\
return [expr $dst_port_]\n\
}\n\
\n\
Agent instproc attach-source {s_type} {\n\
set source [new Source/$s_type]\n\
$source attach $self\n\
$self set type_ $s_type\n\
return $source\n\
}\n\
\n\
Agent instproc attach-app {s_type} {\n\
set app_ [new Application/$s_type]\n\
$app_ attach-agent $self\n\
$self set type_ $s_type\n\
return $app_\n\
}\n\
\n\
Agent instproc attach-tbf { tbf } {\n\
$tbf target [$self target]\n\
$self target $tbf\n\
\n\
}\n\
\n\
\n\
Class Agent/Null -superclass Agent\n\
\n\
Agent/Null instproc init args {\n\
eval $self next $args\n\
}\n\
\n\
Agent/LossMonitor instproc log-loss {} {\n\
}\n\
\n\
Agent/CBR/UDP/SA instproc attach-tbf { tbf } {\n\
$tbf target [$self target]\n\
$self target $tbf\n\
$self ctrl-target [$tbf target]\n\
}\n\
\n\
\n\
Agent proc set-maxttl {objectOrClass var} {\n\
if { [catch \"$objectOrClass set $var\" value] ||	\\\n\
($value < [Agent set ttl_]) } {\n\
$objectOrClass set $var [Agent set ttl_]\n\
}\n\
$objectOrClass set $var\n\
}\n\
\n\
\n\
\n\
Agent/TCP instproc init {} {\n\
eval $self next\n\
set ns [Simulator instance]\n\
$ns create-eventtrace Event $self\n\
}\n\
\n\
\n\
\n\
Agent/TCP/FullTcp/Tahoe instproc init {} {\n\
$self next\n\
$self instvar reno_fastrecov_\n\
set reno_fastrecov_ false\n\
}\n\
\n\
Agent/TCP/FullTcp/Sack instproc init {} {\n\
$self next\n\
$self instvar reno_fastrecov_ maxburst_ open_cwnd_on_pack_\n\
set reno_fastrecov_ false\n\
set maxburst_ 5\n\
set open_cwnd_on_pack_ false\n\
}\n\
\n\
Agent/TCP/FullTcp/Newreno instproc init {} {\n\
$self next\n\
$self instvar open_cwnd_on_pack_\n\
set open_cwnd_on_pack_ false\n\
}\n\
\n\
\n\
Agent/TORA instproc init args {\n\
\n\
$self next $args\n\
}       \n\
\n\
Agent/TORA set sport_	0\n\
Agent/TORA set dport_	0\n\
\n\
Agent/AODV instproc init args {\n\
\n\
$self next $args\n\
}\n\
\n\
Agent/AODV set sport_   0\n\
Agent/AODV set dport_   0\n\
\n\
\n\
\n\
\n\
\n\
\n\
\n\
\n\
\n\
\n\
RouteLogic instproc register {proto args} {\n\
$self instvar rtprotos_ node_rtprotos_ default_node_rtprotos_\n\
if [info exists rtprotos_($proto)] {\n\
eval lappend rtprotos_($proto) $args\n\
} else {\n\
set rtprotos_($proto) $args\n\
}\n\
if { [Agent/rtProto/$proto info procs pre-init-all] != \"\" } {\n\
Agent/rtProto/$proto pre-init-all $args\n\
}\n\
}\n\
\n\
RouteLogic instproc configure {} {\n\
$self instvar rtprotos_\n\
if [info exists rtprotos_] {\n\
foreach proto [array names rtprotos_] {\n\
eval Agent/rtProto/$proto init-all $rtprotos_($proto)\n\
}\n\
} else {\n\
Agent/rtProto/Static init-all\n\
}\n\
}\n\
\n\
RouteLogic instproc lookup { nodeid destid } {\n\
if { $nodeid == $destid } {\n\
return $nodeid\n\
}\n\
\n\
set ns [Simulator instance]\n\
set node [$ns get-node-by-id $nodeid]\n\
\n\
if [Simulator hier-addr?] {\n\
set dest [$ns get-node-by-id $destid]\n\
set nh [$self hier-lookup [$node node-addr] [$dest node-addr]]\n\
return [$ns get-node-id-by-addr $nh]\n\
}\n\
set rtobj [$node rtObject?]\n\
if { $rtobj != \"\" } {\n\
$rtobj lookup [$ns get-node-by-id $destid]\n\
} else {\n\
$self cmd lookup $nodeid $destid\n\
} \n\
}\n\
\n\
RouteLogic instproc notify {} {\n\
$self instvar rtprotos_\n\
foreach i [array names rtprotos_] {\n\
Agent/rtProto/$i compute-all\n\
}\n\
\n\
foreach i [CtrMcastComp info instances] {\n\
$i notify\n\
}\n\
if { [rtObject info instances] == \"\"} {\n\
foreach node [[Simulator instance] all-nodes-list] {\n\
$node notify-mcast 0\n\
}\n\
}\n\
}\n\
\n\
RouteLogic instproc append-addr {level addrstr} {\n\
if {$level != 0} {\n\
set str [lindex $addrstr 0]\n\
for {set i 1} {$i < $level} {incr i} {\n\
append str . [lindex $addrstr [expr $i]]\n\
}\n\
return $str\n\
}\n\
}\n\
\n\
RouteLogic instproc dump nn {\n\
set i 0\n\
while { $i < $nn } {\n\
set j 0\n\
while { $j < $nn } {\n\
puts \"$i -> $j via [$self lookup $i $j]\"\n\
incr j\n\
}\n\
incr i\n\
}\n\
}\n\
\n\
Simulator instproc rtproto {proto args} {\n\
$self instvar routingTable_\n\
if {$proto == \"Algorithmic\"} {\n\
set routingTable_ [new RouteLogic/Algorithmic]\n\
}\n\
eval [$self get-routelogic] register $proto $args\n\
}\n\
\n\
Simulator instproc get-routelogic {} {\n\
$self instvar routingTable_\n\
if ![info exists routingTable_] {\n\
set routingTable_ [new RouteLogic]\n\
}\n\
return $routingTable_\n\
}\n\
\n\
Simulator instproc dump-approx-sim-data {} {\n\
\n\
$self instvar routingTable_ Node_ link_\n\
if ![info exists routingTable_] {\n\
puts \"error: routing table is not computed yet!\"\n\
return 0\n\
}\n\
\n\
puts \"# Dumping Approx-Sim Data\"  \n\
\n\
set n [Node set nn_]\n\
puts \"m [Link set nl_] \"\n\
foreach qn [array names link_] {\n\
set l $link_($qn)\n\
puts \"link [expr [$l set id_] + 1] [expr [$l bw] / 8000] [expr [$l bw] / 8000] [$l delay] [$l qsize]\"\n\
}\n\
\n\
puts \"\"\n\
global nconn_ conn_\n\
puts \"n $nconn_\"\n\
\n\
for { set i 0 } { $i < $nconn_ } { incr i } {\n\
\n\
set len 0\n\
set str \"\"\n\
\n\
set list [split $conn_($i) \":\"] \n\
set srcid [lindex $list 0]\n\
set dstid [lindex $list 1]\n\
\n\
while { $srcid != $dstid } {\n\
incr len\n\
set nh [$routingTable_ lookup $srcid $dstid]\n\
append str \" \" [$link_($srcid:$nh) id] \n\
set srcid  $nh\n\
}\n\
\n\
puts \"route [expr $i + 1] $len $str\"\n\
\n\
}\n\
}\n\
\n\
Simulator instproc dump-routelogic-nh {} {\n\
$self instvar routingTable_ Node_ link_\n\
if ![info exists routingTable_] {\n\
puts \"error: routing table is not computed yet!\"\n\
return 0\n\
}\n\
\n\
puts \"Dumping Routing Table: Next Hop Information\"\n\
set n [Node set nn_]\n\
set i 0\n\
puts -nonewline \"\\t\"\n\
while { $i < $n } {\n\
if ![info exists Node_($i)] {\n\
incr i\n\
continue\n\
}\n\
puts -nonewline \"$i\\t\"\n\
incr i\n\
}\n\
set i 0\n\
while { $i < $n } {\n\
if ![info exists Node_($i)] {\n\
incr i\n\
continue\n\
}\n\
puts -nonewline \"\\n$i\\t\"\n\
set n1 $Node_($i)\n\
set j 0\n\
while { $j < $n } {\n\
if { $i != $j } {\n\
set nh [$routingTable_ lookup $i $j]\n\
if { $nh >= 0 } {\n\
puts -nonewline \"$nh\\t\"\n\
}\n\
} else {\n\
puts -nonewline \"--\\t\"\n\
}\n\
incr j\n\
}\n\
incr i\n\
}\n\
puts \"\"\n\
}\n\
\n\
Simulator instproc dump-routelogic-distance {} {\n\
$self instvar routingTable_ Node_ link_\n\
if ![info exists routingTable_] {\n\
puts \"error: routing table is not computed yet!\"\n\
return 0\n\
}\n\
\n\
set n [Node set nn_]\n\
set i 0\n\
puts -nonewline \"\\t\"\n\
while { $i < $n } {\n\
if ![info exists Node_($i)] {\n\
incr i\n\
continue\n\
}\n\
puts -nonewline \"$i\\t\"\n\
incr i\n\
}\n\
\n\
for {set i 0} {$i < $n} {incr i} {\n\
if ![info exists Node_($i)] {\n\
continue\n\
}\n\
puts -nonewline \"\\n$i\\t\"\n\
set n1 $Node_($i)\n\
for {set j 0} {$j < $n} {incr j} {\n\
if { $i == $j } {\n\
puts -nonewline \"0\\t\"\n\
continue\n\
}\n\
set nh [$routingTable_ lookup $i $j]\n\
if { $nh < 0 } {\n\
puts -nonewline \"0\\t\"\n\
continue\n\
}\n\
set distance 0\n\
set tmpfrom $i\n\
set tmpto $j\n\
while {$tmpfrom != $tmpto} {\n\
set tmpnext [$routingTable_ lookup \\\n\
$tmpfrom $tmpto]\n\
set distance [expr $distance + \\\n\
[$link_($tmpfrom:$tmpnext) cost?]]\n\
set tmpfrom $tmpnext\n\
}\n\
puts -nonewline \"$distance\\t\"\n\
}\n\
}\n\
puts \"\"\n\
}\n\
\n\
Simulator instproc compute-routes {} {\n\
if [Simulator hier-addr?] {\n\
$self compute-hier-routes \n\
} else {\n\
$self compute-flat-routes\n\
}\n\
}\n\
\n\
Simulator instproc compute-flat-routes {} {\n\
$self instvar Node_ link_\n\
if { [ Simulator set nix-routing] } {\n\
puts \"Using NixVector routing, skipping route computations\"\n\
return\n\
}\n\
\n\
set r [$self get-routelogic]\n\
$self cmd get-routelogic $r  ;# propagate rl in C++\n\
\n\
foreach ln [array names link_] {\n\
set L [split $ln :]\n\
set srcID [lindex $L 0]\n\
set dstID [lindex $L 1]\n\
if { [$link_($ln) up?] == \"up\" } {\n\
$r insert $srcID $dstID [$link_($ln) cost?]\n\
} else {\n\
$r reset $srcID $dstID\n\
}\n\
}\n\
\n\
$r compute\n\
\n\
set n [Node set nn_]\n\
\n\
\n\
$self populate-flat-classifiers $n\n\
\n\
\n\
\n\
}\n\
\n\
Simulator instproc get-link-head { n1 n2 } {\n\
$self instvar link_\n\
return [$link_($n1:$n2) head]\n\
}\n\
\n\
\n\
\n\
Simulator instproc hier-topo {rl} {\n\
AddrParams instvar domain_num_ cluster_num_ nodes_num_ \n\
\n\
if ![info exists cluster_num_] {\n\
if {[AddrParams hlevel] > 1} {\n\
set def [AddrParams set def_clusters]\n\
puts \"Default value for cluster_num set to $def\\n\"\n\
for {set i 0} {$i < $domain_num_} {incr i} {\n\
lappend clusters $def\n\
}\n\
} else {\n\
puts stderr \"hierarchy level = 1; should use flat-rtg instead of hier-rtg\" \n\
exit 1\n\
}\n\
AddrParams set cluster_num_ $clusters\n\
}\n\
\n\
if ![info exists nodes_num_ ] {\n\
set total_node 0\n\
set def [AddrParams set def_nodes]\n\
puts \"Default value for nodes_num set to $def\\n\"\n\
for {set i 0} {$i < $domain_num_} {incr i} {\n\
set total_node [expr $total_node + \\\n\
[lindex $clusters $i]]\n\
}\n\
for {set i 0} {$i < $total_node} {incr i} {\n\
lappend nodes $def\n\
}\n\
AddrParams set nodes_num_ $nodes\n\
}\n\
eval $rl send-num-of-domains $domain_num_\n\
eval $rl send-num-of-clusters $cluster_num_\n\
eval $rl send-num-of-nodes $nodes_num_\n\
}\n\
\n\
Simulator instproc compute-hier-routes {} {\n\
$self instvar Node_ link_\n\
set r [$self get-routelogic]\n\
$self cmd get-routelogic $r ;# propagate rl in C++    \n\
\n\
if ![info exists link_] {\n\
return\n\
}\n\
set level [AddrParams hlevel]\n\
$r hlevel-is $level\n\
$self hier-topo $r\n\
foreach ln [array names link_] {\n\
set L [split $ln :]\n\
set srcID [[$self get-node-by-id [lindex $L 0]] node-addr]\n\
set dstID [[$self get-node-by-id [lindex $L 1]] node-addr]\n\
if { [$link_($ln) up?] == \"up\" } {\n\
$r hier-insert $srcID $dstID [$link_($ln) cost?]\n\
} else {\n\
$r hier-reset $srcID $dstID\n\
}\n\
}\n\
\n\
$r hier-compute\n\
\n\
\n\
\n\
set n [Node set nn_]\n\
$self populate-hier-classifiers $n\n\
\n\
\n\
\n\
}\n\
\n\
\n\
\n\
set rtglibRNG [new RNG]\n\
$rtglibRNG seed 1\n\
\n\
Class rtObject\n\
\n\
rtObject set unreach_ -1\n\
rtObject set maxpref_   255\n\
\n\
rtObject proc init-all args {\n\
foreach node $args {\n\
if { [$node rtObject?] == \"\" } {\n\
set rtobj($node) [new rtObject $node]\n\
}\n\
}\n\
foreach node $args {	;# XXX\n\
$rtobj($node) compute-routes\n\
}\n\
}\n\
\n\
rtObject instproc init node {\n\
$self next\n\
$self instvar ns_ nullAgent_\n\
$self instvar nextHop_ rtpref_ metric_ node_ rtVia_ rtProtos_\n\
\n\
set ns_ [Simulator instance]\n\
set nullAgent_ [$ns_ set nullAgent_]\n\
\n\
$node init-routing $self\n\
set node_ $node\n\
foreach dest [$ns_ all-nodes-list] {\n\
set nextHop_($dest) \"\"\n\
if {$node == $dest} {\n\
set rtpref_($dest) 0\n\
set metric_($dest) 0\n\
set rtVia_($dest) \"Agent/rtProto/Local\" ;# make dump happy\n\
} else {\n\
set rtpref_($dest) [$class set maxpref_]\n\
set metric_($dest) [$class set unreach_]\n\
set rtVia_($dest)    \"\"\n\
$node add-route [$dest id] $nullAgent_\n\
}\n\
}\n\
$self add-proto Direct $node\n\
$rtProtos_(Direct) compute-routes\n\
}\n\
\n\
rtObject instproc add-proto {proto node} {\n\
$self instvar ns_ rtProtos_\n\
set rtProtos_($proto) [new Agent/rtProto/$proto $node]\n\
$ns_ attach-agent $node $rtProtos_($proto)\n\
set rtProtos_($proto)\n\
}\n\
\n\
rtObject instproc lookup dest {\n\
$self instvar nextHop_ node_\n\
if {![info exists nextHop_($dest)] || $nextHop_($dest) == \"\"} {\n\
return -1\n\
} else {\n\
return [[$nextHop_($dest) set toNode_] id]\n\
}\n\
}\n\
\n\
rtObject instproc compute-routes {} {\n\
$self instvar ns_ node_ rtProtos_ nullAgent_\n\
$self instvar nextHop_ rtpref_ metric_ rtVia_\n\
set protos \"\"\n\
set changes 0\n\
foreach p [array names rtProtos_] {\n\
if [$rtProtos_($p) set rtsChanged_] {\n\
incr changes\n\
$rtProtos_($p) set rtsChanged_ 0\n\
}\n\
lappend protos $rtProtos_($p)\n\
}\n\
if !$changes return\n\
\n\
set changes 0\n\
foreach dst [$ns_ all-nodes-list] {\n\
if {$dst == $node_} continue\n\
set nh \"\"\n\
set pf [$class set maxpref_]\n\
set mt [$class set unreach_]\n\
set rv \"\"\n\
foreach p $protos {\n\
set pnh [$p set nextHop_($dst)]\n\
if { $pnh == \"\" } continue\n\
\n\
set ppf [$p set rtpref_($dst)]\n\
set pmt [$p set metric_($dst)]\n\
if {$ppf < $pf || ($ppf == $pf && $pmt < $mt) || $mt < 0} {\n\
set nh  $pnh\n\
set pf  $ppf\n\
set mt  $pmt\n\
set rv  $p\n\
}\n\
}\n\
if { $nh == \"\" } {\n\
if { $nextHop_($dst) != \"\" } {\n\
$node_ delete-routes [$dst id] $nextHop_($dst) $nullAgent_\n\
set nextHop_($dst) $nh\n\
set rtpref_($dst)  $pf\n\
set metric_($dst)  $mt\n\
set rtVia_($dst)   $rv\n\
incr changes\n\
}\n\
} else {\n\
if { $rv == $rtVia_($dst) } {\n\
if { $nh != $nextHop_($dst) } {\n\
$node_ delete-routes [$dst id] $nextHop_($dst) $nullAgent_\n\
set nextHop_($dst) $nh\n\
$node_ add-routes [$dst id] $nextHop_($dst)\n\
incr changes\n\
}\n\
if { $mt != $metric_($dst) } {\n\
set metric_($dst) $mt\n\
incr changes\n\
}\n\
if { $pf != $rtpref_($dst) } {\n\
set rtpref_($dst) $pf\n\
}\n\
} else {\n\
if { $rtVia_($dst) != \"\" } {\n\
set nextHop_($dst) [$rtVia_($dst) set nextHop_($dst)]\n\
set rtpref_($dst)  [$rtVia_($dst) set rtpref_($dst)]\n\
set metric_($dst)  [$rtVia_($dst) set metric_($dst)]\n\
}\n\
if {$rtpref_($dst) != $pf || $metric_($dst) != $mt} {\n\
$node_ delete-routes [$dst id] $nextHop_($dst) $nullAgent_\n\
set nextHop_($dst) $nh\n\
set rtpref_($dst)  $pf\n\
set metric_($dst)  $mt\n\
set rtVia_($dst)   $rv\n\
$node_ add-routes [$dst id] $nextHop_($dst)\n\
incr changes\n\
}\n\
}\n\
}\n\
}\n\
foreach proto [array names rtProtos_] {\n\
$rtProtos_($proto) send-updates $changes\n\
}\n\
$self flag-multicast $changes\n\
}\n\
\n\
rtObject instproc flag-multicast changes {\n\
$self instvar node_\n\
$node_ notify-mcast $changes\n\
}\n\
\n\
rtObject instproc intf-changed {} {\n\
$self instvar ns_ node_ rtProtos_ rtVia_ nextHop_ rtpref_ metric_\n\
foreach p [array names rtProtos_] {\n\
$rtProtos_($p) intf-changed\n\
$rtProtos_($p) compute-routes\n\
}\n\
$self compute-routes\n\
}\n\
\n\
rtObject instproc dump-routes chan {\n\
$self instvar ns_ node_ nextHop_ rtpref_ metric_ rtVia_\n\
\n\
if {$ns_ != \"\"} {\n\
set time [$ns_ now]\n\
} else {\n\
set time 0.0\n\
}\n\
puts $chan [concat \"Node:\\t${node_}([$node_ id])\\tat t =\"		\\\n\
[format \"%4.2f\" $time]]\n\
puts $chan \"  Dest\\t\\t nextHop\\tPref\\tMetric\\tProto\"\n\
foreach dest [$ns_ all-nodes-list] {\n\
if {[llength $nextHop_($dest)] > 1} {\n\
set p [split [$rtVia_($dest) info class] /]\n\
set proto [lindex $p [expr [llength $p] - 1]]\n\
foreach rt $nextHop_($dest) {\n\
puts $chan [format \"%-5s(%d)\\t%-5s(%d)\\t%3d\\t%4d\\t %s\"	 \\\n\
$dest [$dest id] $rt [[$rt set toNode_] id]	 \\\n\
$rtpref_($dest) $metric_($dest) $proto]\n\
}\n\
} elseif {$nextHop_($dest) != \"\"} {\n\
set p [split [$rtVia_($dest) info class] /]\n\
set proto [lindex $p [expr [llength $p] - 1]]\n\
puts $chan [format \"%-5s(%d)\\t%-5s(%d)\\t%3d\\t%4d\\t %s\"	 \\\n\
$dest [$dest id]					 \\\n\
$nextHop_($dest) [[$nextHop_($dest) set toNode_] id] \\\n\
$rtpref_($dest) $metric_($dest) $proto]\n\
} elseif {$dest == $node_} {\n\
puts $chan [format \"%-5s(%d)\\t%-5s(%d)\\t%03d\\t%4d\\t %s\"	\\\n\
$dest [$dest id] $dest [$dest id] 0 0 \"Local\"]\n\
} else {\n\
puts $chan [format \"%-5s(%d)\\t%-5s(%s)\\t%03d\\t%4d\\t %s\"	\\\n\
$dest [$dest id] \"\" \"-\" 255 32 \"Unknown\"]\n\
}\n\
}\n\
}\n\
\n\
rtObject instproc rtProto? proto {\n\
$self instvar rtProtos_\n\
if [info exists rtProtos_($proto)] {\n\
return $rtProtos_($proto)\n\
} else {\n\
return \"\"\n\
}\n\
}\n\
\n\
rtObject instproc nextHop? dest {\n\
$self instvar nextHop_\n\
$self set nextHop_($dest)\n\
}\n\
\n\
rtObject instproc rtpref? dest {\n\
$self instvar rtpref_\n\
$self set rtpref_($dest)\n\
}\n\
\n\
rtObject instproc metric? dest {\n\
$self instvar metric_\n\
$self set metric_($dest)\n\
}\n\
\n\
Class rtPeer\n\
\n\
rtPeer instproc init {addr port cls} {\n\
$self next\n\
$self instvar addr_ port_ metric_ rtpref_\n\
set addr_ $addr\n\
set port_ $port\n\
foreach dest [[Simulator instance] all-nodes-list] {\n\
set metric_($dest) [$cls set INFINITY]\n\
set rtpref_($dest) [$cls set preference_]\n\
}\n\
}\n\
\n\
rtPeer instproc addr? {} {\n\
$self instvar addr_\n\
return $addr_\n\
}\n\
\n\
rtPeer instproc port? {} {\n\
$self instvar port_\n\
return $port_\n\
}\n\
\n\
rtPeer instproc metric {dest val} {\n\
$self instvar metric_\n\
set metric_($dest) $val\n\
}\n\
\n\
rtPeer instproc metric? dest {\n\
$self instvar metric_\n\
return $metric_($dest)\n\
}\n\
\n\
rtPeer instproc preference {dest val} {\n\
$self instvar rtpref_\n\
set rtpref_($dest) $val\n\
}\n\
\n\
rtPeer instproc preference? dest {\n\
$self instvar rtpref_\n\
return $rtpref_($dest)\n\
}\n\
\n\
\n\
Agent/rtProto proc pre-init-all args {\n\
}\n\
\n\
Agent/rtProto proc init-all args {\n\
error \"No initialization defined\"\n\
}\n\
\n\
Agent/rtProto instproc init node {\n\
$self next\n\
\n\
$self instvar ns_ node_ rtObject_ preference_ ifs_ ifstat_\n\
set ns_ [Simulator instance]\n\
\n\
catch \"set preference_ [[$self info class] set preference_]\" ret\n\
if { $ret == \"\" } {\n\
set preference_ [$class set preference_]\n\
}\n\
foreach nbr [$node set neighbor_] {\n\
set link [$ns_ link $node $nbr]\n\
set ifs_($nbr) $link\n\
set ifstat_($nbr) [$link up?]\n\
}\n\
set rtObject_ [$node rtObject?]\n\
}\n\
\n\
Agent/rtProto instproc compute-routes {} {\n\
error \"No route computation defined\"\n\
}\n\
\n\
Agent/rtProto instproc intf-changed {} {\n\
}\n\
\n\
Agent/rtProto instproc send-updates args {\n\
}\n\
\n\
Agent/rtProto proc compute-all {} {\n\
}\n\
\n\
Class Agent/rtProto/Static -superclass Agent/rtProto\n\
\n\
Agent/rtProto/Static proc init-all args {\n\
[Simulator instance] compute-routes\n\
}\n\
\n\
Class Agent/rtProto/Session -superclass Agent/rtProto\n\
\n\
Agent/rtProto/Session proc init-all args {\n\
[Simulator instance] compute-routes\n\
}\n\
\n\
Agent/rtProto/Session proc compute-all {} {\n\
[Simulator instance] compute-routes\n\
}\n\
\n\
Class Agent/rtProto/Direct -superclass Agent/rtProto\n\
Agent/rtProto/Direct instproc init node {\n\
$self next $node\n\
$self instvar ns_ rtpref_ nextHop_ metric_ ifs_\n\
\n\
foreach node [$ns_ all-nodes-list] {\n\
set rtpref_($node) 255\n\
set nextHop_($node) \"\"\n\
set metric_($node) -1\n\
}\n\
foreach node [array names ifs_] {\n\
set rtpref_($node) [$class set preference_]\n\
}\n\
}\n\
\n\
Agent/rtProto/Direct instproc compute-routes {} {\n\
$self instvar ifs_ ifstat_ nextHop_ metric_ rtsChanged_\n\
set rtsChanged_ 0\n\
foreach nbr [array names ifs_] {\n\
if {$nextHop_($nbr) == \"\" && [$ifs_($nbr) up?] == \"up\"} {\n\
set ifstat_($nbr) 1\n\
set nextHop_($nbr) $ifs_($nbr)\n\
set metric_($nbr) [$ifs_($nbr) cost?]\n\
incr rtsChanged_\n\
} elseif {$nextHop_($nbr) != \"\" && [$ifs_($nbr) up?] != \"up\"} {\n\
set ifstat_($nbr) 0\n\
set nextHop_($nbr) \"\"\n\
set metric_($nbr) -1\n\
incr rtsChanged_\n\
}\n\
}\n\
}\n\
\n\
Agent/rtProto/DV set UNREACHABLE	[rtObject set unreach_]\n\
Agent/rtProto/DV set mid_		  0\n\
\n\
Agent/rtProto/DV proc init-all args {\n\
if { [llength $args] == 0 } {\n\
set nodeslist [[Simulator instance] all-nodes-list]\n\
} else {\n\
eval \"set nodeslist $args\"\n\
}\n\
Agent set-maxttl Agent/rtProto/DV INFINITY\n\
eval rtObject init-all $nodeslist\n\
foreach node $nodeslist {\n\
set proto($node) [[$node rtObject?] add-proto DV $node]\n\
}\n\
foreach node $nodeslist {\n\
foreach nbr [$node neighbors] {\n\
set rtobj [$nbr rtObject?]\n\
if { $rtobj != \"\" } {\n\
set rtproto [$rtobj rtProto? DV]\n\
if { $rtproto != \"\" } {\n\
$proto($node) add-peer $nbr [$rtproto set agent_addr_] [$rtproto set agent_port_]\n\
}\n\
}\n\
}\n\
}\n\
}\n\
\n\
Agent/rtProto/DV instproc init node {\n\
global rtglibRNG\n\
\n\
$self next $node\n\
$self instvar ns_ rtObject_ ifsUp_\n\
$self instvar preference_ rtpref_ nextHop_ nextHopPeer_ metric_ multiPath_\n\
\n\
set UNREACHABLE [$class set UNREACHABLE]\n\
foreach dest [$ns_ all-nodes-list] {\n\
set rtpref_($dest) $preference_\n\
set nextHop_($dest) \"\"\n\
set nextHopPeer_($dest) \"\"\n\
set metric_($dest)  $UNREACHABLE\n\
}\n\
set ifsUp_ \"\"\n\
set multiPath_ [[$rtObject_ set node_] set multiPath_]\n\
set updateTime [$rtglibRNG uniform 0.0 0.5]\n\
$ns_ at $updateTime \"$self send-periodic-update\"\n\
}\n\
\n\
Agent/rtProto/DV instproc add-peer {nbr agentAddr agentPort} {\n\
$self instvar peers_\n\
$self set peers_($nbr) [new rtPeer $agentAddr $agentPort $class]\n\
}\n\
\n\
Agent/rtProto/DV instproc send-periodic-update {} {\n\
global rtglibRNG\n\
\n\
$self instvar ns_\n\
$self send-updates 1	;# Anything but 0\n\
set updateTime [expr [$ns_ now] + \\\n\
([$class set advertInterval] * [$rtglibRNG uniform 0.9 1.1])]\n\
$ns_ at $updateTime \"$self send-periodic-update\"\n\
}\n\
\n\
Agent/rtProto/DV instproc compute-routes {} {\n\
$self instvar ns_ ifs_ rtpref_ metric_ nextHop_ nextHopPeer_\n\
$self instvar peers_ rtsChanged_ multiPath_\n\
\n\
set INFINITY [$class set INFINITY]\n\
set MAXPREF  [rtObject set maxpref_]\n\
set UNREACH	 [rtObject set unreach_]\n\
set rtsChanged_ 0\n\
foreach dst [$ns_ all-nodes-list] {\n\
set p [lindex $nextHopPeer_($dst) 0]\n\
if {$p != \"\"} {\n\
set metric_($dst) [$p metric? $dst]\n\
set rtpref_($dst) [$p preference? $dst]\n\
}\n\
\n\
set pf $MAXPREF\n\
set mt $INFINITY\n\
set nh(0) 0\n\
foreach nbr [lsort -dictionary [array names peers_]] {\n\
set pmt [$peers_($nbr) metric? $dst]\n\
set ppf [$peers_($nbr) preference? $dst]\n\
\n\
\n\
if { $pmt < 0 || $pmt >= $INFINITY || $ppf > $pf || $pmt > $mt } \\\n\
continue\n\
if { $ppf < $pf || $pmt < $mt } {\n\
set pf $ppf\n\
set mt $pmt\n\
unset nh	;# because we must compute *new* next hops\n\
}\n\
set nh($ifs_($nbr)) $peers_($nbr)\n\
}\n\
catch \"unset nh(0)\"\n\
if { $pf == $MAXPREF && $mt == $INFINITY } continue\n\
if { $pf > $rtpref_($dst) ||				\\\n\
($metric_($dst) >= 0 && $mt > $metric_($dst)) }	\\\n\
continue\n\
if {$mt >= $INFINITY} {\n\
set mt $UNREACH\n\
}\n\
\n\
incr rtsChanged_\n\
if { $pf < $rtpref_($dst) || $mt < $metric_($dst) } {\n\
set rtpref_($dst) $pf\n\
set metric_($dst) $mt\n\
set nextHop_($dst) \"\"\n\
set nextHopPeer_($dst) \"\"\n\
foreach n [array names nh] {\n\
lappend nextHop_($dst) $n\n\
lappend nextHopPeer_($dst) $nh($n)\n\
if !$multiPath_ break;\n\
}\n\
continue\n\
}\n\
\n\
set rtpref_($dst) $pf\n\
set metric_($dst) $mt\n\
set newNextHop \"\"\n\
set newNextHopPeer \"\"\n\
foreach rt $nextHop_($dst) {\n\
if [info exists nh($rt)] {\n\
lappend newNextHop $rt\n\
lappend newNextHopPeer $nh($rt)\n\
unset nh($rt)\n\
}\n\
}\n\
set nextHop_($dst) $newNextHop\n\
set nextHopPeer_($dst) $newNextHopPeer\n\
if { $multiPath_ || $nextHop_($dst) == \"\" } {\n\
foreach rt [array names nh] {\n\
lappend nextHop_($dst) $rt\n\
lappend nextHopPeer_($dst) $nh($rt)\n\
if !$multiPath_ break\n\
}\n\
}\n\
}\n\
set rtsChanged_\n\
}\n\
\n\
Agent/rtProto/DV instproc intf-changed {} {\n\
$self instvar ns_ peers_ ifs_ ifstat_ ifsUp_ nextHop_ nextHopPeer_ metric_\n\
set INFINITY [$class set INFINITY]\n\
set ifsUp_ \"\"\n\
foreach nbr [lsort -dictionary [array names peers_]] {\n\
set state [$ifs_($nbr) up?]\n\
if {$state != $ifstat_($nbr)} {\n\
set ifstat_($nbr) $state\n\
if {$state != \"up\"} {\n\
if ![info exists all-nodes] {\n\
set all-nodes [$ns_ all-nodes-list]\n\
}\n\
foreach dest ${all-nodes} {\n\
$peers_($nbr) metric $dest $INFINITY\n\
}\n\
} else {\n\
lappend ifsUp_ $nbr\n\
}\n\
}\n\
}\n\
}\n\
\n\
Agent/rtProto/DV proc get-next-mid {} {\n\
set ret [Agent/rtProto/DV set mid_]\n\
Agent/rtProto/DV set mid_ [expr $ret + 1]\n\
set ret\n\
}\n\
\n\
Agent/rtProto/DV proc retrieve-msg id {\n\
set ret [Agent/rtProto/DV set msg_($id)]\n\
Agent/rtProto/DV unset msg_($id)\n\
set ret\n\
}\n\
\n\
Agent/rtProto/DV instproc send-updates changes {\n\
$self instvar peers_ ifs_ ifsUp_\n\
\n\
if $changes {\n\
set to-send-to [lsort -dictionary [array names peers_]]\n\
} else {\n\
set to-send-to $ifsUp_\n\
}\n\
set ifsUp_ \"\"\n\
foreach nbr ${to-send-to} {\n\
if { [$ifs_($nbr) up?] == \"up\" } {\n\
$self send-to-peer $nbr\n\
}\n\
}\n\
}\n\
\n\
Agent/rtProto/DV instproc send-to-peer nbr {\n\
$self instvar ns_ rtObject_ ifs_ peers_\n\
set INFINITY [$class set INFINITY]\n\
foreach dest [$ns_ all-nodes-list] {\n\
set metric [$rtObject_ metric? $dest]\n\
if {$metric < 0} {\n\
set update($dest) $INFINITY\n\
} else {\n\
set update($dest) [$rtObject_ metric? $dest]\n\
foreach nh [$rtObject_ nextHop? $dest] {\n\
if {$nh == $ifs_($nbr)} {\n\
set update($dest) $INFINITY\n\
}\n\
}\n\
}\n\
}\n\
\n\
if { $peers_($nbr) == \"\" } {\n\
return\n\
}\n\
\n\
set id [$class get-next-mid]\n\
$class set msg_($id) [array get update]\n\
\n\
$self send-update [$peers_($nbr) addr?] [$peers_($nbr) port?] $id [array size update]\n\
}\n\
\n\
Agent/rtProto/DV instproc recv-update {peerAddr id} {\n\
$self instvar peers_ ifs_ nextHopPeer_ metric_\n\
$self instvar rtsChanged_ rtObject_\n\
\n\
set INFINITY [$class set INFINITY]\n\
set UNREACHABLE  [$class set UNREACHABLE]\n\
set msg [$class retrieve-msg $id]\n\
array set metrics $msg\n\
foreach nbr [lsort -dictionary [array names peers_]] {\n\
if {[$peers_($nbr) addr?] == $peerAddr} {\n\
set peer $peers_($nbr)\n\
if { [array size metrics] > [Node set nn_] } {\n\
error \"$class::$proc update $peerAddr:$msg:$count is larger than the simulation topology\"\n\
}\n\
set metricsChanged 0\n\
foreach dest [array names metrics] {\n\
set metric [expr $metrics($dest) + [$ifs_($nbr) cost?]]\n\
if {$metric > $INFINITY} {\n\
set metric $INFINITY\n\
}\n\
if {$metric != [$peer metric? $dest]} {\n\
$peer metric $dest $metric\n\
incr metricsChanged\n\
}\n\
}\n\
if $metricsChanged {\n\
$self compute-routes\n\
incr rtsChanged_ $metricsChanged\n\
$rtObject_ compute-routes\n\
} else {\n\
$rtObject_ flag-multicast -1\n\
}\n\
return\n\
}\n\
}\n\
error \"$class::$proc update $peerAddr:$msg:$count from unknown peer\"\n\
}\n\
\n\
Agent/rtProto/DV proc compute-all {} {\n\
}\n\
\n\
Class Agent/rtProto/Manual -superclass Agent/rtProto\n\
\n\
Agent/rtProto/Manual proc pre-init-all args {\n\
Node enable-module Manual\n\
}\n\
\n\
Agent/rtProto/Manual proc init-all args {\n\
}\n\
\n\
\n\
\n\
\n\
Class rtQueue\n\
\n\
Simulator instproc rtmodel { dist parms args } {\n\
set ret \"\"\n\
if { [rtModel info subclass rtModel/$dist] != \"\" } {\n\
$self instvar  rtModel_\n\
set ret [eval new rtModel/$dist $self]\n\
eval $ret set-elements $args\n\
eval $ret set-parms $parms\n\
set trace [$self get-ns-traceall]\n\
if {$trace != \"\"} {\n\
$ret trace $self $trace\n\
}\n\
set trace [$self get-nam-traceall]\n\
if {$trace != \"\"} {\n\
$ret trace $self $trace \"nam\"\n\
}\n\
if [info exists rtModel_] {\n\
lappend rtModel_ $ret\n\
} else {\n\
set rtModel_ $ret\n\
}\n\
}\n\
return $ret\n\
}\n\
\n\
Simulator instproc rtmodel-configure {} {\n\
$self instvar rtq_ rtModel_\n\
if [info exists rtModel_] {\n\
set rtq_ [new rtQueue $self]\n\
foreach m $rtModel_ {\n\
$m configure\n\
}\n\
}\n\
}\n\
\n\
Simulator instproc rtmodel-at {at op args} {\n\
set parms [list $op $at]\n\
eval $self rtmodel Manual [list $parms] $args\n\
}\n\
\n\
Simulator instproc rtmodel-delete model {\n\
$self instvar rtModel_\n\
set idx [lsearch -exact $rtModel_ $model]\n\
if { $idx != -1 } {\n\
delete $model\n\
set rtModel_ [lreplace $rtModel_ $idx $idx]\n\
}\n\
}\n\
\n\
rtQueue instproc init ns {\n\
$self next\n\
$self instvar ns_\n\
set ns_ $ns\n\
}\n\
\n\
rtQueue instproc insq-i { interval obj iproc args } {\n\
$self instvar rtq_ ns_\n\
set time [expr $interval + [$ns_ now]]\n\
if ![info exists rtq_($time)] {\n\
$ns_ at $time \"$self runq $time\"\n\
}\n\
lappend rtq_($time) \"$obj $iproc $args\"\n\
return $time\n\
}\n\
\n\
rtQueue instproc insq { at obj iproc args } {\n\
$self instvar rtq_ ns_\n\
if {[$ns_ now] >= $at} {\n\
puts stderr \"$proc: Cannot set event in the past\"\n\
set at \"\"\n\
} else {\n\
if ![info exists rtq_($at)] {\n\
$ns_ at $at \"$self runq $at\"\n\
}\n\
lappend rtq_($at) \"$obj $iproc $args\"\n\
}\n\
return $at\n\
}\n\
\n\
rtQueue instproc delq { time obj } {\n\
$self instvar rtq_\n\
set ret \"\"\n\
set nevent \"\"\n\
if [info exists rtq_($time)] {\n\
foreach event $rtq_($time) {\n\
if {[lindex $event 0] != $obj} {\n\
lappend nevent $event\n\
} else {\n\
set ret $event\n\
}\n\
}\n\
set rtq_($time) $nevent		;# XXX\n\
}\n\
return ret\n\
}\n\
\n\
rtQueue instproc runq { time } {\n\
$self instvar rtq_\n\
set objects \"\"\n\
foreach event $rtq_($time) {\n\
set obj   [lindex $event 0]\n\
set iproc [lindex $event 1]\n\
set args  [lrange $event 2 end]\n\
eval $obj $iproc $args\n\
lappend objects $obj\n\
}\n\
foreach obj $objects {\n\
$obj notify\n\
}\n\
unset rtq_($time)\n\
}\n\
\n\
Class rtModel\n\
\n\
rtModel set rtq_ \"\"\n\
\n\
rtModel instproc init ns {\n\
$self next\n\
$self instvar ns_ startTime_ finishTime_\n\
set ns_ $ns\n\
set startTime_ [$class set startTime_]\n\
set finishTime_ [$class set finishTime_]\n\
}\n\
\n\
rtModel instproc set-elements args {\n\
$self instvar ns_ links_ nodes_\n\
if { [llength $args] == 2 } {\n\
set n0 [lindex $args 0]\n\
set n1 [lindex $args 1]\n\
set n0id [$n0 id]\n\
set n1id [$n1 id]\n\
\n\
set nodes_($n0id) $n0\n\
set nodes_($n1id) $n1\n\
set links_($n0id:$n1id) [$ns_ link $n0 $n1]\n\
set links_($n1id:$n0id) [$ns_ link $n1 $n0]\n\
} else {\n\
set n0 [lindex $args 0]\n\
set n0id [$n0 id]\n\
set nodes_($n0id) $n0\n\
foreach nbr [$n0 set neighbor_] {\n\
set n1 $nbr\n\
set n1id [$n1 id]\n\
\n\
set nodes_($n1id) $n1\n\
set links_($n0id:$n1id) [$ns_ link $n0 $n1]\n\
set links_($n1id:$n0id) [$ns_ link $n1 $n0]\n\
}\n\
}\n\
}\n\
\n\
rtModel instproc set-parms args {\n\
$self instvar startTime_ upInterval_ downInterval_ finishTime_\n\
\n\
set cls [$self info class]\n\
foreach i {startTime_ upInterval_ downInterval_ finishTime_} {\n\
if [catch \"$cls set $i\" $i] {\n\
set $i [$class set $i]\n\
}\n\
}\n\
\n\
set off \"-\"\n\
set up  \"-\"\n\
set dn  \"-\"\n\
set fin \"-\"\n\
\n\
switch [llength $args] {\n\
4 {\n\
set off [lindex $args 0]\n\
set up  [lindex $args 1]\n\
set dn  [lindex $args 2]\n\
set fin [lindex $args 3]\n\
}\n\
3 {\n\
set off [lindex $args 0]\n\
set up  [lindex $args 1]\n\
set dn  [lindex $args 2]\n\
}\n\
2 {\n\
set up [lindex $args 0]\n\
set dn [lindex $args 1]\n\
}\n\
}\n\
if {$off != \"-\" && $off != \"\"} {\n\
set startTime_ $off\n\
}\n\
if {$up != \"-\" && $up != \"\"} {\n\
set upInterval_ $up\n\
}\n\
if {$dn != \"-\" && $dn != \"\"} {\n\
set downInterval_ $dn\n\
}\n\
if {$fin != \"-\" && $fin != \"\"} {\n\
set finishTime_ $fin\n\
}\n\
}\n\
\n\
rtModel instproc configure {} {\n\
$self instvar ns_ links_\n\
if { [rtModel set rtq_] == \"\" } {\n\
rtModel set rtq_ [$ns_ set rtq_]\n\
}\n\
\n\
foreach l [array names links_] {\n\
$links_($l) dynamic\n\
}\n\
$self set-first-event\n\
}\n\
\n\
rtModel instproc set-event-exact {fireTime op} {\n\
$self instvar ns_ finishTime_\n\
if {$finishTime_ != \"-\" && $fireTime > $finishTime_} {\n\
if {$op == \"up\"} {\n\
[rtModel set rtq_] insq $finishTime_ $self $op\n\
}\n\
} else {\n\
[rtModel set rtq_] insq $fireTime $self $op\n\
}\n\
}\n\
\n\
rtModel instproc set-event {interval op} {\n\
$self instvar ns_\n\
$self set-event-exact [expr [$ns_ now] + $interval] $op\n\
}\n\
\n\
rtModel instproc set-first-event {} {\n\
$self instvar startTime_ upInterval_\n\
$self set-event [expr $startTime_ + $upInterval_] down\n\
}\n\
\n\
rtModel instproc up {} {\n\
$self instvar links_\n\
foreach l [array names links_] {\n\
$links_($l) up\n\
}\n\
}\n\
\n\
rtModel instproc down {} {\n\
$self instvar links_\n\
foreach l [array names links_] {\n\
$links_($l) down\n\
}\n\
}\n\
\n\
rtModel instproc notify {} {\n\
$self instvar nodes_ ns_\n\
foreach n [array names nodes_] {\n\
$nodes_($n) intf-changed\n\
}\n\
[$ns_ get-routelogic] notify\n\
}\n\
\n\
rtModel instproc trace { ns f {op \"\"} } {\n\
$self instvar links_\n\
foreach l [array names links_] {\n\
$links_($l) trace-dynamics $ns $f $op\n\
}\n\
}\n\
\n\
\n\
Class rtModel/Exponential -superclass rtModel\n\
\n\
rtModel/Exponential instproc set-first-event {} {\n\
global rtglibRNG\n\
\n\
$self instvar startTime_ upInterval_\n\
$self set-event [expr $startTime_ + [$rtglibRNG exponential] * $upInterval_] down\n\
}\n\
\n\
rtModel/Exponential instproc up { } {\n\
global rtglibRNG\n\
\n\
$self next\n\
$self instvar upInterval_\n\
$self set-event [expr [$rtglibRNG exponential] * $upInterval_] down\n\
}\n\
\n\
rtModel/Exponential instproc down { } {\n\
global rtglibRNG\n\
\n\
$self next\n\
$self instvar downInterval_\n\
$self set-event [expr [$rtglibRNG exponential] * $downInterval_] up\n\
}\n\
\n\
\n\
Class rtModel/Deterministic -superclass rtModel\n\
\n\
rtModel/Deterministic instproc up { } {\n\
$self next\n\
$self instvar upInterval_\n\
$self set-event $upInterval_ down\n\
}\n\
\n\
rtModel/Deterministic instproc down { } {\n\
$self next\n\
$self instvar downInterval_\n\
$self set-event $downInterval_ up\n\
}\n\
\n\
\n\
Class rtModel/Trace -superclass rtModel\n\
\n\
rtModel/Trace instproc get-next-event {} {\n\
$self instvar tracef_ links_\n\
while {[gets $tracef_ event] >= 0} {\n\
set toks [split $event]\n\
if [info exists links_([lindex $toks 3]:[lindex $toks 4])] {\n\
return $toks\n\
}\n\
}\n\
return \"\"\n\
}\n\
\n\
rtModel/Trace instproc set-trace-events {} {\n\
$self instvar ns_ nextEvent_ evq_\n\
\n\
set time [lindex $nextEvent_ 1]\n\
while {$nextEvent_ != \"\"} {\n\
set nextTime [lindex $nextEvent_ 1]\n\
if {$nextTime < $time} {\n\
puts stderr \"event $nextEvent_  is before current time $time. ignored.\"\n\
continue\n\
}\n\
if {$nextTime > $time} break\n\
if ![info exists evq_($time)] {\n\
set op [string range [lindex $nextEvent_ 2] 5 end]\n\
$self set-event-exact $time $op\n\
set evq_($time) 1\n\
}\n\
set nextEvent_ [$self get-next-event]\n\
}\n\
}\n\
\n\
\n\
rtModel/Trace instproc set-parms traceFile {\n\
$self instvar tracef_ nextEvent_\n\
if [catch \"open $traceFile r\" tracef_] {\n\
puts stderr \"cannot open $traceFile\"\n\
} else {\n\
set nextEvent_ [$self get-next-event]\n\
if {$nextEvent_ == \"\"} {\n\
puts stderr \"no relevant events in $traceFile\"\n\
}\n\
}\n\
}\n\
\n\
rtModel/Trace instproc set-first-event {} {\n\
$self set-trace-events\n\
}\n\
\n\
rtModel/Trace instproc up {} {\n\
$self next\n\
$self set-trace-events\n\
}\n\
\n\
rtModel/Trace instproc down {} {\n\
$self next\n\
$self set-trace-events\n\
}\n\
\n\
Class rtModel/Manual -superclass rtModel\n\
\n\
rtModel/Manual instproc set-first-event {} {\n\
$self instvar op_ at_\n\
$self set-event-exact $at_ $op_ ;# you could concievably set a finishTime_?\n\
}\n\
\n\
rtModel/Manual instproc set-parms {op at} {\n\
$self instvar op_ at_\n\
set op_ $op\n\
set at_ $at\n\
}\n\
\n\
rtModel/Manual instproc notify {} {\n\
$self next\n\
delete $self		;# XXX wierd code alert.\n\
}\n\
Class Agent/rtProto/Algorithmic -superclass Agent/rtProto\n\
\n\
Agent/rtProto/Algorithmic proc init-all args {\n\
[Simulator instance] compute-algo-routes\n\
}\n\
\n\
Agent/rtProto/Algorithmic proc compute-all {} {\n\
[Simulator instance] compute-algo-routes\n\
}\n\
\n\
RouteLogic/Algorithmic instproc BFS {} {\n\
$self instvar ns_ children_ root_ rank_\n\
\n\
set ns_ [Simulator instance]\n\
if {[$ns_ info class] == \"Simulator\"} {\n\
$ns_ instvar link_\n\
foreach ln [array names link_] {\n\
set L [split $ln :]\n\
set srcID [lindex $L 0]\n\
set dstID [lindex $L 1]\n\
if ![info exist adj($srcID)] {\n\
set adj($srcID) \"\"\n\
}\n\
if ![info exist adj($dstID)] {\n\
set adj($dstID) \"\"\n\
}\n\
if {[lsearch $adj($srcID) $dstID] < 0} {\n\
lappend adj($srcID) $dstID\n\
}\n\
if {[lsearch $adj($dstID) $srcID] < 0} {\n\
lappend adj($dstID) $srcID\n\
}\n\
}\n\
} elseif {[$ns_ info class] == \"SessionSim\"} {\n\
$ns_ instvar delay_\n\
foreach ln [array names delay_] {\n\
set L [split $ln :]\n\
set srcID [lindex $L 0]\n\
set dstID [lindex $L 1]\n\
if ![info exist adj($srcID)] {\n\
set adj($srcID) \"\"\n\
}\n\
if ![info exist adj($dstID)] {\n\
set adj($dstID) \"\"\n\
}\n\
if {[lsearch $adj($srcID) $dstID] < 0} {\n\
lappend adj($srcID) $dstID\n\
}\n\
if {[lsearch $adj($dstID) $srcID] < 0} {\n\
lappend adj($dstID) $srcID\n\
}\n\
}\n\
}\n\
\n\
\n\
set rank_ 0\n\
set root_ 0\n\
set traversed($root_) 1\n\
set queue \"$root_\"\n\
\n\
while {[llength $queue] > 0} {\n\
set parent [lindex $queue 0]\n\
set queue [lreplace $queue 0 0]\n\
if ![info exist children_($parent)] {\n\
set children_($parent) \"\"\n\
}\n\
\n\
foreach nd $adj($parent) {\n\
if ![info exist traversed($nd)] {\n\
set traversed($nd) 0\n\
}\n\
if !$traversed($nd) {\n\
set traversed($nd) 1\n\
lappend children_($parent) $nd\n\
lappend queue $nd\n\
}\n\
}\n\
set num_children [llength $children_($parent)]\n\
if {$rank_ < $num_children} {\n\
set rank_ $num_children\n\
}\n\
}\n\
}\n\
\n\
RouteLogic/Algorithmic instproc compute {} {\n\
$self instvar root_ children_ rank_ id_ algoAdd_\n\
\n\
\n\
set queue [list [list $root_ 0]]\n\
\n\
while {[llength $queue] > 0} {\n\
set parent [lindex $queue 0]\n\
set queue [lreplace $queue 0 0]\n\
set id [lindex $parent 0]\n\
set algoAdd [lindex $parent 1]\n\
set id_($algoAdd) $id\n\
set algoAdd_($id) $algoAdd\n\
\n\
set i 0\n\
foreach child $children_($id) {\n\
incr i\n\
lappend queue [list $child [expr [expr $algoAdd * $rank_] + $i]]\n\
}\n\
}\n\
}\n\
\n\
RouteLogic/Algorithmic instproc lookup {src dst} {\n\
$self instvar id_ algoAdd_\n\
set algosrc $algoAdd_($src)\n\
set algodst $algoAdd_($dst)\n\
set algonxt [$self algo-lookup $algosrc $algodst]\n\
return $id_($algonxt)\n\
}\n\
\n\
\n\
RouteLogic/Algorithmic instproc algo-lookup {src dst} {\n\
$self instvar rank_\n\
\n\
if {$src == $dst} {\n\
return $src\n\
}\n\
set a $src\n\
set b $dst\n\
set offset 0\n\
\n\
while {$b > $a} {\n\
set offset [expr $b % $rank_]\n\
set b [expr $b / $rank_]\n\
if {$offset == 0} {\n\
set offset $rank_\n\
set b [expr $b - 1]\n\
}\n\
}\n\
\n\
if {$b == $a} {\n\
return [expr [expr $a * $rank_] + $offset]\n\
} else {\n\
return [expr [expr $a - 1] / $rank_]\n\
}\n\
}\n\
\n\
\n\
Simulator instproc compute-algo-routes {} {\n\
$self instvar Node_ link_\n\
set r [$self get-routelogic]\n\
\n\
$r BFS\n\
$r compute\n\
\n\
set i 0\n\
set n [Node set nn_]\n\
while { $i < $n } {\n\
if ![info exists Node_($i)] {\n\
incr i\n\
continue\n\
}\n\
set n1 $Node_($i)\n\
$n1 set rtsize_ 1 \n\
set j 0\n\
while { $j < $n } {\n\
if { $i != $j } {\n\
set nh [$r lookup $i $j]\n\
if { $nh >= 0 } {\n\
$n1 add-route $j [$link_($i:$nh) head]\n\
}\n\
} \n\
incr j\n\
}\n\
incr i\n\
}\n\
}\n\
\n\
\n\
\n\
\n\
\n\
\n\
\n\
ErrorModel/Trace instproc init {{filename \"\"}} {\n\
$self instvar file_\n\
$self next\n\
set file_ \"\"\n\
if {$filename != \"\"} {\n\
$self open $filename\n\
}\n\
}\n\
\n\
ErrorModel/Trace instproc open {filename} {\n\
$self instvar file_\n\
if {! [file readable $filename]} {\n\
puts \"$class: cannot open $filename\"\n\
return\n\
}\n\
if {$file_ != \"\"} {\n\
close $file_\n\
}\n\
set file_ [open $filename]\n\
$self read\n\
}\n\
\n\
ErrorModel/Trace instproc read {} {\n\
$self instvar file_ good_ loss_\n\
if {$file_ != \"\"} {\n\
set line [gets $file_]\n\
set good_ [lindex $line 0]\n\
set loss_ [lindex $line 1]\n\
} else {\n\
set good_ 123456789\n\
set loss_ 0\n\
}\n\
}\n\
\n\
\n\
ErrorModel/TwoState instproc init {rv0 rv1 {unit \"pkt\"}} {\n\
$self next\n\
$self unit $unit\n\
$self ranvar 0 $rv0\n\
$self ranvar 1 $rv1\n\
}\n\
\n\
Class ErrorModel/Uniform -superclass ErrorModel\n\
Class ErrorModel/Expo -superclass ErrorModel/TwoState\n\
Class ErrorModel/Empirical -superclass ErrorModel/TwoState\n\
\n\
ErrorModel/Uniform instproc init {rate {unit \"pkt\"}} {\n\
$self next\n\
$self unit $unit\n\
$self set rate_ $rate\n\
}\n\
\n\
ErrorModel/Expo instproc init {avgList {unit \"pkt\"}} {\n\
set rv0 [new RandomVariable/Exponential]\n\
set rv1 [new RandomVariable/Exponential]\n\
$rv0 set avg_ [lindex $avgList 0]\n\
$rv1 set avg_ [lindex $avgList 1]\n\
$self next $rv0 $rv1 $unit\n\
}\n\
\n\
ErrorModel/Empirical instproc init {fileList {unit \"pkt\"}} {\n\
set rv0 [new RandomVariable/Empirical]\n\
set rv1 [new RandomVariable/Empirical]\n\
$rv0 loadCDF [lindex $fileList 0]\n\
$rv1 loadCDF [lindex $fileList 1]\n\
$self next $rv0 $rv1 $unit\n\
}\n\
\n\
ErrorModel/MultiState instproc init {states periods trans transunit sttype nstates start} {\n\
\n\
$self instvar states_ transmatrix_ transunit_ nstates_ curstate_ eu_ periods_\n\
\n\
$self next\n\
set states_ $states\n\
set periods_ $periods\n\
set transmatrix_ $trans\n\
set transunit_ $transunit\n\
$self sttype $sttype\n\
set nstates_ $nstates\n\
set curstate_ $start\n\
set eu_ $transunit\n\
$self error-model $start\n\
\n\
if { [$self sttype] == \"time\" } {\n\
for { set i 0 } { $i < $nstates_ } {incr i} {\n\
if { [lindex $states_ $i] == $curstate_ } {\n\
break\n\
}\n\
}\n\
$self set curperiod_ [lindex $periods_ $i]\n\
}\n\
}\n\
\n\
ErrorModel/MultiState instproc corrupt { } {\n\
$self instvar states_ transmatrix_ transunit_ curstate_\n\
\n\
set cur $curstate_\n\
if { [$self sttype] == \"time\" } {\n\
set curstate_ [$self time-transition]\n\
} else {\n\
set curstate_ [$self transition]\n\
}\n\
\n\
if { $cur != $curstate_ } {\n\
$cur reset\n\
$self reset\n\
$self error-model $curstate_\n\
}\n\
return [$curstate_ next]\n\
}\n\
\n\
\n\
ErrorModel/MultiState instproc time-transition { } {\n\
$self instvar states_ transmatrix_ transunit_ curstate_ nstates_ periods_\n\
\n\
if {[$self set texpired_] != 1} {\n\
return $curstate_\n\
}\n\
\n\
for { set i 0 } { $i < $nstates_ } {incr i} {\n\
if { [lindex $states_ $i] == $curstate_ } {\n\
break\n\
}\n\
}\n\
\n\
set trans [lindex $transmatrix_ $i]\n\
set p [uniform 0 1]\n\
set total 0\n\
for { set i 0 } { $i < $nstates_ } {incr i } {\n\
set total [expr $total + [lindex $trans $i]]\n\
if { $p <= $total } {\n\
$self set curperiod_ [lindex $periods_ $i]\n\
return [lindex $states_ $i]\n\
}\n\
}\n\
puts \"Misconfigured state transition: prob $p total $total $nstates_\"\n\
return $curstate_\n\
}\n\
\n\
ErrorModel/MultiState instproc transition { } {\n\
$self instvar states_ transmatrix_ transunit_ curstate_ nstates_\n\
\n\
for { set i 0 } { $i < $nstates_ } {incr i} {\n\
if { [lindex $states_ $i] == $curstate_ } {\n\
break\n\
}\n\
}\n\
set trans [lindex $transmatrix_ $i]\n\
set p [uniform 0 1]\n\
set total 0\n\
for { set i 0 } { $i < $nstates_ } {incr i } {\n\
set total [expr $total + [lindex $trans $i]]\n\
if { $p <= $total } {\n\
return [lindex $states_ $i]\n\
}\n\
}\n\
puts \"Misconfigured state transition: prob $p total $total $nstates_\"\n\
return $curstate_\n\
}\n\
\n\
\n\
Class ErrorModel/TwoStateMarkov -superclass ErrorModel/TwoState\n\
\n\
ErrorModel/TwoStateMarkov instproc init {rate eu {transition}} {\n\
$self next\n\
$self unit time\n\
\n\
set rv0 [new RandomVariable/Exponential]\n\
set rv1 [new RandomVariable/Exponential]\n\
$rv0 set avg_ [lindex $rate 0]\n\
$rv1 set avg_ [lindex $rate 1]\n\
$self ranvar 0 $rv0\n\
$self ranvar 1 $rv1\n\
\n\
\n\
}\n\
\n\
\n\
\n\
ErrorModule instproc init { cltype { clslots 29 } } {\n\
\n\
$self next\n\
set nullagent [[Simulator instance] set nullAgent_]\n\
\n\
set classifier [new Classifier/Hash/$cltype $clslots]\n\
$self cmd classifier $classifier\n\
$self cmd target [new Connector]\n\
$self cmd drop-target [new Connector]\n\
$classifier proc unknown-flow { src dst fid } {\n\
puts \"warning: classifier $self unknown flow s:$src, d:$dst, fid:$fid\"\n\
}\n\
}\n\
\n\
ErrorModule instproc default errmodel {\n\
set cl [$self cmd classifier]\n\
if { $errmodel == \"pass\" } {\n\
set target [$self cmd target]\n\
set pslot [$cl installNext $target]\n\
$cl set default_ $pslot\n\
return\n\
}\n\
\n\
set emslot [$cl findslot $errmodel]\n\
if { $emslot == -1 } {\n\
puts \"ErrorModule: couldn't find classifier entry for error model $errmodel\"\n\
return\n\
}\n\
$cl set default_ $emslot\n\
}\n\
\n\
ErrorModule instproc insert errmodel {\n\
$self instvar models_\n\
$errmodel target [$self cmd target]\n\
$errmodel drop-target [$self cmd drop-target]\n\
if { [info exists models_] } {\n\
set models_ [concat $models_ $errmodel]\n\
} else {\n\
set models_ $errmodel\n\
}\n\
}\n\
\n\
ErrorModule instproc errormodels {} {\n\
$self instvar models_\n\
return $models_\n\
}\n\
\n\
ErrorModule instproc bind args {\n\
\n\
set nargs [llength $args]\n\
set errmod [lindex $args 0]\n\
set a [lindex $args 1]\n\
if { $nargs == 3 } {\n\
set b [lindex $args 2]\n\
} else {\n\
set b $a\n\
}       \n\
set cls [$self cmd classifier]\n\
while { $a <= $b } {\n\
set slot [$cls installNext $errmod] \n\
$cls set-hash auto 0 0 $a $slot\n\
incr a  \n\
}\n\
}\n\
\n\
ErrorModule instproc target args {\n\
if { $args == \"\" } {\n\
return [[$self cmd target] target]\n\
}\n\
set obj [lindex $args 0]\n\
\n\
[$self cmd target] target $obj\n\
[$self cmd target] drop-target $obj\n\
}\n\
\n\
ErrorModule instproc drop-target args {\n\
if { $args == \"\" } {\n\
return [[$self cmd drop-target] target]\n\
}\n\
\n\
set obj [lindex $args 0]\n\
\n\
[$self cmd drop-target] drop-target $obj\n\
[$self cmd drop-target] target $obj\n\
}\n\
\n\
Queue/SimpleIntServ set qlimit1_ 50\n\
Queue/SimpleIntServ set qlimit0_ 50\n\
\n\
Agent/SA set rate_ 0\n\
Agent/SA set bucket_ 0\n\
Agent/SA set packetSize_ 210\n\
\n\
ADC set backoff_ true\n\
ADC set dobump_ true\n\
ADC/MS set backoff_ false\n\
\n\
ADC set src_ -1\n\
ADC set dst_ -1\n\
ADC/MS set utilization_ 0.95\n\
ADC/MSPK set utilization_ 0.95\n\
ADC/Param set utilization_ 1.0\n\
ADC/HB set epsilon_ 0.7\n\
ADC/ACTO set s_ 0.002\n\
ADC/ACTO set dobump_ false\n\
ADC/ACTP set s_ 0.002\n\
ADC/ACTP set dobump_ false\n\
\n\
\n\
Est/TimeWindow set T_ 3\n\
Est/ExpAvg set w_ 0.125\n\
Est set period_ 0.5\n\
\n\
ADC set bandwidth_ 0\n\
\n\
SALink set src_ -1\n\
SALink set dst_ -1\n\
\n\
Est set src_ -1\n\
Est set dst_ -1\n\
\n\
\n\
Class IntServLink -superclass  SimpleLink\n\
IntServLink instproc init { src dst bw delay q arg {lltype \"DelayLink\"} } {\n\
\n\
$self next $src $dst $bw $delay $q $lltype ; # SimpleLink ctor\n\
$self instvar queue_ link_\n\
\n\
$self instvar measclassifier_ signalmod_ adc_ est_ measmod_\n\
\n\
set ns_ [Simulator instance]\n\
\n\
set adctype [lindex $arg 3]\n\
set adc_ [new ADC/$adctype]\n\
$adc_ set bandwidth_ $bw\n\
$adc_ set src_ [$src id]\n\
$adc_ set dst_ [$dst id]\n\
\n\
if { [lindex $arg 5] == \"CL\" } {\n\
set esttype [lindex $arg 4]\n\
set est_ [new Est/$esttype]\n\
$est_ set src_ [$src id]\n\
$est_ set dst_ [$dst id]\n\
$adc_ attach-est $est_ 1\n\
\n\
set measmod_ [new MeasureMod]\n\
$measmod_ target $queue_\n\
$adc_ attach-measmod $measmod_ 1\n\
}\n\
\n\
set signaltype [lindex $arg 2]\n\
set signalmod_ [new $signaltype]\n\
$signalmod_ set src_ [$src id]\n\
$signalmod_ set dst_ [$dst id]\n\
$signalmod_ attach-adc $adc_\n\
$self add-to-head $signalmod_\n\
\n\
\n\
$self create-meas-classifier\n\
$signalmod_ target $measclassifier_\n\
\n\
$ns_ at 0.0 \"$adc_ start\"\n\
}\n\
IntServLink instproc buffersize { b } {\n\
$self instvar est_ adc_\n\
$est_ setbuf [set b]\n\
$adc_ setbuf [set b]\n\
}\n\
\n\
\n\
\n\
IntServLink instproc create-meas-classifier {} {\n\
$self instvar measclassifier_ measmod_ link_ queue_\n\
\n\
set measclassifier_ [new Classifier/Hash/Fid 1 ]\n\
set slot [$measclassifier_ installNext $queue_]\n\
$measclassifier_ set-hash auto 0 0 0 $slot \n\
\n\
set slot [$measclassifier_ installNext $measmod_]\n\
$measclassifier_ set default_ 1\n\
}\n\
\n\
IntServLink instproc trace-sig { f } {\n\
$self instvar signalmod_ est_ adc_\n\
$signalmod_ attach $f\n\
$est_ attach $f\n\
$adc_ attach $f\n\
set ns [Simulator instance]\n\
$ns at 0.0 \"$signalmod_ add-trace\"\n\
}\n\
\n\
IntServLink instproc trace-util { interval {f \"\"}} {\n\
$self instvar est_\n\
set ns [Simulator instance]\n\
if { $f != \"\" } {\n\
puts $f \"[$ns now] [$est_ load-est] [$est_ link-utlzn]\" \n\
}\n\
$ns at [expr [$ns now]+$interval] \"$self trace-util $interval $f\" \n\
}\n\
\n\
CMUTrace instproc init { tname type } {\n\
$self next $tname $type\n\
$self instvar type_ src_ dst_ callback_ show_tcphdr_\n\
\n\
set type_ $type\n\
set src_ 0\n\
set dst_ 0\n\
set callback_ 0\n\
set show_tcphdr_ 0\n\
}\n\
\n\
CMUTrace instproc attach fp {\n\
$self instvar fp_\n\
set fp_ $fp\n\
$self cmd attach $fp_\n\
}\n\
\n\
Class CMUTrace/Send -superclass CMUTrace\n\
CMUTrace/Send instproc init { tname } {\n\
$self next $tname \"s\"\n\
}\n\
\n\
Class CMUTrace/Recv -superclass CMUTrace\n\
CMUTrace/Recv instproc init { tname } {\n\
$self next $tname \"r\"\n\
}\n\
\n\
Class CMUTrace/Drop -superclass CMUTrace\n\
CMUTrace/Drop instproc init { tname } {\n\
$self next $tname \"D\"\n\
}\n\
\n\
\n\
CMUTrace/Recv set src_ 0\n\
CMUTrace/Recv set dst_ 0\n\
CMUTrace/Recv set callback_ 0\n\
CMUTrace/Recv set show_tcphdr_ 0\n\
\n\
CMUTrace/Send set src_ 0\n\
CMUTrace/Send set dst_ 0\n\
CMUTrace/Send set callback_ 0\n\
CMUTrace/Send set show_tcphdr_ 0\n\
\n\
CMUTrace/Drop set src_ 0\n\
CMUTrace/Drop set dst_ 0\n\
CMUTrace/Drop set callback_ 0\n\
CMUTrace/Drop set show_tcphdr_ 0\n\
\n\
\n\
\n\
Node/Broadcast instproc init {} {\n\
$self next\n\
$self instvar address_ classifier_ id_ dmux_\n\
\n\
[Simulator instance] add-broadcast-node $self $id_\n\
\n\
set classifier_ [new Classifier/Hash/Dest/Bcast 32]\n\
$classifier_ set mask_ [AddrParams NodeMask 1]\n\
$classifier_ set shift_ [AddrParams NodeShift 1]\n\
set address_ $id_\n\
if { $dmux_ == \"\" } {\n\
set dmux_ [new Classifier/Port/Reserve]\n\
$dmux_ set mask_ [AddrParams set ALL_BITS_SET]\n\
$dmux_ set shift_ 0\n\
$self add-route $address_ $dmux_\n\
\n\
}\n\
$classifier_ bcast-receiver $dmux_\n\
\n\
$self attach-classifier $classifier_\n\
}\n\
\n\
Node/Broadcast instproc mk-default-classifier {} { }\n\
\n\
\n\
\n\
\n\
Node/Broadcast instproc add-route { dst target } {\n\
[$self set classifier_] install $dst $target\n\
}\n\
\n\
Node/Broadcast instproc delete-route { dst nullagent } {\n\
[$self set classifier_] install $dst $nullagent\n\
}\n\
\n\
Node/Broadcast instproc add-target { agent port } {\n\
$agent target [$self entry]\n\
[$self demux] install $port $agent\n\
}\n\
\n\
MIPEncapsulator instproc tunnel-exit mhaddr {\n\
$self instvar node_\n\
return [[$node_ set regagent_] set TunnelExit_($mhaddr)]\n\
}\n\
\n\
Class Node/MIPBS -superclass Node/Broadcast\n\
\n\
Node/MIPBS instproc init { args } {\n\
eval $self next $args\n\
$self instvar regagent_ encap_ decap_ agents_ address_ dmux_ id_\n\
\n\
if { $dmux_ == \"\" } {\n\
error \"serious internal error at Node/MIPBS\\n\"\n\
}\n\
set regagent_ [new Agent/MIPBS $self]\n\
$self attach $regagent_ 0\n\
$regagent_ set mask_ [AddrParams NodeMask 1]\n\
$regagent_ set shift_ [AddrParams NodeShift 1]\n\
$regagent_ set dst_addr_ [expr (~0) << [AddrParams NodeShift 1]]\n\
$regagent_ set dst_port_ 0\n\
\n\
set encap_ [new MIPEncapsulator]\n\
set decap_ [new Classifier/Addr/MIPDecapsulator]\n\
\n\
lappend agents_ $decap_\n\
\n\
set nodeaddr [AddrParams addr2id $address_]\n\
$encap_ set addr_ $nodeaddr\n\
$encap_ set port_ 1\n\
$encap_ target [$self entry]\n\
$encap_ set node_ $self\n\
\n\
$dmux_ install 1 $decap_\n\
\n\
$encap_ set mask_ [AddrParams NodeMask 1]\n\
$encap_ set shift_ [AddrParams NodeShift 1]\n\
$decap_ set mask_ [AddrParams NodeMask 1]\n\
$decap_ set shift_ [AddrParams NodeShift 1]\n\
}\n\
\n\
Class Node/MIPMH -superclass Node/Broadcast\n\
\n\
Node/MIPMH instproc init { args } {\n\
eval $self next $args\n\
$self instvar regagent_\n\
set regagent_ [new Agent/MIPMH $self]\n\
$self attach $regagent_ 0\n\
$regagent_ set mask_ [AddrParams NodeMask 1]\n\
$regagent_ set shift_ [AddrParams NodeShift 1]\n\
$regagent_ set dst_addr_ [expr (~0) << [AddrParams NodeShift 1]]\n\
$regagent_ set dst_port_ 0\n\
}\n\
\n\
\n\
Agent/MIPBS instproc init { node args } {\n\
eval $self next $args\n\
\n\
if {[$node info class] != \"MobileNode/MIPBS\" && \\\n\
[$node info class] != \"Node/MobileNode\"} {\n\
$self instvar BcastTarget_\n\
set BcastTarget_ [new Classifier/Replicator]\n\
$self bcast-target $BcastTarget_\n\
}\n\
$self beacon-period 1.0	;# default value\n\
}\n\
\n\
Agent/MIPBS instproc clear-reg mhaddr {\n\
$self instvar node_ OldRoute_ RegTimer_\n\
if [info exists OldRoute_($mhaddr)] {\n\
$node_ add-route $mhaddr $OldRoute_($mhaddr)\n\
}\n\
if {[$node_ info class] == \"MobileNode/MIPBS\" || [$node_ info class] ==\"Node/MobileNode\" } {\n\
eval $node_ delete-route [AddrParams id2addr $mhaddr]\n\
}\n\
if { [info exists RegTimer_($mhaddr)] && $RegTimer_($mhaddr) != \"\" } {\n\
[Simulator instance] cancel $RegTimer_($mhaddr)\n\
set RegTimer_($mhaddr) \"\"\n\
}\n\
}\n\
\n\
Agent/MIPBS instproc encap-route { mhaddr coa lifetime } {\n\
$self instvar node_ TunnelExit_ OldRoute_ RegTimer_\n\
set ns [Simulator instance]\n\
set encap [$node_ set encap_]\n\
\n\
if {[$node_ info class] == \"MobileNode/MIPBS\" || [$node_ info class] == \"Node/MobileNode\"} {\n\
set addr [AddrParams id2addr $mhaddr]\n\
set a [split $addr]\n\
set b [join $a .]\n\
$node_ add-route $b $encap\n\
} else {\n\
set or [[$node_ entry] slot $mhaddr]\n\
if { $or != $encap } {\n\
set OldRoute_($mhaddr) $or\n\
$node_ add-route $mhaddr $encap\n\
}\n\
}\n\
set TunnelExit_($mhaddr) $coa\n\
if { [info exists RegTimer_($mhaddr)] && $RegTimer_($mhaddr) != \"\" } {\n\
$ns cancel $RegTimer_($mhaddr)\n\
}\n\
set RegTimer_($mhaddr) [$ns at [expr [$ns now] + $lifetime] \\\n\
\"$self clear-reg $mhaddr\"]\n\
}\n\
\n\
Agent/MIPBS instproc decap-route { mhaddr target lifetime } {\n\
$self instvar node_ RegTimer_\n\
\n\
if {[$node_ info class] != \"MobileNode/MIPBS\" && \\\n\
[$node_ info class] != \"Node/MobileNode\" } {\n\
set ns [Simulator instance]\n\
[$node_ set decap_] install $mhaddr $target\n\
\n\
if { [info exists RegTimer_($mhaddr)] && \\\n\
$RegTimer_($mhaddr) != \"\" } {\n\
$ns cancel $RegTimer_($mhaddr)\n\
}\n\
set RegTimer_($mhaddr) [$ns at [expr [$ns now] + $lifetime] \\\n\
\"$self clear-decap $mhaddr\"]\n\
} else {\n\
[$node_ set decap_] defaulttarget [$node_ set ragent_]\n\
}\n\
}\n\
\n\
Agent/MIPBS instproc clear-decap mhaddr {\n\
$self instvar node_ RegTimer_\n\
if { [info exists RegTimer_($mhaddr)] && $RegTimer_($mhaddr) != \"\" } {\n\
[Simulator instance] cancel $RegTimer_($mhaddr)\n\
set RegTimer_($mhaddr) \"\"\n\
}\n\
[$node_ set decap_] clear $mhaddr\n\
}\n\
\n\
Agent/MIPBS instproc get-link { src dst } {\n\
$self instvar node_\n\
if {[$node_ info class] != \"MobileNode/MIPBS\" && \\\n\
[$node_ info class] != \"Node/MobileNode\"} {\n\
set ns [Simulator instance]\n\
return [[$ns link [$ns get-node-by-addr $src] \\\n\
[$ns get-node-by-addr $dst]] head]\n\
} else { \n\
return \"\"\n\
}\n\
}\n\
\n\
Agent/MIPBS instproc add-ads-bcast-link { ll } {\n\
$self instvar BcastTarget_\n\
$BcastTarget_ installNext [$ll head]\n\
}\n\
\n\
Agent/MIPMH instproc init { node args } {\n\
eval $self next $args\n\
if {[$node info class] != \"MobileNode/MIPMH\" && \\\n\
[$node info class] != \"SRNode/MIPMH\" && \\\n\
[$node info class] != \"Node/MobileNode\" } {\n\
$self instvar BcastTarget_\n\
set BcastTarget_ [new Classifier/Replicator]\n\
$self bcast-target $BcastTarget_\n\
}\n\
$self beacon-period 1.0	;# default value\n\
}\n\
\n\
Agent/MIPMH instproc update-reg coa {\n\
$self instvar node_\n\
if {[$node_ info class] != \"MobileNode/MIPMH\" && \\\n\
[$node_ info class] != \"SRNode/MIPMH\" && \\\n\
[$node_ info class] != \"Node/MobileNode\" } {\n\
set n [Node set nn_]\n\
set ns [Simulator instance]\n\
set id [$node_ id]\n\
set l [[$ns link $node_ [$ns get-node-by-addr $coa]] head]\n\
for { set i 0 } { $i < $n } { incr i } {\n\
if { $i != $id } {\n\
$node_ add-route $i $l\n\
}\n\
}\n\
}\n\
}\n\
\n\
Agent/MIPMH instproc get-link { src dst } {\n\
$self instvar node_\n\
if {[$node_ info class] != \"MobileNode/MIPMH\" && \\\n\
[$node_ info class] != \"SRNode/MIPMH\" && \\\n\
[$node_ info class] != \"Node/MobileNode\" } {\n\
set ns [Simulator instance]\n\
return [[$ns link [$ns get-node-by-addr $src] \\\n\
[$ns get-node-by-addr $dst]] head]\n\
} else {\n\
return \"\"\n\
}\n\
}\n\
\n\
Agent/MIPMH instproc add-sol-bcast-link { ll } {\n\
$self instvar BcastTarget_\n\
$BcastTarget_ installNext [$ll head]\n\
}\n\
\n\
\n\
\n\
\n\
\n\
\n\
\n\
\n\
\n\
\n\
\n\
Node/SatNode instproc init args {\n\
eval $self next $args		;# parent class constructor\n\
\n\
$self instvar nifs_ \n\
$self instvar phy_tx_ phy_rx_ mac_ ifq_ ll_ pos_ hm_\n\
\n\
set nifs_	0		;# number of network interfaces\n\
set ns_ [Simulator instance]\n\
set trace_ [$ns_ get-ns-traceall]\n\
if {$trace_ != \"\"} {\n\
set dropT_ [$ns_ create-trace Sat/Drop $trace_ $self $self \"\"]\n\
$self set_trace $dropT_\n\
}\n\
}\n\
\n\
Node/SatNode instproc reset {} {\n\
eval $self next \n\
$self instvar hm_ instvar nifs_ phy_tx_ phy_rx_ mac_ ifq_ ll_\n\
set ns [Simulator instance]\n\
set now_ [$ns now]\n\
for {set i 0} {$i < $nifs_} {incr i} {\n\
$phy_tx_($i) reset\n\
$phy_rx_($i) reset\n\
if {[info exists mac_($i)]} {\n\
$mac_($i) reset\n\
}\n\
if {[info exists ll_($i)]} {\n\
$ll_($i) reset\n\
}\n\
if {[info exists ifq_($i)]} {\n\
$ifq_($i) reset\n\
}\n\
}\n\
if {$now_ == 0} {\n\
if {[info exists hm_]} {\n\
$ns at 0.0 \"$self start_handoff\"\n\
}\n\
}\n\
}\n\
\n\
Node/SatNode instproc set_next {node_} {\n\
$self instvar pos_\n\
$pos_ set_next [$node_ set pos_]\n\
}\n\
\n\
Node/SatNode instproc add-target {agent port } {\n\
\n\
$self instvar dmux_ \n\
\n\
if { $port == [Node set rtagent_port_] } {			\n\
[$self set classifier_] defaulttarget $agent\n\
$dmux_ install $port $agent\n\
} else {\n\
$agent target [$self entry]\n\
\n\
$dmux_ install $port $agent\n\
}\n\
}\n\
\n\
\n\
Simulator instproc satnode-polar {alt inc lon alpha plane linkargs chan} {\n\
set tmp [$self satnode polar $alt $inc $lon $alpha $plane]\n\
$self add-first-links $tmp gsl $linkargs $chan\n\
return $tmp\n\
}\n\
\n\
Simulator instproc satnode-geo {lon linkargs chan} {\n\
set tmp [$self satnode geo $lon]\n\
$self add-first-links $tmp gsl $linkargs $chan\n\
return $tmp\n\
}\n\
\n\
Simulator instproc satnode-geo-repeater {lon chan} {\n\
set tmp [$self satnode geo $lon]\n\
$self add-first-links $tmp gsl-repeater \"\" $chan\n\
return $tmp\n\
}\n\
\n\
Simulator instproc satnode-terminal {lat lon} {\n\
$self satnode terminal $lat $lon\n\
}\n\
\n\
Simulator instproc satnode args {\n\
$self instvar Node_\n\
set node [new Node/SatNode]\n\
if {[lindex $args 0] == \"polar\" || [lindex $args 0] == \"Polar\"} {\n\
set args [lreplace $args 0 0]\n\
$node set pos_ [new Position/Sat/Polar $args]\n\
$node cmd set_position [$node set pos_]\n\
[$node set pos_] setnode $node\n\
$node set hm_ [new HandoffManager/Sat]\n\
$node cmd set_handoff_mgr [$node set hm_]\n\
[$node set hm_] setnode $node\n\
$node create-ragent \n\
} elseif {[lindex $args 0] == \"geo\" || [lindex $args 0] == \"Geo\"} {  \n\
set args [lreplace $args 0 0]\n\
$node set pos_ [new Position/Sat/Geo $args]\n\
$node cmd set_position [$node set pos_]\n\
[$node set pos_] setnode $node\n\
$node create-ragent\n\
} elseif {[lindex $args 0] == \"geo-repeater\" || [lindex $args 0] == \"Geo-repeater\"} {  \n\
set args [lreplace $args 0 0]\n\
$node set pos_ [new Position/Sat/Geo $args]\n\
$node cmd set_position [$node set pos_]\n\
[$node set pos_] setnode $node\n\
} elseif {[lindex $args 0] == \"terminal\" || [lindex $args 0] == \"Terminal\"} {  \n\
set args [lreplace $args 0 0]\n\
$node set pos_ [new Position/Sat/Term $args]\n\
$node cmd set_position [$node set pos_]\n\
[$node set pos_] setnode $node\n\
$node set hm_ [new HandoffManager/Term]\n\
$node cmd set_handoff_mgr [$node set hm_]\n\
[$node set hm_] setnode $node\n\
$node create-ragent\n\
} else {\n\
puts \"Otcl error; satnode specified incorrectly: $args\"\n\
}\n\
set Node_([$node id]) $node\n\
$node set ns_ $self\n\
if [$self multicast?] {\n\
$node enable-mcast $self\n\
}\n\
$self check-node-num\n\
return $node\n\
}\n\
\n\
\n\
Simulator instproc add-first-links {node_ linktype linkargs chan} {\n\
$node_ set_downlink $chan\n\
$node_ set_uplink $chan\n\
if {$linktype == \"gsl-repeater\"} {\n\
$node_ add-repeater $chan\n\
} else {\n\
eval $node_ add-interface $linktype $linkargs\n\
}\n\
$node_ attach-to-outlink [$node_ set downlink_]\n\
$node_ attach-to-inlink [$node_ set uplink_]\n\
}\n\
\n\
Node/SatNode instproc add-gsl {ltype opt_ll opt_ifq opt_qlim opt_mac \\\n\
opt_bw opt_phy opt_inlink opt_outlink} {\n\
$self add-interface $ltype $opt_ll $opt_ifq $opt_qlim $opt_mac $opt_bw \\\n\
$opt_phy \n\
$self attach-to-inlink $opt_inlink\n\
$self attach-to-outlink $opt_outlink\n\
}\n\
\n\
Simulator instproc add-isl {ltype node1 node2 bw qtype qlim} {\n\
set opt_ll LL/Sat\n\
set opt_mac Mac/Sat\n\
set opt_phy Phy/Sat\n\
set opt_chan Channel/Sat\n\
set chan1 [new $opt_chan]\n\
set chan2 [new $opt_chan]\n\
$node1 add-interface $ltype $opt_ll $qtype $qlim $opt_mac $bw $opt_phy $chan1 $chan2\n\
$node2 add-interface $ltype $opt_ll $qtype $qlim $opt_mac $bw $opt_phy $chan2 $chan1\n\
if {$ltype == \"crossseam\"} {\n\
$node1 add-interface $ltype $opt_ll $qtype $qlim $opt_mac $bw $opt_phy \n\
$node2 add-interface $ltype $opt_ll $qtype $qlim $opt_mac $bw $opt_phy \n\
\n\
}\n\
}\n\
\n\
Node/SatNode instproc add-repeater chan { \n\
$self instvar nifs_ phy_tx_ phy_rx_ linkhead_ \n\
\n\
set t $nifs_\n\
incr nifs_\n\
\n\
set linkhead_($t) [new Connector/LinkHead/Sat]\n\
set phy_tx_($t)	[new Phy/Repeater]		;# interface\n\
set phy_rx_($t)	[new Phy/Repeater]\n\
\n\
$linkhead_($t) setnode $self\n\
$linkhead_($t) setphytx $phy_tx_($t)\n\
$linkhead_($t) setphyrx $phy_rx_($t)\n\
$linkhead_($t) set_type \"gsl-repeater\"\n\
$linkhead_($t) set type_ \"gsl-repeater\"\n\
\n\
$phy_rx_($t) up-target $phy_tx_($t)\n\
$phy_tx_($t) linkhead $linkhead_($t)\n\
$phy_rx_($t) linkhead $linkhead_($t)\n\
$phy_tx_($t) node $self		;# Bind node <---> interface\n\
$phy_rx_($t) node $self		;# Bind node <---> interface\n\
}\n\
\n\
Node/SatNode instproc add-interface args { \n\
\n\
$self instvar nifs_ phy_tx_ phy_rx_ mac_ ifq_ ll_ drophead_ linkhead_\n\
\n\
global ns_ MacTrace opt\n\
\n\
set t $nifs_\n\
incr nifs_\n\
\n\
set linkhead_($t) [new Connector/LinkHead/Sat]\n\
\n\
set linktype 	[lindex $args 0]\n\
set ll_($t)	[new [lindex $args 1]]		;# link layer\n\
set ifq_($t)	[new [lindex $args 2]]		;# interface queue\n\
set qlen	[lindex $args 3]\n\
set mac_($t)	[new [lindex $args 4]]		;# mac layer\n\
set mac_bw	[lindex $args 5]\n\
set phy_tx_($t)	[new [lindex $args 6]]		;# interface\n\
set phy_rx_($t)	[new [lindex $args 6]]		;# interface\n\
set inchan 	[lindex $args 7]\n\
set outchan 	[lindex $args 8]\n\
set drophead_($t) [new Connector]	;# drop target for queue\n\
set iif_($t) [new NetworkInterface]\n\
\n\
\n\
set linkhead $linkhead_($t)\n\
set phy_tx $phy_tx_($t)\n\
set phy_rx $phy_rx_($t)\n\
set mac $mac_($t)\n\
set ifq $ifq_($t)\n\
set ll $ll_($t)\n\
set drophead $drophead_($t)\n\
set iif $iif_($t)\n\
\n\
$linkhead setnode $self\n\
$linkhead setll $ll\n\
$linkhead setmac $mac\n\
$linkhead setqueue $ifq\n\
$linkhead setphytx $phy_tx\n\
$linkhead setphyrx $phy_rx\n\
$linkhead setnetinf $iif\n\
$self addlinkhead $linkhead; # Add NetworkInterface to node's list\n\
$linkhead target $ll; \n\
$linkhead set_type $linktype\n\
$linkhead set type_ $linktype\n\
\n\
$iif target [$self entry]\n\
\n\
$ll mac $mac; # XXX is this needed?\n\
$ll up-target $iif\n\
$ll down-target $ifq\n\
$ll set delay_ 0ms; # processing delay between ll and ifq\n\
\n\
$ifq target $mac\n\
$ifq set limit_ $qlen\n\
$drophead target [[Simulator instance] set nullAgent_]\n\
$ifq drop-target $drophead\n\
\n\
\n\
$mac netif $phy_tx; # Not used by satellite code at this time\n\
$mac up-target $ll\n\
$mac down-target $phy_tx\n\
$mac set bandwidth_ $mac_bw; \n\
\n\
$phy_rx up-target $mac\n\
$phy_tx linkhead $linkhead\n\
$phy_rx linkhead $linkhead\n\
$phy_tx node $self		;# Bind node <---> interface\n\
$phy_rx node $self		;# Bind node <---> interface\n\
\n\
if {$outchan != \"\" && $inchan != \"\"} {\n\
$phy_tx channel $outchan\n\
$phy_rx channel $inchan\n\
$inchan addif $phy_rx\n\
}\n\
return $t\n\
}\n\
\n\
Node/SatNode instproc set_uplink {chan} {\n\
$self instvar uplink_\n\
set uplink_ [new $chan]\n\
$self cmd set_uplink $uplink_\n\
}\n\
\n\
Node/SatNode instproc set_downlink {chan} {\n\
$self instvar downlink_\n\
set downlink_ [new $chan]\n\
$self cmd set_downlink $downlink_\n\
}\n\
\n\
Node/SatNode instproc attach-to-outlink {chan {index 0} } {\n\
$self instvar phy_tx_ mac_\n\
$phy_tx_($index) channel $chan\n\
}\n\
\n\
Node/SatNode instproc attach-to-inlink { chan {index 0}} {\n\
$self instvar phy_rx_ \n\
$phy_rx_($index) channel $chan\n\
$chan addif $phy_rx_($index)\n\
}\n\
\n\
Node/SatNode instproc interface-errormodel { em { index 0 } } {\n\
$self instvar mac_ ll_ em_ linkhead_\n\
$mac_($index) up-target $em\n\
$em target $ll_($index)\n\
$em drop-target [new Agent/Null]; # otherwise, packet is only marked\n\
set em_($index) $em\n\
$linkhead_($index) seterrmodel $em\n\
} \n\
\n\
\n\
Node/SatNode instproc create-ragent {} {\n\
set ragent [new Agent/SatRoute]\n\
$self attach $ragent 255; # attaches to default target of classifier  \n\
$ragent set myaddr_ [$self set id_]\n\
$self set_ragent $ragent; # sets pointer at C++ level\n\
$ragent set_node $self; # sets back pointer in ragent to node\n\
}\n\
\n\
Class Agent/rtProto/Dummy -superclass Agent/rtProto\n\
\n\
Agent/rtProto/Dummy proc init-all args {\n\
}\n\
\n\
\n\
Simulator instproc trace-all-satlinks {f} {\n\
$self instvar Node_\n\
foreach nn [array names Node_] {\n\
if {![$Node_($nn) info class Node/SatNode]} {\n\
continue; # Not a SatNode\n\
}\n\
$Node_($nn) trace-all-satlinks $f\n\
}\n\
}\n\
\n\
Node/SatNode instproc trace-all-satlinks {f} {\n\
$self instvar nifs_ enqT_ rcvT_ linkhead_\n\
for {set i 0} {$i < $nifs_} {incr i} {\n\
if {[$linkhead_($i) set type_] == \"gsl-repeater\"} {\n\
continue;\n\
}\n\
if {[info exists enqT_($i)]} {\n\
puts \"Tracing already exists on node [$self id]\"\n\
} else {\n\
$self trace-outlink-queue $f $i\n\
}\n\
if {[info exists rcvT_($i)]} {\n\
puts \"Tracing already exists on node [$self id]\"\n\
} else {\n\
$self trace-inlink-queue $f $i\n\
}\n\
}\n\
}\n\
\n\
Node/SatNode instproc trace-outlink-queue {f {index_ 0} } {\n\
$self instvar id_ enqT_ deqT_ drpT_ mac_ ll_ ifq_ drophead_ \n\
\n\
set ns [Simulator instance]\n\
set fromNode_ $id_\n\
set toNode_ -1\n\
\n\
set enqT_($index_) [$ns create-trace Sat/Enque $f $fromNode_ $toNode_]\n\
$enqT_($index_) target $ifq_($index_)\n\
$ll_($index_) down-target $enqT_($index_)\n\
\n\
set deqT_($index_) [$ns create-trace Sat/Deque $f $fromNode_ $toNode_]\n\
$deqT_($index_) target $mac_($index_)\n\
$ifq_($index_) target $deqT_($index_)\n\
\n\
set drpT_($index_) [$ns create-trace Sat/Drop $f $fromNode_ $toNode_]\n\
$drpT_($index_) target [$drophead_($index_) target]\n\
$drophead_($index_) target $drpT_($index_)\n\
$ifq_($index_) drop-target $drpT_($index_)\n\
}\n\
\n\
Node/SatNode instproc trace-inlink-queue {f {index_ 0} } {\n\
$self instvar id_ rcvT_ mac_ ll_ phy_rx_ em_ errT_    \n\
\n\
set ns [Simulator instance]\n\
set toNode_ $id_\n\
set fromNode_ -1\n\
\n\
if {[info exists em_($index_)]} {\n\
set errT_($index_) [$ns create-trace Sat/Error $f $fromNode_ $toNode_]\n\
$errT_($index_) target [$em_($index_) drop-target]\n\
$em_($index_) drop-target $errT_($index_)\n\
set rcvT_($index_) [$ns create-trace Sat/Recv $f $fromNode_ $toNode_]\n\
$rcvT_($index_) target [$em_($index_) target]\n\
$em_($index_) target $rcvT_($index_)\n\
} else {\n\
set rcvT_($index_) [$ns create-trace Sat/Recv $f $fromNode_ $toNode_]\n\
$rcvT_($index_) target [$mac_($index_) up-target]\n\
$mac_($index_) up-target $rcvT_($index_)\n\
}\n\
\n\
}\n\
\n\
\n\
\n\
\n\
Class Trace/Sat/Hop -superclass Trace/Sat\n\
Trace/Sat/Hop instproc init {} {\n\
$self next \"h\"\n\
}\n\
\n\
Class Trace/Sat/Enque -superclass Trace/Sat\n\
Trace/Sat/Enque instproc init {} {\n\
$self next \"+\"\n\
}\n\
\n\
Trace/Sat/Deque instproc init {} {\n\
$self next \"-\"\n\
}\n\
\n\
Class Trace/Sat/Recv -superclass Trace/Sat\n\
Trace/Sat/Recv instproc init {} {\n\
$self next \"r\"\n\
}\n\
\n\
Class Trace/Sat/Drop -superclass Trace/Sat\n\
Trace/Sat/Drop instproc init {} {\n\
$self next \"d\"\n\
}\n\
\n\
Class Trace/Sat/Error -superclass Trace/Sat\n\
Trace/Sat/Error instproc init {} {\n\
$self next \"e\"\n\
}\n\
\n\
Class Trace/Sat/Generic -superclass Trace/Sat\n\
Trace/Sat/Generic instproc init {} {\n\
$self next \"v\"\n\
}\n\
\n\
\n\
\n\
Node/SatNode set dist_routing_ \"false\"; # distributed routing not yet supported\n\
Position/Sat set time_advance_ 0; # time offset to start of simulation \n\
Position/Sat/Polar set plane_ 0\n\
HandoffManager/Term set elevation_mask_ 0\n\
HandoffManager/Term set term_handoff_int_ 10\n\
HandoffManager/Sat set sat_handoff_int_ 10\n\
HandoffManager/Sat set latitude_threshold_ 70\n\
HandoffManager/Sat set longitude_threshold_ 0\n\
HandoffManager set handoff_randomization_ \"false\" \n\
SatRouteObject set metric_delay_ \"true\"\n\
SatRouteObject set data_driven_computation_ \"false\"\n\
Mac/Sat/UnslottedAloha set mean_backoff_ 1s; # mean backoff time upon collision\n\
Mac/Sat/UnslottedAloha set rtx_limit_ 3; # Retransmission limit \n\
Mac/Sat/UnslottedAloha set send_timeout_ 270ms; # Timer interval for new sends\n\
\n\
Agent/SatRoute set myaddr_       0        ;# My address\n\
Mac/Sat set bandwidth_ 2Mb \n\
\n\
\n\
Simulator instproc set-nix-routing { } {\n\
Simulator set nix-routing 1\n\
Node enable-module \"Nix\"\n\
}\n\
\n\
Simulator instproc get-link-head { n1 n2 } {\n\
$self instvar link_\n\
return [$link_($n1:$n2) head]\n\
}\n\
\n\
Link instproc set-ipaddr { ipaddr netmask } {\n\
}\n\
\n\
\n\
\n\
proc mvar args {\n\
upvar self _s\n\
uplevel $_s instvar $args\n\
}\n\
\n\
Session/RTP set uniq_srcid 0\n\
Session/RTP proc alloc_srcid {} {\n\
set id [Session/RTP set uniq_srcid]\n\
Session/RTP set uniq_srcid [expr $id+1]\n\
return $id\n\
}\n\
\n\
Session/RTP instproc init {} {\n\
$self next \n\
mvar dchan_ cchan_\n\
set cchan_ [new Agent/RTCP]\n\
set dchan_ [new Agent/CBR/RTP]\n\
$dchan_ set packetSize_ 512\n\
\n\
$dchan_ session $self\n\
$cchan_ session $self\n\
\n\
$self set rtcp_timer_ [new RTCPTimer $self]\n\
\n\
mvar srcid_ localsrc_\n\
set srcid_ [Session/RTP alloc_srcid]\n\
set localsrc_ [new RTPSource $srcid_]\n\
$self localsrc $localsrc_\n\
\n\
$self set srctab_ $localsrc_\n\
$self set stopped_ 1\n\
}\n\
\n\
Session/RTP instproc start {} {\n\
mvar group_\n\
if ![info exists group_] {\n\
puts \"error: can't transmit before joining group!\"\n\
exit 1\n\
}\n\
\n\
mvar cchan_ \n\
$cchan_ start \n\
}\n\
\n\
Session/RTP instproc stop {} {\n\
$self instvar cchan_ dchan_\n\
$dchan_ stop\n\
$cchan_ stop\n\
$self set stopped_ 1\n\
}\n\
\n\
Session/RTP instproc report-interval { i } {\n\
mvar cchan_\n\
$cchan_ set interval_ $i\n\
}\n\
\n\
Session/RTP instproc bye {} {\n\
mvar cchan_ dchan_\n\
$dchan_ stop\n\
$cchan_ bye\n\
}\n\
\n\
Session/RTP instproc attach-node { node } {\n\
mvar dchan_ cchan_\n\
global ns\n\
$ns attach-agent $node $dchan_\n\
$ns attach-agent $node $cchan_\n\
\n\
$self set node_ $node\n\
}\n\
\n\
Session/RTP instproc detach-node { node } {\n\
mvar dchan_ cchan_\n\
global ns\n\
$ns detach-agent $node $dchan_\n\
$ns detach-agent $node $cchan_\n\
\n\
$self unset node_\n\
}\n\
\n\
Session/RTP instproc rtcp_timeout {} {\n\
mvar rtcp_timeout_callback_\n\
\n\
if [info exists rtcp_timeout_callback_] {\n\
eval $rtcp_timeout_callback_\n\
}\n\
}\n\
\n\
Session/RTP instproc join-group { g } {\n\
set g [expr $g]\n\
\n\
$self set group_ $g\n\
\n\
mvar node_ dchan_ cchan_ \n\
\n\
$dchan_ set dst_ $g\n\
$node_ join-group $dchan_ $g\n\
\n\
incr g\n\
\n\
$cchan_ set dst_ $g\n\
$node_ join-group $cchan_ $g\n\
}\n\
\n\
Session/RTP instproc leave-group { } {\n\
mvar group_ node_ cchan_ dchan_\n\
$node_ leave-group $dchan_ $group_\n\
$node_ leave-group $cchan_ [expr $group_+1]\n\
\n\
$self unset group_\n\
}\n\
\n\
Session/RTP instproc session_bw { bspec } {\n\
set b [bw_parse $bspec]\n\
\n\
$self set session_bw_ $b\n\
\n\
mvar rtcp_timer_\n\
$rtcp_timer_ session-bw $b\n\
}\n\
\n\
Session/RTP instproc transmit { bspec } {\n\
set b [bw_parse $bspec]\n\
\n\
\n\
$self set txBW_ $b\n\
\n\
$self instvar dchan_ stopped_\n\
if { $b == 0 } {\n\
$dchan_ stop\n\
set stopped_ 1\n\
}\n\
\n\
set ps [$dchan_ set packetSize_]\n\
$dchan_ set interval_  [expr 8.*$ps/$b]\n\
if { $stopped_ == 1 } {\n\
$dchan_ start\n\
set stopped_ 0\n\
} else {\n\
$dchan_ rate-change\n\
}\n\
}\n\
\n\
\n\
Session/RTP instproc sample-size { cc } {\n\
mvar rtcp_timer_\n\
$rtcp_timer_ sample-size $cc\n\
}\n\
\n\
Session/RTP instproc adapt-timer { nsrc nrr we_sent } {\n\
mvar rtcp_timer_\n\
$rtcp_timer_ adapt $nsrc $nrr $we_sent\n\
}\n\
\n\
Session/RTP instproc new-source { srcid } {\n\
set src [new RTPSource $srcid]\n\
$self enter $src\n\
\n\
mvar srctab_\n\
lappend srctab_ $src\n\
\n\
return $src\n\
}\n\
\n\
Class RTCPTimer \n\
\n\
RTCPTimer instproc init { session } {\n\
$self next\n\
\n\
\n\
mvar session_bw_fraction_ min_rpt_time_ inv_sender_bw_fraction_\n\
mvar inv_rcvr_bw_fraction_ size_gain_ avg_size_ inv_bw_\n\
\n\
set session_bw_fraction_ 0.05\n\
\n\
set min_rpt_time_ 1.   \n\
\n\
set sender_bw_fraction 0.25\n\
set rcvr_bw_fraction [expr 1. - $sender_bw_fraction]\n\
\n\
set inv_sender_bw_fraction_ [expr 1. / $sender_bw_fraction]\n\
set inv_rcvr_bw_fraction_ [expr 1. / $rcvr_bw_fraction]\n\
\n\
set size_gain_ 0.125	\n\
\n\
set avg_size_ 128.\n\
set inv_bw_ 0.\n\
\n\
mvar session_\n\
set session_ $session\n\
\n\
\n\
mvar min_rtp_time_ avg_size_ inv_bw_\n\
set rint [expr 8*$avg_size_ * $inv_bw_]\n\
\n\
set t [expr $min_rpt_time_ / 2.]\n\
\n\
if { $rint < $t } {\n\
set rint $t\n\
}\n\
\n\
$session_ report-interval $rint\n\
}\n\
\n\
RTCPTimer instproc sample-size { cc } {\n\
mvar avg_size_ size_gain_\n\
\n\
set avg_size_ [expr $avg_size_ + $size_gain_ * ($cc + 28 - $avg_size_)]\n\
}\n\
\n\
RTCPTimer instproc adapt { nsrc nrr we_sent } {\n\
mvar inv_bw_ avg_size_ min_rpt_time_\n\
mvar inv_sender_bw_fraction_ inv_rcvr_bw_fraction_\n\
\n\
\n\
set ibw $inv_bw_\n\
if { $nrr > 0 } {\n\
if { $we_sent } {\n\
set ibw [expr $ibw * $inv_sender_bw_fraction_]\n\
set nsrc $nrr\n\
} else {\n\
set ibw [expr $ibw * $inv_rcvr_bw_fraction_]\n\
incr nsrc -$nrr\n\
}\n\
}\n\
\n\
set rint [expr 8*$avg_size_ * $nsrc * $ibw]	\n\
if { $rint < $min_rpt_time_ } {\n\
set rint $min_rpt_time_\n\
}\n\
\n\
mvar session_\n\
$session_ report-interval $rint\n\
}\n\
\n\
RTCPTimer instproc session-bw { b } {\n\
$self set inv_bw_ [expr 1. / $b ]\n\
}\n\
\n\
Agent/RTCP set interval_ 0.\n\
Agent/RTCP set random_ 0\n\
Agent/RTCP set class_ 32\n\
\n\
RTPSource set srcid_ -1\n\
NetworkInterface set ifacenum_ 0\n\
NetworkInterface proc getid {} {\n\
$self instvar ifacenum_\n\
return [incr ifacenum_]\n\
}\n\
\n\
NetworkInterface instproc init {} {\n\
$self next\n\
$self cmd label [NetworkInterface getid]\n\
}\n\
\n\
Channel set delay_ 4us\n\
\n\
Classifier/Mac set bcast_ 0\n\
\n\
Mac set bandwidth_ 2Mb\n\
Mac set delay_ 0us\n\
\n\
if [TclObject is-class Mac/802_11] {\n\
Mac/802_11 set delay_ 64us\n\
Mac/802_11 set ifs_ 16us\n\
Mac/802_11 set slotTime_ 16us\n\
Mac/802_11 set cwmin_ 16\n\
Mac/802_11 set cwmax_ 1024\n\
Mac/802_11 set rtxLimit_ 16\n\
Mac/802_11 set bssId_ -1\n\
Mac/802_11 set sifs_ 8us\n\
Mac/802_11 set pifs_ 12us\n\
Mac/802_11 set difs_ 16us\n\
Mac/802_11 set rtxAckLimit_ 1\n\
Mac/802_11 set rtxRtsLimit_ 3\n\
}\n\
\n\
if [TclObject is-class Mac/Mcns] {\n\
Mac/Mcns set bandwidth_ 10Mb\n\
Mac/Mcns set hlen_ 6\n\
Mac/Mcns set bssId_ -1\n\
Mac/Mcns set slotTime_ 10us\n\
}\n\
\n\
if [TclObject is-class Mac/Multihop] {\n\
Mac/Multihop set bandwidth_ 100Kb\n\
Mac/Multihop set delay_ 10ms\n\
Mac/Multihop set tx_rx_ 11.125ms\n\
Mac/Multihop set rx_tx_ 13.25ms\n\
Mac/Multihop set rx_rx_ 10.5625\n\
Mac/Multihop set backoffBase_ 20ms\n\
Mac/Multihop set hlen_ 16\n\
}\n\
\n\
Mac instproc classify-macs {peerinfo} {\n\
set peerlabel [lindex $peerinfo 0]\n\
set peerll [lindex $peerinfo 1]\n\
$self instvar mclass_\n\
set mclass_ [new Classifier/Mac]\n\
$mclass_ install $peerlabel $peerll\n\
$self target $mclass_\n\
}\n\
\n\
Node instproc addmac {mac} { \n\
$self instvar machead_ mactail_\n\
\n\
if ![info exists mactail_] {\n\
set mactail_ [set machead_ $mac]\n\
$mac maclist $mactail_\n\
} else {\n\
$mactail_ maclist $mac\n\
$mac maclist $machead_\n\
set mactail_ $mac\n\
}\n\
}\n\
LL set bandwidth_ 0      ;# not used\n\
LL set delay_ 1ms\n\
LL set macDA_ 0\n\
\n\
\n\
if [TclObject is-class LL/Arq] {\n\
LL/Arq set mode_ 2\n\
LL/Arq set hlen_ 16\n\
LL/Arq set slen_ 1400\n\
LL/Arq set limit_ 8\n\
LL/Arq set timeout_ 100ms\n\
\n\
Class LL/Rlp -superclass LL/Arq\n\
LL/Rlp set mode_ 1\n\
LL/Rlp set hlen_ 6\n\
LL/Rlp set slen_ 30\n\
LL/Rlp set limit_ 63\n\
LL/Rlp set timeout_ 500ms\n\
LL/Rlp set delay_ 70ms\n\
}\n\
\n\
\n\
if [TclObject is-class Snoop] {\n\
Snoop set snoopTick_ 0.1\n\
Snoop set snoopDisable_ 0\n\
Snoop set srtt_ 0.1\n\
Snoop set rttvar_ 0.25\n\
Snoop set g_ 0.125\n\
Snoop set tailTime_ 0\n\
Snoop set rxmitStatus_ 0\n\
Snoop set lru_ 0\n\
Snoop set maxbufs_ 0\n\
}\n\
\n\
if [TclObject is-class LL/LLSnoop] {\n\
LL/LLSnoop set integrate_ 0\n\
LL/LLSnoop set delay_ 0ms\n\
Snoop set srtt_ 0.1\n\
Snoop set rttvar_ 0.25\n\
Snoop set g_ 0.125\n\
LL/LLSnoop set snoopTick_ 0.1\n\
}\n\
\n\
LL/LLSnoop instproc get-snoop { src dst } {\n\
$self instvar snoops_ off_ll_ delay_\n\
\n\
if { ![info exists snoops_($src:$dst)] } {\n\
set snoops_($src:$dst) [new Snoop]\n\
}\n\
$snoops_($src:$dst) llsnoop $self\n\
$snoops_($src:$dst) set delay_ $delay_\n\
return $snoops_($src:$dst)\n\
}\n\
\n\
LL/LLSnoop instproc integrate { src dst } {\n\
$self instvar snoops_\n\
\n\
set conn $src:$dst\n\
if {![info exists snoops_($conn)]} {\n\
return\n\
}\n\
\n\
set snoop $snoops_($conn)\n\
set threshtime [$snoop set tailTime_]\n\
\n\
foreach a [array names snoops_] {\n\
if { $a != $conn } {\n\
$snoops_($a) check-rxmit $threshtime\n\
if { [$snoops_($a) set rxmitStatus_] == 2 } {\n\
break;\n\
}\n\
}\n\
}\n\
}\n\
\n\
\n\
\n\
\n\
\n\
\n\
\n\
\n\
\n\
\n\
LanNode set ifqType_   Queue/DropTail\n\
LanNode set llType_    LL\n\
LanNode set macType_   Mac\n\
LanNode set chanType_  Channel\n\
LanNode set phyType_   Phy/WiredPhy\n\
LanNode set address_   \"\"\n\
LanNode set mactrace_   false\n\
\n\
LanNode instproc address  {val} { $self set address_  $val }\n\
LanNode instproc bw       {val} { $self set bw_       $val }\n\
LanNode instproc delay    {val} { $self set delay_    $val }\n\
LanNode instproc ifqType  {val} { $self set ifqType_  $val }\n\
LanNode instproc llType   {val} { $self set llType_   $val }\n\
LanNode instproc macType  {val} { $self set macType_  $val }\n\
LanNode instproc chanType {val} { $self set chanType_ $val }\n\
LanNode instproc phyType  {val} { $self set phyType_  $val }\n\
LanNode instproc mactrace    {val} { $self set mactrace_    $val }\n\
\n\
LanNode instproc init {ns args} {\n\
set args [eval $self init-vars $args]\n\
$self instvar bw_ delay_ ifqType_ llType_ macType_ chanType_\n\
$self instvar phyType_ mactrace_\n\
$self instvar ns_ nodelist_ defRouter_ cost_\n\
$self instvar id_ address_ channel_ mcl_ varp_\n\
$ns instvar Node_\n\
\n\
$self next\n\
set ns_ $ns\n\
set nodelist_ \"\"\n\
set cost_ 1\n\
\n\
set id_ [Node getid]\n\
$self nodeid $id_\n\
$ns_ add-lannode $self $id_\n\
set Node_($id_) $self\n\
if [Simulator hier-addr?] {\n\
if {$address_ == \"\"} {\n\
error \"LanNode: use \\\"-address\\\" option \\\n\
with hierarchical routing\"\n\
}\n\
} else {\n\
set address_ $id_\n\
}\n\
$self addr $address_\n\
set defRouter_ [new LanRouter $ns $self]\n\
if [$ns multicast?] {\n\
set switch_ [new Classifier/Hash/Dest 32]\n\
$switch_ set mask_ [AddrParams McastMask]\n\
$switch_ set shift_ [AddrParams McastShift]\n\
\n\
$defRouter_ switch $switch_\n\
}\n\
set channel_ [new $chanType_]\n\
set varp_ [new VARPTable]\n\
}\n\
\n\
LanNode instproc addNode {nodes bw delay {llType \"\"} {ifqType \"\"} \\\n\
{macType \"\"} {phyType \"\"} {mactrace \"\"}} {\n\
$self instvar ifqType_ llType_ macType_ chanType_ phyType_ mactrace_\n\
$self instvar id_ channel_ mcl_ lanIface_\n\
$self instvar ns_ nodelist_ cost_ varp_\n\
$ns_ instvar link_ Node_ \n\
\n\
if {$ifqType == \"\"} { set ifqType $ifqType_ }\n\
if {$macType == \"\"} { set macType $macType_ }\n\
if {$llType  == \"\"} { set llType $llType_ }\n\
if {$phyType  == \"\"} { set phyType $phyType_ }\n\
if {$mactrace == \"\"}  { set mactrace $mactrace_ }\n\
\n\
set vlinkcost [expr $cost_ / 2.0]\n\
foreach src $nodes {\n\
set nif [new LanIface $src $self \\\n\
-ifqType $ifqType \\\n\
-llType  $llType \\\n\
-macType $macType \\\n\
-phyType $phyType \\\n\
-mactrace $mactrace ]\n\
\n\
set tr [$ns_ get-ns-traceall]\n\
if {$tr != \"\"} {\n\
$nif trace $ns_ $tr\n\
}\n\
set tr [$ns_ get-nam-traceall]\n\
if {$tr != \"\"} {\n\
$nif nam-trace $ns_ $tr\n\
}\n\
\n\
\n\
set ll [$nif set ll_]\n\
$ll set delay_ $delay\n\
$ll varp $varp_\n\
\n\
$varp_ mac-addr [[$nif set node_] id] \\\n\
[[$nif set mac_] id]\n\
\n\
set phy [$nif set phy_]\n\
$phy node $src\n\
$phy channel $channel_\n\
$channel_ addif $phy\n\
$phy set bandwidth_ $bw\n\
\n\
set lanIface_($src) $nif\n\
\n\
$src add-neighbor $self\n\
\n\
set sid [$src id]\n\
set link_($sid:$id_) [new Vlink $ns_ $self $src  $self $bw 0]\n\
set link_($id_:$sid) [new Vlink $ns_ $self $self $src  $bw 0]\n\
\n\
$src add-oif [$link_($sid:$id_) head]  $link_($sid:$id_)\n\
$src add-iif [[$nif set iface_] label] $link_($id_:$sid)\n\
[$link_($sid:$id_) head] set link_ $link_($sid:$id_)\n\
\n\
$link_($sid:$id_) queue [$nif set ifq_]\n\
$link_($id_:$sid) queue [$nif set ifq_]\n\
\n\
$link_($sid:$id_) set iif_ [$nif set iface_]\n\
$link_($id_:$sid) set iif_ [$nif set iface_]\n\
\n\
$link_($sid:$id_) cost $vlinkcost\n\
$link_($id_:$sid) cost $vlinkcost\n\
}\n\
set nodelist_ [concat $nodelist_ $nodes]\n\
}\n\
\n\
LanNode instproc assign-mac {ip} {\n\
return $ip ;# use ip addresses at MAC layer\n\
}\n\
\n\
LanNode instproc cost c {\n\
$self instvar ns_ nodelist_ id_ cost_\n\
$ns_ instvar link_\n\
set cost_ $c\n\
set vlinkcost [expr $c / 2.0]\n\
foreach node $nodelist_ {\n\
set nid [$node id]\n\
$link_($id_:$nid) cost $vlinkcost\n\
$link_($nid:$id_) cost $vlinkcost\n\
}\n\
}\n\
\n\
LanNode instproc cost? {} {\n\
$self instvar cost_\n\
return $cost_\n\
}\n\
\n\
LanNode instproc rtObject? {} {\n\
}\n\
\n\
LanNode instproc id {} { $self set id_ }\n\
\n\
LanNode instproc node-addr {{addr \"\"}} { \n\
eval $self set address_ $addr\n\
}\n\
\n\
LanNode instproc reset {} {\n\
}\n\
\n\
LanNode instproc is-lan? {} { return 1 }\n\
\n\
LanNode instproc dump-namconfig {} {\n\
$self instvar ns_ bw_ delay_ nodelist_ id_\n\
$ns_ puts-nam-config \\\n\
\"X -t * -n $id_ -r $bw_ -D $delay_ -o left\"\n\
set cnt 0\n\
set LanOrient(0) \"up\"\n\
set LanOrient(1) \"down\"\n\
\n\
foreach n $nodelist_ {\n\
$ns_ puts-nam-config \\\n\
\"L -t * -s $id_ -d [$n id] -o $LanOrient($cnt)\"\n\
set cnt [expr 1 - $cnt]\n\
}\n\
}\n\
\n\
LanNode instproc init-outLink {} { \n\
}\n\
\n\
LanNode instproc start-mcast {} { \n\
}\n\
\n\
LanNode instproc getArbiter {} {\n\
}\n\
\n\
LanNode instproc attach {agent} {\n\
}\n\
\n\
LanNode instproc sp-add-route {args} {\n\
}\n\
\n\
LanNode instproc add-route {args} {\n\
}\n\
\n\
LanNode instproc add-hroute {args} {\n\
}\n\
\n\
Class LanIface \n\
LanIface set ifqType_ Queue/DropTail\n\
LanIface set macType_ Mac\n\
LanIface set llType_  LL\n\
LanIface set phyType_  Phy/WiredPhy\n\
LanIface set mactrace_ false\n\
\n\
LanIface instproc llType {val} { $self set llType_ $val }\n\
LanIface instproc ifqType {val} { $self set ifqType_ $val }\n\
LanIface instproc macType {val} { $self set macType_ $val }\n\
LanIface instproc phyType {val} { $self set phyType_ $val }\n\
LanIface instproc mactrace {val} { $self set mactrace_ $val }\n\
\n\
LanIface instproc entry {} { $self set entry_ }\n\
LanIface instproc init {node lan args} {\n\
set args [eval $self init-vars $args]\n\
eval $self next $args\n\
\n\
$self instvar llType_ ifqType_ macType_ phyType_ mactrace_\n\
$self instvar node_ lan_ ifq_ mac_ ll_ phy_\n\
$self instvar iface_ entry_ drophead_\n\
\n\
set node_ $node\n\
set lan_ $lan\n\
\n\
set ll_ [new $llType_]\n\
set ifq_ [new $ifqType_]\n\
set mac_ [new $macType_]\n\
if {[string compare $macType_ \"Mac/802_3\"] == 0} {\n\
$mac_ set trace_ $mactrace_\n\
}\n\
set iface_ [new NetworkInterface]\n\
set phy_ [new $phyType_]\n\
\n\
set entry_ [new Connector]\n\
set drophead_ [new Connector]\n\
\n\
$ll_ set macDA_ -1	;# bcast address if there is no LAN router\n\
$ll_ lanrouter [$lan set defRouter_]\n\
$ll_ up-target $iface_\n\
$ll_ down-target $ifq_\n\
$ll_ mac $mac_\n\
$ll_ ifq $ifq_\n\
\n\
$ifq_ target $mac_\n\
\n\
$mac_ up-target $ll_\n\
$mac_ down-target $phy_\n\
$mac_ netif $phy_\n\
\n\
$phy_ up-target $mac_\n\
\n\
$iface_ target [$node entry]\n\
$entry_ target $ll_\n\
\n\
set ns [Simulator instance]\n\
\n\
$drophead_ target [$ns set nullAgent_]\n\
\n\
$ifq_ drop-target $drophead_ \n\
$mac_ drop-target $drophead_ \n\
$ll_ drop-target $drophead_\n\
}\n\
\n\
LanIface instproc trace {ns f {op \"\"}} {\n\
$self instvar hopT_ rcvT_ enqT_ deqT_ drpT_ \n\
$self instvar iface_ entry_ node_ lan_ drophead_ \n\
$self instvar ll_ ifq_ mac_ mactrace_\n\
\n\
set hopT_ [$ns create-trace Hop   $f $node_ $lan_  $op]\n\
set rcvT_ [$ns create-trace Recv  $f $lan_  $node_ $op]\n\
set enqT_ [$ns create-trace Enque $f $node_ $lan_  $op]\n\
set deqT_ [$ns create-trace Deque $f $node_ $lan_  $op]\n\
set drpT_ [$ns create-trace Drop  $f $node_ $lan_  $op]\n\
if {[string compare $mactrace_ \"true\"] == 0} {\n\
set macdrpT_ [$ns create-trace Collision $f $node_ $lan_ $op]\n\
set macdrophead_ [new Connector]\n\
$mac_ drop-target $macdrophead_\n\
$macdrophead_ target $macdrpT_\n\
}\n\
\n\
$hopT_ target [$entry_ target]\n\
$entry_ target $hopT_\n\
\n\
$rcvT_ target [$iface_ target]\n\
$iface_ target $rcvT_\n\
\n\
$enqT_ target [$ll_ down-target]\n\
$ll_ down-target $enqT_\n\
\n\
$deqT_ target [$ifq_ target]\n\
$ifq_ target $deqT_\n\
\n\
$drpT_ target [$drophead_ target]\n\
$drophead_ target $drpT_\n\
}\n\
LanIface instproc nam-trace {ns f} {\n\
$self instvar hopT_ rcvT_ enqT_ deqT_ drpT_ \n\
if [info exists hopT_] {\n\
$hopT_ namattach $f\n\
} else {\n\
$self trace $ns $f \"nam\"\n\
}\n\
$rcvT_ namattach $f\n\
$enqT_ namattach $f\n\
$deqT_ namattach $f\n\
$drpT_ namattach $f\n\
}\n\
LanIface instproc add-receive-filter filter {\n\
$self instvar mac_\n\
$filter target [$mac_ target]\n\
$mac_ target $filter\n\
}\n\
\n\
\n\
Class Vlink\n\
Vlink instproc up? {} {\n\
return \"up\"\n\
}\n\
Vlink instproc queue {{q \"\"}} {\n\
eval $self set queue_ $q\n\
}\n\
Vlink instproc init {ns lan src dst b d} {\n\
$self instvar ns_ lan_ src_ dst_ bw_ delay_\n\
\n\
set ns_ $ns\n\
set lan_ $lan\n\
set src_ $src\n\
set dst_ $dst\n\
set bw_ $b\n\
set delay_ $d\n\
}\n\
Vlink instproc src {}	{ $self set src_	}\n\
Vlink instproc dst {}	{ $self set dst_	}\n\
Vlink instproc dump-nam-queueconfig {} {\n\
}\n\
Vlink instproc head {} {\n\
$self instvar lan_ dst_ src_\n\
if {$src_ == $lan_ } {\n\
return \"\"\n\
} else {\n\
set src_lif [$lan_ set lanIface_($src_)]\n\
return [$src_lif entry]\n\
}\n\
}\n\
Vlink instproc cost c { $self set cost_ $c}	\n\
Vlink instproc cost? {} {\n\
$self instvar cost_\n\
if ![info exists cost_] {\n\
return 1\n\
}\n\
return $cost_\n\
}\n\
\n\
\n\
LanRouter instproc init {ns lan} {\n\
$self next\n\
if [Simulator hier-addr?] {\n\
$self routing hier\n\
} else {\n\
$self routing flat\n\
}\n\
$self lanaddr [$lan node-addr]\n\
$self routelogic [$ns get-routelogic]\n\
}\n\
\n\
\n\
Node instproc is-lan? {} { return 0 }\n\
\n\
Simulator instproc newLan {nodelist bw delay args} {\n\
set lan [eval new LanNode $self -bw $bw -delay $delay $args]\n\
$lan addNode $nodelist $bw $delay\n\
return $lan\n\
}\n\
\n\
\n\
Simulator instproc make-lan { args } {\n\
\n\
set t [lindex $args 0]\n\
set mactrace \"false\"\n\
if { $t == \"-trace\" } {\n\
set mactrace [lindex $args 1]\n\
if {$mactrace == \"on\" } {\n\
set mactrace \"true\"\n\
}\n\
\n\
}\n\
\n\
if { $t == \"-trace\" } {\n\
set nodelist [lindex $args 2]\n\
set bw [lindex $args 3]\n\
set delay [lindex $args 4]\n\
set llType [lindex $args 5]\n\
set ifqType [lindex $args 6]\n\
set macType [lindex $args 7]\n\
set chanType [lindex $args 8]\n\
set phyType [lindex $args 9]\n\
} else {\n\
set nodelist [lindex $args 0]\n\
set bw [lindex $args 1]\n\
set delay [lindex $args 2]\n\
set llType [lindex $args 3]\n\
set ifqType [lindex $args 4]\n\
set macType [lindex $args 5]\n\
set chanType [lindex $args 6]\n\
set phyType [lindex $args 7]\n\
}\n\
\n\
if { $llType == \"\" } {\n\
set llType \"LL\"\n\
}\n\
if { $ifqType == \"\" } {\n\
set ifqtype \"Queue/DropTail\"\n\
}\n\
if { $macType == \"\" } {\n\
set macType \"Mac\"\n\
}\n\
if { $chanType == \"\" } {\n\
set chanType \"Channel\"\n\
}\n\
if { $phyType == \"\"} {\n\
set phyType \"Phy/WiredPhy\"\n\
}\n\
\n\
if {[string compare $macType \"Mac/Csma/Cd\"] == 0} {\n\
puts \"Warning: Mac/Csma/Cd is out of date\"\n\
puts \"Warning: Please use Mac/802_3 to replace Mac/Csma/Cd\"\n\
set macType \"Mac/802_3\"\n\
}\n\
\n\
set lan [new LanNode $self \\\n\
-bw $bw \\\n\
-delay $delay \\\n\
-llType $llType \\\n\
-ifqType $ifqType \\\n\
-macType $macType \\\n\
-chanType $chanType \\\n\
-phyType $phyType \\\n\
-mactrace $mactrace]\n\
$lan addNode $nodelist $bw $delay $llType $ifqType $macType \\\n\
$phyType $mactrace\n\
\n\
return $lan\n\
}\n\
\n\
\n\
\n\
\n\
\n\
\n\
\n\
\n\
\n\
\n\
\n\
\n\
AbsLanNode set address_   \"\"\n\
\n\
AbsLanNode instproc address  {val} { $self set address_  $val }\n\
AbsLanNode instproc bw       {val} { $self set bw_       $val }\n\
AbsLanNode instproc delay    {val} { $self set delay_    $val }\n\
AbsLanNode instproc qlen     {val} { $self set qlen_     $val }\n\
\n\
AbsLanNode instproc init {ns args} {\n\
set args [eval $self init-vars $args]\n\
$self instvar bw_ delay_ qlen_\n\
$self instvar ns_ nodelist_ defRouter_ cost_\n\
$self instvar id_ address_ q_ dlink_ mcl_ varp_\n\
$ns instvar Node_\n\
\n\
$self next\n\
set ns_ $ns\n\
set nodelist_ \"\"\n\
set cost_ 1\n\
\n\
set id_ [Node getid]\n\
$ns_ add-abslan-node $self $id_\n\
$self nodeid $id_	;# Propagate id_ into c++ space\n\
set Node_($id_) $self\n\
set address_ $id_       ;# won't work for hier rtg!\n\
set defRouter_ [new LanRouter $ns $self]\n\
if [$ns multicast?] {\n\
set switch_ [new Classifier/Hash/Dest 32]\n\
$switch_ set mask_ [AddrParams set McastMask_]\n\
$switch_ set shift_ [AddrParams set McastShift_]\n\
\n\
$defRouter_ switch $switch_\n\
}\n\
\n\
set varp_ [new VARPTable]\n\
\n\
set q_ [new Queue/DropTail]\n\
set dlink_ [new DelayLink]\n\
$dlink_ set bandwidth_ $bw_\n\
$dlink_ set delay_ $delay_\n\
set mcl_ [new Classifier/Replicator]\n\
$mcl_ set offset_ [PktHdr_offset PacketHeader/Mac macDA_]\n\
$mcl_ set direction_ true\n\
$q_ target $dlink_\n\
$q_ set limit_ $qlen_\n\
$dlink_ target $mcl_\n\
\n\
}\n\
\n\
AbsLanNode instproc addNode {nodes} {\n\
$self instvar id_ lanIface_\n\
$self instvar q_ ns_ nodelist_ cost_ varp_ \n\
$self instvar dlink_ mcl_ bw_\n\
$self instvar deqT_\n\
$ns_ instvar link_ Node_ \n\
\n\
\n\
set vlinkcost [expr $cost_ / 2.0]\n\
foreach src $nodes {\n\
set nif [new AbsLanIface $src $self]\n\
\n\
set tr [$ns_ get-ns-traceall]\n\
if {$tr != \"\"} {\n\
$nif trace $ns_ $tr\n\
}\n\
\n\
set tr [$ns_ get-nam-traceall]\n\
if {$tr != \"\"} {\n\
$nif nam-trace $ns_ $tr\n\
}\n\
\n\
\n\
$mcl_ installNext [$nif set mac_]\n\
$varp_ mac-addr [[$nif set node_] id] \\\n\
[[$nif set mac_] id] \n\
\n\
$q_ drop-target [$nif set drophead_]\n\
set lanIface_($src) $nif\n\
\n\
$src add-neighbor $self\n\
\n\
set sid [$src id]\n\
\n\
set link_($sid:$id_) [new Vlink $ns_ $self $src  $self $bw_ 0]\n\
set link_($id_:$sid) [new Vlink $ns_ $self $self $src  $bw_ 0]\n\
\n\
[$link_($sid:$id_) head] set link_ $link_($sid:$id_)\n\
\n\
$link_($sid:$id_) queue [$self set q_ ]\n\
$link_($id_:$sid) queue [$self set q_ ]\n\
\n\
\n\
$link_($sid:$id_) cost $vlinkcost\n\
$link_($id_:$sid) cost $vlinkcost\n\
}\n\
\n\
set nodelist_ [concat $nodelist_ $nodes]\n\
\n\
set f [$ns_ get-ns-traceall]\n\
set deqT_ [$ns_ create-trace Deque  $f $src $self ]\n\
\n\
$deqT_ target $dlink_\n\
$q_ target $deqT_\n\
\n\
}\n\
\n\
AbsLanNode instproc assign-mac {ip} {\n\
return $ip ;# use ip addresses at MAC layer\n\
}\n\
\n\
AbsLanNode instproc cost c {\n\
$self instvar ns_ nodelist_ id_ cost_\n\
$ns_ instvar link_\n\
set cost_ $c\n\
set vlinkcost [expr $c / 2.0]\n\
foreach node $nodelist_ {\n\
set nid [$node id]\n\
$link_($id_:$nid) cost $vlinkcost\n\
$link_($nid:$id_) cost $vlinkcost\n\
}\n\
}\n\
\n\
AbsLanNode instproc cost? {} {\n\
$self instvar cost_\n\
return $cost_\n\
}\n\
\n\
AbsLanNode instproc rtObject? {} {\n\
}\n\
\n\
AbsLanNode instproc id {} { $self set id_ }\n\
\n\
AbsLanNode instproc node-addr {{addr \"\"}} { \n\
eval $self set address_ $addr\n\
}\n\
\n\
AbsLanNode instproc reset {} {\n\
}\n\
\n\
AbsLanNode instproc is-lan? {} { return 1 }\n\
\n\
AbsLanNode instproc dump-namconfig {} {\n\
$self instvar ns_ bw_ delay_ nodelist_ id_\n\
$ns_ puts-nam-config \\\n\
\"X -t * -n $id_ -r $bw_ -D $delay_ -o left\"\n\
set cnt 0\n\
set LanOrient(0) \"up\"\n\
set LanOrient(1) \"down\"\n\
\n\
foreach n $nodelist_ {\n\
$ns_ puts-nam-config \\\n\
\"L -t * -s $id_ -d [$n id] -o $LanOrient($cnt)\"\n\
set cnt [expr 1 - $cnt]\n\
}\n\
}\n\
\n\
AbsLanNode instproc init-outLink {} { \n\
}\n\
\n\
AbsLanNode instproc start-mcast {} { \n\
}\n\
\n\
AbsLanNode instproc getArbiter {} {\n\
}\n\
\n\
AbsLanNode instproc attach {agent} {\n\
}\n\
\n\
AbsLanNode instproc sp-add-route {args} {\n\
}\n\
\n\
AbsLanNode instproc add-route {args} {\n\
}\n\
\n\
AbsLanNode instproc add-hroute {args} {\n\
}\n\
\n\
AbsLanNode instproc split-addrstr addrstr {\n\
set L [split $addrstr .]\n\
return $L\n\
}\n\
\n\
\n\
\n\
Class AbsLanIface \n\
\n\
AbsLanIface instproc entry {} { $self set entry_ }\n\
\n\
AbsLanIface instproc init {node lan } {\n\
\n\
$self next \n\
\n\
$self instvar node_ lan_ \n\
$self instvar entry_ mac_ ll_ \n\
$self instvar drophead_\n\
\n\
set node_ $node\n\
set lan_ $lan\n\
\n\
set entry_ [new Connector]\n\
set ll_ [new LL]\n\
set mac_ [new Mac]\n\
$mac_ set abstract_ true\n\
\n\
$entry_ target $ll_\n\
\n\
$ll_ mac $mac_\n\
$ll_ up-target [$node entry]\n\
$ll_ down-target $mac_\n\
$ll_ set macDA_ -1\n\
$ll_ set delay_ 0\n\
$ll_ lanrouter [$lan set defRouter_]\n\
$ll_ varp [$lan set varp_]\n\
\n\
$mac_ up-target $ll_\n\
$mac_ down-target [$lan set q_]\n\
$mac_ set delay_ 0\n\
\n\
set ns [Simulator instance]\n\
set drophead_ [new Connector]\n\
$drophead_ target [$ns set nullAgent_]\n\
\n\
$mac_ drop-target $drophead_\n\
$ll_ drop-target $drophead_\n\
}\n\
\n\
AbsLanIface instproc trace {ns f {op \"\"}} {\n\
$self instvar hopT_ rcvT_ enqT_ drpT_ deqT_ \n\
$self instvar iface_ entry_ node_ lan_ drophead_ \n\
$self instvar ll_ mac_ \n\
\n\
set hopT_ [$ns create-trace Hop   $f $node_ $lan_  $op]\n\
set rcvT_ [$ns create-trace Recv  $f $lan_  $node_ $op]\n\
set enqT_ [$ns create-trace Enque $f $node_ $lan_  $op]\n\
set drpT_ [$ns create-trace Drop  $f $node_ $lan_  $op]\n\
\n\
$hopT_ target [$entry_ target]\n\
$entry_ target $hopT_\n\
\n\
$rcvT_ target [$ll_ up-target]\n\
$ll_ up-target $rcvT_\n\
\n\
$enqT_ target [$mac_ down-target]\n\
$mac_ down-target $enqT_\n\
\n\
\n\
$drpT_ target [$drophead_ target]\n\
$drophead_ target $drpT_\n\
}\n\
\n\
AbsLanIface instproc nam-trace {ns f} {\n\
$self instvar hopT_ rcvT_ enqT_  drpT_ deqT_\n\
if [info exists hopT_] {\n\
$hopT_ namattach $f\n\
} else {\n\
$self trace $ns $f \"nam\"\n\
}\n\
$rcvT_ namattach $f\n\
$enqT_ namattach $f\n\
$drpT_ namattach $f\n\
}\n\
\n\
Simulator instproc make-abslan {nodelist bw delay {qlen 50}} {\n\
set lan [new AbsLanNode $self \\\n\
-bw $bw \\\n\
-delay $delay \\\n\
-qlen $qlen]\n\
$lan addNode $nodelist \n\
return $lan\n\
}\n\
\n\
\n\
\n\
\n\
\n\
\n\
\n\
\n\
\n\
Class Timer\n\
\n\
Timer instproc init { ns } {\n\
$self set ns_ $ns\n\
}\n\
\n\
Timer instproc sched delay {\n\
$self instvar ns_\n\
$self instvar id_\n\
$self cancel\n\
set id_ [$ns_ after $delay \"$self timeout\"]\n\
}\n\
\n\
Timer instproc destroy {} {\n\
$self cancel\n\
}\n\
\n\
Timer instproc cancel {} {\n\
$self instvar ns_\n\
$self instvar id_\n\
if [info exists id_] {\n\
$ns_ cancel $id_\n\
unset id_\n\
}\n\
}\n\
\n\
Timer instproc resched delay {\n\
$self sched $delay \n\
}\n\
\n\
Timer instproc expire {} {\n\
$self timeout\n\
}\n\
\n\
\n\
Class Timer/Iface -superclass Timer\n\
\n\
Timer/Iface instproc init { protocol source group oiface sim} {\n\
$self instvar proto_ src_ grp_ oif_\n\
$self next $sim\n\
set proto_ $protocol\n\
set src_ $source\n\
set grp_ $group\n\
set oif_ $oiface\n\
}\n\
\n\
Timer/Iface instproc schedule {} {\n\
$self sched [[$self info class] set timeout]\n\
}\n\
\n\
\n\
Class MultiSim -superclass Simulator\n\
\n\
MultiSim instproc init args {\n\
eval $self next $args\n\
$self multicast on\n\
}\n\
\n\
Simulator instproc multicast args {\n\
$self set multiSim_ 1\n\
Node enable-module Mcast\n\
}\n\
\n\
Simulator instproc multicast? {} {\n\
$self instvar multiSim_\n\
if { ![info exists multiSim_] } {\n\
set multiSim_ 0\n\
}\n\
set multiSim_\n\
}\n\
\n\
Simulator instproc run-mcast {} {\n\
$self instvar Node_\n\
foreach n [array names Node_] {\n\
set node $Node_($n)\n\
$node start-mcast\n\
}\n\
$self next\n\
}\n\
\n\
Simulator instproc clear-mcast {} {\n\
$self instvar Node_\n\
foreach n [array names Node_] {\n\
$Node_($n) stop-mcast\n\
}\n\
}\n\
\n\
Simulator instproc mrtproto { mproto { nodelist \"\" } } {\n\
$self instvar Node_ MrtHandle_\n\
\n\
set MrtHandle_ \"\"\n\
if { $mproto == \"CtrMcast\" } {\n\
set MrtHandle_ [new CtrMcastComp $self]\n\
$MrtHandle_ set ctrrpcomp [new CtrRPComp $self]\n\
}\n\
\n\
if { $mproto == \"BST\" } {\n\
foreach n [array names Node_] {\n\
if ![$Node_($n) is-lan?] {\n\
$Node_($n) instvar multiclassifier_ switch_\n\
set multiclassifier_ [new Classifier/Multicast/Replicator/BST]\n\
$multiclassifier_ set node_ $Node_($n)\n\
$switch_ install 1 $multiclassifier_\n\
}\n\
}\n\
}\n\
\n\
if { $nodelist == \"\" } {\n\
foreach n [array names Node_] {\n\
$self mrtproto-iifs $mproto $Node_($n) \"\"\n\
}\n\
} else {\n\
foreach node $nodelist {\n\
$self mrtproto-iifs $mproto $node \"\"\n\
}\n\
}\n\
$self at 0.0 \"$self run-mcast\"\n\
\n\
return $MrtHandle_\n\
}\n\
Simulator instproc mrtproto-iifs {mproto node iiflist } {\n\
set mh [new $mproto $self $node]\n\
set arbiter [$node getArbiter]\n\
if { $arbiter != \"\" } {\n\
$arbiter addproto $mh $iiflist\n\
}\n\
}\n\
\n\
Node proc allocaddr {} {\n\
set addr [Simulator set McastAddr_]\n\
Simulator set McastAddr_ [expr $addr + 1]\n\
return $addr\n\
}\n\
\n\
Node proc expandaddr {} {\n\
[Simulator instance] set-address-format expanded\n\
puts \"Backward compatibility: Use \\\"set-address-format expanded\\\" instead of \\\"Node expandaddr\\\";\" \n\
}\n\
\n\
Node instproc start-mcast {} {\n\
$self instvar mrtObject_\n\
$mrtObject_ start\n\
}\n\
\n\
Node instproc getArbiter {} {\n\
$self instvar mrtObject_\n\
if [info exists mrtObject_] {\n\
return $mrtObject_\n\
}\n\
return \"\"\n\
}\n\
\n\
Node instproc notify-mcast changes {\n\
$self instvar mrtObject_\n\
if [info exists mrtObject_] {\n\
$mrtObject_ notify $changes\n\
}\n\
}\n\
\n\
Node instproc stop-mcast {} {\n\
$self instvar mrtObject_\n\
$self clear-caches\n\
$mrtObject_ stop\n\
}\n\
\n\
Node instproc clear-caches {} {\n\
$self instvar Agents_  multiclassifier_ replicator_\n\
\n\
$multiclassifier_ clearAll\n\
$multiclassifier_ set nrep_ 0\n\
\n\
foreach var {Agents_ replicator_} {\n\
$self instvar $var\n\
if { [info exists $var] } {\n\
delete $var\n\
unset $var\n\
}\n\
}\n\
}\n\
\n\
Node instproc dump-routes args {\n\
$self instvar mrtObject_\n\
if { [info exists mrtObject_] } {\n\
eval $mrtObject_ dump-routes $args\n\
}\n\
}\n\
\n\
Node instproc check-local { group } {\n\
$self instvar Agents_\n\
if [info exists Agents_($group)] {\n\
return [llength $Agents_($group)]\n\
}\n\
return 0\n\
}\n\
\n\
Node instproc new-group { src group iface code } {\n\
$self instvar mrtObject_\n\
$mrtObject_ upcall $code $src $group $iface\n\
}\n\
\n\
Node instproc join-group { agent group { src \"\" } } {\n\
$self instvar replicator_ Agents_ mrtObject_\n\
set group [expr $group] ;# use expr to convert to decimal\n\
\n\
$mrtObject_ join-group $group $src\n\
\n\
lappend Agents_($group) $agent\n\
if { $src == \"\" } {\n\
set reps [$self getReps \"*\" $group]\n\
} else {\n\
set reps [$self getReps $src $group]\n\
}\n\
foreach rep $reps {\n\
$rep insert $agent\n\
}\n\
}\n\
\n\
Node instproc leave-group { agent group { src \"\" } } {\n\
$self instvar replicator_ Agents_ mrtObject_\n\
set group [expr $group] ;# use expr to get rid of possible leading 0x\n\
if { $src == \"\" } {\n\
set reps [$self getReps \"*\" $group]\n\
} else {\n\
set reps [$self getReps $src $group]\n\
}\n\
foreach rep $reps  {\n\
$rep disable $agent\n\
}\n\
if [info exists Agents_($group)] {\n\
set k [lsearch -exact $Agents_($group) $agent]\n\
set Agents_($group) [lreplace $Agents_($group) $k $k]\n\
\n\
$mrtObject_ leave-group $group $src\n\
} else {\n\
warn \"cannot leave a group without joining it\"\n\
}\n\
}\n\
\n\
Node instproc add-mfc { src group iif oiflist } {\n\
$self instvar multiclassifier_ \\\n\
replicator_ Agents_ \n\
\n\
if [info exists replicator_($src:$group)] {\n\
set r $replicator_($src:$group)\n\
} else {\n\
set r [new Classifier/Replicator/Demuxer]\n\
$r set srcID_ $src\n\
$r set grp_ $group\n\
set replicator_($src:$group) $r\n\
$r set node_ $self\n\
if [info exists Agents_($group)] {\n\
foreach a $Agents_($group) {\n\
$r insert $a\n\
}\n\
}\n\
if [info exists Agents_($src:$group)] {\n\
foreach a $Agents_($src:$group) {\n\
$r insert $a\n\
}\n\
}\n\
$multiclassifier_ add-rep $r $src $group $iif\n\
}\n\
\n\
foreach oif [lsort $oiflist] {\n\
$r insert $oif\n\
}\n\
}\n\
\n\
Node instproc del-mfc { srcID group oiflist } {\n\
$self instvar replicator_ multiclassifier_\n\
if [info exists replicator_($srcID:$group)] {\n\
set r $replicator_($srcID:$group)  \n\
foreach oif $oiflist {\n\
$r disable $oif\n\
}\n\
return 1\n\
} \n\
return 0\n\
}\n\
\n\
Class Classifier/Multicast/Replicator -superclass Classifier/Multicast\n\
\n\
Classifier/Multicast instproc new-group { src group iface code} {\n\
$self instvar node_\n\
$node_ new-group $src $group $iface $code\n\
}\n\
\n\
Classifier/Multicast instproc no-slot slot {\n\
}\n\
\n\
Classifier/Multicast/Replicator instproc init args {\n\
$self next\n\
$self instvar nrep_\n\
set nrep_ 0\n\
}\n\
\n\
Classifier/Multicast/Replicator instproc add-rep { rep src group iif } {\n\
$self instvar nrep_\n\
$self set-hash $src $group $nrep_ $iif\n\
$self install $nrep_ $rep\n\
incr nrep_\n\
}\n\
\n\
Class Classifier/Replicator/Demuxer -superclass Classifier/Replicator\n\
Classifier/Replicator/Demuxer set ignore_ 0\n\
Classifier/Replicator/Demuxer instproc init args {\n\
eval $self next $args\n\
$self instvar nslot_ nactive_\n\
set nactive_ 0\n\
}\n\
\n\
Classifier/Replicator/Demuxer instproc is-active {} {\n\
$self instvar nactive_\n\
expr $nactive_ > 0\n\
}\n\
\n\
Classifier/Replicator/Demuxer instproc insert target {\n\
$self instvar nactive_ active_ \n\
\n\
if ![info exists active_($target)] {\n\
set active_($target) -1\n\
}\n\
if {$active_($target) < 0} {\n\
$self enable $target\n\
}\n\
}\n\
\n\
Classifier/Replicator/Demuxer instproc dump-oifs {} {\n\
set oifs \"\"\n\
if [$self is-active] {\n\
$self instvar active_\n\
foreach target [array names active_] {\n\
if { $active_($target) >= 0 } {\n\
lappend oifs [$self slot $active_($target)]\n\
}\n\
}\n\
}\n\
return [lsort $oifs]\n\
}\n\
\n\
Classifier/Replicator/Demuxer instproc disable target {\n\
$self instvar nactive_ active_\n\
if {[info exists active_($target)] && $active_($target) >= 0} {\n\
$self clear $active_($target)\n\
set active_($target) -1\n\
incr nactive_ -1\n\
}\n\
}\n\
\n\
Classifier/Replicator/Demuxer instproc enable target {\n\
$self instvar nactive_ active_ ignore_\n\
if {$active_($target) < 0} {\n\
set active_($target) [$self installNext $target]\n\
incr nactive_\n\
set ignore_ 0\n\
}\n\
}\n\
\n\
Classifier/Replicator/Demuxer instproc exists target {\n\
$self instvar active_\n\
info exists active_($target)\n\
}\n\
\n\
Classifier/Replicator/Demuxer instproc is-active-target target {\n\
$self instvar active_\n\
if { [info exists active_($target)] && $active_($target) >= 0 } {\n\
return 1\n\
} else {\n\
return 0\n\
}\n\
}\n\
\n\
Classifier/Replicator/Demuxer instproc drop { src dst {iface -1} } {\n\
$self instvar node_\n\
[$node_ getArbiter] drop $self $src $dst $iface\n\
}\n\
\n\
Node instproc change-iface { src dst oldiface newiface} {\n\
$self instvar multiclassifier_\n\
$multiclassifier_ change-iface $src $dst $oldiface $newiface\n\
}\n\
\n\
Node instproc lookup-iface { src dst } {\n\
$self instvar multiclassifier_\n\
$multiclassifier_ lookup-iface $src $dst\n\
}\n\
\n\
Classifier/Replicator/Demuxer instproc reset {} {\n\
$self instvar nactive_ active_\n\
foreach { target slot } [array get active_] {\n\
$self clear $slot\n\
}\n\
set nactive_ 0\n\
unset active_\n\
}\n\
\n\
Agent/Mcast/Control instproc init { protocol } {\n\
$self next\n\
$self instvar proto_\n\
set proto_ $protocol\n\
}\n\
\n\
Agent/Mcast/Control array set messages {}\n\
Agent/Mcast/Control set mcounter 0\n\
\n\
Agent/Mcast/Control instproc send {type from src group args} {\n\
Agent/Mcast/Control instvar mcounter messages\n\
set messages($mcounter) [concat [list $from $src $group] $args]\n\
$self cmd send $type $mcounter\n\
incr mcounter\n\
}\n\
\n\
Agent/Mcast/Control instproc recv {type iface m} {\n\
Agent/Mcast/Control instvar messages\n\
eval $self recv2 $type $iface $messages($m)\n\
}\n\
\n\
Agent/Mcast/Control instproc recv2 {type iface from src group args} {\n\
$self instvar proto_\n\
eval $proto_ recv-$type $from $src $group $iface $args\n\
}\n\
\n\
Node instproc rpf-nbr src {\n\
$self instvar ns_ id_\n\
if [catch \"$src id\" srcID] {	\n\
set srcID $src\n\
}\n\
$ns_ get-node-by-id [[$ns_ get-routelogic] lookup $id_ $srcID]\n\
}\n\
\n\
LanNode instproc rpf-nbr src {\n\
$self instvar ns_ id_\n\
if [catch \"$src id\" srcID] {	\n\
set srcID $src\n\
}\n\
$ns_ get-node-by-id [[$ns_ get-routelogic] lookup $id_ $srcID]\n\
}\n\
\n\
Node instproc getReps { src group } {\n\
$self instvar replicator_\n\
set reps \"\"\n\
foreach key [array names replicator_ \"$src:$group\"] { \n\
lappend reps $replicator_($key)\n\
}\n\
return [lsort $reps]\n\
}\n\
\n\
Node instproc getReps-raw { src group } {\n\
$self array get replicator_ \"$src:$group\"\n\
}\n\
\n\
Node instproc clearReps { src group } {\n\
$self instvar multiclassifier_\n\
foreach {key rep} [$self getReps-raw $src $group] {\n\
$rep reset\n\
delete $rep\n\
\n\
foreach {slot val} [$multiclassifier_ adjacents] {\n\
if { $val == $rep } {\n\
$multiclassifier_ clear $slot\n\
}\n\
}\n\
\n\
$self unset replicator_($key)\n\
}\n\
}\n\
\n\
Node instproc add-oif {head link} {\n\
$self instvar outLink_\n\
set outLink_($head) $link\n\
}\n\
\n\
Node instproc add-iif {iflbl link} {\n\
$self set inLink_($iflbl) $link\n\
}\n\
\n\
Node instproc get-all-oifs {} {\n\
$self instvar outLink_\n\
return [lsort [array names outLink_]]\n\
}\n\
\n\
Node instproc get-all-iifs {} {\n\
$self instvar inLink_\n\
return [array names inLink_]\n\
}\n\
\n\
Node instproc iif2oif ifid {\n\
$self instvar ns_\n\
set link [$self iif2link $ifid]\n\
set outlink [$ns_ link $self [$link src]]\n\
return [$self link2oif $outlink]\n\
}\n\
\n\
Node instproc iif2link ifid {\n\
$self set inLink_($ifid)\n\
}\n\
\n\
Node instproc link2iif link {\n\
return [[$link set iif_] label]\n\
}\n\
\n\
Node instproc link2oif link {\n\
$link head\n\
}\n\
\n\
Node instproc oif2link oif {\n\
$oif set link_\n\
}\n\
\n\
Node instproc from-node-iface { node } {\n\
$self instvar ns_\n\
catch {\n\
set node [$ns_ get-node-by-id $node]\n\
}\n\
set rpfnbr [$self rpf-nbr $node]\n\
set rpflink [$ns_ link $rpfnbr $self]\n\
if { $rpflink != \"\" } {\n\
return [$rpflink if-label?]\n\
}\n\
return \"?\" ;#unknown iface\n\
}\n\
\n\
Vlink instproc if-label? {} {\n\
$self instvar iif_\n\
$iif_ label\n\
}\n\
\n\
\n\
Simulator instproc src_rting args {\n\
$self set srcRt_ 1\n\
add-packet-header Src_rt\n\
Node enable-module Source\n\
}\n\
\n\
Simulator instproc src_rting? {} {\n\
$self instvar srcRt_\n\
if { ![info exists srcRt_] } {\n\
set srcRt_ 0\n\
}\n\
set srcRt_\n\
}  \n\
Class McastProtocol\n\
\n\
McastProtocol instproc init {sim node} {\n\
$self next\n\
$self instvar ns_ node_ status_ type_ id_\n\
set ns_   $sim\n\
set node_ $node\n\
set status_ \"down\"\n\
set type_   [$self info class]\n\
set id_ [$node id]\n\
\n\
$ns_ maybeEnableTraceAll $self $node_\n\
}\n\
\n\
McastProtocol instproc getType {} { $self set type_ }\n\
\n\
McastProtocol instproc start {}		{ $self set status_ \"up\"   }\n\
McastProtocol instproc stop {}		{ $self set status_\"down\"  }\n\
McastProtocol instproc getStatus {}	{ $self set status_	   }\n\
\n\
McastProtocol instproc upcall {code args} {\n\
eval $self handle-$code $args\n\
}\n\
\n\
McastProtocol instproc handle-wrong-iif { srcID group iface } {\n\
return 0\n\
}\n\
\n\
McastProtocol instproc handle-cache-miss { srcID group iface } {\n\
return 0\n\
}\n\
\n\
McastProtocol instproc annotate args {\n\
$self instvar dynT_ node_ ns_\n\
set s \"[$ns_ now] [$node_ id] $args\" ;#nam wants uinique first arg???\n\
if [info exists dynT_] {\n\
foreach tr $dynT_ {\n\
$tr annotate $s\n\
}\n\
}\n\
}\n\
\n\
McastProtocol instproc join-group arg	{ \n\
$self annotate $proc $arg \n\
}\n\
McastProtocol instproc leave-group arg	{ \n\
$self annotate $proc $arg\n\
}\n\
\n\
McastProtocol instproc trace { f src {op \"\"} } {\n\
$self instvar ns_ dynT_\n\
if {$op == \"nam\" && [info exists dynT_] > 0} {\n\
foreach tr $dynT_ {\n\
$tr namattach $f\n\
}\n\
} else {\n\
lappend dynT_ [$ns_ create-trace Generic $f $src $src $op]\n\
}\n\
}\n\
McastProtocol instproc notify { dummy } {\n\
$self instvar ns_ node_ PruneTimer_\n\
\n\
foreach r [$node_ getReps \"*\" \"*\"] {\n\
set src_id [$r set srcID_]\n\
set sources($src_id) 1\n\
}\n\
set sourceIDs [array names sources]\n\
foreach src_id $sourceIDs {\n\
set src [$ns_ get-node-by-id $src_id]\n\
if {$src != $node_} {\n\
set upstream [$node_ rpf-nbr $src]\n\
if { $upstream != \"\"} {\n\
set inlink [$ns_ link $upstream $node_]\n\
set newiif [$node_ link2iif $inlink]\n\
set reps [$node_ getReps $src_id \"*\"]\n\
foreach r $reps {\n\
set oldiif [$node_ lookup-iface $src_id [$r set grp_]]\n\
if { $oldiif != $newiif } {\n\
$node_ change-iface $src_id [$r set grp_] $oldiif $newiif\n\
}\n\
}\n\
}\n\
}\n\
set oiflist \"\"\n\
foreach nbr [$node_ neighbors] {\n\
set nbr_id [$nbr id]\n\
set nh [$nbr rpf-nbr $src] \n\
if { $nh != $node_ } {\n\
continue\n\
}\n\
set oif [$node_ link2oif [$ns_ link $node_ $nbr]]\n\
set oifs($oif) 1\n\
}\n\
set oiflist [array names oifs]\n\
\n\
set reps [$node_ getReps $src_id \"*\"]\n\
foreach r $reps {\n\
set grp [$r set grp_]\n\
set oldoifs [$r dump-oifs]\n\
set newoifs $oiflist\n\
foreach old $oldoifs {\n\
if [catch \"$node_ oif2link $old\" ] {\n\
continue\n\
}\n\
set idx [lsearch $newoifs $old]\n\
if { $idx < 0} {\n\
$r disable $old\n\
if [info exists PruneTimer_($src_id:$grp:$old)] {\n\
delete $PruneTimer_($src_id:$grp:$old)\n\
unset PruneTimer_($src_id:$grp:$old)\n\
}\n\
} else {\n\
set newoifs [lreplace $newoifs $idx $idx]\n\
}\n\
}\n\
foreach new $newoifs {\n\
foreach r $reps {\n\
$r insert $new\n\
}\n\
}\n\
}\n\
}\n\
}\n\
\n\
McastProtocol instproc dump-routes {chan {grp \"\"} {src \"\"}} {\n\
$self instvar ns_ node_\n\
if { $grp == \"\" } {\n\
array set reps [$node_ getReps-raw * *]\n\
} elseif { $src == \"\" } {\n\
array set reps [$node_ getReps-raw * $grp]  ;# actually, more than *,g\n\
} else {\n\
array set reps [$node_ getReps-raw $src $grp]\n\
}\n\
puts $chan [concat \"Node:\\t${node_}([$node_ id])\\tat t =\"	\\\n\
[format \"%4.2f\" [$ns_ now]]]\n\
puts $chan \"\\trepTag\\tActive\\t\\tsrc\\tgroup\\tiifNode\\t\\tdest_nodes\"\n\
foreach ent [lsort [array names reps]] {\n\
set sg [split $ent \":\"]\n\
if { [$reps($ent) is-active] } {\n\
set active Y\n\
} else {\n\
set active N\n\
}\n\
set dest \"\"\n\
foreach oif [$reps($ent) dump-oifs] {\n\
if ![catch { set nbr [[$node_ oif2link $oif] dst] } ] {\n\
set nbrid [$nbr id]\n\
if [$nbr is-lan?] {\n\
set nbrid ${nbrid}(L)\n\
}\n\
lappend dest $nbrid\n\
}\n\
}\n\
set s [lindex $sg 0]\n\
set g [lindex $sg 1]\n\
set iif [$node_ lookup-iface $s $g]\n\
\n\
set iif_node_id $iif\n\
catch {\n\
set iif_node [[$node_ iif2link $iif] src]\n\
if [$iif_node is-lan?] {\n\
set iif_node_id [$iif_node id](L)\n\
} else {\n\
set iif_node_id [$iif_node id]\n\
}\n\
}\n\
\n\
puts $chan [format \"\\t%5s\\t  %s\\t\\t%d\\t0x%x\\t%s\\t\\t%s\"	\\\n\
$reps($ent) $active $s $g $iif_node_id $dest]\n\
}\n\
}\n\
\n\
\n\
Class mrtObject\n\
\n\
mrtObject set mask-wkgroups	0xfff0\n\
mrtObject set wkgroups(Allocd)	[mrtObject set mask-wkgroups]\n\
\n\
mrtObject proc registerWellKnownGroups name {\n\
set newGroup [mrtObject set wkgroups(Allocd)]\n\
mrtObject set wkgroups(Allocd) [expr $newGroup + 1]\n\
mrtObject set wkgroups($name)  $newGroup\n\
}\n\
\n\
mrtObject proc getWellKnownGroup name {\n\
assert \"\\\"$name\\\" != \\\"Allocd\\\"\"\n\
mrtObject set wkgroups($name)\n\
}\n\
\n\
mrtObject registerWellKnownGroups ALL_ROUTERS\n\
mrtObject registerWellKnownGroups ALL_PIM_ROUTERS\n\
\n\
mrtObject proc expandaddr {} {\n\
mrtObject set mask-wkgroups	0x7fffffff\n\
\n\
foreach {name group} [mrtObject array get wkgroups] {\n\
mrtObject set wkgroups($name) [expr $group | 0x7fffffff]\n\
}\n\
}\n\
\n\
mrtObject instproc init { node } {\n\
$self next\n\
$self set node_	     $node\n\
}\n\
\n\
mrtObject instproc addproto { proto { iiflist \"\" } } {\n\
$self instvar node_ protocols_\n\
if { $iiflist == \"\" } {\n\
set iiflist [$node_ get-all-iifs]\n\
lappend iiflist -1 ;#for local packets\n\
}\n\
foreach iif $iiflist {\n\
set protocols_($iif) $proto\n\
}\n\
}\n\
\n\
mrtObject instproc getType { protocolType } {\n\
$self instvar protocols_\n\
foreach iif [array names protocols_] {\n\
if { [$protocols_($iif) getType] == $protocolType } {\n\
return $protocols_($iif)\n\
}\n\
}\n\
return \"\"\n\
}\n\
\n\
mrtObject instproc all-mprotos {op args} {\n\
$self instvar protocols_\n\
foreach iif [array names protocols_] {\n\
set p $protocols_($iif)\n\
if ![info exists protos($p)] {\n\
set protos($p) 1\n\
eval $p $op $args\n\
}\n\
}\n\
}\n\
\n\
mrtObject instproc start {}	{ $self all-mprotos start	}\n\
mrtObject instproc stop {}	{ $self all-mprotos stop	}\n\
mrtObject instproc notify dummy { $self all-mprotos notify $dummy }\n\
mrtObject instproc dump-routes args {\n\
$self all-mprotos dump-routes $args\n\
}\n\
\n\
mrtObject instproc join-group { grp src } {\n\
eval $self all-mprotos join-group $grp $src\n\
}\n\
\n\
mrtObject instproc leave-group { grp src } {\n\
eval $self all-mprotos leave-group $grp $src\n\
}\n\
\n\
mrtObject instproc upcall { code source group iface } {\n\
set wkgroup [expr [$class set mask-wkgroups]]\n\
if { [expr ( $group & $wkgroup ) == $wkgroup] } {\n\
$self instvar node_\n\
$node_ add-mfc $source $group -1 {}\n\
return 1\n\
} else {\n\
$self instvar protocols_\n\
$protocols_($iface) upcall $code $source $group $iface\n\
}\n\
}\n\
\n\
mrtObject instproc drop { replicator src dst {iface -1} } {\n\
$self instvar protocols_\n\
$protocols_($iface) drop $replicator $src $dst $iface\n\
}\n\
\n\
Class DM -superclass McastProtocol\n\
\n\
DM set PruneTimeout  0.5\n\
DM set CacheMissMode pimdm ;#or dvmrp (lowercase)\n\
\n\
DM instproc init { sim node } {\n\
$self instvar mctrl_\n\
set mctrl_ [new Agent/Mcast/Control $self]\n\
$node attach $mctrl_\n\
Timer/Iface/Prune set timeout [[$self info class] set PruneTimeout]\n\
$self next $sim $node\n\
}\n\
\n\
DM instproc join-group  { group } {\n\
$self instvar node_\n\
$self next $group\n\
set listOfReps [$node_ getReps * $group]\n\
foreach r $listOfReps {\n\
if ![$r is-active] {\n\
$self send-ctrl \"graft\" [$r set srcID_] $group\n\
set nbr [$node_ rpf-nbr [$r set srcID_]]\n\
set nbrs($nbr) 1\n\
}\n\
}\n\
foreach nbr [array names nbrs] {\n\
if [$nbr is-lan?] {\n\
$nbr instvar receivers_\n\
if [info exists receivers_($group)] {\n\
incr receivers_($group)\n\
} else {\n\
set receivers_($group) 1\n\
}\n\
}\n\
}\n\
}\n\
\n\
DM instproc leave-group { group } {\n\
$self next $group\n\
\n\
$self instvar node_\n\
set listOfReps [$node_ getReps * $group]\n\
foreach r $listOfReps {\n\
set nbr [$node_ rpf-nbr [$r set srcID_]]\n\
set nbrs($nbr) 1\n\
}\n\
foreach nbr [array names nbrs] {\n\
if [$nbr is-lan?] {\n\
$nbr instvar receivers_\n\
if { [info exists receivers_($group)] && \\\n\
$receivers_($group) > 0 } {\n\
incr receivers_($group) -1\n\
}\n\
}\n\
}\n\
}\n\
\n\
DM instproc handle-wrong-iif { srcID group iface } {\n\
$self instvar node_ ns_\n\
set inlink  [$node_ iif2link $iface]\n\
set from [$inlink src]\n\
$self send-ctrl \"prune\" $srcID $group [$from id]\n\
return 0 ;# don't call this method two times\n\
}\n\
\n\
DM instproc handle-cache-miss  { srcID group iface } {\n\
DM instvar CacheMissMode\n\
$self handle-cache-miss-$CacheMissMode $srcID $group $iface\n\
return 1 ;#call again\n\
}\n\
\n\
DM instproc handle-cache-miss-pimdm { srcID group iface } {\n\
$self instvar node_ ns_\n\
\n\
if { $iface >= 0 } {\n\
set rpf_nbr [$node_ rpf-nbr $srcID]\n\
set inlink  [$node_ iif2link $iface]\n\
set rpflink [$ns_ link $rpf_nbr $node_]\n\
\n\
if { $inlink != $rpflink } {\n\
set from [$inlink src]\n\
$self send-ctrl \"prune\" $srcID $group [$from id]\n\
return 0; #drop this packet\n\
}\n\
set rpfoif [$node_ iif2oif $iface]\n\
} else {\n\
set rpfoif \"\"\n\
}\n\
set alloifs [$node_ get-all-oifs]\n\
set oiflist \"\"\n\
foreach oif $alloifs {\n\
if {$oif == $rpfoif} {\n\
continue ;#exclude incoming iface\n\
}\n\
set dst [[$node_ oif2link $oif] dst]\n\
if { [$dst is-lan?] && [$dst rpf-nbr $srcID] != $node_  } {\n\
continue \n\
}\n\
lappend oiflist $oif\n\
}\n\
\n\
$node_ add-mfc $srcID $group $iface $oiflist\n\
}\n\
\n\
DM instproc handle-cache-miss-dvmrp { srcID group iface } {\n\
$self instvar node_ ns_\n\
\n\
set oiflist \"\"\n\
foreach nbr [$node_ neighbors] {\n\
set rpfnbr [$nbr rpf-nbr $srcID]\n\
if { $rpfnbr == $node_ } {\n\
set link [$ns_ link $node_ $nbr]\n\
lappend oiflist [$node_ link2oif $link]\n\
}\n\
\n\
}\n\
$node_ add-mfc $srcID $group $iface $oiflist\n\
}\n\
\n\
DM instproc drop { replicator src dst iface} {\n\
$self instvar node_ ns_\n\
\n\
if { $iface < 0 } {\n\
$replicator set ignore_ 1\n\
} else {\n\
set from [[$node_ iif2link $iface] src]\n\
if [$from is-lan?] {\n\
$self send-ctrl \"prune\" $src $dst\n\
} else {\n\
$self send-ctrl \"prune\" $src $dst [$from id]\n\
}\n\
}\n\
}\n\
\n\
DM instproc recv-prune { from src group iface} {\n\
$self instvar node_ PruneTimer_ ns_\n\
\n\
set r [$node_ getReps $src $group]\n\
if { $r == \"\" } { \n\
return 0\n\
}\n\
set id [$node_ id]\n\
set tmpoif [$node_ iif2oif $iface]\n\
\n\
if { [$r is-active-target $tmpoif] } {\n\
$r disable $tmpoif\n\
if ![$r is-active] {\n\
if { $src != $id } {\n\
$self send-ctrl prune $src $group\n\
}\n\
}\n\
}\n\
if ![info exists PruneTimer_($src:$group:$tmpoif)] {\n\
set PruneTimer_($src:$group:$tmpoif) \\\n\
[new Timer/Iface/Prune $self $src $group $tmpoif $ns_]\n\
}\n\
$PruneTimer_($src:$group:$tmpoif) schedule\n\
\n\
}\n\
\n\
DM instproc recv-graft { from src group iface} {\n\
$self instvar node_ PruneTimer_ ns_\n\
\n\
set id [$node_ id]\n\
set r [$node_ getReps $src $group]\n\
if { $r == \"\" } {\n\
if { $id == $src } {\n\
set iif \"?\"\n\
} else {\n\
set rpfnbr [$node_ rpf_nbr $src]\n\
set rpflnk [$ns_ link $src $id]\n\
set iif [$node_ link2iif $rpflnk]\n\
}\n\
$node_ add-mfc $src $group $iif \"\"\n\
set r [$node_ getReps $src $group]\n\
} \n\
if { ![$r is-active] && $src != $id } {\n\
$self send-ctrl graft $src $group\n\
}\n\
set tmpoif [$node_ iif2oif $iface]\n\
$r enable $tmpoif\n\
if [info exists PruneTimer_($src:$group:$tmpoif)] {\n\
delete $PruneTimer_($src:$group:$tmpoif)\n\
unset  PruneTimer_($src:$group:$tmpoif)\n\
}\n\
}\n\
\n\
DM instproc send-ctrl { which src group { to \"\" } } {\n\
$self instvar mctrl_ ns_ node_\n\
if { $to != \"\" } {\n\
set n [$ns_ get-node-by-id $to]\n\
if [$n is-lan?] return\n\
set toid $to\n\
} else {\n\
set toid $src\n\
}\n\
set nbr [$node_ rpf-nbr $toid]\n\
if [$nbr is-lan?] {\n\
$nbr instvar receivers_\n\
if { [info exists receivers_($group)] && \\\n\
$receivers_($group) > 0 } return \n\
set nbr [$nbr rpf-nbr $toid]\n\
}\n\
$ns_ simplex-connect $mctrl_ \\\n\
[[[$nbr getArbiter] getType [$self info class]] set mctrl_]\n\
if { $which == \"prune\" } {\n\
$mctrl_ set class_ 30\n\
} else {\n\
$mctrl_ set class_ 31\n\
}        \n\
$mctrl_ send $which [$node_ id] $src $group\n\
}\n\
\n\
DM instproc timeoutPrune { oif src grp } {\n\
$self instvar node_ PruneTimer_ ns_\n\
set r [$node_ getReps $src $grp]\n\
\n\
$r insert $oif\n\
if [info exists PruneTimer_($src:$grp:$oif)] {\n\
delete $PruneTimer_($src:$grp:$oif)\n\
unset PruneTimer_($src:$grp:$oif)\n\
}\n\
return\n\
}\n\
\n\
\n\
Class Timer/Iface/Prune -superclass Timer/Iface\n\
Timer/Iface/Prune set timeout 0.5\n\
\n\
Timer/Iface/Prune instproc timeout {} {\n\
$self instvar proto_ src_ grp_ oif_\n\
$proto_ timeoutPrune $oif_ $src_ $grp_\n\
}\n\
\n\
\n\
\n\
Class CtrMcast -superclass McastProtocol\n\
\n\
CtrMcast instproc init { sim node } {\n\
$self next $sim $node\n\
$self instvar ns_ node_\n\
$self instvar agent_ defaultTree_ decapagent_\n\
$self instvar c_rp_ c_bsr_ priority_\n\
\n\
set agent_ [$ns_ set MrtHandle_]\n\
\n\
set defaultTree_ \"RPT\"\n\
\n\
set decapagent_ [new Agent/Decapsulator]\n\
$ns_ attach-agent $node_ $decapagent_\n\
\n\
set c_rp_      1\n\
set c_bsr_     1\n\
set priority_  0\n\
}\n\
\n\
CtrMcast instproc join-group  { group } {\n\
$self next $group\n\
$self instvar node_ ns_ agent_\n\
$self instvar defaultTree_\n\
\n\
if { [$agent_ treetype? $group] == \"\" } {\n\
$agent_ treetype $group $defaultTree_\n\
$agent_ add-new-group $group\n\
}\n\
\n\
$agent_ add-new-member $group $node_\n\
\n\
foreach src [$agent_ sources? $group] {\n\
$agent_ compute-branch $src $group $node_\n\
}\n\
}\n\
\n\
CtrMcast instproc leave-group  { group } {\n\
$self next $group\n\
$self instvar node_ ns_ agent_ defaultTree_\n\
\n\
$agent_ remove-member $group $node_\n\
foreach src [$agent_ sources? $group] {\n\
$agent_ prune-branch $src $group $node_\n\
}\n\
}\n\
\n\
CtrMcast instproc handle-cache-miss { srcID group iface } {\n\
$self instvar ns_ agent_ node_\n\
$self instvar defaultTree_\n\
\n\
if { [$agent_ treetype? $group] == \"\" } {\n\
$agent_ treetype $group $defaultTree_\n\
}\n\
if { [$node_ id] == $srcID } {\n\
set RP [$self get_rp $group]\n\
if {[$agent_ treetype? $group] == \"RPT\" && $srcID != [$RP id]} {\n\
set encapagent [new Agent/Encapsulator]\n\
$ns_ attach-agent $node_ $encapagent\n\
\n\
set ctrmcast [[$RP getArbiter] getType \"CtrMcast\"]\n\
$ns_ connect $encapagent [$ctrmcast set decapagent_]\n\
\n\
$node_ add-mfc-reg $srcID $group -1 $encapagent\n\
}\n\
\n\
if [$agent_ new-source? $group $node_] {\n\
$agent_ compute-tree $node_ $group\n\
}\n\
} elseif [SessionSim set MixMode_] {\n\
set srcnode [$ns_ get-node-by-id $srcID]\n\
if [$agent_ new-source? $group $srcnode] {\n\
$agent_ compute-tree $srcnode $group\n\
}\n\
}\n\
return 1 ;#call again\n\
}\n\
\n\
CtrMcast instproc drop  { replicator src group iface } {\n\
}\n\
\n\
CtrMcast instproc handle-wrong-iif { srcID group iface } {\n\
warn \"$self: $proc for <S: $srcID, G: $group, if: $iface>?\"\n\
return 0 ;#call once\n\
}\n\
\n\
CtrMcast instproc notify { dummy } {\n\
}\n\
CtrMcast instproc get_rp group {\n\
$self instvar rpset_ agent_\n\
\n\
if ![info exists rpset_] {\n\
[$agent_ set ctrrpcomp] compute-rpset\n\
assert [info exists rpset_]\n\
}\n\
set returnrp -1\n\
set hashval -1\n\
foreach rp $rpset_ {\n\
if {[$self hash $rp $group] > $hashval} {\n\
set hashval [$self hash $rp $group]\n\
set returnrp $rp\n\
}\n\
}\n\
set returnrp		;# return\n\
}\n\
\n\
CtrMcast instproc hash {rp group} {\n\
$rp id\n\
}\n\
\n\
CtrMcast instproc set-rpset args {\n\
eval $self set rpset_ \"$args\"\n\
}\n\
\n\
CtrMcast instproc get_bsr {} {\n\
warn \"$self: CtrMcast doesn't require a BSR\"\n\
}\n\
\n\
CtrMcast instproc set_c_bsr { prior } {\n\
$self instvar c_bsr_ priority_\n\
set c_bsr_ 1\n\
set priority_ $prior\n\
}\n\
\n\
CtrMcast instproc set_c_rp {} {\n\
$self instvar c_rp_\n\
set c_rp_ 1\n\
}\n\
\n\
CtrMcast instproc unset_c_rp {} {\n\
$self instvar c_rp_\n\
set c_rp_ 0\n\
}\n\
\n\
\n\
Node instproc add-mfc-reg { src group iif oiflist } {\n\
$self instvar multiclassifier_ Regreplicator_\n\
\n\
\n\
if [info exists Regreplicator_($group)] {\n\
foreach oif $oiflist {\n\
$Regreplicator_($group) insert $oif\n\
}\n\
return 1\n\
}\n\
set r [new Classifier/Replicator/Demuxer]\n\
$r set node_ $self\n\
$r set srcID_ $src\n\
set Regreplicator_($group) $r\n\
\n\
foreach oif $oiflist {\n\
$r insert $oif\n\
}\n\
\n\
$multiclassifier_ add-rep $r $src $group $iif\n\
}\n\
\n\
Node instproc getRegreplicator group {\n\
$self instvar Regreplicator_\n\
if [info exists Regreplicator_($group)] {\n\
return $Regreplicator_($group)\n\
} else {\n\
return -1\n\
}\n\
}\n\
\n\
Class CtrMcastComp\n\
\n\
CtrMcastComp instproc init sim {\n\
$self instvar ns_\n\
\n\
set ns_ $sim\n\
$self init-groups\n\
$ns_ maybeEnableTraceAll $self {}\n\
}\n\
\n\
CtrMcastComp instproc id {} {\n\
return 0\n\
}\n\
\n\
CtrMcastComp instproc trace { f nop {op \"\"} } {\n\
$self instvar ns_ dynT_\n\
if {$op == \"nam\" && [info exists dynT_]} {\n\
foreach tr $dynT_ {\n\
$tr namattach $f\n\
}\n\
} else {\n\
lappend dynT_ [$ns_ create-trace Generic $f $self $self $op]\n\
}\n\
}\n\
\n\
CtrMcastComp instproc reset-mroutes {} {\n\
$self instvar ns_\n\
\n\
foreach node [$ns_ all-nodes-list] {\n\
foreach group [$self groups?] {\n\
set class_info [$node info class]\n\
if {$class_info != \"LanNode\"} {\n\
$node clearReps * $group\n\
}\n\
}\n\
}\n\
}\n\
\n\
CtrMcastComp instproc compute-mroutes {} {\n\
$self reset-mroutes\n\
foreach group [$self groups?] {\n\
foreach src [$self sources? $group] {\n\
$self compute-tree $src $group\n\
}\n\
}\n\
}\n\
\n\
CtrMcastComp instproc compute-tree { src group } {\n\
foreach mem [$self members? $group] {\n\
$self compute-branch $src $group $mem\n\
}\n\
}\n\
\n\
\n\
CtrMcastComp instproc compute-branch { src group nodeh } {\n\
$self instvar ns_\n\
\n\
set tt [$self treetype? $group]\n\
if { $tt == \"SPT\" } {\n\
set target $src\n\
} elseif { $tt == \"RPT\" } {\n\
set target [$self get_rp $nodeh $group]\n\
}\n\
\n\
for {\n\
set downstreamtmp \"\"\n\
set tmp $nodeh\n\
} { $downstreamtmp != $target } {\n\
set downstreamtmp $tmp\n\
set tmp [$tmp rpf-nbr $target]\n\
} {\n\
\n\
if {[SessionSim set MixMode_] && $downstreamtmp != \"\" && ![$ns_ detailed-link? [$tmp id] [$downstreamtmp id]]} {\n\
break\n\
}\n\
\n\
\n\
if {$tmp == $target} {\n\
set iif -1\n\
} else {\n\
set rpfl [$ns_ link [$tmp rpf-nbr $target] $tmp]\n\
\n\
if {[SessionSim set MixMode_] && $rpfl == \"\"} {\n\
set iif -1\n\
set ttmp $tmp\n\
while {$ttmp != $target} {\n\
set rpfl [$ns_ link [$ttmp rpf-nbr $target] $ttmp]\n\
if {$rpfl != \"\"} {\n\
set iif [$rpfl if-label?]\n\
break\n\
}\n\
set ttmp [$ttmp rpf-nbr $target]\n\
}\n\
} else {\n\
set iif [$rpfl if-label?]\n\
}\n\
}\n\
\n\
set oiflist \"\"\n\
if { $downstreamtmp != \"\" } {\n\
set rpfnbr [$downstreamtmp rpf-nbr $target]\n\
if { $rpfnbr == $tmp } {\n\
set rpflink [$ns_ link $tmp $downstreamtmp]\n\
if {$rpflink != \"\"} {\n\
set oiflist [$tmp link2oif $rpflink]\n\
} \n\
}\n\
}\n\
\n\
if { [set r [$tmp getReps [$src id] $group]] != \"\" } {\n\
if [$r is-active] {\n\
if { $oiflist != \"\" } {\n\
$r insert [lindex $oiflist 0]\n\
}\n\
break\n\
} else {\n\
if { $oiflist != \"\" } {\n\
$r insert [lindex $oiflist 0]\n\
}\n\
}\n\
} else {\n\
$tmp add-mfc [$src id] $group $iif $oiflist\n\
}\n\
}\n\
}\n\
\n\
\n\
CtrMcastComp instproc prune-branch { src group nodeh } {\n\
$self instvar ns_\n\
\n\
set tt [$self treetype? $group]\n\
if { $tt == \"SPT\" } {\n\
set target $src\n\
} elseif { $tt == \"RPT\" } {\n\
set target [$self get_rp $nodeh $group]\n\
}\n\
\n\
for {\n\
set downstreamtmp \"\"\n\
set tmp $nodeh\n\
} { $downstreamtmp != $target } {\n\
set downstreamtmp $tmp\n\
set tmp [$tmp rpf-nbr $target]\n\
} {\n\
set iif -1\n\
set oif \"\"\n\
if { $downstreamtmp != \"\" } {\n\
set rpfnbr [$downstreamtmp rpf-nbr $target]\n\
if { $rpfnbr == $tmp } {\n\
set oif [$tmp link2oif [$ns_ link $tmp $downstreamtmp]]\n\
}\n\
}\n\
\n\
if { [set r [$tmp getReps [$src id] $group]] != \"\" } {\n\
if { $oif != \"\" } {\n\
$r disable $oif\n\
}\n\
\n\
if [$r is-active] {\n\
break\n\
}\n\
} else {\n\
break\n\
}\n\
}\n\
\n\
}\n\
\n\
CtrMcastComp instproc notify {} {\n\
$self instvar ctrrpcomp\n\
\n\
$ctrrpcomp compute-rpset\n\
$self compute-mroutes\n\
}\n\
\n\
CtrMcastComp instproc init-groups {} {\n\
$self set Glist_ \"\"\n\
}\n\
\n\
CtrMcastComp instproc add-new-group group {\n\
$self instvar Glist_ \n\
set group [expr $group]\n\
\n\
if ![info exist Glist_] {\n\
set Glist_ \"\"\n\
}\n\
if {[lsearch $Glist_ $group] < 0} {\n\
lappend Glist_ $group\n\
}\n\
}\n\
\n\
CtrMcastComp instproc add-new-member {group node} {\n\
$self instvar Mlist_ \n\
set group [expr $group]\n\
\n\
$self add-new-group $group\n\
if ![info exist Mlist_($group)] {\n\
set Mlist_($group) \"\"\n\
}\n\
\n\
if {[lsearch $Mlist_($group) $node] < 0} {\n\
lappend Mlist_($group) $node\n\
}\n\
}\n\
\n\
CtrMcastComp instproc new-source? {group node} {\n\
$self instvar Slist_ \n\
set group [expr $group]\n\
\n\
$self add-new-group $group\n\
if ![info exist Slist_($group)] {\n\
set Slist_($group) \"\"\n\
}\n\
\n\
if {[lsearch $Slist_($group) $node] < 0} {\n\
lappend Slist_($group) $node\n\
return 1\n\
} else {\n\
return 0\n\
}\n\
}\n\
\n\
CtrMcastComp instproc groups? {} {\n\
$self set Glist_\n\
}\n\
\n\
CtrMcastComp instproc members? group {\n\
$self instvar Mlist_\n\
set group [expr $group]\n\
if ![info exists Mlist_($group)] {\n\
set Mlist_($group) \"\"\n\
}\n\
set Mlist_($group)\n\
}\n\
\n\
CtrMcastComp instproc sources? group {\n\
$self instvar Slist_\n\
set group [expr $group]\n\
if ![info exists Slist_($group)] {\n\
set Slist_($group) \"\"\n\
}\n\
set Slist_($group)\n\
}\n\
\n\
CtrMcastComp instproc remove-member {group node} {\n\
$self instvar Mlist_ Glist_\n\
set group [expr $group]\n\
\n\
set k [lsearch $Mlist_($group) $node]\n\
if {$k < 0} {\n\
puts \"warning: removing non-member\"\n\
} else {\n\
set Mlist_($group) [lreplace $Mlist_($group) $k $k]\n\
}\n\
\n\
if { $Mlist_($group) == \"\" } {\n\
set k [lsearch $Glist_ $group]\n\
if {$k < 0} {\n\
puts \"warning: removing non-existing group\"\n\
} else {\n\
set Glist_ [lreplace $Glist_ $k $k]\n\
}\n\
}\n\
}\n\
\n\
CtrMcastComp instproc treetype? group {\n\
$self instvar treetype_\n\
set group [expr $group]\n\
if [info exists treetype_($group)] {\n\
return $treetype_($group)\n\
} else {\n\
return \"\"\n\
}\n\
}\n\
\n\
CtrMcastComp instproc treetype {group tree} {\n\
$self set treetype_([expr $group]) $tree\n\
}\n\
\n\
CtrMcastComp instproc switch-treetype group {\n\
$self instvar treetype_ dynT_\n\
set group [expr $group]\n\
\n\
if [info exists dynT_] {\n\
foreach tr $dynT_ {\n\
$tr annotate \"$group switch tree type\"\n\
}\n\
}\n\
set treetype_($group) \"SPT\"\n\
$self add-new-group $group\n\
$self compute-mroutes\n\
}\n\
\n\
CtrMcastComp instproc set_c_rp args {\n\
$self instvar ns_\n\
\n\
\n\
foreach n [$ns_ all-nodes-list] {\n\
set arbiter [$n getArbiter]\n\
if {$arbiter != \"\"} {\n\
set ctrmcast [$arbiter getType \"CtrMcast\"]\n\
$ctrmcast instvar c_rp_\n\
$ctrmcast unset_c_rp\n\
}\n\
}\n\
\n\
foreach node $args {\n\
set arbiter [$node getArbiter]	   \n\
set ctrmcast [$arbiter getType \"CtrMcast\"]\n\
$ctrmcast set_c_rp\n\
}\n\
}\n\
\n\
CtrMcastComp instproc set_c_bsr args {\n\
foreach node $args {\n\
set tmp [split $node :]\n\
set node [lindex $tmp 0]\n\
set prior [lindex $tmp 1]\n\
set arbiter [$node getArbiter]\n\
set ctrmcast [$arbiter getType \"CtrMcast\"]\n\
$ctrmcast set_c_bsr $prior\n\
}\n\
}\n\
\n\
CtrMcastComp instproc get_rp { node group } {\n\
set ctrmcast [[$node getArbiter] getType \"CtrMcast\"]\n\
$ctrmcast get_rp $group\n\
}\n\
\n\
CtrMcastComp instproc get_bsr { node } {\n\
set arbiter [$node getArbiter]\n\
set ctrmcast [$arbiter getType \"CtrMcast\"]\n\
$ctrmcast get_bsr\n\
}\n\
\n\
Class CtrRPComp\n\
CtrRPComp instproc init sim {\n\
$self set ns_ $sim\n\
$self next\n\
}\n\
\n\
CtrRPComp instproc compute-rpset {} {\n\
$self instvar ns_\n\
\n\
foreach node [$ns_ all-nodes-list] {\n\
set connected($node) 0\n\
}\n\
set urtl [$ns_ get-routelogic]\n\
\n\
foreach node [$ns_ all-nodes-list] {\n\
foreach {vertix lvertix} [array get ldomain] {\n\
if {[$urtl lookup [$node id] [$vertix id]] >= 0} {\n\
lappend ldomain($vertix) $node\n\
set connected($node) 1\n\
break\n\
}\n\
}\n\
\n\
if {!$connected($node)} {\n\
set ldomain($node) $node\n\
set connected($node) 1\n\
}\n\
}\n\
\n\
foreach {vnode lvertix} [array get ldomain] {\n\
set hasbsr 0\n\
set rpset \"\"\n\
\n\
foreach vertix $lvertix {\n\
set class_info [$vertix info class]\n\
if {$class_info != \"LanNode\"} {\n\
set ctrdm [[$vertix getArbiter] getType \"CtrMcast\"]\n\
if [$ctrdm set c_bsr_] {set hasbsr 1}\n\
if [$ctrdm set c_rp_] {\n\
lappend rpset $vertix\n\
}\n\
}\n\
}\n\
\n\
foreach vertix $lvertix {\n\
set class_info [$vertix info class]\n\
if {$class_info != \"LanNode\"} {\n\
set ctrdm [[$vertix getArbiter] getType \"CtrMcast\"]\n\
if $hasbsr {\n\
$ctrdm set-rpset $rpset\n\
} else {\n\
$ctrdm set-rpset \"\"\n\
puts \"no c_bsr\"\n\
}\n\
}\n\
}\n\
}\n\
}\n\
\n\
Class BST -superclass McastProtocol\n\
\n\
BST instproc init { sim node } {\n\
$self instvar mctrl_ oiflist_\n\
BST instvar RP_\n\
\n\
set mctrl_ [new Agent/Mcast/Control $self]\n\
$node attach $mctrl_\n\
$self next $sim $node\n\
}\n\
\n\
BST instproc start {} {\n\
$self instvar node_ oiflist_\n\
BST instvar RP_\n\
\n\
foreach grpx [array names RP_] {\n\
set grp [expr $grpx]\n\
if { [string compare $grp $grpx] } {\n\
set RP_($grp) $RP_(grpx)\n\
unset RP_($grpx)\n\
}\n\
set rpfiif [$node_ from-node-iface $RP_($grp)]\n\
if { $rpfiif != \"?\" } {\n\
set rpfoif [$node_ iif2oif $rpfiif]\n\
} else {\n\
set rpfoif \"\"\n\
}\n\
\n\
set oiflist_($grp) $rpfoif\n\
set neighbors [$node_ set neighbor_]\n\
if [info exists neighbors] {\n\
for {set i 0} {$i < [llength $neighbors]} {incr i} {\n\
set neighbor [lindex $neighbors $i]\n\
set class_info [$neighbor info class]\n\
if {$class_info == \"LanNode\"} {\n\
$neighbor designate-ump-router $grp \\\n\
$RP_($grp)\n\
}\n\
}\n\
}\n\
}\n\
\n\
}\n\
\n\
BST instproc join-group  { group {src \"x\"} } {\n\
$self instvar node_ ns_ oiflist_\n\
BST instvar RP_\n\
\n\
set nbr [$node_ rpf-nbr $RP_($group)]\n\
set nbrs($nbr) 1\n\
$node_ add-mark m1 blue \"[$node_ get-shape]\"\n\
foreach nbr [array names nbrs] {\n\
if [$nbr is-lan?] {\n\
$nbr instvar receivers_\n\
if [info exists receivers_($group)] {\n\
incr receivers_($group)\n\
} else {\n\
$self send-ctrl \"graft\" $RP_($group) $group\n\
set receivers_($group) 1\n\
}\n\
}\n\
$self next $group ; #annotate\n\
}\n\
\n\
if { ![$node_ check-local $group] || [$node_ getReps \"x\" \\\n\
$group] == \"\"} { \n\
$self send-ctrl \"graft\" $RP_($group) $group\n\
}\n\
\n\
}\n\
\n\
BST instproc leave-group { group {src \"x\"} } {\n\
BST instvar RP_ \n\
\n\
$self next $group ;#annotate\n\
$self instvar node_ oiflist_\n\
\n\
set nbr [$node_ rpf-nbr $RP_($group)]\n\
if  [$nbr is-lan?] {\n\
$nbr instvar receivers_\n\
if [info exists receivers_($group)] {\n\
if {$receivers_($group) > 0} {\n\
incr receivers_($group) -1\n\
if {$receivers_($group) == 0} {\n\
$node_ delete-mark m1\n\
$self send-ctrl \"prune\" $RP_($group) $group\n\
}\n\
}\n\
} else {\n\
return\n\
}\n\
} else {\n\
set rpfiif [$node_ from-node-iface $RP_($group)]\n\
if { $rpfiif != \"?\" } {\n\
set rpfoif [$node_ iif2oif $rpfiif]\n\
} else {\n\
set rpfoif \"\"\n\
}\n\
if { $oiflist_($group) == \\\n\
$rpfoif && ![$node_ check-local $group] } {\n\
$self send-ctrl \"prune\" $RP_($group) $group\n\
$node_ delete-mark m1\n\
}\n\
}\n\
\n\
}\n\
\n\
BST instproc handle-wrong-iif { srcID group iface } {\n\
$self instvar node_ oiflist_\n\
BST instvar RP_\n\
\n\
\n\
set rep [$node_ getReps \"x\" $group]\n\
\n\
$node_ add-mfc \"x\" $group $iface $oiflist_($group)\n\
set iif [$node_ lookup-iface \"x\" $group]\n\
if { $iface >= 0 } {\n\
set oif [$node_ iif2oif $iface]\n\
set rpfiif [$node_ from-node-iface $RP_($group)]\n\
if { $iface == $rpfiif } {\n\
$rep disable [$node_ iif2oif $rpfiif]\n\
} else {\n\
$rep disable $oif\n\
if { $node_ != $RP_($group) } {\n\
$rep insert [$node_ iif2oif $rpfiif]\n\
}\n\
}\n\
}\n\
$node_ change-iface \"x\" $group $iif $iface\n\
return 1 ;#classify packet again\n\
}\n\
\n\
BST instproc handle-cache-miss { srcID group iface } {\n\
$self instvar node_  ns_ oiflist_\n\
BST instvar RP_\n\
\n\
if { [$node_ getReps \"x\" $group] != \"\" } {\n\
error\n\
}\n\
\n\
\n\
\n\
\n\
if {$iface != -1} {\n\
set neighbors [$node_ set neighbor_]\n\
if [info exists neighbors] {\n\
for {set i 0} {$i < [llength $neighbors]} {incr i} {\n\
set neighbor [lindex $neighbors $i]\n\
set nbr [$node_ rpf-nbr $RP_($group)]\n\
if {[$neighbor is-lan?] && \\\n\
[$nbr info class] != \"LanNode\"} {\n\
$neighbor instvar up_\n\
set up [$neighbor set up_($group)]\n\
if {$node_ != $up} {\n\
if [$self link2lan? $neighbor \\\n\
$iface] {\n\
return 0\n\
}\n\
}\n\
}\n\
}\n\
}\n\
}\n\
\n\
$node_ add-mfc \"x\" $group $iface $oiflist_($group)\n\
\n\
if { $iface > 0 } {\n\
set rep [$node_ getReps \"x\" $group]\n\
$rep disable [$node_ iif2oif $iface]\n\
}\n\
return 1 ;# classify the packet again.\n\
}\n\
\n\
BST instproc drop { replicator src dst iface} {\n\
$self instvar node_ ns_\n\
BST instvar RP_\n\
\n\
\n\
if {$iface >= 0} {\n\
}\n\
}\n\
\n\
BST instproc recv-prune { from src group iface} {\n\
$self instvar node_ ns_ oiflist_ \n\
BST instvar RP_ \n\
\n\
set rep [$node_ getReps \"x\" $group]\n\
if {$rep != \"\"} {\n\
set oif [$node_ iif2oif $iface]\n\
set idx [lsearch $oiflist_($group) $oif]\n\
if { $idx >= 0 } {\n\
set oiflist_($group) [lreplace $oiflist_($group) $idx $idx]\n\
$rep disable $oif\n\
set rpfiif [$node_ from-node-iface $RP_($group)]\n\
if { $rpfiif != \"?\" } {\n\
set rpfoif [$node_ iif2oif $rpfiif]\n\
} else {\n\
set rpfoif \"\"\n\
}\n\
if { $oiflist_($group) == $rpfoif && ![$node_ check-local $group] } {\n\
$node_ delete-mark m2\n\
$self send-ctrl \"prune\" $RP_($group) $group\n\
}\n\
}\n\
}\n\
}\n\
\n\
BST instproc recv-graft { from to group iface } {\n\
$self instvar node_ ns_ oiflist_\n\
BST instvar RP_\n\
\n\
set oif [$node_ iif2oif $iface]\n\
set rpfiif [$node_ from-node-iface $RP_($group)]\n\
if { $rpfiif != \"?\" } {\n\
set rpfoif [$node_ iif2oif $rpfiif]\n\
} else {\n\
set rpfoif \"\"\n\
}\n\
\n\
if { $oiflist_($group) == $rpfoif && ![$node_ check-local $group] } {\n\
$node_ add-mark m2 red circle\n\
$self send-ctrl \"graft\" $RP_($group) $group\n\
}\n\
if { [lsearch $oiflist_($group) $oif] < 0 } {\n\
lappend oiflist_($group) $oif\n\
if { [$node_ lookup-iface \"x\" $group] != $iface } {\n\
set rep [$node_ getReps \"x\" $group]\n\
if { $rep != \"\" } {\n\
$rep insert $oif\n\
}\n\
}\n\
}\n\
}\n\
\n\
BST instproc send-ctrl { which dst group } {\n\
$self instvar mctrl_ ns_ node_\n\
\n\
if {$node_ != $dst} {\n\
set nbr [$node_ rpf-nbr $dst]\n\
if [$nbr is-lan?] {\n\
$nbr instvar receivers_\n\
if { [info exists receivers_($group)] && \\\n\
$receivers_($group) > 0 } return\n\
\n\
set nbr [$nbr rpf-nbr $dst]\n\
}\n\
\n\
\n\
$ns_ simplex-connect $mctrl_ \\\n\
[[[$nbr getArbiter] getType [$self info class]] set mctrl_]\n\
if { $which == \"prune\" } {\n\
$mctrl_ set fid_ 2\n\
} else {\n\
$mctrl_ set fid_ 3\n\
}\n\
$mctrl_ send $which [$node_ id] $dst $group\n\
}\n\
}\n\
\n\
\n\
BST instproc dbg arg {\n\
$self instvar ns_ node_\n\
puts [format \"At %.4f : node [$node_ id] $arg\" [$ns_ now]]\n\
}\n\
\n\
\n\
\n\
LanNode instproc designate-ump-router {group dst} {\n\
$self instvar nodelist_\n\
$self instvar up_\n\
\n\
set nbr [$self rpf-nbr $dst]\n\
set up_($group) $nbr\n\
return\n\
}\n\
\n\
\n\
BST instproc next-hop-router {node group} {\n\
BST instvar RP_\n\
\n\
set nbr [$node rpf-nbr $RP_($group)]\n\
if [$nbr is-lan?] {\n\
set nbr [$nbr rpf-nbr $RP_($group)]\n\
}\n\
return $nbr\n\
}\n\
\n\
BST instproc is-group-bidir? {group} {\n\
BST instvar RP_\n\
\n\
foreach grp [array names RP_] {\n\
if {$grp == $group} {\n\
return 1\n\
}\n\
}\n\
return 0\n\
}\n\
\n\
BST instproc match-oif {group link} {\n\
$self instvar oiflist_\n\
\n\
set oiflist $oiflist_($group)\n\
if {$oiflist != \"\"} {\n\
foreach oif $oiflist {\n\
set oiflink [$oif set link_]\n\
if {$oiflink == $link} {\n\
return $oiflink\n\
}\n\
}\n\
}\n\
return\n\
}\n\
\n\
BST instproc find-oif {dst group} {\n\
$self instvar node_ ns_\n\
\n\
if {$node_ != $dst} {\n\
set ns [$self set ns_]\n\
$ns instvar link_\n\
set link [$ns set link_([$node_ id]:[$dst id])]\n\
return [$self match-oif $group $link]\n\
} else {\n\
return \"\"\n\
}\n\
}\n\
\n\
BST instproc link2lan? {neighbor iface} {\n\
$self instvar node_ ns_\n\
\n\
set link1 [[[$node_ iif2oif $iface] set link_] set iif_]\n\
set link2 [[$ns_ link $node_ $neighbor] set iif_]\n\
if {$link1 == $link2} {\n\
return 1\n\
} else {\n\
return 0\n\
}\n\
}\n\
\n\
Class Classifier/Multicast/Replicator/BST -superclass Classifier/Multicast/BST\n\
\n\
Classifier/Multicast/BST instproc new-group { src group iface code} {\n\
$self instvar node_\n\
$node_ new-group $src $group $iface $code\n\
}\n\
\n\
Classifier/Multicast/BST instproc no-slot slot {\n\
}\n\
\n\
Classifier/Multicast/Replicator/BST instproc init args {\n\
$self next\n\
$self instvar nrep_\n\
set nrep_ 0\n\
}\n\
\n\
Classifier/Multicast/Replicator/BST instproc add-rep { rep src group iif } {\n\
$self instvar nrep_\n\
$self set-hash $src $group $nrep_ $iif\n\
$self install $nrep_ $rep\n\
incr nrep_\n\
}\n\
\n\
Classifier/Multicast/Replicator/BST instproc match-BST-iif {iface group} {\n\
$self instvar node_\n\
\n\
list retval_\n\
set agents [$node_ set agents_]\n\
for {set i 0} {$i < [llength $agents]} {incr i} {\n\
set agent [lindex $agents $i]\n\
$agent instvar proto_\n\
if [info exists proto_] {\n\
set protocol [$agent set proto_]\n\
if {[$protocol info class] == \"BST\"} {\n\
BST instvar RP_\n\
$protocol instvar oiflist_\n\
set bidir [$protocol is-group-bidir? $group]\n\
if {$bidir == 1} {\n\
if {$node_ == $RP_($group)} {\n\
return 1\n\
}\n\
\n\
set iif [$node_ from-node-iface \\\n\
$RP_($group)]\n\
if {$iif == $iface} {\n\
return 1\n\
} else {\n\
return 0\n\
}\n\
}\n\
}\n\
}\n\
}\n\
return -1\n\
}\n\
\n\
Classifier/Multicast/Replicator/BST instproc upstream-link {group} {\n\
$self instvar node_\n\
\n\
list retval_\n\
set agents [$node_ set agents_]\n\
for {set i 0} {$i < [llength $agents]} {incr i} {\n\
set agent [lindex $agents $i]\n\
$agent instvar proto_\n\
if [info exists proto_] {\n\
set protocol [$agent set proto_]\n\
if {[$protocol info class] == \"BST\"} {\n\
BST instvar RP_\n\
$protocol instvar oiflist_\n\
set bidir [$protocol is-group-bidir? $group]\n\
if {$bidir == 1} {\n\
set nbr [$node_ rpf-nbr $RP_($group)]\n\
\n\
set oif [$protocol find-oif $nbr \\\n\
$group]\n\
\n\
if {$oif == \"\"} {\n\
set oif \"self\"\n\
} \n\
lappend retval_ $oif\n\
\n\
if [$nbr is-lan?] {\n\
set nbr [$nbr rpf-nbr $RP_($group)]\n\
}\n\
lappend retval_ [$nbr id]\n\
return $retval_\n\
\n\
}\n\
}\n\
}\n\
}\n\
return {}\n\
}\n\
\n\
Classifier/Multicast/Replicator/BST instproc check-rpf-link {node group} {\n\
$self instvar node_\n\
\n\
set agents [$node_ set agents_]\n\
for {set i 0} {$i < [llength $agents]} {incr i} {\n\
set agent [lindex $agents $i]\n\
$agent instvar proto_\n\
if [info exists proto_] {\n\
set protocol [$agent set proto_]\n\
set classInfo [$protocol info class]\n\
if {$classInfo == \"BST\"} {\n\
BST instvar RP_\n\
set rpfiif [$node_ from-node-iface \\\n\
$RP_($group)]\n\
return $rpfiif\n\
}\n\
}\n\
}\n\
return -1\n\
}\n\
\n\
\n\
\n\
Agent instproc traffic-source agent {\n\
$self instvar tg_\n\
set tg_ $agent\n\
$tg_ target $self\n\
$tg_ set agent_addr_ [$self set agent_addr_]\n\
$tg_ set agent_port_ [$self set agent_port_]\n\
}\n\
\n\
Agent/SRM set packetSize_  1024	;# assume default message size for repair is 1K\n\
Agent/SRM set groupSize_   0\n\
Agent/SRM set app_fid_ 0\n\
\n\
Agent/SRM set distanceCompute_	ewma\n\
\n\
Agent/SRM set C1_	2.0\n\
Agent/SRM set C2_	2.0\n\
Agent/SRM set requestFunction_	\"SRM/request\"\n\
Agent/SRM set requestBackoffLimit_	5\n\
\n\
Agent/SRM set D1_	1.0\n\
Agent/SRM set D2_	1.0\n\
Agent/SRM set repairFunction_	\"SRM/repair\"\n\
\n\
Agent/SRM set sessionDelay_ 1.0\n\
Agent/SRM set sessionFunction_	\"SRM/session\"\n\
\n\
Class Agent/SRM/Deterministic -superclass Agent/SRM\n\
Agent/SRM/Deterministic set C2_ 0.0\n\
Agent/SRM/Deterministic set D2_ 0.0\n\
\n\
Class Agent/SRM/Probabilistic -superclass Agent/SRM\n\
Agent/SRM/Probabilistic set C1_ 0.0\n\
Agent/SRM/Probabilistic set D1_ 0.0\n\
\n\
Class Agent/SRM/Fixed -superclass Agent/SRM\n\
\n\
Class SRM\n\
Class SRM/request -superclass SRM\n\
Class SRM/repair -superclass SRM\n\
Class SRM/session -superclass SRM\n\
\n\
\n\
Agent/SRM/Adaptive set pdistance_	0.0	;# bound instance variables\n\
Agent/SRM/Adaptive set requestor_ 0\n\
\n\
Agent/SRM/Adaptive set C1_	2.0\n\
Agent/SRM/Adaptive set MinC1_	0.5\n\
Agent/SRM/Adaptive set MaxC1_	2.0\n\
Agent/SRM/Adaptive set C2_	2.0\n\
Agent/SRM/Adaptive set MinC2_	1.0\n\
Agent/SRM/Adaptive set MaxC2_	1.0	;# G\n\
\n\
Agent/SRM/Adaptive set D1_	-1	;# log10 G\n\
Agent/SRM/Adaptive set MinD1_	0.5\n\
Agent/SRM/Adaptive set MaxD1_	0.0	;# log10 G\n\
Agent/SRM/Adaptive set D2_	-1	;# log10 G	XXX\n\
Agent/SRM/Adaptive set MinD2_	1.0\n\
Agent/SRM/Adaptive set MaxD2_	1.0	;# G\n\
\n\
Agent/SRM/Adaptive set requestFunction_	\"SRM/request/Adaptive\"\n\
Agent/SRM/Adaptive set repairFunction_	\"SRM/repair/Adaptive\"\n\
\n\
Agent/SRM/Adaptive set AveDups_	1.0\n\
Agent/SRM/Adaptive set AveDelay_	1.0\n\
\n\
Agent/SRM/Adaptive set eps_	0.10\n\
\n\
Agent/SRM/Adaptive instproc init args {\n\
\n\
eval $self next $args\n\
$self array set closest_ \"requestor 0 repairor 0\"\n\
\n\
$self set AveDups_	[$class set AveDups_]\n\
$self set AveDelay_	[$class set AveDelay_]\n\
\n\
foreach i [list MinC1_ MaxC1_ MinC2_ MaxC2_			\\\n\
MinD1_ MaxD1_ MinD2_ MaxD2_] {\n\
$self instvar $i\n\
set $i [$class set $i]\n\
}\n\
\n\
$self set eps_	[$class set eps_]\n\
}\n\
\n\
Agent/SRM/Adaptive instproc check-bounds args {\n\
set G [$self set groupSize_]\n\
$self set MaxC2_ $G\n\
$self set MaxD1_ [expr log10($G)]\n\
$self set MaxD2_ $G\n\
if {[llength $args] <= 0} {\n\
set args \"C1_ C2_ D1_ D2_\"\n\
}\n\
foreach i $args {\n\
$self instvar $i\n\
set val [$self set $i]	      ;# We do this for notational convenience\n\
set min [$self set Min$i]\n\
set max [$self set Max$i]\n\
if { $val < $min } {\n\
set $i $min\n\
} elseif { $val > $max } {\n\
set $i $max\n\
}\n\
}\n\
}\n\
\n\
Agent/SRM/Adaptive instproc recompute-request-params {} {\n\
$self instvar closest_ C1_ C2_ stats_ AveDups_ AveDelay_ eps_\n\
if {$stats_(ave-req-delay) < 0} {\n\
$self check-bounds C1_ C2_	;# adjust bounds to estimated size of G\n\
return\n\
}\n\
\n\
$self compute-ave dup-req\n\
if $closest_(requestor) {\n\
set C2_ [expr $C2_ - 0.1]\n\
set closest_(requestor)	0\n\
} elseif {$stats_(ave-dup-req) >= $AveDups_} {\n\
set C1_ [expr $C1_ + 0.1]\n\
set C2_ [expr $C2_ + 0.5]\n\
} elseif {$stats_(ave-dup-req) < [expr $AveDups_ - $eps_]} {\n\
if {$stats_(ave-req-delay) > $AveDelay_} {\n\
set C2_ [expr $C2_ - 0.1]\n\
}\n\
if {$stats_(ave-dup-req) < 0.25} {\n\
set C1_ [expr $C1_ - 0.05]\n\
}\n\
} else {\n\
set C1_ [expr $C1_ + 0.05]\n\
}\n\
$self check-bounds C1_ C2_\n\
}\n\
\n\
Agent/SRM/Adaptive instproc sending-request {} {\n\
$self set C1_ [expr [$self set C1_] - 0.1]  ;# XXX SF's code uses other\n\
$self set closest_(requestor) 1\n\
$self check-bounds C1_\n\
}\n\
\n\
Agent/SRM/Adaptive instproc recv-request {r d s m} {\n\
$self instvar pending_ closest_\n\
if { [info exists pending_($s:$m)]  && $d == 1 } {\n\
set closeness [$pending_($s:$m) closest-requestor?]\n\
if {$closeness >= 0} {\n\
set closest_(requestor) $closeness\n\
}\n\
}\n\
$self next $r $d $s $m\n\
}\n\
\n\
Agent/SRM/Adaptive instproc recompute-repair-params {} {\n\
$self instvar closest_ D1_ D2_ stats_ AveDups_ AveDelay_ eps_\n\
if {$stats_(ave-rep-delay) < 0} {\n\
set logG [expr log10([$self set groupSize_])]\n\
set D1_  $logG\n\
set D2_  $logG\n\
$self check-bounds D1_ D2_	;# adjust bounds to estimated size of G\n\
return\n\
}\n\
\n\
$self compute-ave dup-rep\n\
if $closest_(repairor) {\n\
set D2_ [expr $D2_ - 0.1]\n\
set closest_(repairor) 0\n\
} elseif {$stats_(ave-dup-rep) >= $AveDups_} {\n\
set D1_ [expr $D1_ + 0.1]\n\
set D2_ [expr $D2_ + 0.5]\n\
} elseif {$stats_(ave-dup-rep) < [expr $AveDups_ - $eps_]} {\n\
if {$stats_(ave-rep-delay) > $AveDelay_} {\n\
set D2_ [expr $D2_ - 0.1]\n\
}\n\
if {$stats_(ave-dup-rep) < 0.25} {\n\
set D1_ [expr $D1_ - 0.05]\n\
}\n\
} else {\n\
set D1_ [expr $D1_ + 0.05]\n\
}\n\
$self check-bounds D1_ D2_\n\
}\n\
\n\
Agent/SRM/Adaptive instproc sending-repair {} {\n\
$self set D1_ [expr [$self set D1_] - 0.1]  ;# XXX SF's code uses other\n\
$self set closest_(repairor) 1\n\
$self check-bounds D1_\n\
}\n\
\n\
Agent/SRM/Adaptive instproc recv-repair {d s m} {\n\
$self instvar pending_ closest_\n\
if { [info exists pending_($s:$m)] && $d == 1 } {\n\
set closeness [$pending_($s:$m) closest-repairor?]\n\
if {$closeness >= 0} {\n\
set closest_(repairor) $closeness\n\
}\n\
}\n\
$self next $d $s $m\n\
}\n\
\n\
Class SRM/request/Adaptive -superclass SRM/request\n\
SRM/request/Adaptive instproc set-params args {\n\
$self instvar agent_\n\
$agent_ recompute-request-params\n\
eval $self next $args\n\
}\n\
\n\
SRM/request/Adaptive instproc backoff? {} {\n\
$self instvar backoff_ backoffCtr_ backoffLimit_\n\
set retval $backoff_\n\
if {[incr backoffCtr_] <= $backoffLimit_} {\n\
set backoff_ [expr $backoff_ * 3]\n\
}\n\
set retval\n\
}\n\
\n\
SRM/request/Adaptive instproc schedule {} {\n\
$self next\n\
}\n\
\n\
SRM/request/Adaptive instproc send-request {} {\n\
$self instvar agent_ round_\n\
if { $round_ == 1 } {\n\
$agent_ sending-request\n\
}\n\
$self next\n\
}\n\
\n\
SRM/request/Adaptive instproc closest-requestor? {} {\n\
$self instvar agent_ sender_ sent_ round_\n\
if {$sent_ == 1 && $round_ == 1} {	;# since repairs aren't rescheduled.\n\
if {[$agent_ set pdistance_] >			\\\n\
[expr 1.5 * [$self distance? $sender_]]} {\n\
return 1\n\
} else {\n\
return 0\n\
}\n\
} else {\n\
return -1\n\
}\n\
}\n\
\n\
SRM/request/Adaptive instproc closest-repairor? {} {\n\
return -1\n\
}\n\
\n\
Class SRM/repair/Adaptive -superclass SRM/repair\n\
SRM/repair/Adaptive instproc set-params args {\n\
$self instvar agent_\n\
$agent_ recompute-repair-params\n\
eval $self next $args\n\
}\n\
\n\
SRM/repair/Adaptive instproc schedule {} {\n\
$self next\n\
}\n\
\n\
SRM/repair/Adaptive instproc send-repair {} {\n\
$self instvar round_ agent_\n\
if { $round_ == 1 } {\n\
$agent_ sending-repair\n\
}\n\
$self next\n\
}\n\
\n\
SRM/repair/Adaptive instproc closest-requestor? {} {\n\
return -1\n\
}\n\
\n\
SRM/repair/Adaptive instproc closest-repairor? {} {\n\
$self instvar agent_ requestor_ sent_ round_\n\
if {$sent_ == 1 && $round_ == 1} {\n\
if {[$agent_ set pdistance_] >			\\\n\
[expr 1.5 * [$self distance? $requestor_]]} {\n\
return 1\n\
} else {\n\
return 0\n\
}\n\
} else {\n\
return -1\n\
}\n\
}\n\
\n\
Agent/SRM instproc init {} {\n\
$self next\n\
$self instvar ns_ requestFunction_ repairFunction_\n\
set ns_ [Simulator instance]\n\
$self init-instvar sessionDelay_\n\
foreach var {C1_ C2_ D1_ D2_} {\n\
$self init-instvar $var\n\
}\n\
$self init-instvar requestFunction_\n\
$self init-instvar repairFunction_\n\
$self init-instvar sessionFunction_\n\
$self init-instvar requestBackoffLimit_\n\
$self init-instvar distanceCompute_\n\
\n\
$self array set stats_ [list		\\\n\
dup-req		-1	ave-dup-req	-1	\\\n\
dup-rep		-1	ave-dup-rep	-1	\\\n\
req-delay	0.0	ave-req-delay	-1	\\\n\
rep-delay	0.0	ave-rep-delay	-1	\\\n\
]\n\
}\n\
\n\
Agent/SRM instproc delete {} {\n\
$self instvar ns_ pending_ done_ session_ tg_\n\
foreach i [array names pending_] {\n\
$pending_($i) cancel DELETE\n\
delete $pending_($i)\n\
}\n\
$self cleanup\n\
delete $session_\n\
if [info exists tg_] {\n\
delete $tg_\n\
}\n\
}\n\
\n\
Agent/SRM instproc start {} {\n\
$self instvar node_ dst_addr_	;# defined in Agent base class\n\
set dst_addr_ [expr $dst_addr_]	; # get rid of possibly leading 0x etc.\n\
$self cmd start\n\
\n\
$node_ join-group $self $dst_addr_\n\
\n\
$self instvar ns_ session_ sessionFunction_\n\
set session_ [new $sessionFunction_ $ns_ $self]\n\
$session_ schedule\n\
}\n\
\n\
Agent/SRM instproc start-source {} {\n\
$self instvar tg_\n\
if ![info exists tg_] {\n\
error \"No source defined for agent $self\"\n\
}\n\
$tg_ start\n\
}\n\
\n\
Agent/SRM instproc sessionFunction f {\n\
$self instvar sessionFunction_\n\
set sessionFunction_ $f\n\
}\n\
\n\
Agent/SRM instproc requestFunction f {\n\
$self instvar requestFunction_\n\
set requestFunction_ $f\n\
}\n\
\n\
Agent/SRM instproc repairFunction f {\n\
$self instvar repairFunction_\n\
set repairFunction_ $f\n\
}\n\
\n\
Agent/SRM instproc groupSize? {} {\n\
$self set groupSize_\n\
}\n\
\n\
global alpha\n\
if ![info exists alpha] {\n\
set alpha	0.25\n\
}\n\
\n\
proc ewma {ave cur} {\n\
if {$ave < 0} {\n\
return $cur\n\
} else {\n\
global alpha\n\
return [expr (1 - $alpha) * $ave + $alpha * $cur]\n\
}\n\
}\n\
\n\
proc instantaneous {ave cur} {\n\
set cur\n\
}\n\
\n\
Agent/SRM instproc compute-ave var {\n\
$self instvar stats_\n\
set stats_(ave-$var) [ewma $stats_(ave-$var) $stats_($var)]\n\
}\n\
\n\
\n\
Agent/SRM instproc recv {type args} {\n\
eval $self recv-$type $args\n\
}\n\
\n\
Agent/SRM instproc recv-data {sender msgid} {\n\
$self instvar pending_\n\
if ![info exists pending_($sender:$msgid)] {\n\
error \"Oy vey!  How did we get here?\"\n\
}\n\
if {[$pending_($sender:$msgid) set round_] == 1} {\n\
$pending_($sender:$msgid) cancel DATA\n\
$pending_($sender:$msgid) evTrace Q DATA\n\
delete $pending_($sender:$msgid)\n\
unset pending_($sender:$msgid)\n\
} else {\n\
$pending_($sender:$msgid) recv-repair\n\
}\n\
}\n\
\n\
Agent/SRM instproc mark-period period {\n\
$self compute-ave $period\n\
$self set stats_($period) 0\n\
}\n\
\n\
Agent/SRM instproc request {sender args} {\n\
$self instvar pending_ ns_ requestFunction_\n\
set newReq 0\n\
foreach msgid $args {\n\
if [info exists pending_($sender:$msgid)] {\n\
error \"duplicate loss detection in agent\"\n\
}\n\
set pending_($sender:$msgid) [new $requestFunction_ $ns_ $self]\n\
$pending_($sender:$msgid) set-params $sender $msgid\n\
$pending_($sender:$msgid) schedule\n\
\n\
if ![info exists old_($sender:$msgid)] {\n\
incr newReq\n\
}\n\
}\n\
if $newReq {\n\
$self mark-period dup-req\n\
}\n\
}\n\
\n\
Agent/SRM instproc update-ave {type delay} {\n\
$self instvar stats_\n\
set stats_(${type}-delay) $delay\n\
$self compute-ave ${type}-delay\n\
}\n\
\n\
Agent/SRM instproc recv-request {requestor round sender msgid} {\n\
$self instvar pending_ stats_\n\
if [info exists pending_($sender:$msgid)] {\n\
if { $round == 1 } {\n\
incr stats_(dup-req) [$pending_($sender:$msgid)	\\\n\
dup-request?]\n\
}\n\
$pending_($sender:$msgid) recv-request\n\
} else {\n\
$self repair $requestor $sender $msgid\n\
}\n\
}\n\
\n\
Agent/SRM instproc repair {requestor sender msgid} {\n\
$self instvar pending_ ns_ repairFunction_\n\
if [info exists pending_($sender:$msgid)] {\n\
error \"duplicate repair detection in agent??  really??\"\n\
}\n\
set pending_($sender:$msgid) [new $repairFunction_ $ns_ $self]\n\
$pending_($sender:$msgid) set requestor_ $requestor\n\
$pending_($sender:$msgid) set-params $sender $msgid\n\
$pending_($sender:$msgid) schedule\n\
$self mark-period dup-rep\n\
}\n\
\n\
Agent/SRM instproc recv-repair {round sender msgid} {\n\
$self instvar pending_ stats_\n\
if ![info exists pending_($sender:$msgid)] {\n\
$self instvar trace_ ns_ node_ \n\
if [info exists trace_] {\n\
}\n\
} else {\n\
if { $round == 1 } {\n\
incr stats_(dup-rep) [$pending_($sender:$msgid)	\\\n\
dup-repair?]\n\
}\n\
$pending_($sender:$msgid) recv-repair\n\
}\n\
}\n\
\n\
Agent/SRM/Fixed instproc repair args {\n\
$self set D1_ [expr log10([$self set groupSize_])]\n\
$self set D2_ [expr log10([$self set groupSize_])]\n\
eval $self next $args\n\
}\n\
\n\
\n\
Agent/SRM instproc clear {obj s m} {\n\
$self instvar pending_ done_ old_ logfile_\n\
set done_($s:$m) $obj\n\
set old_($s:$m) [$obj set round_]\n\
if [info exists logfile_] {\n\
$obj dump-stats $logfile_\n\
}\n\
unset pending_($s:$m)\n\
if {[array size done_] > 32} {\n\
$self instvar ns_\n\
$ns_ at [expr [$ns_ now] + 0.01] \"$self cleanup\"\n\
}\n\
}\n\
\n\
Agent/SRM instproc round? {s m} {\n\
$self instvar old_\n\
if [info exists old_($s:$m)] {\n\
return $old_($s:$m)\n\
} else {\n\
return 0\n\
}\n\
}\n\
\n\
Agent/SRM instproc cleanup {} {\n\
$self instvar done_\n\
if [info exists done_] {\n\
foreach i [array names done_] {\n\
delete $done_($i)\n\
}\n\
unset done_\n\
}\n\
}\n\
\n\
Agent/SRM instproc trace file {\n\
$self set trace_ $file\n\
}\n\
\n\
Agent/SRM instproc log file {\n\
$self set logfile_ $file\n\
}\n\
\n\
SRM instproc init {ns agent} {\n\
$self next\n\
$self instvar ns_ agent_ nid_ distf_\n\
set ns_ $ns\n\
set agent_ $agent\n\
set nid_ [[$agent_ set node_] id]\n\
set distf_ [$agent_ set distanceCompute_]\n\
if ![catch \"$agent_ set trace_\" traceVar] {\n\
$self set trace_ $traceVar\n\
}\n\
$self array set times_ [list		\\\n\
startTime [$ns_ now] serviceTime -1 distance -1]\n\
}\n\
\n\
SRM instproc set-params {sender msgid} {\n\
$self next\n\
$self instvar agent_ sender_ msgid_ round_ sent_\n\
set sender_ $sender\n\
set msgid_  $msgid\n\
set round_  [$agent_ round? $sender_ $msgid_]\n\
set sent_	0\n\
}\n\
\n\
SRM instproc cancel {} {\n\
$self instvar ns_ eventID_\n\
if [info exists eventID_] {\n\
$ns_ cancel $eventID_\n\
unset eventID_\n\
}\n\
}\n\
\n\
SRM instproc schedule {} {\n\
$self instvar round_\n\
incr round_\n\
}\n\
\n\
SRM instproc distance? node {\n\
$self instvar agent_ times_ distf_\n\
set times_(distance) [$distf_ $times_(distance)	\\\n\
[$agent_ distance? $node]]\n\
}\n\
\n\
SRM instproc serviceTime {} {\n\
$self instvar ns_ times_\n\
set times_(serviceTime) [expr ([$ns_ now] - $times_(startTime)) / \\\n\
( 2 * $times_(distance))]\n\
}\n\
\n\
SRM instproc logpfx fp {\n\
$self instvar ns_ nid_ sender_ msgid_ round_\n\
puts -nonewline $fp [format \"%7.4f\" [$ns_ now]]\n\
puts -nonewline $fp \" n $nid_ m <$sender_:$msgid_> r $round_ \"\n\
}\n\
\n\
SRM instproc dump-stats fp {\n\
$self instvar times_ statistics_\n\
$self logpfx $fp\n\
puts -nonewline $fp \"type [string range [$self info class] 4 end] \"\n\
puts $fp \"[array get times_] [array get statistics_]\"\n\
}\n\
\n\
SRM instproc evTrace {tag type args} {\n\
$self instvar trace_\n\
if [info exists trace_] {\n\
$self logpfx $trace_\n\
puts -nonewline $trace_ \"$tag $type\"\n\
foreach elem $args {\n\
puts -nonewline $trace_ \" $elem\"\n\
}\n\
puts $trace_ {}\n\
}\n\
}\n\
\n\
\n\
SRM/request instproc init args {\n\
eval $self next $args\n\
$self array set statistics_ \"dupRQST 0 dupREPR 0 #sent 0 backoff 0\"\n\
}\n\
\n\
SRM/request instproc set-params args {\n\
eval $self next $args\n\
$self instvar agent_ sender_\n\
foreach var {C1_ C2_} {\n\
if ![catch \"$agent_ set $var\" val] {\n\
$self instvar $var\n\
set $var $val\n\
}\n\
}\n\
$self distance? $sender_\n\
$self instvar backoff_ backoffCtr_ backoffLimit_\n\
set backoff_ 1\n\
set backoffCtr_ 0\n\
set backoffLimit_ [$agent_ set requestBackoffLimit_]\n\
\n\
$self evTrace Q DETECT\n\
}\n\
\n\
SRM/request instproc dup-request? {} {\n\
$self instvar ns_ round_ ignore_\n\
if {$round_ == 2 && [$ns_ now] <= $ignore_} {\n\
return 1\n\
} else {\n\
return 0\n\
}\n\
}\n\
\n\
SRM/request instproc dup-repair? {} {\n\
return 0\n\
}\n\
\n\
SRM/request instproc backoff? {} {\n\
$self instvar backoff_ backoffCtr_ backoffLimit_\n\
set retval $backoff_\n\
if {[incr backoffCtr_] <= $backoffLimit_} {\n\
incr backoff_ $backoff_\n\
}\n\
set retval\n\
}\n\
\n\
SRM/request instproc compute-delay {} {\n\
$self instvar C1_ C2_\n\
set rancomp [expr $C1_ + $C2_ * [uniform 0 1]]\n\
\n\
$self instvar sender_ backoff_\n\
set dist [$self distance? $sender_]\n\
$self evTrace Q INTERVALS C1 $C1_ C2 $C2_ d $dist i $backoff_\n\
set delay [expr $rancomp * $dist]\n\
}\n\
\n\
SRM/request instproc schedule {} {\n\
$self instvar ns_ eventID_ delay_\n\
$self next\n\
set now [$ns_ now]\n\
set delay_ [expr [$self compute-delay] * [$self backoff?]]\n\
set fireTime [expr $now + $delay_]\n\
\n\
$self evTrace Q NTIMER at $fireTime\n\
\n\
set eventID_ [$ns_ at $fireTime \"$self send-request\"]\n\
}\n\
\n\
SRM/request instproc cancel type {\n\
$self next\n\
if {$type == \"REQUEST\" || $type == \"REPAIR\"} {\n\
$self instvar agent_ round_\n\
if {$round_ == 1} {\n\
$agent_ update-ave req [$self serviceTime]\n\
}\n\
}\n\
}\n\
\n\
SRM/request instproc send-request {} {\n\
$self instvar agent_ round_ sender_ msgid_ sent_ round_\n\
$self evTrace Q SENDNACK\n\
\n\
$agent_ send request $round_ $sender_ $msgid_\n\
\n\
$self instvar statistics_\n\
incr statistics_(#sent)\n\
set sent_ $round_\n\
}\n\
\n\
SRM/request instproc recv-request {} {\n\
$self instvar ns_ agent_ round_ delay_ ignore_ statistics_\n\
if {[info exists ignore_] && [$ns_ now] < $ignore_} {\n\
incr statistics_(dupRQST)\n\
} else {\n\
$self cancel REQUEST\n\
$self schedule          ;# or rather, reschedule-rqst \n\
set ignore_ [expr [$ns_ now] + ($delay_ / 2)]\n\
incr statistics_(backoff)\n\
$self evTrace Q NACK IGNORE-BACKOFF $ignore_\n\
}\n\
}\n\
\n\
SRM/request instproc recv-repair {} {\n\
$self instvar ns_ agent_ sender_ msgid_ ignore_ eventID_\n\
if [info exists eventID_] {\n\
$self serviceTime\n\
set ignore_ [expr [$ns_ now] + 3 * [$self distance? $sender_]]\n\
$ns_ at $ignore_ \"$agent_ clear $self $sender_ $msgid_\"\n\
$self cancel REPAIR\n\
$self evTrace Q REPAIR IGNORES $ignore_\n\
} else {		;# we must be in the 3dS,B holdDown interval\n\
$self instvar statistics_\n\
incr statistics_(dupREPR)\n\
}\n\
}\n\
\n\
SRM/repair instproc init args {\n\
eval $self next $args\n\
$self array set statistics_ \"dupRQST 0 dupREPR 0 #sent 0\"\n\
}\n\
\n\
SRM/repair instproc set-params args {\n\
eval $self next $args\n\
$self instvar agent_ requestor_\n\
foreach var {D1_ D2_} {\n\
if ![catch \"$agent_ set $var\" val] {\n\
$self instvar $var\n\
set $var $val\n\
}\n\
}\n\
$self distance? $requestor_\n\
$self evTrace P NACK from $requestor_\n\
}\n\
\n\
SRM/repair instproc dup-request? {} {\n\
return 0\n\
}\n\
\n\
SRM/repair instproc dup-repair? {} {\n\
$self instvar ns_ round_\n\
if {$round_ == 1} {		;# because repairs do not reschedule\n\
return 1\n\
} else {\n\
return 0\n\
}\n\
}\n\
SRM/repair instproc compute-delay {} {\n\
$self instvar D1_ D2_\n\
set rancomp [expr $D1_ + $D2_ * [uniform 0 1]]\n\
\n\
$self instvar requestor_\n\
set dist [$self distance? $requestor_]\n\
$self evTrace P INTERVALS D1 $D1_ D2 $D2_ d $dist\n\
set delay [expr $rancomp * $dist]\n\
}\n\
\n\
SRM/repair instproc schedule {} {\n\
$self instvar ns_ eventID_\n\
$self next\n\
set fireTime [expr [$ns_ now] + [$self compute-delay]]\n\
\n\
$self evTrace P RTIMER at $fireTime\n\
\n\
set eventID_ [$ns_ at $fireTime \"$self send-repair\"]\n\
}\n\
\n\
SRM/repair instproc cancel type {\n\
$self next\n\
if {$type == \"REQUEST\" || $type == \"REPAIR\"} {\n\
$self instvar agent_ round_\n\
if {$round_ == 1} {\n\
$agent_ update-ave rep [$self serviceTime]\n\
}\n\
}\n\
}\n\
\n\
SRM/repair instproc send-repair {} {\n\
$self instvar ns_ agent_ round_ sender_ msgid_ requestor_ sent_ round_\n\
$self evTrace P SENDREP\n\
\n\
$agent_ set requestor_ $requestor_\n\
$agent_ send repair $round_ $sender_ $msgid_\n\
\n\
$self instvar statistics_\n\
incr statistics_(#sent)\n\
set sent_ $round_\n\
}\n\
\n\
SRM/repair instproc recv-request {} {\n\
$self instvar statistics_\n\
incr statistics_(dupRQST)\n\
}\n\
\n\
SRM/repair instproc recv-repair {} {\n\
$self instvar ns_ agent_ round_ sender_ msgid_ eventID_ requestor_\n\
if [info exists eventID_] {\n\
set holdDown [expr [$ns_ now] +		\\\n\
3 * [$self distance? $requestor_]]\n\
$ns_ at $holdDown \"$agent_ clear $self $sender_ $msgid_\"\n\
$self cancel REPAIR\n\
$self evTrace P REPAIR IGNORES $holdDown\n\
} else {		;# we must in the 3dS,B holdDown interval\n\
$self instvar statistics_\n\
incr statistics_(dupREPR)\n\
}\n\
}\n\
\n\
SRM/session instproc init args {\n\
eval $self next $args\n\
$self instvar agent_ sessionDelay_ round_\n\
set sessionDelay_ [$agent_ set sessionDelay_]\n\
set round_ 1\n\
$self array set statistics_ \"#sent 0\"\n\
\n\
$self set sender_ 0\n\
$self set msgid_  0\n\
}\n\
\n\
SRM/session instproc delete {} {\n\
$self instvar $ns_ eventID_\n\
$ns_ cancel $eventID_\n\
$self next\n\
}\n\
\n\
SRM/session instproc schedule {} {\n\
$self instvar ns_ agent_ sessionDelay_ eventID_\n\
\n\
$self next\n\
\n\
set fireTime [expr $sessionDelay_ * [uniform 0.9 1.1]]\n\
\n\
set eventID_ [$ns_ at [expr [$ns_ now] + $fireTime]		\\\n\
\"$self send-session\"]\n\
}\n\
\n\
SRM/session instproc send-session {} {\n\
$self instvar agent_ statistics_\n\
$agent_ send session\n\
$self evTrace S SESSION\n\
incr statistics_(#sent)\n\
$self schedule\n\
}\n\
\n\
SRM/session instproc evTrace args {}	;# because I don't want to trace\n\
\n\
Class SRM/session/log-scaled -superclass SRM/session\n\
SRM/session/log-scaled instproc schedule {} {\n\
$self instvar ns_ agent_ sessionDelay_ eventID_\n\
\n\
set fireTime [expr $sessionDelay_ * [uniform 0.9 1.1] * \\\n\
(1 + log([$agent_ set groupSize_])) ]\n\
\n\
set eventID_ [$ns_ at [expr [$ns_ now] + $fireTime]		\\\n\
\"$self send-session\"]\n\
}\n\
Agent/SRM/SSM set group_scope_ 32\n\
Agent/SRM/SSM set local_scope_ 2\n\
Agent/SRM/SSM set scope_flag_  2\n\
Agent/SRM/SSM set rep_id_ 0\n\
Agent/SRM/SSM set numrep_ 0\n\
Agent/SRM/SSM set repthresh_up_ 100\n\
Agent/SRM/SSM set repthresh_low_ 7\n\
Agent/SRM/SSM set Z1_ 1.5\n\
Agent/SRM/SSM set S1_ 0.0\n\
Agent/SRM/SSM set S2_ 3.0\n\
\n\
Agent/SRM/SSM instproc init {} {\n\
$self next\n\
$self instvar numrep_ numloc_ repthresh_up_ repthresh_low_ Z1_ \\\n\
S1_ S2_\n\
set numrep_ 0\n\
set numloc_ 0\n\
set repthresh_up_ [$class set repthresh_up_]\n\
set repthresh_low_ [$class set repthresh_low_]\n\
set Z1_ [$class set Z1_]\n\
set S1_ [$class set S1_]\n\
set S2_ [$class set S2_]\n\
}\n\
\n\
Agent/SRM/SSM instproc start {} {\n\
$self next \n\
$self instvar deactivateID_ sessionDelay_ ns_\n\
set now [expr [$ns_ now]]\n\
set deactivateID_ [$ns_ at [expr $now + 3 * $sessionDelay_] \\\n\
\"$self deactivate-reps $now\"]\n\
}\n\
\n\
\n\
Agent/SRM/SSM instproc repid { rep} {\n\
\n\
$self instvar rep_id_\n\
$self set rep_id_ [$rep set addr_]\n\
$self ch-rep \n\
\n\
}\n\
\n\
Agent/SRM/SSM instproc member-scope {scope } {\n\
$self instvar scope_flag_\n\
$self set scope_flag_ $scope\n\
\n\
}\n\
\n\
Agent/SRM/SSM instproc local-member? {} {\n\
$self instvar scope_flag_\n\
if {$scope_flag_ == 1 } {\n\
return 1\n\
} else {\n\
return 0\n\
}\n\
}\n\
\n\
Agent/SRM/SSM instproc global-member? {} {\n\
$self instvar scope_flag_\n\
if {$scope_flag_ == 2 } {\n\
return 1\n\
} else {\n\
return 0\n\
}\n\
}\n\
\n\
Agent/SRM/SSM instproc local-member {} {\n\
$self member-scope 1	\n\
}\n\
\n\
Agent/SRM/SSM instproc global-rep {} {\n\
$self member-scope 2\n\
set rep_id_ [$self set addr_]\n\
$self ch-rep\n\
}\n\
\n\
Agent/SRM/SSM instproc set-local-scope {scope} {\n\
$self instvar local_scope_\n\
$self set local_scope_ $scope\n\
}\n\
\n\
Agent/SRM/SSM instproc set-global-scope {scope} {\n\
$self instvar global-scope\n\
$self set global-scope $scope\n\
}\n\
\n\
Agent/SRM/SSM instproc set-repid {rep} {\n\
$self instvar rep_id_\n\
$self set rep_id_ [$rep set addr_]\n\
$self ch-rep \n\
}\n\
\n\
Agent/SRM/SSM instproc dump-reps {} {\n\
$self instvar ns_ activerep_ numrep_\n\
puts \"[ft $ns_ $self] numreps: $numrep_\"\n\
if [info exists activerep_] {\n\
foreach i [array names activerep_] {\n\
set rtime [$activerep_($i) set recvTime_]\n\
set ttl [$activerep_($i) set ttl_]\n\
puts \"rep: $i recvtime: [ftime $rtime] ttl: $ttl\"\n\
}\n\
\n\
}\n\
}\n\
\n\
Agent/SRM/SSM instproc dump-locs {} {\n\
$self instvar ns_ activeloc_ numloc_\n\
puts \"[ft $ns_ $self] numlocs: $numloc_\"\n\
if [info exists activeloc_] {\n\
foreach i [array names activeloc_] {\n\
set rtime [$activeloc_($i) set recvTime_]\n\
set ttl [$activeloc_($i) set ttl_]\n\
set repid [$activeloc_($i) set repid_]\n\
puts \"loc: $i recvtime: [ftime $rtime] ttl: \\\n\
$ttl repid: $repid\"\n\
}\n\
\n\
}\n\
}\n\
\n\
\n\
Agent/SRM/SSM instproc send-session {} {\n\
$self instvar session_\n\
$session_ send-session\n\
}\n\
\n\
\n\
\n\
\n\
Agent/SRM/SSM instproc repchange-action {} {\n\
$self instvar rep_id_ tentativerep_ tentativettl_\n\
$self instvar ns_\n\
$self cur-num-reps\n\
set rep_id_ $tentativerep_\n\
puts \"[ft $ns_ $self] chrep rep : $tentativerep_\\\n\
ttl : $tentativettl_\"\n\
$self set-local-scope $tentativettl_\n\
$self local-member\n\
$self ch-rep\n\
$self send-session\n\
}\n\
\n\
\n\
Agent/SRM/SSM instproc recv-lsess {sender repid ttl} {\n\
\n\
$self instvar activeloc_ ns_ numloc_ sessionDelay_ deactivatelocID_\n\
$self instvar activerep_ numrep_\n\
$self instvar ch_localID_ tentativerep_ addr_ rep_id_ tentativettl_\n\
\n\
if [info exists activeloc_($sender)] {\n\
$activeloc_($sender) recv-lsess $repid $ttl\n\
} else {\n\
set activeloc_($sender) [new SRMinfo/loc $sender]\n\
incr numloc_\n\
$activeloc_($sender) set-params $ns_ $self\n\
$activeloc_($sender) recv-lsess	$repid $ttl\n\
}\n\
\n\
\n\
if [info exists activerep_($sender)] {\n\
delete $activerep_($sender)\n\
unset activerep_($sender)\n\
incr numrep_ -1\n\
if [info exists ch_localID_] {\n\
if {[info exists tentativerep_] && $tentativerep_ == $sender } {\n\
$self cur-num-reps\n\
}\n\
if { $repid == $addr_} {\n\
$ns_ cancel $ch_localID_\n\
$self unset ch_localID_\n\
$self check-status\n\
}\n\
}\n\
if { [$self local-member?]} {\n\
if { $sender == $rep_id_} {\n\
$self repchange-action\n\
}\n\
} else {\n\
if { $sender == $rep_id_} {\n\
puts \"[ft $ns_ $self] error\"\n\
\n\
}\n\
}			\n\
}\n\
\n\
set time [expr [$ns_ now] - 3 * $sessionDelay_]\n\
if [info exists deactivatelocID_] {\n\
$ns_ cancel $deactivatelocID_\n\
unset deactivatelocID_\n\
}\n\
$self deactivate-locs $time\n\
}\n\
\n\
\n\
Agent/SRM/SSM instproc recv-gsess {sender ttl} {\n\
$self instvar activerep_ ns_ numrep_ sessionDelay_\n\
$self instvar deactivateID_ local_scope_\n\
\n\
$self instvar activeloc_ numloc_\n\
if [info exists activerep_($sender)] {\n\
$activerep_($sender) recv-gsess $ttl\n\
} else {\n\
set activerep_($sender) [new SRMinfo/rep $sender]\n\
incr numrep_\n\
$activerep_($sender) set-params $ns_ $self\n\
$activerep_($sender) recv-gsess	$ttl\n\
}\n\
set time [expr [$ns_ now] - 3 * $sessionDelay_]\n\
if [info exists deactivateID_] {\n\
$ns_ cancel $deactivateID_\n\
unset deactivateID_\n\
}\n\
if [info exists activeloc_($sender)] {\n\
delete $activeloc_($sender)\n\
unset activeloc_($sender)\n\
incr numloc_ -1\n\
}\n\
if { [$self local-member?]} {\n\
if {$ttl < $local_scope_} {\n\
set rep_id_ $sender\n\
puts \"[ft $ns_ $self] closerrep rep : $sender \\\n\
ttl : $ttl\"\n\
$self set-local-scope $ttl\n\
$self local-member\n\
$self ch-rep\n\
$self send-session\n\
}			\n\
}\n\
$self deactivate-reps $time\n\
$self check-status\n\
}\n\
\n\
Agent/SRM/SSM instproc bias {} {\n\
$self instvar activerep_  ns_ sessionDelay_\n\
set now [expr [$ns_ now]]\n\
set biasfactor 0\n\
set time [expr $now - 1.5 * $sessionDelay_]\n\
if [info exists activerep_] {\n\
foreach i [array names activerep_] {\n\
set rtime [$activerep_($i) set recvTime_]\n\
if { $rtime >= $time} {\n\
incr biasfactor \n\
}\n\
}\n\
}\n\
return $biasfactor\n\
}\n\
\n\
Agent/SRM/SSM instproc my-loc {} {\n\
$self instvar activeloc_\n\
set num 0\n\
if [info exists activeloc_] {\n\
foreach i [array names activeloc_] {\n\
set repid [$activeloc_($i) set repid_]\n\
if { $repid == [$self set addr_]} {\n\
incr num\n\
}\n\
}\n\
}\n\
return $num\n\
}\n\
\n\
Agent/SRM/SSM instproc cur-num-reps {} {\n\
$self instvar activerep_  ns_ sessionDelay_ tentativerep_ tentativettl_ \n\
$self instvar Z1_\n\
set now [expr [$ns_ now]]\n\
set num 0\n\
set min_ttl 32\n\
set time [expr $now - $Z1_ * $sessionDelay_]\n\
if [info exists activerep_] {\n\
foreach i [array names activerep_] {\n\
set rtime [$activerep_($i) set recvTime_]\n\
set ttl [$activerep_($i) set ttl_]\n\
if { $rtime >= $time} {\n\
if {$min_ttl > $ttl} {\n\
set tentativerep_ $i\n\
set min_ttl $ttl\n\
}\n\
incr num\n\
}\n\
}\n\
}\n\
set tentativettl_ $min_ttl\n\
return $num\n\
}\n\
\n\
Agent/SRM/SSM instproc compute-localdelay {} {\n\
$self instvar S1_ S2_ sessionDelay_\n\
set num [$self my-loc]\n\
if {$num > 0} {\n\
set rancomp [expr $S1_+ 1 + $S2_ * [uniform 0 1]]\n\
} else {\n\
set rancomp [expr $S1_+ $S2_ * [uniform 0 1]]\n\
}\n\
set delay [expr $rancomp * $sessionDelay_]\n\
return $delay\n\
}\n\
\n\
Agent/SRM/SSM instproc compute-globaldelay {} {\n\
$self instvar S1_ S2_ sessionDelay_\n\
set rancomp [expr $S1_ + $S2_ * [uniform 0 1]]\n\
set delay [expr $rancomp * $sessionDelay_]\n\
return $delay\n\
}\n\
\n\
\n\
\n\
Agent/SRM/SSM instproc schedule-ch-local {} {\n\
$self instvar ns_ ch_localID_\n\
set now [$ns_ now]\n\
set delay [$self compute-localdelay]\n\
set fireTime [expr $now + $delay]\n\
if [info exists ch_localID_] {\n\
puts \"[new_ft $ns_ $self] scheduled called without cancel\"\n\
$ns_ cancel $ch_localID_\n\
unset ch_localID_\n\
}\n\
\n\
set ch_localID_ [$ns_ at $fireTime \"$self ch-local\"]\n\
puts \"[ft $ns_ $self] schlocal [ftime $fireTime] evid : $ch_localID_\"\n\
\n\
}\n\
\n\
Agent/SRM/SSM instproc schedule-ch-global {} {\n\
$self instvar ns_ ch_globalID_\n\
set now [$ns_ now]\n\
set delay [$self compute-globaldelay]\n\
set fireTime [expr $now + $delay]\n\
if [info exists ch_globalID_] {\n\
puts \"[ft $ns_ $self] glbscheduled called without cancel\"\n\
$ns_ cancel $ch_globalID_\n\
unset ch_globalID_\n\
}\n\
\n\
set ch_globalID_ [$ns_ at $fireTime \"$self ch-global\"]\n\
puts \"[ft $ns_ $self] schglobal [ftime $fireTime] evid : $ch_globalID_\"\n\
\n\
}\n\
\n\
\n\
\n\
\n\
Agent/SRM/SSM instproc check-status {} {\n\
$self instvar ns_ numrep_ repthresh_up_ ch_localID_\n\
$self instvar ch_globalID_ repthresh_low_\n\
if { $numrep_ > $repthresh_up_ }  {\n\
if [info exists ch_localID_] {\n\
return;\n\
}\n\
if { [$self local-member?]} {\n\
if [info exists ch_globalID_] {\n\
$ns_ cancel $ch_globalID_\n\
unset ch_globalID_\n\
}\n\
return;\n\
}\n\
$self schedule-ch-local\n\
return;\n\
}\n\
if {$numrep_ < $repthresh_low_} {\n\
if [info exists ch_globalID_] {\n\
return;\n\
}\n\
if { [$self global-member?]} {\n\
if [info exists ch_localID_] {\n\
$ns_ cancel $ch_localID_\n\
unset ch_localID_\n\
}\n\
return;\n\
}\n\
$self schedule-ch-global\n\
return;\n\
}\n\
if [info exists ch_localID_] {\n\
$ns_ cancel $ch_localID_\n\
unset ch_localID_\n\
}\n\
if [info exists ch_globalID_] {\n\
$ns_ cancel $ch_globalID_\n\
unset ch_globalID_\n\
}\n\
\n\
}\n\
\n\
\n\
\n\
Agent/SRM/SSM instproc ch-local {} {\n\
$self instvar repthresh_up_ tentativerep_ tentativettl_ ns_ rep_id_\n\
if {[$self cur-num-reps] > $repthresh_up_} {\n\
set rep_id_ $tentativerep_\n\
puts \"[ft $ns_ $self] chlocal rep : $tentativerep_\\\n\
ttl : $tentativettl_\"\n\
$self local-member\n\
$self ch-rep\n\
$self send-session\n\
$self set-local-scope $tentativettl_\n\
}\n\
if [info exists ch_localID_] {	\n\
$ns_ cancel ch_localID_\n\
unset ch_localID_\n\
}\n\
}\n\
\n\
Agent/SRM/SSM instproc ch-global {} {\n\
$self instvar repthresh_low_ tentativerep_ tentativettl_ ns_ rep_id_\n\
if {[$self cur-num-reps] < $repthresh_low_} {\n\
set rep_id_ [$self set addr_]\n\
puts \"[ft $ns_ $self] chglobal rep : $rep_id_\\\n\
ttl : $tentativettl_\"\n\
$self set-local-scope 0\n\
$self global-rep\n\
$self ch-rep\n\
$self send-session\n\
}\n\
if [info exists ch_globalID_] {	\n\
$ns_ cancel ch_globalID_\n\
unset ch_globalID_\n\
}\n\
}\n\
\n\
\n\
Agent/SRM/SSM instproc deactivate-reps {time} {\n\
$self instvar numrep_ activerep_ deactivateID_ ns_\n\
$self instvar sessionDelay_ rep_id_\n\
if [info exists activerep_] {\n\
foreach i [array names activerep_] {\n\
set rtime [$activerep_($i) set recvTime_]\n\
if { $rtime < $time} {\n\
delete $activerep_($i)\n\
unset activerep_($i)\n\
incr numrep_ -1\n\
if { $i == $rep_id_ } {\n\
puts \"[ft $ns_ $self] $i == $rep_id_\" \n\
$self repchange-action\n\
}\n\
}\n\
}\n\
if {$numrep_ <= 0} {\n\
unset activerep_\n\
}\n\
}\n\
set now [expr [$ns_ now]]\n\
set deactivateID_ [$ns_ at [expr $now + 3 * $sessionDelay_] \\\n\
\"$self deactivate-reps $now\"]\n\
}\n\
\n\
Agent/SRM/SSM instproc deactivate-locs {time} {\n\
$self instvar numloc_ activeloc_ deactivatelocID_ ns_\n\
$self instvar sessionDelay_ local_scope_\n\
set maxttl 0\n\
if [info exists activeloc_] {\n\
foreach i [array names activeloc_] {\n\
set rtime [$activeloc_($i) set recvTime_]\n\
if { $rtime < $time} {\n\
delete $activeloc_($i)\n\
unset activeloc_($i)\n\
incr numloc_ -1\n\
} else {\n\
if { [$self global-member?] } {\n\
set ttl [$activeloc_($i) set ttl_]\n\
if {$maxttl < $ttl} {\n\
set maxttl $ttl\n\
}\n\
set local_scope_ $maxttl\n\
}\n\
}\n\
}\n\
if {$numloc_ <= 0} {\n\
unset activeloc_\n\
}\n\
}\n\
set now [expr [$ns_ now]]\n\
set deactivatelocID_ [$ns_ at [expr $now + 3 * $sessionDelay_] \\\n\
\"$self deactivate-locs $now\"]\n\
}\n\
\n\
\n\
\n\
\n\
Class SRMinfo\n\
\n\
SRMinfo set recvTime_ 0\n\
\n\
SRMinfo instproc init {sender} {\n\
$self next\n\
$self instvar sender_ \n\
set sender_ $sender\n\
}\n\
\n\
SRMinfo instproc set-params {ns agent} {\n\
$self instvar ns_ agent_\n\
set ns_ $ns\n\
set agent_ $agent\n\
}\n\
\n\
Class SRMinfo/rep -superclass SRMinfo\n\
\n\
SRMinfo/rep instproc recv-gsess {ttl} {\n\
$self instvar recvTime_ ns_ ttl_\n\
set now [$ns_ now]\n\
set recvTime_ [expr $now]\n\
set ttl_ [expr $ttl]\n\
}\n\
\n\
Class SRMinfo/loc -superclass SRMinfo\n\
\n\
\n\
SRMinfo/loc instproc recv-lsess {repid ttl} {\n\
$self instvar recvTime_ ns_ ttl_ repid_\n\
set now [$ns_ now]\n\
set recvTime_ [expr $now]\n\
set ttl_ [expr $ttl]\n\
set repid_ [expr $repid]\n\
}\n\
\n\
Agent/MFTP/Snd set dtuSize_ 1424            ;# default size of DTUs (in bytes)\n\
Agent/MFTP/Snd set dtusPerBlock_ 1472       ;# default number of DTUs per block\n\
Agent/MFTP/Snd set dtusPerGroup_ 8          ;# default group size\n\
Agent/MFTP/Snd set fileSize_ 1000000        ;# default file size in bytes\n\
Agent/MFTP/Snd set readAheadBufsize_ 2097152;# default size of read-ahead buffer in bytes\n\
Agent/MFTP/Snd set interval_ 512000         ;# default transmission rate is 512kbps\n\
Agent/MFTP/Snd set txStatusLimit_ 100       ;# default max. number of consecutive status requests without NAK\n\
Agent/MFTP/Snd set txStatusDelay_ 2         ;# default time to wait for status responses after a request before polling again\n\
Agent/MFTP/Snd set rspBackoffWindow_ 1      ;# default max. time for receivers to wait before replying with nak(s) after a request\n\
Agent/MFTP/Snd set reply_addr_ undefined    ; # application _must_ specify the sender address (i.e. the one\n\
;# to which NAKs are unicast to). Default is \"undefined\"\n\
Agent/MFTP/Snd set reply_port_ undefined\n\
\n\
Agent/MFTP/Snd set nakCount_ 0\n\
Agent/MFTP/Snd set seekCount_ 0             ;# number of disk seeks performed\n\
\n\
Agent/MFTP/Snd instproc init {} {\n\
$self next\n\
$self instvar ns_ dtuSize_ dtusPerBlock_ dtusPerGroup_ fileSize_ \n\
$self instvar reply_addr_ reply_port_ readAheadBufsize_ interval_ \n\
$self instvar txStatusLimit_ txStatusDelay_ rspBackoffWindow_ nakCount_ \n\
$self instvar seekCount_\n\
\n\
set ns_ [Simulator instance]\n\
foreach var { dtuSize_ dtusPerBlock_ dtusPerGroup_ fileSize_ \\\n\
readAheadBufsize_ interval_ txStatusLimit_ \\\n\
txStatusDelay_ rspBackoffWindow_ nakCount_ seekCount_ } {\n\
$self init-instvar $var\n\
}\n\
}\n\
\n\
Agent/MFTP/Snd instproc send-data { } {\n\
$self instvar ns_ interval_\n\
if { [$self cmd send data] != -1 } {\n\
$ns_ at [expr [$ns_ now] + $interval_] \"$self send-data\"\n\
}\n\
}\n\
\n\
Agent/MFTP/Snd instproc start {} {\n\
$self instvar node_ dst_addr_\n\
\n\
set dst_addr_ [expr $dst_addr_]           ;# get rid of possibly leading 0x etc.\n\
\n\
$self cmd start\n\
$node_ join-group $self $dst_addr_\n\
$self send-data\n\
}\n\
\n\
Agent/MFTP/Snd instproc pass-finished { CurrentPass NbBlocks } {\n\
$self instvar ns_ dtusPerGroup_ interval_ tx_status_requests_ rspBackoffWindow_\n\
\n\
set tx_status_requests_ 0       ;# number of consecutively sent status requests\n\
if { $CurrentPass >= $dtusPerGroup_ - 1 } {\n\
$self send status-req $CurrentPass 0 [expr $NbBlocks-1] $rspBackoffWindow_\n\
} else {\n\
$ns_ at [expr [$ns_ now] + $interval_] \"$self send-data\"\n\
}\n\
}\n\
\n\
\n\
Agent/MFTP/Snd instproc send-status-req { CurrentPass blockLo blockHi rspBackoffWindow } {\n\
$self instvar ns_ tx_status_requests_ txStatusDelay_\n\
\n\
$self cmd send statreq $CurrentPass $blockLo $blockHi $rspBackoffWindow\n\
incr tx_status_requests_\n\
$ns_ at [expr [$ns_ now] + $txStatusDelay_] \\\n\
\"$self status-rsp-pending $CurrentPass $blockLo $blockHi\"\n\
}\n\
\n\
\n\
Agent/MFTP/Snd instproc status-rsp-pending { CurrentPass blockLo blockHi } {\n\
$self instvar nakCount_ tx_status_requests_ txStatusLimit_ rspBackoffWindow_\n\
\n\
if { $nakCount_ > 0 } {\n\
set nakCount_ 0\n\
$self send-data\n\
} elseif { $tx_status_requests_ < $txStatusLimit_ } {\n\
$self send status-req $CurrentPass $blockLo $blockHi $rspBackoffWindow_\n\
} else {\n\
$self done\n\
}\n\
}\n\
\n\
\n\
Agent/MFTP/Snd instproc recv { type args } {\n\
eval $self evTrace $proc $type $args\n\
eval $self $proc-$type $args\n\
}\n\
\n\
Agent/MFTP/Snd instproc send { type args } {\n\
eval $self evTrace $proc $type $args\n\
eval $self $proc-$type $args\n\
}\n\
\n\
Agent/MFTP/Snd instproc send-notify { args } {\n\
}\n\
\n\
Agent/MFTP/Snd instproc recv-nak { passNb block_nb nak_count} {\n\
}\n\
\n\
\n\
Agent/MFTP/Snd instproc done {} {\n\
}\n\
\n\
\n\
Agent/MFTP/Snd instproc trace fd {\n\
$self instvar trace_\n\
set trace_ $fd\n\
}\n\
\n\
\n\
Agent/MFTP/Snd instproc delete {} {\n\
}\n\
\n\
Agent/MFTP/Snd instproc evTrace { op type args } {\n\
$self instvar trace_ ns_\n\
if [info exists trace_] {\n\
puts $trace_ [format \"%7.4f [[$self set node_] id] $op-$type $args\" [$ns_ now]]\n\
}\n\
}\n\
\n\
\n\
Agent/MFTP/Rcv set dtuSize_ 1424\n\
Agent/MFTP/Rcv set dtusPerBlock_ 1472\n\
Agent/MFTP/Rcv set dtusPerGroup_ 8\n\
Agent/MFTP/Rcv set fileSize_ 1000000\n\
Agent/MFTP/Rcv set nakCount_ 0\n\
Agent/MFTP/Rcv set seekCount_ 0\n\
Agent/MFTP/Rcv set reply_addr_ 0           ; # unicast reply addr (=address of server)\n\
Agent/MFTP/Rcv set reply_port_ 0           ; # unicast reply addr (=address of server)\n\
\n\
Agent/MFTP/Rcv instproc init {} {\n\
$self next\n\
$self instvar ns_ dtuSize_ dtusPerBlock_ dtusPerGroup_ fileSize_ \n\
$self instvar reply_addr_ reply_port_ nakCount_ seekCount_\n\
\n\
set ns_ [Simulator instance]\n\
foreach var { dtuSize_ dtusPerBlock_ dtusPerGroup_ fileSize_ reply_addr_ reply_port_ nakCount_ seekCount_} {\n\
$self init-instvar $var\n\
}\n\
}\n\
\n\
Agent/MFTP/Rcv instproc start {} {\n\
$self instvar node_ dst_addr_\n\
\n\
set dst_addr_ [expr $dst_addr_]           ;# get rid of possibly leading 0x etc.\n\
$self cmd start\n\
$node_ join-group $self $dst_addr_\n\
}\n\
\n\
Agent/MFTP/Rcv instproc delete {} {\n\
}\n\
\n\
Agent/MFTP/Rcv instproc done-notify { args } {\n\
\n\
$self instvar node_ dst_addr_\n\
eval $self evTrace done notify $args\n\
$node_ leave-group $self $dst_addr_\n\
}\n\
\n\
Agent/MFTP/Rcv instproc recv { type args } {\n\
eval $self evTrace $proc $type $args\n\
eval $self recv-$type $args\n\
}\n\
\n\
\n\
Agent/MFTP/Rcv instproc recv-dependent { CurrentPass CurrentGroup CwPat } {\n\
}\n\
\n\
Agent/MFTP/Rcv instproc recv-group-full { CurrentPass CurrentGroup CwPat } {\n\
}\n\
\n\
Agent/MFTP/Rcv instproc recv-useful { CurrentPass CurrentGroup CwPat } {\n\
}\n\
\n\
Agent/MFTP/Rcv instproc recv-status-req { passNb blockLo blockHi txStatusDelay } {\n\
$self instvar ns_\n\
set backoff [uniform 0 $txStatusDelay]\n\
$ns_ at [expr [$ns_ now] + $backoff] \"$self send-nak [list $passNb $blockLo $blockHi]\"\n\
}\n\
\n\
Agent/MFTP/Rcv instproc send-nak { passNb blockLo blockHi } {\n\
while { $blockLo <= $blockHi } {\n\
set bit_count [$self cmd send nak $passNb $blockLo]\n\
$self evTrace send nak $passNb $blockLo $bit_count\n\
incr blockLo\n\
}\n\
}\n\
\n\
Agent/MFTP/Rcv instproc trace fd {\n\
$self instvar trace_\n\
set trace_ $fd\n\
}\n\
\n\
Agent/MFTP/Rcv instproc evTrace { op type args } {\n\
$self instvar trace_ ns_\n\
if [info exists trace_] {\n\
puts $trace_ [format \"%7.4f [[$self set node_] id] $op-$type $args\" [$ns_ now]]\n\
}\n\
}\n\
\n\
\n\
Class Agent/MFTP/Rcv/Stat -superclass Agent/MFTP/Rcv\n\
\n\
Agent/MFTP/Rcv/Stat instproc init { } {\n\
$self instvar nb_useful_recv nb_full_disc nb_lin_dep_disc\n\
\n\
$self next\n\
foreach var [list nb_useful_recv nb_full_disc nb_lin_dep_disc] {\n\
set $var 0\n\
}\n\
}\n\
\n\
Agent/MFTP/Rcv/Stat instproc recv-useful { CurrentPass CurrentGroup CwPat } {\n\
$self instvar nb_useful_recv\n\
\n\
puts stdout \"recv-useful!\"\n\
$self next $CurrentPass $CurrentGroup $CwPat\n\
\n\
incr nb_useful_recv\n\
}\n\
\n\
\n\
Agent/MFTP/Rcv/Stat instproc recv-group-full { CurrentPass CurrentGroup CwPat } {\n\
$self instvar nb_full_disc\n\
\n\
puts stdout \"recv-group-full!\"\n\
$self next $CurrentPass $CurrentGroup $CwPat\n\
\n\
incr nb_full_disc\n\
}\n\
\n\
\n\
Agent/MFTP/Rcv/Stat instproc recv-dependent { CurrentPass CurrentGroup CwPat } {\n\
$self instvar nb_lin_dep_disc\n\
\n\
puts stdout \"recv-dependent!\"\n\
$self next $CurrentPass $CurrentGroup $CwPat\n\
\n\
incr nb_lin_dep_disc\n\
}\n\
\n\
Agent/MFTP/Rcv/Stat instproc done-notify { args } {\n\
$self instvar nb_useful_recv nb_full_disc nb_lin_dep_disc\n\
eval $self next $args $nb_useful_recv $nb_full_disc $nb_lin_dep_disc\n\
}\n\
Class McastMonitor\n\
\n\
McastMonitor instproc init {} {\n\
$self instvar period_ ns_\n\
\n\
set ns_ [Simulator instance]\n\
set period_ 0.03\n\
}\n\
\n\
McastMonitor instproc trace-topo {} {\n\
$self instvar ns_ period_\n\
\n\
$self trace-links [$ns_ all-links-list]\n\
}\n\
\n\
McastMonitor instproc trace-links links {\n\
$self instvar pktmon_\n\
\n\
foreach l $links {\n\
set pktmon_($l) [new PktInTranMonitor]\n\
$pktmon_($l) attach-link $l\n\
$l add-pktmon $pktmon_($l)\n\
}\n\
}\n\
\n\
McastMonitor instproc filter {header field value} {\n\
$self instvar pktmon_\n\
\n\
foreach index [array name pktmon_] {\n\
$pktmon_($index) filter $header $field $value\n\
}\n\
}\n\
\n\
McastMonitor instproc pktintran {} {\n\
$self instvar ns_ pktmon_\n\
\n\
set total 0\n\
foreach index [array name pktmon_] {\n\
if {[$index up?] == \"up\"} {\n\
incr total [$pktmon_($index) pktintran]\n\
}\n\
}\n\
return $total\n\
}\n\
\n\
McastMonitor instproc print-trace {} {\n\
$self instvar ns_ period_ file_\n\
\n\
if [info exists file_] {\n\
puts $file_ \"[$ns_ now] [$self pktintran]\"\n\
} else {\n\
puts \"[$ns_ now] [$self pktintran]\"\n\
}\n\
$ns_ at [expr [$ns_ now] + $period_] \"$self print-trace\"\n\
}\n\
\n\
McastMonitor instproc attach file {\n\
$self instvar file_\n\
set file_ $file\n\
}\n\
\n\
\n\
Class PktInTranMonitor\n\
\n\
PktInTranMonitor instproc init {} {\n\
$self instvar period_ ns_ front_counter_ rear_counter_ front_filter_ rear_filter_ \n\
set ns_ [Simulator instance]\n\
set period_ 0.03\n\
set front_counter_ [new PktCounter]\n\
$front_counter_ set pktInTranMonitor_ $self\n\
set front_filter_ [new Filter/MultiField]\n\
$front_filter_ filter-target $front_counter_\n\
\n\
\n\
set rear_counter_ [new PktCounter]\n\
$rear_counter_ set pktInTranMonitor_ $self\n\
set rear_filter_ [new Filter/MultiField]\n\
$rear_filter_ filter-target $rear_counter_\n\
}\n\
\n\
PktInTranMonitor instproc reset {} {\n\
$self instvar front_counter_ rear_counter_  ns_ next_\n\
$front_counter_ reset\n\
$rear_counter_ reset\n\
if {[info exist next_] && $next_ != 0} {\n\
$next_ reset\n\
}\n\
}\n\
\n\
PktInTranMonitor instproc filter {header field value} {\n\
$self instvar front_filter_ rear_filter_\n\
$front_filter_ filter-field [PktHdr_offset PacketHeader/$header $field] $value\n\
$rear_filter_ filter-field [PktHdr_offset PacketHeader/$header $field] $value\n\
}\n\
\n\
PktInTranMonitor instproc attach-link link {\n\
$self instvar front_filter_ rear_filter_ front_counter_ rear_counter_\n\
\n\
set tmp [$link head]\n\
while {[$tmp target] != [$link link]} {\n\
set tmp [$tmp target]\n\
}\n\
\n\
$tmp target $front_filter_\n\
$front_filter_ target [$link link]\n\
$front_counter_ target [$link link]\n\
\n\
$rear_filter_ target [[$link link] target]\n\
$rear_counter_ target [[$link link] target]\n\
[$link link] target $rear_filter_\n\
}\n\
\n\
PktInTranMonitor instproc attach file {\n\
$self instvar file_\n\
set file_ $file\n\
}\n\
\n\
PktInTranMonitor instproc pktintran {} {\n\
$self instvar front_counter_ rear_counter_ \n\
return [expr [$front_counter_ value] - [$rear_counter_ value]]\n\
}\n\
\n\
PktInTranMonitor instproc output {} {\n\
$self instvar front_counter_ rear_counter_ ns_ file_ \n\
\n\
puts $file_ \"[$ns_ now] [expr [$front_counter_ value] - [$rear_counter_ value]]\"\n\
}\n\
\n\
\n\
PktInTranMonitor instproc periodical-output {} {\n\
$self instvar period_ ns_\n\
\n\
$self output\n\
$ns_ at [expr [$ns_ now] + $period_] \"$self periodical-output\"\n\
}\n\
\n\
Simulator instproc all-links-list {} {\n\
$self instvar link_\n\
set links \"\"\n\
foreach n [array names link_] {\n\
lappend links $link_($n)\n\
}\n\
set links\n\
}\n\
\n\
Link instproc add-pktmon pktmon {\n\
$self instvar pktmon_\n\
\n\
if [info exists pktmon_] {\n\
$pktmon set next_ $pktmon_\n\
} else {\n\
$pktmon set next_ 0\n\
}\n\
set pktmon_ $pktmon\n\
}\n\
\n\
\n\
set rlm_param(alpha) 4\n\
set rlm_param(alpha) 2\n\
set rlm_param(beta) 0.75\n\
set rlm_param(init-tj) 1.5\n\
set rlm_param(init-tj) 10\n\
set rlm_param(init-tj) 5\n\
set rlm_param(init-td) 5\n\
set rlm_param(init-td-var) 2\n\
set rlm_param(max) 600\n\
set rlm_param(max) 60\n\
set rlm_param(g1) 0.25\n\
set rlm_param(g2) 0.25\n\
\n\
\n\
\n\
Class MMG\n\
\n\
MMG instproc init { levels } {\n\
$self next\n\
\n\
$self instvar debug_ env_ maxlevel_\n\
set debug_ 0\n\
set env_ [lindex [split [$self info class] /] 1]\n\
set maxlevel_ $levels\n\
\n\
global rlm_debug_flag\n\
if [info exists rlm_debug_flag] {\n\
set debug_ $rlm_debug_flag\n\
}\n\
\n\
$self instvar TD TDVAR state_ subscription_\n\
global rlm_param\n\
set TD $rlm_param(init-td)\n\
set TDVAR $rlm_param(init-td-var)\n\
set state_ /S\n\
\n\
$self instvar layer_ layers_\n\
set i 1\n\
while { $i <= $maxlevel_ } {\n\
set layer_($i) [$self create-layer [expr $i - 1]]\n\
lappend layers_ $layer_($i)\n\
incr i\n\
}\n\
\n\
set subscription_ 0\n\
$self add-layer\n\
\n\
set state_ /S\n\
\n\
$self set_TJ_timer\n\
}\n\
\n\
MMG instproc set-state s {\n\
$self instvar state_\n\
set old $state_\n\
set state_ $s\n\
$self debug \"FSM: $old -> $s\"\n\
}\n\
\n\
MMG instproc drop-layer {} {\n\
$self dumpLevel\n\
$self instvar subscription_ layer_\n\
set n $subscription_\n\
\n\
if { $n > 0 } {\n\
$self debug \"DRP-LAYER $n\"\n\
$layer_($n) leave-group \n\
incr n -1\n\
set subscription_ $n\n\
}\n\
$self dumpLevel\n\
}\n\
\n\
MMG instproc add-layer {} {\n\
$self dumpLevel\n\
$self instvar maxlevel_ subscription_ layer_\n\
set n $subscription_\n\
if { $n < $maxlevel_ } {\n\
$self debug \"ADD-LAYER\"\n\
incr n\n\
set subscription_ $n\n\
$layer_($n) join-group\n\
}\n\
$self dumpLevel\n\
}\n\
\n\
MMG instproc current_layer_getting_packets {} {\n\
$self instvar subscription_ layer_ TD\n\
set n $subscription_\n\
if { $n == 0 } {\n\
return 0\n\
}\n\
\n\
set l $layer_($subscription_)\n\
$self debug \"npkts [$l npkts]\"\n\
if [$l getting-pkts] {\n\
return 1\n\
}\n\
\n\
set delta [expr [$self now] - [$l last-add]]\n\
if { $delta > $TD } {\n\
set TD [expr 1.2 * $delta]\n\
}\n\
return 0\n\
}\n\
\n\
MMG instproc mmg_loss {} {\n\
$self instvar layers_\n\
set loss 0\n\
foreach l $layers_ {\n\
incr loss [$l nlost]\n\
}\n\
return $loss\n\
}\n\
\n\
MMG instproc mmg_pkts {} {\n\
$self instvar layers_\n\
set npkts 0\n\
foreach l $layers_ {\n\
incr npkts [$l npkts]\n\
}\n\
return $npkts\n\
}\n\
\n\
MMG instproc check-equilibrium {} {\n\
global rlm_param\n\
$self instvar subscription_ maxlevel_ layer_\n\
\n\
set n [expr $subscription_ + 1]\n\
if { $n >= $maxlevel_ || [$layer_($n) timer] >= $rlm_param(max) } {\n\
set eq 1\n\
} else {\n\
set eq 0\n\
}\n\
\n\
$self debug \"EQ $eq\"\n\
}\n\
\n\
MMG instproc backoff-one { n alpha } {\n\
$self debug \"BACKOFF $n by $alpha\"\n\
$self instvar layer_\n\
$layer_($n) backoff $alpha\n\
}\n\
\n\
MMG instproc backoff n {\n\
$self debug \"BACKOFF $n\"\n\
global rlm_param\n\
$self instvar maxlevel_ layer_\n\
set alpha $rlm_param(alpha)\n\
set L $layer_($n)\n\
$L backoff $alpha\n\
incr n\n\
while { $n <= $maxlevel_ } {\n\
$layer_($n) peg-backoff $L\n\
incr n\n\
}\n\
$self check-equilibrium\n\
}\n\
\n\
MMG instproc highest_level_pending {} {\n\
$self instvar maxlevel_\n\
set m \"\"\n\
set n 0\n\
incr n\n\
while { $n <= $maxlevel_ } {\n\
if [$self level_pending $n] {\n\
set m $n\n\
}\n\
incr n\n\
}\n\
return $m\n\
}\n\
\n\
MMG instproc rlm_update_D  D {\n\
global rlm_param\n\
$self instvar TD TDVAR\n\
\n\
set v [expr abs($D - $TD)]\n\
set TD [expr $TD * (1 - $rlm_param(g1)) \\\n\
+ $rlm_param(g1) * $D]\n\
set TDVAR [expr $TDVAR * (1 - $rlm_param(g2)) \\\n\
+ $rlm_param(g2) * $v]\n\
}\n\
\n\
MMG instproc exceed_loss_thresh {} {\n\
$self instvar h_npkts h_nlost\n\
set npkts [expr [$self mmg_pkts] - $h_npkts]\n\
if { $npkts >= 10 } {\n\
set nloss [expr [$self mmg_loss] - $h_nlost]\n\
set loss [expr double($nloss) / ($nloss + $npkts)]\n\
$self debug \"H-THRESH $nloss $npkts $loss\"\n\
if { $loss > 0.25 } {\n\
return 1\n\
}\n\
}\n\
return 0\n\
}\n\
\n\
MMG instproc enter_M {} {\n\
$self set-state /M\n\
$self set_TD_timer_wait\n\
$self instvar h_npkts h_nlost\n\
set h_npkts [$self mmg_pkts]\n\
set h_nlost [$self mmg_loss]\n\
}\n\
\n\
MMG instproc enter_D {} {\n\
$self set-state /D\n\
$self set_TD_timer_conservative\n\
}\n\
\n\
MMG instproc enter_H {} {\n\
$self set_TD_timer_conservative\n\
$self set-state /H\n\
}\n\
\n\
MMG instproc log-loss {} {\n\
$self debug \"LOSS [$self mmg_loss]\"\n\
\n\
$self instvar state_ subscription_ pending_ts_\n\
if { $state_ == \"/M\" } {\n\
if [$self exceed_loss_thresh] {\n\
$self cancel_timer TD\n\
$self drop-layer\n\
$self check-equilibrium\n\
$self enter_D\n\
}\n\
return\n\
}\n\
if { $state_ == \"/S\" } {\n\
$self cancel_timer TD\n\
set n [$self highest_level_pending]\n\
if { $n != \"\" } {\n\
$self backoff $n\n\
if { $n == $subscription_ } {\n\
set ts $pending_ts_($subscription_)\n\
$self rlm_update_D [expr [$self now] - $ts]\n\
$self drop-layer\n\
$self check-equilibrium\n\
$self enter_D\n\
return\n\
}\n\
if { $n == [expr $subscription_ + 1] } {\n\
$self cancel_timer TJ\n\
$self set_TJ_timer\n\
}\n\
}\n\
if [$self our_level_recently_added] {\n\
$self enter_M\n\
return\n\
}\n\
$self enter_H\n\
return\n\
}\n\
if { $state_ == \"/H\" || $state_ == \"/D\" } {\n\
return\n\
}\n\
puts stderr \"rlm state machine botched\"\n\
exit -1\n\
}\n\
\n\
MMG instproc relax_TJ {} {\n\
$self instvar subscription_ layer_\n\
if { $subscription_ > 0 } {\n\
$layer_($subscription_) relax\n\
$self check-equilibrium\n\
}\n\
}\n\
\n\
MMG instproc trigger_TD {} {\n\
$self instvar state_\n\
if { $state_ == \"/H\" } {\n\
$self enter_M\n\
return\n\
}\n\
if { $state_ == \"/D\" || $state_ == \"/M\" } {\n\
$self set-state /S\n\
$self set_TD_timer_conservative\n\
return\n\
}\n\
if { $state_ == \"/S\" } {\n\
$self relax_TJ\n\
$self set_TD_timer_conservative\n\
return\n\
}\n\
puts stderr \"trigger_TD: rlm state machine botched $state)\"\n\
exit -1\n\
}\n\
\n\
MMG instproc set_TJ_timer {} {\n\
global rlm_param\n\
$self instvar subscription_ layer_\n\
set n [expr $subscription_ + 1]\n\
if ![info exists layer_($n)] {\n\
return\n\
}\n\
set I [$layer_($n) timer]\n\
set d [expr $I / 2.0 + [trunc_exponential $I]]\n\
$self debug \"TJ $d\"\n\
$self set_timer TJ $d\n\
}\n\
\n\
MMG instproc set_TD_timer_conservative {} {\n\
$self instvar TD TDVAR\n\
set delay [expr $TD + 1.5 * $TDVAR]\n\
$self set_timer TD $delay\n\
}\n\
\n\
MMG instproc set_TD_timer_wait {} {\n\
$self instvar TD TDVAR\n\
$self instvar subscription_\n\
set k [expr $subscription_ / 2. + 1.5]\n\
$self set_timer TD [expr $TD + $k * $TDVAR]\n\
}\n\
\n\
MMG instproc is-recent { ts } {\n\
$self instvar TD TDVAR\n\
set ts [expr $ts + ($TD + 2 * $TDVAR)]\n\
if { $ts > [$self now] } {\n\
return 1\n\
}\n\
return 0\n\
}\n\
\n\
MMG instproc level_pending n {\n\
$self instvar pending_ts_\n\
if { [info exists pending_ts_($n)] && \\\n\
[$self is-recent $pending_ts_($n)] } {\n\
return 1\n\
}\n\
return 0\n\
}\n\
\n\
MMG instproc level_recently_joined n {\n\
$self instvar join_ts_\n\
if { [info exists join_ts_($n)] && \\\n\
[$self is-recent $join_ts_($n)] } {\n\
return 1\n\
}\n\
return 0\n\
}\n\
\n\
MMG instproc pending_inferior_jexps {} {\n\
set n 0\n\
$self instvar subscription_\n\
while { $n <= $subscription_ } { \n\
if [$self level_recently_joined $n] {\n\
return 1\n\
}\n\
incr n\n\
}\n\
$self debug \"NO-PEND-INF\"\n\
return 0\n\
}\n\
\n\
MMG instproc trigger_TJ {} {\n\
$self debug \"trigger-TJ\"\n\
$self instvar state_ ctrl_ subscription_\n\
if { ($state_ == \"/S\" && ![$self pending_inferior_jexps] && \\\n\
[$self current_layer_getting_packets])  } {\n\
$self add-layer\n\
$self check-equilibrium\n\
set msg \"add $subscription_\"\n\
$ctrl_ send $msg\n\
$self local-join\n\
}\n\
$self set_TJ_timer\n\
}\n\
\n\
MMG instproc our_level_recently_added {} {\n\
$self instvar subscription_ layer_\n\
return [$self is-recent [$layer_($subscription_) last-add]]\n\
}\n\
\n\
\n\
MMG instproc recv-ctrl msg {\n\
$self instvar join_ts_ pending_ts_ subscription_\n\
$self debug \"X-JOIN $msg\"\n\
set what [lindex $msg 0]\n\
if { $what != \"add\" } {\n\
return\n\
}\n\
set level [lindex $msg 1]\n\
set join_ts_($level) [$self now]\n\
if { $level > $subscription_ } {\n\
set pending_ts_($level) [$self now]\n\
}\n\
}\n\
\n\
MMG instproc local-join {} {\n\
$self instvar subscription_ pending_ts_ join_ts_\n\
set join_ts_($subscription_) [$self now]\n\
set pending_ts_($subscription_) [$self now]\n\
}\n\
\n\
MMG instproc debug { msg } {\n\
$self instvar debug_ subscription_ state_\n\
if {$debug_} {\n\
puts stderr \"[gettimeofday] layer $subscription_ $state_ $msg\"\n\
}\n\
}\n\
\n\
MMG instproc dumpLevel {} {\n\
}\n\
\n\
\n\
\n\
Class Layer\n\
\n\
Layer instproc init { mmg } {\n\
$self next\n\
\n\
$self instvar mmg_ TJ npkts_\n\
global rlm_param\n\
set mmg_ $mmg\n\
set TJ $rlm_param(init-tj)\n\
set npkts_ 0\n\
}\n\
\n\
Layer instproc relax {} {\n\
global rlm_param\n\
$self instvar TJ\n\
set TJ [expr $TJ * $rlm_param(beta)]\n\
if { $TJ <= $rlm_param(init-tj) } {\n\
set TJ $rlm_param(init-tj)\n\
}\n\
}\n\
\n\
Layer instproc backoff alpha {\n\
global rlm_param\n\
$self instvar TJ\n\
set TJ [expr $TJ * $alpha]\n\
if { $TJ >= $rlm_param(max) } {\n\
set TJ $rlm_param(max)\n\
}\n\
}\n\
\n\
Layer instproc peg-backoff L {\n\
$self instvar TJ\n\
set t [$L set TJ]    \n\
if { $t >= $TJ } {\n\
set TJ $t\n\
}\n\
}\n\
\n\
Layer instproc timer {} {\n\
$self instvar TJ\n\
return $TJ\n\
}\n\
\n\
Layer instproc last-add {} {\n\
$self instvar add_time_\n\
return $add_time_\n\
}\n\
\n\
Layer instproc join-group {} {\n\
$self instvar npkts_ add_time_ mmg_\n\
set npkts_ [$self npkts]\n\
set add_time_ [$mmg_ now]\n\
}\n\
\n\
Layer instproc leave-group {} {\n\
}\n\
\n\
Layer instproc getting-pkts {} {\n\
$self instvar npkts_\n\
return [expr [$self npkts] != $npkts_]\n\
}\n\
\n\
Agent/LossMonitor set npkts_ 0\n\
Agent/LossMonitor set bytes_ 0\n\
Agent/LossMonitor set nlost_ 0\n\
Agent/LossMonitor set lastPktTime_ 0\n\
\n\
Class LossTrace -superclass Agent/LossMonitor\n\
LossTrace set expected_ -1\n\
\n\
LossTrace instproc init {} {\n\
$self next\n\
$self instvar lastTime\n\
set lastTime 0\n\
}\n\
\n\
LossTrace instproc log-loss {} {\n\
$self instvar mmg_\n\
$mmg_ log-loss\n\
\n\
global lossTraceFile lossNode\n\
if [info exists lossTraceFile] {\n\
set id [[$mmg_ node] id]\n\
if { [info exists lossNode] && $lossNode != $id } {\n\
return\n\
}\n\
set f $lossTraceFile\n\
$self instvar layerNo seqno_ expected_ lastPktTime_ \\\n\
lastSeqno lastTime\n\
if [info exists lastSeqno] {\n\
set npkt [expr $expected_ - $lastSeqno]\n\
puts $f \"p $id $layerNo $lastTime $lastPktTime_ $npkt\"\n\
set lastTime $lastPktTime_\n\
}\n\
set lost [expr $seqno_ - $expected_]\n\
set t [ns-now]\n\
puts $f \"d $id $layerNo $lastPktTime_ $t $lost\"\n\
set lastSeqno $seqno_\n\
set lastTime $t\n\
}\n\
}\n\
\n\
LossTrace instproc flush {} {\n\
global lossTraceFile\n\
$self instvar lastSeqno expected_ layerNo lastTime \\\n\
lastPktTime_ mmg_ seqno_\n\
if [info exists lastSeqno] {\n\
set id [[$mmg_ node] id]\n\
set npkt [expr $seqno_ - $lastSeqno]\n\
if { $npkt != 0 } {\n\
puts $lossTraceFile \\\n\
\"p $id $layerNo $lastTime $lastPktTime_ $npkt\"\n\
}\n\
unset lastSeqno\n\
}\n\
}\n\
\n\
\n\
\n\
Class Layer/ns -superclass Layer\n\
\n\
Layer/ns instproc init {ns mmg addr layerNo} {\n\
$self next $mmg\n\
\n\
$self instvar ns_ addr_ mon_\n\
set ns_ $ns\n\
set addr_ $addr\n\
set mon_ [$ns_ create-agent [$mmg node] LossTrace 0]\n\
$mon_ set layerNo $layerNo\n\
$mon_ set mmg_ $mmg\n\
$mon_ set dst_ $addr\n\
}\n\
\n\
Layer/ns instproc join-group {} {\n\
$self instvar mon_ mmg_ addr_\n\
$mon_ clear\n\
[$mmg_ node] join-group $mon_ $addr_\n\
$self next\n\
}\n\
\n\
Layer/ns instproc leave-group {} {\n\
$self instvar mon_ mmg_ addr_\n\
[$mmg_ node] leave-group $mon_ $addr_\n\
$self next\n\
}\n\
\n\
Layer/ns instproc npkts {} {\n\
$self instvar mon_\n\
return [$mon_ set npkts_]\n\
}\n\
\n\
Layer/ns instproc nlost {} {\n\
$self instvar mon_\n\
return [$mon_ set nlost_]\n\
}\n\
\n\
Layer/ns instproc mon {} {\n\
$self instvar mon_\n\
return $mon_\n\
}\n\
\n\
Class MMG/ns -superclass MMG\n\
\n\
MMG/ns instproc init {ns localNode caddr addrs} {\n\
$self instvar ns_ node_ addrs_\n\
set ns_ $ns\n\
set node_ $localNode\n\
set addrs_ $addrs\n\
\n\
$self next [llength $addrs]\n\
\n\
$self instvar ctrl_\n\
set ctrl_ [$ns create-agent $node_ Agent/Message 0]\n\
$ctrl_ set dst_ $caddr\n\
$ctrl_ proc handle msg \"$self recv-ctrl \\$msg\"\n\
$node_ join-group $ctrl_ $caddr\n\
}\n\
\n\
MMG/ns instproc create-layer {layerNo} {\n\
$self instvar ns_ addrs_\n\
return [new Layer/ns $ns_ $self [lindex $addrs_ $layerNo] $layerNo]\n\
}\n\
\n\
MMG/ns instproc now {} {\n\
$self instvar ns_\n\
return [$ns_ now]\n\
}\n\
\n\
MMG/ns instproc set_timer {which delay} {\n\
$self instvar ns_ timers_\n\
if [info exists timers_($which)] {\n\
puts \"timer botched ($which)\"\n\
exit 1\n\
}\n\
set time [expr [$ns_ now] + $delay]\n\
set timers_($which) [$ns_ at $time \"$self trigger_timer $which\"]\n\
}\n\
\n\
MMG/ns instproc trigger_timer {which} {\n\
$self instvar timers_\n\
unset timers_($which)\n\
$self trigger_$which\n\
}\n\
\n\
MMG/ns instproc cancel_timer {which} {\n\
$self instvar ns_ timers_\n\
if [info exists timers_($which)] {\n\
$ns_ at $timers_($which)\n\
unset timers_($which)\n\
}\n\
}\n\
\n\
\n\
\n\
\n\
MMG/ns instproc node {} {\n\
$self instvar node_\n\
return $node_\n\
}\n\
\n\
MMG/ns instproc debug { msg } {\n\
$self instvar debug_\n\
if {!$debug_} { return }\n\
\n\
$self instvar subscription_ state_ node_\n\
set time [format %.05f [ns-now]]\n\
puts stderr \"$time node [$node_ id] layer $subscription_ $state_ $msg\"\n\
}\n\
\n\
MMG/ns instproc trace { trace } {\n\
$self instvar layers_\n\
foreach s $layers_ {\n\
[$s mon] trace $trace\n\
}\n\
}\n\
\n\
\n\
MMG/ns instproc total_bytes_delivered {} {\n\
$self instvar layers_\n\
set v 0\n\
foreach s $layers_ {\n\
incr v [[$s mon] set bytes]\n\
}\n\
return $v\n\
}\n\
\n\
Class SessionSim -superclass Simulator\n\
SessionSim set MixMode_ 0\n\
SessionSim set rc_ 0\n\
\n\
SessionSim instproc create-session { srcNode srcAgent } {\n\
$self instvar session_\n\
\n\
set nid [$srcNode id]\n\
set dst [$srcAgent set dst_addr_]\n\
set session_($nid:$dst:$nid) [new SessionHelper]\n\
$session_($nid:$dst:$nid) set-node $nid\n\
if {[SessionSim set rc_]} {\n\
$session_($nid:$dst:$nid) set rc_ 1\n\
}\n\
\n\
set trace [$self get-nam-traceall]\n\
if {$trace != \"\"} {\n\
set p [$self create-trace SessEnque $trace $nid $dst \"nam\"]\n\
$srcAgent target $p\n\
$p target $session_($nid:$dst:$nid)\n\
} else {\n\
$srcAgent target $session_($nid:$dst:$nid)\n\
}\n\
\n\
return $session_($nid:$dst:$nid)\n\
}\n\
\n\
SessionSim instproc update-loss-dependency { src dst owner agent group } {\n\
$self instvar session_ routingTable_ loss_\n\
\n\
set loss_rcv 1\n\
set tmp $dst\n\
while {$tmp != $owner} {\n\
set next [$routingTable_ lookup $tmp $owner]\n\
if {[info exists loss_($next:$tmp)] && $loss_($next:$tmp) != 0} {\n\
if {$loss_rcv} {\n\
set dep_loss [$session_($src:$group:$owner) update-loss-rcv $loss_($next:$tmp) $agent]\n\
} else {\n\
set dep_loss [$session_($src:$group:$owner) update-loss-loss $loss_($next:$tmp) $dep_loss]\n\
}\n\
\n\
if {$dep_loss == 0} { \n\
return \n\
}\n\
set loss_rcv 0\n\
}\n\
set tmp $next\n\
}\n\
\n\
if [info exists dep_loss] {\n\
$session_($src:$group:$owner) update-loss-top $dep_loss\n\
}\n\
}\n\
\n\
SessionSim instproc join-group { rcvAgent group } {\n\
$self instvar session_ routingTable_ delay_ bw_\n\
\n\
foreach index [array names session_] {\n\
set tri [split $index :]\n\
if {[lindex $tri 1] == $group} {\n\
set src [lindex $tri 0]\n\
set dst [[$rcvAgent set node_] id]\n\
set delay 0\n\
set accu_bw 0\n\
set ttl 0\n\
set tmp $dst\n\
while {$tmp != $src} {\n\
set next [$routingTable_ lookup $tmp $src]\n\
set delay [expr $delay + $delay_($tmp:$next)]\n\
if {$accu_bw} {\n\
set accu_bw [expr 1 / (1 / $accu_bw + 1 / $bw_($tmp:$next))]\n\
} else {\n\
set accu_bw $bw_($tmp:$next)\n\
}\n\
incr ttl\n\
set tmp $next\n\
}\n\
\n\
$self puts-nam-config \"G -t [$self now] -i $group -a $dst\"\n\
\n\
set f [$self get-nam-traceall]\n\
if {$f != \"\"} { \n\
set p [$self create-trace SessDeque $f $src $dst \"nam\"]\n\
$p target $rcvAgent\n\
$session_($index) add-dst $accu_bw $delay $ttl $dst $p\n\
$self update-loss-dependency $src $dst $src $p $group\n\
} else {\n\
$session_($index) add-dst $accu_bw $delay $ttl $dst $rcvAgent\n\
$self update-loss-dependency $src $dst $src $rcvAgent $group\n\
}\n\
}\n\
}\n\
}\n\
\n\
SessionSim instproc leave-group { rcvAgent group } {\n\
$self instvar session_\n\
\n\
foreach index [array names session_] {\n\
set tri [split $index :]\n\
if {[lindex $tri 1] == $group} {\n\
set dst [[$rcvAgent set node_] id]\n\
$self puts-nam-traceall \\\n\
\"G -t [$self now] -i $group -x $dst\"\n\
}\n\
}\n\
}\n\
\n\
SessionSim instproc insert-loss { lossmodule from to } {\n\
$self instvar loss_ bw_ Node_\n\
\n\
if {[SessionSim set MixMode_] && [$self detailed-link? [$from id] [$to id]]} {\n\
$self lossmodel $lossmodule $from $to\n\
} elseif [info exists bw_([$from id]:[$to id])] {\n\
set loss_([$from id]:[$to id]) $lossmodule\n\
}\n\
}\n\
\n\
SessionSim instproc get-delay { src dst } {\n\
$self instvar routingTable_ delay_\n\
set delay 0\n\
set tmp $src\n\
while {$tmp != $dst} {\n\
set next [$routingTable_ lookup $tmp $dst]\n\
set delay [expr $delay + $delay_($tmp:$next)]\n\
set tmp $next\n\
}\n\
return $delay\n\
}\n\
\n\
SessionSim instproc get-bw { src dst } {\n\
$self instvar routingTable_ bw_\n\
set accu_bw 0\n\
set tmp $src\n\
while {$tmp != $dst} {\n\
set next [$routingTable_ lookup $tmp $dst]\n\
if {$accu_bw} {\n\
set accu_bw [expr 1 / (1 / $accu_bw + 1 / $bw_($tmp:$next))]\n\
} else {\n\
set accu_bw $bw_($tmp:$next)\n\
}\n\
set tmp $next\n\
}\n\
return $accu_bw\n\
}\n\
\n\
SessionSim instproc node args {\n\
$self instvar sessionNode_\n\
if {[llength $args] == 0} {\n\
set node [new SessionNode]\n\
} else {\n\
set node [new SessionNode $args]\n\
}\n\
set sessionNode_([$node id]) $node\n\
$node set ns_ $self\n\
return $node\n\
}\n\
\n\
SessionSim instproc simplex-link { n1 n2 bw delay type } {\n\
$self instvar bw_ delay_ linkAttr_\n\
set sid [$n1 id]\n\
set did [$n2 id]\n\
\n\
set bw_($sid:$did) [bw_parse $bw]\n\
set delay_($sid:$did) [delay_parse $delay]\n\
\n\
set linkAttr_($sid:$did:ORIENT) \"\"\n\
set linkAttr_($sid:$did:COLOR) \"black\"\n\
}\n\
\n\
SessionSim instproc duplex-link { n1 n2 bw delay type } {\n\
$self simplex-link $n1 $n2 $bw $delay $type\n\
$self simplex-link $n2 $n1 $bw $delay $type\n\
\n\
$self session-register-nam-linkconfig [$n1 id]:[$n2 id]\n\
}\n\
\n\
SessionSim instproc simplex-link-of-interfaces { n1 n2 bw delay type } {\n\
$self simplex-link $n1 $n2 $bw $delay $type\n\
}\n\
\n\
SessionSim instproc duplex-link-of-interfaces { n1 n2 bw delay type } {\n\
$self simplex-link $n1 $n2 $bw $delay $type\n\
$self simplex-link $n2 $n1 $bw $delay $type\n\
\n\
$self session-register-nam-linkconfig [$n1 id]:[$n2 id]\n\
}\n\
\n\
SessionSim instproc detailed-node { id address } {\n\
$self instvar Node_\n\
\n\
if { [Simulator info vars EnableMcast_] != \"\" } {\n\
warn \"Flag variable Simulator::EnableMcast_ discontinued.\\n\\t\\\n\
Use multicast methods as:\\n\\t\\t\\\n\
% set ns \\[new Simulator -multicast on]\\n\\t\\t\\\n\
% \\$ns multicast\"\n\
$self multicast\n\
Simulator unset EnableMcast_\n\
}\n\
if ![info exist Node_($id)] {\n\
set node [new [Simulator set node_factory_] $address]\n\
Node set nn_ [expr [Node set nn_] - 1]\n\
$node set id_ $id\n\
set Node_($id) $node\n\
return $node\n\
} else {\n\
return $Node_($id)\n\
}\n\
}\n\
\n\
SessionSim instproc detailed-duplex-link { from to } {\n\
$self instvar bw_ delay_\n\
\n\
SessionSim set MixMode_ 1\n\
set fromNode [$self detailed-node [$from id] [$from set address_]]\n\
set toNode [$self detailed-node [$to id] [$from set address_]]\n\
\n\
$self simulator-duplex-link $fromNode $toNode $bw_([$from id]:[$to id]) $delay_([$from id]:[$to id]) DropTail\n\
}\n\
\n\
SessionSim instproc simulator-duplex-link { n1 n2 bw delay type args } {\n\
$self instvar link_\n\
set i1 [$n1 id]\n\
set i2 [$n2 id]\n\
if [info exists link_($i1:$i2)] {\n\
$self remove-nam-linkconfig $i1 $i2\n\
}\n\
\n\
eval $self simulator-simplex-link $n1 $n2 $bw $delay $type $args\n\
eval $self simulator-simplex-link $n2 $n1 $bw $delay $type $args\n\
}\n\
\n\
SessionSim instproc simulator-simplex-link { n1 n2 bw delay qtype args } {\n\
$self instvar link_ queueMap_ nullAgent_\n\
set sid [$n1 id]\n\
set did [$n2 id]\n\
\n\
if [info exists queueMap_($qtype)] {\n\
set qtype $queueMap_($qtype)\n\
}\n\
set qtypeOrig $qtype\n\
switch -exact $qtype {\n\
ErrorModule {\n\
if { [llength $args] > 0 } {\n\
set q [eval new $qtype $args]\n\
} else {\n\
set q [new $qtype Fid]\n\
}\n\
}\n\
intserv {\n\
set qtype [lindex $args 0]\n\
set q [new Queue/$qtype]\n\
}\n\
default {\n\
set q [new Queue/$qtype]\n\
}\n\
}\n\
\n\
switch -exact $qtypeOrig {\n\
RTM {\n\
set c [lindex $args 1]\n\
set link_($sid:$did) [new CBQLink       \\\n\
$n1 $n2 $bw $delay $q $c]\n\
}\n\
CBQ -\n\
CBQ/WRR {\n\
if {[llength $args] == 0} {\n\
set c [new Classifier/Hash/Fid 33]\n\
} else {\n\
set c [lindex $args 1]\n\
}\n\
set link_($sid:$did) [new CBQLink       \\\n\
$n1 $n2 $bw $delay $q $c]\n\
}\n\
intserv {\n\
set link_($sid:$did) [new IntServLink   \\\n\
$n1 $n2 $bw $delay $q	\\\n\
[concat $qtypeOrig $args]]\n\
}\n\
default {\n\
set link_($sid:$did) [new SimpleLink    \\\n\
$n1 $n2 $bw $delay $q]\n\
}\n\
}\n\
$n1 add-neighbor $n2\n\
\n\
if {[string first \"RED\" $qtype] != -1} {\n\
$q link [$link_($sid:$did) set link_]\n\
}\n\
\n\
set trace [$self get-ns-traceall]\n\
if {$trace != \"\"} {\n\
$self trace-queue $n1 $n2 $trace\n\
}\n\
set trace [$self get-nam-traceall]\n\
if {$trace != \"\"} {\n\
$self namtrace-queue $n1 $n2 $trace\n\
}\n\
\n\
$self register-nam-linkconfig $link_($sid:$did)\n\
}\n\
\n\
SessionSim instproc duplex-link-op { n1 n2 op args } {\n\
$self instvar linkAttr_ bw_\n\
\n\
set sid [$n1 id]\n\
set did [$n2 id]\n\
\n\
if ![info exists bw_($sid:$did)] {\n\
error \"Non-existent link [$n1 id]:[$n2 id]\"\n\
}\n\
\n\
switch $op {\n\
\"orient\" {\n\
set linkAttr_($sid:$did:ORIENT) $args\n\
set linkAttr_($did:$sid:ORIENT) $args\n\
}\n\
\"color\" {\n\
set ns [Simulator instance]\n\
$ns puts-nam-traceall \\\n\
[eval list \"l -t [$self now] -s $sid -d $did \\\n\
-S COLOR -c $args -o $linkAttr_($sid:$did:COLOR)\"]\n\
$ns puts-nam-traceall \\\n\
[eval list \"l -t [$self now] -s $did -d $sid \\\n\
-S COLOR -c $args -o $linkAttr_($sid:$did:COLOR)\"]\n\
eval set attr_($sid:$did:COLOR) $args\n\
eval set attr_($did:$sid:COLOR) $args\n\
}\n\
default {\n\
eval puts \"Duplex link option $args not implemented \\\n\
in SessionSim\"\n\
}\n\
} \n\
}\n\
\n\
SessionSim instproc session-register-nam-linkconfig link {\n\
$self instvar sessionLinkConfigList_ bw_ linkAttr_\n\
if [info exists sessionLinkConfigList_] {\n\
set tmp [split $link :]\n\
set i1 [lindex $tmp 0]\n\
set i2 [lindex $tmp 1]\n\
if [info exists bw_($i2:$i1)] {\n\
set pos [lsearch $sessionLinkConfigList_ $i2:$i1]\n\
if {$pos >= 0} {\n\
set a1 $linkAttr_($i2:$i1:ORIENT)\n\
set a2 $linkAttr_($link:ORIENT)\n\
if {$a1 == \"\" && $a2 != \"\"} {\n\
set sessionLinkConfigList_ [lreplace $sessionLinkConfigList_ $pos $pos]\n\
} else {\n\
return\n\
}\n\
}\n\
}\n\
\n\
set pos [lsearch $sessionLinkConfigList_ $link]\n\
if {$pos >= 0} {\n\
set sessionLinkConfigList_ \\\n\
[lreplace $sessionLinkConfigList_ $pos $pos]\n\
}\n\
}\n\
lappend sessionLinkConfigList_ $link\n\
}\n\
\n\
SessionSim instproc dump-namlinks {} {\n\
$self instvar bw_ delay_ sessionLinkConfigList_ linkAttr_\n\
\n\
set ns [Simulator instance]\n\
foreach lnk $sessionLinkConfigList_ {\n\
set tmp [split $lnk :]\n\
set i1 [lindex $tmp 0]\n\
set i2 [lindex $tmp 1]\n\
$ns puts-nam-traceall \\\n\
\"l -t * -s $i1 -d $i2 -S UP -r $bw_($lnk) -D \\\n\
$delay_($lnk) -o $linkAttr_($lnk:ORIENT)\"\n\
}\n\
}\n\
\n\
SessionSim instproc dump-namnodes {} {\n\
$self instvar sessionNode_\n\
if ![$self is-started] {\n\
return\n\
}\n\
foreach nn [array names sessionNode_] {\n\
if ![$sessionNode_($nn) is-lan?] {\n\
$sessionNode_($nn) dump-namconfig\n\
}\n\
}\n\
}     \n\
\n\
SessionSim instproc compute-routes {} {\n\
if [Simulator hier-addr?] {\n\
$self compute-hier-routes \n\
} else {\n\
$self compute-flat-routes\n\
}\n\
}\n\
\n\
SessionSim instproc compute-flat-routes {} {\n\
$self instvar bw_\n\
set r [$self get-routelogic]\n\
foreach ln [array names bw_] {\n\
set L [split $ln :]\n\
set srcID [lindex $L 0]\n\
set dstID [lindex $L 1]\n\
if {$bw_($ln) != 0} {\n\
$r insert $srcID $dstID\n\
} else {\n\
$r reset $srcID $dstID\n\
}\n\
}\n\
$r compute\n\
}\n\
\n\
SessionSim instproc compute-hier-routes {} {\n\
$self instvar bw_\n\
set r [$self get-routelogic]\n\
set level [AddrParams hlevel]\n\
$r hlevel-is $level\n\
$self hier-topo $r\n\
\n\
foreach ln [array names bw_] {\n\
set L [split $ln :]\n\
set srcID [[$self get-node-by-id [lindex $L 0]] node-addr]\n\
set dstID [[$self get-node-by-id [lindex $L 1]] node-addr]\n\
if { $bw_($ln) != 0 } {\n\
$r hier-insert $srcID $dstID\n\
} else {\n\
$r hier-reset $srcID $dstID\n\
}\n\
}       \n\
$r hier-compute\n\
}\n\
\n\
SessionSim instproc compute-algo-routes {} {\n\
set r [$self get-routelogic]\n\
\n\
\n\
$r BFS\n\
$r compute\n\
}\n\
\n\
SessionSim instproc dump-routelogic-distance {} {\n\
$self instvar routingTable_ sessionNode_ bw_\n\
if ![info exists routingTable_] {\n\
puts \"error: routing table is not computed yet!\"\n\
return 0\n\
}\n\
\n\
set n [Node set nn_]\n\
set i 0\n\
puts -nonewline \"\\t\"\n\
while { $i < $n } {\n\
if ![info exists sessionNode_($i)] {\n\
incr i\n\
continue\n\
}\n\
puts -nonewline \"$i\\t\"\n\
incr i\n\
}\n\
\n\
set i 0\n\
while { $i < $n } {\n\
if ![info exists sessionNode_($i)] {\n\
incr i\n\
continue\n\
}\n\
puts -nonewline \"\\n$i\\t\"\n\
set n1 $sessionNode_($i)\n\
set j 0\n\
while { $j < $n } {\n\
if { $i != $j } {\n\
set nh [$routingTable_ lookup $i $j]\n\
if { $nh >= 0 } {\n\
set distance 0\n\
set tmpfrom $i\n\
set tmpto $j\n\
while {$tmpfrom != $tmpto} {\n\
set tmpnext [$routingTable_ lookup $tmpfrom $tmpto]\n\
set distance [expr $distance + 1]\n\
set tmpfrom $tmpnext\n\
}\n\
puts -nonewline \"$distance\\t\"\n\
} else {\n\
puts -nonewline \"0\\t\"\n\
}\n\
} else {\n\
puts -nonewline \"0\\t\"\n\
}\n\
incr j\n\
}\n\
incr i\n\
}\n\
puts \"\"\n\
}\n\
\n\
SessionSim instproc run args {\n\
$self rtmodel-configure                 ;# in case there are any\n\
[$self get-routelogic] configure\n\
$self instvar scheduler_ sessionNode_ started_\n\
\n\
set started_ 1\n\
\n\
foreach nn [array names sessionNode_] {\n\
$sessionNode_($nn) reset\n\
}\n\
\n\
if {[SessionSim set MixMode_]} {\n\
foreach nn [array names Node_] {\n\
$Node_($nn) reset\n\
}\n\
}\n\
\n\
$self dump-namcolors\n\
$self dump-namnodes\n\
$self dump-namlinks\n\
$self dump-namagents\n\
\n\
return [$scheduler_ run]\n\
}\n\
\n\
SessionSim instproc get-mcast-tree { src grp } {\n\
$self instvar treeLinks_ session_\n\
\n\
if [info exists treeLinks_] {\n\
unset treeLinks_\n\
}\n\
\n\
set sid [$src id] \n\
\n\
foreach idx [array names session_] {\n\
set tri [split $idx :]\n\
if {[lindex $tri 0] == $sid && [lindex $tri 1] == $grp} {\n\
set mbrs [$session_($idx) list-mbr]\n\
break\n\
}\n\
}		\n\
\n\
foreach mbr $mbrs {\n\
while {![string match \"Agent*\" [$mbr info class]]} {\n\
set mbr [$mbr target]\n\
}\n\
set mid [[$mbr set node_] id]\n\
if {$sid == $mid} {\n\
continue\n\
}\n\
$self merge-path $sid $mid\n\
}\n\
\n\
foreach lnk [array names treeLinks_] {\n\
lappend res $lnk $treeLinks_($lnk)\n\
}\n\
return $res\n\
}\n\
\n\
SessionSim instproc merge-path { src mbr } {\n\
$self instvar routingTable_ treeLinks_ bw_\n\
\n\
set tmp $mbr\n\
while {$tmp != $src} {\n\
set nxt [$routingTable_ lookup $tmp $src]\n\
if ![info exists treeLinks_($nxt:$tmp)] {\n\
set treeLinks_($nxt:$tmp) $bw_($nxt:$tmp)\n\
}\n\
if [info exists treeLinks_($tmp:$nxt)] {\n\
error \"Reverse links in a SPT!\"\n\
}\n\
set tmp $nxt\n\
}\n\
}\n\
\n\
SessionSim instproc get-node-by-id id {\n\
$self instvar sessionNode_ Node_\n\
if [info exists Node_($id)] {\n\
set Node_($id)\n\
} else {\n\
set sessionNode_($id)\n\
}\n\
}\n\
\n\
SessionSim instproc get-node-id-by-addr address {\n\
$self instvar sessionNode_\n\
set n [Node set nn_]\n\
for {set q 0} {$q < $n} {incr q} {\n\
set nq $sessionNode_($q)\n\
if {[string compare [$nq node-addr] $address] == 0} {\n\
return $q\n\
}\n\
}\n\
error \"get-node-id-by-addr:Cannot find node with given address\"\n\
}\n\
\n\
Class SessionNode -superclass Node\n\
SessionNode instproc init args {\n\
$self instvar id_ np_ address_\n\
set id_ [Node getid]\n\
set np_ 0\n\
if {[llength $args] > 0} {\n\
set address_ $args\n\
} else {\n\
set address_ $id_\n\
}\n\
}\n\
\n\
SessionNode instproc id {} {\n\
$self instvar id_\n\
return $id_\n\
}\n\
\n\
SessionNode instproc reset {} {\n\
}\n\
\n\
SessionNode instproc alloc-port {} {\n\
$self instvar np_\n\
set p $np_\n\
incr np_\n\
return $p\n\
}\n\
\n\
SessionNode instproc attach agent {\n\
$self instvar id_ address_\n\
$agent set node_ $self\n\
set port [$self alloc-port]\n\
$agent set agent_addr_ [AddrParams addr2id $address_]\n\
$agent set agent_port_ $port\n\
}\n\
\n\
SessionNode instproc join-group { rcvAgent group } {\n\
set group [expr $group]\n\
if {[SessionSim set MixMode_]} {\n\
[Simulator instance] join-intermediate-session $rcvAgent $group\n\
} else {\n\
[Simulator instance] join-group $rcvAgent $group\n\
}\n\
}\n\
\n\
SessionNode instproc leave-group { rcvAgent group } {\n\
set group [expr $group]\n\
[Simulator instance] leave-group $rcvAgent $group\n\
}\n\
\n\
\n\
Agent/LossMonitor instproc show-delay { seqno delay } {\n\
$self instvar node_\n\
\n\
puts \"[$node_ id] $seqno $delay\"\n\
}\n\
\n\
\n\
\n\
SessionSim instproc RPF-link { src from to } {\n\
$self instvar routingTable_ link_\n\
if [info exists routingTable_] {\n\
set tmp $to\n\
while {$tmp != $src} {\n\
set reverse [$routingTable_ lookup $tmp $src]\n\
if [info exists link_($reverse:$tmp)] {\n\
return $link_($reverse:$tmp)\n\
}\n\
set tmp $reverse\n\
}\n\
}\n\
return \"\"\n\
}\n\
\n\
SessionSim instproc detailed-link? { from to } {\n\
$self instvar link_\n\
\n\
return [info exist link_($from:$to)]\n\
}\n\
\n\
SessionSim instproc create-intermediate-session { src group nid } {\n\
$self instvar session_\n\
\n\
set session_($src:$group:$nid) [new SessionHelper]\n\
$session_($src:$group:$nid) set-node $nid\n\
\n\
if {[SessionSim set rc_]} {\n\
$session_($src:$group:$nid) set rc_ 1\n\
}\n\
\n\
set trace [$self get-nam-traceall]\n\
if {$trace != \"\"} {\n\
set p [$self create-trace SessEnque $trace $nid $dst \"nam\"]\n\
$p target $session_($src:$group:$nid)\n\
return $p\n\
} else {\n\
return $session_($src:$group:$nid)\n\
}\n\
\n\
}\n\
\n\
SessionSim instproc join-intermediate-session { rcvAgent group } {\n\
$self instvar session_ routingTable_ delay_ bw_ link_ Node_ dlist_\n\
\n\
foreach index [array names session_] {\n\
set tri [split $index :]\n\
set src [lindex $tri 0]\n\
set grp [lindex $tri 1]\n\
set owner [lindex $tri 2]\n\
if {$grp == $group && $src == $owner} {\n\
set session_area 1\n\
set dst [[$rcvAgent set node_] id]\n\
set delay 0\n\
set accu_bw 0\n\
set ttl 0\n\
set tmp $dst\n\
while {$tmp != $src} {\n\
set next [$routingTable_ lookup $tmp $src]\n\
\n\
if {$session_area} {\n\
if [info exist link_($tmp:$next)] {\n\
\n\
set session_area 0\n\
if ![info exist session_($src:$grp:$tmp)] {\n\
set inter_session [$self create-intermediate-session $src $grp $tmp]\n\
} else {\n\
set inter_session $session_($src:$grp:$tmp)\n\
}\n\
if {![info exist dlist_($src:$grp:$tmp)] || [lsearch $dlist_($src:$grp:$tmp) $rcvAgent] < 0 } {\n\
$inter_session add-dst $accu_bw $delay $ttl $dst $rcvAgent\n\
$self update-loss-dependency $src $dst $tmp $rcvAgent $group\n\
lappend dlist_($src:$grp:$tmp) $rcvAgent\n\
}\n\
$Node_($tmp) join-group $inter_session $group\n\
\n\
} else {\n\
\n\
set delay [expr $delay + $delay_($tmp:$next)]\n\
if {$accu_bw} {\n\
set accu_bw [expr 1 / (1 / $accu_bw + 1 / $bw_($tmp:$next))]\n\
} else {\n\
set accu_bw $bw_($tmp:$next)\n\
}\n\
incr ttl\n\
}\n\
} else {\n\
if [info exist link_($tmp:$next)] {\n\
\n\
} else {\n\
\n\
set session_area 1\n\
set accu_bw $bw_($tmp:$next)\n\
set delay $delay_($tmp:$next)\n\
set ttl 1\n\
set dst $tmp\n\
set rcvAgent [$Node_($tmp) entry]\n\
}\n\
}\n\
set tmp $next\n\
}\n\
\n\
$self puts-nam-config \"G -t [$self now] -i $group -a $dst\"\n\
\n\
set f [$self get-nam-traceall]\n\
\n\
if {$session_area} {\n\
if {$f != \"\"} { \n\
set p [$self create-trace SessDeque $f $src $dst \"nam\"]\n\
$p target $rcvAgent\n\
if {![info exist dlist_($index)] || [lsearch $dlist_($index) $rcvAgent] < 0 } {\n\
$session_($index) add-dst $accu_bw $delay $ttl $dst $p\n\
$self update-loss-dependency $src $dst $src $p $group\n\
lappend dlist_($index) $rcvAgent\n\
}\n\
} else {\n\
if {![info exist dlist_($index)] || [lsearch $dlist_($index) $rcvAgent] < 0 } {\n\
$session_($index) add-dst $accu_bw $delay $ttl $dst $rcvAgent\n\
$self update-loss-dependency $src $dst $src $rcvAgent $group\n\
lappend dlist_($index) $rcvAgent\n\
}\n\
}\n\
} else {\n\
if {$f != \"\"} { \n\
set p [$self create-trace SessDeque $f $src $src \"nam\"]\n\
$p target [$Node_($tmp) entry]\n\
if {![info exist dlist_($index)] || [lsearch $dlist_($index) [$Node_($tmp) entry]] < 0 } {\n\
$session_($index) add-dst 0 0 0 $src $p\n\
$self update-loss-dependency $src $src $src $p $group\n\
lappend dlist_($index) [$Node_($tmp) entry]\n\
}\n\
} else {\n\
if {![info exist dlist_($index)] || [lsearch $dlist_($index) [$Node_($tmp) entry]] < 0 } {\n\
$session_($index) add-dst 0 0 0 $src [$Node_($tmp) entry]\n\
$self update-loss-dependency $src $src $src [$Node_($tmp) entry] $group\n\
lappend dlist_($index) [$Node_($tmp) entry]\n\
}\n\
}\n\
}\n\
}\n\
}\n\
}\n\
\n\
\n\
Node instproc shape { shape } {\n\
$self instvar attr_ \n\
set attr_(SHAPE) $shape\n\
}\n\
\n\
Node instproc get-shape {} {\n\
$self instvar attr_\n\
if [info exists attr_(SHAPE)] {\n\
return $attr_(SHAPE)\n\
} else {\n\
return \"\"\n\
}\n\
}\n\
\n\
Node instproc color { color } {\n\
$self instvar attr_ id_\n\
\n\
set ns [Simulator instance]\n\
\n\
if [$ns is-started] {\n\
\n\
$ns puts-nam-config \\\n\
[eval list \"n -t [$ns now] -s $id_ -S COLOR -c $color -o $attr_(COLOR) -i $color -I $attr_(LCOLOR)\"]\n\
set attr_(COLOR) $color\n\
set attr_(LCOLOR) $color\n\
} else {\n\
set attr_(COLOR) $color\n\
set attr_(LCOLOR) $color\n\
}\n\
}\n\
\n\
Node instproc label { str} {\n\
$self instvar attr_ id_\n\
\n\
set ns [Simulator instance]\n\
\n\
if [info exists attr_(DLABEL)] {\n\
$ns puts-nam-config \"n -t [$ns now] -s $id_ -S DLABEL -l \\\"$str\\\" -L $attr_(DLABEL)\"\n\
} else {\n\
$ns puts-nam-config \"n -t [$ns now] -s $id_ -S DLABEL -l \\\"$str\\\" -L \\\"\\\"\"\n\
}\n\
set attr_(DLABEL) \\\"$str\\\"\n\
}\n\
\n\
Node instproc label-color { str} {\n\
$self instvar attr_ id_\n\
\n\
set ns [Simulator instance]\n\
\n\
if [info exists attr_(DCOLOR)] {\n\
$ns puts-nam-config \"n -t [$ns now] -s $id_ -S DCOLOR -e \\\"$str\\\" -E $attr_(DCOLOR)\"\n\
} else {\n\
$ns puts-nam-config \"n -t [$ns now] -s $id_ -S DCOLOR -e \\\"$str\\\" -E \\\"\\\"\"\n\
}\n\
set attr_(DCOLOR) \\\"$str\\\"\n\
}\n\
\n\
Node instproc label-at { str } {\n\
$self instvar attr_ id_\n\
\n\
set ns [Simulator instance]\n\
\n\
if [info exists attr_(DIRECTION)] {\n\
$ns puts-nam-config \"n -t [$ns now] -s $id_ -S DIRECTION -p \\\"$str\\\" -P $attr_(DIRECTION)\"\n\
} else {\n\
$ns puts-nam-config \"n -t [$ns now] -s $id_ -S DIRECTION -p \\\"$str\\\" -P \\\"\\\"\"\n\
}\n\
set attr_(DIRECTION) \\\"$str\\\"\n\
}\n\
\n\
Node instproc dump-namconfig {} {\n\
$self instvar attr_ id_ address_\n\
set ns [Simulator instance]\n\
\n\
if ![info exists attr_(SHAPE)] {\n\
set attr_(SHAPE) \"circle\"\n\
} \n\
if ![info exists attr_(COLOR)] {\n\
set attr_(COLOR) \"black\"\n\
set attr_(LCOLOR) \"black\"\n\
}\n\
if ![info exists attr_(DCOLOR)] {\n\
set attr_(DCOLOR) \"black\"\n\
}\n\
$ns puts-nam-config \\\n\
[eval list \"n -t * -a $address_ -s $id_ -S UP -v $attr_(SHAPE) -c $attr_(COLOR) -i $attr_(LCOLOR)\"]\n\
}\n\
\n\
Node instproc change-color { color } {\n\
puts \"Warning: Node::change-color is obsolete. Use Node::color instead\"\n\
$self color $color\n\
}\n\
\n\
Node instproc get-attribute { name } {\n\
$self instvar attr_\n\
if [info exists attr_($name)] {\n\
return $attr_($name)\n\
} else {\n\
return \"\"\n\
}\n\
}\n\
\n\
Node instproc get-color {} {\n\
puts \"Warning: Node::get-color is obsolete. Please use Node::get-attribute\"\n\
return [$self get-attribute \"COLOR\"]\n\
}\n\
\n\
Node instproc add-mark { name color {shape \"circle\"} } {\n\
$self instvar id_ markColor_ shape_\n\
set ns [Simulator instance]\n\
\n\
$ns puts-nam-config \"m -t [$ns now] -s $id_ -n $name -c $color -h $shape\"\n\
set markColor_($name) $color\n\
set shape_($name) $shape\n\
}\n\
\n\
Node instproc delete-mark { name } {\n\
$self instvar id_ markColor_ shape_\n\
\n\
if ![info exists markColor_($name)] {\n\
return\n\
}\n\
\n\
set ns [Simulator instance]\n\
$ns puts-nam-config \\\n\
\"m -t [$ns now] -s $id_ -n $name -c $markColor_($name) -h $shape_($name) -X\"\n\
}\n\
\n\
SimpleLink instproc dump-namconfig {} {\n\
$self instvar link_ attr_ fromNode_ toNode_\n\
\n\
if ![info exists attr_(COLOR)] {\n\
set attr_(COLOR) \"black\"\n\
}\n\
\n\
if ![info exists attr_(ORIENTATION)] {\n\
set attr_(ORIENTATION) \"\"\n\
}\n\
\n\
set ns [Simulator instance]\n\
set bw [$link_ set bandwidth_]\n\
set delay [$link_ set delay_]\n\
\n\
$ns puts-nam-config \\\n\
\"l -t * -s [$fromNode_ id] -d [$toNode_ id] -S UP -r $bw -D $delay -c $attr_(COLOR) -o $attr_(ORIENTATION)\"\n\
}\n\
\n\
Link instproc dump-nam-queueconfig {} {\n\
$self instvar attr_ fromNode_ toNode_\n\
\n\
if ![info exists attr_(COLOR)] {\n\
set attr_(COLOR) \"black\"\n\
}\n\
\n\
set ns [Simulator instance]\n\
if [info exists attr_(QUEUE_POS)] {\n\
$ns puts-nam-config \"q -t * -s [$fromNode_ id] -d [$toNode_ id] -a $attr_(QUEUE_POS)\"\n\
} else {\n\
set attr_(QUEUE_POS) \"\"\n\
}\n\
}\n\
\n\
Link instproc orient { ori } {\n\
$self instvar attr_\n\
set attr_(ORIENTATION) $ori\n\
[Simulator instance] register-nam-linkconfig $self\n\
}\n\
\n\
Link instproc get-attribute { name } {\n\
$self instvar attr_\n\
if [info exists attr_($name)] {\n\
return $attr_($name)\n\
} else {\n\
return \"\"\n\
}\n\
}\n\
\n\
Link instproc queuePos { pos } {\n\
$self instvar attr_\n\
set attr_(QUEUE_POS) $pos\n\
}\n\
\n\
Link instproc color { color } {\n\
$self instvar attr_ fromNode_ toNode_ trace_\n\
\n\
set ns [Simulator instance]\n\
if [$ns is-started] {\n\
$ns puts-nam-config \\\n\
[eval list \"l -t [$ns now] -s [$fromNode_ id] -d [$toNode_ id] -S COLOR -c $color -o $attr_(COLOR)\"]\n\
set attr_(COLOR) $color\n\
} else {\n\
set attr_(COLOR) $color\n\
}\n\
}\n\
\n\
Link instproc change-color { color } {\n\
puts \"Warning: Link::change-color is obsolete. Please use Link::color.\"\n\
$self color $color\n\
}\n\
\n\
Link instproc get-color {} {\n\
puts \"Warning: Node::get-color is obsolete. Please use Node::get-attribute\"\n\
return [$self get-attribute \"COLOR\"]\n\
}\n\
\n\
Link instproc label { label } {\n\
$self instvar attr_ fromNode_ toNode_ trace_\n\
set ns [Simulator instance]\n\
if [info exists attr_(DLABEL)] {\n\
$ns puts-nam-config \\\n\
\"l -t [$ns now] -s [$fromNode_ id] -d [$toNode_ id] -S DLABEL -l \\\"$label\\\" -L $attr_(DLABEL)\"\n\
} else {\n\
$ns puts-nam-config \\\n\
\"l -t [$ns now] -s [$fromNode_ id] -d [$toNode_ id] -S DLABEL -l \\\"$label\\\" -L \\\"\\\"\"\n\
}\n\
set attr_(DLABEL) \\\"$label\\\"\n\
}\n\
\n\
Link instproc label-color { str } {\n\
$self instvar attr_ fromNode_ toNode_ trace_\n\
set ns [Simulator instance]\n\
if [info exists attr_(DCOLOR)] {\n\
$ns puts-nam-config \\\n\
\"l -t [$ns now] -s [$fromNode_ id] -d [$toNode_ id] -S DCOLOR -e \\\"$str\\\" -E $attr_(DCOLOR)\"\n\
} else {\n\
$ns puts-nam-config \\\n\
\"l -t [$ns now] -s [$fromNode_ id] -d [$toNode_ id] -S DCOLOR -e \\\"$str\\\" -E \\\"\\\"\"\n\
}\n\
set attr_(DCOLOR) \\\"$str\\\"\n\
}\n\
\n\
Link instproc label-at { str } {\n\
$self instvar attr_ fromNode_ toNode_ trace_\n\
set ns [Simulator instance]\n\
if [info exists attr_(DIRECTION)] {\n\
$ns puts-nam-config \\\n\
\"l -t [$ns now] -s [$fromNode_ id] -d [$toNode_ id] -S DIRECTION -p \\\"$str\\\" -P $attr_(DIRECTION)\"\n\
} else {\n\
$ns puts-nam-config \\\n\
\"l -t [$ns now] -s [$fromNode_ id] -d [$toNode_ id] -S DIRECTION -p \\\"$str\\\" -P \\\"\\\"\"\n\
}\n\
set attr_(DIRECTION) \\\"$str\\\"\n\
}\n\
\n\
\n\
Simulator instproc snapshot { } {\n\
set ns [Simulator instance]\n\
$ns puts-nam-config \\\n\
\"v -t [$self now] take_snapshot\"\n\
}\n\
\n\
Simulator instproc rewind-nam { } {\n\
set ns [Simulator instance]\n\
$ns puts-nam-config \\\n\
\"v  -t [$self now] playing_backward\"\n\
}\n\
\n\
Simulator instproc re-rewind-nam { } {\n\
set ns [Simulator instance]\n\
$ns puts-nam-config \\\n\
\"v  -t [$self now] playing_forward\"\n\
}\n\
\n\
Simulator instproc terminate-nam { } {\n\
set ns [Simulator instance]\n\
$ns puts-nam-config \\\n\
\"v  -t [$self now] terminating_nam\"\n\
}\n\
\n\
\n\
Simulator instproc add-agent-trace { agent name {f \"\"} } {\n\
$self instvar tracedAgents_\n\
set tracedAgents_($name) $agent\n\
\n\
set trace [$self get-nam-traceall]\n\
if {$f != \"\"} {\n\
$agent attach-trace $f\n\
} elseif {$trace != \"\"} {\n\
$agent attach-trace $trace\n\
}\n\
}\n\
\n\
Simulator instproc delete-agent-trace { agent } {\n\
$agent delete-agent-trace\n\
}\n\
\n\
Simulator instproc monitor-agent-trace { agent } {\n\
$self instvar monitoredAgents_\n\
lappend monitoredAgents_ $agent\n\
}\n\
\n\
Agent instproc attach-trace { file } {\n\
$self instvar namTrace_\n\
set namTrace_ $file \n\
$self attach $file \n\
}\n\
\n\
Simulator instproc dump-namagents {} {\n\
$self instvar tracedAgents_ monitoredAgents_\n\
\n\
if {![$self is-started]} {\n\
return\n\
}\n\
if [info exists tracedAgents_] {\n\
foreach id [array names tracedAgents_] {\n\
$tracedAgents_($id) add-agent-trace $id\n\
$tracedAgents_($id) cmd dump-namtracedvars\n\
}\n\
unset tracedAgents_\n\
}\n\
if [info exists monitoredAgents_] {\n\
foreach a $monitoredAgents_ {\n\
$a show-monitor\n\
}\n\
unset monitoredAgents_\n\
}\n\
}\n\
\n\
Simulator instproc dump-namversion { v } {\n\
$self puts-nam-config \"V -t * -v $v -a 0\"\n\
}\n\
\n\
Simulator instproc dump-namcolors {} {\n\
$self instvar color_\n\
if ![$self is-started] {\n\
return \n\
}\n\
foreach id [array names color_] {\n\
$self puts-nam-config \"c -t * -i $id -n $color_($id)\"\n\
}\n\
}\n\
\n\
Simulator instproc dump-namlans {} {\n\
if ![$self is-started] {\n\
return\n\
}\n\
$self instvar Node_\n\
foreach nn [array names Node_] {\n\
if [$Node_($nn) is-lan?] {\n\
$Node_($nn) dump-namconfig\n\
}\n\
}\n\
}\n\
\n\
Simulator instproc dump-namlinks {} {\n\
$self instvar linkConfigList_\n\
if ![$self is-started] {\n\
return\n\
}\n\
if [info exists linkConfigList_] {\n\
foreach lnk $linkConfigList_ {\n\
$lnk dump-namconfig\n\
}\n\
unset linkConfigList_\n\
}\n\
}\n\
\n\
Simulator instproc dump-namnodes {} {\n\
$self instvar Node_\n\
if ![$self is-started] {\n\
return\n\
}\n\
foreach nn [array names Node_] {\n\
if ![$Node_($nn) is-lan?] {\n\
$Node_($nn) dump-namconfig\n\
}\n\
}\n\
}\n\
\n\
Simulator instproc dump-namqueues {} {\n\
$self instvar link_\n\
if ![$self is-started] {\n\
return\n\
}\n\
foreach qn [array names link_] {\n\
$link_($qn) dump-nam-queueconfig\n\
}\n\
}\n\
\n\
Simulator instproc dump-namaddress {} {\n\
$self puts-nam-config \\\n\
\"A -t * -n [AddrParams hlevel] -p 0 -o [AddrParams set \\\n\
ALL_BITS_SET] -c [AddrParams McastShift] -a [AddrParams McastMask]\"\n\
\n\
for {set i 1} {$i <= [AddrParams hlevel]} {incr i} {\n\
$self puts-nam-config \"A -t * -h $i -m [AddrParams \\\n\
NodeMask $i] -s [AddrParams NodeShift $i]\"\n\
}\n\
}\n\
\n\
Simulator instproc init-nam {} {\n\
$self instvar annotationSeq_ \n\
\n\
set annotationSeq_ 0\n\
\n\
$self dump-namversion 1.0a5\n\
\n\
$self dump-namaddress\n\
\n\
$self dump-namcolors\n\
\n\
$self dump-namnodes\n\
\n\
$self dump-namlinks \n\
$self dump-namlans\n\
\n\
$self dump-namqueues\n\
\n\
$self dump-namagents\n\
\n\
}\n\
\n\
Simulator instproc trace-annotate { str } {\n\
$self instvar annotationSeq_\n\
$self puts-ns-traceall [format \\\n\
\"v %s %s {set sim_annotation {%s}}\" [$self now] eval $str]\n\
incr annotationSeq_\n\
$self puts-nam-config \\\n\
\"v -t [$self now] sim_annotation [$self now] $annotationSeq_ $str\"\n\
}\n\
\n\
proc trace_annotate { str } {\n\
set ns [Simulator instance]\n\
\n\
$ns trace-annotate $str\n\
}\n\
\n\
proc flash_annotate { start duration msg } {\n\
set ns [Simulator instance]\n\
$ns at $start \"trace_annotate {$msg}\"\n\
$ns at [expr $start+$duration] \"trace_annotate periodic_message\"\n\
}\n\
\n\
Simulator instproc set-animation-rate { rate } {\n\
set r [time_parse $rate]\n\
$self puts-nam-config \"v -t [$self now] set_rate [expr 10*log10($r)] 1\"\n\
}\n\
\n\
Agent/DSDV set sport_        0\n\
Agent/DSDV set dport_        0\n\
Agent/DSDV set wst0_         6        ;# As specified by Pravin\n\
Agent/DSDV set perup_       15        ;# As given in the paper (update period)\n\
Agent/DSDV set use_mac_      0        ;# Performance suffers with this on\n\
Agent/DSDV set be_random_    1        ;# Flavor the performance numbers :)\n\
Agent/DSDV set alpha_        0.875    ;# 7/8, as in RIP(?)\n\
Agent/DSDV set min_update_periods_ 3  ;# Missing perups before linkbreak\n\
Agent/DSDV set verbose_      0        ;# \n\
Agent/DSDV set trace_wst_    0        ;# \n\
\n\
set opt(ragent)		Agent/DSDV\n\
set opt(pos)		NONE			;# Box or NONE\n\
\n\
if { $opt(pos) == \"Box\" } {\n\
puts \"*** DSDV using Box configuration...\"\n\
}\n\
\n\
Agent instproc init args {\n\
eval $self next $args\n\
}       \n\
\n\
Agent/DSDV instproc init args {\n\
eval $self next $args\n\
}       \n\
\n\
\n\
\n\
proc create-dsdv-routing-agent { node id } {\n\
global ns_ ragent_ tracefd opt\n\
\n\
set ragent_($id) [new $opt(ragent)]\n\
set ragent $ragent_($id)\n\
\n\
set addr [$node node-addr]\n\
\n\
$ragent addr $addr\n\
$ragent node $node\n\
if [Simulator set mobile_ip_] {\n\
$ragent port-dmux [$node set dmux_]\n\
}\n\
$node addr $addr\n\
$node set ragent_ $ragent\n\
\n\
$node attach $ragent [Node set rtagent_port_]\n\
\n\
\n\
$ns_ at 0.0 \"$ragent_($id) start-dsdv\"	;# start updates\n\
\n\
set drpT [cmu-trace Drop \"RTR\" $node]\n\
$ragent drop-target $drpT\n\
\n\
set T [new Trace/Generic]\n\
$T target [$ns_ set nullAgent_]\n\
$T attach $tracefd\n\
$T set src_ $id\n\
$ragent tracetarget $T\n\
}\n\
\n\
\n\
proc dsdv-create-mobile-node { id args } {\n\
global ns ns_ chan prop topo tracefd opt node_\n\
global chan prop tracefd topo opt\n\
\n\
set ns_ [Simulator instance]\n\
if [Simulator hier-addr?] {\n\
if [Simulator set mobile_ip_] {\n\
set node_($id) [new MobileNode/MIPMH $args]\n\
} else {\n\
set node_($id) [new Node/MobileNode/BaseStationNode $args]\n\
}\n\
} else {\n\
set node_($id) [new Node/MobileNode]\n\
}\n\
set node $node_($id)\n\
$node random-motion 0		;# disable random motion\n\
$node topography $topo\n\
\n\
if [info exists opt(energy)] {\n\
$node addenergymodel [new $opt(energy) $node 1000 0.5 0.2]\n\
}\n\
\n\
set T [new Trace/Generic]\n\
$T target [$ns_ set nullAgent_]\n\
$T attach $tracefd\n\
$T set src_ $id\n\
$node log-target $T\n\
\n\
if ![info exist opt(err)] {\n\
set opt(err) \"\"\n\
}\n\
if ![info exist opt(fec)] {\n\
set opt(fec) \"\"\n\
}\n\
\n\
$node add-interface $chan $prop $opt(ll) $opt(mac)	\\\n\
$opt(ifq) $opt(ifqlen) $opt(netif) $opt(ant) $opt(err) $opt(fec)\n\
\n\
create-$opt(rp)-routing-agent $node $id\n\
\n\
if { $opt(pos) == \"Box\" } {\n\
set spacing 200\n\
set maxrow 7\n\
set col [expr ($id - 1) % $maxrow]\n\
set row [expr ($id - 1) / $maxrow]\n\
$node set X_ [expr $col * $spacing]\n\
$node set Y_ [expr $row * $spacing]\n\
$node set Z_ 0.0\n\
$node set speed_ 0.0\n\
\n\
$ns_ at 0.0 \"$node_($id) start\"\n\
}\n\
return $node\n\
}\n\
\n\
\n\
\n\
\n\
\n\
\n\
\n\
\n\
\n\
\n\
set opt(rt_port) 255\n\
set opt(cc)      \"off\"            ;# have god check the caches for bad links?\n\
\n\
\n\
Class CacheTimer -superclass Timer\n\
CacheTimer instproc timeout {} {\n\
global opt node_;\n\
$self instvar agent;\n\
$agent check-cache\n\
$self sched 1.0\n\
}\n\
\n\
proc checkcache {a} {\n\
global cachetimer ns\n\
\n\
set cachetimer [new CacheTimer]\n\
$cachetimer set agent $a\n\
$cachetimer sched 1.0\n\
}\n\
\n\
Class SRNode -superclass Node/MobileNode\n\
\n\
SRNode instproc init {args} {\n\
global ns ns_ opt tracefd RouterTrace\n\
$self instvar dsr_agent_ dmux_ entry_point_ address_\n\
set ns_ [Simulator instance]\n\
\n\
eval $self next $args	;# parent class constructor\n\
if {$dmux_ == \"\" } {\n\
set dmux_ [new Classifier/Port]\n\
$dmux_ set mask_ [AddrParams PortMask]\n\
$dmux_ set shift_ [AddrParams PortShift]\n\
}\n\
set dsr_agent_ [new Agent/DSRAgent]\n\
\n\
$dsr_agent_ addr $address_\n\
$dsr_agent_ node $self\n\
if [Simulator set mobile_ip_] {\n\
$dsr_agent_ port-dmux [$self set dmux_]\n\
}\n\
$self addr $address_\n\
\n\
if { $RouterTrace == \"ON\" } {\n\
set rcvT [cmu-trace Recv \"RTR\" $self]\n\
$rcvT target $dsr_agent_\n\
set entry_point_ $rcvT	\n\
} else {\n\
set entry_point_ $dsr_agent_\n\
}\n\
\n\
set drpT [cmu-trace Drop \"RTR\" $self]\n\
$dsr_agent_ drop-target $drpT\n\
\n\
\n\
set T [new Trace/Generic]\n\
$T target [$ns_ set nullAgent_]\n\
$T attach $tracefd\n\
$T set src_ [$self id]\n\
$dsr_agent_ log-target $T\n\
\n\
$dsr_agent_ target $dmux_\n\
\n\
set nullAgent_ [$ns_ set nullAgent_]\n\
$dmux_ install $opt(rt_port) $nullAgent_\n\
\n\
$self instvar classifier_\n\
set classifier_ \"srnode made illegal use of classifier_\"\n\
\n\
}\n\
\n\
SRNode instproc start-dsr {} {\n\
$self instvar dsr_agent_\n\
global opt;\n\
\n\
$dsr_agent_ startdsr\n\
if {$opt(cc) == \"on\"} {checkcache $dsr_agent_}\n\
}\n\
\n\
SRNode instproc entry {} {\n\
$self instvar entry_point_\n\
return $entry_point_\n\
}\n\
\n\
\n\
\n\
SRNode instproc add-interface {args} {\n\
global ns ns_ opt RouterTrace\n\
\n\
eval $self next $args\n\
\n\
$self instvar dsr_agent_ ll_ mac_ ifq_\n\
\n\
$dsr_agent_ mac-addr [$mac_(0) id]\n\
\n\
if { $RouterTrace == \"ON\" } {\n\
set sndT [cmu-trace Send \"RTR\" $self]\n\
$sndT target $ll_(0)\n\
$dsr_agent_ add-ll $sndT $ifq_(0)\n\
} else {\n\
$dsr_agent_ add-ll $ll_(0) $ifq_(0)\n\
}\n\
\n\
$dsr_agent_ install-tap $mac_(0)\n\
\n\
}\n\
\n\
SRNode instproc reset args {\n\
$self instvar dsr_agent_\n\
eval $self next $args\n\
\n\
$dsr_agent_ reset\n\
}\n\
\n\
\n\
proc dsr-create-mobile-node { id args } {\n\
global ns_ chan prop topo tracefd opt node_\n\
set ns_ [Simulator instance] \n\
if [Simulator hier-addr?] {\n\
if [Simulator set mobile_ip_] {\n\
set node_($id) [new SRNode/MIPMH $args]\n\
} else {\n\
set node_($id) [new SRNode $args]\n\
}\n\
} else {\n\
set node_($id) [new SRNode]\n\
}\n\
set node $node_($id)\n\
$node random-motion 0		;# disable random motion\n\
$node topography $topo\n\
\n\
if [info exists opt(energy)] {\n\
$node addenergymodel [new $opt(energy) $node 1000 0.5 0.2]\n\
}\n\
\n\
if ![info exist opt(err)] {\n\
set opt(err) \"\"\n\
}\n\
if ![info exist opt(fec)] {\n\
set opt(fec) \"\"\n\
}\n\
\n\
$node add-interface $chan $prop $opt(ll) $opt(mac)	\\\n\
$opt(ifq) $opt(ifqlen) $opt(netif) $opt(ant) $opt(err) $opt(fec)\n\
\n\
set T [new Trace/Generic]\n\
$T target [$ns_ set nullAgent_]\n\
$T attach $tracefd\n\
$T set src_ $id\n\
$node log-target $T\n\
\n\
$ns_ at 0.0 \"$node start-dsr\"\n\
return $node\n\
}\n\
\n\
proc create-base-station-node {address } {\n\
global topo tracefd opt node node_ ns_\n\
set ns_ [Simulator instance]\n\
if [Simulator set mobile_ip_] {\n\
Simulator set node_factory_ MobileNode/MIPBS\n\
} else {\n\
Simulator set node_factory_ Node/MobileNode/BaseStationNode\n\
}\n\
set node [$ns_ node $address]\n\
set id [$node id]\n\
\n\
$node random-motion 0		;# disable random motion\n\
$node topography $topo\n\
set T [new Trace/Generic]\n\
$T target [$ns_ set nullAgent_]\n\
$T attach $tracefd\n\
$T set src_ $id\n\
$node log-target $T\n\
$node base-station [AddrParams addr2id [$node node-addr]]\n\
\n\
create-$opt(rp)-bs-node $node $id\n\
\n\
Simulator set node_factory_ Node    ;# default value\n\
return $node\n\
}\n\
\n\
\n\
proc create-dsdv-bs-node {node id} {\n\
global ns_ chan prop opt node_\n\
$node instvar regagent_ ragent_\n\
\n\
$node add-interface $chan $prop $opt(ll) $opt(mac)	\\\n\
$opt(ifq) $opt(ifqlen) $opt(netif) \\\n\
$opt(ant)\n\
\n\
create-$opt(rp)-routing-agent $node $id\n\
\n\
if [info exists regagent_] {\n\
$regagent_ ragent $ragent_\n\
}\n\
if { $opt(pos) == \"Box\" } {\n\
set spacing 200\n\
set maxrow 7\n\
set col [expr ($id - 1) % $maxrow]\n\
set row [expr ($id - 1) / $maxrow]\n\
$node set X_ [expr $col * $spacing]\n\
$node set Y_ [expr $row * $spacing]\n\
$node set Z_ 0.0\n\
$node set speed_ 0.0\n\
\n\
$ns_ at 0.0 \"$node_($id) start\"\n\
}\n\
}\n\
\n\
proc create-dsr-bs-node {node id} {\n\
global ns_ chan prop opt\n\
$node instvar regagent_ ragent_\n\
\n\
$node add-interface $chan $prop $opt(ll) $opt(mac)	\\\n\
$opt(ifq) $opt(ifqlen) $opt(netif) \\\n\
$opt(ant)\n\
\n\
create-$opt(rp)-routing-agent $node $id\n\
$node create-xtra-interface \n\
\n\
if [info exists regagent_] {\n\
$regagent_ ragent $ragent_\n\
}\n\
\n\
$ns_ at 0.0 \"$node start-dsr\"\n\
}\n\
\n\
\n\
proc create-dsr-routing-agent { node id } {\n\
global ns_ ragent_ tracefd opt\n\
\n\
set ragent_($id) [new Agent/DSRAgent/BS_DSRAgent]\n\
set ragent $ragent_($id)\n\
\n\
set address [$node node-addr]\n\
$ragent addr $address\n\
$ragent node $node\n\
if [Simulator set mobile_ip_] {\n\
$ragent port-dmux [$node set dmux_]\n\
}\n\
\n\
$node addr $address\n\
$node set ragent_ $ragent\n\
\n\
set dmux [$node set dmux_]\n\
if {$dmux == \"\" } {\n\
set dmux [new Classifier/Hash/Dest 32]\n\
$dmux set mask_ [AddrParams PortMask]\n\
$dmux set shift_ [AddrParams PortShift]\n\
$node add-route $address $ragent\n\
$node set dmux_ $dmux\n\
}\n\
set level [AddrParams hlevel]\n\
\n\
if { [Simulator set RouterTrace_] == \"ON\" } {\n\
set rcvT [cmu-trace Recv \"RTR\" $node]\n\
$rcvT target $ragent\n\
for {set i 1} {$i <= $level} {incr i} {\n\
[$node set classifiers_($i)] defaulttarget $rcvT\n\
[$node set classifiers_($i)] bcast-receiver $rcvT\n\
}\n\
} else {\n\
\n\
for {set i 1} {$i <= $level} {incr i} {\n\
[$node set classifiers_($i)] defaulttarget $ragent\n\
[$node set classifiers_($i)] bcast-receiver $ragent\n\
}\n\
}\n\
set drpT [cmu-trace Drop \"RTR\" $node]\n\
$ragent drop-target $drpT\n\
\n\
\n\
$ragent target $dmux\n\
\n\
$dmux install $opt(rt_port) $ragent\n\
}\n\
\n\
\n\
Node/MobileNode/BaseStationNode instproc create-xtra-interface { } {\n\
global ns_ opt \n\
$self instvar ragent_ ll_ mac_ ifq_\n\
\n\
$ragent_ mac-addr [$mac_(0) id]\n\
\n\
if { [Simulator set RouterTrace_] == \"ON\" } {\n\
set sndT [cmu-trace Send \"RTR\" $self]\n\
$sndT target $ll_(0)\n\
$ragent_ add-ll $sndT $ifq_(0)\n\
} else {\n\
$ragent_ add-ll $ll_(0) $ifq_(0)\n\
}\n\
\n\
$ragent_ install-tap $mac_(0)\n\
\n\
}\n\
\n\
Node/MobileNode/BaseStationNode instproc start-dsr {} {\n\
$self instvar ragent_\n\
global opt;\n\
\n\
$ragent_ startdsr\n\
if {$opt(cc) == \"on\"} {checkcache $dsr_agent_}\n\
}\n\
\n\
Node/MobileNode/BaseStationNode instproc reset args {\n\
$self instvar ragent_\n\
eval $self next $args\n\
\n\
$ragent_ reset\n\
}\n\
\n\
\n\
proc create-god { nodes } {\n\
set god [God info instances]\n\
if { $god == \"\" } {\n\
set god [new God]\n\
}\n\
$god num_nodes $nodes\n\
return $god\n\
}\n\
\n\
God proc instance {} {\n\
set god [God info instances]\n\
if { $god != \"\" } {\n\
return $god\n\
}  \n\
error \"Cannot find instance of god\"\n\
}      \n\
\n\
proc cmu-trace { ttype atype node } {\n\
global ns_ tracefd\n\
\n\
if { $tracefd == \"\" } {\n\
return \"\"\n\
}\n\
set T [new CMUTrace/$ttype $atype]\n\
$T target [$ns_ set nullAgent_]\n\
$T attach $tracefd\n\
$T set src_ [$node id]\n\
\n\
$T node $node\n\
\n\
return $T\n\
}\n\
\n\
proc log-movement {} {\n\
global logtimer ns_ ns\n\
\n\
set ns $ns_\n\
source ../mobility/timer.tcl\n\
Class LogTimer -superclass Timer\n\
LogTimer instproc timeout {} {\n\
global opt node_;\n\
for {set i 0} {$i < $opt(nn)} {incr i} {\n\
$node_($i) log-movement\n\
}\n\
$self sched 0.1\n\
}\n\
\n\
set logtimer [new LogTimer]\n\
$logtimer sched 0.1\n\
}    \n\
\n\
proc set-wireless-traces { args } {\n\
set len [llength $args]\n\
if { $len <= 0 || [expr $len%2] } {\n\
error \"Incorrect number of parameters\"\n\
}\n\
for {set n 0} {$n < $len} {incr n 2} {\n\
if {[string compare [lindex $args $n] \"-AgentTrace\"] == 0 } {\n\
Simulator set AgentTrace_ [lindex $args [expr $n+1]]\n\
} elseif {[string compare [lindex $args $n] \"-RouterTrace\"] == 0 } {\n\
Simulator set RouterTrace_ [lindex $args [expr $n+1]]\n\
} elseif {[string compare [lindex $args $n] \"-MacTrace\"] == 0 } {\n\
Simulator set MacTrace_ [lindex $args [expr $n+1]]\n\
} else {\n\
error \"Unknown wireless trace type: [lindex $args $n]\"\n\
}\n\
}\n\
}  \n\
\n\
\n\
\n\
\n\
\n\
\n\
\n\
\n\
\n\
\n\
Class PLM\n\
\n\
PLM instproc init {levels chk_estimate n_id} {\n\
$self next\n\
$self instvar PP_estimate wait_loss time_loss \n\
$self instvar start_loss time_estimate check_estimate node_id\n\
global rates\n\
set PP_estimate {} \n\
set start_loss -1\n\
set wait_loss 0\n\
set time_loss 0\n\
set time_estimate 0\n\
set check_estimate $chk_estimate\n\
set node_id $n_id\n\
\n\
$self instvar debug_ env_ maxlevel_\n\
\n\
set debug_ 0\n\
set env_ [lindex [split [$self info class] /] 1]\n\
set maxlevel_ $levels\n\
\n\
global plm_debug_flag\n\
if [info exists plm_debug_flag] {\n\
set debug_ $plm_debug_flag\n\
}\n\
\n\
$self instvar subscription_\n\
\n\
$self instvar layer_ layers_\n\
set i 1\n\
while { $i <= $maxlevel_ } {\n\
set layer_($i) [$self create-layer [expr $i - 1]]\n\
lappend layers_ $layer_($i)\n\
incr i\n\
}\n\
\n\
set subscription_ 0\n\
$self add-layer\n\
}\n\
\n\
\n\
\n\
PLM instproc make_estimate {PP_value} {\n\
$self instvar PP_estimate PP_estimate_value ns_ time_estimate check_estimate debug_\n\
global PP_estimation_length\n\
\n\
lappend PP_estimate $PP_value\n\
\n\
$self stability-drop $PP_value\n\
\n\
set ns_time [$ns_ now]\n\
if {$time_estimate==0} {\n\
set time_estimate [expr $ns_time + $check_estimate]\n\
}\n\
if {$debug_>=3} {\n\
trace_annotate \"[$self node]: check: $check_estimate $PP_estimate , nb: [llength $PP_estimate]\"\n\
}\n\
\n\
if {($time_estimate<=$ns_time) && ([llength $PP_estimate] >= $PP_estimation_length)} {\n\
set PP_estimate_value [lindex [lsort -real $PP_estimate] 0]\n\
if {$debug_>=3} {\n\
trace_annotate \"[$self node]: check: $check_estimate PP estim: $PP_estimate, value: $PP_estimate_value\"\n\
}\n\
if {$debug_>=2} {\n\
trace_annotate [expr round($PP_estimate_value)]\n\
}\n\
set PP_estimate {}\n\
set time_estimate [expr $ns_time + $check_estimate]\n\
$self choose_layer $PP_estimate_value\n\
\n\
}\n\
}\n\
\n\
\n\
PLM instproc stability-drop {PP_value} {\n\
$self instvar subscription_ start_loss time_estimate PP_estimate\n\
$self instvar check_estimate ns_\n\
global rates_cum\n\
\n\
set ns_time [$ns_ now]\n\
for {set i 0} {[lindex $rates_cum $i] < [expr round($PP_value)]} {incr i} {\n\
if {$i > [llength $rates_cum]} {break}\n\
}\n\
\n\
if {$subscription_ > $i} {\n\
for {set j $subscription_} {$i < $j} {incr j -1} {\n\
set start_loss -1\n\
$self drop-layer	    \n\
}\n\
set PP_estimate {}\n\
set time_estimate [expr $ns_time + $check_estimate]\n\
}\n\
}\n\
\n\
proc calc_cum {rates} {\n\
set temp 0\n\
set rates_cum {}\n\
for {set i 0} {$i<[llength $rates]} {incr i} {\n\
set temp [expr $temp + [lindex $rates $i]]\n\
lappend rates_cum $temp\n\
}\n\
return $rates_cum\n\
}\n\
\n\
PLM instproc choose_layer {PP_estimate_value} {\n\
$self instvar subscription_ start_loss\n\
global rates_cum\n\
\n\
set start_loss -1\n\
\n\
for {set i 0} {[lindex $rates_cum $i] < [expr round($PP_estimate_value)]} {incr i} {\n\
if {$i > [llength $rates_cum]} {break}\n\
}\n\
\n\
if {$subscription_ < $i} {\n\
for {set j $subscription_} {$j < $i} {incr j} {\n\
$self add-layer	    \n\
}	    \n\
} elseif {$subscription_ > $i} {\n\
for {set j $subscription_} {$i < $j} {incr j -1} {\n\
$self drop-layer	    \n\
}\n\
} elseif {$subscription_ == $i} {\n\
return\n\
}\n\
}\n\
\n\
\n\
PLM instproc log-loss {} {\n\
$self instvar subscription_ h_npkts h_nlost start_loss debug_\n\
$self instvar time_loss ns_ wait_loss\n\
\n\
$self debug \"LOSS [$self plm_loss]\" \n\
\n\
if {$debug_>=2} {\n\
trace_annotate \"$self pkt_lost\"\n\
}\n\
set ns_time [$ns_ now]\n\
\n\
\n\
if {$time_loss <= $ns_time} {\n\
if {$debug_>=2} {\n\
trace_annotate \"not enough losses during 1s: reinitialize\"\n\
}\n\
set start_loss -1\n\
}\n\
\n\
if {($start_loss == -1) || ($wait_loss >= $ns_time)} {\n\
if {$debug_>=2} {\n\
trace_annotate \"$start_loss [expr $wait_loss >= $ns_time] reinitialize\"\n\
}\n\
set h_npkts [$self plm_pkts]\n\
set h_nlost [$self plm_loss]\n\
set start_loss 1\n\
set time_loss [expr [$ns_ now] + 5]\n\
if {$debug_>=2} {\n\
trace_annotate \"time_loss : $time_loss\"\n\
}\n\
}\n\
\n\
if {([$self exceed_loss_thresh]) && ($wait_loss <= $ns_time)} {\n\
$self drop-layer\n\
set start_loss -1\n\
set wait_loss [expr $ns_time + 0.5]\n\
if {$debug_>=2} {\n\
trace_annotate \"drop layer wait_loss: $wait_loss\"\n\
}\n\
}\n\
}\n\
\n\
PLM instproc exceed_loss_thresh {} {\n\
$self instvar h_npkts h_nlost debug_\n\
set npkts [expr [$self plm_pkts] - $h_npkts]\n\
if { $npkts >= 10 } {\n\
set nloss [expr [$self plm_loss] - $h_nlost]\n\
set loss [expr double($nloss) / ($nloss + $npkts)]\n\
$self debug \"H-THRESH $nloss $npkts $loss\"\n\
if { $loss > 0.10 } {\n\
return 1\n\
}\n\
}\n\
return 0\n\
}\n\
\n\
\n\
PLM instproc drop-layer {} {\n\
$self instvar subscription_ layer_ node_id debug_\n\
set n $subscription_\n\
\n\
if { $n > 0 } {\n\
$self debug \"DRP-LAYER $n\"\n\
$layer_($n) leave-group \n\
incr n -1\n\
set subscription_ $n\n\
if {$debug_>=2} {\n\
trace_annotate \" [$self set node_id] : change layer $subscription_ \"\n\
}\n\
}\n\
\n\
if { $subscription_ == 0 } {\n\
set ns [Simulator instance]\n\
set rejoin_timer 30\n\
$ns at [expr [$ns now] + $rejoin_timer] \"$self add-layer\"\n\
if {$debug_>=2} {\n\
trace_annotate \" Try to re-join the session after dropping all the layers \"\n\
}\n\
}\n\
}\n\
\n\
PLM instproc add-layer {} {\n\
$self instvar maxlevel_ subscription_ layer_ node_id debug_\n\
set n $subscription_\n\
if { $n < $maxlevel_ } {\n\
$self debug \"ADD-LAYER\"\n\
incr n\n\
set subscription_ $n\n\
$layer_($n) join-group\n\
if {$debug_>=2} {\n\
trace_annotate \" [$self set node_id] : change layer $subscription_ \"\n\
}\n\
}\n\
}\n\
\n\
PLM instproc plm_loss {} {\n\
$self instvar layers_\n\
set loss 0\n\
foreach l $layers_ {\n\
incr loss [$l nlost]\n\
}\n\
return $loss\n\
}\n\
\n\
PLM instproc plm_pkts {} {\n\
$self instvar layers_\n\
set npkts 0\n\
foreach l $layers_ {\n\
incr npkts [$l npkts]\n\
}\n\
return $npkts\n\
}\n\
\n\
PLM instproc debug { msg } {\n\
$self instvar debug_ subscription_ ns_\n\
\n\
if {$debug_ <1} { return }\n\
set time [format %.05f [$ns_ now]]\n\
puts stderr \"PLM: $time  layer $subscription_ $msg\"\n\
}\n\
\n\
Class PLMLayer\n\
\n\
PLMLayer instproc init { plm } {\n\
$self next\n\
\n\
$self instvar plm_ npkts_\n\
set plm_ $plm\n\
set npkts_ 0\n\
}\n\
\n\
PLMLayer instproc join-group {} {\n\
$self instvar npkts_ add_time_ plm_\n\
set npkts_ [$self npkts]\n\
set add_time_ [$plm_ now]\n\
}\n\
\n\
PLMLayer instproc leave-group {} {\n\
}\n\
\n\
PLMLayer instproc getting-pkts {} {\n\
$self instvar npkts_\n\
return [expr [$self npkts] != $npkts_]\n\
}\n\
\n\
\n\
Application/Traffic/CBR_PP instproc set args {\n\
$self instvar packet_size_ rate_ \n\
if { [lindex $args 0] == \"interval_\" } {\n\
puts \"Cannot use CBR_PP with interval_, specify rate_ instead\"\n\
}\n\
eval $self next $args\n\
}\n\
\n\
Agent/LossMonitor/PLM instproc log-PP {} {\n\
}\n\
\n\
Class PLMLossTrace -superclass Agent/LossMonitor/PLM\n\
PLMLossTrace set expected_ -1\n\
\n\
PLMLossTrace instproc init {} {\n\
$self next\n\
$self instvar lastTime measure debug_\n\
set lastTime 0\n\
set measure -1\n\
global plm_debug_flag\n\
if [info exists plm_debug_flag] {\n\
set debug_ $plm_debug_flag\n\
}\n\
}\n\
\n\
PLMLossTrace instproc log-loss {} {\n\
$self instvar plm_\n\
$plm_ log-loss\n\
}\n\
\n\
\n\
PLMLossTrace instproc log-PP {} {\n\
$self instvar plm_ PP_first measure next_pkt debug_\n\
global PP_burst_length packetSize\n\
\n\
if {[$self set flag_PP_] == 128} {\n\
set measure 1\n\
set next_pkt [expr [$self set seqno_] + 1]\n\
set PP_first [$self set packet_time_PP_]\n\
if {$debug_>=2} {\n\
trace_annotate \"[$plm_ node]:  first PP [$self set seqno_], next: $next_pkt\"\n\
} 	\n\
} elseif {$measure>-1} {\n\
if {[$self set seqno_]==$next_pkt} {\n\
set measure [expr $measure + 1]\n\
set next_pkt [expr [$self set seqno_] + 1]	\n\
if {$debug_>=2} {\n\
trace_annotate \"[$plm_ node]:   pending measurement : $measure, next $next_pkt\"\n\
}\n\
if {$measure==$PP_burst_length} {\n\
set PP_value [expr $packetSize*8.*($PP_burst_length - 1)/([$self set packet_time_PP_] - $PP_first)]\n\
set measure -1\n\
if {$debug_>=2} {\n\
trace_annotate \"[$plm_ node]:  measure : $PP_value\"\n\
}\n\
$plm_ make_estimate $PP_value\n\
} \n\
} else {\n\
if {$debug_>=2} {\n\
trace_annotate \"[$plm_ node]:  out of sequence : [$self set seqno_], next: $next_pkt\"\n\
}\n\
set measure -1\n\
}\n\
}\n\
}\n\
\n\
\n\
\n\
\n\
Class PLMLayer/ns -superclass PLMLayer\n\
\n\
PLMLayer/ns instproc init {ns plm addr layerNo} {\n\
$self next $plm\n\
\n\
$self instvar ns_ addr_ mon_\n\
set ns_ $ns\n\
set addr_ $addr\n\
set mon_ [$ns_ PLMcreate-agent [$plm node] PLMLossTrace 0]\n\
$mon_ set layerNo $layerNo\n\
$mon_ set plm_ $plm\n\
$mon_ set dst_addr_ $addr\n\
$mon_ set dst_port_ 0\n\
}\n\
\n\
PLMLayer/ns instproc join-group {} {\n\
$self instvar mon_ plm_ addr_\n\
$mon_ clear\n\
[$plm_ node] join-group $mon_ $addr_\n\
$self next\n\
}\n\
\n\
PLMLayer/ns instproc leave-group {} {\n\
$self instvar mon_ plm_ addr_\n\
[$plm_ node] leave-group $mon_ $addr_\n\
$self next\n\
}\n\
\n\
PLMLayer/ns instproc npkts {} {\n\
$self instvar mon_\n\
return [$mon_ set npkts_]\n\
}\n\
\n\
PLMLayer/ns instproc nlost {} {\n\
$self instvar mon_\n\
return [$mon_ set nlost_]\n\
}\n\
\n\
PLMLayer/ns instproc mon {} {\n\
$self instvar mon_\n\
return $mon_\n\
}\n\
\n\
Class PLM/ns -superclass PLM\n\
\n\
PLM/ns instproc init {ns localNode addrs check_estimate nn} {\n\
$self instvar ns_ node_ addrs_\n\
set ns_ $ns\n\
set node_ $localNode\n\
set addrs_ $addrs\n\
\n\
$self next [llength $addrs] $check_estimate $nn\n\
}\n\
\n\
PLM/ns instproc create-layer {layerNo} {\n\
$self instvar ns_ addrs_\n\
return [new PLMLayer/ns $ns_ $self [lindex $addrs_ $layerNo] $layerNo]\n\
}\n\
\n\
PLM/ns instproc now {} {\n\
$self instvar ns_\n\
return [$ns_ now]\n\
}\n\
\n\
\n\
\n\
\n\
\n\
PLM/ns instproc node {} {\n\
$self instvar node_\n\
return $node_\n\
}\n\
\n\
PLM/ns instproc debug { msg } {\n\
$self instvar debug_ ns_\n\
if {$debug_ <1} { return }\n\
\n\
$self instvar subscription_ node_\n\
set time [format %.05f [$ns_ now]]\n\
}\n\
\n\
PLM/ns instproc trace { trace } {\n\
$self instvar layers_\n\
foreach s $layers_ {\n\
[$s mon] trace $trace\n\
}\n\
}\n\
\n\
\n\
PLM/ns instproc total_bytes_delivered {} {\n\
$self instvar layers_\n\
set v 0\n\
foreach s $layers_ {\n\
incr v [[$s mon] set bytes]\n\
}\n\
return $v\n\
}\n\
\n\
\n\
\n\
Simulator instproc PLMcreate-agent { node type pktClass } {\n\
$self instvar Agents PortID \n\
set agent [new $type]\n\
$agent set fid_ $pktClass\n\
$self attach-agent $node $agent\n\
$agent proc get var {\n\
return [$self set $var]\n\
}\n\
return $agent\n\
}\n\
\n\
\n\
Simulator instproc PLMcbr_flow_PP { node fid addr bw } {\n\
global packetSize PP_burst_length\n\
set agent [$self PLMcreate-agent $node Agent/UDP $fid]\n\
set cbr [new Application/Traffic/CBR_PP]\n\
$cbr attach-agent $agent\n\
\n\
$agent set dst_addr_ $addr\n\
$agent set dst_port_ 0\n\
\n\
$cbr set packet_size_ $packetSize\n\
$cbr set rate_ $bw\n\
$cbr set random_ 1\n\
$cbr set PBM_ $PP_burst_length\n\
return $cbr\n\
}\n\
\n\
\n\
\n\
Simulator instproc PLMbuild_source_set { plmName rates addrs baseClass node when } {\n\
global src_plm src_rate\n\
set n [llength $rates]\n\
set r [lindex $rates 0]\n\
set addr [expr [lindex $addrs 0]]\n\
set src_rate($addr) $r\n\
set k $plmName:0\n\
set src_plm($k) [$self PLMcbr_flow_PP $node $baseClass $addr $r]\n\
$self at 0 \"$src_plm($k) set maxpkts_ 1; $src_plm($k) start\"\n\
$self at $when \"$src_plm($k) set maxpkts_ 268435456; $src_plm($k) start\"\n\
\n\
for {set i 1} {$i<$n} {incr i} {\n\
set r [lindex $rates $i]\n\
set addr [expr [lindex $addrs $i]]\n\
\n\
set src_rate($addr) $r\n\
set k $plmName:$i\n\
set src_plm($k) [$self PLMcbr_flow_PP $node $baseClass $addr $r]\n\
$self at 0 \"$src_plm($k) set maxpkts_ 1; $src_plm($k) start\"\n\
$self at $when \"$src_plm($k) set maxpkts_ 268435456; $src_plm($k) start\"\n\
}\n\
\n\
}\n\
\n\
Class PLMTopology\n\
\n\
PLMTopology instproc init { simulator } {\n\
$self instvar ns id\n\
set ns $simulator\n\
set id 0\n\
}\n\
\n\
PLMTopology instproc mknode nn {\n\
$self instvar node ns\n\
if ![info exists node($nn)] {\n\
set node($nn) [$ns node]\n\
}\n\
}\n\
\n\
\n\
PLMTopology instproc build_link { a b delay bw } {\n\
global buffers packetSize Queue_sched_\n\
if { $a == $b } {\n\
puts stderr \"link from $a to $b?\"\n\
exit 1\n\
}\n\
$self instvar node ns\n\
$self mknode $a\n\
$self mknode $b\n\
$ns duplex-link $node($a) $node($b) $bw $delay $Queue_sched_\n\
}\n\
\n\
PLMTopology instproc build_link-simple { a b delay bw f} {\n\
global buffers packetSize Queue_sched_ \n\
if { $a == $b } {\n\
puts stderr \"link from $a to $b?\"\n\
exit 1\n\
}\n\
$self instvar node ns\n\
$self mknode $a\n\
$self mknode $b\n\
$ns duplex-link-trace $node($a) $node($b) $bw $delay $Queue_sched_ $f\n\
}\n\
\n\
\n\
\n\
PLMTopology instproc place_source { nn when } {\n\
global rates \n\
$self instvar node ns id addrs\n\
\n\
incr id\n\
set addrs($id) {}\n\
foreach r $rates {\n\
lappend addrs($id) [Node allocaddr]\n\
}\n\
\n\
$ns PLMbuild_source_set s$id $rates $addrs($id) $id $node($nn) $when\n\
\n\
return $id\n\
}\n\
\n\
PLMTopology instproc place_receiver { nn id when check_estimate {nb 1}} {\n\
$self instvar ns  \n\
$ns at $when \"$self build_receiver $nn $id $check_estimate $nb\"\n\
}\n\
\n\
PLMTopology instproc build_receiver { nn id check_estimate nb} {\n\
$self instvar node ns addrs\n\
global PLMrcvr\n\
set PLMrcvr($nb) [new PLM/ns $ns $node($nn) $addrs($id) $check_estimate $nn]\n\
\n\
global plm_debug_flag\n\
$PLMrcvr($nb) set debug_ $plm_debug_flag\n\
}\n\
\n\
\n\
\n\
\n\
\n\
Simulator instproc mpls-node args {\n\
$self node-config -MPLS ON\n\
set n [$self node]\n\
$self node-config -MPLS OFF\n\
return $n\n\
}\n\
\n\
Simulator instproc LDP-peer { src dst } {\n\
if { ![$src is-neighbor $dst] } {\n\
return\n\
}\n\
set ldpsrc [[$src get-module \"MPLS\"] make-ldp]\n\
set ldpdst [[$dst get-module \"MPLS\"] make-ldp]\n\
$ldpsrc set-peer [$dst id]\n\
$ldpdst set-peer [$src id]\n\
$self connect $ldpsrc $ldpdst\n\
}\n\
\n\
Simulator instproc ldp-notification-color {color} {\n\
$self color 101 $color\n\
}\n\
\n\
Simulator instproc ldp-request-color {color} {\n\
$self color 102 $color\n\
}\n\
\n\
Simulator instproc ldp-mapping-color {color} {\n\
$self color 103 $color\n\
}\n\
\n\
Simulator instproc ldp-withdraw-color {color} {\n\
$self color 104 $color\n\
}\n\
\n\
Simulator instproc ldp-release-color {color} {\n\
$self color 105 $color\n\
}\n\
\n\
\n\
\n\
RtModule/MPLS instproc register { node } {\n\
$self instvar classifier_\n\
$self attach-node $node\n\
$node route-notify $self\n\
$node port-notify $self\n\
set classifier_ [new Classifier/Addr/MPLS]\n\
$classifier_ set-node $node $self\n\
$node install-entry $self $classifier_ 0\n\
$self attach-classifier $classifier_\n\
}\n\
\n\
\n\
\n\
\n\
RtModule/MPLS instproc enable-data-driven {} {\n\
[$self set classifier_] cmd enable-data-driven\n\
}\n\
\n\
RtModule/MPLS instproc enable-control-driven {} {\n\
[$self set classifier_] cmd enable-control-driven\n\
}\n\
\n\
RtModule/MPLS instproc make-ldp {} {\n\
set ldp [new Agent/LDP]\n\
$self cmd attach-ldp $ldp\n\
$ldp set-mpls-module $self\n\
[$self node] attach $ldp\n\
return $ldp\n\
}\n\
\n\
RtModule/MPLS instproc exist-fec {fec phb} {\n\
return [[$self set classifier_] exist-fec $fec $phb]\n\
}\n\
\n\
RtModule/MPLS instproc get-incoming-iface {fec lspid} {\n\
return [[$self set classifier_] GetInIface $fec $lspid]\n\
}\n\
\n\
RtModule/MPLS instproc get-incoming-label {fec lspid} {\n\
return [[$self set classifier_] GetInLabel $fec $lspid]\n\
}\n\
\n\
RtModule/MPLS instproc get-outgoing-label {fec lspid} {\n\
return [[$self set classifier_] GetOutLabel $fec $lspid]\n\
}\n\
\n\
RtModule/MPLS instproc get-outgoing-iface {fec lspid} {\n\
return [[$self set classifier_] GetOutIface $fec $lspid]\n\
}\n\
\n\
RtModule/MPLS instproc get-fec-for-lspid {lspid} {\n\
return [[$self set classifier_] get-fec-for-lspid $lspid]\n\
}\n\
\n\
RtModule/MPLS instproc in-label-install {fec lspid iface label} {\n\
set dontcare [Classifier/Addr/MPLS dont-care]\n\
$self label-install $fec $lspid $iface $label $dontcare $dontcare\n\
}\n\
\n\
RtModule/MPLS instproc out-label-install {fec lspid iface label} {\n\
set dontcare [Classifier/Addr/MPLS dont-care]\n\
$self label-install $fec $lspid $dontcare $dontcare $iface $label\n\
}\n\
\n\
RtModule/MPLS instproc in-label-clear {fec lspid} {\n\
set dontcare [Classifier/Addr/MPLS dont-care]\n\
$self label-clear $fec $lspid -1 -1 $dontcare $dontcare\n\
}\n\
\n\
RtModule/MPLS instproc out-label-clear {fec lspid} {\n\
set dontcare [Classifier/Addr/MPLS dont-care]\n\
$self label-clear $fec $lspid $dontcare $dontcare -1 -1\n\
}\n\
\n\
RtModule/MPLS instproc label-install {fec lspid iif ilbl oif olbl} {\n\
[$self set classifier_] LSPsetup $fec $lspid $iif $ilbl $oif $olbl\n\
}\n\
\n\
RtModule/MPLS instproc label-clear {fec lspid iif ilbl oif olbl} {\n\
[$self set classifier_] LSPrelease $fec $lspid $iif $ilbl $oif $olbl\n\
}\n\
\n\
RtModule/MPLS instproc flow-erlsp-install {fec phb lspid} {\n\
[$self set classifier_] ErLspBinding $fec $phb $lspid\n\
}\n\
\n\
RtModule/MPLS instproc erlsp-stacking {erlspid tunnelid} {\n\
[$self set classifier_] ErLspStacking -1 $erlspid -1 $tunnelid\n\
}\n\
\n\
RtModule/MPLS instproc flow-aggregation {fineFec finePhb coarseFec coarsePhb} {\n\
[$self set classifier_] FlowAggregation $fineFec $finePhb $coarseFec \\\n\
$coarsePhb\n\
}\n\
\n\
RtModule/MPLS instproc enable-reroute {option} {\n\
$self instvar classifier_ \n\
$classifier_ set enable_reroute_ 1\n\
if {$option == \"drop\"} {\n\
$classifier_ set reroute_option_ 0\n\
} elseif {$option == \"L3\"} {\n\
$classifier_ set reroute_option_ 1\n\
} elseif {$option == \"new\"} {\n\
$classifier_ set reroute_option_ 2\n\
} else {\n\
$classifier_ set reroute_option_ 0\n\
}\n\
}\n\
\n\
RtModule/MPLS instproc reroute-binding {fec phb lspid} {\n\
[$self set classifier_] aPathBinding $fec $phb -1 $lspid\n\
}\n\
\n\
RtModule/MPLS instproc lookup-nexthop {node fec} {\n\
set ns [Simulator instance]\n\
set routingtable [$ns get-routelogic]\n\
set nexthop [$routingtable lookup $node $fec]\n\
return $nexthop\n\
}\n\
\n\
RtModule/MPLS instproc get-nexthop {fec} {\n\
set nodeid [[$self node] id]\n\
set nexthop [$self lookup-nexthop $nodeid $fec]\n\
return $nexthop\n\
}\n\
\n\
RtModule/MPLS instproc get-link-status {hop} {\n\
if {$hop < 0} {\n\
return \"down\"\n\
}\n\
set nexthop [$self get-nexthop $hop]\n\
if {$nexthop == $hop} {\n\
set status \"up\"\n\
} else {\n\
set status \"down\"\n\
}\n\
return $status\n\
}\n\
\n\
RtModule/MPLS instproc is-egress-lsr { fec } {\n\
if { [[$self node] id] == $fec } {\n\
return  \"1\"\n\
}\n\
set nexthopid [$self get-nexthop $fec]\n\
if { $nexthopid < 0 } {\n\
return \"-1\"\n\
}\n\
set nexthop [[Simulator instance] get-node-by-id $nexthopid]\n\
if { [$nexthop get-module \"MPLS\"] == \"\" } {\n\
return  \"1\"\n\
} else {\n\
return  \"-1\"\n\
}\n\
}\n\
\n\
RtModule/MPLS instproc ldp-trigger-by-routing-table {} {\n\
if { [[$self set classifier_] cmd control-driven?] != 1 } {\n\
return\n\
}\n\
set ns [Simulator instance]\n\
for {set i 0} {$i < [$ns get-number-of-nodes]} {incr i} {\n\
set host [$ns get-node-by-id $i]\n\
if { [$self is-egress-lsr [$host id]] == 1 } {\n\
$self ldp-trigger-by-control [$host id] *\n\
}\n\
}\n\
}\n\
\n\
RtModule/MPLS instproc ldp-trigger-by-control {fec pathvec} {\n\
lappend pathvec [[$self node] id]\n\
set inlabel [$self get-incoming-label $fec -1]\n\
set nexthop [$self get-nexthop $fec]\n\
set ldpagents [lsort [$self get-ldp-agents]]\n\
for {set i 0} {$i < [llength $ldpagents]} {incr i 1} {\n\
set ldpagent [lindex $ldpagents $i]\n\
if { [$ldpagent peer-ldpnode] == $nexthop } {\n\
continue\n\
}\n\
if { $inlabel == -1 } {\n\
if { [$self is-egress-lsr $fec] == 1 } {\n\
set inlabel 0\n\
} else {\n\
set inlabel [$self new-incoming-label]\n\
$self in-label-install $fec -1 -1 $inlabel\n\
}\n\
}\n\
$ldpagent new-msgid\n\
$ldpagent send-mapping-msg $fec $inlabel $pathvec -1\n\
}\n\
}\n\
\n\
RtModule/MPLS instproc ldp-trigger-by-data {reqmsgid src fec pathvec} {\n\
if { [$self is-egress-lsr $fec] == 1 } {\n\
return\n\
}\n\
set outlabel [$self get-outgoing-label $fec -1]\n\
if { $outlabel > -1  } {\n\
set outiface [$self get-outgoing-iface $fec -1]\n\
if { [$self get-link-status $outiface] == \"up\" } {\n\
return\n\
}\n\
}\n\
lappend pathvec [[$self node] id]      \n\
set nexthop [$self get-nexthop $fec]\n\
set ldpagent [$self get-ldp-agent $nexthop]\n\
if { $ldpagent == \"\" } {\n\
return\n\
}\n\
if {$reqmsgid > -1} {\n\
set working [$ldpagent msgtbl-get-msgid $fec -1 $src]\n\
if { $working < 0 } {\n\
set newmsgid [$ldpagent new-msgid]\n\
$ldpagent msgtbl-install $newmsgid $fec -1 \\\n\
$src $reqmsgid\n\
$ldpagent send-request-msg $fec $pathvec\n\
} else {\n\
}\n\
} else {\n\
if {$fec == $nexthop} {\n\
set outlabel 0\n\
} else {\n\
set outlabel [$self new-outgoing-label]\n\
}\n\
$self out-label-install $fec -1 $nexthop $outlabel\n\
$ldpagent new-msgid\n\
$ldpagent send-mapping-msg $fec $outlabel $pathvec -1\n\
}\n\
}\n\
\n\
RtModule/MPLS instproc make-explicit-route {fec er lspid rc} {\n\
$self ldp-trigger-by-explicit-route -1 [[$self node] id] $fec \"*\" \\\n\
$er $lspid $rc\n\
}\n\
\n\
RtModule/MPLS instproc ldp-trigger-by-explicit-route {reqmsgid src fec \\\n\
pathvec er lspid rc} {\n\
$self instvar classifier_\n\
set outlabel [$self get-outgoing-label $fec $lspid]\n\
if { $outlabel > -1 } {\n\
return\n\
}\n\
if { [[$self node] id] != $src && [[$self node] id] == $fec } {\n\
set ldpagent [$self get-ldp-agent $src]\n\
if { $ldpagent != \"\" } {\n\
$ldpagent new-msgid\n\
$ldpagent send-cr-mapping-msg $fec 0 $lspid $reqmsgid\n\
}\n\
return\n\
}\n\
lappend pathvec [[$self node] id]\n\
set er [split $er \"_\"]\n\
set erlen [llength $er]\n\
for {set i 0} {$i <= $erlen} {incr i 1} {\n\
if { $i != $erlen } {\n\
set erhop [lindex $er $i]\n\
} else {\n\
set erhop $fec\n\
}\n\
set stackERhop -1\n\
if { $erhop >= [Classifier/Addr/MPLS minimum-lspid] } {\n\
set lspidFEC [$self get-fec-for-lspid $erhop]\n\
set inlabel  [$self get-incoming-label -1 $erhop]\n\
set outlabel [$self get-outgoing-label -1 $erhop]\n\
if { $lspidFEC == $fec } {\n\
if { $outlabel <= -1 } {\n\
continue\n\
}\n\
if { $inlabel < 0 } {\n\
set inlabel [$self new-incoming-label]\n\
$self in-label-install -1 $erhop \\\n\
$src $inlabel\n\
}\n\
set ldpagent [$self get-ldp-agent $src]\n\
$ldpagent new-msgid\n\
$ldpagent send-cr-mapping-msg $fec $inlabel \\\n\
$lspid $reqmsgid\n\
return\n\
}\n\
set existExplicitPeer [$self exist-ldp-agent $lspidFEC]\n\
if { $outlabel > -1 && $existExplicitPeer == \"1\" } {\n\
set stackERhop $erhop \n\
set erhop $lspidFEC\n\
} elseif { $outlabel > -1 && $existExplicitPeer == \"0\" } {\n\
set nexthop [$self get-outgoing-iface -1 \\\n\
$erhop]\n\
set iiface  [$self get-incoming-iface -1 \\\n\
$erhop]\n\
set ldpagent [$self get-ldp-agent $nexthop]\n\
set working [$ldpagent msgtbl-get-msgid $fec \\\n\
$lspid $src]\n\
if { $working < 0 } {\n\
set newmsgid [$ldpagent new-msgid]\n\
$ldpagent msgtbl-install $newmsgid \\\n\
$fec $lspid $src $reqmsgid\n\
if {($iiface == $src) && \\\n\
($inlabel > -1) } {\n\
$ldpagent msgtbl-set-labelpass $newmsgid\n\
} else {\n\
$ldpagent msgtbl-set-labelstack $newmsgid $erhop\n\
}\n\
$ldpagent send-cr-request-msg $fec \\\n\
$pathvec $er $lspid $rc\n\
}\n\
return\n\
} else {\n\
continue\n\
}\n\
}\n\
if { [lsearch $pathvec $erhop] < 0 } {\n\
set nexthop [$self get-nexthop $erhop]\n\
if { [$self is-egress-lsr $nexthop] == 1 } {\n\
set ldpagent [$self get-ldp-agent $src]\n\
if { $erhop == $fec } {\n\
$ldpagent new-msgid\n\
$ldpagent send-cr-mapping-msg $fec 0 \\\n\
$lspid $reqmsgid\n\
} else {\n\
$ldpagent new-msgid\n\
$ldpagent send-notification-msg \\\n\
\"NoRoute\" $lspid\n\
}\n\
} else {\n\
set ldpagent [$self get-ldp-agent $nexthop]\n\
set working [$ldpagent msgtbl-get-msgid $fec \\\n\
$lspid $src]\n\
if { $working < 0 } {\n\
set newmsgid [$ldpagent new-msgid]\n\
set id [$ldpagent msgtbl-install \\\n\
$newmsgid $fec \\\n\
$lspid $src $reqmsgid]\n\
if { $stackERhop > -1 } {\n\
$ldpagent msgtbl-set-labelstack $newmsgid $stackERhop\n\
}\n\
$ldpagent send-cr-request-msg $fec $pathvec $er $lspid $rc\n\
}\n\
} \n\
return\n\
}\n\
}\n\
set ldpagent [$self get-ldp-agent $src]\n\
if { $ldpagent != \"\" } {\n\
$ldpagent new-msgid\n\
$ldpagent send-notification-msg \"NoRoute\" $lspid\n\
}\n\
}\n\
\n\
RtModule/MPLS instproc ldp-trigger-by-withdraw {fec lspid} {\n\
set inlabel  [$self get-incoming-label $fec $lspid]\n\
set iniface  [$self get-incoming-iface $fec $lspid]\n\
\n\
$self in-label-clear $fec $lspid\n\
\n\
if {$iniface > -1} {\n\
set ldpagent [$self get-ldp-agent $iniface]\n\
if { $ldpagent != \"\" } {\n\
$ldpagent new-msgid\n\
$ldpagent send-withdraw-msg $fec $lspid\n\
}\n\
} else {\n\
set nexthop [$self get-nexthop $fec]\n\
set ldpagents [lsort [$self get-ldp-agents]]\n\
for {set i 0} {$i < [llength $ldpagents]} {incr i 1} {\n\
set ldpagent [lindex $ldpagents $i]\n\
if { [$ldpagent peer-ldpnode] == $nexthop } {\n\
continue\n\
}\n\
$ldpagent new-msgid\n\
$ldpagent send-withdraw-msg $fec $lspid\n\
}\n\
}   \n\
}\n\
\n\
RtModule/MPLS instproc ldp-trigger-by-release {fec lspid} {\n\
set outlabel  [$self get-outgoing-label $fec $lspid]\n\
if {$outlabel < 0} {\n\
return\n\
}\n\
set nexthop [$self get-outgoing-iface $fec $lspid]\n\
$self out-label-clear $fec $lspid \n\
set ldpagent [$self get-ldp-agent $nexthop]\n\
if { $ldpagent != \"\" } {\n\
$ldpagent new-msgid\n\
$ldpagent send-release-msg $fec $lspid\n\
}   \n\
}\n\
\n\
\n\
RtModule/MPLS instproc trace-mpls {} {\n\
[$self set classifier_] set trace_mpls_ 1\n\
}\n\
\n\
RtModule/MPLS instproc pft-dump {} {\n\
set nodeid [[$self node] id]\n\
[$self set classifier_] PFTdump $nodeid\n\
}\n\
\n\
RtModule/MPLS instproc erb-dump {} {\n\
set nodeid [[$self node] id]\n\
[$self set classifier_] ERBdump $nodeid\n\
}\n\
\n\
RtModule/MPLS instproc lib-dump {} {\n\
set nodeid [[$self node] id]\n\
[$self set classifier_] LIBdump $nodeid\n\
}\n\
\n\
\n\
\n\
\n\
\n\
Agent/LDP instproc set-mpls-module { mod } {\n\
$self set module_ $mod\n\
}\n\
\n\
Agent/LDP instproc get-request-msg {msgid src fec pathvec} {\n\
$self instvar node_ module_\n\
\n\
set pathvec [split $pathvec \"_\"]\n\
if {[lsearch $pathvec [$node_ id]] > -1} {\n\
set ldpagent [$module_ get-ldp-agent $src]\n\
$ldpagent new-msgid\n\
$ldpagent send-notification-msg \"LoopDetected\" -1           \n\
\n\
return\n\
}\n\
\n\
set nexthop [$module_ get-nexthop $fec]\n\
if {$src == $nexthop} {\n\
$self request-msg-from-downstream $msgid $src $fec $pathvec\n\
} else {\n\
$self request-msg-from-upstream $msgid $src $fec $pathvec\n\
}\n\
}\n\
\n\
Agent/LDP instproc request-msg-from-downstream {msgid src fec pathvec} {\n\
$self instvar module_\n\
\n\
set outlabel [$module_ get-outgoing-label $fec -1]\n\
if { $outlabel < 0 } {\n\
if { $fec == $src } {\n\
set outlabel 0\n\
} else {\n\
set outlabel [$module_ new-outgoing-label]\n\
}     \n\
$module_ out-label-install $fec -1 $src $outlabel             \n\
} else {\n\
set outIface [$module_ get-outgoing-iface $fec -1]\n\
if { $src != $outIface} {\n\
$module_ out-label-install $fec -1 $src $outlabel\n\
} \n\
}\n\
\n\
set ldpagent [$module_ get-ldp-agent $src]\n\
$ldpagent new-msgid\n\
$ldpagent send-mapping-msg $fec $outlabel \"*\" $msgid\n\
\n\
$module_ ldp-trigger-by-control $fec $pathvec\n\
}\n\
\n\
Agent/LDP instproc request-msg-from-upstream {msgid src fec pathvec} {\n\
$self instvar module_\n\
\n\
set ldpagent [$module_ get-ldp-agent $src]\n\
\n\
if { [$module_ is-egress-lsr $fec] == 1 } {\n\
$ldpagent new-msgid\n\
$ldpagent send-mapping-msg $fec 0 \"*\" $msgid\n\
return\n\
}\n\
\n\
set inlabel  [$module_ get-incoming-label $fec -1]\n\
set outlabel [$module_ get-outgoing-label $fec -1]\n\
if { [Classifier/Addr/MPLS ordered-control?] == 1 } {\n\
if { $outlabel > -1 } {\n\
if { $inlabel < 0 } {\n\
set inlabel [$module_ new-incoming-label]\n\
}\n\
$ldpagent new-msgid\n\
$ldpagent send-mapping-msg $fec inlabel \"*\" $msgid\n\
} else {\n\
$module_ ldp-trigger-by-data $msgid $src $fec $pathvec\n\
}\n\
return\n\
}\n\
if { $inlabel < 0 } {\n\
set inlabel [$module_ new-incoming-label]\n\
$module_ in-label-install $fec -1 $src $inlabel\n\
} else {\n\
set inIface [$module_ get-incoming-iface $fec -1]\n\
if { $src != $inIface} {\n\
set classifier [$module_ set classifier_]\n\
set dontcare [$classifier set dont_care_]\n\
$module_ in-label-install $fec -1 -1 $dontcare\n\
} \n\
}\n\
$ldpagent new-msgid\n\
$ldpagent send-mapping-msg $fec $inlabel \"*\" $msgid\n\
$module_ ldp-trigger-by-data $msgid $src $fec $pathvec\n\
}\n\
\n\
Agent/LDP instproc get-cr-request-msg {msgid src fec pathvec er lspid rc} {\n\
$self instvar node_ module_\n\
\n\
set pathvec [split $pathvec \"_\"]\n\
if {[lsearch $pathvec [$node_ id]] > -1} {\n\
set ldpagent [$module_ get-ldp-agent $src]\n\
$ldpagent new-msgid\n\
$ldpagent send-notification-msg \"NoRoute\" $lspid\n\
return\n\
}\n\
\n\
\n\
set ldpagent [$module_ get-ldp-agent $src]\n\
\n\
set inlabel [$module_ get-incoming-label $fec $lspid]\n\
set outlabel [$module_ get-outgoing-label $fec $lspid]\n\
\n\
if { $outlabel > -1 } {\n\
if { $inlabel < 0 } {\n\
set inlabel [$module_ new-incoming-label]\n\
$module_ in-label-install $fec $lspid $src $inlabel\n\
}\n\
$ldpagent new-msgid\n\
$ldpagent send-cr-mapping-msg $fec $inlabel $lspid $msgid\n\
return\n\
}\n\
\n\
$module_ ldp-trigger-by-explicit-route $msgid $src $fec $pathvec $er \\\n\
$lspid $rc\n\
}\n\
\n\
Agent/LDP instproc get-cr-mapping-msg {msgid src fec label lspid reqmsgid} {\n\
$self instvar node_ trace_ldp_ module_\n\
\n\
\n\
set prvsrc   [$self msgtbl-get-src       $reqmsgid]\n\
set prvmsgid [$self msgtbl-get-reqmsgid  $reqmsgid]\n\
set labelop  [$self msgtbl-get-labelop   $reqmsgid]\n\
if {$labelop == 2} {\n\
set tunnelid [$self msgtbl-get-erlspid   $reqmsgid]\n\
} else {\n\
set tunnelid -1\n\
}\n\
\n\
$self msgtbl-clear $reqmsgid\n\
\n\
if { $trace_ldp_ } {\n\
puts \"$src -> [$node_ id] : prvsrc($prvsrc)\"\n\
}\n\
\n\
if { $labelop == 2 } {\n\
$module_ out-label-install $fec $lspid $src $label\n\
$module_ erlsp-stacking $lspid $tunnelid\n\
} elseif {$labelop == 1} {\n\
set ldpagent [$module_ get-ldp-agent $prvsrc]\n\
$ldpagent new-msgid\n\
$ldpagent send-cr-mapping-msg $fec $label $lspid $prvmsgid\n\
return\n\
} else {\n\
$module_ out-label-install $fec $lspid  $src $label\n\
}\n\
\n\
if {$prvsrc == [$node_ id]} {\n\
return\n\
}\n\
\n\
set inlabel [$module_ new-incoming-label]\n\
$module_ in-label-install $fec $lspid $prvsrc $inlabel\n\
\n\
set ldpagent [$module_ get-ldp-agent $prvsrc]\n\
$ldpagent new-msgid\n\
$ldpagent send-cr-mapping-msg $fec $inlabel $lspid $prvmsgid\n\
}\n\
\n\
Agent/LDP instproc get-mapping-msg {msgid src fec label pathvec reqmsgid} {\n\
$self instvar node_ trace_ldp_ module_\n\
\n\
if { $trace_ldp_ } {\n\
puts \"[[Simulator instance] now]: <mapping-msg> $src ->\\\n\
[$node_ id] : fec($fec), label($label) [$module_ get-nexthop $fec]\"\n\
}\n\
\n\
set pathvec [split $pathvec \"_\"]\n\
if {[lsearch $pathvec [$node_ id]] > -1} {\n\
set ldpagent [$module_ get-ldp-agent $src]\n\
$ldpagent new-msgid\n\
$ldpagent send-notification-msg \"LoopDetected\" -1           \n\
return\n\
}\n\
\n\
set nexthop [$module_ get-nexthop $fec]\n\
if {$src == $nexthop} {\n\
$self mapping-msg-from-downstream $msgid $src $fec $label \\\n\
$pathvec $reqmsgid\n\
} else {\n\
$self mapping-msg-from-upstream $msgid $src $fec $label \\\n\
$pathvec $reqmsgid\n\
}\n\
}\n\
\n\
Agent/LDP instproc mapping-msg-from-downstream {msgid src fec label \\\n\
pathvec reqmsgid} {\n\
$self instvar node_ module_\n\
\n\
$module_ out-label-install $fec -1 $src $label\n\
if { $reqmsgid > -1 } {\n\
\n\
set prvsrc   [$self msgtbl-get-src      $reqmsgid]\n\
set prvmsgid [$self msgtbl-get-reqmsgid $reqmsgid]\n\
$self msgtbl-clear $reqmsgid\n\
if { $prvsrc == [$node_ id] || $prvsrc < 0} {\n\
return\n\
}\n\
if { [Classifier/Addr/MPLS ordered-control?] == 1 } {\n\
\n\
set inlabel [$module_ new-incoming-label]\n\
$module_ in-label-install $fec -1 $prvsrc $inlabel\n\
set ldpagent [$module_ get-ldp-agent $prvsrc]\n\
$ldpagent new-msgid\n\
$ldpagent send-mapping-msg $fec $inlabel -1 $prvmsgid\n\
return\n\
}\n\
} else {\n\
$module_ ldp-trigger-by-control $fec $pathvec\n\
return\n\
}\n\
}\n\
\n\
Agent/LDP instproc mapping-msg-from-upstream {msgid src fec label pathvec \\\n\
reqmsgid} {\n\
$self instvar node_ module_\n\
\n\
set nexthop [$module_ lookup-nexthop $src $fec]\n\
if { $nexthop != [$node_ id] } {\n\
return\n\
}\n\
\n\
set inlabel [$module_ get-incoming-label $fec -1]\n\
if { $inlabel == -1 } {\n\
if { [$module_ is-egress-lsr $fec] == 1 } {\n\
if { $label != 0 } {\n\
set ldpagent [$module_ get-ldp-agent $src]\n\
$ldpagent new-msgid\n\
$ldpagent send-mapping-msg $fec 0 \"*\" $msgid\n\
}\n\
} else {\n\
$module_ in-label-install $fec -1 $src $label\n\
}\n\
} else {\n\
set ldpagent [$module_ get-ldp-agent $src]\n\
if { $reqmsgid < 0 } {\n\
$ldpagent new-msgid\n\
$ldpagent send-mapping-msg $fec $inlabel \"*\" $msgid\n\
}   \n\
}    \n\
if { $reqmsgid < 0 } {\n\
$module_ ldp-trigger-by-data -1 $src $fec $pathvec\n\
} else {   \n\
$self msgtbl-clear $reqmsgid\n\
}    \n\
}\n\
\n\
Agent/LDP instproc get-notification-msg {src status lspid} {\n\
$self instvar node_ trace_ldp_ module_\n\
\n\
if { $trace_ldp_ } {\n\
puts \"Notification($src->[$node_ id]): $status src=$src lspid=$lspid\"\n\
}\n\
set msgid [$self msgtbl-get-msgid -1 $lspid -1]\n\
if {$msgid > -1} {\n\
set prvsrc   [$self msgtbl-get-src      $msgid]\n\
$self msgtbl-clear $msgid            \n\
if { $prvsrc < -1 || $prvsrc == [$node_ id] } {\n\
return\n\
}\n\
set ldpagent [$module_ get-ldp-agent $prvsrc]\n\
$ldpagent new-msgid\n\
$ldpagent send-notification-msg $status $lspid\n\
}\n\
}\n\
\n\
Agent/LDP instproc get-withdraw-msg {src fec lspid} {\n\
$self instvar module_\n\
\n\
set outiface  [$module_ get-outgoing-iface $fec $lspid]\n\
if {$src == $outiface} {\n\
$module_ out-label-clear $fec $lspid\n\
set inlabel [$module_ get-incoming-label $fec $lspid]\n\
if {$inlabel > -1} {\n\
$module_ ldp-trigger-by-withdraw $fec $lspid\n\
}\n\
}\n\
}\n\
\n\
Agent/LDP instproc get-release-msg {src fec lspid} {\n\
$self instvar module_\n\
\n\
set iniface  [$module_ get-incoming-iface $fec $lspid]\n\
set outlabel [$module_ get-outgoing-label $fec $lspid]\n\
if {$iniface == $src} {\n\
$module_ in-label-clear $fec $lspid \n\
if {$outlabel > -1} {\n\
$module_ ldp-trigger-by-release $fec $lspid\n\
}\n\
} \n\
}\n\
\n\
Agent/LDP instproc trace-ldp-packet {src_addr src_port msgtype msgid fec \\\n\
label pathvec lspid er rc reqmsgid status atime} {\n\
$self instvar node_\n\
puts \"$atime [$node_ id]: $src_addr ($msgtype $msgid) $fec $label $pathvec  \\[$reqmsgid $status\\]  \\[$lspid $er $rc\\]\"\n\
}\n\
\n\
Agent/LDP instproc send-notification-msg {status lspid} {\n\
$self set fid_ 101\n\
$self cmd notification-msg $status $lspid\n\
}\n\
\n\
Agent/LDP instproc send-request-msg {fec pathvec} {\n\
$self set fid_ 102\n\
$self request-msg $fec $pathvec\n\
}\n\
\n\
Agent/LDP instproc send-mapping-msg {fec label pathvec reqmsgid} {\n\
$self set fid_ 103\n\
$self cmd mapping-msg $fec $label $pathvec $reqmsgid\n\
}\n\
\n\
Agent/LDP instproc send-withdraw-msg {fec lspid} {\n\
$self set fid_ 104\n\
$self withdraw-msg $fec $lspid\n\
}\n\
\n\
Agent/LDP instproc send-release-msg {fec lspid} {\n\
$self set fid_ 105\n\
$self release-msg $fec $lspid\n\
}\n\
\n\
Agent/LDP instproc send-cr-request-msg {fec pathvec er lspid rc} {\n\
$self set fid_ 102\n\
$self cr-request-msg $fec $pathvec $er $lspid $rc\n\
}\n\
\n\
Agent/LDP instproc send-cr-mapping-msg {fec inlabel lspid prvmsgid} {\n\
$self set fid_ 103\n\
$self cr-mapping-msg $fec $inlabel $lspid $prvmsgid\n\
}\n\
\n\
\n\
\n\
\n\
\n\
Classifier/Addr/MPLS instproc init {args} {\n\
eval $self next $args\n\
$self set rtable_ \"\"\n\
}\n\
\n\
Classifier/Addr/MPLS instproc set-node { node module } {\n\
$self set mpls_node_ $node\n\
$self set mpls_mod_ $module\n\
}\n\
\n\
Classifier/Addr/MPLS instproc no-slot args {\n\
}\n\
\n\
\n\
Classifier/Addr/MPLS instproc trace-packet-switching { time src dst ptype \\\n\
ilabel op oiface olabel ttl psize } {\n\
$self instvar mpls_node_ \n\
puts \"$time [$mpls_node_ id]($src->$dst): $ptype $ilabel $op $oiface $olabel $ttl $psize\"\n\
}\n\
\n\
\n\
Classifier/Addr/MPLS instproc ldp-trigger-by-switch { fec } {\n\
$self instvar mpls_node_ mpls_mod_\n\
if { [Classifier/Addr/MPLS on-demand?] == 1 } {\n\
set msgid  1\n\
} else {\n\
set msgid -1\n\
}\n\
$mpls_mod_ ldp-trigger-by-data $msgid [$mpls_node_ id] $fec *\n\
}\n\
\n\
Classifier/Addr/MPLS instproc rtable-ready { fec } {\n\
$self instvar rtable_\n\
set ns [Simulator instance]\n\
if { [lsearch $rtable_ $fec] == -1 } {\n\
lappend rtable_ $fec\n\
}\n\
set rtlen [llength $rtable_]\n\
set nodelen [$ns array size Node_]\n\
if { $rtlen == $nodelen } {\n\
return 1\n\
} else {\n\
return 0\n\
}\n\
}\n\
\n\
Classifier/Addr/MPLS instproc routing-new { slot time } {\n\
$self instvar mpls_node_ rtable_ mpls_mod_\n\
if { [$self control-driven?] != 1 } {\n\
return\n\
}\n\
if { [lsearch $rtable_ [$mpls_node_ id]] == -1 } {\n\
lappend rtable_ [$mpls_node_ id]\n\
}\n\
if { [$self rtable-ready $slot] == 1 } {\n\
set rtlen [llength $rtable_]\n\
for {set i 0} {$i < $rtlen} {incr i 1} {\n\
set nodeid [lindex $rtable_ $i]\n\
if { [$mpls_mod_ get-nexthop $nodeid] == -1 } {\n\
set rtable_ \"\" \n\
return\n\
}\n\
}\n\
set rtable_ \"\" \n\
[Simulator instance] at [expr $time] \\\n\
\"$mpls_mod_ ldp-trigger-by-routing-table\"\n\
}\n\
}\n\
\n\
Classifier/Addr/MPLS instproc routing-nochange {slot time} {\n\
$self instvar mpls_node_ rtable_ mpls_mod_\n\
\n\
if { [$self control-driven?] != 1 } {\n\
return\n\
}\n\
if { [lsearch $rtable_ [$mpls_node_ id]] == -1 } {\n\
lappend rtable_ [$mpls_node_ id]\n\
}\n\
if { [$self rtable-ready $slot] == 1 } {\n\
set rtable_ \"\" \n\
[Simulator instance] at $time \\\n\
\"$mpls_mod_ ldp-trigger-by-routing-table\"\n\
}\n\
}\n\
\n\
Classifier/Addr/MPLS instproc routing-update {slot time} {\n\
$self instvar mpls_mod_ rtable_\n\
if {[$self control-driven?] != 1} {\n\
return\n\
}\n\
set fec $slot\n\
set pft_outif [$mpls_mod_ get-outgoing-iface $fec -1]\n\
set rt_outif  [$mpls_mod_ get-nexthop $fec]\n\
if { $pft_outif == -1 || $rt_outif == -1 } {\n\
return\n\
}\n\
$mpls_mod_ ldp-trigger-by-control $fec *\n\
return\n\
}\n\
\n\
\n\
\n\
\n\
\n\
\n\
set tcl_precision 17\n\
\n\
Connector set debug_ false\n\
TTLChecker set debug_ false\n\
\n\
Trace set src_ -1\n\
Trace set dst_ -1\n\
Trace set callback_ 0\n\
Trace set show_tcphdr_ 0\n\
Trace set debug_ false\n\
\n\
CMUTrace set debug_ false\n\
\n\
Scheduler/RealTime set maxslop_ 0.010; # max allowed slop b4 error (sec)\n\
\n\
Integrator set lastx_ 0.0\n\
Integrator set lasty_ 0.0\n\
Integrator set sum_ 0.0\n\
\n\
Queue set limit_ 50\n\
Queue set blocked_ false\n\
Queue set unblock_on_resume_ true\n\
\n\
Queue set interleave_ false\n\
Queue set acksfirst_ false\n\
Queue set ackfromfront_ false\n\
Queue set debug_ false\n\
\n\
Queue/SFQ set maxqueue_ 40\n\
Queue/SFQ set buckets_ 16\n\
\n\
Queue/FQ set secsPerByte_ 0\n\
FQLink set queueManagement_ DropTail\n\
\n\
Queue/DropTail set drop_front_ false\n\
\n\
Queue/DropTail/PriQueue set Prefer_Routing_Protocols    1\n\
\n\
Queue/dsRED set numQueues_ 4\n\
Queue/dsRED set ecn_ 0\n\
\n\
Queue/RED set bytes_ false\n\
Queue/RED set queue_in_bytes_ false\n\
Queue/RED set thresh_ 5\n\
Queue/RED set maxthresh_ 15\n\
Queue/RED set mean_pktsize_ 500\n\
Queue/RED set q_weight_ 0.002\n\
Queue/RED set wait_ true\n\
Queue/RED set linterm_ 10\n\
Queue/RED set setbit_ false\n\
Queue/RED set gentle_ true\n\
Queue/RED set drop_tail_ true\n\
Queue/RED set drop_front_ false\n\
Queue/RED set drop_rand_ false\n\
Queue/RED set doubleq_ false\n\
Queue/RED set ns1_compat_ false\n\
Queue/RED set dqthresh_ 50\n\
Queue/RED set ave_ 0.0\n\
Queue/RED set prob1_ 0.0\n\
Queue/RED set curq_ 0\n\
\n\
Queue/RED/RIO set bytes_ false\n\
Queue/RED/RIO set queue_in_bytes_ false\n\
Queue/RED/RIO set thresh_ 5\n\
Queue/RED/RIO set maxthresh_ 15\n\
Queue/RED/RIO set in_thresh_ 15\n\
Queue/RED/RIO set in_maxthresh_ 30\n\
Queue/RED/RIO set out_thresh_ 5\n\
Queue/RED/RIO set out_maxthresh_ 15\n\
Queue/RED/RIO set mean_pktsize_ 500\n\
Queue/RED/RIO set q_weight_ 0.002\n\
Queue/RED/RIO set wait_ true\n\
Queue/RED/RIO set linterm_ 10\n\
Queue/RED/RIO set in_linterm_ 50\n\
Queue/RED/RIO set out_linterm_ 5\n\
Queue/RED/RIO set setbit_ false\n\
Queue/RED/RIO set gentle_ false\n\
Queue/RED/RIO set in_gentle_ false\n\
Queue/RED/RIO set out_gentle_ false\n\
Queue/RED/RIO set drop_tail_ true\n\
Queue/RED/RIO set drop_front_ false\n\
Queue/RED/RIO set drop_rand_ false\n\
Queue/RED/RIO set doubleq_ false\n\
Queue/RED/RIO set ns1_compat_ false\n\
Queue/RED/RIO set dqthresh_ 50\n\
Queue/RED/RIO set ave_ 0.0\n\
Queue/RED/RIO set in_ave_ 0.0\n\
Queue/RED/RIO set out_ave_ 0.0\n\
Queue/RED/RIO set prob1_ 0.0\n\
Queue/RED/RIO set in_prob1_ 0.0\n\
Queue/RED/RIO set out_prob1_ 0.0\n\
Queue/RED/RIO set curq_ 0\n\
Queue/RED/RIO set priority_method_ 0\n\
\n\
Queue/RED/PD set auto_ false\n\
Queue/RED/PD set global_target_ false\n\
Queue/RED/PD set noMonitored_ 0\n\
Queue/RED/PD set targetBW_ 0\n\
Queue/RED/PD set unresponsive_penalty_ 1\n\
Queue/RED/PD set P_testFRp_ -1\n\
Queue/RED/PD set noidle_ false\n\
\n\
Queue/DRR set buckets_ 10\n\
Queue/DRR set blimit_ 25000\n\
Queue/DRR set quantum_ 250\n\
Queue/DRR set mask_ 0\n\
\n\
Queue/CBQ set algorithm_ 0 ;# used by compat only, not bound\n\
Queue/CBQ set maxpkt_ 1024\n\
CBQClass set priority_ 0\n\
CBQClass set level_ 1\n\
CBQClass set extradelay_ 0.0\n\
CBQClass set def_qtype_ DropTail\n\
CBQClass set okborrow_ true\n\
CBQClass set automaxidle_gain_ 0.9375\n\
CBQClass set debug_ false\n\
\n\
SnoopQueue/In set debug_ false\n\
SnoopQueue/Out set debug_ false\n\
SnoopQueue/Drop set debug_ false\n\
SnoopQueue/EDrop set debug_ false\n\
SnoopQueue/Tagger set debug_ false\n\
\n\
PacketQueue/Semantic set acksfirst_ false\n\
PacketQueue/Semantic set filteracks_ false\n\
PacketQueue/Semantic set replace_head_ false\n\
PacketQueue/Semantic set priority_drop_ false\n\
PacketQueue/Semantic set random_drop_ false\n\
PacketQueue/Semantic set reconsAcks_ false\n\
PacketQueue/Semantic set random_ecn_ false\n\
\n\
QueueMonitor set size_ 0\n\
QueueMonitor set pkts_ 0\n\
QueueMonitor set parrivals_ 0\n\
QueueMonitor set barrivals_ 0\n\
QueueMonitor set pdepartures_ 0\n\
QueueMonitor set bdepartures_ 0\n\
QueueMonitor set pdrops_ 0\n\
QueueMonitor set bdrops_ 0\n\
\n\
QueueMonitor set k_ 0.1\n\
QueueMonitor set prevTime_ 0\n\
QueueMonitor set startTime_ 0\n\
QueueMonitor set estRate_ 0\n\
QueueMonitor set estimate_rate_ 0\n\
\n\
QueueMonitor/ED set epdrops_ 0\n\
QueueMonitor/ED set ebdrops_ 0\n\
\n\
QueueMonitor/ED set mon_epdrops_ 0                     \n\
QueueMonitor/ED set mon_ebdrops_ 0\n\
\n\
QueueMonitor/ED/Flowmon set enable_in_ true\n\
QueueMonitor/ED/Flowmon set enable_out_ true\n\
QueueMonitor/ED/Flowmon set enable_drop_ true\n\
QueueMonitor/ED/Flowmon set enable_edrop_ true\n\
QueueMonitor/ED/Flowmon set enable_mon_edrop_ true\n\
\n\
QueueMonitor/ED/Flow set src_ -1\n\
QueueMonitor/ED/Flow set dst_ -1\n\
QueueMonitor/ED/Flow set flowid_ -1\n\
\n\
QueueMonitor/ED/Flow/TB set target_rate_ 128000 \n\
QueueMonitor/ED/Flow/TB set bucket_depth_ 10000\n\
QueueMonitor/ED/Flow/TB set tbucket_ 10000\n\
QueueMonitor/ED/Flow/TSW set target_rate_ 0\n\
QueueMonitor/ED/Flow/TSW set win_len_ 10\n\
QueueMonitor/ED/Flow/TSW set wait_ true\n\
\n\
QueueMonitor/ED/Flow/RedPD set targetBW_ 0\n\
QueueMonitor/ED/Flow/RedPD set currentBW_ 0\n\
QueueMonitor/ED/Flow/RedPD set monitored_ 0\n\
QueueMonitor/ED/Flow/RedPD set unresponsive_ 0\n\
QueueMonitor/ED/Flow/RedPD set monitorStartTime_ 0\n\
QueueMonitor/ED/Flow/RedPD set unresponsiveStartTime_ 0\n\
QueueMonitor/ED/Flow/RedPD set lastDropTime_ 0 \n\
QueueMonitor/ED/Flow/RedPD set auto_ 0 \n\
\n\
DelayLink set bandwidth_ 1.5Mb\n\
DelayLink set delay_ 100ms\n\
DelayLink set debug_ false\n\
\n\
DynamicLink set status_ 1\n\
DynamicLink set debug_ false\n\
\n\
Filter set debug_ false\n\
Filter/Field set offset_ 0\n\
Filter/Field set match_  -1\n\
\n\
\n\
\n\
Classifier set offset_ 0\n\
Classifier set shift_ 0\n\
Classifier set mask_ 0xffffffff\n\
Classifier set debug_ false\n\
\n\
Classifier/Hash set default_ -1; # none\n\
Classifier/Replicator set ignore_ 0\n\
\n\
Classifier/Addr/MPLS set ttl_   32\n\
Classifier/Addr/MPLS set trace_mpls_ 0\n\
Classifier/Addr/MPLS set label_ -1\n\
Classifier/Addr/MPLS set enable_reroute_    0\n\
Classifier/Addr/MPLS set reroute_option_ 0\n\
Classifier/Addr/MPLS set control_driven_ 0\n\
Classifier/Addr/MPLS set data_driven_ 0\n\
\n\
\n\
FECModel set debug_ false\n\
\n\
\n\
ErrorModule set debug_ false\n\
\n\
ErrorModel set enable_ 1\n\
ErrorModel set markecn_ false\n\
ErrorModel set rate_ 0\n\
ErrorModel set bandwidth_ 2Mb\n\
ErrorModel set debug_ false\n\
\n\
ErrorModel/Trace set good_ 123456789\n\
ErrorModel/Trace set loss_ 0\n\
ErrorModel/Periodic set period_ 1.0\n\
ErrorModel/Periodic set offset_ 0.0\n\
ErrorModel/Periodic set burstlen_ 0.0\n\
ErrorModel/MultiState set curperiod_ 0.0\n\
ErrorModel/MultiState set sttype_ pkt\n\
ErrorModel/MultiState set texpired_ 0\n\
\n\
SelectErrorModel set enable_ 1\n\
SelectErrorModel set markecn_ false\n\
SelectErrorModel set rate_ 0\n\
SelectErrorModel set bandwidth_ 2Mb\n\
SelectErrorModel set pkt_type_ 2\n\
SelectErrorModel set drop_cycle_ 10\n\
SelectErrorModel set drop_offset_ 1\n\
SelectErrorModel set debug_ false\n\
SRMErrorModel set enable_ 1\n\
SRMErrorModel set markecn_ false\n\
SRMErrorModel set rate_ 0\n\
SRMErrorModel set bandwidth_ 2Mb\n\
SRMErrorModel set pkt_type_ 2\n\
SRMErrorModel set drop_cycle_ 10\n\
SRMErrorModel set drop_offset_ 1\n\
SRMErrorModel set debug_ false\n\
\n\
rtModel set startTime_ 0.5\n\
rtModel set finishTime_ \"-\"\n\
rtModel/Exponential set upInterval_   10.0\n\
rtModel/Exponential set downInterval_  1.0\n\
rtModel/Deterministic set upInterval_   2.0\n\
rtModel/Deterministic set downInterval_ 1.0\n\
\n\
\n\
Application/Traffic/CBR_PP set rate_ 448Kb ;# corresponds to interval of 3.75ms\n\
Application/Traffic/CBR_PP set packetSize_ 210\n\
Application/Traffic/CBR_PP set random_ 0\n\
Application/Traffic/CBR_PP set maxpkts_ 268435456; # 0x10000000\n\
Application/Traffic/CBR_PP set PBM_ 2\n\
\n\
Application/Traffic/Exponential set burst_time_ .5\n\
Application/Traffic/Exponential set idle_time_ .5\n\
Application/Traffic/Exponential set rate_ 64Kb\n\
Application/Traffic/Exponential set packetSize_ 210\n\
\n\
Application/Traffic/Pareto set burst_time_ 500ms\n\
Application/Traffic/Pareto set idle_time_ 500ms\n\
Application/Traffic/Pareto set rate_ 64Kb\n\
Application/Traffic/Pareto set packetSize_ 210\n\
Application/Traffic/Pareto set shape_ 1.5\n\
\n\
Application/Traffic/RealAudio set burst_time_ 0.05ms\n\
Application/Traffic/RealAudio set idle_time_ 1800ms\n\
Application/Traffic/RealAudio set rate_ 2Kb\n\
Application/Traffic/RealAudio set packetSize_ 240\n\
Application/Traffic/RealAudio set minCDF_ 0\n\
Application/Traffic/RealAudio set maxCDF_ 1\n\
Application/Traffic/RealAudio set interpolation_ 0\n\
Application/Traffic/RealAudio set maxEntry_ 32\n\
\n\
Application/Traffic/CBR set rate_ 448Kb	;# corresponds to interval of 3.75ms\n\
Application/Traffic/CBR set packetSize_ 210\n\
Application/Traffic/CBR set random_ 0\n\
Application/Traffic/CBR set maxpkts_ 268435456; # 0x10000000\n\
\n\
Application/Telnet set interval_ 1.0\n\
\n\
RandomVariable/Uniform set min_ 0.0\n\
RandomVariable/Uniform set max_ 1.0\n\
RandomVariable/Exponential set avg_ 1.0\n\
RandomVariable/Pareto set avg_ 1.0\n\
RandomVariable/Pareto set shape_ 1.5\n\
RandomVariable/ParetoII set avg_ 10.0\n\
RandomVariable/ParetoII set shape_ 1.2\n\
RandomVariable/Constant set val_ 1.0\n\
RandomVariable/HyperExponential set avg_ 1.0\n\
RandomVariable/HyperExponential set cov_ 4.0\n\
RandomVariable/Empirical set minCDF_ 0\n\
RandomVariable/Empirical set maxCDF_ 1\n\
RandomVariable/Empirical set interpolation_ 0\n\
RandomVariable/Empirical set maxEntry_ 32\n\
RandomVariable/Normal set avg_ 0.0\n\
RandomVariable/Normal set std_ 1.0\n\
RandomVariable/LogNormal set avg_ 1.0\n\
RandomVariable/LogNormal set std_ 1.0\n\
\n\
ADC/MS set debug_ false\n\
ADC/HB set debug_ false\n\
ADC/Param set debug_ false\n\
ADC/ACTP set debug_ false\n\
ADC/ACTO set debug_ false\n\
\n\
Est/Null set debug_ false\n\
Est/TimeWindow set debug_ false\n\
Est/ExpAvg set debug_ false\n\
Est/PointSample set debug_ false\n\
\n\
MeasureMod set debug_ false\n\
SALink set debug_ false\n\
\n\
\n\
Node set multiPath_ 0\n\
Node set rtagent_port_ 255\n\
\n\
Node/MobileNode set X_				0\n\
Node/MobileNode set Y_				0\n\
Node/MobileNode set Z_				0\n\
Node/MobileNode set speed_				0\n\
Node/MobileNode set position_update_interval_	0\n\
Node/MobileNode set bandwidth_			0	;# not used\n\
Node/MobileNode set delay_				0	;# not used\n\
Node/MobileNode set REGAGENT_PORT 0\n\
Node/MobileNode set DECAP_PORT 1\n\
\n\
AddrParams set ALL_BITS_SET 0xffffffff\n\
AddrParams PortShift 0\n\
AddrParams PortMask [AddrParams set ALL_BITS_SET]\n\
AddrParams set domain_num_ 1\n\
AddrParams set def_clusters 4\n\
AddrParams set def_nodes 5\n\
\n\
AllocAddrBits set DEFADDRSIZE_ 32\n\
AllocAddrBits set MAXADDRSIZE_ 32                ;# leaving the signed bit\n\
\n\
Simulator set node_factory_ Node\n\
Simulator set nsv1flag 0\n\
Simulator set mobile_ip_ 0			 ;# flag for mobileIP\n\
\n\
\n\
Simulator set routingAgent_ \"\"\n\
Simulator set addressType_   \"\"\n\
Simulator set MovementTrace_ OFF\n\
Simulator set IMEPFlag_ \"\"\n\
Simulator set WirelessNewTrace_ 0\n\
Simulator set propInstCreated_ 0\n\
\n\
SessionSim set rc_ 0\n\
\n\
Simulator set McastBaseAddr_ 0x80000000\n\
Simulator set McastAddr_ 0x80000000\n\
\n\
Simulator set AgentTrace_ ON\n\
Simulator set RouterTrace_ OFF\n\
Simulator set MacTrace_   OFF\n\
\n\
SessionHelper set rc_ 0                      ;# just to eliminate warnings\n\
SessionHelper set debug_ false\n\
\n\
NetworkInterface set debug_ false\n\
\n\
\n\
\n\
TBF set rate_ 64k\n\
TBF set bucket_ 1024\n\
TBF set qlen_ 0\n\
\n\
MIPEncapsulator set addr_ 0\n\
MIPEncapsulator set port_ 0\n\
MIPEncapsulator set shift_ 0\n\
MIPEncapsulator set mask_ [AddrParams set ALL_BITS_SET]\n\
MIPEncapsulator set ttl_ 32\n\
MIPEncapsulator set debug_ false\n\
\n\
\n\
GAFPartner set addr_ 0\n\
GAFPartner set port_ 254\n\
GAFPartner set shift_ 0\n\
GAFPartner set mask_ [AddrParams set ALL_BITS_SET]\n\
GAFPartner set debug_ false                  \n\
\n\
\n\
\n\
Mac set debug_ false\n\
ARPTable set debug_ false\n\
God set debug_ false\n\
\n\
Mac/Tdma set slot_packet_len_	1500\n\
Mac/Tdma set max_node_num_	64\n\
\n\
LL set mindelay_                50us\n\
LL set delay_                   25us\n\
LL set bandwidth_               0       ;# not used\n\
LL set debug_ false\n\
\n\
\n\
\n\
Classifier/Replicator set direction_ false\n\
Mac set abstract_ false\n\
\n\
Mac/802_3 set trace_ false\n\
\n\
Antenna/OmniAntenna set X_ 0\n\
Antenna/OmniAntenna set Y_ 0\n\
Antenna/OmniAntenna set Z_ 1.5 \n\
Antenna/OmniAntenna set Gt_ 1.0\n\
Antenna/OmniAntenna set Gr_ 1.0\n\
\n\
Phy/WirelessPhy set CPThresh_ 10.0\n\
Phy/WirelessPhy set CSThresh_ 1.559e-11\n\
Phy/WirelessPhy set RXThresh_ 3.652e-10\n\
Phy/WirelessPhy set bandwidth_ 2e6\n\
Phy/WirelessPhy set Pt_ 0.28183815\n\
Phy/WirelessPhy set freq_ 914e+6\n\
Phy/WirelessPhy set L_ 1.0  \n\
Phy/WirelessPhy set debug_ false\n\
\n\
Phy/WiredPhy set bandwidth_ 10e6\n\
Phy/WiredPhy set debug_ false\n\
Phy/Repeater set debug_ false\n\
LanRouter set debug_ false\n\
\n\
Phy/Sat set debug_ false\n\
Mac/Sat set debug_ false\n\
LL/Sat set debug_ false\n\
\n\
Propagation/Shadowing set pathlossExp_ 2.0\n\
Propagation/Shadowing set std_db_ 4.0\n\
Propagation/Shadowing set dist0_ 1.0\n\
Propagation/Shadowing set seed_ 0\n\
\n\
\n\
Agent set fid_ 0\n\
Agent set prio_ 0\n\
Agent set agent_addr_ -1\n\
Agent set agent_port_ -1\n\
Agent set dst_addr_ -1\n\
Agent set dst_port_ -1\n\
Agent set flags_ 0\n\
Agent set ttl_ 32 ; # arbitrary choice here\n\
Agent set debug_ false\n\
Agent set class_ 0\n\
\n\
\n\
Agent/Ping set packetSize_ 64\n\
\n\
Agent/UDP set packetSize_ 1000\n\
Agent/UDP instproc done {} { }\n\
\n\
Agent/TCP set seqno_ 0\n\
Agent/TCP set t_seqno_ 0\n\
Agent/TCP set maxburst_ 0\n\
Agent/TCP set maxcwnd_ 0\n\
Agent/TCP set numdupacks_ 3\n\
Agent/TCP set window_ 20\n\
Agent/TCP set windowInit_ 2 ;		# default changed on 2001/5/26.\n\
Agent/TCP set windowInitOption_ 1\n\
Agent/TCP set syn_ true ;		# default changed on 2001/5/17.\n\
Agent/TCP set windowOption_ 1\n\
Agent/TCP set windowConstant_ 4\n\
Agent/TCP set windowThresh_ 0.002\n\
Agent/TCP set decrease_num_ 0.5\n\
Agent/TCP set increase_num_ 1.0\n\
Agent/TCP set k_parameter_ 0.0 ;	# for binomial congestion control\n\
Agent/TCP set l_parameter_ 1.0 ;  	# for binomial congestion control\n\
Agent/TCP set overhead_ 0\n\
Agent/TCP set ecn_ 0\n\
Agent/TCP set old_ecn_ 0\n\
Agent/TCP set packetSize_ 1000\n\
Agent/TCP set tcpip_base_hdr_size_ 40\n\
Agent/TCP set bugFix_ true\n\
Agent/TCP set timestamps_ false\n\
Agent/TCP set slow_start_restart_ true\n\
Agent/TCP set restart_bugfix_ true\n\
Agent/TCP set tcpTick_ 0.1\n\
Agent/TCP set maxrto_ 100000\n\
Agent/TCP set minrto_ 1 ;		# Default set to 1 on 2001/05/14.\n\
Agent/TCP set srtt_init_ 0\n\
Agent/TCP set rttvar_init_ 12\n\
Agent/TCP set rtxcur_init_ 6.0\n\
Agent/TCP set T_SRTT_BITS 3\n\
Agent/TCP set T_RTTVAR_BITS 2\n\
Agent/TCP set rttvar_exp_ 2\n\
Agent/TCP set timerfix_ true ; 		# Variable added on 2001/05/11\n\
Agent/TCP instproc done {} { }\n\
Agent/TCP set noFastRetrans_ false\n\
\n\
Agent/TCP set dupacks_ 0\n\
Agent/TCP set ack_ 0\n\
Agent/TCP set cwnd_ 0\n\
Agent/TCP set awnd_ 0\n\
Agent/TCP set ssthresh_ 0\n\
Agent/TCP set rtt_ 0\n\
Agent/TCP set srtt_ 0\n\
Agent/TCP set rttvar_ 0\n\
Agent/TCP set backoff_ 0\n\
Agent/TCP set maxseq_ 0\n\
Agent/TCP set singledup_ 0\n\
Agent/TCP set precisionReduce_ false\n\
Agent/TCP set oldCode_ false\n\
\n\
Agent/TCP set ndatapack_ 0\n\
Agent/TCP set ndatabytes_ 0\n\
Agent/TCP set nackpack_ 0\n\
Agent/TCP set nrexmit_ 0\n\
Agent/TCP set nrexmitpack_ 0\n\
Agent/TCP set nrexmitbytes_ 0\n\
Agent/TCP set trace_all_oneline_ false\n\
\n\
Agent/TCP set QOption_ 0 \n\
Agent/TCP set EnblRTTCtr_ 0\n\
Agent/TCP set control_increase_ 0\n\
\n\
Agent/TCP set nam_tracevar_ false\n\
\n\
Agent/TCP/Fack set ss-div4_ false\n\
Agent/TCP/Fack set rampdown_ false\n\
\n\
Agent/TCP set eln_ 0\n\
Agent/TCP set eln_rxmit_thresh_ 1\n\
Agent/TCP set delay_growth_ true ;		# default changed on 2001/5/17.\n\
\n\
Agent/TCP set CoarseTimer_      0\n\
\n\
Agent/TCPSink set sport_        0\n\
Agent/TCPSink set dport_        0         \n\
\n\
Agent/TCPSink set packetSize_ 40\n\
Agent/TCPSink set maxSackBlocks_ 3\n\
Agent/TCPSink set ts_echo_bugfix_ false\n\
Agent/TCPSink set generateDSacks_ false\n\
Agent/TCPSink set RFC2581_immediate_ack_ true\n\
\n\
Agent/TCPSink/DelAck set interval_ 100ms\n\
catch {\n\
Agent/TCPSink/Asym set interval_ 100ms\n\
Agent/TCPSink/Asym set maxdelack_ 5\n\
}\n\
Agent/TCPSink/Sack1/DelAck set interval_ 100ms\n\
\n\
Agent/TCP/Newreno set newreno_changes_ 0\n\
Agent/TCP/Newreno set newreno_changes1_ 0\n\
Agent/TCP/Newreno set partial_window_deflation_ 0\n\
Agent/TCP/Newreno set exit_recovery_fix_ 0\n\
\n\
Agent/TCP/Vegas set v_alpha_ 1\n\
Agent/TCP/Vegas set v_beta_ 3\n\
Agent/TCP/Vegas set v_gamma_ 1\n\
Agent/TCP/Vegas set v_rtt_ 0\n\
\n\
Agent/TCP/Vegas/RBP set rbp_scale_ 0.75\n\
Agent/TCP/Vegas/RBP set rbp_rate_algorithm_ 1\n\
Agent/TCP/Vegas/RBP set rbp_segs_actually_paced_ 0\n\
Agent/TCP/Vegas/RBP set rbp_inter_pace_delay_ 0\n\
\n\
Agent/TCP/Reno/RBP set rbp_scale_ 0.75\n\
Agent/TCP/Reno/RBP set rbp_segs_actually_paced_ 0\n\
Agent/TCP/Reno/RBP set rbp_inter_pace_delay_ 0\n\
\n\
Agent/TCP/Asym set g_ 0.125\n\
Agent/TCP/Reno/Asym set g_ 0.125\n\
Agent/TCP/Newreno/Asym set g_ 0.125\n\
\n\
Agent/TCP/RFC793edu set add793expbackoff_  true \n\
Agent/TCP/RFC793edu set add793jacobsonrtt_ false\n\
Agent/TCP/RFC793edu set add793fastrtx_     false\n\
Agent/TCP/RFC793edu set add793slowstart_   false\n\
Agent/TCP/RFC793edu set add793additiveinc_ false\n\
Agent/TCP/RFC793edu set add793karnrtt_     true \n\
Agent/TCP/RFC793edu set rto_               60\n\
Agent/TCP/RFC793edu set syn_               true\n\
Agent/TCP/RFC793edu set add793exponinc_    false\n\
\n\
Agent/TFRC set packetSize_ 1000 \n\
Agent/TFRC set df_ 0.95 ;	# decay factor for accurate RTT estimate\n\
Agent/TFRC set tcp_tick_ 0.1 ;	\n\
Agent/TFRC set ndatapack_ 0 ;	# Number of packets sent\n\
Agent/TFRC set srtt_init_ 0 ;	# Variables for tracking RTT	\n\
Agent/TFRC set rttvar_init_ 12  \n\
Agent/TFRC set rtxcur_init_ 6.0	\n\
Agent/TFRC set rttvar_exp_ 2	\n\
Agent/TFRC set T_SRTT_BITS 3	\n\
Agent/TFRC set T_RTTVAR_BITS 2	\n\
Agent/TFRC set InitRate_ 300 ;	# Initial send rate	\n\
Agent/TFRC set overhead_ 0 ;	# If > 0, dither outgoing packets\n\
Agent/TFRC set ssmult_ 2 ; 	# Rate of increase during slow-start:\n\
Agent/TFRC set bval_ 1 ;	# Value of B for TCP formula\n\
Agent/TFRC set ca_ 1 ; 	 	# Enable Sqrt(RTT) congestion avoidance\n\
Agent/TFRC set printStatus_ 0 \n\
Agent/TFRC set maxHeavyRounds_ 1; # Number of rounds for sending rate allowed\n\
Agent/TFRC set conservative_ 0 ;  # Set to true for a conservative \n\
Agent/TFRC set ecn_ 0 ;		# Set to 1 for ECN-capable connection.\n\
Agent/TFRC set SndrType_ 0 ;    # Set to 1 to use data-producing applications\n\
\n\
Agent/TFRCSink set packetSize_ 40\n\
Agent/TFRCSink set InitHistorySize_ 100000\n\
Agent/TFRCSink set NumFeedback_ 1 \n\
Agent/TFRCSink set AdjustHistoryAfterSS_ 1\n\
Agent/TFRCSink set NumSamples_ -1\n\
Agent/TFRCSink set discount_ 1;	# History Discounting\n\
Agent/TFRCSink set printLoss_ 0\n\
Agent/TFRCSink set smooth_ 1 ;	# smoother Average Loss Interval\n\
Agent/TFRCSink set minlc_ 4\n\
Agent/TFRCSink set algo_ 1 ;  	# 1: algo from sigcomm paper 2: ewma \n\
Agent/TFRCSink set maxint_ 1000 ;     # max loss interval history \n\
Agent/TFRCSink set history_ 0.75 ;    # loss history for EWMA\n\
\n\
if [TclObject is-class Agent/TCP/FullTcp] {\n\
Agent/TCP/FullTcp set segsperack_ 1; # ACK frequency\n\
Agent/TCP/FullTcp set segsize_ 536; # segment size\n\
Agent/TCP/FullTcp set tcprexmtthresh_ 3; # num dupacks to enter recov\n\
Agent/TCP/FullTcp set iss_ 0; # Initial send seq#\n\
Agent/TCP/FullTcp set nodelay_ false; # Nagle disable?\n\
Agent/TCP/FullTcp set data_on_syn_ false; # allow data on 1st SYN?\n\
Agent/TCP/FullTcp set dupseg_fix_ true ; # no rexmt w/dup segs from peer\n\
Agent/TCP/FullTcp set dupack_reset_ false; # exit recov on ack < highest\n\
Agent/TCP/FullTcp set interval_ 0.1 ; # delayed ACK interval 100ms \n\
Agent/TCP/FullTcp set close_on_empty_ false; # close conn if sent all\n\
Agent/TCP/FullTcp set ts_option_size_ 10; # in bytes\n\
Agent/TCP/FullTcp set reno_fastrecov_ true; # fast recov true by default\n\
Agent/TCP/FullTcp set pipectrl_ false; # use \"pipe\" ctrl\n\
Agent/TCP/FullTcp set open_cwnd_on_pack_ true; # ^ win on partial acks?\n\
Agent/TCP/FullTcp set halfclose_ false; # do simplex closes (shutdown)?\n\
\n\
Agent/TCP/FullTcp/Newreno set recov_maxburst_ 2; # max burst dur recov\n\
\n\
Agent/TCP/FullTcp/Sack set sack_block_size_ 8; # bytes in a SACK block\n\
Agent/TCP/FullTcp/Sack set sack_option_size_ 2; # bytes in opt hdr\n\
Agent/TCP/FullTcp/Sack set max_sack_blocks_ 3; # max # of sack blks\n\
}\n\
\n\
if [TclObject is-class Agent/TCP/BayFullTcp] {\n\
Agent/TCP/BayFullTcp set segsperack_ 1; # ACK frequency\n\
Agent/TCP/BayFullTcp set segsize_ 536; # segment size\n\
Agent/TCP/BayFullTcp set tcprexmtthresh_ 3; # num dupacks to enter recov\n\
Agent/TCP/BayFullTcp set iss_ 0; # Initial send seq#\n\
Agent/TCP/BayFullTcp set nodelay_ false; # Nagle disable?\n\
Agent/TCP/BayFullTcp set data_on_syn_ false; # allow data on 1st SYN?\n\
Agent/TCP/BayFullTcp set dupseg_fix_ true ; # no rexmt w/dup segs from peer\n\
Agent/TCP/BayFullTcp set dupack_reset_ false; # exit recov on ack < highest\n\
Agent/TCP/BayFullTcp set interval_ 0.1 ; # delayed ACK interval 100ms \n\
Agent/TCP/BayFullTcp set close_on_empty_ false; # close conn if sent all\n\
}\n\
\n\
Agent/Null set sport_           0\n\
Agent/Null set dport_           0\n\
\n\
Agent/CBR set sport_            0\n\
Agent/CBR set dport_            0\n\
\n\
\n\
Agent/RTP set seqno_ 0\n\
Agent/RTP set interval_ 3.75ms\n\
Agent/RTP set random_ 0\n\
Agent/RTP set packetSize_ 210\n\
Agent/RTP set maxpkts_ 0x10000000\n\
Agent/RTP instproc done {} { }\n\
\n\
Agent/RTCP set seqno_ 0\n\
\n\
Agent/Message set packetSize_ 180\n\
\n\
Agent/LossMonitor set nlost_ 0\n\
Agent/LossMonitor set npkts_ 0\n\
Agent/LossMonitor set bytes_ 0\n\
Agent/LossMonitor set lastPktTime_ 0\n\
Agent/LossMonitor set expected_ 0\n\
\n\
Agent/RAP set packetSize_ 512\n\
Agent/RAP set seqno_ 0\n\
Agent/RAP set sessionLossCount_ 0\n\
Agent/RAP set ipg_ 2.0\n\
Agent/RAP set alpha_ 1.0\n\
Agent/RAP set beta_ 0.5\n\
Agent/RAP set srtt_ 2.0\n\
Agent/RAP set variance_ 0.0\n\
Agent/RAP set delta_ 0.5\n\
Agent/RAP set mu_ 1.2\n\
Agent/RAP set phi_ 4.0\n\
Agent/RAP set timeout_ 2.0\n\
Agent/RAP set overhead_ 0\n\
Agent/RAP set useFineGrain_ 0\n\
Agent/RAP set kfrtt_ 0.9\n\
Agent/RAP set kxrtt_ 0.01\n\
Agent/RAP set debugEnable_ 0\n\
Agent/RAP set rap_base_hdr_size_ 44\n\
Agent/RAP set dpthresh_ 50\n\
Agent/RAP instproc done {} { }\n\
\n\
Agent/Mcast/Control set packetSize_ 80\n\
\n\
Agent/rtProto set preference_ 200		;# global default preference\n\
Agent/rtProto/Direct set preference_ 100\n\
Agent/rtProto/DV set preference_	120\n\
Agent/rtProto/DV set INFINITY		 [Agent set ttl_]\n\
Agent/rtProto/DV set advertInterval	  2\n\
\n\
Agent/Encapsulator set status_ 1\n\
Agent/Encapsulator set overhead_ 20\n\
\n\
Agent/DSRAgent set sport_ 255\n\
Agent/DSRAgent set dport_ 255\n\
\n\
Agent/MIPBS set adSize_ 48\n\
Agent/MIPBS set shift_ 0\n\
Agent/MIPBS set mask_ [AddrParams set ALL_BITS_SET]\n\
Agent/MIPBS set ad_lifetime_ 2\n\
\n\
Agent/MIPMH set home_agent_ 0\n\
Agent/MIPMH set rreqSize_ 52\n\
Agent/MIPMH set reg_rtx_ 0.5\n\
Agent/MIPMH set shift_ 0\n\
Agent/MIPMH set mask_ [AddrParams set ALL_BITS_SET]\n\
Agent/MIPMH set reg_lifetime_ 2\n\
\n\
Agent/Diff_Sink set packetSize_ 512\n\
Agent/Diff_Sink set interval_   0.5\n\
Agent/Diff_Sink set random_     1\n\
Agent/Diff_Sink set maxpkts_    10000\n\
Agent/Diff_Sink set data_type_  0\n\
\n\
Agent/LossMonitor/PLM set flag_PP_ 0\n\
Agent/LossMonitor/PLM set packet_time_PP_ 0\n\
Agent/LossMonitor/PLM set fid_PP_ 0\n\
Agent/LossMonitor/PLM set seqno_ 0\n\
\n\
Agent/LDP set trace_ldp_ 0\n\
\n\
Simulator set nix-routing 0\n\
\n\
RtModule set classifier_ \"\"\n\
RtModule/Base set classifier_ \"\"\n\
if [TclObject is-class Network/Pcap/Live] {\n\
Network/Pcap/Live set snaplen_ 4096;# bpf snap len\n\
Network/Pcap/Live set promisc_ false;\n\
Network/Pcap/Live set timeout_ 0\n\
Network/Pcap/Live set optimize_ true;# bpf code optimizer\n\
Network/Pcap/Live set offset_ 0.0; # \n\
\n\
Network/Pcap/File set offset_ 0.0; # ts for 1st pkt in trace file\n\
}\n\
\n\
if [TclObject is-class Agent/Tap] {\n\
Agent/Tap set maxpkt_ 1600\n\
}\n\
\n\
if [TclObject is-class Agent/TCPTap] {\n\
Agent/TCPTap set maxpkt_ 1600\n\
}\n\
\n\
if [TclObject is-class Agent/IcmpAgent] {\n\
Agent/IcmpAgent set ttl_ 254\n\
}\n\
\n\
if [TclObject is-class Agent/IPTap] {\n\
Agent/IPTap set maxpkt_ 1600\n\
}\n\
\n\
if [TclObject is-class ArpAgent] {\n\
\n\
ArpAgent set cachesize_ 10; # entries in arp cache\n\
ArpAgent instproc init {} {\n\
$self next\n\
}\n\
\n\
ArpAgent instproc config ifname {\n\
$self instvar net_ myether_ myip_\n\
set net_ [new Network/Pcap/Live]\n\
$net_ open readwrite $ifname\n\
set myether_ [$net_ linkaddr]\n\
set myip_ [$net_ netaddr]\n\
$net_ filter \"arp and (not ether src $myether_) and \\\n\
(ether dst $myether_ \\\n\
or ether dst ff:ff:ff:ff:ff:ff)\"\n\
$self cmd network $net_\n\
$self cmd myether $myether_\n\
$self cmd myip $myip_\n\
}\n\
}\n\
\n\
\n\
\n\
\n\
\n\
\n\
Node instproc add-pushback-agent {} {\n\
$self instvar pushback_\n\
set pushback_ [new Agent/Pushback]\n\
[Simulator instance] attach-agent $self $pushback_\n\
$pushback_ initialize $self [[Simulator instance] get-routelogic]\n\
return $pushback_\n\
}\n\
\n\
Node instproc get-pushback-agent {} {\n\
$self instvar pushback_\n\
if [info exists pushback_] {\n\
return $pushback_\n\
} else {\n\
return -1\n\
}\n\
}\n\
\n\
Simulator instproc pushback-duplex-link {n1 n2 bw delay} {\n\
\n\
$self pushback-simplex-link $n1 $n2 $bw $delay\n\
$self pushback-simplex-link $n2 $n1 $bw $delay\n\
}\n\
\n\
Simulator instproc pushback-simplex-link {n1 n2 bw delay} {\n\
\n\
set pba [$n1 get-pushback-agent]\n\
if {$pba == -1} {\n\
puts \"Node does not have a pushback agent\"\n\
exit\n\
}\n\
$self simplex-link $n1 $n2 $bw $delay RED/Pushback $pba\n\
\n\
set link [$self link $n1 $n2]\n\
set qmon [new QueueMonitor/ED]\n\
$link attach-monitors [new SnoopQueue/In] [new SnoopQueue/Out] [new SnoopQueue/Drop] $qmon\n\
\n\
set queue [$link queue]\n\
$queue set pushbackID_ [$pba add-queue $queue]\n\
$queue set-monitor $qmon\n\
$queue set-src-dst [$n1 set id_] [$n2 set id_]\n\
\n\
}\n\
\n\
Agent/Pushback instproc get-pba-port {nodeid} {\n\
\n\
set node [[Simulator instance] set Node_($nodeid)]\n\
set pba [$node get-pushback-agent]\n\
if {$pba == -1} {\n\
return -1\n\
} else {\n\
return [$pba set agent_port_]\n\
}\n\
}\n\
\n\
Agent/Pushback instproc check-queue { src dst qToCheck } {\n\
\n\
set link [[Simulator instance] set link_($src:$dst)]\n\
set queue [$link queue]\n\
if {$qToCheck == $queue} {\n\
return 1\n\
} else {\n\
return 0\n\
}\n\
}\n\
\n\
Queue/RED/Pushback set pushbackID_ -1\n\
Queue/RED/Pushback set rate_limiting_ 1\n\
\n\
Agent/Pushback set last_index_ 0\n\
Agent/Pushback set intResult_ -1\n\
Agent/Pushback set enable_pushback_ 1\n\
Agent/Pushback set verbose_ false\n\
\n\
Queue/RED/Pushback instproc attach-traces {src dst file {op \"\"}} {\n\
\n\
$self next $src $dst $file $op\n\
\n\
set ns [Simulator instance]\n\
\n\
\n\
\n\
set type \"Drop\"\n\
set rldrop_trace [$ns create-trace $type $file $src $dst $op]\n\
\n\
set oldTrace [$self rldrop-trace]\n\
if {$oldTrace!=0} {\n\
puts \"exists\"\n\
$rldrop_trace target $oldTrace\n\
} else {\n\
$rldrop_trace target [$ns set nullAgent_]\n\
}\n\
\n\
$self rldrop-trace $rldrop_trace\n\
\n\
}\n\
\n\
\n\
\n\
Simulator instproc init args {\n\
\n\
\n\
$self create_packetformat\n\
$self use-scheduler Calendar\n\
$self set nullAgent_ [new Agent/Null]\n\
$self set-address-format def\n\
if {[lindex $args 0] == \"-multicast\"} {\n\
$self multicast $args\n\
}\n\
eval $self next $args\n\
}\n\
\n\
Simulator instproc nullagent {} {\n\
$self instvar nullAgent_\n\
return $nullAgent_\n\
}\n\
\n\
Simulator instproc use-scheduler type {\n\
$self instvar scheduler_\n\
if [info exists scheduler_] {\n\
if { [$scheduler_ info class] == \"Scheduler/$type\" } {\n\
return\n\
} else {\n\
delete $scheduler_\n\
}\n\
}\n\
set scheduler_ [new Scheduler/$type]\n\
$scheduler_ now\n\
}\n\
\n\
Simulator instproc delay_parse { spec } {\n\
return [time_parse $spec]\n\
}\n\
\n\
Simulator instproc bw_parse { spec } {\n\
return [bw_parse $spec]\n\
}\n\
\n\
Simulator instproc dumper obj {\n\
set t [$self alloc-trace hop stdout]\n\
$t target $obj\n\
return $t\n\
}\n\
\n\
\n\
Simulator instproc addressType  {val} { $self set addressType_  $val }\n\
Simulator instproc adhocRouting  {val} { $self set routingAgent_  $val }\n\
Simulator instproc llType  {val} { $self set llType_  $val }\n\
Simulator instproc macType  {val} { $self set macType_  $val }\n\
Simulator instproc propType  {val} { $self set propType_  $val }\n\
Simulator instproc propInstance  {val} { $self set propInstance_  $val }\n\
Simulator instproc ifqType  {val} { $self set ifqType_  $val }\n\
Simulator instproc ifqLen  {val} { $self set ifqlen_  $val }\n\
Simulator instproc phyType  {val} { $self set phyType_  $val }\n\
Simulator instproc antType  {val} { $self set antType_  $val }\n\
Simulator instproc channel {val} {$self set channel_ $val}\n\
Simulator instproc channelType {val} {$self set channelType_ $val}\n\
Simulator instproc topoInstance {val} {$self set topoInstance_ $val}\n\
Simulator instproc wiredRouting {val} {$self set wiredRouting_ $val}\n\
Simulator instproc mobileIP {val} {$self set mobileIP_ $val}\n\
Simulator instproc energyModel  {val} { $self set energyModel_  $val }\n\
Simulator instproc initialEnergy  {val} { $self set initialEnergy_  $val }\n\
Simulator instproc txPower  {val} { $self set txPower_  $val }\n\
Simulator instproc rxPower  {val} { $self set rxPower_  $val }\n\
Simulator instproc idlePower  {val} { $self set idlePower_  $val }\n\
Simulator instproc errProc  {val} { $self set errProc_  $val }\n\
Simulator instproc FECProc  {val} { $self set FECProc_  $val }\n\
Simulator instproc agentTrace  {val} { $self set agentTrace_  $val }\n\
Simulator instproc routerTrace  {val} { $self set routerTrace_  $val }\n\
Simulator instproc macTrace  {val} { $self set macTrace_  $val }\n\
Simulator instproc movementTrace  {val} { $self set movementTrace_  $val }\n\
Simulator instproc toraDebug {val} {$self set toraDebug_ $val }\n\
Simulator instproc MPLS { val } { \n\
if { $val == \"ON\" } {\n\
Node enable-module \"MPLS\"\n\
} else {\n\
Node disable-module \"MPLS\"\n\
}\n\
}\n\
\n\
Simulator instproc get-nodetype {} {\n\
$self instvar addressType_ routingAgent_ wiredRouting_ \n\
set val \"\"\n\
\n\
if { [info exists addressType_] && $addressType_ == \"hierarchical\" } {\n\
set val Hier\n\
}\n\
if { [info exists routingAgent_] && $routingAgent_ != \"\" } {\n\
set val Mobile\n\
}\n\
if { [info exists wiredRouting_] && $wiredRouting_ == \"ON\" } {\n\
set val Base\n\
}\n\
if { [info exists wiredRouting_] && $wiredRouting_ == \"OFF\"} {\n\
set val Base\n\
}\n\
if { [Simulator set mobile_ip_] } {\n\
if { $val == \"Base\" && $wiredRouting_ == \"ON\" } {\n\
set val MIPBS\n\
}\n\
if { $val == \"Base\" && $wiredRouting_ == \"OFF\" } {\n\
set val MIPMH\n\
}\n\
}\n\
return $val\n\
}\n\
\n\
Simulator instproc node-config args {\n\
set args [eval $self init-vars $args]\n\
\n\
$self instvar addressType_  routingAgent_ propType_  macTrace_ \\\n\
routerTrace_ agentTrace_ movementTrace_ channelType_ channel_ \\\n\
chan topoInstance_ propInstance_ mobileIP_ rxPower_ \\\n\
txPower_ idlePower_\n\
\n\
if [info exists macTrace_] {\n\
Simulator set MacTrace_ $macTrace_\n\
}\n\
if [info exists routerTrace_] {\n\
Simulator set RouterTrace_ $routerTrace_\n\
}\n\
if [info exists agentTrace_] {\n\
Simulator set AgentTrace_ $agentTrace_\n\
}\n\
if [info exists movementTrace_] {\n\
Simulator set MovementTrace_ $movementTrace_\n\
}\n\
if {[info exists propInstance_]} {\n\
if {[info exists propType_] && [Simulator set propInstCreated_] == 0} {\n\
warn \"Both propType and propInstance are set. propType is ignored.\"\n\
}\n\
} else {\n\
if {[info exists propType_]} {\n\
set propInstance_ [new $propType_]\n\
Simulator set propInstCreated_ 1\n\
}\n\
}\n\
\n\
if {[info exists channelType_] && [info exists channel_]} { \n\
error \"Can't specify both channel and channelType, error!\"\n\
} elseif {[info exists channelType_]} {\n\
warn \"Please use -channel as shown in tcl/ex/wireless-mitf.tcl\"\n\
if {![info exists chan]} {\n\
set chan [new $channelType_]\n\
}\n\
} elseif {[info exists channel_]} {\n\
set chan $channel_\n\
}\n\
if [info exists topoInstance_] {\n\
$propInstance_  topography $topoInstance_\n\
}\n\
if {[string compare $addressType_ \"\"] != 0} {\n\
$self set-address-format $addressType_ \n\
}\n\
if { [info exists mobileIP_] && $mobileIP_ == \"ON\"} {\n\
Simulator set mobile_ip_  1\n\
} else {\n\
if { [info exists mobileIP_] } {\n\
Simulator set mobile_ip_ 0\n\
}\n\
}\n\
}\n\
\n\
Simulator instproc node args {\n\
$self instvar Node_ routingAgent_ wiredRouting_\n\
if { [Simulator info vars EnableMcast_] != \"\" } {\n\
warn \"Flag variable Simulator::EnableMcast_ discontinued.\\n\\t\\\n\
Use multicast methods as:\\n\\t\\t\\\n\
% set ns \\[new Simulator -multicast on]\\n\\t\\t\\\n\
% \\$ns multicast\"\n\
$self multicast\n\
Simulator unset EnableMcast_\n\
}\n\
if { [Simulator info vars NumberInterfaces_] != \"\" } {\n\
warn \"Flag variable Simulator::NumberInterfaces_ discontinued.\\n\\t\\\n\
Setting this variable will not affect simulations.\"\n\
Simulator unset NumberInterfaces_\n\
}\n\
\n\
if { [info exists routingAgent_] && ($routingAgent_ != \"\") } {\n\
set node [eval $self create-wireless-node $args]\n\
if {[info exists wiredRouting_] && $wiredRouting_ == \"ON\"} {\n\
set Node_([$node id]) $node\n\
$self add-node $node [$node id] \n\
}\n\
return $node\n\
}\n\
\n\
set node [eval new [Simulator set node_factory_] $args]\n\
set Node_([$node id]) $node\n\
\n\
$self add-node $node [$node id] \n\
\n\
$node nodeid [$node id]\n\
\n\
$node set ns_ $self\n\
$self check-node-num\n\
return $node\n\
}\n\
\n\
Simulator instproc imep-support {} {\n\
return [Simulator set IMEPFlag_]\n\
}\n\
\n\
Simulator instproc create-wireless-node args {\n\
$self instvar routingAgent_ wiredRouting_ propInstance_ llType_ \\\n\
macType_ ifqType_ ifqlen_ phyType_ chan antType_ energyModel_ \\\n\
initialEnergy_ txPower_ rxPower_ idlePower_ \\\n\
topoInstance_ level1_ level2_ errProc_ FECProc_\n\
\n\
Simulator set IMEPFlag_ OFF\n\
\n\
set node [eval $self create-node-instance $args]\n\
\n\
if { [info exist wiredRouting_] && $wiredRouting_ == \"ON\" } {\n\
$node base-station [AddrParams addr2id [$node node-addr]]\n\
}\n\
switch -exact $routingAgent_ {\n\
DSDV {\n\
set ragent [$self create-dsdv-agent $node]\n\
}\n\
DSR {\n\
$self at 0.0 \"$node start-dsr\"\n\
}\n\
AODV {\n\
set ragent [$self create-aodv-agent $node]\n\
}\n\
TORA {\n\
Simulator set IMEPFlag_ ON\n\
set ragent [$self create-tora-agent $node]\n\
}\n\
DIFFUSION/RATE {\n\
eval $node addr $args\n\
set ragent [$self create-diffusion-rate-agent $node]\n\
}\n\
DIFFUSION/PROB {\n\
eval $node addr $args\n\
set ragent [$self create-diffusion-probability-agent $node]\n\
}\n\
FLOODING {\n\
eval $node addr $args\n\
set ragent [$self create-flooding-agent $node]\n\
}\n\
OMNIMCAST {\n\
eval $node addr $args\n\
set ragent [$self create-omnimcast-agent $node]\n\
}\n\
default {\n\
puts \"Wrong node routing agent!\"\n\
exit\n\
}\n\
}\n\
\n\
if ![info exist errProc_] {\n\
set errProc_ \"\"\n\
}\n\
if ![info exist FECProc_] {\n\
set FECProc_ \"\"\n\
}\n\
\n\
$node add-interface $chan $propInstance_ $llType_ $macType_ \\\n\
$ifqType_ $ifqlen_ $phyType_ $antType_ $errProc_ $FECProc_\n\
if {$routingAgent_ != \"DSR\"} {\n\
$node attach $ragent [Node set rtagent_port_]\n\
}\n\
if {$routingAgent_ == \"DIFFUSION/RATE\" ||\n\
$routingAgent_ == \"DIFFUSION/PROB\" ||\n\
$routingAgent_ == \"FLOODING\" ||\n\
$routingAgent_ == \"OMNIMCAST\" } {\n\
$ragent port-dmux [$node demux]\n\
$node instvar ll_\n\
$ragent add-ll $ll_(0)\n\
}\n\
\n\
if { [info exist wiredRouting_] && $wiredRouting_ == \"ON\" } {\n\
if { $routingAgent_ != \"DSR\" } {\n\
$node mip-call $ragent\n\
}\n\
}\n\
set tracefd [$self get-ns-traceall]\n\
if {$tracefd != \"\" } {\n\
$node nodetrace $tracefd\n\
$node agenttrace $tracefd\n\
}\n\
set namtracefd [$self get-nam-traceall]\n\
if {$namtracefd != \"\" } {\n\
$node namattach $namtracefd\n\
}\n\
if [info exists energyModel_] {\n\
if  [info exists level1_] {\n\
set l1 $level1_\n\
} else {\n\
set l1 0.5\n\
}\n\
if  [info exists level2_] {\n\
set l2 $level2_\n\
} else {\n\
set l2 0.2\n\
}\n\
$node addenergymodel [new $energyModel_ $node \\\n\
$initialEnergy_ $l1 $l2]\n\
}\n\
if [info exists txPower_] {\n\
$node setPt $txPower_\n\
}\n\
if [info exists rxPower_] {\n\
$node setPr $rxPower_\n\
}\n\
if [info exists idlePower_] {\n\
$node setPidle $idlePower_\n\
}\n\
$node topography $topoInstance_\n\
return $node\n\
}\n\
\n\
Simulator instproc create-node-instance args {\n\
$self instvar routingAgent_\n\
if {$routingAgent_ == \"DSR\"} {\n\
set nodeclass [$self set-dsr-nodetype]\n\
} else {\n\
set nodeclass Node/MobileNode\n\
}\n\
return [eval new $nodeclass $args]\n\
}\n\
\n\
Simulator instproc set-dsr-nodetype {} {\n\
$self instvar wiredRouting_ \n\
set nodetype SRNodeNew\n\
if [Simulator set mobile_ip_] {\n\
set nodetype SRNodeNew/MIPMH\n\
} \n\
if { [info exists wiredRouting_] && $wiredRouting_ == \"ON\"} {\n\
set nodetype Node/MobileNode/BaseStationNode\n\
}\n\
return $nodetype\n\
}\n\
\n\
Simulator instproc create-tora-agent { node } {\n\
set ragent [new Agent/TORA [$node id]]\n\
$node set ragent_ $ragent\n\
return $ragent\n\
}\n\
\n\
Simulator instproc create-dsdv-agent { node } {\n\
set ragent [new Agent/DSDV]\n\
set addr [$node node-addr]\n\
$ragent addr $addr\n\
$ragent node $node\n\
if [Simulator set mobile_ip_] {\n\
$ragent port-dmux [$node demux]\n\
}\n\
$node addr $addr\n\
$node set ragent_ $ragent\n\
$self at 0.0 \"$ragent start-dsdv\"    ;# start updates\n\
return $ragent\n\
}\n\
\n\
Simulator instproc create-aodv-agent { node } {\n\
set ragent [new Agent/AODV [$node id]]\n\
$self at 0.0 \"$ragent start\"     ;# start BEACON/HELLO Messages\n\
$node set ragent_ $ragent\n\
return $ragent\n\
}\n\
\n\
Simulator instproc use-newtrace {} {\n\
Simulator set WirelessNewTrace_ 1\n\
} \n\
\n\
Simulator instproc hier-node haddr {\n\
error \"hier-nodes should be created with [$ns_ node $haddr]\"\n\
}\n\
\n\
Simulator instproc now {} {\n\
$self instvar scheduler_\n\
return [$scheduler_ now]\n\
}\n\
\n\
Simulator instproc at args {\n\
$self instvar scheduler_\n\
return [eval $scheduler_ at $args]\n\
}\n\
\n\
Simulator instproc at-now args {\n\
$self instvar scheduler_\n\
return [eval $scheduler_ at-now $args]\n\
}\n\
\n\
Simulator instproc cancel args {\n\
$self instvar scheduler_\n\
return [eval $scheduler_ cancel $args]\n\
}\n\
\n\
Simulator instproc after {ival args} {\n\
eval $self at [expr [$self now] + $ival] $args\n\
}\n\
\n\
Simulator instproc check-node-num {} {\n\
if {[Node set nn_] > [expr pow(2, [AddrParams nodebits])]} {\n\
error \"Number of nodes exceeds node-field-size of [AddrParams nodebits] bits\"\n\
}\n\
}\n\
\n\
Simulator instproc chk-hier-field-lengths {} {\n\
AddrParams instvar domain_num_ cluster_num_ nodes_num_\n\
if [info exists domain_num_] {\n\
if {[expr $domain_num_ - 1]> [AddrParams NodeMask 1]} {\n\
error \"\\# of domains exceed dom-field-size \"\n\
}\n\
} \n\
if [info exists cluster_num_] {\n\
set maxval [expr [find-max $cluster_num_] - 1] \n\
if {$maxval > [expr pow(2, [AddrParams NodeMask 2])]} {\n\
error \"\\# of clusters exceed clus-field-size \"\n\
}\n\
}\n\
if [info exists nodes_num_] {\n\
set maxval [expr [find-max $nodes_num_] -1]\n\
if {$maxval > [expr pow(2, [AddrParams NodeMask 3])]} {\n\
error \"\\# of nodess exceed node-field-size\"\n\
}\n\
}\n\
}\n\
\n\
Simulator instproc run {} {\n\
$self check-node-num\n\
$self rtmodel-configure			;# in case there are any\n\
[$self get-routelogic] configure\n\
$self instvar scheduler_ Node_ link_ started_ \n\
\n\
set started_ 1\n\
\n\
\n\
foreach nn [array names Node_] {\n\
$Node_($nn) reset\n\
if { [Simulator set nix-routing] } {\n\
$Node_($nn) populate-objects\n\
}\n\
}\n\
\n\
\n\
foreach qn [array names link_] {\n\
set q [$link_($qn) queue]\n\
$q reset\n\
}\n\
\n\
$self init-nam\n\
\n\
return [$scheduler_ run]\n\
}\n\
\n\
Simulator instproc log-simstart { } {\n\
global simstart\n\
puts \"Starting Actual Simulation\"\n\
set simstart [clock seconds]\n\
}\n\
\n\
Simulator instproc halt {} {\n\
$self instvar scheduler_\n\
$scheduler_ halt\n\
}\n\
\n\
Simulator instproc dumpq {} {\n\
$self instvar scheduler_\n\
$scheduler_ dumpq\n\
}\n\
\n\
Simulator instproc is-started {} {\n\
$self instvar started_\n\
return [info exists started_]\n\
}\n\
\n\
Simulator instproc clearMemTrace {} {\n\
$self instvar scheduler_\n\
$scheduler_ clearMemTrace\n\
}\n\
\n\
Simulator instproc simplex-link { n1 n2 bw delay qtype args } {\n\
$self instvar link_ queueMap_ nullAgent_\n\
set sid [$n1 id]\n\
set did [$n2 id]\n\
\n\
global slink_\n\
set slink_($sid:$did) $self\n\
\n\
if [info exists queueMap_($qtype)] {\n\
set qtype $queueMap_($qtype)\n\
}\n\
set qtypeOrig $qtype\n\
switch -exact $qtype {\n\
ErrorModule {\n\
if { [llength $args] > 0 } {\n\
set q [eval new $qtype $args]\n\
} else {\n\
set q [new $qtype Fid]\n\
}\n\
}\n\
intserv {\n\
set qtype [lindex $args 0]\n\
set q [new Queue/$qtype]\n\
}\n\
default {\n\
if { [llength $args] == 0} {\n\
set q [new Queue/$qtype]\n\
} else {\n\
set q [new Queue/$qtype $args]\n\
}\n\
}\n\
}\n\
switch -exact $qtypeOrig {\n\
RTM {\n\
set c [lindex $args 1]\n\
set link_($sid:$did) [new CBQLink       \\\n\
$n1 $n2 $bw $delay $q $c]\n\
}\n\
CBQ -\n\
CBQ/WRR {\n\
if {[llength $args] == 0} {\n\
set c [new Classifier/Hash/Fid 33]\n\
} else {\n\
set c [lindex $args 0]\n\
}\n\
set link_($sid:$did) [new CBQLink       \\\n\
$n1 $n2 $bw $delay $q $c]\n\
}\n\
FQ      {\n\
set link_($sid:$did) [new FQLink $n1 $n2 $bw $delay $q]\n\
}\n\
intserv {\n\
set link_($sid:$did) [new IntServLink   \\\n\
$n1 $n2 $bw $delay $q	\\\n\
[concat $qtypeOrig $args]]\n\
}\n\
default {\n\
set link_($sid:$did) [new SimpleLink    \\\n\
$n1 $n2 $bw $delay $q]\n\
}\n\
}\n\
if {$qtype == \"RED/Pushback\"} {\n\
set pushback 1\n\
} else {\n\
set pushback 0\n\
}\n\
$n1 add-neighbor $n2 $pushback\n\
\n\
if {[string first \"RED\" $qtype] != -1} {\n\
$q link [$link_($sid:$did) set link_]\n\
}\n\
\n\
if {[string first \"RIO\" $qtype] != -1} {\n\
$q link [$link_($sid:$did) set link_]\n\
}\n\
\n\
set trace [$self get-ns-traceall]\n\
if {$trace != \"\"} {\n\
$self trace-queue $n1 $n2 $trace\n\
}\n\
set trace [$self get-nam-traceall]\n\
if {$trace != \"\"} {\n\
$self namtrace-queue $n1 $n2 $trace\n\
}\n\
\n\
$self register-nam-linkconfig $link_($sid:$did)\n\
}\n\
\n\
Simulator instproc register-nam-linkconfig link {\n\
$self instvar linkConfigList_ link_\n\
if [info exists linkConfigList_] {\n\
set i1 [[$link src] id]\n\
set i2 [[$link dst] id]\n\
if [info exists link_($i2:$i1)] {\n\
set pos [lsearch $linkConfigList_ $link_($i2:$i1)]\n\
if {$pos >= 0} {\n\
set a1 [$link_($i2:$i1) get-attribute \\\n\
\"ORIENTATION\"]\n\
set a2 [$link get-attribute \"ORIENTATION\"]\n\
if {$a1 == \"\" && $a2 != \"\"} {\n\
set linkConfigList_ [lreplace \\\n\
$linkConfigList_ $pos $pos]\n\
} else {\n\
return\n\
}\n\
}\n\
}\n\
set pos [lsearch $linkConfigList_ $link]\n\
if {$pos >= 0} {\n\
set linkConfigList_ \\\n\
[lreplace $linkConfigList_ $pos $pos]\n\
}\n\
}\n\
lappend linkConfigList_ $link\n\
}\n\
\n\
Simulator instproc remove-nam-linkconfig {i1 i2} {\n\
$self instvar linkConfigList_ link_\n\
if ![info exists linkConfigList_] {\n\
return\n\
}\n\
set pos [lsearch $linkConfigList_ $link_($i1:$i2)]\n\
if {$pos >= 0} {\n\
set linkConfigList_ [lreplace $linkConfigList_ $pos $pos]\n\
return\n\
}\n\
set pos [lsearch $linkConfigList_ $link_($i2:$i1)]\n\
if {$pos >= 0} {\n\
set linkConfigList_ [lreplace $linkConfigList_ $pos $pos]\n\
}\n\
}\n\
\n\
Simulator instproc duplex-link { n1 n2 bw delay type args } {\n\
$self instvar link_\n\
set i1 [$n1 id]\n\
set i2 [$n2 id]\n\
if [info exists link_($i1:$i2)] {\n\
$self remove-nam-linkconfig $i1 $i2\n\
}\n\
eval $self simplex-link $n1 $n2 $bw $delay $type $args\n\
eval $self simplex-link $n2 $n1 $bw $delay $type $args\n\
if { [Simulator set nix-routing] } {\n\
$n1 set-neighbor [$n2 id]\n\
$n2 set-neighbor [$n1 id]\n\
}\n\
}\n\
\n\
Simulator instproc duplex-intserv-link { n1 n2 bw pd sched signal adc args } {\n\
eval $self duplex-link $n1 $n2 $bw $pd intserv $sched $signal $adc $args\n\
}\n\
\n\
Simulator instproc simplex-link-op { n1 n2 op args } {\n\
$self instvar link_\n\
eval $link_([$n1 id]:[$n2 id]) $op $args\n\
}\n\
\n\
Simulator instproc duplex-link-op { n1 n2 op args } {\n\
$self instvar link_\n\
eval $link_([$n1 id]:[$n2 id]) $op $args\n\
eval $link_([$n2 id]:[$n1 id]) $op $args\n\
}\n\
\n\
Simulator instproc flush-trace {} {\n\
$self instvar alltrace_\n\
if [info exists alltrace_] {\n\
foreach trace $alltrace_ {\n\
$trace flush\n\
}\n\
}\n\
}\n\
\n\
Simulator instproc namtrace-all file   {\n\
$self instvar namtraceAllFile_\n\
if {$file != \"\"} {\n\
set namtraceAllFile_ $file\n\
} else {\n\
unset namtraceAllFile_\n\
}\n\
}\n\
\n\
Simulator instproc energy-color-change {level1 level2} {\n\
$self instvar level1_ level2_\n\
set level1_ $level1\n\
set level2_ $level2\n\
}\n\
\n\
Simulator instproc namtrace-all-wireless {file optx opty} {\n\
$self instvar namtraceAllFile_\n\
if {$file != \"\"} {\n\
set namtraceAllFile_ $file\n\
} else {\n\
unset namtraceAllFile_\n\
}\n\
if { $optx != \"\" && $opty != \"\" } {\n\
$self puts-nam-config \"W -t * -x $optx -y $opty\"\n\
}\n\
}\n\
\n\
Simulator instproc nam-end-wireless {stoptime} {\n\
$self instvar namtraceAllFile_\n\
\n\
if {$namtraceAllFile_ != \"\"} {\n\
$self puts-nam-config \"W -t $stoptime\"\n\
}\n\
}\n\
\n\
Simulator instproc namtrace-some file {\n\
$self instvar namtraceSomeFile_\n\
set namtraceSomeFile_ $file\n\
}\n\
\n\
Simulator instproc namtrace-all-wireless {file optx opty} {\n\
$self instvar namtraceAllFile_  \n\
\n\
if {$file != \"\"} { \n\
set namtraceAllFile_ $file\n\
} else {\n\
unset namtraceAllFile_\n\
}       \n\
$self puts-nam-config \"W -t * -x $optx -y $opty\"\n\
}\n\
\n\
\n\
Simulator instproc eventtrace-all {{file \"\"}} {\n\
$self instvar eventTraceAll_ eventtraceAllFile_ traceAllFile_\n\
set eventTraceAll_ 1\n\
if {$file != \"\"} {\n\
set eventtraceAllFile_ $file\n\
} else {\n\
set eventtraceAllFile_ $traceAllFile_\n\
}\n\
\n\
}\n\
\n\
\n\
\n\
Simulator instproc initial_node_pos {nodep size} {\n\
$self instvar addressType_\n\
$self instvar energyModel_ \n\
\n\
if [info exists energyModel_] {  \n\
set nodeColor \"green\"\n\
} else {\n\
set nodeColor \"black\"\n\
}\n\
if { [info exists addressType_] && $addressType_ == \"hierarchical\" } {\n\
$self puts-nam-config \"n -t * -a [$nodep set address_] \\\n\
-s [$nodep id] -x [$nodep set X_] -y [$nodep set Y_] -Z [$nodep set Z_] \\\n\
-z $size -v circle -c $nodeColor\"\n\
} else { \n\
$self puts-nam-config \"n -t * -s [$nodep id] \\\n\
-x [$nodep set X_] -y [$nodep set Y_] -Z [$nodep set Z_] -z $size \\\n\
-v circle -c $nodeColor\"\n\
}\n\
}\n\
\n\
Simulator instproc trace-all file {\n\
$self instvar traceAllFile_\n\
set traceAllFile_ $file\n\
}\n\
\n\
Simulator instproc get-nam-traceall {} {\n\
$self instvar namtraceAllFile_\n\
if [info exists namtraceAllFile_] {\n\
return $namtraceAllFile_\n\
} else {\n\
return \"\"\n\
}\n\
}\n\
\n\
Simulator instproc get-ns-traceall {} {\n\
$self instvar traceAllFile_\n\
if [info exists traceAllFile_] {\n\
return $traceAllFile_\n\
} else {\n\
return \"\"\n\
}\n\
}\n\
\n\
Simulator instproc puts-ns-traceall { str } {\n\
$self instvar traceAllFile_\n\
if [info exists traceAllFile_] {\n\
puts $traceAllFile_ $str\n\
}\n\
}\n\
\n\
Simulator instproc puts-nam-traceall { str } {\n\
$self instvar namtraceAllFile_\n\
if [info exists namtraceAllFile_] {\n\
puts $namtraceAllFile_ $str\n\
} elseif [info exists namtraceSomeFile_] {\n\
puts $namtraceSomeFile_ $str\n\
}\n\
}\n\
\n\
Simulator instproc namtrace-config { f } {\n\
$self instvar namConfigFile_\n\
set namConfigFile_ $f\n\
}\n\
\n\
Simulator instproc get-nam-config {} {\n\
$self instvar namConfigFile_\n\
if [info exists namConfigFile_] {\n\
return $namConfigFile_\n\
} else {\n\
return \"\"\n\
}\n\
}\n\
\n\
Simulator instproc puts-nam-config { str } {\n\
$self instvar namtraceAllFile_ namConfigFile_\n\
\n\
if [info exists namConfigFile_] {\n\
puts $namConfigFile_ $str\n\
} elseif [info exists namtraceAllFile_] {\n\
puts $namtraceAllFile_ $str\n\
} elseif [info exists namtraceSomeFile_] {\n\
puts $namtraceSomeFile_ $str\n\
}\n\
}\n\
\n\
Simulator instproc color { id name } {\n\
$self instvar color_\n\
set color_($id) $name\n\
}\n\
\n\
Simulator instproc get-color { id } {\n\
$self instvar color_\n\
return $color_($id)\n\
}\n\
\n\
Simulator instproc create-trace { type file src dst {op \"\"} } {\n\
$self instvar alltrace_\n\
set p [new Trace/$type]\n\
if [catch {$p set src_ [$src id]}] {\n\
$p set src_ $src\n\
}\n\
if [catch {$p set dst_ [$dst id]}] {\n\
$p set dst_ $dst\n\
}\n\
lappend alltrace_ $p\n\
if {$file != \"\"} {\n\
$p ${op}attach $file		\n\
}\n\
return $p\n\
}\n\
\n\
\n\
Simulator instproc create-eventtrace {type owner } {\n\
$self instvar alltrace_ \n\
$self instvar eventTraceAll_ eventtraceAllFile_ namtraceAllFile_\n\
\n\
if ![info exists eventTraceAll_] return\n\
\n\
if { $eventTraceAll_ == 1 } {\n\
\n\
set et [new BaseTrace/$type]\n\
$owner cmd eventtrace $et\n\
\n\
lappend alltrace_ $et\n\
$et attach $eventtraceAllFile_\n\
if [info exists namtraceAllFile_] {\n\
$et namattach $namtraceAllFile_\n\
}\n\
}\n\
}\n\
\n\
\n\
Simulator instproc namtrace-queue { n1 n2 {file \"\"} } {\n\
$self instvar link_ namtraceAllFile_\n\
if {$file == \"\"} {\n\
if ![info exists namtraceAllFile_] return\n\
set file $namtraceAllFile_\n\
}\n\
$link_([$n1 id]:[$n2 id]) nam-trace $self $file\n\
\n\
set queue [$link_([$n1 id]:[$n2 id]) queue]\n\
$queue attach-nam-traces $n1 $n2 $file\n\
}\n\
\n\
Simulator instproc trace-queue { n1 n2 {file \"\"} } {\n\
$self instvar link_ traceAllFile_\n\
if {$file == \"\"} {\n\
if ![info exists traceAllFile_] return\n\
set file $traceAllFile_\n\
}\n\
$link_([$n1 id]:[$n2 id]) trace $self $file\n\
\n\
set queue [$link_([$n1 id]:[$n2 id]) queue]\n\
$queue attach-traces $n1 $n2 $file\n\
}\n\
\n\
Simulator instproc monitor-queue { n1 n2 qtrace { sampleInterval 0.1 } } {\n\
$self instvar link_\n\
return [$link_([$n1 id]:[$n2 id]) init-monitor $self $qtrace $sampleInterval]\n\
}\n\
\n\
Simulator instproc queue-limit { n1 n2 limit } {\n\
$self instvar link_\n\
[$link_([$n1 id]:[$n2 id]) queue] set limit_ $limit\n\
}\n\
\n\
Simulator instproc drop-trace { n1 n2 trace } {\n\
$self instvar link_\n\
[$link_([$n1 id]:[$n2 id]) queue] drop-target $trace\n\
}\n\
\n\
Simulator instproc cost {n1 n2 c} {\n\
$self instvar link_\n\
$link_([$n1 id]:[$n2 id]) cost $c\n\
}\n\
\n\
Simulator instproc attach-agent { node agent } {\n\
$node attach $agent\n\
}\n\
\n\
Simulator instproc attach-tbf-agent { node agent tbf } {\n\
$node attach $agent\n\
$agent attach-tbf $tbf\n\
}\n\
\n\
\n\
Simulator instproc detach-agent { node agent } {\n\
$self instvar nullAgent_\n\
$node detach $agent $nullAgent_\n\
}\n\
\n\
Simulator instproc delay { n1 n2 delay {type simplex} } {\n\
$self instvar link_\n\
set sid [$n1 id]\n\
set did [$n2 id]\n\
if [info exists link_($sid:$did)] {\n\
set d [$link_($sid:$did) link]\n\
$d set delay_ $delay\n\
}\n\
if {$type == \"duplex\"} {\n\
if [info exists link_($did:$sid)] {\n\
set d [$link_($did:$sid) link]\n\
$d set delay_ $delay\n\
}\n\
}\n\
}\n\
\n\
Simulator instproc bandwidth { n1 n2 bandwidth {type simplex} } {\n\
$self instvar link_\n\
set sid [$n1 id]\n\
set did [$n2 id]\n\
if [info exists link_($sid:$did)] {\n\
set d [$link_($sid:$did) link]\n\
$d set bandwidth_ $bandwidth\n\
} \n\
if {$type == \"duplex\"} {\n\
if [info exists link_($did:$sid)] {\n\
set d [$link_($did:$sid) link]\n\
$d set bandwidth_ $bandwidth\n\
}\n\
}\n\
}\n\
\n\
\n\
Simulator instproc connect {src dst} {\n\
\n\
$self simplex-connect $src $dst\n\
$self simplex-connect $dst $src\n\
\n\
\n\
global nconn_ conn_\n\
set sid [$src nodeid]\n\
set did [$dst nodeid]\n\
\n\
if {[lindex [split [$src info class] \"/\"] 1] == \"TCP\"} {\n\
set conn_($nconn_) $sid:$did\n\
incr nconn_\n\
}\n\
\n\
return $src\n\
}\n\
\n\
Simulator instproc simplex-connect { src dst } {\n\
$src set dst_addr_ [$dst set agent_addr_] \n\
$src set dst_port_ [$dst set agent_port_]\n\
\n\
\n\
if {[lindex [split [$src info class] \"/\"] 1] == \"AbsTCP\"} {\n\
$self at [$self now] \"$self rtt $src $dst\"\n\
$dst set class_ [$src set class_]\n\
}\n\
\n\
return $src\n\
}\n\
\n\
\n\
Simulator proc instance {} {\n\
set ns [Simulator info instances]\n\
if { $ns != \"\" } {\n\
return $ns\n\
}\n\
foreach sim [Simulator info subclass] {\n\
set ns [$sim info instances]\n\
if { $ns != \"\" } {\n\
return $ns\n\
}\n\
}\n\
error \"Cannot find instance of simulator\"\n\
}\n\
\n\
Simulator instproc get-number-of-nodes {} {\n\
return  [$self array size Node_]\n\
}\n\
\n\
Simulator instproc get-node-by-id id {\n\
$self instvar Node_\n\
return $Node_($id)\n\
}\n\
\n\
Simulator instproc get-node-id-by-addr address {\n\
$self instvar Node_\n\
set n [Node set nn_]\n\
for {set q 0} {$q < $n} {incr q} {\n\
set nq $Node_($q)\n\
if {[string compare [$nq node-addr] $address] == 0} {\n\
return $q\n\
}\n\
}\n\
error \"get-node-id-by-addr:Cannot find node with given address\"\n\
}\n\
\n\
Simulator instproc get-node-by-addr address {\n\
return [$self get-node-by-id [$self get-node-id-by-addr $address]]\n\
}\n\
\n\
Simulator instproc all-nodes-list {} {\n\
$self instvar Node_\n\
set nodes \"\"\n\
foreach n [lsort -dictionary [array names Node_]] {\n\
lappend nodes $Node_($n)\n\
}\n\
return $nodes\n\
}\n\
\n\
Simulator instproc link { n1 n2 } {\n\
$self instvar Node_ link_\n\
if { ![catch \"$n1 info class Node\"] } {\n\
set n1 [$n1 id]\n\
}\n\
if { ![catch \"$n2 info class Node\"] } {\n\
set n2 [$n2 id]\n\
}\n\
if [info exists link_($n1:$n2)] {\n\
return $link_($n1:$n2)\n\
}\n\
return \"\"\n\
}\n\
\n\
Simulator instproc create-connection {s_type source d_type dest pktClass} {\n\
set s_agent [new Agent/$s_type]\n\
set d_agent [new Agent/$d_type]\n\
$s_agent set fid_ $pktClass\n\
$d_agent set fid_ $pktClass\n\
$self attach-agent $source $s_agent\n\
$self attach-agent $dest $d_agent\n\
$self connect $s_agent $d_agent\n\
\n\
return $s_agent\n\
}\n\
\n\
Simulator instproc create-connection-list {s_type source d_type dest pktClass} {\n\
set s_agent [new Agent/$s_type]\n\
set d_agent [new Agent/$d_type]\n\
$s_agent set fid_ $pktClass\n\
$d_agent set fid_ $pktClass\n\
$self attach-agent $source $s_agent\n\
$self attach-agent $dest $d_agent\n\
$self connect $s_agent $d_agent\n\
\n\
return [list $s_agent $d_agent]\n\
}   \n\
\n\
Simulator instproc create-connection-listen {s_type source d_type dest pktClass} {\n\
set s_agent [new Agent/$s_type]\n\
set d_agent [new Agent/$d_type]\n\
$s_agent set fid_ $pktClass\n\
$d_agent set fid_ $pktClass\n\
$self attach-agent $source $s_agent\n\
$self attach-agent $dest $d_agent\n\
$self connect $s_agent $d_agent\n\
$d_agent listen\n\
\n\
return $s_agent \n\
}   \n\
\n\
Simulator instproc create-tcp-connection {s_type source d_type dest pktClass} {\n\
set s_agent [new Agent/$s_type]\n\
set d_agent [new Agent/$d_type]\n\
$s_agent set fid_ $pktClass\n\
$d_agent set fid_ $pktClass\n\
$self attach-agent $source $s_agent\n\
$self attach-agent $dest $d_agent\n\
return \"$s_agent $d_agent\"\n\
}\n\
\n\
Classifier instproc install {slot val} {\n\
$self set slots_($slot) $val\n\
$self cmd install $slot $val\n\
}\n\
\n\
Classifier instproc installNext val {\n\
set slot [$self cmd installNext $val]\n\
$self set slots_($slot) $val\n\
set slot\n\
}\n\
\n\
Classifier instproc adjacents {} {\n\
$self array get slots_\n\
}\n\
\n\
Classifier instproc in-slot? slot {\n\
$self instvar slots_\n\
set ret \"\"\n\
if {[array size slots_] < $slot} {\n\
set ret slots_($slot)\n\
}\n\
set ret\n\
}\n\
\n\
Classifier instproc dump {} {\n\
$self instvar slots_ offset_ shift_ mask_\n\
puts \"classifier $self\"\n\
puts \"\\t$offset_ offset\"\n\
puts \"\\t$shift_ shift\"\n\
puts \"\\t$mask_ mask\"\n\
puts \"\\t[array size slots_] slots\"\n\
foreach i [lsort -integer [array names slots_]] {\n\
set iv $slots_($i)\n\
puts \"\\t\\tslot $i: $iv ([$iv info class])\"\n\
}\n\
}\n\
\n\
Classifier instproc no-slot slot {\n\
puts stderr \"--- Classfier::no-slot{} default handler (tcl/lib/ns-lib.tcl) ---\"\n\
puts stderr \"\\t$self: no target for slot $slot\"\n\
puts stderr \"\\t$self type: [$self info class]\"\n\
puts stderr \"content dump:\"\n\
$self dump\n\
puts stderr \"---------- Finished standard no-slot{} default handler ----------\"\n\
[Simulator instance] flush-trace\n\
exit 1\n\
}\n\
\n\
Classifier/Hash instproc dump args {\n\
eval $self next $args\n\
$self instvar default_\n\
puts \"\\t$default_ default\"\n\
}\n\
\n\
Classifier/Hash instproc init nbuck {\n\
$self next $nbuck\n\
$self instvar shift_ mask_\n\
set shift_ [AddrParams NodeShift 1]\n\
set mask_ [AddrParams NodeMask 1]\n\
}\n\
\n\
Classifier/Port/Reserve instproc init args {\n\
eval $self next\n\
$self reserve-port 2\n\
}\n\
\n\
Simulator instproc makeflowmon { cltype { clslots 29 } } {\n\
set flowmon [new QueueMonitor/ED/Flowmon]\n\
set cl [new Classifier/Hash/$cltype $clslots]\n\
\n\
$cl proc unknown-flow { src dst fid }  {\n\
set fdesc [new QueueMonitor/ED/Flow]\n\
set dsamp [new Samples]\n\
$fdesc set-delay-samples $dsamp\n\
set slot [$self installNext $fdesc] \n\
$self set-hash auto $src $dst $fid $slot\n\
}\n\
\n\
$cl proc no-slot slotnum {\n\
}\n\
$flowmon classifier $cl\n\
return $flowmon\n\
}\n\
\n\
\n\
Simulator instproc attach-fmon {lnk fm { edrop 0 } } {\n\
set isnoop [new SnoopQueue/In]\n\
set osnoop [new SnoopQueue/Out]\n\
set dsnoop [new SnoopQueue/Drop]\n\
$lnk attach-monitors $isnoop $osnoop $dsnoop $fm\n\
if { $edrop != 0 } {\n\
set edsnoop [new SnoopQueue/EDrop]\n\
$edsnoop set-monitor $fm\n\
[$lnk queue] early-drop-target $edsnoop\n\
$edsnoop target [$self set nullAgent_]\n\
}\n\
[$lnk queue] drop-target $dsnoop\n\
}\n\
\n\
\n\
Simulator instproc maketbtagger { cltype { clslots 29 } } {\n\
\n\
set tagger [new QueueMonitor/ED/Tagger]\n\
set cl [new Classifier/Hash/$cltype $clslots]\n\
\n\
$cl proc unknown-flow { src dst fid }  {\n\
set fdesc [new QueueMonitor/ED/Flow/TB]\n\
set dsamp [new Samples]\n\
$fdesc set-delay-samples $dsamp\n\
set slot [$self installNext $fdesc]\n\
$self set-hash auto $src $dst $fid $slot\n\
}\n\
\n\
$cl proc set-rate { src dst fid hashbucket rate depth init} {\n\
set fdesc [new QueueMonitor/ED/Flow/TB]\n\
set dsamp [new Samples]\n\
$fdesc set-delay-samples $dsamp\n\
$fdesc set target_rate_ $rate\n\
$fdesc set bucket_depth_ $depth\n\
$fdesc set tbucket_ $init  \n\
set slot [$self installNext $fdesc]\n\
$self set-hash $hashbucket $src $dst $fid $slot\n\
}\n\
\n\
$cl proc no-slot slotnum {\n\
}\n\
$tagger classifier $cl\n\
return $tagger\n\
}\n\
\n\
\n\
Simulator instproc maketswtagger { cltype { clslots 29 } } {\n\
\n\
set tagger [new QueueMonitor/ED/Tagger]\n\
set cl [new Classifier/Hash/$cltype $clslots]\n\
\n\
$cl proc unknown-flow { src dst fid hashbucket }  {\n\
set fdesc [new QueueMonitor/ED/Flow/TSW]\n\
set dsamp [new Samples]\n\
$fdesc set-delay-samples $dsamp\n\
set slot [$self installNext $fdesc]\n\
$self set-hash $hashbucket $src $dst $fid $slot\n\
}\n\
\n\
$cl proc no-slot slotnum {\n\
}\n\
$tagger classifier $cl\n\
return $tagger\n\
}\n\
\n\
\n\
Simulator instproc attach-tagger {lnk fm} {\n\
set isnoop [new SnoopQueue/Tagger]\n\
$lnk attach-taggers $isnoop $fm\n\
}\n\
\n\
Simulator instproc lossmodel {lossobj from to} {\n\
set link [$self link $from $to]\n\
$link errormodule $lossobj\n\
}\n\
\n\
Simulator instproc link-lossmodel {lossobj from to} {\n\
set link [$self link $from $to]\n\
$link insert-linkloss $lossobj\n\
}\n\
\n\
\n\
\n\
Simulator instproc rtt { src dst } {\n\
$self instvar routingTable_ delay_\n\
set srcid [[$src set node_] id]\n\
set dstid [[$dst set node_] id]\n\
set delay 0\n\
set tmpid $srcid\n\
while {$tmpid != $dstid} {\n\
set nextid [$routingTable_ lookup $tmpid $dstid]\n\
set tmpnode [$self get-node-by-id $tmpid]\n\
set nextnode [$self get-node-by-id $nextid]\n\
set tmplink [[$self link $tmpnode $nextnode] link]\n\
set delay [expr $delay + [expr 2 * [$tmplink set delay_]]]\n\
set delay [expr $delay + [expr 8320 / [$tmplink set bandwidth_]]]\n\
set tmpid $nextid\n\
}\n\
$src rtt $delay\n\
return $delay\n\
}\n\
\n\
Simulator instproc abstract-tcp {} {\n\
$self instvar TahoeAckfsm_ RenoAckfsm_ TahoeDelAckfsm_ RenoDelAckfsm_ dropper_ \n\
$self set TahoeAckfsm_ [new FSM/TahoeAck]\n\
$self set RenoAckfsm_ [new FSM/RenoAck]\n\
$self set TahoeDelAckfsm_ [new FSM/TahoeDelAck]\n\
$self set RenoDelAckfsm_ [new FSM/RenoDelAck]\n\
$self set nullAgent_ [new DropTargetAgent]\n\
}\n\
\n\
\n\
Simulator instproc create-diffusion-rate-agent {node} {\n\
global opt\n\
set diff [new Agent/Diffusion/RateGradient]\n\
\n\
$node set diffagent_ $diff\n\
$node set ragent_ $diff\n\
\n\
$diff on-node $node\n\
\n\
if [info exist opt(enablePos)] {\n\
if {$opt(enablePos) == \"true\"} {\n\
$diff enable-pos\n\
} else {\n\
$diff disable-pos\n\
}\n\
} \n\
\n\
if [info exist opt(enableNeg)] {\n\
if {$opt(enableNeg) == \"true\"} {\n\
$diff enable-neg\n\
} else {\n\
$diff disable-neg\n\
}\n\
} \n\
\n\
if [info exist opt(suppression)] {\n\
if {$opt(suppression) == \"true\"} {\n\
$diff enable-suppression\n\
} else {\n\
$diff disable-suppression\n\
}\n\
} \n\
\n\
if [info exist opt(subTxType)] {\n\
$diff set-sub-tx-type $opt(subTxType)\n\
} \n\
\n\
if [info exist opt(orgTxType)] {\n\
$diff set-org-tx-type $opt(orgTxType)\n\
} \n\
\n\
if [info exist opt(posType)] {\n\
$diff set-pos-type $opt(posType)\n\
} \n\
\n\
if [info exist opt(posNodeType)] {\n\
$diff set-pos-node-type $opt(posNodeType)\n\
} \n\
\n\
if [info exist opt(negWinType)] {\n\
$diff set-neg-win-type $opt(negWinType)\n\
} \n\
\n\
if [info exist opt(negThrType)] {\n\
$diff set-neg-thr-type $opt(negThrType)\n\
} \n\
\n\
if [info exist opt(negMaxType)] {\n\
$diff set-neg-max-type $opt(negMaxType)\n\
} \n\
\n\
$self put-in-list $diff\n\
$self at 0.0 \"$diff start\"\n\
\n\
return $diff\n\
}\n\
\n\
Simulator instproc create-diffusion-probability-agent {node} {\n\
global opt\n\
set diff [new Agent/Diffusion/ProbGradient]\n\
\n\
$node set diffagent_ $diff\n\
$node set ragent_ $diff\n\
\n\
$diff on-node $node\n\
\n\
if [info exist opt(enablePos)] {\n\
if {$opt(enablePos) == \"true\"} {\n\
$diff enable-pos\n\
} else {\n\
$diff disable-pos\n\
}\n\
} \n\
if [info exist opt(enableNeg)] {\n\
if {$opt(enableNeg) == \"true\"} {\n\
$diff enable-neg\n\
} else {\n\
$diff disable-neg\n\
}\n\
} \n\
\n\
$self put-in-list $diff\n\
$self at 0.0 \"$diff start\"\n\
\n\
return $diff\n\
}\n\
\n\
Simulator instproc create-flooding-agent {node} {\n\
set flood [new Agent/Flooding]\n\
\n\
$node set ragent_ $flood\n\
\n\
$flood on-node $node\n\
\n\
$self put-in-list $flood\n\
$self at 0.0 \"$flood start\"\n\
\n\
return $flood\n\
}\n\
\n\
Simulator instproc create-omnimcast-agent {node} {\n\
set omni [new Agent/OmniMcast]\n\
\n\
$node set ragent_ $omni\n\
\n\
$omni on-node $node\n\
\n\
$self put-in-list $omni\n\
$self at 0.0 \"$omni start\"\n\
\n\
return $omni\n\
}\n\
\n\
Simulator instproc put-in-list {agent} {\n\
$self instvar lagent\n\
lappend lagent $agent\n\
}\n\
\n\
Simulator instproc terminate-all-agents {} {\n\
$self instvar lagent\n\
foreach i $lagent {\n\
$i terminate\n\
}\n\
}\n\
\n\
Simulator instproc prepare-to-stop {} {\n\
$self instvar lagent\n\
foreach i $lagent {\n\
$i stop\n\
}\n\
}\n\
\n\
";
#include "tclcl.h"
EmbeddedTcl et_ns_lib(code);
