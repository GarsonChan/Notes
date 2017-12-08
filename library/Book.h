#ifndef _Book_H
#define _Book_H
#define m 3
#define NULL (void*)0
#endif // _Book_H

typedef struct BookNode{
    int keyNum;
    int key[m+1];
    struct BookNode *parent;
    struct BookNode *child[m+1];
    struct Book *book[m+1];
}BookNode;

typedef struct BookTree{
    int maxNode;//ÿ������ŵĹؼ������ĸ���
    int minNode;//ÿ������ŵĹؼ������ٵĸ���
    int splitNode;//���ѵ�λ��
    struct BookNode *root;
}BookTree;

typedef struct Borrower{
    int borrowId;//���ĵ�ͼ��֤��
    char *name;//����������
    char *data;//�黹����
}Borrower;

typedef struct Book{
    int id;//���
    char *name;//����
    char *author;//����
    int presentNum;//�ִ���
    int totalNum;//�ܿ��
    Borrower *borrowList;//��������
}Book;

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
