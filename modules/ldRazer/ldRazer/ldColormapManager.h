
#ifndef LDCOLORMAPMANAGER_H //CHECK STYLE STAMP
#define LDCOLORMAPMANAGER_H

#include <qmap.h>
#include "ldColormap.h"

class ldColormapManager
{
public:

	enum LOAD_STYLE
	{
		QT_RESOURCES,
		LDCORE_RESOURCES
	};

	static ldColormapManager& getInstance()
	{
		static ldColormapManager instance;
		return instance;
	}


	int size() const;
	
	//Load Colormap from Qt or ldCore resources using the given filename. True is loaded in memory, false otherwise
	bool loadColormap(QString filename, LOAD_STYLE loadStyle);

	//Checks if the colormap exists in memory.
	bool existColormap(QString colormapName);

	//Return the colormap loaded in memory otherwise returns the default constructed value created by QMap
	ldColormap getColormap(QString colormapName);

	void insert(ldColormap colormap);

private:

	ldColormapManager();
	~ldColormapManager();
	
	QMap<QString, ldColormap> m_loadedMaps;

};

#endif 

