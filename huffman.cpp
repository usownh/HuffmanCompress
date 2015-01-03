#include "huffman.h"

Huffman::Huffman(QThread *parent) :
    QThread(parent)
{
    for(int k=0;k<256;k++)
    {
        for(int mask=128;mask>0;mask/=2)
        {
            if((k&mask)==0) dic[k].append("0");
            else dic[k].append("1");
        }
    }//initial dict. It's easy to decode.
    CompressHeader=QByteArray::fromHex("ff00ff00");
    EncryptHeader=QByteArray::fromHex("ff00ff01");
}
void Huffman::run()
{//compress or decompress in another thread
    if(isCompress)
    {
        this->ComAnalyse();
        this->GenerateCompressFile();
        emit message("Compress Complete!");
    }
    else
    {
        this->DecomAnalyse();
    }
}

void Huffman::Compress(QString comFile, QString after, QString passWord, bool needPass)
{
    if(comFile==after)
    {
        emit error("The input file name and the output file name cann't be the same!");
        return;
    }
    this->passWord=passWord;
    this->needPass=needPass;
    inFile.setFileName(comFile);
    outFile.setFileName(after);
    if(!CheckFile()) return;
    isCompress=true;
    this->start();

}
void Huffman::Decompress(QString decomFile, QString after, QString passWord)
{
    if(decomFile==after)
    {
        emit error("The input file name and the output file name cann't be the same!");
        return;
    }
    this->passWord=passWord;
    inFile.setFileName(decomFile);
    outFile.setFileName(after);
    if(!CheckFile()) return;
    isCompress=false;
    this->start();
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

void Huffman::EncryptOrDecrypt(QByteArray &content,QByteArray &pass,int &posOfPass)
{//content XOR password
    for(int i=0;i<content.size();i++,posOfPass++)
    {
        posOfPass=posOfPass%pass.size();
        content[i]=content[i]^pass[posOfPass];
    }
}

void Huffman::ComAnalyse()
{
    statisticMap.clear();
    char c;
    inFile.open(QIODevice::ReadOnly);
    emit message("Analysing File!");
    while(!inFile.atEnd())
    {//statistics
        inFile.getChar(&c);
        if(statisticMap.contains(c)) statisticMap[c]++;
        else statisticMap[c]=0;
    }
    emit message("Generating Huffman Tree!");
    tree.Generate(statisticMap,codeMap);
    inFile.close();
}
void Huffman::GenerateCompressFile()
{
    emit message("Generating compressed file!");
    outFile.open(QIODevice::WriteOnly);
    QByteArray header,content;
    QByteArray pass=passWord.toLocal8Bit();
    if(needPass)
    {
        header.append(EncryptHeader);
        header.append(QCryptographicHash::hash(pass,QCryptographicHash::Md5));
    }
    else    header.append(CompressHeader);
    outFile.write(header);
    header.clear();
    char c;
    int mapSize=codeMap.size();
    int zero=0;
    inFile.open(QIODevice::ReadOnly);
    while(!inFile.atEnd())
    {//code content with huffman code
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
        header.append(this->formatCodeMap(it.value()));
    }
    int passPos=0;
    if(needPass) this->EncryptOrDecrypt(header,pass,passPos);
    outFile.write(header);
    bool ok;
    QByteArray after;
    after.clear();
    for(int i=0;i<content.size();i=i+8)
    {//transfer string to binary file
        after.append((char)content.mid(i,8).toInt(&ok,2));
        emit progress((i*100)/content.size());
    }
    emit progress(100);
    if(needPass) this->EncryptOrDecrypt(after,pass,passPos);
    outFile.write(after);
    inFile.close();
    outFile.close();
}
QByteArray Huffman::formatCodeMap(QString s)
{
    //format::key.length.huffmancode
    // bytes:: 1    1        -
    int l=s.size();
    QByteArray r;
    r.append((char)l);
    while(s.size()%8!=0) s.append("0");
    bool ok;
    for(int i=0;i<s.size();i=i+8)
    {
        r.append((char)s.mid(i,8).toInt(&ok,2));
    }
    return r;
}

void Huffman::DecomAnalyse()
{
    QByteArray content;
    QByteArray pass=passWord.toLocal8Bit();
    inFile.open(QIODevice::ReadOnly);
    content=inFile.readAll();
    inFile.close();
    if(content.startsWith(CompressHeader)) content.remove(0,4);
    else if(content.startsWith(EncryptHeader))
    {
        content.remove(0,4);
        if(content.mid(0,16)==QCryptographicHash::hash(pass,QCryptographicHash::Md5))
        {
            content.remove(0,16);
            int pos=0;
            this->EncryptOrDecrypt(content,pass,pos);
        }
        else
        {
            emit error("wrong password!");
            return;
        }
    }
    else
    {
        emit error("file error!");
        return;
    }
    int zero=content[0]&0x000000ff;
    int mapSize=content[1]&0x000000ff;
    if(mapSize==0) mapSize=256;
    decodeMap.clear();
    int i,decodeLength;
    maxDecodeLength=0;
    char key;
    QString value;
    emit message("Generating decoding map!");
    for(i=2;mapSize>0;mapSize--)
    {//read decode map
        key=content[i];
        decodeLength=content[i+1]&0xff;
        if(maxDecodeLength<decodeLength) maxDecodeLength=decodeLength;
        value.clear();
        for(i=i+2;decodeLength>0;decodeLength-=8,i++)
        {
            value+=dic[(content[i]&0xff)];
        }
        value=value.mid(0,value.size()+decodeLength);
        decodeMap[value]=key;
    }
    this->GenerateDecompressFile(content.mid(i),zero);
    emit message("Decompress complete!");
}
void Huffman::GenerateDecompressFile(QByteArray content,int zero)
{
    emit message("Generating decompressed file!");
    QString after;
    int index;
    outFile.open(QIODevice::WriteOnly);
    QByteArray out;
    for(int i=0;i<content.size();i++)
    {
        index=content[i]&0x000000ff;
        after.append(dic[index]);
        if(after.size()>100)
        {
            while(after.size()>maxDecodeLength+1)
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
        if(i%1000==0) emit progress((i*100)/content.size());
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
    emit progress(100);
    outFile.write(out);
    outFile.close();
}
