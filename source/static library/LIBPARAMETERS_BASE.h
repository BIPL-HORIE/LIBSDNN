/*! @file
@author Kazumasa HORIE
@date 2015/12/1~
*/

#pragma once
#include "LIBPARAMETERS.h"
#include <vector>
#include "LIBSDNN_UTILITY.h"
#include<iostream>

namespace libsdnn
{
	namespace parameters
	{
		namespace base
		{
			class PARAMETER_LEAF
			{
			private:
				std::string parameter_name_;
				std::string parameter_content_;
			public:
				PARAMETER_LEAF(const std::string parameter_name, const std::string parameter_content):
				parameter_name_(parameter_name), parameter_content_(parameter_content){}
				~PARAMETER_LEAF() {}

				bool IsSameName(const std::string& parameter_name)const { return (parameter_name_ == parameter_name); }
				std::string GetContent()const { return parameter_content_; }
				void Overwrite(const std::string &parameter_content) {
					if(parameter_content.size () < 300)
						std::cout << "overwrite parameter: " << parameter_name_ << " = " << parameter_content << std::endl;				
					parameter_content_ = parameter_content;
				}

				void ListParameter(std::string &out_parameter_list, const int tag_number)const {
					for (int i = 0; i < tag_number; i++)
						out_parameter_list += "    ";
					out_parameter_list += parameter_name_ + "=" + parameter_content_ + "\n";
				}
			};

			class PARAMETER_NODE
			{
			private:
				std::string node_name_;
				std::vector<PARAMETER_LEAF> leafs_;
				std::vector<PARAMETER_NODE> down_nodes_;

			public:
				PARAMETER_NODE(std::string node_name): node_name_(node_name) {}
				~PARAMETER_NODE() {}

				bool IsSameName(const std::string &node_name) { return (node_name_ == node_name); }
				void AddParameter(std::vector<std::string> tag_list, const std::string &parameter_name, const std::string &parameter_content)
				{
					if (tag_list.front() != node_name_)
					{
						std::cout << tag_list.front() <<"," << node_name_<< std::endl;
						utility::error::BugFound(0x2f00ff);
					}
					if (tag_list.size() == 1)
					{
						for (auto leaf_itr = leafs_.begin(); leaf_itr != leafs_.end(); leaf_itr++)
						{
							if (leaf_itr->IsSameName(parameter_name))
							{
								leaf_itr->Overwrite(parameter_content);
								return;
							}
						}
						PARAMETER_LEAF new_leaf(parameter_name, parameter_content);
						leafs_.push_back(new_leaf);
						return;
					}
					else
					{
						tag_list.erase(tag_list.begin());
						for (auto down_node_itr = down_nodes_.begin(); down_node_itr != down_nodes_.end(); down_node_itr++)
						{
							if (down_node_itr->IsSameName(tag_list.front()))
							{
								down_node_itr->AddParameter(tag_list, parameter_name, parameter_content);
								return;
							}
						}
						PARAMETER_NODE new_node(tag_list.front());
						down_nodes_.push_back(new_node);
						(--down_nodes_.end())->AddParameter(tag_list, parameter_name, parameter_content);
						return;
					}
					return;
				}

				std::string ReadParameter(std::vector<std::string> &tag_list, const std::string &parameter_name, const std::string &default_content)
				{
					if (tag_list.front() != node_name_)
					{
						utility::error::BugFound(0x2f00fe);
					}
					if (tag_list.size() == 1)
					{
						for (auto leaf_itr = leafs_.begin(); leaf_itr != leafs_.end(); leaf_itr++)
						{
							if (leaf_itr->IsSameName(parameter_name))
							{
								return leaf_itr->GetContent();
							}
						}
						PARAMETER_LEAF new_leaf(parameter_name, default_content);
						leafs_.push_back(new_leaf);
						std::cout << "add parameter: " << parameter_name << " = " << default_content << std::endl;
						return leafs_.back().GetContent();
					}
					else
					{
						tag_list.erase(tag_list.begin());
						for (auto down_node_itr = down_nodes_.begin(); down_node_itr != down_nodes_.end(); down_node_itr++)
						{
							if (down_node_itr->IsSameName(tag_list.front()))
							{
								return down_node_itr->ReadParameter(tag_list, parameter_name, default_content);
							}
						}
						PARAMETER_NODE new_node(tag_list.front());
						down_nodes_.push_back(new_node);
						std::cout << "add tag: " << tag_list.front() << std::endl;
						return down_nodes_.back().ReadParameter(tag_list,parameter_name,default_content);
					}
				}

				void ListParameter(std::string &out_parameter_list, const int tab_number)const
				{
					for (int i = 0; i < tab_number; i++)
						out_parameter_list += "    ";
					out_parameter_list += "<" + node_name_ + ">\n";
					for (int i = 0; i < tab_number; i++)
						out_parameter_list += "    ";
					out_parameter_list += "{\n";

					for (auto leaf_itr = leafs_.begin(); leaf_itr != leafs_.end(); leaf_itr++)
					{
						leaf_itr->ListParameter(out_parameter_list, tab_number + 1);
					}
					for (auto down_node_itr = down_nodes_.begin(); down_node_itr != down_nodes_.end(); down_node_itr++)
					{
						down_node_itr->ListParameter(out_parameter_list, tab_number + 1);
					}
					for (int i = 0; i < tab_number; i++)
						out_parameter_list += "    ";
					out_parameter_list += "}\n";
				}
			};
		}
	}
}
