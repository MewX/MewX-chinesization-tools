#include <iostream>
#include <fstream>

using namespace std;

/* Process all:
   for %i in (scr_cn\*) do Noesis_s_decrypto %i scr_dec\%~nxi
*/

int main( int argc, char **argv )
{
	if( argc != 3 ) {
		cerr << "\n    <Noesis s-script-archieve decryptor> By MewCatcher, 2014";
		cerr << "\n    Usage: " << argv[ 0 ] << " <infile.s> <outfile.s>\n";
		return -1;
	}
	
	char *tempFileContent;
	int FileSize;
	fstream in;
	
	in.open( argv[ 1 ], ios::in | ios::binary );
	in.seekg( 0, ios::end );
	FileSize = in.tellg( );
	tempFileContent = new char [ FileSize ];
	in.seekg( 0, ios::beg );
	in.read( tempFileContent, FileSize );
	in.close( );
	
	for( int i = 0; i < FileSize; i ++ ) tempFileContent[ i ] ^= 0xFF;
	
	in.open( argv[ 2 ], ios::out | ios::trunc | ios::binary );
	in.write( tempFileContent, FileSize );
	
	in.close( );
	delete [ ] tempFileContent;
	
	return 0;
}

	
