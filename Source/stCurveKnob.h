#include <InterfaceKit.h>

#ifndef _ST_CURVE_KNOB_H_
#define _ST_CURVE_KNOB_H_

class stCurveKnob:public BView
{
public:
	stCurveKnob(BPoint point,BRect movable_rect,BMessage *message);
	~stCurveKnob();
	
	void SetIndex(uint8 index);
	uint8 Index();
	
	virtual void Draw(BRect rect);
	void SetMovableRect(BRect rect);
	virtual void MouseDown(BPoint point);
	BRect MoveArea();
	void SetMoveArea(BRect rect,bool last = false);
	BPoint Position();
	float XPosition();
	void MoveTo(BPoint point);
	void MoveBy(float x,float y);
	virtual void MakeFocus(bool focused=true);
	virtual void KeyDown(const char *bytes, int32 numByte);
private:
	void SendMoved();

	uint8 m_index;
	BMessage *m_message;
	BRect moving_rect; // the rectangle the knob can move within
	BRect invalidate_rect; // the rectangle the knob has to invalidate when it moves
	BRect draw_frame;
};

// inline definitions

inline void stCurveKnob::SetIndex(uint8 index)
{
	m_index = index;
}

inline uint8 stCurveKnob::Index()
{
	return m_index;
}

inline BRect stCurveKnob::MoveArea()
{
	return moving_rect;
}

inline void stCurveKnob::SetMoveArea(BRect rect, bool last)
{
	rect.OffsetBy(-2,-2);
	moving_rect=rect;
	invalidate_rect = rect;
	if (last)
	{
		if (moving_rect.right > Parent()->Bounds().right-2) moving_rect.right = Parent()->Bounds().right-2;
		invalidate_rect.right = Parent()->Bounds().right;
	}
}

inline BPoint stCurveKnob::Position()
{
	return Frame().LeftTop()+BPoint(2,2);
}

inline float stCurveKnob::XPosition()
{
	return Frame().left+2;
}

inline void stCurveKnob::MoveBy(float x,float y)
{
	MoveTo(Position()+BPoint(x,y));
}

#endif // _ST_CURVE_KNOB_H_
