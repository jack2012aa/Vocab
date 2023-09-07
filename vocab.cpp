#include <vector>
#include <iostream>
#include <string>
#include <fstream>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/vector.hpp>

class Word{
    // A word that is used to put into a dictionary.

    private:
        std::string vocab;
        std::string chinese;
        std::vector<std::string> phrases;
        std::vector<std::string> synonymouses;
        int parent = -1;

        friend class boost::serialization::access;
        template<class Archive>
        void serialize(Archive & ar, const unsigned int version)
        {
            ar & vocab;
            ar & chinese;
            ar & phrases;
            ar & synonymouses;
            ar & parent;
        }

    public:

        Word(){
            // Create a single word by user input which parent = -1.

            std::string v, c, p, s;
            std::vector<std::string> phrases, synonymouses;
            std::cout << "Key in the word: ";
            std::getline(std::cin, v);
            std::cout << "Key in its Chinese: ";
            std::getline(std::cin, c);

            do{
                std::cout << "Does it has any phrase? Key in 'END' to terminate. ";
                std::getline(std::cin, p);
                if (p.compare("END") == 0){
                    break;
                }
                phrases.push_back(p);
            } while(1);

            do{
                std::cout << "Does it has any synonymous? Key in 'END' to terminate. ";
                std::getline(std::cin, s);
                if (s.compare("END") == 0){
                    break;
                }
                phrases.push_back(s);
            } while(1);

            this -> vocab = v;
            this -> chinese = c;
            this -> phrases = phrases;
            this -> synonymouses = synonymouses;

        }

        Word (std::string v, std::string c, std::vector<std::string> ps, std::vector<std::string> ss, int p){
            // The basic constructor.
            this -> vocab = v;
            this -> chinese = c;
            this -> phrases = ps;
            this -> synonymouses = ss;
            this -> parent = p;
        }

        Word (std::string v, std::string c, std::vector<std::string> ss, int p){
            // The basic constructor.
            this -> vocab = v;
            this -> chinese = c;
            this -> synonymouses = ss;
            this -> parent = p;
        }

        Word (std::string v){
            // Temp constructor.
            this -> vocab = v;
        }
        
        std::string get_vocab(){
            return this -> vocab;
        }

        std::vector<std::string> get_synonymouses(){
            return this -> synonymouses;
        }

        int get_parent(){
            return this -> parent;
        }

        void set_parent(int p){
            if (p < 0){
                std::cout << "set_parent: p < 0." << std::endl;
                return;
            }
            this -> parent = p;
        }

};

class Dictionary{

    private:

        Word* map;
        // A map describe which words are in the dictionary.
        unsigned int map_length;
        std::vector<Word> vec;

        int hash(Word w, unsigned int base){
            // Return a hash value which fix in the dictionary_length.
            // If dictionary has not initialized, -1 will be returned.

            if (this -> map_length == 0){
                return -1;
            }

            int n = 0;
            for(int i = 0; i < w.get_vocab().length(); i++){
                n = n * 10 + w.get_vocab().at(i);
            }
            n += base;
            n = n % this -> map_length;
            return n;

        }

    public:

        Dictionary(int n){
            // Initialize a dictionary with size n.

            this -> map = (Word*) malloc(n * sizeof(Word));
            this -> map_length = n;
        }

        int add_word(Word w){
            // Add this word into dictionary.
            // Return the hash value of the word.
            // If dictionary map_length is zero, do nothing.

            if (this -> map_length == 0){
                std::cout << "The dictionary has not been initialized." << std::endl;
                return -1;
            }

            // Find the appropriate hash value of the word.
            int base = 0, h;
            do{
                h = this -> hash(w, base);
                if (map[h].get_vocab().empty()){
                    break;
                }
                if (map[h].get_vocab().compare(w.get_vocab()) == 0){
                    return -1;
                }
                base++;
            } while(1);

            // Find one of its synonymouses' hash value as its parent, if exists.
            int p = -1;
            std::vector<std::string> ss = w.get_synonymouses();
            for(int i = 0; i < ss.size(); i++){
                int index = get_index(Word(ss[i]));
                if (index >= 0){
                    p = index;
                    break;
                }
            }
            w.set_parent(p);

            map[h] = w;
            vec.push_back(w);
            return h;

        }

        void add_words(){
            // Add multiple words with same meaing at once. 

           if (this -> map_length == 0){
                std::cout << "The dictionary has not been initialized." << std::endl;
                return;
            }

            // Key in all vocabs.
            std::vector<std::string> vocabs;
            do{
                std::cout << "Key in words one by one. Key in 'END' to terminate: ";
                std::string s;
                getline(std::cin, s);
                if (s.compare("END") == 0){
                    break;
                }
                vocabs.push_back(s);
            } while(1);

            std::string c;
            std::cout << "Key in their Chinese: " << std::endl;
            getline(std::cin, c);
            
            int p = -1;
            for (int i = 0; i < vocabs.size(); i++){
                p = get_index(Word(vocabs[i]));
                if (p != -1){
                    for (int j = 0; j < vocabs.size(); j++){
                        add_word(Word(vocabs[j], c, vocabs, p));
                    }
                    return;
                }
            }
            for (int i = 0; i < vocabs.size(); i++){
                p = add_word(Word(vocabs[i], c, vocabs, p));
            }

        }

        std::vector<Word> get_words_list(){

            return this -> vec;

        }

        std::string find_parent(Word w){
            // Return the root vocab in the tree.

            int current = get_index(w); 
            int next = w.get_parent();
            while (next != -1){
                current = next;
                next = this -> map[next].get_parent();
            }
            return map[current].get_vocab();

        }

        int get_index(Word w){
            // Get the index of word w in the map.

            int h, base = 0;
            while (1){
                h = this -> hash(w, base);
                if (map[h].get_vocab().empty()){
                    return -1;
                }
                if (map[h].get_vocab().compare(w.get_vocab()) == 0){
                    return h;
                }
                base++;
            }

        }

        std::string temp(){
            return map[0].get_vocab();
        }

};

class Teacher{

    private:
    
    public:



};

int main(){

    Word w = Word();
    
    {
        std::ofstream ofs("test.txt");
        boost::archive::text_oarchive oa(ofs);
        oa << w;
    }

    Word newW;
    {   
        std::ifstream ifs("test.txt");
        boost::archive::text_iarchive ia(ifs);
        ia >> newW;
    }
    std::cout << newW.get_vocab();

    return 0;

}