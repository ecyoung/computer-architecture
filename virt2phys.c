#include <stdio.h>
#include <stdlib.h>
#include <math.h> 

// initialize file
FILE *fr;

// declare helper functions
int log2(int n) {
	int r=0;
	while (n>>=1) r++;
	return r;
}

int ones(int N) {
	return ((1<<N)-1);
}

// main function
int main (int argc, char *argv[]) { 
	// read page table file 
	fr = fopen(argv[1], "r");
	// get virtual address 
	int virtualAddress = strtol(argv[2], NULL, 16); // convert hexstring to int
	// get address bits and page size 
	int addressBits; 
	fscanf(fr, "%d", &addressBits); // scan and save address bits value
	// calculate number of virtual pages 
	int pageSize; 
	fscanf(fr, "%d", &pageSize); // scan and save page size value
	// calculate number of virtual pages 
	int virtualPages = pow(2, addressBits) / pageSize; // 2^addressBits / pageSize
	// initialize array of ppn values 
	int ppnArray[virtualPages];
	// map each vpn to ppn and store all entries in ppnArray
	for (int i = 0; i < virtualPages; i++){
		fscanf(fr, "%d", &ppnArray[i]);
	}
	// calculate vpn bits and offset bits
	int vpnBits = log2(virtualPages);
	int offsetBits = addressBits - vpnBits; 
	// calculate vpn
	int vpn = virtualAddress >> offsetBits;
	// calculate ppn corresponding to its vpn
	int ppn = ppnArray[vpn];
	// if the page table indicates the given address has no valid physical page
	if (ppn == -1){
		printf("PAGEFAULT\n");
		fclose(fr);
		return EXIT_SUCCESS;
	}
	// vpn-ppn conversion: keep offset bits and replace upper bits of virtual address with ppn 
	int output = ((ones(addressBits) >> vpnBits) & virtualAddress) | (ppn << offsetBits); 
	printf("%x\n", output);
	fclose(fr);
	return EXIT_SUCCESS;
}  