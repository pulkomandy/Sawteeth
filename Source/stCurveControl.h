/* Copyright 2001, Arvid and Jonas Norberg
Distributed under the terms of the MIT Licence. */

#ifndef _ST_CURVE_CONTROL_H_
#define _ST_CURVE_CONTROL_H_

#include "stCurveView.h"

enum action
{
	ST_ADD_KNOB=0,
	ST_DELETE_KNOB,
	ST_MOVE_KNOB
};

class stCurveControl:public BView
{
public:
	stCurveControl(BRect rect,const char *name,BMessage *message);
	~stCurveControl();
	
	virtual void Draw(BRect rect);

	void SetKnobPosition(BPoint point,uint8 id);
	BPoint KnobPosition(uint8 id);
	void AddKnob();
	void DeleteKnob();

	uint8 KnobCount();

	BView *View();

private:
	stCurveView *curve_view;

	BScrollView *m_scroll;

	BRect curve_frame;
	BMessage *return_message;
};

// inline definitions

inline uint8 stCurveControl::KnobCount()
{
	return curve_view->KnobCount();
}

inline BView *stCurveControl::View()
{
	return curve_view;
}

inline void stCurveControl::SetKnobPosition(BPoint point,uint8 id)
{
	curve_view->SetKnobPosition(point,id);
}

inline BPoint stCurveControl::KnobPosition(uint8 id)
{
	return curve_view->KnobPosition(id);
}

inline void stCurveControl::AddKnob()
{
	curve_view->AddKnob();
}

inline void stCurveControl::DeleteKnob()
{
	curve_view->DeleteKnob();
}

#endif // _ST_CURVE_CONTROL_H_
