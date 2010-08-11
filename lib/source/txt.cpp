#include <stdlib.h>
#include <string.h>

#include "txt.h"

/*_EXPORT*/ txt::txt(char *buffer,int size)
{
	memory = true;
	eof = false;
	bin = false;
	error=JNG_OK;

	buff = buffer;
	bytes_left = size;
	
}

/*_EXPORT*/ txt::txt(const char *filename, bool create)
{
	fname = strdup(filename);
	
	memory = false;
	eof = false;
	bin = false;
	error=JNG_OK;
	if (create)
		f=fopen(filename,"wb");
	else
		f=fopen(filename,"rb");

	if (f==0){
		fprintf(stderr,"%s: no such file\n",filename);
		error=JNG_ERROR;
	}
}

char *txt::FileName(){
	if (memory)
		return "Memory";
	else
		return fname;
}

bool txt::IsMemory()
{
	return memory;
}

/*_EXPORT*/ txt::~txt()
{
	if (memory) return;
	
	free (fname);
	fclose(f);
}

status_t txt::InitCheck()
{
	return error;
}

bool txt::Eof()
{
	return eof;
}

void txt::IsBin(bool b)
{
	bin = b;
}

void txt::jntok(char *b, char *m)
{
	bool cont=true;
	int mc=strlen(m);
	int c;
	while (*b!=0 && cont){
		for (c=0;c<mc;c++)
			if (*b == m[c]){
				*b=0;
				cont=false;
				break;
			}
		b++;
	}	
}

void txt::String(char *txt)
{
	fputs(txt,f);
	if (bin)
		fputc(0,f);
	else
		fputc('\n',f);
}

void txt::Comment(int val)
{
	if (!bin) fprintf(f,"%x",val);
}

void txt::Comment(char *txt)
{
	if (!bin) fputs(txt,f);
}

void txt::Val(int val, int numbytes)
{
	if (bin){
		if (numbytes == 1)
			fputc(val,f);
		else
			for (int c=numbytes; c>0; c--)
				Val((val >> ((c-1) << 3)) & 255, 1);
	}else{
		fprintf(f,"%d\n",val);
	}
}

int txt::AtomRead()
{
	if (memory){
		if (bytes_left > 0){
			uint8 tmp = *buff;
//			printf("%x %c,",tmp,tmp);
			buff ++;
			bytes_left --;
			return tmp;
		}else{
			eof = true;
			return 0;
		}		
	}else{
		if (eof) return 0;
		int tmp = fgetc(f);
		if (tmp < 0){
			eof = true;
			return 0;
		}
		return tmp;
	}
}

int txt::NextVal(int numbytes)
{
	if (bin){
		// binary file
		if (numbytes == 1){
			return AtomRead();
		}

		int tmp=0;
		for (int c=numbytes;c>0;c--)
			tmp |= AtomRead() << ((c-1) << 3);
		
//		printf("\ttxt : NextVal(%d) == %8x\n",numbytes,tmp);
		return tmp;

	}else{
		// text file
		do {
			NextString();
			if (eof) return 0;
			jntok(buf,"/\n");		
		} while ( !eof && (*buf)==0);

		return atoi(buf);	
	}
}

float txt::NextFloat()
{
	if (!bin){
		NextString();
		return (float)atof(buf);
	}
	
	int n = NextVal(4);
	float *f = (float *)&n;
	return (*f);
}

char *txt::NextString()
{
	int tmp,c;
	for ( c = 0 ; (c < BUFSIZE-1) ; c++ ){
		tmp = AtomRead();
		if (eof){
			buf[c] = 0;
			return buf;
		}
		if (tmp == 0 || tmp == '\n') break;
		buf[c]=tmp;
	}

	if (c >= BUFSIZE-1) buf[0] = 0;
	buf[c]=0;
	
	//fprintf(stderr,"txt::NextString()\t%s\n",buf);
	//return strdup(buf);
	return buf;
}

