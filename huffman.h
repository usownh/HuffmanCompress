#ifndef HUFFMAN_H
#define HUFFMAN_H
#include <QObject>
#include <QFile>
#include <QMessageBox>
#include <QMap>
#include <hufftree.h>
class Huffman : public QObject
{
    Q_OBJECT
public:
    explicit Huffman(QObject *parent = 0);
    void Compress(QString comFile,QString after);
    void Decompress(QString decomFile,QString after);
signals:
    void message(QString);
    void error(QString);
public slots:
private:
    bool CheckFile();
    void ComAnalyse();
    void DecomAnalyse();
    void GenerateCompressFile();
    void GenerateDecompressFile(QByteArray content, int zero);
    QFile inFile,outFile;
    QMap <char,int> statisticMap;
    QMap <char,QString> codeMap;
    QMap <QString,char> decodeMap;
    HuffTree tree;
    int decodeLength;
};

#endif // HUFFMAN_H
