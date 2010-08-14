/* Copyright 2001, Arvid and Jonas Norberg
Distributed under the terms of the MIT Licence. */

#include "stInstrumentWindow.h"
#include "stCurveControl.h"
#include "stTrackerView.h"
#include "stTrackerEntry.h"
#include "stMainWindow.h"
#include "song.h"
#include "stdefs.h"

#include <stdio.h>
#include <Application.h>
#include <Message.h>
#include <Point.h>
#include <CheckBox.h>

stInstrumentWindow::stInstrumentWindow(uint8 index, stMainWindow *main_win,BPoint point,Ins *instrument):
	BWindow(BRect(point,point+BPoint(385,425)),instrument->name, B_FLOATING_WINDOW_LOOK, B_FLOATING_SUBSET_WINDOW_FEEL ,B_NOT_RESIZABLE | B_NOT_ZOOMABLE),
	m_index(index),
	m_instrument(instrument),
	m_main_win(main_win)
{

	UpdateTitle();
	m_box = new BBox(Bounds());
	m_box->ResizeBy(1.0,1.0);
	AddChild(m_box);

// ********* set up the InsName ***************
	BTextControl * tc = new BTextControl(BRect(5,5,375,20),"name","Name:",m_instrument->name,new BMessage(ST_INS_NAME));
	tc->SetDivider(50);
	m_box->AddChild(tc);
	
// ********* set up the Amplitude envelope ***************
	BRect frame(5,30,304,170);
	m_amplitude_envelope = new stCurveControl(frame,"Amplitude",new BMessage(41));
		
	int32 time = m_instrument->amp[0].time;
	m_amplitude_envelope->SetKnobPosition(BPoint((float)time, ((float)m_instrument->amp[0].lev)/2.0),0);
	for (int32 i=1; i < m_instrument->amppoints;i++)
	{
		time += m_instrument->amp[i].time;
		m_amplitude_envelope->AddKnob();
		m_amplitude_envelope->SetKnobPosition(BPoint((float)time, ((float)m_instrument->amp[i].lev)/2.0),i);
	}

// ********* set up the Clipping popup ***************
	BPopUpMenu *cmenu = new BPopUpMenu("clipping_menu");
	
	BMenuItem *citem[3];
	
	cmenu->AddItem(citem[0] = new BMenuItem("no clipping",new BMessage(ST_CLIPPING)));
	cmenu->AddItem(citem[1] = new BMenuItem("hard clipping",new BMessage(ST_CLIPPING)));
	cmenu->AddItem(citem[2] = new BMenuItem("sinus clipping",new BMessage(ST_CLIPPING)));

	citem[m_instrument->clipmode % 3]->SetMarked(true);

	BMenuField *cmenu_field = new BMenuField(BRect(160,frame.Height() - 23,frame.right,frame.Height()), "clipping_select", 0, cmenu);
	m_amplitude_envelope->AddChild(cmenu_field);


// ********* set up the Boost popup ***************
	BPopUpMenu *bmenu = new BPopUpMenu("clipping_menu");
	
	BMenuItem *bitem[16];
	
	bmenu->AddItem(bitem[0] = new BMenuItem("0",new BMessage(ST_BOOST)));
	bmenu->AddItem(bitem[1] = new BMenuItem("1",new BMessage(ST_BOOST)));
	bmenu->AddItem(bitem[2] = new BMenuItem("2",new BMessage(ST_BOOST)));
	bmenu->AddItem(bitem[3] = new BMenuItem("3",new BMessage(ST_BOOST)));
	bmenu->AddItem(bitem[4] = new BMenuItem("4",new BMessage(ST_BOOST)));
	bmenu->AddItem(bitem[5] = new BMenuItem("5",new BMessage(ST_BOOST)));
	bmenu->AddItem(bitem[6] = new BMenuItem("6",new BMessage(ST_BOOST)));
	bmenu->AddItem(bitem[7] = new BMenuItem("7",new BMessage(ST_BOOST)));
	bmenu->AddItem(bitem[8] = new BMenuItem("8",new BMessage(ST_BOOST)));
	bmenu->AddItem(bitem[9] = new BMenuItem("9",new BMessage(ST_BOOST)));
	bmenu->AddItem(bitem[10] = new BMenuItem("10",new BMessage(ST_BOOST)));
	bmenu->AddItem(bitem[11] = new BMenuItem("11",new BMessage(ST_BOOST)));
	bmenu->AddItem(bitem[12] = new BMenuItem("12",new BMessage(ST_BOOST)));
	bmenu->AddItem(bitem[13] = new BMenuItem("13",new BMessage(ST_BOOST)));
	bmenu->AddItem(bitem[14] = new BMenuItem("14",new BMessage(ST_BOOST)));
	bmenu->AddItem(bitem[15] = new BMenuItem("15",new BMessage(ST_BOOST)));

	bitem[m_instrument->boost & 15]->SetMarked(true);

	BMenuField *bmenu_field = new BMenuField(BRect(257,frame.Height() - 23,frame.right,frame.Height()), "boost_select", 0, bmenu);
	m_amplitude_envelope->AddChild(bmenu_field);


// ********* set up the Filter envelope ***************
	frame.OffsetBy(0,frame.Height() + 5.0);
	m_filter_envelope = new stCurveControl(frame,"Filter",new BMessage(42));

	time = m_instrument->filter[0].time;
	m_filter_envelope->SetKnobPosition(BPoint((float)time,(float)m_instrument->filter[0].lev/2.0),0);
	for (int32 i=1; i < m_instrument->filterpoints;i++)
	{
		time += m_instrument->filter[i].time;
		m_filter_envelope->AddKnob();
		m_filter_envelope->SetKnobPosition(BPoint((float)time,(float)m_instrument->filter[i].lev/2.0),i);
	}
	
// ********* set up the Filter popup ***************
	BPopUpMenu *menu = new BPopUpMenu("filter_menu");
	
	BMenuItem *item[7];
	
	menu->AddItem(item[0] = new BMenuItem("no filter",new BMessage(44)));
	menu->AddItem(item[1] = new BMenuItem("low-pass(no res)",new BMessage(44)));
	menu->AddItem(item[2] = new BMenuItem("low-pass",new BMessage(44)));
	menu->AddItem(item[3] = new BMenuItem("low-pass2",new BMessage(44)));
	menu->AddItem(item[4] = new BMenuItem("high-pass",new BMessage(44)));
	menu->AddItem(item[5] = new BMenuItem("band-pass",new BMessage(44)));
	menu->AddItem(item[6] = new BMenuItem("band-stop",new BMessage(44)));

	item[m_instrument->filtermode % 7]->SetMarked(true);

	BMenuField *menu_field = new BMenuField(BRect(160,frame.Height() - 23,frame.right,frame.Height()), "filter_select", 0, menu);
	m_filter_envelope->AddChild(menu_field);

// ********* set up the Tracker ***************
	field_type type_list[] = {ST_TYPE_INDEX, ST_TYPE_SEPARATOR,ST_TYPE_NOTE, ST_TYPE_SPACE,ST_TYPE_BOOL,ST_TYPE_NIBBLE};
	m_tracker_control = new stTrackerControl(BRect(305,25,440,270),m_instrument->len,m_instrument->sps,type_list,sizeof(type_list)>>2,new BMessage(43),B_NOT_RESIZABLE);

	for (int32 i=0; i< m_instrument->len; i++)
	{
		m_tracker_control->SetFieldContent(i,2, m_instrument->steps[i].note);
		m_tracker_control->SetFieldContent(i,4, m_instrument->steps[i].relative);
		m_tracker_control->SetFieldContent(i,5, m_instrument->steps[i].wform);
	}

// ********* set up the loop textcontrol ***************
	char tmp[10];
	sprintf(tmp,"%d",m_instrument->loop);
	m_loop = new BTextControl(BRect(305,m_tracker_control->Frame().bottom,370,m_tracker_control->Frame().bottom + 20),"loop_textcontrol","loop",tmp,new BMessage(45));

// ********* set up the resonance slider ***************

	float y_pos = frame.bottom;
	BStringView *view;

	AddChild(view = new BStringView(BRect(3,y_pos,110,y_pos+19),"string_view","resonance"));
	view->SetViewColor(216,216,216);
	view->SetLowColor(216,216,216);
	
	m_resonance = new BSlider(BRect(110, y_pos, 380, y_pos + 19), "resonance_slider", 0, new BMessage(46), 0, 255, B_TRIANGLE_THUMB);
	rgb_color col = {136,136,234};
	m_resonance->UseFillColor(true, &col);
	m_resonance->SetHashMarks(B_HASH_MARKS_BOTTOM);
	m_resonance->SetHashMarkCount(25);
//	m_resonance->SetLimitLabels("0","255");
	m_resonance->SetValue(m_instrument->res);
	y_pos += 19;

// ********* set up the vibrato speed slider ***************

	AddChild(view = new BStringView(BRect(3,y_pos,110,y_pos+19),"string_view","vibrato speed"));
	view->SetViewColor(216,216,216);
	view->SetLowColor(216,216,216);
	
	m_vibrato_speed = new BSlider(BRect(110, y_pos, 380, y_pos + 19), "vibrato_speed_slider", 0, new BMessage(47), 0, 255, B_TRIANGLE_THUMB);
	m_vibrato_speed->UseFillColor(true, &col);
	m_vibrato_speed->SetHashMarks(B_HASH_MARKS_BOTTOM);
	m_vibrato_speed->SetHashMarkCount(25);
//	m_vibrato_speed->SetLimitLabels("0","255");
	m_vibrato_speed->SetValue(m_instrument->vibs);
	y_pos += 19;

// ********* set up the vibrato depth slider ***************

	AddChild(view = new BStringView(BRect(3,y_pos,110,y_pos+19),"string_view","vibrato depth"));
	view->SetViewColor(216,216,216);
	view->SetLowColor(216,216,216);
	
	m_vibrato_depth = new BSlider(BRect(110, y_pos, 380, y_pos + 19), "vibrato_depth_slider", 0, new BMessage(48), 0, 255, B_TRIANGLE_THUMB);
	m_vibrato_depth->UseFillColor(true, &col);
	m_vibrato_depth->SetHashMarks(B_HASH_MARKS_BOTTOM);
	m_vibrato_depth->SetHashMarkCount(25);
//	m_vibrato_depth->SetLimitLabels("0","255");
	m_vibrato_depth->SetValue(m_instrument->vibd);
	y_pos += 19;

// ********* set up the pwm speed slider ***************

	AddChild(view = new BStringView(BRect(3,y_pos,110,y_pos+19),"string_view","pwm speed"));
	view->SetViewColor(216,216,216);
	view->SetLowColor(216,216,216);

	m_pwm_speed = new BSlider(BRect(110, y_pos, 380, y_pos + 19), "pwm_speed_slider", 0, new BMessage(49), 0, 255, B_TRIANGLE_THUMB);
	m_pwm_speed->UseFillColor(true, &col);
	m_pwm_speed->SetHashMarks(B_HASH_MARKS_BOTTOM);
	m_pwm_speed->SetHashMarkCount(25);
//	m_pwm_speed->SetLimitLabels("0","255");
	m_pwm_speed->SetValue(m_instrument->pwms);
	y_pos += 19;

// ********* set up the pwm depth slider ***************

	AddChild(view = new BStringView(BRect(3,y_pos,110,y_pos+19),"string_view","pwm depth"));
	view->SetViewColor(216,216,216);
	view->SetLowColor(216,216,216);
	
	m_pwm_depth = new BSlider(BRect(110, y_pos, 380, y_pos + 19), "pwm_depth_slider", 0, new BMessage(50), 0, 255, B_TRIANGLE_THUMB);
	m_pwm_depth->UseFillColor(true, &col);
	m_pwm_depth->SetHashMarks(B_HASH_MARKS_BOTTOM);
	m_pwm_depth->SetHashMarkCount(25);
//	m_pwm_depth->SetLimitLabels("0","255");
	m_pwm_depth->SetValue(m_instrument->pwmd);
	y_pos += 19;

	m_box->AddChild(m_resonance);
	m_box->AddChild(m_vibrato_speed);
	m_box->AddChild(m_vibrato_depth);
	m_box->AddChild(m_pwm_speed);
	m_box->AddChild(m_pwm_depth);


	m_box->AddChild(m_amplitude_envelope);
	m_box->AddChild(m_filter_envelope);
	m_box->AddChild(m_tracker_control);
	m_box->AddChild(m_loop);

	m_zoom = 1;
}

stInstrumentWindow::~stInstrumentWindow()
{
}

bool stInstrumentWindow::QuitRequested()
{
	m_main_win->CloseInsWin(m_instrument);
	return true;
}

void stInstrumentWindow::UpdateTitle()
{
	char * tmp = new char[strlen(m_instrument->name ) + 10];
	sprintf(tmp,"%x.%s", m_index, m_instrument->name);
	SetTitle(tmp);
	delete tmp;
}

void stInstrumentWindow::MessageReceived(BMessage *message)
{
	switch (message->what)
	{
	case 41:
		{
		uint8 action;
		message->FindInt8("action",(int8*)&action);
		switch (action)
		{
		case ST_ADD_KNOB:
			{
			if (m_instrument->amppoints == INSPOINTS) return;
			m_amplitude_envelope->AddKnob();
			m_instrument->amppoints++;

			BPoint prev_pos(0.0, 0.0);
			BPoint pos;
			
			for (int32 i=0; i < m_amplitude_envelope->KnobCount(); i++)
			{
				pos = m_amplitude_envelope->KnobPosition(i);
				if (pos.x - prev_pos.x > 255)
				{
					pos = BPoint(prev_pos.x + 255.0,pos.y);
					m_amplitude_envelope->SetKnobPosition(pos,i);
					m_instrument->amp[i].time = 255;
					m_instrument->amp[i].lev = (uint8)(pos.y*2.0);
				}
				else
				{
					m_instrument->amp[i].time = (uint8)(pos.x - prev_pos.x);
					m_instrument->amp[i].lev = (uint8)(pos.y * 2.0);
				}
				prev_pos = pos;
			}

			}break;
		case ST_DELETE_KNOB:
			{
			if (m_instrument->amppoints == 1) return;
			m_amplitude_envelope->DeleteKnob();
			m_instrument->amppoints--;
			
			BPoint prev_pos(0.0, 0.0);
			BPoint pos;
			
			for (int32 i=0; i < m_amplitude_envelope->KnobCount(); i++)
			{
				pos = m_amplitude_envelope->KnobPosition(i);
				if (pos.x - prev_pos.x > 255)
				{
					pos = BPoint(prev_pos.x + 255.0,pos.y);
					m_amplitude_envelope->SetKnobPosition(pos,i);
					m_instrument->amp[i].time = 255;
					m_instrument->amp[i].lev = (uint8)(pos.y*2.0);
				}
				else
				{
					m_instrument->amp[i].time = (uint8)(pos.x - prev_pos.x);
					m_instrument->amp[i].lev = (uint8)(pos.y * 2.0);
				}
				prev_pos = pos;
			}
			

			}break;
		case ST_MOVE_KNOB:
			{
			int8 index;
			message->FindInt8("knob",&index);

//			printf("ST_MOVE_KNOB %d\n",index);

			BPoint pos = m_amplitude_envelope->KnobPosition(index);

			BPoint prev_pos;
			if (index > 0) prev_pos = m_amplitude_envelope->KnobPosition(index - 1);
			else prev_pos = BPoint(0.0, 0.0);

			int32 bias = (int32)((pos.x - prev_pos.x) - m_instrument->amp[index].time);

			m_instrument->amp[index].lev = (uint8)(pos.y * 2.0);
			m_instrument->amp[index].time += bias;

			BPoint next_pos = pos;
			while (bias && index < m_amplitude_envelope->KnobCount() - 1)
			{
				index++;
				pos = next_pos;
				next_pos = m_amplitude_envelope->KnobPosition(index);
				
				if (m_instrument->amp[index].time - bias > 255)
				{
					bias = m_instrument->amp[index].time - bias - 255;
					m_instrument->amp[index].time = 255;
					m_amplitude_envelope->SetKnobPosition(next_pos + BPoint(255,0),index);
				}
				else
				{
					m_instrument->amp[index].time -= bias;
					bias = 0;
				}
			}
		
//			printf("time: %d\n",m_instrument->amp[index].time);

			}break;
		}
		}break;
	case 42:
		{
		uint8 action;
		message->FindInt8("action",(int8*)&action);
		switch (action)
		{
		case ST_ADD_KNOB:
			{
			if (m_instrument->filterpoints == INSPOINTS) return;
			m_filter_envelope->AddKnob();
			m_instrument->filterpoints++;

			BPoint prev_pos(0.0, 0.0);
			BPoint pos;
			
			for (int32 i=0; i < m_filter_envelope->KnobCount(); i++)
			{
				pos = m_filter_envelope->KnobPosition(i);
				if (pos.x - prev_pos.x > 255)
				{
					pos = BPoint(prev_pos.x + 255.0,pos.y);
					m_filter_envelope->SetKnobPosition(pos,i);
					m_instrument->filter[i].time = 255;
					m_instrument->filter[i].lev = (uint8)(pos.y*2.0);
				}
				else
				{
					m_instrument->filter[i].time = (uint8)(pos.x - prev_pos.x);
					m_instrument->filter[i].lev = (uint8)(pos.y * 2.0);
				}
				prev_pos = pos;
			}


			}break;
		case ST_DELETE_KNOB:
			{
			if (m_instrument->filterpoints == 1) return;
			m_filter_envelope->DeleteKnob();
			m_instrument->filterpoints--;
			
			BPoint prev_pos(0.0, 0.0);
			BPoint pos;
			
			for (int32 i=0; i < m_filter_envelope->KnobCount(); i++)
			{
				pos = m_filter_envelope->KnobPosition(i);
				if (pos.x - prev_pos.x > 255)
				{
					pos = BPoint(prev_pos.x + 255.0,pos.y);
					m_filter_envelope->SetKnobPosition(pos,i);
					m_instrument->filter[i].time = 255;
					m_instrument->filter[i].lev = (uint8)(pos.y*2.0);
				}
				else
				{
					m_instrument->filter[i].time = (uint8)(pos.x - prev_pos.x);
					m_instrument->filter[i].lev = (uint8)(pos.y * 2.0);
				}
				prev_pos = pos;
			}
			
			}break;
		case ST_MOVE_KNOB:
			{
			int8 index;
			message->FindInt8("knob",&index);

//			printf("ST_MOVE_KNOB %d\n",index);

			BPoint pos = m_filter_envelope->KnobPosition(index);

			BPoint prev_pos;
			if (index > 0) prev_pos = m_filter_envelope->KnobPosition(index - 1);
			else prev_pos = BPoint(0.0, 0.0);

			int32 bias = (int32)((pos.x - prev_pos.x) - m_instrument->filter[index].time);

			m_instrument->filter[index].lev = (uint8)(pos.y * 2.0);
			m_instrument->filter[index].time += bias;

			BPoint next_pos = pos;
			while (bias && index < m_filter_envelope->KnobCount() - 1)
			{
				index++;
				pos = next_pos;
				next_pos = m_filter_envelope->KnobPosition(index);
				
				if (m_instrument->filter[index].time - bias > 255)
				{
					bias = m_instrument->filter[index].time - bias - 255;
					m_instrument->filter[index].time = 255;
					m_filter_envelope->SetKnobPosition(next_pos + BPoint(255,0),index);
				}
				else
				{
					m_instrument->filter[index].time -= bias;
					bias = 0;
				}
			}
		
//			printf("time: %d\n",m_instrument->filter[index].time);

			}break;

		}
		} break;
	case 43:
		{
			uint8 action;
			message->FindInt8("action",(int8*)&action);
			switch (action)
			{
			case ST_SET_LENGTH:
				{
					stTrackerControl *control;
					BTextControl *text;
					message->FindPointer("control",(void**)&control);
					message->FindPointer("source",(void**)&text);
					uint8 len = atoi(text->Text());
					if (len > INSSTEPS) len = INSSTEPS;
					control->SetLength(len);
					m_instrument->len = len;

					if (m_instrument->loop >= m_instrument->len){
						m_instrument->loop = m_instrument->len -1; 

						char temp[10];
						sprintf(temp,"%d",m_instrument->loop);
						m_loop->SetText(temp);
					}					
					
					for (int32 i=0; i< m_instrument->len; i++)
					{
						m_tracker_control->SetFieldContent(i,2, m_instrument->steps[i].note);
						m_tracker_control->SetFieldContent(i,4, m_instrument->steps[i].relative);
						m_tracker_control->SetFieldContent(i,5, m_instrument->steps[i].wform);
					}
					
				} break;
			case ST_SET_SPEED:
				{
					stTrackerControl *control;
					BTextControl *text;
					message->FindPointer("control",(void**)&control);
					message->FindPointer("source",(void**)&text);
					uint8 speed = atoi(text->Text());
					if (speed == 0) speed = 1;
					m_instrument->sps = speed;
				} break;
			}
		} break;
	case ST_ENTRY_CHANGED:
		{
			int8 row,field,val;
			message->FindInt8("field",&field);
			message->FindInt8("row",&row);
			message->FindInt8("value",&val);

//			printf("ENTRY_CHANGED: %d, %d, %d\n",field,row,val);
	
			switch(field)
			{
				case 2:
					m_instrument->steps[row].note = val;
				break;
				case 4:
					m_instrument->steps[row].relative = val;
				break;
				case 5:
					m_instrument->steps[row].wform = val;
				break;
			}
		}break;

	case ST_INS_NAME:{
		BTextControl *tc;
		message->FindPointer("source",(void**)&tc);
		m_instrument->SetName(tc->Text());
		m_main_win->PostMessage(ST_INS_CHANGED);
		UpdateTitle();
	} break;

	case ST_CLIPPING:{
		m_instrument->clipmode = 15 & message->FindInt32("index");
	} break;

	case ST_BOOST:{
		m_instrument->boost = 15 & message->FindInt32("index");
	} break;
	
	case 44:
		{
			int32 index;
			message->FindInt32("index", &index);
			m_instrument->filtermode = index;
		} break;
	case 45:
		{
		BTextControl *tc;
		message->FindPointer("source",(void**)&tc);
		uint8 value = atoi(tc->Text());
		
		if (value >= m_instrument->len) value = m_instrument->len -1; 

		m_instrument->loop = value;
		char tmp[10];
		sprintf(tmp,"%d",value);
		tc->SetText(tmp);
		
		} break;
	case 46:
		{
			m_instrument->res = m_resonance->Value();
		} break;
	case 47:
		{
			m_instrument->vibs = m_vibrato_speed->Value();
		} break;
	case 48:
		{
			m_instrument->vibd = m_vibrato_depth->Value();
		} break;
	case 49:
		{
			m_instrument->pwms = m_pwm_speed->Value();
		} break;
	case 50:
		{
			m_instrument->pwmd = m_pwm_depth->Value();
		} break;
	}
}
