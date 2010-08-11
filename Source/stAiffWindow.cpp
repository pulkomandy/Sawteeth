#include <Screen.h>
#include <Window.h>
#include <StatusBar.h>
#include <TypeConstants.h>
#include <Box.h>

#include "stAiffWindow.h"

static int32 tf(void*);
static int32 tf(void *p){	((stAiffWindow*)p)->Thread(); return 0; }

#define H 40
stAiffWindow::stAiffWindow(Song &copy_this, BFile &file):
	BWindow(BRect(10,10,200,10+H),"Export AIFF", B_MODAL_WINDOW_LOOK, B_NORMAL_WINDOW_FEEL,B_NOT_RESIZABLE | B_NOT_ZOOMABLE | B_ASYNCHRONOUS_CONTROLS )
{
	song = new Song(copy_this);
	f = file;

	BBox *box = new BBox(BRect(0,0,191,H+1),"name");
	pb = new BStatusBar(BRect(5,5,185,H-5),"pb","Saving Stereo 16bit AIFF...");
	
	box->AddChild(pb);
	AddChild(box);

	BScreen screen(this);
	BRect bigr = screen.Frame();
	BRect rect(10,10,200,10+H);
	
	MoveTo((bigr.right-rect.right) / 2, (bigr.bottom-rect.bottom) / 2);

	//fixa buffrar
	tid = spawn_thread(tf, "Export AIFF" , B_NORMAL_PRIORITY, this);
	resume_thread( tid );
}

stAiffWindow::~stAiffWindow()
{
	delete song;
}

status_t stAiffWindow::InitCheck()
{ return init; }


void Big32(BFile &f,uint32 n);
void Big32(BFile &f,uint32 n)
{
	uint8 tmp;
	tmp = n>>24;			f.Write(&tmp,1);
	tmp = (n>>16)&255;	f.Write(&tmp,1);
	tmp = (n>>8)&255;		f.Write(&tmp,1);
	tmp = n&255;			f.Write(&tmp,1);

}

void Big16(BFile &f,uint16 n);
void Big16(BFile &f,uint16 n)
{
	uint8 tmp;
	tmp = n>>8;		f.Write(&tmp,1);
	tmp = n&255;	f.Write(&tmp,1);
}


void stAiffWindow::Thread()
{
	float max = 0, min = 0;

//	PrintBPMTable(s->Sps(), s->GetPart(0)->sps, s->GetPart(0)->len);

	int sbufs = song->Sps();
	int bufc = song->SimpLen();
	pb->SetMaxValue(bufc);
	float *sbuf = new float[2 * sbufs];
	uint8	*obuf = new uint8[2 * 2 * sbufs];
// AIFF HEADER
	uint32 samplecount =  bufc * sbufs;
	uint32 bytespersample = 2;
	Big32( f,'FORM' );
	Big32( f,(2 * samplecount * bytespersample) + 46 );
	Big32( f,'AIFF' );
	Big32( f,'COMM' );

	Big32( f,18 );
	Big16( f,2);						// channelcount!!
	Big32( f,samplecount );
	Big16( f,8 * bytespersample );		// bitcount!!
	
	Big16( f,0x400E );		// z = 400e - 400e ????
	Big16( f,0xAC44 );		// rate = ac44 >> z
	Big16( f,0x0000 );
	Big16( f,0x0000 );

	Big16( f,0x0000 );

	Big32( f,'SSND' );
	Big32( f,( 2 * samplecount * bytespersample) + 8 );

	Big32( f,0x00000000 );
	Big32( f,0x00000000 );
// END OF HEADER

	song->SetPos(0);
	for(int bigc = 0 ; bigc < bufc ; bigc++){
		song->Play(sbuf,sbufs);
		Lock();
		pb->Update(1);
		Unlock();

		for (int c = 0 ; c< (2 * sbufs); c++){

			//max, min and clipping
			if (sbuf[c] > max) max = sbuf[c];if (sbuf[c] < min) min = sbuf[c];
			if (sbuf[c] > 0.999) sbuf[c] = 0.999; else if (sbuf[c] < -0.999) sbuf[c] = -0.999;
			int16 tmp = (int16)(30000.0 * sbuf[c]);
			
			obuf[c*2] =		tmp >> 8;
			obuf[c*2+1] =	tmp & 255;
		}
	
		// här skall man skriva mycket åt gången
		f.Write(obuf, 2 * 2 * sbufs);
	}

	char *buffer="audio/x-aiff";
	f.WriteAttr("BEOS:TYPE",B_STRING_TYPE,0,buffer,strlen(buffer)+1);

fprintf(stderr,"\nmin = %f, max = %f\n",min,max);
	delete sbuf;
		
	Lock();
	Quit();
}
