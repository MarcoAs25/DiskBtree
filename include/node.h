#ifndef NODE_H_INCLUDED
#define NODE_H_INCLUDED

#include<iostream>
#include <vector>

using namespace std;

template <class T, const unsigned int MIN_DEGREE>
class node: serializable {
public:
    static const unsigned int MAX = 2 * MIN_DEGREE - 1;
    static const unsigned int MIN = MIN_DEGREE - 1;
    static const unsigned int NOT_FOUND = -1;
    ~node();
    node();
    node(bool leaf);
    node(const node& other);
    node<T, MIN_DEGREE> operator=(const node<T, MIN_DEGREE>& other);
    bool isleaf() const { return this->leaf; }
    void setLeaf(bool leaf){ this->leaf = leaf; }
    unsigned int getSize() const { return this->n; }
    void setSize(unsigned int n){ this->n = n; }
    void setChildren(unsigned int i, unsigned int indexOfDisk){ this->children[i] = indexOfDisk; }
    unsigned int getChildren(unsigned int i){ return this->children[i]; }
    void setkey(unsigned int i, T key){ this->keys[i] = key; }
    T getkey(unsigned int i){ return this->keys[i]; }
    virtual string toString();
    virtual void fromString(string repr);
    virtual string toXML();
    virtual void fromXML(string repr);
    virtual string toCSV();
    virtual void fromCSV(string repr);
    virtual string toJSON();
    virtual void fromJSON(string repr);
    virtual unsigned long long int size() const;
private:
    vector<T> keys;
    vector<unsigned int> children;
    bool leaf;
    unsigned int n;
};

template <class T, const unsigned int MIN_DEGREE>
node<T, MIN_DEGREE>::node(){

   unsigned int i;
   this->setLeaf(true);
   this->setSize(0);

   keys.resize(MAX);
   children.resize(MAX + 1);

   for (i = 0; i < children.size(); i++) {
      this->setChildren(i,NOT_FOUND);
   }
}

template <class T, const unsigned int MIN_DEGREE>
node<T, MIN_DEGREE>::node(bool leaf){

   unsigned int i;
   this->setLeaf(leaf);
   this->setSize(0);

   keys.resize(MAX);
   children.resize(MAX + 1);

   for (i = 0; i < children.size(); i++) {
      this->setChildren(i,NOT_FOUND);
   }
}

template <class T, const unsigned int MIN_DEGREE>
node<T, MIN_DEGREE>::~node(){

}

template <class T, const unsigned int MIN_DEGREE>
node<T, MIN_DEGREE>::node(const node<T, MIN_DEGREE>& other){

    this->setLeaf(other.isleaf());
    this->setSize(other.getSize());

    keys.resize(MAX);
    children.resize(MAX + 1);

    this->children = other.children;
    this->keys = other.keys;
}

template <class T, const unsigned int MIN_DEGREE>
string node<T, MIN_DEGREE>::toString() {

    string aux = "";
    aux += string(reinterpret_cast<char*>(&this->leaf), sizeof(this->leaf));
    aux += string(reinterpret_cast<char*>(&this->n), sizeof(this->n));

    for(unsigned int i=0; i< keys.size(); i++){
        aux += keys[i].toString();
    }

    for(unsigned int i=0; i< children.size(); i++){
        aux += string(reinterpret_cast<char*>(&this->children[i]), sizeof(this->children[i]));
    }

    return aux;
}

template <class T, const unsigned int MIN_DEGREE>
void node<T, MIN_DEGREE>::fromString(string repr) {
    int pos = 0;

    repr.copy(reinterpret_cast<char*>(&this->leaf), sizeof(this->leaf), pos);
    pos += sizeof(this->leaf);

    repr.copy(reinterpret_cast<char*>(&this->n), sizeof(this->n), pos);
    pos += sizeof(this->n);

    string datastring = "";
    for(unsigned int i=0; i< keys.size(); i++){
        datastring = repr.substr(pos,keys[i].size());
        keys[i].fromString(datastring);
        pos += keys[i].size();
        datastring = "";
    }

    for(unsigned int i=0; i< children.size(); i++){
        repr.copy(reinterpret_cast<char*>(&this->children[i]), sizeof(this->children[i]), pos);
        pos += sizeof(this->children[i]);
    }
}

template <class T, const unsigned int MIN_DEGREE>
string node<T, MIN_DEGREE>::toXML() {
    return "";//não utilizado
}

template <class T, const unsigned int MIN_DEGREE>
void node<T, MIN_DEGREE>::fromXML(string repr) {
//não utilizado
}

template <class T, const unsigned int MIN_DEGREE>
string node<T, MIN_DEGREE>::toCSV() {
    return "";//não utilizado
}

template <class T, const unsigned int MIN_DEGREE>
void node<T, MIN_DEGREE>::fromCSV(string repr) {
//não utilizado
}

template <class T, const unsigned int MIN_DEGREE>
string node<T, MIN_DEGREE>::toJSON() {
    return "";//não utilizado
}

template <class T, const unsigned int MIN_DEGREE>
void node<T, MIN_DEGREE>::fromJSON(string repr) {
//não utilizado
}

template <class T, const unsigned int MIN_DEGREE>
unsigned long long int node<T, MIN_DEGREE>::size() const {
    return keys.size()*keys[0].size() + children.size()*sizeof(children[0]) + sizeof(leaf) + sizeof(n);
}

template <class T, const unsigned int MIN_DEGREE>
node<T, MIN_DEGREE> node<T, MIN_DEGREE>::operator=(const node<T, MIN_DEGREE>& other) {
    if(this != &other){
        this->children = other.children;
        this->keys = other.keys;
        this->leaf = other.leaf;
        this->n = other.n;
        return *this;
    }
    return *this;
}

#endif // NODE_H_INCLUDED
