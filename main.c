#include "defines.h"

int main(int argc, char** argv)
{
    if(argc != 3) {
        printf("usage: (input) (model_dir).");
        exit(0);
    }
    map_init(FIRST_LEN);
    file_to_model(argv[1], argv[2]);
    //file_to_model("company_name", "model");
    printf("\nSUCCESS\n");
    map_destroy();
    /*init("model", 1);
    match_result *p, * result = cut("北京奇格翱翔商贸有限公司", false);
    while(result != NULL)
    {
        p = result->next;
        printf("%s", result->match_str);
        free(result->match_str);
        free(result);
        result = p;
    }
    destroy();*/
    return 0;
}
