//Author: Matthew Fevold 
//Date: 12/10/16
//
//This program is gonna be the shit. - it is shit for now...
//What this program does: takes a hash and a rainbow table to use...
//compares that hash to the endpoints, if any match then reconstruct the chain
//and find the plaintext right before that point.
//
//if it doesn't match, pretend that hash is the 2nd to last hash, run it through
//the reduction and hash alg again. then compare it to the endpoints. if it 
//matches reconstruct the chain and find the plaintext before that point.
//
//repeat this process until you have gone through every length in the chain or 
//you find the plaintext password and output that.


//to compile : g++ -O2 use_table.cpp -o use_table
//to Run: ./use_table HASH_TO_BREAK RAINBOW_TABLE_TO_USE

//should go through these and see if I need all of these.
#include <stdio.h>
#include <fstream>
#include <stdlib.h>
#include <iostream>
#include <string.h>
#include <cstring>
#include <sstream>
#include <cmath>
#include <vector>
#include <algorithm>
#include <map>

//Init values - for lmnt hash - DO NOT TOUCH (maybe put these in the hash function
//						section but be careful and verify
//						if you do)
#define INIT_A 0x67452301
#define INIT_B 0xefcdab89
#define INIT_C 0x98badcfe
#define INIT_D 0x10325476
 
#define SQRT_2 0x5a827999
#define SQRT_3 0x6ed9eba1
 
unsigned int nt_buffer[16];
unsigned int output[4];
//end of stuff for lmnt hash

//to make cout easier... pretty standard using
using namespace std;

//initialize the functions
string use_hash_chain(string hashed_password,string  character_set,int len_min, int len_max, int index,int chain_length, int chain_number);
string hash_function(string);
string reduction_function(string hashed_password,string char_set,int len_min,int len_max,int index, int step_number);
string NTLM(string key);
string search_rainbow_table(string hash_to_search);

//since I am only using 1 rainbow table, it is easier to do it as a global and 
//access it via pointer than it is to pass pointers of the rainbow table(which
//might be quite large) - I have heard of using multiple tables but that is only
//to be attempted after I turn this in and make it my pet project.
std::map<string, string> global_rainbow_table;

int main(int argc, char **argv){

	//read in/open file
	ifstream myfile;
	//read in 2nd argument
	char * inputFile = argv[2];	

	myfile.open(inputFile);
	if(!myfile){
		cout<<"error"<<endl;
	}
	//read in header file
	int a,b,c,d,e;
	string f;
	myfile>>a>>b>>c>>d>>e>>f;
	cout<<"Header File"<<endl;
	cout<<a<<" "<<b<<" "<<c<<" "<<d<<" "<<e<<" "<<f<<endl;
	
	int plaintext_len_min = a;
	int plaintext_len_max = b;
	int table_index = c;
	int chain_len = d; 
	int chain_num = e;
	string character_set = f;
	
	//read in argument for the hash to break(1st arg)
	string hash_to_break = argv[1];
	
	//read out to user what password they use.
	cout<<hash_to_break<<endl;	
	//read in rainbow table

	string u,v;
	while(myfile >>u>>v){
	//v = hash , u = plaintext
	//going like this because hash should be unique	
		global_rainbow_table.insert(std::map<string, string>::value_type(v,u));
	}

	cout<<"SIZE OF RAINBOW TABLE "<<global_rainbow_table.size()<<endl;


	//try and crack the password! This section goes to a funciton where all the
	//really fun stuff happens.
	string cracked_password = use_hash_chain(hash_to_break,character_set,plaintext_len_min,plaintext_len_max,table_index,chain_len,chain_num);

	//use_hash_chain returns NO if no password found, otherwise it returns 
	//the plaintext that corresponds to the hash/
	if(cracked_password.compare("NO")!=0){
		cout<<endl<<"PASSWORD FOUND: "<<cracked_password<<endl;
	}
	else{
		cout<<"Password not found :( "<<endl;
	}
	
}

//search global rainbow table and return the value associated with the hash
//this is not the plaintext of that hash but the first plaintext of that chain
string search_rainbow_table(string hash_to_check){
	
	//instanciate iterator through the map and iterate through it
	std::map<string, string>::const_iterator iter = global_rainbow_table.find(hash_to_check);
	//the final value in the map is unique and if the iterator gets there
	//then your search failed.
	if(iter == global_rainbow_table.end()){
		return "NO";
	}
	else{
	//	cout<<"FOUND : "<<iter->second;
		return iter->second;
	}
}

//this is the prim function of the program.
// it does a lot. will comment on more later...
// returns NO if no password is found else returns the plaintext password
// to note - it is poorly named - should be use_rainbow_table
string use_hash_chain( string password,string char_set, int len_min, int len_max, int index,int chain_length, int chain_number){

	//was having issues... by assigning args into values it fixed them?!? 
	//			wierd...
	string hashed_password = password;	
	int max_len = len_max;	
	
	//this value doesn't matter - it gets rewritten later.
	string plaintext_password="NO";
	

	//if I'm having this many issues with this I could probably
	//count backwards from chain length... check output so that I am not 
	//getting any wierd values...
	if(chain_length = 1){
		string did_find_plaintext = search_rainbow_table(hashed_password);
		if(did_find_plaintext.compare("NO")!=0){
                        //assign that plaintext to the return value
                        plaintext_password = did_find_plaintext;
			return plaintext_password;
		}
	}
	for(int i = (chain_length-1); i > 0 ; i--){
		if(i==1){
			string did_find_plaintext = search_rainbow_table(hashed_password);
			if(did_find_plaintext.compare("NO")!=0){
                        	//assign that plaintext to the return value
                 	       	plaintext_password = did_find_plaintext;
                        	return plaintext_password;	
			}
		}
		
		
		//search current hash (first loop is original) and compare to
		// endpoints in map. it is O(nLog(n)) complexity
		string did_find_plaintext = search_rainbow_table(hashed_password);
		cout<<did_find_plaintext<<endl;	
		//if current hashed_password is found in rainbow table
		//calculate plaintext right before.
		if(did_find_plaintext.compare("NO")!=0){
			//assign that plaintext to the return value
			plaintext_password = did_find_plaintext;
			
		
			cout<<"Found PW"<<endl;	
				//loop i times to find plaintext before hash
			//again inverse i might be easiest.
			for(int j = 0;j<i-1;j++){
				//apply hash on the plaintext ( first loop is 
				//original plaintext.)
				hashed_password = hash_function(plaintext_password);
				//then reduce the hash - if this is done the correct number of times the last time will be the plaintext right before the hash we found in our table.
				//cout<<endl<<hashed_password<<" "<<char_set<<" "<<len_min<<" "<<max_len<<" "<<index<<" "<<j<<endl;

				plaintext_password = reduction_function(hashed_password,char_set,len_min,max_len,index,j);
				
			}	
			//return so we stop looping
			return plaintext_password;
		}	
		//if current hashed is not found...
		// apply reduction (inverse step i) and hash. 
		else{
			
			//apply reduction of this step. might be a mistake to do
			//it this way.
			plaintext_password = reduction_function(hashed_password,char_set,len_min,max_len,index,(i-1));		
		cout<<i-1<<endl;
			
			//apply hash to reduced password
			hashed_password = hash_function(plaintext_password);	
			//if()
		
		}
		//End of loop - Compare again and do all of this again.
	}

	//if we make it here then the whole chain was searched and no password 
	//found. return NO so that we can tell if the return value is correct.
	//	(would returning NULL work?)
	plaintext_password= "NO";
	return plaintext_password;
}


//good old reduction function.
//it takes hash and converts it into a new plaintext to be used in either
//generating the hash chain or to utilize the hash chain.
//	here it is ues to use the hash chain.
string reduction_function(string hashed_password,string char_set, int len_min,int len_max,int table_index, int step){
	
	//don't want to comment this... but I HAVE TO!!!
	//This may be done in a better way - but it has to be done in the
	//create table side of the program. Can I put this function and call it
	//from another file/?? That'd be cool but this works.
	//make sure if changes are made that BOTH functions identical except
	//maybe comments


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
	string to_return = ret_value.substr(start,max_len);	
	
	return to_return;
}

string hash_function(string plaintext_password){	
	
        string hashed_password = NTLM(plaintext_password);

	return hashed_password;
}


//hash function NTLM (Windows authentication)
//from source: https://www.codeproject.com/articles/328761/ntlm-hash-generator
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



