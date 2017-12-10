//
// Created by wxs on 2017/12/9.
//
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <string.h>
#include "Book.h"

//��ʼ��B��
void InitBookTree(BookTree *bt){
    double min = 3;
    min = min/2;
    bt->maxNode = m-1;
    bt->minNode = ceil(min)-1; //ceil����ȡ������
    bt->splitNode = (m+1)/2; //���÷��ѵ��±�
    bt->root = NULL;
    printf("\n\n��ʼ�����: \n--------------------------------------------------------------------");
    printf("\n���ĵ�ַ:%p ,                ���ĸ�����ַ:%p;" ,bt,bt->root);
    printf("\n���Ľ��ؼ��ʵ�������:%d ,���Ľ��ؼ��ʵ���С����:%f;",bt->maxNode,bt->minNode);
    printf("\n--------------------------------------------------------------------\n\n");
}

//���ҹؼ���
int SearchInNode(BookNode p,int key){
    int i = 1;
    //Ѱ��ĳ�������ĳ���ؼ��ʵ��±�
    while(i <= p.keyNum && key > p.key[i])
        i++;
    return i;
}
void SearchNode(BookTree bt,int key,Result *r){
    BookNode *p = bt.root ,*q = NULL;
    int found = 0,index = 0;
    while(NULL != p && found == 0){
        index = SearchInNode(*p ,key);
        //������ص��±�С�ڹؼ��ʸ������ҵ��ؼ��ʣ���found��Ϊ1
        if(index <= p->keyNum && key == p->key[index])
            found = 1;
            //����pָ�����ĺ��ӽ��
        else {
            q = p;
            p = p->child[index-1];
        }
    }
    if(found == 1){
        r->node = p;
        r->flag = 1;
        r->index = index;
    }else {
        r->node = q;
        r->flag = 0;
        r->index = index;
    }
}

//����ؼ���
void AddNewRoot(BookTree *bt,BookNode *child1,BookNode *child2,int key,Book *book){
    BookNode *root = (BookNode*)malloc(sizeof(BookNode));
    printf("\nroot:%p",root);
    root->key[1] = key;
    root->book[0] = NULL;
    root->book[1] = book;
    root->book[2] = NULL;
    root->book[3] = NULL;
    root->keyNum = 1;
    root->child[0] = child1;
    root->child[1] = child2;
    root->child[2] = NULL;
    root->child[3] = NULL;
    if(NULL != child1)child1->parent = root;
    if(NULL != child2)child2->parent = root;
    root->parent = NULL;
    bt->root = root;
}
void InsertKeyInNode(int key,Book *book ,BookNode *p,int index,BookNode *newNode){
    int i;
    int keyNum = p->keyNum;

    //���ؼ��ʺͺ�������
    for(i = keyNum; i >= index; i--){
        p->key[i+1] = p->key[i];
        p->book[i+1] = p->book[i];
        p->child[i+1] = p->child[i];
    }
    p->key[index] = key;
    p->book[index] = book;
    p->child[index] = newNode;
    /**
        �����if else������ִ�еڶ���whileѭ��׼���ģ�
        Ҳ�����Ѻ�splitNode�Ĺؼ��ַŵ�˫�׽���ϣ�˫�׽���һ��childָ����ѳ�����newNode
    **/
    if(newNode != NULL)
        newNode->parent = p;
    p->keyNum++;
}
void SplitNode(BookNode *p ,int splitNode ,BookNode *newNode){
    int i,j;
    int keyNum = p->keyNum;

    for(i = 0;i <= m;i++)
        newNode->child[i] = NULL;
    for(i = 1;i <= m;i++)
        newNode->book[i] = NULL;

    //�������±����Ĺؼ��ʼ����Ӹ����ѳ����Ľ��
    for(i = splitNode + 1,j = 1; i <= keyNum; i++,j++){
        newNode->key[j] = p->key[i];
        newNode->book[j] = p->book[i];
        //���ѳ��Ľ�����������Ҫָ������±��һλ�ؼ��ʵ�������������
        newNode->child[j] = p->child[i];
        newNode->child[j-1] = p->child[i-1];
    }
    p->book[i] = NULL;
    newNode->keyNum = keyNum - splitNode;
    newNode->parent = p->parent;
    for(i = 0; i <= newNode->keyNum; i++){
        if(newNode->child[i] != NULL)
            newNode->child[i]->parent = newNode;
    }

    p->keyNum = splitNode -1;//��Ҫ��1����Ϊ����������splitNodeΪ�±�Ĺؼ��ʻᱻ���뵽˫�׽���С����Դ˴��ȼ�ȥ1
}
void InsertKey(BookTree *bt,int key,Book *book,BookNode *p,int index){
    int n = 0;//�����м�¼�ؼ��ʵı���(����Ҫ����Ĺؼ��ʺͷ���֮����ҪҪ����˫�׽��Ĺؼ���)
    Book *bookTemp = NULL;
    int splitNode = bt->splitNode;//�����ѵ�λ�ã�s=(m+1)/2
    int isFinish = 0;//isFinish�жϲ�����Ƿ���Ҫ���ѽ��Ϳ�����ɣ�
    int newRoot = 0;//�ж��Ƿ���Ҫ���ɸ����
    BookNode *newNode = NULL;//���ѳ����Ľ��(��һ���õ�)
    BookNode *root = bt->root;

    //�����ʱû�и���㣬�򴴽����
    if(root == NULL)
        AddNewRoot(bt ,NULL ,NULL ,key ,book);
    else {
        n = key;
        bookTemp = book;
        while(isFinish == 0 && newRoot == 0){
            //����ؼ���
            InsertKeyInNode(n,bookTemp,p,index,newNode);
            //��������ؼ��ʸ����ڷ�Χ�ڣ������ɹ�
            if(p->keyNum <= bt->maxNode)//p->keyNum < m
                isFinish = 1;
                //���򣬽��з��Ѳ���
            else {
                newNode = (BookNode*)malloc(sizeof(BookNode));
                printf("\nnewNode:%p",newNode);
                //����
                SplitNode(p ,splitNode ,newNode);

                n = p->key[splitNode];
                bookTemp = p->book[splitNode];
                //�������˫�׽�㲻Ϊ�գ����ڷ����±�Ĺؼ��ʲ���˫�׽����,�ظ�ѭ��
                if(p->parent != NULL){
                    p = p->parent;
                    index = SearchInNode((*p) ,n);
                }
                    //������˫�׽��Ϊ��(����ʱpΪ�����)���򴴽��µĸ����
                else
                    newRoot = 1;
            }
        }
    }
    if(newRoot == 1)
        AddNewRoot(bt ,p ,newNode ,n,bookTemp);
}

//ɾ���ؼ���
BookNode* FindKey(BookNode *p ,int index){

    printf("\nFindKey");

    BookNode *child = p->child[index - 1];
    BookNode *node = p;
    Borrower *next = p->book[index]->borrowListHead ,*temp;

    while(child != NULL){
        node = child;
        child = node->child[child->keyNum];
    }

    p->key[index] = node->key[node->keyNum];

    temp = next;
    while(temp != NULL){
        temp = next;
        if(temp == NULL)
            break;
        next = next->next;
        free(temp);
    }
    printf("\n%p",p->book[index]);
    free(p->book[index]);
    p->book[index] = NULL;
    p->book[index] = node->book[node->keyNum];
    node->book[node->keyNum] = NULL;
    node->key[node->keyNum] = 0;

    printf("\nFindKey end");

    return node;
}
//�ϲ����������ֵܻ����ֵ�
void Merge(BookTree *bt ,BookNode *left, BookNode *right, int index){

    printf("\nMerge");

    int i;
    double min = bt->minNode;
    BookNode *parent = left->parent;

    //��Ϊ���������˳�������Ƚ�˫�׽��Ĺؼ��ʸ�ֵ��������
    //��Ϊkey������±�1��ʼ�洢�����Դ˴��ȼ�һ
    left->key[++left->keyNum] = parent->key[index + 1];
    left->book[left->keyNum] = parent->book[index + 1];

    //���Һ��ӵ����ݴ���������,�������ݵĺϲ�
    memcpy(left->key + left->keyNum + 1,right->key + 1, right->keyNum*sizeof(int));
    memcpy(left->book + left->keyNum + 1,right->book + 1,right->keyNum*sizeof(Book*));
    memcpy(left->child + left->keyNum ,right->child, (right->keyNum+1)*sizeof(BookNode*));
    printf("\nMerge end1");
    //�����ֵܵĺ��ӵ�parentָ��ָ�����ֵ�
    for(i = 0; i <= right->keyNum; i++){
        if(right->child[i] != NULL){
            right->child[i]->parent = left;
        }
    }
    left->keyNum += right->keyNum;//�������ֵܵĹؼ��ʸ���
    printf("\nMerge end2");
    //��Ϊ˫�׽��һ���ؼ��ʺϲ����������ϣ�����˫�׽��Ĺؼ����Լ�����ָ��������
    for(i = index; i < parent->keyNum-1; i++){
        parent->key[i+1] = parent->key[i+2];
        parent->book[i+1] = parent->book[i+2];
        parent->child[i+1] = parent->child[i+2];
    }
    printf("\nMerge end3");
    parent->key[i+1] = 0;
    parent->book[i+1] = NULL;
    parent->child[i+1] = NULL;
    parent->keyNum--;
    printf("\nMerge end4");

    free(right);

    printf("\nMerge end");

    //�����ʱ��˫�׽��ؼ��ʸ���С��min�����˫�׽����кϲ����ؼ��ʲ���
    if(parent->keyNum < min)
        ChooseLeftOrRight(bt ,parent ,parent->parent);
    else
        return;
}
//�ж�Ҫ�����ֵܻ������ֵܽ��в���
void Borrow(BookTree *bt ,BookNode *p ,BookNode *bro, BookNode *parent, int left ,int right,int index){

    printf("\nBorrow");

    //index��ʾ���p��˫�׵Ľ���index������
    int borrowKey;
    int t;//��¼��������ת�ؼ���
    int k,j;
    Book *borrowBook;
    Book *bookTemp;//��¼��������ת�鼮��Ϣ

    //����Ǻ����ֵܽ�
    if(left == 1){
        //����ý����ֵܽ��Ĺؼ��ʸ����ܺͲ��������ֵ,����кϲ�
        if((p->keyNum + bro->keyNum) + 1 <= bt->maxNode){
            index--;
            Merge(bt ,bro ,p ,index);
            return;
        }

        borrowKey = bro->key[bro->keyNum];
        borrowBook = bro->book[bro->keyNum];
        t = parent->key[parent->keyNum];
        bookTemp = parent->book[parent->keyNum];

        //��Ϊ��˳��֮������Ҫ�ճ�λ�ò���ؼ���,������
        for(int i = p->keyNum; i > 0;i--){
            p->key[i+1] = p->key[i];
            p->book[i+1] = p->book[i];
            p->child[i+1] = p->child[i];
        }
        p->child[1] = p->child[0];

        //����ɾ���ؼ������ڵĽ��
        p->key[1] = t;
        p->book[1] = bookTemp;
        p->keyNum++;
        p->child[0] = bro->child[bro->keyNum];
        if(NULL != bro->child[bro->keyNum])
            bro->child[bro->keyNum]->parent = p;

        //����parent
        parent->key[parent->keyNum] = borrowKey;
        parent->book[parent->keyNum] = borrowBook;

        //�������ֵ�
        bro->key[bro->keyNum] = 0;
        bro->book[bro->keyNum] = NULL;
        bro->child[bro->keyNum] = NULL;
        bro->keyNum--;
    }
        //����Ǻ����ֵܽ�
    else {

        //ͬ��
        if((p->keyNum + bro->keyNum) + 1 <= m-1){
            Merge(bt ,p ,bro ,index);
            return;
        }

        borrowKey = bro->key[1];
        borrowBook = bro->book[1];
        t = parent->key[index + 1];
        bookTemp = parent->book[index + 1];

        //����ɾ���ؼ������ڵĽ��
        p->key[p->keyNum + 1] = t;
        p->book[p->keyNum + 1] = bookTemp;
        p->keyNum++;
        p->child[p->keyNum] = bro->child[0];
        if(NULL != bro->child[0])
            bro->child[0]->parent = p;

        //����parent
        parent->key[index + 1] = borrowKey;
        parent->book[index + 1] = borrowBook;

        //�������ֵ�
        for(k = 0,j = 1; k < bro->keyNum; k++,j++){
            bro->key[j] = bro->key[j+1];
            bro->book[j] = bro->book[j+1];
            bro->child[k] = bro->child[k+1];
        }
        bro->child[k] = NULL;
        bro->keyNum--;
    }
    printf("\nMerge end");
}
void ChooseLeftOrRight(BookTree *bt ,BookNode *p ,BookNode *parent){

    printf("\nChoose");

    int index;

    //���p�Ǹ���㣬�򲻱ؽ��кϲ�����
    if(NULL == parent){
        if(p->keyNum == 0){
            if(NULL != p->child[0]){
                bt->root = p->child[0];
                p->child[0]->parent = NULL;
            }else
                bt->root = NULL;
            printf("\n%p",p);
            free(p);
            p = NULL;
        }
        return;
    }

    //���Ҹý������˫�׽��ĵڼ�������
    for(index = 0; index <= parent->keyNum; index++)
        if(parent->child[index] == p)
            break;

    if(index == parent->keyNum)
        //����ý����˫�׽������һ�����ӣ���ô�ͺ��������ֵܽ�
        Borrow(bt ,p ,parent->child[index-1] ,parent,1,0,index);
    else
        //����ͺ����ֵܽ�
        Borrow(bt ,p ,parent->child[index+1] ,parent,0,1,index);
    printf("\nMerge end");

}
void RemoveKey(BookTree *bt,BookNode *p,int index) {

    printf("\nRemove key");

    //ɾ���ؼ��֣�������0(����FindKey�ӿ�)
    p->key[index] = p->key[p->keyNum];
    p->book[index] = p->book[p->keyNum];
    p->key[p->keyNum] = 0;
    p->book[p->keyNum] = NULL;
    p->keyNum--;

    //�����ʱp���Ĺؼ��ʸ���������Сֵ(�ڹؼ��ʸ�����Χ��),��ɾ���ɹ�������
    if(p->keyNum >= bt->minNode)
        return;
        //���򣬽��кϲ����ؼ��ʲ���
    else {
        ChooseLeftOrRight(bt ,p ,p->parent);
    }

    printf("\nRemove end");

}
void DeleteKey(BookTree *bt ,BookNode *p ,int index){

    //����ؼ������ڽ�㲻��Ҷ�ӽ�㣬�򽫹ؼ����滻�����ڽ��������е����ֵ��Ȼ��ɾ�����ֵ���ڵĽ��(��ʱΪҶ�ӽ��)
    if(p->child[index-1] != NULL){
        BookNode *node = FindKey(p ,index);
        DeleteKey(bt ,node ,node->keyNum);
    }//����ֱ��ִ��ɾ������
    else {
        RemoveKey(bt ,p,index);
    }
}

//��ʾ������״
void showBookTree(BookNode *bt ,int height){
    if(NULL != bt){
        printf("\n");//����
        for(int k = 1; k < height; k++){
            printf("\t");//�������ڵĸ߶ȼӿո�
        }
        for(int i = 1;i <= bt->keyNum; i++)
            printf("%d ",bt->key[i]);
    }
    else return;

    height++;//�߶ȼ�һ

    for(int j = 0;j <= bt->keyNum;j++){
        if(NULL != bt->child[j]){
            showBookTree(bt->child[j] ,height);
        }
    }
}

//����鼮
void addBook(BookTree *bt,Result *r){

    int key;
    int conti;
    Book *newBook;
    char *newBookName;
    char *newBookAuthor;

    printf("\n\n������Ҫ����鼮��id:");
    scanf("%d" ,&key);

    SearchNode(*bt ,key ,r);

    if(r->flag == 1){
        printf("\n\n������Ѵ��ڣ�����Ϊ:%s",r->node->book[r->index]->name);
        printf("\n���Ƿ�Ҫ���Ӹ���Ŀ�棿");
        scanf("%d",&conti);
        switch(conti){
            case 1:
                r->node->book[r->index]->presentNum++;
                r->node->book[r->index]->totalNum++;
                printf("\n\n%s����һ\n",r->node->book[r->index]->name);
                break;
            case 0:
                break;
        }
        return;
    }
    else {

        newBook = (Book*)malloc(sizeof(Book));
        printf("\nnewBook:%p",newBook);
        newBookName = (char*)malloc(30*sizeof(char));
        newBookAuthor = (char*)malloc(30*sizeof(char));

        newBook->id = key;
        printf("\n\n��������������:");
        scanf("%s",newBookName);
        newBook->name = newBookName;
        printf("\n\n��������������:");
        scanf("%s",newBookAuthor);
        newBook->author = newBookAuthor;
        newBook->borrowListHead = NULL;

        printf("\n\n��ӽ��: \n--------------------------------------------------------------------");
        newBook->presentNum = 1;
        newBook->totalNum = 1;
        InsertKey(bt ,key ,newBook,r->node,r->index);
        showBookTree(bt->root ,1);
        printf("\n--------------------------------------------------------------------\n\n");
        printf("\n��ӳɹ�");
    }
}

//ɾ���鼮
void deleteBook(BookTree *bt,Result *r){
    int key;

    printf("\n\n������Ҫɾ�����鼮id:");
    scanf("%d" ,&key);
    printf("\n\nɾ�����: \n--------------------------------------------------------------------");

    SearchNode(*bt ,key,r);
    if(r->flag == 0)
        printf("\n��Ǹ��������û�и��鼮!");
    else {
        DeleteKey(bt ,r->node ,r->index);
        showBookTree(bt->root ,1);
    }
    printf("\n--------------------------------------------------------------------\n\n");
}

//�����鼮
void addBorrow(BookNode *p ,int index,Borrower *borrower){
    Book *book = p->book[index];
    Borrower *next = book->borrowListHead;
    Borrower *temp = next;
    while(next != NULL){
        temp = next;
        if(temp->borrowId == borrower->borrowId){
            printf("\n��Ǹ�����ѽ��Ĵ��飬�����ظ�����");
            return;
        }
        next = next->next;
    }
    if(temp == NULL)
        book->borrowListHead = borrower;
    else
        temp->next = borrower;
    book->presentNum--;
    printf("\n���ĳɹ�");
}
void borrowBook(BookTree *bt,Result *r){
    int borrowId,key;
    char *name;
    Borrower *borrower;

    printf("\n\n������Ҫ���ĵ��鼮id:");
    scanf("%d" ,&key);

    SearchNode(*bt ,key ,r);

    if(r->flag == 0)
        printf("\n\n��Ǹ��û�и��鼮");
    else if(r->node->book[r->index]->presentNum < 1)
        printf("\n\n��Ǹ�����鼮�ִ���Ϊ0���޷�������");
    else {
        name = (char*)malloc(30*sizeof(char));
        borrower = (Borrower*)malloc(sizeof(Borrower));
        printf("\nborrower:%p",borrower);
        time_t now = time(&now)+10;

        printf("\n\n���������Ľ���ͼ���˺�:");
        scanf("%d" ,&borrowId);

        printf("\n\n��������������:");
        scanf("%s" ,name);

        borrower->borrowId = borrowId;
        borrower->name = name;
        borrower->next = NULL;
        borrower->date = now;

        addBorrow(r->node ,r->index,borrower);
    }
}

void deleteBorrow(BookNode *p, int index ,char *name ,int borrowId){
    Borrower *head = p->book[index]->borrowListHead;
    Borrower *temp = head ,*t = NULL;

    if(temp == NULL){
        printf("\n��Ǹ�����黹û�б����ĳ�ȥ���޷����й黹");
        return;
    }

    while(temp != NULL){
        if(borrowId == temp->borrowId)
            break;
        else {
            t = temp;
            temp = temp->next;
        }
    }
    if(temp == NULL){
        printf("\n��Ǹ������û�����Ľ�����Ϣ����ȷ�����������ͽ���֤��");
        return;
    } else if(temp == head){
        if(temp->next != NULL)
            p->book[index]->borrowListHead = temp->next;
        else
            p->book[index]->borrowListHead = NULL;
        p->book[index]->presentNum++;
        free(temp);
    } else {
        p->book[index]->presentNum++;
        t->next = temp->next;
        temp->next = NULL;
        free(temp);
    }
    printf("\n�黹�ɹ�");
}

void returnBook(BookTree *bt,Result *r){
    int borrowId,key;
    char *name;

    printf("\n\n������Ҫ�黹���鼮id:");
    scanf("%d" ,&key);

    SearchNode(*bt ,key ,r);

    if(r->flag == 0)
        printf("\n\n��Ǹ��û�и��鼮");
    else {
        name = (char*)malloc(30*sizeof(char));

        printf("\n\n���������Ľ���ͼ���˺�:");
        scanf("%d" ,&borrowId);

        printf("\n\n��������������:");
        scanf("%s" ,name);

        deleteBorrow(r->node,r->index,name,borrowId);
    }
}

//��ʾ�鼮��Ϣ
void showBook(BookTree bt,Result *r){
    Book book;
    int key;
    int i = 1;
    time_t now;
    struct tm *tm;
    Borrower *next;

    printf("\n\n������Ҫ���ҵ����:");
    scanf("%d" ,&key);
    printf("\n\n���ҽ��: \n--------------------------------------------------------------------");

    SearchNode(bt ,key ,r);

    if(r->flag == 0)
        printf("\n��Ǹ������û�и���!");
    else {
        book.id = r->node->book[r->index]->id;
        book.name = r->node->book[r->index]->name;
        book.author = r->node->book[r->index]->author;
        book.presentNum = r->node->book[r->index]->presentNum;
        book.totalNum = r->node->book[r->index]->totalNum;
        book.borrowListHead = r->node->book[r->index]->borrowListHead;
        next = book.borrowListHead;

        printf("\n���:%d,����:%s,����:%s,�ִ���:%d,�ܿ��:%d",book.id,book.author,book.name,book.presentNum,book.totalNum);
        printf("\n\n���鱻���ĵ��������");

        if(next == NULL)
            printf("\n���޽������");
        else {
            now = time(&now);
            printf("\n���ڽ������:");
            while(next != NULL){
                if((next->date-now)<0){
                    tm = localtime(&next->date);
                    printf("\n����%d---����:%s,����֤:%d,�黹����:%d-%d-%d %d:%d:%d",i++,next->name,next->borrowId,tm->tm_year+1900,tm->tm_mon+1,tm->tm_mday,tm->tm_hour,tm->tm_min,tm->tm_sec);
                    next = next->next;
                    continue;
                }else {
                    tm = localtime(&next->date);
                    if(i == 1)
                        printf("\n��ʱ���˽��Ĺ���");

                    printf("\n\n���絽�ڵĽ������:");
                    printf("\n��������:%s,����֤:%d,�黹����:%d-%d-%d %d:%d:%d",next->name,next->borrowId,tm->tm_year+1900,tm->tm_mon+1,tm->tm_mday,tm->tm_hour,tm->tm_min,tm->tm_sec);
                    break;
                }
            }
        }

    }
    printf("\n--------------------------------------------------------------------\n\n");
}


