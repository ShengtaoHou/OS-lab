#include<stdio.h>
#include<string.h>    //strlen
#include<stdlib.h>    //strlen
#include<sys/socket.h>
#include<arpa/inet.h> //inet_addr
#include<unistd.h>    //write
#include<pthread.h> //for threading , link with lpthread
pthread_mutex_t mutex;
static int total=0;
static char result[30];
static const char*response[]={
	"200 OK",
	"400 BAD REQUEST",
	"404 NOT FOUND",
	"405 METHOD NOT ALLOWED"
};

static struct DNS_db{
	char domain[50];
	char ip[30];
}db[50];

void *connection_handler(void *);
int check_method(char m[]){
	
	if(strcmp("SET",m)&&strcmp("GET",m)&&strcmp("INFO",m))return 1;
	return 0;
}

int check_domain(char d[]){

    if(d[1]=='.')return 1;
    int dotnum=0;
    int i;
    for(i=0;d[i]!='\0';i++){
    	if(d[i]=='.')dotnum++;
    	if(d[i]>'A'&&d[i]<'Z'){
    		d[i]=d[i]+32;
    	}
    }
    if(d[i-1]=='.')return 1;
    if(dotnum==0)return 1;
    //printf("doamin:%s\n",d);
	return 0;
}

int check_ip(char p[]){
	//printf("ip:%s\n",p);
	int n[4];
    char c[4];
    if (sscanf(p, "%d%c%d%c%d%c%d%c",&n[0], &c[0], &n[1], &c[1],&n[2], &c[2], &n[3], &c[3])== 7){
	    int i;
	    for(i = 0; i < 3; ++i){
	    	if (c[i] != '.')return 1;
	    }
	    for(i = 0; i < 4; ++i){
	    	if (n[i] > 255 || n[i] < 0)return 1;
	    }
	    //printf("ip is ok\n");
	    return 0;
	}else{
		return 1;
	}
}

int check_request(char m[],char d[],char p[],int spacenum){
	if(strcmp("SET",m)==0&&spacenum!=2)return 1;
	if(strcmp("GET",m)==0&&spacenum!=1)return 1;
	if(strcmp("INFO",m)==0&&spacenum!=0)return 1;
	
	if(spacenum==2){
		if(check_domain(d))return 1;
		if(check_ip(p))return 1;
	}else if(spacenum==1){
		if(check_domain(d))return 1;
	}
	
	return 0;
}
int search_or_set_ip(char m[],char d[],char p[]){
    //printf("domain before:%s\n",d);
	 for(int i=0;d[i]!='\0';i++){
    	if(d[i]>='A'&&d[i]<='Z'){
    		d[i]=d[i]+32;
    	}
    }
    //printf("domain after:%s\n",d);

	if(strcmp("SET",m)==0){
		 for(int i=0;i<total;i++){
			if(strcmp(db[i].domain,d)==0){  //domain alreadly exit
				strcpy(db[i].ip,p);
                printf("update the db\n");
				return 0;
		    }
		 }                       
		strcpy(db[total].domain,d);   //domain not exit
		strcpy(db[total].ip,p);
		total++;
        printf("create new db\n");	
		return 0; //found the domain in db ,set the IP
	}else if(strcmp("GET",m)==0){
		for(int i=0;i<total;i++){
			if(strcmp(db[i].domain,d)==0){
				strcpy(result,db[i].ip);
				return 0; //found the domain in db,write it to result
			}
		}
	}else if(strcmp("INFO",m)==0){
        sprintf(result,"%d",total);
        {
            //printf("info result:%s\n",result);
        }
		//itoa(total,result,10); //write the number of db to result
		return 0; 
	}
	
	return 1;// not found
}
int process_request(const char r[]){
	int status=0;
	char method[5];
	char domain[30];
	char ip[30];
	int spacenum=0;
	int m=0;
	int d=0;
	int p=0;
	for(int i=0;r[i]!='\0';i++){
		if(r[i]==' ')spacenum++;
		if(spacenum==0){
			method[m++]=r[i];	
		}else if(spacenum==1){
			domain[d++]=r[i];
		}else if(spacenum==2){
			ip[p++]=r[i];
		}
	}
	method[m]='\0';
	domain[d]='\0';
	ip[p]='\0';
	
	if(check_method(method))return 3; //method not allowed
	if(check_request(method,domain,ip,spacenum))return 1; //bad request
	
	if(search_or_set_ip(method,domain,ip))return 2; // not found
	return 0; //ok 
	//return 1;
}


int main(int argc , char *argv[])
{
    int socket_desc , client_sock , c , *new_sock;
    struct sockaddr_in server , client;
     
    socket_desc = socket(AF_INET , SOCK_STREAM , 0);
    if (socket_desc == -1){
        printf("Could not create socket");
    }
    printf("Socket created\n");
     
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons( 12345 );
     
    if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0){
        perror("bind failed. Error");
        return 1;
    }
    printf("bind done\n");
     
    listen(socket_desc , 10);
     
    printf("Waiting for incoming connections...\n");
    c = sizeof(struct sockaddr_in);   

    while( (client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c)) ){
        printf("Connection accepted\n");      
        pthread_t sniffer_thread;
        new_sock = malloc(1);
        *new_sock = client_sock;

        pthread_mutex_init(&mutex, NULL);
        if( pthread_create( &sniffer_thread , NULL ,  connection_handler , (void*) new_sock) < 0){
            perror("could not create thread");
            return 1;
        }
        printf("New thread created\n"); 

    }
    pthread_mutex_destroy(&mutex);
    if (client_sock < 0){
        perror("accept failed");
        return 1;
    }
    return 0;
}

void *connection_handler(void *socket_desc){

    printf("connection handler assigned\n");
    int sock = *(int*)socket_desc;
    char request[100];

    int read_size;
    memset(request,0,100);
    size_t*size;     
    ssize_t ret;

while((read_size = recv(sock , request , 100 , 0)) >0){
        pthread_mutex_lock(&mutex);
   
        printf("request:%s\n",request);	
		int retid=process_request(request);
		char res[100]={0};

		if(retid==0){		
			strcpy(res,response[retid]);
			strcat(res," ");
			strcat(res,result);		
			memset(result,0,30);//clean it so that no repeat
		}else{
			strcpy(res,response[retid]);
            memset(result,0,30);
		}

        size_t len=strlen(res);
        printf("response: %s\n\n",res);

        if (write(sock, &len, sizeof(size_t)) == -1)
               printf("number fail\n");

        write(sock,res,len);

        memset(res,0,100);  
        memset(request,0,100);
        pthread_mutex_unlock(&mutex);
}

    if(read_size == 0){
        puts("Client disconnected");
        fflush(stdout);
    }else if(read_size == -1){
        perror("recv failed");
    }
    free(socket_desc);  
    return 0;
}
