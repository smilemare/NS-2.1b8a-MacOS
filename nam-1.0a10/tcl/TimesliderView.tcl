#Copyright (C) 1998 by USC/ISI
# All rights reserved.
#
# Redistribution and use in source and binary forms are permitted
# provided that the above copyright notice and this paragraph are
# duplicated in all such forms and that any documentation, advertising
# materials, and other materials related to such distribution and use
# acknowledge that the software was developed by the University of
# Southern California, Information Sciences Institute.  The name of the
# University may not be used to endorse or promote products derived from
# this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR IMPLIED
# WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED WARRANTIES OF
# MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
#

Class TimesliderView -superclass Observer

TimesliderView instproc frame {} {
	$self instvar timeslider
	return $timeslider(frame)
}

TimesliderView instproc swidth {} {
	$self instvar timeslider
	return $timeslider(swidth)
}

TimesliderView instproc init {w mid} {

        $self next

	$self instvar timeslider timeslider_width timeslider_tag \
                    timeslider_pos range mid_

	set mid_ $mid

	set mintime [$mid_ getmintime]
	set maxtime [$mid_ getmaxtime]

	set range [expr $maxtime - $mintime]

        frame $w.f -borderwidth 2 -relief groove
        pack $w.f -side top -fill x -expand 1 

        set timeslider(height) 12
        set timeslider(swidth) 32
        set timeslider(width) 32 
        set timeslider_width($w.f.c) 32
        set timeslider(canvas) $w.f.c
        set timeslider(frame) $w 

        canvas $w.f.c -height $timeslider(height) -width 300 \
                        -background white -highlightthickness 0
        pack $w.f.c -side left -fill x -expand 1 -padx 0 -pady 0

        label $w.f.c.b -bitmap time -highlightthickness 0 -borderwidth 1 \
                        -relief raised
        set timeslider_tag($w.f.c) [$w.f.c create window \
                        [expr $timeslider(swidth)/2] 0 -window $w.f.c.b \
                        -height 12 -width $timeslider(swidth) -anchor n]
        set timeslider_pos($w.f.c) 0  

        bind $w.f.c <ButtonPress-1> "$self timeslidertrough $w.f.c %x %y"
        bind $w.f.c.b <ButtonPress-1> "$self timesliderpress $w.f.c %x %y;break"
        bind $w.f.c.b <ButtonRelease-1> "$self timesliderrelease $w.f.c %x %y"
        bind $w.f.c.b <B1-Motion> "$self timeslidermotion $w.f.c %x %y"
        bind $w.f.c <Configure> "$self timeticks $w.f.c"
}

TimesliderView instproc timeticks { wfc } {

        $self instvar timeslider range timeslider_width \
                        timeslider_tag mid_ 

	set timeslider(canvas) $wfc

	set width [winfo width $timeslider(canvas)]

	#        if {$width == $timeslider_width($wfc)} { return }
	set mintime [$mid_ getmintime]
	set maxtime [$mid_ getmaxtime]

        set timeslider(width) $width
        set timeslider_width($wfc) $width
        $timeslider(canvas) delete ticks

        #we really shouldn't need to do this but there's a redraw bug in the
        #tk canvas that we need to work around (at least in tk4.2) - mjh
        pack forget $timeslider(canvas)
        update
        update idletasks
        pack $timeslider(canvas) -side left -fill x -expand 1 -padx 0 -pady 0

        set width [winfo width $wfc]

        # We need a more adaptive way to draw the ticks. Otherwise for long
        # and sparse simulations, it'll result in long startup time - haoboy
        set x [expr $timeslider(swidth)/2]
        # Unit of time represented by one pixel

        set tickIncr [expr $range / ($width-$timeslider(swidth))]
	# Should check if range is 0
	if {$range == 0} {
		set intertick [expr ($width - $timeslider(swidth)) / 10]
	} else {
		set intertick [expr ($width-$timeslider(swidth))/(10 * $range)]
	}

        if {$intertick < 2} {
                # This increment should be at least 2 pixel
                set intertick 2
        }

        for {set t $mintime} {$t < ($range+$mintime)} {set t [expr $t+$intertick*$tickIncr]} {
                set intx [expr int($x)]
                $timeslider(canvas) addtag ticks withtag \
                        [$timeslider(canvas) create line \
                        $intx [expr $timeslider(height)/2] \
                        $intx $timeslider(height)]
                set x [expr $x+$intertick]
        }
        set x [expr $timeslider(swidth)/2]
	if {$range == 0} {
		set intertick [expr $width - $timeslider(swidth)]
	} else {
		set intertick [expr ($width-$timeslider(swidth))/($range)]
	}
        if {$intertick < 20} {
                set intertick 20
        }
        for {set t $mintime} {$t < ($range+$mintime)} {set t [expr $t+$intertick*$tickIncr]} {
                set intx [expr int($x)]
                $timeslider(canvas) addtag ticks withtag \
                                [$timeslider(canvas) create line \
                                $intx 0 $intx $timeslider(height)]
                set x [expr $x+$intertick]
        }

	# set arrow
	$timeslider(canvas) delete arrow
	set pmode [$mid_ getpipemode]
	if {$pmode == 1 } {
	    set x [expr $width-$timeslider(swidth)/2]
	    set y 0

	    $timeslider(canvas) addtag arrow withtag \
	    [$timeslider(canvas) create polygon $x $y \
	    [expr $x+$timeslider(swidth)/2] [expr $y+$timeslider(height)/2] \
	    $x [expr $y+$timeslider(height)]]
	}

	set wfc_width [winfo width $wfc]
	set now [$mid_ getcurrenttime]
	if {$maxtime > 0} {
		set x [expr ($wfc_width-$timeslider(swidth))*$now/$maxtime]
	} else {
		set x [expr ($wfc_width-$timeslider(swidth))*$now]
	}
	$wfc coords $timeslider_tag($wfc) [expr $x + $timeslider(swidth)/2] 0
}


TimesliderView instproc timesliderpress {w x y} {

        $self instvar timeslider mid_

        set timeslider(oldpos) $x
        $w.b configure -relief sunken

	set animator [$mid_ getanimator]

	$animator setsliderPressed 1
}   


TimesliderView instproc timeslidertrough {w x y} {
        $self instvar timeslider sliderPressed timeslider_pos mid_
	set animator [$mid_ getanimator]
	
        if {$timeslider_pos($w)>$x} {
                $animator rewind
        } else {
                $animator fast_fwd
    }
}

TimesliderView instproc timeslidermotion {wc x y} {
        $self instvar timeslider range timeslider_tag \
                        timeslider_pos timeslider_width mid_

    set mintime [$mid_ getmintime]

    set diff [expr $x - $timeslider(oldpos)]

    set timeslider(canvas) $wc

    $timeslider(canvas) move $timeslider_tag($wc) \
                $diff 0

    set timeslider_pos($wc) [expr $timeslider_pos($wc) + $diff]
    if {$timeslider_pos($wc)<0} {
        $timeslider(canvas) move $timeslider_tag($wc) \
                [expr 0 - $timeslider_pos($wc)] 0
        set timeslider_pos($wc) 0
    }
    if {$timeslider_pos($wc)>[expr $timeslider_width($wc)-$timeslider(swidth)]} {
        $timeslider(canvas) move $timeslider_tag($wc) \
                [expr ($timeslider_width($wc)-$timeslider(swidth))-$timeslider_pos($wc)] 0
        set timeslider_pos($wc) [expr $timeslider_width($wc)-$timeslider(swidth)]
    }
    set tick 0
    catch {
        set tick [expr 1000.0*$timeslider_pos($wc)/($timeslider_width($wc)-$timeslider(swidth))]
    }
    set now [expr ($tick * $range) / 1000. + $mintime]
    set timeslider(tick) $tick

#    set nowDisp [format %.6f $now]
    set animator [$mid_ getanimator]
    $animator setDisp [format %.6f $now]

    if {$range <= 0} {
	    $self timesliderset [expr ($now - $mintime) * 1000]
    } else {
	    $self timesliderset [expr int(1000. * ($now - $mintime) / $range)]
    }
}

TimesliderView instproc timesliderrelease {w x y} {

        $self instvar timeslider running mid_
	set animator [$mid_ getanimator]

        $self timeslidermotion $w $x $y
        $w.b configure -relief raised
        $animator slidetime $timeslider(tick) 1
        $animator setsliderPressed 0
	
        if [$animator getrunning] {
                $animator renderFrame
        }
}

TimesliderView instproc update { timeset } {
	$self instvar timeslider range mid_
		
 	set time [lindex $timeset 0]
	set type [lindex $timeset 1]	

	if {[string compare $type "now"] == 0} {
	    $self timesliderset $time 
	}

	if {[string compare $type "min"] == 0} {
	    $self timesliderset $time 
	}

	if {[string compare $type "max"] == 0} {
	    set mintime [$mid_ getmintime]
	    set range [expr $time - $mintime]
	    $self timeticks $timeslider(canvas) 
	}

}

TimesliderView instproc timesliderset {t} {

        $self instvar timeslider timeslider_width timeslider_tag \
                        timeslider_pos 
                        
                if {[winfo exists $timeslider(canvas)]==1} {
                        set timeslider(width) \
                                $timeslider_width($timeslider(canvas))
                        set x [expr ($timeslider(width)-$timeslider(swidth))*$t/1000.0]
                        $timeslider(canvas) coords \
                                $timeslider_tag($timeslider(canvas)) \
                                [expr $x + $timeslider(swidth)/2] 0
                        set timeslider_pos($timeslider(canvas)) $x

    		}
}















