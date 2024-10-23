#include <string.h>

#include "websocket.h"


void demask(char *data,int len,char *mask){    
	int i;    
	for (i = 0;i < len;i ++)        
		*(data+i) ^= *(mask+(i%4));
}

char* decode_packet(unsigned char *stream, char *mask, int length, int *ret) {

	nty_ophdr *hdr =  (nty_ophdr*)stream;
	unsigned char *data = stream + sizeof(nty_ophdr);
	int size = 0;
	int start = 0;
	//char mask[4] = {0};
	int i = 0;

	if ((hdr->mask & 0x7F) == 126) {

		nty_websocket_head_126 *hdr126 = (nty_websocket_head_126*)data;
		size = hdr126->payload_length;
		
		for (i = 0;i < 4;i ++) {
			mask[i] = hdr126->mask_key[i];
		}
		
		start = 8;
		
	} else if ((hdr->mask & 0x7F) == 127) {

		nty_websocket_head_127 *hdr127 = (nty_websocket_head_127*)data;
		size = hdr127->payload_length;
		
		for (i = 0;i < 4;i ++) {
			mask[i] = hdr127->mask_key[i];
		}
		
		start = 14;

	} else {
		size = hdr->payload_length;

		memcpy(mask, data, 4);
		start = 6;
	}

	*ret = size;
	demask(stream+start, size, mask);

	return stream + start;
	
}

int encode_packet(char *buffer,char *mask, char *stream, int length) {

	nty_ophdr head = {0};
	head.fin = 1;
	head.opcode = 1;
	int size = 0;

	if (length < 126) {
		head.payload_length = length;
		memcpy(buffer, &head, sizeof(nty_ophdr));
		size = 2;
	} else if (length < 0xffff) {
		nty_websocket_head_126 hdr = {0};
		hdr.payload_length = length;
		memcpy(hdr.mask_key, mask, 4);

		memcpy(buffer, &head, sizeof(nty_ophdr));
		memcpy(buffer+sizeof(nty_ophdr), &hdr, sizeof(nty_websocket_head_126));
		size = sizeof(nty_websocket_head_126);
		
	} else {
		
		nty_websocket_head_127 hdr = {0};
		hdr.payload_length = length;
		memcpy(hdr.mask_key, mask, 4);
		
		memcpy(buffer, &head, sizeof(nty_ophdr));
		memcpy(buffer+sizeof(nty_ophdr), &hdr, sizeof(nty_websocket_head_127));

		size = sizeof(nty_websocket_head_127);
		
	}

	memcpy(buffer+2, stream, length);

	return length + 2;
}