#include "imagepacker.h"
#include "packerdialog.h"
#include "aboutdialog.h"


static QString getInputFilter(const QStringList& list){
	QString filter;
	foreach(const QString& temp, list){
		filter.append(" *." + temp);
	}
	return filter.trimmed();
}

ImagePacker::ImagePacker(QWidget *parent, Qt::WFlags flags) : QMainWindow(parent, flags){
	ui.setupUi(this);
	setAcceptDrops(true); // http://www.mail-archive.com/interest@qt-project.org/msg00538.html

	inputFormats << "bmp" << "jpg" << "png";
	outputFormats << "bmp" << "jpg" << "png";
	workingDirectory = QDir::currentPath();
	dirLineEditBase = ui.dirLineEdit->palette().color(QPalette::Base);
	fileLineEditBase = ui.fileLineEdit->palette().color(QPalette::Base);
	ui.dirLineEdit->setText(QDir::currentPath());
	ui.fileLineEdit->setText(tr("untitled"));
	updateCountLabel();
}

ImagePacker::~ImagePacker(){
}

void ImagePacker::dragEnterEvent(QDragEnterEvent* e){
	if(e->mimeData()->hasFormat("text/uri-list"))
		e->acceptProposedAction();
}

void ImagePacker::dropEvent(QDropEvent* e){
	QList<QUrl> urls = e->mimeData()->urls();
	QList<QUrl>::iterator it = urls.begin();
	for(; it != urls.end(); it++){
//		QString suffix = QFileInfo(it->toLocalFile()).suffix().toLower();
		// 拡張子でフィルタリング
		if(!checkInputFile(it->toLocalFile().toLower()))
			continue;	// skip
		// 重複していないか
		QList<QListWidgetItem*> items = ui.listWidget->findItems(it->toLocalFile(), Qt::MatchExactly);
		if(items.isEmpty())
			ui.listWidget->addItem(it->toLocalFile());
	}
	if(ui.listWidget->count() > 0){
		if(!ui.clearPushButton->isEnabled())
			ui.clearPushButton->setEnabled(true);
		updateCountLabel();
		updateStartPushButton();
	}
}

/**
 * スロット:リストにアイテムを追加する
 */
void ImagePacker::addToList(){
	QString filter = tr("Image") + " (" + getInputFilter(inputFormats) + ")";
	QByteArray ba = filter.toLocal8Bit();
	QStringList files = QFileDialog::getOpenFileNames(this, tr("Open File"), workingDirectory, tr(ba.constData()));
	if(!files.empty()){
		foreach(const QString& file, files){
			// 重複していないか
			QList<QListWidgetItem*> items = ui.listWidget->findItems(file, Qt::MatchExactly);
			if(items.isEmpty()){
				ui.listWidget->addItem(file);
			}
		}
		workingDirectory = QFileInfo(files.first()).absolutePath();
		ui.clearPushButton->setEnabled(true);
		updateCountLabel();
		updateStartPushButton();
	}
}

/**
 * スロット:リストからアイテムを除外する
 */
void ImagePacker::removeFromList(){
	QList<QListWidgetItem*> items = ui.listWidget->selectedItems();
	foreach(const QListWidgetItem* item, items){
		ui.listWidget->takeItem(ui.listWidget->row(item));
	}
	if(ui.listWidget->count() <= 0){
		ui.removePushButton->setEnabled(false);
		ui.clearPushButton->setEnabled(false);
	}
	updateCountLabel();
	updateStartPushButton();
}

/**
 * スロット:リストを空にする
 */
void ImagePacker::clearList(){
	ui.removePushButton->setEnabled(false);
	ui.clearPushButton->setEnabled(false);
	ui.listWidget->clear();
	updateCountLabel();
	updateStartPushButton();
}

/**
 * スロット:リストアイテムが選択された
 */
void ImagePacker::clickedListItem(){
	ui.removePushButton->setEnabled(true);
}

/**
 * スロット:モードが変更された
 */
void ImagePacker::changeMode(int){
	switch(getMode()){
	case Mode_WriteOnly:
		ui.fileSettingsGroupBox->setEnabled(true);
		break;
	case Mode_DisplayOnly:
		ui.fileSettingsGroupBox->setEnabled(false);
		break;
	case Mode_Both:
		ui.fileSettingsGroupBox->setEnabled(true);
		break;
	default:
		break;
	}
	updateStartPushButton();
}

/**
 * スロット:フォーマットが変更された
 */
void ImagePacker::changeFormat(int){
	switch(getOutputFormat()){
	case OutputFormat_Bmp:
		ui.jpegQualityGroupBox->setEnabled(false);
		ui.pngCompressionGroupBox->setEnabled(false);
		break;
	case OutputFormat_Jpg:
		ui.jpegQualityGroupBox->setEnabled(true);
		ui.pngCompressionGroupBox->setEnabled(false);
		break;
	case OutputFormat_Png:
		ui.jpegQualityGroupBox->setEnabled(false);
		ui.pngCompressionGroupBox->setEnabled(true);
		break;
	default:
		ui.jpegQualityGroupBox->setEnabled(false);
		ui.pngCompressionGroupBox->setEnabled(false);
		break;
	}
}

/**
 * スロット:出力ディレクトリを選択する
 */
void ImagePacker::chooseOutputDirectory(){
	QFileDialog::Options options = QFileDialog::DontResolveSymlinks | QFileDialog::ShowDirsOnly;
	QString dir = QFileDialog::getExistingDirectory(this, tr("choose output directory"), tr(""), options);	
	// 日本語文字化け対策
//	QByteArray byteArray = ui.lineEdit_3->text().toLocal8Bit();
//	byteArray.constData()
	if(!dir.isEmpty()){
		ui.dirLineEdit->setText(dir);
	}
}

/**
 * スロット:出力ディレクトリの妥当性をチェック
 */
void ImagePacker::checkOutpuDirectory(){
	QPalette palette = ui.dirLineEdit->palette();
	if(QFile::exists(ui.dirLineEdit->text())){
		palette.setColor(QPalette::Base, dirLineEditBase);
	}
	else{
		palette.setColor(QPalette::Base, Qt::red);
	}
	ui.dirLineEdit->setPalette(palette);
	updateStartPushButton();
}

/**
 * スロット:出力ファイルの妥当性をチェック
 */
void ImagePacker::checkOutpuFile(){
	Qt::GlobalColor color = ui.fileLineEdit->text().isEmpty()? Qt::red : Qt::color0;
	QPalette palette = ui.fileLineEdit->palette();
	palette.setColor(QPalette::Base, color);
	ui.fileLineEdit->setPalette(palette);
	updateStartPushButton();
}

/**
 * スロット:パッキングの開始
 */
void ImagePacker::startPacking(){
	QStringList args;
	QString temp, arg;
	// input
	arg = "-i=";
	int row = ui.listWidget->model()->rowCount();
	for(int i = 0; i < row; i++){
		QModelIndex index = ui.listWidget->model()->index(i, 0);
		QString str = ui.listWidget->model()->data(index).toString();
		arg.append(str + ";");
	}
	args << arg;
	// output
	const ModeType mode = getMode();
	if((mode == Mode_WriteOnly) || (mode == Mode_Both)){
		arg = "-o=" + ui.dirLineEdit->text() + "/" + ui.fileLineEdit->text() + "." + getExtension();
		args << arg;
		// parameter
		OutputFormatType type = getOutputFormat();
		if(type == OutputFormat_Jpg){
			temp.setNum(ui.jpegQualitySpinBox->value());
			arg = "-p=" + temp;
			args << arg;
		}
		else
		if(type == OutputFormat_Png){
			temp.setNum(ui.pngCompressionSpinBox->value());
			arg = "-p=" + temp;
			args << arg;
		}
	}
	if((mode == Mode_DisplayOnly) || (mode == Mode_Both)){
		arg = "-d=true";
		args << arg;
	}
	// padding
	temp.setNum(ui.paddingSpinBox->value());
	arg = "-pad=" + temp;
	args << arg;
	// align
	temp = ui.alignCheckBox->isChecked()? "true" : "false";
	arg = "-aln=" + temp;
	args << arg;

	// ダイアログ起動
	PackerDialog dialog(this);
	dialog.setArgs(args);
	dialog.exec();
#if 0
	int i = dialog.exec();
	switch(i){
	case QDialog::Accepted:
		break;
	case QDialog::Rejected:
		break;
	default:
		break;		
	}
#endif
}

/**
 * about dialog を開く
 */
void ImagePacker::openAboutDialog(){
	// ダイアログ起動
	AboutDialog dialog(this);
	dialog.exec();
}

/**
 * カウントラベルの更新
 */
void ImagePacker::updateCountLabel(){
	const int count = ui.listWidget->count();
	ui.countLabel->setText(QString("Total: %1 items").arg(count));
	if(count > 0)
		ui.messageLabel->hide();
	else
		ui.messageLabel->show();
}

bool ImagePacker::isEnabledStartPushButton() const {
	if(ui.listWidget->count() <= 0){
		return false; // 入力ファイルがない
	}
	const ModeType mode = getMode();
	if((mode == Mode_WriteOnly) || (mode == Mode_Both)){
		if(ui.dirLineEdit->text().isEmpty() || !QFile::exists(ui.dirLineEdit->text())){
			return false;	// 出力ディレクトリ指定がない
		}
		if(ui.fileLineEdit->text().isEmpty()){
			return false;	// 出力ファイル指定がない
		}
		return true;
	}
	else
	if(mode == Mode_DisplayOnly){
		return true;
	}
	return false;
}

/**
 * スタートプッシュボタンの更新
 */
void ImagePacker::updateStartPushButton(){
	ui.startPushButton->setEnabled(isEnabledStartPushButton());
}

QString ImagePacker::getExtension() const {
	switch(getOutputFormat()){
	case OutputFormat_Bmp: return QString("bmp");
	case OutputFormat_Jpg: return QString("jpg");
	case OutputFormat_Png: return QString("png");
	default:
		break;
	}
	return QString("");
}

bool ImagePacker::checkInputFile(const QString& filepath) const {
	int index = filepath.lastIndexOf(".");
	if(index < 0)
		return false;
	QString sub = filepath.mid(index+1, filepath.length() - (index+1));
	if(sub.isEmpty())
		return false;

	foreach(const QString& temp, inputFormats){
		if(sub == temp)
			return true;
	}
	return false;
}

ModeType ImagePacker::getMode() const {
	const QRadioButton* rb = dynamic_cast<QRadioButton*>(ui.modeButtonGroup->checkedButton());
	const QString mode = rb->text();

	if(mode == ui.writeOnlyRadioButton->text())
		return Mode_WriteOnly;
	if(mode == ui.displayOnlyRadioButton->text())
		return Mode_DisplayOnly;
	if(mode == ui.bothRadioButton->text())
		return Mode_Both;
	return Mode_Unsupported;
}

OutputFormatType ImagePacker::getOutputFormat() const {
	const QRadioButton* rb = dynamic_cast<QRadioButton*>(ui.formatButtonGroup->checkedButton());
	const QString format = rb->text();

	if(format == ui.bmpRadioButton->text())
		return OutputFormat_Bmp;
	if(format == ui.jpgRadioButton->text())
		return OutputFormat_Jpg;
	if(format == ui.pngRadioButton->text())
		return OutputFormat_Png;
	return OutputFormat_Unsupported;
}

