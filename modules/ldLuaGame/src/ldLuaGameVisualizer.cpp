#include <ldLuaGame/ldLuaGameVisualizer.h>

#include <QtCore/QFile>
#include <QtCore/QString>
#include <QtCore/QTime>

#include <ldCore/ldCore.h>
#include <ldCore/Helpers/Text/ldTextLabel.h>

namespace  {
void clearLog()
{
#ifdef LD_ENABLE_LUA_LOG
    QString filename = "LuaLog.txt";
    QFile file(filename);
    file.open(QIODevice::WriteOnly);
#endif
}

void gameLog(const std::string& message)
{
    qDebug() << QString::fromStdString(message);
#ifdef LD_ENABLE_LUA_LOG
    QString filename = "LuaLog.txt";
    QFile file(filename);
    if (file.open(QIODevice::Append)) {
        QTextStream stream(&file);
        stream
            << "["
            << QTime::currentTime().toString()
            << "] "
            << message.c_str()
            << "\n";
    }
#endif
}
}

// ldLuaGameVisualizer
void ldLuaGameVisualizer::initBaseResources()
{
    Q_INIT_RESOURCE(lua_base);
}

ldLuaGameVisualizer::ldLuaGameVisualizer(const QString &qrcPrefix, const QString &rootLuaFile)
    : ldAbstractGameVisualizer()
    , m_qrcPrefix(qrcPrefix)
    , m_rootLuaFile(rootLuaFile)
{
    clearLog();
    bindToLua();
}

ldLuaGameVisualizer::~ldLuaGameVisualizer() {
}

std::string ldLuaGameVisualizer::readResourceFile(const std::string &fileName)
{
    QString filePath = ":/base/" + QString::fromStdString(fileName);
    if(!QFile::exists(filePath)) {
        filePath = ":/" + m_qrcPrefix + "/" + QString::fromStdString(fileName);
    }

    QFile resFile(filePath);

    bool isOpened = resFile.open(QIODevice::ReadOnly);
    if(!isOpened) {
        qWarning() << "Can't open" << resFile.fileName();
        return "";
    }
    return QString::fromUtf8(resFile.readAll()).toStdString();
}


void ldLuaGameVisualizer::callLuaFunction(const std::string& functionName)
{
    sol::protected_function f(m_lua[functionName], m_lua["Handler"]);
    sol::protected_function_result result = f(this);
    if (result.valid()) {
        // Call succeeded
    }
    else {
        // Call failed
        sol::error err = result;
        std::string what = err.what();
        qWarning() << this << QString::fromStdString(functionName) << "call failed, sol::error::what() is " << QString::fromStdString(what);
    }
}

void ldLuaGameVisualizer::bindToLua() {
    // All bindings hardcoded here now
    m_lua.open_libraries(sol::lib::base, sol::lib::package, sol::lib::math, sol::lib::table, sol::lib::string, sol::lib::io, sol::lib::bit32, sol::lib::os, sol::lib::debug);

    m_lua["log"] = gameLog;
    m_lua.set_function("include", [&](const std::string& path)
    {
        std::string fileText = readResourceFile(path);
        m_lua.do_string(fileText);
    }
    );

    m_lua.new_usertype<QString>(
        "QString",
        sol::constructors<QString()>(),
        "toStdString", &QString::toStdString,
        "fromStdString", &QString::fromStdString);

    m_lua.new_usertype<QByteArray>(
        "QByteArray",
        sol::constructors<QByteArray()>(),
        "toStdString", &QByteArray::toStdString,
        "fromStdString", &QByteArray::fromStdString);

    m_lua.new_usertype<ldVec2>(
        "ldVec2",
        sol::constructors<ldVec2(), ldVec2(float, float)>(),
        "x", &ldVec2::x,
        "y", &ldVec2::y);

    m_lua.new_usertype<QTime>(
        "QTime",
        "currentTime", &QTime::currentTime,
        "hour", &QTime::hour,
        "minute", &QTime::minute,
        "second", &QTime::second,
        "msec", &QTime::msec);

    auto ldColorUtilNamespace = m_lua["ldColorUtil"].get_or_create<sol::table>();
    ldColorUtilNamespace.set_function("lerpInt", &ldColorUtil::lerpInt);

    m_lua["OL_LINESTRIP"] = OL_LINESTRIP;
    m_lua["OL_BEZIERSTRIP"] = OL_BEZIERSTRIP;
    m_lua["OL_POINTS"] = OL_POINTS;

    m_lua.new_usertype<ldSoundEffects>(
        "ldSoundEffects",
        "insert", &ldSoundEffects::insert,
        "play", &ldSoundEffects::play,
        "stop", &ldSoundEffects::stop,
        "setLoops", &ldSoundEffects::setLoops,
        "setSoundEnabled", &ldSoundEffects::setSoundEnabled,
        "setSoundLevel", &ldSoundEffects::setSoundLevel);

    m_lua.new_usertype<ldCore>(
        "ldCore",
        "instance", &ldCore::instance,
        "resourceDir", &ldCore::resourceDir);

    m_lua.new_usertype<ldRendererOpenlase>(
        "ldRendererOpenlase",
        "begin", &ldRendererOpenlase::begin,
        "end_", &ldRendererOpenlase::end,
        "vertex", &ldRendererOpenlase::vertex,
        "vertex3", &ldRendererOpenlase::vertex3);

    m_lua.new_usertype<ldLuaGameVisualizer>(
        "ldBingBongVisualizer",
        "m_renderer", &ldLuaGameVisualizer::m_renderer,
        "m_soundEffects", &ldLuaGameVisualizer::m_soundEffects,
        "readResourceFile", &ldLuaGameVisualizer::readResourceFile);

    m_lua.new_usertype<ldTextLabel>(
        "ldTextLabel",
        sol::constructors<ldTextLabel(const QString&, float, const ldVec2&)>(),
        "setPosition", &ldTextLabel::setPosition,
        "getPosition", &ldTextLabel::getPosition,
        "setColor", &ldTextLabel::setColor,
        "getColor", &ldTextLabel::getColor,
        "setFontSize", &ldTextLabel::setFontSize,
        "getFontSize", &ldTextLabel::getFontSize,
        "setFont", &ldTextLabel::setFont,
        "font", &ldTextLabel::font,
        "letterSpace", &ldTextLabel::letterSpace,
        "setLetterSpace", &ldTextLabel::setLetterSpace,
        "getWidth", &ldTextLabel::getWidth,
        "getHeight", &ldTextLabel::getHeight,
        "setText", &ldTextLabel::setText,
        "getText", &ldTextLabel::getText,
        "clear", &ldTextLabel::clear,
        "innerDraw", &ldTextLabel::innerDraw);

    // load root lua file
    m_lua.script(readResourceFile(m_rootLuaFile.toStdString()));

    callLuaFunction("InitVis");
}

