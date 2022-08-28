#ifndef __TML_H__
#define __TML_H__

#include "core/Types/Str.h"
#include "core/Types/Vector.h"
#include "QString"

class TmlItem {
public:
	enum Kind {
		NODE, ARGUMENT //, IDENT, LARROW, RARROW, EQ, WHITESPACE, NEW_LINE, ARGVALUE, SLASH, DATA
	};

	virtual Kind getKind() = 0;

	int line;
	int pos;
};

class TmlArgument : public TmlItem {
public:

    Kind getKind() override { return TmlItem::ARGUMENT; }

	void setName(const QString& name) { arg_name = name;  }
	const QString& getName() const { return arg_name; }

	void setValue(const QString& value) { arg_value = value; }
	const QString& getValue() const { return arg_value; }

    //void setRowData(const QString& value) { row_data = value; }
    //const QString& getRowData() const { return row_data; }

private:
	QString arg_name;
	QString arg_value;

    //QString row_data;
};

class TmlSource {

public:
    TmlSource() : error(false), error_line(0), error_pos(0), error_msg(""), line(0), pos(-1) {}
    
    enum {
        EOS = 0,
        EOL = 13
    };

    virtual QChar next() = 0;

    virtual QChar forward() = 0;

    void doError(const char* msg) { 
        error_line = line; error_pos = pos; error_msg = msg; error = true;
    }

    void doError(const char* msg, int line, int pos) {
        error_line = line; error_pos = pos; error_msg = msg; error = true;
    }

    bool error;
    int error_line;
    int error_pos;
    QString error_msg;

    QChar ch;
    int line;
    int pos;
};

class TmlStringSource : public TmlSource {
public:
    TmlStringSource(const QString& _src) : TmlSource(), src(_src), src_pos(-1) { 
        src_size = src.size();
        next();
    }

    QChar next() override;
    QChar forward() override;
private:
    const QString& src;
    int src_pos;
    int src_size;
};


class TmlNode : public TmlItem {

public:

    ~TmlNode();

    bool parse(const QString& src);
    void parse(TmlSource* source);

    bool loadFile(const QString& path);

    QString toString(int max_line_size);

    Kind getKind() override { return TmlItem::NODE; }

    void setName(const QString& _name) { name = _name; }
    const QString& getName() const { return name; }


    TmlNode* createChild(const QString& _name);
    int getNumChilds() const { return childs.size(); }
    TmlNode* getChild(int index) { if (index >= childs.size()) return NULL; return childs[index]; }
    TmlNode* getChild(const QString& name);
    int getChildIndex(const QString& name);
    bool removeChild(int index);

    bool isArg(const QString& _name) const { return getArg(_name) != NULL; }
    int getNumArgs() const { return args.size(); }
    TmlArgument* getArg(int index) const { if (index >= args.size()) return NULL; return args[index]; }
    TmlArgument* getArg(const QString& _name) const;
    void setArgValue(const QString& name, const QString& value);
    QString getArgValue(const QString& name);

    void setData(const QString& _data) { data = _data; }
    QString& getData() { return data; }

    void setEmptyElement(bool empty) { empty_element = empty; }
    bool getEmptyElement() { return empty_element; }

    void clear();

    /*
    int setBoolArg(const char* name, bool value);
    bool getBoolArg(const char* name) const;

    int setIntArg(const char* name, int value);
    int getIntArg(const char* name) const;

    int setFloatArg(const char* name, float value);
    float getFloatArg(const char* name) const;

    int setDoubleArg(const char* name, double value);
    double getDoubleArg(const char* name) const;
    */

private:
    QString name;
    QString data;
    bool empty_element;
    Vector<TmlArgument*> args;

    TmlNode* parent;
    Vector<TmlNode*> childs;

    void parseNode(TmlSource* source, TmlNode* parent);
    QString parseIdent(TmlSource* source);
    QString parseArgValue(TmlSource* source);
    QString parseData(TmlSource* source);

    void skipWhitespace(TmlSource* source);
    
    void exportNode(QString& str, TmlNode* node, TmlNode* root, int max_line_size);
    int nodeDephth(TmlNode* node, TmlNode* root);
    QString genSpaces(int num);

    QString decodeString(const QString& in);
    QString encodeString(const QString& in);
};

void testTml(QString& path);


/*

class TmlIdent : public TmlItem {
public:

    Kind getKind() override { return TmlItem::IDENT; }
    
    void setData(const char* _data) { data = _data; }
    QString& getData() { return data; }

private:
    QString data;
};

class TmlLArrow : public TmlItem {
public:

    Kind getKind() override { return TmlItem::LARROW; }

};

class TmlRArrow : public TmlItem {
public:

    Kind getKind() override { return TmlItem::RARROW; }

};

class TmlEQ : public TmlItem {
public:

    Kind getKind() override { return TmlItem::EQ; }

};

class TmlWhitespace : public TmlItem {
public:

    Kind getKind() override { return TmlItem::WHITESPACE; }

    void setData(const char* _data) { data = _data; }
    QString& getData() { return data; }

private:
    QString data;
};

class TmlArgValue : public TmlItem {
public:

    Kind getKind() override { return TmlItem::ARGVALUE; }

    void setData(const char* _data) { data = _data; }
    QString& getData() { return data; }

private:
    QString data;
};

class TmlNewLine : public TmlItem {
public:

    Kind getKind() override { return TmlItem::NEW_LINE; }

};

class TmlSlash : public TmlItem {
public:

    Kind getKind() override { return TmlItem::SLASH; }

};

class TmlData : public TmlItem {
public:

    Kind getKind() override { return TmlItem::DATA; }

    void setData(const char* _data) { data = _data; }
    QString& getData() { return data; }

private:
    QString data;
};

*/

#endif /* __MDML_PARSER_H__ */
