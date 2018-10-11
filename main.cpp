#include <boost/property_tree/ptree.hpp>
// XXX: Should we use XML instead?
//#include <boost/property_tree/xml_parser.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <string>
#include <iostream>
#include <map>
#include <memory>
#include <cstdint>

#include "menu.h"

namespace property_tree = boost::property_tree;
using property_tree::ptree;

enum type_tag : uint8_t {
	TYPE_INT8 = 0,
	TYPE_UINT8,
	TYPE_INT16,
	TYPE_UINT16,
	TYPE_INT32,
	TYPE_UINT32,
};

enum struct_tag : uint8_t {
	STRUCT_INITCFG,
	STRUCT_PROFILE
};

struct ossc_setting_descriptor {
	// XXX: Should we have the menu path here?
	std::string name;
	std::string description;
	type_tag type;
	struct_tag loc;
	uint32_t loc_offset;

	// XXX: Maybe implement an "autounion" here?
	union {
		uint8_t  raw[4];
		int8_t   i8;
		uint8_t  u8;
		int16_t  i16;
		uint16_t u16;
		int32_t  i32;
		uint32_t u32;
	} value_default;
	union {
		uint8_t  raw[4];
		int8_t   i8;
		uint8_t  u8;
		int16_t  i16;
		uint16_t u16;
		int32_t  i32;
		uint32_t u32;
	} value_min;
	union {
		uint8_t  raw[4];
		int8_t   i8;
		uint8_t  u8;
		int16_t  i16;
		uint16_t u16;
		int32_t  i32;
		uint32_t u32;
	} value_max;

	// XXX: Maybe store the mapping of values to strings?
};

typedef std::map<std::string, ossc_setting_descriptor> settings_map;

struct ossc_fw_settings {
	uint8_t version_major;
	uint8_t version_minor;
	std::string version_suffix;

	settings_map settings;
};

typedef std::map<std::string, ossc_fw_settings> ossc_settings;

bool validate_setting(const ossc_setting_descriptor &sd, uint32_t value)
{
	return true;
}

std::unique_ptr<ossc_settings>
load_settings(const std::string &filename)
{
	std::unique_ptr<ossc_settings> ret(new ossc_settings);
	if (!ret)
		return ret;

	ptree tree;
	property_tree::read_json(filename, tree);
	for (const auto &fw_node : tree.get_child("settings")) {
		ossc_fw_settings fs;
		std::cout << fw_node.first << std::endl;

		auto &fw_value_tree = fw_node.second;
		fs.version_major = fw_value_tree.get<uint8_t>("version_major");
		fs.version_minor = fw_value_tree.get<uint8_t>("version_minor");
		fs.version_suffix = fw_value_tree.get<std::string>("version_suffix");
		std::cout << "\tversion: " << (int)fs.version_major << '.'
			      << (int)fs.version_minor << fs.version_suffix
			      << std::endl;

		for (const auto &node : fw_value_tree.get_child("setting_info")) {
			ossc_setting_descriptor sd;
			sd.name = node.first;
			std::cout << '\t' << sd.name << std::endl;

			auto &value_tree = node.second;
			sd.description = value_tree.get<std::string>("description");
			std::cout << "\t\tdescription: " << sd.description << std::endl;

			std::string type = value_tree.get<std::string>("type");
			if (type == "int8")
				sd.type = TYPE_INT8;
			else if (type == "uint8")
				sd.type = TYPE_UINT8;
			else if (type == "int16")
				sd.type = TYPE_INT16;
			else if (type == "uint16")
				sd.type = TYPE_UINT16;
			else if (type == "int32")
				sd.type = TYPE_INT32;
			else if (type == "uint32")
				sd.type = TYPE_UINT32;
			else {
				std::cerr << "Invalid type " << type << std::endl;
				exit(1);
			}
			std::cout << "\t\ttype: " << type << std::endl;

			std::string loc = value_tree.get<std::string>("loc");
			if (loc == "initcfg")
				sd.loc = STRUCT_INITCFG;
			else if (loc == "profile")
				sd.loc = STRUCT_PROFILE;
			else {
				std::cerr << "Invalid loc " << loc << std::endl;
				exit(1);
			}
			std::cout << "\t\tloc: " << loc << std::endl;
			sd.loc_offset = value_tree.get<uint32_t>("loc_offset");
			std::cout << "\t\tloc_offset: " << sd.loc_offset << std::endl;


			switch (sd.type) {
			case TYPE_INT8:
				sd.value_default.i8 = value_tree.get<int8_t>("value_default");
				sd.value_min.i8 = value_tree.get<int8_t>("value_min");
				sd.value_max.i8 = value_tree.get<int8_t>("value_max");
				std::cout << "\t\tvalue_default: " << (int)sd.value_default.i8 << std::endl;
				std::cout << "\t\tvalue_min: " << (int)sd.value_min.i8 << std::endl;
				std::cout << "\t\tvalue_max: " << (int)sd.value_max.i8 << std::endl;
				break;
			case TYPE_UINT8:
				sd.value_default.u8 = value_tree.get<uint8_t>("value_default");
				sd.value_min.u8 = value_tree.get<uint8_t>("value_min");
				sd.value_max.u8 = value_tree.get<uint8_t>("value_max");
				std::cout << "\t\tvalue_default: " << (unsigned int)sd.value_default.u8 << std::endl;
				std::cout << "\t\tvalue_min: " << (unsigned int)sd.value_min.u8 << std::endl;
				std::cout << "\t\tvalue_max: " << (unsigned int)sd.value_max.u8 << std::endl;
				break;
			case TYPE_INT16:
				sd.value_default.i16 = value_tree.get<int16_t>("value_default");
				sd.value_min.i16 = value_tree.get<int16_t>("value_min");
				sd.value_max.i16 = value_tree.get<int16_t>("value_max");
				std::cout << "\t\tvalue_default: " << sd.value_default.i16 << std::endl;
				std::cout << "\t\tvalue_min: " << sd.value_min.i16 << std::endl;
				std::cout << "\t\tvalue_max: " << sd.value_max.i16 << std::endl;
				break;
			case TYPE_UINT16:
				sd.value_default.u16 = value_tree.get<uint16_t>("value_default");
				sd.value_min.u16 = value_tree.get<uint16_t>("value_min");
				sd.value_max.u16 = value_tree.get<uint16_t>("value_max");
				std::cout << "\t\tvalue_default: " << sd.value_default.u16 << std::endl;
				std::cout << "\t\tvalue_min: " << sd.value_min.u16 << std::endl;
				std::cout << "\t\tvalue_max: " << sd.value_max.u16 << std::endl;
				break;
			case TYPE_INT32:
				sd.value_default.i32 = value_tree.get<int32_t>("value_default");
				sd.value_min.i32 = value_tree.get<int32_t>("value_min");
				sd.value_max.i32 = value_tree.get<int32_t>("value_max");
				std::cout << "\t\tvalue_default: " << sd.value_default.i32 << std::endl;
				std::cout << "\t\tvalue_min: " << sd.value_min.i32 << std::endl;
				std::cout << "\t\tvalue_max: " << sd.value_max.i32 << std::endl;
				break;
			case TYPE_UINT32:
				sd.value_default.u32 = value_tree.get<uint32_t>("value_default");
				sd.value_min.u32 = value_tree.get<uint32_t>("value_min");
				sd.value_max.u32 = value_tree.get<uint32_t>("value_max");
				std::cout << "\t\tvalue_default: " << sd.value_default.u32 << std::endl;
				std::cout << "\t\tvalue_min: " << sd.value_min.u32 << std::endl;
				std::cout << "\t\tvalue_max: " << sd.value_max.u32 << std::endl;
				break;
			}

			fs.settings.insert(std::make_pair(sd.name, sd));
		}

		ret->insert(std::make_pair(fw_node.first, fs));
	}

	return ret;
}

void print_settings(const ossc_settings &os)
{
	for (const auto &fw_it : os) {
		auto &fw_name = fw_it.first;
		auto &fw_s = fw_it.second;

		std::cout << fw_name << std::endl;
		std::cout << "\tversion: " << (int)fw_s.version_major << '.'
			      << (int)fw_s.version_minor << fw_s.version_suffix
			      << std::endl;

		for (const auto &s : fw_s.settings) {
			auto &setting_name = s.first;
			auto &setting = s.second;

			std::cout << '\t' << setting_name << std::endl;
			std::cout << "\t\tname: " << setting.name << std::endl;
			std::cout << "\t\tdescription: " << setting.description << std::endl;
			std::cout << "\t\ttype: ";
			switch (setting.type) {
			case TYPE_INT8:
				std::cout << "int8" << std::endl;
				break;
			case TYPE_UINT8:
				std::cout << "uint8" << std::endl;
				break;
			case TYPE_INT16:
				std::cout << "int16" << std::endl;
				break;
			case TYPE_UINT16:
				std::cout << "uint16" << std::endl;
				break;
			case TYPE_INT32:
				std::cout << "int32" << std::endl;
				break;
			case TYPE_UINT32:
				std::cout << "uint32" << std::endl;
				break;
			}

			std::cout << "\t\tloc: ";// << setting.loc << std::endl;
			switch (setting.loc) {
			case STRUCT_INITCFG:
				std::cout << "initcfg" << std::endl;
				break;
			case STRUCT_PROFILE:
				std::cout << "profile" << std::endl;
			}
			std::cout << "\t\tloc_offset: " << setting.loc_offset << std::endl;

			switch (setting.type) {
			case TYPE_INT8:
				std::cout << "\t\tvalue_default: " << (int)setting.value_default.i8 << std::endl;
				std::cout << "\t\tvalue_min: " << (int)setting.value_min.i8 << std::endl;
				std::cout << "\t\tvalue_max: " << (int)setting.value_max.i8 << std::endl;
				break;
			case TYPE_UINT8:
				std::cout << "\t\tvalue_default: " << (unsigned int)setting.value_default.u8 << std::endl;
				std::cout << "\t\tvalue_min: " << (unsigned int)setting.value_min.u8 << std::endl;
				std::cout << "\t\tvalue_max: " << (unsigned int)setting.value_max.u8 << std::endl;
				break;
			case TYPE_INT16:
				std::cout << "\t\tvalue_default: " << setting.value_default.i16 << std::endl;
				std::cout << "\t\tvalue_min: " << setting.value_min.i16 << std::endl;
				std::cout << "\t\tvalue_max: " << setting.value_max.i16 << std::endl;
				break;
			case TYPE_UINT16:
				std::cout << "\t\tvalue_default: " << setting.value_default.u16 << std::endl;
				std::cout << "\t\tvalue_min: " << setting.value_min.u16 << std::endl;
				std::cout << "\t\tvalue_max: " << setting.value_max.u16 << std::endl;
				break;
			case TYPE_INT32:
				std::cout << "\t\tvalue_default: " << setting.value_default.i32 << std::endl;
				std::cout << "\t\tvalue_min: " << setting.value_min.i32 << std::endl;
				std::cout << "\t\tvalue_max: " << setting.value_max.i32 << std::endl;
				break;
			case TYPE_UINT32:
				std::cout << "\t\tvalue_default: " << setting.value_default.u32 << std::endl;
				std::cout << "\t\tvalue_min: " << setting.value_min.u32 << std::endl;
				std::cout << "\t\tvalue_max: " << setting.value_max.u32 << std::endl;
				break;
			}
		}
	}
}

int main()
{
	ptree tree;

	ptree::path_type fw1_path("settings:0.78a", ':');
	auto &fw1_tree = tree.put_child(fw1_path, ptree());
	fw1_tree.put("version_major", 0);
	fw1_tree.put("version_minor", 78);
	fw1_tree.put("version_suffix", "a");
	fw1_tree.put("setting_info.foo.description", 1337);
	fw1_tree.put("setting_info.foo.type", "uint16");
	fw1_tree.put("setting_info.foo.loc", "initcfg");
	fw1_tree.put("setting_info.foo.loc_offset", 18);
	fw1_tree.put("setting_info.foo.value_default", 1337);
	fw1_tree.put("setting_info.foo.value_min", 0);
	fw1_tree.put("setting_info.foo.value_max", 7175);
	{
		ptree arr;
		for (size_t i = 0; i < 5; ++i) {
			ptree node;
			node.put("", i);
			arr.push_back(std::make_pair("", node));
		}
		fw1_tree.put_child("setting_info.foo.array1", arr);
	}
	{
		ptree arr;
		for (size_t i = 0; i < 5; ++i) {
			ptree node;
			node.put("foo", i);
			arr.push_back(std::make_pair("", node));
		}
		fw1_tree.put_child("setting_info.foo.array2", arr);
	}

	ptree::path_type fw2_path("settings:1.79", ':');
	auto &fw2_tree = tree.put_child(fw2_path, ptree());
	fw2_tree.put("version_major", 1);
	fw2_tree.put("version_minor", 79);
	fw2_tree.put("version_suffix", "");
	fw2_tree.put("setting_info.bar.description", 80085);
	fw2_tree.put("setting_info.bar.type", "int8");
	fw2_tree.put("setting_info.bar.loc", "profile");
	fw2_tree.put("setting_info.bar.loc_offset", 325);
	fw2_tree.put("setting_info.bar.value_default", -5);
	fw2_tree.put("setting_info.bar.value_min", -128);
	fw2_tree.put("setting_info.bar.value_max", 127);

	//tree.put("settings.baz.value", 715517);

	const std::string filename = "settings_map.json";
	property_tree::write_json(filename, tree);

	auto settings = load_settings(filename);

	std::cout << "Verifying:" << std::endl;
	print_settings(*settings);

	ptree menu_tree;
	menu_tree.put("menu.description", "root of menu");
	menu_tree.put("menu.type", "menu_node");
	menu_tree.put("menu.children.sub1.description", "value 1");
	menu_tree.put("menu.children.sub1.type", "value_node");
	menu_tree.put("menu.children.sub1.value_default", 1);
	menu_tree.put("menu.children.sub1.value_min", 0);
	menu_tree.put("menu.children.sub1.value_max", 2);
	menu_tree.put("menu.children.sub1.value_descriptions.0", "foo");
	menu_tree.put("menu.children.sub1.value_descriptions.1", "bar");
	menu_tree.put("menu.children.sub1.value_descriptions.2", "baz");
	menu_tree.put("menu.children.sub2.description", "menu 1");
	menu_tree.put("menu.children.sub2.type", "menu_node");
	menu_tree.put("menu.children.sub2.children.sub1.description", "value 2");
	menu_tree.put("menu.children.sub2.children.sub1.type", "value_node");
	menu_tree.put("menu.children.sub2.children.sub1.value_default", 17);
	menu_tree.put("menu.children.sub2.children.sub1.value_min", 5);
	menu_tree.put("menu.children.sub2.children.sub1.value_max", 99);

	const std::string menu_filename = "menu.json";
	property_tree::write_json(menu_filename, menu_tree);

	main_menu menu(menu_filename);
	menu.get_menu().show();
}
