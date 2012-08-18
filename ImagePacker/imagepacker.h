#ifndef IMAGEPACKER_H
#define IMAGEPACKER_H

#include <QtGui/QMainWindow>
#include <QtGui/QFileDialog>
#include <QtGui/QMessageBox>
#include <QtGui/QDropEvent>
#include <QtCore/QUrl>
#include "ui_imagepacker.h"


typedef enum{
	Mode_WriteOnly,
	Mode_DisplayOnly,
	Mode_Both,
	Mode_Unsupported,
	Mode_Size = Mode_Unsupported
}ModeType;

typedef enum{
	OutputFormat_Bmp,
//	OutputFormat_Dib,
	OutputFormat_Jpg,
//	OutputFormat_Jp2,
	OutputFormat_Png,
//	OutputFormat_Pbm,
//	OutputFormat_Pgm,
//	OutputFormat_Ppm,
//	OutputFormat_Ras,
//	OutputFormat_Tiff,
	OutputFormat_Unsupported,
	OutputFormat_Size = OutputFormat_Unsupported
}OutputFormatType;

class ImagePacker : public QMainWindow
{
	Q_OBJECT

public:
	ImagePacker(QWidget *parent = 0, Qt::WFlags flags = 0);
	~ImagePacker();
protected:
	void dragEnterEvent(QDragEnterEvent*);
	void dropEvent(QDropEvent*);
private slots:
	void addToList();
	void removeFromList();
	void clearList();
	void clickedListItem();
	void changeMode(int);
	void changeFormat(int);
	void chooseOutputDirectory();
	void checkOutpuDirectory();
	void checkOutpuFile();
	void startPacking();
	void openAboutDialog();
private:
	void updateCountLabel();
	void updateStartPushButton();
	QString getExtension() const;
	bool checkInputFile(const QString& filepath) const;
	ModeType getMode() const;
	OutputFormatType getOutputFormat() const;
	bool ImagePacker::isEnabledStartPushButton() const;
private:
	Ui::ImagePackerClass ui;
	QStringList inputFormats;
	QStringList outputFormats;
	QString workingDirectory;
	QColor dirLineEditBase;
	QColor fileLineEditBase;
};

#endif // IMAGEPACKER_H
