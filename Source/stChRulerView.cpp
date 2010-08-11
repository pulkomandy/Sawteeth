#include "stChRulerView.h"
#include <stdio.h> // remove this line

stChRulerView::stChRulerView(BRect frame) : BView(frame, "View",  B_FOLLOW_NONE, B_WILL_DRAW )
{
	z = 1;
	
	SetViewColor(0,0,0);
	SetHighColor(255,200,40);
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

void stChRulerView::Draw(BRect)
{
//puts("stChRulerView::Draw()");	

		float start = pos*z;
		FillRect( BRect( start, 0, start + 2 , 5 ) );
}
