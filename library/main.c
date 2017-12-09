#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "Book.h"

int main()
{
    BookTree bt;
    int select;
    int key;
    int conti;//判断是否添加库存
    Book book;
    int height = 1;//输出树时表示所在结点的高度
    Result r;

    while(1){

        printf("\n-------------------欢迎使用--------------------");
        printf("\n1.初始化书籍树    2.查找书籍");
        printf("\n3.添加书籍        4.删除书籍");
        printf("\n5.查看树的形状    6.借阅书籍");
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
                    printf("\n\n请先初始化树再查找关键词!\n\n");
                    break;
                }
                showBook(bt ,&r);

                break;

            case 3:
                if(!isInit(bt)){
                    printf("\n\n请先初始化树再插入关键词!\n\n");
                    break;
                }
                addBook(&bt ,&r);
/**
                Book *newBook = (Book*)malloc(sizeof(Book));
                char *newBookName = (char*)malloc(30*sizeof(char));
                char *newBookAuthor = (char*)malloc(30*sizeof(char));

                printf("\n\n请输入要添加书籍的id:");
                scanf("%d" ,&key);

                SearchNode(bt ,key ,&r);

                if(r.flag == 1){
                    printf("\n\n该书号已存在，书名为:%s",r.node->book[r.index]->name);
                    printf("\n你是否要增加该书的库存？");
                    scanf("%d",&conti);
                    switch(conti){
                        case 1:
                             r.node->book[r.index]->presentNum++;
                             r.node->book[r.index]->totalNum++;
                             printf("\n\n%s库存加一\n",r.node->book[r.index]->name);
                        case 0:
                             break;
                    }
                    break;
                }

                newBook->id = key;
                printf("\n\n请输入该书的书名:");
                scanf("%s",newBookName);
                newBook->name = newBookName;
                printf("\n\n请输入该书的作者:");
                scanf("%s",newBookAuthor);
                newBook->author = newBookAuthor;
                newBook->borrowListHead = NULL;

                printf("\n\n添加结果: \n--------------------------------------------------------------------");

                SearchNode(bt ,key ,&r);
                if(r.flag == 0){
                    newBook->presentNum = 1;
                    newBook->totalNum = 1;
                    InsertKey(&bt ,key ,newBook,r.node,r.index);
                    showBookTree(bt.root ,height);
                }
                printf("\n--------------------------------------------------------------------\n\n");
                printf("\n添加成功");
                **/
                break;

            case 4:
                if(!isInit(bt)){
                    printf("\n\n请先初始化树再删除书籍!\n\n");
                    break;
                }

                printf("\n\n请输入要删除的书籍id:");
                scanf("%d" ,&key);
                printf("\n\n删除结果: \n--------------------------------------------------------------------");

                SearchNode(bt ,key,&r);
                if(r.flag == 0)
                    printf("\n抱歉，该树中没有该书籍!");
                else {
                    DeleteKey(&bt ,r.node ,r.index);
                    showBookTree(bt.root ,height);
                }
                printf("\n--------------------------------------------------------------------\n\n");
                break;

            case 5:
                if(!isInit(bt)){
                    printf("\n\n请先初始化树再查看树形状!\n\n");
                    break;
                }

                printf("\n\n查看结果: \n--------------------------------------------------------------------");

                showBookTree(bt.root ,height);

                printf("\n--------------------------------------------------------------------\n\n");
                break;

            case 6:
                if(!isInit(bt)){
                    printf("\n\n请先初始化树再进行操作!\n\n");
                    break;
                }

                int borrowId;
                char *name = (char*)malloc(30*sizeof(char));

                printf("\n\n请输入您的借阅图书账号:");
                scanf("%d" ,&borrowId);

                printf("\n\n请输入您的姓名:");
                scanf("%s" ,name);

                printf("\n\n请输入要借阅的书籍id:");
                scanf("%d" ,&key);

                SearchNode(bt ,key ,&r);

                if(r.flag == 0)
                    printf("\n\n抱歉，没有该书籍");
                else if(r.node->book[r.index]->presentNum < 1)
                    printf("\n\n抱歉，该书籍现存量为0，无法被借阅");
                else {
                    Borrower *borrower = (Borrower*)malloc(sizeof(Borrower));

                    borrower->borrowId = borrowId;
                    borrower->name = name;
                    borrower->next = NULL;

                    time_t now = time(&now)+2592000;
                    struct tm *tm = localtime(&now);
                    borrower->date = tm;

                    borrowBook(r.node ,r.index,borrower);
                    printf("\n借阅成功");
                }
                break;

            case 7:
                printf("\n\n-------------------感谢使用--------------------");
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

