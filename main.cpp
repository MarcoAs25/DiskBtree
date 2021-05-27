/********************************************************
 * main.cpp - In-Memory B-Tree main program file        *
 *                                                      *
 * Author:  Marco Ant�nio. 2021                         *
 * RA: 0040396                                          *
 * Purpose:  disk b-tree algorithm.                     *
 *                                                      *
 * Obs:This code is part of one of the final works of   *
 * the technical programming discipline.                *
 ********************************************************/

#include <iostream>
#include <clocale>
#include "intserial.h"
#include "charserial.h"
#include "diskbtree.h"

using namespace std;

int main() {

    setlocale(LC_ALL, "Portuguese");
    diskbtree<charserial, 3> disk("file.dat","DBT",3);
    char vetorpraInserir[20] = {')', '6', 'O', 'b', 'L', 'G', 'P', 'C', '3', '%', 'm', '(', 'H', 'W', 'Y', '&', 'k', '1', 'X', '-'};
    char vetorpraRemover[10] = {'C', 'm', '1', 'O', ')', 'W', 'Y', 'L', 'X', '('};
    charserial character;
    char valor;
    string op;
    int n;

    if(disk.isOpen()){
        do{
            cout<<" ------------------------------------------------- "<<endl;
            cout<<"|               �rvore B em Mem�ria               |"<<endl;
            cout<<"|1 - Inserir um caractere                         |"<<endl;
            cout<<"|2 - remover um caractere                         |"<<endl;
            cout<<"|3 - Buscar Um caractere                          |"<<endl;
            cout<<"|4 - imprimir �rvore                              |"<<endl;
            cout<<"|5 - Teste automatizado                           |"<<endl;
            cout<<" ------------------------------------------------- "<<endl;
            cout<<"Digite uma op��o: ";
            cin>>n;

            switch(n){
                case 1:
                    cout<<"Digite um character: ";
                    cin>>valor;
                    character.setValue(valor);
                    if(disk.insertKey(character)){
                        disk.print();
                    }else{
                        cout << "N�o foi poss�vel inserir :(" << endl;
                    }
                    break;
                case 2:
                    cout<<"Digite um character: ";
                    cin>>valor;
                    character.setValue(valor);
                    if(disk.remove(character)){
                        disk.print();
                    }else{
                        cout<<"n�o foi poss�vel remover :("<<endl;
                    }
                    break;
                case 3:
                    cout<<"Digite um character: ";
                    cin>>valor;
                    character.setValue(valor);
                    if(disk.search(character)){
                        cout<<"Valor encontrado"<<endl;
                    }else{
                        cout<<"Valor n�o encontrado"<<endl;
                    }
                    break;
                case 4:
                    disk.print();
                    break;
                case 5:
                    cout<<endl;
                    cout<<"----------------------Efetuando as inser��es--------------------"<<endl;
                    cout<<endl;
                    for(int i = 0; i< 20; i++){
                        character.setValue(vetorpraInserir[i]);
                        if(disk.insertKey(character)){
                            cout<<"Mostrando ap�s a Inser��o de: "<< vetorpraInserir[i] <<endl;
                            disk.print();
                        }
                    }
                    cout<<endl;
                    cout<<"----------------------Efetuando as Remo��es--------------------"<<endl;
                    cout<<endl;
                    for(int i = 0; i< 10; i++){
                        character.setValue(vetorpraRemover[i]);
                        if(disk.remove(character)){
                            cout<<"Mostrando ap�s a Remo��o de: "<< vetorpraRemover[i] <<endl;
                            disk.print();
                        }
                    }
                    break;
                default:
                    cout<<"comando inv�lido!"<<endl;
            }
            cout<<"Continuar? (s) ou (n): ";
            cin>>op;

        }while(op == "s" || op == "S");
        disk.close();
    }else{
        cout<<"Verifique se: "<<endl;
        cout<<"- As vers�es entre o arquivo e a informada batem"<<endl;
        cout<<"- O grau da �rvore � o mesmo que a vers�o"<<endl;
        cout<<"- Caso o grau da �rvore n�o seja o mesmo que a vers�o, por favor, forne�a um arquivo com grau = ver"<<endl;
    }
   return 0;
}
