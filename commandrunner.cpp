/***************************************************************************
                          commandrunner.cpp  -  description
                             -------------------
    begin                : Sat Jul 14 2001
    copyright            : (C) 2001 by Simon Edwards
    email                : simon@simonzone.com
 ***************************************************************************/
/***************************************************************************
    Converted to a pure Qt5 application
    begin                : December 2014
    by                   : Antonio Martins
    email                : digiplan.pt@gmail.com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "commandrunner.h"

#include <QVBoxLayout>
#include <QPushButton>


/*!
 * \brief CommandRunner::CommandRunner
 * \param parent
 */
CommandRunner::CommandRunner(QWidget *parent) : QDialog(parent) {
    QVBoxLayout *verticalLayoutMain;
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout;
    QSpacerItem *horizontalSpacer;

    running = false;

    verticalLayoutMain = new QVBoxLayout(this);
    verticalLayoutMain->setSpacing(6);
    verticalLayoutMain->setContentsMargins(11, 11, 11, 11);
    verticalLayout = new QVBoxLayout();
    verticalLayout->setSpacing(6);

    horizontalLayout = new QHBoxLayout();
    horizontalLayout->setSpacing(6);

    labelIcon = new QLabel(this);
    QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    sizePolicy.setHeightForWidth(labelIcon->sizePolicy().hasHeightForWidth());
    labelIcon->setSizePolicy(sizePolicy);
    labelIcon->setTextFormat(Qt::AutoText);
    labelIcon->setPixmap(QPixmap(QString::fromUtf8(":/images/laserwarn.png")));
    labelIcon->setScaledContents(false);
    labelIcon->setMargin(0);
    horizontalLayout->addWidget(labelIcon);

    headinglabel = new QLabel(this);
    horizontalLayout->addWidget(headinglabel);
    horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    horizontalLayout->addItem(horizontalSpacer);

    verticalLayout->addLayout(horizontalLayout);
    outputview = new QTextEdit(this);
    outputview->setReadOnly(true);
    verticalLayout->addWidget(outputview);
    okbutton = new QPushButton(tr("Ok"), parent);
    verticalLayout->addWidget(okbutton);
    verticalLayoutMain->addLayout(verticalLayout);
    connect(okbutton, SIGNAL(clicked()), this, SLOT(slotOkClicked()));

    kid = new QProcess();
    connect(kid, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(slotKidExited(int, QProcess::ExitStatus)));
    connect(kid, SIGNAL(readyReadStandardOutput()), this, SLOT(slotReceivedStdout()));
    connect(kid, SIGNAL(readyReadStandardError()), this, SLOT(slotReceivedStderr()));
}


/*!
 * \brief CommandRunner::~CommandRunner
 */
CommandRunner::~CommandRunner() {
    delete kid;
}


/*!
 * \brief CommandRunner::setHeading
 * \param heading
 */
void CommandRunner::setHeading(const QString &heading) {
    headinglabel->setText(heading);
}


/*!
 * \brief CommandRunner::run
 * \param cmd
 */
void CommandRunner::run(QString cmd) {
    command = cmd;
    output.truncate(0);
    outputview->setText(output);
    running = true;
    okbutton->setEnabled(false);
    // Start a shell
    kid->start("/bin/sh", QStringList() << "-c" << command);
    exec();
}


/*!
 * \brief CommandRunner::pkexecRun
 * \param cmd
 * Runs command as root using pkexec
 */
void CommandRunner::pkexecRun(QString cmd) {
    command = cmd;
    output.truncate(0);
    outputview->setText(output);
    running = true;
    okbutton->setEnabled(false);
    // Start a shell in pkexec
    kid->start("pkexec", QStringList() << "--disable-internal-agent" << "/bin/sh" << "-c" << command);
    //kid->start("pkexec" "--disable-internal-agent" "/bin/sh", QStringList() << "-c" << command);
    exec();
}


/*!
 * \brief CommandRunner::slotKidExited
 * \param exitCode
 * \param exitStatus
 */
void CommandRunner::slotKidExited(int exitCode, QProcess::ExitStatus exitStatus) {
    if (exitStatus == QProcess::NormalExit) {
        qDebug("Command executed normally. Exit code is %d", exitCode);
    } else {
        qDebug("Command crashed");
    }
    running = false;
    okbutton->setEnabled(true);
}


/*!
 * \brief CommandRunner::slotReceivedStdout
 */
void CommandRunner::slotReceivedStdout() {
    output.append(kid->readAllStandardOutput());
    outputview->setText(output);
    outputview->ensureCursorVisible();
}


/*!
 * \brief CommandRunner::slotReceivedStderr
 */
void CommandRunner::slotReceivedStderr() {
    output.append(kid->readAllStandardError());
    outputview->setText(output);
    outputview->ensureCursorVisible();
}


/*!
 * \brief CommandRunner::slotOkClicked
 */
void CommandRunner::slotOkClicked() {
    done(0);
}
