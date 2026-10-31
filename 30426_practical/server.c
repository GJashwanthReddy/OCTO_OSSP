#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <signal.h>
#include <errno.h>

#define SERVER_FIFO "server_fifo"
#define MAX_CLIENTS 10
#define BUFFER_SIZE 256

volatile sig_atomic_t server_running = 1;

/* Signal handler */
void signal_handler(int sig)
{
    if (sig == SIGINT) {
        server_running = 0;
    }
    else if (sig == SIGUSR1) {
        const char msg[] = "\n[SERVER] SIGUSR1 received\n";
        write(STDOUT_FILENO, msg, sizeof(msg) - 1);
    }
    else if (sig == SIGCHLD) {
        while (waitpid(-1, NULL, WNOHANG) > 0);
    }
}

/* Remove FIFOs */
void cleanup_fifos()
{
    unlink(SERVER_FIFO);

    for (int i = 1; i <= MAX_CLIENTS; i++) {

        char c2s[50];
        char s2c[50];

        sprintf(c2s, "client%d_to_server", i);
        sprintf(s2c, "server_to_client%d", i);

        unlink(c2s);
        unlink(s2c);
    }
}

/* Handler process for each client */
void client_handler(int client_id)
{
    char c2s[50];
    char s2c[50];
    char buffer[BUFFER_SIZE];

    sprintf(c2s, "client%d_to_server", client_id);
    sprintf(s2c, "server_to_client%d", client_id);

    int read_fd = open(c2s, O_RDONLY);

    if (read_fd == -1) {
        perror("open client FIFO");
        exit(EXIT_FAILURE);
    }

    int write_fd = open(s2c, O_WRONLY);

    if (write_fd == -1) {
        perror("open server FIFO");
        close(read_fd);
        exit(EXIT_FAILURE);
    }

    printf("[HANDLER %d] Connected to Client %d\n",
           getpid(), client_id);

    while (1) {

        memset(buffer, 0, sizeof(buffer));

        ssize_t n = read(read_fd, buffer, sizeof(buffer) - 1);

        if (n <= 0) {
            break;
        }

        buffer[n] = '\0';

        printf("[CLIENT %d] %s", client_id, buffer);

        /* Client termination message */
        if (strncmp(buffer, "exit", 4) == 0) {

            char response[] = "Server: Client disconnected\n";
            write(write_fd, response, strlen(response));

            break;
        }

        /* Prepare response */
        char response[BUFFER_SIZE];

	 snprintf(response,
         sizeof(response),
         "Server: Message received from Client %d: %.200s",
         client_id,
         buffer);

        write(write_fd, response, strlen(response));
    }

    close(read_fd);
    close(write_fd);

    printf("[HANDLER %d] Client %d disconnected\n",
           getpid(), client_id);

    exit(EXIT_SUCCESS);
}

int main()
{
    int server_fd;
    char buffer[BUFFER_SIZE];

    struct sigaction sa;

    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = signal_handler;

    sigemptyset(&sa.sa_mask);

    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGUSR1, &sa, NULL);
    sigaction(SIGCHLD, &sa, NULL);

    /* Remove old FIFOs */
    cleanup_fifos();

    /* Create main server FIFO */
    if (mkfifo(SERVER_FIFO, 0666) == -1) {

        if (errno != EEXIST) {
            perror("mkfifo");
            exit(EXIT_FAILURE);
        }
    }

    printf("=====================================\n");
    printf("        MULTI CLIENT SERVER\n");
    printf("=====================================\n");

    printf("[SERVER] Creating client FIFOs...\n");

    /* Create FIFOs for clients */
    for (int i = 1; i <= MAX_CLIENTS; i++) {

        char c2s[50];
        char s2c[50];

        sprintf(c2s, "client%d_to_server", i);
        sprintf(s2c, "server_to_client%d", i);

        mkfifo(c2s, 0666);
        mkfifo(s2c, 0666);
    }

    printf("[SERVER] FIFOs created.\n");
    printf("[SERVER] Waiting for clients...\n");

    /*
     * Open server FIFO.
     * O_RDWR prevents open() from blocking until a writer appears.
     */
    server_fd = open(SERVER_FIFO, O_RDWR);

    if (server_fd == -1) {
        perror("server_fifo");
        cleanup_fifos();
        exit(EXIT_FAILURE);
    }

    while (server_running) {

        memset(buffer, 0, sizeof(buffer));

        ssize_t n = read(server_fd, buffer, sizeof(buffer) - 1);

        if (n <= 0) {
            if (errno == EINTR)
                continue;

            break;
        }

        buffer[n] = '\0';

        /*
         * Client sends:
         * client_id
         */
        int client_id = atoi(buffer);

        if (client_id < 1 || client_id > MAX_CLIENTS) {
            printf("[SERVER] Invalid client ID: %d\n", client_id);
            continue;
        }

        printf("[SERVER] Client %d connected\n", client_id);

        pid_t pid = fork();

        if (pid == -1) {
            perror("fork");
            continue;
        }

        if (pid == 0) {

            /* Child process */

            close(server_fd);

            client_handler(client_id);

            exit(EXIT_SUCCESS);
        }

        /*
         * Parent continues accepting other clients.
         */
    }

    printf("\n[SERVER] Shutting down...\n");

    close(server_fd);

    cleanup_fifos();

    printf("[SERVER] Cleanup complete.\n");

    return 0;
}
