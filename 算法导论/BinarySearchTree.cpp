#include <iostream>
#include <stdlib.h>

using namespace std;

/*
* 搜索二叉树的实现
* 包括SEARCH、MINIMUM、MAXIMUM、
* PREDESCESSOR、SUCCESSOR、INSERT
* 和DELETE等操作
*/

#define NIL NULL

typedef struct node
{
    int key;
    struct node *left;//
    struct node *right;
    struct node *p;//父结点
}Node;

typedef struct tree
{
    Node* root;
}Tree;

/*生成一个键值为k的结点*/
Node* NewNode(int k)
{
    Node* x;
    x = (Node*)malloc(sizeof(Node));
    x->key = k;
    x->left = NIL;
    x->right = NIL;
    x->p = NIL;
    return x;
}

/*中序遍历二叉树*/
void InorderTreeWalk(Node* x)
{
    if(x!=NIL)
    {
        InorderTreeWalk(x->left);
        cout << x->key << " ";
        //printf("%d ",x->key);
        InorderTreeWalk(x->right);
    }
}

/*
* 在二叉搜索树中查找具有给定关键字的结点
* 输入指向根结点的指针x和一个关键字k
* 如果这个结点存在，返回一个指向关键字为k的结点的指针；否则返回NIL
*/
Node* TreeSearch(Node* x,int k)
{
    if(x==NIL || k==x->key)
        return x;
    if(k<x->key)
        return TreeSearch(x->left,k);
    else return TreeSearch(x->right,k);
}

/*循环方式实现上述函数*/
Node* IterativeTreeSearch(Node* x,int k)
{
    while(x!=NIL && k!=x->key)
        if(k<x->key)
            x = x->left;
        else x = x->right;
    return x;
}

/*返回以x为根的子树的最小元素的指针*/
Node* TreeMinimum(Node* x)
{
    while(x->left!=NIL)
        x = x->left;
    return x;
}

/*返回以x为根的子树的最大元素的指针*/
Node* TreeMaximum(Node* x)
{
    while(x->right!=NIL)
        x = x->right;
    return x;
}

/*返回二叉搜索树中结点x的后继，即中序遍历后继*/
Node* TreeSuccessor(Node *x)
{
    if(x->right!=NIL)
        return TreeMinimum(x->right);//右子树最左结点
    Node* y = x->p;
    //从x沿树而上，直到遇到这样一个结点：这个结点是它双亲结点的左孩子
    while(y!=NIL && x==y->right)
    {
        x = y;
        y = x->p;
    }
    return y;
}

/*回二叉搜索树中结点x的前驱，即中序遍历前驱*/
Node* TreePredecessor(Node *x)
{
    if(x->left!=NIL)
        return TreeMaximum((x->left));//左子树最右结点
    Node* y = x->p;
    //从x沿树而上，直到遇到这样一个结点：这个结点是它双亲结点的右孩子
    while(y!=NIL && x==y->left)
    {
        x = y;
        y = x->p;
    }
    return y;
}

/*
* 将新值v插入到二叉搜索树T中
* 结点z为输入，其中z.key = v,z.left=NIL,z.right=NIL,z.p=NIL
*/
void  TreeInsert(Tree &T,Node* z)
{
    Node* y = NIL;
    Node* x = T.root;
    while(x!=NIL)
    {
        y = x;
        if(z->key<x->key)
            x = x->left;
        else x = x->right;
    }
    z->p = y;
    if(y==NIL)
        T.root = z;     //树T为空
    else if(z->key<y->key)
        y->left = z;
    else
        y->right = z;
}

/*
* 用一棵以v为根的子树来替换一棵以u为根的子树
*/
void Transplant(Tree &T,Node* u,Node *v)
{
    if(u->p==NIL)
        T.root = v;
    else if(u==u->p->left)
        u->p->left = v;
    else u->p->right = v;
    if(v!=NIL)
        v->p = u->p;
}

/*
* 删除二叉搜索树T中的结点z
*/
void TreeDelete(Tree &T,Node *z)
{
    if(z->left==NIL)
        Transplant(T,z,z->right);
    else if(z->right==NIL)
        Transplant(T,z,z->left);
    else
    {
        Node* y = TreeMinimum(z->right);
        if(y->p!=z)
        {
            Transplant(T,y,y->right);
            y->right = z->right;
            y->right->p = y;
        }
        Transplant(T,z,y);
        y->left = z->left;
        y->left->p = y;
    }

}

int main()
{
    int n,i,k;
    Node *z;
    Tree T;
    T.root = NIL;
    cout << "请输入待插入的key的个数:";
    cin >> n;
    for(i=0;i<n;i++)
    {
        cin >> k;
        z = NewNode(k);
        TreeInsert(T,z);
    }
    InorderTreeWalk(T.root);
    return 0;
}
