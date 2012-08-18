#ifndef PACKERDIALOG_H
#define PACKERDIALOG_H

#include <QtGui/QDialog>
#include <QtGui/QMessageBox>
#include <QtGui/QPushButton>
#include <QtCore/QProcess>
#include "ui_packerdialog.h"


class PackerDialog : public QDialog{
	Q_OBJECT
public:
	PackerDialog(QWidget* parent = 0, Qt::WindowFlags f = 0);
	void setArgs(const QStringList& args){ this->args = args; }
public slots:
	virtual void reject();
protected:
	virtual void showEvent(QShowEvent* e);
private slots:
	void proc_finished(int exitCode, QProcess::ExitStatus exitStatus);
	void updateStandardOutput();
	void updateStandardError();
private:
	Ui::PackerDialog ui;
	QProcess proc;
	QStringList args;
};

#endif