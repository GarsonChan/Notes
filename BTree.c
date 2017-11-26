#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "BTree.h"

//��ʼ��B��

void InitBTree(BTree *bt){
    double min = 3;
    min = min/2;
    bt->maxNode = m-1;
    bt->minNode = ceil(min)-1; //ceil����ȡ������
    bt->splitNode = (m+1)/2;
    bt->root = NULL;
}

//���ҹؼ���

int SearchInNode(BTNode p,int key){
    int i = 1;
    while(i <= p.keyNum && key > p.key[i])
        i++;
    return i;
}
void SearchNode(BTree bt,int key,Result *r){
    BTNode *p = bt.root ,*q;
    int found = 0,index;
    while(NULL != p && found == 0){
        index = SearchInNode(*p ,key);
        if(index <= p->keyNum && key == p->key[index])
            found = 1;
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

void AddNewRoot(BTree *bt,BTNode *child1,BTNode *child2,int key){
    BTNode *root = (BTNode*)malloc(sizeof(BTNode));
    root->key[1] = key;
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
void InsertKeyInNode(int key,BTNode *p,int index,BTNode *newNode){
    int i;
    int keyNum = p->keyNum;

    //���ؼ��ʺͺ�������
    for(i = keyNum; i >= index; i--){
        p->key[i+1] = p->key[i];
        p->child[i+1] = p->child[i];
    }
    p->key[index] = key;
    p->child[index] = newNode;
    /**
        �����if else������ִ�еڶ���whileѭ��׼���ģ�
        Ҳ�����Ѻ�splitNode�Ĺؼ��ַŵ�˫�׽���ϣ�˫�׽���һ��childָ����ѳ�����newNode
    **/
    if(newNode != NULL)
        newNode->parent = p;
    p->keyNum++;
}
void SplitNode(BTNode *p ,int splitNode ,BTNode *newNode){
    int i,j;
    int keyNum = p->keyNum;

    for(i = 0;i<=m;i++)
        newNode->child[i] = NULL;
    //�������±����Ĺؼ��ʼ����Ӹ����ѳ����Ľ��
    for(i = splitNode + 1,j = 1; i <= keyNum; i++,j++){
        newNode->key[j] = p->key[i];
        //���ѳ��Ľ�����������Ҫָ������±��һλ�ؼ��ʵ�������������
        newNode->child[j] = p->child[i];
        newNode->child[j-1] = p->child[i-1];
    }
    newNode->keyNum = keyNum - splitNode;
    newNode->parent = p->parent;
    for(i = 0; i <= newNode->keyNum; i++){
        if(newNode->child[i] != NULL)
            newNode->child[i]->parent = newNode;
    }

    p->keyNum = splitNode -1;//��Ҫ��1����Ϊ����������splitNodeΪ�±�Ĺؼ��ʻᱻ���뵽˫�׽���С����Դ˴��ȼ�ȥ1
}
void InsertKey(BTree *bt,int key,BTNode *p,int index){
    int n;//�����м�¼�ؼ��ʵı���(����Ҫ����Ĺؼ��ʺͷ���֮����ҪҪ����˫�׽��Ĺؼ���)
    int splitNode = bt->splitNode;//�����ѵ�λ�ã�s=(m+1)/2
    int isFinish = 0;//isFinish�жϲ�����Ƿ���Ҫ���ѽ��Ϳ�����ɣ�
    int newRoot = 0;//�ж��Ƿ���Ҫ���ɸ����
    BTNode *newNode = NULL;//���ѳ����Ľ��(��һ���õ�)
    BTNode *root = bt->root;

    //�����ʱû�и���㣬�򴴽����
    if(root == NULL)
        AddNewRoot(bt ,NULL ,NULL ,key);
    else {
        n = key;
        while(isFinish == 0 && newRoot == 0){
            //����ؼ���
            InsertKeyInNode(n,p,index,newNode);
            //��������ؼ��ʸ����ڷ�Χ�ڣ������ɹ�
            if(p->keyNum <= bt->maxNode)//p->keyNum < m
                isFinish = 1;
            //���򣬽��з��Ѳ���
            else {
                newNode = (BTNode*)malloc(sizeof(BTNode));
                //����
                SplitNode(p ,splitNode ,newNode);

                n = p->key[splitNode];
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
        AddNewRoot(bt ,p ,newNode ,n);
}

//ɾ���ؼ���

BTNode* FindKey(BTNode *p ,int index){

    BTNode *child = p->child[index - 1];
    BTNode *node = p;
    while(child != NULL){
        node = child;
        child = node->child[child->keyNum];
    }
    p->key[index] = node->key[node->keyNum];
    node->key[node->keyNum] = 0;
    return node;
}
//�ϲ����������ֵܻ����ֵ�
void Merge(BTree *bt ,BTNode *left, BTNode *right, int index){

    int i,min;
    min = bt->minNode;
    BTNode *parent = left->parent;

    //��Ϊ���������˳�������Ƚ�˫�׽��Ĺؼ��ʸ�ֵ��������
    //��Ϊkey������±�1��ʼ�洢�����Դ˴��ȼ�һ
    left->key[++left->keyNum] = parent->key[index + 1];

    //���Һ��ӵ����ݴ���������,�������ݵĺϲ�
    memcpy(left->key + left->keyNum + 1,right->key + 1, right->keyNum*sizeof(int));
    memcpy(left->child + left->keyNum ,right->child, (right->keyNum+1)*sizeof(BTNode*));

    //�����ֵܵĺ��ӵ�parentָ��ָ�����ֵ�
    for(i = 0; i <= right->keyNum; i++){
        if(right->child[i] != NULL){
            right->child[i]->parent = left;
        }
    }
    left->keyNum += right->keyNum;//�������ֵܵĹؼ��ʸ���

    //��Ϊ˫�׽��һ���ؼ��ʺϲ����������ϣ�����˫�׽��Ĺؼ����Լ�����ָ��������
    for(i = index; i < parent->keyNum-1; i++){
        parent->key[i+1] = parent->key[i+2];
        parent->child[i+1] = parent->child[i+2];
    }
    parent->key[i+1] = 0;
    parent->child[i+1] = NULL;
    parent->keyNum--;
    free(right);

    //�����ʱ��˫�׽��ؼ��ʸ���С��min�����˫�׽����кϲ����ؼ��ʲ���
    if(parent->keyNum < min)
        ChooseLeftOrRight(bt ,parent ,parent->parent);
    else
        return;
}
//�ж�Ҫ�����ֵܻ������ֵܽ��в���
void Borrow(BTree *bt ,BTNode *p ,BTNode *bro, BTNode *parent, int left ,int right,int index){

    //index��ʾ���p��˫�׵Ľ���index������
    int borrowKey;
    int t;//��¼��������ת�ؼ���
    int k,j;

    //����Ǻ����ֵܽ�
    if(left == 1){
        //����ý����ֵܽ��Ĺؼ��ʸ����ܺͲ��������ֵ,����кϲ�
        if((p->keyNum + bro->keyNum) + 1 <= bt->maxNode){
            index--;
            Merge(bt ,bro ,p ,index);
            return;
        }

        borrowKey = bro->key[bro->keyNum];
        t = parent->key[parent->keyNum];

        //��Ϊ��˳��֮������Ҫ�ճ�λ�ò���ؼ���,������
        for(int i = p->keyNum; i > 0;i--){
            p->key[i+1] = p->key[i];
            p->child[i+1] = p->child[i];
        }
        p->child[1] = p->child[0];

        //����ɾ���ؼ������ڵĽ��
        p->key[1] = t;
        p->keyNum++;
        p->child[0] = bro->child[bro->keyNum];
        if(NULL != bro->child[bro->keyNum])
            bro->child[bro->keyNum]->parent = p;

        //����parent
        parent->key[parent->keyNum] = borrowKey;

        //�������ֵ�
        bro->key[bro->keyNum] = 0;
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
        t = parent->key[index + 1];

        //����ɾ���ؼ������ڵĽ��
        p->key[p->keyNum + 1] = t;
        p->keyNum++;
        p->child[p->keyNum] = bro->child[0];
        if(NULL != bro->child[0])
            bro->child[0]->parent = p;

        //����parent
        parent->key[index + 1] = borrowKey;

        //�������ֵ�
        for(k = 0,j = 1; k < bro->keyNum; k++,j++){
            bro->key[j] = bro->key[j+1];
            bro->child[k] = bro->child[k+1];
        }
        bro->child[k] = NULL;
        bro->keyNum--;
    }
    return;
}
void ChooseLeftOrRight(BTree *bt ,BTNode *p ,BTNode *parent){

    int index;

    //���p�Ǹ���㣬�򲻱ؽ��кϲ�����
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
}
void RemoveKey(BTree *bt,BTNode *p,int index){

    //ɾ���ؼ��֣�������0(����FindKey�ӿ�)
    p->key[index] = p->key[p->keyNum];
    p->key[p->keyNum] = 0;
    p->keyNum--;

    //�����ʱp���Ĺؼ��ʸ���������Сֵ(�ڹؼ��ʸ�����Χ��),��ɾ���ɹ�������
    if(p->keyNum >= bt->minNode)
        return;
    //���򣬽��кϲ����ؼ��ʲ���
    else {
        ChooseLeftOrRight(bt ,p ,p->parent);
    }
}
void DeleteKey(BTree *bt ,BTNode *p ,int index){

    //����ؼ������ڽ�㲻��Ҷ�ӽ�㣬�򽫹ؼ����滻�����ڽ��������е����ֵ��Ȼ��ɾ�����ֵ���ڵĽ��(��ʱΪҶ�ӽ��)
    if(p->child[index-1] != NULL){
        BTNode *node = FindKey(p ,index);
        DeleteKey(bt ,node ,node->keyNum);
    }//����ֱ��ִ��ɾ������
    else {
        RemoveKey(bt ,p,index);
    }
}

//��ʾ������״

void showBTree(BTNode *bt ,int height){
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
            showBTree(bt->child[j] ,height);
        }
    }
}
