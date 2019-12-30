/*
 * Copyright (c) 2004 Lubos Lunak <l.lunak@kde.org>
 * Copyright (c) 2010 Ismael Asensio <isma.af@gmail.com>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#include "rulesdialog.h"
#include "../../rules.h"

#include <QQuickView>
#include <QQmlContext>
#include <QPushButton>
#include <QTimer>

#include <KMessageBox>


namespace KWin
{

RulesDialog::RulesDialog(QWidget* parent, const char* name)
    : QDialog(parent)
{
    setObjectName(name);
    setModal(true);
    setWindowTitle(i18n("Edit Window-Specific Settings"));
    setWindowIcon(QIcon::fromTheme("preferences-system-windows-actions"));
    setLayout(new QVBoxLayout);

    //Init rules model
    m_rulesModel = new RulesModel(this);
    m_rulesModel->init();

    QQuickView *quickView = new QQuickView();
    QQmlContext *qmlContext = quickView->rootContext();
    qmlContext->setContextProperty("rulesModel", m_rulesModel);

    quickView->setSource(QUrl::fromLocalFile(QStandardPaths::locate(
        QStandardPaths::GenericDataLocation,
        QStringLiteral(KWIN_NAME "/kwinruleseditor/main.qml"))));
    quickView->setResizeMode(QQuickView::SizeRootObjectToView);

    quickWidget = QWidget::createWindowContainer(quickView, this);
    quickWidget->setMinimumSize(QSize(700, 740));
    layout()->addWidget(quickWidget);

    widget = new RulesWidget(this);
    widget->hide();
    layout()->addWidget(widget);

    QDialogButtonBox *buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    connect(buttons, SIGNAL(accepted()), SLOT(accept()));
    connect(buttons, SIGNAL(rejected()), SLOT(reject()));

    // Toggle QML and classic UI for debugging
    QPushButton* toggleButton = buttons->addButton(QStringLiteral("Show classic UI"), QDialogButtonBox::ActionRole);
    toggleButton->setCheckable(true);
    connect(toggleButton, &QPushButton::toggled, this, [this](bool checked) {
        if (checked) {
            quickWidget->hide();
            widget->show();
        } else {
            widget->hide();
            quickWidget->show();
        }
    });

    layout()->addWidget(buttons);
}

// window is set only for Alt+F3/Window-specific settings, because the dialog
// is then related to one specific window
Rules* RulesDialog::edit(Rules* r, const QVariantMap& info, bool show_hints)
{
    rules = r;
    widget->setRules(rules);
    if (!info.isEmpty())
    {
        widget->prepareWindowSpecific(info);
    }
    if (show_hints)
        QTimer::singleShot(0, this, SLOT(displayHints()));
    exec();
    return rules;
}

void RulesDialog::displayHints()
{
    QString str = "<qt><p>";
    str += i18n("This configuration dialog allows altering settings only for the selected window"
                " or application. Find the setting you want to affect, enable the setting using the checkbox,"
                " select in what way the setting should be affected and to which value.");
#if 0 // maybe later
    str += "</p><p>" + i18n("Consult the documentation for more details.");
#endif
    str += "</p></qt>";
    KMessageBox::information(this, str, QString(), "displayhints");
}

void RulesDialog::accept()
{
    if (!widget->finalCheck())
        return;
    rules = widget->rules();
    QDialog::accept();
}

}
