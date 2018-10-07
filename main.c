#include "defines.h"

int main(int argc, char** argv)
{
    if(argc != 3) {
        printf("usage: (input) (model_dir).");
        exit(0);
    }
    map_init();
    file_to_model(argv[1], argv[2]);
    printf("\nSUCCESS\n");
    map_destroy();
    return 0;
}
