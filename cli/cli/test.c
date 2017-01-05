#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>


int main(int argc, char **argv)
{
    umOpen();
    umRestore("/vispace/webs/umconfig.txt");
    char	*aaaaa=		umGetFirstUser();
    printf("user %s\n",aaaaa);
}
