#ifndef WINDOW_HPP
#define WINDOW_HPP

#include <QMainWindow>
#include "model.hpp"
#include "stats.hpp"

class QString;
class QComboBox;
class QLabel;
class QPushButton;
class QTableView;
class StatsDialog;

class QuakeWindow : public QMainWindow
{
  Q_OBJECT

public:
  explicit QuakeWindow(QWidget *parent = nullptr);
  ~QuakeWindow() override = default;

private slots:
  void setDataLocation();
  void openCSV();
  void displayStats();
  void about();

private:
  void createMainWidget();
  void createFileSelectors();
  void createButtons();
  void createToolBar();
  void createStatusBar();
  void addFileMenu();
  void addHelpMenu();

  QuakeModel model;
  QString dataLocation; // Changed from pointer to value
  QComboBox *significance{nullptr};
  QComboBox *period{nullptr};
  QPushButton *loadButton{nullptr};
  QPushButton *statsButton{nullptr};
  QTableView *table{nullptr};
  QLabel *fileInfo{nullptr};
  StatsDialog *statsDialog{nullptr};
};

#endif