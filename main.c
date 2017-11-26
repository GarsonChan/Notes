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

    while(1){

        printf("\n-------------------��ӭʹ��--------------------");
        printf("\n1.��ʼ��B��       2.����ĳ���ؼ���");
        printf("\n3.���ĳ���ؼ���  4.ɾ��ĳ���ؼ���");
        printf("\n5.�鿴������״    6.�˳�����");
        printf("\n-----------------------------------------------");
        printf("\n������Ҫ�����Ĳ������: ");
        scanf("%d", &select);

        switch(select){

            case 1:
                if(isInit(bt)){
                    printf("\n\nB���Ѿ�����ʼ��,�����ٽ��г�ʼ������\n\n");
                    break;
                }

                InitBTree(&bt);
                printf("\n\n��ʼ�����: \n--------------------------------------------------------------------");
                printf("\n���ĵ�ַ:%p ,                ���ĸ�����ַ:%p;" ,bt.maxNode,bt.minNode);
                printf("\n���Ľ��ؼ��ʵ�������:%d ,���Ľ��ؼ��ʵ���С����:%d;",bt.maxNode,bt.minNode);
                printf("\n--------------------------------------------------------------------\n\n");
                break;

            case 2:
                if(!isInit(bt)){
                    printf("\n\n���ȳ�ʼ�����ٲ��ҹؼ���!\n\n");
                    break;
                }

                printf("\n\n������Ҫ���ҵĹؼ���:");
                scanf("%d" ,&key);
                printf("\n\n���ҽ��: \n--------------------------------------------------------------------");

                SearchNode(bt ,key ,&r);
                if(r.flag == 0)
                    printf("\n��Ǹ������û�иùؼ���!");
                else
                    printf("\n�ؼ������ڽ����ڴ�λ��Ϊ:%p ,�±�Ϊ:%d ",r.node ,r.index);

                printf("\n--------------------------------------------------------------------\n\n");
                break;

            case 3:
                if(!isInit(bt)){
                    printf("\n\n���ȳ�ʼ�����ٲ���ؼ���!\n\n");
                    break;
                }

                printf("\n\n������Ҫ����Ĺؼ���:");
                scanf("%d" ,&key);
                printf("\n\n������: \n--------------------------------------------------------------------");

                SearchNode(bt ,key ,&r);
                if(r.flag == 0){
                    InsertKey(&bt ,key ,r.node,r.index);
                    showBTree(bt.root ,height);
                }
                else
                    printf("\n��Ǹ���������Ѿ��иùؼ��ʣ������ٲ���!");

                printf("\n--------------------------------------------------------------------\n\n");
                break;

            case 4:
                if(!isInit(bt)){
                    printf("\n\n���ȳ�ʼ������ɾ���ؼ���!\n\n");
                    break;
                }

                printf("\n\n������Ҫɾ���Ĺؼ���:");
                scanf("%d" ,&key);
                printf("\n\nɾ�����: \n--------------------------------------------------------------------");

                SearchNode(bt ,key,&r);

                if(r.flag == 0)
                    printf("\n��Ǹ��������û�иùؼ���!");
                else {
                    DeleteKey(&bt ,r.node ,r.index);
                    showBTree(bt.root ,height);
                }
                printf("\n--------------------------------------------------------------------\n\n");
                break;

            case 5:
                if(!isInit(bt)){
                    printf("\n\n���ȳ�ʼ�����ٲ鿴����״!\n\n");
                    break;
                }

                printf("\n\n�鿴���: \n--------------------------------------------------------------------");

                showBTree(bt.root ,height);

                printf("\n--------------------------------------------------------------------\n\n");
                break;

            case 6:
                printf("\n\n-------------------��лʹ��--------------------");
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

