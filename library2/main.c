#include <stdio.h>
#include "Book.h"

int isInit(BookTree bt){
    if(bt.maxNode != m-1)
        return 0;
    else
        return 1;
}

int main()
{
    BookTree bt;
    int select;
    Result r;

    while(1){

        printf("\n-------------------��ӭʹ��--------------------");
        printf("\n1.��ʼ���鼮��    2.�����鼮");
        printf("\n3.����鼮        4.ɾ���鼮");
        printf("\n5.�����鼮        6.�黹�鼮");
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
                break;

            case 2:
                if(!isInit(bt)){
                    printf("\n\n���ȳ�ʼ�����ٽ��в���!\n\n");
                    break;
                }
                showBook(bt ,&r);

                break;

            case 3:
                if(!isInit(bt)){
                    printf("\n\n���ȳ�ʼ�����ٽ��в���!\n\n");
                    break;
                }
                addBook(&bt ,&r);

                break;

            case 4:
                if(!isInit(bt)){
                    printf("\n\n���ȳ�ʼ�����ٽ��в���!\n\n");
                    break;
                }

                deleteBook(&bt ,&r);

                break;

            case 5:
                if(!isInit(bt)){
                    printf("\n\n���ȳ�ʼ�����ٽ��в���!\n\n");
                    break;
                }
                borrowBook(&bt ,&r);

                break;

            case 6:
                returnBook(&bt ,&r);
                break;

            case 7:
                printf("\n\n-------------------��лʹ��--------------------");
                return 0;
        }
    }
}


