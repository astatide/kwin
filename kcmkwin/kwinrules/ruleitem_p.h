/*
 * Copyright (c) 2020 Ismael Asensio <isma.af@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License or (at your option) version 3 or any later version
 * accepted by the membership of KDE e.V. (or its successor approved
 * by the membership of KDE e.V.), which shall act as a proxy
 * defined in Section 14 of version 3 of the license.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef KWIN_RULEITEM_P_H
#define KWIN_RULEITEM_P_H

#include <QAbstractListModel>
#include <QIcon>
#include <QVariant>


namespace KWin {

enum RuleType {
    Undefined,
    Boolean,
    String,
    Integer,
    Option,
    FlagsOption,
    Percentage,
    Coordinate,
    Shortcut
};

class RuleItemPrivate
{
public:
    RuleItemPrivate();
    RuleItemPrivate(const QString &key,
                    const QString &name,
                    const QString &section,
                    const QString &iconName)
        : m_key(key)
        , m_name(name)
        , m_section(section)
        , m_icon(QIcon::fromTheme(iconName))
        , m_enabled(false)
        , m_flags(0)
        {};

public:
    QString m_key;
    QString m_name;
    QString m_section;
    QIcon m_icon;

    bool m_enabled;

    QString m_description;
    uint m_flags;

    RuleType m_type;
    QVariant m_value;
};


class OptionsModel : public QAbstractListModel
{
    Q_OBJECT
    //FIXME: After Qt 5.14 the QML ComboBox will allow to use the valueRole directly
    //       instead of changing the index. Remove this exposed property
    Q_PROPERTY(int selectedIndex READ selectedIndex WRITE setSelectedIndex NOTIFY valueChanged)

public:
    struct Data {
        Data(const QVariant &value, const QString &text) : value(value), text(text) {}
        Data(const QVariant &value, const QString &text, const QString &iconName)
            : value(value)
            , text(text)
            , icon(QIcon::fromTheme(iconName))
            {}
        Data(const QVariant &value, const QString &text, const QIcon &icon)
            : value(value)
            , text(text)
            , icon(icon)
            {}
        QVariant value;
        QString text;
        QIcon icon;
        QString description {};
    };

public:
    OptionsModel() : QAbstractListModel(), m_data(), m_index(0) {};
    OptionsModel(const QList<Data> &data) : QAbstractListModel(), m_data(data), m_index(0) {};

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QHash<int, QByteArray> roleNames() const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    QVariant value() const;
    void setValue(QVariant value);

    //FIXME: Make private or remove after Qt 5.14
    int selectedIndex() const;
    void setSelectedIndex(int index);

    void updateModelData(const QList<Data> &data);

signals:
    void valueChanged(QVariant value);

public:
    QList<Data> m_data;

protected:
    int m_index = 0;
};

class RulePolicy : public OptionsModel
{
public:
    enum Type {
        NoPolicy,
        StringMatch,
        SetRule,
        ForceRule
    };

public:
    RulePolicy(Type type)
        : OptionsModel(policyOptions(type))
        , m_type(type)
        {};

    Type type() const;
    int value() const;
    QString policyKey(const QString &key) const;

private:
    static QList<Data> policyOptions(RulePolicy::Type type);

private:
    Type m_type;
};

}   //namespace

#endif  //KWIN_RULEITEM_P_H
