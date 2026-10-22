#include <fcntl.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

int main(void) {
    int pipe1[2];
    int pipe2[2];
    int pipe3[2];

    if (pipe(pipe1) == -1 || pipe(pipe2) == -1 || pipe(pipe3) == -1) {
        write(2, "Ошибка создания pipe\n", 20);
        exit(-1);
    }

    pid_t child1 = fork();
    if (child1 == -1) {
        write(2, "Ошибка fork (child1)\n", 21);
        exit(-1);
    } else if (child1 == 0) {
        if (dup2(pipe1[0], STDIN_FILENO) == -1 ||
            dup2(pipe2[1], STDOUT_FILENO) == -1) {
            write(2, "Ошибка dup2 в child1\n", 21);
            exit(-1);
        }
        close(pipe1[0]); close(pipe1[1]);
        close(pipe2[0]); close(pipe2[1]);
        close(pipe3[0]); close(pipe3[1]);

        execlp("./child1", "child1", NULL);
        write(2, "Ошибка запуска child1\n", 22);
        exit(-1);
    }

    pid_t child2 = fork();
    if (child2 == -1) {
        write(2, "Ошибка fork (child2)\n", 21);
        exit(-1);
    } else if (child2 == 0) {
        if (dup2(pipe2[0], STDIN_FILENO) == -1 ||
            dup2(pipe3[1], STDOUT_FILENO) == -1) {
            write(2, "Ошибка dup2 в child2\n", 21);
            exit(-1);
        }
        close(pipe1[0]); close(pipe1[1]);
        close(pipe2[0]); close(pipe2[1]);
        close(pipe3[0]); close(pipe3[1]);

        execlp("./child2", "child2", NULL);
        write(2, "Ошибка запуска child2\n", 22);
        exit(-1);
    }

    close(pipe1[0]);
    close(pipe2[0]);
    close(pipe2[1]);
    close(pipe3[1]);

    char buffer[2048];
    ssize_t bytes;
    while (1) {
        bytes = read(STDIN_FILENO, buffer, sizeof(buffer));
        if (bytes == -1) {
            write(2, "Ошибка чтения из stdin\n", 23);
            exit(-1);
        }
        if (bytes == 0) {
            break;
        }
        if (write(pipe1[1], buffer, bytes) == -1) {
            write(2, "Ошибка записи в pipe1\n", 22);
            exit(-1);
        }
    }
    close(pipe1[1]);

    while (1) {
        bytes = read(pipe3[0], buffer, sizeof(buffer));
        if (bytes == -1) {
            write(2, "Ошибка чтения из pipe3\n", 23);
            exit(-1);
        }
        if (bytes == 0) {
            break;
        }
        if (write(STDOUT_FILENO, buffer, bytes) == -1) {
            write(2, "Ошибка записи в stdout\n", 23);
            exit(-1);
        }
    }
    close(pipe3[0]);

    waitpid(child1, NULL, 0);
    waitpid(child2, NULL, 0);

    return 0;
}