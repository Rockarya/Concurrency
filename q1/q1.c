#define _POSIX_C_SOURCE 199309L //required for clock
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <wait.h>
#include <limits.h>
#include <fcntl.h>
#include <time.h>
#include <pthread.h>
#include <inttypes.h>
#include <math.h>

void swap(int* a, int* b)
{
    int t = *a;
    *a = *b;
    *b = t;
}

int * shareMem(size_t size)
{
     key_t mem_key = IPC_PRIVATE;
     int shm_id = shmget(mem_key, size, IPC_CREAT | 0666);
     return (int*)shmat(shm_id, NULL, 0);
}

void merge (int *arr, int l, int m, int r)
{
    int i,j,k;  
    int s1=m-l+1;  
    int s2=r-m;  

    //create temp arrays 
    int temp1[s1], temp2[s2];  
    for(i=0;i<s1;i++)  
    {
        temp1[i]=arr[l+i];
    }
    for(j=0;j<s2;j++)  
    { 
        temp2[j]=arr[m+1+j];
    }  
  
    /* Merge the temp arrays back into arr[l..r]*/
    i=0; // Initial index of first subarray  
    j=0; // Initial index of second subarray  
    k=l; // Initial index of merged subarray  
    while(i<s1 && j<s2)
    {  
        if(temp1[i]<=temp2[j]) 
        {  
            arr[k]=temp1[i];  
            i++;  
        }  
        else 
        {  
            arr[k]=temp2[j];  
            j++;  
        }  
        k++;  
    }  
    //Copy the left out elements of array L
    while(i<s1)
    {  
        arr[k]=temp1[i];  
        i++;  
        k++;  
    }  
    // Copy the left out elements of array R 
    while(j<s2)
    {  
        arr[k]=temp2[j];  
        j++;  
        k++;  
    }      
}

void normal_mergeSort(int *arr, int l, int r)
{
     if(l<r)
     {
          if(r-l+1<5)
          {
              //implementing selection sort 
              int i,j,min_idx; 
              for(i=l;i<r;i++) 
              {
                    // Find the minimum element in unsorted array 
                    min_idx=i; 
                    for(j=i+1;j<=r;j++) 
                    {
                         if(arr[j]<arr[min_idx]) 
                         {
                              min_idx=j; 
                         }
                    }
                    swap(&arr[min_idx],&arr[i]); 
              } 
          }
          else
          {
             // Same as (l+r)/2, but avoids overflow for  
             // large l and h  
             int m = l + (r - l) / 2;  
       
             // Sort first and second halves  
             normal_mergeSort(arr, l, m);  
             normal_mergeSort(arr, m + 1, r);  
       
             merge(arr, l, m, r);
          }  
     }
}

void mergeSort(int *arr, int low, int high)
{
     if(low<high)
     {
          if(high-low+1<5)
          {
              //implementing selection sort 
              int i,j,min_idx; 
              for(i=low;i<high;i++) 
              {
                    // Find the minimum element in unsorted array 
                    min_idx=i; 
                    for(j=i+1;j<=high;j++) 
                    {
                         if(arr[j]<arr[min_idx]) 
                         {
                              min_idx=j; 
                         }
                    }
                    swap(&arr[min_idx],&arr[i]); 
              } 
          }
          else
          {
               int m = low + (high - low)/2;
               int pid1=fork();
               int pid2,l,r;
               if(pid1==0)
               {
                    mergeSort(arr, low, m);
                    _exit(1);
               }
               else
               {
                    pid2=fork();
                    if(pid2==0)
                    {
                        
                         mergeSort(arr, m + 1, high);
                         _exit(1);
                    }
                    else
                    {
                         int status;
                         waitpid(pid1, &status, 0);
                         waitpid(pid2, &status, 0);
                    }
               }
               merge(arr,low,m,high);
          }
     }
}

struct arg
{
     int l;
     int r;
     int* arr;
};

void *threaded_mergeSort(void* a)
{
     // note that we are passing a struct to the threads for simplicity.
     struct arg *args = (struct arg*) a;

     int l = args->l;
     int r = args->r;
     int *new = args->arr;

     if(r-l+1<5)
     {
         //implementing selection sort 
         int i,j,min_idx; 
         for(i=l;i<r;i++) 
         {
               // Find the minimum element in unsorted array 
               min_idx=i; 
               for(j=i+1;j<=r;j++) 
               {
                    if(new[j]<new[min_idx]) 
                    {
                         min_idx=j; 
                    }
               }
               swap(&new[min_idx],&new[i]); 
         }  
     }
     else if(l<r)
     {
          int m=l+(r-l)/2;
          //sort left half array
          struct arg a1;
          a1.l = l;
          a1.r = m;
          a1.arr = new;
          pthread_t tid1;     
          pthread_create(&tid1, NULL, threaded_mergeSort, &a1);

          //sort right half array
          struct arg a2;
          a2.l = m+1;           
          a2.r = r;
          a2.arr = new;
          pthread_t tid2;
          pthread_create(&tid2, NULL, threaded_mergeSort, &a2);
     
          //wait for the two halves to get sorted     
          pthread_join(tid1, NULL);
          pthread_join(tid2, NULL);

          merge(new,l,m,r);
     }
}

void runSorts(int n)
{

     struct timespec ts;

     //getting shared memory
     int *array = shareMem(sizeof(int)*(n+1));
     for(int i=0;i<n;i++)
     {
          scanf("%d", array+i);
     }

     int brr[n+1],crr[n+1];
     for(int i=0;i<n;i++) 
     {
          crr[i]=array[i];
          brr[i]=array[i];
     }

     printf("Running concurrent_mergesort for n = %d\n", n);
     clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
     long double st = ts.tv_nsec/(1e9)+ts.tv_sec;

     
     mergeSort(array, 0, n-1);
     for(int i=0; i<n; i++)
     {
          printf("%d ",array[i]);
     }
     printf("\n");
     clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
     long double en = ts.tv_nsec/(1e9)+ts.tv_sec;
     printf("time = %Lf\n", en - st);
     long double t1 = en-st;

     pthread_t tid;
     struct arg a;
     a.l = 0;
     a.r = n-1;
     a.arr = brr;
     printf("Running threaded_concurrent_mergesort for n = %d\n", n);
     clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
     st = ts.tv_nsec/(1e9)+ts.tv_sec;

     //multithreaded mergesort
     pthread_create(&tid, NULL, threaded_mergeSort, &a);
     pthread_join(tid, NULL);
     for(int i=0; i<n; i++)
     {
          printf("%d ",brr[i]);
     }
     printf("\n");
     clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
     en = ts.tv_nsec/(1e9)+ts.tv_sec;
     printf("time = %Lf\n", en - st);
     long double t2 = en-st;

     printf("Running normal_mergesort for n = %d\n", n);
     clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
     st = ts.tv_nsec/(1e9)+ts.tv_sec;

     // normal mergesort
     normal_mergeSort(crr, 0, n-1);
     for(int i=0; i<n; i++)
     {
          printf("%d ",crr[i]);
     }
     printf("\n");
     clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
     en = ts.tv_nsec/(1e9)+ts.tv_sec;
     printf("time = %Lf\n", en - st);
     long double t3 = en - st;

     printf("normal_mergesort ran:\n\t\t[ %Lf ] times faster than concurrent_mergesort\n\t\t[ %Lf ] times faster than threaded_concurrent_mergesort\n", t1/t3, t2/t3);
     shmdt(array);
     return;
}

int main()
{
     int n;
     scanf("%d", &n);
     runSorts(n);
     return 0;
}
