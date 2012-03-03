/* Copyright 2001, Arvid and Jonas Norberg
Distributed under the terms of the MIT Licence. */

#include "stBpView.h"
#include "song.h"
#include <stdio.h> // remove this line

stBPView::stBPView(BRect frame, Song *s) :
	BView(frame, "View",  B_FOLLOW_TOP, B_WILL_DRAW ),
	_s(s),
	_z(1)

{
	SetViewColor(0,0,0);
	SetHighColor(140,255,255);
	ResizeTo(100000,11);
}

void stBPView::SetZoom(float zoom)
{
	_z = zoom;
	Invalidate();
}

rgb_color table[] = {
	{255,55,55},
	{55,255,55},
	{55,55,255},

	{255,255,55},
	{55,255,255},
	{255,55,255},

	{255,155,155},
	{155,255,155},
	{155,155,255},

	{255,255,155},
	{155,255,255},
	{255,155,255},

};

void stBPView::Draw(BRect)
{
//puts("stBPView::Draw()");	
	int count = _s->BreakPointCount();
	BreakPoint *bpp = _s->GetBreakPointByIndex(0);
	
	float height = Bounds().Height();

	for (int c = 0 ; c < count ; c++){
		float start = (float)bpp->PAL * _z;
		uint8 col = bpp->command - '0';
		if (col > 9) col = 0;
		SetHighColor(table[col]);
		StrokeLine( BPoint(start,0),BPoint(start,height));
		StrokeLine( BPoint(start+1,0),BPoint(start+1,height));
		bpp++;
	}
}
