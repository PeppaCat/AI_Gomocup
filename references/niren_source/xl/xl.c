/*

  Copyright (C) 2005 Chengtao Chen

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License
  as published by the Free Software Foundation; either version 2
  of the License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include "xl_def.h"

#define XLCM_DISP   0xf0
#define XLCM_EXIT   0xf1
#define XLCM_PLAY  	0xf2
#define XLCM_HELP   0xf3
#define XLCM_NEW    0xf4
#define XLCM_OPEN   0xf5
#define XLCM_SAVE   0xf6
#define XLCM_BACK   0xf7
#define XLCM_THINK  0xf8
#define XLCM_LEVEL  0xf9
#define XLCM_INFO   0xfa
//#define XLCM_CANCEL 0xfb
#define XLCM_MODE   0xfc
#define XLCM_UNKOWN 0xff

typedef struct  XLCM_ITEM
{
	int8_t cm;
	uint8_t id;
	uint8_t param_num;
	int8_t *usage;
}XLCM_ITEM;

/////////////////////////
POSFORMAT 	_pos;
uint8_t _map[256];
uint8_t _autoStart;
int _mode=0;

int8_t *Str2Point(uint8_t *p,int8_t *str)
{
	while(*str==' ') str++;
	if(str[0]>='A' && str[0]<='O' && str[1]>='A' && str[1]<='O')
	{
		*p= MAKEPOS(str[0]-'A',str[1]-'A');
		return str+2;
	}
	*p=0xff;
	return 0;
}

void Pos2Map(uint8_t *map, POSFORMAT *p_ps)
{
	uint8_t i;

	memset(map, Empty, 256);

	for( i=0; i<p_ps->num; i++)
	{
		map[p_ps->pos[i]] = SIDE(i);
	}
}

void ShowBoard(POSFORMAT *p_pos)
{
	uint8_t p,map[256];
	int _close_bracket=0;

	//if(_silence) return;

	Pos2Map(map,p_pos);

	putchar('\n');
	puts("\t   a b c d e f g h i j k l m n o");
	ESB(p);
	if(POSX(p)==0) printf( "\t%c ", 'a'+POSY(p));

	if(p_pos->num>0)
	{
		if(p == p_pos->pos[p_pos->num-1])
		{
			putchar('[');
			_close_bracket=1;
		}
		else if(_close_bracket)
		{
			putchar(']');
			_close_bracket=0;
		}
		else
		{
			putchar(' ');
		}
	}
	else
	{
		putchar(' ');
	}

	switch(map[p])
	{
	case Black:
		putchar('X');
		break;
	case White:
		putchar('O');
		break;
	default :
		switch(p)
		{
		case 0x77:
		case 0x33:
		case 0x3b:
		case 0xb3:
		case 0xbb:
			putchar('+');
			break;
		default:
			putchar('.');
		}
		break;
	}
	if(POSX(p)==BW-1)
	{
		if(_close_bracket)
		{
			putchar(']');
			_close_bracket=0;
		}
		putchar('\n');
	}
	ESE();
	putchar('\n');
}

uint8_t GetCommand(int8_t *param)
{
	static XLCM_ITEM _xlcm[]=
	{
		{'O' ,XLCM_OPEN  ,1 ,NULL},
		{'Q' ,XLCM_EXIT  ,0	,NULL},
		{'M' ,XLCM_MODE  ,1	,NULL},
		{'H' ,XLCM_HELP  ,0	,NULL},
		{'N' ,XLCM_NEW   ,0	,NULL},
		{'S' ,XLCM_SAVE  ,1	,NULL},
		{'B' ,XLCM_BACK  ,0	,NULL},
		{'D' ,XLCM_DISP  ,0 ,NULL},
		{'T' ,XLCM_THINK ,1 ,"T {pos_string}"},
		{'L' ,XLCM_LEVEL ,0 ,"L {level}"},
		{'I' ,XLCM_INFO  ,0 ,NULL}
	};

	int8_t input[1024],*p;
	printf("%c[%d]>",SIDE(_pos.num)?'B':'W',_pos.num+1);
	fflush(stdout);

	//get command
	fgets(input,sizeof(input),stdin);

	//make upcase and trim '\n'
	for(p=input;*p;p++)
	{
		if(*p>='a' && *p<='z')*p-='a'-'A';
		if(*p=='\n')
		{
			*p='\0';
			break;
		}
	}
	if(!*input) return XLCM_PLAY;

	//TRACE(input);

	if(input[1]==' ' || !input[1])
	{
		int i;
		for(i=0;i<sizeof(_xlcm)/sizeof(XLCM_ITEM);i++)
		{
			if(input[0] == _xlcm[i].cm )
			{
				strcpy(param,input+2);
				if(_xlcm[i].param_num&&(!input[1]||!input[2]))
				{
					if(_xlcm[i].usage)
					{
						printf("Usage: ");
						puts(_xlcm[i].usage);
						return XLCM_UNKOWN;
					}
					break;
				}
				return _xlcm[i].id;
			}
		}
	}
	else
	{
		uint8_t p;
		if(strlen(input)==2 && Str2Point(&p,input)) return p;
	}
	puts("Invalid command.");
	return XLCM_UNKOWN;
}

void ShowAppInfo(void)
{
	puts("Niren 0.01");
	puts("chenct, 2005");
	puts("");
}

void DisplayUsage(void)
{
	puts("\t----------------------------------------");
	//puts("\tL [num]  - set Level.");
	puts("\tB [num]  - go Back.");
	//puts("\tO [file] - Open file.");
	//puts("\tS [file] - Save to file.");
	puts("\tN        - New game.");
	puts("\tH        - show Help.");
	puts("\tI        - show application Info.");
	puts("\tD        - Display board.");
	puts("\tQ        - Quit.");
	puts("\t{XY}     - put stone on special pos.");
	puts("\t{Enter}  - computer play.");
	puts("\t----------------------------------------");
}

void NewGame(void)
{
	_pos.num=0;
	memset(_map, Empty, sizeof(_map));
	_autoStart=1;
}


bool MoveAt(uint8_t p)
{
	if(!PosInBoard(p) || _map[p]!= Empty)
	{
		puts("Invalid move.");
		return false;
	}

	_pos.pos[_pos.num]=p;
	_map[p]=SIDE(_pos.num);
	_pos.num++;

	ShowBoard(&_pos);
	switch(XlCheckLastMove(&_pos))
	{
	case 2:
		_autoStart=0;
		puts("Five in a row.");
		break;
	case 1:
		_autoStart=0;
		puts("Forbidden move.");
	}
	return true;
}

void MoveBack(void)
{
	//uint8_t p;
	if(_pos.num<=0) return;
	_pos.num--;
	_map[_pos.pos[_pos.num]] = Empty;
	_autoStart=0;
	ShowBoard(&_pos);
}

void ComputerPlay(void)
{
	uint8_t p;

	printf("computer: ");
	if(0xff!=(p = XlSearch(_pos) ))
	{
		printf("#%c%c\n",'A'+POSX(p),'A'+POSY(p));
		MoveAt(p);
		_autoStart=1;
	}
	else
	{
		puts("Pass.");
		_autoStart=0;
	}
}

void SetMode(char *param)
{
	sscanf(param,"%d",&_mode);
	if(_mode<0)_mode=0;
	if(_mode>1)_mode=1;
	printf("Mode = %d\n",_mode);
}

void set_level(char *param)
{
	//TODO: dasf
}



int PointExist(POSFORMAT *p_pos,uint8_t p)
{
	uint8_t i;
	for(i=0;i<p_pos->num;i++)
	{
		if(p==p_pos->pos[i]) return 1;
	}
	return 0;
}

int Str2Pos(POSFORMAT *p_pos,int8_t *buf)
{
	int8_t *src;
	uint8_t p;
	p_pos->num=0;
	for(src=Str2Point(&p,buf) ; src; src=Str2Point(&p,src) )
	{
		if(PointExist(p_pos,p))  return 0;
		p_pos->pos[p_pos->num++]=p;
	}
	return 1;
}

void extern_play(int8_t *param)
{
	POSFORMAT pos;
	uint8_t ret;

	if(Str2Pos(&pos,param))
	{
		//display_board(&pos);
		if(0xff==(ret=XlSearch(pos)))
		{
			puts("Pass.");
		}
		else
		{
			pos.pos[pos.num++]=ret;
			ShowBoard(&pos);
			printf("#%c%c\n",'A'+POSX(ret),'A'+POSY(ret));
		}
	}
}

void DisposeAll(void)
{
	XlDispose();
	puts("#");
}


int main(int argc, int8_t* argv[])
{
	atexit(DisposeAll);

	ShowAppInfo();

	NewGame();
	ShowBoard(&_pos);


	uint8_t cm;
	static int8_t _param[1024];
	while(XLCM_EXIT != (cm= GetCommand(_param)))
	{
		switch(cm)
		{
		case XLCM_INFO:
			ShowAppInfo();
			break;
		case XLCM_HELP:
			DisplayUsage();
			break;
		case XLCM_NEW:
			NewGame();
			ShowBoard(&_pos);
			break;
		case XLCM_DISP:
			ShowBoard(&_pos);
			break;
		case XLCM_PLAY:
			ComputerPlay();
			break;
		case XLCM_BACK:
			MoveBack();
			break;
		case XLCM_THINK:
			extern_play(_param);
			break;
		case XLCM_LEVEL:
			set_level(_param);
			break;
		case XLCM_MODE:
			SetMode(_param);
			break;
		case XLCM_UNKOWN:
			break;
		default:
			if(MoveAt(cm))
			{
				if(_autoStart)
				{
					ComputerPlay();
				}
			}
		}
	}
	return 0;
}

