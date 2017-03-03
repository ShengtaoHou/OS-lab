#ifndef MAILBOX_H_
#define MAILBOX_H_

typedef struct __MAIL {
    int from;
    int type;
    char sstr[64];
    char lstr[512];
} mail_t;

typedef struct {
    char*m_base;
    char*m_fd;
    int size;
} mailbox_t;

extern void;

mailbox_t mailbox_open(int id){

    int SIZE = 4096;		// file size
    int shm_fd;		// file descriptor, from shm_open()
    char *shm_base;	// base address, from mmap()
    
    char name[20] ={'/','_','m','a','i','l','b','o','x','_'};
    char tmpc[5];
    sprintf(tmpc,"%d",id);
    name[10]=tmpc[0];

    mailbox_t mailbox={0};

    shm_fd = shm_open(name, O_CREAT | O_RDWR, 0666);
    if (shm_fd == -1) {
        printf("Shared memory failed: %s\n", strerror(errno));
        return mailbox;
    }

    ftruncate(shm_fd, SIZE);

    shm_base = mmap(0, SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (shm_base == MAP_FAILED) {
        printf("Map failed: %s\n", strerror(errno));
        return mailbox;
    }

    mailbox.m_base=shm_base;
    mailbox.m_fd=shm_fd;
    mailbox.size=SIZE;
    
    return mailbox;
}
int mailbox_check_empty(mailbox_t box){
    if((box.m_base!= NULL) && (box.m_base[0] == '\0'))return 1;
    return 0;
}
int mailbox_check_full(mailbox_t box){
    if((box.m_base!= NULL) && (box.m_base[0] == '\0'))return 0;
    return 1;
}
int mailbox_close(mailbox_t box){
    if (munmap(box.m_base, box.size) == -1) {
        printf("Unmap failed: %s\n", strerror(errno));
        return -1;
    }
    if (close(box.m_fd) == -1) {
        printf("Close failed: %s\n", strerror(errno));
        return -1;
    }
    return 0;
}

int mailbox_unlink(int id){
    char name[20] ={'/','_','m','a','i','l','b','o','x','_'};
    char tmpc[5];
    sprintf(tmpc,"%d",id);
    name[10]=tmpc[0];

    if (shm_unlink(name) == -1) {
    printf("Error removing %s: %s\n", name, strerror(errno));
    return -1;
  }
    return 0;
}
int mailbox_send(mailbox_t box, mail_t mail){
    if(mailbox_check_full(box))return -1;//the mailbox is full
    char*ptr;
    ptr = box.m_base;
    ptr += sprintf(ptr, "%d ", mail.from);
    ptr += sprintf(ptr, "%d ", mail.type);
    ptr += sprintf(ptr, "%s ", mail.sstr);
    ptr += sprintf(ptr, "%s ", mail.lstr);
    
    return 0;    
}
int mailbox_recv(mailbox_t box, mail_t mail){
    if(mailbox_check_empty(box))return -1;

    int from,type;
    char str1[64];
    char str2[512];
    char*ptr;
    int len;
    
    sscanf(box.m_base,"%d %d %s",&from,&type,&str1);
    len=strlen(str1);
    ptr=box.m_base+5+len;
    strcpy(str2,ptr);
    if(type==1){
        printf("%s join the chatroom\n\n",str1);  
    }else if(type==2){
        printf("%s : %s\n",str1,str2);
    }else if(type==3){
        printf("%s leave the chatroom\n\n",str1);
    }

    memset(box.m_base,0,sizeof(box.m_base));

    return 0;
}

#endif
