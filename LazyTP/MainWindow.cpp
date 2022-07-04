//
// Created by Staz on 25/06/2022.
//

#include "MainWindow.h"
#include <QSize>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonArray>
#include <QDialog>
#include <QDesktopServices>


MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), m_pMainLayout(new QVBoxLayout()),
                                          m_pCoordLayout(new QHBoxLayout()), m_pListWidgetLayout(new QHBoxLayout()),
                                          m_pCoordsInputLayout(new QHBoxLayout()), m_pButtonsLayout(new QHBoxLayout()),
                                          m_pCurrentSelectionLayout(new QHBoxLayout())
{
    m_helpText =
            "1. Connect to B&S using the \"Connect\"-Button.<br><br>"
            "2. Add a TP either by entering the name and the coordinates or if you want to use your current position, just enter a name and press the \"Add\"-Button.<br><br>"
            "3. To teleport, simply double click your desired TP and then press the configured key on your keyboard in game.<br><br>"
            "The default key is \"Numpad-9\". This can be changed in the <a href=\"coords.json\">coords.json</a>, which is located right beside this executable.<br>"
            "The number of the key is given by the <a href=\"https://cherrytree.at/misc/vk.htm\">virtual key codes</a>.<br>";
    m_pBns = Q_NULLPTR;
    m_pSelectedTP = Q_NULLPTR;
    m_jsonFileName = "coords.json";
    m_pMainWidget = new QWidget();
    m_pMainWidget->setLayout(m_pMainLayout);
    setCentralWidget(m_pMainWidget);
    setWindowTitle("Lazy TP");
    setMinimumSize(QSize(200, 600));

    m_pConnectButton = new QPushButton("Connect to BnS");
    m_pHelpButton = new QPushButton("?");
    m_pXCoord = new QLabel("X: ---");
    m_pYCoord = new QLabel("Y: ---");
    m_pZCoord = new QLabel("Z: ---");
    m_pTPList = new QListWidget();
    m_pCurrentSelectionLabel = new QLabel("<b>Currently no TP selected</b>");
    m_pCoordInputLabel = new QLabel("Name: ");
    m_pXInputLabel = new QLabel("X: ");
    m_pYInputLabel = new QLabel("Y: ");
    m_pZInputLabel = new QLabel("Z: ");
    m_pCoordInput = new QLineEdit();
    m_pXInput = new QLineEdit();
    m_pYInput = new QLineEdit();
    m_pZInput = new QLineEdit();
    m_pAddButton = new QPushButton("Add");
    m_pRemoveButton = new QPushButton("Remove");

    m_pMainLayout->addLayout(m_pCoordLayout);
    m_pMainLayout->addLayout(m_pListWidgetLayout);
    m_pMainLayout->addLayout(m_pCurrentSelectionLayout);
    m_pMainLayout->addLayout(m_pCoordsInputLayout);
    m_pMainLayout->addLayout(m_pButtonsLayout);

    m_pCoordLayout->addWidget(m_pConnectButton);
    m_pCoordLayout->addSpacing(25);
    m_pCoordLayout->addWidget(m_pXCoord);
    m_pCoordLayout->addWidget(m_pYCoord);
    m_pCoordLayout->addWidget(m_pZCoord);
    m_pCoordLayout->addWidget(m_pHelpButton, 0, Qt::AlignRight);

    m_pListWidgetLayout->addWidget(m_pTPList);

    m_pCurrentSelectionLayout->addWidget(m_pCurrentSelectionLabel, 0, Qt::AlignCenter);

    m_pCoordsInputLayout->addWidget(m_pCoordInputLabel);
    m_pCoordsInputLayout->addWidget(m_pCoordInput);
    m_pCoordsInputLayout->addWidget(m_pXInputLabel);
    m_pCoordsInputLayout->addWidget(m_pXInput);
    m_pCoordsInputLayout->addWidget(m_pYInputLabel);
    m_pCoordsInputLayout->addWidget(m_pYInput);
    m_pCoordsInputLayout->addWidget(m_pZInputLabel);
    m_pCoordsInputLayout->addWidget(m_pZInput);

    m_pButtonsLayout->addWidget(m_pAddButton);
    m_pButtonsLayout->addWidget(m_pRemoveButton);

    connect(m_pAddButton, &QPushButton::clicked, this, &MainWindow::onAddButton);
    connect(m_pRemoveButton, &QPushButton::clicked, this, &MainWindow::onRemoveButton);
    connect(m_pConnectButton, &QPushButton::clicked, this, &MainWindow::onConnectButton);
    connect(m_pHelpButton, &QPushButton::clicked, this, &MainWindow::onHelpButton);
    connect(m_pTPList, &QListWidget::itemDoubleClicked, this, &MainWindow::onTPSelected);

    m_pTPList->setSelectionBehavior(QAbstractItemView::SelectItems);
    m_pTPList->setSelectionMode(QAbstractItemView::SingleSelection);

    m_pCurrentSelectionLabel->setAlignment(Qt::AlignCenter);
    m_pCurrentSelectionLabel->setTextFormat(Qt::RichText);

    m_pIntValidator = new QIntValidator(this);
    QLocale locale = QLocale::c();
    locale.setNumberOptions(QLocale::RejectGroupSeparator);
    m_pIntValidator->setLocale(locale);
    m_pXInput->setValidator(m_pIntValidator);
    m_pYInput->setValidator(m_pIntValidator);
    m_pZInput->setValidator(m_pIntValidator);

    m_pTPMap = new QMap<QString, QVector3D>();
    loadJson();
    updateTPList();

    m_pTimer = new QTimer(this);
    connect(m_pTimer, &QTimer::timeout, this, &MainWindow::onTimer);
    m_pTimer->start(250);

    m_pHook = new KeyboardHook(m_keyCode);
    connect(m_pHook, &KeyboardHook::keyPressed, this, &MainWindow::onKeyPressed);
}

MainWindow::~MainWindow()
{
    delete m_pHook;
    delete m_pTPMap;
    delete m_pBns;
    delete m_pTimer;
}

void MainWindow::onKeyPressed()
{
    if (!m_pBns || !m_pSelectedTP)
        return;

    auto pos = (*m_pTPMap)[m_pSelectedTP->getName()];
    m_pBns->setCurrentPos(pos);
}

void MainWindow::onAddButton(bool checked)
{
    Q_UNUSED(checked)
    QVector3D pos;
    if (m_pXInput->text().isEmpty() || m_pYInput->text().isEmpty() || m_pZInput->text().isEmpty())
    {
        if (m_pCoordInput->text().isEmpty())
        {
            return;
        }
        else
        {
            if (!m_pBns)
                return;
            pos = m_pBns->getCurrentPos();
        }
    }
    else
    {
        pos = QVector3D(m_pXInput->text().toFloat(), m_pYInput->text().toFloat(), m_pZInput->text().toFloat());
    }
    m_pTPMap->insert(m_pCoordInput->text(), pos);
    m_pCoordInput->clear();
    m_pXInput->clear();
    m_pYInput->clear();
    m_pZInput->clear();
    saveJson();
    updateTPList();
}

void MainWindow::onRemoveButton(bool checked)
{
    Q_UNUSED(checked)
    auto selectedItemList = m_pTPList->selectedItems();
    if (selectedItemList.isEmpty())
        return;
    auto item = m_pTPList->selectedItems().at(0);
    auto name = dynamic_cast<TpListItem *>(item)->getName();
    m_pTPMap->remove(name);
    saveJson();
    updateTPList();
}

void MainWindow::loadJson()
{
    QFile file;
    file.setFileName(m_jsonFileName);
    QString val;

    if (file.open(QIODevice::ReadWrite | QIODeviceBase::Text))
    {
//        qDebug() << "Json opened - " + QString::number(file.size());
        val = file.readAll();
    }
    else
    {
        qDebug() << "Json could not be opened";
    }

    if (!file.exists() || val.isEmpty())
    {
        qDebug() << "Creating dummy file";
        QJsonObject content;
        QJsonArray contentArray;
        content.insert("keyCode", QJsonValue(0x69));
        m_keyCode = 0x69;
        content.insert("TPs", contentArray);
        QJsonDocument basicDoc(content);
        QTextStream stream(&file);
        stream << basicDoc.toJson(QJsonDocument::Indented);
        file.close();
        return;
    }
    file.close();
    QJsonDocument d = QJsonDocument::fromJson(val.toUtf8());
    QJsonObject mainObj = d.object();
    QJsonValue keyCode = mainObj["keyCode"];
    m_keyCode = keyCode.toInt();
    QJsonArray tps = mainObj["TPs"].toArray();
    m_pTPMap->clear();
    for (auto element: tps)
    {
        QJsonObject tp = element.toObject();
        QString name = tp["Name"].toString();
        double x = tp["X"].toDouble();
        double y = tp["Y"].toDouble();
        double z = tp["Z"].toDouble();
        m_pTPMap->insert(name, QVector3D(x, y, z));
    }
}

void MainWindow::saveJson()
{
    QJsonObject mainObj;
    mainObj.insert("keyCode", QJsonValue(m_keyCode));
    QJsonArray tps;
    for (auto e: m_pTPMap->keys())
    {
        QJsonObject tp;
        tp.insert("Name", QJsonValue::fromVariant(e));
        QVector3D pos = (*m_pTPMap)[e];
        tp.insert("X", QJsonValue::fromVariant(pos.x()));
        tp.insert("Y", QJsonValue::fromVariant(pos.y()));
        tp.insert("Z", QJsonValue::fromVariant(pos.z()));
        tps.push_back(tp);
    }
    mainObj.insert("TPs", tps);
    QJsonDocument jsonDoc(mainObj);
    QFile file;
    file.setFileName(m_jsonFileName);
    file.open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream stream(&file);
    stream << jsonDoc.toJson(QJsonDocument::Indented);
    file.close();
}

void MainWindow::updateTPList()
{
    m_pTPList->clear();
    for (auto e: m_pTPMap->keys())
    {
        QVector3D pos = (*m_pTPMap)[e];
        m_pTPList->addItem(new TpListItem(
                e + ":\t" + QString::number(pos.x()) + "\t" + QString::number(pos.y()) + "\t" +
                QString::number(pos.z()), e));
//        m_pTPList->addItem(e + ":\t" + QString::number(pos.x()) + "\t" + QString::number(pos.x()) + "\t" + QString::number(pos.x()));
    }
}

void MainWindow::onTimer()
{
    if (!m_pBns)
        return;
    QVector3D p = m_pBns->getCurrentPos();
    m_pXCoord->setText("X: " + QString::number(p.x()));
    m_pYCoord->setText("Y: " + QString::number(p.y()));
    m_pZCoord->setText("Z: " + QString::number(p.z()));
}

void MainWindow::onConnectButton(bool checked)
{
    Q_UNUSED(checked)
    if (!m_pBns)
        m_pBns = new BnSWrapper();
    m_pConnectButton->setEnabled(false);
}

void MainWindow::onTPSelected(QListWidgetItem *item)
{
    m_pSelectedTP = dynamic_cast<TpListItem *>(item);
    if(!m_pSelectedTP)
    {
        m_pCurrentSelectionLabel->setText("<b>Currently no TP selected</b>");
    }
    else
    {
        m_pCurrentSelectionLabel->setText("<b>" + m_pSelectedTP->getName() + "</b>");
    }
}

void MainWindow::onHelpButton(bool checked)
{
    QDialog dialog = QDialog(this);
    dialog.setModal(true);
    dialog.setMinimumSize(300, 200);
    QHBoxLayout layout = QHBoxLayout();
//    QTextBrowser text = QTextBrowser();
//    text.setText(m_helpText);
//    text.setOpenExternalLinks(true);
//    text.setAcceptRichText(true);
//    text.setReadOnly(true);
    QLabel text(m_helpText);
    text.setTextFormat(Qt::RichText);
    text.setWordWrap(true);
    connect(&text, &QLabel::linkActivated, [](const QString& link){
        QDesktopServices::openUrl(link);
    });
    layout.addWidget(&text);
    dialog.setLayout(&layout);
    dialog.exec();
}
