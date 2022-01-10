#include "ldColormap.h"

#include <QtCore/QtDebug>

#include <cmath>

#include <ldCore/ldCore.h>

ldColormap::ldColormap()
{
}


ldColormap::~ldColormap()
{
}

void ldColormap::addColor(uint32_t color)
{
    m_colors.push_back(color);
}

bool ldColormap::removeColorAtPosition(int position)
{
    bool result = true;

    if (indexOutOfBounds(position)) {
        result = false;
    } 
	else {
        m_colors.removeAt(position);
    }

    return result;
}

int ldColormap::size() const
{
    return m_colors.size();
}

uint32_t ldColormap::elementAt(int index) const
{
    if (indexOutOfBounds(index)) {
        return COLORMAP_ERROR;
    }

    return m_colors[index];
}

//Getters
uint32_t &ldColormap::operator[](int index)
{
    return m_colors[index];
}


ldColormap& ldColormap::operator=(const ldColormap& other)
{
	if (this != &other) { // protect against invalid self-assignment

		this->m_colors.clear(); //remove old colors

		for (int i = 0; i < other.m_colors.size(); i++)
		{
			this->m_colors.append(other.m_colors[i]);
		}

        this->m_name = other.m_name;
	}
	// by convention, always return *this
	return *this;
}

uint32_t ldColormap::getColorByGradient(float nPercentage) const //nPercentage => percentage normalized to have values from 0 to 1
{
    nPercentage = nPercentage > 1.0f ? 1.0f : nPercentage;
    nPercentage = nPercentage < 0.f ? 0 : nPercentage;

    float floatIndex = std::floor(nPercentage * ((float)m_colors.size())) - 1.0f ; //0-indexing
    floatIndex = floatIndex < 0.0f ? 0 : floatIndex;

    return m_colors[floatIndex];
}

uint32_t ldColormap::getColorByGradientPercentage(float percentage) const
{
    float nPercentage = percentage / 100.0f;
    return getColorByGradient(nPercentage);
}

QList<uint32_t> ldColormap::getNFirstColors(int n) const
{
	QList<uint32_t> result;

    if (n > m_colors.size()) {
        result = m_colors;
    } 
	else {
        for (int i = 0; i < n; i++) {
            result.append(m_colors[i]);
        }
    }

    return result;
}

QList<uint32_t> ldColormap::getSubsetColors(int start, int end) const
{
	QList<uint32_t> result;

    if (!indexOutOfBounds(start) && !indexOutOfBounds(end)) {
        for (int i = start; i <= end; i++) {
            result.append(i);
        }
    }

    return result;
}

bool ldColormap::loadFromResourcesFile(QString filename) 
{
    QFile file(filename);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        m_errorMessage = "COLORMAP: Cannot open file " + filename;
        qWarning() << m_errorMessage;
        return false;
    }
	return fillTheColorList(&file);
    
}

//Load from LdCore Resources file
bool ldColormap::loadFromldCoreResourcesFile(QString filename)
{
	QFile file(ldCore::instance()->resourceDir() + filename);
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
		m_errorMessage = "COLORMAP: Cannot open file";
		return false;
	}
	return true;
}

bool ldColormap::fillTheColorList(QFile* file) 
{
	bool result = true;

	QString fileContent = file->readAll();
	file->close();

	QJsonDocument jDocument = QJsonDocument::fromJson(fileContent.toUtf8());
	if (!jDocument.isNull())
	{
		const QJsonObject jObject = jDocument.object();
		if (!jObject.isEmpty()) {
			QJsonArray jColorValues = jObject["colors"].toArray(); //if null an empty array so the rest of the code is safe

			for (int i = 0; i < jColorValues.size(); i++)
				m_colors.append(jColorValues[i].toInt());

            this->m_name = jObject["name"].toString();
		}
		else
			result = false;
	}
	else
		result = false;
	
	return result;
}

QString ldColormap::errorMessage() const
{
    return m_errorMessage;
}

QString ldColormap::name() const
{
    return m_name;
}

//Private section
bool ldColormap::indexOutOfBounds(int requestedIndex) const
{
    bool result = false;

    if (requestedIndex < 0 || requestedIndex > m_colors.size() - 1)
    {
        m_errorMessage = "COLORMAP: Asking a color out of bounds of the colormap";
        result = true;
    }

    return result;
}
