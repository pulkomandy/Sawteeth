/* Copyright 2001, Arvid and Jonas Norberg
Distributed under the terms of the MIT Licence. */

#ifndef _ST_TRACKER_ENTRY_H_
#define _ST_TRACKER_ENTRY_H_

#include <View.h>
#include <ScrollBar.h>

const rgb_color ST_HIGH_COLOR = {0,0,0,255};
const rgb_color ST_LOW_COLOR = {216,216,216,255};

extern char keynote_list[];
//extern const char *note_list[];

enum
{
	ST_ENTRY_CHANGED = 0,
	ST_ENTRY_DELETE,
	ST_ENTRY_INSERT,
	ST_NEW_PART_NAME,
	ST_NEW_PART_SPEED,
	MSG_FROM_TRACKER
};

enum field_type
{
	ST_TYPE_SPACE=0,
	ST_TYPE_SEPARATOR,
	ST_TYPE_INDEX,

	ST_TYPE_NIBBLE,
	ST_TYPE_BLANK_NIBBLE,
	ST_TYPE_BOOL,
	ST_TYPE_NOTE
};


class stTrackerEntry:public BView
{
friend class stTrackerView;
public:
	stTrackerEntry(uint8 num_fields,field_type *type_list);
	~stTrackerEntry();
	
	virtual void MakeFocus(bool focused=true);
	virtual void Draw(BRect rect);
	virtual void MouseDown(BPoint point);
	virtual void KeyDown(const char *bytes,int32 numBytes);
	
	uint8 FieldContent(uint8 index);
	void SetFieldContent(uint8 index,uint8 data);

	bool MoveCursorLeft();
	bool MoveCursorRight();

	uint8 CurrentRow();

	void MoveCursorUp(bool select = false);
	void MoveCursorDown(bool select = false);
	
	bool IsSelected();
	
	static void PreferredSize(BRect *rect,field_type *type_list,uint8 num_fields);
private:

	struct field
	{
		field_type type;
		uint8 content;
	};

	uint8 current_field; // the index of the currently selected field
	field *fields;
	uint8 field_count;
	uint16 font_width;
	static uint8 field_width[7];
};

// inline definitions

inline uint8 stTrackerEntry::FieldContent(uint8 index)
{
	if (index<field_count) return fields[index].content;
	else return 0;
}

inline void stTrackerEntry::SetFieldContent(uint8 index,uint8 data)
{
	if (index<field_count) fields[index].content=data;
	Invalidate();
}

#endif // _ST_TRACKER_ENTRY_H_
