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



#define UN_ERROR_TRANSIENT		1//a(possibly) temporary error, which might have been caused by temporary network conditions
#define UN_ERROR_CONNECT_FAILED	2//possibly a transient error
#define UN_ERROR_SERVER_TIMEOUT	4
#define UN_ERROR_CRITICAL		128//an error that wont be fixed by resetting and trying again

#define UN_MATCH_RSVP_DETECTED 2//a pre-arranged match was detected, bypass the requirement for the user to manuall select UZENET option
#define UN_MATCH_HAVE_NAMES	4//we have all the names of the players in the room?
#define UN_MATCH_WAS_KICKED	8//we got kicked from the room somehow?

#define UN_COMMAND_UPPERCASE	10
#define UN_GET_PLAYER_NAME1	21
#define UN_GET_PLAYER_NAME2	22

#define UN_PASS_EEPROM_OFF	23//8 characters long
#define UN_HOST_EEPROM_OFF	11//up to 8 characters long, used as a suffix to "uze", so a value of "net.us" implies the hostname "uzenet.us"(was the old/unused MAC attribute).
#define UN_SPIR_BASE		512
#define UN_SPIR_BASE_TX	UN_SPIR_BASE+0
#define UN_SPIR_BASE_RX	UN_SPIR_BASE_TX+128
#define UN_PASS_BASE		UN_SPIR_BASE_RX+512
#define UN_HOST_BASE		UN_PASS_BASE+16

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
#define UN_STEP_PRECONNECT	10
#define UN_STEP_GAME_SPECIFY	11
#define UN_STEP_REQ_RSVP	12
#define UN_STEP_CHECK_RSVP	13
#define UN_STEP_CHECK_JOIN	14
#define UN_STEP_NO_MATCH	15
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

//TODO NEED TO SUPPORT NEW BAUD FORMAT IN EMULATOR BEFORE DOING THIS
//#define UN_CUSTOM_BAUD_DIVISOR	30
//const char UN_CUSTOM_BAUD_NUM[] PROGMEM = "57600";//the equivalent baud rate to the above divisor, in string form(for the AT command)


//#define UN_MATCH_MAKING_STYLE	1//override the default simple "join/host any game without password" behavior



u8 local_player;
volatile u16 uzenet_state;
volatile u8 uzenet_sync;
volatile u8 uzenet_error;
volatile u8 uzenet_step,uzenet_wait,uzenet_tick;
u8 uzenet_remote_tick;
volatile u8 uzenet_local_player;
u16 uzenet_spir_rx_pos, uzenet_spir_tx_pos;
u16 uzenet_spir_rx_len, uzenet_spir_rx_pos;//, uzenet_spir_rx_expected;
u8 uzenet_spir_tx_len;
s8 uzenet_match_start;//eat up to(and inclusively through) a given character
s8 uzenet_match_end;

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
}
/*
void Uzenet_SpiRamTxR(u8 *d, u8 len){
	SpiRamSeqWriteStart(0, (u16)(uzenet_spir_tx_pos+uzenet_spir_tx_len));
	while(len--)
		SpiRamSeqWriteU8(*d++);
	SpiRamSeqWriteEnd();
}
*/
u16 Uzenet_SpiRamRxB(){//returns the earliest unread byte, control positioning with timing, flushes, and or positioning
	if(!uzenet_spir_rx_len)
		return -1;
	u8 b = SpiRamReadU8(0, (u16)(uzenet_spir_rx_pos++));
	uzenet_spir_rx_len--;
	return b;
}

void Uzenet_SetStep(u8 s){

	if(uzenet_step == UN_STEP_CHECK_RSVP)//in case we have a floating request(we wont be around for), end it so another player isn't waiting...
		Uzenet_SpiRamTxB(UN_CMD_DATA_LINK_ESCAPE);//this basically breaks us out of the match request

	uzenet_step = s;
}

u8 Uzenet_Sync(u8 s){
	uzenet_sync = s;
	return (uzenet_remote_tick != uzenet_tick);
}

u8 Uzenet_GameError(){
	if(uzenet_step >= UN_STEP_PLAYING)
		return uzenet_error;
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
	uzenet_match_start = 0;
	uzenet_match_end = 0;
	if(iu)
		InitUartRxBuffer();
}

void Uzenet_SpiRamTxFlush(u8 iu){//don't call this until you are done transmitting!
	uzenet_spir_tx_pos = UN_SPIR_BASE_TX;
	uzenet_spir_tx_len = 0;
	if(iu)
		InitUartTxBuffer();
}
/*
void Uzenet_SpiRamRxRepos(u16 p){
	uzenet_spir_rx_pos = p;
	uzenet_spir_rx_len = 0;
	uzenet_spir_rx_expected = 0;
}

void Uzenet_SpiRamTxRepos(u16 p){//don't call this until you are done transmitting!
	uzenet_spir_tx_pos = p;
	uzenet_spir_tx_len = 0;
}
*/

int total_sent = 0;
void Uzenet_Update(){//designed to work with a *MINIMUM* 16 bytes Rx, and 2 byte Tx at 9600 baud. Faster speeds will need a larger Rx buffer for reliability, Tx can remain low

PrintInt(8,0,uzenet_step,1);
PrintInt(8,1,uzenet_spir_rx_pos,1);
PrintInt(8,2,uzenet_spir_rx_len,1);
PrintInt(16,0,total_sent,1);

	u8 unread = UartUnreadCount();

	if(uzenet_match_start){//discarding Rx data until we have eaten a specific character(inclusive)
		for(u8 i=0; i<unread; i++){
			if(UartReadChar() == uzenet_match_start){
				uzenet_match_start = 0;
				break;
			}
		}
	}

	if(unread){//still data left? must have found the match start, store the rest of the data to search through
		SpiRamSeqWriteStart(0, (u16)(uzenet_spir_rx_pos+uzenet_spir_rx_len));
		while(unread--){
			SpiRamSeqWriteU8(UartReadChar());
			uzenet_spir_rx_len++;
		}
		SpiRamSeqWriteEnd();
	}

	if(uzenet_error & UN_ERROR_CRITICAL){
		Uzenet_SpiRamRxFlush(1);
		Uzenet_SpiRamTxFlush(1);
		return;
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

	ReadControllers();
	uzenet_tick++;//always increment timer, used for multiple things

	if(uzenet_step >= UN_STEP_PLAYING){//intercepting joypad input to perform networking transparently

//		return;
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
		uzenet_tick = 0;//set a timer for the device to boot against in the next step(we will get a bunch of data in the coming frames for the boot message)
		uzenet_step = UN_STEP_CHECK_BOOT;

	}else if(uzenet_step == UN_STEP_CHECK_BOOT){//wait for "ready\r\n" which indicates the boot is done, and it is at the expected baud rate

		if(Uzenet_SpiRamRxSearchP(PSTR("ready\r\n"))){
			uzenet_state |= UN_ESP8266_DETECTED;
			Uzenet_SpiRamTxP(PSTR("ATE0\r\n"));//turn off echo(if we got this far, this should always work)
			uzenet_step = UN_STEP_SET_BAUD;
		}
		if(uzenet_tick > 120)
			uzenet_error |= UN_ERROR_CRITICAL;//at least this is useful, we built a list of what is present and what is not

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

		Uzenet_SpiRamTxP(PSTR("AT+CIPSTART=\"TCP\",\"uze"));//due to EEPROM limits, we don't store the entire domain name. We instead assume an "uze" prefix, ie "uzenet.us" represented by "net.us"
		for(u8 i=UN_HOST_EEPROM_OFF;i<UN_HOST_EEPROM_OFF+8;i++){
			char c = ReadEeprom(i) & 0b01111111;//the top bits of these are special game flags not part of the hostname
			if(c == 0)
				break;
			Uzenet_SpiRamTxB(ReadEeprom(i));
		}
		Uzenet_SpiRamRxFlush(1);//scrap previous reponses, we just want to see if we linked
		Uzenet_SpiRamTxP(PSTR("\",2345,7200\r\n"));//response should be "OK\r\nLINKED\r\n" or "ERROR\r\n"
		uzenet_tick = 0;//use this as a timer for response timeout
		uzenet_step = UN_STEP_CHECK_CONNECT;

	}else if(uzenet_step == UN_STEP_CHECK_CONNECT){//wait for the connection response(this is the first time we really error check, for simplicity)

		if(Uzenet_SpiRamRxSearchP(PSTR("OK\r\n"))){
			uzenet_state |= UN_ESP8266_VERIFIED;
			Uzenet_SpiRamTxP(PSTR("AT+CIPMODE=1\r\n"));//set single mode(assume this works...we will find out later)
			uzenet_wait = 1+(14/UART_TX_BUFFER_SIZE);
			uzenet_step = UN_STEP_SET_SEND;
		}else if(uzenet_tick > 180 || Uzenet_SpiRamRxSearchP(PSTR("ERROR\r\n"))){//failed?
			uzenet_step = UN_STEP_PRECONNECT;//
			uzenet_error |= UN_ERROR_TRANSIENT;//program can see we met prerequisites but are "awaiting connect", this might work next time it's attempted(temporary Internet issue?)
		}

	}else if(uzenet_step == UN_STEP_SET_SEND){//start transparent transmission(assume this works...we will find out later if we can't talk to the server)

		Uzenet_SpiRamTxP(PSTR("AT+CIPSEND\r\n"));
		uzenet_wait = 1+(12/UART_TX_BUFFER_SIZE);
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
		Uzenet_SpiRamTxP(UN_STR_PONG);//this will ensure the server doesn't bother us with "PING" requests for a while, so we can assume the next byte is reponse to a command
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
		Uzenet_CustomRsvpHandler();//this user function is responsible for handling uzenet_step = UN_STEP_CUSTOM_MATCH
#else//normal RSVP handler is probably sufficient for most every case
		Uzenet_SpiRamTxB(UN_CMD_CHECK_RSVP);//this command checks is we previously arranged a match(external program before this join), returning 0 or a room number 1-255
		uzenet_step = UN_STEP_CHECK_RSVP;
		uzenet_tick = 0;//use this as a timer for server response timeout
		Uzenet_SpiRamRxFlush(1);//discard any previously received data(we move between states fast enough that there should be no "PING" to worry about)
		//uzenet_wait = 1;//wait for a possible error response to the above send, incase the previous command is an error(indicating the connection dropped)

	}else if(uzenet_step == UN_STEP_CHECK_RSVP){//see if we did have an RSVP(this also verifies communication with the server, and the RSVP is for this game ROM)

		//we previously sent "PONG" so the server doesn't "PING" us for a while; any byte we received here should be a response the previous RSVP request, or possibly a ESP8266 disconnect error
		if(Uzenet_SpiRamRxSearchP(UN_STR_ERROR)){//something broke, there is no recovery except to reconnect and try again...
			uzenet_error |= UN_ERROR_CONNECT_FAILED;//user code should drop out of menu, and the user reselecting UZENET will cause the connection to be re-attempted
			uzenet_step = UN_STEP_PRECONNECT;
			return;
		}
		u16 c = Uzenet_SpiRamRxB();//in transparent mode any byte we receive is either an out of band error, or the response from the server(so we should see response to UN_CMD_CHECK_RSVP here) 
		if(c != -1){//done waiting on a response?
			uzenet_state |= UN_SERVER_DETECTED;//then our login credentials are also verified good
			if(c){//this will be a match number we should join(we will automatically be accepted, for a certain amount of time)
				uzenet_state |= UN_DETECTED_RSVP;
				Uzenet_SpiRamTxB(UN_CMD_JOIN_MATCH);//server responds to this command...
				Uzenet_SpiRamTxB(c);//...with 0 for success, or an error code
				uzenet_step = UN_STEP_CHECK_JOIN;
			}else{//otherwise no RSVP, so just wait for further instruction from the program(like user selecting UZENET)
				uzenet_step = UN_STEP_NO_MATCH;
			}

		}else if(uzenet_tick > 120){//no response from the server or a connection issue, fall back to preconnection(this could mean our login password is bad, or just temporary Internet issues)
			uzenet_error |= UN_ERROR_SERVER_TIMEOUT;
			uzenet_step = UN_STEP_PRECONNECT;
		}
#endif//UN_CUSTOM_RSVP_HANDLER
	}
#ifdef UN_CUSTOM_MATCH_HANDLER
	else if(uzenet_step == UN_STEP_CUSTOM_MATCH)
#else//normal match handler is sufficient for most cases, there are high resource cost to implementing complex matchmaking(consider an external utility which pre-arranges matches)
	else if(uzenet_step == UN_STEP_NO_MATCH)
#endif
						{//we pause here until user code gives "find opponent" intent, which will put us back to UN_STEP_REQ_RSVP until an opponent if found or the flag is unset
#ifdef UN_CUSTOM_MATCH_HANDLER
		Uzenet_CustomMatchHandler();//this user function is responsible for handling uzenet_step = UN_PLAYING_GAME
#else
		if(Uzenet_SpiRamRxSearchP(UN_STR_ERROR) || Uzenet_SpiRamRxSearchP(PSTR("CLOSED"))){//something broke...user code can detect this and drop out of menu
			uzenet_error |= UN_ERROR_CONNECT_FAILED;//we could recover from this by reconnecting and trying again
			uzenet_step = UN_STEP_PRECONNECT;
			return;
		}else if(Uzenet_SpiRamRxSearchP(UN_STR_PING)){//we can ignore "PING" requests so long as we send other data/commands periodically....
			Uzenet_SpiRamTxP(UN_STR_PONG);//...if we actually respond to "PING" with "PONG" the server will regard that as a timed response from the it's last request
		}
		Uzenet_SpiRamRxFlush(1);//get rid of everything, including any previous "PING" requests...we keep up with "PING" so the server can estimate latency for matchmaking purposes

		if(uzenet_tick > 30){//don't spam the server too frequently...but still be responsive to other player's looking for a game(timing windows of requests need to overlap)
			if(uzenet_state & UN_FIND_OPPONENT){//continually request a match(short lived request) and check if an RSVP exists(which means the match request was fulfilled)
				Uzenet_SpiRamTxB(UN_CMD_REQ_MATCH_SIMPLE);//request a match, if it was met, the next UN_CMD_REQ_RSVP will succeed
				uzenet_step = UN_STEP_REQ_RSVP;//...note, transition between requesting a match and stopping is handled in Uzenet_SetStep()
				uzenet_tick = 0;//use this as a timer for server response timeout
				Uzenet_SpiRamRxFlush(1);//discard any previously received data(we move between states fast enough that there should be no "PING" to worry about)
			}else//not looking for a match...sit here responding to "PING", until user code tells us to find an opponent...this will automatically let the server to sort by latency if there are multiple opponent options
				uzenet_tick = 0;//...the design of this RSVP loop is intended to make the simplest possible implementation for menus/game state changes
		}
	}else if(uzenet_step == UN_STEP_CHECK_JOIN){//TODO a match is different than a room!
		u16 c = Uzenet_SpiRamRxB();
		if(c != -1){//done waiting on a response?
			if(c == 0)//success?
				uzenet_step = UN_STEP_PLAYING;
			else//got some error code
				uzenet_step = UN_STEP_NO_MATCH;
		}
		//TODO A MATCH CAN BE JOIN BY MORE PLAYERS, UNTIL ALL PLAYERS SIGNAL MATCH START....ALLOWS 2 or more players easily
#endif//UN_CUSTOM_MATCH_HANDLER
	}
}

#endif//UZENET_H
