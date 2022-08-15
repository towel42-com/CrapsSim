#ifndef CDICE_H
#define CDICE_H

#include <QWidget>
#include <QDate>
#include <memory>
#include <random>
#include <chrono>
class QLabel;
class QVariant;
namespace Ui{ class CDice; }

typedef std::pair< int, QLabel * > TDieType;
typedef std::vector< TDieType > TDieTypeVector;

class CDice : public QWidget
{
    Q_OBJECT
public:
    CDice(QWidget *parent = 0);
    ~CDice();

    std::pair< int, int > roll( bool simulateDice, bool & aOK );
    void refresh();
    int numRolls() const{ return fNumRolls; }
    void clear();
public slots:
private:
    void initValues();
    void setValue( int value, const TDieType & die );
    QString valueString( const QVariant & value, int num, int total ) const;
    int loadTotals( const TDieTypeVector & die  );
    void loadTotals( const std::vector< std::pair< std::vector< int >, QLabel * > > & die );
    void addToTotal( int d1, int d2 );


    std::unique_ptr< Ui::CDice > fImpl;

    int fNumRolls{ 0 };

    TDieTypeVector fDie1;
    TDieTypeVector fDie2;
    std::vector< TDieTypeVector > fResults;
    std::vector< std::pair< std::vector< int >, QLabel * > > fTotals;

    std::random_device fRandomDevice;
    std::mt19937 fGenerator;
    std::uniform_int_distribution<> fDistribution;
};

#endif // ORDERPROCESSOR_H
