#
# Copyright (C) 2001 by USC/ISI
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
# 

# Editor.tcl
#   - the nam editor

Class Editor 

Editor set uniqueID_ 0

Editor proc getid {} {
  set id [Editor set uniqueID_]
  Editor set uniqueID_ [expr $id + 1]
  return $id
}

#----------------------------------------------------------------------
# Called when creating a new editor from nam console
#   from anim-ctrl.tcl:on-new 
#           set editor [new Editor ""]
#----------------------------------------------------------------------
Editor instproc init {tracefile} {
  $self instvar menulist_ SharedEnv id_ editorwindow_ \
                editor_view_ balloon_ menubar_ save_filename_ \
                agent_types_ source_types_ \
                toolbar_buttons_ current_tool_ netmodel_ mintime_ \
                maxtime_ now_ mslider_ vslider_ pipemode_ current_time_
  $self tkvar current_agent_ current_time_ current_source_

  set current_time_ 0.0

  set id_ [Editor getid]
  toplevel .nameditor-$id_
  set editorwindow_ .nameditor-$id_
  if {$tracefile == ""} {
    wm title $editorwindow_ "NAM Editor - $id_"
  } else {
    wm title $editorwindow_ "NAM Editor - $tracefile"
  }

  #set agent_types_ {Null TCP TCP/FullTCP TCP/Reno TCP/NewReno \
  #                 TCP/Vegas TCP/Sack1 TCP/Fack TCPSink \
  #                 TCPSink/DelAck TCPSink/Sack1 UDP}
  set agent_types_ {TCP TCP/Reno TCP/NewReno \
                   TCP/Vegas TCP/Sack1 TCP/Fack UDP - Null TCPSink \
                   TCPSink/DelAck TCPSink/Sack1}
  set current_agent_ UDP
  
  # There is also a "Trace" traffic source which reads
  # traffic information from a file
#  set source_types_ {CBR Exponential FTP Pareto Telnet} 
  set source_types_ {CBR FTP} 
  set current_source_ CBR

  set SharedEnv(CurrentPageId) 1
  set SharedEnv(CurrentCanvas) "" 
  set SharedEnv(PageWidth) 8.5i
  set SharedEnv(PageHeight) 11i
  set SharedEnv(PageVisX) 3i
  set SharedEnv(PageVisY) 2i 
  set SharedEnv(SlideBG) white
  set SharedEnv(Landscape) 0 
  set SharedEnv(ColorWidth) 400      
  set SharedEnv(ColorHeight) 100
  set SharedEnv(PrivateCmap) ""
  set SharedEnv(ButtonOrient) left
  set SharedEnv(ButtonCount)  10
  set SharedEnv(ButtonSize) .35i
  set SharedEnv(ButtonRelief) flat
  set SharedEnv(ButtonStipple1) gray50
  set SharedEnv(ButtonStipple2) {}
  set SharedEnv(ButtonColor) cyan
  set SharedEnv(Fill) "" 
  set SharedEnv(MenuSelect) 0

  button .b
  set SharedEnv(Background) [.b cget -bg]
  set SharedEnv(ActiveBackground) [.b cget -activebackground]
  destroy .b

  set SharedEnv(DefButtonColor) [. cget -bg]
  set SharedEnv(Cut) ""
  set SharedEnv(Copy) ""
  set SharedEnv(Message) "Select"
  set SharedEnv(Xpos) 0
  set SharedEnv(Ypos) 0
  set SharedEnv(Smooth) 0
  set SharedEnv(Outline) black
  set SharedEnv(Stipple) ""
  set SharedEnv(Arrow) none
  set SharedEnv(JoinStyle) miter 
  set SharedEnv(Grid) 0
  set SharedEnv(GridX) .25i
  set SharedEnv(GridY) .25i
  set SharedEnv(UnitX) i
  set SharedEnv(UnitY) i
  set SharedEnv(ScreenH) [winfo screenheight .]
  set SharedEnv(ScreenW) [winfo screenwidth .]
  set SharedEnv(Gravity) 1
  set SharedEnv(GravityVal) 30
  set SharedEnv(Sub-Cursor) ""
  
  set SharedEnv(Start) ""
  set SharedEnv(top_left_corner) bottom_right_corner
  set SharedEnv(top_side) bottom_side
  set SharedEnv(top_right_corner) bottom_left_corner
  set SharedEnv(right_side) left_side
  set SharedEnv(bottom_right_corner) top_left_corner
  set SharedEnv(bottom_side) top_side
  
  set SharedEnv(bottom_left_corner) top_right_corner
  set SharedEnv(left_side) right_side
  set SharedEnv(FixedAspect) True

  # Intialize balloon help
  set balloon_ [new BalloonHelp $editorwindow_]

  if {$tracefile == ""} {
    set save_filename_ ""
  } else {
    set save_filename_ $tracefile
  }

  # Create menubar at the top of the window
  frame $editorwindow_.menubar_ -relief raised -bd 2
  $self buildMenubar $editorwindow_.menubar_
  pack $editorwindow_.menubar_ -side top -fill x

  # Creates Editor viewport with scrollbars and zoom controls
  # Attaches that view to a network model
  # -- This has to be created first because the toolbar needs the
  #    editor_view to be setup in advance.
  frame $editorwindow_.view
  $self buildEditorView $editorwindow_.view

  # TimeControls
  frame $editorwindow_.timecontrols -borderwidth 2 -relief groove
  $self buildTimeControls $editorwindow_.timecontrols

  # Toolbar
  frame $editorwindow_.tools -borderwidth 2 -relief groove
  $self buildToolbar $editor_view_ $editorwindow_.tools

  # TimeSlider
  set mintime_ 0.0
  set maxtime_ 60.0
  set now_ 0.0
  frame $editorwindow_.timeslider -relief flat -borderwidth 0
  $self buildTimeSlider $editorwindow_.timeslider
  

  pack $editorwindow_.tools $editorwindow_.timecontrols -side top -fill x
  pack $editorwindow_.timeslider -side bottom -fill x 
  pack $editorwindow_.view -side left -fill both -expand true

  # Object Properties Window
  #frame $editorwindow_.properties
  #pack $editorwindow_.properties -side left -fill y

  bind $editorwindow_ <q> "$self done"
  bind $editorwindow_ <Q> "$self done"

  $self setCursor Select

  # Initial state: select
  $self chooseAgent UDP
  $self chooseTrafficSource CBR 
  $self chooseTool select

#  $editor_view_ zoom 1.6

  # Load a nam editor ns script
  if {$tracefile != ""} {
    #$self setmodelrange $tracefile
    $self loadNsScript $tracefile
  }
}

#----------------------------------------------------------------------
# Cleanup variables that we have new-ed.
#----------------------------------------------------------------------
Editor instproc destroy {} {
  $self instvar balloon_
  delete $balloon_
  delete $mslider_
  delete $vslider_
}

#----------------------------------------------------------------------
#
#----------------------------------------------------------------------
Editor instproc done {} {
  $self instvar editorwindow_
  destroy $editorwindow_
  delete $self
}

#----------------------------------------------------------------------
#  VERIFY THAT THIS FUNCTION IS NOT USED AND THE REMOVE IT
#----------------------------------------------------------------------
Editor instproc setmodelrange {tracefile} {
  #filter the tcl file into a enam file first
  set stream [new NamStream $tracefile]
  set tmpfId [open /tmp/foo.enam w 0666]

  while {[$stream eof] == 0} {
    set line [$stream gets]
    set mark [string range $line 0 1]
    set mark2 [string range $line 2 2]
    if {[string compare $mark "##"]==0 && [string compare $mark2 " "]!=0} {
      puts $tmpfId [string range $line 2 [expr [string length $line] - 2]]
    }
  }
  close $tmpfId

  set stream [new NamStream $tracefile]
  set time "0"

  while {([$stream eof]==0)&&([string compare $time "*"]!=0)} {
    set line [$stream gets] 
    set time [get_trace_item "-t" $line]
  }

  while {([$stream eof]==0)&&([string compare $time "*"]==0)} {
    set cmd [lindex $line 0]
    set time [get_trace_item "-t" $line]
    if {[string compare $time "*"]!=0} {break}
    switch $cmd {
      "W" {
        set minx [get_trace_item "-x" $line]
        set miny [get_trace_item "-y" $line]
        set maxx [get_trace_item "-X" $line]
        set maxy [get_trace_item "-Y" $line]
        NetworkModel/Editor set Wpxmin_ $minx
        NetworkModel/Editor set Wpymin_ $miny
        NetworkModel/Editor set Wpxmax_ $maxx
        NetworkModel/Editor set Wpymax_ $maxy
        break
      }
    }
    set line [$stream gets]
  }   
  $stream close
}

#----------------------------------------------------------------------
# Editor instproc buildEditorView {w}
#  - Creates main canvas with scrollbars to which the user can add
#    ns objects
#----------------------------------------------------------------------
Editor instproc buildEditorView {w} {
  $self instvar id_ SharedEnv netmodel_ \
        editor_view_ editorwindow_ balloon_ magnification viewOffset

  # Create a new model to hold the editor's objects.
  # Don't include a tracefile to signify this is 
  # the creation of a nam editor window
  set netmodel_ [new NetworkModel/Editor $self " "]


  # Create Window Layout
  frame $w.view
  #frame is just to sink the editor_view_
  frame $w.view.sunk -borderwidth 2 -relief sunken

  $netmodel_ view $w.view.sunk.net
  set editor_view_ $w.view.sunk.net
#    lappend netViews $editor_view_
  set SharedEnv(CurrentCanvas) $editor_view_
  pack $w.view.sunk.net -side top -expand true -fill both

  # X scroll bar
  $editor_view_ xscroll $w.view.hsb
  scrollbar $w.view.hsb -orient horizontal -width 10 -borderwidth 1 \
                        -command "$editor_view_ xview"
  $w.view.hsb set 0.0 1.0
  pack $w.view.hsb -side bottom -fill x
  pack $w.view.sunk -side top -fill both -expand true 

  # Y scroll bar
  frame $w.yscroll
  $editor_view_ yscroll $w.yscroll.vsb
  scrollbar $w.yscroll.vsb -orient vertical -width 10 -borderwidth 1 \
                           -command "$editor_view_ yview"
  $w.yscroll.vsb set 0.0 1.0
  pack $w.yscroll.vsb -side top -fill y -expand true

  # Scrollbar spacer
  frame $w.yscroll.l -width 12 -height -12
  pack $w.yscroll.l -side top -ipady 6 

  set view $editor_view_
  
  # Zoom bar
  set magnification 1.0
  set viewOffset(x) 0.0
  set viewOffset(y) 0.0

  frame $w.zoom -borderwidth 2 -relief groove
  frame $w.zoom.bar 
  pack $w.zoom.bar -side top

  # Put the zoom bar on the left
  button $w.zoom.bar.zoomin -bitmap "zoomin" \
                                   -command "$view zoom 1.6" \
                                   -highlightthickness 0 \
                                   -borderwidth 1

  button $w.zoom.bar.zoomout -bitmap "zoomout" \
                                    -command "$view zoom 0.625" \
                                    -highlightthickness 0 \
                                    -borderwidth 1
  pack $w.zoom.bar.zoomin \
       $w.zoom.bar.zoomout \
       -side top

  $balloon_ balloon_for $w.zoom.bar.zoomin "Zoom In"
  $balloon_ balloon_for $w.zoom.bar.zoomout "Zoom Out"

  
  pack $w.zoom -side left -fill y
  pack $w.yscroll -side right -fill y
  pack $w.view -side left -fill both -expand true

  $self viewBind $editor_view_
}

#----------------------------------------------------------------------
# Editor instproc viewBind {netView}
#   - Setup mouse button bindings
#----------------------------------------------------------------------
Editor instproc viewBind {netView} {
  $self instvar SharedEnv 

  bind $netView <Button-1>        "$self handleLeftButtonClick \%x \%y"
  bind $netView <Button-3>        "$self handleRightButtonClick \%x \%y"
  bind $netView <Any-B1-Motion>   "$self handleLeftButtonDrag \%x \%y"
  bind $netView <ButtonRelease-1> "$self handleLeftButtonRelease \%x \%y"
  bind $netView <Motion>          "$self handleMouseMovement \%x \%y"
  bind $netView <Double-Button-1> "$self handleLeftButtonDoubleClick \%x \%y"

#       bind $netView <ButtonPress-3> "$self start_info $netView \%x \%y right"
#       bind $netView <ButtonPress-2> "$self view_drag_start $netView \%x \%y"
#       bind $netView <B2-Motion> "$self view_drag_motion $netView \%x \%y"
#       bind $netView <ButtonRelease-3> "$self end_info $netView"
}

#----------------------------------------------------------------------
#
#----------------------------------------------------------------------
Editor instproc handleRightButtonClick {x y} {
  $self instvar current_tool_

  set old_tool $current_tool_
  $self chooseTool select 
  $self handleLeftButtonDoubleClick $x $y
  $self chooseTool $old_tool
}

#----------------------------------------------------------------------
#
#----------------------------------------------------------------------
Editor instproc handleLeftButtonDoubleClick {x y} {
  $self instvar SharedEnv editor_view_ current_tool_ editorwindow_
    
  switch $current_tool_ {
    select {
      set objinfo [$editor_view_ getObjectProperty $x $y]
      if {[string compare $objinfo "NONE"] == 0 } {
        return 
      }

      set item [lindex $objinfo 0]
      set item [lindex $item 0]
      $self displayObjectProperties $editorwindow_.properties $item $objinfo
      place_frame $editorwindow_ $editorwindow_.properties $x $y
    }
  }
}

#----------------------------------------------------------------------
# Editor instproc handleMouseMovement {x y}
#----------------------------------------------------------------------
Editor instproc handleMouseMovement {x y} {
  $self instvar SharedEnv editor_view_ current_tool_

  switch $current_tool_ {
    deleteobject -
    select {
      set objinfo [$editor_view_ getObject $x $y]
      if {[string compare $objinfo "NONE"] !=0} {
        $self enterObject $editor_view_ $x $y $objinfo
      } else {
        catch {destroy $editor_view_.f}
      }
    }
  }
}

#----------------------------------------------------------------------
# Editor instproc handleLeftButtonClick {x y}
#     - called when mouse button 1 is pressed
#     - switches over to EditView::command(..) in editview.cc
#----------------------------------------------------------------------
Editor instproc handleLeftButtonClick {x y} {
  $self instvar SharedEnv editor_view_ current_tool_
  $self tkvar current_agent_ current_source_
                
  switch $current_tool_ {
    # These functions are in editview.cc: command()
    select      {$editor_view_ setPoint $x $y 0}
    addnode     {$editor_view_ addNode $x $y}
    addlink     {$editor_view_ addLink $x $y}
    addagent    {$editor_view_ addAgent $x $y $current_agent_}
    addtrafficsource {$editor_view_ addTrafficSource $x $y $current_source_}
    deleteobject { $editor_view_ deleteObject $x $y }
  }
}

#----------------------------------------------------------------------
#
#----------------------------------------------------------------------
Editor instproc handleLeftButtonRelease {x y} {
  $self instvar SharedEnv editor_view_ current_tool_

#  switch $SharedEnv(Cursor) {
  switch $current_tool_ {
#    Select {
    select {
      $editor_view_ relPoint $x $y
    }
#    Link   {
    addlink   {
      $editor_view_ relPoint $x $y
    }
  }
}

#----------------------------------------------------------------------
#
#----------------------------------------------------------------------
Editor instproc handleLeftButtonDrag {x y} {
  $self instvar SharedEnv editor_view_ current_tool_

  switch $current_tool_ {
    select {
      $editor_view_ moveTo $x $y
    }
    addlink   {
      $editor_view_ moveTo $x $y 
    }
  }
}


#----------------------------------------------------------------------
#
#----------------------------------------------------------------------
Editor instproc showpalette {property_variable} {
    $self createColorPalette .colorpalette \
        {0000 3300 6600 9900 CC00 FF00} \
        {0000 3300 6600 9900 CC00 FF00} \
        {0000 3300 6600 9900 CC00 FF00} \
        .colorsp
}

#----------------------------------------------------------------------
#----------------------------------------------------------------------
Editor instproc addTimeListItem {time_menu textbox} {
  $self instvar start_stop_time_list_
  $self tkvar start_stop_time_ 
 
  # Grab the time value from the grab textbox
  set value [$textbox get 0.0 1.end]


  set duplicate "no"
  foreach time $start_stop_time_list_ {
    if {$value == $time} {
      set duplicate "yes"
    }
  }

  if {$duplicate != "yes"} {
    # Create a new sorted list with the new time value added
    set add_list [lsort -real [linsert $start_stop_time_list_ 0 $value]]

    # generate updated menu and update start_stop_time_list_
    $self generateTimeListMenu $time_menu $add_list

    # Set shown menu value to be the newly added value
    set start_stop "start"
    foreach time $start_stop_time_list_ {
      if {$value == $time} {
        set start_stop_time_ "$start_stop $time"
      }

      if {$start_stop == "start"} {
        set start_stop "stop"
      } else {
        set start_stop "start"
      }
    }
  }
}
#----------------------------------------------------------------------
#----------------------------------------------------------------------
Editor instproc deleteTimeListItem {time_menu} {
  $self instvar start_stop_time_list_
  $self tkvar start_stop_time_ 

  # Grab only the time value from the menu which is after the
  # start/stop label
  set time [lindex $start_stop_time_ 1]
  # Find the index in the time list of this time value
  # so that it can be removed using lreplace
  set index [lsearch $start_stop_time_list_ $time]

  # Remove time value and generate new menu
  $self generateTimeListMenu $time_menu \
                         [lreplace $start_stop_time_list_ $index $index]

  # Update the menu to show the first time item which 
  # will always be a start value
  set start_stop_time_ "start [lindex $start_stop_time_list_ 0]"
}
  

#----------------------------------------------------------------------
#----------------------------------------------------------------------
Editor instproc generateTimeListMenu {list_menu list_values} {
 $self instvar start_stop_time_list_
  
 # Remove old menu
 $list_menu delete 0 end
 
 # Create list menu
 set start_stop "start"
 foreach time_value $list_values {
   $list_menu add radiobutton -label "$start_stop $time_value" \
                 -variable start_stop_time_ \
                 -command "set [$self tkvarname start_stop_time_] \
                           \"$start_stop $time_value\""
   if {$start_stop == "start"} {
     set start_stop "stop"
   } else {
     set start_stop "start"
   }
 }

 set start_stop_time_list_ $list_values
}
  
#----------------------------------------------------------------------
# Editor instproc
# displayObjectProperties {display_frame item properties}
#  - A draws the list of properties for an editor object.
#  - The format of the "properties" list is
#    {label modified_variable property_type}
#  - update_object contains a list of each property and it's stored
#    value
#----------------------------------------------------------------------
Editor instproc displayObjectProperties {display_frame item properties} {
  $self instvar SharedEnv colorarea property_values textwidgets
  $self tkvar current_time_ start_stop_time_

  set SharedEnv($item) $properties

  $self undisplayObjectProperties $display_frame
  frame $display_frame -relief raised -borderwidth 1 

  set number 0
  
  foreach property $properties {
    set property_label [lindex $property 0]
    set property_variable [lindex $property 1]
    set property_type [lindex $property 2]
    set property_value [lindex $property 3]
    
    set property_values($property_variable) $property_value 

    # Create frame to hold each property line
    frame $display_frame.line_$number

    if {$property_type == "title"} {
      # Displays the title in the property window frame
      label $display_frame.line_$number.title -wraplength 200 \
                                              -text $property_label
      pack $display_frame.line_$number.title -side top

    } elseif {$property_type == "text"} { 
      # Otherwise just put a label
      label $display_frame.line_$number.label_${property_variable} \
           -wraplength 100 -text "$property_label"
      text $display_frame.line_$number.input_${property_variable} \
           -width 10 -height 1
      set textwidgets($property_variable) \
          $display_frame.line_$number.input_${property_variable}

      # Set text box to current variable value
      $display_frame.line_$number.input_${property_variable} insert \
            0.0 $property_value

      pack $display_frame.line_$number.label_${property_variable} -side left
      pack $display_frame.line_$number.input_${property_variable} -side right

    } elseif {$property_type == "color"} {
      # We have a color property
      button $display_frame.line_$number.label_${property_variable} \
             -text "$property_label" -command "$self showpalette $display_frame.line_$number.input_${property_variable}"
      text $display_frame.line_$number.input_${property_variable} \
           -width 10 -height 1
      set textwidgets($property_variable) \
          $display_frame.line_$number.input_${property_variable}

      $display_frame.line_$number.input_${property_variable} insert \
            0.0 $property_value

      pack $display_frame.line_$number.label_${property_variable} -side left
      pack $display_frame.line_$number.input_${property_variable} -side right

      #set colorarea $display_frame.line_$number.eCOLOR
      set colorarea $textwidgets($property_variable)
      pack $display_frame.line_$number.input_${property_variable} -side right


    } elseif {$property_type == "label"} {
      label $display_frame.line_$number.label_${property_variable} \
           -wraplength 100 -text "$property_label"
      pack $display_frame.line_$number.label_${property_variable} -side left


    } elseif {$property_type == "timelist"} {
      # Displays a complex time list object with
      # Buttons for grabbing the current time, add the time to the 
      # start/stop time list and removing time from the list.
      # It should look like this.
      # [Grab]  ___________  [Add] [Remove] [- time list menu]
      label $display_frame.line_$number.label_${property_variable} \
           -wraplength 100 -text "$property_label"

      button $display_frame.line_$number.set_${property_variable} \
             -text "Grab" \
             -command "$self insertTimeInto \
              $display_frame.line_$number.input_${property_variable}"

      text $display_frame.line_$number.input_${property_variable} \
           -width 10 -height 1
      $display_frame.line_$number.input_${property_variable} insert \
            0.0 [lindex $property_value 0]

      set time_menu $display_frame.line_$number.timelist_${property_variable}.menu

      # Add Remove list widgets
      button $display_frame.line_$number.add_${property_variable} \
             -text "Add" -command "$self addTimeListItem $time_menu \
              $display_frame.line_$number.input_${property_variable}"

      # Add a drop down list of start and stop times that are passed in
      # from the object in the property_value as a list
      set start_stop_time_ " "
      menubutton $display_frame.line_$number.timelist_${property_variable} \
                 -textvariable [$self tkvarname start_stop_time_] \
                 -text $start_stop_time_ \
                 -indicatoron 1 -menu $time_menu \
                 -relief raised -bd 2 -highlightthickness 2 -anchor c \
                 -direction flush -width 15
      
      menu $time_menu -tearoff 0
      $self generateTimeListMenu $time_menu $property_value
      set start_stop_time_ "start [lindex $property_value 0]"
      
#      label $display_frame.line_$number.label_${property_variable} \
#           -text "$property_label"

      button $display_frame.line_$number.remove_${property_variable} \
        -text "Remove" -command "$self deleteTimeListItem $time_menu"

      pack $display_frame.line_$number.label_${property_variable} -side left
      pack $display_frame.line_$number.set_${property_variable} \
           $display_frame.line_$number.input_${property_variable} -side left
      pack $display_frame.line_$number.remove_${property_variable} \
           $display_frame.line_$number.timelist_${property_variable} \
           $display_frame.line_$number.add_${property_variable} -side right

    } elseif {$property_type == "time"} {
      # Display a button and text box
      #  - the button grabs the time from current_time_
      #  - the text box allows the time to be set expicitly
      text $display_frame.line_$number.input_${property_variable} \
           -width 10 -height 1
      set textwidgets($property_variable) \
          $display_frame.line_$number.input_${property_variable}

      button $display_frame.line_$number.label_${property_variable} \
        -text "$property_label" \
        -command "$self insertTimeInto \
                        $display_frame.line_$number.input_${property_variable}"

      #label $display_frame.line_$number.label_${property_variable} \
      #     -text "$property_label"

      # Set text box to current variable value
      $display_frame.line_$number.input_${property_variable} insert \
            0.0 $property_value

      pack $display_frame.line_$number.label_${property_variable} -side left
      pack $display_frame.line_$number.input_${property_variable} -side right
    }

    pack $display_frame.line_$number -side top -fill x 
    incr number
  } ;# End foreach

  
  frame $display_frame.buttons
  button $display_frame.buttons.apply -text "Apply"\
         -command "$self setObjectProperties {$properties}; \
          $self undisplayObjectProperties $display_frame"
  button $display_frame.buttons.cancel -text "Cancel" \
         -command "$self undisplayObjectProperties $display_frame"

  pack $display_frame.buttons.apply $display_frame.buttons.cancel -side left \
       -padx 1m -pady 1m
  pack $display_frame.buttons -side bottom -padx 1m -pady 1m
}


#----------------------------------------------------------------------
#
#----------------------------------------------------------------------
Editor instproc insertTimeInto {textbox} {
  $self tkvar current_time_    
  $textbox delete 0.0 end
  $textbox insert 0.0 $current_time_
  #puts $current_time_
}

#----------------------------------------------------------------------
#
#----------------------------------------------------------------------
Editor instproc undisplayObjectProperties {display_frame} {
  if {[winfo exists $display_frame]} {
    # Need to delete old properties widgets
    foreach widget [pack slaves $display_frame] {
      # Each property has a frame for it's line(ie. label textbox)
      set subwidgets [pack slaves $widget]
      if {[llength $subwidgets] > 0} {
        pack forget $subwidgets
        destroy $subwidgets
      }
      pack forget $widget
      destroy $widget
    }
  }
  catch {destroy $display_frame}
}

#-----------------------------------------------------------------------
# Editor instproc setObjectProperties {properties}
#  - takes property list and sets each property in the list to it's
#    value
#  - properties format is:
#    {{label variable type value} {label variable type value}}
#-----------------------------------------------------------------------
Editor instproc setObjectProperties {properties} {
  $self instvar editor_view_ property_values textwidgets \
                start_stop_time_list_

  foreach property $properties {
  #  puts $property
    
    set property_label [lindex $property 0]
    set property_variable [lindex $property 1]
    set property_type [lindex $property 2]

    if {$property_type == "title"} {
      # This should be the first property and property_value should
      # include the object type and id
      set property_value [lindex $property 3]
      set type [lindex $property_value 0]
      set typeid [lindex $property_value 1] 

    } elseif {$property_type == "label"} {
      # Do nothing for a label

    } elseif {$property_type == "timelist"} {
      set property_value $start_stop_time_list_

    } else {
      # Get the value from the text box
      set property_value [$textwidgets($property_variable) get 0.0 1.end]
      #puts [$textwidgets($property_variable) get 0.0 1.end]
    }

    switch "$type" {
      NODE {
        switch "$property_variable" {
          size_ {
            $editor_view_ setNodeProperty $typeid $property_value 0
          }
          COLOR {
            $editor_view_ setNodeProperty $typeid $property_value 1
          }
          dlabel_ {
            $editor_view_ setNodeProperty $typeid $property_value 2
          }
        }
      }

      AGENT {
        switch "$property_variable" {
          windowInit_ {
            $editor_view_ setAgentProperty $typeid $property_value 0
          }
          window_ {
            $editor_view_ setAgentProperty $typeid $property_value 1
          }
          maxcwnd_ {
            $editor_view_ setAgentProperty $typeid $property_value 2
          }
#          TRACEVAR {
#            $editor_view_ setAgentProperty $typeid $property_value 3
#          }
#          PRODUCEMORE {
#            $editor_view_ setAgentProperty $typeid $property_value 5
#          }
          flowcolor_ {
            $editor_view_ setAgentProperty $typeid $property_value 7
          }
          packetSize_ {
            $editor_view_ setAgentProperty $typeid $property_value 8
          }
        }
      }

      LINK {
        set src_dst [split $typeid "-"]
        set src [lindex $src_dst 0]
        set dst [lindex $src_dst 1]

        switch "$property_variable" {
          bandwidth_ {
            $editor_view_ setLinkProperty $src $dst $property_value 0
          }
          COLOR {
            $editor_view_ setLinkProperty $src $dst $property_value 1
          }
          delay_ {
            $editor_view_ setLinkProperty $src $dst $property_value 2
          }
          dlabel {
            $editor_view_ setLinkProperty $src $dst $property_value 3
          }
        }
      }

      TrafficSource {
        $editor_view_ setTrafficSourceProperty $typeid \
                                               $property_value \
                                               $property_variable

      }
    }
  }
}


#----------------------------------------------------------------------
#
#----------------------------------------------------------------------
Editor instproc propRequestor {name item attr} {
        $self instvar SharedEnv
     
        set w ${name}
     
        toplevel $w
        wm transient $w .
        wm protocol $w WM_DELETE_WINDOW {set dummy 1}
  
  frame $w.p
  $self displayObjectProperties $name $item $attr
     
#        frame $w.f -borderwidth 0 -highlightthickness 0
#        frame $w.p
#        frame $w.fsel -borderwidth 0 -highlightthickness 0
#        frame $w.texttop -borderwidth 0 -highlightthickness 0
#        frame $w.textbottom -relief raised -bd 1
#        canvas $w.c -width 1.5i -height 1.5i -relief sunken -bd 1 -bg white
#        label $w.l
#        scale $w.s -orient horiz -resolution 1 -from 1 -to 1 \
#        -variable SharedEnv(PropItem) -bd 2 -relief raised -highlightthickness 0 \
#        -width 8 -showvalue true
     
#        button $w.ok -text "OK" -command "$self propLower $w"
#        button $w.cancel -text "Cancel" -command "$self propLower $w"
#        button $w.apply -text "Apply" \
#                -command "$self propLower $w"
        bind $w <Key-Escape> "$self propLower $w"
     
#        pack $w.c -in $w.texttop
#        pack $w.l -in $w.texttop -fill x
#        pack $w.s -in $w.texttop -fill x
#        pack $w.ok $w.cancel $w.apply -in $w.textbottom -side left -padx 1m -pady 1m
#        pack $w.texttop -anchor sw -in $w.fsel
#        pack $w.textbottom -anchor sw -in $w.p -fill x
        pack $w.p -in $w -anchor sw -side left
     
        wm withdraw $w
        return $w
}    

#----------------------------------------------------------------------
#
#----------------------------------------------------------------------
Editor instproc explodeList {menu list parentmenu} {
        $self instvar menu_buttons SharedEnv editorwindow_

        set menuname [lindex $list 0]
        set menutitle [lindex $list 1]
        set menuopts [lindex $list 2]
        set menucmd [lindex $list 3]
        set newlist [lindex $list 4]

        # Menubutton
        if {"${parentmenu}" == "${menu}"} {
        menubutton ${parentmenu}.${menuname}_b -text "$menutitle" \
                        -menu ${parentmenu}.${menuname}_b.${menuname} -padx 4m
                if { "$menuopts" != ""} {
                        eval ${parentmenu}.${menuname}_b configure $menuopts
                }
                pack ${parentmenu}.${menuname}_b -side left -fill x
        menu ${parentmenu}.${menuname}_b.${menuname}
                lappend menu_buttons ${parentmenu}.${menuname}_b
                set menuname ${menuname}_b.${menuname}
        # MenuItem
        } elseif {"$newlist" == ""} {
                if {"${menutitle}" == ""} {
                        eval ${parentmenu} add separator ${menuopts}
                } else {
                        eval ${parentmenu} add command -label \"${menutitle}\" \
                                -command \"${menucmd}\" ${menuopts}
                } 
        # MenuCascade (subMenu)
        } else {
                eval ${parentmenu} add cascade -label \"$menutitle\" \
                        -menu ${parentmenu}.${menuname} ${menuopts}
                menu ${parentmenu}.${menuname}
        }
        set len [llength "$newlist"]
        if {$len} {
                for {set i 0} {$i < $len} {incr i} {
                        set l [lindex "$newlist" $i]
                        $self explodeList "$menu" "$l" "${parentmenu}.${menuname}"
                }
        }
        return $menu_buttons
}

#----------------------------------------------------------------------
# Editor instproc buildToolbar {view toolbar} {
#   - Toolbar along the top side of the nam editor window
#   - Tools are:
#        select
#        addnode
#        addlink
#        addagent
#        addtrafficsource
#        deleteobject
#----------------------------------------------------------------------
Editor instproc buildToolbar {view toolbar} {
  $self instvar magnification viewOffset SharedEnv balloon_ \
        state_buttons_ edit_state_ editorwindow_ agent_types_ \
        source_types_ toolbar_buttons_ current_tool_
  $self tkvar current_agent_ current_source_

  # Build toolbar
  frame $toolbar.leftside

  button $toolbar.leftside.select -bitmap "select" \
                                  -command "$self chooseTool select" \
                                  -highlightthickness 0 -borderwidth 1
  set toolbar_buttons_(select) $toolbar.leftside.select

  button $toolbar.leftside.addnode -bitmap "addnode" \
                              -command " $self chooseTool addnode" \
                              -highlightthickness 0 -borderwidth 1
  set toolbar_buttons_(addnode) $toolbar.leftside.addnode

  button $toolbar.leftside.addlink -bitmap "addlink" \
                              -command "$self chooseTool addlink" \
                              -highlightthickness 0 -borderwidth 1
  set toolbar_buttons_(addlink) $toolbar.leftside.addlink

  button $toolbar.leftside.addagent -text "Agent" \
                        -command "$self chooseTool addagent" \
                        -highlightthickness 0 -borderwidth 1
  set toolbar_buttons_(addagent) $toolbar.leftside.addagent

  # Create drop down list of agents
  #   - current_agent_ needs to be initialized to first element in
  #     $agent_types_
  menubutton $toolbar.leftside.agent_list -textvariable [$self tkvarname current_agent_] \
             -text $current_agent_ \
             -indicatoron 1 -menu $toolbar.leftside.agent_list.menu \
             -relief raised -bd 2 -highlightthickness 2 -anchor c \
             -direction flush
  menu $toolbar.leftside.agent_list.menu -tearoff 0
  foreach i $agent_types_ {
    if {$i == "-"} {
      $toolbar.leftside.agent_list.menu add separator
    } else {
      $toolbar.leftside.agent_list.menu add radiobutton -label $i \
                                      -variable current_agent_ \
                                      -command "$self chooseAgent $i" 
    }
  }

  #eval tk_optionMenu $toolbar.leftside.agent_list current_agent_ $agent_types_
  $toolbar.leftside.agent_list configure -width 16
#  bind $toolbar.leftside.agent_list.menu <Leave> [list $self chooseTool addagent]

  button $toolbar.leftside.add_traffic_source -text "Traffic Source" \
                        -command "$self chooseTool addtrafficsource" \
                        -highlightthickness 0 -borderwidth 1
  set toolbar_buttons_(addtrafficsource) $toolbar.leftside.add_traffic_source

  # Drop down list of Traffic Sources
  #   - $current_source_ needs to be initialized to first element in
  #     $source_types_
  menubutton $toolbar.leftside.source_list -textvariable [$self tkvarname current_source_] \
             -indicatoron 1 -menu $toolbar.leftside.source_list.menu \
             -relief raised -bd 2 -highlightthickness 2 -anchor c \
             -direction flush
  menu $toolbar.leftside.source_list.menu -tearoff 0
  foreach i $source_types_ {
    $toolbar.leftside.source_list.menu add radiobutton -label $i \
                                      -variable current_source_ \
                                      -command "$self chooseTrafficSource $i" 
  }
  $toolbar.leftside.source_list configure -width 16
  
  # Delete Object Button
  button $toolbar.leftside.deleteobject -bitmap "delete" \
                        -command "$self chooseTool deleteobject" \
                        -highlightthickness 0 -borderwidth 1
  set toolbar_buttons_(deleteobject) $toolbar.leftside.deleteobject

  pack $toolbar.leftside.select \
       $toolbar.leftside.addnode \
       $toolbar.leftside.addlink \
       $toolbar.leftside.addagent \
       $toolbar.leftside.agent_list \
       $toolbar.leftside.add_traffic_source \
       $toolbar.leftside.source_list \
       $toolbar.leftside.deleteobject \
       -side left

  pack $toolbar.leftside -side left 
#  pack $toolbar.zoombar -side left -fill x

  $balloon_ balloon_for $toolbar.leftside.select "Select Object, Move Object or Get Object Properties"
  $balloon_ balloon_for $toolbar.leftside.addnode "Add Node"
  $balloon_ balloon_for $toolbar.leftside.addlink "Add Link or Connect Agents"
  $balloon_ balloon_for $toolbar.leftside.addagent "Add Agent to a Node"
  $balloon_ balloon_for $toolbar.leftside.agent_list "Choose Which Agent to Add"
  $balloon_ balloon_for $toolbar.leftside.add_traffic_source "Add Traffic Source"
  $balloon_ balloon_for $toolbar.leftside.source_list "Choose Which Traffic Source to Add"
  $balloon_ balloon_for $toolbar.leftside.deleteobject "Delete Object"
}


#-----------------------------------------------------------------------
# Editor instproc buildTimeControls {w}
#  - builds the set of time controls into the frame w
#  - This code was originally copied from build-ui.tcl so it is not
#    very pretty looking 
#-----------------------------------------------------------------------
Editor instproc buildTimeControls {w} {
  $self instvar rateSlider granularity time_step_ stepDisp running \
        direction balloon_ current_time_step_ previous_time_step_
  $self tkvar current_time_

  set current_time_step_ 0.0001

  set f [smallfont]
  frame $w.bar -relief groove -borderwidth 2

#  frame $w.bar.rate -borderwidth 1 -relief sunken

#  scale $w.bar.rate.slider -orient horizontal -width 7p \
#                           -label "Step:" -font [smallfont] \
#                           -from -60 -to -1 -showvalue false \
#                           -relief flat \
#                           -borderwidth 1 -highlightthickness 0 
#                      -troughcolor [option get . background Nam]

#  pack $w.bar.rate.slider -side top -fill both -expand true
#  set rateSlider $w.bar.rate.slider

  #puts [time2real [option get . rate Nam]]
#  set granularity [option get . granularity Nam]
  #set time_step_ [time2real [option get . rate Nam]]
#  set time_step_ 10.0
  
#  set stepDisp [step_format $time_step_]
#  set current_time_step_ [expr 10*log10($time_step_)]
#  set previous_time_step_ $current_time_step_
#  $rateSlider set $current_time_step_

#  bind $rateSlider <ButtonRelease-1> "$self set_rate \[%W get\] 1"
#  bind $rateSlider <ButtonPress-1> "$self on-rateslider-press"
#  bind $rateSlider <B1-Motion> "$self on-rateslider-motion \[%W get\]"

#  trace variable stepDisp w "$self displayStep"

  # Shows the current time in the time control bar
  label $w.bar.timer_label -text "Time :" \
                        -anchor w -font $f -borderwidth 1 \
                        -relief sunken -anchor e
  label $w.bar.timer_value -textvariable [$self tkvarname current_time_] \
                        -width 14 -anchor w -font $f -borderwidth 1 \
                        -relief sunken -anchor e

#  frame $w.bar.run
#  button $w.bar.run.b -bitmap play -borderwidth 1 -relief raised \
#                      -highlightthickness 0 -font $f  \
#                      -command "puts \"$self set_run $w\""
#  frame $w.bar.run.f -height 5 -relief sunken \
#                     -borderwidth 1
#  pack $w.bar.run.b -side top -fill both -expand true
#  pack $w.bar.run.f -side top -fill x
#  $balloon_ balloon_for $w.bar.run.b "play forward" 1000

#  frame $w.bar.back
#  button $w.bar.back.b -bitmap back -borderwidth 1 -relief raised \
#                       -highlightthickness 0 -font $f \
#                       -command "puts \"$self set_back $w\""
#  $balloon_ balloon_for $w.bar.back.b "play backward" 1000

  # hilight running labels as $running changes
#  trace variable running w "$self trace_running_handler $w"
  
#  frame $w.bar.back.f -height 5 -relief sunken \
#                      -borderwidth 1
#  pack $w.bar.back.b -side top -fill both -expand true
#  pack $w.bar.back.f -side top -fill x
        
#  frame $w.bar.stop
#  button $w.bar.stop.b -bitmap stop -borderwidth 1 -relief raised \
#                       -highlightthickness 0 -font $f \
#                       -command "puts \"$self stop 1;\
#                                 $self renderFrame;\
#                                 $self highlight $w.bar.stop 1\""

#  $balloon_ balloon_for $w.bar.stop.b "stop" 1000
#  frame $w.bar.stop.f -height 5 -relief sunken \
#                      -borderwidth 1

#  pack $w.bar.stop.b -side top -fill both -expand true
#  pack $w.bar.stop.f -side top -fill x

#  frame $w.bar.rew
#  button $w.bar.rew.b -bitmap rew -borderwidth 1 -relief raised \
#                      -highlightthickness 0 -font $f \
#                      -command "$self rewind"

#  $balloon_ balloon_for $w.bar.rew.b "rewind" 1000
#  frame $w.bar.rew.f -height 5 -relief sunken \
#                     -borderwidth 1
#  pack $w.bar.rew.b -side top -fill both -expand true
#  pack $w.bar.rew.f -side top -fill x

#  frame $w.bar.ff
#  button $w.bar.ff.b -bitmap ff -borderwidth 1 -relief raised \
#                     -highlightthickness 0 -font $f \
#                     -command "$self fast_fwd"
#  $balloon_ balloon_for $w.bar.ff.b "fast forward" 1000

#  frame $w.bar.ff.f -height 5 -relief sunken \
#                    -borderwidth 1
#  pack $w.bar.ff.b -side top -fill both -expand true
#  pack $w.bar.ff.f -side top -fill x

#  pack $w.bar.rate -side right -fill y
  pack $w.bar.timer_value $w.bar.timer_label -side right -pady 0 \
                       -ipady 1 -padx 1 -fill y
#  pack $w.bar.ff -side right -padx 1 -pady 1 -fill both -expand true
#  pack $w.bar.run -side right -padx 1 -pady 1 -fill both -expand true
#  pack $w.bar.stop -side right -padx 1 -pady 1 -fill both -expand true
#  pack $w.bar.back -side right -padx 1 -pady 1 -fill both -expand true
#  pack $w.bar.rew -side right -padx 1 -pady 1 -fill both -expand true

  pack $w.bar -fill x -expand 1 -side right

#  $self instvar prevbutton
#  set prevbutton $w.bar.stop

#  start out stopped
#  $self highlight $w.bar.stop 1
}

#-----------------------------------------------------------------------
# Editor instproc buildTimeSlider {view w}
#  - creates a time slider to be used in setting time events
#  - w is the slider frame
#-----------------------------------------------------------------------
Editor instproc buildTimeSlider {w} {
  $self tkvar showpanel
  $self instvar mintime_ maxtime_ now_ mslider_ pipemode_ vslider_

  set mslider_ [new TimesliderModel $mintime_ $maxtime_ $now_ $self]
  set vslider_ [new TimesliderView $w $mslider_]

  $mslider_ addObserver $vslider_
  $mslider_ setpipemode 1
}

#------------- Functions needed for TimeSlider -------------
Editor instproc setsliderPressed {v} {
#  puts "setsliderPressed $v"
}

#-----------------------------------------------------------------------
#-----------------------------------------------------------------------
Editor instproc setDisp {time} {
  $self tkvar current_time_
  set current_time_ $time
}

#-----------------------------------------------------------------------
#-----------------------------------------------------------------------
Editor instproc slidetime {a b} {
#  puts "slidetime $a $b"
}

#-----------------------------------------------------------------------
#-----------------------------------------------------------------------
Editor instproc fast_fwd {} {
#  $self instvar current_time_step_ mslider_
#  $self tkvar current_time_
#  puts "fast_fwd -> $current_time_step_"
#  set current_time_ [expr $current_time_ + $current_time_step_]
#  $mslider_ setcurrenttime $current_time_step_
  
}

#-----------------------------------------------------------------------
#-----------------------------------------------------------------------
Editor instproc rewind {} {
#  $self instvar current_time_step_ mslider_
#  $self tkvar current_time_
#  puts "rewind -> $current_time_step_"
#  set current_time_ [expr $current_time_ - $current_time_step_]
#  $mslider_ setcurrenttime $current_time_step_
}

#-----------------------------------------------------------------------
#-----------------------------------------------------------------------
Editor instproc setsliderPressed {b} {
#  puts "setsliderPressed $b"
}

#-----------------------------------------------------------------------
#-----------------------------------------------------------------------
Editor instproc getrunning {} {
#  puts getrunning
  return 1
}


#-----------------------------------------------------------------------
# Editor instproc renderFrame {} 
#  -- called by the TimeSlider after the slider is released
#-----------------------------------------------------------------------
Editor instproc renderFrame {} {
  $self instvar editor_view_
  $self tkvar current_time_
  $editor_view_ draw $current_time_
}
#------------- End Functions needed for TimeSlider -------------


#-----------------------------------------------------------------------
# Editor instproc buildMenubar {w}
#  - w is the parent window frame
#-----------------------------------------------------------------------
Editor instproc buildMenubar {w} {
  $self instvar editorwindow_ menubar_ agent_types_ \
        source_types_ 
  $self tkvar current_agent_ current_source_
  # File Menu
  menubutton $w.file -text File -underline 0 \
             -menu $w.file.menu
  menu $w.file.menu
  $w.file.menu add command -label New \
                     -underline 0 -command "$self newFile"
  $w.file.menu add command -label Open \
                     -underline 0 -command "$self openFile"
  $w.file.menu add command -label Save \
                     -underline 0 -command "$self doSave"
  $w.file.menu add command -label "Save As..." \
                     -underline 4 -command "$self doSaveAs"
  $w.file.menu add separator
  $w.file.menu add command -label "Run Ns" \
                     -underline 1 -command "$self doSave; $self runNs"
  $w.file.menu add command -label "Close" \
                     -underline 0 -command "destroy $editorwindow_"

  # Edit Menu
  menubutton $w.edit -text Edit -underline 0 \
             -menu $w.edit.menu
  menu $w.edit.menu
  $w.edit.menu add command -label "Undo" \
                     -underline 0 -command "puts Undo" -state disabled
  $w.edit.menu add command -label "Redo" \
                     -underline 0 -command "puts Redo" -state disabled
  $w.edit.menu add separator
  $w.edit.menu add command -label "Cut" \
                     -underline 0 -command "puts Cut" -state disabled
  $w.edit.menu add command -label "Copy" \
                     -underline 0 -command "puts Copy" -state disabled
  $w.edit.menu add command -label "Paste" \
                     -underline 0 -command "puts Paste" -state disabled

  # Tools Menu
  menubutton $w.tools -text Tools -underline 0 \
             -menu $w.tools.menu
  menu $w.tools.menu
  $w.tools.menu add command -label "Select" \
                     -underline 0 -command "$self chooseTool select"
  $w.tools.menu add command -label "Add Node" \
      -underline 0  \
      -command "$self chooseTool addnode"
  $w.tools.menu add command -label "Add Link" \
      -underline 0  \
      -command "$self chooseTool addlink"

  # Cascading Agents menu
  $w.tools.menu add cascade -label "Agents" \
      -underline 0 -menu $w.tools.menu.agents
  menu $w.tools.menu.agents
  foreach i $agent_types_ {
    if {$i == "-"} {
      $w.tools.menu.agents add separator
    } else {
      $w.tools.menu.agents add radiobutton \
           -label $i -variable current_agent_ -value $i \
           -command "$self chooseAgent $i"
    }
  }

  # Cascading Sources menu
  $w.tools.menu add cascade -label "Traffic Sources" \
      -underline 0 -menu $w.tools.menu.sources
  menu $w.tools.menu.sources
  foreach i $source_types_ {
    $w.tools.menu.sources add radiobutton \
           -label $i -variable current_source_ -value $i \
           -command "$self chooseTrafficSource $i"
  }

  $w.tools.menu add command -label "Delete" \
                     -underline 0 -command "$self chooseTool deleteobject"

  # Attach Menus to Parent Window
  pack $w.file $w.edit \
       $w.tools -side left   

  # Add Help menu to the Right
  menubutton $w.help -text "Help" -underline 0 \
             -menu $w.help.menu
  menu $w.help.menu
  $w.help.menu add command -label "Help" \
             -underline 0 -command "$self showHelp"
  pack $w.help -side right

  # Enable Keyboard Focus
#  tk_menuBar $editorwindow_.menubar_ \ 
#             $editorwindow_.menubar_.file \
#             $editorwindow_.menubar_.edit

}

#----------------------------------------------------------------------
#
#----------------------------------------------------------------------
Editor instproc buttonReq { name replace } {
  $self instvar SharedEnv

  set var $SharedEnv(Buttons)

  if {$SharedEnv(ButtonOrient) == "left"} {
    set suborient top
  } else {
    set suborient left
  }
  set w ${name}
  if {[winfo exists $w]} {
      wm deiconify $w
      raise $w
      return
  }
  set SharedEnv($name) $replace

  eval toplevel $w
  wm protocol $w WM_DELETE_WINDOW "wm withdraw $w"
  wm title $w "Toolbar"

  frame $w.f

  set bframes "$w.f1 $w.f2 $w.f3 $w.f4 $w.f5 $w.f6 $w.f7 $w.f8 $w.f9"

  set i 0
  set bframe ""
  foreach buttontype $var {

    if {![expr $i % $SharedEnv(ButtonCount)]} {
      if {$bframe != ""} {
        pack $bframe -in $w.f -side $suborient
      }
      set bframe [lindex $bframes [expr $i / $SharedEnv(ButtonCount)]]
      frame $bframe
    }
    incr i
    set type [lindex $buttontype 0]
    set buttonName [lindex $buttontype 1]
    set buttonCanv [lindex $buttontype 2]
    switch "$type" {
      RadioButton {
        $self createRadioButton $w.b_${buttonName} \
          $SharedEnv(ButtonSize) $SharedEnv(ButtonSize) \
          "$buttonCanv" SharedEnv(Cursor) \
          $buttonName -bd 1 -highlightthickness 0
      }
      Button {
        $self createNormalButton $w.b_${buttonName} \
          $SharedEnv(ButtonSize) $SharedEnv(ButtonSize) \
          "$buttonCanv" \
          $buttonName -bd 1 -highlightthickness 0
      }
      JoinButton { 
        createRadioButton $w.b_${buttonName} \
          $SharedEnv(ButtonSize) $SharedEnv(ButtonSize) \
          "$buttonCanv" SharedEnv(JoinStyle) \
          $buttonName -bd 1 -highlightthickness 0
      }
      ArrowButton {
        createRadioButton $w.b_${buttonName} \
          $SharedEnv(ButtonSize) $SharedEnv(ButtonSize) \
          "$buttonCanv" SharedEnv(Arrow) \
          $buttonName -bd 1 -highlightthickness 0
      }
    }
    switch "$buttonName" {
      Paste {
        $self disablePaste
      }
      Rulers {
        bind $w.b_Rulers <Double-Button-1> {
          %W addtag disabled withtag all
                  $SharedEnv(CurrentCanvas) delete $SharedEnv(RulerX)
                  $SharedEnv(CurrentCanvas) delete $SharedEnv(RulerY)
            }     
        bind $w.b_Rulers <ButtonRelease-1> {+
          %W dtag disabled
        }
      }
    }
    set SharedEnv(W_${buttonName}) $w.b_${buttonName}
    pack $w.b_${buttonName} -in $bframe -side $SharedEnv(ButtonOrient)
  }
  pack $bframe -in $w.f -side left
  pack $w.f -in $w
}

#----------------------------------------------------------------------
#
#----------------------------------------------------------------------
Editor instproc createRadioButton {name width height cmds var value args} {
        $self instvar SharedEnv                   
                         
        eval canvas $name -width $width -height $height \
                -relief $SharedEnv(ButtonRelief) $args

        set saveit $SharedEnv(CurrentCanvas)
        set SharedEnv(CurrentCanvas) $name 
        eval $cmds                      
        $self originObject all                
        $self scaleObject all "" [winfo fpixels $name $width] \
                [winfo fpixels $name $height]
        set SharedEnv(CurrentCanvas) $saveit
        bind $name <Button-1> " 
                if {\[%W find withtag disabled\] == \"\"} {
                        $self setRadioButton $var $value
                        if {\"$var\" == \"SharedEnv(Cursor)\"} {
                                $self popupCanvasSet $value
                        }       
                }                       
        "                               
        bind $name <Enter> "            
                if {\[%W find withtag disabled\] == \"\"} {
                        %W configure -bg $SharedEnv(ActiveBackground)
                        if {[info exists SharedEnv(Msg_$value)]} {
                                set SharedEnv(Message) \$SharedEnv(Msg_$value)
                        } else {
                                set SharedEnv(Message) $value
                        }
                        $self enterToolButtons %W %x %y $value
                }
        "
        bind $name <Leave> "
                if {\[%W find withtag disabled\] == \"\"} {
                        %W configure -bg $SharedEnv(Background)
                        set SharedEnv(Message) \[$self setMessage\]
                        $self leaveToolButtons %W
                }
        "
     
        return $name
}    

#----------------------------------------------------------------------
#
#----------------------------------------------------------------------
Editor instproc setMessage {} {
  $self instvar SharedEnv
  return $SharedEnv(Cursor)
}

#----------------------------------------------------------------------
#
#----------------------------------------------------------------------
Editor instproc originObject { name } {
        $self instvar SharedEnv
                
        set bb [$SharedEnv(CurrentCanvas) bbox $name]
        set x [lindex $bb 0]
        set y [lindex $bb 1]
        if {$x > 0} {
                set nx -$x
        } else {
                set nx [expr abs($x)]
        }
        if {$y > 0} {
                set ny -$y              
        } else {         
                set ny [expr abs($y)]
        }       
        $SharedEnv(CurrentCanvas) move $name $nx $ny
}    

#----------------------------------------------------------------------
#
#----------------------------------------------------------------------
Editor instproc scaleObject {name ratio width height} {
  $self instvar SharedEnv

  set bb [eval $SharedEnv(CurrentCanvas) bbox $name]
  if {"$bb" != ""} {
    set x [lindex $bb 2]
    set y [lindex $bb 3]
    if {$ratio == "variable"} {
      set scalex [expr ($width + 0.0) / $x]
      set scaley [expr ($height + 0.0) / $y]
      if {$scalex > $scaley} {
        set scale $scaley
      } else {
        set scale $scalex
      }
    } elseif {$ratio != ""} {
      set scalex $ratio
      set scaley $ratio
      set scale $ratio
    } else {
      set scalex [expr ($width + 0.0) / $x]
      set scaley [expr ($height + 0.0) / $y]
      if {$scalex > $scaley} {
        set scalex $scaley
        set scale $scaley
      } else {
        set scaley $scalex
        set scale $scalex
      }
    }
    $SharedEnv(CurrentCanvas) scale $name 0 0 $scalex $scaley
    foreach i [$SharedEnv(CurrentCanvas) find withtag all] {
      set type [$SharedEnv(CurrentCanvas) type $i]
      if {"$type" != "text"} {
        continue
      }
      if {$SharedEnv(FontScale)} {
        set fn [$SharedEnv(CurrentCanvas) itemcget $i -font]
        regexp \
        {([-][^-]*-[^-]*-[^-]*-[^-]*-[^-]*-[^-]*-[^-]*-)([^-]*)(-.*)}\
          $fn dummy d1 size d2;
        if {"$dummy" != ""} {
          set nsize [expr round($size * $scale)]
          if {$nsize < 20} {
            set nsize 20
          }
          $SharedEnv(CurrentCanvas) itemconfigure $i \
            -font ${d1}${nsize}${d2}
          set fnn [$SharedEnv(CurrentCanvas) itemcget $i -font]
          regexp \
            {([-][^-]*-[^-]*-[^-]*-[^-]*-[^-]*-[^-]*-[^-]*-)([^-]*)(-.*)}\
            $fnn dummy d1 nsize d2;
          if { ($scale < 1 && $nsize < $size ) || \
             ($scale > 1 && $nsize > $size) } {
            $SharedEnv(CurrentCanvas) itemconfigure $i \
              -width [expr [$SharedEnv(CurrentCanvas) itemcget $i \
              -width] * $scale]
          } else {
            $SharedEnv(CurrentCanvas) itemconfigure $i \
              -font $fn
          }
        }
      } else {
        $SharedEnv(CurrentCanvas) itemconfigure $i \
          -width [expr [$SharedEnv(CurrentCanvas) itemcget $i -width] \
            * $scale]
      }
    }
  }
}

#----------------------------------------------------------------------
#
#----------------------------------------------------------------------
Editor instproc createNormalButton {name width height cmds value args} {
        $self instvar SharedEnv

        eval canvas $name -width $width -height $height \
                -relief $SharedEnv(ButtonRelief) $args
     
        set saveit $SharedEnv(CurrentCanvas)
        set SharedEnv(CurrentCanvas) $name
        eval $cmds
        $self originObject all
        $self scaleObject all "" [winfo fpixels $name $width] \
                [winfo fpixels $name $height]
        set SharedEnv(CurrentCanvas) $saveit
        bind $name <Button-1> "
                if {\[%W find withtag disabled\] == \"\"} {
                        set bd \[%W cget -bd\]
                        %W configure -relief sunken
                        %W move all \$bd \$bd
                        update idletasks
                }
        "
        bind $name <ButtonRelease-1> "
                if {\[%W find withtag disabled\] == \"\"} {
                        set bd \[%W cget -bd\]
                        $self popupCanvasSet $value
                        %W move all -\$bd -\$bd
                        %W configure -relief $SharedEnv(ButtonRelief)
                }
        "
        bind $name <Enter> "
                if {\[%W find withtag disabled\] == \"\"} {
                        %W configure -bg $SharedEnv(ActiveBackground)
                        if {[info exists SharedEnv(Msg_$value)]} {
                                set SharedEnv(Message) \$SharedEnv(Msg_$value)
                        } else {
                                set SharedEnv(Message) $value
                        }
                }
        "
        bind $name <Leave> " 
                if {\[%W find withtag disabled\] == \"\"} {
                        %W configure -bg $SharedEnv(Background)
                        set SharedEnv(Message) $SharedEnv(Cursor)
                }
        "
     
        return $name
}    

#----------------------------------------------------------------------
#
#----------------------------------------------------------------------
Editor instproc disablePaste {} {  
        $self instvar  SharedEnv  
     
        if {$SharedEnv(Cut) == ""} {
                $self disableButton .buttons.b_Paste
        }
}    

#----------------------------------------------------------------------
#
#----------------------------------------------------------------------
Editor instproc disableButton { name } {
        $self instvar SharedEnv
     
        $name create rectangle 0 0 $SharedEnv(ButtonSize) $SharedEnv(ButtonSize) \
                -fill $SharedEnv(Background) -stipple gray50 -outline {} -tags disabled
}   

#----------------------------------------------------------------------
#
#----------------------------------------------------------------------
Editor instproc setCursor { value } {                              
        $self instvar SharedEnv                                   
                                                        
        set SharedEnv(Cursor) $value                       
        $self setRadioButton SharedEnv(Cursor) $value    
}

#----------------------------------------------------------------------
#
#----------------------------------------------------------------------
Editor instproc setRadioButton { var value } {
        $self instvar SharedEnv

        if {[info exists SharedEnv(W_${value})]} {
                set bd [$SharedEnv(W_${value}) cget -bd]
                $self clearRadioVariable $var
                $SharedEnv(W_${value}) configure -relief sunken
                $SharedEnv(W_${value}) move all $bd $bd
                set SharedEnv($var) $SharedEnv(W_${value})
        }        
        set $var "$value"       
}

#----------------------------------------------------------------------
#
#----------------------------------------------------------------------
Editor instproc clearRadioVariable {var} { 
        $self instvar SharedEnv           
        if {[info exists SharedEnv($var)] && [winfo exists $SharedEnv($var)]} {
                set bd [$SharedEnv($var) cget -bd]
                $SharedEnv($var) configure -relief $SharedEnv(ButtonRelief)
                eval $SharedEnv($var) move all -$bd -$bd
        }       
        set $var ""  
}

#----------------------------------------------------------------------
#
#----------------------------------------------------------------------
Editor instproc enterObject { w x y value } {
  catch {destroy $w.f}
  frame $w.f -relief groove -borderwidth 2
  message $w.f.msg -width 8c -text $value
  pack $w.f.msg
  pack $w.f
  catch {place_frame $w $w.f [expr $x+10] [expr $y+10]}
}


#----------------------------------------------------------------------
#
#----------------------------------------------------------------------
Editor instproc enterToolButtons { w x y value } {
  $self instvar SharedEnv
    set geo [winfo geometry $w]
    set px [lindex [split $geo "+"] 1]
    set py [lindex [split $geo "+"] 2]
    set psize [lindex [split $geo "+"] 0]
    set pxsize [lindex [split $psize "x"] 0]
    set pysize [lindex [split $psize "x"] 1]
                        
    set w [winfo parent $w]

    catch {destroy $w.fm}

    frame $w.fm -relief groove -borderwidth 2
    message $w.fm.msg -width 10c -text $value -background yellow
    pack $w.fm.msg
    pack $w.fm 

    catch {place_frame $w $w.fm [expr $pxsize+$px] [expr $pysize+$py]}
}

#----------------------------------------------------------------------
#
#----------------------------------------------------------------------
Editor instproc leaveToolButtons { w } {
    set w [winfo parent $w]
    catch {destroy $w.fm} 
}


#----------------------------------------------------------------------
# Editor instproc createColorPalette {name redlist greenlist bluelist replace}
#  - This procedure is pretty messy but it seems to work in creating
#    a color selection box and putting the result into the variable
#    colorarea via the getcolornam procedure.
#  - I don't feel like renaming the variables to make to more readable
#    and straightforward like I have done on other parts of 
#    poorly written code similar to this.
#----------------------------------------------------------------------
Editor instproc createColorPalette {name redlist greenlist bluelist replace} {
	$self instvar SharedEnv netmodel_

	# Setup
	set w ${name}
	if {[winfo exists $w]} {
		wm deiconify $w
		raise $w
		return
	}

	set SharedEnv($name) $replace
	eval toplevel $w $SharedEnv(PrivateCmap)

	wm protocol $w WM_DELETE_WINDOW "wm withdraw $w"

	frame $w.f

	foreach red $redlist {
		frame $w.f.rcol_${red}
		foreach green $greenlist {
			frame $w.f.grow_${red}${green}
			foreach blue $bluelist {
				if { [info exists SharedEnv($w.f.c${red}${green}${blue})] } {
					frame $w.f.c${red}${green}${blue} \
						-relief raised -height 2m -width 2m \
						-highlightthickness 0 \
						-bd 1 -bg $SharedEnv($w.f.c${red}${green}${blue})
				} else {
					frame $w.f.c${red}${green}${blue} \
						-relief raised -height 2m -width 2m \
						-highlightthickness 0 \
						-bd 1 -bg "#${red}${green}${blue}"
				}
				pack $w.f.c${red}${green}${blue} -side left \
					-in $w.f.grow_${red}${green} -fill both -expand true

				bind $w.f.c${red}${green}${blue} <1> {
					%W configure -relief sunken
				}

				# This grabs the color and puts it into the color text box 
				bind $w.f.c${red}${green}${blue} <Double-Button-1> "$self getcolorname %W; destroy $w"

				bind $w.f.c${red}${green}${blue} <ButtonRelease-1> "$self getcolorname %W"
          
			}
			pack $w.f.grow_${red}${green} -side top -in $w.f.rcol_${red} -fill both -expand true
		}
		pack $w.f.rcol_${red} -side left -in $w.f -fill both -expand true
	}

	frame $w.f.c_none -width 4m -relief raised -bd 1 \
		-highlightthickness 0
	pack $w.f.c_none -in $w.f -side left -fill y 

	pack $w.f -in $w -expand true -fill both

	# Return
	wm geometry $w $SharedEnv(ColorWidth)x$SharedEnv(ColorHeight)
}

#----------------------------------------------------------------------
# Editor instproc getcolorname {w} 
#  - gets the clicked on color value from the color palette window
#    and puts the result into the textbox pointed at by colorarea
#  - colorarea is set in the displayObjectProperties procedure.
#----------------------------------------------------------------------
Editor instproc getcolorname {w} {
  $self instvar netmodel_ colorarea

  $w configure -relief raised
  set colorv [$w cget -bg] 
  set rgb [winfo rgb $w $colorv]
  set colorn "[$netmodel_ lookupColorName [lindex $rgb 0] \
               [lindex $rgb 1] [lindex $rgb 2]]"
  
  if {[info exists colorarea] && [winfo exists $colorarea]} {

      $colorarea delete 0.0 end
      $colorarea insert 0.0 $colorn 
      catch { $colorarea tag add bgcolor 0.0 end }
      $colorarea tag configure bgcolor -background $colorn
  
  }
  
}

#----------------------------------------------------------------------
#
#----------------------------------------------------------------------
Editor instproc showHelp {} {
  $self instvar editorwindow_
  set w $editorwindow_.1
  if {[winfo exists $w]} {
    wm deiconify $w
    raise $w
    return
  }
  toplevel $w
  wm title $w "Quick Tip to use Nam Editor"
  listbox $w.lbox -width 48 -height 11
  pack $w.lbox -side left -expand yes -fill both
  $w.lbox insert 0 \
          "Nam Editor Howto"\
          "-----------------------------------------------" \
          "1) Create nodes using the add node tool " \
          "2) Connect them using the add link tool." \
          "3) Attach an agent to a node"\
          "4) Connect agents by using the link tool."\
          "5) Set options by double clicking on each object."\
          "6) Save them with file name xxx.enam"\
          "7) Typing \"ns xxx.enam\""\
          "-----------------------------------------------" \
          "Please report all bugs to ns-users@isi.edu"
  $w.lbox selection set 0
}

#----------------------------------------------------------------------
# Editor instproc chooseTool {tool}
#  - tools are created in the buildToolbar procedure
#----------------------------------------------------------------------
Editor instproc chooseTool {tool} {
  $self instvar SharedEnv state_buttons_ toolbar_buttons_ \
                current_tool_ 
  $self tkvar current_agent_ current_source_

  # Release last tool button
  if [info exists current_tool_] {
    $toolbar_buttons_($current_tool_) configure -relief raised
  }

  # Depress current tool button
  set current_tool_ $tool
  if [info exists toolbar_buttons_($tool)] {
    $toolbar_buttons_($tool) configure -relief sunken
  }

  switch $tool {
    addagent {
#      puts "Current Agent: $current_agent_"
    }
    addtrafficsource {
#      puts "Current Source: $current_source_"
    }
  }
}

#----------------------------------------------------------------------
# Editor instproc chooseAgent {agent}
#  -- This function is used to set the current_agent_ variable.
#     I believe there is a bug in otcl and creating menubuttons.
#     The scope of a instvar varible doesn't seem to be passed in
#     properly to the menubutton function.
#----------------------------------------------------------------------
Editor instproc chooseAgent {agent} {
  $self tkvar current_agent_

  set current_agent_ $agent
  $self chooseTool addagent

}

#----------------------------------------------------------------------
# Editor instproc chooseTrafficSource {source} 
#  -- This function is used to set the current_source_ variable.
#     I believe there is a bug in otcl and creating menubuttons.
#     The scope of a instvar varible doesn't seem to be passed in
#     properly to the menubutton function.
#
#----------------------------------------------------------------------
Editor instproc chooseTrafficSource {source} {
 $self tkvar current_source_

 set current_source_ $source
 $self chooseTool addtrafficsource

}

#----------------------------------------------------------------------
#
#----------------------------------------------------------------------
Editor instproc popupCanvasSet { type } {
  $self instvar SharedEnv edit_state_ state_buttons_

  # Release last state button, press down new one
  if [info exists edit_state_] {
    $state_buttons_($edit_state_) configure -relief raised
  }
  set edit_state_ $type
  if [info exists state_buttons_($type)] {
    $state_buttons_($type) configure -relief sunken
  }

  $self setCursor "$type"
  switch "$SharedEnv(Cursor)" {
    Cut {
      set SharedEnv(Message) Cut
      $self setCursor "Cut"
      #cutCopySelected Cut Imp_Selected 1
      #set SharedEnv(Message) Select
      #enableSelectionButtons
    }
    Paste {
      if {$SharedEnv(Cut) == ""} {
        setCursor "Select"
        set SharedEnv(Message) Select
        enableSelectionButtons
      }
    }
    Link -
    Node -
    Queue -
    Agent -
    Text {
      set SharedEnv(Sub-Cursor) ""
      #$SharedEnv(CurrentCanvas) delete SharedEnv(curline)
      #$SharedEnv(CurrentCanvas) delete selector
      $SharedEnv(CurrentCanvas) dctag
      set SharedEnv(Message) $SharedEnv(Cursor)
      #deselectAll
      #disableSelectionButtons
    }
    Select {
      set SharedEnv(Message) $SharedEnv(Cursor)
      #deselectAll
      #enableSelectionButtons
    }
  
    Print {
      #printRaise .print
      
      return
    }
  }
  focus $SharedEnv(CurrentCanvas)
}

#----------------------------------------------------------------------
#
#----------------------------------------------------------------------
Editor instproc newFile {} {
        set editor [new Editor ""]
}

#----------------------------------------------------------------------
#
#----------------------------------------------------------------------
Editor instproc openFile {} {
  $self instvar editorwindow_
#  $self tkvar openFile_

  set openFile_ [tk_getOpenFile \
                -filetypes {{{Nam Editor} {.ns .enam}} \
                            {{All Files} {*}}} \
                -parent $editorwindow_]

  if {$openFile_ == ""} {
    return 
  }

  set editor [new Editor $openFile_]
  return
}

#----------------------------------------------------------------------
# Editor instproc doSave {}
#  - Saves the file if save_filename_ is defined otherwise it
#    calls doSaveAs
#----------------------------------------------------------------------
Editor instproc doSave {} {
  $self instvar save_filename_
  
  if {"$save_filename_" != ""} {
    $self writeFile $save_filename_
  } else {
    $self doSaveAs
  }
}

#----------------------------------------------------------------------
# Editor instproc doSaveAs {}
#  - brings up a saveas dialog box and if the filename exists it calls
#    writeFile to write out the nam editor ns script
#----------------------------------------------------------------------
Editor instproc doSaveAs {} {
  $self instvar save_filename_ editorwindow_

  set filename [tk_getSaveFile -defaultextension ".ns" \
                               -filetypes {{{Nam Editor} {.ns .enam}}} \
                               -parent $editorwindow_]
  if {"$filename" != ""} {
    $self writeFile $filename
  }
}

#----------------------------------------------------------------------
# Editor instproc writeFile {filename}
#  - calls saveasns to write out the nam generate ns script to a file
#    and raises a message if a problem occurred.
#  - "saveasns" is part of enetmodel.cc:command
#----------------------------------------------------------------------
Editor instproc writeFile {filename} {
  $self instvar netmodel_ save_filename_ editorwindow_

  if {"$filename" != ""} {
    catch {$netmodel_ saveasns $filename} error

    if {![info exists error]} {
      $self msgRaise .diagmsg "ERROR: Unable to save file: $filename."
    } else {
      set save_filename_ $filename
      wm title $editorwindow_ "NAM Editor - $save_filename_"
      update
    }
  }
}

#----------------------------------------------------------------------
#
#----------------------------------------------------------------------
Editor instproc runNs {} {
  $self instvar save_filename_
  if {$save_filename_ != ""} {
    set filename $save_filename_
    catch {exec ns $filename &} error
    if {[info exists error]} {
      if {[regexp {([0-9]+)} $error matched process_id]} {
        $self msgRaise .diagmsg "Ns should be running as process id $process_id.\n\
                                 The result will be shown when it finishes."
      } else {
        $self msgRaise .diagmsg "Ns was unable to run.          \n\
                                 The error message reported was:\n\
                                 \[ $error \]"
      }
    }
  }
}

#----------------------------------------------------------------------
#
#----------------------------------------------------------------------
Editor instproc msgRaise {name message} {  
        $self instvar SharedEnv   

        set w ${name}   
        set SharedEnv(MsgReplace) $SharedEnv(CurrentCanvas)

        if {![winfo exists $name]} {
               $self  msgRequestor $name
        }
        $w.message configure -text "$message"
        wm deiconify $w
        focus $w
}

#----------------------------------------------------------------------
#
#----------------------------------------------------------------------
Editor instproc msgRequestor {name} {
        $self instvar SharedEnv

        set w ${name}   
                toplevel $w
#               wm transient $w .
                wm protocol $w WM_DELETE_WINDOW {set dummy 1}
        frame $w.f -borderwidth 0 -highlightthickness 0
        frame $w.texttop -borderwidth 0 -highlightthickness 0
        frame $w.textbottom -relief raised -bd 1
        label $w.message
        button $w.ok -text "Ok" -command "$self msgLower $w"
        pack $w.message -in $w.texttop -fill both -expand true
        pack $w.ok -in $w.textbottom -side bottom -padx 2m -pady 2m
        pack $w.texttop $w.textbottom -anchor sw -in $w -fill x -expand true
        pack $w.f -in $w
        wm withdraw $w
}

#----------------------------------------------------------------------
#
#----------------------------------------------------------------------
Editor instproc msgLower {name} {
        $self instvar SharedEnv

        set w ${name}
        wm withdraw $w
}


#-----------------------------------------------------------------------
# proc radioButtonMenu {parent selection_variable values}
#   - Adds a radiobutton list menu from a list of values
#-----------------------------------------------------------------------
Editor instproc radioButtonMenu {parent_menu selection values} {
  $self instvar $selection

  foreach i $values {
    $parent_menu add radiobutton -label $i -variable $selection
  }
  return $parent_menu
}
