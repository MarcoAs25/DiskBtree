#include "charserial.h"
#include <iostream>
using namespace std;

charserial::charserial() : serializable() {
    this->value = '\0';
}

charserial::charserial(char c) : serializable() {
    this->value = c;
}

charserial::charserial(const charserial& other) {
    this->value = other.value;
}

charserial::~charserial() {

}

charserial charserial::operator=(const charserial& other) {
    if(this != &other){
        this->value = other.value;
        return *this;
    }
    return *this;
}

bool charserial::operator==(const charserial &other) const{

    return this->lowCase(this->value) == other.lowCase(other.value);
}

bool charserial::operator<(const charserial &other) const{
    return this->lowCase(this->value) < other.lowCase(other.value);
}

bool charserial::operator<=(const charserial &other) const{
    return this->lowCase(this->value) <= other.lowCase(other.value);
}

bool charserial::operator>(const charserial &other) const{
    return this->lowCase(this->value) > other.lowCase(other.value);
}

bool charserial::operator>=(const charserial &other) const{
    return this->lowCase(this->value) >= other.lowCase(other.value);
}

bool charserial::operator!=(const charserial &other) const{

    return this->lowCase(this->value) != other.lowCase(other.value);

}

void charserial::setValue(char c) {
    this->value = c;
}
//retorna o valor em string
string charserial::getValue() const {

    string aux(1,this->value);
    return aux;
}
//retorna um caracter em caixa baixa(caso o caracter esteja entre a - z ou A - Z)
char charserial::lowCase(const char c) const{
    bool isUpper = (c <= 'Z' && c >= 'A');
    bool islow = (c <= 'z' && c >= 'a');

    if(islow){
        return c;
    }else if(isUpper){
        return c + 32;
    }else{
        return c;
    }
}
//retorna um caracter em caixa ALTA(caso o caracter esteja entre a - z ou A - Z)
char charserial::upperCase(const char c)const {
    bool isUpper = (c <= 'Z' && c >= 'A');
    bool islow = (c <= 'z' && c >= 'a');

    if(islow){
        return c - 32;
    }else if(isUpper){
        return c;
    }else{
        return c;
    }
}
//transforma os atributos em uma string
string charserial::toString() {
    string repr = "";
    repr += string(reinterpret_cast<char*>(&this->value), sizeof(this->value));
    return repr;
}
//transforma preenche os atributos do objeto com uma string que representa
void charserial::fromString(string repr) {
    int pos = 0;
    int asciivalue = 0;
    repr.copy(reinterpret_cast<char*>(&asciivalue), sizeof(this->value), pos);
    this->value = char(asciivalue);
    pos += sizeof(this->value);
}

string charserial::toXML() {
    return "";
}

void charserial::fromXML(string repr) {
//não utilizado
}

string charserial::toCSV() {
    return "";
}

void charserial::fromCSV(string repr) {
//não utilizado
}

string charserial::toJSON() {
    return "";
}

void charserial::fromJSON(string repr) {
//não utilizado
}
//Tamanho do charSerial
unsigned long long int charserial::size() const {
   return sizeof(this->value);
}
