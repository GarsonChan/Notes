#ifndef _BTREE_H
#define _BTREE_H
#define m double
#define m 3
#define NULL (void*)0
#endif // _BTREE_H

typedef struct BTNode{
    int keyNum;
    int key[m+1];
    struct BTNode *parent;
    struct BTNode *child[m+1];
}BTNode;

typedef struct BTree{
    int maxNode;//每个结点存放的关键词最多的个数
    int minNode;//每个结点存放的关键词最少的个数
    int splitNode;//分裂点位置
    struct BTNode *root;
}BTree;

typedef struct{
    BTNode *node;
    int index;
    int flag;
}Result;

//初始化树
void InitBTree(BTree *bt);
//查找某个关键词
void SearchNode(BTree bt,int key ,Result *r);
//插入关键词
void InsertKey(BTree *bt,int key,BTNode *p,int index);
//删除关键词
void DeleteKey(BTree *bt ,BTNode *node,int index);
//显示树
void showBTree(BTNode *bt ,int height);

