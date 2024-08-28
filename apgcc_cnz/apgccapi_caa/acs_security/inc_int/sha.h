#include <limits.h>

void shaInit(void *priv);
void shaUpdate(void *priv, void const *bufIn, size_t len);
void const *shaFinal(void *priv);

#define SHA_BLOCKBYTES	64
#define SHA_BLOCKWORDS	16

#define SHA_HASHBYTES	20
#define SHA_HASHWORDS	5

typedef struct SHAContext
{
	unsigned int key[SHA_BLOCKWORDS];
	unsigned int iv[SHA_HASHWORDS];
	unsigned int bytesHi, bytesLo;
} SHAContext;
