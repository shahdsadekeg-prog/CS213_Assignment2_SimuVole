#pragma once

#include <QMainWindow>
#include <QPushButton>
#include <QTableWidget>
#include <QTextEdit>
#include <QLineEdit>
#include <QComboBox>
#include <QLabel>
#include <vector>
#include <string>

#include "Memory.h"
#include "ProgramLoader.h"
#include "ALU.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

private slots:
    void onLoadFile();
    void onRun();
    void onStop();
    void onStep();
    void onSingleStep();
    void onReset();
    void onStoreInMemory();
    void onModeChanged(int index);

private:
    void setupUI();
    void setupToolbar(QWidget* parent);
    void setupScreenPanel(QWidget* parent);
    void setupMemoryGrid(QWidget* parent);
    void setupRegistersPanel(QWidget* parent);
    void setupConnections();
    void applyStyles();

    void refreshAll();
    void refreshMemoryGrid();
    void refreshRegisters();
    void refreshRegistersGrid();
    void refreshStatus();
    void refreshScreen();

    // ---- Visual Widgets ----
    QPushButton* btnLoad;
    QPushButton* btnRun;
    QPushButton* btnStop;
    QPushButton* btnStep;
    QPushButton* btnReset;
    QPushButton* btnStore;
    QComboBox* cmbMode;

    QLabel* lblPC;
    QLabel* lblIR;
    QLabel* lblState;

    QTableWidget* memoryTable;
    QTableWidget* registerTable;
    QTextEdit* screenOutput;
    QTextEdit* txtDirectCode;
    QLineEdit* txtStartAddress;

    // ---- Engine Data & Objects ----
    Memory memory;
    ProgramLoader loader;
    std::vector<uint8_t> registers{ std::vector<uint8_t>(16, 0) };
    int pc = 0;
    std::string currentIR = "0000";
};