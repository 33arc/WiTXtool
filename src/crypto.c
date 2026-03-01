#include "crypto.h"
#include <windows.h>
#include <wincrypt.h>
#include <string.h>

// standard RC4 state structure
typedef struct {
    unsigned char state[256];
    int x, y;
} RC4_State;

// key scheduling algorithm (KSA)
void rc4_setup(RC4_State *s, unsigned char *key,int keyLen) {
    int i, j = 0;
    unsigned char tmp;
    for (i = 0; i < 256; i++) s->state[i] = i;
    s->x = 0; s->y = 0;
    for (i = 0; i < 256; i++) {
        j = (j + s->state[i] + key[i % keyLen]) % 256;
        tmp = s->state[i];
        s->state[i] = s->state[j];
        s->state[j] = tmp;
    }
}

// pseudo-random generation algorithm (PRGA)
void rc4_crypt(RC4_State *s, unsigned char *data, size_t dataLen) {
    int x = s->x, y = s->y;
    unsigned char tmp;
    for (int i = 0; i < dataLen; i++) {
        x = (x + 1) % 256;
        y = (y + s->state[x]) % 256;
        tmp = s->state[x];
        s->state[x] = s->state[y];
        s->state[y] = tmp;
        data[i] ^= s->state[(s->state[x] + s->state[y]) % 256];
    }
    s->x = x; s->y = y;
}

int rc4_operation(unsigned char *buffer, size_t size) {
    // the game uses an RC4 stream cipher. the 128-bit session key is generated
    // by taking the MD5 hash of the password.
    const char *password = "W17VVI$G0ODG4M3";
    // using windows native cryptography to calculate md5
    HCRYPTPROV hProv = 0;
    HCRYPTHASH hHash = 0;
    BYTE key[16];
    DWORD cbHash = 16;

    if(!CryptAcquireContext(&hProv, NULL, NULL, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT)) return 1;
    if(!CryptCreateHash(hProv, CALG_MD5, 0, 0, &hHash)) { CryptReleaseContext(hProv, 0); return 1; }
    
    if(!CryptHashData(hHash, (BYTE*)password, (DWORD)strlen(password), 0)) { 
        CryptDestroyHash(hHash); CryptReleaseContext(hProv, 0); return 1; 
    }
    
    if(!CryptGetHashParam(hHash, HP_HASHVAL, key, &cbHash, 0)) {
        CryptDestroyHash(hHash); CryptReleaseContext(hProv, 0); return 1;
    }

    // initialize rc4
    RC4_State s;
    rc4_setup(&s, key, 16);

    // transform (symmetric operation)
    rc4_crypt(&s, buffer, size);

    CryptDestroyHash(hHash);
    CryptReleaseContext(hProv, 0);
    return 0;
}
