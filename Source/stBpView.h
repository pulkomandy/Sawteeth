// SawTeeth RulerView
// Jonas Norberg
//
// Currently not connected width parts

#ifndef ST_BP_VIEW_H
#define ST_BP_VIEW_H

#include <View.h>
#include "song.h"

class stBPView : public BView
{
public:
	stBPView(BRect frame, Song *s);
	
void	Draw(BRect frame);
void	SetZoom(float PixelsPerPAL);

private:
	Song * _s;
	float _z;
};

#endif