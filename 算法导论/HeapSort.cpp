#include <iostream>

using namespace std;

/*
* 堆调整过程， 将节点i逐级下降，直到其满足性质
* n为堆大小
*/
void MaxHeapify(int arr[],int i,int len)
{
    int left = 2*i+1;//左子树
    int right = 2*i+2;//右子树
    int maxId = i;//maxId是3个数中最大数的下标
    if(left<len && arr[left]>arr[maxId])
        maxId = left;
    if(right<len && arr[right]>arr[maxId])
        maxId = right;
    if(maxId!=i)//如果需要调整
    {
        swap(arr[maxId],arr[i]);
        MaxHeapify(arr,maxId,len);//递归调整其他不满足堆性质的部分
    }
}

/*
* 堆排序，将数组arr中的数据从小到大排列
*/
void HeapSort(int arr[],int len)
{
    int i;
    for(i = len/2-1;i>=0;i--)
        MaxHeapify(arr,i,len);//对每一个非叶节点进行调整，从而建立最大堆
    for(i=len-1;i>0;i--)
    {
        swap(arr[0],arr[i]);//将堆顶结点交换到堆尾
        MaxHeapify(arr,0,i);//调整堆
    }
}

int main()
{
    int n,i,*arr;
    cout << "请输入数组大小:";
    cin >> n;
    arr = new int[n];
    for(i=0;i<n;i++)
        cin >> arr[i];
    HeapSort(arr,n);
    for(i=0;i<n;i++)
        cout << arr[i] <<" ";
    delete arr;
    return 0;
}
