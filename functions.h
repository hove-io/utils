#pragma once
#include <iostream>
#include <vector>
#include <memory>
#include<map>

double str_to_double(std::string);
int str_to_int(std::string str);

/**
  Cette fonction permet de recupérer une chaine qui se trouve à une position donnée
  */
std::vector< std::string > split_string(const std::string&,const std::string & );


/**
  Cette fonction permet de recupérer une valeur par une clef à partir de std::map<key, value>
  */
std::string value_by_key(const std::map<std::string, std::string>& vect, const std::string& key);

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

/**
 * Adding a make_unique for unique_ptr construction to ease use and ensure better exception safety
 */
#if __cplusplus <= 201103L //the make_unique will be added in c++14
namespace std {
template <typename T, typename ...Args>
unique_ptr<T> make_unique(Args&& ...args) {
    return unique_ptr<T>(new T(forward<Args>(args)...));
}
}
#endif
