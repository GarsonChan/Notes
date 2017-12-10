//
// Created by wxs on 2017/12/9.
//
#include <time.h>

#ifndef _Book_H
#define _Book_H
#define m 3
#endif // _Book_H

typedef struct Borrower{
    int borrowId;//借阅的图书证号
    char *name;//借阅者姓名
    time_t date;//归还期限
    struct Borrower *next;
}Borrower;

typedef struct Book{
    int id;//书号
    char *name;//书名
    char *author;//作者
    int presentNum;//现存量
    int totalNum;//总库存
    struct Borrower *borrowListHead;//借阅链表
}Book;

typedef struct BookNode{
    int keyNum;
    int key[m+1];
    struct BookNode *parent;
    struct BookNode *child[m+1];
    struct Book *book[m+1];
}BookNode;

typedef struct BookTree{
    int maxNode;//每个结点存放的关键词最多的个数
    double minNode;//每个结点存放的关键词最少的个数
    int splitNode;//分裂点位置
    struct BookNode *root;
}BookTree;

typedef struct{
    BookNode *node;
    int index;
    int flag;
}Result;

//初始化树
void InitBookTree(BookTree *bt);
//查找某个关键词
void SearchNode(BookTree bt,int key ,Result *r);
//插入关键词
void InsertKey(BookTree *bt,int key,Book *book,BookNode *p,int index);
//删除关键词
void DeleteKey(BookTree *bt ,BookNode *node,int index);
//显示树
void showBookTree(BookNode *bt ,int height);

void addBook(BookTree *bt,Result *r);

void deleteBook(BookTree *bt,Result *r);

void borrowBook(BookTree *bt,Result *r);

void returnBook(BookTree *bt,Result *r);

void showBook(BookTree bt,Result *r);

