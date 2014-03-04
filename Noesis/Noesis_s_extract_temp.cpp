/*
  Name: Noesis s-script extractor
  Copyright: MewCatcher
  Author: MewCatcher
  Date: 09/02/14 11:22
  Description: This program is for "Cure Girl"
  Usage: for %i in (script_iga_dec\*) do Noesis_s_extract_temp %i script_iga_txt\%~ni.txt 2>>s2txt_stderr.txt
*/
#include <iostream>
#include <fstream>
#include <string>
#include <cstdio>
#include <Windows.h>

#define CHARSET_DEFAULT 932

using namespace std;


bool JudgeMatched( char *buff, unsigned int len, const char *tab, char *end );
wstring StringToWstring( UINT LocalOption, string str );


int main( int argc, char **argv )
{
    /* < test codes >
    string str = "cl.exe的wstring中文转码测试\r\n";
    wstring wstr = StringToWstring( 936, str );

    fstream f;
    f.open( "wstring_test.txt", ios::trunc | ios::out | ios::binary );
    f.write( "\xFF\xFE", 2 );
    f.write( (char *)wstr.c_str( ), wstr.length( ) * 2 );
    f.close( );
    */

    if( argc != 3 ) {
        cerr << "\n    <Noesis s-script extractor> By MewCatcher, 2014";
        cerr << "\n    Usage: " << argv[ 0 ] << " <*.s> <*.txt>\n";
        return -1;
    }
    cerr << "In file: " << argv[ 1 ] << endl;

    /* Initial */
    fstream file;
    char *temp;
    unsigned int FileLen = 0;
	wstring Header = L"// 文件上面一行作为参照，下面一行进行修改，两个斜杠代表注释，就像本句\r\n\r\n";


    /* Read file */
    file.open( argv[ 1 ], ios::in | ios::binary );                   // For read
    file.seekg( 0, ios::end );
    FileLen = file.tellg( );
    file.seekg( 0, ios::beg );
    temp = new char [ FileLen ];
    file.read( temp, FileLen );
    file.close( );

    file.open( argv[ 2 ], ios::out | ios::trunc | ios::binary );     // For write
    file.write( "\xFF\xFE", 2 ); // Unicode file header
	file.write( (char *)Header.c_str( ), Header.length( ) * 2 );


    /* Match tables ( Not Safe! ) */
    const char tabName[ 7 ] = "\x00\x04\x00?\x81\x94";
    const unsigned int tabName_len = 6;
    const char tabUknowm1[ 3 ] = "?\x08";
    const unsigned int tabUknowm1_len = 2;
    const char tabAfterName[ 6 ] = "?\x0C\x08\x00\x00";
    const unsigned int tabAfterName_len = 5; // follow 4 bytes to ( "\x00\x04\x00" or Voice )
    const char tabVoice[ 12 ] = "??\x00\x00\x27\x08\x00\x00\x00\x00\x00"; // follow 1 byte length
    const unsigned int tabVoice_len = 11;
    const char tabTextShow[ 4 ] = "\x00\x04\x00";
    const unsigned int tabTextShow_len = 3;

    //const char tabChoice[ 13 ] = "\x1C\x04\x00\x00\x1D\x08????\x00\x00";
    //const unsigned int tabChoice_len = 12;
    const char tabChoice[ 11 ] = "\x00\x00\x1D\x08?\x00??\x00\x00";
    const unsigned int tabChoice_len = 10;
    const char tabNextChoice[ 6 ] = "\x00??\x00\x00";
    const unsigned int tabNextChoice_len = 5;

    //const char tabTextOnly[ 12 ] = "\x0C\x08\x00\x00??\x00\x00\x00\x04\x00";
    //const unsigned int tabTextOnly_len = 11;
    const char tabTextOnly[ 4 ] = "\x00\x04\x00";
    const unsigned int tabTextOnly_len = 3;

    /*operation-entry*/
    const char tabShowWord[ 4 ] = "\x00\x04\x00";
    const unsigned int tabShowWord_len = 3;
    const char tabShowEmail[ 4 ] = "\x3F\x04\x00";
    const unsigned int tabShowEmail_len = 3;
	const char tabShowLoadGoto[ 3 ] = "\x1D\x08";
	const unsigned int tabShowLoadGoto_len = 2;


    /* Extract */
    unsigned int index = 4, countLine = 1;
    wstring LineSentence, PeopleName;
    string tempLineStc;
    bool hasName;
    while( index < FileLen ) {
        if( JudgeMatched( &temp[ index ], tabShowWord_len, &tabShowWord[ 0 ], &temp[ FileLen - 1 ] ) ) {
            // ShowWord part
            cerr << "tabShowWord at " << index << endl;
            index += tabShowWord_len;
            if( temp[ index + 1 ] == '\x81' && temp[ index + 2 ] == '\x94' ) {
                // People name

                int Name_len = (unsigned char)temp[ index ++ ];
				if( !Name_len ) continue;
				
				index += 2;
                tempLineStc = "[";
                tempLineStc += (char *)&temp[ index ];
                tempLineStc += "]";
                index += Name_len;

                hasName = true;
                PeopleName = StringToWstring( CHARSET_DEFAULT, tempLineStc );

            }
            else {
                // Text
                int Text_len = (unsigned char)temp[ index ++ ];
				if( !Text_len ) continue;
                tempLineStc = (char *)&temp[ index ];
                index += Text_len;
				
                char tNum[ 10 ];
                sprintf( tNum, "%04d", countLine ++ );
				
                if( hasName ) {
                    hasName = false;
                    LineSentence = L"●" + StringToWstring( CHARSET_DEFAULT, tNum ) + L"●" + PeopleName + StringToWstring( CHARSET_DEFAULT, tempLineStc ) + L"\r\n"
					             + L"○" + StringToWstring( CHARSET_DEFAULT, tNum ) + L"○" + PeopleName + StringToWstring( CHARSET_DEFAULT, tempLineStc ) + L"\r\n\r\n";
                }
                else {
                    LineSentence = L"●" + StringToWstring( CHARSET_DEFAULT, tNum ) + L"●" + StringToWstring( CHARSET_DEFAULT, tempLineStc ) + L"\r\n"
					             + L"○" + StringToWstring( CHARSET_DEFAULT, tNum ) + L"○" + StringToWstring( CHARSET_DEFAULT, tempLineStc ) + L"\r\n\r\n";
                }
                file.write( (char *)LineSentence.c_str( ), LineSentence.length( ) * 2 );
            }
        }
        else if( JudgeMatched( &temp[ index ], tabShowEmail_len, &tabShowEmail[ 0 ], &temp[ FileLen - 1 ] ) ) {
            // ShowEmail part
			if( temp[ index ] != tabShowEmail[ 0 ] ) goto Next;
            cerr << "tabShowEmail at " << index << "; info: " << (const char *)&temp[ index + tabShowEmail_len + 1 ] << endl;
            index += tabShowEmail_len;
			
			int Msg_len = (unsigned char)temp[ index ++ ];
			if( !Msg_len ) continue;
            tempLineStc = (char *)&temp[ index ];
			index += Msg_len;
			
            char tNum[ 10 ];
            sprintf( tNum, "%04d Msg", countLine ++ );
			LineSentence = L"●" + StringToWstring( CHARSET_DEFAULT, tNum ) + L"●" + StringToWstring( CHARSET_DEFAULT, tempLineStc ) + L"\r\n"
					     + L"○" + StringToWstring( CHARSET_DEFAULT, tNum ) + L"○" + StringToWstring( CHARSET_DEFAULT, tempLineStc ) + L"\r\n\r\n";
            file.write( (char *)LineSentence.c_str( ), LineSentence.length( ) * 2 );
		}
        else if( JudgeMatched( &temp[ index ], tabShowLoadGoto_len, &tabShowLoadGoto[ 0 ], &temp[ FileLen - 1 ] ) ) {
		    // Jump to script info
			cerr << "tabShowLoadGoto at " << index << "; info: " << (const char *)&temp[ index + tabShowLoadGoto_len + 4 + 2 ] << endl;
			index += tabShowLoadGoto_len;
			
			int Cho_len = (int)( *(unsigned short *)&temp[ index ] );
			if( !Cho_len ) continue;
			index += 2 + 4;
            tempLineStc = (char *)&temp[ index ];
			tempLineStc = tempLineStc.substr( 0, Cho_len );
			cout << "Cho_len =" << Cho_len << endl;
			index += Cho_len;
			
            char tNum[ 10 ];
            sprintf( tNum, "%04d Cho", countLine ++ );
			LineSentence = L"●" + StringToWstring( CHARSET_DEFAULT, tNum ) + L"●" + StringToWstring( CHARSET_DEFAULT, tempLineStc ) + L"\r\n"
					     + L"○" + StringToWstring( CHARSET_DEFAULT, tNum ) + L"○" + StringToWstring( CHARSET_DEFAULT, tempLineStc ) + L"\r\n\r\n";
            file.write( (char *)LineSentence.c_str( ), LineSentence.length( ) * 2 );
		}
		else {
		Next:
		    index ++;
		}
    }

    /* CLean memory */
    delete [ ] temp;
    file.close( );
    cerr << endl;

    return 0;
}


bool JudgeMatched( char *buff, unsigned int len, const char *tab, char *end )
{
    if( buff + len > end ) return false; // too long

    unsigned int i = 0;
    for( ; i < len; i ++ )
        if( tab[ i ] != '?' && buff[ i ] != tab[ i ] ) break;

    return i == len;
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
