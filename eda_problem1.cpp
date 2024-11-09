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


struct port_name
{
    int node;
    int port;
};
//超边
struct h_edge
{
    string name;
    vector<int> fro;
    vector<int> to;
    vector<port_name> in_ports;
};


//边
struct edge
{
    string name;
    int fro;
    int to;
    port_name in_ports;
};


//超边集合和边集合
unordered_map<string,h_edge> HE;
vector<edge> E;


struct Node
{
    string name;
    TYPE gate_type;
    vector<edge> e;
    int scc;//所属scc序号
};

vector<Node> G;

//将超边转换成边，并且添加到G中
void he_to_e(unordered_map<string,h_edge> HE1, vector<edge> &E1)
{
    for(auto &pair:HE1)
    {
        h_edge &he1=pair.second;
        
        if(he1.fro.empty()==1)//该信号线为输入信号
        {
            //添加输入节点
            Node node1;
            node1.name=he1.name;
            node1.gate_type=inp;
            G.push_back(node1);

            
            he1.fro.push_back(gate_num);
            gate_num++;
            
        }

        for(int i=0;i<he1.to.size();i++)
        {
            edge e1;
            e1.name=he1.name;
            e1.fro=he1.fro[0];
            e1.to=he1.to[i];
            e1.in_ports=he1.in_ports[i];

            E1.push_back(e1);
            G[he1.fro[0]].e.push_back(e1);
        }
    }
}


struct scc_node
{
    vector<int> node;
    vector<edge> e_in;
    vector<edge> e_out;
    unordered_map<string,bool> signals_temp;
    vector<port_name> ports_temp;
    vector<string> gates;
    vector<string> signals;
};

vector<scc_node> G_scc;




//tarjan求解scc
vector<bool> vis;
vector<int> dfn;
vector<int> low;
stack<int> st;
int tot=0;

void tarjan(int u)
{
    low[u]=tot;
    dfn[u]=low[u];
    tot++;
    st.push(u);
    vis[u]=1;


    //dfs
    for(int i=0;i<G[u].e.size();i++)
    {
        int v=G[u].e[i].to;
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
        int k=-1;
        scc_node scc_node1;
        do
        {
            k=st.top();
            vis[k]=0;
            st.pop();
            scc_node1.node.push_back(k);
        }while(u!=k); 

        G_scc.push_back(scc_node1);
        for(int i=0;i<scc_node1.node.size();i++)
        {
            int v=scc_node1.node[i];
            G[v].scc=G_scc.size()-1;
        }
    }
}

int main(int argc,char *argv[])
{

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
            HE[input_wires[i]].in_ports.push_back({node_id,i+1});
        }

        gate_num++;
    }

    file.close();

    //超边转换为边，构建有向图
    he_to_e(HE,E);



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
    for(int i=0;i<E.size();i++)
    {
        int fro=E[i].fro;
        int to=E[i].to;
        
        int scc_fro=G[fro].scc;
        int scc_to=G[to].scc;

        if(scc_fro==scc_to)
        {
            G_scc[scc_fro].e_in.push_back(E[i]);
            G_scc[scc_to].signals_temp[E[i].name]=1;
            G_scc[scc_to].ports_temp.push_back(E[i].in_ports);
        }
        else 
        {
            G_scc[scc_fro].e_out.push_back(E[i]);
        }
    }
    

    for(int i=0;i<G_scc.size();i++)
    {
        for(auto &pair:G_scc[i].signals_temp)
        {
            G_scc[i].signals.push_back(pair.first);
        }
        for(int j=0;j<G_scc[i].ports_temp.size();j++)
        {
            string temp1;
            temp1=G[G_scc[i].ports_temp[j].node].name+".port"+to_string(G_scc[i].ports_temp[j].port);
            G_scc[i].gates.push_back(temp1);
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
            {
                outputFile<<G_scc[i].signals[j]<<",";
            }
            outputFile<<endl;
            //输出门信息
            outputFile<<"Loop Gates:";
            for(int j=0;j<G_scc[i].gates.size();j++)
            {
                outputFile<<G_scc[i].gates[j]<<",";
            }
            outputFile<<endl;
            num1++;
        }
    }
    }

    else {
    std::cerr << "Failed to open the file." << std::endl;
    }



    return 0;

}