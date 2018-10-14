#include "defines.h"

int main(int argc, char** argv)
{
    /*if(argc != 3) {
        printf("usage: (input) (model_dir).");
        exit(0);
    }
    map_init(FIRST_LEN);
    file_to_model(argv[1], argv[2]);
    printf("\nSUCCESS\n");
    map_destroy();*/
    FILE *fp = fopen("/home/fengxw/files/out", "rb");
    init("/home/fengxw/files/model", 1);
    //char buf[1024 * 128];
    //int len = 1024 * 128;
    //while(!feof(fp)) {
        //memset(buf, 0, len);
        //fgets(buf, len, fp);
        match_result *p, * result = cut("111111111111", false);
        while(result != NULL)
        {
            p = result->next;
            printf("%s", result->match_str);
            free(result->match_str);
            free(result);
            result = p;
        }
    //}
    fclose(fp);
    destroy();
    return 0;
}
