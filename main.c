#include "defines.h"

int main(int argc, char** argv)
{
    if(argc < 2) {
        printf("Usage:\n");
        printf("    search [OPTION] <FILE>\n");
        printf("Options:\n");
        printf("    -out out model dir, must exists\n");
        printf("    -fl first length default 4\n");
        printf("    -mx max length default 30\n");
        exit(0);
    }
    model_config config;
    config.model_dir = "model";
    config.first_len = 4;
    config.max_item = 30;
    int i = 1;
    for (; i < argc; i++) {
        if(strcmp(argv[i], "-out") == 0 && i < argc - 1) {
            config.model_dir = argv[++i];
        }else if(strcmp(argv[i], "-fl") == 0 && i < argc - 1) {
            config.first_len = atoi(argv[++i]);
        }else if(strcmp(argv[i], "-mx") == 0 && i < argc - 1) {
            config.max_item = atoi(argv[++i]);
        }else{
            config.file_input = argv[i];
        }
    }
    config.rest_len = config.max_item - config.first_len;
    map_init(config.first_len);
    file_to_model(&config);
    printf("\nSUCCESS\n");
    map_destroy();
    return 0;
}
