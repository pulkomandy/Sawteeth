/* Copyright 2001, Arvid and Jonas Norberg
Distributed under the terms of the MIT Licence. */

#include "stCurveControl.h"

stCurveControl::stCurveControl(BRect bounds,char *name, BMessage *message):BView(bounds,"CurveView",B_FOLLOW_NONE,B_WILL_DRAW)
{
	SetViewColor(216,216,216);

	return_message=message;
	BRect frame = bounds;
	frame.InsetBy(5,5);
	frame.bottom -= 18;
	frame.OffsetTo(1,1);
	curve_view=new stCurveView(frame,name,message);
	curve_frame=curve_view->Frame();
	curve_frame.InsetBy(-1,-1);
	AddChild(curve_view);

	BMessage *button_message;
	button_message=new BMessage(*return_message);
	button_message->AddInt8("action",ST_ADD_KNOB);
	button_message->AddPointer("control",this);
	AddChild(new BButton(BRect(0,frame.bottom+3,60,frame.bottom+23),"add_point","Add point",button_message));

	button_message=new BMessage(*return_message);
	button_message->AddInt8("action",ST_DELETE_KNOB);
	button_message->AddPointer("control",this);
	AddChild(new BButton(BRect(60,frame.bottom+3,140,frame.bottom+23),"delete_point","Delete point",button_message));
}

stCurveControl::~stCurveControl()
{
}

void stCurveControl::Draw(BRect )
{
	StrokeRect(curve_frame);
}
