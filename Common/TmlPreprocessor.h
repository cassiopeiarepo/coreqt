#ifndef __TML_PREPROCESSOR_H__
#define __TML_PREPROCESSOR_H__

#include "core/types/Str.h"
#include "core/types/Vector.h"
#include "coreqt/Common/Tml.h"

#include <QString>
#include <QFileInfo>

class TmlPreprocessorItem {
	TmlNode* node;
	QFileInfo fi;
	int line;
	int pos;
};


class TmlPreprocessor {

	TmlPreprocessor();

	//file_type : c
	void parseFile(const QString& file_path, const QString& file_type);

	void parseDir(const QString& dir_path, const QString& file_type);

	const Vector<TmlPreprocessorItem*> getItems() { return items; }

protected:

	QChar next();
	QChar forward();

	void parse_c(const QFileInfo& fi);

	Vector<TmlPreprocessorItem*> items;

	QString input;
	QChar ch;
	int str_pos;
	int src_size;
	int line;
	int pos;
};

#endif /* __TML_PREPROCESSOR_H__ */
