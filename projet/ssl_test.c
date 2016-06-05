#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/rand.h>
#include <openssl/des.h>
#include <openssl/evp.h>

//pour générer un sel
void generateChallenge(unsigned char *challenge,int chl_size){

    int r = 0;

    r = RAND_bytes(challenge,chl_size);

    if (!r) {
        printf("\nInternal error !\n");
        exit(EXIT_FAILURE);
    }

}

//fonction pour chiffrer/déchiffrer
int crypto(int mode)
    {
        char *key_data="Password";

        /* Allow enough space in output buffer for additional block */
        unsigned char inbuf[1024], outbuf[1024 + EVP_MAX_BLOCK_LENGTH],
                      key[32], iv[32], salt[8];

        int key_data_len = strlen(key_data), nrounds = 1, inlen, outlen;
        if (mode!=1)
        {
           FILE *in = fopen("text.txt","rb");
            FILE *out = fopen("text.e.txt","wb");

            generateChallenge(salt,8);
            fwrite(salt, 1, 8, out);

            //derivate key & iv from the supplied password
            EVP_BytesToKey(EVP_aes_256_cbc(), EVP_md5(), salt, (unsigned char*)key_data, key_data_len, nrounds, key, iv);

            EVP_CIPHER_CTX ctx;
            EVP_CIPHER_CTX_init(&ctx);
            EVP_CipherInit_ex(&ctx, EVP_aes_256_cbc(), NULL, key, iv,1);

            for(;;)
            {
             inlen = fread(inbuf, 1, 1024, in);
                if(inlen <= 0) break;
                if(!EVP_CipherUpdate(&ctx, outbuf, &outlen, inbuf, inlen))
                {
                    /* Error */
                    EVP_CIPHER_CTX_cleanup(&ctx);
                    return 0;
                }
                fwrite(outbuf, 1, outlen, out);
            }

            if(!EVP_CipherFinal_ex(&ctx, outbuf, &outlen))
            {
                /* Error */
                EVP_CIPHER_CTX_cleanup(&ctx);
                return 0;
            }

            fwrite(outbuf, 1, outlen, out);
            EVP_CIPHER_CTX_cleanup(&ctx);

            fclose(in);
            fclose(out); 
        }
        else
        {
            FILE *in2 = fopen("text.e.txt","rb");
            FILE *out2 = fopen("text.d.txt","wb");

            fread(salt, 1, 8, in2);

            //derivate key & iv from the supplied password
            EVP_BytesToKey(EVP_aes_256_cbc(), EVP_md5(), salt, (unsigned char*)key_data, key_data_len, nrounds, key, iv);

            EVP_CIPHER_CTX ctx2;
            EVP_CIPHER_CTX_init(&ctx2);
            EVP_CipherInit_ex(&ctx2, EVP_aes_256_cbc(), NULL, key, iv, 0);

            for(;;)
            {
                inlen = fread(inbuf, 1, 1024, in2);
                if(inlen <= 0) break;
                if(!EVP_CipherUpdate(&ctx2, outbuf, &outlen, inbuf, inlen))
                {
                    /* Error */
                    EVP_CIPHER_CTX_cleanup(&ctx2);
                    return 0;
                }
                fwrite(outbuf, 1, outlen, out2);
            }

            if(!EVP_CipherFinal_ex(&ctx2, outbuf, &outlen))
            {
                /* Error */
                EVP_CIPHER_CTX_cleanup(&ctx2);
                return 0;
            }

            fwrite(outbuf, 1, outlen, out2);
            EVP_CIPHER_CTX_cleanup(&ctx2);

            fclose(in2);
            fclose(out2);
        }

        return 1;
    }

int main(void)
{
    int mode;
    //char* password[50];
    printf("Entrez un chiffre : pour le cryptage => 0 ou  pour le decryptage => 1\n");
    scanf("%d",&mode);
    //printf("\nEntrez un password: \n");
    //scanf("%s",&password);
    crypto(mode);
    return 0;
}

