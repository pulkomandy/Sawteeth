/* Copyright 2001, Arvid and Jonas Norberg
Distributed under the terms of the MIT Licence. */

#ifndef STSP
#define STSP
//////////////////////////////
#include <SoundPlayer.h>
#include "stSong.h"

class stsp : public stSong {
public:
	stsp(txt &flat);	
	~stsp();

	void	UsePartPlay(bool p);
	
	void	Start();
	void	Stop();
	
private:
	bool usepartplay;
	static void BufferProc(void *, void *, size_t , const media_raw_audio_format &);
	BSoundPlayer *player;
};
//////////////////////////////
#endif
