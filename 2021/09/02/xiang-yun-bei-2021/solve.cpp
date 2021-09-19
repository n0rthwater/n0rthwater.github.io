#include <cstdio>
#include <iostream>
#include <cstdlib>

unsigned char rol(unsigned char num, unsigned bit) {
  return (num >> (8-bit)) | (num << bit);
}

void gen_rand(unsigned char *outbuf) {
  srand(1);
  for (int i = 0;i < 32;i++) {
    outbuf[i] = rand() % 6;
  }
}


void accum_xor(unsigned char *buffer, uint32_t length) {
    for (uint32_t i = 1;i < length;i++) {
      buffer[i] ^= buffer[i-1];
    }
}


void calc_key_0(unsigned char *inkey,unsigned char *outkey) {
    for (int i = 0;i < 32;i++) {
      outkey[i] = inkey[i]+16;
    }
}

void calc_key_1(unsigned char *inkey, unsigned char *outkey)
{
  for (int i = 0; i < 32; i++)
  {
    outkey[i] = inkey[i] + 0xB0;
  }
}

void calc_key_2(unsigned char *inkey, unsigned char *outkey)
{
  for (int i = 0; i < 32; i++)
  {
    outkey[i] = inkey[i];
  }
}

void calc_key_3(unsigned char *inkey, unsigned char *outkey)
{
  for (int i = 0; i < 32; i++)
  {
    outkey[i] = inkey[i] + 0xB0;
  }
}

void calc_key_4(unsigned char *inkey, unsigned char *outkey)
{
  for(int i=0;i<32;i++)
    outkey[i] = inkey[i];
}

void calc_key_5(unsigned char *inkey, unsigned char *outkey, int round)
{
  for (int i = 0; i < 32; i++)
  {
    if ((unsigned char)(inkey[i] - 0x21) > 0x2E)
    {
      if ((unsigned char)(inkey[i] - 0x51) > 0x2E)
      {
        if (inkey[i] > 0x80)
          outkey[i] = inkey[i] - 0x30;
        else
        {
          outkey[i] = inkey[i];
        }
        
      }
      else {
        outkey[i] = inkey[i] - 0x30;
      }
    }
    else
    {
      outkey[i] = (unsigned char)(inkey[i] - 0x50);
    }
  }
}

void rev_enc_0(uint8_t *cipher,unsigned char *key) {
    for (int i = 0;i < 32;i++) {
      cipher[i] ^= key[i]; 
    }
}

void rev_enc_1(uint8_t *cipher, unsigned char *key)
{
  unsigned char xor_key[32];
  for (int i = 0; i < 32; i++)
  {
    xor_key[i] = rol(key[i],4);
  }

  for (int i = 0; i < 32; i++)
  {
    cipher[i] ^= xor_key[i];
  }
}

void rev_enc_2(uint8_t *cipher, unsigned char *key)
{
  rev_enc_0(cipher, key);
}

void rev_enc_3(uint8_t *cipher, unsigned char *key)
{
  for (int i = 0; i < 32; i+=2)
  {
    cipher[i] ^= key[i]<< 4;

    cipher[i+1] ^= key[i] >> 4;
  }
}

void rev_enc_4(uint8_t *cipher, unsigned char *key)
{
  rev_enc_0(cipher, key);
}

void rev_enc_5(uint8_t *cipher, unsigned char *inkey, unsigned char *outkey)
{
  for (int i = 0; i < 32; i++)
  {

    if ((unsigned char)(inkey[i] - 0x21) > 0x2E)
    {
      if ((unsigned char)(inkey[i] - 0x51) > 0x2E)
      {
        if (inkey[i] > 0x80)
          cipher[i] += outkey[i];
      }
      else
      {
        cipher[i] ^= (outkey[i] >> 4);
      }
    }
    else
    {
      cipher[i] -= outkey[i];
        }
  }
}

void gen_roundkey(unsigned char *rand_serial, unsigned char *init_key, unsigned char *roundkey, size_t len=32) {
    memcpy(roundkey,init_key,32);
    for(int i = 0;i < 32;i++) {
      switch (rand_serial[i])
      {
      case 0:
        calc_key_0(&roundkey[len * i], &roundkey[len * (i + 1)]);
        break;
      case 1:
        calc_key_1(&roundkey[len * i], &roundkey[len * (i + 1)]);
        break;
      case 2:
        calc_key_2(&roundkey[len * i], &roundkey[len * (i + 1)]);
        break;
      case 3:
        calc_key_3(&roundkey[len * i], &roundkey[len * (i + 1)]);
        break;
      case 4:
        calc_key_4(&roundkey[len * i], &roundkey[len * (i + 1)]);
        break;
      case 5:
        calc_key_5(&roundkey[len * i], &roundkey[len * (i + 1)],i);
        break;
      default:
        break;
      }
    }
}


void decrypt(uint8_t *cipher,unsigned char *roundkey,unsigned char *rand_serial)
{
  for (int i = 32;i > 0;i--)
  {
    switch (rand_serial[i-1])
    {
    case 0:
      rev_enc_0(cipher, &roundkey[32 * i]); break;
    case 1:
      rev_enc_1(cipher, &roundkey[32 * i]); break;
    case 2:
      rev_enc_2(cipher, &roundkey[32 * i]); break;
    case 3:
      rev_enc_3(cipher, &roundkey[32 * i]); break;
    case 4:
      rev_enc_4(cipher, &roundkey[32 * i]); break;
    case 5:
      rev_enc_5(cipher, &roundkey[32 * (i-1)], &roundkey[32 * i]); break;

    default:
      break;
    }
    for (int i = 0; i < 32; i++)
    {
      printf("%2x ", cipher[i]);
    }
    printf("\n");
  }
}


void gen_enc(uint32_t *text,uint8_t *enc){
  for (int i=0;i<32;i++) {
    enc[i] = (text[i/4] >> (8*(i%4))) & 0xff;
  }
}

int main() {
  uint32_t final_text[] = {0x1B38C557, 0x2F34A83A, 0x0E4C69739, 0x0EE8F2F04, 0x6780515E, 0x486FC924, 0x0C7BD7F5B, 0x0EBC2C2B0};
  uint8_t enc[32];
  unsigned char init_key[] = {0xa5, 0xcf, 0xcd, 0xd6, 0xc5, 0xc3, 0xb1, 0xc5, 0xd2, 0xd9, 0xd7, 0xc7, 0xd6, 0xcd, 0xd4, 0xd8, 0xc3, 0xbb, 0xcd, 0xd8, 0xcc, 0xc3, 0xb0, 0xc5, 0xd8, 0xc9, 0xdc, 0x0, 0x0, 0x0, 0x0, 0x0 } ;
  unsigned char roundkey[33][32];
  unsigned char rand_serial[32];



  gen_enc(final_text,enc);

  gen_rand(rand_serial);
  accum_xor(init_key,32);
  gen_roundkey(rand_serial,init_key,*roundkey);

  for (int i = 0; i < 32; i++)
  {
    printf("%x\n", rand_serial[i]);
  }


  decrypt(enc,*roundkey,rand_serial);
  for (int i = 0; i < 32; i++)
  {
      printf("%c", enc[i]);
  }

  return 0;
}