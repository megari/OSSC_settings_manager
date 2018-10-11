#include "menu.h"
#include <boost/property_tree/ptree.hpp>
// XXX: Should we use XML instead?
//#include <boost/property_tree/xml_parser.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/lexical_cast.hpp>
#include <string>
#include <iostream>

namespace property_tree = boost::property_tree;
using property_tree::ptree;

#if 0
static std::unique_ptr<base_node>
process_node(const std::string &name, const ptree &tree);
#endif

value_node
main_menu::process_value_node(const std::string &name, const ptree &tree)
{
	std::string description = tree.get<std::string>("description");
	auto v_def = tree.get<value_node::value_type>("value_default");
	auto v_min = tree.get<value_node::value_type>("value_min");
	auto v_max = tree.get<value_node::value_type>("value_max");
	value_node ret(name, description, v_def, v_min, v_max);

	std::cout << "Entered value node " << name << std::endl;

	std::cout << "\tvalue_default: " << v_def << std::endl;
	std::cout << "\tvalue_min: " << v_min << std::endl;
	std::cout << "\tvalue_max: " << v_max << std::endl;

	std::cout << "\tvalue_descriptions:" << std::endl;
	ptree foo;
	for (const auto &node : tree.get_child("value_descriptions", foo)) {
		auto desc_idx = boost::lexical_cast<value_node::value_type>(node.first);
		const std::string &desc_str = node.second.data();

		std::cout << "\t\t" << desc_idx << " -> " << desc_str << std::endl;
		ret.add_value_description(desc_idx, desc_str);
	}

	std::cout << "Exited value node " << name << std::endl;

	return ret;
}

menu_node
main_menu::process_menu_node(const std::string &name, const ptree &tree)
{
	std::string description = tree.get<std::string>("description");
	menu_node ret(name, description);

	std::cout << "Entered menu node " << name << std::endl;

	for (const auto &node : tree.get_child("children")) {
		const std::string &node_name = node.first;
		const auto &subtree = node.second;
		ret.add_child(process_node(node_name, subtree));
	}

	std::cout << "Exited menu node " << name << std::endl;

	return ret;
}

std::unique_ptr<base_node>
main_menu::process_node(const std::string &name, const ptree &tree)
{
	std::string description = tree.get<std::string>("description");
	std::string type = tree.get<std::string>("type");

	if (type == "menu_node") {
		return std::unique_ptr<base_node>(new menu_node(process_menu_node(name, tree)));
	} else if (type == "value_node") {
		return std::unique_ptr<base_node>(new value_node(process_value_node(name, tree)));
	} else if (type == "struct_node" || type == "array_node") {
		std::cerr << "Unimplemented node type " << type << std::endl;
		std::cout << "Exited node " << name << std::endl;

		return std::unique_ptr<base_node>(new unimpl_node());
	} else {
		std::cerr << "Unknown node type " << type << std::endl;
		std::cout << "Exited node " << name << std::endl;

		return std::unique_ptr<base_node>(new invalid_node());
	}
}

#if 0
std::unique_ptr<menu_node>
load_menu(const std::string &filename)
{
	ptree tree;
	property_tree::read_json(filename, tree);
	const ptree &menu = tree.get_child("menu");

	return std::unique_ptr<menu_node>(new menu_node(process_menu_node("menu", menu)));
}
#endif
