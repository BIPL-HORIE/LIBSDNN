/*! @file LIBSDNN_PARAMETERS.h
@brief classes and functions for parameters
@author Kazumasa HORIE
@date 2015/12/1~
*/
#include "LIBPARAMETERS_BASE.h"
#include "LIBSDNN_LEXIAL.h"
#include "LIBPARAMETERS_NUMERICAL_FORMULA_ANALYSIS.h"
#include "LIBSDNN_UTILITY.h"
#include "LIBPARAMETERS_JUDGEMENT.h"
#include <iostream>
#include <fstream>
#include <sstream>


/*!
@brief PIMPL for parameter
@author Kazumasa HORIE
@date 2015/12/1~
*/
class libsdnn::parameters::PARAMETERS::PARAMETERS_ROOT
{
private:
	std::vector<base::PARAMETER_LEAF> leafs_;
	std::vector<base::PARAMETER_NODE> down_nodes_;

	void SplitTagName(std::vector<std::string> &tag_list, std::string &parameter_name, const std::string &material)
	{
		lexial::Split(tag_list, material, '\\');
		parameter_name = tag_list.back();
		tag_list.erase(--tag_list.end());
	}
	void AddParameter(std::vector<std::string> tag_list, std::string parameter_name, const std::string parameter_content)
	{
		if (tag_list.size() == 0)
		{
			for (auto leaf_itr = leafs_.begin(); leaf_itr != leafs_.end(); leaf_itr++)
			{
				if (leaf_itr->IsSameName(parameter_name))
				{
					leaf_itr->Overwrite(parameter_content);
					return;
				}
			}
			base::PARAMETER_LEAF new_leaf(parameter_name, parameter_content);
			leafs_.push_back(new_leaf);
			return;
		}
		else
		{
			for (auto down_node_itr = down_nodes_.begin(); down_node_itr != down_nodes_.end(); down_node_itr++)
			{
				if (down_node_itr->IsSameName(tag_list.front()))
				{
					down_node_itr->AddParameter(tag_list, parameter_name, parameter_content);
					return;
				}
			}
			base::PARAMETER_NODE new_node(tag_list.front());
			down_nodes_.push_back(new_node);
			(--down_nodes_.end())->AddParameter(tag_list, parameter_name, parameter_content);
			return;
		}
	}

public:

	/*! @brief load parameters.
	@param[in] stream ofstream/stringstream storing parameters.
	@return void.
	*/
	template<typename stream_type>
	void LoadFile(stream_type &stream)
	{
		std::vector<std::string> tag_list;
		std::string parameter_name;
		std::string parameter_content;

		std::string line_buffer;
		std::vector<std::string> divided_line_buffer;
		int reading_line = 0;

		while (getline(stream, line_buffer))
		{
			reading_line++;
			lexial::DeleteCharactor(line_buffer, " \t{");//スペース削除
			divided_line_buffer.clear();
			lexial::Split(divided_line_buffer, line_buffer, '%');
			line_buffer = divided_line_buffer.front();

			if (line_buffer.find("<") != std::string::npos)//タグを検知
			{
				libsdnn::lexial::DeleteCharactor(line_buffer, "<>{");//スペース削除
				tag_list.push_back(line_buffer);
			}

			else if (line_buffer.find("}") != std::string::npos)//タグ終端を検知
			{
				if (tag_list.size() == 0)
				{
					throw "line " + std::to_string(reading_line) + ": Illegal } exists.";
				}
				tag_list.pop_back();
			}
			else if (line_buffer.find("=") != std::string::npos)//パラメータを検知
			{
				lexial::Split(divided_line_buffer, line_buffer, '=');
				parameter_name = divided_line_buffer.front();
				parameter_content = divided_line_buffer[1];

				if (parameter_name == "")
				{
					throw "line " + std::to_string(reading_line) + ": Illegal \"=\" exists.";
				}
				if (lexial::CountCharactor(parameter_name, "\\"))
				{
					throw "line " + std::to_string(reading_line) + ": Illegal Parameter: " + parameter_name + "\n\\ is not permitted.";
				}
				AddParameter(tag_list, parameter_name, parameter_content);
			}
			else if (line_buffer != "")
			{
				throw "line " + std::to_string(reading_line) + ": "+ line_buffer + ": use % to comment out";
			}
		}

	}

	/*! @brief overwrite parameter
	@param[in] parameter_name name of parameter
	@param[in] parameter_content content of parameter
	@return なし．エラー発生時はthrow．
	*/
	void OverwriteParameter(const std::string parameter_name, const std::string parameter_content)
	{
		std::vector<std::string> tag_list;
		std::string parameter_name_buffer;
		SplitTagName(tag_list, parameter_name_buffer, parameter_name);
		AddParameter(tag_list, parameter_name_buffer, parameter_content);
	}

	/*! @brief read parameters
	@note when the function can not find the parameter and the default value is not defined, it return "".
	@param[out] out_parameter_content content of parameter
	@param[in] parameter_name name of parameter
	@param[in] default_parameter default content of parameter. when the function can not find the parameter, this value will be used as content.
	@return void
	*/
	void ReadParameter(std::string &out_parameter_content, const std::string parameter_name, const std::string default_content)
	{
		std::vector<std::string> tag_list;
		std::string parameter_name_buffer;
		SplitTagName(tag_list, parameter_name_buffer, parameter_name);

		if (tag_list.size() == 0)
		{
			for (auto leaf_itr = leafs_.begin(); leaf_itr != leafs_.end(); leaf_itr++)
			{
				if (leaf_itr->IsSameName(parameter_name_buffer))
				{
					out_parameter_content = leaf_itr->GetContent();
					return;
				}
			}
			base::PARAMETER_LEAF new_leaf(parameter_name_buffer, default_content);
			leafs_.push_back(new_leaf);
			std::cout << "add parameter: " << parameter_name_buffer << " = " << default_content << std::endl;
			out_parameter_content = (--leafs_.end())->GetContent();
			return;
		}
		else
		{
			for (auto down_node_itr = down_nodes_.begin(); down_node_itr != down_nodes_.end(); down_node_itr++)
			{
				if (down_node_itr->IsSameName(tag_list.front()))
				{
					out_parameter_content = down_node_itr->ReadParameter(tag_list, parameter_name_buffer, default_content);
					return;
				}
			}
			base::PARAMETER_NODE new_node(tag_list.front());
			down_nodes_.push_back(new_node);
			std::cout << "add tag: " << tag_list.front() << std::endl;
			out_parameter_content = (--down_nodes_.end())->ReadParameter(tag_list, parameter_name_buffer, default_content);
			return;
		}

	}

	/*! @brief get list of parameter
	@param[out] out_parameter_list list of parameter
	@return void;
	*/
	void ListParameter(std::string &out_parameter_list)const
	{
		for (auto leaf_itr = leafs_.begin(); leaf_itr != leafs_.end(); leaf_itr++)
			leaf_itr->ListParameter(out_parameter_list, 0);
		for (auto down_node_itr = down_nodes_.begin(); down_node_itr != down_nodes_.end(); down_node_itr++)
			down_node_itr->ListParameter(out_parameter_list, 0);
	}
};
template void libsdnn::parameters::PARAMETERS::PARAMETERS_ROOT::LoadFile(std::ifstream &stream);
template void libsdnn::parameters::PARAMETERS::PARAMETERS_ROOT::LoadFile(std::stringstream &stream);

//PARAMETERS
libsdnn::parameters::PARAMETERS::PARAMETERS() :pimpl_(new PARAMETERS_ROOT){}
libsdnn::parameters::PARAMETERS::~PARAMETERS()
{
}

void libsdnn::parameters::PARAMETERS::LoadFile(const std::string parameter_file_name)
{
	try {
		std::ifstream parameter_file;
		utility::OpenFile(parameter_file, parameter_file_name);
		pimpl_->LoadFile(parameter_file);
		parameter_file.close();
	}
	catch (std::string str)
	{
		std::cout << "Parameter file Syntax Error" << std::endl << str << std::endl;
		utility::Wait();
		exit(1);
	}
	catch (...)
	{
		//? ERROR ?//
		utility::error::BugFound(0xff01);
	}
}

void libsdnn::parameters::PARAMETERS::LoadFile(std::stringstream &parameter_stream)
{
	try {
		pimpl_->LoadFile(parameter_stream);
	}
	catch (...)
	{
		//? ERROR ?//
		utility::error::BugFound(0xff0d);
	}
}

void libsdnn::parameters::PARAMETERS::OverwriteParameter(const std::string parameter_name, const std::string &parameter_content)
{
	try {
		pimpl_->OverwriteParameter(parameter_name, parameter_content);
	}
	catch (...)
	{
		//? ERROR ?//
		utility::error::BugFound(0xff02);
	}
}

template<>
void libsdnn::parameters::PARAMETERS::OverwriteParameter(const std::string parameter_name, const std::vector<std::string> parameter_content)
{
	try {
		if (parameter_content.size() == 0)
			utility::error::BugFound(0x210);
		std::string content_string_buffer = "[";
		for (auto parameter_content_iterator = parameter_content.begin(); parameter_content_iterator != parameter_content.end(); ++parameter_content_iterator)
		{
			content_string_buffer += (*parameter_content_iterator) + ",";
		}
		*(content_string_buffer.end() - 1) = ']';
		OverwriteParameter(parameter_name, content_string_buffer);
	}
	catch (...)
	{
		//? ERROR ?//
		utility::error::BugFound(0xff03);
	}
}

template<typename content_type>
void libsdnn::parameters::PARAMETERS::OverwriteParameter(const std::string parameter_name, const std::vector<content_type> parameter_content)
{
	try {
		if (parameter_content.size() == 0)
			utility::error::BugFound(0x211);
		std::string content_string_buffer = "[";
		for (auto parameter_content_iterator = parameter_content.begin(); parameter_content_iterator != parameter_content.end(); ++parameter_content_iterator)
		{
			content_string_buffer += std::to_string(*parameter_content_iterator) + ",";
		}
		*(content_string_buffer.end() - 1) = ']';
		OverwriteParameter(parameter_name, content_string_buffer);
	}
	catch (...)
	{
		//? ERROR ?//
		utility::error::BugFound(0xff04);
	}
}

template void libsdnn::parameters::PARAMETERS::OverwriteParameter(const std::string parameter_name, const std::vector<int> parameter_content);
template void libsdnn::parameters::PARAMETERS::OverwriteParameter(const std::string parameter_name, const std::vector<double> parameter_content);
template void libsdnn::parameters::PARAMETERS::OverwriteParameter(const std::string parameter_name, const std::vector<unsigned int> parameter_content);

template<>
void libsdnn::parameters::PARAMETERS::ReadParameter(std::string &out_parameter_content, const std::string parameter_name, const std::string default_parameter_content, judgement::JUDGEMENT *judgement)
{
	if (judgement != nullptr)
	{
		while (!judgement->judgement(this, parameter_name, default_parameter_content))
		{
			std::cout << "Parameter<" << parameter_name << "> is incorrect." << std::endl;
			std::cout << "You can set the following parameters." << std::endl;
			judgement->coutList();
			std::cout << "Type a correct parameter: ";
			std::string content_buffer;
			std::cin >> content_buffer;
			OverwriteParameter(parameter_name, content_buffer);
			std::cin.ignore();
		}
	}

	try {
		pimpl_->ReadParameter(out_parameter_content, parameter_name, default_parameter_content);
		if (default_parameter_content == "" && out_parameter_content == "")
		{
			//? ERROR ?//
			std::cout << "Parameter<" << parameter_name << "> not found" << std::endl;
			std::cout << "this parameter dont have default value" << std::endl;
			std::cout << "Type a correct parameter: " << std::endl;
			std::string parameter_content;
			std::cin >> parameter_content;
			std::cin.ignore();
			OverwriteParameter(parameter_name, parameter_content);
			out_parameter_content = parameter_content;
		}
	}
	catch (...)
	{
		//? ERROR ?//
		utility::error::BugFound(0xff05);
	}
}

template<typename content_type>
void libsdnn::parameters::PARAMETERS::ReadParameter(content_type &out_parameter_content, const std::string parameter_name, const std::string default_parameter_content, judgement::JUDGEMENT *judgement)
{
	std::string content_buffer;
	ReadParameter(content_buffer, parameter_name, default_parameter_content,judgement);

	std::string content_buffer_checker = content_buffer;
	lexial::DeleteCharactor(content_buffer_checker, "0123456789+-*/^().");
	if (content_buffer_checker.size() != 0)
	{
		out_parameter_content = 0;
		//? ERROR ?//
		std::cout << "parameter: " << parameter_name << "is not a numerical value" << std::endl;
		std::cout << "type the parameter correctly: ";
		std::cin >> content_buffer;
		std::cin.ignore();
		OverwriteParameter(parameter_name, content_buffer);
		ReadParameter(out_parameter_content, parameter_name, default_parameter_content, judgement);
		return;
	}

	if (content_buffer == "")
		out_parameter_content = 0;
	else if (lexial::CountCharactor(content_buffer, "+-*/^()."))
	{
		try { out_parameter_content = static_cast<content_type>(nfa::NumericalFormulaAnalysis(content_buffer)); }
		catch (const char* str)
		{
			std::cout << "syntactic error：" << content_buffer << std::endl;
			std::cout << str << std::endl;
			std::cout << "type a formula correctly: ";
			std::cin >> content_buffer;
			std::cin.ignore();
			OverwriteParameter(parameter_name, content_buffer);
			ReadParameter(out_parameter_content, parameter_name, default_parameter_content, judgement);
		}
		catch (...)
		{
			utility::error::BugFound(0xff06);
		}
	}
	else
	{
		try{
			out_parameter_content = static_cast<content_type>(std::stoul(content_buffer));
		}
		catch (...) 
		{
			//? ERROR ?//
			utility::error::BugFound(0xff07);
		}
	}
}

//instantiate template
template void libsdnn::parameters::PARAMETERS::ReadParameter(int &out_parameter_content, const std::string parameter_name, const std::string default_parameter_content, judgement::JUDGEMENT *judgement);
template void libsdnn::parameters::PARAMETERS::ReadParameter(double &out_parameter_content, const std::string parameter_name, const std::string default_parameter_content, judgement::JUDGEMENT *judgement);
template void libsdnn::parameters::PARAMETERS::ReadParameter(unsigned int &out_parameter_content, const std::string parameter_name, const std::string default_parameter_content, judgement::JUDGEMENT *judgement);


template<>
void libsdnn::parameters::PARAMETERS::ReadParameter(std::vector<std::string> &out_parameter_content, const std::string parameter_name, const std::string default_parameter_content, judgement::JUDGEMENT *judgement)
{
	try
	{
		out_parameter_content.clear();

		std::string content_buffer;
		ReadParameter(content_buffer, parameter_name, default_parameter_content,judgement);

		if (content_buffer == "")
			return;

		if (content_buffer.c_str()[0] != '[' || content_buffer.c_str()[content_buffer.size() - 1] != ']')
		{
			//? ERROR ?//
			std::cout << "warning: Parameter<" << parameter_name << "> is not vector" << std::endl;
			std::cout << "Enclose in \"[]\" & separate each elements with \",\"" << std::endl;
			std::cout << "Type a correct parameter: " << std::endl;
			std::cin >> content_buffer;
			std::cin.ignore();
			OverwriteParameter(parameter_name, content_buffer);
			ReadParameter(out_parameter_content, parameter_name, default_parameter_content, judgement);
			return;
		}
		content_buffer.erase(content_buffer.begin());
		content_buffer.erase(--content_buffer.end());

		std::vector<std::string> vector_buffer;
		{
			int	brackets_count = 0;
			int start = 0;
			for (unsigned int i = 0; i < content_buffer.size(); i++)
			{
				switch (content_buffer.c_str()[i])
				{
				case '(':
					brackets_count++;
					break;
				case ')':
					brackets_count--;
					if (brackets_count < 0)
						brackets_count = 0;
					break;
				case ',':
					if (brackets_count == 0)
					{
						vector_buffer.push_back(content_buffer.substr(start, i-start));
						start = i+1;
					}
					break;
				}
			}
			vector_buffer.push_back(content_buffer.substr(start, content_buffer.size()));
		}
		for (auto vector_buffer_itr = vector_buffer.begin(); vector_buffer_itr != vector_buffer.end(); vector_buffer_itr++)
		{
			if (vector_buffer_itr->size() == 0)
			{
				//? ERROR ?//
				std::cout << "warning: Parameter<" << parameter_name << "> is not vector" << std::endl;
				std::cout << "Enclose in \"[]\" & separate each elements with \",\"" << std::endl;
				std::cout << "Type a correct parameter: " << std::endl;
				std::cin >> content_buffer;
				std::cin.ignore();
				OverwriteParameter(parameter_name, content_buffer);
				ReadParameter(out_parameter_content, parameter_name, default_parameter_content, judgement);
				return;
			}
		}
		
		for (auto vector_iterator = vector_buffer.begin(); vector_iterator != vector_buffer.end(); ++vector_iterator)
		{
			std::vector<std::string> iterator_buffer;
			lexial::Split(iterator_buffer, *vector_iterator, ':');
			switch (iterator_buffer.size())
			{
			case 1:
				out_parameter_content.push_back(*vector_iterator);
				break;
			case 2:
				for (int push_counter = 0; push_counter < stoi(iterator_buffer[1]); push_counter++)
					out_parameter_content.push_back(iterator_buffer[0]);
				break;
			default:
				std::cout << "warning: Parameter<" << parameter_name << "> is incorrect" << std::endl;
				std::cout << "[a:b] = [a,..,a](number of element = b)" << std::endl;
				std::cout << "Type a correct parameter: " << std::endl;
				std::cin >> content_buffer;
				std::cin.ignore();
				OverwriteParameter(parameter_name, content_buffer);
				ReadParameter(out_parameter_content, parameter_name, default_parameter_content,judgement);
				return;
			}
		}
	}
	catch (...)
	{
		//? ERROR ?//
		utility::error::BugFound(0xff08);
	}
}

template<typename content_type>
void libsdnn::parameters::PARAMETERS::ReadParameter(std::vector<content_type> &out_parameter_content, const std::string parameter_name, const std::string default_parameter_content, judgement::JUDGEMENT *judgement)
{
	try
	{
		std::vector<std::string> content_buffer;
		ReadParameter(content_buffer, parameter_name, default_parameter_content,judgement);

		out_parameter_content.clear();

		for (auto content_buffer_iterator = content_buffer.begin(); content_buffer_iterator != content_buffer.end(); ++content_buffer_iterator)
		{
			std::string content_buffer_checker = *content_buffer_iterator;

			lexial::DeleteCharactor(content_buffer_checker, "0123456789+-*/^().[]");
			if (content_buffer_checker.size() != 0)
			{
				//? ERROR ?//
				std::cout << "parameter: " << parameter_name << "is not a numerical vector" << std::endl;
				std::cout << "type the parameter correctly" << std::endl;
				std::string content_buffer2;
				std::cin >> content_buffer2;
				std::cin.ignore();
				OverwriteParameter(parameter_name, content_buffer2);
				ReadParameter(out_parameter_content, parameter_name, default_parameter_content,judgement);
				return;
			}

			if (lexial::CountCharactor(*content_buffer_iterator, "+*-/^().[]"))
			{
				try { out_parameter_content.push_back(static_cast<content_type>(nfa::NumericalFormulaAnalysis(*content_buffer_iterator))); }
				catch (const char* str)
				{
					std::cout << "syntactic error：" << *content_buffer_iterator << std::endl;
					std::cout << str << std::endl;
					std::cout << "type the formula correctly: ";
					std::cin >> *content_buffer_iterator;
					std::cin.ignore();
					OverwriteParameter(parameter_name, content_buffer);
					ReadParameter(out_parameter_content, parameter_name, default_parameter_content,judgement);
					return;
				}
				catch (...)
				{
					utility::error::BugFound(0xff09);
				}
			}
			else
			{
				try {
					out_parameter_content.push_back(static_cast<content_type>(std::stoul(*content_buffer_iterator)));
				}
				catch (...)
				{
					//? ERROR ?//
					utility::error::BugFound(0xff0a);
				}
			}
		}
	}
	catch (...)
	{
		//? ERROR ?//
		utility::error::BugFound(0xff0b);
	}
}

template void libsdnn::parameters::PARAMETERS::ReadParameter(std::vector<int> &out_parameter_content, const std::string parameter_name, const std::string default_parameter_content, judgement::JUDGEMENT *judgement);
template void libsdnn::parameters::PARAMETERS::ReadParameter(std::vector<double> &out_parameter_content, const std::string parameter_name, const std::string default_parameter_content, judgement::JUDGEMENT *judgement);
template void libsdnn::parameters::PARAMETERS::ReadParameter(std::vector<unsigned int> &out_parameter_content, const std::string parameter_name, const std::string default_parameter_content, judgement::JUDGEMENT *judgement);

void libsdnn::parameters::PARAMETERS::ListParameter(std::string &out_parameter_list)const
{
	try {
		pimpl_->ListParameter(out_parameter_list);
	}
	catch (...)
	{
		//? ERROR ?//
		utility::error::BugFound(0xff0c);
	}
}
