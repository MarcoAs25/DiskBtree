/********************************************************
 * main.cpp - In-Memory B-Tree main program file        *
 *                                                      *
 * Author:  Marco Antônio. 2021                         *
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
            cout<<"|               Árvore B em Memória               |"<<endl;
            cout<<"|1 - Inserir um caractere                         |"<<endl;
            cout<<"|2 - remover um caractere                         |"<<endl;
            cout<<"|3 - Buscar Um caractere                          |"<<endl;
            cout<<"|4 - imprimir árvore                              |"<<endl;
            cout<<"|5 - Teste automatizado                           |"<<endl;
            cout<<" ------------------------------------------------- "<<endl;
            cout<<"Digite uma opção: ";
            cin>>n;

            switch(n){
                case 1:
                    cout<<"Digite um character: ";
                    cin>>valor;
                    character.setValue(valor);
                    if(disk.insertKey(character)){
                        disk.print();
                    }else{
                        cout << "Não foi possível inserir :(" << endl;
                    }
                    break;
                case 2:
                    cout<<"Digite um character: ";
                    cin>>valor;
                    character.setValue(valor);
                    if(disk.remove(character)){
                        disk.print();
                    }else{
                        cout<<"não foi possível remover :("<<endl;
                    }
                    break;
                case 3:
                    cout<<"Digite um character: ";
                    cin>>valor;
                    character.setValue(valor);
                    if(disk.search(character)){
                        cout<<"Valor encontrado"<<endl;
                    }else{
                        cout<<"Valor não encontrado"<<endl;
                    }
                    break;
                case 4:
                    disk.print();
                    break;
                case 5:
                    cout<<endl;
                    cout<<"----------------------Efetuando as inserções--------------------"<<endl;
                    cout<<endl;
                    for(int i = 0; i< 20; i++){
                        character.setValue(vetorpraInserir[i]);
                        if(disk.insertKey(character)){
                            cout<<"Mostrando após a Inserção de: "<< vetorpraInserir[i] <<endl;
                            disk.print();
                        }
                    }
                    cout<<endl;
                    cout<<"----------------------Efetuando as Remoções--------------------"<<endl;
                    cout<<endl;
                    for(int i = 0; i< 10; i++){
                        character.setValue(vetorpraRemover[i]);
                        if(disk.remove(character)){
                            cout<<"Mostrando após a Remoção de: "<< vetorpraRemover[i] <<endl;
                            disk.print();
                        }
                    }
                    break;
                default:
                    cout<<"comando inválido!"<<endl;
            }
            cout<<"Continuar? (s) ou (n): ";
            cin>>op;

        }while(op == "s" || op == "S");
        disk.close();
    }else{
        cout<<"Verifique se: "<<endl;
        cout<<"- As versões entre o arquivo e a informada batem"<<endl;
        cout<<"- O grau da árvore é o mesmo que a versão"<<endl;
        cout<<"- Caso o grau da árvore não seja o mesmo que a versão, por favor, forneça um arquivo com grau = ver"<<endl;
    }
   return 0;
}
