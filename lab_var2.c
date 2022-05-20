#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>

#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>

#include <sys/time.h>
#include <time.h>


#define FORMAT "N:%d - pid:%d - ppid:%d- %s - %s - time:%ld \n"
#define EX "N:%d - pid:%d - ppid:%d - ended work after %d sig SIGUSR1 and %d sig SIGUSR2\n"
#define SLEEPTIME 1000



int N=0,X=0,Y=0,GET=0;
pid_t adress;


long int getTime()
{
	struct timeval time;
	gettimeofday(&time,NULL);
	return time.tv_sec*1000+time.tv_usec/1000;
}

void sigUsr1(int sig,siginfo_t *info,void *context){

	if(sig!=SIGUSR1){
			 return;
		}
		else{
			GET++;
			 if(GET<101){
				 printf(FORMAT,N,getpid(),getppid(),"get","SIGUSR1",getTime());
				 printf(FORMAT,N,getpid(),getppid(),"put","SIGUSR2",getTime());
				 Y=Y+4;
				 usleep(SLEEPTIME);
				 kill(adress,SIGUSR2);
			 }
			 else{
			 	int status;
			 	usleep(SLEEPTIME*2);
			 	kill(adress,SIGTERM);
			 	for(int i=0;i<7;i++)
			 	wait(&status);
			 	
			 	printf(EX,N,getpid(),getppid(),X,Y);
			 	exit(0);
			 	
			 	
			 }
		}
}



void sigUsr234(int sig,siginfo_t *info,void *context)
{
	if(sig==SIGTERM){
	int status;
		kill(adress,SIGTERM);
		waitpid(adress,&status,0);
		printf(EX,N,getpid(),getppid(),X,Y);
		exit(0);
	}

	if(sig!= SIGUSR2){
				return;
			}
			else{			
				printf(FORMAT,N,getpid(),getppid(),"get","SIGUSR2",getTime());
				printf(FORMAT,N,getpid(),getppid(),"put","SIGUSR1",getTime());
				X++;
				usleep(SLEEPTIME);
				kill(adress,SIGUSR1);
			}
}

void sigUsr5(int sig,siginfo_t *info,void *context)
{
	if(sig==SIGTERM){
		printf(EX,N,getpid(),getppid(),X,Y);
		exit(0);
	}

	if(sig!= SIGUSR2){
				return;
			}
			else{
			 	printf(FORMAT,N,getpid(),getppid(),"get","SIGUSR2",getTime());
			 }
}

void sigUsr67(int sig,siginfo_t *info,void *context)
{
	if(sig==SIGTERM){
		printf(EX,N,getpid(),getppid(),X,Y);
		exit(0);
	}

	if(sig!= SIGUSR1){
				return;
			}
			else{
			 	printf(FORMAT,N,getpid(),getppid(),"get","SIGUSR1",getTime());
			 }
}

void sigUsr8(int sig,siginfo_t *info,void *context)
{
	if(sig==SIGTERM){
		printf(EX,N,getpid(),getppid(),X,Y);
		exit(0);
	}
	

	if(sig!= SIGUSR1){
				return;
			}
			else{
			 	printf(FORMAT,N,getpid(),getppid(),"get","SIGUSR1",getTime());
			 	printf(FORMAT,N,getpid(),getppid(),"put","SIGUSR1",getTime());
			 	X++;
			 	usleep(SLEEPTIME);
			 	kill(adress,SIGUSR1);
			 }
}

void sigfunc1(int sig,siginfo_t *info,void *context)
{
	printf("%d\n",N);
}


int main(int argc,char* argv[]){

pid_t p;
struct sigaction act={0};

p=fork();
if(p==0){ 
	setpgid(getpid(),getpid());
	p=fork();
	if(p>0){
		p=fork();
		if(p>0){
			p=fork();
			if(p>0){
				p=fork();
				if(p>0){//CHILD1
					adress=-getpgid(getpid());
					N=1;
					act.sa_flags=SA_SIGINFO;
					act.sa_sigaction=&sigUsr1;
					sigaction(SIGUSR1,&act,NULL);
					sigaction(SIGUSR2,&act,NULL);
					sigaction(SIGTERM,&act,NULL);
					
					usleep(SLEEPTIME*1000);
					Y=Y+4;
					kill(adress,SIGUSR2);
					
					
					
				}//~CHILD1
				else if(p==0){ //CHILD5
					N=5;
					act.sa_flags=SA_SIGINFO;
					act.sa_sigaction=&sigUsr5;
					sigaction(SIGUSR1,&act,NULL);
					sigaction(SIGUSR2,&act,NULL);
					sigaction(SIGTERM,&act,NULL);
					
				}//~CHILD5
			
			
			}
			else if(p==0){
				p=fork();
				if(p>0){//CHILD4
					adress=p;
					N=4;
					act.sa_flags=SA_SIGINFO;
					act.sa_sigaction=&sigUsr234;
					sigaction(SIGUSR1,&act,NULL);
					sigaction(SIGUSR2,&act,NULL);
					sigaction(SIGTERM,&act,NULL);
					
				
				}//~CHILD4
				else if(p==0){//CHILD8
					adress=getpgid(getpid());
					setpgid(getpid(),getpid());
					N=8;
					act.sa_flags=SA_SIGINFO;
					act.sa_sigaction=&sigUsr8;
					sigaction(SIGUSR1,&act,NULL);
					sigaction(SIGUSR2,&act,NULL);
					sigaction(SIGTERM,&act,NULL);
					
					
				}//~CHILD8
			}
		}
		else if(p==0){ 
			p=fork();
			if(p>0){//CHILD3
				adress=p;
				N=3;
				act.sa_flags=SA_SIGINFO;
				act.sa_sigaction=&sigUsr234;
				sigaction(SIGUSR1,&act,NULL);
				sigaction(SIGUSR2,&act,NULL);
				sigaction(SIGTERM,&act,NULL);
				
			
			}//~CHILD3
			else if(p==0){//CHILD7
				
				setpgid(getpid(),getpid());
				N=7;
				act.sa_flags=SA_SIGINFO;
				act.sa_sigaction=&sigUsr67;
				sigaction(SIGUSR1,&act,NULL);
				sigaction(SIGUSR2,&act,NULL);
				sigaction(SIGTERM,&act,NULL);
				
				
			}//~CHILD7

		} 
	}
	else if(p==0){ 
		p=fork();
		if(p>0){//CHILD2
			adress=p;
			N=2;
			act.sa_flags=SA_SIGINFO;
			act.sa_sigaction=&sigUsr234;
			sigaction(SIGUSR1,&act,NULL);
			sigaction(SIGUSR2,&act,NULL);
			sigaction(SIGTERM,&act,NULL);
			
		
		}//~CHILD2
		else if(p==0){//CHILD6
			
			setpgid(getpid(),getpid());
			N=6;
			act.sa_flags=SA_SIGINFO;
			act.sa_sigaction=&sigUsr67;
			sigaction(SIGUSR1,&act,NULL);
			sigaction(SIGUSR2,&act,NULL);
			sigaction(SIGTERM,&act,NULL);
		}//~CHILD6

	} 

	
}
else if(p>0){//PARENT0
	act.sa_flags=SA_SIGINFO;
	act.sa_sigaction=&sigfunc1;
	sigaction(SIGUSR1,&act,NULL);
	sigaction(SIGUSR2,&act,NULL);
	sigaction(SIGTERM,&act,NULL);
	
	int status;
	waitpid(p,&status,0);
	printf(EX,N,getpid(),getppid(),X,Y);
	exit(0);
	
	
	
}//~PARENT0

int i=0;
while(1){i++;}



}
