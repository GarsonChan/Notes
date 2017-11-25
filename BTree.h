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
    int maxNode;//ÿ������ŵĹؼ������ĸ���
    int minNode;//ÿ������ŵĹؼ������ٵĸ���
    int splitNode;//���ѵ�λ��
    struct BTNode *root;
}BTree;

typedef struct{
    BTNode *node;
    int index;
    int flag;
}Result;

//��ʼ����
void InitBTree(BTree *bt);
//����ĳ���ؼ���
void SearchNode(BTree bt,int key ,Result *r);
//����ؼ���
void InsertKey(BTree *bt,int key,BTNode *p,int index);
//ɾ���ؼ���
void DeleteKey(BTree *bt ,BTNode *node,int index);
//��ʾ��
void showBTree(BTNode *bt ,int height);

