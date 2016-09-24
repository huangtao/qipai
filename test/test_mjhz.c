#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "mjhz.h"

void test_mjhz();

int main(int argc, char** argv)
{
    srand((unsigned int)time(NULL));

    test_mjhz();

    return 0;
}

void test_mjhz()
{
    int i,n;
    mjhz_t mj;

    printf("=== start testing... ===\n");

    mjhz_init(&mj, 0, 4);
    n = 0;
    for (i = 0; i < MJHZ_DECK_PAIS; i++) {
        printf("%s ", mjpai_string(&mj.deck[i]));
        n++;
        if (n >= 10) {
            printf("\n");
            n = 0;
        }
    }
}
