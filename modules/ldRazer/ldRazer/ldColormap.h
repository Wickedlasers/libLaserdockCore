#ifndef LDCOLORMAP_H //CHECK STYLE STAMP
#define LDCOLORMAP_H

//QT
#include <qlist.h>
#include <qfile.h>
#include <qjsondocument.h>
#include <qjsonobject.h>
#include <qjsonvalue.h>
#include <qjsonarray.h>
#include <qstring.h>

#include <ldCore/ldCore_global.h>

class ldColormap
{
public:
    static const uint32_t COLORMAP_ERROR = 16777216; //0xFFFFFF + 1

    //constructor
    ldColormap();
    ~ldColormap();

	/*ldColormap(const ldColormap&) = delete;*/
	ldColormap& operator=(const ldColormap& other);

    //functions
    //Add color to the end of the color map
    void addColor(uint32_t color);

    //Remove color in the given position, returns true if succeed, false otherwise.
    //Read errorMessage variable for more information
    bool removeColorAtPosition(int position);

    int size() const;

	uint32_t elementAt(int index) const;

	uint32_t &operator[] (int index);

    //get the color stored using values between 0 and 1
	uint32_t getColorByGradient(float nPercentage) const;

    //get the color stored using values between 0 and 100
	uint32_t getColorByGradientPercentage(float percentage) const;

    //get the first n colors stored
	QList<uint32_t> getNFirstColors(int n) const;

    //Returns a set of colors from start index position to end index position (both inclusive) only if both indexes aren't out of bound, otherwise a empty list is returned.
	QList<uint32_t> getSubsetColors(int start, int end) const;

	//Load from Qt Resources File .QRC
    bool loadFromResourcesFile(QString filename);

	//Load from LdCore Resources file
	bool loadFromldCoreResourcesFile(QString filename);

    QString errorMessage() const;
    QString name() const;


private:
    //Checks if the index is out of bounds and sets the error message if so.
    bool indexOutOfBounds(int requestedIndex) const;

	bool fillTheColorList(QFile* file);
    //fields
    QList<uint32_t> m_colors;
    mutable QString m_errorMessage;
    QString m_name;

};

#endif // LDCOLORMAP_H

