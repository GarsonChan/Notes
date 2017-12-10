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

        printf("\n-------------------欢迎使用--------------------");
        printf("\n1.初始化书籍树    2.查找书籍");
        printf("\n3.添加书籍        4.删除书籍");
        printf("\n5.借阅书籍        6.归还书籍");
        printf("\n-----------------------------------------------");
        printf("\n请输入要操作的操作序号: ");
        scanf("%d", &select);

        switch(select){

            case 1:
                if(isInit(bt)){
                    printf("\n\nB树已经被初始化,请勿再进行初始化操作\n\n");
                    break;
                }
                InitBookTree(&bt);
                break;

            case 2:
                if(!isInit(bt)){
                    printf("\n\n请先初始化树再进行操作!\n\n");
                    break;
                }
                showBook(bt ,&r);

                break;

            case 3:
                if(!isInit(bt)){
                    printf("\n\n请先初始化树再进行操作!\n\n");
                    break;
                }
                addBook(&bt ,&r);

                break;

            case 4:
                if(!isInit(bt)){
                    printf("\n\n请先初始化树再进行操作!\n\n");
                    break;
                }

                deleteBook(&bt ,&r);

                break;

            case 5:
                if(!isInit(bt)){
                    printf("\n\n请先初始化树再进行操作!\n\n");
                    break;
                }
                borrowBook(&bt ,&r);

                break;

            case 6:
                returnBook(&bt ,&r);
                break;

            case 7:
                printf("\n\n-------------------感谢使用--------------------");
                return 0;
        }
    }
}


