#ifndef MENU_H
#define MENU_H

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <iostream>
#include <cstdint>
#include <boost/lexical_cast.hpp>
#include <boost/property_tree/ptree.hpp>
// XXX: Should we use XML instead?
//#include <boost/property_tree/xml_parser.hpp>
#include <boost/property_tree/json_parser.hpp>

namespace property_tree = boost::property_tree;
using property_tree::ptree;

struct base_node {
protected:
	std::string name;
	std::string description;
	// XXX: need for a parent?

public:
	base_node(const std::string &name_, const std::string &desc)
		: name(name_), description(desc)
	{ }

	virtual ~base_node()
	{ }

	base_node& operator=(base_node &&n)
	{
		if (this != &n) {
			name = std::move(n.name);
			description = std::move(n.description);
		}
		return *this;
	}

	const std::string& get_name() const
	{
		return name;
	}

	const std::string& get_description() const
	{
		return description;
	}

	virtual void show() = 0;
};

struct unimpl_node : public base_node {
	unimpl_node() : base_node("unimplemented", "unimplemented")
	{ }

	virtual void show()
	{
		std::cout << "ERROR: unimplemented" << std::endl;
	}
};

struct invalid_node : public base_node {
	invalid_node() : base_node("invalid", "invalid")
	{ }

	virtual void show()
	{
		std::cout << "ERROR: invalid" << std::endl;
	}
};

struct value_node : public base_node {
	typedef uint32_t value_type;
	typedef std::map<value_type, std::string> value_descr_map;

protected:
	value_type value_default;
	value_type value_min;
	value_type value_max;
	value_type value;
	value_descr_map value_descriptions;

public:
	value_node(const std::string &name_, const std::string &desc,
		value_type v_def=0, value_type v_min=0, value_type v_max=0)
		: base_node(name_, desc), value_default(v_def),
		  value_min(v_min), value_max(v_max), value(v_def),
		  value_descriptions(value_descr_map())
	{ }

	value_node(value_node &&n)
		: base_node(std::move(n.name), std::move(n.description)),
		  value_default(n.value_default), value_min(n.value_min),
		  value_max(n.value_max), value(n.value),
		  value_descriptions(std::move(n.value_descriptions))
	{ }

#if 0
	virtual ~value_node()
	{ }
#endif

	value_node& operator=(value_node &&n)
	{
		if (this != &n) {
			base_node::operator=(std::move(n));
			value_default = n.value_default;
			value_min = n.value_min;
			value_max = n.value_max;
			value = n.value;
			value_descriptions = std::move(n.value_descriptions);
		}
		return *this;
	}


	bool set_value(value_type v)
	{
		if (v < value_min || v > value_max)
			return false;
		value = v;
		return true;
	}

	void set_default_value()
	{
		set_value(value_default);
	}

	value_type get_value() const
	{
		return value;
	}

	bool add_value_description(value_type v, const std::string &s)
	{
		if (v < value_min || v > value_max)
			return false;
		return value_descriptions.insert(std::make_pair(v, s)).second;
	}

	const value_descr_map& get_value_descriptions() const
	{
		return value_descriptions;
	}

	virtual void show()
	{
		while (true) {
			std::cout << description << std::endl;

			const auto &curr_val_desc = value_descriptions.find(value);
			if (curr_val_desc != value_descriptions.end())
				std::cout << "\tCurrent value: " << curr_val_desc->second
					      << std::endl;
			else
				std::cout << "\tCurrent value: " << value << std::endl;

			std::cout << "Set value in the range [" << value_min << "-"
				  << value_max << "] (b to go back):" << std::endl;
			for (auto v = value_min; v <= value_max; ++v) {
				const auto &val_desc = value_descriptions.find(v);
				if (val_desc != value_descriptions.end()) {			
					if (v == value)
						std::cout << "\t[" << v << "] => "
							  << val_desc->second << std::endl;
					else
						std::cout << "\t " << v << "  => "
							  << val_desc->second << std::endl;
				}
			}
			std::cout << "> " << std::flush;
			value_type new_val;
			std::string input;
			std::cin >> input;
			if (input == "b")
				break;
			else
				new_val = boost::lexical_cast<value_type>(input);

			if (!set_value(new_val))
				std::cout << "Invalid value!" << std::endl;
		}
	}
};

struct menu_node : public base_node {
	typedef std::unique_ptr<base_node> child_type;
	typedef std::vector<child_type> child_vector;

protected:
	child_vector children;

public:
	menu_node(const std::string &name_, const std::string &desc)
		: base_node(name_, desc), children(child_vector())
	{ }

	menu_node(menu_node &&n)
		: base_node(std::move(n.name), std::move(n.description)),
		  children(std::move(n.children))
	{ }

#if 0
	virtual ~menu_node()
	{ }
#endif

	menu_node& operator=(menu_node &&n)
	{
		if (this != &n) {
			base_node::operator=(std::move(n));
			children = std::move(n.children);
		}
		return *this;
	}

#if 0
	bool add_child(const menu_node &child)
	{
		if (value_set)
			return false;
		children.push_back(child_type(new menu_node(child)));
		return true;
	}
#endif

	bool add_child(menu_node &&child)
	{
		children.push_back(child_type(new menu_node(std::move(child))));
		return true;
	}

	bool add_child(child_type &&child)
	{
		children.push_back(std::move(child));
		return true;
	}

	bool has_children() const
	{
		return !children.empty();
	}

	const child_vector& get_children() const
	{
		return children;
	}

	virtual void show()
	{
		while (true) {
			std::cout << description << std::endl;

			std::cout << "Select menu item (b to go back):" << std::endl;
			for (size_t i = 0; i < children.size(); ++i) {
				const auto &child = children[i];
				std::cout << "\t" << i << " => " << child->get_description()
				          << std::endl;
			}
			std::cout << "> " << std::flush;
			size_t selected;
			std::string input;
			std::cin >> input;
			if (input == "b")
				break;
			else
				selected = boost::lexical_cast<size_t>(input);

			if (selected >= children.size())
				std::cout << "Invalid selection!" << std::endl;
			else
				children[selected]->show();
		}
	}

};

class main_menu {
	menu_node menu;

	static value_node
	process_value_node(const std::string &name, const ptree &tree);
	static menu_node
	process_menu_node(const std::string &name, const ptree &tree);
	static std::unique_ptr<base_node>
	process_node(const std::string &name, const ptree &tree);
public:
	main_menu(const std::string &filename) : menu("temp", "temp")
	{
		ptree tree;
		property_tree::read_json(filename, tree);
		const ptree &menu_tree = tree.get_child("menu");

		menu = process_menu_node("main_menu", menu_tree);
	}

	menu_node& get_menu()
	{
		return menu;
	}
};

//std::unique_ptr<menu_node> load_menu(const std::string &filename);
#endif
