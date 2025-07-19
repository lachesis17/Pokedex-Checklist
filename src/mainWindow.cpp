#include "mainWindow.h"
#include "animatedHoverButton.h"
#include "pokemonData.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QFile>
#include <QDir>
#include <QStandardPaths>
#include <QFont>
#include <QTextStream>
#include <QDialog>
#include <QListWidget>
#include <QInputDialog>
#include <QMessageBox>
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/prettywriter.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), isConfirmState(false)
{
    pokemonList.listName = "Kanto Pokédex";
    pokemonList.pokemon = DEFAULT_KANTO_POKEMON;
    pokemonList.isCustom = false;

    setupUI();
    loadChecklistState();
    updateProgressCounter();
}

void MainWindow::setupUI()
{
    setWindowTitle(pokemonList.listName + " Checklist");
    resize(500, 600);

    auto *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    auto *mainLayout = new QVBoxLayout(centralWidget);

    auto *titleLayout = new QHBoxLayout();
    titleLayout->setContentsMargins(0, 0, 0, 0);

    QWidget *titleContainer = new QWidget(this);
    auto *centerLayout = new QHBoxLayout(titleContainer);
    centerLayout->setContentsMargins(0, 0, 0, 0);
    centerLayout->setSpacing(8);

    auto *titleLabel = new QLabel(QString("%1 Checklist (%2)")
                                      .arg(pokemonList.listName)
                                      .arg(static_cast<int>(getCurrentPokemonList().size())),
                                  titleContainer);
    QFont titleFont = titleLabel->font();
    titleFont.setPointSize(16);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);
    titleLabel->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);
    titleLabel->setMargin(8);
    centerLayout->addWidget(titleLabel);

    QPixmap iconPixmap(":/logo.png");
    iconLabel = new AnimatedHoverIcon("https://github.com/lachesis17/Pokedex-Checklist", titleContainer);
    iconLabel->setPixmap(iconPixmap.scaled(64, 64, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    iconLabel->setAlignment(Qt::AlignCenter);
    iconLabel->resize(64, 64);
    centerLayout->addWidget(iconLabel);

    centerLayout->addStretch();
    titleLayout->addStretch();
    titleLayout->addWidget(titleContainer, 0, Qt::AlignHCenter);
    titleLayout->addStretch();

    mainLayout->addLayout(titleLayout);

    searchBox = new QLineEdit(this);
    searchBox->setPlaceholderText("Pokémon search...");
    searchBox->setClearButtonEnabled(true);
    searchBox->setFixedHeight(30);
    searchBox->setTextMargins(4, 0, 0, 0);
    connect(searchBox, &QLineEdit::textChanged, this, &MainWindow::onSearchTextChanged);
    mainLayout->addWidget(searchBox);

    scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    mainLayout->addWidget(scrollArea);

    refreshPokemonList();

    setupBottomLayout(mainLayout);

    QString appDataPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(appDataPath);
    saveFilePath = appDataPath + "/" + SAVE_FILE;
}

void MainWindow::setupBottomLayout(QVBoxLayout *mainLayout)
{
    auto *bottomLayout = new QGridLayout();
    bottomLayout->setContentsMargins(0, 6, 0, 6);

    progressCounter = new QLabel(QString("0/%1 caught").arg(getCurrentPokemonList().size()), this);
    progressCounter->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    editListButton = new AnimatedHoverButton("Edit List", this);
    editListButton->setHoverColor(QColor("#58D68D"));
    connect(editListButton, &AnimatedHoverButton::clicked, this, &MainWindow::onEditListButtonClicked);
    editListButton->setToolTip("Customize your Pokemon list");

    clearButton = new AnimatedHoverButton("Clear All", this);
    setupClearButtonConfirmation();
    clearButton->setToolTip("Clear all checkboxes");

    highlightMissing = new QCheckBox("Highlight Missing", this);
    highlightMissing->setChecked(false);
    connect(highlightMissing, &QCheckBox::checkStateChanged, this, &MainWindow::onHighlightMissingChanged);

    bottomLayout->addWidget(progressCounter, 0, 0, Qt::AlignLeft | Qt::AlignVCenter);
    bottomLayout->addWidget(editListButton, 0, 1, Qt::AlignCenter);
    bottomLayout->addWidget(clearButton, 0, 2, Qt::AlignCenter);
    bottomLayout->addWidget(highlightMissing, 0, 3, Qt::AlignRight | Qt::AlignVCenter);

    bottomLayout->setColumnStretch(0, 1);
    bottomLayout->setColumnStretch(1, 1);
    bottomLayout->setColumnStretch(2, 1);
    bottomLayout->setColumnStretch(3, 1);

    mainLayout->addLayout(bottomLayout);
}

void MainWindow::refreshPokemonList()
{
    for (auto *checkbox : checkboxes)
    {
        delete checkbox;
    }
    checkboxes.clear();

    auto *containerWidget = new QWidget();
    auto *containerLayout = new QVBoxLayout(containerWidget);

    const auto &currentList = getCurrentPokemonList();
    checkboxes.reserve(currentList.size());

    for (size_t i = 0; i < currentList.size(); ++i)
    {
        QString pokemonText = QString("#%1 - %2").arg(i + 1, 3, 10, QChar('0')).arg(currentList[i]);
        auto *checkbox = new QCheckBox(pokemonText, containerWidget);
        connect(checkbox, &QCheckBox::checkStateChanged, this, &MainWindow::onCheckboxStateChanged);
        checkboxes.push_back(checkbox);
        containerLayout->addWidget(checkbox);
    }

    containerLayout->addStretch();
    scrollArea->setWidget(containerWidget);

    setWindowTitle("Pokemon " + pokemonList.listName + " Checklist");
}

void MainWindow::onEditListButtonClicked()
{
    QDialog dialog(this);
    dialog.setWindowTitle("Edit Pokemon List");
    dialog.resize(400, 500);

    auto *layout = new QVBoxLayout(&dialog);
    auto *nameLayout = new QHBoxLayout();
    nameLayout->addWidget(new QLabel("List Name:"));
    auto *nameEdit = new QLineEdit(pokemonList.listName);
    nameLayout->addWidget(nameEdit);
    layout->addLayout(nameLayout);

    auto *listWidget = new QListWidget();
    listWidget->setSelectionMode(QAbstractItemView::MultiSelection);
    const auto &currentList = getCurrentPokemonList();
    for (const QString &pokemon : currentList)
    {
        listWidget->addItem(pokemon);
    }
    layout->addWidget(listWidget);

    auto *buttonLayout = new QHBoxLayout();
    auto *addButton = new QPushButton("Add");
    connect(addButton, &QPushButton::clicked, [listWidget]()
            {
        bool ok;
        QString name = QInputDialog::getText(nullptr, "Add", "Name:", QLineEdit::Normal, "", &ok);
        if (ok && !name.isEmpty()) {
            listWidget->addItem(name);
        } });

    auto *removeButton = new QPushButton("Remove Selected");
    connect(removeButton, &QPushButton::clicked, [listWidget]()
            {
        QList<QListWidgetItem *> selectedItems = listWidget->selectedItems();
        for (int i = selectedItems.size() - 1; i >= 0; --i) {
            delete listWidget->takeItem(listWidget->row(selectedItems[i]));
        } });

    auto *resetButton = new QPushButton("Reset to Default");
    connect(resetButton, &QPushButton::clicked, [listWidget, nameEdit]()
            {
        listWidget->clear();
        for (const QString& pokemon : DEFAULT_KANTO_POKEMON) {
            listWidget->addItem(pokemon);
        }
        nameEdit->setText("Kanto"); });

    buttonLayout->addWidget(addButton);
    buttonLayout->addWidget(removeButton);
    buttonLayout->addWidget(resetButton);
    layout->addLayout(buttonLayout);

    auto *okCancelLayout = new QHBoxLayout();
    auto *okButton = new QPushButton("OK");
    auto *cancelButton = new QPushButton("Cancel");

    connect(okButton, &QPushButton::clicked, &dialog, &QDialog::accept);
    connect(cancelButton, &QPushButton::clicked, &dialog, &QDialog::reject);

    okCancelLayout->addStretch();
    okCancelLayout->addWidget(okButton);
    okCancelLayout->addWidget(cancelButton);
    layout->addLayout(okCancelLayout);

    if (dialog.exec() == QDialog::Accepted)
    {
        pokemonList.listName = nameEdit->text().trimmed();
        if (pokemonList.listName.isEmpty())
        {
            pokemonList.listName = "Custom List";
        }

        pokemonList.pokemon.clear();
        for (int i = 0; i < listWidget->count(); ++i)
        {
            pokemonList.pokemon.push_back(listWidget->item(i)->text());
        }

        pokemonList.isCustom = (pokemonList.pokemon != DEFAULT_KANTO_POKEMON || pokemonList.listName != "Kanto");

        refreshPokemonList();
        updateProgressCounter();
        saveChecklistState();
    }
}

const std::vector<QString> &MainWindow::getCurrentPokemonList() const
{
    return pokemonList.isCustom ? pokemonList.pokemon : DEFAULT_KANTO_POKEMON;
}

void MainWindow::setupClearButtonConfirmation()
{
    confirmTimer = new QTimer(this);
    confirmTimer->setSingleShot(true);
    confirmTimer->setInterval(3000);

    connect(clearButton, &AnimatedHoverButton::clicked, this, &MainWindow::onClearButtonClicked);
    connect(confirmTimer, &QTimer::timeout, this, &MainWindow::onConfirmTimerTimeout);
}

void MainWindow::onCheckboxStateChanged()
{
    saveChecklistState();
    updateProgressCounter();
    applyHighlights();
}

void MainWindow::onSearchTextChanged(const QString &text)
{
    searchPokemon(text);
}

void MainWindow::onHighlightMissingChanged()
{
    checkMissingPokemon();
}

void MainWindow::onClearButtonClicked()
{
    if (!isConfirmState)
    {
        isConfirmState = true;
        clearButton->setText("Are you sure?");
        clearButton->setStyleSheet(
            "QPushButton {"
            "    background-color: #740000;"
            "    color: yellow;"
            "    padding: 8px 16px;"
            "    margin: 2px;"
            "    border: 1px solid #999;"
            "    border-radius: 6px;"
            "    font-size: 12px;"
            "    font-weight: bold;"
            "}");
        confirmTimer->start();
    }
    else
    {
        for (auto *checkbox : checkboxes)
        {
            checkbox->setChecked(false);
        }
        saveChecklistState();
        updateProgressCounter();

        isConfirmState = false;
        clearButton->setText("Clear All");
        confirmTimer->stop();
        clearButton->restoreAnimatedState();
    }
}

void MainWindow::onConfirmTimerTimeout()
{
    isConfirmState = false;
    clearButton->setText("Clear All");
    clearButton->restoreAnimatedState();
}

void MainWindow::updateProgressCounter()
{
    int checkedCount = 0;
    for (const auto *checkbox : checkboxes)
    {
        if (checkbox->isChecked())
        {
            checkedCount++;
        }
    }

    const int totalCount = static_cast<int>(getCurrentPokemonList().size());
    progressCounter->setText(QString("%1/%2 caught").arg(checkedCount).arg(totalCount));

    if (checkedCount == totalCount)
    {
        progressCounter->setStyleSheet(
            "QLabel {"
            "    font-size: 12px;"
            "    font-weight: bold;"
            "    font-style: italic;"
            "    color: #4CAF50;"
            "    padding: 4px 8px;"
            "}");
    }
    else
    {
        progressCounter->setStyleSheet(
            "QLabel {"
            "    font-size: 12px;"
            "    font-weight: bold;"
            "    font-style: italic;"
            "    color: #666;"
            "    padding: 4px 8px;"
            "}");
    }
}

void MainWindow::applyHighlights()
{
    for (auto *checkbox : checkboxes)
    {
        checkbox->setStyleSheet("");
        checkbox->update();
    }

    if (highlightMissing->isChecked())
    {
        for (auto *checkbox : checkboxes)
        {
            if (!checkbox->isChecked())
            {
                checkbox->setStyleSheet("QCheckBox { background-color: #20375d; }");
            }
        }
    }

    QString searchText = searchBox->text();
    if (!searchText.isEmpty())
    {
        for (size_t i = 0; i < checkboxes.size(); ++i)
        {
            QString checkboxText = checkboxes[i]->text();
            if (checkboxText.contains(searchText, Qt::CaseInsensitive))
            {
                scrollArea->ensureWidgetVisible(checkboxes[i]);
                checkboxes[i]->setStyleSheet("QCheckBox { background-color: #480053; }");
                checkboxes[i]->update();
                break;
            }
        }
    }
}

void MainWindow::searchPokemon(const QString &searchText)
{
    applyHighlights();
}

void MainWindow::checkMissingPokemon()
{
    applyHighlights();
}

void MainWindow::saveChecklistState()
{
    rapidjson::Document document;
    document.SetObject();
    rapidjson::Document::AllocatorType &allocator = document.GetAllocator();

    rapidjson::Value checkboxStates(rapidjson::kObjectType);
    for (size_t i = 0; i < checkboxes.size(); ++i)
    {
        std::string key = std::to_string(i);
        rapidjson::Value keyValue(key.c_str(), allocator);
        checkboxStates.AddMember(keyValue, checkboxes[i]->isChecked(), allocator);
    }
    document.AddMember("checkbox_states", checkboxStates, allocator);

    rapidjson::Value customList(rapidjson::kObjectType);
    rapidjson::Value listName(pokemonList.listName.toUtf8().constData(), allocator);
    customList.AddMember("list_name", listName, allocator);
    customList.AddMember("is_custom", pokemonList.isCustom, allocator);

    rapidjson::Value pokemonArray(rapidjson::kArrayType);
    for (const QString &pokemon : pokemonList.pokemon)
    {
        rapidjson::Value pokemonName(pokemon.toUtf8().constData(), allocator);
        pokemonArray.PushBack(pokemonName, allocator);
    }
    customList.AddMember("pokemon", pokemonArray, allocator);
    document.AddMember("custom_list", customList, allocator);

    rapidjson::StringBuffer buffer;
    rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);
    document.Accept(writer);

    QFile file(saveFilePath);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QTextStream out(&file);
        out << buffer.GetString();
        file.close();
    }
}

void MainWindow::loadChecklistState()
{
    QFile file(saveFilePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        return;
    }

    QTextStream in(&file);
    QString jsonString = in.readAll();
    file.close();

    rapidjson::Document document;
    document.Parse(jsonString.toUtf8().constData());

    if (document.HasParseError() || !document.IsObject())
    {
        return;
    }

    if (document.HasMember("custom_list") && document["custom_list"].IsObject())
    {
        const auto &customListObj = document["custom_list"];

        if (customListObj.HasMember("list_name") && customListObj["list_name"].IsString())
        {
            pokemonList.listName = QString::fromUtf8(customListObj["list_name"].GetString());
        }

        if (customListObj.HasMember("is_custom") && customListObj["is_custom"].IsBool())
        {
            pokemonList.isCustom = customListObj["is_custom"].GetBool();
        }

        if (customListObj.HasMember("pokemon") && customListObj["pokemon"].IsArray())
        {
            pokemonList.pokemon.clear();
            const auto &pokemonArray = customListObj["pokemon"];
            for (rapidjson::SizeType i = 0; i < pokemonArray.Size(); i++)
            {
                if (pokemonArray[i].IsString())
                {
                    pokemonList.pokemon.push_back(QString::fromUtf8(pokemonArray[i].GetString()));
                }
            }
        }

        refreshPokemonList();
    }

    if (document.HasMember("checkbox_states") && document["checkbox_states"].IsObject())
    {
        const auto &checkboxStates = document["checkbox_states"];
        for (size_t i = 0; i < checkboxes.size(); ++i)
        {
            std::string key = std::to_string(i);
            if (checkboxStates.HasMember(key.c_str()) && checkboxStates[key.c_str()].IsBool())
            {
                checkboxes[i]->blockSignals(true);
                checkboxes[i]->setChecked(checkboxStates[key.c_str()].GetBool());
                checkboxes[i]->blockSignals(false);
            }
        }
    }
    else
    {
        for (size_t i = 0; i < checkboxes.size(); ++i)
        {
            std::string key = std::to_string(i);
            if (document.HasMember(key.c_str()) && document[key.c_str()].IsBool())
            {
                checkboxes[i]->blockSignals(true);
                checkboxes[i]->setChecked(document[key.c_str()].GetBool());
                checkboxes[i]->blockSignals(false);
            }
        }
    }
}
