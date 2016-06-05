#include <stdio.h>
#include <stdlib.h>
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
int do_crypt (char *infile, char *outfile, int do_encrypt)
    {
 
        /* Allow enough space in output buffer for additional block */
        unsigned char inbuf[1024], outbuf[1024 + EVP_MAX_BLOCK_LENGTH],
                      key[32], iv[32], salt[8];
 
        char *key_data="monMonDePasse";
        int key_data_len = strlen(key_data), nrounds = 1, inlen, outlen;
 
        FILE *in = fopen(infile,"rb");
        FILE *out = fopen(outfile,"wb");
 
        if(do_encrypt){ //if encryption case
            generateChallenge(salt,8);
            fwrite(salt, 1, 8, out);
        }
        else{ //if decryption case
            fread(salt, 1, 8, in);
        }
 
        //derivate key & iv from the supplied password
        EVP_BytesToKey(EVP_aes_256_cbc(), EVP_md5(), salt, (unsigned char*)key_data, key_data_len, nrounds, key, iv);
 
        EVP_CIPHER_CTX ctx;
        EVP_CIPHER_CTX_init(&ctx);
        EVP_CipherInit_ex(&ctx, EVP_aes_256_cbc(), NULL, key, iv, do_encrypt);
 
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
 
        return 1;
    }
 
    int main(){
        do_crypt("test.txt","test.txt.e",1); //encryption
        do_crypt("test.txt.e","test.txt.d",0); //decryption
    }