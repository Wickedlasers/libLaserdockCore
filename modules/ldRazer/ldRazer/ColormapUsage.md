# Colormap 

## What is a colormap

Colormap is a structure that holds a collection of colors. In the current implementation a colormap contains an array of unsigned integers that are the decimal representation of colors and a string field that holds the name of the colormap. 

Colormaps can be  code created and loaded from file. With the colormap the colors are easily accesible by code and helps keeping your code clean. Import this new structure using the following clauses.

~~~
#include "ldRazer/ldColormapManager.h"
#include "ldRazer/ldColormap.h"
~~~

## How to load a colormap 

Colormap can be loaded from json file. This file can be located in the Qt resources folder or in the ldCore resources folder. loading can be achieved by using "loadColormap" call in the ldColormapManager class provinding the file name and the loading method
~~~
//To load it from Qt Resources
ldColormapManager::getInstance().loadColormap((QString) ":/colormaps/rgb.json", ldColormapManager::LOAD_STYLE::QT_RESOURCES);

//To load it from ldCoreResources
ldColormapManager::getInstance().loadColormap((QString) ":/colormaps/rgb.json", ldColormapManager::LOAD_STYLE::QT_RESOURCES);

ldColormapManager::getInstance().loadColormap((QString) "/colormaps/rgb.json", ldColormapManager::LOAD_STYLE::LDCORE_RESOURCES);
~~~

In order to avoid unnecesary calls checking if the colormap is already in memory is recommended

~~~
if (!ldColormapManager::getInstance().existColormap("rgb"))
{
	ldColormapManager::getInstance().loadColormap((QString) ":/colormaps/rgb.json", ldColormapManager::LOAD_STYLE::QT_RESOURCES);
}
~~~

## Get the colormap located in memory

After loading the colormap into memory, the colormap can be accessed by using the following call
~~~~ 
ldColormap rgbColormap = ldColormapManager::getInstance().getColormap("rgb");
~~~~ 

And the color can be accessed by index in the two following ways:

~~~
rgbColormap.elementAt(0); //index checking
rgbColormap[0]; //not index cheking is done.
~~~

Besides color can be accessed by float or by percentage:

~~~
rgbColormap.getColorByGradient(0.5); //Takes the color in the middle of the color map using values from 0 to 1
rgbColormap.getColorByGradientPercentage(50); //Takes the color in the middle of the color map using values 0 to 100
~~~

Also subset of colors can be accessed by using the following calls:

~~~
rgbColormap.getNFirstColors(2); //Gets the two first colors
rgbColormap.getSubsetColors(1,2); //Takes the colors from position from 1 to 2 (0-indexed)
~~~
