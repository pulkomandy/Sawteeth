#include <stdio.h> // remove this line
#include "stChannelView.h"
#include "stSeqView.h"

#include <Message.h>
#include <Clipboard.h>

stChannelView::stChannelView(Song *song, Channel *channel, BRect frame) : BView(frame, "Channel",  B_FOLLOW_NONE, B_WILL_DRAW )
{
	m_z=1;
	s = song;
	chan = channel;
	
	int col = 222 - ( (channel-(song->GetChannel(0)) & 1) * 10);
	SetViewColor(col,col,col);
	height = (int)frame.Height();
	
	for (int cnt=0 ; cnt<chan->len ; cnt++){
		ps[cnt] = new stPartSquare(s, chan, cnt, &(chan->steps[cnt].transp), &(chan->steps[cnt].part), &(chan->steps[cnt].damp),BRect(0,0,10,height / 3));
		AddChild(ps[cnt]);
	}

	ResizeTo(100000,height);
	Update();
}

void stChannelView::Update()
{
	int count = 0;
	for (int cnt=0 ; cnt<chan->len ; cnt++){
		Part *tmp = s->GetPart(chan->steps[cnt].part);
		ps[cnt]->SetZoom( m_z );
		int tmps = tmp->len * tmp->sps;
		ps[cnt]->SetStartPAL( count );
		count += tmps; 
		ps[cnt]->SetPALs( tmps );
		ps[cnt]->Invalidate();
	}
}

stChannelView::~stChannelView()
{
//	fprintf(stderr,"stChannelView::~stChannelView()\n");
}

int	stChannelView::Width()
{
	return (int) ps[chan->len - 1]->Frame().right;
}

void stChannelView::SetZoom(float z)
{
//	fprintf(stderr,"Channel[%d].SetZoom()\n",chan - s->GetChannel(0));
	m_z = z;
	for (int c=0; c<chan->len; c++) ps[c]->SetZoom(z);
}

void stChannelView::Draw(BRect )
{
//	puts("stChannelView::Draw()");	
}

void stChannelView::MessageReceived(BMessage *message) 
{
	switch ( message->what ) { 
		case 'setp'://	puts("channview-ch-right");
		{
			//kolla om vi kan gå åt höger
			uint16 tmp = message->FindInt16("part");
			if (tmp < s->PartCount()-1)
				chan->steps[focus].part = tmp;
			else
				chan->steps[focus].part = s->PartCount()-1;
				
			Update();
		} break; 

		case 'mv_r'://	puts("channview-mv-right");
			MakeFocusInd(focus + 1);
		break; 

		case 'mv_l'://	puts("channview-mv-left");
			MakeFocusInd(focus - 1);
		break; 

		case 'mk_r'://	puts("channview-mk-right");
			Mark(focus + 1);
		break; 

		case 'mk_l'://	puts("channview-mk-left");
			Mark(focus - 1);
		break; 

		case 'ch_r'://	puts("channview-ch-right");
			//kolla om vi kan höja volumen
			if (chan->steps[focus].damp < 239){
				chan->steps[focus].damp+=16;
				ps[focus]->Update();
			}else{
				chan->steps[focus].damp = 255;
				ps[focus]->Update();
			}

		break; 

		case 'ch_l'://	puts("channview-ch-left");
			//kolla om vi kan sänka volumen
			if (chan->steps[focus].damp > 16){
				chan->steps[focus].damp-= 16;
				ps[focus]->Update();
			}else{
				chan->steps[focus].damp = 0;
				ps[focus]->Update();
			}
		break; 

		case 'tr_u'://	puts("channview-tr-up");
			Transpose(1);
		break; 

		case 'tr_d'://	puts("channview-tr-down");
			Transpose(-1);
		break;
		default:
			Window()->DetachCurrentMessage();
			Window()->PostMessage(message);
	}
}

void stChannelView::Transpose(int8 t)
{
	if (marking){
		int a = MIN(markstart,focus);
		int b = MAX(markstart,focus);
		for (;a <= b ; a++){
			chan->steps[a].transp += t;
			ps[a]->Update();
		}
	}else{
		chan->steps[focus].transp += t;
		ps[focus]->Update();
	}
}

void stChannelView::MakeSemiFocus()
{
	BMessage *mess = new BMessage(ST_SEMI_FOCUS);
	mess->AddInt16("focus",chan - s->GetChannel(0));
	Window()->PostMessage(mess,Parent());
}

void stChannelView::MakeFocusPAL(uint32 startPAL)
{
	focus = 0;
	while ((focus < chan->len) && (ps[focus]->StartPAL() <= startPAL)) focus++;
	focus --;
	if (focus < 0) return;
	ps[focus]->MakeFocus(true);
}

void stChannelView::MakeFocusInd(int32 ind)
{
	if (marking){
		UnMark();
		marking = false;
	}

	if (ind < 0){
		focus = 0;
		ps[0]->MakeFocus();
		return;
	}
	
	if (ind > chan->len-1){
	////////////////// HAER /////////////
		ps[chan->len] = new stPartSquare(s, chan, chan->len, &(chan->steps[chan->len].transp), &(chan->steps[chan->len].part), &(chan->steps[chan->len].damp), BRect(0,0,10,height / 3));
		ps[chan->len]->SetZoom(m_z);
		AddChild(ps[chan->len]);

		ps[chan->len]->MakeFocus();
		focus = chan->len;
		chan->len ++;
		Update();
		((stSeqView*)Parent())->Update(false,ps[focus]->Frame().left,ps[focus]->Frame().right);
		return;
	}

	focus = ind;
	ps[focus]->MakeFocus();
	((stSeqView*)Parent())->Update(false,ps[focus]->Frame().left,ps[focus]->Frame().right);
}

void stChannelView::UnMark()
{
	for ( int c= 0 ; c < chan->len ; c++){
		ps[c]->Mark(false);
		ps[c]->Invalidate();
	}
	marking = false;
}

void stChannelView::Mark(uint32 ind)
{
	if (!marking) {
		ps[focus]->Mark();
		ps[focus]->Invalidate();
		markstart=focus;
		marking = true;
	}
	if ( ind > (uint32)(chan->len-1)) return;
	ps[ind]->MakeFocus();

	if (ind < markstart)
		// vänster om start
		if (ind < focus)
			// gå vänster
			ps[ind]->Mark(true);
		else
			// gå höger
			ps[focus]->Mark(false);
	else
		// höger eller == om start
		if (ind == markstart)
			ps[focus]->Mark(false);
		else
			if (ind > focus)
				// gå vänster
				ps[ind]->Mark(true);
			else
				// gå höger
				ps[focus]->Mark(false);

	focus = ind;
}

int stChannelView::GetFocusPosition()
{
	int c = 0;
	while (c < chan->len ){
		if (ps[c]->IsFocus()){
			return ps[c]->StartPAL();
		}
		c++;
	}
	return 0;
}

void stChannelView::FindSquare()
{
	focus = chan->len-1;
	while ( focus >0 && !ps[focus]->IsFocus()) focus--;
}

void stChannelView::MovePart(int source, int dest)
{
	if ((source >= chan->len) || (dest >= chan->len)) return;

//	fprintf(stderr,"d = %d, d = %d\n",dest,source);

	chan->steps[dest].part =	chan->steps[source].part;
	chan->steps[dest].transp =	chan->steps[source].transp;
	chan->steps[dest].damp =	chan->steps[source].damp;
}

void stChannelView::Cut() // kör copy och tar bort...
{
	Copy();
	Del();
}
void stChannelView::Del() // kör copy och tar bort...
{
	if (!marking) markstart = focus;
	// 0 1 2 3 4 5 6 7	
	//     s   m
	int a = MIN(markstart,focus);
	int b = MAX(markstart,focus);
	int c = b-a+1; // antal markerade

	// some protection
	if (c == chan->len){
		fprintf(stderr,"stChannelView::Del() can not delete whole channel (copied it though)\n");
		return;
	}

	for (int z = b+1 ; z < chan->len ; z++)
		MovePart(z,z-c);	

	for (int z = chan->len - c ; z < chan->len ; z++){
		RemoveChild(ps[z]);
		delete ps[z];
	}
	chan->len -= c;

	if (chan->rloop > a ) chan->rloop -= c;
	if (chan->lloop > a ) chan->lloop -= c;
	
	if (chan->len >a)
		MakeFocusInd(a);
	else
		MakeFocusInd(a-1);
		
	UnMark();
	Update();
}

void stChannelView::Copy() // kopiera till clipboard
{
	if (!marking) markstart = focus;
	
	BMessage *clip = (BMessage *)NULL; 

	if (be_clipboard->Lock()){ 
	    be_clipboard->Clear(); 
	    clip = be_clipboard->Data();
	    if (clip != 0){
	
			int a = MIN(markstart,focus);
			int b = MAX(markstart,focus);
			uint8 biasedtransp;
			for (int z = a ; z <= b ; z++){
				clip->AddData("application/x-vnd.Jonas_Norberg-Sawteeth-Ins", B_MIME_TYPE, &(chan->steps[z].part), 1);
				biasedtransp = chan->steps[z].transp + 127;
				clip->AddData("application/x-vnd.Jonas_Norberg-Sawteeth-Note", B_MIME_TYPE, &biasedtransp, 1);
				clip->AddData("application/x-vnd.Jonas_Norberg-Sawteeth-Damp", B_MIME_TYPE, &(chan->steps[z].damp), 1);
			}
			be_clipboard->Commit();
		}
    	be_clipboard->Unlock(); 
	}
}

void stChannelView::Paste() // töm clipboard och insert i 
{
	BMessage *clip = (BMessage *)NULL; 

	if (be_clipboard->Lock()) { 
		clip = be_clipboard->Data();
		if (clip){
			type_code typeFound;
			int32 countFound;
			ssize_t temp;
			uint8 *point;
			if ( B_OK == clip->GetInfo("application/x-vnd.Jonas_Norberg-Sawteeth-Ins", &typeFound, &countFound)){

				if (chan->rloop >= focus) chan->rloop += countFound;
				if (chan->lloop >= focus) chan->lloop += countFound;

				// make room for data...
				chan->len += countFound;
				for (uint32 z = chan->len-1 ; z >= focus+countFound ; z--){
					MovePart(z-countFound,z);
				}

				// set their properties				
				for (uint32 z = focus ; z < focus+countFound ; z++){

					if (B_OK == clip->FindData("application/x-vnd.Jonas_Norberg-Sawteeth-Ins", B_MIME_TYPE, z-focus, (const void**)&point, &temp)){
						chan->steps[z].part = *point;
					}else puts("no note");

					if ( B_OK == clip->FindData("application/x-vnd.Jonas_Norberg-Sawteeth-Note", B_MIME_TYPE , z-focus, (const void**)&point, &temp)){
						chan->steps[z].transp = uint8(*point) - 127;
					}else puts("no ins");

					if ( B_OK == clip->FindData("application/x-vnd.Jonas_Norberg-Sawteeth-Damp", B_MIME_TYPE , z-focus, (const void**)&point, &temp)){
						chan->steps[z].damp = uint8(*point);
					}else puts("no damp");
				}

			// create the new views
				for (int cnt = chan->len - countFound ; cnt < chan->len ; cnt++){
					ps[cnt] = new stPartSquare(s, chan, cnt, &(chan->steps[cnt].transp), &(chan->steps[cnt].part), &(chan->steps[cnt].damp), BRect(0,0,10,height / 3));
					AddChild(ps[cnt]);
				}
				Update();	
			}
		be_clipboard->Unlock();
		}
	}

	MakeFocusInd(focus);
	UnMark();
	Update();

}
