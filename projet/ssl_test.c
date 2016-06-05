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
int crypto(int mode, char* password)
    {
        char *key_data=password;

        /* Allow enough space in output buffer for additional block */
        unsigned char inbuf[1024], outbuf[1024 + EVP_MAX_BLOCK_LENGTH],
                      key[32], iv[32], salt[16];

        int key_data_len = strlen(key_data), nrounds = 14, inlen, outlen;
        if (mode!=1)
        {
           FILE *in = fopen("text.txt","rb");
            FILE *out = fopen("text.e.txt","wb");

            generateChallenge(salt,16);
            fwrite(salt, 1, 16, out);

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

            fread(salt, 1, 16, in2);

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
void generateMdp(char* motDePasse)
{
    char password[120]="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz1234567890<>,?;.:/!§*µù$£¤¨+=})]à@ç^_`è|-[({'#é~&";
    int max=101,i,alea;
    srand(time(NULL));
    for(i=0;i<32;i++)
    {       
        alea=rand()%(max);
        motDePasse[i]=password[alea];
    }   
    motDePasse[33]='\0';
    printf("le mdp est %s\n",motDePasse);
}

int main(void)
{
    //int mode;
    printf("Entrez un chiffre : pour le cryptage => 0 ou  pour le decryptage => 1\n");
    //scanf("%d",&mode);
    //getchar();
    char motDePasse[33];
    //if (mode == 0)
        generateMdp(motDePasse);
    //else
        //motDePasse
    printf("\n Programme en cours ... \n");
    crypto(0,motDePasse);
    crypto(1,motDePasse);
    printf("\n Programme fini... \n");
    return 0;
}

