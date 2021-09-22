#pragma GCC optimize(2)
#include<map>
#include<unordered_map>
#include<set>
#include<unordered_set>
#include<cmath>
#include<queue>
#include<cstdio>
#include<vector>
#include<climits>
#include<cstring>
#include<cstdlib>
#include<iostream>
#include<algorithm>
#include<ctime>
#include <sys/io.h>
#include<stack>
#include<sstream>
#define open_file_name "test/p_hat1000-1.clq"
//#define open_file_name "test/brock200_2.clq"
using namespace std;

int cnt = 0;

const int maxn = 1e5 + 50;
int n , m;
vector<int>fin_ans;
long long C_initial = 0;
long long C_max = 0;
int cnt_edge = 0;
vector<int>V;
long long C_stop = 5436;
long long total_w_assigned[maxn];
long long first_total_w_assigned[maxn];
long long max_upper[maxn];

struct input_edge
{
    int v1;
    int v2;
    long long weight;
};
vector<input_edge> input_edge_cache; //from 1~n
vector<int> vertex_adject[maxn];
long long max_upper_num = 0;
unordered_map<unsigned long long , int>id_input_edge;

inline void init()
{
    V.clear();
    input_edge_cache.clear();
    input_edge ep;
    ep.v1 = -1; ep.v2 = -1; ep.weight = -1;
    input_edge_cache.push_back(ep);
}

unsigned long long encode_pairID(unsigned long long v1 , unsigned long long v2)
{
    unsigned long long n1 , n2;
    unsigned long long pairID;
    if (v1 < v2)
    {
        n1 = v1; n2 = v2;
    }
    else
    {
        n1 = v2; n2 = v1;
    }
    pairID = ( (n1 + n2 + 1) * (n1 + n2) >> 1) + n2;
    return pairID;
}


inline void add(int &x , int &y , long long &c)
{
    cnt_edge ++;
    input_edge temp;
    temp.v1 = x;
    temp.v2 = y;
    temp.weight = c;
    unsigned long long temp_id = encode_pairID(x , y);
    id_input_edge[temp_id] = cnt_edge;
    input_edge_cache.push_back(temp);
    /*vertex_degree[x].push_back(cnt_edge);
    vertex_degree[y].push_back(cnt_edge);*/
    vertex_adject[x].push_back(y);
    vertex_adject[y].push_back(x);
    max_upper[x] += c;
    max_upper[y] += c;
}

/*void pri(set<int>W)
{
    for (auto it : W)
    {
        cout<<it<<' ';
    }
    cout<<endl;
}*/

long long get_val(int &u , int &v)
{
    unsigned long long temp_id = encode_pairID(u , v);
    int tem = id_input_edge[temp_id];
    if (tem != 0) return input_edge_cache[tem].weight;
    else return 0;
}

long long vertex_upper_bound[maxn];
stack<int>vertex_sequence;
struct Node
{
    int id;
    int val;
    bool operator < (const Node &a) const
    {
        //if (val == a.val) return max_upper[id] < max_upper[a.id];
        return val > a.val;
    }
};

int flag_X[maxn];
long long res_sum[maxn];

inline void CALC_SEQ_AND_UB(vector<int>&vertex_C , vector<int>&vertex_S)
{
    while(!vertex_sequence.empty()) vertex_sequence.pop();
    /*for (auto v : vertex_S)
    {
        long long sum = 0;
        for (auto u : vertex_C)
        {
            unsigned long long temp_id = encode_pairID(u , v);
            int tem = id_input_edge[temp_id];
            if (tem != 0)
            {
                sum += input_edge_cache[tem].weight;
            }
        }
        total_w_assigned[v] = sum;
    }*/
    for (auto v : vertex_S)
    {
        total_w_assigned[v] = first_total_w_assigned[v];
        res_sum[v] = 0;
    }
    unordered_set<int>vertex_T;
    vertex_T.clear();
    priority_queue<Node>vertex_X;
    long long max_adject_weight = 0;
    for (auto it : vertex_S)
    {
        vertex_T.insert(it);
    }
    int k = 0;
    long long res = 0 , sum = 0;
    int temp_cnt = 0;
    while(vertex_T.size() != 0)
    {
        for (auto it : vertex_S)
        {
            flag_X[it] = 0;
        }
        temp_cnt ++;
        k ++;
        vector<int>independent_set;
        independent_set.clear();
        while(!vertex_X.empty()) vertex_X.pop();
        for (auto it : vertex_T)
        {
            Node temp;
            temp.id = it;
            temp.val = total_w_assigned[it];
            vertex_X.push(temp);
        }
        res = 0;
        while(!vertex_X.empty())
        {
            Node temp = vertex_X.top();
            vertex_X.pop();
            int v = temp.id;
            if (flag_X[v])
            {
                continue;
            }
            independent_set.push_back(v);
            vertex_sequence.push(v);

            vertex_upper_bound[v] = total_w_assigned[v] + res_sum[v];
            //cout<<"cnt = "<<cnt<<' '<<"temp_cnt = "<<temp_cnt<<' '<<"v = "<<v<<' '<<"upper = "<<vertex_upper_bound[v]<<endl;

            /*priority_queue<Node>temp_X;
            while(!temp_X.empty())  temp_X.pop();
            while(!vertex_X.empty())
            {
                temp = vertex_X.top();
                vertex_X.pop();
                unsigned long long temp_id = encode_pairID(temp.id , v);
                int tem = id_input_edge[temp_id];
                if (tem == 0)
                {
                    temp_X.push(temp);
                }
            }
            vertex_X = temp_X;*/
            flag_X[v] = 1;
            for (auto it : vertex_adject[v])
            {
                flag_X[it] = 1;
            }
            vertex_T.erase(v);
        }
        sum += res;
        for (auto v : vertex_T)
        {
            max_adject_weight = 0;
            for (int i = independent_set.size() - 1 ; i >= 0 ; i --)
            {
                long long res_k = get_val(independent_set[i] , v);
                if (res_k != 0)
                {
                    res_sum[v] += total_w_assigned[independent_set[i]];
                    break;
                }
            }
            for (int i = independent_set.size() - 1 ; i >= 0 ; i --)
            {
                long long res_k = get_val(independent_set[i] , v);
                max_adject_weight = max(max_adject_weight , res_k);
            }
            total_w_assigned[v] = total_w_assigned[v] + max_adject_weight;
        }
    }
    return ;
}

int vector_func[maxn];

vector<int> vector_differ(vector<int>A , vector<int>B)
{
    vector<int>ans;
    memset(vector_func , 0 , sizeof(vector_func));
    for (auto it : B)
    {
        vector_func[it] = 1;
    }
    for (auto it : A)
    {
        if (vector_func[it] != 1)
        {
            ans.push_back(it);
        }
    }
    return ans;
}

vector<int> vector_inter(vector<int>A , vector<int>B)
{
    vector<int>ans;
    memset(vector_func , 0 , sizeof(vector_func));
    for (auto it : B)
    {
        vector_func[it] = 1;
    }
    for (auto it : A)
    {
        if (vector_func[it] == 1)
        {
            ans.push_back(it);
        }
    }
    return ans;
}

void pri(vector<int> C , vector<int> T)
{
    cout<<"-----------------------------"<<endl;
    cout<<"cnt = "<<cnt<<endl;
    /*cout<<"T = "<<endl;
    for (auto it : T)
    {
        cout<<it<<' ';
    }
    cout<<endl;
    cout<<"C = "<<endl;
    for (auto it : C)
    {
        cout<<it<<' ';
    }
    cout<<endl;*/
    cout<<"-----------------------------"<<endl;
}

inline void Expand(vector<int> vertex_C , vector<int> vertex_S , long long C_cal)
{
    //pri(vertex_C , vertex_S);
    if (vertex_S.size() == 0)
    {
        if (C_cal > C_max)
        {
            fin_ans = vertex_C;
            C_max = C_cal;
            if (C_max == C_stop)
            {
                return ;
            }
        }
        return ;
    }
    cnt ++;
    CALC_SEQ_AND_UB(vertex_C , vertex_S);
    vector<int>vertex_vis;
    vertex_vis.clear();
    stack<int>copy_vertex_sequence;
    while(!copy_vertex_sequence.empty()) copy_vertex_sequence.pop();
    copy_vertex_sequence = vertex_sequence;

    unordered_map<int,int> copy_vertex_upper_bound;
    copy_vertex_upper_bound.clear();
    for (int i = 0 ; i < vertex_S.size() ; i ++)
    {
        copy_vertex_upper_bound[vertex_S[i]] = vertex_upper_bound[vertex_S[i]];
    }

    while(!copy_vertex_sequence.empty())
    {
        int p = copy_vertex_sequence.top();
        copy_vertex_sequence.pop();
        //cout<<"p = "<<p<<' '<<"up_p = "<<vertex_upper_bound[p]<<endl;
        if (C_cal + copy_vertex_upper_bound[p] > C_max)
        {
            vector<int>S_temp;
            S_temp.clear();
            vertex_C.push_back(p);
            vector<int>::iterator it = vertex_S.begin();
            while(it != vertex_S.end())
            {
                if (*it == p)
                {
                    it = vertex_S.erase(it);
                }
                else
                {
                    if (get_val(*it , p))
                    {
                        S_temp.push_back(*it);
                    }
                    it ++;
                }
            }
            long long add_sum = 0;
            for (auto it : vertex_C)
            {
                add_sum += get_val(it , p);
            }
            for (auto u : S_temp)
            {
                first_total_w_assigned[u] += get_val(u , p);
            }
            Expand(vertex_C , S_temp , C_cal + add_sum);
            vertex_C.pop_back();
            for (auto u : S_temp)
            {
                first_total_w_assigned[u] -= get_val(u , p);
            }
        }
    }
}

inline long long MECQ()
{
    memset(first_total_w_assigned , 0 , sizeof(first_total_w_assigned));
    C_max = C_initial;
    vector<int>init;
    init.clear();
    Expand(init , V , 0);
    return C_max;
}

int x , y;

void read()
{
    string line;
    string p, tmp;
    string e;
    istringstream is;
    int cnt = 0;
    do
    {
        cnt ++;
        getline(cin, line);
        is.clear();
        is.str(line);
        is >> p >> tmp >> n >> m;
    }
    while (p != "p");
    for (int i = 1 ; i <= n ; i ++)
    {
        V.push_back(i);
    }
    for (int i = 1 ; i <= m ; i ++)
    {
        getline(cin, line);
        is.clear();
        is.str(line);
        is >> e >> x >> y;
        long long c = (x + y) % 200 + 1;
        //long long c = 1;
        add(x , y , c);
    }
    return ;
}

int main()
{
    fin_ans.clear();
    freopen(open_file_name , "r" , stdin);
    ios::sync_with_stdio(false);
    cout<<"The file_name is: "<<open_file_name<<endl;
    double time=0;
    double counts=0;
    clock_t startTime,endTime;
    startTime = clock();
    init();
    read();
    MECQ();
    endTime = clock();
    time = (double)(endTime - startTime) / CLOCKS_PER_SEC;
    cout<<C_max<<endl;
    cout<<"Time is: "<<time<<"s"<<endl;


    /*cout<<"fin_ans_num = "<<fin_ans.size()<<endl;
    sort(fin_ans.begin() , fin_ans.end());
    for (int i = 0 ; i < fin_ans.size() ; i ++)
    {
        cout<<fin_ans[i]<<endl;
    }*/
    system("pause");
}

