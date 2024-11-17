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



//johnson找到所有的环
int cir_count;



void johnson(int u1,vector<Node> &scc1)
{
    vis[u1]=1;
    st.push(u1);

    
    for(int i=0;i<scc1[scc_place_map[u1].second].to_ports.size();i++)
    {
    

        port_name port1=scc1[scc_place_map[u1].second].to_ports[i];
        scc1[scc_place_map[u1].second].to_ports[i].in=0;
        port1.in=0;
        scc1[scc_place_map[port1.node].second].in_ports.push_back(port1);
        
        
        int v1=scc1[scc_place_map[u1].second].to_ports[i].node;

        if(vis[v1]==0)
        {
            johnson(v1,scc1);
        }
        
        else
        {
            vector<int> st_temp;
            int k1=v1;
            while(true)
            {
                scc1[scc_place_map[k1].second].cir.push_back(cir_count);
                k1=st.top();
                if(k1==v1) break;
                st.pop();
                st_temp.push_back(k1);
            }
            cir_count++;
            for(int j=0;j<st_temp.size();j++)
            {
                st.push(st_temp[st_temp.size()-1-j]);
            }
        }


        //两个输入端口相同的情况
        if(i==1)
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
    return;
}


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
/*
    for(int i=0;i<G.size();i++)
    {
        cout<<i<<":"<<G[i].name<<" "<<G[i].gate_type<<":";
        cout<<"e:"<<G[i].e[0].name<<endl;
    }

    for(auto &pair:HE)
    {
        cout<<pair.second.name<<" ";
        for(int i=0;i<pair.second.to.size();i++)
        {
            cout<<G[pair.second.to_ports[i].node].name<<" ";
            cout<<pair.second.to_ports[i].port<<" ";
        }
        cout<<endl;
    }

*/
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

    //开始第二问
    auto start2=std::chrono::high_resolution_clock::now();
    vector<vector<string>> osc_cannt;
    vector<vector<string>> osc_can;
    vector<vector<string>> osc_can_condition;

    /*检查端口关系是否正确
    for(int i=0;i<scc_vec.size();i++)
    {
        for(int j=0;j<scc_vec[i].size();j++)
        {
            cout<<scc_vec[i][j].name<<" ";
            for(int k=0;k<scc_vec[i][j].to_ports.size();k++)
            {
                cout<<scc_vec[i][j].to_ports[k].node<<".port";
                cout<<scc_vec[i][j].to_ports[k].port<<" ";
            }
            cout<<endl;
        }
    }
    */

    

    //初始化vis、st
    for(int i=0;i<vis.size();i++)
    vis[i]=0;
    while(!st.empty())
    {
        st.pop();
    }
    
    for(int i=0;i<scc_vec.size();i++)
    {
        
        if(scc_vec[i].size()==1) continue;
        cir_count=0;
        johnson(scc_vec[i][0].id,scc_vec[i]);
        /*检查环路是否正确
        for(int j=0;j<scc_vec[i].size();j++)
        {
            cout<<scc_vec[i][j].name<<" ";
            for(int k=0;k<scc_vec[i][j].cir.size();k++)
            {
                cout<<scc_vec[i][j].cir[k]<<" ";
            }
            cout<<endl;
        }
        */
     
        //向结点添加输入端口
        for(int j=0;j<scc_vec[i].size();j++)
        {
            Node& node1=scc_vec[i][j];
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
        

        /*检查输入端口添加是否正确；
        for(int j=0;j<scc_vec[i].size();j++)
        {
            cout<<scc_vec[i][j].name<<" ";
            for(int k=0;k<scc_vec[i][j].in_ports.size();k++)
            {
                cout<<"port"<<k+1<<":"<<scc_vec[i][j].in_ports[k].in<<" ";
            }
            cout<<endl;
        }
       */


        //该scc之中只有一个环
        if(cir_count==0)
        {
            vector<string> signals_temp;
            if(oscilation_judge0(scc_vec[i],signals_temp))//如果能振荡
            {
                sort(signals_temp.begin(),signals_temp.end());
                osc_can_condition.push_back(signals_temp);
                osc_can.push_back(G_scc[i].gates);
            }
            else//如果不能振荡
            {
                osc_cannt.push_back(G_scc[i].gates);
            }
        }
       
    }
    
    
    auto end2=std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration2=end2-start2;
    
    
    
    return 0;

}