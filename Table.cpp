#include "Table.h"
#include "ui_Table.h"


SPlayInfo::SPlayInfo( const std::pair< int, int > & dice, int * currentDollars, bool logRoll ) :
    fLogRoll( logRoll ),
    fCurrentDollars( currentDollars ),
    fDice( dice )
{

}

CTable::CTable(QWidget *parent)
    : QWidget(parent, Qt::MSWindowsFixedSizeDialogHint),
    fImpl( new Ui::CTable )
{
    fImpl->setupUi(this);
    initValues();
    setWindowIcon( QIcon( ":/resources/craps.png" ) );

    connect( fImpl->start, &QToolButton::clicked, this, &CTable::sigStart );
    connect( fImpl->stop, &QToolButton::clicked, this, &CTable::sigStop );
    QImage image( ":/resources/table.png" );
    QPalette pal;
    pal.setBrush( fImpl->tableWidget->backgroundRole(), QBrush( image ) );
    fImpl->tableWidget->setPalette( pal );
}

void CTable::slotRunning( bool running )
{
    fImpl->start->setEnabled( !running );
    fImpl->stop->setEnabled( running );
}

void CTable::initValues()
{
    if ( fDiceImages.empty() )
    {
        fDiceImages = {
            QPixmap( ":/resources/dice1.png" ).scaled( QSize( 20, 20), Qt::KeepAspectRatio ),
            QPixmap( ":/resources/dice2.png" ).scaled( QSize( 20, 20), Qt::KeepAspectRatio ),
            QPixmap( ":/resources/dice3.png" ).scaled( QSize( 20, 20), Qt::KeepAspectRatio ),
            QPixmap( ":/resources/dice4.png" ).scaled( QSize( 20, 20), Qt::KeepAspectRatio ),
            QPixmap( ":/resources/dice5.png" ).scaled( QSize( 20, 20), Qt::KeepAspectRatio ),
            QPixmap( ":/resources/dice6.png" ).scaled( QSize( 20, 20), Qt::KeepAspectRatio ),
            QPixmap( ":/resources/off.png" ).scaled( QSize( 20, 20), Qt::KeepAspectRatio ),
            QPixmap( ":/resources/on.png" ).scaled( QSize( 20, 20), Qt::KeepAspectRatio ),
        };
    }
    fDice = { { 0, fImpl->die1 }, { 0, fImpl->die2 } };
    clearTable();
}

int CTable::amountOnTable( bool passBets, bool dontPassBets ) const
{
    int retVal = 
        fFourBet.first +
        fFiveBet.first +
        fSixBet.first +
        fEightBet.first +
        fNineBet.first +
        fTenBet.first;
    if ( passBets )
        retVal += fPassBet.first + fPassOddsBet.first;
    if ( dontPassBets )
        retVal += fDontPassBet.first + fDontPassOddsBet.first;
    return retVal;
}


void CTable::clearTable()
{
    fFourBet = { 0, fImpl->fourBet };
    fFiveBet = { 0, fImpl->fiveBet };
    fSixBet = { 0, fImpl->sixBet };
    fEightBet = { 0, fImpl->eightBet };
    fNineBet = { 0, fImpl->nineBet };
    fTenBet = { 0, fImpl->tenBet };
    fPassBet = { 0, fImpl->passBet };
    fPassOddsBet = { 0, fImpl->passOddsBet };
    fDontPassBet = { 0, fImpl->dontPassBet };
    fDontPassOddsBet = { 0, fImpl->dontPassOddsBet };
}

CTable::~CTable()
{
}


void CTable::setValue( const TBetType & bet )
{
    if ( !bet.second )
        return;
    bet.second->setText( tr( "%1" ).arg( bet.first ? QString::number( bet.first ) : QString() ) );
}

void CTable::refresh()
{
    setDie( fDice.first );
    setDie( fDice.second );

    setValue( fFourBet );
    setValue( fFiveBet );
    setValue( fSixBet );
    setValue( fEightBet );
    setValue( fNineBet );
    setValue( fTenBet );
    setValue( fPassBet );
    setValue( fPassOddsBet );
    setValue( fDontPassBet );
    setValue( fDontPassOddsBet );

    fImpl->tableAmount->setText( tr( "Amount on Table: %1" ).arg( amountOnTable( true, true ) ) );

    fImpl->fourPoint->setPixmap( QPixmap() );
    fImpl->fivePoint->setPixmap( QPixmap() );
    fImpl->sixPoint->setPixmap( QPixmap() );
    fImpl->eightPoint->setPixmap( QPixmap() );
    fImpl->ninePoint->setPixmap( QPixmap() );
    fImpl->tenPoint->setPixmap( QPixmap() );
    fImpl->offPoint->setPixmap( QPixmap() );
    
    switch( fPoint )
    {
        case EPoint::eNotSet:
            fImpl->offPoint->setPixmap( fDiceImages[ 6 ] );
            break;
        case EPoint::eFour:
            fImpl->fourPoint->setPixmap( fDiceImages[ 7 ] );
            break;
        case EPoint::eFive:
            fImpl->fivePoint->setPixmap( fDiceImages[ 7 ] );
            break;
        case EPoint::eSix:
            fImpl->sixPoint->setPixmap( fDiceImages[ 7 ] );
            break;
        case EPoint::eEight:
            fImpl->eightPoint->setPixmap( fDiceImages[ 7 ] );
            break;
        case EPoint::eNine:
            fImpl->ninePoint->setPixmap( fDiceImages[ 7 ] );
            break;
        case EPoint::eTen:
            fImpl->tenPoint->setPixmap( fDiceImages[ 7 ] );
            break;
    }
}

void CTable::setDie( const std::pair< int, QLabel * > & die )
{
    if( die.first >= 1 && die.first <= 6 )
        die.second->setPixmap( fDiceImages[ die.first - 1 ] );
    else
        die.second->setPixmap( QPixmap() );
}

void CTable::clear()
{
    initValues();
    setPoint( EPoint::eNotSet );
}

void CTable::setPoint( EPoint point )
{
    if( point != fPoint )
    {
        fPoint = point;
        if( fPlayInfo && fPlayInfo->fLogRoll )
        {
            fPlayInfo->fLogData += tr( "        New Point Set: %1\n" ).arg( getCurrentPoint() );
        }
    }
}


void CTable::setPoint()
{
    switch( fPlayInfo->total() )
    {
        case 4:
            setPoint( EPoint::eFour ); break;
        case 5:
            setPoint( EPoint::eFive ); break;
        case 6:
            setPoint( EPoint::eSix ); break;
        case 8:
            setPoint( EPoint::eEight ); break;
        case 9:
            setPoint( EPoint::eNine ); break;
        case 10:
            setPoint( EPoint::eTen ); break;
        default:
            setPoint( EPoint::eNotSet ); break;
            break;
    }
}

void CTable::run( SPlayInfo * playInfo )
{
    fPlayInfo = playInfo;
    if ( fPlayInfo->fLogRoll )
    {
        fPlayInfo->fLogData.clear();
        fPlayInfo->fLogData = tr( "Roll #: %1\n" )
            .arg( fPlayInfo->fNumRolls );
        fPlayInfo->fLogData += tr( "Dice: %2-%3(%4) Starting Bankroll Cash: %5 Total Bets on Table: %6 - Current Point: %7\n" )
            .arg( fPlayInfo->fDice.first ).arg( fPlayInfo->fDice.second ).arg( fPlayInfo->total() )
            .arg( *fPlayInfo->fCurrentDollars ).arg( amountOnTable( true, true ) ).arg( getCurrentPoint() );
    }
    bet();
    setDice();
    collect();
    if ( fPlayInfo->fLogRoll )
    {
        fPlayInfo->fLogData += tr( "        End Bankroll Cash: %1 Total Bets on Table: %2 - Current Point: %3\n" )
            .arg( *fPlayInfo->fCurrentDollars ).arg( amountOnTable( true, true ) ).arg(getCurrentPoint() );
    }
    fPlayInfo = nullptr;
}

void CTable::bet()
{
    if( fPoint == EPoint::eNotSet )
    {
        // point not set
        if ( fPlayInfo->fPlayPass )
        {
            if( fPassBet.first == 0 )
            {
                fPassBet.first = fPlayInfo->bet( fPlayInfo->fBaseBet );
                if ( fPlayInfo->fLogRoll )
                    fPlayInfo->fLogData += tr( "        Pass Line Bet: %2\n" ).arg( fPassBet.first );
            }
        }
        if( fPlayInfo->fPlayDontPass )
        {
            if( fDontPassBet.first == 0 )
            {
                fDontPassBet.first = fPlayInfo->bet( fPlayInfo->fBaseBet );
                if ( fPlayInfo->fLogRoll )
                    fPlayInfo->fLogData += tr( "        Don't Pass Line Bet: %2\n" ).arg( fDontPassBet.first );
            }
        }
    }
    else // point set
    {
        if ( fPlayInfo->fPlayPass )
        {
            if ( fPassOddsBet.first == 0 )
            {
                fPassOddsBet.first = fPlayInfo->bet( computeOddsBet() );
                if ( fPlayInfo->fLogRoll )
                    fPlayInfo->fLogData += tr( "        Pass Line Odds Bet: %2\n" ).arg( fPassOddsBet.first );
            }
        }
        if( fPlayInfo->fPlayDontPass )
        {
            if( fDontPassOddsBet.first == 0 )
            {
                fDontPassOddsBet.first = fPlayInfo->bet( computeOddsBet() );
                if ( fPlayInfo->fLogRoll )
                    fPlayInfo->fLogData += tr( "        Don't Pass Line Odds Bet: %2\n" ).arg( fPassOddsBet.first );
            }
        }

        if ( fPlayInfo->fPlayBoard )
        {
            placeBoardBet( fFourBet, EPoint::eFour );
            placeBoardBet( fTenBet, EPoint::eTen );
        }
        if ( fPlayInfo->fPlayBoard || fPlayInfo->fPlayInsideNumbers )
        {
            placeBoardBet( fFiveBet, EPoint::eFive );
            placeBoardBet( fNineBet, EPoint::eNine );
        }
        if ( ( fPlayInfo->fPlayBoard || fPlayInfo->fPlayInsideNumbers ) || fPlayInfo->fPlayInnerNumbers )
        {
            placeBoardBet( fSixBet, EPoint::eSix );
            placeBoardBet( fEightBet, EPoint::eEight );
        }
    }
}

int CTable::computeBoardBet( EPoint which ) const
{
    int baseBet = fPlayInfo->fBaseBet;
    if ( baseBet == 0 )
        return 0;

    int num = 0;
    int den = 1;
    switch ( which )
    {
        case EPoint::eNotSet:
            return 0;
            break;
        case EPoint::eFour:
        case EPoint::eTen:
            num = 1;
            den = 1;
            break;

        case EPoint::eFive:
        case EPoint::eNine:
            num = 1;
            den = 1;
            break;

        case EPoint::eSix:
        case EPoint::eEight:
            num = 6;
            den = 5;
            break;
    }
    return num*baseBet/den;
}

void CTable::placeBoardBet( TBetType & bet, EPoint which )
{
    if ( fPoint == which )
    {
        if ( bet.first != 0 )
        {
            int amt = fPlayInfo->collect( bet.first );
            bet.first = 0;
            if( fPlayInfo->fLogRoll )
                fPlayInfo->fLogData += tr( "        Removing Board Bet on %1: %2\n" ).arg( getPointString( which ) ).arg( amt );
        }
    }
    else
    {
        if( bet.first == 0 )
        {
            bet.first = fPlayInfo->bet( computeBoardBet( which ) );
            if( fPlayInfo->fLogRoll )
                fPlayInfo->fLogData += tr( "        Placing Board Bet on %1: %2\n" ).arg( getPointString( which ) ).arg( bet.first );
        }
    }
}

int CTable::computeOddsBet() const
{
    int base = fPlayInfo->fBaseBet;
    if ( base == 0 )
        return 0;

    int oddsBet = fPlayInfo->fOddsMultiplierBet;
    if ( oddsBet == 0 )
        return 0;

    return oddsBet*base;
}

void CTable::setDice()
{
    fDice.first.first = fPlayInfo->fDice.first;
    fDice.second.first = fPlayInfo->fDice.second;
}


void CTable::collect()
{
    switch ( fPoint )
    {
        case EPoint::eNotSet:
            handleNoPointRoll();
            break;
        case EPoint::eFour:
        case EPoint::eFive:
        case EPoint::eSix:
        case EPoint::eEight:
        case EPoint::eNine:
        case EPoint::eTen:
            handlePointRoll();
            break;
        default:
            break;
    }

}

void CTable::handleNoPointRoll()
{
    if( fPlayInfo->fPlayPass )
    {
        switch( fPlayInfo->total() )
        {
            case 7:
            case 11:
                fPlayInfo->collect( fPassBet.first );
                if( fPlayInfo->fLogRoll )
                    fPlayInfo->fLogData += tr( "        Pass Bet Winner: %1\n" ).arg( fPassBet.first );
                break;
            case 2:
            case 3:
            case 12:
                fPlayInfo->lose( fPassBet.first );
                if( fPlayInfo->fLogRoll )
                    fPlayInfo->fLogData += tr( "        Pass Bet Loser: %1\n" ).arg( fPassBet.first );
                break;
            default:
                break;
        }
    }
    if( fPlayInfo->fPlayDontPass )
    {
        switch( fPlayInfo->total() )
        {
            case 2:
            case 3:
                fPlayInfo->collect( fDontPassBet.first );
                *fPlayInfo->fCurrentDollars += fPlayInfo->fBaseBet;
                if( fPlayInfo->fLogRoll )
                    fPlayInfo->fLogData += tr( "        Don't Pass Bet Winner: %1\n" ).arg( fDontPassBet.first );
                break;
            case 7:
            case 11:
                fPlayInfo->lose( fDontPassBet.first );
                if( fPlayInfo->fLogRoll )
                    fPlayInfo->fLogData += tr( "        Don't Pass Bet Loser: %1\n" ).arg( fDontPassBet.first );
                break;
            default:
                break;
        }
    }

    setPoint();
}

void CTable::handlePointRoll()
{
    bool pointMade = false;
    switch( fPoint )
    {
        case EPoint::eNotSet:
            break;
        case EPoint::eFour:
            pointMade = fPlayInfo->total() == 4;
            break;
        case EPoint::eFive:
            pointMade = fPlayInfo->total() == 5;
            break;
        case EPoint::eSix:
            pointMade = fPlayInfo->total() == 6;
            break;
        case EPoint::eEight:
            pointMade = fPlayInfo->total() == 8;
            break;
        case EPoint::eNine:
            pointMade = fPlayInfo->total() == 9;
            break;
        case EPoint::eTen:
            pointMade = fPlayInfo->total() == 10;
            break;
        default:
            break;
    }

    bool clearTable = false;
    if( fPlayInfo->fPlayPass )
    {
        switch( fPlayInfo->total() )
        {
            case 7:
            {
                int amtLost = amountOnTable( true, false );
                clearTable = true;
                if( fPlayInfo->fLogRoll )
                    fPlayInfo->fLogData += tr( "        Craps Loser: Amount Lost on Table: %1\n" ).arg( amtLost );
                break;
            }
            default:
            {
                if( pointMade )
                {
                    int amtWon = computePassLineWon() + computeOddsWon();
                    fPlayInfo->collect( amtWon );
                    if( fPlayInfo->fLogRoll )
                        fPlayInfo->fLogData += tr( "        Point Made Winner: %1\n" ).arg( amtWon );
                    fPlayInfo->collect( fPassOddsBet.first );
                    if( fPlayInfo->fLogRoll )
                        fPlayInfo->fLogData += tr( "        Pass Odds Removed: %1\n" ).arg( fPassOddsBet.first );
                    fPassOddsBet.first = 0;
                }
                else
                    fPlayInfo->collect( collectFromBoard() );
                break;
            }
        }
    }
    if( fPlayInfo->fPlayDontPass )
    {
        switch( fPlayInfo->total() )
        {
            case 7:
            {
                int amtWon = computeDontPassLineWon() + computeDontPassOddsWon();
                fPlayInfo->collect( amtWon );
                if( fPlayInfo->fLogRoll )
                    fPlayInfo->fLogData += tr( "        Craps Winner: %1" ).arg( amtWon );
                fPlayInfo->collect( fDontPassOddsBet.first );
                if( fPlayInfo->fLogRoll )
                    fPlayInfo->fLogData += tr( "        Don't Pass Odds Removed: %1\n" ).arg( fDontPassOddsBet.first );
                fDontPassOddsBet.first = 0;
                break;
            }
            default:
                break;
        }
    }

    if( pointMade || ( fPlayInfo->total() == 7 ) )
    {
        setPoint( EPoint::eNotSet );
    }

    if ( clearTable )
        this->clearTable();

}

int CTable::collectFromBoard()
{
    int num = 0;
    int den = 1;
    int amt = 0;
    switch ( fPlayInfo->total() )
    {
        case 4:
        case 10:
            num = 9;
            den = 5;
            amt = ( fPlayInfo->total() == 4 ) ? fFourBet.first : fTenBet.first;
            break;
        case 5:
        case 9:
            num = 7;
            den = 5;
            amt = ( fPlayInfo->total() == 5 ) ? fFiveBet.first : fNineBet.first;
            break;

        case 6:
        case 8:
            num = 7;
            den = 6;
            amt = ( fPlayInfo->total() == 6 ) ? fSixBet.first : fEightBet.first;
            break;
        default:
            return 0;
            break;
    }
    if ( amt != 0 )
    {
        amt = amt * num / den;
        if ( fPlayInfo->fLogRoll )
            fPlayInfo->fLogData += tr( "        Board Bet Win: %1\n" ).arg( amt );
    }
    return amt;
}

int CTable::computePassLineWon() const
{
    return fPassBet.first;
}

int CTable::computeOddsWon() const
{
    int oddsBet = fPassOddsBet.first;
    if ( oddsBet == 0 )
        return 0;

    int num = 0;
    int den = 1;
    switch ( fPoint )
    {
        case EPoint::eNotSet:
            return 0;
            break;
        case EPoint::eFour:
        case EPoint::eTen:
            num = 2;
            den = 1;
            break;

        case EPoint::eFive:
        case EPoint::eNine:
            num = 3;
            den = 2;
            break;

        case EPoint::eSix:
        case EPoint::eEight:
            num = 6;
            den = 5;
            break;
    }
    return static_cast< int >( std::ceil( 1.0*num*oddsBet/den ) );
}


int CTable::computeDontPassLineWon() const
{
    int base = fDontPassBet.first;
    return base;
}
           

int CTable::computeDontPassOddsWon() const
{
    int oddsBet = fDontPassOddsBet.first;
    if ( oddsBet == 0 )
        return 0;

    int num = 0;
    int den = 1;
    switch ( fPoint )
    {
        case EPoint::eNotSet:
            return 0;
            break;
        case EPoint::eFour:
        case EPoint::eTen:
            num = 1;
            den = 2;
            break;

        case EPoint::eFive:
        case EPoint::eNine:
            num = 2;
            den = 3;
            break;

        case EPoint::eSix:
        case EPoint::eEight:
            num = 5;
            den = 6;
            break;
    }
    return static_cast< int >( std::ceil( 1.0*num*oddsBet/den ) );
}

QString CTable::getPointString( EPoint point ) const
{
    switch( point )
    {
        case EPoint::eNotSet:
            return "Not Set";
        case EPoint::eFour:
            return "Four";
        case EPoint::eFive:
            return "Five";
        case EPoint::eSix:
            return "Six";
        case EPoint::eEight:
            return "Eight";
        case EPoint::eNine:
            return "Nine";
        case EPoint::eTen:
            return "Ten";
    }
    return QString();
}
