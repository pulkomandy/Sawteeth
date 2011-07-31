/* Copyright 2001, Arvid and Jonas Norberg
Distributed under the terms of the MIT Licence. */

#include "stChRulerView.h"
#include <stdio.h> // remove this line

stChRulerView::stChRulerView(BRect frame) : BView(frame, "View",  B_FOLLOW_TOP_BOTTOM, B_WILL_DRAW )
{
	z = 1;
	
	SetViewColor(B_TRANSPARENT_COLOR);
	ResizeTo(100000,frame.Height());
}

void stChRulerView::SetZoom(float zoom)
{
	z = zoom;
	Invalidate();
}

void stChRulerView::SetPos(uint32 p)
{
	if (pos == p) return;
	
	pos = p;
	Invalidate();
}

void stChRulerView::Draw(BRect r)
{
	SetHighColor(0,0,0);
	FillRect( BRect( r.left, r.top, r.right, 11) );

	float start = pos*z;
	SetHighColor(255,200,40);
	FillRect( BRect( start, 0, start + 2 , Bounds().Height() ) );
}
