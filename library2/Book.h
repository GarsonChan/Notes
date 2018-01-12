//
// Created by Garson on 2017/12/9.
//
#include <time.h>

#ifndef _Book_H
#define _Book_H
#define m 3
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <string.h>
#endif // _Book_H

typedef struct Borrower{
    int borrowId;//���ĵ�ͼ��֤��
    char *name;//����������
    time_t date;//�黹����
    struct Borrower *next;
}Borrower;

typedef struct ReserveInfo{
    Borrower *borrower;//ԤԼ��
    time_t date;//ԤԼʱ��
}ReserveInfo;

typedef struct Book{
    int id;//���
    char *name;//����
    char *author;//����
    int presentNum;//�ִ���
    int totalNum;//�ܿ��
    struct Borrower *borrowListHead;//��������
    struct ReserveInfo *reserveInfo;//ԤԼ��Ϣ
}Book;

typedef struct BookNode{
    int keyNum;
    int key[m+1];
    struct BookNode *parent;
    struct BookNode *child[m+1];
    struct Book *book[m+1];
}BookNode;

typedef struct BookTree{
    int maxNode;//ÿ������ŵĹؼ������ĸ���
    double minNode;//ÿ������ŵĹؼ������ٵĸ���
    int splitNode;//���ѵ�λ��
    struct BookNode *root;
}BookTree;

typedef struct{
    BookNode *node;
    int index;
    int flag;
}Result;

//��ʼ����
void InitBookTree(BookTree *bt);
//����ĳ���ؼ���
void SearchNode(BookTree bt,int key ,Result *r);
//����ؼ���
void InsertKey(BookTree *bt,int key,Book *book,BookNode *p,int index);
//ɾ���ؼ���
void DeleteKey(BookTree *bt ,BookNode *node,int index);
//��ʾ��
void showBookTree(BookNode *bt ,int height);

//����鼮
void addBook(BookTree *bt,Result *r);
//ɾ���鼮
void deleteBook(BookTree *bt,Result *r);
//�����鼮
void borrowBook(BookTree *bt,Result *r);
//�黹�鼮
void returnBook(BookTree *bt,Result *r);
//�鿴�鼮
void showBook(BookTree bt,Result *r);
//����ĳ���ߵ��鼮
void showBookByAuthor(BookNode *bt);
//ԤԼ�鼮
void reserveBook(BookTree *bt,Result *r);