#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <unistd.h>
#include <inttypes.h>
// #include <sys/sem_wait.h>
struct memory {
char buff[100];
int data[10];
};

struct memory* shmptr;

sem_t readsem,writesem;
int readercount=0;
int indx=0;
long data=0;
// int data = 0,readercount = 0;
void *reader(void *arg) {
  while (data==0);
  int s,t;
  // sem_getvalue(&readsem,&t);
  sem_getvalue(&writesem,&s);
  if (s==0)
  {
  printf("Reader %ld can't read, someone is writing\n",(uintptr_t)arg);
  }

  sem_wait(&readsem);
  readercount++;
  // int s;
  // sem_getvalue(&writesem,&s);
  if (readercount==1)
  {
    sem_wait(&writesem);
  }

  sem_post(&readsem);

  printf("Data read by the Reader %ld is %s\n",(uintptr_t)arg,shmptr->buff);
  //to read from array
  // for (int i=0;i<indx;i++)
  // {
  //   printf("Data read by Reader %ld is %d\n",(uintptr_t)arg,shmptr->data[i]);
  // }
  // usleep(100);
  sem_wait(&readsem);
  readercount--;
  if(readercount==0)
  {
   sem_post(&writesem);
  }
  sem_post(&readsem);

  return 0;
}
void *writer(void *arg){
  int s,t;
  // sem_getvalue(&readsem,&t);
  sem_getvalue(&writesem,&s);
  if (s==0)
  {
  printf("Writer %ld can't write, someone is either writing or reading\n",(uintptr_t)arg);
  }



  /* code */
  // sem_post(&readsem);
  // sem_wait(&readsem);
  sem_wait(&writesem);

  sprintf(shmptr->buff,"written by Writer %ld",(uintptr_t)arg);

  printf("%s\n",shmptr->buff);
  data=(uintptr_t)arg;
  shmptr->data[indx]=data;
  indx=(indx+1)%10;

  // usleep(100);


  sem_post(&writesem);
  // sem_post(&readsem);


  return 0;
}
int main()
{
int shmid;
int key = 1234742;
shmid = shmget(key, sizeof(struct memory), IPC_CREAT | 0666);
  shmptr = (struct memory*)shmat(shmid, NULL, 0);
  pthread_t readerid[5],writerid[5];
  int z=sem_init(&readsem,0,1);
  if (z==-1)
  {
    printf("Can't initialise readsemaphore\n");
    exit(0);
  }
  int x=sem_init(&writesem,0,1);
  if (x==-1)
  {
    printf("Can't initialise writesemaphore\n");
    exit(0);
  }
  pthread_create(&writerid[0],NULL,writer,(void *)(size_t)0+1);
  // usleep(100);
  pthread_create(&readerid[0],NULL,reader,(void *)(size_t)0+1);
  for(int i=1;i<=3;i++)
  {
    pthread_create(&writerid[i],NULL,writer,(void *)(size_t)i+1);
    // usleep(100);
    pthread_create(&readerid[i],NULL,reader,(void *)(size_t)i+1);
  }
  for(int i=0;i<=3;i++)
  {
    pthread_join(writerid[i],NULL);
    pthread_join(readerid[i],NULL);
  }
  shmdt(shmptr);
  shmctl(shmid, IPC_RMID, NULL);
  return 0;
}
