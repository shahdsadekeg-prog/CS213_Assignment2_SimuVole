#include "MainWindow.h"
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
#include <QTimer>
#include <QCoreApplication>
#include <cctype>

#include "Memory.h"
#include "ProgramLoader.h"
#include "CPU.h"
#include "ALU.h"

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    setupUI();
    setupConnections();
    applyStyles();
    refreshAll();
}

MainWindow::~MainWindow()
{
}

void MainWindow::setupUI()
{
    setWindowTitle("Vole Machine Simulator - Advanced Starry Night Edition");
    resize(1340, 860);

    QWidget* central = new QWidget(this);
    setCentralWidget(central);

    QVBoxLayout* mainLayout = new QVBoxLayout(central);
    mainLayout->setContentsMargins(12, 12, 12, 12);
    mainLayout->setSpacing(10);

    // Toolbar
    QWidget* toolbar = new QWidget(central);
    setupToolbar(toolbar);
    mainLayout->addWidget(toolbar);

    mainLayout->addSpacing(10);

    // Main Splitter
    QSplitter* hSplitter = new QSplitter(Qt::Horizontal, central);

    // Left: Screen Output
    QGroupBox* screenGroup = new QGroupBox("Screen Output (Memory mapped at 0xFF)", hSplitter);
    setupScreenPanel(screenGroup);
    hSplitter->addWidget(screenGroup);

    // Middle: Registers Table
    QGroupBox* regGroup = new QGroupBox("Registers (CPU)", hSplitter);
    QVBoxLayout* regLayout = new QVBoxLayout(regGroup);
    regLayout->setContentsMargins(10, 20, 10, 10);
    setupRegistersPanel(regGroup);
    regLayout->addWidget(registerTable);
    hSplitter->addWidget(regGroup);

    // Right: Main Memory Table
    QGroupBox* memGroup = new QGroupBox("Main Memory (256 Bytes)", hSplitter);
    QVBoxLayout* memLayout = new QVBoxLayout(memGroup);
    memLayout->setContentsMargins(10, 20, 10, 10);
    setupMemoryGrid(memGroup);
    memLayout->addWidget(memoryTable);
    hSplitter->addWidget(memGroup);

    hSplitter->setStretchFactor(0, 3);
    hSplitter->setStretchFactor(1, 3);
    hSplitter->setStretchFactor(2, 4);

    mainLayout->addWidget(hSplitter, 1);

    // Footer Bar
    QLabel* lblFooter = new QLabel(
        "This app was developed at the Faculty of Computers and Artificial Intelligence, Cairo University "
        "as part of an assignment under the supervision of Dr. Mohamed El-Ramly.\n"
        "Copyright © 2025",
        this
    );
    lblFooter->setAlignment(Qt::AlignCenter);
    lblFooter->setObjectName("lblFooter");
    mainLayout->addWidget(lblFooter);

    statusBar()->showMessage("Ready");
}

void MainWindow::setupToolbar(QWidget* parent)
{
    QHBoxLayout* layout = new QHBoxLayout(parent);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(8);

    btnLoad = new QPushButton("Load File", parent);
    btnRun = new QPushButton("Run", parent);
    btnStop = new QPushButton("Stop", parent);
    btnStep = new QPushButton("Single Step", parent);
    btnReset = new QPushButton("Reset", parent);
    cmbMode = new QComboBox(parent);

    lblPC = new QLabel("PC: 00", parent);
    lblIR = new QLabel("IR: 0000", parent);
    lblState = new QLabel("State: READY", parent);

    btnLoad->setObjectName("btnLoad");
    btnRun->setObjectName("btnRun");
    btnStop->setObjectName("btnStop");
    btnStep->setObjectName("btnStep");
    btnReset->setObjectName("btnReset");

    cmbMode->addItem("Continuous Execution");
    cmbMode->addItem("Step-by-Step Execution");

    layout->addWidget(btnLoad);
    layout->addWidget(btnRun);
    layout->addWidget(btnStop);
    layout->addWidget(btnStep);
    layout->addWidget(btnReset);
    layout->addWidget(cmbMode);
    layout->addSpacing(15);
    layout->addWidget(lblPC);
    layout->addSpacing(10);
    layout->addWidget(lblIR);
    layout->addSpacing(10);
    layout->addWidget(lblState);
    layout->addStretch();
}

void MainWindow::setupScreenPanel(QWidget* parent)
{
    QVBoxLayout* layout = qobject_cast<QVBoxLayout*>(parent->layout());
    if (!layout) {
        layout = new QVBoxLayout(parent);
        layout->setContentsMargins(10, 20, 10, 10);
    }

    // شاشة العرض
    screenOutput = new QTextEdit(parent);
    screenOutput->setReadOnly(true);

    QHBoxLayout* addrLayout = new QHBoxLayout();
    QLabel* lblStart = new QLabel("Start Address (Hex):", parent);
    txtStartAddress = new QLineEdit("00", parent);
    txtStartAddress->setMaximumWidth(60);
    addrLayout->addWidget(lblStart);
    addrLayout->addWidget(txtStartAddress);
    addrLayout->addStretch();

    // مربع كتابة التعليمات
    txtDirectCode = new QTextEdit(parent);
    txtDirectCode->setPlaceholderText("Enter instructions here (e.g. 2145)...");

    btnStore = new QPushButton("Store in Memory", parent);
    btnStore->setObjectName("btnStore");

    // توزيع المساحة بشكل متساوي (Stretch Factor = 1 لكل منهما)
    layout->addWidget(screenOutput, 1);
    layout->addLayout(addrLayout);
    layout->addWidget(txtDirectCode, 1);
    layout->addWidget(btnStore);
}

void MainWindow::setupMemoryGrid(QWidget* parent)
{
    memoryTable = new QTableWidget(256, 3, parent);
    memoryTable->setHorizontalHeaderLabels(QStringList() << "Address" << "Binary" << "Hexadecimal");
    memoryTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    for (int i = 0; i < 256; ++i) {
        QString addrStr = QString("%1").arg(i, 2, 16, QChar('0')).toUpper();
        memoryTable->setItem(i, 0, new QTableWidgetItem(addrStr));
        memoryTable->setItem(i, 1, new QTableWidgetItem("00000000"));
        memoryTable->setItem(i, 2, new QTableWidgetItem("00"));
    }
}

void MainWindow::setupRegistersPanel(QWidget* parent)
{
    registerTable = new QTableWidget(16, 3, parent);
    registerTable->setHorizontalHeaderLabels(QStringList() << "Register" << "Binary" << "Hexadecimal");
    registerTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    for (int i = 0; i < 16; ++i) {
        QString regName = "R" + QString::number(i, 16).toUpper();
        registerTable->setItem(i, 0, new QTableWidgetItem(regName));
        registerTable->setItem(i, 1, new QTableWidgetItem("00000000"));
        registerTable->setItem(i, 2, new QTableWidgetItem("00"));
    }
}

void MainWindow::setupConnections()
{
    connect(btnLoad, &QPushButton::clicked, this, &MainWindow::onLoadFile);
    connect(btnRun, &QPushButton::clicked, this, &MainWindow::onRun);
    connect(btnStop, &QPushButton::clicked, this, &MainWindow::onStop);
    connect(btnStep, &QPushButton::clicked, this, &MainWindow::onSingleStep);
    connect(btnReset, &QPushButton::clicked, this, &MainWindow::onReset);
    connect(btnStore, &QPushButton::clicked, this, &MainWindow::onStoreInMemory);
    connect(cmbMode, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::onModeChanged);
}

void MainWindow::applyStyles()
{
    QString styleSheet = R"(
QMainWindow, QWidget {
    background-color: #34283F;
    color: #BDDEDD;
    font-family: 'Segoe UI', Tahoma, Geneva, sans-serif;
    font-size: 13px;
}

QGroupBox {
    background-color: #564A70;
    border: 2px solid #7F5388;
    border-radius: 8px;
    margin-top: 15px;
    padding-top: 10px;
    font-weight: bold;
    color: #BDDEDD;
}

QGroupBox::title {
    subcontrol-origin: margin;
    subcontrol-position: top left;
    left: 12px;
    top: 2px;
    padding: 3px 10px;
    background-color: #7F5388;
    color: #BDDEDD;
    border-radius: 4px;
}

QPushButton {
    border: none;
    border-radius: 5px;
    padding: 7px 14px;
    font-weight: bold;
    color: #34283F;
    background-color: #8BB9C1;
}

QPushButton:hover {
    background-color: #BDDEDD;
    color: #34283F;
}

QPushButton#btnLoad, QPushButton#btnStore, QPushButton#btnRun, QPushButton#btnStep, QPushButton#btnReset {
    background-color: #8BB9C1;
    color: #34283F;
}

QPushButton#btnLoad:hover, QPushButton#btnStore:hover, QPushButton#btnRun:hover, QPushButton#btnStep:hover, QPushButton#btnReset:hover {
    background-color: #BDDEDD;
}

QPushButton#btnStop {
    background-color: #7F5388;
    color: #BDDEDD;
}

QPushButton#btnStop:hover {
    background-color: #8BB9C1;
    color: #34283F;
}

QTableWidget {
    background-color: #BDDEDD;
    gridline-color: #564A70;
    color: #34283F;
    border: 1px solid #7F5388;
    border-radius: 4px;
    selection-background-color: #8BB9C1;
    selection-color: #34283F;
}

QTableCornerButton::section {
    background-color: #BDDEDD;
    border: 1px solid #564A70;
}

QHeaderView::section {
    background-color: #7F5388;
    color: #BDDEDD;
    font-weight: bold;
    border: 1px solid #564A70;
    padding: 4px;
}

QHeaderView::section:vertical {
    background-color: #BDDEDD;
    color: #34283F;
    border: 1px solid #564A70;
}

QTextEdit, QLineEdit {
    background-color: #BDDEDD;
    color: #34283F;
    border: 1px solid #7F5388;
    border-radius: 4px;
    font-family: 'Consolas', monospace;
    font-size: 13px;
}

QComboBox {
    background-color: #8BB9C1;
    color: #34283F;
    border: 1px solid #7F5388;
    border-radius: 4px;
    padding: 5px;
    font-weight: bold;
}

QComboBox QAbstractItemView {
    background-color: #BDDEDD;
    color: #34283F;
    selection-background-color: #8BB9C1;
}

QLabel#lblPC, QLabel#lblIR, QLabel#lblState, QLabel#lblFooter {
    color: #BDDEDD;
    font-weight: bold;
}

QLabel#lblFooter {
    font-size: 11px;
    padding-top: 6px;
    padding-bottom: 2px;
}

QStatusBar {
    background-color: #7F5388;
    color: #BDDEDD;
}
)";
    this->setStyleSheet(styleSheet);
}

void MainWindow::onLoadFile()
{
    QString fileName = QFileDialog::getOpenFileName(
        this,
        "Open Vole Machine Code File",
        "",
        "Text Files (*.txt);;All Files (*)"
    );

    if (fileName.isEmpty()) return;

    bool ok = false;
    int startAddr = txtStartAddress->text().toInt(&ok, 16);
    if (!ok || startAddr < 0 || startAddr >= 256 || startAddr % 2 != 0) {
        startAddr = 0;
        txtStartAddress->setText("00");
    }

    if (loader.loadProgramFromFile(fileName.toStdString(), startAddr, memory)) {
        pc = startAddr;
        refreshAll();
        lblState->setText("State: LOADED");
        QMessageBox::information(this, "Success", "Program loaded successfully into memory!");
    }
    else {
        QMessageBox::critical(this, "Error", "Failed to load program!\nCheck file path, Hex format, or start address.");
    }
}

void MainWindow::onRun()
{
    lblState->setText("State: RUNNING...");
    btnRun->setEnabled(false);
    btnStep->setEnabled(false);

    while (pc < 256 && lblState->text() != "State: STOPPED") {
        unsigned char b1 = memory.read(pc);
        unsigned char b2 = memory.read(pc + 1);

        QString instStr = QString("%1%2")
            .arg(b1, 2, 16, QChar('0'))
            .arg(b2, 2, 16, QChar('0')).toUpper();

        currentIR = instStr.toStdString();

        if (instStr == "0000" || instStr.startsWith("C")) {
            pc += 2;
            lblState->setText("State: FINISHED");
            break;
        }

        Decoder dec = ALU::decode(currentIR);
        if (!dec.isValid) {
            lblState->setText("State: ERROR (Invalid Instruction)");
            break;
        }

        int nextPC = pc + 2;

        // معالجة الأوامر التي تتعامل مع الذاكرة والمسجلات مباشرة
        if (dec.opcode == 1) { // Load R from Memory XY
            registers[dec.R] = memory.read(dec.xy);
            pc = nextPC;
        }
        else if (dec.opcode == 2) { // Load R with Immediate Value XY
            registers[dec.R] = static_cast<uint8_t>(dec.xy);
            pc = nextPC;
        }
        else if (dec.opcode == 3) { // Store R into Memory XY
            memory.write(dec.xy, registers[dec.R]);
            pc = nextPC;
        }
        else if (dec.opcode == 4) { // Move R_S to R_T
            registers[dec.T] = registers[dec.S];
            pc = nextPC;
        }
        else { // الأوامر الحسابية والمنطقية والقفز (5-B) تُنفّذ عبر ALU
            ALU::execute(dec, registers, nextPC);
            pc = nextPC;
        }

        refreshAll();
        QCoreApplication::processEvents();
    }

    if (lblState->text() != "State: STOPPED" && !lblState->text().startsWith("State: ERROR")) {
        lblState->setText("State: FINISHED");
    }

    refreshAll();
    btnRun->setEnabled(true);
    btnStep->setEnabled(true);
}

void MainWindow::onStop()
{
    lblState->setText("State: STOPPED");
}

void MainWindow::onStep()
{
    onSingleStep();
}

void MainWindow::onSingleStep()
{
    if (pc >= 256) {
        lblState->setText("State: HALTED");
        return;
    }

    unsigned char b1 = memory.read(pc);
    unsigned char b2 = memory.read(pc + 1);

    QString instStr = QString("%1%2")
        .arg(b1, 2, 16, QChar('0'))
        .arg(b2, 2, 16, QChar('0')).toUpper();

    currentIR = instStr.toStdString();

    if (instStr == "0000" || instStr.startsWith("C")) {
        pc += 2;
        lblState->setText("State: HALTED");
        refreshAll();
        return;
    }

    Decoder dec = ALU::decode(currentIR);
    if (dec.isValid) {
        int nextPC = pc + 2;

        if (dec.opcode == 1) {
            registers[dec.R] = memory.read(dec.xy);
            pc = nextPC;
        }
        else if (dec.opcode == 2) {
            registers[dec.R] = static_cast<uint8_t>(dec.xy);
            pc = nextPC;
        }
        else if (dec.opcode == 3) {
            memory.write(dec.xy, registers[dec.R]);
            pc = nextPC;
        }
        else if (dec.opcode == 4) {
            registers[dec.T] = registers[dec.S];
            pc = nextPC;
        }
        else {
            ALU::execute(dec, registers, nextPC);
            pc = nextPC;
        }
        lblState->setText("State: STEPPED");
    }
    else {
        lblState->setText("State: ERROR");
    }

    refreshAll();
}

void MainWindow::onReset()
{
    memory.clear();
    std::fill(registers.begin(), registers.end(), 0);
    pc = 0;
    currentIR = "0000";
    screenOutput->clear();
    refreshAll();
    lblState->setText("State: READY");
    btnRun->setEnabled(true);
    btnStep->setEnabled(true);
}

void MainWindow::onStoreInMemory()
{
    bool ok = false;
    int startAddr = txtStartAddress->text().toInt(&ok, 16);
    if (!ok || startAddr < 0 || startAddr >= 256 || startAddr % 2 != 0) {
        QMessageBox::warning(this, "Invalid Address", "Please enter a valid EVEN start address in Hex (e.g., 00, 02, 10).");
        return;
    }

    pc = startAddr;

    QString codeText = txtDirectCode->toPlainText().trimmed();
    if (codeText.isEmpty()) {
        QMessageBox::warning(this, "Empty Input", "Please enter instructions to store.");
        return;
    }

    QStringList lines = codeText.split('\n', Qt::SkipEmptyParts);
    int currentAddr = startAddr;
    int storedCount = 0;

    for (QString line : lines) {
        line = line.trimmed();
        int commentIdx = line.indexOf(';');
        if (commentIdx != -1) line = line.left(commentIdx).trimmed();
        line.remove(' ');

        if (line.length() == 4) {
            bool hex1Ok = false, hex2Ok = false;
            int b1 = line.mid(0, 2).toInt(&hex1Ok, 16);
            int b2 = line.mid(2, 2).toInt(&hex2Ok, 16);

            if (hex1Ok && hex2Ok && (currentAddr + 1 < 256)) {
                memory.write(currentAddr, static_cast<unsigned char>(b1));
                memory.write(currentAddr + 1, static_cast<unsigned char>(b2));
                currentAddr += 2;
                storedCount++;
            }
        }
    }

    if (storedCount > 0) {
        refreshAll();
        lblState->setText("State: READY");
        QMessageBox::information(this, "Success", QString("%1 instruction(s) stored successfully!").arg(storedCount));
    }
    else {
        QMessageBox::warning(this, "Error", "No valid 4-digit Hex instructions found.");
    }
}

void MainWindow::onModeChanged(int index)
{
    if (index == 0) { // Continuous Execution
        btnRun->setEnabled(true);
        btnStep->setEnabled(false);
    }
    else { // Step-by-Step Execution
        btnRun->setEnabled(true);
        btnStep->setEnabled(true);
    }
}

void MainWindow::refreshAll()
{
    refreshMemoryGrid();
    refreshRegisters();
    refreshStatus();
    refreshScreen();
}

void MainWindow::refreshMemoryGrid()
{
    for (int i = 0; i < 256; ++i) {
        unsigned char val = memory.read(i);
        QString hexStr = QString("%1").arg(val, 2, 16, QChar('0')).toUpper();
        QString binStr = QString("%1").arg(val, 8, 2, QChar('0'));

        if (memoryTable->item(i, 1)) memoryTable->item(i, 1)->setText(binStr);
        if (memoryTable->item(i, 2)) memoryTable->item(i, 2)->setText(hexStr);
    }
}

void MainWindow::refreshRegisters()
{
    for (int i = 0; i < 16; ++i) {
        uint8_t val = registers[i];

        QString hexStr = QString("%1").arg(val, 2, 16, QChar('0')).toUpper();
        QString binStr = QString("%1").arg(val, 8, 2, QChar('0'));

        if (registerTable->item(i, 1)) {
            registerTable->item(i, 1)->setText(binStr);
        }
        if (registerTable->item(i, 2)) {
            registerTable->item(i, 2)->setText(hexStr);
        }
    }
}

void MainWindow::refreshStatus()
{
    lblPC->setText(QString("PC: %1").arg(pc, 2, 16, QChar('0')).toUpper());
    lblIR->setText(QString("IR: %1").arg(QString::fromStdString(currentIR)).toUpper());
}

void MainWindow::refreshScreen()
{
    screenOutput->clear(); // مسح الشاشة قبل الطباعة لمنع التكرار
    unsigned char screenVal = memory.read(0xFF);
    if (screenVal != 0) {
        char ch = static_cast<char>(screenVal);
        if (std::isprint(static_cast<unsigned char>(ch))) {
            screenOutput->setText(QString(ch));
        }
        else {
            screenOutput->setText(QString("[0x%1]").arg(screenVal, 2, 16, QChar('0')).toUpper());
        }
    }
}