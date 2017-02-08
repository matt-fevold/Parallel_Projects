//This program is gonna be the shit.


//to compile : g++ -O0 create_table.cpp -o create_table

#include <stdio.h>
#include <fstream>
#include <stdlib.h>
#include <iostream>
#include <string.h>
#include <cstring>
#include <sstream>

//Init values - for lmnt hash
#define INIT_A 0x67452301
#define INIT_B 0xefcdab89
#define INIT_C 0x98badcfe
#define INIT_D 0x10325476
 
#define SQRT_2 0x5a827999
#define SQRT_3 0x6ed9eba1
 
unsigned int nt_buffer[16];
unsigned int output[4];
char hex_format[33];
char itoa16[17] = "0123456789ABCDEF";
//char *test = "fuck";

using namespace std;

//
string create_hash_chain(string password,string  character_set,int len_min, int len_max, int index,int chain_length, int chain_number);
string hash_function(string);
string reduction_function(string hashed_password,string char_set,int len_min,int len_max,int index, int step_number);
void NTLM(string key);
string global_plaintext_password;
string global_hashed_password;

//function - hash_algorithm charset plaintext_len_min plaintext_len_max
//table_index chain_len chain_num part_index 

int main(int argc, char **argv){
	//convert args into vars
	//char *hash_algorithm;
	int plaintext_len_min = 1;
	int plaintext_len_max = 5;
	int table_index = 1;
	int chain_len = 3; 
	int chain_num = 20;
	string character_set = "0123456789";	

	//char *filepath = "~/git-repos/projects/parallel/final/finaloutput.txt";
	ofstream myfile;

	myfile.open("rainbow_table.txt");
	if(!myfile){
		cout<<"error"<<endl;
	}
		
	//this should just be a series of function calls
	
	//create hashchain
	//pass it a random password (could be a file/arg passed here??? :D )
	//do this for as many chains as desired
	//int seed = table_index;
	//char *initial_plaintext_password = (char *) ((1234567877777777+seed)%(10*plaintext_len_max));
	for(int i = 0; i < chain_num;i++){
		//random starting password
		//int test = ((100000+(table_index+i)*17)%(10*plaintext_len_max));
		
		int a = 1234+table_index*7*i;
		stringstream ss;
		ss <<a ;
		string str = ss.str();	
		
		string plaintext_password =str;  //(char ) ((100000+table_index)%(10*plaintext_len_max));
		//write first password into file
		myfile <<plaintext_password<< " , ";

		//create hash chains!
		string final_hashed_password = create_hash_chain(plaintext_password,character_set, plaintext_len_min,plaintext_len_max,table_index,chain_len,chain_num);

		myfile <<final_hashed_password<<endl;
	}	

	myfile.close();
}
string create_hash_chain( string password,string char_set, int len_min, int len_max, int index,int chain_length, int chain_number){
	//first turn password into hash
	global_plaintext_password = password;
	//hashed_password;
	cout<<endl<<len_max<<endl;
	int max_len = len_max;
	for(int i = 0; i < chain_length ; i++){
		//for as many links as desired, hash and reduce in a loop
		/*hashed_password =*/ hash_function(global_plaintext_password);
				
		/*plaintext_password =*/ reduction_function(global_hashed_password,char_set,len_min,max_len,index,i);	
		//plaintext_password = global_plaintext_password;
	}
	
	return global_hashed_password;
}



string reduction_function(string hashed_password,string char_set, int len_min,int len_max,int index, int step){
	
	//cout<<endl<<"::: "<<len_max<<endl;
	//because was getting seg faults since len_max was getting changed...
	int max_len = len_max;
	int current_step = step;
	string to_reduce[33];// = hashed_password; 
	for(int i = 0;i<32;i++){
		to_reduce[i] = hashed_password[i];
	}
	//reduction time!!!
	string character_set(char_set);
	string hash(hashed_password);
	//cout <<character_set<<endl;
	//cout<<endl<<to_reduce[1].find(character_set)<<endl;
	//random length between min and max???	
	//cout<<endl<<max_len<<endl;
	//cout<<endl<<hash.find_first_of("c")<<endl;
	
	//cout<<hash.size()<<endl;
	char ret_value[hash.size()];
	cout<<hash<<endl<<endl;
	for(int i = 0;i<32;i++){
		int flag = 0;
		for(int j = 0;j<10;j++){
			string::size_type loc = hash.find(char_set[j],i);
			//int flag = 0;
			if(loc != string::npos && flag ==0){
				if(hash[i]==hash[loc]){
				//	cout<<hash[i]<< " : "<<hash[loc] <<" :D "<<endl;
					flag = 1;
					ret_value[i]=hash[loc];
					
				}				
			}
		}
		
		if(flag ==0 ){
			//cout<<index<<" *** "<<step<<endl;
			ret_value[i]=((step+1)*(index+1))%2;
		}

	}
	for(int k = 0; k<32;k++)
		cout<<ret_value[k];
	//string testers(ret_value);
	//cout<<endl<<testers<<endl;
	cout<<endl;
	//int test = 0;	
	//for(int k = 0;k<len_min;k++)
		global_plaintext_password = ret_value;
	return ret_value;

}

string hash_function(string plaintext_password){
	//char * test = "asdf";
	string test ;
	char * plain;// = plaintext_password;
	for(int i = 0; i<plaintext_password.size()-1;i++){
		//plain[i] = plaintext_password[i];
	}
	
	NTLM(plaintext_password);
        test = hex_format;
	global_hashed_password = hex_format;
	return test;
}


//hash function NTLM (Windows authentication)
void NTLM(string key)
{
	//char hex_format[33];
	
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
	hex_format[33] = 0;
	//return &(* hex_format);
}



