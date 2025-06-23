# libLaserdockCore
cmake repository for laserdock library with WiFi Cube Support

Windows/Mac/Linux: [![Build status](https://ci.appveyor.com/api/projects/status/mdaio12e06dyvjh9/branch/master?svg=true)](https://ci.appveyor.com/project/sergeywl/liblaserdockcore/branch/master)

# Supported platforms

Windows (msvc2019 x86)
<br>
macOS (clang)
<br>
android 
<br>
Linux

# Prepare to development

* Download Qt Online installer from https://www.qt.io/download-qt-installer
* Run installer and install Qt for mac/windows (the latest version is recommended, library was tested with Qt 6.7)

### Qt Creator:
* Check Qt Creator Preferences -> Kits -> Kits page for any warnings
* If you need Android build ensure that page Preferences ->  SDKs is filled

# How to start:

### Qt Creator:

1) Open root CMakeLists.txt. 
2) Setup build dirs and create them manually. Qt Creator won't do it until you check the corresponding checkbox in Preferences -> Kits -> Cmake. However this checkbox can create unnecessary dirs if you decide to change build dir later.
3) Build & run

## Windows visual studio build (can be broken since nobody uses it)

1) mkdir build && cd build

2) cmake -G "Visual Studio 16 2019" -A Win32 -DQTDIR="C:\dev\Qt\5.15.2\msvc2019" ..

or

   cmake -G "Visual Studio 16 2019" -A x64 -DQTDIR="C:\dev\Qt\5.15.2\msvc2019_64" ..

3) cmake -- build .

(or run .sln file)


# Code Style
Code style helps to read and edit project easier to all of us. It is not strict policy but more like general recommendations.

### Common
Do not use exceptions.     

Do not use tabs symbols in code. You can setup your IDE to save tabs as spaces (1 tab is 4 spaces).

Try to declare float variables with _f_ suffix in new code - _1.0f_ or _1.f_ instead of _1.0_ or _1_

Try to avoid copy-paste.

##### Containers
Try to use std containers instead of Qt containers if it is possible. STD containers are more 'clear' than Qt. In some cases you have no choice and should use Qt container (if you want to use QStringList as **Q_PROPERTY** for example).

### Naming

##### Fields
Recommended class field prefix is _m\_\*_, but try to keep the same style if some class has other prefix.

We often use _\_\*_ as prefix. It is a bit dangerous however. 
<br>
С++ has some restrictions for underscore prefix usage by standard. _\_\*_ is reserved to use in global namespace.
In ANY namespace standard reserves _\__\*_ (2 underscores prefix) and _\_A\*_ (underscore + any capital letter) for compiler internal usage so you should never use them.

##### 
Try to give significant names to variables and functions and avoid short names (except some obvious one like x,y, etc).

##### Classes
Common prefix for all classes/structs is _ld\*_. If some class is strictly connected to one mini-app, you can use app name as second prefix - _ldTunes\*_, _ldClock\*_, but app name can be changed later and we should be careful with it. Many classes are started with _ldVisualizer\*_ by historical reason but not related to *Visualizer* app.

I used _\*WindowLogic_ classes before but it is better to avoid them and use plain good-structured objects in QML directly. QQmlHelpers library has some very useful macros for properties.


##### Member Order
Recommended class member order: _public, public slots, signals, protected slots, protected, private slots, private_. In each section order is: _enum, static functions, constructor, functions, fields_. Static fields should be avoided generally.

##### Include Order
From common to local:

\<C++ std module\>
<br>
\<Qt Module with explicit submodule> 
<br>
\<Windows/mac/linux specific>
<br>
\<3rdparty library> 
<br>
\<ldCore/../..>
<br>
"LaserdocVisualizer/../.." 
<br>
"Subfolder/ldClass.h" 

Try to sort includes in each include subgroup in alphabetical order. 
<br>Class forward declarations are preferred to Include directives.
<br>in .cpp file the first inlcude should be current ".h" file

### C++11


##### for
When it is possible try to use C++11 syntax _for_ with iterators, e.g _for(const ldClass &myClass : myClasses)_ instead of index-based for. It helps to avoid unnecessary variable for index in most cases and easier to read. If you need index than use old-style _for_ for sure. An try to avoid index names like i, j, k.

##### Const correctness
Pass all classes/strucs in functions by const reference and try to use const reference whenever it is possible for huge data. If some function doesn't change class field and shouldn't do it in future by design mark it as _const_.

##### enum class
_enum class_ syntax from C++11 is preferrable instead of old-style enum. However old-style enum is good to use as flag with **Q_FLAG**. The advantage of enum class that you should explicitly cast it to int using ldEnumHelper::asInteger and that you shouldn't worry about namespace - you always should use _ClassName::EnumName::EnumValue_ syntax and can't have issues with _ClassName::EnumValue_. For any enum try to avoid if-else long trees and use _switch(){}_ instead.

##### nullptr
In new code use C++11 _nullptr_ keyword. It's better than _NULL_ or _0_ because you can't compare 'nullptr' with int type, only with pointer. 

##### Field initialization
Use C++11 style of variable initialization if it is possible - in \*.h file. If something is initialized from constructor then initialize it in constructor declaration. And initialize field in constructor itself if you don't have other options. Also we should try to avoid classes with fields without default values, we use them quite often now but it looks weird because you can't rely on default constructor.

##### auto
Use _auto_ keyword only for iterators and lambdas.

##### Smart pointers
Try to avoid using keywords _delete_ / _free_ and any manul memory operations. Prefer to use _std::unique_ptr_, _std::shared_ptr_ or Qt parent-based object system.



### String

Need to be discussed. Currently we have mess with QString and std::string, char* sometimes.

##### char*
char* should be avoided in modern C++ code generally. There is no any reason to use it for your code. Use it only when you need to pass it somewhere to 3rdparty library and there is no choice.

##### QString vs std::string
QString is faster, has more functions and easier to use. So it is recommended string.


The only reason not to use it is if you don't want to restrict your class to Qt library and std::string is enough for you. In this cases std::string looks ok since you can always convert it to/from QString with one line.
Sometimes we have mess as the result with many convertions and it looks ugly so try to use one string in the scope.


### Qt

`QFileInfo::completeSuffix` and `QFileInfo::baseName` shouldn't be used in general because they don't work as you can expect it if filename has other "." symbols. They can be used only for internal data even in theory and should never be used for any file with a custom name. `QFileInfo::suffix` and `QFileInfo::completeBaseName` are ok.


