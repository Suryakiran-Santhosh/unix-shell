/*
University of California Davis ECS 150: Operating System
By: Suryakiran Santhosh
Project 2: Shell
April 8, 2021
*/


#include <fcntl.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/uio.h>
#include <unistd.h>
#include <errno.h>

#include <iostream>
#include <string>
#include <vector>
#include <cstring>

using namespace std;

void error();
string lowercase(string);
vector<string> string_processing(string);
vector<string> separate_by_ampersand(string);
vector<string> split_by_parallel_command(string);

bool built_in_checker(string);
void cmd_exit(int);
void cmd_cd(string);
vector<string> set_path(vector<string>, vector<string>);

void parallel_commands(vector<vector<string>>, vector<string>);
int redirection_check(vector<string>);
void redirection(vector<string>,  string, int);
string system_commands(vector<string>, vector<string>);

void execute_command(vector<string>, string);


vector<string> split_by_newline(string);
int parallel_check(vector<string>);


vector<string> split_by_spaces(string);
bool redirection_exists(string);
vector<string> split_by_redirection(string);

vector<string> parallel_operator_validation_and_refactoring(vector<string>);
bool whitespace_removal_check(string);

int main(int argc, char *argv[]){
    
    if (argc == 1){
        // interactive mode
        //cout << "interactive mode" << endl;
        /*
        string prompt = "wish> ";
        int prompt_size = prompt.size();
        string user_input = "";
    
        vector<string> search_paths = {"/bin"};

        while(user_input != "exit"){
            //prints out the shell prompt
            write(STDOUT_FILENO, prompt.c_str(), prompt_size); 
            
            // gets the entire line of user input
            getline(cin, user_input); 
            
            // to hold input commands
            // this array should reset on each iteration of the prompt
            vector<string> commands = string_processing(user_input);

            // each command is parsing by space
            vector<vector<string>> cmd; 
            for(int i = 0; i < int(commands.size()); i++){
                cmd.push_back(split_by_parallel_command(commands[i]));
            }

            //built in command check
            if(built_in_checker(cmd[0][0])){
                if(cmd[0][0] ==  "cd"){
                    if(cmd[0].size() != 2){
                        error();
                    }
                    else{
                        cmd_cd(cmd[0][1]);
                    }   
                }
                else if (cmd[0][0] == "exit"){
                    cmd_exit(int(cmd[0].size()));
                }
                else if (cmd[0][0] == "path"){
                    search_paths = set_path(search_paths, cmd[0]);
                }
            }
            else{
                cout << "WILL IMPLEMENT INTERACTIVE MODE LATER" << endl;
            }
        }

        */
    }
    else{
        // batch file mode
        //cout << "inside batch" << endl;
        vector<string> search_paths = {"/bin"};

        int fd;
        const int SIZE = 1;  // size of the read buffer
        
        // iterate through all the elements that were passed into the executable
        for(int file = 1; file < argc; file++){
            
            // try to open the file
            fd = open(argv[file], O_RDONLY, 0644);  // S_IRWXU
            if(fd == -1){ 
                //cout << "insider this  error 1" << endl;
                // unable to open the file
                error();
                exit(1);
            }
            
            char buffer[SIZE];
            int ret;

            string command_line_input = "";

            // read from the file (the entire file)
            while((ret = read(fd, buffer, SIZE)) > 0){
                command_line_input += buffer[0];
            }

            // all the multiple commands
            vector<string> commands = split_by_newline(command_line_input);
            
            
            // each command is parsing by space
            vector<vector<string>> tmp; 
            for(int i = 0; i < int(commands.size()); i++){
                tmp.push_back(split_by_parallel_command(commands[i]));
            }
            
            
            // parse command by spaces
            vector<vector<string>> cmd; 
            for(int i = 0; i < int(commands.size()); i++){
                cmd.push_back(split_by_spaces(commands[i]));
            }
    
            // check to see if whitespaces have been removed if they haven't then remove  them here.
            
            for(int y = 0; y < int(cmd.size()); y++)
            {
                auto tmp = cmd[y];
                if (tmp.size() == 0){
                    cmd.erase(cmd.begin() + y);
                }   
            }
            
            // iterate through each command
            for(int line = 0; line < int(cmd.size()); line++){
                // each command
                vector<string> command = cmd[line];
                if (command.size() == 0){
                    exit(0);
                }

                /*
                vector<string> tmp;
                cout << command.size() << endl<< endl;
                cout << parallel_check(command) << endl << endl;
                if(command.size() == 1 and parallel_check(command) != 0){
                    cout << "here" << endl;
                    for(int i = 0; i < int(command.size()); i++)
                    {
                        tmp = split_by_parallel_command(command[i]);
                    }
                    command.clear();
                    for(int i = 0; i < int(tmp.size()); i++){
                        command[i] = tmp[i];
                    }
                }
                */
                
                

                // cout << command.size() << endl;
                /*
                cout << "iteration " << line << endl;
                cout << "=======================" << endl;
                cout << command.size() << endl;
                for(int x = 0; x <int(command.size()); x++){
                    // cout << "here"  << endl;
                    //cout << command.size() << endl;
                    cout << x << ".) " << command[x] << endl;
                }
                */


                // check if command is a parallel command
                bool parallel = false;
                if(parallel_check(command) != 0){
                    
                    if(command.size() == 1 &&  command[0] == "&"){
                        exit(0);
                    }
                    if(command.size() != 1){
                        parallel = true;
                    }
                }
                else{
                    int para = 0;
                    for(string val: command){
                        // cout << val << endl;
                        for(int o = 0; o < int(val.size()); o++){
                            if (val[o] == '&'){
                                para++;
                            }
                        }
                    }
                    
                    vector<string> tmp;
                    string w = "";
                    if(para != 0){
                        
                        for(int ind = 0; ind < int(command.size()); ind++){
                            string k = command[ind];
                            for(int j = 0; j < int(k.size()); j++){
                                string cond = "";
                                cond += k[j];
                                if (cond == "&"){
                                    tmp.push_back(w);

                                    
                                    tmp.push_back(cond);
                                    w.clear();
                                }
                                else{
                                    w += k[j];
                                }
                            }
                        }
                        if (w.length() != 0){
                            tmp.push_back(w);
                        }
                        command = tmp;
                        parallel = true;
                    }
                }
                

                if (parallel){
                    
                    vector<string> input;
                    
                    // whitespace check
                    for(int index = 0; index < int(command.size()); index++)
                    {
                        if(!whitespace_removal_check(command[index]))
                        {
                            //cout << "here" << endl;
                            input.push_back(command[index]);
                        }
                    }

                    /*
                    cout << "=======================" << endl;
                    for(int x = 0; x <int(input.size()); x++){
                        // cout << "here"  << endl;
                        //cout << command.size() << endl;
                        cout << x << ".)" << input[x] << "->" << input[x].size() << endl;
                    }
                    cout << endl << endl;
                    */
                    

                    // at this point input vector is split by spaces
                    vector<vector<string>> input_parallel;
                    vector<string> temp;
                    for(int i = 0; i < int(input.size()); i++)
                    {
                        if (input[i] != "&")
                        {
                            temp.push_back(input[i]);
                        }
                        else
                        {
                            input_parallel.push_back(temp);
                            temp.clear();
                        }
                    }

                    // the leftovers in temp
                    if(temp.size() != 0){
                        input_parallel.push_back(temp);
                    }
                    /*
                    cout << "here" << endl;
                    cout << "=======================" << endl;
                    for(vector<string> l: input_parallel){
                        for (string u : l){
                            cout << u << endl;
                        }
                    }
                    cout <<  endl << endl;
                    */
                    
                    /*
                    for(int x = 0; x <int(input_parallel.size()); x++){
                        // cout << "here"  << endl;
                        //cout << command.size() << endl;
                        cout << x << ".)" << input[x] << " and size: " << input[x].size() << endl;
                    }
                    */

                    //cout << endl << "PARALLEL COMMAND FUNCTION CALL: " << endl;
                    parallel_commands(input_parallel, search_paths);

                }
                else{
                    //cout << "in here" << endl;
                    // if not parallel command
                    //built in command check
                    // string u = "here lien 311\n";
                    // write(STDERR_FILENO, u.c_str(), u.size());

                    string operation = command[0];
                    if(built_in_checker(operation)){
                        if(operation ==  "cd"){
                            if(command.size() != 2){
                                error();
                            }
                            else{
                                cmd_cd(command[1]);
                            }
                        }
                        else if (operation == "exit"){
                            // string p = "exit\n";
                            // write(STDERR_FILENO, p.c_str(), p.size());
                            // exit(0);
                            cmd_exit(int(command.size()));
                            // exit(0);
                        }
                        else if (operation == "path"){
                            search_paths = set_path(search_paths, command);
                        }
                    }
                    else{
                        // look for redirection command
                        vector<string> ready_to_execute;
                        bool needed_redirection = false;

                        for(int x = 0; x < int(command.size()); x++){
                            if(redirection_exists(command[x])){
                                // if there is a redirection command then process it
                                vector<string> processed = split_by_redirection(command[x]);

                                for (int i = 0; i < int(processed.size()); i++)
                                {
                                    ready_to_execute.push_back(processed[i]);
                                }
                                needed_redirection = true;
                            }
                            else{
                                // if no redirection command just append 
                                ready_to_execute.push_back(command[x]);
                            }
                        } // done with spliting by redirection operator
                        
                        /*
                        cout << "ready to execute: " << endl;
                        for(int i = 0; i < int(ready_to_execute.size()); i++){
                            cout <<"ready_to_execute[" << i<< "]: " << ready_to_execute[i] << endl;
                        }
                        */

                        // need to check and make sure that the > operator is located in the correct place
                        // check to see if the command is valid based off of position
                        int valid_redirection_or_index = redirection_check(ready_to_execute);

                        if(needed_redirection && valid_redirection_or_index == -1 && needed_redirection){
                            // the case of invalid redirection
                            //cout << "here" << endl;
                            error();
                        }
                        else if (needed_redirection && valid_redirection_or_index != -1 ){
                            // the properly formatted case of redirection
                            /*
                            cout << "valid redirection" << endl;
                            cout << "index of redirection operator is: ";
                            cout << valid_redirection_or_index << endl;
                            */
                            // need to make sure that the command is valid and in a path
                            //cout << "here 1" << endl;
                            string path = system_commands(ready_to_execute, search_paths);
                            
                            // execute the redirected command
                            redirection(ready_to_execute, path, valid_redirection_or_index);
                            needed_redirection  = true;
                        }
                        else if(needed_redirection == false){
                            //cout << "in here" << endl;
                            string cmd_path = system_commands(ready_to_execute, search_paths);
                            if(cmd_path == "-1")
                            {
                                error();
                            }
                            else
                            {
                                
                                // not a parallel command case
                                execute_command(ready_to_execute, cmd_path);
                            }
                            
                        }
                        

                    } // built-in commands vs. system commands
                } // parallel and !parallel commands
            } // for loop through each line of the input file
        } // for loop for each input file
        //return 0;
    } // interactive vs. batch mode

    return 0;
} // main


vector<string> split_by_newline(string in){
    vector<string> result;
    string command = "";

    for(int i = 0; i < int(in.size()); i++){
        if(in[i] == '\n'){
            result.push_back(command);
            command.clear();
        }
        else{
            command += in[i];
        }
    }

    /*
    cout << "no new line characters: " << endl;
    for(int i = 0; i < int(result.size()); i++){
        cout << result[i] << endl;
    }
    */
    
    return result;
}


bool redirection_exists(string command){
    // checks if there is a redirection operator in the string
    for (int i = 0; i < int(command.size()); i++){
        if (command[i] == '>'){
            return true;
        }
    }
    return false;
}

int redirection_check(vector<string> in){
    // returns the index of the redirection operator
    // if redirection operator exists and it is in the right location
    
    const int invalid = -1;

    if(in.size() < 3){
        // for redirection the length of the command should be at least 3
        return invalid;
    }
    else{
        int index = 0;
        int counter = 0;

        // iterate through commands and find the index of the redirection command
        for(int i = 0; i < int(in.size()); i++){
            if(in[i] == ">"){
                counter++;
                index = i;
            }
        }

        // multiple redirection is not allowed
        if(counter != 1){ 
            return invalid;
        }

        int last_index = int(in.size()) - 1;

        // the redirection index has to be the second to last element in the commands array
        if((index + 1) == last_index){
            return index;
        }
        else{
            return invalid;
        }
    }
}


void redirection(vector<string> cmd, string path, int index){

    // the index of the ">" is located at index
    // execute the left side of redirection index 
    pid_t pid = fork();
    
    if(pid == 0)
    {
        // child process is always denoted by 0, on successful creation 
        string output_file = cmd[index + 1];

        // open the file to the right side of the index 
        // if can't open file then create the file and then output to it
        int out_file_fd = open(output_file.c_str(), O_TRUNC | O_CREAT);

        // num of things to execute
        int operations = index;
        char * arr[operations];

        for(int i = 0; i < operations; i++){
            arr[i] = strdup(cmd[i].c_str());
            //  cout <<  arr[i] << endl;
        } 
        arr[operations] = NULL;

        execv(path.c_str(),  arr);

        dup2(out_file_fd, STDOUT_FILENO); // redirecting output

    }
    else
    {
        // parent process code goes here
        wait(0);
        // open the output file
        // if file doesn't exit then create file and then output to it 
    }
    // cout << "Will Implement " << endl;
}


int parallel_check(vector<string> in){
    int result = 0;

    for(int i = 0; i < int(in.size()); i++){
        if (in[i] == "&"){
            result++;
        }
    }

    return result;
}


bool whitespace_removal_check(string cmd)
{
    if (cmd.size() == 0)
    {
        return true;
    }
    else{
        for(int i = 0; i < int(cmd.size()); i++)
        {
            if(isspace(cmd[i]))
            {
                cout << "whitespace" << endl;
                return true;
            }
        }
    }
    
    return false;
}


void parallel_commands(vector<vector<string>> commands, vector<string> path){

    int number_of_processes = int(commands.size());
     // int status;
    vector<pid_t> process_ids (number_of_processes);
    // cout << number_of_processes << endl;
    //int process_ids[number_of_processes];

    // run each command
    for(int process = 0; process < number_of_processes; process++){
        vector<string> command = commands[process];

        // redirection necessity check
        bool redirect = false;
        for(string c : command){
            if(redirection_exists(c)){
                redirect = true;
            }
        }

        if (redirect){
            // check if a valid redirection 
            int redirection_index = redirection_check(command);
            if(redirection_index ==  -1){
                // invalid redirection command
                error();
            }
            else{
                // valid redirection
                process_ids[process] = fork();
    
                if(process_ids[process] == 0){
                    // child process is always denoted by 0, on successful creation 
                    string output_file = command[redirection_index + 1];

                    // open the file to the right side of the index 
                    // if can't open file then create the file and then output to it
                    // int out_file_fd = open(output_file.c_str(), O_TRUNC | O_CREAT, 0644);
                    FILE* file;
                    file = fopen(output_file.c_str(), "w");

                    if(file == NULL){
                        error();
                        exit(1);
                    }
                    
                    int fd = fileno(file); 
                    dup2(fd, STDOUT_FILENO);
                    dup2(fd, STDERR_FILENO);
                    fclose(file);

                    // num of things to execute
                    int operations = redirection_index;
                    char * arr[operations];

                    for(int i = 0; i < operations; i++){
                        arr[i] = strdup(command[i].c_str());
                        //  cout <<  arr[i] << endl;
                    } 
                    arr[operations] = NULL;

                    string p = system_commands(command, path);
                    if (p == "-1"){
                        error();
                    }
                    
                    execv(p.c_str(),  arr);

                    // dup2(out_file_fd, STDOUT_FILENO); // redirecting output                
                }
            }
        }
        else{
            // normal commands
            process_ids[process] = fork();
            
            if(process_ids[process] == 0){
                 // cout << " here" << endl;
                // child process
                string valid_path = system_commands(command, path);
                if (valid_path == "-1"){
                    error();
                }
                else{
                    // execute_command(command, valid_path);
                     // num of things to execute
                    int operations = int(command.size());
                    char * arr[operations];

                    for(int i = 0; i < operations; i++){
                        arr[i] = strdup(command[i].c_str());
                        //cout <<  arr[i] << endl;
                    } 
                    arr[operations] = NULL;

                    execv(valid_path.c_str(),  arr);
                }
            }
        }
    }

    pid_t wait_pid;
    while ((wait_pid = wait(NULL)) > 0);;
    

    // cout << endl << "Will Implement" << endl;
}


vector<string> split_by_parallel_command(string input){
    vector<string> result;

    // check for multiple commands
    string cmd = "";
    for(int i = 0; i < int(input.size()); i++){
        if (input[i] == '&'){
            result.push_back(cmd);  // add the command to results
            result.push_back("&"); 
            cmd.clear();  // clear to search for new commands
        }
        else{
            cmd += input[i];
        }
    }
    
    if(cmd.size() != 0){
        result.push_back(cmd);
    }
    
    /*
    cout << "In the parallel Command Function: " << endl;
    for(int i = 0; i < int(result.size()); i++){
        cout << result[i] << endl;
    }
    cout << "----------------------" << endl;
    */
    

    return result;
}



string system_commands(vector<string> commands, vector<string> search_paths){

    // find a working path
    for(int i = 0; i < int(commands.size()); i++){
        string possible_path = "";
        possible_path += search_paths[i];
        
        string cmd = "/" + commands[0];
        possible_path += cmd;

        if (access(possible_path.c_str(), X_OK) == 0){
            return possible_path;
        } 
        else{
            possible_path.clear();
            cmd.clear();
        }
    }
    
    return "-1";
}


void execute_command(vector<string> cmd, string path){
    // cout << endl << "program is in the fork portion" << endl;
    // singular commands
    
    pid_t process_id = fork();
    if(process_id == 0){
        // cout << "here" << endl;
        // the child process, the id of child process will always be 0
            
        // num of things to execute
        int operations = int(cmd.size());
        
        char * arr[operations + 1];

        for(int i = 0; i < operations; i++){
            arr[i] = strdup(cmd[i].c_str());
        } 
        arr[operations] = NULL;
        /*
        for(int i = 0; i < operations; i++){
            cout <<i<< " : "  << arr[i] << endl;
        } 
        */
        // cout << path << endl;
        execv(path.c_str(),  arr);
    }
    else{
        // the parent process
        //cout << "parent scope after fork" << endl;
        // wait for the child process to end
        //waitpid(process_id, NULL, 0);
        wait(0);
        //return;
    }
}


vector<string> split_by_spaces(string command){
    vector<string> result;
    string word = "";
    
    // iterate through line
    for(int i  = 0; i < int(command.size()); i++){
        // string line = command[i]; // line
        // cout << "line : " << line << endl  << "length: " << line.length() << endl;
        
        if (isspace(command[i])){
            if (word.size() != 0){
                result.push_back(word);
            }
            word.clear();
        }
        else{
            word += command[i];
        }
        
        /*
        // iterate through each character in the line
        for(int j = 0; j < int(line.size()); j++){
            if (isspace(line[j])){
                if (word.size() != 0){
                    result.push_back(word);
                }
                word.clear();
            }
            else{
                word += line[j];
            }
        }   
        */
    }

    // the last element
    if (word.size() != 0){
        result.push_back(word);
    }

    /*
    // test loop
    for(int i = 0; i < int(result.size()); i++){
        cout << i << ": " << result[i] << endl;
    }
    */
    

    return result;
}



vector<string> split_by_redirection(string command){
    vector<string> result;

    string current_string = "";

    for(int i = 0; i < int(command.size()); i++)
    {
        if (command[i]  == '>')
        {
            if(current_string.size() != 0)
            {
                result.push_back(current_string);
            }
            current_string.clear();
            result.push_back(">");
        }
        else
        {
            current_string += command[i];
        }

        // the very last current string in the case that nothing comes up again
        if(i == int(command.size()) - 1)
        {
            if (current_string.size() != 0)
            {
                result.push_back(current_string);
            }
        }
    }
    
    return result;
}
/*
    vector<string> result;

    // interate through each string in the input command
    // int counter = 0;
    string before_redirection = "";
    for(string word : command){
        // cout << ++counter;
        // cout << ".) word: " << word << " length: "<< word.length() << endl;
        for(int i = 0; i < int(word.size()); i++){
            if (word[i] == '>'){
                result.push_back(before_redirection);
                before_redirection.clear();
                result.push_back(">");
            }
            else{
                before_redirection += word[i];
                // cout << before_redirection << endl;
            }
        }
    }

    return result;
}
*/

// DON'T NEED THIS ONE
vector<string> cmd_parsing(string in){
    vector<string>  result;
    string current_input = "";

    for (int i = 0; i < int(in.size()); i++){
        // check for whitespace if whitespace append or move on
        if (isspace(in[i])){
            if (current_input.size() != 0){
                result.push_back(current_input);
            }
            current_input.clear();
        }
        else{
            current_input += in[i];
        }
    }
    
    // the last element
    if (current_input.size() != 0){
        result.push_back(current_input);
    }

    /*
    for(int x = 0; x < int(result.size()); x++){
        cout << result[x] << " -> ";
    }
    */

    return result;
} 


bool built_in_checker(string in){
    // checks if the command is built into the shell or not 
    in = lowercase(in);
    if ((lowercase(in) == "cd") || (lowercase(in) == "path") || (lowercase(in) == "exit")){
        return true;
    }
    return false;
}


void cmd_exit(int size){  
    // size is the size of the commands list  
    if (size != 1){
        // prints an error message
        // cout << "here" << endl;
        error();
    }
    else {
        exit(0);  // end the program
    }
}


void cmd_cd(string directory){
    int ret = chdir(directory.c_str());
    if (ret == -1){
        error();
    }
    /*
    else{
        cout << "I seems like it worked" << endl;
    }*/
}


vector<string> set_path(vector<string> search_paths, vector<string> cmd) {
    vector<string> result;

    /*
    for(auto i : search_paths){
        cout << "element: " << i << endl;
    }
    */

    if(cmd.size() == 1){
        search_paths.clear();
        // result.push_back("empty");
    }
    else{
        for(int x = 1; x < int(cmd.size()); x++){
            result.push_back(cmd[x]);
        }
    }
    
    return result;
}


void error(){
    char error_message[30] = "An error has occurred\n";
    write(STDERR_FILENO, error_message, strlen(error_message));
}


string lowercase(string in){
    for(int i = 0; i < int(in.size()); i++){
        in[i] = tolower(in[i]);
    }
    return in;
}


vector<string> parallel_operator_validation_and_refactoring(vector<string> in)
{
    vector<string> result;

    for(int i = 0; i < int(in.size()); i++)
    {
        if (in[i] == "&")
        {
            bool tmp = i < int(in.size()) - 1;
            if (tmp)
            {
                result.push_back(in[i]);
            }
        }
        else
        {
            result.push_back(in[i]);
        }
    }

    
    // cout << "in here" << endl;
    vector<string> tmp;
    for(int i = 0; i < int(result.size()); i++)
    {
        vector<string> func_output = split_by_parallel_command(result[i]);
        
        for(string word : func_output)
        {
            //cout << word << endl;
            tmp.push_back(word);
        } 
        /*
        for(int j = 0; j < int(func_output.size()); j++)
        {
            cout << "here" << endl;
            cout << func_output[j] << endl;
            tmp.push_back(func_output[i]);
        }   
        */
    }

    
    /*
    cout << "output after spliting by parallel command: " << endl;
    for(int i = 0; i < int(tmp.size()); i++)
    {
        cout << tmp[i] << endl;
    }
    */

    return tmp;
    

    //return result;
}
