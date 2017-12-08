#include <stdio.h>
#include <stdlib.h>
#include "Book.h"

int main()
{
    BookTree bt;
    int select;
    int key;
    int conti;//�ж��Ƿ���ӿ��
    Book book;
    int height = 1;//�����ʱ��ʾ���ڽ��ĸ߶�
    Result r;
    int a[20];
    int b[10];

    while(1){

        printf("\n-------------------��ӭʹ��--------------------");
        printf("\n1.��ʼ���鼮��    2.�����鼮");
        printf("\n3.����鼮        4.ɾ���鼮");
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

                InitBookTree(&bt);
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

                printf("\n\n������Ҫ���ҵ����:");
                scanf("%d" ,&key);
                printf("\n\n���ҽ��: \n--------------------------------------------------------------------");

                SearchNode(bt ,key ,&r);
                if(r.flag == 0)
                    printf("\n��Ǹ������û�и���!");
                else {
                    book.id = r.node->book[r.index]->id;
                    book.name = r.node->book[r.index]->name;
                    book.author = r.node->book[r.index]->author;
                    book.presentNum = r.node->book[r.index]->presentNum;
                    book.totalNum = r.node->book[r.index]->totalNum;
                    printf("\n���:%d,����:%s,����:%s,�ִ���:%d,�ܿ��:%d",book.id,book.author,book.name,book.presentNum,book.totalNum);
                }

                printf("\n--------------------------------------------------------------------\n\n");
                break;

            case 3:
                if(!isInit(bt)){
                    printf("\n\n���ȳ�ʼ�����ٲ���ؼ���!\n\n");
                    break;
                }

                Book *newBook = (Book*)malloc(sizeof(Book));
                char *newBookName = (char*)malloc(30*sizeof(char));
                char *newBookAuthor = (char*)malloc(30*sizeof(char));

                printf("\n\n������Ҫ����鼮��id:");
                scanf("%d" ,&key);

                SearchNode(bt ,key ,&r);

                if(r.flag == 1){
                    printf("\n\n������Ѵ��ڣ�����Ϊ:%s",r.node->book[r.index]->name);
                    printf("\n���Ƿ�Ҫ���Ӹ���Ŀ�棿");
                    scanf("%d",&conti);
                    switch(conti){
                        case 1:
                             r.node->book[r.index]->presentNum++;
                             r.node->book[r.index]->totalNum++;
                             printf("\n\n%s����һ\n",r.node->book[r.index]->name);
                        case 0:
                             break;
                    }
                    break;
                }

                newBook->id = key;
                printf("\n\n��������������:");
                scanf("%s",newBookName);
                newBook->name = newBookName;
                printf("\n\n��������������:");
                scanf("%s",newBookAuthor);
                newBook->author = newBookAuthor;
                printf("\n\n��ӽ��: \n--------------------------------------------------------------------");

                SearchNode(bt ,key ,&r);
                if(r.flag == 0){
                    newBook->presentNum = 1;
                    newBook->totalNum = 1;
                    InsertKey(&bt ,key ,newBook,r.node,r.index);
                    showBookTree(bt.root ,height);
                }
                printf("\n--------------------------------------------------------------------\n\n");
                printf("\n��ӳɹ�");
                break;

            case 4:
                if(!isInit(bt)){
                    printf("\n\n���ȳ�ʼ������ɾ���鼮!\n\n");
                    break;
                }

                printf("\n\n������Ҫɾ�����鼮id:");
                scanf("%d" ,&key);
                printf("\n\nɾ�����: \n--------------------------------------------------------------------");

                SearchNode(bt ,key,&r);
                if(r.flag == 0)
                    printf("\n��Ǹ��������û�и��鼮!");
                else {
                    DeleteKey(&bt ,r.node ,r.index);
                    showBookTree(bt.root ,height);
                }
                printf("\n--------------------------------------------------------------------\n\n");
                break;

            case 5:
                if(!isInit(bt)){
                    printf("\n\n���ȳ�ʼ�����ٲ鿴����״!\n\n");
                    break;
                }

                printf("\n\n�鿴���: \n--------------------------------------------------------------------");

                showBookTree(bt.root ,height);

                printf("\n--------------------------------------------------------------------\n\n");
                break;

            case 6:
                printf("\n\n-------------------��лʹ��--------------------");
                return 0;
        }
    }
    return 0;
}

int isInit(BookTree bt){
    if(bt.maxNode != m-1)
        return 0;
    else
        return 1;
}

