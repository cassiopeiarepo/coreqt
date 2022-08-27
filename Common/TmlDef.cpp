#include "TmlDef.h"


TmlDef::TmlDef() {
	defineSimpleType(QString("bool"));
	defineSimpleType(QString("int"));
	defineSimpleType(QString("float"));
	defineSimpleType(QString("double"));
	defineSimpleType(QString("string"));

}

void TmlDef::defineSimpleType(QString& _name) {
	TmlDefTypeDef* type_def = new TmlDefTypeDef(_name, TmlDefTypeDef::SIMPLE);
	type_defs.append(type_def);
}

TmlDefTypeDef* TmlDef::defineRefType(QString& _name, TmlDefTypeDef* ref_type) {
	if (findType(_name) != NULL) return NULL;
	if (type_defs.findIndex(ref_type) < 0) return NULL;

	TmlDefTypeDef* type_def = new TmlDefTypeDef(_name, TmlDefTypeDef::REF);
	type_def->setRef(ref_type);
	type_defs.append(type_def);
	return type_def;
}

TmlDefTypeDef* TmlDef::defineRegExType(QString& _name, QString& _pattern) {
	if (findType(_name) != NULL) return NULL;

	TmlDefTypeDef* type_def = new TmlDefTypeDef(_name, TmlDefTypeDef::REGEX);
	type_def->setPattern(_pattern);
	type_defs.append(type_def);
	return type_def;
}

TmlDefTypeDef* TmlDef::defineEnumType(QString& _name) {
	if (findType(_name) != NULL) return NULL;

	TmlDefTypeDef* type_def = new TmlDefTypeDef(_name, TmlDefTypeDef::ENUM);
	type_defs.append(type_def);
	return type_def;
}

TmlDefTypeDef* TmlDef::findType(QString& _name) {
	for (int i = 0; i < type_defs.size(); i++) {
		if (type_defs[i]->getName() == _name) {
			return type_defs[i];
		}
	}

	return NULL;
}

TmlNodeDef* TmlDef::defineNodeDef(QString& type, TmlNodeDef* _derived) {
	if (findNodeDef(type) != NULL) return NULL;

	TmlNodeDef* node_def = defineNodeDef(type, _derived);
	node_defs.append(node_def);
	return node_def;
}

TmlNodeDef* TmlDef::findNodeDef(QString& _name) {
	for (int i = 0; i < node_defs.size(); i++) {
		if (node_defs[i]->getName() == _name) {
			return node_defs[i];
		}
	}

	return NULL;
}

bool TmlDef::load(QString& path) 
{
	TmlNode* node = new TmlNode();
	
	if (node->loadFile(path)) {
		if (node->getName() == "tmldef") {
			
			for (int i = 0; i < node->getNumChilds(); i++) {
				TmlNode* child = node->getChild(i);

				if (child->getName() == "typedef_ref")
				{
					QString name = child->getArgValue("name");
					QString type = child->getArgValue("type");

					if (name == "" || type == "") {
						return false;
					}

					TmlDefTypeDef* type_def_ref = findType(name);

					if (type_def_ref)
						return false;		// type with name exist

					type_def_ref = findType(type);

					if (type_def_ref == NULL)
						return false;

					TmlDefTypeDef* type_def = defineRefType(name, type_def_ref);
					if (type_def == NULL)
						return false;
				}
				else if (child->getName() == "typedef_regex") {
					QString name = child->getArgValue("name");
					QString pattern = child->getArgValue("pattern");

					if (name == "" || pattern == "") {
						return false;
					}

					TmlDefTypeDef* type_def_ref = findType(name);

					if (type_def_ref)
						return false;		// type with name exist

					TmlDefTypeDef* type_def = defineRegExType(name, pattern);
					if (type_def == NULL)
						return false;
					
				}
				else if (child->getName() == "typedef_enum") {
					QString name = child->getArgValue("name");

					if (name == "") {
						return false;
					}

					TmlDefTypeDef* type_def_ref = findType(name);

					if (type_def_ref)
						return false;		// type with name exist

					TmlDefTypeDef* type_def = defineEnumType(name);
					if (type_def == NULL)
						return false;

					for (int j = 0; j < child->getNumChilds(); j++) {
						TmlNode* enum_item = child->getChild(i);

						if (enum_item->getName() == "enumitem") {
							QString name = enum_item->getArgValue("name");
							if (name == "")
								return false;

							if (!type_def->isEnumItem(name)) {
								type_def->addEnumItem(name);
							}
						}

					}
				}

				else if (child->getName() == "nodedef") {
					QString _typename = child->getArgValue("type");
					TmlNodeDef* derived = NULL;

					if (_typename == "") {
						return false;
					}

					QString _derivedtype = child->getArgValue("derived");

					if (_derivedtype != "") {
						derived = findNodeDef(_derivedtype);
						if (derived == NULL) {
							return false;
						}
					}

					TmlNodeDef* node_def = defineNodeDef(_typename, derived);

					for (int i = 0; i < node->getNumChilds(); i++) {
						TmlNode* child = node->getChild(i);

						if (child->getName() == "nodedef_arg")
						{

						}
					}
				}
			}
		}
	}

}





