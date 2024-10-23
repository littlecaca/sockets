struct _nty_ophdr {

	unsigned char opcode:4,
		 rsv3:1,
		 rsv2:1,
		 rsv1:1,
		 fin:1;
	unsigned char payload_length:7,
		mask:1;

} __attribute__ ((packed));

struct _nty_websocket_head_126 {
	unsigned short payload_length;
	char mask_key[4];
	unsigned char data[8];
} __attribute__ ((packed));

struct _nty_websocket_head_127 {

	unsigned long long payload_length;
	char mask_key[4];

	unsigned char data[8];
	
} __attribute__ ((packed));

typedef struct _nty_websocket_head_127 nty_websocket_head_127;
typedef struct _nty_websocket_head_126 nty_websocket_head_126;
typedef struct _nty_ophdr nty_ophdr;

int encode_packet(char *buffer,char *mask, char *stream, int length);
char* decode_packet(unsigned char *stream, char *mask, int length, int *ret);
