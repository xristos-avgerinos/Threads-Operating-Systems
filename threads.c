#define _GNU_SOURCE
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#define NUMBER_OF_THREADS     10
#define handle_error_en(en, msg) \
   do { errno = en; perror(msg); exit(EXIT_FAILURE); } while (0)

pthread_t threads[NUMBER_OF_THREADS];
pthread_attr_t tattr[NUMBER_OF_THREADS];

static void display_pthread_attr(pthread_attr_t *attr, char *prefix)
{
       int s, i;
       size_t v;
       void *stkaddr;
       struct sched_param sp;

       s = pthread_attr_getdetachstate(attr, &i);
       if (s != 0)
           handle_error_en(s, "pthread_attr_getdetachstate");
       printf("%sDetach state        = %s\n", prefix,
               (i == PTHREAD_CREATE_DETACHED) ? "PTHREAD_CREATE_DETACHED" :
               (i == PTHREAD_CREATE_JOINABLE) ? "PTHREAD_CREATE_JOINABLE" :
               "???");

       s = pthread_attr_getscope(attr, &i);
       if (s != 0)
           handle_error_en(s, "pthread_attr_getscope");
       printf("%sScope               = %s\n", prefix,
               (i == PTHREAD_SCOPE_SYSTEM)  ? "PTHREAD_SCOPE_SYSTEM" :
               (i == PTHREAD_SCOPE_PROCESS) ? "PTHREAD_SCOPE_PROCESS" :
               "???");

       s = pthread_attr_getinheritsched(attr, &i);
       if (s != 0)
           handle_error_en(s, "pthread_attr_getinheritsched");
       printf("%sInherit scheduler   = %s\n", prefix,
               (i == PTHREAD_INHERIT_SCHED)  ? "PTHREAD_INHERIT_SCHED" :
               (i == PTHREAD_EXPLICIT_SCHED) ? "PTHREAD_EXPLICIT_SCHED" :
               "???");

       s = pthread_attr_getschedpolicy(attr, &i);
       if (s != 0)
           handle_error_en(s, "pthread_attr_getschedpolicy");
       printf("%sScheduling policy   = %s\n", prefix,
               (i == SCHED_OTHER) ? "SCHED_OTHER" :
               (i == SCHED_FIFO)  ? "SCHED_FIFO" :
               (i == SCHED_RR)    ? "SCHED_RR" :
               "???");

       s = pthread_attr_getschedparam(attr, &sp);
       if (s != 0)
           handle_error_en(s, "pthread_attr_getschedparam");
       printf("%sScheduling priority = %d\n", prefix, sp.sched_priority);

       s = pthread_attr_getguardsize(attr, &v);
       if (s != 0)
           handle_error_en(s, "pthread_attr_getguardsize");
       printf("%sGuard size          = %ld bytes\n", prefix, v);

       s = pthread_attr_getstack(attr, &stkaddr, &v);
       if (s != 0)
           handle_error_en(s, "pthread_attr_getstack");
       printf("%sStack address       = %p\n", prefix, stkaddr);
       printf("%sStack size          = 0x%lx bytes\n", prefix, v);
}
   
void *print_hello(void *tid)
{
   long i = (long) tid;
   printf("Hello! Greetings from thread %ld! \n", i);
   if(i!=7){
      pthread_exit(NULL);
   }
} 
void *destroy_attr(void *tid)
{
      long i = (long) tid;
      printf("Hello! Greetings from thread %ld!I am destroying thread%ld's attributes and terminating it.\n", i, i-1);
      int status = pthread_attr_destroy(&tattr[i-1]);
      if (status!=0){
         printf("Oops.pthread_attr_destroy returned error code %d \n", status);
         exit(-1);
      }
      pthread_exit(&threads[i-1]);
      
      pthread_exit(NULL);

}

void *gentle_thread(void *tid)
{
   pthread_yield();
   long i = (long) tid;
   printf("Hello! Greetings from thread %ld! I am <gentle> and I granted the execution to all the other threads.That's why I executed last\n", i);
   pthread_exit(NULL);
}

int main (int argc, char *argv[])
{
   int status;
   long i;
   
   
   for(i=0; i<NUMBER_OF_THREADS; i++){
      
      printf("main here: creating thread %ld\n", i);
      
      status = pthread_attr_init(&tattr[i]);
      if (status!=0){
         printf("Oops.pthread_attr_init returned error code %d \n", status);
         exit(-1);
      }
      
      pthread_attr_setguardsize(&tattr[i],4000);
      
      display_pthread_attr(&tattr[i],"\t");
           
      if  ((i<=7)){
          status = pthread_create(&threads[i], &tattr[i], print_hello,(void *)i );
          if (status!=0){
             printf("Oops.pthread_create returned error code %d \n", status);
             exit(-1);
          }
      }
      else if (i==9){
          status = pthread_create(&threads[i], &tattr[i], destroy_attr,(void *)i );
          if (status!=0){
             printf("Oops.pthread_create returned error code %d \n", status);
             exit(-1);
          }
         
      }
      else if (i==8){
      //το thread 8 θα εκτελεστει τελευταιο μιας και υλοποιει την συναρτηση yield
          status = pthread_create(&threads[i], &tattr[i], gentle_thread, (void *)i);
          if (status!=0){
             printf("Oops.pthread_create returned error code %d \n", status);
             exit(-1);
          }
      }
      
      //status = pthread_join(threads[i], NULL);
      
   }
   //Στο Δ ερωτημα με την εντολη ps -l παρατηρω οτι οποιαδηποτε στιγμη και αν εκτελεστει το προγραμμα μου καταναλωνει την ιδια CPU
}
