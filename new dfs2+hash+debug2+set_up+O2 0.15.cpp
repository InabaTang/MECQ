#pragma GCC optimize(2)
#include<bits/stdc++.h>
#include<windows.h>
using namespace std;

constexpr int maxn = 1e5 + 50;
long long C_initial = 0;
long long C_max = 0;
int cnt_edge = 0;
vector<int>V;
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
vector<int> vertex_degree[maxn]; //from 0~v[i].size()
unordered_set<int>unorder_vertex_adject[maxn];
vector<int>vertex_adject[maxn];
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
    vertex_adject[x].push_back(y);
    vertex_adject[y].push_back(x);
    max_upper[x] += c;
    max_upper[y] += c;
    unorder_vertex_adject[x].insert(y);
    unorder_vertex_adject[y].insert(x);
}

void pri(vector<int>W)
{
    for (auto it : W)
    {
        cout<<it<<' ';
    }
    cout<<endl;
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

long long max_adject_weight = 0;

inline void CALC_SEQ_AND_UB(vector<int>&vertex_C , vector<int>&vertex_S)
{
    while(!vertex_sequence.empty()) vertex_sequence.pop();
    /*for (auto it : vertex_S)
    {
        total_w_assigned[it] = first_total_w_assigned[it];
    }*/
    unordered_set<int>vertex_T;
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
        if (k == 1)
        {
            for (auto it : vertex_T)
            {
                Node temp;
                temp.id = it;
                temp.val = first_total_w_assigned[it];
                vertex_X.push(temp);
            }
        }
        else
        {
            for (auto it : vertex_T)
            {
                Node temp;
                temp.id = it;
                temp.val = total_w_assigned[it];
                vertex_X.push(temp);
            }
        }
        res = 0;
        vector<int>independent_sets;
        independent_sets.clear();
        while(!vertex_X.empty())
        {
            Node temp = vertex_X.top();
            vertex_X.pop();
            int v = temp.id;
            vertex_sequence.push(v);

            if (k == 1) vertex_upper_bound[v] = min(max_upper[v] , first_total_w_assigned[v] + sum);
            else vertex_upper_bound[v] = min(max_upper[v] , total_w_assigned[v] + sum);
            if (k == 1) res = max(res , first_total_w_assigned[v]);
            else res = max(res , total_w_assigned[v]);

            for (int i = 0 ; i < vertex_degree[v].size() ; i ++)
            {
                max_adject_weight = max(max_adject_weight , get_weight(v , i));
            }

            independent_sets.push_back(v);
            priority_queue<Node>temp_X;
            while(!temp_X.empty())  temp_X.pop();
            while(!vertex_X.empty()) // wait
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
            vertex_X = temp_X;
            vertex_T.erase(v);
        }
        sum += res;
        for (auto v : vertex_T)
        {
            if (k == 1) total_w_assigned[v] = first_total_w_assigned[v] + max_adject_weight;
            else total_w_assigned[v] = total_w_assigned[v] + max_adject_weight;
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
    /*cout<<"C = "; pri(vertex_C);
    cout<<"S = "; pri(vertex_S);
    cout<<endl;*/
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
            //cout<<"C_max = "<<C_max<<' '<<"zuo = "<<C_cal + vertex_upper_bound[p]<<endl;
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
            //wait
            for (auto v : S_inter_temp)
            {
                unsigned long long temp_id = encode_pairID(p , v);
                int tem = id_input_edge[temp_id];
                if (tem != 0)
                {
                   first_total_w_assigned[v] += input_edge_cache[tem].weight;
                }
            }
            Expand(C_temp , S_inter_temp , C_cal + add_sum);
            for (auto v : S_inter_temp)
            {
                unsigned long long temp_id = encode_pairID(p , v);
                int tem = id_input_edge[temp_id];
                if (tem != 0)
                {
                    first_total_w_assigned[v] -= input_edge_cache[tem].weight;
                }
            }
            vertex_vis.push_back(p);
        }
    }
}

inline long long MECQ()
{
    C_max = C_initial;
    vector<int>init;
    init.clear();
    Expand(init , V , 0);
    return C_max;
}

int n , m;
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

