/* Copyright 2001, Arvid and Jonas Norberg
Distributed under the terms of the MIT Licence. */

#include "stTrackerView.h"

#include <Window.h>

stTrackerView::stTrackerView(uint8 num_lines,field_type *type_list,uint8 num_fields,BMessage *message)
	: BGroupView(B_VERTICAL, 0.0f)
{
	octave = 4;
	entry_count=0;
	entry_list=NULL;
	return_message=message;
	field_count=num_fields;

	SetViewColor(ST_LOW_COLOR);

	field_list=new field_type[field_count];
	uint8 i;
	for (i=0;i<field_count;i++)
		field_list[i]=type_list[i];

	BRect rect;
	stTrackerEntry::PreferredSize(&rect,type_list,num_fields);
	entry_height=(uint16)rect.Height();
	entry_width=(uint16)rect.Width();

	SetLength(num_lines);

	SetExplicitMinSize(BSize(entry_width, 9 * entry_height));
}

stTrackerView::~stTrackerView()
{
	delete []entry_list;
	delete []field_list;
}

void stTrackerView::FrameResized(float ,float )
{
	UpdateScrollbars();
}

void stTrackerView::AttachedToWindow()
{
	if (field_count > 0) entry_list[0]->MakeFocus();
	UpdateScrollbars();
}

void stTrackerView::SetLength(uint8 length)
{
	stTrackerEntry **tmp_list;
	if (length<entry_count)
	{
		uint8 i;
		for (i=length;i<entry_count;i++) RemoveChild(entry_list[i]);
		entry_count=length;
	}
	else
	{
		tmp_list=new stTrackerEntry*[length];
		uint i;
		for (i=0;i<entry_count;i++) tmp_list[i]=entry_list[i];
		for (i=entry_count;i<length;i++)
		{
			tmp_list[i]=new stTrackerEntry(field_count,field_list);
			uint8 j;
			for(j=0;j<field_count;j++) 
				if (field_list[j]==ST_TYPE_INDEX) tmp_list[i]->SetFieldContent(j,i);
			AddChild(tmp_list[i]);
		}
		if (entry_list) delete []entry_list;
		entry_list=tmp_list;
		entry_count=length;
	}

	UpdateScrollbars();
}

void stTrackerView::UpdateScrollbars()
{
	if (Parent() == NULL) return;
	float h = Parent()->Bounds().Height();
	UpdateScrollbars(h);
}

void stTrackerView::UpdateScrollbars(float height)
{
	BScrollBar *sb;
	if ((sb=ScrollBar(B_VERTICAL))!=NULL)
	{
		float range=entry_count*entry_height - height;

		sb->SetProportion(height/(entry_count*entry_height));
		sb->SetRange(0,range);
		// Steps are 1 line for small steps
		//   and 1/2 visible window for large steps
		sb->SetSteps(entry_height, height / 2.0);
	}
}

void stTrackerView::MoveCursorUp(uint8 field)
{
	uint8 i;
	for (i=0;i<entry_count;i++)
		if (entry_list[i]->IsFocus()) break;
	if (i>0) 
	{
		entry_list[i-1]->current_field=field;
		entry_list[i-1]->MakeFocus();
		if (ConvertToParent(entry_list[i-1]->ConvertToParent(BPoint(0,0))).y<0)
			ScrollTo(entry_list[i-1]->ConvertToParent(BPoint(0,0)));
	}
}

void stTrackerView::MoveCursorDown(uint8 field)
{
	uint8 i;
	for (i=0;i<entry_count;i++)
		if (entry_list[i]->IsFocus()) break;
	if (i<entry_count-1)
	{
		entry_list[i+1]->current_field=field;
		entry_list[i+1]->MakeFocus();
		if (ConvertToParent(entry_list[i+1]->ConvertToParent(BPoint(0,entry_height))).y>Parent()->Frame().Height())
			ScrollTo(entry_list[i+1]->ConvertToParent(BPoint(0,entry_height)-BPoint(0,Parent()->Frame().Height()-4)));
	}
}
