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

    while(1){

        printf("\n-------------------欢迎使用--------------------");
        printf("\n1.初始化B树       2.查找某个关键词");
        printf("\n3.添加某个关键词  4.删除某个关键词");
        printf("\n5.查看树的形状    6.退出操作");
        printf("\n-----------------------------------------------");
        printf("\n请输入要操作的操作序号: ");
        scanf("%d", &select);

        switch(select){

            case 1:
                if(isInit(bt)){
                    printf("\n\nB树已经被初始化,请勿再进行初始化操作\n\n");
                    break;
                }

                InitBTree(&bt);
                printf("\n\n初始化结果: \n--------------------------------------------------------------------");
                printf("\n树的地址:%p ,                树的根结点地址:%p;" ,bt.maxNode,bt.minNode);
                printf("\n树的结点关键词的最大个数:%d ,树的结点关键词的最小个数:%d;",bt.maxNode,bt.minNode);
                printf("\n--------------------------------------------------------------------\n\n");
                break;

            case 2:
                if(!isInit(bt)){
                    printf("\n\n请先初始化树再查找关键词!\n\n");
                    break;
                }

                printf("\n\n请输入要查找的关键词:");
                scanf("%d" ,&key);
                printf("\n\n查找结果: \n--------------------------------------------------------------------");

                SearchNode(bt ,key ,&r);
                if(r.flag == 0)
                    printf("\n抱歉，该树没有该关键词!");
                else
                    printf("\n关键词所在结点的内存位置为:%p ,下标为:%d ",r.node ,r.index);

                printf("\n--------------------------------------------------------------------\n\n");
                break;

            case 3:
                if(!isInit(bt)){
                    printf("\n\n请先初始化树再插入关键词!\n\n");
                    break;
                }

                printf("\n\n请输入要插入的关键词:");
                scanf("%d" ,&key);
                printf("\n\n插入结果: \n--------------------------------------------------------------------");

                SearchNode(bt ,key ,&r);
                if(r.flag == 0){
                    InsertKey(&bt ,key ,r.node,r.index);
                    showBTree(bt.root ,height);
                }
                else
                    printf("\n抱歉，该树中已经有该关键词，请勿再插入!");

                printf("\n--------------------------------------------------------------------\n\n");
                break;

            case 4:
                if(!isInit(bt)){
                    printf("\n\n请先初始化树再删除关键词!\n\n");
                    break;
                }

                printf("\n\n请输入要删除的关键词:");
                scanf("%d" ,&key);
                printf("\n\n删除结果: \n--------------------------------------------------------------------");

                SearchNode(bt ,key,&r);

                if(r.flag == 0)
                    printf("\n抱歉，该树中没有该关键词!");
                else {
                    DeleteKey(&bt ,r.node ,r.index);
                    showBTree(bt.root ,height);
                }
                printf("\n--------------------------------------------------------------------\n\n");
                break;

            case 5:
                if(!isInit(bt)){
                    printf("\n\n请先初始化树再查看树形状!\n\n");
                    break;
                }

                printf("\n\n查看结果: \n--------------------------------------------------------------------");

                showBTree(bt.root ,height);

                printf("\n--------------------------------------------------------------------\n\n");
                break;

            case 6:
                printf("\n\n-------------------感谢使用--------------------");
                return 0;
        }
    }
    return 0;
}

int isInit(BTree bt){
    if(bt.maxNode != m-1)
        return 0;
    else
        return 1;
}

