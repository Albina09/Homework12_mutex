#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

#define N 3
#define M 5

pthread_mutex_t mutex;
pthread_cond_t cond;

int shops[M] = {900, 950, 1000, 1050, 1100}; 
int table[M] = {0, 0, 0, 0, 0};

struct pers{
    int person;
    int need;
};

void buy(int p, int j, struct pers *arg){
    printf("-----------------------------------------\n");
    printf("Покупатель %d\n", arg->person);
    printf("Нужда до покупки %d\n", arg->need);
    printf("\n");
    printf("Магазин %d\n",j);
    printf("Продуктов до покупки %d\n", shops[j]);
    shops[j] -= p;
    arg->need -= p;
    printf("\n");
    printf("Нужды после покупки %d\n", arg->need);
    printf("Продуктов после покупки %d\n", shops[j]);
    printf("-----------------------------------------\n");
}

void *sh(void *args){
    struct pers *arg = (struct pers*) args;
    int a, p;
    int j = rand() % M;
    if(arg->need > 0){
    // хочет зайти в магазин
    pthread_mutex_lock(&mutex);
    
    
            
        while (table[j]){
            pthread_cond_wait(&cond, &mutex);
            if(j != 0)
                j--;
            else
                j++;
        }
        table[j] = 1;
            
        if((shops[j] < arg->need) && (arg->need > 0) && (shops[j] > 0))
            p = 1 + rand() % shops[j];
             
        else if((arg->need > 0) && (shops[j] > 0))
            p = 1 + rand() % arg->need; 
    
        buy(p, j, arg);
        sleep(2);
   
        pthread_mutex_unlock(&mutex);

        //освобождает магазин
        pthread_mutex_lock (&mutex);
    
        printf("Покупатель %d свободил магазин %d\n",arg->person,j);
        table[j] = 0;
        pthread_cond_broadcast(&cond);
    
        pthread_mutex_unlock(&mutex);
    }
    
}

void *sklad(void *args){
    int j;

    pthread_mutex_lock(&mutex);

    j = rand() % M;

    while(table[j]){
        j = rand() % M;
    }
    printf("-----------------------------------------\n");
	printf("Количество продуктов в магазине %d было %d\n", j, shops[j]);
    shops[j] += 500;
	printf("Количество продуктов в магазине %d  стало %d, после привоза продуктов\n", j, shops[j]);
    printf("-----------------------------------------\n");
    sleep(1);
        
    pthread_mutex_unlock(&mutex);

}

int main(){
    pthread_t thread[N];
    struct pers people[N];

    for(int i = 0; i < N; i++){
        people[i].person = i;
        people[i].need = 5000;
    }
    pthread_mutex_init(&mutex,NULL);
    
    while(1){

        for(int i = 0; i < N; i++){
            
            if(pthread_create(&thread[i], NULL, sh, (void*)&people[i]) == -1){
                perror("ptread_create");
                exit(EXIT_FAILURE);
            }
        }
        
        if(pthread_create(&thread[N-1], NULL, sklad, NULL) == -1){
            perror("ptread_create");
            exit(EXIT_FAILURE);
        }

        for(int i = 0; i < N; i++){
	            pthread_join(thread[i], NULL);
	    }

        if(people[0].need == 0 && people[1].need == 0 && people[2].need == 0){
            pthread_mutex_destroy(&mutex);
            pthread_cond_destroy(&cond);
            
	        exit(EXIT_SUCCESS);
	    }
        
	    
    }
}
