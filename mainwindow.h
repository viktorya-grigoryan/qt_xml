#pragma once

#include <QtWidgets/QMainWindow>
#include <QMainWindow>
#include <QMenu>
#include <QMenuBar>
#include <QFileDialog>
#include <QMessageBox>
#include <QTreeView>
#include <QAction>
#include "xmlmodel.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void openFileMenu();
    void closeAllFiles();
    void exit();
    void ContextMenu(const QPoint& pos);
    void closeFile();
    void newTab();

private:
    XMLModel* model;
    QTreeView* view;
    QTabWidget* tabs;
    QTreeView* createTab(const QString& title);
};
