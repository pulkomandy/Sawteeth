/* Copyright 2001, Arvid and Jonas Norberg
Distributed under the terms of the MIT Licence. */

#include <stdio.h>
#include <SoundPlayer.h>
#include "stsp.h"
#include "song.h"

void stsp::BufferProc(void *player_, void *buffer, size_t size, const media_raw_audio_format &format)
{
	stsp *player = (reinterpret_cast<stsp*>(player_));

// only work for mono floating-point audio 
	if (format.channel_count != 2 || 
		format.format != media_raw_audio_format::B_AUDIO_FLOAT) { 
		puts("sorry, I only do stereo-float");
		return; 
	} 

	if (player->usepartplay ){
//		fputs("BufferProc PartPlay\n",stderr);
		player->PartPlay((float*)buffer,size >> 3);	
	}else{
//		fputs("BufferProc Play\n",stderr);
		player->Play((float*)buffer,size >> 3);	
	}
}

void stsp::UsePartPlay(bool p)
{
	usepartplay = p;
}

stsp::stsp(txt &flat):Song(flat)
{
	UsePartPlay(false);
	
	// mono-float
	media_raw_audio_format af = {	44100,
									2,
									media_raw_audio_format::B_AUDIO_FLOAT,
#ifdef __INTEL__
									B_MEDIA_LITTLE_ENDIAN,
#else
									B_MEDIA_BIG_ENDIAN,
#endif
									4096 };
	player = new BSoundPlayer(&af, "SawTeeth", BufferProc, NULL, this); 
}

stsp::~stsp()
{
	delete player;
}

void stsp::Start()
{
	player->SetHasData(true); 
	player->Start(); 
}

void stsp::Stop()
{
	player->Stop();
}
