#include <stdio.h>

int main(int argc, char* argv[])
{
    if (argc != 3)
    {
        printf("PROGRAM2::MAIN -> Argument number is off\n");
        return -1;
    }

    printf("Name: %s\tSurname: %s\n", argv[1], argv[2]);
    return 0;
}