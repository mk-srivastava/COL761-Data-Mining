#include<string>
#include"io_handler.h"
#include"helper.h"
using namespace std;
template<typename T, typename C>
struct Apriori {
    // file
    string filename;
    // load file
    FILE* load_file(string mode="r"){
        FILE *file=fopen(filename.c_str(),mode.c_str());
        file_error(file);
        return file;
    }
    void file_error(FILE *file){
        if(!file){
            cout<<filename<<" absent"<<endl;
            assert(false);
        }
    }
    void close_file(FILE *file){
        fclose(file);
    }
    // transactions
    int nTransactions;
    // support threshold
    float support_threshold; 
    int nSupportThreshold; // minimum transactions needed for bypassing support threshold
    // Apriori parameters
    int K;
    map<C,int> C_K;
    set<C> F_K;
    vector<C> frequent_itemsets;
    // Member functions
    Apriori(string s, float threshold){
        // filename constructor
        filename=s;
        // no. of transactions constructor
        FILE *file=load_file();
        nTransactions=0;
        vector<T> temp;
        while(getSingleTransaction<vector<T> >(temp,file)){
            nTransactions++;
        }
        close_file(file);
        // support threshold constructor
        support_threshold=threshold;
        nSupportThreshold=(support_threshold*nTransactions)/100;
        if(nSupportThreshold<(support_threshold*nTransactions)/100){
            nSupportThreshold++;
        }
        // Apriori Paramenters constructor
        K=0;
    }
    void getFrequent(){
        if(C_K.size()<2){ // no. of items in last stage is insufficient to keep continuing the analysis
            return;
        }
        cout<<K<<":::::::::::::::::"<<endl;
        getCandidates();
        for(pair<C,int> p:C_K){
            // calculate the support in the transactions
            FILE *file=load_file();
            vector<T> transaction;
            for(int i=0;i<nTransactions;i++){
                getSingleTransaction<vector<T> >(transaction,file);
#ifdef NOT_SORTED 
                sort(all(transaction));
#endif  
                // Manish: can be optimised
                if(isSubsetVector<T>(p.first,transaction)){
                    p.second++;
                }
            }
            close_file(file);
            if(p.second>=nSupportThreshold){
                F_K.insert(F_K.end(),p.first);
                frequent_itemsets.insert(frequent_itemsets.end(),p.first);
            }
        }
        K=K+1;
        getFrequent(); 
    }
    void getCandidates(){
        if(K!=0){
            // 1a stage is to merge two elements of set C_K so that new element will have a size of C_(K+1)
            C_K.clear();
            for(C itemset_1:F_K){
                for(C itemset_2:F_K){
                    if(itemset_1==itemset_2){
                        continue;
                    }
                    // Manish: can be optimised
                    set<T> merged=vector_to_set<T>(merge<T>(itemset_1,itemset_2));
                    if(merged.size()==K+1){
                        // 1b to get all elements of size K of the new set and check whether it exists in the C_K
                        bool flag=true;
                        for(T itemset:merged){
                            merged.erase(itemset);
                            if(F_K.find(set_to_vector<int>(merged))==F_K.end()){
                                flag=false;
                                break;
                            }
                            merged.insert(itemset);
                        }
                        if(flag){
                            C_K.insert({set_to_vector<int>(merged),0});
                        }
                    }
                }
            }
        }
        else{
            set<T> s;
            FILE *file=load_file();
            // populate C_K
            for(int i=0;i<nTransactions;i++){
                vector<T> transaction;
                getSingleTransaction<vector<T> >(transaction,file);
                for(T item:transaction){
                    s.insert(item);
                }
            }
            close_file(file);
            // set<int> to vector<vector<int> > 
            set<vector<int> > v;
            for(int item: s){
                v.insert(vector<int>(1,item));
            }
            C_K=set_to_map<vector<int>,int,map<vector<int>,int > >(v,0);
        }
    }
    vector<C> getAllFrequentItemsets(){
        getCandidates();
        getFrequent();
        return frequent_itemsets;
    }
};  