#include "Dice.h"
#include "ui_Dice.h"
#include "DiceRoller.h"

CDice::CDice(QWidget *parent)
    : QWidget(parent, Qt::MSWindowsFixedSizeDialogHint),
    fImpl( new Ui::CDice ),
    fNumRolls( 0 ),
    fRandomDevice(),
    fGenerator( fRandomDevice() ),
    fDistribution( 1, 6 )
{
    fImpl->setupUi(this);
    initValues();
    setWindowIcon( QIcon( ":/resources/craps.png" ) );
}

void CDice::initValues()
{
    fDie1 =
    {
        { 0, fImpl->numHit11tot },
        { 0, fImpl->numHit12tot },
        { 0, fImpl->numHit13tot },
        { 0, fImpl->numHit14tot },
        { 0, fImpl->numHit15tot },
        { 0, fImpl->numHit16tot }
    };

    fDie2 =
    {
        { 0, fImpl->numHit21tot },
        { 0, fImpl->numHit22tot },
        { 0, fImpl->numHit23tot },
        { 0, fImpl->numHit24tot },
        { 0, fImpl->numHit25tot },
        { 0, fImpl->numHit26tot }
    };

    fResults =
    {
        { { 0, fImpl->numHit11 }, { 0, fImpl->numHit12 }, { 0, fImpl->numHit13 }, { 0, fImpl->numHit14 }, { 0, fImpl->numHit15 }, { 0, fImpl->numHit16 } },
        { { 0, fImpl->numHit21 }, { 0, fImpl->numHit22 }, { 0, fImpl->numHit23 }, { 0, fImpl->numHit24 }, { 0, fImpl->numHit25 }, { 0, fImpl->numHit26 } },
        { { 0, fImpl->numHit31 }, { 0, fImpl->numHit32 }, { 0, fImpl->numHit33 }, { 0, fImpl->numHit34 }, { 0, fImpl->numHit35 }, { 0, fImpl->numHit36 } },
        { { 0, fImpl->numHit41 }, { 0, fImpl->numHit42 }, { 0, fImpl->numHit43 }, { 0, fImpl->numHit44 }, { 0, fImpl->numHit45 }, { 0, fImpl->numHit46 } },
        { { 0, fImpl->numHit51 }, { 0, fImpl->numHit52 }, { 0, fImpl->numHit53 }, { 0, fImpl->numHit54 }, { 0, fImpl->numHit55 }, { 0, fImpl->numHit56 } },
        { { 0, fImpl->numHit61 }, { 0, fImpl->numHit62 }, { 0, fImpl->numHit63 }, { 0, fImpl->numHit64 }, { 0, fImpl->numHit65 }, { 0, fImpl->numHit66 } },
    };

    fTotals = {
        { { 0 }, nullptr },           
        { { 0 }, nullptr },           
        { { 0 }, fImpl->total2 },                // 2 - 1 1
        { { 0, 0 }, fImpl->total3 },             // 3 - 1 2, 2 1
        { { 0, 0, 0 }, fImpl->total4 },          // 4 - 1 3, 2 2, 3 1
        { { 0, 0, 0, 0 }, fImpl->total5 },       // 5 - 1 4, 2 3, 3 2, 4 1
        { { 0, 0, 0, 0, 0 }, fImpl->total6 },    // 6 - 1 5, 2 4, 3 3, 4 2, 5 1
        { { 0, 0, 0, 0, 0, 0 }, fImpl->total7 }, // 7 - 1 6, 2 5, 3 4, 4 3, 5 2, 6 1
        { { 0, 0, 0, 0, 0 }, fImpl->total8 },    // 8 - 2 6, 3 5, 4 4, 5 3, 6 2
        { { 0, 0, 0, 0 }, fImpl->total9 },       // 9 - 3 6, 4 5, 5 4, 6 2
        { { 0, 0, 0 }, fImpl->total10 },         //10 - 4 6, 5 5, 6 4
        { { 0, 0 }, fImpl->total11 },            //11 - 5 6, 6 5
        { { 0 }, fImpl->total12 }                //12 - 6 6
    };
}

CDice::~CDice()
{
}

std::pair< int, int > CDice::roll( bool simulateDice, bool & aOK )
{
    int die1 = 0;
    int die2 = 0;
    if ( simulateDice )
    {
        die1 = fDistribution( fGenerator );
        die2 = fDistribution( fGenerator );
        aOK = true;
    }
    else
    {
        CDiceRoller dlg;
        if ( dlg.exec() != QDialog::Accepted )
        {
            aOK = false;
            return std::make_pair( 0, 0 );
        }
        die1 = dlg.die1();
        die2 = dlg.die2();
        aOK = true;
    }

    fDie1[ die1 - 1 ].first++;
    fDie2[ die2 - 1 ].first++;

    fResults[ die1 - 1 ][ die2 - 1 ].first++;
    addToTotal( die1, die2 );

    fNumRolls++;
    return std::make_pair( die1, die2 );
}


void CDice::addToTotal( int d1, int d2 )
{
    int total = d1+d2;
    if ( ( total < 2 ) || ( total > 12 ) )
        return;

    auto & values = fTotals[ total ].first;
    switch( total )
    {
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
        case 7:
            values[ d1 - 1 ]++;
            break;
        case 8:
            values[ d1 - 2 ]++;
            break;
        case 9:
            values[ d1 - 3 ]++;
            break;
        case 10:
            values[ d1 - 4 ]++;
            break;
        case 11:
            values[ d1 - 5 ]++;
            break;
        case 12:
            values[ d1 - 6 ]++;
            break;
    };
}

QString CDice::valueString( const QVariant & value, int num, int total ) const
{
    return tr( "%1 - %2(%3%)" ).arg( value.toString() ).arg( num ).arg( static_cast< int >( std::round( total ? ( num*100.0 / total ) : 0 ) ) );
}


void CDice::setValue( int value, const TDieType & die )
{
    if ( !die.second )
        return;
    die.second->setText( valueString( value, die.first, fNumRolls ) );
}

int CDice::loadTotals( const TDieTypeVector & die )
{
    int retVal = 0;
    for( size_t ii = 0; ii < die.size(); ++ii )
    {
        setValue( ii + 1, die[ ii ] );
        retVal += die[ ii ].first;
    }
    return retVal;
}

void CDice::loadTotals( const std::vector< std::pair< std::vector< int >, QLabel * > > & totals )
{
    for( size_t ii = 0; ii < totals.size(); ++ii )
    {
        if ( !totals[ ii ].second )
            continue;
        int total = std::accumulate( totals[ ii ].first.begin(), totals[ ii ].first.end(), 0 );
        QString label = valueString( ii, total, fNumRolls );
        if ( ( ii % 2 ) == 0 && ( ii != 2 ) && ( ii != 12 ) )
        {
            int midPoint = totals[ ii ].first.size() / 2;
            int numSoft = 0;
            int numHard = 0;
            for( size_t jj = 0; jj < totals[ ii ].first.size(); ++jj )
            {
                if ( jj == midPoint )
                    numHard = totals[ ii ].first[ jj ];
                else
                    numSoft += totals[ ii ].first[ jj ];
            }

            label += valueString( " Soft", numSoft, total );
            label += valueString( " Hard", numHard, total );
        }

        //if ( ( ii != 2 ) && ( ii != 12 ) && ( ii != 3 ) && ( ii != 11 ) && ( )
        //{
        //    for( size_t jj = 0; jj < totals[ ii ].first.size(); ++jj )
        //    {
        //        int die1 = 0;
        //        int die2 = 0;
        //        if( ii <= 7 )
        //        {
        //            die1 = 1 + jj;
        //            die2 = ii - die1;
        //        }
        //        else // total = 8->12 
        //        {
        //            die2 = 6 - jj;
        //            die1 = ii - die2;
        //        }

        //        int num = totals[ ii ].first[ jj ];
        //        label += tr( " %1-%2 %3(%4%)" ).arg( die1 ).arg( die2 ).arg( num ).arg( static_cast<int>( std::round( total ? ( num*100.0 / total ) : 0 ) ) );
        //    }
        //}
        totals[ ii ].second->setText( label );
    }
}

void CDice::refresh()
{
    int die1Total = loadTotals( fDie1 );
    int die2Total = loadTotals( fDie2 );
    loadTotals( fTotals );
    for( size_t die1 = 0; die1 < 6; die1++ )
    {
        for( size_t die2 = 0; die2 < 6; die2++ )
        {
            setValue( die1+die2 + 2, fResults[ die1 ][ die2 ] );
        }
    }
    fImpl->numHitTot->setText( tr( "%1, %2" ).arg( die1Total ).arg( die2Total ) );
}

void CDice::clear()
{
    fNumRolls = 0;
    initValues();
}

