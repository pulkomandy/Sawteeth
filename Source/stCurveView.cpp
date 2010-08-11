#include "stCurveView.h"
#include "stCurveKnob.h"
#include <Point.h>
#include <Font.h>

#include "stlimits.h"

stCurveView::stCurveView(BRect bounds,char *name,BMessage *message):BView(bounds,"CurveView",B_FOLLOW_NONE,B_NAVIGABLE/*_JUMP*/+B_WILL_DRAW)
{
	m_name = name;
	BFont name_font(be_bold_font);
	font_height height;
	name_font.GetHeight(&height);
	m_name_pos = BPoint(bounds.Width() - name_font.StringWidth(name) - 3.0, height.ascent + 1.0);
	SetFont(&name_font);

	m_message = message;
	knob_count = 1;
	knob=new stCurveKnob*[knob_count];

	m_height=bounds.Height();
	width=bounds.Width();

	uint8 i;

	for (i=0;i<knob_count;i++)
		knob[i]=new stCurveKnob(BPoint(i,m_height),BRect(0,0,100,100),m_message);

	for (i=0;i<knob_count;i++)
		AddChild(knob[i]);

	CalcLimits();

	SetViewColor(B_TRANSPARENT_COLOR);
	SetLowColor(200,200,200);
	SetHighColor(136,136,234);
	UpdateIndices();
}

stCurveView::~stCurveView()
{
	uint8 i;
	for (i=0;i<knob_count;i++)
		delete knob[i];
}

void stCurveView::DeleteKnob()
{
	if (knob_count==1) return;
	stCurveKnob **tmp_knob;
	tmp_knob=new stCurveKnob*[knob_count-1];
	uint8 i,j=0;
	bool removed=false;
	for (i=0;i<knob_count;i++)
	{
		if ((knob[i]->IsFocus() || i==knob_count-1) && !removed) // we don't want to remove more than one knob
		{ // if no knob has the focus we delete the last one
			if (i>0) knob[i-1]->MakeFocus();
			else knob[i+1]->MakeFocus();

			RemoveChild(knob[i]);
			removed=true;
			i++;
			if (i==knob_count) continue;
		}
		tmp_knob[j++]=knob[i];
	}
	knob_count--;

	delete []knob;
	knob=tmp_knob;
	CalcLimits();
	Invalidate();
	UpdateIndices();
}

void stCurveView::AddKnob()
{
	stCurveKnob **tmp_knob;
	tmp_knob=new stCurveKnob*[knob_count+1];
	uint8 i;
	for (i=0;i<knob_count;i++)
		tmp_knob[i]=knob[i];

	tmp_knob[knob_count]=new stCurveKnob(knob[knob_count-1]->Position()+BPoint(10,0),BRect(knob[knob_count-1]->XPosition(),0,width,m_height),m_message);
	AddChild(tmp_knob[knob_count]);
	knob_count++;

	delete []knob;
	knob = tmp_knob;

	BPoint memory1;
	BPoint memory2;
	bool copy=false;
	for (i=0;i<knob_count;i++)
	{
		if (copy)
		{
			memory2=knob[i]->Position();
			knob[i]->MoveTo(memory1);
			memory1=memory2;
		}
		else if (knob[i]->IsFocus() && i!=knob_count-1)
		{
			copy=true;
			memory1=BPoint((knob[i+1]->Position().x+knob[i]->Position().x)/2,(knob[i+1]->Position().y+knob[i]->Position().y)/2);
		}
	}
	CalcLimits();
	UpdateIndices();
}

void stCurveView::CalcLimits()
{
	if (knob_count>2)
	{
		BRect rect(0,0,knob[1]->XPosition()-1,m_height);
		knob[0]->SetMoveArea(rect);
		uint8 i;
		for (i=0;i<knob_count-2;i++)
		{
			rect=BRect(knob[i]->XPosition()+1,0,knob[i+2]->XPosition()-1,m_height);
			knob[i+1]->SetMoveArea(rect);
		}
		rect=BRect(knob[knob_count-2]->XPosition()+1,0,knob[knob_count-2]->XPosition()+255,m_height);
		knob[knob_count-1]->SetMoveArea(rect,true);
	}
	else if (knob_count==2)
	{
		BRect rect(0,0,knob[1]->XPosition()-1,m_height);
		knob[0]->SetMoveArea(rect);
	
		rect=BRect(knob[0]->XPosition()+1,0,knob[0]->XPosition()+255,m_height);
		knob[1]->SetMoveArea(rect,true);
	}
	else
	{
		BRect rect(0,0,255,m_height);
		knob[0]->SetMoveArea(rect,true);
	}
}

void stCurveView::Draw(BRect )
{
	uint32 i;
	for (i=0;i<knob_count;i++)
		points[i] = knob[i]->Position();

	points[i++]=BPoint(width,knob[knob_count-1]->Position().y);
	points[i++]=BPoint(width,0);
	points[i]=BPoint(0,0);
	
	FillPolygon(points,knob_count+3,B_SOLID_LOW);

	i=knob_count;
	points[i++]=BPoint(width,knob[knob_count-1]->Position().y);
	points[i++]=BPoint(width,m_height);
	points[i]=BPoint(0,m_height);
	
	FillPolygon(points,knob_count+3);

	points[0] = BPoint(0,0);
	points[1] = BPoint(0,m_height);
	points[2] = knob[0]->Position();
	
	SetHighColor(175,175,217);
	
	FillPolygon(points,3);

	SetHighColor(0,0,0,60);
	SetDrawingMode(B_OP_ALPHA);
	DrawString(m_name, m_name_pos);
	SetDrawingMode(B_OP_COPY);
	SetHighColor(136,136,234,255);
}
