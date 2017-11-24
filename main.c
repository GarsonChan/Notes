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
        printf("\n\n请输入你要进行的操作编码：");
        scanf("%d", &select);
        switch(select){
            case 1:
                InitBTree(&bt);
                printf("\n树的地址:%p; 树的根结点地址:%p; 树的结点关键词的最大个数:%d; 树的结点关键词的最小个数:%d;" ,&bt,bt.root,bt.maxNode,bt.minNode);
                printf("\n初始化成功");
                break;
            case 2:
                if(bt.maxNode != m-1){
                    printf("请先初始化树再插入关键词!");
                    break;
                }
                printf("请输入关键词:");
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
                    printf("树还未被初始化!无法查看!!!");
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
        printf("\n");//换行
        for(int k = 1; k < height; k++){
            printf("\t");//根据所在的高度加空格
        }
        for(int i = 1;i <= bt->keyNum; i++)
            printf("%d ",bt->key[i]);
    }
    else return;

    height++;//高度加一

    for(int j = 0;j < m;j++){
        if(NULL != bt->child[j]){
            showBTree(bt->child[j] ,height);
        }
    }
}
