/* Copyright 2001, Arvid and Jonas Norberg
Distributed under the terms of the MIT Licence. */

// Currently not connected with parts

#ifndef ST_BP_VIEW_H
#define ST_BP_VIEW_H

#include <View.h>
#include "stSong.h"

class stBPView : public BView
{
public:
	stBPView(BRect frame, stSong *s);
	
void	Draw(BRect frame);
void	SetZoom(float PixelsPerPAL);

private:
	stSong * _s;
	float _z;
};

#endif
