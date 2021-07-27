#include "buf.h"

int main(int argc, char **argv) {

    char db_file[100];

    int input, do_print = 1;
    int unique_id, buf_size;
    char instruction;
    char table_id;
    char val[120];
    FILE *fp;
    


    usage_1();  
    usage_2();

    printf("Input buffer size\n-->");
    scanf("%d", &buf_size);
    init_db(buf_size);

    printf("Input table name\n-->");
    scanf("%s%c", db_file, &instruction);
    printf("Unique table id:%d\n", open_table(db_file));

    if (argc > 1) {
        fp = fopen(argv[1], "r");
        if (fp != NULL) {
            while (!feof(fp)) {
                fscanf(fp, "%d %s", &input, val);
                db_insert(1, input, val);
                //db_delete(1, input);
            }
        }
    }

    printf("> ");
    while (scanf("%c", &instruction) != EOF) {
        switch (instruction) {
        case 'd':
            scanf("%d %d",&unique_id, &input);
            db_delete(unique_id, input);
            print_tree(unique_id, do_print);
            break;
        case 'i':
            scanf("%d %d %s", &unique_id, &input, val);
            db_insert(unique_id, input, val);
            print_tree(unique_id, do_print);
            break;
        case 'f':
            scanf("%d %d", &unique_id, &input);
            if(!db_find(unique_id, input, val))
                printf("Find success <%d:%s>\n", input, val);
            else
                printf("Find fail\n");
            break;
        case 'q':
            while (getchar() != (int)'\n');
            shutdown_db();
            return 0;
            break;
        case 't':
            scanf("%d", &unique_id);
            print_tree(unique_id,1);
            break;
        case 'm':
            printf("toggle delayed merge : %d.\n", 
                    toggle_delayed_merge()); 
            break;
        case 'p':
            do_print = !do_print;
            printf("print tree when insert or delete : %d\n",
                    do_print);
            break;
        case 'o':
            scanf("%s", db_file);
            unique_id = open_table(db_file);
            if(unique_id < 0) 
                printf("Open_table(%s) failed.\n", db_file);
            else
                printf("File name [%s] opened. id : %d\n", db_file, unique_id);
            break;
        case 'c':
            scanf("%d", &unique_id);
            if(!close_table(unique_id)) 
                printf("File close [%d] successed.\n", unique_id);
            else
                printf("File close [%d] failed.\n", unique_id);
            break;
        default:
            usage_2();
            break;
        }
        while (getchar() != (int)'\n');
        printf("> ");
    }
    printf("\n");

    return 0;
}

