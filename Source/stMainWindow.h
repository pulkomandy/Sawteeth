#ifndef _INSTRUMENT_WINDOW_H
#define _INSTRUMENT_WINDOW_H

#include <Window.h>
#include <Path.h>
#include <MenuBar.h>
#include <Bitmap.h>

#include "stPartWindow.h"
#include "stInstrumentWindow.h"
#include "stSeqView.h"
#include "stsp.h"
#include "stdefs.h"

class stInstrumentWindow;


class stMainWindow : public BWindow
{
public:
	stMainWindow(BRect frame, txt &flat);
	status_t InitCheck();
	~stMainWindow();
	// virtual void FrameMoved(BPoint scrPt);
	// virtual void FrameResized(float w, float h);
	// virtual void MenusBeginning(void);
	// virtual void MenusEnding(void);
	// virtual void Minimize(bool min);
	// virtual void ScreenChanged(BRect frame, color_space mode);
	// virtual void WindowActivated(bool active);
	// virtual void WorkspaceActivated(int32 workspc, bool active);
	// virtual void WorkspaceChanged(int32 oldworkspc, int32 newworkspc);
	// virtual void Zoom(BPoint lefttop, float wide, float high);
	virtual void MessageReceived(BMessage *message);

	void UpdateTitle();
	void ClosePartWin(Part *part);
	void CloseInsWin(Ins *ins);
	
	Song *GetSong();
private:
	void SetFileType(const char *filename);
	void ExportAIFF(BFile &f);
	void ConstructMixWin();
	void UnmuteAll();
	BSlider *mw[CHN*2];
	
	void BuildPartMenu();
	void BuildInstrumentMenu();

	status_t init;
	
	BWindow *mixwin;
	
	stPartWindow *pw[PARTS]; // alla parts.
	stInstrumentWindow *iw[INS]; // alla instrumet.

	BFilePanel *save, *aiff_save, *release_save; // the save-panel

	float zoom;
	BPath *file_name;
	stSeqView *st_seq_view;
	BScrollView *st_scroll_view;
	BMenuBar *menu; // the menu
	BMenu *part_menu; // the part menu
	BMenu *dup_part_menu; // the duplicate partmenu
	BMenu *dup_ins_menu; // the duplicate instrument menu
	BMenu *instrument_menu; // the instrument menu
	BMenuItem *save_item;
	BMenuItem *play_item;
	bool playing;
	
	stsp *song;
};

#endif // _INSTRUMENT_WINDOW_H
