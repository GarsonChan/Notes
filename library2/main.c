#include <stdio.h>
#include "Book.h"

int main()
{
    BookTree bt;
    int select;
    Result r;
    InitBookTree(&bt);

    while(1){

        printf("\n-------------------��ӭʹ��--------------------");
        printf("\n1.�����鼮        2.�����鼮");
        printf("\n3.ɾ���鼮        4.�����鼮");
        printf("\n5.�黹�鼮        6.����ĳ���ߵ��鼮");
        printf("\n7.�˳�����");
        printf("\n-----------------------------------------------");
        printf("\n������Ҫ�����Ĳ������: ");
        scanf("%d", &select);

        switch(select){

            case 1:
                showBook(bt ,&r);
                break;

            case 2:
                addBook(&bt ,&r);
                break;

            case 3:
                deleteBook(&bt ,&r);
                break;

            case 4:
                borrowBook(&bt ,&r);
                break;

            case 5:
                returnBook(&bt ,&r);
                break;

            case 6:
                showBookByAuthor(bt.root);
                break;
            case 7:
                reserveBook(&bt ,&r);
                break;
            case 8:
                printf("\n\n-------------------��лʹ��--------------------");
                return 0;

            default:
                printf("\n��������1~8�Ĳ���\n");
                break;
        }
    }
}


