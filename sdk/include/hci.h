
#define VERSION "(\"GECKO TWS CHARGER\" - Charger MCU Firware  - "__DATE__" - "__TIME__")"
//#define GECKO_FW_VERSION 			0121 			/*  software revision */
#define PLATFORMSTRING 				"GECKO1109-v0101"
#define MANUFACTURER_NAME  		"Gecko Inc."
//////////#define CHIP_LOCAL_NAME        "B52"
#define CHIP_LOCAL_NAME        "AK47"

enum{
	UART1=0x01,
	UART2,
};

enum{
	UART_SPEED_9600BPS=9600,
	UART_SPEED_19200BPS=115200,
	UART_SPEED_38400BPS=38400,
	UART_SPEED_115200BPS=115200,	
};

enum{
	UART_RX_MODE=0x01,
	UART_TX_MODE,
};

// internal message id
enum{
	/* tws uart message define */
	MSG_GET_VBAT_REQ=0x01,
	MSG_GET_VBAT_CNF,
	MSG_GET_TWS_BTADDR_REQ,
	MSG_GET_TWS_BTADDR_CNF,
	MSG_CLEAR_PAIR_REQ,
	MSG_DELETE_PAIRED_DEVICE_REQ,
	MSG_BATCASE_OPEN_IND,
	MSG_BATCASE_CLOSE_IND,
	
	/* lightning message define */	
	MSG_74_CMD_RES,
	MSG_75_CMD_CNF,
};


#define HCI_cmdCLEAR_PAIR													0x01
#define HCI_cmdPAIR																0x02
#define HCI_cmdREAD_ADDR													0x03
#define HCI_cmdPOWER_OFF													0x03
#define HCI_cmdGET_VBAT_LVL												0x04
