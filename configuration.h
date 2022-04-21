/* Copyright © 2001-2014, Hove and/or its affiliates. All rights reserved.

This file is part of Navitia,
    the software to build cool stuff with public transport.

Hope you'll enjoy and contribute to this project,
    powered by Hove (www.hove.com).
Help us simplify mobility and open public transport:
    a non ending quest to the responsive locomotion way of traveling!

LICENCE: This program is free software; you can redistribute it and/or modify
it under the terms of the GNU Affero General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU Affero General Public License for more details.

You should have received a copy of the GNU Affero General Public License
along with this program. If not, see <http://www.gnu.org/licenses/>.

Stay tuned using
twitter @navitia
IRC #navitia on freenode
https://groups.google.com/d/forum/navitia
www.navitia.io
*/

#pragma once

#include <boost/lexical_cast.hpp>
#include <boost/thread/shared_mutex.hpp>

#include <map>
#include <string>

/** Configuration est un singleton : c'est à dire qu'on l'utilise
 * Configuration * conf = Configuration::get();
 * Au premier appel une instance sera créée
 * Aux appels suivants, la même instance sera utilisée
 * Cela permet d'avoir une configuration unique sans utiliser de variables globales
 */
struct Configuration {
private:
    static Configuration* instance;
    /// Dictionnaire de chaînes de caractères
    /// Par défaut contient au moins :
    /// application : le nom de la dll ou de l'exécutable
    /// path : le chemin où se situe la dll ou l'exécutable
    std::map<std::string, std::string> strings;
    std::map<std::string, int> ints;
    std::map<std::string, float> floats;

    /// Mutex permetant multiple-read/single-write
    boost::shared_mutex mutex;

public:
    /// Premet de récupérer l'instance de configuration
    static Configuration* get();

    /// Retourne vrai si la configuration a déjà été instancié
    static bool is_instanciated();

    /// Récupère une chaîne de caractères
    std::string get_string(const std::string& key);

    /// Récupère un entier
    int get_int(const std::string& key);

    /// Définit une clef-valeur dont la valeur est une string
    void set_string(const std::string& key, const std::string& value);

    /// Définit une clef-valeur dont la valeur est un entier
    void set_int(const std::string& key, int value);

    /// Contient les données du fichier ini
    /// Pour y accéder ini[section][clef]
    /// Si la clef ou la section n'existe pas, retourne une string vide
    /// C'est à l'utilisateur de faire la conversion qui l'intéresse
    std::map<std::string, std::map<std::string, std::string> > ini;

    /// Retourne vrai si la section existe
    bool has_section(const std::string& section_name);

    /// Exception levée lorsque qu'on ne trouve pas un élément
    struct not_found {};

    /// Cherche une clef dans une section et effectue la conversion
    /// On doit spécifier la valeur par défaut lorsque la section ou la clef n'existe pas
    template <class T>
    T get_as(const std::string& section, const std::string& key, T default_value) {
        auto section_it = ini.find(section);
        if (section_it == ini.end())
            return default_value;
        auto element_it = section_it->second.find(key);
        if (element_it == section_it->second.end())
            return default_value;
        try {
            return boost::lexical_cast<T>(element_it->second);
        } catch (...) {
            return default_value;
        }
    }

    /// Charge un fichier ini
    /// Il est possible de charger plusieurs fichiers
    /// Si dans deux fichiers il y a la même section et la même clef, la valeur sera écrasée
    void load_ini(const std::string& filename);
};
