#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include "mailbox.h"

void;

mailbox_t mailbox_open(int id);
int mailbox_unlink(int id);
int mailbox_close(mailbox_t box);
int mailbox_send(mailbox_t box, mail_t mail);
int mailbox_recv(mailbox_t box, mail_t mail);
int mailbox_check_empty(mailbox_t box);
int mailbox_check_full(mailbox_t box);

int main(int argc, char const *argv[]){
    
    int i;
    mailbox_t box0,box3,box9;
    box0=mailbox_open(0);
    box3=mailbox_open(3);
    box9=mailbox_open(9);

    mail_t firstmail;
    firstmail.from=3;
    firstmail.type=1;
    strcpy(firstmail.sstr,"Trump");
    mailbox_send(box0,firstmail);

    mail_t mymail;
    mymail.from=3;
    mymail.type=2;
    strcpy(mymail.sstr,"Trump");   
    printf("------------------------------------------------\n | Welcome to this chatroom                   |\n | Type ./cli3 or ./cli9 to join the chatroom |\n | Type LEAVE to leave the chatroom           |\n------------------------------------------------\n\n\n");

    while(1){
        char buf[600];
        fcntl(0, F_SETFL, fcntl(0, F_GETFL) | O_NONBLOCK);
        sleep(1);
        mailbox_recv(box3,mymail);

        if(fgets(buf, 600, stdin)){       
                strcpy(mymail.lstr,buf);
                mailbox_send(box0,mymail);
                if(strncmp(mymail.lstr,"LEAVE",5)==0)break;
                printf("\n");           
        }
    }

    mailbox_close(box3);
    mailbox_unlink(3);
    return 0;
}
