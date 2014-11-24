#ifndef HUFFTREE_H
#define HUFFTREE_H
#include <QMap>
#include <QList>
#include <QString>
struct  Node
{
    Node *left;
    Node *right;
    Node *parent;
    int count;
    char c;
};

class HuffTree
{
public:
    HuffTree();
    void Generate(QMap<char,int> &map, QMap<char, QString> &codeMap);
private:
    Node* FindMin(QList<Node *> &list);
    void GenerateCode(Node *node,QString s,QMap<char,QString> &codeMap);
};

#endif // HUFFTREE_H
