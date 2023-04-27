/*
 *  Uzebox Default Patches
 *  Copyright (C) 2008  Alec Bourque
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

/*
	Patches are made of a command stream made of 3 bytes per command:
		1=delta time
		2=command
		3=command parameter

	Patches must start with 1 byte describing the sound type:
		0=wave channel (i.e.; channel 0,1 and 2)
		1=noise channel (channel 3)
		2=PCM (channel 3)
		->For type=2 two more byte follows in order: sample adress low byte, sample adress hi byte

	It must end with <0,PATCH_END> and this command takes
	only two bytes (no parameter).
*/


//INST: Synth Piano
const char patch00[] PROGMEM ={ 
0,PC_WAVE,6,
0,PC_ENV_SPEED,-5,
0,PATCH_END
};

//FX: "Echo Droplet" 'rotate'
const char patch01[] PROGMEM ={ 
0,PC_ENV_SPEED,-12,
5,PC_NOTE_UP,12, 
5,PC_NOTE_DOWN,12,
5,PC_NOTE_UP,12, 
5,PC_NOTE_DOWN,12,
5,PC_NOTE_CUT,0,
0,PATCH_END
};


//empty 
const char patch02[] PROGMEM ={
0,PATCH_END
};


//Square Piano
const char patch03[] PROGMEM ={	
0,PC_WAVE,9-1,
0,PC_ENV_SPEED,-15,  
4,PC_ENV_SPEED,-7,  
0,PATCH_END
};


//FX: drop floor noise
const char patch04[] PROGMEM ={	
0,PC_NOISE_PARAMS,10,
0,PC_ENV_SPEED,-30, 
2,PC_NOISE_PARAMS,0x5,
1,PC_NOISE_PARAMS,0x3,
1,PC_NOISE_PARAMS,0x1, 
2,PC_NOTE_CUT,0,		 
0,PATCH_END
};

//FX: 'move'
const char patch05[] PROGMEM ={	
0,PC_ENV_SPEED,-25,
2,PC_NOTE_UP,12,  
4,PC_NOTE_CUT,0,								 
0,PATCH_END
};


//FX: 'drop'
const char patch06[] PROGMEM ={	
0,PC_ENV_SPEED,-15,
0,PC_PITCH,90,
2,PC_NOTE_DOWN,3,
2,PC_NOTE_DOWN,4,
2,PC_NOTE_DOWN,5,
2,PC_NOTE_DOWN,6,
2,PC_NOTE_CUT,0,
0,PATCH_END
};


//INST: hi-hat
const char patch07[] PROGMEM ={	
0,PC_NOISE_PARAMS,1,
0,PC_ENV_VOL,0x90,
0,PC_ENV_SPEED,-30, 
8,PC_NOTE_CUT,0,
0,PATCH_END
};


//INST: organ
const char patch08[] PROGMEM ={	
0,PC_WAVE,8-1,
1,PC_ENV_VOL,200, 
1,PC_NOTE_HOLD,0,
1,PC_ENV_SPEED,-25,
0,PATCH_END
};


//INST: Crash cymbal
const char patch09[] PROGMEM ={	
0,PC_NOISE_PARAMS,0,
0,PC_ENV_VOL,0xa0,
0,PC_ENV_SPEED,-20, 
1,PC_ENV_SPEED,-2,
0,PC_NOISE_PARAMS,1,
0,PATCH_END
};

//FX: drop floor noise
const char patch10[] PROGMEM ={	
0,PC_NOISE_PARAMS,3,
0,PC_ENV_SPEED,-30, 
3,PC_NOISE_PARAMS,0x6,
4,PC_NOISE_PARAMS,0x9,
3,PC_NOTE_CUT,0,
0,PATCH_END
};


//INST: SawTooth
const char patch11[] PROGMEM ={	
0,PC_WAVE,1,
1,PC_ENV_VOL,200,
1,PC_NOTE_HOLD,0,
1,PC_ENV_SPEED,-20, 
0,PATCH_END
};


//Square Piano 2
const char patch12[] PROGMEM ={	
0,PC_WAVE,4,
0,PC_ENV_SPEED,-15,  
4,PC_ENV_SPEED,-7,  
0,PATCH_END
};


//SawTooth no hold
const char patch13[] PROGMEM ={	
0,PC_WAVE,1,
0,PC_ENV_SPEED,-15, 
0,PATCH_END
};

//Snare
const char patch14[] PROGMEM ={
0,PC_ENV_SPEED,-19,
0,PC_NOISE_PARAMS,6,
2,PC_NOISE_PARAMS,0x1,
0,PATCH_END 
};

//Clap
const char patch15[] PROGMEM ={
0,PC_ENV_SPEED,-20,
0,PC_NOISE_PARAMS,2,
1,PC_ENV_VOL,0,
2,PC_ENV_VOL,0xff,
0,PC_NOISE_PARAMS,3, 
0,PATCH_END  
};

//flute
const char patch16[] PROGMEM ={
0,PC_WAVE,8-1,
0,PC_ENV_VOL,100, 
1,PC_ENV_SPEED,20,
5,PC_ENV_SPEED,-9, 
0,PATCH_END
};

//Pizzicato
const char patch17[] PROGMEM ={	
0,PC_WAVE,9-1,
0,PC_ENV_SPEED,-15, 
0,PATCH_END
};

//LINE CLEAR
const char patch18[] PROGMEM ={	
0,PC_WAVE,4,
0,PC_ENV_SPEED,-8,
0,PC_PITCH,80,
3,PC_PITCH,78,
3,PC_PITCH,84,
2,PC_PITCH,76,
2,PC_PITCH,88,
2,PC_PITCH,74,
1,PC_PITCH,92,
0,PC_ENV_SPEED,-4,
3,PC_PITCH,96,
3,PC_PITCH,90,
3,PC_PITCH,96,
3,PC_PITCH,90,
3,PC_PITCH,96,
3,PC_PITCH,90,
1,PC_NOTE_CUT,0,
0,PATCH_END 
};

//BONUS/ TSPIN?
const char patch19[] PROGMEM ={	
0,PC_WAVE,4,
0,PC_ENV_SPEED,-5,

0,PC_PITCH,62,
1,PC_NOTE_UP,2, 
1,PC_NOTE_UP,2, 
1,PC_NOTE_UP,2, 
1,PC_NOTE_UP,2, 
1,PC_NOTE_UP,2, 


1,PC_NOTE_DOWN,10,
1,PC_NOTE_UP,2, 
1,PC_NOTE_UP,2, 
1,PC_NOTE_UP,2, 
1,PC_NOTE_UP,2, 
1,PC_NOTE_UP,2, 


1,PC_NOTE_DOWN,2,
1,PC_NOTE_UP,2, 
1,PC_NOTE_UP,2, 
1,PC_NOTE_UP,2, 
1,PC_NOTE_UP,2, 
1,PC_NOTE_UP,2, 


1,PC_NOTE_DOWN,10,
1,PC_NOTE_UP,2, 
1,PC_NOTE_UP,2, 
1,PC_NOTE_UP,2, 
1,PC_NOTE_UP,2, 
1,PC_NOTE_UP,2, 

1,PC_NOTE_DOWN,5,
1,PC_NOTE_UP,2, 
1,PC_NOTE_UP,2, 
1,PC_NOTE_UP,2, 
1,PC_NOTE_UP,2, 
1,PC_NOTE_UP,2, 

1,PC_NOTE_DOWN,5,
1,PC_NOTE_UP,2, 
1,PC_NOTE_UP,2, 
1,PC_NOTE_UP,2, 
1,PC_NOTE_UP,2, 
1,PC_NOTE_UP,2, 

1,PC_NOTE_DOWN,5,
1,PC_NOTE_UP,2, 
1,PC_NOTE_UP,2, 
1,PC_NOTE_UP,2, 
1,PC_NOTE_UP,2, 
1,PC_NOTE_UP,2, 
1,PC_NOTE_CUT,0,

0,PATCH_END 
};

//TETRIS
const char patch20[] PROGMEM ={	
0,PC_WAVE,3,
0,PC_PITCH,86,
2,PC_NOTE_DOWN,10, 
2,PC_NOTE_DOWN,8, 
2,PC_NOTE_DOWN,6, 
2,PC_NOTE_DOWN,4, 
2,PC_NOTE_DOWN,2, 
 
0,PC_PITCH,66, 
2,PC_NOTE_UP,5, 
2,PC_NOTE_DOWN,2, 
2,PC_NOTE_UP,5, 

2,PC_NOTE_UP,5, 
2,PC_NOTE_DOWN,2, 
2,PC_NOTE_UP,5, 

0,PC_ENV_SPEED,-8,  

4,PC_NOTE_UP,5, 
4,PC_NOTE_DOWN,5, 
4,PC_NOTE_UP,5, 
4,PC_NOTE_DOWN,5, 
4,PC_NOTE_UP,5,
4,PC_NOTE_DOWN,5, 
4,PC_NOTE_UP,5,
1,PC_NOTE_CUT,0,
0,PATCH_END 

};

//dead
const char patch21[] PROGMEM ={	
0,PC_WAVE,4,
0,PC_ENV_SPEED,-5,

0,PC_PITCH,52,
1,PC_NOTE_UP,2, 
1,PC_NOTE_UP,4, 
1,PC_NOTE_UP,6, 

1,PC_NOTE_DOWN,10,
1,PC_NOTE_UP,2, 
1,PC_NOTE_UP,4, 
1,PC_NOTE_UP,6, 


1,PC_NOTE_DOWN,10,
1,PC_NOTE_UP,2, 
1,PC_NOTE_UP,4, 
1,PC_NOTE_UP,6, 

1,PC_NOTE_DOWN,5,
1,PC_NOTE_UP,2, 
1,PC_NOTE_UP,4, 
1,PC_NOTE_UP,6, 
 
2,PC_NOTE_UP,2, 
2,PC_NOTE_UP,4, 
2,PC_NOTE_UP,6, 

3,PC_NOTE_DOWN,5,
3,PC_NOTE_UP,2, 
3,PC_NOTE_UP,4, 
3,PC_NOTE_UP,6, 
 
4,PC_NOTE_DOWN,5,
4,PC_NOTE_UP,2, 
4,PC_NOTE_UP,4, 
4,PC_NOTE_UP,6, 
1,PC_NOTE_CUT,0,
0,PATCH_END 
};

//back-to-back l1
const char patch22[] PROGMEM ={	
0,PC_ENV_VOL,0,

4,PC_WAVE,4,
0,PC_ENV_VOL,0xff,
0,PC_TREMOLO_LEVEL,0x90,     
0,PC_TREMOLO_RATE,60, 
0,PC_ENV_SPEED,-5,
1,PC_PITCH,80,
3,PC_NOTE_UP,5, 
3,PC_NOTE_DOWN,5, 
3,PC_NOTE_UP,5, 
3,PC_NOTE_DOWN,5, 
3,PC_NOTE_UP,5, 
3,PC_NOTE_DOWN,5, 
3,PC_NOTE_UP,5, 
3,PC_NOTE_DOWN,5, 
3,PC_NOTE_UP,5, 
3,PC_NOTE_DOWN,5, 
3,PC_NOTE_UP,5, 
3,PC_NOTE_DOWN,5, 
3,PC_NOTE_UP,5, 
3,PC_NOTE_CUT,0,
0,PATCH_END 
};

//back-to-back l2
const char patch23[] PROGMEM ={	
0,PC_ENV_VOL,0,

4,PC_WAVE,4,
0,PC_ENV_VOL,0xff,
0,PC_TREMOLO_LEVEL,0x90,     
0,PC_TREMOLO_RATE,60, 
0,PC_ENV_SPEED,-5,
1,PC_PITCH,83,
3,PC_NOTE_UP,5, 
3,PC_NOTE_DOWN,5, 
3,PC_NOTE_UP,5, 
3,PC_NOTE_DOWN,5, 
3,PC_NOTE_UP,5, 
3,PC_NOTE_DOWN,5, 
3,PC_NOTE_UP,5, 
3,PC_NOTE_DOWN,5, 
3,PC_NOTE_UP,5, 
3,PC_NOTE_DOWN,5, 
3,PC_NOTE_UP,5, 
3,PC_NOTE_DOWN,5, 
3,PC_NOTE_UP,5, 
3,PC_NOTE_CUT,0,
0,PATCH_END 
};

//back-to-back l2
const char patch24[] PROGMEM ={	
0,PC_ENV_VOL,0,

4,PC_WAVE,4,
0,PC_ENV_VOL,0xff,
0,PC_TREMOLO_LEVEL,0x90,     
0,PC_TREMOLO_RATE,60, 
0,PC_ENV_SPEED,-5,
1,PC_PITCH,86,
3,PC_NOTE_UP,5, 
3,PC_NOTE_DOWN,5, 
3,PC_NOTE_UP,5, 
3,PC_NOTE_DOWN,5, 
3,PC_NOTE_UP,5, 
3,PC_NOTE_DOWN,5, 
3,PC_NOTE_UP,5, 
3,PC_NOTE_DOWN,5, 
3,PC_NOTE_UP,5, 
3,PC_NOTE_DOWN,5, 
3,PC_NOTE_UP,5, 
3,PC_NOTE_DOWN,5, 
3,PC_NOTE_UP,5, 
3,PC_NOTE_CUT,0,
0,PATCH_END 
};

//back-to-back l3
const char patch25[] PROGMEM ={	
0,PC_ENV_VOL,0,

4,PC_WAVE,4,
0,PC_ENV_VOL,0xff,
0,PC_TREMOLO_LEVEL,0x90,     
0,PC_TREMOLO_RATE,60, 
0,PC_ENV_SPEED,-5,
1,PC_PITCH,89,
3,PC_NOTE_UP,5, 
3,PC_NOTE_DOWN,5, 
3,PC_NOTE_UP,5, 
3,PC_NOTE_DOWN,5, 
3,PC_NOTE_UP,5, 
3,PC_NOTE_DOWN,5, 
3,PC_NOTE_UP,5, 
3,PC_NOTE_DOWN,5, 
3,PC_NOTE_UP,5, 
3,PC_NOTE_DOWN,5, 
3,PC_NOTE_UP,5, 
3,PC_NOTE_DOWN,5, 
3,PC_NOTE_UP,5, 
3,PC_NOTE_CUT,0,
0,PATCH_END 
};


//INST: triangular - no env
const char patch26[] PROGMEM ={	

0,PC_WAVE,2,
1,PC_NOTE_HOLD,0,
1,PC_ENV_SPEED,-32,
0,PATCH_END
};

//INST: SawTooth -  no env
const char patch27[] PROGMEM ={	

0,PC_WAVE,1,
1,PC_NOTE_HOLD,0,
1,PC_ENV_SPEED,-32,
0,PATCH_END
};

//INST: organ -  no env
const char patch28[] PROGMEM ={	

0,PC_WAVE,8-1,
1,PC_NOTE_HOLD,0,
1,PC_ENV_SPEED,-32,
0,PATCH_END};


//INST: square -  no env
const char patch29[] PROGMEM ={	

0,PC_WAVE,5,
1,PC_NOTE_HOLD,0,
1,PC_ENV_SPEED,-32,
0,PATCH_END
};

//INST: Filtered square -  no env
const char patch30[] PROGMEM ={	

0,PC_WAVE,9-1,
1,PC_NOTE_HOLD,0,
1,PC_ENV_SPEED,-32,
0,PATCH_END
};

//Logo "kling" sound
const char patch31[] PROGMEM ={	
0,PC_WAVE,INTRO_WAVETABLE,
0,PC_PITCH,85,
4,PC_PITCH,90,
0,PC_ENV_SPEED,-8,   
0,PC_TREMOLO_LEVEL,0x90,     
0,PC_TREMOLO_RATE,30, 
50,PC_NOTE_CUT,0,
0,PATCH_END  
};

const struct PatchStruct patches[] PROGMEM = {
{0,0,patch00,0,0},
{0,0,patch01,0,0},
{0,0,patch02,0,0},
{0,0,patch03,0,0},
{1,0,patch04,0,0},
{0,0,patch05,0,0},
{0,0,patch06,0,0},
{1,0,patch07,0,0}, 
{0,0,patch08,0,0},
{1,0,patch09,0,0},
{1,0,patch10,0,0},
{0,0,patch11,0,0},
{0,0,patch12,0,0},
{0,0,patch13,0,0},
{1,0,patch14,0,0},
{1,0,patch15,0,0},
{0,0,patch16,0,0},
{0,0,patch17,0,0},
{0,0,patch18,0,0},
{0,0,patch19,0,0},
{0,0,patch20,0,0},
{0,0,patch21,0,0},
{0,0,patch22,0,0},
{0,0,patch23,0,0},
{0,0,patch24,0,0},
{0,0,patch25,0,0},
{0,0,patch26,0,0},
{0,0,patch27,0,0},
{0,0,patch28,0,0},
{0,0,patch29,0,0},
{0,0,patch30,0,0},
{0,0,patch31,0,0}
};
