#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QScrollArea>
#include <QPushButton>
#include <QCheckBox>
#include <QLabel>
#include <QLineEdit>
#include <QTimer>
#include <vector>
#include <QString>
#include "animatedHoverButton.h"
#include "animatedHoverIcon.h"
#include "pokemonData.h"

class MainWindow : public QMainWindow
{
public:
    explicit MainWindow(QWidget *parent = nullptr);

private slots:
    void onCheckboxStateChanged();
    void onSearchTextChanged(const QString &text);
    void onHighlightMissingChanged();
    void onClearButtonClicked();
    void onConfirmTimerTimeout();
    void onEditListButtonClicked();

private:
    CustomPokemonList pokemonList;

    std::vector<QCheckBox *> checkboxes;
    QScrollArea *scrollArea;
    QLineEdit *searchBox;
    QLabel *progressCounter;
    QCheckBox *highlightMissing;
    AnimatedHoverIcon *iconLabel;
    AnimatedHoverButton *clearButton;
    AnimatedHoverButton *editListButton;
    QTimer *confirmTimer;

    QString saveFilePath;
    bool isConfirmState;

    void setupUI();
    void setupBottomLayout(QVBoxLayout *mainLayout);
    void setupClearButtonConfirmation();
    void refreshPokemonList();

    void updateProgressCounter();
    void applyHighlights();
    void searchPokemon(const QString &searchText);
    void checkMissingPokemon();

    void saveChecklistState();
    void loadChecklistState();

    const std::vector<QString> &getCurrentPokemonList() const;
};

#endif
