#include <stdio.h> 
#include <pthread.h> 
#include <semaphore.h> 
#include <unistd.h> 
#include <stdlib.h>
#include <time.h>
#include <string.h>
int k,a,e,c,t1,t2,t,f,itr=0,Sf;     //f and Sf are flags
int patient[10005];
char nm[10005][105];
int performing[10005];      //defined globally so all values are already 0
int flag[10005];            //defined globally so all values are 0 initially
sem_t amutex;
sem_t emutex; 
sem_t tshirt;
  
typedef struct s
{
    char name[10005];
    char instrument[10005];
    int arrival_time;
    int performance_time;
} s;

void * Sacoustic(void *arg)
{
    s* inputs = (s*) arg;
    char s1[10005]={'\0'},s2[10005]={'\0'};
    int at,pt;

    strcpy(s1,inputs->name);
    strcpy(s2,inputs->instrument);
    at=inputs->arrival_time;
    pt=inputs->performance_time;

    int var;
    struct timespec tm;
    clock_gettime(CLOCK_REALTIME,&tm);
    tm.tv_sec+=t;

    //wait 
    var=sem_timedwait(&amutex,&tm);
    if(var==-1)
    {
        //timeout of the process.It will not occur now.
    }
    else if(Sf==0)
    {
        Sf=1;

        //critical section 
        printf("\033[1;33m%s %s is performing on acoustic stage for %d seconds\n",s1,s2,pt); 
        sleep(pt); 
          
        //signal 
        sem_post(&amutex);
        patient[itr]=1;
        printf("\033[1;34m%s %s exited from acoustic stage\n",s1,s2);
    }     
}

void * Selectric(void *arg)
{
    s* inputs = (s*) arg;
    char s1[10005]={'\0'},s2[10005]={'\0'};
    int at,pt;

    strcpy(s1,inputs->name);
    strcpy(s2,inputs->instrument);
    at=inputs->arrival_time;
    pt=inputs->performance_time;

    int var;
    struct timespec tm;
    clock_gettime(CLOCK_REALTIME,&tm);
    tm.tv_sec+=t;

    //wait
    var=sem_timedwait(&emutex,&tm);
    if(var==-1)
    {
        //timeout of the process.It will not occur now.
    }
    else if(Sf==0)
    {
        Sf=1;
    
        //critical section 
        performing[itr]=1;
        printf("\033[1;33m%s %s is performing on electric stage for %d seconds\n",s1,s2,pt); 
        sleep(pt); 
          
        //signal 
        sem_post(&emutex);
        patient[itr]=1;
        printf("\033[1;34m%s %s exited from electric stage\n",s1,s2);
    }       
}

void * Sjoinperformance(void *arg)
{
    s* inputs = (s*) arg;
    char s1[10005]={'\0'},s2[10005]={'\0'};
    int at,pt,i;

    strcpy(s1,inputs->name);
    strcpy(s2,inputs->instrument);
    at=inputs->arrival_time;
    pt=inputs->performance_time;

    int var;
    struct timespec tm;
    clock_gettime(CLOCK_REALTIME,&tm);
    tm.tv_sec+=t;

    time_t startTime = time(NULL); // return current time in seconds
    while(time(NULL) - startTime < t && Sf==0)
    {
        for(i=0;i<k;i++)
        {
            if(performing[i]==1)
            {
                performing[i]=0;
                flag[i]=1;
                Sf=1;
                patient[itr]=1;
                printf("\033[1;35m%s joined %s performace,performance extended by 2 seconds\n",s1,nm[i]);
                break;
            }
        }   
        if(Sf==1)
        {
            break;
        }
    }
}

void * acoustic(void *arg)
{
    s* inputs = (s*) arg;
    char s1[10005]={'\0'},s2[10005]={'\0'};
    int at,pt;

    strcpy(s1,inputs->name);
    strcpy(s2,inputs->instrument);
    at=inputs->arrival_time;
    pt=inputs->performance_time;

    int var;
    struct timespec tm;
    clock_gettime(CLOCK_REALTIME,&tm);
    tm.tv_sec+=t;

    //wait 
    var=sem_timedwait(&amutex,&tm);
    if(var==-1)
    {
        //timeout of the process.It will not occur now.
    }
    else if(f==0)
    {
        f=1;

        //critical section 
        performing[itr]=1;
        printf("\033[1;33m%s %s is performing on acoustic stage for %d seconds\n",s1,s2,pt); 
        sleep(pt);
        if(flag[itr]==1)
        {
            sleep(2);
        }  
          
        //signal 
        sem_post(&amutex);
        patient[itr]=1;
        printf("\033[1;34m%s %s exited from acoustic stage\n",s1,s2);
    }     
}

void * electric(void *arg)
{
    s* inputs = (s*) arg;
    char s1[10005]={'\0'},s2[10005]={'\0'};
    int at,pt;

    strcpy(s1,inputs->name);
    strcpy(s2,inputs->instrument);
    at=inputs->arrival_time;
    pt=inputs->performance_time;

    int var;
    struct timespec tm;
    clock_gettime(CLOCK_REALTIME,&tm);
    tm.tv_sec+=t;

    //wait
    var=sem_timedwait(&emutex,&tm);
    if(var==-1)
    {
        //timeout of the process.It will not occur now.
    }
    else if(f==0)
    {
        f=1;
    
        //critical section 
        performing[itr]=1;
        printf("\033[1;33m%s %s is performing on electric stage for %d seconds\n",s1,s2,pt); 
        sleep(pt); 
        if(flag[itr]==1)
        {
            sleep(2);
        } 
          
        //signal 
        sem_post(&emutex);
        patient[itr]=1;
        printf("\033[1;34m%s %s exited from electric stage\n",s1,s2);
    }       
}

void * thread(void * arg) 
{ 
    s* inputs = (s*) arg;
    char s1[10005]={'\0'},s2[10005]={'\0'};
    int val1,val2,at,pt,i;

    strcpy(s1,inputs->name);
    strcpy(s2,inputs->instrument);
    at=inputs->arrival_time;
    pt=inputs->performance_time;

    sleep(at);      //sleeping till arrival time
    printf("\033[1;36m%s %s arrived\n",s1,s2);
    //to get semaphore value 
    sem_getvalue(&amutex,&val1);
    sem_getvalue(&emutex,&val2);
    if(strcmp(s2,"s")==0)
    {
        if(val1>0)      //solo acoustic
        {
            //wait
            sem_wait(&amutex);

            //critical section 
            printf("\033[1;33m%s %s is performing on acoustic stage for %d seconds\n",s1,s2,pt); 
            sleep(pt); 
              
            //signal 
            sem_post(&amutex);
            patient[itr]=1;
            printf("\033[1;34m%s %s exited from acoustic stage\n",s1,s2);
        }
        else if(val2>0)     //solo electric
        {
            //wait
            sem_wait(&emutex);

            //critical section
            printf("\033[1;33m%s %s is performing on electric stage for %d seconds\n",s1,s2,pt);  
            sleep(pt); 
              
            //signal 
            sem_post(&emutex);
            patient[itr]=1;
            printf("\033[1;34m%s %s exited from electric stage\n",s1,s2); 
        }
        else
        {
            //combine performance
            Sf=0;
            pthread_t tid1,tid2,tid3;
            struct s operands[3];

            strcpy(operands[0].name,s1);
            strcpy(operands[0].instrument,s2);
            operands[0].arrival_time=at;
            operands[0].performance_time=pt;

            strcpy(operands[1].name,s1);
            strcpy(operands[1].instrument,s2);
            operands[1].arrival_time=at;
            operands[1].performance_time=pt;

            strcpy(operands[2].name,s1);
            strcpy(operands[2].instrument,s2);
            operands[2].arrival_time=at;
            operands[2].performance_time=pt;
            
            //We have made 3 thread.One will check if singer can perform solo in electric stage or acoustic stage and other will check if singer can join a running performance of a musician
            pthread_create(&tid1,NULL,Sacoustic,(void *)(&operands[0]));
            pthread_create(&tid2,NULL,Selectric,(void *)(&operands[1]));
            pthread_create(&tid3,NULL,Sjoinperformance,(void *)(&operands[2]));

            pthread_join(tid1,NULL);
            pthread_join(tid2,NULL);
            pthread_join(tid3,NULL);
            if(Sf==0)
            {
                printf("\033[1;31m%s %s left due to impatience\n",s1,s2); 
            }
        }
    }
    else
    {
        if(strcmp(s2,"p")==0 || strcmp(s2,"g")==0)    //acoustic and electric
        {       
            if(val1>0)
            {
                //wait
                sem_wait(&amutex);

                //critical section 
                performing[itr]=1;
                printf("\033[1;33m%s %s is performing on acoustic stage for %d seconds\n",s1,s2,pt); 
                sleep(pt);
                if(flag[itr]==1)
                {
                    sleep(2);
                } 
                  
                //signal 
                sem_post(&amutex);
                patient[itr]=1;
                printf("\033[1;34m%s %s exited from acoustic stage\n",s1,s2);
            }
            else if(val2>0)
            {
                //wait
                sem_wait(&emutex);

                //critical section
                performing[itr]=1;
                printf("\033[1;33m%s %s is performing on electric stage for %d seconds\n",s1,s2,pt);  
                sleep(pt); 
                if(flag[itr]==1)
                {
                    sleep(2);
                } 
                  
                //signal 
                sem_post(&emutex);
                patient[itr]=1;
                printf("\033[1;34m%s %s exited from electric stage\n",s1,s2); 
            }   
            else
            {
                f=0;
                pthread_t tid1,tid2;
                struct s operands[2];

                strcpy(operands[0].name,s1);
                strcpy(operands[0].instrument,s2);
                operands[0].arrival_time=at;
                operands[0].performance_time=pt;

                strcpy(operands[1].name,s1);
                strcpy(operands[1].instrument,s2);
                operands[1].arrival_time=at;
                operands[1].performance_time=pt;

                pthread_create(&tid1,NULL,acoustic,(void *)(&operands[0]));
                pthread_create(&tid2,NULL,electric,(void *)(&operands[1]));

                pthread_join(tid1,NULL);
                pthread_join(tid2,NULL);
                if(f==0)
                {
                    printf("\033[1;31m%s %s left due to impatience\n",s1,s2); 
                }
            }
        }
        else if(strcmp(s2,"v")==0) //acoustic
        {
            int var;
            struct timespec tm;
            clock_gettime(CLOCK_REALTIME,&tm);
            tm.tv_sec+=t;

            //wait
            var=sem_timedwait(&amutex,&tm);
            if(var==-1)
            {
                //timeout of the process.It will not occur now.
                printf("\033[1;31m%s %s left due to impatience\n",s1,s2); 
            }
            else
            {
                //critical section 
                performing[itr]=1;
                printf("\033[1;33m%s %s is performing on acoustic stage for %d seconds\n",s1,s2,pt); 
                sleep(pt); 
                if(flag[itr]==1)
                {
                    sleep(2);
                } 
                  
                //signal 
                sem_post(&amutex);
                patient[itr]=1;
                printf("\033[1;34m%s %s exited from acoustic stage\n",s1,s2);
            } 
        }
        else if(strcmp(s2,"b")==0) //electric
        {
            int var;
            struct timespec tm;
            clock_gettime(CLOCK_REALTIME,&tm);
            tm.tv_sec+=t;

            //wait
            var=sem_timedwait(&emutex,&tm);
            if(var==-1)
            {
                //timeout of the process.It will not occur now.
                printf("\033[1;31m%s %s left due to impatience\n",s1,s2); 
            }
            else
            {
                //critical section 
                performing[itr]=1;
                printf("\033[1;33m%s %s is performing on electric stage for %d seconds\n",s1,s2,pt); 
                sleep(pt); 
                if(flag[itr]==1)
                {
                    sleep(2);
                } 
                  
                //signal 
                sem_post(&emutex);
                patient[itr]=1;
                printf("\033[1;34m%s %s exited from electric stage\n",s1,s2);
                performing[itr]=0;
            }       
        }
        performing[itr]=0;
    }
    if(patient[itr]==1 && c!=0)       //tshirt distribution
    {
        //wait
        sem_wait(&tshirt);

        //critical section
        printf("\033[1;32m%s collecting tshirt\n",s1); 
        sleep(2); 
          
        //signal 
        sem_post(&tshirt);
    }
    itr++;
} 

  
int main() 
{
    int pt,at,i;
    char s1[10005]={'\0'},s2[10005]={'\0'};
    scanf("%d %d %d %d %d %d %d",&k,&a,&e,&c,&t1,&t2,&t);
    sem_init(&amutex,0,a);  
    sem_init(&emutex,0,e);
    sem_init(&tshirt,0,c);
    pthread_t thr[k+1];

    struct s operands[k+1];
    for(i=0;i<k;i++)
    {
        scanf("%s %s %d",s1,s2,&at);
        strcpy(nm[i],s1);
        pt=rand()%(t2-t1+1) + t1;

        strcpy(operands[i].name,s1);
        strcpy(operands[i].instrument,s2);
        operands[i].arrival_time=at;
        operands[i].performance_time=pt;

        pthread_create(&thr[i],NULL,thread,(void *)(&operands[i]));
    }
    for(i=0;i<k;i++)
    {
        pthread_join(thr[i],NULL);
    }
    sem_destroy(&amutex);
    sem_destroy(&emutex); 
    sem_destroy(&tshirt);
    printf("\033[1;31mFinished\n");
    return 0; 
} 