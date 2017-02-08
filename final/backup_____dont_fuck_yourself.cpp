//This program is gonna be the shit.


//to compile : g++ -O0 create_table.cpp -o create_table

#include <stdio.h>
#include <fstream>
#include <stdlib.h>
#include <iostream>
#include <string.h>
#include <cstring>
#include <sstream>
#include <cmath>
#include <time.h>
#include <sys/time.h>
#include <pthread.h>


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
pthread_mutex_t lock;
using namespace std;

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

//
int get_index(int a);
string create_hash_chain(void *s);
//string create_hash_chain(string password,string  character_set,int len_min, int len_max, int index,int chain_length, int chain_number);
string hash_function(string);
string reduction_function(string hashed_password,string char_set,int len_min,int len_max,int index, int step_number);
string NTLM(string key);
//string global_plaintext_password;
//string global_hashed_password;

//function - hash_algorithm charset plaintext_len_min plaintext_len_max
//table_index chain_len chain_num part_index 
int chain_num;
int thread_number = 1;


int main(int argc, char **argv){
	
	struct timeval start, end;
	gettimeofday(&start,NULL);
	
	//convert args into vars
	//char *hash_algorithm;
	int plaintext_len_min;// = 1;
	int plaintext_len_max;// = 7;
	int table_index;// = 1;
	int chain_len;// = 10; 
	//int chain_num;// = 20;
	string character_set;// = "0123456789";
	//int thread_number = 1;
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
		//cout<<character_set<<endl;
	}
	
	//char *filepath = "~/git-repos/projects/parallel/final/finaloutput.txt";
	ofstream myfile;

	myfile.open("rainbow_table.txt");
	if(!myfile){
		cout<<"error"<<endl;
	}
		
	//this should just be a series of function calls
	
	myfile<<plaintext_len_min<<" "<<plaintext_len_max<<" "<<table_index<<" "<<chain_len<<" "<<chain_num<<" "<<character_set<<endl;
	
	pthread_t thread[thread_number];
	struct thread_data data[thread_number];
	//create hashchain
	//pass it a random password (could be a file/arg passed here??? :D )
	//do this for as many chains as desired
	for(int i = 0; i < thread_number;i++){
		//random starting password
		
		//int to string convert
		double max_len = pow(10,plaintext_len_max);
		int a = 1001*(table_index)*7*(i+1) % (int)max_len;// pow(10,plaintext_len_max);
		stringstream ss;
		//cout<<a<<endl<<endl;
		ss <<a ;
		string str = ss.str();	
		string original_plaintext_password =str;
		string plaintext_password = original_plaintext_password;
	//cout<<endl<<"NEW CHAIN : Password :"<< plaintext_password <<endl;	
		//write first password into file
		//pthread_mutex_lock(&lock);
		//myfile <<plaintext_password<< " ";
		//pthread time!!!
		data[i].thread_id 	= i;
		data[i].password 	= plaintext_password;
		data[i].character_set 	= character_set;	
		data[i].len_min		= plaintext_len_min;
		data[i].len_max 	= plaintext_len_max;
		data[i].table_index 	= table_index;
		data[i].chain_length 	= chain_len;
		data[i].chain_number 	= chain_num;

		int rc = pthread_create(&thread[i],NULL,create_hash_chain,(void *) &data[i]);
		if(rc){
			cout<<"ERROR"<<endl;
		}
		else{
			cout<<"thread created"<<endl;
		}
		
		//create hash chains!
		//string final_hashed_password = create_hash_chain(plaintext_password,character_set, plaintext_len_min,plaintext_len_max,table_index,chain_len,chain_num);
		pthread_mutex_lock(&lock);
		
		myfile <<original_plaintext_password<<" "<<final_hashed_password<<endl;
		pthread_mutex_unlock(&lock);
	}	
	
	for(int i = 0; i < thread_number;i++){
		//pthread_join(
	}
	myfile.close();
	
		

	gettimeofday(&end,NULL);

	int second_to_microsecond = 1000000;
        int time = ((end.tv_sec * second_to_microsecond + end.tv_usec) - (start.tv_sec * second_to_microsecond + start.tv_usec));
        cout<< "Execution Time in Micro Seconds: " << time<<endl;

        return 0;


	
}

//for determining where thread starts
int get_index(int a ){
	int index = a * ceil((1.0 * chain_num)/thread_number);
	
	return index;

}


//string create_hash_chain( string password,string char_set, int len_min, int len_max, int index,int chain_length, int chain_number)
	
string create_hash_chain(void *arg){
	struct thread_data *data;
	data = (struct thread_data*) arg;

	
/*	
	//first turn password into hash
	//global_plaintext_password = password;
	string plaintext_password = password;	
		

	string hashed_password="0";
	


	int max_len = len_max;
	
	for(int i = 0; i < chain_length ; i++){
		//for as many links as desired, hash and reduce in a loop
		hashed_password = hash_function(plaintext_password);	
		
		plaintext_password = reduction_function(hashed_password,char_set,len_min,max_len,index,i);
}*/
	return "1234";
	//return hashed_password;
}



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
	int start = (hash.size()-step) %max_len; 

	//starting point is between = 0 and max_len based on step!!!
	//this particular reduction func. could cause more collisions... but works

	
	string to_return = ret_value.substr(start,max_len);	
//	cout<<to_return<<endl<<endl;		
	return to_return;
}

string hash_function(string plaintext_password){	
	
	//NTLM(plaintext_password);
        string hashed_password = NTLM(plaintext_password);
//	cout<<hashed_password<<endl;
	return hashed_password;
}


//hash function NTLM (Windows authentication)
//from source: give credit!!!
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
 
	/* Round 2 */
	a += ((b & (c | d)) | (c & d)) + nt_buffer[0] +SQRT_2; a = (a<<3 ) | (a>>29);
	d += ((a & (b | c)) | (b & c)) + nt_buffer[4] +SQRT_2; d = (d<<5 ) | (d>>27);
	c += ((d & (a | b)) | (a & b)) + nt_buffer[8] +SQRT_2; c = (c<<9 ) | (c>>23);
	b += ((c & (d | a)) | (d & a)) + nt_buffer[12]+SQRT_2; b = (b<<13) | (b>>19);
 
	a += ((b & (c | d)) | (c & d)) + nt_buffer[1] +SQRT_2; a = (a<<3 ) | (a>>29);
	d += ((a & (b | c)) | (b & c)) + nt_buffer[5] +SQRT_2; d = (d<<5 ) | (d>>27);
	c += ((d & (a | b)) | (a & b)) + nt_buffer[9] +SQRT_2; c = (c<<9 ) | (c>>23);
	b += ((c & (d | a)) | (d & a)) + nt_buffer[13]+SQRT_2; b = (b<<13) | (b>>19);
 
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
//	cout<<ret_value<<endl;
	return ret_value;
}



