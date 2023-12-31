#include"main_parser.h"

int w;  //获得GetToken函数的返回值即读入的单词种类编码
char kind[100]; //储存类型关键字
char tokenText0[100]; //储存第一个函数名或者变量名
int indent0 = 0;  //初始化缩进值
queue<print> printList; //用于方便打印缩进

status Program(FILE* fp, CTree& T)  //语法单位<程序>的子程序
{
	CTree c;
	struct print elem = { indent0,line_num };
	printList.push(elem);//存入程序的缩进值
	w = GetToken(fp);
	if (!ExternalDefList(fp, c))
        return ERROR;  //调用外部定义序列函数
	T.n = 1; T.r = 0;
	T.nodes[0].data = (char*)malloc((strlen("程序") + 1) * sizeof(char)); //定义语法树的根结点nodes[0]
	strcpy(T.nodes[0].data, "程序");
	T.nodes[0].indent = 0;
	T.nodes[0].FirstChild = nullptr;
	InsertChild(T, T.r, 1, c); //将c子树插入到语法树中
	return OK;
}


status ExternalDefList(FILE* fp, CTree& T) //语法单位<外部定义序列>的子程序
{
	CTree c; 
	status flag;//查看是否建立第二棵子树
	if (w == EOF)
        return INFEASIBLE;
	T.n = 1;  T.r = 0;
	T.nodes[0].data = (char*)malloc((strlen("外部定义序列") + 1) * sizeof(char)); //创建外部定义序列树
	strcpy(T.nodes[0].data, "外部定义序列");
	T.nodes[0].indent = 0;
	T.nodes[0].FirstChild = nullptr;
	if (!ExternalDef(fp, c))
        return ERROR;
	InsertChild(T, T.r, 1, c); //处理一个外部定义，得到一棵子树，作为根的第一棵子树
	flag = ExternalDefList(fp, c);
	if (flag == OK)
        InsertChild(T, T.r, 2, c); //得到的子树，作为根的第二棵子树
	if (flag==ERROR)
        return ERROR;
	return OK;
}


status ExternalDef(FILE* fp, CTree& T)  //语法单位<外部定义>的子程序
{
	status flag;
	if (w != INT && w != LONG && w != SHORT && w != SIGNED && w != UNSIGNED &&w != FLOAT && w != DOUBLE && w != CHAR &&w!=VOID)
    {
        printf("外部定义错误\n");
        return ERROR;
    }
	strcpy(kind, token_text);			//保存类型关键字
	w = GetToken(fp);
	if (w != IDENT)
    {
        printf("外部变量命名错误\n");
        return ERROR;
    }
//        return ERROR;
	strcpy(tokenText0, token_text);		//保存第一个变量名或函数名到tokenText0
	w = GetToken(fp);
	if (w != LS)
        flag = ExternalVariableDef(fp, T);	//调用外部变量定义子程序
	else
        flag = Function(fp, T);			//调用函数定义子程序
    if (!flag)
        return ERROR;
	return OK;
}


status ExternalVariableDef(FILE* fp, CTree& T)  //语法单位<外部变量定义>子程序
{
	CTree c; CTree p;
	T.n = 1; T.r = 0;
	T.nodes[0].data = (char*)malloc((strlen("外部变量定义：") + 1) * sizeof(char)); //生成外部变量定义结点
	strcpy(T.nodes[0].data, "外部变量定义：");
	T.nodes[0].indent = 1;
	T.nodes[0].FirstChild = nullptr;
	c.n = 1; c.r = 0;   //生成外部变量类型结点
	c.nodes[0].data = (char*)malloc((strlen(kind) + strlen("类型：") + 1) * sizeof(char));
	strcpy(c.nodes[0].data, "类型：");
	strcat(c.nodes[0].data, kind);
	c.nodes[0].indent = 1;
	c.nodes[0].FirstChild = nullptr;
	if (!InsertChild(T, T.r, 1, c)) //c作为T的第一个孩子
        return ERROR;
	if (!VariableList(fp, p))
        return ERROR;
	if (!InsertChild(T, T.r, 2, p)) //p作为T的第二个孩子
        return ERROR;
	return OK;
}


status VariableList(FILE* fp, CTree& T)  //语法单位<变量序列>子程序
{
	CTree c; CTree t; //c树用来构建此变量结点，t树用来构建可能存在的下一变量结点
	T.n = 1; T.r = 0;//生成变量序列结点
	T.nodes[0].data = (char*)malloc((strlen("变量序列") + 1) * sizeof(char));
	strcpy(T.nodes[0].data, "变量序列");
	T.nodes[0].indent = 0;
	T.nodes[0].FirstChild = nullptr;
	if (w == LM)     //区分是数组变量还是变量
	{
		strcat(tokenText0, "[");  //识别数组，只支持一维数组
		if ((w = GetToken(fp)) == INT_CONST)
		{
			strcat(tokenText0, token_text);
			if ((w = GetToken(fp)) == RM)
			{
				strcat(tokenText0, "]");  //识别数组成功
				c.n = 1; c.r = 0;   //创建数组结点
				c.nodes[0].data = (char*)malloc((strlen(tokenText0) + strlen("Array: ") + 1) * sizeof(char));
				strcpy(c.nodes[0].data, "Array: ");
				strcat(c.nodes[0].data, tokenText0); // 初始时，tokenText0保存了第一个变量名
				c.nodes[0].indent = 1;
				c.nodes[0].FirstChild = nullptr;
				w = GetToken(fp);
			}
			else
            {
                printf("数组定义错误，缺少']'\n");
                return ERROR;
            }
//                return ERROR;
		}
		else
        {
            printf("数组定义错误，缺少数组大小\n");
            return ERROR;
        }
//            return ERROR;
	}
	else
	{
		c.n = 1; c.r = 0;  //创建变量结点
		c.nodes[0].data = (char*)malloc((strlen(tokenText0) + strlen("ID: ") + 1) * sizeof(char));
		strcpy(c.nodes[0].data, "ID: ");
		strcat(c.nodes[0].data, tokenText0);
		c.nodes[0].indent = 1;
		c.nodes[0].FirstChild = nullptr;
	}
	if (!InsertChild(T, T.r, 1, c))	//识别的变量结点作为T的第一个孩子
        return ERROR;
	if (w != COMMA && w != SEMI)
    {
        printf("变量定义错误，缺少','或';'\n");
        return ERROR;
    }
//        return ERROR;
	if (w == SEMI)//如果标识符后是分号，直接结束
	{
		w = GetToken(fp);
		return OK;
	}
	w = GetToken(fp);
	if (w != IDENT)
        return ERROR;	//如果w不是标识符则报错，反之后面还有第二个变量
	strcpy(tokenText0, token_text);
	w = GetToken(fp);
	if (!VariableList(fp, t))
        return ERROR;
	if (!InsertChild(T, T.r, 2, t))
        return ERROR;
	return OK;
}


status Function(FILE* fp, CTree& T)  //语法单位<函数定义>子程序
{
	CTree c; CTree p; //c结点为函数返回值类型，p结点为函数名结点
	CTree q; CTree f; CTree s; //q结点为参数序列结点,f为函数体结点，s为可能的复合语句结点
	T.n = 1; T.r = 0;		//生成函数定义结点T
	T.nodes[0].data = (char*)malloc((strlen("函数定义：") + 1) * sizeof(char));
	strcpy(T.nodes[0].data, "函数定义：");
	T.nodes[0].indent = 1;
	T.nodes[0].FirstChild = nullptr;
	c.n = 1; c.r = 0;  //生成函数返回值类型结点c
	c.nodes[0].data = (char*)malloc((strlen(kind) + strlen("类型：") + 1) * sizeof(char));
	strcpy(c.nodes[0].data, "类型：");
	strcat(c.nodes[0].data, kind);
	c.nodes[0].indent = 1;
	c.nodes[0].FirstChild = nullptr;
	if (!InsertChild(T, T.r, 1, c))
        return ERROR;
	w = GetToken(fp);//函数括号内可能无参数，可能是void，可能是参数序列，其他情况报错
	if (w != RS && w != VOID && w != INT && w != LONG && w != SHORT && w != SIGNED && w != UNSIGNED && w != FLOAT && w != DOUBLE && w != CHAR)
    {
        printf("函数定义错误，参数序列错误！\n");
        return ERROR;
    }
//		return ERROR;
	p.n = 1; p.r = 0;		//生成函数名结点
	p.nodes[0].data = (char*)malloc((strlen(tokenText0) + strlen("函数名：") + 1) * sizeof(char));
	strcpy(p.nodes[0].data, "函数名：");
	strcat(p.nodes[0].data, tokenText0);
	p.nodes[0].indent = 1;
	p.nodes[0].FirstChild = nullptr;
	if (w == RS || w == VOID) //判断void情况或参数序列情况
	{
		if (w == VOID)
		{
			w = GetToken(fp);
			if (w != RS)
            {
                printf("函数定义错误，返回值与变量不匹配！\n");
                return ERROR;
            }
//                return ERROR;
        }
	}
	else
	{
		if (!ParameterList(fp, q))
            return ERROR;
		if (!InsertChild(p, p.r, 1, q))
            return ERROR;
	}
	if (!InsertChild(T, T.r, 2, p))
        return ERROR;
	w = GetToken(fp);
	if (w != SEMI && w != LL)
    {
        printf("函数定义错误，缺少';'或'{'\n");
        return ERROR;
    }
//        return ERROR;
	f.n = 1; f.r = 0;  //生成函数体结点
	f.nodes[0].data = (char*)malloc((strlen("函数体：") + 1) * sizeof(char));
	strcpy(f.nodes[0].data, "函数体：");
	f.nodes[0].indent = 1;
	f.nodes[0].FirstChild = nullptr;
	if (w == LL)  //存在函数体则判断复合语句
	{
		if (!ComplexStat(fp, s))
            return ERROR;
		if (!InsertChild(f, f.r, 1, s))
            return ERROR;
		if(!InsertChild(T, T.r, 3, f))
            return ERROR;
	}
	return OK;//若是函数声明则直接返回OK
}


status ParameterList(FILE* fp, CTree& T)  //语法单位<形参序列>子程序
{
	CTree c; //c用于生成形参子树
	CTree p; //p用于生成可能出现的下一个形参序列
	T.n = 1; T.r = 0;  //生成形参序列结点
	T.nodes[0].data = (char*)malloc((strlen("形参序列") + 1) * sizeof(char));
	strcpy(T.nodes[0].data, "形参序列");
	T.nodes[0].indent = 0;
	T.nodes[0].FirstChild = nullptr;
	if (!FormParameterDef(fp, c))
        return ERROR;
	if (!InsertChild(T, T.r, 1, c))
        return ERROR;
	w = GetToken(fp);
	if (w != RS && w != COMMA)
    {
        printf("形参序列错误，形参定义未闭合，缺少')'或','\n");
        return ERROR;
    }
//        return ERROR;
	if (w == COMMA)
	{
		w = GetToken(fp);
		if (!ParameterList(fp, p))
            return ERROR;
		InsertChild(T, T.r, 2, p);
	}
	return OK;
}


status FormParameterDef(FILE* fp, CTree& T)  //语法单位<形参>子程序
{
//w此前已读取第一个形参类型
	CTree c; //用于生成形参类型结点
	CTree p;  //用于生成形参变量结点
	T.n = 1; T.r = 0;	//生成形参结点
	T.nodes[0].data = (char*)malloc((strlen("形参：") + 1) * sizeof(char));
	strcpy(T.nodes[0].data, "形参：");
	T.nodes[0].indent = 1;
	T.nodes[0].FirstChild = nullptr;
	if (w != INT && w != LONG && w != SHORT && w != SIGNED && w != UNSIGNED &&w != FLOAT && w != DOUBLE && w != CHAR)
    {
        printf("形参定义错误，类型说明符错误\n");
        return ERROR;
    }
//        return ERROR;
	c.n = 1; c.r = 0;		//生成形参类型结点
	c.nodes[0].data = (char*)malloc((strlen(token_text) + strlen("类型：") + 1) * sizeof(char));
	strcpy(c.nodes[0].data, "类型：");
	strcat(c.nodes[0].data, token_text);
	c.nodes[0].indent = 1;
	c.nodes[0].FirstChild = nullptr;
	InsertChild(T, T.r, 1, c);
	w = GetToken(fp);
	if (w != IDENT)
    {
        printf("形参命名错误\n");
        return ERROR;
    }
//        return ERROR;
	p.n = 1; p.r = 0;   //生成形参变量结点
	p.nodes[0].data = (char*)malloc((strlen(token_text) + strlen("ID: ") + 1) * sizeof(char));
	strcpy(p.nodes[0].data, "ID: ");
	strcat(p.nodes[0].data, token_text);
	p.nodes[0].indent = 1;
	p.nodes[0].FirstChild = nullptr;
	InsertChild(T, T.r, 2, p);
	return OK;
}


status ComplexStat(FILE* fp, CTree& T)  //语法单位<复合语句>子程序
{
	CTree c; CTree p; //c用于生成局部变量定义子树，p用于生成语句序列子树
	status flag;
	print elem{};
	T.n = 1; T.r = 0;		//生成复合语句结点
	T.nodes[0].data = (char*)malloc((strlen("复合语句：") + 1) * sizeof(char));
	strcpy(T.nodes[0].data, "复合语句：");
	T.nodes[0].indent = 1;
	T.nodes[0].FirstChild = nullptr;//注意其中局部变量说明和语句序列都可以为空
	w = GetToken(fp);
	elem = { ++indent0,line_num };
	printList.push(elem);  //添加缩进值
	if (w == INT || w == LONG || w == SHORT || w == SIGNED || w == UNSIGNED|| w == FLOAT || w == DOUBLE || w == CHAR)
	{
		if (!LocalVariableList(fp, c))
            return ERROR;
		if (!InsertChild(T, T.r, 1, c))
            return ERROR;
		flag = StatementList(fp, p);
		if (!flag)
            return ERROR;
		if (flag == OK)
			if (!InsertChild(T, T.r, 2, p))
                return ERROR;
	}
	else
	{
		flag = StatementList(fp, p);
		if (!flag)
            return ERROR;
		if (flag == OK && !InsertChild(T, T.r, 1, p))
            return ERROR;
	}
	elem = { --indent0,line_num };
	printList.push(elem);

//    w = GetToken(fp);

	if (w != RL)
    {
        printf("复合语句错误，缺少'}'\n");
        return ERROR;
    }
//        return ERROR;
	w = GetToken(fp);
	return OK;
}

status LocalVariableList(FILE* fp, CTree& T)  //语法单位<局部变量定义序列>子程序
{
	CTree c; CTree p;//c生成局部变量定义子树，p生成可能存在的下一个局部变量定义序列子树
	status flag;
	if (w != INT && w != LONG && w != SHORT && w != SIGNED && w != UNSIGNED && w != FLOAT && w != DOUBLE && w != CHAR)
		return INFEASIBLE;
	//读到的后继单词不为类型说明符时，变量定义序列结束
	T.n = 1;  T.r = 0;  //生成局部变量定义序列结点
	T.nodes[0].data = (char*)malloc((strlen("局部变量定义序列") + 1) * sizeof(char));
	strcpy(T.nodes[0].data, "局部变量定义序列");
	T.nodes[0].indent = 0;
	T.nodes[0].FirstChild = nullptr;
	if (!LocalVariableDef(fp, c))
        return ERROR;
	if (!InsertChild(T, T.r, 1, c))
        return ERROR;
	flag = LocalVariableList(fp, p);
	if (flag == OK)
        InsertChild(T, T.r, 2, p);
	if (!flag)
        return ERROR;
	return OK;
}

status LocalVariableDef(FILE* fp, CTree& T)//语法单位<局部变量定义>子程序
{
	CTree c;  CTree p; //c生成局部变量类型结点,p生成变量序列子树
	if (w != INT && w != LONG && w != SHORT && w != SIGNED && w != UNSIGNED && w != FLOAT && w != DOUBLE && w != CHAR)
    {
        printf("局部变量定义错误\n");
        return ERROR;
    }
//        return ERROR;
	T.n = 1; T.r = 0;		//生成局部变量定义结点
	T.nodes[0].data = (char*)malloc((strlen("局部变量定义：") + 1) * sizeof(char));
	strcpy(T.nodes[0].data, "局部变量定义：");
	T.nodes[0].indent = 1;
	T.nodes[0].FirstChild = nullptr;
	
	c.n = 1; c.r = 0;		//生成局部变量类型结点
	c.nodes[0].data = (char*)malloc((strlen(token_text) + strlen("类型：") + 1) * sizeof(char));
	strcpy(c.nodes[0].data, "类型：");
	strcat(c.nodes[0].data, token_text);
	c.nodes[0].indent = 1;
	c.nodes[0].FirstChild = nullptr;
	if (!InsertChild(T, T.r, 1, c))
        return ERROR;
	w = GetToken(fp);
	if (w != IDENT){
        printf("局部变量命名错误\n");
        return ERROR;
    }
//        return ERROR;
	strcpy(tokenText0, token_text);
	w = GetToken(fp);
	if (!VariableList(fp, p))
        return ERROR;
	if (!InsertChild(T, T.r, 2, p))
        return ERROR;
	return OK;
}

status StatementList(FILE* fp, CTree& T)  //语法单位<语句序列>子程序
{
	CTree c; CTree p;  //c生成语句树,p生成可能出现的语句序列树
	status flag;
	flag = Statement(fp, c);
	if (flag == INFEASIBLE)
        return INFEASIBLE;
	if (!flag)
        return ERROR;
	else
	{
		T.n = 1; T.r = 0;		//生成语句序列结点
		T.nodes[0].data = (char*)malloc((strlen("语句序列") + 1) * sizeof(char));
		strcpy(T.nodes[0].data, "语句序列");
		T.nodes[0].indent = 0;
		T.nodes[0].FirstChild = nullptr;
		InsertChild(T, T.r, 1, c);
		flag = StatementList(fp, p);
		if (!flag)
            return ERROR;
		if (flag == OK && !InsertChild(T, T.r, 2, p))
            return ERROR;
	}
	return OK;
}

status Statement(FILE* fp, CTree& T)  //语法单位<语句>子程序
{
	CTree c; CTree p; CTree q; CTree k;
	print elem{};
	
	if (w == IF) //分析条件语句,p用于生成表达式树,q用于生成if模块子句数，k用于生成else模块子句数
    {
		w = GetToken(fp);
		if (w != LS)
        {
            printf("if语句错误,'('缺失！\n");
            return ERROR;
        }
//            return ERROR;
		w = GetToken(fp);
		if (w == RS)
        {
            printf("if语句错误,条件缺失！\n");
            return ERROR;
        }
//            return ERROR;
		if (!Expression(fp, p, RS))
            return ERROR;
		c.n = 1; c.r = 0;  //生成if语句子树
		c.nodes[0].data = (char*)malloc((strlen("条件：") + 1) * sizeof(char));
		strcpy(c.nodes[0].data, "条件：");
		c.nodes[0].indent = 1;
		c.nodes[0].FirstChild = nullptr;
		InsertChild(c, c.r, 1, p);
		w = GetToken(fp);
		if (w == LL)
            if (!ComplexStat(fp, q))
                return ERROR;
        else
		{
			elem = { ++indent0,line_num } ;
			printList.push(elem);
			if (!Statement(fp, q))
                return ERROR;
			elem = { --indent0,line_num };
			printList.push(elem);
		}
		p.n = 1; p.r = 0;
		p.nodes[0].data = (char*)malloc((strlen("IF子句：") + 1) * sizeof(char));
		strcpy(p.nodes[0].data, "IF子句：");
		p.nodes[0].indent = 1;
		p.nodes[0].FirstChild = nullptr;
		InsertChild(p, p.r, 1, q);
		if (w == ELSE)
		{
            printf("else\n");
			w = GetToken(fp);
			if (w == LL)
				if (!ComplexStat(fp, k))
                    return ERROR;
			else
			{
				elem = { ++indent0,line_num };
				printList.push(elem);
				if (!Statement(fp, k))
                    return ERROR;
				elem = { --indent0,line_num };
				printList.push(elem);
			}
			q.n = 1; q.r = 0;  //生成else语句子树
			q.nodes[0].data = (char*)malloc((strlen("ELSE子句：") + 1) * sizeof(char));
			strcpy(q.nodes[0].data, "ELSE子句：");
			q.nodes[0].indent = 1;
			q.nodes[0].FirstChild = nullptr;
			InsertChild(q, q.r, 1, k);
			T.n = 1; T.r = 0;
			T.nodes[0].data = (char*)malloc((strlen("if-else语句：") + 1) * sizeof(char));
			strcpy(T.nodes[0].data, "if-else语句：");
			T.nodes[0].indent = 1;
			T.nodes[0].FirstChild = nullptr;
			InsertChild(T, T.r, 1, c);
			InsertChild(T, T.r, 2, p);
			InsertChild(T, T.r, 3, q);
		}
		else
		{
			T.n = 1; T.r = 0;
			T.nodes[0].data = (char*)malloc((strlen("if语句：") + 1) * sizeof(char));
			strcpy(T.nodes[0].data, "if语句：");
			T.nodes[0].indent = 1;
			T.nodes[0].FirstChild = nullptr;
			InsertChild(T, T.r, 1, c);
			InsertChild(T, T.r, 2, p);
		}
		return OK;
	}
	else if (w == LL)
    {
		if (!ComplexStat(fp, T))
            return ERROR;
		return OK;
	}
	else if (w == FOR) //分析for语句
    {
		T.n = 1; T.r = 0;
		T.nodes[0].data = (char*)malloc((strlen("for语句：") + 1) * sizeof(char));
		strcpy(T.nodes[0].data, "for语句：");
		T.nodes[0].indent = 1;
		T.nodes[0].FirstChild = nullptr;
		c.n = 1; c.r = 0;
		c.nodes[0].data = (char*)malloc((strlen("初始表达式：") + 1) * sizeof(char));
		strcpy(c.nodes[0].data, "初始表达式：");
		c.nodes[0].indent = 1;
		c.nodes[0].FirstChild = nullptr;
		InsertChild(T, T.r, 1, c);
		c.n = 1; c.r = 0;
		c.nodes[0].data = (char*)malloc((strlen("终止条件：") + 1) * sizeof(char));
		strcpy(c.nodes[0].data, "终止条件：");
		c.nodes[0].indent = 1;
		c.nodes[0].FirstChild = nullptr;
		InsertChild(T, T.r, 2, c);
		c.n = 1; c.r = 0;
		c.nodes[0].data = (char*)malloc((strlen("循环表达式：") + 1) * sizeof(char));
		strcpy(c.nodes[0].data, "循环表达式：");
		c.nodes[0].indent = 1;
		c.nodes[0].FirstChild = nullptr;
		InsertChild(T, T.r, 3, c);
		c.n = 1; c.r = 0;
		c.nodes[0].data = (char*)malloc((strlen("for子句：") + 1) * sizeof(char));
		strcpy(c.nodes[0].data, "for子句：");
		c.nodes[0].indent = 1;
		c.nodes[0].FirstChild = nullptr;
		InsertChild(T, T.r, 4, c);
		w = GetToken(fp);
		if (w != LS)
            return ERROR;
		w = GetToken(fp);
		if (!Expression(fp, c, SEMI))
            return ERROR;
		InsertChild(T, T.nodes[0].FirstChild->child, 1, c);
		w = GetToken(fp);
		if (w == SEMI)
            return ERROR;
		if (!Expression(fp, c, SEMI))
            return ERROR;
		InsertChild(T, T.nodes[0].FirstChild->next->child, 1, c);
		w = GetToken(fp);
		if (!Expression(fp, c, RS))
            return ERROR;
		InsertChild(T, T.nodes[0].FirstChild->next->next->child, 1, c);
		w = GetToken(fp);
		if (w == LL)
			if (!ComplexStat(fp, c))
                return ERROR;

		else
		{
			elem = { ++indent0,line_num };
			printList.push(elem);
			if (!Statement(fp, c))
                return ERROR;
			elem = { --indent0,line_num };
			printList.push(elem);
		}
		InsertChild(T, T.nodes[0].FirstChild->next->next->next->child, 1, c);
		return OK;
	}
	else if (w == WHILE)
    {
		w = GetToken(fp);
		if (w != LS)
            return ERROR;
		w = GetToken(fp);
		if (w == RS)
            return ERROR;
		if (!Expression(fp, c, RS))
            return ERROR;
		w = GetToken(fp);
		if (w == LL)
			if (!ComplexStat(fp, p))
                return ERROR;

		else
		{
			elem = { ++indent0,line_num };
			printList.push(elem);
			if (!Statement(fp, p))
                return ERROR;
			elem = { --indent0,line_num };
			printList.push(elem);
		}
		T.n = 1; T.r = 0;
		T.nodes[0].data = (char*)malloc((strlen("while语句：") + 1) * sizeof(char));
		strcpy(T.nodes[0].data, "while语句：");
		T.nodes[0].indent = 1;
		T.nodes[0].FirstChild = nullptr;
		InsertChild(T, T.r, 1, c);
		InsertChild(T, T.r, 2, p);
		return OK;
	}
	else if (w == CONTINUE)
    {
		T.n = 1; T.r = 0;
		T.nodes[0].data = (char*)malloc((strlen("continue语句：") + 1) * sizeof(char));
		strcpy(T.nodes[0].data, "continue语句：");
		T.nodes[0].indent = 1;
		T.nodes[0].FirstChild = nullptr;
		w = GetToken(fp);
		if (w != SEMI)
        {
            printf("continue语句错误，缺少';'\n");
            return ERROR;
        }
//            return ERROR;
		w = GetToken(fp);
		return OK;
	}
	else if (w == BREAK)
    {
		T.n = 1; T.r = 0;
		T.nodes[0].data = (char*)malloc((strlen("break语句：") + 1) * sizeof(char));
		strcpy(T.nodes[0].data, "break语句：");
		T.nodes[0].indent = 1;
		T.nodes[0].FirstChild = nullptr;
		w = GetToken(fp);
		if (w != SEMI)
        {
            printf("break语句错误，缺少';'\n");
            return ERROR;
        }
//            return ERROR;
		w = GetToken(fp);
		return OK;
	}
	else if (w == RETURN)
    {
		T.n = 1; T.r = 0;
		T.nodes[0].data = (char*)malloc((strlen("return语句：") + 1) * sizeof(char));
		strcpy(T.nodes[0].data, "return语句：");
		T.nodes[0].indent = 1;
		T.nodes[0].FirstChild = nullptr;
		w = GetToken(fp);
		if (w == SEMI)
        {
            printf("return语句错误，缺少表达式\n");
            return ERROR;
        }
//            return ERROR;
		if (!Expression(fp, c, SEMI))
            return ERROR;
		w = GetToken(fp);
		InsertChild(T, T.r, 1, c);
		return OK;
	}
	else if (w == LS)
    {
		if (!Expression(fp, T, RS))
            return ERROR;
		w = GetToken(fp);
		return OK;
	}
	else if (w == IDENT || w == INT_CONST || w == UNSIGNED_CONST || w == LONG_CONST || w == UNSIGNED_LONG_CONST || w == DOUBLE_CONST || w == FLOAT_CONST || w == LONG_DOUBLE_CONST || w == CHAR_CONST) {
		if (!Expression(fp, T, SEMI))
            return ERROR;
		w = GetToken(fp);
		return OK;
	}
	else if (w == RL)
		return INFEASIBLE;

	else if (w == SEMI)
    {
		T.n = 1; T.r = 0;
		T.nodes[0].data = (char*)malloc((strlen("空语句") + 1) * sizeof(char));
		strcpy(T.nodes[0].data, "空语句");
		T.nodes[0].indent = 1;
		T.nodes[0].FirstChild = nullptr;
		w = GetToken(fp);
		return OK;
	}
	else
    {
        printf("语句错误，缺少尾部元素！\n");
        return ERROR;
    }
//        return ERROR;

}

status Expression(FILE* fp, CTree& T, int EndSym)//语法单位<表达式>子程序
{
	//已经读入了第一个单词在w中
	SqStack op;		//运算符栈
	SqStack opn;	//操作数栈
	auto* node = (CTree*)malloc(sizeof(CTree));
	auto* child1 = (CTree*)malloc(sizeof(CTree));
	auto* child2 = (CTree*)malloc(sizeof(CTree));
	T.n = 1; T.r = 0;  //生成表达式结点
	T.nodes[0].data = (char*)malloc((strlen("表达式语句：") + 1) * sizeof(char));
	strcpy(T.nodes[0].data, "表达式语句：");
	T.nodes[0].indent = 1;
	T.nodes[0].FirstChild = nullptr;
	int error = 0;
	node->n = 1; node->r = 0; //设立起止符号
	node->nodes[0].data = (char*)malloc((strlen("#") + 1) * sizeof(char));
	strcpy(node->nodes[0].data, "#");
	node->nodes[0].FirstChild = nullptr;
	InitStack(opn);
    InitStack(op);
    Push(op, node);		//初始化，将起止符#入栈
	while ((w != POUND || strcmp(node->nodes[0].data, "#") != 0) && !error)  //当运算符栈栈顶不是起止符号，并没有错误时
	{
		if (w == IDENT || w == INT_CONST || w == UNSIGNED_CONST || w == LONG_CONST
			|| w == UNSIGNED_LONG_CONST || w == DOUBLE_CONST || w == FLOAT_CONST
			|| w == LONG_DOUBLE_CONST || w == CHAR_CONST)
		{
			node = (CTree*)malloc(sizeof(CTree));
			node->n = 1; node->r = 0;
			node->nodes[0].data = (char*)malloc((strlen(token_text) + strlen("ID: ") + 1) * sizeof(char));
			strcpy(node->nodes[0].data, "ID: ");
			strcat(node->nodes[0].data, token_text);
			node->nodes[0].indent = 1;
			node->nodes[0].FirstChild = nullptr;
			Push(opn, node);			//根据w生成一个结点，结点指针进栈opn
			w = GetToken(fp);
		}
		else if (w == PLUS || w == MINUS || w == MULTIPLY || w == DIVIDE ||
			w == MOD || w == LS || w == RS || ((w>=MORE)&&(w<= LESS_EQUAL)) || w == EQUAL_TO ||
			w == AND || w == OR || w == POUND)
		{
			node = (CTree*)malloc(sizeof(CTree));
			GetTop(op, node);
			if (w == POUND)
                strcpy(token_text, "#");
			switch (Precedence(node->nodes[0].data, token_text))
			{
                case '<':
                    node = (CTree*)malloc(sizeof(CTree));
                    node->nodes[0].data = (char*)malloc((strlen(token_text) + 1) * sizeof(char));
                    strcpy(node->nodes[0].data, token_text);
                    node->nodes[0].indent = 1;
                    node->nodes[0].FirstChild = nullptr;
                    node->n = 1; node->r = 0;
                    Push(op, node);		//根据w生成一个结点，结点指针进栈opn
                    w = GetToken(fp);
                    break;
                case '=':
                    if (!Pop(op, node))
                        error++;
                    w = GetToken(fp);
                    break;   //去括号
                case '>':
                    if (!Pop(opn, child2))
                        error++;
                    if (!Pop(opn, child1))
                        error++;
                    if (!Pop(op, node))
                        error++;
                    //根据运算符栈退栈得到的运算符node和操作数的结点指针child1和child2，
                    //完成建立生成一个运算符的结点，结点指针进栈opn
                    InsertChild(*node, node->r, 1, *child1);
                    InsertChild(*node, node->r, 2, *child2);
                    Push(opn, node);
                    break;
                default:
                    if (w == EndSym)
                        w = POUND; //遇到结束标记），w被替换成#
                    else
                        error++;
			}
		}
		else if (w == EndSym)
            w = POUND;//遇到结束标记分号，w被替换成#
		else
            error = 1;
		GetTop(op, node);
	}
	if (error)
    {
        printf("表达式错误\n");
        return ERROR;
    }
//        return ERROR;
	GetTop(opn, node);
	InsertChild(T, T.r, 1, *node);
	return OK;
}

char Precedence(const char* a, const char* b)
{
	int i, j;		//指示运算符对应的编号
	//定义一个二维数组，用于存放优先级
	char Precedence[13][13] =
	{   //				 +		 -		 *		 /		 %		（		 ）	  	 =		>和<	  ==和!=		 #		&&		||
		/* + */			'>',	'>',	'<',	'<',	'<',	'<',	'>',	'?',	'>',		'>',		'>',	'>',	'>',
		/* - */			'>',	'>',	'<',	'<',	'<',	'<',	'>',	'?',	'>',		'>',		'>',	'>',	'>',
		/* * */			'>',	'>',	'>',	'>',	'>',	'<',	'>',	'?',	'>',		'>',		'>',	'>',	'>',
		/* / */			'>',	'>',	'>',	'>',	'>',	'<',	'>',	'?',	'>',		'>',		'>',	'>',	'>',
		/* % */			'>',	'>',	'<',	'<',	'<',	'<',	'>',	'?',	'>',		'>',		'>',	'>',	'>',
		/* ( */			'<',	'<',	'<',	'<',	'<',	'<',	'=',	'?',	'<',		'<',		'>',	'<',	'<',
		/* ) */			'>',	'>',	'>',	'>',	'>',	'>',	'?',	'?',	'>',		'>',		'>',	'>',	'>',
		/* = */			'<',	'<',	'<',	'<',	'<',	'<',	'?',	'<',	'<',		'<',		'>',	'<',	'<',
		/* >和< */		'<',	'<',	'<',	'<',	'<',	'<',	'>',	'?',	'>',		'>',		'>',	'>',	'>',
		/* ==和!= */	'<',	'<',	'<',	'<',	'<',	'<',	'>',	'?',	'<',		'>',		'>',	'>',	'>',
		/* # */			'<',	'<',	'<',	'<',	'<',	'<',	'?',	'<',	'<',		'<',		'=',	'<',	'<',
		/* && */		'<',	'<',	'<',	'<',	'<',	'<',	'>',	'>',	'<',		'<',		'>',	'>',	'>',
		/* || */		'<',	'<',	'<',	'<',	'<',	'<',	'>',	'>',	'<',		'<',		'>',	'>',	'>'
	};
	switch (a[0])
	{
        case '+':
            i = 0;
            break;
        case '-':
            i = 1;
            break;
        case '*':
            i = 2;
            break;
        case '/':
            i = 3;
            break;
        case '%':
            i = 4;
            break;
        case '(':
            i = 5;
            break;
        case ')':
            i = 6;
            break;
        case '=':
            if (a[1] == '=')
                i = 9;
            else
                i = 7;
            break;
        case '>':
        case '<':
            i = 8;
            break;
        case '!':
            if (a[1] == '=')
                i = 9;
            else
                return '?';
            break;
        case '#':
            i = 10;
            break;
        case '&':
            if (a[1] == '&')
                i = 11;
            else
                return '?';
            break;
        case '|':
            if (a[1] == '|')
                i = 12;
            else
                return '?';
            break;
        default:
            return '?';
        }
        switch (b[0])
        {
        case '+':
            j = 0;
            break;
        case '-':
            j = 1;
            break;
        case '*':
            j = 2;
            break;
        case '/':
            j = 3;
            break;
        case '%':
            j = 4;
            break;
        case '(':
            j = 5;
            break;
        case ')':
            j = 6;
            break;
        case '=':
            if (b[1] == '=')
                j = 9;
            else
                j = 7;
            break;
        case '>':
        case '<':
            j = 8;
            break;
        case '!':
            if (b[1] == '=')
                j = 9;
            else
                return '?';
            break;
        case '#':
            j = 10;
            break;
        case '&':
            if (b[1] == '&')
                j = 11;
            else
                return '?';
            break;
        case '|':
            if (b[1] == '|')
                j = 12;
            else
                return '?';
            break;
        default:
            return '?';
	}
	return Precedence[i][j];
}

status PrintTree(char* data, int indent) //打印函数
{
	int i;
	for (i = 0; i < indent-1; i++)
		printf("\t");
	printf("%s\n", data);
	return OK;
}