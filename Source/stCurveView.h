/* Copyright 2001, Arvid and Jonas Norberg
Distributed under the terms of the MIT Licence. */

#ifndef _ST_CURVE_VIEW_H_
#define _ST_CURVE_VIEW_H_

#include "stCurveKnob.h"
#include <stdio.h>
#include <Point.h>

#include "stlimits.h"

class stCurveView:public BView
{
public:
	stCurveView(BRect rect,char *name,BMessage *message);
	~stCurveView();
	
	virtual void Draw(BRect rect);
	void CalcLimits();
	void AddKnob();
	void DeleteKnob();
	void SetKnobPosition(BPoint point,uint8 id);
	BPoint KnobPosition(uint8 id);
	
	uint8 KnobCount();

	void MouseUp(BPoint where);
	
private:
	void UpdateIndices();

	BPoint points[INSPOINTS + 4]; // for draw function

	stCurveKnob **knob;
	BMessage *m_message;
	uint8 knob_count;
	float m_height;
	float width;
	char *m_name;
	BPoint m_name_pos;
};

// inline definitions

inline uint8 stCurveView::KnobCount()
{
	return knob_count;
}

inline void stCurveView::UpdateIndices()
{
	for (int32 i=0;i<knob_count;i++)
		knob[i]->SetIndex(i);
}

inline void stCurveView::SetKnobPosition(BPoint point,uint8 id)
{
	float m_height=Frame().Height();
	if (id < knob_count)
	{
		knob[id]->MoveTo(BPoint(point.x,m_height-point.y));
		CalcLimits();
	}
}

inline BPoint stCurveView::KnobPosition(uint8 id)
{
	if (id < knob_count)
	{
		float m_height = Frame().Height();
		BPoint point = knob[id]->Position();
		return BPoint(point.x,m_height-point.y);
	}
	return BPoint(0,0);
}

#endif // _ST_CURVE_VIEW_H_
