#include "QStringUtils.h"

#include <QFile>
#include <QTextStream>
#include <QDate>
#include <QTime>


QList<QString> QStringUtils::splitLinesWindows(const QString& str){
	QList<QString> result;

	QString line;

	for (int i = 0; i < str.size(); i++) {

		if (str[i] == '\r' || str[i] == '\n')
		{
			if (i > 0)
			{
				if (str[i] == '\n' && str[i - 1] == '\r') {
					result.push_back(line);
					line = "";
				}
			}
		}
		else {
			line += str[i];
		}
	}

	if (line.size() > 0)
		result.push_back(line);

	return result;
}

QList<QString> QStringUtils::splitLinesUnix(const QString& str) {
	QList<QString> result;

	QString line;

	for (int i = 0; i < str.size(); i++) {

		if (str[i] == '\r')
		{
			result.push_back(line);
			line = "";
		}
		else {
			line += str[i];
		}
	}

	if (line.size() > 0)
		result.push_back(line);

	return result;
}

bool QStringUtils::loadStringFromFileWindows(QString& file_path, QString& str) {
		QFile file(file_path);

		str = "";

		if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
		{
			return false;
		}

		QTextStream in(&file);
		in.setCodec("UTF-8");

		//text = in.readAll();

		while (!in.atEnd())
		{
			QString line = in.readLine();
			str += line;
			if (!in.atEnd()) {
				str += "\r\n";
			}
		}

		in.flush();
		file.close();
}

bool QStringUtils::loadStringFromFileUnix(QString& file_path, QString& str) {
	QFile file(file_path);

	str = "";

	if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		return false;
	}

	QTextStream in(&file);
	in.setCodec("UTF-8");

	//text = in.readAll();

	while (!in.atEnd())
	{
		QString line = in.readLine();
		str += line;
		if (!in.atEnd()) {
			str += "\n";
		}
	}

	in.flush();
	file.close();
}

bool QStringUtils::StrToIntArray(QString& str, int* _array, int size) {
	QStringList items = str.split(",");

	if (items.size() != size)
		return false;

	for (int i = 0; i < size; i++) {
		bool ok = false;
		_array[i] = items[i].toInt(&ok);
		if (!ok)
			return false;
	}
	return true;
}

bool QStringUtils::StrToFloatArray(QString& str, float* _array, int size) {
	QStringList items = str.split(",");

	if (items.size() != size)
		return false;

	for (int i = 0; i < size; i++) {
		bool ok = false;
		_array[i] = items[i].toFloat(&ok);
		if (!ok)
			return false;
	}
	return true;
}

bool QStringUtils::StrToFloat(const QString& str, float& value) {
	bool ok;
	value = str.toFloat(&ok);
	return ok;
}

bool QStringUtils::StrToInt(const QString& str, int& value) {
	bool ok;
	value = str.toInt(&ok);
	return ok;
}

bool QStringUtils::StrToDuration(const QString& str, float& value) {
	QStringList items = str.split(":");

	if (items.size() != 2) {
		return false;
	}

	bool ok;
	value = items[0].toInt(&ok);
	
	if (!ok) {
		return false;
	}

	if (value < 0)
		return false;

	int val = items[1].toInt(&ok);

	if (!ok) {
		return false;
	}

	if (val < 0 || val > 59)
		return false;

	value += val / 60.0;
}

bool QStringUtils::StrToDateTime(const QString& str, QDateTime& value) {

	value = QDateTime();

	if (str.size() != 16)
		return false;

	if (str[4] != '/' || str[7] != '/' || str[13] != ':' || str[10] != ' ')
		return false;

	if (!(str[0].isNumber() && str[1].isNumber() && str[2].isNumber() && str[3].isNumber()))
		return false;

	if (!(str[5].isNumber() && str[6].isNumber() && str[8].isNumber() && str[9].isNumber()))
		return false;

	int year = str[0].digitValue() * 1000 + str[1].digitValue() * 100 + str[2].digitValue() * 10 + str[3].digitValue();
	int manth = str[5].digitValue() * 10 + str[6].digitValue();
	int day = str[8].digitValue() * 10 + str[9].digitValue();

	if (manth > 12)
		return false;

	value.date().setDate(year, manth, day);

	if (value.date().daysInMonth() > day)
		return false;

	if (!(str[11].isNumber() && str[12].isNumber() && str[14].isNumber() && str[15].isNumber()))
		return false;

	int hours = str[11].digitValue() * 10 + str[12].digitValue();
	int min = str[14].digitValue() * 10 + str[15].digitValue();

	if (hours > 23)
		return false;

	if (min > 59)
		return false;

	value.time().setHMS(hours, min, 0);
	
	return true;
}

bool QStringUtils::StrToDate(const QString& str, QDate& value) {
	value = QDate();

	if (str.size() != 10)
		return false;

	if (str[4] != '/' || str[7] != '/')
		return false;

	if (!(str[0].isNumber() && str[1].isNumber() && str[2].isNumber() && str[3].isNumber()))
		return false;

	if (!(str[5].isNumber() && str[6].isNumber() && str[8].isNumber() && str[9].isNumber()))
		return false;

	int year = str[0].digitValue() * 1000 + str[1].digitValue() * 100 + str[2].digitValue() * 10 + str[3].digitValue();
	int manth = str[5].digitValue() * 10 + str[6].digitValue();
	int day = str[8].digitValue() * 10 + str[9].digitValue();

	if (manth > 12)
		return false;

	value.setDate(year, manth, day);

	int ddays = value.daysInMonth();

	if (day > value.daysInMonth())
		return false;

	return true;
}

bool QStringUtils::StrToTime(const QString& str, QTime& value) {
	value = QTime();

	if (str.size() != 5)
		return false;

	if (str[2] != ':')
		return false;

	if (!(str[0].isNumber() && str[1].isNumber() && str[3].isNumber() && str[4].isNumber()))
		return false;

	int hours = str[0].digitValue() * 10 + str[1].digitValue();
	int min = str[3].digitValue() * 10 + str[4].digitValue();

	if (hours > 23)
		return false;

	if (min > 59)
		return false;

	value.setHMS(hours, min, 0);

	return true;

}


