/* Copyright 2001, Arvid and Jonas Norberg
Distributed under the terms of the MIT Licence. */

#include <stdio.h>
#include <stdlib.h>

#include "stPartWindow.h"
#include "stCurveControl.h"
#include "stTrackerView.h"
#include "stTrackerEntry.h"
#include "stApp.h"
#include "song.h"

stPartWindow::stPartWindow(stMainWindow *win, BPoint point, Part *part_to_edit,
	Song *song)
	: BWindow(BRect(point, point + BPoint(300, 300)), "",
		B_FLOATING_WINDOW_LOOK,	B_FLOATING_SUBSET_WINDOW_FEEL,
		B_NOT_H_RESIZABLE)
{
	main_win = win;
	part = part_to_edit;
	s = song;

	// fixa shortcuts för att sätta ut breakpoints
	for (int c = '0' ; c <= '9' ; c++) {
		BMessage *mess = new BMessage(ST_ADD_BP);
		mess->AddInt32("cmd", c);
		AddShortcut(c, B_COMMAND_KEY, mess);
	}

	AddShortcut('r', B_COMMAND_KEY, new BMessage(ST_DEL_BP));

	field_type type_list[] = {
		ST_TYPE_INDEX,
		ST_TYPE_SEPARATOR,
		ST_TYPE_SPACE,
		ST_TYPE_NOTE,
		ST_TYPE_SPACE,
		ST_TYPE_BLANK_NIBBLE,
		ST_TYPE_NIBBLE,
		ST_TYPE_SPACE,
		ST_TYPE_BLANK_NIBBLE,
		ST_TYPE_NIBBLE,
		ST_TYPE_SPACE};

	BRect rect = BRect(0,0,0,0);
	BMenuBar* menu = new BMenuBar(rect, "menu");
		BMenu* partMenu = new BMenu("part");
			// TODO : switch the label to Stop while playing
			BMenuItem* play = new BMenuItem("▶ Play", new BMessage(ST_PPLAY_START), ' ', B_COMMAND_KEY);
			partMenu->AddItem(play);
		menu->AddItem(partMenu);

	float bottom = menu->Bounds().bottom;
	bottom += 40;

	tracker_control = new stTrackerControl(BRect(5, bottom, 120, bottom + 235), part->len,
		part->sps, type_list, sizeof(type_list)>>2,
		new BMessage(MSG_FROM_TRACKER), B_FOLLOW_TOP_BOTTOM);
	float right_border = tracker_control->Frame().right;

	bottom -= 20;

	name_string = new BTextControl(BRect(5, bottom, right_border, bottom + 20),
		"name_string", "Name:", part->name, new BMessage(ST_NEW_PART_NAME));
	name_string->SetDivider(30);

	FullUpdate(0, part->len - 1);

	// I should use a menu here ?
	/*
	BButton* delButton = new BButton(BRect(25, 0, 50, 10), "delete", "✗",
		new BMessage('DELE'));
	delButton->SetFontSize(14);
	*/

	ResizeTo(right_border + 5.0, 300);

	BRect frame = Bounds();
	frame.bottom += 1.0;
	frame.right += 1.0;

	BBox *box = new BBox(frame, "background", B_FOLLOW_ALL_SIDES);

	AddChild(box);
	box->AddChild(menu);
	box->AddChild(tracker_control);
	box->AddChild(name_string);
	UpdateTitle();
}


stPartWindow::~stPartWindow()
{
	main_win->ClosePartWin(part);
}


void stPartWindow::FullUpdate(int8 low, uint8 high)
{
	for (uint16 i = low; i <= high; i++)
	{
		tracker_control->SetFieldContent(i, 3, part->steps[i].note);

		tracker_control->SetFieldContent(i, 5, (part->steps[i].ins & 0xf0) >> 4);
		tracker_control->SetFieldContent(i, 6, part->steps[i].ins & 0xf);

		tracker_control->SetFieldContent(i, 8, (part->steps[i].eff & 0xf0) >> 4);
		tracker_control->SetFieldContent(i, 9, part->steps[i].eff & 0xf);
	}
}


void stPartWindow::UpdateTitle()
{
	char * tmp = new char[strlen(part->name) + 20];
	sprintf(tmp, "%ld.%s", part - s->GetPart(0), part->name);
	SetTitle(tmp);
	delete tmp;
}


void stPartWindow::MessageReceived(BMessage *message)
{
	switch(message->what)
	{
		case ST_ADD_BP:
		{
			int32 cmd = message->FindInt32("cmd");

			BMessage *mess = new BMessage(ST_ADD_BP);
			mess->AddInt32("cmd", cmd);
			mess->AddInt32("PAL",
				part->sps * tracker_control->TrackerView()->CurrentRow());
			main_win->PostMessage(mess);
		} break;

		case ST_DEL_BP:
		{
			BMessage *mess = new BMessage(ST_DEL_BP);
			mess->AddInt32("PAL",
				part->sps * tracker_control->TrackerView()->CurrentRow());
			main_win->PostMessage(mess);
		} break;

		case ST_PPLAY_START:
		{
			BMessage *mess = new BMessage(ST_PPLAY_START);
			int16 temp = part - s->GetPart(0);
			mess->AddInt16("part", temp);
			mess->AddInt16("step",
				tracker_control->TrackerView()->CurrentRow());

			main_win->PostMessage( mess );
		} break;

		case ST_NEW_PART_NAME:
		{
			part->SetName((char*)name_string->Text());
			UpdateTitle();
			main_win->PostMessage(ST_PART_CHANGED);
		} break;

		case MSG_FROM_TRACKER:
		{
			uint8 action;
			message->FindInt8("action", (int8*)&action);
			switch (action)
			{
				case ST_SET_LENGTH:
				{
					stTrackerControl *control;
					BTextControl *text;
					message->FindPointer("control", (void**)&control);
					message->FindPointer("source", (void**)&text);
					control->SetLength(atoi(text->Text()));

					char tmp[10];
					sprintf(tmp, "%d", atoi(text->Text()));
					text->SetText(tmp);

					part->len = atoi(text->Text());

					for (uint16 i=0;i<part->len;i++)
					{
						tracker_control->SetFieldContent(i,3,part->steps[i].note);

						tracker_control->SetFieldContent(i,5,(part->steps[i].ins & 0xf0) >> 4);
						tracker_control->SetFieldContent(i,6,part->steps[i].ins & 0xf);

						tracker_control->SetFieldContent(i,8,(part->steps[i].eff & 0xf0) >> 4);
						tracker_control->SetFieldContent(i,9,part->steps[i].eff & 0xf);
					}
					main_win->PostMessage(ST_PART_CHANGED);
				}break;
				case ST_SET_SPEED:
				{
					stTrackerControl *control;
					BTextControl *text;
					message->FindPointer("control",(void**)&control);
					message->FindPointer("source",(void**)&text);
					part->sps = atoi(text->Text());
					if (part->sps < 1) part->sps = 1;

					char tmp[10];
					sprintf(tmp,"%d",part->sps);
					text->SetText(tmp);

					main_win->PostMessage(ST_PART_CHANGED);
				} break;
			}
		}break;

		case ST_ENTRY_INSERT:{
			int8 row;
			message->FindInt8("row",&row);
			for (int c = part->len - 1 ; c > row ; c--){
				part->steps[c].note = part->steps[c-1].note;
				part->steps[c].ins = part->steps[c-1].ins;
				part->steps[c].eff = part->steps[c-1].eff;
			}
			part->steps[row].note = 0;
			part->steps[row].ins = 0;
			part->steps[row].eff = 0;
			
			FullUpdate(row,part->len-1);
		} break;
		
		case ST_ENTRY_DELETE:{
			int8 row;
			message->FindInt8("row",&row);
			if (row > 0){
				for (int c = row-1 ; c < part->len-1 ; c++){
					part->steps[c].note = part->steps[c+1].note;
					part->steps[c].ins = part->steps[c+1].ins;
					part->steps[c].eff = part->steps[c+1].eff;
				}
				part->steps[part->len - 1].note = 0;
				part->steps[part->len - 1].ins = 0;
				part->steps[part->len - 1].eff = 0;
			
				FullUpdate(row-1,part->len-1);
			}
		} break;

		case ST_ENTRY_CHANGED:
			{
			int8 row,field,val;
			message->FindInt8("field",&field);
			message->FindInt8("row",&row);
			message->FindInt8("value",&val);
//			printf("ST_ENTRY_CHANGED (%d,%d)=%d\n",field,row,val);
			switch(field)
			{
				case 3: // note
				{
					part->steps[row].note = val;
					s->SetPartPos(row);
				}break;
				case 5:
				{
					part->steps[row].ins &= 0xf;
					part->steps[row].ins |= val << 4;
				 // protection
					if (part->steps[row].ins >= s->InsCount()){
						part->steps[row].ins = s->InsCount()-1;
						tracker_control->SetFieldContent(row,5,(part->steps[row].ins & 0xf0) >> 4);
						tracker_control->SetFieldContent(row,6,part->steps[row].ins & 0xf);
					}
				}break;
				case 6:
				{
					part->steps[row].ins &= 0xf0;
					part->steps[row].ins |= val;
				 // protection
					if (part->steps[row].ins >= s->InsCount()){
						part->steps[row].ins = s->InsCount()-1;
						tracker_control->SetFieldContent(row,5,(part->steps[row].ins & 0xf0) >> 4);
						tracker_control->SetFieldContent(row,6,part->steps[row].ins & 0xf);
					}
				}break;
				case 8:
				{
					part->steps[row].eff &= 0xf;
					part->steps[row].eff |= val << 4;
				}break;
				case 9:
				{
					part->steps[row].eff &= 0xf0;
					part->steps[row].eff |= val;
				}break;
			}
		}
	
		default:{
			DetachCurrentMessage();
			main_win->PostMessage(message);
		}
	}
}
