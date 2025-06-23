#ifndef QQMLHELPERS_H
#define QQMLHELPERS_H

#include <QObject>
#include <QtCore/QtDebug>

#define QML_WRITABLE_PROPERTY(type, name) \
    protected: \
        Q_PROPERTY (type name READ get_##name WRITE set_##name NOTIFY name##Changed) \
    private: \
        type m_##name; \
    public: \
        type get_##name () const { \
            return m_##name ; \
        } \
    public Q_SLOTS: \
        bool set_##name (type name) { \
            bool ret = false; \
            if ((ret = (m_##name != name))) { \
                m_##name = name; \
                emit name##Changed (m_##name); \
            } \
            return ret; \
        } \
    Q_SIGNALS: \
        void name##Changed (type name); \
    private:

#define QML_READONLY_PROPERTY(type, name) \
    protected: \
        Q_PROPERTY (type name READ get_##name NOTIFY name##Changed) \
    private: \
        type m_##name; \
    public: \
        type get_##name () const { \
            return m_##name ; \
        } \
        bool update_##name (type name) { \
            bool ret = false; \
            if ((ret = (m_##name != name))) { \
                m_##name = name; \
                emit name##Changed (m_##name); \
            } \
            return ret; \
        } \
    Q_SIGNALS: \
        void name##Changed (type name); \
    private:

#define QML_CONSTANT_PROPERTY(type, name) \
    protected: \
        Q_PROPERTY (type name READ get_##name CONSTANT) \
    private: \
        type m_##name; \
    public: \
        type get_##name () const { \
            return m_##name ; \
        } \
    private:

#define QML_LIST_PROPERTY(CLASS, NAME, TYPE) \
    public: \
        static int NAME##_count (QQmlListProperty<TYPE> * prop) { \
            CLASS * instance = qobject_cast<CLASS *> (prop->object); \
            return (instance != NULL ? instance->m_##NAME.count () : 0); \
        } \
        static void NAME##_clear (QQmlListProperty<TYPE> * prop) { \
            CLASS * instance = qobject_cast<CLASS *> (prop->object); \
            if (instance != NULL) { \
                instance->m_##NAME.clear (); \
            } \
        } \
        static void NAME##_append (QQmlListProperty<TYPE> * prop, TYPE * obj) { \
            CLASS * instance = qobject_cast<CLASS *> (prop->object); \
            if (instance != NULL && obj != NULL) { \
                instance->m_##NAME.append (obj); \
            } \
        } \
        static TYPE * NAME##_at (QQmlListProperty<TYPE> * prop, int idx) { \
            CLASS * instance = qobject_cast<CLASS *> (prop->object); \
            return (instance != NULL ? instance->m_##NAME.at (idx) : NULL); \
        } \
        QList<TYPE *> get_##NAME##s (void) const { \
            return m_##NAME; \
        } \
    private: \
        QList<TYPE *> m_##NAME;

#define QML_ENUM_CLASS(name, ...) \
    class name : public QObject { \
        Q_GADGET \
    public: \
        enum Type { __VA_ARGS__ }; \
        Q_ENUMS (Type) \
    };

// -------- ldCore modifications ----------

#define LD_MIN_MAX_WRITABLE_PROPERTY(type, name) \
    protected: \
        Q_PROPERTY (type name READ get_##name WRITE set_##name NOTIFY name##Changed) \
    private: \
        type m_##name; \
    public: \
        type get_##name () const { \
            return m_##name ; \
        } \
    public Q_SLOTS: \
        bool set_##name (type name) { \
            if(name < m_min_##name || name > m_max_##name) { \
                qWarning() << __FUNCTION__ << "min/max error" << name << m_min_##name << m_max_##name; \
                return false; \
            } \
            bool ret = false; \
            if ((ret = (m_##name != name))) { \
                m_##name = name; \
                emit name##Changed (m_##name); \
            } \
            return ret; \
        } \
    Q_SIGNALS: \
        void name##Changed (type name); \
    private:

#define LD_MIN_MAX_READONLY_PROPERTY(type, name) \
    protected: \
        Q_PROPERTY (type name READ get_##name NOTIFY name##Changed) \
    private: \
        type m_##name; \
    public: \
        type get_##name () const { \
            return m_##name ; \
        } \
        bool update_##name (type name) { \
            if(name < m_min_##name || name > m_max_##name) { \
                qWarning() << __FUNCTION__ << "min/max error" << name << m_min_##name << m_max_##name; \
                return false; \
            } \
            bool ret = false; \
            if ((ret = (m_##name != name))) { \
                m_##name = name; \
                emit name##Changed (m_##name); \
            } \
            return ret; \
        } \
    Q_SIGNALS: \
        void name##Changed (type name); \
    private:

#define LD_MIN_MAX_READONLY_QVARIANT_PROPERTY( name) \
protected: \
    Q_PROPERTY (QVariant name READ get_##name NOTIFY name##Changed) \
    private: \
    QVariant m_##name; \
    public: \
    QVariant get_##name () const { \
        return m_##name ; \
} \
    bool update_##name (QVariant name) { \
        if (name.type()==QVariant::Double) { \
            if(name.toDouble() < m_min_##name .toDouble() || name.toDouble() > m_max_##name .toDouble()) { \
                qWarning() << __FUNCTION__ << "min/max error" << name << m_min_##name << m_max_##name; \
                return false; \
            } \
        } \
        else if (name.type()==QVariant::Int) { \
                    if(name.toInt() < m_min_##name .toInt() || name.toInt() > m_max_##name .toInt()) { \
                    qWarning() << __FUNCTION__ << "min/max error" << name << m_min_##name << m_max_##name; \
                    return false; \
            } \
        } \
        bool ret = false; \
        if ((ret = (m_##name != name))) { \
            m_##name = name; \
            emit name##Changed (m_##name); \
    } \
        return ret; \
} \
    Q_SIGNALS: \
    void name##Changed (QVariant name); \
    private:

#define LD_MIN_MAX_WRITABLE_QVARIANT_PROPERTY( name) \
protected: \
    Q_PROPERTY (QVariant name READ get_##name WRITE set_##name NOTIFY name##Changed) \
    private: \
    QVariant m_##name; \
    public: \
    QVariant get_##name () const { \
        return m_##name ; \
} \
    public Q_SLOTS: \
    bool set_##name (QVariant name) { \
        if (name.type()==QVariant::Double) { \
                if(name.toDouble() < m_min_##name .toDouble() || name.toDouble() > m_max_##name .toDouble()) { \
                    qWarning() << __FUNCTION__ << "min/max error" << name << m_min_##name << m_max_##name; \
                    return false; \
            } \
        } \
            else if (name.type()==QVariant::Int) { \
                if(name.toInt() < m_min_##name .toInt() || name.toInt() > m_max_##name .toInt()) { \
                    qWarning() << __FUNCTION__ << "min/max error" << name << m_min_##name << m_max_##name; \
                    return false; \
            } \
        } \
        bool ret = false; \
        if ((ret = (m_##name != name))) { \
            m_##name = name; \
            emit name##Changed (m_##name); \
    } \
        return ret; \
} \
    Q_SIGNALS: \
    void name##Changed (QVariant name); \
    private:

#define LD_WRITABLE_MIN_MAX_PROPERTY(type, property) LD_MIN_MAX_WRITABLE_PROPERTY(type, property) \
    QML_READONLY_PROPERTY(type, min_##property) \
    QML_READONLY_PROPERTY(type, max_##property) \
    QML_READONLY_PROPERTY(type, default_##property)

#define LD_READONLY_MIN_MAX_PROPERTY(type, property) LD_MIN_MAX_READONLY_PROPERTY(type, property) \
    QML_READONLY_PROPERTY(type, min_##property) \
    QML_READONLY_PROPERTY(type, max_##property) \
    QML_READONLY_PROPERTY(type, default_##property)

#define LD_READONLY_MIN_MAX_QVARIANT_PROPERTY(property) LD_MIN_MAX_READONLY_QVARIANT_PROPERTY(property) \
    QML_READONLY_PROPERTY(QVariant, min_##property) \
    QML_READONLY_PROPERTY(QVariant, max_##property) \
    QML_READONLY_PROPERTY(QVariant, default_##property)

#define LD_WRITABLE_MIN_MAX_QVARIANT_PROPERTY(property) LD_MIN_MAX_WRITABLE_QVARIANT_PROPERTY(property) \
    QML_READONLY_PROPERTY(QVariant, min_##property) \
    QML_READONLY_PROPERTY(QVariant, max_##property) \
    QML_READONLY_PROPERTY(QVariant, default_##property)

#define LD_INIT_MIN_MAX_DEFAULT_PROPERTY(property, value, minValue, maxValue, defaultValue) m_##property(value) \
, m_min_##property(minValue) \
, m_max_##property(maxValue) \
, m_default_##property(defaultValue)

#define LD_INIT_MIN_MAX_PROPERTY(property, value, minValue, maxValue) \
    LD_INIT_MIN_MAX_DEFAULT_PROPERTY(property, value, minValue, maxValue, value)

#define LD_RESTORE_MIN_MAX_PROPERTY(key, property, type) property = ldSettings()->value(key, m_default_##property).to##type(); \
    if(property < m_min_##property || property > m_max_##property) property = m_default_##property


class QmlProperty : public QObject { Q_OBJECT }; // NOTE : to avoid "no suitable class found" MOC note

#endif // QQMLHELPERS_H
