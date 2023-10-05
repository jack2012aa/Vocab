#include <vector>
#include <iostream>
#include <string>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/vector.hpp>

class Word{
    // A word that is used to put into a dictionary.

    private:
        std::string vocab;
        std::string chinese;
        std::vector<std::string> phrases;
        std::vector<std::string> synonymouses;
        // Used in relation tree in Dictionary to represnet synonymouse relation.
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

        std::string get_chinese(){
            return this -> chinese;
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

        std::string get_synonymouses_string(){
            std::string result = "";
            for (int i = 0; i < synonymouses.size(); i++){
                result.append(synonymouses[i]);
                result.append(" ");
            }
            return result;
        }

            std::string get_phrases_string(){
            std::string result = "";
            for (int i = 0; i < phrases.size(); i++){
                result.append(phrases[i]);
                result.append(", ");
            }
            return result;
        }


};

class Dictionary{

    private:

        // A map helps faster access.
        std::vector<Word> map;
        unsigned int map_length;
        // Store words.
        std::vector<Word> vec;

        friend class boost::serialization::access;
        template<class Archive>
        void serialize(Archive & ar, const unsigned int version)
        {
            ar & map;
            ar & map_length;
            ar & BOOST_SERIALIZATION_NVP(vec);
        }


        int hash(Word w, unsigned int base){
            // Return a hash value which fix in the dictionary_length.
            // If dictionary has not initialized, -1 will be returned.

            if (this -> map_length == 0){
                return -1;
            }

            int n = 0;
            for(int i = 0; i < w.get_vocab().length(); i++){
                n = (n * 10 + w.get_vocab().at(i)) % this -> map_length;
            }
            n += base;
            n = n % this -> map_length;
            return n;

        }

    public:

        Dictionary(){
            this -> map_length = 2000;
            for (int i = 0; i < 2000; i++){
                map.push_back(Word(""));
            }
        }

        Dictionary(int n){
            this -> map_length = n;
            for (int i = 0; i < n; i++){
                map.push_back(Word(""));
            }
        }

        Word create_word(){
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

            return Word(v, c, phrases, synonymouses, -1);

        }

        int insert_word(Word w){
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

        int add_word(){
            // Create a new word and insert into dictionary.
            // Return the hash value of the word.
            // If dictionary map_length is zero, do nothing.

            if (this -> map_length == 0){
                std::cout << "The dictionary has not been initialized." << std::endl;
                return -1;
            }

            Word w = create_word();

            return insert_word(w);

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
                    break;
                }
            }
            for (int i = 0; i < vocabs.size(); i++){
                p = insert_word(Word(vocabs[i], c, vocabs, p));
            }

        }

        std::vector<Word> get_words_list(){

            return this -> vec;

        }

        int find_root(Word w){
            // Return the root vocab in the tree.

            int current = get_index(w); 
            if (current == -1){
                return -1;
            }
            w = map[current];
            int next = w.get_parent();
            while (next != -1){
                current = next;
                next = this -> map[next].get_parent();
            }
            return current;

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

        bool is_synonymouse(std::string vocab1, std::string vocab2){

            return find_root(Word(vocab1)) == find_root(Word(vocab2));

        }

        int get_length(){
            return this -> vec.size();
        }

};

class Teacher{

    private:
    
        Dictionary dict;
        std::vector<Word> test;
        int next_question = 0, score = 0;

        void swap(int i, int j){
            if (i == j){
                return;
            }
            Word temp = test[i];
            test[i] = test[j];
            test[j] = temp;
        }

    public:

        Teacher(){};

        Teacher(Dictionary dict){
            this -> dict = dict;
        }

        void generate_test(int n_of_words){
            // Get a list of vocab from the library and reorder it.

            test.clear();
            std::vector<Word> list = dict.get_words_list();
            for (int i = list.size() - n_of_words; i < list.size(); i++){
                test.push_back(list[i]);
            }
            int n = test.size();
            srand(time(NULL));

            for (int i = 0; i < 3 * n; i++){
                swap(rand() % n, rand() % n);
            }
            next_question = 0;
            score = 0;

        }

        void check_english_answer(std::string answer){
            // Check whether the answer is synonymouse to the original word.
            if (dict.is_synonymouse(test[next_question].get_vocab(), answer)){
                score++;
                std::cout << "Correct!" << std::endl;
                std::cout << "Other similar words: "<< test[next_question].get_synonymouses_string() << std::endl;
                std::cout << "Usage: " << test[next_question].get_phrases_string() << std::endl;
                return;
            } else{
                std::cout << "Incorrect!" << std::endl;
                std::cout << "Answer: " << test[next_question].get_chinese() << " " << test[next_question].get_vocab() << " " << test[next_question].get_synonymouses_string() << std::endl;
                std::cout << "Usage: " << test[next_question].get_phrases_string() << std::endl;
                return;
            }

        }

        void ask_e_to_c_question(){
            // Print a vocab in English. 
            // Students should answer its Chinese meaning or give a synonymouse of it.

            std::cout << "What is the meaning of " << test[next_question].get_vocab() << "?" << std::endl;
            std::string answer;
            getline(std::cin,answer);

            // If the student answer in Chinese:
            if (answer.compare(test[next_question].get_chinese()) == 0){
                score++;
                std::cout << "Correct!" << std::endl;
                std::cout << "Other similar words: "<< test[next_question].get_synonymouses_string() << std::endl << std::endl;
                next_question++;
                return;
            }

            // If the student answer in English:
            check_english_answer(answer);
            next_question++;

        }

        void ask_c_to_e_question(){
            // Print a vocab in Chinese.
            // Student should answer in English.

            std::cout << "What is " << test[next_question].get_chinese() << " in English?" << std::endl;
            std::string answer;
            getline(std::cin,answer);
            check_english_answer(answer);
            next_question++;

        }

};

int main(){

    std::string next;
    Dictionary dict = Dictionary(2000);
    Teacher teacher;
    std::cout << "Load dictionary -> other operations" << std::endl;
    {
    std::ifstream ifs("save.txt");
    boost::archive::text_iarchive ia(ifs);
    ia >> dict;
    teacher = Teacher(dict);    
    }

    while (true){
        
        std::cout << "Add word: A; Add words: As; Generate test: G; English to Chinese question: EtC; Chinese to English question: CtE; Have a test on today's new vocabs: Today; End: E" << std::endl;
        getline(std::cin, next);

        if (next.compare("E") == 0){
            return 0;
        } else if (next.compare("A") == 0){
            dict.add_word();
            std::ofstream ofs("save.txt");
            boost::archive::text_oarchive oa(ofs);
            oa << dict;
        } else if (next.compare("As") == 0){
            int old = dict.get_length();
            dict.add_words();
            std::ofstream ofs("save.txt");
            boost::archive::text_oarchive oa(ofs);
            oa << dict;
        } else if (next.compare("EtC") == 0){
            teacher.ask_e_to_c_question();
        } else if (next.compare("CtE") == 0){
            teacher.ask_c_to_e_question();
        } else if (next.compare("G") == 0){
            teacher.generate_test(dict.get_length());
        } else if (next.compare("temp") == 0){
            std::string s;
            getline(std::cin, s);
            std::cout << dict.get_index(Word(s)) << std::endl;
            std::cout << dict.find_root(Word(s)) << std::endl;
        } else if (next.compare("Today") == 0){
            teacher.generate_test(50);
            while(true){
                teacher.ask_e_to_c_question();
            }
        }
        
        
    }

    return 0;

}