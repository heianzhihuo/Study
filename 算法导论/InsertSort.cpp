#include <iostream>

using namespace std;

void InsertSort(int *arr,int n)
{
    int i,j,x;
    for(i=1;i<n;i++)
    {
        x = arr[i];
        j = i-1;
        while(j>-1 && arr[j]>x)//寻找正确的位置
        {
            arr[j+1] = arr[j];
            j--;
        }
        arr[j+1] = x;
    }
}

int main()
{
    int n,*arr,i;
    cout << "请输入待排序数组大小:";
    cin >> n;
    arr = new int[n];
    for(i=0;i<n;i++)
    {
        cin >> arr[i];
    }
    InsertSort(arr,n);//排序
    for(i=0;i<n;i++)
    {
        cout << arr[i] <<" ";
    }
    return 0;
}
