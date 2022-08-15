#ifndef CTable_H
#define CTable_H

#include <QWidget>
#include <QDate>
#include <memory>
#include <random>
#include <chrono>
#include <QPixmap>
class QLabel;
namespace Ui{ class CTable; }


typedef std::pair< int, QLabel * > TBetType;

struct SPlayInfo
{
    SPlayInfo( const std::pair< int, int > & dice, int * currentDollars, bool logRoll );
    int total() const{ return fDice.first + fDice.second; }
    int bet( int amount ){ *fCurrentDollars -= amount; return amount; }
    int collect( int amount ){ *fCurrentDollars += amount; return amount; }
    int lose( int amount ){ *fCurrentDollars -= amount; return amount; }


    QString fLogData;
    bool fLogRoll;
    std::pair< int, int > fDice;
    int fNumRolls{ 0 };
    int * fCurrentDollars{ nullptr };

    int fBaseBet{ 0 };
    bool fPlayPass{ false };
    int fOddsMultiplierBet{ 0 };
    bool fPlayDontPass{ false };

    bool fPlayInnerNumbers{ false };
    bool fPlayInsideNumbers{ false };
    bool fPlayBoard{ false };
};

class CTable : public QWidget
{
    Q_OBJECT
public:
    CTable(QWidget *parent = 0);
    ~CTable();

    void refresh();
    void clear();
    void run( SPlayInfo * playInfo );


    enum class EPoint
    {
        eNotSet,
        eFour,
        eFive,
        eSix,
        eEight,
        eNine,
        eTen
    };

signals:
    void sigStart();
    void sigStop();

public slots:
    void slotRunning( bool running );
private:
    void setPoint( EPoint point );

    void bet();

    void setDice();
    void collect();
    int collectFromBoard();

    void initValues();

    void clearTable();
    int amountOnTable( bool passBets, bool dontPassBets ) const;

    void setValue( const TBetType & bet );
    void setDie( const std::pair< int, QLabel * > & die );

    std::unique_ptr< Ui::CTable > fImpl;

    std::pair< std::pair< int, QLabel * >, std::pair< int, QLabel * > > fDice;
    TBetType fFourBet;
    TBetType fFiveBet;
    TBetType fSixBet;
    TBetType fEightBet;
    TBetType fNineBet;
    TBetType fTenBet;
    TBetType fPassBet;
    TBetType fPassOddsBet;
    TBetType fDontPassBet;
    TBetType fDontPassOddsBet;

    void placeBoardBet( TBetType & bet, EPoint which );
    int computeBoardBet( EPoint which ) const;


    QString getCurrentPoint() const{ return getPointString( fPoint ); }
    int computePassLineWon() const;
    int computeOddsBet() const;
    int computeOddsWon() const;
    int computeDontPassLineWon() const;
    int computeDontPassOddsWon() const;

    void handleNoPointRoll();

    void setPoint();

    void handlePointRoll();

    QString getPointString( EPoint point ) const;
    EPoint fPoint{ EPoint::eNotSet };

    std::vector< QPixmap > fDiceImages;
    SPlayInfo * fPlayInfo{ nullptr };
};

#endif // ORDERPROCESSOR_H
