//This program is gonna be the shit.
//time to comment

//Author: Matthew Fevold
//
//This program creates a series of rainbow chains called a rainbow table and
//outputs that to a file along with the header info associated with it.
//
//it works be doing this n times: take a plaintext - hash it - reduce it repeat
//that process m times. tada! 

//to compile :
//	   g++ -O2 2_parallel_create_table.cpp -lpthread -o 2_parallel_create_table
//to run: 
//	   ./2_parallel_create_table length_min length_max table_index chain_legth //					chain_number character_set thread_number
//				     output_file
//	explanation of args:
//		length_min - length_max: minimum and max plaintext password length
//		table_index - create unique rainbow table based on this value
//		chain_len - how many hash/reduce functions you do
//		chain_num - how many chains to create
//		character_set - which characters do you want to break - currently
//				only 0-9 works
//		thread_number - how many threads do you want to spawn?
//		output_file - file path (from current directory) to write to
//
//	example of execution: ./2_parallel_create_table 1 *7* 1 *10000* *2000* 
//					0123456789 *4* *file.txt*
// typically you should only change the values between **'s

#include <stdio.h>
#include <fstream>
#include <stdlib.h>
#include <iostream>
//I think I only need 1 of the next 2
#include <string.h>
#include <cstring>
#include <sstream>
#include <cmath>
#include <time.h>
#include <sys/time.h>
#include <pthread.h>

//DO NOT TOUCH
//Init values - for lmnt hash
#define INIT_A 0x67452301
#define INIT_B 0xefcdab89
#define INIT_C 0x98badcfe
#define INIT_D 0x10325476
 
#define SQRT_2 0x5a827999
#define SQRT_3 0x6ed9eba1
 
unsigned int nt_buffer[16];
unsigned int output[4];
//end of stuff for lmnt hash
//END OF DO NOT TOUCH

//mutex for writing to file.
pthread_mutex_t lock;

//for convineince
using namespace std;

//so that many functions can use the same file.
char * outputFile;

//struct for pthread passing of info.
struct thread_data
{
	int thread_id;
	string password;
	string character_set;
	int len_min;
	int len_max;
	int table_index;
	int chain_length;
	int chain_number;
};

//function declaration
int get_index(int a,int);
void * thread_action (void *);
string create_hash_chain(string password,string  character_set,int len_min, int len_max, int index,int chain_length, int chain_number);
string hash_function(string);
string reduction_function(string hashed_password,string char_set,int len_min,int len_max,int index, int step_number);
string NTLM(string key);

//global variables make things easier
int chain_num;
//declared as 1 for default...
int thread_number = 1;


int main(int argc, char **argv){
	
	//following 2 statements for execution time
	struct timeval start, end;
	gettimeofday(&start,NULL);
	
	
	//convert args into vars
	//char *hash_algorithm;
	int plaintext_len_min;	// = 1;
	int plaintext_len_max;	// = 7;
	int table_index;	// = 1;
	int chain_len;		// = 10; 
	int chain_num;		// = 20;
	string character_set;	// = "0123456789";
	
	//if no args use default
	if(argc<2){
		plaintext_len_min = 1;
		plaintext_len_max = 7;
		table_index = 1;
		chain_len = 10; 
		chain_num = 20;
		character_set = "0123456789";	
		thread_number = 1;
	}
	else{
		plaintext_len_min = atoi(argv[1]);
		plaintext_len_max =  atoi(argv[2]);
		table_index =  atoi(argv[3]);
		chain_len =    atoi(argv[4]);
		chain_num =  atoi(argv[5]);
		character_set = argv[6];
		thread_number = atoi(argv[7]);
	}
	
	//output file
	ofstream myfile;
	outputFile = argv[8];
	myfile.open(outputFile);
	if(!myfile){
		cout<<"error"<<endl;
	}	
		
	//this should just be a series of function calls
	
	//create header file.
	myfile<<plaintext_len_min<<" "<<plaintext_len_max<<" "<<table_index<<" "<<chain_len<<" "<<chain_num<<" "<<character_set<<endl;
	
	//close the file for writting - will be opened again later.
	myfile.close();
	
	//pthread array.
	pthread_t thread[thread_number];
	// struct associated with the array
	struct thread_data data[thread_number];
	
	//create hashchain
	//pass it a random password (could be a file/arg passed here??? :D )
	//do this for as many chains as desired
	for(int i = 0; i < thread_number;i++){
		
		//pass values into struct associated with the array	
		data[i].thread_id 	= i;
		data[i].character_set 	= character_set;	
		data[i].len_min		= plaintext_len_min;
		data[i].len_max 	= plaintext_len_max;
		data[i].table_index 	= table_index;
		data[i].chain_length 	= chain_len;
		data[i].chain_number 	= chain_num;

		//create threads
		int rc = pthread_create(&thread[i],NULL,thread_action,(void *) &data[i]);
		if(rc){
			cout<<"ERROR"<<endl;
		}
		else{
			//carry on
		}	
	}	
	
	//barrier sync.
	for(int i = 0; i < thread_number;i++){
		pthread_join(thread[i],NULL);
	}
	//don't think I need this? Remove if everything is fine...
	myfile.close();
	
		
	//for calculating execution time. I could calculate in seconds might be useful...
	gettimeofday(&end,NULL);

	int second_to_microsecond = 1000000;
        int time = ((end.tv_sec * second_to_microsecond + end.tv_usec) - (start.tv_sec * second_to_microsecond + start.tv_usec));
        cout<< "Execution Time in Micro Seconds: " << time<<endl;
	double test = time * (.000001);
	cout<<"Execution Time in Seconds(verify this matches micro): "<<test<<endl;

        return 0;
}

//for determining where thread starts
//if I want to use 8 threads and do N chains each does its own chunck.
//this might not make them equal exactly but it will make sure each has a start 
//point and to get its end point just get the next threads starting point...
int get_index(int a,int chain_num ){
	int index = a * ceil((1.0 * chain_num)/thread_number);
	
	return index;
}

//what each thread does...
//create N/threads chains and write them to the file - thread safely.
void * thread_action(void *struct_data){

	//convert the void * into struct for reading data.
	struct thread_data *data;
        data = (struct thread_data*) struct_data;
	
	int thread_id = data->thread_id;
	string character_set = data->character_set;
	int plaintext_len_min = data->len_min;
	int plaintext_len_max =  data->len_max;
	int table_index = data->table_index;
	int chain_len= data->chain_length;
	int chain_num = data->chain_number;
	
	int start = get_index(thread_id,chain_num);
	int stop = get_index(thread_id+1,chain_num);

	//create N chains.
	for(int i = start; i< stop;i++){
		//this is kinda tricky and neat
		//enforces max_length	
		double max_len = pow(10,plaintext_len_max);
		
		//generate a random password (could be a function??? :D )
                int a = 1001*(table_index)*7*(i+1) % (int)max_len;// pow(10,plaintext_len_max);
                
		//convert int to string
		stringstream ss;
                ss <<a ;
                string str = ss.str();
		//this is for writting to file
                string original_plaintext_password =str;
		//this is for passing to the function. - unnecessary?
                string plaintext_password = original_plaintext_password;
		
		//this actually creates a chain - returns the final hash
		//				  which is all we care about
	
		string final_hashed_password = create_hash_chain(plaintext_password,character_set, plaintext_len_min,plaintext_len_max,table_index,chain_len,chain_num);
		
		//for writting to a file
		ofstream myfile;
		
		//this locks the file for writing (so we don't get anywierd things
		//				   happening here.
		pthread_mutex_lock(&lock);
		
		myfile.open(outputFile,std::ios_base::app);
		if(!myfile){
			cout<<"error"<<endl;
		}

		myfile<<original_plaintext_password<<" "<<final_hashed_password<<endl;
		pthread_mutex_unlock(&lock);
		myfile.close();
	}
	pthread_exit(NULL);
}

//this actually creates a hash chain given parameters.
//process - take plaintext->hash->reduce->repeat N times.
string create_hash_chain( string password,string char_set, int len_min, int len_max, int index,int chain_length, int chain_number){

	//so I'm not always using the parameter... should do/check all these values
	string plaintext_password = password;	
	int max_len = len_max;

	//init hashed_password - this value doesn't matter.
	string hashed_password="0";

	//meat and potatoes of the function.
	//for N chains desired. hash->reduce->repeat.	
	for(int i = 0; i < chain_length ; i++){
		
		hashed_password = hash_function(plaintext_password);	
		
		plaintext_password = reduction_function(hashed_password,char_set,len_min,max_len,index,i);
	}
	
	//return the final hash- I end up reducing this again but not really
	//			 doing anything with that - kinda clever
	return hashed_password;
}


//reduction function... take params mostly hash - and reduce it into plaintext
//return said plaintext
string reduction_function(string hashed_password,string char_set, int len_min,int len_max,int table_index, int step){
	
	
	//because was getting seg faults since len_max was getting changed...
	int max_len = len_max;
	int current_step = step;
	string to_reduce[33];// = hashed_paret_value
	//reduction time!!!
	string character_set(char_set);
	string hash(hashed_password);
	int index_table = table_index;	

	string ret_value(32,'0');//[hash.size()];
	
	for(int i = 0;i<32;i++){
		int flag = 0;
		for(int j = 0;j<10;j++){
			string::size_type loc = hash.find(char_set[j],i);
			
			if(loc != string::npos && flag ==0){
				if(hash[i]==hash[loc]){
					flag = 1;
					ret_value[i]=hash[loc];
					
				}				
			}
		}
		if(flag ==0 ){
			int a = (current_step + index_table) % 10;
						
			//convert to char
			char ch = (a + '0');			
				
			ret_value[i]= ch;
		}
	}
	//32-(0->N) % max_len = (0->max_len varying per step)
	int start = (hash.size()-step) %max_len; 

	//starting point is between = 0 and max_len based on step!!!
	//this particular reduction func. could cause more collisions... but works
	//sorta???
	
	string to_return = ret_value.substr(start,max_len);	

	return to_return;
}

//hash function returns hash -> extendable if I want to add different hashes this
//is where to start.
string hash_function(string plaintext_password){	
	
        string hashed_password = NTLM(plaintext_password);
	return hashed_password;
}


//hash function NTLM (Windows authentication) - DO NOT TOUCH THIS ALG. IT WORKS
//from source: https://www.codeproject.com/articles/328761/ntlm-hash-generator
// only thing I changed was change key into string vs it being a char which was
// causing issues (I like c++ strings..)
string NTLM(string key)
{
	char hex_format[33];
	char itoa16[17] = "0123456789ABCDEF";
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Prepare the string for hash calculation
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	int i = 0;
	//int length = strlen(key);
	int length = key.size();
	memset(nt_buffer, 0, 16*4);
	//The length of key need to be <= 27
	for(; i<length/2; i++)	
		nt_buffer[i] = key[2 * i] | (key[2 * i + 1] << 16);
 	//padding
	if(length % 2 == 1)
		nt_buffer[i] = key[length - 1] | 0x800000;
	else
		nt_buffer[i] = 0x80;
	//put the length
	nt_buffer[14] = length << 4;
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// NTLM hash calculation
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	unsigned int a = INIT_A;
	unsigned int b = INIT_B;
	unsigned int c = INIT_C;
	unsigned int d = INIT_D;
 	/* Round 1 */
	a += (d ^ (b & (c ^ d)))  +  nt_buffer[0]  ;a = (a << 3 ) | (a >> 29);
	d += (c ^ (a & (b ^ c)))  +  nt_buffer[1]  ;d = (d << 7 ) | (d >> 25);
	c += (b ^ (d & (a ^ b)))  +  nt_buffer[2]  ;c = (c << 11) | (c >> 21);
	b += (a ^ (c & (d ^ a)))  +  nt_buffer[3]  ;b = (b << 19) | (b >> 13);
 
	a += (d ^ (b & (c ^ d)))  +  nt_buffer[4]  ;a = (a << 3 ) | (a >> 29);
	d += (c ^ (a & (b ^ c)))  +  nt_buffer[5]  ;d = (d << 7 ) | (d >> 25);
	c += (b ^ (d & (a ^ b)))  +  nt_buffer[6]  ;c = (c << 11) | (c >> 21);
	b += (a ^ (c & (d ^ a)))  +  nt_buffer[7]  ;b = (b << 19) | (b >> 13);
 
	a += (d ^ (b & (c ^ d)))  +  nt_buffer[8]  ;a = (a << 3 ) | (a >> 29);
	d += (c ^ (a & (b ^ c)))  +  nt_buffer[9]  ;d = (d << 7 ) | (d >> 25);
	c += (b ^ (d & (a ^ b)))  +  nt_buffer[10] ;c = (c << 11) | (c >> 21);
	b += (a ^ (c & (d ^ a)))  +  nt_buffer[11] ;b = (b << 19) | (b >> 13);
 
	a += (d ^ (b & (c ^ d)))  +  nt_buffer[12] ;a = (a << 3 ) | (a >> 29);
	d += (c ^ (a & (b ^ c)))  +  nt_buffer[13] ;d = (d << 7 ) | (d >> 25);
	c += (b ^ (d & (a ^ b)))  +  nt_buffer[14] ;c = (c << 11) | (c >> 21);
	b += (a ^ (c & (d ^ a)))  +  nt_buffer[15] ;b = (b << 19) | (b >> 13);
 
	/* Round 2 [movde row back 2 spaces to display it better in editor  */
      a += ((b & (c | d)) | (c & d)) + nt_buffer[0] +SQRT_2; a = (a<<3 ) | (a>>29);
      d += ((a & (b | c)) | (b & c)) + nt_buffer[4] +SQRT_2; d = (d<<5 ) | (d>>27);
      c += ((d & (a | b)) | (a & b)) + nt_buffer[8] +SQRT_2; c = (c<<9 ) | (c>>23);
      b += ((c & (d | a)) | (d & a)) + nt_buffer[12]+SQRT_2; b = (b<<13) | (b>>19);
  
      a += ((b & (c | d)) | (c & d)) + nt_buffer[1] +SQRT_2; a = (a<<3 ) | (a>>29);
      d += ((a & (b | c)) | (b & c)) + nt_buffer[5] +SQRT_2; d = (d<<5 ) | (d>>27);
      c += ((d & (a | b)) | (a & b)) + nt_buffer[9] +SQRT_2; c = (c<<9 ) | (c>>23);      b += ((c & (d | a)) | (d & a)) + nt_buffer[13]+SQRT_2; b = (b<<13) | (b>>19);
 
      a += ((b & (c | d)) | (c & d)) + nt_buffer[2] +SQRT_2; a = (a<<3 ) | (a>>29);
      d += ((a & (b | c)) | (b & c)) + nt_buffer[6] +SQRT_2; d = (d<<5 ) | (d>>27);
      c += ((d & (a | b)) | (a & b)) + nt_buffer[10]+SQRT_2; c = (c<<9 ) | (c>>23);
      b += ((c & (d | a)) | (d & a)) + nt_buffer[14]+SQRT_2; b = (b<<13) | (b>>19);
  
      a += ((b & (c | d)) | (c & d)) + nt_buffer[3] +SQRT_2; a = (a<<3 ) | (a>>29);
      d += ((a & (b | c)) | (b & c)) + nt_buffer[7] +SQRT_2; d = (d<<5 ) | (d>>27);
      c += ((d & (a | b)) | (a & b)) + nt_buffer[11]+SQRT_2; c = (c<<9 ) | (c>>23);
      b += ((c & (d | a)) | (d & a)) + nt_buffer[15]+SQRT_2; b = (b<<13) | (b>>19);
 
	/* Round 3 */
	a += (d ^ c ^ b) + nt_buffer[0]  +  SQRT_3; a = (a << 3 ) | (a >> 29);
	d += (c ^ b ^ a) + nt_buffer[8]  +  SQRT_3; d = (d << 9 ) | (d >> 23);
	c += (b ^ a ^ d) + nt_buffer[4]  +  SQRT_3; c = (c << 11) | (c >> 21);
	b += (a ^ d ^ c) + nt_buffer[12] +  SQRT_3; b = (b << 15) | (b >> 17);
 
	a += (d ^ c ^ b) + nt_buffer[2]  +  SQRT_3; a = (a << 3 ) | (a >> 29);
	d += (c ^ b ^ a) + nt_buffer[10] +  SQRT_3; d = (d << 9 ) | (d >> 23);
	c += (b ^ a ^ d) + nt_buffer[6]  +  SQRT_3; c = (c << 11) | (c >> 21);
	b += (a ^ d ^ c) + nt_buffer[14] +  SQRT_3; b = (b << 15) | (b >> 17);
 
	a += (d ^ c ^ b) + nt_buffer[1]  +  SQRT_3; a = (a << 3 ) | (a >> 29);
	d += (c ^ b ^ a) + nt_buffer[9]  +  SQRT_3; d = (d << 9 ) | (d >> 23);
	c += (b ^ a ^ d) + nt_buffer[5]  +  SQRT_3; c = (c << 11) | (c >> 21);
	b += (a ^ d ^ c) + nt_buffer[13] +  SQRT_3; b = (b << 15) | (b >> 17);

 	a += (d ^ c ^ b) + nt_buffer[3]  +  SQRT_3; a = (a << 3 ) | (a >> 29);
	d += (c ^ b ^ a) + nt_buffer[11] +  SQRT_3; d = (d << 9 ) | (d >> 23);
	c += (b ^ a ^ d) + nt_buffer[7]  +  SQRT_3; c = (c << 11) | (c >> 21);
	b += (a ^ d ^ c) + nt_buffer[15] +  SQRT_3; b = (b << 15) | (b >> 17);

	output[0] = a + INIT_A;
	output[1] = b + INIT_B;
	output[2] = c + INIT_C;
	output[3] = d + INIT_D;
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Convert the hash to hex (for being readable)
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	for(i=0; i<4; i++)
	{
		int j = 0;
		unsigned int n = output[i];
		//iterate the bytes of the integer		
		for(; j<4; j++)
		{
			unsigned int convert = n % 256;
			hex_format[i * 8 + j * 2 + 1] = itoa16[convert % 16];
			convert = convert / 16;
			hex_format[i * 8 + j * 2 + 0] = itoa16[convert % 16];
			n = n / 256;
		}	
	}
	//return hex_format;
	//null terminate the string
	hex_format[32] = 0;
	
	string ret_value(hex_format);
	return ret_value;
}



