#include "Semaphore.hpp"


Semaphore::Semaphore() {
    this->value=0;
    pthread_cond_init(&cond, NULL);
    pthread_mutex_init(&mutex, NULL);
}
Semaphore::~Semaphore() {
    pthread_cond_destroy(&cond);
    pthread_mutex_destroy(&mutex);
}

Semaphore::Semaphore(unsigned int val) {
    this->value=val;
    pthread_cond_init(&cond, NULL);
    pthread_mutex_init(&mutex, NULL);
}

void Semaphore::up() {
    pthread_mutex_lock(&mutex);
    value++;
    if(value > 0){//there is resources
        pthread_cond_signal(&cond);//maybe broadcast?
    }
    pthread_mutex_unlock(&mutex);

}

void Semaphore::down() {
    pthread_mutex_lock(&mutex);
    while(value==0){
        pthread_cond_wait(&cond,&mutex);
    }
    value--;
    pthread_mutex_unlock(&mutex);

}