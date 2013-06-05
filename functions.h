#pragma once
#include <iostream>
#include <vector>

double str_to_double(std::string);
int str_to_int(std::string str);

/**
  Cette fonction permet de recupérer une chaine qui se trouve à une position donnée
  */
std::vector< std::string > split_string(const std::string&,const std::string & );

/** Foncteur permettant de comparer les objets en passant des pointeurs vers ces objets */
struct Less{
    template<class T>
        bool operator() (T* x, T* y) const{
            return *x < *y;
        }
};

/** Foncteur fixe le membre "idx" d'un objet en incrémentant toujours de 1
 *
 * Cela permet de numéroter tous les objets de 0 à n-1 d'un vecteur de pointeurs
 */
template<typename idx_t>
struct Indexer{
    idx_t idx;
    Indexer(): idx(0){}

    template<class T>
        void operator()(T* obj){obj->idx = idx; idx++;}
};
