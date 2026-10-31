#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>
#include <errno.h>

#define SERVER_FIFO "server_fifo"
#define BUFFER_SIZE 256

volatile sig_atomic_t client_running = 1;

/* Signal handler */
void signal_handler(int sig)
{
    if (sig == SIGINT) {
        client_running = 0;
    }
    else if (sig == SIGUSR1) {
        const char msg[] = "\n[CLIENT] SIGUSR1 received\n";
        write(STDOUT_FILENO, msg, sizeof(msg) - 1);
    }
}

int main(int argc, char *argv[])
{
    if (argc != 2) {
        printf("Usage: %s <client_id>\n", argv[0]);
        printf("Example: %s 1\n", argv[0]);
        return 1;
    }

    int client_id = atoi(argv[1]);

    if (client_id < 1 || client_id > 10) {
        printf("Client ID must be between 1 and 10.\n");
        return 1;
    }

    char c2s[50];
    char s2c[50];

    sprintf(c2s, "client%d_to_server", client_id);
    sprintf(s2c, "server_to_client%d", client_id);

    /* Install signal handlers */
    struct sigaction sa;

    memset(&sa, 0, sizeof(sa));

    sa.sa_handler = signal_handler;
    sigemptyset(&sa.sa_mask);

    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGUSR1, &sa, NULL);

    printf("=====================================\n");
    printf("             CLIENT %d\n", client_id);
    printf("=====================================\n");

    /*
     * STEP 1:
     * Tell the server that this client wants to connect.
     *
     * This MUST happen before opening client_to_server FIFO,
     * otherwise both processes can wait for each other.
     */
    int server_fd = open(SERVER_FIFO, O_WRONLY);

    if (server_fd == -1) {
        perror("[CLIENT] open server_fifo");
        return 1;
    }

    char id_message[20];

    snprintf(id_message,
             sizeof(id_message),
             "%d",
             client_id);

    if (write(server_fd,
              id_message,
              strlen(id_message)) == -1) {

        perror("[CLIENT] write client ID");

        close(server_fd);

        return 1;
    }

    close(server_fd);

    /*
     * STEP 2:
     * Now the server has received the ID and created
     * the handler process.
     *
     * Open Client -> Server FIFO.
     */
    int write_fd = open(c2s, O_WRONLY);

    if (write_fd == -1) {
        perror("[CLIENT] open client_to_server FIFO");
        return 1;
    }

    /*
     * STEP 3:
     * Open Server -> Client FIFO.
     */
    int read_fd = open(s2c, O_RDONLY);

    if (read_fd == -1) {
        perror("[CLIENT] open server_to_client FIFO");

        close(write_fd);

        return 1;
    }

    printf("[CLIENT %d] Connected to server.\n", client_id);

    while (client_running) {

        char message[BUFFER_SIZE];
        char response[BUFFER_SIZE];

        printf("\nClient %d > ", client_id);
        fflush(stdout);

        if (fgets(message, sizeof(message), stdin) == NULL) {
            break;
        }

        /*
         * Send message to server.
         */
        if (write(write_fd,
                  message,
                  strlen(message)) == -1) {

            if (errno == EINTR)
                continue;

            perror("[CLIENT] write");
            break;
        }

        /*
         * If client typed exit,
         * receive final response and terminate.
         */
        if (strncmp(message, "exit", 4) == 0) {

            memset(response, 0, sizeof(response));

            ssize_t n = read(read_fd,
                             response,
                             sizeof(response) - 1);

            if (n > 0) {
                response[n] = '\0';
                printf("%s", response);
            }

            break;
        }

        /*
         * Receive server response.
         */
        memset(response, 0, sizeof(response));

        ssize_t n = read(read_fd,
                         response,
                         sizeof(response) - 1);

        if (n < 0) {

            if (errno == EINTR)
                continue;

            perror("[CLIENT] read");
            break;
        }

        if (n > 0) {

            response[n] = '\0';

            printf("%s", response);
        }
    }

    close(write_fd);
    close(read_fd);

    printf("\n[CLIENT %d] Disconnected.\n", client_id);

    return 0;
}
