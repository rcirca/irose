//-------------------------------------------------------------------------------------------------
/**
    @file       
    @brief      Stream Cipher Encryption Algorithm "Arcfour" 
    @author     김 대 성
    @date       2005-08-10 오후 3:12:12
*/
//-------------------------------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "Arcfour.h"



//-------------------------------------------------------------------------------------------------
/**
    @brief		초기화
*/
//-------------------------------------------------------------------------------------------------
void Arcfour::init(
				   const unsigned char *key,	///< 키값 (16바이트 이상)
				   int key_len					///< 키값길이
				   )
{
	this->arcfour_init(&enc_ctx, (const unsigned char *)key, (unsigned int)key_len);
	this->arcfour_init(&dec_ctx, (const unsigned char *)key, (unsigned int)key_len);
}

//-------------------------------------------------------------------------------------------------
/**
    @brief		encryption
*/
//-------------------------------------------------------------------------------------------------
void Arcfour::encrypt(
					  void *dest,		///< dest
					  void *src,		///< src
					  int len			///< 길이
					  )
{
	this->arcfour_encrypt(&enc_ctx, (unsigned char *)dest, (const unsigned char *)src, (unsigned int)len) ;
}

//-------------------------------------------------------------------------------------------------
/**
    @brief		encryption
*/
//-------------------------------------------------------------------------------------------------
void Arcfour::encrypt(
					  void *src,		///< src, dest
					  int len			///< 길이
					  )
{
	this->arcfour_encrypt(&enc_ctx, (unsigned char *)src, (const unsigned char *)src, (unsigned int)len) ;
}

//-------------------------------------------------------------------------------------------------
/**
    @brief		decryption
*/
//-------------------------------------------------------------------------------------------------
void Arcfour::decrypt(
					  void *dest,		///< dest
					  void *src,		///< src
					  int len			///< 길이
					  )
{
	this->arcfour_encrypt(&dec_ctx, (unsigned char *)dest, (const unsigned char *)src, (unsigned int)len);
}

//-------------------------------------------------------------------------------------------------
/**
    @brief		decryption
*/
//-------------------------------------------------------------------------------------------------
void Arcfour::decrypt(
					  void *src,		///< src, dest	
					  int len			///< 길이
					  )	
{
	this->arcfour_encrypt(&dec_ctx, (unsigned char *)src, (const unsigned char *)src, (unsigned int)len);
}

//-------------------------------------------------------------------------------------------------
/**
    @brief		초기화
*/
//-------------------------------------------------------------------------------------------------
void Arcfour::arcfour_init(
						   ArcfourContext *ctx,			///< ArcfourContext
						   const unsigned char *key,	///< 키값 (16바이트 이상)
						   unsigned int key_len			///< 키값길이
						   ) 
{ 
    unsigned int t, u; 
    unsigned int keyindex; 
    unsigned int stateindex; 
    unsigned char *state; 
    unsigned int counter; 
 
    state = ctx->state; 
    ctx->x = 0; 
    ctx->y = 0; 
 
    for (counter = 0; counter < 256; counter++) 
        state[counter] = counter; 
 
    keyindex = 0; 
    stateindex = 0; 
 
    for (counter = 0; counter < 256; counter++) 
    { 
        t = state[counter]; 
        stateindex = (stateindex + key[keyindex] + t) & 0xff; 
        u = state[stateindex]; 
        state[stateindex] = t; 
        state[counter] = u; 
        if (++keyindex >= key_len) 
            keyindex = 0; 
    } 
} 
 
//-------------------------------------------------------------------------------------------------
/**
    @brief		arcfour_byte
*/
//-------------------------------------------------------------------------------------------------
unsigned int Arcfour::arcfour_byte(
								   ArcfourContext *ctx	///< ArcfourContext
								   ) 
{ 
    unsigned int x; 
    unsigned int y; 
    unsigned int sx, sy; 
    unsigned char *state; 
 
    state = ctx->state; 
    x = (ctx->x + 1) & 0xff; 
    sx = state[x]; 
    y = (sx + ctx->y) & 0xff; 
    sy = state[y]; 
    ctx->x = x; 
    ctx->y = y; 
    state[y] = sx; 
    state[x] = sy; 
 
    return state[(sx + sy) & 0xff]; 
} 
 
//-------------------------------------------------------------------------------------------------
/**
    @brief		encryption, decryption
*/
//-------------------------------------------------------------------------------------------------
void Arcfour::arcfour_encrypt(
							  ArcfourContext *ctx,		///< ArcfourContext
							  unsigned char *dest,		///< dest
							  const unsigned char *src,	///< src
							  unsigned int len			///< 길이
							  ) 
{ 
    unsigned int i; 
    for (i = 0; i < len; i++) 
        dest[i] = src[i] ^ arcfour_byte(ctx); 
} 

///////////////////////////////////////////////////////////////////////////////////////////////////

