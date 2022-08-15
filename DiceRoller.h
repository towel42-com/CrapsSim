#ifndef CDiceRoller_H
#define CDiceRoller_H

#include <QDialog>
#include <QDate>
#include <memory>
#include <random>
#include <chrono>
class QLabel;
namespace Ui{ class CDiceRoller; }

typedef std::pair< int, QLabel * > TDieType;
typedef std::vector< TDieType > TDieTypeVector;

class CDiceRoller : public QDialog
{
    Q_OBJECT
public:
    CDiceRoller(QWidget *parent = 0);
    ~CDiceRoller();

    int die1() const;
    int die2() const;
private:
    std::unique_ptr< Ui::CDiceRoller > fImpl;
};

#endif // ORDERPROCESSOR_H
