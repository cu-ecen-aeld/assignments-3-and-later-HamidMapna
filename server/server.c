#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/time.h>
#include <syslog.h>
#include <signal.h>

#define MAX_BUFFER_LEN 50000
#define port 9000

char *file_path = "/var/tmp/aesdsocketdata";
char *client_addr = NULL;
char log[100] = {'\0'};
int _socket = -1, client;

send_syslog(char *log){
    openlog("local server: ", LOG_CONS, LOG_DEBUG);
    syslog(LOG_INFO, log); 
     closelog();
}

void create_socket(){
    _socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(_socket < 0){
        fprintf(stderr, "failed to create server socket. errno: %d, erroe= %s\n", errno, strerror(errno));
        return -1;
    }
}
    
    void setopt_socket(){
    int opt_val = 1;
    int sockopt_result = setsockopt(_socket, SOL_SOCKET, SO_REUSEADDR, &opt_val, sizeof(opt_val));
    if(sockopt_result < 0){
        fprintf(stderr, "failed to set option reuse address. errno: %d, error:%s\n", errno, strerror(errno));
        return -1;
    }
}

void bind_socket(){
    struct sockaddr_in service;
    memset(&service, 0, sizeof(service));
    service.sin_family = PF_INET;
    service.sin_port = htons(port);
    service.sin_addr.s_addr = inet_addr("127.0.0.1");
    int bind_result = bind(_socket, (struct sock_addr*)&service, sizeof(service));
    if(bind_result < 0){
        fprintf(stderr, "failed to bind socket. errno: %d, error:%s\n", errno, strerror(errno));
        return -1;
    }
}

void accept_socket(){
     struct sockaddr_in incomm_service;     
     memset(&incomm_service, 0, sizeof(struct sockaddr_in));
     int sz = sizeof(struct sockaddr_in);
     if((client = accept(_socket,(struct sockaddr*)&incomm_service, (socklen_t*)&sz)) < 0){
        fprintf(stderr, "failed to accept socket on receiving connection. errno: %d, error:%s\n", errno, strerror(errno));
        return -1;
     }
     char *client_addr = inet_ntoa(incomm_service.sin_addr);
     sprintf(log,"Accepted connection from %s", client_addr);
     send_syslog(log);
}

void create_and_establish_socket(){
    create_socket();
    setopt_socket();
    bind_socket(9000);
    listen(_socket, 1);
    accept_socket();
}

void append_tempfile(char *line){
    FILE *fp = fopen(file_path, "a+");
    if(fp == NULL){
        fprintf(stderr, "failed to log into /var/tmp directory. error: %s\n", strerror(errno));
        return;
    }
    fwrite(line, strlen(line), sizeof(char), fp);
    fclose(fp);
}

void rem_file(){
    if (remove(file_path) != 0)
        printf("Unable to delete the file.\n");
}

void sendback_client(int _socket){
    FILE *fp = fopen(file_path, "r");
    char buffer[MAX_BUFFER_LEN] = {'\0'};
    if(fp == NULL){
        fprintf(stderr, "failed to open /var/tmp/ log file to send content back to client. error: %s\n", strerror(errno));
        return;
    }
    fseek(fp, 0, SEEK_END);
    int length = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    int flen = fread(buffer, length, sizeof(char), fp);
    if(flen <=0){
        fprintf(stderr, "file %s is empty.\n", file_path);
        return;
    }
    int slen = send(_socket, buffer, strlen(buffer), 0);
    if(slen <= 0){
        fprintf(stderr, "failed to send data back to client.len:%d, error:%s\n", slen, strerror(errno));
        return;
    }
    fflush(stdout);
    fclose(fp);
}

void communicate(int _socket){
    char buf[MAX_BUFFER_LEN] = {'\0'};
    char *buf_temp = buf;
    int recv_len = 0;
   while(1)
    {        
        int len = recv(_socket, buf+recv_len, 1, 0);
        if(len < 0){
            fprintf(stderr, "failed to receive data from client. error: %s\n", strerror(errno));
            return EXIT_FAILURE;
        }
        if(len == 0){
            fprintf(stderr, "socket is empty. stop receiving data from client.\n");
            sprintf(log, "Closed connection from %s", client_addr);
            send_syslog(log);
            break;
        }
        if(buf[recv_len] == '\n'){
            append_tempfile(buf_temp);
            sendback_client(_socket);
            *buf = buf_temp;
            recv_len = 0;
            memset(buf, '\0', MAX_BUFFER_LEN);
        }
        else
            recv_len += len;        
    }   
 }

 void signal_handler(int signal){
    printf("signal number:%d\n", signal);
    close(client);
    close(_socket);
    rem_file();
    send_syslog("Caught signal, exiting");
    exit(1);
 }

 void register_signals(){
    struct sigaction action;
    memset(&action, 0, sizeof(action));
    action.sa_handler = signal_handler;
    action.sa_flags = 0;
    sigaction(SIGTERM, &action, 0);
    sigaction(SIGINT, &action, 0);
 }   
 
 int main(int argc, char **argv){
    register_signals();
    rem_file();
    while(1){
        create_and_establish_socket();
        communicate(client);
        close(client);
        close(_socket);
    }
 }
 



