/*
 *  Uzebox(tm) Megatris
 *  Copyright (C) 2008-2009  Alec Bourque
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <avr/io.h>
#include <avr/pgmspace.h>
#include <uzebox.h>
#include "uzenet.h"
#include "Megatris.h"

void DoIntro(){//draw logo inline(lets Uzenet start working sooner...)
	InitMusicPlayer(patches);
	SetFontTable(font);
	SetTileTable(intro_tiles);
	ClearVram();
	WaitVsync(15 * (MODE1_FAST_VSYNC+1));		
	TriggerFx(31,0xff,1);
	DrawMap(18,12,map_uzeboxlogo);
	WaitVsync(3);
	DrawMap(18,12,map_uzeboxlogo2);
	WaitVsync(2);
	DrawMap(18,12,map_uzeboxlogo);
	WaitVsync(32 * (MODE1_FAST_VSYNC+1));
	ClearVram();
	WaitVsync(10 * (MODE1_FAST_VSYNC+1));

	u16 r;
	if((r = GetTrueRandomSeed()) == 0) //attempt to get a truly random number(watchdog drift based)
		r = 0b1010110010011101; //sometimes it fails?
	GetPrngNumber(r); //seed our random numbers(will be overrided by server for Uzenet games)
}
/*
u8 VirtualKBMode;
u16 VirtualKBPad;
u8 VirtualKBPadLastCode;
void KeyboardUpdate(){


	u8 key=KeyboardGetKey();

	if(key != 0xF0){//key up/break code?
		
	}
	VirtualKBPadLastCode=key;

	
	if(VirtualKBPadLastCode == 0xE0)


//	KeyboardPoll();//checks for new keys

	//process the keyboard
//	if(KeyboardHasKey()){
//		u8 key=KeyboardGetKey(true);
		//terminal_ProcessKey(key);
//	}
}
*/


int main(){
//	SetUserPreVsyncCallback(&KeyboardUpdate);
	SetUserPostVsyncCallback(&Uzenet_Update);//this will handle the majority of Uzenet functionality behind the scenes
	DoIntro();//inline version of kernel Logo
	SetTileTable(tetrisTiles);
	SetTileMap(map_main+2); //set the map to use for custom asm restore()

	if(!(uzenet_state & UN_SPI_RAM_DETECTED) || !(uzenet_state & UN_ESP8266_DETECTED) || !(uzenet_state & UN_PASS_DETECTED)){
		if(!(uzenet_state & UN_SPI_RAM_DETECTED)){
			Print(10,12,PSTR("SPI RAM NOT DETECTED"));
		}
		if(!(uzenet_state & UN_ESP8266_DETECTED)){
			Print(10,13,PSTR("ESP8266 NOT DETECTED"));
		}
		if(!(uzenet_state & UN_PASS_DETECTED)){
			Print(10,14,PSTR("UZEPASS NOT DETECTED"));
		}

		Print(10,16,PSTR("ONLINE PLAY DISABLED"));
		Print(10,18,PSTR("VERIFY UZENET MODULE"));
		Print(10,19,PSTR("AND RUN SETUP UTILITY"));
		Print(9,21,PSTR("PRESS START TO CONTINUE"));
		TriggerFx(6,0xff,1);
		while(1){
			WaitVsync(1);
			if(ReadJoypad(0)&BTN_START)
				break;
		}
		while(ReadJoypad(0)&BTN_START);
	}
/*
GetPrngNumber(0b00000001);
for(u8 i=0;i<25;i++){
PrintInt(9,i,GetPrngNumber(0),1);PrintInt(21,i,GetPrngNumber(0),1);PrintInt(33,i,GetPrngNumber(0),1);
}
while(1);
*/
#if CPU_PLAYER > 0
	//SetRenderingParameters(110U, 32U);//(FIRST_RENDER_LINE+10,FRAME_LINES-10);
#endif
	for(u8 i=0;i<2;i++){
		fields[i].maxAutoRepeatDelay = DEF_AUTO_REPEAT_DELAY;//players can customize this in game for Uzenet
		fields[i].maxInterRepeatDelay = DEF_INTER_REPEAT_DELAY;
		fields[i].startLevel = DEF_START_LEVEL;
	}

	u8 r;
GAME_TOP:
	r = MainMenu();

	if(r == 0)//single player
		vsMode = 0;
	else//2 player(local) or Uzenet
		vsMode = 1;
	if(r == 2){//Uzenet game
		if(!ConnectMenu())//failed to connect or match?
			goto GAME_TOP;
	}

	while(1){
		if(IsSongPlaying())
			ResumeSong();
		else
			StartSongNo(songNo);
GAME_NEW_MATCH:
		while(1){//new match
			LoadMap();
			initFields();	

			SetTile(14,25,8);
			SetTile(15,25,20);
			PrintHexByte(16,25,0);

			SetTile(22,25,8);
			SetTile(23,25,20);
			PrintHexByte(24,25,0);

		//	if(uzenet_step < UN_STEP_PLAYING) //if uzenet, we already started the song to alert the players of connection
		//		StartSongNo(songNo);

			fields[0].currentState=0;
			fields[1].currentState=0;

			if(uzenet_state & UN_STATE_GOT_NAMES){//draw the network names if applicable
				SpiRamSeqReadStart(0,UN_PLAYER_INFO_BASE);
				for(u8 i=0;i<2;i++){
					for(u8 j=0;j<13;j++){
						SetFont(fields[i].left,fields[i].top,SpiRamSeqReadU8());
					}
				}
				SpiRamSeqReadEnd();
			}

			while(!fields[0].gameOver && !fields[1].gameOver){//on going match
				WaitVsync(1); //syncronize gameplay on vsync (60 hz)
				if(uzenet_error)
					break;

				r = 0;
				for(u8 i=0;i<2;i++){
					fields[i].lastButtons=fields[i].currButtons;
					fields[i].currButtons=ReadJoypad(i);

					if(i == 1 && !vsMode)
						break;
					runStateMachine(i);
					processAnimations(i);
					if((fields[i].currButtons&BTN_START) && !(fields[i].lastButtons&BTN_START)){
						r = OptionsMenu();
						if(r == 0)//resume
							break;
						FadeOut(1,1);
						FadeIn(1,0);
						if(r == 1)//restart
							goto GAME_NEW_MATCH;
						if(r == 2)
							goto GAME_TOP;
						break;
					}
				}
				
			}//on going match
		}//new match
	}

	goto GAME_TOP;
}



void StartSongNo(u8 songNo){

	switch(songNo){
		case 0: StartSong(song_tetrisnt); break;				
		case 1: StartSong(song_korobeiniki); break;	
	}
}



u8 OptionsMenu(){
//if(!Uzenet_Sync(0)){return;}

	u8 dx;
	u8 option[2]={0,0};
	u8 breakout = 0;

	for(u8 i=0;i<2;i++){
		if(i == 1 && ! vsMode)
			break;
		dx = fields[i].left+1;
		fill(fields[i].left,fields[i].top+8,FIELD_WIDTH,FIELD_HEIGHT-8,0);
		Print(dx,14,PSTR("RESUME"));
		Print(dx,15,PSTR("RESTART"));
		Print(dx,16,PSTR("QUIT"));
		Print(dx,18,PSTR("MUSIC:"));
		Print(dx,19,PSTR("GHOST:"));
	}

//	goto FIRST_DRAW; //avoid taking too long before field is overwritten...
Print(0,0,PSTR("1P>ALL YOUR BASE ARE BELONG TO US!!"));
Print(0,1,PSTR("FOR GREAT JUSTICE! MUAHAHA!"));
Print(0,3,PSTR("2P>WHAT YOU SAY?! OH NO HE SET US U"));
Print(0,4,PSTR("P THE BOMB. MAKE YOUR TIME."));
	while(1){
		u8 menuSfx = 0;
		WaitVsync(1);
		for(u8 i=0;i<2;i++){
			if(i == 1 && !vsMode)
				break;

			fields[i].lastButtons=fields[i].currButtons;
			fields[i].currButtons=ReadJoypad(i);
		
			if((fields[i].currButtons&(BTN_START|BTN_A|BTN_B)) && !(fields[i].lastButtons&(BTN_START|BTN_A|BTN_B))){
			
				if(option[i]==0){//resume
					breakout=1;
					break;
				}else if(option[i]==1){//restart
					breakout=2;
					StopSong();
					break;
				}else if(option[i]==2){//quit
					breakout=3;
					break;
				}else if(option[i]==3){//song
					if(IsSongPlaying()){
						StopSong();
					}else{
						StartSongNo(songNo);
					}
				}
			}

			if((fields[i].currButtons&BTN_LEFT) && !(fields[i].lastButtons&BTN_LEFT) && option[i]<5)
				menuSfx=1;

			if((fields[i].currButtons&BTN_LEFT) && !(fields[i].lastButtons&BTN_LEFT)){
				if(option[i]==3 && songNo>0){
					songNo--;
				}else if(option[i]==4){
					fields[i].noGhostBlock=!fields[i].noGhostBlock;
				}else if(option[i]==5 && fields[i].startLevel>0){
					fields[i].startLevel--;
				}else if(option[i]==6 && fields[i].maxAutoRepeatDelay > MIN_AUTO_REPEAT_DELAY){
					fields[i].maxAutoRepeatDelay--;
				}else if(option[i]==7 && fields[i].maxInterRepeatDelay > MIN_INTER_REPEAT_DELAY){
					fields[i].maxInterRepeatDelay--;
				}	
			}
			if((fields[i].currButtons&BTN_RIGHT)&&!(fields[i].lastButtons&BTN_RIGHT)){
				if(option[i]==3 && songNo<3){
					songNo++;
				}else if(option[i]==4){
					fields[i].noGhostBlock=!fields[i].noGhostBlock;
				}else if(option[i]==5 && fields[i].startLevel<30){
					fields[i].startLevel++;
					if(fields[i].level < fields[i].startLevel)
						fields[i].level++;
				}else if(option[i]==6 && fields[i].maxAutoRepeatDelay < MAX_AUTO_REPEAT_DELAY){
					fields[i].maxAutoRepeatDelay++;
				}else if(option[i]==7 && fields[i].maxInterRepeatDelay < MAX_INTER_REPEAT_DELAY){
					fields[i].maxInterRepeatDelay++;
				}	
			}

			if((fields[i].currButtons&(BTN_UP|BTN_DOWN|BTN_SELECT)) && !(fields[i].lastButtons&(BTN_UP|BTN_DOWN|BTN_SELECT))){
				menuSfx=1;

				if(fields[i].currButtons&BTN_UP){
					if(option[i]==0){
						option[i]=7;
					}else{
						option[i]--;
					}

				}else if(fields[i].currButtons&(BTN_DOWN|BTN_SELECT)){
					if(option[i]==7){
						option[i]=0;
					}else{
						option[i]++;
					}
				}
			}

			if(menuSfx){
				menuSfx = 0;
				TriggerFx(1,0x90,1);
			}

		}
		if(breakout)
			break;
FIRST_DRAW:
		for(u8 i=0;i<2;i++){
			if(i == 1 && !vsMode)
				continue;
			dx = fields[i].left+1;
			PrintHexByte(dx+6,18,songNo);
			Print(dx+6,19,(fields[i].noGhostBlock) ? PSTR("OFF"):PSTR("ON "));
			PrintByte(dx+7,20,fields[i].startLevel,1);
			Print(dx,20,PSTR("LEVEL:"));
			PrintByte(dx+7,21,fields[i].maxAutoRepeatDelay,1);
			Print(dx,21,PSTR("AUTOR:"));
			PrintByte(dx+7,22,fields[i].maxInterRepeatDelay,1);
			Print(dx,22,PSTR("INTER:"));

			for(u8 i=14;i<14+9;i++) //erase old cursor
				SetTile(dx-1,i,0);
			SetTile(dx-1,14+option[i]+(option[i]>2?1:0),CURSOR_TILE);
		}	
	}

	for(u8 i=0;i<2;i++){
		if(i == 1 && !vsMode)
			break;
		restore(fields[i].left,fields[i].top+2,FIELD_WIDTH,FIELD_HEIGHT-2);
		for(u8 cy=2;cy<FIELD_HEIGHT;cy++){
			for(u8 cx=0;cx<FIELD_WIDTH;cx++){
				if(fields[i].surface[cy][cx]!=0){
					SetTile(fields[i].left+cx,fields[i].top+cy,fields[i].surface[cy][cx]);
				}
			}				
		}
		updateGhostPiece(i,0);
	}
//sync?
	return breakout-1;
}


int ConnectMenu(){

	ClearVram();

	u8 error = 0;
	u8 connected = 0;
	u16 timer = 0;
	u8 anim0 = 0;
	u8 anim1 = 0;
	u8 anim2 = 0;
	u8 anim3 = 0;
	while(1){

		WaitVsync(1);
		ClearVram();
		DrawMap(3,3,map_title);

		if(connected || error)//waiting or error is indefinite
			timer = 0;
		else{
			timer++;
			uzenet_state |= UN_FIND_OPPONENT;
		}
//if(timer == 30)uzenet_step=UN_CONNECT_SERVER;
anim0++;
if(anim0 > 2){
	anim0 = 0;
	anim1++;
	anim2 = anim1>>1;
	anim3 = anim2>>1;
}


u16 c;
		if(!error){
			if(!(uzenet_state&UN_SERVER_DETECTED)){
				Print(9,16,PSTR("CONNECTING TO UZENET...."));
			}else{
				Print(9,16,PSTR("WAITING FOR OPPONENT...."));
				Print(9,19,PSTR("PRESS A FOR BOT MATCH"));
			}
		}else{//some error happened
			Print(4,14,PSTR("ERROR 0X"));
			PrintHexByte(12,14,uzenet_error);
			Print(4,16,PSTR("NETWORK ERROR OR MODULE FAILURE"));
			Print(4,18,PSTR("CHECK INTERNET OR RUN SETUP TOOL"));
			Print(4,20,PSTR("IF THIS PROBLEM PERSISTS"));
		c = ReadJoypad(0);
		if(c & BTN_B){
			while(ReadJoypad(0) != 0); //wait for key release
			TriggerFx(5,0xff,1);
			FadeOut(1,1);
			FadeIn(1,0);
			if(uzenet_error)
				uzenet_step = 0;//if it's an error, it could be transient...try to start it all over(will take a bit)
			return 0;//go back to main menu
		}
		}
		Print(error?4:11,22,PSTR("PRESS B TO CANCEL       "));
//+((anim3+1)>>3)%5
//drawTetramino(pgm_read_byte(&sin_tablex[(anim2%sizeof(sin_tablex))]),pgm_read_byte(&sin_tabley[(anim2%sizeof(sin_tabley))]),0,anim1&3,0,0,0);
//drawTetramino(pgm_read_byte(&sin_tablex[(anim1%sizeof(sin_tablex))]),pgm_read_byte(&sin_tabley[(anim3%sizeof(sin_tabley))]),1,anim2&3,0,0,0);
drawTetramino(pgm_read_byte(&sin_tablex[(anim3%sizeof(sin_tablex))]),pgm_read_byte(&sin_tabley[(anim1%sizeof(sin_tabley))]),2,anim3&3,0,0,0);
//drawTetramino(pgm_read_byte(&sin_tablex[(anim2%sizeof(sin_tablex))]),pgm_read_byte(&sin_tabley[(anim3%sizeof(sin_tabley))]),3,anim1&3,0,0,0);
//drawTetramino(pgm_read_byte(&sin_tablex[(anim1%sizeof(sin_tablex))]),pgm_read_byte(&sin_tabley[(anim2%sizeof(sin_tabley))]),4,anim2&3,0,0,0);
drawTetramino(pgm_read_byte(&sin_tablex[(anim3%sizeof(sin_tablex))]),pgm_read_byte(&sin_tabley[(anim3%sizeof(sin_tabley))]),5,anim3&3,0,0,0);


//		if(c & BTN_A)
//			UartSendChar('C');//since we are in simple 2P matchmaking mode, this is all we need to do and the server will make a bot join

		if(error)
			continue;

		for(uint8_t j=29;j<35;j++){//animate "progress bar"

			if(((timer&31)/8) > j-29)
				continue;
			SetTile(j,16,0);
		}

		if(uzenet_error || (!(uzenet_state&UN_SERVER_DETECTED) && (timer == ((60*4)-1)))){ //something timed out, indicating an unrecoverable error
			error = 1;
			TriggerFx(21,0xff,1);
		}

		if(uzenet_step >= UN_STEP_PLAYING)
			break;
//		if(connected && !error && uzenet_step > UN_CONNECT_SERVER) //now in game?
//			break;
	}

	TriggerFx(25,0xff,1); //alert the players
	StartSongNo(songNo);

	u8 ready1 = 0;
	u8 ready2 = 0;
	u8 got1 = 0;
	//u8 got2 = 0;
	u8 requested2 = 0;
	timer = 0;
	//fields[0].surface[0] = fields[1].surface[0] = 0;//prepare a place to keep the player names
//	char *pname = fields[0].surface;
//	UartSendChar(UN_COMMAND_UPPERCASE);//send only uppercase strings
//	UartSendChar(UN_GET_PLAYER_NAME1);
	DrawMap(0,0,map_main);
	while(1){//wait for each player to be ready(there might have been a long wait before finding an opponent...)
		WaitVsync(2);

		u16 c1 = ReadJoypad(0);
		u16 c2 = ReadJoypad(1);
		Print(15,19,PSTR("PRESS B TO"));
		Print(15,20,PSTR("DISCONNECT"));

		if(c1 & BTN_B){
			while(ReadJoypad(0) != 0); //wait for key release
			TriggerFx(5,0xff,1);
			FadeOut(1,1);
			FadeIn(1,0);
			return 0;//go back to main menu
		}

	/*	if(!got1){//still need player 1 name?
			s16 c = UartReadChar();
			if(c != -1){
				c &= 0xff;
				
			}
		}
		if(fields[0].surface[0] && !requested2){//already drew player 1 name, but not player 2 name?
			requested2 = 1;//don't request again
			UartSendChar(UN_GET_PLAYER_NAME2);//get name of second player from the server...
		}
*/
		if(!vram[(VRAM_TILES_H*19)+9] || !vram[(VRAM_TILES_H*21)+9]){ //didn't get player names yet? wait until we do, so we can copy it to the game field
			Print(9,18,PSTR("RETRIEVING DATA...."));

			continue;
		}

	//	Print(9,16,PSTR("PRESS START"));		
	//	if((timer&63) > 31)
	//		Print(9,16,PSTR("           "));
		if(!ready1 && (c1 & BTN_START)){
			TriggerFx(25,0xff,1);
			ready1 = 1;
		}
		if(!ready2 && (c2 & BTN_START)){
			TriggerFx(25,0xff,1);
			ready2 = 1;
		}
if(ready1)
	ready2 = 1;
		if(ready1)
			Print(2,11,PSTR("   READY  "));
		else
			Print(2,11,PSTR("PUSH START"));
		if(ready2)
			Print(28,11,PSTR("   READY  "));
		else
			Print(28,11,PSTR("PUSH START"));
		if(ready1 && ready2)
			break;

	}
	return 1;
}



u8 MainMenu(){

	u8 option = 0;

	while(1){
		WaitVsync(1);
		ClearVram();
		DrawMap(3,3,map_title);

//		if(uzenet_step >= UN_STEP_CHECK_READY)//Uzenet backend must have found an RSVP match(setup before the game was booted?)
//			return 3;

		Print(15,16,PSTR("[1]SINGLE"));
		Print(15,17,PSTR("[2]VERSUS"));
		Print(15,18,PSTR("[3]UZENET"));

		Print(11,21,PSTR("KEYBOARD SUPPORTED"));
		Print(11,23,PSTR("\\2008 ALEC BOURQUE"));
		Print(7,24,PSTR("LICENSED UNDER GNU GPL V3"));
		Print(9,25,PSTR("HTTPS://WWW.UZEBOX.ORG"));

		for(u8 i=0;i<2;i++){

			fields[i].lastButtons=fields[i].currButtons;
			fields[i].currButtons=ReadJoypad(i);

			if((fields[i].currButtons&(BTN_A|BTN_START)) && !(fields[i].lastButtons&(BTN_A|BTN_START))){
				FadeOut(1,1);
				FadeIn(2,0);
				return option;
			}

			if((fields[i].currButtons&(BTN_UP|BTN_DOWN|BTN_SELECT)) && !(fields[i].lastButtons&(BTN_UP|BTN_DOWN|BTN_SELECT))){

				TriggerFx(1,0x90,1);

				if((fields[i].currButtons&BTN_UP)){
					if(--option>2) //rollover
						option=2;

				}else{ //BTN_DOWN|BTN_SELECT
					if(++option>2)
						option=0;
				}
			}
			SetTile(14,16+option,CURSOR_TILE); //draw cursor
		}
	}

	return 0;
}



void processAnimations(u8 f){ //animate non-locking stuff
	u8 dx,frame;

	if(fields[f].backToBackAnimFrame>0){ //BACK-TO-BACK
		if(f==0){
			dx=14;		
		}else{
			dx=21;
		}
		u8 dy=16;

		switch(fields[f].backToBackAnimFrame){			
			case 1:
				restore(dx,dy,5,3);
				break;
			case 3:		
			case 80:
				DrawMap(dx,dy,map_anim_backtoback1);
				break;
			case 6:
			case 77:			
				DrawMap(dx,dy,map_anim_backtoback2);
				break;
			case 9:
			case 74:			
				DrawMap(dx,dy,map_anim_backtoback3);
				break;


		}	
		fields[f].backToBackAnimFrame--;
	}
		
	if(fields[f].tSpinAnimFrame>0){ //T-SPIN
		if(f==0){
			dx=14;		
		}else{
			dx=21;
		}
		u8 dy=19;

		switch(fields[f].tSpinAnimFrame){			
			case 1:
				restore(dx,dy,7,4);
				break;
			case 3:			
			case 90:
				DrawMap(dx,dy,map_anim_spark1);
				break;
			case 6:
			case 87:				
				DrawMap(dx,dy,map_anim_spark2);
				break;
			case 9:
			case 84:				
				DrawMap(dx,dy,map_anim_spark3);
				break;
			case 2:
			case 81:
				DrawMap(dx,dy,map_anim_spark4);
				break;			
			case 78:
				DrawMap(dx+1,dy+1,map_anim_tspin);
				if(fields[f].lastClearCount==1){			
					DrawMap(dx+1,dy+2,map_anim_single);
				}else if(fields[f].lastClearCount==2){
					DrawMap(dx+1,dy+2,map_anim_double);
				}else if(fields[f].lastClearCount==3){
					DrawMap(dx+1,dy+2,map_anim_triple);
				}
				
		}	
		fields[f].tSpinAnimFrame--;
	}

	frame=fields[f].tetrisAnimFrame;
	if(frame>0){ //TETRIS
		if(f==0){
			dx=14;		
		}else{
			dx=21;
		}
		u8 dy=19;

		switch(frame){	
			case 1:
				restore(dx,dy,7,4);
				break;
			case 2 ... 3:			
			case 88 ... 90:
				DrawMap(dx,dy,map_anim_spark1);
				break;
			case 4 ... 6:
			case 85 ... 87:			
				DrawMap(dx,dy,map_anim_spark2);
				break;
			case 9: //... 9:
			case 82 ... 84:		
				DrawMap(dx,dy,map_anim_spark3);
				break;
			case 10 ... 12:
			case 79 ... 81:
				DrawMap(dx,dy,map_anim_spark4);
				break;			
			
			default:
											
				switch((frame&15)>>2){
					case 0:
						DrawMap(dx+1,dy+1,map_anim_tetris1);				
						break;
					case 1:
						DrawMap(dx+1,dy+1,map_anim_tetris2);				
						break;
					case 2:
						DrawMap(dx+1,dy+1,map_anim_tetris3);				
						break;
					case 3:
						DrawMap(dx+1,dy+1,map_anim_tetris2);				
						break;
				}				
		}	
		fields[f].tetrisAnimFrame--;
	}
}


void runStateMachine(u8 p){
	switch(fields[p].currentState){
		case 0: //moving
			if(processGravity(p)==1){
				TriggerFx(4,0x90,1);
				fields[p].subState=0;
				fields[p].currentState=1;
				break;				
			}

			if(processControls(p)==1){
				fields[p].currentState=6;
			}
			break;
		case 1: //piece on ground
			if(lockBlock(p)==1){
				fields[p].subState=0;
				fields[p].currentState=2;
			}
			break;
		case 2: //piece locked
			if(animField(p)==1){
				fields[p].subState=0;
				fields[p].currentState=3;
			}
			break;			
		case 3: //done locking
			if(updateField(p)==1){
				fields[p].subState=0;
				fields[p].currentState=4;
			}
			break;
		case 4: //processing garbage

//if(!Uzenet_Sync(0)){return;}

			if(processGarbage(p)==1){
				fields[p].subState=0;
				fields[p].currentState=5;			
			}
			break;	
		case 5: //issuing new block
			issueNewBlock(p);
			updateGhostPiece(p,0);
			fields[p].currentState=0;
			break;			
		case 6: //dropping block
			hardDrop(p);
			TriggerFx(4,0x90,1);
			fields[p].subState=0;
			fields[p].currentState=1;
	}
}


void initFields(void){

	for(u8 x=0;x<2;x++){
		fields[x].level=fields[x].startLevel;
		fields[x].gravity=30-(fields[x].startLevel);
		fields[x].currGravity=0;
		fields[x].currLockDelay=0;
		fields[x].locking=0;
		fields[x].bottom=25;
		fields[x].top=4;
		fields[x].gameOver=0;
		//fields[x].lastButtons=0;
		fields[x].autoRepeatDelay=fields[x].maxAutoRepeatDelay;
		fields[x].interRepeatDelay=fields[x].maxInterRepeatDelay;
		fields[x].kickUp=0;		
		fields[x].lines=0;
		fields[x].nextLevel=10;
		fields[x].score=0;
		fields[x].height=1;
		fields[x].currBlockX=3;
		fields[x].currBlockY=0;
		fields[x].currBlockRotation=0;
		fields[x].holdBlock=NO_BLOCK;
		fields[x].nextBlock=0;
		fields[x].canHold=1;
		fields[x].ghostBlockY=UNDEFINED;
		fields[x].bagPos=7;
		fields[x].nextBlockPosY=13;
		fields[x].holdBlockPosY=21;
		fields[x].garbageQueue=0;
		fields[x].backToBack=0;
		fields[x].tSpin=0;
		fields[x].tSpinAnimFrame=0;
		fields[x].tetrisAnimFrame=0;
		fields[x].lastHole=0;
	}
	//set field specifics
	fields[0].left=2;
	fields[0].right=13;
	fields[0].nextBlockPosX=14;
	fields[0].holdBlockPosX=14;

	fields[1].left=28;
	fields[1].right=39;
	fields[1].nextBlockPosX=22;
	fields[1].holdBlockPosX=22;

	for(u8 y=0;y<FIELD_HEIGHT;y++){
		for(u8 x=0;x<FIELD_WIDTH;x++){
			fields[0].surface[y][x]=0;
			fields[1].surface[y][x]=0;
		}
	}

	for(u8 i=0;i<2;i++){
		issueNewBlock(i);
		fields[i].currBlock=fields[i].nextBlock;
		issueNewBlock(i); //issue twice to have a "next" block randomly defined
		updateField(i);
		updateGhostPiece(i,0);
	}
}



u8 processGravity(u8 p){

	if(fields[p].locking){ //check lock delay
		fields[p].currLockDelay++;
		if(fields[p].currLockDelay>LOCK_DELAY){
			fields[p].locking=0;
			fields[p].currLockDelay=0;
			fields[p].currGravity=0;
			return 1;
		}
	}else{

		fields[p].currGravity++;
		if(fields[p].currGravity>=fields[p].gravity){
			fields[p].currGravity=0;
			//attemp to move block down
			if(!moveBlock(p, fields[p].currBlockX,fields[p].currBlockY+1)){
				fields[p].locking=1;
			}
		}
	}
	return 0;
}



u8 lockBlock(u8 p){

	switch(fields[p].subState){

		case 0:
			drawTetramino(fields[p].currBlockX+fields[p].left,fields[p].currBlockY+fields[p].top,fields[p].currBlock,fields[p].currBlockRotation,10,0,1);
			break;
		case 1:
			drawTetramino(fields[p].currBlockX+fields[p].left,fields[p].currBlockY+fields[p].top,fields[p].currBlock,fields[p].currBlockRotation,12+fields[p].currBlock,0,1);
			break;

		default:
			drawTetramino(fields[p].currBlockX+fields[p].left,fields[p].currBlockY+fields[p].top,fields[p].currBlock,fields[p].currBlockRotation,0,0,1);
			//draw block on surface
			s8 s=pgm_read_byte(&(tetraminos[fields[p].currBlock].size));

			for(s8 cy=0;cy<s;cy++){
				for(s8 cx=0;cx<s;cx++){
					s8 tile=pgm_read_byte(&(tetraminos[fields[p].currBlock].blocks[fields[p].currBlockRotation][(cy*s)+cx]));
					if(tile!=0){
						fields[p].surface[fields[p].currBlockY+cy][fields[p].currBlockX+cx]=fields[p].currBlock+1;
					}
				}
			}

			fields[p].currLockDelay=0;
			fields[p].locking=0;
			fields[p].hardDroppped=0;

			//detect potential t-spin
			if(fields[p].currBlock==T_TETRAMINO && fields[p].lastOpIsRotation==1){
				//check if at least 3 corners are occupied
				u8 occupied=0;

				if(fields[p].surface[(u8)fields[p].currBlockY][(u8)fields[p].currBlockX]!=0)occupied++;
				if(fields[p].surface[(u8)fields[p].currBlockY][(u8)fields[p].currBlockX+2]!=0)occupied++;
				if(fields[p].surface[(u8)fields[p].currBlockY+2][(u8)fields[p].currBlockX]!=0)occupied++;
				if(fields[p].surface[(u8)fields[p].currBlockY+2][(u8)fields[p].currBlockX+2]!=0)occupied++;
				if(occupied>=3){
					fields[p].tSpin=1;
				}else{
					fields[p].tSpin=0;
				}
			}
			return 1;
	}

	fields[p].subState++;
	return 0;
}


u8 moveBlock(u8 p, s8 x,s8 y){
	u8 updateGhost=(fields[p].currBlockX!=x);

	//check if block can fit in the location
	if(!fitCheck(p,x,y,fields[p].currBlock,fields[p].currBlockRotation)) return 0;

	//erase previous block
	if(updateGhost) updateGhostPiece(p,1);
	drawTetramino(fields[p].currBlockX+fields[p].left,fields[p].currBlockY+fields[p].top,fields[p].currBlock,fields[p].currBlockRotation,0,1,1);	

	fields[p].currBlockX=x;
	fields[p].currBlockY=y;

	//draw new one
	if(updateGhost) updateGhostPiece(p,0);
	drawTetramino(fields[p].currBlockX+fields[p].left,fields[p].currBlockY+fields[p].top,fields[p].currBlock,fields[p].currBlockRotation,0,0,1);

	fields[p].lastOpIsRotation=0;

	return 1;
}


s8 randomize(void){

	return (s8)(GetPrngNumber(0) % 7);

}


void issueNewBlock(u8 p){

	if(p && !vsMode)
		return;
	s8 b1,b2,b3,b4,b5,b6,b7;
	s8 next=0;
	s8 nextnext=0;
	if(fields[p].bagPos==7){ //time to fill a new bag 
//if(!Uzenet_Sync(0)){return;}				
		b1=randomize();
		do{b2=randomize();}while (b2==b1);
		do{b3=randomize();}while (b3==b2);
		do{b4=randomize();}while (b4==b3);
		do{b5=randomize();}while (b5==b4);
		do{b6=randomize();}while (b6==b5);
		do{b7=randomize();}while (b7==b6);

		fields[p].bag[0]=b1;
		fields[p].bag[1]=b2;
		fields[p].bag[2]=b3;
		fields[p].bag[3]=b4;
		fields[p].bag[4]=b5;
		fields[p].bag[5]=b6;
		fields[p].bag[6]=b7;

		next=b1;
		nextnext=b2;
		fields[p].bagPos=1;
	}else{
		next=fields[p].bag[fields[p].bagPos++];
		nextnext=fields[p].bag[fields[p].bagPos];
	}

	drawTetramino(fields[p].nextBlockPosX,fields[p].nextBlockPosY,fields[p].nextBlock,0,0,1,0);
	drawTetramino(fields[p].nextBlockPosX,fields[p].nextBlockPosY+3,fields[p].nextBlock,0,0,1,0);

	drawTetramino(fields[p].nextBlockPosX,fields[p].nextBlockPosY,next,0,0,0,0);
	drawTetramino(fields[p].nextBlockPosX,fields[p].nextBlockPosY+3,next,0,0,0,0);

	fields[p].currBlock=fields[p].nextBlock;
	fields[p].nextBlock=next;
	fields[p].currBlockX=3;
	fields[p].currBlockY=0;
	fields[p].currBlockRotation=0;
	fields[p].kickUp=0;
	fields[p].canHold=1;
	
	//check if game over
	if(!moveBlock(p, fields[p].currBlockX,fields[p].currBlockY))
		doGameOver(p);
}


void updateGhostPiece(u8 p, u8 restore){ //this is an expensive function(up to 14613 cycles on empty well?)

	if((!vsMode && p == 1) || (uzenet_step >= UN_STEP_PLAYING && (p != uzenet_local_player))) //save some cycles, don't draw for what we can't control
		return;

	if(!fields[p].noGhostBlock){

		s16 y=fields[p].currBlockY;
		while(1){
			if(!fitCheck(p,fields[p].currBlockX,y,fields[p].currBlock,fields[p].currBlockRotation)) break;
			y++;
		}

		drawTetramino(fields[p].currBlockX+fields[p].left,y+fields[p].top-1,fields[p].currBlock,fields[p].currBlockRotation,GHOST_TILE,restore,1);
	}
}


void hardDrop(u8 p){

	s16 y=fields[p].currBlockY;
	while(1){
		if(!fitCheck(p,fields[p].currBlockX,y,fields[p].currBlock,fields[p].currBlockRotation)) break;
		y++;
	}
	
	moveBlock(p, fields[p].currBlockX,y-1);
	
	fields[p].locking=0;
	fields[p].currLockDelay=0;
	fields[p].currGravity=0;
	fields[p].hardDroppped=1;

}


void hold(u8 p){

	if(fields[p].canHold){
		//erase current block from field
		updateGhostPiece(p,1);
		drawTetramino(fields[p].currBlockX+fields[p].left,fields[p].currBlockY+fields[p].top,fields[p].currBlock,fields[p].currBlockRotation,0,1,1);

		if(fields[p].holdBlock==NO_BLOCK){

			fields[p].holdBlock=fields[p].currBlock;

			//update the hold block
			drawTetramino(fields[p].holdBlockPosX,fields[p].holdBlockPosY,fields[p].holdBlock,0,0,0,0);

			issueNewBlock(p);
			updateGhostPiece(p,0);	
		}else{
			//swap the hold block and current block
			drawTetramino(fields[p].holdBlockPosX,fields[p].holdBlockPosY,fields[p].holdBlock,0,0,1,0);
			drawTetramino(fields[p].holdBlockPosX,fields[p].holdBlockPosY,fields[p].currBlock,0,0,0,0);


			s16 temp=fields[p].currBlock;
			fields[p].currBlock=fields[p].holdBlock;
			fields[p].holdBlock=temp;

			fields[p].currBlockX=3;
			fields[p].currBlockY=0;
			fields[p].currBlockRotation=0;
			fields[p].kickUp=0;

			fields[p].currLockDelay=0;
			fields[p].locking=0;

			//check if game over
			if(!fitCheck(p,fields[p].currBlockX,fields[p].currBlockY,fields[p].currBlock,fields[p].currBlockRotation))
				doGameOver(p);
			
			updateGhostPiece(p,0);
		}

		fields[p].canHold=0;
	}
}


u8 processControls(u8 p){
	s8 dispX=0,dispY=0;

	//process hard drop
	if((fields[p].currButtons&BTN_UP) && !(fields[p].lastButtons&BTN_UP) && !(fields[p].currButtons&(BTN_RIGHT|BTN_LEFT))){
		hardDrop(p);
		return 1;
	}

	//process hold
	if(((fields[p].currButtons&BTN_SELECT) && !(fields[p].lastButtons&BTN_SELECT)) || ((fields[p].currButtons&BTN_SR) && !(fields[p].lastButtons&BTN_SR)) ){
		hold(p);
		return 0;
	}

	//process left-right-down
	if(fields[p].currButtons&BTN_RIGHT){
		if(!(fields[p].lastButtons&BTN_RIGHT) || (fields[p].autoRepeatDelay>fields[p].maxAutoRepeatDelay && fields[p].interRepeatDelay>fields[p].maxInterRepeatDelay)){
			dispX=1;
		}
	}

	if(fields[p].currButtons&BTN_LEFT){
		if(!(fields[p].lastButtons&BTN_LEFT) || (fields[p].autoRepeatDelay>fields[p].maxAutoRepeatDelay && fields[p].interRepeatDelay>fields[p].maxInterRepeatDelay)){
			dispX=-1;
		}
	}

	if(fields[p].currButtons&BTN_DOWN){
		if(fields[p].softDropDelay>SOFT_DROP_DELAY){
			dispY=1;
			fields[p].softDropDelay=0;
		}
	}

	if((dispX!=0 || dispY!=0) && moveBlock(p, fields[p].currBlockX+dispX,fields[p].currBlockY+dispY)){
		fields[p].currLockDelay=0;
		fields[p].locking=0;
		fields[p].interRepeatDelay=0;
		TriggerFx(5,0x40,1);
	}

	if(fields[p].currButtons&(BTN_LEFT|BTN_RIGHT)){
		fields[p].autoRepeatDelay++;
		fields[p].interRepeatDelay++;
	}else{
		fields[p].autoRepeatDelay=0;
		fields[p].interRepeatDelay=0;
	}

	if(fields[p].currButtons&BTN_DOWN){
		fields[p].softDropDelay++;
	}else{
		fields[p].softDropDelay=0;
	}

	//process rotation
	s8 rot=0;
	if(fields[p].currButtons&BTN_A && !(fields[p].lastButtons&BTN_A)){
		//rotate right
		rot=fields[p].currBlockRotation+1;
		if(rot==4) rot=0;
		TriggerFx(5,0x40,1);
		rotateBlock(p,rot);
	}
	if(fields[p].currButtons&BTN_B && !(fields[p].lastButtons&BTN_B)){
		//rotate right
		if(fields[p].currBlockRotation==0){
			rot=3;
		}else{
			rot=fields[p].currBlockRotation-1;
		}
		TriggerFx(5,0x40,1);
		rotateBlock(p,rot);
	}

	return 0;
}


u8 fitCheck(u8 p, s8 x,s8 y,s8 block,s8 rotation){
	//check if block overlaps existing blocks

	s16 s=pgm_read_byte(&(tetraminos[block].size));

	for(s8 cy=0;cy<s;cy++){
		for(s8 cx=0;cx<s;cx++){
			s8 tile=pgm_read_byte(&(tetraminos[block].blocks[rotation][(cy*s)+cx]));
			if(tile!=0){
				//check field boundaries
				if((y+cy)>FIELD_HEIGHT-1 || (x+cx)<0 || (x+cx)>FIELD_WIDTH-1) return 0;

				//check for collisions with surface blocks
				if(fields[p].surface[y+cy][x+cx]!=0) return 0;

			}
		}
	}
	return 1;
}


u8 rotateBlock(u8 p, s8 newRotation){
	s8 x,y;
	u8 rotateRight=0;

	if(fields[p].currBlockRotation==3 && newRotation==0){
		rotateRight=1;
	}else if(fields[p].currBlockRotation==0 && newRotation==3){
		rotateRight=0;
	}else if(newRotation > fields[p].currBlockRotation){
		rotateRight=1;
	}

	x=fields[p].currBlockX;
	y=fields[p].currBlockY;

	if(fields[p].currBlock==O_TETRAMINO) return 1; //can't rotate the square tetramino


	if(!fitCheck(p,x,y,fields[p].currBlock,newRotation)){
		//try simple wall kicks
		if(fitCheck(p,x+1,y,fields[p].currBlock,newRotation)){
			x++;
		}else{
			if(fitCheck(p,x-1,y,fields[p].currBlock,newRotation)){
				x--;
			}else{
				if(fitCheck(p,x,y-1,fields[p].currBlock,fields[p].currBlockRotation) && fitCheck(p,x,y-1,fields[p].currBlock,newRotation)){
					y--;
					fields[p].kickUp=1;
				}else{
					//special srs moves
					if(rotateRight==1 && fitCheck(p,x-1,y+1,fields[p].currBlock,newRotation)){
						x--;
						y++;
					}else if(rotateRight==1 && fitCheck(p,x-1,y+2,fields[p].currBlock,newRotation)){
						x--;
						y+=2;
					}else if(rotateRight==0 && fitCheck(p,x+1,y+1,fields[p].currBlock,newRotation)){
						x++;
						y++;
					}else if(rotateRight==0 && fitCheck(p,x+1,y+2,fields[p].currBlock,newRotation)){
						x++;
						y+=2;
					}else{
						//test 2 blocks for I tetramino
						if(fitCheck(p,x+2,y,fields[p].currBlock,newRotation)){
							x+=2;
						}else{
							if(fitCheck(p,x-2,y,fields[p].currBlock,newRotation)){
								x-=2;
							}else{
								return 0;
							}	
						}

					}
				}
			}
		}
	}

	//if past rotations on same block resulted in a "kick-up", insure this rotation will not result in a space under it
	if(fields[p].kickUp){
		if(fitCheck(p,x,y+1,fields[p].currBlock,newRotation)){
			y++;
		}
	}

	updateGhostPiece(p,1); //erase previous block
	drawTetramino(fields[p].currBlockX+fields[p].left,fields[p].currBlockY+fields[p].top,fields[p].currBlock,fields[p].currBlockRotation,0,1,1);

	fields[p].currBlockRotation=newRotation;
	fields[p].currBlockX=x;
	fields[p].currBlockY=y;

	updateGhostPiece(p,0); //draw new one
	drawTetramino(fields[p].currBlockX+fields[p].left,fields[p].currBlockY+fields[p].top,fields[p].currBlock,fields[p].currBlockRotation,0,0,1);

	fields[p].lastOpIsRotation=1;
	fields[p].currLockDelay=0;
	fields[p].locking=0;

	return 1;
}


u8 lineCleared(u8 p, s8 y){

	u8 cleared=1;
	for(u8 x=0;x<10;x++){
		if(fields[p].surface[y][x]!=0){
			cleared=0;
			break;
		}		
	}
	return cleared;
}


u8 lineFull(u8 p, s8 y){

	u8 full=1;
	for(u8 x=0;x<10;x++){
		if(fields[p].surface[y][x]==0){
			full=0;
			break;
		}		
	}
	return full;
}


u8 animField(u8 p){ //animate the cleared lines
	u8 y,size,tile,temp;

	size=pgm_read_byte(&(tetraminos[fields[p].currBlock].size));

	if(size+fields[p].currBlockY>FIELD_HEIGHT){
		size=FIELD_HEIGHT-fields[p].currBlockY;
	}
	
	switch(fields[p].subState){
		
		case 0 ... 1:
			fields[p].lastClearCount=0;
			for(y=0;y<size;y++){
				if(lineFull(p,fields[p].currBlockY+y)){				
					fill(fields[p].left,y+fields[p].currBlockY+fields[p].top,FIELD_WIDTH,1,10);			
					fields[p].lastClearCount++;
				}		
			}

			if(fields[p].tSpin==1){
				fields[p].tSpinAnimFrame=90;
				TriggerFx(19,0xff,1);
			}else if(fields[p].lastClearCount==4){
				fields[p].tetrisAnimFrame=90;
				TriggerFx(20,0xff,1);
			}else if(fields[p].lastClearCount==0){
				return 1;				
			}else{
				//at least one line completed
				TriggerFx(18,0xc0,1);
			}
			break;

		case 2 ... 23:
			if(fields[p].lastClearCount<4){
				switch(fields[p].subState){
					case 2 ... 6:
						tile=22;
						break;
					case 8 ... 12:
						tile=24;
						break;
					case 14 ... 16:
						tile=26;
						break;
					default:
						tile=28;
						break;
				}
				fields[p].subState++;
			}else{
				tile=22+(fields[p].subState/2);
				
			}
			

			for(y=0;y<size;y++){
				if(lineFull(p,fields[p].currBlockY+y)){		

					if((y+fields[p].currBlockY)<8){
						temp=tile+8;				
					}else{
						temp=tile;
					}

					if(temp>=37) temp=0;

					fill(fields[p].left,y+fields[p].currBlockY+fields[p].top,FIELD_WIDTH,1,temp);						
				}		
			}
			break;

		default:
			for(y=0;y<size;y++){		
				if(lineFull(p,fields[p].currBlockY+y)){
					//we have a completed row!
					fillFieldLine(p,y+fields[p].currBlockY,0);
				}		
			}
			return 1;  //last animation frame
	}

	fields[p].subState++;
	return 0;
}


u8 updateField(u8 p){

	//scan the surface for completed rows
	s16 y,size,tail,clearCount,top,garbageLines=0,fx;	
	u8 difficult=0;

	size=pgm_read_byte(&(tetraminos[fields[p].currBlock].size));

	if(size+fields[p].currBlockY>FIELD_HEIGHT){
		size=FIELD_HEIGHT-fields[p].currBlockY;
	}

	clearCount=fields[p].lastClearCount;
	if(clearCount>0){

		if(fields[p].subState==0){

				tail=size-1;

				for(top=0;top<size;top++){
	
					if(lineCleared(p,fields[p].currBlockY+tail)){			
						copyFieldLines(p,fields[p].currBlockY,fields[p].currBlockY+1,tail);
					}else{
						tail--;
					}

				}


		}else if(fields[p].subState<=clearCount){
				//move rest of field down
				copyFieldLines(p,fields[p].subState-1,fields[p].subState,fields[p].currBlockY);

		}else{	
			//clear top lines
			for(y=0;y<clearCount;y++){
				fillFieldLine(p,y,0);
			}
				
			TriggerFx(10,0xa0,1);

			//Process garbage lines 
			if(clearCount>0){

				if(fields[p].tSpin==1){
					garbageLines=2*clearCount;
				}else{
					if(clearCount==2){
						garbageLines=1;
					}else if(clearCount==3){
						garbageLines=2;
					}else if(clearCount==4){
						garbageLines=4;						
					}
				}

				if(garbageLines==4 || fields[p].tSpin==1){
					difficult=1;
				}

				if(difficult && fields[p].backToBack!=0){
					garbageLines++;
					fx=22+fields[p].backToBack-1;
					if(fx>25)fx=25;

					TriggerFx(fx,0xff,1);
					fields[p].backToBackAnimFrame=90;
				}

				//send garbage lines to the other field(in 2 players mode)
				if(vsMode){
					if(fields[p].garbageQueue){
						if(fields[p].garbageQueue >= garbageLines){
							fields[p].garbageQueue -= garbageLines;
							garbageLines = 0;
						}else{
							garbageLines -= fields[p].garbageQueue;
							fields[p].garbageQueue = 0;
						}	
					}
						fields[1].garbageQueue+=garbageLines;
				}
				PrintHexByte(16,25,fields[0].garbageQueue);
				PrintHexByte(24,25,fields[1].garbageQueue);
			}

			//update score, lines, etc
			if(clearCount!=0){
				fields[p].lines+=clearCount;
				u16 bonus;
				if(clearCount==1){
					bonus=100;
				}else if(clearCount==2){
					bonus=300;
				}else if(clearCount==3){
					bonus=500;
				}else{
					bonus=800;
				}

				if(fields[p].backToBack!=0)
					bonus=(bonus*3)/2;

				if(difficult){				
					fields[p].backToBack++;
				}else{
					fields[p].backToBack=0;						
				}

				fields[p].score+=(fields[p].level*fields[p].height)*bonus;

				if(fields[p].score>999999)
					fields[p].score=999999;
			}		

			if(fields[p].lines > fields[p].nextLevel){
				//increase speed
				if(fields[p].gravity>0){
					fields[p].gravity--;
				}
				fields[p].level+=1;
				fields[p].nextLevel+=LINES_PER_LEVEL;
			}
			fields[p].tSpin=0;
			return 1;
		}
	}else{
		fields[p].tSpin=0;
		return 1;
	}

	//print scores, lines,etc
//	u8 x = p?27:19;

//	PrintLong(x,18,fields[p].lines);
//	PrintLong(x,21,fields[p].level);
//	PrintLong(x,24,fields[p].score);	

	fields[p].subState++;
	return 0;
}


void copyFieldLines(u8 p, s8 rowSource,s8 rowDest,s8 len){

	if(len>0){
		s8 rs=rowSource+len-1;
		s8 rd=rowDest+len-1;

		for(s8 y=0;y<len;y++){
			for(s8 x=0;x<10;x++){
				fields[p].surface[rd][x]=fields[p].surface[rs][x];
				if(rd+fields[p].top>=(fields[p].top+2)){
					if(fields[p].surface[rs][x]==0){
						RestoreTile(x+fields[p].left,rd+fields[p].top);
					}else{	
						SetTile(x+fields[p].left,rd+fields[p].top,fields[p].surface[rs][x]);
					}
				}
			}
			rs--;
			rd--;
		}		
	}
}


void fill(s8 x,s8 y,s8 width,s8 height,u8 tile){
	
	for(u8 cy=0;cy<height;cy++){
		for(u8 cx=0;cx<width;cx++){		
			SetTile(x+cx,y+cy,tile);
		}
	}
}


void restore(s16 x,s16 y,s16 width,s16 height){ //uses custom assembly function
	
	for(u8 cy=0;cy<height;cy++){
		for(u8 cx=0;cx<width;cx++){		
			RestoreTile(x+cx,y+cy);
		}
	}
}


void fillFieldLine(u8 p, s8 y,u8 tile){

	for(u8 h=0;h<10;h++){	
		fields[p].surface[y][h]=tile;
	}
}


void doGameOver(u8 p){
	u8 p2=(p^1);

	fields[p].gameOver=1;
	StopSong();
	TriggerFx(21,0xff,1);
	WaitVsyncAndProcessAnimations(25);
	StartSong(song_ending);

	//clear field with stars animation
	for(u8 t=0;t<FIELD_HEIGHT+5;t++){
		for(u8 y=0;y<=t;y++){
			u8 tile=22+y; //oscillate between adjacent sprites
			if(tile>=(22+7)) tile=29;

			if(t-y+2 < FIELD_HEIGHT){		
				if(tile!=29){
					u8 temp;
					if((t-y+2)<8){
						temp=tile+8;				
					}else{
						temp=tile;
					}

					Fill(fields[p].left,fields[p].top+t-y+2,FIELD_WIDTH,1,temp);
					if(vsMode){
						Fill(fields[p2].left,fields[p2].top+t-y+2,FIELD_WIDTH,1,temp);
					}	
				}else{
					restore(fields[p].left,fields[p].top+t-y+2,FIELD_WIDTH,1);
					if(vsMode){
						restore(fields[p2].left,fields[p2].top+t-y+2,FIELD_WIDTH,1);
					}						
				}					
			}
		}
			
		WaitVsyncAndProcessAnimations(1);
	}

	for(u8 y=0;y<10;y++){
		if(!vsMode){
			if(y>0)restore(fields[p].left,fields[p].top+y-1+2,FIELD_WIDTH,1);
			Print(fields[p].left,fields[p].top+y+2,strGameOver);
		}else{
			if(y>0)restore(fields[p].left,fields[p].top+y-1+2,FIELD_WIDTH,1);
			Print(fields[p].left,fields[p].top+y+2,strYouLose);
			
			if(y>0)restore(fields[p2].left,fields[p2].top+y-1+2,FIELD_WIDTH,1);
			Print(fields[p2].left+1,fields[p2].top+y+2,strYouWin);

		}
		WaitVsyncAndProcessAnimations(1);
	}
/*
	if(!vsMode){
		Print(fields[p].left,fields[p].top+11,strGameOver);
	}else{
		Print(fields[p].left,fields[p].top+11,strYouLose);
		Fill(fields[p2].left,fields[p2].top+10,FIELD_WIDTH,3,26);
		Print(fields[p2].left+1,fields[p2].top+11,strYouWin);

	}
*/
	u8 a=1;
	u8 y=1;
	u8 anim=22;
	while(1){
		if(++anim>29)
			anim=22;

		for(u8 i=0;i<2;i++){
			if(i == 1 && !vsMode)
				break;
			Fill(fields[i].left,fields[i].top+8,FIELD_WIDTH,FIELD_HEIGHT-8,0);
			Print(fields[i].left,18,PSTR("LINES"));
			PrintByte(fields[i].left+6,19,fields[i].lines,1);
			Print(fields[i].left,21,PSTR("LEVEL"));
			PrintByte(fields[i].left+6,22,fields[i].level,1);
			Print(fields[i].left,24,PSTR("SCORE"));
			PrintInt(fields[i].left+9,25,(fields[i].score)&0xFFFF,1);
			PrintInt(fields[i].left+4,25,((fields[i].score)>>16)&0xFFFF,1);
			fields[i].lastButtons=fields[i].currButtons;
			fields[i].currButtons=ReadJoypad(i);

			if((fields[i].currButtons&(BTN_START) && !(fields[i].lastButtons&(BTN_START)))){	
				fields[0].lastButtons |= BTN_START;
				fields[1].lastButtons |= BTN_START;
				return;
			}
		}	

		if(vsMode){
			//Fill(fields[p2].left,fields[p2].top+10,FIELD_WIDTH,3,22+a);
			Print(fields[p].left,fields[p].top+11,strYouLose);
Print(0,0,PSTR("1>GG!"));
Print(0,3,PSTR("2>HAAAXX!!"));
		Fill(fields[p2].left,fields[p2].top+10,FIELD_WIDTH,3,anim);
			Print(fields[p2].left+1,fields[p2].top+11,strYouWin);
			a+=y;
			if(a==6){				
				y=-1;
			}else if(a==0){
				y=1;
			}
		}else
			Print(fields[p].left,fields[p].top+11,strGameOver);
		WaitVsyncAndProcessAnimations(1);
	}
}


void WaitVsyncAndProcessAnimations(u8 count){

	for(u8 i=0;i<count;i++){
		WaitVsync(1);

		processAnimations(0);
		processAnimations(1);
	}
}


void drawTetramino(s8 x,s8 y,s8 tetramino,s8 rotation,s8 forceTile,u8 restore,u8 clipToField){
	u8 s=pgm_read_byte(&(tetraminos[tetramino].size));
	for(u8 cy=0;cy<s;cy++){
		for(u8 cx=0;cx<s;cx++){
			u8 tile=pgm_read_byte(&(tetraminos[tetramino].blocks[rotation][(cy*s)+cx]));
			if(tile!=0){
								
				if(!clipToField || (clipToField && (cy+y)>=(fields[0].top+2)) ){

					if(restore){			
						RestoreTile(cx+x,cy+y);
					}else{
						if(forceTile!=0)tile=forceTile;
						SetTile(cx+x,cy+y,tile);
					}
				}
			}
		}
	}
}


u8 processGarbage(u8 p){
	if(fields[p].garbageQueue>0){
		s8 rs,rd;

		rs=1;
		rd=0;

		for(u8 y=0;y<(FIELD_HEIGHT-1);y++){ //move field up
			for(u8 x=0;x<10;x++){

				//update the field array
				fields[p].surface[rd][x]=fields[p].surface[rs][x];
				
				//draw only the visible rows
				if(rd+fields[p].top>=(fields[p].top+2)){
					if(fields[p].surface[rs][x]==0){
						RestoreTile(x+fields[p].left,rd+fields[p].top);
					}else{	
						SetTile(x+fields[p].left,rd+fields[p].top,fields[p].surface[rs][x]);
					}
				}
			}
			rs++;
			rd++;
		}		

		rd=FIELD_HEIGHT-1;
		u8 hole=(GetPrngNumber(0) % 200);
		if(hole < GARBAGE_ALIGNMENT_PROBABILITY) //draw aligned to the last hole
			hole = fields[p].lastHole;
		else{
			hole=(GetPrngNumber(0) % 9);
			fields[p].lastHole = hole; //store this for next time
		}
		
		for(u8 y=0;y<1;y++){ //draw garbage lines at bottom of field
			for(u8 x=0;x<10;x++){
				if(x==hole){
					RestoreTile(x+fields[p].left,fields[p].bottom-y);
					fields[p].surface[rd][x]=0;
				}else{
					SetTile(x+fields[p].left,fields[p].bottom-y,8);
					fields[p].surface[rd][x]=8;
				}
			}
			rd--;
		}

		fields[p].garbageQueue--;

		if(fields[p].garbageQueue==0){
			return 1;
		}


	}else{	
		PrintHexByte(16,25,fields[0].garbageQueue);
		PrintHexByte(24,25,fields[1].garbageQueue);
		return 1;
	}
	
	fields[p].subState++;
	return 0;
}


#if CPU_PLAYER > 0

#endif
