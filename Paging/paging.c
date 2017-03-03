#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<time.h>
#define max 50

static int N;//virtual page number
static int M;//physical frame number

//---table zone
struct PageTable{
	//int VPN;
	int present;   //in physical memory or not,0 not in memory, 1 in memory,default not in 
	int PFN;      //if in memory, physical frame number;default -1
	int inuse;	//in use or not, 0 not in use; 1 in use,maybe in disk
	int disknum; //if in disk, tell the disk number,easier access
	int time;
}PT[max];			//suppose the Page number no more than max;index is virtual page number
static int PTE_num=0;      //the number of PTE,PTEnum=N

//---queue zone; FIFO use,define a simple queue with a max size max
int queue[max]={0};
int head=0;
int tail=0;

void enterqueue(int n){
	queue[tail]=n;
	tail=(tail+1)%max; //when the node reach the end, it move to the front
}
int dequeue(){
	int ret=queue[head];
	head=(head+1)%max;
	return ret; //return the head value
}
int is_empty(){
	if(head==tail)return 1; //we ingore the full case, since it is useless in this function 
	else return 0;
}

//---physical memory zone 
struct physical_mem{
	int VPN; //which VPN occupy the current mem entry
	//int time;
}mem[max];
static int mem_num;//the number of physical memory number
static mem_top=0;//the top position of free memory
//static int phymem_full=0;//0 not full, 1 full
int check_mem_full(){
	if(mem_top==mem_num)return 1;//memory is full
	return 0;//memory not full
} 

//---Disk zone
struct Disk{
	int VPN;//the VPN occupy the current disk 
	int free;//free or not, 0 free,1 not free,default free
}disk[max];
static int least_free;//the least number of free disk frame

void up_least_free(int p){
	//printf("p before%d\n",p);
	while(disk[p].free==1){
		p++;
	}
	//printf("p after%d\n",p);
	least_free=p;
} 
void down_least_free(int p){
	if(p<least_free){
		least_free=p;
	}
}
//output zone
static int PFN;
static int outVPN; //the VPN swap out
static int dest;//VPN swap out to which disk frame 
static int inVPN;//the VPN swap in
static int source;//swap in from which disk frame 
static int is_swap; //swap or not

void output(int ref,FILE*fout){
	char buf[40];
	
	if(is_swap==1){
		sprintf(buf,"Miss, %d, %d>>%d, %d<<%d\n",PT[ref].PFN,outVPN,dest,inVPN,source);	
	}else{
		sprintf(buf,"Hit, %d=>>%d\n",ref,PT[ref].PFN);
	}
	fputs(buf,fout);
}

//action zone 
void swap_out(int outP){
		PT[outP].present=0; //��Ҫ�߳�ȥ�Ĵ�mem��ɾ�� 
		//PT[out].PFN=-1;    //PFN=-1����ռ��PF 
		int outnum=least_free; //ѡ��Ҫ�ߵ�����ȥ���ߵ�disk�����λ�� 
		PT[outP].disknum=outnum;   //����������߳�ȥ��VPN��disk num�������ߵ���λ�� 
		disk[outnum].free=1;      //ռ���ߵ���λ�� 
		disk[outnum].VPN=outP;     //����disk��ֵ�����ڱ��߳�ȥ��VPN 
					
		outVPN=outP; //��ȥ��VPN 
		dest=outnum; //ȥ������
		is_swap=1;  //�з���page fault
		PT[outP].time=0;		
		up_least_free(outnum);       //������������diskλ�ã�����disk��Ϳ���λ��	
}
void swap_in(int ref,int outP){
	PT[ref].inuse=1;   //�����VPN��ʼʹ�� 
	PT[ref].present=1; //VPNռ��mem 
	PT[ref].PFN=PT[outP].PFN; //VPNռ�ñ��߳�ȥ��VPN��λ�� 
	PT[ref].time=0;
	enterqueue(ref);//����mem��ͬʱ����queue�� 
	inVPN=ref;  //������VPN 	
}
//policy zone
int maxtime(){
	int i,ret;
	int maxnum=-1;
	for(i=0;i<N;i++){
		if(PT[i].present==1&&PT[i].time>maxnum){
			maxnum=PT[i].time;
			ret=i;
		}
	}
	return ret;
}
int random(){
	//srand((unsigned)time(NULL));  //��������ÿ�ν����һ�������� 
	int r=rand()%M;
	return mem[r].VPN;
}
int policy(int method){
	if(method==1)return dequeue();
	if(method==2)return maxtime();
	if(method==3)return random();
}
void init(){
	//init the page table and mem
	int i;
	for(i=0;i<N;i++){
		PT[i].present=0;
		PT[i].PFN=-1;
		PT[i].inuse=0;
		PT[i].disknum=-1;
	}
	PTE_num=N;
	mem_num=M;
} 
int judge_method(char b[]){
	if(strcmp(b,"Policy: FIFO\n")==0){
	
		return 1;
	}
	if(strcmp(b,"Policy: LRU\n")==0)return 2;
	if(strcmp(b,"Policy: Random\n")==0)return 3;
	printf("error\n");
	return 1;
}
int getnum(char buf[]){
	int len=strlen(buf);
	int i;
	int num=0;
	for(i=0;i<len;i++){
		if(buf[i]>='0'&&buf[i]<='9'){
			num=num*10+buf[i]-'0';
		}
	}
	return num;
}
//hit rate record
static int hit=0;
static int total=0;

int main(){	
	int i,j,ref;
	//-----test random part:
	//FILE *fin  = fopen ("trace/random/fifo_20_10_500.txt", "r");
    //FILE *fout = fopen ("trace/random/fifo_20_10_500_res.txt", "w");
    //FILE *fin  = fopen ("trace/random/fifo_20_10_5000.txt", "r");
    //FILE *fout = fopen ("trace/random/fifo_20_10_5000_res.txt", "w");
    //FILE *fin  = fopen ("trace/random/fifo_20_19_500.txt", "r");
    //FILE *fout = fopen ("trace/random/fifo_20_19_500_res.txt", "w");
    //FILE *fin  = fopen ("trace/random/fifo_20_19_5000.txt", "r");
    //FILE *fout = fopen ("trace/random/fifo_20_19_5000_res.txt", "w");
    
    //FILE *fin  = fopen ("trace/random/lru_20_10_500.txt", "r");
    //FILE *fout = fopen ("trace/random/lru_20_10_500_res.txt", "w");
    //FILE *fin  = fopen ("trace/random/lru_20_10_5000.txt", "r");
    //FILE *fout = fopen ("trace/random/lru_20_10_5000_res.txt", "w");
    //FILE *fin  = fopen ("trace/random/lru_20_19_500.txt", "r");
    //FILE *fout = fopen ("trace/random/lru_20_19_500_res.txt", "w");
    //FILE *fin  = fopen ("trace/random/lru_20_19_5000.txt", "r");
    //FILE *fout = fopen ("trace/random/lru_20_19_5000_res.txt", "w");
    
    FILE *fin  = fopen ("trace/random/random_20_10_500.txt", "r");
    FILE *fout = fopen ("trace/random/random_20_10_500_res.txt", "w");
    //FILE *fin  = fopen ("trace/random/random_20_10_5000.txt", "r");
    //FILE *fout = fopen ("trace/random/random_20_10_5000_res.txt", "w");
    //FILE *fin  = fopen ("trace/random/random_20_19_500.txt", "r");
    //FILE *fout = fopen ("trace/random/random_20_19_500_res.txt", "w");
    //FILE *fin  = fopen ("trace/random/random_20_19_5000.txt", "r");
    //FILE *fout = fopen ("trace/random/random_20_19_5000_res.txt", "w");
    
    //-----test special_locality part:
    //FILE *fin  = fopen ("trace/spacial_locality/fifo_best.txt", "r");
    //FILE *fout = fopen ("trace/spacial_locality/fifo_best_res.txt", "w");
    //FILE *fin  = fopen ("trace/spacial_locality/fifo_worse.txt", "r");
    //FILE *fout = fopen ("trace/spacial_locality/fifo_worse_res.txt", "w");
    //FILE *fin  = fopen ("trace/spacial_locality/lru_worse.txt", "r");
    //FILE *fout = fopen ("trace/spacial_locality/lru_worse_res.txt", "w");
    //FILE *fin  = fopen ("trace/spacial_locality/random_best.txt", "r");
    //FILE *fout = fopen ("trace/spacial_locality/random_best_res.txt", "w");
    //FILE *fin  = fopen ("trace/spacial_locality/random_worse.txt", "r");
    //FILE *fout = fopen ("trace/spacial_locality/random_worse_res.txt", "w");
    
    //-----test temporal_locality part:
    //FILE *fin  = fopen ("trace/temporal_locality/fifo_best.txt", "r");
    //FILE *fout = fopen ("trace/temporal_locality/fifo_best_res.txt", "w");
    //FILE *fin  = fopen ("trace/temporal_locality/fifo_worse.txt", "r");
    //FILE *fout = fopen ("trace/temporal_locality/fifo_worse_res.txt", "w");
    //FILE *fin  = fopen ("trace/temporal_locality/lru_best.txt", "r");
    //FILE *fout = fopen ("trace/temporal_locality/lru_best_res.txt", "w");
    //FILE *fin  = fopen ("trace/temporal_locality/lru_worse.txt", "r");
    //FILE *fout = fopen ("trace/temporal_locality/lru_worse_res.txt", "w");
    //FILE *fin  = fopen ("trace/temporal_locality/random_best.txt", "r");
    //FILE *fout = fopen ("trace/temporal_locality/random_best_res.txt", "w");
    //FILE *fin  = fopen ("trace/temporal_locality/random_worse.txt", "r");
    //FILE *fout = fopen ("trace/temporal_locality/random_worse_res.txt", "w");
    char buf[40];
	//char buf2[40];
	char*buf2;
	
    //��ȡ���� 
	fgets(buf,40,fin);
	int method=judge_method(buf);
	
	//��ȡM,N 
	fgets(buf,40,fin);
	N=getnum(buf);
	
	fgets(buf,40,fin);
	M=getnum(buf);
	
	fgets(buf,40,fin);
	
	srand((unsigned)time(NULL));
	
	init();
			
while(fgets(buf,40,fin)){
		total++;
		ref=getnum(buf);
		
		for(j=0;j<N;j++){
			if(PT[j].present==1){  //����mem�����+1 
				PT[j].time++;
			}
		} 
					
	//check in memory or not
	if(PT[ref].present==1){ //in mem, hit
		hit++;
		is_swap=0;
		PT[ref].time=0; //���õ���ʱ����� 
		output(ref,fout);
		//enterqueue(ref); hit will not change the queue position
		
	}else{					   //not int mem, miss
									//not in memory; if memory not full, just put the VPN in the memory
		if(check_mem_full()==0){
			is_swap=1;         //page fault occur
			mem[mem_top].VPN=ref;
			
			PT[ref].inuse=1;
			
			PT[ref].present=1;
			PT[ref].PFN=mem_top;
			
			mem_top++;
			//updata the output
			outVPN=-1;
			dest=-1;
			inVPN=ref;
			source=-1;
			output(ref,fout);
			enterqueue(ref); //VPN in queue
		}else{                    //not in memory; and memory full; check in disk or not
			if(PT[ref].inuse==0){      //mem���ˣ��ֲ���disk���� 			
					int outP=policy(method); //ѡ��һ���߳�ȥ 														 
					swap_out(outP);
					swap_in(ref,outP);																							
					source=-1;  //����-1��˵������disk					 					 
					output(ref,fout);	//��� 	
			}else{                 //in disk, swap case 2				
					int outP=policy(method);//ѡ��һ���߳�ȥ					
					swap_out(outP);
					swap_in(ref,outP);											
					source=PT[ref].disknum;//important														
					down_least_free(PT[ref].disknum);//Ψһ��������� VPN��disk���������ԣ�free p���ܼ��� 					
					output(ref,fout);
			}
		}
	}
	
	}
	float rate=1-1.0*hit/total;
	sprintf(buf,"Page Fault Rate: %.3f",rate);
	fputs(buf,fout);
	return 0;
}
 





 
