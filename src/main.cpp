    ////////////////////////////////////////
   //      AileScript by SnegirSoft      //
  //                                    //
 //  File: main.cpp                    //
////////////////////////////////////////

#include "Parser.h"
#include "encoding.h"

#include <iostream>
#include <fstream>

void new_line(const std::string& line, Parser& parser)
{
	std::u16string str16 = enc::to_16(line); // Convert to UTF-16
	for (char16_t c : str16)
	{
		parser.next(c);
	}
	parser.next(u'\n');
	TokenList tokens = parser.extract_tokens(); // Get parsed tokens
	for (Token::Ptr token : tokens) // I haven't read an interpreter yet, for now just print every token
	{
		token->print();
	}
}

int main(int argc, char* argv[])
{
	Parser parser;

	if (argc == 1) // Live from console
	{
		std::string buff;
		while (true)
		{
			std::cout << " > ";
			std::getline(std::cin, buff); // Read a line
			new_line(buff, parser);
		}
	}
	else // From the file
	{
		std::string buff;
		for (int i_file = 1; i_file < argc; ++i_file) // Each element of argv is a file
		{
			std::ifstream file(argv[i_file]);
			if (file.is_open()) // If the file is successfully opened
			{
				while (std::getline(file, buff))
				{
					new_line(buff, parser);
				}
			}
			else
			{
				std::cout << "Error: couldn't open file " << argv[i_file] << std::endl;
			}
		}
	}
	return 0;
}
