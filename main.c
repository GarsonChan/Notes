#include <stdio.h>
#include <stdlib.h>
#include "BTree.h"

int main()
{
    //BTree bt = (BTree)malloc(sizeof(BTNode));
    BTree bt;
    int select;
    int key;
    int height = 1;
    Result r;
    while(1){
        printf("\n\n��������Ҫ���еĲ������룺");
        scanf("%d", &select);
        switch(select){
            case 1:
                InitBTree(&bt);
                printf("\n���ĵ�ַ:%p; ���ĸ�����ַ:%p; ���Ľ��ؼ��ʵ�������:%d; ���Ľ��ؼ��ʵ���С����:%d;" ,&bt,bt.root,bt.maxNode,bt.minNode);
                printf("\n��ʼ���ɹ�");
                break;
            case 2:
                if(bt.maxNode != m-1){
                    printf("���ȳ�ʼ�����ٲ���ؼ���!");
                    break;
                }
                printf("������ؼ���:");
                scanf("%d" ,&key);
                SearchNode(bt ,key ,&r);
                if(r.flag == 0){
                    InsertKey(&bt ,key ,r.node,r.index);
                    break;
                }
                else break;
            case 3:
                for(int i = 1;i <= bt.root->keyNum; i++)
                    printf("%d ",bt.root->key[i]);
                break;
            case 4:
                 if(bt.maxNode != m-1){
                    printf("����δ����ʼ��!�޷��鿴!!!");
                    break;
                }
                showBTree(bt.root ,height);
                break;
            }
    }
    return 0;
}

void showBTree(BTNode *bt ,int height){
    if(NULL != bt){
        printf("\n");//����
        for(int k = 1; k < height; k++){
            printf("\t");//�������ڵĸ߶ȼӿո�
        }
        for(int i = 1;i <= bt->keyNum; i++)
            printf("%d ",bt->key[i]);
    }
    else return;

    height++;//�߶ȼ�һ

    for(int j = 0;j < m;j++){
        if(NULL != bt->child[j]){
            showBTree(bt->child[j] ,height);
        }
    }
}
