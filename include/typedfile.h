#ifndef TYPEDFILE_H
#define TYPEDFILE_H

#include <fstream>
#include <type_traits>
#include <header.h>
#include "node.h"
#include "record.h"

using namespace std;

const ios::openmode mode = ios::in | ios::out | ios::binary;

template <class T, const unsigned int MIN_DEGREE>
class typedFile : private fstream {
   static_assert(is_base_of<serializable, T>::value, "T must be serializable");
   public:
      header head;
      typedFile();
      typedFile(const string name, const string type, const unsigned int version, ios::openmode openmode=mode);
      virtual ~typedFile();
      bool open(const string name, const string type, const unsigned int version, ios::openmode openmode);
      bool open();
      bool isOpen();
      bool close();
      void clear();
      bool readRecord(record<T, MIN_DEGREE> &r, unsigned long long int i);
      bool writeRecord(record<T, MIN_DEGREE> &r, unsigned long long int i);
      bool insertRoot(record<T, MIN_DEGREE> &r);
      bool writeNode(record<T, MIN_DEGREE> &r, unsigned long long int i, bool isnew);
      bool deleteRecord(unsigned long long int i);
      bool deleteNode(record<T, MIN_DEGREE> &r, unsigned long long int i);
      unsigned long long int getFirstValid();
      void setfirstValid(unsigned long long int index);
      unsigned long long int getFirstDeleted();
      unsigned long long int search(T data);
      unsigned long long int lastPosition();
      unsigned long long int calculateIndex();
   private:
      bool readHeader(header &h);
      bool writeHeader(header &h);
      unsigned long long int index2pos(unsigned long long int i);
      unsigned long long int pos2index(unsigned long long int p);
      bool deletefromBeg(unsigned long long int indexOfRecord);
      bool deletelefromRightneighbor(unsigned long long int indexOfNeighborRecord);
};

template <class T, const unsigned int MIN_DEGREE>
typedFile<T, MIN_DEGREE>::typedFile() : fstream() {
    //se não houve nenhum problema ao abrir o arquivo, informo ao usuario
    if(this->open("defaultBtr.dat","DBT",MIN_DEGREE,mode)){
        cout<<"defaultBtr.dat aberto!"<<endl;
    }else{
        //senao, fecho o arquivo caso tenha aberto de forma errada
        cout<<"Erro ao abrir o arquivo"<<endl;
        close();
    }
}

template <class T, const unsigned int MIN_DEGREE>
typedFile<T, MIN_DEGREE>::typedFile(const string name, const string type, const unsigned int ver, ios::openmode openmode) : fstream(name.c_str(), mode) {
    if(this->open(name,type,ver,mode)){
        cout<<"arquivo aberto!"<<endl;
    }else{
        cout<<"Erro ao abrir o arquivo, versões e/ou tipos diferentes"<<endl;
        close();
    }
}

template <class T, const unsigned int MIN_DEGREE>
typedFile<T, MIN_DEGREE>::~typedFile() {
    this->close();
}

template <class T, const unsigned int MIN_DEGREE>
bool typedFile<T, MIN_DEGREE>::open(const string name, const string type, const unsigned int ver, ios::openmode openmode) {
    fstream::open(name.c_str(), openmode);
    //se não foi aberto, abro o arquivo apenas no modo de ios::out e seto valores padrões
    if (!this->isOpen() && ver == MIN_DEGREE) {
        fstream::open(name.c_str(), ios::out);
        fstream::close();
        fstream::open(name.c_str(), openmode);
        this->head.setType(type);
        this->head.setFirstDeleted(0);
        this->head.setFirstValid(0);
        this->head.setVersion(ver);
        this->writeHeader(this->head);
        //por ser um novo arquivo, já seto uma nova raiz vazia
        record<T, MIN_DEGREE> rcrd;
        insertRoot(rcrd);
        return true;
    }else{
        //caso ja exista o header, carrego ele na memoria
        this->readHeader(this->head);
        //verifico se as versões, tipos e o grau da arvore b são iguais aos informados
        if(type == this->head.getType() && ver == this->head.getVersion() && MIN_DEGREE == ver){
            return true;
        }else{
            return false;
        }
    }
}

template <class T, const unsigned int MIN_DEGREE>
bool typedFile<T, MIN_DEGREE>::isOpen() {
   return fstream::is_open();
}

template <class T, const unsigned int MIN_DEGREE>
bool typedFile<T, MIN_DEGREE>::close() {
    fstream::close();
    return true;
}

template <class T, const unsigned int MIN_DEGREE>
void typedFile<T, MIN_DEGREE>::clear() {
    fstream::clear();
}

template <class T, const unsigned int MIN_DEGREE>
bool typedFile<T, MIN_DEGREE>::readRecord(record<T, MIN_DEGREE> &r, unsigned long long int i) {
    char *aux = new char[r.size()];
    this->clear();
    //posiciono a cabeça de leitura na posição referente ao i a partir do início e leio até bytes
    fstream::seekg(this->index2pos(i), ios::beg);
    fstream::read(aux, r.size());

    //efetuo o cast
    string serializedRecord = string(aux,r.size());
    //seto as informações no record r
    r.fromString(serializedRecord);
    //deleto o ponteiro aux e aponto o mesmo para null
    delete[] aux;
    aux = nullptr;
    return true;
}

template <class T, const unsigned int MIN_DEGREE>
bool typedFile<T, MIN_DEGREE>::writeRecord(record<T, MIN_DEGREE> &r, unsigned long long int i) {
    this->clear();
    //seto a cabeça de escrita na posição i a partir do inicio
    fstream::seekp(this->index2pos(i), ios::beg);
    //escrevo no arquivo o record r r.size() bytes
    fstream::write(r.toString().c_str(), r.size());
    return true;
}

template <class T, const unsigned int MIN_DEGREE>
bool typedFile<T, MIN_DEGREE>::insertRoot(record<T, MIN_DEGREE> &r) {
    //se tem record deletado
    if(this->head.getFirstDeleted() != 0){
        //pego o index do record deletado
        unsigned long long int index = this->head.getFirstDeleted();
        record<T, MIN_DEGREE> rcrd;
        //se foi possível ler o record
        if(this->readRecord(rcrd,index)){
            //seto no header o proximo record deletado e a o proximo válido
            //na arvore b, o próximo válido é sempre a raiz
            this->head.setFirstDeleted(rcrd.getNext());
            //seto o record
            rcrd = r;
            rcrd.setNext(this->head.getFirstValid());
            //salvo o novo primeiro válido, e salvo o head e record
            this->head.setFirstValid(index);
            this->writeRecord(rcrd,index);
            this->writeHeader(this->head);
            return true;
        }else{
            return false;
        }
    }else{
        //caso não tenha arquivos deletados
        unsigned long long int pos;
        //posiciono a cabeça de gravação na última posição do arquivo
        seekp(0,ios::end);
        pos = fstream::tellp();
        //seto o proximo do record(sem necessidade para a arvore b)
        r.setNext(this->head.getFirstValid());
        //seto o proximo no head
        this->head.setFirstValid(this->pos2index(pos));
        //salvo head e record
        this->writeHeader(this->head);
        this->writeRecord(r,this->pos2index(pos));
        return true;
    }
}

template <class T, const unsigned int MIN_DEGREE>
bool typedFile<T, MIN_DEGREE>::deleteRecord(unsigned long long int i) {
    //utilizado apenas no typedfile
    unsigned long long int index = this->getFirstValid();
    unsigned long long int leftNeighborIndex = 0;
    record<T, MIN_DEGREE> aux;

    if(i != index){
        while(fstream::good() && index != 0){
            this->readRecord(aux,index);
            if(i == aux.getNext()){
                leftNeighborIndex = index;
                return deletelefromRightneighbor(leftNeighborIndex);
            }
            leftNeighborIndex = index;
            index = aux.getNext();
        }
        return false;
    }else{
        this->readRecord(aux,index);
        return deletefromBeg(index);;
    }
}

template <class T, const unsigned int MIN_DEGREE>
unsigned long long int typedFile<T, MIN_DEGREE>::getFirstValid() {
    return this->head.getFirstValid();
}

template <class T, const unsigned int MIN_DEGREE>
unsigned long long int typedFile<T, MIN_DEGREE>::getFirstDeleted() {
    return this->head.getFirstDeleted();
}

template <class T, const unsigned int MIN_DEGREE>
unsigned long long int typedFile<T, MIN_DEGREE>::search(T data) {
    unsigned long long int index = this->getFirstValid();
    //utilizado apenas no typedfile
    while(fstream::good() && index != 0){
        record<T, MIN_DEGREE> aux;
        this->readRecord(aux,index);
        if(data == aux.getData()){
            return index;
        }
        index = aux.getNext();
    }
    return 0;
}

template <class T, const unsigned int MIN_DEGREE>
bool typedFile<T, MIN_DEGREE>::readHeader(header &h) {

    char *aux = new char[this->head.size()];
    this->clear();
    //carrego o header para a memoria na pos (0)
    fstream::seekg(0, ios::beg);
    fstream::read(aux, this->head.size());
    string serializedHeader = string(aux, this->head.size());
    //chamo o fromString do head
    this->head.fromString(serializedHeader);
    //deleto o ponteiro aux
    delete[] aux;
    aux = nullptr;
    return true;
}

template <class T, const unsigned int MIN_DEGREE>
bool typedFile<T, MIN_DEGREE>::writeHeader(header &h) {
    //escrevo o header da pos 0 até h,size() na memoria
    this->clear();
    fstream::seekp(0, ios::beg);
    fstream::write(h.toString().c_str(), h.size());
    return true;
}

template <class T, const unsigned int MIN_DEGREE>
unsigned long long int typedFile<T, MIN_DEGREE>::index2pos(unsigned long long int i) {
    record<T, MIN_DEGREE> aux;
    //transformo index em uma posição entendida pelo fstrem
    unsigned long long int position = this->head.size() + ((i - 1) * aux.size());
    return position;
}

template <class T, const unsigned int MIN_DEGREE>
unsigned long long int typedFile<T, MIN_DEGREE>::pos2index(unsigned long long int p) {
    record<T, MIN_DEGREE> aux;
    //transformo posição em index entendida pelo typedfile
    unsigned long long int index = ((p - this->head.size()) / aux.size()) + 1;
    return index;
}

template <class T, const unsigned int MIN_DEGREE>
bool typedFile<T, MIN_DEGREE>::deletefromBeg(unsigned long long int indexOfRecord) {
    record<T, MIN_DEGREE> deletedRecord;
    unsigned long long int indexOfFirstDeleted = this->getFirstDeleted();
    //utilizado apenas no typedfile
    this->readRecord(deletedRecord,indexOfRecord);
    this->head.setFirstValid(deletedRecord.getNext());
    deletedRecord.setNext(indexOfFirstDeleted);
    deletedRecord.del();

    this->head.setFirstDeleted(indexOfRecord);
    this->writeRecord(deletedRecord,indexOfRecord);
    this->writeHeader(this->head);
    return true;
}

template <class T, const unsigned int MIN_DEGREE>
bool typedFile<T, MIN_DEGREE>::deletelefromRightneighbor(unsigned long long int indexOfNeighborRecord) {
    record<T, MIN_DEGREE> deletedRecord, neighborOfRecord;
    unsigned long long int indexOfRecord;
    unsigned long long int indexOfFirstDeleted = this->getFirstDeleted();
    //utilizado apenas no typedfile
    this->readRecord(neighborOfRecord,indexOfNeighborRecord);
    indexOfRecord = neighborOfRecord.getNext();
    this->readRecord(deletedRecord,indexOfRecord);

    neighborOfRecord.setNext(deletedRecord.getNext());
    deletedRecord.setNext(indexOfFirstDeleted);
    deletedRecord.del();
    this->head.setFirstDeleted(indexOfRecord);

    this->writeRecord(neighborOfRecord,indexOfNeighborRecord);
    this->writeRecord(deletedRecord,indexOfRecord);
    this->writeHeader(this->head);
    return true;

}

template <class T, const unsigned int MIN_DEGREE>
unsigned long long int typedFile<T, MIN_DEGREE>::lastPosition() {
    unsigned long long int pos;
    //retorno o index da última posição  em bytes do arquivo
    seekp(0,ios::end);
    pos = fstream::tellp();
    return pos2index(pos);
}

template <class T, const unsigned int MIN_DEGREE>
void typedFile<T, MIN_DEGREE>::setfirstValid(unsigned long long int index) {
    head.setFirstValid(index);
    writeHeader(this->head);
}

template <class T, const unsigned int MIN_DEGREE>
bool typedFile<T, MIN_DEGREE>::writeNode(record<T, MIN_DEGREE> &r,unsigned long long int index, bool isnew){
    if(!isnew){
        //se não é um novo node, eu apenas salvo
        return this->writeRecord(r, index);
    }else{
        //se é um node novo, utilizo lógica do insertRoot porém, sem atualizar o firstValid
        if(this->head.getFirstDeleted() != 0){
            index = this->head.getFirstDeleted();
            record<T, MIN_DEGREE> rcrd;
            if(this->readRecord(rcrd,index)){
                this->head.setFirstDeleted(rcrd.getNext());
                rcrd = r;
                this->writeRecord(rcrd,index);
                this->writeHeader(this->head);
                return true;
            }else{
                return false;
            }
        }else{
            index = lastPosition();
            this->writeRecord(r,index);
            return true;
        }
    }

}

template <class T, const unsigned int MIN_DEGREE>
unsigned long long int typedFile<T, MIN_DEGREE>::calculateIndex() {
    //calculo o index para o próximo record a ser inserido
    if(this->head.getFirstDeleted() != 0){
        //se possui um record deletado, retorno o index do mesmo
        return this->head.getFirstDeleted();
    }else{
        //caso não tenha um record deletado, retorno uma posição a partir do fim do arquivo
        unsigned long long int pos;
        seekp(0,ios::end);
        pos = fstream::tellp();
        return pos2index(pos);
    }

}

template <class T, const unsigned int MIN_DEGREE>
bool typedFile<T, MIN_DEGREE>::deleteNode(record<T, MIN_DEGREE> &r, unsigned long long int i) {
    //atualizo o primeiro deletado
    r.setNext(this->getFirstDeleted());
    head.setFirstDeleted(i);
    //salvo o header e o record
    this->writeHeader(this->head);
    return this->writeRecord(r,i);
}

#endif // TYPEDFILE_H
