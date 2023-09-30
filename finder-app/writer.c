#include <stdio.h>
#include <stdlib.h>
#include <syslog.h>

int main(int argc, char **argv){
    openlog("Assignment2",LOG_CONS, LOG_DEBUG);
    if(argc != 3){
        printf("Insufficient number of input arguments.\n");
        syslog(LOG_ERR, "Insufficient number of input arguments.");
        exit(1);
    }
    
    FILE *fp = fopen(argv[1], "w+");
    if(!fp){
        perror("failed to create the file.");
        syslog(LOG_ERR, "failed to create the file %s", argv[1]);
        exit(1);
    }
    fprintf(fp,"%s",argv[2]);
    syslog(LOG_INFO, "writing %s to %s ", argv[2], argv[1]);
    fclose(fp);   
    closelog();
 return 0;
}
