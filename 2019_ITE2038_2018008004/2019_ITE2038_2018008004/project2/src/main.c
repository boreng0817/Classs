#include "bpt.h"

int main(int argc, char **argv) {

    char db_file[100];

    int input, do_print = 1;
    char instruction;
    char val[120];
    FILE *fp;

    usage_1();  
    usage_2();

    printf("Input table name\n-->");
    scanf("%s%c", db_file, &instruction);
    printf("Unique table id:%d\n", open_table(db_file));

    if (argc > 1) {
        fp = fopen(argv[1], "r");
        if (fp != NULL) {
            while (!feof(fp)) {
                fscanf(fp, "%d %s", &input, val);
                db_insert(input, val);
            }
        }
    }

    printf("> ");
    while (scanf("%c", &instruction) != EOF) {
        switch (instruction) {
        case 'd':
            scanf("%d", &input);
            db_delete(input);
            print_tree(do_print);
            break;
        case 'i':
            scanf("%d %s", &input, val);
            db_insert(input, val);
            print_tree(do_print);
            break;
        case 'f':
            scanf("%d", &input);
            if(!db_find(input, val))
                printf("Find success <%d:%s>\n", input, val);
            else
                printf("Find fail\n");
            break;
        case 'q':
            while (getchar() != (int)'\n');
            close_table();
            return 0;
            break;
        case 't':
            print_tree(1);
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

