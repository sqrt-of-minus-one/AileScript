    ////////////////////////////////////////
   //      AileScript by SnegirSoft      //
  //                                    //
 //  File: main.cpp                    //
////////////////////////////////////////

#include "Parser/Parser.h"
#include "Interpreter/Interpreter.h"
#include "encoding.h"

#include <iostream>
#include <fstream>

void new_line(const std::string& line, Parser& parser, Interpreter& interpreter)
{
	parser << enc::to_16(line) << u'\n';
	interpreter << parser;
}

int main(int argc, char* argv[])
{
	Parser parser;
	Interpreter interpreter;

	if (argc == 1) // Live from console
	{
		std::string buff;
		while (true)
		{
			switch (parser.get_input_status())
			{
			case Parser::EInputStatus::READY:
				switch (interpreter.get_input_status())
				{
				case Interpreter::EInputStatus::READY:
					std::cout << "   > ";
					break;
				case Interpreter::EInputStatus::PARENTHESIS:
					std::cout << "(  > ";
					break;
				case Interpreter::EInputStatus::SQUARE_BRACKET:
					std::cout << "[  > ";
					break;
				case Interpreter::EInputStatus::BRACKET:
					std::cout << "{  > ";
					break;
				}
				break;
			case Parser::EInputStatus::CHAR:
				std::cout << "'  > ";
				break;
			case Parser::EInputStatus::STRING:
				std::cout << "\"  > ";
				break;
			case Parser::EInputStatus::SINGLE_LINE_COMMENT:
				std::cout << "// > ";
				break;
			case Parser::EInputStatus::MULTI_LINE_COMMENT:
				std::cout << "/* > ";
				break;
			}
			std::getline(std::cin, buff); // Read a line
			new_line(buff, parser, interpreter);
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
					new_line(buff, parser, interpreter);
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
