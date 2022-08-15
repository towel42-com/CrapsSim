#include "DiceRoller.h"
#include "ui_DiceRoller.h"
#include <QSettings>

QString kDie1{ "Die1" };
QString kDie2( "Die2" );

CDiceRoller::CDiceRoller(QWidget *parent)
    : QDialog(parent, Qt::MSWindowsFixedSizeDialogHint),
    fImpl( new Ui::CDiceRoller )
{
    fImpl->setupUi(this);
    QSettings settings;
    fImpl->die1->setValue( settings.value( kDie1, 1 ).toInt() );
    fImpl->die2->setValue( settings.value( kDie2, 1 ).toInt() );
}

CDiceRoller::~CDiceRoller()
{
    QSettings settings;
    settings.setValue( kDie1, fImpl->die1->value() );
    settings.setValue( kDie2, fImpl->die2->value() );
}

int CDiceRoller::die1() const
{
    return fImpl->die1->value();
}

int CDiceRoller::die2() const
{
    return fImpl->die2->value();
}
