/* -*-	Mode:C++; c-basic-offset:8; tab-width:8; indent-tabs-mode:t -*- */
/*
 * Copyright (c) Xerox Corporation 1997. All rights reserved.
 *  
 * License is granted to copy, to use, and to make and to use derivative
 * works for research and evaluation purposes, provided that Xerox is
 * acknowledged in all documentation pertaining to any such copy or derivative
 * work. Xerox grants no other licenses expressed or implied. The Xerox trade
 * name should not be used in any advertising without its written permission.
 *  
 * XEROX CORPORATION MAKES NO REPRESENTATIONS CONCERNING EITHER THE
 * MERCHANTABILITY OF THIS SOFTWARE OR THE SUITABILITY OF THIS SOFTWARE
 * FOR ANY PARTICULAR PURPOSE.  The software is provided "as is" without
 * express or implied warranty of any kind.
 *  
 * These notices must be retained in any copies of any part of this software.
 */

#ifndef lint
static const char rcsid[] =
    "@(#) $Header: /nfs/jade/vint/CVSROOT/ns-2/realaudio/realaudio.cc,v 1.4 2000/11/14 01:48:56 kclan Exp $ (USC/ISI)";
#endif

#ifndef WIN32 
#include <sys/time.h>
#endif
#include "random.h"
#include "trafgen.h"
#include "ranvar.h"

class RA_Traffic : public TrafficGenerator {
 public:
	RA_Traffic();
	int loadCDF(const char* filename);
        int lookup(double u);
	virtual double value();
	virtual double interpolate(double u, double x1, double y1, double x2, double y2);
	virtual double next_interval(int&);

 protected:
	void init();
	double ontime_;  /* average length of burst (sec) */
	double offtime_; /* average idle period (sec) */
	double rate_;    /* send rate during burst (bps) */
	double interval_; /* inter-packet time at burst rate */
	double burstlen_; /* average # packets/burst */
	unsigned int rem_; /* number of packets remaining in current burst */
        double minCDF_;         // min value of the CDF (default to 0)
	double maxCDF_;         // max value of the CDF (default to 1)
	int interpolation_;     // how to interpolate data (INTER_DISCRETE...)
	int numEntry_;          // number of entries in the CDF table
	int maxEntry_;          // size of the CDF table (mem allocation)
	CDFentry* table_;       // CDF table of (val_, cdf_)
	RNG* rng_;

//        EmpiricalRandomVariable Offtime_ ;
//      EmpiricalRandomVariable Ontime_ ;
};


static class RATrafficClass : public TclClass {
 public:
	RATrafficClass() : TclClass("Application/Traffic/RealAudio") {}
 	TclObject* create(int, const char*const*) {
		return (new RA_Traffic());
	}
} class_ra_traffic;

RA_Traffic::RA_Traffic() : minCDF_(0), maxCDF_(1), maxEntry_(32), table_(0)
{
	bind("minCDF_", &minCDF_);
	bind("maxCDF_", &maxCDF_);
	bind("interpolation_", &interpolation_);
	bind("maxEntry_", &maxEntry_);
	bind_time("burst_time_", &ontime_);
	bind_time("idle_time_", &offtime_);
	bind_bw("rate_", &rate_);
	bind("packetSize_", &size_);

	rng_ = RNG::defaultrng();
}

void RA_Traffic::init()
{
        int res = loadCDF("offtimecdf");
//      int res1 = Ontime_.loadCDF("ontimecdf");
        timeval tv;
	gettimeofday(&tv, 0);

        if (res < 0)  printf("error:unable to load offtimecdf");

	interval_ = ontime_ ;
	burstlen_ = (double) ( rate_ * (ontime_ + offtime_))/ (double)(size_ << 3);
	rem_ = 0;
	if (agent_)
		agent_->set_pkttype(PT_REALAUDIO);
}

double RA_Traffic::next_interval(int& size)
{
//	double t = Ontime_.value() ;
	double o = value() ; //off-time is taken from CDF

	double t = ontime_ ; //fixed on-time
//	double o = offtime_ ;

//	o = o * ran ;

	if (rem_ == 0) {
		/* compute number of packets in next burst */
		rem_ = int(burstlen_ + .5);

		//recalculate the number of packet sent during ON-period if
		// off-time is mutliple of 1.8s
		if (o > offtime_ ) {
	           double b = 
		   // (double) ( rate_ * (ontime_ + o))/ (double)(size_ << 3);
		   (double) ( rate_ * (t + o))/ (double)(size_ << 3);
		   rem_ = int(b);
		}

//		if (ran > 0.8 ) rem_++ ;
//		if (ran < 0.2 ) rem_-- ;

		/* make sure we got at least 1 */
		if (rem_<= 0)
			rem_ = 1;	
		/* start of an idle period, compute idle time */
		t += o ;
	}	
	rem_--;

	size = size_;
	return(t);

}

int RA_Traffic::loadCDF(const char* filename)
{
	FILE* fp;
	char line[256];
	CDFentry* e;

	fp = fopen(filename, "r");
	if (fp == 0)
		return 0;

	if (table_ == 0)
		table_ = new CDFentry[maxEntry_];
	for (numEntry_=0;  fgets(line, 256, fp);  numEntry_++) {
		if (numEntry_ >= maxEntry_) {	// resize the CDF table
			maxEntry_ *= 2;
			e = new CDFentry[maxEntry_];
			for (int i=numEntry_-1; i >= 0; i--)
				e[i] = table_[i];
			delete table_;
			table_ = e;
		}
		e = &table_[numEntry_];
		// Use * and l together raises a warning
		sscanf(line, "%lf %*f %lf", &e->val_, &e->cdf_);
	}
	return numEntry_;
}

double RA_Traffic::value()
{
	if (numEntry_ <= 0)
		return 0;
	double u = rng_->uniform(minCDF_, maxCDF_);
	int mid = lookup(u);
	if (mid && interpolation_ && u < table_[mid].cdf_)
		return interpolate(u, table_[mid-1].cdf_, table_[mid-1].val_,
				   table_[mid].cdf_, table_[mid].val_);
	return table_[mid].val_;
}

double RA_Traffic::interpolate(double x, double x1, double y1, double x2, double y2)
{
	double value = y1 + (x - x1) * (y2 - y1) / (x2 - x1);
	if (interpolation_ == INTER_INTEGRAL)	// round up
		return ceil(value);
	return value;
}

int RA_Traffic::lookup(double u)
{
	// always return an index whose value is >= u
	int lo, hi, mid;
	if (u <= table_[0].cdf_)
		return 0;
	for (lo=1, hi=numEntry_-1;  lo < hi; ) {
		mid = (lo + hi) / 2;
		if (u > table_[mid].cdf_)
			lo = mid + 1;
		else hi = mid;
	}
	return lo;
}
