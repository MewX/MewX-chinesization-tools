/*
  Name: Noesis s-script packer_temp
  Copyright: MewCatcher
  Author: MewCatcher
  Date: 09/02/14 11:22
  Description: This program is for "Cure Girl", translated words cannot be larger than source text.
  Usage: Noesis_s_pack_temp script_iga_txt script_iga_dec script_iga_redec sDebug.txt
         Noesis_s_pack_temp test_dec test_scr test_pack debug
		 cl Noesis_s_pack_temp.cpp EasyUnicodeFileLE.cpp
  Note: After this step, user still need to encrypt all the file in "script_iga_redec" folder.
*/
#include <iostream>
#include <fstream>
#include <string>
#include <cstdio>
#include <Windows.h>
#include "EasyUnicodeFileLE.h"

using namespace std;


bool JudgeMatched( char *buff, unsigned int len, const char *tab, char *end );
int ReadWholeFile( string FileName, char *&buf );
int WriteWholeFile( string FileName, char *buf, int len );
string WstringToString( UINT LocalOption, wstring wstr );
wstring StringToWstring( UINT LocalOption, string str );


int main( int argc, char **argv )
{
    if( argc != 5 ) {
        cerr << "\n    <Noesis s-script packer_temp> By MewCatcher, 2014";
        cerr << "\n    Usage: " << argv[ 0 ] << " <translatedTxtFolder> <sourceSFolder> <targetSFolder> <(debugInfo).txt>\n";
        return -1;
    }
    //cerr << "In file: " << argv[ 1 ] << endl;

    
    /* Command Line */
    system( "md temp 2>nul" );
    for( int i = 1; i <= 2; i ++ ) { 
        const string opt_temp = argv[ i ]; // translatedTxtFolder, sourceSFolder
        system( ( "del temp\\" + opt_temp + ".lst 2>nul" ).c_str( ) );
        system( ( "dir "+ opt_temp + " /B /A-D | find /v /c \":\" >> temp\\" + opt_temp + ".lst" ).c_str( ) );
        system( ( "dir " + opt_temp + " /A-D /B >> temp\\" + opt_temp + ".lst" ).c_str( ) );
    }
    system( ( "md " + (string)argv[ 3 ] + " 2>nul" ).c_str( ) );
    system( ( "md " + (string)argv[ 3 ] + "_enc 2>nul" ).c_str( ) );


    /* Match tables ( Not Safe! ) */
    const char tabShowWord[ 4 ] = "\x00\x04\x00";
    const unsigned int tabShowWord_len = 3;
    const char tabShowEmail[ 4 ] = "\x3F\x04\x00";
    const unsigned int tabShowEmail_len = 3;
    const char tabShowLoadGoto[ 3 ] = "\x1D\x08";
    const unsigned int tabShowLoadGoto_len = 2;

    
    /* Definitions */
    bool hasBug = false;
    ofstream logFile, debugFile, outputFile;
    ifstream txtListFile, srcListFile;
    EasyUnicodeFileLE txtLEFile;
    string tempFileName;
	int txtListFile_count, srcListFile_count;
    
    
    /* Open files */
    logFile.open( "temp\\sPack.log", ios::out | ios::trunc );
    if( !logFile.good( ) ) return -10;
    debugFile.open( ( "temp\\" + (string)argv[ 4 ] + ".txt" ).c_str( ), ios::out | ios::trunc );
    if( !debugFile.good( ) ) return -11;
    txtListFile.open( ( "temp\\" + (string)argv[ 1 ] + ".lst" ).c_str( ), ios::in );
    if( !txtListFile.good( ) ) return -12;
	txtListFile >> txtListFile_count; // read line counts
    srcListFile.open( ( "temp\\" + (string)argv[ 2 ] + ".lst" ).c_str( ), ios::in );
    if( !srcListFile.good( ) ) return -13;
	srcListFile >> srcListFile_count; // read line counts
    
    
    /* Debug all: through comparing line1 to line2 */
    logFile << "Starting debug-process:" << endl
            << "  For more info, refer the debug log file." << endl;
    
    for( int k = 0; k < txtListFile_count; k ++ ) {
	    txtListFile >> tempFileName;
        if( tempFileName.length( ) == 0 ) break;
		
		debugFile << "Trying to open " << tempFileName << "...";
        txtLEFile.open( ( (string)argv[ 1 ] + "\\" + tempFileName ).c_str( ), ios::in );
        if( !txtLEFile.IsOpen( ) ) {
		    debugFile << "failed!" << endl;
			continue;
		}
		else debugFile << "succeeded!" << endl;
		
		int lnCount = 0;
		wstring line1, line2, name1, name2; // When use over, make it empty
		while( !txtLEFile.IsEOF( ) ) {
		    const wstring wstr = txtLEFile.readLine( );
			lnCount ++;
			
			/* line header check */
			if( wstr.length( ) == 0 ) continue;
			else if( wstr[ 0 ] == L'/' && wstr[ 1 ] == L'/' ) continue;
			else if( wstr[ 0 ] != L'●' && wstr[ 0 ] != L'○' ) {
			    debugFile << "  (line: " << lnCount << ") ERR0001 - unknown line header char, please check if a space or tab is at the beginning of a line." << endl;
				hasBug = true;
				continue;
			}
			
			/* save strings */
			int startPos;
			if( wstr[ 0 ] == L'●' ) {
			    if( wstr[ 5 ] == L'●' ) startPos = 6;
				else if( wstr[ 9 ] == L'●' ) startPos = 10;
				else {
				    debugFile << "  (line: " << lnCount << ") ERR0002 - what's wrong with the line number?" << endl;
					hasBug = true;
					continue;
				}
				
				if( wstr[ startPos ] == L'[' ) { // Name
				    startPos ++;
				    int end = startPos + 1;
					while( wstr[ end ] != L']' && end < wstr.length( ) ) end ++;
					if( end == wstr.length( ) ) {
						debugFile << "  (line: " << lnCount << ") ERR0006 - what's wrong with the name?" << endl;
						hasBug = true;
						continue;
					}
					name1 = wstr.substr( startPos, end - startPos + 1 );
					startPos = end + 1;
				}
				line1 = wstr.substr( startPos, wstr.length( ) - startPos ); // sentence
				
				continue;
			}
			else if( wstr[ 0 ] == L'○' ) {
			    if( wstr[ 5 ] == L'○' ) startPos = 6;
				else if( wstr[ 9 ] == L'○' ) startPos = 10;
				else {
				    debugFile << "  (line: " << lnCount << ") ERR0003 - what's wrong with the line number?" << endl;
					hasBug = true;
					continue;
				}
				
				if( wstr[ startPos ] == L'[' ) { // Name
				    startPos ++;
				    int end = startPos + 1;
					while( wstr[ end ] != L']' && end < wstr.length( ) ) end ++;
					if( end == wstr.length( ) ) {
						debugFile << "  (line: " << lnCount << ") ERR0007 - what's wrong with the name?" << endl;
						hasBug = true;
						continue;
					}
					name2 = wstr.substr( startPos, end - startPos );
					startPos = end + 1;
				}
				line2 = wstr.substr( startPos, wstr.length( ) - startPos );
				
				/* Compare lengths */
				if( name2.length( ) > name1.length( ) ) {
					debugFile << "  (line: " << lnCount << ") ERR0008 - name too long." << endl;
					hasBug = true;
					continue;
				}
				if( line2.length( ) > line1.length( ) ) {
					debugFile << "  (line: " << lnCount << ") ERR0004 - sentence too long." << endl;
					hasBug = true;
					continue;
				}
				
				line1 = L"";
				line2 = L"";
			}
			else {
			    debugFile << "  (line: " << lnCount << ") ERR0005 - I don't know what just happened!" << endl;
				hasBug = true;
				continue;
			}
		}
		
		txtLEFile.close( );
		debugFile << "File closed." << endl << endl;
    }
    
    logFile << "Debug finished!" << endl << endl;
    if( hasBug ) {
        logFile << "Due to bugs detected, program terminated!" << endl
                << "Any puzzle? Please refer the debug log file." << endl << endl;
        return -20;
    }
    
    
    /* Pack all */
	logFile << "Starting pack-process:" << endl;
	
    for( int k = 0; k < srcListFile_count; k ++ ) {
	    srcListFile >> tempFileName;
		if( tempFileName.length( ) == 0 ) break;
		logFile << "Trying to open " << tempFileName << "...";
		
		/* read .s file */
		char *tempContent = NULL;
		int tempContent_len = ReadWholeFile( ( (string)argv[ 2 ] + "\\" + tempFileName ).c_str( ), tempContent );
		if( tempContent_len == 0 ) {
			cerr << "Cannot read: " << (string)argv[ 2 ] + "\\" + tempFileName << endl;
			logFile << "failed." << endl;
			return -30;
		}
		else logFile << "succeeded. ( fileSize = " << tempContent_len << " )" << endl;
		
		/* check .txt file */
		logFile << "  Checking .txt file." << endl;
		tempFileName = tempFileName.substr( 0, tempFileName.length( ) - 1 ) + "txt";
		txtLEFile.open( ( (string)argv[ 1 ] + "\\" + tempFileName ).c_str( ), ios::in );
		tempFileName = tempFileName.substr( 0, tempFileName.length( ) - 3 ) + "s";
        if( !txtLEFile.IsOpen( ) ) {
		    logFile << "  Didn't find " << tempFileName << ". Just copy this .s file." << endl;
			WriteWholeFile( ( (string)argv[ 3 ] +  "\\" + tempFileName ).c_str( ), tempContent, tempContent_len );
			for( int ii = 0; ii < tempContent_len; ii ++ ) tempContent[ ii ] ^= 0xFF;
			WriteWholeFile( ( (string)argv[ 3 ] +  "_enc\\" + tempFileName ).c_str( ), tempContent, tempContent_len );
			delete [ ] tempContent;
			continue;
		}
		
		/* File loop */
		logFile << "  In loop." << endl;
		int index = 4, lnCount = 0;
		while( 1 ) { // process 2 files of .s & .txt types
		    bool hasName = false;
		
			/* get .txt one line */
			wstring wstr;
		    string str;
			int startPos = 0;
			while( wstr.length( ) == 0 || wstr[ 0 ] != L'○' ) {
			    lnCount ++;
			    wstr = txtLEFile.readLine( );
				if( txtLEFile.IsEOF( ) ) break;
			}
			if( txtLEFile.IsEOF( ) ) break;
			
			if( wstr[ 5 ] == L'○' ) startPos = 6;
		    else if( wstr[ 9 ] == L'○' ) startPos = 10;
			else logFile << "  Uknown error happened!" << endl;
			
			int end;
			if( wstr[ startPos ] == L'[' ) {
			    hasName = true;
			    startPos ++;
			    end = startPos + 1;
				while( wstr[ end ] != L']' && end < wstr.length( ) ) end ++;
				//startPos = end + 1;
			}
			
			/* go to .s insertion position */
			int insert_len = 0;
			char type = '0'; // '1' - Name, '2' - Text, '3' - Msg, '4' - Choice
		FIND_INSERT_POSITION:
		    while( index < tempContent_len ) {
			    if( JudgeMatched( &tempContent[ index ], tabShowWord_len, &tabShowWord[ 0 ], &tempContent[ tempContent_len - 1 ] ) ) {
					// ShowWord part
					index += tabShowWord_len;
					logFile << "  index1 = " << index << endl;
					if( tempContent[ index + 1 ] == '\x81' && tempContent[ index + 2 ] == '\x94' ) {
						// People name
						type = '1';
						insert_len = (unsigned char)tempContent[ index ++ ];
						if( !insert_len ) continue;
						index += 2;
					    ////////////////////////////////////////////////
						//tempLineStc = "[";
						//tempLineStc += (char *)&temp[ index ];
						//tempLineStc += "]";
						break;
					}
					else {
						// Text
						type = '2';
						insert_len = (unsigned char)tempContent[ index ++ ];
						if( !insert_len ) continue;
					    ///////////////////////////////////////////////
						//tempLineStc = (char *)&temp[ index ];
						break;
					}
				}
				else if( JudgeMatched( &tempContent[ index ], tabShowEmail_len, &tabShowEmail[ 0 ], &tempContent[ tempContent_len - 1 ] ) ) {
					// ShowEmail part
					type = '3';
					index += tabShowEmail_len;
					logFile << "  index2 = " << index << endl;
					
					insert_len = (unsigned char)tempContent[ index ++ ];
					if( !insert_len ) continue;
					//////////////////////////////////////////////////////
					//tempLineStc = (char *)&temp[ index ];
					break;
				}
				else if( JudgeMatched( &tempContent[ index ], tabShowLoadGoto_len, &tabShowLoadGoto[ 0 ], &tempContent[ tempContent_len - 1 ] ) ) {
					// Jump to script info
					type = '4';
					index += tabShowLoadGoto_len;
					logFile << "  index3 = " << index << endl;
					
					insert_len = (int)( *(unsigned short *)&tempContent[ index ] );
					if( !insert_len ) continue;
					index += 2 + 4;
					///////////////////////////////////////////////////////
					//tempLineStc = (char *)&temp[ index ];
					//tempLineStc = tempLineStc.substr( 0, Cho_len );
					break;
				}
				else index ++;
			}
			if( index == tempContent_len ) break;
		    
			
			/* insert */
			//if( type == '4' )
			//    for( int m = 0; m < insert_len; m ++ ) tempContent[ index + m ] = '\0';
			//else
			//    for( int m = 0; m < insert_len && tempContent[ m ] != '\0'; m ++ )
			//	    tempContent[ index + m ] = '\0';
			
			if( type == '3' ) {
			    for( int y = 0; y < wstr.length( ); y ++ )
				    if( wstr[ y ] == L'＄' ) wstr[ y ] = L'亹';
			}
			
			if( hasName ) {
			    str = WstringToString( 936, wstr.substr( startPos, end - startPos ) );
				if( wstr.substr( startPos, end - startPos ) != StringToWstring( 936, str ) ) {
				    logFile << "  WideChars to mutibytes losed: (line: " << lnCount << ") " << str << endl;
				}
			}
			else {
			    str = WstringToString( 936, wstr.substr( startPos, wstr.length( ) - startPos ) );
			    if( wstr.substr( startPos, wstr.length( ) - startPos ) != StringToWstring( 936, str ) ) {
				    logFile << "  WideChars to mutibytes losed: (line: " << lnCount << ") " << str << endl;
				}
			}
			
			for( int j = 0; j < str.length( ); j ++ ) {
			    if( str[ j ] == ' ' ) { // Space is special
				    tempContent[ index + j ] = 0xA1;
				    tempContent[ ( ++ index ) + j ] = 0xA1;
				}
			    else if( '!' <= str[ j ] && str[ j ] <= '~' ) {
				    tempContent[ index + j ] = 0xA3;
				    tempContent[ ( ++ index ) + j ] = str[ j ] + 0x80;
				}
				else {
				    tempContent[ index + j ] = str[ j ];
					j ++;
				    tempContent[ index + j ] = str[ j ];
				}
			}
			if( type != '4' ) tempContent[ index + str.length( ) ] = '\0';
			index += str.length( );
			
			if( hasName ) {
			    startPos = end + 1;
				hasName = false;
				goto FIND_INSERT_POSITION;
			}
		}
		
		WriteWholeFile( ( (string)argv[ 3 ] +  "\\" + tempFileName ).c_str( ), tempContent, tempContent_len );
		for( int ii = 0; ii < tempContent_len; ii ++ ) tempContent[ ii ] ^= 0xFF;
		WriteWholeFile( ( (string)argv[ 3 ] +  "_enc\\" + tempFileName ).c_str( ), tempContent, tempContent_len );
		
		txtLEFile.close( );
		delete [ ] tempContent;
		logFile << "File closed." << endl << endl;
	}
    
	logFile << "All processes done!" << endl;

    /* CLean memory */
    logFile.close( );
    debugFile.close( );
	txtListFile.close( );
	srcListFile.close( );

    return 0;
}


bool JudgeMatched( char *buff, unsigned int len, const char *tab, char *end )
{
    //cerr << "JudgeMatched( buff=" << (int)&buff[ 0 ] << ", len=" << len << ", tab=" << &tab << ", end=" << (int)&end[ 0 ] << ");" << endl;
    if( buff + len > end ) return false; // too long

    unsigned int i = 0;
    for( ; i < len; i ++ )
        if( buff[ i ] != tab[ i ] ) break;
    return i == len;
}

int ReadWholeFile( string FileName, char *&buf )
{
    fstream file;
    file.open( FileName.c_str( ), ios::in | ios::binary );
    if( !file.good( ) ) return 0;
    
    file.seekg( 0, ios::end );
    int len = file.tellg( );
    buf = new char [ len ];
    file.seekg( 0, ios::beg );
    file.read( buf, len );
    file.close( );
    return len;
}

int WriteWholeFile( string FileName, char *buf, int len )
{
    fstream file;
	file.open( FileName.c_str( ), ios::out | ios::binary | ios::trunc );
	if( !file.good( ) ) return 0;
	
	// Decrypt
	
	file.write( buf, len );
	return 1;
}

string WstringToString( UINT LocalOption, wstring wstr )
{
    wchar_t *wszString = new wchar_t [ wstr.length( ) + 1 ];
    for ( UINT i = 0; i < wstr.length( ); i ++ ) {
        wszString[ i ] = wstr[ i ];
    }
    wszString[ wstr.length( ) ] = '\0';
	
    UINT strLen = ::WideCharToMultiByte( LocalOption, NULL, wszString, -1, NULL, 0, NULL, NULL );
    char *szAnsi = new char [ strLen ];
    ::WideCharToMultiByte( LocalOption, NULL, wszString, -1, szAnsi, strLen, NULL, NULL );
    szAnsi[ strLen - 1 ] = '\0';

    string str = szAnsi;

    delete [ ] szAnsi;
    delete [ ] wszString;

    /* check */
    // if( StringToWstring( LocalOption, str ) != wstr ) cout << "# Warning: Converting unsafe!" << endl;
    
    return str;
}

wstring StringToWstring( UINT LocalOption, string str )
{
    // 932 shift_jis ANSI/OEM Japanese; Japanese (Shift-JIS)
    // 936 gb2312 ANSI/OEM Simplified Chinese (PRC, Singapore); Chinese Simplified (GB2312)
    // 949 ks_c_5601-1987 ANSI/OEM Korean (Unified Hangul Code)
    // 950 big5 ANSI/OEM Traditional Chinese (Taiwan; Hong Kong SAR, PRC); Chinese Traditional (Big5)

    char *szAnsi = new char [ str.length( ) + 1 ];
    for ( UINT i = 0; i < str.length( ); i ++ ) szAnsi[ i ] = str[ i ];
    szAnsi[ str.length( ) ] = '\0';

    int wcsLen = ::MultiByteToWideChar( LocalOption, NULL, szAnsi, strlen(szAnsi), NULL, 0);
    wchar_t *wszString = new wchar_t [ wcsLen + 1 ];
    ::MultiByteToWideChar( LocalOption, NULL, szAnsi, strlen( szAnsi ), wszString, wcsLen) ;
    wszString[ wcsLen ] = '\0';

    wstring wstr = wszString;
    delete [ ] szAnsi;
    delete [ ] wszString;

    return wstr;
}
