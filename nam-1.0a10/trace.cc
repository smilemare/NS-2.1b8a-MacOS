/*
 * Copyright (c) 1991,1993 Regents of the University of California.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *  This product includes software developed by the Computer Systems
 *  Engineering Group at Lawrence Berkeley Laboratory.
 * 4. Neither the name of the University nor of the Laboratory may be used
 *    to endorse or promote products derived from this software without
 *    specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * @(#) $Header: /nfs/jade/vint/CVSROOT/nam-1/trace.cc,v 1.65 2001/05/11 00:32:56 mehringe Exp $ (LBL)
 */

#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <fcntl.h>
#ifdef WIN32
#include <windows.h>
#include <io.h>
#else
#include <unistd.h>
#include <sys/file.h>
#endif
#include <sys/stat.h>
#include <ctype.h>

#include "trace.h"
#include "state.h"
#include "packet.h"
#include "address.h"
#include "nam_stream.h"

extern double time_atof(const char*);

class TraceClass : public TclClass {
 public:
  TraceClass() : TclClass("Trace") {}
  TclObject* create(int argc, const char*const* argv);
} trace_class;

TclObject* TraceClass::create(int argc, const char*const* argv) {
  if (argc != 6)
    return (0);
  /*
   * Open the trace file for reading and read its first line
   * to be able to determine needed values: trace start time,
   * trace duration, etc.
   */
  Trace* tr = new Trace(argv[4], argv[5]);
  if (!tr->valid()) {
    delete tr;
    tr = NULL;
  }
  return (tr);
}

//----------------------------------------------------------------------
// Trace::Trace(const char *fname, const char *animator) 
//   - Create a new tracefile parser and event handler for
//     file "fname" and connect it to the animator object
//----------------------------------------------------------------------
Trace::Trace(const char *fname, const char *animator) : 
  handlers_(0),
  nam_(0),
  skipahead_mode_(0),
  count_(0) {
  
  last_event_time_ = 0.0;

  // Connect to nam animator
  nam_ = (NetworkAnimator *)TclObject::lookup(animator);

  strncpy(fileName_, fname, sizeof(fileName_)-1);
  fileName_[sizeof(fileName_)-1] = 0;
  nam_stream_ = NamStream::open(fileName_);
  if (!nam_stream_->is_ok()) {
    delete nam_stream_;
    nam_stream_ = NULL;
    perror("nam: fdopen");
    // exit(1);
    return; // tr->valid() will check this
  }
  findLastLine();

  /*
   * Go to the beginning of the file, read the first line and
   * save its contents.
   */
  off_t pos = nam_stream_->seek(0, SEEK_SET);
  assert(pos == 0);

  /*
   * Minimum time on the nam time slider is the time of the first
   * trace event.
   */
  mintime_ = nextTime();
  now_ = mintime_;

  State::instance()->setTimes(mintime_, maxtime_);

  // Initialize ParseTable
  parse_table_ = new ParseTable(&pending_);
}

//----------------------------------------------------------------------
// Trace::~Trace()
//----------------------------------------------------------------------
Trace::~Trace() {
  delete parse_table_;
}

//----------------------------------------------------------------------
//----------------------------------------------------------------------
int
Trace::valid() {
  return (nam_stream_ && nam_stream_->is_ok());
}

//----------------------------------------------------------------------
//----------------------------------------------------------------------
int
Trace::ReadEvent(double now, TraceEvent& e) {
  /*
   * If specified time is after the next event's time, read
   * the next line and return 1. Otherwise, just return 0.
   */
  if (direction_ == FORWARDS) {
    if ((pending_.time != TIME_EOF) && (pending_.time < now)) {
      e = pending_;
      scan();
      return (1);
    }
  } else {
    /*going backwards!*/
    /*
     * two occasions: 1) pending_.time > now, 
     * 2) pending_.time = -1, which means go to the end of file
     */
    if ((pending_.time > now) || (pending_.time == TIME_EOF)) {
      e = pending_;
      scan();
      if (pending_.time == TIME_EOF)
        pending_.time = TIME_BOF;
      return (1);
    }
  }
  return (0);
}

//---------------------------------------------------------------------
// int
// Trace::nextLine()
//
// - This needs to be cleaned up and it may effect other parts of
//   the file parsing code.  It is not very efficient with the amount
//   of seeking going on.  
//---------------------------------------------------------------------
int
Trace::nextLine() {
  char * position;

  if (direction_ == FORWARDS) {
    // moving forward through the trace file
    // We need to read in one line at a time
    nam_stream_->gets(pending_.image, sizeof(pending_.image));

    ++lineno_;
    if (nam_stream_->eof()) {
      Tcl& tcl = Tcl::instance();
      tcl.evalf("%s stopmaxtime %f", nam()->name(), pending_.time);
      return 0;
    }

  } else {
    // moving backwards through the trace file 
    position = nam_stream_->rgets(pending_.image, sizeof(pending_.image));

    lineno_--;
    if (position == NULL ) {
      fprintf(stderr, "nam file reading error.\n");
      return -1; // signal reading error
    }

    if (nam_stream_->tell() == 0) {
      // Beginning of file reached so we need to stop
      return -1;
    }  
  }

  return(nam_stream_->tell());
}

//---------------------------------------------------------------------
// void
// Trace::scan()
//   - scan for the next valid nam event line in file
//   - Once everything becomes stable you can to get rid of all the
//     data type specific scan functions
//---------------------------------------------------------------------
void
Trace::scan() {
  char * time;
  /*
   * Read the next line in the trace file and store its contents
   * depending on line type.
   */
  while (1) {
    TraceEvent & p = pending_;
    time = p.image;

    p.offset = nextLine();

    if (p.offset <= 0) {
      // We reached the beginning of the file
      pending_.time = TIME_EOF;
      // Reset the last event time tracker
      last_event_time_ = 0.0;
      fprintf(stderr, "EOF Reached.\n");
      return;
    }

    // -----------------------------------------------
    // This -t * line skipping is a hack to
    // enable the wireless code to work properly.
    // All the -t * lines are parsed in tcl files
    // before the animation starts and reparsing them
    // seems to screw up the wireless stuff.
    // ------------------------------------------------
    
    // Skip over -t * lines
    while (*time != '\0') {
      // Find the -t flag
      if (*time == '-') {
        time++;
        if (*time == 't') {
          // Eat the whitespace
          time++;
          while (*time == ' ' || *time == '\t') {
            time++;
          } 

          // We should be pointing to the
          // -t flag value now
          if (*time == '*') {
            break;
          }
        }
      } else {
        time++;
      }
    }
    
    if (*time == '*') {
      // Notify user if their tracefile has initialization events that
      // are mixed in with animation events
      switch (direction_) {
        case FORWARDS:
          // last_event_time_ is initialized to 0.0 since events cannot happen before this time.
          if (last_event_time_ != 0.0) {
            fprintf(stderr, "Warning: Initialization event is mixed in with animation events.\n");
            fprintf(stderr, "%s", p.image);
            fprintf(stderr, "The above event should occur before all -t <time> animation events.\n\n");
            fprintf(stderr, "last_time_event_ %f\n", last_event_time_);
          }
          break;
      }
      last_event_time_ = 0.0;

      if (p.offset <= 0) {
	      pending_.time = TIME_EOF;
	      fprintf(stderr, "EOF Reached.\n");
	      return;
      }
      continue;
    } 

    if (parse_table_->parseLine(p.image)) {
      switch (direction_) {
        // Notify user if their tracefile has records that are out of order
        case FORWARDS:
          if (pending_.time < last_event_time_) {
            fprintf(stderr, "Warning: Tracefile events are not sorted by time.\n");
            fprintf(stderr, "%s", p.image);
            fprintf(stderr, "The above event should occur at or after -t %f.\n\n",
                            last_event_time_);
          }
          break;
        case BACKWARDS:
          if (pending_.time > last_event_time_ && last_event_time_ != 0.0) {
            fprintf(stderr, "Warning: Tracefile events are not sorted by time.\n");
            fprintf(stderr, "%s", p.image);
            fprintf(stderr, "The above event should occur at or before -t %f.\n\n",
                            last_event_time_);
          }
          break;
      }
      last_event_time_ = pending_.time;
      return;

    } else {
      fprintf(stderr, "Parsing error in event.\n");
    }
    
  } // end while
  /* NOTREACHED */
}


char *Trace::parse_string_literal(char *orig, char *buffer)
{
  char *tmp, *str;
  str=orig;
  if (*str=='"') {
    strncpy(buffer, str+1, MAXNAME);
    str=str+1;
    tmp=strchr(buffer, '"');
    if (tmp!=NULL)
      str=strchr(str, '"');
    else {
      tmp=strchr(buffer, '\n');
      if (tmp!=NULL)
  str=strchr(str, '\n');
#ifdef WIN32
      // Remove \r
      if (tmp != NULL) *tmp = *(tmp-1) = 0;
#endif
    }
    if ((tmp!=NULL)&&(tmp-buffer<=MAXNAME))
  *tmp='\0';
  } else {
    strncpy(buffer, str, MAXNAME);
    tmp=strchr(buffer, ' ');
    if (tmp!=NULL)
      str=strchr(str, ' ');
    else {
      tmp=strchr(buffer, '\n');
      if (tmp!=NULL)
  str=strchr(str, '\n');
#ifdef WIN32
      // Remove \r
      if (tmp != NULL) *tmp = *(tmp-1) = 0;
#endif
    }
    if ((tmp!=NULL)&&(tmp-buffer<=MAXNAME))
      {
  *tmp='\0';
      }
  }
  return str;
}

int Trace::packetscan(char *cp, double *time, PacketEvent *pe) {
  char buf[MAXVALUE];
  char *tmp;
  char *value;
  char *str=cp;
  int check=0;
  pe->pkt.convid[0]='\0';
  while(str != NULL) {
    str=strchr(str,' ');
    if (str==NULL)
      break;
    
    // Read spaces
    while(*str==' ')
      str++;

    if (*str!='-')
      break;

    str++;
    if (*(str+1)==' ')
      value=str+2;
    switch (*str) {
      case 't': /*time*/
        check++;
        *time=atof(value);
        str+=2;
        break;
      case 's': /*src*/
        check++;
        // we need new space because strtok may damage the orig string
        parse_string_literal(value, buf);
        pe->src = atoi(buf);
        //pe->src = Address::instance().str2addr(buf);
        str+=2;
        break;
      case 'd': /*dst*/
        check++;
        parse_string_literal(value, buf);
        pe->dst = atoi(buf);
        //pe->dst = Address::instance().str2addr(buf);
        str+=2;
        break;
      case 'e': /*extent*/
        check++;
        pe->pkt.size=atoi(value);
        str+=2;
        break;
      case 'a': /*attr*/
        check++;
        pe->pkt.attr=atoi(value);
        str+=2;
        break;
      case 'i': /*id*/
        check++;
        pe->pkt.id=atoi(value);
        str+=2;
        break;
      case 'l': /*energy*/
        check++;
        pe->pkt.energy=atoi(value);
        str+=2;
        break;
      case 'c': /*conversation*/
        check++;
        strncpy(pe->pkt.convid, value, CONVLEN);
        tmp=strchr(pe->pkt.convid, ' ');
        if (tmp==NULL) {
          tmp=strchr(pe->pkt.convid, '\n');
#ifdef WIN32
          if (tmp != NULL)
            *tmp = *(tmp-1) = 0;
#endif
        }
        if ((tmp!=NULL) && (tmp-pe->pkt.convid<=CONVLEN))
          *tmp='\0';
        str+=strlen(pe->pkt.convid)+2;
        break;
      case 'x': /*comment*/
        char *tmp1, *tmp2 , *tmp3;
        char tmpstr[CONVLEN];
        strncpy(tmpstr, value, CONVLEN);
        tmp=strchr(tmpstr, '}');
        if (tmp==NULL) {
          tmp=strchr(tmpstr, '\n');
#ifdef WIN32
          if (tmp != NULL)
            *tmp = *(tmp-1) = 0;
#endif
        }
        if ((tmp!=NULL)&&(tmp-tmpstr<=CONVLEN))
          *tmp='\0';
        str+=strlen(pe->pkt.convid)+2;

        //retrieve end2end source and destination
        tmp1=strchr(tmpstr, '.');
        tmp2=strchr(tmpstr, ' ');
        *tmp1='\0';
        tmp3=strchr(tmp2, '.');
        *tmp3='\0';
        pe->pkt.esrc=atoi(tmpstr+1);
        pe->pkt.edst=atoi(tmp2+1);
        break;
      case 'p': /*packet type*/
        strncpy(pe->pkt.type, value, PTYPELEN);
        tmp=strchr(pe->pkt.type, ' ');
        if (tmp==NULL) {
          tmp=strchr(pe->pkt.type, '\n');
#ifdef WIN32
          if (tmp != NULL) *tmp = *(tmp-1) = 0;
#endif
        }
        if ((tmp!=NULL)&&(tmp-pe->pkt.type<=PTYPELEN))
          *tmp='\0';
        str+=strlen(pe->pkt.type)+2;
        break;
      case 'k': /*packet type*/
        strncpy(pe->pkt.wtype, value, PTYPELEN);
        tmp=strchr(pe->pkt.wtype, ' ');
        if (tmp==NULL) {
          tmp=strchr(pe->pkt.wtype, '\n');
#ifdef WIN32
          if (tmp != NULL)
            *tmp = *(tmp-1) = 0;
#endif
        }
        if ((tmp!=NULL)&&(tmp-pe->pkt.wtype<=PTYPELEN))
        *tmp='\0';
        str+=strlen(pe->pkt.wtype)+2;
        break;
    }
  }

  // Check for proper number of arguments
  if (check<6)
    return -1;
  else 
    return 0;
}

int Trace::linkscan(char *cp, double *time, LinkEvent *le) {
  char *tmp;
  char *str=cp;
  char *value;
  int check=0;
  // initialize LinkEvent variables to the default values
  le->link.dlabel[0] = le->link.odlabel[0] = 0;
  le->link.length = 1.0;
  le->link.angle = 0.0;
  parse_string_literal("black", le->link.color);
  parse_string_literal("black", le->link.oldColor);
  parse_string_literal ("UP", le->link.state);

  // Parse the nam animation line
  while(str!=NULL) {
    str=strchr(str,' ');
    if (str==NULL)
      break;

    // Read Whitespace
    while(*str==' ')
      str++;

    if (*str!='-')
      break;
    str++;
      
    if (*(str+1)==' ')
      value=str+2;

    switch (*str) {
      case 'c':
        /* color */
        check++;
        str = parse_string_literal(value, le->link.color);
        break;
      case 'o':
        // Either Orientation or old color
        le->link.angle = atof(value);
        if (!le->link.angle) {
          /* old color, used for backtracing */
          check++;
          str = parse_string_literal(value, le->link.oldColor);
        }
        break;
      case 't':
        /*time*/
        if (strncmp(value, "*", 1)==0) { // Skip over links with t = *
          if (*time < 0)
            *time = 0;
           
          // Set src and dst to -1 to indicate 
          // this node is to be skipped by NetModel::handle()
          le->src = -1;
          le->dst = -1;
          return 0;
        }
        check++;
        *time=atof(value);
        str+=2;
        break;
      case 's': /*source node id*/
        check++;
        le->src=atoi(value);
        str+=2;
        break;
      case 'd': /*destnation node id*/
        check++;
        le->dst=atoi(value);
        str+=2;
        break;
      case 'S': /*link state*/
        check++;
        strncpy(le->link.state, value, MAXVALUE);
        tmp=strchr(le->link.state, ' ');
        if (tmp==NULL) {
          tmp=strchr(le->link.state, '\n');

          #ifdef WIN32
          if (tmp != NULL)
            *tmp = *(tmp-1) = 0;
          #endif

        }
        if ((tmp!=NULL) && 
            (tmp-le->link.state <= MAXVALUE))
          *tmp='\0';

        str+=strlen(le->link.state)+2;
        break;
      case 'l': // Check for length or label
        le->link.length = atof(value);
        if (!le->link.length) {
          /* Label beneath a link (dlabel) */
          check++;
          str = parse_string_literal(value, le->link.dlabel);
        }
        break;
      case 'L': /* old dlabel, used for backtracking */
        check++;
        str = parse_string_literal(value, le->link.odlabel);
        break;
      case 'p': /* locate label (maybe label location?)*/
        check++;
        str = parse_string_literal(value, le->link.direction);
        break;
      case 'P': /* locate label, for backtracking*/
        check++;
        str = parse_string_literal(value, le->link.odirection);
        break;
      case 'e': /* label color*/
        check++;
        str = parse_string_literal(value, le->link.dcolor);
        break;
      case 'E': /* old label color*/
        check++;
        str = parse_string_literal(value, le->link.odcolor);
        break;
      case 'r': // Link Bandwith (Rate)
        check++;
        le->link.rate = atof(value);

        /* move the pointer */
        tmp = strchr(value, ' ');
        if (tmp == NULL) {
          str = tmp;  
        } else {
          size_t pos1 = strcspn(str, value);
          size_t pos2 = strcspn(str, tmp);
          str+=pos2-pos1+2;
        }
        break; 
      case 'D': // Delay
        check++;
        le->link.delay = atof(value);

        /* move the pointer */
        tmp = strchr(value, ' ');
        if (tmp == NULL) {
          str = tmp;  
        } else {
          size_t pos1 = strcspn(str, value);
          size_t pos2 = strcspn(str, tmp);
          str+=pos2-pos1+2;
        }
        break;
    }
  }
  //
  // Parameter Check
  if (check < 4)
    return -1;
  else 
    return 0;
}

int Trace::group_scan(char *cp, double *time, GroupEvent *ge) {
  char *str=cp;
  char *value ;

  int check=0;
  ge->grp.name[0] = 0;

  while(str!=NULL) {
    str=strchr(str,' ');
    if (str==NULL)
      break;
    while(*str==' ')
      str++;
    if (*str!='-')
      break;
    str++;
    value = str + 2;

    switch (*str) {
      case 't': /* time */
        check++;
        *time=atof(value);
        str=value+1;
        break;
      case 'n': /* group name, if any */
        char *tmp;
        check++;
        if (*(value)=='"') {
          strncpy(ge->grp.name, value+1, MAXNAME);
          tmp=strchr(ge->grp.name, '"');
          if (tmp==NULL) {
            tmp=strchr(ge->grp.name, '\n');
#ifdef WIN32
            if (tmp != NULL)
              *tmp = *(tmp-1) = 0;
#endif
          }
          if ((tmp!=NULL)&&(tmp-ge->grp.name<=MAXNAME))
            *tmp='\0';
          str+=strlen(ge->grp.name)+4;
        } else {
          strncpy(ge->grp.name, value, MAXNAME);
          tmp=strchr(ge->grp.name, ' ');
          if (tmp==NULL) {
            tmp=strchr(ge->grp.name, '\n');
#ifdef WIN32
            if (tmp != NULL)
              *tmp = *(tmp-1) = 0;
#endif
          }
          if ((tmp!=NULL)&&(tmp-ge->grp.name<=MAXNAME))
            *tmp='\0';
          str+=strlen(ge->grp.name)+2;
        }
        break;
      case 'i': /* node with which this mark is associated */
        check++;
        ge->src = atoi(value);
        str+=2;
        break;
      case 'a': /* join */
        check++;
        ge->grp.flag = GROUP_EVENT_JOIN;
        ge->grp.mbr = atoi(value);
        str = value + 1;
        break;
      case 'x': /* leave */
        check++;
        ge->grp.flag = GROUP_EVENT_LEAVE;
        ge->grp.mbr = atoi(value);
        str = value + 1;
        break;
    }
  }

  if (check < 3)
    return -1;
  else 
    return 0;
}

int Trace::nodemark_scan(char *cp, double *time, NodeMarkEvent *ne) {
  char *str=cp;
  char *value;

  int check=0;
  /* ne->dst=-1; */
  ne->mark.expired = 0;

  while(str!=NULL) {
    str=strchr(str,' ');
    if (str==NULL)
      break;

    while(*str==' ')
      str++;

    if (*str!='-')
      break;
    str++;
    value=str+2;

    switch (*str) {
      case 't': /* time */
        check++;
        *time=atof(value);
        str=value+1;
        break;
      case 'n': /* node mark name */
        char *tmp;
        check++;
        if (*(value)=='"') {
          strncpy(ne->mark.name, value+1, MAXVALUE);
          tmp=strchr(ne->mark.name, '"');
          if (tmp==NULL) {
            tmp=strchr(ne->mark.name, '\n');
#ifdef WIN32
            if (tmp != NULL)
              *tmp = *(tmp-1) = 0;
#endif
          }
          if ((tmp!=NULL)&&(tmp-ne->mark.name<=MAXVALUE))
            *tmp='\0';
          str+=strlen(ne->mark.name)+4;
        } else {
          strncpy(ne->mark.name, value, MAXVALUE);
          tmp=strchr(ne->mark.name, ' ');
          if (tmp==NULL) {
            tmp=strchr(ne->mark.name, '\n');
#ifdef WIN32
            if (tmp != NULL)
              *tmp = *(tmp-1) = 0;
#endif
          }
          if ((tmp!=NULL)&&(tmp-ne->mark.name<=MAXVALUE))
            *tmp='\0';
          str+=strlen(ne->mark.name)+2;
        }
        break;
      case 's': /* node with which this mark is associated */
        check++;
        ne->src=atoi(value);
        str+=2;
        break;
      case 'c': /* color */
        check++;
        str = parse_string_literal(value, ne->mark.color);
        break;
      case 'h': /* shape */
        check++;
        str = parse_string_literal(value, ne->mark.shape);
        break;
      case 'X': /* expired? */
        check++;
        ne->mark.expired = 1;
        break;
    }
  }

  if (check < 4)
    return -1;
  else 
    return 0;
}

int Trace::nodescan(char *cp, double *time, NodeEvent *ne) {
  char *str = cp;
  char *value;
  int check = 0;
  double temp;

  // Initialize default values
  ne->dst=-1;
  ne->node.dlabel[0] = ne->node.odlabel[0] = 0;
  ne->node.lcolor[0] = 0;
  ne->x = 0.0;
  ne->y = 0.0;
  ne->size = 0.0;

  while(str!=NULL) {
    str=strchr(str,' ');
    if (str==NULL)
      break;

    // Eat spaces
    while(*str==' ')
      str++;

    if (*str!='-')
      break;

    str++;
    if (*(str+1)==' ')
      value=str+2;

    switch (*str) {
      case 'c':
        /* color */
        check++;
        str = parse_string_literal(value, ne->node.color);
        break;
      case 'o':
        /* old color, used for backtracing */
        check++;
        str = parse_string_literal(value, ne->node.oldColor);
        break;
      case 't': 
        /* time */
        if (strncmp(value,"*",1) == 0) {
          if (*time < 0)
            *time = 0;
          // 
          // Set ne->src to -1 so that it is skipped by NetModel::handle()
          //
          ne->src = -1;
          return 0;
        }
        check++;
        *time = atof(value);
        str = value + 1;
        break;
      case 's': /* source */
        check++;
        ne->src=atoi(value);
        str = value + 1;
        break;
      case 'd': /* destination */
        ne->dst = atoi(value);
        str = value + 1;
        break;
      case 'S': /*node state*/
        char *tmp;
        check++;
        strncpy(ne->node.state, value, MAXNAME);
        tmp = strchr(ne->node.state, ' ');
        if (tmp==NULL) {
          tmp=strchr(ne->node.state, '\n');
#ifdef WIN32
          if (tmp != NULL)
            *tmp = *(tmp-1) = 0;
#endif
        }
        if ((tmp!=NULL)&&(tmp-ne->node.state<=MAXNAME))
          *tmp='\0';
        str+=strlen(ne->node.state)+2;
        break;
      case 'a': /* Node address */
        char tmp_buffer[MAXVALUE];
        check++;
        str = parse_string_literal(value, tmp_buffer);
        ne->node.addr = Address::instance().str2addr(tmp_buffer);
        break;
      case 'l': /* Label beneath a node (dlabel) */
        check++;
        str = parse_string_literal(value, ne->node.dlabel);
        break;
      case 'L': /* old dlabel, used for backtracking */
         check++;
         str = parse_string_literal(value, ne->node.odlabel);
         break;
      case 'p': /* locate label (maybe label location?)*/
        check++;
        str = parse_string_literal(value, ne->node.direction);
        break;
      case 'P':
        /* locate label, for backtracking*/
        check++;
        str = parse_string_literal(value, ne->node.odirection);
        break;
      case 'i':
        /* inside label color*/
        check++;
        str = parse_string_literal(value, ne->node.lcolor);
        break;
      case 'I':
        /* inside label color -old */
        check++;
        str = parse_string_literal(value, ne->node.olcolor);
        break;
      case 'e':
        /* label color*/
        check++;
        str = parse_string_literal(value, ne->node.dcolor);
        break;
      case 'E':
        /* label color*/
        check++;
        str = parse_string_literal(value, ne->node.odcolor);
        break;
      case 'x':
        /* node x location */
        check++;
        ne->x=atof(value);
        str=value+1;  
        break;
      case 'y':
        /* node y location */
        check++;
        ne->y=atof(value);
        str=value+1;  
        break;
      case 'u':
        /* code x velocity */
        check++;
        ne->x_vel_=atof(value);
        str=value+1;
        break;
      case 'v':
        // Check if velocity or node shape
        if (!strncmp(value, "circle",6)) {
          strcpy(ne->mark.shape,"circle");
          str = value + 6; 
        } else if (!strncmp(value, "box",3)) {
          strcpy(ne->mark.shape,"box");
          str = value + 3; 
        } else if (!strncmp(value, "hexagon",7)) {
          strcpy(ne->mark.shape,"hexagon");
          str = value + 7; 
        } else {
          /* code y velocity */
          ne->y_vel_=atof(value);
          str = value + 1;
        }
        check++;
        break;
      case 'T':
        /* node stop time */
        check++;
        ne->stoptime = atof(value);
        str = value + 1;
        break;
      case 'Z': 
        // Unknown 
        temp = atof(value);
        str = value + 1;
        break;
      case 'z':
        // Size of node ?
        ne->size = atof(value);
        str = value + 1;
        break;
    }
  }

  if (check < 3)
    return -1;
  else 
    return 0;
}

int Trace::agentscan(char *cp, double *time, AgentEvent *ae) {
  char *str=cp;
  char *value;
  int check=0;
 
  // Initialize default values
  ae->dst=-1;
  ae->agent.expired=0;
  while(str!=NULL) {
    str=strchr(str,' ');
    if (str==NULL)
      break;
    while(*str==' ')
      str++;
    if (*str!='-')
      break;
    str++;
    value = str+2;
    switch (*str) {
      case 't': /*time*/
        check++;
        *time=atof(value);
        str+=2;
        break;
      case 's': /* source node id */
        check++;
        ae->src=atoi(value);
        str+=2;
        break;
      case 'd': /* destination node id */
        ae->dst=atoi(value);
        str+=2;
        break;
      case 'x':
      case 'X': /*this agent is to be deleted*/
        check++;
        ae->agent.expired=1;
        break;
      case 'n': /* agent name */
        char *tmp;
        check++;
        if (*(value)=='"') {
          strncpy(ae->agent.name, value+1, MAXNAME);
          tmp = strchr(ae->agent.name, '"');
          if (tmp==NULL) {
            tmp = strchr(ae->agent.name, '\n');
#ifdef WIN32
            if (tmp != NULL)
              *tmp = *(tmp-1) = 0;
#endif
          }
          if ((tmp!=NULL)&&(tmp-ae->agent.name<=MAXNAME))
            *tmp='\0';
          str+=strlen(ae->agent.name)+4;
        } else {
          strncpy(ae->agent.name, value, MAXNAME);
          tmp=strchr(ae->agent.name, ' ');
          if (tmp==NULL) {
            tmp=strchr(ae->agent.name, '\n');
#ifdef WIN32
            if (tmp != NULL)
              *tmp = *(tmp-1) = 0;
#endif
          }
          if ((tmp!=NULL)&&(tmp-ae->agent.name<=MAXNAME))
            *tmp='\0';
          str+=strlen(ae->agent.name)+2;
        }
        break;
    }
  }
  if (check < 3)
    return -1;
  else 
    return 0;
}

int Trace::featurescan(char *cp, double *time, FeatureEvent *fe) {
  char *str=cp;
  char *value;
  int check=0;

  // Initialize default values
  fe->dst=-1;
  fe->feature.expired=0;
  fe->feature.oldvalue[0]='\0';
  fe->feature.value[0]='\0';

  // Parse animation line
  while(str!=NULL) {
    str=strchr(str,' ');
    if (str==NULL)
      break;
    while(*str==' ')
    str++;
    if (*str!='-')
      break;
    str++;
    value=str+2;
    switch (*str) {
      case 't': /*time*/
        check++;
        *time=atof(value);
        str+=2;
        break;
      case 's': /* source node id*/
        check++;
        fe->src=atoi(value);
        str+=2;
        break;
      case 'd': /* destination node id */
        fe->dst=atoi(value);
        str+=2;
        break;
      case 'x': /*this feature is to be deleted*/
        fe->feature.expired=1;
        break;
      case 'T': /*the type of the feature*/
        check++;
        fe->feature.type=*(value);
        str += 2;
        break;
      case 'n': /* name */
        check++;
        str=parse_string_literal(value, fe->feature.name);
        break;
      case 'a': // agent
        check++;
        str = parse_string_literal(value, fe->feature.agent);
        break;
      case 'v': // value (?)
        check++;
        str=parse_string_literal(value, fe->feature.value);
        break;
      case 'o': // old value
        check++;
        str=parse_string_literal(value, fe->feature.oldvalue);
        break;
    }
  }
  if (check < 3)
    return -1;
  else 
    return 0;
}

int Trace::routescan(char *cp, double *time, RouteEvent *re) {
  char *str=cp;
  char *value;
  int check=0;

  // initialize defaults
  re->dst=-1;
  re->route.neg=0;
  re->route.expired=0;

  while(str!=NULL) {
    str=strchr(str,' ');
    if (str==NULL)
      break;
    while(*str==' ')
      str++;
    if (*str!='-')
      break;
    str++;
    value=str+2;
    
    switch (*str) {
      case 't': /*time*/
        check++;
        *time=atof(value);
        str+=2;
        break;
      case 's': /* source node id */
        check++;
        re->src=atoi(value);
        str+=2;
        break;
      case 'd': /* destination */
        check++;
        re->dst=atoi(value);
        str += 2;
        break;
      case 'g': /* multicast group */
        if (*(str+1)=='*')
          re->route.group=-1;
        else 
           re->route.group=atoi(value);
        str+=2;
        break;
      case 'p': /*packet source id*/
        check++;
        if (*(str+1)=='*')
          re->route.pktsrc=-1;
        else 
          re->route.pktsrc=atoi(value);
        str+=2;
        break;
      case 'n': /*negative cache*/
        re->route.neg = 1;
        break;
      case 'x': /*this route just timed out*/
        re->route.expired=1;
        break;
      case 'T': /*timeout*/
        re->route.timeout=atof(value);
        str+=2;
        break;
      case 'm': /* mode - typically iif or oif */
        char *tmp;
        strncpy(re->route.mode, value, MAXVALUE);
        tmp=strchr(re->route.mode, ' ');
        if (tmp==NULL) {
          tmp=strchr(re->route.mode, '\n');
#ifdef WIN32
          if (tmp != NULL) *tmp = *(tmp-1) = 0;
#endif
        }
        if ((tmp!=NULL)&&(tmp-re->route.mode<=MAXVALUE))
          *tmp='\0';
        str+=strlen(re->route.mode)+2;
        break;
    }
  }
  if (check < 4)
    return -1;
  else 
    return 0;
}

//----------------------------------------------------------------------
// void
// Trace::findLastLine()
//   - Go to the specified trace file's last line, verify that
//     the file isn't empty and that its last line is correctly
//     formatted. Compute the time interval for the entire trace
//     assuming it started at time 0.
//----------------------------------------------------------------------
void Trace::findLastLine() {
  off_t pos;

  // should initialize it
  direction_ = FORWARDS;
  pending_.time = 0.0;
  pending_.image[0] = 0;

  /*
   * Fake it?
   */
  if (!nam_stream_->seekable()) {
    maxtime_ = 0;
    return;
  }

  /*
   * If we can, find the end of time.
   */
  char buf[TRACE_LINE_MAXLEN+1];

  /* is there stuff in the file */
  nam_stream_->seek(0, SEEK_END);
  if (nam_stream_->tell() <= 1) {
    fprintf(stderr, "nam: empty trace file `%s'.", fileName_);
    exit(1);
  }

  /*
   * If the file is larger than 'buf', go to the end of the file
   * at the point just large enough to fit into 'buf'.
   */
  if (nam_stream_->tell() > (off_t)(sizeof(buf) - 1)) {
    pos = nam_stream_->seek(- (sizeof(buf) - 1), SEEK_CUR);
    assert(pos != -1);
  } else 
    /* If the file is smaller than 'buf', go to the head of file */
    nam_stream_->seek(0, SEEK_SET);

  int n = nam_stream_->read(buf, sizeof(buf) - 1);
  if (n < 0) {
    perror("read");
    exit(1);
  }
  buf[n] = 0;

  /*
   * This next check is bogus, but zlib fails it.
   * Go figure.
   * Fortunately compressed files should not be seekable.
   */
  pos = nam_stream_->tell();
  assert(pos != -1);

  /* Error if the last line doesn't have '\n' in it. */
  char *cp = strrchr(buf, '\n');
  if (cp == 0) {
    fprintf(stderr, "nam: error, missing newline on the last line of `%s'.",
      fileName_);
    exit(1);
  }

  /*
   * Look for the first '\n' and check if the line following
   * it has the correct type, i.e., it starts with any of the
   * characters in [h+-dv].
   */
  *cp = 0;
  cp = strrchr(buf, '\n');
  if (cp == 0 || strchr("h+-dvrRanlfDEGPAmVNXWT", *++cp) == 0) {
    fprintf(stderr, "nam: error reading last line of `%s'.\n", fileName_);
    //fprintf(stderr, "     Unknown event %s\n", buf);
    exit(1);
  }

  /*
   * Compute the time interval from the beginning of the trace
   * to the end.
   * XXX this should include the duration of the last event but
   *     since we changed the format of 'size' from 'time' to
   *     'bytes', we can no longer figure this out.
   */
  double time = 0.;
  (void)sscanf(++cp, " -t %lg", &time);
  maxtime_ = time;
}

/* Go to the beginning of the file, read the first line and process it. */
void Trace::rewind(long offset)
{
  nam_stream_->seek(offset, SEEK_SET);
  //lineno_ = 0;
  scan();
}

/*
 * Put the specified trace handler to the beginning of the trace 
 * handler list.
 */
void Trace::addHandler(TraceHandler* th)
{
  TraceHandlerList* p = new TraceHandlerList;
  p->th = th;
  p->next = handlers_;
  handlers_ = p;
}

/* Set the current trace time to 'now'. */
void Trace::settime(double now, int timeSliderClicked) {
  if (now < now_) {
#ifdef OLDWAY
    for (TraceHandlerList* p = handlers_; p != 0; p = p->next)
      p->th->reset(now);
    StateInfo si;
    State::instance()->get(now, si);
    rewind(si.offset);
#endif
    direction_=BACKWARDS;
  } else {
    direction_=FORWARDS;
  }

  now_ = now;

  /*
   * Scan the trace file until the event occurring at time 'now'
   * is read. For each event scanned, pass it on to all trace
   * handlers in the trace handler list so they can handle
   * them as needed.
   */
  Tcl& tcl = Tcl::instance();
  Tcl_Interp* interp = tcl.interp();
  static char event[128];
  TraceEvent e;
  while (ReadEvent(now, e)) {
     //if (e.time == TIME_BOF) {
     if (pending_.time == TIME_BOF) {
       // Reached the beginning of file during backtracking
       // Rewind to the beginning
       direction_ = FORWARDS;
       rewind(0);
       continue;
     }

    if (skipahead_mode_) {
      count_ = 0;
      skipahead_mode_ = 0;
      tcl.evalf("%s normalspeed", nam()->name());
    }
     
    // Find Handler for this pending event?
    for (TraceHandlerList* p = handlers_; p != 0; p = p->next)
      p->th->handle(e, now, direction_);

    if (e.tt == 'h' && (e.pe.src == 0 || e.pe.dst == 0)) {
      sprintf(event, "%d %d %.6f %d/", e.pe.src, e.pe.dst, 
                                       e.time, e.pe.pkt.id);
      if (timeSliderClicked)
        tcl.result((const char *)event);
      else
        Tcl_AppendResult(interp, event, 0);
    }
  }

  for (TraceHandlerList* p = handlers_; p != 0; p = p->next)
    p->th->update(now);

  if (Packet::count() == 0 && ++count_ == 2) {
    count_ = 0;
    skipahead_mode_ = 1;
    tcl.evalf("%s speedup %g", nam()->name(), pending_.time);
  }

}

/*
 * Process 'nam trace' commands:
 *   mintime - return the trace start time
 *   maxtime - return the trace duration
 *   connect - add the specified trace handler to the trace handler list
 *   settime - reset current time to specified time and adjust the
 *             display as needed
 *   nxtevent - read next event in trace file and animate from that point
 *   reset    - reset nam tracefile reading
 */
int Trace::command(int argc, const char* const* argv)
{
  Tcl& tcl = Tcl::instance();

  if (argc == 2) {
    if (strcmp(argv[1], "mintime") == 0) {
      char* bp = tcl.buffer();
      sprintf(bp, "%g", mintime_);
      tcl.result(bp);
      return (TCL_OK);
    } else if (strcmp(argv[1], "maxtime") == 0) {
      char* bp = tcl.buffer();
      sprintf(bp, "%g", maxtime_);
      tcl.result(bp);
      return (TCL_OK);
    } else if (strcmp(argv[1], "nxtevent") == 0) {
      char* bp = tcl.buffer();
      sprintf(bp, "%g", pending_.time);
      tcl.result(bp);
      return (TCL_OK);
    } else if (strcmp(argv[1], "reset") == 0) {
      nam_stream_->close();
      nam_stream_ = NULL;
      nam_stream_ = NamStream::open(fileName_);
      if (nam_stream_->is_ok()) {
        delete nam_stream_;
        nam_stream_ = NULL;
        perror("nam: fdopen");
        // exit(1);
        return TCL_ERROR; // tr->valid() will check this
      }
                        return (TCL_OK);
    }
  }
  if (argc == 3) {
    if (strcmp(argv[1], "connect") == 0) {
      /*
       * Get the handler for the specified trace and
       * add it to the trace's list of handlers.
       */
      TraceHandler* th = (TraceHandler*)TclObject::lookup(argv[2]);
      if (th == 0) {
        tcl.resultf("nam connect: no such trace %s",
              argv[2]);
        return (TCL_ERROR);
      }
      addHandler(th);
      return (TCL_OK);
    }
  }
  if (argc == 4 ) {
    if (strcmp(argv[1], "settime") == 0) {
      /*
       * Set current time to the specified time and update
       * the display to reflect the new current time.
       */
      double now = atof(argv[2]);
      int timeSliderClicked = atoi(argv[3]);
      settime(now, timeSliderClicked);
      return (TCL_OK);
    }
  }
  return (TclObject::command(argc, argv));
}
