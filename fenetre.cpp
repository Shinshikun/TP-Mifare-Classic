#include "fenetre.h"
#include "ui_fenetre.h"
#include "ODALID.h"
#include <QtGui>
#include <QDebug>



Fenetre::Fenetre(QWidget *parent) : QMainWindow(parent) , ui(new Ui::Fenetre) {
    ui->setupUi(this);

}

Fenetre::~Fenetre() {
    delete ui;
}

char pszHost[]="192.168.1.4";
ReaderName MonLecteur;
uint8_t key_ff[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

void Fenetre::on_connect_clicked(){
    char version[30];
    uint8_t serial[4];
    char stackReader[20];

    //MonLecteur.Type=ReaderTCP;
    //strcpy(MonLecteur.IPReader,pszHost);
    MonLecteur.Type=ReaderCDC;
    MonLecteur.device=0;
    if(OpenCOM(&MonLecteur) != 0){
        ui->affichage->insertPlainText("Impossible de se connecter au lecteur.");
        ui->affichage->moveCursor(QTextCursor::End);
        return;
    }
    LEDBuzzer(&MonLecteur,BUZZER_OFF);
    LEDBuzzer(&MonLecteur,BUZZER_ON);
    Version(&MonLecteur, version, serial, stackReader);
    ui->affichage->insertPlainText(version);

    ui->affichage->moveCursor(QTextCursor::End);

    Mf_Classic_LoadKey(&MonLecteur, true, key_ff,0);
    ui->affichage->update();
    emit on_lire_clicked();
    affichageValeur();

    ui->connect->setDisabled(true);

}

void Fenetre::on_quitter_clicked(){
    int16_t status=0;
    RF_Power_Control(&MonLecteur,FALSE,0);
    status=LEDBuzzer(&MonLecteur,LED_OFF);
    status=CloseCOM(&MonLecteur);
    qApp->quit();
}

void Fenetre::on_saisie_clicked(){
    RF_Power_Control(&MonLecteur,TRUE, 0);

    BYTE atq[2];
    BYTE sak[1];
    BYTE uid[12];
    uint16_t uid_len = 12;

    ISO14443_3_A_PollCard(&MonLecteur, atq, sak, uid, &uid_len);

    QString Nom = ui->nom->toPlainText();
    QString Prenom = ui->prenom->toPlainText();
    if(Nom.length()<10) {
        Nom.append("\0");
    }
    if(Prenom.length()<10) {
        Prenom.append("\0");
    }
    QByteArray pre = Prenom.toLocal8Bit();
    unsigned char *prenom = (unsigned char *)strdup(pre.constData());
    QByteArray nom0 = Nom.toLocal8Bit();
    unsigned char *nom = (unsigned char *)strdup(nom0.constData());

    Mf_Classic_Write_Block(&MonLecteur, TRUE, 10, nom, Auth_KeyB, 2);
    Mf_Classic_Write_Block(&MonLecteur, TRUE, 9, prenom, Auth_KeyB, 2);
    LEDBuzzer(&MonLecteur,BUZZER_OFF);
    LEDBuzzer(&MonLecteur,BUZZER_ON);
}

void Fenetre::on_lire_clicked(){
    RF_Power_Control(&MonLecteur,TRUE, 0);
    BYTE atq[2];
    BYTE sak[1];
    BYTE uid[12];
    uint16_t uid_len = 12;

    QString *nomPrenom = new QString("");
    ISO14443_3_A_PollCard(&MonLecteur, atq, sak, uid, &uid_len);
    unsigned char nom[10];
    unsigned char prenom[10];

    Mf_Classic_Read_Block(&MonLecteur, TRUE, 10, nom, Auth_KeyA, 2);
    for(int i=0; i<10; i++) {
        if(nom[i] == '\0') {
            break;
        }
        nomPrenom->append(nom[i]);
    }

    nomPrenom->append(" ");

    Mf_Classic_Read_Block(&MonLecteur, TRUE, 9, prenom, Auth_KeyA, 2);
    for(int i=0; i<10; i++) {
        if(prenom[i] == '\0') {
            break;
        }
        nomPrenom->append(prenom[i]);
    }

    ui->affichageNoms->setText(*nomPrenom);
    LEDBuzzer(&MonLecteur,BUZZER_OFF);
    LEDBuzzer(&MonLecteur,BUZZER_ON);


}

void Fenetre::on_increment_clicked(){
    RF_Power_Control(&MonLecteur,TRUE, 0);
    BYTE atq[2];
    BYTE sak[1];
    BYTE uid[12];
    uint16_t uid_len = 12;

    ISO14443_3_A_PollCard(&MonLecteur, atq, sak, uid, &uid_len);
    backupValeur();
    Mf_Classic_Increment_Value(&MonLecteur, TRUE, 14, ui->getValeur->value(), 14, Auth_KeyB, 3);
    affichageValeur();
    LEDBuzzer(&MonLecteur,BUZZER_OFF);
    LEDBuzzer(&MonLecteur,BUZZER_ON);

}

void Fenetre::on_decrement_clicked(){
    RF_Power_Control(&MonLecteur,TRUE, 0);
    BYTE atq[2];
    BYTE sak[1];
    BYTE uid[12];
    uint16_t uid_len = 12;

    ISO14443_3_A_PollCard(&MonLecteur, atq, sak, uid, &uid_len);
    backupValeur();
    Mf_Classic_Decrement_Value(&MonLecteur, TRUE, 14, ui->getValeur->value(), 14, Auth_KeyB, 3);
    affichageValeur();
    LEDBuzzer(&MonLecteur,BUZZER_OFF);
    LEDBuzzer(&MonLecteur,BUZZER_ON);


}

void Fenetre::affichageValeur() {
    uint32_t val = 0;
    Mf_Classic_Read_Value(&MonLecteur, TRUE, 14, &val, Auth_KeyA, 3);
    QString valWrite = "";
    valWrite = QString::number((int) val);
    ui->affichageVal->setText("Valeur de la carte:" + valWrite);
}

void Fenetre::backupValeur() {
    RF_Power_Control(&MonLecteur,TRUE, 0);
    BYTE atq[2];
    BYTE sak[1];
    BYTE uid[12];
    uint16_t uid_len = 12;

    ISO14443_3_A_PollCard(&MonLecteur, atq, sak, uid, &uid_len);

    uint32_t val = 0;
    Mf_Classic_Read_Value(&MonLecteur, TRUE, 14, &val, Auth_KeyA, 3);
    Mf_Classic_Decrement_Value(&MonLecteur, TRUE, 13, 0, 13, Auth_KeyB, 3);

}





