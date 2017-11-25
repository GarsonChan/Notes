#include <stdio.h>
#include <stdlib.h>
#include "BTree.h"

int main()
{
    BTree bt;
    int select;
    int key;
    int height = 1;//输出树时表示所在结点的高度
    Result r;
    double a = 3;
    int test;

    while(1){
        printf("\n\n请输入你要进行的操作编码：");
        scanf("%d", &select);
        switch(select){
            case 1:
                InitBTree(&bt);
                printf("\n树的地址:%p; \n树的根结点地址:%p; \n树的结点关键词的最大个数:%d; \n树的结点关键词的最小个数:%d;" ,&bt,bt.root,bt.maxNode,bt.minNode);
                printf("\n初始化成功");
                break;
            case 2:
                if(bt.maxNode != m-1){
                    printf("请先初始化树再插入关键词!");
                    break;
                }
                printf("请输入要插入的关键词:");
                scanf("%d" ,&key);
                SearchNode(bt ,key ,&r);
                if(r.flag == 0){
                    InsertKey(&bt ,key ,r.node,r.index);
                    break;
                }
                else break;
            case 3:
                printf("请输入要查找的关键词:");
                scanf("%d" ,&key);
                SearchNode(bt ,key ,&r);
                if(r.flag == 0)
                    printf("\n抱歉，该树没有该关键词");
                else
                    printf("\n关键词所在结点的位置为:%p; \n下标为:%d ",r.node ,r.index);
                break;
            case 4:
                 if(bt.maxNode != m-1){
                    printf("树还未被初始化!无法查看!!!");
                    break;
                }
                showBTree(bt.root ,height);
                break;
            case 5:
                printf("请输入要删除的关键词:");
                scanf("%d" ,&key);
                SearchNode(bt ,key,&r);
                DeleteKey(&bt ,r.node ,r.index);
                break;
        }
    }
    return 0;
}

