#include <algorithm>
#include <thread>
#include <iostream>
#include <vector>
#include <chrono>

constexpr size_t MAXQUEEN=20;

using ui64 = unsigned long long;
using std::vector;
class TreeArray{
private:
    vector<int> tree;
    int size;
    inline int lowbit(int x){
        return x & (-x);
    }
public:
    TreeArray(int N):size(N){
        tree.resize(N+1,0);
    }
    void update(int pos,int num){
        while(pos<=size){
            tree[pos]+=num;
            pos+=lowbit(pos);
        }
    }
    int get(int pos){
        int result=0;
        while(pos>0){
            result+=tree[pos];
            pos-=lowbit(pos);
        }
        return result;
    }
};

template <size_t N>
struct getFact{
    ui64 num[N+1];
    constexpr getFact():num(){
        num[0]=1;
        for(int i=1;i<=N;i++){
            num[i]=i*num[i-1];
        }
    }
};
constexpr auto facts = getFact<MAXQUEEN>();

vector<int> reverse_cantor(size_t N,ui64 order){
    if(N>MAXQUEEN) return {};
    vector<int> result;
    TreeArray record(N);
    for(int i=1;i<=N;i++){
        record.update(i,1);
    }
    for(int i=N;i>=1;i--){
        int rank = order / facts.num[i-1];
        order %= facts.num[i-1];
        int left=1,right=N;
        while(left<right){
            int middle = (left+right) >> 1;
            if(record.get(middle)-1 >= rank){
                right = middle;
            } else {
                left = middle+1;
            }
        }
        result.push_back(right);
        record.update(right,-1);
    }
    return result;
}

bool checkQueens(const vector<int>& index){
    const int N=index.size();
    vector<bool> lDia(2*N-1,false),rDia(2*N-1,false);
    for(int i=0;i<N;++i){
        if(lDia[i-index[i]+N]){
            return false;
        } else {
            lDia[i-index[i]+N] = true;
        }
        if(rDia[i+index[i]-1]){
            return false;
        } else {
            rDia[i+index[i]-1] = true;
        }
    }
    return true;
}

void findQueens(size_t N, ui64 start, ui64 end, ui64& ways){
    auto index = reverse_cantor(N,start);
    ui64 result=0;
    for(auto i=start;i<end;i++){
        if(checkQueens(index)){ result++; }
        std::next_permutation(index.begin(),index.end());
    }
    std::cout << "Find " << result << " results in this thread.\n";
    ways+=result;
}

int main(){
    size_t N,T;
    std::cout << "How many queens are there: ";
    std::cin >> N;
    std::cout << "How many threads you want: ";
    std::cin >> T;
    vector<std::thread> threads;
    ui64 result = 0;
    ui64 begin = 0;
    for(int i=0;i<T;i++){
        ui64 end = i!=T-1? begin+facts.num[N]/T :facts.num[N];
        threads.emplace_back(std::thread(findQueens,N,begin,end,std::ref(result)));
        threads[i].join();
        begin = end;
    }
    std::cout << result << std::endl;
    return 0;
}
