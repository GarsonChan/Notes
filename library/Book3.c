#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include "Book.h"

//初始化B树
void InitBookTree(BookTree *bt){
    double min = 3;
    min = min/2;
    bt->maxNode = m-1;
    bt->minNode = ceil(min)-1; //ceil向上取整函数
    bt->splitNode = (m+1)/2; //设置分裂点下标
    bt->root = NULL;
    printf("\n\n初始化结果: \n--------------------------------------------------------------------");
    printf("\n树的地址:%p ,                树的根结点地址:%p;" ,bt->maxNode,bt->minNode);
    printf("\n树的结点关键词的最大个数:%d ,树的结点关键词的最小个数:%d;",bt->maxNode,bt->minNode);
    printf("\n--------------------------------------------------------------------\n\n");
}

//查找关键词
int SearchInNode(BookNode p,int key){
    int i = 1;
    //寻找某个结点中某个关键词的下标
    while(i <= p.keyNum && key > p.key[i])
        i++;
    return i;
}
void SearchNode(BookTree bt,int key,Result *r){
    BookNode *p = bt.root ,*q;
    int found = 0,index;
    while(NULL != p && found == 0){
        index = SearchInNode(*p ,key);
        //如果返回的下标小于关键词个数且找到关键词，则将found置为1
        if(index <= p->keyNum && key == p->key[index])
            found = 1;
        //否则将p指向它的孩子结点
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

//插入关键词
void AddNewRoot(BookTree *bt,BookNode *child1,BookNode *child2,int key,Book *book){
    BookNode *root = (BookNode*)malloc(sizeof(BookNode));
    root->key[1] = key;
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

    //结点关键词和孩子右移
    for(i = keyNum; i >= index; i--){
        p->key[i+1] = p->key[i];
        p->book[i+1] = p->book[i];
        p->child[i+1] = p->child[i];
    }
    p->key[index] = key;
    p->book[index] = book;
    p->child[index] = newNode;
    /**
        下面的if else语句块是执行第二次while循环准备的，
        也即分裂后将splitNode的关键字放到双亲结点上，双亲结点的一个child指向分裂出来的newNode
    **/
    if(newNode != NULL)
        newNode->parent = p;
    p->keyNum++;
}
void SplitNode(BookNode *p ,int splitNode ,BookNode *newNode){
    int i,j;
    int keyNum = p->keyNum;

    for(i = 0;i<=m;i++)
        newNode->child[i] = NULL;
    //将分裂下标后面的关键词及孩子给分裂出来的结点
    for(i = splitNode + 1,j = 1; i <= keyNum; i++,j++){
        newNode->key[j] = p->key[i];
        newNode->book[j] = p->book[i];
        //分裂出的结点的两个孩子要指向分裂下标后一位关键词的左右两个孩子
        newNode->child[j] = p->child[i];
        newNode->child[j-1] = p->child[i-1];
    }
    newNode->keyNum = keyNum - splitNode;
    newNode->parent = p->parent;
    for(i = 0; i <= newNode->keyNum; i++){
        if(newNode->child[i] != NULL)
            newNode->child[i]->parent = newNode;
    }

    p->keyNum = splitNode -1;//需要减1是因为后续操作中splitNode为下标的关键词会被插入到双亲结点中。所以此处先减去1
}
void InsertKey(BookTree *bt,int key,Book *book,BookNode *p,int index){
    int n;//过程中记录关键词的变量(包括要插入的关键词和分裂之后需要要插入双亲结点的关键词)
    Book *bookTemp;
    int splitNode = bt->splitNode;//结点分裂的位置，s=(m+1)/2
    int isFinish = 0;//isFinish判断插入后是否不需要分裂结点就可以完成；
    int newRoot = 0;//判断是否需要生成根结点
    BookNode *newNode = NULL;//分裂出来的结点(不一定用到)
    BookNode *root = bt->root;

    //如果此时没有根结点，则创建结点
    if(root == NULL)
        AddNewRoot(bt ,NULL ,NULL ,key ,book);
    else {
        n = key;
        bookTemp = book;
        while(isFinish == 0 && newRoot == 0){
            //插入关键词
            InsertKeyInNode(n,bookTemp,p,index,newNode);
            //如果插入后关键词个数在范围内，则插入成功
            if(p->keyNum <= bt->maxNode)//p->keyNum < m
                isFinish = 1;
            //否则，进行分裂操作
            else {
                newNode = (BookNode*)malloc(sizeof(BookNode));
                //分裂
                SplitNode(p ,splitNode ,newNode);

                n = p->key[splitNode];
                bookTemp = p->book[splitNode];
                //如果结点的双亲结点不为空，则将在分裂下标的关键词插入双亲结点内,重复循环
                if(p->parent != NULL){
                    p = p->parent;
                    index = SearchInNode((*p) ,n);
                }
                //如果结点双亲结点为空(即此时p为根结点)，则创建新的根结点
                else
                    newRoot = 1;
            }
        }
    }
    if(newRoot == 1)
        AddNewRoot(bt ,p ,newNode ,n,bookTemp);
}

//删除关键词
BookNode* FindKey(BookNode *p ,int index){

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

    free(p->book[index]);
    p->book[index] = node->book[node->keyNum];
    node->book[node->keyNum] = NULL;
    node->key[node->keyNum] = 0;
    return node;
}
//合并结点和其左兄弟或右兄弟
void Merge(BookTree *bt ,BookNode *left, BookNode *right, int index){

    int i,min;
    min = bt->minNode;
    BookNode *parent = left->parent;

    //因为结点数据有顺序，所以先进双亲结点的关键词赋值到左孩子中
    //因为key数组从下标1开始存储，所以此处先加一
    left->key[++left->keyNum] = parent->key[index + 1];
    left->book[left->keyNum] = parent->book[index + 1];

    //将右孩子的数据传到左孩子中,进行数据的合并
    memcpy(left->key + left->keyNum + 1,right->key + 1, right->keyNum*sizeof(int));
    memcpy(left->book + left->keyNum + 1,right->book + 1,right->keyNum*sizeof(Book));
    memcpy(left->child + left->keyNum ,right->child, (right->keyNum+1)*sizeof(BookNode*));

    //将右兄弟的孩子的parent指针指向左兄弟
    for(i = 0; i <= right->keyNum; i++){
        if(right->child[i] != NULL){
            right->child[i]->parent = left;
        }
    }
    left->keyNum += right->keyNum;//增加左兄弟的关键词个数

    //因为双亲结点一个关键词合并到了左孩子上，所以双亲结点的关键词以及孩子指针往左移
    for(i = index; i < parent->keyNum-1; i++){
        parent->key[i+1] = parent->key[i+2];
        parent->book[i+1] = parent->book[i+2];
        parent->child[i+1] = parent->child[i+2];
    }
    parent->key[i+1] = 0;
    parent->book[i+1] = NULL;
    parent->child[i+1] = NULL;
    parent->keyNum--;
    free(right);

    //如果此时的双亲结点关键词个数小于min，则对双亲结点进行合并或借关键词操作
    if(parent->keyNum < min)
        ChooseLeftOrRight(bt ,parent ,parent->parent);
    else
        return;
}
//判断要和左兄弟还是右兄弟进行操作
void Borrow(BookTree *bt ,BookNode *p ,BookNode *bro, BookNode *parent, int left ,int right,int index){

    //index表示结点p的双亲的结点第index个孩子
    int borrowKey;
    int t;//记录父结点的跳转关键词
    int k,j;
    Book *borrowBook;
    Book *bookTemp;//记录父结点的跳转书籍信息

    //如果是和左兄弟借
    if(left == 1){
        //如果该结点和兄弟结点的关键词个数总和不超过最大值,则进行合并
        if((p->keyNum + bro->keyNum) + 1 <= bt->maxNode){
            index--;
            Merge(bt ,bro ,p ,index);
            return;
        }

        borrowKey = bro->key[bro->keyNum];
        borrowBook = bro->book[bro->keyNum];
        t = parent->key[parent->keyNum];
        bookTemp = parent->book[parent->keyNum];

        //因为有顺序之分所以要空出位置插入关键词,往右移
        for(int i = p->keyNum; i > 0;i--){
            p->key[i+1] = p->key[i];
            p->book[i+1] = p->book[i];
            p->child[i+1] = p->child[i];
        }
        p->child[1] = p->child[0];

        //处理被删除关键词所在的结点
        p->key[1] = t;
        p->book[1] = bookTemp;
        p->keyNum++;
        p->child[0] = bro->child[bro->keyNum];
        if(NULL != bro->child[bro->keyNum])
            bro->child[bro->keyNum]->parent = p;

        //处理parent
        parent->key[parent->keyNum] = borrowKey;
        parent->book[parent->keyNum] = borrowBook;

        //处理左兄弟
        bro->key[bro->keyNum] = 0;
        bro->book[bro->keyNum] = NULL;
        bro->child[bro->keyNum] = NULL;
        bro->keyNum--;
    }
    //如果是和右兄弟借
    else {

        //同上
        if((p->keyNum + bro->keyNum) + 1 <= m-1){
            Merge(bt ,p ,bro ,index);
            return;
        }

        borrowKey = bro->key[1];
        borrowBook = bro->book[1];
        t = parent->key[index + 1];
        bookTemp = parent->book[index + 1];

        //处理被删除关键词所在的结点
        p->key[p->keyNum + 1] = t;
        p->book[p->keyNum + 1] = bookTemp;
        p->keyNum++;
        p->child[p->keyNum] = bro->child[0];
        if(NULL != bro->child[0])
            bro->child[0]->parent = p;

        //处理parent
        parent->key[index + 1] = borrowKey;
        parent->book[index + 1] = borrowBook;

        //处理右兄弟
        for(k = 0,j = 1; k < bro->keyNum; k++,j++){
            bro->key[j] = bro->key[j+1];
            bro->book[j] = bro->book[j+1];
            bro->child[k] = bro->child[k+1];
        }
        bro->child[k] = NULL;
        bro->keyNum--;
    }
    return;
}
void ChooseLeftOrRight(BookTree *bt ,BookNode *p ,BookNode *parent){

    int index;

    //如果p是根结点，则不必进行合并处理
    if(NULL == parent){
        if(p->keyNum == 0){
            if(NULL != p->child[0]){
                bt->root = p->child[0];
                p->child[0]->parent = NULL;
            }else
                bt->root = NULL;
            free(p);
        }
        return;
    }

    //查找该结点是其双亲结点的第几个孩子
    for(index = 0; index <= parent->keyNum; index++)
        if(parent->child[index] == p)
            break;

    if(index == parent->keyNum)
        //如果该结点是双亲结点的最后一个孩子，那么就和它的左兄弟借
        Borrow(bt ,p ,parent->child[index-1] ,parent,1,0,index);
    else
        //否则就和右兄弟借
        Borrow(bt ,p ,parent->child[index+1] ,parent,0,1,index);
}
void RemoveKey(BookTree *bt,BookNode *p,int index){

    //删除关键字，可能是0(经过FindKey接口)
    p->key[index] = p->key[p->keyNum];
    p->book[index] = p->book[p->keyNum];
    p->key[p->keyNum] = 0;
    p->keyNum--;

    //如果此时p结点的关键词个数大于最小值(在关键词个数范围内),则删除成功，返回
    if(p->keyNum >= bt->minNode)
        return;
    //否则，进行合并或借关键词操作
    else {
        ChooseLeftOrRight(bt ,p ,p->parent);
    }
}
void DeleteKey(BookTree *bt ,BookNode *p ,int index){

    //如果关键词所在结点不是叶子结点，则将关键词替换该所在结点的子树中的最大值，然后删除最大值所在的结点(此时为叶子结点)
    if(p->child[index-1] != NULL){
        BookNode *node = FindKey(p ,index);
        DeleteKey(bt ,node ,node->keyNum);
    }//否则直接执行删除操作
    else {
        RemoveKey(bt ,p,index);
    }
}

//显示树的形状
void showBookTree(BookNode *bt ,int height){
    if(NULL != bt){
        printf("\n");//换行
        for(int k = 1; k < height; k++){
            printf("\t");//根据所在的高度加空格
        }
        for(int i = 1;i <= bt->keyNum; i++)
            printf("%d ",bt->key[i]);
    }
    else return;

    height++;//高度加一

    for(int j = 0;j <= bt->keyNum;j++){
        if(NULL != bt->child[j]){
            showBookTree(bt->child[j] ,height);
        }
    }
}

void addBook(BookTree *bt,Result *r){

    int key;
    int conti;
    Book *newBook;
    char *newBookName;
    char *newBookAuthor;

    printf("\n\n请输入要添加书籍的id:");
    scanf("%d" ,&key);

    SearchNode(*bt ,key ,r);

    if(r->flag == 1){
        printf("\n\n该书号已存在，书名为:%s",r->node->book[r->index]->name);
        printf("\n你是否要增加该书的库存？");
        scanf("%d",&conti);
        switch(conti){
            case 1:
                 r->node->book[r->index]->presentNum++;
                 r->node->book[r->index]->totalNum++;
                 printf("\n\n%s库存加一\n",r->node->book[r->index]->name);
            case 0:
                 break;
        }
        return;
    }
    else {

        newBook = (Book*)malloc(sizeof(Book));
        newBookName = (char*)malloc(30*sizeof(char));
        newBookAuthor = (char*)malloc(30*sizeof(char));

        newBook->id = key;
        printf("\n\n请输入该书的书名:");
        scanf("%s",newBookName);
        newBook->name = newBookName;
        printf("\n\n请输入该书的作者:");
        scanf("%s",newBookAuthor);
        newBook->author = newBookAuthor;
        newBook->borrowListHead = NULL;

        printf("\n\n添加结果: \n--------------------------------------------------------------------");
        newBook->presentNum = 1;
        newBook->totalNum = 1;
        InsertKey(bt ,key ,newBook,r->node,r->index);
        showBookTree(bt->root ,1);
        printf("\n--------------------------------------------------------------------\n\n");
        printf("\n添加成功");
    }
}

void borrowBook(BookNode *p ,int index,Borrower *borrower){
    Book *book = p->book[index];
    Borrower *next = book->borrowListHead;
    Borrower *temp = next;
    while(next != NULL){
        temp = next;
        next = next->next;
    }
    if(temp == NULL)
        book->borrowListHead = borrower;
    else
        temp->next = borrower;
    book->presentNum--;
}

void showBook(BookTree bt,Result *r){
    Book book;
    int key;
    int i = 1;
    Borrower *next;

    printf("\n\n请输入要查找的书号:");
    scanf("%d" ,&key);
    printf("\n\n查找结果: \n--------------------------------------------------------------------");

    SearchNode(bt ,key ,r);
    if(r->flag == 0)
        printf("\n抱歉，该树没有该书!");
    else {
        book.id = r->node->book[r->index]->id;
        book.name = r->node->book[r->index]->name;
        book.author = r->node->book[r->index]->author;
        book.presentNum = r->node->book[r->index]->presentNum;
        book.totalNum = r->node->book[r->index]->totalNum;
        book.borrowListHead = r->node->book[r->index]->borrowListHead;
        next = book.borrowListHead;

        printf("\n书号:%d,作者:%s,书名:%s,现存量:%d,总库存:%d",book.id,book.author,book.name,book.presentNum,book.totalNum);
        printf("\n\n该书被借阅的情况为:");

        if(next == NULL)
            printf("\n暂无借阅情况");
        else {
            while(next != NULL){
                printf("\n借阅%d---姓名:%s,借阅证:%d,归还期限:%d-%d-%d",i++,next->name,next->borrowId,next->date->tm_year+1900,next->date->tm_mon+1,next->date->tm_mday);
                next = next->next;
            }
        }

    }
    printf("\n--------------------------------------------------------------------\n\n");
}
