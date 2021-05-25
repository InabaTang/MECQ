#include<bits/stdc++.h>
#include<windows.h>
using namespace std;

const int maxn = 1e5 + 50;
long long C_initial = 0;
long long C_max = 0;
int cnt_edge = 0;
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
set<int>vertex_adject[maxn];

inline void init()
{
    V.clear();
    input_edge_cache.clear();
    input_edge ep;
    ep.v1 = -1; ep.v2 = -1; ep.weight = -1;
    input_edge_cache.push_back(ep);
}

inline void add(int x , int y , int c)
{
    cnt_edge ++;
    input_edge temp;
    temp.v1 = x;
    temp.v2 = y;
    temp.weight = c;
    input_edge_cache.push_back(temp);
    vertex_degree[x].push_back(cnt_edge);
    vertex_degree[y].push_back(cnt_edge);
    vertex_adject[x].insert(y);
    vertex_adject[y].insert(x);
}

inline int get_adject(int v , int i)
{
    int id = vertex_degree[v][i];
    int v1 = input_edge_cache[id].v1 , v2 = input_edge_cache[id].v2;
    int adject_id = (v1 == v ? v2 : v1);
    return adject_id;
}

/*void pri(set<int>W)
{
    for (auto it : W)
    {
        cout<<it<<' ';
    }
    cout<<endl;
}*/

inline long long get_weight(int v , int i)
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
inline void CALC_SEQ_AND_UB(vector<int>C , vector<int>S)
{
    while(!vertex_sequence.empty()) vertex_sequence.pop();
    for (auto v : S)
    {
        long long sum = 0;
        for (int i = 0 ; i < vertex_degree[v].size() ; i ++)
        {
            int adject_id = get_adject(v , i);
            if (find(C.begin(), C.end(), adject_id) != C.end())
            {
                sum += get_weight(v , i);
            }
        }
        total_w_assigned[v] = sum;
    }
    vector<int>T;
    T.clear();
    priority_queue<Node>X;
    long long max_adject_weight = 0;
    T = S;
    int k = 0;
    long long res = 0 , sum = 0;
    while(T.size() != 0)
    {
        max_adject_weight = 0;
        k ++;
        while(!X.empty()) X.pop();
        for (auto it : T)
        {
            Node temp;
            temp.id = it;
            temp.val = total_w_assigned[it];
            X.push(temp);
        }
        res = 0;
        while(!X.empty())
        {
            Node temp = X.top();
            X.pop();
            int v = temp.id;
            vertex_sequence.push(v);
            vertex_upper_bound[v] = total_w_assigned[v] + sum;
            res = max(res , total_w_assigned[v]);
            for (int i = 0 ; i < vertex_degree[v].size() ; i ++)
            {
                max_adject_weight = max(max_adject_weight , get_weight(v , i));
            }
            priority_queue<Node>temp_X;
            while(!X.empty())
            {
                temp = X.top();
                X.pop();
                if (!vertex_adject[v].count(temp.id))
                {
                    temp_X.push(temp);
                }
            }
            X = temp_X;
            while(!temp_X.empty()) temp_X.pop();
            for (vector<int>::iterator it = T.begin(); it !=T.end() ; it++)
            {
                if (*it == v)
                {
                    T.erase(it);
                    break;
                }
            }
        }
        sum += res;
        for (auto v : T)
        {
            total_w_assigned[v] = total_w_assigned[v] + max_adject_weight;
        }
        /*Sleep(3000);
        cout<<endl;*/
    }
    return ;
}

inline void Expand(vector<int>C , vector<int>S , long long C_cal)
{
    if (S.size() == 0)
    {
        if (C_cal > C_max)
        {
            /*cout<<"C_cal = "<<C_cal<<endl;
            cout<<"C = "; pri(C);*/
            C_max = C_cal;
        }
        return ;
    }
    CALC_SEQ_AND_UB(C , S);
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
            vector<int>C_temp; C_temp.clear(); C_temp = C;
            C_temp.push_back(p);
            vector<int>S_temp; S_temp.clear();
            vector<int>S_inter_temp; S_inter_temp.clear();
            set_difference(S.begin() , S.end() , vertex_vis.begin() , vertex_vis.end() ,
                           inserter(S_temp,S_temp.end()));
            set_intersection(S_temp.begin(),S_temp.end(),vertex_adject[p].begin(),
                             vertex_adject[p].end(),inserter(S_inter_temp,S_inter_temp.end()));
            long long add_sum = 0;
            for (int i = 0 ; i < vertex_adject[p].size() ; i ++)
            {
                int adject_id = get_adject(p , i);
                if (find(C.begin(), C.end(), adject_id) != C.end())
                {
                    add_sum += get_weight(p , i);
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
        //cout<<"x = "<<x<<' '<<"y = "<<y<<' '<<"c = "<<c<<endl;
    }
    return ;
}

int main()
{
    freopen("p_hat300-1.clq" , "r" , stdin);
    cout<<"样例名称: "<<"p_hat300-1.clq"<<endl;
    ios::sync_with_stdio(false);
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
}
