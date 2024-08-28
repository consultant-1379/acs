#include "StdAfx.h"
#include "twofish.h"
#include "table.h"
#include <memory.h>
#pragma warning (disable : 4244)

#define		CONST
CONST		fullSbox MDStab;		/* not actually const.  Initialized ONE time */
int			needToBuildMDS=1;		/* is MDStab initialized yet? */
int			debug = 0;				/* Set to 1 and compile with the debug configuration for
									full cihper debug output. Output will be sent to logfile 
									using WriteLog() and is very VERY noisy */

int			numRounds[4]= {0,ROUNDS_128,ROUNDS_192,ROUNDS_256};

#define		_sBox_	 key->sBox8x32
#define _sBox8_(N) (((BYTE *) _sBox_) + (N)*256)

#define	Fe32_(x,R) (_sBox_[0][2*_b(x,R  )] ^ _sBox_[0][2*_b(x,R+1)+1] ^	\
				    _sBox_[2][2*_b(x,R+2)] ^ _sBox_[2][2*_b(x,R+3)+1])

#define sbSet(N,i,J,v) { _sBox_[N&2][2*i+(N&1)+2*J]=MDStab[N][v]; }

#include	"debug.h"


int setRounds(int keyLen,int nRounds)
{
	if ((nRounds < 2) || (nRounds > MAX_ROUNDS) || (nRounds & 1))
		return FALSE;
	
	switch (keyLen)
	{
		case 128:	numRounds[1]=nRounds;	break;
		case 192:	numRounds[2]=nRounds;	break;
		case 256:	numRounds[3]=nRounds;	break;
		default:	return FALSE;
	}
	
	return TRUE;
}

int ParseHexDword(int bits,CONST char *srcTxt,DWORD *d,char *dstTxt)
{
	int i;
	char c;
	DWORD b;
	
	for (i=0;i*32<bits;i++)
		d[i]=0;
	
	for (i=0;i*4<bits;i++)
	{
		c=srcTxt[i];
		if (dstTxt) dstTxt[i]=c;
		if ((c >= '0') && (c <= '9'))
			b=c-'0';
		else if ((c >= 'a') && (c <= 'f'))
			b=c-'a'+10;
		else if ((c >= 'A') && (c <= 'F'))
			b=c-'A'+10;
		else
			return BAD_KEY_MAT;	/* This should not happen */
		d[i/8] |= b << (4*((i^1)&7));
	}
	return 0;
}


DWORD RS_MDS_Encode(DWORD k0,DWORD k1)
{
	int i,j;
	DWORD r;

	for (i=r=0;i<2;i++)
	{
		r ^= (i) ? k0 : k1;
		for (j=0;j<4;j++)
			RS_rem(r);
	}
	return r;
}


void BuildMDS(void)
{
	int i;
	DWORD d;
	BYTE m1[2],mX[2],mY[2];
	
	for (i=0;i<256;i++)
	{
		m1[0]=P8x8[0][i];
		mX[0]=(BYTE) Mul_X(m1[0]);
		mY[0]=(BYTE) Mul_Y(m1[0]);
		
		m1[1]=P8x8[1][i];
		mX[1]=(BYTE) Mul_X(m1[1]);
		mY[1]=(BYTE) Mul_Y(m1[1]);

		// Temporary redefine these.
#undef	Mul_1
#undef	Mul_X
#undef	Mul_Y
#define	Mul_1	m1
#define	Mul_X	mX				
#define	Mul_Y	mY
		
#define	SetMDS(N)				\
	b0(d) = M0##N[P_##N##0];	\
	b1(d) = M1##N[P_##N##0];	\
	b2(d) = M2##N[P_##N##0];	\
	b3(d) = M3##N[P_##N##0];	\
		MDStab[N][i] = d;
		
		SetMDS(0);
		SetMDS(1);
		SetMDS(2);
		SetMDS(3);
	}
#undef	Mul_1
#undef	Mul_X
#undef	Mul_Y
#define	Mul_1	Mx_1
#define	Mul_X	Mx_X
#define	Mul_Y	Mx_Y
	
	needToBuildMDS=0;
}


void ReverseRoundSubkeys(keyInstance *key,BYTE newDir)
{
	DWORD t0,t1;
	register DWORD *r0=key->subKeys+ROUND_SUBKEYS;
	register DWORD *r1=r0 + 2*key->numRounds - 2;
	
	for (;r0 < r1;r0+=2,r1-=2)
	{
		t0=r0[0];
		t1=r0[1];
		r0[0]=r1[0];
		r0[1]=r1[1];
		r1[0]=t0;
		r1[1]=t1;
	}
	
	key->direction=newDir;
}

void Xor256(void *dst,void *src,BYTE b)
{ 
	register DWORD		x=b*0x01010101u;
	register DWORD *d=(DWORD *)dst;
	register DWORD *s=(DWORD *)src;
#define X_8(N)	{ d[N]=s[N] ^ x; d[N+1]=s[N+1] ^ x; }
#define X_32(N)	{ X_8(N); X_8(N+2); X_8(N+4); X_8(N+6); }

	X_32(0 ); X_32( 8); X_32(16); X_32(24);
	d+=32;
	s+=32;
	X_32(0 ); X_32( 8); X_32(16); X_32(24);
}

int reKey(keyInstance *key)
{
	int		i,j,k64Cnt,keyLen;
	int		subkeyCnt;
	DWORD	A,B,q;
	DWORD	sKey[MAX_KEY_BITS/64],k32e[MAX_KEY_BITS/64],k32o[MAX_KEY_BITS/64];
	BYTE	L0[256],L1[256];


	if (needToBuildMDS)
		BuildMDS();

	/* This was a function, but this is faster */
#define	F32(res,x,k32)	\
	{															\
	DWORD t=x;													\
	switch (k64Cnt & 3)											\
	    {														\
		case 0:													\
					b0(t)   = p8(04)[b0(t)] ^ b0(k32[3]);		\
					b1(t)   = p8(14)[b1(t)] ^ b1(k32[3]);		\
					b2(t)   = p8(24)[b2(t)] ^ b2(k32[3]);		\
					b3(t)   = p8(34)[b3(t)] ^ b3(k32[3]);		\
																\
		case 3:		b0(t)   = p8(03)[b0(t)] ^ b0(k32[2]);		\
					b1(t)   = p8(13)[b1(t)] ^ b1(k32[2]);		\
					b2(t)   = p8(23)[b2(t)] ^ b2(k32[2]);		\
					b3(t)   = p8(33)[b3(t)] ^ b3(k32[2]);		\
																\
		case 2:													\
			res=	MDStab[0][p8(01)[p8(02)[b0(t)] ^ b0(k32[1])] ^ b0(k32[0])] ^	\
					MDStab[1][p8(11)[p8(12)[b1(t)] ^ b1(k32[1])] ^ b1(k32[0])] ^	\
					MDStab[2][p8(21)[p8(22)[b2(t)] ^ b2(k32[1])] ^ b2(k32[0])] ^	\
					MDStab[3][p8(31)[p8(32)[b3(t)] ^ b3(k32[1])] ^ b3(k32[0])] ;	\
		}														\
	}

	subkeyCnt = ROUND_SUBKEYS + 2*key->numRounds;
	keyLen=key->keyLen;
	k64Cnt=(keyLen+63)/64;
	for (i=0,j=k64Cnt-1;i<k64Cnt;i++,j--)
	{
		k32e[i]=key->key32[2*i  ];
		k32o[i]=key->key32[2*i+1];
		sKey[j]=key->sboxKeys[j]=RS_MDS_Encode(k32e[i],k32o[i]);
	}

	for (i=q=0;i<subkeyCnt/2;i++,q+=SK_STEP)	
	{
		F32(A,q        ,k32e);
		F32(B,q+SK_BUMP,k32o);
		B = ROL(B,8);
		key->subKeys[2*i  ] = A+B;
		B = A + 2*B;
		key->subKeys[2*i+1] = ROL(B,SK_ROTL);
	}

	#define one256(N,J) sbSet(N,i,J,p8(N##1)[p8(N##2)[L0[i+J]]^k1]^k0);
	#define	sb256(N) {										\
		Xor256(L1,p8(N##4),b##N(sKey[3]));					\
		for (i=0;i<256;i+=2) {L0[i  ]=p8(N##3)[L1[i]];		\
							  L0[i+1]=p8(N##3)[L1[i+1]]; }	\
		Xor256(L0,L0,b##N(sKey[2]));						\
		{ register DWORD k0=b##N(sKey[0]);					\
		  register DWORD k1=b##N(sKey[1]);					\
		  for (i=0;i<256;i+=2) { one256(N,0); one256(N,1); } } }
	sb256(0); sb256(1);	sb256(2); sb256(3);

#ifdef DEBUG
	DebugDumpKey(key);
#endif

	if (key->direction == DIR_ENCRYPT){
		ReverseRoundSubkeys(key,DIR_ENCRYPT);
        }

	return TRUE;
}

int makeKey(keyInstance *key, BYTE direction, int keyLen,CONST char *keyMaterial)
{
	if (key == NULL)			
		return BAD_KEY_INSTANCE;
	if ((direction != DIR_ENCRYPT) && (direction != DIR_DECRYPT))
		return BAD_KEY_DIR;
	if ((keyLen > MAX_KEY_BITS) || (keyLen < 8) || (keyLen & 0x3F))
		return BAD_KEY_MAT;
	if (keyMaterial == NULL)	
		return BAD_KEY_MAT;
	key->keySig = VALID_SIG;

	key->direction	= direction;
	key->keyLen		= (keyLen+63) & ~63;
	key->numRounds	= numRounds[(keyLen-1)/64];
	memset(key->key32,0,sizeof(key->key32));

	if (ParseHexDword(keyLen,keyMaterial,key->key32,key->keyMaterial))
		return BAD_KEY_MAT;	

	key->keyMaterial[MAX_KEY_SIZE]=0;

	return reKey(key);
}


int cipherInit(cipherInstance *cipher, BYTE mode,CONST char *IV)
{
	int i;

	if (cipher == NULL)			
		return BAD_PARAMS;
	if ((mode != MODE_ECB) && (mode != MODE_CBC) && (mode != MODE_CFB1))
		return BAD_CIPHER_MODE;
	if ((mode != MODE_ECB) && (IV == NULL))
		return BAD_PARAMS;
	cipher->cipherSig = VALID_SIG;

	if (mode != MODE_ECB)
	{
		if (ParseHexDword(BLOCK_SIZE,IV,cipher->iv32,NULL))
			return BAD_IV_MAT;
		for (i=0;i<BLOCK_SIZE/32;i++)
			((DWORD *)cipher->IV)[i] = Bswap(cipher->iv32[i]);
	}

	cipher->mode = mode;

	return TRUE;
}

int blockEncrypt(cipherInstance *cipher, keyInstance *key,CONST BYTE *input,
				int inputLen, BYTE *outBuffer)
{
	int   i,n;
	DWORD x[BLOCK_SIZE/32];
	DWORD t0,t1;
	int	  rounds=key->numRounds;
	BYTE  bit,ctBit,carry;
	int	  mode = cipher->mode;
	DWORD sk[TOTAL_SUBKEYS];
	DWORD IV[BLOCK_SIZE/32];


	if ((cipher == NULL) || (cipher->cipherSig != VALID_SIG))
		return BAD_CIPHER_STATE;
	if ((key == NULL) || (key->keySig != VALID_SIG))
		return BAD_KEY_INSTANCE;
	if ((rounds < 2) || (rounds > MAX_ROUNDS) || (rounds&1))
		return BAD_KEY_INSTANCE;
	if ((mode != MODE_CFB1) && (inputLen % BLOCK_SIZE))
		return BAD_INPUT_LEN;

	if (mode == MODE_CFB1)
	{
		cipher->mode = MODE_ECB;
		for (n=0;n<inputLen;n++)
		{
			blockEncrypt(cipher,key,cipher->IV,BLOCK_SIZE,(BYTE *)x);
			bit	  = 0x80 >> (n & 7);
			ctBit = (input[n/8] & bit) ^ ((((BYTE *) x)[0] & 0x80) >> (n&7));
			outBuffer[n/8] = (outBuffer[n/8] & ~ bit) | ctBit;
			carry = ctBit >> (7 - (n&7));
			for (i=BLOCK_SIZE/8-1;i>=0;i--)
			{
				bit = cipher->IV[i] >> 7;
				cipher->IV[i] = (cipher->IV[i] << 1) ^ carry;
				carry = bit;
			}
		}
		cipher->mode = MODE_CFB1;
		return inputLen;
	}

	if (key->direction != DIR_ENCRYPT)
		ReverseRoundSubkeys(key,DIR_ENCRYPT);

	memcpy(sk,key->subKeys,sizeof(DWORD)*(ROUND_SUBKEYS+2*rounds));
	if (mode == MODE_CBC)
		BlockCopy(IV,cipher->iv32)
	else
		IV[0]=IV[1]=IV[2]=IV[3]=0;

	for (n=0;n<inputLen;n+=BLOCK_SIZE,input+=BLOCK_SIZE/8,outBuffer+=BLOCK_SIZE/8)
	{
#ifdef _DEBUG
		DebugDump(input,"\n",-1,0,0,0,1);
		if (cipher->mode == MODE_CBC)
			DebugDump(IV,"",IV_ROUND,0,0,0,0);
#endif

#define	LoadBlockE(N)  x[N]=Bswap(((DWORD *)input)[N]) ^ sk[INPUT_WHITEN+N] ^ IV[N]
		LoadBlockE(0);	LoadBlockE(1);	LoadBlockE(2);	LoadBlockE(3);

#ifdef _DEBUG
		DebugDump(x,"",0,0,0,0,0);
#endif

#define	EncryptRound(K,R,id)	\
			t0	   = Fe32##id(x[K  ],0);					\
			t1	   = Fe32##id(x[K^1],3);					\
			x[K^3] = ROL(x[K^3],1);							\
			x[K^2]^= t0 +   t1 + sk[ROUND_SUBKEYS+2*(R)  ];	\
			x[K^3]^= t0 + 2*t1 + sk[ROUND_SUBKEYS+2*(R)+1];	\
			x[K^2] = ROR(x[K^2],1);							\
			DebugDump(x,"",rounds-(R),0,0,1,0);
#define		Encrypt2(R,id)	{ EncryptRound(0,R+1,id); EncryptRound(2,R,id); }

		Encrypt2(14,_);
		Encrypt2(12,_);
		Encrypt2(10,_);
		Encrypt2( 8,_);
		Encrypt2( 6,_);
		Encrypt2( 4,_);
		Encrypt2( 2,_);
		Encrypt2( 0,_);

#define	StoreBlockE(N)	((DWORD *)outBuffer)[N]=x[N^2] ^ sk[OUTPUT_WHITEN+N]
		StoreBlockE(0);	StoreBlockE(1);	StoreBlockE(2);	StoreBlockE(3);


		if (mode == MODE_CBC)
		{
			IV[0]=Bswap(((DWORD *)outBuffer)[0]);
			IV[1]=Bswap(((DWORD *)outBuffer)[1]);
			IV[2]=Bswap(((DWORD *)outBuffer)[2]);
			IV[3]=Bswap(((DWORD *)outBuffer)[3]);
		}
#ifdef DEBUG
		DebugDump(outBuffer,"",rounds+1,0,0,0,1);
		if (cipher->mode == MODE_CBC)
			DebugDump(IV,"",IV_ROUND,0,0,0,0);
#endif
	}

	if (mode == MODE_CBC)
		BlockCopy(cipher->iv32,IV);

	return inputLen;
}

int blockDecrypt(cipherInstance *cipher, keyInstance *key,CONST BYTE *input,
				int inputLen, BYTE *outBuffer)
{
	int   i,n;
	DWORD x[BLOCK_SIZE/32];
	DWORD t0,t1;
	int	  rounds=key->numRounds;
	BYTE  bit,ctBit,carry;
	int	  mode = cipher->mode;
	DWORD sk[TOTAL_SUBKEYS];
	DWORD IV[BLOCK_SIZE/32];

	if ((cipher == NULL) || (cipher->cipherSig != VALID_SIG))
		return BAD_CIPHER_STATE;
	if ((key == NULL) || (key->keySig != VALID_SIG))
		return BAD_KEY_INSTANCE;
	if ((rounds < 2) || (rounds > MAX_ROUNDS) || (rounds&1))
		return BAD_KEY_INSTANCE;
	if ((cipher->mode != MODE_CFB1) && (inputLen % BLOCK_SIZE))
		return BAD_INPUT_LEN;

	if (cipher->mode == MODE_CFB1)
	{
		cipher->mode = MODE_ECB;
		for (n=0;n<inputLen;n++)
		{
			blockEncrypt(cipher,key,cipher->IV,BLOCK_SIZE,(BYTE *)x);
			bit	  = 0x80 >> (n & 7);
			ctBit = input[n/8] & bit;
			outBuffer[n/8] = (outBuffer[n/8] & ~ bit) |
							 (ctBit ^ ((((BYTE *) x)[0] & 0x80) >> (n&7)));
			carry = ctBit >> (7 - (n&7));
			for (i=BLOCK_SIZE/8-1;i>=0;i--)
			{
				bit = cipher->IV[i] >> 7;
				cipher->IV[i] = (cipher->IV[i] << 1) ^ carry;
				carry = bit;
			}
		}
		cipher->mode = MODE_CFB1;
		return inputLen;
	}

	if (key->direction != DIR_DECRYPT)
		ReverseRoundSubkeys(key,DIR_DECRYPT);

	memcpy(sk,key->subKeys,sizeof(DWORD)*(ROUND_SUBKEYS+2*rounds));
	if (mode == MODE_CBC)
		BlockCopy(IV,cipher->iv32)
	else
		IV[0]=IV[1]=IV[2]=IV[3]=0;

	for (n=0;n<inputLen;n+=BLOCK_SIZE,input+=BLOCK_SIZE/8,outBuffer+=BLOCK_SIZE/8)
	{
#ifdef DEBUG
		DebugDump(input,"\n",rounds+1,0,0,0,1);
#endif
#define LoadBlockD(N) x[N^2]=Bswap(((DWORD *)input)[N]) ^ sk[OUTPUT_WHITEN+N]
		LoadBlockD(0);	LoadBlockD(1);	LoadBlockD(2);	LoadBlockD(3);

#define	DecryptRound(K,R,id)								\
			t0	   = Fe32##id(x[K  ],0);					\
			t1	   = Fe32##id(x[K^1],3);					\
			DebugDump(x,"",(R)+1,0,0,1,0);					\
			x[K^2] = ROL (x[K^2],1);						\
			x[K^2]^= t0 +   t1 + sk[ROUND_SUBKEYS+2*(R)  ];	\
			x[K^3]^= t0 + 2*t1 + sk[ROUND_SUBKEYS+2*(R)+1];	\
			x[K^3] = ROR (x[K^3],1);						\

#define		Decrypt2(R,id)	{ DecryptRound(2,R+1,id); DecryptRound(0,R,id); }

		Decrypt2(14,_);
		Decrypt2(12,_);
		Decrypt2(10,_);
		Decrypt2( 8,_);
		Decrypt2( 6,_);
		Decrypt2( 4,_);
		Decrypt2( 2,_);
		Decrypt2( 0,_);

#ifdef DEBUG
		DebugDump(x,"",0,0,0,0,0);
#endif

		if (cipher->mode == MODE_ECB)
		{
#define	StoreBlockD(N)	((DWORD *)outBuffer)[N] = x[N] ^ sk[INPUT_WHITEN+N]
			StoreBlockD(0);	StoreBlockD(1);	StoreBlockD(2);	StoreBlockD(3);
#undef  StoreBlockD

#ifdef DEBUG
			DebugDump(outBuffer,"",-1,0,0,0,1);
#endif
			continue;
		}
		else
		{
#define	StoreBlockD(N)	x[N]   ^= sk[INPUT_WHITEN+N] ^ IV[N];	\
						IV[N]   = Bswap(((DWORD *)input)[N]);	\
						((DWORD *)outBuffer)[N] = Bswap(x[N]);
			StoreBlockD(0);	StoreBlockD(1);	StoreBlockD(2);	StoreBlockD(3);
#undef  StoreBlockD
			DebugDump(outBuffer,"",-1,0,0,0,1);
		}

	}

	if (mode == MODE_CBC)
		BlockCopy(cipher->iv32,IV);

	return inputLen;
}
