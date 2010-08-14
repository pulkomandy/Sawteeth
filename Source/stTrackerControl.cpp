/* Copyright 2001, Arvid and Jonas Norberg
Distributed under the terms of the MIT Licence. */

#include <ScrollView.h>
#include <TextControl.h>

#include "stTrackerControl.h"

stTrackerControl::stTrackerControl(BRect bounds,uint8 num_lines,uint8 speed, field_type *type_list,uint8 num_fields,BMessage *message,int32 resize):BBox(bounds,"TrackerControl",resize,B_FRAME_EVENTS,B_NO_BORDER)
{
	return_message=message;

// ************ Set up the scroll view ***************'

	bounds.OffsetTo(0,0);
	bounds.bottom-=25;
	tracker_view=new stTrackerView(bounds,num_lines,type_list,num_fields,message);
	tracker_frame=tracker_view->Frame();

	AddChild((BView*)new BScrollView("tracker_scroll",tracker_view,B_FOLLOW_TOP_BOTTOM | B_FOLLOW_LEFT,0,false,true));

	ResizeTo(tracker_frame.Width() + B_V_SCROLL_BAR_WIDTH,tracker_frame.Height() + 43.0);

// ************ Set up length gadget ***************'

	BMessage *control_message;
	control_message=new BMessage(*return_message);
	control_message->AddInt8("action",ST_SET_LENGTH);
	control_message->AddPointer("control",this);
	
	length_string = new BTextControl(BRect(Bounds().left,tracker_frame.Height()+4,Bounds().right,tracker_frame.Height()+24),"set_length","length:","",control_message,B_FOLLOW_BOTTOM | B_FOLLOW_LEFT);
	length_string->SetDivider(be_plain_font->StringWidth("length:"));
	AddChild((BView*)length_string);

	char tmp[10];
	sprintf(tmp,"%d",num_lines);
	length_string->SetText(tmp);

// ************ Set up speed gadget ***************'
	control_message=new BMessage(*return_message);
	control_message->AddInt8("action",ST_SET_SPEED);
	control_message->AddPointer("control",this);

	speed_string = new BTextControl(BRect(Bounds().left,tracker_frame.Height()+24,Bounds().right,tracker_frame.Height()+48),"set_speed","speed:","",control_message,B_FOLLOW_BOTTOM | B_FOLLOW_LEFT);
	speed_string->SetDivider(be_plain_font->StringWidth("speed:"));
	AddChild((BView*)speed_string);

	sprintf(tmp,"%d",speed);
	speed_string->SetText(tmp);
}
stTrackerControl::~stTrackerControl()
{
}

stTrackerView *stTrackerControl::TrackerView()
{
	return tracker_view;
}

void stTrackerControl::FrameResized(float ,float height)
{
	tracker_view->ResizeTo(tracker_view->Frame().Width(),height - 25.0);
}
