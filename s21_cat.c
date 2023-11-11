#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

struct s_avi_flags {
    char flag;              // флаг
    char *equivalent_flags; // эквивалентные простые флаги
};

void flags_parser(char *flags, int argc, char **argv, int *index);
void append_flags(char *flags, char flag);
int print_file(char *name, char *flags);
void print_symb(int c, int *prev, char *flags, int *index, bool *eline_printed);

int main(int argc, char **argv) {
    char flags[7] = "\0";
    int index_end_flags = 1;
    flags_parser(flags, argc, argv, &index_end_flags);

    // Your existing code starts here
    int line_number = 0; // Line number counter
    int prev_char = '\n'; // Initialize to '\n' to start with line numbering
    bool display_tabs_as_i = false;
    bool display_non_printable = false;
    bool squeeze_blank = false;

    // Process the remaining command line arguments
    for (int i = index_end_flags + 1; i < argc; i++) {
        if (strcmp(argv[i], "--") == 0)
            continue;
        print_file(argv[i], flags);
    }
    return 0;
}

void flags_parser(char *flags, int argc, char **argv, int *index) {
    for (int i = 1; i < argc; i++) {
        if (argv[i][0] != '-'  strcmp(argv[i], "--") == 0  strcmp(argv[i], "-") == 0) {
            break;
        } else {
            *index = i;
            for (size_t j = 1; j < strlen(argv[i]); j++) {
                append_flags(flags, argv[i][j]);
            }
        }
    }
}

void append_flags(char *flags, char flag) {
    int err_code = 1;
    struct s_avi_flags avi_flags[8] = {{'b', "b"}, {'E', "E"}, {'e', "Ev"},
                                       {'n', "n"}, {'s', "s"}, {'T', "T"},
                                       {'t', "Tv"}, {'v', "v"}};
    for (int i = 0; i < 8; i++) {
        if (avi_flags[i].flag == flag) {
            for (size_t j = 0; j < strlen(avi_flags[i].equivalent_flags); j++) {
                append_flags(flags, avi_flags[i].equivalent_flags[j]);
            }
            err_code = 0;
            break;
        }
    }
}

int print_file(char *name, char *flags) {
    int err_code = 0;
    FILE *f;
    // если имя файла "-" работаем с stdin
    if (strcmp("-", name) == 0)
        f = stdin;
    else
        f = fopen(name, "rt");

    if (f != NULL) {
        int index = 0;
        bool eline_printed = 0;
        int c = fgetc(f), prev = '\n';
        while (c != EOF) {
            print_symb(c, &prev, flags, &index, &eline_printed);
            c = fgetc(f);
        }
        if (f != stdin)
            fclose(f); // закрываем файл только если он не stdin
    } else {
        err_code = 1;
    }
    return err_code;
}

void print_symb(int c, int *prev, char *flags, int *index, bool *eline_printed) {
    // если s и это пустая строка и пустая строка уже была выведена, пропустим, сюда не зайдем
    if (!(strchr(flags, 's') != NULL && *prev == '\n' && c == '\n' && *eline_printed)) {

        if (*prev == '\n' && c == '\n')
            *eline_printed = 1;
        else
            *eline_printed = 0;

        // если ( (n без b) или (b и текущий символ не '\n') ) и пред символ '\n'
        if (((strchr(flags, 'n') != NULL && strchr(flags, 'b') == NULL) || (strchr(flags, 'b') != NULL && c != '\n')) && *prev == '\n') {
            *index += 1;
            printf("%6d\t", *index);
        }

        if (strchr(flags, 'E') != NULL && c == '\n')
            printf("$");
        if (strchr(flags, 'T') != NULL && c == '\t') {
            printf("^");
            c = '\t' + 64;
        }
        if (strchr(flags, 'v') != NULL && c >= 0 && c <= 31 && c != '\n' && c != '\t') {
            printf("^");
            c = c + 64;
        }
        fputc(c, stdout);
    }
    *prev = c;
}
