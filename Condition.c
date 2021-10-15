#include <pthread.h>
#include <stdio.h>
#define NUM_THREADS 3
#define TCOUNT 10
#define COUNT_LIMIT 12

int count = 0;
int thread_ids[4] = {0, 1, 2, 3};

pthread_mutex_t count_mutex;
pthread_cond_t count_threshold_cv;

void *inc_count(void *idp);
void *watch_count(void *idp);

int main(int argc, char *argv[]){
    int i,rc;

    pthread_t threads[4]; // 스레드 3 개
    pthread_attr_t attr; // mutex와 condition variable 의 attr
    pthread_mutex_init(&count_mutex,NULL); // MUTEX 초기화
    pthread_cond_init(&count_threshold_cv, NULL); // 조건 변수 초기화

    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    pthread_create(&threads[0], &attr, inc_count, (void *) &thread_ids[0]); // int_count 함수 실행 -> threads[0]이 실행 thread 0
    pthread_create(&threads[1], &attr, inc_count, (void *) &thread_ids[1]); // int_count 함수 실행 -> threads[1]이 실행 thread 1
    pthread_create(&threads[2], &attr, inc_count, (void *) &thread_ids[2]); // int_count 함수 실행 -> threads[2]이 실행 thread 2
    pthread_create(&threads[3], &attr, watch_count, (void *) &thread_ids[3]); // watch_count 함수 실행

    for(i = 0; i<NUM_THREADS; i++){
        pthread_join(threads[i], NULL);
    }

    printf("Main(): Waited on %d threads. Done.\n", NUM_THREADS);
}

void *inc_count(void *idp){  // 10회 반복 thread 3개면 30까지 count가 더해짐.
    int i,j;

    double result = 0.0;
    int *my_id = idp;

    for(i=0; i< TCOUNT; i++){
        pthread_mutex_lock(&count_mutex); // 공유 변수 count 접근 시 lock
        count++;

        if(count == COUNT_LIMIT){ // count 가 12가 될 때 실행 
            pthread_cond_signal(&count_threshold_cv); // signal을 보내기 위해 for문 첫 줄에서 lock을 건다.
            printf("inc_count(): thread %d, count= %d Threshold reached.\n", *my_id, count);
        }

        printf("inc_count(): thread %d, count = %d, unlocking mutex\n", *my_id, count);
        pthread_mutex_unlock(&count_mutex); // lock 해제

        for(j=0; j<1000; j++){
            result = result + (double)random();
        }
    }
    pthread_exit(NULL);
}

void *watch_count(void *idp){
    int *my_id = idp;
    printf("Starting watch_count(): thread %d\n", *my_id);
    pthread_mutex_lock(&count_mutex);

    while(count < COUNT_LIMIT){ // count 가 12가 될 때까지 반복
        pthread_cond_wait(&count_threshold_cv, &count_mutex); // signal이 올 때 까지 lock이 걸린 상태로 대기 -> 이 함수 호출되면 자동적으로 mutex를 unlock하고 대기
        printf("watch_count(): thread %d Condition signal recieved.\n", *my_id); // count가 12 되었을 때 signal이 보내져서 printf()가 수행된다.
    }

    pthread_mutex_unlock(&count_mutex);
    pthread_exit(NULL);
}