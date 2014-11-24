#include "huffman.h"

Huffman::Huffman(QObject *parent) :
    QObject(parent)
{
}
void Huffman::Compress(QString comFile, QString after)
{
    if(comFile==after)
    {
        emit error("The input file name and the output file name cann't be the same!");
        return;
    }
    inFile.setFileName(comFile);
    outFile.setFileName(after);
    if(!CheckFile()) return;
    this->ComAnalyse();
    this->GenerateCompressFile();
    emit message("Compress Complete!");
}
void Huffman::Decompress(QString decomFile, QString after)
{
    if(decomFile==after)
    {
        emit error("The input file name and the output file name cann't be the same!");
        return;
    }
    inFile.setFileName(decomFile);
    outFile.setFileName(after);
    if(!CheckFile()) return;
    this->DecomAnalyse();
    emit message("Decompress complete!");
}

bool Huffman::CheckFile()
{
    if(!inFile.exists())
    {
        emit error("File doesn't exist!");
        return false;
    }
    if(outFile.exists())
    {
        return QMessageBox::question(NULL,"OverWrite?","File exists! OverWrite it?");
    }
    return true;
}
void Huffman::ComAnalyse()
{
    statisticMap.clear();
    char c;
    inFile.open(QIODevice::ReadOnly);
    while(!inFile.atEnd())
    {
        inFile.getChar(&c);
        if(statisticMap.contains(c)) statisticMap[c]++;
        else statisticMap[c]=0;
    }
    tree.Generate(statisticMap,codeMap);
    inFile.close();
}
void Huffman::GenerateCompressFile()
{
    outFile.open(QIODevice::WriteOnly);
    QByteArray header,content;
    char c;
    int mapSize=codeMap.size();
    int zero=0;
    inFile.open(QIODevice::ReadOnly);
    while(!inFile.atEnd())
    {
        inFile.getChar(&c);
        content.append(codeMap[c]);
    }
    zero=8-content.size()%8;
    if(zero==8) zero=0;
    for(int i=0;i<zero;i++) content.append("0");
    header.append((char)zero);
    header.append((char)mapSize);
    QMap<char,QString>::iterator it;
    for(it=codeMap.begin();it!=codeMap.end();it++)
    {
        header.append(it.key());
        header.append(it.value());
        header.append(';');
    }
    outFile.write(header);
    bool ok;
    QByteArray after;
    after.clear();
    for(int i=0;i<content.size();i=i+8)
    {
        after.append((char)content.mid(i,8).toInt(&ok,2));
    }
    outFile.write(after);
    inFile.close();
    outFile.close();
}

void Huffman::DecomAnalyse()
{
    QByteArray content;
    inFile.open(QIODevice::ReadOnly);
    content=inFile.readAll();
    inFile.close();
    int zero=content[0]&0x000000ff;
    int mapSize=content[1]&0x000000ff;
    if(mapSize==0) mapSize=256;
    decodeMap.clear();
    int i,j;
    decodeLength=0;
    for(i=2;mapSize>0;mapSize--)
    {
        j=content.indexOf(";",i+1);
        decodeMap[QString(content.mid(i+1,j-i-1))]=content[i];
        if(decodeLength<j-i-1) decodeLength=j-i-1;
        i=j+1;
    }
    this->GenerateDecompressFile(content.mid(i),zero);
}
void Huffman::GenerateDecompressFile(QByteArray content,int zero)
{
    QString after;
    QString dic[256];
    for(int k=0;k<256;k++)
    {
        for(int mask=128;mask>0;mask/=2)
        {
            if((k&mask)==0) dic[k].append("0");
            else dic[k].append("1");
        }
    }
    int index;
    outFile.open(QIODevice::WriteOnly);
    QByteArray out;
    for(int i=0;i<content.size();i++)
    {
        index=content[i]&0x000000ff;
        after.append(dic[index]);
        if(after.size()>100)
        {
            while(after.size()>decodeLength+1)
            {
                for(int i=1;i<after.size();i++)
                {
                    if(decodeMap.contains(after.mid(0,i)))
                    {
                        out.append(decodeMap[after.mid(0,i)]);
                        if(out.size()>1000)
                        {
                            outFile.write(out);
                            out.clear();
                        }
                        after.remove(0,i);
                        break;
                    }
                }
            }
        }
    }
    after=after.mid(0,after.size()-zero);
    for(int i=1;i<=after.size();i++)
    {
        if(decodeMap.contains(after.mid(0,i)))
        {
            out.append(decodeMap[after.mid(0,i)]);
            after.remove(0,i);
            i=0;
        }
    }
    outFile.write(out);
    outFile.close();
}
