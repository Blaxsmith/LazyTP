//
// Created by Staz on 25/06/2022.
//

#ifndef LAZYTP_MAINWINDOW_H
#define LAZYTP_MAINWINDOW_H

#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QListWidget>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QIntValidator>
#include <QMainWindow>
#include <QVector3D>
#include <QMap>
#include <QTimer>
#include "TpListItem.h"
#include "KeyboardHook.h"
#include "BnSWrapper.h"


class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = Q_NULLPTR);
    virtual ~MainWindow();

    void loadJson();
    void saveJson();
    void updateTPList();

private slots:
    void onKeyPressed();
    void onAddButton(bool checked = false);
    void onRemoveButton(bool checked = false);
    void onConnectButton(bool checked = false);
    void onHelpButton(bool checked = false);
    void onTimer();
    void onTPSelected(QListWidgetItem* item);

private:
    TpListItem* m_pSelectedTP;
    QMap<QString, QVector3D>* m_pTPMap;
    QString m_helpText;

    QString m_jsonFileName;

    KeyboardHook* m_pHook;
    BnSWrapper* m_pBns;

    QWidget* m_pMainWidget;
    QVBoxLayout* m_pMainLayout;
    QHBoxLayout* m_pCoordLayout;
    QHBoxLayout* m_pListWidgetLayout;
    QHBoxLayout* m_pCoordsInputLayout;
    QHBoxLayout* m_pButtonsLayout;
    QHBoxLayout* m_pCurrentSelectionLayout;

    QPushButton* m_pConnectButton;
    QPushButton* m_pHelpButton;

    QLabel* m_pXCoord;
    QLabel* m_pYCoord;
    QLabel* m_pZCoord;

    QLabel* m_pCurrentSelectionLabel;

    QListWidget* m_pTPList;

    QLabel* m_pCoordInputLabel;
    QLabel* m_pXInputLabel;
    QLabel* m_pYInputLabel;
    QLabel* m_pZInputLabel;
    QLineEdit* m_pCoordInput;
    QLineEdit* m_pXInput;
    QLineEdit* m_pYInput;
    QLineEdit* m_pZInput;

    QPushButton* m_pAddButton;
    QPushButton* m_pRemoveButton;

    QIntValidator* m_pIntValidator;

    QTimer* m_pTimer;

    int m_keyCode;
};


#endif //LAZYTP_MAINWINDOW_H
