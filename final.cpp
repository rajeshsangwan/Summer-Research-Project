#include <iostream>
#include <bits/stdc++.h>
#include <unordered_map>
#include <set>
#include <stack>
#include <vector>

using namespace std;

set<string> numbers;
set<string> variable;
set<string> opertor = {"+", "@", "-", "?", "*", "/", "\\", "|", "#", "$", "%", ".", "^", "&", "(", ")", "{", "}", "[", "]", ":", ";", "'", "\"", "~", "`", "<", ">", ",", "=", "!"};
set<string> keywords = {"always", "timescale", "end", "ifnone", "or", "rpmos", "tranif1", "and", "endcase", "initial", "output", "rtran", "@", "tri", "assign", "endmodule", "inout", "parameter", "rtranif0", "tri0", "begin", "endfunction", "input", "pmos", "rtranif1", "tri1", "buf", "endprimitive", "integer", "posedge", "scalared", "triand", "bufif0", "endspecify", "join", "primitive", "small", "trior", "bufif1", "endtable", "large", "pull0", "specify", "trireg", "case", "endtask", "macromodule", "pull1", "specparam", "vectored", "casex", "event", "medium", "pullup", "strong0", "wait", "casez", "for", "module", "pulldown", "strong1", "wand", "cmos", "force", "nand", "rcmos", "supply0", "weak0", "deassign", "forever", "negedge", "real", "supply1", "weak1", "default", "for", "nmos", "realtime", "table", "while", "defparam", "function", "nor", "reg", "task", "wire", "disable", "highz0", "not", "release", "time", "wor", "edge", "highz1", "notif0", "repeat", "tran", "xnor", "else", "if", "notif1", "rnmos", "tranif0", "xor"};

int balanced = 0;

int semi_colon = 1;
bool search_begin = false;

bool case_condition = false;

map<string,set<string>> COD;
map<string,set<string>> COI;
unordered_map <string,int> curr_depend;
stack<vector<string> > blocks;
stack<pair<int,string> > block_name;
int curr_depth=0;

bool if_variable(string &s) {
    if ((s[0] - 'a' < 0 || s[0] - 'a' > 26) && (s[0] - 'A' < 0 || s[0] - 'A' > 25) && (s[0] != ' ')) {
        return false;
    }
    return true;
}

// Function to check if a character is a valid operator
bool operator_check(char ch) {
    string temp = "";
    temp += ch;

    if (opertor.find(temp) == opertor.end()) {
        return false;
    }

    return true;
}

// Function to check if a string is a C++ keyword
bool keyword_check(string &s) {
    if (keywords.find(s) != keywords.end()) {
        return true;
    }
    return false;
}

// Function to check if a string represents a variable or number
bool var_num(string &s) {
    if (opertor.find(s) == opertor.end() && keywords.find(s) == keywords.end()) {
        return true;
    }
    return false;
}

// Function to check if a string represents a variable
bool variable_check(string &s) {
    if (var_num(s) && if_variable(s)) {
        return true;
    }
    return false;
}

// Function to check if a string represents a number
bool number_check(string &s) {
    if (var_num(s) && !if_variable(s)) {
        return true;
    }
    return false;
}

void read_line_COI(const string& temp_line) {
    string curr = "";
    int n = temp_line.size();

    bool lhs = true;
    vector<string> lhs_variable;
    vector<string> rhs_variable;

    vector<string> line;
    for (int i = 0; i < n; i++) {
        if (temp_line[i] == '\'' && i < n - 1 && temp_line[i + 1] == 'b') {
            curr += temp_line[i];
            curr += temp_line[i + 1];
            i++;
            continue;
        }
        if (temp_line[i] == ' ' || operator_check(temp_line[i])) {
            if (!curr.empty()) {
                line.push_back(curr);
            }
            curr = "";
            curr += temp_line[i];
            if (temp_line[i] != ' ')
                line.push_back(curr);
            curr = "";
        } else {
            curr += temp_line[i];
        }
    }
    if (!curr.empty())
        line.push_back(curr);

    n = line.size();

    bool ready_to_push = false;
    vector<string> to_push;
    for (int i = 0; i < n; i++) {
        if (line[i] == "module") {
            return;
        }

        if (line[i] == ";" && search_begin && semi_colon == 1) {
            semi_colon--;
            curr_depth--;
            search_begin = false;

            string stck_top = block_name.top().second;
            int stck_int = block_name.top().first;

            if (stck_int > curr_depth) {
                vector<string> temp = blocks.top();
                block_name.pop();
                blocks.pop();

                for (const auto& varr : temp) {
                    curr_depend[varr]--;
                }
            }
            stck_top = block_name.top().second;
            if (stck_top == "else" || stck_top == "always" || stck_top == "for") {
                vector<string> temp = blocks.top();
                blocks.pop();
                block_name.pop();
                for (const auto& varr : temp) {
                    curr_depend[varr]--;
                }
            }
        } else if (line[i] == "(") {
            balanced++;
        } else if (line[i] == ")") {
            balanced--;
            if (balanced == 0 && ready_to_push == true) {
                ready_to_push = false;
                blocks.push(to_push);
            }
        } else if (line[i] == "=") {
            lhs = false;
        } 

        else if(line[i]=="," || line[i]==";" )
        {
            for(auto i: lhs_variable)
            {
                if(COI.find(i)==COI.end())
                {
                    COI[i]=set<string> {};
                }
                for(auto j: rhs_variable)
                {
                    if(i!=j)
                    {
                        COI[i].insert(j);
                    }
                }
            }
            lhs_variable.clear();
            rhs_variable.clear();
        }
        
        if (variable_check(line[i])) {
            variable.insert(line[i]);

            if (ready_to_push) {
                to_push.push_back(line[i]);
                curr_depend[line[i]]++;
            } else {
                if(lhs==true)
                {
                    lhs_variable.push_back(line[i]);
                }
                else
                {
                    rhs_variable.push_back(line[i]);
                }

                //If lhs is true, the current variable is added to the COI map, and its dependencies are updated by adding other variables from the curr_depend map.
                if(lhs==true)
                {
                    if(COI.find(line[i])==COI.end())
                    {
                        COI[line[i]]=set<string> {};
                    }
                    
                    for(auto varr: curr_depend)
                    {
                        if(varr.first!=line[i] and varr.second>0)
                        {
                            COI[line[i]].insert(varr.first);
                        }
                    }
            }
        }
    }
    else if(keyword_check(line[i])==true)
        {
            //If the keyword is "if", "always", or "for", it indicates the beginning of a new block. 
            if(line[i]=="if" or line[i]=="always" or line[i]=="for")
            {
                //The balanced, semi_colon, and search_begin variables are initialized accordingly, and the current block is pushed to the blocks vector if necessary.
                balanced=0;
                semi_colon=1;
                search_begin=true;
                if(block_name.size()>0 and block_name.top().first==curr_depth)
                {
                    vector<string> temp=blocks.top();
                    blocks.pop();
                    block_name.pop();
                    for(auto varr:temp)
                    {
                        curr_depend[varr]--;
                    }
                }

                block_name.push({curr_depth,line[i]});
                ready_to_push=true;
                curr_depth++;
            }
            //If the keyword is "else", it indicates an "else" block.
            else if(line[i]=="else")
            {
                //The balanced, semi_colon, and search_begin variables are initialized, and the current block is pushed to the blocks vector if necessary.
                semi_colon=1;
                search_begin=true;
                if(i<(n-1) and line[i+1]=="if")
                {
                    balanced=0;
                    to_push=blocks.top();
                    blocks.pop();

                    block_name.pop();
                    block_name.push({curr_depth,"elseif"});
                    
                    ready_to_push=true;
                }
                else
                {
                    block_name.pop();
                    block_name.push({curr_depth,"else"});
                }
                curr_depth++;
            }
            //If the keyword is "initial", it indicates the beginning of an "initial" block.
            else if(line[i]=="initial")
            {
                block_name.push({curr_depth,line[i]});

                //The current depth is incremented, and the necessary variables are initialized.
                curr_depth++;
                search_begin=true;
                semi_colon=1;
                blocks.push(vector<string> {});

            }

            //If the keyword is "begin", it indicates the beginning of a block, and search_begin is set to false.
            if(line[i]=="begin")
            {
                search_begin=false;
            }
            //If the keyword is "end", it indicates the end of a block.
            else if(line[i]=="end" )
            {
                //The current depth is decremented, and the necessary actions are taken based on the type of block being closed.
                curr_depth--;

                string stck_top=block_name.top().second;
                int stck_int=block_name.top().first;
                
                if(stck_int>curr_depth)
                {
                    vector<string> temp=blocks.top();
                    block_name.pop();
                    blocks.pop();

                    for(auto varr:temp)
                    {
                        curr_depend[varr]--;
                    }
                }

                stck_top=block_name.top().second;
                if(stck_top=="else" or stck_top=="always" or stck_top=="for")
                {
                    vector<string> temp=blocks.top();
                
                    blocks.pop();
                    block_name.pop();
                    for(auto varr:temp)
                    {
                        curr_depend[varr]--;
                    }
                }
            }    
        }
        //If the token passes the number_check function, it is identified as a number and added to the numbers set.
        else if(number_check(line[i])==true)
        {
            numbers.insert(line[i]);
        } 
    }

    for (const auto& i : lhs_variable) {
        if (COI.find(i) == COI.end()) {
            COI[i] = set<string> {};
        }
        for (const auto& j : rhs_variable) {
            if (i != j)
                COI[i].insert(j);
        }
    }
}
void read_line_COD(const string& temp_line) {
    string curr = "";
    int n = temp_line.size();

    bool lhs = true;
    vector<string> lhs_variable;
    vector<string> rhs_variable;

    vector<string> line;
    for (int i = 0; i < n; i++) {
        if (temp_line[i] == '\'' && i < n - 1 && temp_line[i + 1] == 'b') {
            curr += temp_line[i];
            curr += temp_line[i + 1];
            i++;
            continue;
        }
        if (temp_line[i] == ' ' || operator_check(temp_line[i])) {
            if (!curr.empty()) {
                line.push_back(curr);
            }
            curr = "";
            curr += temp_line[i];
            if (temp_line[i] != ' ')
                line.push_back(curr);
            curr = "";
        } else {
            curr += temp_line[i];
        }
    }
    if (!curr.empty())
        line.push_back(curr);

    n = line.size();

    // for(int i=0;i<n;i++)
    // {
    //     cout<<line[i]<<" ";
    // }

    bool ready_to_push = false;
    vector<string> to_push;
    for (int i = 0; i < n; i++) {
        if (line[i] == "module") {
            return;
        }

        if (line[i] == ";" && search_begin && semi_colon == 1) {
            semi_colon--;
            curr_depth--;
            search_begin = false;

            string stck_top = block_name.top().second;
            int stck_int = block_name.top().first;

            if (stck_int > curr_depth) {
                vector<string> temp = blocks.top();
                block_name.pop();
                blocks.pop();

                for (const auto& varr : temp) {
                    curr_depend[varr]--;
                }
            }
            stck_top = block_name.top().second;
            if (stck_top == "else" || stck_top == "always" || stck_top == "for") {
                vector<string> temp = blocks.top();
                blocks.pop();
                block_name.pop();
                for (const auto& varr : temp) {
                    curr_depend[varr]--;
                }
            }
        } else if (line[i] == "(") {
            balanced++;
        } else if (line[i] == ")") {
            balanced--;
            if (balanced == 0 && ready_to_push == true) {
                ready_to_push = false;
                blocks.push(to_push);
            }
        } else if (line[i] == "=") {
            lhs = false;
        } 
        // else if (line[i] == "," || line[i] == ";") {
        //     if (ready_to_push) {
        //         to_push.push_back(line[i]);
        //     } else {
        //         for (const auto& i : lhs_variable) {
        //             if (COD.find(i) == COD.end()) {
        //                 COD[i] = set<string> {};
        //             }
        //             for (const auto& j : rhs_variable) {
        //                 if (i != j)
        //                     COD[i].insert(j);
        //             }
        //         }
        //         lhs_variable.clear();
        //         rhs_variable.clear();
        //     }
        // } else if (line[i] == ";") {
        //     if (ready_to_push) {
        //         to_push.push_back(line[i]);
        //         curr_depend[line[i]]++;
        //     } else {
        //         for (const auto& i : lhs_variable) {
        //             if (COD.find(i) == COD.end()) {
        //                 COD[i] = set<string> {};
        //             }
        //             for (const auto& j : rhs_variable) {
        //                 if (i != j)
        //                     COD[i].insert(j);
        //             }
        //         }
        //         lhs_variable.clear();
        //         rhs_variable.clear();
        //     }
        // }
        else if(line[i]=="," || line[i]==";" )
        {
            for(auto i: lhs_variable)
            {
                if(COD.find(i)==COD.end())
                {
                    COD[i]=set<string> {};
                }
                for(auto j: rhs_variable)
                {
                    if(i!=j)
                    {
                        COD[i].insert(j);
                    }
                }
            }
            lhs_variable.clear();
            rhs_variable.clear();
        }
        
        if (variable_check(line[i])) {
            variable.insert(line[i]);

            if (ready_to_push) {
                to_push.push_back(line[i]);
                curr_depend[line[i]]++;
            } else {
                // if (lhs) {
                //     lhs_variable.push_back(line[i]);
                //     if (COD.find(line[i]) == COD.end()) {
                //         COD[line[i]] = set<string> {};
                //     }
                //     for (const auto& varr : curr_depend) {
                //         if (varr.first != line[i] && varr.second > 0)
                //             COD[line[i]].insert(varr.first);
                //     }
                // } else {
                //     rhs_variable.push_back(line[i]);
                // }
                //If lhs is true (left-hand side), the variable is added to the lhs_variable vector. Otherwise, it is added to the rhs_variable vector.
                if(lhs==true)
                {
                    lhs_variable.push_back(line[i]);
                }
                else
                {
                    rhs_variable.push_back(line[i]);
                }

                //If lhs is true, the current variable is added to the COD map, and its dependencies are updated by adding other variables from the curr_depend map.
                if (COD.find(line[i]) == COD.end()) {
                    COD[line[i]] = set<string> {};
                }
                
                // Reverse the relationship and update COD accordingly
                for (const auto& var : lhs_variable) {
                    if (var != line[i]) {
                        COD[line[i]].insert(var);
                    }
                }
        }
    }
    else if(keyword_check(line[i])==true)
        {
            //If the keyword is "if", "always", or "for", it indicates the beginning of a new block. 
            if(line[i]=="if" or line[i]=="always" or line[i]=="for")
            {
                //The balanced, semi_colon, and search_begin variables are initialized accordingly, and the current block is pushed to the blocks vector if necessary.
                balanced=0;
                semi_colon=1;
                search_begin=true;
                if(block_name.size()>0 and block_name.top().first==curr_depth)
                {
                    vector<string> temp=blocks.top();
                    blocks.pop();
                    block_name.pop();
                    for(auto varr:temp)
                    {
                        curr_depend[varr]--;
                    }
                }

                block_name.push({curr_depth,line[i]});
                ready_to_push=true;
                curr_depth++;
            }
            //If the keyword is "else", it indicates an "else" block.
            else if(line[i]=="else")
            {
                //The balanced, semi_colon, and search_begin variables are initialized, and the current block is pushed to the blocks vector if necessary.
                semi_colon=1;
                search_begin=true;
                if(i<(n-1) and line[i+1]=="if")
                {
                    balanced=0;
                    to_push=blocks.top();
                    blocks.pop();

                    block_name.pop();
                    block_name.push({curr_depth,"elseif"});
                    
                    ready_to_push=true;
                }
                else
                {
                    block_name.pop();
                    block_name.push({curr_depth,"else"});
                }
                curr_depth++;
            }
            //If the keyword is "initial", it indicates the beginning of an "initial" block.
            else if(line[i]=="initial")
            {
                block_name.push({curr_depth,line[i]});

                //The current depth is incremented, and the necessary variables are initialized.
                curr_depth++;
                search_begin=true;
                semi_colon=1;
                blocks.push(vector<string> {});

            }

            //If the keyword is "begin", it indicates the beginning of a block, and search_begin is set to false.
            if(line[i]=="begin")
            {
                search_begin=false;
            }
            //If the keyword is "end", it indicates the end of a block.
            else if(line[i]=="end" )
            {
                //The current depth is decremented, and the necessary actions are taken based on the type of block being closed.
                curr_depth--;

                string stck_top=block_name.top().second;
                int stck_int=block_name.top().first;
                
                if(stck_int>curr_depth)
                {
                    vector<string> temp=blocks.top();
                    block_name.pop();
                    blocks.pop();

                    for(auto varr:temp)
                    {
                        curr_depend[varr]--;
                    }
                }

                stck_top=block_name.top().second;
                if(stck_top=="else" or stck_top=="always" or stck_top=="for")
                {
                    vector<string> temp=blocks.top();
                
                    blocks.pop();
                    block_name.pop();
                    for(auto varr:temp)
                    {
                        curr_depend[varr]--;
                    }
                }
            }    
        }
        //If the token passes the number_check function, it is identified as a number and added to the numbers set.
        else if(number_check(line[i])==true)
        {
            numbers.insert(line[i]);
        } 
    }

    for (const auto& i : lhs_variable) {
        if (COD.find(i) == COD.end()) {
            COD[i] = set<string> {};
        }
        for (const auto& j : rhs_variable) {
            if (i != j)
                COD[i].insert(j);
        }
    }

    // for(auto entry: COD)
    // {
    //     string key=entry.first;
    //     set<string> values=entry.second;

    //     cout<<"Key:"<<key<<endl;
    //     cout<<"Values: ";
    //     for(auto value:values)
    //     {
    //         cout<<value<<" ";
    //     }
    // }
}

void COI_bfs(string s) {
    string ans;
    set<string> visited;

    if (variable.find(s) == variable.end()) {
        ans = "Variable not found in file\n";
        cout << ans;
        return;
    } else if (COI.find(s) == COI.end()) {
        ans = "No dependencies\n";
        cout << ans;
        return;
    }

    queue<string> qu;
    qu.push(s);
    qu.push("module");
    int current_level = 1;

    while (qu.size() > 1) {
        if (qu.front() != "module") {
            string curr = qu.front();
            if (curr != s) {
                ans += curr;
                ans += '\n';
            }
            qu.pop();
            for (const auto& i : COI[curr]) {
                if (visited.find(i) == visited.end()) {
                    visited.insert(i);
                    qu.push(i);
                }
            }
        } else {
            qu.pop();
            if (qu.size() > 0) {
                string temp = "Level";
                string temp2 = to_string(current_level);
                temp += temp2;
                temp += '\n';
                current_level++;
                ans += temp;
            } else {
                break;
            }
            qu.push("module");
        }
    }

    cout << ans;
}

void COD_bfs(const string& s) {
    string ans;
    set<string> visited;

    if (variable.find(s) == variable.end()) {
        ans = "Variable not found in file\n";
        cout << ans;
        return;
    } else if (COD.find(s) == COD.end()) {
        ans = "No dependencies\n";
        cout << ans;
        return;
    }

    queue<string> qu;
    qu.push(s);
    qu.push("module");
    int current_level = 1;

    while (qu.size() > 1) {
        if (qu.front() != "module") {
            string curr = qu.front();
            if (curr != s) {
                ans += curr;
                ans += '\n';
            }
            qu.pop();
            for (const auto& i : COD) {
                if (i.second.find(curr) != i.second.end()) {
                    string dependent_var = i.first;
                    if (visited.find(dependent_var) == visited.end()) {
                        visited.insert(dependent_var);
                        qu.push(dependent_var);
                    }
                }
            }
        } else {
            qu.pop();
            if (qu.size() > 0) {
                string temp = "Level";
                string temp2 = to_string(current_level);
                temp += temp2;
                temp += '\n';
                current_level++;
                ans += temp;
            } else {
                break;
            }
            qu.push("module");
        }
    }

    cout << ans;
}




string removeComments(const string& fileContent) {
    string result;
    bool singleComment = false;
    bool multiComment = false;

    for (size_t i = 0; i < fileContent.size(); ++i) {
        if (!singleComment && !multiComment) {
            if (fileContent[i] == '/') {
                if (i < fileContent.size() - 1) {
                    if (fileContent[i + 1] == '/') {
                        singleComment = true;
                        ++i; // Skip the next character '/'
                        continue;
                    }
                    else if (fileContent[i + 1] == '*') {
                        multiComment = true;
                        ++i; // Skip the next character '*'
                        continue;
                    }
                }
            }
            result += fileContent[i];
        }
        else if (singleComment && fileContent[i] == '\n') {
            singleComment = false;
            result += fileContent[i];
        }
        else if (multiComment && fileContent[i] == '*' && i < fileContent.size() - 1 && fileContent[i + 1] == '/') {
            multiComment = false;
            ++i; // Skip the next character '/'
        }
    }

    return result;
}

int main() {
    string file_name;  // Specify the Verilog file name
    cout << "Enter the name of the Verilog file: ";
    getline(cin, file_name);

    // string variable_name = "FORMAL"; // Specify the variable name

    ifstream file(file_name);
    if (!file) {
        cout << "Error opening file!\n";
        return 1;
    }

    string fileContent((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
    file.close();

    string temp_file = removeComments(fileContent);
    
    vector<string> line;
    string curr="";
    bool last_space=false;
    bool line_start=true;
    bool last_semi=false;
    int n=temp_file.size();
    for(int i=0;i<n;i++)
    {
        if(line_start==true)
        {   
            //If the parsing is at the start of a line, it skips any leading whitespace, semicolons, or newline characters.
            if((temp_file[i]==' ' or temp_file[i]=='\t' or temp_file[i]==';' or temp_file[i]=='\n'))
            {
                continue;
            }
            else{
                //Once a non-whitespace, non-semi-colon character is encountered, it marks the end of the line start and appends the character to the curr string.
                line_start=false;
                curr+=temp_file[i];
            }
        }
        else
        {
            if(last_semi==true)
            {
                //Once a non-whitespace, non-semi-colon character is encountered, it marks the end of the line start and appends the character to the curr string.
                if(temp_file[i]==';' or temp_file[i]==' '  or temp_file[i]=='\t' or temp_file[i]=='\n')
                {
                    continue;
                }
                else
                {
                    last_semi=false;
                }
            }
            if(last_space==true)
            {
                if(temp_file[i]==' '  or temp_file[i]=='\t')
                {
                    continue;
                }
            }
            //If the current character is a tab, it is replaced with a space character in curr.
            if(temp_file[i]=='\t')
            {
                curr+=' ';
            }
            //If the current character is not a newline character, it is appended to curr.
            else if(temp_file[i]!='\n')
            {
                curr+=temp_file[i];
            }
            //If the current character is a space or tab, the last_space flag is set.
            if(temp_file[i]==' ' or temp_file[i]=='\t')
            {
                last_space=true;
            }
            //If the current character is a newline character, it marks the end of the line, resets the flags, appends curr to the line vector, and clears curr.
            else if(temp_file[i]=='\n')
            {
                last_space=false;
                last_semi=false;
                line_start=true;

                line.push_back(curr);
                curr="";
            }
            else if(temp_file[i]==';')
            {
                last_semi==true;
                last_space=false;
                line_start=true;

                line.push_back(curr);
                curr="";
            }else
            {
                last_space=false;
            }
        }
    }

    // for(int i=0; i<line.size(); i++)
    // {
    //     cout<<line[i]<<" ";
    // }

    for (const string& li: line) {
        // Process each line here (e.g., call read_line(line))
        read_line_COD(li);
    }

    for (const string& li: line) {
        // Process each line here (e.g., call read_line(line))
        read_line_COI(li);
    }

    cout<<"Below is the list of variables present in verilog file:"<<endl;
    for(auto var: variable)
    {
        cout<<var<<" ";
    }
    cout<<endl;

    // for(auto num: numbers)
    // {
    //     cout<<num<<" ";
    // }

    // for(auto i: COD){

    //     cout<<"variable "<<i.first<<" \n";
    //     for(auto j: i.second){
    //         cout<<j<<" ";
    //     }
    //     cout<<'\n';
    // }

    // for(auto i: COD){

    //     if(i.first==variable_name)
    //     {
    //         for(auto j: i.second){
    //         cout<<j<<" ";
    //     }
    //     }
    //     cout<<'\n';
    // }
    cout<<"For which variable above mentioned you want to see dependencies."<<endl;
    string variable_name;  // Specify the Verilog file name
    cout << "Enter the name of variable: ";
    getline(cin, variable_name);
    cout<<"Below is answer of COI of variable "<<variable_name<<":"<<endl;
    COI_bfs(variable_name);
    cout<<endl;
    cout<<"Below is answer of COD of variable "<<variable_name<<":"<<endl;
    COD_bfs(variable_name); // with the specified variable_name here
    cout<<"Note : if you are getting clear screen, it means that your entered variable has no dependecies"<<endl;
    return 0;
}
