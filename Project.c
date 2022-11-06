#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

#define N 3
#define M 5

pthread_mutex_t mutex;


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

void *purch(void *args){
    struct pers *arg = (struct pers*) args;
    int a, p, j;

    while(arg->need > 0){
        j = rand() % M;
        pthread_mutex_lock(&mutex);

        while (table[j]){
            if(j != 0)
                j--;
            else
                j++;
        }

        if((arg->need > 0) && (shops[j] > 0)){
                
            if((shops[j] < arg->need))
                p = 1 + rand() % shops[j];            
            else 
                p = 1 + rand() % arg->need; 
            buy(p, j, arg);        
            table[j] = 1;

        }else 
            printf("Покупатель %d вошёл в пустой магазин %d\n",arg->person, j);
        pthread_mutex_unlock(&mutex);
        sleep(3);
        printf("Покупатель %d свободил магазин %d\n",arg->person,j);
        table[j] = 0;
            
    }
    
}

void *stock(void *args){
    int j;
    while(1){
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
        
        pthread_mutex_unlock(&mutex);
        sleep(2);
    }
}

int main(){
    pthread_t thread[N], thr_stock;
    struct pers people[N];
    int j = 0; 

    for(int i = 0; i < N; i++){
        people[i].person = i;
        people[i].need = 5000;
    }
    pthread_mutex_init(&mutex,NULL);
    
    for(int i = 0; i < N; i++){
        j = i;
        if(pthread_create(&thread[j], NULL, purch, (void*)&people[j]) == -1){
            perror("ptread_create");
            exit(EXIT_FAILURE);
        }
    }
        
    if(pthread_create(&thr_stock, NULL, stock, NULL) == -1){
        perror("ptread_create");
            exit(EXIT_FAILURE);
    }

    for(int i = 0; i < N; i++)
	        pthread_join(thread[i], NULL);
    
    pthread_cancel(thr_stock);
    pthread_mutex_destroy(&mutex);
    
	exit(EXIT_SUCCESS);   
    
}
