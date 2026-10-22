#include <unistd.h>
#include <stdlib.h>

int main(void) {
    char buffer[4096];
    char out[4096];
    ssize_t bytes;
    int prev_space = 0;

    while (1) {
        bytes = read(STDIN_FILENO, buffer, sizeof(buffer));
        if (bytes == -1) {
            write(2, "Ошибка чтения в child2\n", 24);
            exit(-1);
        } else if (bytes == 0) {
            break;
        }

        ssize_t k = 0;
        for (ssize_t i = 0; i < bytes; ++i) {
            if (buffer[i] == ' ') {
                if (!prev_space) {
                    out[k++] = ' ';
                    prev_space = 1;
                }
            } else {
                out[k++] = buffer[i];
                prev_space = 0;
            }
        }

        if (k > 0 && write(STDOUT_FILENO, out, k) == -1) {
            write(2, "Ошибка записи в child2\n", 24);
            exit(-1);
        }
    }

    return 0;
}