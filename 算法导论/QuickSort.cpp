#include <iostream>
#include <stdlib.h>

using namespace std;

void QuickSort(int arr[],int low,int high)
{
    if(low>=high)
        return;
    int i = low;
    int j = high;
    int k = low + rand()%(high-low);//随机选取一个数
    int key = arr[k];//
    arr[k] = arr[low];//交换到最左
    while(i<j)
    {
        while(i<j && key<= arr[j])//右边找到一个比key大的数
            j--;
        arr[i] = arr[j];
        while(i<j && key>=arr[i])//左边找比key小的数
            i++;
        arr[j] = arr[i];
    }
    arr[i] = key;
    QuickSort(arr,low,i-1);
    QuickSort(arr,i+1,high);
}

int main()
{
    int n,i,*arr;
    cout << "请输入数组大小:";
    cin >> n;
    arr = new int[n];
    for(i=0;i<n;i++)
        cin >> arr[i];
    QuickSort(arr,0,n-1);
    for(i=0;i<n;i++)
        cout << arr[i] <<" ";
    delete arr;
    return 0;
}
