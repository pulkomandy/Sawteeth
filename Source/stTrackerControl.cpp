/* Copyright 2001, Arvid and Jonas Norberg
Distributed under the terms of the MIT Licence. */

#include <ScrollView.h>
#include <TextControl.h>

#include "stTrackerControl.h"

class stScrollView: public BScrollView
{
	private:
		stTrackerView& target;
	public:
	stScrollView(stTrackerView* view)
		: BScrollView("tracker_scroll", view, B_FRAME_EVENTS, false, true)
		, target(*view)
	{
	}

	void FrameResized(float width, float height)
	{
		BScrollView::FrameResized(width, height);
		target.UpdateScrollbars(height);
	}
};

stTrackerControl::stTrackerControl(uint8 num_lines, uint8 speed,
	field_type *type_list, uint8 num_fields, BMessage *message)
	: BGroupView(B_VERTICAL, 0)
{
	return_message = message;

// ************ Set up the scroll view ***************'

	tracker_view = new stTrackerView(num_lines, type_list, num_fields,
		message);

	AddChild(new stScrollView(tracker_view));

// ************ Set up length gadget ***************'

	BMessage *control_message;
	control_message = new BMessage(*return_message);
	control_message->AddInt8("action", ST_SET_LENGTH);
	control_message->AddPointer("control", this);

	length_string = new BTextControl(
		"set_length", "length", "", control_message);
	AddChild(length_string);


	char tmp[10];
	sprintf(tmp, "%d", num_lines);
	length_string->SetText(tmp);

// ************ Set up speed gadget ***************'
	control_message = new BMessage(*return_message);
	control_message->AddInt8("action", ST_SET_SPEED);
	control_message->AddPointer("control", this);

	speed_string = new BTextControl(
		"set_speed", "speed", "", control_message);
	AddChild(speed_string);

	sprintf(tmp, "%d", speed);
	speed_string->SetText(tmp);
}


stTrackerControl::~stTrackerControl()
{
}


stTrackerView *stTrackerControl::TrackerView()
{
	return tracker_view;
}

