/*
  Name: Noesis iga-archieve packer
  Copyright: MewCatcher
  Author: MewCatcher
  Date: 01/02/14 09:08
  Description: This program is for "Cure Girl"
  Reference: Asmodean's "exiga"
*/

#include <iostream>
#include <fstream>
#include <string>

using namespace std;

#pragma pack( 1 )

/** < Arc Structure >
 *  ArcInfo          * 1
 *  FileInfo_len     * 1
 *  FileInfo         * n
 *  FileNameList_len * 1
 *  FileName         * n
 *  FileContent      * n
 */

 
typedef struct IGAHDR {
  char Sign[ 4 ]; // "IGA0"
  unsigned long unknown1; // May filename_offset?  database_offset? file_count? header_size? Checksum?
  unsigned long unknown2;
  unsigned long unknown3;
} ArcInfoStruct;

typedef struct IGAENTRY {
  unsigned long FileName_Offset;
  unsigned long Content_Offset;
  unsigned long FileSize;
  string FileName;            // I added
  unsigned long FileName_len; // I added
} FileInfoStruct;


unsigned int put_multibyte_long( unsigned char *buff, unsigned long src )
{
    /* This is a storage-saving method! */
    unsigned int count = 1;
    if( src == 0 ) *buff = 0x01;
    else {
        count = 0; 
        unsigned int i = 0;
        unsigned char temp[ 4 ] = { 0 };
        src = ( src << 1 ) | 1;
        while( src ) {
            temp[ i ++ ] = (unsigned char)src & 0xFE;
            count ++;
            ( src >> 7 ) == 1 ? src >>= 8 : src >>= 7;
        }
        while( i ) *buff++ = temp[ -- i ];
        *( buff - 1 ) |= 1;
    }
    return count; // return count of bytes
}


int main( int argc, char **argv )
{
     /* Judge opt counts */
    if( argc != 3 ) {
        cerr << "\n    <Noesis iga-archieve packer> By MewCatcher, 2014";
        cerr << "\n    Usage: " << argv[ 0 ] << " <SourceFolder> <*.iga>\n";
        return -1;
    }
    
    
    /* Command Line */
    string opt_temp = argv[ 1 ];
    system( "md temp 2>nul" );
    system( ( "del temp\\" + opt_temp + ".lst 2>nul" ).c_str( ) );
    system( ( "dir " + opt_temp + " /B /A-D | find /v /c \":\" >> temp\\" + opt_temp + ".lst" ).c_str( ) );
    system( ( "dir " + opt_temp + " /A-D /B >> temp\\" + opt_temp + ".lst" ).c_str( ) );
    
    
    /* Initial */
    string tempFileName;
    unsigned int FileCount, tempFileSize, tempFileName_len;
    char *tempFileContent, *FileNameArray, *FileInfoArray;
    fstream ListFile, tempFile, TargFile;
    
    ListFile.open( ( "temp\\" + opt_temp + ".lst" ).c_str( ), ios::in );
    TargFile.open( argv[ 2 ], ios::out | ios::trunc | ios::binary );
    ListFile >> FileCount; // read file count
    
    ArcInfoStruct ArcInfo;    memset( &ArcInfo, 0, sizeof( ArcInfoStruct ) );
    FileInfoStruct *FileInfo; FileInfo = new FileInfoStruct [ FileCount ];
    
    
    /* Read FileInfo ( FileName && FileSize ) */
    for( unsigned int i = 0; i < FileCount; i ++ ) {
        ListFile >> FileInfo[ i ].FileName;
        tempFile.open( ( opt_temp + "\\" + FileInfo[ i ].FileName).c_str( ), ios::in | ios::binary );
        tempFile.seekg( 0, ios::end );
        FileInfo[ i ].FileSize = tempFile.tellg( );
        tempFile.close( );
    }
    ListFile.close( );
    
    
    /* Calc all FileInfo */
    unsigned int pFileNameArray = 0;
    FileNameArray = new char [ FileCount * 20 ]; // Large enough
    FileInfo[ 0 ].FileName_Offset = 0; // FileName_Offset from 0
    FileInfo[ 0 ].Content_Offset  = 0; // Content_Offset  from 0
    for( int j = 0; j < FileInfo[ 0 ].FileName.length( ); j ++ ) {
        tempFileName_len = put_multibyte_long(
                               (unsigned char *)&FileNameArray[ pFileNameArray ],
                               FileInfo[ 0 ].FileName[ j ] );
        pFileNameArray += tempFileName_len;
    }
    FileInfo[ 0 ].FileName_len = pFileNameArray; // Calc 1st data
    
    for( int i = 1; i < FileCount; i ++ ) {
        FileInfo[ i ].FileName_Offset = FileInfo[ i - 1 ].FileName_Offset
                                      + FileInfo[ i - 1 ].FileName_len;
        FileInfo[ i ].Content_Offset  = FileInfo[ i - 1 ].Content_Offset
                                      + FileInfo[ i - 1 ].FileSize;
                                      
        for( int j = 0; j < FileInfo[ i ].FileName.length( ); j ++ ) {
            tempFileName_len = put_multibyte_long(
                                   (unsigned char *)&FileNameArray[ pFileNameArray ],
                                   FileInfo[ i ].FileName[ j ] );
            pFileNameArray += tempFileName_len;
        }
        FileInfo[ i ].FileName_len = pFileNameArray - FileInfo[ i ].FileName_Offset;
    } // calc the rest, fill FileNameArray
    
    
    /* Make header */
    strcpy( ArcInfo.Sign, "IGA0" );
    ArcInfo.unknown1 = 0; // ??????????????????????????????????????????????????????
    ArcInfo.unknown2 = 2;
    ArcInfo.unknown3 = 2;
    TargFile.write( (char *)&ArcInfo, sizeof( ArcInfoStruct ) );     // (1) ArcInfo
    
    unsigned char FileInfoSizeConverted[ 4 ] = { 0 };
    unsigned int pFileInfoArray = 0; // == FileInfoSize
    FileInfoArray = new char [ sizeof( FileInfoStruct ) * FileCount ]; // Large enough
    for( int i = 0; i < FileCount; i ++ ) {
        pFileInfoArray += put_multibyte_long(
                              (unsigned char *)&FileInfoArray[ pFileInfoArray ],
                              FileInfo[ i ].FileName_Offset );
        pFileInfoArray += put_multibyte_long(
                              (unsigned char *)&FileInfoArray[ pFileInfoArray ],
                              FileInfo[ i ].Content_Offset );
        pFileInfoArray += put_multibyte_long(
                              (unsigned char *)&FileInfoArray[ pFileInfoArray ],
                              FileInfo[ i ].FileSize );
    }
    for( int i = 0; i < FileCount; i ++ ) {
        cout << FileInfo[ i ].FileName << "\tFileName_Offset: " << FileInfo[ i ].FileName_Offset
             << "\tContent_Offset: " << FileInfo[ i ].Content_Offset
             << "\tFileSize: " << FileInfo[ i ].FileSize << endl;
    }
    put_multibyte_long( FileInfoSizeConverted, pFileInfoArray );
    TargFile.write( (char *)&FileInfoSizeConverted[ 0 ],
                    strlen( (char *)&FileInfoSizeConverted[ 0 ] ) ); // (2) FileInfoSize
    TargFile.write( FileInfoArray, pFileInfoArray );                 // (3) FileInfoList
    delete [ ] FileInfoArray;
    
    
    put_multibyte_long( FileInfoSizeConverted, pFileNameArray );     // <==> FileNameListConverted
    TargFile.write( (char *)&FileInfoSizeConverted[ 0 ],
                    strlen( (char *)&FileInfoSizeConverted[ 0 ] ) ); // (4) NameListSize
    TargFile.write( FileNameArray, pFileNameArray );                 // (5) NameList
    delete [ ] FileNameArray;
    
    
    /* Fulfiill files */
    for( int i = 0; i < FileCount; i ++ ) {
        /* Read file */ 
        tempFile.open( ( opt_temp + "\\" + FileInfo[ i ].FileName ).c_str( ), ios::in | ios::binary );
        tempFileContent = new char [ FileInfo[ i ].FileSize ];
        tempFile.read( tempFileContent, FileInfo[ i ].FileSize );
        tempFile.close( );
        
        /* Encrypt */
        for( int j = 0; j < FileInfo[ i ].FileSize; j ++ ) tempFileContent[ j ] ^= (unsigned char)( j + 2 );
            
        /* Write to archieve */
        TargFile.write( tempFileContent, FileInfo[ i ].FileSize );
        delete [ ] tempFileContent;
    }
    
    
    /* Clean memory */
    delete [ ] FileInfo;
    TargFile.close( );
    
    return 0;
}
