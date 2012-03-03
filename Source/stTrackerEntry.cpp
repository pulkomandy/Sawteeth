/* Copyright 2001, Arvid and Jonas Norberg
Distributed under the terms of the MIT Licence. */

#include "stTrackerEntry.h"
#include "stTrackerView.h"
#include "stCurveKnob.h"
#include "stApp.h"

#include <Message.h>
#include <Point.h>

// describes the notes mapped to the scancodes
char keynote_list[]=
{
	13,15,-1,18,20,22,-1,25,27,-1,-1,
	-1,-1,-1,-1,-1,-1,-1,-1,-1,
	12,14,16,17,19,21,23,24,26,28,29,
	-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
	-1,1,3,-1,6,8,10,-1,13,15,-1,-1,
	-1,-1,-1,-1,
	0,2,4,5,7,9,11,12,14,16
};

static char hex_list[]=
{
	'0',
	'1',
	'2',
	'3',
	'4',
	'5',
	'6',
	'7',
	'8',
	'9',
	'a',
	'b',
	'c',
	'd',
	'e',
	'f'
};

char *note_list[]=
{
	"c-",
	"c#",
	"d-",
	"d#",
	"e-",
	"f-",
	"f#",
	"g-",
	"g#",
	"a-",
	"a#",
	"b-",
};

stTrackerEntry::stTrackerEntry(uint8 num_fields,field_type *type_list):
	BView("TrackerEntry",B_NAVIGABLE/*_JUMP*/+B_WILL_DRAW)
{
//	printf("stTrackerEntry::stTrackerEntry()\n");
	current_field = 3;
	field_count=num_fields;
	fields=new field[field_count];
	uint8 i;

	SetFont(be_fixed_font);

	uint16 font_width= (uint16) be_fixed_font->StringWidth("0"); // retreives the font width
	field_width[0]=3;
	field_width[1]=3;
	field_width[2]=2*font_width+1;
	field_width[3]=font_width+1;
	field_width[4]=font_width+1;
	field_width[5]=font_width+1;
	field_width[6]=3*font_width+1;

	uint16 width=0;
	for (i=0;i<field_count;i++)
	{
		fields[i].type=type_list[i];
		width+=field_width[fields[i].type];
		if (fields[i].type==ST_TYPE_NOTE) fields[i].content=0;
		else fields[i].content=0;
	}
	font_height height;
	GetFontHeight(&height);
	//ResizeTo(width,height.ascent+height.descent);

	SetViewColor(ST_LOW_COLOR);
	SetLowColor(ST_LOW_COLOR);
	SetHighColor(ST_HIGH_COLOR);

	BRect r;
	PreferredSize(&r, type_list, num_fields);

	BSize sz(r.Width(), r.Height());
	SetExplicitMinSize(sz);
	SetExplicitMaxSize(sz);
}

stTrackerEntry::~stTrackerEntry()
{
}

void stTrackerEntry::PreferredSize(BRect *rect,field_type *type_list,uint8 num_fields)
{
	uint8 i;

	uint16 font_width=(uint16)be_fixed_font->StringWidth("0"); // retreives the font width

	uint8 field_width[5];
	field_width[0]=3;
	field_width[1]=3;
	field_width[2]=2*font_width+1;
	field_width[3]=font_width+1;
	field_width[4]=font_width+1;
	field_width[5]=font_width+1;
	field_width[6]=3*font_width+1;

	uint16 width=0;
	for (i=0;i<num_fields;i++)
		width+=field_width[type_list[i]];

	font_height height;
	be_fixed_font->GetHeight(&height);
	rect->Set(0,0,width,height.ascent+height.descent);
//	printf("preferred size: %d,%f\n",width,height.ascent+height.descent);
}

void stTrackerEntry::MakeFocus(bool focused)
{
	BView::MakeFocus(focused);
	Invalidate();
}

void stTrackerEntry::MouseDown(BPoint point)
{
	uint8 i;
	uint16 x_pos=0;
	for (i=0;i<field_count;i++)
	{
		x_pos+=field_width[fields[i].type];
		if (point.x<x_pos)
		{
			if (fields[i].type>ST_TYPE_INDEX)
			{
				current_field=i;
				MakeFocus();
			}
			break;
		}
	}
}

void stTrackerEntry::KeyDown(const char *bytes,int32 numBytes)
{
	if ( modifiers() & B_SHIFT_KEY && ( *bytes == B_DELETE )){
		for (int c = 0 ; c < field_count ; c++)
		{
			if (FieldContent(c) != 0
			 && fields[c].type != ST_TYPE_SPACE
			 && fields[c].type != ST_TYPE_SEPARATOR
			 && fields[c].type != ST_TYPE_INDEX)
			{
//				printf("field type: %d\n",fields[c].type);
				SetFieldContent(c,0);
				BMessage *msg=new BMessage(ST_ENTRY_CHANGED);

				msg->AddInt8("field",c);
				msg->AddInt8("row",CurrentRow());
				msg->AddInt8("value",FieldContent(c));
				Window()->PostMessage(msg);
			}
		}

		MoveCursorDown();
		Invalidate();

		return;
	}
	
	switch (*bytes)
	{
		case B_BACKSPACE:{
				BMessage *msg=new BMessage(ST_ENTRY_DELETE);
				msg->AddInt8("row",CurrentRow());
				Window()->PostMessage(msg);
				MoveCursorUp();
			}return;
		case B_INSERT:{
				BMessage *msg=new BMessage(ST_ENTRY_INSERT);
				msg->AddInt8("row",CurrentRow());
				Window()->PostMessage(msg);
			}return;
		case B_LEFT_ARROW:
			if (MoveCursorLeft()) Invalidate();
			return;
		case B_RIGHT_ARROW:
			if (MoveCursorRight()) Invalidate();
			return;
		case B_UP_ARROW:
			MoveCursorUp();
			return;
		case B_DOWN_ARROW:
			MoveCursorDown();
			return;
		case B_FUNCTION_KEY:
		{
			BMessage message = *Window()->CurrentMessage();
			int32 key;
			message.FindInt32("key",&key);
			if (key>=B_F1_KEY && key<=B_F12_KEY)
			{
				((stTrackerView*)Parent())->SetOctave(key - B_F1_KEY);
			}
			return;
		}
	}

	uint8 octave = ((stTrackerView*) Parent())->Octave();
//fprintf(stderr,"%d\n",octave);
	BMessage *msg=new BMessage(ST_ENTRY_CHANGED);
	switch (fields[current_field].type)
	{
		case ST_TYPE_NIBBLE:
		case ST_TYPE_BLANK_NIBBLE:
			if (*bytes>='0' && *bytes<='9')
				fields[current_field].content=*bytes-'0';
			else if (*bytes>='a' && *bytes<='f')
				fields[current_field].content=*bytes-'a'+10;
			else if (*bytes==B_DELETE)
				fields[current_field].content=0;
			else
			{
				BView::KeyDown(bytes,numBytes);
				return;
			}
			msg->AddInt8("field",current_field);
			msg->AddInt8("row",CurrentRow());
			msg->AddInt8("value",fields[current_field].content);
/*
			if (current_field<field_count-1 && fields[current_field+1].type>ST_TYPE_INDEX)
				MoveCursorRight();
			else
			{
				if (current_field!=0)
				{
					int8 i=current_field;
					while (--i>=0 && fields[i].type>ST_TYPE_INDEX);
					current_field=i+1;
				}
				MoveCursorDown();
			}
*/
			break;
		case ST_TYPE_BOOL:
			if (*bytes==B_SPACE)
			{
				fields[current_field].content++;
				fields[current_field].content&=1;
			}
			else if (*bytes==B_DELETE)
				fields[current_field].content=0;
			else
			{
				BView::KeyDown(bytes,numBytes);
				return;
			}
			msg->AddInt8("field",current_field);
			msg->AddInt8("row",CurrentRow());
			msg->AddInt8("value",fields[current_field].content);
			break;
		case ST_TYPE_NOTE:
		{
			BMessage *message=Window()->CurrentMessage();
			int32 key;
			message->FindInt32("key",&key); // finds the scancode for the key
			
			if (key>18 && key<86 && keynote_list[key-19]!=-1)
				fields[current_field].content=keynote_list[key-19] + octave * 12;
			else if (*bytes==B_DELETE){
				fields[current_field].content=0;
			}else
			{
				BView::KeyDown(bytes,numBytes);
				return;
			}
			msg->AddInt8("field",current_field);
			msg->AddInt8("row",CurrentRow());
			msg->AddInt8("value",fields[current_field].content);
			break;
		}
		case ST_TYPE_SEPARATOR: break;
		case ST_TYPE_INDEX: break;
		case ST_TYPE_SPACE: break;
	}
	if (modifiers() & B_SHIFT_KEY) MoveCursorDown(true);
	else MoveCursorDown();
	Invalidate();

	Window()->PostMessage(msg);
}

void stTrackerEntry::Draw(BRect )
{
//	printf("stTrackerEntry::Draw()\n");
	uint8 i;
	uint16 x_pos=0;
	char number;
	
//	if (IsSelected())
//		SetViewColor(200,200,200);
//	else
		SetViewColor(ST_LOW_COLOR);
	
	for (i=0;i<field_count;i++)
	{
		switch (fields[i].type)
		{
			case ST_TYPE_SEPARATOR:
			{
				BPoint start(x_pos+(x_pos+field_width[fields[i].type]-1)>>1,0);
				BPoint end=start;
				end+=BPoint(0,Frame().Height()-1);
				StrokeLine(start,end);
			} break;
			case ST_TYPE_NOTE:
				char tmp[4];
				if (fields[i].content == 0)
				{
					tmp[0]=' ';
					tmp[1]='-';
					tmp[2]=' ';
					tmp[3]=0;
				}
				else
				{
					strcpy(tmp,note_list[(fields[i].content)%12]);
					tmp[2]=hex_list[((fields[i].content)/12)];
					tmp[3]=0;
				}
				
				if (i==current_field && IsFocus())
				{
					SetLowColor(50,60,220);
					SetHighColor(255,255,255);
					FillRect(BRect(x_pos,0,x_pos+field_width[fields[i].type]-1,10),B_SOLID_LOW);
					DrawString(tmp,BPoint(x_pos,10));
					SetHighColor(ST_HIGH_COLOR);
					SetLowColor(ST_LOW_COLOR);
				}
				else DrawString(tmp,BPoint(x_pos,10));
				break;
			case ST_TYPE_INDEX:
				number=(fields[i].content>>4)&0xf;
				number=(number<10)?number+'0':number+'a'-10;
				DrawChar(number,BPoint(x_pos,10));
				number=fields[i].content&0xf;
				number=(number<10)?number+'0':number+'a'-10;
				DrawChar(number);
				break;
			case ST_TYPE_NIBBLE:
				number=fields[i].content;
				number=(number<10)?number+'0':number+'a'-10;
				if (i==current_field && IsFocus())
				{
					SetLowColor(50,60,220);
					SetHighColor(255,255,255);
					FillRect(BRect(x_pos,0,x_pos+field_width[fields[i].type]-1,10),B_SOLID_LOW);
					DrawChar(number,BPoint(x_pos,10));
					SetHighColor(ST_HIGH_COLOR);
					SetLowColor(ST_LOW_COLOR);
				}
				else DrawChar(number,BPoint(x_pos,10));
				break;
			case ST_TYPE_BLANK_NIBBLE:
				number=fields[i].content;

				if (number == 0) number = ' ';
				else number=(number<10)?number+'0':number+'a'-10;

				if (i==current_field && IsFocus())
				{
					SetLowColor(50,60,220);
					SetHighColor(255,255,255);
					FillRect(BRect(x_pos,0,x_pos+field_width[fields[i].type]-1,10),B_SOLID_LOW);
					DrawChar(number,BPoint(x_pos,10));
					SetHighColor(ST_HIGH_COLOR);
					SetLowColor(ST_LOW_COLOR);
				}
				else DrawChar(number,BPoint(x_pos,10));
				break;
			case ST_TYPE_BOOL:
			{
				BRect rect(x_pos,0,x_pos+field_width[fields[i].type]-1,10);
				BRect rect2=rect;
				rect2.InsetBy(1,2);
				if (i==current_field && IsFocus())
				{
					SetLowColor(50,60,220);
					SetHighColor(255,255,255);
					FillRect(rect,B_SOLID_LOW);
					if (fields[i].content) FillEllipse(rect2);
					SetHighColor(ST_HIGH_COLOR);
					SetLowColor(ST_LOW_COLOR);
				}
				else if (fields[i].content) FillEllipse(rect2);
				break;
			}
			case ST_TYPE_SPACE: break;
		}
		x_pos+=field_width[fields[i].type];
	}
}

bool stTrackerEntry::MoveCursorLeft()
{
	if (current_field==0) return false;

	int8 i=current_field-1;
	while (i>=0 && fields[i].type<=ST_TYPE_INDEX) i--;
	if (i<0) return false;
	current_field=i;
	return true;
}

bool stTrackerEntry::MoveCursorRight()
{
	if (current_field>field_count-2) return false;

	int8 i=current_field+1;
	while (i<field_count && fields[i].type<=ST_TYPE_INDEX) i++;
	if (i==field_count) return false;
	current_field=i;
	return true;
}

void stTrackerEntry::MoveCursorUp(bool )
{
	((stTrackerView*)Parent())->MoveCursorUp(current_field);
}

void stTrackerEntry::MoveCursorDown(bool )
{
	((stTrackerView*)Parent())->MoveCursorDown(current_field);
}

uint8 stTrackerEntry::CurrentRow()
{
	return ((stTrackerView*)Parent())->CurrentRow();
}
