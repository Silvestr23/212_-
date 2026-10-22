#include <unistd.h>
#include <stdlib.h>

int main(void) {
    char buffer[4096];
    ssize_t bytes;

    while (1) {
        bytes = read(STDIN_FILENO, buffer, sizeof(buffer));
        if (bytes == -1) {
            write(2, "Ошибка чтения в child1\n", 24);
            exit(-1);
        } else if (bytes == 0) {
            break;
        }

        for (ssize_t i = 0; i < bytes; ++i) {
            if (buffer[i] >= 'a' && buffer[i] <= 'z') {
                buffer[i] -= 32;
            }
        }

        if (write(STDOUT_FILENO, buffer, bytes) == -1) {
            write(2, "Ошибка записи в child1\n", 24);
            exit(-1);
        }
    }

    return 0;
}