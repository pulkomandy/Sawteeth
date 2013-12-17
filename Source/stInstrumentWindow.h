/* Copyright 2001, Arvid and Jonas Norberg
Distributed under the terms of the MIT Licence. */

#ifndef _ST_INSTRUMENT_WINDOW_H_
#define _ST_INSTRUMENT_WINDOW_H_

#include "stCurveControl.h"
#include "stTrackerControl.h"
#include "stChannelView.h"
#include "stMainWindow.h"
#include "stSong.h"

class stTrackerView;

class stInstrumentWindow:public BWindow
{
public:
	stInstrumentWindow(uint8 index, stMainWindow *main_win,BPoint point,Ins *instrument);
	~stInstrumentWindow();
	virtual bool QuitRequested();
	virtual void MessageReceived(BMessage *message);
private:
	void UpdateTitle();
		
	uint8 m_index;
	stCurveControl *m_amplitude_envelope;
	stCurveControl *m_filter_envelope;

	stTrackerControl *m_tracker_control;
	float m_zoom;
	Ins *m_instrument;
	stMainWindow *m_main_win;

	BTextControl *m_loop;
	
	BSlider *m_resonance;
	BSlider *m_vibrato_speed;
	BSlider *m_vibrato_depth;
	BSlider *m_pwm_speed;
	BSlider *m_pwm_depth;
};

#endif // _ST_INSTRUMENT_WINDOW_H_
