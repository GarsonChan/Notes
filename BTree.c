#include <stdio.h>
#include "BTree.h"

void InitBTree(BTree *bt){
    bt->maxNode = m-1;
    bt->minNode = (m/2)-1;
    bt->splitNode = (m+1)/2;
    bt->root = NULL;
}

void SearchNode(BTree bt,int key,Result *r){
    printf("\nsearching node...");
    BTNode *p = bt.root ,*q;
    int found = 0,index;
    while(NULL != p && found == 0){
        index = SearchInNode(*p ,key);
        if(index <= p->keyNum && key == p->key[index])
            found = 1;
        else {
            q = p;
            p = p->child[index-1];
        }
    }
    if(found == 1){
        r->node = p;
        r->flag = 1;
        r->index = index;
    }else {
        r->node = q;
        r->flag = 0;
        r->index = index;
    }
    printf("\nsearching node finished!");
}
int SearchInNode(BTNode p,int key){
    printf("searching in node...");
    int i = 1;
    while(i <= p.keyNum && key > p.key[i])
        i++;
    printf("searching in node finished!");
    return i;
}

void InsertKey(BTree *bt,int key,BTNode *p,int index){
    printf("\ninserting key ...");

    printf("\np的地址:%p" ,p);
    printf("\nbt->root的地址:%p" ,bt->root);

    int n;//过程中记录关键词的变量(包括要插入的关键词和分裂之后需要要插入双亲结点的关键词)
    int splitNode = bt->splitNode;//结点分裂的位置，s=(m+1)/2
    int isFinish = 0;//isFinish判断插入后是否不需要分裂结点就可以完成；
    int newRoot = 0;//判断是否需要生成根结点
    BTNode *newNode = NULL;//分裂出来的结点(不一定用到)
    BTNode *root = bt->root;

    if(root == NULL)AddNewRoot(bt ,NULL ,NULL ,key);
    else {
        n = key;
        while(isFinish == 0 && newRoot == 0){
            InsertKeyInNode(n,p,index,newNode);
            if(p->keyNum < m)
                isFinish = 1;
            else {
                newNode = (BTNode*)malloc(sizeof(BTNode));
                SplitNode(p ,splitNode ,newNode);
                n = p->key[splitNode];
                printf("\nn的值: %d" ,n);
                if(p->parent != NULL){
                    p = p->parent;
                    index = SearchInNode((*p) ,n);
                }
                else
                    newRoot = 1;
            }
        }
    }
    if(newRoot == 1)
        AddNewRoot(bt ,p ,newNode ,n);
    printf("\ninserting key finished!");
}
void AddNewRoot(BTree *bt,BTNode *child1,BTNode *child2,int key){
    printf("\nadd new root...");
    BTNode *root = (BTNode*)malloc(sizeof(BTNode));
    root->key[1] = key;
    root->keyNum = 1;
    root->child[0] = child1;
    root->child[1] = child2;
    root->child[2] = NULL;
    root->child[3] = NULL;
    if(NULL != child1)child1->parent = root;
    if(NULL != child2)child2->parent = root;
    root->parent = NULL;
    bt->root = root;
}
void InsertKeyInNode(int key,BTNode *p,int index,BTNode *newNode){
    printf("\nInsertKeyInNode的p指针: %p",p);
    printf("\nkey为: %d",key);
    int i;
    int keyNum = p->keyNum;
    for(i = keyNum; i >= index; i--){
        p->key[i+1] = p->key[i];
        p->child[i+1] = p->child[i];
    }
    p->key[index] = key;
    p->child[index] = newNode;
    /**
        下面的if else语句块是执行第二次while循环准备的，
        也即分裂后将splitNode的关键字放到双亲结点上，双亲结点的一个child指向分裂出来的newNode
    **/
    if(newNode != NULL)
        newNode->parent = p;
    p->keyNum++;
}
void SplitNode(BTNode *p ,int splitNode ,BTNode *newNode){
    int i,j,k;
    int keyNum = p->keyNum;
    //newNode = (BTNode*)malloc(sizeof(BTNode));

    for(i = 0;i<=m;i++)
        newNode->child[i] = NULL;
    for(i = splitNode + 1,j = 1; i <= keyNum; i++,j++){
        newNode->key[j] = p->key[i];
        newNode->child[j] = p->child[i];
    }
    newNode->keyNum = keyNum - splitNode;
    newNode->parent = p->parent;
    for(i = 0; i <= newNode->keyNum; i++){
        if(newNode->child[i] != NULL)
            newNode->child[i]->parent = newNode;
    }
    p->keyNum = splitNode -1;//需要减1是因为后续操作中splitNode为下标的关键词会被插入到双亲结点中。所以此处先减去1
}
