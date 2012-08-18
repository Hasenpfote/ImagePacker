#ifndef ABOUTDIALOG_H
#define ABOUTDIALOG_H

#include <QtGui/QDialog>
#include <QtGui/QMessageBox>
#include <QtGui/QPushButton>
#include <QtCore/QProcess>
#include "ui_aboutdialog.h"


class AboutDialog : public QDialog{
	Q_OBJECT
public:
	AboutDialog(QWidget* parent = 0, Qt::WindowFlags f = 0);
private:
	Ui::AboutDialog ui;
};

#endif