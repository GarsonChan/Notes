#include <stdio.h>
#include "Book.h"

int main()
{
    BookTree bt;
    int select;
    Result r;
    InitBookTree(&bt);

    while(1){

        printf("\n-------------------欢迎使用--------------------");
        printf("\n1.查找书籍        2.添加书籍");
        printf("\n3.删除书籍        4.借阅书籍");
        printf("\n5.归还书籍        6.查找某作者的书籍");
        printf("\n7.退出程序");
        printf("\n-----------------------------------------------");
        printf("\n请输入要操作的操作序号: ");
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
                printf("\n\n-------------------感谢使用--------------------");
                return 0;

            default:
                printf("\n请输入编号1~8的操作\n");
                break;
        }
    }
}



