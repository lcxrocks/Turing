#include<stdio.h>
#include<cstdarg>
#include<stdlib.h>
#include<unistd.h>
#include<getopt.h>
#include<iostream>
#include<fstream>
#include<string.h>
#include<vector>
#include<set>

#define print(...) \
if(v_flag) \
	printf(__VA_ARGS__) 

#define err(...) \
if(v_flag) \
	fprintf(stderr, __VA_ARGS__) \

#define panic_on(cond, s) \
  do { \
    if (cond) { \
      printf("%s", s); \
      printf(" Line:  %d\n", __LINE__); \
      exit(0); \
    } \
  } while (0)

#define INPUT_HDR  6 // #x = {
#define INPUT_TAIL 1 // }

using namespace std;

static const char* short_opts = "hv";
static const struct option long_opts[]={
	{"help", no_argument, NULL, 'h'},
	{"verbose", no_argument, NULL, 'v'},
	{NULL,0,NULL,0}
};

struct transition
{
	string old_s;
	string old_char;
	string new_char;
	string direction;
	string new_s;
};

struct Tape
{
	int ptr;
	int start_index;
	int end_index;
	vector<char> w;
	vector<int>  index;
};

set<string> Q;
set<char> S;
set<char> G;
string q0;
char B;
set<string> F;
int N;
vector<transition> delta;
vector<Tape> tapes;


string& trim(string &s) {
    if (s.empty()){
        return s;
    }
    s.erase(0,s.find_first_not_of(" ")); // comment in sentence
    if(s.find_first_of(";")!=0)
    	s = s.substr(0, s.find_first_of(";"));
    s.erase(s.find_last_not_of(" ") + 1);
    return s;
}

void check_format(string str){
	if(str[0]!='#') {
		fprintf(stderr, "[exitcode:14] syntax error --> Illegal definition format. Check your <tm> file.\n");
		exit(EXIT_FAILURE);
	}
	else if(str[2]!=' ') {
		fprintf(stderr, "[exitcode:14] syntax error --> Illegal definition format. Check your <tm> file.\n");
		exit(EXIT_FAILURE);
	}
	else if(str[3]!='=') {
		fprintf(stderr, "[exitcode:14] syntax error --> Illegal definition format. Check your <tm> file.\n");
		exit(EXIT_FAILURE);
	}
	else if(str[4]!=' ') {
		fprintf(stderr, "[exitcode:14] syntax error --> Illegal definition format. Check your <tm> file.\n");
		exit(EXIT_FAILURE);
	}
	else if(str[5]!='{') {
		fprintf(stderr, "[exitcode:14] syntax error --> Illegal definition format. Check your <tm> file.\n");
		exit(EXIT_FAILURE);
	}
	else if(str[str.size()-1]!='}') {
		fprintf(stderr, "[exitcode:14] syntax error --> Illegal definition format. Check your <tm> file.\n");
		exit(EXIT_FAILURE);
	}
	
}

void check_short_format(string str, int len){
	if(str[0]!='#'){
		fprintf(stderr, "[exitcode:14] syntax error --> Illegal definition format. Check your <tm> file.\n");
		exit(EXIT_FAILURE);
	}
	else if(str[len+1]!=' '){
		fprintf(stderr, "[exitcode:14] syntax error --> Illegal definition format. Check your <tm> file.\n");
		exit(EXIT_FAILURE);
	}
	else if(str[len+2]!='='){
		fprintf(stderr, "[exitcode:14] syntax error --> Illegal definition format. Check your <tm> file.\n");
		exit(EXIT_FAILURE);
	}
	else if(str[len+3]!=' '){
		fprintf(stderr, "[exitcode:14] syntax error --> Illegal definition format. Check your <tm> file.\n");
		exit(EXIT_FAILURE);
	}
}

void split_string(const string& s, vector<string>& v, const string& c){
	string::size_type pos1, pos2;
	pos2 = s.find(c);
	pos1 = 0;
	while(std::string::npos != pos2){
		v.push_back(s.substr(pos1, pos2-pos1));
		pos1 = pos2 + c.size();
    	pos2 = s.find(c, pos1);
  	}
  	if(pos1 != s.length())
   		v.push_back(s.substr(pos1));
}

void get_defs(string str, vector<string> &defs){
	defs.clear();
	split_string(str.substr(INPUT_HDR, str.size()-INPUT_HDR-INPUT_TAIL), defs, ",");
}

void get_delta(string str, vector<string> &defs){
	defs.clear();
	split_string(str, defs, " ");
}

int parse(char *filename, bool verbose_flag){
	ifstream src_file;
	string str;
	vector<string> defs;
	src_file.open(filename, ios::in);
	if(!src_file){
		fprintf(stderr, "Open file failed.\n");
		return -1;
	}
	bool Qflag = false;
	bool Sflag = false;
	bool Gflag = false;
	bool qflag = false;
	bool Fflag = false;
	bool Bflag = false;
	bool Nflag = false;
	bool transflag = false;
	while(getline(src_file, str)){
		if(trim(str)[0]==';') continue; // comments
		if(trim(str)=="") continue; // empty lines
		switch(str[1]){
			case 'Q': 
				check_format(str);
				get_defs(str, defs);
				for (int i = 0; i < defs.size(); ++i){
					Q.insert(defs[i]);
				}
				Qflag = true;
				break;
			case 'S':
				check_format(str);
				get_defs(str, defs);
				for (int i = 0; i < defs.size(); ++i){
					S.insert(defs[i][0]);
				}
				Sflag = true;
				break;
			case 'G':
				check_format(str);
				get_defs(str, defs);
				for (int i = 0; i < defs.size(); ++i){
					G.insert(defs[i][0]);
				}
				Gflag = true;
				break;
			case 'q':
				check_short_format(str, 2);
				q0  = str.erase(0,6);
				qflag = true;
				break;
			case 'B':
				check_short_format(str, 1);
				Bflag = true;
				B = str[5];
				break;
			case 'F':
				check_format(str);
				get_defs(str, defs);
				for (int i = 0; i < defs.size(); ++i){
					F.insert(defs[i]);
				}
				Fflag = true;
				break;
			case 'N':
				check_short_format(str, 1);
				N = atoi(str.erase(0,5).c_str());
				Nflag = true;
				break;
			default: // delta
				transflag = true; 
				get_delta(str, defs);
				transition d_tmp; 
				d_tmp.old_s = defs[0];
				d_tmp.old_char = defs[1];
				d_tmp.new_char = defs[2];
				d_tmp.direction = defs[3];
				d_tmp.new_s = defs[4];
				delta.push_back(d_tmp);
				break;
		}
	}
	if(Qflag  && Sflag  && Gflag  && qflag  && Fflag  && Bflag  && Nflag  && transflag )
		return 0;
	else 
		return -1;
	// cout<<"#Q = "<<endl;
	// for(set<string>::iterator it=Q.begin() ;it!=Q.end();it++)
    // {
    //     cout<<*it<<" ";
    // }
    // cout<<endl;
    // cout<<"#S = "<<endl;
	// for(set<char>::iterator it=S.begin() ;it!=S.end();it++)
    // {
    //     cout<<*it<<" ";
    // }
    // cout<<endl;
    // cout<<"#G = "<<endl;
	// for(set<char>::iterator it=G.begin() ;it!=G.end();it++)
    // {
    //     cout<<*it<<" ";
    // }
    // cout<<endl;
    // cout<<"#F = "<<endl;
	// for(set<string>::iterator it=F.begin() ;it!=F.end();it++)
    // {
    //     cout<<*it<<" ";
    // }
    // cout<<endl;
    // cout<<"#q0 = "<<q0<<endl;
    // cout<<"#B = "<<B<<endl;
    // cout<<"#N = "<<N<<endl;
    // cout<<"#delta:"<<endl;
    // for (int i = 0; i < delta.size(); ++i)
    // {
    // 	cout<<delta[i].old_s<<" "<< delta[i].old_char<<" "<< delta[i].new_char<<" "<<delta[i].direction<<" "<< delta[i].new_s<<endl;
    // }
    // cout<<endl;
}

int digit(int num){
	int cnt = 1;
	while(num/10 != 0){
		cnt++;
		num = num/10;
	}
	return cnt;
}

int main(int argc, char *argv[]){
	int opt = 0;
	int opt_cnt = 0;
	bool v_flag = false;
	while((opt = getopt_long(argc, argv, short_opts, long_opts, NULL))!=-1){
		switch(opt){
			case 'h':
				printf("Usage: turing [-v|--verbose] [-h|--help] <tm> <input>\n");
				return 0;
			case 'v':
				v_flag = true;
		}
		opt_cnt++;
	}
	int tm_idx = 1 ; //default
	int input_idx = 2; //default
	tm_idx += opt_cnt;
	input_idx += opt_cnt;

	if(input_idx+1 != argc){
		fprintf(stderr, "[exitcode:0] wrong argument number.\n");
		return 1;
	}

	char *tm_file = argv[tm_idx];
	char *input = argv[input_idx];
	int input_len = strlen(input);
	//printf("tm_file is : %s, input is %s [len: %d]\n", tm_file, input_len==0 ? "[EMPTY STRING]":input, input_len);
	int parse_status = 0; 
	parse_status = parse(tm_file, v_flag);
	for (int i = 0; i < delta.size(); i++){
		if(Q.count(delta[i].old_s)!=1 || Q.count(delta[i].new_s)!=1){
			fprintf(stderr, "[exitcode:11] syntax error --> Illegal state in transition function. Check your <tm> file.\n");
			cout<<delta[i].old_s<<" "<<delta[i].old_char<< " "<< delta[i].new_char<< " "<<delta[i].direction<<" "<<delta[i].new_s<<endl;
			exit(EXIT_FAILURE);
		}
		for (int j = 0; j < N; j++)
		{
			if(G.count(delta[i].old_char[j])!=1){
				fprintf(stderr, "[exitcode:12] syntax error --> Illegal character in transition function (old_char). Check your <tm> file.\n");
				exit(EXIT_FAILURE);
			}
			if(G.count(delta[i].new_char[j])!=1){
				fprintf(stderr, "[exitcode:13] syntax error --> Illegal character in transition function (new_char). Check your <tm> file.\n");
				exit(EXIT_FAILURE);
			}
		}
	}

	if(parse_status == -1){
		fprintf(stderr, "[exitcode:10] syntax error --> Missing <tm> components. Check your <tm> file.\n");
		exit(EXIT_FAILURE);
	}

	print("Input: %s\n", input);
	
	/* 1. 检查输入串的符号是否合法 */
	for (int i = 0; i < input_len; i++){
		if(S.count(input[i])==0){
			if(!v_flag) {
				fprintf(stderr, "[exitcode:2] illegal input.\n");
				exit(EXIT_FAILURE);
			}
			else{
				fprintf(stderr, "==================== ERR ==================== \n");
				fprintf(stderr, "error: '%c' was not declared in the set of input symbols [exitcode:2]\n", input[i]);
				fprintf(stderr, "Input: %s\n", input);
				fprintf(stderr, "       ");
				for(int j = 0; j < i; j++) fprintf(stderr, "%c", ' ');
				fprintf(stderr, "^\n");
				fprintf(stderr, "==================== END ==================== \n");
				exit(EXIT_FAILURE);
			}
		}
	}

	print("==================== RUN ==================== \n");
	
	/* 2. 进行对串的模拟 */
	// 初始化：
	int steps = 0;
	string cstate = q0; // current state
	Tape tape0;	
	tape0.ptr = 0;
	tape0.start_index = 0;
	if(input_len == 0) tape0.end_index = 0;
	for (int i = 0; i < input_len; ++i){
		tape0.w.push_back(input[i]);
		tape0.index.push_back(i);
		if(i==0) tape0.end_index=0;
		else tape0.end_index +=1;
		//printf("pushing: %c（[%d]/input_len）\n", input[i], i+1);
	}
	tape0.w.push_back(B); // 多在末尾push一个B
	tape0.index.push_back(input_len);
	tapes.push_back(tape0);
	for (int i = 0; i < N-1; i++)
	{
		Tape tmp;
		tmp.ptr = 0;
		tmp.start_index = 0;
		tmp.end_index = 0; // Blank
		tmp.w.push_back(B);
		tmp.index.push_back(0);
		tapes.push_back(tmp);
	}
	
	while(F.count(cstate)!=1){
		// 展示目前纸带状态
		print("Step   : %d\n", steps);
		for (int j = 0; j < N; j++){
			print("Index%d : ", j);
			for(int i=min(tapes[j].start_index, tapes[j].ptr); i <= max(tapes[j].end_index, tapes[j].ptr); i++){
				print("%d ", tapes[j].index[i]);
			}
			print("\n");
			print("Tape%d  : ", j);
			for(int i=min(tapes[j].start_index, tapes[j].ptr); i <= max(tapes[j].end_index, tapes[j].ptr); i++){
				print("%c", tapes[j].w[i]);	
				int b_cnt = digit(tapes[j].index[i]);
				for (int p = 0; p < b_cnt; p++){
					print("%c",' ');
				}
			}
			print("\n");
			print("Head%d  : ", j);
			for (int i = i=min(tapes[j].start_index, tapes[j].ptr); i < tapes[j].ptr; i++){
				print(" ");
				int b_cnt = digit(tapes[j].index[i]);
				for (int p = 0; p < b_cnt; p++){
					print("%c",' ');
				}
			}
			print("^\n");	
		}
		print("State  : ");
		print("%s\n", cstate.c_str());
		print("---------------------------------------------\n");
		steps ++;
		// 根据转移函数更改目前纸带状态
		int find_d = -1; // 是否找到转移函数
		for (int i = 0; i < delta.size(); i++){
			if(delta[i].old_s == cstate){
				for (int k = 0; k < N; k++){
					if(delta[i].old_char[k] == tapes[k].w[tapes[k].ptr]){
						find_d = i;
					}
					else{
						find_d = -1;
						break;
					}
				}
				if(find_d == -1) continue;
				else break;
			}
		}
		// 定位到适合的转移函数（状态和当前字符都是正确的）
		// 开始更新状态
		if(find_d == -1) {
			//err("NO transition function.\n"); 
			break;
		}
		// printf("transition function: ");
		// cout<<delta[find_d].old_s<<" "<<delta[find_d].old_char<< " "<< delta[find_d].new_char<< " "<<delta[find_d].direction<<" "<<delta[find_d].new_s<<endl;
		cstate = delta[find_d].new_s;
		for (int k = 0; k < N; k++) // 对第k条纸带做修改
		{
			// printf("current tape: ");
			// for (int i = 0; i <= tapes[k].w.size(); i++)
			// {
			// 	printf("%c ", tapes[k].w[i]);
			// }
			// printf("\n");
			if(delta[find_d].direction[k] == 'r'){
				tapes[k].w[tapes[k].ptr] = delta[find_d].new_char[k];
				//printf("TAPE[%d](%d): %c\n", k, tapes[k].ptr, tapes[k].w[tapes[k].ptr]);
				tapes[k].ptr += 1;
			}
			else if(delta[find_d].direction[k] == 'l'){
				tapes[k].w[tapes[k].ptr] = delta[find_d].new_char[k];
				tapes[k].ptr -= 1;
			}
			else{
				tapes[k].w[tapes[k].ptr] = delta[find_d].new_char[k];
			}	
		}
		// 统计B的位置
		for (int k = 0; k < N; k++)
		{
			if(tapes[k].ptr == tapes[k].w.size()){
				tapes[k].w.push_back(B);
				int last = tapes[k].index[tapes[k].index.size()-1];
				tapes[k].index.push_back(last+1);
			}

			tapes[k].end_index = tapes[k].w.size() - 1; //re-count
			tapes[k].start_index = 0;
			
			bool start_index_set = false;
			for (int i = tapes[k].start_index; i <= tapes[k].end_index; i++)
			{// 从前向后
				//printf("tapes[%d].w[%i]: %c/ _\n", k, i, tapes[k].w[i]);
				if(tapes[k].start_index == tapes[k].end_index) break;
				if(tapes[k].w[i] == B){
					tapes[k].start_index += 1;
				}
				else break;
			}
			//printf("tape[%d]: start: %d\n", k, tapes[k].start_index);
			for (int i = tapes[k].end_index; i >= tapes[k].start_index; i--)
			{// 从后向前
				//printf("tapes[%d].w[%i]: %c/ _\n", k, i, tapes[k].w[i]);
				if(tapes[k].start_index == tapes[k].end_index) break;
				if(tapes[k].w[i] == B){
					tapes[k].end_index -= 1;
				}
				else break;
			}
			if(tapes[k].start_index == tapes[k].w.size()-1) {
				tapes[k].start_index = tapes[k].ptr;
				tapes[k].end_index = tapes[k].ptr;
				//printf("[All Empty] ");
			}
			// if(tapes[k].ptr > tapes[k].end_index){
			// 	int n = tapes[k].ptr - tapes[k].end_index;
		//printf("ptr: %d, start_index: %d, end_index:%d\n", tapes[k].ptr, tapes[k].start_index, tapes[k].end_index);
			// 	panic_on(n!=1, "out of boundary(last)");
			// 	int last = tapes[k].index[tapes[k].end_index];
			// 	tapes[k].w[last+1] = B;
			// 	tapes[k].index[last+1] = last+1;
			// }
			if(tapes[k].ptr < 0){
				int n = 0 - tapes[k].ptr;
				panic_on(n!=1, "out of boundary(before)");
				tapes[k].w.insert(tapes[k].w.begin(), B);
				int first = tapes[k].index[0];
				tapes[k].index.insert(tapes[k].index.begin(), first+1);
				tapes[k].ptr++;
				tapes[k].start_index++;
				tapes[k].end_index++;
			}
			//printf("TAPE[%d]: Start counting B: start: %d, end: %d, ptr: %d, w.size: %lu\n", k, tapes[k].start_index, tapes[k].end_index, tapes[k].ptr, tapes[k].w.size());
		}
		//printf("==========================================\n");
	}
	// 4. 模拟执行完毕，输出结果
	print("Step   : [Final State]\n");
	for (int j = 0; j < N; j++){
		print("Index%d : ", j);
		for(int i=min(tapes[j].start_index, tapes[j].ptr); i <= max(tapes[j].end_index, tapes[j].ptr); i++){
			print("%d ", tapes[j].index[i]);
		}
		print("\n");
		print("Tape%d  : ", j);
		for(int i=min(tapes[j].start_index, tapes[j].ptr); i <= max(tapes[j].end_index, tapes[j].ptr); i++){
			print("%c", tapes[j].w[i]);	
			int b_cnt = digit(tapes[j].index[i]);
			for (int p = 0; p < b_cnt; p++){
				print("%c",' ');
			}
		}
		print("\n");
		print("Head%d  : ", j);
		for (int i = i=min(tapes[j].start_index, tapes[j].ptr); i < tapes[j].ptr; i++){
			print(" ");
			int b_cnt = digit(tapes[j].index[i]);
			for (int p = 0; p < b_cnt; p++){
				print("%c",' ');
			}
		}
		print("^\n");
	}
	print("State  : ");
	print("%s\n", cstate.c_str());
	print("---------------------------------------------\n");
	print("Result: ");
	for(int i=tapes[0].start_index; i <= tapes[0].end_index; i++){
			printf("%c", tapes[0].w[i]);
	}
	printf("\n");
	print("==================== END ====================\n");
}