#ifndef LaserdockVisualizer__ldLuaGameVisualizer__
#define LaserdockVisualizer__ldLuaGameVisualizer__

#include <sol/sol.hpp>

#include <ldCore/Visualizations/Visualizers/Games/ldAbstractGameVisualizer.h>

class ldLuaGameVisualizer : public ldAbstractGameVisualizer
{
    Q_OBJECT
public:
    static void initBaseResources();

    explicit ldLuaGameVisualizer(const QString &qrcPrefix, const QString &rootLuaFile);
    virtual ~ldLuaGameVisualizer();

    std::string readResourceFile(const std::string &fileName);

protected:
    void callLuaFunction(const std::string& functionName);
    template <class T> void callLuaFunction(const std::string& functionName, T arg);

private:
    void bindToLua();

    QString m_qrcPrefix;
    QString m_rootLuaFile;

    sol::state m_lua;
};


template <class T> void ldLuaGameVisualizer::callLuaFunction(const std::string& functionName, T arg)
{
    sol::protected_function f(m_lua[functionName], m_lua["Handler"]);
    sol::protected_function_result result = f(this, arg);
    if (result.valid()) {
        // Call succeeded
    }
    else {
        // Call failed
        sol::error err = result;
        std::string what = err.what();
        qWarning() << QString::fromStdString(functionName) << "call failed, sol::error::what() is " << QString::fromStdString(what);
    }
}
#endif /*__LaserdockVisualizer__ldLuaGameVisualizer__*/
