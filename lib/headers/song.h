/* Copyright 2001, Jonas Norberg
Distributed under the terms of the MIT Licence. */

#ifndef Song___H
#define Song___H

#include "stdlib.h"
#include "string.h"
#include "types.h"
#include "ins_part_channel.h"
#include "txt.h"

#include "st_version.h"

#include "stlimits.h"

// välj vad som skall kompileras


class Song
{
friend class Player;
friend class InsPly;
public:

	Song( const Song &s );
	Song( txt &flat );
	virtual ~Song();

	status_t	Load( txt &t);
	status_t	InitCheck();
	
	// returnerar huruvida låten loopade senaste cykeln
	bool		Play( float *Buffer, uint32 FrameCount );

	float	Version();
	static float	LoaderVersion();
	
	uint32		GetPos();
	void		SetPos( uint32 PALs );

	uint32		SimpLen(); // len i pals
	uint32		Sps();

#ifdef TARGET_EDITOR
	void		PartPlay( float *Buffer, uint32 SampleCount );
	void		SetPartToPlay( uint8 part = 0 , uint8 step = 0 );
	void		SetPartPos( uint8 step = 0 );

	void		Save( char *filename, bool bin=false , bool no_names = false);

	// // // // // // // // // // CHANNEL stuff
	uint8		ChannelCount();
	Channel		*GetChannel( uint8 c );
	uint8		NewChannel(); // returnerar den som skapats
	status_t	RemoveChannel( uint8 channel );

	// // // // // // // // // // PART stuff
	uint8		PartCount();
	Part		*GetPart( uint8 part );
	uint8		NewPart( Part *copy_me = 0 );
	status_t	RemovePart( uint8 part );	// B_ERROR if part is used

	// // // // // // // // // // INS stuff
	uint8		InsCount();
	Ins		*GetIns( uint8 i );
	uint8		NewIns( uint8 copy_me );
	uint8		NewIns( txt &t );
	status_t	RemoveIns(uint8 l_ins);


	// // // // // // // // // // BREAKPOINT stuff
	uint8		GetBreakPointIndex( uint32 PAL );
	void		SetBreakPointByIndex( uint8 index, uint32 command );
	uint32		GetBreakPoint( uint32 PAL );
	void		NewBreakPoint( uint32 PAL, uint32 command );
	void		RemoveBreakPoint( uint32 PAL );

	// // // // // // // // // // OPTIONAL stuff
	status_t	Optimize();		//B_OK if something was removed.
#endif

	
	inline uint8 BreakPointCount()	{ return breakpcount; }

	inline BreakPoint *GetBreakPointByIndex( uint8 index ){
	//	if ( index < breakpcount ) return 0;
		return breakpoints + index;
	}
	
	float	*N2F;
	float	*R2F;
	float cmul[CHN];

	char	*Name();
	char	*Author();

	void SetName(const char *);
	void SetAuthor(const char *);
private:
	char	*name;
	char	*author;

	void	Init();
	void	FreeSong();
	
	uint16 st_version;
	uint16 spspal;

	uint8 channelcount;
	uint8 partcount;
	uint8 instrumentcount;	// anger inte antal instrument, ett för mycket
	
	Channel	*chan;
	Part	*parts;
	Ins		*ins;

	uint8 breakpcount;			// antal breakpoints
	BreakPoint *breakpoints;	// breakpoints
	status_t init;	

	bool looped;
	
	// player
	float *stereobuffer;
	void MemMulAdd( float *d, float *s, uint32 count, float l, float r);
	void MemMulMove( float *d, float *s, uint32 count, float l, float r);
	uint32 PALs;
	uint32 smp_left;
	float *bs;
	Player **p;
	
#ifdef TARGET_EDITOR
	// partplay
	Channel pchan;
	ChStep pchanstep;
	uint32 psmp_left;
	float *pbs;
	Player *pp;
#endif

};

#endif
