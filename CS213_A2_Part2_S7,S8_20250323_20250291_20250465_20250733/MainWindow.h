#pragma once

/*#include <QMainWindow>

class QTableWidget;
class QTextEdit;
class QLabel;
class QPushButton;
class QComboBox;

// ---- Backend forward declarations ----
class Simulator;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override;

private slots:
    void onLoadFile();
    void onRun();
    void onStep();
    void onReset();
    void onModeChanged(int index);

private:
    void setupUI();
    void setupToolbar(QWidget* parent);
    void setupMemoryGrid(QWidget* parent);
    void setupRegistersPanel(QWidget* parent);
    void setupStatusPanel(QWidget* parent);
    void setupScreenPanel(QWidget* parent);
    void setupConnections();
    void applyStyles();

    void refreshAll();
    void refreshMemoryGrid();
    void refreshRegisters();
    void refreshStatus();
    void refreshScreen();

    // ---- UI ----
    QPushButton* btnLoad = nullptr;
    QPushButton* btnRun = nullptr;
    QPushButton* btnStep = nullptr;
    QPushButton* btnReset = nullptr;
    QComboBox* cmbMode = nullptr;

    QTableWidget* memoryTable = nullptr;
    QTableWidget* registerTable = nullptr;

    QLabel* lblPC = nullptr;
    QLabel* lblIR = nullptr;
    QLabel* lblState = nullptr;

    QTextEdit* screenOutput = nullptr;

    // ---- Backend ----
    Simulator* simulator = nullptr;
    int highlightedPC = -1;
};*/