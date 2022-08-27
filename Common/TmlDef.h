#ifndef __TMLDEF_H__
#define __TMLDEF_H__

#include "Core/Utils/Str.h"
#include "Core/Utils/Vector.h"
#include "Core/Common/Tml.h"
#include "QString"

class TmlDefTypeDef {
public:

	enum Kind {
		SIMPLE, ENUM, REGEX, REF
	};

	TmlDefTypeDef(QString& _name, Kind _kind) : name(_name), kind(_kind) {}

	QString& getName() { return name;  }
	Kind getKind() { return kind; }

	TmlDefTypeDef* getRef() { return ref;  }
	void setRef(TmlDefTypeDef* _ref) { ref = _ref; }

	QString getPattern() { return pattern; }
	void setPattern(QString& _pattern) { pattern = _pattern; }

	void addEnumItem(QString& _item) {
		if (enumItems.findIndex(_item) >= 0) return;
		enumItems.append(QString(_item));
	}

	bool isEnumItem(QString& _item) {
		return (enumItems.findIndex(_item) >= 0);
	}

private:
	QString name;
	Kind kind;
	TmlDefTypeDef* ref;
	QString pattern;
	Vector<QString> enumItems;

};

class TmlNodeDefArg {
public:

	TmlNodeDefArg(QString& _name, TmlDefTypeDef* _type) : name(_name), type(_type), optional(false){ }

	QString& getName() { return name; }
	TmlDefTypeDef* getType() { return type; }

	bool getOptional() { return optional; }
	void setOptional(bool _optional) { optional = _optional; }

private:
	QString name;
	TmlDefTypeDef* type;
	bool optional;
};

class TmlNodeDefChild {
public:

	TmlNodeDefChild(QString& _name, TmlDefTypeDef* _type) : name(_name), type(_type) { }

	QString& getName() { return name; }
	TmlDefTypeDef* getType() { return type; }

	bool getOptional() { return optional; }
	void setOptional(bool _optional) { optional = _optional; }

private:
	QString name;
	TmlDefTypeDef* type;
	bool optional;
};

class TmlNodeDef {
public:

	TmlNodeDef(QString& _name, TmlNodeDef* _derived_type) : name(_name), derived(_derived_type) { }

	QString& getName() { return name; }
	TmlNodeDef* getDerived() { return derived; }

	int getNumArgs() { return args.size(); }
	TmlNodeDefArg* getArg(int index) { return args[index]; }
	void addArg(TmlNodeDefArg* arg) { args.append(arg);  }

private:
	QString name;
	TmlNodeDef* derived;
	Vector<TmlNodeDefArg*> args;

};

class TmlDef {

public:
	TmlDef();

	TmlDefTypeDef* defineRefType(QString& _name, TmlDefTypeDef* ref_type);
	TmlDefTypeDef* defineRegExType(QString& _name, QString& _pattern);
	TmlDefTypeDef* defineEnumType(QString& _name);
	TmlDefTypeDef* findType(QString& _name);

	TmlNodeDef* defineNodeDef(QString& type, TmlNodeDef* _derived);
	TmlNodeDef* findNodeDef(QString& _name);

	bool load(QString& path);

private:
	void defineSimpleType(QString& _name);

	Vector<TmlDefTypeDef*> type_defs;
	Vector<TmlNodeDef*> node_defs;
};



#endif /* __TMLDEF_H__ */
