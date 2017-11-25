#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "BTree.h"

void InitBTree(BTree *bt){

    bt->maxNode = m-1;
    bt->minNode = ceil(m/2)-1; //ceil向上取整函数
    bt->splitNode = (m+1)/2;
    bt->root = NULL;
}
int SearchInNode(BTNode p,int key){
    //printf("searching in node...");
    int i = 1;
    while(i <= p.keyNum && key > p.key[i])
        i++;
    //printf("searching in node finished!");
    return i;
}
void SearchNode(BTree bt,int key,Result *r){
    //printf("\nsearching node...");
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
    //printf("\nsearching node finished!");
}


void AddNewRoot(BTree *bt,BTNode *child1,BTNode *child2,int key){
    //printf("\nadd new root...");
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
   // printf("\nInsertKeyInNode的p指针: %p",p);
    //printf("\nkey为: %d",key);
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
    int i,j;
    int keyNum = p->keyNum;
    //newNode = (BTNode*)malloc(sizeof(BTNode));

    for(i = 0;i<=m;i++)
        newNode->child[i] = NULL;
    for(i = splitNode + 1,j = 1; i <= keyNum; i++,j++){
        newNode->key[j] = p->key[i];
        newNode->child[j] = p->child[i];
        newNode->child[j-1] = p->child[i-1];
    }
    newNode->keyNum = keyNum - splitNode;
    newNode->parent = p->parent;
    for(i = 0; i <= newNode->keyNum; i++){
        if(newNode->child[i] != NULL)
            newNode->child[i]->parent = newNode;
    }
    p->keyNum = splitNode -1;//需要减1是因为后续操作中splitNode为下标的关键词会被插入到双亲结点中。所以此处先减去1
}
void InsertKey(BTree *bt,int key,BTNode *p,int index){
    //printf("\ninserting key ...");

   // printf("\np的地址:%p" ,p);
    //printf("\nbt->root的地址:%p" ,bt->root);

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
               // printf("\nn的值: %d" ,n);
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
    //printf("\ninserting key finished!");
}

BTNode* FindKey(BTNode *p ,int index){

    BTNode *child = p->child[index - 1];
    BTNode *node = p;
    while(child != NULL){
        node = child;
        child = node->child[child->keyNum];
    }
    p->key[index] = node->key[node->keyNum];
    node->key[node->keyNum] = 0;
    return node;
}
void DeleteKey(BTree *bt ,BTNode *p ,int index){

    if(p->child[index-1] != NULL){
        BTNode *node = FindKey(p ,index);
        DeleteKey(bt ,node ,node->keyNum);
    }else {
        RemoveKey(bt ,p);
    }
}
void Borrow(BTree *bt ,BTNode *p ,BTNode *bro, BTNode *parent, int left ,int right,int index){

    //index表示结点p的双亲的结点第index个孩子
   // printf("\np->child[0]:%p",p->child[0]);
    int borrowKey;
    int t;//记录父结点的跳转关键词
    int k,j;

    if(left == 1){
        //是否要加1！！！！
        if((p->keyNum + bro->keyNum) <= m-1){
            Merge(bt ,bro ,p ,index);
            return;
        }

        borrowKey = bro->key[bro->keyNum];
        t = parent->key[parent->keyNum];
        //因为有顺序之分所以要空出位置插入关键词
        for(int i = p->keyNum; i > 0;i--){
            p->key[i+1] = p->key[i];
            p->child[i+1] = p->child[i];
        }
        p->child[1] = p->child[0];

        //处理被删除关键词所在的结点
        p->key[1] = t;
        p->keyNum++;
        p->child[0] = bro->child[bro->keyNum];
        if(NULL != bro->child[bro->keyNum])
            bro->child[bro->keyNum]->parent = p;

        //处理parent
        parent->key[parent->keyNum] = borrowKey;

        //处理左兄弟
        bro->key[bro->keyNum] = 0;
        bro->child[bro->keyNum] = NULL;
        bro->keyNum--;
    }else {
        //是否要加1！！！！！
        if((p->keyNum + bro->keyNum) <= m-1){
            Merge(bt ,p ,bro ,index);
            return;
        }

        borrowKey = bro->key[1];
        t = parent->key[index + 1];
        //处理被删除关键词所在的结点
        p->key[p->keyNum + 1] = t;
        p->keyNum++;
        p->child[p->keyNum] = bro->child[0];
        if(NULL != bro->child[0])
            bro->child[0]->parent = p;

        //处理parent
        parent->key[index + 1] = borrowKey;

        //处理右兄弟
        for(k = 0,j = 1; k < bro->keyNum; k++,j++){
            bro->key[j] = bro->key[j+1];
            bro->child[k] = bro->child[k+1];
        }
        bro->child[k] = NULL;
        bro->keyNum--;
    }

}

void Merge(BTree *bt ,BTNode *left, BTNode *right, int index){
    /*printf("\nright->num:%d",right->keyNum);
    printf("\nleft->child[0]:%p",left->child[0]);
    printf("\nleft:%p" ,left);
    printf("\nright:%p" ,right);
    */
    int i,a,keyNum;
    double min = 3;
    min = min/2;
    min = ceil(min)-1;
    BTNode *parent = left->parent;
    //因为结点数据有顺序，所以先进双亲结点的关键词赋值到左孩子中
    if(index == 0){
        int keyNum = left->keyNum;
        keyNum++;
        left->key[keyNum] = parent->key[index+1];
        left->keyNum++;
    }
    else
        left->key[++left->keyNum] = parent->key[index];//index+1???
    printf("\nleft->child[0]:%p",left->child[0]);
    //将右孩子的数据传到左孩子中,进行数据的合并

    memcpy(left->key + left->keyNum + 1,right->key + 1, right->keyNum*sizeof(int));
    memcpy(left->child + left->keyNum ,right->child, (right->keyNum+1)*sizeof(BTNode*));
    //printf("\nleft->child[0]:%p",left->child[0]);
    //printf("\nright->keyNum:%d",right->keyNum);
    for(i = 0; i <= right->keyNum; i++){
        if(right->child[i] != NULL){
            printf("\nright->child[%d]:%p",i,right->child[i]);
            right->child[i]->parent = left;
        }
    }
    left->keyNum += right->keyNum;
    //printf("\nleft->child[0]:%p",left->child[0]);
    for(i = index; i < parent->keyNum-1; i++){
        parent->key[i] = parent->key[i+1];
        parent->child[i+1] = parent->child[i+2];
    }
    parent->key[i+2] = 0;
    parent->child[i+1] = NULL;
    parent->keyNum--;
    free(right);
    //printf("\nleft->child[0]:%p",left->child[0]);
    if(parent->keyNum < min)
        ChooseLeftOrRight(bt ,parent ,parent->parent);
    return;
}
void ChooseLeftOrRight(BTree *bt ,BTNode *p ,BTNode *parent){
    int index;
    //如果p是根结点，则不必进行合并处理
    if(NULL == parent){
        printf("\n原根结点:%p" ,p);
        if(p->keyNum == 0){
            if(NULL != p->child[0]){
                bt->root = p->child[0];
                p->child[0]->parent = NULL;
                //printf("\nroot->child[0]:%p",bt->root->child[0]);
            }else
                bt->root = NULL;
            free(p);
        }
        return;
    }

    //查找该结点是其双亲结点的第几个孩子
    for(index = 0; index <= parent->keyNum; index++)
        if(parent->child[index] == p)
            break;
    if(index == parent->keyNum)
        //如果该结点是双亲结点的最后一个孩子，那么就和它的左兄弟借
        Borrow(bt ,p ,parent->child[index-1] ,parent,1,0,index);
    else
        //否则就和右兄弟借
        Borrow(bt ,p ,parent->child[index+1] ,parent,0,1,index);
}


void RemoveKey(BTree *bt,BTNode *p){
    int index;
    p->keyNum--;
    double min = 3;
    min = min/2;
    min = ceil(min)-1;
    if(p->keyNum >= min)
        return;
    else {
        ChooseLeftOrRight(bt ,p ,p->parent);
    }
}

void showBTree(BTNode *bt ,int height){
    if(NULL != bt){
        printf("\n");//换行
        for(int k = 1; k < height; k++){
            printf("\t");//根据所在的高度加空格
        }
        for(int i = 1;i <= bt->keyNum; i++)
            printf("%d ",bt->key[i]);
    }
    else return;

    height++;//高度加一

    for(int j = 0;j <= bt->keyNum;j++){
        if(NULL != bt->child[j]){
            showBTree(bt->child[j] ,height);
        }
    }
}
