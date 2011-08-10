/* Copyright 2001, Arvid and Jonas Norberg
Distributed under the terms of the MIT Licence. */

#ifndef _ST_TRACKER_VIEW_H_
#define _ST_TRACKER_VIEW_H_

#include "stTrackerEntry.h"

class stTrackerView:public BView
{
public:
	stTrackerView(BRect bounds,uint8 num_lines,field_type *type_list,uint8 num_fields,BMessage *message);
	virtual ~stTrackerView();

	void SetLength(uint8 length);
	void UpdateScrollbars();
	virtual void AttachedToWindow();

	virtual void FrameResized(float w,float h);

	uint8 FieldContent(uint8 line,uint8 field);
	void SetFieldContent(uint8 line,uint8 field,uint8 data);

	void MoveCursorUp(uint8 field);
	void MoveCursorDown(uint8 field);
	
	uint8 CurrentRow();
	
	uint8 Octave();
	void SetOctave(uint8 oct);
	
	int OptimalSize() { return entry_height * entry_count; }
private:

	uint16 entry_height;
	uint16 entry_width;

	uint8 entry_count;
	stTrackerEntry **entry_list;
	BMessage *return_message;
	uint8 current_octave;

	uint8 octave;

	field_type *field_list;
	uint8 field_count;
};

// inline definitions

inline uint8 stTrackerView::Octave()
{
	return octave;
}

inline void stTrackerView::SetOctave(uint8 oct)
{
	octave = oct;
}

inline uint8 stTrackerView::FieldContent(uint8 line,uint8 field)
{
	if (line<entry_count) return entry_list[line]->FieldContent(field);
	return 0;
}

inline void stTrackerView::SetFieldContent(uint8 line,uint8 field,uint8 data)
{
	if (line<entry_count) entry_list[line]->SetFieldContent(field,data);
}

inline uint8 stTrackerView::CurrentRow()
{
	uint8 i;
	for (i=entry_count-1;i > 0;i--)
		if (entry_list[i]->IsFocus()) break;
	return i;
}

#endif // _ST_TRACKER_VIEW_H_
