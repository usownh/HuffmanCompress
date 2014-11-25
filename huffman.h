#ifndef HUFFMAN_H
#define HUFFMAN_H
#include <QThread>
#include <QFile>
#include <QMessageBox>
#include <QMap>
#include <hufftree.h>
class Huffman : public QThread
{
    Q_OBJECT
public:
    explicit Huffman(QThread *parent = 0);
    void Compress(QString comFile,QString after);
    void Decompress(QString decomFile,QString after);
    void run();
signals:
    void message(QString);
    void error(QString);
    void progress(int);
public slots:
private:
    bool CheckFile();
    void ComAnalyse();
    void DecomAnalyse();
    void GenerateCompressFile();
    void GenerateDecompressFile(QByteArray content, int zero);
    QByteArray formatCodeMap(QString s);
    QFile inFile,outFile;
    QMap <char,int> statisticMap;
    QMap <char,QString> codeMap;
    QMap <QString,char> decodeMap;
    HuffTree tree;
    int maxDecodeLength;
    QString dic[256];
    bool isCompress;
};

#endif // HUFFMAN_H
