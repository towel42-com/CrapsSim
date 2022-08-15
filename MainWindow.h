#ifndef CMAINWINDOW_H
#define CMAINWINDOW_H

#include <QMainWindow>
#include <QDate>
#include <memory>
#include <random>
#include <chrono>
class QLabel;
class CDice;
class CTable;
namespace Ui{ class CMainWindow; }

typedef std::pair< int, QLabel * > TDieType;
typedef std::vector< TDieType > TDieTypeVector;

class CMainWindow : public QMainWindow
{
    Q_OBJECT
public:
    CMainWindow(QWidget *parent = 0);

    void loadSettings();

    void initValues();

    ~CMainWindow();

    void saveSettings();
signals:
    void sigRunning( bool running );
public slots:
    void slotStart();
    void slotStop();
    void slotRefresh();
    void slotRunning( bool running );
    void slotRoll();
    void slotClear();
    void slotViewAboutToShow();
    void slotSimulateDiceChanged();
private:
    void refresh( bool force );
    int loadTotals( const TDieTypeVector & die, bool addOne );
    void setValue( int value, const TDieType & die );
    void logResults();

    std::unique_ptr< Ui::CMainWindow > fImpl;
    std::unique_ptr< CDice > fDice;
    std::unique_ptr< CTable > fTable;

    bool fRunning{false};

    int fStopAtRolls{0};
    int fLastNumRolls{ 0 };
    std::chrono::time_point< std::chrono::high_resolution_clock > fLastUpdate;

  
    int fCurrentDollars{500};
    int fStartingDollars{ fCurrentDollars };
    bool fBeenRun{ false };
};

#endif // ORDERPROCESSOR_H
