#include <unistd.h>
#include <cstdio>

const int NUM_SECONDS = 1;

int main()
{
    int i;
    int count = 1;
    for(;;)
    {
        // delay for NUM_SECONDS seconds
        for(i = 0 ; i < NUM_SECONDS ; i++) { usleep(1000 * 1000); }
        printf("stdout %d\n", STDOUT_FILENO);
        printf("stderr %d\n", STDERR_FILENO);
        printf("Second %d\n", count++);
    }
    return 0;
}
