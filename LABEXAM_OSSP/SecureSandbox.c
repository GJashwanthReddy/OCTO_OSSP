#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
int main()
{
    int status;
    pid_t pid = fork();
    if(pid == 0)
    {
       execlp("ls", "ls", "-la", (char *)NULL);
       perror("exec failed");
       exit(1);
    }
    else if(pid > 0)
    {
       wait(&status);
       if(WIFEXITED(status))
         printf("Child exit code: %d\n", WEXITSTATUS(status));
       printf("Parent process completed\n");
    }
    else
      perror("fork failed");
   return 0;
}

