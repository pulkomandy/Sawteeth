/* Copyright 2001, Arvid and Jonas Norberg
Distributed under the terms of the MIT Licence. */

// Currently not connected with parts

#ifndef ST_CHRULER_VIEW_H
#define ST_CHRULER_VIEW_H

#include <View.h>

class stChRulerView : public BView
{
public:
	stChRulerView(BRect frame);
	
void	Draw(BRect frame);
void	SetZoom(float PixelsPerPAL);
void	SetPos(uint32 p);

private:
	uint32 pos;
	float z;
};

#endif
