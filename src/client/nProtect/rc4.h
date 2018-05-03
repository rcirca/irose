#ifndef RC4_H
#define RC4_H

#define RC4_INT		unsigned int
#define RC4_CHUNK	unsigned long
#define RC4_INDEX

typedef struct rc4_key_st
{
	RC4_INT x,y;
	RC4_INT data[256];
} RC4_KEY;

void RC4_set_key(RC4_KEY *key, int len, const unsigned char *data);
void RC4(RC4_KEY *key, unsigned long len, const unsigned char *indata,unsigned char *outdata);

#endif