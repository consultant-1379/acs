#define	ROL(x,n) (((x) << ((n) & 0x1F)) | ((x) >> (32-((n) & 0x1F))))
#define	ROR(x,n) (((x) >> ((n) & 0x1F)) | ((x) << (32-((n) & 0x1F))))

#include	<stdlib.h>

#define		Bswap(x)			(x)		/* Not really needed in this environment */
#define		ADDR_XOR			0

#define	_b(x,N)	(((BYTE *)&x)[((N) & 3) ^ ADDR_XOR])

#define		b0(x)			_b(x,0)
#define		b1(x)			_b(x,1)
#define		b2(x)			_b(x,2)
#define		b3(x)			_b(x,3)

