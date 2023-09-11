#include "mainwindow.h"
#include <qxmlstream.h>
#include <QTreeWidgetItem>


MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    auto myMenu = new QMenu{tr("Menu") };
    auto openFileAction = new QAction{tr("Open file"), this };
    myMenu->addAction(openFileAction);
    connect(openFileAction, SIGNAL(triggered()), this, SLOT(openFileMenu()));

    auto closeAllFilesAction = new QAction{tr("Close all files"), this };
    myMenu->addAction(closeAllFilesAction);
    connect(closeAllFilesAction, SIGNAL(triggered()), this, SLOT(closeAllFiles()));

    auto exitAction = new QAction{tr("Exit"), this };
    myMenu->addAction(exitAction);
    connect(exitAction, SIGNAL(triggered()), this, SLOT(exit()));

    menuBar()->addMenu(myMenu);

    tabs = new QTabWidget();
    view = new QTreeView(tabs);
    view->setContextMenuPolicy(Qt::CustomContextMenu);

    model = new XMLModel(view);
    view->setModel(model);

    connect(view, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(ContextMenu(QPoint)));

    tabs->addTab(view, "Tab 1");
    tabs->setCurrentIndex(tabs->indexOf(view));

    setCentralWidget(tabs);
}

MainWindow::~MainWindow() {
    delete model;
    delete view;
}

void MainWindow::openFileMenu() {
    QString fileName = QFileDialog::getOpenFileName(this, tr("Choose a file to open"), ":/xml", tr("XML ����� (*.xml)"));
    model->ParseFile(fileName);
    view->reset();
}

void MainWindow::closeAllFiles() {
    model->closeAll();
    view->reset();
}

void MainWindow::exit() {
    close();
}

void MainWindow::closeFile() {
    auto* curTV = qobject_cast<QTreeView*>(tabs->currentWidget());
    auto* curModel = qobject_cast<XMLModel*>(curTV->model());
    auto curIndex = curTV->currentIndex();
    curModel->removeRow(curIndex.row());
    view->reset();
}

void MainWindow::newTab() {
    auto* curTV = qobject_cast<QTreeView*>(tabs->currentWidget());
    auto* curModel = qobject_cast<XMLModel*>(curTV->model());
    auto curIndex = curTV->currentIndex();

    auto* newTV = createTab("Tab ");
    auto* newModel = qobject_cast<XMLModel*>(newTV->model());;
    newModel->append(curIndex);

    curModel->removeRow(curIndex.row());
}

QTreeView* MainWindow::createTab(const QString& title) {
    auto* tv = new QTreeView(tabs);
    auto* model = new XMLModel(tv);
    tv->setModel(model);
    static int i = 1;
    tabs->addTab(tv, title + QString::number(++i));
    tabs->setCurrentIndex(tabs->indexOf(tv));
    return tv;
}

void MainWindow::ContextMenu(const QPoint& pos) {
    if (view->currentIndex().parent() == view->rootIndex()) {
        QMenu* menu = new QMenu(this);
        auto actionActive = new QAction{ tr("Close"), this };
        menu->addAction(actionActive);
        connect(actionActive, &QAction::triggered,this, &MainWindow::closeFile);
        auto newtab = new QAction{ tr("Open in new tab"), this };
        connect(newtab, &QAction::triggered, this, &MainWindow::newTab);
        if (model->rowCount() < 2 ) {
            newtab->setEnabled(false);
        }
        menu->addAction(newtab);
        menu->popup(view->viewport()->mapToGlobal(pos));
    }

}





