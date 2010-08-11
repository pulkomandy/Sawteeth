#ifndef STSP
#define STSP
//////////////////////////////
#include <SoundPlayer.h>
#include "song.h"

class stsp : public Song {
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