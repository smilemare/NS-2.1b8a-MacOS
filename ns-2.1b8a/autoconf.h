/* autoconf.h.  Generated automatically by configure.  */
/* -*-	Mode:C++; c-basic-offset:8; tab-width:8; indent-tabs-mode:t -*- */
/*
 * Copyright (c) 1997 University of Southern California.
 * All rights reserved.                                            
 *                                                                
 * Redistribution and use in source and binary forms are permitted
 * provided that the above copyright notice and this paragraph are
 * duplicated in all such forms and that any documentation, advertising
 * materials, and other materials related to such distribution and use
 * acknowledge that the software was developed by the University of
 * Southern California, Information Sciences Institute.  The name of the
 * University may not be used to endorse or promote products derived from
 * this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 *
 */

/* This file should contain variables changed only by autoconf. */

#define NSPERL_PATH "@PERL@"
#define NSTCLSH_PATH "@V_TCLSH@"

/* If you need these from tcl, see the file tcl/lib/ns-autoconf.tcl.in */

/*
 * Put autoconf #define's here to keep them off the command line.
 * see autoconf.info(Header Templates) in the autoconf docs.
 */

/* what does random(3) return? */
#define RANDOM_RETURN_TYPE long

/* type definitions */
/* #undef int8_t */
/* #undef int16_t */
/* #undef int32_t */
/* #undef u_int8_t */
/* #undef u_int16_t */
/* #undef u_int32_t */

/* 64-bits */
#define HAVE_INT64 1
#define SIZEOF_LONG 8
/* #undef int64_t */

/* functions */
#define HAVE_BCOPY 1
#define HAVE_BZERO 1
#define HAVE_GETRUSAGE 1
#define HAVE_SBRK 1
#define HAVE_SNPRINTF 1
#define HAVE_STRTOLL 1
#define HAVE_STRTOQ 1
#define HAVE_ADDR2ASCII 1

/* headers */
#define STDC_HEADERS 1
#define HAVE_STRING_H 1
#define HAVE_STRINGS_H 1
#define HAVE_ARPA_INET_H 1
#define HAVE_NETINET_IN_H 1
#define HAVE_NET_ETHERNET_H 1

/* structures */
/* #undef HAVE_ETHER_HEADER_STRUCT */
/* #undef HAVE_ETHER_ADDRESS_STRUCT */

/* constants */
/* #undef HAVE_SIOCGIFHWADDR */
