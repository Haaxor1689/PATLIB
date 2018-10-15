/* This is dic2traskelet, a tool for picking the list of used characters
 * out of a file. The file may be either 8-bit ASCII or UTF-8 encoded.
 */
// $Id: dic2traskelet.C,v 1.1 2001/11/08 11:50:02 antos Exp $

#include <iostream>
#include <fstream>
#include <set>
#include <string>

// do we work in utf-8 regime?
bool utf_8;

// we need unsigned strings
typedef unsigned char uchar;
typedef basic_string<uchar> ustring;

// this procedure finds all chars in the dic file and writes them to tra
void find_all_chars(char *dicname, char *traname) {
  ifstream dic(dicname);

  // prepare a set to put the chars
  set<ustring> characters;

  uchar chr;
  ustring str;
  cout<<"Reading the word list..."<<endl;
  
  while(dic>>chr) {
    str = chr;
    
    uchar first = chr; // copy for shifting
    if (utf_8 && (first & 0x80)) { // read the rest of the multibyte sequence
      first = first << 1; // shift left
      while (first & 0x80) {
	dic>>chr;
	if (!(chr & 0x80)) {
	  cout<<"The input file contains a malformed utf-8 character."<<endl;
	  cout<<"This is probably quite wrong."<<endl;
	}
	str += chr;
	first = first << 1;
      }	
    }
    
    characters.insert(str);
  }

  cout<<"... done"<<endl;
  // write the chars to tra file
  cout<<"Writing the characters"<<endl;
  ofstream tra(traname);
  for (set<ustring>::iterator i = characters.begin(); i != characters.end();
      i++) {
    tra<<*i<<endl;
  }
  cout<<"... done"<<endl;
}

int main(int argc, char **argv) {
  cout<<"This is dic2traskelet, version 0.1"<<endl;
  cout<<"Written and maintained by David Antos, (c) 2001 David Antos"<<endl;

  if (argc == 3) {
    utf_8 = false;
    find_all_chars(argv[1], argv[2]);
  }
  else if (argc == 4 && (0 == strcmp(argv[1], "-u8"))) {
    utf_8 = true;
    find_all_chars(argv[2], argv[3]);
  }
  else {// this is an error
    cout<<endl;
    cout<<"This program takes a word list for OPATGEN and creates a skeleton"
      <<endl<<"for translate file. It means it reads the word list and writes"
      <<endl<<"to a file all distinct characters occurring there."<<endl;
    cout<<endl;
    cout<<"Usage:"<<endl;
    cout<<"dic2traskelet [-u8] dic tra"<<endl;
    cout<<"  -u8   the input is in UTF-8 encoding, otherwise 8-bit ASCII "
      <<"(the default)"<<endl;
    cout<<"  dic   the dictionary file"<<endl;
    cout<<"  tra   the skelton for the translate file"<<endl;
    cout<<endl;
    cout<<"Note that we are able to produce only the list of used characters, "
      <<"nothing more."<<endl;
    cout<<endl;
  } 
}
