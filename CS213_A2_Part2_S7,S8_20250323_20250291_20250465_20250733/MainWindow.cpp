/*#include "MainWindow.h"

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QSplitter>
#include <QHeaderView>
#include <QFileDialog>
#include <QInputDialog>
#include <QMessageBox>
#include <QLineEdit>
#include <QFont>
#include <QColor>
#include <QBrush>
#include <QTextCursor>
#include <QStatusBar>

// ---- Backend includes ----
#include "Simulator.h"
#include "Memory.h"
#include "CPU.h"
#include "RegisterFile.h"
#include "ScreenDevice.h"

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    simulator = new Simulator();

    setupUI();
    setupConnections();
    applyStyles();
    refreshAll();
}

MainWindow::~MainWindow()
{
    delete simulator;
}

void MainWindow::setupUI()
{
    setWindowTitle("Vole Machine Simulator - Starry Night Edition");
    resize(1280, 820);

    QWidget* central = new QWidget(this);
    setCentralWidget(central);

    QVBoxLayout* mainLayout = new QVBoxLayout(central);
    mainLayout->setContentsMargins(10, 10, 10, 10);
    mainLayout->setSpacing(8);

    // ---- Top toolbar ----
    QWidget* toolbar = new QWidget(central);
    setupToolbar(toolbar);
    mainLayout->addWidget(toolbar);

    // ---- Vertical splitter ----
    QSplitter* vSplitter = new QSplitter(Qt::Vertical, central);
    QSplitter* hSplitter = new QSplitter(Qt::Horizontal, vSplitter);

    // Memory grid
    QGroupBox* memGroup = new QGroupBox("Main Memory (256 bytes)", hSplitter);
    QVBoxLayout* memLayout = new QVBoxLayout(memGroup);
    memLayout->setContentsMargins(8, 8, 8, 8);
    setupMemoryGrid(memGroup);
    memLayout->addWidget(memoryTable);
    hSplitter->addWidget(memGroup);

    // Right panel: registers + status
    QWidget* rightPanel = new QWidget(hSplitter);
    QVBoxLayout* rightLayout = new QVBoxLayout(rightPanel);
    rightLayout->setContentsMargins(0, 0, 0, 0);
    rightLayout->setSpacing(8);

    QGroupBox* regGroup = new QGroupBox("General Registers", rightPanel);
    QVBoxLayout* regLayout = new QVBoxLayout(regGroup);
    regLayout->setContentsMargins(8, 8, 8, 8);
    setupRegistersPanel(regGroup);
    regLayout->addWidget(registerTable);
    rightLayout->addWidget(regGroup);

    QGroupBox* statusGroup = new QGroupBox("CPU Status", rightPanel);
    QVBoxLayout* statusLayout = new QVBoxLayout(statusGroup);
    statusLayout->setContentsMargins(8, 8, 8, 8);
    setupStatusPanel(statusGroup);
    rightLayout->addWidget(statusGroup);
    rightLayout->addStretch();

    hSplitter->addWidget(rightPanel);
    hSplitter->setStretchFactor(0, 3);
    hSplitter->setStretchFactor(1, 2);

    vSplitter->addWidget(hSplitter);

    // Screen output
    QGroupBox* screenGroup = new QGroupBox("Screen Output (memory-mapped at 0xFF)", vSplitter);
    QVBoxLayout* screenLayout = new QVBoxLayout(screenGroup);
    screenLayout->setContentsMargins(8, 8, 8, 8);
    setupScreenPanel(screenGroup);
    screenLayout->addWidget(screenOutput);

    vSplitter->addWidget(screenGroup);
    vSplitter->setStretchFactor(0, 3);
    vSplitter->setStretchFactor(1, 1);

    mainLayout->addWidget(vSplitter, 1);
    statusBar()->showMessage("Ready");
}

void MainWindow::setupToolbar(QWidget* parent)
{
    QHBoxLayout* layout = new QHBoxLayout(parent);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(8);

    btnLoad = new QPushButton("Load File", parent);
    btnRun = new QPushButton("Run", parent);
    btnStep = new QPushButton("Single Step", parent);
    btnReset = new QPushButton("Reset", parent);
    cmbMode = new QComboBox(parent);

    btnLoad->setObjectName("btnLoad");
    btnRun->setObjectName("btnRun");
    btnStep->setObjectName("btnStep");
    btnReset->setObjectName("btnReset");

    cmbMode->addItem("Continuous Execution");
    cmbMode->addItem("Step-by-Step Execution");

    layout->addWidget(btnLoad);
    layout->addWidget(btnRun);
    layout->addWidget(btnStep);
    layout->addWidget(btnReset);
    layout->addWidget(cmbMode);
    layout->addStretch();
}

void MainWindow::setupMemoryGrid(QWidget* parent)
{
    memoryTable = new QTableWidget(16, 16, parent);
    QStringList headers;
    for (int i = 0; i < 16; ++i) {
        headers << QString::number(i, 16).toUpper();
    }
    memoryTable->setHorizontalHeaderLabels(headers);
    memoryTable->setVerticalHeaderLabels(headers);
    memoryTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    memoryTable->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}

void MainWindow::setupRegistersPanel(QWidget* parent)
{
    registerTable = new QTableWidget(16, 2, parent);
    registerTable->setHorizontalHeaderLabels(QStringList() << "Register" << "Value");
    registerTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    for (int i = 0; i < 16; ++i) {
        QString regName = "R" + QString::number(i, 16).toUpper();
        registerTable->setItem(i, 0, new QTableWidgetItem(regName));
        registerTable->setItem(i, 1, new QTableWidgetItem("00"));
    }
}

void MainWindow::setupStatusPanel(QWidget* parent)
{
    QGridLayout* layout = new QGridLayout(parent);
    lblPC = new QLabel("PC: 00", parent);
    lblIR = new QLabel("IR: 0000", parent);
    lblState = new QLabel("State: READY", parent);

    lblPC->setObjectName("lblPC");
    lblIR->setObjectName("lblIR");
    lblState->setObjectName("lblState");

    layout->addWidget(lblPC, 0, 0);
    layout->addWidget(lblIR, 0, 1);
    layout->addWidget(lblState, 1, 0, 1, 2);
}

void MainWindow::setupScreenPanel(QWidget* parent)
{
    screenOutput = new QTextEdit(parent);
    screenOutput->setReadOnly(true);
}

void MainWindow::setupConnections()
{
    connect(btnLoad, &QPushButton::clicked, this, &MainWindow::onLoadFile);
    connect(btnRun, &QPushButton::clicked, this, &MainWindow::onRun);
    connect(btnStep, &QPushButton::clicked, this, &MainWindow::onStep);
    connect(btnReset, &QPushButton::clicked, this, &MainWindow::onReset);
    connect(cmbMode, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::onModeChanged);
}

void MainWindow::applyStyles()
{
    QString styleSheet = R"(
        /* النافذة الرئيسية والقوائم */
QMainWindow, QWidget{
    background - color: #0B192C;
    color: #E0E6ED;
    font - family: 'Segoe UI', Tahoma, Geneva, sans - serif;
    font - size: 13px;
}

/* المجموعات 
QGroupBox{
    background - color: #112236;
    border: 2px solid #0E5C7C;
    border - radius: 8px;
    margin - top: 12px;
    font - weight: bold;
    color: #19B6BE;
}
QGroupBox::title{
    subcontrol - origin: margin;
    subcontrol - position: top left;
    padding: 2px 8px;
    background - color: #0E5C7C;
    color: #E8D15D;
    border - radius: 4px;
}

/* الأزرار بالوان Starry Night Palette 
QPushButton{
    border: none;
    border - radius: 5px;
    padding: 8px 16px;
    font - weight: bold;
    color: #FFFFFF;
}

QPushButton#btnLoad{
    background - color: #19B6BE;
    color: #0B192C;
}
QPushButton#btnLoad:hover{
    background - color: #8BC281;
}

QPushButton#btnRun{
    background - color: #2D6658;
}
QPushButton#btnRun:hover{
    background - color: #8BC281;
    color: #0B192C;
}

QPushButton#btnStep{
    background - color: #0E5C7C;
}
QPushButton#btnStep:hover{
    background - color: #19B6BE;
}

QPushButton#btnReset{
    background - color: #8C4192;
}
QPushButton#btnReset:hover{
    background - color: #E8D15D;
    color: #0B192C;
}

/* الجداول 
QTableWidget{
    background - color: #0F1C2E;
    gridline - color: #0E5C7C;
    color: #E0E6ED;
    border: 1px solid #0E5C7C;
    border - radius: 4px;
    selection - background - color: #19B6BE;
    selection - color: #0B192C;
}
QHeaderView::section{
    background - color: #0E5C7C;
    color: #E8D15D;
    font - weight: bold;
    border: 1px solid #0F1C2E;
    padding: 4px;
}

/* شاشة المخرجات 
QTextEdit{
    background - color: #060D17;
    color: #8BC281;
    border: 1px solid #19B6BE;
    border - radius: 4px;
    font - family: 'Consolas', monospace;
    font - size: 14px;
}

/* QComboBox 
QComboBox{
    background - color: #0E5C7C;
    color: #FFFFFF;
    border: 1px solid #19B6BE;
    border - radius: 4px;
    padding: 5px;
}

/* Status Labels 
QLabel#lblPC, QLabel#lblIR{
    color: #E8D15D;
    font - weight: bold;
}
QLabel#lblState{
    color: #8BC281;
    font - weight: bold;
}

QStatusBar{
    background - color: #0E5C7C;
    color: #E8D15D;
}
)";

this->setStyleSheet(styleSheet);
}
/*
void MainWindow::onLoadFile() { /* يتم التوصيل مع الـ Simulator لاحقاً */ }
//void MainWindow::onRun() { /* يتم التوصيل مع الـ Simulator لاحقاً */ }
//void MainWindow::onStep() { /* يتم التوصيل مع الـ Simulator لاحقاً */ }
//void MainWindow::onReset() { /* يتم التوصيل مع الـ Simulator لاحقاً */ }
//void MainWindow::onModeChanged(int index) { Q_UNUSED(index); }
/*void MainWindow::refreshAll()
{
    refreshMemoryGrid();
    refreshRegisters();
    refreshStatus();
    refreshScreen();
}

void MainWindow::refreshMemoryGrid() {}
void MainWindow::refreshRegisters() {}
void MainWindow::refreshStatus() {}
void MainWindow::refreshScreen() {}*/
