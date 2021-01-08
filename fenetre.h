#ifndef FENETRE_H
#define FENETRE_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class Fenetre; }
QT_END_NAMESPACE

class Fenetre : public QMainWindow
{
    Q_OBJECT

public:
    Fenetre(QWidget *parent = nullptr);
    ~Fenetre();

private slots:
    void on_connect_clicked();
    void on_quitter_clicked();
    void on_saisie_clicked();
    void on_lire_clicked();
    void on_increment_clicked();
    void on_decrement_clicked();

private:
    Ui::Fenetre *ui;
    void affichageValeur();
    void backupValeur();
};
#endif // FENETRE_H
