//-------------------------------------------------------------------------------------------------
/**
    @file       Arcfour.h
    @brief      Stream Cipher Encryption Algorithm "Arcfour" 
    @author     김 대 성
	@date       2005-08-10 오후 3:12:07
*/
//-------------------------------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////////////////////////

#if !defined(_ARCFOUR_H_)
#define _ARCFOUR_H_

///////////////////////////////////////////////////////////////////////////////////////////////////

//-------------------------------------------------------------------------------------------------
/**
    @brief      class Arcfour
*/
//-------------------------------------------------------------------------------------------------
class Arcfour
{
public:
	// 초기화
	void init(const unsigned char *key, int key_len); 

	// encryption
	void encrypt(void *dest, void *src, int len); 

	// encryption
	void encrypt(void *src, int len); 

	// decryption
	void decrypt(void *dest, void *src, int len); 

	// decryption
	void decrypt(void *src, int len); 

private:
	typedef struct 
	{ 
		unsigned int x; 
		unsigned int y; 
		unsigned char state[256]; 
	} ArcfourContext; 

	ArcfourContext enc_ctx;		// encryption ArcfourContext
	ArcfourContext dec_ctx;		// decryption ArcfourContext
	
	// 초기화
	void arcfour_init(ArcfourContext *ctx, const unsigned char *key, unsigned int key_len);

	// arcfour_byte
	unsigned int arcfour_byte(ArcfourContext *ctx); 

	// encryption, decryption
	void arcfour_encrypt(ArcfourContext *ctx, unsigned char *dest, const unsigned char *src, unsigned int len);
};

///////////////////////////////////////////////////////////////////////////////////////////////////



#endif // !defined(_ARCFOUR_H_)

///////////////////////////////////////////////////////////////////////////////////////////////////