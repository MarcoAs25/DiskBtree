#ifndef CHARSERIAL_H_INCLUDED
#define CHARSERIAL_H_INCLUDED

#include "serializable.h"

class charserial : serializable {
   public:
      charserial();
      charserial(char c);
      charserial(const charserial& other);
      virtual ~charserial();
      charserial operator=(const charserial& other);
      bool operator==(const charserial &other)const;
      bool operator<(const charserial &other)const;
      bool operator<=(const charserial &other)const;
      bool operator>(const charserial &other)const;
      bool operator>=(const charserial &other)const;
      bool operator!=(const charserial &other)const;
      void setValue(char c);
      string getValue() const;
      char lowCase(const char c) const ;
      char upperCase(const char c) const;
      virtual string toString();
      virtual void fromString(string repr);
      virtual string toXML();
      virtual void fromXML(string repr);
      virtual string toCSV();
      virtual void fromCSV(string repr);
      virtual string toJSON();
      virtual void fromJSON(string repr);
      virtual unsigned long long int size() const;
   protected:
      char value;
};


#endif // CHARSERIAL_H_INCLUDED
