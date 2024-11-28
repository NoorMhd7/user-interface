#include "window.hpp"
#include "stats.hpp"
#include <QtWidgets>

static const int MIN_WIDTH = 875;

QuakeWindow::QuakeWindow(QWidget *parent)
    : QMainWindow(parent), statsDialog(nullptr)
{
  createMainWidget();
  createFileSelectors();
  createButtons();
  createToolBar();
  createStatusBar();
  addFileMenu();
  addHelpMenu();

  setMinimumWidth(MIN_WIDTH);
  setWindowTitle("Water Quality Monitor");
}

void QuakeWindow::createMainWidget()
{
  QWidget *centralWidget = new QWidget(this);
  QVBoxLayout *layout = new QVBoxLayout(centralWidget);

  // Add welcome label
  QLabel *welcomeLabel = new QLabel(
      "Click 'Load CSV File' to begin analyzing water quality data.",
      centralWidget);
  welcomeLabel->setAlignment(Qt::AlignCenter);
  layout->addWidget(welcomeLabel);

  // Create table
  table = new QTableView(centralWidget);
  table->setModel(&model);
  table->setAlternatingRowColors(true);
  QFont tableFont = QFontDatabase::systemFont(QFontDatabase::FixedFont);
  table->setFont(tableFont);
  layout->addWidget(table);

  setCentralWidget(centralWidget);
}

void QuakeWindow::createFileSelectors()
{
  QStringList periodOptions;
  periodOptions << "month" << "week" << "day";
  period = new QComboBox();
  period->addItems(periodOptions);

  QStringList significanceOptions;
  significanceOptions << "all" << "major" << "minor";
  significance = new QComboBox();
  significance->addItems(significanceOptions);
}

void QuakeWindow::createButtons()
{
  loadButton = new QPushButton("Load Data");
  statsButton = new QPushButton("Show Stats");

  connect(loadButton, &QPushButton::clicked, this, &QuakeWindow::openCSV);
  connect(statsButton, &QPushButton::clicked, this, &QuakeWindow::displayStats);
}

void QuakeWindow::createToolBar()
{
  QToolBar *toolBar = addToolBar("Main");

  // Add "Load CSV" button first
  QAction *loadCsvAction = new QAction("Load CSV File", this);
  loadCsvAction->setIcon(style()->standardIcon(QStyle::SP_FileIcon)); // Add an icon
  connect(loadCsvAction, &QAction::triggered, this, [this]()
          {
        QString filename = QFileDialog::getOpenFileName(this,
            "Open Water Quality Data", 
            QString(),
            "CSV Files (*.csv);;All Files (*)");
            
        if (!filename.isEmpty()) {
            try {
                model.updateFromFile(filename);
                fileInfo->setText("Current file: " + QFileInfo(filename).fileName());
                table->resizeColumnsToContents();
            } catch (const std::exception& e) {
                QMessageBox::critical(this, "Error", 
                    QString("Failed to load file: %1").arg(e.what()));
            }
        } });
  toolBar->addAction(loadCsvAction);
  toolBar->addSeparator();

  // Add filter options
  toolBar->addWidget(new QLabel("Period: "));
  toolBar->addWidget(period);
  toolBar->addSeparator();
  toolBar->addWidget(new QLabel("Filter: "));
  toolBar->addWidget(significance);
  toolBar->addSeparator();
  toolBar->addWidget(statsButton);
}

void QuakeWindow::createStatusBar()
{
  fileInfo = new QLabel("No file loaded");
  statusBar()->addWidget(fileInfo);
}

void QuakeWindow::addFileMenu()
{
  QMenu *fileMenu = menuBar()->addMenu("&File");

  // Add Open CSV action
  QAction *openAction = new QAction("&Open CSV...", this);
  openAction->setShortcut(QKeySequence::Open); // Ctrl+O / Cmd+O
  connect(openAction, &QAction::triggered, this, [this]()
          {
        QString filename = QFileDialog::getOpenFileName(this,
            "Open Water Quality Data", 
            QString(),
            "CSV Files (*.csv);;All Files (*)");
            
        if (!filename.isEmpty()) {
            try {
                model.updateFromFile(filename);
                fileInfo->setText("Current file: " + QFileInfo(filename).fileName());
                table->resizeColumnsToContents();
            } catch (const std::exception& e) {
                QMessageBox::critical(this, "Error", 
                    QString("Failed to load file: %1").arg(e.what()));
            }
        } });

  fileMenu->addAction(openAction);
  fileMenu->addSeparator();

  QAction *exitAction = new QAction("E&xit", this);
  exitAction->setShortcut(QKeySequence::Quit);
  connect(exitAction, &QAction::triggered, this, &QWidget::close);

  fileMenu->addAction(exitAction);
}

void QuakeWindow::addHelpMenu()
{
  QMenu *helpMenu = menuBar()->addMenu("&Help");

  QAction *aboutAction = new QAction("&About", this);
  connect(aboutAction, &QAction::triggered, this, &QuakeWindow::about);

  QAction *aboutQtAction = new QAction("About &Qt", this);
  connect(aboutQtAction, &QAction::triggered, qApp, &QApplication::aboutQt);

  helpMenu->addAction(aboutAction);
  helpMenu->addAction(aboutQtAction);
}

// Add these implementations to window.cpp

void QuakeWindow::openCSV()
{
  if (dataLocation.isEmpty())
  {
    QMessageBox::critical(this, "Data Location Error",
                          "Data location has not been set!\n\n"
                          "You can specify this via the File menu.");
    return;
  }

  auto filename = QString("%1_%2.csv")
                      .arg(significance->currentText())
                      .arg(period->currentText());

  auto path = dataLocation + "/" + filename;

  try
  {
    model.updateFromFile(path);
  }
  catch (const std::exception &error)
  {
    QMessageBox::critical(this, "CSV File Error", error.what());
    return;
  }

  fileInfo->setText(QString("Current file: <kbd>%1</kbd>").arg(filename));
  table->resizeColumnsToContents();

  if (statsDialog && statsDialog->isVisible())
  {
    double avgLevel = model.meanMagnitude();
    int total = model.rowCount();
    int belowLimit = model.getBelowDetectionCount();
    statsDialog->update(avgLevel, total, belowLimit);
  }
}

void QuakeWindow::setDataLocation()
{
  QString directory = QFileDialog::getExistingDirectory(
      this, "Data Location", ".",
      QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

  if (!directory.isEmpty())
  {
    dataLocation = directory;
  }
}

void QuakeWindow::displayStats()
{
  if (model.hasData())
  {
    if (!statsDialog)
    {
      statsDialog = new StatsDialog(this);
    }

    double avgLevel = model.meanMagnitude();
    int total = model.rowCount();
    int belowLimit = model.getBelowDetectionCount();

    statsDialog->update(avgLevel, total, belowLimit);
    statsDialog->show();
    statsDialog->raise();
    statsDialog->activateWindow();
  }
  else
  {
    QMessageBox::warning(this, "No Data",
                         "Please load data before viewing statistics.");
  }
}

void QuakeWindow::about()
{
  QMessageBox::about(this, "About Water Quality Monitor",
                     "Water Quality Monitor displays and analyzes water quality data "
                     "from UK/EU monitoring stations.\n\n"
                     "Data source: Environment Agency\n\n"
                     "Features:\n"
                     "- Load and view water quality measurements\n"
                     "- View statistics and summaries\n"
                     "- Monitor pollutant levels\n"
                     "- Track measurements below detection limits");
}
