#include <linux/init.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/kobject.h>
#include <linux/string.h>
//#include <linux/stdlib.h>

MODULE_LICENSE("Dual BSD/GPL");

static struct kobject *hw1_kobject;
static int mask =111;

//swap string
static int index;
static char str1[50];

//cal_str
static int calres;

//sum tree
static char treetmp;
static char str3[50];
//static char res[50]={0};

static char *name1 ="swap_string";
static char *name2 ="calc";
static char *name3 ="sum_tree";

module_param(mask,int,S_IRUGO | S_IWUSR);
module_param(name1,charp,S_IRUGO);
module_param(name2,charp,S_IRUGO);
module_param(name3,charp,S_IRUGO);


static ssize_t swap_show(struct kobject *kobj,struct kobj_attribute *attr,char *buf)
{    
	char temp[50];
    int length=strlen(str1);
    index--;
    int i;
    for(i=0;i<length;i++){
		if(index==strlen(str1)-1)
            index=0;      
        else
            index++;                
        temp[i]=str1[index];
    } 
    return sprintf(buf,"%s \n",temp);
}
static ssize_t swap_store(struct kobject *kobj,struct kobj_attribute *attr,char *buf,size_t count)
{
	sscanf(buf,"%d %s",&index,&str1);
    return count;
}

static int calculate(int left,int right,char op){
	int result=0;
	switch(op)
	{
		case '+':
			result=left+right;
			break;
		case '-':
			result=left-right;
			break;
		case '*':
			result=left*right;
			break;
		case '/':
			result=left/right;
			break;
		case '%':
			result=left%right;
			break;
	}
	return result;
}
static int cmp(char a,char b)
{
	if((a=='+'||a=='-')&&(b=='*'||b=='/'||b=='%')){
		return 0; // less	
	}else{
		return 1; 
	}
}
static ssize_t calc_store(struct kobject *kobj,struct kobj_attribute *attr,char *buf,size_t count)
{   
     
    char str2[50];
    int num[30];
    int num_top=0;
    char op[30];
    int op_top=0;
    int numtemp=0;
    int offset;
    sscanf(buf,"%s%n",&str2,&offset);
	buf+=offset;
	int size=strlen(str2);
    int i;
	for(i=0;i<size;++i){
		char ch=str2[i];
		if(ch>='0'&&ch<='9'){
			numtemp=numtemp*10+ch-'0';	
		}
		else if(ch=='+'||ch=='-'||ch=='*'||ch=='/'||ch=='%'){
			    num[num_top++]=numtemp;
				numtemp=0;
				char op2=ch;
				for(;op_top>0;){
					char op1=op[op_top-1];
					int cmpret=cmp(op1,op2);
					if(cmpret==1){ //op1>=op2
						int num2=num[num_top-1];
						int num1=num[num_top-2];
						op_top--;
						int ret=calculate(num1,num2,op1);
						num[num_top-2]=ret;
						num[num_top-1]=0;
						num_top--;
					}else if(cmpret==0){
						break;
					}	
				}  //end for
			op[op_top++]=op2;	
			}	//end if
	}//end for
	
	num[num_top++]=numtemp;
	for(;op_top>0;){
		int num2=num[num_top-1];
		int num1=num[num_top-2];
		char opt=op[op_top-1];
		op_top--;
		
		int ret=calculate(num1,num2,opt);
		num[num_top-2]=ret;
		num[num_top-1]=0;
		num_top--;
	}
        calres=num[0];
   return count;
}
static ssize_t calc_show(struct kobject *kobj,struct kobj_attribute *attr,char *buf)
{        
    
    return sprintf(buf,"%d\n",calres);
}

//******sum tree********


static ssize_t sum_tree_store(struct kobject *kobj,struct kobj_attribute *attr,char *buf,size_t count)
{   
    int offset;
    //int length;
    //length=strlen(buf);
    int t=0;
    while(sscanf(buf,"%c%n",&treetmp,&offset)==1){
        str3[t]=treetmp;
        t++;
        buf+=offset;
    }
    //sscanf(buf,"%[ 0-9()]%n",str3,offset); 
    //buf+=offset;
    return count;
}
static ssize_t sum_tree_show(struct kobject *kobj,struct kobj_attribute *attr,char *buf)
{

    char res[50]={0}; 
    //char res2[50]={0};
	int tree[30][30];
	char tmpc[10];
	int length=strlen(str3);
	int k;
	int i=0;
	int j=0;
	int x;
	int y;
	int numtemp=0;
    
	for(i=0;i<30;i++){
		for(j=0;j<30;j++){
			tree[i][j]=999;
		}
	}
	i=0;
	j=0;
	for(k=0;k<length;k++)
	{
		if(str3[k]>='0'&&str3[k]<='9'&&str3[k+1]>='0'&&str3[k+1]<='9'){
			numtemp=str3[k]-'0';	//accumalate
		}else if(str3[k]>='0'&&str3[k]<='9'&&str3[k+1]==' '){
			numtemp=numtemp*10+str3[k]-'0';   //move to the next
			tree[i][j]=numtemp;     
			i++;
			numtemp=0;
		}else if(str3[k]>='0'&&str3[k]<='9'&&str3[k+1]=='('){
			numtemp=numtemp*10+str3[k]-'0';   //move to the next
			tree[i][j]=numtemp;     
			j++;
			numtemp=0;
		}else if(str3[k]>='0'&&str3[k]<='9'&&str3[k+1]==')'){
			numtemp=numtemp*10+str3[k]-'0';   //move to the next
			tree[i][j]=numtemp;     
			i++;
			j--;
			numtemp=0;
		}else{
			
		}
		
	}//k for end
	x=i;
    y=j;

	for(j=0;j<y+1;j++){
		int tag=111;
		for(i=0;i<x;i++){
			if(tree[i][j]!=999&&tree[i][j+1]!=999){
				// father node
				tag=tree[i][j];
			}
			if(tag!=111&&tree[i][j]==999){
				tree[i][j]=tag;
			}
		}
	}	
    int first=1;
	for(i=0;i<x;i++){
		int sum=0;
		for(j=0;j<y+2;j++){
			if(tree[i][j]!=999){
				sum+=tree[i][j];
//				
			}
		}
		
		sprintf(tmpc,"%d",sum);
        if(first){
            first=0;
            strcat(res,tmpc);
        }else{
            strcat(res,",");
            strcat(res,tmpc);
        }
	}
    //int leng=strlen(res);
    //memset(res2,res,sizeof(char)*(leng-1));

  return sprintf(buf,"%s \n",res);
}
static struct kobj_attribute swap_string_attribute = __ATTR(swap_string,0660,swap_show,swap_store);
static struct kobj_attribute calc_attribute = __ATTR(calc,0660,calc_show,calc_store);
static struct kobj_attribute sum_tree_attribute = __ATTR(sum_tree,0660,sum_tree_show,sum_tree_store);


//module init
static int __init hw1_module_init(void)
{   int error =0 ;
    hw1_kobject = kobject_create_and_add("hw1",kernel_kobj);

    swap_string_attribute.attr.name=name1;
    calc_attribute.attr.name=name2;
    sum_tree_attribute.attr.name=name3;

    if(mask==111){
        error=sysfs_create_file(hw1_kobject,&swap_string_attribute.attr);
		error=sysfs_create_file(hw1_kobject,&calc_attribute.attr);
		error=sysfs_create_file(hw1_kobject,&sum_tree_attribute.attr);
    }else if(mask==001){
		error=sysfs_create_file(hw1_kobject,&sum_tree_attribute.attr);
    }else if(mask==010){
		error=sysfs_create_file(hw1_kobject,&calc_attribute.attr);	
    }else if(mask==011){
		error=sysfs_create_file(hw1_kobject,&calc_attribute.attr);
		error=sysfs_create_file(hw1_kobject,&sum_tree_attribute.attr);
    }else if(mask==100){
		sysfs_create_file(hw1_kobject,&swap_string_attribute.attr);
    }else if(mask==101){
		error=sysfs_create_file(hw1_kobject,&swap_string_attribute.attr);
		error=sysfs_create_file(hw1_kobject,&sum_tree_attribute.attr);
    }else if(mask==110){
		error=sysfs_create_file(hw1_kobject,&swap_string_attribute.attr);
		error=sysfs_create_file(hw1_kobject,&calc_attribute.attr);
    }else if(mask==000){
    }

    return error;
}
static void __exit hw1_module_exit(void)
{
   printk(KERN_ALERT "bye\n");
   pr_debug("Module un initialized successfully\n");
   kobject_put(hw1_kobject);

}
module_init(hw1_module_init);
module_exit(hw1_module_exit);
