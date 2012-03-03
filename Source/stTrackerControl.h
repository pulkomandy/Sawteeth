/* Copyright 2001, Arvid and Jonas Norberg
Distributed under the terms of the MIT Licence. */

#ifndef _ST_TRACKER_CONTROL_H_
#define _ST_TRACKER_CONTROL_H_

#include <View.h>
#include <TextControl.h>
#include <GroupView.h>

#include "song.h"
#include "stTrackerView.h"

enum
{
	ST_SET_LENGTH = 2,
	ST_SET_SPEED
};

class stTrackerControl:public BGroupView
{
public:
	stTrackerControl(uint8 num_lines, uint8 speed, field_type *type_list,uint8 num_fields,BMessage *message);
	~stTrackerControl();

	void SetLength(uint8);
	uint8 FieldContent(uint8 line,uint8 field);
	void SetFieldContent(uint8 line,uint8 field,uint8 data);

	stTrackerView *TrackerView();

private:
	stTrackerView *tracker_view;
	BMessage *return_message;
	BTextControl *length_string;
	BTextControl *speed_string;
};

// inline definitions

inline void stTrackerControl::SetLength(uint8 length)
{
	if (length < 1) length = 1;
	tracker_view->SetLength(length);
	char tmp[4];
	sprintf(tmp,"%d",length);
	length_string->SetText(tmp);
}

inline uint8 stTrackerControl::FieldContent(uint8 line,uint8 field)
{
	return tracker_view->FieldContent(line,field);
}

inline void stTrackerControl::SetFieldContent(uint8 line,uint8 field,uint8 data)
{
	tracker_view->SetFieldContent(line,field,data);
}

#endif // _ST_TRACKER_CONTROL_H_
