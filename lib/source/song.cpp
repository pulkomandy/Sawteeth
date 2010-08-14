/* Copyright 2001, Jonas Norberg
Distributed under the terms of the MIT Licence. */

#include <string.h>
#include <stdlib.h>
#include <math.h>

#include "song.h"
#include "txt.h"
#include "player.h"

void *CALLOC(int s1, int s2);
void *CALLOC(int s1, int s2)
{
	char *tmp = (char*)malloc(s1 * s2);
	for (int c = 0; c< (s1*s2) ; c++) tmp[c]=0;
	return (void*)tmp;
}

void Song::Init()
{
	//////////////////////////////////////////////////////// calc-multiplyers
	const float floor = (float)0.1;
	{
		for (uint32 c = 0 ; c < CHN ; c++){
			cmul[c]=((1-floor)/(float)c)+floor;
			//printf("[%ld] == %f\n",c,cmul[c]);
		}
	}

	//////////////////////////////////////////////////////// FREQ-TABLES
	const double MUL = 1.0594630943593;

	N2F = (float *) malloc(22*12*sizeof(float));
	R2F = (float *) malloc(22*12*sizeof(float));

	int count = 0;
	double octbase=1.02197486445547712033;

	{
		for (int oc=0;oc<22;oc++){
			double base=octbase;
			for (int c=0;c<12;c++){
				N2F[count++] = (float)base;
				base *= MUL;
			}
			octbase *= 2;
		}
	}

	count = 0;
	octbase = 1;
	stereobuffer = 0;
	 	
	{
		for (int oc=0;oc<22;oc++){
			double base=octbase;
			for (int c=0;c<12;c++){
				R2F[count++] = (float)base;
				base *= MUL;
			}
			octbase *= 2;
		}
	}

	//////////////////////////////////////////////////////// END OF FREQ-TABLES

	channelcount = 0;
	partcount = 0;
	instrumentcount = 0;
	breakpcount = 0;

	looped = false;
	p = 0;
	chan = 0;
	parts = 0;
	ins = 0;
	name = 0;
	author = 0;
	breakpoints = 0;
	PALs = 0;
	stereobuffer = 0;

#ifdef TARGET_EDITOR
	pp = 0;
#endif

}

/*_EXPORT*/ Song::Song(const Song &s)
{
	Init();
	init = JNG_OK;

	
/////////////////////////////////////////////////////////////////////////////////// COPY COPY COPY

	uint32 c, c1;
	
	st_version = s.st_version;
	spspal = s.spspal;
	if (spspal < 1){
		fputs("spspal < 1\n",stderr);
		init = JNG_ERROR;
	}

	//////////////////////////////////////////////////////// CHANNELS
//fprintf(stderr,"CHANNELS\n");
	channelcount = s.channelcount;		// no. of channels
	if (channelcount < 1){
		fputs("channelcount < 1\n",stderr);
		init = JNG_ERROR;
		return;
	}

#ifdef TARGET_EDITOR
	chan = (Channel*) CALLOC( CHN , sizeof(Channel));	// allocates more space for editor
#else
	chan = (Channel*) CALLOC( channelcount , sizeof(Channel));
#endif

	for (c=0; c < channelcount; c++){
		chan[c].left = s.chan[c].left;
		chan[c].right = s.chan[c].right;
		chan[c].len = s.chan[c].len;
		chan[c].lloop = s.chan[c].lloop;
		chan[c].rloop = s.chan[c].rloop;
		if (1 > chan[c].len){
			fprintf(stderr,"chan[%ld].len < 1\n",c);
			init=JNG_ERROR;
			return;
		}
		
		//	seq
#ifdef TARGET_EDITOR
		chan[c].steps = (ChStep*) CALLOC( CHNSTEPS , sizeof(ChStep));
#else
		chan[c].steps = (ChStep*) CALLOC( chan[c].len , sizeof(ChStep));
#endif

		for (c1=0; c1 < chan[c].len; c1++){
			chan[c].steps[c1].part = s.chan[c].steps[c1].part;
			chan[c].steps[c1].transp = s.chan[c].steps[c1].transp;
			chan[c].steps[c1].damp = s.chan[c].steps[c1].damp;
		}
	}
	
	//////////////////////////////////////////////////////// PARTS
//fprintf(stderr,"PARTS\n");
	partcount = s.partcount;
	if (1 > partcount){fputs("partcount < 1\n",stderr);init=JNG_ERROR;return;}

#ifdef TARGET_EDITOR
	parts = (Part*) CALLOC( PARTS , sizeof(Part));	// allocates more space for editor
#else
	parts = (Part*) CALLOC( partcount , sizeof(Part));
#endif

	for (c=0; c < partcount; c++){
		parts[c].sps = s.parts[c].sps;
		if (parts[c].sps < 1){
			fprintf(stderr,"parts[%ld].sps < 1\n",c);
			init = JNG_ERROR;
			return;
		}

		parts[c].len = s.parts[c].len;
		if (parts[c].len < 1){
			fprintf(stderr,"parts[%ld].len < 1\n",c);
			init = JNG_ERROR;
			return;
		}

#ifdef TARGET_EDITOR
		parts[c].steps = (Step*) CALLOC( PARTSTEPS , sizeof(Step));
#else
		parts[c].steps = (Step*) CALLOC( parts[c].len , sizeof(Step));
#endif

		for (c1=0; c1 < parts[c].len; c1++){
			parts[c].steps[c1].ins = s.parts[c].steps[c1].ins;
			parts[c].steps[c1].eff = s.parts[c].steps[c1].eff;
			parts[c].steps[c1].note = s.parts[c].steps[c1].note;
//			fprintf(stderr,"%3d - %3d - %3d\n", parts[c].steps[c1].ins, parts[c].steps[c1].eff, parts[c].steps[c1].note );
		}
	}
	
	
	//////////////////////////////////////////////////////// INSTRUMENTS
//fprintf(stderr,"INS\n");

	instrumentcount = s.instrumentcount;
	if (instrumentcount < 2){
		fprintf(stderr,"instrumentcount < 1\n");
		init = JNG_ERROR;
		return;
	}

#ifdef TARGET_EDITOR
	ins = (Ins*) CALLOC( INS , sizeof(Ins));	// allocates more space for editor
#else
	ins = (Ins*) CALLOC( instrumentcount , sizeof(Ins));
#endif

// allocate dummy instrument
	ins[0].name = strdup("");
	ins[0].filterpoints	= 1;
	ins[0].amppoints	= 1;
	ins[0].filter		= (TimeLev*) CALLOC( 1 , sizeof(TimeLev));
	ins[0].amp			= (TimeLev*) CALLOC( 1 , sizeof(TimeLev));

	ins[0].filtermode = 0;
	ins[0].clipmode = 0;
	ins[0].boost = 1;

	ins[0].sps = 30;
	ins[0].res = 0;

	ins[0].vibs = 1;
	ins[0].vibd= 1;

	ins[0].pwms= 1;
	ins[0].pwmd= 1;

	
	ins[0].len = 1;
	ins[0].loop = 0;
	
	ins[0].steps = (InsStep*) CALLOC(1 , sizeof(InsStep));
// end of dummy ins allocation


	for (c = 1; c < instrumentcount; c++){
		
		ins[c].filterpoints = s.ins[c].filterpoints;
		if (ins[c].filterpoints < 1){ fprintf(stderr,"ins[%ld].filterpoints < 1\n",c);init = JNG_ERROR;return;}

#ifdef TARGET_EDITOR
		ins[c].filter = (TimeLev*) CALLOC( INSPOINTS , sizeof(TimeLev));
#else
		ins[c].filter = (TimeLev*) CALLOC( ins[c].filterpoints , sizeof(TimeLev));
#endif
		for (c1= 0; c1 < ins[c].filterpoints; c1++){
			ins[c].filter[c1].time =	s.ins[c].filter[c1].time;
			ins[c].filter[c1].lev =		s.ins[c].filter[c1].lev;
		}
		
		ins[c].amppoints = s.ins[c].amppoints;
		if (ins[c].amppoints < 1){ fprintf(stderr,"ins[%ld].amppoints < 1\n",c);init = JNG_ERROR;return;}

#ifdef TARGET_EDITOR
		ins[c].amp = (TimeLev*) CALLOC( INSPOINTS , sizeof(TimeLev));
#else
		ins[c].amp = (TimeLev*) CALLOC( ins[c].amppoints , sizeof(TimeLev));
#endif
		for (c1= 0; c1 < ins[c].amppoints; c1++){
			ins[c].amp[c1].time =	s.ins[c].amp[c1].time;
			ins[c].amp[c1].lev =		s.ins[c].amp[c1].lev;
		}
		
		ins[c].filtermode =	s.ins[c].filtermode;
		ins[c].clipmode = 	s.ins[c].clipmode;
		ins[c].boost = 		s.ins[c].boost;
		ins[c].vibs = 			s.ins[c].vibs;
		ins[c].vibd = 			s.ins[c].vibd;
		ins[c].pwms =			s.ins[c].pwms;
		ins[c].pwmd =			s.ins[c].pwmd;
		ins[c].res =			s.ins[c].res;
		ins[c].sps =			s.ins[c].sps;
		if (ins[c].sps < 1){ fprintf(stderr,"ins[%ld].sps < 1\n",c); init = JNG_ERROR; return;}
		ins[c].len =			s.ins[c].len;
		ins[c].loop =			s.ins[c].loop;

      if (ins[c].len < 1){ fprintf(stderr,"ins[%ld].len < 1\n",c); init = JNG_ERROR; return;}
		
#ifdef TARGET_EDITOR
		ins[c].steps = (InsStep*) CALLOC( INSSTEPS , sizeof(InsStep));
#else
		ins[c].steps = (InsStep*) CALLOC( (ins[c].len) , sizeof(InsStep));
#endif

		for (c1=0; c1 < (uint32)(ins[c].len & 0x7f); c1++){
			ins[c].steps[c1].relative =	s.ins[c].steps[c1].relative;
			ins[c].steps[c1].wform =		s.ins[c].steps[c1].wform;
			ins[c].steps[c1].note =			s.ins[c].steps[c1].note;
			//fprintf(stderr,"%3d - %3d\n", ins[c].steps[c1].mask, ins[c].steps[c1].note );
		}
	}
	
	/////////////////// Load BreakPoints
	breakpcount = s.breakpcount;

#ifdef TARGET_EDITOR
	breakpoints = (BreakPoint*) CALLOC( BREAKPOINTS , sizeof(BreakPoint*));
#else
	breakpoints = (BreakPoint*) CALLOC( breakpcount , sizeof(BreakPoint*));
#endif

	{
		for (int c = 0 ; c < breakpcount ; c++){
			breakpoints[c].PAL =			s.breakpoints[c].PAL;
			breakpoints[c].command =	s.breakpoints[c].command;
		}
	}

	/////////////////// Load Names
	SetName(s.name);
	SetAuthor(s.author);
//	printf("songname = %s\n",name);

	{
		for (int c = 0 ; c < partcount ; c++){
			parts[c].SetName(s.parts[c].name);
//			printf("part[%d] = %s\n",c,parts[c].name);
		}
	}

	{
		for (int c = 1 ; c < instrumentcount ; c++){
			ins[c].SetName(s.ins[c].name);
//			printf("ins[%d] = %s\n",c,ins[c].name);
		}
	}

	/////////////////// Create Playaz
#ifdef TARGET_EDITOR
	p = (Player **)CALLOC(CHN , sizeof(Player *));
#else
	p = (Player **)CALLOC(channelcount , sizeof(Player *));
#endif
	{
		for (int c = 0; c< channelcount; c++){
			p[c] = new Player(this,chan + c);
		}		
	}

/////////////////////////////////////////////////////////////////////////////////// COPY COPY COPY

	stereobuffer = new float[spspal * 2];
	bs = stereobuffer;
	
	///////////////////////////////////////	partplayer
#ifdef TARGET_EDITOR
		pchan.steps = &pchanstep;
		pchan.len = 1;
		pchan.rloop = 0;
		pchan.lloop = 0;
		pchanstep.part = 0;
		pchanstep.transp = 0;
		pp = new Player(this,&pchan);
		pbs = stereobuffer;
#endif

	SetPos(0);
	return;

}

/*_EXPORT*/ Song::Song(txt &flat)
{
	Init();
	init = JNG_OK;
	
	///////////////////////////////////////	här allokeras elände till sången...
	
	if (JNG_OK != (init = Load(flat))) return;

	stereobuffer = new float[spspal * 2];
	bs = stereobuffer;

	///////////////////////////////////////	partplayer
#ifdef TARGET_EDITOR
		pchan.steps = &pchanstep;
		pchan.len = 1;
		pchan.rloop = 0;
		pchan.lloop = 0;
		pchanstep.part = 0;
		pchanstep.transp = 0;
		pp = new Player(this,&pchan);
		pbs = stereobuffer;
#endif

	SetPos(0);
	return;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*_EXPORT*/ status_t Song::Load(txt &t)
{
#ifdef TARGET_EDITOR
	FreeSong();
#endif
	uint32 c, c1, num;
	if (JNG_OK!=t.InitCheck()){
		init=JNG_ERROR;
		return init;
	};

	fprintf(stderr,"Loading %s\n",t.FileName());
	
	t.IsBin(true); // för att kunna läsa in första fyra
	num = t.NextVal(4);
//fprintf(stderr,"<%c%c%c%c>\n",num>>24,num>>16 & 255,num>>8 & 255,num & 255);
	switch (num){
		case 'SWTD': 		break;
		case 'SWTT': t.IsBin(false);	break;
		default : return init=JNG_ERROR;
	}
	

	st_version = t.NextVal(2);

	fprintf(stderr,"file_version = %d\n",st_version);
	
	if (st_version > ST_CURRENT_FILE_VERSION){
		fprintf(stderr,"Loader version %2f < song version %2f (0x%x)\n",(float)ST_CURRENT_FILE_VERSION/1000.0, (float)st_version/1000.0, st_version);
		return init = JNG_ERROR_VERSION;
	}
		
	if (st_version < 900 )			// special-hack för kompatibilitet med CLOSED_BETA
		spspal = 882;
	else
		spspal = t.NextVal(2);
		

	if (spspal < 1){
		fputs("spspal < 1\n",stderr);
		return init = JNG_ERROR;
	}

	//////////////////////////////////////////////////////// CHANNELS
//fprintf(stderr,"CHANNELS\n");
	channelcount = t.NextVal();		// no. of channels
	if (channelcount < 1){
		fputs("channelcount < 1\n",stderr);
		return init = JNG_ERROR;
	}

#ifdef TARGET_EDITOR
	chan = (Channel*) CALLOC( CHN , sizeof(Channel));	// allocates more space for editor
#else
	chan = (Channel*) CALLOC( channelcount , sizeof(Channel));
#endif

	for (c=0; c < channelcount; c++){
		chan[c].left = t.NextVal();
		chan[c].right = t.NextVal();
		chan[c].len = t.NextVal(2);

		// previous we did not have loop points
		if (st_version < 910){
			chan[c].lloop = 0;
		}else{
			chan[c].lloop = t.NextVal(2);
		}
		
		// previous we did not have right-loop points
		if (st_version < 1200)
			chan[c].rloop = chan[c].len-1;
		else
			chan[c].rloop = t.NextVal(2);
			
		if (1 > chan[c].len){
			fprintf(stderr,"chan[%ld].len < 1\n",c);
			init=JNG_ERROR;
			return init;
		}

		if (CHNSTEPS < chan[c].len){
			fprintf(stderr,"chan[%d].len > %ld\n",chan[c].len,CHNSTEPS);
			init=JNG_ERROR;
			return init;
		}

		if (chan[c].rloop >= chan[c].len){
			fprintf(stderr,"chan[%ld].rloop >= chan[%ld].len\n",c,c);
			chan[c].rloop = chan[c].len-1;
		}

		
		//	seq
#ifdef TARGET_EDITOR
		chan[c].steps = (ChStep*) CALLOC( CHNSTEPS , sizeof(ChStep));
#else
		chan[c].steps = (ChStep*) CALLOC( chan[c].len , sizeof(ChStep));
#endif

		{
			for (c1=0; c1 < chan[c].len; c1++){
				chan[c].steps[c1].part = t.NextVal();
				chan[c].steps[c1].transp = t.NextVal();
				chan[c].steps[c1].damp = t.NextVal();
			}
		}
	}
	
	//////////////////////////////////////////////////////// PARTS
//fprintf(stderr,"PARTS\n");
	partcount = t.NextVal();
	if (1 > partcount){fputs("partcount < 1\n",stderr);init=JNG_ERROR;return init;}

#ifdef TARGET_EDITOR
	parts = (Part*) CALLOC( PARTS , sizeof(Part));	// allocates more space for editor
#else
	parts = (Part*) CALLOC( partcount , sizeof(Part));
#endif

	for (c=0; c < partcount; c++){
		parts[c].sps = t.NextVal();
		if (parts[c].sps < 1){
			fprintf(stderr,"parts[%ld].sps < 1\n",c);
			return init = JNG_ERROR;
		}

		parts[c].len = t.NextVal();
		if (parts[c].len < 1){
			fprintf(stderr,"parts[%ld].len < 1\n",c);
			return init = JNG_ERROR;
		}

#ifdef TARGET_EDITOR
		parts[c].steps = (Step*) CALLOC( PARTSTEPS , sizeof(Step));
#else
		parts[c].steps = (Step*) CALLOC( parts[c].len , sizeof(Step));
#endif

		for (c1=0; c1 < parts[c].len; c1++){
			parts[c].steps[c1].ins = t.NextVal();
			parts[c].steps[c1].eff = t.NextVal();
			parts[c].steps[c1].note = t.NextVal();
//			fprintf(stderr,"%3d - %3d - %3d\n", parts[c].steps[c1].ins, parts[c].steps[c1].eff, parts[c].steps[c1].note );
		}
	}

	// testa om man laddat in nått i kanalerna som refererar till en part som inte finns
	for (c = 0 ; c < channelcount ; c++){	
		for (c1=0; c1 < chan[c].len; c1++){
			if (chan[c].steps[c1].part >= partcount){
				fprintf(stderr,"Warning Clamped a part at index %d in channel %d\n",c1,c);
				chan[c].steps[c1].part = partcount -1;
			}
		}
	}
		
		
	//////////////////////////////////////////////////////// INSTRUMENTS
//fprintf(stderr,"INS\n");

	instrumentcount = 1+t.NextVal();
	if (instrumentcount < 2){
		fprintf(stderr,"instrumentcount < 1\n");
		return init = JNG_ERROR;
	}

#ifdef TARGET_EDITOR
	ins = (Ins*) CALLOC( INS , sizeof(Ins));	// allocates more space for editor
#else
	ins = (Ins*) CALLOC( instrumentcount , sizeof(Ins));
#endif

// allocate dummy instrument, is this really needed nowdays?
	ins[0].name = strdup("");
	ins[0].filterpoints	= 1;
	ins[0].amppoints	= 1;
	ins[0].filter		= (TimeLev*) CALLOC( 1 , sizeof(TimeLev));
	ins[0].amp			= (TimeLev*) CALLOC( 1 , sizeof(TimeLev));

	ins[0].filtermode = 0;
	ins[0].clipmode = 0;
	ins[0].boost = 1;

	ins[0].sps = 30;
	ins[0].res = 0;

	ins[0].vibs = 1;
	ins[0].vibd = 1;

	ins[0].pwms = 1;
	ins[0].pwmd = 1;

	
	ins[0].len = 1;
	ins[0].loop = false;
	
	ins[0].steps = (InsStep*) CALLOC(1 , sizeof(InsStep));
// end of dummy ins allocation
	

	for (c = 1; c < instrumentcount; c++)
		if (JNG_OK !=ins[c].Load(t, st_version)){
			fprintf(stderr,"instrumentcount load error ins[%ld]\n",c);
			return init = JNG_ERROR;
		}

	
	/////////////////// Load BreakPoints
	breakpcount = 0;
	breakpcount = t.NextVal();

#ifdef TARGET_EDITOR
		breakpoints = (BreakPoint*) CALLOC( BREAKPOINTS , sizeof(BreakPoint*));
#else
		breakpoints = (BreakPoint*) CALLOC( breakpcount , sizeof(BreakPoint*));
#endif

	{
		for (int c = 0 ; c < breakpcount ; c++){
			breakpoints[c].PAL = t.NextVal(4);
			breakpoints[c].command = t.NextVal(4);
		}
	}
	
	/////////////////// Load Names
	name = strdup(t.NextString());
	author = strdup(t.NextString());
//	printf("songname = %s\n",name);

	{
		for (int c = 0 ; c < partcount ; c++){
			parts[c].SetName(t.NextString());
//			printf("part[%d] = %s\n",c,parts[c].name);
		}
	}
	{
		for (int c = 1 ; c < instrumentcount ; c++){
			ins[c].SetName(t.NextString());
//			printf("ins[%d] = %s\n",c,ins[c].name);
		}
	}

	/////////////////// Create Playaz
#ifdef TARGET_EDITOR
	p = (Player **)CALLOC(CHN , sizeof(Player *));
#else
	p = (Player **)CALLOC(channelcount , sizeof(Player *));
#endif
	{
		for (int c = 0; c< channelcount; c++){
			p[c] = new Player(this,chan + c);
		}		
	}

	return InitCheck();
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*_EXPORT*/ status_t Song::InitCheck(){
// check for big instruments
	for (int c = 0 ; c < partcount ; c++){
		for (int c2 = 0 ; c2 < parts[c].len ; c2++){
			if (parts[c].steps[c2].ins >= instrumentcount){
				fprintf(stderr,"ERROR-Ins-# too big: part #%d - row #%d\n",c,c2);
				return JNG_ERROR;
			}
		}
	}

	return init;
}

/*_EXPORT*/ void Song::FreeSong()
{
#ifdef TARGET_EDITOR
	if (pp) delete pp;
#endif

	int c;
	//////////////////////////////////////////////////////// CHANNELS
	for (c=0; c < channelcount; c++)
	{
		if (p) if (p[c]) delete p[c];
		if (chan[c].steps) free( chan[c].steps );
	}

	if (chan){
		free( chan );
		if (p) free( p );
		chan = 0L;
		channelcount = 0;
	}
	
	//////////////////////////////////////////////////////// PARTS
	for (c=0; c < partcount; c++){
		if ( parts[c].name ) free( parts[c].name );
		if ( parts[c].steps )free( parts[c].steps );
	}

	if (parts){
		free( parts );
		parts = 0L;
		partcount = 0;
	}
	
	//////////////////////////////////////////////////////// INSTRUMENTS
	for (c = 0; c < instrumentcount; c++) ins[c].Free();
	
	if (ins){
		free( ins );
		ins = 0L;
		instrumentcount = 0;
	}
	
	
	//////////////////////////////////////////////////////// SONG STUFF
	if (name) free( name );
	if (author) free( author );
	
	if (breakpoints) free( breakpoints );
	breakpcount = 0;
}

/*_EXPORT*/ Song::~Song()
{
	if (0 != stereobuffer) delete stereobuffer;

	free( N2F );
	free( R2F );
	FreeSong();
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*_EXPORT*/ bool Song::Play(	float *buff,
					uint32 FrameCount	)
{
// ----------------------
// SoundPart
//fprintf(stderr, "\nSong::Play()\n");

	float *bd = buff;
//	float *bs = p[0]->Buffer();

// skall köra allas buffrar
	while (FrameCount > 0){
//fprintf(stderr, "\t[B]samplecount:%d, smp_left:%d, bs:%p\n\n", SampleCount, smp_left, bs);
		uint32 lim;
		if (FrameCount < smp_left){
			lim = FrameCount;
			FrameCount = 0;
			smp_left -= lim;
		}else{
			lim = smp_left;
			smp_left = 0;
			FrameCount -= lim;
		}
		
		lim <<=1; // stereo
		while ( lim ){
			*bd = *bs;
			bd++;
			bs++;
			lim--;
		}
//fprintf(stderr, "\t[A]samplecount:%d, smp_left:%d, bs:%p\n\n", SampleCount, smp_left, bs);
		
		
		float channelmul = cmul[channelcount] / (float)255.0;

		// a bit optimized
		if (smp_left < 1){
			//////////// NextBuffer and move the first one found
			int c = 0;
			for (; c < channelcount ; c++){
				if ( p[c]->NextBuffer() ){
					MemMulMove(stereobuffer,p[c]->Buffer(), spspal,
						(float)chan[c].left  * channelmul ,
						(float)chan[c].right * channelmul);
					break;
				}
			}			
			c++;

			// after that, add the next buffers
			if (c > channelcount )
				memset( stereobuffer, 0, spspal * sizeof(float) * 2);
			else{
				for (; c < channelcount ; c++){
					if (p[c]->NextBuffer())
						MemMulAdd( stereobuffer, p[c]->Buffer(), spspal,
						(float)chan[c].left  * channelmul ,
						(float)chan[c].right * channelmul );
				}
			}

			//////////// PAL looping
			PALs++;
			if (p[0]->Looped()){
				looped = true;
				PALs = 0;
				for ( int cnt = 0 ; cnt < chan[0].lloop ; cnt++){
					PALs += ( parts[chan[0].steps[cnt].part].sps * parts[chan[0].steps[cnt].part].len );
				}
//				printf("PALs == %d\n",PALs);
			}

			smp_left = spspal;
			bs = stereobuffer;
		}
		
	}
	
// ----------------------
// BreakPointPart
	if (looped){
		looped = false;
		return true;
	} else return false;
}


inline void Song::MemMulMove( float *d, float *s, uint32 count, float l, float r)
{
//	fprintf(stderr,"%f,%f\n",l,r);
	while ( count > 4 ){
		d[0] = s[0] * l;
		d[1] = s[0] * r;
		d[2] = s[1] * l;
		d[3] = s[1] * r;
		d[4] = s[2] * l;
		d[5] = s[2] * r;
		d[6] = s[3] * l;
		d[7] = s[3] * r;
		d+=8;
		s+=4;
		count -= 4;
	}

	while ( count-- ){
		d[0] = *s * l;
		d[1] = *s * r;
		d+=2;
		s++;
	}
}

inline void Song::MemMulAdd( float *d, float *s, uint32 count, float l, float r)
{
//	fprintf(stderr,"%f,%f\n",l,r);
	while ( count > 4 ){
		d[0] += s[0] * l;
		d[1] += s[0] * r;
		d[2] += s[1] * l;
		d[3] += s[1] * r;
		d[4] += s[2] * l;
		d[5] += s[2] * r;
		d[6] += s[3] * l;
		d[7] += s[3] * r;
		d+=8;
		s+=4;
		count -= 4;
	}

	while ( count-- ){
		d[0] += *s * l;
		d[1] += *s * r;
		d+=2;
		s++;
	}
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*_EXPORT*/ uint32 Song::GetPos()
{
	return PALs;
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*_EXPORT*/ void Song::SetPos(uint32 pals)
{
	smp_left = 0;
	PALs = pals;

	for (int c=0;c<channelcount;c++){
		// set seqcount
		int32	sum = 0;
		int32	seqcount = 0, stepcount = 0, palcount = 0, sps = 0, len = 0;
		
		while (sum <= (int32)PALs){
			len = parts[chan[c].steps[seqcount].part].len;
			sps = parts[chan[c].steps[seqcount].part].sps;
			sum += sps * len;

			if (seqcount > chan[c].rloop) seqcount=chan[c].lloop;
			seqcount++;
		}

		if (seqcount > 0)
			seqcount--;
		else
			seqcount = chan[c].rloop;

		len = parts[chan[c].steps[seqcount].part].len;
		sps = parts[chan[c].steps[seqcount].part].sps;

		sum -= sps * len;

			
		stepcount	= (PALs - sum) / sps;
		palcount	= (PALs - sum) % sps;

		p[c]->JumpPos((uint8)seqcount,(uint8)stepcount,(uint8)palcount);
//		printf("PALs[%d] == Seq[%d] Step[%d] Rest[%d]\n",PALs,seqcount,stepcount,palcount);

		if (c == 0){
		// snygga till PAL
			PALs = 0;
			for ( int cnt = 0 ; cnt < seqcount ; cnt++){
				PALs += ( parts[chan[0].steps[cnt].part].sps * parts[chan[0].steps[cnt].part].len );
			}
			PALs += stepcount * parts[chan[0].steps[seqcount].part].sps;
			PALs += palcount;
		}
	}

}


float Song::Version(){ 			return (float)st_version/(float)1000.0; }
float Song::LoaderVersion(){	return (float)ST_CURRENT_FILE_VERSION/(float)1000.0; }

char *Song::Name(){		return name;	}
char *Song::Author(){	return author;	}

void Song::SetName(const char *n)
{
	if (name) free (name);
	name = strdup(n);
}

void Song::SetAuthor(const char *a)
{
	if (author) free (author);
	author = strdup(a);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*_EXPORT*/ uint32 Song::SimpLen()
{
	int max = 0;
	// beräkna sångens längd

	for (int cc = 0 ; cc < channelcount ; cc++){
		int bufc = 0;	// number of buffers
		for (int c = 0 ; c < chan[0].len ; c++){
			Part *tpart = parts + (chan[0].steps[c].part);
			bufc += tpart->sps * tpart->len;			
		}

		if (bufc > max) max = bufc;
	}
	return max;
}

/*_EXPORT*/ uint32 Song::Sps()
{
	return spspal;
}


void _____________________________________();
void _____________________________________(){};

#ifdef TARGET_EDITOR

/*_EXPORT*/ void Song::PartPlay( float *buff, uint32 SampleCount )
{
// ----------------------
// SoundPart
//fprintf(stderr, "\nSong::PartPlay(%p,%ld)\n",buff,SampleCount);

	float *pbd = buff;

	while (SampleCount > 0){
//fprintf(stderr, "\t[B]samplecount:%d, smp_left:%d, bs:%p\n\n", SampleCount, psmp_left, pbs);
		uint32 lim;
		if (SampleCount < psmp_left){
			lim = SampleCount;
			SampleCount = 0;
			psmp_left -= lim;
		}else{
			lim = psmp_left;
			psmp_left = 0;
			SampleCount -= lim;
		}
		
		lim <<=1;
		while ( lim ){
			*pbd = *pbs;
			pbd++;
			pbs++;
			lim--;
		}
//fprintf(stderr, "\t[A]samplecount:%d, smp_left:%d, bs:%p\n\n", SampleCount, psmp_left, pbs);
		
		if (psmp_left < 1){
			if (pp->NextBuffer() )
				MemMulMove(stereobuffer,pp->Buffer(),spspal, cmul[channelcount] , cmul[channelcount] );
			else
				memset( stereobuffer, 0, spspal * sizeof(float) * 2);			
			psmp_left = spspal;
			pbs = stereobuffer;
		}
		
	}
};

/*_EXPORT*/ void Song::SetPartToPlay( uint8 part, uint8 step)
{
//	fprintf(stderr,"Song::SetPartToPlay( %d, %d)\n",part,step);
	pchanstep.part = part;
	psmp_left = 0;
	
	pp->JumpPos(0,step,0);
};

/*_EXPORT*/ void Song::SetPartPos(uint8 step)
{
//		printf("PALs[%d] == Seq[%d] Step[%d] Rest[%d]\n",PALs,seqcount,stepcount,palcount);
	pp->JumpPos(0,step,0);
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*_EXPORT*/ void Song::Save(char *filename,bool bin, bool no_names)
{
	txt t(filename,true);
	if (JNG_OK!=t.InitCheck()) return;
	
	int c, c1;
	
	t.IsBin(bin);
	
	if (bin)
		t.Val('SWTD',4);
	else
		t.String("SWTT\n");
		
	t.Comment("// SAWTEETH Generated\n");
	t.Comment("// file format version\n");
	t.Val(ST_CURRENT_FILE_VERSION,2);
	t.Comment("// spspal\n");
	t.Val(spspal,2);
	
	// channels
	t.Comment("\n\n\n// CHANNELS -----------\n");
	
	t.Comment("// # of channels\n");
	t.Val(channelcount);
	
	for (c=0; c < channelcount; c++){
		t.Comment("\n// CHANNEL ---------- ");
		t.Comment(c);
		t.Comment("\n// amp left-right\n");
		t.Val(chan[c].left);
		t.Val(chan[c].right);
		t.Comment("\n// len - lloop - rloop\n");
		t.Val(chan[c].len,2);
		t.Val(chan[c].lloop,2);
		t.Val(chan[c].rloop,2);
		
		//	seq
		t.Comment("// part-transp-damp\n");
		for (c1=0; c1 < chan[c].len; c1++){
			t.Val(chan[c].steps[c1].part);
			t.Val((uint8)chan[c].steps[c1].transp);
			t.Val(chan[c].steps[c1].damp);
		}
	}
	
	//	parts
	t.Comment("\n\n\n// PARTS --------------\n");
	t.Comment("// # of parts\n");
	t.Val(partcount);
	
	for (c=0; c < partcount; c++){
		t.Comment("\n// PART ------------- ");
		t.Comment(c);
		t.Comment(" --- ");
		t.Comment(parts[c].name);
		t.Comment("\n// sps & len\n");
		t.Val(parts[c].sps);
		t.Val(parts[c].len);
		t.Comment("// ins - eff - note\n");
		for (c1=0; c1 < parts[c].len; c1++){
			t.Val(parts[c].steps[c1].ins);
			t.Val(parts[c].steps[c1].eff);
			t.Val(parts[c].steps[c1].note);
		}
	}
	
	// instruments
	//fprintf(stderr,"INS\n");
	t.Comment("\n\n\n// INSTRUMENTS\n");
	t.Comment("// # of ins\n");
	t.Val(instrumentcount-1);
	
	for (c = 1; c < instrumentcount; c++) ins[c].Save(t);

	t.Comment("\n\n\n// breakpoints, PAL - command\n");
	t.Val(breakpcount);

	for (c = 0 ; c < breakpcount ; c++){
		t.Val( breakpoints[c].PAL , 4 );
		t.Val( breakpoints[c].command , 4 );
	}

	if (name) t.String(name);
	if (author) t.String(author);

	if (no_names) return;
	for (c=0; c < partcount; c++)
		if (parts[c].name) t.String(parts[c].name);

	for (c = 1; c < instrumentcount; c++)
		if (ins[c].name) t.String(ins[c].name);

}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*_EXPORT*/	uint8 Song::ChannelCount()		{ return channelcount;		}
/*_EXPORT*/	uint8 Song::PartCount()			{ return partcount;			}
/*_EXPORT*/	uint8 Song::InsCount()			{ return instrumentcount;	}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*_EXPORT*/ Channel *Song::GetChannel( uint8 c)
{
	if (c < channelcount)
		return chan + c;
	else
		return NULL;
}


/*_EXPORT*/ uint8 Song::NewChannel()
{
	if (channelcount >= CHN) return 0;

	chan[channelcount].steps = (ChStep*) CALLOC( CHNSTEPS , sizeof(ChStep));
	chan[channelcount].left = 200;
	chan[channelcount].right = 200;
	chan[channelcount].len = 1;
	chan[channelcount].lloop = 0;
	chan[channelcount].rloop = 0;

	p[channelcount] = new Player(this,chan + channelcount);
	channelcount++;
	return channelcount-1;		
}

/*_EXPORT*/ status_t Song::RemoveChannel(uint8 c)
{
	// om det är den sista
	if (channelcount <= 1) return JNG_ERROR;

	channelcount--;

	free( chan[c].steps );
	delete p[ c ];
	
	// om den ligger sist finns inget hål att fylla
	if (c == channelcount) return JNG_OK;

	chan[c] = chan[ channelcount ];
	p[c] = p[ channelcount ];

	return JNG_OK;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*_EXPORT*/ Part *Song::GetPart( uint8 part )
{
	if (part < partcount)
		return parts + part;
	else
		return parts;
}


/*_EXPORT*/ uint8 Song::NewPart( Part* copy_me )
{
	if (partcount>= PARTS ) return 0;

	parts[partcount].steps = (Step*) CALLOC( PARTSTEPS, sizeof(Step));
	
	if ( copy_me != NULL ){
		parts[partcount].sps = copy_me->sps;
		parts[partcount].len = copy_me->len;
		memcpy(parts[partcount].steps,copy_me->steps,parts[partcount].len *sizeof(Step)); 
		parts[partcount].name = new char[strlen(copy_me->name)+6];
		sprintf(parts[partcount].name,"%s copy",copy_me->name);
	} else {
		parts[partcount].sps = 6;
		parts[partcount].len = 8;
		parts[partcount].name = strdup("Unnamed Part");
	}
	
	partcount++;
	return partcount-1;
}


/*_EXPORT*/ status_t Song::RemovePart(uint8 part)
{
	// om det är den sista
	if (partcount == 1) return JNG_ERROR;

	int c;
	// check if part is used
	for (c = 0; c < channelcount; c++){
		for (int c2 = 0; c2 < chan[c].len; c2++){
			if (part == chan[c].steps[c2].part) return JNG_ERROR;
		}
	}
	partcount--;

	// om den ligger i mitten
	free( parts[part].name );
	free( parts[part].steps );

	// om den ligger sist
	if (part == partcount) return JNG_OK;

	parts[part] = parts[ partcount ];

	// replace
	for (c = 0; c < channelcount; c++)
		for (int c2 = 0; c2 < chan[c].len; c2++)
			if (partcount == chan[c].steps[c2].part)
				chan[c].steps[c2].part = part;

	return JNG_OK;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*_EXPORT*/ Ins *Song::GetIns( uint8 i ){
	if (i < instrumentcount)
		return ins + i;
	else
		return ins;
}


/*_EXPORT*/ uint8 Song::NewIns( uint8 copy_me){
	if ( instrumentcount >= INS ) return 0;
	ins[instrumentcount].Free();
	ins[instrumentcount] = *(GetIns(copy_me));
	instrumentcount++;
	return instrumentcount-1;
}

/*_EXPORT*/ uint8 Song::NewIns(txt &t){
	if ( instrumentcount >= INS ) return 0;
	ins[instrumentcount].Load(t, st_version);
	instrumentcount++;
	return instrumentcount-1;
}

/*_EXPORT*/ status_t Song::RemoveIns(uint8 l_ins)
{
	// om det är den sista
	if (instrumentcount == 2) return JNG_ERROR;

	int c;
	// check if ins is used
	for (c = 0; c < partcount; c++)
		for (int c2 = 0; c2 < parts[c].len; c2++)
			if (l_ins == parts[c].steps[c2].ins) return JNG_ERROR;

	instrumentcount--;

	// om den ligger i mitten
	ins[l_ins].Free();

	// om den ligger sist
	if (l_ins == instrumentcount) return JNG_OK;

	ins[l_ins] = ins[ instrumentcount ];

	// replace
	for (c = 0; c < partcount; c++)
		for (int c2 = 0; c2 < parts[c].len; c2++)
			if (instrumentcount == parts[c].steps[c2].ins)
				parts[c].steps[c2].ins = l_ins;

	return JNG_OK;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*_EXPORT*/ status_t Song::Optimize(){
	// ta bort parts som ej används
	int c;
	for (c = partcount-1 ; c >=0 ; c--){
		if (JNG_OK==RemovePart(c)) fprintf(stderr,"p%2d\n",c);
	}

	// ta bort instruments
	for (c = instrumentcount-1 ; c > 0 ; c--){
		if (JNG_OK==RemoveIns(c)) fprintf(stderr,"i%2d\n",c);
	}
	return JNG_OK;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ______________________________________();
void ______________________________________(){};

#define FIND int c = 0; while ( ( c < breakpcount ) && ( PAL > breakpoints[c].PAL ) ) c++;

/*_EXPORT*/ uint8 Song::GetBreakPointIndex( uint32 PAL ){
	FIND
	// check if exists
	if ( c >= breakpcount ) return 0;
	if ( PAL != breakpoints[c].PAL ) return 0;

	return c;
}

/*_EXPORT*/ void Song::SetBreakPointByIndex( uint8 index, uint32 command ){
//	if ( index < breakpcount ) return;
	breakpoints[index].command = command;
}

/*_EXPORT*/ uint32 Song::GetBreakPoint( uint32 PAL ){
	FIND

	// check if exists
	if ( c >= breakpcount ) return 0;
	if ( PAL != breakpoints[c].PAL ) return 0;

	return breakpoints[c].command;
}

/*_EXPORT*/ void Song::NewBreakPoint( uint32 PAL, uint32 command ){
	puts("NewBP");
	FIND

	// check if exists
	if (PAL == breakpoints[c].PAL){ 
		puts("case 0");
		breakpoints[c].command = command;
		return;
	}

	// flytta efter
	for (int d = breakpcount; d > c ; d--){
		breakpoints[d].PAL = breakpoints[d-1].PAL;
		breakpoints[d].command = breakpoints[d-1].command;
	}

	breakpcount++;

	breakpoints[c].PAL = PAL;
	breakpoints[c].command = command;
}

/*_EXPORT*/ void Song::RemoveBreakPoint( uint32 PAL ){
	FIND

	// check if exists
	if (c >= breakpcount) return;
	if (PAL != breakpoints[c].PAL) return;

	// flytta efter
	breakpcount--;
	for (;c < breakpcount; c++){
		breakpoints[c].command = breakpoints[c+1].command;
		breakpoints[c].PAL = breakpoints[c+1].PAL;
	}
}


#endif

