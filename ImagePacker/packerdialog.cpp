#include "packerdialog.h"

PackerDialog::PackerDialog(QWidget* parent, Qt::WindowFlags f) : QDialog(parent, f){
	ui.setupUi(this);
	QObject::connect(&proc, SIGNAL(readyReadStandardOutput()), this, SLOT(updateStandardOutput()));
	QObject::connect(&proc, SIGNAL(readyReadStandardError()), this, SLOT(updateStandardError()));
	QObject::connect(&proc, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(proc_finished(int, QProcess::ExitStatus)));
	QPushButton* pushButton = ui.buttonBox->button(QDialogButtonBox::Ok);
	pushButton->setEnabled(false);
}

/**
 * 基底関数のオーバーライド
 */
void PackerDialog::reject(){
	QDialog::reject();
	proc.kill();
}

/**
 * 基底関数のオーバーライド
 * exec()のタイミングで呼ばれる
 */
void PackerDialog::showEvent(QShowEvent* e){
	QDialog::showEvent(e);
	// ToDo: 試験的に絶対パスで指定しているが最終的には直下に
	proc.start("E:\\GitHubRepository\\2dpacker\\debug\\2dpacker.exe", args);
}

/**
 * スロット:プロセス終了時に呼ばれる
 */
void PackerDialog::proc_finished(int exitCode, QProcess::ExitStatus exitStatus){
	QString message, result;
	result = (exitStatus == QProcess::NormalExit)? "Succeeded" : "failed";
	message = QString("result:%1 (code:%2 status:%3)").arg(result).arg(exitCode).arg(exitStatus);
	ui.listWidget->addItem(message);
	ui.listWidget->scrollToBottom();
	QPushButton* pushButton = ui.buttonBox->button(QDialogButtonBox::Ok);
	pushButton->setEnabled(true);
}

/**
 * スロット:標準出力更新
 */
void PackerDialog::updateStandardOutput(){
	QByteArray ba = proc.readAllStandardOutput();
	QString str = QString::fromLocal8Bit(ba);
	str = str.trimmed();
	QStringList list = str.split("\n");
	QString line;
	foreach(line, list){
		ui.listWidget->addItem(line);
	}
	ui.listWidget->scrollToBottom();
}

/**
 * スロット:標準エラー出力更新
 */
void PackerDialog::updateStandardError(){
	QByteArray ba = proc.readAllStandardError();
	QString str = QString::fromLocal8Bit(ba);
	str = str.trimmed();
	QStringList list = str.split("\n");
	QString line;
	foreach(line, list){
		ui.listWidget->addItem(line);
	}
	ui.listWidget->scrollToBottom();
}
