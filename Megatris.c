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


int main(){
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

	for(u8 i=0;i<2;i++){
		fields[i].maxAutoRepeatDelay = DEF_AUTO_REPEAT_DELAY;//players can customize this in game for Uzenet
		fields[i].maxInterRepeatDelay = DEF_INTER_REPEAT_DELAY;
		fields[i].startLevel = DEF_START_LEVEL;
	}

GAME_TOP:
	if(MainMenu()){
		do{
			do{
				LoadMap();
				initFields();	

				SetTile(14,25,8);
				SetTile(15,25,20);
				PrintHexByte(16,25,0);

				SetTile(22,25,8);
				SetTile(23,25,20);
				PrintHexByte(24,25,0);

				if(uzenet_step < UN_STEP_PLAYING) //if uzenet, we already started the song to alert the players of connection
					StartSongNo(songNo);

				fields[0].currentState=0;
			 	fields[1].currentState=0;
				restart=0;

				while(!fields[0].gameOver && !fields[1].gameOver){	
					WaitVsync(1); //syncronize gameplay on vsync (60 hz)
					if(uzenet_error)
						break;

					f=0;
					runStateMachine();
					processAnimations(0);

					if(vsMode){
						f=1;
						runStateMachine();
						processAnimations(1);
					}
				}
			}while(restart);
		}while(doGameContinue());
	}
	goto GAME_TOP;
}


void StartSongNo(u8 songNo){

	switch(songNo){
		case 0: StartSong(song_tetrisnt); break;				
		case 1: StartSong(song_korobeiniki); break;	
	}
}


void OptionsMenu(u8 p){
if(!Uzenet_Sync(0)){return;}

	u8 c,dx,option=0;
	dx = fields[p].left+1;
	fill(fields[p].left,fields[p].top+2,FIELD_WIDTH,FIELD_HEIGHT-2,0);
	Print(dx,14,PSTR("RESUME"));
	Print(dx,15,PSTR("RESTART"));
	Print(dx,16,PSTR("QUIT"));
	Print(dx,18,PSTR("MUSIC:"));
	Print(dx,19,PSTR("GHOST:"));

	goto FIRST_DRAW; //avoid taking too long before field is overwritten...

	while(1){
		u8 menuSfx = 0;
		WaitVsync(1);
		c=ReadJoypad(p);
		
		if(c&(BTN_START|BTN_A|BTN_B)){
			
			while(ReadJoypad(p)!=0); //wait for key release
			
			if(option==0){
				if(IsSongPlaying()){
					StopSong();
				}else{
					StartSongNo(songNo);
				}
			}else if(option==4){
				StopSong();
				break;
			}
		}

		if(c&BTN_LEFT && option<5)
			menuSfx=1;

		if(c&BTN_LEFT){
			if(option==0 && songNo>0){
				songNo--;
			}else if(option==1){
				fields[p].noGhostBlock=!fields[p].noGhostBlock;
			}else if(option==2 && fields[p].startLevel>0){
				fields[p].startLevel--;
			}

			while(ReadJoypad(p)!=0); //wait for key release	
		}
		if(c&BTN_RIGHT){
			if(option==0 && songNo<3){
				songNo++;
			}else if(option==1){
				fields[0].noGhostBlock=!fields[0].noGhostBlock;
			}else if(option==2 && fields[p].startLevel<30){
				fields[p].startLevel++;
			}

			while(ReadJoypad(p)!=0); //wait for key release	
		}

		if(c&BTN_UP || c&BTN_DOWN || c&BTN_SELECT){
			menuSfx=1;

			if(c&BTN_UP){
				if(option==0){
					option=5;
				}else{
					option--;
				}

			}else if(c&BTN_DOWN || c&BTN_SELECT){
				if(option==5){
					option=0;
				}else{
					option++;
				}
			}

			while(ReadJoypad(p)!=0); //wait for key release
		}

		if(menuSfx){
			menuSfx = 0;
			TriggerFx(1,0x90,1);
		}
FIRST_DRAW:
		PrintHexByte(dx+6,18,songNo);
		Print(dx+6,19,(fields[p].noGhostBlock) ? PSTR("OFF"):PSTR("ON "));
		PrintByte(dx+7,20,fields[p].startLevel,1);
		Print(dx,20,PSTR("LEVEL:"));
		PrintByte(dx+7,21,fields[p].maxAutoRepeatDelay,1);
		Print(dx,21,PSTR("AUTOR:"));
		PrintByte(dx+7,22,fields[p].maxInterRepeatDelay,1);
		Print(dx,22,PSTR("INTER:"));

		for(u8 i=14;i<14+8;i++) //erase old cursor
			SetTile(dx-1,i,0);
		SetTile(dx-1,14+option,CURSOR_TILE);	
	}
	restore(fields[p].left,fields[p].top+2,FIELD_WIDTH,FIELD_HEIGHT-2);
	for(u8 cy=2;cy<FIELD_HEIGHT;cy++){
		for(u8 cx=0;cx<FIELD_WIDTH;cx++){
			if(fields[p].surface[cy][cx]!=0){
				SetTile(fields[p].left+cx,fields[p].top+cy,fields[p].surface[cy][cx]);
			}
		}				
	}
}


int UzenetConnect(){

	ClearVram();

	u8 error = 0;
	u8 connected = 0;
	u16 timer = 0;
	while(1){

		WaitVsync(1);
		DrawMap(3,3,map_title);

		if(connected || error)//waiting or error is indefinite
			timer = 0;
		else
			timer++;
//if(timer == 30)uzenet_step=UN_CONNECT_SERVER;
		if(!error){
			if(!connected){
				Print(9,16,PSTR("CONNECTING TO UZENET...."));
			}else{
				Print(4,16,PSTR("WAITING FOR OPPONENT...."));
				Print(4,19,PSTR("PRESS A FOR BOT MATCH"));
			}
		}else{//some error happened
			Print(4,16,PSTR("NETWORK ERROR OR MODULE FAILURE"));
			Print(4,18,PSTR("RUN SETUP TOOL IF THIS PERSISTS"));
		}
		Print(error?4:11,20,PSTR("PRESS B TO CANCEL       "));

		u16 c = ReadJoypad(0);
		if(c & BTN_B){
			while(ReadJoypad(0) != 0); //wait for key release
			TriggerFx(5,0xff,1);
			FadeOut(1,1);
			FadeIn(1,0);
			return 0;//go back to main menu
		}

		if(c & BTN_A)
			UartSendChar('C');//since we are in simple 2P matchmaking mode, this is all we need to do and the server will make a bot join

		if(error)
			continue;

		for(uint8_t j=29;j<35;j++){//animate "progress bar"

			if(((timer&31)/8) > j-29)
				continue;
			SetTile(j,16,0);
		}

		if(uzenet_error || timer == ((60*4)-1)){ //something timed out, indicating an unrecoverable error
			error = 1;
			TriggerFx(21,0xff,1);
		}

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

		if(!got1){//still need player 1 name?
			s16 c = UartReadChar();
			if(c != -1){
				c &= 0xff;
				
			}
		}
		if(fields[0].surface[0] && !requested2){//already drew player 1 name, but not player 2 name?
			requested2 = 1;//don't request again
			UartSendChar(UN_GET_PLAYER_NAME2);//get name of second player from the server...
		}

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


u8 MainMenu(){ //will never get here during an Uzenet game

	u8 option = 0;
	u16 lastButtons,currButtons=0;

	while(1){
		WaitVsync(1);
		GetPrngNumber(0);
		ClearVram();
		DrawMap(3,3,map_title);
		Print(15,15,PSTR("1 PLAYER"));
		Print(15,16,PSTR("2 PLAYER"));
		Print(15,17,PSTR("UZENET"));

	
		PrintChar(10,22,92);
		Print(9,22,PSTR("2008-2023 ALEC BOURQUE"));
		Print(7,23,PSTR("LICENSED UNDER GNU GPL V3"));
		Print(9,25,PSTR("HTTPS://WWW.UZEBOX.ORG"));

		lastButtons=currButtons;
		currButtons=ReadJoypad(0);

		if((currButtons&(BTN_A|BTN_START)) && !(lastButtons&(BTN_A|BTN_START))){
			
			if(option==0){
				vsMode=0;
				break;
			}else if(option==1){
				vsMode=1;
				break;
			}else{
				if(!(uzenet_error & UN_ERROR_CRITICAL)){
					if(UzenetConnect()){//if we connect, we wont return to the main menu until we disconnect
						vsMode=1;
						break;
					}
					return 0;
				}else
					TriggerFx(6,0xff,1);
			}
		}

		if((currButtons&(BTN_UP|BTN_DOWN|BTN_SELECT)) && !(lastButtons&(BTN_UP|BTN_DOWN|BTN_SELECT))){

			TriggerFx(1,0x90,1);

			if((currButtons&BTN_UP)){
				if(--option>2) //rollover
					option=2;

			}else{ //BTN_DOWN|BTN_SELECT
				if(++option>2)
					option=0;
			}
		}
		SetTile(14,15+option,CURSOR_TILE); //draw cursor
	}
	FadeOut(1,1);
	FadeIn(2,0);
	return 1;
}


u8 doGameContinue(void){
	u8 option=0,cx=fields[0].left+2,cy=15;
	u16 c;

	restore(fields[0].left,fields[0].top+2,FIELD_WIDTH,FIELD_HEIGHT-2);
	restore(fields[1].left,fields[1].top+2,FIELD_WIDTH,FIELD_HEIGHT-2);

	Print(cx+1,cy,PSTR("PLAY"));
	Print(cx+1,cy+1,PSTR("MENU"));
	SetTile(cx,cy+option,CURSOR_TILE); //draw new cursor

	while(1){
		c=ReadJoypad(0);
	
		if(c&(BTN_START|BTN_A|BTN_B)){
		
			while(ReadJoypad(0)!=0); //wait for key release
		
			if(option==0){
				return 1;
			}else{
				return 0;
			}		
		}

		if(c&BTN_UP || c&BTN_DOWN || c&BTN_SELECT){
			RestoreTile(cx,cy+option); //erase old cursor
			TriggerFx(1,0x90,1);

			if(c&BTN_UP){
				if(option==0){
					option=1;
				}else{
					option--;
				}

			}else if(c&BTN_DOWN || c&BTN_SELECT){
				if(option==1){
					option=0;
				}else{
					option++;
				}
			}
		
			SetTile(cx,cy+option,CURSOR_TILE); //draw new cursor
			while(ReadJoypad(0)!=0); //wait for key release
		}
	}
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


void runStateMachine(){
	switch(fields[f].currentState){
		case 0: //moving
			if(processGravity()==1){
				TriggerFx(4,0x90,1);
				fields[f].subState=0;
				fields[f].currentState=1;
				break;				
			}

			if(processControls()==1){
				fields[f].currentState=6;
			}
			break;
		case 1: //piece on ground
			if(lockBlock()==1){
				fields[f].subState=0;
				fields[f].currentState=2;
			}
			break;
		case 2: //piece locked
			if(animFields()==1){
				fields[f].subState=0;
				fields[f].currentState=3;
			}
			break;			
		case 3: //done locking
			if(updateFields()==1){
				fields[f].subState=0;
				fields[f].currentState=4;
			}
			break;
		case 4: //processing garbage

if(!Uzenet_Sync(0)){return;}

			if(processGarbage()==1){
				fields[f].subState=0;
				fields[f].currentState=5;			
			}
			break;	
		case 5: //issuing new block
			issueNewBlock();
			updateGhostPiece(0);
			fields[f].currentState=0;
			break;			
		case 6: //dropping block
			hardDrop();
			TriggerFx(4,0x90,1);
			fields[f].subState=0;
			fields[f].currentState=1;
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
		fields[x].lastButtons=0;
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
		fields[x].holdBlockPosY=2;
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
	fields[0].holdBlockPosX=2;

	fields[1].left=28;
	fields[1].right=39;
	fields[1].nextBlockPosX=22;
	fields[1].holdBlockPosX=34;

	for(u8 y=0;y<FIELD_HEIGHT;y++){
		for(u8 x=0;x<FIELD_WIDTH;x++){
			fields[0].surface[y][x]=0;
			fields[1].surface[y][x]=0;
		}
	}

	f=0;
	issueNewBlock();
	fields[f].currBlock=fields[f].nextBlock;
	issueNewBlock(); //issue twice to have a "next" block randomly defined
	updateFields();
	updateGhostPiece(0);

	if(vsMode){
		f=1;
		issueNewBlock();
		fields[f].currBlock=fields[f].nextBlock;
		issueNewBlock();
		updateFields();
		updateGhostPiece(0);
	}
}


u8 processGravity(void){

	if(fields[f].locking){ //check lock delay
		fields[f].currLockDelay++;
		if(fields[f].currLockDelay>LOCK_DELAY){
			fields[f].locking=0;
			fields[f].currLockDelay=0;
			fields[f].currGravity=0;
			return 1;
		}
	}else{

		fields[f].currGravity++;
		if(fields[f].currGravity>=fields[f].gravity){
			fields[f].currGravity=0;
			//attemp to move block down
			if(!moveBlock(fields[f].currBlockX,fields[f].currBlockY+1)){
				fields[f].locking=1;
			}
		}
	}
	return 0;
}


u8 lockBlock(void){

	switch(fields[f].subState){

		case 0:
			drawTetramino(fields[f].currBlockX+fields[f].left,fields[f].currBlockY+fields[f].top,fields[f].currBlock,fields[f].currBlockRotation,10,0,1);
			break;
		case 1:
			drawTetramino(fields[f].currBlockX+fields[f].left,fields[f].currBlockY+fields[f].top,fields[f].currBlock,fields[f].currBlockRotation,12+fields[f].currBlock,0,1);
			break;

		default:
			drawTetramino(fields[f].currBlockX+fields[f].left,fields[f].currBlockY+fields[f].top,fields[f].currBlock,fields[f].currBlockRotation,0,0,1);
			//draw block on surface
			s8 s=pgm_read_byte(&(tetraminos[fields[f].currBlock].size));

			for(s8 cy=0;cy<s;cy++){
				for(s8 cx=0;cx<s;cx++){
					s8 tile=pgm_read_byte(&(tetraminos[fields[f].currBlock].blocks[fields[f].currBlockRotation][(cy*s)+cx]));
					if(tile!=0){
						fields[f].surface[fields[f].currBlockY+cy][fields[f].currBlockX+cx]=fields[f].currBlock+1;
					}
				}
			}

			fields[f].currLockDelay=0;
			fields[f].locking=0;
			fields[f].hardDroppped=0;

			//detect potential t-spin
			if(fields[f].currBlock==T_TETRAMINO && fields[f].lastOpIsRotation==1){
				//check if at least 3 corners are occupied
				u8 occupied=0;

				if(fields[f].surface[(u8)fields[f].currBlockY][(u8)fields[f].currBlockX]!=0)occupied++;
				if(fields[f].surface[(u8)fields[f].currBlockY][(u8)fields[f].currBlockX+2]!=0)occupied++;
				if(fields[f].surface[(u8)fields[f].currBlockY+2][(u8)fields[f].currBlockX]!=0)occupied++;
				if(fields[f].surface[(u8)fields[f].currBlockY+2][(u8)fields[f].currBlockX+2]!=0)occupied++;
				if(occupied>=3){
					fields[f].tSpin=1;
				}else{
					fields[f].tSpin=0;
				}
			}
			return 1;
	}

	fields[f].subState++;
	return 0;
}


u8 moveBlock(s8 x,s8 y){
	u8 updateGhost=(fields[f].currBlockX!=x);

	//check if block can fit in the location
	if(!fitCheck(x,y,fields[f].currBlock,fields[f].currBlockRotation)) return 0;

	//erase previous block
	if(updateGhost) updateGhostPiece(1);
	drawTetramino(fields[f].currBlockX+fields[f].left,fields[f].currBlockY+fields[f].top,fields[f].currBlock,fields[f].currBlockRotation,0,1,1);	

	fields[f].currBlockX=x;
	fields[f].currBlockY=y;

	//draw new one
	if(updateGhost) updateGhostPiece(0);
	drawTetramino(fields[f].currBlockX+fields[f].left,fields[f].currBlockY+fields[f].top,fields[f].currBlock,fields[f].currBlockRotation,0,0,1);

	fields[f].lastOpIsRotation=0;

	return 1;
}


s8 randomize(void){

	return (s8)(GetPrngNumber(0) % 7);

}


void issueNewBlock(){

	s8 b1,b2,b3,b4,b5,b6,b7;
	s16 next=0;
	
	if(fields[f].bagPos==7){ //time to fill a new bag 
if(!Uzenet_Sync(0)){return;}				
		b1=randomize();
		do{b2=randomize();}while (b2==b1);
		do{b3=randomize();}while (b3==b2);
		do{b4=randomize();}while (b4==b3);
		do{b5=randomize();}while (b5==b4);
		do{b6=randomize();}while (b6==b5);
		do{b7=randomize();}while (b7==b6);

		fields[f].bag[0]=b1;
		fields[f].bag[1]=b2;
		fields[f].bag[2]=b3;
		fields[f].bag[3]=b4;
		fields[f].bag[4]=b5;
		fields[f].bag[5]=b6;
		fields[f].bag[6]=b7;

		next=b1;
		fields[f].bagPos=1;
	}else{
		next=fields[f].bag[fields[f].bagPos];
		fields[f].bagPos++;
	}
	
	//update the next block
	drawTetramino(fields[f].nextBlockPosX,fields[f].nextBlockPosY,fields[f].nextBlock,0,0,1,0);
	drawTetramino(fields[f].nextBlockPosX,fields[f].nextBlockPosY,next,0,0,0,0);

	fields[f].currBlock=fields[f].nextBlock;
	fields[f].nextBlock=next;
	fields[f].currBlockX=3;
	fields[f].currBlockY=0;
	fields[f].currBlockRotation=0;
	fields[f].kickUp=0;
	fields[f].canHold=1;
	
	//check if game over
	if(!moveBlock(fields[f].currBlockX,fields[f].currBlockY))
		doGameOver();
}


void updateGhostPiece(u8 restore){ //this is an expensive function(up to 14613 cycles on empty well?)

	if(uzenet_step >= UN_STEP_PLAYING && (f != uzenet_local_player)) //save some cycles, don't draw for what we can't control
		return;

	if(!fields[f].noGhostBlock){

		s16 y=fields[f].currBlockY;
		while(1){
			if(!fitCheck(fields[f].currBlockX,y,fields[f].currBlock,fields[f].currBlockRotation)) break;
			y++;
		}

		drawTetramino(fields[f].currBlockX+fields[f].left,y+fields[f].top-1,fields[f].currBlock,fields[f].currBlockRotation,GHOST_TILE,restore,1);
	}
}


void hardDrop(void){

	s16 y=fields[f].currBlockY;
	while(1){
		if(!fitCheck(fields[f].currBlockX,y,fields[f].currBlock,fields[f].currBlockRotation)) break;
		y++;
	}
	
	moveBlock(fields[f].currBlockX,y-1);
	
	fields[f].locking=0;
	fields[f].currLockDelay=0;
	fields[f].currGravity=0;
	fields[f].hardDroppped=1;

}


void hold(void){

	if(fields[f].canHold){
		//erase current block from field
		updateGhostPiece(1);
		drawTetramino(fields[f].currBlockX+fields[f].left,fields[f].currBlockY+fields[f].top,fields[f].currBlock,fields[f].currBlockRotation,0,1,1);

		if(fields[f].holdBlock==NO_BLOCK){

			fields[f].holdBlock=fields[f].currBlock;

			//update the hold block
			drawTetramino(fields[f].holdBlockPosX,fields[f].holdBlockPosY,fields[f].holdBlock,0,0,0,0);

			issueNewBlock();
			updateGhostPiece(0);	
		}else{
			//swap the hold block and current block
			drawTetramino(fields[f].holdBlockPosX,fields[f].holdBlockPosY,fields[f].holdBlock,0,0,1,0);
			drawTetramino(fields[f].holdBlockPosX,fields[f].holdBlockPosY,fields[f].currBlock,0,0,0,0);


			s16 temp=fields[f].currBlock;
			fields[f].currBlock=fields[f].holdBlock;
			fields[f].holdBlock=temp;

			fields[f].currBlockX=3;
			fields[f].currBlockY=0;
			fields[f].currBlockRotation=0;
			fields[f].kickUp=0;

			fields[f].currLockDelay=0;
			fields[f].locking=0;

			//check if game over
			if(!fitCheck(fields[f].currBlockX,fields[f].currBlockY,fields[f].currBlock,fields[f].currBlockRotation))
				doGameOver();
			
			updateGhostPiece(0);
		}

		fields[f].canHold=0;
	}
}


u8 processControls(void){
	s16 dispX=0,dispY=0;
	u16 joy=ReadJoypad(f);

	//process hard drop
	if((joy&BTN_UP) && !(fields[f].lastButtons&BTN_UP) && !(joy&BTN_RIGHT) && !(joy&BTN_LEFT)){
		//hardDrop();
		fields[f].lastButtons=joy;
		return 1;
	}

	//process hold
	if(((joy&BTN_SELECT) && !(fields[f].lastButtons&BTN_SELECT)) || ((joy&BTN_SR) && !(fields[f].lastButtons&BTN_SR)) ){
		hold();
		fields[f].lastButtons=joy;
		return 0;
	}

	//process pause
	if((joy&BTN_START) && !(fields[f].lastButtons&BTN_START)){
		OptionsMenu(f);
		return 0;
	}

	//process left-right-down
	if(joy&BTN_RIGHT){
		if(!(fields[f].lastButtons&BTN_RIGHT) || (fields[f].autoRepeatDelay>fields[f].maxAutoRepeatDelay && fields[f].interRepeatDelay>fields[f].maxInterRepeatDelay)){
			dispX=1;
		}
	}

	if(joy&BTN_LEFT){
		if(!(fields[f].lastButtons&BTN_LEFT) || (fields[f].autoRepeatDelay>fields[f].maxAutoRepeatDelay && fields[f].interRepeatDelay>fields[f].maxInterRepeatDelay)){
			dispX=-1;
		}
	}

	if(joy&BTN_DOWN){
		if(fields[f].softDropDelay>SOFT_DROP_DELAY){
			dispY=1;
			fields[f].softDropDelay=0;
		}
	}

	if((dispX!=0 || dispY!=0) && moveBlock(fields[f].currBlockX+dispX,fields[f].currBlockY+dispY)){
		fields[f].currLockDelay=0;
		fields[f].locking=0;
		fields[f].interRepeatDelay=0;
		TriggerFx(5,0x40,1);
	}

	if(joy&(BTN_LEFT|BTN_RIGHT)){
		fields[f].autoRepeatDelay++;
		fields[f].interRepeatDelay++;
	}else{
		fields[f].autoRepeatDelay=0;
		fields[f].interRepeatDelay=0;
	}

	if(joy&BTN_DOWN){
		fields[f].softDropDelay++;
	}else{
		fields[f].softDropDelay=0;
	}

	//process rotation
	s16 rot=0;
	if(joy&BTN_A && !(fields[f].lastButtons&BTN_A)){
		//rotate right
		rot=fields[f].currBlockRotation+1;
		if(rot==4) rot=0;
		TriggerFx(5,0x40,1);
		rotateBlock(rot);
	}
	if(joy&BTN_B && !(fields[f].lastButtons&BTN_B)){
		//rotate right
		if(fields[f].currBlockRotation==0){
			rot=3;
		}else{
			rot=fields[f].currBlockRotation-1;
		}
		TriggerFx(5,0x40,1);
		rotateBlock(rot);
	}

	fields[f].lastButtons=joy;
	return 0;
}


u8 fitCheck(s8 x,s8 y,s8 block,s8 rotation){
	//check if block overlaps existing blocks

	s16 s=pgm_read_byte(&(tetraminos[block].size));

	for(s8 cy=0;cy<s;cy++){
		for(s8 cx=0;cx<s;cx++){
			s8 tile=pgm_read_byte(&(tetraminos[block].blocks[rotation][(cy*s)+cx]));
			if(tile!=0){
				//check field boundaries
				if((y+cy)>FIELD_HEIGHT-1 || (x+cx)<0 || (x+cx)>FIELD_WIDTH-1) return 0;

				//check for collisions with surface blocks
				if(fields[f].surface[y+cy][x+cx]!=0) return 0;

			}
		}
	}
	return 1;
}


u8 rotateBlock(s8 newRotation){
	s8 x,y;
	u8 rotateRight=0;

	if(fields[f].currBlockRotation==3 && newRotation==0){
		rotateRight=1;
	}else if(fields[f].currBlockRotation==0 && newRotation==3){
		rotateRight=0;
	}else if(newRotation > fields[f].currBlockRotation){
		rotateRight=1;
	}

	x=fields[f].currBlockX;
	y=fields[f].currBlockY;

	if(fields[f].currBlock==O_TETRAMINO) return 1; //can't rotate the square tetramino


	if(!fitCheck(x,y,fields[f].currBlock,newRotation)){
		//try simple wall kicks
		if(fitCheck(x+1,y,fields[f].currBlock,newRotation)){
			x++;
		}else{
			if(fitCheck(x-1,y,fields[f].currBlock,newRotation)){
				x--;
			}else{
				if(fitCheck(x,y-1,fields[f].currBlock,fields[f].currBlockRotation) && fitCheck(x,y-1,fields[f].currBlock,newRotation)){
					y--;
					fields[f].kickUp=1;
				}else{
					//special srs moves
					if(rotateRight==1 && fitCheck(x-1,y+1,fields[f].currBlock,newRotation)){
						x--;
						y++;
					}else if(rotateRight==1 && fitCheck(x-1,y+2,fields[f].currBlock,newRotation)){
						x--;
						y+=2;
					}else if(rotateRight==0 && fitCheck(x+1,y+1,fields[f].currBlock,newRotation)){
						x++;
						y++;
					}else if(rotateRight==0 && fitCheck(x+1,y+2,fields[f].currBlock,newRotation)){
						x++;
						y+=2;
					}else{
						//test 2 blocks for I tetramino
						if(fitCheck(x+2,y,fields[f].currBlock,newRotation)){
							x+=2;
						}else{
							if(fitCheck(x-2,y,fields[f].currBlock,newRotation)){
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
	if(fields[f].kickUp){
		if(fitCheck(x,y+1,fields[f].currBlock,newRotation)){
			y++;
		}
	}

	updateGhostPiece(1); //erase previous block
	drawTetramino(fields[f].currBlockX+fields[f].left,fields[f].currBlockY+fields[f].top,fields[f].currBlock,fields[f].currBlockRotation,0,1,1);

	fields[f].currBlockRotation=newRotation;
	fields[f].currBlockX=x;
	fields[f].currBlockY=y;

	updateGhostPiece(0); //draw new one
	drawTetramino(fields[f].currBlockX+fields[f].left,fields[f].currBlockY+fields[f].top,fields[f].currBlock,fields[f].currBlockRotation,0,0,1);

	fields[f].lastOpIsRotation=1;
	fields[f].currLockDelay=0;
	fields[f].locking=0;

	return 1;
}


u8 lineCleared(s8 y){

	u8 cleared=1;
	for(u8 x=0;x<10;x++){
		if(fields[f].surface[y][x]!=0){
			cleared=0;
			break;
		}		
	}
	return cleared;
}


u8 lineFull(s8 y){

	u8 full=1;
	for(u8 x=0;x<10;x++){
		if(fields[f].surface[y][x]==0){
			full=0;
			break;
		}		
	}
	return full;
}


u8 animFields(void){ //animate the cleared lines
	u8 y,size,tile,temp;

	size=pgm_read_byte(&(tetraminos[fields[f].currBlock].size));

	if(size+fields[f].currBlockY>FIELD_HEIGHT){
		size=FIELD_HEIGHT-fields[f].currBlockY;
	}
	
	switch(fields[f].subState){
		
		case 0 ... 1:
			fields[f].lastClearCount=0;
			for(y=0;y<size;y++){
				if(lineFull(fields[f].currBlockY+y)){				
					fill(fields[f].left,y+fields[f].currBlockY+fields[f].top,FIELD_WIDTH,1,10);			
					fields[f].lastClearCount++;
				}		
			}

			if(fields[f].tSpin==1){
				fields[f].tSpinAnimFrame=90;
				TriggerFx(19,0xff,1);
			}else if(fields[f].lastClearCount==4){
				fields[f].tetrisAnimFrame=90;
				TriggerFx(20,0xff,1);
			}else if(fields[f].lastClearCount==0){
				return 1;				
			}else{
				//at least one line completed
				TriggerFx(18,0xc0,1);
			}
			break;

		case 2 ... 23:
			if(fields[f].lastClearCount<4){
				switch(fields[f].subState){
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
				fields[f].subState++;
			}else{
				tile=22+(fields[f].subState/2);
				
			}
			

			for(y=0;y<size;y++){
				if(lineFull(fields[f].currBlockY+y)){		

					if((y+fields[f].currBlockY)<8){
						temp=tile+8;				
					}else{
						temp=tile;
					}

					if(temp>=37) temp=0;

					fill(fields[f].left,y+fields[f].currBlockY+fields[f].top,FIELD_WIDTH,1,temp);						
				}		
			}
			break;

		default:
			for(y=0;y<size;y++){		
				if(lineFull(fields[f].currBlockY+y)){
					//we have a completed row!
					fillFieldLine(y+fields[f].currBlockY,0);
				}		
			}
			return 1;  //last animation frame
	}

	fields[f].subState++;
	return 0;
}


u8 updateFields(void){

	//scan the surface for completed rows
	s16 y,size,tail,clearCount,top,garbageLines=0,fx;	
	u8 difficult=0;

	size=pgm_read_byte(&(tetraminos[fields[f].currBlock].size));

	if(size+fields[f].currBlockY>FIELD_HEIGHT){
		size=FIELD_HEIGHT-fields[f].currBlockY;
	}

	clearCount=fields[f].lastClearCount;
	if(clearCount>0){

		if(fields[f].subState==0){

				tail=size-1;

				for(top=0;top<size;top++){
	
					if(lineCleared(fields[f].currBlockY+tail)){			
						copyFieldLines(fields[f].currBlockY,fields[f].currBlockY+1,tail);
					}else{
						tail--;
					}

				}


		}else if(fields[f].subState<=clearCount){
				//move rest of field down
				copyFieldLines(fields[f].subState-1,fields[f].subState,fields[f].currBlockY);

		}else{	
			//clear top lines
			for(y=0;y<clearCount;y++){
				fillFieldLine(y,0);
			}
				
			TriggerFx(10,0xa0,1);

			//Process garbage lines 
			if(clearCount>0){

				if(fields[f].tSpin==1){
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

				if(garbageLines==4 || fields[f].tSpin==1){
					difficult=1;
				}

				if(difficult && fields[f].backToBack!=0){
					garbageLines++;
					fx=22+fields[f].backToBack-1;
					if(fx>25)fx=25;

					TriggerFx(fx,0xff,1);
					fields[f].backToBackAnimFrame=90;
				}

				//send garbage lines to the other field(in 2 players mode)
				if(vsMode){
					if(fields[f].garbageQueue){
						if(fields[f].garbageQueue >= garbageLines){
							fields[f].garbageQueue -= garbageLines;
							garbageLines = 0;
						}else{
							garbageLines -= fields[f].garbageQueue;
							fields[f].garbageQueue = 0;
						}	
					}
						fields[1].garbageQueue+=garbageLines;
				}
				PrintHexByte(16,25,fields[0].garbageQueue);
				PrintHexByte(24,25,fields[1].garbageQueue);
			}

			//update score, lines, etc
			if(clearCount!=0){
				fields[f].lines+=clearCount;
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

				if(fields[f].backToBack!=0)
					bonus=(bonus*3)/2;

				if(difficult){				
					fields[f].backToBack++;
				}else{
					fields[f].backToBack=0;						
				}

				fields[f].score+=(fields[f].level*fields[f].height)*bonus;

				if(fields[f].score>999999)
					fields[f].score=999999;
			}		

			if(fields[f].lines > fields[f].nextLevel){
				//increase speed
				if(fields[f].gravity>0){
					fields[f].gravity--;
				}
				fields[f].level+=1;
				fields[f].nextLevel+=LINES_PER_LEVEL;
			}
			fields[f].tSpin=0;
			return 1;
		}
	}else{
		fields[f].tSpin=0;
		return 1;
	}

	//print scores, lines,etc
//	u8 x = f?27:19;

//	PrintLong(x,18,fields[f].lines);
//	PrintLong(x,21,fields[f].level);
//	PrintLong(x,24,fields[f].score);	

	fields[f].subState++;
	return 0;
}


void copyFieldLines(s8 rowSource,s8 rowDest,s8 len){

	if(len>0){
		s8 rs=rowSource+len-1;
		s8 rd=rowDest+len-1;

		for(s8 y=0;y<len;y++){
			for(s8 x=0;x<10;x++){
				fields[f].surface[rd][x]=fields[f].surface[rs][x];
				if(rd+fields[f].top>=(fields[f].top+2)){
					if(fields[f].surface[rs][x]==0){
						RestoreTile(x+fields[f].left,rd+fields[f].top);
					}else{	
						SetTile(x+fields[f].left,rd+fields[f].top,fields[f].surface[rs][x]);
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


void fillFieldLine(s8 y,u8 tile){

	for(u8 h=0;h<10;h++){	
		fields[f].surface[y][h]=tile;
	}
}


void doGameOver(void){
	u8 f2=(f^1);

	fields[f].gameOver=1;
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

					Fill(fields[f].left,fields[f].top+t-y+2,FIELD_WIDTH,1,temp);
					if(vsMode){
						Fill(fields[f2].left,fields[f2].top+t-y+2,FIELD_WIDTH,1,temp);
					}	
				}else{
					restore(fields[f].left,fields[f].top+t-y+2,FIELD_WIDTH,1);
					if(vsMode){
						restore(fields[f2].left,fields[f2].top+t-y+2,FIELD_WIDTH,1);
					}						
				}					
			}
		}
			
		WaitVsyncAndProcessAnimations(1);
	}

	for(u8 y=0;y<10;y++){
		if(!vsMode){
			if(y>0)restore(fields[f].left,fields[f].top+y-1+2,FIELD_WIDTH,1);
			Print(fields[f].left,fields[f].top+y+2,strGameOver);
		}else{
			if(y>0)restore(fields[f].left,fields[f].top+y-1+2,FIELD_WIDTH,1);
			Print(fields[f].left,fields[f].top+y+2,strYouLose);
			
			if(y>0)restore(fields[f2].left,fields[f2].top+y-1+2,FIELD_WIDTH,1);
			Print(fields[f2].left+1,fields[f2].top+y+2,strYouWin);

		}
		WaitVsyncAndProcessAnimations(1);
	}

	if(!vsMode){
		Print(fields[f].left,fields[f].top+11,strGameOver);
	}else{
		Print(fields[f].left,fields[f].top+11,strYouLose);
		Fill(fields[f2].left,fields[f2].top+10,FIELD_WIDTH,3,26);
		Print(fields[f2].left+1,fields[f2].top+11,strYouWin);

	}

	u8 a=1;
	u8 y=1;
	while(1){
		
		WaitVsyncAndProcessAnimations(2);

		if(ReadJoypad(0)&(BTN_A|BTN_B|BTN_START)){
			
			while(ReadJoypad(0)!=0); //wait for key release			
			return;			
		}	

		if(vsMode){
			Fill(fields[f2].left,fields[f2].top+10,FIELD_WIDTH,3,22+a);			
			Print(fields[f2].left+1,fields[f2].top+11,strYouWin);
			a+=y;
			if(a==6){				
				y=-1;
			}else if(a==0){
				y=1;
			}
		}
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
								
				if(!clipToField || (clipToField && (cy+y)>=(fields[f].top+2)) ){

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


u8 processGarbage(){
	if(fields[f].garbageQueue>0){
		s8 rs,rd;

		rs=1;
		rd=0;

		for(u8 y=0;y<(FIELD_HEIGHT-1);y++){ //move field up
			for(u8 x=0;x<10;x++){

				//update the field array
				fields[f].surface[rd][x]=fields[f].surface[rs][x];
				
				//draw only the visible rows
				if(rd+fields[f].top>=(fields[f].top+2)){
					if(fields[f].surface[rs][x]==0){
						RestoreTile(x+fields[f].left,rd+fields[f].top);
					}else{	
						SetTile(x+fields[f].left,rd+fields[f].top,fields[f].surface[rs][x]);
					}
				}
			}
			rs++;
			rd++;
		}		

		rd=FIELD_HEIGHT-1;
		u8 hole=(GetPrngNumber(0) % 200);
		if(hole < GARBAGE_ALIGNMENT_PROBABILITY) //draw aligned to the last hole
			hole = fields[f].lastHole;
		else{
			hole=(GetPrngNumber(0) % 9);
			fields[f].lastHole = hole; //store this for next time
		}
		
		for(u8 y=0;y<1;y++){ //draw garbage lines at bottom of field
			for(u8 x=0;x<10;x++){
				if(x==hole){
					RestoreTile(x+fields[f].left,fields[f].bottom-y);
					fields[f].surface[rd][x]=0;
				}else{
					SetTile(x+fields[f].left,fields[f].bottom-y,8);
					fields[f].surface[rd][x]=8;
				}
			}
			rd--;
		}

		fields[f].garbageQueue--;

		if(fields[f].garbageQueue==0){
			return 1;
		}


	}else{	
		PrintHexByte(16,25,fields[0].garbageQueue);
		PrintHexByte(24,25,fields[1].garbageQueue);
		return 1;
	}
	
	fields[f].subState++;
	return 0;
}
