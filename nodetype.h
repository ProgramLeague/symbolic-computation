#pragma once
#include <string>
#include <vector>
#include <functional>
using namespace std;

enum nodeType{Num,String,Var,Pro,Fun,VarRef};

class BasicNode //不可直接创建对象
{
protected:
    bool isRet=false;
public:
    virtual int getType();
    virtual void addNode(BasicNode* node) {this->sonNode.push_back(node);} //使用该方法添加成员可查错
    virtual BasicNode* eval();
    virtual ~BasicNode();

    //fix:这个不对劲，准备删掉
    void setRet() {this->isRet=true;} //不可eval节点设置ret无效
    bool getRet() {return this->isRet;}
    vector<BasicNode*> sonNode;
};
typedef function<bool(vector<BasicNode*>&sonNode)>canBE; //检测函数基础求值参数表是否合法
typedef function<BasicNode*(vector<BasicNode*>&sonNode)>BE; //进行基础求值


class NumNode : public BasicNode
{
protected:
    double num;
public:
    virtual int getType() {return Num;}
    virtual void addNode(BasicNode *node) {throw string("NumNode no sonNode");}
    virtual BasicNode* eval() {return dynamic_cast<BasicNode*>(this);}
    NumNode(double num) {this->num=num;}
    NumNode(NumNode* node) {this->num=node->num;}

    double getNum() {return this->num;}
};


class StringNode : public BasicNode
{
protected:
    string str;
public:
    virtual int getType() {return String;}
    virtual void addNode(BasicNode *node) {throw string("String no sonNode");}
    virtual BasicNode* eval() {return dynamic_cast<BasicNode*>(this);}
    StringNode(string str) {this->str=str;}
    StringNode(StringNode* node) {this->str=node->str;}

    string getStr() {return this->str;}
};


class VarNode : public BasicNode
{
protected:
    BasicNode* val=nullptr;
    int valtype=-1;
    bool isownership;
public:
    virtual int getType() {return Var;}
    virtual void addNode(BasicNode* node) {throw string("VariableNode no sonNode");}
    virtual BasicNode* eval();
    virtual ~VarNode();
    VarNode(){}
    VarNode(VarNode* node);

    bool isEmpty() {return (this->valtype==-1);}
    int getValType() {return this->valtype;}
    bool getOwnership() {return this->isownership;}
    void setVal(BasicNode* val); //直接对值进行赋值，用这个传进来意味着转移所有权到本类（一般赋值为字面量用）
    void setBorrowVal(BasicNode* val); //直接对值进行赋值，用这个不转移所有权（一般赋值为变量指针用）
    void setVarVal(VarNode* node); //传递变量的值到this的值，即需要进行一次解包
    void clearVal();
};
typedef VarNode Variable; //内存实体是Variable，其指针才作为节点（不像某些节点一样是遇到一个就new一次），参考函数实体和函数节点的思想


class VarRefNode : public BasicNode
{
protected:
    BasicNode* val=nullptr;
    int valtype=-1;
    bool isownership;

    void setVal(BasicNode* val);
    void setBorrowVal(BasicNode* val);
public:
    virtual int getType() {return VarRef;}
    virtual void addNode(BasicNode* node) {throw string("VarRefNode no sonNode");}
    virtual ~VarRefNode(); //实参在绑定到VarRef时进行复制，因此本类对象具备所有权
    virtual BasicNode* eval(); //eval结果是目前形参绑定到的实参

    void bind(BasicNode* val); //传进来的是求值后且已经拷贝完毕的实参，传进来即转移所有权（拷贝的过程由函数的eval完成）
    void unbind();
    bool isbind() {return (this->val!=nullptr);}
    bool getOwnership() {return this->isownership;}
};


class ProNode : public BasicNode
{
public:
    virtual int getType() {return Pro;}
    virtual BasicNode* eval() {throw string("ProNode cannot be eval");}

    //BasicNode* getHeadNode() {return this->sonNode.at(0);}
    BasicNode* getSen(int sub) {return this->sonNode.at(sub);}
    void passEffect(vector<BasicNode*>sonNode);
};


class Function
{
private:
    int parnum; //参数个数
    ProNode* pronode; //是ret节点返回，最后一个元素视为返回值（如果没有填nullptr）（fix:这个路子是错的，ret方式需要更改）
    bool VLP; //是否不进行参数个数检查
    //关于基础求值
    canBE canBEfun;
    BE BEfun;
    bool iscanBE=false;
    //BasicNode* basicEval(vector<BasicNode *> &sonNode); //直接在eval里写了，这里消掉
public:
    Function(int parnum,ProNode* pronode=nullptr,bool VLP=false):parnum(parnum),pronode(pronode),VLP(VLP){} //普通函数（有函数体）
    Function(int parnum,canBE canBEfun,BE BEfun,bool VLP=false):
        parnum(parnum),canBEfun(canBEfun),BEfun(BEfun),VLP(VLP),iscanBE(true){} //调用到函数接口
    ~Function() {delete pronode;}

    ProNode* getFunBody() {return this->pronode;}
    int getParnum() {return this->parnum;}
    bool isVLP() {return this->VLP;}
    BasicNode* eval(vector<BasicNode *> &sonNode);
};


class FunNode : public BasicNode
{
protected:
    Function* funEntity; //所有权在scope，不在这里析构
public:
    virtual int getType() {return Fun;}
    virtual void addNode(BasicNode* node);
    virtual BasicNode* eval();
    FunNode(Function* funEntity=nullptr):funEntity(funEntity){}

    bool haveEntity() {return this->fsunEntity!=nullptr;}
    void setEntity(Function* funEntity) {this->funEntity=funEntity;}
    ProNode* getFunBody() {return this->funEntity->getFunBody();}
};
