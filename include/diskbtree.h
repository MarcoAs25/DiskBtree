#ifndef DISKBTREE_H_INCLUDED
#define DISKBTREE_H_INCLUDED

#include<iostream>
#include <string>
#include <vector>
#include "typedfile.h"
#include "node.h"
#include "record.h"

using namespace std;

template <class T,const unsigned int MIN_DEGREE>
class diskbtree: private typedFile<T, MIN_DEGREE>{
private:
    node<T,MIN_DEGREE> root;
    //auxiliar do print
    void printAux(node<T, MIN_DEGREE> x, vector<string> &v, unsigned int lvl);
    //auxiliar do search
    bool searchAux(node<T,MIN_DEGREE> x, T key);

    //auxiliar do insertKey
    unsigned int haskey(node<T, MIN_DEGREE> x, T key);
    bool insertInNodeNonFull( node<T,MIN_DEGREE>& x, T key, unsigned int index);
    void splitChildNode(node<T,MIN_DEGREE>& x, unsigned int index, unsigned int indexOfX);

    //auxiliar do Remove
    bool removeAux(node<T,MIN_DEGREE> x, T key, unsigned int indexOfX);
    bool removeFromLeaf(node<T, MIN_DEGREE> x, unsigned int index, unsigned int indexOfX);
    bool removeFromNonLeaf(node<T, MIN_DEGREE> x, unsigned int index, unsigned int indexOfX);
    T findPred(node<T, MIN_DEGREE> x, unsigned int index, unsigned int indexOfX);
    T findSuc(node<T, MIN_DEGREE> x, unsigned int index, unsigned int indexOfX);
    void merge(node<T, MIN_DEGREE> x, unsigned int index, unsigned int indexOfX);
    bool goDownAndOrganize(node<T, MIN_DEGREE> x,unsigned int index, unsigned int indexOfX);
    void rotatekeys(node<T, MIN_DEGREE> x,unsigned int index, unsigned int indexOfX,bool direction);

    //auxiliar do get e set
    node<T,MIN_DEGREE> getRoot() const { return this->root; }
    void setRoot(node<T,MIN_DEGREE> root ) { this->root = root; }

    //auxiliar do typedFile
    node<T,MIN_DEGREE> readNode(unsigned int indexOfNode);
    bool writeNode(node<T,MIN_DEGREE> x, unsigned int indexOfNode,bool isnew);
    unsigned int getLastPosition();
    unsigned int getRootIndex();
    void setRootIndex(unsigned int index);
    bool insertRoot(node<T, MIN_DEGREE> n);
    bool deleteNode(node<T, MIN_DEGREE> n, unsigned int i);

public:
    ~diskbtree();
    diskbtree();
    diskbtree(const string name, const string type, const unsigned int version);
    bool insertKey(T key);
    void print();
    bool search(T key);
    bool remove(T key);
    void close();
    bool isOpen();

};

template <class T, const unsigned int MIN_DEGREE>
diskbtree<T, MIN_DEGREE>::~diskbtree(){
    //fecha o arquivo
    close();
}

template <class T, const unsigned int MIN_DEGREE>
diskbtree<T, MIN_DEGREE>::diskbtree():typedFile<T, MIN_DEGREE>::typedFile(){
    //carrego a raiz para a memória
    this->setRoot(this->readNode(this->getRootIndex()));
}

template <class T, const unsigned int MIN_DEGREE>
diskbtree<T, MIN_DEGREE>::diskbtree(const string name, const string type, const unsigned int version):typedFile<T, MIN_DEGREE>::typedFile(name, type, version){
    //carrego a raiz para a memória
    this->setRoot(this->readNode(this->getRootIndex()));
}

template <class T, const unsigned int MIN_DEGREE>
bool diskbtree<T, MIN_DEGREE>::insertKey(T key){
    bool result = false;
    node<T, MIN_DEGREE> r = this->getRoot();
    //verifica se a raiz está cheia
    if(r.getSize() == r.MAX){
        //caso esteja cheia, crio uma nova raiz
        node<T, MIN_DEGREE> s(false),aux;
        unsigned int i = 0;

        s.setChildren(0, this->getRootIndex());

        this->insertRoot(s);
        //divido o filho mais a esquerda(antiga raiz) e a chave do meio child[MIN_DEGREE] é tranferido para a raiz
        splitChildNode(s,0, this->getRootIndex());

        //após a divisão, verifico por onde se deve descer(0 ou 1)
        if (s.getkey(0) < key){
            i++;
        }
        //recarrego o filho 0 ou 1 da raiz para a memória
        aux = this->readNode(s.getChildren(i));

        //insiro a chave passando o filho chave e indexNo arquivo para aux
        result = insertInNodeNonFull(aux,key,s.getChildren(i));

        this->setRoot(s);//carrego a raiz para a memória
        return result;
    }
    else{
        //caso não esteja cheia, mando inserir em algum node não cheio e carrego a raiz para a memória
        result = insertInNodeNonFull(r,key,this->getRootIndex());
        this->setRoot(r);
        return result;
    }
}

template <class T, const unsigned int MIN_DEGREE>
bool diskbtree<T, MIN_DEGREE>::insertInNodeNonFull(node<T,MIN_DEGREE>& x,T key,unsigned int index){

    //carrego o tamanho do node analisado
    int i = x.getSize() - 1;
    //se for folha insere
    if(x.isleaf()){
        while(i >= 0 && key < x.getkey(i)){
            x.setkey(i + 1, x.getkey(i));
            --i;
        }
        ++i;
        x.setkey(i, key);
        x.setSize(x.getSize()+1);
        return this->writeNode(x,index, false);
    }else{
        //caso não seja folha
        node<T, MIN_DEGREE> aux;
       // verifico por onde devo descer
        int n = 0;
        while(n <= i && x.getkey(n) < key){
            n++;
        }

        aux = this->readNode(x.getChildren(n));
        //verifico se posso descer
        if (aux.getSize() == aux.MAX)
        {
            //caso não posso descer para inserir, divido o filho de x em i
            splitChildNode(x,n,index);
            //verifico novamente por onde devo descer
            if (x.getkey(n) < key)
                n++;
        }
        //carreco o filho e desço para remover
        aux = this->readNode(x.getChildren(n));
        return insertInNodeNonFull(aux,key,x.getChildren(n));
    }
}

template <class T, const unsigned int MIN_DEGREE>
void diskbtree<T, MIN_DEGREE>::splitChildNode(node<T,MIN_DEGREE>& x, unsigned int index, unsigned int indexOfX){

    node <T, MIN_DEGREE> y,z;
    //carrego o filho da esquerda de x
    y = this->readNode(x.getChildren(index));
    //y e z estão no mesmo nível então, se y é folha, z é folha, se y não é folha, z também não é
    z.setLeaf(y.isleaf());
    z.setSize(z.MIN);// splitchild é chamado quando y contém MAX, ao final do split y e z terá MIN

    //copio MIN chaves para z
    for (unsigned int j = 0; j < z.getSize(); j++){
        z.setkey(j, y.getkey(j + MIN_DEGREE));
    }
    //copio MIN + 1 filhos de y para z(começando se MIN_DEGREE)
    if(!y.isleaf()){
        for (unsigned int j = 0; j < MIN_DEGREE; j++){
            z.setChildren(j, y.getChildren(j + MIN_DEGREE));
        }
    }
    //seto o tamanho de y para MIN
    y.setSize(y.MIN);
    //realoco os filhos de x uma posição á frente até index + 1
    for (unsigned int j = x.getSize(); j >= index + 1; j--){
        x.setChildren(j+1, x.getChildren(j));
        if(j == 0){
            break;
        }
    }
    //calculo a posição que z terá ao salvar no arquivo e salvo no filho de x
    x.setChildren(index + 1, this->getLastPosition());

    // seto as chaves de x uma posição à frente
    if(x.getSize() > 0){
        for (unsigned int j = x.getSize() - 1; j >= index; j--){
            x.setkey(j + 1, x.getkey(j));
            if(j == 0){
                break;
            }
        }
    }
    //seto a key de x como sendo a antiga do meio de y
    x.setkey(index, y.getkey(y.MIN));
    x.setSize(x.getSize() + 1);
    //salvo x,y,z
    this->writeNode(x, indexOfX, false);
    this->writeNode(y, x.getChildren(index),false);
    this->writeNode(z,0, true);
}

template <class T, const unsigned int MIN_DEGREE>
bool diskbtree<T, MIN_DEGREE>::search(T key)
{   //carrego a raiz e se ela for ferente de 0, retorno searchAux, senão retorno false
    node<T, MIN_DEGREE> r = this->getRoot();
    if (r.getSize() != 0){
      return searchAux(r, key);
    }
    return false;
}

template <class T, const unsigned int MIN_DEGREE>
bool diskbtree<T, MIN_DEGREE>::searchAux(node<T, MIN_DEGREE> x,T key){
    node <T, MIN_DEGREE> aux;
    unsigned int i = 0;
    //pesquiso por onde devo descer(ou a chave)
    while (i < x.getSize() && key > x.getkey(i)){
        i++;
    }
    //verifico se a chave se encontra  no node
    if (x.getkey(i) == key){
        return true;
    }
    //se não estava e for folha, retorno false
    if (x.isleaf()){
        return false;
    }
    //se não estava e é página interna, carrego o filho de x em i
    aux = this->readNode(x.getChildren(i));
    //retorno recursivamente searchAux passando o filho de x em i e key;
    return searchAux(aux, key);

}

template <class T, const unsigned int MIN_DEGREE>
bool diskbtree<T, MIN_DEGREE>::remove(T key){

    bool removido = false;

    node<T, MIN_DEGREE> root = this->getRoot();
    // se o tamanho a raiz é 0, retora falso
    if (root.getSize() == 0)
    {
        return removido;
    }

    //chamo o removeAux passando a raiz , chave e o index da raiz
    removido = removeAux(root ,key , this->getRootIndex());

    //carrego a raiz da memória
    this->setRoot(this->readNode(this->getRootIndex()));

    root = this->getRoot();
    //se a raiz ficou com tamanho 0
    if (root.getSize() == 0 && !root.isleaf())
    {
        //carrego o filho da direita como a nova raiz e deleto a raiz antiga
        node<T, MIN_DEGREE> oldRoot = root;
        unsigned int oldRootIndex = getRootIndex();

        this->setRootIndex(root.getChildren(0));

        this->setRoot(this->readNode(root.getChildren(0)));

        return deleteNode(oldRoot, oldRootIndex);
    }
    return removido;
}

template <class T, const unsigned int MIN_DEGREE>
bool diskbtree<T, MIN_DEGREE>::removeAux(node<T, MIN_DEGREE> x, T key, unsigned int indexOfX){

    unsigned int index = haskey(x, key);
    //carrego o index de onde devo descer ou da chave no node
    //verifico se a  chave se encontra no node atual e se é uma chave válida
    if(index < x.getSize() && x.getkey(index) == key){
        if(x.isleaf()){
            //caso esteja em um node e este node seja folha, chamo o caso 1
            return removeFromLeaf(x, index, indexOfX);
        }else{
            //caso esteja em um node e este node seja uma página interna, chamo o caso 2
            return removeFromNonLeaf(x, index, indexOfX);
        }
    }else if (!x.isleaf()){
        // se não está no node e o mesmo não é uma folha é chamado o caso 3
        node<T, MIN_DEGREE> child;

        //para que possamos saber se o houve um merge no ultimo filho, verifico se o size
        //do node atual é mesmo valor que o index
        bool ultimoFilhoFoiAgrupado = ((index == x.getSize())? true : false );


        //carrego o filho
        child = this->readNode(x.getChildren(index));

        //verifico se o filho.se tem MIN_DEGREE
        if(child.getSize() < MIN_DEGREE){
            //caso não tenha, desço para organizar e recarrego x
            goDownAndOrganize(x, index, indexOfX);
            x = this->readNode(indexOfX);
        }

        //se desci pelo filho mais a direita de x e x diminuiu de tamanho
        if(ultimoFilhoFoiAgrupado && index > x.getSize()){
            //carrego o child a esquerda
            child = this->readNode(x.getChildren(index - 1));
            return removeAux(child,key,x.getChildren(index - 1));

        }else{
            //carrego o child
            child = this->readNode(x.getChildren(index));
            return removeAux(child,key,x.getChildren(index));
        }

    }else{
        return false;
    }
}

template <class T, const unsigned int MIN_DEGREE>
unsigned int diskbtree<T, MIN_DEGREE>::haskey(node<T, MIN_DEGREE> x, T key){
    unsigned int index = 0;
    //carrego o index de onde devo descer ou da chave no node
    while(index < x.getSize() && key > x.getkey(index)){
        index++;
    }
    return index;
}

template <class T, const unsigned int MIN_DEGREE>
bool diskbtree<T, MIN_DEGREE>::removeFromLeaf(node<T, MIN_DEGREE> x, unsigned int index, unsigned int indexOfX){
    //removo a key no index de x e salvo x
    for (unsigned int i = index; i < x.getSize() - 1; i++){
        x.setkey(i, x.getkey(i + 1));
    }
    x.setSize(x.getSize() - 1);

    return this->writeNode(x, indexOfX, false);
}

template <class T, const unsigned int MIN_DEGREE>
bool diskbtree<T, MIN_DEGREE>::removeFromNonLeaf(node<T, MIN_DEGREE> x, unsigned int index, unsigned int indexOfX){
    T key = x.getkey(index);
    node<T, MIN_DEGREE> childrenLeft,childrenRight;

    //CARREGANDO OS FILHOS
    childrenLeft = this->readNode(x.getChildren(index));
    childrenRight = this->readNode(x.getChildren(index+1));

    //caso meu vizinho da esquerda tenha o MIN_DEGREE o mais chaves
    if(childrenLeft.getSize() >= MIN_DEGREE){
        //encontro o predecessor
        T pred = findPred(x, index, indexOfX);
        //salvo o predecessor em x no index da chave que seria removida
        x.setkey(index, pred);
        //salvo x
        return this->writeNode(x, indexOfX, false);

    }else if(childrenRight.getSize() >= MIN_DEGREE){
        //encontro o sucessor
        T suc = findSuc(x, index, indexOfX);
        //salvo o sucessor em x no index da chave que seria removida
        x.setkey(index, suc);
        //salvo x
        return this->writeNode(x, indexOfX, false);

    }else{
        //caso os filhos da esquerda e direita estejam com MIN keys
        //efetuo o merge entre eles
        merge(x,index,indexOfX);
        //carrego o children da esquerda
        childrenLeft = this->readNode(x.getChildren(index));
        //chamo removeAux
        return removeAux(childrenLeft, key, x.getChildren(index));

    }

}

template <class T, const unsigned int MIN_DEGREE>
T diskbtree<T, MIN_DEGREE>::findPred(node<T, MIN_DEGREE> x, unsigned int index, unsigned int indexOfX){
    node<T, MIN_DEGREE> child;
    child = this->readNode(x.getChildren(index));
    T key;

    //flag para verificar se irei descer pelo filho mais a direita

    bool ultimoFilhoFoiAgrupado = ((index == x.getSize())? true : false );
    //verifico se preciso organizar antes de descer
    if(child.getSize() < MIN_DEGREE){
            //organizo e carrego o filho
        this->goDownAndOrganize(x, index, indexOfX);
        child = this->readNode(x.getChildren(index));
    }

    //verifico quem contém o index de child
    if(ultimoFilhoFoiAgrupado && index > x.getSize()){
        indexOfX = x.getChildren(index - 1);

    }else{
        indexOfX = x.getChildren(index);
    }


    while(!child.isleaf()){
        //efetuo os mesmos passos acima, porém, sempre entrando no filho mais a direita
        node<T, MIN_DEGREE> aux = this->readNode(child.getChildren(child.getSize()));
        unsigned int idx = child.getSize();

        if(aux.getSize() < MIN_DEGREE){
            this->goDownAndOrganize(child, child.getSize(), indexOfX);
            child = this->readNode(indexOfX);
            aux = this->readNode(child.getChildren(child.getSize()));
        }

        if(idx > child.getSize()){
            indexOfX = child.getChildren(idx - 1);
        }else{
            indexOfX = child.getChildren(idx);
        }
        child = aux;
    }
    //carrego o filho
    child = this->readNode(indexOfX);
    //salvo a chave em aux
    key = child.getkey(child.getSize() - 1);
    //removo a chave em k child
    this->removeFromLeaf(child, child.getSize() - 1, indexOfX);
    return key;
}

template <class T, const unsigned int MIN_DEGREE>
T diskbtree<T, MIN_DEGREE>::findSuc(node<T, MIN_DEGREE> x, unsigned int index, unsigned int indexOfX){
    //mesma idéia do findPred, porém, a primeira parte do código entramos no filho mais
    //a direita e depois sempre no filho mais a direita,
    node<T, MIN_DEGREE> child;
    child = this->readNode(x.getChildren(index + 1));

    bool ultimoFilhoFoiAgrupado = (((index + 1) == x.getSize())? true : false );

    if(child.getSize() < MIN_DEGREE){
        this->goDownAndOrganize(x, index + 1, indexOfX);
        child = this->readNode(x.getChildren(index + 1));
    }


    if(ultimoFilhoFoiAgrupado && index > x.getSize()){
        indexOfX = x.getChildren(index);

    }else{
        indexOfX = x.getChildren(index + 1);
    }


    while(!child.isleaf()){

        node<T, MIN_DEGREE> aux = this->readNode(child.getChildren(0));
        unsigned int idx = 0;

        if(aux.getSize() < MIN_DEGREE){
            this->goDownAndOrganize(child, 0, indexOfX);
            child = this->readNode(indexOfX);
            aux = this->readNode(child.getChildren(0));
        }


        indexOfX = child.getChildren(idx);
        child = aux;
    }

    T key;
    child = this->readNode(indexOfX);
    key = child.getkey(0);
    this->removeFromLeaf(child, 0, indexOfX);
    return key;


}

template <class T, const unsigned int MIN_DEGREE>
void diskbtree<T, MIN_DEGREE>::merge(node<T, MIN_DEGREE> x, unsigned int index,unsigned int indexOfX ){
    node<T, MIN_DEGREE> childrenLeft,childrenRight;
    int indexChildrenRight = x.getChildren(index + 1);

    //CARREGANDO OS FILHOS
    childrenLeft = this->readNode(x.getChildren(index));
    childrenRight = this->readNode(x.getChildren(index + 1));
    //a chave de x é transferida para o filho da direita
    childrenLeft.setkey(childrenLeft.MIN, x.getkey(index));

    //copio as chaves do filho da direita para os da esquerda
    for (unsigned int i = 0; i < childrenRight.getSize(); i++){
        childrenLeft.setkey(i + MIN_DEGREE, childrenRight.getkey(i));
    }
    //se o filho for folha:
    //copio os filhos do filho da direita para os da esquerda
    if (!childrenLeft.isleaf())
    {
        for(unsigned int i = 0; i <= childrenRight.getSize(); i++){
            childrenLeft.setChildren(i + MIN_DEGREE, childrenRight.getChildren(i));
        }
    }
    //seto realoco as chaves de x uma posição para trás
    for (unsigned int i = index + 1; i < x.getSize(); i++){
        x.setkey(i - 1, x.getkey(i));
    }

    //realoca os filhos de x uma posição para trás
    for (unsigned int i = index + 2; i <= x.getSize(); i++){
        x.setChildren(i - 1, x.getChildren(i));
    }
    //seto os novos tamanhos do filho a esquerda e de seu pai x
    childrenLeft.setSize(childrenLeft.getSize() + 1 + childrenRight.getSize());
    x.setSize(x.getSize() - 1);

    //salvo x e childrenLeft, deletao childrenRight
    this->writeNode(childrenLeft, x.getChildren(index), false);
    this->writeNode(x, indexOfX, false);
    this->deleteNode(childrenRight, indexChildrenRight);
    return;
}

template <class T, const unsigned int MIN_DEGREE>
bool diskbtree<T, MIN_DEGREE>::goDownAndOrganize(node<T, MIN_DEGREE> x, unsigned int index,unsigned int indexOfX ){

    node<T, MIN_DEGREE> childrenAux;
    // se meu index não for 0, posso pedir para rotacionar da esquerda para a direita
    if ( index != 0 ){

        childrenAux = this->readNode(x.getChildren(index - 1));


        if(childrenAux.getSize() >= MIN_DEGREE){
            rotatekeys(x, index, indexOfX, false);
            return true;
        }

    }
    // se meu index não for o tamanho máximo e nem 0, posso pedir para rotacionar da direita para esquerda
    if( index != x.getSize() ){

        childrenAux = this->readNode(x.getChildren(index + 1));

        if(childrenAux.getSize() >= MIN_DEGREE){
            rotatekeys(x, index, indexOfX, true);
            return true;
        }

    }
    //senão verifico se o filho que quero fundir tem irmao a direita
    if(index != x.getSize()){
        //se tiver, efetuo a fusão com o filho da direita
        merge( x, index, indexOfX );
        return true;
    }else{
        //senão, efetuo a fusão com o filho da esquerda
        merge( x, index - 1, indexOfX );
        return true;
    }
}

template <class T, const unsigned int MIN_DEGREE>
void diskbtree<T, MIN_DEGREE>::rotatekeys(node<T, MIN_DEGREE> x, unsigned int index,unsigned int indexOfX, bool direction){

    //caso a rotação seja da direita para a esquerda
    if(direction){

        node<T, MIN_DEGREE> child,right;

        //CARREGANDO OS FILHOS
        child = this->readNode(x.getChildren(index));
        right = this->readNode(x.getChildren(index + 1));

        //a chave de x desce para child
        child.setkey(child.getSize(), x.getkey(index));

        //caso child seja uma página interna
        //o  primeiro filho do irmao a esquerda vai para child
        if(!child.isleaf()){
            child.setChildren(child.getSize() + 1, right.getChildren(0));
        }
        //a primeira chave do irmao a direita sobe para o pai
        x.setkey(index, right.getkey(0));
        //reloco as chaves do irmao a esquerda
        for (unsigned int i = 1; i < right.getSize(); i++){
            right.setkey(i - 1, right.getkey(i));
        }
        //caso o irmao a esquerda seja uma página interna
        if (!right.isleaf())
        {
            //realoco seus filhos também
            for(unsigned int i = 1; i <= right.getSize(); i++){
                right.setChildren(i - 1, right.getChildren(i));
            }
        }
        //child incremendo child e decremento size
        child.setSize(child.getSize() + 1);
        right.setSize( right.getSize() - 1);

        //salvo x,right e child
        this->writeNode(x, indexOfX, false);
        this->writeNode(right, x.getChildren(index + 1), false);
        this->writeNode(child, x.getChildren(index), false);


        return;

    }else{
        //mesma idéia do caso á cima, porém a rotação é com o filho da esquerda
        //então a rotação é feita da esquerda para a direita
        node<T, MIN_DEGREE> child,left;

        //CARREGANDO OS FILHOS
        child = this->readNode(x.getChildren(index));
        left = this->readNode(x.getChildren(index - 1));


        for (unsigned int i = child.getSize() - 1 ; i >= 0; i--){
            child.setkey(i + 1, child.getkey(i));
            if(i == 0){
                break;
            }
        }

        if (!child.isleaf()){

            for(unsigned int i = child.getSize(); i >= 0; i--){
                child.setChildren( i + 1 , child.getChildren(i));
                if(i == 0){
                    break;
                }
            }
        }

        child.setkey(0, x.getkey(index - 1));

        if(!child.isleaf()){
            child.setChildren(0, left.getChildren(left.getSize()));
        }

        x.setkey(index - 1, left.getkey(left.getSize() - 1));

        child.setSize(child.getSize() + 1);
        left.setSize( left.getSize() - 1);

        this->writeNode(x, indexOfX, false);
        this->writeNode(left, x.getChildren(index - 1),false);
        this->writeNode(child, x.getChildren(index), false);

    }
}
template <class T, const unsigned int MIN_DEGREE>
node<T, MIN_DEGREE> diskbtree<T, MIN_DEGREE>::readNode(unsigned int indexOfNode){
    //carrego um record para a memoria e retorno o node
    record<T, MIN_DEGREE> rcrd;
    typedFile<T, MIN_DEGREE>::readRecord(rcrd, indexOfNode);
    return rcrd.getData();
}

template <class T, const unsigned int MIN_DEGREE>
unsigned int diskbtree<T, MIN_DEGREE>::getRootIndex(){
    //retorno o index da raiz
    return typedFile<T, MIN_DEGREE>::getFirstValid();
}

template <class T, const unsigned int MIN_DEGREE>
void diskbtree<T, MIN_DEGREE>::setRootIndex(unsigned int index){
    //seto o index da raiz
    typedFile<T, MIN_DEGREE>::setfirstValid(index);
}

template <class T, const unsigned int MIN_DEGREE>
unsigned int diskbtree<T, MIN_DEGREE>::getLastPosition(){
    //calculo o index no arquivo para um novo node
    return typedFile<T, MIN_DEGREE>::calculateIndex();
}

template <class T, const unsigned int MIN_DEGREE>
bool diskbtree<T, MIN_DEGREE>::insertRoot(node<T, MIN_DEGREE> n){
    //insiro uma nova raiz
    record<T, MIN_DEGREE> rcrd1;
    rcrd1.setData(n);
    return typedFile<T, MIN_DEGREE>::insertRoot(rcrd1);
}

template <class T, const unsigned int MIN_DEGREE>
bool diskbtree<T, MIN_DEGREE>::writeNode(node<T,MIN_DEGREE> x, unsigned int indexOfNode, bool isnew){
    //salvo um node
    record<T, MIN_DEGREE> rcrd;
    rcrd.setData(x);
    return typedFile<T, MIN_DEGREE>::writeNode(rcrd,indexOfNode, isnew);
}

template <class T, const unsigned int MIN_DEGREE>
bool diskbtree<T, MIN_DEGREE>::deleteNode(node<T, MIN_DEGREE> x, unsigned int i){
    //deleto um node(coloco na lista de deletados)
    record<T, MIN_DEGREE> rcrd;
    rcrd.setData(x);
    return typedFile<T, MIN_DEGREE>::deleteNode(rcrd, i);
}

template <class T, const unsigned int MIN_DEGREE>
void diskbtree<T, MIN_DEGREE>::close() {
    typedFile<T, MIN_DEGREE>::close();
}

template <class T, const unsigned int MIN_DEGREE>
bool diskbtree<T, MIN_DEGREE>::isOpen(){
    return typedFile<T, MIN_DEGREE>::isOpen();
}

template <class T, const unsigned int MIN_DEGREE>
void diskbtree<T, MIN_DEGREE>::print() {
   //percorro minha árvore e vou salvando as keys de seus nodes em um vetor
   //após percorrer, mostro o vetor de string
   int lvl = 0;
   vector<string> levels(1);
   printAux(root, levels, lvl);

   for (string s : levels) {
      cout << s << endl;
   }
}

template <class T, const unsigned int MIN_DEGREE>
void diskbtree<T, MIN_DEGREE>::printAux(node<T, MIN_DEGREE> x, vector<string> &v, unsigned int lvl) {
   string str = "[";
   unsigned int i = 0;
   //caso tenha mudado de nivel, efetuo um resize em v
   if (v.size() < lvl + 1) {
      v.resize(lvl + 1);
   }
   //se x não é folha
   if (!x.isleaf()) {
      //visito meus filhos, e chamo o printaux recursivamente e aumentando o nivel
      for (i = 0; i <= x.getSize(); i++) {
         if (x.getChildren(i) != x.NOT_FOUND){
            node<T, MIN_DEGREE> aux = this->readNode(x.getChildren(i));
            printAux(aux, v, lvl + 1);
         }
      }
   }
   //salvo em uma string todas as minhas chaves
   for (i = 0; i < x.getSize(); i++) {
      str += (x.getkey(i).getValue()) + ", ";
   }
   //organizo a string
   if (str.length() > 1) {
      str += "\b\b] ";
   } else {
      str += "] ";
   }
   //somo no vetor com seu lvl indicado
   v[lvl] += str;
}

#endif // DISKBTREE_H_INCLUDED
