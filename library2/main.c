#include "Book.h"

int main()
{
    BookTree bt;
    int select;
    Result r;
    InitBookTree(&bt);
    printf("\n班级：16级网络工程二班");
    printf("\n姓名：陈嘉盛");
    printf("\n学号：3116004946\n");
    while(1){

        printf("\n-------------------欢迎使用--------------------");
        printf("\n1.查找书籍        2.添加书籍");
        printf("\n3.删除书籍        4.借阅书籍");
        printf("\n5.归还书籍        6.查找某作者的书籍");
        printf("\n7.预约书籍        8.退出操作");
        printf("\n-----------------------------------------------");
        printf("\n请输入要操作的操作序号: ");
        fflush(stdin);
        if(scanf("%d", &select)!=1){
            printf("\n1");
            continue;
        }

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



