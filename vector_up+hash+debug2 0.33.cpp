#include<bits/stdc++.h>
#include<windows.h>
using namespace std;

constexpr int maxn = 1e5 + 50;
long long C_initial = 0;
long long C_max = 0;
int cnt_edge = 0;
int n , m;
vector<int>V;
long long total_w_assigned[maxn];

struct input_edge
{
    int v1;
    int v2;
    long long weight;
};
vector<input_edge> input_edge_cache; //from 1~n
vector<int> vertex_degree[maxn]; //from 0~v[i].size()
unordered_set<int>unorder_vertex_adject[maxn];
set<int>vertex_adject[maxn];
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
    id_input_edge[temp_id] = cnt_edge;
    input_edge_cache.push_back(temp);
    vertex_degree[x].push_back(cnt_edge);
    vertex_degree[y].push_back(cnt_edge);
    vertex_adject[x].insert(y);
    vertex_adject[y].insert(x);
    unorder_vertex_adject[x].insert(y);
    unorder_vertex_adject[y].insert(x);
}

/*void pri(set<int>W)
{
    for (auto it : W)
    {
        cout<<it<<' ';
    }
    cout<<endl;
}*/

inline int get_adject(int v , int i)
{
    int id = vertex_degree[v][i];
    int v1 = input_edge_cache[id].v1 , v2 = input_edge_cache[id].v2;
    int adject_id = (v1 == v ? v2 : v1);
    return adject_id;
}

inline long long get_weight(int &v , int &i)
{
    int id = vertex_degree[v][i];
    return input_edge_cache[id].weight;
}

long long vertex_upper_bound[maxn];
stack<int>vertex_sequence;
struct Node
{
    int id;
    int val;
    bool operator < (const Node &a) const
    {
        if (val == a.val) return id > a.id;
        return val > a.val;
    }
};

long long max_adject_weight[maxn];

inline void CALC_SEQ_AND_UB(vector<int>&vertex_C , vector<int>&vertex_S)
{
    while(!vertex_sequence.empty()) vertex_sequence.pop();
    for (auto v : vertex_S)
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
    }
    set<int>vertex_T;
    vertex_T.clear();
    priority_queue<Node>vertex_X;
    for (auto it : vertex_S)
    {
        vertex_T.insert(it);
    }
    int k = 0;
    long long res = 0 , sum = 0;
    while(vertex_T.size() != 0)
    {
        k ++;
        while(!vertex_X.empty()) vertex_X.pop();
        for (auto it : vertex_T)
        {
            Node temp;
            temp.id = it;
            temp.val = total_w_assigned[it];
            vertex_X.push(temp);
        }
        res = 0;
        for (int i = 1 ; i <= n ; i ++)
        {
            max_adject_weight[i] = 0;
        }
        while(!vertex_X.empty())
        {
            Node temp = vertex_X.top();
            vertex_X.pop();
            int v = temp.id;
            vertex_sequence.push(v);
            vertex_upper_bound[v] = total_w_assigned[v] + sum;
            res = max(res , total_w_assigned[v]);
            for (auto u : vertex_T)
            {
                int adject_id = u;
                unsigned long long temp_id = encode_pairID(adject_id , v);
                int tem = id_input_edge[temp_id];
                if (tem != 0)
                {
                    max_adject_weight[adject_id] =
                    max(max_adject_weight[adject_id] , input_edge_cache[tem].weight);
                }
            }
            priority_queue<Node>temp_X;
            while(!temp_X.empty())  temp_X.pop();
            while(!vertex_X.empty())
            {
                temp = vertex_X.top();
                vertex_X.pop();
                if (unorder_vertex_adject[v].find(temp.id) == unorder_vertex_adject[v].end())
                {
                    temp_X.push(temp);
                }
            }
            vertex_X = temp_X;
            vertex_T.erase(v);
        }
        sum += res;
        for (auto v : vertex_T)
        {
            total_w_assigned[v] = total_w_assigned[v] + max_adject_weight[v];
        }
    }
    return ;
}

inline void Expand(vector<int>&vertex_C , vector<int>&vertex_S , long long C_cal)
{
    if (vertex_S.size() == 0)
    {
        if (C_cal > C_max)
        {
            C_max = C_cal;
        }
        return ;
    }
    CALC_SEQ_AND_UB(vertex_C , vertex_S);
    vector<int>vertex_vis;
    vertex_vis.clear();
    stack<int>copy_vertex_sequence;
    while(!copy_vertex_sequence.empty()) copy_vertex_sequence.pop();
    copy_vertex_sequence = vertex_sequence;
    while(!copy_vertex_sequence.empty())
    {
        int p = copy_vertex_sequence.top();
        copy_vertex_sequence.pop();
        //if (find(S.begin(), S.end(), p) != S.end()) assert(0);
        if (C_cal + vertex_upper_bound[p] > C_max)
        {
            vector<int>C_temp; C_temp.clear(); C_temp = vertex_C;
            C_temp.push_back(p);
            vector<int>S_temp; S_temp.clear();
            vector<int>S_inter_temp; S_inter_temp.clear();
            set_difference(vertex_S.begin() , vertex_S.end() , vertex_vis.begin() , vertex_vis.end() ,
                           inserter(S_temp,S_temp.end()));
            set_intersection(S_temp.begin(),S_temp.end(),vertex_adject[p].begin(),
                             vertex_adject[p].end(),inserter(S_inter_temp,S_inter_temp.end()));
            long long add_sum = 0;
            for (auto it : vertex_C)
            {
                unsigned long long temp_id = encode_pairID(it , p);
                int tem = id_input_edge[temp_id];
                if (tem != 0)
                {
                    add_sum += input_edge_cache[tem].weight;
                }
            }
            Expand(C_temp , S_inter_temp , C_cal + add_sum);
            vertex_vis.push_back(p);
        }
    }
}

inline long long MECQ()
{
    C_max = C_initial;
    vector<int>init;
    init.clear();
    Expand(init , V , C_max);
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
        add(x , y , c);
    }
    return ;
}

int main()
{
    freopen("p_hat300-1.clq" , "r" , stdin);
    ios::sync_with_stdio(false);
    cout<<"样例名称: "<<"p_hat300-1.clq"<<endl;
    double time=0;
    double counts=0;
    LARGE_INTEGER nFreq;
    LARGE_INTEGER nBeginTime;
    LARGE_INTEGER nEndTime;
    QueryPerformanceFrequency(&nFreq);
    QueryPerformanceCounter(&nBeginTime);
    init();
    read();
    MECQ();
    QueryPerformanceCounter(&nEndTime);
    time=(double)(nEndTime.QuadPart-nBeginTime.QuadPart)/(double)nFreq.QuadPart;
    cout<<C_max<<endl;
    cout<<"程序执行时间："<<time<<"s"<<endl;
    system("pause");
}

