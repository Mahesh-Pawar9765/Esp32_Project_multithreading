/*
Question :
Project A1: Multi-threading on ESP32
Start with "hello world" project present in esp-idf
Create 2 threads - A) number_writer B) number_reader
A) number_writer:
	1.This thread will generate 2 random numbers and write those numbers in an array.
	2.Thread will wait till these numbers are used by number_reader thread.
	3.Once these numbers are consumed by number_reader thread, number_writer thread will sleep for 10 seconds
	4.Repeat steps#1-3
B) number_reader:
	1.This thread will read 2 numbers from the above mentioned array.
	2.It will add those numbers and print the sum.
	3.Repeat step#1-2

Solution :
	1.Solve that problem we not need to use mutex.
	2.We use conditional variable for that conditional variable purpose we need to implement mutex.

*/

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
//#include<errno.h>
#include<unistd.h>
#include<pthread.h>
//#include<sys/types.h>

struct input
{
	unsigned long long int i_arr[2];
};

struct output
{
	unsigned long long int i_sum;
};

void* thread_function_number_reader(void*);
void* thread_function_number_writer(void*);

pthread_mutex_t rw_mutex;
pthread_cond_t cond_var = PTHREAD_COND_INITIALIZER;

int main(void)
{
 struct input pI = { 0 };
 pthread_t thread_id1;
 pthread_t thread_id2;
// struct output *pO = NULL;
 
 printf("Enter in main_thread.\n\n");

 while(1)
 {
	 pthread_create(&thread_id1, NULL, thread_function_number_writer, (void*)&pI);
	 pthread_create(&thread_id2, NULL, thread_function_number_reader, (void*)&pI);
	 printf();
	 pthread_join(thread_id1, NULL);
	 pthread_join(thread_id2, NULL);
 }
// printf("Number is %lld %lld \n",pI.i_arr[0], pI.i_arr[1]);
// printf("Sum is %lld\n",pO -> i_sum);
 
// free(pO);
// pO = NULL;
 
 printf("Exit main_thread.\n");

 return 0;
}

void* thread_function_number_writer(void *args)
{
  int i;
  struct input *pI = (struct input*)args;

  pthread_mutex_lock(&rw_mutex);	
  printf("Enter thread_writer_function. thread_id_1 : [%lld].\n", pthread_self());

  for(i = 0; i < 2; i++)
 	pI -> i_arr[i] = rand();
 	
  pthread_cond_wait(&cond_var, &rw_mutex);
  pthread_mutex_unlock(&rw_mutex);
  
  printf("Going sleep for 10 seconds:\n");
  sleep(10);
  printf("Exit thread_writer_function .\n\n");

  return ((void*)pI);
}

void* thread_function_number_reader(void *args)
{
 struct input *pI = (struct input*)args;
 struct output *pO =NULL;
 int i;
 
 pthread_mutex_lock(&rw_mutex);
 
 printf("Enter thread_reader_function : thread_id_2 : [%lld].\n", pthread_self());
 
 pO = (struct output*)malloc(sizeof(struct output));
 if(NULL == pO)
 {
 	printf("Memory allocation failure : malloc");
 	exit(0);
 }
 
 memset(pO, 0, sizeof(struct output));
 for(i =0; i < 2; i++)
   pO -> i_sum = pO -> i_sum + pI -> i_arr[i];
   
 printf("Numbers are %lld %lld \n",pI -> i_arr[0], pI -> i_arr[1]);
 printf("Sum is %lld\n",pO -> i_sum);

 pthread_cond_signal(&cond_var);
 pthread_mutex_unlock(&rw_mutex);
 
 free(pO);
 pO = NULL;
 printf("Exit thread_reader_function .\n");
 
 return ((void*)pO);
}
