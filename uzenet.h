#ifndef UZENET_H
#define UZENET_H
#include "spiram.h"

extern u8 UartUnreadCount();
extern s16 UartReadChar();
extern s8 UartSendChar(u8 data);		
extern bool IsUartTxBufferEmpty();
extern bool IsUartTxBufferFull();
extern void InitUartTxBuffer();
extern void InitUartRxBuffer();
extern unsigned char ReadEeprom(unsigned int addr);

extern void Uzenet_SyncLFSR();//all games using an LFSR must define this
//extern const char UN_ROM_IDENTIFIER[];

//optional user supplied custom implementations
#ifdef UN_CUSTOM_RSVP_HANDLER
extern void Uzenet_CustomRsvpHandler();
#endif
#ifdef UN_CUSTOM_MATCH_HANDLER
extern void Uzenet_CustomMatchHandler();
#endif
#ifdef UN_FONT_SPECIFICATION
extern const char UN_FONT_SPECIFICATION_TABLE[];
#endif

//#define UN_HARDCODED_SERVER	1
#ifdef UN_HARDCODED_SERVER
const char UN_HARDCODED_SERVER_NAME[] PROGMEM = "10.0.99.158";
#endif

const char UN_ROM_IDENTIFIER[] PROGMEM = "MGATRS00";//must be 8 characters, the first 6 are registers to the original game implementor(or overriden by admins), the last 2 are for versioning
#define UN_FONT_SPECIFICATION	1
const char UN_FONT_SPECIFICATION_TABLE[] PROGMEM = " !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]???ABCDEFGHIJKLMNOPQRSTUVWXYZ?????";

//(186=9600=16/frame),(124=14400=24/frame),(92=19200=32/frame),(61=28800=48/frame),(46=38400=64/frame),(30=57600=96/frame),(22=76800=128/frame),(15=115200=192/frame),
#define UN_BAUD_RATE	9600//the default state is to boot in 9600, but you can use a higher rate after the intial setup(higher speeds required larger UART_RX_BUFFER_SIZE values)
//http://wormfood.net/avrbaudcalc.php?bitrate=&clock=28.63636&databits=8
//https://lucidar.me/en/serialib/most-used-baud-rates-table/
//the below divisors all assume double speed mode bit is set
#if (UN_BAUD_RATE == 600)//1 byte/frame, 2982 divisor, 0,0% error, **16.67ms** serialization delay
	#define UN_RX_MIN_BUF	1
	#define UN_UBRR0H	1
	#define UN_UBRR0L	
#elif (UN_BAUD_RATE == 1200)//2 bytes/frame, 1490 divisor, 0.0% error, **8.33ms** serialization delay 
	#define UN_RX_MIN_BUF	2
	#define UN_UBRR0L	
#elif (UN_BAUD_RATE == 2400)//4 bytes/frame, 1490 0.0% error, **4.17ms** serialization delay
	#define UN_RX_MIN_BUF	4
	#define UN_UBRR0H	1
	#define UN_UBRR0L	
#elif (UN_BAUD_RATE == 4800)//8 bytes/frame, 745 0.0% error, **2.08ms** serialization delay
	#define UN_RX_MIN_BUF	8
	#define UN_UBRR0H	1
	#define UN_UBRR0L	
#elif (UN_BAUD_RATE == 9600)//16 bytes/frame, 185 0.0% error, 1.04ms serialization delay
	#define UN_RX_MIN_BUF	16

#elif (UN_BAUD_RATE == 14400)//24 bytes/frame, 248 0.2% error,
	#define UN_RX_MIN_BUF	24+8//UART power of 2 implementation requires 32
	#define UN_UBRR0H 	1
	#define UN_UBRR0L	
#elif (UN_BAUD_RATE == 19200)//32 bytes/frame, 185 0.2% error, 0.52ms serialization delay
	#define UN_RX_MIN_BUF	32
	#define UN_UBRR0H	0
	#define UN_UBRR0L	
#elif (UN_BAUD_RATE == 28800)//48 bytes/frame, 123 0.2% error, 0.35ms serialization delay
	#define UN_RX_MIN_BUF	48+16//UART power of 2 implementation requires 64
	#define UN_UBRR0H	0
	#define UN_UBRR0L	
#elif (UN_BAUD_RATE == 38400)//64 bytes/frame, 92 0.2% error, 0.26ms serialization delay
	#define UN_RX_MIN_BUF	64
	#define UN_UBRR0H	0
	#define UN_UBRR0L	
#elif (UN_BAUD_RATE == 57600)//96 bytes/frame, 61 0.2% error, 0.17ms serialization delay
	#define UN_RX_MIN_BUF	96+32//UART power of 2 implementation requires 128
	#define UN_UBRR0H	0
	#define UN_UBRR0L	
#elif (UN_BAUD_RATE == 76800)//128 bytes/frame, 46 0.8% error, 0.13ms serialization delay
	#define UN_RX_MIN_BUF	128
	#define UN_UBRR0H	0
	#define UN_UBRR0L	
#elif (UN_BAUD_RATE == 115200)//192 bytes/frame, 30 0.2% error, 0.09ms serialization delay
	#define UN_RX_MIN_BUF	192+64//UART power of 2 implementation requires 256
	#define UN_UBRR0H	0
	#define UN_UBRR0L	15
#elif (UN_BAUD_RATE == 230400)//**384 bytes/frame, 7 **2.9%** error, 0.04ms serialization delay
	#define UN_RX_MIN_BUF	328-128//**limited by UART implementation to 256 ***WARNING, possible to overflow mid-frame(use with consideration)
	#define UN_NO_DOUBLE	1//less error issues?
	#define UN_UBRR0H	0
	#define UN_UBRR0L	7
#elif (UN_BAUD_RATE == 250000)//**417/+1 bytes/frame, 6 **2.3%** error, <0.04ms serialization delay
	#define UN_RX_MIN_BUF	417-161//**limited by UART implementation to 256 ***WARNING, possible to overflow mid-frame(use with consideration)
	#define UN_NO_DOUBLE	1//less error issues?
	#define UN_UBRR0H	0
	#define UN_UBRR0L	6
#endif

#define UN_MATCH_IMMEDIATE_READY 1

const char UN_STR_PING[] PROGMEM = "PING";
const char UN_STR_PONG[] PROGMEM = "PONG";
const char UN_STR_ERROR[] PROGMEM = "ERROR";


#define ESP_RESET PD3
#define ESP_ENABLE PA6

#define UN_ESP8266_DETECTED	1//recieved at least 1 byte(could have been garbage/bad baud)
#define UN_ESP8266_VERIFIED	2//received something intelligble, we know the ESP8266 can talk to us
#define UN_SPI_RAM_DETECTED	4
#define UN_PASS_DETECTED	8//something is in EEPROM at least, not verified to be a real password
#define UN_HOST_DETECTED	16//something is in EEPROM at least, not verified to be a real password
#define UN_IP_DETECTED		32
#ifdef UN_SKIP_EEPROM_CHECK
	#define UN_PREREQUISITES	(UN_ESP8266_DETECTED|UN_ESP8266_VERIFIED|UN_SPI_RAM_DETECTED|UN_HOST_DETECTED|UN_PASS_DETECTED|UN_IP_DETECTED)	
#else
	#define UN_PREREQUISITES	(UN_ESP8266_DETECTED|UN_ESP8266_VERIFIED|UN_SPI_RAM_DETECTED|UN_IP_DETECTED)
#endif
#define UN_SERVER_DETECTED	64
#define UN_FIND_OPPONENT	128
#define UN_DETECTED_RSVP	256
#define UN_STATE_GOT_NAMES	512

#define UN_STATE_DO_SYNC		1024
#define UN_STATE_PLAYING	2048
#define UN_STATE_QUIT_GAME	4096
#define UN_STATE_TRANSPARENT	8192
#define UN_STATE_TX_BREAK	16384
#define UN_STATE_	32768


#define UN_ERROR_TRANSIENT		1//a(possibly) temporary error, which might have been caused by temporary network conditions
#define UN_ERROR_CONNECT_FAILED	2//possibly a transient error
#define UN_ERROR_SERVER_TIMEOUT	4//possibly a transient error
#define UN_ERROR_GAME_FAILED		64
#define UN_ERROR_CRITICAL		128//an error that wont be fixed by resetting and trying again

#define UN_MATCH_RSVP_DETECTED 2//a pre-arranged match was detected, bypass the requirement for the user to manuall select UZENET option
#define UN_MATCH_HAVE_NAMES	4//we have all the names of the players in the room?
#define UN_MATCH_WAS_KICKED	8//we got kicked from the room somehow?


#define UN_PASS_EEPROM_OFF	23//8 characters long
#define UN_HOST_EEPROM_OFF	11//up to 8 characters long, used as a suffix to "uze", so a value of "net.us" implies the hostname "uzenet.us"(was the old/unused MAC attribute).
#define UN_SPIR_BASE		512
#define UN_SPIR_BASE_TX	UN_SPIR_BASE+0
#define UN_SPIR_BASE_RX	UN_SPIR_BASE_TX+1024
#define UN_PASS_BASE		UN_SPIR_BASE_RX+1024
#define UN_SPIR_HOST_BASE	UN_PASS_BASE+16
#define UN_PLAYER_INFO_BASE	UN_SPIR_HOST_BASE+64
#define UN_PAD_HISTORY_BASE	UN_PLAYER_INFO_BASE+(13*4)//512 per player(256 pad states each)


#define UN_STEP_RESET		0
#define UN_STEP_START		1
#define UN_STEP_HOST_DETECTED	2
#define UN_STEP_CHECK_BOOT	3
#define UN_STEP_SET_BAUD	4
#define UN_STEP_SET_MUX	5
#define UN_STEP_WAIT_IP	6
#define UN_STEP_CONNECT	7
#define UN_STEP_CHECK_CONNECT	8
#define UN_STEP_SET_SEND	9
#define UN_STEP_SEND_LOGIN	10
#define UN_STEP_PRECONNECT	11
#define UN_STEP_GAME_SPECIFY	12
#define UN_STEP_REQ_RSVP	13
#define UN_STEP_CHECK_RSVP	14
#define UN_STEP_CHECK_JOIN	15
#define UN_STEP_NO_MATCH	16
#define UN_STEP_IN_MATCH	17
#define UN_STEP_CHECK_READY	18
#define UN_STEP_PLAYER_INFO	19
#define UN_STEP_QUIT_GAME	40
//custom override states, to allow user implementations of RSVP and matchmaking handling
#define UN_STEP_CUSTOM_RSVP	101
#define UN_STEP_CUSTOM_MATCH	151


#define UN_STEP_PLAYING	240

//COMMAND:				ASCII//HEX:(DESCRIPTION):
#define UN_CMD_NULL			0//0x00:(NULL, as a command this will cause the server to disconnect assumed as error)
#define UN_CMD_HEADER_START		1//0x01:(start of header)
#define UN_CMD_TEXT_START		2//0x02:(start of text)
#define UN_CMD_TEXT_END		3//0x03:(end of text)
#define UN_CMD_TX_END			4//0x04:(end of transmission)disconnects from the server
#define UN_CMD_ENQUIRY			5//0x05:(enquiry)
#define UN_CMD_ACKNOWLEDGE		6//0x06:(acknowledge)
#define UN_CMD_BELL			7//0x07:(bell)
#define UN_CMD_BACKSPACE		8//0x08:(backspace)
#define UN_CMD_HORIZONTAL_TAB		9//0x09:(horizontal tab)
#define UN_CMD_LINE_FEED		10//0x0A:(line feed)
#define UN_CMD_VERTICAL_TAB		11//0x0B:(vertical tab)
#define UN_CMD_FORM_FEED		12//0x0C:(form feed)
#define UN_CMD_CARRIAGE_RETURN	13//0x0D:(carriage return)
#define UN_CMD_SHIFT_OUT		14//0x0E:(shift out)
#define UN_CMD_SHIFT_IN		15//0x0F:(shift in)
#define UN_CMD_DATA_LINK_ESCAPE	16//0x10:(data link escape)
#define UN_CMD_DEVICE_CONTROL1	17//0x11:(device control 1)
#define UN_CMD_DEVICE_CONTROL2	18//0x12:(device control 2)
#define UN_CMD_DEVICE_CONTROL3	19//0x13:(device control 3)
#define UN_CMD_DEVICE_CONTROL4	20//0x14:(device control 4)
#define UN_CMD_NEGATIVE_ACKNOWLEDGE	21//0x15:(negative acknowledge)
#define UN_CMD_SYNCHRONIZE		22//0x16:(synchronize)
#define UN_CMD_END_OF_TX_BLOCK	23//0x17:(end of transmission block)
#define UN_CMD_CANCEL			24//0x18:(cancel)
#define UN_CMD_END_OF_MEDIUM		25//0x19:(end of medium)
#define UN_CMD_SUBSTITUTE		26//0x1A:(substitute)
#define UN_CMD_ESCAPE			27//0x1B:(escape)
#define UN_CMD_FILE_SEPARATOR		28//0x1C:(file separator)
#define UN_CMD_GROUP_SEPARATOR	29//0x1D:(group separator)
#define UN_CMD_RECORD_SEPARATOR	30//0x1E:(record separator)
#define UN_CMD_UNIT_SEPARATOR		31//0x1F:(unit separator)

#define UN_CMD_ROM_IDENTIFY		32//' '
#define UN_FONT_SPECIFICATION_COMMON	33//'!'
#define UN_CMD_FONT_SPECIFY		34//'"'
#define UN_CMD_CHECK_RSVP		35//'#'
#define UN_CMD_JOIN_MATCH		36//'$'
#define UN_CMD_REQ_MATCH_SIMPLE	37//'%'
#define UN_CMD_CHECK_MATCH_READY	38
#define UN_CMD_SEND_MATCH_READY	39
#define UN_CMD_PLAYER_INFO_SIMPLE	40

#define UN_CMD_DO_SYNC			45

#define UN_CMD_BAD_SYNC		49
#define UN_CMD_QUIT_MATCH		50

#define UN_CMD_CHAT_BYTE		55

#define UN_CMD_PAD_DATA_SIMPLE	60
//TODO NEED TO SUPPORT NEW BAUD FORMAT IN EMULATOR BEFORE DOING THIS
//#define UN_CUSTOM_BAUD_DIVISOR	30
//const char UN_CUSTOM_BAUD_NUM[] PROGMEM = "57600";//the equivalent baud rate to the above divisor, in string form(for the AT command)


//#define UN_MATCH_MAKING_STYLE	1//override the default simple "join/host any game without password" behavior

#define UN_KBD_ROLE_GET_ASCII		0
#define UN_KBD_ROLE_EMULATE_PAD	1


#define UN_MAX_RUN_AHEAD	120
#define UN_MAX_SYNC_WAIT	180

#define MAX_UZENET_PLAYERS	2

// This system lets you use a small Tx buffer, which makes this RAM nearly free...
volatile u16 uzenet_state;
volatile u8 uzenet_sync;
volatile u8 uzenet_error;
volatile u8 uzenet_step,uzenet_wait,uzenet_local_tick;
volatile u8 uzenet_remote_player,uzenet_remote_tick,uzenet_remote_cmd,uzenet_remote_last_rx_tick;//[MAX_UZENET_PLAYERS];
volatile u8 uzenet_local_player;
volatile u16 uzenet_spir_rx_pos, uzenet_spir_tx_pos;
volatile u16 uzenet_spir_rx_len;
volatile u8 uzenet_spir_tx_len;

u8 uzenet_last_rx;
u8 uzenet_crc;//use during critical sections

void Uzenet_SpiRamTxS(char *s){
	SpiRamSeqWriteStart(0, (u16)(uzenet_spir_tx_pos+uzenet_spir_tx_len));
	while(s[0] != 0){
		SpiRamSeqWriteS8(s[0]);
		s++;
		uzenet_spir_tx_len++;
	}
	SpiRamSeqWriteEnd();
}


void Uzenet_SpiRamTxP(const char *s){
	SpiRamSeqWriteStart(0, (u16)(uzenet_spir_tx_pos+uzenet_spir_tx_len));
	char c;
	while((c = pgm_read_byte(s++)) != 0){
		SpiRamSeqWriteS8(c);
		uzenet_spir_tx_len++;
	}
	SpiRamSeqWriteEnd();
}


void Uzenet_SpiRamTxB(u8 d){
	SpiRamWriteU8(0, (u16)(uzenet_spir_tx_pos+uzenet_spir_tx_len), d);
	uzenet_spir_tx_len++;
}
/*
void Uzenet_SpiRamTxR(u8 *d, u8 len){
	SpiRamSeqWriteStart(0, (u16)(uzenet_spir_tx_pos+uzenet_spir_tx_len));
	while(len--)
		SpiRamSeqWriteU8(*d++);
	SpiRamSeqWriteEnd();
}
*/
s16 Uzenet_SpiRamRxB(){//returns the earliest unread byte, control positioning with timing, flushes, and or positioning
	if(!uzenet_spir_rx_len)
		return -1;
	u8 b = SpiRamReadU8(0, (u16)(uzenet_spir_rx_pos++));
	uzenet_spir_rx_len--;
	if(!uzenet_spir_rx_len)
		uzenet_spir_rx_pos = 0;
	return b;
}


void DEBUGRXBUF(){
	u8 x = 3;
	u8 y = 0;
	while(uzenet_spir_rx_len){
		PrintHexByte(x,y++,Uzenet_SpiRamRxB()&127);
		if(y > 24){
			y = 0;
			x += 4;
		}
	}
	Uzenet_SpiRamRxFlush(1);
	Uzenet_SpiRamTxFlush(1);
	uzenet_state |= UN_ERROR_CRITICAL;
	while(1);
}

void Uzenet_SetStep(u8 s){

	if(uzenet_step == UN_STEP_CHECK_RSVP)//in case we have a floating request(we wont be around for), end it so another player isn't waiting...
		Uzenet_SpiRamTxB(UN_CMD_DATA_LINK_ESCAPE);//this basically breaks us out of the match request

	uzenet_step = s;
}


u8 Uzenet_Sync(u8 s){
//	uzenet_sync = s;
	if(uzenet_state < UN_STEP_PLAYING)
		return 1;
	u8 wait_time = 0;
	while((uzenet_remote_tick != uzenet_local_tick) && wait_time < UN_MAX_SYNC_WAIT){//volatile
		WaitVsync(1);
		if(uzenet_error)//volatile
			return 0;
		wait_time++;
	}
	return (wait_time < UN_MAX_SYNC_WAIT);
}


u8 Uzenet_GameError(){
	if(uzenet_step >= UN_STEP_PLAYING)
		return uzenet_error;
	return 0;
}


void Uzenet_FindOpponent(){
	uzenet_state |= UN_FIND_OPPONENT;
}


int Uzenet_SpiRamRxSearchP(const char *s){
	u16 spos = (u16)(uzenet_spir_rx_pos+0);
	u16 epos = (u16)(uzenet_spir_rx_pos+uzenet_spir_rx_len);
	const char *p = s;
	SpiRamSeqReadStart(0, spos);
	while(spos < epos){
		u8 c = SpiRamSeqReadU8();
		if(c == pgm_read_byte(p++)){
			if(pgm_read_byte(p) == 0){//matched the last char?
				SpiRamSeqReadEnd();
				return 1;
			}
		}else
			p = s;//reset string compare
		spos++;
	}
	SpiRamSeqReadEnd();
	return 0;
}


void Uzenet_SpiRamRxFlush(u8 iu){
	uzenet_spir_rx_pos = UN_SPIR_BASE_RX;
	uzenet_spir_rx_len = 0;
//	uzenet_spir_rx_expected = 0;
//	uzenet_match_start = 0;
//	uzenet_match_end = 0;
	if(iu)
		InitUartRxBuffer();
}


void Uzenet_SpiRamTxFlush(u8 iu){//don't call this until you are done transmitting!
	uzenet_spir_tx_pos = UN_SPIR_BASE_TX;
	uzenet_spir_tx_len = 0;
	if(iu)
		InitUartTxBuffer();
}


u16 Uzenet_ReadJoypad(u8 p){
	if(uzenet_step < UN_STEP_PLAYER_INFO)//normal local play
		return ReadJoypad(p);

	if(p == uzenet_local_player){
		//TODO see if we should stall with 0xFFFF
		return ReadJoypad(0);//first joypad, regardless of which player we actually are in game
	}
	//otherwise remote player, get data for their current tick(which might be 0xFFFF)
	u16 off = UN_PAD_HISTORY_BASE+((uzenet_remote_player*2*256)+(uzenet_remote_tick*2));
	u16 pad = SpiRamReadU16(0, off);//if this data wasn't filled by network data, it will be 0xFFFF
}


int total_sent = 0;
void Uzenet_Update(){//designed to work with a *MINIMUM* 16 bytes Rx, and 2 byte Tx at 9600 baud. Faster speeds will need a larger Rx buffer for reliability, Tx can remain low

//PrintInt(8,0,uzenet_step,1);
//PrintInt(8,1,uzenet_spir_rx_pos-UN_SPIR_BASE_RX,1);
//PrintInt(8,2,uzenet_spir_rx_len,1);
//PrintInt(16,0,total_sent,1);
//PrintInt(16,1,uzenet_spir_tx_len,1);
//PrintInt(16,2,uzenet_last_rx,1);

	u8 unread = UartUnreadCount();

	if(unread){//still data left? must have found the match start, store the rest of the data to search through
		SpiRamSeqWriteStart(0, (u16)(uzenet_spir_rx_pos+uzenet_spir_rx_len));
		while(unread--){
uzenet_last_rx = UartReadChar();
SpiRamSeqWriteU8(uzenet_last_rx);			//SpiRamSeqWriteU8(UartReadChar());
			uzenet_spir_rx_len++;
		}
		SpiRamSeqWriteEnd();
	}

	if(uzenet_spir_tx_len){//always send available Tx data from SPI RAM right away(to allow a small Tx buffer)
		SpiRamSeqReadStart(0, uzenet_spir_tx_pos);
		do{
			if(IsUartTxBufferFull())
				break;
			total_sent++;
			UartSendChar(SpiRamSeqReadU8());
			uzenet_spir_tx_pos++;
		}while(--uzenet_spir_tx_len);
		SpiRamSeqReadEnd();
	}

	if(uzenet_step < UN_STEP_PLAYER_INFO)//not commited to a match yet? then process local input so the user can quit the menu
		ReadControllers();

	if(uzenet_error & UN_ERROR_CRITICAL){
		Uzenet_SpiRamRxFlush(1);
		Uzenet_SpiRamTxFlush(1);
		return;
	}

	uzenet_local_tick++;//always increment timer, used for multiple things

	if(uzenet_step >= UN_STEP_PLAYING){//intercepting joypad input to perform networking transparently
		
		while(uzenet_spir_rx_len){//have some bytes to process?
			if(uzenet_remote_cmd == 0){
				uzenet_remote_cmd = Uzenet_SpiRamRxB();
			}

			if(uzenet_remote_cmd == UN_CMD_PAD_DATA_SIMPLE){

				if(uzenet_spir_rx_len < 4)
					break;
				SpiRamSeqReadStart(0, uzenet_spir_rx_pos);
				u8 padnum = SpiRamSeqReadU8();
				u8 ticknum = SpiRamSeqReadU8();
				u8 padhi = SpiRamSeqReadU8();
				u8 padlo = SpiRamSeqReadU8();
				SpiRamSeqReadEnd();
				uzenet_spir_rx_pos += 4;

				if(padnum == uzenet_remote_player && (uzenet_remote_last_rx_tick+1) == ticknum){
					uzenet_remote_last_rx_tick++;
					u16 off = UN_PAD_HISTORY_BASE+(uzenet_remote_player*2*256)+(uzenet_remote_tick*2);
					SpiRamSeqWriteStart(0, off);
					SpiRamSeqWriteU16((u16)((padhi<<8)|padlo));//write new data
					SpiRamSeqWriteU16(0xFFFF);//mark the data as unavailable when the counter wraps, unless we fill it before then
					SpiRamSeqWriteEnd();

				}else{//shouldn't happen, must be out of sync
					Uzenet_SpiRamTxB(UN_CMD_BAD_SYNC);
					Uzenet_SpiRamTxB(UN_CMD_QUIT_MATCH);
					uzenet_error |= UN_ERROR_GAME_FAILED;
					//uzenet_step = UN_STEP_
				}
					//Uzenet_SpiRamRxB();//log remote input(2 bytes)

			}else if(uzenet_remote_cmd == UN_CMD_DO_SYNC){//other side wants us to send up to(and including) a specific tick...

				if(uzenet_spir_rx_len < 1)//don't have the tick number yet....
					break;
	//			u8 sync_tick = SpiRamReadU8();//we assume this is within 180 ticks ago, since otherwise things already broke...

	/*			if(sync_tick < uzenet_local_tick){
					u8 delta = uzenet_local_tick-sync_tick;
					if(delta > 180){//being asked to sync to a point we already went past?
						//TODO send our own sync signal, we can't go back, they shouldn't have went forward either...reconcile
					}
				}
*/
			}else if(uzenet_remote_cmd == UN_CMD_QUIT_MATCH){//other side leaving, we leave too

				Uzenet_SpiRamTxB(UN_CMD_QUIT_MATCH);
				uzenet_error |= UN_ERROR_GAME_FAILED;
				//uzenet_step = UN_STEP_

			}else if(uzenet_remote_cmd == UN_CMD_CHAT_BYTE){

				if(uzenet_spir_rx_len < 1)
					break;
				

			}
		}
//		if(uzenet_state & UN_STATE_DO_SYNC){
//			while(!GetVsyncFlag());
//			return;
//		}
		return;
	}

	if(uzenet_spir_tx_len)//still sending data, don't advance until we could actually output more
		return;

	if(uzenet_wait && --uzenet_wait)
		return;//still waiting for something, try again next tick

	if(uzenet_step == UN_STEP_RESET){//shutdown module, and check for SPI RAM

		DDRD |= (1<<ESP_RESET); PORTD &= ~(1<<ESP_RESET);//shut the module down
		UBRR0H = 0;//TODO FIX THIS, 9600 baud IS WRONG, FIX IN EMULATOR AS WELL!!!!! TODO
		UBRR0L = 185;//set the standard 9600 baud used at ESP8266 boot
		UCSR0A = (1<<U2X0);//[double speed mode]
		UCSR0C = (1<<UCSZ01)+(1<<UCSZ00)+(0<<USBS0); UCSR0B = (1<<RXEN0)+(1<<TXEN0);//[8-bit frame, no parity, 1 stop bit]; [Enable Tx/Rx];

		if(!(uzenet_state & UN_SPI_RAM_DETECTED) && SpiRamInit())//only do once per boot
				uzenet_state |= UN_SPI_RAM_DETECTED;

		uzenet_error = 0;//get rid of any errors, since the reset might fix things...
		Uzenet_SpiRamRxFlush(1); Uzenet_SpiRamTxFlush(1);//eat any existing data(this might be a retry)
		uzenet_step = UN_STEP_START;

	}else if(uzenet_step == UN_STEP_START){//turn module back on, detect server entry, and detect password entry
	
		PORTD |= (1<<ESP_RESET);//start the module(it will be a while before it boots...), verify something is in the host entry
#ifndef UN_SKIP_EEPROM_CHECK
		u8 c = ReadEeprom(UN_HOST_EEPROM_OFF);
		if(c && c != 0xFF)
			uzenet_state |= UN_HOST_DETECTED;
		c = ReadEeprom(UN_PASS_EEPROM_OFF);
		if(c && c != 0xFF)
			uzenet_state |= UN_PASS_DETECTED;
#endif
		uzenet_local_tick = 0;//set a timer for the device to boot against in the next step(we will get a bunch of data in the coming frames for the boot message)
		uzenet_step = UN_STEP_CHECK_BOOT;

	}else if(uzenet_step == UN_STEP_CHECK_BOOT){//wait for "ready\r\n" which indicates the boot is done, and it is at the expected baud rate

		if(Uzenet_SpiRamRxSearchP(PSTR("ready\r\n"))){
			uzenet_state |= UN_ESP8266_DETECTED;
			Uzenet_SpiRamTxP(PSTR("ATE0\r\n"));//turn off echo(if we got this far, this should always work)
			uzenet_step = UN_STEP_SET_BAUD;
		}
		if(uzenet_local_tick > 120){
			uzenet_error |= UN_ERROR_CRITICAL;//at least this is useful, we built a list of what is present and what is not
			uzenet_step = 0;
		}

	}else if(uzenet_step == UN_STEP_SET_BAUD){//change to the baud rate we want to use in game(if we got this far, this should always work)
#ifdef UN_CUSTOM_BAUD_DIVISOR
		Uzenet_SpiRamTxP(PSTR("AT+CIOBAUD_CUR="));//USE NEW FORM: AT+UART_CUR=<baudrate>,<databits>,<stopbits>,<parity>,<flow control>
		uzenet_SpiRamTxP(UN_CUSTOM_BAUD_NUM);//user defined baud string
		Uzenet_SpiRamTxP(PSTR("\r\n"));//Uzenet_SpiRamTxP(PSTR(",8,0,1,0\r\n"));
		uzenet_wait = 1+(24/UART_TX_BUFFER_SIZE);
#endif
		uzenet_step = UN_STEP_SET_MUX;

	}else if(uzenet_step == UN_STEP_SET_MUX){//set single connection mode(so we can use transparent transmission mode)
#ifdef UN_CUSTOM_BAUD_DIVISOR
		Uzenet_SpiRamRxFlush(1);//scrap any garbage response we got at the wrong baud rate
		UBRR0L = UN_CUSTOM_BAUD_DIVISOR;//match the user defined baud divisor(was set on module by last command, we didn't set it there so we could Tx anything queued first...)
#endif
		Uzenet_SpiRamTxP(PSTR("AT+CIPMUX=0\r\n"));//single mode
		uzenet_step = UN_STEP_WAIT_IP;

	}else if(uzenet_step == UN_STEP_WAIT_IP){//wait until we get an IP, this might take a while, but doesn't indicate an error(game could advance into the connect screen and wait there...)

		if(Uzenet_SpiRamRxSearchP(PSTR("WIFI GOT IP"))){
			uzenet_state |= UN_IP_DETECTED;
			uzenet_step = UN_STEP_CONNECT;
			Uzenet_SpiRamRxFlush(1);//dump all previous Rx
		}		

	}else if(uzenet_step == UN_STEP_CONNECT){//make an initial connection, to see if there is a reserved match waiting for us
#ifdef UN_HARDCODED_SERVER
		Uzenet_SpiRamTxP(PSTR("AT+CIPSTART=\"TCP\",\""));
		Uzenet_SpiRamTxP(UN_HARDCODED_SERVER_NAME);
#else //use EEPROM data
		Uzenet_SpiRamTxP(PSTR("AT+CIPSTART=\"TCP\",\"uze"));//due to EEPROM limits, we don't store the entire domain name. We instead assume an "uze" prefix, ie "uzenet.us" represented by "net.us"
		for(u8 i=UN_HOST_EEPROM_OFF;i<UN_HOST_EEPROM_OFF+8;i++){
			u8 c = ReadEeprom(i) & 0b01111111;//the top bits of these are special game flags not part of the hostname

			if(c == 0)
				break;
			Uzenet_SpiRamTxB(c);
		}

#endif
		Uzenet_SpiRamRxFlush(1);//scrap previous reponses, we just want to see if we linked
		Uzenet_SpiRamTxP(PSTR("\",2345,7200\r\n"));//response should be "OK\r\nLINKED\r\n" or "ERROR\r\n"
		uzenet_local_tick = 0;//use this as a timer for response timeout
		uzenet_step = UN_STEP_CHECK_CONNECT;

	}else if(uzenet_step == UN_STEP_CHECK_CONNECT){//wait for the connection response(this is the first time we really error check, for simplicity)

		if(Uzenet_SpiRamRxSearchP(PSTR("OK\r\n"))){
			uzenet_state |= UN_ESP8266_VERIFIED;
			Uzenet_SpiRamTxP(PSTR("AT+CIPMODE=1\r\n"));//set single mode(assume this works...we will find out later)
			uzenet_wait = 1+(14/UART_TX_BUFFER_SIZE);
			uzenet_step = UN_STEP_SET_SEND;
		}else if(uzenet_local_tick > 180 || Uzenet_SpiRamRxSearchP(PSTR("ERROR\r\n"))){//failed?
			uzenet_step = UN_STEP_PRECONNECT;//
			uzenet_error |= UN_ERROR_TRANSIENT;//program can see we met prerequisites but are "awaiting connect", this might work next time it's attempted(temporary Internet issue?)
		}

	}else if(uzenet_step == UN_STEP_SET_SEND){//start transparent transmission(assume this works...we will find out later if we can't talk to the server)

		Uzenet_SpiRamTxP(PSTR("AT+CIPSEND\r\n"));
		uzenet_wait = 1+(12/UART_TX_BUFFER_SIZE);
		uzenet_step = UN_STEP_SEND_LOGIN;

	}else if(uzenet_step == UN_STEP_SEND_LOGIN){//just send it, we will know if it failed when we check for specific output later(if it fails, it will drop us back to AT mode)

		for(u8 i=UN_PASS_EEPROM_OFF;i<UN_PASS_EEPROM_OFF+8;i++)
			Uzenet_SpiRamTxB(ReadEeprom(i));

		uzenet_step = UN_STEP_GAME_SPECIFY;
	}else if(uzenet_step == UN_STEP_GAME_SPECIFY){//send commands to server: register game name, check if we have an RSVP(this could all be combined into 1 string, this is for illustrative purposes)

		//Uzenet_SpiRamTxP(UN_MATCH_SPECIFIER);//replace all the following commands with 1 string...
		Uzenet_SpiRamTxB(UN_CMD_ROM_IDENTIFY);//this command has no response, but is required before checking for RSVP(RSVP needs to be for the same ROM/VERSION)
		Uzenet_SpiRamTxP(UN_ROM_IDENTIFIER);//the argument is the fixed length ROM(6)VERSION(2) to register with the server
#ifndef UN_FONT_SPECIFICATION//assume the common font set consisting of [ !"#$%&`()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\]^_]
		Uzenet_SpiRamTxP(UN_FONT_SPECIFICATION_COMMON);//this command has no response...
#else
		Uzenet_SpiRamTxB(UN_CMD_FONT_SPECIFY);//this command has no response...
		Uzenet_SpiRamTxP(UN_FONT_SPECIFICATION_TABLE);//...we specify a full ASCII translation for the server to use for any received chat data(convert unsupported characters to supported)
#endif
/////		Uzenet_SpiRamTxP(UN_STR_PONG);//this will ensure the server doesn't bother us with "PING" requests for a while, so we can assume the next byte is reponse to a command
#ifndef UN_CUSTOM_RSVP_HANDLER
		uzenet_step = UN_STEP_REQ_RSVP;//the next step wont actually process until all the above is transmitted, due to logic at top of this function
#else
		uzenet_step = UN_STEP_CUSTOM_RSVP;//allow a custom implementation of the entire RSVP check(probably not necessary)
#endif
		Uzenet_SpiRamRxFlush(1);//flush any previous responses, like ">\r\n" from above step, the server wont send anything until we ask for it

	}else if(
#ifndef UN_CUSTOM_RSVP_HANDLER
		uzenet_step == UN_STEP_REQ_RSVP)
#else
		uzenet_step == UN_STEP_CUSTOM_RSVP)
#endif
						{
#ifdef UN_CUSTOM_RSVP_HANDLER
	//	Uzenet_CustomRsvpHandler();//this user function is responsible for handling uzenet_step = UN_STEP_CUSTOM_MATCH
#else//normal RSVP handler is probably sufficient for most every case
		Uzenet_SpiRamTxB(UN_CMD_CHECK_RSVP);//this command checks if we previously arranged a match(in another step, or external program), 0 is none, else a room number 1-255 we can join
		uzenet_step = UN_STEP_CHECK_RSVP;
		uzenet_local_tick = 0;//use this as a timer for server response timeout
		Uzenet_SpiRamRxFlush(1);//discard any previously received data(we move between states fast enough that there should be no "PING" to worry about)
//Uzenet_SpiRamTxB(UN_CMD_CHECK_RSVP);
		//uzenet_wait = 1;//wait for a possible error response to the above send, incase the previous command is an error(indicating the connection dropped)

	}else if(uzenet_step == UN_STEP_CHECK_RSVP){//see if we did have an RSVP(this also verifies communication with the server, and the RSVP is for this game ROM)
//STEP 14
		//we previously sent "PONG" so the server doesn't "PING" us for a while; any byte we received here should be a response the previous RSVP request, or possibly a ESP8266 disconnect error
//		if(Uzenet_SpiRamRxSearchP(UN_STR_ERROR)){//something broke, there is no recovery except to reconnect and try again...
//			uzenet_error |= UN_ERROR_CONNECT_FAILED;//user code should drop out of menu, and the user reselecting UZENET will cause the connection to be re-attempted
//			uzenet_step = UN_STEP_PRECONNECT;
//			return;
//		}
		s16 c = Uzenet_SpiRamRxB();//in transparent mode any byte we receive is either an out of band error, or the response from the server(so we should see response to UN_CMD_CHECK_RSVP here) 
		if(c != -1){//done waiting on a response?
			uzenet_state |= UN_SERVER_DETECTED;//then our login credentials are also verified good
			if(c){//this will be a match number we should join(we will automatically be accepted, for a certain amount of time)

				uzenet_state |= UN_DETECTED_RSVP;
				Uzenet_SpiRamTxB(UN_CMD_JOIN_MATCH);//server responds to this command...
				Uzenet_SpiRamTxB(c);//...0 for success, otherwise an error code
				uzenet_step = UN_STEP_CHECK_JOIN;
			}else{//otherwise no RSVP, so just wait for further instruction from the program(like user selecting UZENET)
				uzenet_step = UN_STEP_NO_MATCH;

			}

		}else if(uzenet_local_tick > 120){//no response from the server or a connection issue, fall back to preconnection(this could mean our login password is bad, or just temporary Internet issues)

			uzenet_error |= UN_ERROR_SERVER_TIMEOUT;
			uzenet_step = UN_STEP_PRECONNECT;
		}
#endif//UN_CUSTOM_RSVP_HANDLER
	}
#ifdef UN_CUSTOM_MATCH_HANDLER
	else if(uzenet_step == UN_STEP_CUSTOM_MATCH)
#else//normal match handler is sufficient for most cases, there are high resource cost to implementing complex matchmaking(consider an external utility which pre-arranges matches)
	else if(uzenet_step == UN_STEP_NO_MATCH)//STEP 16
#endif
						{//we pause here until user code gives "find opponent" intent, which will put us back to UN_STEP_REQ_RSVP until an opponent if found or the flag is unset
#ifdef UN_CUSTOM_MATCH_HANDLER
		Uzenet_CustomMatchHandler();//this user function is responsible for handling uzenet_step = UN_PLAYING_GAME
#else
//Print(6,14,PSTR("NO MATCH!!!!"));
		if(Uzenet_SpiRamRxSearchP(UN_STR_ERROR) || Uzenet_SpiRamRxSearchP(PSTR("CLOSED"))){//something broke...user code can detect this and drop out of menu
//while(1);Print(10,20,PSTR("FOUND ERROR"));while(1);
			uzenet_error |= UN_ERROR_CONNECT_FAILED;//we could recover from this by reconnecting and trying again
			uzenet_step = UN_STEP_PRECONNECT;
			return;
		}else if(Uzenet_SpiRamRxSearchP(UN_STR_PING)){//we can ignore "PING" requests so long as we send other data/commands periodically....
			Uzenet_SpiRamTxP(UN_STR_PONG);//...if we actually respond to "PING" with "PONG" the server will regard that as a timed response from the it's last request
		}
		Uzenet_SpiRamRxFlush(1);//get rid of everything, including any previous "PING" requests...we keep up with "PING" so the server can estimate latency for matchmaking purposes

		if(uzenet_local_tick > 30){//don't spam the server too frequently...but still be responsive to other player's looking for a game(timing windows of requests need to overlap)
			if(uzenet_state & UN_FIND_OPPONENT){//continually request a match(short lived request) and check if an RSVP exists(which means the match request was fulfilled)
Uzenet_SpiRamRxFlush(1);	
			Uzenet_SpiRamTxB(UN_CMD_REQ_MATCH_SIMPLE);//request a match, if it was met, the next UN_CMD_REQ_RSVP will succeed
uzenet_wait = 5;
				uzenet_step = UN_STEP_REQ_RSVP;//...note, transition between requesting a match and stopping is handled in Uzenet_SetStep()
				uzenet_local_tick = 0;//use this as a timer for server response timeout
				//Uzenet_SpiRamRxFlush(1);//discard any previously received data(we move between states fast enough that there should be no "PING" to worry about)
			}else//not looking for a match...sit here responding to "PING", until user code tells us to find an opponent...this will automatically let the server to sort by latency if there are multiple opponent options
				uzenet_local_tick = 0;//...the design of this RSVP loop is intended to make the simplest possible implementation for menus/game state changes
		}
	}else if(uzenet_step == UN_STEP_CHECK_JOIN){
		s16 c = Uzenet_SpiRamRxB();
		if(c != -1){//done waiting on a response?
			if(c){//match join was a success? then our player number is returned
				uzenet_local_player = c-1;//Uzenet server starts counts at 1...for reasons
				uzenet_step = UN_STEP_IN_MATCH;
			}else{//otherwise got some error code
				uzenet_step = UN_STEP_NO_MATCH;
			}
		}else if(uzenet_local_tick > 120){
			uzenet_error |= UN_ERROR_SERVER_TIMEOUT;
			uzenet_step = UN_STEP_PRECONNECT;
		}
		//TODO A MATCH CAN BE JOIN BY MORE PLAYERS, UNTIL ALL PLAYERS SIGNAL MATCH START....ALLOWS 2 or more players easily
#endif//UN_CUSTOM_MATCH_HANDLER
	}else if(uzenet_step == UN_STEP_IN_MATCH){// waiting for the game to begin, signal we are ready(simple case)

		Uzenet_SpiRamTxB(UN_CMD_SEND_MATCH_READY);//let everyone know we are ready, at least...
		Uzenet_SpiRamTxB(UN_CMD_CHECK_MATCH_READY);//see conditions are met to start playing(if yes, it means game is in motion, start syncing!)
		uzenet_local_tick = 0;//use this as a timer for the next step
		uzenet_step = UN_STEP_CHECK_READY;
		Uzenet_SpiRamRxFlush(1);//discard any previous data, we only care about the result of this query

	}else if(uzenet_step == UN_STEP_CHECK_READY){

		s16 c = Uzenet_SpiRamRxB();
		if(c != -1){//done waiting on a response?

			if(c>0){//server signaled ready? then this signal is also a simple LFSR we can use(worst case 0b0000001 fixes itself in < 40 iterations)

				GetPrngNumber(c);
				for(u8 i=0;i<40;i++)//8 bit seed actually seems ok, in combination with random player "ready" wait frames
					GetPrngNumber(0);
//DEBUGRXBUF();

				uzenet_step = UN_STEP_PLAYER_INFO;//UN_STEP_PLAYING;
				Uzenet_SpiRamTxB(UN_CMD_PLAYER_INFO_SIMPLE);//server sends 4 fixed length, zero padded, concatenated names
				uzenet_local_tick = 0;//use this as a timer
				uzenet_remote_tick = 0;//use this as a byte counter

				Uzenet_SpiRamRxFlush(1);
			}else{//otherwise not ready, check again

				uzenet_step = UN_STEP_IN_MATCH;
				uzenet_wait = 30;
			}
		}else if(uzenet_local_tick > 120){

			uzenet_error |= UN_ERROR_SERVER_TIMEOUT;
			uzenet_step = UN_STEP_PRECONNECT;
		}

	}else if(uzenet_step == UN_STEP_PLAYER_INFO){//retrieve player names in a fixed length format(4*max length names, zero padded)

		//SpiRamSeqWriteStart(0,(u16)(UN_PLAYER_INFO_BASE+uzenet_remote_tick));
		do{
			if(uzenet_remote_tick == (13*4)){//got all the name data(advanced games need to use a different method)
				uzenet_remote_tick = 0;//reset our temp timer
				uzenet_local_tick = 0;//reset our temp counter
				uzenet_step = UN_STEP_PLAYING;
				uzenet_state |= (UN_STATE_GOT_NAMES|UN_STATE_DO_SYNC|UN_STATE_PLAYING);
				uzenet_sync = 1;//each player will send an initial state to get rolling...
			//	Uzenet_SpiRamTxB(UN_CMD_PAD_DATA_SIMPLE);//tell the server we are sending game data(we didn't read pads this tick, neither did they)
			//	Uzenet_SpiRamTxP(PSTR("\x00\x00\x00"));//tick 0(default 2 bytes format) empty pad state(they will send the same to get us out of the first sync)
				break;
			}
			s16 c = Uzenet_SpiRamRxB();
			if(c == -1)
				break;
			SpiRamWriteU8(0,(u16)(UN_PLAYER_INFO_BASE+uzenet_remote_tick++),c);
		/*	if(uzenet_remote_tick < 20)
				PrintByte(10,uzenet_remote_tick,c,1);
			else{
				uzenet_error|= UN_ERROR_CRITICAL;
				while(1);
			}*/
			uzenet_local_tick = 0;//reset timer since we got data
		}while(1);
		//SpiRamSeqWriteEnd();

		if(uzenet_local_tick > 120){

			uzenet_error |= UN_ERROR_SERVER_TIMEOUT;
			uzenet_step = UN_STEP_PRECONNECT;
		}


	}/*else if(uzenet_step == UN_STEP_QUIT_GAME){

		uzenet_step = UN_STEP_TX_BREAK;
		uzenet_remote_tick = 0;//set a timer, we need to wait a bit to make sure we are in a new transparent mode timing window to send '+++'

	}else if(uzenet_step == UN_STEP_TX_BREAK){

		if(!uzenet_remote_tick)
			uzenet_remote_tick = 1;
		else{
			Uzenet_SpiRamTxP(PSTR("+++"));
			uzenet_step = UN_STEP_DISCONNECT;
		}

	}else if(uzenet_step == UN_STEP_DISCONNECT_SERVER){

		if(!uzenet_remote_tick)
			uzenet_remote_tick = 1;
		else{
			Uzenet_SpiRamTxP("AT+CIPCLOSE\r\n");
			uzenet_step = UN_STEP_PRE_CONNECT;
		}

	}*/


		

	/*else if(uzenet_step == UN_STEP_GET_LFSR){//get the server LFSR(it was created the moment the match was, remote will get the same)

		s16 c = Uzenet_SpiRamRxB();
		if(c != -1){//done waiting on a response?
			if(c == 1){//ready?
				uzenet_step = UN_STEP_PLAYING;
			//	Uzenet_SpiRamTxB(UN_CMD_GET_LFSR_SIMPLE);//get the first LFSR only(remote side will get the same number from the server)
			//	Uzenet_SpiRamTxB(UN_CMD_GET_PLAYER_FLOW_SIMPLE);//allow player data to come to us(after we get the LFSR!)
			}else{//otherwise not ready, check again
				uzenet_step = UN_STEP_IN_MATCH;
				uzenet_wait = 30;
			}
		}else if(uzenet_local_tick > 120){

			uzenet_error |= UN_ERROR_SERVER_TIMEOUT;
			uzenet_step = UN_STEP_PRECONNECT;
		}

	}*/
}



/*
u8 GetScanCode(u8 command){
	static u8 state=0;
	u8 data=0;

	unsigned char i;

	if(state==0){//ready to transmit condition
		JOYPAD_OUT_PORT&=~(_BV(JOYPAD_CLOCK_PIN));
		JOYPAD_OUT_PORT|=_BV(JOYPAD_LATCH_PIN);
		Wait200ns();
		Wait200ns();
		Wait200ns();
		Wait200ns();
		Wait200ns();
		JOYPAD_OUT_PORT&=~(_BV(JOYPAD_LATCH_PIN));
		JOYPAD_OUT_PORT|=_BV(JOYPAD_CLOCK_PIN);
		Wait200ns();
		Wait200ns();
		Wait200ns();
		Wait200ns();
		Wait200ns();

		if(command==KB_SEND_END)
			state=0;
		else
			state=1;
		
	}

	//read button states
	for(i=0;i<8;i++){

		data<<=1;
		//data out
		if(command&0x80)
			JOYPAD_OUT_PORT|=_BV(JOYPAD_LATCH_PIN);
		else
			JOYPAD_OUT_PORT&=~(_BV(JOYPAD_LATCH_PIN));
		

		//pulse clock pin
		JOYPAD_OUT_PORT&=~(_BV(JOYPAD_CLOCK_PIN));

		command<<=1;
		Wait100ns();

		if((JOYPAD_IN_PORT&(1<<JOYPAD_DATA2_PIN))!=0) data|=1;

		JOYPAD_OUT_PORT|=_BV(JOYPAD_CLOCK_PIN);

	}

	JOYPAD_OUT_PORT&=~(_BV(JOYPAD_LATCH_PIN));

	return data;

}
*/
#endif//UZENET_H
