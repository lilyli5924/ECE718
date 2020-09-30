//============================================================================
// Name        : cachesim.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

// Didn't get a chance to implement argument since I am part-time student still need to work
// Here I have made the input to be read from here.
#define BLOCK_SIZE 32
#define CACHE_SIZE 16384
#define ASSOCIATIVITY 1
#define WRITE_POLICY 'b'

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstdlib>
#include <cmath>
#include <iomanip>
using namespace std;

typedef struct {
	int tag;
	bool valid;
	bool dirty;
	int age;
} Blk;

typedef struct {
	Blk *block;
} Set;


void updateAge(Set *set_in, int set_num, int cur_set) {
	int n;
	// Update the age for remaining blocks age
	for (n=0; n<set_num; n++) {
		set_in->block[n].age += 1;
	}
	// Make the current used block age = 0
	set_in->block[cur_set].age = 0;
}


int binToDecimal (string value)
{
	int cnt;
	int decimal = 0;
	int power = value.length() - 1;
	for (cnt=0; cnt<value.length(); cnt++) {
		if (value[cnt] == '1') {
			decimal = decimal + pow(2.0, power);
		}
		else {
			decimal = decimal + 0;
		}
		power--;
	}
	return decimal;
}

string hexTable(char value) {
	switch (value) {
		case '0':
			return "0000";
			break;
		case '1':
			return "0001";
			break;
		case '2':
			return "0010";
			break;
		case '3':
			return "0011";
			break;
		case '4':
			return "0100";
			break;
		case '5':
			return "0101";
			break;
		case '6':
			return "0110";
			break;
		case '7':
			return "0111";
			break;
		case '8':
			return "1000";
			break;
		case '9':
			return "1001";
			break;
		case 'a':
			return "1010";
			break;
		case 'b':
			return "1011";
			break;
		case 'c':
			return "1100";
			break;
		case 'd':
			return "1101";
			break;
		case 'e':
			return "1110";
			break;
		case 'f':
			return "1111";
			break;
		default :
			cout << "error";
			break;
	}
    return "";
}

string hexToBinaryConv(string hex_value) {
    char hex_value_char = 'g';
    string converted_hex = "";
    for (int i = 0; i < hex_value.length(); i++) {
        hex_value_char = hex_value[i];
        converted_hex = converted_hex + hexTable(hex_value_char);
    }
    return converted_hex;
}


int main(int argc, char **argv) {

	// Input
	int associative = ASSOCIATIVITY;					// i.e. 4-way set associative
	int block_size = BLOCK_SIZE; 						// i.e. 32 Bytes
	int cache_size = CACHE_SIZE;						// i.e. 16384 Bytes
	char write_policy = WRITE_POLICY;					// Either 't' or 'b'

	int i, j, k, n;
	int num_set = 0;
	int num_block = 0;
	int tag_size = 0;
	int offset_size = 0;
	int set_size = 0;

	// Calculations
	num_set = cache_size / (block_size * associative);
	num_block = num_set * associative;
	set_size  = ceil(log(num_set) / log(2));
	offset_size    = ceil(log(block_size) / log(2));
	tag_size       = 24 - set_size - offset_size;

	// Dynamic Memory Allocation of a n-way set associative cache
	Set *sets;
	Set *curSet;

	// Dynamic memory allocation
	sets = (Set *)malloc(num_set * sizeof(Set));
	for(i=0; i<num_set; i++) {
		sets[i].block = (Blk *)malloc(associative*sizeof(Blk));
	}

	// OUTPUT STATEMENT
	cout << cache_size << " Bytes " << associative << "-way associative cache:" << endl;
	cout << "    Block size = " << block_size << " bytes" << endl;
	cout << "    Number of [sets,blocks] = [" << num_set << "," << num_block << "]" << endl;
	cout << "    Extra space for tag storage = 640 bytes ( 3.91%)" << endl;
	cout << "    Bits for [tag,index,offset] = [" << tag_size << ", " << set_size << ", " << offset_size << "]" << endl;
	if (write_policy == 't') {
		cout << "    Write policy = Write-through" << endl;
	}
	else {
		cout << "    Write policy = Write-back" << endl;
	}
	cout << endl;

	cout << "Hex     Binary	  Address                 Set  Blk            Memory" << endl;
	cout << "Address (tag/index/offset)           Tag Index off  Way UWay Read Writ" << endl;
	cout << "====================================================================" << endl;

	string line = "";
	string mode = "";
	string addr = "";
	string addr_bin = "";
	string append = "";
	string tag = "";
	string set = "";
	string offset = "";
	int tag_dec = 0;
	int set_dec = 0;
	int offset_dec = 0;

	int hit = 0;
	int miss = 0;
	int num_write = 0;
	int num_read = 0;

	int max;
	int max_idx;
	int count_ref = 0;
	int count_r = 0;
	int count_w = 0;

	bool flag = false;
	bool read_flag = false;
	bool write_flag = false;
	bool flag_inner = false;

	int way = 0;

	ifstream memtrace_file ("../cachesim/src/ex_trace.trc");

	if (memtrace_file.is_open()) {
		while (getline(memtrace_file,line)) {
			string input = line;
			for (int k=0; k<input.length(); k++) {
				if (k == 0) {
					mode = input[k];
				}
				else if (k == 1) {
					continue;
				}
				else {
					addr = addr + input[k];
				}
			}
			addr_bin = hexToBinaryConv(addr);
			for (int n=0; n<(24-addr_bin.length()); n++) {
				append += "0";
			}
			addr_bin = append + addr_bin;
			for (k=0; k<addr_bin.length(); k++) {
			    if (k < tag_size) {
			    	tag = tag + addr_bin[k];
			    }
			    else if (k>=tag_size && k<(tag_size+set_size)) {
			    	set = set + addr_bin[k];
			    }
			    else {
			    	offset = offset + addr_bin[k];
			    }
			}
			tag_dec = binToDecimal(tag);
			set_dec = binToDecimal(set);
			offset_dec = binToDecimal(offset);

			curSet = &sets[set_dec];

			// Counters
			count_ref++;
			if(mode == "W") {
				count_w++;
			}else {
				count_r++;
			}

			// My intepretation: when read you don't care since when hit it's read you don't update the data
			// but for write when a hit happens you still UPDATING the block data!!!
			// that's why the 4th entry is still a miss, since the data now is different even though the tag is there
			if (write_policy == 't') {
				if (mode == "W") {
					max = 0;
					max_idx = 0;
					flag = false;
					read_flag = false;
					write_flag = false;
					way = false;

					num_write++;
					write_flag = true;

					for (i=0; i<associative; i++) {
						if (curSet->block[i].tag == tag_dec) {
							if (curSet->block[i].valid) {
								hit++;
								way = i;

								// Still want to update data in the cache (write-through: force to write to memory)
								curSet->block[i].tag = (tag_dec + offset_dec);
								updateAge(curSet, associative, i);
								flag = true;
								break;
							}
						}
					}
					if (!flag) {
						miss++;
						way = -1;
						flag_inner = false;

						// If there is still empty cache block to write
						for(i=0; i<associative; i++) {
							if (!curSet->block[i].valid) {
								curSet->block[i].valid = true;
								curSet->block[i].tag = (tag_dec + offset_dec);
								updateAge(curSet, associative, i);

								flag_inner = true;
								break;
							}
						}

						if (!flag_inner) {
							// If all blocks within set are taken, find the least used block (LRU)
							for (n=0; n<associative; n++) {
								if (curSet->block[n].age > max) {
									max = curSet->block[n].age;
									max_idx = n;
								}
							}
							// Update the tag to least used block
							curSet->block[max_idx].tag = (tag_dec + offset_dec);
							updateAge(curSet, associative, max_idx);
						}
					}
				}
				else if (mode == "R"){
					// READ
					max = 0;
					max_idx = 0;
					read_flag = false;
					write_flag = false;
					flag = false;
					way = false;

					for(i=0; i<associative; i++) {
						if (curSet->block[i].tag == tag_dec) {
							if (curSet->block[i].valid) {
								hit++;
								way = i;
								updateAge(curSet, associative, i);

								flag = true;
								break;
							}
						}
					}
					if (!flag) {
						miss++;
						way = -1;
						flag_inner = false;

						num_read++;
						read_flag = true;

						// If there are still space in the cache set block
						for(i=0; i<associative; i++) {
							if (!curSet->block[i].valid) {
								curSet->block[i].valid = true;
								curSet->block[i].tag = tag_dec;
								updateAge(curSet, associative, i);

								flag_inner = true;
								break;
							}
						}

						if (!flag_inner) {
							// If all blocks already have data
							for (n=0; n<associative; n++) {
								if (curSet->block[n].age > max) {
									max = curSet->block[n].age;
									max_idx = n;
								}
							}
							// Update the tag to least used block
							curSet->block[max_idx].tag = tag_dec;
							updateAge(curSet, associative, max_idx);
						}
					}
				}
			}
			else {  // Write-back
				if (mode == "W") {
					max = 0;
					max_idx = 0;
					flag = false;
					read_flag = false;
					write_flag = false;
					way = false;

					for (i=0; i<associative; i++) {
						if (curSet->block[i].tag == tag_dec) {
							if (curSet->block[i].valid) {
								hit++;
								way = i;

								// Set the dirty bit to indicate that the data in cache != memory, but do not write to memory
								curSet->block[i].tag = tag_dec;
								curSet->block[i].dirty = ~curSet->block[i].dirty;
								updateAge(curSet, associative, i);

								flag = true;
								break;
							}
						}
					}
					if (!flag) {
						miss++;
						// When misses, need to read from the memory and update the data
						num_read++;
						way = -1;
						flag_inner = false;

						// If there is still empty cache block to write
						for(i=0; i<associative; i++) {
							if (!curSet->block[i].valid) {
								curSet->block[i].valid = true;
								curSet->block[i].tag = tag_dec;

								// Block modified, update the dirty bit
								curSet->block[i].dirty = ~curSet->block[i].dirty;

								updateAge(curSet, associative, i);

								flag_inner = true;
								break;
							}
						}

						if (!flag_inner) {
							// If all blocks within set are taken, find the least used block
							for (n=0; n<associative; n++) {
								if (curSet->block[n].age > max) {
									max = curSet->block[n].age;
									max_idx = n;
								}
							}

							// Only update memory write if the cache set needs to be replaced (sets are full)
							if(curSet->block[max_idx].dirty) {
								num_write++;
								write_flag = true;
							}

							// Block modified, update the dirty bit
							curSet->block[i].dirty = ~curSet->block[i].dirty;

							// Update the tag to least used block
							curSet->block[max_idx].tag = tag_dec;
							updateAge(curSet, associative, max_idx);
						}
					}
				}
				else if (mode == "R") {
					// READ
					max = 0;
					max_idx = 0;
					flag = false;
					read_flag = false;
					write_flag = false;
					way = false;


					for(i=0; i<associative; i++) {
						if (curSet->block[i].tag == tag_dec) {
							if (curSet->block[i].valid) {
								hit++;
								way = i;
								updateAge(curSet, associative, i);

								flag = true;
								break;
							}
						}
					}
					if (!flag) {
						miss++;
						num_read++;
						read_flag = true;
						way = -1;
						flag_inner = false;

						// If there are still space in the cache set block
						for(i=0; i<associative; i++) {
							if (!curSet->block[i].valid) {
								curSet->block[i].valid = true;
								curSet->block[i].tag = tag_dec;

								// Memory updated, clear the dirty bit
								curSet->block[i].dirty = ~curSet->block[i].dirty;

								updateAge(curSet, associative, i);
								flag_inner = true;
								break;
							}
						}

						if (!flag_inner) {
							// If all blocks already have data
							for (n=0; n<associative; n++) {
								if (curSet->block[n].age > max) {
									max = curSet->block[n].age;
									max_idx = n;
								}
							}

							// Only update memory write if the cache set needs to be replaced (sets are full)
							if(curSet->block[max_idx].dirty) {
								num_write++;
								write_flag = true;
							}

							// Memory updated, clear the dirty bit
							curSet->block[i].dirty = ~curSet->block[i].dirty;

							// Update the tag to least used block
							curSet->block[max_idx].tag = tag_dec;
							updateAge(curSet, associative, max_idx);
						}
					}
				}
			}

			cout << setiosflags(ios::right) << setw(6) << addr << mode << " ";
			cout << tag << " " << set << " " << offset << " ";
			cout << setiosflags(ios::right) << setw(4) << tag_dec << " ";
			cout << setiosflags(ios::right) << setw(4) << set_dec << " ";
			cout << setiosflags(ios::right) << setw(4) << offset_dec << " ";
			cout << setiosflags(ios::right) << setw(4) << way << " ";
			cout << setiosflags(ios::right) << setw(4) << read_flag << " ";
			cout << setiosflags(ios::right) << setw(4) << write_flag << endl;


			// Reset the two parameters
			mode = "";
			addr = "";
			addr_bin = "";
			append = "";
			tag = "";
			set = "";
			offset = "";
			tag_dec = 0;
			set_dec = 0;
			offset_dec = 0;

		}
		memtrace_file.close();
	}
	else {
		cout << "Unable to open memtrace file";
	}



	double hit_ratio = 0.00;
	double miss_ratio = 0.00;

	hit_ratio = (double)hit / (double)(hit + miss);
	miss_ratio = 1 - (double)hit_ratio;



	cout << setprecision(2) << endl;
	cout << "nref=" << count_ref << ", " << "nread=" << count_r << ", " << "nwrite=" << count_w << endl;
	cout << "hits 	= " << hit << ", " << "hit rate = " << hit_ratio << endl;
	cout << "misses	= " << miss << ", " << "miss rate = " << miss_ratio << endl;
	cout << "main memory reads=" << num_read << ", " << "main memory writes=" << num_write << endl;


	for(i=0; i<num_set; i++) {
		free(curSet->block);
	}
	free(sets);

	return 0;
}


