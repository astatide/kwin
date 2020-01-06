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

#include <KConfig>
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

    //Init model and QML QuickView
    m_rulesModel = new RulesModel(this);

    QQuickView *quickView = new QQuickView();
    QQmlContext *qmlContext = quickView->rootContext();
    qmlContext->setContextProperty("rulesModel", m_rulesModel);

    quickView->setSource(QUrl::fromLocalFile(QStandardPaths::locate(
        QStandardPaths::GenericDataLocation,
        QStringLiteral(KWIN_NAME "/kwinruleseditor/main.qml"))));
    quickView->setResizeMode(QQuickView::SizeRootObjectToView);

    quickWidget = QWidget::createWindowContainer(quickView, this);
    quickWidget->setMinimumSize(QSize(500, 540));
    quickWidget->setVisible(isQuickUIShown);
    layout()->addWidget(quickWidget);

    widget = new RulesWidget(this);
    widget->setVisible(!isQuickUIShown);
    layout()->addWidget(widget);

    QDialogButtonBox *buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    connect(buttons, SIGNAL(accepted()), SLOT(accept()));
    connect(buttons, SIGNAL(rejected()), SLOT(reject()));

    // Toggle QML and classic UI for debugging
    QPushButton* toggleButton = buttons->addButton(QStringLiteral("Toggle classic/new UI"), QDialogButtonBox::ActionRole);
    toggleButton->setCheckable(true);
    toggleButton->setChecked(isQuickUIShown);
    connect(toggleButton, &QPushButton::toggled, this, &RulesDialog::toggleUI);
    layout()->addWidget(buttons);
}

void RulesDialog::toggleUI(bool showQuickUI)
{
    if (isQuickUIShown == showQuickUI) {
        return;
    }
    isQuickUIShown = showQuickUI;

    if (showQuickUI) {
        m_rulesModel->importFromRules(widget->rules());
        widget->hide();
        quickWidget->show();
    } else {
        widget->setRules(m_rulesModel->exportToRules());
        quickWidget->hide();
        widget->show();
    }
}

// window is set only for Alt+F3/Window-specific settings, because the dialog
// is then related to one specific window
Rules* RulesDialog::edit(Rules* r, const QVariantMap& info, bool show_hints)
{
    rules = r;
    widget->setRules(rules);

    m_rulesModel->importFromRules(rules);

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
    if (isQuickUIShown) {
        rules = m_rulesModel->exportToRules();
    } else {
        if (!widget->finalCheck())
            return;
        rules = widget->rules();
    }
    QDialog::accept();
}

}
