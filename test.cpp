#pragma GCC optimize(2)
#include<bits/stdc++.h>
//test/test_100000_5.clq
//test/test_20000_4.clq
#include <dirent.h>
using namespace std;

int cnt = 0;
clock_t startTime,endTime;

const int maxn = 2e6 + 50;
int n , m;
vector<int>fin_ans;
long long C_initial = 0;
long long C_max = 0;
int cnt_edge = 0;
vector<int>V;
long long total_w_assigned[maxn];
long long first_total_w_assigned[maxn];
int reduciton_id[maxn];
long long sum_weight[maxn];
int degree_num[maxn];
long long reduction_UB[maxn];
unordered_map<unsigned long long , int>upper_contribute;
stack<int>wait_to_delete;
int wait_to_delete_id[maxn];
int reduction_emax_id[maxn];
int reduction_emin_id[maxn];
set<int>set_fir;

struct adject_Point
{
    int id;
    int val;
    adject_Point(int x = 0 , int y = 0)
    {
        id = x; val = y;
    }
};

struct input_edge
{
    int v1;
    int v2;
    long long weight;
};
vector<input_edge> input_edge_cache; //from 1~n
vector<adject_Point> vertex_adject[maxn];
long long vertex_upper_bound[maxn];
long long max_upper_num = 0;
unordered_map<unsigned long long , int>id_input_edge;

struct UB_Node
{
    int id;
    long long val;
    bool operator < (const UB_Node &a) const
    {
        return val > a.val;
    }
};
priority_queue<UB_Node>reduction_ub;
priority_queue<UB_Node>copy_reduction_ub;

inline void init()
{
    set_fir.clear();
    while(!reduction_ub.empty())
    {
        reduction_ub.pop();
    }
    upper_contribute.clear();
    max_upper_num = 0;
    C_initial = 0;
    C_max = 0;
    cnt_edge = 0;
    V.clear();
    id_input_edge.clear();
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
    degree_num[x] ++;
    degree_num[y] ++;
    temp.weight = c;
    unsigned long long temp_id = encode_pairID(x , y);
    id_input_edge[temp_id] = cnt_edge;
    input_edge_cache.push_back(temp);
    /*vertex_degree[x].push_back(cnt_edge);
    vertex_degree[y].push_back(cnt_edge);*/
    vertex_adject[x].push_back(adject_Point(y , c));
    vertex_adject[y].push_back(adject_Point(x , c));
    sum_weight[x] += c;
    sum_weight[y] += c;
    reduction_emax_id[x] ++;
    reduction_emax_id[y] ++;
}

void cal_UB()
{
    UB_Node temp;
    for (int v = 1 ; v <= n ; v ++)
    {
        temp.id = v;
        if (degree_num[v] == 0)
        {
            temp.val = 0;
        }
        else if (degree_num[v] == 1)
        {
            temp.val = sum_weight[v];
        }
        else
        {
            long long res = 0;
            for (auto u_adpoint : vertex_adject[v])
            {
                int u = u_adpoint.id;
                if (degree_num[v] >= degree_num[u])
                {
                    res += sum_weight[u];
                }
                else
                {
                    long long EMAX = degree_num[v] * vertex_adject[u][reduction_emax_id[u]].val;
                    long long EMIN = sum_weight[u] - (degree_num[u] - degree_num[v]) * vertex_adject[u][reduction_emin_id[u]].val;
                    res += min(EMAX , EMIN);
                }
            }
            temp.val = (sum_weight[v] + res) / 2;
        }
        reduction_UB[v] = temp.val;
        //cout<<"i = "<<v<<' '<<"val = "<<temp.val<<endl;
        reduction_ub.push(temp);
    }
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
    cout<<"n = "<<n<<' '<<"m = "<<m<<endl;
    for (int i = 1 ; i <= n ; i ++)
    {
        V.push_back(i);
        vertex_adject[i].clear();
        total_w_assigned[i] = 0;
        first_total_w_assigned[i] = 0;
        vertex_upper_bound[i] = 0;
        degree_num[i] = 0;
        reduciton_id[i] = 0;
        sum_weight[i] = 0;
        reduction_emin_id[i] = 0;
        reduction_emax_id[i] = 0;
        reduction_UB[i] = 0;
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
        if (reduciton_id[v] == 1) continue;
        total_w_assigned[v] = first_total_w_assigned[v];
        res_sum[v] = 0;
    }
    unordered_set<int>vertex_T;
    vertex_T.clear();
    priority_queue<Node>vertex_X;
    long long max_adject_weight = 0;
    for (auto it : vertex_S)
    {
        if (reduciton_id[it] == 1) continue;
        vertex_T.insert(it);
    }
    int k = 0;
    long long res = 0 , sum = 0;
    int temp_cnt = 0;
    while(vertex_T.size() != 0)
    {
        //cout<<"t_size = "<<vertex_T.size()<<endl;
        for (auto it : vertex_S)
        {
            if (reduciton_id[it] == 1) continue;
            flag_X[it] = 0;
        }
        temp_cnt ++;
        k ++;
        vector<int>independent_set;
        independent_set.clear();
        while(!vertex_X.empty()) vertex_X.pop();
        for (auto it : vertex_T)
        {
            if (reduciton_id[it] == 1) continue;
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
            /*cout<<"t.size = "<<vertex_T.size()<<endl;
            cout<<"x.size = "<<vertex_X.size()<<endl;*/
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
            for (auto it_point: vertex_adject[v])
            {
                int it = it_point.id;
                flag_X[it] = 1;
            }
            vertex_T.erase(v);
        }
        //cout<<"independent_set.size() = "<<independent_set.size()<<endl;
        sum += res;
        for (auto v : vertex_T)
        {
            if (reduciton_id[v] == 1) continue;
            max_adject_weight = 0;
            if (independent_set.size() <= vertex_adject[v].size())
            {
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
            }
            else
            {
                long long res_k;
                long long res_s = 0;
                for (auto it_point : vertex_adject[v])
                {
                    int it = it_point.id;
                    if (flag_X[it])
                    {
                        res_k = get_val(it , v);
                        if (res_k != 0)
                        {
                            res_s = max(res_s , total_w_assigned[it]);
                        }
                        max_adject_weight = max(max_adject_weight , res_k);
                    }
                }
                res_sum[v] += res_s;
            }
            total_w_assigned[v] = total_w_assigned[v] + max_adject_weight;
        }
        //cout<<"iden"<<endl;
    }
    return ;
}


void pri(vector<int> C , vector<int> T)
{
    cout<<"-----------------------------"<<endl;
    cout<<"cnt = "<<cnt<<endl;
    cout<<"T = "<<endl;
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
    cout<<endl;
    cout<<"-----------------------------"<<endl;
}

void update_Emin_Emax(int id)
{
    adject_Point temp;
    for (int i = reduction_emin_id[id] ; i <= reduction_emax_id[id] ; i ++)
    {
        temp = vertex_adject[id][i];
        if (reduciton_id[temp.id] == 0)
        {
            reduction_emin_id[id] = i;
            break;
        }
    }
    for (int i = reduction_emax_id[id] ; i >= reduction_emin_id[id] ; i --)
    {
        temp = vertex_adject[id][i];
        if (reduciton_id[temp.id] == 0)
        {
            reduction_emax_id[id] = i;
            break;
        }
    }
}

void update_UB(int id)
{
    UB_Node temp;
    for (auto v_point : vertex_adject[id])
    {
        int v = v_point.id;
        degree_num[v] --;
        sum_weight[v] -= get_val(v , id);
        id_input_edge.erase(encode_pairID(v , id));
        if (reduciton_id[v]) continue;
        update_Emin_Emax(v);
        temp.id = id;
        if (degree_num[v] == 0)
        {
            temp.val = 0;
        }
        else if (degree_num[v] == 1)
        {
            temp.val = sum_weight[v];
        }
        else
        {
            long long res = 0;
            for (auto u_point : vertex_adject[v])
            {
                int u = u_point.id;
                if (reduciton_id[v] == 1) continue;
                if (degree_num[v] >= degree_num[u])
                {
                    res += sum_weight[u];
                }
                else
                {
                    long long EMAX = degree_num[v] * vertex_adject[u][reduction_emax_id[u]].val;
                    long long EMIN = sum_weight[u] - (degree_num[u] - degree_num[v]) * vertex_adject[u][reduction_emin_id[u]].val;
                    res += min(EMAX , EMIN);
                }
            }
            temp.val = (sum_weight[v] + res) / 2;
            reduction_UB[v] = temp.val;
            if (temp.val < C_max)
            {
                wait_to_delete.push(temp.id);
            }
            else
            {
                if (reduciton_id[temp.id] != 1) reduction_ub.push(temp);
            }
        }
    }
}


void do_reduction_delete()
{
    //cout<<"do_reduction_delete"<<endl;
    while(!wait_to_delete.empty())
    {
        int id = wait_to_delete.top();
        wait_to_delete.pop();
        if (wait_to_delete_id[id] == 1) continue;
        //cout<<"wait to update , id = "<<id<<endl;
        update_UB(id);
        //cout<<"update_end"<<endl;
        wait_to_delete_id[id] = 1;
        for (auto u_point : vertex_adject[id])
        {
            int u = u_point.id;
            if (wait_to_delete_id[u] == 1) continue;
            if (reduction_UB[u] < C_max)
            {
                wait_to_delete.push(u);
            }
        }
    }
    //cout<<"delete complete"<<endl;
}

int temp_reduction_flag = 0;
void do_reduction()
{
    //cout<<"do_reduciton"<<endl;
    while(!reduction_ub.empty())
    {
        UB_Node temp = reduction_ub.top();
        if (temp.val >= C_max)
        {
            break;
        }
        else
        {
            //cout<<"temp.id = "<<temp.id<<endl;
            reduciton_id[temp.id] = 1;
            reduction_ub.pop();
            wait_to_delete.push(temp.id);
        }
    }
    do_reduction_delete();
    if (temp_reduction_flag == 0)
    {
        endTime = clock();
        double time = (double)(endTime - startTime) / CLOCKS_PER_SEC;
        cout<<"fir_delete_time = "<<time<<endl;
        temp_reduction_flag = 1;
        int dcnt = 0;
        for (int i = 1 ; i <= n ; i ++)
        {
            if (reduciton_id[i])
            {
                dcnt ++;
            }
        }
        cout<<"fir_delete_num = "<<dcnt<<endl;
    }
}

inline void Expand(vector<int> vertex_C , vector<int> vertex_S , long long C_cal)
{
    //if (C_max == 16912230) pri(vertex_C , vertex_S);
    endTime = clock();
    double time = (double)(endTime - startTime) / CLOCKS_PER_SEC;
    if(time > 3600)
    {
        cout<<"Time out! Now max_clique = "<<C_max<<endl;
        exit(0);
    }
    if (vertex_S.size() == 0)
    {
        if (C_cal > C_max)
        {
            fin_ans = vertex_C;
            C_max = C_cal;
            cout<<"update_new_answer"<<endl;
            cout<<"C_max = "<<C_max<<endl;
            endTime = clock();
            double time = (double)(endTime - startTime) / CLOCKS_PER_SEC;
            cout<<"Now time is = "<<time<<"s"<<endl;
            do_reduction();
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
        if (reduciton_id[p] == 1) continue;
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
                        if (reduciton_id[*it] == 0) S_temp.push_back(*it);
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
        else
        {
            copy_vertex_upper_bound.clear();
            while(!copy_vertex_sequence.empty())
            {
                p = copy_vertex_sequence.top();
                copy_vertex_sequence.pop();
            }
            vector<int>cle;
            vertex_C.swap(cle);
            vertex_S.swap(cle);
        }
    }
}

string input_filename;

inline long long MECQ()
{
    memset(first_total_w_assigned , 0 , sizeof(first_total_w_assigned));
    C_max = C_initial;
    vector<int>init;
    init.clear();
    Expand(init , V , 0);
    return C_max;
}

vector<int>first_point;
int fir_vis[maxn];

bool fir_judge(int id)
{
    for (int i = 0 ; i < first_point.size() - 1; i ++)
    {
        unsigned long long temp_id = encode_pairID(first_point[i] , id);
        if (!id_input_edge.count(temp_id))
        {
            return 0;
        }
    }
    return 1;
}

long long dfs()
{
    /*cout<<"now_vector = ";
    for (int i = 0 ; i < first_point.size() - 1; i ++)
    {
        cout<<first_point[i]<<' ';
    }
    cout<<endl;*/
    long long res = 0;
    if (set_fir.size() == 0) return 0;
    int de_maxx = 0 , maxx_id = -1;
    for (auto it : set_fir)
    {
        if (degree_num[it] > de_maxx)
        {
            maxx_id = it;
            de_maxx = degree_num[it];
        }
    }
    for (auto it : set_fir)
    {
        unsigned long long temp = encode_pairID(it , maxx_id);
        if (id_input_edge.count(temp))
        {
            set_fir.erase(it);
        }
    }
    for (auto it : first_point)
    {
        res += get_val(it , maxx_id);
    }
    first_point.push_back(maxx_id);
    res += dfs();
    return res;
}

void cal_fir()
{
    first_point.clear();
    long long res = 0;
    int degree_max = 0 , id = 0;
    for (int i = 1 ; i <= n ; i ++)
    {
        if (degree_num[i] > degree_max)
        {
            degree_max = degree_num[i];
            id = i;
        }
    }
    first_point.push_back(id);
    fir_vis[id] = 1;
    for (auto it : vertex_adject[id])
    {
        int wait_id = it.id;
        set_fir.insert(wait_id);
    }
    first_point.push_back(id);
    res = dfs();
    C_max = res;
    endTime = clock();
    double time = (double)(endTime - startTime) / CLOCKS_PER_SEC;
    cout<<"fir_C = "<<C_max<<endl;
    cout<<"fir_time = "<<time<<endl;
    do_reduction();
}

char file_name[maxn];

bool cmp_vertex_adject(adject_Point a , adject_Point b)
{
    return a.val < b.val;
}

void solve()
{
    //strcpy(file_name , "test/test_10000_5.clq");
    fin_ans.clear();
    freopen(file_name , "r" , stdin);
    ios::sync_with_stdio(false);
    cout<<"The file_name is: "<<file_name<<endl;
    double time=0;
    double counts=0;
    startTime = clock();
    init();
    read();
    for (int i = 1 ; i <= n ; i ++)
    {
        reduction_emax_id[i] --;
        sort(vertex_adject[i].begin() , vertex_adject[i].end() , cmp_vertex_adject);
    }
    //cout<<"start cal_UB"<<endl;
    cal_UB();
    //cout<<"end"<<endl;
    //cout<<"start cal_fir"<<endl;
    cal_fir();
    //cout<<"end"<<endl;
    MECQ();
    int dcnt = 0;
    for (int i = 1 ; i <= n ; i ++)
    {
        if (reduciton_id[i])
        {
            dcnt ++;
        }
    }
    cout<<"delete_num = "<<dcnt<<endl;
    endTime = clock();
    time = (double)(endTime - startTime) / CLOCKS_PER_SEC;
    cout<<"--------------------------------------"<<endl;
    cout<<C_max<<endl;
    cout<<"Time is: "<<time<<"s"<<endl;
    cout<<"--------------------------------------"<<endl;


    /*cout<<"fin_ans_num = "<<fin_ans.size()<<endl;
    sort(fin_ans.begin() , fin_ans.end());
    for (int i = 0 ; i < fin_ans.size() ; i ++)
    {
        cout<<fin_ans[i]<<endl;
    }*/
    cout<<endl;
}

int main()
{

    DIR *directory_pointer;
    struct dirent *entry;
    directory_pointer = opendir("can_test");
    if(directory_pointer == NULL)
    {
        printf("Error open\n");
        return 0;
    }
    else
    {
        while((entry = readdir(directory_pointer))!=NULL)
        {
            if(entry -> d_name[0] == '.') continue;
            string file_str_name = entry -> d_name;
            strcpy(file_name,"can_test/");
            const char *p = file_str_name.data();
            strcat(file_name,p);
            solve();
        }
    }
    return 0;
}
