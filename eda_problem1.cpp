#include<iostream>
#include<cstring>
#include<queue> 
#include<stack>
#include<vector>
#include<unordered_map>
#include<fstream>
#include<fstream>
#include<sstream>
#include<algorithm>
#include<cmath>
#include<chrono>
#include<unordered_set>
using namespace std;


int gate_num=0;

//定义门的类型；
enum TYPE
{
    inp,
    not11,
    or2,
    nand2,
    and2
};
enum value
{
    zero,
    one,
    x,
    x_prime
};



//存储端口名称，包括结点名和端口名
struct port_name
{
    int node;
    int port;
    int in;//是否为scc的输入结点
};


//超边
struct h_edge
{
    string name;
    vector<int> fro;
    vector<int> to;
    vector<port_name> to_ports;
    vector<int> cir;
};

//超边集合和边集合
unordered_map<string,h_edge> HE;



struct Node
{
    int id;
    string name;
    TYPE gate_type;
    vector<h_edge> e;
    int scc;//所属scc序号
    vector<port_name> to_ports;
    vector<port_name> in_ports;
    vector<int> cir;//所属的环
    //johnson之中添加
    vector<int> input;
    //以下变量在judge2之中加入
    int indeg;
    value gate_value;
};

vector<Node> G;


struct e_in_scc
{
    h_edge edge;
    port_name port;
};

//便于输出的scc格式
struct scc_node
{
    vector<int> node;
    vector<e_in_scc> e;
    vector<port_name> inputs;
    vector<string> gates;
    vector<string> signals;
};

vector<scc_node> G_scc;


vector<vector<Node>> scc_vec;


//tarjan求解scc
vector<bool> vis;
vector<int> dfn;
vector<int> low;
stack<int> st;
int tot=0;

//存储结点在scc_vec之中的位置
struct scc_place
{
    int first;
    int second;
};
unordered_map<int,scc_place> scc_place_map;
int first_temp=0;

void tarjan(int u)
{
    low[u]=tot;
    dfn[u]=low[u];
    tot++;
    st.push(u);
    vis[u]=1;


    //dfs
    for(int i=0;i<G[u].e[0].to.size();i++)
    {
        int v=G[u].e[0].to[i];
        //dfn[v]=-1，该节点为新节点
        if(dfn[v]==-1)
        {
            tarjan(v);//往下搜索
            low[u]=min(low[u],low[v]);//更新该节点的low[u];
        }

        else if(vis[v])//v在栈内，说明v是u的祖先
        //dfn[v]!=-1,v被搜索过
        //vis[v]=1,v是这次搜索到的
        {
            low[u]=min(low[u],dfn[v]);
        }
    }

    if(low[u]==dfn[u])//说明u是scc的祖先结点
    {

        int second_temp=0;
        
        int k=-1;
        scc_node scc_node1;
        vector<Node> scc1;
        
        do
        {
            k=st.top();
            vis[k]=0;
            st.pop();
            scc_node1.node.push_back(k);

            Node node1;
            node1.name=G[k].name;
            node1.gate_type=G[k].gate_type;
            node1.e=G[k].e;
            node1.id=k;
            scc1.push_back(node1);
            
            scc_place_map[k].first=first_temp;
            scc_place_map[k].second=second_temp;
            second_temp++;
            
        }while(u!=k); 

        G_scc.push_back(scc_node1);
        scc_vec.push_back(scc1);
        first_temp++;
        
        
        
        //更新结点的SCC信息；
        for(int i=0;i<scc_node1.node.size();i++)
        {
            int v=scc_node1.node[i];
            G[v].scc=G_scc.size()-1;
        }
        
    }
}



//记录可以振荡的环的连线

vector<vector<string>> cir_can_signals;
//johnson找到所有的环
int cir_count;



void johnson(int u1,vector<Node> &scc1)
{
    vis[u1]=1;
    st.push(u1);

    
    for(int i=0;i<scc1[scc_place_map[u1].second].to_ports.size();i++)
    {

        port_name &port1=scc1[scc_place_map[u1].second].to_ports[i];
        scc1[scc_place_map[u1].second].to_ports[i].in=0;
        port1.in=0;
        scc1[scc_place_map[port1.node].second].in_ports.push_back(port1);
   

        //为方便后面的judge2，记录输入结点
        scc1[scc_place_map[port1.node].second].input.push_back(u1);
        
        int v1=scc1[scc_place_map[u1].second].to_ports[i].node;

        if(vis[v1]==0)
        {
            johnson(v1,scc1);
        }
        
        else
        {
            int k1=v1;
            stack<int> st_temp;
            while(true)
            {
                scc1[scc_place_map[k1].second].cir.push_back(cir_count);
                k1=st.top();
                if(k1==v1) break;
                st.pop();
                st_temp.push(k1);
            }
            cir_count++;
            while(!st_temp.empty())
            {
                int v2=st_temp.top();
                st_temp.pop();
                st.push(v2);
            }
        }


        //两个输入端口相同的情况
        if(i==0&&scc1[scc_place_map[u1].second].to_ports.size()==2)
        {
            if(scc1[scc_place_map[u1].second].to_ports[0].node==scc1[scc_place_map[u1].second].to_ports[1].node)
            {
            scc1[scc_place_map[u1].second].to_ports[1].in=0;
            port_name port2=scc1[scc_place_map[u1].second].to_ports[1];
            scc1[scc_place_map[port1.node].second].in_ports.push_back(port2);   
            break;
            }
        }
        
    }
    
    st.pop();
    vis[u1]=0;
    return;
}

//判断单环起振
bool oscilation_judge0(vector<Node> &scc1,vector<string> &signals)
{
    int not_num=0;//统计负反馈数目
    bool locked=0;//如果某个输入锁定locked=1，然而在一个环的情况下，锁定的情况不存在；
    for(int i=0;i<scc1.size();i++)
    {
        Node &node1=scc1[i];
        if(node1.gate_type==not11)//not
        {
            not_num++;    
        }
        else if(node1.gate_type==and2)
        {
            for(int j=0;j<node1.in_ports.size();j++)
            {
                if(node1.in_ports[j].in==1)
                {
                    string signal_temp=node1.name+".port"+to_string(node1.in_ports[j].port)+"=1";
                    signals.push_back(signal_temp);
                }
            }
        }
        else if(node1.gate_type==or2)
        {
            for(int j=0;j<node1.in_ports.size();j++)
            {
                if(node1.in_ports[j].in==1)
                {
                    string signal_temp=node1.name+".port"+to_string(node1.in_ports[j].port)+"=0";
                    signals.push_back(signal_temp);
                }
            }
        }
        else if(node1.gate_type==nand2)
        {
            not_num++;
            for(int j=0;j<node1.in_ports.size();j++)
            {
                if(node1.in_ports[j].in==1)
                {
                    string signal_temp=node1.name+".port"+to_string(node1.in_ports[j].port)+"=1";
                    signals.push_back(signal_temp);
                }
            }
        }
    }
    

    
    if(not_num%2==0)
    return false;
    else return true;
}


queue<int> topo_q;
vector<int> topo_vec;
void topo(int start,vector<Node> &scc1)
{
    topo_q.push(scc1[start].id);
    topo_vec.push_back(scc1[start].id);

    while(!topo_q.empty())
    {
        int u1=topo_q.front();
        topo_q.pop();
        for(int i=0;i<scc1[scc_place_map[u1].second].to_ports.size();i++)
        {
            int v1=scc1[scc_place_map[u1].second].to_ports[i].node;
            scc1[scc_place_map[v1].second].indeg--;
            if(scc1[scc_place_map[v1].second].indeg==0)
            {
                topo_q.push(scc1[scc_place_map[v1].second].id);
                topo_vec.push_back(scc1[scc_place_map[v1].second].id);
            }
        }
    }

}
//判断双环起振

//condition放置函数
void condition_placement(Node &node1,string &string1,vector<string> &conditions1,vector<string> &conditions0,vector<string> &conditions01)
{
    if(node1.cir.size()==2)
    {
        conditions01.push_back(string1);
        conditions1.push_back(string1);
        conditions0.push_back(string1);
    }
    else if(node1.cir[0]==0)
    {
        conditions01.push_back(string1);
        conditions0.push_back(string1);
    }
    else if(node1.cir[0]==1)
    {
        conditions01.push_back(string1);
        conditions1.push_back(string1);
    }
    return;

}


void condition_find(int value1,vector<string> &condition1,vector<vector<string>> &conditions,int id1,vector<Node> &scc1)
{
    //回退到了双环上，返回
    if(scc1[scc_place_map[id1].second].cir.size()==2) return;

    if(value1==1)//如果该处需要的值是1
    {
        if(scc1[scc_place_map[id1].second].gate_type==not11)
        {
        if(scc1[scc_place_map[id1].second].input.size()!=0)
        condition_find(0,condition1,conditions,scc1[scc_place_map[id1].second].input[0],scc1);
        else 
        {string signal_temp=scc1[scc_place_map[id1].second].name+".port"+to_string(scc1[scc_place_map[id1].second].in_ports[0].port)+"=0";
        condition1.push_back(signal_temp);
        }
        }
        else if(scc1[scc_place_map[id1].second].gate_type==and2)
        {
            if(scc1[scc_place_map[id1].second].in_ports[0].in==1)
            {
                string signal_temp=scc1[scc_place_map[id1].second].name+".port"+to_string(scc1[scc_place_map[id1].second].in_ports[0].port)+"=1";
                condition1.push_back(signal_temp);
            }
            else 
            {
                string signal_temp=scc1[scc_place_map[id1].second].name+".port"+to_string(scc1[scc_place_map[id1].second].in_ports[1].port)+"=1";
                condition1.push_back(signal_temp);
            }
            condition_find(1,condition1,conditions,scc1[scc_place_map[id1].second].input[0],scc1);
        }
        else if(scc1[scc_place_map[id1].second].gate_type==or2)
        {
            if(scc1[scc_place_map[id1].second].in_ports[0].in==1)
            {
               string signal_temp=scc1[scc_place_map[id1].second].name+".port"+to_string(scc1[scc_place_map[id1].second].in_ports[0].port)+"=1";
               condition1.push_back(signal_temp);
               conditions.push_back(condition1);
               signal_temp=scc1[scc_place_map[id1].second].name+".port"+to_string(scc1[scc_place_map[id1].second].in_ports[0].port)+"=0";
               condition1[condition1.size()-1]=signal_temp;
               condition_find(1,condition1,conditions,scc1[scc_place_map[id1].second].input[0],scc1); 
            }
            else 
            {
               string signal_temp=scc1[scc_place_map[id1].second].name+".port"+to_string(scc1[scc_place_map[id1].second].in_ports[1].port)+"=1";
               condition1.push_back(signal_temp);
               conditions.push_back(condition1);
               signal_temp=scc1[scc_place_map[id1].second].name+".port"+to_string(scc1[scc_place_map[id1].second].in_ports[1].port)+"=0";
               condition1[condition1.size()-1]=signal_temp;
               condition_find(1,condition1,conditions,scc1[scc_place_map[id1].second].input[0],scc1); 
            }
        }
        else if(scc1[scc_place_map[id1].second].gate_type==nand2)
        {
            if(scc1[scc_place_map[id1].second].in_ports[0].in==1)
            {
                string signal_temp=scc1[scc_place_map[id1].second].name+".port"+to_string(scc1[scc_place_map[id1].second].in_ports[0].port)+"=0";
                condition1.push_back(signal_temp);
                conditions.push_back(condition1);
                signal_temp=scc1[scc_place_map[id1].second].name+".port"+to_string(scc1[scc_place_map[id1].second].in_ports[0].port)+"=1";
                condition1[condition1.size()-1]=signal_temp;
                condition_find(0,condition1,conditions,scc1[scc_place_map[id1].second].input[0],scc1);
            }
            else 
            {
                string signal_temp=scc1[scc_place_map[id1].second].name+".port"+to_string(scc1[scc_place_map[id1].second].in_ports[1].port)+"=0";
                condition1.push_back(signal_temp);
                conditions.push_back(condition1);
                signal_temp=scc1[scc_place_map[id1].second].name+".port"+to_string(scc1[scc_place_map[id1].second].in_ports[1].port)+"=1";
                condition1[condition1.size()-1]=signal_temp;
                condition_find(0,condition1,conditions,scc1[scc_place_map[id1].second].input[0],scc1);
            }
        }
    }
    //如果此处需要的值是0
    else 
    {
        if(scc1[scc_place_map[id1].second].gate_type==not11)
        {
            if(scc1[scc_place_map[id1].second].input.size()!=0)
            condition_find(1,condition1,conditions,scc1[scc_place_map[id1].second].input[0],scc1);
            else 
            {
                string signal_temp=scc1[scc_place_map[id1].second].name+".port"+to_string(scc1[scc_place_map[id1].second].in_ports[0].port)+"=1";
                condition1.push_back(signal_temp);
            }
        }
        else if(scc1[scc_place_map[id1].second].gate_type==and2)
        {
            if(scc1[scc_place_map[id1].second].in_ports[0].in==1)
            {
                string signal_temp=scc1[scc_place_map[id1].second].name+".port"+to_string(scc1[scc_place_map[id1].second].in_ports[0].port)+"=0";
                condition1.push_back(signal_temp);
                conditions.push_back(condition1);
                signal_temp=scc1[scc_place_map[id1].second].name+".port"+to_string(scc1[scc_place_map[id1].second].in_ports[0].port)+"=1";
                condition1[condition1.size()-1]=signal_temp;
                condition_find(0,condition1,conditions,scc1[scc_place_map[id1].second].input[0],scc1);
            }
            else 
            {
                string signal_temp=scc1[scc_place_map[id1].second].name+".port"+to_string(scc1[scc_place_map[id1].second].in_ports[1].port)+"=0";
                condition1.push_back(signal_temp);
                conditions.push_back(condition1);
                signal_temp=scc1[scc_place_map[id1].second].name+".port"+to_string(scc1[scc_place_map[id1].second].in_ports[1].port)+"=1";
                condition1[condition1.size()-1]=signal_temp;
                condition_find(0,condition1,conditions,scc1[scc_place_map[id1].second].input[0],scc1);
            }
        }
        else if(scc1[scc_place_map[id1].second].gate_type==or2)
        {
            string signal_temp;
            if(scc1[scc_place_map[id1].second].in_ports[0].in==1)
            {
            signal_temp=scc1[scc_place_map[id1].second].name+".port"+to_string(scc1[scc_place_map[id1].second].in_ports[0].port)+"=0";
            }
            else signal_temp=scc1[scc_place_map[id1].second].name+".port"+to_string(scc1[scc_place_map[id1].second].in_ports[1].port)+"=0";
            condition1.push_back(signal_temp);
            condition_find(0,condition1,conditions,scc1[scc_place_map[id1].second].input[0],scc1);
        }
        else if(scc1[scc_place_map[id1].second].gate_type==nand2)
        {
            string signal_temp;
            if(scc1[scc_place_map[id1].second].in_ports[0].in==1)
            signal_temp=scc1[scc_place_map[id1].second].name+".port"+to_string(scc1[scc_place_map[id1].second].in_ports[0].port)+"=1";
            else signal_temp=scc1[scc_place_map[id1].second].name+".port"+to_string(scc1[scc_place_map[id1].second].in_ports[1].port)+"=1";
            condition1.push_back(signal_temp);
            condition_find(1,condition1,conditions,scc1[scc_place_map[id1].second].input[0],scc1);
        }
    }

    return;
}



bool oscilation_judge1(vector<Node> &scc1,vector<vector<string>> &conditions)
{

    int non_num0=0;
    int non_num1=0;
    int double_cir_start=-1;//记录双环公用的输入结点
    vector<string> wires0;
    vector<string> wires1;

    vector<string> conditions_temp1;//单独1环起振
    vector<string> conditions_temp0;//单独0环起振
    vector<string> conditions_temp01;//0和1环共同起振
    for(int i=0;i<scc1.size();i++)
    {
        Node &node1=scc1[i];
        if(node1.cir.size()==2&&node1.input.size()==2)
        {
            double_cir_start=node1.id;
        }

        if(node1.gate_type==not11)
        {
            node1.indeg=1;
            if(node1.cir.size()==2)
            {
                non_num0++;
                non_num1++;
                wires0.push_back(node1.e[0].name);
                wires1.push_back(node1.e[0].name);
            }
            else if(node1.cir[0]==0)
            {
                non_num0++;
                wires0.push_back(node1.e[0].name);
            }
            else 
            {non_num1++;
            wires1.push_back(node1.e[0].name);
            }
        }
        else if(node1.gate_type==and2)
        {
            int indeg_temp=0;
            for(int j=0;j<node1.in_ports.size();j++)
            {
                if(node1.in_ports[j].in==1)
                {   
                    indeg_temp++;
                    string signal_temp=node1.name+".port"+to_string(node1.in_ports[j].port)+"=1";
                    condition_placement(node1,signal_temp,conditions_temp1,conditions_temp0,conditions_temp01);
                }

            }
            node1.indeg=2-indeg_temp;
            if(node1.cir.size()==2)
            {
                wires0.push_back(node1.e[0].name);
                wires1.push_back(node1.e[0].name);
            }
            else if(node1.cir[0]==0)
            {
                wires0.push_back(node1.e[0].name);
            }
            else 
            {
            wires1.push_back(node1.e[0].name);
            }
        }
        else if(node1.gate_type==or2)
        {
            int indeg_temp=0;
            for(int j=0;j<node1.in_ports.size();j++)
            {
                if(node1.in_ports[j].in==1)
                {
                    indeg_temp++;
                    string signal_temp=node1.name+".port"+to_string(node1.in_ports[j].port)+"=0";
                    condition_placement(node1,signal_temp,conditions_temp1,conditions_temp0,conditions_temp01);
                }
            }
            node1.indeg=2-indeg_temp;
            if(node1.cir.size()==2)
            {
                wires0.push_back(node1.e[0].name);
                wires1.push_back(node1.e[0].name);
            }
            else if(node1.cir[0]==0)
            {
                wires0.push_back(node1.e[0].name);
            }
            else 
            {
            wires1.push_back(node1.e[0].name);
            }
        }
        else if(node1.gate_type==nand2)
        {
            int indeg_temp=0;
            for(int j=0;j<node1.in_ports.size();j++)
            {
                if(node1.in_ports[j].in==1)
                {
                    indeg_temp++;
                    string signal_temp=node1.name+".port"+to_string(node1.in_ports[j].port)+"=1";
                    condition_placement(node1,signal_temp,conditions_temp1,conditions_temp0,conditions_temp01);
                }
            }
            node1.indeg=2-indeg_temp;
            if(node1.cir.size()==2)
            {
                non_num1++;
                non_num0++;
                wires1.push_back(node1.e[0].name);
                wires0.push_back(node1.e[0].name);
            }
            else if(node1.cir[0]==0)
            {
                non_num0++;
                wires0.push_back(node1.e[0].name);
            }
            else 
            {
                non_num1++;
                wires1.push_back(node1.e[0].name);
            }
        }
    }


    //没有奇数负反馈不会起振
    if((non_num1%2==0&&non_num0%2==0)) 
    return false;

    //负反馈数量都是奇数肯定会起振,但是要考虑多重震荡情况的存在
    else if(non_num1%2==1&&non_num0%2==1)
    {
        cir_can_signals.push_back(wires0);
        cir_can_signals.push_back(wires1);
        conditions.push_back(conditions_temp01);
        

        int start_value;
        if(G[double_cir_start].gate_type==nand2||G[double_cir_start].gate_type==and2)
        start_value=1;
        else start_value=0;
        

        for(int i=0;i<conditions_temp0.size();i++)

        for(int i=0;i<conditions_temp1.size();i++)

        
        for(int i=0;i<scc1[scc_place_map[double_cir_start].second].input.size();i++)
        {
            int v1=scc1[scc_place_map[double_cir_start].second].input[i];
          
            if(scc1[scc_place_map[v1].second].cir.size()==1&&scc1[scc_place_map[v1].second].cir[0]==1)
            {
                condition_find(start_value,conditions_temp0,conditions,v1,scc1);
            }
            else if(scc1[scc_place_map[v1].second].cir.size()==1&&scc1[scc_place_map[v1].second].cir[0]==0)
            {
                condition_find(start_value,conditions_temp1,conditions,v1,scc1);
            }
        }

        return true;
    }

    
    else//有一个环是奇数个负反馈
    {
        int non_num;
        if(non_num0%2==1)
        non_num=0;
        else non_num=1;

        vector<string> condition_variable;
        if(non_num==0) condition_variable=conditions_temp0;
        else condition_variable=conditions_temp1;

        int start_value;
        if(G[double_cir_start].gate_type==nand2||G[double_cir_start].gate_type==and2)
        start_value=1;
        else start_value=0;
        
        for(int i=0;i<scc1[scc_place_map[double_cir_start].second].input.size();i++)
        {
            conditions.clear();
            int v1=scc1[scc_place_map[double_cir_start].second].input[i];
            if(scc1[scc_place_map[v1].second].cir.size()==1&&scc1[scc_place_map[v1].second].cir[0]!=non_num)
            {
                condition_find(start_value,condition_variable,conditions,v1,scc1);
                if(conditions.size()!=0)
                {
                    if(non_num==0) cir_can_signals.push_back(wires0);
                    else cir_can_signals.push_back(wires1);
                    return true;
                }
            }
        }
        
    }


    return false;

}



bool condition_find_for_3loop(vector<Node> &scc1,int id,value value1,int non,vector<string> &condition_temp)
{
    Node& node1=scc1[scc_place_map[id].second];
    //该点已经被搜索过
    if(node1.gate_value!=x)
    {
        if(node1.gate_value==value1)
        {
            return true;
        }
        else return false;
    }


    //该点返回到了目标环
    for(int i=0;i<node1.cir.size();i++)
    {
        if(node1.cir[i]==non)
        return false;
    }


    node1.gate_value=value1;

    //如果此处需要的值是0
    if(value1==zero)
    {

        //not1
        if(node1.gate_type==not11)
        {
            //为输入端口
            if(node1.input.size()==0)
            {
                string signal_temp=node1.name+".port"+to_string(node1.in_ports[0].port)+"=1";
                condition_temp.push_back(signal_temp);
                return true;
            }
            //不是输入端口
            else 
            {
                if(condition_find_for_3loop(scc1,node1.input[0],one,non,condition_temp))
                return true;
            }
        }

        //and2
        else if(node1.gate_type==and2)
        {

            //and2有一个输入在scc里面
            if(node1.input.size()==1)
            {
                int port_num;
                if(node1.in_ports[0].in==1)
                {
                 port_num=node1.in_ports[0].port;   
                }
                else port_num=node1.in_ports[1].port;


                string signal_temp=node1.name+".port"+to_string(port_num)+"=0";
                condition_temp.push_back(signal_temp);
                return true;
            }
            //and2有两个输入在scc里面
            else
            {
                //贪心找输入条件
                bool flag=condition_find_for_3loop(scc1,node1.input[0],zero,non,condition_temp)||condition_find_for_3loop(scc1,node1.input[1],zero,non,condition_temp);
                if(flag)
                return true;
            }
        }

        //or2
        else if(node1.gate_type==or2)
        {
            //or2有一个输入在scc里面
            if(node1.input.size()==1)
            {
                int port_num;
                if(node1.in_ports[0].in==1)
                {
                    port_num=node1.in_ports[0].port;
                }
                else port_num=node1.in_ports[1].port;

                if(condition_find_for_3loop(scc1,node1.input[0],zero,non,condition_temp))
                {
                string signal_temp=node1.name+".port"+to_string(port_num)+"=0";
                condition_temp.push_back(signal_temp);
                return true;
                }
            }

            //or2有两个输入在scc里面
            else
            {
                //贪心，先找到第一个输入的固定条件，再找到第二个输入的固定条件
                bool flag=condition_find_for_3loop(scc1,node1.input[0],zero,non,condition_temp)&&condition_find_for_3loop(scc1,node1.input[1],zero,non,condition_temp);
                if(flag)
                return true;
            }
        }


        //nand2
        else if(node1.gate_type==nand2)
        {
            //nand2只有一个输入在scc里面
            if(node1.input.size()==1)
            {
                int port_num;
                if(node1.in_ports[0].in==1)
                {
                    port_num=node1.in_ports[0].port;
                }

                else
                port_num=node1.in_ports[1].port;
                
                if(condition_find_for_3loop(scc1,node1.input[0],one,non,condition_temp))
                {
                    
                string signal_temp=node1.name+".port"+to_string(port_num)+"=1";
                condition_temp.push_back(signal_temp);
                return true;
                }       
            }
            //nand2有两个输入在scc里面
            else 
            {
                //贪心，先找一个输入端口的固定条件，再找另一个输入端口的固定条件
                bool flag=condition_find_for_3loop(scc1,node1.input[0],one,non,condition_temp)&&condition_find_for_3loop(scc1,node1.input[1],one,non,condition_temp);

            }
        }



    }


    //如果此处需要的值是1
    else
    {
        //not1
        if(node1.gate_type==not11)
        {
            if(condition_find_for_3loop(scc1,node1.input[0],zero,non,condition_temp))
            return true;
        }

        //and2
        else if(node1.gate_type==and2)
        {
            //and2有一个输入在scc里面
            if(node1.input.size()==1)
            {
                int port_num;
                if(node1.in_ports[0].in==1)
                {
                    port_num=node1.in_ports[0].port;
                }
                else port_num=node1.in_ports[1].port;

                if(condition_find_for_3loop(scc1,node1.input[0],one,non,condition_temp))
                {
                    string signal_temp=node1.name+".port"+to_string(port_num)+"=1";
                    condition_temp.push_back(signal_temp);
                    return true;
                }
            }

            else
            {
                bool flag=condition_find_for_3loop(scc1,node1.input[0],one,non,condition_temp)&&condition_find_for_3loop(scc1,node1.input[1],one,non,condition_temp);
                if(flag)
                return true;
            }
        }

        //or2
        else if(node1.gate_type==or2)
        {
            //or2有一个输入在scc里面
            if(node1.input.size()==1)
            {
                int port_num;
                if(node1.in_ports[0].in==1)
                {
                    port_num=node1.in_ports[0].port;
                }
                else port_num=node1.in_ports[1].port;

                string signal_temp=node1.name+".port"+to_string(port_num)+"=1";
                condition_temp.push_back(signal_temp);
                return true;
            }


            //or2有两个输入在一个scc里面
            else 
            {
                bool flag=condition_find_for_3loop(scc1,node1.input[0],one,non,condition_temp)||condition_find_for_3loop(scc1,node1.input[1],one,non,condition_temp);
                if(flag) 
                return true;
            }
        }


        //nand2
        else if(node1.gate_type==nand2)
        {
            //nand2只有一个输入在scc里面
            if(node1.input.size()==1)
            {
                int port_num;
                if(node1.in_ports[0].in==1)
                {
                    port_num=node1.in_ports[0].port;
                }
                else port_num=node1.in_ports[1].port;

                string signal_temp=node1.name+".port"+to_string(port_num)+"=0";
                condition_temp.push_back(signal_temp);
                return true;
            }

            //nand2有两个输入在scc里面
            else
            {
                bool flag=condition_find_for_3loop(scc1,node1.input[0],zero,non,condition_temp)||condition_find_for_3loop(scc1,node1.input[1],zero,non,condition_temp);
                if(flag)
                return true;
            }

        }
    }


    //回溯
    node1.gate_value=x;
    return false;
    


}

bool oscilation_judge2(vector<Node> &scc1,vector<vector<string>> &conditions)
{

    //存储奇数负反馈的个数
    int non_num[3]={0};

    //存储每个环的端口条件
    vector<vector<string>> condition_temp;
    //存储交叉点
    vector<vector<int>> cir_junction_point;
    for(int i=0;i<3;i++)
    {
        cir_junction_point.push_back(vector<int>());
    }


    for(int i=0;i<3;i++)
    {
        condition_temp.push_back(vector<string>());
    }


    for(int i=0;i<scc1.size();i++)
    {

        //not1
        if(scc1[i].gate_type==not11)
        {
            for(int j=0;j<scc1[i].cir.size();j++)
            {
                non_num[scc1[i].cir[j]]++;
            }
        }


        //and2
        else if(scc1[i].gate_type==and2)
        {
            for(int j=0;j<scc1[i].in_ports.size();j++)
            {
                if(scc1[i].in_ports[j].in==1)
                {
                    string condition1=scc1[i].name+".port"+to_string(scc1[i].in_ports[j].port)+"=1";
                    for(int k=0;k<scc1[i].cir.size();k++)
                    {
                        condition_temp[scc1[i].cir[k]].push_back(condition1);
                    }
                }
            }

            //and2是环的交界输入点
            if(scc1[i].input.size()==2)
            {
                for(int j=0;j<scc1[i].cir.size();j++)
                {
                    cir_junction_point[scc1[i].cir[j]].push_back(scc1[i].id);
                }
            }
        }

        //or2
        else if(scc1[i].gate_type==or2)
        {
            for(int j=0;j<scc1[i].in_ports.size();j++)
            {
                if(scc1[i].in_ports[j].in==1)
                {
                    string condition1=scc1[i].name+".port"+to_string(scc1[i].in_ports[j].port)+"=0";
                    for(int k=0;k<scc1[i].cir.size();k++)
                    {
                        condition_temp[scc1[i].cir[k]].push_back(condition1);
                    }
                }
            }


            //or2是环的交界输入点
            if(scc1[i].input.size()==2)
            {
                for(int j=0;j<scc1[i].cir.size();j++)
                {
                    cir_junction_point[scc1[i].cir[j]].push_back(scc1[i].id);
                }
            }
        }

        else if(scc1[i].gate_type==nand2)
        {
            vector<string> condition_temp1;
            for(int j=0;j<scc1[i].in_ports.size();j++)
            {
                if(scc1[i].in_ports[j].in==1)
                {
                    condition_temp1.push_back(scc1[i].name+".port"+to_string(scc1[i].in_ports[j].port)+"=1");
                }
            }

            for(int j=0;j<scc1[i].cir.size();j++)
            {
                non_num[scc1[i].cir[j]]++;
                for(int k=0;k<condition_temp1.size();k++)
                {
                    condition_temp[scc1[i].cir[j]].push_back(condition_temp1[k]);
                }
            }


            //nand2是环的交界输入点
            if(scc1[i].input.size()==2)
            {
                for(int j=0;j<scc1[i].cir.size();j++)
                {
                    cir_junction_point[scc1[i].cir[j]].push_back(scc1[i].id);
                }
            }
        }
    }

    //三个负反馈环，肯定会起振
    if(non_num[0]%2==1&&non_num[1]%2==1&&non_num[2]%2==1)
    {
        unordered_set<string> uniqueSet;
        uniqueSet.insert(condition_temp[0].begin(),condition_temp[0].end());
        uniqueSet.insert(condition_temp[1].begin(),condition_temp[1].end());
        uniqueSet.insert(condition_temp[2].begin(),condition_temp[2].end());

        conditions.push_back(vector<string>(uniqueSet.begin(),uniqueSet.end()));

        return true;
    }


    //没有可以振荡的环
    else if(non_num[0]%2==0&&non_num[1]%2==0&&non_num[2]%2==0)
    return false;


    //只有一个环是负反馈环
    else if(int(non_num[0]%2==1)+int(non_num[1]%2==1)+int(non_num[2]%2==1)==1)
    {
        int non;
        if(non_num[0]%2==1) non=0;
        else if(non_num[1]%2==1) non=1;
        else non=2;

        bool flag1=true;
        for(int i=0;i<cir_junction_point[non].size();i++)
        {
            int id=cir_junction_point[non][i];
            Node &node1=scc1[scc_place_map[id].second];
            
            value value1;
            if(node1.gate_type==and2||node1.gate_type==nand2) value1=one;
            else value1=zero;

            for(int i=0;i<node1.input.size();i++)
            {
                auto it=find(scc1[scc_place_map[node1.input[i]].second].cir.begin(),scc1[scc_place_map[node1.input[i]].second].cir.end(),non);
                
                //该处的输入端不在本次搜索的环里面
                if(it==scc1[scc_place_map[node1.input[i]].second].cir.end())
                {
                    flag1=condition_find_for_3loop(scc1,node1.input[i],value1,non,condition_temp[non]);
                }

                if(!flag1) 
                break;
            }

            if(!flag1) break;
        }


        if(flag1)
        {
            conditions.push_back(condition_temp[non]);

            return true;
        }


    }

    

    return false;
}


int main(int argc,char *argv[])
{

    auto start=std::chrono::high_resolution_clock::now();
    
    
    //读入
    if(argc<2)
    {
        cerr<<"Usage:"<<argv[0]<<" "<<"<filename>"<<endl;
        return 1;
    }

    string filename=argv[1];
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Error opening file: " << filename <<endl;
        return 1;
    }

    string line;

    //跳过第一行
    getline(file,line);


    //读取一行
    while(getline(file,line))
    {
        istringstream iss(line);
        int node_id;
        string gate_type_str,gate_name,output_wire,maohao;
        vector<string> input_wires;
        
        iss>>node_id>>maohao>>gate_type_str>>gate_name>>output_wire;

        string input_wire;
        while(iss>>input_wire)
        {
            input_wires.push_back(input_wire);
        }
        


        Node node1;
        node1.name=gate_name;
        node1.id=node_id;
        if(gate_type_str=="not1") node1.gate_type=not11;
        else if(gate_type_str=="or2") node1.gate_type=or2;
        else if(gate_type_str=="nand2") node1.gate_type=nand2;
        else if(gate_type_str=="and2") node1.gate_type=and2;
        else
        {
            cerr<<"Error: unknown gate type: "<<gate_type_str<<endl;
            return 1;
        }

        G.push_back(node1);


        HE[output_wire].name=output_wire;

        HE[output_wire].fro.push_back(node_id);
        
        for(int i=0;i<input_wires.size();i++)
        {
            HE[input_wires[i]].name=input_wires[i];
            HE[input_wires[i]].to.push_back(node_id);
            HE[input_wires[i]].to_ports.push_back({node_id,i+1,1});
        }

        gate_num++;
    }

    file.close();

    for(auto &pair:HE)
    {
        h_edge &edge=pair.second;
        //输出端口的信号线
        if(edge.fro.size()==0)
        {
            Node node1;
            node1.name=edge.name;
            node1.gate_type=inp;
            edge.fro.push_back(gate_num);
            node1.e.push_back(edge);
            G.push_back(node1);
            gate_num++;
        }
        else 
        {
            int node_id=edge.fro[0];
            G[node_id].e.push_back(edge);
        }

    }

    for(int i=0;i<gate_num;i++)
    {
        vis.push_back(0);
        dfn.push_back(-1);
        low.push_back(-1);
    }


    //tarjan算法求解scc；
    for(int i=0;i<gate_num;i++)
    {
        if(dfn[i]==-1)
        {
            tarjan(i);
        }
    }
    

    //构建scc之中边的关系

    for(auto pair:HE)
    {
        h_edge &edge=pair.second;
        int u1=edge.fro[0];
        for(int i=0;i<edge.to.size();i++)
        {
            int v1=edge.to[i];
            if(G[u1].scc==G[v1].scc)
            {
                G_scc[G[u1].scc].e.push_back({edge,edge.to_ports[i]});
                scc_vec[scc_place_map[u1].first][scc_place_map[u1].second].to_ports.push_back(edge.to_ports[i]);
            }
            else 
            {
                G_scc[G[v1].scc].inputs.push_back(edge.to_ports[i]);
            }
        }
    }



    for(int i=0;i<G_scc.size();i++)
    {
        unordered_map<string,bool> signal_map;
        for(int j=0;j<G_scc[i].e.size();j++)
        {
            h_edge &edge=G_scc[i].e[j].edge;
            port_name port=G_scc[i].e[j].port;
            string signal_name=edge.name;
            signal_map[signal_name]=true;
            string gate_name;
            gate_name=G[port.node].name+".port"+to_string(port.port);
            G_scc[i].gates.push_back(gate_name);
        }

        for(auto &pair:signal_map)
        {
            G_scc[i].signals.push_back(pair.first);
        }
    }


    //输出第一题结果
    ofstream outputFile("result_1.txt");
    if(outputFile.is_open())
    {
    int num1=1;
    for(int i=0;i<G_scc.size();i++)
    {
        if(G_scc[i].node.size()!=1)
        {
            sort(G_scc[i].signals.begin(),G_scc[i].signals.end());
            sort(G_scc[i].gates.begin(),G_scc[i].gates.end());
            outputFile<<num1<<")"<<endl;
            //输出信号线;
            outputFile<<"Loop Signals:";
            for(int j=0;j<G_scc[i].signals.size();j++)
            {   if(j==0)
                outputFile<<G_scc[i].signals[j];
                else 
                outputFile<<","<<G_scc[i].signals[j];
            }
            outputFile<<endl;
            //输出门信息
            outputFile<<"Loop Gates:";
            for(int j=0;j<G_scc[i].gates.size();j++)
            {
                if(j==0)
                outputFile<<G_scc[i].gates[j];
                else 
                outputFile<<","<<G_scc[i].gates[j];
            }
            outputFile<<endl;
            num1++;
        }
    }

    /*第一题用时*/
    auto end1=std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration1 = end1 - start;
    cout<<"Time taken of Problem1: " << duration1.count() << " seconds" << endl;

    }
    
    else {
    std::cerr << "Failed to open the file." << std::endl;
    }

    outputFile.close();
    G.clear();
    HE.clear();


    //开始第二问
    auto start2=std::chrono::high_resolution_clock::now();
    vector<vector<string>> osc_cannt_gates;
    vector<vector<string>> osc_cannt_signals;
    vector<vector<string>> osc_can_gates;
    vector<vector<vector<string>>> osc_can_conditions;//由于震荡可能有多重情况，用三维数组存储
    vector<vector<string>> osc_can_signals;

    for(int i=0;i<vis.size();i++)
    {
        vis[i]=0;
    }
    while(!st.empty())
    {
        st.pop();
    }
    
    for(int i=0;i<scc_vec.size();i++)
    {
        if(scc_vec[i].size()==1) continue;
        cir_count=0;
        johnson(scc_vec[i][0].id,scc_vec[i]);
    


        /*//检查johnson()
        for(int j=0;j<scc_vec[i].size();j++)
        {
            cout<<scc_vec[i][j].name<<".cir:";
            for(int k=0;k<scc_vec[i][j].cir.size();k++)
            cout<<scc_vec[i][j].cir[k]<<" ";
            cout<<endl;
        }*/



        //向结点添加输入端口
        for(int j=0;j<scc_vec[i].size();j++)
        {
            Node& node1=scc_vec[i][j];
            node1.gate_value=x;
            if(node1.gate_type==not11) continue;// not不可能有额外的输入端口
            else
            {
                if(node1.in_ports.size()!=2)
                {
                    port_name port1;
                    port1.in=1;
                    port1.node=node1.id;
                    if(node1.in_ports[0].port==1)
                    port1.port=2;
                    else port1.port=1;
                    node1.in_ports.push_back(port1);
                }
            }
        }

        //该scc之中只有一个环
        if(cir_count==1)
        {
            vector<string> signals_temp;
            if(oscilation_judge0(scc_vec[i],signals_temp))//如果能振荡
            {
                sort(signals_temp.begin(),signals_temp.end());
                vector<vector<string>> condition_temp;
                condition_temp.push_back(signals_temp);
                osc_can_conditions.push_back(condition_temp);
                osc_can_gates.push_back(G_scc[i].gates);
                osc_can_signals.push_back(G_scc[i].signals);
                cir_can_signals.push_back(G_scc[i].signals);
            }
            else//如果不能振荡
            {
                osc_cannt_gates.push_back(G_scc[i].gates);
                osc_cannt_signals.push_back(G_scc[i].signals);
            }
        }

        else if(cir_count==2)
        {
            vector<vector<string>> condition_temp;
            if(oscilation_judge1(scc_vec[i],condition_temp))
            {


            for(int j=0;j<condition_temp.size();j++)
            sort(condition_temp[j].begin(),condition_temp[j].end());
            osc_can_conditions.push_back(condition_temp);
            
         

            osc_can_gates.push_back(G_scc[i].gates);
            osc_can_signals.push_back(G_scc[i].signals);
            }
            else
            {
                osc_cannt_gates.push_back(G_scc[i].gates);
                osc_cannt_signals.push_back(G_scc[i].signals);
            }
        }

        else if(cir_count==3)
        {
            vector<vector<string>> condition_temp;
            if(oscilation_judge2(scc_vec[i],condition_temp))
            {
                for(int j=0;j<condition_temp.size();j++)
                sort(condition_temp[j].begin(),condition_temp[j].end());
                osc_can_conditions.push_back(condition_temp);

                osc_can_gates.push_back(G_scc[i].gates);
                osc_can_signals.push_back(G_scc[i].signals);
            }

            else 
            {
                osc_cannt_gates.push_back(G_scc[i].gates);
                osc_cannt_signals.push_back(G_scc[i].signals);
            }
        }

    }

    ofstream outputFile2("result_2.txt");
    if(outputFile2.is_open())
    {
        for(int i=0;i<osc_cannt_gates.size();i++)
        {
            outputFile2<<i+1<<")"<<endl;
            outputFile2<<"Loop Signals:";
            for(int j=0;j<osc_cannt_signals[i].size();j++)
            {
                if(j==0)
                {
                outputFile2<<osc_cannt_signals[i][j];
                }
                else
                {
                outputFile2<<","<<osc_cannt_signals[i][j];
                }
            }
            for(int j=0;j<osc_cannt_gates[i].size();j++)
            {
                if(j==0)
                {
                outputFile2<<endl<<"Loop Gate:"<<osc_cannt_gates[i][j];
                }
                else
                {
                outputFile2<<","<<osc_cannt_gates[i][j];
                }
            }
            outputFile2<<endl;
        }
    }
    else {
    std::cerr << "Failed to open the file." << std::endl;
    }
    outputFile2.close();

    
    ofstream outputFile3("result_3.txt");
    if(outputFile3.is_open())
    {
        for(int i=0;i<osc_can_gates.size();i++)
        {
            outputFile3<<i+1<<")"<<endl;
            outputFile3<<"Loop Signals:";
            for(int j=0;j<osc_can_signals[i].size();j++)
            {
                if(j==0)
                {
                outputFile3<<osc_can_signals[i][j];
                }
                else
                {
                outputFile3<<","<<osc_can_signals[i][j];
                }
            }
            for(int j=0;j<osc_can_gates[i].size();j++)
            {
                if(j==0)
                {
                outputFile3<<endl<<"Loop Gate:"<<osc_can_gates[i][j];
                }
                else
                {
                outputFile3<<","<<osc_can_gates[i][j];
                }
            }
            for(int j=0;j<osc_can_conditions[i].size();j++)
            {
                for(int k=0;k<osc_can_conditions[i][j].size();k++)
                {
                if(k==0)
                {
                outputFile3<<endl<<"Loop Condition:"<<osc_can_conditions[i][j][k];
                }
                else
                {
                outputFile3<<","<<osc_can_conditions[i][j][k];
                }
                }
            }    
            outputFile3<<endl;    
            }
    }
    else {
    std::cerr << "Failed to open the file." << std::endl;
    }
    outputFile3.close();
    
    auto end2=std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration2=end2-start2;
    cout<<"Time taken of Problem2 and Problem3:"<<duration2.count()<<"seconds"<<endl;

    
    return 0;

}