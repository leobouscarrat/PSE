#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

int main(int argc, char** argv)
{
char  password[120]="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz1234567890<>,?;.:/!§*µù$£¤¨+=})]à@ç^_`è|-[({'#é~&";
int max=101,i,alea;
char motDePasse[65];
srand(time(NULL));
for(i=0;i<64;i++)
{
alea=rand()%(max);
motDePasse[i]=password[alea];
}
motDePasse[65]='\0';
printf("%s\n",motDePasse);
 
return 0;
}
