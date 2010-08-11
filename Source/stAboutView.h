#ifndef _ABOUT_VIEW_H
#define _ABOUT_VIEW_H

#include <Bitmap.h>
#include <View.h>

class stBmView : public BView
{
public:
	stBmView(BRect rect, BBitmap *bm);

	void Draw(BRect);
	void MouseDown(BPoint);

private:
	BBitmap *m_bm;
	char verstr[10];
};

class stAboutView : public BView
{
public:
	stAboutView(BRect rect, int halfseconds);

	void MouseDown(BPoint);
	void Draw(BRect);
	void Pulse();
private:
	int stop;
	int count;
	BRect m_rect;

};

#endif // _ABOUT_VIEW_H
