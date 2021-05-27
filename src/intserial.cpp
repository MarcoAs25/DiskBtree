#include "intserial.h"
intSerial::intSerial() : serializable() {
    this->value = 0;
}

intSerial::intSerial(int i) : serializable() {
    this->value = i;
}

intSerial::intSerial(const intSerial& other) {
    this->value = other.value;
}

intSerial::~intSerial() {

}

intSerial intSerial::operator=(const intSerial& other) {
    if(this != &other){
        this->value = other.value;
        return *this;
    }
    return *this;
}

bool intSerial::operator==(const intSerial &other) const {
    return this->value == other.value;
}

bool intSerial::operator<(const intSerial &other) const {
    return this->value < other.value;
}

bool intSerial::operator<=(const intSerial &other) const {
    return this->value <= other.value;
}

bool intSerial::operator>(const intSerial &other) const {
    return this->value > other.value;
}

bool intSerial::operator>=(const intSerial &other) const {
    return this->value >= other.value;
}

bool intSerial::operator!=(const intSerial &other) const {
    return this->value != other.value;
}

intSerial intSerial::operator+(const intSerial& other) const {
    intSerial aux(this->value + other.value);
    return aux;
}

intSerial intSerial::operator-(const intSerial& other) const {
    intSerial aux(this->value - other.value);
    return aux;
}

intSerial intSerial::operator*(const intSerial& other) const {
    intSerial aux(this->value * other.value);
    return aux;
}

intSerial intSerial::operator/(const intSerial& other) const {
    intSerial aux(this->value / other.value);
    return aux;
}

intSerial intSerial::operator++() {
    this->value = this->value + 1;
    intSerial aux(this->value);
    return aux;
}

intSerial intSerial::operator--() {
    this->value = this->value - 1;
    intSerial aux(this->value);
    return aux;
}

void intSerial::setValue(int v) {
    this->value = v;
}

int intSerial::getValue() const {
    return this->value;
}

string intSerial::toString() {
    string repr = "";
    repr += string(reinterpret_cast<char*>(&this->value), sizeof(this->value));
    return repr;
}

void intSerial::fromString(string repr) {
    int pos = 0;
    repr.copy(reinterpret_cast<char*>(&this->value), sizeof(this->value), pos);
    pos += sizeof(this->value);
}

string intSerial::toXML() {
    return "";
}

void intSerial::fromXML(string repr) {
//não utilizado
}

string intSerial::toCSV() {
    return "";
}

void intSerial::fromCSV(string repr) {
//não utilizado
}

string intSerial::toJSON() {
    return "";
}

void intSerial::fromJSON(string repr) {
//não utilizado
}
unsigned long long int intSerial::size() const {
   return sizeof(this->value);
}
