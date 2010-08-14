/* Copyright 2001, Arvid and Jonas Norberg
Distributed under the terms of the MIT Licence. */

#include "stCurveKnob.h"
#include "stCurveView.h"
#include "stCurveControl.h"

stCurveKnob::stCurveKnob(BPoint point,BRect movable_rect,BMessage *message):BView(BRect(point-BPoint(2,2),point+BPoint(2,2)),"CurveKnob",B_FOLLOW_NONE,B_NAVIGABLE+B_WILL_DRAW)
{
	m_message = message;
	movable_rect.OffsetBy(-2,-2);
	moving_rect=movable_rect;
	draw_frame=Frame();
	draw_frame.OffsetTo(0,0);
	SetHighColor(0,0,0);
	SetViewColor(255,255,255);
}

stCurveKnob::~stCurveKnob()
{
}

void stCurveKnob::Draw(BRect )
{
	StrokeRect(draw_frame);
}

void stCurveKnob::MoveTo(BPoint point)
{
	point-=BPoint(2,2);
	point.ConstrainTo(moving_rect);
	BView::MoveTo(point);
	BRect draw_rect=invalidate_rect;
	draw_rect.InsetBy(-2,0);
	draw_rect.OffsetBy(2,2);
	Parent()->Invalidate(draw_rect);
}

void stCurveKnob::SendMoved()
{
	BMessage *msg = new BMessage(*m_message);
	msg->AddInt8("action",ST_MOVE_KNOB);
	msg->AddInt8("knob",Index());
	Window()->PostMessage(msg);
}

void stCurveKnob::MakeFocus(bool focused)
{
	if (!focused) ((stCurveView*)Parent())->CalcLimits();

	BView::MakeFocus(focused);
	if (focused) SetViewColor(keyboard_navigation_color());
	else SetViewColor(255,255,255);
	Invalidate();
}

void stCurveKnob::KeyDown(const char *bytes, int32 numBytes)
{
	switch (*bytes)
	{
	case B_LEFT_ARROW:
		MoveBy(-1,0);
		SendMoved();
		break;
	case B_RIGHT_ARROW:
		MoveBy(1,0);
		SendMoved();
		break;
	case B_UP_ARROW:
		MoveBy(0,-1);
		SendMoved();
		break;
	case B_DOWN_ARROW:
		MoveBy(0,1);
		SendMoved();
		break;
	case B_DELETE:
	case B_BACKSPACE:
		((stCurveView*)Parent())->DeleteKnob();
		break;
	case B_INSERT:
		((stCurveView*)Parent())->AddKnob();
		break;
	default:
		BView::KeyDown(bytes,numBytes);
	}
}

void stCurveKnob::MouseDown(BPoint point)
{
	MakeFocus();
	uint32 buttons;
	uint32 new_buttons;
	GetMouse(&point,&buttons);
	new_buttons=buttons;
	BPoint new_point=point;
	while (new_buttons==buttons)
	{
		snooze(50000);
		point=new_point;
		GetMouse(&new_point,&buttons);
		if (new_point!=point)
			MoveTo(ConvertToParent(new_point));
	}
	((stCurveView*)Parent())->CalcLimits();
	SendMoved();
}
