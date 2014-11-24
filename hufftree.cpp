#include "hufftree.h"

HuffTree::HuffTree()
{
}

void HuffTree::Generate(QMap<char, int> &map,QMap<char,QString> &codeMap)
{
    QMap<char,int>::Iterator it;
    QList<Node *> list;
    Node * node;
    for(it=map.begin();it!=map.end();it++)
    {
        node=new Node();
        node->left=0;
        node->right=0;
        node->parent=0;
        node->count=it.value();
        node->c=it.key();
        list.append(node);
    }
    Node *min1,*min2;
    while(list.size()>1)
    {
        min1=this->FindMin(list);
        min2=this->FindMin(list);
        node =new Node();
        node->left=min1;
        node->right=min2;
        node->parent=0;
        node->count=min1->count+min2->count;
        min1->parent=min2->parent=node;
        list.append(node);
    }
    codeMap.clear();
    this->GenerateCode(node,"",codeMap);
}
void HuffTree::GenerateCode(Node *node,QString s, QMap<char, QString> &codeMap)
{
    if(node->left==0)
    {
        codeMap.insert(node->c,s);
    }
    else
    {
        GenerateCode(node->left,s+"0",codeMap);
        GenerateCode(node->right,s+"1",codeMap);
    }
}

Node* HuffTree::FindMin(QList<Node *> &list)
{
    Node * min;
    int index=0;
    min=list.first();
    for(int i=1;i<list.size();i++)
    {
        if(min->count>list[i]->count)
        {
            min=list[i];
            index=i;
        }
    }
    list.removeAt(index);
    return min;
}
