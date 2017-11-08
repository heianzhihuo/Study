#include <iostream>
#include <stdlib.h>

using namespace std;

/*
* 计数排序，要求待排序数均为非负整数
* 其中A为输入数组，B为输出数组
* len为数组长度，k为待排序数中的最大数
*/
void CountSort(int A[],int B[],int len,int k)
{
    int *C,i;
    C = new int[k+1];
    for(i=0;i<=k;i++)
        C[i] = 0;
    for(i=0;i<len;i++)
        C[A[i]]++;
    //C[i] now is the number of elements equal to i
    for(i=1;i<=k;i++)
        C[i] = C[i]+C[i-1];
    //C[i] now is the number of elements less than or equal to i;
    for(i=len-1;i>=0;i--)
    {
        B[C[A[i]]-1] = A[i];
        C[A[i]]--;
    }
    delete C;
}

/*
* 获取数组arr中的最大数
*/
int getMax(int arr[],int len)
{
    int i,maxn;
    maxn = -1;
    for(i=0;i<len;i++)
        if(arr[i]>maxn)
            maxn = arr[i];
    return maxn;
}

/*
* 获取基数排序的循环次数,基为d
*/
int getLoopTime(int arr[],int len)
{
    int maxn = getMax(arr,len);
    int n = 0;
    while(maxn>0)
    {
        n++;
        maxn = maxn/10;
    }
    return n;
}

/*
* 基数排序
* 基为d
*/
void RadixSort(int arr[],int len)
{
    int d = getLoopTime(arr,len);
    int *count = new int[10];//计数器
    int *temp = new int[len];//
    int radix = 1;//
    int i,j,k;
    for(i=0;i<d;i++)//进行d次排序
    {
        for(j=0;j<10;j++)//每次排序前清空计数器
            count[j] = 0;
        for(j=0;j<len;j++)//统计在该位上某个数字出现的次数
            count[(arr[j]/radix)%10]++;
        for(j=1;j<10;j++)
            count[j] = count[j] + count[j-1];//计算位置
        for(j=len-1;j>=0;j--)
        {
            k = (arr[j]/radix)%10;
            temp[count[k]-1] = arr[j];
            count[k]--;
        }
        for(j=0;j<len;j++)//将临时数组内容复制到arr中
            arr[j] = temp[j];
        radix *= 10;
    }
    delete count;
    delete temp;
}

int main()
{
    int n,i,*arr,*brr,m;
    cout << "请输入数组大小:";
    cin >> n;
    arr = new int[n];
    brr = new int[n];
    for(i=0;i<n;i++)
        cin >> arr[i];
    m = getMax(arr,n);
    CountSort(arr,brr,n,m);
    //RadixSort(arr,n);
    for(i=0;i<n;i++)
        cout << brr[i] <<" ";
    delete arr;
    delete brr;
    return 0;
}
