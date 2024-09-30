#include <stddef.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <utils.h>

// forks, execs, then waits
void run_program(char *program_file, char *argv[]) {
    pid_t pid = fork();
    if (pid == 0) {
        // hey... I'm not the original! I'm a child???
        execvp(program_file, argv);
        exit(127);
    } else {
        // I'm the original, suck it! (and wait)
        int status;
        waitpid(pid, &status, 0);

        if (WIFSIGNALED(status)) {
            printf("Program terminated by signal %i\n", WTERMSIG(status));
        } else if (WIFEXITED(status) && (WEXITSTATUS(status) == 127)) {
            printf("Application \"%s\" does not exist or could not be found.\n", program_file);
        }
    }
}

void run_line(char *line_o) {
    if (line_o[0] == '#') return;
    char **argv = calloc(0, sizeof(char*));
    bool in_quotes = false;
    size_t line_len = strlen(line_o);
    char *line = (char*) malloc(line_len + 1);
    memcpy(line, line_o, line_len + 1);
    size_t sym_start = (size_t) line;
    size_t num_args = 0;
    for (size_t c = 0; c <= line_len; c++) {
        if (line[c] == '"') {
            if (!in_quotes) sym_start = (size_t) &line[c + 1];
            in_quotes = !in_quotes;
            if (in_quotes)
                continue;
        }
        if (line[c] == ' ' || line[c] == 0 || line[c] == 10 || line[c] == '"') {
            if (in_quotes && line[c] != '"') continue;
            line[c] = 0;
            num_args++;
            argv = realloc(argv, num_args * sizeof(char*));
            argv[num_args - 1] = (char*) sym_start;
            sym_start = (size_t) &line[c + 1];
        }
    }
    argv = realloc(argv, (num_args + 1) * sizeof(char*));
    argv[num_args - 1] = NULL;
    if (memcmp("cd", line_o, 2) == 0) {
        if (argv[1] == NULL) return;
        int cd_status = chdir(argv[1]);
        if (cd_status != 0) printf("Directory \"%s\" does not exist or isn't a directory.\n", argv[1]);
        return;
    }
    run_program(argv[0], argv);
}

void shell_mode() {
    static char input_buffer[100];
    char current_dir_buffer[100];
    while (true) {
        getcwd(current_dir_buffer, 100);
        printf(WHT "[%s] " BGRN "$ " WHT, current_dir_buffer);
        fgets(input_buffer, 100, stdin);
        run_line(input_buffer);
    }
}

bool str_contains(char *str, char ch) {
    size_t len = strlen(str);
    for (int c = 0; c < len; c++)
        if (str[c] == ch) return true;
    return false;
}

void file_mode(char *filename) {
    FILE *f;
    char *this_line = NULL;
    size_t len = 0;
    ssize_t read;

    f = fopen(filename, "r");
    if (f == NULL) {
        printf("Couldn't open \"%s\".\n", filename);
        exit(1);
    }

    while ((read = getline(&this_line, &len, f)) != -1) {
        if (read == 1) continue;
        if (!str_contains(this_line, 10)) {
            this_line[read    ] = 10;
            this_line[read + 1] = 0;
        }
        run_line(this_line);
    }

    fclose(f);
    if (this_line) free(this_line);
}

int main(int argc, char **argv) {
    if (argc == 1)
        shell_mode();
    else if (argc == 2)
        file_mode(argv[1]);
    else
        printf("Too many arguments.\n");
}