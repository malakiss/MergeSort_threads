#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define MAX 10000 
typedef struct{
     int *arr;
     int left;
     int right;
}thread_data_t;
void merge(int arr[], int left, int mid, int right) {
    int i, j, k;
    int n1 = mid - left + 1;
    int n2 = right - mid;

    int L[n1], R[n2];
    for (i = 0; i < n1; i++)
        L[i] = arr[left + i];
    for (j = 0; j < n2; j++)
        R[j] = arr[mid + 1 + j];

    i = 0; 
    j = 0; 
    k = left; 

    while (i < n1 && j < n2) {
        if (L[i] <= R[j]) {
            arr[k] = L[i];
            i++;
        } else {
            arr[k] = R[j];
            j++;
        }
        k++;
    }

    while (i < n1) {
        arr[k] = L[i];
        i++;
        k++;
    }

    while (j < n2) {
        arr[k] = R[j];
        j++;
        k++;
    }
}

void* merge_sort(void *arg) {
    thread_data_t *data=(thread_data_t*) arg;
     if (data->left < data->right) {
        int mid = data->left + (data->right - data->left) / 2;
        thread_data_t data1={data->arr,data->left,mid};
        thread_data_t data2={data->arr,mid+1,data->right};
        pthread_t thread2;
        pthread_t thread3;
   
        pthread_create( &thread2,NULL,merge_sort,&data1);
        pthread_create( &thread3,NULL,merge_sort,&data2);
   
        pthread_join(thread2,NULL);
        pthread_join(thread3,NULL);
        
        merge(data->arr, data->left, mid, data->right);
    }
    return NULL;
}

int main() { //create intial thread

    FILE* file = fopen("input", "r");
    if (!file) {
        printf("Error opening file.\n");
        return 1;
    }
    
    int n;
    fscanf(file, "%d", &n);
    int arr[MAX];
    for (int i = 0; i < n; i++) {
        fscanf(file, "%d", &arr[i]); 
    }
    fclose(file);
    
    thread_data_t data={arr,0,n-1};
    pthread_t thread1;
   pthread_create( &thread1,NULL,merge_sort,&data);
    pthread_join(thread1,NULL);
    printf("Sorted array:\n");
    
    for (int i = 0; i < n; i++) {
        printf("%d ", arr[i]);
    }
    printf("\n");

    return 0;
}
