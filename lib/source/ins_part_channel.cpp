/* Copyright 2001, Jonas Norberg
Distributed under the terms of the MIT Licence. */

#include <stdlib.h>
#include <string.h>

#include "ins_part_channel.h"
#include "txt.h"
#include "st_version.h"
#include "stlimits.h"

Ins::Ins(){
	name = 0;
	steps = 0;
	filter = 0;
	amp = 0;
};


const Ins &Ins::operator = ( const Ins &i ){
	name = strdup(i.name);
		
	filterpoints = i.filterpoints;
	if (filterpoints < 1){ fprintf(stderr,"filterpoints < 1\n"); return *this;}

#ifdef TARGET_EDITOR
		filter = new TimeLev [ INSPOINTS ];
		memset((void*)filter,0,INSPOINTS * sizeof(TimeLev));
#else
		filter = new TimeLev [ filterpoints ];
#endif
		{
			for (int c1= 0; c1 < filterpoints; c1++){
				filter[c1].time =	i.filter[c1].time;
				filter[c1].lev =	i.filter[c1].lev;
			}
		}
		amppoints = i.amppoints;
		if (amppoints < 1){ fprintf(stderr,"amppoints < 1\n"); return *this;}

#ifdef TARGET_EDITOR
		amp = new TimeLev[ INSPOINTS ];
		memset((void*)amp,0,INSPOINTS * sizeof(TimeLev));
#else
		amp = new TimeLev [ amppoints ];
#endif
		{
			for (int c1 = 0; c1 < amppoints; c1++){
				amp[c1].time =	i.amp[c1].time;
				amp[c1].lev =	i.amp[c1].lev;
			}
		}
		
		filtermode = i.filtermode;
		clipmode = i.clipmode;
		boost = i.boost;
		
		vibs = i.vibs;
		vibd = i.vibd;
		pwms = i.pwms;
		pwmd = i.pwmd;
		res = i.res;
		sps = i.sps;
		if (sps < 1){ fprintf(stderr,"ins.sps < 1\n"); return *this;}
		loop = i.loop;
		len = i.len;

		if (len < 1){ fprintf(stderr,"ins.len < 1\n"); return *this;}
		
#ifdef TARGET_EDITOR
		steps = new InsStep [ INSSTEPS ];
		memset((void*)steps,0,INSSTEPS * sizeof(InsStep));
#else
		steps = new InsStep [ len ];
#endif

		{
			for (int c1=0; c1 < len ; c1++){
				steps[c1].relative = i.steps[c1].relative;
				steps[c1].wform =		i.steps[c1].wform;	
				steps[c1].note = 		i.steps[c1].note;		
				//fprintf(stderr,"%3d - %3d\n", steps[c1].mask, steps[c1].note );
			}
		}
		
	return *this;
};

Ins::~Ins(){
};

void Ins::Free(){
	if (name)	free(name); 
	if (steps)	delete steps;
	if (amp) 	delete amp;
	if (filter) delete filter;
};


void Ins::Save(txt &t){
	t.Comment("\n// INS -------------- ");
	t.Comment(name);
	t.Comment("\n// points - fadsr time-lev\n");
	t.Val(filterpoints);

	{
		for (int c1= 0; c1 < filterpoints; c1++){
			t.Val(filter[c1].time);
			t.Val(filter[c1].lev);
		}
	}

	t.Comment("// points - adsr time-lev\n");
	t.Val(amppoints);

	{
		for (int c1= 0; c1 < amppoints; c1++){
			t.Val(amp[c1].time);
			t.Val(amp[c1].lev);
		}
	}

	t.Comment("// filtermode, clipping\n");
	t.Val(filtermode);
	t.Val((clipmode << 4) | boost);

	t.Comment("// vibs, vibd, pwms, pwmd\n");
	t.Val(vibs);
	t.Val(vibd);
	t.Val(pwms);
	t.Val(pwmd);
	t.Comment("// res\n");
	t.Val(res);
	t.Comment("// sps\n");
	t.Val(sps);
	t.Comment("// len - loop\n");
	t.Val(len);
	t.Val(loop);

	t.Comment("// track mask - note\n");
	
	{
		for (int c1=0; c1 < (len & 0x7f); c1++){
			t.Val(steps[c1].wform | (steps[c1].relative?128:0));
			t.Val(steps[c1].note);
		}
	}
}





status_t Ins::Load(txt &t, uint16 st_version){
		filterpoints = t.NextVal();
		if (filterpoints < 1){ fprintf(stderr,"filterpoints < 1\n");return JNG_ERROR;}

#ifdef TARGET_EDITOR
		filter = new TimeLev [ INSPOINTS ];
		memset((void*)filter,0,INSPOINTS * sizeof(TimeLev));
#else
		filter = new TimeLev [ filterpoints ];
#endif
		{
			for (int c1= 0; c1 < filterpoints; c1++){
				filter[c1].time =	t.NextVal();
				filter[c1].lev =		t.NextVal();
			}
		}

		amppoints = t.NextVal();
		if (amppoints < 1){ fprintf(stderr,"amppoints < 1\n"); return JNG_ERROR;}

#ifdef TARGET_EDITOR
		amp = new TimeLev[ INSPOINTS ];
		memset((void*)amp,0,INSPOINTS * sizeof(TimeLev));
#else
		amp = new TimeLev [ amppoints ];
#endif
		{
			for (int c1 = 0; c1 < amppoints; c1++){
				amp[c1].time =	t.NextVal();
				amp[c1].lev =	t.NextVal();
			}
		}
		
		filtermode = t.NextVal();
		clipmode = t.NextVal();
		boost = clipmode & 15;
		clipmode >>= 4;
		
		vibs = t.NextVal();
		vibd = t.NextVal();
		pwms = t.NextVal();
		pwmd = t.NextVal();
		res = t.NextVal();
		sps = t.NextVal();
		if (sps < 1){ fprintf(stderr,"ins.sps < 1\n"); return JNG_ERROR;}
		
		if (st_version < 900){
			uint8 tmp = t.NextVal();
			len = tmp & 127;
			loop = (tmp & 1)?0:(len-1);
		}else{
			len = t.NextVal();
			loop = t.NextVal();
			if (loop >= len){ fprintf(stderr,"ins.loop(%d) >= ins.len(%d)\n",loop,len); return JNG_ERROR;}
		}

      if (len < 1){ fprintf(stderr,"ins.len < 1\n"); return JNG_ERROR;}
		
#ifdef TARGET_EDITOR
		steps = new InsStep [ INSSTEPS ];
		{
			for (int c = 0 ; c < INSSTEPS ; c++){
				steps[c].relative = true;
				steps[c].wform =		0;
				steps[c].note = 		0;			
			}
		}
#else
		steps = new InsStep [ len ];
#endif

		{
			for (int c1=0; c1 < len ; c1++){
				uint8 temp = t.NextVal();
				steps[c1].relative = (temp & 0x80) != 0;
				steps[c1].wform =		(temp & 0xf);
				steps[c1].note = 		t.NextVal();
				//fprintf(stderr,"%3d - %3d\n", steps[c1].mask, steps[c1].note );
			}
		}

	return JNG_OK;
}







/*_EXPORT*/ void Ins::SetName(const char *iname){
	if (name) free(name);
	if (iname == NULL){
		name = strdup("");
		return;
	};
	name = strdup(iname);
}

Part::Part(){ name = 0; };
Part::~Part(){ if (name) free(name); };

/*_EXPORT*/ void Part::SetName(const char *iname){
	if (name) free(name);
	if (iname == NULL){
		name = strdup("");
		return;
	};
	name = strdup(iname);
}

Channel::Channel(){};
Channel::~Channel(){};

