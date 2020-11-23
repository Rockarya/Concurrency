#include<stdio.h>
#include<pthread.h>
#include<stdlib.h>
#include<unistd.h>

int n,m,st,o,f=0,num=0,count=0;  //num is number of students currently .Count is just the number of students avialable for vaccination.
int vaccine[1005][1005];      //batches ranges b/w [1,5]...initialized to 0 already
float prob[10005];

pthread_mutex_t studs,pharma,zones;

typedef struct s
{
    int vac;    //count of number of vaccine in batch
    int index;
} s;

int min(int a,int b)
{
    return (a<b)?a:b;
}

void * pharmaceutical(void * inp)
{
    f=0;
    s* inputs = (s*) inp;
    int w,r,p,i;

    w=rand()%4 + 2;     //random time b/w [2,5] is alloted
    sleep(w);
    r=rand()%5 +1;     //random number of batches b/w [1,5] is alloted

    pthread_mutex_lock(&pharma);
    if(r==0)
    {
        //do nothing
    } 
    else
    {
        printf("Pharmaceutical Company %d is preparing %d batches of vaccines which have success probability %0.2f\n",inputs->index,r,prob[inputs->index]);
        sleep(1);
        for(i=1;i<=r;i++)
        {
            p=rand()%11 +10;        //random capacity b/w [10,20]
            vaccine[inputs->index][i]=p;
        }      
        r=0;
    }
    pthread_mutex_unlock(&pharma);
    printf("Pharmaceutical Company %d has prepared %d batches of vaccines which have success probability %0.2f\n",inputs->index,r,prob[inputs->index]);
    sleep(1);
    if(inputs->index==n)
    {
        printf("All Pharmaceutical Companies have prepared vaccines\n");
        f=1;
    }
}

void* vphase(void* inp)
{
    int var,slots,temp,i;
    s* inputs = (s*) inp;
    var=inputs->vac;        //number of vaccines in the batch(will keep changing)
    while(var>0 && count>0)
    {
        temp=min(8,min(var,count));
        if(temp>=1)
        {
            slots=rand()%temp +1;
        }
        printf("Vaccination Zone %d is ready to vaccinate with %d slots\n",inputs->index,slots);
        sleep(1);
        count-=slots;
        var-=slots;
        pthread_mutex_t mutex;
        pthread_mutex_init(&mutex, NULL);
        printf("Vaccination Zone %d entering Vaccination Phase\n",inputs->index);
        sleep(1);
        pthread_mutex_lock(&mutex);
        if (slots == 0)
        {
            //do nothing
        } 
        else
        {
            printf("%d students got vaccinated at Vaccination Zone %d\n",slots,inputs->index);
            slots=0;
        }
        pthread_mutex_unlock(&mutex);
    }
}

void * vaccination(void * inp)
{
    sleep(2);
    int i,j,flag;
    s* inputs = (s*) inp;
    while(1)
    {
        while(f==0)
        {
            
        }
        if(f==1 && count>0)
        {
            flag=0;
            for(i=1;i<=n;i++)
            {
                for(j=1;j<=5;j++)       //batches can be [1,5] in number
                {
                    if(vaccine[i][j]>0)
                    {
                        printf("Pharmaceutical Company %d is delivering a vaccine batch to Vaccination Zone %d which has success probability %0.2f\n",i,inputs->index,prob[i]);
                        sleep(1);
                        flag=1;
                        break;
                    }
                }
                if(flag==1)
                {
                    break;
                }
            }
            printf("Zone %d received a vaccine batch from Pharmaceutical Company %d\n",inputs->index,i);
            inputs->vac=vaccine[i][j];
            vaccine[i][j]=0;
            vphase(inputs);
            if(count==0 && num==st)
            {
               break; 
            }
            else
            {
                //either vaccines are finished or vaccination zones are busy
                if(flag==1)
                {
                    //vaccination zones are busy.
                    continue;
                }
                else 
                {
                    printf("All the vaccines prepared by Pharmaceutical Companies are emptied. Resuming production now\n");
                    pthread_t tid[n];
                    struct s operand[n];
                    for(i=0;i<n;i++)
                    {    
                        operand[i].index=i+1;
                        pthread_create(&tid[i],NULL,pharmaceutical,(void*)(&operand[i]));
                    }
                    for(i=0;i<n;i++)
                    {
                        pthread_join(tid[i], NULL);
                    }
                }
            }
        }
    }
}

void * students(void * inp)
{
    int k;
    k=rand()%6;         //randomly arriving students in time delay b/w [0,5] seconds
    sleep(k);
    s* inputs = (s*) inp;
    pthread_mutex_lock(&studs);
    if(o==0)
    {
        //do nothing
    } 
    else
    {
        printf("Student %d have arrived\n",inputs->index);
        num++;
        o--;
        count++;
        sleep(1);
        printf("Student %d is waiting to be allocated a slot on a Vaccination Zone\n",inputs->index);
    }
    pthread_mutex_unlock(&studs);
}

int main()
{
    int i;
    scanf("%d %d %d",&n,&m,&st);
    o=st;
    for(i=1;i<=n;i++)
    {
        scanf("%f",&prob[i]);
    }

	pthread_t Student[st],Company[n],Zone[m];
    struct s operands1[st],operands2[n],operands3[m];

    pthread_mutex_init(&studs,NULL);
    pthread_mutex_init(&pharma,NULL);
    pthread_mutex_init(&zones,NULL);
    for(i=0;i<st;i++)
    {
        operands1[i].index=i+1;
        pthread_create(&Student[i],NULL,students,(void*)(&operands1[i]));
    }
    for(i=0;i<n;i++)
    {    
        operands2[i].index=i+1;
        pthread_create(&Company[i],NULL,pharmaceutical,(void*)(&operands2[i]));
    }
    for(i=0;i<m;i++)
    {
        operands3[i].index=i+1;
        pthread_create(&Zone[i],NULL,vaccination,(void*)(&operands3[i]));
    }

    //join waits for the thread with id=tid to finish execution before proceeding
    for(i=0;i<st;i++)
    {
        pthread_join(Student[i], NULL);
    }
    for(i=0;i<n;i++)
    {
        pthread_join(Company[i], NULL);
    }
    for(i=0;i<m;i++)
    {
        pthread_join(Zone[i],NULL);
    }

    pthread_mutex_destroy(&studs);
    pthread_mutex_destroy(&pharma);
    pthread_mutex_destroy(&zones);

    printf("Simulation is over\n");
	return 0;
}