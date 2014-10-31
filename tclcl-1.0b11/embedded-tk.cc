static const char code[] = "\n\
if {[info commands package] == \"\"} {\n\
error \"version mismatch: library\\nscripts expect Tcl version 7.5b1 or later but the loaded version is\\nonly [info patchlevel]\"\n\
}\n\
package require -exact Tcl 8.3\n\
\n\
\n\
if {![info exists auto_path]} {\n\
if {[info exist env(TCLLIBPATH)]} {\n\
set auto_path $env(TCLLIBPATH)\n\
} else {\n\
set auto_path \"\"\n\
}\n\
}\n\
if {[string compare [info library] {}]} {\n\
foreach __dir [list [info library] [file dirname [info library]]] {\n\
if {[lsearch -exact $auto_path $__dir] < 0} {\n\
lappend auto_path $__dir\n\
}\n\
}\n\
}\n\
set __dir [file join [file dirname [file dirname \\\n\
[info nameofexecutable]]] lib]\n\
if {[lsearch -exact $auto_path $__dir] < 0} {\n\
lappend auto_path $__dir\n\
}\n\
if {[info exist tcl_pkgPath]} {\n\
foreach __dir $tcl_pkgPath {\n\
if {[lsearch -exact $auto_path $__dir] < 0} {\n\
lappend auto_path $__dir\n\
}\n\
}\n\
}\n\
if {[info exists __dir]} {\n\
unset __dir\n\
}\n\
\n\
\n\
if {(![interp issafe]) && [string equal $tcl_platform(platform) \"windows\"]} {\n\
namespace eval tcl {\n\
proc envTraceProc {lo n1 n2 op} {\n\
set x $::env($n2)\n\
set ::env($lo) $x\n\
set ::env([string toupper $lo]) $x\n\
}\n\
}\n\
foreach p [array names env] {\n\
set u [string toupper $p]\n\
if {[string compare $u $p]} {\n\
switch -- $u {\n\
COMSPEC -\n\
PATH {\n\
if {![info exists env($u)]} {\n\
set env($u) $env($p)\n\
}\n\
trace variable env($p) w [list tcl::envTraceProc $p]\n\
trace variable env($u) w [list tcl::envTraceProc $p]\n\
}\n\
}\n\
}\n\
}\n\
if {[info exists p]} {\n\
unset p\n\
}\n\
if {[info exists u]} {\n\
unset u\n\
}\n\
if {![info exists env(COMSPEC)]} {\n\
if {[string equal $tcl_platform(os) \"Windows NT\"]} {\n\
set env(COMSPEC) cmd.exe\n\
} else {\n\
set env(COMSPEC) command.com\n\
}\n\
}\n\
}\n\
\n\
\n\
package unknown tclPkgUnknown\n\
\n\
\n\
if {[llength [info commands exec]] == 0} {\n\
\n\
\n\
set auto_noexec 1\n\
}\n\
set errorCode \"\"\n\
set errorInfo \"\"\n\
\n\
\n\
if {[llength [info commands tclLog]] == 0} {\n\
proc tclLog {string} {\n\
catch {puts stderr $string}\n\
}\n\
}\n\
\n\
\n\
proc unknown args {\n\
global auto_noexec auto_noload env unknown_pending tcl_interactive\n\
global errorCode errorInfo\n\
\n\
\n\
set cmd [lindex $args 0]\n\
if {[regexp \"^namespace\\[ \\t\\n\\]+inscope\" $cmd] && [llength $cmd] == 4} {\n\
set arglist [lrange $args 1 end]\n\
set ret [catch {uplevel $cmd $arglist} result]\n\
if {$ret == 0} {\n\
return $result\n\
} else {\n\
return -code $ret -errorcode $errorCode $result\n\
}\n\
}\n\
\n\
\n\
set savedErrorCode $errorCode\n\
set savedErrorInfo $errorInfo\n\
set name [lindex $args 0]\n\
if {![info exists auto_noload]} {\n\
if {[info exists unknown_pending($name)]} {\n\
return -code error \"self-referential recursion in \\\"unknown\\\" for command \\\"$name\\\"\";\n\
}\n\
set unknown_pending($name) pending;\n\
set ret [catch {auto_load $name [uplevel 1 {namespace current}]} msg]\n\
unset unknown_pending($name);\n\
if {$ret != 0} {\n\
append errorInfo \"\\n    (autoloading \\\"$name\\\")\"\n\
return -code $ret -errorcode $errorCode -errorinfo $errorInfo $msg\n\
}\n\
if {![array size unknown_pending]} {\n\
unset unknown_pending\n\
}\n\
if {$msg} {\n\
set errorCode $savedErrorCode\n\
set errorInfo $savedErrorInfo\n\
set code [catch {uplevel 1 $args} msg]\n\
if {$code ==  1} {\n\
\n\
set new [split $errorInfo \\n]\n\
set new [join [lrange $new 0 [expr {[llength $new] - 6}]] \\n]\n\
return -code error -errorcode $errorCode \\\n\
-errorinfo $new $msg\n\
} else {\n\
return -code $code $msg\n\
}\n\
}\n\
}\n\
\n\
if {([info level] == 1) && [string equal [info script] \"\"] \\\n\
&& [info exists tcl_interactive] && $tcl_interactive} {\n\
if {![info exists auto_noexec]} {\n\
set new [auto_execok $name]\n\
if {[string compare {} $new]} {\n\
set errorCode $savedErrorCode\n\
set errorInfo $savedErrorInfo\n\
set redir \"\"\n\
if {[string equal [info commands console] \"\"]} {\n\
set redir \">&@stdout <@stdin\"\n\
}\n\
return [uplevel exec $redir $new [lrange $args 1 end]]\n\
}\n\
}\n\
set errorCode $savedErrorCode\n\
set errorInfo $savedErrorInfo\n\
if {[string equal $name \"!!\"]} {\n\
set newcmd [history event]\n\
} elseif {[regexp {^!(.+)$} $name dummy event]} {\n\
set newcmd [history event $event]\n\
} elseif {[regexp {^\\^([^^]*)\\^([^^]*)\\^?$} $name dummy old new]} {\n\
set newcmd [history event -1]\n\
catch {regsub -all -- $old $newcmd $new newcmd}\n\
}\n\
if {[info exists newcmd]} {\n\
tclLog $newcmd\n\
history change $newcmd 0\n\
return [uplevel $newcmd]\n\
}\n\
\n\
set ret [catch {set cmds [info commands $name*]} msg]\n\
if {[string equal $name \"::\"]} {\n\
set name \"\"\n\
}\n\
if {$ret != 0} {\n\
return -code $ret -errorcode $errorCode \\\n\
\"error in unknown while checking if \\\"$name\\\" is a unique command abbreviation: $msg\"\n\
}\n\
if {[llength $cmds] == 1} {\n\
return [uplevel [lreplace $args 0 0 $cmds]]\n\
}\n\
if {[llength $cmds]} {\n\
if {[string equal $name \"\"]} {\n\
return -code error \"empty command name \\\"\\\"\"\n\
} else {\n\
return -code error \\\n\
\"ambiguous command name \\\"$name\\\": [lsort $cmds]\"\n\
}\n\
}\n\
}\n\
return -code error \"invalid command name \\\"$name\\\"\"\n\
}\n\
\n\
\n\
proc auto_load {cmd {namespace {}}} {\n\
global auto_index auto_oldpath auto_path\n\
\n\
if {[string length $namespace] == 0} {\n\
set namespace [uplevel {namespace current}]\n\
}\n\
set nameList [auto_qualify $cmd $namespace]\n\
lappend nameList $cmd\n\
foreach name $nameList {\n\
if {[info exists auto_index($name)]} {\n\
uplevel #0 $auto_index($name)\n\
return [expr {[info commands $name] != \"\"}]\n\
}\n\
}\n\
if {![info exists auto_path]} {\n\
return 0\n\
}\n\
\n\
if {![auto_load_index]} {\n\
return 0\n\
}\n\
foreach name $nameList {\n\
if {[info exists auto_index($name)]} {\n\
uplevel #0 $auto_index($name)\n\
if { ![string equal [namespace which -command $name] \"\"] } {\n\
return 1\n\
}\n\
}\n\
}\n\
return 0\n\
}\n\
\n\
\n\
proc auto_load_index {} {\n\
global auto_index auto_oldpath auto_path errorInfo errorCode\n\
\n\
if {[info exists auto_oldpath] && \\\n\
[string equal $auto_oldpath $auto_path]} {\n\
return 0\n\
}\n\
set auto_oldpath $auto_path\n\
\n\
\n\
set issafe [interp issafe]\n\
for {set i [expr {[llength $auto_path] - 1}]} {$i >= 0} {incr i -1} {\n\
set dir [lindex $auto_path $i]\n\
set f \"\"\n\
if {$issafe} {\n\
catch {source [file join $dir tclIndex]}\n\
} elseif {[catch {set f [open [file join $dir tclIndex]]}]} {\n\
continue\n\
} else {\n\
set error [catch {\n\
set id [gets $f]\n\
if {[string equal $id \\\n\
\"# Tcl autoload index file, version 2.0\"]} {\n\
eval [read $f]\n\
} elseif {[string equal $id \"# Tcl autoload index file: each line identifies a Tcl\"]} {\n\
while {[gets $f line] >= 0} {\n\
if {[string equal [string index $line 0] \"#\"] \\\n\
|| ([llength $line] != 2)} {\n\
continue\n\
}\n\
set name [lindex $line 0]\n\
set auto_index($name) \\\n\
\"source [file join $dir [lindex $line 1]]\"\n\
}\n\
} else {\n\
error \"[file join $dir tclIndex] isn't a proper Tcl index file\"\n\
}\n\
} msg]\n\
if {[string compare $f \"\"]} {\n\
close $f\n\
}\n\
if {$error} {\n\
error $msg $errorInfo $errorCode\n\
}\n\
}\n\
}\n\
return 1\n\
}\n\
\n\
\n\
proc auto_qualify {cmd namespace} {\n\
\n\
set n [regsub -all {::+} $cmd :: cmd]\n\
\n\
\n\
\n\
if {[regexp {^::(.*)$} $cmd x tail]} {\n\
if {$n > 1} {\n\
return [list $cmd]\n\
} else {\n\
return [list $tail]\n\
}\n\
}\n\
\n\
\n\
if {$n == 0} {\n\
if {[string equal $namespace ::]} {\n\
return [list $cmd]\n\
} else {\n\
return [list ${namespace}::$cmd $cmd]\n\
}\n\
} elseif {[string equal $namespace ::]} {\n\
return [list ::$cmd]\n\
} else {\n\
return [list ${namespace}::$cmd ::$cmd]\n\
}\n\
}\n\
\n\
\n\
proc auto_import {pattern} {\n\
global auto_index\n\
\n\
\n\
if {![string match *::* $pattern]} {\n\
return\n\
}\n\
\n\
set ns [uplevel namespace current]\n\
set patternList [auto_qualify $pattern $ns]\n\
\n\
auto_load_index\n\
\n\
foreach pattern $patternList {\n\
foreach name [array names auto_index] {\n\
if {[string match $pattern $name] && \\\n\
[string equal \"\" [info commands $name]]} {\n\
uplevel #0 $auto_index($name)\n\
}\n\
}\n\
}\n\
}\n\
\n\
\n\
if {[string equal windows $tcl_platform(platform)]} {\n\
proc auto_execok name {\n\
global auto_execs env tcl_platform\n\
\n\
if {[info exists auto_execs($name)]} {\n\
return $auto_execs($name)\n\
}\n\
set auto_execs($name) \"\"\n\
\n\
set shellBuiltins [list cls copy date del erase dir echo mkdir \\\n\
md rename ren rmdir rd time type ver vol]\n\
if {[string equal $tcl_platform(os) \"Windows NT\"]} {\n\
lappend shellBuiltins \"start\"\n\
}\n\
\n\
if {[lsearch -exact $shellBuiltins $name] != -1} {\n\
return [set auto_execs($name) [list $env(COMSPEC) /c $name]]\n\
}\n\
\n\
if {[llength [file split $name]] != 1} {\n\
foreach ext {{} .com .exe .bat} {\n\
set file ${name}${ext}\n\
if {[file exists $file] && ![file isdirectory $file]} {\n\
return [set auto_execs($name) [list $file]]\n\
}\n\
}\n\
return \"\"\n\
}\n\
\n\
set path \"[file dirname [info nameof]];.;\"\n\
if {[info exists env(WINDIR)]} {\n\
set windir $env(WINDIR) \n\
}\n\
if {[info exists windir]} {\n\
if {[string equal $tcl_platform(os) \"Windows NT\"]} {\n\
append path \"$windir/system32;\"\n\
}\n\
append path \"$windir/system;$windir;\"\n\
}\n\
\n\
foreach var {PATH Path path} {\n\
if {[info exists env($var)]} {\n\
append path \";$env($var)\"\n\
}\n\
}\n\
\n\
foreach dir [split $path {;}] {\n\
if {[info exists checked($dir)] || [string equal {} $dir]} { continue }\n\
set checked($dir) {}\n\
foreach ext {{} .com .exe .bat} {\n\
set file [file join $dir ${name}${ext}]\n\
if {[file exists $file] && ![file isdirectory $file]} {\n\
return [set auto_execs($name) [list $file]]\n\
}\n\
}\n\
}\n\
return \"\"\n\
}\n\
\n\
} else {\n\
proc auto_execok name {\n\
global auto_execs env\n\
\n\
if {[info exists auto_execs($name)]} {\n\
return $auto_execs($name)\n\
}\n\
set auto_execs($name) \"\"\n\
if {[llength [file split $name]] != 1} {\n\
if {[file executable $name] && ![file isdirectory $name]} {\n\
set auto_execs($name) [list $name]\n\
}\n\
return $auto_execs($name)\n\
}\n\
foreach dir [split $env(PATH) :] {\n\
if {[string equal $dir \"\"]} {\n\
set dir .\n\
}\n\
set file [file join $dir $name]\n\
if {[file executable $file] && ![file isdirectory $file]} {\n\
set auto_execs($name) [list $file]\n\
return $auto_execs($name)\n\
}\n\
}\n\
return \"\"\n\
}\n\
\n\
}\n\
\n\
\n\
namespace eval tcl {\n\
variable history\n\
if {![info exists history]} {\n\
array set history {\n\
nextid	0\n\
keep	20\n\
oldest	-20\n\
}\n\
}\n\
}\n\
\n\
\n\
proc history {args} {\n\
set len [llength $args]\n\
if {$len == 0} {\n\
return [tcl::HistInfo]\n\
}\n\
set key [lindex $args 0]\n\
set options \"add, change, clear, event, info, keep, nextid, or redo\"\n\
switch -glob -- $key {\n\
a* { # history add\n\
\n\
if {$len > 3} {\n\
return -code error \"wrong # args: should be \\\"history add event ?exec?\\\"\"\n\
}\n\
if {![string match $key* add]} {\n\
return -code error \"bad option \\\"$key\\\": must be $options\"\n\
}\n\
if {$len == 3} {\n\
set arg [lindex $args 2]\n\
if {! ([string match e* $arg] && [string match $arg* exec])} {\n\
return -code error \"bad argument \\\"$arg\\\": should be \\\"exec\\\"\"\n\
}\n\
}\n\
return [tcl::HistAdd [lindex $args 1] [lindex $args 2]]\n\
}\n\
ch* { # history change\n\
\n\
if {($len > 3) || ($len < 2)} {\n\
return -code error \"wrong # args: should be \\\"history change newValue ?event?\\\"\"\n\
}\n\
if {![string match $key* change]} {\n\
return -code error \"bad option \\\"$key\\\": must be $options\"\n\
}\n\
if {$len == 2} {\n\
set event 0\n\
} else {\n\
set event [lindex $args 2]\n\
}\n\
\n\
return [tcl::HistChange [lindex $args 1] $event]\n\
}\n\
cl* { # history clear\n\
\n\
if {($len > 1)} {\n\
return -code error \"wrong # args: should be \\\"history clear\\\"\"\n\
}\n\
if {![string match $key* clear]} {\n\
return -code error \"bad option \\\"$key\\\": must be $options\"\n\
}\n\
return [tcl::HistClear]\n\
}\n\
e* { # history event\n\
\n\
if {$len > 2} {\n\
return -code error \"wrong # args: should be \\\"history event ?event?\\\"\"\n\
}\n\
if {![string match $key* event]} {\n\
return -code error \"bad option \\\"$key\\\": must be $options\"\n\
}\n\
if {$len == 1} {\n\
set event -1\n\
} else {\n\
set event [lindex $args 1]\n\
}\n\
return [tcl::HistEvent $event]\n\
}\n\
i* { # history info\n\
\n\
if {$len > 2} {\n\
return -code error \"wrong # args: should be \\\"history info ?count?\\\"\"\n\
}\n\
if {![string match $key* info]} {\n\
return -code error \"bad option \\\"$key\\\": must be $options\"\n\
}\n\
return [tcl::HistInfo [lindex $args 1]]\n\
}\n\
k* { # history keep\n\
\n\
if {$len > 2} {\n\
return -code error \"wrong # args: should be \\\"history keep ?count?\\\"\"\n\
}\n\
if {$len == 1} {\n\
return [tcl::HistKeep]\n\
} else {\n\
set limit [lindex $args 1]\n\
if {[catch {expr {~$limit}}] || ($limit < 0)} {\n\
return -code error \"illegal keep count \\\"$limit\\\"\"\n\
}\n\
return [tcl::HistKeep $limit]\n\
}\n\
}\n\
n* { # history nextid\n\
\n\
if {$len > 1} {\n\
return -code error \"wrong # args: should be \\\"history nextid\\\"\"\n\
}\n\
if {![string match $key* nextid]} {\n\
return -code error \"bad option \\\"$key\\\": must be $options\"\n\
}\n\
return [expr {$tcl::history(nextid) + 1}]\n\
}\n\
r* { # history redo\n\
\n\
if {$len > 2} {\n\
return -code error \"wrong # args: should be \\\"history redo ?event?\\\"\"\n\
}\n\
if {![string match $key* redo]} {\n\
return -code error \"bad option \\\"$key\\\": must be $options\"\n\
}\n\
return [tcl::HistRedo [lindex $args 1]]\n\
}\n\
default {\n\
return -code error \"bad option \\\"$key\\\": must be $options\"\n\
}\n\
}\n\
}\n\
\n\
\n\
proc tcl::HistAdd {command {exec {}}} {\n\
variable history\n\
set i [incr history(nextid)]\n\
set history($i) $command\n\
set j [incr history(oldest)]\n\
if {[info exists history($j)]} {unset history($j)}\n\
if {[string match e* $exec]} {\n\
return [uplevel #0 $command]\n\
} else {\n\
return {}\n\
}\n\
}\n\
\n\
\n\
proc tcl::HistKeep {{limit {}}} {\n\
variable history\n\
if {[string length $limit] == 0} {\n\
return $history(keep)\n\
} else {\n\
set oldold $history(oldest)\n\
set history(oldest) [expr {$history(nextid) - $limit}]\n\
for {} {$oldold <= $history(oldest)} {incr oldold} {\n\
if {[info exists history($oldold)]} {unset history($oldold)}\n\
}\n\
set history(keep) $limit\n\
}\n\
}\n\
\n\
\n\
proc tcl::HistClear {} {\n\
variable history\n\
set keep $history(keep)\n\
unset history\n\
array set history [list \\\n\
nextid	0	\\\n\
keep	$keep	\\\n\
oldest	-$keep	\\\n\
]\n\
}\n\
\n\
\n\
proc tcl::HistInfo {{num {}}} {\n\
variable history\n\
if {$num == {}} {\n\
set num [expr {$history(keep) + 1}]\n\
}\n\
set result {}\n\
set newline \"\"\n\
for {set i [expr {$history(nextid) - $num + 1}]} \\\n\
{$i <= $history(nextid)} {incr i} {\n\
if {![info exists history($i)]} {\n\
continue\n\
}\n\
set cmd [string trimright $history($i) \\ \\n]\n\
regsub -all \\n $cmd \"\\n\\t\" cmd\n\
append result $newline[format \"%6d  %s\" $i $cmd]\n\
set newline \\n\n\
}\n\
return $result\n\
}\n\
\n\
\n\
proc tcl::HistRedo {{event -1}} {\n\
variable history\n\
if {[string length $event] == 0} {\n\
set event -1\n\
}\n\
set i [HistIndex $event]\n\
if {$i == $history(nextid)} {\n\
return -code error \"cannot redo the current event\"\n\
}\n\
set cmd $history($i)\n\
HistChange $cmd 0\n\
uplevel #0 $cmd\n\
}\n\
\n\
\n\
proc tcl::HistIndex {event} {\n\
variable history\n\
if {[catch {expr {~$event}}]} {\n\
for {set i [expr {$history(nextid)-1}]} {[info exists history($i)]} \\\n\
{incr i -1} {\n\
if {[string match $event* $history($i)]} {\n\
return $i;\n\
}\n\
if {[string match $event $history($i)]} {\n\
return $i;\n\
}\n\
}\n\
return -code error \"no event matches \\\"$event\\\"\"\n\
} elseif {$event <= 0} {\n\
set i [expr {$history(nextid) + $event}]\n\
} else {\n\
set i $event\n\
}\n\
if {$i <= $history(oldest)} {\n\
return -code error \"event \\\"$event\\\" is too far in the past\"\n\
}\n\
if {$i > $history(nextid)} {\n\
return -code error \"event \\\"$event\\\" hasn't occured yet\"\n\
}\n\
return $i\n\
}\n\
\n\
\n\
proc tcl::HistEvent {event} {\n\
variable history\n\
set i [HistIndex $event]\n\
if {[info exists history($i)]} {\n\
return [string trimright $history($i) \\ \\n]\n\
} else {\n\
return \"\";\n\
}\n\
}\n\
\n\
\n\
proc tcl::HistChange {cmd {event 0}} {\n\
variable history\n\
set i [HistIndex $event]\n\
set history($i) $cmd\n\
}\n\
\n\
\n\
if {[string equal $tcl_platform(platform) \"windows\"]} {\n\
set tcl_wordchars \"\\\\S\"\n\
set tcl_nonwordchars \"\\\\s\"\n\
} else {\n\
set tcl_wordchars \"\\\\w\"\n\
set tcl_nonwordchars \"\\\\W\"\n\
}\n\
\n\
\n\
proc tcl_wordBreakAfter {str start} {\n\
global tcl_nonwordchars tcl_wordchars\n\
set str [string range $str $start end]\n\
if {[regexp -indices \"$tcl_wordchars$tcl_nonwordchars|$tcl_nonwordchars$tcl_wordchars\" $str result]} {\n\
return [expr {[lindex $result 1] + $start}]\n\
}\n\
return -1\n\
}\n\
\n\
\n\
proc tcl_wordBreakBefore {str start} {\n\
global tcl_nonwordchars tcl_wordchars\n\
if {[string equal $start end]} {\n\
set start [string length $str]\n\
}\n\
if {[regexp -indices \"^.*($tcl_wordchars$tcl_nonwordchars|$tcl_nonwordchars$tcl_wordchars)\" [string range $str 0 $start] result]} {\n\
return [lindex $result 1]\n\
}\n\
return -1\n\
}\n\
\n\
\n\
proc tcl_endOfWord {str start} {\n\
global tcl_nonwordchars tcl_wordchars\n\
if {[regexp -indices \"$tcl_nonwordchars*$tcl_wordchars+$tcl_nonwordchars\" \\\n\
[string range $str $start end] result]} {\n\
return [expr {[lindex $result 1] + $start}]\n\
}\n\
return -1\n\
}\n\
\n\
\n\
proc tcl_startOfNextWord {str start} {\n\
global tcl_nonwordchars tcl_wordchars\n\
if {[regexp -indices \"$tcl_wordchars*$tcl_nonwordchars+$tcl_wordchars\" \\\n\
[string range $str $start end] result]} {\n\
return [expr {[lindex $result 1] + $start}]\n\
}\n\
return -1\n\
}\n\
\n\
\n\
proc tcl_startOfPreviousWord {str start} {\n\
global tcl_nonwordchars tcl_wordchars\n\
if {[string equal $start end]} {\n\
set start [string length $str]\n\
}\n\
if {[regexp -indices \\\n\
\"$tcl_nonwordchars*($tcl_wordchars+)$tcl_nonwordchars*\\$\" \\\n\
[string range $str 0 [expr {$start - 1}]] result word]} {\n\
return [lindex $word 0]\n\
}\n\
return -1\n\
}\n\
\n\
\n\
package provide http 2.3\n\
\n\
namespace eval http {\n\
variable http\n\
array set http {\n\
-accept */*\n\
-proxyhost {}\n\
-proxyport {}\n\
-useragent {Tcl http client package 2.3}\n\
-proxyfilter http::ProxyRequired\n\
}\n\
\n\
variable formMap\n\
variable alphanumeric a-zA-Z0-9\n\
variable c\n\
variable i 0\n\
for {} {$i <= 256} {incr i} {\n\
set c [format %c $i]\n\
if {![string match \\[$alphanumeric\\] $c]} {\n\
set formMap($c) %[format %.2x $i]\n\
}\n\
}\n\
array set formMap {\n\
\" \" +   \\n %0d%0a\n\
}\n\
\n\
variable urlTypes\n\
array set urlTypes {\n\
http	{80 ::socket}\n\
}\n\
\n\
namespace export geturl config reset wait formatQuery register unregister\n\
}\n\
\n\
\n\
proc http::register {proto port command} {\n\
variable urlTypes\n\
set urlTypes($proto) [list $port $command]\n\
}\n\
\n\
\n\
proc http::unregister {proto} {\n\
variable urlTypes\n\
if {![info exists urlTypes($proto)]} {\n\
return -code error \"unsupported url type \\\"$proto\\\"\"\n\
}\n\
set old $urlTypes($proto)\n\
unset urlTypes($proto)\n\
return $old\n\
}\n\
\n\
\n\
proc http::config {args} {\n\
variable http\n\
set options [lsort [array names http -*]]\n\
set usage [join $options \", \"]\n\
if {[llength $args] == 0} {\n\
set result {}\n\
foreach name $options {\n\
lappend result $name $http($name)\n\
}\n\
return $result\n\
}\n\
regsub -all -- - $options {} options\n\
set pat ^-([join $options |])$\n\
if {[llength $args] == 1} {\n\
set flag [lindex $args 0]\n\
if {[regexp -- $pat $flag]} {\n\
return $http($flag)\n\
} else {\n\
return -code error \"Unknown option $flag, must be: $usage\"\n\
}\n\
} else {\n\
foreach {flag value} $args {\n\
if {[regexp -- $pat $flag]} {\n\
set http($flag) $value\n\
} else {\n\
return -code error \"Unknown option $flag, must be: $usage\"\n\
}\n\
}\n\
}\n\
}\n\
\n\
\n\
proc http::Finish { token {errormsg \"\"} {skipCB 0}} {\n\
variable $token\n\
upvar 0 $token state\n\
global errorInfo errorCode\n\
if {[string length $errormsg] != 0} {\n\
set state(error) [list $errormsg $errorInfo $errorCode]\n\
set state(status) error\n\
}\n\
catch {close $state(sock)}\n\
catch {after cancel $state(after)}\n\
if {[info exists state(-command)] && !$skipCB} {\n\
if {[catch {eval $state(-command) {$token}} err]} {\n\
if {[string length $errormsg] == 0} {\n\
set state(error) [list $err $errorInfo $errorCode]\n\
set state(status) error\n\
}\n\
}\n\
if {[info exist state(-command)]} {\n\
unset state(-command)\n\
}\n\
}\n\
}\n\
\n\
\n\
proc http::reset { token {why reset} } {\n\
variable $token\n\
upvar 0 $token state\n\
set state(status) $why\n\
catch {fileevent $state(sock) readable {}}\n\
catch {fileevent $state(sock) writable {}}\n\
Finish $token\n\
if {[info exists state(error)]} {\n\
set errorlist $state(error)\n\
unset state\n\
eval error $errorlist\n\
}\n\
}\n\
\n\
\n\
proc http::geturl { url args } {\n\
variable http\n\
variable urlTypes\n\
\n\
\n\
if {![info exists http(uid)]} {\n\
set http(uid) 0\n\
}\n\
set token [namespace current]::[incr http(uid)]\n\
variable $token\n\
upvar 0 $token state\n\
reset $token\n\
\n\
\n\
array set state {\n\
-blocksize 	8192\n\
-queryblocksize 8192\n\
-validate 	0\n\
-headers 	{}\n\
-timeout 	0\n\
-type           application/x-www-form-urlencoded\n\
-queryprogress	{}\n\
state		header\n\
meta		{}\n\
currentsize	0\n\
totalsize	0\n\
querylength	0\n\
queryoffset	0\n\
type            text/html\n\
body            {}\n\
status		\"\"\n\
http            \"\"\n\
}\n\
set options {-blocksize -channel -command -handler -headers \\\n\
-progress -query -queryblocksize -querychannel -queryprogress\\\n\
-validate -timeout -type}\n\
set usage [join $options \", \"]\n\
regsub -all -- - $options {} options\n\
set pat ^-([join $options |])$\n\
foreach {flag value} $args {\n\
if {[regexp $pat $flag]} {\n\
if {[info exists state($flag)] && \\\n\
[string is integer -strict $state($flag)] && \\\n\
![string is integer -strict $value]} {\n\
unset $token\n\
return -code error \"Bad value for $flag ($value), must be integer\"\n\
}\n\
set state($flag) $value\n\
} else {\n\
unset $token\n\
return -code error \"Unknown option $flag, can be: $usage\"\n\
}\n\
}\n\
\n\
\n\
set isQueryChannel [info exists state(-querychannel)]\n\
set isQuery [info exists state(-query)]\n\
if {$isQuery && $isQueryChannel} {\n\
unset $token\n\
return -code error \"Can't combine -query and -querychannel options!\"\n\
}\n\
\n\
\n\
if {![regexp -nocase {^(([^:]*)://)?([^/:]+)(:([0-9]+))?(/.*)?$} $url \\\n\
x prefix proto host y port srvurl]} {\n\
unset $token\n\
return -code error \"Unsupported URL: $url\"\n\
}\n\
if {[string length $proto] == 0} {\n\
set proto http\n\
set url ${proto}://$url\n\
}\n\
if {![info exists urlTypes($proto)]} {\n\
unset $token\n\
return -code error \"Unsupported URL type \\\"$proto\\\"\"\n\
}\n\
set defport [lindex $urlTypes($proto) 0]\n\
set defcmd [lindex $urlTypes($proto) 1]\n\
\n\
if {[string length $port] == 0} {\n\
set port $defport\n\
}\n\
if {[string length $srvurl] == 0} {\n\
set srvurl /\n\
}\n\
if {[string length $proto] == 0} {\n\
set url http://$url\n\
}\n\
set state(url) $url\n\
if {![catch {$http(-proxyfilter) $host} proxy]} {\n\
set phost [lindex $proxy 0]\n\
set pport [lindex $proxy 1]\n\
}\n\
\n\
\n\
if {$state(-timeout) > 0} {\n\
set state(after) [after $state(-timeout) \\\n\
[list http::reset $token timeout]]\n\
set async -async\n\
} else {\n\
set async \"\"\n\
}\n\
\n\
\n\
if {[info exists phost] && [string length $phost]} {\n\
set srvurl $url\n\
set conStat [catch {eval $defcmd $async {$phost $pport}} s]\n\
} else {\n\
set conStat [catch {eval $defcmd $async {$host $port}} s]\n\
}\n\
if {$conStat} {\n\
\n\
Finish $token \"\" 1\n\
cleanup $token\n\
return -code error $s\n\
}\n\
set state(sock) $s\n\
\n\
\n\
if {$state(-timeout) > 0} {\n\
fileevent $s writable [list http::Connect $token]\n\
http::wait $token\n\
\n\
if {[string equal $state(status) \"error\"]} {\n\
set err [lindex $state(error) 0]\n\
cleanup $token\n\
return -code error $err\n\
} elseif {![string equal $state(status) \"connect\"]} {\n\
return $token\n\
}\n\
set state(status) \"\"\n\
}\n\
\n\
\n\
fconfigure $s -translation {auto crlf} -buffersize $state(-blocksize)\n\
\n\
\n\
catch {fconfigure $s -blocking off}\n\
set how GET\n\
if {$isQuery} {\n\
set state(querylength) [string length $state(-query)]\n\
if {$state(querylength) > 0} {\n\
set how POST\n\
set contDone 0\n\
} else {\n\
unset state(-query)\n\
set isQuery 0\n\
}\n\
} elseif {$state(-validate)} {\n\
set how HEAD\n\
} elseif {$isQueryChannel} {\n\
set how POST\n\
fconfigure $state(-querychannel) -blocking 1 -translation binary\n\
set contDone 0\n\
}\n\
\n\
if {[catch {\n\
puts $s \"$how $srvurl HTTP/1.0\"\n\
puts $s \"Accept: $http(-accept)\"\n\
puts $s \"Host: $host\"\n\
puts $s \"User-Agent: $http(-useragent)\"\n\
foreach {key value} $state(-headers) {\n\
regsub -all \\[\\n\\r\\]  $value {} value\n\
set key [string trim $key]\n\
if {[string equal $key \"Content-Length\"]} {\n\
set contDone 1\n\
set state(querylength) $value\n\
}\n\
if {[string length $key]} {\n\
puts $s \"$key: $value\"\n\
}\n\
}\n\
if {$isQueryChannel && $state(querylength) == 0} {\n\
\n\
set start [tell $state(-querychannel)]\n\
seek $state(-querychannel) 0 end\n\
set state(querylength) \\\n\
[expr {[tell $state(-querychannel)] - $start}]\n\
seek $state(-querychannel) $start\n\
}\n\
\n\
\n\
if {$isQuery || $isQueryChannel} {\n\
puts $s \"Content-Type: $state(-type)\"\n\
if {!$contDone} {\n\
puts $s \"Content-Length: $state(querylength)\"\n\
}\n\
puts $s \"\"\n\
fconfigure $s -translation {auto binary}\n\
fileevent $s writable [list http::Write $token]\n\
} else {\n\
puts $s \"\"\n\
flush $s\n\
fileevent $s readable [list http::Event $token]\n\
}\n\
\n\
if {! [info exists state(-command)]} {\n\
\n\
\n\
wait $token\n\
if {[string equal $state(status) \"error\"]} {\n\
return -code error [lindex $state(error) 0]\n\
}		\n\
}\n\
} err]} {\n\
\n\
\n\
if {[string equal $state(status) \"error\"]} {\n\
Finish $token $err 1\n\
}\n\
cleanup $token\n\
return -code error $err\n\
}\n\
\n\
return $token\n\
}\n\
\n\
\n\
proc http::data {token} {\n\
variable $token\n\
upvar 0 $token state\n\
return $state(body)\n\
}\n\
proc http::status {token} {\n\
variable $token\n\
upvar 0 $token state\n\
return $state(status)\n\
}\n\
proc http::code {token} {\n\
variable $token\n\
upvar 0 $token state\n\
return $state(http)\n\
}\n\
proc http::ncode {token} {\n\
variable $token\n\
upvar 0 $token state\n\
if {[regexp {[0-9]{3}} $state(http) numeric_code]} {\n\
return $numeric_code\n\
} else {\n\
return $state(http)\n\
}\n\
}\n\
proc http::size {token} {\n\
variable $token\n\
upvar 0 $token state\n\
return $state(currentsize)\n\
}\n\
\n\
proc http::error {token} {\n\
variable $token\n\
upvar 0 $token state\n\
if {[info exists state(error)]} {\n\
return $state(error)\n\
}\n\
return \"\"\n\
}\n\
\n\
\n\
proc http::cleanup {token} {\n\
variable $token\n\
upvar 0 $token state\n\
if {[info exist state]} {\n\
unset state\n\
}\n\
}\n\
\n\
\n\
proc http::Connect {token} {\n\
variable $token\n\
upvar 0 $token state\n\
global errorInfo errorCode\n\
if {[eof $state(sock)] ||\n\
[string length [fconfigure $state(sock) -error]]} {\n\
Finish $token \"connect failed [fconfigure $state(sock) -error]\" 1\n\
} else {\n\
set state(status) connect\n\
fileevent $state(sock) writable {}\n\
}\n\
return\n\
}\n\
\n\
\n\
proc http::Write {token} {\n\
variable $token\n\
upvar 0 $token state\n\
set s $state(sock)\n\
\n\
\n\
set done 0\n\
if {[catch {\n\
\n\
\n\
if {[info exists state(-query)]} {\n\
\n\
\n\
puts -nonewline $s \\\n\
[string range $state(-query) $state(queryoffset) \\\n\
[expr {$state(queryoffset) + $state(-queryblocksize) - 1}]]\n\
incr state(queryoffset) $state(-queryblocksize)\n\
if {$state(queryoffset) >= $state(querylength)} {\n\
set state(queryoffset) $state(querylength)\n\
set done 1\n\
}\n\
} else {\n\
\n\
\n\
set outStr [read $state(-querychannel) $state(-queryblocksize)]\n\
puts -nonewline $s $outStr\n\
incr state(queryoffset) [string length $outStr]\n\
if {[eof $state(-querychannel)]} {\n\
set done 1\n\
}\n\
}\n\
} err]} {\n\
\n\
set state(posterror) $err\n\
set done 1\n\
}\n\
if {$done} {\n\
catch {flush $s}\n\
fileevent $s writable {}\n\
fileevent $s readable [list http::Event $token]\n\
}\n\
\n\
\n\
if {[string length $state(-queryprogress)]} {\n\
eval $state(-queryprogress) [list $token $state(querylength)\\\n\
$state(queryoffset)]\n\
}\n\
}\n\
\n\
\n\
proc http::Event {token} {\n\
variable $token\n\
upvar 0 $token state\n\
set s $state(sock)\n\
\n\
if {[eof $s]} {\n\
Eof $token\n\
return\n\
}\n\
if {[string equal $state(state) \"header\"]} {\n\
if {[catch {gets $s line} n]} {\n\
Finish $token $n\n\
} elseif {$n == 0} {\n\
set state(state) body\n\
if {![regexp -nocase ^text $state(type)]} {\n\
fconfigure $s -translation binary\n\
if {[info exists state(-channel)]} {\n\
fconfigure $state(-channel) -translation binary\n\
}\n\
}\n\
if {[info exists state(-channel)] &&\n\
![info exists state(-handler)]} {\n\
fileevent $s readable {}\n\
CopyStart $s $token\n\
}\n\
} elseif {$n > 0} {\n\
if {[regexp -nocase {^content-type:(.+)$} $line x type]} {\n\
set state(type) [string trim $type]\n\
}\n\
if {[regexp -nocase {^content-length:(.+)$} $line x length]} {\n\
set state(totalsize) [string trim $length]\n\
}\n\
if {[regexp -nocase {^([^:]+):(.+)$} $line x key value]} {\n\
lappend state(meta) $key [string trim $value]\n\
} elseif {[regexp ^HTTP $line]} {\n\
set state(http) $line\n\
}\n\
}\n\
} else {\n\
if {[catch {\n\
if {[info exists state(-handler)]} {\n\
set n [eval $state(-handler) {$s $token}]\n\
} else {\n\
set block [read $s $state(-blocksize)]\n\
set n [string length $block]\n\
if {$n >= 0} {\n\
append state(body) $block\n\
}\n\
}\n\
if {$n >= 0} {\n\
incr state(currentsize) $n\n\
}\n\
} err]} {\n\
Finish $token $err\n\
} else {\n\
if {[info exists state(-progress)]} {\n\
eval $state(-progress) {$token $state(totalsize) $state(currentsize)}\n\
}\n\
}\n\
}\n\
}\n\
\n\
\n\
proc http::CopyStart {s token} {\n\
variable $token\n\
upvar 0 $token state\n\
if {[catch {\n\
fcopy $s $state(-channel) -size $state(-blocksize) -command \\\n\
[list http::CopyDone $token]\n\
} err]} {\n\
Finish $token $err\n\
}\n\
}\n\
\n\
\n\
proc http::CopyDone {token count {error {}}} {\n\
variable $token\n\
upvar 0 $token state\n\
set s $state(sock)\n\
incr state(currentsize) $count\n\
if {[info exists state(-progress)]} {\n\
eval $state(-progress) {$token $state(totalsize) $state(currentsize)}\n\
}\n\
if {[string length $error]} {\n\
Finish $token $error\n\
} elseif {[catch {eof $s} iseof] || $iseof} {\n\
Eof $token\n\
} else {\n\
CopyStart $s $token\n\
}\n\
}\n\
\n\
\n\
proc http::Eof {token} {\n\
variable $token\n\
upvar 0 $token state\n\
if {[string equal $state(state) \"header\"]} {\n\
set state(status) eof\n\
} else {\n\
set state(status) ok\n\
}\n\
set state(state) eof\n\
Finish $token\n\
}\n\
\n\
\n\
proc http::wait {token} {\n\
variable $token\n\
upvar 0 $token state\n\
\n\
if {![info exists state(status)] || [string length $state(status)] == 0} {\n\
vwait $token\\(status)\n\
}\n\
\n\
return $state(status)\n\
}\n\
\n\
\n\
proc http::formatQuery {args} {\n\
set result \"\"\n\
set sep \"\"\n\
foreach i $args {\n\
append result $sep [mapReply $i]\n\
if {[string compare $sep \"=\"]} {\n\
set sep =\n\
} else {\n\
set sep &\n\
}\n\
}\n\
return $result\n\
}\n\
\n\
\n\
proc http::mapReply {string} {\n\
variable formMap\n\
\n\
\n\
set alphanumeric	a-zA-Z0-9\n\
regsub -all \\[^$alphanumeric\\] $string {$formMap(&)} string\n\
regsub -all \\n $string {\\\\n} string\n\
regsub -all \\t $string {\\\\t} string\n\
regsub -all {[][{})\\\\]\\)} $string {\\\\&} string\n\
return [subst $string]\n\
}\n\
\n\
\n\
proc http::ProxyRequired {host} {\n\
variable http\n\
if {[info exists http(-proxyhost)] && [string length $http(-proxyhost)]} {\n\
if {![info exists http(-proxyport)] || ![string length $http(-proxyport)]} {\n\
set http(-proxyport) 8080\n\
}\n\
return [list $http(-proxyhost) $http(-proxyport)]\n\
} else {\n\
return {}\n\
}\n\
}\n\
\n\
\n\
package require -exact Tk 8.3\n\
package require -exact Tcl 8.3\n\
\n\
\n\
if {[info exists auto_path] && [string compare {} $tk_library] && \\\n\
[lsearch -exact $auto_path $tk_library] < 0} {\n\
lappend auto_path $tk_library\n\
}\n\
\n\
\n\
set tk_strictMotif 0\n\
\n\
\n\
namespace eval ::tk {\n\
}\n\
\n\
proc ::tk::PlaceWindow {w {place \"\"} {anchor \"\"}} {\n\
wm withdraw $w\n\
update idletasks\n\
set checkBounds 1\n\
if {[string equal -len [string length $place] $place \"pointer\"]} {\n\
if {[string equal -len [string length $anchor] $anchor \"center\"]} {\n\
set x [expr {[winfo pointerx $w]-[winfo reqwidth $w]/2}]\n\
set y [expr {[winfo pointery $w]-[winfo reqheight $w]/2}]\n\
} else {\n\
set x [winfo pointerx $w]\n\
set y [winfo pointery $w]\n\
}\n\
} elseif {[string equal -len [string length $place] $place \"widget\"] && \\\n\
[winfo exists $anchor] && [winfo ismapped $anchor]} {\n\
set x [expr {[winfo rootx $anchor] + \\\n\
([winfo width $anchor]-[winfo reqwidth $w])/2}]\n\
set y [expr {[winfo rooty $anchor] + \\\n\
([winfo height $anchor]-[winfo reqheight $w])/2}]\n\
} else {\n\
set x [expr {([winfo screenwidth $w]-[winfo reqwidth $w])/2}]\n\
set y [expr {([winfo screenheight $w]-[winfo reqheight $w])/2}]\n\
set checkBounds 0\n\
}\n\
if {$checkBounds} {\n\
if {$x < 0} {\n\
set x 0\n\
} elseif {$x > ([winfo screenwidth $w]-[winfo reqwidth $w])} {\n\
set x [expr {[winfo screenwidth $w]-[winfo reqwidth $w]}]\n\
}\n\
if {$y < 0} {\n\
set y 0\n\
} elseif {$y > ([winfo screenheight $w]-[winfo reqheight $w])} {\n\
set y [expr {[winfo screenheight $w]-[winfo reqheight $w]}]\n\
}\n\
}\n\
wm geometry $w +$x+$y\n\
wm deiconify $w\n\
}\n\
\n\
proc ::tk::SetFocusGrab {grab {focus {}}} {\n\
set index \"$grab,$focus\"\n\
upvar ::tk::FocusGrab($index) data\n\
\n\
lappend data [focus]\n\
set oldGrab [grab current $grab]\n\
lappend data $oldGrab\n\
if {[winfo exists $oldGrab]} {\n\
lappend data [grab status $oldGrab]\n\
}\n\
grab $grab\n\
if {[winfo exists $focus]} {\n\
focus $focus\n\
}\n\
}\n\
\n\
proc ::tk::RestoreFocusGrab {grab focus {destroy destroy}} {\n\
set index \"$grab,$focus\"\n\
foreach {oldFocus oldGrab oldStatus} $::tk::FocusGrab($index) { break }\n\
unset ::tk::FocusGrab($index)\n\
\n\
catch {focus $oldFocus}\n\
grab release $grab\n\
if {[string equal $destroy \"withdraw\"]} {\n\
wm withdraw $grab\n\
} else {\n\
destroy $grab\n\
}\n\
if {[winfo exists $oldGrab] && [winfo ismapped $oldGrab]} {\n\
if {[string equal $oldStatus \"global\"]} {\n\
grab -global $oldGrab\n\
} else {\n\
grab $oldGrab\n\
}\n\
}\n\
}\n\
\n\
\n\
proc tkScreenChanged screen {\n\
set x [string last . $screen]\n\
if {$x > 0} {\n\
set disp [string range $screen 0 [expr {$x - 1}]]\n\
} else {\n\
set disp $screen\n\
}\n\
\n\
uplevel #0 upvar #0 tkPriv.$disp tkPriv\n\
global tkPriv\n\
global tcl_platform\n\
\n\
if {[info exists tkPriv]} {\n\
set tkPriv(screen) $screen\n\
return\n\
}\n\
array set tkPriv {\n\
activeMenu	{}\n\
activeItem	{}\n\
afterId		{}\n\
buttons		0\n\
buttonWindow	{}\n\
dragging	0\n\
focus		{}\n\
grab		{}\n\
initPos		{}\n\
inMenubutton	{}\n\
listboxPrev	{}\n\
menuBar		{}\n\
mouseMoved	0\n\
oldGrab		{}\n\
popup		{}\n\
postedMb	{}\n\
pressX		0\n\
pressY		0\n\
prevPos		0\n\
selectMode	char\n\
}\n\
set tkPriv(screen) $screen\n\
set tkPriv(tearoff) [string equal $tcl_platform(platform) \"unix\"]\n\
set tkPriv(window) {}\n\
}\n\
\n\
\n\
tkScreenChanged [winfo screen .]\n\
\n\
\n\
proc tkEventMotifBindings {n1 dummy dummy} {\n\
upvar $n1 name\n\
\n\
if {$name} {\n\
set op delete\n\
} else {\n\
set op add\n\
}\n\
\n\
event $op <<Cut>> <Control-Key-w>\n\
event $op <<Copy>> <Meta-Key-w> \n\
event $op <<Paste>> <Control-Key-y>\n\
}\n\
\n\
\n\
if {[string equal [info commands tk_chooseColor] \"\"]} {\n\
proc tk_chooseColor {args} {\n\
return [eval tkColorDialog $args]\n\
}\n\
}\n\
if {[string equal [info commands tk_getOpenFile] \"\"]} {\n\
proc tk_getOpenFile {args} {\n\
if {$::tk_strictMotif} {\n\
return [eval tkMotifFDialog open $args]\n\
} else {\n\
return [eval ::tk::dialog::file::tkFDialog open $args]\n\
}\n\
}\n\
}\n\
if {[string equal [info commands tk_getSaveFile] \"\"]} {\n\
proc tk_getSaveFile {args} {\n\
if {$::tk_strictMotif} {\n\
return [eval tkMotifFDialog save $args]\n\
} else {\n\
return [eval ::tk::dialog::file::tkFDialog save $args]\n\
}\n\
}\n\
}\n\
if {[string equal [info commands tk_messageBox] \"\"]} {\n\
proc tk_messageBox {args} {\n\
return [eval tkMessageBox $args]\n\
}\n\
}\n\
if {[string equal [info command tk_chooseDirectory] \"\"]} {\n\
proc tk_chooseDirectory {args} {\n\
return [eval ::tk::dialog::file::chooseDir::tkChooseDirectory $args]\n\
}\n\
}\n\
\n\
\n\
switch $tcl_platform(platform) {\n\
\"unix\" {\n\
event add <<Cut>> <Control-Key-x> <Key-F20> \n\
event add <<Copy>> <Control-Key-c> <Key-F16>\n\
event add <<Paste>> <Control-Key-v> <Key-F18>\n\
event add <<PasteSelection>> <ButtonRelease-2>\n\
if {[info exists tcl_platform(os)]} {\n\
switch $tcl_platform(os) {\n\
\"IRIX\"  -\n\
\"Linux\" { event add <<PrevWindow>> <ISO_Left_Tab> }\n\
\"HP-UX\" { event add <<PrevWindow>> <hpBackTab> }\n\
}\n\
}\n\
trace variable tk_strictMotif w tkEventMotifBindings\n\
set tk_strictMotif $tk_strictMotif\n\
}\n\
\"windows\" {\n\
event add <<Cut>> <Control-Key-x> <Shift-Key-Delete>\n\
event add <<Copy>> <Control-Key-c> <Control-Key-Insert>\n\
event add <<Paste>> <Control-Key-v> <Shift-Key-Insert>\n\
event add <<PasteSelection>> <ButtonRelease-2>\n\
}\n\
\"macintosh\" {\n\
event add <<Cut>> <Control-Key-x> <Key-F2> \n\
event add <<Copy>> <Control-Key-c> <Key-F3>\n\
event add <<Paste>> <Control-Key-v> <Key-F4>\n\
event add <<PasteSelection>> <ButtonRelease-2>\n\
event add <<Clear>> <Clear>\n\
}\n\
}\n\
\n\
\n\
if {[string compare $tcl_platform(platform) \"macintosh\"] && \\\n\
[string compare {} $tk_library]} {\n\
}\n\
\n\
\n\
event add <<PrevWindow>> <Shift-Tab>\n\
bind all <Tab> {tkTabToWindow [tk_focusNext %W]}\n\
bind all <<PrevWindow>> {tkTabToWindow [tk_focusPrev %W]}\n\
\n\
\n\
proc tkCancelRepeat {} {\n\
global tkPriv\n\
after cancel $tkPriv(afterId)\n\
set tkPriv(afterId) {}\n\
}\n\
\n\
\n\
proc tkTabToWindow {w} {\n\
if {[string equal [winfo class $w] Entry]} {\n\
$w selection range 0 end\n\
$w icursor end\n\
}\n\
focus $w\n\
}\n\
\n\
proc tkColorDialog {args} {\n\
global tkPriv\n\
set w .__tk__color\n\
upvar #0 $w data\n\
\n\
set data(lines,red,start)   0\n\
set data(lines,red,last)   -1\n\
set data(lines,green,start) 0\n\
set data(lines,green,last) -1\n\
set data(lines,blue,start)  0\n\
set data(lines,blue,last)  -1\n\
\n\
set data(NUM_COLORBARS) 8\n\
\n\
set data(BARS_WIDTH) 128\n\
\n\
set data(PLGN_HEIGHT) 10\n\
\n\
set data(PLGN_WIDTH) 10\n\
\n\
tkColorDialog_Config $w $args\n\
tkColorDialog_InitValues $w\n\
\n\
set sc [winfo screen $data(-parent)]\n\
set winExists [winfo exists $w]\n\
if {!$winExists || [string compare $sc [winfo screen $w]]} {\n\
if {$winExists} {\n\
destroy $w\n\
}\n\
toplevel $w -class tkColorDialog -screen $sc\n\
tkColorDialog_BuildDialog $w\n\
}\n\
\n\
wm transient $w $data(-parent)\n\
\n\
\n\
::tk::PlaceWindow $w widget $data(-parent)\n\
wm title $w $data(-title)\n\
\n\
\n\
::tk::SetFocusGrab $w $data(okBtn)\n\
\n\
\n\
vwait tkPriv(selectColor)\n\
::tk::RestoreFocusGrab $w $data(okBtn)\n\
unset data\n\
\n\
return $tkPriv(selectColor)\n\
}\n\
\n\
proc tkColorDialog_InitValues {w} {\n\
upvar #0 $w data\n\
\n\
set data(intensityIncr) [expr {256 / $data(NUM_COLORBARS)}]\n\
\n\
set data(colorbarWidth) \\\n\
[expr {$data(BARS_WIDTH) / $data(NUM_COLORBARS)}]\n\
\n\
set data(indent) [expr {$data(PLGN_WIDTH) / 2}]\n\
\n\
set data(colorPad) 2\n\
set data(selPad)   [expr {$data(PLGN_WIDTH) / 2}]\n\
\n\
set data(minX) $data(indent)\n\
\n\
set data(maxX) [expr {$data(BARS_WIDTH) + $data(indent)-1}]\n\
\n\
set data(canvasWidth) [expr {$data(BARS_WIDTH) + $data(PLGN_WIDTH)}]\n\
\n\
set data(selection) $data(-initialcolor)\n\
set data(finalColor)  $data(-initialcolor)\n\
set rgb [winfo rgb . $data(selection)]\n\
\n\
set data(red,intensity)   [expr {[lindex $rgb 0]/0x100}]\n\
set data(green,intensity) [expr {[lindex $rgb 1]/0x100}]\n\
set data(blue,intensity)  [expr {[lindex $rgb 2]/0x100}]\n\
}\n\
\n\
proc tkColorDialog_Config {w argList} {\n\
global tkPriv\n\
upvar #0 $w data\n\
\n\
if {[info exists tkPriv(selectColor)] && \\\n\
[string compare $tkPriv(selectColor) \"\"]} {\n\
set defaultColor $tkPriv(selectColor)\n\
} else {\n\
set defaultColor [. cget -background]\n\
}\n\
\n\
set specs [list \\\n\
[list -initialcolor \"\" \"\" $defaultColor] \\\n\
[list -parent \"\" \"\" \".\"] \\\n\
[list -title \"\" \"\" \"Color\"] \\\n\
]\n\
\n\
tclParseConfigSpec $w $specs \"\" $argList\n\
\n\
if {[string equal $data(-title) \"\"]} {\n\
set data(-title) \" \"\n\
}\n\
if {[catch {winfo rgb . $data(-initialcolor)} err]} {\n\
error $err\n\
}\n\
\n\
if {![winfo exists $data(-parent)]} {\n\
error \"bad window path name \\\"$data(-parent)\\\"\"\n\
}\n\
}\n\
\n\
proc tkColorDialog_BuildDialog {w} {\n\
upvar #0 $w data\n\
\n\
set topFrame [frame $w.top -relief raised -bd 1]\n\
\n\
set stripsFrame [frame $topFrame.colorStrip]\n\
\n\
foreach c { Red Green Blue } {\n\
set color [string tolower $c]\n\
\n\
set f [frame $stripsFrame.$color]\n\
\n\
set box [frame $f.box]\n\
\n\
label $box.label -text $c: -width 6 -under 0 -anchor ne\n\
entry $box.entry -textvariable [format %s $w]($color,intensity) \\\n\
-width 4\n\
pack $box.label -side left -fill y -padx 2 -pady 3\n\
pack $box.entry -side left -anchor n -pady 0\n\
pack $box -side left -fill both\n\
\n\
set height [expr \\\n\
{[winfo reqheight $box.entry] - \\\n\
2*([$box.entry cget -highlightthickness] + [$box.entry cget -bd])}]\n\
\n\
canvas $f.color -height $height\\\n\
-width $data(BARS_WIDTH) -relief sunken -bd 2\n\
canvas $f.sel -height $data(PLGN_HEIGHT) \\\n\
-width $data(canvasWidth) -highlightthickness 0\n\
pack $f.color -expand yes -fill both\n\
pack $f.sel -expand yes -fill both\n\
\n\
pack $f -side top -fill x -padx 0 -pady 2\n\
\n\
set data($color,entry) $box.entry\n\
set data($color,col) $f.color\n\
set data($color,sel) $f.sel\n\
\n\
bind $data($color,col) <Configure> \\\n\
[list tkColorDialog_DrawColorScale $w $color 1]\n\
bind $data($color,col) <Enter> \\\n\
[list tkColorDialog_EnterColorBar $w $color]\n\
bind $data($color,col) <Leave> \\\n\
[list tkColorDialog_LeaveColorBar $w $color]\n\
\n\
bind $data($color,sel) <Enter> \\\n\
[list tkColorDialog_EnterColorBar $w $color]\n\
bind $data($color,sel) <Leave> \\\n\
[list tkColorDialog_LeaveColorBar $w $color]\n\
\n\
bind $box.entry <Return> [list tkColorDialog_HandleRGBEntry $w]\n\
}\n\
\n\
pack $stripsFrame -side left -fill both -padx 4 -pady 10\n\
\n\
set selFrame [frame $topFrame.sel]\n\
set lab [label $selFrame.lab -text \"Selection:\" -under 0 -anchor sw]\n\
set ent [entry $selFrame.ent -textvariable [format %s $w](selection) \\\n\
-width 16]\n\
set f1  [frame $selFrame.f1 -relief sunken -bd 2]\n\
set data(finalCanvas) [frame $f1.demo -bd 0 -width 100 -height 70]\n\
\n\
pack $lab $ent -side top -fill x -padx 4 -pady 2\n\
pack $f1 -expand yes -anchor nw -fill both -padx 6 -pady 10\n\
pack $data(finalCanvas) -expand yes -fill both\n\
\n\
bind $ent <Return> [list tkColorDialog_HandleSelEntry $w]\n\
\n\
pack $selFrame -side left -fill none -anchor nw\n\
pack $topFrame -side top -expand yes -fill both -anchor nw\n\
\n\
set botFrame [frame $w.bot -relief raised -bd 1]\n\
button $botFrame.ok     -text OK            -width 8 -under 0 \\\n\
-command [list tkColorDialog_OkCmd $w]\n\
button $botFrame.cancel -text Cancel        -width 8 -under 0 \\\n\
-command [list tkColorDialog_CancelCmd $w]\n\
\n\
set data(okBtn)      $botFrame.ok\n\
set data(cancelBtn)  $botFrame.cancel\n\
\n\
pack $botFrame.ok $botFrame.cancel \\\n\
-padx 10 -pady 10 -expand yes -side left\n\
pack $botFrame -side bottom -fill x\n\
\n\
\n\
\n\
bind $w <Alt-r> [list focus $data(red,entry)]\n\
bind $w <Alt-g> [list focus $data(green,entry)]\n\
bind $w <Alt-b> [list focus $data(blue,entry)]\n\
bind $w <Alt-s> [list focus $ent]\n\
bind $w <KeyPress-Escape> [list tkButtonInvoke $data(cancelBtn)]\n\
bind $w <Alt-c> [list tkButtonInvoke $data(cancelBtn)]\n\
bind $w <Alt-o> [list tkButtonInvoke $data(okBtn)]\n\
\n\
wm protocol $w WM_DELETE_WINDOW [list tkColorDialog_CancelCmd $w]\n\
}\n\
\n\
proc tkColorDialog_SetRGBValue {w color} {\n\
upvar #0 $w data \n\
\n\
set data(red,intensity)   [lindex $color 0]\n\
set data(green,intensity) [lindex $color 1]\n\
set data(blue,intensity)  [lindex $color 2]\n\
\n\
tkColorDialog_RedrawColorBars $w all\n\
\n\
foreach color { red green blue } {\n\
set x [tkColorDialog_RgbToX $w $data($color,intensity)]\n\
tkColorDialog_MoveSelector $w $data($color,sel) $color $x 0\n\
}\n\
}\n\
\n\
proc tkColorDialog_XToRgb {w x} {\n\
upvar #0 $w data\n\
\n\
return [expr {($x * $data(intensityIncr))/ $data(colorbarWidth)}]\n\
}\n\
\n\
proc tkColorDialog_RgbToX {w color} {\n\
upvar #0 $w data\n\
\n\
return [expr {($color * $data(colorbarWidth)/ $data(intensityIncr))}]\n\
}\n\
\n\
\n\
proc tkColorDialog_DrawColorScale {w c {create 0}} {\n\
global lines\n\
upvar #0 $w data\n\
\n\
set col $data($c,col)\n\
set sel $data($c,sel)\n\
\n\
if {$create} {\n\
if { $data(lines,$c,last) > $data(lines,$c,start)} {\n\
for {set i $data(lines,$c,start)} \\\n\
{$i <= $data(lines,$c,last)} { incr i} {\n\
$sel delete $i\n\
}\n\
}\n\
if {[info exists data($c,index)]} {\n\
$sel delete $data($c,index)\n\
}\n\
\n\
tkColorDialog_CreateSelector $w $sel $c\n\
$sel bind $data($c,index) <ButtonPress-1> \\\n\
[list tkColorDialog_StartMove $w $sel $c %x $data(selPad) 1]\n\
$sel bind $data($c,index) <B1-Motion> \\\n\
[list tkColorDialog_MoveSelector $w $sel $c %x $data(selPad)]\n\
$sel bind $data($c,index) <ButtonRelease-1> \\\n\
[list tkColorDialog_ReleaseMouse $w $sel $c %x $data(selPad)]\n\
\n\
set height [winfo height $col]\n\
set data($c,clickRegion) [$sel create rectangle 0 0 \\\n\
$data(canvasWidth) $height -fill {} -outline {}]\n\
\n\
bind $col <ButtonPress-1> \\\n\
[list tkColorDialog_StartMove $w $sel $c %x $data(colorPad)]\n\
bind $col <B1-Motion> \\\n\
[list tkColorDialog_MoveSelector $w $sel $c %x $data(colorPad)]\n\
bind $col <ButtonRelease-1> \\\n\
[list tkColorDialog_ReleaseMouse $w $sel $c %x $data(colorPad)]\n\
\n\
$sel bind $data($c,clickRegion) <ButtonPress-1> \\\n\
[list tkColorDialog_StartMove $w $sel $c %x $data(selPad)]\n\
$sel bind $data($c,clickRegion) <B1-Motion> \\\n\
[list tkColorDialog_MoveSelector $w $sel $c %x $data(selPad)]\n\
$sel bind $data($c,clickRegion) <ButtonRelease-1> \\\n\
[list tkColorDialog_ReleaseMouse $w $sel $c %x $data(selPad)]\n\
} else {\n\
set l $data(lines,$c,start)\n\
}\n\
\n\
set highlightW [expr {[$col cget -highlightthickness] + [$col cget -bd]}]\n\
for {set i 0} { $i < $data(NUM_COLORBARS)} { incr i} {\n\
set intensity [expr {$i * $data(intensityIncr)}]\n\
set startx [expr {$i * $data(colorbarWidth) + $highlightW}]\n\
if {[string equal $c \"red\"]} {\n\
set color [format \"#%02x%02x%02x\" \\\n\
$intensity \\\n\
$data(green,intensity) \\\n\
$data(blue,intensity)]\n\
} elseif {[string equal $c \"green\"]} {\n\
set color [format \"#%02x%02x%02x\" \\\n\
$data(red,intensity) \\\n\
$intensity \\\n\
$data(blue,intensity)]\n\
} else {\n\
set color [format \"#%02x%02x%02x\" \\\n\
$data(red,intensity) \\\n\
$data(green,intensity) \\\n\
$intensity]\n\
}\n\
\n\
if {$create} {\n\
set index [$col create rect $startx $highlightW \\\n\
[expr {$startx +$data(colorbarWidth)}] \\\n\
[expr {[winfo height $col] + $highlightW}]\\\n\
-fill $color -outline $color]\n\
} else {\n\
$col itemconfigure $l -fill $color -outline $color\n\
incr l\n\
}\n\
}\n\
$sel raise $data($c,index)\n\
\n\
if {$create} {\n\
set data(lines,$c,last) $index\n\
set data(lines,$c,start) [expr {$index - $data(NUM_COLORBARS) + 1}]\n\
}\n\
\n\
tkColorDialog_RedrawFinalColor $w\n\
}\n\
\n\
proc tkColorDialog_CreateSelector {w sel c } {\n\
upvar #0 $w data\n\
set data($c,index) [$sel create polygon \\\n\
0 $data(PLGN_HEIGHT) \\\n\
$data(PLGN_WIDTH) $data(PLGN_HEIGHT) \\\n\
$data(indent) 0]\n\
set data($c,x) [tkColorDialog_RgbToX $w $data($c,intensity)]\n\
$sel move $data($c,index) $data($c,x) 0\n\
}\n\
\n\
proc tkColorDialog_RedrawFinalColor {w} {\n\
upvar #0 $w data\n\
\n\
set color [format \"#%02x%02x%02x\" $data(red,intensity) \\\n\
$data(green,intensity) $data(blue,intensity)]\n\
\n\
$data(finalCanvas) configure -bg $color\n\
set data(finalColor) $color\n\
set data(selection) $color\n\
set data(finalRGB) [list \\\n\
$data(red,intensity) \\\n\
$data(green,intensity) \\\n\
$data(blue,intensity)]\n\
}\n\
\n\
proc tkColorDialog_RedrawColorBars {w colorChanged} {\n\
upvar #0 $w data\n\
\n\
switch $colorChanged {\n\
red { \n\
tkColorDialog_DrawColorScale $w green\n\
tkColorDialog_DrawColorScale $w blue\n\
}\n\
green {\n\
tkColorDialog_DrawColorScale $w red\n\
tkColorDialog_DrawColorScale $w blue\n\
}\n\
blue {\n\
tkColorDialog_DrawColorScale $w red\n\
tkColorDialog_DrawColorScale $w green\n\
}\n\
default {\n\
tkColorDialog_DrawColorScale $w red\n\
tkColorDialog_DrawColorScale $w green\n\
tkColorDialog_DrawColorScale $w blue\n\
}\n\
}\n\
tkColorDialog_RedrawFinalColor $w\n\
}\n\
\n\
\n\
proc tkColorDialog_StartMove {w sel color x delta {dontMove 0}} {\n\
upvar #0 $w data\n\
\n\
if {!$dontMove} {\n\
tkColorDialog_MoveSelector $w $sel $color $x $delta\n\
}\n\
}\n\
\n\
proc tkColorDialog_MoveSelector {w sel color x delta} {\n\
upvar #0 $w data\n\
\n\
incr x -$delta\n\
\n\
if { $x < 0 } {\n\
set x 0\n\
} elseif { $x >= $data(BARS_WIDTH)} {\n\
set x [expr {$data(BARS_WIDTH) - 1}]\n\
}\n\
set diff [expr {$x - $data($color,x)}]\n\
$sel move $data($color,index) $diff 0\n\
set data($color,x) [expr {$data($color,x) + $diff}]\n\
\n\
return $x\n\
}\n\
\n\
proc tkColorDialog_ReleaseMouse {w sel color x delta} {\n\
upvar #0 $w data \n\
\n\
set x [tkColorDialog_MoveSelector $w $sel $color $x $delta]\n\
\n\
set data($color,intensity) [tkColorDialog_XToRgb $w $x]\n\
\n\
tkColorDialog_RedrawColorBars $w $color\n\
}\n\
\n\
proc tkColorDialog_ResizeColorBars {w} {\n\
upvar #0 $w data\n\
\n\
if { ($data(BARS_WIDTH) < $data(NUM_COLORBARS)) || \n\
(($data(BARS_WIDTH) % $data(NUM_COLORBARS)) != 0)} {\n\
set data(BARS_WIDTH) $data(NUM_COLORBARS)\n\
}\n\
tkColorDialog_InitValues $w\n\
foreach color { red green blue } {\n\
$data($color,col) configure -width $data(canvasWidth)\n\
tkColorDialog_DrawColorScale $w $color 1\n\
}\n\
}\n\
\n\
proc tkColorDialog_HandleSelEntry {w} {\n\
upvar #0 $w data\n\
\n\
set text [string trim $data(selection)]\n\
if {[catch {set color [winfo rgb . $text]} ]} {\n\
set data(selection) $data(finalColor)\n\
return\n\
}\n\
\n\
set R [expr {[lindex $color 0]/0x100}]\n\
set G [expr {[lindex $color 1]/0x100}]\n\
set B [expr {[lindex $color 2]/0x100}]\n\
\n\
tkColorDialog_SetRGBValue $w \"$R $G $B\"\n\
set data(selection) $text\n\
}\n\
\n\
proc tkColorDialog_HandleRGBEntry {w} {\n\
upvar #0 $w data\n\
\n\
foreach c {red green blue} {\n\
if {[catch {\n\
set data($c,intensity) [expr {int($data($c,intensity))}]\n\
}]} {\n\
set data($c,intensity) 0\n\
}\n\
\n\
if {$data($c,intensity) < 0} {\n\
set data($c,intensity) 0\n\
}\n\
if {$data($c,intensity) > 255} {\n\
set data($c,intensity) 255\n\
}\n\
}\n\
\n\
tkColorDialog_SetRGBValue $w \"$data(red,intensity) $data(green,intensity) \\\n\
$data(blue,intensity)\"\n\
}    \n\
\n\
proc tkColorDialog_EnterColorBar {w color} {\n\
upvar #0 $w data\n\
\n\
$data($color,sel) itemconfig $data($color,index) -fill red\n\
}\n\
\n\
proc tkColorDialog_LeaveColorBar {w color} {\n\
upvar #0 $w data\n\
\n\
$data($color,sel) itemconfig $data($color,index) -fill black\n\
}\n\
\n\
proc tkColorDialog_OkCmd {w} {\n\
global tkPriv\n\
upvar #0 $w data\n\
\n\
set tkPriv(selectColor) $data(finalColor)\n\
}\n\
\n\
proc tkColorDialog_CancelCmd {w} {\n\
global tkPriv\n\
\n\
set tkPriv(selectColor) \"\"\n\
}\n\
\n\
\n\
proc tclParseConfigSpec {w specs flags argList} {\n\
upvar #0 $w data\n\
\n\
foreach spec $specs {\n\
if {[llength $spec] < 4} {\n\
error \"\\\"spec\\\" should contain 5 or 4 elements\"\n\
}\n\
set cmdsw [lindex $spec 0]\n\
set cmd($cmdsw) \"\"\n\
set rname($cmdsw)   [lindex $spec 1]\n\
set rclass($cmdsw)  [lindex $spec 2]\n\
set def($cmdsw)     [lindex $spec 3]\n\
set verproc($cmdsw) [lindex $spec 4]\n\
}\n\
\n\
if {[llength $argList] & 1} {\n\
set cmdsw [lindex $argList end]\n\
if {![info exists cmd($cmdsw)]} {\n\
error \"bad option \\\"$cmdsw\\\": must be [tclListValidFlags cmd]\"\n\
}\n\
error \"value for \\\"$cmdsw\\\" missing\"\n\
}\n\
\n\
foreach cmdsw [array names cmd] {\n\
set data($cmdsw) $def($cmdsw)\n\
}\n\
\n\
foreach {cmdsw value} $argList {\n\
if {![info exists cmd($cmdsw)]} {\n\
error \"bad option \\\"$cmdsw\\\": must be [tclListValidFlags cmd]\"\n\
}\n\
set data($cmdsw) $value\n\
}\n\
\n\
}\n\
\n\
proc tclListValidFlags {v} {\n\
upvar $v cmd\n\
\n\
set len [llength [array names cmd]]\n\
set i 1\n\
set separator \"\"\n\
set errormsg \"\"\n\
foreach cmdsw [lsort [array names cmd]] {\n\
append errormsg \"$separator$cmdsw\"\n\
incr i\n\
if {$i == $len} {\n\
set separator \", or \"\n\
} else {\n\
set separator \", \"\n\
}\n\
}\n\
return $errormsg\n\
}\n\
\n\
\n\
\n\
proc tkFocusGroup_Create {t} {\n\
global tkPriv\n\
if {[string compare [winfo toplevel $t] $t]} {\n\
error \"$t is not a toplevel window\"\n\
}\n\
if {![info exists tkPriv(fg,$t)]} {\n\
set tkPriv(fg,$t) 1\n\
set tkPriv(focus,$t) \"\"\n\
bind $t <FocusIn>  [list tkFocusGroup_In  $t %W %d]\n\
bind $t <FocusOut> [list tkFocusGroup_Out $t %W %d]\n\
bind $t <Destroy>  [list tkFocusGroup_Destroy $t %W]\n\
}\n\
}\n\
\n\
proc tkFocusGroup_BindIn {t w cmd} {\n\
global tkFocusIn tkPriv\n\
if {![info exists tkPriv(fg,$t)]} {\n\
error \"focus group \\\"$t\\\" doesn't exist\"\n\
}\n\
set tkFocusIn($t,$w) $cmd\n\
}\n\
\n\
\n\
proc tkFocusGroup_BindOut {t w cmd} {\n\
global tkFocusOut tkPriv\n\
if {![info exists tkPriv(fg,$t)]} {\n\
error \"focus group \\\"$t\\\" doesn't exist\"\n\
}\n\
set tkFocusOut($t,$w) $cmd\n\
}\n\
\n\
proc tkFocusGroup_Destroy {t w} {\n\
global tkPriv tkFocusIn tkFocusOut\n\
\n\
if {[string equal $t $w]} {\n\
unset tkPriv(fg,$t)\n\
unset tkPriv(focus,$t) \n\
\n\
foreach name [array names tkFocusIn $t,*] {\n\
unset tkFocusIn($name)\n\
}\n\
foreach name [array names tkFocusOut $t,*] {\n\
unset tkFocusOut($name)\n\
}\n\
} else {\n\
if {[info exists tkPriv(focus,$t)] && \\\n\
[string equal $tkPriv(focus,$t) $w]} {\n\
set tkPriv(focus,$t) \"\"\n\
}\n\
catch {\n\
unset tkFocusIn($t,$w)\n\
}\n\
catch {\n\
unset tkFocusOut($t,$w)\n\
}\n\
}\n\
}\n\
\n\
proc tkFocusGroup_In {t w detail} {\n\
global tkPriv tkFocusIn\n\
\n\
if {[string compare $detail NotifyNonlinear] && \\\n\
[string compare $detail NotifyNonlinearVirtual]} {\n\
return\n\
}\n\
if {![info exists tkFocusIn($t,$w)]} {\n\
set tkFocusIn($t,$w) \"\"\n\
return\n\
}\n\
if {![info exists tkPriv(focus,$t)]} {\n\
return\n\
}\n\
if {[string equal $tkPriv(focus,$t) $w]} {\n\
return\n\
} else {\n\
set tkPriv(focus,$t) $w\n\
eval $tkFocusIn($t,$w)\n\
}\n\
}\n\
\n\
proc tkFocusGroup_Out {t w detail} {\n\
global tkPriv tkFocusOut\n\
\n\
if {[string compare $detail NotifyNonlinear] && \\\n\
[string compare $detail NotifyNonlinearVirtual]} {\n\
return\n\
}\n\
if {![info exists tkPriv(focus,$t)]} {\n\
return\n\
}\n\
if {![info exists tkFocusOut($t,$w)]} {\n\
return\n\
} else {\n\
eval $tkFocusOut($t,$w)\n\
set tkPriv(focus,$t) \"\"\n\
}\n\
}\n\
\n\
proc tkFDGetFileTypes {string} {\n\
foreach t $string {\n\
if {[llength $t] < 2 || [llength $t] > 3} {\n\
error \"bad file type \\\"$t\\\", should be \\\"typeName {extension ?extensions ...?} ?{macType ?macTypes ...?}?\\\"\"\n\
}\n\
eval lappend [list fileTypes([lindex $t 0])] [lindex $t 1]\n\
}\n\
\n\
set types {}\n\
foreach t $string {\n\
set label [lindex $t 0]\n\
set exts {}\n\
\n\
if {[info exists hasDoneType($label)]} {\n\
continue\n\
}\n\
\n\
set name \"$label (\"\n\
set sep \"\"\n\
foreach ext $fileTypes($label) {\n\
if {[string equal $ext \"\"]} {\n\
continue\n\
}\n\
regsub {^[.]} $ext \"*.\" ext\n\
if {![info exists hasGotExt($label,$ext)]} {\n\
append name $sep$ext\n\
lappend exts $ext\n\
set hasGotExt($label,$ext) 1\n\
}\n\
set sep ,\n\
}\n\
append name \")\"\n\
lappend types [list $name $exts]\n\
\n\
set hasDoneType($label) 1\n\
}\n\
\n\
return $types\n\
}\n\
\n\
\n\
if {[string match \"macintosh\" $tcl_platform(platform)]} {\n\
bind Radiobutton <Enter> {\n\
tkButtonEnter %W\n\
}\n\
bind Radiobutton <1> {\n\
tkButtonDown %W\n\
}\n\
bind Radiobutton <ButtonRelease-1> {\n\
tkButtonUp %W\n\
}\n\
bind Checkbutton <Enter> {\n\
tkButtonEnter %W\n\
}\n\
bind Checkbutton <1> {\n\
tkButtonDown %W\n\
}\n\
bind Checkbutton <ButtonRelease-1> {\n\
tkButtonUp %W\n\
}\n\
}\n\
if {[string match \"windows\" $tcl_platform(platform)]} {\n\
bind Checkbutton <equal> {\n\
tkCheckRadioInvoke %W select\n\
}\n\
bind Checkbutton <plus> {\n\
tkCheckRadioInvoke %W select\n\
}\n\
bind Checkbutton <minus> {\n\
tkCheckRadioInvoke %W deselect\n\
}\n\
bind Checkbutton <1> {\n\
tkCheckRadioDown %W\n\
}\n\
bind Checkbutton <ButtonRelease-1> {\n\
tkButtonUp %W\n\
}\n\
bind Checkbutton <Enter> {\n\
tkCheckRadioEnter %W\n\
}\n\
\n\
bind Radiobutton <1> {\n\
tkCheckRadioDown %W\n\
}\n\
bind Radiobutton <ButtonRelease-1> {\n\
tkButtonUp %W\n\
}\n\
bind Radiobutton <Enter> {\n\
tkCheckRadioEnter %W\n\
}\n\
}\n\
if {[string match \"unix\" $tcl_platform(platform)]} {\n\
bind Checkbutton <Return> {\n\
if {!$tk_strictMotif} {\n\
tkCheckRadioInvoke %W\n\
}\n\
}\n\
bind Radiobutton <Return> {\n\
if {!$tk_strictMotif} {\n\
tkCheckRadioInvoke %W\n\
}\n\
}\n\
bind Checkbutton <1> {\n\
tkCheckRadioInvoke %W\n\
}\n\
bind Radiobutton <1> {\n\
tkCheckRadioInvoke %W\n\
}\n\
bind Checkbutton <Enter> {\n\
tkButtonEnter %W\n\
}\n\
bind Radiobutton <Enter> {\n\
tkButtonEnter %W\n\
}\n\
}\n\
\n\
bind Button <space> {\n\
tkButtonInvoke %W\n\
}\n\
bind Checkbutton <space> {\n\
tkCheckRadioInvoke %W\n\
}\n\
bind Radiobutton <space> {\n\
tkCheckRadioInvoke %W\n\
}\n\
\n\
bind Button <FocusIn> {}\n\
bind Button <Enter> {\n\
tkButtonEnter %W\n\
}\n\
bind Button <Leave> {\n\
tkButtonLeave %W\n\
}\n\
bind Button <1> {\n\
tkButtonDown %W\n\
}\n\
bind Button <ButtonRelease-1> {\n\
tkButtonUp %W\n\
}\n\
\n\
bind Checkbutton <FocusIn> {}\n\
bind Checkbutton <Leave> {\n\
tkButtonLeave %W\n\
}\n\
\n\
bind Radiobutton <FocusIn> {}\n\
bind Radiobutton <Leave> {\n\
tkButtonLeave %W\n\
}\n\
\n\
if {[string match \"windows\" $tcl_platform(platform)]} {\n\
\n\
\n\
\n\
proc tkButtonEnter w {\n\
global tkPriv\n\
if {[string compare [$w cget -state] \"disabled\"] \\\n\
&& [string equal $tkPriv(buttonWindow) $w]} {\n\
$w configure -state active -relief sunken\n\
}\n\
set tkPriv(window) $w\n\
}\n\
\n\
\n\
proc tkButtonLeave w {\n\
global tkPriv\n\
if {[string compare [$w cget -state] \"disabled\"]} {\n\
$w configure -state normal\n\
}\n\
if {[string equal $tkPriv(buttonWindow) $w]} {\n\
$w configure -relief $tkPriv(relief)\n\
}\n\
set tkPriv(window) \"\"\n\
}\n\
\n\
\n\
proc tkCheckRadioEnter w {\n\
global tkPriv\n\
if {[string compare [$w cget -state] \"disabled\"] \\\n\
&& [string equal $tkPriv(buttonWindow) $w]} {\n\
$w configure -state active\n\
}\n\
set tkPriv(window) $w\n\
}\n\
\n\
\n\
proc tkButtonDown w {\n\
global tkPriv\n\
set tkPriv(relief) [$w cget -relief]\n\
if {[string compare [$w cget -state] \"disabled\"]} {\n\
set tkPriv(buttonWindow) $w\n\
$w configure -relief sunken -state active\n\
}\n\
}\n\
\n\
\n\
proc tkCheckRadioDown w {\n\
global tkPriv\n\
set tkPriv(relief) [$w cget -relief]\n\
if {[string compare [$w cget -state] \"disabled\"]} {\n\
set tkPriv(buttonWindow) $w\n\
$w configure -state active\n\
}\n\
}\n\
\n\
\n\
proc tkButtonUp w {\n\
global tkPriv\n\
if {[string equal $tkPriv(buttonWindow) $w]} {\n\
set tkPriv(buttonWindow) \"\"\n\
$w configure -relief $tkPriv(relief)\n\
if {[string equal $tkPriv(window) $w]\n\
&& [string compare [$w cget -state] \"disabled\"]} {\n\
$w configure -state normal\n\
uplevel #0 [list $w invoke]\n\
}\n\
}\n\
}\n\
\n\
}\n\
\n\
if {[string match \"unix\" $tcl_platform(platform)]} {\n\
\n\
\n\
\n\
proc tkButtonEnter {w} {\n\
global tkPriv\n\
if {[string compare [$w cget -state] \"disabled\"]} {\n\
$w configure -state active\n\
if {[string equal $tkPriv(buttonWindow) $w]} {\n\
$w configure -state active -relief sunken\n\
}\n\
}\n\
set tkPriv(window) $w\n\
}\n\
\n\
\n\
proc tkButtonLeave w {\n\
global tkPriv\n\
if {[string compare [$w cget -state] \"disabled\"]} {\n\
$w configure -state normal\n\
}\n\
if {[string equal $tkPriv(buttonWindow) $w]} {\n\
$w configure -relief $tkPriv(relief)\n\
}\n\
set tkPriv(window) \"\"\n\
}\n\
\n\
\n\
proc tkButtonDown w {\n\
global tkPriv\n\
set tkPriv(relief) [$w cget -relief]\n\
if {[string compare [$w cget -state] \"disabled\"]} {\n\
set tkPriv(buttonWindow) $w\n\
$w configure -relief sunken\n\
}\n\
}\n\
\n\
\n\
proc tkButtonUp w {\n\
global tkPriv\n\
if {[string equal $w $tkPriv(buttonWindow)]} {\n\
set tkPriv(buttonWindow) \"\"\n\
$w configure -relief $tkPriv(relief)\n\
if {[string equal $w $tkPriv(window)] \\\n\
&& [string compare [$w cget -state] \"disabled\"]} {\n\
uplevel #0 [list $w invoke]\n\
}\n\
}\n\
}\n\
\n\
}\n\
\n\
if {[string match \"macintosh\" $tcl_platform(platform)]} {\n\
\n\
\n\
\n\
proc tkButtonEnter {w} {\n\
global tkPriv\n\
if {[string compare [$w cget -state] \"disabled\"]} {\n\
if {[string equal $w $tkPriv(buttonWindow)]} {\n\
$w configure -state active\n\
}\n\
}\n\
set tkPriv(window) $w\n\
}\n\
\n\
\n\
proc tkButtonLeave w {\n\
global tkPriv\n\
if {[string equal $w $tkPriv(buttonWindow)]} {\n\
$w configure -state normal\n\
}\n\
set tkPriv(window) \"\"\n\
}\n\
\n\
\n\
proc tkButtonDown w {\n\
global tkPriv\n\
if {[string compare [$w cget -state] \"disabled\"]} {\n\
set tkPriv(buttonWindow) $w\n\
$w configure -state active\n\
}\n\
}\n\
\n\
\n\
proc tkButtonUp w {\n\
global tkPriv\n\
if {[string equal $w $tkPriv(buttonWindow)]} {\n\
$w configure -state normal\n\
set tkPriv(buttonWindow) \"\"\n\
if {[string equal $w $tkPriv(window)]\n\
&& [string compare [$w cget -state] \"disabled\"]} {\n\
uplevel #0 [list $w invoke]\n\
}\n\
}\n\
}\n\
\n\
}\n\
\n\
\n\
\n\
proc tkButtonInvoke w {\n\
if {[string compare [$w cget -state] \"disabled\"]} {\n\
set oldRelief [$w cget -relief]\n\
set oldState [$w cget -state]\n\
$w configure -state active -relief sunken\n\
update idletasks\n\
after 100\n\
$w configure -state $oldState -relief $oldRelief\n\
uplevel #0 [list $w invoke]\n\
}\n\
}\n\
\n\
\n\
proc tkCheckRadioInvoke {w {cmd invoke}} {\n\
if {[string compare [$w cget -state] \"disabled\"]} {\n\
uplevel #0 [list $w $cmd]\n\
}\n\
}\n\
\n\
\n\
\n\
proc tk_dialog {w title text bitmap default args} {\n\
global tkPriv tcl_platform\n\
\n\
if {[string is int $default]} {\n\
if {$default >= [llength $args]} {\n\
return -code error \"default button index greater than number of\\\n\
buttons specified for tk_dialog\"\n\
}\n\
} elseif {[string equal {} $default]} {\n\
set default -1\n\
} else {\n\
set default [lsearch -exact $args $default]\n\
}\n\
\n\
\n\
catch {destroy $w}\n\
toplevel $w -class Dialog\n\
wm title $w $title\n\
wm iconname $w Dialog\n\
wm protocol $w WM_DELETE_WINDOW { }\n\
\n\
if { [winfo viewable [winfo toplevel [winfo parent $w]]] } {\n\
wm transient $w [winfo toplevel [winfo parent $w]]\n\
}    \n\
\n\
if {[string equal $tcl_platform(platform) \"macintosh\"]} {\n\
unsupported1 style $w dBoxProc\n\
}\n\
\n\
frame $w.bot\n\
frame $w.top\n\
if {[string equal $tcl_platform(platform) \"unix\"]} {\n\
$w.bot configure -relief raised -bd 1\n\
$w.top configure -relief raised -bd 1\n\
}\n\
pack $w.bot -side bottom -fill both\n\
pack $w.top -side top -fill both -expand 1\n\
\n\
\n\
option add *Dialog.msg.wrapLength 3i widgetDefault\n\
if {[string equal $tcl_platform(platform) \"macintosh\"]} {\n\
option add *Dialog.msg.font system widgetDefault\n\
} else {\n\
option add *Dialog.msg.font {Times 12} widgetDefault\n\
}\n\
\n\
label $w.msg -justify left -text $text\n\
pack $w.msg -in $w.top -side right -expand 1 -fill both -padx 3m -pady 3m\n\
if {[string compare $bitmap \"\"]} {\n\
if {[string equal $tcl_platform(platform) \"macintosh\"] && \\\n\
[string equal $bitmap \"error\"]} {\n\
set bitmap \"stop\"\n\
}\n\
label $w.bitmap -bitmap $bitmap\n\
pack $w.bitmap -in $w.top -side left -padx 3m -pady 3m\n\
}\n\
\n\
\n\
set i 0\n\
foreach but $args {\n\
button $w.button$i -text $but -command [list set tkPriv(button) $i]\n\
if {$i == $default} {\n\
$w.button$i configure -default active\n\
} else {\n\
$w.button$i configure -default normal\n\
}\n\
grid $w.button$i -in $w.bot -column $i -row 0 -sticky ew -padx 10\n\
grid columnconfigure $w.bot $i\n\
if {[string equal $tcl_platform(platform) \"macintosh\"]} {\n\
set tmp [string tolower $but]\n\
if {[string equal $tmp \"ok\"] || [string equal $tmp \"cancel\"]} {\n\
grid columnconfigure $w.bot $i -minsize [expr {59 + 20}]\n\
}\n\
}\n\
incr i\n\
}\n\
\n\
\n\
if {$default >= 0} {\n\
bind $w <Return> \"\n\
[list $w.button$default] configure -state active -relief sunken\n\
update idletasks\n\
after 100\n\
set tkPriv(button) $default\n\
\"\n\
}\n\
\n\
\n\
bind $w <Destroy> {set tkPriv(button) -1}\n\
\n\
\n\
wm withdraw $w\n\
update idletasks\n\
set x [expr {[winfo screenwidth $w]/2 - [winfo reqwidth $w]/2 \\\n\
- [winfo vrootx [winfo parent $w]]}]\n\
set y [expr {[winfo screenheight $w]/2 - [winfo reqheight $w]/2 \\\n\
- [winfo vrooty [winfo parent $w]]}]\n\
wm geom $w +$x+$y\n\
wm deiconify $w\n\
\n\
\n\
set oldFocus [focus]\n\
set oldGrab [grab current $w]\n\
if {[string compare $oldGrab \"\"]} {\n\
set grabStatus [grab status $oldGrab]\n\
}\n\
grab $w\n\
if {$default >= 0} {\n\
focus $w.button$default\n\
} else {\n\
focus $w\n\
}\n\
\n\
\n\
tkwait variable tkPriv(button)\n\
catch {focus $oldFocus}\n\
catch {\n\
\n\
bind $w <Destroy> {}\n\
destroy $w\n\
}\n\
if {[string compare $oldGrab \"\"]} {\n\
if {[string compare $grabStatus \"global\"]} {\n\
grab $oldGrab\n\
} else {\n\
grab -global $oldGrab\n\
}\n\
}\n\
return $tkPriv(button)\n\
}\n\
\n\
\n\
bind Entry <<Cut>> {\n\
if {![catch {tkEntryGetSelection %W} tkPriv(data)]} {\n\
clipboard clear -displayof %W\n\
clipboard append -displayof %W $tkPriv(data)\n\
%W delete sel.first sel.last\n\
unset tkPriv(data)\n\
}\n\
}\n\
bind Entry <<Copy>> {\n\
if {![catch {tkEntryGetSelection %W} tkPriv(data)]} {\n\
clipboard clear -displayof %W\n\
clipboard append -displayof %W $tkPriv(data)\n\
unset tkPriv(data)\n\
}\n\
}\n\
bind Entry <<Paste>> {\n\
global tcl_platform\n\
catch {\n\
if {[string compare $tcl_platform(platform) \"unix\"]} {\n\
catch {\n\
%W delete sel.first sel.last\n\
}\n\
}\n\
%W insert insert [selection get -displayof %W -selection CLIPBOARD]\n\
tkEntrySeeInsert %W\n\
}\n\
}\n\
bind Entry <<Clear>> {\n\
%W delete sel.first sel.last\n\
}\n\
bind Entry <<PasteSelection>> {\n\
if {!$tkPriv(mouseMoved) || $tk_strictMotif} {\n\
tkEntryPaste %W %x\n\
}\n\
}\n\
\n\
\n\
bind Entry <1> {\n\
tkEntryButton1 %W %x\n\
%W selection clear\n\
}\n\
bind Entry <B1-Motion> {\n\
set tkPriv(x) %x\n\
tkEntryMouseSelect %W %x\n\
}\n\
bind Entry <Double-1> {\n\
set tkPriv(selectMode) word\n\
tkEntryMouseSelect %W %x\n\
catch {%W icursor sel.first}\n\
}\n\
bind Entry <Triple-1> {\n\
set tkPriv(selectMode) line\n\
tkEntryMouseSelect %W %x\n\
%W icursor 0\n\
}\n\
bind Entry <Shift-1> {\n\
set tkPriv(selectMode) char\n\
%W selection adjust @%x\n\
}\n\
bind Entry <Double-Shift-1>	{\n\
set tkPriv(selectMode) word\n\
tkEntryMouseSelect %W %x\n\
}\n\
bind Entry <Triple-Shift-1>	{\n\
set tkPriv(selectMode) line\n\
tkEntryMouseSelect %W %x\n\
}\n\
bind Entry <B1-Leave> {\n\
set tkPriv(x) %x\n\
tkEntryAutoScan %W\n\
}\n\
bind Entry <B1-Enter> {\n\
tkCancelRepeat\n\
}\n\
bind Entry <ButtonRelease-1> {\n\
tkCancelRepeat\n\
}\n\
bind Entry <Control-1> {\n\
%W icursor @%x\n\
}\n\
\n\
bind Entry <Left> {\n\
tkEntrySetCursor %W [expr {[%W index insert] - 1}]\n\
}\n\
bind Entry <Right> {\n\
tkEntrySetCursor %W [expr {[%W index insert] + 1}]\n\
}\n\
bind Entry <Shift-Left> {\n\
tkEntryKeySelect %W [expr {[%W index insert] - 1}]\n\
tkEntrySeeInsert %W\n\
}\n\
bind Entry <Shift-Right> {\n\
tkEntryKeySelect %W [expr {[%W index insert] + 1}]\n\
tkEntrySeeInsert %W\n\
}\n\
bind Entry <Control-Left> {\n\
tkEntrySetCursor %W [tkEntryPreviousWord %W insert]\n\
}\n\
bind Entry <Control-Right> {\n\
tkEntrySetCursor %W [tkEntryNextWord %W insert]\n\
}\n\
bind Entry <Shift-Control-Left> {\n\
tkEntryKeySelect %W [tkEntryPreviousWord %W insert]\n\
tkEntrySeeInsert %W\n\
}\n\
bind Entry <Shift-Control-Right> {\n\
tkEntryKeySelect %W [tkEntryNextWord %W insert]\n\
tkEntrySeeInsert %W\n\
}\n\
bind Entry <Home> {\n\
tkEntrySetCursor %W 0\n\
}\n\
bind Entry <Shift-Home> {\n\
tkEntryKeySelect %W 0\n\
tkEntrySeeInsert %W\n\
}\n\
bind Entry <End> {\n\
tkEntrySetCursor %W end\n\
}\n\
bind Entry <Shift-End> {\n\
tkEntryKeySelect %W end\n\
tkEntrySeeInsert %W\n\
}\n\
\n\
bind Entry <Delete> {\n\
if {[%W selection present]} {\n\
%W delete sel.first sel.last\n\
} else {\n\
%W delete insert\n\
}\n\
}\n\
bind Entry <BackSpace> {\n\
tkEntryBackspace %W\n\
}\n\
\n\
bind Entry <Control-space> {\n\
%W selection from insert\n\
}\n\
bind Entry <Select> {\n\
%W selection from insert\n\
}\n\
bind Entry <Control-Shift-space> {\n\
%W selection adjust insert\n\
}\n\
bind Entry <Shift-Select> {\n\
%W selection adjust insert\n\
}\n\
bind Entry <Control-slash> {\n\
%W selection range 0 end\n\
}\n\
bind Entry <Control-backslash> {\n\
%W selection clear\n\
}\n\
bind Entry <KeyPress> {\n\
tkEntryInsert %W %A\n\
}\n\
\n\
\n\
bind Entry <Alt-KeyPress> {# nothing}\n\
bind Entry <Meta-KeyPress> {# nothing}\n\
bind Entry <Control-KeyPress> {# nothing}\n\
bind Entry <Escape> {# nothing}\n\
bind Entry <Return> {# nothing}\n\
bind Entry <KP_Enter> {# nothing}\n\
bind Entry <Tab> {# nothing}\n\
if {[string equal $tcl_platform(platform) \"macintosh\"]} {\n\
bind Entry <Command-KeyPress> {# nothing}\n\
}\n\
\n\
if {[string compare $tcl_platform(platform) \"windows\"]} {\n\
bind Entry <Insert> {\n\
catch {tkEntryInsert %W [selection get -displayof %W]}\n\
}\n\
}\n\
\n\
\n\
bind Entry <Control-a> {\n\
if {!$tk_strictMotif} {\n\
tkEntrySetCursor %W 0\n\
}\n\
}\n\
bind Entry <Control-b> {\n\
if {!$tk_strictMotif} {\n\
tkEntrySetCursor %W [expr {[%W index insert] - 1}]\n\
}\n\
}\n\
bind Entry <Control-d> {\n\
if {!$tk_strictMotif} {\n\
%W delete insert\n\
}\n\
}\n\
bind Entry <Control-e> {\n\
if {!$tk_strictMotif} {\n\
tkEntrySetCursor %W end\n\
}\n\
}\n\
bind Entry <Control-f> {\n\
if {!$tk_strictMotif} {\n\
tkEntrySetCursor %W [expr {[%W index insert] + 1}]\n\
}\n\
}\n\
bind Entry <Control-h> {\n\
if {!$tk_strictMotif} {\n\
tkEntryBackspace %W\n\
}\n\
}\n\
bind Entry <Control-k> {\n\
if {!$tk_strictMotif} {\n\
%W delete insert end\n\
}\n\
}\n\
bind Entry <Control-t> {\n\
if {!$tk_strictMotif} {\n\
tkEntryTranspose %W\n\
}\n\
}\n\
bind Entry <Meta-b> {\n\
if {!$tk_strictMotif} {\n\
tkEntrySetCursor %W [tkEntryPreviousWord %W insert]\n\
}\n\
}\n\
bind Entry <Meta-d> {\n\
if {!$tk_strictMotif} {\n\
%W delete insert [tkEntryNextWord %W insert]\n\
}\n\
}\n\
bind Entry <Meta-f> {\n\
if {!$tk_strictMotif} {\n\
tkEntrySetCursor %W [tkEntryNextWord %W insert]\n\
}\n\
}\n\
bind Entry <Meta-BackSpace> {\n\
if {!$tk_strictMotif} {\n\
%W delete [tkEntryPreviousWord %W insert] insert\n\
}\n\
}\n\
bind Entry <Meta-Delete> {\n\
if {!$tk_strictMotif} {\n\
%W delete [tkEntryPreviousWord %W insert] insert\n\
}\n\
}\n\
\n\
\n\
bind Entry <2> {\n\
if {!$tk_strictMotif} {\n\
%W scan mark %x\n\
set tkPriv(x) %x\n\
set tkPriv(y) %y\n\
set tkPriv(mouseMoved) 0\n\
}\n\
}\n\
bind Entry <B2-Motion> {\n\
if {!$tk_strictMotif} {\n\
if {abs(%x-$tkPriv(x)) > 2} {\n\
set tkPriv(mouseMoved) 1\n\
}\n\
%W scan dragto %x\n\
}\n\
}\n\
\n\
\n\
proc tkEntryClosestGap {w x} {\n\
set pos [$w index @$x]\n\
set bbox [$w bbox $pos]\n\
if {($x - [lindex $bbox 0]) < ([lindex $bbox 2]/2)} {\n\
return $pos\n\
}\n\
incr pos\n\
}\n\
\n\
\n\
proc tkEntryButton1 {w x} {\n\
global tkPriv\n\
\n\
set tkPriv(selectMode) char\n\
set tkPriv(mouseMoved) 0\n\
set tkPriv(pressX) $x\n\
$w icursor [tkEntryClosestGap $w $x]\n\
$w selection from insert\n\
if {[string equal [$w cget -state] \"normal\"]} {focus $w}\n\
}\n\
\n\
\n\
proc tkEntryMouseSelect {w x} {\n\
global tkPriv\n\
\n\
set cur [tkEntryClosestGap $w $x]\n\
set anchor [$w index anchor]\n\
if {($cur != $anchor) || (abs($tkPriv(pressX) - $x) >= 3)} {\n\
set tkPriv(mouseMoved) 1\n\
}\n\
switch $tkPriv(selectMode) {\n\
char {\n\
if {$tkPriv(mouseMoved)} {\n\
if {$cur < $anchor} {\n\
$w selection range $cur $anchor\n\
} elseif {$cur > $anchor} {\n\
$w selection range $anchor $cur\n\
} else {\n\
$w selection clear\n\
}\n\
}\n\
}\n\
word {\n\
if {$cur < [$w index anchor]} {\n\
set before [tcl_wordBreakBefore [$w get] $cur]\n\
set after [tcl_wordBreakAfter [$w get] [expr {$anchor-1}]]\n\
} else {\n\
set before [tcl_wordBreakBefore [$w get] $anchor]\n\
set after [tcl_wordBreakAfter [$w get] [expr {$cur - 1}]]\n\
}\n\
if {$before < 0} {\n\
set before 0\n\
}\n\
if {$after < 0} {\n\
set after end\n\
}\n\
$w selection range $before $after\n\
}\n\
line {\n\
$w selection range 0 end\n\
}\n\
}\n\
update idletasks\n\
}\n\
\n\
\n\
proc tkEntryPaste {w x} {\n\
global tkPriv\n\
\n\
$w icursor [tkEntryClosestGap $w $x]\n\
catch {$w insert insert [selection get -displayof $w]}\n\
if {[string equal [$w cget -state] \"normal\"]} {focus $w}\n\
}\n\
\n\
\n\
proc tkEntryAutoScan {w} {\n\
global tkPriv\n\
set x $tkPriv(x)\n\
if {![winfo exists $w]} return\n\
if {$x >= [winfo width $w]} {\n\
$w xview scroll 2 units\n\
tkEntryMouseSelect $w $x\n\
} elseif {$x < 0} {\n\
$w xview scroll -2 units\n\
tkEntryMouseSelect $w $x\n\
}\n\
set tkPriv(afterId) [after 50 [list tkEntryAutoScan $w]]\n\
}\n\
\n\
\n\
proc tkEntryKeySelect {w new} {\n\
if {![$w selection present]} {\n\
$w selection from insert\n\
$w selection to $new\n\
} else {\n\
$w selection adjust $new\n\
}\n\
$w icursor $new\n\
}\n\
\n\
\n\
proc tkEntryInsert {w s} {\n\
if {[string equal $s \"\"]} {\n\
return\n\
}\n\
catch {\n\
set insert [$w index insert]\n\
if {([$w index sel.first] <= $insert)\n\
&& ([$w index sel.last] >= $insert)} {\n\
$w delete sel.first sel.last\n\
}\n\
}\n\
$w insert insert $s\n\
tkEntrySeeInsert $w\n\
}\n\
\n\
\n\
proc tkEntryBackspace w {\n\
if {[$w selection present]} {\n\
$w delete sel.first sel.last\n\
} else {\n\
set x [expr {[$w index insert] - 1}]\n\
if {$x >= 0} {$w delete $x}\n\
if {[$w index @0] >= [$w index insert]} {\n\
set range [$w xview]\n\
set left [lindex $range 0]\n\
set right [lindex $range 1]\n\
$w xview moveto [expr {$left - ($right - $left)/2.0}]\n\
}\n\
}\n\
}\n\
\n\
\n\
proc tkEntrySeeInsert w {\n\
set c [$w index insert]\n\
if {($c < [$w index @0]) || ($c > [$w index @[winfo width $w]])} {\n\
$w xview $c\n\
}\n\
}\n\
\n\
\n\
proc tkEntrySetCursor {w pos} {\n\
$w icursor $pos\n\
$w selection clear\n\
tkEntrySeeInsert $w\n\
}\n\
\n\
\n\
proc tkEntryTranspose w {\n\
set i [$w index insert]\n\
if {$i < [$w index end]} {\n\
incr i\n\
}\n\
set first [expr {$i-2}]\n\
if {$first < 0} {\n\
return\n\
}\n\
set new [string index [$w get] [expr {$i-1}]][string index [$w get] $first]\n\
$w delete $first $i\n\
$w insert insert $new\n\
tkEntrySeeInsert $w\n\
}\n\
\n\
\n\
if {[string equal $tcl_platform(platform) \"windows\"]}  {\n\
proc tkEntryNextWord {w start} {\n\
set pos [tcl_endOfWord [$w get] [$w index $start]]\n\
if {$pos >= 0} {\n\
set pos [tcl_startOfNextWord [$w get] $pos]\n\
}\n\
if {$pos < 0} {\n\
return end\n\
}\n\
return $pos\n\
}\n\
} else {\n\
proc tkEntryNextWord {w start} {\n\
set pos [tcl_endOfWord [$w get] [$w index $start]]\n\
if {$pos < 0} {\n\
return end\n\
}\n\
return $pos\n\
}\n\
}\n\
\n\
\n\
proc tkEntryPreviousWord {w start} {\n\
set pos [tcl_startOfPreviousWord [$w get] [$w index $start]]\n\
if {$pos < 0} {\n\
return 0\n\
}\n\
return $pos\n\
}\n\
\n\
proc tkEntryGetSelection {w} {\n\
set entryString [string range [$w get] [$w index sel.first] \\\n\
[expr {[$w index sel.last] - 1}]]\n\
if {[string compare [$w cget -show] \"\"]} {\n\
regsub -all . $entryString [string index [$w cget -show] 0] entryString\n\
}\n\
return $entryString\n\
}\n\
\n\
\n\
proc tk_focusNext w {\n\
set cur $w\n\
while {1} {\n\
\n\
\n\
set parent $cur\n\
set children [winfo children $cur]\n\
set i -1\n\
\n\
\n\
while {1} {\n\
incr i\n\
if {$i < [llength $children]} {\n\
set cur [lindex $children $i]\n\
if {[string equal [winfo toplevel $cur] $cur]} {\n\
continue\n\
} else {\n\
break\n\
}\n\
}\n\
\n\
\n\
set cur $parent\n\
if {[string equal [winfo toplevel $cur] $cur]} {\n\
break\n\
}\n\
set parent [winfo parent $parent]\n\
set children [winfo children $parent]\n\
set i [lsearch -exact $children $cur]\n\
}\n\
if {[string equal $w $cur] || [tkFocusOK $cur]} {\n\
return $cur\n\
}\n\
}\n\
}\n\
\n\
\n\
proc tk_focusPrev w {\n\
set cur $w\n\
while {1} {\n\
\n\
\n\
if {[string equal [winfo toplevel $cur] $cur]}  {\n\
set parent $cur\n\
set children [winfo children $cur]\n\
set i [llength $children]\n\
} else {\n\
set parent [winfo parent $cur]\n\
set children [winfo children $parent]\n\
set i [lsearch -exact $children $cur]\n\
}\n\
\n\
\n\
while {$i > 0} {\n\
incr i -1\n\
set cur [lindex $children $i]\n\
if {[string equal [winfo toplevel $cur] $cur]} {\n\
continue\n\
}\n\
set parent $cur\n\
set children [winfo children $parent]\n\
set i [llength $children]\n\
}\n\
set cur $parent\n\
if {[string equal $w $cur] || [tkFocusOK $cur]} {\n\
return $cur\n\
}\n\
}\n\
}\n\
\n\
\n\
proc tkFocusOK w {\n\
set code [catch {$w cget -takefocus} value]\n\
if {($code == 0) && ($value != \"\")} {\n\
if {$value == 0} {\n\
return 0\n\
} elseif {$value == 1} {\n\
return [winfo viewable $w]\n\
} else {\n\
set value [uplevel #0 $value [list $w]]\n\
if {$value != \"\"} {\n\
return $value\n\
}\n\
}\n\
}\n\
if {![winfo viewable $w]} {\n\
return 0\n\
}\n\
set code [catch {$w cget -state} value]\n\
if {($code == 0) && [string equal $value \"disabled\"]} {\n\
return 0\n\
}\n\
regexp Key|Focus \"[bind $w] [bind [winfo class $w]]\"\n\
}\n\
\n\
\n\
proc tk_focusFollowsMouse {} {\n\
set old [bind all <Enter>]\n\
set script {\n\
if {[string equal \"%d\" \"NotifyAncestor\"] \\\n\
|| [string equal \"%d\" \"NotifyNonlinear\"] \\\n\
|| [string equal \"%d\" \"NotifyInferior\"]} {\n\
if {[tkFocusOK %W]} {\n\
focus %W\n\
}\n\
}\n\
}\n\
if {[string compare $old \"\"]} {\n\
bind all <Enter> \"$old; $script\"\n\
} else {\n\
bind all <Enter> $script\n\
}\n\
}\n\
\n\
\n\
\n\
\n\
bind Listbox <1> {\n\
if {[winfo exists %W]} {\n\
tkListboxBeginSelect %W [%W index @%x,%y]\n\
}\n\
}\n\
\n\
\n\
bind Listbox <Double-1> {\n\
}\n\
\n\
bind Listbox <B1-Motion> {\n\
set tkPriv(x) %x\n\
set tkPriv(y) %y\n\
tkListboxMotion %W [%W index @%x,%y]\n\
}\n\
bind Listbox <ButtonRelease-1> {\n\
tkCancelRepeat\n\
%W activate @%x,%y\n\
}\n\
bind Listbox <Shift-1> {\n\
tkListboxBeginExtend %W [%W index @%x,%y]\n\
}\n\
bind Listbox <Control-1> {\n\
tkListboxBeginToggle %W [%W index @%x,%y]\n\
}\n\
bind Listbox <B1-Leave> {\n\
set tkPriv(x) %x\n\
set tkPriv(y) %y\n\
tkListboxAutoScan %W\n\
}\n\
bind Listbox <B1-Enter> {\n\
tkCancelRepeat\n\
}\n\
\n\
bind Listbox <Up> {\n\
tkListboxUpDown %W -1\n\
}\n\
bind Listbox <Shift-Up> {\n\
tkListboxExtendUpDown %W -1\n\
}\n\
bind Listbox <Down> {\n\
tkListboxUpDown %W 1\n\
}\n\
bind Listbox <Shift-Down> {\n\
tkListboxExtendUpDown %W 1\n\
}\n\
bind Listbox <Left> {\n\
%W xview scroll -1 units\n\
}\n\
bind Listbox <Control-Left> {\n\
%W xview scroll -1 pages\n\
}\n\
bind Listbox <Right> {\n\
%W xview scroll 1 units\n\
}\n\
bind Listbox <Control-Right> {\n\
%W xview scroll 1 pages\n\
}\n\
bind Listbox <Prior> {\n\
%W yview scroll -1 pages\n\
%W activate @0,0\n\
}\n\
bind Listbox <Next> {\n\
%W yview scroll 1 pages\n\
%W activate @0,0\n\
}\n\
bind Listbox <Control-Prior> {\n\
%W xview scroll -1 pages\n\
}\n\
bind Listbox <Control-Next> {\n\
%W xview scroll 1 pages\n\
}\n\
bind Listbox <Home> {\n\
%W xview moveto 0\n\
}\n\
bind Listbox <End> {\n\
%W xview moveto 1\n\
}\n\
bind Listbox <Control-Home> {\n\
%W activate 0\n\
%W see 0\n\
%W selection clear 0 end\n\
%W selection set 0\n\
event generate %W <<ListboxSelect>>\n\
}\n\
bind Listbox <Shift-Control-Home> {\n\
tkListboxDataExtend %W 0\n\
}\n\
bind Listbox <Control-End> {\n\
%W activate end\n\
%W see end\n\
%W selection clear 0 end\n\
%W selection set end\n\
event generate %W <<ListboxSelect>>\n\
}\n\
bind Listbox <Shift-Control-End> {\n\
tkListboxDataExtend %W [%W index end]\n\
}\n\
bind Listbox <<Copy>> {\n\
if {[string equal [selection own -displayof %W] \"%W\"]} {\n\
clipboard clear -displayof %W\n\
clipboard append -displayof %W [selection get -displayof %W]\n\
}\n\
}\n\
bind Listbox <space> {\n\
tkListboxBeginSelect %W [%W index active]\n\
}\n\
bind Listbox <Select> {\n\
tkListboxBeginSelect %W [%W index active]\n\
}\n\
bind Listbox <Control-Shift-space> {\n\
tkListboxBeginExtend %W [%W index active]\n\
}\n\
bind Listbox <Shift-Select> {\n\
tkListboxBeginExtend %W [%W index active]\n\
}\n\
bind Listbox <Escape> {\n\
tkListboxCancel %W\n\
}\n\
bind Listbox <Control-slash> {\n\
tkListboxSelectAll %W\n\
}\n\
bind Listbox <Control-backslash> {\n\
if {[string compare [%W cget -selectmode] \"browse\"]} {\n\
%W selection clear 0 end\n\
event generate %W <<ListboxSelect>>\n\
}\n\
}\n\
\n\
\n\
bind Listbox <2> {\n\
%W scan mark %x %y\n\
}\n\
bind Listbox <B2-Motion> {\n\
%W scan dragto %x %y\n\
}\n\
\n\
\n\
bind Listbox <MouseWheel> {\n\
%W yview scroll [expr {- (%D / 120) * 4}] units\n\
}\n\
\n\
if {[string equal \"unix\" $tcl_platform(platform)]} {\n\
bind Listbox <4> {\n\
if {!$tk_strictMotif} {\n\
%W yview scroll -5 units\n\
}\n\
}\n\
bind Listbox <5> {\n\
if {!$tk_strictMotif} {\n\
%W yview scroll 5 units\n\
}\n\
}\n\
}\n\
\n\
\n\
proc tkListboxBeginSelect {w el} {\n\
global tkPriv\n\
if {[string equal [$w cget -selectmode] \"multiple\"]} {\n\
if {[$w selection includes $el]} {\n\
$w selection clear $el\n\
} else {\n\
$w selection set $el\n\
}\n\
} else {\n\
$w selection clear 0 end\n\
$w selection set $el\n\
$w selection anchor $el\n\
set tkPriv(listboxSelection) {}\n\
set tkPriv(listboxPrev) $el\n\
}\n\
event generate $w <<ListboxSelect>>\n\
}\n\
\n\
\n\
proc tkListboxMotion {w el} {\n\
global tkPriv\n\
if {$el == $tkPriv(listboxPrev)} {\n\
return\n\
}\n\
set anchor [$w index anchor]\n\
switch [$w cget -selectmode] {\n\
browse {\n\
$w selection clear 0 end\n\
$w selection set $el\n\
set tkPriv(listboxPrev) $el\n\
event generate $w <<ListboxSelect>>\n\
}\n\
extended {\n\
set i $tkPriv(listboxPrev)\n\
if {[string equal {} $i]} {\n\
set i $el\n\
$w selection set $el\n\
}\n\
if {[$w selection includes anchor]} {\n\
$w selection clear $i $el\n\
$w selection set anchor $el\n\
} else {\n\
$w selection clear $i $el\n\
$w selection clear anchor $el\n\
}\n\
if {![info exists tkPriv(listboxSelection)]} {\n\
set tkPriv(listboxSelection) [$w curselection]\n\
}\n\
while {($i < $el) && ($i < $anchor)} {\n\
if {[lsearch $tkPriv(listboxSelection) $i] >= 0} {\n\
$w selection set $i\n\
}\n\
incr i\n\
}\n\
while {($i > $el) && ($i > $anchor)} {\n\
if {[lsearch $tkPriv(listboxSelection) $i] >= 0} {\n\
$w selection set $i\n\
}\n\
incr i -1\n\
}\n\
set tkPriv(listboxPrev) $el\n\
event generate $w <<ListboxSelect>>\n\
}\n\
}\n\
}\n\
\n\
\n\
proc tkListboxBeginExtend {w el} {\n\
if {[string equal [$w cget -selectmode] \"extended\"]} {\n\
if {[$w selection includes anchor]} {\n\
tkListboxMotion $w $el\n\
} else {\n\
tkListboxBeginSelect $w $el\n\
}\n\
}\n\
}\n\
\n\
\n\
proc tkListboxBeginToggle {w el} {\n\
global tkPriv\n\
if {[string equal [$w cget -selectmode] \"extended\"]} {\n\
set tkPriv(listboxSelection) [$w curselection]\n\
set tkPriv(listboxPrev) $el\n\
$w selection anchor $el\n\
if {[$w selection includes $el]} {\n\
$w selection clear $el\n\
} else {\n\
$w selection set $el\n\
}\n\
event generate $w <<ListboxSelect>>\n\
}\n\
}\n\
\n\
\n\
proc tkListboxAutoScan {w} {\n\
global tkPriv\n\
if {![winfo exists $w]} return\n\
set x $tkPriv(x)\n\
set y $tkPriv(y)\n\
if {$y >= [winfo height $w]} {\n\
$w yview scroll 1 units\n\
} elseif {$y < 0} {\n\
$w yview scroll -1 units\n\
} elseif {$x >= [winfo width $w]} {\n\
$w xview scroll 2 units\n\
} elseif {$x < 0} {\n\
$w xview scroll -2 units\n\
} else {\n\
return\n\
}\n\
tkListboxMotion $w [$w index @$x,$y]\n\
set tkPriv(afterId) [after 50 [list tkListboxAutoScan $w]]\n\
}\n\
\n\
\n\
proc tkListboxUpDown {w amount} {\n\
global tkPriv\n\
$w activate [expr {[$w index active] + $amount}]\n\
$w see active\n\
switch [$w cget -selectmode] {\n\
browse {\n\
$w selection clear 0 end\n\
$w selection set active\n\
event generate $w <<ListboxSelect>>\n\
}\n\
extended {\n\
$w selection clear 0 end\n\
$w selection set active\n\
$w selection anchor active\n\
set tkPriv(listboxPrev) [$w index active]\n\
set tkPriv(listboxSelection) {}\n\
event generate $w <<ListboxSelect>>\n\
}\n\
}\n\
}\n\
\n\
\n\
proc tkListboxExtendUpDown {w amount} {\n\
if {[string compare [$w cget -selectmode] \"extended\"]} {\n\
return\n\
}\n\
set active [$w index active]\n\
if {![info exists tkPriv(listboxSelection)]} {\n\
global tkPriv\n\
$w selection set $active\n\
set tkPriv(listboxSelection) [$w curselection]\n\
}\n\
$w activate [expr {$active + $amount}]\n\
$w see active\n\
tkListboxMotion $w [$w index active]\n\
}\n\
\n\
\n\
proc tkListboxDataExtend {w el} {\n\
set mode [$w cget -selectmode]\n\
if {[string equal $mode \"extended\"]} {\n\
$w activate $el\n\
$w see $el\n\
if {[$w selection includes anchor]} {\n\
tkListboxMotion $w $el\n\
}\n\
} elseif {[string equal $mode \"multiple\"]} {\n\
$w activate $el\n\
$w see $el\n\
}\n\
}\n\
\n\
\n\
proc tkListboxCancel w {\n\
global tkPriv\n\
if {[string compare [$w cget -selectmode] \"extended\"]} {\n\
return\n\
}\n\
set first [$w index anchor]\n\
set last $tkPriv(listboxPrev)\n\
if { [string equal $last \"\"] } {\n\
return\n\
}\n\
if {$first > $last} {\n\
set tmp $first\n\
set first $last\n\
set last $tmp\n\
}\n\
$w selection clear $first $last\n\
while {$first <= $last} {\n\
if {[lsearch $tkPriv(listboxSelection) $first] >= 0} {\n\
$w selection set $first\n\
}\n\
incr first\n\
}\n\
event generate $w <<ListboxSelect>>\n\
}\n\
\n\
\n\
proc tkListboxSelectAll w {\n\
set mode [$w cget -selectmode]\n\
if {[string equal $mode \"single\"] || [string equal $mode \"browse\"]} {\n\
$w selection clear 0 end\n\
$w selection set active\n\
} else {\n\
$w selection set 0 end\n\
}\n\
event generate $w <<ListboxSelect>>\n\
}\n\
\n\
\n\
\n\
\n\
bind Menubutton <FocusIn> {}\n\
bind Menubutton <Enter> {\n\
tkMbEnter %W\n\
}\n\
bind Menubutton <Leave> {\n\
tkMbLeave %W\n\
}\n\
bind Menubutton <1> {\n\
if {[string compare $tkPriv(inMenubutton) \"\"]} {\n\
tkMbPost $tkPriv(inMenubutton) %X %Y\n\
}\n\
}\n\
bind Menubutton <Motion> {\n\
tkMbMotion %W up %X %Y\n\
}\n\
bind Menubutton <B1-Motion> {\n\
tkMbMotion %W down %X %Y\n\
}\n\
bind Menubutton <ButtonRelease-1> {\n\
tkMbButtonUp %W\n\
}\n\
bind Menubutton <space> {\n\
tkMbPost %W\n\
tkMenuFirstEntry [%W cget -menu]\n\
}\n\
\n\
\n\
bind Menu <FocusIn> {}\n\
\n\
bind Menu <Enter> {\n\
set tkPriv(window) %W\n\
if {[string equal [%W cget -type] \"tearoff\"]} {\n\
if {[string compare \"%m\" \"NotifyUngrab\"]} {\n\
if {[string equal $tcl_platform(platform) \"unix\"]} {\n\
tk_menuSetFocus %W\n\
}\n\
}\n\
}\n\
tkMenuMotion %W %x %y %s\n\
}\n\
\n\
bind Menu <Leave> {\n\
tkMenuLeave %W %X %Y %s\n\
}\n\
bind Menu <Motion> {\n\
tkMenuMotion %W %x %y %s\n\
}\n\
bind Menu <ButtonPress> {\n\
tkMenuButtonDown %W\n\
}\n\
bind Menu <ButtonRelease> {\n\
tkMenuInvoke %W 1\n\
}\n\
bind Menu <space> {\n\
tkMenuInvoke %W 0\n\
}\n\
bind Menu <Return> {\n\
tkMenuInvoke %W 0\n\
}\n\
bind Menu <Escape> {\n\
tkMenuEscape %W\n\
}\n\
bind Menu <Left> {\n\
tkMenuLeftArrow %W\n\
}\n\
bind Menu <Right> {\n\
tkMenuRightArrow %W\n\
}\n\
bind Menu <Up> {\n\
tkMenuUpArrow %W\n\
}\n\
bind Menu <Down> {\n\
tkMenuDownArrow %W\n\
}\n\
bind Menu <KeyPress> {\n\
tkTraverseWithinMenu %W %A\n\
}\n\
\n\
\n\
if {[string equal $tcl_platform(platform) \"unix\"]} {\n\
bind all <Alt-KeyPress> {\n\
tkTraverseToMenu %W %A\n\
}\n\
\n\
bind all <F10> {\n\
tkFirstMenu %W\n\
}\n\
} else {\n\
bind Menubutton <Alt-KeyPress> {\n\
tkTraverseToMenu %W %A\n\
}\n\
\n\
bind Menubutton <F10> {\n\
tkFirstMenu %W\n\
}\n\
}\n\
\n\
\n\
proc tkMbEnter w {\n\
global tkPriv\n\
\n\
if {[string compare $tkPriv(inMenubutton) \"\"]} {\n\
tkMbLeave $tkPriv(inMenubutton)\n\
}\n\
set tkPriv(inMenubutton) $w\n\
if {[string compare [$w cget -state] \"disabled\"]} {\n\
$w configure -state active\n\
}\n\
}\n\
\n\
\n\
proc tkMbLeave w {\n\
global tkPriv\n\
\n\
set tkPriv(inMenubutton) {}\n\
if {![winfo exists $w]} {\n\
return\n\
}\n\
if {[string equal [$w cget -state] \"active\"]} {\n\
$w configure -state normal\n\
}\n\
}\n\
\n\
\n\
proc tkMbPost {w {x {}} {y {}}} {\n\
global tkPriv errorInfo\n\
global tcl_platform\n\
\n\
if {[string equal [$w cget -state] \"disabled\"] || \\\n\
[string equal $w $tkPriv(postedMb)]} {\n\
return\n\
}\n\
set menu [$w cget -menu]\n\
if {[string equal $menu \"\"]} {\n\
return\n\
}\n\
set tearoff [expr {[string equal $tcl_platform(platform) \"unix\"] \\\n\
|| [string equal [$menu cget -type] \"tearoff\"]}]\n\
if {[string first $w $menu] != 0} {\n\
error \"can't post $menu:  it isn't a descendant of $w (this is a new requirement in Tk versions 3.0 and later)\"\n\
}\n\
set cur $tkPriv(postedMb)\n\
if {[string compare $cur \"\"]} {\n\
tkMenuUnpost {}\n\
}\n\
set tkPriv(cursor) [$w cget -cursor]\n\
set tkPriv(relief) [$w cget -relief]\n\
$w configure -cursor arrow\n\
$w configure -relief raised\n\
\n\
set tkPriv(postedMb) $w\n\
set tkPriv(focus) [focus]\n\
$menu activate none\n\
tkGenerateMenuSelect $menu\n\
\n\
\n\
update idletasks\n\
if {[catch {\n\
switch [$w cget -direction] {\n\
above {\n\
set x [winfo rootx $w]\n\
set y [expr {[winfo rooty $w] - [winfo reqheight $menu]}]\n\
$menu post $x $y\n\
}\n\
below {\n\
set x [winfo rootx $w]\n\
set y [expr {[winfo rooty $w] + [winfo height $w]}]\n\
$menu post $x $y\n\
}\n\
left {\n\
set x [expr {[winfo rootx $w] - [winfo reqwidth $menu]}]\n\
set y [expr {(2 * [winfo rooty $w] + [winfo height $w]) / 2}]\n\
set entry [tkMenuFindName $menu [$w cget -text]]\n\
if {[$w cget -indicatoron]} {\n\
if {$entry == [$menu index last]} {\n\
incr y [expr {-([$menu yposition $entry] \\\n\
+ [winfo reqheight $menu])/2}]\n\
} else {\n\
incr y [expr {-([$menu yposition $entry] \\\n\
+ [$menu yposition [expr {$entry+1}]])/2}]\n\
}\n\
}\n\
$menu post $x $y\n\
if {[string compare $entry {}] && [string compare [$menu entrycget $entry -state] \"disabled\"]} {\n\
$menu activate $entry\n\
tkGenerateMenuSelect $menu\n\
}\n\
}\n\
right {\n\
set x [expr {[winfo rootx $w] + [winfo width $w]}]\n\
set y [expr {(2 * [winfo rooty $w] + [winfo height $w]) / 2}]\n\
set entry [tkMenuFindName $menu [$w cget -text]]\n\
if {[$w cget -indicatoron]} {\n\
if {$entry == [$menu index last]} {\n\
incr y [expr {-([$menu yposition $entry] \\\n\
+ [winfo reqheight $menu])/2}]\n\
} else {\n\
incr y [expr {-([$menu yposition $entry] \\\n\
+ [$menu yposition [expr {$entry+1}]])/2}]\n\
}\n\
}\n\
$menu post $x $y\n\
if {[string compare $entry {}] && [string compare [$menu entrycget $entry -state] \"disabled\"]} {\n\
$menu activate $entry\n\
tkGenerateMenuSelect $menu\n\
}\n\
}\n\
default {\n\
if {[$w cget -indicatoron]} {\n\
if {[string equal $y {}]} {\n\
set x [expr {[winfo rootx $w] + [winfo width $w]/2}]\n\
set y [expr {[winfo rooty $w] + [winfo height $w]/2}]\n\
}\n\
tkPostOverPoint $menu $x $y [tkMenuFindName $menu [$w cget -text]]\n\
} else {\n\
$menu post [winfo rootx $w] [expr {[winfo rooty $w]+[winfo height $w]}]\n\
}  \n\
}\n\
}\n\
} msg]} {\n\
\n\
set savedInfo $errorInfo\n\
tkMenuUnpost {}\n\
error $msg $savedInfo\n\
\n\
}\n\
\n\
set tkPriv(tearoff) $tearoff\n\
if {$tearoff != 0} {\n\
focus $menu\n\
if {[winfo viewable $w]} {\n\
tkSaveGrabInfo $w\n\
grab -global $w\n\
}\n\
}\n\
}\n\
\n\
\n\
proc tkMenuUnpost menu {\n\
global tcl_platform\n\
global tkPriv\n\
set mb $tkPriv(postedMb)\n\
\n\
\n\
catch {focus $tkPriv(focus)}\n\
set tkPriv(focus) \"\"\n\
\n\
\n\
catch {\n\
if {[string compare $mb \"\"]} {\n\
set menu [$mb cget -menu]\n\
$menu unpost\n\
set tkPriv(postedMb) {}\n\
$mb configure -cursor $tkPriv(cursor)\n\
$mb configure -relief $tkPriv(relief)\n\
} elseif {[string compare $tkPriv(popup) \"\"]} {\n\
$tkPriv(popup) unpost\n\
set tkPriv(popup) {}\n\
} elseif {[string compare [$menu cget -type] \"menubar\"] \\\n\
&& [string compare [$menu cget -type] \"tearoff\"]} {\n\
\n\
while {1} {\n\
set parent [winfo parent $menu]\n\
if {[string compare [winfo class $parent] \"Menu\"] \\\n\
|| ![winfo ismapped $parent]} {\n\
break\n\
}\n\
$parent activate none\n\
$parent postcascade none\n\
tkGenerateMenuSelect $parent\n\
set type [$parent cget -type]\n\
if {[string equal $type \"menubar\"] || \\\n\
[string equal $type \"tearoff\"]} {\n\
break\n\
}\n\
set menu $parent\n\
}\n\
if {[string compare [$menu cget -type] \"menubar\"]} {\n\
$menu unpost\n\
}\n\
}\n\
}\n\
\n\
if {($tkPriv(tearoff) != 0) || [string compare $tkPriv(menuBar) \"\"]} {\n\
if {[string compare $menu \"\"]} {\n\
set grab [grab current $menu]\n\
if {[string compare $grab \"\"]} {\n\
grab release $grab\n\
}\n\
}\n\
tkRestoreOldGrab\n\
if {[string compare $tkPriv(menuBar) \"\"]} {\n\
$tkPriv(menuBar) configure -cursor $tkPriv(cursor)\n\
set tkPriv(menuBar) {}\n\
}\n\
if {[string compare $tcl_platform(platform) \"unix\"]} {\n\
set tkPriv(tearoff) 0\n\
}\n\
}\n\
}\n\
\n\
\n\
proc tkMbMotion {w upDown rootx rooty} {\n\
global tkPriv\n\
\n\
if {[string equal $tkPriv(inMenubutton) $w]} {\n\
return\n\
}\n\
set new [winfo containing $rootx $rooty]\n\
if {[string compare $new $tkPriv(inMenubutton)] \\\n\
&& ([string equal $new \"\"] \\\n\
|| [string equal [winfo toplevel $new] [winfo toplevel $w]])} {\n\
if {[string compare $tkPriv(inMenubutton) \"\"]} {\n\
tkMbLeave $tkPriv(inMenubutton)\n\
}\n\
if {[string compare $new \"\"] \\\n\
&& [string equal [winfo class $new] \"Menubutton\"] \\\n\
&& ([$new cget -indicatoron] == 0) \\\n\
&& ([$w cget -indicatoron] == 0)} {\n\
if {[string equal $upDown \"down\"]} {\n\
tkMbPost $new $rootx $rooty\n\
} else {\n\
tkMbEnter $new\n\
}\n\
}\n\
}\n\
}\n\
\n\
\n\
proc tkMbButtonUp w {\n\
global tkPriv\n\
global tcl_platform\n\
\n\
set menu [$w cget -menu]\n\
set tearoff [expr {[string equal $tcl_platform(platform) \"unix\"] || \\\n\
([string compare $menu {}] && \\\n\
[string equal [$menu cget -type] \"tearoff\"])}]\n\
if {($tearoff != 0) && [string equal $tkPriv(postedMb) $w] \\\n\
&& [string equal $tkPriv(inMenubutton) $w]} {\n\
tkMenuFirstEntry [$tkPriv(postedMb) cget -menu]\n\
} else {\n\
tkMenuUnpost {}\n\
}\n\
}\n\
\n\
\n\
proc tkMenuMotion {menu x y state} {\n\
global tkPriv\n\
if {[string equal $menu $tkPriv(window)]} {\n\
if {[string equal [$menu cget -type] \"menubar\"]} {\n\
if {[info exists tkPriv(focus)] && \\\n\
[string compare $menu $tkPriv(focus)]} {\n\
$menu activate @$x,$y\n\
tkGenerateMenuSelect $menu\n\
}\n\
} else {\n\
$menu activate @$x,$y\n\
tkGenerateMenuSelect $menu\n\
}\n\
}\n\
if {($state & 0x1f00) != 0} {\n\
$menu postcascade active\n\
}\n\
}\n\
\n\
\n\
proc tkMenuButtonDown menu {\n\
global tkPriv\n\
global tcl_platform\n\
\n\
if {![winfo viewable $menu]} {\n\
return\n\
}\n\
$menu postcascade active\n\
if {[string compare $tkPriv(postedMb) \"\"] && \\\n\
[winfo viewable $tkPriv(postedMb)]} {\n\
grab -global $tkPriv(postedMb)\n\
} else {\n\
while {[string equal [$menu cget -type] \"normal\"] \\\n\
&& [string equal [winfo class [winfo parent $menu]] \"Menu\"] \\\n\
&& [winfo ismapped [winfo parent $menu]]} {\n\
set menu [winfo parent $menu]\n\
}\n\
\n\
if {[string equal $tkPriv(menuBar) {}]} {\n\
set tkPriv(menuBar) $menu\n\
set tkPriv(cursor) [$menu cget -cursor]\n\
$menu configure -cursor arrow\n\
}\n\
\n\
\n\
if {[string compare $menu [grab current $menu]]} {\n\
tkSaveGrabInfo $menu\n\
}\n\
\n\
\n\
if {[string equal $tcl_platform(platform) \"unix\"]} {\n\
grab -global $menu\n\
}\n\
}\n\
}\n\
\n\
\n\
proc tkMenuLeave {menu rootx rooty state} {\n\
global tkPriv\n\
set tkPriv(window) {}\n\
if {[string equal [$menu index active] \"none\"]} {\n\
return\n\
}\n\
if {[string equal [$menu type active] \"cascade\"]\n\
&& [string equal [winfo containing $rootx $rooty] \\\n\
[$menu entrycget active -menu]]} {\n\
return\n\
}\n\
$menu activate none\n\
tkGenerateMenuSelect $menu\n\
}\n\
\n\
\n\
proc tkMenuInvoke {w buttonRelease} {\n\
global tkPriv\n\
\n\
if {$buttonRelease && [string equal $tkPriv(window) {}]} {\n\
\n\
$w postcascade none\n\
$w activate none\n\
event generate $w <<MenuSelect>>\n\
tkMenuUnpost $w\n\
return\n\
}\n\
if {[string equal [$w type active] \"cascade\"]} {\n\
$w postcascade active\n\
set menu [$w entrycget active -menu]\n\
tkMenuFirstEntry $menu\n\
} elseif {[string equal [$w type active] \"tearoff\"]} {\n\
tkTearOffMenu $w\n\
tkMenuUnpost $w\n\
} elseif {[string equal [$w cget -type] \"menubar\"]} {\n\
$w postcascade none\n\
set active [$w index active]\n\
set isCascade [string equal [$w type $active] \"cascade\"]\n\
\n\
\n\
if { $isCascade } {\n\
$w activate none\n\
event generate $w <<MenuSelect>>\n\
}\n\
\n\
tkMenuUnpost $w\n\
\n\
\n\
if { !$isCascade } {\n\
uplevel #0 [list $w invoke $active]\n\
}\n\
} else {\n\
tkMenuUnpost $w\n\
uplevel #0 [list $w invoke active]\n\
}\n\
}\n\
\n\
\n\
proc tkMenuEscape menu {\n\
set parent [winfo parent $menu]\n\
if {[string compare [winfo class $parent] \"Menu\"]} {\n\
tkMenuUnpost $menu\n\
} elseif {[string equal [$parent cget -type] \"menubar\"]} {\n\
tkMenuUnpost $menu\n\
tkRestoreOldGrab\n\
} else {\n\
tkMenuNextMenu $menu left\n\
}\n\
}\n\
\n\
\n\
proc tkMenuUpArrow {menu} {\n\
if {[string equal [$menu cget -type] \"menubar\"]} {\n\
tkMenuNextMenu $menu left\n\
} else {\n\
tkMenuNextEntry $menu -1\n\
}\n\
}\n\
\n\
proc tkMenuDownArrow {menu} {\n\
if {[string equal [$menu cget -type] \"menubar\"]} {\n\
tkMenuNextMenu $menu right\n\
} else {\n\
tkMenuNextEntry $menu 1\n\
}\n\
}\n\
\n\
proc tkMenuLeftArrow {menu} {\n\
if {[string equal [$menu cget -type] \"menubar\"]} {\n\
tkMenuNextEntry $menu -1\n\
} else {\n\
tkMenuNextMenu $menu left\n\
}\n\
}\n\
\n\
proc tkMenuRightArrow {menu} {\n\
if {[string equal [$menu cget -type] \"menubar\"]} {\n\
tkMenuNextEntry $menu 1\n\
} else {\n\
tkMenuNextMenu $menu right\n\
}\n\
}\n\
\n\
\n\
proc tkMenuNextMenu {menu direction} {\n\
global tkPriv\n\
\n\
\n\
if {[string equal $direction \"right\"]} {\n\
set count 1\n\
set parent [winfo parent $menu]\n\
set class [winfo class $parent]\n\
if {[string equal [$menu type active] \"cascade\"]} {\n\
$menu postcascade active\n\
set m2 [$menu entrycget active -menu]\n\
if {[string compare $m2 \"\"]} {\n\
tkMenuFirstEntry $m2\n\
}\n\
return\n\
} else {\n\
set parent [winfo parent $menu]\n\
while {[string compare $parent \".\"]} {\n\
if {[string equal [winfo class $parent] \"Menu\"] \\\n\
&& [string equal [$parent cget -type] \"menubar\"]} {\n\
tk_menuSetFocus $parent\n\
tkMenuNextEntry $parent 1\n\
return\n\
}\n\
set parent [winfo parent $parent]\n\
}\n\
}\n\
} else {\n\
set count -1\n\
set m2 [winfo parent $menu]\n\
if {[string equal [winfo class $m2] \"Menu\"]} {\n\
if {[string compare [$m2 cget -type] \"menubar\"]} {\n\
$menu activate none\n\
tkGenerateMenuSelect $menu\n\
tk_menuSetFocus $m2\n\
\n\
\n\
set tmp [$m2 index active]\n\
$m2 activate none\n\
$m2 activate $tmp\n\
return\n\
}\n\
}\n\
}\n\
\n\
\n\
set m2 [winfo parent $menu]\n\
if {[string equal [winfo class $m2] \"Menu\"]} {\n\
if {[string equal [$m2 cget -type] \"menubar\"]} {\n\
tk_menuSetFocus $m2\n\
tkMenuNextEntry $m2 -1\n\
return\n\
}\n\
}\n\
\n\
set w $tkPriv(postedMb)\n\
if {[string equal $w \"\"]} {\n\
return\n\
}\n\
set buttons [winfo children [winfo parent $w]]\n\
set length [llength $buttons]\n\
set i [expr {[lsearch -exact $buttons $w] + $count}]\n\
while {1} {\n\
while {$i < 0} {\n\
incr i $length\n\
}\n\
while {$i >= $length} {\n\
incr i -$length\n\
}\n\
set mb [lindex $buttons $i]\n\
if {[string equal [winfo class $mb] \"Menubutton\"] \\\n\
&& [string compare [$mb cget -state] \"disabled\"] \\\n\
&& [string compare [$mb cget -menu] \"\"] \\\n\
&& [string compare [[$mb cget -menu] index last] \"none\"]} {\n\
break\n\
}\n\
if {[string equal $mb $w]} {\n\
return\n\
}\n\
incr i $count\n\
}\n\
tkMbPost $mb\n\
tkMenuFirstEntry [$mb cget -menu]\n\
}\n\
\n\
\n\
proc tkMenuNextEntry {menu count} {\n\
global tkPriv\n\
\n\
if {[string equal [$menu index last] \"none\"]} {\n\
return\n\
}\n\
set length [expr {[$menu index last]+1}]\n\
set quitAfter $length\n\
set active [$menu index active]\n\
if {[string equal $active \"none\"]} {\n\
set i 0\n\
} else {\n\
set i [expr {$active + $count}]\n\
}\n\
while {1} {\n\
if {$quitAfter <= 0} {\n\
\n\
return\n\
}\n\
while {$i < 0} {\n\
incr i $length\n\
}\n\
while {$i >= $length} {\n\
incr i -$length\n\
}\n\
if {[catch {$menu entrycget $i -state} state] == 0} {\n\
if {[string compare $state \"disabled\"]} {\n\
break\n\
}\n\
}\n\
if {$i == $active} {\n\
return\n\
}\n\
incr i $count\n\
incr quitAfter -1\n\
}\n\
$menu activate $i\n\
tkGenerateMenuSelect $menu\n\
if {[string equal [$menu type $i] \"cascade\"]} {\n\
set cascade [$menu entrycget $i -menu]\n\
if {[string compare $cascade \"\"]} {\n\
$menu postcascade $i\n\
tkMenuFirstEntry $cascade\n\
}\n\
}\n\
}\n\
\n\
\n\
proc tkMenuFind {w char} {\n\
global tkPriv\n\
set char [string tolower $char]\n\
set windowlist [winfo child $w]\n\
\n\
foreach child $windowlist {\n\
if {[string compare [winfo toplevel [focus]] \\\n\
[winfo toplevel $child]]} {\n\
continue\n\
}\n\
if {[string equal [winfo class $child] \"Menu\"] && \\\n\
[string equal [$child cget -type] \"menubar\"]} {\n\
if {[string equal $char \"\"]} {\n\
return $child\n\
}\n\
set last [$child index last]\n\
for {set i [$child cget -tearoff]} {$i <= $last} {incr i} {\n\
if {[string equal [$child type $i] \"separator\"]} {\n\
continue\n\
}\n\
set char2 [string index [$child entrycget $i -label] \\\n\
[$child entrycget $i -underline]]\n\
if {[string equal $char [string tolower $char2]] \\\n\
|| [string equal $char \"\"]} {\n\
if {[string compare [$child entrycget $i -state] \"disabled\"]} {\n\
return $child\n\
}\n\
}\n\
}\n\
}\n\
}\n\
\n\
foreach child $windowlist {\n\
if {[string compare [winfo toplevel [focus]] \\\n\
[winfo toplevel $child]]} {\n\
continue\n\
}\n\
switch [winfo class $child] {\n\
Menubutton {\n\
set char2 [string index [$child cget -text] \\\n\
[$child cget -underline]]\n\
if {[string equal $char [string tolower $char2]] \\\n\
|| [string equal $char \"\"]} {\n\
if {[string compare [$child cget -state] \"disabled\"]} {\n\
return $child\n\
}\n\
}\n\
}\n\
\n\
default {\n\
set match [tkMenuFind $child $char]\n\
if {[string compare $match \"\"]} {\n\
return $match\n\
}\n\
}\n\
}\n\
}\n\
return {}\n\
}\n\
\n\
\n\
proc tkTraverseToMenu {w char} {\n\
global tkPriv\n\
if {[string equal $char \"\"]} {\n\
return\n\
}\n\
while {[string equal [winfo class $w] \"Menu\"]} {\n\
if {[string compare [$w cget -type] \"menubar\"] \\\n\
&& [string equal $tkPriv(postedMb) \"\"]} {\n\
return\n\
}\n\
if {[string equal [$w cget -type] \"menubar\"]} {\n\
break\n\
}\n\
set w [winfo parent $w]\n\
}\n\
set w [tkMenuFind [winfo toplevel $w] $char]\n\
if {[string compare $w \"\"]} {\n\
if {[string equal [winfo class $w] \"Menu\"]} {\n\
tk_menuSetFocus $w\n\
set tkPriv(window) $w\n\
tkSaveGrabInfo $w\n\
grab -global $w\n\
tkTraverseWithinMenu $w $char\n\
} else {\n\
tkMbPost $w\n\
tkMenuFirstEntry [$w cget -menu]\n\
}\n\
}\n\
}\n\
\n\
\n\
proc tkFirstMenu w {\n\
set w [tkMenuFind [winfo toplevel $w] \"\"]\n\
if {[string compare $w \"\"]} {\n\
if {[string equal [winfo class $w] \"Menu\"]} {\n\
tk_menuSetFocus $w\n\
set tkPriv(window) $w\n\
tkSaveGrabInfo $w\n\
grab -global $w\n\
tkMenuFirstEntry $w\n\
} else {\n\
tkMbPost $w\n\
tkMenuFirstEntry [$w cget -menu]\n\
}\n\
}\n\
}\n\
\n\
\n\
proc tkTraverseWithinMenu {w char} {\n\
if {[string equal $char \"\"]} {\n\
return\n\
}\n\
set char [string tolower $char]\n\
set last [$w index last]\n\
if {[string equal $last \"none\"]} {\n\
return\n\
}\n\
for {set i 0} {$i <= $last} {incr i} {\n\
if {[catch {set char2 [string index \\\n\
[$w entrycget $i -label] [$w entrycget $i -underline]]}]} {\n\
continue\n\
}\n\
if {[string equal $char [string tolower $char2]]} {\n\
if {[string equal [$w type $i] \"cascade\"]} {\n\
$w activate $i\n\
$w postcascade active\n\
event generate $w <<MenuSelect>>\n\
set m2 [$w entrycget $i -menu]\n\
if {[string compare $m2 \"\"]} {\n\
tkMenuFirstEntry $m2\n\
}\n\
} else {\n\
tkMenuUnpost $w\n\
uplevel #0 [list $w invoke $i]\n\
}\n\
return\n\
}\n\
}\n\
}\n\
\n\
\n\
proc tkMenuFirstEntry menu {\n\
if {[string equal $menu \"\"]} {\n\
return\n\
}\n\
tk_menuSetFocus $menu\n\
if {[string compare [$menu index active] \"none\"]} {\n\
return\n\
}\n\
set last [$menu index last]\n\
if {[string equal $last \"none\"]} {\n\
return\n\
}\n\
for {set i 0} {$i <= $last} {incr i} {\n\
if {([catch {set state [$menu entrycget $i -state]}] == 0) \\\n\
&& [string compare $state \"disabled\"] \\\n\
&& [string compare [$menu type $i] \"tearoff\"]} {\n\
$menu activate $i\n\
tkGenerateMenuSelect $menu\n\
if {[string equal [$menu type $i] \"cascade\"] && \\\n\
[string equal [$menu cget -type] \"menubar\"]} {\n\
set cascade [$menu entrycget $i -menu]\n\
if {[string compare $cascade \"\"]} {\n\
$menu postcascade $i\n\
tkMenuFirstEntry $cascade\n\
}\n\
}\n\
return\n\
}\n\
}\n\
}\n\
\n\
\n\
proc tkMenuFindName {menu s} {\n\
set i \"\"\n\
if {![regexp {^active$|^last$|^none$|^[0-9]|^@} $s]} {\n\
catch {set i [$menu index $s]}\n\
return $i\n\
}\n\
set last [$menu index last]\n\
if {[string equal $last \"none\"]} {\n\
return\n\
}\n\
for {set i 0} {$i <= $last} {incr i} {\n\
if {![catch {$menu entrycget $i -label} label]} {\n\
if {[string equal $label $s]} {\n\
return $i\n\
}\n\
}\n\
}\n\
return \"\"\n\
}\n\
\n\
\n\
proc tkPostOverPoint {menu x y {entry {}}}  {\n\
global tcl_platform\n\
\n\
if {[string compare $entry {}]} {\n\
if {$entry == [$menu index last]} {\n\
incr y [expr {-([$menu yposition $entry] \\\n\
+ [winfo reqheight $menu])/2}]\n\
} else {\n\
incr y [expr {-([$menu yposition $entry] \\\n\
+ [$menu yposition [expr {$entry+1}]])/2}]\n\
}\n\
incr x [expr {-[winfo reqwidth $menu]/2}]\n\
}\n\
$menu post $x $y\n\
if {[string compare $entry {}] \\\n\
&& [string compare [$menu entrycget $entry -state] \"disabled\"]} {\n\
$menu activate $entry\n\
tkGenerateMenuSelect $menu\n\
}\n\
}\n\
\n\
\n\
proc tkSaveGrabInfo w {\n\
global tkPriv\n\
set tkPriv(oldGrab) [grab current $w]\n\
if {[string compare $tkPriv(oldGrab) \"\"]} {\n\
set tkPriv(grabStatus) [grab status $tkPriv(oldGrab)]\n\
}\n\
}\n\
\n\
\n\
proc tkRestoreOldGrab {} {\n\
global tkPriv\n\
\n\
if {[string compare $tkPriv(oldGrab) \"\"]} {\n\
\n\
\n\
catch {\n\
if {[string equal $tkPriv(grabStatus) \"global\"]} {\n\
grab set -global $tkPriv(oldGrab)\n\
} else {\n\
grab set $tkPriv(oldGrab)\n\
}\n\
}\n\
set tkPriv(oldGrab) \"\"\n\
}\n\
}\n\
\n\
proc tk_menuSetFocus {menu} {\n\
global tkPriv\n\
if {![info exists tkPriv(focus)] || [string equal $tkPriv(focus) {}]} {\n\
set tkPriv(focus) [focus]\n\
}\n\
focus $menu\n\
}\n\
\n\
proc tkGenerateMenuSelect {menu} {\n\
global tkPriv\n\
\n\
if {[string equal $tkPriv(activeMenu) $menu] \\\n\
&& [string equal $tkPriv(activeItem) [$menu index active]]} {\n\
return\n\
}\n\
\n\
set tkPriv(activeMenu) $menu\n\
set tkPriv(activeItem) [$menu index active]\n\
event generate $menu <<MenuSelect>>\n\
}\n\
\n\
\n\
proc tk_popup {menu x y {entry {}}} {\n\
global tkPriv\n\
global tcl_platform\n\
if {[string compare $tkPriv(popup) \"\"] \\\n\
|| [string compare $tkPriv(postedMb) \"\"]} {\n\
tkMenuUnpost {}\n\
}\n\
tkPostOverPoint $menu $x $y $entry\n\
if {[string equal $tcl_platform(platform) \"unix\"] \\\n\
&& [winfo viewable $menu]} {\n\
tkSaveGrabInfo $menu\n\
grab -global $menu\n\
set tkPriv(popup) $menu\n\
tk_menuSetFocus $menu\n\
}\n\
}\n\
\n\
\n\
proc tk_setPalette {args} {\n\
if {[winfo depth .] == 1} {\n\
return\n\
}\n\
\n\
global tkPalette\n\
\n\
\n\
if {[llength $args] == 1} {\n\
set new(background) [lindex $args 0]\n\
} else {\n\
array set new $args\n\
}\n\
if {![info exists new(background)]} {\n\
error \"must specify a background color\"\n\
}\n\
if {![info exists new(foreground)]} {\n\
set new(foreground) black\n\
}\n\
set bg [winfo rgb . $new(background)]\n\
set fg [winfo rgb . $new(foreground)]\n\
set darkerBg [format #%02x%02x%02x [expr {(9*[lindex $bg 0])/2560}] \\\n\
[expr {(9*[lindex $bg 1])/2560}] [expr {(9*[lindex $bg 2])/2560}]]\n\
foreach i {activeForeground insertBackground selectForeground \\\n\
highlightColor} {\n\
if {![info exists new($i)]} {\n\
set new($i) $new(foreground)\n\
}\n\
}\n\
if {![info exists new(disabledForeground)]} {\n\
set new(disabledForeground) [format #%02x%02x%02x \\\n\
[expr {(3*[lindex $bg 0] + [lindex $fg 0])/1024}] \\\n\
[expr {(3*[lindex $bg 1] + [lindex $fg 1])/1024}] \\\n\
[expr {(3*[lindex $bg 2] + [lindex $fg 2])/1024}]]\n\
}\n\
if {![info exists new(highlightBackground)]} {\n\
set new(highlightBackground) $new(background)\n\
}\n\
if {![info exists new(activeBackground)]} {\n\
\n\
foreach i {0 1 2} {\n\
set light($i) [expr {[lindex $bg $i]/256}]\n\
set inc1 [expr {($light($i)*15)/100}]\n\
set inc2 [expr {(255-$light($i))/3}]\n\
if {$inc1 > $inc2} {\n\
incr light($i) $inc1\n\
} else {\n\
incr light($i) $inc2\n\
}\n\
if {$light($i) > 255} {\n\
set light($i) 255\n\
}\n\
}\n\
set new(activeBackground) [format #%02x%02x%02x $light(0) \\\n\
$light(1) $light(2)]\n\
}\n\
if {![info exists new(selectBackground)]} {\n\
set new(selectBackground) $darkerBg\n\
}\n\
if {![info exists new(troughColor)]} {\n\
set new(troughColor) $darkerBg\n\
}\n\
if {![info exists new(selectColor)]} {\n\
set new(selectColor) #b03060\n\
}\n\
\n\
toplevel .___tk_set_palette\n\
wm withdraw .___tk_set_palette\n\
foreach q {button canvas checkbutton entry frame label listbox \\\n\
menubutton menu message radiobutton scale scrollbar text} {\n\
$q .___tk_set_palette.$q\n\
}\n\
\n\
\n\
eval [tkRecolorTree . new]\n\
\n\
catch {destroy .___tk_set_palette}\n\
\n\
\n\
foreach option [array names new] {\n\
option add *$option $new($option) widgetDefault\n\
}\n\
\n\
\n\
array set tkPalette [array get new]\n\
}\n\
\n\
\n\
proc tkRecolorTree {w colors} {\n\
global tkPalette\n\
upvar $colors c\n\
set result {}\n\
foreach dbOption [array names c] {\n\
set option -[string tolower $dbOption]\n\
if {![catch {$w config $option} value]} {\n\
set defaultcolor [option get $w $dbOption widgetDefault]\n\
if {[string match {} $defaultcolor]} {\n\
set defaultcolor [winfo rgb . [lindex $value 3]]\n\
} else {\n\
set defaultcolor [winfo rgb . $defaultcolor]\n\
}\n\
set chosencolor [winfo rgb . [lindex $value 4]]\n\
if {[string match $defaultcolor $chosencolor]} {\n\
append result \";\\noption add [list \\\n\
*[winfo class $w].$dbOption $c($dbOption) 60]\"\n\
$w configure $option $c($dbOption)\n\
}\n\
}\n\
}\n\
foreach child [winfo children $w] {\n\
append result \";\\n[tkRecolorTree $child c]\"\n\
}\n\
return $result\n\
}\n\
\n\
\n\
proc tkDarken {color percent} {\n\
foreach {red green blue} [winfo rgb . $color] {\n\
set red [expr {($red/256)*$percent/100}]\n\
set green [expr {($green/256)*$percent/100}]\n\
set blue [expr {($blue/256)*$percent/100}]\n\
break\n\
}\n\
if {$red > 255} {\n\
set red 255\n\
}\n\
if {$green > 255} {\n\
set green 255\n\
}\n\
if {$blue > 255} {\n\
set blue 255\n\
}\n\
return [format \"#%02x%02x%02x\" $red $green $blue]\n\
}\n\
\n\
\n\
proc tk_bisque {} {\n\
tk_setPalette activeBackground #e6ceb1 activeForeground black \\\n\
background #ffe4c4 disabledForeground #b0b0b0 foreground black \\\n\
highlightBackground #ffe4c4 highlightColor black \\\n\
insertBackground black selectColor #b03060 \\\n\
selectBackground #e6ceb1 selectForeground black \\\n\
troughColor #cdb79e\n\
}\n\
\n\
\n\
\n\
bind Scale <Enter> {\n\
if {$tk_strictMotif} {\n\
set tkPriv(activeBg) [%W cget -activebackground]\n\
%W config -activebackground [%W cget -background]\n\
}\n\
tkScaleActivate %W %x %y\n\
}\n\
bind Scale <Motion> {\n\
tkScaleActivate %W %x %y\n\
}\n\
bind Scale <Leave> {\n\
if {$tk_strictMotif} {\n\
%W config -activebackground $tkPriv(activeBg)\n\
}\n\
if {[string equal [%W cget -state] \"active\"]} {\n\
%W configure -state normal\n\
}\n\
}\n\
bind Scale <1> {\n\
tkScaleButtonDown %W %x %y\n\
}\n\
bind Scale <B1-Motion> {\n\
tkScaleDrag %W %x %y\n\
}\n\
bind Scale <B1-Leave> { }\n\
bind Scale <B1-Enter> { }\n\
bind Scale <ButtonRelease-1> {\n\
tkCancelRepeat\n\
tkScaleEndDrag %W\n\
tkScaleActivate %W %x %y\n\
}\n\
bind Scale <2> {\n\
tkScaleButton2Down %W %x %y\n\
}\n\
bind Scale <B2-Motion> {\n\
tkScaleDrag %W %x %y\n\
}\n\
bind Scale <B2-Leave> { }\n\
bind Scale <B2-Enter> { }\n\
bind Scale <ButtonRelease-2> {\n\
tkCancelRepeat\n\
tkScaleEndDrag %W\n\
tkScaleActivate %W %x %y\n\
}\n\
bind Scale <Control-1> {\n\
tkScaleControlPress %W %x %y\n\
}\n\
bind Scale <Up> {\n\
tkScaleIncrement %W up little noRepeat\n\
}\n\
bind Scale <Down> {\n\
tkScaleIncrement %W down little noRepeat\n\
}\n\
bind Scale <Left> {\n\
tkScaleIncrement %W up little noRepeat\n\
}\n\
bind Scale <Right> {\n\
tkScaleIncrement %W down little noRepeat\n\
}\n\
bind Scale <Control-Up> {\n\
tkScaleIncrement %W up big noRepeat\n\
}\n\
bind Scale <Control-Down> {\n\
tkScaleIncrement %W down big noRepeat\n\
}\n\
bind Scale <Control-Left> {\n\
tkScaleIncrement %W up big noRepeat\n\
}\n\
bind Scale <Control-Right> {\n\
tkScaleIncrement %W down big noRepeat\n\
}\n\
bind Scale <Home> {\n\
%W set [%W cget -from]\n\
}\n\
bind Scale <End> {\n\
%W set [%W cget -to]\n\
}\n\
\n\
\n\
proc tkScaleActivate {w x y} {\n\
if {[string equal [$w cget -state] \"disabled\"]} {\n\
return\n\
}\n\
if {[string equal [$w identify $x $y] \"slider\"]} {\n\
set state active\n\
} else {\n\
set state normal\n\
}\n\
if {[string compare [$w cget -state] $state]} {\n\
$w configure -state $state\n\
}\n\
}\n\
\n\
\n\
proc tkScaleButtonDown {w x y} {\n\
global tkPriv\n\
set tkPriv(dragging) 0\n\
set el [$w identify $x $y]\n\
if {[string equal $el \"trough1\"]} {\n\
tkScaleIncrement $w up little initial\n\
} elseif {[string equal $el \"trough2\"]} {\n\
tkScaleIncrement $w down little initial\n\
} elseif {[string equal $el \"slider\"]} {\n\
set tkPriv(dragging) 1\n\
set tkPriv(initValue) [$w get]\n\
set coords [$w coords]\n\
set tkPriv(deltaX) [expr {$x - [lindex $coords 0]}]\n\
set tkPriv(deltaY) [expr {$y - [lindex $coords 1]}]\n\
$w configure -sliderrelief sunken\n\
}\n\
}\n\
\n\
\n\
proc tkScaleDrag {w x y} {\n\
global tkPriv\n\
if {!$tkPriv(dragging)} {\n\
return\n\
}\n\
$w set [$w get [expr {$x-$tkPriv(deltaX)}] [expr {$y-$tkPriv(deltaY)}]]\n\
}\n\
\n\
\n\
proc tkScaleEndDrag {w} {\n\
global tkPriv\n\
set tkPriv(dragging) 0\n\
$w configure -sliderrelief raised\n\
}\n\
\n\
\n\
proc tkScaleIncrement {w dir big repeat} {\n\
global tkPriv\n\
if {![winfo exists $w]} return\n\
if {[string equal $big \"big\"]} {\n\
set inc [$w cget -bigincrement]\n\
if {$inc == 0} {\n\
set inc [expr {abs([$w cget -to] - [$w cget -from])/10.0}]\n\
}\n\
if {$inc < [$w cget -resolution]} {\n\
set inc [$w cget -resolution]\n\
}\n\
} else {\n\
set inc [$w cget -resolution]\n\
}\n\
if {([$w cget -from] > [$w cget -to]) ^ [string equal $dir \"up\"]} {\n\
set inc [expr {-$inc}]\n\
}\n\
$w set [expr {[$w get] + $inc}]\n\
\n\
if {[string equal $repeat \"again\"]} {\n\
set tkPriv(afterId) [after [$w cget -repeatinterval] \\\n\
[list tkScaleIncrement $w $dir $big again]]\n\
} elseif {[string equal $repeat \"initial\"]} {\n\
set delay [$w cget -repeatdelay]\n\
if {$delay > 0} {\n\
set tkPriv(afterId) [after $delay \\\n\
[list tkScaleIncrement $w $dir $big again]]\n\
}\n\
}\n\
}\n\
\n\
\n\
proc tkScaleControlPress {w x y} {\n\
set el [$w identify $x $y]\n\
if {[string equal $el \"trough1\"]} {\n\
$w set [$w cget -from]\n\
} elseif {[string equal $el \"trough2\"]} {\n\
$w set [$w cget -to]\n\
}\n\
}\n\
\n\
\n\
proc tkScaleButton2Down {w x y} {\n\
global tkPriv\n\
\n\
if {[string equal [$w cget -state] \"disabled\"]} {\n\
return\n\
}\n\
$w configure -state active\n\
$w set [$w get $x $y]\n\
set tkPriv(dragging) 1\n\
set tkPriv(initValue) [$w get]\n\
set coords \"$x $y\"\n\
set tkPriv(deltaX) 0\n\
set tkPriv(deltaY) 0\n\
}\n\
\n\
\n\
proc tkTearOffMenu {w {x 0} {y 0}} {\n\
\n\
if {$x == 0} {\n\
set x [winfo rootx $w]\n\
}\n\
if {$y == 0} {\n\
set y [winfo rooty $w]\n\
}\n\
\n\
set parent [winfo parent $w]\n\
while {[string compare [winfo toplevel $parent] $parent] \\\n\
|| [string equal [winfo class $parent] \"Menu\"]} {\n\
set parent [winfo parent $parent]\n\
}\n\
if {[string equal $parent \".\"]} {\n\
set parent \"\"\n\
}\n\
for {set i 1} 1 {incr i} {\n\
set menu $parent.tearoff$i\n\
if {![winfo exists $menu]} {\n\
break\n\
}\n\
}\n\
\n\
$w clone $menu tearoff\n\
\n\
\n\
set parent [winfo parent $w]\n\
if {[string compare [$menu cget -title] \"\"]} {\n\
wm title $menu [$menu cget -title]\n\
} else {\n\
switch [winfo class $parent] {\n\
Menubutton {\n\
wm title $menu [$parent cget -text]\n\
}\n\
Menu {\n\
wm title $menu [$parent entrycget active -label]\n\
}\n\
}\n\
}\n\
\n\
$menu post $x $y\n\
\n\
if {[winfo exists $menu] == 0} {\n\
return \"\"\n\
}\n\
\n\
\n\
bind $menu <Enter> {\n\
set tkPriv(focus) %W\n\
}\n\
\n\
\n\
set cmd [$w cget -tearoffcommand]\n\
if {[string compare $cmd \"\"]} {\n\
uplevel #0 $cmd [list $w $menu]\n\
}\n\
return $menu\n\
}\n\
\n\
\n\
proc tkMenuDup {src dst type} {\n\
set cmd [list menu $dst -type $type]\n\
foreach option [$src configure] {\n\
if {[llength $option] == 2} {\n\
continue\n\
}\n\
if {[string equal [lindex $option 0] \"-type\"]} {\n\
continue\n\
}\n\
lappend cmd [lindex $option 0] [lindex $option 4]\n\
}\n\
eval $cmd\n\
set last [$src index last]\n\
if {[string equal $last \"none\"]} {\n\
return\n\
}\n\
for {set i [$src cget -tearoff]} {$i <= $last} {incr i} {\n\
set cmd [list $dst add [$src type $i]]\n\
foreach option [$src entryconfigure $i]  {\n\
lappend cmd [lindex $option 0] [lindex $option 4]\n\
}\n\
eval $cmd\n\
}\n\
\n\
\n\
set tags [bindtags $src]\n\
set srcLen [string length $src]\n\
\n\
\n\
while {[set index [string first $src $tags]] != -1} {\n\
append x [string range $tags 0 [expr {$index - 1}]]$dst\n\
set tags [string range $tags [expr {$index + $srcLen}] end]\n\
}\n\
append x $tags\n\
\n\
bindtags $dst $x\n\
\n\
foreach event [bind $src] {\n\
unset x\n\
set script [bind $src $event]\n\
set eventLen [string length $event]\n\
\n\
\n\
while {[set index [string first $event $script]] != -1} {\n\
append x [string range $script 0 [expr {$index - 1}]]\n\
append x $dst\n\
set script [string range $script [expr {$index + $eventLen}] end]\n\
}\n\
append x $script\n\
\n\
bind $dst $event $x\n\
}\n\
}\n\
\n\
\n\
\n\
\n\
bind Text <1> {\n\
tkTextButton1 %W %x %y\n\
%W tag remove sel 0.0 end\n\
}\n\
bind Text <B1-Motion> {\n\
set tkPriv(x) %x\n\
set tkPriv(y) %y\n\
tkTextSelectTo %W %x %y\n\
}\n\
bind Text <Double-1> {\n\
set tkPriv(selectMode) word\n\
tkTextSelectTo %W %x %y\n\
catch {%W mark set insert sel.last}\n\
catch {%W mark set anchor sel.first}\n\
}\n\
bind Text <Triple-1> {\n\
set tkPriv(selectMode) line\n\
tkTextSelectTo %W %x %y\n\
catch {%W mark set insert sel.last}\n\
catch {%W mark set anchor sel.first}\n\
}\n\
bind Text <Shift-1> {\n\
tkTextResetAnchor %W @%x,%y\n\
set tkPriv(selectMode) char\n\
tkTextSelectTo %W %x %y\n\
}\n\
bind Text <Double-Shift-1>	{\n\
set tkPriv(selectMode) word\n\
tkTextSelectTo %W %x %y 1\n\
}\n\
bind Text <Triple-Shift-1>	{\n\
set tkPriv(selectMode) line\n\
tkTextSelectTo %W %x %y\n\
}\n\
bind Text <B1-Leave> {\n\
set tkPriv(x) %x\n\
set tkPriv(y) %y\n\
tkTextAutoScan %W\n\
}\n\
bind Text <B1-Enter> {\n\
tkCancelRepeat\n\
}\n\
bind Text <ButtonRelease-1> {\n\
tkCancelRepeat\n\
}\n\
bind Text <Control-1> {\n\
%W mark set insert @%x,%y\n\
}\n\
bind Text <Left> {\n\
tkTextSetCursor %W insert-1c\n\
}\n\
bind Text <Right> {\n\
tkTextSetCursor %W insert+1c\n\
}\n\
bind Text <Up> {\n\
tkTextSetCursor %W [tkTextUpDownLine %W -1]\n\
}\n\
bind Text <Down> {\n\
tkTextSetCursor %W [tkTextUpDownLine %W 1]\n\
}\n\
bind Text <Shift-Left> {\n\
tkTextKeySelect %W [%W index {insert - 1c}]\n\
}\n\
bind Text <Shift-Right> {\n\
tkTextKeySelect %W [%W index {insert + 1c}]\n\
}\n\
bind Text <Shift-Up> {\n\
tkTextKeySelect %W [tkTextUpDownLine %W -1]\n\
}\n\
bind Text <Shift-Down> {\n\
tkTextKeySelect %W [tkTextUpDownLine %W 1]\n\
}\n\
bind Text <Control-Left> {\n\
tkTextSetCursor %W [tkTextPrevPos %W insert tcl_startOfPreviousWord]\n\
}\n\
bind Text <Control-Right> {\n\
tkTextSetCursor %W [tkTextNextWord %W insert]\n\
}\n\
bind Text <Control-Up> {\n\
tkTextSetCursor %W [tkTextPrevPara %W insert]\n\
}\n\
bind Text <Control-Down> {\n\
tkTextSetCursor %W [tkTextNextPara %W insert]\n\
}\n\
bind Text <Shift-Control-Left> {\n\
tkTextKeySelect %W [tkTextPrevPos %W insert tcl_startOfPreviousWord]\n\
}\n\
bind Text <Shift-Control-Right> {\n\
tkTextKeySelect %W [tkTextNextWord %W insert]\n\
}\n\
bind Text <Shift-Control-Up> {\n\
tkTextKeySelect %W [tkTextPrevPara %W insert]\n\
}\n\
bind Text <Shift-Control-Down> {\n\
tkTextKeySelect %W [tkTextNextPara %W insert]\n\
}\n\
bind Text <Prior> {\n\
tkTextSetCursor %W [tkTextScrollPages %W -1]\n\
}\n\
bind Text <Shift-Prior> {\n\
tkTextKeySelect %W [tkTextScrollPages %W -1]\n\
}\n\
bind Text <Next> {\n\
tkTextSetCursor %W [tkTextScrollPages %W 1]\n\
}\n\
bind Text <Shift-Next> {\n\
tkTextKeySelect %W [tkTextScrollPages %W 1]\n\
}\n\
bind Text <Control-Prior> {\n\
%W xview scroll -1 page\n\
}\n\
bind Text <Control-Next> {\n\
%W xview scroll 1 page\n\
}\n\
\n\
bind Text <Home> {\n\
tkTextSetCursor %W {insert linestart}\n\
}\n\
bind Text <Shift-Home> {\n\
tkTextKeySelect %W {insert linestart}\n\
}\n\
bind Text <End> {\n\
tkTextSetCursor %W {insert lineend}\n\
}\n\
bind Text <Shift-End> {\n\
tkTextKeySelect %W {insert lineend}\n\
}\n\
bind Text <Control-Home> {\n\
tkTextSetCursor %W 1.0\n\
}\n\
bind Text <Control-Shift-Home> {\n\
tkTextKeySelect %W 1.0\n\
}\n\
bind Text <Control-End> {\n\
tkTextSetCursor %W {end - 1 char}\n\
}\n\
bind Text <Control-Shift-End> {\n\
tkTextKeySelect %W {end - 1 char}\n\
}\n\
\n\
bind Text <Tab> {\n\
tkTextInsert %W \\t\n\
focus %W\n\
break\n\
}\n\
bind Text <Shift-Tab> {\n\
break\n\
}\n\
bind Text <Control-Tab> {\n\
focus [tk_focusNext %W]\n\
}\n\
bind Text <Control-Shift-Tab> {\n\
focus [tk_focusPrev %W]\n\
}\n\
bind Text <Control-i> {\n\
tkTextInsert %W \\t\n\
}\n\
bind Text <Return> {\n\
tkTextInsert %W \\n\n\
}\n\
bind Text <Delete> {\n\
if {[string compare [%W tag nextrange sel 1.0 end] \"\"]} {\n\
%W delete sel.first sel.last\n\
} else {\n\
%W delete insert\n\
%W see insert\n\
}\n\
}\n\
bind Text <BackSpace> {\n\
if {[string compare [%W tag nextrange sel 1.0 end] \"\"]} {\n\
%W delete sel.first sel.last\n\
} elseif {[%W compare insert != 1.0]} {\n\
%W delete insert-1c\n\
%W see insert\n\
}\n\
}\n\
\n\
bind Text <Control-space> {\n\
%W mark set anchor insert\n\
}\n\
bind Text <Select> {\n\
%W mark set anchor insert\n\
}\n\
bind Text <Control-Shift-space> {\n\
set tkPriv(selectMode) char\n\
tkTextKeyExtend %W insert\n\
}\n\
bind Text <Shift-Select> {\n\
set tkPriv(selectMode) char\n\
tkTextKeyExtend %W insert\n\
}\n\
bind Text <Control-slash> {\n\
%W tag add sel 1.0 end\n\
}\n\
bind Text <Control-backslash> {\n\
%W tag remove sel 1.0 end\n\
}\n\
bind Text <<Cut>> {\n\
tk_textCut %W\n\
}\n\
bind Text <<Copy>> {\n\
tk_textCopy %W\n\
}\n\
bind Text <<Paste>> {\n\
tk_textPaste %W\n\
}\n\
bind Text <<Clear>> {\n\
catch {%W delete sel.first sel.last}\n\
}\n\
bind Text <<PasteSelection>> {\n\
if {!$tkPriv(mouseMoved) || $tk_strictMotif} {\n\
tkTextPaste %W %x %y\n\
}\n\
}\n\
bind Text <Insert> {\n\
catch {tkTextInsert %W [selection get -displayof %W]}\n\
}\n\
bind Text <KeyPress> {\n\
tkTextInsert %W %A\n\
}\n\
\n\
\n\
bind Text <Alt-KeyPress> {# nothing }\n\
bind Text <Meta-KeyPress> {# nothing}\n\
bind Text <Control-KeyPress> {# nothing}\n\
bind Text <Escape> {# nothing}\n\
bind Text <KP_Enter> {# nothing}\n\
if {[string equal $tcl_platform(platform) \"macintosh\"]} {\n\
bind Text <Command-KeyPress> {# nothing}\n\
}\n\
\n\
\n\
bind Text <Control-a> {\n\
if {!$tk_strictMotif} {\n\
tkTextSetCursor %W {insert linestart}\n\
}\n\
}\n\
bind Text <Control-b> {\n\
if {!$tk_strictMotif} {\n\
tkTextSetCursor %W insert-1c\n\
}\n\
}\n\
bind Text <Control-d> {\n\
if {!$tk_strictMotif} {\n\
%W delete insert\n\
}\n\
}\n\
bind Text <Control-e> {\n\
if {!$tk_strictMotif} {\n\
tkTextSetCursor %W {insert lineend}\n\
}\n\
}\n\
bind Text <Control-f> {\n\
if {!$tk_strictMotif} {\n\
tkTextSetCursor %W insert+1c\n\
}\n\
}\n\
bind Text <Control-k> {\n\
if {!$tk_strictMotif} {\n\
if {[%W compare insert == {insert lineend}]} {\n\
%W delete insert\n\
} else {\n\
%W delete insert {insert lineend}\n\
}\n\
}\n\
}\n\
bind Text <Control-n> {\n\
if {!$tk_strictMotif} {\n\
tkTextSetCursor %W [tkTextUpDownLine %W 1]\n\
}\n\
}\n\
bind Text <Control-o> {\n\
if {!$tk_strictMotif} {\n\
%W insert insert \\n\n\
%W mark set insert insert-1c\n\
}\n\
}\n\
bind Text <Control-p> {\n\
if {!$tk_strictMotif} {\n\
tkTextSetCursor %W [tkTextUpDownLine %W -1]\n\
}\n\
}\n\
bind Text <Control-t> {\n\
if {!$tk_strictMotif} {\n\
tkTextTranspose %W\n\
}\n\
}\n\
\n\
if {[string compare $tcl_platform(platform) \"windows\"]} {\n\
bind Text <Control-v> {\n\
if {!$tk_strictMotif} {\n\
tkTextScrollPages %W 1\n\
}\n\
}\n\
}\n\
\n\
bind Text <Meta-b> {\n\
if {!$tk_strictMotif} {\n\
tkTextSetCursor %W [tkTextPrevPos %W insert tcl_startOfPreviousWord]\n\
}\n\
}\n\
bind Text <Meta-d> {\n\
if {!$tk_strictMotif} {\n\
%W delete insert [tkTextNextWord %W insert]\n\
}\n\
}\n\
bind Text <Meta-f> {\n\
if {!$tk_strictMotif} {\n\
tkTextSetCursor %W [tkTextNextWord %W insert]\n\
}\n\
}\n\
bind Text <Meta-less> {\n\
if {!$tk_strictMotif} {\n\
tkTextSetCursor %W 1.0\n\
}\n\
}\n\
bind Text <Meta-greater> {\n\
if {!$tk_strictMotif} {\n\
tkTextSetCursor %W end-1c\n\
}\n\
}\n\
bind Text <Meta-BackSpace> {\n\
if {!$tk_strictMotif} {\n\
%W delete [tkTextPrevPos %W insert tcl_startOfPreviousWord] insert\n\
}\n\
}\n\
bind Text <Meta-Delete> {\n\
if {!$tk_strictMotif} {\n\
%W delete [tkTextPrevPos %W insert tcl_startOfPreviousWord] insert\n\
}\n\
}\n\
\n\
\n\
if {[string equal $tcl_platform(platform) \"macintosh\"]} {\n\
bind Text <FocusIn> {\n\
%W tag configure sel -borderwidth 0\n\
%W configure -selectbackground systemHighlight -selectforeground systemHighlightText\n\
}\n\
bind Text <FocusOut> {\n\
%W tag configure sel -borderwidth 1\n\
%W configure -selectbackground white -selectforeground black\n\
}\n\
bind Text <Option-Left> {\n\
tkTextSetCursor %W [tkTextPrevPos %W insert tcl_startOfPreviousWord]\n\
}\n\
bind Text <Option-Right> {\n\
tkTextSetCursor %W [tkTextNextWord %W insert]\n\
}\n\
bind Text <Option-Up> {\n\
tkTextSetCursor %W [tkTextPrevPara %W insert]\n\
}\n\
bind Text <Option-Down> {\n\
tkTextSetCursor %W [tkTextNextPara %W insert]\n\
}\n\
bind Text <Shift-Option-Left> {\n\
tkTextKeySelect %W [tkTextPrevPos %W insert tcl_startOfPreviousWord]\n\
}\n\
bind Text <Shift-Option-Right> {\n\
tkTextKeySelect %W [tkTextNextWord %W insert]\n\
}\n\
bind Text <Shift-Option-Up> {\n\
tkTextKeySelect %W [tkTextPrevPara %W insert]\n\
}\n\
bind Text <Shift-Option-Down> {\n\
tkTextKeySelect %W [tkTextNextPara %W insert]\n\
}\n\
\n\
}\n\
\n\
\n\
bind Text <Control-h> {\n\
if {!$tk_strictMotif} {\n\
if {[%W compare insert != 1.0]} {\n\
%W delete insert-1c\n\
%W see insert\n\
}\n\
}\n\
}\n\
bind Text <2> {\n\
if {!$tk_strictMotif} {\n\
%W scan mark %x %y\n\
set tkPriv(x) %x\n\
set tkPriv(y) %y\n\
set tkPriv(mouseMoved) 0\n\
}\n\
}\n\
bind Text <B2-Motion> {\n\
if {!$tk_strictMotif} {\n\
if {(%x != $tkPriv(x)) || (%y != $tkPriv(y))} {\n\
set tkPriv(mouseMoved) 1\n\
}\n\
if {$tkPriv(mouseMoved)} {\n\
%W scan dragto %x %y\n\
}\n\
}\n\
}\n\
set tkPriv(prevPos) {}\n\
\n\
\n\
bind Text <MouseWheel> {\n\
%W yview scroll [expr {- (%D / 120) * 4}] units\n\
}\n\
\n\
if {[string equal \"unix\" $tcl_platform(platform)]} {\n\
bind Text <4> {\n\
if {!$tk_strictMotif} {\n\
%W yview scroll -5 units\n\
}\n\
}\n\
bind Text <5> {\n\
if {!$tk_strictMotif} {\n\
%W yview scroll 5 units\n\
}\n\
}\n\
}\n\
\n\
\n\
proc tkTextClosestGap {w x y} {\n\
set pos [$w index @$x,$y]\n\
set bbox [$w bbox $pos]\n\
if {[string equal $bbox \"\"]} {\n\
return $pos\n\
}\n\
if {($x - [lindex $bbox 0]) < ([lindex $bbox 2]/2)} {\n\
return $pos\n\
}\n\
$w index \"$pos + 1 char\"\n\
}\n\
\n\
\n\
proc tkTextButton1 {w x y} {\n\
global tkPriv\n\
\n\
set tkPriv(selectMode) char\n\
set tkPriv(mouseMoved) 0\n\
set tkPriv(pressX) $x\n\
$w mark set insert [tkTextClosestGap $w $x $y]\n\
$w mark set anchor insert\n\
if {[string equal [$w cget -state] \"normal\"]} {focus $w}\n\
}\n\
\n\
\n\
proc tkTextSelectTo {w x y {extend 0}} {\n\
global tkPriv tcl_platform\n\
\n\
set cur [tkTextClosestGap $w $x $y]\n\
if {[catch {$w index anchor}]} {\n\
$w mark set anchor $cur\n\
}\n\
set anchor [$w index anchor]\n\
if {[$w compare $cur != $anchor] || (abs($tkPriv(pressX) - $x) >= 3)} {\n\
set tkPriv(mouseMoved) 1\n\
}\n\
switch $tkPriv(selectMode) {\n\
char {\n\
if {[$w compare $cur < anchor]} {\n\
set first $cur\n\
set last anchor\n\
} else {\n\
set first anchor\n\
set last $cur\n\
}\n\
}\n\
word {\n\
if {[$w compare $cur < anchor]} {\n\
set first [tkTextPrevPos $w \"$cur + 1c\" tcl_wordBreakBefore]\n\
if { !$extend } {\n\
set last [tkTextNextPos $w \"anchor\" tcl_wordBreakAfter]\n\
} else {\n\
set last anchor\n\
}\n\
} else {\n\
set last [tkTextNextPos $w \"$cur - 1c\" tcl_wordBreakAfter]\n\
if { !$extend } {\n\
set first [tkTextPrevPos $w anchor tcl_wordBreakBefore]\n\
} else {\n\
set first anchor\n\
}\n\
}\n\
}\n\
line {\n\
if {[$w compare $cur < anchor]} {\n\
set first [$w index \"$cur linestart\"]\n\
set last [$w index \"anchor - 1c lineend + 1c\"]\n\
} else {\n\
set first [$w index \"anchor linestart\"]\n\
set last [$w index \"$cur lineend + 1c\"]\n\
}\n\
}\n\
}\n\
if {$tkPriv(mouseMoved) || [string compare $tkPriv(selectMode) \"char\"]} {\n\
if {[string compare $tcl_platform(platform) \"unix\"] \\\n\
&& [$w compare $cur < anchor]} {\n\
$w mark set insert $first\n\
} else {\n\
$w mark set insert $last\n\
}\n\
$w tag remove sel 0.0 $first\n\
$w tag add sel $first $last\n\
$w tag remove sel $last end\n\
update idletasks\n\
}\n\
}\n\
\n\
\n\
proc tkTextKeyExtend {w index} {\n\
global tkPriv\n\
\n\
set cur [$w index $index]\n\
if {[catch {$w index anchor}]} {\n\
$w mark set anchor $cur\n\
}\n\
set anchor [$w index anchor]\n\
if {[$w compare $cur < anchor]} {\n\
set first $cur\n\
set last anchor\n\
} else {\n\
set first anchor\n\
set last $cur\n\
}\n\
$w tag remove sel 0.0 $first\n\
$w tag add sel $first $last\n\
$w tag remove sel $last end\n\
}\n\
\n\
\n\
proc tkTextPaste {w x y} {\n\
$w mark set insert [tkTextClosestGap $w $x $y]\n\
catch {$w insert insert [selection get -displayof $w]}\n\
if {[string equal [$w cget -state] \"normal\"]} {focus $w}\n\
}\n\
\n\
\n\
proc tkTextAutoScan {w} {\n\
global tkPriv\n\
if {![winfo exists $w]} return\n\
if {$tkPriv(y) >= [winfo height $w]} {\n\
$w yview scroll 2 units\n\
} elseif {$tkPriv(y) < 0} {\n\
$w yview scroll -2 units\n\
} elseif {$tkPriv(x) >= [winfo width $w]} {\n\
$w xview scroll 2 units\n\
} elseif {$tkPriv(x) < 0} {\n\
$w xview scroll -2 units\n\
} else {\n\
return\n\
}\n\
tkTextSelectTo $w $tkPriv(x) $tkPriv(y)\n\
set tkPriv(afterId) [after 50 [list tkTextAutoScan $w]]\n\
}\n\
\n\
\n\
proc tkTextSetCursor {w pos} {\n\
global tkPriv\n\
\n\
if {[$w compare $pos == end]} {\n\
set pos {end - 1 chars}\n\
}\n\
$w mark set insert $pos\n\
$w tag remove sel 1.0 end\n\
$w see insert\n\
}\n\
\n\
\n\
proc tkTextKeySelect {w new} {\n\
global tkPriv\n\
\n\
if {[string equal [$w tag nextrange sel 1.0 end] \"\"]} {\n\
if {[$w compare $new < insert]} {\n\
$w tag add sel $new insert\n\
} else {\n\
$w tag add sel insert $new\n\
}\n\
$w mark set anchor insert\n\
} else {\n\
if {[$w compare $new < anchor]} {\n\
set first $new\n\
set last anchor\n\
} else {\n\
set first anchor\n\
set last $new\n\
}\n\
$w tag remove sel 1.0 $first\n\
$w tag add sel $first $last\n\
$w tag remove sel $last end\n\
}\n\
$w mark set insert $new\n\
$w see insert\n\
update idletasks\n\
}\n\
\n\
\n\
proc tkTextResetAnchor {w index} {\n\
global tkPriv\n\
\n\
if {[string equal [$w tag ranges sel] \"\"]} {\n\
$w mark set anchor $index\n\
return\n\
}\n\
set a [$w index $index]\n\
set b [$w index sel.first]\n\
set c [$w index sel.last]\n\
if {[$w compare $a < $b]} {\n\
$w mark set anchor sel.last\n\
return\n\
}\n\
if {[$w compare $a > $c]} {\n\
$w mark set anchor sel.first\n\
return\n\
}\n\
scan $a \"%d.%d\" lineA chA\n\
scan $b \"%d.%d\" lineB chB\n\
scan $c \"%d.%d\" lineC chC\n\
if {$lineB < $lineC+2} {\n\
set total [string length [$w get $b $c]]\n\
if {$total <= 2} {\n\
return\n\
}\n\
if {[string length [$w get $b $a]] < ($total/2)} {\n\
$w mark set anchor sel.last\n\
} else {\n\
$w mark set anchor sel.first\n\
}\n\
return\n\
}\n\
if {($lineA-$lineB) < ($lineC-$lineA)} {\n\
$w mark set anchor sel.last\n\
} else {\n\
$w mark set anchor sel.first\n\
}\n\
}\n\
\n\
\n\
proc tkTextInsert {w s} {\n\
if {[string equal $s \"\"] || [string equal [$w cget -state] \"disabled\"]} {\n\
return\n\
}\n\
catch {\n\
if {[$w compare sel.first <= insert] \\\n\
&& [$w compare sel.last >= insert]} {\n\
$w delete sel.first sel.last\n\
}\n\
}\n\
$w insert insert $s\n\
$w see insert\n\
}\n\
\n\
\n\
proc tkTextUpDownLine {w n} {\n\
global tkPriv\n\
\n\
set i [$w index insert]\n\
scan $i \"%d.%d\" line char\n\
if {[string compare $tkPriv(prevPos) $i]} {\n\
set tkPriv(char) $char\n\
}\n\
set new [$w index [expr {$line + $n}].$tkPriv(char)]\n\
if {[$w compare $new == end] || [$w compare $new == \"insert linestart\"]} {\n\
set new $i\n\
}\n\
set tkPriv(prevPos) $new\n\
return $new\n\
}\n\
\n\
\n\
proc tkTextPrevPara {w pos} {\n\
set pos [$w index \"$pos linestart\"]\n\
while {1} {\n\
if {([string equal [$w get \"$pos - 1 line\"] \"\\n\"] \\\n\
&& [string compare [$w get $pos] \"\\n\"]) \\\n\
|| [string equal $pos \"1.0\"]} {\n\
if {[regexp -indices {^[ 	]+(.)} [$w get $pos \"$pos lineend\"] \\\n\
dummy index]} {\n\
set pos [$w index \"$pos + [lindex $index 0] chars\"]\n\
}\n\
if {[$w compare $pos != insert] || [string equal $pos 1.0]} {\n\
return $pos\n\
}\n\
}\n\
set pos [$w index \"$pos - 1 line\"]\n\
}\n\
}\n\
\n\
\n\
proc tkTextNextPara {w start} {\n\
set pos [$w index \"$start linestart + 1 line\"]\n\
while {[string compare [$w get $pos] \"\\n\"]} {\n\
if {[$w compare $pos == end]} {\n\
return [$w index \"end - 1c\"]\n\
}\n\
set pos [$w index \"$pos + 1 line\"]\n\
}\n\
while {[string equal [$w get $pos] \"\\n\"]} {\n\
set pos [$w index \"$pos + 1 line\"]\n\
if {[$w compare $pos == end]} {\n\
return [$w index \"end - 1c\"]\n\
}\n\
}\n\
if {[regexp -indices {^[ 	]+(.)} [$w get $pos \"$pos lineend\"] \\\n\
dummy index]} {\n\
return [$w index \"$pos + [lindex $index 0] chars\"]\n\
}\n\
return $pos\n\
}\n\
\n\
\n\
proc tkTextScrollPages {w count} {\n\
set bbox [$w bbox insert]\n\
$w yview scroll $count pages\n\
if {[string equal $bbox \"\"]} {\n\
return [$w index @[expr {[winfo height $w]/2}],0]\n\
}\n\
return [$w index @[lindex $bbox 0],[lindex $bbox 1]]\n\
}\n\
\n\
\n\
proc tkTextTranspose w {\n\
set pos insert\n\
if {[$w compare $pos != \"$pos lineend\"]} {\n\
set pos [$w index \"$pos + 1 char\"]\n\
}\n\
set new [$w get \"$pos - 1 char\"][$w get  \"$pos - 2 char\"]\n\
if {[$w compare \"$pos - 1 char\" == 1.0]} {\n\
return\n\
}\n\
$w delete \"$pos - 2 char\" $pos\n\
$w insert insert $new\n\
$w see insert\n\
}\n\
\n\
\n\
proc tk_textCopy w {\n\
if {![catch {set data [$w get sel.first sel.last]}]} {\n\
clipboard clear -displayof $w\n\
clipboard append -displayof $w $data\n\
}\n\
}\n\
\n\
\n\
proc tk_textCut w {\n\
if {![catch {set data [$w get sel.first sel.last]}]} {\n\
clipboard clear -displayof $w\n\
clipboard append -displayof $w $data\n\
$w delete sel.first sel.last\n\
}\n\
}\n\
\n\
\n\
proc tk_textPaste w {\n\
global tcl_platform\n\
catch {\n\
if {[string compare $tcl_platform(platform) \"unix\"]} {\n\
catch {\n\
$w delete sel.first sel.last\n\
}\n\
}\n\
$w insert insert [selection get -displayof $w -selection CLIPBOARD]\n\
}\n\
}\n\
\n\
\n\
if {[string equal $tcl_platform(platform) \"windows\"]}  {\n\
proc tkTextNextWord {w start} {\n\
tkTextNextPos $w [tkTextNextPos $w $start tcl_endOfWord] \\\n\
tcl_startOfNextWord\n\
}\n\
} else {\n\
proc tkTextNextWord {w start} {\n\
tkTextNextPos $w $start tcl_endOfWord\n\
}\n\
}\n\
\n\
\n\
proc tkTextNextPos {w start op} {\n\
set text \"\"\n\
set cur $start\n\
while {[$w compare $cur < end]} {\n\
set text $text[$w get $cur \"$cur lineend + 1c\"]\n\
set pos [$op $text 0]\n\
if {$pos >= 0} {\n\
set dump [$w dump -image -window $start \"$start + $pos c\"]\n\
if {[llength $dump]} {\n\
set pos [expr {$pos + ([llength $dump]/3)}]\n\
}\n\
return [$w index \"$start + $pos c\"]\n\
}\n\
set cur [$w index \"$cur lineend +1c\"]\n\
}\n\
return end\n\
}\n\
\n\
\n\
proc tkTextPrevPos {w start op} {\n\
set text \"\"\n\
set cur $start\n\
while {[$w compare $cur > 0.0]} {\n\
set text [$w get \"$cur linestart - 1c\" $cur]$text\n\
set pos [$op $text end]\n\
if {$pos >= 0} {\n\
set dump [$w dump -image -window \"$cur linestart\" \"$start - 1c\"]\n\
if {[llength $dump]} {\n\
if {[$w compare [lindex $dump 2] > \\\n\
\"$cur linestart - 1c + $pos c\"]} {\n\
incr pos -1\n\
}\n\
set pos [expr {$pos + ([llength $dump]/3)}]\n\
}\n\
return [$w index \"$cur linestart - 1c + $pos c\"]\n\
}\n\
set cur [$w index \"$cur linestart - 1c\"]\n\
}\n\
return 0.0\n\
}\n\
\n\
\n\
proc tkIconList {w args} {\n\
upvar #0 $w data\n\
\n\
tkIconList_Config $w $args\n\
tkIconList_Create $w\n\
}\n\
\n\
proc tkIconList_Config {w argList} {\n\
upvar #0 $w data\n\
\n\
set specs {\n\
{-browsecmd \"\" \"\" \"\"}\n\
{-command \"\" \"\" \"\"}\n\
}\n\
\n\
tclParseConfigSpec $w $specs \"\" $argList\n\
}\n\
\n\
proc tkIconList_Create {w} {\n\
upvar #0 $w data\n\
\n\
frame $w\n\
set data(sbar)   [scrollbar $w.sbar -orient horizontal \\\n\
-highlightthickness 0 -takefocus 0]\n\
set data(canvas) [canvas $w.canvas -bd 2 -relief sunken \\\n\
-width 400 -height 120 -takefocus 1]\n\
pack $data(sbar) -side bottom -fill x -padx 2\n\
pack $data(canvas) -expand yes -fill both\n\
\n\
$data(sbar) config -command [list $data(canvas) xview]\n\
$data(canvas) config -xscrollcommand [list $data(sbar) set]\n\
\n\
set data(maxIW) 1\n\
set data(maxIH) 1\n\
set data(maxTW) 1\n\
set data(maxTH) 1\n\
set data(numItems) 0\n\
set data(curItem)  {}\n\
set data(noScroll) 1\n\
\n\
bind $data(canvas) <Configure>	[list tkIconList_Arrange $w]\n\
\n\
bind $data(canvas) <1>		[list tkIconList_Btn1 $w %x %y]\n\
bind $data(canvas) <B1-Motion>	[list tkIconList_Motion1 $w %x %y]\n\
bind $data(canvas) <B1-Leave>	[list tkIconList_Leave1 $w %x %y]\n\
bind $data(canvas) <B1-Enter>	[list tkCancelRepeat]\n\
bind $data(canvas) <ButtonRelease-1> [list tkCancelRepeat]\n\
bind $data(canvas) <Double-ButtonRelease-1> \\\n\
[list tkIconList_Double1 $w %x %y]\n\
\n\
bind $data(canvas) <Up>		[list tkIconList_UpDown $w -1]\n\
bind $data(canvas) <Down>		[list tkIconList_UpDown $w  1]\n\
bind $data(canvas) <Left>		[list tkIconList_LeftRight $w -1]\n\
bind $data(canvas) <Right>		[list tkIconList_LeftRight $w  1]\n\
bind $data(canvas) <Return>		[list tkIconList_ReturnKey $w]\n\
bind $data(canvas) <KeyPress>	[list tkIconList_KeyPress $w %A]\n\
bind $data(canvas) <Control-KeyPress> \";\"\n\
bind $data(canvas) <Alt-KeyPress>	\";\"\n\
\n\
bind $data(canvas) <FocusIn>	[list tkIconList_FocusIn $w]\n\
\n\
return $w\n\
}\n\
\n\
proc tkIconList_AutoScan {w} {\n\
upvar #0 $w data\n\
global tkPriv\n\
\n\
if {![winfo exists $w]} return\n\
set x $tkPriv(x)\n\
set y $tkPriv(y)\n\
\n\
if {$data(noScroll)} {\n\
return\n\
}\n\
if {$x >= [winfo width $data(canvas)]} {\n\
$data(canvas) xview scroll 1 units\n\
} elseif {$x < 0} {\n\
$data(canvas) xview scroll -1 units\n\
} elseif {$y >= [winfo height $data(canvas)]} {\n\
} elseif {$y < 0} {\n\
} else {\n\
return\n\
}\n\
\n\
tkIconList_Motion1 $w $x $y\n\
set tkPriv(afterId) [after 50 [list tkIconList_AutoScan $w]]\n\
}\n\
\n\
proc tkIconList_DeleteAll {w} {\n\
upvar #0 $w data\n\
upvar #0 $w:itemList itemList\n\
\n\
$data(canvas) delete all\n\
catch {unset data(selected)}\n\
catch {unset data(rect)}\n\
catch {unset data(list)}\n\
catch {unset itemList}\n\
set data(maxIW) 1\n\
set data(maxIH) 1\n\
set data(maxTW) 1\n\
set data(maxTH) 1\n\
set data(numItems) 0\n\
set data(curItem)  {}\n\
set data(noScroll) 1\n\
$data(sbar) set 0.0 1.0\n\
$data(canvas) xview moveto 0\n\
}\n\
\n\
proc tkIconList_Add {w image text} {\n\
upvar #0 $w data\n\
upvar #0 $w:itemList itemList\n\
upvar #0 $w:textList textList\n\
\n\
set iTag [$data(canvas) create image 0 0 -image $image -anchor nw]\n\
set tTag [$data(canvas) create text  0 0 -text  $text  -anchor nw \\\n\
-font $data(font)]\n\
set rTag [$data(canvas) create rect  0 0 0 0 -fill \"\" -outline \"\"]\n\
\n\
set b [$data(canvas) bbox $iTag]\n\
set iW [expr {[lindex $b 2]-[lindex $b 0]}]\n\
set iH [expr {[lindex $b 3]-[lindex $b 1]}]\n\
if {$data(maxIW) < $iW} {\n\
set data(maxIW) $iW\n\
}\n\
if {$data(maxIH) < $iH} {\n\
set data(maxIH) $iH\n\
}\n\
\n\
set b [$data(canvas) bbox $tTag]\n\
set tW [expr {[lindex $b 2]-[lindex $b 0]}]\n\
set tH [expr {[lindex $b 3]-[lindex $b 1]}]\n\
if {$data(maxTW) < $tW} {\n\
set data(maxTW) $tW\n\
}\n\
if {$data(maxTH) < $tH} {\n\
set data(maxTH) $tH\n\
}\n\
\n\
lappend data(list) [list $iTag $tTag $rTag $iW $iH $tW $tH $data(numItems)]\n\
set itemList($rTag) [list $iTag $tTag $text $data(numItems)]\n\
set textList($data(numItems)) [string tolower $text]\n\
incr data(numItems)\n\
}\n\
\n\
proc tkIconList_Arrange {w} {\n\
upvar #0 $w data\n\
\n\
if {![info exists data(list)]} {\n\
if {[info exists data(canvas)] && [winfo exists $data(canvas)]} {\n\
set data(noScroll) 1\n\
$data(sbar) config -command \"\"\n\
}\n\
return\n\
}\n\
\n\
set W [winfo width  $data(canvas)]\n\
set H [winfo height $data(canvas)]\n\
set pad [expr {[$data(canvas) cget -highlightthickness] + \\\n\
[$data(canvas) cget -bd]}]\n\
if {$pad < 2} {\n\
set pad 2\n\
}\n\
\n\
incr W -[expr {$pad*2}]\n\
incr H -[expr {$pad*2}]\n\
\n\
set dx [expr {$data(maxIW) + $data(maxTW) + 8}]\n\
if {$data(maxTH) > $data(maxIH)} {\n\
set dy $data(maxTH)\n\
} else {\n\
set dy $data(maxIH)\n\
}\n\
incr dy 2\n\
set shift [expr {$data(maxIW) + 4}]\n\
\n\
set x [expr {$pad * 2}]\n\
set y [expr {$pad * 1}] ; # Why * 1 ?\n\
set usedColumn 0\n\
foreach sublist $data(list) {\n\
set usedColumn 1\n\
set iTag [lindex $sublist 0]\n\
set tTag [lindex $sublist 1]\n\
set rTag [lindex $sublist 2]\n\
set iW   [lindex $sublist 3]\n\
set iH   [lindex $sublist 4]\n\
set tW   [lindex $sublist 5]\n\
set tH   [lindex $sublist 6]\n\
\n\
set i_dy [expr {($dy - $iH)/2}]\n\
set t_dy [expr {($dy - $tH)/2}]\n\
\n\
$data(canvas) coords $iTag $x                    [expr {$y + $i_dy}]\n\
$data(canvas) coords $tTag [expr {$x + $shift}]  [expr {$y + $t_dy}]\n\
$data(canvas) coords $tTag [expr {$x + $shift}]  [expr {$y + $t_dy}]\n\
$data(canvas) coords $rTag $x $y [expr {$x+$dx}] [expr {$y+$dy}]\n\
\n\
incr y $dy\n\
if {($y + $dy) > $H} {\n\
set y [expr {$pad * 1}] ; # *1 ?\n\
incr x $dx\n\
set usedColumn 0\n\
}\n\
}\n\
\n\
if {$usedColumn} {\n\
set sW [expr {$x + $dx}]\n\
} else {\n\
set sW $x\n\
}\n\
\n\
if {$sW < $W} {\n\
$data(canvas) config -scrollregion [list $pad $pad $sW $H]\n\
$data(sbar) config -command \"\"\n\
$data(canvas) xview moveto 0\n\
set data(noScroll) 1\n\
} else {\n\
$data(canvas) config -scrollregion [list $pad $pad $sW $H]\n\
$data(sbar) config -command [list $data(canvas) xview]\n\
set data(noScroll) 0\n\
}\n\
\n\
set data(itemsPerColumn) [expr {($H-$pad)/$dy}]\n\
if {$data(itemsPerColumn) < 1} {\n\
set data(itemsPerColumn) 1\n\
}\n\
\n\
if {$data(curItem) != \"\"} {\n\
tkIconList_Select $w [lindex [lindex $data(list) $data(curItem)] 2] 0\n\
}\n\
}\n\
\n\
proc tkIconList_Invoke {w} {\n\
upvar #0 $w data\n\
\n\
if {$data(-command) != \"\" && [info exists data(selected)]} {\n\
uplevel #0 $data(-command)\n\
}\n\
}\n\
\n\
proc tkIconList_See {w rTag} {\n\
upvar #0 $w data\n\
upvar #0 $w:itemList itemList\n\
\n\
if {$data(noScroll)} {\n\
return\n\
}\n\
set sRegion [$data(canvas) cget -scrollregion]\n\
if {[string equal $sRegion {}]} {\n\
return\n\
}\n\
\n\
if {![info exists itemList($rTag)]} {\n\
return\n\
}\n\
\n\
\n\
set bbox [$data(canvas) bbox $rTag]\n\
set pad [expr {[$data(canvas) cget -highlightthickness] + \\\n\
[$data(canvas) cget -bd]}]\n\
\n\
set x1 [lindex $bbox 0]\n\
set x2 [lindex $bbox 2]\n\
incr x1 -[expr {$pad * 2}]\n\
incr x2 -[expr {$pad * 1}] ; # *1 ?\n\
\n\
set cW [expr {[winfo width $data(canvas)] - $pad*2}]\n\
\n\
set scrollW [expr {[lindex $sRegion 2]-[lindex $sRegion 0]+1}]\n\
set dispX [expr {int([lindex [$data(canvas) xview] 0]*$scrollW)}]\n\
set oldDispX $dispX\n\
\n\
if {($x2 - $dispX) >= $cW} {\n\
set dispX [expr {$x2 - $cW}]\n\
}\n\
if {($x1 - $dispX) < 0} {\n\
set dispX $x1\n\
}\n\
\n\
if {$oldDispX != $dispX} {\n\
set fraction [expr {double($dispX)/double($scrollW)}]\n\
$data(canvas) xview moveto $fraction\n\
}\n\
}\n\
\n\
proc tkIconList_SelectAtXY {w x y} {\n\
upvar #0 $w data\n\
\n\
tkIconList_Select $w [$data(canvas) find closest \\\n\
[$data(canvas) canvasx $x] [$data(canvas) canvasy $y]]\n\
}\n\
\n\
proc tkIconList_Select {w rTag {callBrowse 1}} {\n\
upvar #0 $w data\n\
upvar #0 $w:itemList itemList\n\
\n\
if {![info exists itemList($rTag)]} {\n\
return\n\
}\n\
set iTag   [lindex $itemList($rTag) 0]\n\
set tTag   [lindex $itemList($rTag) 1]\n\
set text   [lindex $itemList($rTag) 2]\n\
set serial [lindex $itemList($rTag) 3]\n\
\n\
if {![info exists data(rect)]} {\n\
set data(rect) [$data(canvas) create rect 0 0 0 0 \\\n\
-fill #a0a0ff -outline #a0a0ff]\n\
}\n\
$data(canvas) lower $data(rect)\n\
set bbox [$data(canvas) bbox $tTag]\n\
eval [list $data(canvas) coords $data(rect)] $bbox\n\
\n\
set data(curItem) $serial\n\
set data(selected) $text\n\
\n\
if {$callBrowse && $data(-browsecmd) != \"\"} {\n\
eval $data(-browsecmd) [list $text]\n\
}\n\
}\n\
\n\
proc tkIconList_Unselect {w} {\n\
upvar #0 $w data\n\
\n\
if {[info exists data(rect)]} {\n\
$data(canvas) delete $data(rect)\n\
unset data(rect)\n\
}\n\
if {[info exists data(selected)]} {\n\
unset data(selected)\n\
}\n\
}\n\
\n\
proc tkIconList_Get {w} {\n\
upvar #0 $w data\n\
\n\
if {[info exists data(selected)]} {\n\
return $data(selected)\n\
} else {\n\
return \"\"\n\
}\n\
}\n\
\n\
\n\
proc tkIconList_Btn1 {w x y} {\n\
upvar #0 $w data\n\
\n\
focus $data(canvas)\n\
tkIconList_SelectAtXY $w $x $y\n\
}\n\
\n\
proc tkIconList_Motion1 {w x y} {\n\
global tkPriv\n\
set tkPriv(x) $x\n\
set tkPriv(y) $y\n\
\n\
tkIconList_SelectAtXY $w $x $y\n\
}\n\
\n\
proc tkIconList_Double1 {w x y} {\n\
upvar #0 $w data\n\
\n\
if {[string compare $data(curItem) {}]} {\n\
tkIconList_Invoke $w\n\
}\n\
}\n\
\n\
proc tkIconList_ReturnKey {w} {\n\
tkIconList_Invoke $w\n\
}\n\
\n\
proc tkIconList_Leave1 {w x y} {\n\
global tkPriv\n\
\n\
set tkPriv(x) $x\n\
set tkPriv(y) $y\n\
tkIconList_AutoScan $w\n\
}\n\
\n\
proc tkIconList_FocusIn {w} {\n\
upvar #0 $w data\n\
\n\
if {![info exists data(list)]} {\n\
return\n\
}\n\
\n\
if {[string compare $data(curItem) {}]} {\n\
tkIconList_Select $w [lindex [lindex $data(list) $data(curItem)] 2] 1\n\
}\n\
}\n\
\n\
proc tkIconList_UpDown {w amount} {\n\
upvar #0 $w data\n\
\n\
if {![info exists data(list)]} {\n\
return\n\
}\n\
\n\
if {[string equal $data(curItem) {}]} {\n\
set rTag [lindex [lindex $data(list) 0] 2]\n\
} else {\n\
set oldRTag [lindex [lindex $data(list) $data(curItem)] 2]\n\
set rTag [lindex [lindex $data(list) [expr {$data(curItem)+$amount}]] 2]\n\
if {[string equal $rTag \"\"]} {\n\
set rTag $oldRTag\n\
}\n\
}\n\
\n\
if {[string compare $rTag \"\"]} {\n\
tkIconList_Select $w $rTag\n\
tkIconList_See $w $rTag\n\
}\n\
}\n\
\n\
proc tkIconList_LeftRight {w amount} {\n\
upvar #0 $w data\n\
\n\
if {![info exists data(list)]} {\n\
return\n\
}\n\
if {[string equal $data(curItem) {}]} {\n\
set rTag [lindex [lindex $data(list) 0] 2]\n\
} else {\n\
set oldRTag [lindex [lindex $data(list) $data(curItem)] 2]\n\
set newItem [expr {$data(curItem)+($amount*$data(itemsPerColumn))}]\n\
set rTag [lindex [lindex $data(list) $newItem] 2]\n\
if {[string equal $rTag \"\"]} {\n\
set rTag $oldRTag\n\
}\n\
}\n\
\n\
if {[string compare $rTag \"\"]} {\n\
tkIconList_Select $w $rTag\n\
tkIconList_See $w $rTag\n\
}\n\
}\n\
\n\
\n\
proc tkIconList_KeyPress {w key} {\n\
global tkPriv\n\
\n\
append tkPriv(ILAccel,$w) $key\n\
tkIconList_Goto $w $tkPriv(ILAccel,$w)\n\
catch {\n\
after cancel $tkPriv(ILAccel,$w,afterId)\n\
}\n\
set tkPriv(ILAccel,$w,afterId) [after 500 [list tkIconList_Reset $w]]\n\
}\n\
\n\
proc tkIconList_Goto {w text} {\n\
upvar #0 $w data\n\
upvar #0 $w:textList textList\n\
global tkPriv\n\
\n\
if {![info exists data(list)]} {\n\
return\n\
}\n\
\n\
if {[string equal {} $text]} {\n\
return\n\
}\n\
\n\
if {$data(curItem) == \"\" || $data(curItem) == 0} {\n\
set start  0\n\
} else {\n\
set start  $data(curItem)\n\
}\n\
\n\
set text [string tolower $text]\n\
set theIndex -1\n\
set less 0\n\
set len [string length $text]\n\
set len0 [expr {$len-1}]\n\
set i $start\n\
\n\
while {1} {\n\
set sub [string range $textList($i) 0 $len0]\n\
if {[string equal $text $sub]} {\n\
set theIndex $i\n\
break\n\
}\n\
incr i\n\
if {$i == $data(numItems)} {\n\
set i 0\n\
}\n\
if {$i == $start} {\n\
break\n\
}\n\
}\n\
\n\
if {$theIndex > -1} {\n\
set rTag [lindex [lindex $data(list) $theIndex] 2]\n\
tkIconList_Select $w $rTag\n\
tkIconList_See $w $rTag\n\
}\n\
}\n\
\n\
proc tkIconList_Reset {w} {\n\
global tkPriv\n\
\n\
catch {unset tkPriv(ILAccel,$w)}\n\
}\n\
\n\
\n\
namespace eval ::tk::dialog {}\n\
namespace eval ::tk::dialog::file {}\n\
\n\
\n\
proc ::tk::dialog::file::tkFDialog {type args} {\n\
global tkPriv\n\
set dataName __tk_filedialog\n\
upvar ::tk::dialog::file::$dataName data\n\
\n\
::tk::dialog::file::Config $dataName $type $args\n\
\n\
if {[string equal $data(-parent) .]} {\n\
set w .$dataName\n\
} else {\n\
set w $data(-parent).$dataName\n\
}\n\
\n\
if {![winfo exists $w]} {\n\
::tk::dialog::file::Create $w TkFDialog\n\
} elseif {[string compare [winfo class $w] TkFDialog]} {\n\
destroy $w\n\
::tk::dialog::file::Create $w TkFDialog\n\
} else {\n\
set data(dirMenuBtn) $w.f1.menu\n\
set data(dirMenu) $w.f1.menu.menu\n\
set data(upBtn) $w.f1.up\n\
set data(icons) $w.icons\n\
set data(ent) $w.f2.ent\n\
set data(typeMenuLab) $w.f3.lab\n\
set data(typeMenuBtn) $w.f3.menu\n\
set data(typeMenu) $data(typeMenuBtn).m\n\
set data(okBtn) $w.f2.ok\n\
set data(cancelBtn) $w.f3.cancel\n\
}\n\
wm transient $w $data(-parent)\n\
\n\
\n\
trace variable data(selectPath) w \"::tk::dialog::file::SetPath $w\"\n\
$data(dirMenuBtn) configure \\\n\
-textvariable ::tk::dialog::file::${dataName}(selectPath)\n\
\n\
if {[llength $data(-filetypes)]} {\n\
$data(typeMenu) delete 0 end\n\
foreach type $data(-filetypes) {\n\
set title  [lindex $type 0]\n\
set filter [lindex $type 1]\n\
$data(typeMenu) add command -label $title \\\n\
-command [list ::tk::dialog::file::SetFilter $w $type]\n\
}\n\
::tk::dialog::file::SetFilter $w [lindex $data(-filetypes) 0]\n\
$data(typeMenuBtn) config -state normal\n\
$data(typeMenuLab) config -state normal\n\
} else {\n\
set data(filter) \"*\"\n\
$data(typeMenuBtn) config -state disabled -takefocus 0\n\
$data(typeMenuLab) config -state disabled\n\
}\n\
::tk::dialog::file::UpdateWhenIdle $w\n\
\n\
\n\
::tk::PlaceWindow $w widget $data(-parent)\n\
wm title $w $data(-title)\n\
\n\
\n\
::tk::SetFocusGrab $w $data(ent)\n\
$data(ent) delete 0 end\n\
$data(ent) insert 0 $data(selectFile)\n\
$data(ent) selection range 0 end\n\
$data(ent) icursor end\n\
\n\
\n\
tkwait variable tkPriv(selectFilePath)\n\
\n\
::tk::RestoreFocusGrab $w $data(ent) withdraw\n\
\n\
\n\
foreach trace [trace vinfo data(selectPath)] {\n\
trace vdelete data(selectPath) [lindex $trace 0] [lindex $trace 1]\n\
}\n\
$data(dirMenuBtn) configure -textvariable {}\n\
\n\
return $tkPriv(selectFilePath)\n\
}\n\
\n\
proc ::tk::dialog::file::Config {dataName type argList} {\n\
upvar ::tk::dialog::file::$dataName data\n\
\n\
set data(type) $type\n\
\n\
\n\
foreach trace [trace vinfo data(selectPath)] {\n\
trace vdelete data(selectPath) [lindex $trace 0] [lindex $trace 1]\n\
}\n\
\n\
set specs {\n\
{-defaultextension \"\" \"\" \"\"}\n\
{-filetypes \"\" \"\" \"\"}\n\
{-initialdir \"\" \"\" \"\"}\n\
{-initialfile \"\" \"\" \"\"}\n\
{-parent \"\" \"\" \".\"}\n\
{-title \"\" \"\" \"\"}\n\
}\n\
\n\
if {![info exists data(selectPath)]} {\n\
set data(selectPath) [pwd]\n\
set data(selectFile) \"\"\n\
}\n\
\n\
tclParseConfigSpec ::tk::dialog::file::$dataName $specs \"\" $argList\n\
\n\
if {$data(-title) == \"\"} {\n\
if {[string equal $type \"open\"]} {\n\
set data(-title) \"Open\"\n\
} else {\n\
set data(-title) \"Save As\"\n\
}\n\
}\n\
\n\
if {$data(-initialdir) != \"\"} {\n\
if {[file isdirectory $data(-initialdir)]} {\n\
set old [pwd]\n\
cd $data(-initialdir)\n\
set data(selectPath) [pwd]\n\
cd $old\n\
} else {\n\
set data(selectPath) [pwd]\n\
}\n\
}\n\
set data(selectFile) $data(-initialfile)\n\
\n\
set data(-filetypes) [tkFDGetFileTypes $data(-filetypes)]\n\
\n\
if {![winfo exists $data(-parent)]} {\n\
error \"bad window path name \\\"$data(-parent)\\\"\"\n\
}\n\
}\n\
\n\
proc ::tk::dialog::file::Create {w class} {\n\
set dataName [lindex [split $w .] end]\n\
upvar ::tk::dialog::file::$dataName data\n\
global tk_library tkPriv\n\
\n\
toplevel $w -class $class\n\
\n\
set f1 [frame $w.f1]\n\
label $f1.lab -text \"Directory:\" -under 0\n\
set data(dirMenuBtn) $f1.menu\n\
set data(dirMenu) [tk_optionMenu $f1.menu [format %s(selectPath) ::tk::dialog::file::$dataName] \"\"]\n\
set data(upBtn) [button $f1.up]\n\
if {![info exists tkPriv(updirImage)]} {\n\
set tkPriv(updirImage) [image create bitmap -data {\n\
#define updir_width 28\n\
#define updir_height 16\n\
static char updir_bits[] = {\n\
0x00, 0x00, 0x00, 0x00, 0x80, 0x1f, 0x00, 0x00, 0x40, 0x20, 0x00, 0x00,\n\
0x20, 0x40, 0x00, 0x00, 0xf0, 0xff, 0xff, 0x01, 0x10, 0x00, 0x00, 0x01,\n\
0x10, 0x02, 0x00, 0x01, 0x10, 0x07, 0x00, 0x01, 0x90, 0x0f, 0x00, 0x01,\n\
0x10, 0x02, 0x00, 0x01, 0x10, 0x02, 0x00, 0x01, 0x10, 0x02, 0x00, 0x01,\n\
0x10, 0xfe, 0x07, 0x01, 0x10, 0x00, 0x00, 0x01, 0x10, 0x00, 0x00, 0x01,\n\
0xf0, 0xff, 0xff, 0x01};}]\n\
}\n\
$data(upBtn) config -image $tkPriv(updirImage)\n\
\n\
$f1.menu config -takefocus 1 -highlightthickness 2\n\
\n\
pack $data(upBtn) -side right -padx 4 -fill both\n\
pack $f1.lab -side left -padx 4 -fill both\n\
pack $f1.menu -expand yes -fill both -padx 4\n\
\n\
if { [string equal $class TkFDialog] } {\n\
set fNameCaption \"File name:\"\n\
set fNameUnder 5\n\
set iconListCommand [list ::tk::dialog::file::OkCmd $w]\n\
} else {\n\
set fNameCaption \"Selection:\"\n\
set fNameUnder 0\n\
set iconListCommand [list ::tk::dialog::file::chooseDir::DblClick $w]\n\
}\n\
set data(icons) [tkIconList $w.icons \\\n\
-browsecmd [list ::tk::dialog::file::ListBrowse $w] \\\n\
-command   $iconListCommand]\n\
\n\
set f2 [frame $w.f2 -bd 0]\n\
label $f2.lab -text $fNameCaption -anchor e -width 14 \\\n\
-under $fNameUnder -pady 0\n\
set data(ent) [entry $f2.ent]\n\
\n\
global $w.icons\n\
set $w.icons(font) [$data(ent) cget -font]\n\
\n\
set f3 [frame $w.f3 -bd 0]\n\
\n\
if { [string equal $class TkFDialog] } {\n\
\n\
set data(typeMenuLab) [button $f3.lab -text \"Files of type:\" \\\n\
-anchor e -width 14 -under 9 \\\n\
-bd [$f2.lab cget -bd] \\\n\
-highlightthickness [$f2.lab cget -highlightthickness] \\\n\
-relief [$f2.lab cget -relief] \\\n\
-padx [$f2.lab cget -padx] \\\n\
-pady [$f2.lab cget -pady]]\n\
bindtags $data(typeMenuLab) [list $data(typeMenuLab) Label \\\n\
[winfo toplevel $data(typeMenuLab)] all]\n\
\n\
set data(typeMenuBtn) [menubutton $f3.menu -indicatoron 1 \\\n\
-menu $f3.menu.m]\n\
set data(typeMenu) [menu $data(typeMenuBtn).m -tearoff 0]\n\
$data(typeMenuBtn) config -takefocus 1 -highlightthickness 2 \\\n\
-relief raised -bd 2 -anchor w\n\
}\n\
\n\
set data(okBtn)     [button $f2.ok     -text OK     -under 0 -width 6 \\\n\
-default active -pady 3]\n\
set data(cancelBtn) [button $f3.cancel -text Cancel -under 0 -width 6\\\n\
-default normal -pady 3]\n\
\n\
pack $data(okBtn) -side right -padx 4 -anchor e\n\
pack $f2.lab -side left -padx 4\n\
pack $f2.ent -expand yes -fill x -padx 2 -pady 0\n\
\n\
pack $data(cancelBtn) -side right -padx 4 -anchor w\n\
if { [string equal $class TkFDialog] } {\n\
pack $data(typeMenuLab) -side left -padx 4\n\
pack $data(typeMenuBtn) -expand yes -fill x -side right\n\
}\n\
\n\
pack $f1 -side top -fill x -pady 4\n\
pack $f3 -side bottom -fill x\n\
pack $f2 -side bottom -fill x\n\
pack $data(icons) -expand yes -fill both -padx 4 -pady 1\n\
\n\
\n\
wm protocol $w WM_DELETE_WINDOW [list ::tk::dialog::file::CancelCmd $w]\n\
$data(upBtn)     config -command [list ::tk::dialog::file::UpDirCmd $w]\n\
$data(cancelBtn) config -command [list ::tk::dialog::file::CancelCmd $w]\n\
bind $w <KeyPress-Escape> [list tkButtonInvoke $data(cancelBtn)]\n\
bind $w <Alt-c> [list tkButtonInvoke $data(cancelBtn)]\n\
bind $w <Alt-d> [list focus $data(dirMenuBtn)]\n\
\n\
\n\
if { [string equal $class TkFDialog] } {\n\
bind $data(ent) <Return> [list ::tk::dialog::file::ActivateEnt $w]\n\
$data(okBtn)     config -command [list ::tk::dialog::file::OkCmd $w]\n\
bind $w <Alt-t> [format {\n\
if {[string equal [%s cget -state] \"normal\"]} {\n\
focus %s\n\
}\n\
} $data(typeMenuBtn) $data(typeMenuBtn)]\n\
bind $w <Alt-n> [list focus $data(ent)]\n\
bind $w <Alt-o> [list ::tk::dialog::file::InvokeBtn $w Open]\n\
bind $w <Alt-s> [list ::tk::dialog::file::InvokeBtn $w Save]\n\
} else {\n\
set okCmd [list ::tk::dialog::file::chooseDir::OkCmd $w]\n\
bind $data(ent) <Return> $okCmd\n\
$data(okBtn) config -command $okCmd\n\
bind $w <Alt-s> [list focus $data(ent)]\n\
bind $w <Alt-o> [list tkButtonInvoke $data(okBtn)]\n\
}\n\
\n\
tkFocusGroup_Create $w\n\
tkFocusGroup_BindIn $w  $data(ent) [list ::tk::dialog::file::EntFocusIn $w]\n\
tkFocusGroup_BindOut $w $data(ent) [list ::tk::dialog::file::EntFocusOut $w]\n\
}\n\
\n\
proc ::tk::dialog::file::UpdateWhenIdle {w} {\n\
upvar ::tk::dialog::file::[winfo name $w] data\n\
\n\
if {[info exists data(updateId)]} {\n\
return\n\
} else {\n\
set data(updateId) [after idle [list ::tk::dialog::file::Update $w]]\n\
}\n\
}\n\
\n\
proc ::tk::dialog::file::Update {w} {\n\
\n\
if {![winfo exists $w]} {\n\
return\n\
}\n\
set class [winfo class $w]\n\
if { [string compare $class TkFDialog] && \\\n\
[string compare $class TkChooseDir] } {\n\
return\n\
}\n\
\n\
set dataName [winfo name $w]\n\
upvar ::tk::dialog::file::$dataName data\n\
global tk_library tkPriv\n\
catch {unset data(updateId)}\n\
\n\
if {![info exists tkPriv(folderImage)]} {\n\
set tkPriv(folderImage) [image create photo -data {\n\
R0lGODlhEAAMAKEAAAD//wAAAPD/gAAAACH5BAEAAAAALAAAAAAQAAwAAAIghINhyycvVFsB\n\
QtmS3rjaH1Hg141WaT5ouprt2HHcUgAAOw==}]\n\
set tkPriv(fileImage)   [image create photo -data {\n\
R0lGODlhDAAMAKEAALLA3AAAAP//8wAAACH5BAEAAAAALAAAAAAMAAwAAAIgRI4Ha+IfWHsO\n\
rSASvJTGhnhcV3EJlo3kh53ltF5nAhQAOw==}]\n\
}\n\
set folder $tkPriv(folderImage)\n\
set file   $tkPriv(fileImage)\n\
\n\
set appPWD [pwd]\n\
if {[catch {\n\
cd $data(selectPath)\n\
}]} {\n\
tk_messageBox -type ok -parent $w -message \\\n\
\"Cannot change to the directory \\\"$data(selectPath)\\\".\\nPermission denied.\"\\\n\
-icon warning\n\
cd $appPWD\n\
return\n\
}\n\
\n\
set entCursor [$data(ent) cget -cursor]\n\
set dlgCursor [$w         cget -cursor]\n\
$data(ent) config -cursor watch\n\
$w         config -cursor watch\n\
update idletasks\n\
\n\
tkIconList_DeleteAll $data(icons)\n\
\n\
foreach f [lsort -dictionary [glob -nocomplain .* *]] {\n\
if {[string equal $f .]} {\n\
continue\n\
}\n\
if {[string equal $f ..]} {\n\
continue\n\
}\n\
if {[file isdir ./$f]} {\n\
if {![info exists hasDoneDir($f)]} {\n\
tkIconList_Add $data(icons) $folder $f\n\
set hasDoneDir($f) 1\n\
}\n\
}\n\
}\n\
if { [string equal $class TkFDialog] } {\n\
if {[string equal $data(filter) *]} {\n\
set files [lsort -dictionary \\\n\
[glob -nocomplain .* *]]\n\
} else {\n\
set files [lsort -dictionary \\\n\
[eval glob -nocomplain $data(filter)]]\n\
}\n\
\n\
foreach f $files {\n\
if {![file isdir ./$f]} {\n\
if {![info exists hasDoneFile($f)]} {\n\
tkIconList_Add $data(icons) $file $f\n\
set hasDoneFile($f) 1\n\
}\n\
}\n\
}\n\
}\n\
\n\
tkIconList_Arrange $data(icons)\n\
\n\
set list \"\"\n\
set dir \"\"\n\
foreach subdir [file split $data(selectPath)] {\n\
set dir [file join $dir $subdir]\n\
lappend list $dir\n\
}\n\
\n\
$data(dirMenu) delete 0 end\n\
set var [format %s(selectPath) ::tk::dialog::file::$dataName]\n\
foreach path $list {\n\
$data(dirMenu) add command -label $path -command [list set $var $path]\n\
}\n\
\n\
cd $appPWD\n\
\n\
if { [string equal $class TkFDialog] } {\n\
if {[string equal $data(type) open]} {\n\
$data(okBtn) config -text \"Open\"\n\
} else {\n\
$data(okBtn) config -text \"Save\"\n\
}\n\
}\n\
\n\
$data(ent) config -cursor $entCursor\n\
$w         config -cursor $dlgCursor\n\
}\n\
\n\
proc ::tk::dialog::file::SetPathSilently {w path} {\n\
upvar ::tk::dialog::file::[winfo name $w] data\n\
\n\
trace vdelete  data(selectPath) w [list ::tk::dialog::file::SetPath $w]\n\
set data(selectPath) $path\n\
trace variable data(selectPath) w [list ::tk::dialog::file::SetPath $w]\n\
}\n\
\n\
\n\
proc ::tk::dialog::file::SetPath {w name1 name2 op} {\n\
if {[winfo exists $w]} {\n\
upvar ::tk::dialog::file::[winfo name $w] data\n\
::tk::dialog::file::UpdateWhenIdle $w\n\
if { [string equal [winfo class $w] TkChooseDir] } {\n\
$data(ent) delete 0 end\n\
$data(ent) insert end $data(selectPath)\n\
}\n\
}\n\
}\n\
\n\
proc ::tk::dialog::file::SetFilter {w type} {\n\
upvar ::tk::dialog::file::[winfo name $w] data\n\
upvar \\#0 $data(icons) icons\n\
\n\
set data(filter) [lindex $type 1]\n\
$data(typeMenuBtn) config -text [lindex $type 0] -indicatoron 1\n\
\n\
$icons(sbar) set 0.0 0.0\n\
\n\
::tk::dialog::file::UpdateWhenIdle $w\n\
}\n\
\n\
proc ::tk::dialog::file::ResolveFile {context text defaultext} {\n\
\n\
set appPWD [pwd]\n\
\n\
set path [::tk::dialog::file::JoinFile $context $text]\n\
\n\
if {![file isdirectory $path] && [string equal [file ext $path] \"\"]} {\n\
set path \"$path$defaultext\"\n\
}\n\
\n\
\n\
if {[catch {file exists $path}]} {\n\
return [list ERROR $path \"\"]\n\
}\n\
\n\
if {[file exists $path]} {\n\
if {[file isdirectory $path]} {\n\
if {[catch {cd $path}]} {\n\
return [list CHDIR $path \"\"]\n\
}\n\
set directory [pwd]\n\
set file \"\"\n\
set flag OK\n\
cd $appPWD\n\
} else {\n\
if {[catch {cd [file dirname $path]}]} {\n\
return [list CHDIR [file dirname $path] \"\"]\n\
}\n\
set directory [pwd]\n\
set file [file tail $path]\n\
set flag OK\n\
cd $appPWD\n\
}\n\
} else {\n\
set dirname [file dirname $path]\n\
if {[file exists $dirname]} {\n\
if {[catch {cd $dirname}]} {\n\
return [list CHDIR $dirname \"\"]\n\
}\n\
set directory [pwd]\n\
set file [file tail $path]\n\
if {[regexp {[*]|[?]} $file]} {\n\
set flag PATTERN\n\
} else {\n\
set flag FILE\n\
}\n\
cd $appPWD\n\
} else {\n\
set directory $dirname\n\
set file [file tail $path]\n\
set flag PATH\n\
}\n\
}\n\
\n\
return [list $flag $directory $file]\n\
}\n\
\n\
\n\
proc ::tk::dialog::file::EntFocusIn {w} {\n\
upvar ::tk::dialog::file::[winfo name $w] data\n\
\n\
if {[string compare [$data(ent) get] \"\"]} {\n\
$data(ent) selection range 0 end\n\
$data(ent) icursor end\n\
} else {\n\
$data(ent) selection clear\n\
}\n\
\n\
tkIconList_Unselect $data(icons)\n\
\n\
if { [string equal [winfo class $w] TkFDialog] } {\n\
if {[string equal $data(type) open]} {\n\
$data(okBtn) config -text \"Open\"\n\
} else {\n\
$data(okBtn) config -text \"Save\"\n\
}\n\
}\n\
}\n\
\n\
proc ::tk::dialog::file::EntFocusOut {w} {\n\
upvar ::tk::dialog::file::[winfo name $w] data\n\
\n\
$data(ent) selection clear\n\
}\n\
\n\
\n\
proc ::tk::dialog::file::ActivateEnt {w} {\n\
upvar ::tk::dialog::file::[winfo name $w] data\n\
\n\
set text [string trim [$data(ent) get]]\n\
set list [::tk::dialog::file::ResolveFile $data(selectPath) $text \\\n\
$data(-defaultextension)]\n\
set flag [lindex $list 0]\n\
set path [lindex $list 1]\n\
set file [lindex $list 2]\n\
\n\
switch -- $flag {\n\
OK {\n\
if {[string equal $file \"\"]} {\n\
set data(selectPath) $path\n\
$data(ent) delete 0 end\n\
} else {\n\
::tk::dialog::file::SetPathSilently $w $path\n\
set data(selectFile) $file\n\
::tk::dialog::file::Done $w\n\
}\n\
}\n\
PATTERN {\n\
set data(selectPath) $path\n\
set data(filter) $file\n\
}\n\
FILE {\n\
if {[string equal $data(type) open]} {\n\
tk_messageBox -icon warning -type ok -parent $w \\\n\
-message \"File \\\"[file join $path $file]\\\" does not exist.\"\n\
$data(ent) selection range 0 end\n\
$data(ent) icursor end\n\
} else {\n\
::tk::dialog::file::SetPathSilently $w $path\n\
set data(selectFile) $file\n\
::tk::dialog::file::Done $w\n\
}\n\
}\n\
PATH {\n\
tk_messageBox -icon warning -type ok -parent $w \\\n\
-message \"Directory \\\"$path\\\" does not exist.\"\n\
$data(ent) selection range 0 end\n\
$data(ent) icursor end\n\
}\n\
CHDIR {\n\
tk_messageBox -type ok -parent $w -message \\\n\
\"Cannot change to the directory \\\"$path\\\".\\nPermission denied.\"\\\n\
-icon warning\n\
$data(ent) selection range 0 end\n\
$data(ent) icursor end\n\
}\n\
ERROR {\n\
tk_messageBox -type ok -parent $w -message \\\n\
\"Invalid file name \\\"$path\\\".\"\\\n\
-icon warning\n\
$data(ent) selection range 0 end\n\
$data(ent) icursor end\n\
}\n\
}\n\
}\n\
\n\
proc ::tk::dialog::file::InvokeBtn {w key} {\n\
upvar ::tk::dialog::file::[winfo name $w] data\n\
\n\
if {[string equal [$data(okBtn) cget -text] $key]} {\n\
tkButtonInvoke $data(okBtn)\n\
}\n\
}\n\
\n\
proc ::tk::dialog::file::UpDirCmd {w} {\n\
upvar ::tk::dialog::file::[winfo name $w] data\n\
\n\
if {[string compare $data(selectPath) \"/\"]} {\n\
set data(selectPath) [file dirname $data(selectPath)]\n\
}\n\
}\n\
\n\
proc ::tk::dialog::file::JoinFile {path file} {\n\
if {[string match {~*} $file] && [file exists $path/$file]} {\n\
return [file join $path ./$file]\n\
} else {\n\
return [file join $path $file]\n\
}\n\
}\n\
\n\
\n\
\n\
proc ::tk::dialog::file::OkCmd {w} {\n\
upvar ::tk::dialog::file::[winfo name $w] data\n\
\n\
set text [tkIconList_Get $data(icons)]\n\
if {[string compare $text \"\"]} {\n\
set file [::tk::dialog::file::JoinFile $data(selectPath) $text]\n\
if {[file isdirectory $file]} {\n\
::tk::dialog::file::ListInvoke $w $text\n\
return\n\
}\n\
}\n\
\n\
::tk::dialog::file::ActivateEnt $w\n\
}\n\
\n\
proc ::tk::dialog::file::CancelCmd {w} {\n\
upvar ::tk::dialog::file::[winfo name $w] data\n\
global tkPriv\n\
\n\
set tkPriv(selectFilePath) \"\"\n\
}\n\
\n\
proc ::tk::dialog::file::ListBrowse {w text} {\n\
upvar ::tk::dialog::file::[winfo name $w] data\n\
\n\
if {[string equal $text \"\"]} {\n\
return\n\
}\n\
\n\
set file [::tk::dialog::file::JoinFile $data(selectPath) $text]\n\
if {![file isdirectory $file]} {\n\
$data(ent) delete 0 end\n\
$data(ent) insert 0 $text\n\
\n\
if { [string equal [winfo class $w] TkFDialog] } {\n\
if {[string equal $data(type) open]} {\n\
$data(okBtn) config -text \"Open\"\n\
} else {\n\
$data(okBtn) config -text \"Save\"\n\
}\n\
}\n\
} else {\n\
if { [string equal [winfo class $w] TkFDialog] } {\n\
$data(okBtn) config -text \"Open\"\n\
}\n\
}\n\
}\n\
\n\
proc ::tk::dialog::file::ListInvoke {w text} {\n\
upvar ::tk::dialog::file::[winfo name $w] data\n\
\n\
if {[string equal $text \"\"]} {\n\
return\n\
}\n\
\n\
set file [::tk::dialog::file::JoinFile $data(selectPath) $text]\n\
set class [winfo class $w]\n\
if {[string equal $class TkChooseDir] || [file isdirectory $file]} {\n\
set appPWD [pwd]\n\
if {[catch {cd $file}]} {\n\
tk_messageBox -type ok -parent $w -message \\\n\
\"Cannot change to the directory \\\"$file\\\".\\nPermission denied.\"\\\n\
-icon warning\n\
} else {\n\
cd $appPWD\n\
set data(selectPath) $file\n\
}\n\
} else {\n\
set data(selectFile) $file\n\
::tk::dialog::file::Done $w\n\
}\n\
}\n\
\n\
proc ::tk::dialog::file::Done {w {selectFilePath \"\"}} {\n\
upvar ::tk::dialog::file::[winfo name $w] data\n\
global tkPriv\n\
\n\
if {[string equal $selectFilePath \"\"]} {\n\
set selectFilePath [::tk::dialog::file::JoinFile $data(selectPath) \\\n\
$data(selectFile)]\n\
set tkPriv(selectFile)     $data(selectFile)\n\
set tkPriv(selectPath)     $data(selectPath)\n\
\n\
if {[file exists $selectFilePath] && [string equal $data(type) save]} {\n\
set reply [tk_messageBox -icon warning -type yesno\\\n\
-parent $w -message \"File\\\n\
\\\"$selectFilePath\\\" already exists.\\nDo\\\n\
you want to overwrite it?\"]\n\
if {[string equal $reply \"no\"]} {\n\
return\n\
}\n\
}\n\
}\n\
set tkPriv(selectFilePath) $selectFilePath\n\
}\n\
\n\
namespace eval ::tk::dialog {}\n\
\n\
image create bitmap ::tk::dialog::b1 -foreground black \\\n\
-data \"#define b1_width 32\\n#define b1_height 32\n\
static unsigned char q1_bits[] = {\n\
0x00, 0xf8, 0x1f, 0x00, 0x00, 0x07, 0xe0, 0x00, 0xc0, 0x00, 0x00, 0x03,\n\
0x20, 0x00, 0x00, 0x04, 0x10, 0x00, 0x00, 0x08, 0x08, 0x00, 0x00, 0x10,\n\
0x04, 0x00, 0x00, 0x20, 0x02, 0x00, 0x00, 0x40, 0x02, 0x00, 0x00, 0x40,\n\
0x01, 0x00, 0x00, 0x80, 0x01, 0x00, 0x00, 0x80, 0x01, 0x00, 0x00, 0x80,\n\
0x01, 0x00, 0x00, 0x80, 0x01, 0x00, 0x00, 0x80, 0x01, 0x00, 0x00, 0x80,\n\
0x01, 0x00, 0x00, 0x80, 0x02, 0x00, 0x00, 0x40, 0x02, 0x00, 0x00, 0x40,\n\
0x04, 0x00, 0x00, 0x20, 0x08, 0x00, 0x00, 0x10, 0x10, 0x00, 0x00, 0x08,\n\
0x60, 0x00, 0x00, 0x04, 0x80, 0x03, 0x80, 0x03, 0x00, 0x0c, 0x78, 0x00,\n\
0x00, 0x30, 0x04, 0x00, 0x00, 0x40, 0x04, 0x00, 0x00, 0x40, 0x04, 0x00,\n\
0x00, 0x80, 0x04, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00, 0x06, 0x00,\n\
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};\"\n\
image create bitmap ::tk::dialog::b2 -foreground white \\\n\
-data \"#define b2_width 32\\n#define b2_height 32\n\
static unsigned char b2_bits[] = {\n\
0x00, 0x00, 0x00, 0x00, 0x00, 0xf8, 0x1f, 0x00, 0x00, 0xff, 0xff, 0x00,\n\
0xc0, 0xff, 0xff, 0x03, 0xe0, 0xff, 0xff, 0x07, 0xf0, 0xff, 0xff, 0x0f,\n\
0xf8, 0xff, 0xff, 0x1f, 0xfc, 0xff, 0xff, 0x3f, 0xfc, 0xff, 0xff, 0x3f,\n\
0xfe, 0xff, 0xff, 0x7f, 0xfe, 0xff, 0xff, 0x7f, 0xfe, 0xff, 0xff, 0x7f,\n\
0xfe, 0xff, 0xff, 0x7f, 0xfe, 0xff, 0xff, 0x7f, 0xfe, 0xff, 0xff, 0x7f,\n\
0xfe, 0xff, 0xff, 0x7f, 0xfc, 0xff, 0xff, 0x3f, 0xfc, 0xff, 0xff, 0x3f,\n\
0xf8, 0xff, 0xff, 0x1f, 0xf0, 0xff, 0xff, 0x0f, 0xe0, 0xff, 0xff, 0x07,\n\
0x80, 0xff, 0xff, 0x03, 0x00, 0xfc, 0x7f, 0x00, 0x00, 0xf0, 0x07, 0x00,\n\
0x00, 0xc0, 0x03, 0x00, 0x00, 0x80, 0x03, 0x00, 0x00, 0x80, 0x03, 0x00,\n\
0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00,\n\
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};\"\n\
image create bitmap ::tk::dialog::q -foreground blue \\\n\
-data \"#define q_width 32\\n#define q_height 32\n\
static unsigned char q_bits[] = {\n\
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,\n\
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe0, 0x07, 0x00,\n\
0x00, 0x10, 0x0f, 0x00, 0x00, 0x18, 0x1e, 0x00, 0x00, 0x38, 0x1e, 0x00,\n\
0x00, 0x38, 0x1e, 0x00, 0x00, 0x10, 0x0f, 0x00, 0x00, 0x80, 0x07, 0x00,\n\
0x00, 0xc0, 0x01, 0x00, 0x00, 0xc0, 0x00, 0x00, 0x00, 0xc0, 0x00, 0x00,\n\
0x00, 0x00, 0x00, 0x00, 0x00, 0xc0, 0x00, 0x00, 0x00, 0xe0, 0x01, 0x00,\n\
0x00, 0xe0, 0x01, 0x00, 0x00, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,\n\
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,\n\
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,\n\
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,\n\
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};\"\n\
image create bitmap ::tk::dialog::i -foreground blue \\\n\
-data \"#define i_width 32\\n#define i_height 32\n\
static unsigned char i_bits[] = {\n\
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,\n\
0x00, 0xe0, 0x01, 0x00, 0x00, 0xf0, 0x03, 0x00, 0x00, 0xf0, 0x03, 0x00,\n\
0x00, 0xe0, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,\n\
0x00, 0xf8, 0x03, 0x00, 0x00, 0xf0, 0x03, 0x00, 0x00, 0xe0, 0x03, 0x00,\n\
0x00, 0xe0, 0x03, 0x00, 0x00, 0xe0, 0x03, 0x00, 0x00, 0xe0, 0x03, 0x00,\n\
0x00, 0xe0, 0x03, 0x00, 0x00, 0xe0, 0x03, 0x00, 0x00, 0xf0, 0x07, 0x00,\n\
0x00, 0xf8, 0x0f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,\n\
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,\n\
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,\n\
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,\n\
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};\"\n\
image create bitmap ::tk::dialog::w1 -foreground black \\\n\
-data \"#define w1_width 32\\n#define w1_height 32\n\
static unsigned char w1_bits[] = {\n\
0x00, 0x80, 0x01, 0x00, 0x00, 0x40, 0x02, 0x00, 0x00, 0x20, 0x04, 0x00,\n\
0x00, 0x10, 0x04, 0x00, 0x00, 0x10, 0x08, 0x00, 0x00, 0x08, 0x08, 0x00,\n\
0x00, 0x08, 0x10, 0x00, 0x00, 0x04, 0x10, 0x00, 0x00, 0x04, 0x20, 0x00,\n\
0x00, 0x02, 0x20, 0x00, 0x00, 0x02, 0x40, 0x00, 0x00, 0x01, 0x40, 0x00,\n\
0x00, 0x01, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x00, 0x01,\n\
0x40, 0x00, 0x00, 0x01, 0x40, 0x00, 0x00, 0x02, 0x20, 0x00, 0x00, 0x02,\n\
0x20, 0x00, 0x00, 0x04, 0x10, 0x00, 0x00, 0x04, 0x10, 0x00, 0x00, 0x08,\n\
0x08, 0x00, 0x00, 0x08, 0x08, 0x00, 0x00, 0x10, 0x04, 0x00, 0x00, 0x10,\n\
0x04, 0x00, 0x00, 0x20, 0x02, 0x00, 0x00, 0x20, 0x01, 0x00, 0x00, 0x40,\n\
0x01, 0x00, 0x00, 0x40, 0x01, 0x00, 0x00, 0x40, 0x02, 0x00, 0x00, 0x20,\n\
0xfc, 0xff, 0xff, 0x1f, 0x00, 0x00, 0x00, 0x00};\"\n\
image create bitmap ::tk::dialog::w2 -foreground yellow \\\n\
-data \"#define w2_width 32\\n#define w2_height 32\n\
static unsigned char w2_bits[] = {\n\
0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x01, 0x00, 0x00, 0xc0, 0x03, 0x00,\n\
0x00, 0xe0, 0x03, 0x00, 0x00, 0xe0, 0x07, 0x00, 0x00, 0xf0, 0x07, 0x00,\n\
0x00, 0xf0, 0x0f, 0x00, 0x00, 0xf8, 0x0f, 0x00, 0x00, 0xf8, 0x1f, 0x00,\n\
0x00, 0xfc, 0x1f, 0x00, 0x00, 0xfc, 0x3f, 0x00, 0x00, 0xfe, 0x3f, 0x00,\n\
0x00, 0xfe, 0x7f, 0x00, 0x00, 0xff, 0x7f, 0x00, 0x00, 0xff, 0xff, 0x00,\n\
0x80, 0xff, 0xff, 0x00, 0x80, 0xff, 0xff, 0x01, 0xc0, 0xff, 0xff, 0x01,\n\
0xc0, 0xff, 0xff, 0x03, 0xe0, 0xff, 0xff, 0x03, 0xe0, 0xff, 0xff, 0x07,\n\
0xf0, 0xff, 0xff, 0x07, 0xf0, 0xff, 0xff, 0x0f, 0xf8, 0xff, 0xff, 0x0f,\n\
0xf8, 0xff, 0xff, 0x1f, 0xfc, 0xff, 0xff, 0x1f, 0xfe, 0xff, 0xff, 0x3f,\n\
0xfe, 0xff, 0xff, 0x3f, 0xfe, 0xff, 0xff, 0x3f, 0xfc, 0xff, 0xff, 0x1f,\n\
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};\"\n\
image create bitmap ::tk::dialog::w3 -foreground black \\\n\
-data \"#define w3_width 32\\n#define w3_height 32\n\
static unsigned char w3_bits[] = {\n\
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,\n\
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,\n\
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,\n\
0x00, 0xc0, 0x03, 0x00, 0x00, 0xe0, 0x07, 0x00, 0x00, 0xe0, 0x07, 0x00,\n\
0x00, 0xe0, 0x07, 0x00, 0x00, 0xe0, 0x07, 0x00, 0x00, 0xe0, 0x07, 0x00,\n\
0x00, 0xc0, 0x03, 0x00, 0x00, 0xc0, 0x03, 0x00, 0x00, 0xc0, 0x03, 0x00,\n\
0x00, 0x80, 0x01, 0x00, 0x00, 0x80, 0x01, 0x00, 0x00, 0x80, 0x01, 0x00,\n\
0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x01, 0x00, 0x00, 0xc0, 0x03, 0x00,\n\
0x00, 0xc0, 0x03, 0x00, 0x00, 0x80, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00,\n\
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,\n\
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};\"\n\
\n\
proc tkMessageBox {args} {\n\
global tkPriv tcl_platform tk_strictMotif\n\
\n\
set w tkPrivMsgBox\n\
upvar #0 $w data\n\
\n\
set specs {\n\
{-default \"\" \"\" \"\"}\n\
{-icon \"\" \"\" \"info\"}\n\
{-message \"\" \"\" \"\"}\n\
{-parent \"\" \"\" .}\n\
{-title \"\" \"\" \" \"}\n\
{-type \"\" \"\" \"ok\"}\n\
}\n\
\n\
tclParseConfigSpec $w $specs \"\" $args\n\
\n\
if {[lsearch -exact {info warning error question} $data(-icon)] == -1} {\n\
error \"bad -icon value \\\"$data(-icon)\\\": must be error, info, question, or warning\"\n\
}\n\
if {[string equal $tcl_platform(platform) \"macintosh\"]} {\n\
switch -- $data(-icon) {\n\
\"error\"     {set data(-icon) \"stop\"}\n\
\"warning\"   {set data(-icon) \"caution\"}\n\
\"info\"      {set data(-icon) \"note\"}\n\
}\n\
}\n\
\n\
if {![winfo exists $data(-parent)]} {\n\
error \"bad window path name \\\"$data(-parent)\\\"\"\n\
}\n\
\n\
switch -- $data(-type) {\n\
abortretryignore {\n\
set buttons {\n\
{abort  -width 6 -text Abort -under 0}\n\
{retry  -width 6 -text Retry -under 0}\n\
{ignore -width 6 -text Ignore -under 0}\n\
}\n\
}\n\
ok {\n\
set buttons {\n\
{ok -width 6 -text OK -under 0}\n\
}\n\
if {[string equal $data(-default) \"\"]} {\n\
set data(-default) \"ok\"\n\
}\n\
}\n\
okcancel {\n\
set buttons {\n\
{ok     -width 6 -text OK     -under 0}\n\
{cancel -width 6 -text Cancel -under 0}\n\
}\n\
}\n\
retrycancel {\n\
set buttons {\n\
{retry  -width 6 -text Retry  -under 0}\n\
{cancel -width 6 -text Cancel -under 0}\n\
}\n\
}\n\
yesno {\n\
set buttons {\n\
{yes    -width 6 -text Yes -under 0}\n\
{no     -width 6 -text No  -under 0}\n\
}\n\
}\n\
yesnocancel {\n\
set buttons {\n\
{yes    -width 6 -text Yes -under 0}\n\
{no     -width 6 -text No  -under 0}\n\
{cancel -width 6 -text Cancel -under 0}\n\
}\n\
}\n\
default {\n\
error \"bad -type value \\\"$data(-type)\\\": must be abortretryignore, ok, okcancel, retrycancel, yesno, or yesnocancel\"\n\
}\n\
}\n\
\n\
if {[string compare $data(-default) \"\"]} {\n\
set valid 0\n\
foreach btn $buttons {\n\
if {[string equal [lindex $btn 0] $data(-default)]} {\n\
set valid 1\n\
break\n\
}\n\
}\n\
if {!$valid} {\n\
error \"invalid default button \\\"$data(-default)\\\"\"\n\
}\n\
}\n\
\n\
if {[string compare $data(-parent) .]} {\n\
set w $data(-parent).__tk__messagebox\n\
} else {\n\
set w .__tk__messagebox\n\
}\n\
\n\
\n\
catch {destroy $w}\n\
toplevel $w -class Dialog\n\
wm title $w $data(-title)\n\
wm iconname $w Dialog\n\
wm protocol $w WM_DELETE_WINDOW { }\n\
\n\
if { [winfo viewable [winfo toplevel $data(-parent)]] } {\n\
wm transient $w $data(-parent)\n\
}    \n\
\n\
if {[string equal $tcl_platform(platform) \"macintosh\"]} {\n\
unsupported1 style $w dBoxProc\n\
}\n\
\n\
frame $w.bot\n\
pack $w.bot -side bottom -fill both\n\
frame $w.top\n\
pack $w.top -side top -fill both -expand 1\n\
if {[string compare $tcl_platform(platform) \"macintosh\"]} {\n\
$w.bot configure -relief raised -bd 1\n\
$w.top configure -relief raised -bd 1\n\
}\n\
\n\
\n\
option add *Dialog.msg.wrapLength 3i widgetDefault\n\
if {[string equal $tcl_platform(platform) \"macintosh\"]} {\n\
option add *Dialog.msg.font system widgetDefault\n\
} else {\n\
option add *Dialog.msg.font {Times 18} widgetDefault\n\
}\n\
\n\
label $w.msg -anchor nw -justify left -text $data(-message)\n\
if {[string compare $data(-icon) \"\"]} {\n\
if {[string equal $tcl_platform(platform) \"macintosh\"] \\\n\
|| ([winfo depth $w] < 4) || $tk_strictMotif} {\n\
label $w.bitmap -bitmap $data(-icon)\n\
} else {\n\
canvas $w.bitmap -width 32 -height 32 -highlightthickness 0\n\
switch $data(-icon) {\n\
error {\n\
$w.bitmap create oval 0 0 31 31 -fill red -outline black\n\
$w.bitmap create line 9 9 23 23 -fill white -width 4\n\
$w.bitmap create line 9 23 23 9 -fill white -width 4\n\
}\n\
info {\n\
$w.bitmap create image 0 0 -anchor nw \\\n\
-image ::tk::dialog::b1\n\
$w.bitmap create image 0 0 -anchor nw \\\n\
-image ::tk::dialog::b2\n\
$w.bitmap create image 0 0 -anchor nw \\\n\
-image ::tk::dialog::i\n\
}\n\
question {\n\
$w.bitmap create image 0 0 -anchor nw \\\n\
-image ::tk::dialog::b1\n\
$w.bitmap create image 0 0 -anchor nw \\\n\
-image ::tk::dialog::b2\n\
$w.bitmap create image 0 0 -anchor nw \\\n\
-image ::tk::dialog::q\n\
}\n\
default {\n\
$w.bitmap create image 0 0 -anchor nw \\\n\
-image ::tk::dialog::w1\n\
$w.bitmap create image 0 0 -anchor nw \\\n\
-image ::tk::dialog::w2\n\
$w.bitmap create image 0 0 -anchor nw \\\n\
-image ::tk::dialog::w3\n\
}\n\
}\n\
}\n\
}\n\
grid $w.bitmap $w.msg -in $w.top -sticky news -padx 2m -pady 2m\n\
grid columnconfigure $w.top 1 -weight 1\n\
grid rowconfigure $w.top 0 -weight 1\n\
\n\
\n\
set i 0\n\
foreach but $buttons {\n\
set name [lindex $but 0]\n\
set opts [lrange $but 1 end]\n\
if {![llength $opts]} {\n\
set capName [string toupper $name 0]\n\
set opts [list -text $capName]\n\
}\n\
\n\
eval button [list $w.$name] $opts [list -command [list set tkPriv(button) $name]]\n\
\n\
if {[string equal $name $data(-default)]} {\n\
$w.$name configure -default active\n\
}\n\
pack $w.$name -in $w.bot -side left -expand 1 -padx 3m -pady 2m\n\
\n\
set underIdx [$w.$name cget -under]\n\
if {$underIdx >= 0} {\n\
set key [string index [$w.$name cget -text] $underIdx]\n\
bind $w <Alt-[string tolower $key]>  [list $w.$name invoke]\n\
bind $w <Alt-[string toupper $key]>  [list $w.$name invoke]\n\
}\n\
incr i\n\
}\n\
\n\
if {[string compare {} $data(-default)]} {\n\
bind $w <FocusIn> {\n\
if {[string equal Button [winfo class %W]]} {\n\
%W configure -default active\n\
}\n\
}\n\
bind $w <FocusOut> {\n\
if {[string equal Button [winfo class %W]]} {\n\
%W configure -default normal\n\
}\n\
}\n\
}\n\
\n\
\n\
bind $w <Return> {\n\
if {[string equal Button [winfo class %W]]} {\n\
tkButtonInvoke %W\n\
}\n\
}\n\
\n\
\n\
::tk::PlaceWindow $w widget $data(-parent)\n\
\n\
\n\
if {[string compare $data(-default) \"\"]} {\n\
set focus $w.$data(-default)\n\
} else {\n\
set focus $w\n\
}\n\
::tk::SetFocusGrab $w $focus\n\
\n\
\n\
tkwait variable tkPriv(button)\n\
\n\
::tk::RestoreFocusGrab $w $focus\n\
\n\
return $tkPriv(button)\n\
}\n\
\n\
\n\
proc tk_optionMenu {w varName firstValue args} {\n\
upvar #0 $varName var\n\
\n\
if {![info exists var]} {\n\
set var $firstValue\n\
}\n\
menubutton $w -textvariable $varName -indicatoron 1 -menu $w.menu \\\n\
-relief raised -bd 2 -highlightthickness 2 -anchor c \\\n\
-direction flush\n\
menu $w.menu -tearoff 0\n\
$w.menu add radiobutton -label $firstValue -variable $varName\n\
foreach i $args {\n\
$w.menu add radiobutton -label $i -variable $varName\n\
}\n\
return $w.menu\n\
}\n\
\n\
\n\
if {[string compare $tcl_platform(platform) \"windows\"] && \\\n\
[string compare $tcl_platform(platform) \"macintosh\"]} {\n\
\n\
bind Scrollbar <Enter> {\n\
if {$tk_strictMotif} {\n\
set tkPriv(activeBg) [%W cget -activebackground]\n\
%W config -activebackground [%W cget -background]\n\
}\n\
%W activate [%W identify %x %y]\n\
}\n\
bind Scrollbar <Motion> {\n\
%W activate [%W identify %x %y]\n\
}\n\
\n\
\n\
bind Scrollbar <Leave> {\n\
if {$tk_strictMotif && [info exists tkPriv(activeBg)]} {\n\
%W config -activebackground $tkPriv(activeBg)\n\
}\n\
%W activate {}\n\
}\n\
bind Scrollbar <1> {\n\
tkScrollButtonDown %W %x %y\n\
}\n\
bind Scrollbar <B1-Motion> {\n\
tkScrollDrag %W %x %y\n\
}\n\
bind Scrollbar <B1-B2-Motion> {\n\
tkScrollDrag %W %x %y\n\
}\n\
bind Scrollbar <ButtonRelease-1> {\n\
tkScrollButtonUp %W %x %y\n\
}\n\
bind Scrollbar <B1-Leave> {\n\
}\n\
bind Scrollbar <B1-Enter> {\n\
}\n\
bind Scrollbar <2> {\n\
tkScrollButton2Down %W %x %y\n\
}\n\
bind Scrollbar <B1-2> {\n\
}\n\
bind Scrollbar <B2-1> {\n\
}\n\
bind Scrollbar <B2-Motion> {\n\
tkScrollDrag %W %x %y\n\
}\n\
bind Scrollbar <ButtonRelease-2> {\n\
tkScrollButtonUp %W %x %y\n\
}\n\
bind Scrollbar <B1-ButtonRelease-2> {\n\
}\n\
bind Scrollbar <B2-ButtonRelease-1> {\n\
}\n\
bind Scrollbar <B2-Leave> {\n\
}\n\
bind Scrollbar <B2-Enter> {\n\
}\n\
bind Scrollbar <Control-1> {\n\
tkScrollTopBottom %W %x %y\n\
}\n\
bind Scrollbar <Control-2> {\n\
tkScrollTopBottom %W %x %y\n\
}\n\
\n\
bind Scrollbar <Up> {\n\
tkScrollByUnits %W v -1\n\
}\n\
bind Scrollbar <Down> {\n\
tkScrollByUnits %W v 1\n\
}\n\
bind Scrollbar <Control-Up> {\n\
tkScrollByPages %W v -1\n\
}\n\
bind Scrollbar <Control-Down> {\n\
tkScrollByPages %W v 1\n\
}\n\
bind Scrollbar <Left> {\n\
tkScrollByUnits %W h -1\n\
}\n\
bind Scrollbar <Right> {\n\
tkScrollByUnits %W h 1\n\
}\n\
bind Scrollbar <Control-Left> {\n\
tkScrollByPages %W h -1\n\
}\n\
bind Scrollbar <Control-Right> {\n\
tkScrollByPages %W h 1\n\
}\n\
bind Scrollbar <Prior> {\n\
tkScrollByPages %W hv -1\n\
}\n\
bind Scrollbar <Next> {\n\
tkScrollByPages %W hv 1\n\
}\n\
bind Scrollbar <Home> {\n\
tkScrollToPos %W 0\n\
}\n\
bind Scrollbar <End> {\n\
tkScrollToPos %W 1\n\
}\n\
}\n\
\n\
proc tkScrollButtonDown {w x y} {\n\
global tkPriv\n\
set tkPriv(relief) [$w cget -activerelief]\n\
$w configure -activerelief sunken\n\
set element [$w identify $x $y]\n\
if {[string equal $element \"slider\"]} {\n\
tkScrollStartDrag $w $x $y\n\
} else {\n\
tkScrollSelect $w $element initial\n\
}\n\
}\n\
\n\
\n\
proc tkScrollButtonUp {w x y} {\n\
global tkPriv\n\
tkCancelRepeat\n\
if {[info exists tkPriv(relief)]} {\n\
$w configure -activerelief $tkPriv(relief)\n\
tkScrollEndDrag $w $x $y\n\
$w activate [$w identify $x $y]\n\
}\n\
}\n\
\n\
\n\
proc tkScrollSelect {w element repeat} {\n\
global tkPriv\n\
if {![winfo exists $w]} return\n\
switch -- $element {\n\
\"arrow1\"	{tkScrollByUnits $w hv -1}\n\
\"trough1\"	{tkScrollByPages $w hv -1}\n\
\"trough2\"	{tkScrollByPages $w hv 1}\n\
\"arrow2\"	{tkScrollByUnits $w hv 1}\n\
default		{return}\n\
}\n\
if {[string equal $repeat \"again\"]} {\n\
set tkPriv(afterId) [after [$w cget -repeatinterval] \\\n\
[list tkScrollSelect $w $element again]]\n\
} elseif {[string equal $repeat \"initial\"]} {\n\
set delay [$w cget -repeatdelay]\n\
if {$delay > 0} {\n\
set tkPriv(afterId) [after $delay \\\n\
[list tkScrollSelect $w $element again]]\n\
}\n\
}\n\
}\n\
\n\
\n\
proc tkScrollStartDrag {w x y} {\n\
global tkPriv\n\
\n\
if {[string equal [$w cget -command] \"\"]} {\n\
return\n\
}\n\
set tkPriv(pressX) $x\n\
set tkPriv(pressY) $y\n\
set tkPriv(initValues) [$w get]\n\
set iv0 [lindex $tkPriv(initValues) 0]\n\
if {[llength $tkPriv(initValues)] == 2} {\n\
set tkPriv(initPos) $iv0\n\
} elseif {$iv0 == 0} {\n\
set tkPriv(initPos) 0.0\n\
} else {\n\
set tkPriv(initPos) [expr {(double([lindex $tkPriv(initValues) 2])) \\\n\
/ [lindex $tkPriv(initValues) 0]}]\n\
}\n\
}\n\
\n\
\n\
proc tkScrollDrag {w x y} {\n\
global tkPriv\n\
\n\
if {[string equal $tkPriv(initPos) \"\"]} {\n\
return\n\
}\n\
set delta [$w delta [expr {$x - $tkPriv(pressX)}] [expr {$y - $tkPriv(pressY)}]]\n\
if {[$w cget -jump]} {\n\
if {[llength $tkPriv(initValues)] == 2} {\n\
$w set [expr {[lindex $tkPriv(initValues) 0] + $delta}] \\\n\
[expr {[lindex $tkPriv(initValues) 1] + $delta}]\n\
} else {\n\
set delta [expr {round($delta * [lindex $tkPriv(initValues) 0])}]\n\
eval [list $w] set [lreplace $tkPriv(initValues) 2 3 \\\n\
[expr {[lindex $tkPriv(initValues) 2] + $delta}] \\\n\
[expr {[lindex $tkPriv(initValues) 3] + $delta}]]\n\
}\n\
} else {\n\
tkScrollToPos $w [expr {$tkPriv(initPos) + $delta}]\n\
}\n\
}\n\
\n\
\n\
proc tkScrollEndDrag {w x y} {\n\
global tkPriv\n\
\n\
if {[string equal $tkPriv(initPos) \"\"]} {\n\
return\n\
}\n\
if {[$w cget -jump]} {\n\
set delta [$w delta [expr {$x - $tkPriv(pressX)}] \\\n\
[expr {$y - $tkPriv(pressY)}]]\n\
tkScrollToPos $w [expr {$tkPriv(initPos) + $delta}]\n\
}\n\
set tkPriv(initPos) \"\"\n\
}\n\
\n\
\n\
proc tkScrollByUnits {w orient amount} {\n\
set cmd [$w cget -command]\n\
if {[string equal $cmd \"\"] || ([string first \\\n\
[string index [$w cget -orient] 0] $orient] < 0)} {\n\
return\n\
}\n\
set info [$w get]\n\
if {[llength $info] == 2} {\n\
uplevel #0 $cmd scroll $amount units\n\
} else {\n\
uplevel #0 $cmd [expr {[lindex $info 2] + $amount}]\n\
}\n\
}\n\
\n\
\n\
proc tkScrollByPages {w orient amount} {\n\
set cmd [$w cget -command]\n\
if {[string equal $cmd \"\"] || ([string first \\\n\
[string index [$w cget -orient] 0] $orient] < 0)} {\n\
return\n\
}\n\
set info [$w get]\n\
if {[llength $info] == 2} {\n\
uplevel #0 $cmd scroll $amount pages\n\
} else {\n\
uplevel #0 $cmd [expr {[lindex $info 2] + $amount*([lindex $info 1] - 1)}]\n\
}\n\
}\n\
\n\
\n\
proc tkScrollToPos {w pos} {\n\
set cmd [$w cget -command]\n\
if {[string equal $cmd \"\"]} {\n\
return\n\
}\n\
set info [$w get]\n\
if {[llength $info] == 2} {\n\
uplevel #0 $cmd moveto $pos\n\
} else {\n\
uplevel #0 $cmd [expr {round([lindex $info 0]*$pos)}]\n\
}\n\
}\n\
\n\
\n\
proc tkScrollTopBottom {w x y} {\n\
global tkPriv\n\
set element [$w identify $x $y]\n\
if {[string match *1 $element]} {\n\
tkScrollToPos $w 0\n\
} elseif {[string match *2 $element]} {\n\
tkScrollToPos $w 1\n\
}\n\
\n\
\n\
set tkPriv(relief) [$w cget -activerelief]\n\
}\n\
\n\
\n\
proc tkScrollButton2Down {w x y} {\n\
global tkPriv\n\
set element [$w identify $x $y]\n\
if {[string match {arrow[12]} $element]} {\n\
tkScrollButtonDown $w $x $y\n\
return\n\
}\n\
tkScrollToPos $w [$w fraction $x $y]\n\
set tkPriv(relief) [$w cget -activerelief]\n\
\n\
\n\
update idletasks\n\
$w configure -activerelief sunken\n\
$w activate slider\n\
tkScrollStartDrag $w $x $y\n\
}\n\
";
#include "tclcl.h"
EmbeddedTcl et_tk(code);
