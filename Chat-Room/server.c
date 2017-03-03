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

static char usertable[10][20]={0};
static int usernum=0;
void;

mailbox_t mailbox_open(int id);
int mailbox_unlink(int id);
int mailbox_close(mailbox_t box);
int mailbox_send(mailbox_t box, mail_t mail);
int mailbox_recv_send(mailbox_t box, mail_t mail);
int mailbox_check_empty(mailbox_t box);
int mailbox_check_full(mailbox_t box);

void display(){
    int i;
    printf("------------------\n| ID | Username\n------------------\n");
    for(i=0;i<10;i++){
        if(strcmp(usertable[i],"")){
            printf("| %d  |  %s\n------------------\n",i,usertable[i]);
        }
    }
    printf("\n");
}
int mailbox_recv_send(mailbox_t box, mail_t mail){
    
    if(mailbox_check_empty(box))return 0;

    char*ptr;
    int len;
    mailbox_t box3=mailbox_open(3);
    mailbox_t box9=mailbox_open(9);
    
    sscanf(box.m_base,"%d %d %s",&mail.from,&mail.type,&mail.sstr);
    len=strlen(mail.sstr);
    ptr=box.m_base+5+len;
    strcpy(mail.lstr,ptr);

    if(strncmp(mail.lstr,"LEAVE",5)==0){
        mail.type=3;
    }

    if(mail.type==1){
        printf("%s join the chatroom\n",mail.sstr);
        if(mail.from==3){
            mailbox_send(box9,mail);
        }else if(mail.from==9){
            mailbox_send(box3,mail);
        }
        strcpy(usertable[mail.from],mail.sstr);
        display();
        usernum++;
    }else if(mail.type==2){
        printf("%s : %s\n",mail.sstr,mail.lstr);
        if(mail.from==3){
            mailbox_send(box9,mail);
        }else if(mail.from==9){
            mailbox_send(box3,mail);
        }
    }else if(mail.type==3){
        printf("%s leave the chatroom\n",mail.sstr);
        if(mail.from==3){
            if(usernum>1){
                mailbox_send(box9,mail);
            }
            memset(usertable[3],0,20);
            display(); 
        }else if(mail.from==9){
            if(usernum>1){
                mailbox_send(box3,mail);
            }
            memset(usertable[9],0,20);
            display();
        }
        usernum--;
    }

    memset(box.m_base,0,sizeof(box.m_base));

    return 1;
}

int main(int argc, char const *argv[]){
    int i;
    mailbox_t box0;
    box0=mailbox_open(0);   
    mail_t mymail;

    while(1){
        fcntl(0, F_SETFL, fcntl(0, F_GETFL) | O_NONBLOCK);
        sleep(1);
        mailbox_recv_send(box0,mymail);       
    }

    mailbox_close(box0);
    mailbox_unlink(0);
    return 0;
}

