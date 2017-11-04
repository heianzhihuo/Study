#include <iostream>

using namespace std;

/*
* Merge(arr,p,q,r)
* arr是一个数组，p、q、r是数组下标，满足p<=q<r
* 该过程假设arr[p...q-1]和arr[q..r-1]都已经排好序
* 他们合并这两个子数组形成单一的已排好序的子数组
* 并代替当前子数组arr[p...r-1]
*/
void Merge(int *arr,int p,int q,int r)
{
    int m,n,i,j,k,*L,*R;
    m = q-p;
    n = r-q;
    L = new int[m];
    R = new int[n];
    for(i=0;i<m;i++)//复制arr[p...q-1]
        L[i] = arr[p+i];
    for(j=0;j<n;j++)//复制arr[q...r-1]
        R[j] = arr[q+j];
    i = j = 0;
    for(k=p;k<r;k++)
    {
        if(i>=m || (j<n && R[j]<=L[i]))//左数组空，或者右数组小
        {
            arr[k] = R[j];
            j++;
        } else //右数组空，或者左数组小
        {
            arr[k] = L[i];
            i++;
        }
    }
}

/*
* MergeSort(arr,p,r)
* 对子数组arr[p..r-1]中的元素进行排序
*/
void MergeSort(int *arr,int p,int r)
{
    if(p<r-1)
    {
        int q = (p+r)/2;
        MergeSort(arr,p,q);
        MergeSort(arr,q,r);
        Merge(arr,p,q,r);
    }
}

int main()
{
    int *arr,i,q,r;
    /*
    cout << "请输入q r:";
    cin >> q >> r;
    arr = new int[r];
    cout << "请输入第一个子数组arr[p...q-1]:";
    for(i=0;i<q;i++)
        cin >> arr[i];
    cout << "请输入第二个子数组arr[q...r-1]:";
    for(;i<r;i++)
        cin >> arr[i];
    Merge(arr,0,q,r);//归并排序*/
    cout << "请输入待排序数组长度:";
    cin >> r;
    arr = new int[r];
    for(i=0;i<r;i++)
        cin >> arr[i];
    MergeSort(arr,0,r);

    for(i=0;i<r;i++)
    {
        cout << arr[i] <<" ";
    }
    delete arr;
    return 0;
}
