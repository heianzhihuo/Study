#include <iostream>

#define MAX 65536

using namespace std;

typedef struct
{
    int left;//左界
    int right;//右界
    int sum;//和
}SUB;

/*
* FindMaxCrossingSubArray(arr,low,mid,high)
* 在子数组arr[low...high]中找到跨越mid的最大子串
* 其中low<=mid<high,且mid = (low+high)/2
*/
SUB FindMaxCrossingSubArray(int *arr,int low,int mid,int high)
{
    int left_sum,right_sum,sum,i;
    int max_left,max_right;
    left_sum = arr[mid];
    max_left = mid;
    sum = 0;
    for(i = mid;i>=low;i--)
    {
        sum += arr[i];
        if(sum>left_sum)
        {
            left_sum = sum;
            max_left = i;
        }
    }
    right_sum = arr[mid+1];
    max_right = mid+1;
    sum = 0;
    for(i = mid+1;i<=high;i++)
    {
        sum += arr[i];
        if(sum>right_sum)
        {
            right_sum = sum;
            max_right = i;
        }
    }
    SUB res = {max_left,max_right,left_sum+right_sum};
    return res;
}

/*
* FindMaximumSubArray(arr,low,high)
* 在子数组arr[low...high]中找到最大子数组
* 其中low<=high
*/
SUB FindMaximumSubArray(int *arr,int low,int high)
{

    SUB left_sub;
    SUB right_sub;
    SUB cross_sub;
    if(low==high)
    {
        SUB res = {low,high,arr[low]};
        return res;
    }else
    {
        int mid = (low+high)/2;
        left_sub = FindMaximumSubArray(arr,low,mid);
        right_sub = FindMaximumSubArray(arr,mid+1,high);
        cross_sub = FindMaxCrossingSubArray(arr,low,mid,high);
        if(left_sub.sum>=right_sub.sum && left_sub.sum>cross_sub.sum)
            return left_sub;
        else if(right_sub.sum>left_sub.sum && right_sub.sum>cross_sub.sum)
            return right_sub;
        else
            return cross_sub;
    }
}

int main()
{
    int *arr,i,high;
    SUB temp;
    cout <<"请输入数组长度:";
    cin >> high;
    arr = new int[high];
    for(i=0;i<high;i++)
        cin >> arr[i];
    temp = FindMaximumSubArray(arr,0,high-1);
    cout <<"\n最大子数组下标范围:" << temp.left << "-" <<
    temp.right << "\t最大子数组之和:" << temp.sum <<endl;
    delete arr;
    return 0;
}
