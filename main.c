#include <stdio.h>
#include <stdlib.h>
#include "BTree.h"

int main()
{
    BTree bt;
    int select;
    int key;
    int height = 1;//�����ʱ��ʾ���ڽ��ĸ߶�
    Result r;
    double a = 3;
    int test;

    while(1){
        printf("\n\n��������Ҫ���еĲ������룺");
        scanf("%d", &select);
        switch(select){
            case 1:
                InitBTree(&bt);
                printf("\n���ĵ�ַ:%p; \n���ĸ�����ַ:%p; \n���Ľ��ؼ��ʵ�������:%d; \n���Ľ��ؼ��ʵ���С����:%d;" ,&bt,bt.root,bt.maxNode,bt.minNode);
                printf("\n��ʼ���ɹ�");
                break;
            case 2:
                if(bt.maxNode != m-1){
                    printf("���ȳ�ʼ�����ٲ���ؼ���!");
                    break;
                }
                printf("������Ҫ����Ĺؼ���:");
                scanf("%d" ,&key);
                SearchNode(bt ,key ,&r);
                if(r.flag == 0){
                    InsertKey(&bt ,key ,r.node,r.index);
                    break;
                }
                else break;
            case 3:
                printf("������Ҫ���ҵĹؼ���:");
                scanf("%d" ,&key);
                SearchNode(bt ,key ,&r);
                if(r.flag == 0)
                    printf("\n��Ǹ������û�иùؼ���");
                else
                    printf("\n�ؼ������ڽ���λ��Ϊ:%p; \n�±�Ϊ:%d ",r.node ,r.index);
                break;
            case 4:
                 if(bt.maxNode != m-1){
                    printf("����δ����ʼ��!�޷��鿴!!!");
                    break;
                }
                showBTree(bt.root ,height);
                break;
            case 5:
                printf("������Ҫɾ���Ĺؼ���:");
                scanf("%d" ,&key);
                SearchNode(bt ,key,&r);
                DeleteKey(&bt ,r.node ,r.index);
                break;
        }
    }
    return 0;
}

