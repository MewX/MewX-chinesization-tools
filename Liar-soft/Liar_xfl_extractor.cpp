/*
  Name: Liar-soft xfl-archieve extractor
  Copyright: MewCatcher
  Author: MewCatcher
  Date: 31/01/14 19:37
  Description: This program was for game "FOREST".
*/

#include <iostream>
#include <fstream>
#include <string>

using namespace std;

#pragma pack( 1 )


typedef struct {
	char Sign[ 4 ]; // "LB\x01"
	int FileListSize;
	int FileCount;
} ArcInfoStruct;

typedef struct {
	char FileName[ 32 ];
	int Offset; // from 0
	int Size;
} FileInfoStruct;


int main( int argc, char **argv )
{
 	/* Judge opt counts */
	if( argc != 3 ) {
		cerr << "\n    <Liar-soft xfl-archieve extractor> By MewCatcher, 2014";
		cerr << "\n    Usage: " << argv[ 0 ] << " <*.xfl> <TargetFolder>\n";
		return -1;
	}
	
	/* Judge filename */
	fstream ArcFile, tempFile;
	ArcFile.open( argv[ 1 ], ios::binary | ios::in );
	if( !ArcFile.good( ) ) {
		cerr << "\n    Cannot open file: \"" << argv[ 1 ] << "\".\n";
		return -2;
	}
	
	/* Make folder */
	string opt_2 = argv[ 2 ];
	system( ( "md " + opt_2 + " 2>nul" ).c_str( ) );
	
	/* Read all infomation */
	ArcInfoStruct ArcInfo;
	FileInfoStruct *FileInfo;
	ArcFile.read( (char *)(&ArcInfo), sizeof( ArcInfoStruct ) );
	if( strcmp( ArcInfo.Sign, "LB\x01" ) ) {
		cerr << "\n    Error in signature, file: \"" << argv[ 1 ] << "\".\n";
		return -3;
	}
	FileInfo = new FileInfoStruct [ ArcInfo.FileCount ];
	ArcFile.read( (char *)(&FileInfo[ 0 ]),
				  sizeof( FileInfoStruct ) * ArcInfo.FileCount );
	
	/* Extract all */
	char *temp;
	for( int i = 0; i < ArcInfo.FileCount; i ++ ) {
		/* Set file pointer */ 
		ArcFile.seekg( sizeof( ArcInfoStruct ) + ArcInfo.FileListSize + FileInfo[ i ].Offset, ios::beg );
		
		/* Read file content */ 
		temp = new char [ FileInfo[ i ].Size ];
		ArcFile.read( temp, FileInfo[ i ].Size );
		
		/* Open target file */
		tempFile.open( ( opt_2 + "\\" + FileInfo[ i ].FileName ).c_str( ),
					   ios::out | ios::trunc | ios::binary );
		if( !tempFile.good( ) ) {
			cerr << "\n    Cannot open file to write: \""
				 << FileInfo[ i ].FileName << "\".\n";
			return -4;
		}
		tempFile.write( temp, FileInfo[ i ].Size );
		
		/* Clean memory */
		tempFile.close( );
		delete [ ] temp;
	}
	
	ArcFile.close( );
		
	return 0;
}
