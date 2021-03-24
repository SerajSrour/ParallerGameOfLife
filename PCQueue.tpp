
template <typename T>
PCQueue<T>::PCQueue(){
    pthread_mutex_init(&mutexx, NULL);
}
template <typename T>
PCQueue<T>::~PCQueue(){
    pthread_mutex_destroy(&mutexx);
}
template <typename T>
T PCQueue<T>::pop() {
    sem.down();
    pthread_mutex_lock(&mutexx);
    T tmp = this->wait_que.front();
    this->wait_que.pop();
    pthread_mutex_unlock(&mutexx);
    return tmp;
}

template <typename T>
void PCQueue<T>::push(const T &item) {
    pthread_mutex_lock(&mutexx);
    this->wait_que.push(item);
    pthread_mutex_unlock(&mutexx);
    sem.up();
}

template<class T>
int PCQueue<T>::size() {
    return wait_que.size();
}
