#include "StdAfx.h"
#include "sha.h"

#define f1(x,y,z)	( z ^ (x & (y ^ z) ) )		/* Rounds  0-19 */
#define f2(x,y,z)	( x ^ y ^ z )				/* Rounds 20-39 */
#define f3(x,y,z)	( (x & y) + (z & (x ^ y) ) )	/* Rounds 40-59 */
#define f4(x,y,z)	( x ^ y ^ z )			/* Rounds 60-79 */

/* The SHA Mysterious Constants, almost as magical as 42. */
#define K2	0x5A827999L	/* Rounds  0-19 - floor(sqrt(2) * 2^30) */
#define K3	0x6ED9EBA1L	/* Rounds 20-39 - floor(sqrt(3) * 2^30) */
#define K5	0x8F1BBCDCL	/* Rounds 40-59 - floor(sqrt(5) * 2^30) */
#define K10	0xCA62C1D6L	/* Rounds 60-79 - floor(sqrt(10) * 2^30) */

#define ROTL(n,X)  ( (X << n) | (X >> (32-n)) )

#define expandx(W,i) (t = W[i&15] ^ W[(i-14)&15] ^ W[(i-8)&15] ^ W[(i-3)&15], ROTL(1, t))
#define expand(W,i) (W[i&15] = expandx(W,i))

#define subRound(a, b, c, d, e, f, k, data) \
	( e += ROTL(5,a) + f(b, c, d) + k + data, b = ROTL(30, b) )


static void shaByteSwap(unsigned int *dest, unsigned char const *src, unsigned words)
{
	do
	{
		*dest++ = (unsigned int)((unsigned)src[0] << 8 | src[1]) << 16 |
		                  ((unsigned)src[2] << 8 | src[3]);
		src += 4;
	} while (--words);
}

void shaInit(void *priv)
{
	SHAContext *ctx = (SHAContext *)priv;

	/* Another set of constants, refered to as H(0),...,H(4) in FIPS-180-1 */
	ctx->iv[0] = 0x67452301;
	ctx->iv[1] = 0xEFCDAB89;
	ctx->iv[2] = 0x98BADCFE;
	ctx->iv[3] = 0x10325476;
	ctx->iv[4] = 0xC3D2E1F0;

	ctx->bytesHi = 0;
	ctx->bytesLo = 0;
}

void shaTransform(unsigned int *block, unsigned int *key)
{
	register unsigned int A, B, C, D, E;
	register unsigned int t;

	/* Set up first buffer */
	A = block[0];
	B = block[1];
	C = block[2];
	D = block[3];
	E = block[4];

	/* Let the heavy mangling begin, in 4 sub-rounds of 20 interations each. */
	subRound( A, B, C, D, E, f1, K2, key[ 0] );
	subRound( E, A, B, C, D, f1, K2, key[ 1] );
	subRound( D, E, A, B, C, f1, K2, key[ 2] );
	subRound( C, D, E, A, B, f1, K2, key[ 3] );
	subRound( B, C, D, E, A, f1, K2, key[ 4] );
	subRound( A, B, C, D, E, f1, K2, key[ 5] );
	subRound( E, A, B, C, D, f1, K2, key[ 6] );
	subRound( D, E, A, B, C, f1, K2, key[ 7] );
	subRound( C, D, E, A, B, f1, K2, key[ 8] );
	subRound( B, C, D, E, A, f1, K2, key[ 9] );
	subRound( A, B, C, D, E, f1, K2, key[10] );
	subRound( E, A, B, C, D, f1, K2, key[11] );
	subRound( D, E, A, B, C, f1, K2, key[12] );
	subRound( C, D, E, A, B, f1, K2, key[13] );
	subRound( B, C, D, E, A, f1, K2, key[14] );
	subRound( A, B, C, D, E, f1, K2, key[15] );
	subRound( E, A, B, C, D, f1, K2, expand(key, 16) );
	subRound( D, E, A, B, C, f1, K2, expand(key, 17) );
	subRound( C, D, E, A, B, f1, K2, expand(key, 18) );
	subRound( B, C, D, E, A, f1, K2, expand(key, 19) );

	subRound( A, B, C, D, E, f2, K3, expand(key, 20) );
	subRound( E, A, B, C, D, f2, K3, expand(key, 21) );
	subRound( D, E, A, B, C, f2, K3, expand(key, 22) );
	subRound( C, D, E, A, B, f2, K3, expand(key, 23) );
	subRound( B, C, D, E, A, f2, K3, expand(key, 24) );
	subRound( A, B, C, D, E, f2, K3, expand(key, 25) );
	subRound( E, A, B, C, D, f2, K3, expand(key, 26) );
	subRound( D, E, A, B, C, f2, K3, expand(key, 27) );
	subRound( C, D, E, A, B, f2, K3, expand(key, 28) );
	subRound( B, C, D, E, A, f2, K3, expand(key, 29) );
	subRound( A, B, C, D, E, f2, K3, expand(key, 30) );
	subRound( E, A, B, C, D, f2, K3, expand(key, 31) );
	subRound( D, E, A, B, C, f2, K3, expand(key, 32) );
	subRound( C, D, E, A, B, f2, K3, expand(key, 33) );
	subRound( B, C, D, E, A, f2, K3, expand(key, 34) );
	subRound( A, B, C, D, E, f2, K3, expand(key, 35) );
	subRound( E, A, B, C, D, f2, K3, expand(key, 36) );
	subRound( D, E, A, B, C, f2, K3, expand(key, 37) );
	subRound( C, D, E, A, B, f2, K3, expand(key, 38) );
	subRound( B, C, D, E, A, f2, K3, expand(key, 39) );

	subRound( A, B, C, D, E, f3, K5, expand(key, 40) );
	subRound( E, A, B, C, D, f3, K5, expand(key, 41) );
	subRound( D, E, A, B, C, f3, K5, expand(key, 42) );
	subRound( C, D, E, A, B, f3, K5, expand(key, 43) );
	subRound( B, C, D, E, A, f3, K5, expand(key, 44) );
	subRound( A, B, C, D, E, f3, K5, expand(key, 45) );
	subRound( E, A, B, C, D, f3, K5, expand(key, 46) );
	subRound( D, E, A, B, C, f3, K5, expand(key, 47) );
	subRound( C, D, E, A, B, f3, K5, expand(key, 48) );
	subRound( B, C, D, E, A, f3, K5, expand(key, 49) );
	subRound( A, B, C, D, E, f3, K5, expand(key, 50) );
	subRound( E, A, B, C, D, f3, K5, expand(key, 51) );
	subRound( D, E, A, B, C, f3, K5, expand(key, 52) );
	subRound( C, D, E, A, B, f3, K5, expand(key, 53) );
	subRound( B, C, D, E, A, f3, K5, expand(key, 54) );
	subRound( A, B, C, D, E, f3, K5, expand(key, 55) );
	subRound( E, A, B, C, D, f3, K5, expand(key, 56) );
	subRound( D, E, A, B, C, f3, K5, expand(key, 57) );
	subRound( C, D, E, A, B, f3, K5, expand(key, 58) );
	subRound( B, C, D, E, A, f3, K5, expand(key, 59) );

	subRound( A, B, C, D, E, f4, K10, expand(key, 60) );
	subRound( E, A, B, C, D, f4, K10, expand(key, 61) );
	subRound( D, E, A, B, C, f4, K10, expand(key, 62) );
	subRound( C, D, E, A, B, f4, K10, expand(key, 63) );
	subRound( B, C, D, E, A, f4, K10, expand(key, 64) );
	subRound( A, B, C, D, E, f4, K10, expand(key, 65) );
	subRound( E, A, B, C, D, f4, K10, expand(key, 66) );
	subRound( D, E, A, B, C, f4, K10, expand(key, 67) );
	subRound( C, D, E, A, B, f4, K10, expand(key, 68) );
	subRound( B, C, D, E, A, f4, K10, expand(key, 69) );
	subRound( A, B, C, D, E, f4, K10, expand(key, 70) );
	subRound( E, A, B, C, D, f4, K10, expand(key, 71) );
	subRound( D, E, A, B, C, f4, K10, expand(key, 72) );
	subRound( C, D, E, A, B, f4, K10, expand(key, 73) );
	subRound( B, C, D, E, A, f4, K10, expand(key, 74) );
	subRound( A, B, C, D, E, f4, K10, expand(key, 75) );
	subRound( E, A, B, C, D, f4, K10, expand(key, 76) );
	subRound( D, E, A, B, C, f4, K10, expandx(key, 77) );
	subRound( C, D, E, A, B, f4, K10, expandx(key, 78) );
	subRound( B, C, D, E, A, f4, K10, expandx(key, 79) );

	/* Build MD */
	block[0] += A;
	block[1] += B;
	block[2] += C;
	block[3] += D;
	block[4] += E;
}

void shaUpdate(void *priv, void const *bufIn, size_t len)
{
	SHAContext *ctx = (SHAContext *)priv;
	unsigned char *buf = (unsigned char *) bufIn;
	unsigned i;

	/* Update bitcount */

	unsigned int t = ctx->bytesLo;
	if ( ( ctx->bytesLo = t + len ) < t )
		ctx->bytesHi++;

	i = (unsigned)t % SHA_BLOCKBYTES;

	if (SHA_BLOCKBYTES-i > len)
	{
		memcpy((unsigned char *)ctx->key + i, buf, len);
		return;
	}

	if (i)
	{
		memcpy((unsigned char *)ctx->key + i, buf, SHA_BLOCKBYTES - i);
		shaByteSwap(ctx->key, (unsigned char *)ctx->key, SHA_BLOCKWORDS);
		shaTransform(ctx->iv, ctx->key);
		buf += SHA_BLOCKBYTES-i;
		len -= SHA_BLOCKBYTES-i;
	}

	while (len >= SHA_BLOCKBYTES)
	{
		shaByteSwap(ctx->key, buf, SHA_BLOCKWORDS);
		shaTransform(ctx->iv, ctx->key);
		buf += SHA_BLOCKBYTES;
		len -= SHA_BLOCKBYTES;
	}

	if (len)
		memcpy(ctx->key, buf, len);
}

void const *shaFinal(void *priv)
{
	SHAContext *ctx = (SHAContext *)priv;
	unsigned char *digest;
	unsigned i = (unsigned)ctx->bytesLo % SHA_BLOCKBYTES;
	unsigned char *p = (unsigned char *)ctx->key + i;	/* First unused byte */
	unsigned int t;

	*p++ = 0x80;

	i = SHA_BLOCKBYTES - 1 - i;

	if (i < 8)
	{
		memset(p, '\0', i);
		shaByteSwap(ctx->key, (unsigned char *)ctx->key, 16);
		shaTransform(ctx->iv, ctx->key);
		p = (unsigned char *)ctx->key;
		i = 64;
	}
	memset(p, '\0', i-8);
	shaByteSwap(ctx->key, (unsigned char *)ctx->key, 14);

	ctx->key[14] = ctx->bytesHi << 3 | ctx->bytesLo >> 29;
	ctx->key[15] = ctx->bytesLo << 3;
	shaTransform(ctx->iv, ctx->key);

	digest = (unsigned char *)ctx->iv;
	for (i = 0; i < SHA_HASHWORDS; i++) {
		t = ctx->iv[i];
		digest[0] = (unsigned char)(t >> 24);
		digest[1] = (unsigned char)(t >> 16);
		digest[2] = (unsigned char)(t >> 8);
		digest[3] = (unsigned char)t;
		digest += 4;
	}
	return (unsigned char const *)ctx->iv;
}
