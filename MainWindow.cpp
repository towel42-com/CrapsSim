#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "Dice.h"
#include "Table.h"

#include <QFileDialog>
#include <QSqlTableModel>
#include <QFile>
#include <QDir>
#include <QTextStream>
#include <QMessageBox>
#include <QStandardPaths>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>
#include <QSqlDriver>
#include <QDebug>
#include <QDate>
#include <QHeaderView>
#include <QIcon>
#include <QItemSelectionModel>
#include <QTimer>
#include <QTime>
#include <QSettings>

QString kRefreshRate{ "RefreshRate" };
QString kStopRolls{ "StopRolls" };
QString kBaseBet{ "BaseBet" };
QString kOddsBet{ "OddsBet" };
QString kPlayPass{ "PlayPass" };
QString kPlayDontPass{ "PlayDontPass" };
QString kLogRolls{ "LogRolls" };
QString kResetOnGo{ "ResetOnGo" };
QString kPlayInnerNumbers{ "PlayInnerNumbers" };
QString kPlayInsideNumbers{ "PlayInsideNumbers" };
QString kPlayBoard{ "PlayBoard" };
QString kSimulateDice( "SimulateDice" );


CMainWindow::CMainWindow(QWidget *parent)
    : QMainWindow(parent),
    fImpl( new Ui::CMainWindow ),
    fDice( new CDice( nullptr ) ),
    fTable( new CTable( nullptr ) )
{
    fImpl->setupUi(this);
    setWindowIcon( QIcon( ":/resources/craps.png" ) );

    connect( fImpl->start, SIGNAL( clicked() ), this, SLOT( slotStart() ) );
    connect( fImpl->stop, SIGNAL( clicked() ), this, SLOT( slotStop() ) );
    connect( fImpl->clear, SIGNAL( clicked() ), this, SLOT( slotClear() ) );
    connect( fImpl->refresh, SIGNAL( clicked() ), this, SLOT( slotRefresh() ) );
    
    connect( fImpl->actionStart, SIGNAL( triggered() ), this, SLOT( slotStart() ) );
    connect( fImpl->actionStop, SIGNAL( triggered() ), this, SLOT( slotStop() ) );
    connect( fImpl->actionClear, SIGNAL( triggered() ), this, SLOT( slotClear() ) );

    connect( fImpl->actionDice, SIGNAL( triggered( bool ) ), fDice.get(), SLOT( setVisible( bool ) ) );
    connect( fImpl->actionTable, SIGNAL( triggered( bool ) ), fTable.get(), SLOT( setVisible( bool ) ) );

    connect( fImpl->menuView, &QMenu::aboutToShow, this, &CMainWindow::slotViewAboutToShow );

    connect( fTable.get(), &CTable::sigStart, this, &CMainWindow::slotStart );
    connect( fTable.get(), &CTable::sigStop, this, &CMainWindow::slotStop );
    connect( this, &CMainWindow::sigRunning, fTable.get(), &CTable::slotRunning );

    connect( fImpl->simulateDice, &QToolButton::clicked, this, &CMainWindow::slotSimulateDiceChanged ) ;

    setAttribute(Qt::WA_DeleteOnClose);

    initValues();

    slotClear();

    loadSettings();

    QTimer::singleShot( 0, fDice.get(), SLOT( show() ) );
    QTimer::singleShot( 0, fTable.get(), SLOT( show() ) );
}

void CMainWindow::loadSettings()
{
    QSettings settings;
    fImpl->refreshRate->setValue( settings.value( kRefreshRate, 1000 ).toInt() );
    fImpl->stopRolls->setValue( settings.value( kStopRolls, 100000 ).toInt() );
    fImpl->baseBet->setValue( settings.value( kBaseBet, 5 ).toInt() );
    fImpl->oddsBet->setValue( settings.value( kOddsBet, 5 ).toInt() );
    fImpl->playPass->setChecked( settings.value( kPlayPass, true ).toBool() );
    fImpl->playDontPass->setChecked( settings.value( kPlayDontPass, false ).toBool() );
    fImpl->logRolls->setChecked( settings.value( kLogRolls, true ).toBool() );
    fImpl->resetOnGo->setChecked( settings.value( kResetOnGo, true ).toBool() );
    fImpl->playInnerNumbers->setChecked( settings.value( kPlayInnerNumbers, false ).toBool() );
    fImpl->playInsideNumbers->setChecked( settings.value( kPlayInsideNumbers, false ).toBool() );
    fImpl->playBoard->setChecked( settings.value( kPlayBoard, false ).toBool() );   
    fImpl->simulateDice->setChecked( settings.value( kSimulateDice, true ).toBool() );   
    slotSimulateDiceChanged();
}

void CMainWindow::initValues()
{
}

CMainWindow::~CMainWindow()
{
    saveSettings();
}

void CMainWindow::saveSettings()
{
    QSettings settings;
    settings.setValue( kRefreshRate, fImpl->refreshRate->value() );
    settings.setValue( kStopRolls, fImpl->stopRolls->value() );
    settings.setValue( kBaseBet, fImpl->baseBet->value() );
    settings.setValue( kOddsBet, fImpl->oddsBet->value() );
    settings.setValue( kPlayPass, fImpl->playPass->isChecked() );
    settings.setValue( kPlayDontPass, fImpl->playDontPass->isChecked() );
    settings.setValue( kLogRolls, fImpl->logRolls->isChecked() );
    settings.setValue( kResetOnGo, fImpl->resetOnGo->isChecked() );

    settings.setValue( kPlayInsideNumbers, fImpl->playInsideNumbers->isChecked() );
    settings.setValue( kPlayInnerNumbers, fImpl->playInnerNumbers->isChecked() );
    settings.setValue( kPlayBoard, fImpl->playBoard->isChecked() );
    settings.setValue( kSimulateDice, fImpl->simulateDice->isChecked() );
}

void CMainWindow::slotSimulateDiceChanged()
{
    fImpl->refreshRate->setEnabled( fImpl->simulateDice->isChecked() );
    fImpl->stopRolls->setEnabled( fImpl->simulateDice->isChecked() );
}

void CMainWindow::slotRoll()
{
    if ( fDice->numRolls() >= fStopAtRolls )
    {
        logResults();
        slotRunning( false );
    }
    if ( !fRunning )
        return;
    bool aOK = false;
    auto dice = fDice->roll( fImpl->simulateDice->isChecked(), aOK );

    if (!aOK )
    {
        slotRunning( false );
        return;
    }
    
    SPlayInfo playInfo( dice, &fCurrentDollars, fImpl->logRolls->isChecked() );
    playInfo.fNumRolls = fDice->numRolls();
    playInfo.fBaseBet = fImpl->baseBet->value();
    playInfo.fOddsMultiplierBet = fImpl->oddsBet->value();

    playInfo.fPlayPass = fImpl->playPass->isChecked();
    playInfo.fPlayDontPass = fImpl->playDontPass->isChecked();
    playInfo.fPlayInnerNumbers = fImpl->playInnerNumbers->isChecked();
    playInfo.fPlayInsideNumbers = fImpl->playInsideNumbers->isChecked();
    playInfo.fPlayBoard = fImpl->playBoard->isChecked();
    
    fTable->run( &playInfo );

    if ( fImpl->logRolls->isChecked() )
        fImpl->log->appendPlainText( playInfo.fLogData );
    refresh( false );
    QTimer::singleShot( 0, this, SLOT( slotRoll() ) );
}

void CMainWindow::logResults()
{
    if ( fImpl->logRolls->isChecked() )
        return;

    QString logData = tr( 
        "======================\n"
        "%1\n"
        "Base Bet: %2\n"
        "Odds Bet (multiplier): %3\n"
        "Play Pass Line: %4\n"
        "Play Don't Pass Line: %5\n"
        "Play Inner Numbers: %6\n"
        "Play Inside Numbers: %7\n"
        "Play the Board: %8\n"
        "Number of Rolls: %9\n"
        "Net Winnings: %7" )
        .arg( QTime::currentTime().toString() )
        .arg( fImpl->baseBet->value() )
        .arg( fImpl->oddsBet->value() )
        .arg( fImpl->playPass->isChecked() ? "Yes" : "No" )
        .arg( fImpl->playDontPass->isChecked() ? "Yes" : "No" )
        .arg( fDice->numRolls() )
        .arg( fCurrentDollars - fStartingDollars )
        ;

    fImpl->log->appendPlainText( logData );
}

void CMainWindow::slotStart()
{
    if ( fImpl->resetOnGo->isChecked() )
        slotClear();
    if ( fImpl->simulateDice->isChecked() )
        fStopAtRolls = fDice->numRolls() + fImpl->stopRolls->value();
    else
        fStopAtRolls = fDice->numRolls() + 1;

    slotRunning( true );
    QTimer::singleShot( 0, this, SLOT( slotRoll() ) );
}

void CMainWindow::slotStop()
{
    slotRunning( false );
}

void CMainWindow::slotClear()
{
    slotRunning( false );

    fImpl->log->clear();
    fDice->clear();
    fTable->clear();
    fLastNumRolls = 0;
    fCurrentDollars = 500;
    fStartingDollars = fCurrentDollars;

    fBeenRun = true;
    fLastUpdate = std::chrono::time_point< std::chrono::high_resolution_clock >();
    refresh( true );
}

void CMainWindow::slotViewAboutToShow()
{
    fImpl->actionDice->setChecked( fDice->isVisible() );
    fImpl->actionTable->setChecked( fTable->isVisible() );
}

void CMainWindow::setValue( int value, const TDieType & die )
{
    if ( !die.second )
        return;
    die.second->setText( tr( "%1 - %2(%3%)" ).arg( value ).arg( die.first ).arg( ( fDice->numRolls() ? ( die.first*100.0 / fDice->numRolls() ) : 0 ), 0, 'f', 2 ) );
}

int CMainWindow::loadTotals( const TDieTypeVector & die, bool addOne )
{
    int retVal = 0;
    for( size_t ii = 0; ii < die.size(); ++ii )
    {
        setValue( ii + ( addOne ? 1 : 0 ), die[ ii ] );
        retVal += die[ ii ].first;
    }
    return retVal;
}

void CMainWindow::slotRefresh()
{
    refresh( true );
}

void CMainWindow::refresh( bool force )
{
    if ( !force && ( fDice->numRolls() % fImpl->refreshRate->value() ) != 0 )
        return;

    fDice->refresh();
    fTable->refresh();
    fImpl->rollsLabel->setText( tr( "Number of Rolls: %1" ).arg( fDice->numRolls() ) );

    fImpl->currentCash->setText( tr( "Current Cash Level: %1 Net Profit: %2" ).arg( fCurrentDollars ).arg( fCurrentDollars - fStartingDollars ) );
    double rollsPerSec = 0;
    if ( fBeenRun )
    {
        auto numRolls = fDice->numRolls() - fLastNumRolls;

        auto now = std::chrono::high_resolution_clock::now();
        auto elapsed = std::chrono::duration_cast< std::chrono::microseconds >( now - fLastUpdate );
        rollsPerSec = numRolls*1000000.0 / elapsed.count();
    }
    fBeenRun = true;
    if ( rollsPerSec != 0.0 )
        fImpl->rollsPerSec->setText( tr( "Rolls/Sec: %1" ).arg( rollsPerSec, 0, 'f', 1 ) );
    fLastNumRolls = fDice->numRolls();
    fLastUpdate = std::chrono::high_resolution_clock::now();
}

void CMainWindow::slotRunning( bool running )
{
    fRunning = running;
    fImpl->start->setEnabled( !running );
    fImpl->stop->setEnabled( running );
    fImpl->refresh->setEnabled( !running );
    fImpl->clear->setEnabled( !running );
    fImpl->playStyle->setEnabled( !running );
    fImpl->logRolls->setEnabled( !running );
    fImpl->simulateDice->setEnabled( !running );
    fImpl->refreshRate->setEnabled( !running );
    fImpl->stopRolls->setEnabled( !running );
    fImpl->resetOnGo->setEnabled( !running );
    if ( !running )
        refresh( true );
    emit sigRunning( running );
}

